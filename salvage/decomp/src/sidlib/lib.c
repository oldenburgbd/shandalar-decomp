/* sidlib/lib.c — graphics page management.
 *
 * Reconstructed from Shandalar.exe 0x00579050 onwards. Assert line numbers are
 * the originals (289, 311, 315 -> 0x121, 0x137, 0x13b).
 */

#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "sidlib/sidgfx.h"

/* --- 0x00578c20 - 0x00578ca4 -------------------------------------------
 *
 * Eight very small functions sitting between window.c's /Od range (which ends
 * at 0x00578c02) and Sid_CreateScreenPage (0x00578e80). They are /O2 — direct
 * [esp+N] argument access and short-form jumps, none of the /Od tells — so a
 * new translation unit starts here, and lib.c is the nearest one it can belong
 * to. Attribution is by adjacency and build flags, not by assert evidence.
 *
 * Four of them do nothing at all: two return -1, one is empty, one takes an
 * argument and returns 0. Their callers sit in one init routine around
 * 0x0046e6f0 which opens archives named "misc.exe", "mgraphic.exe" and
 * "nsound.cvl" — names that do not exist in the shipped install. This looks
 * like a resource layer carried over from an earlier build of the game with
 * most of it stubbed out for the Windows release, but that is inference.
 */

/* Purpose unknown beyond the return value. One caller each, both inside the
 * same init routine. Named for their addresses rather than guessed at. */
// FUNCTION: SHANDALAR 0x00578c20
int Sid_Unknown578c20(void)
{
    return -1;
}

// FUNCTION: SHANDALAR 0x00578c30
int Sid_Unknown578c30(void)
{
    return -1;
}

// FUNCTION: SHANDALAR 0x00578c40
// FUNCTION: FACEMAKER 0x0040b320
int Sid_ShowCursor(void)
{
    return ShowCursor(TRUE);
}

// FUNCTION: SHANDALAR 0x00578c50
// FUNCTION: FACEMAKER 0x0040b330
int Sid_HideCursor(void)
{
    return ShowCursor(FALSE);
}

/* Reads the pending event flags and clears them in one go. The caller ORs the
 * result with a second flag word (0x00986d94) before storing it, so this is one
 * half of an accumulated event mask rather than a whole one. */
// FUNCTION: SHANDALAR 0x00578c60
int Sid_TakeEventFlags(void)
{
    int flags;

    flags = g_sidEventFlags;
    g_sidEventFlags = 0;
    return flags;
}

/* Empty. Two callers, both of which simply call it and continue. */
// FUNCTION: SHANDALAR 0x00578c70
void Sid_Noop(void)
{
}

/* Takes the handle Sid_OpenArchive returned and discards it. Every call site
 * pairs the two, so this is the close half of an open/close pair whose body
 * was removed rather than a function that never had one. */
// FUNCTION: SHANDALAR 0x00578c80
int Sid_CloseArchive(int handle)
{
    (void)handle;
    return 0;
}

/* Called as (dataFile, fontFile) with the data archive name always supplied and
 * the font file only sometimes: ("misc.exe", NULL), ("mgraphic.exe",
 * "fonts.cv"), ("nsound.cvl", NULL). Only the font file is acted on — the data
 * archive argument is never read, which is the clearest sign that this layer
 * used to do more than it does. */
