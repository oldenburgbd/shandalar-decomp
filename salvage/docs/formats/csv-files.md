# Secondary CSVs — Info, Legacy, Rogues, Rarity, Deckbuilder, Concise

Status: **documented and loaded** (issue #7). Loaders: `Shandalar.Core.Cards.InfoCsv` and
`AuxCsvs`. All are parsed line-scoped (`Csv.ParseLine`, the Q-001 lesson) and read as **Latin-1**
(1990s Windows text — quotes/ellipses mangle under UTF-8).

## Info.csv — per-card display & deck-builder data

Header: `ID,Card Name,Short Name,Card Set,Color,Type,Subtype,Group,Occuran.,Mana Costs,Pow,Tuff`.
One row per card for **all 1,250 IDs** (including effect pseudo-cards). Fields:

- **Color** includes `Grey` (lands) and `Gold` (multicolour) beyond the five colours.
- **Occuran.** = rarity (`Common`/`Uncommon`/`Rare`/`na`).
- **Mana Costs** = six characters `[W][G][R][B][U][generic]` — e.g. Serra Angel `200003` = 3WW,
  Air Elemental `000023` = 3UU, Craw Wurm `020004` = 4GG. A generic character of **`x`** marks an
  X spell (Fireball `00100x` = XR), matching Cards.dat's sentinel-40 encoding (Q-003).
- **Pow/Tuff** are numbers or `na` (loaded as nullable).

Golden-cross-validated: the decoded mana agrees with `Cards.dat` on 946 of the 999 shared IDs.
The 53 disagreements are (a) Info.csv typos on real cards (e.g. 0460 Sorceress Queen missing its
generic 1), (b) Shandalar token creatures (0885–0889: Wasp, Sand Warrior, …) where Cards.dat's
zero cost is correct, and (c) internal "None" records (0955+) whose Cards.dat mana bytes carry
non-mana parameter data. **Cards.dat is authoritative for gameplay**; Info.csv is display data.

## Legacy.csv — effect / legacy sub-card text

Header: `ID,Card Name,Damage Text,Effect Title,Effect Text,Legacy Title,Legacy Text`; IDs
**0000–0999**. Mostly empty; populated for cards that spawn separate damage/effect/"legacy"
sub-cards. **Q-005**: the final record (0999, "None") has an unterminated quote followed by binary
garbage — line-scoped parsing plus the numeric-ID filter skips the garbage continuation.

## Rogues.csv — duel opponents

Header: `Screen Name,Difficulty,Quote,Face Art`; 66 records (`rogue01.pic`–`rogue72.pic`, gaps).
The dueling-table/gauntlet opponent roster: display name, difficulty 1–5, taunt quote, portrait.

## Rarity.csv & Deckbuilder.csv — AI / deck-builder weight tables

Both are per-card tables keyed by name + game ID with per-set rarity letters (`C/U/R/-` for
Revised, 4th, 5th, Chronicles), mana-source flags, and — in Rarity.csv — a long tail of AI
deck-building weights ("AI Values", "Gains Value Versus", "Dependencies", "Class Value
Modifiers"…) plus tuning rows ("AAKnob…"). Structure: **Rarity.csv has 3 header rows,
Deckbuilder.csv has 2**, then one row per card.

These are loaded as **raw rows** (`AuxCsvs.LoadRawTable`) — enough for set/rarity lookups. The
deep AI-weight semantics belong with the Phase 2 dispatch/AI analysis (#12) and are deliberately
deferred; decoding them without the AI code as reference would be guesswork.

## Concise.csv — numeric triples (partially understood)

Headerless, 511 lines of space-separated `cardId group value` triples where `group` ∈ {1,2,3}
and the first rows are basic lands with large values (Swamp/Island/Forest…). Likely a
deck-generation or card-pool weighting table. Loaded as typed triples
(`AuxCsvs.LoadConcise`); semantics to be confirmed against the decompiled deck generator (#12) —
documented here as **partially understood** rather than guessed at.
