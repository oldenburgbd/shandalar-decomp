# Toolchain

**Status: resolved and validated end to end, 2026-07-30.** This was the project's top risk. It
is closed.

## The compiler

The originals were built with **Microsoft Visual C++ 4.2** (1996). Every in-scope module reports
linker version 4.20 except `Gcconn.dll` (5.00, and third-party).

Installed at **`C:\Tools\msvc42\MSDEV`** from genuine *Visual C++ 4.2 Enterprise Edition* media
(`disk01.iso`), copied out of `D:\MSDEV`:

| Component | |
|---|---|
| `BIN\` | 30.0 MB — `CL`, `C1`, `C2`, `LINK`, `NMAKE`, `LIB`, `RC`, `CVTRES`, `DUMPBIN`, `MSPDB41.DLL` |
| `INCLUDE\` | 7.1 MB, 401 entries |
| `LIB\` | 29.2 MB, 121 files — incl. `KERNEL32`, `USER32`, `GDI32`, `MSVCRT`, `LIBC`, `LIBCMT`, `LIBCD`, `LIBCMTD` |
| `CRT\` | 6.7 MB |

`HELP`, `SAMPLES`, `MFC`, `DEBUG`, `REDIST`, `MAC`, `MSQUERY` were not copied — nothing in the
target set uses MFC, and they are large.

### Version, verified rather than assumed

```
CL   -> Microsoft (R) 32-bit C/C++ Optimizing Compiler Version 10.20.6166 for 80x86
LINK -> Microsoft (R) 32-Bit Incremental Linker Version 4.20.6164
```

Compiler 10.20 ⇒ `_MSC_VER == 1020` ⇒ Visual C++ 4.2 (4.0 = 1000, 4.1 = 1010).

**`C2.EXE` MD5 = `DCD69F1DD28B02DD03DD7ED02984299A`** (549,888 bytes) — byte-identical to the
build the LEGO Island (`isle`) decompilation standardised on. This is the canonical known-good
MSVC 4.2 back end, not merely *a* 4.2.

### It runs natively on Windows 11 x64

No DOSBox, no VM, no Wine. Two requirements:

- `C:\Windows\SysWOW64\msvcrt40.dll` — already present on this machine.
- `MSPDB41.DLL` must sit next to `CL.EXE` / `LINK.EXE`, or they exit `0xC0000135` silently.

Paths containing spaces are fine — tested explicitly. Do not restructure anything for that.

## Verification: the archived build reproduces exactly

The archived first attempt's tree was rebuilt with this install and re-scored with reccmp
against `orig/Shandalar.exe`:

| | Archive's record | Our rebuild |
|---|---|---|
| Scored functions | 784 | **785** |
| **Byte-exact (100%)** | **194** | **194** |
| Stubs | 195 | 195 |
| `Spr_GetFrameExtent` | 68.89% | **68.89%** |
| `Spr_LoadFileMax` | 66.67% | **66.67%** |
| Aggregate accuracy | — | 85.94% |

Per-function percentages reproduce to two decimal places. That is conclusive: this install is
bit-identical in behaviour to the one that produced the archived results, so every finding in
`salvage/ARCHIVED-CLAUDE.md` about MSVC 4.2 codegen carries over directly.

## Build flags (established empirically by the first attempt)

```
sidlib  /nologo /c /O2 /G5 /W3 /Zi /MD /Iinclude /Fd<pdb> /D_WINDOWS /DWIN32
game    /nologo /c /Od /G5 /W3 /Zi /MD /Iinclude /Fd<pdb> /D_WINDOWS /DWIN32
link    /nologo /DLL /DEBUG /DEBUGTYPE:CV /INCREMENTAL:NO /PDB:<pdb>
libs    kernel32.lib user32.lib gdi32.lib
```

Each of these is load-bearing, and the reasons were measured, not guessed:

- **`/MD`** — the originals import `MSVCRT.dll`. With the static CRT the compiler inlines things
  like `_fileno` and every CRT call diffs against the original's import thunks.
- **`/G5`** (Pentium scheduling) — worth roughly 7 aggregate points and takes several functions to
  exactly 100%. `/G3`, `/G4` and the default `/GB` all produce a byte-identical object; only
  `/G5` differs.
- **`/INCREMENTAL:NO`** — `/DEBUG` turns incremental linking on by default, which routes every
  call to another of our functions through a jump thunk. The originals call directly.
- **`/O2` for `sidlib`, `/Od` for the game layer.** The game layer is genuinely unoptimised:
  frame pointers everywhere, locals spilled, long-form conditional jumps over one-instruction
  bodies. `window.c` is likewise `/Od`. Building it `/O2` drops it to near-zero similarity.
- **`/Zi /DEBUGTYPE:CV`** — emits the CodeView PDB that reccmp reads to locate our functions.

## Verification tool

**reccmp 0.1.6**, installed into the project Python. Same tool the `isle` project uses.

```bash
REC="$LOCALAPPDATA/Programs/Python/Python312/Scripts/reccmp-reccmp.exe"
"$REC" --paths orig/Shandalar.exe build/shandalar_decomp.dll \
       build/shandalar_decomp.pdb . --no-color
"$REC" --paths ... -v 0x57b710      # assembly diff for one function
reccmp-decomplint.exe --module SHANDALAR src include
```

reccmp locates our functions through `//` line-comment annotations above the **definition**:

```c
// FUNCTION: SHANDALAR 0x0057b710
int Spr_LoadFile(void **frames, const char *path)

// GLOBAL: SHANDALAR 0x0073a828
void *g_sprArenaBase;

    // STRING: SHANDALAR 0x005a17cc
    fp = fopen(path, "wb");
```

Block comments are silently ignored, which looks exactly like "reccmp didn't find my function".
`GLOBAL` and `STRING` markers are not optional decoration — without them every instruction
touching a global or a literal scores as a mismatch.

## Gotchas

- **Git Bash mangles `/flags` into paths.** `CL.EXE /nologo` arrives as
  `CL.EXE C:/Program Files/Git/nologo`. Set `MSYS_NO_PATHCONV=1` and `MSYS2_ARG_CONV_EXCL='*'`,
  or drive the compiler from `cmd`/PowerShell. The archived `build.cmd` is a batch file for
  exactly this reason.
- **NMAKE 1.x does not relink the object it just compiled.** It snapshots timestamps at start
  and never re-stats a prerequisite after rebuilding it, so the run that recompiles `sprite.obj`
  prints *"up-to-date"* and skips the link — the new code reaches the DLL only on the *next*
  build. This silently feeds reccmp the previous build. `build.cmd` loops NMAKE until no tool is
  invoked; do not "simplify" that away.
- VC 4.2 predates the modern `vcvars` layout and chokes on some spaced paths in `VCVARS32.BAT`.
  Set `INCLUDE` / `LIB` / `PATH` directly instead of calling it.

## Environment

```bash
export MSVC42='/c/Tools/msvc42/MSDEV'
export INCLUDE='C:\Tools\msvc42\MSDEV\INCLUDE'
export LIB='C:\Tools\msvc42\MSDEV\LIB'
export PATH="$MSVC42/BIN:$PATH"
export MSYS_NO_PATHCONV=1 MSYS2_ARG_CONV_EXCL='*'
```
