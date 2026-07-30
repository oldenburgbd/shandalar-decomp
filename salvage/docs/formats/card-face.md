# Card face composition

How the duel-style card face is reconstructed from original assets
(`Rendering/CardFace.cs`, `Rendering/CardFaceRenderer.cs`, `Rendering/ManaStrip.cs`).

> **Provisional.** The original's layout constants and frame-selection logic live in
> `Magic.exe` and have not been decompiled yet. Everything below was measured from the
> shipped assets and verified visually against in-game appearance; it will be confirmed or
> corrected when the card-draw routine is analyzed (#11). Deviations found then get quirk
> entries.

## Frame templates — `Cardart/Cardbk_*.pic`

All frames are **228x325**, external palette (render with `Duel.plogpal`). One per colour
plus land variants:

| Frame | Used for |
| --- | --- |
| `Cardbk_Black/Blue/Green/Red/White.pic` | Cards with that colour code |
| `Cardbk_Artifact.pic` | Colour code 3 (artifacts) |
| `Cardbk_Gold.pic` | Colour code 4 (multicolour, mostly Legends) |
| `Cardbk_Whiteland/Blueland/Blackland/Redland/Greenland.pic` | Basic lands (by name: Plains/Island/Swamp/Mountain/Forest) |
| `Cardbk_Antiquitiesland/Arabiannightsland/Darklandsland/Legendsland.pic` | Lands from those sets (set from Info.csv) |
| `Cardbk_Special.pic` | Fallback: other core-set lands, internal records (colour code 0) |
| `Cardbk_Legends.pic`, `Cardbk_Fallenempiresland.pic`, `Cardbk_Iceageland.pic` | **Unmapped** — Legends has different window geometry; FE/IA sets don't ship in v3 |
| `Cardback.pic` | Card back (not composed) |

Colour code 4 = gold and 0 = internal were confirmed from Cards.dat colour-byte
distribution (63 and 71 records respectively); see `CardColorCode`.

## Layout (measured from `Cardbk_Red.pic`)

| Region | Rect (x, y, w, h) | Content |
| --- | --- | --- |
| Name bar | 12, 3, 204, 14 | Card name left; mana cost pips right-aligned (14px cells) |
| Art window | 20, 22, 188, 158 | Medium art (288x232) nearest-neighbour scaled to fit |
| Type line | 20, 182, 188, 13 | e.g. "Summon Angel" |
| Text box | 24, 200, 180, 96 | Rules text + flavor text |
| Bottom bar | 12, 300, 204, 20 | Artist credit left; power/toughness right |

Art aspect (1.24) vs window aspect (1.19): the art is slightly stretched vertically, which
matches what the window demands; whether the original crops instead is a decomp question.

Text is **not** rasterized by `CardFaceRenderer` — the original drew card text with Windows
GDI fonts at runtime, so the viewer overlays text at these rects instead. Observed formatting
(matched by the viewer; exact font metrics deferred to Phase 4):

- **Name, type line, artist credit, power/toughness**: bold serif (Times-family).
- **Rules + flavor**: plain sans (Arial-family), sized up to fill the text box.
- **Text colour adapts to the frame**: white on dark frames (black/blue/red/gold), black on
  light ones — the viewer samples each region's frame luminance to decide.
- The expansion symbol (see below) is composited by Core onto the type line, right-aligned.

## Expansion symbols — `Cardart/Cardsets.pic`

330x15, external palette; **five 66px cells** in this left-to-right order: The Dark (moon),
Legends (column), Arabian Nights (scimitar), Antiquities (anvil), Astral (star). The symbol
is drawn right-aligned on the type line. Core-set (`Magic`) cards carry **no** symbol, which
matches real cards of the era; the set comes from Info.csv's "Card Set" column. Within each
cell the glyph is trimmed to its occupied columns so right-alignment is tight
(`Rendering/SetSymbols.cs`). Fixed 66px cells are used rather than gap-detection because the
Legends column and Arabian scimitar touch at their shared cell boundary.

## Mana symbols — `Cardart/Manasymbols.pic`

342x18, external palette; 19 cells of 18x18:

| Cell(s) | Symbol |
| --- | --- |
| 0 | X |
| 1–11 | numerals 0–10 |
| 12 | White |
| 13 | Red |
| 14 | Blue |
| 15 | Black |
| 16 | Green |
| 17 | Tap |
| 18 | empty |

The palette index found at the strip's top-left corner is the transparent background key.
Cost rendering order: X or generic numeral first, then W/U/B/R/G pips (`ManaStrip.CostCells`).
A cost with no coloured pips and no X draws its numeral even when 0; lands and internal
records draw no cost.

Related files not yet used: `Cardcounters.pic`, `Manastripes.pic`, `Abilities.pic`
(duel-time overlays).

## Golden facts

- Serra Angel (221) face SHA-256 (RGBA): `2480d99c45737955c5f1986a276cc0afeb3b21c915f80edc447cc79fc0c33b3b`
- One face of every frame kind renders without error (`CardFaceGoldenTests`).
