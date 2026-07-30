/* sidlib/text.c — formatted text output.
 *
 * Reconstructed from Shandalar.exe. The eleven entry points below are pure
 * forwarders into Sid_DrawTextCore (0x0057b230), each pinning a different
 * combination of its four layout flags; the game never uses the other five
 * combinations. Ordered by address, which is also the order they appear in the
 * original object file.
 *
 * `va_start(args, y)` is deliberate: the format string is unnamed and arrives
 * as the first variadic argument, so the list has to start immediately after y
 * — which is exactly the stack address the original passes.
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "sidlib/sidtext.h"

/* Loads a font file: a 16-bit count, then that many 16-bit file offsets, then
 * one record per offset. Slots are 1-based — offsets[0] is never written and
 * never read, and the record loader is called with 1..count — so the font table
 * wastes entry 0. Returns the count, and also publishes it to g_sidFontCount.
 *
 * This is where SID_MAX_FONTS stops being a placeholder. The header has carried
 * 16 as "large enough to link" with a note that the real bound was not
 * established; the assert here tests `count < 16` and passes 16 to its own
 * message, so 16 is the original's own limit.
 *
 * Both counts are read as 16 bits into a zeroed 32-bit slot, which is why each
 * offset is cleared immediately before its fread rather than the array being
 * cleared once. A file offset therefore cannot exceed 65535.
 *
 * The condition on the first assert is a *boolean* — the original emits
 * cmp/sbb/inc to normalise the FILE* to 0 or 1 — unlike Spr_LoadFile, which
 * pushes the pointer raw. Per-site, as always. */
// FUNCTION: SHANDALAR 0x0057a4e0
// FUNCTION: FACEMAKER 0x00408710
int Sid_LoadFontFile(const char *path)
{
    /* Declaring `i` earlier was tried: 79.56% -> 78.45%. What remains is a
     * permutation of ebx/edi/ebp among the loop counter, the walking pointer
     * and the cached fseek import -- the original keeps the counter in edi
     * across both loops where we split it. Every instruction is present and in
     * order; this is register naming, not structure. */
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

/* Reads one font record from wherever the stream currently sits, and builds the
 * two GDI bitmaps Sid_DrawString blits from.
 *
 * The stream is positioned by the caller *past* the record, so every read here
 * seeks backwards first: -8 for the eight-byte header, then another
 * -(8 + charCount) for the width table. Only fonts with no fixed-width override
 * carry a width table, and it is stored indexed from `firstChar`, so it is read
 * straight into &charWidth[firstChar] rather than at the start of the array.
 *
 * The glyph strip is one long row-major 1-bit image, charCount cells wide,
 * whose row pitch is rounded up to even -- which is what `oddPad` records. Rows
 * are read one at a time because the file has no padding and the buffer does.
 *
 * The buffer is over-allocated by 10000 bytes. Nothing uses the slack.
 *
 * Both bitmaps come from the same bits: the glyph bitmap from them as read, and
 * the mask from their complement, which is why the buffer is inverted, used,
 * and inverted back.
 *
 * Returns 0, which its one caller ignores. */
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
    /* The eight-byte header is addressed through its own pointer, not off the
     * record base -- the original reads firstChar as *hdr while reading
     * lastChar as rec->lastChar, so the two are distinct variables in the
     * source. */
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

/* Returns the font's base height. The compiler strength-reduces the 684-byte
 * record stride into `index*19`, `<<2`, then `*9` via a pair of LEAs; writing
 * the plain struct index reproduces that. */
// FUNCTION: SHANDALAR 0x0057ae30
// FUNCTION: FACEMAKER 0x00409180
int Sid_GetFontHeight(int font)
{
    return g_sidFonts[font].height;
}

/* Base height plus one of two extras. Which extra is chosen turns on the
 * pointer at +0x14 being non-NULL — the same field the string drawer at
 * 0x0057ae50 tests — so the +0x224 value belongs to whatever that pointer
 * refers to and +0x02 is the fallback. Note the two are not the same width:
 * +0x224 is read as a dword, +0x02 as a byte. */
// FUNCTION: SHANDALAR 0x0057adf0
// FUNCTION: FACEMAKER 0x00409140
int Sid_GetFontLineSpacing(int font)
{
    if (g_sidFonts[font].scaled != NULL) {
        return g_sidFonts[font].height + g_sidFonts[font].scaledExtra;
    }
    /* The original loads height into AL and extra into DL; we emit the two
     * the other way round, and writing the addition as `extra + height`
     * changes nothing -- measured, identical score. MSVC canonicalises the
     * operand order of `+` between two byte field loads, so this last 10.53%
     * is not reachable from the source expression. Confirmed a second time at
     * the copy of this expression inside Sid_DrawTextCore, which diffs the same
     * way and is equally immovable. */
    return g_sidFonts[font].height + g_sidFonts[font].extra;
}

/* Releases a loaded font slot. The `scaled` pointer doubles as the "this slot
 * is in use" flag, which is why it is tested rather than hFont. Note the return
 * value: the early exit emits `xor eax,eax`, so the original really does return
 * 0 there rather than being a void function whose EAX happens to be clear. */
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

/* Width of the widest line in a string, in the context's font.
 *
 * Two completely separate implementations, chosen by whether the font slot has
 * a `scaled` pointer. With one, the string is measured through GDI a line at a
 * time and each line's width gets +2. Without, the widths are summed out of the
 * font's own per-character table.
 *
 * The bitmap path re-tests `scaled` per character and has a whole GDI
 * per-character branch behind it — using GetCharABCWidthsA on a screen DC it
 * fetches and releases for every character. That branch is unreachable: it is
 * inside the arm the outer test already proved NULL. Reproduced because it is
 * in the binary, and it is the only place in sidlib that names
 * GetCharABCWidthsA, so it is worth keeping visible.
 *
 * The newline handling writes a NUL over the '\n' and puts it back afterwards,
 * so the caller's string is temporarily modified — which is why the parameter
 * cannot be const. */
// FUNCTION: SHANDALAR 0x0057aae0
int Sid_MeasureText(SidContext *ctx, char *s)
{
    int cur;
    int maxw;
    /* One twelve-byte slot serves both measurements. The original's frame is
     * 0x14 -- two ints plus twelve bytes -- and two separate locals make it
     * 0x1c, so the two out-parameters share storage in the source rather than
     * MSVC having overlapped them. */
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
            /* Signed index: the original sign-extends the character before
             * indexing, so anything above 0x7f reads backwards out of this
             * font's table and into the previous slot's. See quirks Q-019. */
            cur += g_sidFonts[ctx->font].spacing + g_sidFonts[ctx->font].charWidth[*p];
        }
        p++;
    }

    if (maxw <= cur) {
        maxw = cur;
    }
    return maxw;
}

