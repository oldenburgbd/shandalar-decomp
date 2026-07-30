/* Declaration surface adopted from the archived attempt (salvage/decomp/
 * include/sidlib/sidtext.h); see gameslot.h provenance note. */

/* sidlib - MicroProse in-house 2D engine, text.c: fonts and formatted text.
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

#ifndef SIDLIB_TEXT_H
#define SIDLIB_TEXT_H

#include "sidlib/sidcore.h"

/* --- text.c: formatted text ---------------------------------------------- */

/* The scratch buffer Sid_DrawTextCore formats into, and the limit it passes to
 * _vsnprintf. 1024, from the frame size: 0x410 of locals is this plus the four
 * dwords below it. */
#define SID_TEXT_BUFFER 1024

/* The formatting core all the text entry points funnel into. Formats through
 * _vsnprintf into a 1 KB buffer, splits on '\n', and draws each line.
 *
 *   shadow    also draw the string at (x+1, y+1) in colour 0 underneath
 *   resScaled treat x/y as 640x480 coordinates and scale to the real screen
 *   hCenter   subtract half the measured line width from x
 *   vCenter   subtract half (font height * line count) from y
 *
 * `color` is applied by swapping ctx->textColor for the duration when >= 0.
 * Returns whatever _vsnprintf returned.
 *
 * `args` points AT the format string, not past it: the entry points hand over
 * the address of their own last named parameter, so args[0] is the format and
 * args + 4 is what _vsnprintf needs. */
/* FUNCTION: SHANDALAR 0x0057b230 */
int Sid_DrawTextCore(SidContext *ctx, int color, int shadow, int resScaled,
                     int hCenter, int vCenter, int x, int y, va_list args);

/* Width of a string in the context's font (0x0057aae0), and the drawer that
 * puts one at a position (0x0057ae50). Neither is reconstructed; both are
 * called only from Sid_DrawTextCore, which is what pins their argument lists.
 * The drawer returns a value that Sid_DrawTextCore discards. */
/* FUNCTION: SHANDALAR 0x0057aae0 */
int Sid_MeasureText(SidContext *ctx, char *s);
/* FUNCTION: SHANDALAR 0x0057ae50 */
int Sid_DrawString(SidContext *ctx, int x, int y, const char *s);

/* The eleven entry points, one per flag combination the game actually uses.
 * Suffixes: S = shadow, R = resolution-scaled, H = horizontally centred,
 * V = vertically centred. All take (ctx, color, x, y, format, ...). */
int Sid_DrawText(SidContext *ctx, int color, int x, int y, ...);
int Sid_DrawTextV(SidContext *ctx, int color, int x, int y, ...);
int Sid_DrawTextHV(SidContext *ctx, int color, int x, int y, ...);
int Sid_DrawTextS(SidContext *ctx, int color, int x, int y, ...);
int Sid_DrawTextSH(SidContext *ctx, int color, int x, int y, ...);
int Sid_DrawTextSV(SidContext *ctx, int color, int x, int y, ...);
int Sid_DrawTextSHV(SidContext *ctx, int color, int x, int y, ...);
int Sid_DrawTextRHV(SidContext *ctx, int color, int x, int y, ...);
int Sid_DrawTextSRH(SidContext *ctx, int color, int x, int y, ...);
int Sid_DrawTextSRV(SidContext *ctx, int color, int x, int y, ...);
int Sid_DrawTextSRHV(SidContext *ctx, int color, int x, int y, ...);

/* --- text.c: the font table ---------------------------------------------- */

/* Font records live in one fixed array at 0x009837e4 with a 684-byte stride,
 * recovered from the index arithmetic in the accessors (index*19, <<2, then *9).
 * Only the fields below are established; everything else is padding we have not
 * accounted for yet, so do not reorder or rename these to something tidier. */
/* The record starts 132 bytes EARLIER than this file used to say. Sid_MeasureText
 * indexes a per-character width table at 0x00983760 with the same 684-byte
 * stride, and reads two more byte fields at 0x009837e3 and 0x009837e5 — one
 * below and one above what was recorded as offset 0. So the array base is
 * 0x00983760, not 0x009837e4, and `height` sits at +0x084.
 *
 * The old layout's trailing `unknown228[0x84]` was exactly 132 bytes of
 * padding-to-stride. It is the same 132 bytes, now at the front where they
 * belong and mostly identified. That the two independently reach 0x2ac is the
 * check that this is right. */
