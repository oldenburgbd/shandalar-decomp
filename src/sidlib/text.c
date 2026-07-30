/* sidlib/text.c — formatted text output.
 *
 * The eleven entry points at the bottom are pure forwarders into
 * Sid_DrawTextCore (0x0057b230), each pinning a different combination of its
 * four layout flags. Ordered by address.
 *
 * `va_start(args, y)` is deliberate in the forwarders: the format string is
 * unnamed and arrives as the first variadic argument, so the list starts
 * immediately after y — the stack address the original passes.
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "sidlib/sidtext.h"

/* Loads a font file: a 16-bit count, then that many 16-bit file offsets, then
 * one record per offset. Slots are 1-based — offsets[0] is never touched.
 * Returns the count and publishes it to g_sidFontCount. The assert pins
 * SID_MAX_FONTS at 16, the original's own limit.
 *
 * Each offset is zeroed immediately before its 2-byte fread — the reads land
 * in the low half of a 32-bit slot.
 *
 * The first assert condition is a boolean (the original normalises the FILE*
 * to 0 or 1), unlike Spr_LoadFile which pushes the pointer raw. */
// FUNCTION: SHANDALAR 0x0057a4e0
// FUNCTION: FACEMAKER 0x00408710
int Sid_LoadFontFile(const char *path)
{
    /* Declaration order measured; moving `i` earlier scores worse. */
    long offsets[SID_MAX_FONTS];
    int count;
    FILE *fp;
    int i;

    count = 0;
    // STRING: SHANDALAR 0x005a17a4
    fp = fopen(path, "rb");
    Sid_Assert(fp != NULL, SIDLIB_TEXT_C, 131, g_sidMsgFileOpen, path);

    fread(&count, 2, 1, fp);
    g_sidFontCount = count;

    // STRING: SHANDALAR 0x005a1780
    Sid_Assert(count < SID_MAX_FONTS, SIDLIB_TEXT_C, 138, "Can not load more than %d fonts\n", SID_MAX_FONTS);

    for (i = 1; i <= count; i++) {
        offsets[i] = 0;
        fread(&offsets[i], 2, 1, fp);
    }

    for (i = 1; i <= count; i++) {
        fseek(fp, offsets[i], SEEK_SET);
        Sid_LoadFontRecord(i, fp);
    }

    fclose(fp);
    return count;
}

/* Reads one font record and builds the two GDI bitmaps Sid_DrawString blits
 * from. The stream arrives positioned *past* the record, so every read seeks
 * backwards first. Width tables are stored indexed from `firstChar` and read
 * straight into &charWidth[firstChar]. The glyph strip's row pitch is rounded
 * up to even (`oddPad`); rows are read one at a time because the file has no
 * padding and the buffer does. The buffer is over-allocated by 10000 bytes and
 * nothing uses the slack. The mask bitmap comes from the complemented bits,
 * which is why the buffer is inverted, used, and inverted back. Returns 0,
 * ignored by its caller. */
