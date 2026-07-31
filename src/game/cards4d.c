/* Card behaviour scripts in the 0x4Dxxxx range. Split into small translation
 * units on purpose: MSVC re-rolls codegen across a whole TU when anything in
 * it changes, so keeping units small confines the blast radius. Shared
 * CUR/SRC macros live in game/gameslot.h. */

#include "game/gameslot.h"

// FUNCTION: SHANDALAR 0x004dac78
int Game_CardScript4dac78(int a, int b, int kind)
{
    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x32
        && a == g_game8e1aa8
        && (signed char)g_gameCards[g_gameSlots[g_game8e1aa8][g_game951bdc].field6c].field29 == 0
        && Game_Helper411d90(a, b) != 0) {
        if (Game_Helper411d90(g_game8e1aa8, g_game951bdc) != 0) {
            g_game7a01e0++;
        }
        return 0;
    }
    if (kind == 0x34
        && a == g_game8e1aa8
        && (signed char)g_gameCards[g_gameSlots[g_game8e1aa8][g_game951bdc].field6c].field29 == 0
        && Game_Helper411d90(g_game8e1aa8, g_game951bdc) != 0) {
        g_game7a01e0 |= 0x40;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004d0cc6
int Game_CardScript4d0cc6(int a, int b, int kind)
{
    int v4;

    if (kind == 0x7f
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        v4 = Game_Helper4ba392(a, b, 5);
        g_game765420[v4] += 3;
        return 0;
    }
    if (kind == 0x74) {
        return 1;
    }
    if ((kind == 0x6c || kind == 0xc7)
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_game94d63c += g_game77ed80[1 - a][Game_Helper4ba392(a, b, 5)] - g_game77ed80[a][Game_Helper4ba392(a, b, 5)] + g_game77ed80[a][Game_Helper4ba392(a, b, 5)] + g_game77ed80[1 - a][Game_Helper4ba392(a, b, 5)] - g_game77ed80[a][Game_Helper4ba392(a, b, 5)] + g_game77ed80[a][Game_Helper4ba392(a, b, 5)] * 2 + 3 * 4;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004dab86
int Game_CardScript4dab86(int a, int b, int kind)
{
    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x81
        && g_game8e1aa8 != a
        && Game_Helper488147(g_game8e1aa8, g_game951bdc, Game_Helper4ba309(a, b, 3)) != 0) {
        Game_Helper4883aa(a, 1, a, b);
        return 0;
    }
    if ((kind == 0x6c || kind == 0xc7)
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_game94d63c += g_game77ed80[g_game8c776c][Game_Helper4ba309(a, b, 3)] + g_game77ed80[g_game8c776c][Game_Helper4ba309(a, b, 3)] * 2 + 3 * 8;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004db096
int Game_CardScript4db096(int a, int b, int kind)
{
    if (kind == 0x32) {
        if ((g_gameSlots[g_game8e1aa8][g_game951bdc].flags08 & 0x4) != 0
            && a == g_game7a82e0
            && g_game8e1aa8 == a
            && Game_Helper411d90(a, b) != 0) {
            g_game7a01e0++;
        }
        return 0;
    }
    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_game94d63c += g_game8e3ed0[a] * 12;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004db180
int Game_CardScript4db180(int a, int b, int kind)
{
    if ((kind == 0x32 || kind == 0x33)
        && Game_Helper411d90(a, b) != 0) {
        if (Game_Helper411d90(g_game8e1aa8, g_game951bdc) != 0
            && ((1 << Game_Helper4ba392(a, b, 5)) & 0x15) != 0) {
            g_game7a01e0++;
        }
        return 0;
    }
    if (kind == 0x74) {
        return 1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004dafcb
int Game_CardScript4dafcb(int a, int b, int kind)
{
    if ((kind == 0x32 || kind == 0x33)
        && Game_Helper411d90(a, b) != 0
        && Game_Helper411d90(g_game8e1aa8, g_game951bdc) != 0
        && ((1 << Game_Helper4ba392(a, b, 1)) & 0x15) != 0) {
        g_game7a01e0++;
    }
    if (kind == 0x74) {
        return 1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004db6e2
int Game_CardScript4db6e2(int a, int b, int kind)
{
    int v4;

    if (kind == 0x74) {
        return Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58a4a0, 0x58a498);
            }
            if (Game_Helper49f87b(a, a, b) == 0) {
                g_game8c83ec = 1;
            } else {
                g_game8c83ec = 0;
            }
            return 0;
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
            g_gameSlots[a][b].field38 = 0;
            return 0;
        }
        if (g_game94d178 == 0xda
            && g_gameSlots[a][b].field38 == 0) {
            g_gameSlots[a][b].field38 = 1;
            g_game94d178 = -1;
            if ((signed char)g_gameSlots[a][b].field50 == g_game7a82e0
                && (g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags08 & 0x4) != 0
                && Game_Helper411d90(g_game8e1aa8, g_game951bdc) != 0
                && (signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field24 == -1
                && g_game8e1aa8 != g_game7a82e0
                && Game_Helper4dbdfc(g_game8e1aa8, g_game951bdc, (signed char)g_gameSlots[a][b].field50, g_gameSlots[a][b].field04) != 0) {
                if ((signed char)g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field24 == -1) {
                    v4 = g_gameSlots[a][b].field04;
                } else {
                    v4 = (signed char)g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field24;
                }
                if (kind == 0x7d) {
                    g_game7a01e0 |= 2;
                }
                if (kind == 0x7e) {
                    g_gameSlots[g_game8e1aa8][g_game951bdc].field24 = v4;
                    g_gameSlots[g_game8e1aa8][g_game951bdc].flags08 |= 0x8008;
                }
            }
            g_game94d178 = 0xda;
            g_gameSlots[a][b].field38 = 0;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004dc5a1
int Game_CardScript4dc5a1(int a, int b, int kind)
{
    int v4;

    if (kind == 0x74) {
        return Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58a4d4, 0x58a4c4);
            }
            if (Game_Helper49f87b(a, 1 - a, b) == 0) {
                g_game8c83ec = 1;
            } else {
                g_game8c83ec = 0;
            }
            if (g_game8c83ec != 1) {
                if (g_gameSlots[a][b].field74 == g_game8c776c) {
                    g_game94d63c += 0x18;
                }
                if (g_gameSlots[a][b].field74 == g_game8c7468) {
                    g_game94d63c -= 0x60;
                }
            }
            return 0;
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
            return 0;
        }
        if (kind == 0x77
            && g_gameSlots[a][b].field04 == g_game951bdc
            && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8
            && g_game951bdc != -1) {
            v4 = Game_FindFreeSlot(a, g_game94d474);
            if (v4 != -1) {
                g_gameSlots[a][v4].field3c = g_gameSlots[a][b].field6c;
                g_gameSlots[a][v4].flags08 |= 2;
                g_gameSlots[a][v4].field64 = 0x32;
                g_gameSlots[a][v4].field38 = Game_Helper40d7b2(g_game8e1aa8, g_game951bdc, 0x33, -1);
                g_gameSlots[a][v4].field50 = g_game8e1aa8;
                Game_Helper4111b1(a, v4);
            }
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004dcace
int Game_CardScript4dcace(int a, int b, int kind)
{
    if (kind == 0x74) {
        return Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58a4ec, 0x58a4e0);
            }
            if (Game_Helper49f87b(a, 2, b) == 0) {
                g_game8c83ec = 1;
            } else {
                g_game8c83ec = 0;
            }
            return 0;
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
            return 0;
        }
        if (kind == 0x21) {
            if ((g_game94d464 == 0x1a || g_game94d464 == 0x19)
                && g_gameSlots[g_game8e1aa8][g_game951bdc].field6c == g_game93a7d4) {
                if ((signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field50 == (signed char)g_gameSlots[a][b].field50
                    && g_gameSlots[g_game8e1aa8][g_game951bdc].field04 == g_gameSlots[a][b].field04) {
                    g_gameSlots[g_game8e1aa8][g_game951bdc].field37 = g_gameSlots[g_game8e1aa8][g_game951bdc].field38;
                    g_gameSlots[g_game8e1aa8][g_game951bdc].field38 = 0;
                }
                if ((signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field0c == (signed char)g_gameSlots[a][b].field50
                    && g_gameSlots[g_game8e1aa8][g_game951bdc].field48 == g_gameSlots[a][b].field04) {
                    g_gameSlots[g_game8e1aa8][g_game951bdc].field37 = g_gameSlots[g_game8e1aa8][g_game951bdc].field38;
                    g_gameSlots[g_game8e1aa8][g_game951bdc].field38 = 0;
                }
            }
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004d829b
int Game_CardScript4d829b(int a, int b, int kind)
{
    if (kind == 0x74) {
        Game_Helper4be650(0, 0, a, 2, 2, 0x200, 1, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58a408, 0x58a3fc);
            }
            if (Game_Helper49fda3(a, 1 - a, b) == 0) {
                g_game8c83ec = 1;
            } else {
                g_game8c83ec = 0;
            }
            if (g_game8c83ec != 1) {
                if (g_gameSlots[a][b].field74 == g_game8c776c) {
                    g_game94d63c += 0x30;
                }
                if (g_gameSlots[a][b].field74 == g_game8c7468) {
                    g_game94d63c -= 0x60;
                }
            }
            return 0;
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
            return 0;
        }
        if (kind == 0x73) {
            if (g_game94d464 == 4
                && g_game77c57c == g_game7a82e0
                && (signed char)g_gameSlots[a][b].field50 == g_game7a82e0
                && (g_gameSlots[a][b].field38 & 0x1) == 0) {
                g_gameSlots[a][b].field5c |= 0x101;
                g_game8c7420 |= 3;
                return 1;
            }
            return 0;
        }
        if (kind == 4
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            g_gameSlots[a][b].field38 |= 1;
            g_game7be9ac = 1;
            g_game7a01e0 |= 1;
            return 0;
        }
        if (kind == 0x86) {
            Game_Helper4bab38((signed char)g_gameSlots[a][b].field50, 1, g_game94f280, g_game930f5c);
            return 0;
        }
        if (kind == 0x22) {
            g_gameSlots[a][b].field38 &= ~1;
            return 0;
        }
        if (kind == 0xc7
            && Game_Helper411d90(a, b) != 0
            && (signed char)g_gameSlots[a][b].field50 != -1) {
            if ((signed char)g_gameSlots[a][b].field50 == g_game8c776c) {
                if (0x18 - g_game8c83f0[(signed char)g_gameSlots[a][b].field50] <= 1) {
                }
                g_game94d63c += (1 + 2) * 8;
                return 0;
            }
            if (0x18 - g_game8c83f0[(signed char)g_gameSlots[a][b].field50] <= 1) {
            }
            g_game94d63c -= -(0 - (1 + 2) * 8);
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004d8c4a
int Game_CardScript4d8c4a(int a, int b, int kind)
{
    if (kind == 0x74) {
        Game_Helper4be650(0, 0, a, 2, 2, 0x200, 1, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58a424, 0x58a414);
            }
            if (Game_Helper49fda3(a, 1 - a, b) == 0) {
                g_game8c83ec = 1;
            } else {
                g_game8c83ec = 0;
            }
            if (g_game8c83ec != 1) {
                if (g_gameSlots[a][b].field74 == g_game8c776c) {
                    g_game94d63c += 0x40 / (g_game77ed80[g_game8c776c][7] + 1);
                }
                if (g_gameSlots[a][b].field74 == g_game8c7468) {
                    g_game94d63c -= 0x60;
                }
                if (g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].field6c == 0) {
                    g_game94d63c -= 0x90;
                }
            }
            return 0;
        }
        if (kind == 0x71) {
            if (Game_Helper4bea85(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78, 0, a, 2, 2, 0x200, 1, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                g_gameSlots[a][b].field50 = g_gameSlots[a][b].field74;
                g_gameSlots[a][b].field04 = g_gameSlots[a][b].field78;
                g_gameSlots[a][b].field38 = 1;
                g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field6c = Game_Helper4ba309(a, b, g_gameSlots[a][b].field38) - 1;
                g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field28 |= 0x1000000;
            } else {
                Game_Helper46d8c1(a, b, 1);
                g_game8c83ec = 1;
            }
            g_gameSlots[a][b].field36 = 0;
            return 0;
        }
        if (kind == 0x3c
            && (g_game8c83fa & 0x2) == 0
            && g_gameSlots[a][b].field04 == g_game951bdc
            && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8
            && g_game951bdc != -1
            && Game_Helper411d90(a, b) != 0) {
            g_game7a01e0 = Game_Helper4ba309(a, b, g_gameSlots[a][b].field38) - 1;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004d92a4
int Game_CardScript4d92a4(int a, int b, int kind)
{
    int v4;

    if (kind == 0x6e
        && g_gameSlots[g_game8e1aa8][g_game951bdc].field6c == g_game93a7d4) {
        if ((signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field50 == a
            && g_gameSlots[g_game8e1aa8][g_game951bdc].field04 == -1
            && g_gameSlots[g_game8e1aa8][g_game951bdc].field38 != 0) {
            g_gameSlots[a][b].field4c += g_gameSlots[g_game8e1aa8][g_game951bdc].field38;
        }
        return 0;
    }
    if (g_game94d178 == 0xd7
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && g_gameSlots[a][b].field4c != 0
        && a == g_game8c8afc) {
        if (kind == 0x7d) {
            g_game7a01e0 |= 2;
        }
        if (kind == 0x7e) {
            Game_Helper49f61a(a, b, g_gameSlots[a][b].field4c);
            g_gameSlots[a][b].field4c = 0;
        }
    }
    if (kind == 0x74) {
        Game_Helper4be650(0, 0, a, 2, 2, 0x200, 0x40, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58a440, 0x58a430);
            }
            if (Game_Helper4a011a(a, 2, b) == 0) {
                g_game8c83ec = 1;
            } else {
                g_game8c83ec = 0;
            }
            if (g_game8c83ec != 1) {
                if (g_game8c83f0[1 - a] <= g_game8c83f0[a]) {
                    v4 = 1;
                } else {
                    v4 = 3;
                }
                if (g_gameSlots[a][b].field74 == g_game8c776c) {
                    g_game94d63c += ((signed char)g_gameCards[g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].field6c].field2c * v4 + (signed char)g_gameCards[g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].field6c].field2c * v4) * 8;
                }
                if (g_gameSlots[a][b].field74 == g_game8c7468) {
                    g_game94d63c += v4 * 24 / 2;
                }
            }
            return 0;
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
            return 0;
        }
        if (kind == 0x73) {
            if (g_game94d464 == 4
                && a == g_game7a82e0
                && g_game77c57c == a
                && g_gameSlots[a][b].field38 == 0
                && Game_Helper49f838(a, b) != 0) {
                if (g_game765420[Game_Helper40dffd((signed char)g_gameSlots[a][b].field1e)] != 0
                    && Game_Helper4419ca(a, b, 7, 0) == 0) {
                    return 0;
                }
                if (g_game8c7468 == a
                    && (g_game93a934 & 0x2) == 0) {
                    g_game8c7420 |= 3;
                }
                return 1;
            }
            return 0;
        }
        if (kind == 0x6d
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            if (g_game765420[Game_Helper40dffd((signed char)g_gameSlots[a][b].field1e)] != 0) {
                Game_Helper417301(a, b, 0, 0);
            }
            if (g_game8c83ec != 1) {
                g_gameSlots[a][b].field38++;
                Game_Helper49f577(a, b);
            }
            return 0;
        }
        if (kind == 0x72) {
            Game_Helper4883aa(a, 1, g_game94f280, g_game930f5c);
            return 0;
        }
        if (kind == 0x22 || kind == 0xc7) {
            g_gameSlots[a][b].field38 = 0;
            g_gameSlots[a][b].field4c = 0;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004da7c9
int Game_CardScript4da7c9(int a, int b, int kind)
{
    if (kind == 0x81
        && Game_Helper411d90(a, b) != 0) {
        if ((g_gameCards[g_gameSlots[g_game8e1aa8][g_game951bdc].field6c].flags & 0x1) != 0
            && g_game93a794 != -1) {
            Game_Helper4bab38(g_game8e1aa8, 1, a, b);
        }
        return 0;
    }
    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x6c
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        g_game94d63c += (g_game77ed80[g_game8c7468][7] - g_game77ed80[g_game8c776c][7]) * 24;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004db252
int Game_CardScript4db252(int a, int b, int kind)
{
    if (kind == 0x74) {
        Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58a48c, 0x58a47c);
            }
            if (Game_Helper49f87b(a, a, b) == 0) {
                g_game8c83ec = 1;
            } else {
                g_game8c83ec = 0;
            }
            if (g_game8c83ec != 1
                && g_gameSlots[a][b].field74 == g_game8c776c) {
                g_game94d63c -= 0x18;
            }
            return 0;
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
            return 0;
        }
        if (kind == 0x32
            && g_gameSlots[a][b].field04 == g_game951bdc
            && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8
            && g_game951bdc != -1
            && Game_Helper411d90(a, b) != 0) {
            g_game7a01e0 += g_game77ed80[a][Game_Helper4ba309(a, b, 3)] / 2;
        }
        if (kind == 0x33
            && g_gameSlots[a][b].field04 == g_game951bdc
            && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8
            && g_game951bdc != -1
            && Game_Helper411d90(a, b) != 0) {
            g_game7a01e0 += (g_game77ed80[a][Game_Helper4ba309(a, b, 3)] + 1) / 2;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004d8911
int Game_CardScript4d8911(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x73) {
        if (g_game94d464 == 4
            && (g_gameSlots[a][b].field38 & 0x1) == 0
            && g_game77c57c == g_game7a82e0
            && g_game77ed80[g_game77c57c][Game_Helper4ba309(a, b, 1)] != 0) {
            g_gameSlots[a][b].field5c |= 0x101;
            g_game8c7420 |= 3;
            return 1;
        }
        return 0;
    }
    if (kind == 4
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_gameSlots[a][b].field38 |= 1;
        g_game7be9ac = 1;
        g_game7a01e0 |= 1;
        return 0;
    }
    if (kind == 0x86) {
        Game_Helper4bab38(g_game7a82e0, g_game77ed80[g_game7a82e0][Game_Helper4ba309(a, b, 1)], g_game94f280, g_game930f5c);
        return 0;
    }
    if (kind == 0x22) {
        g_gameSlots[a][b].field38 &= ~1;
    }
    if (kind == 0xc7) {
        ig = 1 - g_game7a82e0;
        Game_Helper4bab38(ig, g_game77ed80[ig][Game_Helper4ba309(a, b, 1)], a, b);
    }
    if (kind == 0xc7
        && Game_Helper411d90(a, b) != 0) {
        iv = g_game77ed80[g_game8c776c][Game_Helper4ba309(a, b, 1)];
        if (iv != 0) {
            ih = g_game8c83f0[g_game8c776c] / iv;
            if (0x18 - ih <= 1) {
            }
            g_game94d63c += (1 + 2) * 8;
        }
        iv = g_game77ed80[g_game8c7468][Game_Helper4ba309(a, b, 1)];
        if (iv != 0) {
            ih = g_game8c83f0[g_game8c7468] / iv;
            if (0x18 - ih <= 1) {
            }
            g_game94d63c -= -(0 - (1 + 2) * 8);
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004d2d45
int Game_CardScript4d2d45(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        return Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 1, 0, 0);
    } else {
        if (kind == 0x6c
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58a2ac, 0x58a29c);
            }
            if (Game_Helper4c0efa(a, 2, a, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 1, 0, 0, 0x8aa920, 1, &v8) != 0) {
                g_gameSlots[a][b].field74 = v8;
                g_gameSlots[a][b].field78 = v4;
                g_gameSlots[a][b].field36 = 1;
            } else {
                g_game8c83ec = 1;
            }
            return 0;
        }
        if (kind == 0x71) {
            if (Game_Helper4bea85(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 1, 0, 0) != 0) {
                g_gameSlots[a][b].field50 = g_gameSlots[a][b].field74;
                g_gameSlots[a][b].field04 = g_gameSlots[a][b].field78;
            } else {
                Game_Helper46d8c1(a, b, 1);
                g_game8c83ec = 1;
            }
            g_gameSlots[a][b].field36 = 0;
            return 0;
        }
        if (kind == 0x34
            && g_gameSlots[a][b].field04 == g_game951bdc
            && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8
            && g_game951bdc != -1
            && (g_gameSlots[a][b].flags18 & 0x80) == 0) {
            g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags18 |= 0x800;
        }
        if (kind == 0x77
            && g_game951bdc == b
            && g_game8e1aa8 == a
            && g_gameSlots[a][b].field04 != -1) {
            g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags18 &= ~0x800;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004d32db
int Game_CardScript4d32db(int a, int b, int kind)
{
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && g_game8bd200 != 1) {
        Game_Helper56ca10(0x58a2c8, 0x58a2b8);
    }
    return Game_Helper4d34d6(a, b, kind, 2);
}

// FUNCTION: SHANDALAR 0x004d3348
int Game_CardScript4d3348(int a, int b, int kind)
{
    if (kind != 0x3c
        && (g_gameSlots[a][b].flags18 & 0x80) == 0
        && (signed char)g_gameSlots[a][b].field50 != -1
        && (g_gameCards[g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field6c].flags & 0x40) == 0) {
        Game_Helper46d8c1(a, b, 2);
    }
    if (kind == 0x6c
        && b == g_game951bdc
        && a == g_game8e1aa8
        && g_game8bd200 != 1) {
        Game_Helper56ca10(0x58a2e4, 0x58a2d4);
    }
    Game_Helper4d34d6(a, b, kind, 0x40);
    return 0;
}

// FUNCTION: SHANDALAR 0x004d5020
int Game_CardScript4d5020(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0xc7
        && Game_Helper411d90(a, b) != 0
        && (signed char)g_gameSlots[a][b].field50 != -1) {
        if ((signed char)g_gameSlots[a][b].field50 == g_game8c776c) {
            if (0x18 - g_game8c83f0[(signed char)g_gameSlots[a][b].field50] <= 1) {
            }
            g_game94d63c += (1 + 2) * 8;
        } else {
            if (0x18 - g_game8c83f0[(signed char)g_gameSlots[a][b].field50] <= 1) {
            }
            g_game94d63c -= -(0 - (1 + 2) * 8);
        }
    }
    if (kind == 0x74) {
        Game_Helper4be650(0, 0, a, 2, 2, 0x200, 4, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c
            && g_game951bdc == b
            && a == g_game8e1aa8) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58a2fc, 0x58a2f0);
            }
            if (Game_Helper4c0efa(a, 2, 1 - a, 0x200, 4, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &v8) != 0) {
                if (g_gameSlots[a][b].field74 == g_game8c776c) {
                    g_game94d63c += 0x30;
                }
                if (g_gameSlots[a][b].field74 == g_game8c7468) {
                    g_game94d63c -= 0x60;
                }
                g_gameSlots[a][b].field74 = v8;
                g_gameSlots[a][b].field78 = v4;
                g_gameSlots[a][b].field36 = 1;
                return 0;
            }
            g_game8c83ec = 1;
            return 0;
        }
        if (kind == 0x71) {
            if (Game_Helper4bea85(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78, 0, a, 2, 2, 0x200, 4, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                g_gameSlots[a][b].field50 = g_gameSlots[a][b].field74;
                g_gameSlots[a][b].field04 = g_gameSlots[a][b].field78;
            } else {
                Game_Helper46d8c1(a, b, 1);
                g_game8c83ec = 1;
            }
            g_gameSlots[a][b].field36 = 0;
            return 0;
        }
        if (kind == 0x73) {
            if (g_game94d464 == 4
                && g_game77c57c == g_game7a82e0
                && (signed char)g_gameSlots[a][b].field50 == g_game7a82e0
                && (g_gameSlots[a][b].field38 & 0x1) == 0) {
                g_gameSlots[a][b].field5c |= 0x101;
                g_game8c7420 |= 3;
                return 1;
            }
            return 0;
        }
        if (kind == 4
            && g_game951bdc == b
            && a == g_game8e1aa8) {
            g_gameSlots[a][b].field38 |= 1;
            g_game7be9ac = 1;
            g_game7a01e0 |= 1;
            return 0;
        }
        if (kind == 0x86) {
            Game_Helper4bab38((signed char)g_gameSlots[a][b].field50, 1, g_game94f280, g_game930f5c);
            return 0;
        }
        if (kind == 0x22) {
            g_gameSlots[a][b].field38 &= ~1;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004da8d7
int Game_CardScript4da8d7(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;
    int ii;

    if (kind == 0x7f
        && (g_gameCards[g_gameSlots[g_game8e1aa8][g_game951bdc].field6c].flags & 0x1) != 0) {
        if ((g_gameSlots[g_game8e1aa8][g_game951bdc].flags08 & 0x10) == 0) {
            ih = (signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field58;
            for (ig = 0, iv = 0; ig < 7; ig++) {
                if ((ih & 0x13) != 0) {
                    iv++;
                }
            }
            if (iv > 0) {
                Game_Helper4412ec(g_game8e1aa8, ih, 1);
                return 0;
            }
            Game_Helper441539(g_game8e1aa8, Game_Helper40dffd(ih), 1);
        }
        return 0;
    }
    if (kind == 0x81
        && Game_Helper411d90(a, b) != 0) {
        if (g_gameSlots[g_game8e1aa8][g_game951bdc].field6c != -1) {
            ii = g_gameSlots[g_game8e1aa8][g_game951bdc].field6c;
        } else {
            ii = g_gameSlots[g_game8e1aa8][g_game951bdc].field3c;
        }
        if ((g_gameCards[ii].flags & 0x1) != 0) {
            if (g_game93a794 != -1) {
                Game_Helper4415c5(g_game8e1aa8, g_game93a794, 1);
            }
            return 0;
        }
    }
    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_game94d63c += 0x30;
    }
    return 0;
}


/* Functions below were reconstructed mechanically from disassembly; names are
 * addresses because nothing here establishes meaning. */

// FUNCTION: SHANDALAR 0x004d7623
int Game_CardScript4d7623(int a, int b, int kind)
{
    int v4;

    v4 = 1 - a;
    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        if (g_game8c7468 == a) {
            if (g_game8e3ed8[0] == 0) {
                g_game94d63c -= 0xf0;
                return 0;
            }
            g_game94d63c += (Game_Helper441699(g_game8c7468, 7, 1) / 2 + (g_game8e3ed8[0] - g_game8e3edc)) * 24;
        }
        return 0;
    }
    if (kind == 0x85
        && (g_gameCards[g_gameSlots[g_game8e1aa8][g_game951bdc].field6c].flags & 0x40) != 0
        && g_game77c57c == g_game8e1aa8
        && g_game77c57c == g_game7a82e0) {
        g_gameSlots[g_game8e1aa8][g_game951bdc].field5c |= 3;
        g_gameSlots[g_game8e1aa8][g_game951bdc].field124 = (signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field124 + 2;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004dadbe
int Game_CardScript4dadbe(int a, int b, int kind)
{
    if (kind == 0x74) {
        return 1;
    }
    if ((kind == 0x32 || kind == 0x33)
        && Game_Helper411d90(a, b) != 0
        && Game_Helper411d90(g_game8e1aa8, g_game951bdc) != 0) {
        if ((1 << Game_Helper4ba392(a, b, 2) & 0x15) != 0) {
            g_game7a01e0++;
        }
        return 0;
    }
    if (kind == 0x85
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && g_game7a82e0 == a
        && g_game77c57c == a) {
        g_gameSlots[a][b].field5c |= 1;
        g_gameSlots[a][b].field126 = (signed char)g_gameSlots[a][b].field126 + 2;
        return 0;
    }
    if (kind == 0x86) {
        Game_Helper46d8c1(g_game94f280, g_game930f5c, 1);
        return 0;
    }
    if (kind == 0xc7
        && g_game77ed80[a][2] < 2) {
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004d09fa
int Game_CardScript4d09fa(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        return 1;
    } else {
        if (kind == 0x6c && g_game951bdc == b && g_game8e1aa8 == a) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58a204, 0x58a1fc);
            }
            if (Game_Helper4c0efa(a, 2, 1 - a, 0x1000, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &v8) != 0) {
                g_gameSlots[a][b].field38 = v8;
                g_gameSlots[a][b].field74 = v8;
                g_gameSlots[a][b].field78 = v4;
                g_gameSlots[a][b].field36 = 1;
                if (Game_Helper44aa1f(a, g_gameSlots[a][b].field6c, -1) == 0) {
                    g_game94d63c += 0x30;
                }
            } else {
                g_game8c83ec = 1;
            }
        }
        if (kind == 0x6c && (g_game951bdc != b || g_game8e1aa8 != a)) {
            if (Game_Helper411d90(a, b) != 0) {
                if (g_gameSlots[a][b].field38 == g_game8e1aa8 && (g_gameCards[g_gameSlots[g_game8e1aa8][g_game951bdc].field6c].flags & 0x43) != 0) {
                    g_gameSlots[g_game8e1aa8][g_game951bdc].flags08 |= 0x10;
                }
            }
        }
        return 0;
    }
}

// FUNCTION: SHANDALAR 0x004d5741
int Game_CardScript4d5741(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        return Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c && g_game951bdc == b && g_game8e1aa8 == a) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58a314, 0x58a308);
            }
            if (Game_Helper4c0efa(a, 2, 1 - a, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &v8) != 0) {
                g_gameSlots[a][b].field74 = v8;
                g_gameSlots[a][b].field78 = v4;
                g_gameSlots[a][b].field36 = 1;
                if (v8 != a) {
                    g_game94d63c += Game_Helper40d7b2(v8, v4, 0x32, -1) * 12 / 2;
                } else {
                    g_game94d63c -= -(0 - Game_Helper40d7b2(v8, v4, 0x32, -1) * 12 / 2);
                }
            } else {
                g_game8c83ec = 1;
            }
            return 0;
        } else {
            if (kind == 0x71) {
                if (Game_Helper4bea85(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                    g_gameSlots[a][b].field50 = g_gameSlots[a][b].field74;
                    g_gameSlots[a][b].field04 = g_gameSlots[a][b].field78;
                } else {
                    Game_Helper46d8c1(a, b, 1);
                    g_game8c83ec = 1;
                }
                g_gameSlots[a][b].field36 = 0;
                return 0;
            } else {
                if (g_game94d178 == 0xdc && g_game94d464 == 0x15 && g_game951bdc == b && g_game8e1aa8 == a && g_game7a82e0 == g_game8c8afc && g_gameSlots[a][b].field38 == 0 && (signed char)g_gameSlots[a][b].field50 == g_game7bf4b8 && g_gameSlots[a][b].field04 == g_game8e2fc4) {
                    if (Game_Helper441699(g_game7a82e0, 7, 3) != 0) {
                        if (kind == 0x7d) {
                            g_game7a01e0 |= 2;
                        }
                        if (kind == 0x7e) {
                            Game_Helper40f77c(a, b, 0x7e, a, 0);
                            Game_Helper4143e0(g_game7a82e0, 0, 3);
                            Game_Helper41040d();
                            if (g_game8c83ec == 1) {
                                g_game7a01dc = 1;
                                g_game8c83ec = 0;
                            } else {
                                g_gameSlots[a][b].field38 = 1;
                            }
                        }
                    } else {
                        g_game7a01dc = 1;
                    }
                    if (g_game7a01dc != 0) {
                        g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags08 &= ~0x8000;
                    }
                }
                if (kind == 0x79 && g_gameSlots[a][b].field38 == 0) {
                    if (Game_Helper441699(g_game7a82e0, 7, 3) == 0) {
                        g_game7a01e0 = 1;
                    }
                    return 0;
                } else {
                    if (kind == 0x22 || kind == 0xc7) {
                        g_gameSlots[a][b].field38 = 0;
                    }
                    return 0;
                }
            }
        }
    }
}

// FUNCTION: SHANDALAR 0x004de7f8
int Game_CardScript4de7f8(int a, int b, int kind)
{
    int v4;

    if (kind == 1) {
        g_game94d660[a][1]++;
    }
    if (kind == 0x74) {
        return Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c && b == g_game951bdc && a == g_game8e1aa8) {
            g_gameSlots[a][b].field4c = 0;
            g_gameSlots[a][b].field38 = g_gameSlots[a][b].field4c;
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58a534, 0x58a528);
            }
            if (Game_Helper49f87b(a, a, b) != 0) {
                if (a == g_game8c7468 && (g_game93a934 & 0x2) == 0) {
                    if (Game_Helper4e25e6(a, b, g_gameSlots[a][b].field6c) != 0) {
                        g_game94d63c -= 0x60;
                    }
                    if (g_gameSlots[a][b].field74 == a) {
                        g_game94d63c += 0xc;
                    } else {
                        g_game94d63c -= 0xc;
                    }
                }
            } else {
                g_game8c83ec = 1;
            }
            return 0;
        } else {
            if (kind == 0x71) {
                if (Game_Helper4bea85(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                    g_gameSlots[a][b].field50 = g_gameSlots[a][b].field74;
                    g_gameSlots[a][b].field04 = g_gameSlots[a][b].field78;
                } else {
                    Game_Helper46d8c1(a, b, 1);
                    g_game8c83ec = 1;
                }
                g_gameSlots[a][b].field36 = 0;
                return 0;
            } else {
                if (kind == 0x73) {
                    return Game_Helper4419ca(a, b, 5, 1);
                } else {
                    if (kind == 0x90) {
                        if (a == g_game7a82e0) {
                            if (g_game765420[Game_Helper40dffd((signed char)g_gameSlots[a][b].field1e)] != 0) {
                                g_game74b868 = 1;
                            } else {
                                Game_Helper4c42ae(0);
                            }
                        } else {
                            g_game74b868 = 1;
                        }
                        g_game939ce8 = (signed char)g_gameSlots[a][b].field50 * 256 | g_gameSlots[a][b].field04;
                        return 0;
                    } else {
                        if (kind == 0x6d) {
                            if (Game_Helper4419ca(a, b, 5, 1) != 0) {
                                if (a == g_game7a82e0) {
                                    if (g_game765420[Game_Helper40dffd((signed char)g_gameSlots[a][b].field1e)] != 0) {
                                        g_game8c8274 = Game_Helper417301(a, b, 5, 1);
                                    } else {
                                        Game_Helper4143e0(a, 5, -1);
                                    }
                                    if (g_game8c8274 > 0) {
                                        g_gameSlots[a][b].field4c = g_game8c8274;
                                    } else {
                                        g_game8c83ec = 1;
                                    }
                                } else {
                                    if (g_game765420[Game_Helper40dffd((signed char)g_gameSlots[a][b].field1e)] != 0) {
                                        Game_Helper417301(a, b, 5, 1);
                                    } else {
                                        Game_Helper4143e0(a, 5, 1);
                                    }
                                    g_gameSlots[a][b].field4c = 1;
                                }
                                if (g_game8c83ec != 1) {
                                    g_gameSlots[a][b].field74 = (signed char)g_gameSlots[a][b].field50;
                                    g_gameSlots[a][b].field78 = g_gameSlots[a][b].field04;
                                    g_gameSlots[a][b].field36 = 1;
                                    if (g_gameSlots[a][b].field38 == 0) {
                                        g_gameSlots[a][b].field38 |= 0x80000;
                                    }
                                } else {
                                    g_gameSlots[a][b].field4c = 0;
                                }
                            }
                            return 0;
                        } else {
                            if (kind == 0x72) {
                                if (g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field6c != -1) {
                                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 += g_gameSlots[a][b].field4c & 0xff;
                                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 += (g_gameSlots[a][b].field4c & 0xff) * 256;
                                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
                                    if ((g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 & 0x80000) != 0) {
                                        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 &= ~0x80000;
                                        v4 = Game_Helper4a9910(g_game94f280, g_game930f5c, g_game8bcff8, (signed char)g_gameSlots[a][b].field50, g_gameSlots[a][b].field04);
                                        if (v4 != -1) {
                                            g_gameSlots[a][v4].field12 = g_gameSlots[a][b].field4c;
                                            g_gameSlots[a][v4].field1c = g_gameSlots[a][b].field4c;
                                            g_gameSlots[a][v4].field38 |= 0x80000;
                                        }
                                    }
                                } else {
                                    g_game8c83ec = 1;
                                }
                                return 0;
                            } else {
                                if (kind == 0xc7) {
                                    if (a == g_game8c7468) {
                                        g_game94d63c += (g_game77ed80[a][5] + g_game77ed80[a][5] * 2 + 6) * 4;
                                    } else {
                                        g_game94d63c -= -(0 - (g_game77ed80[a][5] + g_game77ed80[a][5] * 2 + 6) * 4);
                                    }
                                }
                                if (kind == 0x22 || kind == 0xc7) {
                                    g_gameSlots[a][b].field4c = 0;
                                    g_gameSlots[a][b].field38 = g_gameSlots[a][b].field4c;
                                }
                                return 0;
                            }
                        }
                    }
                }
            }
        }
    }
}

// FUNCTION: SHANDALAR 0x004df65e
int Game_CardScript4df65e(int a, int b, int kind)
{
    int v4;

    if (kind == 1) {
        g_game94d660[a][0]++;
    }
    if (kind == 0x74) {
        return Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c && g_game951bdc == b && g_game8e1aa8 == a) {
            g_gameSlots[a][b].field4c = 0;
            g_gameSlots[a][b].field38 = g_gameSlots[a][b].field4c;
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58a550, 0x58a540);
            }
            if (Game_Helper49f87b(a, a, b) != 0) {
                if (a == g_game8c7468 && (g_game93a934 & 0x2) == 0) {
                    if (Game_Helper4e25e6(a, b, g_gameSlots[a][b].field6c) != 0) {
                        g_game94d63c -= 0x60;
                    }
                    if (g_gameSlots[a][b].field74 == a) {
                        g_game94d63c += 0xc;
                    } else {
                        g_game94d63c -= 0xc;
                    }
                }
            } else {
                g_game8c83ec = 1;
            }
            return 0;
        } else {
            if (kind == 0x71) {
                if (Game_Helper4bea85(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                    g_gameSlots[a][b].field50 = g_gameSlots[a][b].field74;
                    g_gameSlots[a][b].field04 = g_gameSlots[a][b].field78;
                } else {
                    Game_Helper46d8c1(a, b, 1);
                    g_game8c83ec = 1;
                }
                g_gameSlots[a][b].field36 = 0;
                return 0;
            } else {
                if (kind == 0x73) {
                    return Game_Helper4419ca(a, b, 4, 1);
                } else {
                    if (kind == 0x90) {
                        if (a == g_game7a82e0) {
                            if (g_game765420[Game_Helper40dffd((signed char)g_gameSlots[a][b].field1e)] != 0) {
                                g_game74b868 = 1;
                            } else {
                                Game_Helper4c42ae(0);
                            }
                        } else {
                            g_game74b868 = 1;
                        }
                        g_game939ce8 = (signed char)g_gameSlots[a][b].field50 * 256 | g_gameSlots[a][b].field04;
                        return 0;
                    } else {
                        if (kind == 0x6d) {
                            if (Game_Helper4419ca(a, b, 4, 1) != 0) {
                                if (a == g_game7a82e0) {
                                    if (g_game765420[Game_Helper40dffd((signed char)g_gameSlots[a][b].field1e)] != 0) {
                                        g_game8c8274 = Game_Helper417301(a, b, 4, 1);
                                    } else {
                                        Game_Helper4143e0(a, 4, -1);
                                    }
                                    if (g_game8c8274 > 0) {
                                        g_gameSlots[a][b].field4c = g_game8c8274;
                                    } else {
                                        g_game8c83ec = 1;
                                    }
                                } else {
                                    if (g_game765420[Game_Helper40dffd((signed char)g_gameSlots[a][b].field1e)] != 0) {
                                        Game_Helper417301(a, b, 4, 1);
                                    } else {
                                        Game_Helper4143e0(a, 4, 1);
                                    }
                                    g_gameSlots[a][b].field4c = 1;
                                }
                                if (g_game8c83ec != 1) {
                                    g_gameSlots[a][b].field74 = (signed char)g_gameSlots[a][b].field50;
                                    g_gameSlots[a][b].field78 = g_gameSlots[a][b].field04;
                                    g_gameSlots[a][b].field36 = 1;
                                    if (g_gameSlots[a][b].field38 == 0) {
                                        g_gameSlots[a][b].field38 |= 0x80000;
                                    }
                                } else {
                                    g_gameSlots[a][b].field4c = 0;
                                }
                            }
                            return 0;
                        } else {
                            if (kind == 0x72) {
                                if (g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field6c != -1) {
                                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 += g_gameSlots[a][b].field4c & 0xff;
                                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
                                    if ((g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 & 0x80000) != 0) {
                                        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 &= ~0x80000;
                                        v4 = Game_Helper4a9910(g_game94f280, g_game930f5c, g_game8bcff8, (signed char)g_gameSlots[a][b].field50, g_gameSlots[a][b].field04);
                                        if (v4 != -1) {
                                            g_gameSlots[a][v4].field12 = g_gameSlots[a][b].field4c;
                                            g_gameSlots[a][v4].field38 |= 0x80000;
                                        }
                                    }
                                } else {
                                    g_game8c83ec = 1;
                                }
                                return 0;
                            } else {
                                if (kind == 0xc7) {
                                    if (a == g_game8c7468) {
                                        g_game94d63c += (g_game77ed80[a][4] + g_game77ed80[a][4] * 2 + 3) * 4;
                                    } else {
                                        g_game94d63c -= -(0 - (g_game77ed80[a][4] + g_game77ed80[a][4] * 2 + 3) * 4);
                                    }
                                }
                                if (kind == 0x22 || kind == 0xc7) {
                                    g_gameSlots[a][b].field4c = 0;
                                    g_gameSlots[a][b].field38 = g_gameSlots[a][b].field4c;
                                }
                                return 0;
                            }
                        }
                    }
                }
            }
        }
    }
}
