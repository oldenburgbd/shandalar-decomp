/* Card behaviour scripts in the 0x0051xxxx range.
 *
 * Derived from the disassembly. All three have the shape
 * int f(int a, int b, int kind), where (a, b) address g_gameSlots and kind
 * selects the event. Pointers into the original image's data (prompt strings,
 * the out-buffer at 0x8aa920) are pushed as address literals, which reproduces
 * the original instruction without carrying the data. /Od locals are named
 * after their frame offsets and declared in the order the decompiler recovered
 * them; locals read before any write are deliberate -- the original picked up
 * the second half of an out-pair the helper had just filled.
 */

#include "game/gameslot.h"

/* Referenced by this TU only; not in the shared headers. Addresses from the
 * disassembly. Only the CRT needs declaring here -- every global and every
 * callee these three scripts touch is already in gameslot.h. */
int sprintf(char *buf, const char *fmt, ...);


/* Card script: a five-option activated ability. The 0x6d handler puts the menu
 * up and stores the chosen option in field4c; the 0x72 handler attaches the
 * card to a target slot (remembering it in field0c/field48) and turns the
 * option into one of four bits on the target's field38. */
// FUNCTION: SHANDALAR 0x0051323c
int Game_CardScript51323c(int a, int b, int kind)
{
    char v260[600];
    int v8;

    if (kind == 0x73) {
        if (a == g_game8c7468 && (g_game93a934 & 2) == 0
            && g_gameSlots[a][b].field4c != 0
            && (g_gameSlots[a][b].field28 & 0x20) == 0) {
            return 0;
        }
        return 1;
    }
    if (kind == 0x6d) {
        if (g_game8bd200 != 1) {
            Game_Helper56ca10(0x58cfa0, 0x58cf90);
            sprintf(v260, (char *)0x58cfac, (char *)0x8aa920, (char *)0x8aaa4c,
                    (char *)0x8aab78, (char *)0x8aaca4, (char *)0x8aadd0,
                    (char *)0x8aaefc);
        }
        g_gameSlots[a][b].field4c =
            Game_Helper558538(a, a, b, -1, -1, (int)v260, 0) + 1;
        if (g_gameSlots[a][b].field4c == 5) {
            g_game8c83ec = 1;
            g_gameSlots[a][b].field4c = 0;
        }
    }
    if (kind == 0x72) {
        if (g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field6c == -1) {
            g_game8c83ec = 1;
        } else {
            if ((signed char)g_gameSlots[a][b].field0c == -1
                && g_gameSlots[a][b].field48 == -1) {
                v8 = Game_Helper4a9910(g_game94f280, g_game930f5c, g_game8aa734,
                                       g_game94f280, g_game930f5c);
                if (v8 != -1) {
                    g_gameSlots[a][v8].field28 = 0;
                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field0c = a;
                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field48 = v8;
                }
            } else {
                v8 = g_gameSlots[a][b].field48;
            }
            if (v8 != -1) {
                switch (g_gameSlots[a][b].field4c) {
                case 1:
                    g_gameSlots[a][v8].field38 |= 0x20;
                    break;
                case 2:
                    g_gameSlots[a][v8].field38 |= 0x40;
                    break;
                case 3:
                    g_gameSlots[a][v8].field38 |= 0x100;
                    break;
                case 4:
                    g_gameSlots[a][v8].field38 |= 0x80;
                    break;
                }
                g_gameSlots[a][v8].field12++;
                g_gameSlots[a][v8].field1c++;
            }
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field28 = 0x8000000;
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field12--;
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field1c--;
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38++;
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field4c = 0;
        }
    }
    if (kind == 0x22 || kind == 199) {
        g_gameSlots[a][b].field12 += (short)g_gameSlots[a][b].field38;
        g_gameSlots[a][b].field1c += (short)g_gameSlots[a][b].field38;
        g_gameSlots[a][b].field38 = 0;
        g_gameSlots[a][b].field48 = -1;
        g_gameSlots[a][b].field0c = g_gameSlots[a][b].field48;
    }
    return 0;
}

