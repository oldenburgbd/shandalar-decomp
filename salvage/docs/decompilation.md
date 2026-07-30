# Decompilation environment and findings

Status: **environment stood up, first two formats verified against the binary** (opens Phase 2,
supports #11). Nothing in this document is game content — only facts about the binaries.

## The binaries

Everything below is measured from the real install, not assumed.

| Fact | Value | How we know |
| --- | --- | --- |
| Compiler | **Microsoft Visual C++ 4.2** | PE linker version `4.20` on every original binary |
| Language | **C, not C++** | zero RTTI type descriptors, zero `.cpp` strings, every leaked source path is `.c` |
| Graphics | **GDI only, no DirectX** | imports are kernel32/user32/gdi32/comctl32/comdlg32/winmm/msvfw32; software rendering into a `CreateDIBSection` DIB |
| Symbols | stripped | no debug directory |
| Relocations | none in the EXEs | fixed image base `0x400000`, so every address is stable |
| CRT | dynamically linked | `msvcrt.dll` imported, so `.text` is nearly all game code |

`Shandalar.exe` is the untampered adventure build and is our reference binary.
**`Magic.exe` carries an extra `.mok` section** marked read/write/**execute** containing
PEB-walking code and a `SystemParametersInfoA` reference — almost certainly a crack added by the
"RIP-Version" repack, not MicroProse code. Do not use `Magic.exe` as a decompilation base without
first obtaining a clean copy.

## Recovered original source layout

`assert()` is compiled in, so the binary carries `__FILE__` / `__LINE__` for every assert site.
`ghidra-work/scripts/assertmap.py` walks `.text` for `push offset "<path>.c"` and reads the
preceding `push` as the line number, yielding 29 `(file, line, address)` triples in
`Shandalar.exe`. That recovers the original tree:

```
D:\Newmagic\multiplayer\NedCard\   Catalog.c  haar.c  Palette.c
D:\Newmagic\multiplayer\sid\       glue.c  Ai.c  iniFile.c  Minit.c  Test.c
D:\NewMagic\sources\sidlib\        lib.c  text.c  sprite.c  Fileio.c  Pcxw.c
```

`sidlib` is MicroProse's in-house 2D engine, `NedCard` the card-art subsystem, `sid` the game
layer. **The linker laid translation units out contiguously**, so these addresses partition
`.text` into modules:

**sidlib ships several entry points in two forms**, one taking a `SidContext *` and one taking a
bare page index — `Sid_GetPixel` (0x005795f0) and `Sid_GetPixelPage` (0x00579520) are 199 and 198
bytes and identical instruction for instruction apart from the extra `mov eax,[eax]` that reads
`ctx->page`. `Sid_PutLine` and `Sid_GetLine` take a page index; the drawing primitives take a
context. When a function's neighbours look like near-duplicates of it, they probably are, and the
second one is nearly free once the first is done. Enumerating function boundaries with
`scripts/bounds.py` over a range is worth doing before assuming a gap is empty — that is how
`Sid_GetPixelPage` was found, sitting unannotated between two functions that were already
reconstructed.

| Module | Address range | Contents |
| --- | --- | --- |
| `sidlib/lib.c` | `0x00579050`–`0x0057a4c0` | graphics pages: `CreateDIBSection`, `CreateFileMapping`, `BitBlt`, `StretchBlt`, `SetDIBitsToDevice`, palette |
| `sidlib/text.c` | `0x0057a4e0`–`0x0057b430` | font loading, `CreateFontA`, `TextOutA` |
| `sidlib/sprite.c` | `0x0057b440`–`0x0057c57f` | `.spr` load + blit |
| `sidlib/Fileio.c` | `0x0057c580`–`0x0057d17f` | file open helpers |
| `sidlib/Pcxw.c` | `0x0057d180`–`0x0057de2c` | **actual PCX files**, not `.pic` |

## `MPS_CODE` is hand-written assembly

Every MicroProse binary has a `MPS_CODE` section (`0x4A8` bytes, marked initialized-data
read/write, no `CODE` flag) that nonetheless contains executable code. In `Shandalar.exe` it sits
at `0x0098a000` and **contains the entire `.pic` codec**, written by hand in assembly
(`LODSW`/`STOSW`/`LOOP`, manual `push`/`pop` around calls). This is why the `.pic` decoder is not
in any `.c` file — `Pcxw.c` handles genuine PCX images for a different purpose.

## Verification: `.pic` (confirms `Shandalar.Core.Formats.Pic`)

Decompiled `Pic_ReadBlocks` (`0x0098a000`) and `Pic_DecodeImage` / `Pic_DecodeRows`
(`0x0098a245` / `0x0098a300`). Every claim in `docs/formats/pic.md` is confirmed by the original:

- Block loop compares the tag word against `'M0'` (`0x304d`) and `'M1'` (`0x314d`) and breaks out
  of the loop when the tag's first byte is `'X'`.
- The image subtype comes from `(tag >> 8) & 1` — `X0` → 0, `X1` → 1 — matching our `fourBit`.
- LZW init is literal: start width 9, mask `0x1FF`, threshold `0x100`, dictionary cap `0x800`
  (2048 entries of 3 bytes at `0x5a18c0`), and `max_bits` **clamped to 11** (`CMP AL,0xB` /
  `MOV AL,0xB`).
- RLE uses control byte `0x90`; `0x90 0x00` emits a literal `0x90`; otherwise `DEC AL` gives
  `CC - 1` additional copies of the previously emitted byte.
- `X1` splits each byte as low nibble = left pixel, high nibble = right pixel, and the per-row
  count is `(width + 1) >> 1` — exactly our row padding.

**Q-002 is confirmed at the source.** The original reads the block length field into `0x5a58c8`
and *never reads it back*; the decoder bounds output by `width * height` instead. Ignoring that
field was correct, not a workaround.

One apparent discrepancy resolved: the original issues three `LODSW` for the image header and a
fourth for `max_bits`, which reads as a 6-byte header against our 5. Real files settle it —
`byte[+8]` is `0x0B` in every file while `byte[+9]` varies randomly, so `max_bits` is one byte and
the stream starts at `+9`. The assembly pre-loads the first stream byte into the bit accumulator
(`[0x5a58e4] = 8`, i.e. 8 bits already buffered) as a hand optimisation. **Our decoder is right.**

## Recovered type: the graphics page (`lib.c`)

`sidlib` renders into "graphics pages" — offscreen DIB sections backed by named file mappings.
`Sid_AllocPage` (`0x00579050`) allocates `0x30` bytes; the layout is confirmed twice over, by
its own field stores and independently by the accessors, which reach through the page table at
`+0x04`, `+0x20` and `+0x24`.

| Offset | Field | Notes |
| --- | --- | --- |
| `+0x00` | `hFileMapping` | `CreateFileMappingA`, named `"rpbips<N>"` |
| `+0x04` | `hdc` | `CreateCompatibleDC` |
| `+0x08` | `hBitmap` | `CreateDIBSection` |
| `+0x0c` | `hOldBitmap` | displaced by `SelectObject` |
| `+0x10` | `pBitmapInfo` | built by `Sid_BuildBitmapInfo` (`0x0057a440`) |
| `+0x14` | `hPalette` | the global realized palette |
| `+0x18` | `pBits` | DIB section bits |
| `+0x1c` | `sizeBytes` | |
| `+0x20` | `width` | |
| `+0x24` | `height` | |
| `+0x28` | `bpp` | |
| `+0x2c` | `rowPadding` | **pixels**, not bytes, added per row for DWORD alignment |

Details worth keeping:

- **At most 10 pages** (`Sid_Assert(page < 10, lib.c:289, ...)`), and **page 0 is the screen**,
  which takes an entirely different creation path (`0x00578e80`).
- The `BITMAPINFO` is **top-down** (negative `biHeight`), and for 8bpp it carries a 256-entry
  `DIB_PAL_COLORS` index table of `0..255` — so pixel values index the realized palette
  directly. `CreateDIBSection`'s `iUsage` is literally `(bpp == 8)`.
- Row padding is computed in pixels: `rem = ((width*bpp)/8) % 4; padding = rem ? 4-rem : 0`,
  then folded into the width when sizing: `((padding + width) * bpp * height) / 8`.
- The surface is zeroed by hand, dwords first then trailing bytes.

## Recovered type: the palette and colour encoding (`lib.c`)

`0x00986550` is the RGB table behind the palette indices — 256 entries of 4 bytes
(`r, g, b, unused`). The drawing primitives index it as `[index * 4]`.

Colour arguments to the drawing calls are **overloaded on sign**:

| Value | Meaning |
| --- | --- |
| `>= 0` | palette index → `PALETTEINDEX(c & 0xffff)` |
| `< 0` | an RGB value stored negated → `PALETTERGB(...)` |
| `0xff` | plain white (`0x00ffffff`) |
| `0xff000001` | plain white — it is just `-0x00ffffff` |

The odd bit-twiddling Ghidra shows for these is simply the Win32 `RGB` / `PALETTERGB` /
`PALETTEINDEX` macros expanded: `RGB`'s `((WORD)((BYTE)(g)) << 8)` cast is what produces the
distinctive `or ecx, 0x20000` / `shl ecx, 8` shape. Writing the macros reproduces it.

Not every primitive uses the same one — `Sid_ClearPage` builds its brush with `PALETTERGB`
while `Sid_DrawLine` uses plain `RGB` for the same lookup. That asymmetry is in the original;
don't normalise it.

Pixel addressing within an offscreen page is confirmed by `Sid_GetPixel`:
`pBits[(rowPadding + width) * y + x]` — which independently confirms that `rowPadding` is
counted in **pixels**. Page 0 has no readable bit buffer, so `Sid_GetPixel` reads through GDI
and linearly searches the palette, masking the low three bits of each channel when the screen
is 16bpp (`mask = 0xf8`, else `0xff`), returning `-1` after 256 misses.

### Two more compiler idioms

- **Branch polarity is load-bearing.** `Sid_SetPixel` tests `color >= 0` with the *negative*
  case as the jump target. Writing `if (color < 0)` first inverts the layout and cascades
  through the whole function — it cost about 11 points on its own.
- **Signedness picks the shift.** The same function extracts the blue channel with `sar`, not
  `shr`, so the negated colour is held in a signed `int`. Ghidra's `uint` locals are a hint,
  not evidence; the shift instruction is the evidence.

`Sid_SetPixel` remains the module's outlier at 27.9%. Its structure now matches — the tests,
branch order and both colour arms line up — but the original keeps the colour in `eax` where
ours picks `edx`, and the original carries alignment `nop`s we do not. That is register
allocation, not misunderstanding.

## Recovered type: the drawing context (`lib.c`)

Most drawing primitives take a pointer whose first `int` is a page index, and for a while that
was all we could say about it. `Sid_SetClipRect` (`0x00579ea0`) settles it: it reads and writes
four more fields at `+0x04..+0x10` of the same pointer.

| Offset | Field |
| --- | --- |
| `+0x00` | `page` — index into `g_sidPages` |
| `+0x04` | `clipLeft` |
| `+0x08` | `clipTop` |
| `+0x0c` | `clipRight` |
| `+0x10` | `clipBottom` |

Anything past `+0x14` is still unknown. Note the context stores the clip as **edge pairs** while
`Sid_SetClipRect` hands the previous one back as **(x, y, w, h)** — two conventions in one
function, so don't assume a single rect type across the module.

### Struct returns look like out-parameters

`Sid_SetClipRect` returns its `SidRect` **by value**. MSVC 4.2 implements that with a hidden
pointer parameter, which Ghidra faithfully renders as a leading `undefined4 *param_1` — and
writing it that way scores badly (14.3%). Writing it as a real by-value return, with the result
built in a local and returned, took it to 41.7%. Expect this wherever a function appears to take
an out-pointer it writes exactly once at the end.

## The blitters (`lib.c`)

`Sid_BlitPage` (`0x00579890`, **47 callers**) is the module's workhorse. It copies a rect
between pages and picks one of three strategies:

1. **Destination is the screen**, `srcX` is 8-pixel aligned, and the fast path is enabled →
   vertically flip the source rows in place, then `SetDIBitsToDevice`. The pages are top-down
   but the call is handed a positive `biHeight`, so the rows must be reversed first. **The flip
   is left in place** — the source page is modified as a side effect.
2. **Both pages offscreen at equal depth** → per-row `memmove`. Copying within one page
   downwards would overwrite rows still to be read, so `src == dst && dstY < srcY` walks
   forwards and every other case walks backwards.
3. Otherwise → GDI `BitBlt`.

`Sid_BlitPageRestore` (`0x00579bf0`) is the same screen path with the flip undone afterwards,
so the source survives; it has no page-to-page branch at all. Curiously its two flips use
*different* scratch buffers (`0x005a1618` outbound, `0x005a161c` inbound), and `Sid_BlitPage`
uses a third (`0x005a1614`).

The row swap goes through `Sid_FastMemcpy` (`0x00566cea`), a `memcpy` moving **8 bytes per
`FLD`/`FSTP` pair** — the x87 stack used as a wide move unit. Worth setting against the
survey's "no floating point" result: that stays true in the sense that matters (no FP
*arithmetic* to model), but the FPU does show up as a bulk-copy device.

Two quirks came out of these: **Q-006** (a branch on `srcY` whose arms are identical) and
**Q-007** (`rowPadding` treated as pixels in one place, bytes in another — they coincide only
at 8bpp). Both reproduced verbatim; see `docs/quirks.md`.

## Verification: `.spr` (confirms `Shandalar.Core.Formats.Spr`)

`Spr_LoadFile` (`0x0057b710`, assert `sprite.c:163`) and `Spr_DrawScaled` (`0x0057bfa0`,
50 callers) confirm the container and header:

```c
n = 0;
while (*(int *)buf != -1) {        /* 0xFFFFFFFF terminator      */
    frames[n++] = buf;
    buf += *(int *)buf;            /* leading dword = frame size */
}
```

Frame header offsets used by the blitter: **width `+4`, height `+6`, emptyLines `+12`, row data
`+16`** — all matching `Spr.cs`.

Two deltas worth tracking:

1. **`+14` is a real field we ignore.** `Spr_DrawScaled` reads `*(short *)(frame + 0x0e)`;
   `Spr.cs` reads `+8` and `+10` (as `u1`/`u2`) and never reads `+14`. Its meaning is not yet
   established — needs a follow-up issue before the field is named.
2. The original does **no bounds checking** in the frame walk (no `size < 16`, no end-of-buffer
   test). Ours does. That is a safety addition that cannot change behaviour on valid files, but it
   is a deliberate deviation and should be recorded as such.

## Recovered design: the sprite build arena (`sprite.c`)

Four functions at `0x0057b650`–`0x0057b6c0`, none of which carries an assert and so none of
which can be pinned to a file by string evidence. They are placed in `sprite.c` because they sit
between `text.c`'s last function (`0x0057b620`) and `sprite.c`'s first *proven* one
(`0x0057b710`), and because the arena's consumer is the frame builder at `0x0057b840`. The
attribution is inference; it is also cost-free to be wrong about, since both translation units
build `/O2`.

Sprites are not assembled frame-by-frame into individual allocations. Instead:

| Address | Name | What |
| --- | --- | --- |
| `0x0057b650` | `Spr_ArenaBegin` | `malloc(0x200000)`; base and cursor both set to it |
| `0x0057b840` | *(not yet done)* | appends a frame, bumps the cursor, returns an interior pointer |
| `0x0057b670` | `Spr_ArenaEnd` | writes `0xFFFFFFFF` at the cursor, shrinks the block to fit |
| `0x0057b6a0` | `Sid_Free` | `free()` wrapper, used binary-wide (30+ call sites) |

Nineteen functions across the binary call `Spr_ArenaBegin` and `Spr_ArenaEnd` as a matched pair.
The globals are `g_sprArenaBase` (`0x0073a828`) and `g_sprArenaCur` (`0x0073a82c`).

The load-bearing detail is that `Spr_ArenaEnd` shrinks with **`_expand`, not `realloc`**. By that
point the builder has already handed interior pointers into the block out to its callers, so the
block must not move — and `realloc` is free to move it. `_expand` is the MSVC-specific "resize in
place or fail" allocator, and choosing it is what makes the whole scheme safe. It also creates
the failure mode in Q-011.

The terminator `Spr_ArenaEnd` writes is the same `0xFFFFFFFF` sentinel `Spr_LoadFile` stops on,
which confirms the arena's output is a `.spr` image: the game builds sprite files in memory in
exactly the format it loads them from disk. `Spr_DumpToFile` (`0x0057b6c0`, one surviving call
site) writes such a block straight out — an authoring helper left in the shipped binary. It
takes the length from `_msize`, so it only works on a pointer that came straight from the CRT
allocator.

## Decompilability survey

`survey.py` decompiles every function and scores how clean the output is. Over `Shandalar.exe`:

| Metric | Value |
| --- | --- |
| Functions | 2,216 (2,190 game code, 26 library-identified) |
| Decompile failures | **0** |
| Unrecovered jump tables | **0** |
| Floating-point functions | **0** |
| Structurally clean (no `goto`, no recovery artifacts) | 95.2% |
| Contain `goto` | 4.1% |
| Contain `undefined` types | 76.4% |
| Total decompiled C | ~131,000 lines |

Two of those zeros matter disproportionately. **No unrecovered jump tables** means every switch
resolved, so the control-flow graph has no holes. **No floating point** means the game is
entirely integer/fixed-point, which removes x87 stack reconstruction — the single worst category
of 1990s decompilation, and the area where Ghidra is weakest.

Volume is concentrated: 875 functions under 200 bytes account for 14k lines, while **80
functions over 3 KB account for 32k lines** — a quarter of the code. Those are where the
difficulty lives.

The `undefined`-type figure is the workload, not a defect: it is the gap between
`*(short *)(param_6 + 4)` and `frame->width`, and it is what type recovery closes.

## Environment

Not in this repo (game-derived; see hard rule 2). Lives at `..\ghidra-work`:

```
ghidra-work/
  bin/      copies of the original binaries (never modify the install)
  proj/     Ghidra 12.1.2 project, Shandalar.exe imported + auto-analysed (~50s)
  scripts/  gh.py (PyGhidra harness), assertmap.py, inventory.py, findconst.py,
            disasm.py, decomp.py, seed.py
  venv/     python 3.11 + pyghidra 3.1.0 + jpype 1.5.2
  out/      generated inventories and decompiled C
