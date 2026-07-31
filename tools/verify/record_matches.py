#!/usr/bin/env python3
"""
Turn a reccmp JSON report into progress-DB `matching` records, honestly.

A Shandalar function is recorded `matching` only when:
  1. reccmp scored it exactly 1.0 (every instruction identical to the
     original, with linker-resolved fields matched through markers), and
  2. it carries a `// FUNCTION: SHANDALAR 0x...` marker in src/ (so the C
     that produced it is in this tree).

A Magic.exe function is recorded `matching` only when its normalised bytes
(funcbytes L1: rel32 + in-image absolutes masked, both sides) are identical
to a Shandalar function recorded above AND the correspondence is **unique in
both directions** -- exactly one function with those normalised bytes in each
image. This is the shared-core cross-credit the charter plans for ("work the
common core once and both benefit"), under the archive's discipline:

  "Cross-binary credit is earned, not assumed. A function counts toward
   another binary only when it is a unique, byte-exact match there."
  -- salvage/ARCHIVED-CLAUDE.md

Uniqueness matters because it is the difference between a real measurement
and an inflated one. Without it, one 18-byte card-script body credits every
other 18-byte body in the other image: dropping the requirement here turned
194 source functions into 515 Magic "matches" (28,464 B) instead of the 84
(20,491 B) that are genuinely one-to-one. The twin check is recomputed from
the bytes on every run, never cached.

The same test is applied to every other in-scope binary, not just Magic.exe:
`sidlib` is statically linked into Deckdll, Drawcardlib, Cardartlib and both
Facemakers, so a matched sidlib function legitimately credits there too --
under the same uniqueness rule, and never over an existing matching/library row.

Usage:
  record_matches.py <reccmp.json>          write reports/recon-import.json
  record_matches.py <reccmp.json> --apply  ... and apply it to the DB
  record_matches.py <reccmp.json> --by-tu  print matched bytes per translation
                                           unit and exit
  record_matches.py <new.json> --against <old.json>
                                           print per-TU and per-function moves
                                           between two runs, and exit nonzero
                                           if any TU lost matched bytes

`--by-tu` exists because editing anything in a translation unit re-rolls MSVC
4.2's register allocation across all of it, so a change aimed at one function
routinely moves its neighbours. Matched BYTES PER TU is the quantity a change
has to improve; the per-function percentage is not. Save a baseline, make the
change, compare -- and revert if the TU total falls.
"""

import json
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
from funcbytes import PE, normalise, h, load_csv  # noqa: E402

PY = sys.executable

# Every other in-scope module. Magic.exe first because it shares the most.
TARGETS = ["Magic.exe", "Deckdll.dll", "Drawcardlib.dll", "Cardartlib.dll",
           "Cdtools.dll", "Facemaker.exe", "Facemaker_sub.exe", "Manalink.exe",
           "Manalinkinterface.dll", "Statwin.dll", "Magvid.dll", "Magsnd.dll",
           "Deck.exe", "Gcconn.dll"]


def marker_map():
    """addr(int) -> (name, tu) from FUNCTION markers in src/."""
    out = {}
    pat = re.compile(r"^\s*//\s*FUNCTION:\s*SHANDALAR\s+0x([0-9a-fA-F]+)")
    for dirpath, _, files in os.walk(os.path.join(ROOT, "src")):
        for fn in files:
            if not fn.endswith(".c"):
                continue
            path = os.path.join(dirpath, fn)
            tu = os.path.relpath(path, os.path.join(ROOT, "src")).replace("\\", "/")
            lines = open(path, encoding="utf-8", errors="replace").read().splitlines()
            for i, line in enumerate(lines):
                m = pat.match(line)
                if not m:
                    continue
                name = None
                for j in range(i + 1, min(i + 4, len(lines))):
                    m2 = re.match(r"\s*(?:int|void|unsigned|char|short|long)\b.*?(\w+)\s*\(", lines[j])
                    if m2:
                        name = m2.group(1)
                        break
                out[int(m.group(1), 16)] = (name, tu)
    return out


