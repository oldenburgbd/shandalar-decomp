/* Globals and helper stubs -- ADOPTED from the archived attempt
 * (salvage/decomp/src/stubs.c). Nothing in this file is a reconstruction:
 * the function bodies are 'return 0' placeholders (STUB markers), and the
 * globals are declarations whose addresses were established from the
 * binary. The GLOBAL/STUB marker set is the archive's tuned configuration. */

/* Placeholders for functions that decompiled code calls but that have not been
 * reconstructed yet.
 *
 * These carry STUB annotations, not FUNCTION ones. The distinction is exactly
 * what we want: reccmp uses a STUB marker to resolve calls to that address, so
 * every caller stops counting the call as a mismatch, but it excludes the stub
 * itself from both the implemented count and the accuracy average. So the
 * annotation claims "this is where that function lives", never "this body
 * matches". When a stub's real function is decompiled it moves to its proper
 * module and its marker becomes FUNCTION there.
 *
 * Sid_Assert is marked too. A STUB marker asserts only where the function
 * lives, never that our body matches it, so an uncertain signature is no reason
 * to withhold one - and withholding it leaves every caller's score at the mercy
 * of DLL layout.
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

/* The one file that legitimately includes everything: it defines every global
 * and every stub. Safe because nothing here carries a FUNCTION marker, so this
 * translation unit's register allocation is never scored. */
#include "sidlib/sidcore.h"
#include "sidlib/sidgfx.h"
#include "sidlib/sidtext.h"
#include "sidlib/sidsprite.h"
#include "sidlib/sidfileio.h"
#include "sidlib/sidpalette.h"
#include "game/gameslot.h"
#include "sidlib/sidwindow.h"

/* The original's globals. Under the Phase-2 hook-and-replace harness these
 * resolve to fixed addresses in the loaded image; for now they are our own so
 * the DLL links.
 *
 * The GLOBAL annotations are not documentation — reccmp reads them. Without
 * them every instruction that touches a global counts as a mismatch, because
 * reccmp sees a named symbol on our side and a bare offset on the original's
 * and cannot tell they are the same thing. Annotating turns those into matches.
 * A global with no annotation below is one whose original address we have not
 * established yet; do not invent one to silence the diff. */

/* g_sidPages is NOT defined here. It has to sit immediately after
 * g_sidPaletteRgb, so the two are defined together further down -- see the
 * note there. */

/* How many fonts the loaded font file declared. Written once by
 * Sid_LoadFontFile, which also returns it. */
// GLOBAL: SHANDALAR 0x00983750
int g_sidFontCount;

/* The assert message Sid_LoadFontFile passes when fopen fails. It is a `char *`
 * variable in the original, not a literal at the call site: the code loads the
 * pointer out of .data (`mov eax,[0x005861c4]; push eax`) rather than pushing a
 * string address. The string it points to sits immediately after it at
 * 0x005861c8, which is what a file-scope `char *x = "...";` produces. */
// GLOBAL: SHANDALAR 0x005861c4
// STRING: SHANDALAR 0x005861c8
char *g_sidMsgFileOpen = "File %s could not be opened: EXITING\r\n";

// GLOBAL: SHANDALAR 0x00986220
// GLOBAL: FACEMAKER 0x004280d0
void *g_sidPalette;

/* Sid_SetPalette copies the whole incoming block here before unpacking it.
 * Read by seventeen call sites outside sidlib, so it is the engine's idea of
 * "the current palette, as loaded". */
// GLOBAL: SHANDALAR 0x00986230
unsigned char g_sidPaletteRaw[1024];

/* Five bytes written by Sid_SetPalette's tail. 0x00986d80 is where the object
 * containing them starts as far as the offsets go; whether that is its real
 * base is not established, and neither is what it is for. */
// GLOBAL: SHANDALAR 0x00986d80
unsigned char g_sidUnknown986d80[16];

/* The external palette-viewer window, found by class name each time the
 * palette changes and invalidated so it repaints. Absent in a shipped run. */
// GLOBAL: SHANDALAR 0x00738800
void *g_sidPaletteWindow;

/* The two palette-format tags, compared as 16-bit values against the block's
 * first two bytes. Literals in the pool, sitting immediately after "Palette". */
// GLOBAL: SHANDALAR 0x005a1730
char g_sidTagM1[] = "M1";
// GLOBAL: SHANDALAR 0x005a1734
char g_sidTagM0[] = "M0";

/* THESE TWO MUST STAY ADJACENT, IN THIS ORDER. See docs/quirks.md Q-022.
 *
 * Sid_CreateScreenPage's palette-clearing loop has no counter. It walks a
 * pointer from g_sidPaletteRgb upwards and stops when the pointer reaches
 * g_sidPages:
 *
 *     entry = g_sidPaletteRgb;
 *     do { ...; entry++; } while (entry < (SidRgb *)g_sidPages);
 *
 * which terminates after exactly 256 iterations only because the original's
 * linker put the page table 1,024 bytes after the palette table (0x00986550 ->
 * 0x00986950). That is a property of the original's data layout, not of the
 * code, and ours does not reproduce it for free: as plain uninitialised globals
 * both land in COMMON, where the linker orders them however it likes. It put
 * them 812,304 bytes apart, and the loop wrote over all of it -- the host EXE
 * died with an access violation the first time this function was ever actually
 * executed. (The scored DLL never runs, and the differential test driver is
 * required to keep Sid_CreateScreenPage unreachable for a related reason --
 * quirk Q-009 -- so nothing had exercised it before.)
 *
 * The fix belongs in the layout, not the loop: lib.c reproduces the original
 * faithfully and is scored, so it is left alone. Pinning both objects into one
 * named segment, with initialisers so they are emitted to it in definition
 * order rather than being collected into COMMON, restores the 1,024-byte
 * spacing the loop depends on. Verify with the linker map after touching this;
 * the addresses must differ by exactly 0x400. */
#pragma data_seg("SIDPAL")

// GLOBAL: SHANDALAR 0x00986550
// GLOBAL: FACEMAKER 0x00428400
SidRgb g_sidPaletteRgb[256] = {0};

// GLOBAL: SHANDALAR 0x00986950
// GLOBAL: FACEMAKER 0x00428800
SidPage *g_sidPages[SID_MAX_PAGES] = {0};

#pragma data_seg()

// GLOBAL: SHANDALAR 0x00986980
// GLOBAL: FACEMAKER 0x00428830
int g_sidScreenBpp;

// GLOBAL: SHANDALAR 0x007483f8
// GLOBAL: FACEMAKER 0x0041de64
void *g_sidScreenDC;

// GLOBAL: SHANDALAR 0x00986990
// GLOBAL: FACEMAKER 0x00428840
unsigned char g_sidDibPalette[256 * 4];

// GLOBAL: SHANDALAR 0x005a1608
// GLOBAL: FACEMAKER 0x0040fb04
int g_sidDibBlitEnabled;

// GLOBAL: SHANDALAR 0x005a1614
// GLOBAL: FACEMAKER 0x0040fb10
void *g_sidRowTemp1;

// GLOBAL: SHANDALAR 0x005a1618
// GLOBAL: FACEMAKER 0x0040fb14
void *g_sidRowTemp2;

// GLOBAL: SHANDALAR 0x005a161c
// GLOBAL: FACEMAKER 0x0040fb18
void *g_sidRowTemp3;

// GLOBAL: SHANDALAR 0x00737fe0
// GLOBAL: FACEMAKER 0x0041a0d0
void *g_blitBmi;

// GLOBAL: SHANDALAR 0x007387fc
// GLOBAL: FACEMAKER 0x0041a8ec
int g_blitBmiReady;

// GLOBAL: SHANDALAR 0x00739008
// GLOBAL: FACEMAKER 0x0041b0f8
void *g_blitBmi2;

// GLOBAL: SHANDALAR 0x007357d0
// GLOBAL: FACEMAKER 0x004178c0
int g_blitBmi2Ready;

// GLOBAL: SHANDALAR 0x00986978
// GLOBAL: FACEMAKER 0x00428828
int g_sidScreenHeight;

// GLOBAL: SHANDALAR 0x0098697c
// GLOBAL: FACEMAKER 0x0042882c
int g_sidScreenWidth;

// GLOBAL: SHANDALAR 0x00739814
// GLOBAL: FACEMAKER 0x0041b904
int g_sidScreenPageReady;

// GLOBAL: SHANDALAR 0x00986d90
// GLOBAL: FACEMAKER 0x00428c40
void *g_sidLogPalette;

// GLOBAL: SHANDALAR 0x00748414
// GLOBAL: FACEMAKER 0x0041de6c
void *g_sidPaletteHandle;

// GLOBAL: SHANDALAR 0x005a160c
// GLOBAL: FACEMAKER 0x0040fb08
int g_sidUnknown160c;

// GLOBAL: SHANDALAR 0x005a1610
int g_sidLinePaletteDirty;

