#!/usr/bin/env python3
"""
Progress database and reporting for the MTG/Shandalar matching-decompilation project.

The database is the project's source of truth for "what is done". Ghidra inventories
seed it; humans and agents annotate it; the verifier updates match status mechanically.

Subcommands
-----------
  init      create the schema
  scan      (re)seed functions from reports/ghidra/*.functions.csv  (idempotent)
  set       change status/tu/name for one or more functions
  import    bulk-apply a JSON annotation file
  report    write reports/progress.json + reports/PROGRESS.md
  dash      write reports/dashboard.html
  stats     print a summary to stdout

Status ladder (a function may only ever move forward; `set --force` overrides):
  unstarted   nothing done
  identified  purpose known, named, signature known
  decompiled  C written that compiles
  equivalent  compiles and is behaviourally equivalent, but bytes differ
  matching    compiles to byte-identical code (the goal)
  library     CRT/compiler-helper code -- matched by linking, not by hand
  nonmatching explicitly accepted as permanently non-matching (needs a reason)
"""

import argparse
import csv
import json
import os
import sqlite3
import sys
import time
from datetime import datetime, timezone

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
DB = os.path.join(ROOT, "progress", "progress.db")
GHIDRA = os.path.join(ROOT, "reports", "ghidra")
REPORTS = os.path.join(ROOT, "reports")

STATUSES = ["unstarted", "identified", "decompiled", "equivalent",
            "matching", "library", "nonmatching"]
# Statuses that count as "done" for the headline metric.
DONE = {"matching", "library"}
RANK = {s: i for i, s in enumerate(STATUSES)}

SCHEMA = """
CREATE TABLE IF NOT EXISTS binary (
    name        TEXT PRIMARY KEY,
    sha256      TEXT,
    size        INTEGER,
    linker      TEXT,
    notes       TEXT
);
CREATE TABLE IF NOT EXISTS func (
    binary      TEXT NOT NULL,
    addr        TEXT NOT NULL,           -- hex, no 0x, as Ghidra emits
    size        INTEGER NOT NULL,
    section     TEXT,
    orig_name   TEXT,                    -- Ghidra's auto name
    name        TEXT,                    -- our name once identified
    tu          TEXT,                    -- translation unit, e.g. sidlib/lib.c
    subsystem   TEXT,
    status      TEXT NOT NULL DEFAULT 'unstarted',
    num_blocks  INTEGER,
    num_callers INTEGER,
    num_callees INTEGER,
    notes       TEXT,
    updated     TEXT,
    PRIMARY KEY (binary, addr)
);
CREATE INDEX IF NOT EXISTS idx_func_status ON func(status);
CREATE INDEX IF NOT EXISTS idx_func_tu     ON func(tu);
CREATE INDEX IF NOT EXISTS idx_func_sub    ON func(subsystem);
CREATE TABLE IF NOT EXISTS snapshot (
    ts          TEXT PRIMARY KEY,
    payload     TEXT NOT NULL            -- JSON metrics, for the burn-up chart
);
"""


def connect():
    os.makedirs(os.path.dirname(DB), exist_ok=True)
    con = sqlite3.connect(DB)
    con.row_factory = sqlite3.Row
    con.executescript(SCHEMA)
    return con


def now():
    return datetime.now(timezone.utc).isoformat(timespec="seconds")


# ---------------------------------------------------------------- scan

def cmd_scan(args):
    con = connect()
    if not os.path.isdir(GHIDRA):
        sys.exit(f"no Ghidra reports at {GHIDRA} -- run tools/ghidra/run-headless.sh first")

    added = updated = 0
    for fn in sorted(os.listdir(GHIDRA)):
        if not fn.endswith(".functions.csv"):
            continue
        binary = fn[:-len(".functions.csv")]

        summ = os.path.join(GHIDRA, binary + ".summary.json")
        if os.path.exists(summ):
            with open(summ, encoding="utf-8") as fh:
                s = json.load(fh)
            con.execute(
                "INSERT INTO binary(name,sha256,size) VALUES(?,?,?) "
                "ON CONFLICT(name) DO UPDATE SET sha256=excluded.sha256",
                (binary, s.get("executable_sha256"), None))

        with open(os.path.join(GHIDRA, fn), newline="", encoding="utf-8") as fh:
            for row in csv.DictReader(fh):
                if row.get("is_thunk") == "true":
                    continue
                addr = row["address"]
                cur = con.execute(
                    "SELECT status FROM func WHERE binary=? AND addr=?", (binary, addr)
                ).fetchone()
                if cur is None:
                    con.execute(
                        "INSERT INTO func(binary,addr,size,section,orig_name,"
                        "num_blocks,num_callers,num_callees,status,updated) "
                        "VALUES(?,?,?,?,?,?,?,?,'unstarted',?)",
                        (binary, addr, int(row["size"]), row.get("section"),
                         row.get("name"), _i(row.get("num_blocks")),
                         _i(row.get("num_callers")), _i(row.get("num_callees")), now()))
                    added += 1
                else:
                    # Refresh structural facts but never clobber human annotation.
                    con.execute(
                        "UPDATE func SET size=?,section=?,num_blocks=?,num_callers=?,"
                        "num_callees=? WHERE binary=? AND addr=?",
                        (int(row["size"]), row.get("section"), _i(row.get("num_blocks")),
                         _i(row.get("num_callers")), _i(row.get("num_callees")),
                         binary, addr))
                    updated += 1
    con.commit()
    print(f"scan: {added} functions added, {updated} refreshed")


