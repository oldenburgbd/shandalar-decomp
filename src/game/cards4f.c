/* Shandalar game layer: card behaviour scripts at 0x4Fxxxx.
 *
 * Split by address range into small translation units so MSVC codegen
 * perturbations stay confined to the unit being changed.
 *
 * Shared CUR/SRC macros live in game/gameslot.h. */

#include "game/gameslot.h"

// FUNCTION: SHANDALAR 0x004fbb9f
int Game_CardScript4fbb9f(int a, int b, int kind)
{
    if (kind == 0x74) {
        return 1;
    }
    if (g_game7a82e0 == a
        && (g_game8c83f8 & 0x1) != 0
        && g_game94d464 >= 0x14
        && g_game94d464 <= 0x1e) {
        g_game8c83f8 &= ~1;
    }
    if ((g_game94d178 == 0xdb || g_game94d178 == 0xd3)
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && g_game8e3f00 >= 1
        && a == g_game8c8afc
        && g_game7bf4b8 == a
        && g_gameSlots[g_game7bf4b8][g_game8e2fc4].field6c != -1
        && (g_gameCards[g_gameSlots[g_game7bf4b8][g_game8e2fc4].field6c].flags & 0x1) != 0
        && (g_gameSlots[a][b].flags08 & 0x20) == 0) {
        if (kind == 0x7d) {
            g_game7a01e0 |= 2;
        }
        if (kind == 0x7e) {
            Game_Helper4bab38(a, 1, a, b);
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004fc825
int Game_CardScript4fc825(int a, int b, int kind)
{
    if (kind == 0x74) {
        return 1;
    }
    if ((kind == 0x81 || kind == 0x91)
        && (g_gameCards[g_gameSlots[g_game8e1aa8][g_game951bdc].field6c].flags & 0x40) != 0
        && (g_gameSlots[a][b].flags08 & 0x20) == 0) {
        Game_Helper4bab38(g_game8e1aa8, 1, a, b);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004fc8fa
int Game_CardScript4fc8fa(int a, int b, int kind)
{
    if (kind == 0x74) {
        Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c
            && b == g_game951bdc
            && a == g_game8e1aa8) {
            Game_Helper56ca10(0x58b458, 0x58b448);
            if (Game_Helper49f87b(a, a, b) == 0) {
                g_game8c83ec = 1;
            } else {
                g_game8c83ec = 0;
            }
        }
        if (kind == 0x71) {
            if (Game_Helper4bea85(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                g_gameSlots[a][b].field50 = g_gameSlots[a][b].field74;
                g_gameSlots[a][b].field04 = g_gameSlots[a][b].field78;
            } else {
                Game_Helper46d8c1(a, b, 1);
                g_game8c83ec = 1;
            }
            g_gameSlots[a][b].field36 = 0;
        }
        if (kind == 0x78
            && g_gameSlots[a][b].field04 == g_game8e26b0
            && (signed char)g_gameSlots[a][b].field50 == g_game94e928
            && (g_gameSlots[a][b].flags08 & 0x20) == 0
            && (signed char)g_gameCards[g_gameSlots[g_game8e1aa8][g_game951bdc].field6c].field29 != 0) {
            g_game7a01e0++;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004fe0c6
int Game_CardScript4fe0c6(int a, int b, int kind)
{
    if (kind == 0x74) {
        Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            Game_Helper56ca10(0x58b504, 0x58b4f8);
            if (Game_Helper49f87b(a, 2, b) == 0) {
                g_game8c83ec = 1;
            } else {
                g_game8c83ec = 0;
            }
        }
        if (kind == 0x71) {
            if (Game_Helper4bea85(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                g_gameSlots[a][b].field50 = g_gameSlots[a][b].field74;
                g_gameSlots[a][b].field04 = g_gameSlots[a][b].field78;
                Game_Helper4885e4(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78);
            } else {
                Game_Helper46d8c1(a, b, 1);
                g_game8c83ec = 1;
            }
            g_gameSlots[a][b].field36 = 0;
        }
        if (g_game94d178 == 0xd4
            && g_game951bdc == b
            && g_game8e1aa8 == a
            && a == g_game8c8afc
            && g_game7bf4b8 == a
            && g_game8e2fc4 == b
            && (signed char)g_gameSlots[a][b].field50 != -1
            && g_gameSlots[a][b].field04 != -1) {
            if (kind == 0x7d) {
                g_game7a01e0 |= 2;
            }
            if (kind == 0x7e) {
                Game_Helper488474((signed char)g_gameSlots[a][b].field50, g_gameSlots[a][b].field04);
            }
        }
        if (kind == 0x77
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            Game_Helper488474((signed char)g_gameSlots[a][b].field50, g_gameSlots[a][b].field04);
            g_gameSlots[a][b].field04 = -1;
            g_gameSlots[a][b].field50 = g_gameSlots[a][b].field04;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004fb4fd
int Game_CardScript4fb4fd(int a, int b, int kind)
{
    if (kind == 0x74) {
        Game_Helper4be650(0, 0, a, 2, a, 0x200, 1, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            Game_Helper56ca10(0x58b3d0, 0x58b3c4);
            if (Game_Helper49fda3(a, a, b) == 0) {
                g_game8c83ec = 1;
            } else {
                g_game8c83ec = 0;
            }
            if (g_game8c83ec != 1) {
                if (g_gameSlots[a][b].field74 == g_game8c776c) {
                    g_game94d63c -= 0x60;
                }
                if (g_gameSlots[a][b].field74 == g_game8c7468) {
                    g_game94d63c += 0x30;
                }
            }
        }
        if (kind == 0x71) {
            if (Game_Helper4bea85(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78, 0, a, 2, 2, 0x200, 1, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                g_gameSlots[a][b].field50 = g_gameSlots[a][b].field74;
                g_gameSlots[a][b].field04 = g_gameSlots[a][b].field78;
            } else {
                Game_Helper46d8c1(a, b, 1);
                g_game8c83ec = 1;
            }
            g_gameSlots[a][b].field36 = 0;
        }
        if (kind == 0x73) {
            if (g_game94d464 == 4
                && (signed char)g_gameSlots[a][b].field50 == g_game7a82e0
                && g_gameSlots[a][b].field38 == 0
                && Game_Helper4419ca(a, b, 5, 2) != 0) {
                if (a == g_game8c7468
                    && (g_game93a934 & 0x2) == 0) {
                    if ((g_game77ed80[a][5] + 1) / 2 * 20 > Game_Helper522508(0x64) || g_game8c83f0[g_game8c7468] < 5) {
                        g_game8c7420 |= 3;
                    }
                }
                return 1;
            }
            return 0;
        }
        if (kind == 0x6d) {
            g_gameSlots[a][b].field38++;
            Game_Helper417301(a, b, 5, 2);
            if (g_game8c83ec != 1) {
                g_gameSlots[a][b].field74 = (signed char)g_gameSlots[a][b].field50;
                g_gameSlots[a][b].field78 = -1;
                g_gameSlots[a][b].field36 = 1;
            } else {
                g_gameSlots[a][b].field38 = 0;
            }
        }
        if (kind == 0x72) {
            Game_Helper4883aa(g_gameSlots[a][b].field74, 1, g_game94f280, g_game930f5c);
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
        }
        if (kind == 0x22 || kind == 0xc7) {
            g_gameSlots[a][b].field38 = 0;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004fa491
int Game_CardScript4fa491(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && Game_Helper44aa1f(a, g_gameSlots[a][b].field6c, -1) == 0) {
        g_game94d63c += (g_game8e3ec0[7 - a] - g_game8e3ed8[a]) * 12;
    }
    if (kind == 0x82
        && (g_gameCards[g_gameSlots[g_game8e1aa8][g_game951bdc].field6c].flags & 0x40) != 0) {
        g_gameSlots[g_game8e1aa8][g_game951bdc].field11c &= ~2;
    }
    if (g_game94d464 == 1
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        if (kind == 0x7d) {
            if (Game_Helper4be650(0, 0, g_game7a82e0, g_game7a82e0, g_game7a82e0, 0x200, 0x40, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0x800, 0) != 0) {
            } else {
                if (Game_Helper4be650(0, 0, g_game7a82e0, g_game7a82e0, g_game7a82e0, 0x200, 0x40, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0x400, 0) != 0) {
                    g_game7a01e0 |= 2;
                }
            }
        }
        if (kind == 0x7e) {
            if (g_game8c7468 == g_game7a82e0
                && (g_game93a934 & 0x2) == 0) {
                ih = g_game7a82e0;
                ig = Game_Helper4ec5d7(g_game7a82e0, 0x40);
                Game_Helper56ca10(0x58b350, 0x58b340);
                Game_Helper558538(a, a, b, ih, ig, 0x8aa920 + 0x12c, 0);
            } else {
                Game_Helper56ca10(0x58b370, 0x58b360);
                Game_Helper4c0efa(g_game7a82e0, g_game7a82e0, g_game7a82e0, 0x200, 0x40, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0x401, 0, 0x8aa920, 0, &ih);
            }
            g_gameSlots[ih][ig].field11c |= 2;
            for (iv = 0; g_game78e970[g_game7a82e0] > iv; iv++) {
                if (Game_Helper411d90(g_game7a82e0, iv) != 0
                    && (g_gameSlots[g_game7a82e0][iv].flags08 & 0x10) != 0
                    && (g_gameCards[g_gameSlots[g_game7a82e0][iv].field6c].flags & 0x40) != 0
                    && (g_gameSlots[g_game7a82e0][iv].field11c & 0x2) == 0) {
                    g_gameSlots[g_game7a82e0][iv].field11c &= ~1;
                }
            }
        }
    }
    if (kind == 0x22) {
        g_gameSlots[a][b].field38 = 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004fbd52
int Game_CardScript4fbd52(int a, int b, int kind)
{
    int v4;

    if (kind == 0x6c
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        Game_Helper56ca10(0x58b3f0, 0x58b3e0);
    }
    v4 = Game_Helper4e4fa0(a, b, kind, 1 << Game_Helper4ba309(a, b, 2) - 1);
    if (kind == 0x6c
        && b == g_game951bdc
        && a == g_game8e1aa8
        && a == g_game8c7468) {
        if ((g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].field28 & 0x17) != 0 || g_gameSlots[a][b].field74 == g_game8c776c) {
            g_game94d63c -= 0x60;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004fd888
int Game_CardScript4fd888(int a, int b, int kind)
{
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        Game_Helper56ca10(0x58b4d0, 0x58b4c8);
    }
    Game_Helper4e4fa0(a, b, kind, 0x100);
    return 0;
}

// FUNCTION: SHANDALAR 0x004f8890
int Game_CardScript4f8890(int a, int b, int kind)
{
    if (kind == 0x74) {
        return Game_Helper4be650(0, 0, a, 2, 2, 0x200, 0x40, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c && g_game951bdc == b && g_game8e1aa8 == a) {
            Game_Helper56ca10(0x58b2cc, 0x58b2b8);
            if (Game_Helper4a011a(a, 2, b) != 0) {
                if (g_game8c7468 == a && g_game8c83ec != 1) {
                    if (g_gameSlots[a][b].field74 == g_game8c7468) {
                        g_game94d63c -= 0x90;
                    } else {
                        if ((g_gameCards[g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].field6c].flags & 0x2) != 0 || (g_gameCards[g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].field6c].flags3c & 0x1001) != 0) {
                            g_game94d63c += 0x30;
                        }
                    }
                }
            } else {
                g_game8c83ec = 1;
            }
        }
        if (kind == 0x71) {
            if (Game_Helper4bea85(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78, 0, a, 2, 2, 0x200, 0x40, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                g_gameSlots[a][b].field50 = g_gameSlots[a][b].field74;
                g_gameSlots[a][b].field04 = g_gameSlots[a][b].field78;
            } else {
                Game_Helper46d8c1(a, b, 1);
                g_game8c83ec = 1;
            }
            g_gameSlots[a][b].field36 = 0;
        }
        if ((kind == 0x81 || kind == 0x91) && g_gameSlots[a][b].field04 == g_game951bdc && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8 && g_game951bdc != -1 && (g_gameSlots[a][b].flags08 & 0x20) == 0) {
            Game_Helper4bab38((signed char)g_gameSlots[a][b].field50, 2, a, b);
        }
        return 0;
    }
}

// FUNCTION: SHANDALAR 0x004f92f9
int Game_CardScript4f92f9(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        return Game_Helper4be650(0, 0, a, 2, 2, 0x200, 1, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c && g_game951bdc == b && g_game8e1aa8 == a) {
            Game_Helper56ca10(0x58b30c, 0x58b2fc);
            if (Game_Helper4c0efa(a, 2, 2, 0x200, 1, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &v8) != 0) {
                g_gameSlots[a][b].field74 = v8;
                g_gameSlots[a][b].field78 = v4;
                g_gameSlots[a][b].field36 = 1;
                if (a == g_game8c7468) {
                    if ((g_game93a934 & 0x2) == 0) {
                        if (v8 == g_game8c776c) {
                            g_game94d63c -= 0x30;
                        } else {
                            if (Game_Helper4e25e6(a, b, g_gameSlots[a][b].field6c) != 0) {
                                g_game94d63c -= 0x60;
                            } else {
                                if ((g_gameCards[g_gameSlots[v8][v4].field6c].flags & 0x2) != 0) {
                                    g_game94d63c += 0x60;
                                }
                            }
                        }
                    }
                }
            } else {
                g_game8c83ec = 1;
            }
        }
        if (kind == 0x71) {
            if (Game_Helper4bea85(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78, 0, a, 2, 2, 0x200, 1, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                g_gameSlots[a][b].field50 = g_gameSlots[a][b].field74;
                g_gameSlots[a][b].field04 = g_gameSlots[a][b].field78;
                g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags18 |= 0x4000000;
                Game_Helper49f3c0((int)Game_Callback4f9d1d, -1);
            } else {
                Game_Helper46d8c1(a, b, 1);
                g_game8c83ec = 1;
            }
            g_gameSlots[a][b].field36 = 0;
        }
        if (kind == 0x34 && g_gameSlots[a][b].field04 == g_game951bdc && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8 && g_game951bdc != -1) {
            g_game7a01e0 |= 0x20000;
        }
        if (kind == 0x77 && (signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field68 == 2 && g_gameSlots[a][b].field04 == g_game951bdc && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8 && g_game951bdc != -1) {
            g_gameSlots[g_game8e1aa8][g_game951bdc].field68 = 0;
            g_gameSlots[g_game8e1aa8][g_game951bdc].field14 = 0;
            g_game7a01e0++;
        }
        if (kind == 0x77 && g_game951bdc == b && g_game8e1aa8 == a && (signed char)g_gameSlots[a][b].field50 != -1 && g_gameSlots[a][b].field04 != -1 && g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field6c != -1) {
            g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags18 &= ~0x4000000;
        }
        if (g_game94d178 == 0xd4 && g_game951bdc == b && g_game8e1aa8 == a && (signed char)g_gameSlots[a][b].field50 != -1 && g_gameSlots[a][b].field04 != -1 && g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field6c != -1 && g_game7bf4b8 == a && g_game8e2fc4 == b && a == g_game8c8afc) {
            if (kind == 0x7d) {
                g_game7a01e0 |= 2;
            }
            if (kind == 0x7e) {
                g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags18 &= ~0x4000000;
            }
        }
        return 0;
    }
}

// FUNCTION: SHANDALAR 0x004fe9cd
int Game_CardScript4fe9cd(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        return 1;
    } else {
        if (kind == 0x71) {
            v4 = 0;
            v8 = 1 - a;
            for (; g_game78e970[v8] > v4; v4++) {
                if (Game_Helper411d90(v8, v4) != 0) {
                    if ((g_gameCards[g_gameSlots[v8][v4].field6c].flags & 0x40) != 0) {
                        g_game94d63c += 0xc;
                    }
                }
            }
        }
        if ((kind == 0x81 || kind == 0x91) && (g_gameCards[g_gameSlots[g_game8e1aa8][g_game951bdc].field6c].flags & 0x40) != 0 && g_game8e1aa8 != a && (g_gameSlots[a][b].flags08 & 0x20) == 0) {
            Game_Helper4883aa(a, 1, a, b);
        }
        return 0;
    }
}