```

- Ghidra **12.1.2** at `C:\Tools\ghidra_12.1.2_PUBLIC`, JDK 21.
- Auto-analysis applies the bundled `vsOlder_x86.fidbf` FunctionID database, which names the
  MSVC-era CRT helpers (`strcpy`, `memcpy`, `_atexit`, …) so they need not be decompiled.
- `seed.py` is re-runnable and writes 16 evidence-based function names plus module attribution
  comments on all 29 assert sites into the Ghidra database. Names carry their evidence in the
  function's plate comment.

### Verification toolchain

The original compiler runs on Windows 11. **Visual C++ 4.2** is extracted to
`C:\Tools\msvc42\MSDEV` (from the Internet Archive `en_vc42ent` disc 1) — `CL.EXE`, `C1.EXE`,
`C2.EXE`, `LINK.EXE`, `NMAKE.EXE`, plus `INCLUDE` and `LIB`. It compiles and links, and the
binaries it produces stamp **linker version 4.20**, matching `Shandalar.exe` exactly. No
installer is needed; the toolchain is copy-deployable.

`reccmp` 0.1.6 is installed in the PyGhidra venv. The loop is live end-to-end:

```
decomp/  ->  MSVC 4.2  ->  shandalar_decomp.dll + .pdb  ->  reccmp  ->  % per function
```

Current measured state — **31 functions, 67.02% overall**. The eleven `Sid_DrawText*`
entry points are listed once; they all score identically, for the reason given below.

| Function | Original | Module | Accuracy |
| --- | --- | --- | --- |
| `Sid_BuildBitmapInfo` | `0x0057a440` | lib.c | **100.0%** |
| `Sid_DrawText*` (11 functions) | `0x0057b440`–`0x0057b620` | text.c | 94.1% each |
| `Spr_LoadFile` | `0x0057b710` | sprite.c | 87.6% |
| `Sid_BlitPageToScreenUnchecked` | `0x00579440` | lib.c | 83.3% |
| `Sid_FreeBitmapInfo` | `0x0057a4c0` | lib.c | 83.3% |
| `Sid_DrawLine` | `0x005796c0` | lib.c | 80.0% |
| `Sid_BlitToWindow24` | `0x0057a3d0` | lib.c | 77.4% |
| `Sid_CreateScreenPage` | `0x00578e80` | lib.c | 67.7% |
| `Sid_BlitPageToScreen` | `0x00579400` | lib.c | 59.6% |
| `Sid_FreePage` | `0x00579240` | lib.c | 57.1% |
| `Sid_AllocPage` | `0x00579050` | lib.c | 56.4% |
| `Sid_GetPixel` | `0x005795f0` | lib.c | 50.7% |
| `Sid_ClearPage` | `0x00579480` | lib.c | 45.2% |
| `Spr_LoadFileMax` | `0x0057b7a0` | sprite.c | 44.2% |
| `Sid_SetClipRect` | `0x00579ea0` | lib.c | 41.7% |
| `Sid_PutLine` | `0x00579f10` | lib.c | 36.3% |
| `Sid_BlitPage` | `0x00579890` | lib.c | 34.0% |
| `Sid_FillRect` | `0x005797e0` | lib.c | 29.1% |
| `Sid_SetPixel` | `0x00579760` | lib.c | 27.9% |
| `Sid_BlitPageRestore` | `0x00579bf0` | lib.c | 24.0% |
| `Sid_GetLine` | `0x0057a000` | lib.c | 18.0% |

### Optimisation settings are NOT uniform across the binary

This is the most important build finding so far, and it invalidates the earlier claim that
"`/O2` is the right optimisation level" as a global statement. It is right for *some*
translation units.

The four window-identity helpers at `0x00578a71`–`0x00578c02` are compiled **without
optimisation**. The tells are unmistakable in the disassembly:

- full frame pointer setup (`push ebp; mov ebp,esp; sub esp,N`) in a leaf function,
- `ebx`/`esi`/`edi` pushed and never used,
- the return value spilled to a stack slot and immediately reloaded,
- `jmp` to the literally following instruction,
- long-form conditional jumps (`0F 84`) where a short one would do,
- `GetWindowLongA`'s offset argument loaded from an `.rdata` constant rather than folded to an
  immediate.

Meanwhile `Sid_BuildBitmapInfo` — a few hundred bytes away in `lib.c` — matches **byte for byte
at `/O2`**. So these are different object files with different flags, not a uniform build.

Rebuilding just this file with `/Od`:

| Function | `/O2` | `/Od` |
| --- | --- | --- |
| `Sid_WindowMatchesKey` | 12.7% | **53.7%** |
| `Sid_WindowHasId` | 13.6% | **67.4%** |
| `Sid_GetWindowId` | 23.3% | **68.8%** |
| `Sid_GetWindowExtra8` | 0.0% | **59.3%** |

**Method consequence:** a near-zero score on a small, obviously-understood function is evidence
about *build flags*, not about the reconstruction. Before rewriting such a function, check its
disassembly for the `/Od` tells above. The Makefile now carries a `CFLAGS_OD` variant and
per-file rules; expect more translation units to need it.

**Attribution consequence:** these four cannot be part of `lib.c`, which is `/O2`. They are an
unidentified translation unit, kept in `decomp/src/window.c` rather than guessed into a module.

### `/INCREMENTAL:NO` is required

`/DEBUG` turns incremental linking on by default, which routes every call to another of our own
functions through a jump thunk. The original calls directly, so each such call diffs. Turning it
off is worth a couple of points on any function that calls a sibling
(`Sid_BlitToWindow24` 75.3% → 77.4%, `Sid_CreateScreenPage` 66.2% → 67.7%).

### The `text.c` entry-point family — the best waterfall so far

`0x0057b440`–`0x0057b620` is eleven consecutive 42-byte functions that are pure forwarders into
one core (`Sid_DrawTextCore`, `0x0057b230`), each pinning a different combination of four layout
flags:

| Flag | Effect |
| --- | --- |
| `shadow` | also draw at (x+1, y+1) in colour 0 underneath |
| `resScaled` | treat x/y as 640×480 coordinates and scale to the real screen |
| `hCenter` | subtract half the measured line width from x |
| `vCenter` | subtract half (font height × line count) from y |

The core formats through `_vsnprintf` into a 1 KB buffer, splits on `\n`, and draws line by
line. The game uses only 11 of the 16 possible flag combinations. Understanding one function
produced eleven at 94.1% in a single step — worth remembering as the shape of cheap progress:
**look for repeated small functions before tackling big ones.**

They are all stuck at exactly 94.1% because the one differing instruction is the `call` to the
core, which cannot match until the core itself is reconstructed. That is a ceiling, not a defect.

**Module boundary correction:** these wrappers sit above `0x0057b430`, so `text.c` extends to
about `0x0057b64f` and `sprite.c` starts at the allocation helpers around `0x0057b650` — later
than the earlier estimate in this document's module table.

Previous state — **20 functions, 52.36% overall**:

| Function | Original | Module | Accuracy |
| --- | --- | --- | --- |
| `Sid_BuildBitmapInfo` | `0x0057a440` | lib.c | **100.0%** |
| `Spr_LoadFile` | `0x0057b710` | sprite.c | 87.6% |
| `Sid_BlitPageToScreenUnchecked` | `0x00579440` | lib.c | 83.3% |
| `Sid_FreeBitmapInfo` | `0x0057a4c0` | lib.c | 83.3% |
| `Sid_DrawLine` | `0x005796c0` | lib.c | 80.0% |
| `Sid_BlitToWindow24` | `0x0057a3d0` | lib.c | 75.3% |
| `Sid_CreateScreenPage` | `0x00578e80` | lib.c | 66.2% |
| `Sid_BlitPageToScreen` | `0x00579400` | lib.c | 59.6% |
| `Sid_FreePage` | `0x00579240` | lib.c | 57.1% |
| `Sid_AllocPage` | `0x00579050` | lib.c | 55.3% |
| `Sid_GetPixel` | `0x005795f0` | lib.c | 50.7% |
| `Sid_ClearPage` | `0x00579480` | lib.c | 45.2% |
| `Spr_LoadFileMax` | `0x0057b7a0` | sprite.c | 44.2% |
| `Sid_SetClipRect` | `0x00579ea0` | lib.c | 41.7% |
| `Sid_PutLine` | `0x00579f10` | lib.c | 35.1% |
| `Sid_BlitPage` | `0x00579890` | lib.c | 30.7% |
| `Sid_FillRect` | `0x005797e0` | lib.c | 29.1% |
| `Sid_SetPixel` | `0x00579760` | lib.c | 27.9% |
| `Sid_BlitPageRestore` | `0x00579bf0` | lib.c | 23.5% |
| `Sid_GetLine` | `0x0057a000` | lib.c | 18.0% |

**Read these numbers carefully — they are noisier than they look.**

The aggregate is a coverage-weighted mean, so adding large below-average functions lowers it
even though nothing regressed: 61.1% → 57.8% → 55.3% → 52.4% purely by adding work.

More importantly, **per-function scores move when neighbouring functions change, with no edit
to the function itself**. Across the last two rounds, without being touched:

| Function | Moved | |
| --- | --- | --- |
| `Spr_LoadFile` | 62.9% → 87.6% | up |
| `Sid_SetPixel` | 27.9% → 51.8% → 70.6% → 27.9% | both ways |
| `Sid_FreePage` | 57.1% → 71.4% → 57.1% | both ways |

The likely cause is that our DLL packs functions in source order while the original has them at
fixed addresses, so adding code changes inter-function call and thunk offsets, which shifts how
reccmp's diff aligns the two instruction streams. That is a hypothesis, not a confirmed
diagnosis — it has not been investigated properly yet.

**The practical rule: trust the per-function assembly diff (`-v <addr>`), not the score, and
don't treat a single function's percentage as meaningful until its module is complete.** Score
movements of this size should not be reported as progress or regression on their own.

**`/O2` is the right level for `sidlib`**, confirmed by sweeping all four across the module set:
`/O2` 57.82%, `/Ox` 57.77%, `/O1` 33.34%, `/Oy` 29.64%. It is **not** right for the whole
binary — see the section on non-uniform optimisation settings below.

**`Sid_BuildBitmapInfo` is byte-exact.** That settles the feasibility question: with MSVC 4.2
and this build configuration, 100% matching is reachable, not just approachable.

### Compiler idioms learned so far

These generalise to all 2,190 functions, which is why calibration is worth doing before scale.

- **`/MD` is correct, `/MT` is not** — 52% vs 37.5% on identical source. The original imports
  `msvcrt.dll`, and the static CRT additionally inlines things the original calls out to.
- **`_fileno` must have its macro suppressed.** MSVC 4.2's `stdio.h` defines
  `_fileno(s) ((s)->_file)`, but the original calls the imported function, so the original
  sources had the macro undefined. Adding `#undef _fileno` moved `Spr_LoadFile` 52% → 62.9%
  and `Spr_LoadFileMax` 38.4% → 42.3%. Expect the same for other macro/function CRT pairs.
