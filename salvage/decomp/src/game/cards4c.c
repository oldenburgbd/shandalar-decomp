/* Shandalar game layer: card behaviour scripts at 0x4Cxxxx.
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

// FUNCTION: SHANDALAR 0x004ceb50
int Game_CardScript4ceb50(int a, int b, int kind)
{
    return 0;
}
