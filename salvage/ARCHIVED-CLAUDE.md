# CLAUDE.md — agent handoff guide

Read this whole file before doing anything. It exists so an agent with zero context can
continue this project without breaking its rules.

## What this project is

A **decompilation** of *Magic: The Gathering — Duels of the Planeswalkers v3.0* (MicroProse,
Jan 1998), including the Shandalar adventure game. There is no original source code — only
Win32 binaries plus data files.

The goal, stated by the owner on 2026-07-27 and **superseding all earlier framing**:

> "I honestly don't care what the language ultimately ends up being. I only care how easy it
> will be to decompile the binary into usable, observable code. This is the most important
> aspect of the project and supercedes all previous intent."

So the ordering is: **observability first, everything else second.**

"Observable" has a mechanical meaning here. It is not "the tests pass" — it is *you can
recompile your source and measure how close the machine code is to the original.* That
measurement is the product. Anything that weakens it loses.

### Why the target language is C

Not a preference — a consequence. The binary was compiled from C by Visual C++ 4.2. The only
mechanical proof of correctness available is "compile it and compare the machine code," and
that comparison only exists if we emit the same language with the same compiler family. Write
C, and every function gets a percentage. Write anything else and you are back to hand-written
tests that only cover what you thought to test.

That failure mode already happened: the earlier C# port has 120 tests and pinned golden hashes
and *still* missed that the sprite blitter reads a field at frame offset `+14`. Behavioural
tests were never going to catch it. See `docs/decompilation.md`.

## The product is fifteen binaries, not one

Measured 2026-07-27 by reading the shipped install. Easy to forget, because almost all work so
far has been on `Shandalar.exe` alone.

**Fourteen of the fifteen shipped binaries are linker 4.20** (MSVC 4.2). Only `Gcconn.dll`
(matchmaking) is 5.00. One toolchain across the whole product, tools included — so a build-flag
finding on one binary is very likely to hold for all of them.

**`sidlib` is shared infrastructure, not Shandalar code.** It is statically linked into five
separate binaries: `Shandalar.exe`, `Magic.exe`, `Deckdll.dll`, `Drawcardlib.dll`, and
`Facemaker.exe` — the last being an *art tool*, which is what proves the library is reusable
rather than game-specific. The embedded assert paths give the original source tree:

```
D:\NewMagic\
  sources\sidlib\        lib.c sprite.c text.c Fileio.c Pcxw.c
  sources\NedCard\       Catalog.c Palette.c haar.c   (card rendering; haar = wavelet coding)
  sources\1player\FaceMaker\code\test.c
  multiplayer\sidlib\    Pcxw.c        <- a copy-paste fork of the whole tree
  multiplayer\sid\       Ai.c glue.c
```

Three consequences for how work gets planned:

1. **Analyse a library in the smallest binary that contains it.** `Facemaker.exe` is 89 KB and
   contains *all five* sidlib translation units including `Fileio.c` and `Pcxw.c`, which are not
   started. `Shandalar.exe` is 1.7 MB of mostly unrelated code. Same library, a twentieth of the
   noise. Prefer Facemaker for new sidlib work.
2. **The same function exists at several addresses.** That is free cross-validation: diff a
   function's instances across binaries, and whatever differs is compiler/linker noise while
   whatever agrees is real code. It also lets a reconstruction be checked against instances no
   annotation touched.
