#!/usr/bin/env python3
"""
Filter raw libmatch reports down to matches whose evidence is strong enough to
record as `library` in the progress DB, and emit the bulk-import JSON.

Why filtering is necessary (measured, not theoretical):

  * MSVC /Od compiles ANY trivial `return 0;` function to the identical 18-byte
    shell that LIBCD's ___init_collate has; Deckdll.dll contains a run of 28
    such stubs in app territory that byte-match it and are NOT the CRT.
  * `_strtoul` (a 38-byte forwarding wrapper) matched at 4 scattered addresses
    inside one EXE. The linker includes strtol.obj once, so at least 3 of those
    are app wrappers with coincidentally identical bytes.

Acceptance rules, in order:

  1. ANCHOR:   unmasked >= 64 compared bytes, or >= 4 relocations.
  2. ASM:      unmasked >= 24 and the source member is hand-written assembly
               (chkstk, ll*/ull* arithmetic, mem*/str* primitives) whose /Ox
               byte patterns cannot be confused with /Od C codegen.
  3. CLUSTER:  unmasked >= 24 and within 1 KiB of already-accepted code,
               grown to a fixed point. CRT objects link contiguously, so real
               small CRT functions sit next to big ones.
  4. COVERS:   sub-functions Ghidra split out of an accepted object function.
  5. DUP CAP:  a de-duplicated candidate is accepted at most once per distinct
               source member per binary (nearest-to-anchor copies win).

Everything else stays in the report as a lead but is NOT recorded.

Usage:  mkimport.py            writes reports/libmatch/import.json
"""

import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
D = os.path.join(ROOT, "reports", "libmatch")

ASM_MEMBERS = {
    "chkstk.obj", "llshl.obj", "llmul.obj", "lldiv.obj", "llrem.obj",
    "ullshr.obj", "ulldiv.obj", "ullrem.obj", "ulldvrm.obj", "lldvrm.obj",
    "memset.obj", "memcpy.obj", "memcmp.obj", "memmove.obj",
    "strlen.obj", "strcmp.obj", "strncmp.obj", "strncpy.obj",
    "strcspn.obj", "strpbrk.obj", "strcat.obj", "strchr.obj", "strrchr.obj",
}


def member_base(src):
    # "LIBCD.LIB:build\intel\xst_obj\chkstk.obj!_sym" -> "chkstk.obj"
    m = re.match(r"[^:]+:(.+)!", src)
    path = m.group(1) if m else src
    return path.replace("\\", "/").split("/")[-1].lower()


def filter_binary(matches):
    accepted = set()
    anchors = []

    def is_anchor(m):
        return m["unmasked"] >= 64 or m["nrelocs"] >= 4

    for m in matches:
        if m.get("part_of"):
            continue
        if is_anchor(m):
            accepted.add(m["addr"]); anchors.append(m["va"])
        elif m["unmasked"] >= 24 and member_base(m["sources"][0]) in ASM_MEMBERS:
            accepted.add(m["addr"])

    changed = True
    while changed:
        changed = False
        spans = [(m["va"], m["va"] + max(m["cand_len"], m["ghidra_size"]))
                 for m in matches if m["addr"] in accepted]
        for m in matches:
            if m["addr"] in accepted or m.get("part_of") or m["unmasked"] < 24:
                continue
            lo, hi = m["va"], m["va"] + m["ghidra_size"]
            if any(lo - 1024 <= e and b <= hi + 1024 for b, e in spans):
                accepted.add(m["addr"]); changed = True

    # duplicate cap: one accepted copy per distinct source member per candidate
    by_cand = {}
    for m in matches:
        if m["addr"] in accepted and not m.get("part_of"):
            by_cand.setdefault(m["sources"][0], []).append(m)
    for src, ms in by_cand.items():
        allowed = len({member_base(s) for m in ms for s in m["sources"]})
        if len(ms) <= allowed:
            continue
        def anchor_dist(m):
            return min((abs(m["va"] - a) for a in anchors), default=1 << 40)
        ms.sort(key=anchor_dist)
        for m in ms[allowed:]:
            accepted.discard(m["addr"])

    # covers-entries inherit their (still-)accepted parent
    for m in matches:
        if m.get("part_of") and m["part_of"] in accepted:
            accepted.add(m["addr"])
    return accepted


def main():
    records, rej_b = [], 0
    summary = []
    for fn in sorted(os.listdir(D)):
        if not fn.endswith(".json") or fn == "import.json":
            continue
        j = json.load(open(os.path.join(D, fn), encoding="utf-8"))
        ms = j["matches"]
        acc = filter_binary(ms)
        a_b = r_b = 0
        for m in ms:
            if m["addr"] not in acc:
                r_b += m["ghidra_size"]
                continue
            a_b += m["ghidra_size"]
            src = m["sources"][0]
            alt = f" (+{len(m['sources']) - 1} alt sources)" if len(m["sources"]) > 1 else ""
            if m.get("part_of"):
                note = (f"libmatch: inner part of {src}{alt}, "
                        f"parent {m['part_of']}; bytes verified with the parent span")
            else:
                note = (f"libmatch: exact bytes modulo {m['nrelocs']} reloc field(s) vs "
                        f"{src}{alt}; {m['unmasked']}/{m['cand_len']} bytes compared")
            records.append({
                "binary": j["binary"], "addr": m["addr"],
                "status": "library",
                "name": m["sym"],
                "tu": "crt/" + member_base(src),
                "subsystem": "crt",
                "notes": note,
            })
        rej_b += r_b
        summary.append((j["binary"], sum(1 for m in ms if m["addr"] in acc), a_b,
                        len(ms) - sum(1 for m in ms if m["addr"] in acc), r_b))

    out = os.path.join(D, "import.json")
    with open(out, "w", encoding="utf-8") as fh:
        json.dump(records, fh, indent=1)
    for b, af, ab, rf, rb in summary:
        print(f"  {b:<24} accept {af:>4} fn {ab:>7,} B   reject {rf:>3} fn {rb:>6,} B")
    tot = sum(s[2] for s in summary)
    print(f"\n{len(records)} records -> {out}")
    print(f"accepted {tot:,} B, rejected {rej_b:,} B")


if __name__ == "__main__":
    main()
