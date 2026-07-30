# Decompilation Plan — Duels of the Planeswalkers v3.0

**Decided 2026-07-27, superseding the 2026-07-23 C#/.NET plan:** the objective is to turn the
binary into **usable, observable C**, and ease of doing so outranks every other consideration
— including what language the project eventually ships in. See `CLAUDE.md` for the full
statement and for why "observable" forces C as the target.

The previous plan's phases were organised around *features* (asset viewer → duel engine →
campaign). This one is organised around *coverage and verification*, because that is what the
new objective measures.

## 1. What we are decompiling

The source copy (`MtG_DotP_SotA/`) is the retail v3.0 release (14 Jan 1998) and contains no
source code. Measured facts about the binaries live in `docs/decompilation.md`; the short form:

- **C, not C++** (no RTTI, every leaked source path is `.c`), built with **MSVC 4.2**.
- **GDI only, no DirectX** — software rendering into a `CreateDIBSection` DIB.
- **No floating point anywhere** — the game is entirely integer/fixed-point.
- Stripped of symbols, but **no relocations**: fixed image base `0x400000`, stable addresses.
- CRT is dynamically linked, so `.text` is essentially all game code.

| Component | Files | Notes |
| --- | --- | --- |
| Shandalar adventure | `Shandalar.exe` (1.75 MB) | **The reference binary.** Overworld, cities, dungeons, quests, economy |
| Duel/rules engine + duel UI | `Magic.exe` (1.6 MB) | Rules, combat, ~1,250 card implementations, duel AI. **Carries a crack (`.mok`); get a clean copy before decompiling** |
| Support DLLs | `Deckdll.dll`, `Drawcardlib.dll`, `Cardartlib.dll`, `Magsnd.dll`, `Magvid.dll`, `Statwin.dll`, `Cdtools.dll` | ~500 KB combined |
| Multiplayer shell | `Manalink.exe`, `Gcconn.dll` | Lowest priority; likely cut |
| Card database | `Master.csv`, `Cards.dat`, and six auxiliary CSVs | **Formats already reversed** |
| Graphics | `.pic`, `.spr`, `Cardart/`, palettes (`.tr`, `.plogpal`) | **Formats already reversed and now confirmed against the binary** |
| Audio/video, text, fonts, saves | `Sound/`, `*.txt`, `*.ttf`, `MAGIC3.SVE` | Text and fonts directly reusable; save format still to reverse |

### Scope baseline (measured, not estimated)

From `survey.py` over `Shandalar.exe`:

| Metric | Value |
| --- | --- |
| Functions | 2,216 (2,190 game code; only 26 auto-identified as library) |
| Decompile failures | **0** |
| Unrecovered jump tables | **0** |
| Floating-point functions | **0** |
| Structurally clean | 95.2% |
| Total decompiled C | ~131,000 lines |
| Functions needing type recovery | 76.4% |

Distribution — **80 functions hold 25% of the code volume**, and that is where the difficulty
concentrates:

| Bucket | Functions | Lines |
| --- | --- | --- |
| <200 bytes | 875 | 14k |
| 200–800 | 751 | 33k |
| 800–3000 | 484 | 51k |
| >3000 | 80 | 32k |

The work is not extraction — extraction is already done and lossless. The work is
**comprehension** (types, names, structs) and **verification**.

## 2. Strategy

**Incremental hook-and-replace, verified by reccmp.** This is the OpenRCT2 model, chosen over
the isle "all-or-nothing byte-match" model because it keeps the game runnable at every commit:

1. **Understand** the target functions in Ghidra: apply types, name globals, read the
   disassembly where the pseudocode is ambiguous.
2. **Reconstruct** them as C in `decomp/`, annotated with the original address.
3. **Measure** with reccmp. A number per function, every build.
4. **Run** them: the original EXE is patched to load our DLL, which replaces functions one at
   a time. Both sides share the original's globals in place — which matters, because `.data`
   is **4.2 MB of global state** that never has to be migrated.

Two properties of the binary make this unusually cheap: the fixed image base means original
functions and globals sit at constant addresses, and the absence of DirectX means there is no
graphics API to emulate — only GDI calls we can let through untouched.

`docs/` remains a first-class artifact, but it is no longer the *durable* one: the C is. Docs
now record what a module does and why, not the full spec needed to reimplement it blind.

### Force multipliers

- The **C# tree is now an oracle**. Its `.pic`/`.spr`/`Cards.dat`/`.cat` parsers are verified
  against real files, so reconstructed C can be checked behaviourally against known-good output
  before its reccmp score is anywhere near 100%.
- **Compiled-in asserts** give `(file, line)` for 29 call sites, and the linker laid translation
  units out contiguously — so module boundaries in `.text` are known, not guessed.