3. **Expect forked near-duplicates.** `multiplayer\sidlib\` is a copy of `sources\sidlib\`,
   normal 1997 practice. When a function appears to exist twice with small divergences, do not
   assume one is canonical.

Shipping asserts with `__FILE__`/`__LINE__` compiled in is unusual — most releases define
`NDEBUG` — and it is where the entire module map came from. On any new binary, run
`strings <file> | grep -i ':\\'` first; it costs five seconds and may hand over the original
filenames, directory layout and translation-unit boundaries outright.

**Prior art:** `reccmp` comes from **isledecomp**, the LEGO Island decompilation — also 1997,
also MSVC 4.2. Their notes on that compiler's idioms are the most directly applicable outside
resource for this project. Read them before reconstructing another hundred functions rather than
rediscovering the same idioms one at a time.

## Facemaker is the second verification target, and the primary one for new sidlib work

Decided 2026-07-28, owner-directed. `FACEMAKER` is a full reccmp target alongside `SHANDALAR`
(`reccmp-project.yml`), scoring the *same* source tree against a second independently-linked
image of sidlib:

```powershell
& "$V\reccmp-reccmp.exe" --target FACEMAKER --no-color      # config-driven; no --paths needed
```

Three reasons it earns that place:

1. **49 kB of `.text` against Shandalar's 1.47 MB**, with *more* assert anchors (lib.c:1152,
   text.c:219/221, Fileio.c:365 — none of which Shandalar links).
2. **It still ships its debug directory**: an FPO table with exact boundaries, parameter bytes,
   locals bytes and saved-register count for 151 functions, plus the PDB path
   (`D:\NewMagic\1player\FaceMaker\code\new___Wi\FaceMaker.pdb`). `Deck.exe` has one too, and its
   path ends in `Debug\`, which is why the whole product ships compiled-in asserts.
3. **Agreement between two links is stronger evidence than one score.** Where both images score
   the same, the residual is our source; where they diverge, it is layout noise.

**The per-TU address delta is a trap.** All seven of lib.c's Shandalar↔Facemaker assert anchors
line up at exactly `0x1728d0`, which looks like a constant relocation — and it is wrong, because
the two images link different subsets of each object file, so the delta breaks at the first
function one of them drops. Facemaker addresses come from **content matching** instead:
`ghidra-work/scripts/xmatch.py` masks relocated operands and relative branches, then requires a
**unique, byte-exact** match before a mapping is usable. 35 of 64 annotated functions qualified;
the 11 `Sid_DrawText*` wrappers tie against each other (identical bodies, only masked constants
differ) and are deliberately left unmapped — a plausible-but-wrong FUNCTION address fabricates a
score, which is worse than none.

**Data markers must be mapped per-target too.** A byte-exact function scores below 100 on
FACEMAKER if its globals carry only SHANDALAR markers — reccmp sees a name on one side and a bare
address on the other. `xmatch.py --globals` derives the data-address correspondence from inside
matched function pairs (each masked dword where both images hold an in-image address is one
symbol's two addresses, cross-checked over every pair that references it; conflicts are reported,
not resolved). 117 mappings, zero conflicts, and adding the 46 markers took FACEMAKER from
82.74% to 87.71% with zero code change.

Byte-exact against Facemaker so far: `Sid_DrawTextCore` (520 bytes), `Sid_OpenFileCore` (608)
and `Sid_LoadFontFile` (257) — all three still stubs in our tree, waiting to be reconstructed
against the small binary first.

## Hard rules — never violate

1. **This repo stays PRIVATE.** It contains code decompiled from copyrighted binaries, and the
   game content is Wizards of the Coast IP. Never change visibility, never copy contents to a
   public repo, never publish extracted assets. Decompiling to C makes this repo unambiguously
   a derivative work — more so than a translated reimplementation would be. That trade was made
   deliberately, in exchange for verifiability.
2. **Never commit original game files or content extracted/converted from them** (no `.pic`,
   sprites-as-PNG, card art, sounds). Test fixtures must be synthetic files we authored. The
   only game-derived things allowed in-repo are *facts*: addresses, format docs, hashes,
   dimensions, record counts, short excerpts in assertions. **Decompiled C is allowed and is
   the point of the repo** — that is what rule 1 pays for.
3. **The original install is read-only.** It lives at
   `c:\Users\Ben\Desktop\Shandalar\MtG_DotP_SotA`. Never modify, move, or delete anything in
   it. Tooling reaches it via `SHANDALAR_GAME_DIR`, or via the staged copies in
   `..\ghidra-work\bin`.
4. **Never translate un-understood code.** Do not paste decompiler output and call it done.
   Name it, type it, understand why each branch exists, *then* write it. A function at 40% that
   you understand beats one at 80% that you guessed.
5. **Fidelity over correctness.** When the original behaves oddly, match it by default.
   Deviating is a `decision`-labelled issue plus a `docs/quirks.md` entry, never a silent fix.
   Don't "modernise" the 1998 rules (pre-6th-Edition timing: interrupts/batches, no modern
   stack).
6. **`Shandalar.exe` is the reference binary, not `Magic.exe`.** `Magic.exe` carries an added
   `.mok` section — a crack from the "RIP-Version" repack. Do not decompile it.
7. **Measure before and after.** Any change to `decomp/` is justified by a reccmp number. If a
   change lowers the score, say so and revert it — the number is the referee, not your
   intuition about what the compiler "should" emit.
8. **Breadth before depth, this phase.** Reconstruct the next function rather than perfecting the
   last one. See "THIS PHASE IS BREADTH-FIRST" below for where the line falls.

## The verification loop

This is the core workflow. Everything else supports it.

```powershell
cd decomp
.\build.cmd                       # MSVC 4.2 -> build\shandalar_decomp.dll + .pdb
$V = "..\..\ghidra-work\venv\Scripts"
& "$V\reccmp-reccmp.exe" --paths `
    ..\..\ghidra-work\bin\Shandalar.exe `
    build\shandalar_decomp.dll build\shandalar_decomp.pdb . --no-color
```

Output is a percentage per annotated function. To see *why* a function differs, pass the
original address:

```powershell
& "$V\reccmp-reccmp.exe" --paths ... -v 0x57b710      # assembly diff
& "$V\reccmp-decomplint.exe" --module SHANDALAR src include   # validate annotations
```

**Annotation format.** Every reconstructed function carries the original's address in a `//`
line comment directly above its *definition* (not its declaration):

