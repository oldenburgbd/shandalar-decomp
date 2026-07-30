/* Shared helpers invoked by the card scripts; reconstructed from the
 * disassembly by helpergen and checked by reccmp score. Names are raw
 * addresses because no meaning has been established yet. */
#include "game/helpers.h"

/* used by 14 card scripts */
// FUNCTION: SHANDALAR 0x0042d790
int Game_Helper42d790(int a1, int a2, int a3, int a4)
{
    if (a3 == 0x7f
        && g_game951bdc == a2
        && g_game8e1aa8 == a1) {
        if (((g_gameSlots[a1][a2].flags08 & 0x30000) == 0 || (g_gameCards[g_gameSlots[a1][a2].field6c].flags & 0x2) == 0) && (g_gameSlots[a1][a2].flags08 & 0x10) == 0) {
            Game_Helper441539(a1, a4, 1);
        }
        return 0;
    }
    if (a3 == 0x73) {
        if ((g_gameSlots[a1][a2].flags08 & 0x10) == 0) {
            if ((g_gameSlots[a1][a2].flags08 & 0x30000) == 0 || (g_gameCards[g_gameSlots[a1][a2].field6c].flags & 0x2) == 0) {
                return 1;
            }
        }
        return 0;
    }
    if (a3 == 0x6d) {
        Game_Helper441651(a1, a4, 1);
        g_gameSlots[a1][a2].flags08 |= 0x10;
        g_game93a794 = a4;
        return 0;
    }
    if (a3 == 0x71
        && g_game8bd200 != 1) {
        Game_Helper40ebb9(a4 + 8);
    }
    return 0;
}
