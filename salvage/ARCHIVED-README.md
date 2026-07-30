# Shandalar Port

A modern C#/.NET reimplementation of **Magic: The Gathering — Duels of the Planeswalkers v3.0**
(MicroProse, 1998), including the Shandalar adventure game, ported with deep fidelity to the
original binaries.

> **This repository must remain private.** The port is decompilation-assisted (Ghidra analysis of
> `Magic.exe` / `Shandalar.exe`), which makes translated code a derivative work of the original
> binaries, and all card names/text/art are Wizards of the Coast IP. Personal use only.

## Layout

| Path | Purpose |
| --- | --- |
| `CLAUDE.md` | Agent handoff guide — rules, commands, current state |
| `docs/migration-plan.md` | The full phased migration plan — read this first |
| `docs/formats/` | Reverse-engineered file-format documentation (`.pic`, `.spr`, `Cards.dat`, …) |
| `src/Shandalar.Core/` | Clean-room-quality C# engine and data-model code |
| `src/Shandalar.Cli/` | `shandalar` CLI: inventory, extraction, and card-database tools |
| `tests/` | xUnit tests — synthetic fixtures always run; golden tests against a real install are opt-in |
| `reference/` | Decompilation workspace (Ghidra exports, annotated C). Never `using`'d by `src/` |

## Prerequisites

- .NET 10 SDK
- A retail install/copy of *Duels of the Planeswalkers* v3.0 (not committed to this repo)

Point the tooling at your game files:

```powershell
$env:SHANDALAR_GAME_DIR = "c:\Users\Ben\Desktop\Shandalar\MtG_DotP_SotA"
```

## Build & test

```powershell
dotnet build
dotnet test          # integration tests auto-skip if SHANDALAR_GAME_DIR is unset
dotnet run --project src/Shandalar.Cli -- scan    # inventory your game install
dotnet run --project src/Shandalar.Cli -- cards   # dump the card database
```

CI (GitHub Actions) builds and runs the fixture-based test suite on Windows and Linux for every
push and pull request.
