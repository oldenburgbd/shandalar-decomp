/* Card-script helpers in the 0x004bxxxx range, replicated from the
 * archived reconstruction whose per-function reccmp scores were verified.
 * Names are addresses; meaning is not yet established.
 */
#include "game/helpers.h"

// FUNCTION: SHANDALAR 0x004b183c
int Game_Helper4b183c(int a1, int a2)
{
    if ((g_gameSlots[a1][a2].flags08 & 0x10) == 0) {
        g_gameSlots[a1][a2].flags08 |= 0x10;
        if ((g_gameCards[g_gameSlots[a1][a2].field6c].flags3c & 0x1000) != 0) {
            g_game93a794 = -1;
        }
        Game_Helper40e158(a1, a2, 0x81);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004ba2ed
int Game_Helper4ba2ed(int a1)
{
    g_gameCards[a1].mask = -1;
    return a1 * 8;
}

// FUNCTION: SHANDALAR 0x004ba41b
int Game_Helper4ba41b(int a1, int a2)
{
    int v4;
    int v8;

    Game_Helper56ca10(0x589298, 0x589284);
    Game_Helper57e820(0x5aa4f0, 0x8aa920);
    g_game7bf4b8 = a1;
    g_game8e2fc4 = a2;
    Game_Helper410efc(g_game7a82e0, 0xd4, 0x5aa4f0, 0);
    g_gameSlots[a1][a2].field6c = -1;
    Game_Helper46e159(a1, a2);
    if ((g_gameSlots[a1][a2].flags18 & 0x10) != 0) {
    } else {
        if ((g_gameSlots[a1][a2].flags08 & 0x1000) != 0) {
            v8 = 1;
        } else {
            v8 = 0;
        }
        v4 = Game_FindFreeSlot(v8, g_gameSlots[a1][a2].field3c);
        if (v4 != -1) {
            Game_Helper558321(v8, v4, 8, 2);
        }
        g_game8e3ec8[v8]++;
    }
    /* The original returns no value here: it falls off the end and its
     * epilogue leaves eax holding whatever the last expression produced.
     * Ghidra types FUN_004ba41b as void; a `return 0;` would add the
     * `xor eax,eax` + `jmp` pair that the original does not have. */
}

// FUNCTION: SHANDALAR 0x004bab38
int Game_Helper4bab38(int a1, int a2, int a3, int a4)
{
    Game_AllocSlot(a1, -1, a2, a3, a4);
    return 0;
}
