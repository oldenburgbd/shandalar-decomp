/* Ordinary Shandalar game code (gamewire-reconstructed): plain .text
 * functions, neither card scripts nor their helpers. Address-derived names;
 * reccmp is the correctness check. */
#include "game/gamefns.h"

// FUNCTION: SHANDALAR 0x005307c3
int Game_Fn5307c3(int a1)
{
    return g_game77c020[a1];
}

// FUNCTION: SHANDALAR 0x00530b7e
int Game_Fn530b7e(void)
{
    return 0;
}

// FUNCTION: SHANDALAR 0x0053133f
int Game_Fn53133f(void)
{
    int iv;   /* [ebp-4] */
    int ig;   /* [ebp-8] */
    int ih;   /* [ebp-12] */

    iv = 0;
    ig = 1;
    for (; ig <= 5; ig++) {
        if ((g_game789904 & 0x13) == 0) {
            iv++;
        }
    }
    ih = Game_Helper522508(iv);
    ig = 1;
    for (; ig <= 5 && ih != 0; ig++) {
        if ((g_game789904 & 0x13) == 0) {
            ih--;
        }
    }
    return ig;
}

// FUNCTION: SHANDALAR 0x0053182b
int Game_Fn53182b(void)
{
    int iv;   /* [ebp-4] */
    int ig;   /* [ebp-8] */
    int ih;   /* [ebp-12] */

    iv = 0;
    ig = 0;
    for (; ig < 0xc; ig++) {
        if ((g_game78cee4 & 0x13) == 0) {
            iv++;
        }
    }
    if (iv == 0) {
        return -1;
    } else {
        ih = Game_Helper522508(iv);
        ig = 0;
        for (; ig < 0xc && ih != 0; ig++) {
            if ((g_game78cee4 & 0x13) == 0) {
                ih--;
            }
        }
        return ig;
    }
}

// FUNCTION: SHANDALAR 0x005318e3
int Game_Fn5318e3(void)
{
    return 0;
}

// FUNCTION: SHANDALAR 0x00536039
int Game_Fn536039(int a1, int a2, int a3)
{
    if (a1 != 0) {
        Game_Helper46537b(a1);
    }
    if (a2 != 0) {
        Game_Helper46537b(a2);
    }
    if (a3 != 0) {
        return Game_Helper46537b(a3);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x005377df
int Game_Fn5377df(int a1, int a2, int a3, int a4)
{
    if (a1 != 0) {
        Game_Helper46537b(a1);
    }
    if (a2 != 0) {
        Game_Helper46537b(a2);
    }
    if (a3 != 0) {
        Game_Helper46537b(a3);
    }
    if (a4 != 0) {
        return Game_Helper46537b(a4);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00532809
int Game_Fn532809(int a1)
{
    Game_Helper57c7e0(1, a1);
    return Game_Helper579890(g_game5832dc, 0, 0, g_game5863b8, g_game5863bc, g_game5832b4, 0, 0);
}
