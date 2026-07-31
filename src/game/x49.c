/* Card script at 0x00497962 -- a "remember every slot that targeted me, then
 * pay them all back" script.
 *
 * On kind 0x6e it appends the current (row, column) pair to a list held in the
 * slot's own record and bumps the count at +0x4c; on the 0xd7/0x7e teardown it
 * walks the list backwards, resolves each remembered slot to a number through
 * 0x004ecec6 and hands the result to 0x004883aa.
 *
 * The list lives at slot offset +0x74 as 19 (row, column) pairs of 8 bytes,
 * which is what the arithmetic proves: the count at +0x4c is guarded against
 * 0x13, and 0x74 + 19*8 = 0x10c closes flush on field10c. GameSlot in the
 * shared header names +0x74 and +0x78 as two scalars and the header must not
 * change, so the pairs are reached by indexing off field74/field78; the byte
 * arithmetic is identical either way.
 *
 * Local names encode the measured /Od stack slots: the prologue is
 * `sub esp, 0xc`, and the disassembly at 0x00497df1 stores the 0x004ecec6
 * result to [ebp-4] while 0x00497df9/0x00497e08 read the remembered row from
 * [ebp-0xc] and the column from [ebp-8].
 */

#include "game/gameslot.h"

// FUNCTION: SHANDALAR 0x00497962
int Game_CardScript497962(int a, int b, int kind)
{
    int v4;
    int v8;
    int vc;

    if (kind == 0x6e
        && g_gameSlots[g_game8e1aa8][g_game951bdc].field6c == g_game93a7d4
        && g_gameSlots[g_game8e1aa8][g_game951bdc].field38 != 0) {
        if (g_gameSlots[g_game8e1aa8][g_game951bdc].field48 == b
            && (signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field0c == a
            && g_gameSlots[a][b].field4c < 0x13) {
            (&g_gameSlots[a][b].field78)[g_gameSlots[a][b].field4c * 2] = g_game951bdc;
            (&g_gameSlots[a][b].field74)[g_gameSlots[a][b].field4c * 2] = g_game8e1aa8;
            g_gameSlots[a][b].field4c = g_gameSlots[a][b].field4c + 1;
        }
    } else if (g_game94d178 == 0xd7
               && g_game951bdc == b
               && g_game8e1aa8 == a
               && g_gameSlots[a][b].field4c != 0
               && a == g_game8c8afc) {
        if (kind == 0x7d) {
            g_game7a01e0 = g_game7a01e0 | 2;
        }
        if (kind == 0x7e) {
            do {
                g_gameSlots[a][b].field4c = g_gameSlots[a][b].field4c - 1;
                vc = (&g_gameSlots[a][b].field74)[g_gameSlots[a][b].field4c * 2];
                v8 = (&g_gameSlots[a][b].field78)[g_gameSlots[a][b].field4c * 2];
                if (g_gameSlots[vc][v8].field04 == -1) {
                    v4 = Game_Helper4ecec6(g_gameSlots[vc][v8].field38,
                                           0,
                                           g_game8c83f0[(signed char)g_gameSlots[vc][v8].field50]);
                } else {
                    v4 = Game_Helper4ecec6(g_gameSlots[vc][v8].field38,
                                           0,
                                           Game_Helper40d7b2((signed char)g_gameSlots[vc][v8].field50,
                                                             g_gameSlots[vc][v8].field04,
                                                             0x33,
                                                             -1));
                }
                Game_Helper4883aa(a, v4, a, b);
            } while (g_gameSlots[a][b].field4c != 0);
        }
    }
    return 0;
}
