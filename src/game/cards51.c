/* Shandalar game layer: card behaviour scripts at 0x51xxxx.
 *
 * Split by address range into small translation units so MSVC codegen
 * perturbations stay confined to the unit being changed.
 *
 * Shared CUR/SRC macros live in game/gameslot.h. */

#include "game/gameslot.h"

/* Two separate blocks (0x22/0xc7 slot reset, 0x34 counter bit); each repeats
 * the same two global tests rather than sharing them. */
// FUNCTION: SHANDALAR 0x00512f6e
int Game_CardScript512f6e(int a, int b, int kind)
{
    if ((kind == 0x22 || kind == 0xc7)
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_gameSlots[a][b].field38 = 0;
    }
    if (kind == 0x34
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_game7a01e0 |= 0x20000;
    }
    return 0;
}

/* Counter bumps when slot bit 4 is clear, OR when it is set but the backing
 * card has bit 1 set; the short-circuit order is visible in codegen. */
// FUNCTION: SHANDALAR 0x00517888
int Game_CardScript517888(int a, int b, int kind)
{
    if (kind == 0xa) {
        if ((g_gameSlots[a][b].flags08 & 0x10) == 0
            || (g_gameCards[g_gameSlots[a][b].field6c].flags & 2) != 0) {
            g_game7a01e0++;
        }
    }
    return 0;
}


