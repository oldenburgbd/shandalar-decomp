#!/usr/bin/env python3
"""
Emit an objdiff-format `report.json` from the project progress database, so that
progress can be displayed by a decomp.dev instance (or the objdiff GUI).

Schema: objdiff.report.Report -- https://github.com/encounter/objdiff
        objdiff-core/protos/report.proto
JSON encoding is snake_case, matching what decomp.dev serves at /projects.json.

Mapping used here
-----------------
  objdiff "unit"     -> one translation unit (`func.tu`), or a per-binary
                        "<binary>/UNASSIGNED" bucket while TUs are still unknown
  objdiff "function" -> one function row from the progress DB
  objdiff "category" -> one per binary, plus one per subsystem, so the site can
                        break progress down both ways
  objdiff "section"  -> not emitted; we do not track data/rodata matching yet

Honesty rule (see CLAUDE.md): fuzzy_match_percent is 100 only for statuses that
mean byte-identical (`matching`, `library`). Everything else is 0 until a real
build exists to measure partial similarity against. We do not award credit for
"looks right".

Usage:
  objdiff_report.py [-o reports/report.json] [--version-name shandalar] [--pretty]
"""

import argparse
import json
import os
import re
import sqlite3
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
DB = os.path.join(ROOT, "progress", "progress.db")

# objdiff-core REPORT_VERSION as of rev 0c48d711 (the rev decomp.dev pins).
# Reports below this are accepted but migrated by the parser; emitting current
# avoids relying on migration.
REPORT_VERSION = 2

# Statuses that mean "the bytes are identical".
MATCHED = {"matching", "library"}

# proto3 JSON maps uint64 to string, but prost+serde commonly emits a number.
# Which one objdiff-core accepts is verified against the local decomp.dev
# instance; --addr-format switches encoding without touching anything else.
ADDR_FORMAT = "int"


def enc_addr(v):
    if ADDR_FORMAT == "hex":
        return f"0x{v:x}"
    if ADDR_FORMAT == "str":
        return str(v)
    return v


def slug(s):
    return re.sub(r"[^a-z0-9]+", "-", (s or "").lower()).strip("-") or "misc"


def measures(total_code=0, matched_code=0, total_functions=0, matched_functions=0,
             fuzzy_sum=0.0, total_units=0, complete_units=0,
             complete_code=0):
    """Build a Measures object. fuzzy_sum is size-weighted, divided out here."""
    pct = lambda a, b: (100.0 * a / b) if b else 0.0
    return {
        "fuzzy_match_percent": round(fuzzy_sum / total_code, 6) if total_code else 0.0,
        "total_code": total_code,
        "matched_code": matched_code,
        "matched_code_percent": round(pct(matched_code, total_code), 6),
        "total_data": 0,
        "matched_data": 0,
        "matched_data_percent": 0.0,
        "total_functions": total_functions,
        "matched_functions": matched_functions,
        "matched_functions_percent": round(pct(matched_functions, total_functions), 6),
        "complete_code": complete_code,
        "complete_code_percent": round(pct(complete_code, total_code), 6),
        "complete_data": 0,
        "complete_data_percent": 0.0,
        "total_units": total_units,
        "complete_units": complete_units,
    }


class Acc:
    """Accumulates measures over a set of functions."""

    def __init__(self):
        self.total_code = self.matched_code = 0
        self.total_functions = self.matched_functions = 0
        self.fuzzy_sum = 0.0
        self.total_units = self.complete_units = 0
        self.complete_code = 0

    def add_fn(self, size, fuzzy):
        self.total_code += size
        self.total_functions += 1
        self.fuzzy_sum += fuzzy * size
        if fuzzy >= 100.0:
            self.matched_code += size
            self.matched_functions += 1

    def add_unit(self, complete, code):
        self.total_units += 1
        if complete:
            self.complete_units += 1
            self.complete_code += code

    def build(self):
        return measures(self.total_code, self.matched_code, self.total_functions,
                        self.matched_functions, self.fuzzy_sum,
                        self.total_units, self.complete_units, self.complete_code)


def fuzzy_for(status):
    return 100.0 if status in MATCHED else 0.0


