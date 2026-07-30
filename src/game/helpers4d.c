/* Card-system helpers in the 0x4Dxxxx range, reconstructed by helpergen.
 * Pattern-matched from the disassembly; the reccmp score is the check.
 * Address names only -- nothing here establishes meaning yet.
 */
#include "game/helpers.h"

/* called by 1 of the 608 card scripts;
 * verified against reports/ghidra/decomp/004d646a.c */
// FUNCTION: SHANDALAR 0x004d646a
int Game_Helper4d646a(int a1, int a2)
{
    g_gameSlots[(signed char)g_gameSlots[a1][a2].field50][g_gameSlots[a1][a2].field04].field00 += 0x100;
    if (g_game8bd200 != 1) {
        Game_Helper40ebb9(0x25);
    }
    g_gameSlots[(signed char)g_gameSlots[a1][a2].field50][g_gameSlots[a1][a2].field04].field1c = g_gameSlots[(signed char)g_gameSlots[a1][a2].field50][g_gameSlots[a1][a2].field04].field1c - 2;
    return 0;
}

/* called by 1 of the 608 card scripts */
// FUNCTION: SHANDALAR 0x004dbdfc
int Game_Helper4dbdfc(int a1, int a2, int a3, int a4)
{
    int iv;
    int ig;
    int ih;
    int ii;
    int ij;

    ii = Game_Helper40d7b2(a3, a4, 0x34, -1);
    Game_Helper4c5bf6(&iv, &ig);
    if (a1 == 1) {
        ih = iv;
    } else {
        ih = ig;
    }
    ij = Game_Helper40d146(a1, a2, a3, a4, ii, ih);
    return 0;
}

/* called by 2 of the 608 card scripts */
// FUNCTION: SHANDALAR 0x004d34d6
int Game_Helper4d34d6(int a1, int a2, int a3, int a4)
{
    int iv;
    int ig;
    int ih;

    if (a3 == 0x74) {
        return Game_Helper4be650(0, 0, a1, 2, 2, 0x200, a4, 0, 0, Game_Helper488cc4(a1, a2), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (a3 == 0x6c && g_game951bdc == a2 && g_game8e1aa8 == a1) {
            if (Game_Helper4c0efa(a1, 2, 1 - a1, 0x200, a4, 0, 0, Game_Helper488cc4(a1, a2), 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &ih) != 0) {
                g_gameSlots[a1][a2].field74 = ih;
                g_gameSlots[a1][a2].field78 = ig;
                g_gameSlots[a1][a2].field36 = 1;
            } else {
                g_game8c83ec = 1;
            }
            return 0;
        } else {
            if (a3 == 0x71) {
                if (Game_Helper4bea85(g_gameSlots[a1][a2].field74, g_gameSlots[a1][a2].field78, 0, a1, 2, 2, 0x200, a4, 0, 0, Game_Helper488cc4(a1, a2), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                    g_gameSlots[a1][a2].field50 = g_gameSlots[a1][a2].field74;
                    g_gameSlots[a1][a2].field04 = g_gameSlots[a1][a2].field78;
                    if (Game_Helper485eb8(a1, a2) != 0) {
                        g_gameSlots[a1][a2].flags18 |= 0x1000000;
                        if (g_gameSlots[a1][a2].field74 != a1) {
                            if (g_game8bd200 != 1) {
                                Game_Helper40ebb9(0x29);
                            }
                            iv = Game_Helper4d40a4((signed char)g_gameSlots[a1][a2].field50, g_gameSlots[a1][a2].field04);
                            g_gameSlots[a1][a2].field50 = a1;
                            g_gameSlots[a1][a2].field04 = iv;
                        }
                    }
                } else {
                    Game_Helper46d8c1(a1, a2, 1);
                    g_game8c83ec = 1;
                }
                g_gameSlots[a1][a2].field36 = 0;
                return 0;
            } else {
                if (g_game94d178 == 0xd4 && g_game951bdc == a2 && g_game8e1aa8 == a1 && (signed char)g_gameSlots[a1][a2].field50 != -1 && g_gameSlots[(signed char)g_gameSlots[a1][a2].field50][g_gameSlots[a1][a2].field04].field6c != -1 && g_game7bf4b8 == a1 && g_game8e2fc4 == a2 && a1 == g_game8c8afc) {
                    if (a3 == 0x7d) {
                        g_game7a01e0 |= 2;
                    }
                    if (a3 == 0x7e) {
                        if ((g_gameSlots[a1][a2].flags18 & 0x1000000) != 0) {
                            if ((signed char)g_gameSlots[a1][a2].field0c != -1) {
                                g_gameSlots[(signed char)g_gameSlots[a1][a2].field0c][g_gameSlots[a1][a2].field48].flags18 |= 0x1000000;
                                if ((signed char)g_gameSlots[a1][a2].field0c != (signed char)g_gameSlots[a1][a2].field50) {
                                    Game_Helper4d40a4((signed char)g_gameSlots[a1][a2].field50, g_gameSlots[a1][a2].field04);
                                }
                            } else {
                                if (g_gameSlots[(signed char)g_gameSlots[a1][a2].field50][g_gameSlots[a1][a2].field04].field6c != -1 && ((g_gameSlots[(signed char)g_gameSlots[a1][a2].field50][g_gameSlots[a1][a2].field04].flags08 & 0x400000) != 0 && (signed char)g_gameSlots[a1][a2].field50 == g_game8c776c || (g_gameSlots[(signed char)g_gameSlots[a1][a2].field50][g_gameSlots[a1][a2].field04].flags08 & 0x400000) == 0 && (signed char)g_gameSlots[a1][a2].field50 == g_game8c7468)) {
                                    Game_Helper4d40a4((signed char)g_gameSlots[a1][a2].field50, g_gameSlots[a1][a2].field04);
                                }
                            }
                        } else {
                            Game_Helper49f3c0((int)Game_Callback4d3f22, -1);
                        }
                    }
                }
                return 0;
            }
        }
    }
}