// GLOBAL: SHANDALAR 0x00737fe8
void *g_lineBmi;

// GLOBAL: SHANDALAR 0x007387f8
int g_lineBmiReady;

// GLOBAL: SHANDALAR 0x00986da0
int g_sidEventFlags;

/* Was recorded at 0x009837e4, which is `height`, not the start of the record.
 * See the SidFont comment in sidtext.h. */
// GLOBAL: SHANDALAR 0x00983760
// GLOBAL: FACEMAKER 0x00425610
SidFont g_sidFonts[SID_MAX_FONTS];

// GLOBAL: SHANDALAR 0x00739818
// GLOBAL: FACEMAKER 0x0041b9d8
int g_sprScaleXStep;

// GLOBAL: SHANDALAR 0x0073981c
// GLOBAL: FACEMAKER 0x0041b9dc
int g_sprScaleYStep;

// GLOBAL: SHANDALAR 0x00739820
// GLOBAL: FACEMAKER 0x0041b9e0
int g_sprScaleRight;

// GLOBAL: SHANDALAR 0x00739828
// GLOBAL: FACEMAKER 0x0041b9e8
int g_sprScaleSrcToDst[1024];

// GLOBAL: SHANDALAR 0x0073a830
// GLOBAL: FACEMAKER 0x0041c9f0
int g_sprScaleSrcH;

// GLOBAL: SHANDALAR 0x0073a834
// GLOBAL: FACEMAKER 0x0041c9f4
int g_sprScaleXAcc;

// GLOBAL: SHANDALAR 0x0073a838
// GLOBAL: FACEMAKER 0x0041c9f8
int g_sprScaleYAcc;

// GLOBAL: SHANDALAR 0x0073a83c
// GLOBAL: FACEMAKER 0x0041c9fc
int g_sprScaleDstH;

// GLOBAL: SHANDALAR 0x0073a840
// GLOBAL: FACEMAKER 0x0041ca00
int g_sprScaleLeft;

// GLOBAL: SHANDALAR 0x0073a844
// GLOBAL: FACEMAKER 0x0041ca04
int g_sprScaleDstW;

// GLOBAL: SHANDALAR 0x0073a848
// GLOBAL: FACEMAKER 0x0041ca08
int g_sprScaleDstToSrc[1024];

// GLOBAL: SHANDALAR 0x0073b848
// GLOBAL: FACEMAKER 0x0041da08
int g_sprScaleSrcW;

// GLOBAL: SHANDALAR 0x0073a828
// GLOBAL: FACEMAKER 0x0041c9e8
void *g_sprArenaBase;

// GLOBAL: SHANDALAR 0x0073a82c
// GLOBAL: FACEMAKER 0x0041c9ec
void *g_sprArenaCur;

/* Fileio.c's raw-descriptor path. The .pcx path uses a FILE* and these are not
 * touched; the two are alternatives, not layers. */
// GLOBAL: SHANDALAR 0x0073b858
// GLOBAL: FACEMAKER 0x0041da18
int g_sidFileFd;

// GLOBAL: SHANDALAR 0x00982540
// GLOBAL: FACEMAKER 0x004243e8
unsigned char *g_mpsCursor;

// GLOBAL: SHANDALAR 0x00982550
// GLOBAL: FACEMAKER 0x004243f0
unsigned char g_mpsBuffer[SID_MPS_BUFFER];

/* Initialised, and the initialiser is the point: both binaries hold
 * g_mpsBuffer + 512 here, which is why the two images disagree about what
 * address that is (Shandalar's row buffer, Facemaker's callback slot). It is
 * arithmetic on the buffer, not a reference to whatever the linker put next. */
// GLOBAL: SHANDALAR 0x005a1814
// GLOBAL: FACEMAKER 0x004102dc
unsigned char *g_mpsBufferEnd = g_mpsBuffer + SID_MPS_BUFFER;

// GLOBAL: SHANDALAR 0x00748428
// GLOBAL: FACEMAKER 0x004245f0
void (*g_mpsRefill)(void);

// GLOBAL: SHANDALAR 0x0073b860
// GLOBAL: FACEMAKER 0x0041da20
int g_sidFileFdSaved;

// GLOBAL: SHANDALAR 0x0073b85c
// GLOBAL: FACEMAKER 0x0041da1c
int g_sidUnknownB85c;

// GLOBAL: SHANDALAR 0x005a1818
// GLOBAL: FACEMAKER 0x004102e0
int g_sidInvalidFd = -1;

/* Written by Pcx_ReadHeader from the PCX header's extent fields and read by
 * Sid_OpenFileCore's row loops. Also read by a cluster of functions around
 * 0x004c8900, outside sidlib. */
// GLOBAL: SHANDALAR 0x005a58c0
// GLOBAL: FACEMAKER 0x00414328
int g_sidImageWidth;

// GLOBAL: SHANDALAR 0x005a58c4
// GLOBAL: FACEMAKER 0x0041432c
int g_sidImageHeight;

/* The row loop's counter. A global, not a local: the original reloads it from
 * memory on every iteration, which it would not do for a local it had put in a
 * register. */
// GLOBAL: SHANDALAR 0x0073b854
// GLOBAL: FACEMAKER 0x0041da14
int g_sidImageRow;

// GLOBAL: SHANDALAR 0x00982750
// GLOBAL: FACEMAKER 0x00424600
unsigned char g_sidRowBuffer[1024];

// GLOBAL: SHANDALAR 0x0097f994
// GLOBAL: FACEMAKER 0x00424340
FILE *g_sidPcxFile;

// GLOBAL: SHANDALAR 0x0097f998
// GLOBAL: FACEMAKER 0x00424348
const char *g_sidPcxName;

// GLOBAL: SHANDALAR 0x0097f9a0
unsigned char g_sidPcxHeader[128];

// GLOBAL: SHANDALAR 0x005a5d10
// STRING: SHANDALAR 0x005a5cec
char *g_sidPcxwFile = "D:\\NewMagic\\sources\\sidlib\\Pcxw.c";

/* Pcxw.c 0x0057d850 and 0x0057da00 - the PCX header/palette reader and the RLE
 * row expander. Both are understood in detail (see sidfileio.h) and neither is
 * reconstructed yet. */
/* MPS_CODE. Hand-written assembly; these two exist only so that the calls to
 * them resolve. They will never be reconstructed - see sidfileio.h.
 *
 * SHANDALAR_HOST is defined only when this file is compiled for the runnable
 * host EXE, which supplies working versions in src/host/mps.c and would
 * otherwise collide with these at link time. The DLL reccmp scores is built
 * without it and keeps the stubs, so nothing measured changes. */
#ifndef SHANDALAR_HOST
// STUB: SHANDALAR 0x0098a000
// STUB: FACEMAKER 0x0042a000
void Mps_Begin(void *palette)
{
    (void)palette;
}

// STUB: SHANDALAR 0x0098a484
// STUB: FACEMAKER 0x0042a484
void Mps_Read(void *dst, int count)
{
    (void)dst; (void)count;
}
#endif

/* 0x00566cea — x87-based bulk memcpy. Lives outside sidlib and is not yet
 * reconstructed; the real one moves 8 bytes per FLD/FSTP pair. */
// STUB: SHANDALAR 0x00566cea
void Sid_FastMemcpy(void *dst, const void *src, unsigned int count)
{
    memcpy(dst, src, count);
}

/* 0x0046a520, outside sidlib. The address is certain - every compiled-in
 * assert in the binary calls it - so the STUB marker is justified even though
 * the body is ours. Leaving it unmarked was a mistake: an unresolvable call
 * target makes every assert-using function's score swing with DLL layout,
 * which is what had Spr_LoadFile oscillating between 66% and 93%. */
// STUB: SHANDALAR 0x0046a520
void Sid_Assert(int condition, const char *file, int line, const char *fmt, ...)
{
    va_list args;

    if (condition) {
        return;
    }

    fprintf(stderr, "%s(%d): ", file, line);
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    exit(1);
}

/* text.c 0x0057aae0 and 0x0057ae50 - the text measurer and the string drawer.
 * Sid_DrawTextCore is the only caller of either, so its call sites are what
 * establish the argument lists; the bodies are 454 and 990 bytes and neither is
 * reconstructed. The drawer does return a value, which its one caller ignores. */
/* 0x004523d4 — resolves a sidlib window key pair to a small integer id. Lives
 * outside sidlib and is not yet reconstructed. */
// STUB: SHANDALAR 0x004523d4
int Sid_LookupWindowId(long keyLow, long keyHigh)
{
    (void)keyLow; (void)keyHigh;
    return -1;
}

/* --- game layer -------------------------------------------------------- */

// GLOBAL: SHANDALAR 0x008ca260
int g_game8ca260;
// GLOBAL: SHANDALAR 0x008ca264
int g_game8ca264;

