/* sidlib - MicroProse in-house 2D engine, sprite.c: the build arena and .spr loading.
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

#ifndef SIDLIB_SPRITE_H
#define SIDLIB_SPRITE_H

#include "sidlib/sidcore.h"

/* --- sprite.c ----------------------------------------------------------- */

/* The sprite build arena.
 *
 * Sprites are not assembled into individually-malloc'd frames. Instead the
 * builder grabs one fixed 2 MiB block, appends frames to it by bumping a
 * cursor, and at the end shrinks the block down to what was actually used.
 * There is no bounds check anywhere: a build larger than 2 MiB runs off the
 * end of the allocation. See docs/quirks.md Q-010.
 *
 * The shrink uses _expand rather than realloc, and that choice is load-bearing
 * rather than stylistic — see Spr_ArenaEnd. */
#define SPR_ARENA_BYTES 0x200000

/* Base of the arena (0x0073a828). Rewritten by Spr_ArenaEnd to _expand's
 * result. */
extern void *g_sprArenaBase;

/* Append cursor (0x0073a82c). Set to the base by Spr_ArenaBegin and bumped by
 * the frame builder at 0x0057b840. */
extern void *g_sprArenaCur;

/* Starts a sprite build: allocates the arena and resets the cursor. The malloc
 * result is not checked. */
/* FUNCTION: SHANDALAR 0x0057b650 */
void Spr_ArenaBegin(void);

/* Ends a sprite build: writes the 0xFFFFFFFF end-of-file marker at the cursor
 * and shrinks the arena to fit. */
/* FUNCTION: SHANDALAR 0x0057b670 */
void Spr_ArenaEnd(void);

/* free() wrapper. Used binary-wide, not just by sprite code — over thirty call
 * sites across unrelated modules go through this rather than calling free
 * directly. */
/* FUNCTION: SHANDALAR 0x0057b6a0 */
void Sid_Free(void *block);

/* Writes a heap block to `path` and returns its size. The length comes from
 * _msize, so this only works on a pointer that came straight from the CRT
 * allocator. A debug/authoring helper: exactly one call site survives in the
 * shipped binary (0x00446b69). No error checking on the fopen. */
/* FUNCTION: SHANDALAR 0x0057b6c0 */
unsigned int Spr_DumpToFile(void *block, const char *path);

/* A .spr file is a flat sequence of frames terminated by a 0xFFFFFFFF size
 * field. Frames are never copied: the whole file is read into one allocation
 * and the frame table holds pointers into it. Frame layout, as used by the
 * blitter at 0x0057bfa0:
 *
 *   +0  int32   size of this frame, in bytes (0xFFFFFFFF = end of file)
 *   +4  int16   width
 *   +6  int16   height
 *   +8  int16   unknown
 *   +10 int16   unknown
 *   +12 int16   emptyLines - fully transparent rows before the first row data
 *   +14 int16   encodedRows - how many rows of run data follow
 *   +16 ...     row data
 */

/* Row data, established by Spr_GetFrameExtent (0x0057c450), which walks it:
 *
 *   Each of the `encodedRows` rows begins with a start-x byte. A start-x of
 *   0xff means the row is entirely empty and nothing else follows for it.
 *   Otherwise a run-length byte follows; a length of 0xfe is an escape meaning
 *   the real length is in the next byte. That many pixel bytes then follow.
 *
 * One run per row, so a row is a single horizontal span rather than a chain of
 * spans.
 *
 * encodedRows is what BOTH the extent scanner and the renderer walk: Spr_DrawScaled
 * draws while (yAcc >> 16) < emptyLines + encodedRows. Measured over all 924 shipped
 * frames it equals height - emptyLines in 921 of them and is smaller in 3, so a
 * frame's real content height is emptyLines + encodedRows and can be less than
 * height. See docs/formats/spr.md. */
typedef struct SprFrame {
    long size;
    short width;
    short height;
    /* +8 and +10 were "unknown" until the encoder at 0x0057b840 was read. Each
     * is the length of a run of identical pixels along a guard line just
     * outside the captured rectangle: +8 scans right along the row one below
     * the bottom edge, +10 scans down the column one left of the left edge.
     * Both count how many pixels match the first one, stop at the first that
     * differs, and store -1 when the entire guard line matched or when the
     * guard line would fall outside the page.
     *
     * That is what the code does. What the artist was *marking* with those runs
     * is not established, so the names say run length and nothing more. */
    short borderRunX;
    short borderRunY;
    short emptyLines;
    short encodedRows;
} SprFrame;

/* The row buffer the encoder reads one scanline into. 1024, from its frame
 * size: 0x410 of locals is this plus the four dwords below it. A capture wider
 * than this overruns it; nothing checks. */
#define SPR_ROW_MAX 1024

