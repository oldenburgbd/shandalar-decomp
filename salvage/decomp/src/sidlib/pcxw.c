/* sidlib/Pcxw.c - the PCX reader.
 *
 * Identified by its assert at Pcxw.c:173, "%s Not a pcx file", which fires when
 * the first header byte is not 0x0A. Only Fileio.c's image loader calls into
 * this module.
 */

#include <stdio.h>
#include <string.h>

#include "sidlib/sidcore.h"
#include "sidlib/sidfileio.h"

/* Reads the 128-byte header, publishes the image extent, and -- when the caller
 * wants one -- loads the palette and rewinds to the pixel data. Always returns
 * 1, including down the "not in a recognizable format" path, so the return
 * value carries no information and no caller reads it.
 *
 * Only two of the many PCX variants are handled: 8 bits on one plane, whose
 * 768-byte VGA palette sits in the last 768 bytes of the file, and 1 bit on
 * four planes, whose 16-entry EGA palette lives inside the header. Anything
 * else asserts.
 *
 * The four-plane path does not work -- it seeks to SEEK_END + 16 rather than
 * SEEK_SET + 16, so all sixteen of its reads happen past the end of the file.
 * See docs/quirks.md Q-018. Reproduced exactly. */
// FUNCTION: SHANDALAR 0x0057d850
int Pcx_ReadHeader(unsigned char *paletteOut)
{
    int i;

    fread(g_sidPcxHeader, 0x80, 1, g_sidPcxFile);
    // STRING: SHANDALAR 0x005a5d64
    Sid_Assert(g_sidPcxHeader[0] == 0x0a, g_sidPcxwFile, 173, "%s Not a pcx file\r\n",
               g_sidPcxName);
    // STRING: SHANDALAR 0x005a5d44
    Sid_Assert(g_sidPcxHeader[1] == 5, g_sidPcxwFile, 174, "%s Not a version 5 pcx file\r\n",
               g_sidPcxName);

    /* Zero-extended, not sign-extended: the original pairs each word load with
     * an xor of the full register. */
    g_sidImageWidth = *(unsigned short *)(g_sidPcxHeader + SID_PCX_XMAX)
                    - *(unsigned short *)(g_sidPcxHeader + SID_PCX_XMIN) + 1;
    g_sidImageHeight = *(unsigned short *)(g_sidPcxHeader + SID_PCX_YMAX)
                     - *(unsigned short *)(g_sidPcxHeader + SID_PCX_YMIN) + 1;

    if (paletteOut == NULL) {
        return 1;
    }

    if (g_sidPcxHeader[SID_PCX_NPLANES] == 1 && g_sidPcxHeader[SID_PCX_BPP] == 8) {
        fseek(g_sidPcxFile, -768L, SEEK_END);
        fread(paletteOut, 1, 768, g_sidPcxFile);
        fseek(g_sidPcxFile, 128L, SEEK_SET);
        return 1;
    }

    if (g_sidPcxHeader[SID_PCX_NPLANES] == 4 && g_sidPcxHeader[SID_PCX_BPP] == 1) {
        fseek(g_sidPcxFile, 16L, SEEK_END);
        i = 16;
        do {
            fread(paletteOut, 1, 3, g_sidPcxFile);
            paletteOut += 4;
        } while (--i != 0);
        fseek(g_sidPcxFile, 128L, SEEK_SET);
        return 1;
    }

    // STRING: SHANDALAR 0x005a5d1c
    Sid_Assert(0, g_sidPcxwFile, 212, "%s is not in a recognizable format\r\n", g_sidPcxName);
    return 1;
}

/* Expands one RLE-encoded scanline into `dst`.
 *
 * Standard PCX run-length encoding: a byte with both top bits set is a count in
 * its low six bits and the next byte is the value. Note the count is only
 * honoured above 1 -- a run of 0 or 1 falls through and stores the value byte
 * once, which for count 0 writes a pixel the encoder did not ask for and
 * consumes one of the row's remaining bytes. Reproduced; see docs/quirks.md
 * Q-017.
 *
 * The row length comes from the header rather than from the caller.
 *
 * The unsigned-vs-signed question was measured, not assumed: the original
 * compares the count with JBE, so an unsigned type looks right, but declaring
 * it unsigned scores 55.45%% against 56.00%% for int. Reverted -- the number is
 * the referee.
 *
 * The row length comes from the header rather than from the caller, so this is
 * only meaningful between Pcx_ReadHeader and the end of the pixel data. Always
 * returns 1. */
// FUNCTION: SHANDALAR 0x0057da00
int Pcx_DecodeRow(unsigned char *dst)
{
    int remaining;
    int count;
    int b;

    remaining = *(short *)(g_sidPcxHeader + SID_PCX_BYTES_PER_LINE);
    while (remaining > 0) {
        b = fgetc(g_sidPcxFile);
        if ((b & 0xc0) == 0xc0) {
            count = b & 0x3f;
            b = fgetc(g_sidPcxFile);
            if (count > 1) {
                memset(dst, b, count);
                dst += count;
                remaining -= count;
                continue;
            }
        }
        *dst++ = (unsigned char)b;
        remaining--;
    }
    return 1;
}
