# `.pic` — MicroProse PIC image format (PICv3)

Status: **reversed and implemented** (issue #1). Parser: `Shandalar.Core.Formats.Pic`.

`.pic` is MicroProse's block-structured palettized image format, shared across several of their
late-90s titles (Darklands, Civilization II, this game). Duels of the Planeswalkers ships 99
`.pic` files in the base directory (screens, backgrounds, map/dungeon art), plus more under the
resolution-variant folders. This document covers the **PICv3** variant, the only one this game
uses.

Verified empirically against the real install: `Title.pic`, `Face.pic`, `Art.pic`, `Dungeon.pic`
and others decode to exactly their header dimensions and render correctly (see the golden tests).

## File = a sequence of blocks

A `.pic` file is one or more concatenated blocks. Every block starts with a 4-byte header:

```
offset  size  field
0       2     tag       two ASCII chars: base type + subtype
2       2     length    uint16 LE — number of bytes following this field
4       ...   data
```

Tags seen in this game (the wider format also defines `C0`/`E0` CGA/EGA dither maps, unused here):

| Tag | Meaning | Count in base dir |
| --- | --- | --- |
| `M0` | Palette block, 6-bit VGA DAC (values 0–63) | 21 |
| `M1` | Palette block, 8-bit DAC (values 0–255) | 38 |
| `X0` | Image block, 8 bits/pixel (1 index per byte) | 40 (as first block) |
| `X1` | Image block, 4 bits/pixel (2 indices per byte) | none in this install |

Two file shapes occur:

- **Self-contained**: `[M0|M1 palette][X0|X1 image]` — the 59 `M*` files. The image indices are
  resolved through the embedded palette.
- **External-palette**: `[X0 image]` alone — the 40 `X*` files. The palette comes from a `.tr`
  file (see issue #3); community note: `WinBak01.pic`'s palette is a near-universal fallback.

Our parser walks blocks until it has consumed one image block (always the terminal block in this
game's files), collecting any palette block seen before it.

## Palette block (`M0` / `M1`)

```
offset  size  field
0       1     first     index of first entry (always 0x00 here)
1       1     last      index of last entry  (always 0xFF here → 256 entries)
2       3N    rgb       (last-first+1) entries, each [R, G, B]
```

With first=0, last=255 the block is `1 + 1 + 256*3 = 770` bytes of data → `length` = 770, total
block 774 bytes. This is always < 64 KB, so its `length` never wraps (see Q-002).

**`M0` values are 6-bit** (0–63, the VGA hardware DAC range) and must be scaled to 8-bit for
display. We use `v8 = (v6 << 2) | (v6 >> 4)` (maps 63→255, 0→0). **`M1` values are already 8-bit**
and used directly. Skipping the `M0` scaling leaves images roughly 4× too dark — that is the tell.

## Image block (`X0` / `X1`)

```
offset  size  field
0       2     width     uint16 LE, pixels
2       2     height    uint16 LE, pixels
4       1     max_bits  LZW maximum code width; always 0x0B (11) in this game
5       ...   stream    RLE-then-LZW compressed pixel indices
```

Packing is fixed by the tag: `X0` = one index per byte; `X1` = two indices per byte, left pixel
in the low nibble, right pixel in the high nibble, rows padded to a whole byte.

### Decompression: LZW first, then RLE

The encoder applied RLE to the raw index stream, then LZW to the result. **Decoding reverses the
order: LZW-decompress the block data, then RLE-expand.**

**LZW** — a variable-width, LSB-first scheme (ported faithfully from the reference decoder):

- Codes start at **9 bits** (`8 + indicatorLength`, `indicatorLength` starts at 1) and widen to
  10 then 11 bits as more codes are emitted, driven by a counter with thresholds
  256 → 512 → 1024. When the next width would exceed `max_bits` (11), the width **resets** to 9
  bits. Bits are consumed low-to-high; each code is `bits[0..7]` plus `indicatorLength` high bits.
- The dictionary holds indices 0–255 as single-byte literals, index 256 reserved, new entries
  starting at **257**, up to `1 << max_bits = 2048` entries. When the dictionary fills, it is
  rebuilt from scratch and the next code is taken as a fresh literal. The width-reset schedule and
  the dictionary-refill are designed to coincide (~1792 codes per cycle).

**RLE** — control byte `0x90`:

- `VV … 0x90 CC`: repeat the **previously emitted** byte `VV` until the run totals `CC` bytes
  (`CC` is the total run length; the reference expands `CC - 1` additional copies).
- `0x90 0x00`: a literal `0x90` byte.
- The first byte of the RLE stream is always emitted literally.

### Under-filled images

Some images decode to **fewer** than `width * height` indices (e.g. `Dungeon.pic` yields 241,664
of 307,200). The original draws onto a persistent framebuffer, so trailing uncovered pixels simply
keep whatever was there. We allocate a full `width * height` buffer, write the decoded indices, and
leave the remainder as index 0. If a stream over-produces, we truncate. This is not an error.

## Quirks

- **Q-002** (`docs/quirks.md`): the block `length` field is `uint16` and **wraps mod 65536** for
  blocks larger than 64 KB. Do not trust it to bound an image block — decode by `width * height`
  and read the compressed stream to end-of-file. (Confirmed: `Tradscrn.pic` remaining = 65,694,
  stored length = 158 = 65,694 − 65,536.)

## References

- Format reversed with help from the MicroProse PIC community work: Canadian Avenger's
  "PIC as we know it" write-up and the `benprew/mp_pic_tools` reference decoder (`lzw.py`,
  `rle.py`, `pic_headers.py`). Recorded here per the Phase-2 community-survey task (#10).
