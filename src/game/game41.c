/* Shandalar game code, addresses 0x0041xxxx. Reconstructed against the
 * original binary; formulations follow the measured-good salvage text.
 */
#include "game/gamefns.h"

// FUNCTION: SHANDALAR 0x004105b7
int Game_Fn4105b7(void)
{
    g_game8e266c = 0;
    g_game94d640 = g_game8e266c;
    g_game7a82dc = 0;
    return g_game8e266c;
}

// FUNCTION: SHANDALAR 0x0041116d
int Game_Fn41116d(void)
{
    int iv;   /* [ebp-4] */

    for (iv = 0; iv < 0x1f4; iv++) {
        g_game94eab0[iv] = -1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00412bae
int Game_Fn412bae(int a1, int a2, int a3)
{
    return Game_Fn412bff(a1, g_game5863b8 * a2 / 0x140, (a3 - (a3 & 1)) * g_game5863bc / 0xc8 + (a3 & 1));
}

// FUNCTION: SHANDALAR 0x00412bff
int Game_Fn412bff(int a1, int a2, int a3)
{
    g_game5a6074 = a2;
    g_game5a6078 = a3;
    g_game580d80 = -1;
    return Game_Helper412c37(a1, 1);
}

// FUNCTION: SHANDALAR 0x00413796
int Game_Fn413796(int a1, int a2, int a3, int a4, int a5)
{
    Game_Fn430e00(a1, a2, a3 + a1, a2, a5);
    Game_Fn430e00(a1, a4 + a2, a3 + a1, a4 + a2, a5);
    Game_Fn430e00(a3 + a1, a2, a3 + a1, a4 + a2, a5);
    return Game_Fn430e00(a1, a2, a1, a4 + a2, a5);
}

// FUNCTION: SHANDALAR 0x00413829
int Game_Fn413829(int a1, int a2, int a3, int a4, int a5, int a6)
{
    Game_Fn430e00(a1, a2, a3 + a1, a2, a6);
    Game_Fn430e00(a1, a4 + a2, a3 + a1, a4 + a2, a5);
    Game_Fn430e00(a3 + a1, a2, a3 + a1, a4 + a2, a6);
    return Game_Fn430e00(a1, a2 + 1, a1, a4 + a2, a5);
}

// FUNCTION: SHANDALAR 0x004170f8
int Game_Fn4170f8(void)
{
    int iv;   /* [ebp-4] */

    if (g_game580dac < 0xa) {
        for (iv = 0; iv < 7; iv++) {
            g_game5a6080[g_game580dac][iv] = 0;
        }
        g_game580dac++;
        return 1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0041716b
int Game_Fn41716b(int a1, int a2)
{
    if (g_game580dac > 0) {
        g_game5a6080[g_game580dac - 1][a1] += a2;
        return 1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004171b7
int Game_Fn4171b7(void)
{
    if (g_game580dac > 0) {
        g_game580dac--;
        return 1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004171eb
int Game_Fn4171eb(int a1)
{
    int iv;   /* [ebp-4] */

    if (g_game580dac > 0) {
        for (iv = 0; iv < 7; iv++) {
            g_game7806a0[a1][iv] += g_game5a6080[g_game580dac - 1][iv];
            g_game7806a0[a1][7] += g_game5a6080[g_game580dac - 1][iv];
        }
        return 1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00418a2d
int Game_Fn418a2d(int a1)
{
    int iv;   /* [ebp-4] */

    for (iv = 0; iv < 0x1f4; iv++) {
        if (g_gameCards[deck[iv] & 0xfff].mask == a1) {
            return 1;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0041d169
int Game_Fn41d169(void)
{
    if (g_game581804 != 0) {
        return 1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0041d1ef
int Game_Fn41d1ef(void)
{
    if (g_game581804 == 0) {
        return -1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0041d21d
int Game_Fn41d21d(void)
{
    if (g_game581804 == 0) {
        return 0;
    }
    return g_game5a7520;
}

// FUNCTION: SHANDALAR 0x00412b80
int Game_Fn412b80(int a1, int a2, int a3)
{
    g_game5a6074 = a2;
    g_game5a6078 = a3;
    return Game_Helper412c37(a1, 0);
}
