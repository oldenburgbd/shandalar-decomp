/* Shandalar game layer: a state predicate. */

#include "game/gameslot.h"

/* Tests the global mode word against 52 values and returns 0 for any of them,
 * 1 otherwise -- except for 0x8e, which is special-cased ahead of the chain and
 * defers to a second global.
 *
 * Takes no arguments and reads only globals. 855 bytes of `cmp`/`je`, no calls.
 *
 * Two details say this chain was written by hand rather than generated. The
 * span 0x6a..0x89 is contiguous -- thirty-two consecutive values -- and is
 * still written out as thirty-two separate comparisons instead of a range
 * test. And the last two terms are 0xdc then 0xdb, out of order. Both are
 * reproduced; tidying either changes the generated code.
 *
 * 0x8e is tested twice, once at the top and once inside the chain. The second
 * test is unreachable. That is in the original too.
 *
 * Ghidra reports this function as 840 bytes. It is 855: the boundary lands
 * before the final `mov eax,1` and epilogue, so the inventory under-counts it
 * by 15 bytes. */
// FUNCTION: SHANDALAR 0x0046c6cf
int Game_Predicate46c6cf(void)
{
    if (g_game7befec == 0x8e) {
        if (g_game8ca260 != 0) {
            return 1;
        } else {
            return 0;
        }
    }

    if (g_game7befec == 0x6a ||
        g_game7befec == 0x6b ||
        g_game7befec == 0x6c ||
        g_game7befec == 0x6d ||
        g_game7befec == 0x6e ||
        g_game7befec == 0x6f ||
        g_game7befec == 0x70 ||
        g_game7befec == 0x71 ||
        g_game7befec == 0x72 ||
        g_game7befec == 0x73 ||
        g_game7befec == 0x74 ||
        g_game7befec == 0x75 ||
        g_game7befec == 0x76 ||
        g_game7befec == 0x77 ||
        g_game7befec == 0x78 ||
        g_game7befec == 0x79 ||
        g_game7befec == 0x7a ||
        g_game7befec == 0x7b ||
        g_game7befec == 0x7c ||
        g_game7befec == 0x7d ||
        g_game7befec == 0x7e ||
        g_game7befec == 0x7f ||
        g_game7befec == 0x80 ||
        g_game7befec == 0x81 ||
        g_game7befec == 0x82 ||
        g_game7befec == 0x83 ||
        g_game7befec == 0x84 ||
        g_game7befec == 0x85 ||
        g_game7befec == 0x86 ||
        g_game7befec == 0x87 ||
        g_game7befec == 0x88 ||
        g_game7befec == 0x89 ||
        g_game7befec == 0x8e ||
        g_game7befec == 0xc7 ||
        g_game7befec == 0xc8 ||
        g_game7befec == 0xc9 ||
        g_game7befec == 0xca ||
        g_game7befec == 0xcb ||
        g_game7befec == 0xcc ||
        g_game7befec == 0xcd ||
        g_game7befec == 0xce ||
        g_game7befec == 0xcf ||
        g_game7befec == 0xd2 ||
        g_game7befec == 0xd3 ||
        g_game7befec == 0xd4 ||
        g_game7befec == 0xd5 ||
        g_game7befec == 0xd6 ||
        g_game7befec == 0xd7 ||
        g_game7befec == 0xd8 ||
        g_game7befec == 0xd9 ||
        g_game7befec == 0xdc ||
        g_game7befec == 0xdb) {
        return 0;
    }
    return 1;
}
