/* Shared helpers invoked by the card scripts; reconstructed from the
 * disassembly by helpergen and checked by reccmp score. Names are raw
 * addresses because no meaning has been established yet. */
#include "game/helpers.h"

/* used by 1 card script */
// FUNCTION: SHANDALAR 0x00558321
int Game_Helper558321(int a1, int a2, int a3, int a4)
{
    if (g_game8bd200 == 1) {
    } else {
        Game_Helper450813(a1, a2, a3, a4);
    }
    return 0;
}

/* used by 16 card scripts */
// FUNCTION: SHANDALAR 0x00558356
int Game_Helper558356(int a1)
{
    Game_Helper57e820(0x669498, a1);
    Game_Helper451c24(a1);
    return 0;
}

/* used by 1 card script */
// FUNCTION: SHANDALAR 0x00558828
int Game_Helper558828(int a1, int a2)
{
    if (g_game8c776c == a1
        && (g_game93a934 & 0x2) != 0) {
        g_game8c6ae8 = 0xd;
        g_game8c6aec = a2;
        Game_Helper56c931(a1, 0xd);
    }
    return a2;
}

/* used by 2 card scripts */
// FUNCTION: SHANDALAR 0x00558941
int Game_Helper558941(int a1, int a2, int a3)
{
    int v4;

    if (g_game8bd200 == 1) {
    } else {
        if (a1 == g_game8c7468
            && (g_game93a934 & 0x2) != 0) {
            Game_Helper56c91f(a1, 0xe);
            a3 = g_game8c6aec;
        }
        v4 = Game_Helper53c35e(a1, a2, a3);
        if (g_game8c776c == a1
            && (g_game93a934 & 0x2) != 0) {
            g_game8c6ae8 = 0xe;
            g_game8c6aec = v4;
            Game_Helper56c931(a1, 0xe);
        }
    }
    return 0;
}

/* used by 8 card scripts */
// FUNCTION: SHANDALAR 0x005589ed
int Game_Helper5589ed(int a1, int a2, int a3, int a4, int a5)
{
    int v4;

    if (g_game8bd200 == 1) {
    } else {
        if (a1 == g_game8c7468
            && (g_game93a934 & 0x2) != 0) {
            Game_Helper56c91f(a1, 0xf);
        } else {
            v4 = Game_Helper53cb05(a1, a2, a3, a4, a5);
            if (g_game8c776c == a1
                && (g_game93a934 & 0x2) != 0) {
                g_game8c6ae8 = 0xf;
                g_game8c6aec = v4;
                Game_Helper56c931(a1, 0xf);
            }
        }
    }
    return 0;
}
