# mtg-re — matching decompilation of MicroProse *Magic: The Gathering* (1997)

A byte-accurate reverse engineering project targeting the MicroProse *Magic: The Gathering*
PC game — the **Shandalar** campaign — together with the **ManaLink** multiplayer module.

The goal is a **matching decompilation**: C source that, compiled with the period-correct
toolchain, produces byte-identical output to the original 1998 binaries. Not a remake, not a
port, not a reimplementation.

**Status: infrastructure complete, 0.000% verified here — but 808 functions of reconstruction
salvaged from an earlier attempt and awaiting re-verification.**

| | |
|---|---|
| Modules in scope | 15 |
| Functions | **7,668** |
| Code bytes | **3,921,020** |
| Verified matching | **0** (0.000%) |
| Reconstruction salvaged | **808 functions / 321,052 bytes**, of which **193 were previously measured byte-exact** |
| Original compiler | Microsoft Visual C++ **4.2** (linker 4.20); MSVC 5.0 for `Gcconn.dll` |

> **Toolchain: solved and validated.** Genuine Visual C++ 4.2 Enterprise is installed at
> `C:\Tools\msvc42\MSDEV` and runs natively on Windows 11 x64. `C2.EXE` MD5 matches the canonical
> known-good build. Rebuilding the archived tree with it reproduces that tree's reccmp scores
> **exactly** — 194 byte-exact functions, per-function percentages equal to two decimal places.
> See [docs/02-toolchain.md](docs/02-toolchain.md).

The 0.000% is deliberate and honest. `salvage/` is kept as **reference**, not promoted to the
live source tree, so the project's own reconstruction starts empty. The salvaged work supplies
names, translation units, subsystem structure and a proven set of compiler flags — the metric
will move as functions are re-derived and reccmp confirms them here.

## Start here

| Document | What it covers |
|---|---|
| [docs/00-charter.md](docs/00-charter.md) | Goals, definition of done, scope, principles |
| [docs/01-binary-inventory.md](docs/01-binary-inventory.md) | Every measured fact about the binaries |
| [docs/04-plan.md](docs/04-plan.md) | Phased plan with entry/exit criteria |
| [CLAUDE.md](CLAUDE.md) | Operating rules — **read before touching anything** |
| [docs/adr/](docs/adr/) | Architecture decision records |

## Layout

```
orig/          pristine, hash-verified copies of the shipped binaries  (never modified, never committed)
src/           reimplemented C, organised by the recovered original source tree
include/
config/        symbol maps, translation-unit splits
tools/
  ghidra/      headless import + function-inventory export
  verify/      function byte extraction, x86 normalisation, match checking
  progress/    progress database, metrics, dashboard, objdiff report emitter
reports/       generated: PROGRESS.md, progress.json, report.json, dashboard.html
progress/      progress.db — the project's source of truth for what is done
build/         Ghidra projects and build output (disposable)
docs/          charter, inventory, plan, methodology, ADRs, notes
```

## Quick start

```bash
PY="$HOME/AppData/Local/Programs/Python/Python312/python.exe"

# refresh every metric (fast — no re-analysis)
bash tools/progress/refresh.sh

# ...including a full Ghidra re-analysis of all 15 binaries (~4 min)
bash tools/progress/refresh.sh --full

# where do things stand
"$PY" tools/progress/progress.py stats
```

`reports/dashboard.html` is a self-contained local dashboard. `reports/report.json` is an
[objdiff](https://github.com/encounter/objdiff)-format report for
[decomp.dev](https://github.com/encounter/decomp.dev) — see [docs/05-decomp-dev.md](docs/05-decomp-dev.md).

## Inspecting a function

```bash
# raw + normalised bytes for one function
"$PY" tools/verify/funcbytes.py dump orig/Shandalar.exe 4bea85 9019

# shared code between the two main executables
"$PY" tools/verify/funcbytes.py compare \
    orig/Shandalar.exe reports/ghidra/Shandalar.exe.functions.csv \
    orig/Magic.exe     reports/ghidra/Magic.exe.functions.csv
```

Normalisation masks rel32 branch targets and in-image absolute addresses using a real x86 length
decoder, so two builds of the same source compare equal even at different link addresses, while
genuine code differences still show up.

## What makes this target tractable

- **No relocations** in the two main executables — fixed base `0x400000`, so absolute addresses
  are stable and directly comparable.
- **98.8% of `.text` recovered** into functions by Ghidra's first analysis pass. The code is
  unobfuscated and cleanly formed.
- **The original source tree is partly recoverable** from `assert()` paths still embedded in the
  binaries — `D:\NewMagic\sources\sidlib\`, `...\multiplayer\sid\`, `...\FamiliarWS\` and more,
  giving real module boundaries instead of invented ones.
- **Named exports** on all nine DLLs supply real API names and real subsystem seams.
- **The two big executables share ~69–79% of their code content**, so together they are roughly
  1.2× the work of one, not 2×.

## What makes it hard

- **Obtaining and running MSVC 4.2.** Without the period compiler there is no byte matching, and
  the project degrades to a functional reimplementation. This is the top risk.
- The shipped `Shandalar.exe` / `Magic.exe` are **third-party binary patches** (Mok's ManaLink
  v1.3.2, 2001), not clean build output — `Magic.exe` even has an added RWX `.mok` section.
  See [ADR-0002](docs/adr/0002-target-the-1998-build-not-the-mok-patch.md).
- An `MPS_CODE` anti-piracy stub that is self-modifying and not normal compiler output.
- A 4.2 MB `.data` section (against 155 KB raw) whose ordering will matter for full section
  matching.

## Legal

This repository contains **no** original game code, assets, or binaries, and none may be
committed — `.gitignore` enforces that and should not be weakened. `orig/` holds local
hash-verified copies for verification only; they are not redistributed. All game content remains
the property of its rights holders (MicroProse / Wizards of the Coast / Hasbro).

**Posture: eventual public release**, in the manner of the LEGO Island (`isle`) decompilation.
Two things follow, and both are constraints on day-to-day work rather than end-of-project
cleanup:

- Reconstructed C is a derivative work of the original binaries. That is inherent to a matching
  decompilation and is accepted deliberately, in exchange for verifiability.
- Nothing game-derived beyond *facts* may enter the repo — addresses, format documentation,
  hashes, record counts, and short excerpts inside assertions. No art, sound, card text, or
  extracted data, ever. Test fixtures must be synthetic.
