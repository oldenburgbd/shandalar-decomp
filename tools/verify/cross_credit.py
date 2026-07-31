#!/usr/bin/env python3
"""
Extend the Shandalar `matching` records to every other in-scope binary that
carries a byte-identical copy of the same function.

Rule (same as record_matches.py's Magic pass, applied per binary): a target
function is credited only when its normalised bytes (funcbytes L1) equal a
recorded-matching Shandalar function's AND the correspondence is unique in
both images -- exactly one function with that normalised body on each side.
Non-unique bodies get no credit; a shared trivial body cannot identify which
copy our source corresponds to. Already-credited rows (matching/library) are
skipped.

Usage:
  cross_credit.py            write reports/cross-import.json
  cross_credit.py --apply    ... and apply it to the DB
"""

import json
import os
import sqlite3
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
from funcbytes import PE, normalise, h, load_csv  # noqa: E402

TARGETS = ["Cardartlib.dll", "Cdtools.dll", "Deck.exe", "Deckdll.dll",
           "Drawcardlib.dll", "Facemaker.exe", "Facemaker_sub.exe",
           "Gcconn.dll", "Magsnd.dll", "Magvid.dll", "Manalink.exe",
           "Manalinkinterface.dll", "Statwin.dll"]


def main():
    apply_it = "--apply" in sys.argv
    src = json.load(open(os.path.join(ROOT, "reports", "recon-import.json"), encoding="utf-8"))
    sh = [r for r in src if r["binary"] == "Shandalar.exe"]

    peS = PE(os.path.join(ROOT, "orig", "Shandalar.exe"))
    S = {f["addr"]: f for f in load_csv(os.path.join(ROOT, "reports", "ghidra",
                                                     "Shandalar.exe.functions.csv"))}
    sidx = {}
    for f in S.values():
        _, n = normalise(peS, f["va"], f["size"])
        if n:
            sidx[h(n)] = sidx.get(h(n), 0) + 1

    con = sqlite3.connect(os.path.join(ROOT, "progress", "progress.db"))
    done = {(b, a) for b, a in con.execute(
        "SELECT binary, addr FROM func WHERE status IN ('matching','library')")}

    records = []
    for binname in TARGETS:
        csvp = os.path.join(ROOT, "reports", "ghidra", binname + ".functions.csv")
        if not os.path.exists(csvp):
            continue
        pe = PE(os.path.join(ROOT, "orig", binname))
        idx = {}
        for f in load_csv(csvp):
            _, n = normalise(pe, f["va"], f["size"])
            if n:
                idx.setdefault(h(n), []).append(f)
        got = b = 0
        for r in sh:
            f = S.get(r["addr"])
            if f is None:
                continue
            _, ns = normalise(peS, f["va"], f["size"])
            if ns is None:
                continue
            k = h(ns)
            cands = idx.get(k, [])
            if len(cands) != 1 or sidx.get(k, 0) != 1:
                continue
            tw = cands[0]
            if (binname, tw["addr"]) in done:
                continue
            _, nm = normalise(pe, tw["va"], tw["size"])
            if nm != ns:
                continue
            got += 1
            b += tw["size"]
            records.append({
                "binary": binname, "addr": tw["addr"],
                "status": "matching", "name": r["name"], "tu": r["tu"],
                "notes": (f"unique L1-identical twin of Shandalar.exe:{f['addr']} "
                          f"(normalised bytes equal, funcbytes; one-to-one in both "
                          f"images); source {r['tu']} at reccmp 100.00%"),
            })
        if got:
            print(f"  {binname:<24} {got:>3} fn, {b:>7,} B")

    out = os.path.join(ROOT, "reports", "cross-import.json")
    with open(out, "w", encoding="utf-8") as fh:
        json.dump(records, fh, indent=1)
    print(f"{len(records)} records -> {out}")

    if apply_it:
        subprocess.check_call([sys.executable,
                               os.path.join(ROOT, "tools", "progress", "progress.py"),
                               "import", out])
        subprocess.check_call([sys.executable,
                               os.path.join(ROOT, "tools", "progress", "progress.py"),
                               "report"])


if __name__ == "__main__":
    main()