/* Captures a rectangle of a page and appends it to the arena as one frame.
 * Nineteen call sites. This is the encoder that produces the format the rest of
 * this header describes, so it is the authority on it.
 *
 * It also settles what the 0xfe byte in the row data means. The encoder writes
 * it when `memchr(span, 0, len)` finds NO zero — that is, when the span is
 * fully opaque — so it marks a run the blitter can copy without testing each
 * pixel for transparency, rather than being a length escape. The length byte
 * follows it either way. (Both readings fit the byte layout; only this one fits
 * the condition that emits it.)
 *
 * Reads pixels one at a time for the guard scans and a whole row at a time for
 * the body, so `page` must be a real page and not the screen. */
/* FUNCTION: SHANDALAR 0x0057b840 */
void Spr_AppendFrame(int page, int x, int y, int width, int height);

/* What the encoder reaches into lib.c for. Declared here rather than pulling in
 * sidgfx.h: fewer declarations visible to sprite.c means less disturbance to
 * its register allocation. Both must stay character-identical to sidgfx.h. */
unsigned int Sid_GetPixelPage(int page, int x, int y);
void Sid_GetLine(void *dst, int page, int x, int y, unsigned int count);

/* Measures a frame's horizontal extent: the leftmost start-x over all rows and
 * the rightmost end-x. Writes INT_MAX and 0 first, so a NULL frame or one with
 * no encoded rows leaves an inverted, obviously-empty range rather than zeroes
 * that would read as a valid box at the origin. Empty rows (start-x 0xff) do
 * not contribute. */
/* FUNCTION: SHANDALAR 0x0057c450 */
void Spr_GetFrameExtent(const SprFrame *frame, int *minX, int *maxX);

/* Loads every frame in a .spr file. Returns the number of frames stored into
 * `frames`. The caller supplies an array large enough for the file; the
 * original performs no bounds checking whatsoever. */
/* FUNCTION: SHANDALAR 0x0057b710 */
int Spr_LoadFile(void **frames, const char *path);

/* As Spr_LoadFile, but stops after `maxFrames` frames. */
/* FUNCTION: SHANDALAR 0x0057b7a0 */
int Spr_LoadFileMax(void **frames, const char *path, int maxFrames);

/* --- the scaled-sprite blitter's state (0x0057bfa0) ----------------------
 *
 * Spr_DrawScaled scales with two 16.16 fixed-point step values and a pair of
 * memoised 1024-entry lookup tables. The tables are rebuilt only when any of
 * the four cached dimensions changes, which is why they are file-scope rather
 * than locals: consecutive draws of the same sprite at the same size reuse them.
 *
 * The addresses corroborate the sizes exactly. g_sprScaleSrcToDst spans
 * 0x00739828..0x0073a828, ending precisely where g_sprArenaBase begins, and
 * g_sprScaleDstToSrc spans 0x0073a848..0x0073b848, ending precisely at
 * g_sprScaleSrcW. Both are 1024 ints, matching the 0x400 clear loop. */

/* 16.16 source-pixels-per-destination-pixel. */
extern int g_sprScaleXStep;          /* 0x00739818 */
extern int g_sprScaleYStep;          /* 0x0073981c */

/* Destination column range after clipping, in destination pixels. */
extern int g_sprScaleLeft;           /* 0x0073a840 */
extern int g_sprScaleRight;          /* 0x00739820 */

/* source column -> first destination column that samples it, or -1. */
extern int g_sprScaleSrcToDst[1024]; /* 0x00739828 */

/* destination column -> source column. */
extern int g_sprScaleDstToSrc[1024]; /* 0x0073a848 */

/* Fixed-point accumulators, kept in globals rather than locals. */
extern int g_sprScaleXAcc;           /* 0x0073a834 */
extern int g_sprScaleYAcc;           /* 0x0073a838 */

/* The four dimensions the cached tables were built for. */
extern int g_sprScaleSrcW;           /* 0x0073b848 */
extern int g_sprScaleSrcH;           /* 0x0073a830 */
extern int g_sprScaleDstW;           /* 0x0073a844 */
extern int g_sprScaleDstH;           /* 0x0073a83c */

/* Draws a frame scaled into dstW x dstH at (x, y). Page 0 goes through
 * Sid_PutLine or Sid_SetPixel; every other page is written directly. Colour
 * index 0 is transparent. */
/* Live in lib.c. Declared here as single lines rather than by including
 * sidgfx.h, per the one-header-per-module rule. */
void Sid_PutLine(const void *src, int page, int x, int y, unsigned int count);
void Sid_SetPixel(const SidContext *ctx, int x, int y, long color);

/* FUNCTION: SHANDALAR 0x0057bfa0 */
void Spr_DrawScaled(const SidContext *ctx, int x, int y, int dstW, int dstH,
                    const SprFrame *frame);

#endif /* SIDLIB_SPRITE_H */
