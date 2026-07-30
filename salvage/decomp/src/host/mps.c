/* host/mps.c -- a WORKING stand-in for the MPS_CODE section.
 *
 * NOT A DECOMPILATION. Nothing in this file carries a FUNCTION marker and
 * nothing in it is scored, deliberately.
 *
 * MPS_CODE is 0x4a8 bytes of hand-written assembly in the original image
 * (0x0098a000 in Shandalar.exe, 0x0042a000 in Facemaker.exe). It uses LODSW,
 * pushes registers around an indirect call and returns through LEAVE, so it was
 * never compiled from C and is explicitly not a reconstruction target -- see the
 * long note in include/sidlib/sidfileio.h. src/stubs.c therefore defines
 * Mps_Begin and Mps_Read as do-nothing stubs, which is right for the DLL reccmp
 * scores and useless for a program that has to actually draw.
 *
 * So this file supplies the same two entry points with real behaviour, for the
 * host EXE only. It is compiled solely into build\Shandalar.exe; the scored DLL
 * keeps the stubs (see the SHANDALAR_HOST guard in src/stubs.c). The algorithm
 * is the documented PICv3 pipeline from docs/formats/pic.md -- LZW, then RLE --
 * ported from Shandalar.Core.Formats.Pic, the C# oracle, which is verified
 * against the real install including Title.pic. It is a reimplementation of a
 * known format, not a guess at the assembly's instruction sequence, and it makes
 * no claim about how MPS_CODE is written internally.
 *
 * Contract, read off Sid_OpenFileCore (src/sidlib/fileio.c):
 *
 *   Mps_Begin(palette)   consumes blocks up to and including the image header,
 *                        publishes g_sidImageWidth/g_sidImageHeight, and
 *                        installs any palette block it passed on the way.
 *   Mps_Read(dst, count) hands back the next `count` decoded index bytes, which
 *                        the caller feeds to Sid_PutLine one row at a time.
 *
 * The real MPS_CODE streams, pulling bytes through g_mpsCursor and calling
 * g_mpsRefill when it runs dry. This decodes the whole image up front in
 * Mps_Begin and serves Mps_Read out of the result. The behaviour at the two
 * entry points is the same; the buffering is not, and the files are small enough
 * (the largest .pic in the install is 383 KB) that it does not matter.
 * Sid_OpenFileCore has already opened the file and left its descriptor in
 * g_sidFileFd, so that is what is read here.
 */

#include <io.h>
#include <stdlib.h>
#include <string.h>

#include "sidlib/sidcore.h"
#include "sidlib/sidfileio.h"

#define MPS_RLE_CONTROL 0x90
#define MPS_MAX_BITS    11
#define MPS_DICT_MAX    (1 << MPS_MAX_BITS)

/* The decoded image, and how far Mps_Read has got through it. */
static unsigned char *g_mpsImage;
static long g_mpsImageSize;
static long g_mpsImagePos;

/* Used when the caller passes the overloaded 1 rather than a block of its own.
 * On the PCX path Sid_OpenFileCore substitutes a stack block for that value
 * before it reaches a reader; on this path it does not (quirk Q-013), so the
 * reader needs somewhere of its own to put the palette. */
static SidPaletteBlock g_mpsOwnPalette;

/* ---------------------------------------------------------------------------
 * Stage 1: LZW
 *
 * Variable-width codes, LSB-first, 9 bits widening towards maxBits and then
 * resetting. Bit-unpacking and the dictionary walk are two separate passes in
 * the reference decoder and are kept that way: the dictionary is rebuilt from
 * scratch whenever it fills, and the outer loop's code-skip on restart is
 * load-bearing. Fusing the passes would quietly change which code gets skipped.
 * ------------------------------------------------------------------------ */