- The **slightlymagic.net / Manalink modding community** documented formats and parts of
  `Magic.exe`. Survey before reversing from scratch.
- The 1998 rules are roughly **5th Edition** — pre-Sixth-Edition (interrupts, batches, no
  modern stack). Fidelity means keeping that model.

## 3. Phases

Exit criteria are measurable: coverage of `.text`, reccmp accuracy, and whether the game runs.

### Phase 0 — Formats and environment ✅ done
- All shipped asset formats reversed, documented, and independently implemented (C# oracle).
- Ghidra 12.1.2 project with auto-analysis, FID applied, PyGhidra harness, module attribution
  seeded from assert sites.
- MSVC 4.2 (linker 4.20) building, reccmp reporting per-function accuracy on a real module.

### Phase 1 — Method calibration on `sidlib` *(current)*
`sidlib` is the right proving ground: self-contained, at a known contiguous address range, and
its behaviour is independently verified by the C# oracle.
- Full type recovery for `sidlib`: graphics-page struct, `SprFrame`, font tables, file handles.
- Reconstruct `sprite.c`, `Fileio.c`, `lib.c`, `text.c`.
- **Exit criteria**: every `sidlib` function annotated and building; median reccmp accuracy
  ≥90%; a written estimate of hours-per-1000-lines that the remaining phases can be scheduled
  against.

### Phase 2 — Hook-and-replace harness
- Patch `Shandalar.exe`'s import table to load `shandalar_decomp.dll` at startup.
- Address-map header exposing the original's globals and not-yet-decompiled functions so
  reconstructed code can call into them.
- **Exit criteria**: the real game boots with at least one `sidlib` function served from our
  DLL, and plays indistinguishably.

### Phase 3 — Asset substrate complete
- `NedCard` (`Catalog.c`, `haar.c`, `Palette.c`) and the `MPS_CODE` `.pic` codec.
- `MPS_CODE` is **hand-written assembly** (1,192 bytes) and will not decompile to matching C —
  keep it as a hand-translated `.asm` module rather than fighting it.
- **Exit criteria**: all image, sprite, palette and card-art loading served from our code with
  the game running on it.

### Phase 4 — Game layer
- The `sid` modules: `Test.c` (WinMain/message loop), `glue.c`, `Minit.c`, `iniFile.c`.
- Name and type the `.data` globals that the game layer touches — the single biggest
  readability lever at this stage.
- **Exit criteria**: main loop, window/message handling and startup owned by our code.

### Phase 5 — Duel engine and card effects
The long tail, and where the 80 huge functions mostly live.
- Card-effect dispatch: establish how ~1,250 card behaviours map to code before porting any.
- Effects reconstructed in tranches (lands → vanilla creatures → keywords → uniques).
- `Ai.c` and the duel AI heuristics.
- **Exit criteria**: a full legal duel runs entirely on reconstructed code.

### Phase 6 — Adventure layer
- Overworld, cities, dungeons, quests, economy, world magic, campaign state.
- `MAGIC3.SVE` save format (read, ideally write).
- **Exit criteria**: new-game-to-victory campaign on reconstructed code.

### Phase 7 — Standalone
- Drop the original binary: our build *is* the game.
- Optional afterwards, and only once the C is complete and verified: retarget to another
  language or platform. That is a translation problem with a working, observable reference —
  which is the whole reason for doing it in this order.

## 4. Verification policy

- **Every `decomp/` change reports its reccmp delta.** A change that lowers the score is
  reverted or justified in the commit message. The number is the referee.
- **Behavioural cross-check against the C# oracle** for anything with a format it covers.
- **The game must still run** after every Phase-2+ change.
- **C# tree CI stays green** (build, test, `dotnet format`) so the oracle remains trustworthy.
  The `decomp/` build needs MSVC 4.2 and is verified locally, not in CI.

## 5. Risks

| Risk | Mitigation |
| --- | --- |
| Type recovery is the real cost and is easy to under-scope (76% of functions) | Phase 1 exists purely to measure the rate before committing to a schedule |
| 80 huge functions hold 25% of the code | Identify them early; they are almost certainly duel engine / AI / main loop, and should be scheduled as their own tranches |
| Chasing reccmp percentages instead of understanding code | Hard rule 4: never translate un-understood code. A well-understood 40% beats a guessed 80% |
| `MPS_CODE` hand-assembly will never match | Accepted: keep it as `.asm`. Small (1,192 bytes) and already fully understood |
| `Magic.exe` is cracked | Obtain a clean copy before Phase 5; `Shandalar.exe` is unaffected |
| Card-effect long tail | Dispatch-table analysis first, so we know the count of *distinct* behaviours rather than assuming 1,250 |
| Solo-project burnout | Phase 2 onward the game is always runnable and always improving; progress is a visible number |
