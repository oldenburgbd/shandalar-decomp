#!/usr/bin/env python3
"""
Import the archived first attempt's reconstruction into the progress database.

Two sources, joined on address:

  1. `salvage/decomp/src|include/**.c|h` -- reccmp annotations. These are the
     authoritative record of WHICH binary a reconstruction targets and what the
     function was named:
         // FUNCTION: SHANDALAR 0x0057b710
         int Spr_LoadFile(void **frames, const char *path)
     Also GLOBAL (variable addresses) and STRING (literal addresses).

  2. `salvage/decomp/progress/history.jsonl` -- the last record holds the final
     per-function reccmp score from the archived build.

Status assignment is deliberately conservative. The archived scores were
measured against an MSVC 4.2 build we cannot currently reproduce (the compiler
binaries are present but its INCLUDE headers are not), so nothing is imported
as `matching`. A 100% score becomes `decompiled` with the prior measurement
recorded in notes, and is promoted to `matching` only when re-measured here.
Claiming otherwise would put unverifiable numbers into the headline metric.

  reccmp 100%      -> decompiled   (note: "prior reccmp 100% - awaiting re-verification")
  reccmp 0 < s <100-> decompiled   (note: "prior reccmp NN.N%")
  annotated, unscored -> identified

Usage:
  import_salvage.py [--apply]      # default is a dry run
"""

import argparse
import json
import os
import re
import sqlite3
import sys
from collections import defaultdict
from datetime import datetime, timezone

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
DB = os.path.join(ROOT, "progress", "progress.db")
SALVAGE = os.path.join(ROOT, "salvage", "decomp")

# reccmp module name -> our binary name.
# FACEMAKER is the copy in the game's Facemaker\ subdirectory, NOT the one in the
# game root. Confirmed by the sha256 pinned in salvage/decomp/reccmp-project.yml:
# c8d15f00... == our Facemaker_sub.exe. The root Facemaker.exe (b4f3d196...) is a
# different, later build and is not a reccmp target.
MODULE_BINARY = {
    "SHANDALAR": "Shandalar.exe",
    "FACEMAKER": "Facemaker_sub.exe",
    "DECK": "Deck.exe",
    "MAGIC": "Magic.exe",
    "DECKDLL": "Deckdll.dll",
    "DRAWCARDLIB": "Drawcardlib.dll",
    "CARDARTLIB": "Cardartlib.dll",
}

MARKER = re.compile(r"//\s*(FUNCTION|GLOBAL|STRING)\s*:\s*([A-Z_]+)\s+(0x[0-9a-fA-F]+)")
# A C definition: grab the identifier immediately before '(' on the next
# non-blank, non-comment line.
DEFN = re.compile(r"([A-Za-z_][A-Za-z0-9_]*)\s*\(")
VARDEF = re.compile(r"([A-Za-z_][A-Za-z0-9_]*)\s*(?:\[[^\]]*\])?\s*(?:=|;)")


def parse_annotations():
    """-> {'FUNCTION': {(binary, addr): {name, file}}, 'GLOBAL': {...}, ...}"""
    out = {"FUNCTION": {}, "GLOBAL": {}, "STRING": {}}
    unknown_modules = set()
    for sub in ("src", "include"):
        base = os.path.join(SALVAGE, sub)
        for dirpath, _, files in os.walk(base):
            for fn in files:
                if not fn.endswith((".c", ".h")):
                    continue
                path = os.path.join(dirpath, fn)
                rel = os.path.relpath(path, SALVAGE).replace("\\", "/")
                with open(path, encoding="utf-8", errors="replace") as fh:
                    lines = fh.readlines()
                for i, line in enumerate(lines):
                    m = MARKER.search(line)
                    if not m:
                        continue
                    kind, module, addr = m.group(1), m.group(2), m.group(3)
                    binary = MODULE_BINARY.get(module)
                    if binary is None:
                        unknown_modules.add(module)
                        continue
                    name = None
                    for j in range(i + 1, min(i + 6, len(lines))):
                        nxt = lines[j].strip()
                        if not nxt or nxt.startswith("//") or nxt.startswith("/*"):
                            continue
                        pat = DEFN if kind == "FUNCTION" else VARDEF
                        mm = pat.search(nxt)
                        if mm:
                            name = mm.group(1)
                        break
                    key = (binary, int(addr, 16))
                    out[kind][key] = {"name": name, "file": rel}
    if unknown_modules:
        print(f"  ! unmapped reccmp modules: {sorted(unknown_modules)}", file=sys.stderr)
    return out