// FUNCTION: SHANDALAR 0x0057a5f0
int Sid_LoadFontRecord(int font, FILE *fp)
{
    SidFont *rec;
    unsigned char *hdr;
    unsigned char *dst;
    int charCount;
    int height;
    int stripBytes;
    int oddPad;
    int total;
    int i;

    rec = &g_sidFonts[font];
    /* The header is addressed through its own pointer, distinct from the
     * record base — the original reads firstChar as *hdr but lastChar as
     * rec->lastChar. */
    hdr = &rec->firstChar;

    fseek(fp, -8L, SEEK_CUR);
    fread(hdr, 1, 8, fp);

    charCount = rec->lastChar - *hdr + 1;
    if (rec->fixedWidth == 0) {
        fseek(fp, -8L - charCount, SEEK_CUR);
        fread(&rec->charWidth[*hdr], 1, charCount, fp);
        fseek(fp, 8L, SEEK_CUR);
    }

    height = rec->height;
    stripBytes = rec->cellBytes * charCount;
    oddPad = stripBytes & 1;
    rec->oddPad = (unsigned char)oddPad;

    if (rec->bits != NULL) {
        free(rec->bits);
    }
    if (rec->hGlyphBitmap != NULL) {
        DeleteObject((HGDIOBJ)rec->hGlyphBitmap);
    }
    if (rec->hMaskBitmap != NULL) {
        DeleteObject((HGDIOBJ)rec->hMaskBitmap);
    }

    total = (oddPad + stripBytes) * height;
    rec->bits = malloc(total + 10000);

    dst = (unsigned char *)rec->bits;
    if (height > 0) {
        i = height;
        do {
            fread(dst, 1, stripBytes, fp);
            dst += stripBytes + oddPad;
            i--;
        } while (i != 0);
    }

    stripBytes *= 8;
    rec->hGlyphBitmap = CreateBitmap(stripBytes, height, 1, 1, rec->bits);

    dst = (unsigned char *)rec->bits;
    if (total > 0) {
        i = total;
        do {
            *dst = (unsigned char)~*dst;
            dst++;
            i--;
        } while (i != 0);
    }

    rec->hMaskBitmap = CreateBitmap(stripBytes, height, 1, 1, rec->bits);

    dst = (unsigned char *)rec->bits;
    if (total > 0) {
        do {
            *dst = (unsigned char)~*dst;
            dst++;
            total--;
        } while (total != 0);
    }

    if (rec->hMemDC == NULL) {
        rec->hMemDC = CreateCompatibleDC(NULL);
    }
    rec->scaled = NULL;
    return 0;
}

/* Base height. The plain struct index reproduces the original's LEA-pair
 * strength reduction of the 684-byte stride. */
// FUNCTION: SHANDALAR 0x0057ae30
// FUNCTION: FACEMAKER 0x00409180
int Sid_GetFontHeight(int font)
{
    return g_sidFonts[font].height;
}

/* Base height plus one of two extras, chosen on the `scaled` pointer.
 * scaledExtra is a dword, extra a byte. */
// FUNCTION: SHANDALAR 0x0057adf0
// FUNCTION: FACEMAKER 0x00409140
int Sid_GetFontLineSpacing(int font)
{
    if (g_sidFonts[font].scaled != NULL) {
        return g_sidFonts[font].height + g_sidFonts[font].scaledExtra;
    }
    /* Residual AL/DL swap here is MSVC canonicalisation, not source order —
     * measured immovable, here and at the copy inside Sid_DrawTextCore. */
    return g_sidFonts[font].height + g_sidFonts[font].extra;
}

/* Releases a loaded font slot. `scaled` doubles as the in-use flag. The early
 * exit really returns 0 (the original emits xor eax,eax). */
// FUNCTION: SHANDALAR 0x0057a9f0
// FUNCTION: FACEMAKER 0x00408d40
int Sid_FreeFont(int font)
{
    if (g_sidFonts[font].scaled == NULL) {
        return 0;
    }

    DeleteObject((HGDIOBJ)g_sidFonts[font].hFont);
    return RemoveFontResourceA(g_sidFonts[font].fontPath);
}

/* Width of the widest line, in the context's font. Two implementations chosen
 * on `scaled`: GDI measurement per line (+2 each), or a sum of the font's own
 * per-character widths. The GetCharABCWidthsA branch inside the bitmap arm is
 * unreachable — the outer test already proved `scaled` NULL — but it is in the
 * binary. Newlines are temporarily NUL-ed and restored, so the parameter
 * cannot be const. */
