/* sidlib/sprite.c — the sprite build arena and .spr container loading.
 *
 * Assert line numbers (163, 197) are the originals. The four functions at
 * 0x0057b650..0x0057b6c0 carry no assert evidence; they are placed here by
 * adjacency (the gap between text.c's last function and this file's first
 * proven one) and by subject matter. Both TUs are /O2, so the attribution
 * cannot change the generated code.
 */

#include <io.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include "sidlib/sidsprite.h"

// FUNCTION: SHANDALAR 0x0057b650
// FUNCTION: FACEMAKER 0x00409e70
void Spr_ArenaBegin(void)
{
    g_sprArenaBase = malloc(SPR_ARENA_BYTES);
    g_sprArenaCur = g_sprArenaBase;
}

// FUNCTION: SHANDALAR 0x0057b670
void Spr_ArenaEnd(void)
{
    /* Terminate the frame chain the way Spr_LoadFile expects. */
    *(long *)g_sprArenaCur = -1L;

    /* _expand, not realloc: interior pointers into this block are already
     * out, so it must shrink in place. Its NULL-on-failure result is stored
     * back unconditionally, as the original does. Reading g_sprArenaBase
     * twice is load-bearing — hoisting it into a local measured worse. */
    g_sprArenaBase = _expand(g_sprArenaBase,
                             (char *)g_sprArenaCur - (char *)g_sprArenaBase + 16);
}

// FUNCTION: SHANDALAR 0x0057b6a0
// FUNCTION: FACEMAKER 0x00409ec0
void Sid_Free(void *block)
{
    /* The push/pop pair around the call is /G5 scheduling padding, not a
     * missing construct. */
    free(block);
}

// FUNCTION: SHANDALAR 0x0057b6c0
// FUNCTION: FACEMAKER 0x00409ee0
unsigned int Spr_DumpToFile(void *block, const char *path)
{
    unsigned int size;
    FILE *fp;

    size = _msize(block);
    // STRING: SHANDALAR 0x005a17cc
    // STRING: FACEMAKER 0x00410294
    fp = fopen(path, "wb");
    fwrite(block, 1, size, fp);
    fclose(fp);

    return size;
}

/* Captures a rectangle of a page and appends it as one frame. The horizontal
 * guard scan clamps its limit to zero when width == -2 even though width + 2
 * is already zero there; the vertical scan does not — the asymmetry is the
 * binary's. Nothing bounds `row` or the arena. */
