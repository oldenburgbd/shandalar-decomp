/* Shandalar game layer: card behaviour scripts at 0x52xxxx.
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

// FUNCTION: SHANDALAR 0x0052160a
int Game_CardScript52160a(int a, int b, int kind)
{
    if (kind == 0x6c
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        g_game7be740[a] |= 2;
    }
    if (kind == 0x1f
        && a == g_game7a82e0) {
        if ((g_gameSlots[a][b].flags08 & 0x10) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) != 0) {
            g_game7a01e0++;
        }
    }
    if (kind == 0x77
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        g_game7be740[a] &= ~2;
    }
    if (g_game94d178 == 0xd4
        && b == g_game951bdc
        && a == g_game8e1aa8
        && a == g_game7bf4b8
        && b == g_game8e2fc4
        && g_game8c8afc == a) {
        if (kind == 0x7d) {
            g_game7a01e0 |= 2;
        }
        if (kind == 0x7e) {
            g_game7be740[a] &= ~2;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00521be8
int Game_CardScript521be8(int a, int b, int kind)
{
    if (kind == 0x7f
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        if ((g_gameSlots[a][b].flags08 & 0x10) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) != 0) {
            Game_Helper44147b(a, Game_Helper4ba392(a, b, 5), Game_Helper4ba392(a, b, 4));
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00521791
int Game_CardScript521791(int a, int b, int kind)
{
    if (kind == 0x73) {
        if (Game_Helper441699(a, 7, 3) != 0) {
            if (((g_gameSlots[a][b].flags08 & 0x30000) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0)
                && (g_gameSlots[a][b].flags08 & 0x10) == 0) {
                return 1;
            }
        }
        return 0;
    }
    if (kind == 0x6d) {
        Game_Helper4143e0(a, 0, 3);
        if (g_game8c83ec != 1) {
            g_gameSlots[a][b].flags08 |= 0x10;
        }
    }
    if (kind == 0x72) {
        Game_Helper5218f3(a, b);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00520c71
int Game_CardScript520c71(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x73) {
        if (g_gameSlots[a][b].field38 == 0
            && Game_Helper441699(a, 7, 5) != 0) {
            if (((g_gameSlots[a][b].flags08 & 0x30000) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0)
                && (g_gameSlots[a][b].flags08 & 0x10) == 0) {
                if (a == g_game8c7468
                    && (g_game93a934 & 0x2) == 0
                    && g_game94d45c > 0) {
                    g_game8c7420 |= 3;
                }
                return 1;
            }
        }
        return 0;
    }
    if (kind == 0x6d
        && Game_Helper441699(a, 7, 5) != 0) {
        Game_Helper4143e0(a, 0, 5);
        if (g_game8c83ec != 1) {
            g_gameSlots[a][b].flags08 |= 0x10;
            g_gameSlots[a][b].field38 |= 1;
            if (g_game94d45c > 0) {
                g_game94d45c--;
            }
        }
    }
    if (kind == 0x72) {
        v8 = Game_Helper56c705(0x375);
        v4 = Game_FindFreeSlot(a, v8);
        if (v4 != -1) {
            Game_Helper4d1c03(a, v4);
            g_gameSlots[a][v4].flags18 |= 0x10;
        }
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 = 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00520787
int Game_CardScript520787(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_game94d63c += 0x18;
    }
    if (kind == 0x73) {
        if (((g_gameSlots[a][b].flags08 & 0x30000) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0)
            && (g_gameSlots[a][b].flags08 & 0x10) == 0
            && Game_Helper441699(a, 7, 2) != 0
            && Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, 0x2002, -1, 0, 0, 0) != 0) {
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
        && Game_Helper441699(a, 7, 2) != 0) {
        Game_Helper4143e0(a, 0, 2);
        if (g_game8c83ec != 1) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58d2e4, 0x58d2d8);
            }
            if (Game_Helper4c0efa(a, 2, a, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, 0x2002, -1, 0, 0, 0, 0x8aa920, 1, &v8) != 0) {
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
        if (Game_Helper4bea85(v8, v4, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, 0x2002, -1, 0, 0, 0) != 0) {
            Game_Helper4a9910(g_game94f280, g_game930f5c, g_game7bfe7c, v8, v4);
        } else {
            g_game8c83ec = 1;
        }
    }
    if (kind == 0xc7) {
        if (a == g_game8c7468) {
            g_game94d63c += 0x18;
            return 0;
        }
        g_game94d63c -= 0x18;
    }
    return 0;
}
