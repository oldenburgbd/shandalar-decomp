# Phased Plan

Written 2026-07-30, after surveying the binaries and salvaging the archived first attempt.

## Where we actually are

This is **not** a greenfield project. An earlier attempt got a real matching decompilation
working and reached, by its own last measurement (`5f19457`, 2026-07-29):

- **979 annotated functions**, 784 scored by reccmp, **194 byte-exact**
- a working MSVC 4.2 build, a reccmp verification loop, and a runnable host EXE
- complete file-format documentation and a C# parsing oracle

That work is salvaged into `salvage/` and imported: **808 functions in our inventory now carry
the original authors' names, a translation unit, and a subsystem** — 321,052 bytes of
reconstruction, 193 of which were previously measured byte-exact (32,218 bytes).

**The headline metric still reads 0.000%, and that is correct.** Those scores were measured
against a build we cannot currently reproduce. Nothing is counted until it is re-measured here.

## The one thing blocking everything

**MSVC 4.2's INCLUDE headers and full LIB set are missing from this machine.**

What we have:

| Component | State |
|---|---|
| `CL.EXE`, `C1.EXE`, `C2.EXE`, `MSPDB41.DLL` | present — `C:\vc42verify\bin` |
| `LINK.EXE` | present — **version 4.20.6164**, the exact linker that built the originals |
| Runs natively on Windows 11 x64 | **yes** — verified, a test EXE builds and runs |
| Tolerates spaces in paths | verified separately in `C:\vc42 space test` |
| `LIB\` | partial — `KERNEL32`, `USER32`, `LIBCD`, `LIBCMTD`, `MSVCRT`, `OLDNAMES` |
| `INCLUDE\` | **missing entirely** |
| `GDI32.LIB` | **missing** (the build needs it) |

The archived build expected a full install at `C:\Tools\msvc42\MSDEV`, which no longer exists,
along with `..\ghidra-work` (its Ghidra project, venv and helper scripts).

This is not substitutable. The reconstruction depends on MSVC 4.2's *own* headers — `sidcore.h`
explicitly `#undef`s `_fileno` because "MSVC 4.2's stdio.h defines `_fileno` as a macro, but the
original binary calls the real imported function". Modern headers would change codegen and
invalidate every score.

**Until this is resolved, no byte can be verified and the metric cannot move.** Everything in
Phase 1 is about closing it.

## Phase 0 — Infrastructure ✅ complete

- Ghidra 12.1.2 headless pipeline; 15 binaries analysed; 98.8% `.text` coverage
- Progress database (7,668 functions / 3,921,020 bytes), forward-only status ladder
- `funcbytes.py` — PE function extraction and x86-length-decoder normalisation
- objdiff v2 report emitter; local decomp.dev instance publishing at
  `http://localhost:3000/mtg-re/shandalar`
- `refresh.sh` one-command refresh, wired to session hooks, with a tripwire on `orig/`
- reccmp 0.1.6 installed
- Archived first attempt salvaged and imported

## Phase 1 — Restore the build (the critical path)

**Exit criteria:** `salvage/decomp` builds with MSVC 4.2, and reccmp reproduces a score for at
least one previously-100% function.