// GLOBAL: SHANDALAR 0x008ca270
GameSlot g_gameSlots[GAME_SLOT_OWNERS][GAME_SLOTS_PER_OWNER];

// GLOBAL: SHANDALAR 0x00594208
GameCard g_gameCards[1024];

// GLOBAL: SHANDALAR 0x0093a7d4
int g_game93a7d4;
// GLOBAL: SHANDALAR 0x0092e95c
int g_game92e95c;
// GLOBAL: SHANDALAR 0x0094d464
int g_game94d464;
// GLOBAL: SHANDALAR 0x007a04c4
int g_game7a04c4;
// GLOBAL: SHANDALAR 0x008bcfe8
int g_game8bcfe8;
// GLOBAL: SHANDALAR 0x008e32fc
int g_game8e32fc;
// GLOBAL: SHANDALAR 0x008c83f8
int g_game8c83f8;

/* Returns a free column in an owner's row, or -1. Not reconstructed. */
// STUB: SHANDALAR 0x0056ae5d
int Game_FindFreeSlot(int owner, int arg)
{
    (void)owner; (void)arg;
    return -1;
}

/* Sits immediately before Game_Predicate466fe6, so same object file. */
// STUB: SHANDALAR 0x00466f70
int Game_CombineMask(int mask, int a, int b)
{
    (void)mask; (void)a; (void)b;
    return 0;
}

// GLOBAL: SHANDALAR 0x008c6ae4
int g_gameEventCount;
// GLOBAL: SHANDALAR 0x0094d2b0
int g_gameEventWord[GAME_EVENT_MAX];
// GLOBAL: SHANDALAR 0x00939310
GameEventPair g_gameEventWhere[GAME_EVENT_MAX];
// GLOBAL: SHANDALAR 0x0093a940
GameEventPair g_gameEventWhat[GAME_EVENT_MAX];
// GLOBAL: SHANDALAR 0x008a9230
int g_gameEventMode[GAME_EVENT_MAX];
// GLOBAL: SHANDALAR 0x007be4e0
int g_gameEventExtra[GAME_EVENT_MAX];

// GLOBAL: SHANDALAR 0x0094d178
int g_game94d178;
// GLOBAL: SHANDALAR 0x008bd200
int g_game8bd200;

/* memcpy-shaped, at 0x0057e82c. Outside sidlib's mapped range and not
 * reconstructed. */
// STUB: SHANDALAR 0x0057e82c
void Game_CopyBlock(void *dst, const void *src, int n)
{
    memcpy(dst, src, (size_t)n);
}

// GLOBAL: SHANDALAR 0x007befec
int g_game7befec;

/* --- what Game_ResetState clears ---------------------------------------- */
// GLOBAL: SHANDALAR 0x007bfee0
int g_game7bfee0[2];
// GLOBAL: SHANDALAR 0x007806a0
int g_game7806a0[2][8];
// GLOBAL: SHANDALAR 0x0077ed80
int g_game77ed80[2][8];
// GLOBAL: SHANDALAR 0x0077d170
int g_game77d170[2][8];
// GLOBAL: SHANDALAR 0x0074c790
int g_game74c790[2][51];
// GLOBAL: SHANDALAR 0x0074cdc0
int g_game74cdc0[2][11];
// GLOBAL: SHANDALAR 0x007806e0
int g_game7806e0[2];
// GLOBAL: SHANDALAR 0x008e2690
int g_game8e2690[8];
// GLOBAL: SHANDALAR 0x007be740
int g_game7be740[8];
// GLOBAL: SHANDALAR 0x0073c00c
int g_game73c00c;
// GLOBAL: SHANDALAR 0x008c776c
int g_game8c776c;
// GLOBAL: SHANDALAR 0x008c7468
int g_game8c7468;
// GLOBAL: SHANDALAR 0x008e26bc
int g_game8e26bc;
// GLOBAL: SHANDALAR 0x0078e5d8
int g_game78e5d8;
// GLOBAL: SHANDALAR 0x0074f1e0
int g_game74f1e0;
// GLOBAL: SHANDALAR 0x0077f440
int g_game77f440;
// GLOBAL: SHANDALAR 0x0074d26c
int g_game74d26c;
// GLOBAL: SHANDALAR 0x0074ccc4
int g_game74ccc4;
// GLOBAL: SHANDALAR 0x0077f600
int g_game77f600;
// GLOBAL: SHANDALAR 0x007a81a8
int g_game7a81a8;
// GLOBAL: SHANDALAR 0x007beb20
int g_game7beb20;
// GLOBAL: SHANDALAR 0x007bfe74
int g_game7bfe74;
// GLOBAL: SHANDALAR 0x007beb04
int g_game7beb04;
// GLOBAL: SHANDALAR 0x007bfe80
int g_game7bfe80;
// GLOBAL: SHANDALAR 0x007bf0ec
int g_game7bf0ec;
// GLOBAL: SHANDALAR 0x0078cf04
int g_game78cf04;
// GLOBAL: SHANDALAR 0x00939bf4
int g_game939bf4;
// GLOBAL: SHANDALAR 0x0078e5e0
int g_game78e5e0;
// GLOBAL: SHANDALAR 0x0077cf1c
int g_game77cf1c;
// GLOBAL: SHANDALAR 0x0077edc0
int g_game77edc0;
// GLOBAL: SHANDALAR 0x0092f414
int g_game92f414;
// GLOBAL: SHANDALAR 0x008bcf6c
int g_game8bcf6c;
// GLOBAL: SHANDALAR 0x007beff0
int g_game7beff0;
// GLOBAL: SHANDALAR 0x0093a794
int g_game93a794;
// GLOBAL: SHANDALAR 0x00939410
int g_game939410;
// GLOBAL: SHANDALAR 0x008c759c
int g_game8c759c;
// GLOBAL: SHANDALAR 0x0093022c
int g_game93022c;
// GLOBAL: SHANDALAR 0x0077e590
int g_game77e590;
// GLOBAL: SHANDALAR 0x0094e964
int g_game94e964;
// GLOBAL: SHANDALAR 0x0093a798
int g_game93a798;
// GLOBAL: SHANDALAR 0x0094e968
int g_game94e968;
// GLOBAL: SHANDALAR 0x0074c960
int g_game74c960;
// GLOBAL: SHANDALAR 0x0094d640
int g_game94d640;
// GLOBAL: SHANDALAR 0x008e266c
int g_game8e266c;
// GLOBAL: SHANDALAR 0x0092e820
int g_game92e820;
// GLOBAL: SHANDALAR 0x008e2688
int g_game8e2688;
// GLOBAL: SHANDALAR 0x008e2674
int g_game8e2674;
// GLOBAL: SHANDALAR 0x00939304
int g_game939304;
// GLOBAL: SHANDALAR 0x007a82dc
int g_game7a82dc;

// GLOBAL: SHANDALAR 0x008e1aa8
int g_game8e1aa8;
// GLOBAL: SHANDALAR 0x00951bdc
int g_game951bdc;

// GLOBAL: SHANDALAR 0x007a01e0
int g_game7a01e0;
// GLOBAL: SHANDALAR 0x007a82e0
int g_game7a82e0;

// GLOBAL: SHANDALAR 0x008e26b0
int g_game8e26b0;
// GLOBAL: SHANDALAR 0x0094e928
int g_game94e928;
// GLOBAL: SHANDALAR 0x008e44b8
int g_game8e44b8[512][38];

// GLOBAL: SHANDALAR 0x007a0230
int g_game7a0230;
// GLOBAL: SHANDALAR 0x007a00a4
int g_game7a00a4;

/* --- card-script helpers -------------------------------------------------
 *
 * Stubs, not reconstructions. Chosen because stubbing these six unblocks 87
 * card scripts; see the note in include/game/gameslot.h. */
// STUB: SHANDALAR 0x004a9910
int Game_Helper4a9910(int a, int b, int c, int d, int e) { return 0; }
// STUB: SHANDALAR 0x004ba309
int Game_Helper4ba309(int a, int b, int c) { return 0; }
// STUB: SHANDALAR 0x004ba392
int Game_Helper4ba392(int a, int b, int c) { return 0; }

