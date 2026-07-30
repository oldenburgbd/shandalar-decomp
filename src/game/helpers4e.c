/* Shared helpers invoked by the card scripts; reconstructed from the
 * disassembly by helpergen and checked by reccmp score. Names are raw
 * addresses because no meaning has been established yet. */
#include "game/helpers.h"

/* used by 3 card scripts */
// FUNCTION: SHANDALAR 0x004e4fa0
int Game_Helper4e4fa0(int a1, int a2, int a3, int a4)
{
    if (a3 == 0x74) {
        Game_Helper4be650(0, 0, a1, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a1, a2), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (a3 == 0x6c
            && g_game951bdc == a2
            && g_game8e1aa8 == a1) {
            if (Game_Helper49f87b(a1, a1, a2) == 0) {
                g_game8c83ec = 1;
            } else {
                g_game8c83ec = 0;
            }
        }
        if (a3 == 0x71) {
            if (Game_Helper4bea85(g_gameSlots[a1][a2].field74, g_gameSlots[a1][a2].field78, 0, a1, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a1, a2), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                g_gameSlots[a1][a2].field50 = g_gameSlots[a1][a2].field74;
                g_gameSlots[a1][a2].field04 = g_gameSlots[a1][a2].field78;
            } else {
                Game_Helper46d8c1(a1, a2, 1);
                g_game8c83ec = 1;
            }
            g_gameSlots[a1][a2].field36 = 0;
        }
        if (g_gameSlots[a1][a2].field04 == g_game951bdc
            && (signed char)g_gameSlots[a1][a2].field50 == g_game8e1aa8
            && g_game951bdc != -1
            && (g_gameSlots[a1][a2].flags08 & 0x20) == 0
            && a3 == 0x34) {
            g_game7a01e0 |= a4;
        }
    }
    return 0;
}

/* used by 11 card scripts */
// FUNCTION: SHANDALAR 0x004ecec6
int Game_Helper4ecec6(int a1, int a2, int a3)
{
    if (a2 > a1) {
        a1 = a2;
    }
    if (a3 < a1) {
        a1 = a3;
    }
    return a1;
}
