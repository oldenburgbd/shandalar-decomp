/* Card behaviour scripts at 0x4Axxxx.
 *
 * Split into per-range translation units because MSVC 4.2 re-rolls codegen
 * across a whole TU when anything in it changes; smaller units confine the
 * blast radius. Formulations follow the archived attempt, whose per-function
 * reccmp scores were validated to reproduce exactly under this toolchain.
 *
 * Address literals passed as ints are pointers into the original image's
 * data and are intentional. CUR/SRC live in game/gameslot.h. */

#include "game/gameslot.h"


// FUNCTION: SHANDALAR 0x004a3190
int Game_CardScript4a3190(int a, int b, int kind)
{
    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x71) {
        Game_Helper488b70(a, b, (int)Game_Callback480f45, g_game7a82e0);
        Game_Helper46d8c1(a, b, 1);
    }
    if (kind == 0x3b) {
        if (Game_Helper441699(a, 5, 2) != 0 && Game_Helper441699(a, 7, 3) != 0) {
            g_game7beb10[a] += 2;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004ae290
int Game_CardScript4ae290(int a, int b, int kind)
{
    if (kind == 0x22
        && g_gameSlots[a][b].field4c == 0) {
        Game_Helper4ba2ed(g_gameSlots[a][b].field20);
        Game_Helper46d8c1(a, b, 1);
        g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field28 |= 0x1000000;
        Game_Helper40d7b2((signed char)g_gameSlots[a][b].field50, g_gameSlots[a][b].field04, 0x3c, -1);
        Game_Helper488b70(a, b, (int)Game_Callback4ae506, -1);
        Game_Helper55912c();
    }
    if (kind == 0x3c
        && (g_game8c83fa & 0x2) == 0
        && g_gameSlots[a][b].field04 == g_game951bdc
        && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8
        && g_game951bdc != -1) {
        g_game7a01e0 = g_gameSlots[a][b].field20;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004aeac7
int Game_CardScript4aeac7(int a, int b, int kind)
{
    if (kind == 0x21) {
        if (g_game94d464 != 0x1a) {
            if (g_game94d464 == 0x19) {
                if ((signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field50 == (signed char)g_gameSlots[a][b].field50
                    && g_gameSlots[g_game8e1aa8][g_game951bdc].field04 == g_gameSlots[a][b].field04) {
                    g_gameSlots[g_game8e1aa8][g_game951bdc].field38 = 0;
                }
                if ((signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field0c == (signed char)g_gameSlots[a][b].field50
                    && g_gameSlots[g_game8e1aa8][g_game951bdc].field48 == g_gameSlots[a][b].field04) {
                    g_gameSlots[g_game8e1aa8][g_game951bdc].field38 = 0;
                }
            }
            if (g_game94d178 != 0xcc) {
                if (kind == 0xc7
                    && g_game951bdc == b
                    && g_game8e1aa8 == a) {
                    if (kind == 0x7d) {
                        g_game7a01e0 |= 2;
                    }
                    if (kind != 0x7e) {
                        if (kind == 0xc7) {
                            Game_Helper46d8c1(a, b, 1);
                        }
                    }
                }
            }
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004abf34
int Game_CardScript4abf34(int a, int b, int kind)
{
    if (kind == 0x73) {
        Game_Helper441699(a, 7, 1);
    } else {
        if (kind == 0x6d
            && g_game951bdc == b
            && a == g_game8e1aa8
            && Game_Helper441699(a, 7, 1) != 0) {
            Game_Helper4143e0(a, 0, 1);
        }
        if (kind == 0x72) {
            Game_Helper46d8c1(g_game94f280, g_game930f5c, 4);
        }
        if (g_game94d178 != 0xce) {
            if (kind == 0xc7
                && g_game951bdc == b
                && a == g_game8e1aa8
                && a == g_game7a82e0
                && a == g_game8c8afc
                && g_gameSlots[a][b].field38 == 0) {
                if (kind == 0x7d) {
                    g_game7a01e0 |= 2;
                }
                if (kind != 0x7e) {
                    if (kind == 0xc7) {
                        Game_Helper56ca10(0x588e34, 0x588e28);
                        Game_Helper558538(a, a, b, -1, -1, 0x8aa920, 0);
                        Game_Helper4bab38(a, 1, a, b);
                        Game_Helper46d8c1(a, b, 2);
                    }
                    if (kind != 0x22) {
                        if (kind == 0xc7) {
                            if (g_game8bd200 == 1) {
                                Game_Helper4bab38(a, 1, a, b);
                            }
                            g_gameSlots[a][b].field38 = 0;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004ac273
int Game_CardScript4ac273(int a, int b, int kind)
{
    if (g_game94d178 != 0xcc) {
        if (kind == 0xc7
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            if (kind == 0x7d) {
                g_game7a01e0 |= 2;
            }
            if (kind != 0x7e) {
                if (kind == 0xc7) {
                    if ((signed char)g_gameSlots[a][b].field50 != -1) {
                        Game_Helper4bab38(a, 5, a, b);
                    }
                    if (Game_Helper411d90((signed char)g_gameSlots[a][b].field0c, g_gameSlots[a][b].field48) != 0) {
                        Game_Helper46d8c1((signed char)g_gameSlots[a][b].field0c, g_gameSlots[a][b].field48, 1);
                    }
                    Game_Helper46d8c1(a, b, 1);
                }
            }
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004ae6a4
int Game_CardScript4ae6a4(int a, int b, int kind)
{
    if ((g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags18 & 0x80) != 0) {
        g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field68 = 4;
    }
    if (kind == 0x22 || kind == 0xc7) {
        if ((signed char)g_gameSlots[a][b].field50 != -1) {
            g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field28 = 0x8000000;
        }
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004aed84
int Game_CardScript4aed84(int a, int b, int kind)
{
    if (kind == 0x3c
        && (g_game8c83fa & 0x2) == 0
        && g_gameSlots[a][b].field04 == g_game951bdc
        && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8
        && g_game951bdc != -1
        && g_gameSlots[a][b].field20 != -1) {
        g_game7a01e0 = g_gameSlots[a][b].field20;
        g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags18 |= 0x40;
    }
    if (g_game94d178 != 0xc9) {
        if (kind == 0xc7
            && g_game951bdc == b
            && g_game8e1aa8 == a
            && g_game7a82e0 == a
            && a == g_game8c8afc) {
            if (kind == 0x7d) {
                g_game7a01e0 |= 2;
            }
            if (kind != 0x7e) {
                if (kind == 0xc7) {
                    Game_Helper4ba2ed(g_gameSlots[a][b].field20);
                    g_gameSlots[a][b].field20 = -1;
                    g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field28 |= 0x1000000;
                    Game_Helper40d7b2((signed char)g_gameSlots[a][b].field50, g_gameSlots[a][b].field04, 0x3c, -1);
                    Game_Helper55912c();
                    Game_Helper46d8c1(a, b, 1);
                }
            }
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004aa306
int Game_CardScript4aa306(int a, int b, int kind)
{
    if ((kind == 0x22 || kind == 0xc7)
        && (signed char)g_gameSlots[a][b].field50 == g_game7a82e0) {
        if (g_gameSlots[a][b].field38 != 0) {
            g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags18 &= ~0x8000;
            Game_Helper46d8c1(a, b, 1);
            return 0;
        }
        g_gameSlots[a][b].field38++;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004ac420
int Game_CardScript4ac420(int a, int b, int kind)
{
    if (kind == 0x32
        && g_gameSlots[a][b].field04 == g_game951bdc
        && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8
        && g_game951bdc != -1
        && Game_Helper411d90((signed char)g_gameSlots[a][b].field50, g_gameSlots[a][b].field04) != 0
        && (g_gameCards[g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field6c].flags & 0x2) != 0) {
        g_game7a01e0 += g_gameSlots[a][b].field38;
    }
    if (kind == 0x33
        && g_gameSlots[a][b].field04 == g_game951bdc
        && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8
        && g_game951bdc != -1
        && Game_Helper411d90((signed char)g_gameSlots[a][b].field50, g_gameSlots[a][b].field04) != 0
        && (g_gameCards[g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field6c].flags & 0x2) != 0) {
        g_game7a01e0 += g_gameSlots[a][b].field4c;
    }
    if (kind == 0x22 || kind == 0xc7) {
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004ac7ed
int Game_CardScript4ac7ed(int a, int b, int kind)
{
    int v4;

    if (kind == 0x21) {
        if (g_game94d464 == 0x1a || g_game94d464 == 0x19) {
            if (g_gameSlots[(signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field0c][g_gameSlots[g_game8e1aa8][g_game951bdc].field48].field6c != -1) {
                v4 = g_gameSlots[(signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field0c][g_gameSlots[g_game8e1aa8][g_game951bdc].field48].field6c;
            } else {
                v4 = g_gameSlots[(signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field0c][g_gameSlots[g_game8e1aa8][g_game951bdc].field48].field3c;
            }
            if ((g_gameCards[v4].flags & 0x2) != 0) {
                g_gameSlots[g_game8e1aa8][g_game951bdc].field38 = 0;
            }
        }
    }
    if (kind == 0x22 || kind == 0xc7) {
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004ac14c
int Game_CardScript4ac14c(int a, int b, int kind)
{
    if ((g_game94d178 == 0xcc || kind == 0xc7)
        && g_game951bdc == b
        && a == g_game8e1aa8) {
        if (kind == 0x7d) {
            g_game7a01e0 |= 2;
        }
        if (kind == 0x7e || kind == 0xc7) {
            if ((signed char)g_gameSlots[a][b].field50 != -1) {
                Game_Helper46d8c1((signed char)g_gameSlots[a][b].field50, g_gameSlots[a][b].field04, 2);
            }
            Game_Helper46d8c1(a, b, 1);
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004afd4c
int Game_CardScript4afd4c(int a, int b, int kind)
{
    if (kind == 0x73
        && g_game94d464 == 0xa
        && a == g_game77c57c
        && g_gameSlots[a][b].field38 == 0
        && g_game94d178 == -1) {
        g_game8c7420 |= 3;
        return 1;
    }
    if (kind == 0x6d) {
        g_gameSlots[a][b].field38 |= 1;
    }
    if (kind == 0x72) {
        Game_Helper46d8c1(g_game94f280, g_game930f5c, 4);
        Game_Helper409a0b(a);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004a3da3
int Game_CardScript4a3da3(int a, int b, int kind)
{
    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        Game_Helper56ca10(0x588060, 0x588050);
        Game_Helper498e8a(a, b, 4);
        if (g_game8c7468 == a) {
            g_game94d63c += 3 - g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].field0e * 12;
        }
    }
    if (kind == 0x71) {
        if (Game_Helper49914d(a, b, kind, 4) != 0) {
            Game_Helper4bab38(a, 2, a, b);
        }
        g_gameSlots[a][b].field36 = 0;
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004aa246
int Game_CardScript4aa246(int a, int b, int kind)
{
    if (kind == 0x78
        && g_gameSlots[a][b].field04 == g_game8e26b0
        && (signed char)g_gameSlots[a][b].field50 == g_game94e928) {
        g_game7a01e0++;
    }
    if (kind == 0x22 || kind == 0xc7) {
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004afe3b
int Game_CardScript4afe3b(int a, int b, int kind)
{
    int v4;

    if (kind == 0x74) {
        Game_Helper4c4263(0);
        if (g_game8e2688 != -1) {
            g_gameSlots[a][b].field38 = g_game8c8274;
            if ((g_gameCards[g_gameSlots[g_game8e2688][g_game8e2674].field6c].flags & 0x18) != 0
                && Game_Helper4bea85(g_game8e2688, g_game8e2674, 0, a, 2, 2, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 2, 0, 0) != 0) {
                return 0x63;
            }
            return 0;
        }
        return 0;
    }
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        if (g_game8e2688 != -1) {
            g_gameSlots[a][b].field74 = g_game8e2688;
            g_gameSlots[a][b].field78 = g_game8e2674;
            g_gameSlots[a][b].field36 = 1;
        } else {
            g_game8c83ec = 1;
        }
    }
    if (kind == 0x71) {
        v4 = Game_FindFreeSlot(a, g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].field6c);
        if (v4 != -1) {
            g_gameSlots[a][v4].field1e = 0x10;
            g_gameSlots[a][v4].flags18 |= 8;
            g_game8c8274 = g_gameSlots[a][b].field38;
            g_game8c83f8 |= 0x400;
            Game_Helper4d1c03(a, v4);
            g_game8c83f8 &= ~0x400;
        }
        g_gameSlots[a][b].field36 = 0;
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004aab29
int Game_CardScript4aab29(int a, int b, int kind)
{
    if (kind == 0x34
        && g_gameSlots[a][b].field04 == g_game951bdc
        && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8
        && g_game951bdc != -1) {
        g_game7a01e0 &= ~g_gameSlots[a][b].field38;
    }
    if (kind == 0x22 || kind == 0xc7) {
        if ((signed char)g_gameSlots[a][b].field50 != -1) {
            g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field28 = 0x8000000;
        }
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004a4e37
int Game_CardScript4a4e37(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        Game_Helper4be650(0, 0, a, a, a, 0x200, 2, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            Game_Helper56ca10(0x5880ec, 0x5880e0);
            g_gameSlots[a][b].field78 = Game_Helper49fb64(a);
            g_gameSlots[a][b].field74 = a;
            Game_Helper46d8c1(a, g_gameSlots[a][b].field78, 3);
        }
        if (kind == 0x71) {
            v4 = g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].field3c;
            v8 = (signed char)g_gameCards[v4].field2b + (signed char)g_gameCards[v4].field2c;
            if ((signed char)g_gameCards[v4].field2c == -1) {
                v8++;
            }
            Game_Helper4415c5(a, 1, v8);
            Game_Helper46d8c1(a, b, 1);
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004ae8b2
int Game_CardScript4ae8b2(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (kind == 0x89) {
        Game_Helper49f3c0((int)Game_Callback4aea1d, -1);
    }
    if ((kind == 0x22 || kind == 0xc7)
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        ih = 1 - a;
        for (ig = 0; g_game78e970[ih] > ig; ig++) {
            iv = g_gameSlots[ih][ig].field6c;
            if (Game_Helper411d90(ih, ig) != 0
                && (g_gameCards[iv].flags & 0x2) != 0
                && (signed char)g_gameCards[iv].field29 != 0
                && (g_gameSlots[ih][ig].flags08 & 0x30040) == 0) {
                Game_Helper46d8c1(ih, ig, 2);
            }
        }
        Game_Helper46d8c1(a, b, 2);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004a3232
int Game_CardScript4a3232(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        if (g_game94d464 == 0x16
            && Game_Helper4be650(0, 0, a, 1 - g_game7a82e0, 1 - g_game7a82e0, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        Game_Helper56ca10(0x587fd8, 0x587fc8);
        if (Game_Helper4c0efa(a, 1 - g_game7a82e0, 1 - g_game7a82e0, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &v8) != 0) {
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
        if (Game_Helper4bea85(v8, v4, 0, a, 1 - g_game7a82e0, 1 - g_game7a82e0, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
            Game_Helper4a9910(a, b, g_game8bcff0, v8, v4);
        } else {
            g_game8c83ec = 1;
        }
        g_gameSlots[a][b].field36 = 0;
        Game_Helper46d8c1(a, b, 2);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004a354a
int Game_CardScript4a354a(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        if ((g_game8c83f8 & 0x4) != 0
            && Game_Helper4be650(0, 0, a, 2, 2, 0x200, 0, 0, 0, 0, 0, 0, g_game93a7d4, -1, -1, -1, 0, 0, 0) != 0) {
            return 0x63;
        }
        return 0;
    }
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        Game_Helper56ca10(0x587ff8, 0x587fe8);
        if (Game_Helper4c0efa(a, 2, 2, 0x200, 0, 0, 0, 0, 0, 0, g_game93a7d4, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &v8) != 0) {
            g_gameSlots[a][b].field74 = v8;
            g_gameSlots[a][b].field78 = v4;
            g_gameSlots[a][b].field36 = 1;
            g_gameSlots[a][b].field38 = g_game8c8274;
        } else {
            g_game8c83ec = 1;
        }
    }
    if (kind == 0x71) {
        v8 = g_gameSlots[a][b].field74;
        v4 = g_gameSlots[a][b].field78;
        if (Game_Helper4bea85(v8, v4, 0, a, 2, 2, 0x200, 0, 0, 0, 0, 0, 0, g_game93a7d4, -1, -1, -1, 0, 0, 0) != 0) {
            if (g_gameSlots[v8][v4].field38 > g_gameSlots[a][b].field38) {
                g_gameSlots[v8][v4].field38 -= -(0 - g_gameSlots[a][b].field38);
            } else {
                g_gameSlots[v8][v4].field38 = 0;
            }
            Game_Helper4a9910(a, b, g_game8e330c, (signed char)g_gameSlots[v8][v4].field50, g_gameSlots[v8][v4].field04);
        } else {
            g_game8c83ec = 1;
        }
        g_gameSlots[a][b].field36 = 0;
        Game_Helper46d8c1(a, b, 2);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004a4784
int Game_CardScript4a4784(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        return Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c && g_game951bdc == b && g_game8e1aa8 == a) {
            g_game94d63c += 0x48;
            Game_Helper56ca10(0x5880d0, 0x5880bc);
            if (Game_Helper4c0efa(a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &v8) != 0) {
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
            if (Game_Helper4bea85(v8, v4, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                if ((g_gameSlots[v8][v4].flags08 & 0x4) != 0) {
                    Game_Helper4a9910(a, b, g_game7bfe7c, v8, v4);
                } else {
                    if ((signed char)g_gameSlots[v8][v4].field24 != -1 && v8 != g_game7a82e0) {
                        Game_Helper4a9910(a, b, g_game8c824c, v8, v4);
                    }
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

// FUNCTION: SHANDALAR 0x004a4ae4
int Game_CardScript4a4ae4(int a, int b, int kind)
{
    if (kind == 0x74) {
        Game_Helper4c4263(0);
        if (g_game8e2688 != -1) {
            if (Game_Helper4bea85(g_game8e2688, g_game8e2674, 0, a, 2, 2, 0, 0x40, 0, 0, 0, 0, 0, -1, -1, -1, -1, 2, 0, 0) != 0) {
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
        if (kind == 0x71) {
            if (Game_Helper4bea85(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78, 0, a, 2, 2, 0, 0x40, 0, 0, 0, 0, 0, -1, -1, -1, -1, 2, 0, 0) != 0) {
                if ((g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].flags08 & 0x20) != 0) {
                    Game_Helper46d8c1(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78, 1);
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

// FUNCTION: SHANDALAR 0x004aded3
int Game_CardScript4aded3(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;
    int ii;
    int ij;

    if ((g_gameSlots[a][b].flags18 & 0x1000000) != 0 && g_gameSlots[a][b].field04 == g_game951bdc && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8 && g_game951bdc != -1 && (kind == 0x32 || kind == 0x33)) {
        ii = 0;
        ih = ii;
        ig = (signed char)g_gameSlots[a][b].field50;
        iv = 0;
        ij = 0;
        for (; g_game78e970[ig] > iv && ij == 0; iv++) {
            if (g_gameSlots[ig][iv].field6c == g_game951968) {
                if (Game_Helper411d90(ig, iv) != 0) {
                    if ((signed char)g_gameSlots[a][b].field50 == (signed char)g_gameSlots[ig][iv].field50 && g_gameSlots[a][b].field04 == g_gameSlots[ig][iv].field04) {
                        ij = 1;
                        ih = -g_gameSlots[ig][iv].field12;
                        ii = -g_gameSlots[ig][iv].field1c;
                    }
                }
            }
        }
        if (kind == 0x32) {
            g_game7a01e0 += g_gameSlots[a][b].field12 + ih;
        } else {
            g_game7a01e0 += g_gameSlots[a][b].field1c + ii;
        }
    }
    if ((g_gameSlots[a][b].flags18 & 0x20) == 0 && (kind == 0x22 || kind == 0xc7)) {
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}
