/* Declaration surface adopted from the archived attempt (salvage/decomp/
 * include/sidlib/sidcore.h); see gameslot.h provenance note. */

/* sidlib - MicroProse in-house 2D engine, shared core.
 *
 * Original translation units (recovered from compiled-in assert strings):
 *   D:\NewMagic\sources\sidlib\{lib,text,sprite,Fileio,Pcxw}.c
 *
 * Written for Microsoft Visual C++ 4.2, which is C89 only: declarations must
 * precede statements, no // comments in translation units we want to match.
 *
 * ONE HEADER PER MODULE, DELIBERATELY. Every declaration visible to a
 * translation unit perturbs MSVC 4.2's register allocation across that whole
 * unit -- adding six prototypes to the old shared header once took Sid_SetPixel
 * from a 100% match to 31% without touching lib.c at all. Keeping each
 * module's declarations out of its neighbours' sight is what makes per-function
 * scores comparable between builds. Do not merge these back together, and do
 * not include a module header you do not actually need.
 */

#ifndef SIDLIB_CORE_H
#define SIDLIB_CORE_H

#include <stdarg.h>
#include <stdio.h>

/* MSVC 4.2's stdio.h defines _fileno as a macro ((_stream)->_file), but the
 * original binary calls the real imported function, so the original sources
 * had the macro suppressed. Undefining it here reproduces their codegen. */
#undef _fileno
#undef fileno

/* The original source path strings are embedded verbatim so that the string
 * literals the compiler emits match the ones in the binary. Do not replace
 * these with __FILE__ - that would bake in our own build path.
 *
 * The STRING markers belong here rather than at the call sites. reccmp binds a
 * marker to the literal on the following line, and these expand inside
 * multi-line Sid_Assert argument lists where a marker would desynchronise its
 * parser. Annotating the #define covers every use in every module at once:
 * putting the sprite.c one here took Spr_LoadFile from 94.44% to byte-exact. */
// STRING: SHANDALAR 0x005a17d0
#define SIDLIB_SPRITE_C "D:\\NewMagic\\sources\\sidlib\\sprite.c"
// STRING: SHANDALAR 0x005a166c
// STRING: FACEMAKER 0x0040fb68
#define SIDLIB_LIB_C    "D:\\NewMagic\\sources\\sidlib\\lib.c"
// STRING: SHANDALAR 0x005a17a8
#define SIDLIB_TEXT_C   "D:\\NewMagic\\sources\\sidlib\\text.c"
// STRING: SHANDALAR 0x005a181c
// STRING: FACEMAKER 0x004102e4
#define SIDLIB_FILEIO_C "D:\\NewMagic\\sources\\sidlib\\Fileio.c"

/* Shared assert/error reporter (original at 0x0046a520). Every compiled-in
 * assert in the binary calls this with (condition, file, line, printf-format,
 * ...). It returns normally when the condition holds. Not yet decompiled -
 * see decomp/src/stubs.c.
 *
 * `condition` is whatever the caller tested, passed through as an int-sized
 * value rather than normalised to 0/1 - the original pushes a FILE* straight
 * onto the stack, so callers cast instead of writing `!= NULL`. */
void Sid_Assert(int condition, const char *file, int line, const char *fmt, ...);

/* memcpy(dst, src, n) that moves 8 bytes at a time through the x87 stack
 * (FLD/FSTP) - the FPU used as a wide move unit, not for arithmetic.
 * At 0x00566cea, outside sidlib. */
void Sid_FastMemcpy(void *dst, const void *src, unsigned int count);

/* SidPage and the page table live in the core header rather than sidgfx.h
 * because sprite.c renders into pages too. This is the engine's fundamental
 * surface type; keep the rest of the graphics API out of here. */
/* A "graphics page" is an offscreen surface: a DIB section backed by a named
 * file mapping, plus the DC it is selected into. Page 0 is the screen and is
 * handled separately (see Sid_AllocPage). At most 10 pages exist.
 *
 * Layout confirmed two ways: the malloc(0x30) + field stores in Sid_AllocPage
 * (0x00579050), and independently by the accessors, which reach through the
 * page table at +0x04 (hdc), +0x20 (width) and +0x24 (height).
 */
typedef struct SidPage {
    void *hFileMapping;  /* +0x00  CreateFileMappingA, named "rpbips<N>"      */
    void *hdc;           /* +0x04  CreateCompatibleDC                          */
    void *hBitmap;       /* +0x08  CreateDIBSection                            */
    void *hOldBitmap;    /* +0x0c  what SelectObject displaced                 */
    void *pBitmapInfo;   /* +0x10  BITMAPINFO built by Sid_BuildBitmapInfo     */
    void *hPalette;      /* +0x14  the global palette                          */
    void *pBits;         /* +0x18  DIB section bits                            */
    int sizeBytes;       /* +0x1c  total bytes of pBits                        */
    int width;           /* +0x20                                              */
    int height;          /* +0x24                                              */
    int bpp;             /* +0x28                                              */
    int rowPadding;      /* +0x2c  pixels added per row for DWORD alignment    */
} SidPage;

#define SID_MAX_PAGES 10

/* The page table. Page 0 is the screen. Lives in the original's .data
 * at 0x00986950. */
extern SidPage *g_sidPages[SID_MAX_PAGES];

/* The drawing context the primitives take. Its first field is the page index —
 * which is all most of them touch — but Sid_SetClipRect (0x00579ea0) reads and
 * writes a clip rectangle at +0x04..+0x10 of the same pointer, which is how we
 * know the shape. Anything beyond +0x14 is still unknown. */
typedef struct SidContext {
    int page;        /* +0x00  index into g_sidPages */
    int clipLeft;    /* +0x04 */
    int clipTop;     /* +0x08 */
    int clipRight;   /* +0x0c */
    int clipBottom;  /* +0x10 */
    int unknown14;   /* +0x14 */
    int textColor;   /* +0x18  saved/restored around a text draw */
    int unknown1c;   /* +0x1c */
    int font;        /* +0x20  index into the font table */
} SidContext;

#endif /* SIDLIB_CORE_H */
