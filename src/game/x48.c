/* Shandalar card scripts, 0x0048xxxx. Derived from Ghidra's decompilation of
 * orig/Shandalar.exe; names are addresses because nothing establishes meaning.
 *
 * Every script has the shape int f(int a, int b, int kind), where (a, b)
 * address g_gameSlots and kind selects the event.
 *
 * Address literals passed where a pointer is wanted (0x586d08, 0x8aa920, ...)
 * are pointers into the original image's data -- format strings in .rdata and
 * the out-buffers at 0x8aa920. Pushing the literal reproduces the original
 * instruction without carrying the copyrighted data; this is the convention
 * cards43.c established.
 */

#include "game/gameslot.h"

/* Referenced by this TU only; not in the shared headers. Addresses from the
   disassembly. */
extern int g_game7bea98;
extern int g_game7bea9c;

/* Names the other module headers already give these addresses. */
int Game_Helper40d414(int a1, int a2, int a3, int a4);
int Game_Helper56bf4c(int a1);
int Game_Helper56c91f(int a1, int a2);
int Game_Helper56c931(int a1, int a2);

/* Not reconstructed anywhere yet. */
int Game_Fn56a37b(int a1);
int Game_Fn56bd9d(int a1);

/* CRT, imported from msvcrt at /MD. Declared here rather than by including
   <stdio.h>/<string.h> so this TU's declaration surface stays minimal. */
int sprintf(char *buf, const char *fmt, ...);
unsigned int strlen(const char *s);


/* Card script: an attachment that redirects blocking. The slot it acts on
 * carries the real subject in +0x50 (row) and +0x04 (column), and the whole
 * body re-reads that pair rather than caching it. On 0x7e it claims one of the
 * opponent's blockers into the subject's +0x24, then clones the subject through
 * a fresh card record (script 0x0048592f) once per additional blocker. */