// FUNCTION: SHANDALAR 0x00578c90
// FUNCTION: FACEMAKER 0x00406390
int Sid_OpenArchive(const char *dataFile, const char *fontFile)
{
    (void)dataFile;

    if (fontFile != NULL) {
        return Sid_LoadFontFile(fontFile);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0057a440
// FUNCTION: FACEMAKER 0x00408360
void *Sid_BuildBitmapInfo(int width, int height, int bpp)
{
    BITMAPINFO *bmi;
    unsigned short *palIndex;
    int i;

    /* 8bpp carries a 256-entry colour table; everything else needs only the
     * single RGBQUAD that BITMAPINFO already declares.
     *
     * The 24bpp arm is deliberate. The original enumerates it separately even
     * though it computes the same size, and the compiler folds the two bodies
     * together but still emits the now-dead `cmp esi,0x18` before the call.
     * Without this arm the function stops at 93.67%; with it, 100%. */
    if (bpp == 8) {
        bmi = (BITMAPINFO *)malloc(sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));
    } else if (bpp == 24) {
        bmi = (BITMAPINFO *)malloc(sizeof(BITMAPINFO));
    } else {
        bmi = (BITMAPINFO *)malloc(sizeof(BITMAPINFO));
    }

    bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi->bmiHeader.biWidth = width;
    bmi->bmiHeader.biHeight = -height;   /* negative => top-down rows */
    bmi->bmiHeader.biPlanes = 1;
    bmi->bmiHeader.biBitCount = (WORD)bpp;
    bmi->bmiHeader.biCompression = 0;
    bmi->bmiHeader.biSizeImage = 0;
    bmi->bmiHeader.biXPelsPerMeter = 0;
    bmi->bmiHeader.biYPelsPerMeter = 0;

    if (bpp == 8) {
        bmi->bmiHeader.biClrUsed = 256;
        bmi->bmiHeader.biClrImportant = 256;
        /* DIB_PAL_COLORS table: identity, so pixel values index the realized
         * palette directly. */
        palIndex = (unsigned short *)bmi->bmiColors;
        for (i = 0; i < 256; i++) {
            *palIndex++ = (unsigned short)i;
        }
    } else {
        bmi->bmiHeader.biClrUsed = 0;
        bmi->bmiHeader.biClrImportant = 0;
    }

    return bmi;
}

// FUNCTION: SHANDALAR 0x0057a4c0
int Sid_FreeBitmapInfo(void *bmi)
{
    free(bmi);
    return 1;
}

// FUNCTION: SHANDALAR 0x00579050
// FUNCTION: FACEMAKER 0x00406780
SidPage *Sid_AllocPage(int page, int width, int height, int bpp)
{
    SidPage *p;
    /* An initialised array, not a memcpy. The original reserves exactly 16
     * bytes, copies 7 from the literal and then zeroes the remaining 9 --
     * which is precisely what MSVC emits for `char name[16] = "rpbips";` and
     * not what any memcpy of 7 bytes would produce. The tail zeroing is the
     * evidence: nothing in the function reads name[7..15].
     *
     * Mapping name is "rpbips" with the page number appended in place of the
     * terminator, giving "rpbips0", "rpbips1", ... */
    // STRING: SHANDALAR 0x005a16b8
    // STRING: FACEMAKER 0x0040fbb4
    char name[16] = "rpbips";
    int bytesPerRow;
    int rem;
    unsigned long *fill;
    unsigned int words;
    unsigned int bytes;

    if (page == 0) {
        return Sid_CreateScreenPage(width, height, bpp);
    }

    // STRING: SHANDALAR 0x005a1690
    // STRING: FACEMAKER 0x0040fb8c
    Sid_Assert(page < SID_MAX_PAGES, SIDLIB_LIB_C, 289, "Graphic Page number out of range: %d\n", page);

    p = (SidPage *)malloc(sizeof(SidPage));
    p->width = width;
    p->height = height;
    p->bpp = bpp;

    /* Rows are padded to a 4-byte boundary. The padding is counted in pixels,
     * not bytes, and folded into the width when sizing the surface. */
    bytesPerRow = (bpp * width) / 8;
    rem = bytesPerRow % 4;
    if (rem == 0) {
        p->rowPadding = 0;
    } else {
        p->rowPadding = 4 - rem;
    }
    p->sizeBytes = ((p->rowPadding + width) * bpp * height) / 8;

    _itoa(page, name + 6, 10);

    p->hdc = CreateCompatibleDC((HDC)0);
    p->pBitmapInfo = Sid_BuildBitmapInfo(width, height, bpp);

    p->hFileMapping = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL,
                                         SEC_COMMIT | PAGE_READWRITE,
                                         0, p->sizeBytes, name);
    // STRING: SHANDALAR 0x005a1644
    // STRING: FACEMAKER 0x0040fb40
    Sid_Assert((int)p->hFileMapping, SIDLIB_LIB_C, 311, "Create File Mapping failed: page %d\n", page);

    /* iUsage is DIB_PAL_COLORS (1) for 8bpp, DIB_RGB_COLORS (0) otherwise. */
    p->hBitmap = CreateDIBSection((HDC)p->hdc, (BITMAPINFO *)p->pBitmapInfo,
                                  (UINT)(bpp == 8), (void **)&p->pBits,
                                  (HANDLE)p->hFileMapping, 0);
    // STRING: SHANDALAR 0x005a1628
    // STRING: FACEMAKER 0x0040fb24
    Sid_Assert((int)p->hBitmap, SIDLIB_LIB_C, 315, "WM_CREATE CreateDIBSection");

    p->hOldBitmap = SelectObject((HDC)p->hdc, (HGDIOBJ)p->hBitmap);
    p->hPalette = g_sidPalette;
    SelectPalette((HDC)p->hdc, (HPALETTE)p->hPalette, 0);
    RealizePalette((HDC)p->hdc);
    SetStretchBltMode((HDC)p->hdc, COLORONCOLOR);

    /* Clear the surface: dwords first, then the trailing bytes. */
    fill = (unsigned long *)p->pBits;
    for (words = (unsigned int)p->sizeBytes >> 2; words != 0; words--) {
        *fill++ = 0;
    }
    for (bytes = (unsigned int)p->sizeBytes & 3; bytes != 0; bytes--) {
        *(unsigned char *)fill = 0;
        fill = (unsigned long *)((char *)fill + 1);
    }

    return p;
}

