/* sidlib - MicroProse in-house 2D engine: installing a palette.
 *
 * Sid_SetPalette (0x0057a060) has its own translation unit here rather than
 * living in lib.c, and that is a measured conclusion rather than tidiness.
 * Putting it in lib.c means lib.c sees these five declarations, and that alone
 * takes Sid_SetPixel from byte-exact to 33% -- so the original's lib.c cannot
 * have had them in scope, and therefore cannot have contained this function.
 *
 * Which unit it really belongs to is still not established. What is
 * established is that it is not lib.c.
 *
 * ONE HEADER PER MODULE, DELIBERATELY -- see the note in sidcore.h.
 */

#ifndef SIDLIB_PALETTE_H
#define SIDLIB_PALETTE_H

#include "sidlib/sidcore.h"

/* Sid_SetPalette copies the whole incoming block here before unpacking it.
 * Read by seventeen call sites outside sidlib. */
extern unsigned char g_sidPaletteRaw[1024];
/* Five bytes written by the tail; base and purpose both unestablished. */
extern unsigned char g_sidUnknown986d80[16];
/* The external palette-viewer window, found by class name on every change. */
extern void *g_sidPaletteWindow;
/* The two accepted format tags, compared as 16-bit values. */
extern char g_sidTagM1[];
extern char g_sidTagM0[];

/* What this module reaches into lib.c for. Kept character-identical to the
 * declarations in sidgfx.h and sidcore.h. */
typedef struct SidRgb SidRgb;
extern SidRgb g_sidPaletteRgb[256];
extern unsigned char g_sidDibPalette[256 * 4];
extern int g_sidScreenBpp;
extern void *g_sidPalette;
extern int g_sidLinePaletteDirty;

/* FUNCTION: SHANDALAR 0x0057a060 */
void Sid_SetPalette(const void *block);

#endif /* SIDLIB_PALETTE_H */
