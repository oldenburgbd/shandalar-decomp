/* Shandalar game layer: card behaviour scripts at 0x4Bxxxx.
 *
 * One of several translation units, split by address range. THIS IS NOT
 * COSMETIC: MSVC re-rolls codegen across a whole translation unit when
 * anything in it changes, so a single large file meant every new script
 * perturbed every earlier one. Smaller units confine that blast radius.
 *
 * It is also the closer match to the original, which evidently had one source
 * file per card: three byte-identical 18-byte handlers sit at three different
 * addresses rather than being one function reused.
 *
 * The shared CUR/SRC macros live in game/gameslot.h. */

#include "game/gameslot.h"

// FUNCTION: SHANDALAR 0x004b0512
int Game_CardScript4b0512(int a, int b, int kind)
{
    if (kind == 0x74) {
        if (a != g_game7a82e0 && g_game94d464 < 0x15) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x71) {
        Game_Helper4a9910(a, b, g_game7a00a4, -1, -1);
        Game_Helper46d8c1(a, b, 2);
    }
    return 0;
}

/* --- scripts that call ---------------------------------------------------
 *
 * The first four taken from behind the stub barrier. Everything below calls at
 * least one helper, which is why none of them could be attempted before. */

/* Two helpers on kind 0x71, and an early 1 for 0x74. The -1, -1 tail on the
 * five-argument call is literal in the original. */
