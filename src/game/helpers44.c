/* Card-script helpers in the 0x0044xxxx range, replicated from the
 * archived reconstruction whose per-function reccmp scores were verified.
 * Names are addresses; meaning is not yet established.
 */
#include "game/helpers.h"

// FUNCTION: SHANDALAR 0x00441539
int Game_Helper441539(int a1, int a2, int a3)
{
    g_game77d170[a1][a2] += a3;
    g_game77d180[a1][3] += a3;
    return g_game77d170[a1][a2];
}

// FUNCTION: SHANDALAR 0x0044157b
int Game_Helper44157b(int a1, int a2, int a3)
{
    g_game77d170[a1][a2] -= -(0 - a3);
    g_game77d180[a1][3] -= -(0 - a3);
    return g_game77d170[a1][a2];
}

// FUNCTION: SHANDALAR 0x004415c5
int Game_Helper4415c5(int a1, int a2, int a3)
{
    g_game7806a0[a1][a2] += a3;
    g_game7806a0[a1][7] += a3;
    return g_game7806a0[a1][a2];
}

// FUNCTION: SHANDALAR 0x00441651
int Game_Helper441651(int a1, int a2, int a3)
{
    Game_Helper44157b(a1, a2, a3);
    Game_Helper4415c5(a1, a2, a3);
    return g_game7806a0[a1][a2];
}

// FUNCTION: SHANDALAR 0x0044818c
int Game_Helper44818c(int a1, int a2)
{
    int v4;

    for (v4 = 0; v4 < a2; v4++) {
        Game_Helper409a0b(a1);
        if (a1 != 0) {
            g_game74cdb4 = 0;
        }
    }
    g_game8e3ec8[a1] = a2;
    return 0;
}

// FUNCTION: SHANDALAR 0x0044a96c
int Game_Helper44a96c(int a1, int a2)
{
    int v4;
    int v8;

    v8 = 1;
    for (v4 = 0; g_game78e970[a1] > v4; v4++) {
        if (g_gameSlots[a1][v4].field6c == a2
            && (g_gameSlots[a1][v4].flags08 & 0x2) == 0) {
            v8++;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004419ca
int Game_Helper4419ca(int a1, int a2, int a3, int a4)
{
    int v4;

    v4 = 0;
    if (a4 == 0) {
        if (g_game765420[Game_Helper40dffd((signed char)g_gameSlots[a1][a2].field1e)] > 0) {
            return Game_Helper441699(a1, 7, g_game765420[Game_Helper40dffd((signed char)g_gameSlots[a1][a2].field1e)]);
        } else {
            return 1;
        }
    } else {
        v4 = Game_Helper441699(a1, a3, a4);
        if (v4 != 0) {
            if (g_game765420[Game_Helper40dffd((signed char)g_gameSlots[a1][a2].field1e)] > 0) {
                return Game_Helper441699(a1, 7, g_game765420[Game_Helper40dffd((signed char)g_gameSlots[a1][a2].field1e)] + a4);
            } else {
                return v4;
            }
        } else {
            return 0;
        }
    }
}

// FUNCTION: SHANDALAR 0x00440b8a
int Game_Helper440b8a(int a1)
{
    int iv;
    int ig;
    int ih;
    int ii;

    iv = 0;
    ih = 0;
    for (; iv < 2 && ih == 0; iv++) {
        ii = 0;
        for (; g_game78e970[iv] > ii && ih == 0; ii++) {
            if (Game_Helper411d90(iv, ii) != 0) {
                if ((g_gameCards[g_gameSlots[iv][ii].field6c].flags & 0x4) != 0 && (signed char)g_gameSlots[iv][ii].field50 == a1) {
                    ig = g_gameSlots[(signed char)g_gameSlots[iv][ii].field50][g_gameSlots[iv][ii].field04].field6c;
                    if (ig != -1) {
                        if ((g_gameCards[ig].flags & 0x2) != 0) {
                            ih = 1;
                        }
                    }
                }
            }
        }
    }
    return ih;
}