- **`Sid_Assert`'s first argument is per-site, and the disassembly tells you which.** Pointer
  tests are pushed raw — the original pushes the `FILE*` or the `HANDLE` straight onto the
  stack, so writing `fp != NULL` wrongly adds a `cmp/sbb/inc` normalisation. Integer tests are
  written as comparisons and *do* normalise: `page != 0` is `cmp esi,1 / sbb eax,eax / inc eax`,
  and `page < SID_MAX_PAGES` is `cmp/setl`. Passing the raw value where the original wrote
  `page != 0` cost `Sid_FreePage` 8 points; fixing it took the function to byte-exact. Read the
  three instructions before the push and match them, rather than applying either habit by rote.
- **Loop-variable caching is neutral.** Measured, a no-op; don't spend time on it.
- **Local declaration order is the tie-breaker between locals whose live ranges start at the
  same point** — worth 22 points on `Spr_GetFrameExtent`. This entry previously read "declaration
  order does not affect register allocation", which was measured but over-generalised: it holds
  only when live ranges already rank the locals. See "Declaration order breaks ties" below, and
  note that declaration *presence* matters regardless.
- **`char name[16] = "rpbips";` — an initialised array, not a `memcpy`.** The tell is the tail:
  the original reserves exactly 16 bytes, copies 7 from the literal, then zeroes the remaining 9
  with three stores. No `memcpy(name, "rpbips", 7)` produces that trailing zero-fill, and nothing
  in either function ever reads `name[7..15]`. Recognising it was worth +11.02 on
  `Sid_CreateScreenPage`. When a fixed-size stack buffer is partly written from a literal and the
  rest zeroed, count the bytes: literal length plus zero-fill equals the array size is the
  signature of an initialiser.
