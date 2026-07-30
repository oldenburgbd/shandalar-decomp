# Project Charter

## What we are building

A **matching decompilation** of MicroProse *Magic: The Gathering* (1997) — the Shandalar campaign
and the ManaLink multiplayer module: C source that, compiled with the period-correct toolchain,
reproduces the original binaries **byte for byte**.

## Definition of done

"Near 100% byte-for-byte" needs to be operationalised, because there are several distinct things
it could mean and they have very different difficulty. In increasing order of strictness:

| Level | Claim | Verified by |
|---|---|---|
| **L1** | Every function's code bytes match after masking relocation-dependent fields | `funcbytes.py` normalised hash per function |
| **L2** | Every function matches *exactly*, including absolute addresses and branch targets | raw hash per function |
| **L3** | Each `.obj` we produce is byte-identical to the original's contribution | per-object comparison at fixed link addresses |
| **L4** | The linked `.text` section is byte-identical | whole-section hash |
| **L5** | The whole PE file is byte-identical, including headers, `.idata` ordering, resources, and section padding | `sha256sum` of the output vs `orig/` |

**The project target is L4** — a byte-identical `.text`, `.rdata`, and `.data`, with L5 pursued
where it is cheap. L5 in full is partly a *linker archaeology* problem rather than a decompilation
problem: import-table ordering, resource-directory layout, section padding bytes, and the PE
timestamp are all linker/build-environment artefacts rather than properties of the source.

We track L1 as the working metric because it is the one that gives a usable per-function
signal during the long middle of the project. L4 is the acceptance gate.

### Two different originals

The shipped `Shandalar.exe` and `Magic.exe` are **Mok's unofficial ManaLink v1.3.2 (2001)
binary patches** of MicroProse's 1998 build — `Magic.exe` even carries an added read-write-execute
`.mok` section. They are not clean compiler output and cannot be produced by compiling *any*
source.

Therefore:

- **Primary target:** MicroProse's **1998 ManaLink 1.3 build**. This is a real build product and
  is a legitimate byte-for-byte goal.
- **Secondary:** Mok's v1.3.2 patch, modelled explicitly as a **post-link binary transformation**
  applied to the primary target, with its own separate verification.

Conflating these two would make the headline metric meaningless. See ADR-0002.

## Scope

**In scope** — the 15 MicroProse/ManaLink modules, 7,668 functions, 3,921,020 bytes of code:

- `Shandalar.exe`, `Magic.exe` (the two large targets, ~76% of all code)
- `Manalink.exe`, `Deck.exe`, `Facemaker.exe` (+ the `Facemaker/` variant)
- `Deckdll`, `Drawcardlib`, `Cardartlib`, `Cdtools`, `Statwin`, `Manalinkinterface`,
  `Magvid`, `Magsnd`
- `Gcconn.dll` — flagged: third-party TEN client, C++, MSVC 5.0. Different toolchain and
  different provenance. Lowest priority; may be declared out of scope (see open questions).

**Out of scope:**

- `Msvcrt.dll` / `Msvcrtd.dll` — stock Microsoft redistributables, not MicroProse code.
- Game assets (art, sound, video, card data). We will *document* their formats because the code
  cannot be understood without knowing what it reads, but we will not redistribute them and we
  are not reimplementing the content.
- Any attempt to make the game run on modern systems, add features, or "improve" anything.
  This is an archaeology project, not a port. A port may become a *downstream* project once a
  matching decompilation exists; it is not this project.

## Principles

1. **Measured, not remembered.** Every project fact comes with a re-runnable command. Training
   data about a 1996 MicroProse binary is thin and probably wrong.
2. **Honest metrics.** `matching` means byte-identical. Recording anything else as matching
   destroys the only signal we steer by.
3. **The originals are read-only.** Always.
4. **Recovered names beat invented names.** Exports, `assert()` strings, and format strings carry
   the original authors' vocabulary. Use it.
5. **Do not hand-decompile the CRT.** Library code gets matched by linking the right library.
6. **Shared core first.** The two big EXEs share ~70–79% of their code content. Work the common
   core once and both benefit.

## Why this is tractable

Real advantages this target has over a typical decompilation:

- **No relocations** in the two main EXEs (`.reloc` stripped, fixed base `0x400000`), so every
  absolute address is stable and directly comparable.
- **98.8% executable coverage** from Ghidra's initial auto-analysis — the code is
  well-formed, unobfuscated, and cleanly recovered.
- **The original source tree layout is recoverable** from embedded `assert()` paths, giving us
  real module and translation-unit boundaries instead of guesses.
- **Named export surfaces** on all nine DLLs give real API boundaries and real function names.
- **A large static-debug-CRT fraction** in four DLLs is library code, matched by linking.
- **A close precedent exists**: LEGO Island (`isle`) is a 1997 Windows game built with the same
  MSVC 4.2 toolchain, with an active matching decompilation to borrow methodology from.

## Known hard parts

- Obtaining and running **MSVC 4.2**. Without the period compiler, byte-matching is not possible
  and the project reduces to a functional reimplementation. This is the top risk.
- The `MPS_CODE` anti-piracy stub — self-modifying and not normal compiler output.
- The `.mok` patch section — hand-written assembly with no source.
- `Gcconn.dll` — C++, different compiler, third-party.
- Data/`.rdata` ordering and the huge `.data` BSS (4.2 MB virtual vs 155 KB raw) will make L4/L5
  matching sensitive to declaration order in a way code matching is not.

## Current state

Baseline established 2026-07-30: **0.000% matched**, 7,668 functions / 3,921,020 bytes tracked.
Infrastructure (Ghidra pipeline, progress DB, verifier core) is operational.
