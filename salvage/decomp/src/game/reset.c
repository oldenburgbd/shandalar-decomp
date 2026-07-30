/* Shandalar game layer: clearing the tables and globals back to a start state. */

#include "game/gameslot.h"

/* Sets fifty-odd globals to 0, 1 or -1, clears five tables, and blanks sixteen
 * card records. No calls, no arguments, no return value.
 *
 * There is no way to tell from this function alone what "state" it resets --
 * the name says only what the code does. Two things it touches are already
 * known from elsewhere: the event log's terminator (g_gameEventWhere[0].a) and
 * the flags word g_game8c83f8 that Game_AllocSlot sets bit 1 of.
 *
 * Three details are the original's and are reproduced rather than tidied:
 *
 *  - The last three statements of the inner loop depend only on `i`, so they
 *    are executed eight times each to the same effect. Hoisting them out is
 *    what a person would do and is not what the binary does.
 *  - g_game8e26bc is set to -1 twice, once through the chain near the top and
 *    once on its own near the bottom.
 *  - The first loop re-reads g_game73c00c on every test rather than latching
 *    the bound, and writes the comparison with the bound on the left. Both are
 *    visible in the codegen: the bound lands in a register and the counter is
 *    the memory operand of the cmp.
 *
 * The chained assignments are likewise the original's shape, not a shorthand:
 * /Od stores to the rightmost target, reloads it, and stores that to the next,
 * which is exactly the load-after-store pattern in the disassembly.
 *
 * RESIDUAL, 95.15%: one difference, twice. In the loop at reset.c:66 the
 * original computes the card index as `mov eax,[g_game73c00c] / add eax,[i]`
 * and we emit the two the other way round. Writing the addends in either
 * order in the source produces our order -- /Od loads the local into eax and
 * adds the global to it whichever way it is spelled -- so this is codegen
 * canonicalisation, not a difference in what the source says. It costs a byte
 * each time (`mov eax,[mem32]` is five bytes, `mov eax,[ebp-8]` three), which
 * is where the two shifted jump displacements come from. */
// FUNCTION: SHANDALAR 0x00409680
void Game_ResetState(void)
{
    /* THE NAMES ARE LOAD-BEARING, and they read backwards on purpose: the
     * OUTER loop counts with `i` and the INNER one with `j`.
     *
     * Under MSVC 4.2 /Od a local's stack slot depends on its IDENTIFIER NAME,
     * and on neither declaration order nor order of first use. Probed with
     * (a,b) using a first, (b,a) using a first, and (a,b) using b first: all
     * three put a at [ebp-4] and b at [ebp-8]. Naming this function's counters
     * the natural way round measures 74.43%; this way round, 95.15%, with
     * nothing else changed.
     *
     * The rule behind it is NOT alphabetical -- (zz,aa) puts zz at [ebp-4] --
     * and is not established. Treat renaming as the lever and the mechanism as
     * unknown. See docs/quirks.md Q-021. */
    int i;
    int j;

    g_game8c776c = 0;
    g_game8c7468 = 1;

    for (j = g_game73c00c; g_game73c00c + 0x10 > j; j++) {
        g_gameCards[j].mask = -1;
    }

    for (i = 0; i < 2; i++) {
        g_game7bfee0[i] = 0;
        for (j = 0; j <= 7; j++) {
            g_game77d170[i][j] = g_game77ed80[i][j] = g_game7806a0[i][j] = 0;
            g_game74c790[i][0] = -1;
            g_game74cdc0[i][0] = -1;
            g_game7806e0[i] = 0;
        }
    }

    g_gameEventWhere[0].a = -1;

    for (j = 0; j < 0x10; j++) {
        if (g_gameCards[j + g_game73c00c].field00 == -1) {
            g_gameCards[j + g_game73c00c].mask = -1;
        }
    }

    g_game8c83f8 = 0;
    g_game74f1e0 = g_game78e5d8 = g_game8e26bc = -1;
    g_game74ccc4 = g_game74d26c = g_game77f440 = -1;
    g_game77f600 = 0;
    g_game7a81a8 = 0;
    g_game7bfe74 = g_game7beb20 = -1;
    g_game7beb04 = 0;
    g_game7bfe80 = 0;
    g_game7bf0ec = 0;
    g_game7befec = -1;

    if (g_game78cf04 == 0) {
        g_game939bf4 |= 2;
    } else {
        g_game939bf4 &= ~2;
    }

    g_game78e5e0 = 0;
    g_game77cf1c = 0x30;
    g_game77edc0 = -1;
    g_game92f414 = -1;
    g_game8bcf6c = -1;
    g_game7beff0 = -1;
    g_game93a794 = -1;
    g_game939410 = 0;
    g_game8c759c = 1;
    g_game93022c = 0;
    g_game77e590 = 0;
    g_game94e964 = 0;
    g_game8ca260 = 0;
    g_game93a798 = -1;
    g_game94e968 = 0;
    g_game74c960 = 1;
    g_game94d640 = 0;
    g_game8e266c = 0;
    g_game92e820 = 0;

    for (j = 0; j <= 7; j++) {
        g_game8e2690[j] = 0;
    }

    g_game8e26bc = -1;
    g_game8e2688 = -1;
    g_game8e2674 = -1;
    g_game939304 = 0;

    g_game7be740[g_game8c776c] = g_game7be740[g_game8c7468] = 0;
    g_game7a82dc = 0;
}