def _i(v):
    try:
        return int(v)
    except (TypeError, ValueError):
        return None


# ---------------------------------------------------------------- set / import

def cmd_set(args):
    con = connect()
    if args.status and args.status not in STATUSES:
        sys.exit(f"bad status {args.status!r}; one of {STATUSES}")
    sets, vals = [], []
    for col in ("status", "name", "tu", "subsystem", "notes"):
        v = getattr(args, col, None)
        if v is not None:
            sets.append(f"{col}=?")
            vals.append(v)
    if not sets:
        sys.exit("nothing to set")
    sets.append("updated=?")
    vals.append(now())

    n = 0
    for addr in args.addr:
        cur = con.execute("SELECT status FROM func WHERE binary=? AND addr=?",
                          (args.binary, addr)).fetchone()
        if cur is None:
            print(f"  ! {args.binary}:{addr} not found", file=sys.stderr)
            continue
        if args.status and not args.force:
            if RANK[args.status] < RANK[cur["status"]]:
                print(f"  ! {addr}: refusing {cur['status']} -> {args.status} "
                      f"(use --force)", file=sys.stderr)
                continue
        con.execute(f"UPDATE func SET {','.join(sets)} WHERE binary=? AND addr=?",
                    (*vals, args.binary, addr))
        n += 1
    con.commit()
    print(f"set: {n} function(s) updated")


def cmd_import(args):
    """Bulk annotation. JSON: [{"binary":..,"addr":..,"name":..,"tu":..,"status":..}, ...]"""
    con = connect()
    with open(args.file, encoding="utf-8") as fh:
        recs = json.load(fh)
    n = 0
    for r in recs:
        cols = [c for c in ("name", "tu", "subsystem", "status", "notes") if c in r]
        if not cols:
            continue
        con.execute(
            f"UPDATE func SET {','.join(c + '=?' for c in cols)},updated=? "
            "WHERE binary=? AND addr=?",
            (*[r[c] for c in cols], now(), r["binary"], r["addr"]))
        n += con.total_changes and 1 or 0
    con.commit()
    print(f"import: applied {len(recs)} records")


# ---------------------------------------------------------------- metrics

def metrics(con):
    out = {"generated": now(), "binaries": {}, "totals": {}, "by_status": {},
           "by_subsystem": {}, "by_tu": {}}

    tot_f = tot_b = done_f = done_b = 0
    for b in con.execute("SELECT name,sha256 FROM binary ORDER BY name"):
        rows = con.execute(
            "SELECT status, COUNT(*) c, COALESCE(SUM(size),0) s FROM func "
            "WHERE binary=? GROUP BY status", (b["name"],)).fetchall()
        bf = sum(r["c"] for r in rows)
        bb = sum(r["s"] for r in rows)
        df = sum(r["c"] for r in rows if r["status"] in DONE)
        db_ = sum(r["s"] for r in rows if r["status"] in DONE)
        out["binaries"][b["name"]] = {
            "sha256": b["sha256"],
            "functions": bf, "bytes": bb,
            "done_functions": df, "done_bytes": db_,
            "pct_functions": round(100 * df / bf, 3) if bf else 0.0,
            "pct_bytes": round(100 * db_ / bb, 3) if bb else 0.0,
            "status": {r["status"]: {"functions": r["c"], "bytes": r["s"]} for r in rows},
        }
        tot_f += bf; tot_b += bb; done_f += df; done_b += db_

    out["totals"] = {
        "functions": tot_f, "bytes": tot_b,
        "done_functions": done_f, "done_bytes": done_b,
        "pct_functions": round(100 * done_f / tot_f, 3) if tot_f else 0.0,
        "pct_bytes": round(100 * done_b / tot_b, 3) if tot_b else 0.0,
    }

    for r in con.execute("SELECT status, COUNT(*) c, COALESCE(SUM(size),0) s "
                         "FROM func GROUP BY status ORDER BY s DESC"):
        out["by_status"][r["status"]] = {"functions": r["c"], "bytes": r["s"]}

    for key, dest in (("subsystem", "by_subsystem"), ("tu", "by_tu")):
        for r in con.execute(
                f"SELECT COALESCE({key},'(unassigned)') k, COUNT(*) c, "
                "COALESCE(SUM(size),0) s, "
                "SUM(CASE WHEN status IN ('matching','library') THEN 1 ELSE 0 END) d, "
                "SUM(CASE WHEN status IN ('matching','library') THEN size ELSE 0 END) db "
                f"FROM func GROUP BY {key} ORDER BY s DESC"):
            out[dest][r["k"]] = {
                "functions": r["c"], "bytes": r["s"],
                "done_functions": r["d"] or 0, "done_bytes": r["db"] or 0,
                "pct_bytes": round(100 * (r["db"] or 0) / r["s"], 2) if r["s"] else 0.0,
            }
    return out