```c
// FUNCTION: SHANDALAR 0x0057b710
int Spr_LoadFile(void **frames, const char *path)
```

reccmp parses `//` line comments only — block comments are silently ignored, which looks
exactly like "reccmp didn't find my function."

**`FUNCTION` is not the only marker, and skipping the others costs real points.** reccmp also
reads `GLOBAL` (above a variable definition) and `STRING` (above a line containing a string
literal). Without them, every instruction touching a global or literal scores as a mismatch —
reccmp sees a name on our side and a bare address on the original's. Adding the `GLOBAL` markers
in `src/stubs.c` moved the aggregate 67.34% → 69.90% *without changing a line of code*, and took
one already-perfect function from 66.67% to 100%.

```c
// GLOBAL: SHANDALAR 0x0073a828
void *g_sprArenaBase;

    // STRING: SHANDALAR 0x005a17cc
    fp = fopen(path, "wb");
```

Two rules come with them:

- **Never invent an address to silence a diff.** An unannotated global is one whose original
  address is not yet established. A guessed marker fabricates a match and corrupts the only
  measurement the project has.
- **Never put a `STRING` marker inside a multi-line call's argument list.** It desynchronises
  the parser, which then binds each later `FUNCTION` annotation to the *next* function's symbol
  — real names, wrong addresses, believable scores. `decomplint` does not catch it.
- **A `STRING` marker can go on a `#define`,** and for the `SIDLIB_*_C` path macros it must:
  they expand inside multi-line `Sid_Assert` argument lists, which is exactly where a marker
  cannot go. Annotating the `#define` in `sidcore.h` covers every use in every module at once.
  Doing that for one macro took `Spr_LoadFile` from 94.44% to byte-exact.
- **Unannotated strings are the cheapest points in the project, and they hide in plain sight.**
  One sweep — seven literals plus two path macros, no code change — moved six functions and took
  two of them to 100%. When a diff shows `push 0x005a1690` against `push "..." (STRING)`, that is
  not a mismatch to explain, it is a missing marker. Read the address out of the diff, confirm it
  with `rdstr.py <addr>`, then annotate.

**One header per module, and keep it that way.** Every declaration a translation unit can see
perturbs MSVC 4.2's register allocation across that whole unit. While `sidlib.h` was one shared
file, adding six prototypes for a module `lib.c` never calls took `Sid_SetPixel` from a 100%
match to 31% with its source untouched. The header is now split — `sidcore.h` (shared surface
only) plus one per module — and cross-module references are single declared lines, never an
include of a neighbour's header. `src/stubs.c` is the only file that includes everything, which
is safe because nothing in it carries a `FUNCTION` marker so its codegen is never scored.
Verified: adding throwaway prototypes to `sidtext.h` now changes zero scores anywhere.

**This holds at `/Od` too — do not assume the game layer is immune.** `gameslot.h` used to claim
unoptimised code does not re-roll when a declaration appears; that was measured false on
2026-07-28. Adding a typedef, two externs and one prototype to `gameslot.h` moved
`Game_ResetState` from 95.15% to **100%** with `reset.c` untouched, by flipping the operand order
MSVC picked for an array index — that function's entire residual. Reverting only the new *struct
member* kept the 100%, so it was the plain declarations. It cut favourably that time and will cut
the other way eventually: after touching a shared header, measure the whole target, not just the
function you were working on.

**Declaration order of locals sets register allocation, and it is worth real points.** MSVC 4.2
assigns registers to locals in the order they are *declared*, not the order they are first used
— and locals rank after parameters. Moving one line, `int rows;` above `const unsigned char *p;`,
took `Spr_GetFrameExtent` from 46.67% to 68.89% and moved no other function. When a diff is a
pure register permutation with the instruction sequence already matching, reordering declarations
is the first thing to try; it is semantically identical, so it is a legitimate experiment rather
than a guess. `register` is **not** a lever — measured a no-op under `/O2`, in both directions.

**Under `/Od` the lever is a different one: a local's stack slot follows its NAME.** Declaration
order and order of first use are both measured no-ops there — `(a,b)` used a-first, `(b,a)` used
a-first, and `(a,b)` used b-first all put `a` at `[ebp-4]` and `b` at `[ebp-8]`. So when a `/Od`
diff is nothing but two locals in swapped slots, **rename them**; it costs one build. Doing that
in `Game_ResetState` was worth 74.43% → 95.15% with nothing else changed. The rule behind it is
*not* alphabetical and is not established (`(zz,aa)` puts `zz` at `-4`), so measure rather than
predict. See `docs/quirks.md` Q-021. This is the one carve-out from "do not chase slot order"
below, and it applies only to `/Od` — i.e. to the game layer, not to `sidlib`.

**NMAKE does not relink the object it just compiled.** It snapshots file timestamps when it
starts and never re-stats a target's prerequisites after rebuilding them, so the run that
recompiles `sprite.obj` prints `'build\shandalar_decomp.dll' is up-to-date` and skips the link.
The new code reaches the DLL only on the *next* build.

