/* Card behaviour scripts, cards43 batch (0x401000-0x45c62c).
 *
 * Layout follows the archived attempt's cards43 unit, whose per-function
 * reccmp scores were reproduced exactly; its function order, local variable
 * names and expression shapes are the measured-good formulation and are kept
 * verbatim (MSVC 4.2 /Od assigns stack slots by identifier name, and
 * re-rolls codegen across a whole translation unit when anything changes).
 *
 * Every script has the shape int f(int a, int b, int kind), where (a, b)
 * address g_gameSlots and kind selects the event. Scripts reach the slot
 * they were invoked on through CUR (two globals), not through (a, b).
 *
 * Address literals passed as plain ints (e.g. 0x583c68) are pointers into
 * the original image's data (string constants, an out-buffer at 0x8aa920);
 * pushing the literal reproduces the original instruction without carrying
 * the copyrighted data. Locals read before any write are deliberate too --
 * the original picked up whatever the helper left in that stack slot.
 */

#include "game/gameslot.h"


// FUNCTION: SHANDALAR 0x00433ca1
int Game_CardScript433ca1(int a, int b, int kind)
{
    if (kind == 0x79
        && a == g_game7a82e0
        && (g_gameCards[CUR.field6c].flags & 0x42) != 0x42
        && g_game8e44b8[g_gameCards[CUR.field6c].mask][0] != 0x70) {
        g_game7a01e0++;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00434189
int Game_CardScript434189(int a, int b, int kind)
{
    if (kind == 0x78
        && g_game8e26b0 == b
        && g_game94e928 == a
        && g_gameCards[CUR.field6c].field29 == 0) {
        g_game7a01e0 = 1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0043b476
int Game_CardScript43b476(int a, int b, int kind)
{
    if (g_game951bdc == b
        && g_game8e1aa8 == a
        && g_game77ed80[1 - a][Game_Helper4ba309(a, b, 2)] > 0
        && kind == 0x33) {
        g_game7a01e0++;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0043d4e3
int Game_CardScript43d4e3(int a, int b, int kind)
{
    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x79 && (CUR.field28 & 0x20) == 0) {
        g_game7a01e0++;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0043f46e
int Game_CardScript43f46e(int a, int b, int kind)
{
    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x71) {
        g_gameSlots[a][b].field38 = Game_Helper4ba309(a, b, 3);
        Game_Helper488b70(a, b, (int)Game_Callback440b07, -1);
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0044e945
int Game_CardScript44e945(int a, int b, int kind)
{
    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x71) {
        g_gameSlots[a][b].field38 = Game_Helper4ba309(a, b, 5);
        Game_Helper488b70(a, b, (int)Game_Callback440b07, -1);
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

/* The helper's return indexes the other side's row of g_game77ed80; the row
 * is 1 - a both in the lookup and in the first argument. */
// FUNCTION: SHANDALAR 0x00440704
int Game_CardScript440704(int a, int b, int kind)
{
    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x71) {
        Game_Helper4bab38(1 - a,
                          g_game77ed80[1 - a][Game_Helper4ba309(a, b, 2)],
                          a, b);
        g_gameSlots[a][b].field36 = 0;
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00432b51
int Game_CardScript432b51(int a, int b, int kind)
{
    if (kind == 0x6e
        && g_gameSlots[g_game8e1aa8][g_game951bdc].field6c == g_game93a7d4) {
        if (g_gameSlots[g_game8e1aa8][g_game951bdc].field04 == -1
            && (signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field0c == a
            && g_gameSlots[g_game8e1aa8][g_game951bdc].field48 == b
            && g_gameSlots[g_game8e1aa8][g_game951bdc].field38 != 0) {
            g_gameSlots[a][b].field0c = g_game8e1aa8;
            g_gameSlots[a][b].field48 = g_game951bdc;
        }
        return 0;
    }
    if (g_game94d178 == 0xd7
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && (signed char)g_gameSlots[a][b].field0c != -1
        && a == g_game8c8afc) {
        if (kind == 0x7d) {
            g_game7a01e0 |= 2;
        }
        if (kind == 0x7e) {
            g_game7bfee0[(signed char)g_gameSlots[a][b].field0c]++;
            g_gameSlots[a][b].field0c = 0xff;
            Game_Helper403eeb();
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00440388
int Game_CardScript440388(int a, int b, int kind)
{
    int v4;

    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x71) {
        Game_Helper4bab38(1 - a, 2, a, b);
        v4 = Game_FindFreeSlot(a, g_game92f3cc);
        if (v4 != -1) {
            g_gameSlots[a][v4].field3c = g_gameSlots[a][b].field6c;
            g_gameSlots[a][v4].flags08 |= 2;
            g_gameSlots[a][v4].field64 = 0x325;
            g_gameSlots[a][v4].field14 = 0xd7;
            Game_Helper4111b1(a, v4);
            g_gameSlots[a][v4].field0c = a;
            g_gameSlots[a][v4].field48 = b;
        }
        Game_Helper46d8c1(a, b, 1);
    }
    if (kind == 0x6e
        && g_gameSlots[g_game8e1aa8][g_game951bdc].field6c == g_game93a7d4
        && g_gameSlots[g_game8e1aa8][g_game951bdc].field04 == -1
        && (signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field0c == a
        && g_gameSlots[g_game8e1aa8][g_game951bdc].field48 == b
        && g_gameSlots[g_game8e1aa8][g_game951bdc].field38 != 0) {
        g_gameSlots[a][b].field0c = g_game8e1aa8;
        g_gameSlots[a][b].field48 = g_game951bdc;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00432260
int Game_CardScript432260(int a, int b, int kind)
{
    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x10) == 0
            && Game_Helper441699(a, 7, 2) != 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x6d) {
        Game_Helper4143e0(a, 0, 2);
        if (g_game8c83ec != 1) {
            g_gameSlots[a][b].flags08 |= 0x10;
        }
        return 0;
    }
    if (kind == 0x72) {
        Game_Helper488b70(g_game94f280, g_game930f5c, (int)Game_Callback4323cc, -1);
        return 0;
    }
    if (kind == 0x3b
        && (g_gameSlots[a][b].flags08 & 0x20010) == 0
        && Game_Helper441699(a, 7, 2) != 0) {
        g_game7beb10[1 - a]--;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004382fd
int Game_CardScript4382fd(int a, int b, int kind)
{
    int v4;

    if (kind == 0x71) {
        v4 = Game_Helper4a9910(a, b, g_game951968, a, b);
        if (v4 != -1) {
            g_gameSlots[a][v4].field38 = 3;
            g_gameSlots[a][v4].field4c = 0x109;
            g_gameSlots[a][v4].flags18 = 0x10020;
            g_gameSlots[a][v4].flags18 |= 0x2000000;
            g_gameSlots[a][b].field0c = a;
            g_gameSlots[a][b].field48 = v4;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00456897
int Game_CardScript456897(int a, int b, int kind)
{
    if ((kind == 0x33 || kind == 0x32)
        && Game_Helper411d90(a, b) != 0) {
        if (((g_gameSlots[a][b].flags08 & 0x10) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) != 0)
            && ((1 << Game_Helper4ba392(a, b, 4)) & 0x15) != 0) {
            g_game7a01e0++;
        }
    }
    if (kind == 0x81
        && Game_Helper488147(g_game8e1aa8, g_game951bdc, Game_Helper4ba309(a, b, 4)) != 0
        && Game_Helper411d90(a, b) != 0) {
        if (((g_gameSlots[a][b].flags08 & 0x10) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) != 0)
            && g_game93a794 != -1) {
            Game_Helper4415c5(g_game8e1aa8, 4, 1);
        }
    }
    if (kind == 0x7f
        && Game_Helper488147(g_game8e1aa8, g_game951bdc, Game_Helper4ba309(a, b, 4)) != 0
        && Game_Helper411d90(a, b) != 0) {
        if ((g_gameSlots[a][b].flags08 & 0x10) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) != 0) {
            Game_Helper441539(g_game8e1aa8, 4, 1);
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004584e4
int Game_CardScript4584e4(int a, int b, int kind)
{
    if (kind == 0x6c
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        g_game94d63c += g_game8e3ed0[a] - g_game8e3ec0[5 - a] * 12;
    }
    if (kind == 0x32) {
        if (((g_gameSlots[a][b].flags08 & 0x10) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) != 0)
            && (g_gameSlots[g_game8e1aa8][g_game951bdc].flags08 & 0x4) != 0
            && g_game8e1aa8 == g_game7a82e0
            && Game_Helper411d90(a, b) != 0) {
            g_game7a01e0++;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00435f5f
int Game_CardScript435f5f(int a, int b, int kind)
{
    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x20010) == 0
            && Game_Helper441699(a, 3, 1) != 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x6d) {
        Game_Helper4143e0(a, 3, 1);
        if (g_game8c83ec != 1) {
            Game_Helper441651(a, 4, 1);
            g_gameSlots[a][b].flags08 |= 0x10;
            g_game93a794 = 4;
        }
        return 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00436525
int Game_CardScript436525(int a, int b, int kind)
{
    if (kind == 0x79
        && g_gameSlots[a][b].field38 != 0) {
        g_game7a01e0 = 1;
    }
    if (kind == 0x22
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && g_game7a82e0 == a) {
        if ((g_gameSlots[a][b].flags08 & 0x40) != 0) {
            g_gameSlots[a][b].field38 = 1;
            return 0;
        }
        g_gameSlots[a][b].field38 = 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004369d5
int Game_CardScript4369d5(int a, int b, int kind)
{
    if (kind == 0x3c
        && (g_game8c83fa & 0x2) == 0
        && Game_Helper411d90(a, b) != 0
        && g_gameSlots[a][b].field38 == 0
        && Game_Helper411d90(g_game8e1aa8, g_game951bdc) != 0
        && a == g_game8e1aa8
        && (signed char)g_gameCards[g_game7a01e0].field29 == 4) {
        g_gameSlots[a][b].field38 = 1;
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0043d2cc
int Game_CardScript43d2cc(int a, int b, int kind)
{
    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x81
        && g_game8e1aa8 != a
        && Game_Helper488147(g_game8e1aa8, g_game951bdc, Game_Helper4ba309(a, b, 4)) != 0) {
        Game_Helper4883aa(a, 1, a, b);
        return 0;
    }
    if ((kind == 0x6c || kind == 0xc7)
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_game94d63c += g_game77ed80[g_game8c776c][Game_Helper4ba309(a, b, 4)] + g_game77ed80[g_game8c776c][Game_Helper4ba309(a, b, 4)] * 2 + 3 * 8;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0043d3be
int Game_CardScript43d3be(int a, int b, int kind)
{
    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x34
        && Game_Helper411d90(a, b) != 0
        && Game_Helper411d90(g_game8e1aa8, g_game951bdc) != 0
        && (g_gameCards[g_gameSlots[g_game8e1aa8][g_game951bdc].field6c].flags & 0x2) != 0
        && ((1 << Game_Helper4ba392(a, b, 3)) & 0x15) != 0) {
        g_game7a01e0 |= 1 << Game_Helper4ba309(a, b, 3) - 1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0045c62c
int Game_CardScript45c62c(int a, int b, int kind)
{
    if (kind == 0x32) {
        if (((g_gameSlots[a][b].flags08 & 0x10) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) != 0)
            && (g_gameSlots[g_game8e1aa8][g_game951bdc].flags08 & 0x4) != 0
            && g_game8e1aa8 == g_game7a82e0
            && Game_Helper411d90(a, b) != 0) {
            g_game7a01e0--;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00401611
int Game_CardScript401611(int a, int b, int kind)
{
    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x71) {
        Game_Helper485308(a);
        Game_Helper485308(1 - a);
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

/* Acts on the slot this one is linked to: row from field50, column from
 * field04, guarded by the link matching the current slot. */
// FUNCTION: SHANDALAR 0x0041a76b
int Game_CardScript41a76b(int a, int b, int kind)
{
    if (kind == 0x82
        && g_gameSlots[a][b].field04 == g_game951bdc
        && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8
        && g_game951bdc != -1) {
        g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field11c &= ~3;
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0042d548
int Game_CardScript42d548(int a, int b, int kind)
{
    if (kind == 0x7f
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        if (((g_gameSlots[a][b].flags08 & 0x30000) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0)
            && (g_gameSlots[a][b].flags08 & 0x10) == 0) {
            Game_Helper441539(a, 6, 3);
        }
        return 0;
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
        Game_Helper441651(a, 6, 3);
        g_gameSlots[a][b].flags08 |= 0x10;
        g_game93a794 = 6;
        return 0;
    }
    if (kind == 0x71
        && g_game8bd200 != 1) {
        Game_Helper40ebb9(8);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00434213
int Game_CardScript434213(int a, int b, int kind)
{
    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x30000) == 0
            && Game_Helper441699(a, 7, 3) != 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x6d) {
        Game_Helper4143e0(a, 0, 3);
        if (g_game8c83ec != 1) {
            Game_Helper441651(a, 4, 3);
            g_game93a794 = 4;
        }
    }
    if (kind == 0x72) {
        if (g_game8bd200 != 1) {
            Game_Helper40ebb9(0xf);
        }
        Game_Helper46d8c1(g_game94f280, g_game930f5c, 3);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0043469a
int Game_CardScript43469a(int a, int b, int kind)
{
    if (kind == 0x78
        && g_game8e26b0 == b
        && g_game94e928 == a) {
        if (((1 << Game_Helper4ba392(a, b, 4)) & 0x15) != 0) {
            g_game7a01e0 = 1;
        }
        return 0;
    }
    if (kind == 0x87) {
        if (g_game77ed80[a][Game_Helper4ba309(a, b, 2)] > 0) {
            g_game7a01e0 |= 1;
        }
        return 0;
    }
    if (kind == 0x85
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && g_gameSlots[a][b].field38 == 0
        && a == g_game7a82e0
        && a == g_game77c57c) {
        g_gameSlots[a][b].field5c |= 0x101;
    }
    if (kind == 4
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_gameSlots[a][b].field38++;
        if (g_game77ed80[a][Game_Helper4ba309(a, b, 2)] > 0) {
            Game_Helper440d44(a, b, 1, Game_Helper4ba309(a, b, 2), 0);
            return 0;
        }
        g_game7a01e0 |= 1;
        return 0;
    }
    if (kind == 0x86) {
        Game_Helper56ca10(0x583430, 0x583424);
        Game_Helper558538(a, a, b, -1, -1, 0x8aa920 + 0x12c, 0);
        Game_Helper4bab38(a, 6, g_game94f280, g_game930f5c);
        Game_Helper46d8c1(g_game94f280, g_game930f5c, 1);
        return 0;
    }
    if (kind == 0xc7
        && g_game77ed80[a][Game_Helper4ba309(a, b, 2)] == 0) {
        Game_Helper4bab38(a, 6, a, b);
        Game_Helper46d8c1(a, b, 1);
    }
    if ((kind == 0x22 || kind == 0xc7)
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_gameSlots[a][b].field38 = 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00434c40
int Game_CardScript434c40(int a, int b, int kind)
{
    int v4;

    if (kind == 0x73) {
        if (Game_Helper441699(a, 3, 2) != 0
            && (g_gameSlots[a][b].field38 & 0x100) == 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x6d) {
        Game_Helper4143e0(a, 3, 2);
        if (g_game8c83ec != 1) {
            g_gameSlots[a][b].field74 = a;
            g_gameSlots[a][b].field78 = b;
            g_gameSlots[a][b].field36 = 1;
        }
        return 0;
    }
    if (kind == 0x72) {
        if (g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field6c != -1
            && (g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 & 0x100) == 0) {
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 |= 0x100;
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
            v4 = Game_Helper4a9910(g_game94f280, g_game930f5c, g_game8aa734, g_game94f280, g_game930f5c);
            if (v4 != -1) {
                g_gameSlots[a][v4].field38 = 0x100;
                g_gameSlots[a][v4].field28 = 0;
            }
        }
        return 0;
    }
    if (kind == 0x8c
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && Game_Helper441699(a, 3, 2) != 0) {
        g_game97db34 |= 0x100;
        return 0;
    }
    if (kind == 0x22 || kind == 0xc7) {
        g_gameSlots[a][b].field38 = 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004372e0
int Game_CardScript4372e0(int a, int b, int kind)
{
    int v4;

    v4 = 1 << Game_Helper4ba392(a, b, 4);
    if ((g_gameSlots[a][b].flags18 & 0x2000) == 0
        && (v4 & 0x13) == 0) {
        g_gameSlots[a][b].field1e = 1 << Game_Helper4ba392(a, b, 4);
        if (g_game8bd200 != 1) {
            Game_Helper40ebb9(0x27);
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0043d553
int Game_CardScript43d553(int a, int b, int kind)
{
    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x73) {
        if (g_game94d464 == 4
            && g_game7a82e0 == g_game77c57c
            && (g_gameSlots[a][b].field38 & 0x1) == 0
            && g_game77ed80[g_game7a82e0][Game_Helper4ba309(a, b, 5)] != 0) {
            g_gameSlots[a][b].field5c |= 0x101;
            g_game8c7420 |= 3;
            return 1;
        }
        return 0;
    }
    if (kind == 4
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        g_gameSlots[a][b].field38 |= 1;
        g_game7be9ac = 1;
        g_game7a01e0 |= 1;
        return 0;
    }
    if (kind == 0x86) {
        Game_Helper4883aa(g_game7a82e0, 1, g_game94f280, g_game930f5c);
        return 0;
    }
    if (kind == 0x22) {
        g_gameSlots[a][b].field38 &= ~1;
        return 0;
    }
    if (kind == 0xc7
        && g_game94d464 == 0x22
        && Game_Helper411d90(a, b) != 0
        && a == g_game8c7468
        && g_game77ed80[a][Game_Helper4ba309(a, b, 5)] != 0) {
        g_game94d63c += 0x18;
    }
    return 0;
}

/* Swaps field0e and field34 of the linked slot whenever field38 is armed. */
// FUNCTION: SHANDALAR 0x004407b3
int Game_CardScript4407b3(int a, int b, int kind)
{
    int v4;

    if (kind == 0x34
        && g_gameSlots[a][b].field04 == g_game951bdc
        && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8
        && g_game951bdc != -1) {
        g_gameSlots[a][b].field38++;
    }
    if (g_gameSlots[a][b].field38 != 0) {
        v4 = g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field0e;
        g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field0e = g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field34;
        g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field34 = v4;
        g_gameSlots[a][b].field38 = 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004481e7
int Game_CardScript4481e7(int a, int b, int kind)
{
    g_gameSlots[a][b].field58 = 1;
    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x71) {
        Game_Helper4a9910(a, b, g_game8bce34, -1, -1);
        Game_Helper441539(a, 0, 1);
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004547d0
int Game_CardScript4547d0(int a, int b, int kind)
{
    int v4;

    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x10) == 0
            && Game_Helper4be650(0, 0, a, a, a, 0x200, 2, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x6d) {
        Game_Helper56ca10(0x58550c, 0x5854fc);
        v4 = Game_Helper49fb64(a);
        if (v4 != -1) {
            Game_Helper46d8c1(a, v4, 3);
            Game_Helper441651(a, 0, 2);
            g_game93a794 = 0;
        } else {
            g_game8c83ec = 1;
        }
    }
    if (kind == 0x7f
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && (g_gameSlots[a][b].flags08 & 0x10) == 0) {
        if (((g_gameSlots[a][b].flags08 & 0x30000) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0)
            && Game_Helper4be650(0, 0, a, a, a, 0x200, 2, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
            Game_Helper441539(a, 0, 2);
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00455b59
int Game_CardScript455b59(int a, int b, int kind)
{
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_game94d63c += g_game8c83f0[a] - g_game8c83f0[1 - a] * 24;
    }
    if (kind == 0x73) {
        if (g_game94d464 == 4
            && (g_gameSlots[a][b].field38 & 0x1) == 0
            && g_game7a82e0 == g_game77c57c) {
            if ((g_gameSlots[a][b].flags08 & 0x10) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) != 0) {
                g_gameSlots[a][b].field5c |= 0x101;
                g_game8c7420 |= 3;
                return 1;
            }
        }
        return 0;
    }
    if (kind == 4
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_gameSlots[a][b].field38 |= 1;
        g_game7be9ac = 1;
        g_game7a01e0 |= 1;
    }
    if (kind == 0x86) {
        Game_Helper4bab38(g_game7a82e0, 1, g_game94f280, g_game930f5c);
    }
    if (kind == 0x22) {
        g_gameSlots[a][b].field38 &= ~1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0045a8d4
int Game_CardScript45a8d4(int a, int b, int kind)
{
    if (kind == 0x77
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && (g_gameSlots[a][b].flags08 & 0x20) == 0
        && (signed char)g_gameSlots[a][b].field68 != 4) {
        Game_Helper4415c5(a, 0, 4);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0045a98b
int Game_CardScript45a98b(int a, int b, int kind)
{
    int v4;

    if (kind == 0x77
        && (g_gameSlots[a][b].flags08 & 0x20) == 0
        && (g_gameSlots[g_game8e1aa8][g_game951bdc].flags08 & 0x20) == 0
        && g_game8e1aa8 == a) {
        if (g_gameSlots[g_game8e1aa8][g_game951bdc].field6c != -1) {
            v4 = g_gameSlots[g_game8e1aa8][g_game951bdc].field6c;
        } else {
            v4 = g_gameSlots[g_game8e1aa8][g_game951bdc].field3c;
        }
        if ((g_gameCards[v4].flags & 0x40) != 0
            && (signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field68 != 0
            && (signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field68 != 4) {
            if ((g_gameSlots[a][b].flags08 & 0x10) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) != 0) {
                if ((g_gameSlots[a][b].field38 & 0x100) != 0) {
                    g_gameSlots[a][b].field38 = 1;
                } else {
                    g_gameSlots[a][b].field38++;
                }
            }
        }
    }
    if (g_game94d178 == 0xd5
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && g_gameSlots[a][b].field38 != 0
        && a == g_game8c8afc) {
        if ((g_gameSlots[a][b].flags08 & 0x10) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) != 0) {
            g_gameSlots[a][b].field38 |= 0x100;
            if (kind == 0x7d
                && Game_Helper441699(a, 7, 1) != 0) {
                if (g_game8c7468 == a
                    && (g_game93a934 & 0x2) == 0) {
                    g_game7a01e0 |= 2;
                } else {
                    g_game7a01e0 |= 1;
                }
            }
            if (kind == 0x7e) {
                Game_LogEvent(a, b, 0x72, 0, 0);
                Game_Helper4143e0(a, 0, 1);
                Game_Helper41040d();
                if (g_game8c83ec != 1) {
                    Game_Helper40e158(a, b, 0x91);
                    Game_Helper4883aa(a, 1, a, b);
                    g_gameSlots[a][b].field38--;
                } else {
                    g_game8c83ec = -1;
                }
            }
            if (g_gameSlots[a][b].field38 != 0) {
                g_gameSlots[a][b].flags08 &= ~0x100;
            }
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00432455
int Game_CardScript432455(int a, int b, int kind)
{
    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x73) {
        if (a == g_game8c7468
            && (g_game93a934 & 0x2) == 0
            && g_game8c83f0[a] == 2) {
            return 0;
        }
        if (Game_Helper4419ca(a, b, 7, 2) != 0
            && g_game8c83f0[a] >= 2) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x6d) {
        Game_Helper417301(a, b, 0, 2);
        if (g_game8c83ec != 1) {
            g_game8c83f0[a] -= 2;
            g_game94d63c -= -(0 - (g_game8e3ec8[a] + g_game8e3ec8[a] - 4 - g_game8c83f0[a] + (g_game8e3ec8[a] + g_game8e3ec8[a] - 4 - g_game8c83f0[a]) * 2 + 0x1e) * 4);
        }
        return 0;
    }
    if (kind == 0x72) {
        Game_Helper409a0b(a);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00432575
int Game_CardScript432575(int a, int b, int kind)
{
    int v4;

    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && g_game8c7468 == a
        && (g_game93a934 & 0x2) == 0) {
        v4 = Game_Helper44aa1f(a, g_gameSlots[a][b].field6c, a);
        if (v4 != 0
            && g_game77ed80[g_game8c7468][7] / v4 >= 4) {
            g_game94d63c += 0x30;
        }
    }
    if (kind == 0x73) {
        if (Game_Helper441699(a, 7, 2) != 0) {
            if (((g_gameSlots[a][b].flags08 & 0x30000) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0)
                && (g_gameSlots[a][b].flags08 & 0x10) == 0) {
                return 1;
            }
        }
        return 0;
    }
    if (kind == 0x6d) {
        Game_Helper4143e0(a, 0, 2);
        if (g_game8c83ec != 1) {
            g_gameSlots[a][b].flags08 |= 0x10;
        }
    }
    if (kind == 0x72) {
        Game_Helper4883aa(g_game94f280, 1, g_game94f280, g_game930f5c);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00433d6f
int Game_CardScript433d6f(int a, int b, int kind)
{
    if (kind == 0x73) {
        if (g_game8c7468 == a
            && (g_game93a934 & 0x2) == 0
            && Game_Helper441699(a, 7, 1) == 0) {
            return 0;
        }
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
        Game_Helper4143e0(a, 0, -1);
        if (g_game8c83ec != 1) {
            g_gameSlots[a][b].field38 = g_game8c8274;
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x5833f4, 0x5833ec);
            }
            Game_Helper498e8a(a, b, g_gameSlots[a][b].field38 / 2);
            if (g_game8c83ec != 1) {
                g_gameSlots[a][b].flags08 |= 0x10;
            }
        }
        return 0;
    }
    if (kind == 0x72) {
        Game_Helper49914d(a, b, kind, g_gameSlots[a][b].field38 / 2);
        Game_Helper4bab38(a, (g_gameSlots[a][b].field38 + 1) / 2, g_game94f280, g_game930f5c);
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00434a1e
int Game_CardScript434a1e(int a, int b, int kind)
{
    if (kind == 0x7f
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        if ((g_gameSlots[a][b].flags08 & 0x20010) == 0) {
            Game_Helper441539(a, 1, 1);
        }
        return 0;
    }
    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x20010) == 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x6d) {
        Game_Helper441651(a, 1, 1);
        g_gameSlots[a][b].flags08 |= 0x10;
        g_game93a794 = 1;
        return 0;
    }
    if (kind == 0x81
        && b == g_game951bdc
        && a == g_game8e1aa8
        && g_game93a794 != -1) {
        Game_Helper4bab38(g_game8e1aa8, 1, a, b);
    }
    if (kind == 0x8a
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        g_game97db30 += 0x18 / (g_game77ed80[a][1] + 2);
        return 0;
    }
    if (kind == 0x8b
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        g_game97db30 -= -(0 - 0x60 / (g_game77ed80[a][1] + 2));
        return 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00435c34
int Game_CardScript435c34(int a, int b, int kind)
{
    int v4;

    if (kind == 1) {
        g_game94d660[a][0]++;
    }
    if (kind == 0x73) {
        if (g_gameSlots[a][b].field38 == 0
            && Game_Helper441699(a, 4, 1) != 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x6d) {
        Game_Helper4143e0(a, 4, 1);
        if (g_game8c83ec != 1) {
            g_gameSlots[a][b].field74 = a;
            g_gameSlots[a][b].field78 = b;
            g_gameSlots[a][b].field36 = 1;
            g_gameSlots[a][b].field38 = 1;
        } else {
            g_gameSlots[a][b].field38 = 0;
        }
    }
    if (kind == 0x72) {
        v4 = Game_Helper4a9910(g_game94f280, g_game930f5c, g_game8bcff8, g_game94f280, g_game930f5c);
        if (v4 != -1) {
            g_gameSlots[a][v4].field12 = 1;
        }
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
    }
    if (kind == 0x39) {
        Game_Helper4ecec6(Game_Helper441699(a, 4, 1), 0, 1);
    } else {
        if (kind == 0x22
            && b == g_game951bdc
            && a == g_game8e1aa8) {
            g_gameSlots[a][b].field38 = 0;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0043a9c9
int Game_CardScript43a9c9(int a, int b, int kind)
{
    int v4;

    if (g_game94d178 == 0xcb
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && g_gameSlots[a][b].field4c == 0) {
        g_gameSlots[a][b].field4c++;
        if (a == g_game7a82e0
            && a == g_game8c8afc) {
            g_gameSlots[a][b].field38 = 0;
            for (v4 = 0; g_game94d930[a][v4] != -1; v4++) {
                if ((g_gameCards[g_game94d930[a][v4]].flags & 0x2) != 0) {
                    g_gameSlots[a][b].field38++;
                }
            }
            g_gameSlots[a][b].field28 |= 0x2000000;
            Game_Helper40d7b2(a, b, 0x33, -1);
        }
        g_gameSlots[a][b].field4c = 0;
    }
    if (kind == 0x33
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_game7a01e0 += g_gameSlots[a][b].field38;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0043ca02
int Game_CardScript43ca02(int a, int b, int kind)
{
    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && Game_Helper44aa1f(a, g_gameSlots[a][b].field6c, -1) == 0) {
        g_game94d63c += (g_game8e3320[g_game8c7468][0] - g_game8e3320[g_game8c776c][0]) * 12;
    }
    if (kind == 0x71) {
        g_gameSlots[a][b].field38 = 4;
        return 0;
    }
    if (kind == 0x3c
        && (g_game8c83fa & 0x2) == 0
        && Game_Helper411d90(a, b) != 0
        && Game_Helper411d90(g_game8e1aa8, g_game951bdc) != 0
        && (g_gameCards[g_game7a01e0].flags & 0x1) != 0
        && (signed char)g_gameCards[g_game7a01e0].field29 != 0xd) {
        g_game7a01e0 = Game_Helper4ba309(a, b, g_gameSlots[a][b].field38) - 1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0043f348
int Game_CardScript43f348(int a, int b, int kind)
{
    int v4;

    if (kind == 0x74) {
        if (g_game7a82e0 != a
            && g_game94d464 >= 0xa
            && g_game94d464 <= 0x22) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x71) {
        for (v4 = 0; g_game78e970[a] > v4; v4++) {
            if (Game_Helper411d90(a, v4) != 0
                && (g_gameCards[g_gameSlots[a][v4].field6c].flags & 0x1) != 0) {
                g_gameSlots[a][v4].flags08 &= ~0x10;
            }
        }
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0043f9d9
int Game_CardScript43f9d9(int a, int b, int kind)
{
    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_gameSlots[a][b].field74 = 1 - a;
        g_gameSlots[a][b].field78 = -1;
        g_gameSlots[a][b].field36 = 1;
        return 0;
    }
    if (kind == 0x71) {
        Game_Helper4bab38(g_gameSlots[a][b].field74, g_game77ed80[a][Game_Helper4ba309(a, b, 4)], a, b);
        Game_Helper4bab38(a, (g_game77ed80[a][Game_Helper4ba309(a, b, 4)] + 1) / 2, a, b);
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00459be7
int Game_CardScript459be7(int a, int b, int kind)
{
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_game94d63c += (g_game77d180[g_game8c7468][3] + g_game77d180[g_game8c7468][3] * 2 + 0xfffffff4) * 4;
    }
    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x10) == 0
            && Game_Helper441699(a, 7, 2) != 0
            && (g_gameSlots[a][b].flags08 & 0x30000) == 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x90) {
        Game_Helper4c4263(1);
        return 0;
    }
    if (kind == 0x6d
        && Game_Helper441699(a, 7, 2) != 0) {
        Game_Helper4143e0(a, 0, 2);
        if (g_game8c83ec != 1) {
            Game_Helper56ca10(0x5856c0, 0x5856b0);
            Game_Helper498e8a(a, b, 1);
        }
    }
    if (kind == 0x72) {
        Game_Helper49914d(a, b, kind, 1);
        if (g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field6c != -1) {
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 = 1;
        }
    }
    if ((kind == 0x22 || kind == 0xc7)
        && g_gameSlots[a][b].field38 != 0) {
        Game_Helper46d8c1(a, b, 2);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0045bf15
int Game_CardScript45bf15(int a, int b, int kind)
{
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_game94d63c += g_game77ed80[g_game8c7468][7] * 12;
    }
    if (g_game94d178 == 0xd3
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && a == g_game8c8afc) {
        if (((g_gameSlots[a][b].flags08 & 0x10) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) != 0)
            && (g_gameSlots[a][b].flags08 & 0x20) == 0
            && Game_Helper441699(a, 7, 1) != 0
            && g_gameSlots[g_game7bf4b8][g_game8e2fc4].field6c != -1
            && (g_gameCards[g_gameSlots[g_game7bf4b8][g_game8e2fc4].field6c].flags & 0x40) != 0) {
            if (kind == 0x7d) {
                if (g_game8c7468 == a
                    && (g_game93a934 & 0x2) == 0) {
                    g_game7a01e0 |= 2;
                } else {
                    g_game7a01e0 |= 1;
                }
            }
            if (kind == 0x7e) {
                Game_LogEvent(a, b, 0x72, 0, 0);
                Game_Helper4143e0(a, 0, 1);
                Game_Helper41040d();
                if (g_game8c83ec != 1) {
                    Game_Helper40e158(a, b, 0x91);
                    Game_Helper4883aa(a, 1, a, b);
                    if (g_game8c7468 == a) {
                        g_game94d63c -= 0x18;
                    }
                }
            }
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00402ad0
int Game_CardScript402ad0(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x71) {
        for (v4 = 0; v4 < 2; v4++) {
            for (v8 = 0; g_game78e970[v4] > v8; v8++) {
                if (Game_Helper411d90(v4, v8) != 0
                    && (g_gameCards[g_gameSlots[v4][v8].field6c].flags & 0x40) != 0) {
                    Game_Helper46d8c1(v4, v8, 1);
                }
            }
        }
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00433233
int Game_CardScript433233(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x73) {
        if (g_gameSlots[a][b].field38 == 0
            && Game_Helper441699(a, 7, 4) != 0) {
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
    if (kind == 0x6d) {
        Game_Helper4143e0(a, 0, 4);
        if (g_game8c83ec != 1) {
            g_gameSlots[a][b].flags08 |= 0x10;
            g_gameSlots[a][b].field38 |= 1;
            if (g_game94d45c > 0) {
                g_game94d45c--;
            }
        }
    }
    if (kind == 0x72) {
        v8 = Game_Helper56c705(0x377);
        v4 = Game_FindFreeSlot(a, v8);
        if (v4 != -1) {
            Game_Helper4d1c03(a, v4);
            g_gameSlots[a][v4].flags18 |= 0x10;
        }
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 = 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00434051
int Game_CardScript434051(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x34
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_game7a01e0 |= 0x800 << Game_Helper4ba392(a, b, 4) - 1;
    }
    if ((kind == 0x32 || kind == 0x33)
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        v8 = 1 - a;
        for (v4 = 0; g_game78e970[v8] > v4; v4++) {
            if (Game_Helper411d90(v8, v4) != 0
                && (1 << Game_Helper4ba392(a, b, 5) & 0x15) != 0) {
                g_game7a01e0++;
                break;
            }
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0043c89c
int Game_CardScript43c89c(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    ih = 0;
    if (kind == 0x74) {
        return 1;
    }
    if ((kind == 0x32 || kind == 0x33)
        && Game_Helper411d90(a, b) != 0
        && a == g_game8e1aa8) {
        ig = ~(1 << Game_Helper4ba392(a, b, 5) | 1);
        for (iv = 0; g_game78e970[a] > iv; iv++) {
            if (Game_Helper411d90(a, iv) != 0
                && (g_gameCards[g_gameSlots[a][iv].field6c].flags & 0x42) == 2
                && (ig & 0x13) != 0) {
                ih = 1;
                break;
            }
        }
        if (ih == 0) {
            g_game7a01e0++;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0043e1c6
int Game_CardScript43e1c6(int a, int b, int kind)
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
                        g_gameSlots[a][ih].field38 = -1;
                        g_gameSlots[a][ih].field4c = 0;
                    }
                }
            }
        }
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0043e33a
int Game_CardScript43e33a(int a, int b, int kind)
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
                    && (g_gameCards[g_gameSlots[ig][iv].field6c].flags & 0x2) != 0
                    && (1 << Game_Helper4ba392(a, b, 5) & 0x15) == 0) {
                    ih = Game_Helper4a9910(a, b, g_game93a864, ig, iv);
                    if (ih != -1) {
                        g_gameSlots[a][ih].field38 = -1;
                        g_gameSlots[a][ih].field4c = -1;
                    }
                }
            }
        }
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0043e500
int Game_CardScript43e500(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x71) {
        ig = 1 << Game_Helper4ba392(a, b, 2);
        for (ih = 0; ih < 2; ih++) {
            for (iv = 0; g_game78e970[ih] > iv; iv++) {
                if (Game_Helper411d90(ih, iv) != 0
                    && (g_gameCards[g_gameSlots[ih][iv].field6c].flags & 0x2) != 0
                    && (ig & 0x13) != 0) {
                    Game_Helper4b183c(ih, iv);
                }
            }
        }
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0043e64b
int Game_CardScript43e64b(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x71) {
        for (v4 = 0; g_game78e970[a] > v4; v4++) {
            if (Game_Helper411d90(a, v4) != 0
                && (g_gameCards[g_gameSlots[a][v4].field6c].flags & 0x2) != 0) {
                v8 = Game_Helper4a9910(a, b, g_game8bcff8, a, v4);
                if (v8 != -1) {
                    g_gameSlots[a][v8].field1c = 2;
                }
            }
        }
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0043f890
int Game_CardScript43f890(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x71) {
        for (v8 = 0; v8 < 2; v8++) {
            for (v4 = 0; g_game78e970[v8] > v4; v4++) {
                if (Game_Helper411d90(v8, v4) != 0
                    && (g_gameCards[g_gameSlots[v8][v4].field6c].flags & 0x2) != 0
                    && (1 << Game_Helper4ba392(a, b, 1) & 0x15) != 0) {
                    Game_Helper46d8c1(v8, v4, 2);
                }
            }
        }
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00448939
int Game_CardScript448939(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x71) {
        for (v4 = 0; v4 < 2; v4++) {
            for (v8 = 0; g_game78e970[v4] > v8; v8++) {
                if (Game_Helper411d90(v4, v8) != 0
                    && (g_gameCards[g_gameSlots[v4][v8].field6c].flags & 0x4) != 0) {
                    Game_Helper46d8c1(v4, v8, 2);
                }
            }
        }
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00449791
int Game_CardScript449791(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x71) {
        for (v4 = 0; v4 < 2; v4++) {
            for (v8 = 0; g_game78e970[v4] > v8; v8++) {
                if (Game_Helper411d90(v4, v8) != 0
                    && (g_gameCards[g_gameSlots[v4][v8].field6c].flags & 0x1) != 0) {
                    Game_Helper46d8c1(v4, v8, 2);
                }
            }
        }
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00449888
int Game_CardScript449888(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x71) {
        for (v4 = 0; v4 < 2; v4++) {
            for (v8 = 0; g_game78e970[v4] > v8; v8++) {
                if (Game_Helper411d90(v4, v8) != 0
                    && (g_gameCards[g_gameSlots[v4][v8].field6c].flags & 0x1) != 0
                    && Game_Helper488147(v4, v8, Game_Helper4ba309(a, b, 2)) != 0) {
                    Game_Helper46d8c1(v4, v8, 2);
                }
            }
        }
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0044e84e
int Game_CardScript44e84e(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x71) {
        for (v4 = 0; v4 < 2; v4++) {
            for (v8 = 0; g_game78e970[v4] > v8; v8++) {
                if (Game_Helper411d90(v4, v8) != 0
                    && (g_gameCards[g_gameSlots[v4][v8].field6c].flags & 0x2) != 0) {
                    Game_Helper46d8c1(v4, v8, 1);
                }
            }
        }
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0044f1ac
int Game_CardScript44f1ac(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (kind == 0x74) {
        if (a == g_game8c7468
            && (g_game93a934 & 0x2) == 0
            && Game_Helper441699(a, 7, 2) == 0) {
            return 0;
        }
        return 1;
    }
    if (kind == 0x6c
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        g_gameSlots[a][b].field38 = g_game8c8274;
        if (g_game8bd200 != 1) {
            Game_Helper56ca10(0x583f44, 0x583f34);
        }
        if (Game_Helper498e8a(a, b, g_gameSlots[a][b].field38) != 0) {
            g_game94d63c -= -(0 - 0x30 / Game_Helper44a96c(a, g_gameSlots[a][b].field6c));
        }
    }
    if (kind == 0x71) {
        ih = g_gameSlots[a][b].field74;
        ig = g_gameSlots[a][b].field78;
        if (Game_Helper49914d(a, b, kind, g_gameSlots[a][b].field38) != 0
            && g_gameSlots[a][b].field78 != -1) {
            iv = Game_Helper4a9910(a, b, g_game8c7e90, ih, ig);
            if (iv != -1) {
                g_gameSlots[a][iv].field38 = 0x200;
            }
            g_gameSlots[ih][ig].field28 = 0x8000000;
        }
        g_gameSlots[a][b].field36 = 0;
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00401000
int Game_CardScript401000(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_game94d63c += 0x48 - g_game8e3ec8[a] * 24;
    }
    if (kind == 0x71) {
        for (v4 = 0, v8 = 0; g_game78e970[a] > v4; v4++) {
            if (Game_Helper40c4e1(a, v4) != 0) {
                v8++;
            }
        }
        for (v4 = 0; v8 > v4; v4++) {
            Game_Helper409ee8(a, 0, 0);
        }
        Game_Helper485308(a);
        Game_Helper44818c(a, 7);
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00419fc9
int Game_CardScript419fc9(int a, int b, int kind)
{
    if (kind == 0x73) {
        if (Game_Helper441699(a, 7, 2) != 0
            && (g_gameSlots[a][b].flags08 & 0x20014) == 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x90) {
        Game_Helper4c4263(1);
        Game_Helper4c42ae(0);
        return 0;
    }
    if (kind == 0x6d
        && Game_Helper41a64f(a, b) != 0) {
        Game_Helper4143e0(a, 0, 2);
        if (g_game8c83ec != 1) {
            Game_Helper4143e0(a, 4, -1);
        }
        if (g_game8c83ec != 1) {
            g_gameSlots[a][b].field38 = g_game8c8274;
            g_gameSlots[a][b].flags08 |= 0x10;
        }
    }
    if (kind == 0x72
        && g_gameSlots[a][b].field38 > 0) {
        if (g_game8bd200 != 1) {
            Game_Helper40ebb9(0x33);
        }
        Game_Helper41a252(g_game94f280, g_game930f5c, g_gameSlots[a][b].field38);
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 = 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00429ee0
int Game_CardScript429ee0(int a, int b, int kind)
{
    Game_Helper42d790(a, b, kind, 1);
    return 0;
}

// FUNCTION: SHANDALAR 0x00429f06
int Game_CardScript429f06(int a, int b, int kind)
{
    Game_Helper42d790(a, b, kind, 2);
    return 0;
}

// FUNCTION: SHANDALAR 0x00429f2c
int Game_CardScript429f2c(int a, int b, int kind)
{
    Game_Helper42d790(a, b, kind, 3);
    return 0;
}

// FUNCTION: SHANDALAR 0x00429f52
int Game_CardScript429f52(int a, int b, int kind)
{
    Game_Helper42d790(a, b, kind, 4);
    return 0;
}

// FUNCTION: SHANDALAR 0x00429f78
int Game_CardScript429f78(int a, int b, int kind)
{
    Game_Helper42d790(a, b, kind, 5);
    return 0;
}

// FUNCTION: SHANDALAR 0x00432798
int Game_CardScript432798(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x73) {
        if (g_game94d464 == 4
            && g_game7a82e0 == a) {
            if ((g_gameSlots[a][b].flags08 & 0x10) == 0
                && g_gameSlots[a][b].field38 == 0
                && Game_Helper4be650(0, 0, a, a, a, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                return 1;
            }
            return 0;
        }
        return 0;
    }
    if (kind == 0x90) {
        Game_Helper4c4263(0);
        return 0;
    }
    if (kind == 0x6d) {
        Game_Helper56ca10(0x5833a0, 0x583394);
        if (Game_Helper4c0efa(a, a, a, 0x200, 2, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &v8) != 0) {
            g_gameSlots[a][b].field4c = g_gameSlots[v8][v4].field0e;
            if (g_game8bd200 != 1) {
                Game_Helper40ebb9(0xf);
            }
            Game_Helper46d8c1(v8, v4, 3);
            g_gameSlots[a][b].field38 = 1;
        } else {
            g_game8c83ec = 1;
            g_gameSlots[a][b].field38 = 0;
        }
    }
    if (kind == 0x72) {
        Game_Helper4883aa(a, g_gameSlots[a][b].field4c, g_game94f280, g_game930f5c);
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
    }
    if (kind == 0x22
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_gameSlots[a][b].field38 = 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00434305
int Game_CardScript434305(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x20010) == 0
            && Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0x20, 0) != 0) {
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
            Game_Helper56ca10(0x583414, 0x583404);
        }
        if (Game_Helper4c0efa(a, 2, 1 - a, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0x20, 0, 0x8aa920, 1, &v8) != 0) {
            g_gameSlots[a][b].field74 = v8;
            g_gameSlots[a][b].field78 = v4;
            g_gameSlots[a][b].field36 = 1;
            g_gameSlots[a][b].flags08 |= 0x10;
            return 0;
        }
        g_game8c83ec = 1;
        return 0;
    }
    if (kind == 0x72) {
        v8 = g_gameSlots[a][b].field74;
        v4 = g_gameSlots[a][b].field78;
        if (Game_Helper4bea85(v8, v4, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0x20, 0) != 0) {
            Game_AllocSlot(v8, v4, 1, g_game94f280, g_game930f5c);
        } else {
            g_game8c83ec = 1;
        }
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004350e4
int Game_CardScript4350e4(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 1) {
        g_game94d660[a][1]++;
        return 0;
    }
    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x20010) == 0
            && Game_Helper441699(a, 5, 1) != 0
            && Game_Helper441699(a, 7, 2) != 0
            && Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 1 << Game_Helper4ba392(a, b, 1), 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x90) {
        Game_Helper4c4263(0);
        return 0;
    }
    if (kind == 0x6d) {
        g_game8e2690[0] = 1;
        Game_Helper4143e0(a, 5, 1);
        if (g_game8c83ec != 1) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58344c, 0x583440);
            }
            if (Game_Helper4c0efa(a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 1 << Game_Helper4ba392(a, b, 1), 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &v8) != 0) {
                g_gameSlots[a][b].field74 = v8;
                g_gameSlots[a][b].field78 = v4;
                g_gameSlots[a][b].field36 = 1;
                g_gameSlots[a][b].flags08 |= 0x10;
                return 0;
            }
            g_game8c83ec = 1;
        }
        return 0;
    }
    if (kind == 0x72) {
        v8 = g_gameSlots[a][b].field74;
        v4 = g_gameSlots[a][b].field78;
        if (Game_Helper4bea85(v8, v4, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 1 << Game_Helper4ba392(a, b, 1), 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
            Game_Helper46d8c1(v8, v4, 2);
        } else {
            g_game8c83ec = 1;
        }
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
    }
    if (kind == 0xc7
        && g_game94d464 == 0x22
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_game94d63c += g_game77ed80[1 - a][Game_Helper4ba392(a, b, 1)] * 12;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00436655
int Game_CardScript436655(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x20010) == 0
            && Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 1, 0, 0) != 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x90) {
        Game_Helper4c4263(0);
        return 0;
    }
    if (kind == 0x6d) {
        Game_Helper56ca10(0x5834b8, 0x5834a4);
        if (Game_Helper4c0efa(a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 1, 0, 0, 0x8aa920, 1, &v8) != 0) {
            g_gameSlots[a][b].field74 = v8;
            g_gameSlots[a][b].field78 = v4;
            g_gameSlots[a][b].field36 = 1;
            g_gameSlots[a][b].flags08 |= 0x10;
            Game_Helper46d8c1(a, b, 3);
        } else {
            g_game8c83ec = 1;
        }
    }
    if (kind == 0x72) {
        v8 = g_gameSlots[a][b].field74;
        v4 = g_gameSlots[a][b].field78;
        if (Game_Helper4bea85(v8, v4, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 1, 0, 0) != 0) {
            Game_Helper46d8c1(v8, v4, 2);
        } else {
            g_game8c83ec = 1;
        }
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00437637
int Game_CardScript437637(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x20010) == 0
            && Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 1 << Game_Helper4ba309(a, b, 2) - 1, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x6d) {
        if (g_game8bd200 != 1) {
            Game_Helper56ca10(0x58352c, 0x583518);
        }
        if (Game_Helper4c0efa(a, 2, 2, 0x200, 2, 0, 1 << Game_Helper4ba309(a, b, 2) - 1, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &v8) != 0) {
            g_gameSlots[a][b].field74 = v8;
            g_gameSlots[a][b].field78 = v4;
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
        v8 = g_gameSlots[a][b].field74;
        v4 = g_gameSlots[a][b].field78;
        if (Game_Helper4bea85(v8, v4, 0, a, 2, 2, 0x200, 2, 0, 1 << Game_Helper4ba309(a, b, 2) - 1, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
            Game_Helper46d8c1(v8, v4, 2);
        } else {
            g_game8c83ec = 1;
        }
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
        return 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004392a0
int Game_CardScript4392a0(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x20010) == 0
            && Game_Helper441699(a, 3, 1) != 0
            && Game_Helper4be650(0, 0, a, 2, 2, 0x200, 0x40, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x90) {
        Game_Helper4c4263(0);
        return 0;
    }
    if (kind == 0x6d) {
        Game_Helper4143e0(a, 3, 1);
        if (g_game8c83ec != 1) {
            Game_Helper56ca10(0x5835fc, 0x5835ec);
            if (Game_Helper4c0efa(a, 2, 2, 0x200, 0x40, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &v8) != 0) {
                g_gameSlots[a][b].field74 = v8;
                g_gameSlots[a][b].field78 = v4;
                g_gameSlots[a][b].field36 = 1;
                g_gameSlots[a][b].flags08 |= 0x10;
                Game_Helper46d8c1(a, b, 3);
            } else {
                g_game8c83ec = 1;
            }
        }
    }
    if (kind == 0x72) {
        v8 = g_gameSlots[a][b].field74;
        v4 = g_gameSlots[a][b].field78;
        if (Game_Helper4bea85(v8, v4, 0, a, 2, 2, 0x200, 0x40, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
            Game_Helper46d8c1(v8, v4, 2);
        } else {
            g_game8c83ec = 1;
        }
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00439655
int Game_CardScript439655(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x20010) == 0
            && Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 1 << Game_Helper4ba392(a, b, 2), 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x6d) {
        if (g_game8bd200 != 1) {
            Game_Helper56ca10(0x58361c, 0x58360c);
        }
        if (Game_Helper4c0efa(a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 1 << Game_Helper4ba392(a, b, 2), 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &v8) != 0) {
            g_gameSlots[a][b].field74 = v8;
            g_gameSlots[a][b].field78 = v4;
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
        v8 = g_gameSlots[a][b].field74;
        v4 = g_gameSlots[a][b].field78;
        if (Game_Helper4bea85(v8, v4, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 1 << Game_Helper4ba392(a, b, 2), 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
            Game_Helper46d8c1(v8, v4, 2);
        } else {
            g_game8c83ec = 1;
        }
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
        return 0;
    }
    if (kind == 0x8a
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && g_game77ed80[1 - a][Game_Helper4ba392(a, b, 2)] != 0) {
        g_game97db30 += 0x18;
        return 0;
    }
    if (kind == 0x8b
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && g_game77ed80[1 - a][Game_Helper4ba392(a, b, 2)] != 0) {
        g_game97db30 -= 0x18;
        return 0;
    }
    if (kind == 0xc7
        && g_game94d464 == 0x22
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_game94d63c += g_game77ed80[1 - a][Game_Helper4ba392(a, b, 2)] * 12;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0043d798
int Game_CardScript43d798(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        Game_Helper4c4263(0);
        Game_Helper4be650(0, 0, a, 2, 2, 0x200, 0x1047, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            g_game94d63c -= 0x30;
            Game_Helper56ca10(0x5836e8, 0x5836dc);
            if (Game_Helper4c0efa(a, 2, 1 - a, 0x200, 0x1047, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &v8) != 0) {
                g_gameSlots[a][b].field78 = v4;
                g_gameSlots[a][b].field74 = v8;
                g_gameSlots[a][b].field36 = 1;
            } else {
                g_game8c83ec = 1;
            }
        }
        if (kind == 0x71) {
            v8 = g_gameSlots[a][b].field74;
            v4 = g_gameSlots[a][b].field78;
            if (Game_Helper4bea85(v8, v4, 0, a, 2, 2, 0x200, 0x1047, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
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

// FUNCTION: SHANDALAR 0x0043e76e
int Game_CardScript43e76e(int a, int b, int kind)
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
        && a == g_game8e1aa8) {
        if (g_game8bd200 != 1) {
            Game_Helper56ca10(0x583748, 0x583738);
        }
        if (Game_Helper4c0efa(a, 2, 1 - a, 0x1000, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &ij) != 0) {
            g_gameSlots[a][b].field74 = ij;
            g_gameSlots[a][b].field78 = ii;
            g_gameSlots[a][b].field36 = 1;
        } else {
            g_game8c83ec = 1;
        }
    }
    if (kind == 0x71) {
        ih = g_gameSlots[a][b].field74;
        for (ig = 0, iv = 0; g_game78e970[ih] > ig; ig++) {
            if (Game_Helper40c4e1(ih, ig) != 0) {
                iv++;
            }
        }
        Game_Helper4bab38(ih, iv, a, b);
        g_gameSlots[a][b].field36 = 0;
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0043fefe
int Game_CardScript43fefe(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;
    int ii;
    int ij;
    int ik;

    if (kind == 0x74) {
        return 1;
    }
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        if (g_game8bd200 != 1) {
            Game_Helper56ca10(0x5837ec, 0x5837e0);
        }
        if (Game_Helper4c0efa(a, 2, 1 - a, 0x1000, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &ik) != 0) {
            g_gameSlots[a][b].field74 = ik;
            g_gameSlots[a][b].field78 = ij;
            g_gameSlots[a][b].field36 = 1;
        } else {
            g_game8c83ec = 1;
        }
    }
    if (kind == 0x71) {
        ii = g_gameSlots[a][b].field74;
        iv = 1 << Game_Helper4ba392(a, b, 5);
        for (ih = 0, ig = 0; g_game78e970[ii] > ih; ih++) {
            if (Game_Helper411d90(ii, ih) != 0
                && (g_gameCards[g_gameSlots[ii][ih].field6c].flags & 0x2) != 0
                && (iv & 0x13) != 0) {
                ig++;
            }
        }
        if (ig != 0) {
            Game_Helper4bab38(ii, ig + ig, a, b);
        }
        g_gameSlots[a][b].field36 = 0;
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

/* 0x583c68 / 0x583c5c are "prompts.txt" / "BRAINGEYSER" in the original's
 * data; 0x8aa920 is an out-buffer the 20-argument helper fills. */
// FUNCTION: SHANDALAR 0x004476d4
int Game_CardScript4476d4(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;
    int ii;

    if (kind == 0x74) {
        if (a == g_game8c7468
            && (g_game93a934 & 0x2) == 0
            && Game_Helper441699(a, 7, 3) == 0) {
            return 0;
        }
        return 1;
    }
    if (kind == 0x6c
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        if (g_game8bd200 != 1) {
            Game_Helper56ca10(0x583c68, 0x583c5c);
        }
        if (Game_Helper4c0efa(a, 2, a, 0x1000, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &ii) != 0) {
            g_gameSlots[a][b].field38 = g_game8c8274;
            g_gameSlots[a][b].field74 = ii;
            g_gameSlots[a][b].field78 = ih;
            g_gameSlots[a][b].field36 = 1;
        } else {
            g_game8c83ec = 1;
        }
    }
    if (kind == 0x71) {
        iv = g_gameSlots[a][b].field74;
        for (ig = 0; g_gameSlots[a][b].field38 > ig; ig++) {
            Game_Helper409a0b(iv);
        }
        g_gameSlots[a][b].field36 = 0;
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00448662
int Game_CardScript448662(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        if (a == g_game8c7468
            && (g_game93a934 & 0x2) == 0
            && Game_Helper441699(a, 7, 2) == 0) {
            return 0;
        }
        return 1;
    }
    if (kind == 0x6c
        && g_game951bdc == b
        && a == g_game8e1aa8) {
        g_game94d63c -= -(0 - g_game8c83f0[a] * 24 / Game_Helper44a96c(a, g_gameSlots[a][b].field6c));
        if (g_game8bd200 != 1) {
            Game_Helper56ca10(0x583cd8, 0x583cc8);
        }
        if (Game_Helper4c0efa(a, 2, a, 0x1000, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &v8) != 0) {
            g_gameSlots[a][b].field38 = g_game8c8274;
            g_gameSlots[a][b].field74 = v8;
            g_gameSlots[a][b].field78 = v4;
            g_gameSlots[a][b].field36 = 1;
        } else {
            g_game8c83ec = 1;
        }
    }
    if (kind == 0x71) {
        Game_Helper4883aa(g_gameSlots[a][b].field74, g_gameSlots[a][b].field38, a, b);
        g_gameSlots[a][b].field36 = 0;
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00449f2c
int Game_CardScript449f2c(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        Game_Helper4be650(0, 0, a, 2, 2, 0x200, 0x1047, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            g_game94d63c -= -(0 - 0x30 / Game_Helper44a96c(a, g_gameSlots[a][b].field6c));
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x583d30, 0x583d20);
            }
            if (Game_Helper4c0efa(a, 2, 1 - a, 0x200, 0x1047, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &v8) != 0) {
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
            if (Game_Helper4bea85(v8, v4, 0, a, 2, 2, 0x200, 0x1047, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
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

// FUNCTION: SHANDALAR 0x0044fd8c
int Game_CardScript44fd8c(int a, int b, int kind)
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
        if (g_game8bd200 != 1) {
            Game_Helper56ca10(0x583f8c, 0x583f80);
        }
        if (Game_Helper4c0efa(a, 2, 1 - a, 0x1000, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &ii) != 0) {
            g_gameSlots[a][b].field74 = ii;
            g_gameSlots[a][b].field78 = ih;
            g_gameSlots[a][b].field36 = 1;
        } else {
            g_game8c83ec = 1;
        }
    }
    if (kind == 0x71) {
        iv = g_gameSlots[a][b].field74;
        Game_Helper49f3c0((int)Game_Callback44ffd0, iv);
        if (iv != a) {
            for (ig = 0; ig <= 7; ig++) {
                g_game7806a0[a][ig] += g_game7806a0[iv][ig];
                g_game7806a0[iv][ig] = 0;
            }
        }
        g_gameSlots[a][b].field36 = 0;
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00456cd1
int Game_CardScript456cd1(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x6c
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        g_game94d63c += 0xc;
    }
    if (kind == 0x73) {
        if (Game_Helper441699(a, 7, 1) != 0) {
            if (((g_gameSlots[a][b].flags08 & 0x30000) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0)
                && (g_gameSlots[a][b].flags08 & 0x10) == 0
                && Game_Helper4be650(0, 0, a, 2, 2, 0x200, 0x43, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                return 1;
            }
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
            Game_Helper56ca10(0x585574, 0x585564);
            if (Game_Helper4c0efa(a, 2, 2, 0x200, 0x43, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &v8) != 0) {
                g_gameSlots[a][b].field74 = v8;
                g_gameSlots[a][b].field78 = v4;
                g_gameSlots[a][b].field36 = 1;
                if (a == g_game8c7468) {
                    if ((g_gameCards[g_gameSlots[v8][v4].field6c].flags & 0x1) != 0) {
                        g_game94d63c -= 0x18;
                    }
                    if (v8 == g_game8c7468) {
                        g_game94d63c -= 0x60;
                    }
                }
                g_gameSlots[a][b].flags08 |= 0x10;
            } else {
                g_game8c83ec = 1;
            }
        }
    }
    if (kind == 0x72) {
        v8 = g_gameSlots[a][b].field74;
        v4 = g_gameSlots[a][b].field78;
        if (Game_Helper4bea85(v8, v4, 0, a, 2, 2, 0x200, 0x43, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
            Game_Helper4b183c(v8, v4);
        } else {
            g_game8c83ec = 1;
        }
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0045782f
int Game_CardScript45782f(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x10) == 0) {
            if (((g_gameSlots[a][b].flags08 & 0x30000) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0)
                && Game_Helper441699(a, 7, 2) != 0) {
                if (g_game8c7468 == a
                    && (g_game93a934 & 0x2) == 0) {
                    for (ih = 0, iv = 0; g_game78e970[a] > ih; ih++) {
                        ig = g_gameSlots[a][ih].field6c;
                        if (ig != -1
                            && (g_gameCards[ig].flags & 0x1) != 0) {
                            iv++;
                        }
                    }
                    if (iv < 3 || iv > 8) {
                        g_game94d63c += 0x30;
                    }
                }
                return 1;
            }
        }
        return 0;
    }
    if (kind == 0x6d) {
        Game_Helper4143e0(a, 0, 2);
        if (g_game8c83ec != 1) {
            g_gameSlots[a][b].flags08 |= 0x10;
        }
    }
    if (kind == 0x72) {
        Game_Helper409a0b(a);
        Game_Helper409ee8(a, 0, 0);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0045862c
int Game_CardScript45862c(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x10) == 0) {
            if (((g_gameSlots[a][b].flags08 & 0x30000) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0)
                && Game_Helper441699(a, 7, 6) != 0
                && Game_Helper4be650(0, 0, a, a | 0xc, a | 0xc, 0x200, 0x1047, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                return 1;
            }
        }
        return 0;
    }
    if (kind == 0x90) {
        Game_Helper4c4263(0);
        return 0;
    }
    if (kind == 0x6d) {
        Game_Helper4143e0(a, 0, 6);
        if (g_game8c83ec != 1) {
            Game_Helper56ca10(0x5855ec, 0x5855d8);
            if (Game_Helper4c0efa(a, a | 0xc, a | 0xc, 0x200, 0x1047, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &v8) != 0) {
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
        if (Game_Helper4bea85(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78, 0, a, a | 0xc, a | 0xc, 0x200, 0x1047, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
            Game_Helper4ba41b(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78);
        } else {
            g_game8c83ec = 1;
        }
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00459f70
int Game_CardScript459f70(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (kind == 0x6c
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        g_game94d63c += 0xc;
    }
    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x10) == 0) {
            if (((g_gameSlots[a][b].flags08 & 0x30000) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0)
                && Game_Helper441699(a, 7, 2) != 0
                && Game_Helper4be650(0, 0, a, 2, a, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                return 1;
            }
        }
        return 0;
    }
    if (kind == 0x90) {
        Game_Helper4c4263(0);
        return 0;
    }
    if (kind == 0x6d) {
        Game_Helper4143e0(a, 0, 2);
        if (g_game8c83ec != 1) {
            Game_Helper56ca10(0x5856e4, 0x5856d0);
            if (Game_Helper4c0efa(a, 2, a, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &ih) != 0) {
                g_gameSlots[a][b].field74 = ih;
                g_gameSlots[a][b].field78 = ig;
                g_gameSlots[a][b].field36 = 1;
                g_gameSlots[a][b].flags08 |= 0x10;
            } else {
                g_game8c83ec = 1;
            }
        }
    }
    if (kind == 0x72) {
        ih = g_gameSlots[a][b].field74;
        ig = g_gameSlots[a][b].field78;
        if (Game_Helper4bea85(ih, ig, 0, a, 2, 2, 0x200, 2, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
            iv = Game_Helper4a9910(g_game94f280, g_game930f5c, g_game8aa734, ih, ig);
            if (iv != -1) {
                g_gameSlots[a][iv].field38 = 1 << Game_Helper4ba309(a, b, 2) - 1;
            }
            g_gameSlots[ih][ig].field28 = 0x8000000;
        } else {
            g_game8c83ec = 1;
        }
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00429f9e
int Game_CardScript429f9e(int a, int b, int kind)
{
    if (kind == 0x7f) {
        Game_Helper42d790(a, b, kind, Game_Helper40dffd((signed char)g_gameSlots[a][b].field58));
    } else {
        if (kind == 0x73) {
            Game_Helper42d790(a, b, kind, Game_Helper40dffd((signed char)g_gameSlots[a][b].field58));
        } else {
            if (kind == 0x6d) {
                Game_Helper42d790(a, b, kind, Game_Helper40dffd((signed char)g_gameSlots[a][b].field58));
            } else {
                if (kind == 0x71 || (kind == 0x81 || kind == 0x91) && b == g_game951bdc && a == g_game8e1aa8) {
                    if (g_game8bd200 != 1) {
                        Game_Helper40ebb9(0x31);
                    }
                    if ((g_game93a934 & 0x2) != 0) {
                        g_gameSlots[a][b].field58 = 1 << Game_Helper522535(a, 5) + 1;
                        g_gameSlots[a][b].field38 = (signed char)g_gameSlots[a][b].field58;
                        return 0;
                    }
                    if (a == g_game8c776c) {
                        g_gameSlots[a][b].field58 = 1 << Game_Helper522508(5) + 1;
                        g_gameSlots[a][b].field38 = (signed char)g_gameSlots[a][b].field58;
                        return 0;
                    }
                    g_gameSlots[a][b].field58 = g_game97c85c;
                    g_gameSlots[a][b].field38 = (signed char)g_gameSlots[a][b].field58;
                    return 0;
                }
                if (kind == 1) {
                    Game_Helper42d790(a, b, kind, Game_Helper40dffd((signed char)g_gameSlots[a][b].field58));
                } else {
                }
            }
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00435590
int Game_CardScript435590(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (kind == 0x6c && g_game8e1aa8 == a && (g_gameCards[g_gameSlots[g_game8e1aa8][g_game951bdc].field6c].flags & 0x2) != 0) {
        g_game94d63c += 0x30;
    }
    if (kind == 0x73) {
        return Game_Helper4be650(0, 0, a, a, a, 0x200, 2, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x90) {
            Game_Helper4c4263(0);
            return 0;
        } else {
            if (kind == 0x6d) {
                Game_Helper56ca10(0x58346c, 0x58345c);
                if (Game_Helper4c0efa(a, a, a, 0x200, 2, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &ih) != 0) {
                    if (g_game8bd200 != 1) {
                        Game_Helper40ebb9(0xf);
                    }
                    Game_Helper46d8c1(ih, ig, 3);
                    g_gameSlots[a][b].field74 = a;
                    g_gameSlots[a][b].field78 = b;
                    g_gameSlots[a][b].field36 = 1;
                    if (g_gameSlots[a][b].field38 == 0) {
                        g_gameSlots[a][b].field38 |= 0x80000;
                    }
                } else {
                    g_game8c83ec = 1;
                }
            }
            if (kind == 0x72) {
                if (g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field6c != -1) {
                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 += 2;
                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 += 0x100;
                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
                    if ((g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 & 0x80000) != 0) {
                        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 &= ~0x80000;
                        iv = Game_Helper4a9910(g_game94f280, g_game930f5c, g_game8bcff8, g_game94f280, g_game930f5c);
                        if (iv != -1) {
                            g_gameSlots[a][iv].field38 |= 0x80000;
                        }
                    }
                } else {
                    g_game8c83ec = 1;
                }
            }
            if (kind == 0x22 || kind == 0xc7) {
                g_gameSlots[a][b].field38 = 0;
            }
            if (kind == 0xc7 && g_game94d464 == 0x1f && g_game8c7468 == a) {
                g_game94d63c += g_game8e3ed4 * 24;
            }
            return 0;
        }
    }
}

// FUNCTION: SHANDALAR 0x00439b5c
int Game_CardScript439b5c(int a, int b, int kind)
{
    int v4;

    if (kind == 1) {
        v4 = Game_Helper4ba309(a, b, 1);
        g_game94d650[a][v4]++;
    }
    if (kind == 0x73 || kind == 0x6d || kind == 0x72) {
        return Game_Helper4909b0(a, b, kind, 1, 1);
    } else {
        return 0;
    }
}

// FUNCTION: SHANDALAR 0x0043a19e
int Game_CardScript43a19e(int a, int b, int kind)
{
    int v4;

    if (kind == 0x6c && b == g_game951bdc && a == g_game8e1aa8) {
        g_gameSlots[a][b].field4c = 0;
        g_gameSlots[a][b].field38 = g_gameSlots[a][b].field4c;
        return 0;
    } else {
        if (kind == 0x73) {
            return Game_Helper441699(a, 7, 1);
        } else {
            if (kind == 0x90) {
                Game_Helper4c42ae(0);
                return 0;
            } else {
                if (kind == 0x6d) {
                    if (Game_Helper441699(a, 7, 1) != 0) {
                        if (a == g_game7a82e0) {
                            Game_Helper4143e0(a, 0, -1);
                            if (g_game8c8274 > 0) {
                                g_gameSlots[a][b].field4c = g_game8c8274;
                            } else {
                                g_game8c83ec = 1;
                            }
                        } else {
                            Game_Helper4143e0(a, 0, 1);
                            g_gameSlots[a][b].field4c = 1;
                        }
                        if (g_game8c83ec != 1) {
                            g_gameSlots[a][b].field74 = a;
                            g_gameSlots[a][b].field78 = b;
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
                        if (g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field6c != -1) {
                            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 += g_gameSlots[a][b].field4c & 0xff;
                            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
                            if ((g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 & 0x80000) != 0) {
                                g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 &= ~0x80000;
                                v4 = Game_Helper4a9910(g_game94f280, g_game930f5c, g_game8bcff8, g_game94f280, g_game930f5c);
                                if (v4 != -1) {
                                    g_gameSlots[a][v4].field12 = 1;
                                    g_gameSlots[a][v4].field1c = 1;
                                    g_gameSlots[a][v4].field38 |= 0x80000;
                                }
                            }
                        } else {
                            g_game8c83ec = 1;
                        }
                        return 0;
                    } else {
                        if (kind == 0x39) {
                            return Game_Helper441699(a, 7, 1);
                        } else {
                            if (kind == 0x8f) {
                                g_game7a01e0 |= 1;
                                return 0;
                            } else {
                                if (kind == 0xc7 && g_game94d464 == 0x1f) {
                                    if (a == g_game8c7468) {
                                        g_game94d63c += (g_game77ed80[a][7] + g_game77ed80[a][7] * 2 + 6) * 4;
                                    } else {
                                        g_game94d63c -= -(0 - (g_game77ed80[a][7] + g_game77ed80[a][7] * 2 + 6) * 4);
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

// FUNCTION: SHANDALAR 0x0043da66
int Game_CardScript43da66(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        Game_Helper4c4263(0);
        return Game_Helper4be650(0, 0, a, 2, 2, 0x200, 0x40, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c && g_game951bdc == b && g_game8e1aa8 == a) {
            Game_Helper56ca10(0x583708, 0x5836f8);
            if (Game_Helper4c0efa(a, 2, 2, 0x200, 0x40, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &v8) != 0) {
                g_gameSlots[a][b].field74 = v8;
                g_gameSlots[a][b].field78 = v4;
                g_gameSlots[a][b].field36 = 1;
                if (g_game8c7468 == a) {
                    if ((signed char)g_gameCards[g_gameSlots[v8][v4].field6c].field2c == 0) {
                        g_game94d63c -= 0x30;
                    }
                }
            } else {
                g_game8c83ec = 1;
            }
        }
        if (kind == 0x71) {
            v8 = g_gameSlots[a][b].field74;
            v4 = g_gameSlots[a][b].field78;
            if (Game_Helper4bea85(v8, v4, 0, a, 2, 2, 0x200, 0x40, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                Game_Helper4883aa(a, (signed char)g_gameCards[g_gameSlots[v8][v4].field6c].field2b + (signed char)g_gameCards[g_gameSlots[v8][v4].field6c].field2c, a, b);
                Game_Helper46d8c1(v8, v4, 2);
            } else {
                g_game8c83ec = 1;
            }
            g_gameSlots[a][b].field36 = 0;
            Game_Helper46d8c1(a, b, 1);
        }
        return 0;
    }
}

// FUNCTION: SHANDALAR 0x0043ddff
int Game_CardScript43ddff(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;
    int ii;

    if (kind == 0x74) {
        Game_Helper4c4263(0);
        return Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x6c && g_game951bdc == b && g_game8e1aa8 == a) {
            Game_Helper56ca10(0x583728, 0x583718);
            if (Game_Helper4c0efa(a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &ii) != 0) {
                g_gameSlots[a][b].field74 = ii;
                g_gameSlots[a][b].field78 = ih;
                g_gameSlots[a][b].field36 = 1;
            } else {
                g_game8c83ec = 1;
            }
        }
        if (kind == 0x71) {
            ii = g_gameSlots[a][b].field74;
            ih = g_gameSlots[a][b].field78;
            if (Game_Helper4bea85(ii, ih, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                ig = Game_Helper4a9910(a, b, g_game8bcff8, ii, ih);
                if (ig != -1) {
                    if ((signed char)g_gameCards[g_gameSlots[ii][ih].field6c].field2c == -1) {
                        iv = 0;
                    } else {
                        iv = (signed char)g_gameCards[g_gameSlots[ii][ih].field6c].field2c;
                    }
                    g_gameSlots[a][ig].field1c = (signed char)g_gameCards[g_gameSlots[ii][ih].field6c].field2b + iv;
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

// FUNCTION: SHANDALAR 0x0043ec68
int Game_CardScript43ec68(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

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
        if (kind == 0x6c && g_game951bdc == b && a == g_game8e1aa8) {
            if (g_game8e2688 != -1) {
                g_gameSlots[a][b].field74 = g_game8e2688;
                g_gameSlots[a][b].field78 = g_game8e2674;
                g_gameSlots[a][b].field36 = 1;
            } else {
                g_game8c83ec = 1;
            }
            if (g_game8c7468 == g_game8e2688) {
                g_game94d63c -= 0x24;
            }
        }
        if (kind == 0x38 && g_game77d160[a][6] >= 1) {
            g_game94d63c += 0x18;
        }
        if (kind == 0x71) {
            ih = g_gameSlots[a][b].field74;
            ig = g_gameSlots[a][b].field78;
            if (Game_Helper4bea85(ih, ig, 0, a, 2, 2, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 2, 0, 0) != 0) {
                if (Game_Helper441699(ih, 7, 1) != 0) {
                    if ((ih == g_game8c776c || (g_game93a934 & 0x2) != 0) && g_game8bd200 != 1) {
                        Game_Helper40f77c(a, b, 0x7e, 0, 0);
                        iv = Game_Helper4143e0(ih, 0, 1);
                        Game_Helper41040d();
                        g_game8c83ec = 0;
                    } else {
                        iv = Game_Helper4143e0(ih, 0, 1);
                    }
                } else {
                    iv = 0;
                }
                if (iv == 0) {
                    Game_Helper46d8c1(ih, ig, 1);
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

// FUNCTION: SHANDALAR 0x0044bde2
int Game_CardScript44bde2(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x74) {
        if (Game_Helper4be650(0, 0, a, 2, 2, 0x200, 0x40, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
            return 1;
        } else {
            return 0;
        }
    } else {
        if (kind == 0x6c && g_game951bdc == b && g_game8e1aa8 == a) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x583d9c, 0x583d90);
            }
            if (Game_Helper4c0efa(a, 2, 1 - a, 0x200, 0x40, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &v8) != 0) {
                g_gameSlots[a][b].field38 = g_game8c8274;
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
            if (Game_Helper4bea85(v8, v4, 0, a, 2, 2, 0x200, 0x40, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
                if ((signed char)g_gameCards[g_gameSlots[v8][v4].field6c].field2b + (signed char)g_gameCards[g_gameSlots[v8][v4].field6c].field2c == g_gameSlots[a][b].field38) {
                    Game_Helper4bab38(v8, g_gameSlots[a][b].field38, a, b);
                    Game_Helper46d8c1(v8, v4, 1);
                } else {
                    g_game8c83ec = 1;
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

// FUNCTION: SHANDALAR 0x00455247
int Game_CardScript455247(int a, int b, int kind)
{
    int v4;

    if (kind == 0x82 && g_game951bdc == b && g_game8e1aa8 == a) {
        g_gameSlots[a][b].field11c &= ~3;
    }
    if (kind == 0x73) {
        v4 = 1;
        if ((g_gameSlots[a][b].flags08 & 0x30000) != 0) {
            if ((g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) != 0) {
                v4 = 0;
            }
        }
        if ((g_gameSlots[a][b].flags08 & 0x10) != 0) {
            if (Game_Helper441699(a, 7, 3) == 0) {
                v4 = 0;
            }
        }
        if (g_game8c7468 == a && (g_game93a934 & 0x2) == 0 && g_gameSlots[a][b].field38 != 0) {
            v4 = 0;
        }
        return v4;
    } else {
        if (kind == 0x6d) {
            if ((g_gameSlots[a][b].flags08 & 0x10) != 0) {
                if (Game_Helper441699(a, 7, 3) != 0) {
                    Game_Helper4143e0(a, 0, 3);
                    if (g_game8c83ec != 1) {
                        g_gameSlots[a][b].field4c = 1;
                    }
                }
            } else {
                g_game94d63c -= 0x24;
                Game_Helper441651(a, 0, 3);
                g_game93a794 = 0;
                g_gameSlots[a][b].flags08 |= 0x10;
            }
        }
        if (kind == 0x72 && (g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].flags08 & 0x10) != 0 && g_gameSlots[a][b].field4c != 0) {
            g_game7bfe78 |= 2;
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 = 1;
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field4c = 0;
        }
        if ((g_game94d178 == 0xcb || g_game94d178 == 0xe2 || g_game94d178 == 0xe3 || g_game94d178 == 0xe4) && g_game951bdc == b && g_game8e1aa8 == a && a == g_game8c8afc && (g_gameSlots[a][b].flags08 & 0x10) != 0 && g_gameSlots[a][b].field38 != 0) {
            if (kind == 0x7d) {
                g_game7a01e0 |= 2;
            }
            if (kind == 0x7e) {
                g_gameSlots[a][b].field38 = 0;
                g_gameSlots[a][b].flags08 &= ~0x10;
            }
        }
        if (kind == 0x7f && g_game951bdc == b && g_game8e1aa8 == a && (g_gameSlots[a][b].flags08 & 0x10) == 0 && ((g_gameSlots[a][b].flags08 & 0x30000) == 0 || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0)) {
            Game_Helper441539(a, 0, 3);
        }
        return 0;
    }
}

// FUNCTION: SHANDALAR 0x00457f9e
int Game_CardScript457f9e(int a, int b, int kind)
{
    if (kind == 0x73 || kind == 0x6d || kind == 0x72) {
        return Game_Helper4909b0(a, b, kind, 0, 1);
    } else {
        return 0;
    }
}

// FUNCTION: SHANDALAR 0x0043c5b6
int Game_CardScript43c5b6(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;
    int ii;

    if (kind == 0xc7 && g_game94d464 == 0x22 && g_game951bdc == b && a == g_game8e1aa8) {
        ih = 0;
        ii = 1 - a;
        iv = 0;
        for (; g_game78e970[ii] > ih; ih++) {
            if ((1 << Game_Helper4ba392(a, b, 1) & 0x15) != 0) {
                if (Game_Helper411d90(ii, ih) != 0) {
                    iv += 0x18;
                }
            }
        }
        if (iv != 0) {
            g_game94d63c += iv + 0x18;
        }
    }
    if (kind == 0x34 && g_game951bdc == b && a == g_game8e1aa8) {
        g_game7a01e0 |= 0x800 << Game_Helper4ba392(a, b, 1) - 1;
        ig = g_game7a01e0;
        Game_Helper49c6b1(a, b, 1);
        g_game7a01e0 = ig;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0043c72a
int Game_CardScript43c72a(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;
    int ii;

    if (kind == 0xc7 && g_game94d464 == 0x22 && b == g_game951bdc && a == g_game8e1aa8) {
        ih = 0;
        ii = 1 - a;
        iv = 0;
        for (; g_game78e970[ii] > ih; ih++) {
            if ((1 << Game_Helper4ba392(a, b, 5) & 0x15) != 0) {
                if (Game_Helper411d90(ii, ih) != 0) {
                    iv += 0x18;
                }
            }
        }
        if (iv != 0) {
            g_game94d63c += iv + 0x18;
        }
    }
    if (kind == 0x34 && b == g_game951bdc && a == g_game8e1aa8) {
        g_game7a01e0 |= 0x800 << Game_Helper4ba392(a, b, 5) - 1;
        ig = g_game7a01e0;
        Game_Helper49c6b1(a, b, 5);
        g_game7a01e0 = ig;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0044710d
int Game_CardScript44710d(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;
    int ii;

    if (kind == 0x74) {
        return 1;
    } else {
        if (kind == 0x6c && g_game951bdc == b && g_game8e1aa8 == a) {
            g_game94d63c += 0x60;
        }
        if (kind == 0x71) {
            if (g_game93a798 == -1) {
                ii = 0;
                ig = 0;
                for (; ig < 2 && ii == 0; ig++) {
                    iv = 0;
                    for (; g_game78e970[ig] > iv; iv++) {
                        if (g_gameSlots[ig][iv].field6c == g_game8c8b44) {
                            if ((g_gameSlots[ig][iv].flags18 & 0x100) != 0) {
                                ii = 1;
                            }
                        }
                    }
                }
                if (ii == 0) {
                    g_game93a798 = a;
                }
            }
            ih = Game_Helper4a9910(a, b, g_game8c8b44, -1, -1);
            g_gameSlots[a][ih].flags18 |= 0x120;
            Game_Helper46d8c1(a, b, 1);
        }
        return 0;
    }
}

// FUNCTION: SHANDALAR 0x00447de6
int Game_CardScript447de6(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;
    int ii;

    if (kind == 0x74) {
        return 1;
    } else {
        if (kind == 0x71) {
            ih = 0;
            iv = g_game7a82e0;
            while (ih < 2) {
                ii = 0;
                ig = 0;
                for (; g_game78e970[iv] > ig; ig++) {
                    if (Game_Helper40c4e1(iv, ig) != 0) {
                        Game_Helper56c4f7(iv, g_gameSlots[iv][ig].field6c);
                        g_gameSlots[iv][ig].field6c = -1;
                        ii++;
                    }
                }
                Game_Helper558aea(0, 0x30);
                Game_Helper56c1b5(a, iv);
                Game_Helper44818c(iv, ii);
                ih++;
                if (g_game7a82e0 != 0) {
                    iv--;
                } else {
                    iv++;
                }
            }
            Game_Helper46d8c1(a, b, 1);
        }
        return 0;
    }
}

// FUNCTION: SHANDALAR 0x00447f4b
int Game_CardScript447f4b(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (kind == 0x74) {
        return 1;
    } else {
        if (kind == 0x6c && b == g_game951bdc && a == g_game8e1aa8) {
            g_game94d63c += 0x30 - g_game8e3ec8[a] * 24;
        }
        if (kind == 0x71) {
            ih = 0;
            iv = g_game7a82e0;
            while (ih < 2) {
                ig = 0;
                for (; g_game78e970[iv] > ig; ig++) {
                    if (Game_Helper40c4e1(iv, ig) != 0) {
                        Game_Helper56c4f7(iv, g_gameSlots[iv][ig].field6c);
                        g_gameSlots[iv][ig].field6c = -1;
                    }
                }
                ig = 0;
                for (; ig < 0x1f4 && g_game94d930[iv][ig] != -1; ig++) {
                    Game_Helper56c4f7(iv, g_game94d930[iv][ig]);
                }
                ig = 0;
                for (; ig < 0x1f4; ig++) {
                    g_game94d930[iv][ig] = -1;
                }
                Game_Helper558aea(0, 0x30);
                Game_Helper56c1b5(a, iv);
                Game_Helper44818c(iv, 7);
                ih++;
                if (g_game7a82e0 != 0) {
                    iv--;
                } else {
                    iv++;
                }
            }
            Game_Helper46d8c1(a, b, 1);
        }
        return 0;
    }
}
