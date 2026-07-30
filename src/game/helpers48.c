/* Card-script helpers in the 0x0048xxxx range, replicated from the
 * archived reconstruction whose per-function reccmp scores were verified.
 * Names are addresses; meaning is not yet established.
 */
#include "game/helpers.h"

// FUNCTION: SHANDALAR 0x0048808b
int Game_Helper48808b(int a1, int a2)
{
    int v4;

    v4 = 0;
    if ((signed char)g_gameCards[a1].field29 == 0xd
        && g_gameCards[a1].mask == g_game93a77c[a2]) {
        v4 = 1;
    }
    if ((signed char)g_gameCards[a1].field29 == 0xc
        && (1 << a2 & 0x13) != 0) {
        v4 = 1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00488147
int Game_Helper488147(int a1, int a2, int a3)
{
    int v4;
    int v8;

    v8 = 0;
    if (g_gameSlots[a1][a2].field6c != -1) {
        v4 = g_gameSlots[a1][a2].field6c;
    } else {
        v4 = g_gameSlots[a1][a2].field3c;
    }
    if (a1 == -1 || a2 == -1) {
        return 0;
    }
    if ((signed char)g_gameCards[v4].field29 == 0xd
        && g_gameCards[v4].mask == g_game93a77c[a3]) {
        v8 = 1;
    }
    if ((signed char)g_gameCards[v4].field29 == 0xc
        && (1 << a3 & 0x13) != 0) {
        v8 = 1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004882b5
int Game_Helper4882b5(int a1)
{
    int iv;
    int ig;
    int ih;

    iv = 0;
    for (ih = 0; g_game78e970[a1] > ih; ih++) {
        ig = g_gameSlots[a1][ih].field6c;
        if (Game_Helper411d90(a1, ih) != 0
            && (signed char)g_gameCards[ig].field29 == 8) {
            if (g_gameCards[ig].mask == 0x21d) {
                iv |= 1;
            }
            if (g_gameCards[ig].mask == 0x21f) {
                iv |= 2;
            }
            if (g_gameCards[ig].mask == 0x220) {
                iv |= 4;
            }
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004883aa
int Game_Helper4883aa(int a1, int a2, int a3, int a4)
{
    int iv;
    int ig;
    int ih;

    g_game8c83f0[a1] += a2;
    if ((g_game7bfe7a & 0x20) != 0) {
        ig = g_game7bf4b8;
        ih = g_game8e2fc4;
        Game_Helper40e646();
        g_game7bf4b8 = a1;
        g_game8e2fc4 = -1;
        g_game8e4060 = a2;
        Game_Helper410efc(g_game7a82e0, 0xe0, 0x7a0370, 0);
        g_game7bf4b8 = ig;
        g_game8e2fc4 = ih;
        Game_Helper40e6fc();
    }
    if (g_game8c8650[a1] != 0) {
        for (iv = 0; a2 > iv; iv++) {
            Game_Helper409a0b(a1);
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00488474
int Game_Helper488474(int a1, int a2)
{
    if (a1 == -1 || a2 == -1) {
        return 0;
    }
    Game_Helper488789(a1, a2);
    Game_Helper4884c1(a1, a2);
    return 0;
}

// FUNCTION: SHANDALAR 0x004885e4
int Game_Helper4885e4(int a1, int a2)
{
    if (a1 == -1 || a2 == -1) {
        return 0;
    }
    Game_Helper488b70(a1, a2, (int)Game_Callback48889d, -1);
    Game_Helper488638(a1, a2);
    return 0;
}

// FUNCTION: SHANDALAR 0x00488cc4
int Game_Helper488cc4(int a1, int a2)
{
    int v4;
    int v8;

    v4 = 0;
    if (g_gameSlots[a1][a2].field6c == -1 || g_gameSlots[a1][a2].field6c == g_game92e95c) {
        v8 = g_gameCards[g_gameSlots[a1][a2].field3c].flags;
    } else {
        v8 = g_gameCards[g_gameSlots[a1][a2].field6c].flags;
    }
    if ((v8 & 0x8) != 0) {
        v4 |= 0x100000;
    }
    if ((v8 & 0x10) != 0) {
        v4 |= 0x40000;
    }
    if ((v8 & 0x20) != 0) {
        v4 |= 0x80000;
    }
    if ((v8 & 0x4) != 0) {
        v4 |= 0x20000;
    }
    if ((v8 & 0x40) != 0) {
        v4 |= 0x10000;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0048c204
int Game_Helper48c204(int a1, int a2)
{
    int v4;
    int v8;

    v4 = g_gameSlots[a1][a2].field4c;
    v8 = Game_FindFreeSlot(v4, g_game8bd38c);
    if (v8 != -1) {
        g_gameSlots[v4][v8].flags08 |= 2;
        g_gameSlots[v4][v8].flags18 |= 8;
        g_gameSlots[v4][v8].field38 = 1;
        g_gameSlots[v4][v8].field1e = g_gameSlots[a1][a2].field1e;
        g_gameSlots[v4][v8].field64 = Game_CombineMask(0x1bc, a1, a2) * 65536 | 0x1bc;
        g_gameSlots[v4][v8].field0c = a1;
        g_gameSlots[v4][v8].field48 = a2;
        g_gameSlots[v4][v8].field50 = v4;
        g_gameSlots[v4][v8].field04 = -1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00485185
int Game_Helper485185(int a1, int a2, int a3)
{
    int v4;

    v4 = 0;
    while (g_game8e2e10[a1][v4] != -1 && v4 < 0x10) {
        v4++;
    }
    if (v4 < 0x10 && g_gameSlots[a2][a3].field6c != -1) {
        g_game8e2e10[a1][v4] = g_gameSlots[a2][a3].field6c;
        g_gameSlots[a2][a3].flags18 |= 8;
        Game_Helper46d8c1(a2, a3, 2);
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: SHANDALAR 0x00485308
int Game_Helper485308(int a1)
{
    int v4;
    int v8;

    v8 = 0;
    while (g_game8e2e10[a1][v8] != -1 && v8 < 0x10) {
        v8++;
    }
    if (v8 < 0x10) {
        v4 = Game_Helper409a0b(a1);
        g_game8e2e10[a1][v8] = g_gameSlots[a1][v4].field6c;
        g_game8e3ec8[a1]--;
        Game_Helper56ca10(0x586ca0, 0x586c94);
        if (a1 == g_game8c776c) {
            Game_Helper558538(a1, a1, v4, -1, -1, 0x8aa920, 0);
        } else {
            Game_Helper558538(a1, a1, v4, -1, -1, 0x8aa920 + 0x12c, 0);
        }
        g_gameSlots[a1][v4].field6c = -1;
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: SHANDALAR 0x004869b0
int Game_Helper4869b0(void)
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
                if ((g_gameCards[g_gameSlots[iv][ii].field6c].flags & 0x4) != 0 && (signed char)g_gameSlots[iv][ii].field50 != -1) {
                    ig = g_gameSlots[(signed char)g_gameSlots[iv][ii].field50][g_gameSlots[iv][ii].field04].field6c;
                    if (ig != -1) {
                        if ((g_gameCards[ig].flags & 0x1) != 0) {
                            ih = 1;
                        }
                    }
                }
            }
        }
    }
    return ih;
}