/* Card script: an equipment-style attachment. The 0x6d handler picks a target
 * and remembers it in field74/field78; the 0x72 handler creates the attachment
 * slot and links it through field0c/field48; the 0x77 handler and the
 * unconditional tail detach it again. field11c carries the two per-turn bits
 * the untap-step (phase 1) predicate reads. */
// FUNCTION: SHANDALAR 0x005156b0
int Game_CardScript5156b0(int a, int b, int kind)
{
    int v10;
    int vc;
    int v8;

    if (kind == 0x82 && b == g_game951bdc && a == g_game8e1aa8) {
        g_gameSlots[a][b].field11c &= ~2;
    }
    if (g_game94d464 == 1 && b == g_game951bdc && a == g_game8e1aa8) {
        if (kind == 0x7d && (g_gameSlots[a][b].field11c & 1) != 0
            && (g_gameSlots[a][b].field11c & 2) == 0
            && (g_game939e64 & g_gameCards[g_gameSlots[a][b].field6c].flags) == 0) {
            if (a == g_game8c7468 && (g_game93a934 & 2) == 0
                || g_game8bd200 == 1 || g_game939410 != 0) {
                if (g_gameSlots[a][b].field48 == -1
                    || (g_gameSlots[(signed char)g_gameSlots[a][g_gameSlots[a][b].field48].field50][g_gameSlots[a][g_gameSlots[a][b].field48].field04].field11c & 1) == 0
                       && (g_gameSlots[(signed char)g_gameSlots[a][g_gameSlots[a][b].field48].field50][g_gameSlots[a][g_gameSlots[a][b].field48].field04].flags08 & 0x10) != 0) {
                    g_game7a01e0 |= 2;
                }
            } else {
                g_game7a01e0 |= 1;
            }
        }
        if (kind == 0x7e) {
            g_gameSlots[a][b].field11c |= 2;
        }
    }
    if (kind == 0x73) {
        if (((g_gameSlots[a][b].flags08 & 0x30000) == 0
             || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0)
            && (g_gameSlots[a][b].flags08 & 0x10) == 0
            && Game_Helper441699(a, 7, 2) != 0) {
            if (Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0,
                                  Game_Helper488cc4(a, b), 0, 0,
                                  -1, -1, -1, -1, 0, 0, 0) != 0) {
                return 1;
            }
        }
    } else if (kind == 0x90) {
        Game_Helper4c4263(0);
    } else {
        if (kind == 0x6d && (g_gameSlots[a][b].flags08 & 0x10) == 0
            && Game_Helper441699(a, 7, 2) != 0
            && (Game_Helper4143e0(a, 0, 2), g_game8c83ec != 1)) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x58d07c, 0x58d06c);
            }
            if (Game_Helper4c0efa(a, 2, a, 0x200, 2, 0, 0,
                                  Game_Helper488cc4(a, b), 0, 0,
                                  -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &v10) == 0) {
                g_game8c83ec = 1;
            } else {
                g_gameSlots[a][b].field74 = v10;
                g_gameSlots[a][b].field78 = vc;
                g_gameSlots[a][b].field36 = 1;
                g_gameSlots[a][b].flags08 |= 0x10;
            }
        }
        if (kind == 0x72) {
            v10 = g_gameSlots[a][b].field74;
            vc = g_gameSlots[a][b].field78;
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
            if (Game_Helper4bea85(v10, vc, 0, a, 2, 2, 0x200, 2, 0, 0,
                                  Game_Helper488cc4(a, b), 0, 0,
                                  -1, -1, -1, -1, 0, 0, 0) == 0) {
                g_game8c83ec = 1;
            } else {
                v8 = Game_Helper4a9910(g_game94f280, g_game930f5c, g_game8bcff8,
                                       v10, vc);
                if (v8 != -1) {
                    g_gameSlots[a][v8].flags18 |= 0x20;
                    g_gameSlots[a][v8].field12 = 1;
                    g_gameSlots[a][v8].field1c = 1;
                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field0c = a;
                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field48 = v8;
                }
            }
        }
        if (kind == 0x77) {
            if (g_gameSlots[a][b].field48 != -1
                && (signed char)g_gameSlots[(signed char)g_gameSlots[a][b].field0c][g_gameSlots[a][b].field48].field50 == g_game8e1aa8
                && g_gameSlots[(signed char)g_gameSlots[a][b].field0c][g_gameSlots[a][b].field48].field04 == g_game951bdc) {
                g_gameSlots[a][b].field48 = -1;
                g_gameSlots[a][b].field0c = g_gameSlots[a][b].field48;
            }
            if (b == g_game951bdc && a == g_game8e1aa8
                && g_gameSlots[a][b].field48 != -1) {
                Game_Helper46d8c1((signed char)g_gameSlots[a][b].field0c,
                                  g_gameSlots[a][b].field48, 1);
                g_gameSlots[a][b].field48 = -1;
                g_gameSlots[a][b].field0c = g_gameSlots[a][b].field48;
            }
        }
        if (g_gameSlots[a][b].field48 != -1
            && (g_gameSlots[a][b].flags08 & 0x10) == 0) {
            Game_Helper46d8c1((signed char)g_gameSlots[a][b].field0c,
                              g_gameSlots[a][b].field48, 1);
            g_gameSlots[a][b].field48 = -1;
        }
        if (kind == 0x3b && (g_gameSlots[a][b].flags08 & 0x20010) == 0
            && Game_Helper441699(a, 7, 2) != 0) {
            g_game7beb10[a]++;
            g_game7beb18[a]++;
        }
    }
    return 0;
}

