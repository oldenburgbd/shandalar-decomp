# Quirks Register

Observable oddities of the original game/data, with our keep/fix decision. IDs are stable —
reference them from code comments and commit messages.

## Q-001 — Master.csv rows truncated mid-quote

**Found:** 2026-07-23, while loading the card DB.
**What:** Several rows (e.g. 0432 *Jihad* at line 434, 0542 *Urza's Miter* at line 544) are cut
off at exactly 143 characters inside an open quoted Rule Text field, with no closing quote —
the text ends mid-word ("…opponent controls no c"). Likely a fixed-width export buffer in
MicroProse's pipeline.
**Consequence:** A strict RFC-4180 parser treats the rest of the file as part of that field and
silently drops the following records (902 instead of 1,250 loaded).
**Decision:** *Keep-compatible.* Parse Master.csv line-scoped (one physical line = one record;
an unterminated quote ends its field at end-of-line), as the original engine evidently did —
`Csv.ParseLine`. Note the display text for these cards is genuinely truncated in the original
data; actual rules behavior lives in `Magic.exe`, so gameplay is unaffected. If we ever want
clean display text, restore it from `Rarity.csv`/`Info.csv` or the real card wording as a
deliberate, flagged deviation.

## Q-002 — PIC block length field wraps at 64 KB

**Found:** 2026-07-23, while reversing the `.pic` format (issue #1).
**What:** Each PIC block header stores a `uint16` `length` = bytes following the length field.
For blocks larger than 65,535 bytes (many full-screen images) this field simply **wraps mod
65536**. Example: `Tradscrn.pic` has 65,694 bytes after the field but stores `length` = 158
(65,694 − 65,536).
**Consequence:** Trusting `length` to bound or skip an image block truncates decoding and
corrupts anything that reads subsequent blocks by offset.
**Decision:** *Keep-compatible.* For image (`X0`/`X1`) blocks we ignore `length` entirely: read
the compressed stream to end-of-file and bound the output by `width * height`. Palette (`M0`/`M1`)
blocks are always 770 bytes, well under the wrap point, so their `length` is used normally.
See `docs/formats/pic.md`.

## Q-003 — Cards.dat encodes X (variable) mana as generic 40

**Found:** 2026-07-23, while reversing Cards.dat (issue #4).
**What:** The mana-cost pip array stores the generic-mana count in byte 0. For X spells (Fireball
`XR`, Braingeyser `XUU`), that slot holds the sentinel value **40** instead of a real generic
count — no printed card has 40 generic mana, so it unambiguously means "X".
**Consequence:** Reading the generic slot literally gives a nonsensical converted mana cost (40+).
**Decision:** *Keep-compatible.* `CardsDat` treats generic == 40 as X: it sets
`ManaCost.IsVariable = true` and `Generic = 0`, so converted mana cost counts only the fixed pips.
See `docs/formats/cards-dat.md`.

## Q-004 — Master.csv and Cards.dat names differ (display vs canonical)

**Found:** 2026-07-23, joining Cards.dat to Master.csv (issue #4).
**What:** 172 of 999 shared cards have different names in the two files. `Master.csv` uses short
**display names** for the deck-builder UI ("Blue Circle", "Merfolk", "Sword to Plowshare") and
contains outright typos ("Ressurection", "Hypnotic Spectre"). `Cards.dat` carries the **canonical**
card names ("Circle of Protection: Blue", "Merfolk of the Pearl Trident", "Swords to Plowshares",
"Resurrection").
**Consequence:** You cannot join or match the two databases by name; only the **ID** (0..998) is a
reliable key.
**Decision:** Join on ID. Prefer `Cards.dat` names as canonical; treat `Master.csv` names as the
original UI display strings. Tests compare stats by ID, never by name equality.

## Q-005 — Legacy.csv's final record is malformed (binary garbage)

**Found:** 2026-07-23, loading the secondary CSVs (issue #7).
**What:** The last record of `Legacy.csv` (ID 0999, name "None") ends with an **unterminated
quoted field followed by binary garbage bytes** on a continuation line — the Q-001 failure mode
plus junk that was evidently in the write buffer when the file was produced.
**Consequence:** Strict CSV parsing corrupts or rejects the file; naive line parsing yields a
bogus trailing record.
**Decision:** *Keep-compatible.* Line-scoped parsing plus requiring a numeric leading ID drops the
garbage continuation cleanly; record 0999 itself loads with what it has. See
`docs/formats/csv-files.md`.

## Q-006 — Blitter branches on srcY but both arms are identical

**Found:** 2026-07-27, decompiling `Sid_BlitPage` (`0x00579890`) and
`Sid_BlitPageRestore` (`0x00579bf0`).
**What:** Both blitters set up their scratch `BITMAPINFO` with

```c
bmi->bmiHeader.biWidth = src->width;
if (srcY == 0) bmi->bmiHeader.biHeight = src->height;
else           bmi->bmiHeader.biHeight = src->height;
```

The test on `srcY` has no effect — both arms assign the same value. It is present in the
compiled code of both functions, so it is in the original source, not a decompiler artifact.
Most likely the `else` arm was meant to account for the sub-rectangle (`src->height - srcY`,
or a negated height for top-down rows) and was either never finished or later "fixed" by making
both sides the same.
**Consequence:** None observable. `biHeight` is positive in both cases, which is why the
blitters have to manually flip the rows before calling `SetDIBitsToDevice` — that flip is doing
the work this branch looks like it was meant to do.
**Decision:** *Keep.* Reproduced verbatim in `decomp/src/sidlib/lib.c`; removing it changes the
generated code and breaks the match. Flagged here so nobody "cleans it up" later.

## Q-007 — rowPadding is used as pixels in one place and bytes in another

**Found:** 2026-07-27, decompiling `lib.c`.
**What:** `SidPage.rowPadding` (`+0x2c`) is computed in `Sid_AllocPage` as a count of **pixels**
and folded into the width before the depth conversion:
`sizeBytes = ((rowPadding + width) * bpp * height) / 8`. `Sid_GetPixel` indexes the same way,
`pBits[(rowPadding + width) * y + x]`. But the blitters treat it as a count of **bytes**:
`stride = rowPadding + (width * bpp) / 8`.
**Consequence:** The two expressions agree only when `bpp == 8`, which is the only depth the
shipping game ever uses, so the inconsistency never bites. Any non-8bpp page would allocate and
address itself inconsistently.
**Decision:** *Keep.* Each function is reproduced as written. Do not "unify" the stride helper
across the module — that would silently change behaviour at other depths and break the match.
`SID_PIXEL_AT` and `SID_ROW_BYTES` in `sidlib.h` deliberately encode the two different formulas.

## Q-008 — Sid_CreateScreenPage leaks a BITMAPINFO at startup

**Found:** 2026-07-27, decompiling `Sid_CreateScreenPage` (`0x00578e80`).
**What:** The screen page's `pBitmapInfo` is built twice in succession:

```c
p->pBitmapInfo = Sid_BuildBitmapInfo(width, height, bpp);   /* discarded */
p->pBitmapInfo = Sid_BuildBitmapInfo(width, height, 8);     /* kept */
```

The first allocation is overwritten without being freed. It is 44 bytes at anything other than
8bpp, or 1,068 bytes when the display is already 8bpp.
**Consequence:** A one-off leak of at most ~1 KB per process. `Sid_CreateScreenPage` is
idempotent — it returns early if the screen page already exists — so it cannot repeat.
**Decision:** *Keep.* Reproduced verbatim; the second call is what determines behaviour and the
leak is unobservable. Flagged so it is not "fixed" into a mismatch.

## Q-009 — The palette clear loop terminates by running into the next global

**Found:** 2026-07-27, decompiling `Sid_CreateScreenPage` (`0x00578e80`).
**What:** Clearing the 256-entry RGB table is written as a pointer walk whose stop condition is
the *address of the page table*, not a count:

```c
entry = g_sidPaletteRgb;          /* 0x00986550 */
do { ...; entry++; } while (entry < (SidRgb *)g_sidPages);   /* 0x00986950 */
```

`g_sidPaletteRgb` is 256 x 4 = 1,024 bytes at `0x00986550`, which ends exactly at `0x00986950`
where `g_sidPages` begins. The loop is correct only because the linker happened to place the two
adjacently.
**Consequence:** None in the original. **In our build it is a hazard**: our globals are not
adjacent, so if this code were executed outside the hook-and-replace harness the loop would run
until it happened to pass our `g_sidPages` address, corrupting memory.
**Decision:** *Keep, but do not execute standalone.* Reproduced as written because rewriting it
as `entry < &g_sidPaletteRgb[256]` changes the generated code. Under the Phase-2 harness the
globals sit at their original addresses and the loop is correct again. Until then,
`Sid_CreateScreenPage` must not be called from our DLL.

## Q-010 — The sprite build arena has no bounds check at all

**Found:** 2026-07-27, decompiling `Spr_ArenaBegin` (`0x0057b650`) and its callers.
**What:** A sprite build allocates one fixed 2 MiB block and appends frames by bumping a cursor
(`g_sprArenaCur`, `0x0073a82c`). Nothing anywhere compares the cursor against the end of the
allocation — not the builder at `0x0057b840`, not `Spr_ArenaEnd`, not any of the nineteen
call sites.

```c
g_sprArenaBase = malloc(0x200000);   /* 2 MiB, result not checked either */
g_sprArenaCur  = g_sprArenaBase;
```

**Consequence:** A build totalling more than 2 MiB writes past the end of the heap block. The
shipped content evidently stays under the limit, so it never fires in practice — the size was
presumably chosen as "comfortably more than any sprite set". The `malloc` return is not checked
either, so on an allocation failure the first frame write dereferences NULL.
**Decision:** *Keep.* Reproduced as written. This is a data-dependent constraint on any content
we might later generate, not a bug to fix: adding a check would change the generated code and
break the match, and the limit is real regardless of whether it is enforced.

## Q-011 — A failed arena shrink silently nulls the base pointer

**Found:** 2026-07-27, decompiling `Spr_ArenaEnd` (`0x0057b670`).
**What:** The shrink-to-fit stores `_expand`'s result back unconditionally:

```c
g_sprArenaBase = _expand(g_sprArenaBase,
                         (char *)g_sprArenaCur - (char *)g_sprArenaBase + 16);
```

`_expand` returns NULL when it cannot resize the block in place.
**Consequence:** On failure `g_sprArenaBase` becomes NULL and the 2 MiB block leaks — the only
pointer to it has just been overwritten. Callers do not use the return value, so nothing
notices. In practice `_expand` shrinking a block always succeeds, which is why this never
fires.
**Note:** `_expand` rather than `realloc` is deliberate and correct — see
`docs/decompilation.md`. The builder has already handed interior pointers into this block to its
callers, so it must not move. The quirk is the unchecked store, not the choice of allocator.
**Decision:** *Keep.* Reproduced as written.

## Q-012 — The scale table's gap-fill reads one entry before the array

**Found:** 2026-07-28, decompiling `Spr_DrawScaled` (`0x0057bfa0`).
**What:** When a sprite is scaled *down*, some source columns are sampled by no destination
column and are left as -1 in `g_sprScaleSrcToDst`. The fill pass gives each such entry its left
neighbour's value — starting at index 0:

```c
e = g_sprScaleSrcToDst;          /* 0x00739828 */
i = srcW + 1;
do {
    if (*e == -1) {
        *e = e[-1];              /* at i == 0 this reads 0x00739824 */
    }
    e++;
    i--;
} while (i != 0);
```

**Consequence:** On the first iteration `e[-1]` is the dword immediately below the table, which
is not part of it. It only matters if entry 0 is ever -1, which requires the first source column
to be sampled by no destination column — impossible while the accumulator starts at 0, since
destination column 0 always maps to source column 0. So the read happens but its value is never
used. **In our build it is equally harmless but reads a different neighbour**, since our globals
are not laid out at the original's addresses.
**Decision:** *Keep.* Reproduced as written; rewriting the loop to start at index 1 would change
the generated code for no behavioural gain.

## Q-013 — The "use a block of mine" palette flag is a pointer value of 1

**Found:** 2026-07-28, decompiling `Sid_OpenFileCore` (`0x0057c580` / Facemaker `0x0040aec0`).
**What:** The last argument is a `SidPaletteBlock *`, and the literal value `1` is used as a
sentinel meaning "decode the palette into a block of your own":

```c
if (palette == (SidPaletteBlock *)1) {
    palette = (SidPaletteBlock *)local;   /* 1 KB of stack frame */
}
if (palette != NULL) { ... }
```

Two of the four wrappers pass `1`, one passes `0`, and one passes the caller's own pointer
through — so all three cases are live.
**Consequence:** The sentinel is only tested on the PCX path. The other path passes the argument
straight to the MPS_CODE assembly routine without checking it, so calling
`Sid_OpenFileInGroup` or `Sid_OpenFileByName` on a file that is not named `*.pcx` hands the
decompressor the address `0x00000001`. Whether MPS_CODE dereferences it is not established.
**Decision:** *Keep.* Reproduced as written. Recorded because it is the kind of asymmetry that
looks like a transcription error in the reconstruction and is not.

## Q-014 — Two assert sites pass the wrong thing as the format string

**Found:** 2026-07-28, decompiling `Sid_OpenFileCore`.
**What:** `Sid_Assert` is variadic — `(condition, file, line, format, ...)` — and the two asserts
on the raw-descriptor path stop one argument short of a format string:

```c
Sid_Assert(0, SIDLIB_FILEIO_C, 136, name);   /* Fileio.c:136, _open failed  */
Sid_Assert(0, SIDLIB_FILEIO_C, 168, NULL);   /* Fileio.c:168, _close failed */
```

The first passes the file *name* where the format belongs, so the failure message is the name
interpreted as a printf format. The second passes a null pointer.
**Consequence:** A file name containing a `%` prints garbage or faults; the `_close` failure
path calls `vfprintf` with a null format. Both are unreachable in normal operation — they only
fire when the assert has already failed — so neither has ever been observed.
**Decision:** *Keep.* Reproduced exactly, including the argument counts, which are visible in the
original as `add esp, 0x10` against the five-argument sites' `add esp, 0x14`. The PCX path's
assert on the same page is correctly formed, so this is per-site sloppiness rather than a
misunderstanding of the function.

## Q-015 — The line counter reads one byte below its buffer for an empty string

**Found:** 2026-07-28, decompiling `Sid_DrawTextCore` (`0x0057b230` / Facemaker `0x00409660`).
**What:** Counting lines walks the formatted buffer and then checks whether the last character
was already a newline, so that a string not ending in one still counts as a line:

```c
p = buf;
if (*p != '\0') {
    do { if (*p == '\n') lines++; p++; } while (*p != '\0');
}
if (p[-1] != '\n') { lines++; }    /* p is still buf when the string is empty */
```

The guard skips the walk for an empty string but not the trailing check, so `p` never advanced
and `p[-1]` reads the byte below a 1,024-byte stack buffer.
**Consequence:** A one-byte read of live stack — another local of the same frame, so never
unmapped and never a fault. Whatever it happens to be decides whether an empty string counts as
one line or zero, which changes nothing downstream because a zero-length line draws nothing.
**Decision:** *Keep.* Reproduced as written. Hoisting the check inside the guard would change the
generated code for no behavioural gain.

## Q-016 — Shadowed text leaves the context's colour changed on the way out

**Found:** 2026-07-28, decompiling `Sid_DrawTextCore`.
**What:** The function saves `ctx->textColor` before overriding it, and the per-line shadow pass
saves it again — into the *same* variable:

```c
if (color >= 0) { saved = ctx->textColor; ctx->textColor = color; }   /* the caller's colour */
...
    if (shadow) {
        saved = ctx->textColor;      /* now `color`, and the caller's is gone */
        ctx->textColor = 0;
        Sid_DrawString(ctx, lineX + 1, y + 1, line);
        ctx->textColor = saved;
    }
...
if (color >= 0) { ctx->textColor = saved; }                          /* restores `color`  */
```

That it is one variable and not two is settled by the frame, not by inference: both stores go to
the same slot at frame+0x18, their live ranges overlap, and the reconstruction reproduces the
original's `sub esp, 0x410` exactly — a second variable would have made the frame four bytes
larger.
**Consequence:** Any call with both `color >= 0` and `shadow` — which is most of the `...S...`
entry points — leaves `ctx->textColor` set to that call's colour instead of restoring the
caller's. It is invisible until something later draws with `color < 0`, meaning "use the
context's colour", and gets the last shadowed draw's colour instead.
**Decision:** *Keep.* Reproduced exactly. Splitting it into two variables would change the frame
size and every stack offset in the function.

## Q-017 — A zero-length PCX run still writes a pixel

**Found:** 2026-07-28, decompiling `Pcx_DecodeRow` (`0x0057da00`).
**What:** The RLE decoder reads a marker byte, takes the low six bits as a count, reads the value
byte — and then only takes the run path when the count exceeds 1:

```c
if ((b & 0xc0) == 0xc0) {
    count = b & 0x3f;
    b = fgetc(g_sidPcxFile);
    if (count > 1) { memset(dst, b, count); dst += count; remaining -= count; continue; }
}
*dst++ = (unsigned char)b;   /* count 0 and count 1 both land here */
remaining--;
```

A marker of exactly `0xC0` encodes a run of zero. The original stores the value byte once anyway
and consumes one of the row's remaining bytes.
**Consequence:** A row containing `0xC0` decodes one pixel wider than the encoder intended, and
every pixel after it on that row shifts left by one. No PCX encoder emits a zero run, so the
shipped art never triggers it.
**Decision:** *Keep.* Reproduced as written. The `count > 1` rather than `count > 0` is deliberate
enough in the original — it is what makes a run of 1 take the cheaper path — that treating the
zero case as a bug to fix would be second-guessing the author.

## Q-018 — The EGA palette path seeks past the end of the file

**Found:** 2026-07-28, decompiling `Pcx_ReadHeader` (`0x0057d850`).
**What:** The reader handles two PCX variants. The 8-bit path finds its 768-byte VGA palette in
the last 768 bytes of the file and seeks there correctly. The 4-plane 1-bit path wants the
16-entry EGA palette that lives *inside the header* at offset 16 — and seeks to offset 16 from
the **end**:

```c
fseek(g_sidPcxFile, -768L, SEEK_END);   /* 8-bit:   correct              */
...
fseek(g_sidPcxFile, 16L, SEEK_END);     /* 4-plane: should be SEEK_SET   */
i = 16;
do { fread(paletteOut, 1, 3, g_sidPcxFile); paletteOut += 4; } while (--i != 0);
```

The origin is `2` (`SEEK_END`) in the instruction stream, not `0`, and the 8-bit path two blocks
above uses the same encoding for its own genuine `SEEK_END`. So this is a wrong constant, not a
misreading.
**Consequence:** The stream is positioned 16 bytes past EOF, so all sixteen `fread`s read nothing
and return 0. `paletteOut` is left holding whatever it held before — for the caller that passes
`1`, that is 1 KB of uninitialised stack. Every EGA-format PCX loads with a garbage palette.
**Decision:** *Keep.* Reproduced exactly, and the function is byte-exact with it. The shipped art
is evidently all 8-bit, or this would have been noticed in 1998 — which is also the reason it
survived. Anything we later convert must not rely on this path working.

## Q-019 — The character-width table is indexed with a signed char

**Found:** 2026-07-28, decompiling `Sid_MeasureText` (`0x0057aae0`).
**What:** The bitmap-font path looks each character's advance up in the font record's own
128-entry table, and sign-extends the character to do it:

```asm
movsx eax, bl                              ; bl is the character
mov   cl, byte ptr [eax + edi + 0x983760]  ; edi = font * 684
```

`movsx`, not `movzx`. In C that is `charWidth[*p]` with `p` a plain `char *`, which MSVC treats
as signed.
**Consequence:** Any byte from 0x80 to 0xFF indexes *backwards* out of this font's table and into
the 132 bytes ending the previous slot's record — for font 1, into slot 0, which
`Sid_LoadFontFile` never fills because its slots are 1-based. So high-bit characters measure as
whatever happens to be there. The table is only 128 entries, so there was never room for them;
this is a missing feature reached through undefined behaviour rather than a typo.
**Decision:** *Keep.* Reproduced as written. Text drawn through the bitmap path is evidently all
7-bit, or the widths would visibly disagree with the glyphs.

## Q-020 — Pure white is nudged out of the palette, except at index 255

**Found:** 2026-07-28, decompiling `Sid_SetPalette` (`0x0057a060`).
**What:** After unpacking each entry, both palette paths check whether it came out pure white and,
if so, darken it by one in every component:

```c
if (*(int *)&g_sidDibPalette[i * 4] == 0xffffff && i != 255) {
    white = (mask << 16) | (mask << 8) | mask;
    *(int *)&g_sidPaletteRgb[i]     = white & 0x1fefefe;
    *(int *)&g_sidDibPalette[i * 4] = white & 0xfefefe;
}
```

Index 255 is exempt, so exactly one entry in the palette is allowed to be 0xFFFFFF.
**Consequence:** No colour except index 255 can be pure white; anything that would be becomes
0xFEFEFE, a difference no one can see. Reserving one value that is guaranteed unique is what you
do for a colour key, and index 255 being the reserved one lines up with `Sid_DrawString` clamping
its text colour to 0xFE — but nothing here proves that is the reason.
**Decision:** *Keep.* Reproduced in both paths. Note the two masks differ by a bit:
`0x01fefefe` for `g_sidPaletteRgb`, `0x00fefefe` for `g_sidDibPalette`. The extra bit lands in
`SidRgb.pad`, which is the flags byte the same function sets to 1 elsewhere, so the wider mask is
setting that flag rather than touching a colour.

## Q-021 — Under /Od, a local's stack slot follows its *name*

**Found:** 2026-07-28, reconstructing `Game_ResetState` (`0x00409680`).
**What:** MSVC 4.2 at `/Od` assigns each local an `[ebp-N]` slot in an order that depends on the
identifier, and on neither the declaration order nor the order of first use. Probed with three
one-file functions:

| declared | used first | result |
| --- | --- | --- |
| `a`, `b` | `a` | `a=-4`, `b=-8` |
| `b`, `a` | `a` | `a=-4`, `b=-8` |
| `a`, `b` | `b` | `a=-4`, `b=-8` |

All three agree, so both of the obvious levers are ruled out. `Game_ResetState` needs its
loop-A counter at `[ebp-8]` and its loop-B counter at `[ebp-4]`; naming them the natural way
round (`i` outer-most in the text, `j` inner) measures **74.43%**, and swapping only the two
names measures **95.15%** with no other change.

**Consequence:** When a `/Od` diff is nothing but locals in swapped slots, renaming them is a
one-build fix worth real points. This does not generalise to `/O2` register allocation, where
declaration order *is* the lever (see `Spr_GetFrameExtent`, 46.67% → 68.89%).

**Established 2026-07-29 - the mechanism.** MSVC 4.2 keeps locals in a **16-bucket hash table
keyed on the identifier** and lays the frame out by walking buckets 0..15, so bucket order *is*
slot order. Within one bucket the order is the **reverse of declaration order**. For a single
character the bucket is `ord(c) % 16`; position weights are additive mod 16, and the
second-to-last character contributes `(4c + (c >> 4)) % 16`.

That explains the earlier observations rather than contradicting them. `(zz, aa)` is not sorting
backwards - the two are one bucket apart. The alphabet does not pair up 17 apart, it pairs 16
apart; the pairs only look like collisions until you notice declaration order is reversed inside
a bucket, which is what made `(a, b)` and `(b, a)` both give `a=-4` while `(p, q, a)` and
`(a, q, p)` differ.

The compiler will say directly: `/FAsc` opens its listing with `_name$ = -N` for every local.
`ghidra-work/scripts/slotprobe.py` asks it, `slotname.py` keeps a measured table of names
covering all sixteen buckets, and `slotfix.py` renames the locals of bodies already in the tree.
The closed form for long identifiers is **still not settled and does not need to be** - nothing
has to predict an arbitrary name, only to find names landing in given buckets, and a measured
table is exact where a guessed formula would not be.

`cardgen` now names each local from the bucket matching the slot the original used. `0x00481688`
- 4,944 bytes, eleven locals - went **15.35% to 91.09%** on nothing but its local names.

**Decision:** *Keep, and exploit.* Nothing about this is a deviation from the original — it is a
property of the compiler both sides use. Where a reconstruction names locals to match slots
rather than to read well, say so at the declaration, as `Game_ResetState` does.

## Q-022 — Sid_CreateScreenPage's palette loop terminates by running into the page table

**Found:** 2026-07-29, the first time `Sid_CreateScreenPage` (`0x00578e80`) was ever *executed*,
by the new host EXE (`decomp/src/host/`). It crashed instantly.

**What:** The loop that clears the 256-entry RGB table has no counter. It walks a pointer and
stops when the pointer reaches an unrelated object:

```c
entry = g_sidPaletteRgb;
do {
    entry->r = 0; entry->g = 0; entry->b = 0; entry->pad = 1;
    entry++;
} while (entry < (SidRgb *)g_sidPages);
```

That runs exactly 256 times only because the original's linker placed the page table 1,024 bytes
after the palette table — `0x00986550` → `0x00986950`. The bound is a property of the original's
**data layout**, not of the code. Any other layout gives a different iteration count.

**Consequence:** Ours did not reproduce it for free. As plain uninitialised globals both landed in
COMMON, where the linker orders them as it pleases; it put them **812,304 bytes apart**, and the
loop wrote zeroes over all of it before dying with an access violation. Nothing had caught this
because nothing had ever run the function: the scored DLL is never executed, and the differential
test driver is required to keep `Sid_CreateScreenPage` unreachable for an unrelated reason
(Q-009).

**Decision:** *Keep the code, pin the layout.* `lib.c` reproduces the original faithfully and is
scored, so it is not touched. `src/stubs.c` instead defines the two objects together inside one
`#pragma data_seg("SIDPAL")`, with initialisers so they are emitted to that segment in definition
order rather than being collected into COMMON. That restores the 1,024-byte spacing the loop
depends on. Measured with `score.ps1 -Against`: **no function moved**.

Verify after touching either object — the linker map must show them exactly `0x400` apart:

```powershell
Select-String -Path build\host.map -Pattern "g_sidPaletteRgb|g_sidPages"
```

**The general lesson is worth more than the fix.** A reconstruction can be byte-exact and still
not *run*, because matching the instructions does not match the address space they assumed. Every
loop bounded by comparing a pointer against another object is a latent instance of this, and the
only thing that finds them is executing the code.
