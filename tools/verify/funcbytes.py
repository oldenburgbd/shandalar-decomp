#!/usr/bin/env python3
"""
Function byte extraction + normalisation for match verification.

This is the core primitive of the whole project. Two jobs:

  1. Pull the raw bytes of a function out of a PE given (RVA, size).
  2. Normalise them so that two compilations of the SAME source differ only where
     the code genuinely differs -- not merely because they sit at different
     addresses or call through different thunks.

Normalisation masks, using a real x86 length decoder so instruction boundaries
are correct:
  - rel32 displacements of E8 (call), E9 (jmp), 0F 8x (jcc)
  - rel8  displacements of EB (jmp), 7x (jcc), E0-E3 (loop/jcxz)
  - any 4-byte immediate or displacement whose value lands inside the image
    (imageBase .. imageBase+sizeOfImage), i.e. an absolute address
  - trailing alignment padding (CC / 90 runs)

What is deliberately NOT masked: register allocation, instruction selection,
immediate constants, and stack layout. Those are exactly the things that must
match for a decompilation to be considered matching.

Usage
-----
  funcbytes.py info    <pe>
  funcbytes.py dump    <pe> <rva-hex> <size>           raw + normalised hexdump
  funcbytes.py hash    <pe> <functions.csv>            emit addr,size,rawhash,normhash
  funcbytes.py compare <peA> <csvA> <peB> <csvB>       shared-code report
"""

import csv
import hashlib
import struct
import sys

# --------------------------------------------------------------------- PE


class PE:
    def __init__(self, path):
        with open(path, "rb") as fh:
            self.data = fh.read()
        d = self.data
        if d[:2] != b"MZ":
            raise ValueError(f"{path}: not MZ")
        pe = struct.unpack_from("<I", d, 0x3C)[0]
        if d[pe:pe + 4] != b"PE\0\0":
            raise ValueError(f"{path}: not PE")
        self.path = path
        nsec, = struct.unpack_from("<H", d, pe + 6)
        optsz, = struct.unpack_from("<H", d, pe + 20)
        oh = pe + 24
        self.magic, = struct.unpack_from("<H", d, oh)
        self.image_base, = struct.unpack_from("<I", d, oh + 28)
        self.size_of_image, = struct.unpack_from("<I", d, oh + 56)
        self.entry, = struct.unpack_from("<I", d, oh + 16)

        self.sections = []
        so = oh + optsz
        for i in range(nsec):
            s = so + i * 40
            name = d[s:s + 8].rstrip(b"\0").decode("ascii", "replace")
            vsize, vaddr, rawsize, rawptr = struct.unpack_from("<IIII", d, s + 8)
            chars, = struct.unpack_from("<I", d, s + 36)
            self.sections.append({
                "name": name, "vsize": vsize, "vaddr": vaddr,
                "rawsize": rawsize, "rawptr": rawptr, "chars": chars,
                "exec": bool(chars & 0x20000000),
            })

    def rva_to_off(self, rva):
        for s in self.sections:
            span = max(s["vsize"], s["rawsize"])
            if s["vaddr"] <= rva < s["vaddr"] + span:
                off = s["rawptr"] + (rva - s["vaddr"])
                return off if off < len(self.data) else None
        return None

    def va_to_off(self, va):
        return self.rva_to_off(va - self.image_base)

    def read_va(self, va, size):
        off = self.va_to_off(va)
        if off is None:
            return None
        return self.data[off:off + size]

    def in_image(self, va):
        return self.image_base <= va < self.image_base + self.size_of_image


# ----------------------------------------------------- x86 length decoder
#
# Enough of a 32-bit x86 decoder to get instruction lengths and to locate the
# displacement/immediate fields we need to mask. Not a disassembler.

PREFIXES = {0xF0, 0xF2, 0xF3, 0x2E, 0x36, 0x3E, 0x26, 0x64, 0x65, 0x66, 0x67}

# one-byte opcodes -> (has_modrm, imm_size)  imm_size -1 means "operand size"
_1B = {}
for op in range(0x00, 0x40):
    if (op & 7) < 4:
        _1B[op] = (True, 0)
    elif (op & 7) == 4:
        _1B[op] = (False, 1)
    elif (op & 7) == 5:
        _1B[op] = (False, -1)