/* Card script: a damage-prevention ability usable only in the two combat
 * sub-phases (0x19 / 0x1a). The 0x6d handler picks an attacker whose card
 * carries flag 2 and that is not already marked, remembering it in
 * field74/field78; the 0x72 handler caps that slot's field38 at one. */
// FUNCTION: SHANDALAR 0x00516f08
int Game_CardScript516f08(int a, int b, int kind)
{
    int vc;
    int v8;

    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x10) == 0
            || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) != 0) {
            if ((g_game8c83f8 & 0x4) == 0) {
                return 0;
            }
            if (g_game94d464 == 0x1a || g_game94d464 == 0x19) {
                if (Game_Helper441699(a, 7, 1) == 0) {
                    return 0;
                }
                if (Game_Helper4be650(0, 2, a, 1 - a, 1 - a, 0x200, 2, 0, 0, 0,
                                      0, 0, -1, -1, -1, -1, 0, 2, 8) == 0) {
                    return 0;
                }
                return 99;
            }
            return 0;
        }
        return 0;
    }
    if (kind == 0x90) {
        Game_Helper4c4263(0);
        return 0;
    }
    if (kind == 0x6d && Game_Helper441699(a, 7, 1) != 0
        && (g_game8c83f8 & 0x4) != 0
        && (g_game94d464 == 0x1a || g_game94d464 == 0x19)) {
        Game_Helper4143e0(a, 0, 1);
        if (g_game8bd200 != 1) {
            Game_Helper56ca10(0x58d0b8, 0x58d0ac);
        }
        if (Game_Helper4c0efa(a, a, a, 0x200, 0, 0, 0, 0, 0, 0, g_game93a7d4,
                              -1, -1, -1, 0x20, 0, 0, 0x8aa920, 1, &vc) == 0) {
            g_game8c83ec = 1;
        } else if ((g_gameCards[g_gameSlots[(signed char)g_gameSlots[vc][v8].field0c][g_gameSlots[vc][v8].field48].field6c].flags & 0x2) != 0
                   && (g_gameSlots[(signed char)g_gameSlots[vc][v8].field0c][g_gameSlots[vc][v8].field48].flags08 & 0x200) == 0) {
            g_gameSlots[a][b].field74 = vc;
            g_gameSlots[a][b].field78 = v8;
            g_gameSlots[a][b].field36 = 1;
        }
    }
    if (kind == 0x72) {
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
        if (Game_Helper4bea85(g_gameSlots[a][b].field74,
                              g_gameSlots[a][b].field78, 0, a, a, a, 0x200,
                              0, 0, 0, 0, 0, 0, g_game93a7d4, -1, -1, -1,
                              0, 0, 0) == 0) {
            g_game8c83ec = 1;
        } else if (g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].field38 != 0) {
            g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].field38 = 1;
        }
    }
    return 0;
}
