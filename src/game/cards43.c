/* Card behaviour scripts, first batch (addresses 0x401000-0x45a8d4).
 *
 * Re-derived from the Ghidra decompilation of orig/Shandalar.exe
 * (reports/ghidra/decomp/<addr>.c), cross-checked against the archived
 * attempt's naming and measured codegen levers (salvage/, quirks Q-021:
 * local names set /Od stack slots, so vN names locals at [ebp-N]).
 *
 * Every script has the same shape: int f(int a, int b, int kind), where
 * (a, b) address g_gameSlots and kind selects the event. kind 0x74 answers
 * "does this card respond" and 0x71 is the act. Scripts reach the slot they
 * were invoked on through CUR (two globals), not through (a, b).
 *
 * Address literals passed as plain ints (e.g. 0x583c68) are pointers into
 * the original image's data (here: string constants); pushing the literal
 * reproduces the original instruction without carrying the copyrighted data.
 */

#include "game/gameslot.h"

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