/* Draws one line of text at (x, y). No newline handling — Sid_DrawTextCore has
 * already split on those.
 *
 * Two implementations again, on the same `scaled` test. The GDI one is a single
 * TextOutA. The bitmap one walks the string twice: once blitting the mask with
 * SRCAND to clear the glyph shape out of the destination, then again blitting
 * the glyphs with SRCPAINT to paint them in. Both bitmaps live in one memory DC
 * on the font record, and a character's cell is found by its offset from
 * `firstChar` times the cell pitch times eight.
 *
 * Three different return values: 0 when clipped out or given an empty string,
 * 1 from the GDI path, and the number of characters drawn from the bitmap
 * path. No caller reads any of them.
 *
 * The dead GDI-per-character branch from Sid_MeasureText appears here twice
 * more, once in each pass, and is unreachable for the same reason. */
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
 * `args` points at the format string rather than past it, so args[0] is the
 * format and args + 4 is the variadic tail. That is not a convention _vsnprintf
 * knows about -- it is what the eleven entry points happen to pass, and the
 * split here is what makes the call match.
 *
 * The line loop rewrites the buffer as it goes: each line's terminator is
 * replaced with '\0' to draw it and then written back as '\n'. On the last
 * line that byte was the string's own NUL, so the buffer comes out of the loop
 * with no terminator at all. Harmless -- nothing reads it afterwards and it
 * dies with the frame -- but it is why the walk is bounded by the line count
 * rather than by looking for the end of the string.
 *
 * The line spacing repeats Sid_GetFontLineSpacing's body instead of calling it.
 * MSVC 4.2 does not inline across functions without __inline, so the
 * duplication is in the original source too. */
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
    /* Reads buf[-1] when the formatted string is empty, because `p` never
     * advanced. One byte below a stack buffer; see docs/quirks.md Q-015. */
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

        /* `saved` again, not a second variable: the original reuses the one
         * stack slot, so a call with both a colour and a shadow restores the
         * colour rather than the caller's on the way out. See Q-016. */
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
