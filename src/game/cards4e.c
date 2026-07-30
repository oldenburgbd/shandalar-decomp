/* Card behaviour scripts in the 0x4Exxxx range. Split into small translation
 * units on purpose: MSVC re-rolls codegen across a whole TU when anything in
 * it changes, so keeping units small confines the blast radius. Shared
 * CUR/SRC macros live in game/gameslot.h. */

#include "game/gameslot.h"

// FUNCTION: SHANDALAR 0x004e03af
int Game_CardScript4e03af(int a, int b, int kind)
{
    if (kind == 0x78
        && g_gameSlots[a][b].field04 == g_game8e26b0
        && (signed char)g_gameSlots[a][b].field50 == g_game94e928
        && (g_gameSlots[a][b].flags08 & 0x20) == 0) {
        if ((g_gameCards[g_gameSlots[g_game8e1aa8][g_game951bdc].field6c].flags & 0x40) == 0
            && ((1 << Game_Helper4ba392(a, b, 1)) & 0x15) == 0) {
            g_game7a01e0++;
        }
        return 0;
    }
    if (kind == 0x74) {
        Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58a564, 0x58a55c);
            }
            if (Game_Helper49f87b(a, a, b) == 0) {
                g_game8c83ec = 1;
                return 0;
            }
            g_game8c83ec = 0;
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
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004e07ad
int Game_CardScript4e07ad(int a, int b, int kind)
{
    if (kind == 0x78
        && g_gameSlots[a][b].field04 == g_game8e26b0
        && (signed char)g_gameSlots[a][b].field50 == g_game94e928
        && (g_gameSlots[a][b].flags08 & 0x20) == 0) {
        if ((g_gameCards[g_gameSlots[g_game8e1aa8][g_game951bdc].field6c].flags & 0x40) == 0
            && ((1 << Game_Helper4ba392(a, b, 5)) & 0x15) == 0) {
            g_game7a01e0++;
        }
        return 0;
    }
    if (kind == 0x74) {
        Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58a578, 0x58a570);
            }
            if (Game_Helper49f87b(a, a, b) == 0) {
                g_game8c83ec = 1;
                return 0;
            }
            g_game8c83ec = 0;
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
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004e5a30
int Game_CardScript4e5a30(int a, int b, int kind)
{
    if (kind == 0x33
        && Game_Helper411d90(a, b) != 0) {
        if (a == g_game8e1aa8
            && (g_gameSlots[g_game8e1aa8][g_game951bdc].flags08 & 0x14) == 0
            && Game_Helper411d90(g_game8e1aa8, g_game951bdc) != 0) {
            g_game7a01e0 += 2;
        }
        return 0;
    }
    if (kind == 0x74) {
        return 1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004e6541
int Game_CardScript4e6541(int a, int b, int kind)
{
    if (kind == 0x74) {
        Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58a708, 0x58a6f4);
            }
            if (Game_Helper49f87b(a, a, b) == 0) {
                g_game8c83ec = 1;
            } else {
                g_game8c83ec = 0;
            }
            if (g_game8c83ec != 1
                && g_game8c7468 == a
                && (g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].flags08 & 0x30000) != 0) {
                g_game94d63c -= 0x63;
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
        if ((kind == 0x32 || kind == 0x33)
            && Game_Helper411d90(a, b) != 0) {
            if (g_gameSlots[a][b].field04 == g_game951bdc
                && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8
                && g_game951bdc != -1) {
                g_game7a01e0 += 3;
            }
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
            g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field12--;
            g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field1c--;
            g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field00 += 0x10000;
            if (g_game8bd200 != 1) {
                Game_Helper40ebb9(0x25);
            }
            return 0;
        }
        if (kind == 0x22) {
            g_gameSlots[a][b].field38 &= ~1;
        }
        if (kind == 0xc7) {
            g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field12 = g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field12 - 2;
            g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field1c = g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field1c - 2;
            g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field28 |= 0x6000000;
            if (g_game8c7468 == a
                && g_game8c7468 == g_game7a82e0
                && (g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags08 & 0x40) == 0) {
                g_game94d63c -= 0x3c;
            }
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004e9531
int Game_CardScript4e9531(int a, int b, int kind)
{
    if (g_gameSlots[a][b].field04 == g_game951bdc
        && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8
        && g_game951bdc != -1
        && (g_gameSlots[a][b].flags18 & 0x80) == 0
        && (g_gameSlots[a][b].flags08 & 0x20) == 0) {
        g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags08 |= 0x2000;
    }
    if (kind == 0x74) {
        Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58a794, 0x58a784);
            }
            if (Game_Helper49f87b(a, a, b) == 0) {
                g_game8c83ec = 1;
            } else {
                g_game8c83ec = 0;
            }
            if (g_game8c83ec != 1
                && g_game8c7468 == a
                && (g_game93a934 & 0x2) == 0) {
                if (Game_Helper40cd94(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78) != 0 || g_gameSlots[a][b].field74 == g_game8c776c) {
                    g_game94d63c -= 0x30;
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
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags08 &= ~0x2000;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004e17ca
int Game_CardScript4e17ca(int a, int b, int kind)
{
    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_game94d63c += g_game8e3300[1 - a][6] - g_game8e3300[a][6];
        return 0;
    }
    if (kind == 0x82
        && (1 << Game_Helper4ba392(a, b, 2) & 0x15) != 0
        && (g_gameCards[g_gameSlots[g_game8e1aa8][g_game951bdc].field6c].flags & 0x2) != 0) {
        g_gameSlots[g_game8e1aa8][g_game951bdc].field11c &= ~3;
        return 0;
    }
    if (kind == 0x84
        && (g_gameSlots[g_game8e1aa8][g_game951bdc].flags08 & 0x10) != 0
        && g_game8e1aa8 == g_game7a82e0
        && g_game77c57c == g_game7a82e0) {
        if ((g_gameCards[g_gameSlots[g_game8e1aa8][g_game951bdc].field6c].flags & 0x2) != 0
            && (1 << Game_Helper4ba392(a, b, 2) & 0x15) != 0) {
            g_gameSlots[g_game8e1aa8][g_game951bdc].field5c |= 0x10;
            g_gameSlots[g_game8e1aa8][g_game951bdc].field2c = (signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field2c + 4;
        }
        return 0;
    }
    if (kind == 0x6c) {
        if ((1 << Game_Helper4ba392(a, b, 2) & 0x15) != 0
            && (g_gameCards[g_gameSlots[g_game8e1aa8][g_game951bdc].field6c].flags & 0x2) != 0) {
            g_gameSlots[g_game8e1aa8][g_game951bdc].field2c = (signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field2c + 4;
        }
        return 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004e31e9
int Game_CardScript4e31e9(int a, int b, int kind)
{
    if (kind == 0x74) {
        Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58a5f8, 0x58a5ec);
            }
            if (Game_Helper49f87b(a, 1 - a, b) == 0) {
                g_game8c83ec = 1;
            } else {
                g_game8c83ec = 0;
            }
            if (g_gameSlots[a][b].field74 == g_game8c776c) {
                g_game94d63c += 0x30;
            }
            if (g_gameSlots[a][b].field74 == g_game8c7468) {
                g_game94d63c -= 0x60;
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

// FUNCTION: SHANDALAR 0x004e49ba
int Game_CardScript4e49ba(int a, int b, int kind)
{
    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x73) {
        if (g_game8c7468 == a
            && (g_game93a934 & 0x2) == 0
            && g_game8c83f0[a] == 2) {
            return 0;
        }
        if (Game_Helper4419ca(a, b, 1, 1) != 0
            && g_game8c83f0[a] >= 2) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x6d) {
        Game_Helper417301(a, b, 1, 1);
        g_game94d63c -= -(0 - (g_game8e3ec8[a] + g_game8e3ec8[a] - 4 - g_game8c83f0[a] + (g_game8e3ec8[a] + g_game8e3ec8[a] - 4 - g_game8c83f0[a]) * 2 + 0x1e) * 4);
        return 0;
    }
    if (kind == 0x72) {
        Game_Helper409a0b(a);
        g_game8c83f0[a] -= 2;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004e9ad4
int Game_CardScript4e9ad4(int a, int b, int kind)
{
    int v4;

    if (kind == 0x32
        && g_gameSlots[a][b].field04 == g_game951bdc
        && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8
        && g_game951bdc != -1
        && Game_Helper411d90(a, b) != 0) {
        g_game7a01e0++;
    }
    if (kind == 1) {
        g_game94d660[a][0] += 2;
        return 0;
    }
    if (kind == 0x74) {
        Game_Helper4c4263(0);
        Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58a7ac, 0x58a7a0);
            }
            if (Game_Helper49f87b(a, a, b) == 0) {
                g_game8c83ec = 1;
            } else {
                g_game8c83ec = 0;
            }
            if (g_game8c83ec != 1
                && g_game8c7468 == a
                && (g_game93a934 & 0x2) == 0
                && Game_Helper4e25e6(a, b, g_gameSlots[a][b].field6c) != 0) {
                g_game94d63c -= 0xc;
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
        if (kind == 0x73
            && (g_game8c83f9 & 0x2) != 0
            && g_gameSlots[a][b].field38 == 0) {
            v4 = 1;
            if ((g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags08 & 0x800002) != 2) {
                v4 = 0;
            }
            if ((signed char)g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field68 != 2) {
                v4 = 0;
            }
            if (Game_Helper4419ca(a, b, 4, 3) == 0) {
                v4 = 0;
            }
            if (v4 != 0) {
                return 0x63;
            }
            return 0;
        }
        if (kind == 0x90) {
            Game_Helper4c4263(0);
            return 0;
        }
        if (kind == 0x6d
            && (g_game8c83f9 & 0x2) != 0) {
            Game_Helper417301(a, b, 4, 3);
            if (g_game8c83ec != 1) {
                g_game7be9ac = 1;
                g_gameSlots[a][b].field38++;
            }
            return 0;
        }
        if (kind == 0x72
            && (g_game8c83f9 & 0x2) != 0) {
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 = 0;
            Game_Helper490c6d((signed char)g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field50, g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field04);
            return 0;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004eb68f
int Game_CardScript4eb68f(int a, int b, int kind)
{
    if (kind == 0x74) {
        Game_Helper4be650(0, 0, a, 2, 2, 0x200, 1, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c
            && b == g_game951bdc
            && a == g_game8e1aa8) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58a84c, 0x58a840);
            }
            if (Game_Helper49fda3(a, a, b) == 0 || g_gameSlots[a][b].field74 == g_game8c7468) {
                g_game94d63c += g_game8e3300[g_game8c7468][7] * 12;
                return 0;
                g_game8c83ec = 1;
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
        if (kind == 0x81
            && g_gameSlots[a][b].field04 == g_game951bdc
            && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8
            && g_game951bdc != -1
            && g_game93a794 != -1) {
            Game_Helper4415c5((signed char)g_gameSlots[a][b].field50, 3, 1);
            return 0;
        }
        if (kind == 0x7f
            && g_gameSlots[a][b].field04 == g_game951bdc
            && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8
            && g_game951bdc != -1
            && (g_gameSlots[g_game8e1aa8][g_game951bdc].flags08 & 0x10) == 0) {
            Game_Helper441539(g_game8e1aa8, 3, 1);
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004ebfa3
int Game_CardScript4ebfa3(int a, int b, int kind)
{
    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_game94d63c += g_game77ed80[1 - a][1] * 5 + 0x18;
        return 0;
    }
    if (kind == 0x73) {
        if (g_game8e2688 != -1) {
            if ((g_game8c83f8 & 0x20) != 0
                && Game_Helper4419ca(a, b, 3, 2) != 0
                && Game_Helper4bea85(g_game8e2688, g_game8e2674, 0, a, 2, 2, 0, 0, 0, 0, 0, 1 << Game_Helper4ba392(a, b, 1), 0, -1, -1, -1, -1, 2, 0, 0) != 0) {
                return 0x63;
            }
            return 0;
        }
        return 0;
    }
    if (kind == 0x6d
        && Game_Helper4419ca(a, b, 3, 2) != 0
        && g_game8e2688 != -1) {
        Game_Helper417301(a, b, 3, 2);
        if (g_game8c83ec != 1) {
            g_gameSlots[a][b].field74 = g_game8e2688;
            g_gameSlots[a][b].field78 = g_game8e2674;
        }
        return 0;
    }
    if (kind == 0x72) {
        if (Game_Helper4bea85(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78, 0, a, 2, 2, 0, 0, 0, 0, 0, 1 << Game_Helper4ba392(a, b, 1), 0, -1, -1, -1, -1, 2, 0, 0) != 0) {
            Game_Helper46d8c1(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78, 1);
            return 0;
        }
        g_game8c83ec = 1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004ec2be
int Game_CardScript4ec2be(int a, int b, int kind)
{
    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x6c
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        g_game94d63c += g_game77ed80[1 - a][3] * 5 + 0x18;
        return 0;
    }
    if (kind == 0x73) {
        if (g_game8e2688 != -1) {
            if ((g_game8c83f8 & 0x20) != 0
                && Game_Helper4419ca(a, b, 1, 2) != 0
                && Game_Helper4bea85(g_game8e2688, g_game8e2674, 0, a, 2, 2, 0, 0, 0, 0, 0, 1 << Game_Helper4ba392(a, b, 3), 0, -1, -1, -1, -1, 2, 0, 0) != 0) {
                return 0x63;
            }
            return 0;
        }
        return 0;
    }
    if (kind == 0x6d
        && Game_Helper4419ca(a, b, 1, 2) != 0
        && g_game8e2688 != -1) {
        Game_Helper417301(a, b, 1, 2);
        if (g_game8c83ec != 1) {
            g_gameSlots[a][b].field74 = g_game8e2688;
            g_gameSlots[a][b].field78 = g_game8e2674;
        }
        return 0;
    }
    if (kind == 0x72) {
        if (Game_Helper4bea85(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78, 0, a, 2, 2, 0, 0, 0, 0, 0, 1 << Game_Helper4ba392(a, b, 3), 0, -1, -1, -1, -1, 2, 0, 0) != 0) {
            Game_Helper46d8c1(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78, 1);
            return 0;
        }
        g_game8c83ec = 1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004e0bab
int Game_CardScript4e0bab(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58a588, 0x58a584);
            }
            if (Game_Helper49f87b(a, a, b) == 0) {
                g_game8c83ec = 1;
            } else {
                g_game8c83ec = 0;
            }
            if (g_game8c83ec != 1
                && g_game8c7468 == a
                && (g_game93a934 & 0x2) == 0) {
                for (v8 = 0, v4 = -1; g_game78e970[1 - a] > v8; v8++) {
                    if (Game_Helper411d90(1 - a, v8) != 0
                        && g_gameSlots[1 - a][v8].field34 > v4) {
                        v4 = g_gameSlots[1 - a][v8].field34;
                    }
                }
                if (g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].field0e <= v4
                    && g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].field0e + 2 > v4) {
                    g_game94d63c += v4 * 3 + 0x18;
                }
                if ((g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].field28 & 0x420) != 0) {
                    g_game94d63c -= 0x18;
                    return 0;
                }
                g_game94d63c += g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].field0e;
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
        if (kind == 0x33
            && g_gameSlots[a][b].field04 == g_game951bdc
            && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8
            && g_game951bdc != -1
            && Game_Helper411d90(a, b) != 0) {
            g_game7a01e0 += 2;
        }
        if (kind == 0x34
            && g_gameSlots[a][b].field04 == g_game951bdc
            && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8
            && g_game951bdc != -1
            && Game_Helper411d90(a, b) != 0) {
            g_game7a01e0 |= 0x400;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004e2723
int Game_CardScript4e2723(int a, int b, int kind)
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
        if (Game_Helper44aa1f(a, g_gameSlots[a][b].field6c, -1) == 0) {
            g_game94d63c += (g_game8e3320[g_game8c776c][3] - g_game8e3320[g_game8c7468][3]) * 12;
        }
        return 0;
    }
    if (kind == 0x82
        && (g_gameCards[g_gameSlots[g_game8e1aa8][g_game951bdc].field6c].flags & 0x2) != 0) {
        g_gameSlots[g_game8e1aa8][g_game951bdc].field11c &= ~2;
        g_game939e64 |= 2;
        return 0;
    }
    if (g_game94d464 == 1
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        if (kind == 0x7d) {
            if (Game_Helper4be650(0, 0, g_game7a82e0, g_game7a82e0, g_game7a82e0, 0x200, 2, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0x800, 0) != 0) {
            } else {
                if (Game_Helper4be650(0, 0, g_game7a82e0, g_game7a82e0, g_game7a82e0, 0x200, 2, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0x400, 0) != 0) {
                    g_game7a01e0 |= 2;
                }
            }
        }
        if (kind == 0x7e) {
            if (g_game8c7468 == g_game7a82e0
                && (g_game93a934 & 0x2) == 0) {
                ih = g_game7a82e0;
                ig = Game_Helper4ec5d7(g_game7a82e0, 2);
                Game_Helper56ca10(0x58a5b4, 0x58a5ac);
                Game_Helper558538(a, a, b, ih, ig, 0x8aa920 + 0x12c, 0);
            } else {
                Game_Helper56ca10(0x58a5c8, 0x58a5c0);
                Game_Helper4c0efa(g_game7a82e0, g_game7a82e0, g_game7a82e0, 0x200, 2, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0x401, 0, 0x8aa920, 0, &ih);
            }
            g_gameSlots[ih][ig].field11c |= 2;
            for (iv = 0; g_game78e970[g_game7a82e0] > iv; iv++) {
                if (Game_Helper411d90(g_game7a82e0, iv) != 0
                    && (g_gameSlots[g_game7a82e0][iv].flags08 & 0x10) != 0
                    && (g_gameCards[g_gameSlots[g_game7a82e0][iv].field6c].flags & 0x2) != 0
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

// FUNCTION: SHANDALAR 0x004e2bc8
int Game_CardScript4e2bc8(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x6c
        && g_game951bdc == b
        && a == g_game8e1aa8) {
    }
    if (kind == 0x6a) {
        g_gameSlots[a][b].field4c = 0;
        for (v8 = 0; g_game78e970[g_game7a82e0] > v8; v8++) {
            if (Game_Helper411d90(g_game7a82e0, v8) != 0
                && (g_gameSlots[g_game7a82e0][v8].flags08 & 0x10) == 0
                && (g_gameCards[g_gameSlots[g_game7a82e0][v8].field6c].flags & 0x1) != 0) {
                g_gameSlots[a][b].field4c++;
            }
        }
        return 0;
    }
    if (kind == 0x73) {
        if (g_game94d464 == 4
            && (g_gameSlots[a][b].field38 & 0x1) == 0
            && g_game77c57c == g_game7a82e0) {
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
        Game_Helper4bab38(g_game7a82e0, g_gameSlots[a][b].field4c, g_game94f280, g_game930f5c);
        g_gameSlots[a][b].field4c = 0;
        return 0;
    }
    if (kind == 0x22) {
        g_gameSlots[a][b].field38 &= ~1;
    }
    if (kind == 0xc7) {
        for (v8 = 0, v4 = 0; g_game78e970[1 - g_game7a82e0] > v8; v8++) {
            if (Game_Helper411d90(1 - g_game7a82e0, v8) != 0
                && (g_gameSlots[1 - g_game7a82e0][v8].flags08 & 0x10) == 0
                && (g_gameCards[g_gameSlots[1 - g_game7a82e0][v8].field6c].flags & 0x1) != 0) {
                v4++;
            }
        }
        Game_Helper4bab38(1 - g_game7a82e0, v4, a, b);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004e303a
int Game_CardScript4e303a(int a, int b, int kind)
{
    int v4;

    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && g_game8bd200 != 1) {
        Game_Helper56ca10(0x58a5e0, 0x58a5d4);
    }
    v4 = Game_Helper4e4fa0(a, b, kind, 1 << Game_Helper4ba309(a, b, 4) - 1);
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && g_game8c7468 == a) {
        if ((g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].field28 & 0x17) != 0 || g_gameSlots[a][b].field74 == g_game8c776c) {
            g_game94d63c -= 0x60;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004e45de
int Game_CardScript4e45de(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_game94d63c += 0x30;
        return 0;
    }
    if (kind == 0x73) {
        if (Game_Helper4419ca(a, b, 2, 2) != 0
            && Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b) | 0x20, 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x6d) {
        Game_Helper417301(a, b, 2, 2);
        if (g_game8c83ec != 1) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58a628, 0x58a620);
            }
            if (Game_Helper4c0efa(a, 2, 1 - a, 0x200, 2, 0, 0, Game_Helper488cc4(a, b) | 0x20, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &v8) != 0) {
                g_gameSlots[a][b].field74 = v8;
                g_gameSlots[a][b].field78 = v4;
                g_gameSlots[a][b].field36 = 1;
                return 0;
            }
            g_game8c83ec = 1;
        }
        return 0;
    }
    if (kind == 0x72) {
        if (Game_Helper4bea85(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b) | 0x20, 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
            Game_Helper4b183c(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78);
        } else {
            g_game8c83ec = 1;
        }
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004e52db
int Game_CardScript4e52db(int a, int b, int kind)
{
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && g_game8bd200 != 1) {
        Game_Helper56ca10(0x58a644, 0x58a634);
    }
    Game_Helper4e5571(a, b, kind, 1, 2);
    return 0;
}

// FUNCTION: SHANDALAR 0x004e534a
int Game_CardScript4e534a(int a, int b, int kind)
{
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && g_game8bd200 != 1) {
        Game_Helper56ca10(0x58a660, 0x58a650);
    }
    Game_Helper4e5571(a, b, kind, 2, 2);
    return 0;
}

// FUNCTION: SHANDALAR 0x004e53b9
int Game_CardScript4e53b9(int a, int b, int kind)
{
    if (kind == 0x6c
        && b == g_game951bdc
        && a == g_game8e1aa8
        && g_game8bd200 != 1) {
        Game_Helper56ca10(0x58a678, 0x58a66c);
    }
    Game_Helper4e5571(a, b, kind, 2, 0xfffffffe);
    return 0;
}

// FUNCTION: SHANDALAR 0x004e5426
int Game_CardScript4e5426(int a, int b, int kind)
{
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && g_game8bd200 != 1) {
        Game_Helper56ca10(0x58a69c, 0x58a684);
    }
    Game_Helper4e5571(a, b, kind, 3, 3);
    return 0;
}

// FUNCTION: SHANDALAR 0x004e5495
int Game_CardScript4e5495(int a, int b, int kind)
{
    if (kind == 0x6c
        && b == g_game951bdc
        && a == g_game8e1aa8
        && g_game8bd200 != 1) {
        Game_Helper56ca10(0x58a6b8, 0x58a6a8);
    }
    Game_Helper4e5571(a, b, kind, 2, 1);
    return 0;
}

// FUNCTION: SHANDALAR 0x004e5502
int Game_CardScript4e5502(int a, int b, int kind)
{
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && g_game8bd200 != 1) {
        Game_Helper56ca10(0x58a6d0, 0x58a6c4);
    }
    Game_Helper4e5571(a, b, kind, 0xfffffffe, -1);
    return 0;
}

// FUNCTION: SHANDALAR 0x004e5aea
int Game_CardScript4e5aea(int a, int b, int kind)
{
    Game_Helper4e5bfd(a, b, kind, Game_Helper4ba392(a, b, 1));
    return 0;
}

// FUNCTION: SHANDALAR 0x004e5b21
int Game_CardScript4e5b21(int a, int b, int kind)
{
    Game_Helper4e5bfd(a, b, kind, Game_Helper4ba392(a, b, 3));
    return 0;
}

// FUNCTION: SHANDALAR 0x004e5b58
int Game_CardScript4e5b58(int a, int b, int kind)
{
    Game_Helper4e5bfd(a, b, kind, Game_Helper4ba392(a, b, 2));
    return 0;
}

// FUNCTION: SHANDALAR 0x004e5b8f
int Game_CardScript4e5b8f(int a, int b, int kind)
{
    Game_Helper4e5bfd(a, b, kind, Game_Helper4ba392(a, b, 4));
    return 0;
}

// FUNCTION: SHANDALAR 0x004e5bc6
int Game_CardScript4e5bc6(int a, int b, int kind)
{
    Game_Helper4e5bfd(a, b, kind, Game_Helper4ba392(a, b, 5));
    return 0;
}

// FUNCTION: SHANDALAR 0x004ea2d4
int Game_CardScript4ea2d4(int a, int b, int kind)
{
    Game_Helper4ea3e7(a, b, kind, Game_Helper4ba392(a, b, 1));
    return 0;
}

// FUNCTION: SHANDALAR 0x004ea30b
int Game_CardScript4ea30b(int a, int b, int kind)
{
    Game_Helper4ea3e7(a, b, kind, Game_Helper4ba392(a, b, 5));
    return 0;
}

// FUNCTION: SHANDALAR 0x004ea342
int Game_CardScript4ea342(int a, int b, int kind)
{
    Game_Helper4ea3e7(a, b, kind, Game_Helper4ba392(a, b, 4));
    return 0;
}

// FUNCTION: SHANDALAR 0x004ea379
int Game_CardScript4ea379(int a, int b, int kind)
{
    Game_Helper4ea3e7(a, b, kind, Game_Helper4ba392(a, b, 2));
    return 0;
}

// FUNCTION: SHANDALAR 0x004ea3b0
int Game_CardScript4ea3b0(int a, int b, int kind)
{
    Game_Helper4ea3e7(a, b, kind, Game_Helper4ba392(a, b, 3));
    return 0;
}

// FUNCTION: SHANDALAR 0x004e12ec
int Game_CardScript4e12ec(int a, int b, int kind)
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
        if (Game_Helper44aa1f(a, g_gameSlots[a][b].field6c, -1) == 0) {
            g_game94d63c += g_game8e3320[1 - a][3] - g_game8e3320[a][3];
        }
        g_gameSlots[a][b].field50 = a;
        return 0;
    }
    if (kind == 0x85
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && a == g_game7a82e0
        && g_game77c57c == g_game7a82e0) {
        g_gameSlots[a][b].field5c |= 1;
        g_gameSlots[a][b].field126 = (signed char)g_gameSlots[a][b].field126 + 1;
        return 0;
    }
    if (kind == 0x86) {
        Game_Helper46d8c1(g_game94f280, g_game930f5c, 1);
        return 0;
    }
    if (kind == 0xc7) {
        if (g_game77ed80[a][2] < 1) {
            Game_Helper46d8c1(a, b, 1);
            return 0;
        }
        if (g_game78e970[g_game8c7468] <= g_game78e970[g_game8c776c]) {
        }
        ih = g_game78e970[g_game8c776c];
        for (iv = 0, ig = 0; iv < ih; iv++) {
            if (Game_Helper411d90(g_game8c776c, iv) != 0
                && (g_gameCards[g_gameSlots[g_game8c776c][iv].field6c].flags & 0x2) != 0) {
                if ((g_gameSlots[g_game8c776c][iv].flags08 & 0x10) != 0) {
                    ig += g_gameSlots[g_game8c776c][iv].field34 + g_gameSlots[g_game8c776c][iv].field34;
                } else {
                    if (Game_Helper40cd94(g_game8c776c, iv) == 0) {
                        ig += g_gameSlots[g_game8c776c][iv].field34;
                    }
                }
            }
            if (Game_Helper411d90(g_game8c7468, iv) != 0
                && (g_gameCards[g_gameSlots[g_game8c7468][iv].field6c].flags & 0x2) != 0) {
                if ((g_gameSlots[g_game8c7468][iv].flags08 & 0x10) != 0) {
                    ig -= -(0 - (g_gameSlots[g_game8c7468][iv].field34 + g_gameSlots[g_game8c7468][iv].field34));
                } else {
                    if (Game_Helper40cd94(g_game8c7468, iv) == 0) {
                        ig -= -(0 - g_gameSlots[g_game8c7468][iv].field34);
                    }
                }
            }
        }
        g_game94d63c += ig * 12;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004eb3bf
int Game_CardScript4eb3bf(int a, int b, int kind)
{
    if (kind == 0x74) {
        return 1;
    }
    if ((kind == 0x6c || kind == 0xc7)
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && Game_Helper44aa1f(a, g_gameSlots[a][b].field6c, -1) == 0) {
        g_game94d63c += (g_game8e3320[g_game8c7468][1] - g_game8e3320[g_game8c776c][1]) * 12;
    }
    if (kind == 0x71) {
        g_gameSlots[a][b].field38 = 5;
        return 0;
    }
    if (kind == 0x85
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && g_game7a82e0 == a
        && g_game77c57c == g_game7a82e0) {
        g_gameSlots[a][b].field5c |= 1;
        g_gameSlots[a][b].field129 = (signed char)g_gameSlots[a][b].field129 + 2;
        return 0;
    }
    if (kind == 0x86) {
        Game_Helper46d8c1(g_game94f280, g_game930f5c, 1);
        return 0;
    }
    if (kind == 0x3c
        && (g_game8c83fa & 0x2) == 0
        && Game_Helper411d90(a, b) != 0
        && Game_Helper411d90(g_game8e1aa8, g_game951bdc) != 0
        && Game_Helper48808b(g_game7a01e0, Game_Helper4ba309(a, b, 4)) != 0) {
        g_game7a01e0 = Game_Helper4ba309(a, b, g_gameSlots[a][b].field38) - 1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004eadb0
int Game_CardScript4eadb0(int a, int b, int kind)
{
    int v4;

    if (kind == 0x74) {
        return Game_Helper4be650(0, 0, a, 2, 2, 0x200, 1, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c && g_game951bdc == b && g_game8e1aa8 == a) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58a814, 0x58a800);
            }
            if (Game_Helper49fda3(a, 1 - a, b) != 0) {
                if (a == g_game8c776c || (g_game93a934 & 0x2) != 0) {
                    if (g_game8bd200 != 1) {
                        Game_Helper56ca10(0x58a834, 0x58a820);
                    }
                    v4 = Game_Helper5589ed(a, 0x8aa920 + 0x12c, 0, -1, 0x3e);
                    if (v4 == -1) {
                        g_game8c83ec = 1;
                    }
                } else {
                    if (g_game8bd200 == 1) {
                        g_game94d46c = Game_Helper522508(5) + 1;
                        v4 = g_game94d46c;
                        Game_Helper4c4181();
                    } else {
                        Game_Helper4c42f8();
                        v4 = g_game94d46c;
                    }
                }
                if (g_gameSlots[a][b].field74 == a) {
                    g_game94d63c -= 0x30;
                }
                g_gameSlots[a][b].field20 = v4;
            } else {
                g_game8c83ec = 1;
            }
            return 0;
        } else {
            if (kind == 0x71) {
                if (Game_Helper4bea85(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78, 0, a, 2, 2, 0x200, 1, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                    g_gameSlots[a][b].field50 = g_gameSlots[a][b].field74;
                    g_gameSlots[a][b].field04 = g_gameSlots[a][b].field78;
                    g_gameSlots[a][b].field20--;
                    g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field6c = g_gameSlots[a][b].field20;
                    g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field28 |= 0x1000000;
                } else {
                    Game_Helper46d8c1(a, b, 1);
                    g_game8c83ec = 1;
                }
                g_gameSlots[a][b].field36 = 0;
                return 0;
            } else {
                if (kind == 0x3c && (g_game8c83fa & 0x2) == 0 && g_gameSlots[a][b].field04 == g_game951bdc && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8 && g_game951bdc != -1) {
                    if (Game_Helper411d90(a, b) != 0) {
                        g_game7a01e0 = g_gameSlots[a][b].field20;
                    }
                }
                return 0;
            }
        }
    }
}

// FUNCTION: SHANDALAR 0x004ebb33
int Game_CardScript4ebb33(int a, int b, int kind)
{
    if (kind == 0x74) {
        Game_Helper4c4263(0);
        return Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c && g_game951bdc == b && a == g_game8e1aa8) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58a860, 0x58a858);
            }
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
            return 0;
        } else {
            if (kind == 0x71) {
                if (Game_Helper4bea85(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                    g_gameSlots[a][b].field50 = g_gameSlots[a][b].field74;
                    g_gameSlots[a][b].field04 = g_gameSlots[a][b].field78;
                    g_gameSlots[a][b].field38 = 1;
                } else {
                    Game_Helper46d8c1(a, b, 1);
                    g_game8c83ec = 1;
                }
                g_gameSlots[a][b].field36 = 0;
                return 0;
            } else {
                if (g_gameSlots[a][b].field38 != 0 && g_gameSlots[a][b].field78 == g_game951bdc && g_gameSlots[a][b].field74 == g_game8e1aa8 && g_game951bdc != -1 && kind == 0x34) {
                    g_game7a01e0 |= 0x20;
                }
                return 0;
            }
        }
    }
}
