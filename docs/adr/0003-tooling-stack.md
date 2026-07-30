# ADR-0003 — Tooling stack

**Status:** accepted · **Date:** 2026-07-30

## Context

No RE tooling was installed on this machine at project start beyond MinGW binutils and LLVM.
Ghidra 12.1.2 and a JDK 21 had been staged on the Desktop but were not wired up. The project
needs a disassembler, a scripting path for bulk extraction, a verification primitive, and a
progress store — all of which have to work unattended, because most of the work will be done by
agents in headless sessions.

## Decision

| Role | Choice | Why |
|---|---|---|
| Disassembler / decompiler | **Ghidra 12.1.2** headless | Free, scriptable, no licence server, already staged. Auto-analysis recovers 98.8% of `.text` on the first pass. |
| Bulk extraction | **Java `GhidraScript`** (`tools/ghidra/ExportFunctionInventory.java`) | Compiled on the fly by Ghidra; avoids the Jython-vs-PyGhidra churn in recent Ghidra versions. |
| Verification primitive | **Python + a hand-written x86 length decoder** (`tools/verify/funcbytes.py`) | Needs exact instruction boundaries to mask displacement fields; no dependency on an external disassembler in the hot loop. |
| Progress store | **SQLite** via `tools/progress/progress.py` | Single file, queryable, survives across sessions, easy to diff and back up. |
| Progress publishing | **objdiff report format** → decomp.dev | The ecosystem standard; see ADR-0004. |
| Scripting language | **Python 3.12** for permanent tooling; Node for throwaway probes | Python matches the RE ecosystem; Node was already present for the bootstrap PE parser. |

## Rationale

Ghidra headless was verified working end to end before anything was built on top of it:
15 binaries imported, analysed, and inventoried in roughly 3 minutes wall clock for the two
1.5 MB executables.

The x86 length decoder is deliberately hand-written rather than pulled from Capstone. It only
needs lengths and field offsets, not full disassembly, and keeping it dependency-free means the
verifier runs anywhere without a native extension. It was validated against real function bytes:
rel32 branch targets and in-image absolute addresses get masked, while stack displacements
(`8b 45 10`) and non-address immediates (`81 ec 24 01 00 00`) are correctly preserved.

## Not chosen

- **IDA Pro / Hex-Rays** — not installed, commercial. Ghidra is sufficient.
- **radare2 / rizin** — not installed; would add nothing over Ghidra here.
- **Capstone for the verifier** — a native dependency for a job that needs only lengths.
- **Modern MSVC 14.51 as the build compiler** — cannot reproduce 1996 codegen. The compiler
  question is the project's top risk and is decided separately in ADR-0005.

## Consequences

- `JAVA_HOME` must point at the staged JDK 21 for any Ghidra invocation;
  `tools/ghidra/run-headless.sh` sets it.
- The Store-alias `python` on `PATH` is a stub and does not work. All tooling must use the
  interpreter at `%LOCALAPPDATA%\Programs\Python\Python312\python.exe`, which the scripts default
  to and `MTGRE_PYTHON` can override.
- Ghidra projects live in `build/ghidra-projects/` and are disposable — they can be regenerated
  from `orig/` at any time with `refresh.sh --full`, so they are not committed.