// FUNCTION: SHANDALAR 0x00481688
int Game_CardScript481688(int a, int b, int kind)
{
    char iv;
    int ig;
    int ih;
    int ii;
    int ij;
    int ik;
    int v30;
    int v18;
    int v14;
    int v10;

    v10 = 0;
    ih = 0;
    v18 = -1;
    ii = (signed char)g_gameSlots[a][b].field50;
    ig = g_gameSlots[a][b].field04;
    if (g_game94d178 == 0xda
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && g_game8c8afc == a) {
        if (kind == 0x7d) {
            g_game7a01e0 |= 2;
        }
        if (kind == 0x7e) {
            if ((g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags08 & 0x8) == 0) {
                ij = 1 - ii;
                v14 = 0;
                while (v14 < g_game78e970[ij] && !ih) {
                    ik = Game_Helper411d90(ij, v14);
                    if (ik != 0
                        && (g_gameSlots[ij][v14].flags08 & 0x4) != 0
                        && (ik = Game_Helper40d414(ii, ig, ij, v14), ik != 0)) {
                        iv = g_gameSlots[ij][v14].field24;
                        if (iv == -1) {
                            g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field24 = (char)v14;
                        } else {
                            g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field24 = iv;
                        }
                        g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags08 |= 8;
                        if (g_game8bd200 != 1 && iv == -1) {
                            Game_Helper40ebb9(0x15);
                        }
                        v10 = v10 + 1;
                        ih = 1;
                    }
                    v14 = v14 + 1;
                }
            }
            if ((g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags08 & 0x8) != 0) {
                iv = g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field24;
                ij = 1 - ii;
                for (v14 = 0; v14 < g_game78e970[ij]; v14 = v14 + 1) {
                    ik = Game_Helper411d90(ij, v14);
                    if (ik != 0
                        && (g_gameSlots[ij][v14].flags08 & 0x4) != 0
                        && (ik = Game_Helper40d414(ii, ig, ij, v14), ik != 0)) {
                        ik = (signed char)g_gameSlots[ij][v14].field24;
                        v30 = ik;
                        if (ik == -1) {
                            v30 = v14;
                        }
                        if (iv != v30 && (ik == -1 || ik == v14)) {
                            if (v18 == -1
                                && (v18 = Game_Helper4ba25e(g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field6c), v18 != -1)) {
                                g_gameCards[v18].script = (int)Game_Callback48592f;
                                g_gameCards[v18].flags3c = 0;
                                g_gameCards[v18].mask = g_game8e32fc;
                            }
                            if (v18 != -1
                                && (ik = Game_Helper56ae5d((signed char)g_gameSlots[a][b].field50, v18), ik != -1)) {
                                g_gameSlots[ii][ik].flags08 = g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags08 & 0xfffffff7;
                                g_gameSlots[ii][ik].field28 = g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field28;
                                g_gameSlots[ii][ik].flags18 = 0x8000008;
                                g_gameSlots[ii][ik].field0c = g_gameSlots[a][b].field50;
                                g_gameSlots[ii][ik].field48 = g_gameSlots[a][b].field04;
                                if (g_gameSlots[ij][v14].field24 == -1) {
                                    g_gameSlots[ii][ik].field24 = (char)v14;
                                } else {
                                    g_gameSlots[ii][ik].field24 = g_gameSlots[ij][v14].field24;
                                }
                                g_gameSlots[ii][ik].flags08 |= 8;
                                g_gameSlots[ii][ik].field64 = g_gameCards[g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field6c].mask;
                                v10 = v10 + 1;
                                g_game7bfe78 |= 4;
                            }
                        }
                    }
                }
                g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field24 = iv;
                g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags08 |= 8;
            }
            if (1 < v10) {
                for (v14 = 0; v14 < g_game78e970[ii]; v14 = v14 + 1) {
                    if (g_gameSlots[ii][v14].field6c == v18) {
                        g_gameSlots[ii][v14].field38 = v10;
                    }
                }
                g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags18 |= 0x8000000;
            }
            Game_Helper558aea(0, 0xff);
        }
    }
    if (kind == 0x22 || kind == 0xc7) {
        Game_Helper46d8c1(a, b, 1);
    }
    if (g_game94d178 == 0xd4
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && g_game8c8afc == a
        && (signed char)g_gameSlots[a][b].field50 == g_game7bf4b8
        && g_gameSlots[a][b].field04 == g_game8e2fc4) {
        if (kind == 0x7d) {
            g_game7a01e0 |= 2;
        }
        if (kind == 0x7e
            && g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field24 != -1) {
            ii = (signed char)g_gameSlots[a][b].field50;
            for (v14 = 0; v14 < g_game78e970[ii]; v14 = v14 + 1) {
                ij = Game_Helper411d90(ii, v14);
                if (ij != 0
                    && g_gameCards[g_gameSlots[ii][v14].field6c].script == (int)Game_Callback48592f
                    && g_gameSlots[ii][v14].field0c == g_gameSlots[a][b].field50
                    && g_gameSlots[ii][v14].field48 == g_gameSlots[a][b].field04) {
                    Game_Helper46d8c1(ii, v14, 2);
                }
            }
        }
    }
    if (kind == 0x77
        && g_gameSlots[a][b].field04 == g_game951bdc
        && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8
        && g_game951bdc != -1
        && g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field24 != -1) {
        ii = (signed char)g_gameSlots[a][b].field50;
        for (v14 = 0; v14 < g_game78e970[ii]; v14 = v14 + 1) {
            ij = Game_Helper411d90(ii, v14);
            if (ij != 0
                && g_gameCards[g_gameSlots[ii][v14].field6c].script == (int)Game_Callback48592f
                && g_gameSlots[ii][v14].field0c == g_gameSlots[a][b].field50
                && g_gameSlots[ii][v14].field48 == g_gameSlots[a][b].field04) {
                Game_Helper46d8c1(ii, v14, 2);
            }
        }
    }
    return 0;
}

/* Card script: picks a random colour at 0x6c (a bit in +0x4c, 0x800 << n),
 * grants it at 0x71, and on 0x6d offers a three-way prompt whose answer either
 * taps for the choice, latches bit 0 of +0x4c, or aborts. The 0x72 resolution
 * either adds a random amount to the target's +0x38 or plants a 0x100 marker
 * slot; 0x34 reports the colour bits through 0x0040dffd. */