for op in range(0x40, 0x60):
    _1B[op] = (False, 0)                       # inc/dec/push/pop reg
for op in (0x60, 0x61, 0x6C, 0x6D, 0x6E, 0x6F, 0x90, 0x98, 0x99, 0x9B,
           0x9C, 0x9D, 0x9E, 0x9F, 0xC3, 0xC9, 0xCB, 0xCC, 0xCE, 0xCF,
           0xF4, 0xF5, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xD7):
    _1B[op] = (False, 0)
for op in range(0x91, 0x98):
    _1B[op] = (False, 0)
_1B[0x62] = (True, 0); _1B[0x63] = (True, 0)
_1B[0x68] = (False, -1); _1B[0x6A] = (False, 1)
_1B[0x69] = (True, -1);  _1B[0x6B] = (True, 1)
for op in range(0x70, 0x80):
    _1B[op] = (False, 1)                       # jcc rel8
_1B[0x80] = (True, 1); _1B[0x81] = (True, -1)
_1B[0x82] = (True, 1); _1B[0x83] = (True, 1)
for op in range(0x84, 0x90):
    _1B[op] = (True, 0)                        # test/xchg/mov/lea/pop r/m
for op in range(0xA0, 0xA4):
    _1B[op] = (False, 4)                       # mov al/eax, moffs
for op in (0xA4, 0xA5, 0xA6, 0xA7, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF):
    _1B[op] = (False, 0)
_1B[0xA8] = (False, 1); _1B[0xA9] = (False, -1)
for op in range(0xB0, 0xB8):
    _1B[op] = (False, 1)
for op in range(0xB8, 0xC0):
    _1B[op] = (False, -1)
_1B[0xC0] = (True, 1); _1B[0xC1] = (True, 1)
_1B[0xC2] = (False, 2); _1B[0xCA] = (False, 2)
_1B[0xC4] = (True, 0); _1B[0xC5] = (True, 0)
_1B[0xC6] = (True, 1); _1B[0xC7] = (True, -1)
_1B[0xC8] = (False, 3)
_1B[0xCD] = (False, 1)
for op in (0xD0, 0xD1, 0xD2, 0xD3):
    _1B[op] = (True, 0)
_1B[0xD4] = (False, 1); _1B[0xD5] = (False, 1)
for op in range(0xD8, 0xE0):
    _1B[op] = (True, 0)                        # x87
for op in range(0xE0, 0xE4):
    _1B[op] = (False, 1)                       # loop/jcxz rel8
_1B[0xE4] = (False, 1); _1B[0xE5] = (False, 1)
_1B[0xE6] = (False, 1); _1B[0xE7] = (False, 1)
_1B[0xE8] = (False, -1); _1B[0xE9] = (False, -1)
_1B[0xEA] = (False, 6)
_1B[0xEB] = (False, 1)
for op in (0xEC, 0xED, 0xEE, 0xEF):
    _1B[op] = (False, 0)
_1B[0xF6] = (True, 0); _1B[0xF7] = (True, 0)   # imm handled specially
_1B[0xFE] = (True, 0); _1B[0xFF] = (True, 0)


def _modrm_len(b, i, addr32):
    """Length of modrm + sib + disp starting at i. Returns (total, disp_off, disp_size)."""
    if i >= len(b):
        return 1, None, 0
    m = b[i]
    mod, rm = m >> 6, m & 7
    n = 1
    disp_off = disp_size = None
    if not addr32:                                    # 16-bit addressing (rare here)
        if mod == 0 and rm == 6:
            disp_off, disp_size = i + n, 2; n += 2
        elif mod == 1:
            disp_off, disp_size = i + n, 1; n += 1
        elif mod == 2:
            disp_off, disp_size = i + n, 2; n += 2
        return n, disp_off, disp_size or 0
    if mod != 3 and rm == 4:                          # SIB
        if i + n < len(b):
            sib = b[i + n]; n += 1
            if mod == 0 and (sib & 7) == 5:
                disp_off, disp_size = i + n, 4; n += 4
    if mod == 0 and rm == 5:
        disp_off, disp_size = i + n, 4; n += 4
    elif mod == 1:
        disp_off, disp_size = i + n, 1; n += 1
    elif mod == 2:
        disp_off, disp_size = i + n, 4; n += 4
    return n, disp_off, disp_size or 0


