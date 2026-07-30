/* Ordinary Shandalar game code (gamewire-reconstructed): plain .text
 * functions, neither card scripts nor their helpers. Address-derived names;
 * reccmp is the correctness check. */
#include "game/gamefns.h"

// FUNCTION: SHANDALAR 0x00485297
int Game_Fn485297(int a1, int a2)
{
    int iv;   /* [ebp-4] */

    iv = 0;
    while (g_game8e2e10[a1][iv] != -1 && iv < 0x10) {
        iv++;
    }
    if (iv < 0x10) {
        g_game8e2e10[a1][iv] = a2;
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: SHANDALAR 0x00487077
int Game_Fn487077(int a1, int a2)
{
    int iv;   /* [ebp-4] */
    int ig;   /* [ebp-8] */

    if (a2 == a1 || a1 < 1 || a1 > 5 || a2 < 1 || a2 > 5) {
        return -1;
    } else {
        if (a2 > a1) {
            iv = a1;
            a1 = a2;
            a2 = iv;
        }
        iv = 1;
        ig = 0;
        while (iv <= a2) {
            iv++;
            ig += iv;
        }
        return a2 + a2 * 4 + 0xfffffffb + (a1 - 1) - ig;
    }
}