static int *mps_unpack_codes(const unsigned char *data, long len, int maxBits,
                             long *outCount)
{
    int *codes;
    long n = 0;
    long pos = 0;
    unsigned long usableBits = 0;
    int usableBitCount = 0;
    int indicatorLength = 1;
    int indicatorFlag = 0x001;
    long nextThreshold = 0x100;
    long counter = 0;

    /* Every code is at least 9 bits, so there are never more codes than input
     * bytes. One slot per byte is a safe bound. */
    codes = (int *)malloc((size_t)(len + 1) * sizeof(int));
    if (codes == NULL) {
        *outCount = 0;
        return NULL;
    }

    while (pos < len || usableBitCount >= 8 + indicatorLength) {
        while (usableBitCount < 8 + indicatorLength && pos < len) {
            usableBits |= (unsigned long)data[pos++] << usableBitCount;
            usableBitCount += 8;
        }
        if (usableBitCount < 8 + indicatorLength) {
            break;
        }

        while (usableBitCount >= 8 + indicatorLength) {
            int code = (int)(usableBits &
                             (unsigned long)(((indicatorFlag << 8) & 0xff00) | 0x00ff));

            usableBits >>= 8 + indicatorLength;
            usableBitCount -= 8 + indicatorLength;

            if (++counter == nextThreshold) {
                counter = 0;
                indicatorLength++;
                indicatorFlag = (indicatorFlag << 1) | 1;
                nextThreshold <<= 1;
                if (8 + indicatorLength > maxBits) {
                    counter = 0;
                    indicatorLength = 1;
                    indicatorFlag = 0x001;
                    nextThreshold = 0x100;
                }
            }
            codes[n++] = code;
        }
    }

    *outCount = n;
    return codes;
}

/* The dictionary, as parallel arrays rather than the reference's list of byte
 * strings: an entry is a prefix index plus one suffix byte, which is the usual
 * way to write LZW in C and avoids reallocating up to 2048 growing strings.
 * `first` and `len` are carried alongside so neither the "append the first byte
 * of" step nor the expansion needs a chain walk to find them. */
static int g_lzwPrefix[MPS_DICT_MAX];
static unsigned char g_lzwSuffix[MPS_DICT_MAX];
static unsigned char g_lzwFirst[MPS_DICT_MAX];
static int g_lzwLen[MPS_DICT_MAX];

/* Expands entry `k` into `dst`, which must have room for g_lzwLen[k] bytes. The
 * chain runs backwards from the entry to its root, so bytes are laid down from
 * the end of the span towards the start. */
static void mps_expand(int k, unsigned char *dst)
{
    int n = g_lzwLen[k];

    while (k >= 0) {
        dst[--n] = g_lzwSuffix[k];
        k = g_lzwPrefix[k];
    }
}

static unsigned char *mps_lzw(const unsigned char *data, long len, int maxBits,
                              long *outLen)
{
    int *codes;
    long codeCount = 0;
    unsigned char *out;
    long outCap;
    long outN = 0;
    unsigned char *entry;
    int dictCap;
    long i = 0;
    int c;

    if (maxBits < 9 || maxBits > MPS_MAX_BITS) {
        *outLen = 0;
        return NULL;
    }
    dictCap = 1 << maxBits;

    codes = mps_unpack_codes(data, len, maxBits, &codeCount);
    if (codes == NULL) {
        *outLen = 0;
        return NULL;
    }

    /* LZW output grows with the dictionary, so bound it generously and grow on
     * demand rather than trying to predict it. */
    outCap = codeCount * 4 + 1024;
    out = (unsigned char *)malloc((size_t)outCap);
    /* An entry can be at most as deep as the dictionary. */
    entry = (unsigned char *)malloc((size_t)dictCap + 2);
    if (out == NULL || entry == NULL) {
        free(codes);
        free(out);
        free(entry);
        *outLen = 0;
        return NULL;
    }

    while (i < codeCount) {
        int curPos;
        int prev;               /* dictionary index of the previous entry */

        /* Rebuild the dictionary: 0..255 are the literals, 256 is reserved and
         * never emitted, new entries start at 257. */
        for (c = 0; c < 256; c++) {
            g_lzwPrefix[c] = -1;
            g_lzwSuffix[c] = (unsigned char)c;
            g_lzwFirst[c] = (unsigned char)c;
            g_lzwLen[c] = 1;
        }
        g_lzwPrefix[256] = -1;
        g_lzwSuffix[256] = 0;
        g_lzwFirst[256] = 0;
        g_lzwLen[256] = 0;
        curPos = 0x101;

        /* The first code after a rebuild is taken as a bare literal and adds
         * nothing to the dictionary. */
        prev = codes[i] & 0xff;
        if (outN < outCap) {
            out[outN++] = (unsigned char)prev;
        }

        while (curPos < dictCap && i < codeCount - 1) {
            int k;
            int entryLen;

            i++;
            k = codes[i];

            if (k < curPos) {
                entryLen = g_lzwLen[k];
                if (entryLen == 0) {
                    break;              /* the reserved code: stream is corrupt */
                }
                mps_expand(k, entry);
            } else if (k == curPos) {
                /* KwKwK: the entry is the previous one with its own first byte
                 * appended, and it is exactly what this round is about to add
                 * to the dictionary. */
                entryLen = g_lzwLen[prev] + 1;
                mps_expand(prev, entry);
                entry[entryLen - 1] = g_lzwFirst[prev];
            } else {
                break;                  /* code beyond the dictionary */
            }

            if (outN + entryLen > outCap) {
                unsigned char *bigger;
                outCap = (outN + entryLen) * 2;
                bigger = (unsigned char *)realloc(out, (size_t)outCap);
                if (bigger == NULL) {
                    break;
                }
                out = bigger;
            }
            memcpy(out + outN, entry, (size_t)entryLen);
            outN += entryLen;

            /* Add `previous entry + first byte of this one`. */
            g_lzwPrefix[curPos] = prev;
            g_lzwSuffix[curPos] = entry[0];
            g_lzwFirst[curPos] = g_lzwFirst[prev];
            g_lzwLen[curPos] = g_lzwLen[prev] + 1;

            /* In the KwKwK case the entry just emitted IS the slot just filled,
             * so that is the index the next round carries forward. */
            prev = (k == curPos) ? curPos : k;
            curPos++;
        }
        i++;
    }

    free(codes);
    free(entry);
    *outLen = outN;
    return out;
}