// FUNCTION: SHANDALAR 0x0057b840
void Spr_AppendFrame(int page, int x, int y, int width, int height)
{
    unsigned char row[SPR_ROW_MAX];
    SprFrame *frame;
    unsigned char *out;
    unsigned char *p;
    unsigned char *q;
    int emptyLines;
    int scan;
    int borderRunX;
    int borderRunY;
    int step;
    int limit;
    int count;
    unsigned int ref;
    int n;
    int lead;
    unsigned int len;

    emptyLines = 0;

    /* Guard coordinates recomputed inside each test, not hoisted — hoisting
     * measured worse despite looking like what the registers want. */
    if (x - 1 >= 0 && y + height + 1 >= 0) {
        step = (width != -2);
        ref = Sid_GetPixelPage(page, x - 1, y + height + 1);
        limit = width + 2;
        if (width == -2) {
            limit = 0;
        }
        count = 0;
        scan = x - 1;
        if (limit > 0) {
            do {
                if (Sid_GetPixelPage(page, scan, y + height + 1) != ref) {
                    break;
                }
                count++;
                scan += step;
            } while (count < limit);
        }
        borderRunX = -1;
        if (limit != count) {
            borderRunX = count;
        }
    } else {
        borderRunX = -1;
    }

    if (x - 1 >= 0 && y - 1 >= 0) {
        step = (height != -2);
        ref = Sid_GetPixelPage(page, x - 1, y - 1);
        limit = height + 2;
        count = 0;
        scan = y - 1;
        if (limit > 0) {
            do {
                if (Sid_GetPixelPage(page, x - 1, scan) != ref) {
                    break;
                }
                count++;
                scan += step;
            } while (count < limit);
        }
        borderRunY = -1;
        if (limit != count) {
            borderRunY = count;
        }
    } else {
        borderRunY = -1;
    }

    frame = (SprFrame *)g_sprArenaCur;
    out = (unsigned char *)g_sprArenaCur + sizeof(SprFrame);

    /* Skip rows that are entirely transparent. */
    do {
        Sid_GetLine(row, page, x, y + emptyLines, width);
        p = row;
        n = width;
        while (*p == 0 && n != 0) {
            p++;
            n--;
        }
    } while (p - width == row && ++emptyLines < height);

    frame->width = (short)width;
    frame->height = (short)height;
    frame->borderRunX = (short)borderRunX;
    frame->borderRunY = (short)borderRunY;
    frame->emptyLines = (short)emptyLines;
    frame->encodedRows = (short)height - (short)emptyLines;

    for (; emptyLines < height; emptyLines++) {
        p = row;
        n = width;
        while (*p == 0 && n != 0) {
            p++;
            n--;
        }
        lead = p - row;
        if (lead == width) {
            *out++ = 0xff;
        } else {
            *out++ = (unsigned char)lead;
            q = &row[width - 1];
            n = width;
            while (*q == 0 && n != 0) {
                q--;
                n--;
            }
            len = (unsigned int)(q - &row[width - 1] + (width - lead));
            if (memchr(row + lead, 0, len) == NULL) {
                *out++ = 0xfe;
            }
            *out++ = (unsigned char)len;
            memcpy(out, row + lead, len);
            out += len;
        }
        Sid_GetLine(row, page, x, y + emptyLines + 1, width);
    }

    g_sprArenaCur = out;
    /* Drop trailing all-empty rows, shrinking encodedRows to match. */
    while (*((char *)g_sprArenaCur - 1) == -1) {
        frame->encodedRows--;
        g_sprArenaCur = (char *)g_sprArenaCur - 1;
    }

    /* Align to four, in two steps, as the original writes it. */
    if (((unsigned int)g_sprArenaCur & 1) != 0) {
        g_sprArenaCur = (char *)g_sprArenaCur + 1;
    }
    if (((unsigned int)g_sprArenaCur & 2) != 0) {
        g_sprArenaCur = (char *)g_sprArenaCur + 2;
    }
    *(long *)g_sprArenaCur = -1;
    frame->size = (char *)g_sprArenaCur - (char *)frame;
}

/* Walks a frame's run-length rows to find its horizontal extent. encodedRows
 * is read as a signed short (movsx), hence the `> 0` guard. Both outputs are
 * written before the NULL check. */
// FUNCTION: SHANDALAR 0x0057c450
// FUNCTION: FACEMAKER 0x0040ad50
void Spr_GetFrameExtent(const SprFrame *frame, int *minX, int *maxX)
{
    /* `rows` before `p` is load-bearing: MSVC 4.2 assigns registers to locals
     * in declaration order, and `p` on edi makes the decode loop byte-exact. */
    int rows;
    const unsigned char *p;
    int x;
    int len;

    *minX = 0x7fffffff;
    *maxX = 0;

    if (frame == NULL) {
        return;
    }

    rows = frame->encodedRows;
    p = (const unsigned char *)frame + 16;

    while (rows > 0) {
        x = *p++;
        if (x != 0xff) {
            len = *p++;
            if (len == 0xfe) {
                len = *p++;
            }

            /* Reload through the pointers each time — the original does. */
            if (*minX > x) {
                *minX = x;
            }
            x += len;
            if (*maxX < x) {
                *maxX = x;
            }
            p += len;
        }
        rows--;
    }
}

// FUNCTION: SHANDALAR 0x0057b710
int Spr_LoadFile(void **frames, const char *path)
{
    FILE *fp;
    unsigned char *buf;
    size_t size;
    long frameSize;
    int count;

    count = 0;
    // STRING: SHANDALAR 0x005a17a4
    // STRING: FACEMAKER 0x0041026c
    fp = fopen(path, "rb");
    // STRING: SHANDALAR 0x005a17f4
    Sid_Assert((int)fp, SIDLIB_SPRITE_C, 163, "Could not open Sprite File %s\n", path);

    size = _filelength(_fileno(fp));
    buf = (unsigned char *)malloc(size);
    fread(buf, 1, size, fp);
    fclose(fp);

    /* Frames chained by their leading size field; 0xFFFFFFFF terminates.
     * No bounds check. The size is read into a local and re-read at the
     * bottom of the loop, matching the decompiled shape. */
    frameSize = *(long *)buf;
    while (frameSize != -1L) {
        *frames++ = buf;
        count++;
        buf += *(long *)buf;
        frameSize = *(long *)buf;
    }

    return count;
}

