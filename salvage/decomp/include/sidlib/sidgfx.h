/* sidlib - MicroProse in-house 2D engine, lib.c: graphics pages, primitives, blitters.
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

#ifndef SIDLIB_GFX_H
#define SIDLIB_GFX_H

#include "sidlib/sidcore.h"

/* --- lib.c: small helpers at 0x00578c20 ---------------------------------- */

/* Pending event flags (0x00986da0), read-and-cleared by Sid_TakeEventFlags. */
extern int g_sidEventFlags;

/* Loads a font file (text.c 0x0057a4e0). Not reconstructed — see stubs.c. */
/* Lives in text.c (0x0057a4e0), declared here rather than by including
 * sidtext.h: Sid_OpenArchive is the only thing in this module that calls it,
 * and one line of explicit coupling is cheaper than exposing every font
 * declaration to lib.c. Keep cross-module declarations to single lines. */
int Sid_LoadFontFile(const char *path);

/* Purpose unknown; both return -1 and have one caller each. */
int Sid_Unknown578c20(void);
int Sid_Unknown578c30(void);

/* ShowCursor(TRUE) / ShowCursor(FALSE), returning the display counter. */
int Sid_ShowCursor(void);
int Sid_HideCursor(void);

/* Returns the pending event flags and clears them. */
int Sid_TakeEventFlags(void);

/* Empty. */
void Sid_Noop(void);

/* Opens a data archive plus an optional font file; only the font file is acted
 * on. Sid_CloseArchive discards the result and does nothing else. */
int Sid_OpenArchive(const char *dataFile, const char *fontFile);
int Sid_CloseArchive(int handle);

/* --- lib.c: graphics pages ---------------------------------------------- */


/* The realized palette every page is selected into. */
extern void *g_sidPalette;

/* The RGB values behind each palette index, 4 bytes per entry, at 0x00986550.
 * The fourth byte is never read. */
typedef struct SidRgb {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char pad;
} SidRgb;

extern SidRgb g_sidPaletteRgb[256];

/* Screen colour depth (0x00986980) and the screen DC (0x007483f8). At 16bpp
 * the low three bits of each channel are dropped, so palette lookups have to
 * compare masked values. */
extern int g_sidScreenBpp;
extern void *g_sidScreenDC;

/* Colour arguments to the drawing calls are overloaded:
 *   >= 0   a palette index          -> PALETTEINDEX
 *   < 0    an RGB value, negated    -> PALETTERGB
 * with 0xff and 0xff000001 both meaning plain white. */
#define SID_WHITE 0xffffffL

/* Pixel address within an offscreen page, as Sid_GetPixel computes it. */
#define SID_PIXEL_AT(p, x, y) \
    ((unsigned char *)(p)->pBits + ((p)->rowPadding + (p)->width) * (y) + (x))

/* Row stride in bytes, as the blitters compute it. Note this is NOT the same
 * expression as SID_PIXEL_AT: that one adds rowPadding to a pixel count, this
 * one adds it to a byte count. The two agree only at 8bpp, which is the only
 * depth the game ships. The inconsistency is the original's — see
 * docs/decompilation.md. */
#define SID_ROW_BYTES(p)  ((p)->rowPadding + ((p)->width * (p)->bpp) / 8)

/* The RGBQUAD palette handed to SetDIBitsToDevice when the screen is not
 * 8bpp (0x00986990). Distinct from g_sidPaletteRgb. */
extern unsigned char g_sidDibPalette[256 * 4];

/* Gates the fast SetDIBitsToDevice path (0x005a1608). */
extern int g_sidDibBlitEnabled;

/* Scratch row buffers used to swap rows during the vertical flip. The original
 * keeps three separate ones (0x005a1614, 0x005a1618, 0x005a161c). */
extern void *g_sidRowTemp1;
extern void *g_sidRowTemp2;
extern void *g_sidRowTemp3;

/* Lazily built 8bpp BITMAPINFO used by each blitter, with its init flag. */
extern void *g_blitBmi;
extern int g_blitBmiReady;
extern void *g_blitBmi2;
extern int g_blitBmi2Ready;

/* Builds the BITMAPINFO for a page. Top-down (negative biHeight); for 8bpp it
 * appends a 256-entry DIB_PAL_COLORS index table (0..255). */
void *Sid_BuildBitmapInfo(int width, int height, int bpp);

/* Releases a BITMAPINFO. Always returns 1. */
int Sid_FreeBitmapInfo(void *bmi);

/* Creates page 0, the screen. Idempotent — returns the existing page if already
 * built. `bpp` of -1 means "ask the display". Also builds and realizes the
 * global 256-entry palette. */
SidPage *Sid_CreateScreenPage(int width, int height, int bpp);

/* Screen dimensions, kept alongside the page table (0x00986978/0x0098697c). */
extern int g_sidScreenHeight;
extern int g_sidScreenWidth;

/* Set once page 0 exists (0x00739814). */
extern int g_sidScreenPageReady;