- **Declaring an unused local perturbs register allocation across the whole translation unit.**
  Measured in both directions and reproducible. Deleting three genuinely unused locals from
  `Sid_BlitPage` and `Sid_BlitPageRestore` — variables left behind when their copy loops became
  `memcpy` calls, and which generate no code — moved `Sid_GetLine`, a function in the same file
  that was not edited at all, from 91.89% to 78.38%. Its page-pointer load swapped EAX for ECX
  while every other instruction stayed identical. Restoring the declarations restored the score.

  **It reaches across files too.** Adding one function prototype to `include/sidlib/sidlib.h`
  changed register allocation inside `sprite.c` — a translation unit that was not edited and
  that does not even call the new function at its point of declaration. `Spr_LoadFileMax` swung
  82.46% ↔ 63.16% on the presence of that single line, worth about one point of aggregate.
  Verified by removing the line and putting it back.

  So **any header edit reshuffles scores across every translation unit that includes it**.

  **This was first written up as roughly ±1 point of aggregate noise. That estimate was far
  too generous.** Adding a struct, a global and six prototypes to `sidlib.h` — while touching
  nothing in `lib.c` at all — took `Sid_SetPixel` from a 100% effective match to **31.11%**,
  `Sid_BlitToWindow24` from 100% to 77.42%, and `Spr_LoadFileMax` from 82.46% to 61.40%. The
  `Sid_SetPixel` diff is the same EAX-versus-EDX permutation that reconstructing the function
  correctly had already fixed once: the source is untouched and still right, and the register
  allocator simply chose differently because the translation unit now declares more symbols.

  A single function can therefore swing 70 points on an edit to a file it does not read from.
  Per-function scores are only comparable **within one build**; comparing a function's score
  across builds that differ in any header is meaningless unless a `-v` diff confirms it.

  **Fixed.** The single `sidlib.h` was split into one header per module — `sidcore.h`,
  `sidgfx.h`, `sidtext.h`, `sidsprite.h`, `sidfileio.h`, `sidwindow.h` — and each translation
  unit now includes only its own plus the core. `sidcore.h` holds the genuinely shared surface
  and nothing else: the assert reporter, the source-path macros, the `_fileno` suppression,
  `Sid_FastMemcpy` and `SidContext`. Cross-module references are single declared lines with a
  comment naming the owning module, never an include of another module's header.

  The split paid for itself immediately, taking the aggregate 90.42% → **92.30%** and restoring
  `Sid_SetPixel` to a clean 100%, because `lib.c` stopped seeing the font and file-io
  declarations that had displaced its register allocation.

  It was then verified rather than assumed. Adding three throwaway prototypes to `sidtext.h`,
  rebuilding clean, and diffing all 58 per-function scores against the baseline produced **zero
  differences anywhere** — `text.obj` was genuinely rebuilt, and no other module moved by so
  much as a hundredth of a point. Before the split the same class of edit moved `Sid_SetPixel`
  by 69 points.

  `src/stubs.c` is the one file that includes everything, which is safe precisely because
  nothing in it carries a `FUNCTION` marker: its register allocation is never scored. Keep it
  that way. **Do not merge these headers back together, and do not include a module header you
  do not actually need.**

  Two limits worth being clear about. The split cost a **one-time re-roll**: shrinking each
  module's visible declarations changed its allocation once, which is why `Spr_LoadFile` sits
  at 68.52% having been 92.59% before, with every instruction still matching and only EBX and
  EDI swapped. And it fixes *cross*-module churn only — **adding a function to a module still
  re-rolls that module's other functions**, which is what took `Sid_FillRect` from 92.91% to
  57.14% when `Sid_StretchBlitPage` was added to `lib.c`. Functions already at 100% are stable
  under this; ones in the middle drift. Nothing to do about it beyond finishing functions
  properly, but do not read those movements as regressions in the work.

  **Annotate every stub, including the ones whose signature you are unsure of.** `Sid_Assert`
  was deliberately left unmarked on the grounds that its signature was unconfirmed. That was a
  mistake: a `STUB` marker asserts only *where* a function lives, never that the body matches,
  and withholding it left an unresolvable `call` in every assert-using function whose score
  then swung with DLL layout. Marking it (0x0046a520) lifted `Sid_GetLine` to 94.59%,
  `Sid_FreePage` to 86.13% and `Spr_LoadFileMax` to 85.96%.

  The plausible mechanism (unverified) is that MSVC 4.2's allocator breaks ties using TU-wide
  internal symbol numbering, which shifts when any declaration is added or removed anywhere in
  the file or its headers. **This is the concrete explanation for the "scores move when neighbouring functions
  change" instability recorded elsewhere in this document** — it is not address relocation, which
  was the earlier hypothesis and is wrong. The differences are genuine register permutations in
  our own generated code.

  Two practical consequences. Do not chase a few points by adding or withholding declarations:
  that is overfitting to the metric. The tree deliberately keeps the warning-free version at
  83.73% over the dead-locals version at 84.03%, and deliberately keeps the `Sid_GetPixelPage`
  prototype in `sidlib.h` at 87.78% over omitting it at 88.83% — the original's header must have
  declared it, because `sprite.c` calls it fourteen times. Where neither choice is more faithful,
  choose the better code and accept the number. And when a function's score moves without that
  function being touched, verify with `-v` before believing either the gain or the loss.
