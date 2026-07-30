/* Ordinary Shandalar game code (gamewire-reconstructed): plain .text
 * functions, neither card scripts nor their helpers. Address-derived names;
 * reccmp is the correctness check. */
#include "game/gamefns.h"

// FUNCTION: SHANDALAR 0x005224a0
int Game_Fn5224a0(int a1)
{
    return Game_Helper56a515(g_game8c776c, 0x8a9410 + 0x7d0, 0x1f4, 0x74bcc0 + 0x32, 0, 0x58e048);
}

// FUNCTION: SHANDALAR 0x005224d9
int Game_Fn5224d9(int a1)
{
    return Game_Helper56a515(g_game8c776c, 0x8a9410, 0x1f4, 0x74bcc0, 0, 0x58e04c);
}

// FUNCTION: SHANDALAR 0x00522660
int Game_Fn522660(void)
{
    g_game604420 = 0;
    return 0;
}

// FUNCTION: SHANDALAR 0x00522daa
int Game_Fn522daa(void)
{
    Game_Helper442f6a();
    Game_Helper4432ff();
    Game_Helper4431d8();
    Game_Helper465764();
    Game_Helper46488b(g_game7a00a0, g_game93a938);
    g_game93a938 = 0;
    g_game7a00a0 = 0;
    Game_Helper46297d();
    return Game_Helper56f8c1();
}

// FUNCTION: SHANDALAR 0x0052fac0
int Game_Fn52fac0(int a1, int a2)
{
    return a1 << a2 * 3;
}
