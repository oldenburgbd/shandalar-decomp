/* Ordinary Shandalar game code (gamewire-reconstructed): plain .text
 * functions, neither card scripts nor their helpers. Address-derived names;
 * reccmp is the correctness check. */
#include "game/gamefns.h"

// FUNCTION: SHANDALAR 0x0051dcf2
int Game_Fn51dcf2(int a1, int a2, int a3)
{
    if (Game_Helper411d90(a1, a2) != 0
        && (g_gameCards[a3].flags & 0x2) != 0) {
        Game_Helper46d8c1(a1, a2, 2);
    }
    Game_Helper46db2e();
    if (Game_Helper411d90(a1, a2) != 0
        && (g_gameCards[a3].flags & 0x44) != 0) {
        Game_Helper46d8c1(a1, a2, 2);
    }
    return 0;
}
