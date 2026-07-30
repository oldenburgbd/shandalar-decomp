/* Shandalar game layer: allocating a slot. */

#include "game/gameslot.h"

/* Finds a free column in `owner`'s row, fills it from the five arguments, and
 * returns its index. 37 call sites.
 *
 * The five parameters are named by the shape of the code and nothing more.
 * `owner` and `source` are the two that are checked against -1 up front and the
 * two that index the table; `source` is also a slot index in the same table,
 * which the new slot copies two fields from. What any of them mean is not
 * established.
 *
 * The last block reads a card record and either copies the source's field74 or
 * builds a new one, depending on whether the card's mask matches any of three
 * globals. */
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

    /* 0x1002 for any row but zero, 2 for row zero. The original computes it
     * without a branch, through cmp/adc/and. */
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

    /* Which of the source slot's two id fields to take. */
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
        /* The call's result occupies the high half; the card's own mask the
         * low. The shl is in the original and is easy to lose. */
        g_gameSlots[row][slot].field64 =
            (Game_CombineMask(g_gameCards[card].mask, owner, source) << 16)
            | g_gameCards[card].mask;
    }

    g_game8c83f8 |= 2;
    return slot;
}