def cmd_report(args):
    con = connect()
    m = metrics(con)
    os.makedirs(REPORTS, exist_ok=True)

    con.execute("INSERT OR REPLACE INTO snapshot(ts,payload) VALUES(?,?)",
                (now(), json.dumps(m["totals"])))
    con.commit()

    with open(os.path.join(REPORTS, "progress.json"), "w", encoding="utf-8") as fh:
        json.dump(m, fh, indent=1)

    t = m["totals"]
    L = [
        "# Decompilation Progress",
        "",
        f"_Generated {m['generated']}_",
        "",
        f"## Overall: **{t['pct_bytes']:.3f}%** of code bytes matched",
        "",
        f"- **{t['done_bytes']:,} / {t['bytes']:,}** bytes",
        f"- **{t['done_functions']:,} / {t['functions']:,}** functions",
        "",
        "## By binary",
        "",
        "| Binary | Functions | Bytes | Matched fn | Matched bytes | % bytes |",
        "|---|---:|---:|---:|---:|---:|",
    ]
    for name, b in sorted(m["binaries"].items(),
                          key=lambda kv: -kv[1]["bytes"]):
        L.append(f"| {name} | {b['functions']:,} | {b['bytes']:,} | "
                 f"{b['done_functions']:,} | {b['done_bytes']:,} | "
                 f"{b['pct_bytes']:.2f}% |")

    L += ["", "## By status", "",
          "| Status | Functions | Bytes |", "|---|---:|---:|"]
    for s in STATUSES:
        if s in m["by_status"]:
            v = m["by_status"][s]
            L.append(f"| {s} | {v['functions']:,} | {v['bytes']:,} |")

    if len(m["by_subsystem"]) > 1:
        L += ["", "## By subsystem", "",
              "| Subsystem | Functions | Bytes | % matched |", "|---|---:|---:|---:|"]
        for k, v in m["by_subsystem"].items():
            L.append(f"| {k} | {v['functions']:,} | {v['bytes']:,} | {v['pct_bytes']:.2f}% |")

    with open(os.path.join(REPORTS, "PROGRESS.md"), "w", encoding="utf-8") as fh:
        fh.write("\n".join(L) + "\n")
    print(f"report: {t['pct_bytes']:.3f}% bytes "
          f"({t['done_bytes']:,}/{t['bytes']:,}), "
          f"{t['pct_functions']:.3f}% functions -> reports/PROGRESS.md")


def cmd_stats(args):
    con = connect()
    m = metrics(con)
    t = m["totals"]
    print(f"functions : {t['done_functions']:>7,} / {t['functions']:>7,}  "
          f"({t['pct_functions']:6.3f}%)")
    print(f"bytes     : {t['done_bytes']:>7,} / {t['bytes']:>7,}  "
          f"({t['pct_bytes']:6.3f}%)")
    print()
    for s in STATUSES:
        if s in m["by_status"]:
            v = m["by_status"][s]
            print(f"  {s:<12} {v['functions']:>6,} fn  {v['bytes']:>10,} B")


# ---------------------------------------------------------------- dashboard

def cmd_dash(args):
    con = connect()
    m = metrics(con)
    hist = [json.loads(r["payload"]) | {"ts": r["ts"]}
            for r in con.execute("SELECT ts,payload FROM snapshot ORDER BY ts")]
    html = DASH_HTML.replace("__DATA__", json.dumps({"metrics": m, "history": hist}))
    out = os.path.join(REPORTS, "dashboard.html")
    with open(out, "w", encoding="utf-8") as fh:
        fh.write(html)
    print(f"dash: {out}")


