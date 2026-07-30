/* Shandalar game layer: card behaviour scripts at 0x50xxxx.
 *
 * Split by address range into small translation units so MSVC codegen
 * perturbations stay confined to the unit being changed.
 *
 * Shared CUR/SRC macros live in game/gameslot.h. */

#include "game/gameslot.h"

// FUNCTION: SHANDALAR 0x0050f7c0
int Game_CardScript50f7c0(int a, int b, int kind)
{
    Game_Helper521cd0(a, b, kind, 3);
    return 0;
}

// FUNCTION: SHANDALAR 0x0050f7e6
int Game_CardScript50f7e6(int a, int b, int kind)
{
    Game_Helper521cd0(a, b, kind, 1);
    return 0;
}

// FUNCTION: SHANDALAR 0x0050f80c
int Game_CardScript50f80c(int a, int b, int kind)
{
    Game_Helper521cd0(a, b, kind, 5);
    return 0;
}

// FUNCTION: SHANDALAR 0x0050f832
int Game_CardScript50f832(int a, int b, int kind)
{
    Game_Helper521cd0(a, b, kind, 4);
    return 0;
}

// FUNCTION: SHANDALAR 0x0050f858
int Game_CardScript50f858(int a, int b, int kind)
{
    Game_Helper521cd0(a, b, kind, 2);
    return 0;
}
