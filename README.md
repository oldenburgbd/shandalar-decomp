#decompilation of MicroProse *Magic: The Gathering* (1997)

A byte-accurate reverse engineering project targeting the MicroProse *Magic: The Gathering*
PC game — the **Shandalar** campaign — together with the **ManaLink** multiplayer module.

The goal is a **matching decompilation**: C source that, compiled with the period-correct
toolchain, produces byte-identical output to the original 1998 binaries. Not a remake, not a
port, not a reimplementation.

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
