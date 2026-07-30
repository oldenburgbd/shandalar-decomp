/* sidlib - MicroProse in-house 2D engine, Fileio.c: file opening.
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

#ifndef SIDLIB_FILEIO_H
#define SIDLIB_FILEIO_H

#include <stdio.h>

/* --- Fileio.c ------------------------------------------------------------ */

/* The palette block sidlib passes around: a six-byte header and 256 RGB
 * triples. Sid_OpenFileCore stamps the header itself after the PCX reader has
 * filled `rgb`, so every field's value is visible in the code that writes it.
 *
 * `tag0`/`tag1` are two chars, not one short: the writer stores them with two
 * byte moves and `bytes` with a word move, which is the shape of three fields
 * and not of a 2-char array. Sid_SetPalette then reads offset 0 as one 16-bit
 * value and compares it against 'M','0' -- so "M1" and "M0" are two palette
 * formats and this is the "M1" one. What distinguishes them is not
 * established. */
#define SID_PALETTE_RGB 768

/* What Sid_OpenFileCore reserves on the stack for a block of its own: 1024
 * bytes, not the 774 the struct needs. Established from the frame size, which
 * is exactly 0x400 with the block at offset 0. 256 entries of four bytes is the
 * obvious reading -- the M1 format fills three-byte entries and leaves the rest
 * untouched -- but nothing in either binary confirms the intent, only the
 * size. */
#define SID_PALETTE_BLOCK 1024

typedef struct SidPaletteBlock {
    unsigned char  tag0;                 /* +0x00  'M'                      */
    unsigned char  tag1;                 /* +0x01  '1'                      */
    unsigned short bytes;                /* +0x02  768 = 256 entries x 3    */
    unsigned char  first;                /* +0x04  0                        */
    unsigned char  last;                 /* +0x05  255                      */
    unsigned char  rgb[SID_PALETTE_RGB]; /* +0x06                           */
} SidPaletteBlock;

/* Loads an image file into a graphics page (0x0057c580).
 *
 * Two formats, chosen by extension and served by entirely different I/O
 * layers: ".pcx" goes through a FILE* and Pcxw.c's PCX reader, anything else
 * through a raw descriptor and the MPS_CODE decompressor. Both then walk the
 * image one row at a time and hand each row to Sid_PutLine at (x, y + row).
 *
 * `palette` is overloaded three ways. A real pointer is the caller's own block
 * to fill and install; 1 means "use a block of mine"; 0 means "leave the
 * palette alone". The 1 is only recognised on the PCX path -- see
 * docs/quirks.md Q-013.
 *
 * A negative `page` zeroes the row count, so the file is decoded and its
 * palette installed without anything being drawn. That is how the two
 * name-only wrappers load a palette. */
int Sid_OpenFileCore(int page, int x, int y, const char *name, SidPaletteBlock *palette);

/* FUNCTION: SHANDALAR 0x0057c7e0 */
int Sid_OpenFileInGroup(int group, const char *name);
/* FUNCTION: SHANDALAR 0x0057c800 */
int Sid_OpenFileInGroupAlt(int group, const char *name);
/* FUNCTION: SHANDALAR 0x0057c820 */
int Sid_OpenFileByName(const char *name);
/* FUNCTION: SHANDALAR 0x0057c840 */
int Sid_OpenFileByNameFlag(const char *name, SidPaletteBlock *palette);

/* --- the MPS decompressor's input stream --------------------------------- */

/* MPS_CODE is a section of hand-written assembly, 0x4a8 bytes in both
 * Shandalar.exe and Facemaker.exe, that pulls a compressed byte stream through
 * a cursor. When the cursor passes the end of the buffer the routine calls back
 * through a function pointer to refill it, and Fileio.c installs Sid_MpsRefill
 * there.
 *
 * The two copies are the same instruction stream but not the same bytes: 348 of
 * the 1192 differ, in 116 runs of exactly three, each the top three bytes of an
 * absolute data operand. Nothing else moves, which is what you would expect of
 * one .obj assembled once and linked twice.
 *
 * MPS_CODE itself is not compiled C -- it uses LODSW, pushes registers around
 * the indirect call and returns through LEAVE -- so it is not a decompilation
 * target. Only the C that feeds it is. (MPS is MicroProse Software, who
 * published Shandalar; the section name is the only evidence for the expansion
 * and none of the routines are named in either binary.) */
#define SID_MPS_BUFFER 512

/* The descriptor the refill reads from. Opened with _open, not fopen: the two
 * paths through Sid_OpenFileCore use different I/O layers entirely. */
extern int g_sidFileFd;
/* Where MPS_CODE reads its next byte. It advances this itself; the refill
 * resets it to the start of the buffer. */
extern unsigned char *g_mpsCursor;
extern unsigned char g_mpsBuffer[SID_MPS_BUFFER];

