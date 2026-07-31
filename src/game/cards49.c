/* Card behaviour scripts at 0x49xxxx.
 *
 * Split into per-range translation units because MSVC 4.2 re-rolls codegen
 * across a whole TU when anything in it changes; smaller units confine the
 * blast radius. Formulations follow the archived attempt, whose per-function
 * reccmp scores were validated to reproduce exactly under this toolchain.
 *
 * Comparison operand order and index-subexpression order are compiler-chosen
 * and not reachable from source (measured in the archive); do not chase them.
 * Address literals passed as ints are pointers into the original image's
 * data and are intentional. CUR/SRC live in game/gameslot.h. */

#include "game/gameslot.h"

/* No kind test; fires on every event aimed at its slot. */
// FUNCTION: SHANDALAR 0x0049744a
int Game_CardScript49744a(int a, int b, int kind)
{
    if (g_game951bdc == b && g_game8e1aa8 == a) {
        g_gameSlots[a][b].flags08 |= 0x2000;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004974ab
int Game_CardScript4974ab(int a, int b, int kind)
{
    if (kind == 0x34
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && a == g_game7a82e0) {
        g_game7a01e0 &= ~0x20;
    }
    return 0;
}

/* Three separate return statements match the three xor/jmp pairs. */
// FUNCTION: SHANDALAR 0x0049963e
int Game_CardScript49963e(int a, int b, int kind)
{
    if (kind == 0x1a
        && a != g_game7a82e0
        && g_gameSlots[a][b].field24 != -1) {
        g_gameSlots[a][b].field38 = 1;
        return 0;
    }
    if (kind == 0x79) {
        if (g_gameSlots[a][b].field38 == 0) {
            g_game7a01e0 = 1;
        }
        return 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0049c127
int Game_CardScript49c127(int a, int b, int kind)
{
    if (kind == 0x77 && b == g_game951bdc && a == g_game8e1aa8) {
        CUR.field68 = 4;
    }
    return 0;
}

/* Sets a bit for the duration of the inner call, then restores the counter.
 * 0x0049c630 below is the same with 1 replaced by 5. */
// FUNCTION: SHANDALAR 0x0049c5b0
int Game_CardScript49c5b0(int a, int b, int kind)
{
    int saved;

    if (kind == 0x34 && g_game951bdc == b && a == g_game8e1aa8) {
        g_game7a01e0 |= 0x800 << (Game_Helper4ba392(a, b, 1) - 1);
        saved = g_game7a01e0;
        Game_Helper49c6b1(a, b, 1);
        g_game7a01e0 = saved;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0049c630
int Game_CardScript49c630(int a, int b, int kind)
{
    int saved;

    if (kind == 0x34 && g_game951bdc == b && a == g_game8e1aa8) {
        g_game7a01e0 |= 0x800 << (Game_Helper4ba392(a, b, 5) - 1);
        saved = g_game7a01e0;
        Game_Helper49c6b1(a, b, 5);
        g_game7a01e0 = saved;
    }
    return 0;
}


/* First block runs on every event, guarded only by the helpers' answers. */
// FUNCTION: SHANDALAR 0x0049959d
int Game_CardScript49959d(int a, int b, int kind)
{
    if (Game_Helper411d90(a, b) != 0
        && g_game77ed80[a][Game_Helper4ba309(a, b, 2)] == 0) {
        Game_Helper46d8c1(a, b, 2);
    }
    if (kind == 0x79
        && g_game77ed80[1 - a][Game_Helper4ba309(a, b, 2)] == 0) {
        g_game7a01e0 = 1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00498baa
int Game_CardScript498baa(int a, int b, int kind)
{
    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x20010) == 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x90) {
        Game_Helper4c4263(1);
        return 0;
    }
    if (kind == 0x6d) {
        if (g_game8bd200 != 1) {
            Game_Helper56ca10(0x58704c, 0x587038);
        }
        Game_Helper498e8a(a, b, 1);
        if (g_game8c83ec != 1) {
            g_gameSlots[a][b].flags08 |= 0x10;
        }
        return 0;
    }
    if (kind == 0x72) {
        Game_Helper49914d(a, b, kind, 1);
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
        return 0;
    }
    if (kind == 0x3b
        && (g_gameSlots[a][b].flags08 & 0x20014) == 0) {
        g_game7beb18[1 - a]--;
        return 0;
    }
    if (kind == 0xc7
        && a == g_game7a82e0
        && g_game8c7468 == a
        && (g_gameSlots[a][b].flags08 & 0x20010) == 0) {
        g_game94d63c += 0x18;
    }
    if (kind == 0x8a
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_game97db30 += 0x30;
        return 0;
    }
    if (kind == 0x8b
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_game97db30 -= 0x30;
        return 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00497760
int Game_CardScript497760(int a, int b, int kind)
{
    if (kind == 0xc7
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && g_game7a82e0 == a
        && (g_gameSlots[a][b].flags08 & 0x30044) == 0) {
        if (g_game8c776c != a) {
            if ((g_game93a934 & 0x2) != 0
                && g_game8bd200 != 1) {
                Game_Helper56ca10(0x586fbc, 0x586fb0);
                Game_Helper558538(a, a, b, -1, -1, 0x8aa920, 0);
            }
            Game_Helper4bab38(a, 2, a, b);
            if (g_game94d178 == 0xcd
                && g_game951bdc == b
                && g_game8e1aa8 == a
                && g_game7a82e0 == a
                && a == g_game8c8afc
                && (g_gameSlots[a][b].flags08 & 0x30044) == 0) {
                if (kind == 0x7d) {
                    g_game7a01e0 |= 2;
                }
                if (kind == 0x7e) {
                    if (g_game8c776c != a) {
                        if ((g_game93a934 & 0x2) != 0
                            && g_game8bd200 != 1) {
                            Game_Helper56ca10(0x586fd4, 0x586fc8);
                            Game_Helper558538(a, a, b, -1, -1, 0x8aa920, 0);
                        }
                        Game_Helper4bab38(a, 2, a, b);
                    }
                }
            }
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004985d4
int Game_CardScript4985d4(int a, int b, int kind)
{
    if (kind == 0x73) {
        return Game_Helper441699(a, 4, 2) != 0
            && Game_Helper441699(a, 7, 3) != 0;
    }
    if (kind == 0x90) {
        Game_Helper4c4263(1);
        return 0;
    }
    if (kind == 0x6d) {
        g_game8e2690[0] = 1;
        Game_Helper4143e0(a, 4, 2);
        if (g_game8c83ec != 1) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58700c, 0x586ff8);
            }
            Game_Helper498e8a(a, b, 1);
        }
        return 0;
    }
    if (kind == 0x72) {
        if (Game_Helper49914d(a, b, kind, 1) != 0) {
            Game_Helper4bab38(a, 1, g_game94f280, g_game930f5c);
        }
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
        return 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004993ea
int Game_CardScript4993ea(int a, int b, int kind)
{
    if (kind == 0x73) {
        return (g_gameSlots[a][b].flags08 & 0x20010) == 0;
    }
    if (kind == 0x90) {
        Game_Helper4c4263(1);
        return 0;
    }
    if (kind == 0x6d) {
        if (g_game8bd200 != 1) {
            Game_Helper56ca10(0x587068, 0x58705c);
        }
        Game_Helper498e8a(a, b, 1);
        if (g_game8c83ec != 1) {
            g_gameSlots[a][b].flags08 |= 0x10;
        }
    }
    if (kind == 0x72) {
        Game_Helper49914d(a, b, kind, 1);
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
    }
    Game_CardScript49959d(a, b, kind);
    return 0;
}

// FUNCTION: SHANDALAR 0x00499d35
int Game_CardScript499d35(int a, int b, int kind)
{
    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x20010) == 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x90) {
        Game_Helper4c4263(1);
        return 0;
    }
    if (kind == 0x6d) {
        if (g_game8bd200 != 1) {
            Game_Helper56ca10(0x5870b4, 0x5870a0);
        }
        Game_Helper498e8a(a, b, 2);
        if (g_game8c83ec != 1) {
            g_gameSlots[a][b].flags08 |= 0x10;
        }
        return 0;
    }
    if (kind == 0x72) {
        if (Game_Helper49914d(a, b, kind, 2) != 0) {
            Game_Helper4bab38(a, 3, g_game94f280, g_game930f5c);
        }
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
        return 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004906e7
int Game_CardScript4906e7(int a, int b, int kind)
{
    int v4;

    if (kind == 0x71) {
        v4 = Game_Helper4a9910(a, b, g_game951968, a, b);
        if (v4 != -1) {
            g_gameSlots[a][v4].field38 = g_gameSlots[a][b].field6c;
            g_gameSlots[a][v4].field4c = 0x20f;
            g_gameSlots[a][v4].flags18 = 0x10020;
            g_gameSlots[a][b].field0c = a;
            g_gameSlots[a][b].field48 = v4;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00499ae9
int Game_CardScript499ae9(int a, int b, int kind)
{
    if (Game_Helper411d90(a, b) != 0
        && Game_Helper411d90(g_game8e1aa8, g_game951bdc) != 0
        && (signed char)g_gameCards[g_gameSlots[g_game8e1aa8][g_game951bdc].field6c].field29 == 1) {
        if (kind == 0x34) {
            g_game7a01e0 |= 1 << Game_Helper4ba309(a, b, 2) - 1;
            return 0;
        }
        if (kind == 0x32 || kind == 0x33) {
            g_game7a01e0++;
            return 0;
        }
        if (kind == 0x77
            && (g_gameSlots[a][b].flags18 & 0x80) != 0) {
            g_gameSlots[g_game8e1aa8][g_game951bdc].field28 |= 0xe000000;
            return 0;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0049bfca
int Game_CardScript49bfca(int a, int b, int kind)
{
    if (g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_gameSlots[a][b].flags08 &= ~0x30000;
    }
    if (kind == 0xc7) {
        Game_Helper46d8c1(a, b, 1);
    }
    if (g_game94d178 == 0xcd
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && g_game8c8afc == a) {
        if (kind == 0x7d) {
            g_game7a01e0 |= 2;
        }
        if (kind == 0x7e) {
            Game_Helper46d8c1(a, b, 1);
        }
    }
    if (kind == 0x8a
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_game97db30 -= 0x3c;
        return 0;
    }
    if (kind == 0x8b
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_game97db30 += 0x3c;
        return 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0049e1f3
int Game_CardScript49e1f3(int a, int b, int kind)
{
    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x20010) == 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x6d
        && (g_gameSlots[a][b].flags08 & 0x10) == 0) {
        Game_Helper441651(a, 4, 1);
        g_gameSlots[a][b].flags08 |= 0x10;
        g_game93a794 = 4;
        return 0;
    }
    if (kind == 0x7f
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        if ((g_gameSlots[a][b].flags08 & 0x20010) == 0) {
            Game_Helper441539(a, 4, 1);
        }
        return 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00490853
int Game_CardScript490853(int a, int b, int kind)
{
    int v4;

    if (kind == 0x71) {
        v4 = Game_Helper4a9910(a, b, g_game951968, a, b);
        if (v4 != -1) {
            g_gameSlots[a][v4].field4c = 0x80d;
            g_gameSlots[a][v4].flags18 = 0x10020;
            g_gameSlots[a][b].field0c = a;
            g_gameSlots[a][b].field48 = v4;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004974fb
int Game_CardScript4974fb(int a, int b, int kind)
{
    if (kind == 0x34
        && Game_Helper411d90(a, b) != 0
        && (signed char)g_gameCards[g_gameSlots[g_game8e1aa8][g_game951bdc].field6c].field29 == 2) {
        g_game7a01e0 |= (1 << Game_Helper4ba309(a, b, 1) - 1) + 0x200;
    }
    if (kind == 0x77
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        Game_Helper49f3c0((int)Game_Callback4975e2, -1);
        Game_Helper558aea(0, 0xff);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0049768d
int Game_CardScript49768d(int a, int b, int kind)
{
    if (Game_Helper411d90(a, b) != 0
        && Game_Helper411d90(g_game8e1aa8, g_game951bdc) != 0
        && (signed char)g_gameCards[g_gameSlots[g_game8e1aa8][g_game951bdc].field6c].field29 == 3) {
        if (kind == 0x34) {
            g_game7a01e0 |= 1 << Game_Helper4ba309(a, b, 4) - 1;
        }
        if (kind == 0x32 || kind == 0x33) {
            g_game7a01e0++;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00499c47
int Game_CardScript499c47(int a, int b, int kind)
{
    if (kind == 0x79) {
        if (g_game77ed80[1 - a][Game_Helper4ba309(a, b, 4)] == 0) {
            g_game7a01e0 = 1;
        }
        return 0;
    }
    if (kind == 0x1a
        && (g_gameSlots[a][b].flags08 & 0x4) != 0) {
        Game_Helper4a9910(a, b, g_game7be608, a, b);
        g_gameSlots[a][b].field38 = 1;
        return 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0049e35e
int Game_CardScript49e35e(int a, int b, int kind)
{
    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x20010) == 0
            && Game_Helper441699(a, 2, 1) != 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x6d
        && (g_gameSlots[a][b].flags08 & 0x10) == 0
        && Game_Helper441699(a, 2, 1) != 0) {
        Game_Helper4143e0(a, 2, 1);
        if (g_game8c83ec != 1) {
            Game_Helper441651(a, 0, 3);
            g_gameSlots[a][b].flags08 |= 0x10;
            g_game93a794 = 0;
        }
        return 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00499f04
int Game_CardScript499f04(int a, int b, int kind)
{
    if (kind == 0xc7
        && Game_Helper411d90(a, b) != 0
        && g_gameSlots[a][b].field0e > 3) {
        if (g_game8c7468 == a) {
            g_game94d63c += 0xc8;
        } else {
            g_game94d63c -= 0xc8;
        }
    }
    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x20010) == 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x90) {
        Game_Helper4c4263(1);
        return 0;
    }
    if (kind == 0x6d) {
        if (g_game8bd200 != 1) {
            Game_Helper56ca10(0x5870d0, 0x5870c0);
        }
        Game_Helper498e8a(a, b, 2);
        if (g_game8c83ec != 1) {
            g_gameSlots[a][b].flags08 |= 0x10;
        }
        return 0;
    }
    if (kind == 0x72) {
        if (Game_Helper49914d(a, b, kind, 2) != 0
            && g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field6c != -1) {
            Game_AllocSlot(g_game94f280, g_game930f5c, 3, g_game94f280, g_game930f5c);
        }
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
        return 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0049bb0a
int Game_CardScript49bb0a(int a, int b, int kind)
{
    int v4;

    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        if (g_game8e3ed0[a] < 2) {
            g_game94d63c -= 0xa8;
        }
        return 0;
    }
    if (kind == 0x87) {
        if (Game_Helper49bf0e(a, b) == 0) {
            g_game7a01e0 |= 1;
        }
        return 0;
    }
    if (kind == 0x85
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && g_gameSlots[a][b].field38 == 0) {
        if (g_game7a82e0 == a
            && g_game77c57c == a) {
            g_gameSlots[a][b].field5c |= 0x101;
            if (Game_Helper49bf0e(a, b) == 0) {
                g_game94d45c++;
            }
        }
        return 0;
    }
    if (kind == 4
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_gameSlots[a][b].field38++;
        if (Game_Helper49bf0e(a, b) != 0) {
            g_gameSlots[a][b].flags08 |= 0x100000;
            Game_Helper558aea(0, 0x20);
            Game_Helper56ca10(0x587164, 0x587154);
            v4 = Game_Helper49fb64(a);
            g_gameSlots[a][b].flags08 &= ~0x100000;
            Game_Helper46d8c1(a, v4, 3);
            return 0;
        }
        g_game7a01e0 |= 1;
        return 0;
    }
    if (kind == 0x86) {
        Game_Helper56ca10(0x587180, 0x587170);
        Game_Helper558538(a, a, b, -1, -1, 0x8aa920 + 0x12c, 0);
        Game_Helper4bab38(a, 7, g_game94f280, g_game930f5c);
        return 0;
    }
    if (kind == 0xc7
        && Game_Helper49bf0e(a, b) == 0) {
        Game_Helper4bab38(a, 7, a, b);
    }
    if ((kind == 0x22 || kind == 0xc7)
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_gameSlots[a][b].field38 = 0;
    }
    if (kind == 0x8a
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_game97db30 -= 0x30;
        return 0;
    }
    if (kind == 0x8b
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_game97db30 += 0x30;
        return 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00497258
int Game_CardScript497258(int a, int b, int kind)
{
    int v4;

    if (g_game94d178 == 0xd3
        && b == g_game951bdc
        && a == g_game8e1aa8
        && a == g_game8c8afc
        && a == g_game7a82e0
        && (g_gameSlots[a][b].flags08 & 0x20) == 0
        && a == g_game8e1aa8
        && g_gameSlots[g_game7bf4b8][g_game8e2fc4].field6c != -1
        && (g_gameCards[g_gameSlots[g_game7bf4b8][g_game8e2fc4].field6c].flags & 0x4) != 0) {
        if (kind == 0x7d) {
            if (a == g_game8c776c || (g_game93a934 & 0x2) != 0) {
                g_game7a01e0 |= 1;
            } else {
                for (v4 = 0; v4 < 0x1f4; v4++) {
                    if (g_game8a9410[a][v4] == -1) {
                        break;
                    }
                }
                if (g_game8e3ec8[a] < 8
                    && v4 > 5) {
                    g_game7a01e0 |= 2;
                } else {
                    g_game7a01e0 |= 1;
                }
            }
        }
        if (kind == 0x7e) {
            Game_Helper40e158(a, b, 0x91);
            Game_Helper409a0b(a);
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0049b183
int Game_CardScript49b183(int a, int b, int kind)
{
    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x20010) == 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x6d
        && (g_gameSlots[a][b].flags08 & 0x10) == 0) {
        Game_Helper441651(a, 3, 1);
        g_gameSlots[a][b].flags08 |= 0x10;
        g_game93a794 = 3;
        return 0;
    }
    if (kind == 0x7f
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        if ((g_gameSlots[a][b].flags08 & 0x20010) == 0) {
            Game_Helper441539(a, 3, 1);
        }
        return 0;
    }
    if (kind == 0x8a
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_game97db30 += 0x18 / (g_game77ed80[a][3] + 2);
        return 0;
    }
    if (kind == 0x8b
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_game97db30 -= -(0 - 0x60 / (g_game77ed80[a][3] + 2));
        return 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00494f82
int Game_CardScript494f82(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (kind == 0x1a
        && g_game7a82e0 == a
        && (g_gameSlots[a][b].flags08 & 0x4) != 0) {
        ig = 1 - a;
        ih = 1;
        for (iv = 0; g_game78e970[ig] > iv; iv++) {
            if (Game_Helper411d90(ig, iv) != 0
                && (signed char)g_gameSlots[ig][iv].field24 == b) {
                ih = 0;
                break;
            }
        }
        if (ih != 0) {
            iv = Game_Helper4a9910(a, b, g_game8bcff8, a, b);
            if (iv != -1) {
                g_gameSlots[a][iv].field12 = 2;
                g_gameSlots[a][iv].field1c = 0;
            }
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0049a1eb
int Game_CardScript49a1eb(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x73
        && (g_game8c83f9 & 0x2) != 0) {
        v4 = Game_Helper4909b0(a, b, kind, 0, 0);
        if (Game_Helper49f838(a, b) == 0) {
            v4 = 0;
        }
    } else {
        if (kind == 0x6d
            && (g_game8c83f9 & 0x2) != 0) {
            v8 = Game_Helper4909b0(a, b, kind, 0, 0);
            Game_Helper49f710(a, b, 1);
        } else {
            if (kind == 0x72
                && (g_game8c83f9 & 0x2) != 0) {
                Game_Helper4909b0(a, b, kind, 0, 0);
            } else {
                if ((g_game94d178 == 0xcd || kind == 0xc7)
                    && g_game951bdc == b
                    && g_game8e1aa8 == a
                    && g_game8e3efc != 0
                    && g_game8c8afc == a) {
                    if (kind == 0x7d) {
                        g_game7a01e0 |= 2;
                    }
                    if (kind == 0x7e || kind == 0xc7) {
                        Game_Helper49f61a(a, b, g_game8e3efc);
                    }
                }
            }
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0049c195
int Game_CardScript49c195(int a, int b, int kind)
{
    int v4;
    int v8;

    if (g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_gameSlots[a][b].flags08 &= ~0x30000;
    }
    if ((kind == 0x8d || kind == 0x77)
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && (g_gameSlots[a][b].flags08 & 0x20) == 0
        && (signed char)g_gameSlots[a][b].field68 != 4) {
        if ((g_gameSlots[a][b].flags08 & 0x1000) != 0) {
            v8 = g_game8c7468;
        } else {
            v8 = g_game8c776c;
        }
        v4 = Game_FindFreeSlot(v8, g_game8bd394);
        if (v4 != -1) {
            g_gameSlots[v8][v4].flags08 |= 2;
            g_gameSlots[v8][v4].field64 = g_gameCards[g_gameSlots[a][b].field6c].mask;
        }
        return 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0049deff
int Game_CardScript49deff(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (kind == 0x1a) {
        ih = 1 - a;
        if (g_game7a82e0 != a
            && (signed char)g_gameSlots[a][b].field24 != -1
            && g_gameSlots[a][b].field38 == 0) {
            g_gameSlots[a][b].field38 |= 1;
            iv = (signed char)g_gameSlots[ih][(signed char)g_gameSlots[a][b].field24].field24;
            if (iv == -1) {
                Game_Helper4a9910(a, b, g_game8e291c, ih, (signed char)g_gameSlots[a][b].field24);
                g_gameSlots[ih][(signed char)g_gameSlots[a][b].field24].flags18 |= 0x8000;
                return 0;
            }
            for (ig = 0; g_game78e970[ih] > ig; ig++) {
                if (Game_Helper411d90(ih, ig) != 0
                    && (signed char)g_gameSlots[ih][ig].field24 == iv) {
                    Game_Helper4a9910(a, b, g_game8e291c, ih, ig);
                    g_gameSlots[ih][ig].flags18 |= 0x8000;
                }
            }
        }
        return 0;
    }
    if (kind == 0x22) {
        g_gameSlots[a][b].field38 = 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00493829
int Game_CardScript493829(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (kind == 1) {
        g_game94d660[a][1]++;
        return 0;
    }
    if (kind == 0x73) {
        ih = 1;
        if ((g_gameSlots[a][b].flags08 & 0x20010) != 0) {
            ih = 0;
        }
        if (ih != 0
            && Game_Helper441699(a, 2, 2) == 0) {
            ih = 0;
        }
        if (ih != 0
            && Game_Helper441699(a, 7, 4) == 0) {
            ih = 0;
        }
        if (ih != 0) {
            ih = Game_Helper4be650(0, 0, a, 2, 2, 0x200, 0x1047, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0x40);
        }
    } else {
        if (kind == 0x90) {
            Game_Helper4c4263(0);
            return 0;
        }
        if (kind == 0x6d
            && (g_gameSlots[a][b].flags08 & 0x20010) == 0) {
            g_game8e2690[0] = 2;
            Game_Helper4143e0(a, 2, 2);
            if (g_game8c83ec != 1) {
                if (g_game8bd200 != 1) {
                    Game_Helper56ca10(0x586ec4, 0x586eb4);
                }
                if (Game_Helper4c0efa(a, 2, 1 - a, 0x200, 0x1047, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0x40, 0x8aa920, 1, &ig) != 0) {
                    g_gameSlots[a][b].field74 = ig;
                    g_gameSlots[a][b].field78 = iv;
                    g_gameSlots[a][b].field36 = 1;
                    g_gameSlots[a][b].flags08 |= 0x10;
                    return 0;
                }
                g_game8c83ec = 1;
            }
            return 0;
        }
        if (kind == 0x72) {
            ig = g_gameSlots[a][b].field74;
            iv = g_gameSlots[a][b].field78;
            if (Game_Helper4bea85(ig, iv, 0, a, 2, 2, 0x200, 0x1047, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0x40) != 0) {
                Game_Helper4ba41b(ig, iv);
            } else {
                g_game8c83ec = 1;
            }
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
            return 0;
        }
        if ((kind == 0x15 || kind == 0xc7)
            && (g_gameSlots[a][b].flags08 & 0x4) != 0) {
            Game_Helper4a9910(a, b, g_game7bf0e4, a, -1);
        }
        if ((kind == 0x1a || kind == 0xc7)
            && g_game94d464 == 0x17
            && (signed char)g_gameSlots[a][b].field24 != -1
            && g_gameCards[g_gameSlots[a][b].field6c].script != (int)Game_Callback48592f) {
            Game_Helper4a9910(a, b, g_game7bf0e4, a, -1);
        }
        if (kind == 0x22 || kind == 0xc7) {
            g_gameSlots[a][b].field38 = 0;
        }
        if (kind == 0x8a
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            g_game97db30 += 0x78;
            return 0;
        }
        if (kind == 0x8b
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            g_game97db30 -= 0x78;
            return 0;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00493e75
int Game_CardScript493e75(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (kind == 1) {
        g_game94d660[a][1]++;
        return 0;
    }
    if (kind == 0x73) {
        ih = 1;
        if ((g_gameSlots[a][b].flags08 & 0x20010) != 0) {
            ih = 0;
        }
        if (ih != 0
            && Game_Helper441699(a, 5, 2) == 0) {
            ih = 0;
        }
        if (ih != 0) {
            ih = Game_Helper4be650(0, 0, a, 2, 2, 0x200, 0x1047, 0, 0, Game_Helper488cc4(a, b), 1 << Game_Helper4ba392(a, b, 1), 0, -1, -1, -1, -1, 0, 0, 0);
        }
    } else {
        if (kind == 0x90) {
            Game_Helper4c4263(0);
            return 0;
        }
        if (kind == 0x6d) {
            if (Game_Helper441699(a, 5, 2) != 0
                && (g_gameSlots[a][b].flags08 & 0x20010) == 0) {
                Game_Helper4143e0(a, 5, 2);
                if (g_game8c83ec != 1) {
                    if (g_game8bd200 != 1) {
                        Game_Helper56ca10(0x586ee4, 0x586ed0);
                    }
                    if (Game_Helper4c0efa(a, 2, 2, 0x200, 0x1047, 0, 0, Game_Helper488cc4(a, b), 1 << Game_Helper4ba392(a, b, 1), 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &ig) != 0) {
                        g_gameSlots[a][b].field74 = ig;
                        g_gameSlots[a][b].field78 = iv;
                        g_gameSlots[a][b].field36 = 1;
                        g_gameSlots[a][b].flags08 |= 0x10;
                        return 0;
                    }
                    g_game8c83ec = 1;
                }
            }
            return 0;
        }
        if (kind == 0x72) {
            ig = g_gameSlots[a][b].field74;
            iv = g_gameSlots[a][b].field78;
            if (Game_Helper4bea85(ig, iv, 0, a, 2, 2, 0x200, 0x1047, 0, 0, Game_Helper488cc4(a, b), 1 << Game_Helper4ba392(a, b, 1), 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                Game_Helper46d8c1(ig, iv, 2);
            } else {
                g_game8c83ec = 1;
            }
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
            return 0;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00494312
int Game_CardScript494312(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (kind == 0x73) {
        ih = 1;
        if ((g_gameSlots[a][b].flags08 & 0x20010) != 0) {
            ih = 0;
        }
        if (ih != 0) {
            ih = Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 1, 0);
        }
    } else {
        if (kind == 0x6d) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x586f00, 0x586ef0);
            }
            if (Game_Helper4c0efa(a, 2, 1 - a, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 1, 0, 0x8aa920, 1, &ig) != 0) {
                g_gameSlots[a][b].field74 = ig;
                g_gameSlots[a][b].field78 = iv;
                g_gameSlots[a][b].field36 = 1;
                g_gameSlots[a][b].flags08 |= 0x10;
                return 0;
            }
            g_game8c83ec = 1;
            return 0;
        }
        if (kind == 0x90) {
            Game_Helper4c4263(0);
            return 0;
        }
        if (kind == 0x72) {
            ig = g_gameSlots[a][b].field74;
            iv = g_gameSlots[a][b].field78;
            if (Game_Helper4bea85(ig, iv, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 1, 0) != 0) {
                Game_Helper46d8c1(ig, iv, 2);
            } else {
                g_game8c83ec = 1;
            }
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
            return 0;
        }
        if (kind == 0x8a
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            g_game97db30 += 0x30;
            return 0;
        }
        if (kind == 0x8b
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            g_game97db30 -= 0x30;
            return 0;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00495df9
int Game_CardScript495df9(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (kind == 0x73) {
        ih = 1;
        if ((g_gameSlots[a][b].flags08 & 0x20010) != 0) {
            ih = 0;
        }
        if (ih != 0) {
            ih = Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, 0x2002, -1, 0, 0, 0);
        }
    } else {
        if (kind == 0x90) {
            Game_Helper4c4263(0);
            return 0;
        }
        if (kind == 0x6d
            && (g_gameSlots[a][b].flags08 & 0x20010) == 0) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x586f54, 0x586f40);
            }
            if (Game_Helper4c0efa(a, 2, a, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, 0x2002, -1, 0, 0, 0, 0x8aa920, 1, &ig) != 0) {
                g_gameSlots[a][b].field74 = ig;
                g_gameSlots[a][b].field78 = iv;
                g_gameSlots[a][b].field36 = 1;
                g_gameSlots[a][b].flags08 |= 0x10;
                return 0;
            }
            g_game8c83ec = 1;
            return 0;
        }
        if (kind == 0x72) {
            ig = g_gameSlots[a][b].field74;
            iv = g_gameSlots[a][b].field78;
            if (Game_Helper4bea85(ig, iv, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, 0x2002, -1, 0, 0, 0) != 0) {
                Game_Helper4a9910(g_game94f280, g_game930f5c, g_game7bfe7c, ig, iv);
            } else {
                g_game8c83ec = 1;
            }
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
            return 0;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004961df
int Game_CardScript4961df(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;
    int ii;

    if (kind == 0x73) {
        ii = 1;
        if ((g_gameSlots[a][b].flags08 & 0x20010) != 0) {
            ii = 0;
        }
        if (ii != 0
            && Game_Helper441699(a, 4, 2) == 0) {
            ii = 0;
        }
        if (ii != 0
            && Game_Helper441699(a, 7, 3) == 0) {
            ii = 0;
        }
        if (ii != 0) {
            ii = Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
        }
    } else {
        if (kind == 0x90) {
            Game_Helper4c4263(0);
            return 0;
        }
        if (g_game951bdc == b
            && g_game8e1aa8 == a
            && (g_gameSlots[a][b].flags08 & 0x44) != 0) {
            if (kind == 0x32) {
                g_game7a01e0++;
            }
            if (kind == 0x33) {
                g_game7a01e0 -= 2;
            }
            return 0;
        }
        if (kind == 0x6d
            && (g_gameSlots[a][b].flags08 & 0x20010) == 0) {
            g_game8e2690[0] = 1;
            Game_Helper4143e0(a, 4, 2);
            if (g_game8c83ec != 1) {
                if (g_game8bd200 != 1) {
                    Game_Helper56ca10(0x586f6c, 0x586f60);
                }
                if (Game_Helper4c0efa(a, 2, a, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &ih) != 0) {
                    g_gameSlots[a][b].field74 = ih;
                    g_gameSlots[a][b].field78 = ig;
                    g_gameSlots[a][b].field36 = 1;
                    if (g_game8c7468 == a) {
                        if ((g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].field28 & 0x17) != 0 || g_gameSlots[a][b].field74 == g_game8c776c) {
                            g_game94d63c -= 0x60;
                        }
                    }
                    g_gameSlots[a][b].flags08 |= 0x10;
                    return 0;
                }
                g_game8c83ec = 1;
            }
            return 0;
        }
        if (kind == 0x72) {
            ih = g_gameSlots[a][b].field74;
            ig = g_gameSlots[a][b].field78;
            if (Game_Helper4bea85(ih, ig, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                iv = Game_Helper4a9910(g_game94f280, g_game930f5c, g_game8aa734, ih, ig);
                if (iv != -1) {
                    g_gameSlots[a][iv].field38 = 1 << Game_Helper4ba309(a, b, 4) - 1;
                }
                g_gameSlots[ih][ig].field28 = 0x8000000;
            } else {
                g_game8c83ec = 1;
            }
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
            return 0;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00498786
int Game_CardScript498786(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (kind == 0x73) {
        ih = 1;
        if ((g_gameSlots[a][b].flags08 & 0x20010) != 0) {
            ih = 0;
        }
        if (ih != 0
            && Game_Helper441699(a, 4, 1) == 0) {
            ih = 0;
        }
        if (ih != 0) {
            ih = Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0x20, 0);
        }
    } else {
        if (kind == 0x90) {
            Game_Helper4c4263(0);
            return 0;
        }
        if (kind == 0x6d
            && (g_gameSlots[a][b].flags08 & 0x20010) == 0) {
            Game_Helper4143e0(a, 4, 1);
            if (g_game8c83ec != 1) {
                if (g_game8bd200 != 1) {
                    Game_Helper56ca10(0x58702c, 0x587018);
                }
                if (Game_Helper4c0efa(a, 2, 1 - a, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0x20, 0, 0x8aa920, 1, &ig) != 0) {
                    g_gameSlots[a][b].field74 = ig;
                    g_gameSlots[a][b].field78 = iv;
                    g_gameSlots[a][b].field36 = 1;
                    g_gameSlots[a][b].flags08 |= 0x10;
                    return 0;
                }
                g_game8c83ec = 1;
            }
            return 0;
        }
        if (kind == 0x72) {
            ig = g_gameSlots[a][b].field74;
            iv = g_gameSlots[a][b].field78;
            if (Game_Helper4bea85(ig, iv, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0x20, 0) != 0) {
                Game_AllocSlot(ig, iv, 1, g_game94f280, g_game930f5c);
            } else {
                g_game8c83ec = 1;
            }
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
            return 0;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0049c834
int Game_CardScript49c834(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x73) {
        if (Game_Helper441699(a, 4, 1) != 0
            && Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 1, 0, 0) != 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x90) {
        Game_Helper4c4263(0);
        return 0;
    }
    if (kind == 0x6d
        && Game_Helper441699(a, 4, 1) != 0) {
        Game_Helper4143e0(a, 4, 1);
        if (g_game8c83ec != 1) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x5871b4, 0x5871a8);
            }
            if (Game_Helper4c0efa(a, 2, 1 - a, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 1, 0, 0, 0x8aa920, 1, &v8) != 0) {
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
        v8 = g_gameSlots[a][b].field74;
        v4 = g_gameSlots[a][b].field78;
        if (Game_Helper4bea85(v8, v4, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 1, 0, 0) != 0) {
            g_gameSlots[v8][v4].flags08 |= 0x10;
        } else {
            g_game8c83ec = 1;
        }
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
        return 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0049cbc7
int Game_CardScript49cbc7(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x20010) == 0
            && Game_Helper4be650(0, 0, a, 2, 2, 0x200, 1, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x90) {
        Game_Helper4c4263(0);
        return 0;
    }
    if (kind == 0x6d) {
        if (g_game8bd200 != 1) {
            Game_Helper56ca10(0x5871cc, 0x5871c0);
        }
        if (Game_Helper4c0efa(a, 2, a, 0x200, 1, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &v8) != 0) {
            g_gameSlots[a][b].flags08 |= 0x10;
            g_gameSlots[a][b].field74 = v8;
            g_gameSlots[a][b].field78 = v4;
            g_gameSlots[a][b].field36 = 1;
            return 0;
        }
        g_game8c83ec = 1;
        return 0;
    }
    if (kind == 0x72) {
        v8 = g_gameSlots[a][b].field74;
        v4 = g_gameSlots[a][b].field78;
        if (Game_Helper4bea85(v8, v4, 0, a, 2, 2, 0x200, 1, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
            g_gameSlots[v8][v4].flags08 &= ~0x10;
            Game_Helper40e3dc(v8, v4, 1, -1, -1);
        } else {
            g_game8c83ec = 1;
        }
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
        return 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0049ad68
int Game_CardScript49ad68(int a, int b, int kind)
{
    if (kind == 0x85
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        if (a == g_game7a82e0
            && a == g_game77c57c) {
            g_gameSlots[a][b].field5c |= 1;
            g_gameSlots[a][b].field125 = (signed char)g_gameSlots[a][b].field125 + 2;
        }
        return 0;
    }
    if (kind == 0x86) {
        Game_Helper46d8c1(g_game94f280, g_game930f5c, 1);
        return 0;
    }
    if (kind == 0xc7
        && g_game77ed80[a][1] < 2) {
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0049aeb3
int Game_CardScript49aeb3(int a, int b, int kind)
{
    if (kind == 0x85
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        if (g_game7a82e0 == a
            && g_game77c57c == a) {
            g_gameSlots[a][b].field5c |= 1;
            g_gameSlots[a][b].field126 = (signed char)g_gameSlots[a][b].field126 + 1;
        }
        return 0;
    }
    if (kind == 0x86) {
        Game_Helper46d8c1(g_game94f280, g_game930f5c, 1);
        return 0;
    }
    if (kind == 0xc7
        && g_game77ed80[a][2] < 1) {
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0049affe
int Game_CardScript49affe(int a, int b, int kind)
{
    if (kind == 0x85
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        if (g_game7a82e0 == a
            && a == g_game77c57c) {
            g_gameSlots[a][b].field5c |= 1;
            g_gameSlots[a][b].field127 = (signed char)g_gameSlots[a][b].field127 + 4;
        }
        return 0;
    }
    if (kind == 0x86) {
        Game_Helper56ca10(0x5870ec, 0x5870dc);
        Game_Helper558538(a, a, b, -1, -1, 0x8aa920, 0);
        Game_Helper4bab38(a, 8, g_game94f280, g_game930f5c);
        return 0;
    }
    if (kind == 0xc7
        && g_game77ed80[a][3] < 4) {
        Game_Helper4bab38(a, 8, a, b);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0049b8dc
int Game_CardScript49b8dc(int a, int b, int kind)
{
    if (kind == 0x85
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        if (g_game7a82e0 == a
            && g_game77c57c == a) {
            g_gameSlots[a][b].field5c |= 1;
            g_gameSlots[a][b].field125 = (signed char)g_gameSlots[a][b].field125 + 3;
            g_gameSlots[a][b].field124 = (signed char)g_gameSlots[a][b].field124 + 3;
        }
        return 0;
    }
    if (kind == 0x86) {
        if (g_game8bd200 != 1) {
            Game_Helper56ca10(0x587148, 0x587138);
        }
        Game_Helper558538(a, a, b, -1, -1, 0x8aa920, 0);
        Game_Helper4bab38(a, 7, g_game94f280, g_game930f5c);
        Game_Helper46d8c1(g_game94f280, g_game930f5c, 1);
        return 0;
    }
    if (kind == 0xc7) {
        if (g_game77ed80[a][1] < 3 || g_game77ed80[a][7] < 6) {
            Game_Helper4bab38(a, 7, a, b);
            Game_Helper46d8c1(a, b, 1);
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0049cf85
int Game_CardScript49cf85(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;
    int ii;
    int ij;

    if (kind == 0x3c) {
        if (g_gameSlots[a][b].field04 != -1
            && (signed char)g_gameSlots[a][b].field50 != -1) {
            g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field1f = (signed char)g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field1f | 0x3f;
        }
        return 0;
    }
    if (kind == 0x74) {
        Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x90) {
            Game_Helper4c4263(0);
            return 0;
        }
        if (kind == 0x6c
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x5871e0, 0x5871d8);
            }
            if ((Game_Helper49f87b(a, a, b) == 0 || g_game8c7468 == a)
                && (g_game93a934 & 0x2) == 0
                && Game_Helper4e25e6(a, b, g_gameSlots[a][b].field6c) != 0) {
                g_game94d63c -= 0x60;
                return 0;
                g_game8c83ec = 1;
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
        if (kind == 0x1a) {
            ii = 1 - (signed char)g_gameSlots[a][b].field50;
            if ((signed char)g_gameSlots[a][b].field50 == g_game7a82e0
                && (g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags08 & 0x44) != 0) {
                if ((signed char)g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field24 == -1) {
                    ij = g_gameSlots[a][b].field04;
                } else {
                    ij = (signed char)g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field24;
                }
                for (ih = 0; g_game78e970[ii] > ih; ih++) {
                    if ((signed char)g_gameSlots[ii][ih].field24 == ij) {
                        iv = g_gameSlots[ii][ih].field6c;
                        if (iv != -1
                            && (signed char)g_gameCards[iv].field29 != 0
                            && (g_gameCards[iv].flags & 0x2) != 0) {
                            Game_Helper4a9910(a, b, g_game8c72b4, ii, ih);
                        }
                    }
                }
            }
            if ((signed char)g_gameSlots[a][b].field50 != g_game7a82e0
                && (signed char)g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field24 != -1) {
                ig = (signed char)g_gameSlots[ii][(signed char)g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field24].field24;
                if (ig == -1) {
                    Game_Helper4a9910(a, b, g_game8c72b4, ii, (signed char)g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field24);
                    return 0;
                }
                for (ih = 0; g_game78e970[ii] > ih; ih++) {
                    if (Game_Helper411d90(ii, ih) != 0
                        && (signed char)g_gameSlots[ii][ih].field24 == ig) {
                        Game_Helper4a9910(a, b, g_game8c72b4, ii, ih);
                    }
                }
            }
            return 0;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0049e49f
int Game_CardScript49e49f(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;
    int ii;
    int ij;
    int ik;

    ij = 1 - a;
    if (kind == 0x3c) {
        g_gameSlots[a][b].field1f = (signed char)g_gameSlots[a][b].field1f | 1 << Game_Helper4ba392(a, b, 3) | 1 << Game_Helper4ba392(a, b, 5) | 0x80;
        return 0;
    }
    if (kind == 0x1a) {
        iv = 1 << Game_Helper4ba392(a, b, 3) | 1 << Game_Helper4ba392(a, b, 5);
        if (a == g_game7a82e0
            && (g_gameSlots[a][b].flags08 & 0x44) != 0) {
            if ((signed char)g_gameSlots[a][b].field24 == -1) {
                ik = b;
            } else {
                ik = (signed char)g_gameSlots[a][b].field24;
            }
            for (ii = 0; g_game78e970[ij] > ii; ii++) {
                if ((signed char)g_gameSlots[ij][ii].field24 == ik) {
                    ig = g_gameSlots[ij][ii].field6c;
                    if (ig != -1
                        && (g_gameCards[ig].flags & 0x2) != 0
                        && (iv & 0x13) != 0) {
                        Game_Helper4a9910(a, b, g_game8c72b4, ij, ii);
                    }
                }
            }
        }
        if (a != g_game7a82e0
            && (signed char)g_gameSlots[a][b].field24 != -1) {
            ih = (signed char)g_gameSlots[ij][(signed char)g_gameSlots[a][b].field24].field24;
            if (ih != -1 || (iv & 0x13) != 0) {
                Game_Helper4a9910(a, b, g_game8c72b4, ij, (signed char)g_gameSlots[a][b].field24);
                return 0;
                for (ii = 0; g_game78e970[ij] > ii; ii++) {
                    if (Game_Helper411d90(ij, ii) != 0
                        && (signed char)g_gameSlots[ij][ii].field24 == ih
                        && (iv & 0x13) != 0) {
                        Game_Helper4a9910(a, b, g_game8c72b4, ij, ii);
                    }
                }
            }
        }
        return 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00490963
int Game_CardScript490963(int a, int b, int kind)
{
    if (kind == 0x73 || kind == 0x6d || kind == 0x72) {
        return Game_Helper4909b0(a, b, kind, 2, 3);
    } else {
        return 0;
    }
}

// FUNCTION: SHANDALAR 0x00495851
int Game_CardScript495851(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;
    int ii;

    if (kind == 0x73) {
        ii = 1;
        if ((g_gameSlots[a][b].flags08 & 0x20010) != 0) {
            ii = 0;
        }
        if (ii != 0) {
            ii = Game_Helper4be650(0, 0, a, a, a, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, g_gameSlots[a][b].field34 - 1 | 0x2000, -1, 0, 0, 0);
        }
        return ii;
    } else {
        if (kind == 0x90) {
            Game_Helper4c4263(0);
            return 0;
        } else {
            if (kind == 0x6d && (g_gameSlots[a][b].flags08 & 0x20010) == 0) {
                if (g_game8bd200 != 1) {
                    Game_Helper56ca10(0x586f34, 0x586f28);
                }
                if (Game_Helper4c0efa(a, a, a, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, Game_Helper40d7b2(a, b, 0x32, -1) - 1 | 0x2000, 0, 0, 0, 0x8aa920, 1, &ih) != 0) {
                    g_gameSlots[a][b].field74 = ih;
                    g_gameSlots[a][b].field78 = ig;
                    g_gameSlots[a][b].field36 = 1;
                    if (a == g_game8c7468) {
                        if ((g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].field28 & 0x20) != 0) {
                            g_game94d63c -= 0x60;
                        }
                    }
                    g_gameSlots[a][b].flags08 |= 0x10;
                } else {
                    g_game8c83ec = 1;
                }
                return 0;
            } else {
                if (kind == 0x72) {
                    ih = g_gameSlots[a][b].field74;
                    ig = g_gameSlots[a][b].field78;
                    if (Game_Helper4bea85(ih, ig, 0, a, a, a, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, g_gameSlots[a][b].field34 - 1 | 0x2000, 0, 0, 0) != 0) {
                        iv = Game_Helper4a9910(g_game94f280, g_game930f5c, g_game8aa734, ih, ig);
                        if (iv != -1) {
                            g_gameSlots[a][iv].field68 = 5;
                            g_gameSlots[a][iv].field38 = 0x20;
                            g_gameSlots[ih][ig].field28 = 0x8000000;
                        }
                    } else {
                        g_game8c83ec = 1;
                    }
                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
                    return 0;
                } else {
                    return 0;
                }
            }
        }
    }
}

// FUNCTION: SHANDALAR 0x00496e1d
int Game_CardScript496e1d(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (kind == 0x73) {
        ih = 1;
        if ((g_game8c83f8 & 0x4) == 0) {
            ih = 0;
        }
        if ((g_gameSlots[a][b].flags08 & 0x20010) != 0) {
            ih = 0;
        }
        if (ih != 0) {
            if (Game_Helper4be650(0, 0, a, 2, 2, 0x200, 0, 0, 0, 0, 0, 0, g_game93a7d4, -1, -1, -1, 0, 0, 0) != 0) {
                ih = 0x63;
            } else {
                ih = 0;
            }
        }
        return ih;
    } else {
        if (kind == 0x90) {
            Game_Helper4c4263(0);
            return 0;
        } else {
            if (kind == 0x6d) {
                if (g_game8bd200 != 1) {
                    Game_Helper56ca10(0x586fa4, 0x586f94);
                }
                if (Game_Helper4c0efa(a, 2, 2, 0x200, 0, 0, 0, 0, 0, 0, g_game93a7d4, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &ig) != 0) {
                    g_gameSlots[a][b].field74 = ig;
                    g_gameSlots[a][b].field78 = iv;
                    g_gameSlots[a][b].field36 = 1;
                    g_gameSlots[a][b].flags08 |= 0x10;
                } else {
                    g_game8c83ec = 1;
                }
                return 0;
            } else {
                if (kind == 0x72) {
                    ig = g_gameSlots[a][b].field74;
                    iv = g_gameSlots[a][b].field78;
                    if (Game_Helper4bea85(ig, iv, 0, a, 2, 2, 0x200, 0, 0, 0, 0, 0, 0, g_game93a7d4, -1, -1, -1, 0, 0, 0) != 0) {
                        if (g_gameSlots[ig][iv].field38 != 0) {
                            g_gameSlots[ig][iv].field38--;
                        }
                    } else {
                        g_game8c83ec = 1;
                    }
                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
                    return 0;
                } else {
                    if (kind == 0x3b && (g_gameSlots[a][b].flags08 & 0x20010) == 0) {
                        g_game7beb18[a]++;
                    }
                    return 0;
                }
            }
        }
    }
}

// FUNCTION: SHANDALAR 0x00497640
int Game_CardScript497640(int a, int b, int kind)
{
    if (kind == 0x73 || kind == 0x6d || kind == 0x72) {
        return Game_Helper4909b0(a, b, kind, 1, 1);
    } else {
        return 0;
    }
}

// FUNCTION: SHANDALAR 0x00497ec5
int Game_CardScript497ec5(int a, int b, int kind)
{
    if (kind == 0x6c && g_game951bdc == b && g_game8e1aa8 == a) {
        g_gameSlots[a][b].flags08 |= 0x10;
        return 0;
    } else {
        if (kind == 0x82 && g_game951bdc == b && g_game8e1aa8 == a) {
            g_gameSlots[a][b].field11c &= ~3;
            return 0;
        } else {
            if (kind == 0x84 && g_game951bdc == b && g_game8e1aa8 == a && (g_gameSlots[a][b].flags08 & 0x10) != 0 && a == g_game7a82e0 && a == g_game77c57c) {
                g_gameSlots[a][b].field5c |= 0x10;
                return 0;
            } else {
                if (kind == 0x88) {
                    if (g_game77ed80[a][Game_Helper4ba309(a, b, 2)] < 2) {
                        g_game7a01e0 |= 1;
                    }
                    return 0;
                } else {
                    if (kind == 1) {
                        if (Game_Helper498365(a, b, 1) != 0) {
                            g_gameSlots[a][b].flags08 &= ~0x10;
                        } else {
                            g_game7a01e0 |= 1;
                        }
                        return 0;
                    } else {
                        if (kind == 0x79 && g_gameSlots[a][b].field4c == 0) {
                            if (g_game77ed80[a][Game_Helper4ba309(a, b, 2)] < 2) {
                                g_game7a01e0 = 1;
                            }
                            return 0;
                        } else {
                            if (g_game94d178 == 0xdc && g_game94d464 == 0x15) {
                                if (g_game951bdc == b && g_game8e1aa8 == a && g_game7a82e0 == g_game8c8afc && g_gameSlots[a][b].field4c == 0 && g_game7bf4b8 == a && g_game8e2fc4 == b) {
                                    if (g_game77ed80[a][Game_Helper4ba309(a, b, 2)] >= 2) {
                                        if (kind == 0x7d) {
                                            g_game7a01e0 |= 2;
                                        }
                                        if (kind == 0x7e) {
                                            if (Game_Helper498365(a, b, 0) == 0) {
                                                g_game7a01dc = 1;
                                                g_game8c83ec = 0;
                                            } else {
                                                g_gameSlots[a][b].field4c = 1;
                                            }
                                        }
                                    } else {
                                        g_game7a01dc = 1;
                                    }
                                }
                                return 0;
                            } else {
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

// FUNCTION: SHANDALAR 0x0049da3f
int Game_CardScript49da3f(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;
    int ii;
    int ij;

    if (kind == 0x3c) {
        g_gameSlots[a][b].field1f = (signed char)g_gameSlots[a][b].field1f | 0x3f;
        return 0;
    } else {
        if (kind == 0x1a) {
            ii = 1 - a;
            if (g_game7a82e0 == a) {
                if ((g_gameSlots[a][b].flags08 & 0x44) != 0) {
                    if ((signed char)g_gameSlots[a][b].field24 == -1) {
                        ij = b;
                    } else {
                        ij = (signed char)g_gameSlots[a][b].field24;
                    }
                    ih = 0;
                    for (; g_game78e970[ii] > ih; ih++) {
                        if ((signed char)g_gameSlots[ii][ih].field24 == ij) {
                            iv = g_gameSlots[ii][ih].field6c;
                            if (iv != -1) {
                                if ((signed char)g_gameCards[iv].field29 != 0 && (g_gameCards[iv].flags & 0x2) != 0) {
                                    Game_Helper4a9910(a, b, g_game8c72b4, ii, ih);
                                }
                            }
                        }
                    }
                }
            }
            if (g_game7a82e0 != a && (signed char)g_gameSlots[a][b].field24 != -1) {
                ig = (signed char)g_gameSlots[ii][(signed char)g_gameSlots[a][b].field24].field24;
                if (ig == -1) {
                    iv = g_gameSlots[ii][(signed char)g_gameSlots[a][b].field24].field6c;
                    if (iv != -1) {
                        if ((signed char)g_gameCards[iv].field29 != 0 && (g_gameCards[iv].flags & 0x2) != 0) {
                            Game_Helper4a9910(a, b, g_game8c72b4, ii, (signed char)g_gameSlots[a][b].field24);
                        }
                    }
                } else {
                    ih = 0;
                    for (; g_game78e970[ii] > ih; ih++) {
                        if (Game_Helper411d90(ii, ih) != 0) {
                            if ((signed char)g_gameSlots[ii][ih].field24 == ig) {
                                iv = g_gameSlots[ii][ih].field6c;
                                if ((signed char)g_gameCards[iv].field29 != 0) {
                                    if ((g_gameCards[iv].flags & 0x2) != 0) {
                                        Game_Helper4a9910(a, b, g_game8c72b4, ii, ih);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        return 0;
    }
}