def decode(b, i):
    """Decode one instruction at b[i]. Returns dict with length and mask fields."""
    start = i
    opsz32 = True
    addr32 = True
    while i < len(b) and b[i] in PREFIXES:
        if b[i] == 0x66: opsz32 = False
        if b[i] == 0x67: addr32 = False
        i += 1
    if i >= len(b):
        return {"len": len(b) - start, "rel": None, "abs": []}

    op = b[i]; i += 1
    rel = None            # (offset, size) of a rip-style relative displacement
    absf = []             # [(offset, size)] of candidate absolute fields

    if op == 0x0F:
        if i >= len(b):
            return {"len": i - start, "rel": None, "abs": []}
        op2 = b[i]; i += 1
        if 0x80 <= op2 <= 0x8F:                       # jcc rel32
            rel = (i, 4); i += 4
            return {"len": i - start, "rel": rel, "abs": []}
        has_modrm = not (op2 in (0x05, 0x06, 0x07, 0x08, 0x09, 0x0B, 0x30, 0x31,
                                 0x32, 0x33, 0x34, 0x35, 0x77, 0xA0, 0xA1, 0xA8,
                                 0xA9, 0xC8) or 0xC8 <= op2 <= 0xCF)
        imm = 0
        if op2 in (0x70, 0x71, 0x72, 0x73, 0xA4, 0xAC, 0xBA, 0xC2, 0xC4, 0xC5, 0xC6):
            imm = 1
        if has_modrm:
            n, doff, dsz = _modrm_len(b, i, addr32)
            if dsz == 4: absf.append((doff, 4))
            i += n
        i += imm
        return {"len": i - start, "rel": rel, "abs": absf}

    has_modrm, immsz = _1B.get(op, (True, 0))
    if immsz == -1:
        immsz = 4 if opsz32 else 2

    if op in (0xE8, 0xE9):
        rel = (i, 4 if opsz32 else 2)
        i += rel[1]
        return {"len": i - start, "rel": rel, "abs": []}
    if op == 0xEB or (0x70 <= op <= 0x7F) or (0xE0 <= op <= 0xE3):
        rel = (i, 1); i += 1
        return {"len": i - start, "rel": rel, "abs": []}

    if has_modrm:
        if i < len(b):
            reg = (b[i] >> 3) & 7
            if op in (0xF6, 0xF7):                    # group 3: imm only for /0,/1
                immsz = (1 if op == 0xF6 else (4 if opsz32 else 2)) if reg in (0, 1) else 0
        n, doff, dsz = _modrm_len(b, i, addr32)
        if dsz == 4: absf.append((doff, 4))
        i += n
    if 0xA0 <= op <= 0xA3:                            # moffs32
        absf.append((i, 4))
    if immsz:
        if immsz == 4:
            absf.append((i, 4))
        i += immsz
    return {"len": i - start, "rel": rel, "abs": absf}


# ------------------------------------------------------------ normalise

def normalise(pe, va, size):
    """Return (raw, normalised) bytes for the function at virtual address `va`."""
    raw = pe.read_va(va, size)
    if raw is None:
        return None, None
    b = bytearray(raw)

    # strip trailing alignment padding
    end = len(b)
    while end > 0 and b[end - 1] in (0xCC, 0x90):
        end -= 1
    b = b[:end]

    i = 0
    n = len(b)
    while i < n:
        ins = decode(b, i)
        ln = max(1, ins["len"])
        if ins["rel"]:
            off, sz = ins["rel"]
            if off + sz <= n:
                b[off:off + sz] = b"\x00" * sz
        for off, sz in ins["abs"]:
            if off is not None and off + sz <= n:
                v = struct.unpack_from("<I", b, off)[0]
                if pe.in_image(v):
                    b[off:off + sz] = b"\x00" * sz
        i += ln
    return raw, bytes(b)


def h(x):
    return hashlib.sha256(x).hexdigest()[:16] if x is not None else None