// FUNCTION: SHANDALAR 0x004893a9
int Game_CardScript4893a9(int a, int b, int kind)
{
    int iv;
    int ig;
    unsigned char v3a4;
    unsigned int v39c;
    char v398[900];
    int v14;
    int v10;
    int vc;
    unsigned int v8;

    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        if ((g_game93a934 & 0x2) == 0) {
            v3a4 = Game_Helper522508(5);
        } else {
            v3a4 = Game_Helper522535(a, 5);
        }
        g_gameSlots[a][b].field4c = 0x800 << (v3a4 & 0x1f);
        iv = 0;
    } else if (kind == 0x71) {
        g_gameSlots[a][b].field28 |= g_gameSlots[a][b].field4c;
        iv = 0;
    } else if (kind == 0x73) {
        ig = Game_Helper441699(a, 5, 2);
        if (ig == 0) {
            ig = Game_Helper441699(a, 7, 1);
            if (ig == 0 || (g_gameSlots[a][b].field4c & 0x100) != 0) {
                iv = 0;
            } else {
                iv = 1;
            }
        } else {
            iv = 1;
        }
    } else if (kind == 0x6d) {
        ig = Game_Helper441699(a, 7, 1);
        v14 = Game_Helper441699(a, 5, 2);
        g_gameSlots[a][b].field4c &= 0xfffffffe;
        if (v14 != 0
            || (ig != 0 && (g_gameSlots[a][b].field4c & 0x100) == 0)) {
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x586d08, 0x586cf8);
                if (v14 == 0) {
                    sprintf(v398, (char *)0x586d18, (char *)0x8aa920);
                } else {
                    sprintf(v398, (char *)0x586d14, (char *)0x8aa920);
                }
                if (ig == 0 || (g_gameSlots[a][b].field4c & 0x100) != 0) {
                    sprintf(v398 + strlen(v398), (char *)0x586d28, (char *)0x8aaa4c);
                } else {
                    sprintf(v398 + strlen(v398), (char *)0x586d20, (char *)0x8aaa4c);
                }
                sprintf(v398 + strlen(v398), (char *)0x586d30, (char *)0x8aab78);
            }
            if (g_game8ca260 == 0
                || ((g_gameSlots[a][b].flags08 & 0x8) == 0
                    && ((g_gameSlots[a][b].flags08 & 0x4) == 0
                        || (g_gameSlots[a][b].flags08 & 0x200) == 0))) {
                if (v14 == 0) {
                    if (ig == 0 || (g_gameSlots[a][b].field4c & 0x100) != 0) {
                        v10 = 2;
                    } else {
                        v10 = 1;
                    }
                } else {
                    v10 = 0;
                }
            } else if (ig == 0 || (g_gameSlots[a][b].field4c & 0x100) != 0) {
                if (v14 == 0) {
                    v10 = 2;
                } else {
                    v10 = 0;
                }
            } else {
                v10 = 1;
            }
            ig = Game_Helper558538(a, a, b, -1, -1, (int)v398, v10);
            if (ig == 0) {
                if (v14 != 0) {
                    Game_Helper4143e0(a, 5, 2);
                    if (g_game8c83ec != 1) {
                        g_gameSlots[a][b].field74 = a;
                        g_gameSlots[a][b].field78 = b;
                        g_gameSlots[a][b].field36 = 1;
                        if (g_gameSlots[a][b].field38 == 0) {
                            g_gameSlots[a][b].field38 |= 0x80000;
                        }
                    }
                }
            } else if (ig == 1) {
                Game_Helper4143e0(a, 0, 1);
                if (g_game8c83ec != 1) {
                    g_gameSlots[a][b].field74 = a;
                    g_gameSlots[a][b].field78 = b;
                    g_gameSlots[a][b].field36 = 1;
                    g_gameSlots[a][b].field4c |= 1;
                }
            } else if (ig == 2) {
                g_game8c83ec = 1;
            }
        }
        iv = 0;
    } else if (kind == 0x72) {
        if (g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field6c == -1) {
            g_game8c83ec = 1;
        } else if ((g_gameSlots[a][b].field4c & 0x1) == 0) {
            if ((g_game93a934 & 0x2) == 0) {
                v39c = Game_Helper522508(3);
            } else {
                v39c = Game_Helper522535(g_game94f280, 3);
            }
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 =
                g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 + (v39c & 0xff);
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x586d48, 0x586d38);
                sprintf(v398, (char *)0x8aaca4, v39c);
                Game_Helper558538(a, a, b, -1, -1, (int)v398, 0);
            }
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
            if (g_game8bd200 != 1) {
                Game_Helper40ebb9(0x3a);
            }
            if (v39c != 0
                && (g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 & 0x80000) != 0) {
                g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 &= 0xfff7ffff;
                ig = Game_Helper4a9910(g_game94f280, g_game930f5c, g_game8bcff8, g_game94f280, g_game930f5c);
                if (ig != -1) {
                    g_gameSlots[a][ig].field12 = (short)v39c;
                    g_gameSlots[a][ig].field38 |= 0x80000;
                    g_gameSlots[a][ig].field4c = 1;
                }
            }
        } else if ((g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field4c & 0x100) == 0) {
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field4c &= 0x1ff800;
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field4c |= 0x100;
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
            vc = Game_Helper4a9910(g_game94f280, g_game930f5c, g_game8aa734, g_game94f280, g_game930f5c);
            if (vc != -1) {
                g_gameSlots[a][vc].field38 = 0x100;
                g_gameSlots[a][vc].field28 = 0;
                g_gameSlots[a][vc].field4c = 2;
            }
            if (g_game8bd200 != 1) {
                Game_Helper40ebb9(0x3a);
            }
        }
        iv = 0;
    } else if (kind == 0x34
               && g_game951bdc == b
               && g_game8e1aa8 == a
               && (g_gameSlots[a][b].flags08 & 0x20) == 0) {
        g_game7a01e0 |= g_gameSlots[a][b].field4c & 0x1ff800;
        v8 = g_game7a01e0;
        iv = Game_Helper40dffd((g_gameSlots[a][b].field4c & 0x1ff800) >> 10);
        Game_Helper49c6b1(a, b, iv);
        g_game7a01e0 = v8;
        iv = 0;
    } else if (kind == 0x8c
               && g_game951bdc == b
               && g_game8e1aa8 == a
               && (ig = Game_Helper441699(a, 7, 1)) != 0) {
        g_game97db34 |= 0x100;
        iv = 0;
    } else if (kind == 0x22 || kind == 0xc7) {
        g_gameSlots[a][b].field38 = 0;
        g_gameSlots[a][b].field4c &= 0xfffffeff;
        iv = 0;
    } else {
        iv = 0;
    }
    return iv;
}

