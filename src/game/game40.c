/* Ordinary Shandalar game code (gamewire-reconstructed): plain .text
 * functions, neither card scripts nor their helpers. Address-derived names;
 * reccmp is the correctness check. */
#include "game/gamefns.h"

// FUNCTION: SHANDALAR 0x00404677
int Game_Fn404677(void)
{
    int iv;   /* [ebp-4] */

    iv = 0xa;
    if (g_game8bd200 < 0 || g_game79ff88 != 0) {
        iv = g_game8bd200;
        g_game8bd200 = 0;
        g_game79ff88 = 0;
        if (iv == -1 || iv == 0xfffffffe) {
            iv = 1;
        } else {
            if (iv == 0xfffffff6) {
                Game_Helper558aea(0, 0xff);
                g_game74d26c = g_game7a82e0;
                g_game74f1e0 = g_game94d464;
                if (g_game94d464 == 0) {
                    g_game74f1e0 = -1;
                }
                if (g_game94d464 == 0x22) {
                    g_game74f1e0 = 0x20;
                }
                if (g_game94d464 == 0) {
                    iv = 2;
                } else {
                    if (g_game94d464 == 1) {
                        iv = 3;
                    } else {
                        if (g_game94d464 == 4) {
                            iv = 4;
                        } else {
                            if (g_game94d464 == 0xa) {
                                iv = 5;
                            } else {
                                if (g_game94d464 == 0x14) {
                                    iv = 6;
                                } else {
                                    if (g_game94d464 == 0x1f) {
                                        iv = 7;
                                    } else {
                                        if (g_game94d464 == 0x22) {
                                            iv = 8;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return iv;
}

// FUNCTION: SHANDALAR 0x0040a561
int Game_Fn40a561(int a1, int a2)
{
    int iv;   /* [ebp-4] */

    iv = 0;
    if (g_game7befec == 0xd3) {
        g_game7a81a8 = 1;
        iv = 1;
    }
    if (Game_Helper40a62b(a1, a2, 0) == 0) {
    } else {
        if (Game_Helper40a62b(a1, a2, 1) == 0) {
        } else {
            if (Game_Helper40b26d(a1, a2) == 0) {
            } else {
                if (iv != 0) {
                    g_game7a81a8 = 0;
                }
                return 1;
            }
        }
    }
    if (iv != 0) {
        g_game7a81a8 = 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0040f0f6
int Game_Fn40f0f6(void)
{
    return Game_Helper56d167();
}

// FUNCTION: SHANDALAR 0x0040f132
int Game_Fn40f132(void)
{
    if (g_gameEventCount == 0) {
        return -1;
    }
    return g_game94d2ac[g_gameEventCount];
}

// FUNCTION: SHANDALAR 0x0040d572
int Game_Fn40d572(int a1, int a2)
{
    g_game8aa72c = 0xffffd8f1;
    g_game8bd200 = 1;
    g_game78e82c = a1;
    g_game930640 = (g_game78cf04 + 1) * a2 / 4;
    g_game97c85c = 1 << Game_Helper522508(5) + 1;
    Game_Helper4c357e();
    Game_Fn4ce955();
    Game_Helper559e3d();
    g_game8e09c0 = 0;
    g_game8bcf74 = 1;
    Game_Helper46ed33();
    Game_Helper578c70(1, 1, g_game749410);
    Game_Fn46ed03();
    g_game789900 = 0;
    return Game_Fn46ed03();
}

// FUNCTION: SHANDALAR 0x0040eb87
int Game_Fn40eb87(int a1, int a2)
{
    if (g_game8bd200 != 1) {
        Game_Fn5582ed(a1, a2);
    }
    g_game8c83ec = 0;
    return Game_Fn5582ed(a1, a2);
}
