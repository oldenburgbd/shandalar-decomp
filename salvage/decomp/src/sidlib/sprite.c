/* sidlib/sprite.c — the sprite build arena and .spr container loading.
 *
 * Assert line numbers (163, 197) are the originals, recovered from the
 * compiled-in assert sites, so they also pin where the loaders sat in the
 * original file.
 *
 * The four functions at 0x0057b650..0x0057b6c0 carry no assert and so no file
 * evidence; they are placed here because they sit in the gap between text.c's
 * last function (0x0057b620) and this file's first *proven* one (0x0057b710),
 * and because what they do is sprite-building. That attribution is inference,
 * not proof. It costs nothing to be wrong about: text.c and sprite.c are both
 * built /O2, so the generated code is identical either way.
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
    /* Terminate the frame chain the way Spr_LoadFile expects to find it. */
    *(long *)g_sprArenaCur = -1L;

    /* _expand, not realloc, and that matters: the frame builder has already
     * handed interior pointers into this block out to its callers, so the
     * block must shrink *in place*. realloc would be free to move it and
     * invalidate every one of them.
     *
     * The flip side is that _expand returns NULL on failure, and its result is
     * stored back unconditionally — a failed shrink silently nulls the base
     * pointer. See docs/quirks.md Q-011.
     *
     * The +16 is the terminator plus padding; the original has no symbolic
     * constant here that survives.
     *
     * Reading g_sprArenaBase twice here looks wasteful and hoisting it into a
     * local was tried: it scores *worse* (84.6% -> 50.0%), because the local
     * makes MSVC evaluate the base before the cursor and compute the size in
     * the other register, reordering four instructions. The redundant-looking
     * form is the one that matches, and with /G5 it matches exactly. */
    g_sprArenaBase = _expand(g_sprArenaBase,
                             (char *)g_sprArenaCur - (char *)g_sprArenaBase + 16);
}

// FUNCTION: SHANDALAR 0x0057b6a0
// FUNCTION: FACEMAKER 0x00409ec0
void Sid_Free(void *block)
{
    /* This sat at 80% with an unexplained `push ebx`/`pop ebx` pair around the
     * call, and the pair was read as evidence of a missing source construct.
     * It was not: the pushes are /G5 Pentium scheduling padding. With the right
     * processor flag this is byte-exact. */
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

/* Captures a rectangle of a page and appends it as one frame. See sidsprite.h
 * for the format and for what the guard scans are.
 *
 * Two things here look like transcription errors and are not. The horizontal
 * guard scan clamps its limit to zero when `width == -2` even though
 * `width + 2` is already zero there -- the original tests it anyway -- and the
 * vertical scan, which is otherwise the same code, does not. The asymmetry is
 * in the binary.
 *
 * Nothing bounds `row` against SPR_ROW_MAX, and nothing bounds the arena. See
 * docs/quirks.md Q-010. */
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

    /* The guard coordinates are recomputed inside each test rather than hoisted
     * into variables. Hoisting them is what the original's register usage looks
     * like it wants -- it keeps y+height+1 live in a register across the whole
     * block -- and measuring says otherwise: 33.64% hoisted against 49.82%
     * here. Do not try it again. */
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

    /* Skip rows that are entirely transparent. The test is "the scan reached
     * the end of the row without finding a non-zero pixel". */
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
    /* Drop trailing all-empty rows, shrinking the encoded-row count to match.
     * This is why encodedRows can be less than height - emptyLines. */
    while (*((char *)g_sprArenaCur - 1) == -1) {
        frame->encodedRows--;
        g_sprArenaCur = (char *)g_sprArenaCur - 1;
    }

    /* Align to four, in two steps, which is how the original writes it. */
    if (((unsigned int)g_sprArenaCur & 1) != 0) {
        g_sprArenaCur = (char *)g_sprArenaCur + 1;
    }
    if (((unsigned int)g_sprArenaCur & 2) != 0) {
        g_sprArenaCur = (char *)g_sprArenaCur + 2;
    }
    *(long *)g_sprArenaCur = -1;
    frame->size = (char *)g_sprArenaCur - (char *)frame;
}