- **Dead comparisons are fossils of folded source branches, and reproducing the branch
  reproduces the instruction.** `Sid_BuildBitmapInfo` emits `cmp esi,0x18` (bpp vs 24) whose
  flags are immediately destroyed by the following `call`. The original enumerated a 24bpp arm
  that computes the same size as the default; MSVC 4.2 folds the bodies but still emits the
  comparison. Writing that redundant `else if (bpp == 24)` took the function from 93.67% to
  **100%**. Expect this pattern wherever a `cmp`/`test` has no consumer — it is evidence about
  the original source, not compiler noise to be ignored.
- **Use the Win32 colour macros where the original used them — and not where it didn't.**
  `GetRValue`/`GetGValue`/`GetBValue` cast to `BYTE`, which compiles to byte-register moves
  (`mov dl,al`, `mov cl,ah`, `shr eax,16; mov cl,al`). Writing the arithmetically identical
  `screen & 0xff`, `(screen >> 8) & 0xff`, `(screen >> 16) & 0xff` compiles to `and reg,0xff`
  instead, and worse, lets the allocator put the value in ESI or EDI — which have no 8-bit
  sub-register, so the byte form becomes impossible and the whole allocation shifts. Switching
  to the macros took `Sid_GetPixel` 63.09% → **97.30%** and its twin 60.15% → 87.84%.

  This is the mirror image of the `PALETTEINDEX` finding above, where the original did *not*
  use the macro and the `(WORD)` cast was wrong. The rule is not "prefer macros" — it is that
  the casts inside these macros are visible in the generated code, so the disassembly tells you
  which spelling the original used. `movzx`/`and` versus a byte move is the tell.
- **At `/Od`, an explicit `else` is not cosmetic.** MSVC 4.2 emits a jump over the else block at
  the end of every then-block and never peepholes it away, even when the then-block already
  returned. So `if (c) { return 1; } else { return 0; }` produces *two* consecutive `jmp`s to the
  epilogue after `mov eax,1`, while `if (c) { return 1; } return 0;` produces one. Five bytes,
  and it took `Sid_WindowMatchesKey` from 84.62% to **100%**. Two consecutive jumps to the same
  target in an unoptimised function mean the source had an `else`.
