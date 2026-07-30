/* Ordinary Shandalar game code (gamewire-reconstructed): plain .text
 * functions, neither card scripts nor their helpers. Address-derived names;
 * reccmp is the correctness check. */
#include "game/gamefns.h"

// FUNCTION: SHANDALAR 0x004f21bb
int Game_Fn4f21bb(int a1, int a2)
{
    int iv;   /* [ebp-4] */
    int ig;   /* [ebp-8] */
    int ih;   /* [ebp-12] */

    ig = a1 - g_game73e9d0;
    ih = a2 - g_game73e9d4;
    if (ih > 0) {
        if (ig > 0) {
            iv = 1;
        } else {
            iv = 2;
        }
    } else {
        if (ig > 0) {
            iv = 0;
        } else {
            iv = 3;
        }
    }
    return iv;
}

// FUNCTION: SHANDALAR 0x004f23bc
int Game_Fn4f23bc(int a1, int a2)
{
    return Game_Helper564e70(a1, a2, 0x77e6e0, g_game7894f0 * 25 + 0x77e220, Game_Helper428615(1 << (g_game5863b0 & 3)));
}

// FUNCTION: SHANDALAR 0x004f716f
int Game_Fn4f716f(void)
{
    return 0;
}

// FUNCTION: SHANDALAR 0x004f7f99
int Game_Fn4f7f99(int a1, int a2)
{
    return Game_Helper4f7fb9(a1, a2);
}