This is not occasional — it happens on every source edit, and it silently feeds reccmp the
previous build, so a change measures as a no-op and the run after some unrelated change picks up
its delta instead. It is why a +35-point change was once written off as worthless. `build.cmd`
now loops NMAKE until no tool is invoked, which fixes it at the source; do not "simplify" that
loop away. Reproduce the underlying behaviour with `nmake /f Makefile build\shandalar_decomp.dll`
after touching a `.c`.

**Update the progress report in the same commit as the work.** `decomp/progress/index.html` is a
self-contained page the owner reads to follow progress. Regenerate it *before* committing:

```powershell
.\tools\progress.ps1            # measure, refresh accuracies, re-render
.\tools\progress.ps1 -Render    # re-render from stored data, no build
.\tools\progress.ps1 -Inventory # re-scan every binary (slow: Ghidra + matching)
```

**Every number in it is matched bytes over total bytes, at the scope shown.** That metric only moves
up, and it moves in proportion to the work. The report deliberately does **not** show accuracy
averaged over annotated functions: that number *falls* whenever a function is reconstructed below the
current average, so it punishes exactly the work that makes progress — six of the first twenty-six
commits dipped for that reason alone. Per-function accuracy is still measured and shown; it is just
never aggregated. Do not reintroduce an aggregate-accuracy headline.

**Scope starts at the whole product and drills down.** Top level is all fourteen shipped game
binaries (3.96 MB); click a binary for its functions. `Shandalar.exe` has a full function inventory
from Ghidra (2,210 functions); every other binary lists only the functions `xmatch.py` proved
byte-identical to our reference copy, with the rest shown as one honest "not yet inventoried"
remainder whose size comes from the PE header and nothing else.

The current picture: **0.278% of the product**, which is 0.383% of `Shandalar.exe` and 9.04% of
`Facemaker.exe`. That last number is why Facemaker is the place to work.

**Cross-binary credit is earned, not assumed.** A function counts toward another binary only when it
is a *unique, byte-exact* match there — near matches count zero rather than partially. The
expectation from the assert paths was that all five sidlib-linking binaries would share the code;
they do not. Facemaker shares 33 of 59, `Deckdll.dll` 2, `Magic.exe` 4, `Drawcardlib.dll` none. So
those binaries carry a different build of sidlib. Which build, and why, is not established — say so
rather than papering over it.

`progress/history.jsonl` is still written and is a data record only; nothing in the page reads it.

History is `decomp/progress/history.jsonl`, one record per commit, and both files are committed.
Because it is keyed by commit sha, run it after `git commit` and amend, or run it, commit, and let
the next run correct the row — re-recording a sha replaces its row rather than duplicating it.

Backfill is the interesting one: it rebuilds each past commit in a throwaway git worktree, so the
history is *measured*, not copied out of commit messages. It force-cleans `build/` per commit (a
stale `.obj` from another revision links in silently) and converges the build itself rather than
trusting the `build.cmd` of whatever revision it checked out, since most of them predate the NMAKE
relink fix. It reproduced every number in this project's commit messages independently.

**Measure with `test\score.ps1`, and prefer `-Against`.** It builds, runs reccmp, and prints the
per-function table plus the aggregate:

```powershell
.\test\score.ps1 -Save before      # record a run
.\test\score.ps1 -Against before   # print only what MOVED
.\test\score.ps1 -Diff 0x57c450    # assembly diff for one function
```

`-Against` is the one that matters. Editing anything in a translation unit re-rolls MSVC's
register allocation across all of it, so a change aimed at one function routinely shifts its
neighbours — and the aggregate alone hides which way. It also warns when the DLL was not
rewritten, by comparing the artefact's timestamp rather than grepping the build log.

**Build settings that matter.** `/MD` (dynamic CRT) is measurably correct because the original
imports `msvcrt.dll`; `/MT` scores ~15 points worse. **`/G5`** (schedule for Pentium) is
required — it was worth 71.87% → 78.64% aggregate in one change and took seven functions to
exactly 100%. `/O2` for most translation units but *not* all (see `src/window.c`), C89, MSVC 4.2
only. Using a modern compiler makes every number meaningless.

**Exhaust the compiler switches before rewriting C.** An instruction you cannot explain is a
hypothesis about the *build* as much as about the source. `/G5` alone accounts for stray
single-byte `NOP`s and for loads hoisted between a `cmp` and its branch — both of which were
previously misread as evidence that reconstructed functions were missing source constructs, and
one of which got written into the docs as a false general rule. Switches are cheap to test and
apply to all 2,190 functions at once.

## The second verification axis: differential testing

reccmp proves our machine code resembles the original's. It proves **nothing about behaviour**,
and for every function below 100% that leaves no evidence of correctness beyond reading a diff
by eye — which has already produced two overconfident "behaviourally verified" claims in this
project.

