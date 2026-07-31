/* Card script at 0x004d0dc7 -- an "animate a creature out of a graveyard"
 * shaped script: it picks a side, prompts for a card in that side's
 * g_game94d930 list, remembers (side, index) plus the slot it created, and
 * later either completes or cancels the move.
 *
 * The local list below is not a guess. The prologue is `sub esp, 0xbd4`, and
 * 16 + 4 + 100 + 4 + 900 + 2000 + 4 = 3028 = 0xbd4 accounts for it exactly, so
 * this declaration ORDER is the original's: MSVC 4.2 /Od gives the first local
 * [ebp-4] and works downward. Ghidra's `cVar1` is not an extra local -- the
 * disassembly at 0x004d1b82 stores it to [ebp-0xbd4], i.e. `io` reused.
 *
 * Slot fields +0x7c and +0x80 fall inside GameSlot's unknown07c blob; the
 * shared headers must not change, so they are reached through that member.
 *
 * Address literals passed as plain ints (0x58a220, 0x8aa920, 0x8bce20) are
 * pointers into the original image's data -- pushing the literal reproduces
 * the original instruction without carrying the data. Same convention as
 * cards43.c and game46.c.
 */

#include "game/gameslot.h"

/* Referenced by this TU only; not in the shared headers this file includes.
   Addresses from the disassembly. */
extern int g_game7be9e8[32];
extern int g_game7be9ec;
int Game_Helper44e4ed(int a1, int a2);
int Game_Helper453770(char *a1);
int Game_Helper46e5d2(int a1, int a2);
int Game_Helper56a45f(int a1, int a2, int a3, int a4, int a5, int a6, int a7);

int sprintf(char *buf, const char *fmt, ...);
char *strcpy(char *dst, const char *src);
void *memset(void *dst, int c, unsigned int n);

