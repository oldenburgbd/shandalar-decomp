/* Shandalar game layer: clearing the tables and globals back to a start state. */

#include "game/gameslot.h"

/* Straight-line reset of ~50 globals plus five table-clearing loops and a
 * pass over sixteen card records. Several oddities are the original's and
 * must stay: the inner loop redoes three i-only statements eight times,
 * g_game8e26bc is assigned -1 twice, the first loop re-reads g_game73c00c
 * each iteration with the bound written on the left, and the chained
 * assignments reproduce /Od's store-then-reload pattern.
 *
 * Known residual (95.15%): in the loop over the sixteen card records the
 * original adds global+local and we emit local+global; source spelling
 * cannot flip it, costing one byte per occurrence. */
// FUNCTION: SHANDALAR 0x00409680
void Game_ResetState(void)
{
    /* Counter names are deliberately swapped (outer `i`, inner `j`): MSVC 4.2
     * /Od picks stack slots by identifier name, and this naming measures
     * 95.15% vs 74.43% the natural way round. See docs/quirks.md Q-021. */
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
