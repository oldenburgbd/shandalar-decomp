/* Shandalar game layer: card behaviour scripts at 0x55xxxx.
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

// FUNCTION: SHANDALAR 0x005553ee
int Game_CardScript5553ee(int a, int b, int kind)
{
    if (kind == 1) {
        Game_Helper42d790(a, b, kind, 0);
    } else {
        if (kind == 0x71) {
            Game_Helper42d790(a, b, kind, 0);
        } else {
            if (kind == 0x73) {
                if ((g_gameSlots[a][b].flags08 & 0x10) == 0) {
                    if ((g_gameSlots[a][b].flags08 & 0x30000) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0) {
                        return 1;
                    }
                }
                return 0;
            }
            if (kind == 0x6d) {
                Game_Helper441651(a, 0, 1);
                g_gameSlots[a][b].flags08 |= 0x10;
                g_game93a794 = 0;
                if (Game_Helper4882b5(a) == 7) {
                    Game_Helper441651(a, 0, 1);
                }
            }
            if (kind == 0x7f
                && g_game951bdc == b
                && g_game8e1aa8 == a) {
                if ((g_gameSlots[a][b].flags08 & 0x10) == 0) {
                    if ((g_gameSlots[a][b].flags08 & 0x30000) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0) {
                        Game_Helper441539(a, 0, 1);
                    }
                }
                if (Game_Helper4882b5(a) == 7) {
                    Game_Helper441539(a, 0, 1);
                }
            }
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00555699
int Game_CardScript555699(int a, int b, int kind)
{
    if (kind == 1) {
        Game_Helper42d790(a, b, kind, 0);
    } else {
        if (kind == 0x71) {
            Game_Helper42d790(a, b, kind, 0);
        } else {
            if (kind == 0x73) {
                if ((g_gameSlots[a][b].flags08 & 0x10) == 0) {
                    if ((g_gameSlots[a][b].flags08 & 0x30000) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0) {
                        return 1;
                    }
                }
                return 0;
            }
            if (kind == 0x6d) {
                Game_Helper441651(a, 0, 1);
                g_gameSlots[a][b].flags08 |= 0x10;
                g_game93a794 = 0;
                if (Game_Helper4882b5(a) == 7) {
                    Game_Helper441651(a, 0, 2);
                }
            }
            if (kind == 0x7f
                && g_game951bdc == b
                && g_game8e1aa8 == a) {
                if ((g_gameSlots[a][b].flags08 & 0x10) == 0) {
                    if ((g_gameSlots[a][b].flags08 & 0x30000) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0) {
                        Game_Helper441539(a, 0, 1);
                    }
                }
                if (Game_Helper4882b5(a) == 7) {
                    Game_Helper441539(a, 0, 2);
                }
            }
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x005530c4
int Game_CardScript5530c4(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;
    int ii;

    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && a == g_game8c7468) {
        g_game94d63c += 0x30;
    }
    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x10) == 0) {
            if ((g_gameSlots[a][b].flags08 & 0x30000) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0) {
                return 1;
            }
        }
        return 0;
    }
    if (kind == 0x6d) {
        if (a == g_game8c7468
            && (g_game93a934 & 0x2) == 0) {
            for (ii = 0, ih = 0, iv = 0; g_game78e970[a] > ii; ii++) {
                ig = g_gameSlots[a][ii].field6c;
                if (ig != -1) {
                    if ((g_gameSlots[a][ii].flags08 & 0x22) == 0
                        && (g_gameCards[ig].flags & 0x20) == 0) {
                        ih++;
                    }
                    if ((g_gameCards[ig].flags & 0x1) != 0) {
                        iv++;
                    }
                }
            }
            g_game94d63c -= -(0 - (7 - ih) * 24);
            if (iv < 3 || iv > 8) {
                g_game94d63c += 0x30;
            }
        }
        g_gameSlots[a][b].flags08 |= 0x10;
        g_game93a794 = -1;
    }
    if (kind == 0x72) {
        Game_Helper409a0b(a);
        Game_Helper409a0b(a);
        for (ii = 0; ii < 3; ii++) {
            if (g_game8e3ec8[a] > 0) {
                Game_Helper409ee8(a, 0, 0);
            }
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00555e68
int Game_CardScript555e68(int a, int b, int kind)
{
    if (kind == 0x6d) {
        Game_Helper56ca10(0x590e2c, 0x590e20);
    }
    Game_Helper555944(a, b, kind, (signed char)g_gameSlots[a][b].field58);
    return 0;
}

// FUNCTION: SHANDALAR 0x00555ed8
int Game_CardScript555ed8(int a, int b, int kind)
{
    if (kind == 0x6d) {
        Game_Helper56ca10(0x590e44, 0x590e3c);
    }
    Game_Helper555944(a, b, kind, (signed char)g_gameSlots[a][b].field58);
    return 0;
}

// FUNCTION: SHANDALAR 0x00555f48
int Game_CardScript555f48(int a, int b, int kind)
{
    if (kind == 0x6d) {
        Game_Helper56ca10(0x590e5c, 0x590e54);
    }
    Game_Helper555944(a, b, kind, (signed char)g_gameSlots[a][b].field58);
    return 0;
}

// FUNCTION: SHANDALAR 0x00555fb8
int Game_CardScript555fb8(int a, int b, int kind)
{
    if (kind == 0x6d) {
        Game_Helper56ca10(0x590e78, 0x590e6c);
    }
    Game_Helper555944(a, b, kind, (signed char)g_gameSlots[a][b].field58);
    return 0;
}

// FUNCTION: SHANDALAR 0x00556028
int Game_CardScript556028(int a, int b, int kind)
{
    if (kind == 0x6d) {
        Game_Helper56ca10(0x590e94, 0x590e88);
    }
    Game_Helper555944(a, b, kind, (signed char)g_gameSlots[a][b].field58);
    return 0;
}

// FUNCTION: SHANDALAR 0x00556098
int Game_CardScript556098(int a, int b, int kind)
{
    if (kind == 0x6d) {
        Game_Helper56ca10(0x590eac, 0x590ea4);
    }
    Game_Helper555944(a, b, kind, (signed char)g_gameSlots[a][b].field58);
    return 0;
}

// FUNCTION: SHANDALAR 0x00556108
int Game_CardScript556108(int a, int b, int kind)
{
    if (kind == 0x6d) {
        Game_Helper56ca10(0x590ecc, 0x590ebc);
    }
    Game_Helper555944(a, b, kind, (signed char)g_gameSlots[a][b].field58);
    return 0;
}

// FUNCTION: SHANDALAR 0x00556178
int Game_CardScript556178(int a, int b, int kind)
{
    if (kind == 0x6d) {
        Game_Helper56ca10(0x590ee4, 0x590edc);
    }
    Game_Helper555944(a, b, kind, (signed char)g_gameSlots[a][b].field58);
    return 0;
}

// FUNCTION: SHANDALAR 0x005561e8
int Game_CardScript5561e8(int a, int b, int kind)
{
    if (kind == 0x6d) {
        Game_Helper56ca10(0x590f04, 0x590ef4);
    }
    Game_Helper555944(a, b, kind, (signed char)g_gameSlots[a][b].field58);
    return 0;
}

// FUNCTION: SHANDALAR 0x00556258
int Game_CardScript556258(int a, int b, int kind)
{
    if (kind == 0x6d) {
        Game_Helper56ca10(0x590f24, 0x590f14);
    }
    Game_Helper555944(a, b, kind, (signed char)g_gameSlots[a][b].field58);
    return 0;
}
