# Operating rules — MTG/Shandalar matching decompilation

Read this before touching anything. This project is driven mostly by agents across many
sessions; these rules exist so that work done in one session is trustworthy in the next.

## The prime directive

The goal is a **matching decompilation**: C source that, compiled with the period-correct
toolchain, produces **byte-identical** object code to the original. "It looks right" and "it
behaves the same" are *not* the goal and must never be recorded as if they were.

If you cannot make something match, mark it `equivalent` or `nonmatching` with a reason. Never
mark it `matching`. A false `matching` is worse than no progress, because it poisons the metric
that the whole project is steered by.

## Never modify the originals

- `../Shandalar/MtG_DotP_SotA/` is the user's real game install. **Read-only. Always.**
- `orig/` holds verified pristine copies. **Read-only.** Verify with
  `sha256sum -c orig/SHA256SUMS.txt` if anything looks off.
- Never commit the contents of `orig/`, game assets, or extracted art. `.gitignore` enforces
  this; do not weaken it. These are copyrighted MicroProse/Wizards of the Coast assets.

## Ground truth beats recollection

This is a 1996-era binary from a defunct publisher. Your training data about it is thin and
probably wrong. For any claim about this software:

1. Establish it from the bytes, with a command someone else can re-run.
2. Write the command into the doc alongside the claim.
3. If you are inferring rather than measuring, write "inferred" next to it.

Community lore (forum posts, wikis, old readmes) is a *lead*, not evidence. Confirm it against
the binary before it becomes a project fact.

## Status ladder

Managed by `tools/progress/progress.py`. A function may only move forward unless you pass
`--force` and say why in `--notes`.

| Status | Means |
|---|---|
| `unstarted` | nothing done |
| `identified` | purpose known, named, signature known |
| `decompiled` | C written that compiles |
| `equivalent` | compiles, behaviourally equivalent, bytes differ |
| `matching` | compiles to **byte-identical** code — the goal |
| `library` | CRT / compiler-helper code; matched by linking, not by hand |
| `nonmatching` | accepted as permanently non-matching; **requires a reason in `--notes`** |

Only `matching` and `library` count toward the headline percentage.

Do not hand-decompile CRT code. If a function is `_ftol`, `__chkstk`, `memcpy`, a stream
routine, or anything else out of the Microsoft CRT, mark it `library` and move on. A high caller
count (say >50) is a strong hint you are looking at library or helper code.

## Workflow for one function

```bash
PY="$HOME/AppData/Local/Programs/Python/Python312/python.exe"

# 1. look at it
"$PY" tools/verify/funcbytes.py dump orig/Shandalar.exe <rva-hex> <size>

# 2. write C into src/<subsystem>/<tu>.c

# 3. build, then check the match
"$PY" tools/verify/funcbytes.py hash orig/Shandalar.exe reports/ghidra/Shandalar.exe.functions.csv

# 4. record the outcome — honestly
"$PY" tools/progress/progress.py set Shandalar.exe <addr> --status matching \
      --name <RealName> --tu sidlib/lib.c --subsystem render

# 5. refresh the numbers
"$PY" tools/progress/progress.py report
```

Never edit `progress/progress.db` by hand. Go through the CLI so the audit trail and the
forward-only rule hold.

## Naming

Use the names the original authors used wherever they can be recovered — from exports, from
`assert()` strings, from format strings, from resource strings. A recovered original name is
always better than an invented one.

When you must invent, follow the surrounding subsystem's convention. Assign the translation unit
(`--tu`) from the recovered source tree in `docs/01-binary-inventory.md`; if you do not yet know
the TU, leave it unset rather than guessing — a wrong TU assignment is harder to detect later
than a missing one.

## Two different targets — do not conflate them

The shipped `Shandalar.exe` / `Magic.exe` are **Mok v1.3.2 binary patches** of MicroProse's 1998
build, not clean build output. `Magic.exe` has an added RWX `.mok` section.

So "reproduce the shipped file byte-for-byte" ≠ "reproduce MicroProse's 1998 build byte-for-byte".
The project targets the **1998 MicroProse build**; Mok's patch is modelled separately as a
post-link transformation. Whenever you touch code near a `.mok` hook, say which target you mean.

## Tool locations on this machine

```
Ghidra 12.1.2   C:\Users\bo1026\Desktop\ghidra_12.1.2_PUBLIC
JDK 21          C:\Users\bo1026\Desktop\jdk-21.0.12+8        (set JAVA_HOME)
Python 3.12     %LOCALAPPDATA%\Programs\Python\Python312\python.exe
MSVC 14.51      C:\Program Files\Microsoft Visual Studio\18\Professional
                (x86 target via VC\Auxiliary\Build\vcvarsamd64_x86.bat)
LLVM            C:\Program Files\LLVM\bin
binutils        MinGW (objdump, strings, nm) — on PATH
```

`python` on PATH is the Microsoft Store stub and does **not** work. Use the full path above.

## Shell

Both PowerShell and Git Bash are available and take different syntax. Bash is generally the
better fit for this project's tooling. Do not chain `sleep` to wait on background work — use
`run_in_background` and wait for the notification.

## Before you end a session

1. `"$PY" tools/progress/progress.py report` — regenerate the metrics.
2. Commit, with the honest state in the message.
3. Append anything surprising to `docs/notes/` — the next session starts cold.
