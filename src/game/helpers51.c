/* Card-system helpers in the 0x51xxxx range, reconstructed by helpergen.
 * Pattern-matched from the disassembly; the reccmp score is the check.
 * Address names only -- nothing here establishes meaning yet.
 */
#include "game/helpers.h"

/* called by 1 of the 608 card scripts */
// FUNCTION: SHANDALAR 0x00512898
int Game_Helper512898(int a1, int a2)
{
    int iv;
    int ig;
    int ih;
    int ii;

    for (iv = 0, ig = 0; iv < 2; iv++) {
        for (ii = 0; g_game78e970[iv] > ii; ii++) {
            if (Game_Helper411d90(iv, ii) != 0) {
                ih = g_gameSlots[iv][ii].field6c;
                if (g_gameCards[ih].mask == 0x37b
                    && (signed char)g_gameSlots[iv][ii].field0c == a1
                    && g_gameSlots[iv][ii].field48 == a2
                    && g_gameSlots[iv][ii].field38 == 0) {
                    ig++;
                }
            }
        }
    }
    return 0;
}

/* called by 1 of the 608 card scripts */
// FUNCTION: SHANDALAR 0x00512a01
int Game_Helper512a01(int a1, int a2, int a3)
{
    int v4;

    v4 = g_gameSlots[a1][a2].field0e;
    Game_Helper49f710(a1, a2, 1);
    g_gameSlots[a1][a2].field28 |= 0x2000000;
    v4 = Game_Helper40d7b2(a1, a2, 0x33, -1);
    if (v4 > 0) {
        g_gameSlots[a1][a2].field28 |= 0x4000000;
        Game_Helper40d7b2(a1, a2, 0x32, -1);
    }
    if (g_game94d45c > 0) {
        g_game94d45c--;
    }
    return 0;
}

/* called by 1 of the 608 card scripts;
 * verified against reports/ghidra/decomp/00512b06.c */
// FUNCTION: SHANDALAR 0x00512b06
int Game_Helper512b06(int a1, int a2)
{
    int v4;

    v4 = Game_FindFreeSlot(a1, Game_Helper56c705(0x37b));
    if (v4 != -1) {
        Game_Helper4d1c03(a1, v4);
        g_gameSlots[a1][v4].field0c = g_game94f280;
        g_gameSlots[a1][v4].field48 = g_game930f5c;
        g_gameSlots[a1][v4].flags18 |= 0x10;
        g_gameSlots[a1][v4].field38 = 1;
    }
    return 0;
}

/* called by 5 of the 608 card scripts */
// FUNCTION: SHANDALAR 0x00518a81
int Game_Helper518a81(int a1, int a2, int a3, int a4)
{
    if (a3 == 0x6c
        && a2 == g_game951bdc
        && a1 == g_game8e1aa8) {
        g_game94d63c += g_game77ed80[g_game8c7468][a4] * 12;
    }
    if (g_game94d178 == 0xd3
        && a2 == g_game951bdc
        && a1 == g_game8e1aa8
        && g_game8c8afc == a1) {
        if (((g_gameSlots[a1][a2].flags08 & 0x10) == 0 || (g_gameCards[g_gameSlots[a1][a2].field6c].flags & 0x2) != 0)
            && Game_Helper411d90(a1, a2) != 0
            && Game_Helper441699(a1, 7, 1) != 0) {
            a4 = 1 << Game_Helper4ba392(a1, a2, a4);
            if ((a4 & 0x13) != 0
                && g_gameSlots[g_game7bf4b8][g_game8e2fc4].field6c != -1
                && g_gameCards[g_gameSlots[g_game7bf4b8][g_game8e2fc4].field6c].flags != 1) {
                if (a3 == 0x7d) {
                    if (a1 == g_game8c7468
                        && (g_game93a934 & 0x2) == 0) {
                        g_game7a01e0 |= 2;
                    } else {
                        g_game7a01e0 |= 1;
                    }
                }
                if (a3 == 0x7e) {
                    Game_LogEvent(a1, a2, 0x72, 0, 0);
                    Game_Helper4143e0(a1, 0, 1);
                    Game_Helper41040d();
                    if (g_game8c83ec != 1) {
                        Game_Helper40e158(a1, a2, 0x91);
                        Game_Helper4883aa(a1, 1, a1, a2);
                        if (a1 == g_game8c7468) {
                            g_game94d63c -= 0x18;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

/* called by 1 of the 608 card scripts */
// FUNCTION: SHANDALAR 0x0051be77
int Game_Helper51be77(int a1, int a2, int a3, int a4)
{
    int v4;
    int v8;

    for (v4 = 0; v4 < 2; v4++) {
        for (v8 = 0; g_game78e970[v4] > v8; v8++) {
            if (g_gameSlots[v4][v8].field6c == g_game93a7d4
                && (g_gameSlots[v4][v8].flags08 & 0x800002) == 2
                && (signed char)g_gameSlots[v4][v8].field50 == a1
                && g_gameSlots[v4][v8].field04 == a2) {
                g_gameSlots[v4][v8].field6c = -1;
                Game_Helper4bab38(a3, g_gameSlots[v4][v8].field38, (signed char)g_gameSlots[v4][v8].field0c, g_gameSlots[v4][v8].field48);
                g_gameSlots[v4][v8].field38 = 0;
            }
        }
    }
    return 0;
}