/* ---------------------------------------------------------------------------
 * Stage 2: RLE, control byte 0x90.
 *
 *   0x90 0x00   a literal 0x90
 *   0x90 CC     repeat the previously emitted byte until the run totals CC
 *   first byte  always emitted literally
 *
 * Decoded straight into the final index buffer. An image that under-produces
 * leaves the remainder as index 0 and one that over-produces is truncated;
 * both are normal (see docs/formats/pic.md).
 * ------------------------------------------------------------------------ */

static void mps_rle(const unsigned char *coded, long len,
                    unsigned char *dst, long dstLen)
{
    long i = 1;
    long n = 0;

    if (len <= 0 || dstLen <= 0) {
        return;
    }

    dst[n++] = coded[0];

    while (i < len && n < dstLen) {
        if (coded[i] != MPS_RLE_CONTROL) {
            dst[n++] = coded[i];
        } else {
            unsigned char next = (i + 1 < len) ? coded[i + 1] : 0;

            if (next == 0x00) {
                dst[n++] = MPS_RLE_CONTROL;
            } else {
                unsigned char val = dst[n - 1];
                int r;
                for (r = 0; r < (int)next - 1 && n < dstLen; r++) {
                    dst[n++] = val;
                }
            }
            i++;
        }
        i++;
    }
}

/* ---------------------------------------------------------------------------
 * The two entry points Fileio.c calls.
 * ------------------------------------------------------------------------ */