DASH_HTML = r"""<!doctype html>
<meta charset="utf-8"><title>MTG RE Progress</title>
<style>
 :root{--bg:#0f1115;--fg:#e6e6e6;--dim:#8b93a7;--line:#242a36;--ok:#4ade80;--mid:#60a5fa}
 @media(prefers-color-scheme:light){:root{--bg:#fff;--fg:#111;--dim:#666;--line:#e3e3e3}}
 body{background:var(--bg);color:var(--fg);font:14px/1.5 ui-sans-serif,system-ui,sans-serif;margin:0;padding:32px}
 h1{font-size:20px;margin:0 0 4px}h2{font-size:15px;margin:28px 0 10px;color:var(--dim);
  text-transform:uppercase;letter-spacing:.08em;font-weight:600}
 .big{font-size:44px;font-weight:700;letter-spacing:-.02em}
 .sub{color:var(--dim);margin-bottom:20px}
 table{border-collapse:collapse;width:100%;max-width:900px}
 th,td{text-align:left;padding:6px 10px;border-bottom:1px solid var(--line)}
 td.n,th.n{text-align:right;font-variant-numeric:tabular-nums}
 .bar{height:7px;background:var(--line);border-radius:4px;overflow:hidden;min-width:90px}
 .bar>i{display:block;height:100%;background:var(--ok)}
 .wrap{overflow-x:auto}
</style>
<h1>Magic: The Gathering / Shandalar — matching decompilation</h1>
<div class=sub id=gen></div>
<div class=big id=pct></div>
<div class=sub id=tot></div>
<h2>By binary</h2><div class=wrap><table id=bins></table></div>
<h2>By status</h2><div class=wrap><table id=stat></table></div>
<h2>By subsystem</h2><div class=wrap><table id=subs></table></div>
<script>
const D=__DATA__, M=D.metrics, T=M.totals;
const n=x=>x.toLocaleString();
gen.textContent='Generated '+M.generated;
pct.textContent=T.pct_bytes.toFixed(3)+'%';
tot.textContent=n(T.done_bytes)+' / '+n(T.bytes)+' bytes  ·  '+
  n(T.done_functions)+' / '+n(T.functions)+' functions';
const bar=p=>'<div class=bar><i style="width:'+Math.max(p,0).toFixed(2)+'%"></i></div>';
function tbl(el,head,rows){el.innerHTML='<tr>'+head.map((h,i)=>
  '<th class="'+(i?'n':'')+'">'+h+'</th>').join('')+'</tr>'+
  rows.map(r=>'<tr>'+r.map((c,i)=>'<td class="'+(i?'n':'')+'">'+c+'</td>').join('')+'</tr>').join('')}
tbl(bins,['Binary','Functions','Bytes','Matched','% bytes','']
 ,Object.entries(M.binaries).sort((a,b)=>b[1].bytes-a[1].bytes).map(([k,v])=>
  [k,n(v.functions),n(v.bytes),n(v.done_bytes),v.pct_bytes.toFixed(2)+'%',bar(v.pct_bytes)]));
tbl(stat,['Status','Functions','Bytes'],Object.entries(M.by_status).map(([k,v])=>
  [k,n(v.functions),n(v.bytes)]));
tbl(subs,['Subsystem','Functions','Bytes','% matched',''],
 Object.entries(M.by_subsystem).map(([k,v])=>
  [k,n(v.functions),n(v.bytes),v.pct_bytes.toFixed(2)+'%',bar(v.pct_bytes)]));
</script>
"""


# ---------------------------------------------------------------- cli

def main():
    p = argparse.ArgumentParser(prog="progress")
    sp = p.add_subparsers(dest="cmd", required=True)

    sp.add_parser("init").set_defaults(fn=lambda a: (connect(), print(f"init: {DB}")))
    sp.add_parser("scan").set_defaults(fn=cmd_scan)
    sp.add_parser("report").set_defaults(fn=cmd_report)
    sp.add_parser("dash").set_defaults(fn=cmd_dash)
    sp.add_parser("stats").set_defaults(fn=cmd_stats)

    s = sp.add_parser("set")
    s.add_argument("binary")
    s.add_argument("addr", nargs="+")
    s.add_argument("--status", choices=STATUSES)
    s.add_argument("--name"); s.add_argument("--tu")
    s.add_argument("--subsystem"); s.add_argument("--notes")
    s.add_argument("--force", action="store_true")
    s.set_defaults(fn=cmd_set)

    i = sp.add_parser("import")
    i.add_argument("file")
    i.set_defaults(fn=cmd_import)

    a = p.parse_args()
    a.fn(a)


if __name__ == "__main__":
    main()