- **An unoptimised build states its own local layout.** `sub esp,N` gives the exact slot count,
  and a local stored then immediately reloaded into EAX before the epilogue is a named return
  value (`x = f(); return x;`) as opposed to a direct `return expr;`. Both are readable straight
  off the disassembly, so at `/Od` the number of locals and the presence of a result variable are
  facts rather than guesses. That is how the four window helpers were reconstructed.
  **Unresolved:** what orders the slots. Declaration order does not (a variable declared first
  still lands last), first-assignment order fits our build but not the original's, and grouping
  by type was tested and made no difference. Two of the four are stuck at 84.09%/77.78% purely
  on `[ebp-N]` displacements, with every instruction otherwise identical.
- **A hand-written copy loop is not a `memcpy`.** The original expands `memcpy` inline to
  `rep movsd` for the dwords plus `rep movsb` for the tail; a dword-then-byte loop written out
  in C generates a completely different body. `Sid_GetLine` 33.71% → 91.89% and `Sid_PutLine`
  55.06% → 96.30% on this alone, and it also lifted both blitters, whose 256-entry RGBQUAD
  palette copies are really a single 1,024-byte `memcpy` (a bare `rep movsd`, no byte tail,
  because the length is an exact multiple of four). **Whenever a diff shows `rep movsd`/`movsb`,
  the source said `memcpy`.** Note this is unrelated to `Sid_FastMemcpy` (0x00566cea), which is a
  separate x87-based bulk mover outside sidlib.
- **`Sid_Assert`'s argument is usually the raw value but not always.** Most sidlib call sites
  pass the tested value directly. `Sid_GetLine` does not: it emits `cmp/sbb/inc` to normalise
  `page` to 0 or 1, so that site passes `page != 0`. Read the call site rather than applying the
  house rule blindly.
- **A ternary and the equivalent arithmetic are different instructions.**
  `mask = (g_sidScreenBpp == 16) ? 0xf8 : 0xff;` compiles to the original's carry trick
  (`sub eax,0x10` / `cmp eax,1` / `sbb esi,esi` / `and` / `add`). Spelling the same value out
  as arithmetic on the comparison result — `(-(bpp == 16) & 0xfffffff9) + 0xff` — gets
  `sete`/`neg` instead. Both are correct C computing the same number. When a diff shows `sbb
  reg,reg`, the source had a conditional, not arithmetic. Worth 15 points on `Sid_GetPixel`.
- **Hoist repeated sub-expressions out of loop conditions when the original does.** The original
  `Sid_GetPixel` extracts all three colour channels before its search loop, spilling two to the
  stack. Leaving `(screen >> 8) & 0xff` inside the loop condition makes MSVC recompute it every
  iteration — a visibly different loop body.
- **Struct fields are assigned in the order the source assigns them.** `Sid_FillRect` builds a
  `RECT`; writing `left, right, top, bottom` instead of declaration order `left, top, right,
  bottom` keeps `x + w` alive in EAX across the `top` store, which pushes the colour value out
  of EAX and permutes the entire second half of the function. 44.80% → 57.14% on the reorder
  alone. When a struct is filled field by field, try declaration order first.
- **Reading the disassembly beats reading the pseudocode.** The functions reconstructed from
  `disasm.py` output score markedly higher on the first attempt than those written from
  `decomp.py` pseudocode. Ghidra typed `Sid_BuildBitmapInfo` as returning `void` and reassigned
  its result variable; the disassembly shows `EAX` holds the allocation throughout while `ECX`
  walks the colour table.

The residual difference in `Spr_LoadFile` is now *purely register allocation*: every
instruction, call and stack offset matches, with `ebx`/`edi` swapped. That means the function
is behaviourally verified even at 62.9%; closing the rest is matching work, not comprehension
work. Worth remembering when reading these percentages — they are a lower bound on
understanding, not a measure of it.

An earlier note here claimed that caching the frame size in a local dropped the score from 61%
to 52%. That was wrong: the drop was entirely the `/MD` switch, measured in the same edit. The
caching change is neutral, as re-measuring one variable at a time showed.

Annotations are `//` line comments above the **definition**, e.g.
`// FUNCTION: SHANDALAR 0x0057b710`. reccmp ignores block comments silently, which presents as
"my function wasn't found". `reccmp-decomplint` validates placement.

### `STUB` is the highest-yield annotation of the lot

`STUB` marks a function whose *address* is known but whose body has not been reconstructed. Its
semantics are exactly what an honest decompilation wants, and they are worth checking in
reccmp's own source rather than taking on trust: a stub is **excluded from both the implemented
count and the accuracy average**, but calls to its address resolve. So the marker asserts "this
is where that function lives" and never "this body matches".

The effect is a waterfall, because every caller of an unannotated stub was counting its `call`
as a mismatch. Annotating four stubs — `Sid_LoadFontFile`, `Sid_FastMemcpy`, `Sid_DrawTextCore`,
`Sid_LookupWindowId` — moved the aggregate 90.01% → **91.80%** with no code change at all:

| | before | after |
| --- | --- | --- |
| the eleven `text.c` entry points | 94.12% | **100%** (all of them) |
| `Sid_BlitToWindow24` | 77.42% | **100%** |
| `Sid_OpenArchive` | 88.89% | **100%** |
| `Sid_GetWindowId` | 77.78% | 83.33% |

The eleven text forwarders had been stuck at 94.12% for several sessions with the *entire*
remaining difference being the unresolvable call to `Sid_DrawTextCore`. They were finished the
whole time.

**Annotate a stub the moment you create one.** A stub without a marker silently caps every
function that calls it, and the cap looks exactly like a reconstruction problem.

### `FUNCTION` is not the only annotation, and the others are worth points

This was being left on the table. reccmp also reads `GLOBAL` and `STRING` markers, and without
them every instruction that touches a global or a literal counts as a mismatch — reccmp sees a
named symbol on our side and a bare address on the original's, and cannot tell they are the
same thing.

Annotating the globals in `src/stubs.c` moved the aggregate from **67.34% to 69.90% with no
code change at all.** `Spr_ArenaBegin` went 66.67% → **100%**: it had been byte-for-byte correct
the whole time and was being scored as two-thirds right. A `STRING` marker on one `fopen` mode
string took `Spr_DumpToFile` 96.43% → **100%**.

```c
// GLOBAL: SHANDALAR 0x0073a828
void *g_sprArenaBase;

    // STRING: SHANDALAR 0x005a17cc
    fp = fopen(path, "wb");
```

Two consequences worth carrying:

- **A low score is not always a reconstruction problem.** Before rewriting C to chase a number,
  read the `-v` diff and check whether the "mismatches" are `<OFFSET>` placeholders. That is a
  missing annotation, not a missing insight.
- **Never invent an address to silence a diff.** A global with no `GLOBAL` marker in `stubs.c`
  is one whose original address has not been established. Annotating it with a guess would
  fabricate a match and corrupt the only measurement the project has.

**Do not put a `STRING` marker inside a multi-line call's argument list.** Placing one before a
continuation line desynchronises reccmp's function tracking: it silently binds each subsequent
`FUNCTION` annotation to the *next* function's symbol, so the report shows real names against
wrong addresses and plausible-looking scores. `reccmp-decomplint` does **not** catch this. The
tell is the report listing a name whose address you know belongs to a different function.

**The other failure mode of the same mistake is the function vanishing entirely, with no
warning.** A `STRING` marker binds to the literal on the line directly below it. Write the call
so the format string wraps onto a continuation line —