// FUNCTION: SHANDALAR 0x004d0dc7
int Game_CardScript4d0dc7(int a, int b, int kind)
{
    int ih[4];         /* [ebp-0x10]  ih[0..1] per-side counts, ih[3] choice */
    int ii;            /* [ebp-0x14]                                        */
    char ij[100];      /* [ebp-0x78]                                        */
    int ik;            /* [ebp-0x7c]                                        */
    char il[900];      /* [ebp-0x400] prompt text                           */
    int im[500];       /* [ebp-0xbd0] per-entry selectable mask             */
    int io;            /* [ebp-0xbd4] side                                  */

    if (kind == 0x74) {
        if (a == g_game8c776c || (g_game93a934 & 0x2) != 0) {
            return (g_game7be9ec | g_game7be9e8[0]) & 2;
        }
        if (g_game8bd200 == 1) {
            g_game94d46c = Game_Helper522508(2);
            Game_Helper4c4181();
        } else {
            Game_Helper4c42f8();
        }
        g_gameSlots[a][b].field38 = g_game94d46c;
        return g_game7be9e8[g_game94d46c] & 2;
    }
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        if ((a == g_game8c7468 && (g_game93a934 & 0x2) == 0) || g_game8bd200 == 1) {
            io = g_gameSlots[a][b].field38;
            ih[3] = Game_Helper44e4ed(io, 2);
        } else {
            ih[1] = 0;
            ih[0] = 0;
            for (ii = 0; ii < 2; ii++) {
                for (ik = 0; ik < 500 && g_game94d930[ii][ik] != -1; ik++) {
                    if ((g_gameCards[g_game94d930[ii][ik]].flags & 0x2) != 0) {
                        ih[ii]++;
                        break;
                    }
                }
            }
            if (ih[0] == 0 || ih[1] == 0) {
                if (ih[0] == 0) {
                    io = 1;
                } else {
                    io = 0;
                }
            } else {
                Game_Helper56ca10(0x58a220, 0x58a210);
                if ((g_game93a934 & 0x2) == 0 || a != g_game8c7468) {
                    sprintf(il, (const char *)0x58a238, 0x8aa920, 0x8aaa4c, 0x8aab78);
                } else {
                    sprintf(il, (const char *)0x58a22c, 0x8aaa4c, 0x8aa920, 0x8aab78);
                }
                io = Game_Helper558538(a, a, b, -1, -1, (int)il, 0);
                if (io == 2) {
                    g_game8c83ec = 1;
                } else {
                    if ((g_game93a934 & 0x2) != 0 && a == g_game8c7468) {
                        io = 1 - io;
                    }
                }
            }
            if (g_game8c83ec != 1) {
                if (io == 0) {
                    Game_Helper56ca10(0x58a270, 0x58a260);
                    strcpy(il, (const char *)0x8aadd0);
                } else {
                    Game_Helper56ca10(0x58a254, 0x58a244);
                    Game_Helper453770(ij);
                    sprintf(il, (const char *)0x8aaca4, ij);
                }
                memset(im, 0, 2000);
                for (ik = 0; ik < 500 && g_game94d930[io][ik] != -1; ik++) {
                    if ((g_gameCards[g_game94d930[io][ik]].flags & 0x2) != 0) {
                        im[ik] = 1;
                    }
                }
                ih[3] = Game_Helper56a45f(a, (int)g_game94d930[io], (int)im, 500,
                                          (int)il, 0, 0x8bce20);
                if (ih[3] == -1) {
                    g_game8c83ec = 1;
                }
            }
        }
        if (g_game8c83ec == 1
            || ih[3] == -1
            || g_game94d930[io][ih[3]] == -1
            || (g_gameCards[g_game94d930[io][ih[3]]].flags & 0x2) == 0) {
            g_game8c83ec = 1;
        } else {
            g_gameSlots[a][b].field04 = Game_Helper56ae5d(a, g_game94d930[io][ih[3]]);
            if (g_gameSlots[a][b].field04 != -1) {
                g_gameSlots[a][b].field50 = (unsigned char)a;
                g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags08 &= 0xffffefff;
                if (io != 0) {
                    g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags08 |= 0x1000;
                }
                g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags08 |= 0x20;
                g_gameSlots[a][b].field4c = 1;
                *(int *)(g_gameSlots[a][b].unknown07c) = io;
                *(int *)(g_gameSlots[a][b].unknown07c + 4) = ih[3];
                g_gameSlots[a][b].field74 = (signed char)g_gameSlots[a][b].field50;
                g_gameSlots[a][b].field78 = g_gameSlots[a][b].field04;
                g_gameSlots[a][b].field36 = 1;
            }
        }
        return 0;
    }
    if (kind == 0x71) {
        ih[3] = *(int *)(g_gameSlots[a][b].unknown07c + 4);
        if (g_game94d930[*(int *)(g_gameSlots[a][b].unknown07c)][ih[3]] == -1) {
            Game_Helper46d8c1(a, b, 2);
            g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].field6c = -1;
            g_game8c83ec = 1;
        } else {
            Game_Helper46e5d2(*(int *)(g_gameSlots[a][b].unknown07c), ih[3]);
            g_gameSlots[a][b].field4c = 0;
            Game_Helper4d1c03((signed char)g_gameSlots[a][b].field50, g_gameSlots[a][b].field04);
            g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field12 = -1;
        }
        g_gameSlots[a][b].field36 = 0;
        return 0;
    }
    if (g_game94d178 == 0xd4
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && (signed char)g_gameSlots[a][b].field50 != -1
        && g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field6c != -1
        && g_game7bf4b8 == a
        && g_game8e2fc4 == b
        && a == g_game8c8afc) {
        if (kind == 0x7d) {
            g_game7a01e0 |= 2;
        }
        if (kind == 0x7e) {
            if (g_gameSlots[a][b].field4c != 0) {
                g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags18 |= 8;
            }
            io = (signed char)g_gameSlots[a][b].field50;
            g_gameSlots[a][b].field50 = 0xff;
            Game_Helper46d8c1(io, g_gameSlots[a][b].field04, 1);
        }
    }
    return 0;
}
