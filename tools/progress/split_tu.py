#!/usr/bin/env python3
"""
Split a multi-function translation unit into one file per function.

Why this exists, measured rather than assumed:

  * Adding anything to a TU re-rolls MSVC 4.2's codegen for every function in
    it (archive finding, re-confirmed here). So functions in a big TU interfere
    with each other and the interference is not directionally predictable.
  * The two TUs in this tree that score 100% of their bytes -- `pred466fe6.c`
    and `reset.c` -- are single-function TUs.
  * The original build evidently had one source file per card script: three
    byte-identical 18-byte handlers sit at three different addresses instead of
    being one shared function. One function per file is the closer model of the
    original, not merely a hedge.

Splitting is only safe for files whose functions are not yet recorded
`matching`: it changes object layout, so it must be measured before and after
with `record_matches.py --by-tu`. This script refuses to touch a file that
contains an address already recorded `matching` in the progress DB unless
--force is given.

Each output file gets the source file's preamble (its comment header, #include
lines and any file-local extern block -- everything before the first FUNCTION
marker), so the declaration surface each function compiles against is
unchanged.

Usage:
  split_tu.py src/game/xtra54.c [...]        write src/game/<name>_<addr>.c
  split_tu.py --dry-run src/game/xtra54.c    report what it would do
  split_tu.py --all-new                      split every src/game/*.c whose
                                             functions are all unrecorded
"""

import os
import re
import sqlite3
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
MARKER = re.compile(r"^\s*//\s*FUNCTION:\s*SHANDALAR\s+0x([0-9a-fA-F]+)\s*$")


def recorded_matching():
    db = os.path.join(ROOT, "progress", "progress.db")
    if not os.path.exists(db):
        return set()
    con = sqlite3.connect(db)
    return {int(a, 16) for a, in con.execute(
        "SELECT addr FROM func WHERE binary='Shandalar.exe' AND status='matching'")}


def split(path, done, dry, force):
    text = open(path, encoding="utf-8", errors="replace").read()
    lines = text.splitlines(keepends=True)

    starts = [(i, int(m.group(1), 16)) for i, l in enumerate(lines)
              if (m := MARKER.match(l))]
    if len(starts) < 2:
        print(f"  {path}: {len(starts)} function(s) -- nothing to split")
        return 0

    clash = [a for _, a in starts if a in done]
    if clash and not force:
        print(f"  {path}: SKIPPED -- holds {len(clash)} already-matching "
              f"function(s) (e.g. 0x{clash[0]:08x}); use --force to override")
        return 0

    preamble = "".join(lines[:starts[0][0]]).rstrip() + "\n\n"
    base = os.path.splitext(path)[0]
    written = 0
    for k, (idx, addr) in enumerate(starts):
        end = starts[k + 1][0] if k + 1 < len(starts) else len(lines)
        body = "".join(lines[idx:end]).rstrip() + "\n"
        out = f"{base}_{addr:06x}.c"
        if dry:
            print(f"    would write {os.path.relpath(out, ROOT)} "
                  f"(0x{addr:08x}, {len(body)} bytes)")
        else:
            with open(out, "w", encoding="utf-8", newline="\n") as fh:
                fh.write(preamble)
                fh.write(body)
        written += 1
    if not dry:
        os.remove(path)
        print(f"  {path}: split into {written} files, original removed")
    else:
        print(f"  {path}: would split into {written} files")
    return written


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    dry = "--dry-run" in sys.argv
    force = "--force" in sys.argv
    done = recorded_matching()

    if "--all-new" in sys.argv:
        gdir = os.path.join(ROOT, "src", "game")
        args = []
        for fn in sorted(os.listdir(gdir)):
            if not fn.endswith(".c"):
                continue
            p = os.path.join(gdir, fn)
            addrs = {int(m.group(1), 16) for m in
                     (MARKER.match(l) for l in open(p, encoding="utf-8",
                                                    errors="replace"))
                     if m}
            if addrs and not (addrs & done):
                args.append(p)

    if not args:
        sys.exit(__doc__)
    total = 0
    for p in args:
        total += split(p, done, dry, force)
    print(f"{'would write' if dry else 'wrote'} {total} single-function files")
    if not dry and total:
        print("now: python tools/progress/genmake.py && build.cmd")


if __name__ == "__main__":
    main()
