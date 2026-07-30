/* Ordinary Shandalar game code (gamewire-reconstructed): plain .text
 * functions, neither card scripts nor their helpers. Address-derived names;
 * reccmp is the correctness check. */
#include "game/gamefns.h"

// FUNCTION: SHANDALAR 0x00540ee8
int Game_Fn540ee8(int a1, int a2)
{
    int iv;   /* [ebp-4] */

    if (a1 == 0 || a2 == 0) {
        iv = 0;
    } else {
        if ((a1 - (a2 - 1)) / a2 <= 0) {
        }
        iv = 0;
    }
    return iv;
}

// FUNCTION: SHANDALAR 0x00544e3e
int Game_Fn544e3e(int a1)
{
    Game_Helper4f8400(3);
    g_game8bd344 = a1;
    g_game8bd370 = 0x8a93d0;
    g_game8bd374 = 0x2a000c;
    if (Game_Helper578c04() != 0) {
        return Game_Helper5044ac(g_game8bd35c);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00548eb1
int Game_Fn548eb1(int a1, int a2, int a3, int a4, int a5, int a6)
{
    int iv;   /* [ebp-4] */
    int ig;   /* [ebp-8] */

    ig = a4 + a2;
    iv = a5 + a3;
    Game_Helper5796c0(a1, a2, a3, ig, a3, a6);
    Game_Helper5796c0(a1, ig, a3, ig, iv, a6);
    Game_Helper5796c0(a1, ig, iv, a2, iv, a6);
    return Game_Helper5796c0(a1, a2, iv, a2, a3, a6);
}

// FUNCTION: SHANDALAR 0x0054a880
int Game_Fn54a880(void)
{
    g_game6506f0 = 0;
    g_game6506f4 = 0;
    return 0;
}

// FUNCTION: SHANDALAR 0x0054a89f
int Game_Fn54a89f(int a1, int a2, int a3, int a4, int a5)
{
    int iv;   /* [ebp-4] */
    int ig;   /* [ebp-8] */

    g_game651740[g_game6506f0] = a2;
    g_game651f40[g_game6506f0] = a3;
    g_game650700[g_game6506f0] = a4;
    g_game650f40[g_game6506f0] = a5;
    if (g_game6506f0 == 0) {
        g_game6527c0[g_game6506f0] = -1;
        g_game6506f0++;
        return g_game6506f0;
    } else {
        ig = g_game6506f4;
        iv = -1;
        while (g_game650700[ig] < a4 && ig != -1) {
            iv = ig;
            ig = g_game6527c0[ig];
        }
        g_game6527c0[g_game6506f0] = ig;
        if (iv != -1) {
            g_game6527c0[iv] = g_game6506f0;
        } else {
            g_game6506f4 = g_game6506f0;
        }
        g_game6506f0++;
        return 0;
    }
}

// FUNCTION: SHANDALAR 0x00544d9c
int Game_Fn544d9c(int a1)
{
    Game_Helper4f8400(1);
    g_game8bd344 = a1;
    g_game8bd370 = 0x8a93d0;
    g_game8bd374 = 0x2a000c;
    if (Game_Helper578c04() != 0) {
        return Game_Helper5040da(g_game8bd35c);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00544ded
int Game_Fn544ded(int a1)
{
    Game_Helper4f8400(2);
    g_game8bd344 = a1;
    g_game8bd370 = 0x8a93d0;
    g_game8bd374 = 0x2a000c;
    if (Game_Helper578c04() != 0) {
        return Game_Helper50425c(g_game8bd35c);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0054cdbd
int Game_Fn54cdbd(void)
{
    g_game6527b0 = -1;
    g_game590764 = g_game6527b0;
    g_game590768 = 1;
    if (g_game8bd200 == 0) {
        Game_Helper57c800(1, g_game589de8);
        Game_Helper579890(g_game5832dc, 0, 0, g_game5863b8, g_game5863bc, g_game5832b4, 0, 0);
        Game_Helper50027e(0, 4);
    }
    g_game73eaa0 = 0;
    if (g_game650f28 == 0) {
        Game_Helper579890(g_game5832dc, 0, Game_Fn5501fe(0x148), Game_Fn5501fe(0x280), Game_Fn5501fe(0x1e0) - Game_Fn5501fe(0x148), g_game58332c, 0, 0);
        Game_Helper579890(g_game5832dc, 0, 0, Game_Fn5501fe(0x40), Game_Fn5501fe(0x148), g_game583354, 0, 0);
        g_game650f28 = 1;
        return Game_Helper579890(g_game5832dc, 0, 0, Game_Fn5501fe(0x40), Game_Fn5501fe(0x148), g_game583354, 0, 0);
    }
    return 0;
}