```powershell
cd decomp
.\test\difftest.ps1            # build both sides, run over all 344 .spr, diff per frame
.\test\difftest.ps1 -VerboseDiff
```

`decomp/test/sprprobe.c` links the **same objects the DLL is built from** and drives the real
reconstructed functions over real game files; `shandalar sprprobe --all` emits the same columns
from the independently-verified C# reader. The script exits non-zero on any *new* disagreement;
the three known ones are listed in it and explained in `docs/formats/spr.md`.

Two properties make this worth more than another reccmp point:

- **It does not drift.** reccmp scores move when an unrelated header changes; behaviour does not.
- **It answers questions reccmp cannot.** It confirmed `Spr_LoadFile` finds identical frames at
  identical offsets across 924 frames, and settled a format dispute that had been guesswork.

Only functions that touch no dangerous globals are reachable from the driver —
`Sid_CreateScreenPage` must never be (quirks Q-009). The game directory is opened read-only.
**When you reconstruct a function that can be driven over real data, extend this rather than
relying on the reccmp number alone.**

## The third axis: a Shandalar.exe that actually runs

Added 2026-07-29. `build.cmd host` links the reconstruction into a real program:

```powershell
cd decomp
.\build.cmd host                                    # -> build\Shandalar.exe
$env:SHANDALAR_GAME_DIR = "c:\Users\Ben\Desktop\Shandalar\MtG_DotP_SotA"
.\build\Shandalar.exe Title.pic                     # window + title screen; Esc or click closes
```

It creates the game's real window and draws a real `.pic` through the reconstructed display path
— `Sid_CreateScreenPage` → `Sid_AllocPage` → `Sid_OpenFileCore` → `Sid_SetPalette` →
`Sid_PutLine` → `Sid_BlitPageToScreen`. Every one of those is scored code; the picture on screen
is produced by the decompilation, not by a viewer written next to it.

**It links the same objects the DLL is built from** (`COREOBJS` in the Makefile), so it cannot
drift from what gets measured. Only two objects differ: `stubs_host.obj` is `src/stubs.c` compiled
with `SHANDALAR_HOST`, which drops the two MPS_CODE stubs, and `src/host/` supplies working ones.
Nothing under `src/host/` carries a reccmp marker and none of it is scored — it is a harness, not
a reconstruction, and it says so at the top of each file.

`src/host/mps.c` is the one substantive piece: MPS_CODE is hand-written assembly and never a
decompilation target, so the host implements the documented PICv3 pipeline (LZW then RLE, see
`docs/formats/pic.md`) behind the same two entry points `Sid_OpenFileCore` calls. Verified against
the C# oracle: **0 mismatches in 307,200 pixels** on `Title.pic`, and the installed palette differs
from the file's in exactly one entry, which is Q-020 firing as documented.

Three things this buys that reccmp cannot:

1. **It executes the code.** Q-022 was found this way and could not have been found any other way:
   `Sid_CreateScreenPage` is byte-faithful and still crashed, because a loop in it is bounded by
   the *distance between two globals* and our linker laid them out differently. A reconstruction
   can match perfectly and still not run.
2. **It finds missing callers.** `Sid_AllocPage` deliberately never publishes its page — the
   original pairs every call with `0x00579310`, which writes `g_sidPages[]`. Reading the function
   alone does not reveal that; running it does.
3. **It is how a startup path gets built.** The real `WinMain` is `0x004cdd70` and its window setup
   is reproduced value-for-value in `src/host/shell.c`, but it gates on `AdvStrings.txt`,
   `AdvBlocks.txt`, three CVL archives and six TrueType faces before spawning the game thread at
   `0x0046e6f0`. Reconstruct those and the host converges on the real WinMain.

**Debugging it:** reconstructed code must never be instrumented — a trace call in `lib.c` re-rolls
MSVC's allocation across that whole unit and moves every score in it. Instead set
`SHANDALAR_HOST_LOG` for stage tracing, let the host's unhandled-exception filter record the
faulting address, and resolve it against `build\host.map` (which is why the host links with
`/MAP`). `SHANDALAR_HOST_DUMP` writes the decoded page for diffing against the oracle.

**Do not add `host` to `all`.** The measured loop is `build.cmd` with no arguments and this target
must never lengthen or perturb it.

## Environment

Windows 11, PowerShell 5.1 (no `&&`; chain with `;`). All paths absolute where possible.

| Thing | Where |
| --- | --- |
| Ghidra 12.1.2 | `C:\Tools\ghidra_12.1.2_PUBLIC` (JDK 21) |
| MSVC 4.2 | `C:\Tools\msvc42\MSDEV` (`BIN\CL.EXE`, linker 4.20 — matches the original) |
| Ghidra project + scripts | `..\ghidra-work` (**outside the repo**, untracked, game-derived) |
| PyGhidra venv + reccmp | `..\ghidra-work\venv` |
| ReVa MCP extension | installed in Ghidra; `..\.mcp.json` points at `http://localhost:8080/mcp/message` |

