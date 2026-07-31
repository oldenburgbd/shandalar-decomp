/* Card behaviour script at 0x00438bce, newly derived from Ghidra's
 * decompilation (reports/ghidra/decomp/00438bce.c).
 *
 * Shape is the usual int f(a, b, kind). The 0x6d branch loops on the target
 * picker until the chosen slot's linked card carries card-flag 0x1, retrying
 * with a message otherwise; 0x72 re-validates the stored pick and clears the
 * pending marker.
 *
 * Address literals (0x5835d8, 0x8aaa4c, ...) are pointers into the original
 * image's data. Pushing the literal reproduces the original instruction
 * without carrying the data itself -- same convention as cards43.c.
 *
 * The 20-argument shapes of Game_Helper4c0efa / Game_Helper4bea85 with a
 * nested Game_Helper488cc4(a, b) are the arities the shared header records
 * (taken from `add esp, N` at the call sites); Ghidra splits the pushes
 * between the two calls and renders 488cc4 with 14 arguments, which the
 * cleanup after it (add esp, 8) contradicts. cards43.c already carries the
 * measured-good form of this exact pair.
 */

#include "game/gameslot.h"

/* Referenced by this TU only; not in the shared headers. Addresses from the
   disassembly; Sleep is the kernel32 import, declared here rather than via
   windows.h so this TU's declaration set stays minimal. */
void __stdcall Sleep(unsigned long dwMilliseconds);
int Game_Helper43919e(void);
int Game_Helper4c442a(void);
int Game_Helper558356(int a1);

// FUNCTION: SHANDALAR 0x00438bce
int Game_CardScript438bce(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;
    int ii;
    int ij;

    if (kind == 0x73) {
        Game_Helper4c4263(0);
        if ((g_gameSlots[a][b].flags08 & 0x20010) == 0
            && Game_Helper441699(a, 3, 2) != 0
            && Game_Helper43919e() != 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x6d) {
        Game_Helper4143e0(a, 3, 2);
        if (g_game8c83ec != 1) {
            ii = 0;
            do {
                Game_Helper56ca10(0x5835d8, 0x5835c8);
                if (Game_Helper4c0efa(a, 2, 2, 0x200, 4, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &iv) == 0) {
                    g_game8c83ec = 1;
                } else if ((signed char)g_gameSlots[iv][ig].field50 == -1
                           || g_gameSlots[iv][ig].field04 == -1
                           || (g_gameCards[g_gameSlots[(signed char)g_gameSlots[iv][ig].field50][g_gameSlots[iv][ig].field04].field6c].flags & 0x1) == 0) {
                    if (g_game8bd200 == 1) {
                        Game_Helper4c442a();
                    } else {
                        Game_Helper558356(0x8aaa4c);
                        Sleep(0x9c4);
                        Game_Helper558356(0x5835e8);
                    }
                } else {
                    g_gameSlots[a][b].field74 = iv;
                    g_gameSlots[a][b].field78 = ig;
                    g_gameSlots[a][b].field36 = 1;
                    g_gameSlots[a][b].flags08 |= 0x10;
                    ii = 1;
                    if (a == g_game8c7468) {
                        if (iv == g_game8c776c) {
                            g_game94d63c += 0x30;
                        } else {
                            g_game94d63c -= 0x30;
                        }
                    }
                }
            } while (g_game8c83ec != 1 && ii == 0);
        }
    }
    if (kind == 0x72) {
        iv = g_gameSlots[a][b].field74;
        ig = g_gameSlots[a][b].field78;
        if (Game_Helper4bea85(iv, ig, 0, a, 2, 2, 0x200, 4, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0) == 0) {
            g_game8c83ec = 1;
        } else {
            ih = g_gameSlots[iv][ig].field04;
            ij = g_gameSlots[(signed char)g_gameSlots[iv][ig].field50][ih].field6c;
            if ((signed char)g_gameSlots[iv][ig].field50 == -1
                || ih == -1
                || ij == -1
                || (g_gameCards[ij].flags & 0x1) == 0) {
                g_game8c83ec = 1;
            } else {
                Game_Helper46d8c1(iv, ig, 2);
            }
        }
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
    }
    return 0;
}