// FUNCTION: SHANDALAR 0x00579240
// FUNCTION: FACEMAKER 0x00406970
int Sid_FreePage(int page)
{
    SidPage *p;

    /* A boolean, like Sid_GetLine's and unlike the range checks: the original
     * emits cmp/sbb/inc to normalise page to 0 or 1 before the call. */
    // STRING: SHANDALAR 0x005a16c0
    // STRING: FACEMAKER 0x0040fbbc
    Sid_Assert(page != 0, SIDLIB_LIB_C, 342, "Cannot explicitly Deallocate page 0\n");
    // STRING: SHANDALAR 0x005a1690
    // STRING: FACEMAKER 0x0040fb8c
    Sid_Assert(page < SID_MAX_PAGES, SIDLIB_LIB_C, 343, "Graphic Page number out of range: %d\n", page);

    p = g_sidPages[page];
    if (p == NULL) {
        return 0;
    }

    SelectObject((HDC)p->hdc, (HGDIOBJ)p->hOldBitmap);
    DeleteObject((HGDIOBJ)p->hBitmap);
    free(p->pBitmapInfo);
    CloseHandle((HANDLE)p->hFileMapping);

    SelectObject((HDC)p->hdc, GetStockObject(DEFAULT_PALETTE));
    RealizePalette((HDC)p->hdc);
    DeleteDC((HDC)p->hdc);

    free(p);
    g_sidPages[page] = NULL;
    return 0;
}

// FUNCTION: SHANDALAR 0x00579480
// FUNCTION: FACEMAKER 0x00406c30
void Sid_ClearPage(int page, int colorIndex)
{
    SidPage *p;
    LOGBRUSH lb;
    HBRUSH brush;
    RECT rc;

    lb.lbStyle = BS_SOLID;
    p = g_sidPages[page];
    lb.lbColor = PALETTERGB(g_sidPaletteRgb[colorIndex].r,
                            g_sidPaletteRgb[colorIndex].g,
                            g_sidPaletteRgb[colorIndex].b);
    brush = CreateBrushIndirect(&lb);

    /* The original's two zero stores land at E-0x18 then E-0x1c, i.e. top
     * before left -- but writing it that way, either as two statements or as
     * `rc.left = rc.top = 0;`, measures 61.54% against 63.74% for left-first.
     * So the emitted order is the scheduler reordering two independent stores
     * of the same immediate, not source order, and it is not a lever. Left
     * first, as in Sid_FillRect. Both stores land after CreateBrushIndirect. */
    rc.left = 0;
    rc.top = 0;
    rc.right = p->width;
    rc.bottom = p->height;
    FillRect((HDC)p->hdc, &rc, brush);
    DeleteObject(brush);
}

// FUNCTION: SHANDALAR 0x00579520
// FUNCTION: FACEMAKER 0x00406dc0
unsigned int Sid_GetPixelPage(int page, int x, int y)
{
    SidPage *p;
    COLORREF screen;
    unsigned char *entry;
    unsigned int index;
    unsigned int mask;
    unsigned int r;
    unsigned int g;
    unsigned int b;

    p = g_sidPages[page];

    if (page == 0) {
        screen = GetPixel((HDC)g_sidScreenDC, x, y);

        r = GetRValue(screen);
        g = GetGValue(screen);
        b = GetBValue(screen);
        mask = (g_sidScreenBpp == 16) ? 0xf8 : 0xff;

        entry = (unsigned char *)g_sidPaletteRgb;
        index = 0;

        while ((entry[0] & mask) != r ||
               (entry[1] & mask) != g ||
               (entry[2] & mask) != b) {
            index++;
            entry += 4;
            if ((int)index >= 256) {
                return 0xffffffff;
            }
        }
        return index;
    }

    return *SID_PIXEL_AT(p, x, y);
}

/* The context-taking twin of the above, 198 and 199 bytes and identical
 * instruction for instruction apart from the extra `mov eax,[eax]` that reads
 * ctx->page. sidlib carries both forms for several entry points — compare
 * Sid_PutLine and Sid_GetLine, which take a page index, against the drawing
 * primitives, which take a context. Which of the two deserves the plain name
 * is a guess; the page-taking one sits first in the file. */