The PyGhidra harness in `..\ghidra-work\scripts` is how analysis gets done headlessly:

```powershell
$env:GHIDRA_INSTALL_DIR="C:\Tools\ghidra_12.1.2_PUBLIC"
cd ..\ghidra-work\scripts
..\venv\Scripts\python.exe inventory.py 0x00579000 0x0057e000 out.txt  # functions + strings
..\venv\Scripts\python.exe decomp.py 0x0057b710 --out out.c            # decompiled C
..\venv\Scripts\python.exe disasm.py 0x0098a245 0x0098a300             # raw disassembly
..\venv\Scripts\python.exe findconst.py 0x3058 0x304d                  # find constants
..\venv\Scripts\python.exe survey.py                                   # decompilability stats
..\venv\Scripts\python.exe seed.py                                     # re-apply names/comments
```

**Read the disassembly before trusting the pseudocode.** Ghidra's C output misled us once
already (an apparent 6-byte `.pic` header that real file bytes disproved). `disasm.py` settles
arguments that `decomp.py` starts.

The C# tree still builds and is now an **oracle, not the product**: its parsers are
independently verified against real files, so they are the fastest way to check whether a piece
of reconstructed C behaves correctly. Do not delete it, do not invest in it.

```powershell
$env:SHANDALAR_GAME_DIR = "c:\Users\Ben\Desktop\Shandalar\MtG_DotP_SotA"
dotnet build ; dotnet test ; dotnet format     # CI fails on formatting; line endings are LF
```

- **GitHub access**: `git push` works via Windows credential manager. If `gh` says "not logged
  in", get a token into `GH_TOKEN` via `git credential fill` (stdin: `protocol=https`,
  `host=github.com`, blank line). Only ever operate on `oldenburgbd/Shandalar`.
- CI (`.github/workflows/ci.yml`) covers the C# tree only; the `decomp/` build needs MSVC 4.2
  and does not run in CI. Verify it locally.

## Repo map

| Path | What |
| --- | --- |
| `decomp/` | **The product.** Reconstructed C + MSVC 4.2 build + reccmp config |
| `decomp/src/`, `decomp/include/` | reconstructed translation units, mirroring the original tree |
| `decomp/reccmp-project.yml` | target `SHANDALAR`, original binary pinned by SHA-256 |
| `docs/decompilation.md` | binary facts, recovered module layout, verified format findings |
| `docs/migration-plan.md` | phased plan and exit criteria |
| `docs/quirks.md` | Q-numbered oddity register |
| `docs/formats/` | one doc per reversed file format |
| `src/`, `tests/` | C# oracle (see above) |

The old plan reserved a `reference/` directory to quarantine decompiled C away from the C#
build. It was never populated, and the concept is dropped: now that decompiled C *is* the
build, a quarantine would be an obstacle. Reconstructed C goes in `decomp/`.

## THIS PHASE IS BREADTH-FIRST. Do not polish.

**Coverage comes from new functions, not from finishing old ones.** The metric is matched bytes
over total bytes, and the arithmetic is not close: grinding a 608-byte function from 86% to 100%
is worth ~85 bytes, while a new 500-byte function at 80% is worth ~400. Five times the return for
less work. The per-function percentage reccmp prints is the salient number and it is the wrong
one to optimise — it is a property of a function, not a measure of progress.

So, per function: measure once, fix what is **structural**, stop.

- Structural, and worth iterating on: a missing `STRING`/`GLOBAL`/`FUNCTION` marker, a wrong
  frame size, an inverted branch, the wrong helper called, an argument in the wrong slot. These
  are real defects in the reconstruction and the diff names them.
- **Not** worth iterating on: a register permutation, comparison operand order, the order of two
  addends. These are allocator output. Three sessions have now tried to reach them from the
  source — declaration order, local vs. parameter, separate accumulators, reversed conditions —
  across ~20 measured variants, and moved **none** of them. Do not run that sweep again. Record
  the residual in a comment and go to the next function.
- **One carve-out, added 2026-07-28: `/Od` locals in swapped stack slots.** That one *does* have
  a lever — the identifier name (Q-021) — and it is a single build, not a sweep. Try one rename;
  if it does not move, stop. Everything else on the list above stays closed.

A function left at 78% with the diff understood and written down is finished work for this phase.
Come back to the last 20% when there is nothing new left to reconstruct, if ever.

## Working style

- **Start from `ghidra-work/out/*.c`.** Ghidra's decompilation of much of sidlib already exists
  there and is the cheapest available starting point. It is not correct — expect aliased loop
  variables, missed locals, and `undefined4` where a struct belongs — but correcting a
  decompilation is far quicker than deriving one from disassembly. Check for the function there
  before opening `xdis.py`. (One function in this repo was hand-derived from raw x86 while its
  decompilation sat unread in `out/text_core.c`.)
