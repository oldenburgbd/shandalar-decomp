/* Shared helpers invoked by the card scripts; reconstructed from the
 * disassembly by helpergen and checked by reccmp score. Names are raw
 * addresses because no meaning has been established yet. */
#include "game/helpers.h"

/* used by 1 card script */
// FUNCTION: SHANDALAR 0x004a008c
int Game_Helper4a008c(int a1)
{
    int v4;
    int v8;

    if (Game_Helper4c0efa(a1, a1, a1, 0x200, 1, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 0, &v8) != 0) {
        if (g_game8bd200 != 1) {
            Game_Helper40ebb9(0xf);
        }
        Game_Helper46d8c1(v8, v4, 3);
        return 1;
    }
    return 0;
}

/* used by 2 card scripts */
// FUNCTION: SHANDALAR 0x004a0403
int Game_Helper4a0403(int a1)
{
    int v4;
    int v8;

    if (Game_Helper4c0efa(a1, a1, a1, 0x200, 0x40, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 0, &v8) != 0) {
        if (g_game8bd200 != 1) {
            Game_Helper40ebb9(0xf);
        }
        Game_Helper46d8c1(v8, v4, 3);
        return 1;
    }
    return 0;
}
