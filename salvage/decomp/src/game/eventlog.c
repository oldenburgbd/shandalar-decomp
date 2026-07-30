/* Shandalar game layer: appending to the event log. */

#include "game/gameslot.h"

/* Records one event across five parallel arrays, and where the event concerns a
 * slot worth preserving, first copies that slot aside into a free one.
 *
 * 22 call sites. Returns 0 on every path, including the two that do nothing,
 * so the return value carries nothing.
 *
 * The names are shape-only. `kind` is the value tested against 0x71 and 0x7e,
 * `row` and `col` index the slot table, and `extra` is stored unexamined. What
 * any of them mean is unestablished.
 *
 * The log is capped at 32 entries and silently drops everything after that --
 * the count is tested before use and never reset here. */
// FUNCTION: SHANDALAR 0x0040f77c
int Game_LogEvent(int row, int col, int kind, int arg, int extra)
{
    int slot;
    int saved;
    int ok;

    if (g_gameEventCount >= GAME_EVENT_MAX) {
        return 0;
    }

    g_gameEventWord[g_gameEventCount] = g_gameSlots[row][col].field6c;
    g_gameEventWord[g_gameEventCount] |= kind << 16;
    g_gameEventWord[g_gameEventCount] |= arg << 24;

    if (kind == 0x71 || kind == 0x7e || g_gameSlots[row][col].field6c <= 4) {
        slot = col;
        ok = 1;
    } else {
        slot = Game_FindFreeSlot(row, g_game92e95c);
        if (slot == -1) {
            ok = 0;
        } else {
            saved = g_gameSlots[row][slot].field54;
            Game_CopyBlock(&g_gameSlots[row][slot], &g_gameSlots[row][col],
                           GAME_SLOT_BYTES);
            g_gameSlots[row][slot].field6c = g_game92e95c;
            g_gameSlots[row][slot].field14 = 0;
            g_gameSlots[row][slot].field68 = 0;

            /* The same "which of the two ids is real" choice Game_AllocSlot
             * makes, written out again rather than shared. */
            if (g_gameSlots[row][col].field6c == -1) {
                g_gameSlots[row][slot].field3c = g_gameSlots[row][col].field3c;
            } else {
                g_gameSlots[row][slot].field3c = g_gameSlots[row][col].field6c;
            }

            g_gameSlots[row][slot].field64 = g_gameSlots[row][col].field64;
            g_gameSlots[row][slot].flags08 |= 2;
            g_gameSlots[row][slot].field10c = row;
            g_gameSlots[row][slot].field110 = col;
            g_gameSlots[row][slot].field54 = saved;
            ok = 1;
        }
    }

    if (ok == 0) {
        return 0;
    }

    g_gameEventWhere[g_gameEventCount].a = row;
    g_gameEventWhere[g_gameEventCount].b = slot;
    g_gameEventWhat[g_gameEventCount].a = (signed char)g_gameSlots[row][col].field50;
    g_gameEventWhat[g_gameEventCount].b = g_gameSlots[row][col].field04;

    if (g_game94d178 == -1) {
        g_gameEventMode[g_gameEventCount] = g_game94d464;
    } else {
        g_gameEventMode[g_gameEventCount] = g_game94d178;
    }

    if (g_game8bd200 != 1) {
        g_gameEventExtra[g_gameEventCount] = extra;
    }

    g_gameEventCount++;
    /* One past the last entry is marked, which is why the array is written
     * with a spare slot at the end. */
    g_gameEventWhere[g_gameEventCount].a = -1;
    return 0;
}
