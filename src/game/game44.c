/* Shandalar game code, addresses 0x0044xxxx. Reconstructed against the
 * original binary; formulations follow the measured-good salvage text.
 */
#include "game/gamefns.h"

// FUNCTION: SHANDALAR 0x00441607
int Game_Fn441607(int a1, int a2, int a3)
{
    g_game7806a0[a1][a2] -= -(0 - a3);
    g_game7806a0[a1][7] -= -(0 - a3);
    return g_game7806a0[a1][a2];
}

// FUNCTION: SHANDALAR 0x00441b70
int Game_Fn441b70(int a1)
{
    return Game_Helper57e83e(a1, 0x20, Game_Helper57e832(a1));
}

// FUNCTION: SHANDALAR 0x004432e0
int Game_Fn4432e0(void)
{
    g_game5a8b30 = 0;
    return 1;
}

// FUNCTION: SHANDALAR 0x00446a57
int Game_Fn446a57(void)
{
    Game_Helper579890(g_game583a74, 0, 0, g_game5863b8, 0x80, g_game583304, 0, 0);
    return Game_Helper579240(9);
}

// FUNCTION: SHANDALAR 0x00441260
int Game_Fn441260(int a1, int a2, int a3)
{
    g_game77ed80[a1][a2] += a3;
    g_game77ed80[a1][7] += a3;
    return g_game77ed80[a1][a2];
}

// FUNCTION: SHANDALAR 0x004412a2
int Game_Fn4412a2(int a1, int a2, int a3)
{
    g_game77ed80[a1][a2] -= -(0 - a3);
    g_game77ed80[a1][7] -= -(0 - a3);
    return g_game77ed80[a1][a2];
}