```c
// STRING: SHANDALAR 0x005a1840
Sid_Assert(g_sidPcxFile != NULL, SIDLIB_FILEIO_C, 247,
           "Error Opening File %s\r\n", name);      /* marker sees no literal */
```

— and reccmp drops the *enclosing function* from the report. Not a zero score: no row at all.
`Sid_OpenFileCore` was measured at 80.49% one build and simply absent the next, while its four
callers dropped from 100% to ~90% because their `call` target had become unresolvable. There is
no error message; `reccmp-cvdump` still shows the symbol in the PDB, and `--target FACEMAKER`
omits it just as silently. **If a function you have annotated is missing from the report, look
at the marker placement inside it before looking at anything else.** Keeping the literal on the
marker's own line fixes it; that one edit was worth +85.98 on a 608-byte function.

**A `STRING` marker works on a `#define`, which is how the assert paths get annotated.** The
`SIDLIB_*_C` macros expand inside multi-line `Sid_Assert` argument lists — the one place a
marker cannot go. Putting the marker above the `#define` in `sidcore.h` instead covers every
use in every module at once:

```c
// STRING: SHANDALAR 0x005a166c
#define SIDLIB_LIB_C "D:\\NewMagic\\sources\\sidlib\\lib.c"
```

One such line took `Spr_LoadFile` from 94.44% to byte-exact.

**A later sweep found the rest of them, and it was the cheapest change in the project.** Seven
literals in `lib.c` and `sprite.c` plus two path macros, no code change: `Sid_GetLine` and
`Spr_LoadFile` to **100%**, `Sid_CreateScreenPage` +11.02, `Sid_AllocPage` +6.88, `Sid_FreePage`
+5.84, `Spr_LoadFileMax` +5.27. The addresses are all readable straight out of the `-v` diff —
where the original shows `push 0x005a1690` against our `push "..." (STRING)`, that is a missing
marker, not a mismatch to explain — and each was confirmed against the binary with
`rdstr.py <addr>` before being written down.

### `/G5` — the binary is scheduled for the Pentium

**This is the single largest build-flag finding so far: aggregate 71.87% → 78.64% in one
change, and it moved every optimised function at once.**

MSVC 4.2 targets a processor with `/G3` (386), `/G4` (486), `/G5` (Pentium) or `/GB` (blended).
The default is `/GB`, which is what we had been building with. The original is `/G5`. Compiling
`lib.c` under each flag makes this immediately visible: `/G3`, `/G4` and `/GB` all produce a
byte-identical 29,977-byte object, `/G5` produces 30,085.

The Pentium is dual-issue (U and V pipes), and `/G5` schedules for it. Two artifacts follow,
both of which had been misread as evidence about the *source*:

- **Stray single-byte `NOP`s mid-function**, used as padding to align the next instruction.
- **Loads hoisted into the gap between a `cmp`/`test` and its branch**, filling a slot that
  would otherwise stall.

Seven functions went to exactly 100% on this flag alone, and `Sid_SetPixel` went 93.18% → 100%.

**This corrects an earlier entry in this document.** `Sid_Free` (0x0057b6a0) had an unexplained
`push ebx`/`pop ebx` pair around its only call, and that was written up here as proof that MSVC
emits callee-saved pushes lazily at first use, and therefore that the original source contained
some construct we had failed to recover. Both claims were wrong. The pushes are `/G5` scheduling
padding; with the correct flag, `free(block)` reproduces the function byte for byte. The same
mistake had been made about the `push ebp` in `Spr_LoadFileMax`.

The general lesson is worth more than the flag: **an unexplained instruction is a hypothesis
about the build, not only about the source.** Before concluding that reconstructed C is missing
something, exhaust the compiler switches — they are cheap to test, they apply to all 2,190
functions at once, and a wrong conclusion about the source gets written into a comment and
believed later.

`/G5` is a no-op at `/Od` (scheduling is an optimiser pass), confirmed by measurement. It is
still set on `CFLAGS_OD` because the original was evidently built with one processor target
across the whole project, and a flag that varies per translation unit for no reason is a trap
for the next person.

### Reusing a parameter keeps the value in the original's register

`Sid_SetPixel` (0x00579760) went **30.23% → 93.18%** on two source changes, then to **100%**
with `/G5`. Both changes are generalisable:

- **The original transforms the colour parameter in place** rather than building the result in a
  separate local. Every arm of the branch writes back to `color`, which is what keeps the whole
  computation in `EAX`. Introducing a separate `COLORREF rgb` gives the result its own register
  and permutes the allocation of everything downstream, which is most of what the 30% was.
- **`PALETTEINDEX` is the wrong macro.** It expands to `0x01000000L | (DWORD)(WORD)(i)`, and
  that `(WORD)` cast compiles to `movzx`. The original emits `and eax,0xffff`, so the source
  masked the value arithmetically instead of casting it. Writing `0x01000000L | (color &
  0xffffL)` matches. Wherever a Win32 colour macro is suspected, check whether the original
  actually used it — the cast chain inside these macros is visible in the generated code.

### Live range decides register class — worth 35 points on one function

`Spr_LoadFileMax` sat at 42.31% with every instruction present and correct, purely because our
locals landed in different registers than the original's. The cause was one line's *position*:

```c
count = 0;              /* before fopen  -> 77.48% */
fp = fopen(path, "rb");
...
count = 0;              /* beside the loop -> 42.31% */
while (frameSize != -1L && maxFrames > count) {
```

Initialising the counter above the run of `fopen`/`fread`/`fclose` calls makes it live across
them, which forces MSVC to give it a callee-saved register (the fourth one). Initialising it at
the loop lets a scratch register do, and the allocation of every other local shifts to
compensate — so almost every instruction reads as a mismatch.

### Declaration order breaks ties between equal live ranges — worth 22 points on one function

This **corrects** the claim above that declaration order is a no-op. It is a no-op only when
live ranges settle the ranking first, which is the common case and is why it looked like a
general rule. When two locals' live ranges *begin at the same point*, declaration order is what
breaks the tie:

```c
const unsigned char *p;  int rows;   /* Spr_GetFrameExtent: 46.67% */
int rows;  const unsigned char *p;   /* the same function:  68.89% */
```

Both start life immediately after the NULL check, so nothing else distinguishes them; swapping
the two lines swaps which gets `esi` and which gets `edi`, and with `p` on `edi` the entire
run-decoding loop goes byte-exact. No other function moved.

The two rules compose, and the order to apply them in is: live range first, declaration order
second. `Spr_LoadFileMax` is the control — swapping *its* `fp` and `buf` declarations changes
nothing, because those two have plainly different live-range starts (`fopen` vs `malloc`), so
the tie-break never runs.

`register` is **not** a third lever. Measured a no-op under `/O2` in both directions, on a
converged build.

**The operand order of a comparison is not source-visible.** An earlier note here claimed it
was, from `Spr_LoadFileMax`'s `cmp maxFrames, count` / `jle`. But our source already says
`maxFrames > count` and MSVC emits `cmp count, maxFrames` / `jge` anyway — the operand order
follows from which register each value landed in, so it is a *symptom* of the allocation, not a
cause. Do not rewrite a comparison to chase it.

### NMAKE does not relink the object it just compiled

Diagnosed properly. The earlier entry here guessed "timestamp granularity, most likely" and was
wrong: NMAKE snapshots file timestamps when it starts and never re-stats a target's
prerequisites after rebuilding them. The run that recompiles `sprite.obj` prints

```
'build\shandalar_decomp.dll' is up-to-date
```

and skips the link, so the new object reaches the DLL only on the *next* build. Reproduce with
`nmake /f Makefile build\shandalar_decomp.dll` after touching any `.c`.

This is deterministic, not occasional. Every measurement taken from a single build after an edit
was reading the previous DLL — the change appears to do nothing, and its delta then attaches to
whatever unrelated change is measured next. It is what made a +35-point change look worthless.