/* Stubs 7-26; see include/game/gameslot.h. */
// STUB: SHANDALAR 0x0040cadc
int Game_Helper40cadc(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x0040d7b2
int Game_Helper40d7b2(int a1, int a2, int a3, int a4) { return 0; }
// STUB: SHANDALAR 0x00409a0b
int Game_Helper409a0b(int a1) { return 0; }
// STUB: SHANDALAR 0x00409ee8
int Game_Helper409ee8(int a1, int a2, int a3) { return 0; }
// STUB: SHANDALAR 0x004111b1
int Game_Helper4111b1(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x004143e0
int Game_Helper4143e0(int a1, int a2, int a3) { return 0; }
// STUB: SHANDALAR 0x00441699
int Game_Helper441699(int a1, int a2, int a3) { return 0; }
// STUB: SHANDALAR 0x00488b70
int Game_Helper488b70(int a1, int a2, int a3, int a4) { return 0; }
// STUB: SHANDALAR 0x00498365
int Game_Helper498365(int a1, int a2, int a3) { return 0; }
// STUB: SHANDALAR 0x0049f3c0
int Game_Helper49f3c0(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x004ba25e
int Game_Helper4ba25e(int a1) { return 0; }
// STUB: SHANDALAR 0x004d40a4
int Game_Helper4d40a4(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x0056ca10
int Game_Helper56ca10(int a1, int a2) { return 0; }

// GLOBAL: SHANDALAR 0x0093a934
int g_game93a934;
// GLOBAL: SHANDALAR 0x007beb10
int g_game7beb10[8];

/* Callbacks passed to Game_Helper488b70. Stubs; see gameslot.h. */
// STUB: SHANDALAR 0x00440b07
int Game_Callback440b07() { return 0; }
// STUB: SHANDALAR 0x00480f45
int Game_Callback480f45() { return 0; }
// STUB: SHANDALAR 0x004b484b
int Game_Callback4b484b() { return 0; }

// GLOBAL: SHANDALAR 0x007beb18
int g_game7beb18[8];
// GLOBAL: SHANDALAR 0x005aa620
int g_game5aa620;
// GLOBAL: SHANDALAR 0x005aa61c
int g_game5aa61c;
// GLOBAL: SHANDALAR 0x005aa4e8
int g_game5aa4e8;
// GLOBAL: SHANDALAR 0x005aa4ec
int g_game5aa4ec;

// STUB: SHANDALAR 0x004b3b46
int Game_Callback4b3b46() { return 0; }
// STUB: SHANDALAR 0x004b39b5
int Game_Callback4b39b5() { return 0; }

/* Referenced by generated card scripts. */
// GLOBAL: SHANDALAR 0x008c83ec
int g_game8c83ec;
// GLOBAL: SHANDALAR 0x008c83fa
int g_game8c83fa;
// GLOBAL: SHANDALAR 0x008c8afc
int g_game8c8afc;
// GLOBAL: SHANDALAR 0x0092f3cc
int g_game92f3cc;
// GLOBAL: SHANDALAR 0x0094d63c
int g_game94d63c;
// GLOBAL: SHANDALAR 0x0097db30
int g_game97db30;
// STUB: SHANDALAR 0x00480238
int Game_Callback480238(void) { return 0; }
// STUB: SHANDALAR 0x004803cc
int Game_Callback4803cc(void) { return 0; }
// STUB: SHANDALAR 0x0048044a
int Game_Callback48044a(void) { return 0; }
// STUB: SHANDALAR 0x004ae506
int Game_Callback4ae506(void) { return 0; }
// STUB: SHANDALAR 0x00498e8a
int Game_Helper498e8a(int a1, int a2, int a3) { return 0; }
// STUB: SHANDALAR 0x0055912c
int Game_Helper55912c(void) { return 0; }

/* Referenced by generated card scripts. */
// GLOBAL: SHANDALAR 0x00930f5c
int g_game930f5c;
// GLOBAL: SHANDALAR 0x0094f280
int g_game94f280;
// GLOBAL: SHANDALAR 0x00951968
int g_game951968;
// STUB: SHANDALAR 0x00558538
int Game_Helper558538(int a1, int a2, int a3, int a4, int a5, int a6, int a7) { return 0; }

/* Referenced by generated card scripts. */
// GLOBAL: SHANDALAR 0x007bf4b8
int g_game7bf4b8;
// GLOBAL: SHANDALAR 0x008e2fc4
int g_game8e2fc4;
// GLOBAL: SHANDALAR 0x008e3ec0
int g_game8e3ec0[32];
// GLOBAL: SHANDALAR 0x008e3ec8
int g_game8e3ec8[32];
// GLOBAL: SHANDALAR 0x008e3ed0
int g_game8e3ed0[32];
// GLOBAL: SHANDALAR 0x008e3f00
int g_game8e3f00;
// STUB: SHANDALAR 0x004323cc
int Game_Callback4323cc(void) { return 0; }
// STUB: SHANDALAR 0x0040ebb9
int Game_Helper40ebb9(int a1) { return 0; }

/* Referenced by generated card scripts. */
// GLOBAL: SHANDALAR 0x00765420
int g_game765420[32];
// GLOBAL: SHANDALAR 0x0077c57c
int g_game77c57c;
// GLOBAL: SHANDALAR 0x007be608
int g_game7be608;
// GLOBAL: SHANDALAR 0x008c7420
int g_game8c7420;
// STUB: SHANDALAR 0x004975e2
int Game_Callback4975e2(void) { return 0; }
// STUB: SHANDALAR 0x0044147b
int Game_Helper44147b(int a1, int a2, int a3) { return 0; }
// STUB: SHANDALAR 0x004be650
int Game_Helper4be650(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12, int a13, int a14, int a15, int a16, int a17, int a18, int a19) { return 0; }
// STUB: SHANDALAR 0x00558aea
int Game_Helper558aea(int a1, int a2) { return 0; }

/* Referenced by generated card scripts. */
// GLOBAL: SHANDALAR 0x007be9ac
int g_game7be9ac;
// GLOBAL: SHANDALAR 0x008aa734
int g_game8aa734;
// GLOBAL: SHANDALAR 0x008bce34
int g_game8bce34;
// GLOBAL: SHANDALAR 0x008bcff8
int g_game8bcff8;
// GLOBAL: SHANDALAR 0x008c6aa4
int g_game8c6aa4;
// GLOBAL: SHANDALAR 0x008c8274
int g_game8c8274;
// GLOBAL: SHANDALAR 0x008c83f0
int g_game8c83f0[32];
// GLOBAL: SHANDALAR 0x008c83f9
int g_game8c83f9;
// GLOBAL: SHANDALAR 0x008e3efc
int g_game8e3efc;
// GLOBAL: SHANDALAR 0x0094d45c
int g_game94d45c;
// GLOBAL: SHANDALAR 0x0094d474
int g_game94d474;
// GLOBAL: SHANDALAR 0x0097db34
int g_game97db34;
// STUB: SHANDALAR 0x0048592f
int Game_Callback48592f(void) { return 0; }
// STUB: SHANDALAR 0x00440d44
int Game_Helper440d44(int a1, int a2, int a3, int a4, int a5) { return 0; }
// STUB: SHANDALAR 0x0049f87b
int Game_Helper49f87b(int a1, int a2, int a3) { return 0; }
// STUB: SHANDALAR 0x0049fb64
int Game_Helper49fb64(int a1) { return 0; }
// STUB: SHANDALAR 0x004bea85
int Game_Helper4bea85(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12, int a13, int a14, int a15, int a16, int a17, int a18, int a19, int a20) { return 0; }
// STUB: SHANDALAR 0x004d1c03
int Game_Helper4d1c03(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x005218f3
int Game_Helper5218f3(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x0056c705
int Game_Helper56c705(int a1) { return 0; }
// STUB: SHANDALAR 0x0056cc4d
int Game_Helper56cc4d(int a1, int a2) { return 0; }

/* Referenced by generated card scripts. */
// GLOBAL: SHANDALAR 0x0077d180
int g_game77d180[32][8];
// GLOBAL: SHANDALAR 0x0078e970
int g_game78e970[32];
// GLOBAL: SHANDALAR 0x008a9410
int g_game8a9410[32][500];
// GLOBAL: SHANDALAR 0x008e3300
int g_game8e3300[32][8];
// GLOBAL: SHANDALAR 0x008e3320
int g_game8e3320[32][8];
// GLOBAL: SHANDALAR 0x0094d660
int g_game94d660[32][8];
// GLOBAL: SHANDALAR 0x0094d930
int g_game94d930[32][500];
// STUB: SHANDALAR 0x0044aa1f
int Game_Helper44aa1f(int a1, int a2, int a3) { return 0; }
// STUB: SHANDALAR 0x0049fda3
int Game_Helper49fda3(int a1, int a2, int a3) { return 0; }
// STUB: SHANDALAR 0x004a011a
int Game_Helper4a011a(int a1, int a2, int a3) { return 0; }
// STUB: SHANDALAR 0x004e25e6
int Game_Helper4e25e6(int a1, int a2, int a3) { return 0; }
// STUB: SHANDALAR 0x00522508
int Game_Helper522508(int a1) { return 0; }
// STUB: SHANDALAR 0x0056c1b5
int Game_Helper56c1b5(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x0056c4f7
int Game_Helper56c4f7(int a1, int a2) { return 0; }

/* Referenced by generated card scripts. */
// GLOBAL: SHANDALAR 0x008bd394
int g_game8bd394;
// GLOBAL: SHANDALAR 0x008c7e90
int g_game8c7e90;
// GLOBAL: SHANDALAR 0x008e291c
int g_game8e291c;
// GLOBAL: SHANDALAR 0x0093a864
int g_game93a864;
// STUB: SHANDALAR 0x004aea1d
int Game_Callback4aea1d(void) { return 0; }

/* Referenced by generated card scripts. */
// GLOBAL: SHANDALAR 0x007bf0e4
int g_game7bf0e4;
// GLOBAL: SHANDALAR 0x007bf4c4
int g_game7bf4c4;
// GLOBAL: SHANDALAR 0x007bfe7c
int g_game7bfe7c;
// GLOBAL: SHANDALAR 0x008aa730
int g_game8aa730;
// GLOBAL: SHANDALAR 0x008bcff0
int g_game8bcff0;
// GLOBAL: SHANDALAR 0x008e330c
int g_game8e330c;
// GLOBAL: SHANDALAR 0x008e3ed8
int g_game8e3ed8[32];
// GLOBAL: SHANDALAR 0x00939e64
int g_game939e64;
// GLOBAL: SHANDALAR 0x0094d46c
int g_game94d46c;
// STUB: SHANDALAR 0x0044ffd0
int Game_Callback44ffd0(void) { return 0; }
// STUB: SHANDALAR 0x004b03cd
int Game_Callback4b03cd(void) { return 0; }
// STUB: SHANDALAR 0x0040e3dc
int Game_Helper40e3dc(int a1, int a2, int a3, int a4, int a5) { return 0; }
// STUB: SHANDALAR 0x0041a252
int Game_Helper41a252(int a1, int a2, int a3) { return 0; }
// STUB: SHANDALAR 0x004412ec
int Game_Helper4412ec(int a1, int a2, int a3) { return 0; }
// STUB: SHANDALAR 0x00476297
int Game_Helper476297(int a1, int a2, int a3) { return 0; }
// STUB: SHANDALAR 0x00487ce0
int Game_Helper487ce0(int a1, int a2, int a3, int a4) { return 0; }
// STUB: SHANDALAR 0x004b8102
int Game_Helper4b8102(int a1, int a2, int a3, int a4) { return 0; }
// STUB: SHANDALAR 0x004c0efa
int Game_Helper4c0efa(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12, int a13, int a14, int a15, int a16, int a17, int a18, int a19, int a20) { return 0; }
// STUB: SHANDALAR 0x004c4181
int Game_Helper4c4181(void) { return 0; }
// STUB: SHANDALAR 0x004e5571
int Game_Helper4e5571(int a1, int a2, int a3, int a4, int a5) { return 0; }
// STUB: SHANDALAR 0x004e5bfd
int Game_Helper4e5bfd(int a1, int a2, int a3, int a4) { return 0; }
// STUB: SHANDALAR 0x004ea3e7
int Game_Helper4ea3e7(int a1, int a2, int a3, int a4) { return 0; }
// STUB: SHANDALAR 0x004ec5d7
int Game_Helper4ec5d7(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x005179e0
int Game_Helper5179e0(int a1, int a2, int a3, int a4) { return 0; }
// STUB: SHANDALAR 0x0051ef70
int Game_Helper51ef70(int a1, int a2, int a3, int a4) { return 0; }
// STUB: SHANDALAR 0x00522535
int Game_Helper522535(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x00555944
int Game_Helper555944(int a1, int a2, int a3, int a4) { return 0; }
// STUB: SHANDALAR 0x0057e820
int Game_Helper57e820(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x0057e826
int Game_Helper57e826(int a1, int a2) { return 0; }

/* Referenced by generated card scripts. */
// GLOBAL: SHANDALAR 0x008c72b4
int g_game8c72b4;
// GLOBAL: SHANDALAR 0x008e3edc
int g_game8e3edc;

/* Referenced by reconstructed card-layer helpers. */
// GLOBAL: SHANDALAR 0x00589a38
int g_game589a38;
// GLOBAL: SHANDALAR 0x005ae0a0
int g_game5ae0a0[32];
// GLOBAL: SHANDALAR 0x005b2200
int g_game5b2200[32];
// GLOBAL: SHANDALAR 0x005b2dd0
int g_game5b2dd0[32];
// GLOBAL: SHANDALAR 0x0074842c
int g_game74842c;
// GLOBAL: SHANDALAR 0x0074b868
int g_game74b868;
// GLOBAL: SHANDALAR 0x0074cdb4
int g_game74cdb4;
// GLOBAL: SHANDALAR 0x007be9e8
int g_game7be9e8[32];
// GLOBAL: SHANDALAR 0x007bea98
int g_game7bea98;
// GLOBAL: SHANDALAR 0x007bea9c
int g_game7bea9c;
// GLOBAL: SHANDALAR 0x007bf4c8
int g_game7bf4c8;
// GLOBAL: SHANDALAR 0x007bfe7a
int g_game7bfe7a;
// GLOBAL: SHANDALAR 0x008a940c
int g_game8a940c[32][500];
// GLOBAL: SHANDALAR 0x008bd38c
int g_game8bd38c;
// GLOBAL: SHANDALAR 0x008c6ae8
int g_game8c6ae8;
// GLOBAL: SHANDALAR 0x008c6aec
int g_game8c6aec;
// GLOBAL: SHANDALAR 0x008c8650
int g_game8c8650[32];
// GLOBAL: SHANDALAR 0x008e4060
int g_game8e4060;
// GLOBAL: SHANDALAR 0x00939ce8
int g_game939ce8;
// GLOBAL: SHANDALAR 0x00939d24
int g_game939d24;
// GLOBAL: SHANDALAR 0x0093a77c
int g_game93a77c[32];
// GLOBAL: SHANDALAR 0x0097f1c0
int g_game97f1c0[32];
// STUB: SHANDALAR 0x0048889d
int Game_Callback48889d(void) { return 0; }
// STUB: SHANDALAR 0x0040d0cb
int Game_Helper40d0cb(int a1, int a2, int a3, int a4) { return 0; }
// STUB: SHANDALAR 0x0040d146
int Game_Helper40d146(int a1, int a2, int a3, int a4, int a5, int a6) { return 0; }
// STUB: SHANDALAR 0x0040e1f5
int Game_Helper40e1f5(int a1) { return 0; }
// STUB: SHANDALAR 0x0040e646
int Game_Helper40e646(void) { return 0; }
// STUB: SHANDALAR 0x0040e6fc
int Game_Helper40e6fc(void) { return 0; }
// STUB: SHANDALAR 0x004105e0
int Game_Helper4105e0(int a1, int a2, int a3, int a4) { return 0; }
// STUB: SHANDALAR 0x00410f46
int Game_Helper410f46(int a1, int a2, int a3, int a4) { return 0; }
// STUB: SHANDALAR 0x00419dc4
int Game_Helper419dc4(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x00450813
int Game_Helper450813(int a1, int a2, int a3, int a4) { return 0; }
// STUB: SHANDALAR 0x00451c24
int Game_Helper451c24(int a1) { return 0; }
// STUB: SHANDALAR 0x0046e159
int Game_Helper46e159(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x004884c1
int Game_Helper4884c1(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x00488638
int Game_Helper488638(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x00488789
int Game_Helper488789(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x004c5bf6
int Game_Helper4c5bf6(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x0053c35e
int Game_Helper53c35e(int a1, int a2, int a3) { return 0; }
// STUB: SHANDALAR 0x0053cb05
int Game_Helper53cb05(int a1, int a2, int a3, int a4, int a5) { return 0; }
// STUB: SHANDALAR 0x005583c5
int Game_Helper5583c5(int a1, int a2, int a3, int a4, int a5, int a6) { return 0; }
// STUB: SHANDALAR 0x0055840e
int Game_Helper55840e(int a1, int a2, int a3, int a4, int a5, int a6) { return 0; }
// STUB: SHANDALAR 0x0056bfa4
int Game_Helper56bfa4(int a1) { return 0; }

/* Referenced by generated card scripts. */
// GLOBAL: SHANDALAR 0x0077d160
int g_game77d160[32][8];
// GLOBAL: SHANDALAR 0x007a01dc
int g_game7a01dc;
// GLOBAL: SHANDALAR 0x007bfe78
int g_game7bfe78;
// GLOBAL: SHANDALAR 0x008c824c
int g_game8c824c;
// GLOBAL: SHANDALAR 0x008e3ed4
int g_game8e3ed4;
// GLOBAL: SHANDALAR 0x0094d650
int g_game94d650[32][8];
// GLOBAL: SHANDALAR 0x0097c85c
int g_game97c85c;
// STUB: SHANDALAR 0x004d3f22
int Game_Callback4d3f22(void) { return 0; }
// STUB: SHANDALAR 0x004f9d1d
int Game_Callback4f9d1d(void) { return 0; }
// STUB: SHANDALAR 0x0040f77c
int Game_Helper40f77c(int a1, int a2, int a3, int a4, int a5) { return 0; }
// STUB: SHANDALAR 0x00474721
int Game_Helper474721(int a1, int a2, int a3, int a4) { return 0; }
// STUB: SHANDALAR 0x004ba5a1
int Game_Helper4ba5a1(int a1, int a2, int a3, int a4, int a5) { return 0; }
// STUB: SHANDALAR 0x0056ae5d
int Game_Helper56ae5d(int a1, int a2) { return 0; }

/* Referenced by reconstructed card-layer helpers. */
// GLOBAL: SHANDALAR 0x007bfee4
int g_game7bfee4;
// GLOBAL: SHANDALAR 0x008c83f4
int g_game8c83f4;
// STUB: SHANDALAR 0x0046dbed
int Game_Callback46dbed(void) { return 0; }
// STUB: SHANDALAR 0x0046dbed
int Game_Helper46dbed(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x00485eb8
int Game_Helper485eb8(int a1, int a2) { return 0; }

/* Referenced by generated card scripts. */
// GLOBAL: SHANDALAR 0x008c7d54
int g_game8c7d54;
// GLOBAL: SHANDALAR 0x008c8b44
int g_game8c8b44;
// GLOBAL: SHANDALAR 0x008e3104
int g_game8e3104;
// STUB: SHANDALAR 0x00477b57
int Game_Callback477b57(void) { return 0; }

/* Referenced by reconstructed card-layer helpers. */
// GLOBAL: SHANDALAR 0x008e2e10
int g_game8e2e10[32][16];

/* Exported by Shandalar.exe; the deck builder reads and writes it
 * through SetCardInDeck. Indexed by card type. */
int deck[4096];

/* Referenced by reconstructed ordinary game functions. */
// GLOBAL: SHANDALAR 0x00580d80
int g_game580d80;
// GLOBAL: SHANDALAR 0x00580dac
int g_game580dac;
// GLOBAL: SHANDALAR 0x00581804
int g_game581804;
// GLOBAL: SHANDALAR 0x00581ea8
int g_game581ea8;
// GLOBAL: SHANDALAR 0x005832b4
int g_game5832b4;
// GLOBAL: SHANDALAR 0x005832dc
int g_game5832dc;
// GLOBAL: SHANDALAR 0x00583304
int g_game583304;
// GLOBAL: SHANDALAR 0x00583a74
int g_game583a74;
// GLOBAL: SHANDALAR 0x005862d8
int g_game5862d8[32];
// GLOBAL: SHANDALAR 0x00586340
int g_game586340[32];
// GLOBAL: SHANDALAR 0x005863b0
int g_game5863b0;
// GLOBAL: SHANDALAR 0x005863b8
int g_game5863b8;
// GLOBAL: SHANDALAR 0x005863bc
int g_game5863bc;
// GLOBAL: SHANDALAR 0x005863c0
int g_game5863c0[32][4];
// GLOBAL: SHANDALAR 0x00586494
int g_game586494;
// GLOBAL: SHANDALAR 0x00587590
int g_game587590[32][4];
// GLOBAL: SHANDALAR 0x005875a0
int g_game5875a0[32][4];
// GLOBAL: SHANDALAR 0x00589df0
int g_game589df0;
// GLOBAL: SHANDALAR 0x0058b584
int g_game58b584;
// GLOBAL: SHANDALAR 0x0058b954
int g_game58b954;
// GLOBAL: SHANDALAR 0x0058b958
int g_game58b958;
// GLOBAL: SHANDALAR 0x0058c03f
int g_game58c03f;
// GLOBAL: SHANDALAR 0x005911f8
int g_game5911f8;
// GLOBAL: SHANDALAR 0x005911fc
int g_game5911fc;
// GLOBAL: SHANDALAR 0x005a1814
int g_game5a1814;
// GLOBAL: SHANDALAR 0x005a6074
int g_game5a6074;
// GLOBAL: SHANDALAR 0x005a6078
int g_game5a6078;
// GLOBAL: SHANDALAR 0x005a6080
int g_game5a6080[32][7];
// GLOBAL: SHANDALAR 0x005a7520
int g_game5a7520;
// GLOBAL: SHANDALAR 0x005a7c08
int g_game5a7c08[32][16];
// GLOBAL: SHANDALAR 0x005a8b30
int g_game5a8b30;
// GLOBAL: SHANDALAR 0x005aa470
int g_game5aa470;
// GLOBAL: SHANDALAR 0x005aa690
int g_game5aa690;
// GLOBAL: SHANDALAR 0x005aae78
int g_game5aae78;
// GLOBAL: SHANDALAR 0x005ace50
int g_game5ace50;
// GLOBAL: SHANDALAR 0x005acff0
int g_game5acff0;
// GLOBAL: SHANDALAR 0x005ad038
int g_game5ad038;
// GLOBAL: SHANDALAR 0x005ae4a0
int g_game5ae4a0[32];
// GLOBAL: SHANDALAR 0x005ae8a0
int g_game5ae8a0;
// GLOBAL: SHANDALAR 0x005b0e68
int g_game5b0e68;
// GLOBAL: SHANDALAR 0x005b0e90
int g_game5b0e90;
// GLOBAL: SHANDALAR 0x005b1000
int g_game5b1000;
// GLOBAL: SHANDALAR 0x005b1180
int g_game5b1180;
// GLOBAL: SHANDALAR 0x005b1184
int g_game5b1184;
// GLOBAL: SHANDALAR 0x005b1190
int g_game5b1190;
// GLOBAL: SHANDALAR 0x005b21f8
int g_game5b21f8;
// GLOBAL: SHANDALAR 0x005b31d0
int g_game5b31d0[32];
// GLOBAL: SHANDALAR 0x005b35d4
int g_game5b35d4;
// GLOBAL: SHANDALAR 0x005b3638
int g_game5b3638;
// GLOBAL: SHANDALAR 0x005b3e90
int g_game5b3e90;
// GLOBAL: SHANDALAR 0x005b3e94
int g_game5b3e94;
// GLOBAL: SHANDALAR 0x005b3e98
int g_game5b3e98;
// GLOBAL: SHANDALAR 0x005b3e9c
int g_game5b3e9c;
// GLOBAL: SHANDALAR 0x005b4140
int g_game5b4140[32];
// GLOBAL: SHANDALAR 0x005b4540
int g_game5b4540[32];
// GLOBAL: SHANDALAR 0x005b4940
int g_game5b4940;
// GLOBAL: SHANDALAR 0x005b4948
int g_game5b4948[32];
// GLOBAL: SHANDALAR 0x005b4d48
int g_game5b4d48;
// GLOBAL: SHANDALAR 0x005b4d4c
int g_game5b4d4c;
// GLOBAL: SHANDALAR 0x005b4d50
int g_game5b4d50;
// GLOBAL: SHANDALAR 0x005b5d00
int g_game5b5d00;
// GLOBAL: SHANDALAR 0x005b5e10
int g_game5b5e10;
// GLOBAL: SHANDALAR 0x005b6618
int g_game5b6618;
// GLOBAL: SHANDALAR 0x005b66a8
int g_game5b66a8;
// GLOBAL: SHANDALAR 0x005b7650
int g_game5b7650;
// GLOBAL: SHANDALAR 0x005b7b6c
int g_game5b7b6c;
// GLOBAL: SHANDALAR 0x005b80e0
int g_game5b80e0[32];
// GLOBAL: SHANDALAR 0x005b8138
int g_game5b8138;
// GLOBAL: SHANDALAR 0x00602fb4
int g_game602fb4;
// GLOBAL: SHANDALAR 0x00604420
int g_game604420;
// GLOBAL: SHANDALAR 0x006506f0
int g_game6506f0;
// GLOBAL: SHANDALAR 0x006506f4
int g_game6506f4;
// GLOBAL: SHANDALAR 0x00650700
int g_game650700[32];
// GLOBAL: SHANDALAR 0x00650f40
int g_game650f40[32];
// GLOBAL: SHANDALAR 0x00651740
int g_game651740[32];
// GLOBAL: SHANDALAR 0x00651f40
int g_game651f40[32];
// GLOBAL: SHANDALAR 0x006527c0
int g_game6527c0[32];
// GLOBAL: SHANDALAR 0x00669390
int g_game669390;
// GLOBAL: SHANDALAR 0x006696f8
int g_game6696f8;
// GLOBAL: SHANDALAR 0x0073226c
int g_game73226c;
// GLOBAL: SHANDALAR 0x0073bfa0
int g_game73bfa0[32];
// GLOBAL: SHANDALAR 0x0073e9d0
int g_game73e9d0;
// GLOBAL: SHANDALAR 0x0073e9d4
int g_game73e9d4;
// GLOBAL: SHANDALAR 0x00746ef0
int g_game746ef0[32];
// GLOBAL: SHANDALAR 0x00746f40
int g_game746f40[32][50];
// GLOBAL: SHANDALAR 0x00747ee0
int g_game747ee0;
// GLOBAL: SHANDALAR 0x00748408
int g_game748408;
// GLOBAL: SHANDALAR 0x0074840c
int g_game74840c;
// GLOBAL: SHANDALAR 0x00748428
int g_game748428;
// GLOBAL: SHANDALAR 0x0074cfe4
int g_game74cfe4;
// GLOBAL: SHANDALAR 0x00765e24
int g_game765e24;
// GLOBAL: SHANDALAR 0x0077c020
int g_game77c020[32];
// GLOBAL: SHANDALAR 0x0077e58c
int g_game77e58c;
// GLOBAL: SHANDALAR 0x00780820
int g_game780820[32];
// GLOBAL: SHANDALAR 0x007817f0
int g_game7817f0[32][4];
// GLOBAL: SHANDALAR 0x007894f0
int g_game7894f0;
// GLOBAL: SHANDALAR 0x007898f0
int g_game7898f0;
// GLOBAL: SHANDALAR 0x007898f4
int g_game7898f4;
// GLOBAL: SHANDALAR 0x007898f8
int g_game7898f8;
// GLOBAL: SHANDALAR 0x00789904
int g_game789904;
// GLOBAL: SHANDALAR 0x00789930
int g_game789930;
// GLOBAL: SHANDALAR 0x00789938
int g_game789938;
// GLOBAL: SHANDALAR 0x0078cee4
int g_game78cee4;
// GLOBAL: SHANDALAR 0x0078cefc
int g_game78cefc;
// GLOBAL: SHANDALAR 0x0078df68
int g_game78df68;
// GLOBAL: SHANDALAR 0x0079ff88
int g_game79ff88;
// GLOBAL: SHANDALAR 0x007a00a0
int g_game7a00a0;
// GLOBAL: SHANDALAR 0x007a090c
int g_game7a090c;
// GLOBAL: SHANDALAR 0x007a09c4
int g_game7a09c4;
// GLOBAL: SHANDALAR 0x008bd344
int g_game8bd344;
// GLOBAL: SHANDALAR 0x008bd35c
int g_game8bd35c;
// GLOBAL: SHANDALAR 0x008bd370
int g_game8bd370;
// GLOBAL: SHANDALAR 0x008bd374
int g_game8bd374;
// GLOBAL: SHANDALAR 0x008c8b50
int g_game8c8b50[32];
// GLOBAL: SHANDALAR 0x008c8b51
int g_game8c8b51[32];
// GLOBAL: SHANDALAR 0x008c8b52
int g_game8c8b52[32];
// GLOBAL: SHANDALAR 0x008e0b00
int g_game8e0b00[32][500];
// GLOBAL: SHANDALAR 0x008e1aa4
int g_game8e1aa4;
// GLOBAL: SHANDALAR 0x00939ce0
int g_game939ce0;
// GLOBAL: SHANDALAR 0x00939ce4
int g_game939ce4;
// GLOBAL: SHANDALAR 0x0093a780
int g_game93a780;
// GLOBAL: SHANDALAR 0x0093a784
int g_game93a784;
// GLOBAL: SHANDALAR 0x0093a788
int g_game93a788;
// GLOBAL: SHANDALAR 0x0093a78c
int g_game93a78c;
// GLOBAL: SHANDALAR 0x0093a790
int g_game93a790;
// GLOBAL: SHANDALAR 0x0093a938
int g_game93a938;
// GLOBAL: SHANDALAR 0x0094d2ac
int g_game94d2ac[32];
// GLOBAL: SHANDALAR 0x0094d634
int g_game94d634;
// GLOBAL: SHANDALAR 0x0094eab0
int g_game94eab0[32];
// GLOBAL: SHANDALAR 0x009523f0
int g_game9523f0[32];
// GLOBAL: SHANDALAR 0x0097f1c1
int g_game97f1c1[32];
// GLOBAL: SHANDALAR 0x00982540
int g_game982540;
// GLOBAL: SHANDALAR 0x00986d94
int g_game986d94;
// GLOBAL: SHANDALAR 0x00986d98
int g_game986d98;
// GLOBAL: SHANDALAR 0x00986d9c
int g_game986d9c;
// STUB: SHANDALAR 0x004c8bec
int Game_Callback4c8bec(void) { return 0; }
// STUB: SHANDALAR 0x0040a62b
int Game_Helper40a62b(int a1, int a2, int a3) { return 0; }
// STUB: SHANDALAR 0x0040b26d
int Game_Helper40b26d(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x00412c37
int Game_Helper412c37(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x00417dc6
int Game_Helper417dc6(int a1) { return 0; }
// STUB: SHANDALAR 0x004261ae
int Game_Helper4261ae(int a1) { return 0; }
// STUB: SHANDALAR 0x0042811a
int Game_Helper42811a(int a1, int a2, int a3, int a4) { return 0; }
// STUB: SHANDALAR 0x00428615
int Game_Helper428615(int a1) { return 0; }
// STUB: SHANDALAR 0x0042fe00
int Game_Helper42fe00(int a1) { return 0; }
// STUB: SHANDALAR 0x00430ef4
int Game_Helper430ef4(int a1, int a2, int a3, int a4) { return 0; }
// STUB: SHANDALAR 0x0043104f
int Game_Helper43104f(int a1, int a2, int a3, int a4) { return 0; }
// STUB: SHANDALAR 0x004310e8
int Game_Helper4310e8(int a1) { return 0; }
// STUB: SHANDALAR 0x00442f6a
int Game_Helper442f6a(void) { return 0; }
// STUB: SHANDALAR 0x004431d8
int Game_Helper4431d8(void) { return 0; }
// STUB: SHANDALAR 0x004432ff
int Game_Helper4432ff(void) { return 0; }
// STUB: SHANDALAR 0x00450110
int Game_Helper450110(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x00450925
int Game_Helper450925(int a1, int a2, int a3, int a4, int a5, int a6) { return 0; }
// STUB: SHANDALAR 0x00452355
int Game_Helper452355(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x00452ad9
int Game_Helper452ad9(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x00453b6c
int Game_Helper453b6c(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x00453f42
int Game_Helper453f42(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x0045cba4
int Game_Helper45cba4(int a1) { return 0; }
// STUB: SHANDALAR 0x0046297d
int Game_Helper46297d(void) { return 0; }
// STUB: SHANDALAR 0x0046488b
int Game_Helper46488b(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x00464c8a
int Game_Helper464c8a(int a1, int a2, int a3) { return 0; }
// STUB: SHANDALAR 0x0046537b
int Game_Helper46537b(int a1) { return 0; }
// STUB: SHANDALAR 0x00465764
int Game_Helper465764(void) { return 0; }
// STUB: SHANDALAR 0x004660d6
int Game_Helper4660d6(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8) { return 0; }
// STUB: SHANDALAR 0x00468365
int Game_Helper468365(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x00468498
int Game_Helper468498(int a1) { return 0; }
// STUB: SHANDALAR 0x0046933c
int Game_Helper46933c(int a1) { return 0; }
// STUB: SHANDALAR 0x0046a674
int Game_Helper46a674(int a1, int a2, int a3, int a4) { return 0; }
// STUB: SHANDALAR 0x0046ed33
int Game_Helper46ed33(void) { return 0; }
// STUB: SHANDALAR 0x0046ef07
int Game_Helper46ef07(int a1) { return 0; }
// STUB: SHANDALAR 0x004a8da8
int Game_Helper4a8da8(int a1) { return 0; }
// STUB: SHANDALAR 0x004bac17
int Game_Helper4bac17(int a1) { return 0; }
// STUB: SHANDALAR 0x004c19a2
int Game_Helper4c19a2(int a1) { return 0; }
// STUB: SHANDALAR 0x004c6c10
int Game_Helper4c6c10(int a1) { return 0; }
// STUB: SHANDALAR 0x004c8f49
int Game_Helper4c8f49(int a1) { return 0; }
// STUB: SHANDALAR 0x004cc0c1
int Game_Helper4cc0c1(int a1) { return 0; }
// STUB: SHANDALAR 0x004ccd9e
int Game_Helper4ccd9e(int a1) { return 0; }
// STUB: SHANDALAR 0x004cea4c
int Game_Helper4cea4c(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x004ecfa2
int Game_Helper4ecfa2(void) { return 0; }
// STUB: SHANDALAR 0x004ed005
int Game_Helper4ed005(void) { return 0; }
// STUB: SHANDALAR 0x004f8400
int Game_Helper4f8400(int a1) { return 0; }
// STUB: SHANDALAR 0x00501b7d
int Game_Helper501b7d(void) { return 0; }
// STUB: SHANDALAR 0x005020fe
int Game_Helper5020fe(int a1) { return 0; }
// STUB: SHANDALAR 0x005044ac
int Game_Helper5044ac(int a1) { return 0; }
// STUB: SHANDALAR 0x00504be8
int Game_Helper504be8(int a1) { return 0; }
// STUB: SHANDALAR 0x0050d809
int Game_Helper50d809(int a1) { return 0; }
// STUB: SHANDALAR 0x0053a054
int Game_Helper53a054(int a1, int a2, int a3, int a4, int a5, int a6, int a7) { return 0; }
// STUB: SHANDALAR 0x00550dba
int Game_Helper550dba(int a1) { return 0; }
// STUB: SHANDALAR 0x00556385
int Game_Helper556385(int a1) { return 0; }
// STUB: SHANDALAR 0x005579fc
int Game_Helper5579fc(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x00559dcd
int Game_Helper559dcd(void) { return 0; }
// STUB: SHANDALAR 0x00559ff4
int Game_Helper559ff4(int a1) { return 0; }
// STUB: SHANDALAR 0x0055a13b
int Game_Helper55a13b(int a1) { return 0; }
// STUB: SHANDALAR 0x00562f92
int Game_Helper562f92(int a1, int a2, int a3) { return 0; }
// STUB: SHANDALAR 0x00564e70
int Game_Helper564e70(int a1, int a2, int a3, int a4, int a5) { return 0; }
// STUB: SHANDALAR 0x0056a515
int Game_Helper56a515(int a1, int a2, int a3, int a4, int a5, int a6) { return 0; }
// STUB: SHANDALAR 0x0056d133
int Game_Helper56d133(int a1) { return 0; }
// STUB: SHANDALAR 0x0056d167
int Game_Helper56d167(void) { return 0; }
// STUB: SHANDALAR 0x0056d194
int Game_Helper56d194(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x0056d4b0
int Game_Helper56d4b0(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x0056dc9c
int Game_Helper56dc9c(int a1) { return 0; }
// STUB: SHANDALAR 0x0056f8c1
int Game_Helper56f8c1(void) { return 0; }
// STUB: SHANDALAR 0x0057378d
int Game_Helper57378d(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x0057456b
int Game_Helper57456b(int a1) { return 0; }
// STUB: SHANDALAR 0x00578c04
int Game_Helper578c04(void) { return 0; }
// STUB: SHANDALAR 0x00578c40
int Game_Helper578c40(void) { return 0; }
// STUB: SHANDALAR 0x00578c60
int Game_Helper578c60(void) { return 0; }
// STUB: SHANDALAR 0x00579240
int Game_Helper579240(int a1) { return 0; }
// STUB: SHANDALAR 0x005795f0
int Game_Helper5795f0(int a1, int a2, int a3) { return 0; }
// STUB: SHANDALAR 0x005796c0
int Game_Helper5796c0(int a1, int a2, int a3, int a4, int a5, int a6) { return 0; }
// STUB: SHANDALAR 0x00579760
int Game_Helper579760(int a1, int a2, int a3, int a4) { return 0; }
// STUB: SHANDALAR 0x005797e0
int Game_Helper5797e0(int a1, int a2, int a3, int a4, int a5, int a6) { return 0; }
// STUB: SHANDALAR 0x00579890
int Game_Helper579890(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8) { return 0; }
// STUB: SHANDALAR 0x00579bf0
int Game_Helper579bf0(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8) { return 0; }
// STUB: SHANDALAR 0x00579e40
int Game_Helper579e40(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) { return 0; }
// STUB: SHANDALAR 0x0057a060
int Game_Helper57a060(int a1) { return 0; }
// STUB: SHANDALAR 0x0057bfa0
int Game_Helper57bfa0(int a1, int a2, int a3, int a4, int a5, int a6) { return 0; }
// STUB: SHANDALAR 0x0057c580
int Game_Helper57c580(int a1, int a2, int a3, int a4, int a5) { return 0; }
// STUB: SHANDALAR 0x0057c7e0
int Game_Helper57c7e0(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x0057c820
int Game_Helper57c820(int a1) { return 0; }
// STUB: SHANDALAR 0x0057e832
int Game_Helper57e832(int a1) { return 0; }
// STUB: SHANDALAR 0x0057e83e
int Game_Helper57e83e(int a1, int a2, int a3) { return 0; }

/* Referenced by reconstructed ordinary game functions. */
// GLOBAL: SHANDALAR 0x0058332c
int g_game58332c;
// GLOBAL: SHANDALAR 0x00583354
int g_game583354;
// GLOBAL: SHANDALAR 0x005863ac
int g_game5863ac;
// GLOBAL: SHANDALAR 0x00589de8
int g_game589de8;
// GLOBAL: SHANDALAR 0x00590764
int g_game590764;
// GLOBAL: SHANDALAR 0x00590768
int g_game590768;
// GLOBAL: SHANDALAR 0x0059121c
int g_game59121c;
// GLOBAL: SHANDALAR 0x00591220
int g_game591220;
// GLOBAL: SHANDALAR 0x0059126c
int g_game59126c;
// GLOBAL: SHANDALAR 0x00591278
int g_game591278[32];
// GLOBAL: SHANDALAR 0x00593c88
int g_game593c88[32];
// GLOBAL: SHANDALAR 0x00650f28
int g_game650f28;
// GLOBAL: SHANDALAR 0x006527b0
int g_game6527b0;
// GLOBAL: SHANDALAR 0x0073eaa0
int g_game73eaa0;
// GLOBAL: SHANDALAR 0x00746af8
int g_game746af8;
// GLOBAL: SHANDALAR 0x00746afc
int g_game746afc;
// GLOBAL: SHANDALAR 0x00746b00
int g_game746b00;
// GLOBAL: SHANDALAR 0x007481d8
int g_game7481d8;
// GLOBAL: SHANDALAR 0x00749410
int g_game749410;
// GLOBAL: SHANDALAR 0x00781728
int g_game781728;
// GLOBAL: SHANDALAR 0x0078172c
int g_game78172c;
// GLOBAL: SHANDALAR 0x00781730
int g_game781730[32];
// GLOBAL: SHANDALAR 0x00781780
int g_game781780[32];
// GLOBAL: SHANDALAR 0x00789900
int g_game789900;
// GLOBAL: SHANDALAR 0x0078e82c
int g_game78e82c;
// GLOBAL: SHANDALAR 0x008aa72c
int g_game8aa72c;
// GLOBAL: SHANDALAR 0x008bcf74
int g_game8bcf74;
// GLOBAL: SHANDALAR 0x008c7e8c
int g_game8c7e8c;
// GLOBAL: SHANDALAR 0x008e09c0
int g_game8e09c0;
// GLOBAL: SHANDALAR 0x008e448c
int g_game8e448c;
// GLOBAL: SHANDALAR 0x00930640
int g_game930640;
// GLOBAL: SHANDALAR 0x00930644
int g_game930644;
// GLOBAL: SHANDALAR 0x00951970
int g_game951970;
// STUB: SHANDALAR 0x004a04a0
int Game_Helper4a04a0(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x004a2cd1
int Game_Helper4a2cd1(int a1, int a2, int a3, int a4) { return 0; }
// STUB: SHANDALAR 0x004c357e
int Game_Helper4c357e(void) { return 0; }
// STUB: SHANDALAR 0x004f7fb9
int Game_Helper4f7fb9(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x0050027e
int Game_Helper50027e(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x005040da
int Game_Helper5040da(int a1) { return 0; }
// STUB: SHANDALAR 0x0050425c
int Game_Helper50425c(int a1) { return 0; }
// STUB: SHANDALAR 0x0053a265
int Game_Helper53a265(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10) { return 0; }
// STUB: SHANDALAR 0x0053bc1a
int Game_Helper53bc1a(int a1, int a2, int a3, int a4, int a5, int a6) { return 0; }
// STUB: SHANDALAR 0x0053c19a
int Game_Helper53c19a(int a1, int a2, int a3) { return 0; }
// STUB: SHANDALAR 0x00544e8f
int Game_Helper544e8f(void) { return 0; }
// STUB: SHANDALAR 0x005472f1
int Game_Helper5472f1(void) { return 0; }
// STUB: SHANDALAR 0x005474e1
int Game_Helper5474e1(void) { return 0; }
// STUB: SHANDALAR 0x00557893
int Game_Helper557893(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x00559999
int Game_Helper559999(void) { return 0; }
// STUB: SHANDALAR 0x00559e3d
int Game_Helper559e3d(void) { return 0; }
// STUB: SHANDALAR 0x0056da03
int Game_Helper56da03(int a1) { return 0; }
// STUB: SHANDALAR 0x00577b8f
int Game_Helper577b8f(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x00578c70
int Game_Helper578c70(int a1, int a2, int a3) { return 0; }
// STUB: SHANDALAR 0x0057b650
int Game_Helper57b650(void) { return 0; }
// STUB: SHANDALAR 0x0057b670
int Game_Helper57b670(void) { return 0; }
// STUB: SHANDALAR 0x0057c800
int Game_Helper57c800(int a1, int a2) { return 0; }
// STUB: SHANDALAR 0x0057c840
int Game_Helper57c840(int a1, int a2) { return 0; }