// FUNCTION: SHANDALAR 0x005795f0
// FUNCTION: FACEMAKER 0x00407110
unsigned int Sid_GetPixel(const SidContext *ctx, int x, int y)
{
    SidPage *p;
    COLORREF screen;
    unsigned char *entry;
    unsigned int index;
    unsigned int mask;
    unsigned int r;
    unsigned int g;
    unsigned int b;

    p = g_sidPages[ctx->page];

    if (ctx->page == 0) {
        /* The screen has no readable bit buffer, so read the pixel through GDI
         * and search the palette for it. At 16bpp the hardware has dropped the
         * low three bits of each channel, so compare masked. */
        screen = GetPixel((HDC)g_sidScreenDC, x, y);

        /* The three channels are extracted once, before the loop — the
         * original spills two of them to the stack and keeps the third in a
         * register. Leaving the extraction inside the loop condition makes
         * MSVC recompute it on every iteration. */
        r = GetRValue(screen);
        g = GetGValue(screen);
        b = GetBValue(screen);

        /* Written as a ternary, which is what produces the original's
         * `sub/cmp/sbb` carry trick. Spelling the same value out as explicit
         * arithmetic on the comparison result gets `sete`/`neg` instead. */
        mask = (g_sidScreenBpp == 16) ? 0xf8 : 0xff;

        entry = (unsigned char *)g_sidPaletteRgb;
        index = 0;

        while ((entry[0] & mask) != r ||
               (entry[1] & mask) != g ||
               (entry[2] & mask) != b) {
            index++;
            entry += 4;
            if ((int)index >= 256) {
                return 0xffffffff;
            }
        }
        return index;
    }

    return *SID_PIXEL_AT(p, x, y);
}

// FUNCTION: SHANDALAR 0x005796c0
// FUNCTION: FACEMAKER 0x004071e0
void Sid_DrawLine(const SidContext *ctx, int x1, int y1, int x2, int y2, int color)
{
    SidPage *p;
    HPEN pen;
    HGDIOBJ oldPen;
    COLORREF rgb;

    p = g_sidPages[ctx->page];

    /* Note this one uses plain RGB, not PALETTERGB as Sid_ClearPage does.
     *
     * The palette case is the `if` branch, not the `else`: the original tests
     * and falls through into the palette lookup, jumping away on `jl` to the
     * negation. Writing the negative case first inverts the whole block. */
    if (color >= 0) {
        rgb = RGB(g_sidPaletteRgb[color].r,
                  g_sidPaletteRgb[color].g,
                  g_sidPaletteRgb[color].b);
    } else {
        rgb = (COLORREF)-color;
    }

    pen = CreatePen(PS_SOLID, 1, rgb);
    oldPen = SelectObject((HDC)p->hdc, pen);
    MoveToEx((HDC)p->hdc, x1, y1, (LPPOINT)0);
    LineTo((HDC)p->hdc, x2, y2);
    SelectObject((HDC)p->hdc, oldPen);
    DeleteObject(pen);
}

// FUNCTION: SHANDALAR 0x00579760
// FUNCTION: FACEMAKER 0x00407280
void Sid_SetPixel(const SidContext *ctx, int x, int y, long color)
{
    SidPage *p;
    long v;

    /* The page is resolved up front, before any of the colour work. */
    p = g_sidPages[ctx->page];

    if (color >= 0) {
        if (color == 0xff) {
            color = SID_WHITE;
        } else {
            /* Not PALETTEINDEX. That macro's (WORD) cast compiles to `movzx`;
             * the original emits `and eax,0xffff`, so the source masked the
             * value itself rather than casting it. Same result, different
             * instruction — and the instruction is the evidence. */
            color = 0x01000000L | (color & 0xffffL);
        }
    } else {
        /* The negated value is what gets tested against white — comparing the
         * original against 0xff000001 is the same predicate but compiles
         * differently. `v` is signed, which is what makes the blue channel
         * extract with SAR rather than SHR. */
        v = -color;
        color = SID_WHITE;
        if (v != SID_WHITE) {
            color = PALETTERGB(GetRValue(v), GetGValue(v), GetBValue(v));
        }
    }

    /* The colour parameter is reassigned rather than built in a separate local.
     * That is what keeps the whole computation in EAX, as the original does:
     * a separate `rgb` local gives it its own register and permutes everything
     * downstream. */
    SetPixelV((HDC)p->hdc, x, y, (COLORREF)color);
}

