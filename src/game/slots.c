/* Shandalar game layer: allocating a slot. */

#include "game/gameslot.h"

/* Finds a free column in `owner`'s row, initialises it from the arguments, and
 * returns its index (-1 on failure). 37 call sites. Parameter names describe
 * the code shape only: `owner` and `source` are the two checked against -1 up
 * front, and `source` names another slot in the same table that two fields are
 * copied from. */
// FUNCTION: SHANDALAR 0x004ba5a1
int Game_AllocSlot(int a, int b, int c, int owner, int source)
{
    int row;
    int slot;
    int card;

    if (a == -1 || owner == -1 || c <= 0) {
        return -1;
    }

    if (b == -1) {
        row = a;
    } else {
        row = owner;
    }

    slot = Game_FindFreeSlot(row, g_game93a7d4);
    if (slot == -1) {
        return -1;
    }

    /* 0x1002 for any nonzero row, 2 for row zero; the original computes this
     * branchlessly via cmp/adc/and. */
    g_gameSlots[row][slot].flags08 |= (row >= 1) ? 0x1002 : 2;
    g_gameSlots[row][slot].field50 = (unsigned char)a;
    g_gameSlots[row][slot].field04 = b;
    g_gameSlots[row][slot].field38 = c;
    g_gameSlots[row][slot].field0c = (unsigned char)owner;
    g_gameSlots[row][slot].field48 = source;

    if (source == -1) {
        g_gameSlots[row][slot].field64 = 0xef;
        g_game8c83f8 |= 2;
        return slot;
    }

    /* Pick whichever of the source slot's two id fields is the real one. */
    if (g_gameSlots[row][source].field6c == -1
        || g_gameSlots[row][source].field6c == g_game92e95c) {
        card = g_gameSlots[row][source].field3c;
    } else {
        card = g_gameSlots[row][source].field6c;
    }

    g_gameSlots[row][slot].field1e = g_gameSlots[row][source].field1e;
    if ((g_gameCards[card].flags & 0x40) != 0) {
        g_gameSlots[row][slot].field1e |= 0x40;
    }
    g_gameSlots[row][slot].field4c = g_gameCards[card].flags;

    if ((g_gameSlots[row][source].flags08 & 4) != 0) {
        if (g_game94d464 == 0x19) {
            g_gameSlots[row][slot].flags18 |= 0x100000;
        }
        if (g_game94d464 == 0x1a) {
            g_gameSlots[row][slot].flags18 |= 0x40000;
        }
    }

    if (g_gameCards[card].mask == g_game7a04c4
        || g_gameCards[card].mask == g_game8bcfe8
        || g_gameCards[card].mask == g_game8e32fc) {
        g_gameSlots[row][slot].field64 = g_gameSlots[row][source].field64;
    } else {
        /* Call result in the high half, the card's own mask in the low; the
         * shl is in the original. */
        g_gameSlots[row][slot].field64 =
            (Game_CombineMask(g_gameCards[card].mask, owner, source) << 16)
            | g_gameCards[card].mask;
    }

    g_game8c83f8 |= 2;
    return slot;
}