// FUNCTION: SHANDALAR 0x00513019
int Game_CardScript513019(int a, int b, int kind)
{
    if (kind == 0x6c
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        Game_Helper49f7b5(a, b, 3);
    }
    if (kind != 0x32) {
        if (kind == 0x33
            && b == g_game951bdc
            && a == g_game8e1aa8) {
            Game_Helper49f838(a, b);
            g_game7a01e0 += Game_Helper49f838(a, b);
        }
        if (kind == 0x73) {
            if (Game_Helper49f838(a, b) > 0) {
                return 1;
            }
            return 0;
        }
        if (kind == 0x90) {
            Game_Helper4c4263(1);
            return 0;
        }
        if (kind == 0x6d
            && Game_Helper49f838(a, b) > 0) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58cf84, 0x58cf78);
            }
            if (Game_Helper498e8a(a, b, 1) != 0) {
                g_gameSlots[a][b].field28 |= 0x6000000;
                Game_Helper49f577(a, b);
            }
        }
        if (kind == 0x72) {
            Game_Helper49914d(a, b, kind, 1);
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0051159c
int Game_CardScript51159c(int a, int b, int kind)
{
    if (kind == 0x85
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && a == g_game7a82e0
        && g_game77c57c == a) {
        g_gameSlots[a][b].field5c |= 1;
    }
    if (kind == 4
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        if (g_game8e3ec8[a] > 0) {
            Game_Helper409ee8(a, 0, 1);
        } else {
            g_game7a01e0 |= 1;
        }
    }
    if (kind == 0x86) {
        Game_Helper56cc4d(0x58ce6c, 0x58ce58);
        Game_Helper558538(a, a, b, -1, -1, 0x8aa920, 0);
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].flags08 |= 0x10;
        Game_Helper4bab38(a, 3, g_game94f280, g_game930f5c);
    }
    if ((kind == 0x22 || kind == 0xc7)
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_gameSlots[a][b].field38 = 0;
    }
    if (kind == 0xc7
        && g_game8e3ec8[a] != 0) {
        Game_Helper4bab38(a, 3, a, b);
        g_game94d63c += 0x60;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0051dd8d
int Game_CardScript51dd8d(int a, int b, int kind)
{
    if (kind == 0x6c
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        g_game94d63c += 0x60;
    }
    if (kind == 0x73) {
        return Game_Helper441699(a, 7, 8) != 0
            && ((g_gameSlots[a][b].flags08 & 0x30000) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0)
            && (g_gameSlots[a][b].flags08 & 0x10) == 0;
    }
    if (kind == 0x90) {
        Game_Helper4c4263(1);
        return 0;
    }
    if (kind == 0x6d
        && Game_Helper441699(a, 7, 8) != 0) {
        Game_Helper4143e0(a, 0, 8);
        if (g_game8c83ec != 1) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58d234, 0x58d224);
            }
            Game_Helper498e8a(a, b, 4);
            if (g_game8c83ec != 1) {
                g_gameSlots[a][b].flags08 |= 0x10;
            }
        }
    }
    if (kind == 0x72) {
        Game_Helper49914d(a, b, kind, 4);
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0051e814
int Game_CardScript51e814(int a, int b, int kind)
{
    if (kind == 0x82
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_gameSlots[a][b].field11c &= ~3;
    }
    if (kind == 0x84
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && (g_gameSlots[a][b].flags08 & 0x10) != 0
        && g_game7a82e0 == a
        && g_game77c57c == a) {
        g_gameSlots[a][b].field5c |= 0x10;
        g_gameSlots[a][b].field2c = (signed char)g_gameSlots[a][b].field2c + 1;
    }
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_gameSlots[a][b].field2c = (signed char)g_gameSlots[a][b].field2c + 1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0051f4af
int Game_CardScript51f4af(int a, int b, int kind)
{
    if (kind == 0x82
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_gameSlots[a][b].field11c &= ~3;
    }
    if (kind == 0x84
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && (g_gameSlots[a][b].flags08 & 0x10) != 0
        && a == g_game7a82e0
        && g_game77c57c == a) {
        g_gameSlots[a][b].field5c |= 0x10;
        g_gameSlots[a][b].field2c = (signed char)g_gameSlots[a][b].field2c + 9;
    }
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_gameSlots[a][b].field2c = (signed char)g_gameSlots[a][b].field2c + 9;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00511395
int Game_CardScript511395(int a, int b, int kind)
{
    int v4;

    if (kind == 0x73) {
        return ((g_gameSlots[a][b].flags08 & 0x30000) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0)
            && (g_gameSlots[a][b].flags08 & 0x10) == 0;
    }
    if (kind == 0x6d
        && (g_gameSlots[a][b].flags08 & 0x10) == 0) {
        g_gameSlots[a][b].flags08 |= 0x10;
        Game_Helper46d8c1(a, b, 4);
    }
    if (kind == 0x72) {
        for (v4 = 0; v4 < 0x1f4; v4++) {
            if (g_game94d930[a][v4] != -1) {
                Game_Helper56c4f7(a, g_game94d930[a][v4]);
                g_game94d930[a][v4] = -1;
            }
        }
        Game_Helper558aea(0, 0x30);
        Game_Helper56c1b5(a, a);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00518d54
int Game_CardScript518d54(int a, int b, int kind)
{
    if (kind == 0x6c
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        g_game94d63c += (g_game77ed80[g_game8c7468][7] - g_game77ed80[g_game8c776c][7]) * 12;
    }
    if ((g_game94d178 == 0xdb || g_game94d178 == 0xd3)
        && b == g_game951bdc
        && a == g_game8e1aa8
        && g_game8c8afc == g_game7a82e0
        && g_gameSlots[g_game7bf4b8][g_game8e2fc4].field6c != -1
        && (g_gameCards[g_gameSlots[g_game7bf4b8][g_game8e2fc4].field6c].flags & 0x1) != 0
        && (g_gameSlots[a][b].flags08 & 0x20) == 0) {
        if ((g_gameSlots[a][b].flags08 & 0x10) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) != 0) {
            if (kind == 0x7d) {
                g_game7a01e0 |= 2;
            }
            if (kind == 0x7e) {
                Game_Helper4bab38(g_game7bf4b8, 2, a, b);
            }
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0051a531
int Game_CardScript51a531(int a, int b, int kind)
{
    int v4;

    if (kind == 0x6c
        && b == g_game951bdc
        && a == g_game8e1aa8
        && a == g_game8c7468
        && (g_game93a934 & 0x2) == 0) {
        v4 = Game_Helper44aa1f(a, g_gameSlots[a][b].field6c, a);
        if (v4 != 0
            && g_game77ed80[g_game8c7468][7] / v4 >= 4) {
            g_game94d63c += 0x30;
        }
    }
    if (kind == 0x73) {
        if (Game_Helper441699(a, 7, 4) != 0) {
            if (((g_gameSlots[a][b].flags08 & 0x30000) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0)
                && (g_gameSlots[a][b].flags08 & 0x10) == 0) {
                return 1;
            }
        }
        return 0;
    }
    if (kind == 0x6d
        && Game_Helper441699(a, 7, 4) != 0) {
        Game_Helper4143e0(a, 0, 4);
        if (g_game8c83ec != 1) {
            g_gameSlots[a][b].flags08 |= 0x10;
        }
    }
    if (kind == 0x72) {
        Game_Helper409a0b(a);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0051aee6
int Game_CardScript51aee6(int a, int b, int kind)
{
    if (kind == 0x6c
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        g_game94d63c += 0xc0 / (g_game77ed80[g_game8c7468][7] + 1);
    }
    if (kind == 0x73) {
        if (((g_gameSlots[a][b].flags08 & 0x30000) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0)
            && (g_gameSlots[a][b].flags08 & 0x10) == 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x6d) {
        g_game94d63c -= 0xc;
        Game_Helper441651(a, 0, 2);
        g_gameSlots[a][b].flags08 |= 0x10;
        g_game93a794 = 0;
    }
    if (kind == 0x7f
        && b == g_game951bdc
        && a == g_game8e1aa8
        && (g_gameSlots[a][b].flags08 & 0x10) == 0) {
        Game_Helper441539(a, 0, 2);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0051c0ad
int Game_CardScript51c0ad(int a, int b, int kind)
{
    int v4;

    if (kind == 0x6c
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        g_game94d63c += 0x30 / Game_Helper4ecec6(g_game8c83f0[g_game8c7468], 1, 0x63);
    }
    if (kind == 0x77
        && b == g_game951bdc
        && a == g_game8e1aa8
        && (g_gameSlots[a][b].flags08 & 0x20) == 0
        && (signed char)g_gameSlots[a][b].field68 != 4) {
        v4 = Game_FindFreeSlot(a, g_game94d474);
        if (v4 != -1) {
            g_gameSlots[a][v4].field3c = g_gameSlots[a][b].field6c;
            g_gameSlots[a][v4].flags08 |= 2;
            g_gameSlots[a][v4].field64 = 0x200;
            g_gameSlots[a][v4].field14 = 0xd5;
            g_gameSlots[a][v4].field68 = 2;
            Game_Helper4111b1(a, v4);
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0051e01b
int Game_CardScript51e01b(int a, int b, int kind)
{
    if (kind == 0x6c
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        g_game94d63c += (g_game77d180[g_game8c7468][3] + g_game77d180[g_game8c7468][3] * 2 + 0xfffffffa) * 4;
    }
    if (kind == 0x73) {
        if (Game_Helper441699(a, 7, 3) != 0) {
            if (((g_gameSlots[a][b].flags08 & 0x30000) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0)
                && (g_gameSlots[a][b].flags08 & 0x10) == 0) {
                return 1;
            }
        }
        return 0;
    }
    if (kind == 0x90) {
        Game_Helper4c4263(1);
        return 0;
    }
    if (kind == 0x6d) {
        Game_Helper4143e0(a, 0, 3);
        if (g_game8c83ec != 1) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58d24c, 0x58d240);
            }
            Game_Helper498e8a(a, b, 1);
            if (g_game8c83ec != 1) {
                g_gameSlots[a][b].flags08 |= 0x10;
            }
        }
    }
    if (kind == 0x72) {
        Game_Helper49914d(a, b, kind, 1);
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0051032b
int Game_CardScript51032b(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;
    int ii;

    if (kind == 0xc7
        && Game_Helper411d90(a, b) != 0) {
        if ((g_gameSlots[a][b].flags08 & 0x1000) != 0) {
            ig = g_game8c776c;
        } else {
            ig = g_game8c7468;
        }
        if (g_game8e3ec8[ig] - 4 <= 0) {
        }
        iv = 0;
        if (iv != 0) {
            ii = g_game8c83f0[ig] / iv;
            if (ig != 0) {
                if (0x18 - ii <= 1) {
                }
                g_game94d63c -= -(0 - (1 + 2) * 8);
            } else {
                if (0x18 - ii <= 1) {
                }
                g_game94d63c += (1 + 2) * 8;
            }
        }
    }
    if (g_game94d178 == 0xcb
        && b == g_game951bdc
        && a == g_game8e1aa8
        && a == g_game8c8afc) {
        if ((g_gameSlots[a][b].flags08 & 0x10) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) != 0) {
            if ((g_gameSlots[a][b].flags08 & 0x1000) != 0) {
                ig = g_game8c776c;
            } else {
                ig = g_game8c7468;
            }
            ih = g_game8e3ec8[ig];
            if (ig == g_game7a82e0
                && ih > 4) {
                if (kind == 0x7d) {
                    g_game7a01e0 |= 2;
                }
                if (kind == 0x7e) {
                    Game_Helper4bab38(ig, ih - 4, a, b);
                }
            }
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00510594
int Game_CardScript510594(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;
    int ii;

    if (kind == 0xc7
        && Game_Helper411d90(a, b) != 0) {
        if ((g_gameSlots[a][b].flags08 & 0x1000) != 0) {
            ig = g_game8c776c;
        } else {
            ig = g_game8c7468;
        }
        if (3 - g_game8e3ec8[ig] <= 0) {
        }
        iv = 0;
        if (iv != 0) {
            ii = g_game8c83f0[ig] / iv;
            if (ig != 0) {
                if (0x18 - ii <= 1) {
                }
                g_game94d63c -= -(0 - (1 + 2) * 8);
            } else {
                if (0x18 - ii <= 1) {
                }
                g_game94d63c += (1 + 2) * 8;
            }
        }
    }
    if (g_game94d178 == 0xcb
        && g_game951bdc == b
        && a == g_game8e1aa8
        && a == g_game8c8afc) {
        if ((g_gameSlots[a][b].flags08 & 0x10) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) != 0) {
            if ((g_gameSlots[a][b].flags08 & 0x1000) != 0) {
                ig = g_game8c776c;
            } else {
                ig = g_game8c7468;
            }
            ih = g_game8e3ec8[ig];
            if (ig == g_game7a82e0
                && ih < 3) {
                if (kind == 0x7d) {
                    g_game7a01e0 |= 2;
                }
                if (kind == 0x7e) {
                    Game_Helper4bab38(ig, 3 - ih, a, b);
                }
            }
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00510802
int Game_CardScript510802(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0xc7
        && Game_Helper411d90(a, b) != 0) {
        if (g_game8e3ec8[a] - 4 <= 0) {
        }
        v4 = 0;
        if (a != 0) {
            if (v4 <= 1) {
            }
            g_game94d63c += (1 + 2) * 8;
        } else {
            if (v4 <= 1) {
            }
            g_game94d63c -= -(0 - (1 + 2) * 8);
        }
    }
    if (g_game94d178 == 0xc9
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && g_game7a82e0 == a
        && g_game8c8afc == a
        && g_game8e3ec8[a] > 4) {
        if ((g_gameSlots[a][b].flags08 & 0x10) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) != 0) {
            if (kind == 0x7d) {
                g_game7a01e0 |= 2;
            }
            if (kind == 0x7e) {
                v8 = g_game8e3ec8[a];
                if (v8 > 4) {
                    Game_Helper4883aa(a, v8 - 4, a, b);
                }
            }
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00517500
int Game_CardScript517500(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x73) {
        if (Game_Helper441699(a, 7, 3) != 0
            && g_game7a82e0 == a) {
            if (((g_gameSlots[a][b].flags08 & 0x30000) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0)
                && (g_gameSlots[a][b].flags08 & 0x10) == 0) {
                return 1;
            }
        }
        return 0;
    }
    if (kind == 0x6d
        && (g_gameSlots[a][b].flags08 & 0x10) == 0
        && Game_Helper441699(a, 7, 3) != 0
        && g_game7a82e0 == a) {
        Game_Helper4143e0(a, 0, 3);
        if (g_game8c83ec != 1) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58d0d8, 0x58d0c4);
            }
            if (Game_Helper4c0efa(a, 2, 1 - a, 0x1000, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &v8) != 0) {
                g_gameSlots[a][b].field74 = v8;
                g_gameSlots[a][b].field78 = v4;
                g_gameSlots[a][b].field36 = 1;
                g_gameSlots[a][b].flags08 |= 0x10;
            } else {
                g_game8c83ec = 1;
            }
        }
    }
    if (kind == 0x72) {
        Game_Helper409ee8(g_gameSlots[a][b].field74, 0, 0);
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00517922
int Game_CardScript517922(int a, int b, int kind)
{
    Game_Helper5179e0(a, b, kind, 2);
    return 0;
}

// FUNCTION: SHANDALAR 0x00517948
int Game_CardScript517948(int a, int b, int kind)
{
    Game_Helper5179e0(a, b, kind, 4);
    return 0;
}

// FUNCTION: SHANDALAR 0x0051796e
int Game_CardScript51796e(int a, int b, int kind)
{
    Game_Helper5179e0(a, b, kind, 3);
    return 0;
}

// FUNCTION: SHANDALAR 0x00517994
int Game_CardScript517994(int a, int b, int kind)
{
    Game_Helper5179e0(a, b, kind, 5);
    return 0;
}

// FUNCTION: SHANDALAR 0x005179ba
int Game_CardScript5179ba(int a, int b, int kind)
{
    Game_Helper5179e0(a, b, kind, 1);
    return 0;
}

// FUNCTION: SHANDALAR 0x005189c3
int Game_CardScript5189c3(int a, int b, int kind)
{
    Game_Helper518a81(a, b, kind, 4);
    return 0;
}

// FUNCTION: SHANDALAR 0x005189e9
int Game_CardScript5189e9(int a, int b, int kind)
{
    Game_Helper518a81(a, b, kind, 5);
    return 0;
}

// FUNCTION: SHANDALAR 0x00518a0f
int Game_CardScript518a0f(int a, int b, int kind)
{
    Game_Helper518a81(a, b, kind, 2);
    return 0;
}

// FUNCTION: SHANDALAR 0x00518a35
int Game_CardScript518a35(int a, int b, int kind)
{
    Game_Helper518a81(a, b, kind, 1);
    return 0;
}

// FUNCTION: SHANDALAR 0x00518a5b
int Game_CardScript518a5b(int a, int b, int kind)
{
    Game_Helper518a81(a, b, kind, 3);
    return 0;
}

// FUNCTION: SHANDALAR 0x0051a01c
int Game_CardScript51a01c(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x6c
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        g_game94d63c += g_game8e3320[g_game8c7468][3] * 3;
    }
    if (kind == 0x73) {
        if (((g_gameSlots[a][b].flags08 & 0x30000) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0)
            && (g_gameSlots[a][b].flags08 & 0x10) == 0
            && Game_Helper441699(a, 7, 2) != 0
            && Game_Helper4be650(0, 0, a, a, a, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 2, 0) != 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x90) {
        Game_Helper4c4263(0);
        return 0;
    }
    if (kind == 0x6d
        && (g_gameSlots[a][b].flags08 & 0x10) == 0
        && Game_Helper441699(a, 7, 2) != 0
        && a == g_game7a82e0
        && g_game8ca260 != 0) {
        Game_Helper4143e0(a, 0, 2);
        if (g_game8c83ec != 1) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58d14c, 0x58d140);
            }
            if (Game_Helper4c0efa(a, a, a, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 2, 0, 0x8aa920, 1, &v8) != 0) {
                g_gameSlots[a][b].field74 = v8;
                g_gameSlots[a][b].field78 = v4;
                g_gameSlots[a][b].field36 = 1;
                g_gameSlots[a][b].flags08 |= 0x10;
            } else {
                g_game8c83ec = 1;
            }
        }
    }
    if (kind == 0x72) {
        v8 = g_gameSlots[a][b].field74;
        v4 = g_gameSlots[a][b].field78;
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
        if (Game_Helper4bea85(v8, v4, 0, a, a, a, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 2, 0) != 0) {
            g_gameSlots[v8][v4].flags08 &= ~0x10;
            Game_Helper4a9910(g_game94f280, g_game930f5c, g_game7bf4c4, v8, v4);
            return 0;
        }
        g_game8c83ec = 1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0051baa5
int Game_CardScript51baa5(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x6c
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        g_game94d63c += (g_game8c83f0[g_game8c7468] - g_game8c83f0[g_game8c776c]) * 12;
    }
    if (kind == 0x73) {
        if ((g_game8c83f8 & 0x4) != 0
            && Game_Helper441699(a, 7, 1) != 0
            && (g_gameSlots[a][b].flags08 & 0x10) == 0
            && Game_Helper4be650(0, 1, a, 2, 2, 0x200, 2, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
            return 0x63;
        }
        return 0;
    }
    if (kind == 0x90) {
        Game_Helper4c4263(0);
        return 0;
    }
    if (kind == 0x6d
        && Game_Helper441699(a, 7, 1) != 0) {
        Game_Helper4143e0(a, 0, 1);
        if (g_game8c83ec != 1) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58d188, 0x58d178);
            }
            if (Game_Helper4c0efa(a, 2, a, 0x200, 2, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0x200, 0, 0x8aa920, 1, &v8) != 0) {
                g_gameSlots[a][b].field74 = v8;
                g_gameSlots[a][b].field78 = v4;
                g_gameSlots[a][b].field36 = 1;
            } else {
                g_game8c83ec = 1;
            }
        }
    }
    if (kind == 0x72) {
        v8 = g_gameSlots[a][b].field74;
        v4 = g_gameSlots[a][b].field78;
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
        if (Game_Helper4bea85(v8, v4, 0, a, 2, 2, 0x200, 2, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0x200, 0) != 0) {
            Game_Helper51be77(v8, v4, a, -1);
            return 0;
        }
        g_game8c83ec = 1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0051c7d3
int Game_CardScript51c7d3(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x20010) == 0
            && Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0x20, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x90) {
        Game_Helper4c4263(0);
        return 0;
    }
    if (kind == 0x6d
        && (g_gameSlots[a][b].flags08 & 0x20010) == 0) {
        if (g_game8bd200 != 1) {
            Game_Helper56ca10(0x58d1c4, 0x58d1b0);
        }
        if (Game_Helper4c0efa(a, 2, 1 - a, 0x200, 2, 0, 0x20, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &v8) != 0) {
            g_gameSlots[a][b].field74 = v8;
            g_gameSlots[a][b].field78 = v4;
            g_gameSlots[a][b].field36 = 1;
            g_gameSlots[a][b].flags08 |= 0x10;
        } else {
            g_game8c83ec = 1;
        }
    }
    if (kind == 0x72) {
        v8 = g_gameSlots[a][b].field74;
        v4 = g_gameSlots[a][b].field78;
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
        if (Game_Helper4bea85(v8, v4, 0, a, 2, 2, 0x200, 2, 0, 0x20, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
            Game_AllocSlot(v8, v4, 1, g_game94f280, g_game930f5c);
            return 0;
        }
        g_game8c83ec = 1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0051e2a5
int Game_CardScript51e2a5(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (kind == 0x6c
        && b == g_game951bdc
        && a == g_game8e1aa8
        && Game_Helper44aa1f(a, g_gameSlots[a][b].field6c, -1) == 0) {
        g_game94d63c += (g_game8e3320[g_game8c7468][3] - g_game8e3320[g_game8c776c][3]) * 12;
    }
    if (kind == 0x82
        && (g_gameCards[g_gameSlots[g_game8e1aa8][g_game951bdc].field6c].flags & 0x1) != 0
        && (g_gameSlots[a][b].flags08 & 0x10) == 0
        && (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0) {
        g_gameSlots[g_game8e1aa8][g_game951bdc].field11c &= ~2;
        g_game939e64 |= 1;
    }
    if (g_game94d464 == 1
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        if ((g_gameSlots[a][b].flags08 & 0x10) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) != 0) {
            if (kind == 0x7d) {
                if (Game_Helper4be650(0, 0, g_game7a82e0, g_game7a82e0, g_game7a82e0, 0x200, 1, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0x800, 0) != 0) {
                } else {
                    if (Game_Helper4be650(0, 0, g_game7a82e0, g_game7a82e0, g_game7a82e0, 0x200, 1, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0x400, 0) != 0) {
                        g_game7a01e0 |= 2;
                    }
                }
            }
            if (kind == 0x7e) {
                if (g_game8c7468 == g_game7a82e0
                    && (g_game93a934 & 0x2) == 0) {
                    ih = g_game7a82e0;
                    ig = Game_Helper4ec5d7(g_game7a82e0, 1);
                    Game_Helper56ca10(0x58d264, 0x58d258);
                    Game_Helper558538(a, a, b, ih, ig, 0x8aa920 + 0x12c, 0);
                } else {
                    Game_Helper56ca10(0x58d27c, 0x58d270);
                    Game_Helper4c0efa(g_game7a82e0, g_game7a82e0, g_game7a82e0, 0x200, 1, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0x401, 0, 0x8aa920, 0, &ih);
                }
                g_gameSlots[ih][ig].field11c |= 2;
                for (iv = 0; g_game78e970[g_game7a82e0] > iv; iv++) {
                    if (Game_Helper411d90(g_game7a82e0, iv) != 0
                        && (g_gameSlots[g_game7a82e0][iv].flags08 & 0x10) != 0
                        && (g_gameCards[g_gameSlots[g_game7a82e0][iv].field6c].flags & 0x1) != 0
                        && (g_gameSlots[g_game7a82e0][iv].field11c & 0x2) == 0) {
                        g_gameSlots[g_game7a82e0][iv].field11c &= ~1;
                    }
                }
            }
        }
    }
    if (kind == 0x22) {
        g_gameSlots[a][b].field38 = 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0051ef24
int Game_CardScript51ef24(int a, int b, int kind)
{
    Game_Helper51ef70(a, b, kind, 7);
    return 0;
}

// FUNCTION: SHANDALAR 0x0051ef4a
int Game_CardScript51ef4a(int a, int b, int kind)
{
    Game_Helper51ef70(a, b, kind, 4);
    return 0;
}

// FUNCTION: SHANDALAR 0x0051fd11
int Game_CardScript51fd11(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (kind == 0x6c
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        g_game94d63c += g_game8e3ed0[a] * 12 / 2;
    }
    if (kind == 0x73) {
        if (((g_gameSlots[a][b].flags08 & 0x30000) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0)
            && (g_gameSlots[a][b].flags08 & 0x10) == 0
            && Game_Helper441699(a, 7, 1) != 0
            && Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x90) {
        Game_Helper4c4263(0);
        return 0;
    }
    if (kind == 0x6d
        && Game_Helper441699(a, 7, 1) != 0) {
        Game_Helper4143e0(a, 0, 1);
        if (g_game8c83ec != 1) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58d2b4, 0x58d2a4);
            }
            if (Game_Helper4c0efa(a, 2, a, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &ih) != 0) {
                g_gameSlots[a][b].field74 = ih;
                g_gameSlots[a][b].field78 = ig;
                g_gameSlots[a][b].field36 = 1;
                if (a == g_game8c7468) {
                    if ((g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].field28 & 0x40) != 0 || g_gameSlots[a][b].field74 == g_game8c776c) {
                        g_game94d63c -= 0x30;
                    }
                }
                g_gameSlots[a][b].flags08 |= 0x10;
            } else {
                g_game8c83ec = 1;
            }
        }
    }
    if (kind == 0x72) {
        ih = g_gameSlots[a][b].field74;
        ig = g_gameSlots[a][b].field78;
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
        if (Game_Helper4bea85(ih, ig, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
            iv = Game_Helper4a9910(g_game94f280, g_game930f5c, g_game8aa734, ih, ig);
            if (iv != -1) {
                g_gameSlots[a][iv].field38 = 0x40;
            }
            g_gameSlots[ih][ig].field28 = 0x8000000;
            return 0;
        }
        g_game8c83ec = 1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0051649a
int Game_CardScript51649a(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;
    int ii;

    if (kind == 0x3c) {
        g_gameSlots[a][b].field1f = (signed char)g_gameSlots[a][b].field1f | 0x40;
    }
    if (kind == 0x1a) {
        ih = 1 - a;
        if (g_game7a82e0 == a
            && (g_gameSlots[a][b].flags08 & 0x44) != 0) {
            if ((signed char)g_gameSlots[a][b].field24 == -1) {
                ii = b;
            } else {
                ii = (signed char)g_gameSlots[a][b].field24;
            }
            for (ig = 0; g_game78e970[ih] > ig; ig++) {
                if ((signed char)g_gameSlots[ih][ig].field24 == ii) {
                    iv = g_gameSlots[ih][ig].field6c;
                    if (iv != -1
                        && (signed char)g_gameCards[iv].field29 == 0
                        && (g_gameCards[iv].flags & 0x2) != 0) {
                        Game_Helper4a9910(a, b, g_game8c72b4, ih, ig);
                    }
                }
            }
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00516e6e
int Game_CardScript516e6e(int a, int b, int kind)
{
    if (kind == 0x73 || kind == 0x6d || kind == 0x72) {
        return Game_Helper4909b0(a, b, kind, 0, 2);
    } else {
        return 0;
    }
}

// FUNCTION: SHANDALAR 0x00516ebb
int Game_CardScript516ebb(int a, int b, int kind)
{
    if (kind == 0x73 || kind == 0x6d || kind == 0x72) {
        return Game_Helper4909b0(a, b, kind, 0, 3);
    } else {
        return 0;
    }
}

// FUNCTION: SHANDALAR 0x0051ea11
int Game_CardScript51ea11(int a, int b, int kind)
{
    int v4;

    if (kind == 0x6c && b == g_game951bdc && a == g_game8e1aa8) {
        g_gameSlots[a][b].field38 = 0;
    }
    if (kind == 0x73) {
        return Game_Helper441699(a, 7, 2);
    } else {
        if (kind == 0x90) {
            g_game74b868 = 2;
            return 0;
        } else {
            if (kind == 0x6d) {
                if (Game_Helper441699(a, 7, 2) != 0) {
                    Game_Helper4143e0(a, 0, 2);
                    if (g_game8c83ec != 1) {
                        if (g_gameSlots[a][b].field38 == 0) {
                            g_gameSlots[a][b].field38 |= 0x80000;
                        }
                    }
                }
            }
            if (kind == 0x72) {
                if (g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field6c != -1) {
                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38++;
                    if ((g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 & 0x80000) != 0) {
                        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 &= ~0x80000;
                        v4 = Game_Helper4a9910(g_game94f280, g_game930f5c, g_game8bcff8, g_game94f280, g_game930f5c);
                        if (v4 != -1) {
                            g_gameSlots[a][v4].field12 = 1;
                            g_gameSlots[a][v4].field38 |= 0x80000;
                        }
                    }
                } else {
                    g_game8c83ec = 1;
                }
            }
            if (kind == 0x39) {
                return g_game77d180[a][3] / 2;
            } else {
                if (kind == 0x8f && g_game7806a0[a][7] > 1) {
                    g_game7a01e0 |= 1;
                }
                if (kind == 0xc7) {
                    if (a == g_game8c7468) {
                        g_game94d63c += (g_game77ed80[a][7] / 2 + g_game77ed80[a][7] / 2 * 2 + 3) * 4;
                    } else {
                        g_game94d63c -= -(0 - (g_game77ed80[a][7] / 2 + g_game77ed80[a][7] / 2 * 2 + 3) * 4);
                    }
                }
                if (kind == 0x22 || kind == 0xc7) {
                    g_gameSlots[a][b].field38 = 0;
                }
                return 0;
            }
        }
    }
}

// FUNCTION: SHANDALAR 0x005109c7
int Game_CardScript5109c7(int a, int b, int kind)
{
    int v4;

    if (kind == 0x1f && ((g_gameSlots[a][b].flags08 & 0x10) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) != 0)) {
        if ((g_gameSlots[a][b].flags08 & 0x1000) != 0) {
            v4 = g_game8c776c;
        } else {
            v4 = g_game8c7468;
        }
        if (v4 == g_game7a82e0 && g_game8e3ec8[g_game7a82e0] > 4) {
            g_game7a01e0 |= 1;
            while (g_game8e3ec8[g_game7a82e0] > 4) {
                Game_Helper409ee8(g_game7a82e0, 0, 0);
            }
        }
    }
    return 0;
}
