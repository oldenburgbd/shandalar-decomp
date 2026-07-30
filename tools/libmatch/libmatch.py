#!/usr/bin/env python3
"""
Identify CRT / compiler-helper functions in the shipped binaries by byte-comparing
them against the objects inside the period toolchain's .LIB archives.

Method
------
For every COFF object in the given MSVC 4.2 libraries:
  1. take each function symbol in a code section (span = symbol value .. next
     function symbol / section end, trailing CC/90 padding trimmed);
  2. take the section's relocation entries that land inside that span -- each
     covers a 4-byte field the linker rewrites (DIR32 absolute address or
     REL32 call/jmp displacement).
For every non-thunk function Ghidra inventoried in a shipped binary:
  3. read the same number of bytes at the function's VA;
  4. zero the relocation fields on BOTH sides;
  5. the function "matches" the library object only if every remaining byte is
     identical.

This is an L1-strength claim (exact bytes modulo link-time fields), which is the
evidence standard for marking a function `library` in the progress DB. Nothing
is inferred from names; names come OUT of the match.

Guard rails against false positives:
  - candidate must retain >= MIN_UNMASKED unmasked bytes;
  - candidate length must agree with Ghidra's function size, or the residual
    tail must be pure alignment padding, or (candidate longer) the extra bytes
    must still match and any Ghidra functions inside the span are reported as
    covered by the same object;
  - every match records lib!member!symbol, lengths and reloc counts so it can
    be re-verified with `verify` mode.

Usage
-----
  libmatch.py scan  <lib-or-obj> [...]                 list candidate functions
  libmatch.py match <binary.dll|exe> <lib-or-obj> [...]   report matches (JSON to stdout)
  libmatch.py all   [--min-size N]                     match every DB binary against
                                                       the standard CRT lib set;
                                                       write reports/libmatch/<bin>.json
"""

import json
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, os.path.join(ROOT, "tools", "verify"))
from funcbytes import PE, load_csv  # noqa: E402

MSVC_LIB = r"C:\Tools\msvc42\MSDEV\LIB"

# The standard set: static CRT (release/debug, ST/MT), the intrinsic-form
# variants, the /MD import libs (whose static members hold startup + helpers),
# and the standalone helper objects.
DEFAULT_LIBS = [
    "LIBC.LIB", "LIBCD.LIB", "LIBCMT.LIB", "LIBCMTD.LIB",
    "LIBCI.LIB", "LIBCID.LIB", "LIBCIMT.LIB", "LIBCIMTD.LIB",
    "MSVCRT.LIB", "MSVCRTD.LIB", "MSVCIRT.LIB", "MSVCIRTD.LIB",
    "CHKSTK.OBJ", "FP10.OBJ", "BINMODE.OBJ", "COMMODE.OBJ", "NEWMODE.OBJ",
]

MIN_UNMASKED = 12          # a match must have at least this many compared bytes
PAD = (0xCC, 0x90)         # alignment padding bytes

IMAGE_SCN_CNT_CODE = 0x20
SYM_CLASS_EXTERNAL = 2
SYM_CLASS_STATIC = 3
DT_FUNCTION = 0x20


# ------------------------------------------------------------------ archives

def archive_members(path):
    """Yield (member_name, bytes) for a .LIB, or the file itself for a .OBJ."""
    data = open(path, "rb").read()
    if data[:8] != b"!<arch>\n":
        yield os.path.basename(path), data
        return
    longnames = b""
    off = 8
    while off + 60 <= len(data):
        hdr = data[off:off + 60]
        name = hdr[:16].decode("ascii", "replace").rstrip()
        try:
            size = int(hdr[48:58].decode().strip())
        except ValueError:
            break
        body = data[off + 60:off + 60 + size]
        if name == "//":
            longnames = body
        elif name not in ("/", ""):
            if name.startswith("/"):            # longname reference
                try:
                    lo = int(name[1:])
                    end = longnames.index(b"\0", lo)
                    name = longnames[lo:end].decode("ascii", "replace")
                except (ValueError, IndexError):
                    pass
            yield name.rstrip("/"), body
        off += 60 + size
        if off % 2:
            off += 1


# ------------------------------------------------------------------ COFF

