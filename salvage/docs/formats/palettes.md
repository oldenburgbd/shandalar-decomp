# Palettes — `.tr`, `.plogpal`, and how they pair with assets

Status: **reversed and implemented** (issue #3). Readers: `Shandalar.Core.Formats.TrPalette`,
`PlogPalette`, and `Palette` (overlay + `Palette.FromFile` dispatcher).

The game is 8-bit palettized: every `.pic`/`.spr` pixel is an index into a 256-colour table. A
`.pic`'s `M0`/`M1` block carries its own palette (see `docs/formats/pic.md`), but the many
external-palette `X0` images and all `.spr` sheets get their palette from one of these files.

## `.tr` — text override palettes

Plain text, one entry per line:

```
index  -  R  G  B  -  <trailing colour-match numbers…>
```

- The first number is the palette **index**; the next three are **R G B in 6-bit VGA** (0–63),
  scaled to 8-bit on load (`v8 = (v6<<2)|(v6>>4)`).
- The trailing numbers after the second `-` vary in count per line and are the game's
  colour-match / nearest-index ordering data (used to map truecolour art down to the palette).
  **They are not needed to render** and are ignored.

**`.tr` files are partial.** They define a subset of indices and are meant to be layered on a base
palette (typically a `.pic`'s embedded palette), the way the game swaps parts of the VGA palette
per screen. Coverage in this install:

| File | Defined entries | Role (inferred) |
| --- | --- | --- |
| `Todpal.tr` | 252 / 256 | Overworld ("time-of-day") — nearly complete, usable standalone |
| `Duelpalall.tr` | 236 | Duel screen (full variant) |
| `Duelpal.tr` | 213 | Duel screen |
| `Palall9.tr` | 93 | Partial shared overrides |
| `Advpal.tr` | 11 | Adventure-UI accent colours only — clearly an overlay |

Use `TrPalette.Load` for a standalone read (undefined entries stay black) or
`TrPalette.LoadOverlay(path, basePalette)` to layer a partial `.tr` onto a base.

## `.plogpal` — binary duel palette

`Duel.plogpal` (1028 bytes) is the duel creature/card-art palette in binary:

```
offset  size  field
0       4     header      byte1 = 3; bytes 2-3 = uint16 LE entry count (256)
4       N*4   entries     256 index-ordered entries of [R, G, B, flag]
```

RGB is **already 8-bit** (values exceed 63). The per-entry `flag` byte is 0 for the reserved index
0 and 1 for the rest; it is not part of the colour. Verified: decoding `Creatures640.pic`
(external-palette `X0`) with this palette renders the duel creature art in correct, natural colour.

## Asset → palette mapping (as established)

Confirmed by rendering real files:

- **Duel creature/card art** (`Creatures*.pic`, and other duel-table `X0` images) → `Duel.plogpal`.
- **Overworld item / location / actor sprites** (`Worlds.spr`, `Icons.spr`, `Compass.spr`, …) →
  `Winbak01.pic`'s embedded palette, a near-universal Shandalar sprite palette (community note).
- **Overworld map** → `Todpal.tr` (as a near-complete standalone palette), with day/night and
  screen-specific `.tr` overlays applied on top.

This is not yet exhaustive: the exact per-screen palette the original loads (base + which `.tr`
overlays, and any mid-screen palette swaps) is a rendering-time concern that will be pinned down
when the Phase 4 duel UI and Phase 5 overworld renderers need it. `Palette.FromFile` accepts any of
`.tr` / `.plogpal` / `.pic` so callers can supply whichever a given screen uses.

## References

`.tr` text handling cross-checked against `benprew/mp_pic_tools` (`shared.py: tr2pal`); `.plogpal`
layout reversed from the file and confirmed by rendering. Logged for the #10 community survey.