def main():
    jpath = sys.argv[1]
    apply_it = "--apply" in sys.argv

    rep = json.load(open(jpath, encoding="utf-8"))
    markers = marker_map()

    matched = []
    for e in rep.get("data", []):
        if e.get("type") != 1 or e.get("matching") != 1.0:
            continue
        va = int(e["address"], 16)
        if va not in markers:
            continue
        name, tu = markers[va]
        matched.append({"va": va, "name": name or e.get("name"), "tu": tu})

    # sizes + twin verification
    peS = PE(os.path.join(ROOT, "orig", "Shandalar.exe"))
    S = {f["va"]: f for f in load_csv(os.path.join(ROOT, "reports", "ghidra", "Shandalar.exe.functions.csv"))}
    # how many Shandalar functions share each normalised hash -- needed for the
    # uniqueness test below (a body shared by many Shandalar functions cannot
    # single out which Magic function our source corresponds to)
    sidx = {}
    for f in S.values():
        _, n = normalise(peS, f["va"], f["size"])
        if n:
            sidx[h(n)] = sidx.get(h(n), 0) + 1

    if "--against" in sys.argv:
        oldp = sys.argv[sys.argv.index("--against") + 1]
        old = json.load(open(oldp, encoding="utf-8"))
        S0 = {f["va"]: f for f in load_csv(os.path.join(
            ROOT, "reports", "ghidra", "Shandalar.exe.functions.csv"))}

        def by_addr(r):
            return {int(e["address"], 16): e.get("matching")
                    for e in r.get("data", []) if e.get("type") == 1}

        A, B = by_addr(old), by_addr(rep)
        per = {}
        moves = []
        for va in set(A) | set(B):
            if va not in markers:
                continue
            tu = markers[va][1]
            size = S0.get(va, {}).get("size", 0)
            a, b = A.get(va), B.get(va)
            d = per.setdefault(tu, {"old": 0, "new": 0})
            if a == 1.0:
                d["old"] += size
            if b == 1.0:
                d["new"] += size
            if a != b:
                moves.append((va, size, a, b, tu))

        regressed = []
        print("per-TU matched bytes (old -> new):")
        for tu in sorted(per, key=lambda k: per[k]["new"] - per[k]["old"]):
            d = per[tu]
            delta = d["new"] - d["old"]
            if delta == 0:
                continue
            flag = "REGRESSED" if delta < 0 else "gained"
            if delta < 0:
                regressed.append(tu)
            print(f"  {tu:<24} {d['old']:>7,} -> {d['new']:>7,}  "
                  f"{delta:+8,}  {flag}")
        go = sum(d["old"] for d in per.values())
        gn = sum(d["new"] for d in per.values())
        print(f"  {'TOTAL':<24} {go:>7,} -> {gn:>7,}  {gn - go:+8,}")

        moves.sort(key=lambda m: -m[1])
        print(f"\n{len(moves)} functions moved:")
        for va, size, a, b, tu in moves[:40]:
            fa = "n/a" if a is None else f"{a*100:.2f}%"
            fb = "n/a" if b is None else f"{b*100:.2f}%"
            print(f"  0x{va:06x} {size:>5}B  {fa:>8} -> {fb:<8} {tu}")

        if regressed:
            print(f"\n{len(regressed)} TU(s) LOST matched bytes -- revert with:")
            for tu in regressed:
                print(f"  git checkout -- src/{tu}")
            sys.exit(1)
        return

    if "--by-tu" in sys.argv:
        S0 = {f["va"]: f for f in load_csv(os.path.join(
            ROOT, "reports", "ghidra", "Shandalar.exe.functions.csv"))}
        per = {}
        for e in rep.get("data", []):
            if e.get("type") != 1:
                continue
            va = int(e["address"], 16)
            if va not in markers:
                continue
            tu = markers[va][1]
            size = S0.get(va, {}).get("size", 0)
            d = per.setdefault(tu, {"tot": 0, "hit": 0, "nfn": 0, "nhit": 0})
            d["tot"] += size
            d["nfn"] += 1
            if e.get("matching") == 1.0:
                d["hit"] += size
                d["nhit"] += 1
        gt = gh = 0
        for tu in sorted(per, key=lambda k: -per[k]["hit"]):
            d = per[tu]
            gt += d["tot"]; gh += d["hit"]
            print(f"  {tu:<24} {d['hit']:>7,} / {d['tot']:>7,} B   "
                  f"{d['nhit']:>3}/{d['nfn']:>3} fn")
        print(f"  {'TOTAL':<24} {gh:>7,} / {gt:>7,} B")
        return

    records, s_bytes = [], 0
    src_of = {}                       # Shandalar addr -> (normhash, record)
    for m in matched:
        f = S.get(m["va"])
        if f is None:
            continue
        s_bytes += f["size"]
        rec = {
            "binary": "Shandalar.exe", "addr": f["addr"],
            "status": "matching", "name": m["name"], "tu": "src/" + m["tu"],
            "notes": f"reccmp 100.00% vs orig/Shandalar.exe (src/{m['tu']})",
        }
        records.append(rec)
        _, ns = normalise(peS, f["va"], f["size"])
        if ns is not None:
            src_of[f["addr"]] = (h(ns), ns, rec)

    print(f"Shandalar: {len(matched)} fn, {s_bytes:,} B at reccmp 100%")

    # cross-binary credit, one target at a time
    for binname in TARGETS:
        csvp = os.path.join(ROOT, "reports", "ghidra", binname + ".functions.csv")
        binp = os.path.join(ROOT, "orig", binname)
        if not (os.path.exists(csvp) and os.path.exists(binp)):
            continue
        pe = PE(binp)
        idx = {}
        for f in load_csv(csvp):
            _, n = normalise(pe, f["va"], f["size"])
            if n:
                idx.setdefault(h(n), []).append(f)
        got = b = ambiguous = 0
        for saddr, (key, ns, rec) in src_of.items():
            cands = idx.get(key, [])
            if len(cands) != 1 or sidx.get(key, 0) != 1:
                if cands:
                    ambiguous += 1
                continue
            tw = cands[0]
            _, nm = normalise(pe, tw["va"], tw["size"])
            if nm != ns:
                continue
            got += 1
            b += tw["size"]
            records.append({
                "binary": binname, "addr": tw["addr"],
                "status": "matching", "name": rec["name"], "tu": rec["tu"],
                "notes": (f"unique L1-identical twin of Shandalar.exe:{saddr} "
                          f"(normalised bytes equal, funcbytes; one-to-one in both "
                          f"images); source {rec['tu']} at reccmp 100.00%"),
            })
        if got or ambiguous:
            print(f"  {binname:<24} {got:>3} fn, {b:>7,} B"
                  f"   ({ambiguous} non-unique, no credit)")

    out = os.path.join(ROOT, "reports", "recon-import.json")
    with open(out, "w", encoding="utf-8") as fh:
        json.dump(records, fh, indent=1)
    print(f"{len(records)} records -> {out}")

    if apply_it:
        subprocess.check_call([PY, os.path.join(ROOT, "tools", "progress", "progress.py"),
                               "import", out])
        subprocess.check_call([PY, os.path.join(ROOT, "tools", "progress", "progress.py"),
                               "report"])


if __name__ == "__main__":
    main()
