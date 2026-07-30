# Methodology

How work actually gets done on this project. The charter says *what* we are building;
this says *how*, in enough detail that a fresh agent session can pick up cold.

## The loop

For each function:

1. **Identify** — what is it, what calls it, what does it touch? Name it, give it a signature,
   assign it to a translation unit and subsystem.
2. **Decompile** — write C that expresses the same computation.
3. **Compile** — with the period toolchain, at the right optimisation settings.
4. **Compare** — normalise both sides and diff the bytes.
5. **Iterate** — adjust the C until the bytes match. This is the real work.
6. **Record** — set the status honestly via `progress.py set`.

Steps 3–5 are the loop that has to be fast. Everything in `tools/` exists to make that loop
mechanical.

## Matching, concretely

A function "matches" when its normalised bytes equal the original's normalised bytes.
Normalisation (`tools/verify/funcbytes.py`) masks:

- rel32 displacements of `E8` (call), `E9` (jmp), `0F 8x` (jcc)
- rel8 displacements of `EB`, `7x`, `E0`–`E3`
- any 4-byte immediate or ModRM displacement whose value lands inside
  `[imageBase, imageBase + sizeOfImage)` — i.e. an absolute address
- trailing `CC` / `90` alignment padding

It deliberately does **not** mask register allocation, instruction selection, non-address
immediates, or stack layout. Those are exactly what must match.

Instruction boundaries come from a real x86 length decoder, not a byte scan, so displacement
fields are located correctly rather than guessed. Verified against live code: in
`Shandalar.exe:0x4bea85`, `push 0x5899a0` and the `call`/`jmp`/`jcc` targets get masked, while
`sub esp, 0x124` and `mov eax, [ebp+0x10]` survive untouched.

## Order of work

Attack the graph from the leaves inward, and the shared core before the specifics.

1. **Library and CRT code first — by classification, not decompilation.** A meaningful fraction
   of the 3.9 MB is Microsoft CRT and compiler helpers, especially in `Drawcardlib`,
   `Cardartlib`, `Cdtools` and `Gcconn`, which statically link the *debug* CRT. Identify these,
   mark them `library`, and match them by linking the right library. Hand-decompiling CRT code
   is wasted effort and actively harmful to the metric's meaning.

2. **The shared core of `Shandalar.exe` / `Magic.exe`.** ~69–79% of code content is common.
   Every function matched there counts twice.

3. **Small DLLs with named exports.** `Magsnd` (19 KB), `Manalinkinterface` (23 KB),
   `Statwin` (33 KB), `Magvid` (36 KB) are small, have fully named export surfaces, and have
   clean responsibilities. They are the right place to shake out the build and verification
   pipeline before committing to the big targets.

4. **Leaf utilities in `sidlib`.** File I/O, PCX decoding, sprite blitting, text rendering.
   High call counts, low complexity, no dependencies — the classic starting point.

5. **Subsystem by subsystem inward**, ending at the duel engine and AI (`sid/Ai.c`), which are
   the largest and most entangled.

## Choosing what to work on

Ranking heuristics, in rough priority order:

- **High caller count, small size** → utility or library code. Cheap, high leverage,
  often `library`.
- **Zero callers, large size** → reached indirectly: window procedures, dispatch table targets,
  thread entry points. Identify the table that references them before decompiling.
- **Already-named** → anything reachable from an export or an `assert()` string starts with a
  free name and a free translation-unit assignment.
- **Byte-identical across both EXEs** → the 954 functions that normalise identically are
  guaranteed shared-core; do them once.

## Naming and attribution

Recovered names beat invented ones, always. Sources of real names, in descending reliability:

1. DLL export tables — 152 real names across the nine DLLs.
2. Exports from the EXEs themselves (11 and 12) — these are shared *globals*
   (`_OpponFace`, `_PlayerFace`, `deck`, `Gold`, `Scards`), which also tells us those modules
   share mutable state across the DLL boundary.
3. `assert()` strings — give the source file, and often the expression and therefore the
   variable names.
4. Format strings, `.ini` key names, resource strings, and window class names.

Translation-unit assignment comes from the recovered tree in
[01-binary-inventory.md](01-binary-inventory.md). If the TU is not known, leave it unset —
`objdiff_report.py` buckets those into `<binary>/UNASSIGNED`. A wrong TU assignment is harder to
notice later than a missing one.

## Status discipline

The ladder is defined in [CLAUDE.md](../CLAUDE.md) and enforced forward-only by `progress.py`.
The rule that matters:

> `matching` means the bytes are identical. Nothing else may be recorded as `matching`.

`equivalent` exists precisely so there is somewhere honest to put "it works but does not match".
It scores zero. That is intentional — the metric has to keep meaning the thing it claims to mean,
because it is the only signal steering a project this long.

## Verification layers

| Layer | What it checks | When |
|---|---|---|
| Tripwire | `orig/` still matches `SHA256SUMS.txt` | every Edit/Write, via hook |
| Per-function | normalised bytes equal | every build |
| Per-object | our `.obj` equals the original's contribution | once linking works |
| Per-section | `.text` / `.rdata` / `.data` byte-identical | acceptance gate (L4) |
| Whole-file | `sha256sum` equals `orig/` | opportunistic (L5) |

## Automation

- `tools/progress/refresh.sh` — rescan inventories, recompute metrics, regenerate the Markdown
  report, the HTML dashboard and the objdiff report. `--full` also re-runs Ghidra.
- `PostToolUse` hook — fast tripwire on `orig/` after every edit.
- `Stop` hook — full refresh at the end of every agent session, so metrics never go stale.
- decomp.dev — see [05-decomp-dev.md](05-decomp-dev.md).

## Things that will waste time if not pre-empted

- **Hand-decompiling CRT code.** Classify first.
- **Chasing a match without knowing the compiler flags.** Optimisation settings are a global
  property; determine them once on a small, well-understood function and reuse the answer.
- **Assuming Ghidra's function boundaries are right.** They are ~98.8% right, which means
  roughly 19 KB of `.text` per big binary is *not* inside a recovered function. Some of that is
  padding; some is missed code. Reconcile it rather than ignoring it.
- **Treating `Shandalar.exe` and `Magic.exe` as independent.** They are not.
- **Forgetting which target you are matching.** The shipped EXEs are patched; see ADR-0002.