/* Card script: a coin-flip-shaped prompt. 0x6d stages the question at the
 * other player (+0x4c = 1 - a) and stores the answer in +0x38; 0x72 acts on
 * it -- answer 0 destroys the target and then a random card from the answering
 * player's row (with a 999-try give-up and a replay path through 0x14), answer
 * 1 costs ten, answer 2 runs 0x0056a37b. */
// FUNCTION: SHANDALAR 0x0048a65a
int Game_CardScript48a65a(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;
    int v394;
    char v390[900];
    int vc;
    int v8;

    if (kind == 0x73) {
        ih = (g_gameSlots[a][b].flags08 & 0x20010) == 0;
    } else if (kind == 0x6d) {
        if (g_game8bd200 != 1) {
            Game_Helper56ca10(0x586d78, 0x586d68);
            sprintf(v390, (char *)0x586d84, (char *)0x8aa920, (char *)0x8aaa4c, (char *)0x8aab78);
        }
        g_gameSlots[a][b].field4c = 1 - a;
        ig = Game_Helper558538(g_gameSlots[a][b].field4c, a, b, -1, -1, (int)v390, 0);
        g_gameSlots[a][b].field38 = ig;
        g_gameSlots[a][b].flags08 |= 0x10;
        ih = 0;
    } else if (kind == 0x72) {
        vc = g_gameSlots[a][b].field4c;
        iv = g_gameSlots[a][b].field38;
        if (iv == 0) {
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].flags08 ^= 0x1000;
            if (g_game8bd200 != 1) {
                Game_Helper40ebb9(0xf);
            }
            Game_Helper46d8c1(g_game94f280, g_game930f5c, 3);
            if (g_game8c776c == a) {
                Game_Helper56bf4c(g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field6c);
            } else {
                Game_Fn56bd9d(g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field6c);
            }
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field6c = -1;
            v394 = 0;
            if ((g_game93a934 & 0x2) == 0 || g_game8c7468 != a) {
                do {
                    do {
                        v8 = Game_Helper522508(g_game78e970[vc]);
                    } while (g_gameSlots[vc][v8].field6c == -1);
                } while ((g_gameSlots[vc][v8].flags08 & 0x2) != 0
                         && (v394 = v394 + 1, v394 < 999));
                if ((g_game93a934 & 0x2) != 0 && g_game8c776c == a) {
                    if (0x3e6 < v394) {
                        v8 = -1;
                    }
                    g_game7bea9c = v8;
                    g_game7bea98 = 0x14;
                    Game_Helper56c931(a, 0x14);
                }
            } else {
                Game_Helper56c91f(a, 0x14);
                v8 = g_game7bea9c;
                if (g_game7bea9c == -1) {
                    v394 = 999;
                }
            }
            if (v394 < 999) {
                if (g_game8bd200 != 1) {
                    Game_Helper56ca10(0x586da0, 0x586d90);
                    sprintf(v390, (char *)0x586dac, (char *)0x8aaca4);
                    Game_Helper558538(a, a, b, vc, v8, (int)v390, 0);
                }
                g_gameSlots[vc][v8].flags08 ^= 0x1000;
                Game_Helper46e4d0(vc, v8);
                if (g_game8c776c == a) {
                    Game_Fn56bd9d(g_gameSlots[vc][v8].field6c);
                } else {
                    Game_Helper56bf4c(g_gameSlots[vc][v8].field6c);
                }
                g_gameSlots[vc][v8].field6c = -1;
            }
        } else if (iv == 1) {
            g_game8c83f0[vc] = g_game8c83f0[vc] + -10;
            Game_Helper46d8c1(g_game94f280, g_game930f5c, 1);
        } else if (iv == 2) {
            if (g_game8bd200 == 1) {
                g_game8c83f0[vc] = 0xffffff9d;
            } else {
                Game_Fn56a37b(vc);
            }
            Game_Helper46d8c1(g_game94f280, g_game930f5c, 1);
        }
        ih = 0;
    } else {
        ih = 0;
    }
    return ih;
}