// FUNCTION: SHANDALAR 0x005797e0
// FUNCTION: FACEMAKER 0x00407300
void Sid_FillRect(const SidContext *ctx, int x, int y, int w, int h, long color)
{
    SidPage *p;
    RECT rc;
    HBRUSH brush;
    long v;

    /* The rect is built before the page is resolved, as in the original, and
     * the fields are assigned in RECT declaration order — left, top, right,
     * bottom — which is the order the original stores them. Assigning right
     * before top keeps x+w alive in EAX across the top store, which knocks the
     * colour value out of EAX further down. */
    rc.left = x;
    rc.top = y;
    rc.right = x + w;
    rc.bottom = h + y;
    p = g_sidPages[ctx->page];

    /* Byte-for-byte the same colour decode as Sid_SetPixel, and written the
     * same way for the same reasons: the positive case first so the branch is
     * `jl` to the negative arm, the parameter reassigned in place to keep the
     * value in EAX, an explicit mask rather than PALETTEINDEX, and the test
     * against white applied to the negated value rather than to 0xff000001.
     * The original inlines this block rather than calling a helper. */
    if (color >= 0) {
        if (color == 0xff) {
            color = SID_WHITE;
        } else {
            color = 0x01000000L | (color & 0xffffL);
        }
    } else {
        v = -color;
        color = SID_WHITE;
        if (v != SID_WHITE) {
            color = PALETTERGB(GetRValue(v), GetGValue(v), GetBValue(v));
        }
    }

    brush = CreateSolidBrush((COLORREF)color);
    FillRect((HDC)p->hdc, &rc, brush);
    DeleteObject(brush);
}

// FUNCTION: SHANDALAR 0x00579890
// FUNCTION: FACEMAKER 0x004073b0
void Sid_BlitPage(const SidContext *src, unsigned int srcX, int srcY,
                  unsigned long w, unsigned long h,
                  const SidContext *dst, int dstX, int dstY)
{
    SidPage *s;
    SidPage *d;
    unsigned char *top;
    unsigned char *bot;
    int i;
    int rows;

    s = g_sidPages[src->page];
    d = g_sidPages[dst->page];

    if (g_blitBmiReady == 0) {
        g_blitBmi = Sid_BuildBitmapInfo(1, 1, 8);
        g_blitBmiReady = 1;
    }

    ((BITMAPINFO *)g_blitBmi)->bmiHeader.biWidth = s->width;
    /* Both arms are identical in the original — the srcY test has no effect.
     * Reproduced as-is; see docs/quirks.md. */
    if (srcY == 0) {
        ((BITMAPINFO *)g_blitBmi)->bmiHeader.biHeight = s->height;
    } else {
        ((BITMAPINFO *)g_blitBmi)->bmiHeader.biHeight = s->height;
    }

    if (dst->page == 0) {
        if ((srcX & 7) == 0 && g_sidDibBlitEnabled != 0) {
            if (g_sidScreenBpp != 8) {
                /* `rep movsd`, as in Sid_PutLine — a memcpy, not a loop. */
                memcpy(((BITMAPINFO *)g_blitBmi)->bmiColors, g_sidDibPalette,
                       256 * sizeof(RGBQUAD));
            }

            /* Flip the rect's rows so the top-down page reads bottom-up, and
             * leave it flipped. */
            top = (unsigned char *)s->pBits + srcY * s->width + srcX;
            bot = (unsigned char *)s->pBits + (srcY + h - 1) * s->width + srcX;
            rows = (int)h / 2;
            if (rows > 0) {
                do {
                    Sid_FastMemcpy(g_sidRowTemp1, top, w);
                    Sid_FastMemcpy(top, bot, w);
                    Sid_FastMemcpy(bot, g_sidRowTemp1, w);
                    top += s->width;
                    bot -= s->width;
                    rows--;
                } while (rows != 0);
            }

            SetDIBitsToDevice((HDC)d->hdc, dstX, dstY, w, h, srcX, srcY,
                              0, s->height, s->pBits,
                              (BITMAPINFO *)g_blitBmi,
                              (UINT)(g_sidScreenBpp == 8));
            return;
        }
    } else if (src->page != 0 && s->bpp == d->bpp) {
        /* Same page copying downwards would overwrite rows it still needs, so
         * that case walks forwards; everything else walks backwards. */
        if (s == d && dstY < srcY) {
            if ((int)h < 1) {
                return;
            }
            i = 0;
            do {
                memmove((char *)d->pBits + SID_ROW_BYTES(d) * (dstY + i)
                            + (dstX * d->bpp) / 8,
                        (char *)s->pBits + SID_ROW_BYTES(s) * (srcY + i)
                            + (srcX * s->bpp) / 8,
                        (w * d->bpp) / 8);
                i++;
            } while (i < (int)h);
            return;
        }

        i = (int)h - 1;
        if (i < 0) {
            return;
        }
        do {
            memmove((char *)d->pBits + SID_ROW_BYTES(d) * (dstY + i)
                        + (dstX * d->bpp) / 8,
                    (char *)s->pBits + SID_ROW_BYTES(s) * (srcY + i)
                        + (srcX * s->bpp) / 8,
                    (w * d->bpp) / 8);
            i--;
        } while (i >= 0);
        return;
    }

    BitBlt((HDC)d->hdc, dstX, dstY, w, h, (HDC)s->hdc, srcX, srcY, SRCCOPY);
}