def parse_scores():
    """-> {addr_int: score_or_None} from the final history record."""
    hp = os.path.join(SALVAGE, "progress", "history.jsonl")
    if not os.path.exists(hp):
        return {}, None
    with open(hp, encoding="utf-8") as fh:
        lines = [l for l in fh.read().strip().splitlines() if l.strip()]
    last = json.loads(lines[-1])
    scores = {}
    for name, f in (last.get("functions") or {}).items():
        try:
            scores[int(f["addr"], 16)] = f.get("score")
        except (KeyError, ValueError):
            continue
    return scores, last


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--apply", action="store_true", help="write to the DB (default: dry run)")
    a = ap.parse_args()

    if not os.path.isdir(SALVAGE):
        sys.exit(f"no salvage tree at {SALVAGE}")

    ann = parse_annotations()
    scores, last = parse_scores()

    print(f"annotations : {len(ann['FUNCTION'])} FUNCTION, "
          f"{len(ann['GLOBAL'])} GLOBAL, {len(ann['STRING'])} STRING")
    if last:
        print(f"history     : {last['sha']} {last['date']} -- "
              f"scored={last.get('scored')} byteExact={last.get('byteExact')}")
        print(f"              {last.get('subject','')}")
    print()

    con = sqlite3.connect(DB)
    con.row_factory = sqlite3.Row
    now = datetime.now(timezone.utc).isoformat(timespec="seconds")

    stats = defaultdict(int)
    bytes_by_status = defaultdict(int)
    unmatched = []
    updates = []

    for (binary, addr), info in sorted(ann["FUNCTION"].items()):
        row = con.execute(
            "SELECT addr, size, status FROM func WHERE binary=? AND addr=?",
            (binary, f"{addr:08x}")).fetchone()
        if row is None:
            unmatched.append((binary, addr, info["name"]))
            stats["not_in_db"] += 1
            continue

        score = scores.get(addr)
        if score is None:
            status, note = "identified", "archived: annotated, unscored"
        elif score >= 100:
            status = "decompiled"
            note = "archived: reccmp 100% - awaiting re-verification here"
        else:
            status = "decompiled"
            note = f"archived: reccmp {score:.2f}%"

        tu = info["file"].replace("src/", "").replace("include/", "")
        subsystem = tu.split("/")[0] if "/" in tu else "core"

        updates.append((info["name"], tu, subsystem, status, note, now, binary, row["addr"]))
        stats[status] += 1
        bytes_by_status[status] += row["size"]
        if score is not None and score >= 100:
            stats["prior_100"] += 1
            bytes_by_status["prior_100"] += row["size"]

    print("=== FUNCTION import ===")
    for k in ("identified", "decompiled", "not_in_db"):
        if stats[k]:
            print(f"  {k:<12} {stats[k]:>5}   {bytes_by_status[k]:>10,} bytes")
    print(f"  {'of which prior reccmp 100%':<12} {stats['prior_100']:>5}   "
          f"{bytes_by_status['prior_100']:>10,} bytes")

    if unmatched:
        print(f"\n  {len(unmatched)} annotated addresses not present in our Ghidra inventory")
        for b, ad, nm in unmatched[:8]:
            print(f"    {b} 0x{ad:08x} {nm}")
        if len(unmatched) > 8:
            print(f"    ... and {len(unmatched)-8} more")

    if not a.apply:
        print("\n(dry run -- pass --apply to write)")
        return

    con.executemany(
        "UPDATE func SET name=?, tu=?, subsystem=?, status=?, notes=?, updated=? "
        "WHERE binary=? AND addr=?", updates)
    con.commit()
    print(f"\napplied {len(updates)} function updates")


if __name__ == "__main__":
    main()