/* Walks a frame's run-length rows to find its horizontal extent. This is the
 * function that establishes the row encoding and names the +0x0e field: it is
 * the count of encoded rows, and it is read as a *signed* short (movsx), which
 * is why the guard below is `> 0` rather than `!= 0`.
 *
 * Both outputs are written before the NULL check, so a NULL frame still leaves
 * a defined — and deliberately inverted — range. */
// FUNCTION: SHANDALAR 0x0057c450
// FUNCTION: FACEMAKER 0x0040ad50
void Spr_GetFrameExtent(const SprFrame *frame, int *minX, int *maxX)
{
    /* `rows` before `p` is load-bearing: 46.67% -> 68.89%. MSVC 4.2 hands out
     * registers to locals in DECLARATION order, so swapping these two swaps
     * which of esi/edi each gets, and with `p` on edi the whole run-decoding
     * loop goes byte-exact. What is left is a rotation among rows/minX/maxX
     * over eax/ecx/esi; the original spends eax on `rows`, which would need a
     * local to outrank a parameter. `register` does not do it -- measured a
     * no-op both ways. The instruction sequence already matches exactly. */
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

            /* Read back through the pointers each time rather than tracking
             * running minima in locals — the original reloads both. */
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

    /* Frames are chained by their leading size field; 0xFFFFFFFF terminates.
     * No bounds check in the original — a truncated file walks off the end.
     * The size is read into a local and re-read at the bottom of the loop,
     * which is the shape the decompiled original has. */
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

    /* Zeroed here rather than beside the loop, matching Spr_LoadFile. This is
     * worth 35 points on its own (42.3% -> 77.5%). Initialising count up here
     * keeps it live across fopen/fread/fclose, which forces MSVC to give it a
     * fourth callee-saved register; initialising it at the loop frees that
     * register and permutes the allocation of every other local, so nearly
     * every instruction in the function reads as a mismatch. */
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

    /* Same loop as Spr_LoadFile with the frame cap added as a second condition.
     * The cap is written `maxFrames > count`, not `count < maxFrames`, because
     * the original puts maxFrames in the left operand and exits on jle. */
    frameSize = *(long *)buf;
    while (frameSize != -1L && maxFrames > count) {
        *frames++ = buf;
        count++;
        buf += *(long *)buf;
        frameSize = *(long *)buf;
    }

    return count;
}

/* The scaled sprite blitter, and the largest function in sidlib.
 *
 * Scaling is 16.16 fixed point through two memoised 1024-entry tables: one maps
 * a destination column to its source column, the other maps a source column to
 * the first destination column that samples it. They are rebuilt only when any
 * of the four cached dimensions changes, so a run of same-size draws pays for
 * them once.
 *
 * The row loop is what settles what SprFrame's +0x0e field means: it runs while
 * the source row is below `emptyLines + encodedRows`, so encodedRows is a count
 * of rows and a frame's content height is not necessarily its `height`.
 *
 * Three destination paths, chosen per row:
 *   page 0, escaped run  - build a scanline in a local buffer, one Sid_PutLine
 *   page 0, normal run   - Sid_SetPixel per non-zero pixel
 *   any other page       - write straight into the page's bits
 * Colour index 0 is transparent on the two per-pixel paths but NOT on the
 * Sid_PutLine path, which copies the run wholesale.
 */
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

    /* The original stores 0 into this slot before the guard, so it is written
     * here too. Measured: neutral, 39.76% either way. It is kept in the
     * original's position rather than beside the loop because that is where the
     * original puts it, not because it moved the number. */
    row = 0;
    page = ctx->page;
    if (frame == NULL || x > ctx->clipRight || y > ctx->clipBottom) {
        return;
    }

    p = g_sidPages[page];
    srcW = frame->width;
    srcH = frame->height;

    /* Rebuild the scale tables only when the geometry actually changed. The
     * comparison order is the original's, which is not symmetric. */
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

        /* Shrinking: source columns that no destination column sampled inherit
         * their left neighbour. Note the first iteration reads one int BEFORE
         * the table -- see docs/quirks.md Q-012. */
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

    /* Advance past the frame's leading transparent rows in destination space. */
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

        /* When the vertical step crosses more than one source row, skip the
         * rows in between. When it crosses none, rp is deliberately left where
         * it is so the same source row is drawn again -- that is how upscaling
         * repeats rows. */
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