// FUNCTION: SHANDALAR 0x00579bf0
// FUNCTION: FACEMAKER 0x00407710
void Sid_BlitPageRestore(const SidContext *src, unsigned int srcX, int srcY,
                         unsigned long w, unsigned long h,
                         const SidContext *dst, int dstX, int dstY)
{
    SidPage *s;
    SidPage *d;
    unsigned char *top;
    unsigned char *bot;
    int half;
    int rows;
    int lastRow;

    s = g_sidPages[src->page];
    d = g_sidPages[dst->page];

    if (g_blitBmi2Ready == 0) {
        g_blitBmi2 = Sid_BuildBitmapInfo(1, 1, 8);
        g_blitBmi2Ready = 1;
    }

    ((BITMAPINFO *)g_blitBmi2)->bmiHeader.biWidth = s->width;
    /* Identical arms again, as in Sid_BlitPage. */
    if (srcY == 0) {
        ((BITMAPINFO *)g_blitBmi2)->bmiHeader.biHeight = s->height;
    } else {
        ((BITMAPINFO *)g_blitBmi2)->bmiHeader.biHeight = s->height;
    }

    if (dst->page == 0 && (srcX & 7) == 0 && g_sidDibBlitEnabled != 0) {
        if (g_sidScreenBpp != 8) {
            memcpy(((BITMAPINFO *)g_blitBmi2)->bmiColors, g_sidDibPalette,
                   256 * sizeof(RGBQUAD));
        }

        top = (unsigned char *)s->pBits + srcX + s->width * srcY;
        lastRow = srcY - 1 + h;
        bot = (unsigned char *)s->pBits + srcX + lastRow * s->width;
        half = (int)h / 2;

        rows = half;
        if (rows > 0) {
            do {
                Sid_FastMemcpy(g_sidRowTemp2, top, w);
                Sid_FastMemcpy(top, bot, w);
                Sid_FastMemcpy(bot, g_sidRowTemp2, w);
                top += s->width;
                bot -= s->width;
                rows--;
            } while (rows != 0);
        }

        SetDIBitsToDevice((HDC)d->hdc, dstX, dstY, w, h, srcX, srcY,
                          0, s->height, s->pBits,
                          (BITMAPINFO *)g_blitBmi2,
                          (UINT)(g_sidScreenBpp == 8));

        /* Flip the rows back so the caller's page is left untouched. Note the
         * original uses a third scratch buffer here, not the one above. */
        top = (unsigned char *)s->pBits + srcX + s->width * srcY;
        bot = (unsigned char *)s->pBits + srcX + lastRow * s->width;
        rows = half;
        if (rows > 0) {
            do {
                Sid_FastMemcpy(g_sidRowTemp3, top, w);
                Sid_FastMemcpy(top, bot, w);
                Sid_FastMemcpy(bot, g_sidRowTemp3, w);
                top += s->width;
                bot -= s->width;
                rows--;
            } while (rows != 0);
        }
        return;
    }

    BitBlt((HDC)d->hdc, dstX, dstY, w, h, (HDC)s->hdc, srcX, srcY, SRCCOPY);
}

// FUNCTION: SHANDALAR 0x00579400
// FUNCTION: FACEMAKER 0x00406bb0
void Sid_BlitPageToScreen(int page)
{
    SidPage *p;
    SidPage *screen;

    /* Both pages are resolved once into locals. The original loads
     * g_sidPages[0] a single time and keeps it in EDX across the guard and all
     * three field reads; going back to the array each time costs an extra
     * register (ours had to spill the BitBlt import into EBP, which is why the
     * original pushes only EBX where ours pushed two).
     *
     * `p` is assigned first even though the original's screen load is emitted
     * first. That is /G5 scheduling, not source order: the argument fetch
     * issues before the frame is set up, then the independent g_sidPages[0]
     * load fills the slot while it is still in flight, and the load that
     * depends on it comes last. Writing them the other way round leaves the
     * argument fetch stranded after the push. */
    p = g_sidPages[page];
    screen = g_sidPages[0];

    if (p->hdc != NULL && screen->hdc != NULL) {
        BitBlt((HDC)screen->hdc, 0, 0, screen->width, screen->height,
               (HDC)p->hdc, 0, 0, SRCCOPY);
    }
}

// FUNCTION: SHANDALAR 0x00579440
// FUNCTION: FACEMAKER 0x00406bf0
void Sid_BlitPageToScreenUnchecked(int page)
{
    SidPage *p;

    p = g_sidPages[page];
    BitBlt((HDC)g_sidPages[0]->hdc, 0, 0, p->width, p->height,
           (HDC)p->hdc, 0, 0, SRCCOPY);
}