// FUNCTION: SHANDALAR 0x0057aae0
int Sid_MeasureText(SidContext *ctx, char *s)
{
    int cur;
    int maxw;
    /* One shared slot: the original's frame size shows the two measurement
     * out-parameters overlapping in the source, not by the compiler. */
    union {
        SIZE size;
        ABC abc;
    } m;
    char *p;
    char *line;
    HDC hdc;
    HGDIOBJ old;
    HDC screen;

    cur = 0;
    maxw = -1;
    p = s;
    line = s;

    if (g_sidFonts[ctx->font].scaled != NULL) {
        hdc = (HDC)g_sidPages[ctx->page]->hdc;
        old = SelectObject(hdc, (HGDIOBJ)g_sidFonts[ctx->font].hFont);

        while (*p != '\0') {
            if (*p == '\n') {
                *p = '\0';
                GetTextExtentPoint32A(hdc, line, strlen(line), &m.size);
                if (maxw < m.size.cx + 2) {
                    maxw = m.size.cx + 2;
                }
                line = p + 1;
                *p = '\n';
            }
            p++;
        }

        GetTextExtentPoint32A(hdc, line, strlen(line), &m.size);
        if (maxw < m.size.cx + 2) {
            maxw = m.size.cx + 2;
        }
        SelectObject(hdc, old);
        return maxw;
    }

    while (*p != '\0') {
        if (*p == '\n') {
            if (maxw < cur) {
                maxw = cur;
            }
            cur = 0;
        } else if (g_sidFonts[ctx->font].scaled != NULL) {
            screen = GetDC(NULL);
            SelectObject(screen, (HGDIOBJ)g_sidFonts[ctx->font].hFont);
            GetCharABCWidthsA(screen, *p, *p, &m.abc);
            ReleaseDC(NULL, screen);
            cur += m.abc.abcA + m.abc.abcB + m.abc.abcC;
        } else if (g_sidFonts[ctx->font].fixedWidth != 0) {
            cur += g_sidFonts[ctx->font].spacing + g_sidFonts[ctx->font].fixedWidth;
        } else {
            /* Signed index: characters above 0x7f read backwards into the
             * previous slot's table. */
            cur += g_sidFonts[ctx->font].spacing + g_sidFonts[ctx->font].charWidth[*p];
        }
        p++;
    }

    if (maxw <= cur) {
        maxw = cur;
    }
    return maxw;
}

/* Draws one line at (x, y); Sid_DrawTextCore has already split on newlines.
 * GDI path is a single TextOutA. The bitmap path walks the string twice —
 * mask with SRCAND, then glyphs with SRCPAINT — through one memory DC on the
 * font record. Returns 0 clipped/empty, 1 from GDI, count from the bitmap
 * path; no caller reads any of them. The dead GetCharABCWidthsA branch
 * appears once per pass. */
// FUNCTION: SHANDALAR 0x0057ae50
int Sid_DrawString(SidContext *ctx, int x, int y, const char *s)
{
    SidFont *font;
    HDC hdc;
    HDC srcDC;
    HDC screen;
    HGDIOBJ old;
    ABC abc;
    const char *start;
    int startX;
    int color;
    int advance;
    int srcX;
    int firstChar;
    int cellBytes;
    int height;

    start = s;
    startX = x;

    if (*s == '\0') {
        return 0;
    }
    if (ctx->clipTop > y) {
        return 0;
    }

    font = &g_sidFonts[ctx->font];
    if (font->scaled != NULL) {
        if (y + font->height + font->scaledExtra > ctx->clipBottom) {
            return 0;
        }
    } else {
        if (y + font->height + font->extra > ctx->clipBottom) {
            return 0;
        }
    }

    if (font->scaled != NULL) {
        hdc = (HDC)g_sidPages[ctx->page]->hdc;
        old = SelectObject(hdc, (HGDIOBJ)font->hFont);

        color = ctx->textColor;
        if (color >= 0xfe) {
            color = 0xfe;
        }
        SetTextColor(hdc, (color & 0xffff) | 0x1000000);
        SetBkMode(hdc, TRANSPARENT);
        TextOutA(hdc, x, y - font->scaledExtra, s, strlen(s));

        SelectObject(hdc, old);
        return 1;
    }

    srcDC = (HDC)font->hMemDC;
    hdc = (HDC)g_sidPages[ctx->page]->hdc;
    height = font->height;
    cellBytes = font->cellBytes;
    firstChar = font->firstChar;

    SelectObject(srcDC, (HGDIOBJ)font->hMaskBitmap);
    SetTextColor(hdc, 0x1000000);
    SetBkColor(hdc, 0xffffff);

    do {
        srcX = (*s - firstChar) * cellBytes;
        if (font->scaled != NULL) {
            screen = GetDC(NULL);
            SelectObject(screen, (HGDIOBJ)font->hFont);
            GetCharABCWidthsA(screen, *s, *s, &abc);
            ReleaseDC(NULL, screen);
            advance = abc.abcA + abc.abcB + abc.abcC;
        } else if (font->fixedWidth != 0) {
            advance = font->spacing + font->fixedWidth;
        } else {
            advance = font->charWidth[*s] + font->spacing;
        }
        BitBlt(hdc, x, y, advance, height, srcDC, srcX * 8, 0, SRCAND);
        s++;
        x += advance;
    } while (*s != '\0');

    x = startX;
    s = start;
    color = 0xfe;
    if (ctx->textColor != 0xff) {
        color = ctx->textColor;
    }

    SelectObject(srcDC, (HGDIOBJ)font->hGlyphBitmap);
    SetTextColor(hdc, 0x1000000);
    SetBkColor(hdc, (color & 0xffff) | 0x1000000);

    do {
        srcX = (*s - firstChar) * cellBytes;
        if (font->scaled != NULL) {
            screen = GetDC(NULL);
            SelectObject(screen, (HGDIOBJ)font->hFont);
            GetCharABCWidthsA(screen, *s, *s, &abc);
            ReleaseDC(NULL, screen);
            advance = abc.abcA + abc.abcB + abc.abcC;
        } else if (font->fixedWidth != 0) {
            advance = font->spacing + font->fixedWidth;
        } else {
            advance = font->charWidth[*s] + font->spacing;
        }
        BitBlt(hdc, x, y, advance, height, srcDC, srcX * 8, 0, SRCPAINT);
        x += advance;
        s++;
    } while (*s != '\0');

    return s - start;
}

