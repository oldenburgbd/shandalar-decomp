# `.spr` — MicroProse SPR sprite format

Status: **reversed and implemented** (issue #2). Parser: `Shandalar.Core.Formats.Spr`.

`.spr` is the sprite-sheet format: a container of one or more small palettized images with
per-row transparency, used for map tiles, item/treasure icons, location icons, UI widgets,
animation frames (moon phases, clock, days of the week), and overworld actors. The base game
directory ships 35 `.spr` files; the `Spr800/` and `Spr1024/` folders hold higher-resolution
variants (344 `.spr` total).

Unlike `.pic`, **SPR frame data is not compressed** — the only encoding is a compact per-row
run scheme for transparency. Frames carry palette **indices**; the palette is **external**
(a `.tr` file or a `.pic`'s embedded palette — see issue #3 for the asset→palette mapping).

Verified against the real install: `Worlds.spr` (60 item sprites), `Icons.spr` (24 location
icons), `Compass.spr`, `S_Moon.spr` (16 moon phases), `Days.spr` (7) and others decode to correct
frame counts and shapes, and render in full colour using the `Winbak01.pic` palette (a
near-universal palette for Shandalar sprites, per community notes).

## Container

A file is a sequence of frames, each `[uint32 size][frame data]`, terminated by a size field of
`0xFFFFFFFF`. `size` is the total frame length **including its 16-byte header**, so the next
frame begins at `frameStart + size`. (Some files have a few trailing bytes after the terminator;
ignore them.)

## Frame header (16 bytes)

```
offset  size  field
0       4     size              uint32 LE — whole frame incl. this header
4       2     width             uint16 LE
6       2     height            uint16 LE
8       2     unknown1          uint16 LE (purpose unconfirmed; likely an anchor/registration point)
10      2     unknown2          uint16 LE (   ""   )
12      2     empty_lines_above uint16 LE — count of fully transparent rows at the top
14      2     cutoff_offset_y   uint16 LE — bottom transparent-region marker (unused by the decoder)
```

The first `empty_lines_above` rows are entirely transparent and have no encoded data. Encoded
rows then follow for `y` in `[empty_lines_above, height)`.

## Per-row encoding

For each encoded row, reading from the frame's data cursor (stop if it reaches the frame end):

1. **Skip padding**: read bytes while they equal `0xFF`; the first non-`0xFF` byte is
   `leading_transparent` — the number of transparent pixels at the row's left edge.
2. **Control byte** `c`:
   - if `c == 0xFE` or `c == 0xFF`: read one more byte → `data_pixels`;
   - else: `data_pixels = c`.
3. Read `data_pixels` raw index bytes. Within these, an index of **`0x00` is transparent**
   (embedded transparency), as is palette index 0 generally.
4. The row is: `leading_transparent` transparent pixels, then the `data_pixels` bytes, then the
   remaining `width - leading_transparent - data_pixels` pixels transparent.

Any rows not produced before the frame end (and all fully-skipped area) are transparent. Our
parser returns each frame as `width*height` **indices** plus a parallel **transparent mask**;
`ToRgba(palette)` writes alpha 0 for transparent pixels.

## Palette

SPR carries no palette. Supply one from:
- a `.tr` text palette (`index - R G B - …` per line; **6-bit** RGB, scale ×4 to 8-bit — see
  `Shandalar.Core.Formats.TrPalette`), or
- a self-contained `.pic`'s embedded palette (e.g. `Winbak01.pic`), via `Pic.Decode(...).Palette`.

Which palette pairs with which sprite set is tracked in issue #3; `Winbak01.pic` works for a
broad range of Shandalar sprites and is a good default.

## What the original's own row walker says (0x0057c450), and what testing it showed

`Spr_GetFrameExtent` measures a frame's horizontal extent by walking the row data, so it is a
second statement of the encoding — from the game rather than from our reader. Two differences
were visible in its disassembly, and **both have now been tested rather than argued about**:
`decomp/test/difftest.ps1` runs the reconstructed C and this decoder over all 344 `.spr` files
and diffs the results frame by frame.

**Result: 924 frames, and every header field agrees on all 924.** `width`, `height`, the two
unknowns, `empty_lines_above` and `+14` are identical everywhere, which also verifies
`Spr_LoadFile` — both readers find the same frames at the same offsets.

**The `0xFF` question did not materialise.** The original reads one start-x byte per row and
treats `0xFF` as "row empty, next row", while this decoder loops past `0xFF` bytes as padding.
On the shipped data the two readings never diverge: only 3 of 924 frames disagree on extent, and
all 3 are explained by the row count below, not by `0xFF`. The earlier warning here has been
downgraded accordingly — **do not change the decoder**, there is no observed defect.

**`+14` is the row count the original walks, but it is not always `height - empty_lines_above`.**
The original reads it as a *signed* short (`movsx`) and uses it directly as the loop count. It
equals `height - empty_lines_above` in 921 of 924 frames. In three it is smaller:

| frame | width | height | empty_lines | +14 | extent, original | extent, this decoder |
| --- | --- | --- | --- | --- | --- | --- |
| `icons.spr` #22 | 96 | 72 | 0 | 4 | 20..51 | 0..71 |
| `icons.spr` #23 | 96 | 72 | 0 | 4 | 20..51 | 0..71 |
| `tsprite2.spr` #29 | 31 | 31 | 26 | 2 | 11..20 | 8..23 |

**The renderer settles it, and not in this decoder's favour.** `Spr_DrawScaled` (0x0057bfa0)
runs its row loop while the source row is below `empty_lines_above + <+14>`:

```c
while (g_sprScaleYAcc >> 16 < rows + emptyLines) { ... }   /* rows = frame->+0x0e */
```

So the game draws exactly `+14` source rows — the same count the extent scanner walks. **Both of
the original's own row walkers agree with each other, and it is this decoder that is the
outlier**, decoding `height - empty_lines_above` rows instead.

An earlier revision of this section reasoned the other way, from the fact that our renders of
those three frames look like clean, complete icons. That was the wrong inference: they are
numbered placeholder frames (`icons.spr` #22 and #23 literally render as blobs labelled "22" and
"23"), and looking coherent is not evidence that the game would draw them that way. It would
draw 4 rows of them.

So a frame's content height is `empty_lines_above + <+14>`, which may be **less** than `height`,
and this decoder over-reads on the three frames listed above. All three appear to be unused
placeholder art, which is why nothing visible has ever broken. **Changing the decoder to honour
`+14` is now justified by the binary** — it is a behaviour change to verified code, so it wants
its own before/after render comparison rather than being folded in here.

## Notes / open questions

- `unknown1`/`unknown2` are captured but unused. They are most likely the sprite's draw anchor
  (hotspot) relative to a tile origin; confirm when the overworld renderer needs correct
  placement (Phase 5).
- The sheet-layout heuristics in the reference `spr2png.py` (grid columns per filename) are a
  display convenience, **not** part of the format; our parser exposes raw frames and leaves
  layout to the caller.

## References

Reversed with the `benprew/mp_pic_tools` reference (`spr_format.md`, `spr2png.py`) logged for the
#10 community survey.
