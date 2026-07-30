/* Shared helpers invoked by the card scripts; reconstructed from the
 * disassembly by helpergen and checked by reccmp score. Names are raw
 * addresses because no meaning has been established yet. */
#include "game/helpers.h"

/* used by 2 card scripts */
// FUNCTION: SHANDALAR 0x0056a3a7
int Game_Helper56a3a7(int a1, int a2, int a3, int a4, int a5, int a6, int a7)
{
    int v4;

    v4 = -1;
    if (g_game8c7468 == a1
        && (g_game93a934 & 0x2) != 0) {
        Game_Helper56c91f(a1, 0x19);
        v4 = g_game7bea9c;
    }
    if (g_game8c776c == a1
        && g_game8bd200 != 1) {
        v4 = Game_Helper5583c5(a2, a3, a4, a5, a6, a7);
        if ((g_game93a934 & 0x2) != 0) {
            g_game7bea9c = v4;
            g_game7bea98 = 0x19;
            Game_Helper56c931(a1, 0x19);
        }
    }
    return 0;
}

/* used by 1 card script */
// FUNCTION: SHANDALAR 0x0056a45f
int Game_Helper56a45f(int a1, int a2, int a3, int a4, int a5, int a6, int a7)
{
    int v4;

    v4 = -1;
    if (a1 == g_game8c7468
        && (g_game93a934 & 0x2) != 0) {
        Game_Helper56c91f(a1, 0x19);
        v4 = g_game7bea9c;
    }
    if (a1 == g_game8c776c
        && g_game8bd200 != 1) {
        v4 = Game_Helper55840e(a2, a3, a4, a5, a6, a7);
        if ((g_game93a934 & 0x2) != 0) {
            g_game7bea9c = v4;
            g_game7bea98 = 0x19;
            Game_Helper56c931(a1, 0x19);
        }
    }
    return 0;
}

/* used by 2 card scripts */
// FUNCTION: SHANDALAR 0x0056bf4c
int Game_Helper56bf4c(int a1)
{
    int v4;

    for (v4 = 0; v4 < 0x1f4; v4++) {
        if ((g_game97f1c0[v4] & 0xfff) == a1) {
            Game_Helper56bfa4(v4);
            break;
        }
    }
    return 0;
}

/* used by 8 card scripts */
// FUNCTION: SHANDALAR 0x0056c496
int Game_Helper56c496(int a1, int a2)
{
    int v4;

    for (v4 = a2 + 1; v4 < 0x1f4; v4++) {
        g_game8a940c[a1][v4] = g_game8a9410[a1][v4];
    }
    return 0;
}

/* used by 6 card scripts */
// FUNCTION: SHANDALAR 0x0056c91f
int Game_Helper56c91f(int a1, int a2)
{
    return 0;
}

/* used by 6 card scripts */
// FUNCTION: SHANDALAR 0x0056c931
int Game_Helper56c931(int a1, int a2)
{
    return 0;
}