/* FUNCTION: SHANDALAR 0x0057c860 */
void Sid_MpsRefill(void);

/* One past the end of g_mpsBuffer, and the only thing MPS_CODE compares the
 * cursor against. Sid_OpenFileCore points the cursor here to force a refill on
 * the first read. */
extern unsigned char *g_mpsBufferEnd;
/* The refill callback slot MPS_CODE calls through. Sid_OpenFileCore installs
 * Sid_MpsRefill; nothing else writes it. */
extern void (*g_mpsRefill)(void);

/* The two MPS_CODE entry points Fileio.c uses. Hand-written assembly, not
 * decompilation targets -- see the note on the buffer above. */
/* STUB: SHANDALAR 0x0098a000 */
void Mps_Begin(void *palette);
/* STUB: SHANDALAR 0x0098a484 */
void Mps_Read(void *dst, int count);

/* --- what Fileio.c reaches into other modules for ------------------------ */

/* Declared here rather than pulled in with sidgfx.h/a Pcxw header on purpose:
 * every declaration visible to a translation unit moves MSVC 4.2's register
 * allocation across the whole unit, so fileio.c sees these five and nothing
 * else. The duplicate prototype of Sid_PutLine is deliberate and has to stay
 * character-identical to the one in sidgfx.h. */
void Sid_PutLine(const void *src, int page, int x, int y, unsigned int count);

/* Pcxw.c. Reads the 128-byte PCX header into a buffer of its own, asserts the
 * 0x0A signature at Pcxw.c:173, publishes the image extent, and -- when
 * `paletteOut` is not NULL -- seeks to the last 768 bytes of the file for the
 * VGA palette and rewinds to the pixel data. Returns 1. */
int Pcx_ReadHeader(unsigned char *paletteOut);
/* Pcxw.c. Expands one RLE-encoded PCX scanline into `dst`. Returns 1. */
/* FUNCTION: SHANDALAR 0x0057d850 */
int Pcx_ReadHeader(unsigned char *paletteOut);
/* FUNCTION: SHANDALAR 0x0057da00 */
int Pcx_DecodeRow(unsigned char *dst);

/* Pcxw.c's path string, for its asserts. A `char *` variable rather than a
 * literal at the call site -- the code loads the pointer out of .data -- so it
 * cannot use the SIDLIB_*_C macro the other modules share. */
extern char *g_sidPcxwFile;

/* The 128-byte PCX header Pcx_ReadHeader freads into. Declared as raw bytes
 * with named offsets rather than a struct: only the three fields sidlib
 * actually touches are established, and inventing the rest of the standard PCX
 * layout would be documenting the format, not the binary. */
extern unsigned char g_sidPcxHeader[128];
#define SID_PCX_BPP            0x03  /* byte, bits per plane             */
#define SID_PCX_XMIN           0x04  /* word                             */
#define SID_PCX_YMIN           0x06  /* word                             */
#define SID_PCX_XMAX           0x08  /* word, with Xmin gives the width  */
#define SID_PCX_YMAX           0x0a  /* word, with Ymin gives the height */
#define SID_PCX_BYTES_PER_LINE 0x42  /* word, the encoded row length     */
#define SID_PCX_NPLANES        0x41  /* byte, checked against 1          */
/* Installs a palette block. Reads the header Sid_OpenFileCore stamps. Defined
 * in lib.c; this prototype must stay character-identical to sidgfx.h. */
void Sid_SetPalette(const void *block);

/* The image extent the two readers publish, and the row the loop is on. All
 * three are globals in the original, including the loop counter. */
extern int g_sidImageWidth;
extern int g_sidImageHeight;
extern int g_sidImageRow;
/* Scratch for one decoded scanline, between the reader and Sid_PutLine. Its
 * real size is not established; it must be at least g_sidImageWidth. */
extern unsigned char g_sidRowBuffer[1024];

/* The PCX path's stream and the name it was opened with. */
extern FILE *g_sidPcxFile;
extern const char *g_sidPcxName;

/* The raw path's descriptor, kept a second time. g_sidFileFd is what the
 * refill reads from; this copy is what the close at Fileio.c:168 tests and
 * closes. Both are written from the same _open result. */
extern int g_sidFileFdSaved;
/* Written -1 next to the two descriptor stores and read nowhere in either
 * binary. Its meaning is not established; do not assume it is a descriptor. */
extern int g_sidUnknownB85c;
/* An int initialised to -1, read once -- by the close test in
 * Sid_OpenFileCore -- and written nowhere in either binary. The comparison is
 * therefore always against -1, but the original loads it from memory rather
 * than comparing with a literal, so it is a variable in the source and not a
 * constant. */
extern int g_sidInvalidFd;

#endif /* SIDLIB_FILEIO_H */
