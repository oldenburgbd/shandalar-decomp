/* Shared helpers invoked by the card scripts; reconstructed from the
 * disassembly by helpergen and checked by reccmp score. Names are raw
 * addresses because no meaning has been established yet. */
#include "game/helpers.h"

/* used by 131 card scripts */
// FUNCTION: SHANDALAR 0x004c4263
int Game_Helper4c4263(int a1)
{
    if (g_game8bd200 != 1) {
        g_game939ce8 = g_game5ae0a0[a1 + g_game74842c];
        if (g_game939ce8 != -1) {
            g_game939ce8 &= 0xfff;
        }
    }
    return 0;
}

/* used by 14 card scripts */
// FUNCTION: SHANDALAR 0x004c42ae
int Game_Helper4c42ae(int a1)
{
    if (g_game8bd200 != 1) {
        g_game74b868 = g_game5b2dd0[g_game74842c + a1];
        if (g_game74b868 == 0x63) {
            g_game74b868 = 0;
        }
    }
    return 0;
}

/* used by 11 card scripts */
// FUNCTION: SHANDALAR 0x004c42f8
int Game_Helper4c42f8(void)
{
    if (g_game5b2200[g_game74842c] != g_game589a38) {
        g_game589a38 |= 0x100;
    }
    g_game939ce8 = g_game5ae0a0[g_game74842c];
    g_game94d46c = g_game5b2dd0[g_game74842c];
    if (g_game94d46c != 0x63) {
        g_game74842c++;
    }
    g_game589a38 = 0;
    return g_game5b2dd0[g_game74842c];
}

/* used by 8 card scripts */
// FUNCTION: SHANDALAR 0x004c442a
int Game_Helper4c442a(void)
{
    if (g_game74842c > 0) {
        g_game74842c--;
    } else {
        g_game74842c = 0;
    }
    return 0;
}