- **Work the game layer, not `sidlib`, and build it `/Od`.** `Magic.exe` is a second build of
  `Shandalar.exe` sharing 595 functions, only six of which are in `sidlib` — so a game function
  credits 3.18 MB of the corpus against a `sidlib` function's 1.65 MB. And the game layer is
  unoptimised, which removes the register-allocation guessing that caps every `sidlib` score: the
  first one attempted was byte-exact on the first build at 3,919 bytes. Pick from the 595,
  largest first (`ghidra-work/scripts/share.py`).
- **The card table is a dispatch table, and that is the seam for the card layer.** Every
  `GameCard` record carries a `__cdecl int (*)(int, int, int)` at **+0x34**: 23 indirect call
  sites plus 50 `cmp`s testing a card's handler against a named function, and all 52 immediates
  compared against it are `.text` addresses. Each handler is a small self-contained `/Od`
  function that reaches the slot it was invoked on through two globals rather than arguments
  (`g_gameSlots[g_game8e1aa8][g_game951bdc]`) — so they need no stubs. The first one taken,
  `Game_CardScript48b2c1`, was byte-exact on the first build.
- **The card layer is 38.6% of `Shandalar.exe`, and you must read the table to see it.** Do *not*
  enumerate the scripts by scanning code: the table is **statically initialised in the image**, so
  the pointers can be read straight out of it. Scanning `.text` for named addresses finds 23;
  reading `+0x34` from all 1,009 records finds **722 cards carrying 608 distinct scripts,
  603,363 bytes**. Median script 788 bytes, 22 are call-free, and `0x00488ca0` is a do-nothing
  handler shared by 88 cards. `ghidra-work/scripts/cardscripts.py` prints the ranking (`--all`,
  `--csv F`). This is the largest single body of code in the product and the right place to spend
  the next several sessions.
- **Stubs are the lever on that 603 KB, not call-free scripts.** Only 23 of the 608 scripts call
  nothing (5,956 bytes, ~1%). The rest are blocked *solely* by unresolved call targets, and the
  targets are shared: `ghidra-work/scripts/stubgain.py` prints the greedy curve, and it is steep —
  **6 stubs unlock 110 scripts / 50,430 bytes (8.4%), 20 stubs reach 15.6%, 40 reach 22%.** A stub
  is three lines and needs no understanding of the callee; get its argument count from the highest
  `[ebp+N]` the callee touches and its convention from `ret` vs `ret N` (all six taken so far are
  `__cdecl`). Add stubs in a batch, then reconstruct behind them.
- **Get the argument count from the CALL SITES, not the callee.** A function that never touches
  its last argument hides it from an `[ebp+N]` scan; `add esp, N` after a `__cdecl` call cannot.
  All 26 stubs taken so far are unanimous across every site (273/273 for `0x00441699`, 531/531 for
  `0x0056ca10`), and every site cleaning up is also what proves they are `__cdecl`.
- **Never put a card-script prototype in a shared header.** Nothing calls a script by name --
  they are reached only through the `+0x34` pointer -- and reccmp scores the *definition* and its
  `// FUNCTION:` marker, so a declaration buys nothing and costs a lot. 28 of them, one per
  reconstructed script, cost 8 byte-exact functions and 630 matched bytes; deleting all 28
  recovered every one (`0x0048b2c1` went 33.64% -> 100%) without touching a function body. The
  general rule this is an instance of: **a declaration nothing needs is not free at /Od.**
  **Do NOT generalise that to "give each TU only what it names."** That was tried on 2026-07-28
  and measured worse: relocating all 103 shared declarations out of `gameslot.h` into the eight
  `.c` files that reference them cost **767 matched bytes** (1.1944% -> 1.1621%, 65 -> 61
  byte-exact, `Game_AllocSlot` 84.84% -> 51.13%) and was reverted. Removing declarations nothing
  needs helps; moving declarations something *does* need changes where they sit relative to the
  rest of the header and hurts. The lever is surplus, not locality — and the only way to tell the
  two apart is to measure.
- **The card scripts are GENERATED now, not hand-written.**
  `ghidra-work/scripts/cardgen.py` pattern-matches the disassembly into C; `cardwire.py` emits it
  into the right `cards*.c` and declares whatever it newly references. `cardgen.py --report` gives
  coverage (currently **96 of 608 scripts, 32,380 bytes**); `cardwire.py --plan N` shows the next
  batch, `--apply N` writes it. It validated by reproducing three hand-written functions exactly,
  two of them already byte-exact.
  - It **refuses rather than guesses** — an unhandled instruction, a loop, an index it cannot
    fold, a jump it cannot structure all abort that function with a reason. Keep it that way. A
    plausible wrong body is worse than none, because reccmp scores it and the number looks like
    progress.
  - **The score is the only verification.** Nobody read these bodies. A high score is proof; a low
    one means the structure is probably wrong. Anything under ~50%% was dropped rather than
    committed (three of the first twelve), and that line is the discipline that makes generation
    honest. Re-check it every batch.