`build.cmd` now runs NMAKE until it invokes no tools, and `test\score.ps1` warns if the DLL's
timestamp did not move. Grepping the build log for `LINK.EXE` was the previous check; it is a
worse one, and it was written two different wrong ways before being replaced.

`reccmp-project detect --what recompiled` currently fails because it matches the recompiled
artifact by *filename*, and we build a DLL rather than `Shandalar.exe`. Comparison runs via
`--paths` until the build produces a full EXE; see `CLAUDE.md` for the command.

### ReVa (Ghidra MCP bridge)

ReVa **7.3.0** is installed at
`C:\Tools\ghidra_12.1.2_PUBLIC\Ghidra\Extensions\reverse-engineering-assistant`, and
`..\.mcp.json` registers it for Claude Code. It runs *inside* the Ghidra GUI, so to use it:

1. Launch `C:\Tools\ghidra_12.1.2_PUBLIC\ghidraRun.bat`, enable the extension when prompted,
   restart Ghidra, and open `ghidra-work/proj/Shandalar.gpr`.
2. Restart Claude Code so it picks up `.mcp.json`; check with `/mcp`.

ReVa listens on `http://localhost:8080/mcp/message`. Without it, the `scripts/` harness above
covers the same ground headlessly and is what produced everything in this document.

### Magic.exe is a second build of Shandalar.exe, and that changes where to work

**Measured 2026-07-28 with `ghidra-work/scripts/share.py`.** Every function in the corpus had
been chosen inside `sidlib`, on the reasoning that `sidlib` is statically linked into several
binaries so reconstructing it once credits all of them. Checking that assumption against all 2,017
Ghidra functions of 48 bytes or more, rather than against the ~60 we had reconstructed, inverts
it:

| binary | exact matches | bytes | ≥90% partial |
|---|---|---|---|
| `Magic.exe` | 595 | 190,836 (12.4% of Shandalar's code) | 236,882 bytes |
| `Drawcardlib.dll` | 28 | 7,691 | 2,783 bytes |
| `Manalink.exe` | 0 | 0 | 0 bytes |

`Magic.exe` and `Shandalar.exe` are two builds of the same program. Only 223 of 2,017 functions
are absent from `Magic.exe` entirely; the rest are exact or near. It has the same section layout
including an `MPS_CODE` of identical size, and it is not packed (entropy 6.01 against Shandalar's
6.10, with normal `int3` padding).

**Only six of those 595 shared functions are inside `sidlib`.** The library is precisely the part
the two builds do *not* share — which is the real answer to the long-standing puzzle about why
`Magic.exe` matched only four of our functions. It was never evidence that the binaries are
unrelated; it was evidence that we had only ever looked at the one region where they differ.

So the leverage is inverted from what the project assumed:

- A `sidlib` function credits `Shandalar.exe` plus the two small `Facemaker.exe` builds — about
  1.65 MB of the 4.00 MB corpus, and the Facemakers are 90 kB of that.
- A **game** function credits `Shandalar.exe` **and** `Magic.exe` — 2.99 MB, **75% of the
  corpus**.

Every function reconstructed from here should be one of the 595, largest first. `Manalink.exe`,
`Gcconn.dll` and `Manalinkinterface.dll` are a later product built with a different toolchain
(`Gcconn.dll` reports linker 5.00) and share nothing; whether they belong in the denominator at
all is a scope question for the owner.

### The game layer is built `/Od`, and that makes it far cheaper to match than `sidlib`

Every one of the twelve largest functions shared with `Magic.exe` opens `push ebp; mov ebp, esp`,
spills its locals to the frame, and uses long-form conditional jumps over single-instruction
bodies. That is unoptimised codegen. `sidlib` is `/O2` with frame-pointer omission; the game layer
is not.

This matters more than it sounds. Every score this session that stalled below 100% stalled on
register allocation — which register a value landed in, which local got spilled, which of two
equivalent orders MSVC picked. **`/Od` has none of that.** Locals live at fixed frame offsets in
declaration order, values are reloaded rather than kept in registers, and the generated code
follows the source statement by statement.

The first game function attempted, `0x00466fe6` at 3,919 bytes, came out **byte-exact on the first
build** with no iteration at all. Nothing in `sidlib` has ever done that at more than a tenth of
the size.

So the cost model is inverted from what the project has been assuming:

| | `sidlib` (`/O2`) | game layer (`/Od`) |
|---|---|---|
| typical first score | 30–60% | 100% so far |
| iterations to bank it | 2–14 builds | 1 |
| binaries credited | Shandalar + 2× Facemaker (1.65 MB) | Shandalar + Magic + Deckdll (3.18 MB) |

Build game translation units with `CFLAGS_OD`, as `window.c` and `src/game/` already are. Getting
this wrong is loud rather than subtle — `window.c` scored near zero at `/O2`.

## The card layer: a statically-initialised dispatch table

Established 2026-07-28, reconstructing `Game_CardScript48b2c1`.

Every 72-byte record in the card table at `0x00594208` carries a `__cdecl int (*)(int, int, int)`
at **+0x34**. It is how the game runs card behaviour, and it is the largest single body of code in
`Shandalar.exe`.

**Read the table, not the code.** Sweeping `.text` for addresses named near that displacement
finds 23 handlers — 23 `call dword ptr [eax + eax*8 + 0x0059423c]` sites and 50 `cmp`s testing a
card's handler against a particular function. That is a rounding error. The table is *statically
initialised in the image* (`.data` file offset `0x192608`), so the pointers can be read directly:

| | |
| --- | --- |
| card records scanned | 1,009 |
| records carrying a script | 722 |
| **distinct scripts** | **608** |
| **total script bytes** | **603,363 — 38.6% of `.text`** |
| median script | 788 bytes |
| call-free scripts | 22, totalling 5,698 bytes |

`ghidra-work/scripts/cardscripts.py` regenerates this and ranks the scripts by cost to take
(call-free first, then descending size); `--csv` writes the full 608 rows.

**The shape they share.** A script reads the slot it was invoked on out of two globals rather
than its arguments — `g_gameSlots[g_game8e1aa8][g_game951bdc]` — so a script that makes no calls
needs no stubs, no struct it does not already have, and no globals beyond those two. The usual
body is: test `kind` against one constant, confirm the globals still point at the slot the
arguments describe, then perform a single mutation. Nine are reconstructed so far and eight are
byte-exact.

`0x00488ca0` is the handler for **88** of the 722 cards and its body is `return 0;`. Two more
18-byte functions elsewhere in `.text` are byte-identical to it and serve one card each, which is
what a per-card source file containing an empty default produces.

**A script's return value carries information.** `0x0043d4e3` answers 1 for `kind == 0x74` and 0
otherwise; the other eight examined return 0 on every path. So the dispatcher reads the result for
at least some kinds, which none of the call sites read so far revealed.

### Two traps this seam sets

- **`test` narrowing decides a field's declared type.** The original tests `slot+0x28` with
  `test byte ptr [mem], 0x20`. Declaring that field `unsigned char` is wrong: MSVC then emits a
  zero-extending load (`xor edx,edx / mov dl,[mem] / test dl,0x20`). Declaring it `int` — which
  MSVC narrows back to a byte `test` — is what matches. That single change was 85.33% → 100%.
  `flags08` at +0x08 confirms it from the other direction: it is written as a dword (`or ..,
  0x2000`) and tested as a byte (`test byte ptr .., 0x10`), and one `int` declaration serves both.
- **Comparison operand order is not reachable.** Whether the global or the parameter lands in the
  register is the compiler's choice, and it is not even consistent between two scripts whose first
  three terms are textually identical. Flipping every mismatching term across three functions
  moved no score by any amount. It is most of the residual in the ones below 100% and it should be
  left there.
