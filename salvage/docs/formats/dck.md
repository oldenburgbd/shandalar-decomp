# `.dck` — deck files

Status: **reversed and implemented** (issue #5). Parser: `Shandalar.Core.Cards.DeckFile`.

`.dck` files are the game's decks. They live in two folders (132 total):

- **`Decks/`** — 55 numeric `NNNN.dck` files: the pre-built theme decks (e.g. `0010.dck` =
  "Lord of Fate").
- **`Playdeck/`** — 77 named files (e.g. `Ape Lord.dck`, `Arzakon.dck`): AI opponent /
  character decks.

They are **plain CRLF text**. Deck cards reference card **IDs** (the Master.csv / Cards.dat ID
space, 0..1249); every referenced ID in the shipped decks resolves. The card names on each line are
the canonical names (like Cards.dat), which differ from Master.csv's display names for some cards
(quirk Q-004) — so match on ID, not name.

## Layout

```
<header>
<blank line>
<main-deck card lines>
[ .v<Color>
  <sideboard card lines> ]   (repeated for each colour; Decks/ only)
```

**Header** — two styles:

- `Decks/`: a single **title line** (the deck's display name), e.g. `Lord of Fate (Bl/Wh, 4th Edition)`.
- `Playdeck/`: a run of **`;`-prefixed comment lines** carrying metadata — name, colours, author,
  e-mail, date, card count, edition, and a literal `;comments`. The first comment is the deck name.

**Card line**:

```
.<id>\t<count>\t<name>
```

A leading `.`, the decimal card **ID**, a tab, the decimal **count**, a tab, and the card **name**.
Example: `.126\t22\tIsland` (22× Island, card 126).

**Sideboard sections** (`Decks/` only — 45 of the 55 files): after the main deck, up to six
sections introduced by a marker line `.v<Colour>` where colour is one of
`None`, `Black`, `Blue`, `Green`, `Red`, `White`. The card lines after each marker are that
section's cards. These are the per-opponent-colour swap-in cards Shandalar's AI uses to tune a deck
against the player's colours (`.vNone` = the default group). Playdeck files and 10 of the Decks
files have no sideboard sections — just a main deck.

Marker disambiguation: a line beginning `.` followed by a **digit** is a card line; `.v…` is a
sideboard marker.

## Sizes

Main decks total 60–138 cards (median ~61). Per-card counts range 1–24 (basic lands dominate).

## Parser output

`DeckFile.Load` returns a `DeckList` with the deck `Name`, raw `Comments`, the `Main` entry list,
and a `Sideboards` map (`SideboardColor` → entries). Each `DeckEntry` is `(CardId, Count, Name)`.
`AllEntries` enumerates main + sideboard entries for validation.

## Validation

Golden test: all 132 shipped `.dck` files parse, and every referenced card ID resolves in the card
database. Names are not compared (Q-004).