// FUNCTION: SHANDALAR 0x0057b7a0
int Spr_LoadFileMax(void **frames, const char *path, int maxFrames)
{
    FILE *fp;
    unsigned char *buf;
    size_t size;
    long frameSize;
    int count;

    /* Zeroed up here, matching Spr_LoadFile — keeping count live across
     * fopen/fread/fclose forces the fourth callee-saved register and is
     * worth ~35 points on its own. */
    count = 0;
    // STRING: SHANDALAR 0x005a17a4
    // STRING: FACEMAKER 0x0041026c
    fp = fopen(path, "rb");
    // STRING: SHANDALAR 0x005a17f4
    Sid_Assert((int)fp, SIDLIB_SPRITE_C, 197, "Could not open Sprite File %s\n", path);

    size = _filelength(_fileno(fp));
    buf = (unsigned char *)malloc(size);
    fread(buf, 1, size, fp);
    fclose(fp);

    /* The cap is written `maxFrames > count` because the original puts
     * maxFrames in the left operand and exits on jle. */
    frameSize = *(long *)buf;
    while (frameSize != -1L && maxFrames > count) {
        *frames++ = buf;
        count++;
        buf += *(long *)buf;
        frameSize = *(long *)buf;
    }

    return count;
}

/* The scaled sprite blitter. 16.16 fixed point through two memoised
 * 1024-entry tables (dst column -> src column, src column -> first dst
 * column), rebuilt only when any of the four cached dimensions changes.
 * The row loop runs while the source row is below emptyLines + encodedRows.
 * Three destination paths per row: page 0 escaped run -> local buffer plus
 * one Sid_PutLine; page 0 normal run -> Sid_SetPixel per non-zero pixel;
 * other pages -> direct writes to the page bits. Colour 0 is transparent
 * on the two per-pixel paths but not on the Sid_PutLine path. */
