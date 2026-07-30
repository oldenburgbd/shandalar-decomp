# `Cards.dat` — binary card database

Status: **reversed and implemented** (issue #4). Parser: `Shandalar.Core.Cards.CardsDat`.

`Cards.dat` (340,025 bytes) holds the **gameplay** data for the real cards — mana cost, colour,
type, power/toughness, and ability/effect data — that `Master.csv` (display text only) lacks. It
also embeds the same text plus each card's **art filename**.

No public spec exists (the Manalink community's card editors are closed-source); this layout was
reversed empirically and verified against known cards.

## File layout

```
offset            size        section
0                 8           header
8                 999 * 152   fixed 152-byte card records (IDs 0..998, in order)
151856            188169      text section (null-terminated strings)
```

- **Header**: `uint32 recordCount` (999) + `uint32 textSectionSize` (188169). The text section
  therefore starts at `fileSize - textSectionSize = 151856`.
- **999 records** vs Master.csv's 1250 cards: IDs 0–998 are the real cards with gameplay stats;
  the higher Master.csv IDs (999–1249) are Shandalar effect pseudo-cards with no `Cards.dat` entry.

## Record layout (152 bytes)

Little-endian. Confirmed fields (byte offsets within the record); unconfirmed slots are almost
certainly ability/effect/AI data, to be finished alongside the effect-dispatch analysis (#12).

| Offset | Type | Field | Notes |
| --- | --- | --- | --- |
| 0 | u32 | **id** | 0-based, equals the record index |
| 4 | u32 | name offset | → text section (see below) |
| 8 | u32 | name2 offset | a second copy of the name |
| 16 | u32 | **colour code** | 1=black, 2=blue, 3=artifact, 5=green, 6=land, 7=red, 8=white |
| 20 | u32 | **type category** | 1=Artifact, 2=Enchantment, 3=Instant, 4=Interrupt, 5=Land, 6=Sorcery, 7=Creature |
| 28 | u32 | type-line offset | → text section |
| 40 | u8[9] | **mana cost** | pip array indexed by colour code (below) |
| 76 | u32 | art-file offset | → text section (e.g. `0000.WVL`) |
| 116 | u32 | rules-text offset | → text section |
| 120 | u32 | flavour offset | → text section |
| 124 | u32 | **power** | 0 for non-creatures |
| 128 | u32 | **toughness** | 0 for non-creatures |
| 136 | u32 | **ability flags** | bitmask; bit `0x4` = **flying** (other bits TBD, #12) |

Other u32 slots (offsets 12, 24, 32, 36, 44+, 64, 132, 140…) carry rarity/AI/effect data not yet
decoded; the parser preserves the raw 152 bytes so nothing is lost.

### Mana cost — pip array indexed by colour

The 9 bytes at offset 40 are a pip array **indexed by the colour code**: `[0]` = generic, and the
coloured pips sit at the index equal to that colour's code — `[1]`=black, `[2]`=blue, `[5]`=green,
`[7]`=red, `[8]`=white. Examples:

- Air Elemental `3UU` → `[3,0,2,0,0,0,0,0,0]` (generic 3, blue 2)
- Serra Angel `3WW` → `[3,0,0,0,0,0,0,0,2]` (generic 3, white 2)
- Craw Wurm `4GG` → `[4,0,0,0,0,2,0,0,0]` (generic 4, green 2)

Converted mana cost = generic + sum of coloured pips. **Quirk Q-003**: X spells store the generic
slot as the sentinel **40** (e.g. Fireball `XR`, Braingeyser `XUU`); treat generic == 40 as X.

## Text section

Null-terminated ASCII strings, concatenated. Each card's strings are located via the record's
offset fields (values are offsets **from the text-section start**, 151856). Per card:
`artfile`, `name`, `name2`, `type`, `rules`, `flavour`. The **art filename** (`NNNN.WVL`) is the
link to the card-art archives (issue #6). Rules/flavour duplicate Master.csv; the art filename is
the unique addition. **Names here are canonical** — Master.csv abbreviates them for its UI and has
some typos, so join the two databases by ID, not name (quirk Q-004).

## Validation

Golden tests assert decoded stats for known cards, e.g. Air Elemental = blue creature, 3UU, 4/4,
flying; Serra Angel = white, 3WW, 4/4, flying; Lightning Bolt = red instant, R; Craw Wurm = green,
4GG, 6/4. The full database decodes without error and joins to Master.csv on ID.