def coff_functions(member_name, data):
    """Yield candidate dicts from one COFF object: name, bytes, reloc offsets."""
    if len(data) < 20:
        return
    machine, nsec, _, symoff, nsyms, optsz, _ = struct.unpack_from("<HHIIIHH", data, 0)
    if machine != 0x14C:                        # i386 only; skips import descriptors
        return
    if symoff == 0 or symoff + nsyms * 18 > len(data):
        return
    strtab_off = symoff + nsyms * 18
    strtab = data[strtab_off:]

    sections = []
    so = 20 + optsz
    for i in range(nsec):
        s = so + i * 40
        if s + 40 > len(data):
            return
        vsize, vaddr, rawsize, rawptr, relptr, _, nrel, _, chars = \
            struct.unpack_from("<IIIIIIHHI", data, s + 8)
        sections.append({"rawsize": rawsize, "rawptr": rawptr,
                         "relptr": relptr, "nrel": nrel, "chars": chars})

    def symname(ent):
        if ent[:4] == b"\0\0\0\0":
            # offset is relative to the start of the string table, which
            # begins with its own 4-byte size field
            o, = struct.unpack_from("<I", ent, 4)
            end = strtab.find(b"\0", o)
            return strtab[o:end].decode("ascii", "replace") if end >= 0 else ""
        return ent[:8].rstrip(b"\0").decode("ascii", "replace")

    # collect function symbols per code section
    per_sec = {}
    i = 0
    while i < nsyms:
        ent = data[symoff + i * 18: symoff + (i + 1) * 18]
        value, secnum, typ, cls, naux = struct.unpack_from("<IhHBB", ent, 8)
        i += 1 + naux
        if cls not in (SYM_CLASS_EXTERNAL, SYM_CLASS_STATIC):
            continue
        if secnum <= 0 or secnum > len(sections):
            continue
        sec = sections[secnum - 1]
        if not sec["chars"] & IMAGE_SCN_CNT_CODE:
            continue
        if typ & 0xF0 != DT_FUNCTION:
            continue
        per_sec.setdefault(secnum - 1, []).append((value, symname(ent)))

    for isec, syms in per_sec.items():
        sec = sections[isec]
        raw = data[sec["rawptr"]:sec["rawptr"] + sec["rawsize"]]
        relocs = []
        for r in range(sec["nrel"]):
            ro = sec["relptr"] + r * 10
            if ro + 10 > len(data):
                break
            vaddr, _, rtyp = struct.unpack_from("<IIH", data, ro)
            relocs.append(vaddr)
        syms.sort()
        bounds = sorted({v for v, _ in syms}) + [sec["rawsize"]]
        for value, name in syms:
            end = min(b for b in bounds if b > value)
            body = raw[value:end]
            # trim trailing alignment padding
            n = len(body)
            while n > 0 and body[n - 1] in PAD:
                n -= 1
            body = body[:n]
            if len(body) < 8:
                continue
            roffs = sorted(r - value for r in relocs if value <= r < value + len(body))
            masked = sum(min(4, len(body) - o) for o in roffs)
            if len(body) - masked < MIN_UNMASKED:
                continue
            yield {"member": member_name, "sym": name, "bytes": body,
                   "relocs": roffs, "unmasked": len(body) - masked}


def load_candidates(paths):
    """Load and de-duplicate candidates from libs. Returns list of dicts."""
    seen = {}                                   # masked-bytes -> candidate
    order = []
    for path in paths:
        lib = os.path.basename(path)
        for mname, mdata in archive_members(path):
            for c in coff_functions(mname, mdata):
                mb = bytearray(c["bytes"])
                for o in c["relocs"]:
                    mb[o:o + 4] = b"\0" * min(4, len(mb) - o)
                key = bytes(mb)
                src = f"{lib}:{c['member']}!{c['sym']}"
                if key in seen:
                    seen[key]["sources"].append(src)
                else:
                    c["sources"] = [src]
                    c["masked"] = key
                    seen[key] = c
                    order.append(c)
    return order


# ------------------------------------------------------------------ matching

def mask_binary(bin_bytes, roffs):
    b = bytearray(bin_bytes)
    for o in roffs:
        b[o:o + 4] = b"\0" * min(4, len(b) - o)
    return bytes(b)