1. **Obtain a complete MSVC 4.2** — `INCLUDE\` and the full `LIB\`. This needs a human decision;
   see *Open questions*. The compiler binaries we have are useless without headers.
2. Reconstitute the layout the archived `build.cmd` expects (`%MSVC42%\BIN|INCLUDE|LIB`).
3. Build. Expect to fight the documented NMAKE 1.x relink bug — `build.cmd` already loops until
   convergence; do not "simplify" that away.
4. Run reccmp against `orig/Shandalar.exe` and compare per-function scores to
   `salvage/decomp/progress/history.jsonl`. Any divergence means the toolchain is not the same
   one, and must be chased down before trusting anything.
5. Wire reccmp's output into `progress.py` so match status is set **mechanically**, not by hand.
   Promote the 193 to `matching` only as reccmp confirms each.

**Expected result on completion: roughly 0.8% of the product**, recovered rather than earned.

## Phase 2 — Re-establish the working loop

**Exit criteria:** a single command builds, scores, diffs against a saved baseline, and updates
both the local dashboard and decomp.dev.

- Port `salvage/decomp/test/score.ps1`'s `-Against` behaviour into the main tooling. It is the
  most important tool the archive has: editing any translation unit re-rolls MSVC 4.2's register
  allocation across all of it, so a change aimed at one function routinely moves its neighbours,
  and the aggregate alone hides which way.
- Re-run the Ghidra analysis the archive depended on. Ours already covers the same binaries;
  what is missing is the decompiled-C export (`ghidra-work/out/*.c`) that made reconstruction
  cheap. Regenerate it headlessly.
- Fold `salvage/docs/quirks.md` (Q-numbered oddity register) and `salvage/docs/decompilation.md`
  into the live docs.

## Phase 3 — Breadth-first reconstruction

**Exit criteria:** 25% of `Shandalar.exe` by bytes.

The archive's strategy is empirically justified and should be kept:

- **Work the game layer, not `sidlib`, and build it `/Od`.** The game layer is unoptimised, which
  removes the register-allocation guessing that caps every `sidlib` score — the first game
  function attempted was byte-exact on the first build at 3,919 bytes.
- **Breadth, not polish.** Matched bytes over total bytes is the metric. Grinding a 608-byte
  function from 86% to 100% is worth ~85 bytes; a new 500-byte function at 80% is worth ~400.
- **Do not chase register permutations.** Three sessions and ~20 measured variants moved none of
  them. Record the residual and move on. The one carve-out is `/Od` locals in swapped stack
  slots, where renaming the identifier is a single-build lever (Q-021).
- Prioritise functions shared between `Shandalar.exe` and `Magic.exe`. Our measurement puts
  content overlap at 69–79%; the archive counted 595 shared functions, only six in `sidlib`.

## Phase 4 — Close out `Shandalar.exe`, then the DLLs

**Exit criteria:** `.text` of `Shandalar.exe` byte-identical (charter level **L4**).

- Classify and mark CRT/compiler-helper code as `library` rather than decompiling it. This is
  substantial in `Drawcardlib`, `Cardartlib`, `Cdtools` and `Gcconn`, which statically link the
  *debug* CRT.
- Resolve data and `.rdata` ordering, which is a declaration-order problem in the source and the
  main remaining obstacle to L4 once code matches.
- Then the DLLs, smallest first: `Magsnd` (19 KB), `Manalinkinterface` (23 KB), `Statwin` (33 KB),
  `Magvid` (36 KB).

## Phase 5 — The hard remainder

- `MPS_CODE` — the 1,192-byte self-modifying anti-piracy stub present in five modules.
- Mok's `.mok` patch, modelled as a post-link transformation (ADR-0002).
- `Gcconn.dll` — C++, MSVC 5.0, third-party TEN client. Different toolchain; may be dropped.
- L5 whole-file matching: import-table ordering, resource layout, section padding, PE timestamp.

## Effort

Honest, and wide. The archive is the only real data point: it reached 194 byte-exact functions
and "product past 8%" over roughly 74 commits.

| Phase | Work | Confidence |
|---|---|---|
| 1 | days — but **gated entirely on obtaining MSVC 4.2**, which may be hours or may be impossible | high once unblocked |
| 2 | days | high |
| 3 | months of sustained agent work | medium |
| 4 | months; L4 is a genuine research problem | low |
| 5 | open-ended | low |

The `/Od` game layer is the reason to be optimistic: unoptimised code matches far more readily
than optimised code, and it is the majority of the corpus by bytes.

## Top risks

| Risk | Mitigation |
|---|---|
| **MSVC 4.2 headers unobtainable** | Blocks byte-matching entirely. Fallback: reconstruct headers from the CRT libs' symbols plus a period SDK, and accept that some codegen will not match. Degrades the project to functional reimplementation. |
| Toolchain obtained but not bit-identical to the original build | Re-scoring the archived 193 is the canary; divergence means wrong compiler build. |
| Metric inflation | `matching` set only by reccmp output, never by hand. |
| Losing the archive's hard-won knowledge | `salvage/` is committed; its findings are being folded into the live docs rather than left to rot. |
| Register-allocation rabbit holes | The archive's closed-list is authoritative: do not reopen it. |

## Open questions for the human

See the summary in the session; the load-bearing one is **where to obtain a complete MSVC 4.2**.