/* A bare StretchBlt between two pages, with none of the DIB fast paths, palette
 * handling or row flipping the other blitters carry — it just resolves both
 * pages to their DCs and hands everything to GDI. Source context first, as in
 * Sid_BlitPage; the destination rectangle is the second group. */
// FUNCTION: SHANDALAR 0x00579e40
// FUNCTION: FACEMAKER 0x00407960
void Sid_StretchBlitPage(const SidContext *src, int srcX, int srcY,
                         int srcW, int srcH,
                         const SidContext *dst, int dstX, int dstY,
                         int dstW, int dstH)
{
    SidPage *s;
    SidPage *d;

    s = g_sidPages[src->page];
    d = g_sidPages[dst->page];

    StretchBlt((HDC)d->hdc, dstX, dstY, dstW, dstH,
               (HDC)s->hdc, srcX, srcY, srcW, srcH, SRCCOPY);
}

// FUNCTION: SHANDALAR 0x00579ea0
// FUNCTION: FACEMAKER 0x00407dc0
SidRect Sid_SetClipRect(SidContext *ctx, int x, int y, int w, int h)
{
    SidRect old;

    /* Captured into a local and returned by value; the compiler turns that into
     * a copy through the caller's hidden result pointer. Handed back as
     * (x, y, w, h), not as the edge pairs the context stores. */
    old.x = ctx->clipLeft;
    old.y = ctx->clipTop;
    old.w = ctx->clipRight - ctx->clipLeft;
    old.h = ctx->clipBottom - ctx->clipTop;

    ctx->clipTop = y;
    ctx->clipLeft = x;
    ctx->clipRight = x + w;
    ctx->clipBottom = h + y;

    return old;
}

// FUNCTION: SHANDALAR 0x0057a000
void Sid_GetLine(void *dst, int page, int x, int y, unsigned int count)
{
    SidPage *p;

    /* Unlike the other assert sites, this one passes a *boolean*: the original
     * emits `cmp/sbb/inc` to normalise page to 0 or 1 before the call. Passing
     * the raw value, which is right everywhere else in sidlib, is wrong here. */
    // STRING: SHANDALAR 0x005a16e8
    Sid_Assert(page != 0, SIDLIB_LIB_C, 1283, "GetLine not implemented for page 0\n");

    p = g_sidPages[page];

    /* A plain memcpy. The original expands to `rep movsd` for the dwords
     * followed by `rep movsb` for the remainder, which is MSVC's inline
     * expansion of the memcpy intrinsic — not a hand-written copy loop. This
     * had been reconstructed as an explicit dword-then-byte loop, which
     * generates a completely different function body.
     *
     * Note this is unrelated to Sid_FastMemcpy (0x00566cea), which is a
     * separate x87-based bulk mover living outside sidlib. */
    memcpy(dst, SID_PIXEL_AT(p, x, y), count);
}

