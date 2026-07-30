# `.cat` / WVL — card-art archives

Status: **reversed and implemented** (issue #6). Parsers: `Shandalar.Core.Formats.CatArchive`
(container) and `Wvl` (image codec).

`Cardart/Smallart.cat` (12.8 MB, 944 entries) and `Cardart/Medart.cat` (31.5 MB, 943 entries)
hold the card art at two resolutions — e.g. card 0 is 144×116 in Smallart and 288×232 in Medart.
Entries are wavelet-compressed images; "WVL" (the extension `Cards.dat` uses in its art-file
field, `NNNN.WVL`) is short for **wavelet**. Both sizes are derived from the same ~580×467
originals (whose full dimensions each header records).

## `.cat` container

```
offset  size   field
0       4      entry count (uint32 LE)
4       12*N   index entries: { u32 runtimePtr, u32 offset, u32 size }
...            entry data blobs (2048-byte-aligned offsets, unordered)
```

- **`runtimePtr` is garbage on disk** — a dumped in-memory pointer (identical values appear in
  both archives). Ignore it.
- Each entry's **name** is stored inside its own blob (see WVL header): `c<id>.tif` (a few lack
  the `c` prefix, e.g. `0001.tif`). The digits are the card ID from `Cards.dat`/`Master.csv`.
  944/943 entries vs 999 cards: some cards share or lack art.

## WVL image blob

156-byte (0x9C) header, then compressed data:

| Offset | Field |
| --- | --- |
| 0x00 | flag: nonzero → chroma stored at half resolution (always the case here) |
| 0x04 | header size (0x9C) |
| 0x08 | original filename, NUL-terminated (`c0000.tif`) |
| 0x14, 0x18 | full-resolution art dimensions (e.g. 580×467) — informational |
| 0x1C, 0x20 | **this image's width, height** |
| 0x24 | DC band edge (9) — the wavelet pyramid's base size |
| 0x28 | piece count (1 in these archives) |
| 0x5C/0x6C/0x7C | per-piece compressed sizes of the Y / Cb / Cr planes |
| 0x90 | decompressed size |

### Codec pipeline (decode)

1. **VLC entropy decode** — a Huffman-style code tree (13-bit node indices, stored first as
   `count` + `count` left/right pairs) over a symbol table, accelerated by a 256-entry byte
   lookup; special codes emit zero runs. Each plane starts with its raw 9×9 DC band, then the
   VLC-coded wavelet coefficients. Planes: Y (width² ints), Cb, Cr (each (width/2)² when chroma
   is halved), separated by 0x80-byte gaps.
2. **Wavelet reconstruction** — per plane, a Haar-style pyramid: starting from the DC band edge
   (9), each pass butterflies the stored sum/difference subbands and merges (÷2), doubling the
   edge (9→18→36→72→144) until it reaches the plane width. Planes are square; only the first
   `height` rows are meaningful.
3. **YCbCr → RGB** — fixed-point integer transform (`r = y + cr·13/8 − 0x333`,
   `b = y + 2·cb − 0x400`, `g = 2y − y/4 − r/2 − b/4 − b/16`) through a clamp table that scales
   by ¼; chroma is upsampled 2× with a horizontal lerp on odd columns and row duplication.

Our implementation is a faithful port of the decompiled `Magic.exe` routines
(`ref: Magic.exe 0x4928AF, 0x505D9A, 0x505EB2, 0x506453, 0x506AD0, 0x506BF0, 0x4922CB, 0x492410,
0x4924E5, 0x4925E6`), via the MIT-licensed **ShandalarImageToolbox** (ogamespec /
CelestialAmber) — logged for the #10 community survey.

## Validation

Golden tests pin entry counts (944 / 943), known entry names/dimensions, and exact decoded RGBA
hashes for card 0 (Air Elemental, Smallart 144×116) and card 221 (Serra Angel, Medart 288×232);
both render correctly (visually verified). A full-archive decode smoke test confirms every entry
in both archives decodes without error.

The codec is verified decode-only; re-encoding is out of scope (the port never writes game files).