/* The LOGPALETTE handed to CreatePalette (0x00986d90), and a second copy of the
 * palette handle (0x00748414). */
extern void *g_sidLogPalette;
extern void *g_sidPaletteHandle;

/* Cleared by Sid_CreateScreenPage (0x005a160c); purpose not yet established. */
extern int g_sidUnknown160c;

/* Set when the DIB colour table needs re-copying before the next line blit
 * (0x005a1610). */
extern int g_sidLinePaletteDirty;

/* Lazily built BITMAPINFO for the scanline blitter, with its flag. */
extern void *g_lineBmi;
extern int g_lineBmiReady;

/* Allocates graphics page `page` (1..9) as a width x height x bpp surface. */
SidPage *Sid_AllocPage(int page, int width, int height, int bpp);

/* Tears a page down and clears its table slot. Page 0 may not be freed. */
int Sid_FreePage(int page);

/* Copies a page to the screen at the screen's dimensions, skipping the blit if
 * either DC is missing. */
void Sid_BlitPageToScreen(int page);

/* As above but sized from the source page, and without the DC checks. */
void Sid_BlitPageToScreenUnchecked(int page);

/* --- lib.c: drawing primitives ------------------------------------------ */


/* A rectangle in (x, y, w, h) form, as Sid_SetClipRect hands the old clip back.
 * Note this is a different convention from the context's edge pairs. */
typedef struct SidRect {
    int x;
    int y;
    int w;
    int h;
} SidRect;

/* Fills the whole page with a palette colour. */
void Sid_ClearPage(int page, int colorIndex);

/* Reads a pixel back as a palette index, or -1 if it cannot be resolved.
 * On page 0 this round-trips through GetPixel and searches the palette. */
unsigned int Sid_GetPixel(const SidContext *ctx, int x, int y);

/* The same function taking a page index rather than a context. Returns the
 * palette index, or 0xffffffff if the screen colour matches no palette entry. */
/* FUNCTION: SHANDALAR 0x00579520 */
unsigned int Sid_GetPixelPage(int page, int x, int y);

/* Draws a 1px line. Colour is a palette index, or an RGB value if negative. */
void Sid_DrawLine(const SidContext *ctx, int x1, int y1, int x2, int y2, int color);

/* Plots a single pixel. */
void Sid_SetPixel(const SidContext *ctx, int x, int y, long color);

/* Fills a rect with a solid brush. Same colour convention as Sid_SetPixel. */
void Sid_FillRect(const SidContext *ctx, int x, int y, int w, int h, long color);

/* Copies `count` bytes of one scanline out of a page into `dst`. Page 0 is not
 * supported — the original asserts on it. Takes a page index, not a context. */
void Sid_GetLine(void *dst, int page, int x, int y, unsigned int count);

/* The inverse: writes one scanline into a page. Page 0 goes out through
 * SetDIBitsToDevice as a 1-row image; every other page is a straight copy. */
void Sid_PutLine(const void *src, int page, int x, int y, unsigned int count);

/* Draws a 24bpp buffer straight to a window, building and discarding a
 * throwaway BITMAPINFO. Returns SetDIBitsToDevice's result. */
int Sid_BlitToWindow24(void *hwnd, const void *bits, int x, int y,
                       unsigned long w, unsigned long h);

/* Installs a new clip rect on the context and returns the previous one in
 * (x, y, w, h) form. Returned by value — MSVC passes a hidden pointer for the
 * result, which is what makes the decompiler show a leading out-parameter. */
SidRect Sid_SetClipRect(SidContext *ctx, int x, int y, int w, int h);

/* --- lib.c: blitters ----------------------------------------------------- */

/* Plain StretchBlt from one page to another, source context first. No fast
 * path, no palette work, no row flipping. */
/* FUNCTION: SHANDALAR 0x00579e40 */
void Sid_StretchBlitPage(const SidContext *src, int srcX, int srcY,
                         int srcW, int srcH,
                         const SidContext *dst, int dstX, int dstY,
                         int dstW, int dstH);

/* The workhorse: copies a rect from one page to another. Three strategies —
 * SetDIBitsToDevice straight to the screen, a per-row memmove between pages of
 * equal depth, or GDI BitBlt as the fallback.
 *
 * The screen path vertically flips the source rows in place and LEAVES THEM
 * FLIPPED, because SetDIBitsToDevice wants bottom-up rows and the pages are
 * top-down. Callers that need the source intact use Sid_BlitPageRestore. */
void Sid_BlitPage(const SidContext *src, unsigned int srcX, int srcY,
                  unsigned long w, unsigned long h,
                  const SidContext *dst, int dstX, int dstY);

/* As Sid_BlitPage's screen path, but flips the source rows back afterwards so
 * the page is left unmodified. Has no page-to-page branch. */
void Sid_BlitPageRestore(const SidContext *src, unsigned int srcX, int srcY,
                         unsigned long w, unsigned long h,
                         const SidContext *dst, int dstX, int dstY);

#endif /* SIDLIB_GFX_H */