// FUNCTION: SHANDALAR 0x00578e80
// FUNCTION: FACEMAKER 0x004065b0
SidPage *Sid_CreateScreenPage(int width, int height, int bpp)
{
    SidPage *p;
    LOGPALETTE *pal;
    HPALETTE hpal;
    HDC hdc;
    SidRgb *entry;
    /* Same initialised-array shape as Sid_AllocPage: 16 bytes reserved, 7
     * copied, 9 zeroed. Page 0's mapping name is built and then never used --
     * no file mapping is created for the screen -- so this whole local is
     * dead, which is presumably how the typo survived: the template is
     * "rpbits" here where Sid_AllocPage uses "rpbips". Reproduced as found. */
    // STRING: SHANDALAR 0x005a1620
    // STRING: FACEMAKER 0x0040fb1c
    char name[16] = "rpbits";
    int i;

    if (g_sidScreenPageReady != 0) {
        return g_sidPages[0];
    }

    p = (SidPage *)malloc(sizeof(SidPage));
    g_sidScreenWidth = width;
    p->width = width;
    g_sidScreenHeight = height;
    p->height = height;

    if (bpp == -1) {
        hdc = GetDC((HWND)0);
        bpp = GetDeviceCaps(hdc, BITSPIXEL);
        g_sidScreenBpp = bpp;
        p->bpp = bpp;
        ReleaseDC((HWND)0, hdc);
    } else {
        g_sidScreenBpp = bpp;
        p->bpp = bpp;
    }

    g_sidUnknown160c = 0;
    p->sizeBytes = (bpp * height * width) / 8;
    _itoa(0, name, 10);

    p->hdc = g_sidScreenDC;
    /* Built twice: the first result is overwritten without being freed, so the
     * screen page leaks one BITMAPINFO at startup. See docs/quirks.md Q-008. */
    p->pBitmapInfo = Sid_BuildBitmapInfo(width, height, bpp);
    p->pBitmapInfo = Sid_BuildBitmapInfo(width, height, 8);
    p->hFileMapping = NULL;
    p->hBitmap = NULL;

    pal = (LOGPALETTE *)malloc(sizeof(LOGPALETTE) + 256 * sizeof(PALETTEENTRY));
    pal->palVersion = 0x300;
    pal->palNumEntries = 256;
    for (i = 256; i != 0; i--) {
        pal->palPalEntry[256 - i].peRed = 0;
        pal->palPalEntry[256 - i].peGreen = 0;
        pal->palPalEntry[256 - i].peBlue = 0;
        pal->palPalEntry[256 - i].peFlags = PC_RESERVED;
    }
    /* Black and white are left unreserved so the system keeps them. */
    pal->palPalEntry[0].peFlags = 0;
    pal->palPalEntry[255].peFlags = 0;
    g_sidLogPalette = pal;

    /* Clears the RGB table. The original terminates this loop by running into
     * the page table, which happens to sit immediately after it in .data —
     * see docs/quirks.md Q-008. */
    entry = g_sidPaletteRgb;
    do {
        entry->r = 0;
        entry->g = 0;
        entry->b = 0;
        entry->pad = 1;
        entry++;
    } while (entry < (SidRgb *)g_sidPages);
    g_sidPaletteRgb[0].pad = 0;
    g_sidPaletteRgb[255].pad = 0;

    hpal = CreatePalette(pal);
    g_sidPaletteHandle = hpal;
    g_sidPalette = hpal;
    p->hPalette = hpal;

    SelectPalette((HDC)p->hdc, hpal, 0);
    RealizePalette((HDC)p->hdc);
    SetStretchBltMode((HDC)p->hdc, COLORONCOLOR);

    g_sidPages[0] = p;
    g_sidScreenPageReady = 1;
    return p;
}

// FUNCTION: SHANDALAR 0x00579f10
void Sid_PutLine(const void *src, int page, int x, int y, unsigned int count)
{
    SidPage *p;

    if (g_lineBmiReady == 0) {
        g_lineBmi = Sid_BuildBitmapInfo(1, 1, 8);
        g_lineBmiReady = 1;
    }

    /* Page first. Putting the width store first -- the /G5 shape that took
     * Sid_BlitPageToScreen to byte-exact -- costs 3.05 here, so that trick is
     * not general. Measured both ways. */
    p = g_sidPages[page];
    ((BITMAPINFO *)g_lineBmi)->bmiHeader.biWidth = count;

    if (page == 0) {
        if (g_sidScreenBpp != 8 && g_sidLinePaletteDirty != 0) {
            /* A 1,024-byte memcpy, which inlines to a bare `rep movsd` with no
             * byte tail because the length is an exact multiple of four. This
             * was an explicit 256-iteration RGBQUAD copy loop. */
            memcpy(((BITMAPINFO *)g_lineBmi)->bmiColors, g_sidDibPalette,
                   256 * sizeof(RGBQUAD));
            g_sidLinePaletteDirty = 0;
        }
        SetDIBitsToDevice((HDC)p->hdc, x, y, count, 1, 0, 0, 0, 1,
                          (void *)src, (BITMAPINFO *)g_lineBmi,
                          (UINT)(g_sidScreenBpp == 8));
        return;
    }

    /* And the line copy itself, `rep movsd` plus a `rep movsb` tail.
     *
     * The original loads width then rowPadding here, where Sid_GetPixelPage
     * loads rowPadding then width -- but spelling this out as
     * `(p->width + p->rowPadding)` measures identical, so the difference is
     * MSVC canonicalising the addition rather than anything in the source.
     * Keep the shared macro. */
    memcpy(SID_PIXEL_AT(p, x, y), src, count);
}

// FUNCTION: SHANDALAR 0x0057a3d0
// FUNCTION: FACEMAKER 0x004082f0
int Sid_BlitToWindow24(void *hwnd, const void *bits, int x, int y,
                       unsigned long w, unsigned long h)
{
    BITMAPINFO *bmi;
    HDC hdc;
    int result;

    bmi = (BITMAPINFO *)Sid_BuildBitmapInfo(w, h, 24);
    hdc = GetDC((HWND)hwnd);
    result = SetDIBitsToDevice(hdc, x, y, w, h, 0, 0, 0, h,
                               (void *)bits, bmi, DIB_RGB_COLORS);
    ReleaseDC((HWND)hwnd, hdc);
    free(bmi);
    return result;
}