typedef struct SidFont {
    unsigned char charWidth[128];    /* +0x000  advance per character          */
    unsigned char firstChar;         /* +0x080  character at cell 0            */
    unsigned char lastChar;          /* +0x081  character at the last cell     */
    unsigned char cellBytes;         /* +0x082  cell pitch; x8 gives pixels    */
    unsigned char fixedWidth;        /* +0x083  non-zero overrides charWidth   */
    unsigned char height;            /* +0x084  base height, read as a byte    */
    unsigned char spacing;           /* +0x085  added to every character       */
    unsigned char extra;             /* +0x086  added to height when !scaled   */
    unsigned char oddPad;            /* +0x087  1 when the row pitch is odd    */
    /* The glyph strip, as two 1-bit bitmaps in one memory DC. Sid_DrawString
     * blits the mask with SRCAND and then the glyphs with SRCPAINT, which is
     * how it gets transparent text out of a monochrome source. */
    void *hMaskBitmap;               /* +0x088                                 */
    void *hGlyphBitmap;              /* +0x08c                                 */
    void *hMemDC;                    /* +0x090                                 */
    /* The 1-bit glyph strip in memory, malloc'd by Sid_LoadFontRecord and kept
     * only so the two bitmaps can be rebuilt. CreateBitmap copies, so nothing
     * reads it after the record is loaded. */
    void *bits;                      /* +0x094                                 */
    void *scaled;                    /* +0x098  non-NULL selects scaledExtra,
                                                 and marks the slot as loaded  */
    void *hFont;                     /* +0x09c  DeleteObject'd by Sid_FreeFont */
    char fontPath[0x208];            /* +0x0a0  handed to RemoveFontResourceA.
                                                 Its real length is NOT known;
                                                 this simply fills to +0x2a8   */
    int scaledExtra;                 /* +0x2a8  added to height when scaled    */
} SidFont;                           /* 0x2ac = 684 bytes, exactly the stride  */

/* Established by Sid_LoadFontFile, which asserts `count < 16` and passes 16 to
 * its own "Can not load more than %d fonts" message. This was a placeholder
 * ("large enough to link") until that assert was read; it is now the original's
 * own bound. Slots are 1-based, so entry 0 is never used. */
#define SID_MAX_FONTS 16

extern SidFont g_sidFonts[SID_MAX_FONTS];

/* Font count from the loaded font file. Defined in stubs.c. */
extern int g_sidFontCount;

/* The fopen-failure message Sid_LoadFontFile passes to Sid_Assert. A variable
 * in the original, not a literal at the call site. Defined in stubs.c. */
extern char *g_sidMsgFileOpen;

/* Reads one font record into slot `font` from wherever the stream sits. Lives
 * at 0x0057a5f0, not yet reconstructed — see stubs.c. */
/* FUNCTION: SHANDALAR 0x0057a5f0 */
int Sid_LoadFontRecord(int font, FILE *fp);

/* Loads a font file and returns how many fonts it declared. */
/* FUNCTION: SHANDALAR 0x0057a4e0 */
int Sid_LoadFontFile(const char *path);

/* Base height of a font. */
/* FUNCTION: SHANDALAR 0x0057ae30 */
int Sid_GetFontHeight(int font);

/* Base height plus whichever extra the `scaled` pointer selects. */
/* FUNCTION: SHANDALAR 0x0057adf0 */
int Sid_GetFontLineSpacing(int font);

/* Releases a loaded font slot: destroys the GDI font and un-registers the file
 * with the system. A slot whose `scaled` pointer is NULL was never loaded and
 * is skipped. Returns RemoveFontResourceA's result, or 0 for an empty slot. */
/* FUNCTION: SHANDALAR 0x0057a9f0 */
int Sid_FreeFont(int font);

#endif /* SIDLIB_TEXT_H */