def load_csv(path):
    out = []
    with open(path, newline="", encoding="utf-8") as fh:
        for r in csv.DictReader(fh):
            if r.get("is_thunk") == "true":
                continue
            try:
                out.append({"va": int(r["address"], 16), "size": int(r["size"]),
                            "addr": r["address"], "section": r.get("section", "")})
            except ValueError:
                continue
    return out


# ------------------------------------------------------------------ cli

def cmd_info(a):
    pe = PE(a[0])
    print(f"{pe.path}\n  imageBase=0x{pe.image_base:x}  sizeOfImage=0x{pe.size_of_image:x}"
          f"  entry=0x{pe.entry:x}")
    for s in pe.sections:
        print(f"  {s['name']:<10} vaddr=0x{s['vaddr']:<8x} vsize={s['vsize']:<9} "
              f"raw={s['rawsize']:<9} ptr=0x{s['rawptr']:<8x} exec={s['exec']}")


def cmd_dump(a):
    pe = PE(a[0]); va = int(a[1], 16); size = int(a[2])
    raw, norm = normalise(pe, va, size)
    if raw is None:
        sys.exit("address not mapped")
    print(f"va=0x{va:x} size={size} rawhash={h(raw)} normhash={h(norm)}")
    for label, data in (("RAW", raw), ("NORM", norm)):
        print(f"-- {label} ({len(data)} bytes) --")
        for o in range(0, min(len(data), 256), 16):
            chunk = data[o:o + 16]
            print(f"  {va+o:08x}  {chunk.hex(' '):<48}  "
                  f"{''.join(chr(c) if 32 <= c < 127 else '.' for c in chunk)}")


def cmd_hash(a):
    pe = PE(a[0])
    w = csv.writer(sys.stdout, lineterminator="\n")
    w.writerow(["address", "size", "rawhash", "normhash"])
    for f in load_csv(a[1]):
        raw, norm = normalise(pe, f["va"], f["size"])
        w.writerow([f["addr"], f["size"], h(raw), h(norm)])


def cmd_compare(a):
    peA, csvA, peB, csvB = PE(a[0]), a[1], PE(a[2]), a[3]
    A, B = load_csv(csvA), load_csv(csvB)

    def index(pe, fns):
        m = {}
        for f in fns:
            _, norm = normalise(pe, f["va"], f["size"])
            if norm is None:
                continue
            k = h(norm)
            m.setdefault(k, []).append(f)
        return m

    ia, ib = index(peA, A), index(peB, B)
    shared_f = shared_b = 0
    onlyA = []
    for k, fns in ia.items():
        nb = len(ib.get(k, []))
        for j, f in enumerate(fns):
            if j < nb:
                shared_f += 1; shared_b += f["size"]
            else:
                onlyA.append(f)

    totA = sum(f["size"] for f in A)
    totB = sum(f["size"] for f in B)
    nA, nB = a[0].split("\\")[-1].split("/")[-1], a[2].split("\\")[-1].split("/")[-1]
    print(f"A = {nA}: {len(A)} funcs, {totA:,} bytes")
    print(f"B = {nB}: {len(B)} funcs, {totB:,} bytes")
    print()
    print(f"SHARED (normalised-identical): {shared_f} funcs, {shared_b:,} bytes "
          f"= {100*shared_b/totA:.1f}% of A, {100*shared_b/totB:.1f}% of B")
    print(f"A-ONLY: {len(onlyA)} funcs, {totA-shared_b:,} bytes")
    union = totA + totB - shared_b
    print(f"UNION (unique code to reverse): {union:,} bytes "
          f"(vs {totA+totB:,} if treated independently -- "
          f"{100*(1-union/(totA+totB)):.1f}% saved)")
    onlyA.sort(key=lambda f: -f["size"])
    print("\n-- 15 largest A-only --")
    for f in onlyA[:15]:
        print(f"  {f['size']:>6}  {f['addr']}  {f['section']}")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    cmd, rest = sys.argv[1], sys.argv[2:]
    fn = {"info": cmd_info, "dump": cmd_dump, "hash": cmd_hash,
          "compare": cmd_compare}.get(cmd)
    if not fn:
        sys.exit(__doc__)
    fn(rest)
