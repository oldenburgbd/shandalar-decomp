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

## The toolchain risk is closed

Genuine **Visual C++ 4.2 Enterprise Edition** media was supplied and is installed at
`C:\Tools\msvc42\MSDEV` — `BIN`, `INCLUDE` (401 entries), `LIB` (121 files), `CRT`. It runs
natively on Windows 11 x64.

Validated, not assumed:

- `CL` reports **10.20.6166**, `LINK` reports **4.20.6164** — the exact linker stamped into the
  original binaries.
- `C2.EXE` MD5 `DCD69F1DD28B02DD03DD7ED02984299A` — byte-identical to the canonical known-good
  MSVC 4.2 back end the `isle` project standardised on.
- **The archived tree rebuilds and re-scores exactly**: 194 byte-exact functions against the
  archive's recorded 194, with per-function percentages equal to two decimal places
  (`Spr_GetFrameExtent` 68.89%, `Spr_LoadFileMax` 66.67%). Aggregate accuracy 85.94%.

That last point is what matters. It proves this install behaves identically to the one that
produced the archived results, so every empirical finding in `salvage/ARCHIVED-CLAUDE.md` about
MSVC 4.2 codegen — the flag set, the header-isolation rule, the declaration-order levers —
transfers directly rather than needing rediscovery. Details in
[02-toolchain.md](02-toolchain.md).

## Decisions taken

| Question | Decision |
|---|---|
| Role of the archived attempt | **Reference only.** `salvage/` is not promoted to the live source tree; functions are re-derived here. Its names, TU structure, flags and quirks register are used as a guide. |
| Publishing | **Public eventually**, in the manner of `isle`. Facts and reconstructed C only — never assets. |
| Scope | **All 15 modules stay in the denominator**, `Gcconn.dll` scheduled last (third-party, C++, MSVC 5.0). |

## Phase 0 — Infrastructure ✅ complete

- Ghidra 12.1.2 headless pipeline; 15 binaries analysed; 98.8% `.text` coverage
- Progress database (7,668 functions / 3,921,020 bytes), forward-only status ladder
- `funcbytes.py` — PE function extraction and x86-length-decoder normalisation
- objdiff v2 report emitter; local decomp.dev instance publishing at
  `http://localhost:3000/mtg-re/shandalar`
- `refresh.sh` one-command refresh, wired to session hooks, with a tripwire on `orig/`
- reccmp 0.1.6 installed
- Archived first attempt salvaged and imported

## Phase 1 — Toolchain ✅ complete

MSVC 4.2 installed, verified by version, by hash, and by exact reproduction of the archived
reccmp scores. reccmp 0.1.6 installed. See [02-toolchain.md](02-toolchain.md).

## Phase 2 — Stand up our own build and scoring loop

**Exit criteria:** one command builds `src/`, scores it with reccmp, diffs against a saved
baseline, and updates both the local dashboard and decomp.dev.

1. Create the project's own `src/` + `include/` + Makefile, modelled on the archive's structure
   but written here. Keep its two hard-won structural rules:
   - **One header per module.** Every declaration visible to a translation unit perturbs MSVC
     4.2's register allocation across that whole unit — adding six unused prototypes to a shared
     header once took a function from 100% to 31% with its source untouched.
   - **`build.cmd` loops NMAKE until convergence.** NMAKE 1.x does not relink the object it just
     compiled, which silently scores the *previous* build.
2. Add `reccmp-project.yml` pinning `Shandalar.exe`, `Deck.exe` and `Facemaker_sub.exe` by
   SHA-256 (all three verified identical to the archive's pins).
3. **Wire reccmp output into `progress.py` so `matching` is set mechanically, never by hand.**
   This closes the honesty gap noted in ADR-0004: until it exists, the metric depends on
   discipline rather than machinery.
4. Port the archive's `score.ps1 -Against` behaviour. It is the single most useful tool it had:
   editing any translation unit re-rolls register allocation across all of it, so a change aimed
   at one function routinely moves its neighbours, and the aggregate alone hides which way.
5. Regenerate Ghidra's decompiled-C export, which is what made reconstruction cheap
   (the archive's `ghidra-work/out/*.c`; that directory is gone, our Ghidra project is not).

Also fold `salvage/docs/quirks.md` (the Q-numbered oddity register) and
`salvage/docs/decompilation.md` into the live docs, and carry over `salvage/docs/formats/` —
nine file-format specifications (`.pic`, `.spr`, `Cards.dat`, `.dck`, palettes, the CAT archive,
the CSVs, card faces) that are already verified against real files by the C# oracle in
`salvage/csharp-oracle/`. That oracle is not the product and should not be invested in, but it
is the fastest way to check whether a piece of reconstructed C behaves correctly.

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
| 0–1 | ✅ complete | — |
| 2 | days | high |
| 3 | months of sustained agent work | medium |
| 4 | months; L4 is a genuine research problem | low |
| 5 | open-ended | low |

The `/Od` game layer is the reason to be optimistic: unoptimised code matches far more readily
than optimised code, and it is the majority of the corpus by bytes. The archive's first game
function was byte-exact on its first build, at 3,919 bytes.

The honest calibration point against over-optimism is `isle`, the closest analogue — a 1997
Windows game on this same compiler. After 38 months and 41 contributors it reports 100%
*implemented* but **99.61% accuracy**, and states plainly that its binaries are not a
byte-for-byte match. Its one unsolved blocker is MSVC 4.2 codegen entropy: unrelated header
edits pseudo-randomly change codegen within a translation unit, affecting roughly 5% of
functions, cause unknown. Expect the same tail here. "Near 100%" should be read as *per-function
equality for the large majority*, not whole-file SHA-256 identity across the set.

## Top risks

| Risk | Mitigation |
|---|---|
| A stubborn non-matching tail (~5%, per `isle`'s experience with the same compiler) | Accept it. Charter targets L4 on `.text`, and `nonmatching` is a legitimate terminal status with a recorded reason. |
| Metric inflation | Phase 2 makes `matching` settable **only** by reccmp output. Until then the honesty rule in `CLAUDE.md` is the sole guard — this is the largest open gap. |
| Register-allocation rabbit holes | The archive's closed-list is authoritative: ~20 measured variants moved none of them. Do not reopen it. The one carve-out is `/Od` locals in swapped stack slots (Q-021), where renaming is a single-build lever. |
| Data / `.rdata` ordering blocking L4 | It is a declaration-order problem in the source, not a code problem. Schedule it explicitly in Phase 4 rather than discovering it late. |
| Losing the archive's hard-won knowledge | `salvage/` is committed and its findings are being folded into the live docs. |
| `MPS_CODE` and `.mok` treated as ordinary code | They are not compiler output. ADR-0002 models Mok's patch as a post-link transformation; `MPS_CODE` is a self-modifying stub. Both are Phase 5. |