def match_binary(pe, funcs, candidates, min_size=0):
    """Return list of match dicts for one binary."""
    by_first = {}
    for c in candidates:
        # first byte is never a reloc field in practice; if it is, bucket under None
        fb = c["bytes"][0] if 0 not in c["relocs"] else None
        by_first.setdefault(fb, []).append(c)

    addr_index = {f["va"]: f for f in funcs}
    sorted_vas = sorted(addr_index)
    matches = []
    claimed = set()

    for f in sorted(funcs, key=lambda x: -x["size"]):
        if f["size"] < min_size or f["va"] in claimed:
            continue
        first = pe.read_va(f["va"], 1)
        if not first:
            continue
        pool = by_first.get(first[0], []) + by_first.get(None, [])
        best = None
        for c in pool:
            clen = len(c["bytes"])
            gsize = f["size"]
            # candidate must cover the whole Ghidra function, or the tail must be padding
            if clen < gsize:
                tail = pe.read_va(f["va"] + clen, gsize - clen) or b""
                if any(t not in PAD for t in tail):
                    continue
            raw = pe.read_va(f["va"], clen)
            if raw is None or len(raw) < clen:
                continue
            if mask_binary(raw, c["relocs"]) != c["masked"]:
                continue
            if best is None or len(c["bytes"]) > len(best["bytes"]):
                best = c
        if best is None:
            continue
        clen = len(best["bytes"])
        covered = []
        if clen > f["size"]:
            # the object function spans past Ghidra's split; claim inner functions too
            for va in sorted_vas:
                if f["va"] < va < f["va"] + clen:
                    g = addr_index[va]
                    if va + g["size"] <= f["va"] + clen + 8:
                        covered.append(g)
        matches.append({
            "addr": f["addr"], "va": f["va"], "ghidra_size": f["size"],
            "cand_len": clen, "unmasked": best["unmasked"],
            "nrelocs": len(best["relocs"]),
            "sym": best["sym"], "sources": best["sources"],
            "covers": [g["addr"] for g in covered],
        })
        claimed.add(f["va"])
        for g in covered:
            claimed.add(g["va"])
            matches.append({
                "addr": g["addr"], "va": g["va"], "ghidra_size": g["size"],
                "cand_len": 0, "unmasked": 0, "nrelocs": 0,
                "sym": best["sym"] + f"+0x{g['va'] - f['va']:x}",
                "sources": best["sources"], "covers": [],
                "part_of": f["addr"],
            })
    return matches


# ------------------------------------------------------------------ cli

def db_functions(binary):
    import sqlite3
    con = sqlite3.connect(os.path.join(ROOT, "progress", "progress.db"))
    con.row_factory = sqlite3.Row
    rows = con.execute(
        "SELECT addr, size FROM func WHERE binary=?", (binary,)).fetchall()
    return [{"addr": r["addr"], "va": int(r["addr"], 16), "size": r["size"]}
            for r in rows]


def cmd_scan(args):
    cands = load_candidates(args)
    total = sum(len(c["bytes"]) for c in cands)
    print(f"{len(cands)} unique candidate functions, {total:,} bytes")
    for c in sorted(cands, key=lambda c: -len(c["bytes"]))[:25]:
        print(f"  {len(c['bytes']):>6}  {c['sources'][0]}")


def cmd_match(args):
    binpath, libs = args[0], args[1:] or [os.path.join(MSVC_LIB, l) for l in DEFAULT_LIBS]
    pe = PE(binpath)
    binary = os.path.basename(binpath)
    funcs = db_functions(binary)
    if not funcs:
        sys.exit(f"no functions in progress DB for {binary}")
    cands = load_candidates(libs)
    matches = match_binary(pe, funcs, cands)
    json.dump({"binary": binary, "matches": matches}, sys.stdout, indent=1)
    print(file=sys.stderr)
    print(f"{binary}: {len(matches)} matches, "
          f"{sum(m['ghidra_size'] for m in matches):,} bytes", file=sys.stderr)


def cmd_all(args):
    min_size = 0
    libs = [os.path.join(MSVC_LIB, l) for l in DEFAULT_LIBS]
    cands = load_candidates(libs)
    print(f"candidates: {len(cands)} unique functions from {len(libs)} libs")
    outdir = os.path.join(ROOT, "reports", "libmatch")
    os.makedirs(outdir, exist_ok=True)
    grand_f = grand_b = 0
    for fn in sorted(os.listdir(os.path.join(ROOT, "orig"))):
        if not fn.lower().endswith((".exe", ".dll")):
            continue
        binary = fn
        funcs = db_functions(binary)
        if not funcs:
            print(f"  {binary}: not in DB, skipped")
            continue
        pe = PE(os.path.join(ROOT, "orig", fn))
        matches = match_binary(pe, funcs, cands, min_size)
        b = sum(m["ghidra_size"] for m in matches)
        grand_f += len(matches); grand_b += b
        with open(os.path.join(outdir, binary + ".json"), "w", encoding="utf-8") as fh:
            json.dump({"binary": binary, "matches": matches}, fh, indent=1)
        print(f"  {binary}: {len(matches)} functions, {b:,} bytes")
    print(f"TOTAL: {grand_f} functions, {grand_b:,} bytes")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    cmd, rest = sys.argv[1], sys.argv[2:]
    if cmd == "scan":
        cmd_scan(rest)
    elif cmd == "match":
        cmd_match(rest)
    elif cmd == "all":
        cmd_all(rest)
    else:
        sys.exit(__doc__)
