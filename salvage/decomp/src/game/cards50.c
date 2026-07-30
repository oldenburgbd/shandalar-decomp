/* Shandalar game layer: card behaviour scripts at 0x50xxxx.
 *
 * One of several translation units, split by address range. THIS IS NOT
 * COSMETIC: MSVC re-rolls codegen across a whole translation unit when
 * anything in it changes, so a single large file meant every new script
 * perturbed every earlier one. Smaller units confine that blast radius.
 *
 * It is also the closer match to the original, which evidently had one source
 * file per card: three byte-identical 18-byte handlers sit at three different
 * addresses rather than being one function reused.
 *
 * The shared CUR/SRC macros live in game/gameslot.h. */

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
