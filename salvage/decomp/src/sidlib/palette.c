/* sidlib: installing a palette. See sidpalette.h for why this is its own
 * translation unit.
 */

#include <string.h>
#include <windows.h>

#include "sidlib/sidgfx.h"
#include "sidlib/sidpalette.h"

/* Installs a palette block: the format Sid_OpenFileCore stamps and Pcx_ReadHeader
 * fills. Two tags are accepted and they differ only in scale -- "M0" carries
 * six-bit VGA components that are widened by 255/63, "M1" carries eight-bit
 * components used as they are. A block with any other tag updates nothing but
 * still runs the tail, so the hardware palette is re-realised regardless.
 *
 * At 16bpp every component is masked to 0xf8, dropping the low three bits,
 * because that is all the display can show and later lookups compare masked
 * values (see the note on g_sidScreenBpp).
 *
 * Two destinations are filled in parallel: g_sidPaletteRgb in RGB order for
 * GDI, and g_sidDibPalette in BGR order for SetDIBColorTable.
 *
 * Entries that come out pure white are nudged down to 0xfe per component --
 * except index 255, which is allowed to stay white. See docs/quirks.md Q-020. */
// FUNCTION: SHANDALAR 0x0057a060
void Sid_SetPalette(const void *block)
{
    const unsigned char *b;
    const unsigned char *src;
    int mask;
    int first;
    int last;
    int i;
    int n;
    int tag;
    int white;

    b = (const unsigned char *)block;
    memcpy(g_sidPaletteRaw, b, (short)(*(const unsigned short *)(b + 2) + 2));

    tag = *(const short *)b;
    mask = (g_sidScreenBpp == 16) ? 0xf8 : 0xff;
    first = b[4];
    last = b[5];
    src = b + 6;

    if (*(const short *)g_sidTagM0 == tag) {
        if (last >= first) {
            src += first * 3;
            n = last - first + 1;
            i = first;
            do {
                g_sidPaletteRgb[i].r = (unsigned char)((src[0] * 255 / 63) & mask);
                g_sidDibPalette[i * 4 + 2] = g_sidPaletteRgb[i].r;
                g_sidPaletteRgb[i].g = (unsigned char)((src[1] * 255 / 63) & mask);
                g_sidDibPalette[i * 4 + 1] = g_sidPaletteRgb[i].g;
                g_sidPaletteRgb[i].b = (unsigned char)((src[2] * 255 / 63) & mask);
                g_sidDibPalette[i * 4 + 0] = g_sidPaletteRgb[i].b;
                g_sidPaletteRgb[i].pad = 1;
                g_sidDibPalette[i * 4 + 3] = 0;

                if (*(int *)&g_sidDibPalette[i * 4] == 0xffffff && i != 255) {
                    white = (mask << 16) | (mask << 8) | mask;
                    *(int *)&g_sidPaletteRgb[i] = white & 0x1fefefe;
                    *(int *)&g_sidDibPalette[i * 4] = white & 0xfefefe;
                }
                i++;
                src += 3;
                n--;
            } while (n != 0);
        }
    } else if (*(const short *)g_sidTagM1 == tag) {
        if (last >= first) {
            src += first * 3;
            n = last - first + 1;
            i = first;
            do {
                g_sidPaletteRgb[i].r = (unsigned char)(src[0] & mask);
                g_sidDibPalette[i * 4 + 2] = g_sidPaletteRgb[i].r;
                g_sidPaletteRgb[i].g = (unsigned char)(src[1] & mask);
                g_sidDibPalette[i * 4 + 1] = g_sidPaletteRgb[i].g;
                g_sidPaletteRgb[i].b = (unsigned char)(src[2] & mask);
                g_sidDibPalette[i * 4 + 0] = g_sidPaletteRgb[i].b;
                g_sidPaletteRgb[i].pad = 1;
                g_sidDibPalette[i * 4 + 3] = 0;

                if (*(int *)&g_sidDibPalette[i * 4] == 0xffffff && i != 255) {
                    white = (mask << 16) | (mask << 8) | mask;
                    white &= 0xfefefe;
                    *(int *)&g_sidDibPalette[i * 4] = white;
                    *(int *)&g_sidPaletteRgb[i] = white;
                    g_sidPaletteRgb[i].pad = 1;
                }
                i++;
                src += 3;
                n--;
            } while (n != 0);
        }
    }

    /* Index 0 is forced to black, and the top three entries have their flag
     * bytes set by hand. Why 253 and 254 are flagged and 255 is not is not
     * established. */
    g_sidPaletteRgb[0].b = 0;
    g_sidPaletteRgb[0].g = 0;
    g_sidPaletteRgb[0].r = 0;
    g_sidPaletteRgb[0].pad = 0;
    g_sidDibPalette[0] = 0;
    g_sidDibPalette[1] = 0;
    g_sidDibPalette[2] = 0;
    g_sidDibPalette[3] = 0;
    g_sidPaletteRgb[253].pad = 1;
    g_sidPaletteRgb[254].pad = 1;
    g_sidPaletteRgb[255].pad = 0;

    g_sidUnknown986d80[7] = 0;
    g_sidUnknown986d80[12] = 0xff;
    g_sidUnknown986d80[13] = 0xff;
    g_sidUnknown986d80[14] = 0xff;
    g_sidUnknown986d80[15] = 0;

    AnimatePalette((HPALETTE)g_sidPalette, 0, 256, (const PALETTEENTRY *)g_sidPaletteRgb);

    if (g_sidPages[0] != NULL) {
        RealizePalette((HDC)g_sidPages[0]->hdc);
    }
    for (i = 1; i < SID_MAX_PAGES; i++) {
        if (g_sidPages[i] != NULL) {
            SetDIBColorTable((HDC)g_sidPages[i]->hdc, 0, 256, (const RGBQUAD *)g_sidDibPalette);
        }
    }

    // STRING: SHANDALAR 0x005a170c
    // STRING: SHANDALAR 0x005a1720
    g_sidPaletteWindow = FindWindowExA(NULL, NULL, "ShowPaletteClass", "Current Palette");
    if (g_sidPaletteWindow != NULL) {
        InvalidateRect((HWND)g_sidPaletteWindow, NULL, FALSE);
        UpdateWindow((HWND)g_sidPaletteWindow);
    }

    g_sidLinePaletteDirty = 1;
}