void Mps_Begin(void *palette)
{
    unsigned char *file;
    long size;
    long off;
    SidPaletteBlock *pal;

    free(g_mpsImage);
    g_mpsImage = NULL;
    g_mpsImageSize = 0;
    g_mpsImagePos = 0;
    g_sidImageWidth = 0;
    g_sidImageHeight = 0;

    /* Sid_OpenFileCore opened the file and has read nothing from it yet -- it
     * only parked the cursor at the end of the buffer so the real MPS_CODE
     * would refill before its first read. Rewind and take the lot. */
    size = _lseek(g_sidFileFd, 0L, SEEK_END);
    if (size <= 0) {
        return;
    }
    _lseek(g_sidFileFd, 0L, SEEK_SET);

    file = (unsigned char *)malloc((size_t)size);
    if (file == NULL) {
        return;
    }
    if (_read(g_sidFileFd, file, (unsigned int)size) != (int)size) {
        free(file);
        return;
    }

    /* A block is a two-char tag, a uint16 length, then that many bytes. */
    off = 0;
    while (off + 4 <= size) {
        int t0 = file[off];
        int t1 = file[off + 1];
        long length = (long)file[off + 2] | ((long)file[off + 3] << 8);

        if (t0 == 'M' && (t1 == '0' || t1 == '1')) {
            /* Palette blocks are always under 64 KB, so `length` is reliable
             * here even though it wraps for large image blocks (quirk Q-002).
             * Sid_SetPalette wants the block complete with its four-byte
             * header, which is exactly the on-disk layout. */
            if (palette == (void *)1) {
                pal = &g_mpsOwnPalette;
            } else {
                pal = (SidPaletteBlock *)palette;
            }
            if (pal != NULL && 4 + length <= size - off) {
                long copy = 4 + length;
                if (copy > (long)sizeof(SidPaletteBlock)) {
                    copy = (long)sizeof(SidPaletteBlock);
                }
                memcpy(pal, file + off, (size_t)copy);
                Sid_SetPalette(pal);
            }
            off += 4 + length;
        } else if (t0 == 'X' && (t1 == '0' || t1 == '1')) {
            const unsigned char *hdr = file + off + 4;
            int width;
            int height;
            int maxBits;
            int fourBit = (t1 == '1');
            unsigned char *lzw;
            long lzwLen = 0;
            long packedLen;

            if (off + 4 + 5 > size) {
                break;
            }
            width = (int)hdr[0] | ((int)hdr[1] << 8);
            height = (int)hdr[2] | ((int)hdr[3] << 8);
            maxBits = hdr[4];
            if (width <= 0 || height <= 0) {
                break;
            }

            /* The stored length is not trusted: it wraps mod 65536 for blocks
             * over 64 KB. The compressed stream runs to end of file. */
            lzw = mps_lzw(hdr + 5, size - (off + 4 + 5), maxBits, &lzwLen);
            if (lzw == NULL) {
                break;
            }

            packedLen = fourBit ? (long)((width + 1) / 2) * height
                                : (long)width * height;
            g_mpsImageSize = (long)width * height;

            if (fourBit) {
                unsigned char *packed = (unsigned char *)calloc((size_t)packedLen, 1);
                g_mpsImage = (unsigned char *)calloc((size_t)g_mpsImageSize, 1);
                if (packed != NULL && g_mpsImage != NULL) {
                    int x;
                    int y;
                    mps_rle(lzw, lzwLen, packed, packedLen);
                    /* Two indices per byte, left pixel in the low nibble, rows
                     * padded to a whole byte. */
                    for (y = 0; y < height; y++) {
                        const unsigned char *row = packed + (long)y * ((width + 1) / 2);
                        for (x = 0; x < width; x += 2) {
                            unsigned char pair = row[x / 2];
                            g_mpsImage[(long)y * width + x] = (unsigned char)(pair & 0x0f);
                            if (x + 1 < width) {
                                g_mpsImage[(long)y * width + x + 1] =
                                    (unsigned char)(pair >> 4);
                            }
                        }
                    }
                }
                free(packed);
            } else {
                g_mpsImage = (unsigned char *)calloc((size_t)g_mpsImageSize, 1);
                if (g_mpsImage != NULL) {
                    mps_rle(lzw, lzwLen, g_mpsImage, g_mpsImageSize);
                }
            }

            free(lzw);
            if (g_mpsImage == NULL) {
                g_mpsImageSize = 0;
                break;
            }

            g_sidImageWidth = width;
            g_sidImageHeight = height;
            break;
        } else {
            break;                      /* unknown tag; nothing more to read */
        }
    }

    free(file);
}

void Mps_Read(void *dst, int count)
{
    long avail;

    if (count <= 0) {
        return;
    }
    if (g_mpsImage == NULL) {
        memset(dst, 0, (size_t)count);
        return;
    }

    avail = g_mpsImageSize - g_mpsImagePos;
    if (avail <= 0) {
        memset(dst, 0, (size_t)count);
        return;
    }
    if (avail > count) {
        avail = count;
    }

    memcpy(dst, g_mpsImage + g_mpsImagePos, (size_t)avail);
    if (avail < count) {
        memset((unsigned char *)dst + avail, 0, (size_t)(count - avail));
    }
    g_mpsImagePos += avail;
}