// FUNCTION: SHANDALAR 0x004b29c6
int Game_CardScript4b29c6(int a, int b, int kind)
{
    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x71) {
        Game_Helper4a9910(a, b, g_game7a0230, -1, -1);
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

/* The 0x74 arm answers 1 on two different routes and 0 on one, and the original
 * emits a dead `jmp` between the first two -- the giveaway that the source had
 * an else branch here rather than a straight fallthrough. */
// FUNCTION: SHANDALAR 0x004b47b1
int Game_CardScript4b47b1(int a, int b, int kind)
{
    if (kind == 0x74) {
        if (a == g_game8c776c || (g_game93a934 & 2) != 0) {
            return 1;
        }
        if (a != g_game7a82e0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x71) {
        Game_Helper488b70(a, b, (int)Game_Callback4b484b, -1);
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}


/* A near-pair with 0x004b3902 below. Both stash their two arguments in a fixed
 * pair of globals, hand a callback to Game_Helper49f3c0, and then answer kind
 * 0x3b by testing two helper results. They differ in which globals they use,
 * whether the second argument is g_game7a82e0 or 1 - g_game7a82e0, the four
 * constants, and which counters they bump. */
// FUNCTION: SHANDALAR 0x004b3a96
int Game_CardScript4b3a96(int a, int b, int kind)
{
    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x71) {
        g_game5aa620 = a;
        g_game5aa61c = b;
        Game_Helper49f3c0((int)Game_Callback4b3b46, 1 - g_game7a82e0);
        Game_Helper46d8c1(a, b, 1);
    }
    if (kind == 0x3b) {
        if (Game_Helper441699(a, 5, 1) != 0 && Game_Helper441699(a, 7, 2) != 0) {
            g_game7beb18[a] += 3;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004b3902
int Game_CardScript4b3902(int a, int b, int kind)
{
    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x71) {
        g_game5aa4e8 = a;
        g_game5aa4ec = b;
        Game_Helper49f3c0((int)Game_Callback4b39b5, g_game7a82e0);
        Game_Helper46d8c1(a, b, 1);
    }
    if (kind == 0x3b) {
        if (Game_Helper441699(a, 5, 2) != 0 && Game_Helper441699(a, 7, 3) != 0) {
            g_game7beb10[a]++;
            g_game7beb18[a]++;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004b3f6b
int Game_CardScript4b3f6b(int a, int b, int kind)
{
    if (kind == 0x74) {
        Game_Helper4c4263(1);
        return 1;
    }
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        Game_Helper56ca10(0x58903c, 0x58902c);
        if (Game_Helper498e8a(a, b, 3) != 0) {
            g_game94d63c -= 0x24;
        }
    }
    if (kind == 0x71) {
        Game_Helper49914d(a, b, kind, 3);
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004b8014
int Game_CardScript4b8014(int a, int b, int kind)
{
    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        if (g_game8e3ec8[a] < 8) {
            g_game94d63c -= 0x3c;
        } else {
            g_game94d63c -= 0x18;
        }
    }
    if (kind == 0x71) {
        Game_Helper4415c5(a, 1, 3);
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004b0e02
int Game_CardScript4b0e02(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        Game_Helper4c4263(0);
        Game_Helper4be650(0, 0, a, 2, 2, 0x200, 0x40, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            Game_Helper56ca10(0x588ec4, 0x588ebc);
            if (Game_Helper4a011a(a, 1 - a, b) != 0) {
                g_game94d63c -= 0x10;
            } else {
                g_game8c83ec = 1;
            }
        }
        if (kind == 0x71) {
            v8 = g_gameSlots[a][b].field74;
            v4 = g_gameSlots[a][b].field78;
            if (Game_Helper4bea85(v8, v4, 0, a, 2, 2, 0x200, 0x40, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                Game_Helper46d8c1(v8, v4, 2);
            } else {
                g_game8c83ec = 1;
            }
            g_gameSlots[a][b].field36 = 0;
            Game_Helper46d8c1(a, b, 1);
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004b1bcb
int Game_CardScript4b1bcb(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x71) {
        for (ig = 0; ig < 2; ig++) {
            for (iv = 0; g_game78e970[ig] > iv; iv++) {
                if (Game_Helper411d90(ig, iv) != 0
                    && (g_gameCards[g_gameSlots[ig][iv].field6c].flags & 0x2) != 0) {
                    ih = Game_Helper4a9910(a, b, g_game93a864, ig, iv);
                    if (ih != -1) {
                        g_gameSlots[a][ih].field38 = 0xfffffffe;
                        g_gameSlots[a][ih].field4c = 0;
                    }
                }
            }
        }
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004b2a22
int Game_CardScript4b2a22(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        Game_Helper4c4263(0);
        Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            Game_Helper56ca10(0x588fa8, 0x588f94);
            if (Game_Helper49f87b(a, 1 - a, b) == 0) {
                g_game8c83ec = 1;
            }
        }
        if (kind == 0x71) {
            v8 = g_gameSlots[a][b].field74;
            v4 = g_gameSlots[a][b].field78;
            if (Game_Helper4bea85(v8, v4, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                Game_Helper4883aa(v8, Game_Helper40d7b2(v8, v4, 0x32, -1), a, b);
                Game_Helper46d8c1(v8, v4, 4);
            } else {
                g_game8c83ec = 1;
            }
            g_gameSlots[a][b].field36 = 0;
            Game_Helper46d8c1(a, b, 1);
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004b4028
int Game_CardScript4b4028(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (kind == 0x74) {
        Game_Helper4c4263(0);
        Game_Helper4be650(0, 0, a, 2, 2, 0x200, 0x40, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            Game_Helper56ca10(0x589050, 0x589048);
            if (Game_Helper4a011a(a, 1 - a, b) == 0) {
                g_game8c83ec = 1;
            }
        }
        if (kind == 0x71) {
            ih = g_gameSlots[a][b].field74;
            ig = g_gameSlots[a][b].field78;
            if (Game_Helper4bea85(ih, ig, 0, a, 2, 2, 0x200, 0x40, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                iv = g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].field6c;
                Game_Helper4883aa(ih, (signed char)g_gameCards[iv].field2b + Game_Helper4ecec6((signed char)g_gameCards[iv].field2c, 0, 0x63), a, b);
                Game_Helper46d8c1(ih, ig, 1);
            } else {
                g_game8c83ec = 1;
            }
            g_gameSlots[a][b].field36 = 0;
            Game_Helper46d8c1(a, b, 1);
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004b45ba
int Game_CardScript4b45ba(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        Game_Helper4c4263(0);
        Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c
            && g_game951bdc == b
            && a == g_game8e1aa8) {
            g_game94d63c -= 0x30;
            Game_Helper56ca10(0x589084, 0x589078);
            if (Game_Helper49f87b(a, 1 - a, b) == 0) {
                g_game8c83ec = 1;
            }
        }
        if (kind == 0x71) {
            v8 = g_gameSlots[a][b].field74;
            v4 = g_gameSlots[a][b].field78;
            if (Game_Helper4bea85(v8, v4, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                Game_Helper4ba41b(v8, v4);
            } else {
                g_game8c83ec = 1;
            }
            g_gameSlots[a][b].field36 = 0;
            Game_Helper46d8c1(a, b, 1);
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004b01c2
int Game_CardScript4b01c2(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;
    int ii;

    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        Game_Helper56ca10(0x588e78, 0x588e6c);
        if (Game_Helper4c0efa(a, 2, 1 - a, 0x1000, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &ih) != 0) {
            g_gameSlots[a][b].field74 = ih;
            g_gameSlots[a][b].field78 = ig;
            g_gameSlots[a][b].field36 = 1;
        } else {
            g_game8c83ec = 1;
        }
    }
    if (kind == 0x71) {
        ii = g_gameSlots[a][b].field74;
        Game_Helper49f3c0((int)Game_Callback4b03cd, ii);
        for (iv = 0; iv <= 7; iv++) {
            g_game7806a0[ii][iv] = 0;
        }
        g_gameSlots[a][b].field36 = 0;
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004b0590
int Game_CardScript4b0590(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        Game_Helper56ca10(0x588e98, 0x588e84);
        if (Game_Helper4c0efa(a, 2, a, 0x1000, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &ih) != 0) {
            g_gameSlots[a][b].field74 = ih;
            g_gameSlots[a][b].field78 = ig;
            g_gameSlots[a][b].field36 = 1;
        } else {
            g_game8c83ec = 1;
        }
    }
    if (kind == 0x71) {
        iv = g_gameSlots[a][b].field74;
        Game_Helper409a0b(iv);
        Game_Helper409a0b(iv);
        Game_Helper409a0b(iv);
        g_gameSlots[a][b].field36 = 0;
        Game_Helper46d8c1(a, b, 2);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004b0fe3
int Game_CardScript4b0fe3(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        Game_Helper4c4263(0);
        Game_Helper4be650(0, 0, a, 2, 2, 0x200, 0x44, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c
            && b == g_game951bdc
            && a == g_game8e1aa8) {
            g_game94d63c -= 0x10;
            Game_Helper56ca10(0x588edc, 0x588ed0);
            if (Game_Helper4c0efa(a, 2, 2, 0x200, 0x44, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &v8) != 0) {
                g_gameSlots[a][b].field74 = v8;
                g_gameSlots[a][b].field78 = v4;
                g_gameSlots[a][b].field36 = 1;
            } else {
                g_game8c83ec = 1;
            }
        }
        if (kind == 0x71) {
            v8 = g_gameSlots[a][b].field74;
            v4 = g_gameSlots[a][b].field78;
            if (Game_Helper4bea85(v8, v4, 0, a, 2, 2, 0x200, 0x44, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                Game_Helper46d8c1(v8, v4, 2);
            } else {
                g_game8c83ec = 1;
            }
            g_gameSlots[a][b].field36 = 0;
            Game_Helper46d8c1(a, b, 1);
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004b1910
int Game_CardScript4b1910(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        Game_Helper4c4263(0);
        Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 1, 0, 0);
    } else {
        if (kind == 0x6c
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            Game_Helper56ca10(0x588f20, 0x588f18);
            if (Game_Helper4c0efa(a, 2, a, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 1, 0, 0, 0x8aa920, 1, &v8) != 0) {
                g_gameSlots[a][b].field74 = v8;
                g_gameSlots[a][b].field78 = v4;
                g_gameSlots[a][b].field36 = 1;
            } else {
                g_game8c83ec = 1;
            }
        }
        if (kind == 0x71) {
            v8 = g_gameSlots[a][b].field74;
            v4 = g_gameSlots[a][b].field78;
            if (Game_Helper4bea85(v8, v4, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 1, 0, 0) != 0) {
                Game_Helper46d8c1(v8, v4, 1);
            } else {
                g_game8c83ec = 1;
            }
            g_gameSlots[a][b].field36 = 0;
            Game_Helper46d8c1(a, b, 1);
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004b315b
int Game_CardScript4b315b(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;
    int ii;
    int ij;

    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        if (g_game8c7468 == a
            && (g_game93a934 & 0x2) == 0) {
            if (g_game8bd200 == 1) {
                if (Game_Helper522508(3) != 0) {
                    g_game94d46c = 0;
                } else {
                    g_game94d46c = 1;
                }
                if (g_game94d46c != 0) {
                    if (Game_Helper49f31f(1 - a, 0x40) == 0) {
                        g_game94d46c = 0;
                    } else {
                        if (Game_Helper49f31f(a, 0x40) == 0) {
                            g_game94d46c = 1;
                        }
                    }
                }
                Game_Helper4c4181();
            } else {
                Game_Helper4c42f8();
            }
            if (g_game94d46c != 0) {
                g_gameSlots[a][b].field74 = a;
            } else {
                g_gameSlots[a][b].field74 = 1 - a;
            }
            g_gameSlots[a][b].field78 = -1;
            g_gameSlots[a][b].field36 = 1;
        } else {
            Game_Helper56ca10(0x588ff8, 0x588fe8);
            if (Game_Helper4c0efa(a, 2, 1 - a, 0x1000, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &ii) != 0) {
                g_gameSlots[a][b].field74 = ii;
                g_gameSlots[a][b].field78 = ih;
                g_gameSlots[a][b].field36 = 1;
            } else {
                g_game8c83ec = 1;
            }
        }
    }
    if (kind == 0x71) {
        if (g_gameSlots[a][b].field74 != 0) {
            ij = 0x1000;
        } else {
            ij = 0;
        }
        for (iv = 0; iv < 2; iv++) {
            for (ig = 0; g_game78e970[iv] > ig; ig++) {
                if (Game_Helper411d90(iv, ig) != 0
                    && (g_gameCards[g_gameSlots[iv][ig].field6c].flags & 0x40) != 0
                    && (g_gameSlots[iv][ig].flags08 & 0x1000) == ij) {
                    Game_Helper4ba41b(iv, ig);
                }
            }
        }
        g_gameSlots[a][b].field36 = 0;
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004b3c5f
int Game_CardScript4b3c5f(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        Game_Helper4c4263(0);
        Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0x40, 0, Game_Helper488cc4(a, b), 0, 1 << Game_Helper4ba392(a, b, 1), -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c
            && b == g_game951bdc
            && a == g_game8e1aa8) {
            Game_Helper56ca10(0x589020, 0x589018);
            if (Game_Helper4c0efa(a, 2, 1 - a, 0x200, 2, 0x40, 0, Game_Helper488cc4(a, b), 0, 1 << Game_Helper4ba392(a, b, 1), -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &v8) != 0) {
                g_gameSlots[a][b].field74 = v8;
                g_gameSlots[a][b].field78 = v4;
                g_gameSlots[a][b].field36 = 1;
            } else {
                g_game8c83ec = 1;
            }
        }
        if (kind == 0x71) {
            v8 = g_gameSlots[a][b].field74;
            v4 = g_gameSlots[a][b].field78;
            if (Game_Helper4bea85(v8, v4, 0, a, 2, 2, 0x200, 2, 0x40, 0, Game_Helper488cc4(a, b), 0, 1 << Game_Helper4ba392(a, b, 1), -1, -1, -1, -1, 0, 0, 0) != 0) {
                Game_Helper46d8c1(v8, v4, 1);
            } else {
                g_game8c83ec = 1;
            }
            g_gameSlots[a][b].field36 = 0;
            Game_Helper46d8c1(a, b, 1);
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004b80b2
int Game_CardScript4b80b2(int a, int b, int kind)
{
    Game_Helper4b8102(a, b, kind, g_game8c8274);
    return 0;
}

// FUNCTION: SHANDALAR 0x004b80dc
int Game_CardScript4b80dc(int a, int b, int kind)
{
    Game_Helper4b8102(a, b, kind, 3);
    return 0;
}

// FUNCTION: SHANDALAR 0x004b9465
int Game_CardScript4b9465(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        Game_Helper4c4263(0);
        if ((g_game8c83f8 & 0x4) != 0
            && Game_Helper4be650(0, 0, a, 2, 2, 0x200, 0, 0, 0, 0, 0, 0, g_game93a7d4, -1, -1, -1, 0x20, 0, 0) != 0) {
            return 0x63;
        }
        return 0;
    }
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        Game_Helper56ca10(0x589230, 0x589224);
        if (Game_Helper4c0efa(a, 2, 2, 0x200, 0, 0, 0, 0, 0, 0, g_game93a7d4, -1, -1, -1, 0x20, 0, 0, 0x8aa920, 1, &v8) != 0) {
            if (g_game8c83f0[1 - a] > 0) {
                g_game94d63c += g_gameSlots[v8][v4].field38 * 100 / g_game8c83f0[1 - a];
            } else {
                g_game94d63c += 0x3e8;
            }
            g_gameSlots[a][b].field74 = v8;
            g_gameSlots[a][b].field78 = v4;
            g_gameSlots[a][b].field36 = 1;
        } else {
            g_game8c83ec = 1;
        }
    }
    if (kind == 0x71) {
        Game_Helper4bab38((signed char)g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].field0c, g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].field38, a, b);
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004b9998
int Game_CardScript4b9998(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        Game_Helper4c4263(0);
        Game_Helper4be650(0, 0, a, 2, 2, 0x200, 3, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            Game_Helper56ca10(0x589244, 0x58923c);
            if (Game_Helper4c0efa(a, 2, 1 - a, 0x200, 3, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &v8) != 0) {
                g_gameSlots[a][b].field74 = v8;
                g_gameSlots[a][b].field78 = v4;
                g_gameSlots[a][b].field36 = 1;
            } else {
                g_game8c83ec = 1;
            }
        }
        if (kind == 0x71) {
            v8 = g_gameSlots[a][b].field74;
            v4 = g_gameSlots[a][b].field78;
            if (Game_Helper4bea85(v8, v4, 0, a, 2, 2, 0x200, 3, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                Game_Helper46d8c1(v8, v4, 1);
            } else {
                g_game8c83ec = 1;
            }
            g_gameSlots[a][b].field36 = 0;
            Game_Helper46d8c1(a, b, 1);
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004b23c6
int Game_CardScript4b23c6(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (kind == 0x74) {
        Game_Helper4c4263(0);
        return Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0x10, 0);
    } else {
        if (kind == 0x6c && g_game951bdc == b && g_game8e1aa8 == a) {
            Game_Helper56ca10(0x588f70, 0x588f60);
            if (Game_Helper4c0efa(a, 2, a, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0x10, 0, 0x8aa920, 1, &ih) != 0) {
                g_gameSlots[a][b].field74 = ih;
                g_gameSlots[a][b].field78 = ig;
                g_gameSlots[a][b].field36 = 1;
            } else {
                g_game8c83ec = 1;
            }
        }
        if (kind == 0x71) {
            ih = g_gameSlots[a][b].field74;
            ig = g_gameSlots[a][b].field78;
            if (Game_Helper4bea85(ih, ig, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0x10, 0) != 0) {
                iv = Game_Helper4a9910(a, b, g_game8bcff8, ih, ig);
                if (iv != -1) {
                    g_gameSlots[a][iv].field12 = 7;
                    g_gameSlots[a][iv].field1c = 7;
                }
            } else {
                g_game8c83ec = 1;
            }
            g_gameSlots[a][b].field36 = 0;
            Game_Helper46d8c1(a, b, 1);
        }
        return 0;
    }
}

// FUNCTION: SHANDALAR 0x004b35bf
int Game_CardScript4b35bf(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (kind == 0x74) {
        Game_Helper4c4263(0);
        return Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c && g_game951bdc == b && g_game8e1aa8 == a) {
            Game_Helper56ca10(0x58900c, 0x589004);
            if (Game_Helper49f87b(a, a, b) != 0) {
                if (g_gameSlots[a][b].field74 == g_game8c776c) {
                    g_game94d63c -= 0x18;
                }
                if ((g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].field28 & 0x20) != 0) {
                    g_game94d63c -= 0x63;
                }
            } else {
                g_game8c83ec = 1;
            }
        }
        if (kind == 0x71) {
            ih = g_gameSlots[a][b].field74;
            ig = g_gameSlots[a][b].field78;
            if (Game_Helper4bea85(ih, ig, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                iv = Game_Helper4a9910(a, b, g_game8aa734, ih, ig);
                if (iv != -1) {
                    g_gameSlots[a][iv].field28 = 0;
                    g_gameSlots[a][iv].field38 = 0x20;
                }
            } else {
                g_game8c83ec = 1;
            }
            g_gameSlots[a][b].field36 = 0;
            Game_Helper46d8c1(a, b, 1);
        }
        return 0;
    }
}

// FUNCTION: SHANDALAR 0x004b647a
int Game_CardScript4b647a(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (kind == 0x74) {
        if (g_game8e2688 == -1) {
            Game_Helper4c4263(0);
            return Game_Helper4be650(0, 0, a, 2, 2, 0x200, 0x1047, 0, 0, Game_Helper488cc4(a, b), 1 << Game_Helper4ba392(a, b, 4), 0, -1, -1, -1, -1, 0, 0, 0);
        } else {
            if (Game_Helper4bea85(g_game8e2688, g_game8e2674, 0, a, 2, 2, 0, 0, 0, 0, 0, 1 << Game_Helper4ba392(a, b, 4), 0, -1, -1, -1, -1, 2, 0, 0) != 0) {
            }
            return 0;
        }
    } else {
        if (kind == 0x6c && g_game951bdc == b && a == g_game8e1aa8) {
            if (g_game8e2688 != -1) {
                g_gameSlots[a][b].field74 = g_game8e2688;
                g_gameSlots[a][b].field78 = g_game8e2674;
                g_gameSlots[a][b].field36 = 1;
            } else {
                Game_Helper56ca10(0x589138, 0x589120);
                if (Game_Helper4c0efa(a, 2, 1 - a, 0x200, 0x1047, 0, 0, Game_Helper488cc4(a, b), 1 << Game_Helper4ba392(a, b, 4), 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &ih) != 0) {
                    g_gameSlots[a][b].field74 = ih;
                    g_gameSlots[a][b].field78 = ig;
                    g_gameSlots[a][b].field36 = 1;
                } else {
                    g_game8c83ec = 1;
                }
            }
        }
        if (kind == 0x71) {
            iv = 0;
            if (g_game8e2688 != -1) {
                if (Game_Helper4bea85(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78, 0, a, 2, 2, 0, 0, 0, 0, 0, 1 << Game_Helper4ba392(a, b, 4), 0, -1, -1, -1, -1, 2, 0, 0) != 0) {
                    iv++;
                }
            } else {
                if (Game_Helper4bea85(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78, 0, a, 2, 2, 0x200, 0, 0, 0, Game_Helper488cc4(a, b), 1 << Game_Helper4ba392(a, b, 4), 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                    iv++;
                }
            }
            if (iv != 0) {
                ih = g_gameSlots[a][b].field74;
                ig = g_gameSlots[a][b].field78;
                if ((1 << Game_Helper4ba392(a, b, 4) & 0x15) != 0) {
                    Game_Helper46d8c1(ih, ig, 2);
                }
            } else {
                g_game8c83ec = 1;
            }
            g_gameSlots[a][b].field36 = 0;
            Game_Helper46d8c1(a, b, 1);
        }
        return 0;
    }
}

// FUNCTION: SHANDALAR 0x004b6a32
int Game_CardScript4b6a32(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        Game_Helper4c4263(0);
        if (g_game8e2688 != -1) {
            if (Game_Helper4bea85(g_game8e2688, g_game8e2674, 0, a, 2, 2, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 2, 0, 0) != 0) {
            }
            return 0;
        } else {
            return 0;
        }
    } else {
        if (kind == 0x6c && g_game951bdc == b && g_game8e1aa8 == a) {
            if (g_game8e2688 != -1) {
                g_gameSlots[a][b].field74 = g_game8e2688;
                g_gameSlots[a][b].field78 = g_game8e2674;
                g_gameSlots[a][b].field36 = 1;
            } else {
                g_game8c83ec = 1;
            }
            g_game94d63c -= 0x24;
        }
        if (kind == 0x38 && g_game77d160[a][6] >= 2) {
            g_game94d63c += 0x18;
        }
        if (kind == 0x71) {
            if (Game_Helper4bea85(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78, 0, a, 2, 2, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 2, 0, 0) != 0) {
                v8 = g_gameSlots[a][b].field74;
                v4 = g_gameSlots[a][b].field78;
                if ((g_gameSlots[v8][v4].flags08 & 0x20) != 0) {
                    Game_Helper46d8c1(v8, v4, 1);
                }
            } else {
                g_game8c83ec = 1;
            }
            g_gameSlots[a][b].field36 = 0;
            Game_Helper46d8c1(a, b, 1);
        }
        return 0;
    }
}

// FUNCTION: SHANDALAR 0x004b7a5b
int Game_CardScript4b7a5b(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (kind == 0x74) {
        if (g_game8e2688 == -1) {
            Game_Helper4c4263(0);
            return Game_Helper4be650(0, 0, a, 2, 2, 0x200, 0x1047, 0, 0, Game_Helper488cc4(a, b), 1 << Game_Helper4ba392(a, b, 2), 0, -1, -1, -1, -1, 0, 0, 0);
        } else {
            if (Game_Helper4bea85(g_game8e2688, g_game8e2674, 0, a, 2, 2, 0, 0, 0, 0, 0, 1 << Game_Helper4ba392(a, b, 2), 0, -1, -1, -1, -1, 2, 0, 0) != 0) {
            }
            return 0;
        }
    } else {
        if (kind == 0x6c && g_game951bdc == b && g_game8e1aa8 == a) {
            if (g_game8e2688 != -1) {
                g_gameSlots[a][b].field74 = g_game8e2688;
                g_gameSlots[a][b].field78 = g_game8e2674;
                g_gameSlots[a][b].field36 = 1;
            } else {
                Game_Helper56ca10(0x589158, 0x589144);
                if (Game_Helper4c0efa(a, 2, 1 - a, 0x200, 0x1047, 0, 0, Game_Helper488cc4(a, b), 1 << Game_Helper4ba392(a, b, 2), 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &ih) != 0) {
                    g_gameSlots[a][b].field74 = ih;
                    g_gameSlots[a][b].field78 = ig;
                    g_gameSlots[a][b].field36 = 1;
                } else {
                    g_game8c83ec = 1;
                }
            }
        }
        if (kind == 0x71) {
            iv = 0;
            if (g_game8e2688 != -1) {
                if (Game_Helper4bea85(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78, 0, a, 2, 2, 0, 0, 0, 0, 0, 1 << Game_Helper4ba392(a, b, 2), 0, -1, -1, -1, -1, 2, 0, 0) != 0) {
                    iv++;
                }
            } else {
                if (Game_Helper4bea85(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78, 0, a, 2, 2, 0x200, 0, 0, 0, Game_Helper488cc4(a, b), 1 << Game_Helper4ba392(a, b, 2), 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                    iv++;
                }
            }
            if (iv != 0) {
                ih = g_gameSlots[a][b].field74;
                ig = g_gameSlots[a][b].field78;
                if ((1 << Game_Helper4ba392(a, b, 2) & 0x15) != 0) {
                    Game_Helper46d8c1(ih, ig, 2);
                }
            } else {
                g_game8c83ec = 1;
            }
            g_gameSlots[a][b].field36 = 0;
            Game_Helper46d8c1(a, b, 1);
        }
        return 0;
    }
}

// FUNCTION: SHANDALAR 0x004b97b4
int Game_CardScript4b97b4(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        return 1;
    } else {
        if (kind == 0x6c && g_game951bdc == b && g_game8e1aa8 == a) {
            g_gameSlots[a][b].field38 = 6;
        }
        if (kind == 0x71) {
            v4 = a;
            while (v4 > -1 && v4 < 2) {
                Game_Helper4bab38(v4, g_gameSlots[a][b].field38, a, b);
                v8 = 0;
                for (; g_game78e970[v4] > v8; v8++) {
                    if (Game_Helper411d90(v4, v8) != 0) {
                        if ((g_gameCards[g_gameSlots[v4][v8].field6c].flags & 0x2) != 0) {
                            Game_Helper4ba5a1(v4, v8, g_gameSlots[a][b].field38, a, b);
                        }
                    }
                }
                if (a != 0) {
                    v4--;
                } else {
                    v4++;
                }
            }
            Game_Helper46d8c1(a, b, 1);
        }
        return 0;
    }
}