/* Formats, splits on newlines, and draws each line.
 *
 * `args` points AT the format string: args[0] is the format, args + 4 the
 * variadic tail — that split is what the eleven entry points pass and what
 * makes the _vsnprintf call match.
 *
 * Each line's terminator is NUL-ed to draw and rewritten as '\n'; the last
 * rewrite clobbers the string's own NUL, harmlessly. When the formatted
 * string is empty, buf[-1] is read (`p` never advanced) — the original's
 * off-by-one, reproduced.
 *
 * `saved` is one slot reused for both the colour and shadow saves, so a call
 * with both restores the colour rather than the caller's — as the original
 * does. The line-spacing expression repeats Sid_GetFontLineSpacing's body;
 * MSVC 4.2 does not inline across functions, so the duplication is source. */
// FUNCTION: SHANDALAR 0x0057b230
// FUNCTION: FACEMAKER 0x00409660
int Sid_DrawTextCore(SidContext *ctx, int color, int shadow, int resScaled,
                     int hCenter, int vCenter, int x, int y, va_list args)
{
    char buf[SID_TEXT_BUFFER];
    char *line;
    int lines;
    int saved;
    int written;
    char *p;
    int lineX;

    written = _vsnprintf(buf, SID_TEXT_BUFFER, ((const char **)args)[0], args + 4);

    lines = 0;
    p = buf;
    if (*p != '\0') {
        do {
            if (*p == '\n') {
                lines++;
            }
            p++;
        } while (*p != '\0');
    }
    if (p[-1] != '\n') {
        lines++;
    }

    if (resScaled) {
        x = g_sidPages[0]->width * x / 640;
        y = g_sidPages[0]->height * y / 480;
    }

    if (vCenter) {
        y -= g_sidFonts[ctx->font].height * lines / 2;
    }

    if (color >= 0) {
        saved = ctx->textColor;
        ctx->textColor = color;
    }

    line = buf;
    p = buf;
    while (lines-- != 0) {
        while (*p != '\0' && *p != '\n') {
            p++;
        }
        *p = '\0';

        lineX = x;
        if (hCenter) {
            lineX -= Sid_MeasureText(ctx, line) / 2;
        }

        if (shadow) {
            saved = ctx->textColor;
            ctx->textColor = 0;
            Sid_DrawString(ctx, lineX + 1, y + 1, line);
            ctx->textColor = saved;
        }

        Sid_DrawString(ctx, lineX, y, line);
        *p++ = '\n';
        line = p;

        if (g_sidFonts[ctx->font].scaled != NULL) {
            y += g_sidFonts[ctx->font].height + g_sidFonts[ctx->font].scaledExtra;
        } else {
            y += g_sidFonts[ctx->font].height + g_sidFonts[ctx->font].extra;
        }
    }

    if (color >= 0) {
        ctx->textColor = saved;
    }
    return written;
}