// FUNCTION: SHANDALAR 0x0057bfa0
// FUNCTION: FACEMAKER 0x0040a860
void Spr_DrawScaled(const SidContext *ctx, int x, int y, int dstW, int dstH,
                    const SprFrame *frame)
{
    SidPage *p;
    int page;
    int srcW;
    int srcH;
    int i;
    int *e;
    const unsigned char *rp;
    const unsigned char *px;
    int emptyLines;
    int scan;
    int rows;
    int stride;
    int row;
    unsigned char *dst;
    int srcY;
    int nextSrcY;
    int skip;
    unsigned int startX;
    unsigned int runLen;
    int escaped;
    int dstY;
    int c0;
    int c1;
    unsigned char line[1024];

    /* Stored before the guard because that is where the original stores it;
     * measured neutral. */
    row = 0;
    page = ctx->page;
    if (frame == NULL || x > ctx->clipRight || y > ctx->clipBottom) {
        return;
    }

    p = g_sidPages[page];
    srcW = frame->width;
    srcH = frame->height;

    /* Rebuild the scale tables only when the geometry changed; the comparison
     * order is the original's, which is not symmetric. */
    if (g_sprScaleDstW != dstW || dstH != g_sprScaleDstH ||
        srcW != g_sprScaleSrcW || srcH != g_sprScaleSrcH) {
        g_sprScaleXStep = (srcW << 16) / dstW;
        g_sprScaleYStep = (srcH << 16) / dstH;

        e = g_sprScaleSrcToDst;
        for (i = 1024; i != 0; i--) {
            *e++ = -1;
        }

        i = 0;
        g_sprScaleXAcc = 0;
        if (dstW + 2 >= 0) {
            do {
                int sx = g_sprScaleXAcc >> 16;
                g_sprScaleDstToSrc[i] = sx;
                if (g_sprScaleSrcToDst[sx] == -1) {
                    g_sprScaleSrcToDst[sx] = i;
                }
                i++;
                g_sprScaleXAcc += g_sprScaleXStep;
            } while (i <= dstW + 2);
        }

        /* Shrinking: unsampled source columns inherit their left neighbour.
         * The first iteration reads one int before the table. */
        if (dstW < srcW && srcW >= 0) {
            e = g_sprScaleSrcToDst;
            i = srcW + 1;
            do {
                if (*e == -1) {
                    *e = e[-1];
                }
                e++;
                i--;
            } while (i != 0);
        }

        g_sprScaleDstW = dstW;
        g_sprScaleDstH = dstH;
        g_sprScaleSrcH = srcH;
        g_sprScaleSrcW = srcW;
    }

    if (x < ctx->clipLeft) {
        g_sprScaleLeft = ctx->clipLeft - x;
    } else {
        g_sprScaleLeft = 0;
    }
    if (ctx->clipRight < x + dstW) {
        g_sprScaleRight = ctx->clipRight - x;
    } else {
        g_sprScaleRight = dstW;
    }

    /* Advance past the frame's leading transparent rows in destination
     * space. */
    emptyLines = frame->emptyLines;
    g_sprScaleYAcc = 0;
    if (emptyLines > 0) {
        do {
            g_sprScaleYAcc += g_sprScaleYStep;
            y++;
        } while (g_sprScaleYAcc >> 16 < emptyLines);
    }

    rp = (const unsigned char *)frame + 16;
    rows = frame->encodedRows;
    stride = p->rowPadding + p->width;
    dst = (unsigned char *)p->pBits + y * stride + x;

    while (g_sprScaleYAcc >> 16 < rows + emptyLines) {
        srcY = g_sprScaleYAcc >> 16;
        px = rp + 1;
        nextSrcY = (g_sprScaleYStep + g_sprScaleYAcc) >> 16;
        startX = *rp;

        if (startX != 0xff) {
            runLen = *px;
            px = rp + 2;
            escaped = (runLen == 0xfe);
            if (escaped) {
                px = rp + 3;
                runLen = rp[2];
            }

            dstY = row + y;
            if (ctx->clipTop <= dstY) {
                if (ctx->clipBottom < dstY) {
                    return;
                }

                c0 = g_sprScaleSrcToDst[startX];
                if (c0 <= g_sprScaleLeft) {
                    c0 = g_sprScaleLeft;
                }
                c1 = g_sprScaleSrcToDst[runLen + startX];
                if (g_sprScaleRight <= c1) {
                    c1 = g_sprScaleRight;
                }
                if (g_sprScaleDstToSrc[c0] - (int)startX < 0) {
                    c0++;
                }

                if (page == 0) {
                    if (escaped) {
                        for (i = c0; i < c1; i++) {
                            line[i] = px[g_sprScaleDstToSrc[i] - startX];
                        }
                        Sid_PutLine(line + c0, 0, x + c0, dstY, c1 - c0);
                    } else {
                        for (; c0 < c1; c0++) {
                            if (px[g_sprScaleDstToSrc[c0] - startX] != 0) {
                                Sid_SetPixel(ctx, x + c0, dstY,
                                             px[g_sprScaleDstToSrc[c0] - startX]);
                            }
                        }
                    }
                } else {
                    for (; c0 < c1; c0++) {
                        if (px[g_sprScaleDstToSrc[c0] - startX] != 0) {
                            dst[c0] = px[g_sprScaleDstToSrc[c0] - startX];
                        }
                    }
                }
            }
            px += runLen;
        }

        /* Skip intermediate source rows when the vertical step crosses more
         * than one; when it crosses none, rp stays put so the same source
         * row repeats — that is how upscaling repeats rows. */
        if (nextSrcY != srcY) {
            skip = nextSrcY - srcY;
            rp = px;
            if (skip != 1) {
                i = skip - 2;
                do {
                    rp = px + 1;
                    if (*px != 0xff) {
                        runLen = *rp;
                        rp = px + 2;
                        if (runLen == 0xfe) {
                            runLen = *rp;
                            rp = px + 3;
                        }
                        rp += runLen;
                    }
                    px = rp;
                } while (i-- != 0);
            }
        }

        row++;
        g_sprScaleYAcc += g_sprScaleYStep;
        dst += stride;
    }
}
