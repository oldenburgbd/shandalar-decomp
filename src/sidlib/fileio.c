/* sidlib/Fileio.c — loading an image file into a graphics page.
 *
 * Sid_OpenFileCore does the work; the four functions after it are pure
 * forwarders, each pinning its first and last arguments. The first argument
 * is a page index (negative = decode but draw nothing); the last selects
 * where the palette goes; the two always-zero middle arguments are the
 * destination x and y.
 */

#include <fcntl.h>
#include <io.h>
#include <string.h>

/* C4035 ("no return value") is expected and must not be silenced by adding
 * one: neither path through Sid_OpenFileCore sets eax before its epilogue,
 * and `return 0;` would emit an xor the original lacks. Plain `disable` —
 * MSVC 4.2 predates the push/pop forms. */
#pragma warning(disable : 4035)

#include "sidlib/sidcore.h"
#include "sidlib/sidfileio.h"

/* Loads an image into a page. Both halves do the same three things — open,
 * decode the palette, loop rows into Sid_PutLine — through two unrelated I/O
 * layers; the duplicated row loop is the original's. Nothing is returned:
 * the wrappers hand back whatever the last call left in eax, and both are
 * byte-exact declared `int`. */
// FUNCTION: SHANDALAR 0x0057c580
// FUNCTION: FACEMAKER 0x0040aec0
int Sid_OpenFileCore(int page, int x, int y, const char *name, SidPaletteBlock *palette)
{
    unsigned char local[SID_PALETTE_BLOCK];
    int fd;

    // STRING: SHANDALAR 0x005a1858
    // STRING: FACEMAKER 0x00410320
    if (_stricmp(".pcx", strchr(name, '.')) == 0) {
        // STRING: SHANDALAR 0x005a17a4
        // STRING: FACEMAKER 0x0041026c
        g_sidPcxFile = fopen(name, "rb");
        /* The format string must sit on the line directly under its marker:
         * reccmp binds a STRING marker to the literal on the following line
         * and silently drops the whole function when it does not find one. */
        // STRING: SHANDALAR 0x005a1840
        // STRING: FACEMAKER 0x00410308
        Sid_Assert(g_sidPcxFile != NULL, SIDLIB_FILEIO_C, 247, "Error Opening File %s\r\n",
                   name);
        g_sidPcxName = name;

        if (palette == (SidPaletteBlock *)1) {
            palette = (SidPaletteBlock *)local;
        }
        if (palette != NULL) {
            Pcx_ReadHeader(palette->rgb);
            palette->tag0 = 'M';
            palette->tag1 = '1';
            palette->bytes = SID_PALETTE_RGB;
            palette->first = 0;
            palette->last = 255;
            Sid_SetPalette(palette);
        } else {
            Pcx_ReadHeader(NULL);
        }

        if (page < 0) {
            g_sidImageHeight = 0;
        }
        for (g_sidImageRow = 0; g_sidImageRow < g_sidImageHeight; g_sidImageRow++) {
            Pcx_DecodeRow(g_sidRowBuffer);
            Sid_PutLine(g_sidRowBuffer, page, x, y + g_sidImageRow, g_sidImageWidth);
        }

        fclose(g_sidPcxFile);
    } else {
        fd = _open(name, _O_BINARY);
        if (fd == -1) {
            /* Four arguments: `name` lands in Sid_Assert's format position
             * and gets printed as a format string — the original's doing. */
            Sid_Assert(0, SIDLIB_FILEIO_C, 136, name);
        }

        g_sidFileFdSaved = fd;
        g_sidFileFd = fd;
        /* Cursor at the end so MPS_CODE refills before its first read. */
        g_mpsCursor = g_mpsBufferEnd;
        g_sidUnknownB85c = -1;
        g_mpsRefill = Sid_MpsRefill;
        Mps_Begin(palette);

        if (page < 0) {
            g_sidImageHeight = 0;
        }
        for (g_sidImageRow = 0; g_sidImageRow < g_sidImageHeight; g_sidImageRow++) {
            Mps_Read(g_sidRowBuffer, g_sidImageWidth);
            Sid_PutLine(g_sidRowBuffer, page, x, y + g_sidImageRow, g_sidImageWidth);
        }

        if (g_sidInvalidFd != g_sidFileFdSaved) {
            if (_close(g_sidFileFdSaved) != 0) {
                Sid_Assert(0, SIDLIB_FILEIO_C, 168, NULL);
            }
        }
    }
}

// FUNCTION: SHANDALAR 0x0057c7e0
// FUNCTION: FACEMAKER 0x0040b120
int Sid_OpenFileInGroup(int group, const char *name)
{
    return Sid_OpenFileCore(group, 0, 0, name, (SidPaletteBlock *)1);
}

// FUNCTION: SHANDALAR 0x0057c800
int Sid_OpenFileInGroupAlt(int group, const char *name)
{
    return Sid_OpenFileCore(group, 0, 0, name, NULL);
}

// FUNCTION: SHANDALAR 0x0057c820
// FUNCTION: FACEMAKER 0x0040b180
int Sid_OpenFileByName(const char *name)
{
    return Sid_OpenFileCore(-1, 0, 0, name, (SidPaletteBlock *)1);
}

// FUNCTION: SHANDALAR 0x0057c840
// FUNCTION: FACEMAKER 0x0040b1a0
int Sid_OpenFileByNameFlag(const char *name, SidPaletteBlock *palette)
{
    return Sid_OpenFileCore(-1, 0, 0, name, palette);
}

/* Refills the MPS decompressor's input buffer through the pointer
 * Sid_OpenFileCore installs. A short read is not detected — _read's return
 * value goes nowhere and the cursor resets regardless, as in the original. */
// FUNCTION: SHANDALAR 0x0057c860
// FUNCTION: FACEMAKER 0x0040b2d0
void Sid_MpsRefill(void)
{
    _read(g_sidFileFd, g_mpsBuffer, SID_MPS_BUFFER);
    g_mpsCursor = g_mpsBuffer;
}