/* Plain. */
// FUNCTION: SHANDALAR 0x0057b440
int Sid_DrawText(SidContext *ctx, int color, int x, int y, ...)
{
    va_list args;

    va_start(args, y);
    return Sid_DrawTextCore(ctx, color, 0, 0, 0, 0, x, y, args);
}

/* Vertically centred. */
// FUNCTION: SHANDALAR 0x0057b470
int Sid_DrawTextV(SidContext *ctx, int color, int x, int y, ...)
{
    va_list args;

    va_start(args, y);
    return Sid_DrawTextCore(ctx, color, 0, 0, 0, 1, x, y, args);
}

/* Centred both ways. */
// FUNCTION: SHANDALAR 0x0057b4a0
int Sid_DrawTextHV(SidContext *ctx, int color, int x, int y, ...)
{
    va_list args;

    va_start(args, y);
    return Sid_DrawTextCore(ctx, color, 0, 0, 1, 1, x, y, args);
}

/* Shadowed. */
// FUNCTION: SHANDALAR 0x0057b4d0
int Sid_DrawTextS(SidContext *ctx, int color, int x, int y, ...)
{
    va_list args;

    va_start(args, y);
    return Sid_DrawTextCore(ctx, color, 1, 0, 0, 0, x, y, args);
}

/* Shadowed, horizontally centred. */
// FUNCTION: SHANDALAR 0x0057b500
int Sid_DrawTextSH(SidContext *ctx, int color, int x, int y, ...)
{
    va_list args;

    va_start(args, y);
    return Sid_DrawTextCore(ctx, color, 1, 0, 1, 0, x, y, args);
}

/* Shadowed, vertically centred. */
// FUNCTION: SHANDALAR 0x0057b530
int Sid_DrawTextSV(SidContext *ctx, int color, int x, int y, ...)
{
    va_list args;

    va_start(args, y);
    return Sid_DrawTextCore(ctx, color, 1, 0, 0, 1, x, y, args);
}

/* Shadowed, centred both ways. */
// FUNCTION: SHANDALAR 0x0057b560
int Sid_DrawTextSHV(SidContext *ctx, int color, int x, int y, ...)
{
    va_list args;

    va_start(args, y);
    return Sid_DrawTextCore(ctx, color, 1, 0, 1, 1, x, y, args);
}

/* 640x480 coords, centred both ways. */
// FUNCTION: SHANDALAR 0x0057b590
int Sid_DrawTextRHV(SidContext *ctx, int color, int x, int y, ...)
{
    va_list args;

    va_start(args, y);
    return Sid_DrawTextCore(ctx, color, 0, 1, 1, 1, x, y, args);
}

/* Shadowed, 640x480 coords, horizontally centred. */
// FUNCTION: SHANDALAR 0x0057b5c0
int Sid_DrawTextSRH(SidContext *ctx, int color, int x, int y, ...)
{
    va_list args;

    va_start(args, y);
    return Sid_DrawTextCore(ctx, color, 1, 1, 1, 0, x, y, args);
}

/* Shadowed, 640x480 coords, vertically centred. */
// FUNCTION: SHANDALAR 0x0057b5f0
int Sid_DrawTextSRV(SidContext *ctx, int color, int x, int y, ...)
{
    va_list args;

    va_start(args, y);
    return Sid_DrawTextCore(ctx, color, 1, 1, 0, 1, x, y, args);
}

/* Shadowed, 640x480 coords, centred both ways. */
// FUNCTION: SHANDALAR 0x0057b620
// FUNCTION: FACEMAKER 0x00409e40
int Sid_DrawTextSRHV(SidContext *ctx, int color, int x, int y, ...)
{
    va_list args;

    va_start(args, y);
    return Sid_DrawTextCore(ctx, color, 1, 1, 1, 1, x, y, args);
}
