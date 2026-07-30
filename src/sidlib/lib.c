/* sidlib/lib.c — graphics page management.
 *
 * Assert line numbers (289, 311, 315) are the originals recovered from the
 * compiled assert sites.
 */

#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "sidlib/sidgfx.h"

/* The eight small functions at 0x00578c20..0x00578ca4 are /O2 code sitting
 * between window.c's /Od range and Sid_CreateScreenPage, so a new TU starts
 * here; lib.c is the nearest home. Several are stubs of an archive layer whose
 * file names ("misc.exe", "mgraphic.exe", "nsound.cvl") do not exist in the
 * shipped install. */

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

/* Fetch-and-clear of the pending event flag word. */
// FUNCTION: SHANDALAR 0x00578c60
int Sid_TakeEventFlags(void)
{
    int flags;

    flags = g_sidEventFlags;
    g_sidEventFlags = 0;
    return flags;
}

// FUNCTION: SHANDALAR 0x00578c70
void Sid_Noop(void)
{
}

/* Close half of the stubbed archive open/close pair. */
// FUNCTION: SHANDALAR 0x00578c80
int Sid_CloseArchive(int handle)
{
    (void)handle;
    return 0;
}

/* Only the font file argument is acted on; the data archive name is ignored. */
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

    /* The separate 24bpp arm is required even though it computes the same
     * size: the compiler folds the bodies but keeps the dead compare. */
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
        /* Identity DIB_PAL_COLORS table. */
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
    /* Must be an initialised array, not a memcpy: the original copies 7 bytes
     * and zeroes the remaining 9, which is what `char name[16] = "rpbips";`
     * compiles to. The page number replaces the terminator. */
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

    /* Rows padded to a 4-byte boundary; padding counted in pixels. */
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

    /* Clear the surface: dwords, then trailing bytes. */
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

    /* The first condition is a boolean — the original normalises it before
     * the call, unlike the range checks. */
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

    /* Left before top, as in Sid_FillRect — measured better than the emitted
     * store order, which is scheduler reordering, not source order. */
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

/* Context-taking twin of Sid_GetPixelPage; identical apart from reading
 * ctx->page. */
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
        /* The screen has no readable bit buffer: read through GDI and search
         * the palette. 16bpp hardware drops the low three bits per channel,
         * so compare masked. */
        screen = GetPixel((HDC)g_sidScreenDC, x, y);

        /* Channels extracted once, before the loop — inside the condition
         * MSVC recomputes them per iteration. */
        r = GetRValue(screen);
        g = GetGValue(screen);
        b = GetBValue(screen);

        /* Ternary form produces the original's sub/cmp/sbb carry trick. */
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

    /* Plain RGB, not PALETTERGB. The palette lookup must be the `if` branch:
     * the original falls through into it and jumps away on `jl`. */
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

    /* Page resolved up front, before the colour work. */
    p = g_sidPages[ctx->page];

    if (color >= 0) {
        if (color == 0xff) {
            color = SID_WHITE;
        } else {
            /* Explicit mask, not PALETTEINDEX: the (WORD) cast would emit
             * movzx where the original has `and eax,0xffff`. */
            color = 0x01000000L | (color & 0xffffL);
        }
    } else {
        /* The negated value is what gets tested against white. `v` signed so
         * the blue channel extracts with SAR. */
        v = -color;
        color = SID_WHITE;
        if (v != SID_WHITE) {
            color = PALETTERGB(GetRValue(v), GetGValue(v), GetBValue(v));
        }
    }

    /* Reassigning the parameter keeps the whole computation in EAX; a
     * separate local permutes everything downstream. */
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

    /* Rect built before the page is resolved, fields in RECT declaration
     * order — the original's store order. */
    rc.left = x;
    rc.top = y;
    rc.right = x + w;
    rc.bottom = h + y;
    p = g_sidPages[ctx->page];

    /* Same colour decode as Sid_SetPixel, inlined here in the original. */
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
    /* Both arms identical in the original — the srcY test has no effect. */
    if (srcY == 0) {
        ((BITMAPINFO *)g_blitBmi)->bmiHeader.biHeight = s->height;
    } else {
        ((BITMAPINFO *)g_blitBmi)->bmiHeader.biHeight = s->height;
    }

    if (dst->page == 0) {
        if ((srcX & 7) == 0 && g_sidDibBlitEnabled != 0) {
            if (g_sidScreenBpp != 8) {
                /* memcpy, not a loop — inlines to `rep movsd`. */
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
        /* Same-page downward copy would clobber rows it still needs, so that
         * case walks forwards; everything else walks backwards. */
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

        /* Flip the rows back so the caller's page is left untouched. The
         * original uses a third scratch buffer here. */
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

    /* Both pages resolved once into locals; the assignment order (p first)
     * is what reproduces the /G5 scheduling of the loads. */
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

/* Bare StretchBlt between two pages, no DIB fast paths. */
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

    /* Returned by value through the caller's hidden result pointer, as
     * (x, y, w, h) rather than edge pairs. */
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

    /* This assert passes a boolean — the original normalises page to 0 or 1
     * before the call, unlike most sidlib sites. */
    // STRING: SHANDALAR 0x005a16e8
    Sid_Assert(page != 0, SIDLIB_LIB_C, 1283, "GetLine not implemented for page 0\n");

    p = g_sidPages[page];

    /* Plain memcpy — MSVC's intrinsic expands to rep movsd + rep movsb,
     * which is what the original has. */
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
    /* Same initialised-array shape as Sid_AllocPage. This local is dead —
     * no file mapping is created for the screen — and carries the original's
     * "rpbits" typo where Sid_AllocPage has "rpbips". */
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
    /* Built twice; the first result is overwritten without being freed —
     * the original leaks one BITMAPINFO at startup. */
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
    /* Black and white left unreserved so the system keeps them. */
    pal->palPalEntry[0].peFlags = 0;
    pal->palPalEntry[255].peFlags = 0;
    g_sidLogPalette = pal;

    /* Clears the RGB table; the loop terminates by running into the page
     * table that follows it in .data. */
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

    /* Page first; putting the width store first was measured worse here. */
    p = g_sidPages[page];
    ((BITMAPINFO *)g_lineBmi)->bmiHeader.biWidth = count;

    if (page == 0) {
        if (g_sidScreenBpp != 8 && g_sidLinePaletteDirty != 0) {
            /* 1,024-byte memcpy — inlines to a bare rep movsd. */
            memcpy(((BITMAPINFO *)g_lineBmi)->bmiColors, g_sidDibPalette,
                   256 * sizeof(RGBQUAD));
            g_sidLinePaletteDirty = 0;
        }
        SetDIBitsToDevice((HDC)p->hdc, x, y, count, 1, 0, 0, 0, 1,
                          (void *)src, (BITMAPINFO *)g_lineBmi,
                          (UINT)(g_sidScreenBpp == 8));
        return;
    }

    /* rep movsd plus a rep movsb tail. */
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
