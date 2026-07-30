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

Usage:
  record_matches.py <reccmp.json>          write reports/recon-import.json
  record_matches.py <reccmp.json> --apply  ... and apply it to the DB
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
    peM = PE(os.path.join(ROOT, "orig", "Magic.exe"))
    S = {f["va"]: f for f in load_csv(os.path.join(ROOT, "reports", "ghidra", "Shandalar.exe.functions.csv"))}
    M = load_csv(os.path.join(ROOT, "reports", "ghidra", "Magic.exe.functions.csv"))
    midx = {}
    for f in M:
        _, n = normalise(peM, f["va"], f["size"])
        if n:
            midx.setdefault(h(n), []).append(f)
    # how many Shandalar functions share each normalised hash -- needed for the
    # uniqueness test below (a body shared by many Shandalar functions cannot
    # single out which Magic function our source corresponds to)
    sidx = {}
    for f in S.values():
        _, n = normalise(peS, f["va"], f["size"])
        if n:
            sidx[h(n)] = sidx.get(h(n), 0) + 1

    records, s_bytes, m_bytes, m_fns, ambiguous = [], 0, 0, 0, 0
    for m in matched:
        f = S.get(m["va"])
        if f is None:
            continue
        s_bytes += f["size"]
        records.append({
            "binary": "Shandalar.exe", "addr": f["addr"],
            "status": "matching", "name": m["name"], "tu": "src/" + m["tu"],
            "notes": f"reccmp 100.00% vs orig/Shandalar.exe (src/{m['tu']})",
        })
        _, ns = normalise(peS, f["va"], f["size"])
        if ns is None:
            continue
        key = h(ns)
        cands = midx.get(key, [])
        # unique in both directions, or no credit
        if len(cands) != 1 or sidx.get(key, 0) != 1:
            ambiguous += 1
            continue
        tw = cands[0]
        _, nm = normalise(peM, tw["va"], tw["size"])
        if nm != ns:
            continue
        m_fns += 1
        m_bytes += tw["size"]
        records.append({
            "binary": "Magic.exe", "addr": tw["addr"],
            "status": "matching", "name": m["name"], "tu": "src/" + m["tu"],
            "notes": (f"unique L1-identical twin of Shandalar.exe:{f['addr']} "
                      f"(normalised bytes equal, funcbytes; one-to-one in both "
                      f"images); source src/{m['tu']} at reccmp 100.00%"),
        })

    out = os.path.join(ROOT, "reports", "recon-import.json")
    with open(out, "w", encoding="utf-8") as fh:
        json.dump(records, fh, indent=1)
    print(f"Shandalar: {len(matched)} fn, {s_bytes:,} B at reccmp 100%")
    print(f"Magic twins: {m_fns} fn, {m_bytes:,} B (unique one-to-one, normalised-identical)")
    print(f"  {ambiguous} Shandalar fn had non-unique twins -- no credit taken")
    print(f"{len(records)} records -> {out}")

    if apply_it:
        subprocess.check_call([PY, os.path.join(ROOT, "tools", "progress", "progress.py"),
                               "import", out])
        subprocess.check_call([PY, os.path.join(ROOT, "tools", "progress", "progress.py"),
                               "report"])


if __name__ == "__main__":
    main()