- **The non-card layer is the working front now, and its blocker is declarations, not
  grammar.** 325 of 1,423 ordinary game functions decode (60,895 of 858,620 bytes) after the
  three-way-compare fix; 229 are in the tree. Of the ~96 that decode and are not applied, the
  filters say: **59 global rank conflict**, 10 need new struct fields, 8 stub promotions, 1
  ready. So the next lever is not another grammar class -- it is that a body wants a global at
  a rank (scalar / `x[]` / `x[][]`) that disagrees with how the tree already declares it.
  Resolving those is a data-layout question, the same kind as the record fields, and it unlocks
  six times what the next grammar fix would.
- **Clearing one refusal class rarely yields decoded code on its own.** Measured three times in
  a row on non-card functions: `unexpected epilogue` (170,011 B) bought nothing, `conditional
  straight to the epilogue` (62,892 B) bought one function, and only the three-way compare
  (105,172 B) moved the count -- because these functions are large and hit several walls each.
  Expect a cleared class to reveal the next wall, not to produce output, and size the work
  accordingly.
  - Biggest remaining refusals (2026-07-29, 339 of 608 decode): `unstructured jmp` (107),
    `improper nesting` (56), `indirect call` (33), `unrecognised memory operand` (19).
  - **The next lever is a real structuring pass, not another peephole.** The frame stack that
    carries `if`/`else`/`for` is at its limit: on `0x00431920` it ends up with a frame closing at
    `0x431c9d` sitting above one that closes at `0x431c93`, so nested `if/else` inside `if/else`
    mis-nests before the jump that gets refused. Peepholes cannot see that, and two defects this
    session (a condition merged into a loop frame, a `||` term merged with `&&` siblings) were both
    of that shape — structurally wrong output that still compiled. What this wants is the immediate
    post-dominator of each branch, so regions are computed rather than guessed at from close
    addresses. `cardgen.py` now reports the refusing jump's own address to start that work from a
    concrete case, and `condregion`/`opencond` already do the equivalent job for conditions: they
    reduce a whole test region to one `&&`/`||` expression and refuse the one shape (`p ? X : Y`
    with both branches non-trivial) that has no short-circuit spelling.
- **A card-script batch has to be big enough to pay for the churn it causes.** Measured
  2026-07-29: three scripts totalling 2,851 bytes into `cards43`/`cards48` cost **13,016 matched
  bytes** — Shandalar.exe 11.13% -> 10.30%, accuracy 86.37% -> 81.34% — and reverting restored the
  number exactly. Adding anything to a translation unit re-rolls codegen for every function in it,
  so a batch is only worth applying when what it adds outweighs what it disturbs. The batches that
  won the same day were 33, 32, 116 and 12 scripts; the one that lost was 3. Before applying, check
  the batch size against the units it touches, and if it is small, bank the generator improvement
  and wait for more scripts to accumulate rather than spending it on a handful.
- **Do NOT bulk-regenerate card scripts that are already in the tree.** Tried twice on
  2026-07-29 and both measured worse: a blanket refresh with the improved generator moved 10
  bodies and cost **560 matched bytes**, and gating it to only bodies scoring under 90% still cost
  **440**. `ghidra-work/scripts/cardrefresh.py` exists and works; the problem is not the tool. Only
  10 of 115 generated bodies were stale at all, the regenerated versions were not reliably better
  (`0x00484044` went 100% -> 37.63%), and rewriting any body re-rolls codegen for every other
  function in that translation unit. Generate a script once, measure it, leave it alone.
- **One translation unit per address range for card scripts, and keep them small.** A single
  `cards.c` meant each new script re-rolled codegen for every earlier one — `0x00517888` fell from
  100% to 60.78% without being touched. Splitting into `cards43/48/49/4b.c` recovered it and three
  others in one build (+3 byte-exact, no source change). It is also the closer match: the original
  evidently had one file per card, which is why three byte-identical 18-byte handlers sit at three
  addresses instead of being one function reused. Split further as these grow.
- **Check `fpo.py` before writing the frame.** Facemaker ships an FPO table giving exact frame
  size, parameter bytes, locals bytes and saved-register count for 151 functions. That is the
  answer to the questions that otherwise cost a build each.
- Issue-driven: every non-trivial change traces to a GitHub issue with the right label and
  milestone. Discovered work becomes a new issue rather than silent scope growth.
- Small commits, each leaving the C# tree's CI green and each reporting the reccmp delta.
- When decompiling: read the existing decompilation, confirm against disassembly, write the C,
  measure, iterate **only on structural differences**.
- When unsure whether something is our bug or the original's quirk: reproduce against the
  original binary or data first, then file it (`quirk` or `bug`) before fixing.
- Percentages are honest or they are worthless. Never report a score you have not just run.