def build_report(con, version_name):
    rows = con.execute(
        "SELECT binary, addr, size, section, orig_name, name, tu, subsystem, status "
        "FROM func ORDER BY binary, addr").fetchall()
    if not rows:
        sys.exit("progress DB is empty -- run tools/progress/progress.py scan first")

    units = {}          # unit name -> {"binary":..,"funcs":[..],"subsystems":set()}
    for r in rows:
        tu = r["tu"] or f"{r['binary']}/UNASSIGNED"
        u = units.setdefault(tu, {"binary": r["binary"], "funcs": [],
                                  "subsystems": set()})
        u["funcs"].append(r)
        if r["subsystem"]:
            u["subsystems"].add(r["subsystem"])

    overall = Acc()
    by_binary = {}
    by_subsystem = {}
    out_units = []

    for name in sorted(units):
        u = units[name]
        ua = Acc()
        items = []
        for r in u["funcs"]:
            fz = fuzzy_for(r["status"])
            ua.add_fn(r["size"], fz)
            items.append({
                "name": r["name"] or r["orig_name"] or f"fn_{r['addr']}",
                "size": r["size"],
                "fuzzy_match_percent": fz,
                "address": enc_addr(int(r["addr"], 16)),
                "metadata": {"virtual_address": enc_addr(int(r["addr"], 16))},
            })

        complete = ua.total_functions > 0 and ua.matched_functions == ua.total_functions
        cats = [slug(u["binary"])] + [slug(s) for s in sorted(u["subsystems"])]

        out_units.append({
            "name": name,
            "measures": ua.build(),
            "sections": [],
            "functions": items,
            "metadata": {
                "complete": complete,
                "module_name": u["binary"],
                "source_path": name if not name.endswith("/UNASSIGNED") else None,
                "progress_categories": cats,
                "auto_generated": False,
            },
        })

        # roll up
        for acc_map, keys in ((by_binary, [u["binary"]]),
                              (by_subsystem, sorted(u["subsystems"]) or [])):
            for k in keys:
                a = acc_map.setdefault(k, Acc())
                for r in u["funcs"]:
                    a.add_fn(r["size"], fuzzy_for(r["status"]))
                a.add_unit(complete, ua.total_code)

        for r in u["funcs"]:
            overall.add_fn(r["size"], fuzzy_for(r["status"]))
        overall.add_unit(complete, ua.total_code)

    categories = []
    for k in sorted(by_binary):
        categories.append({"id": slug(k), "name": k, "measures": by_binary[k].build()})
    for k in sorted(by_subsystem):
        categories.append({"id": slug(k), "name": k.replace("-", " ").title(),
                           "measures": by_subsystem[k].build()})

    return {
        "measures": overall.build(),
        "units": out_units,
        "version": REPORT_VERSION,
        "categories": categories,
    }


def main():
    p = argparse.ArgumentParser()
    p.add_argument("-o", "--out", default=os.path.join(ROOT, "reports", "report.json"))
    p.add_argument("--version-name", default="shandalar",
                   help="report 'version' name used by decomp.dev (e.g. us/eu); "
                        "also names the <version>_report.json artifact")
    p.add_argument("--pretty", action="store_true")
    p.add_argument("--db", default=DB)
    p.add_argument("--addr-format", choices=["int", "str", "hex"], default="int",
                   help="uint64 address encoding; verify against the consuming parser")
    a = p.parse_args()

    global ADDR_FORMAT
    ADDR_FORMAT = a.addr_format

    if not os.path.exists(a.db):
        sys.exit(f"no progress DB at {a.db}")
    con = sqlite3.connect(a.db)
    con.row_factory = sqlite3.Row

    report = build_report(con, a.version_name)

    os.makedirs(os.path.dirname(a.out), exist_ok=True)
    with open(a.out, "w", encoding="utf-8") as fh:
        json.dump(report, fh, indent=1 if a.pretty else None)

    m = report["measures"]
    print(f"wrote {a.out}")
    print(f"  units      {m['total_units']:>6}  ({m['complete_units']} complete)")
    print(f"  functions  {m['matched_functions']:>6} / {m['total_functions']}  "
          f"({m['matched_functions_percent']:.3f}%)")
    print(f"  code       {m['matched_code']:>6} / {m['total_code']} bytes  "
          f"({m['matched_code_percent']:.3f}%)")
    print(f"  categories {len(report['categories'])}")


if __name__ == "__main__":
    main()
