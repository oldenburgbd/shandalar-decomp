/* Combat resolution and the combat status screen (0x0042xxxx), plus three
 * card scripts that live in the same address range.
 *
 * Derived from the disassembly. Formulations follow the measured-good house
 * text: /Od locals are named after their frame offsets and declared in the
 * order the decompiler recovered them, expression shapes are transcribed
 * rather than tidied, and pointers into the original image's data (prompt and
 * caption strings, the scratch text buffer at 0x78cf10, the out-buffer at
 * 0x8aa920) are pushed as address literals, which reproduces the original
 * instruction without carrying the data.
 */

#include "game/gameslot.h"

/* Referenced by this TU only; not in the shared headers. Addresses from the
 * disassembly.
 *
 * These are DEFINITIONS, not extern declarations: nothing else in the build
 * defines them, and an extern with no definition breaks the DLL link. MSVC 4.2
 * emits an uninitialised file-scope object as a common symbol, so a second TU
 * that names the same global merges rather than colliding (measured: two
 * objects each defining the same name link clean). Extents marked below are
 * read off the index arithmetic; where nothing bounds them they are
 * placeholders large enough to index. */

/* Sixteen parallel per-combatant arrays: the slot column, two helper results,
 * a strength, and the assigned amount -- one set for the attacker (0x5a76d0)
 * and one for the defender (0x5a80d8), each capped at 16 entries by the
 * explicit `== 0x10` break. The 0x40-byte spacing of 0x5a7610/0x5a7650 and the
 * flush fit of 0x5a8098 against 0x5a80d8 are what fix the extent at 16. */
int g_game5a7610[16];
int g_game5a7650[16];
int g_game5a76d0[16];
int g_game5a7820[16];
int g_game5a7860[16];
int g_game5a7b1c;                /* attacker count  */
int g_game5a7ba4;                /* defender count  */
int g_game5a8010[16];
int g_game5a8098[16];
int g_game5a80d8[16];
int g_game5a8288;
int g_game74b8b4;
int g_game74b8b8;
int g_game77f1c4;
int g_game78df40[8];             /* one entry per mana index, 0..6 */
int g_game78df58;
int g_game78df5c;
int g_game7a0098;
int g_game930ad8;
int g_game94cf5c;
int g_game94cf68[32];            /* extent unknown; indexed by owner */

/* Written immediately before a callback is handed to Game_Helper49f3c0, so
 * they are how the caller passes it its subject. */
int g_game97c854;
int g_game97c858;

/* Defined in stubs.c; declared in gamefns.h / helpers.h, neither of which is
 * on this file's include path. */
extern int g_game5832b4;         /* pointer to the draw surface */
extern int g_game5832dc;
extern int g_game5863b8;
extern int g_game5863bc;
extern int g_game74cdb4;
extern int g_game7bf4c8;

/* Named by the headers that already cover these addresses, even though those
 * headers are not included here. */
int Game_Fn40eb87(int a1, int a2);
int Game_Helper44157b(int a1, int a2, int a3);
int Game_Helper46db2e(void);
int Game_Helper46ed33(void);
int Game_Fn46ed03(void);
int Game_Callback46dbed(void);
int Game_Helper558356(int a1);
int Game_Fn4289b1(int a1, int a2, int a3, int a4);
int Game_Helper430ef4(int a1, int a2, int a3, int a4);
int Game_Helper43104f(int a1, int a2, int a3, int a4);
int Game_Fn4310a4(int a1, int a2, int a3, int a4);
int Game_Fn56c779(int a1);
int Game_Helper5797e0(int a1, int a2, int a3, int a4, int a5, int a6);
int Game_Helper579400(int a1);
int Game_Helper579440(int a1, int a2);
int Game_Helper579890(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8);
int Game_Helper579e40(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10);
int Game_Helper57c800(int a1, int a2);

/* gamefns.h names 0x453f42 with two parameters, a lower bound taken from the
 * callee's own [ebp+N] use. Its call site here cleans up twelve bytes, so it
 * takes three. Declared locally so the two do not clash. */
int Game_Helper453f42(int a1, int a2, int a3);

/* Not named anywhere yet. */
int Game_Fn4038a8(int a1);
int Game_Fn424a96(int a1, int a2);
int Game_Fn424c13(void);
int Game_Fn424c71(int a1, int a2, int a3, int *a4, int a5, int a6, int *a7, int *a8);
int Game_Fn42476a(int a1, int a2, int a3);
int Game_Fn4252e8(int a1, int a2, int a3, int *a4, int a5, int a6, int *a7, int *a8);
int Game_Fn425964(int a1, int a2, int a3);
int Game_Fn4259e2(int a1, int a2, int a3);
int Game_Fn425df8(int a1, int a2);
int Game_Fn426e30(int a1, int a2, int a3);
int Game_Callback42d474(void);
int Game_Fn46ca26(int a1);
int Game_Helper57bb90(int a1, int a2, int a3, int a4);

/* CRT and Win32 entry points this TU calls. Declared rather than included so
 * the declaration surface stays exactly this wide. */
int sprintf(char *buf, const char *fmt, ...);
char *strcpy(char *dst, const char *src);
char *strcat(char *dst, const char *src);
unsigned int strlen(const char *s);
char *_itoa(int value, char *buf, int radix);
void __stdcall Sleep(unsigned long ms);


/* Runs the two combat sub-phases (0x19 then 0x1a) for one owner: collects the
 * attackers and blockers, either lets the AI assign damage or drives the
 * interactive assignment loop, then applies trample and first-strike leftovers.
 */
// FUNCTION: SHANDALAR 0x00422294
void Game_Fn422294(int a1)
{
    int vf4;
    int vf0;
    int vec;
    int ve8;
    int ve4;
    int ve0;
    int vdc;
    int vd8;
    int vd4;
    int vd0;
    int vcc;
    int vc8;
    int vc4;
    int vc0;
    int vbc;
    int vb8;
    int vb4;
    int vb0[16];
    int v70;
    int v6c;
    int v68;
    int v64;
    int v60;
    int v5c;
    int v58;
    int v54[16];
    int v14;
    int v10;
    int vc;
    int v8;

    v70 = 1 - a1;
    vc0 = 0;
    v68 = 0;
    do {
        if (1 < v68) {
            return;
        }
        if (v68 == 0) {
            g_game94d464 = 0x19;
        } else {
            g_game94d464 = 0x1a;
        }
        Game_Fn40eb87(a1, g_game94d464);
        for (v60 = 0; v60 < g_game78e970[a1]; v60++) {
            if (g_gameSlots[a1][v60].field24 == -1
                || g_gameSlots[a1][v60].field24 == v60) {
                if (vc0 == 0 && g_game8bd200 != 1) {
                    Game_Helper40ebb9(0x14);
                    vc0 = 1;
                }
                v58 = 0;
                v14 = 0;
                g_game5a7b1c = 0;
                vd0 = 0;
                for (vcc = 0; vcc < g_game78e970[a1]; vcc++) {
                    if ((v60 == vcc || g_gameSlots[a1][vcc].field24 == v60)
                        && g_gameSlots[a1][vcc].field6c != -1
                        && (g_gameSlots[a1][vcc].flags08 & 0x800006) == 6) {
                        g_game5a76d0[g_game5a7b1c] = vcc;
                        g_game5a7820[g_game5a7b1c] = Game_Helper40d7b2(a1, vcc, 0x33, -1);
                        g_game5a7610[g_game5a7b1c] = Game_Helper40d7b2(a1, vcc, 0x34, -1);
                        vb8 = 0;
                        g_game5a8010[g_game5a7b1c] = 0;
                        if (Game_Fn424a96(v68, g_game5a7610[g_game5a7b1c]) != 0) {
                            vb8 = Game_Helper40d7b2(a1, vcc, 0x32, -1);
                            if (vb8 < 0) {
                                vb8 = 0;
                            }
                            g_game5a8010[g_game5a7b1c] = vb8;
                            v14 += vb8;
                            if ((g_game5a7610[g_game5a7b1c] & 0x80) != 0) {
                                v58 += vb8;
                            }
                        }
                        g_game5a7b1c++;
                        if (g_game5a7b1c == 0x10) {
                            break;
                        }
                    }
                }
                if (1 < g_game5a7b1c) {
                    vd0 = 1;
                }
                v5c = 0;
                g_game5a7ba4 = 0;
                vb4 = 0;
                for (vcc = 0; vcc < g_game78e970[v70]; vcc++) {
                    if (Game_Helper411d90(v70, vcc) != 0
                        && g_gameSlots[v70][vcc].field24 == v60) {
                        g_game5a80d8[g_game5a7ba4] = vcc;
                        g_game5a7860[g_game5a7ba4] =
                            Game_Helper40d7b2(v70, vcc, 0x33, v60) - g_gameSlots[v70][vcc].field10;
                        g_game5a7650[g_game5a7ba4] = Game_Helper40d7b2(v70, vcc, 0x34, -1);
                        vd8 = 0;
                        g_game5a8098[g_game5a7ba4] = 0;
                        if ((g_gameSlots[v70][vcc].flags08 & 0x10) == 0
                            && Game_Fn424a96(v68, g_game5a7650[g_game5a7ba4]) != 0) {
                            vd8 = Game_Helper40d7b2(v70, vcc, 0x32, v60);
                            if (vd8 < 0) {
                                vd8 = 0;
                            }
                            g_game5a8098[g_game5a7ba4] = vd8;
                            v5c += vd8;
                        }
                        if ((g_game5a7650[g_game5a7ba4] & 0x40) != 0) {
                            vb4 = 1;
                        }
                        g_game5a7ba4++;
                        if (g_game5a7ba4 == 0x10) {
                            break;
                        }
                    }
                }
                if (g_game5a7ba4 != 0) {
                    for (vbc = 0; vbc < g_game5a7b1c; vbc++) {
                        g_gameSlots[a1][g_game5a76d0[vbc]].flags08 |= 0x200;
                    }
                }
                if (g_game5a7b1c != 0 || g_game5a7ba4 != 0) {
                    if (g_game5a7ba4 < 2) {
                        if (g_game5a7ba4 == 1) {
                            for (vbc = 0; vbc < g_game5a7b1c; vbc++) {
                                if (Game_Fn424a96(v68, g_game5a7610[vbc]) != 0) {
                                    v54[0] = Game_AllocSlot(v70, g_game5a80d8[0],
                                                            g_game5a8010[vbc], a1,
                                                            g_game5a76d0[vbc]);
                                    vc = v54[0];
                                    if (v54[0] != -1) {
                                        g_gameSlots[a1][v54[0]].flags18 |= 0x40000;
                                        if ((g_game5a7610[vbc] & 0x80) != 0) {
                                            g_gameSlots[a1][v54[0]].flags18 |= 0x80000;
                                        }
                                        if (v68 == 0) {
                                            g_gameSlots[a1][v54[0]].flags18 |= 0x100000;
                                        }
                                    }
                                }
                            }
                        } else {
                            for (vbc = 0; vbc < g_game5a7b1c; vbc++) {
                                if (Game_Fn424a96(v68, g_game5a7610[vbc]) != 0
                                    && ((g_gameSlots[a1][g_game5a76d0[vbc]].flags08 & 0x200) == 0
                                        || (g_game5a7610[vbc] & 0x80) != 0)) {
                                    Game_Helper4bab38(v70, g_game5a8010[vbc], a1,
                                                      g_game5a76d0[vbc]);
                                }
                            }
                        }
                    } else {
                        if ((vb4 == 0 && a1 == g_game8c7468)
                            || (vb4 != 0 && a1 == g_game8c776c)) {
                            v64 = g_game8c7468;
                        } else {
                            v64 = g_game8c776c;
                        }
                        if ((g_game93a934 & 2) == 0
                            && (g_game8bd200 == 1 || v64 == g_game8c7468)) {
                            ve0 = 0x7fffffff;
                            vdc = -0x7fff;
                            Game_Fn424c13();
                            for (v6c = 0; v6c < 0x10; v6c++) {
                                v54[v6c] = -1;
                            }
                            Game_Fn424c71(a1, 0, vb4, v54, v68, 0, &ve0, &vdc);
                            Game_Fn424c71(a1, 0, vb4, v54, v68, 1, &ve0, &vdc);
                        } else {
                            for (vbc = 0; vbc < g_game5a7b1c; vbc++) {
                                for (v6c = 0; v6c < 0x10; v6c++) {
                                    v54[v6c] = -1;
                                }
                                vb8 = g_game5a8010[vbc];
                                while (vb8 != 0) {
                                    Game_Helper56ca10(0x8aa4f0, 0x58184c);
                                    if (g_game930ad8 == 2) {
                                        if ((g_game5a7610[vbc] & 0x80) == 0) {
                                            sprintf((char *)0x78cf10, (char *)0x8aab78, vb8);
                                        } else {
                                            sprintf((char *)0x78cf10, (char *)0x8aaca4, vb8);
                                        }
                                    } else if ((g_game5a7610[vbc] & 0x80) == 0) {
                                        sprintf((char *)0x78cf10, (char *)0x8aa920,
                                                Game_Helper453f42(a1, g_game5a76d0[vbc], vb8));
                                    } else {
                                        sprintf((char *)0x78cf10, (char *)0x8aaa4c,
                                                Game_Helper453f42(a1, g_game5a76d0[vbc], vb8));
                                    }
                                    Game_Fn425964(a1, g_game5a76d0[vbc], 1);
                                    v8 = 0;
                                    while (v8 == 0) {
                                        Game_Helper4c0efa(v64, v70, v70, 0x200, 2, 0, 0, 0, 0, 0,
                                                          -1, -1, -1, -1, 0, 0x10, 0, 0x78cf10, 0,
                                                          &vc8);
                                        for (v6c = 0; v6c < g_game5a7ba4; v6c++) {
                                            if (g_game5a80d8[v6c] == vc4) {
                                                v8 = 1;
                                            }
                                        }
                                        if (v8 == 0 && g_game8bd200 != 1) {
                                            Game_Helper558356(0x8aadd0);
                                            Sleep(0x5dc);
                                            Game_Helper558356(0x581864);
                                        }
                                        if (v8 == 1 && Game_Fn425df8(vc8, vc4) != 0
                                            && (v8 = 0, g_game8bd200 != 1)) {
                                            Game_Helper558356(0x8aaefc);
                                            Sleep(0x5dc);
                                            Game_Helper558356(0x581868);
                                        }
                                    }
                                    strcpy((char *)0x78cf10, (char *)0x58186c);
                                    Game_Fn425964(a1, g_game5a76d0[vbc], 0);
                                    if (vc8 != -1 && vc4 != -1 && vc4 != -2) {
                                        for (v6c = 0; v6c < g_game5a7ba4; v6c++) {
                                            if (g_game5a80d8[v6c] == vc4) {
                                                if (g_game74b8b8 == 0) {
                                                    vd4 = 1;
                                                } else {
                                                    vd4 = vb8;
                                                }
                                                if (v54[v6c] == -1) {
                                                    vc = Game_AllocSlot(vc8, vc4, vd4, a1,
                                                                        g_game5a76d0[vbc]);
                                                    v54[v6c] = vc;
                                                    if (vc != -1) {
                                                        g_gameSlots[a1][vc].flags18 |= 0x40000;
                                                        if ((g_game5a7610[vbc] & 0x80) != 0) {
                                                            g_gameSlots[a1][vc].flags18 |= 0x80000;
                                                        }
                                                        if (v68 == 0) {
                                                            g_gameSlots[a1][vc].flags18 |= 0x100000;
                                                        }
                                                    }
                                                } else {
                                                    g_gameSlots[a1][v54[v6c]].field38 += vd4;
                                                }
                                                vb8 -= vd4;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    if ((g_game7bfe78 & 4) != 0 && v70 == g_game8c7468
                        && (g_game93a934 & 2) == 0) {
                        ve4 = 0;
                        vec = 0;
                        if (g_game5a7b1c < 2) {
                            for (vbc = 0; vbc < g_game5a7ba4; vbc++) {
                                if ((g_gameSlots[v70][g_game5a80d8[vbc]].flags18 & 0x8000000) == 0) {
                                    vec += g_game5a8098[vbc];
                                } else {
                                    ve4 += g_game5a8098[vbc];
                                }
                            }
                            for (vbc = 0; vbc < g_game5a7ba4; vbc++) {
                                if ((g_gameSlots[v70][g_game5a80d8[vbc]].flags18 & 0x8000000) != 0) {
                                    ve8 = g_game5a8098[vbc];
                                    if (g_gameSlots[a1][g_game5a76d0[0]].field0e <= vec + ve4) {
                                        for (; ve8 != 0
                                               && vec < g_gameSlots[a1][g_game5a76d0[0]].field0e;
                                             vec++) {
                                            ve4--;
                                            ve8--;
                                        }
                                    }
                                    if (ve8 != 0
                                        && (g_gameCards[g_gameSlots[v70][g_game5a80d8[vbc]].field6c].script != (int)Game_Callback48592f
                                            || 1 < g_gameSlots[v70][g_game5a80d8[vbc]].field38)) {
                                        Game_Fn42476a(v70, g_game5a80d8[vbc], ve8);
                                        g_game5a8098[vbc] -= ve8;
                                    }
                                }
                            }
                        } else {
                            for (vbc = 0; vbc < g_game5a7ba4; vbc++) {
                                if ((g_gameSlots[v70][g_game5a80d8[vbc]].flags18 & 0x8000000) != 0
                                    && (g_gameCards[g_gameSlots[v70][g_game5a80d8[vbc]].field6c].script != (int)Game_Callback48592f
                                        || 1 < g_gameSlots[v70][g_game5a80d8[vbc]].field38)) {
                                    Game_Fn42476a(v70, g_game5a80d8[vbc], g_game5a8098[vbc]);
                                    g_game5a8098[vbc] = 0;
                                }
                            }
                        }
                    }
                    if (g_game5a7b1c < 2
                        && ((g_game7bfe78 & 4) == 0
                            || (v70 != g_game8c776c && (g_game93a934 & 2) == 0))) {
                        if (g_game5a7b1c == 1) {
                            for (vbc = 0; vbc < g_game5a7ba4; vbc++) {
                                if (Game_Fn424a96(v68, g_game5a7650[vbc]) != 0) {
                                    vb0[0] = Game_AllocSlot(a1, g_game5a76d0[0],
                                                            g_game5a8098[vbc], v70,
                                                            g_game5a80d8[vbc]);
                                    vc = vb0[0];
                                    if (vb0[0] != -1) {
                                        g_gameSlots[v70][vb0[0]].flags18 |= 0x40000;
                                        if (v68 == 0) {
                                            g_gameSlots[v70][vb0[0]].flags18 |= 0x100000;
                                        }
                                    }
                                }
                            }
                        }
                    } else {
                        if ((vd0 == 0 && a1 == g_game8c776c)
                            || (vd0 != 0 && a1 == g_game8c7468)) {
                            v64 = g_game8c7468;
                        } else {
                            v64 = g_game8c776c;
                        }
                        if ((g_game93a934 & 2) == 0
                            && (g_game8bd200 == 1 || v64 == g_game8c7468)) {
                            vf4 = 0x7fffffff;
                            vf0 = -1;
                            Game_Fn424c13();
                            for (v6c = 0; v6c < 0x10; v6c++) {
                                vb0[v6c] = -1;
                            }
                            Game_Fn4252e8(a1, 0, vd0, vb0, v68, 0, &vf4, &vf0);
                            Game_Fn4252e8(a1, 0, vd0, vb0, v68, 1, &vf4, &vf0);
                        } else {
                            for (vbc = 0; vbc < g_game5a7ba4; vbc++) {
                                for (v6c = 0; v6c < 0x10; v6c++) {
                                    vb0[v6c] = -1;
                                }
                                vd8 = g_game5a8098[vbc];
                                v10 = 0;
                                while (vd8 != 0 && v10 == 0) {
                                    Game_Helper56ca10(0x8aa4f0, 0x581870);
                                    if (g_game930ad8 == 2) {
                                        sprintf((char *)0x78cf10, (char *)0x8ab154, vd8);
                                    } else {
                                        sprintf((char *)0x78cf10, (char *)0x8ab028,
                                                Game_Helper453f42(v70, g_game5a80d8[vbc], vd8));
                                    }
                                    Game_Fn4259e2(v70, g_game5a80d8[vbc], 1);
                                    v8 = 0;
                                    if (g_game5a7b1c == 1
                                        && ((g_gameSlots[v70][g_game5a80d8[vbc]].flags18 & 0x8000000) == 0
                                            || (g_gameCards[g_gameSlots[v70][g_game5a80d8[vbc]].field6c].script == (int)Game_Callback48592f
                                                && g_gameSlots[v70][g_game5a80d8[vbc]].field38 < 2))) {
                                        v8 = 1;
                                        g_game74b8b8 = 0;
                                        vc8 = a1;
                                        vc4 = g_game5a76d0[0];
                                    }
                                    while (v8 == 0) {
                                        Game_Helper4c0efa(v64, a1, a1, 0x200, 2, 0, 0, 0, 0, 0,
                                                          -1, -1, -1, -1, 0, 2, 0, 0x78cf10,
                                                          (g_gameSlots[v70][g_game5a80d8[vbc]].flags18 & 0x8000000) >> 0x1a,
                                                          &vc8);
                                        if (vc4 == -2) {
                                            v8 = 1;
                                            v10 = 1;
                                            Game_Fn42476a(v70, g_game5a80d8[vbc], vd8);
                                        } else {
                                            for (v6c = 0; v6c < g_game5a7b1c; v6c++) {
                                                if (g_game5a76d0[v6c] == vc4) {
                                                    v8 = 1;
                                                }
                                            }
                                            if (v8 == 0 && g_game8bd200 != 1) {
                                                Game_Helper558356(0x8ab280);
                                                Sleep(0x5dc);
                                                Game_Helper558356(0x581888);
                                            }
                                            if (v8 == 1 && Game_Fn425df8(vc8, vc4) != 0
                                                && (v8 = 0, g_game8bd200 != 1)) {
                                                Game_Helper558356(0x8ab3ac);
                                                Sleep(0x5dc);
                                                Game_Helper558356(0x58188c);
                                            }
                                        }
                                    }
                                    strcpy((char *)0x78cf10, (char *)0x581890);
                                    Game_Fn4259e2(v70, g_game5a80d8[vbc], 0);
                                    if (vc8 != -1 && vc4 != -1 && vc4 != -2) {
                                        for (v6c = 0; v6c < g_game5a7b1c; v6c++) {
                                            if (g_game5a76d0[v6c] == vc4) {
                                                if (g_game74b8b8 == 0
                                                    && (g_gameCards[g_gameSlots[v70][g_game5a80d8[vbc]].field6c].script != (int)Game_Callback48592f
                                                        || 1 < g_gameSlots[v70][g_game5a80d8[vbc]].field38)) {
                                                    vd4 = 1;
                                                } else {
                                                    vd4 = vd8;
                                                }
                                                if (vb0[v6c] == -1) {
                                                    vc = Game_AllocSlot(a1, vc4, vd4, v70,
                                                                        g_game5a80d8[vbc]);
                                                    vb0[v6c] = vc;
                                                    if (vc != -1) {
                                                        g_gameSlots[v70][vc].flags18 |= 0x40000;
                                                        if (v68 == 0) {
                                                            g_gameSlots[v70][vc].flags18 |= 0x100000;
                                                        }
                                                    }
                                                } else {
                                                    g_gameSlots[v70][vb0[v6c]].field38 += vd4;
                                                }
                                                vd8 -= vd4;
                                            }
                                        }
                                    }
                                }
                            }
                            /* Outside the loop, so vbc has run past the end: the
                             * subscript is one back, which the original spells as
                             * a base of 0x5a80d4. */
                            if ((g_gameSlots[v70][g_game5a80d8[vbc - 1]].flags18 & 0x8000000) != 0
                                && vc4 != -2
                                && (g_gameCards[g_gameSlots[v70][g_game5a80d8[vbc - 1]].field6c].script != (int)Game_Callback48592f
                                    || 1 < g_gameSlots[v70][g_game5a80d8[vbc - 1]].field38)
                                && ((v68 == 0
                                     && (g_gameSlots[v70][g_game5a80d8[vbc - 1]].field28 & 0x100) != 0)
                                    || (v68 == 1
                                        && (g_gameSlots[v70][g_game5a80d8[vbc - 1]].field28 & 0x100) == 0))) {
                                Game_Fn42476a(v70, g_game5a80d8[vbc - 1], vd8);
                            }
                        }
                    }
                }
            }
        }
        g_game5a7ba4 = 0;
        for (vcc = 0; vcc < g_game78e970[v70]; vcc++) {
            if (g_gameSlots[v70][vcc].field6c != -1
                && g_gameSlots[v70][vcc].field24 != -1) {
                g_game5a80d8[g_game5a7ba4] = vcc;
                g_game5a7860[g_game5a7ba4] =
                    Game_Helper40d7b2(v70, vcc, 0x33, v60) - g_gameSlots[v70][vcc].field10;
                if (Game_Fn425df8(v70, vcc) != 0) {
                    g_game5a7860[g_game5a7ba4] = 0;
                }
                g_game5a7ba4++;
                if (g_game5a7ba4 == 0x10) {
                    break;
                }
            }
        }
        if (g_game8bd200 != 1) {
            g_game94e964 = 0;
        }
        g_game74b8b4 = 0;
        if (g_game8bd200 != 1) {
            g_game77e590 = 0;
        }
        Game_Fn46ca26(a1);
        if (g_game94e964 == 0 && Game_Fn4038a8(g_game94d464) != 0
            && g_game8bd200 != 1) {
            g_game94e964 = 1;
        }
        for (v6c = 0; v6c < g_game5a7ba4; v6c++) {
            vcc = g_game5a80d8[v6c];
            vd8 = g_game5a7860[v6c];
            for (v60 = 0; v60 < g_game78e970[a1]; v60++) {
                if (g_gameSlots[a1][v60].field3c == g_game93a7d4
                    && (signed char)g_gameSlots[a1][v60].field50 == v70
                    && g_gameSlots[a1][v60].field04 == vcc
                    && ((v68 == 0 && (g_gameSlots[a1][v60].flags18 & 0x100000) != 0)
                        || (v68 == 1 && (g_gameSlots[a1][v60].flags18 & 0x100000) == 0))
                    && (g_gameSlots[a1][v60].flags18 & 0x40000) != 0
                    && (g_gameSlots[a1][v60].flags18 & 0x80000) == 0) {
                    vd8 -= g_gameSlots[a1][v60].field38;
                }
            }
            for (v60 = 0; v60 < g_game78e970[a1]; v60++) {
                if (g_gameSlots[a1][v60].field3c == g_game93a7d4
                    && (signed char)g_gameSlots[a1][v60].field50 == v70
                    && g_gameSlots[a1][v60].field04 == vcc
                    && ((v68 == 0 && (g_gameSlots[a1][v60].flags18 & 0x100000) != 0)
                        || (v68 == 1 && (g_gameSlots[a1][v60].flags18 & 0x100000) == 0))
                    && (g_gameSlots[a1][v60].flags18 & 0x80000) != 0
                    && vd8 - ((signed char)g_gameSlots[a1][v60].field37
                              + g_gameSlots[a1][v60].field38) < 0) {
                    Game_Helper4bab38(v70,
                                      (signed char)g_gameSlots[a1][v60].field37
                                              + g_gameSlots[a1][v60].field38
                                          <= -(vd8 - ((signed char)g_gameSlots[a1][v60].field37
                                                      + g_gameSlots[a1][v60].field38))
                                          ? (signed char)g_gameSlots[a1][v60].field37
                                                + g_gameSlots[a1][v60].field38
                                          : -(vd8 - ((signed char)g_gameSlots[a1][v60].field37
                                                     + g_gameSlots[a1][v60].field38)),
                                      (signed char)g_gameSlots[a1][v60].field0c,
                                      g_gameSlots[a1][v60].field48);
                    vd8 -= (signed char)g_gameSlots[a1][v60].field37
                           + g_gameSlots[a1][v60].field38;
                }
            }
        }
        Game_Helper46db2e();
        Game_Fn46ca26(a1);
        if ((g_game7bfe78 & 1) != 0) {
            g_game7bfe78 &= ~1;
            g_game7bf4c8 = (int)Game_Callback46dbed;
            Game_Helper46db2e();
            Game_Fn46ca26(a1);
        }
        g_game74b8b4 = 0;
        if (g_game8bd200 != 1) {
            g_game77e590 = 0;
        }
        if (g_game94e964 == 0) {
            Game_Fn4038a8(g_game94d464);
        }
        v68++;
    } while (1);
}

/* Redraws the combat screen: mana pools, both sides' stacks of cards, the
 * phase caption, the two life totals, and the button row. The three fillers
 * and the opponent index sit in stack slots the decompiler recovered no use
 * for; they are here to keep the frame the width the prologue reserves. */
// FUNCTION: SHANDALAR 0x00426f3c
void Game_Fn426f3c(int a1, int a2)
{
    int v3c;
    int v38;
    int v34;
    int v30;
    int v2c;
    int v28;
    int v24;
    int v20;
    int v1c;
    int v18;
    int v14;
    int v10;
    int vc;
    int v8;
    int v4;

    if (g_game8bd200 != 1) {
        Game_Helper46ed33();
        *(int *)g_game5832b4 = 1;
        g_game5a8288 = 0;
        v20 = 1 - a1;
        g_game77f1c4 = 0;
        for (v18 = 0; v18 < 2; v18++) {
            if (v18 == g_game8c776c) {
                v3c = 0x174;
            } else {
                v3c = 0xac;
            }
            for (v34 = 0; v34 < 7; v34++) {
                for (v1c = 0; v1c < g_game7806a0[v18][v34]; v1c++) {
                    Game_Helper5797e0(g_game5832b4, 0x68, v3c + 3, 0x14, 0xe,
                                      (-(v18 == 0) & 0x23) - 0x24);
                    Game_Helper57bb90(g_game5832b4, 0x66, v3c, g_game78df40[v34]);
                    v3c -= 0x10;
                }
            }
        }
        v3c = 0x86;
        v38 = 0x82;
        v8 = 0x41;
        v10 = 0x82;
        for (vc = 0; vc <= g_game94cf68[a1]; vc++) {
            for (v1c = g_game78e970[a1] - 1; v1c >= 0; v1c--) {
                if (g_gameSlots[a1][v1c].field6c != -1
                    && g_gameSlots[a1][v1c].field54 == vc) {
                    if ((g_gameSlots[a1][v1c].flags08 & 2) == 0) {
                        Game_Fn4289b1(a1, v1c, (v1c & 0xf) + 0xff, v3c);
                        v3c -= 0x12;
                    } else if (g_gameCards[g_gameSlots[a1][v1c].field6c].flags == 1) {
                        v34 = 1;
                        for (v18 = 0; v18 < 2; v18++) {
                            for (v24 = 0; v24 < g_game78e970[v18]; v24++) {
                                if (g_gameSlots[v18][v24].field04 == v1c
                                    && (signed char)g_gameSlots[v18][v24].field50 == a1
                                    && g_gameSlots[v18][v24].field6c != -1) {
                                    Game_Fn4289b1(v18, v24, 8 / v34 + 1, v38 + 4);
                                    v34++;
                                }
                            }
                        }
                        Game_Fn4289b1(a1, v1c, (v1c & 7) + 1, v38);
                        v38 -= 8;
                    } else if (g_gameSlots[a1][v1c].field04 == -1) {
                        v34 = 1;
                        for (v18 = 0; v18 < 2; v18++) {
                            for (v24 = 0; v24 < g_game78e970[v18]; v24++) {
                                if (g_gameSlots[v18][v24].field04 == v1c
                                    && (signed char)g_gameSlots[v18][v24].field50 == a1
                                    && g_gameSlots[v18][v24].field6c != -1) {
                                    Game_Fn4289b1(v18, v24, v8 + 4 / v34, v10 + 8 / v34);
                                    v34++;
                                }
                            }
                        }
                        Game_Fn4289b1(a1, v1c, v8, v10);
                        v10 -= 6;
                        v8 = 0xe0 < v8 + 0x33 ? v8 - 0x62 : v8 + 0x33;
                    }
                    break;
                }
            }
        }
        v38 = 0x2a;
        v8 = 0x41;
        v10 = 0x2a;
        for (vc = 0; vc <= g_game94cf68[v20]; vc++) {
            for (v1c = g_game78e970[v20] - 1; v1c >= 0; v1c--) {
                if (g_gameSlots[v20][v1c].field6c != -1
                    && g_gameSlots[v20][v1c].field54 == vc
                    && (g_gameSlots[v20][v1c].flags08 & 2) != 0) {
                    if (g_gameCards[g_gameSlots[v20][v1c].field6c].flags == 1) {
                        v34 = 1;
                        for (v18 = 0; v18 < 2; v18++) {
                            for (v24 = 0; v24 < g_game78e970[v18]; v24++) {
                                if (g_gameSlots[v18][v24].field04 == v1c
                                    && (signed char)g_gameSlots[v18][v24].field50 == v20
                                    && g_gameSlots[v18][v24].field6c != -1) {
                                    Game_Fn4289b1(v18, v24, 8 / v34 + 1, v38 + 4);
                                    v34++;
                                }
                            }
                        }
                        Game_Fn4289b1(v20, v1c, (v1c & 7) + 1, v38);
                        v38 -= 8;
                    } else if (g_gameSlots[v20][v1c].field04 == -1) {
                        v34 = 1;
                        for (v18 = 0; v18 < 2; v18++) {
                            for (v24 = 0; v24 < g_game78e970[v18]; v24++) {
                                if (g_gameSlots[v18][v24].field04 == v1c
                                    && (signed char)g_gameSlots[v18][v24].field50 == v20
                                    && g_gameSlots[v18][v24].field6c != -1) {
                                    Game_Fn4289b1(v18, v24, v8 + 4 / v34, v10 + 8 / v34);
                                    v34++;
                                }
                            }
                        }
                        Game_Fn4289b1(v20, v1c, v8, v10);
                        v10 -= 6;
                        v8 = 0xe0 < v8 + 0x33 ? v8 - 0x62 : v8 + 0x33;
                    }
                }
            }
        }
        switch (g_game94d464) {
        case 0:
        case 1:
            strcpy((char *)0x78cf10, (char *)0x5819a8);
            break;
        case 2:
        case 4:
            strcpy((char *)0x78cf10, (char *)0x5819b0);
            break;
        case 10:
            strcpy((char *)0x78cf10, (char *)0x5819b8);
            break;
        case 0x14:
            strcpy((char *)0x78cf10, (char *)0x5819c0);
            break;
        case 0x15:
        case 0x17:
        case 0x19:
        case 0x1a:
        case 0x1b:
            strcpy((char *)0x78cf10, (char *)0x5819c8);
            break;
        case 0x1e:
            strcpy((char *)0x78cf10, (char *)0x5819d0);
            break;
        case 0x1f:
        case 0x20:
        case 0x22:
            strcpy((char *)0x78cf10, (char *)0x5819d8);
            break;
        default:
            strcpy((char *)0x78cf10, (char *)0x5819e0);
            break;
        }
        Game_Helper43104f(0x78cf10, 0x248, 4, (-(g_game7a82e0 == 0) & 0x23) - 0x24);
        if (g_game7a0098 == -1) {
            Game_Helper430ef4(0x97e340, 4, 4, 0);
        } else {
            Game_Helper430ef4((int)g_gameCards[g_game7a0098].unknown001, 4, 4, 0xff);
        }
        strcpy((char *)0x78cf10, (char *)0x5819e8);
        strcat((char *)0x78cf10,
               _itoa(g_game8e3ec8[1] + g_game74cdb4, (char *)0x93a870, 10));
        strcat((char *)0x78cf10, (char *)0x5819ec);
        Game_Helper430ef4(0x78cf10, 4, 0x12, 0xff);
        Game_Helper5797e0(g_game5832b4, 0xfe, 400, 0x84, 0x20, 0xff);
        Game_Helper5797e0(g_game5832b4, 0x100, 0x192, 0x80, 0x1e, 3);
        Game_Fn4310a4(0x5819f4, g_game5863b8 / 2, 0x194, 0xff);
        Game_Helper5797e0(g_game5832b4, 0xfe, 0, 0x84, 0x22, 0);
        Game_Helper5797e0(g_game5832b4, 0x100, 0, 0x80, 0x20, 0xd8);
        Game_Fn4310a4(0x7be9f0, g_game5863b8 / 2, 0x12, 0xdc);
        v28 = 400 / Game_Helper4ecec6(g_game8c83f0[a1], 0x19, 100);
        for (v1c = 0; v1c < g_game8c83f0[a1]; v1c++) {
            Game_Helper57bb90(g_game5832b4,
                              (v28 * v1c + g_game5863b8 / 2) - (g_game8c83f0[a1] * v28) / 2,
                              0x1a0, g_game78df58);
        }
        strcpy((char *)0x78cf10, (char *)0x581a00);
        strcat((char *)0x78cf10,
               _itoa(Game_Helper4ecec6(g_game8c83f0[a1], 0, 99), (char *)0x93a870, 10));
        if (g_game7bfee0[a1] != 0) {
            strcat((char *)0x78cf10, (char *)0x581a04);
            strcat((char *)0x78cf10, _itoa(g_game7bfee0[a1], (char *)0x93a870, 10));
            strcat((char *)0x78cf10, (char *)0x581a08);
        }
        strcat((char *)0x78cf10, (char *)0x581a0c);
        Game_Helper43104f(0x78cf10, g_game5863b8 / 2, 0x1a0, 0);
        v28 = 400 / Game_Helper4ecec6(g_game8c83f0[v20], 0x19, 100);
        for (v1c = 0; v1c < g_game8c83f0[v20]; v1c++) {
            Game_Helper57bb90(g_game5832b4,
                              (v28 * v1c + g_game5863b8 / 2) - (g_game8c83f0[v20] * v28) / 2,
                              4, g_game78df5c);
        }
        strcpy((char *)0x78cf10, (char *)0x581a10);
        strcat((char *)0x78cf10,
               _itoa(Game_Helper4ecec6(g_game8c83f0[v20], 0, 99), (char *)0x93a870, 10));
        if (g_game7bfee0[v20] != 0) {
            strcat((char *)0x78cf10, (char *)0x581a14);
            strcat((char *)0x78cf10, _itoa(g_game7bfee0[v20], (char *)0x93a870, 10));
            strcat((char *)0x78cf10, (char *)0x581a18);
        }
        strcat((char *)0x78cf10, (char *)0x581a1c);
        Game_Helper43104f(0x78cf10, g_game5863b8 / 2, 4, 0xff);
        Game_Fn56c779(0x5a8290);
        if (g_game8c776c == g_game7a82e0) {
            if (g_game94d464 < 0x15) {
                strcpy((char *)0x78cf10, (char *)(0x581a20 + ((g_game77f1c4 != 0) - 1 & 8)));
            } else {
                strcpy((char *)0x78cf10, (char *)(0x581a30 + ((0 < g_game8ca260) - 1 & 8)));
                if (0x1d < g_game94d464) {
                    strcpy((char *)0x78cf10, (char *)0x581a40);
                }
            }
            if ((g_game8c83f8 & 0x80) != 0) {
                Game_Fn426e30(0x78cf10, 0x127, 200);
            }
            if (g_game8c83ec == -1) {
                Game_Fn426e30(0x581a48, 0x10, 200);
            }
            if (g_game8c83ec != -1) {
                Game_Fn426e30(0x581a50, 0x18, 200);
            }
        }
        if (a2 == 0) {
            *(int *)g_game5832b4 = 0;
            Game_Helper579890(g_game5832dc, 0, 0, g_game5863b8, g_game5863bc,
                              g_game5832b4, 0, 0);
        } else {
            Game_Helper579440(1, 2);
            *(int *)g_game5832b4 = 0;
            Game_Helper579890(g_game5832dc, 0, 0, g_game5863b8, g_game5863bc,
                              g_game5832b4, 0, 0);
            Game_Helper579400(0);
        }
        Game_Helper57c800(1, 0x581a58);
        Game_Helper579e40(g_game5832dc, 0, 0, 0x140, 200, g_game5832dc, 0, 0,
                          g_game5863b8, g_game5863bc);
        Game_Fn46ed03();
    }
    return;
}

/* Card script: taps for an effect that targets a permanent, remembering the
 * target in field74/field78 and clearing it again on resolution. */
// FUNCTION: SHANDALAR 0x0042a375
int Game_CardScript42a375(int a, int b, int kind)
{
    int v14;
    int v10;
    int vc;
    int v8;

    if (kind == 0x6c && b == g_game951bdc && a == g_game8e1aa8
        && a == g_game8c7468) {
        g_game94d63c += 0x30;
    }
    if (kind == 0x73) {
        v14 = (g_game8c83f8 & 0x4) != 0;
        if ((g_gameSlots[a][b].flags08 & 0x10) != 0) {
            v14 = 0;
        }
        if (v14 != 0 && (g_gameSlots[a][b].flags08 & 0x30000) != 0
            && (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) != 0) {
            v14 = 0;
        }
        if (v14 != 0) {
            v14 = Game_Helper4be650(0, 1, a, 2, 2, 0x200, 2, 0, 0, 0, 0, 0,
                                    -1, -1, -1, -1, 0, 0, 0);
        }
        if (v14 == 0) {
            return 0;
        }
        return 99;
    }
    if (kind == 0x90) {
        Game_Helper4c4263(0);
        return 0;
    }
    if (kind == 0x6d && (g_game8c83f8 & 0x4) != 0) {
        if (g_game94cf5c == 0) {
            v8 = 0;
            while (v8 == 0) {
                Game_Helper56ca10(0x581c60, 0x581c58);
                if (Game_Helper4c0efa(a, 2, 2, 0x200, 0, 0, 0, 0, 0, 0,
                                      g_game93a7d4, -1, -1, -1, 0, 0, 0,
                                      0x8aa920, 1, &v10) == 0) {
                    g_game8c83ec = 1;
                    v8 = 1;
                } else if (g_gameSlots[v10][vc].field04 == -1) {
                    if (g_game8bd200 != 1 && a != g_game8c7468) {
                        Game_Helper558356(0x8aaa4c);
                        Sleep(2000);
                        Game_Helper558356(0x581c6c);
                    }
                } else {
                    v8 = 1;
                    g_gameSlots[a][b].field74 = v10;
                    g_gameSlots[a][b].field78 = vc;
                    g_gameSlots[a][b].field36 = 1;
                    g_gameSlots[a][b].flags08 |= 0x10;
                    g_game93a794 = -1;
                }
            }
        } else {
            g_gameSlots[a][b].field36 = 0;
            g_game8c83ec = 1;
        }
        return 0;
    }
    if (kind == 0x72 && g_gameSlots[a][b].field36 != 0) {
        v10 = g_gameSlots[a][b].field74;
        vc = g_gameSlots[a][b].field78;
        if (Game_Helper4bea85(v10, vc, 0, a, 2, 2, 0x200, 0, 0, 0, 0, 0, 0,
                              g_game93a7d4, -1, -1, -1, 0, 0, 0) != 0) {
            if (g_gameSlots[v10][vc].field38 < 1) {
                g_game8c83ec = 1;
            } else {
                g_gameSlots[v10][vc].field38--;
            }
        }
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
        return 0;
    }
    if (kind == 0x3b && (g_gameSlots[a][b].flags08 & 0x10) == 0) {
        g_game7beb18[a]++;
    }
    return 0;
}

/* Card script: a four-way activated ability. The 0x6d handler builds the
 * prompt a piece at a time -- one clause per option, each guarded by whether
 * the option is legal -- records the choice in field38, and the 0x72 handler
 * carries it out. */
// FUNCTION: SHANDALAR 0x0042b270
int Game_CardScript42b270(int a, int b, int kind)
{
    int v4e4;
    int v4e0[4];
    int v4d0;
    int v4cc;
    int v4c8;
    int v4c4;
    int v4c0;
    char v4bc[1200];
    int vc;
    int v8;

    if (kind == 0x7f) {
        return Game_Helper42d790(a, b, 0x7f, 0);
    }
    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x10) == 0
                && ((g_gameSlots[a][b].flags08 & 0x30000) == 0
                    || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0)
            || Game_Helper441699(a, 7, 1) != 0
                   && (a != g_game8c7468 || (g_game93a934 & 2) != 0
                       || g_game94d464 < 0x17)) {
            if (a == g_game8c7468 && (g_game93a934 & 2) == 0
                && 0 < g_game94d45c) {
                g_game8c7420 |= 3;
            }
            return 1;
        }
        return 0;
    }
    if (kind != 0x6d) {
        if (kind == 0x72
            && g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field6c != -1) {
            v8 = g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38;
            if (v8 != 0) {
                if (v8 == 1) {
                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field6c =
                        Game_Helper56c705(0x38e);
                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field20 =
                        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field6c;
                    g_game8e3ed0[g_game94f280]++;
                    g_game8e3ed8[g_game94f280]++;
                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].flags08 |= 2;
                    Game_Helper40e3dc(g_game94f280, g_game930f5c, 0x6c,
                                      1 - g_game94f280, -1);
                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].flags08 |= 0x80;
                    Game_Helper40e3dc(g_game94f280, g_game930f5c, 0x71,
                                      1 - g_game94f280, -1);
                } else if (v8 == 2 && g_gameSlots[a][b].field36 != 0) {
                    v4c4 = g_gameSlots[a][b].field74;
                    v4c0 = g_gameSlots[a][b].field78;
                    if (Game_Helper4bea85(v4c4, v4c0, 0, a, 2, 2, 0x200, 0, 0, 0,
                                          Game_Helper488cc4(a, b), 0, 0,
                                          Game_Helper56c705(0x38e), -1, -1, -1,
                                          0, 0, 0) == 0) {
                        g_game8c83ec = 1;
                    } else {
                        vc = Game_Helper4a9910(g_game94f280, g_game930f5c,
                                               g_game8bcff8, v4c4, v4c0);
                        if (vc != -1) {
                            g_gameSlots[a][vc].field12 = 1;
                            g_gameSlots[a][vc].field1c = 1;
                        }
                    }
                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
                }
            }
        }
        if (kind == 0x6c && g_game951bdc == b && a == g_game8e1aa8) {
            g_game94d63c += 0x18;
        }
        if (kind == 0x71) {
            return Game_Helper42d790(a, b, 0x71, 0);
        }
        if (kind == 0x3c && (g_game8c83fa & 0x2) == 0
            && g_game951bdc == b && a == g_game8e1aa8
            && Game_Helper411d90(a, b) != 0
            && g_gameSlots[a][b].field20 != 0) {
            g_game7a01e0 = g_gameSlots[a][b].field20;
        }
        if (kind == 199) {
            if (a == g_game8c7468) {
                g_game94d63c += 0x30;
            } else {
                g_game94d63c -= 0x30;
            }
        }
        if (kind == 1) {
            return Game_Helper42d790(a, b, 1, 0);
        }
        return 0;
    }
    v4d0 = 0;
    if (g_game8bd200 != 1) {
        Game_Helper56ca10(0x581cfc, 0x581cec);
    }
    v4e4 = 3;
    if ((g_gameSlots[a][b].flags08 & 0x10) == 0
        && ((g_gameSlots[a][b].flags08 & 0x30000) == 0
            || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0)) {
        if (g_game8bd200 != 1) {
            sprintf(v4bc, (char *)0x581d08, (char *)0x8aaa4c);
        }
        v4e0[0] = 1;
    } else {
        if (g_game8bd200 != 1) {
            sprintf(v4bc, (char *)0x581d10, (char *)0x8aaa4c);
        }
        v4e0[0] = 0;
    }
    if (Game_Helper441699(a, 7, 1) == 0) {
        if (g_game8bd200 != 1) {
            sprintf(v4bc + strlen(v4bc), (char *)0x581d20, (char *)0x8aab78);
        }
        v4e0[1] = 0;
    } else {
        if (g_game8bd200 != 1) {
            sprintf(v4bc + strlen(v4bc), (char *)0x581d18, (char *)0x8aab78);
        }
        if (g_game94d464 < 0x17) {
            v4e4 = 1;
        }
        v4e0[1] = 1;
    }
    if ((g_gameSlots[a][b].flags08 & 0x10) == 0
        && ((g_gameSlots[a][b].flags08 & 0x30000) == 0
            || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0)
        && Game_Helper4be650(0, 0, a, 2, 2, 0x200, 0, 0, 0,
                             Game_Helper488cc4(a, b), 0, 0,
                             Game_Helper56c705(0x38e), -1, -1, -1, 0, 0, 0) != 0) {
        if (g_game8bd200 != 1) {
            sprintf(v4bc + strlen(v4bc), (char *)0x581d28, (char *)0x8aaca4);
        }
        if (g_game94d464 < 0x1e && 0x16 < g_game94d464) {
            v4e4 = 2;
        }
        v4e0[2] = 1;
    } else {
        if (g_game8bd200 != 1) {
            sprintf(v4bc + strlen(v4bc), (char *)0x581d30, (char *)0x8aaca4);
        }
        v4e0[2] = 0;
    }
    if (g_game8bd200 != 1) {
        sprintf(v4bc + strlen(v4bc), (char *)0x581d38, (char *)0x8aadd0);
    }
    v4e0[3] = 1;
    if (g_game94cf5c == 0) {
        do {
            v8 = Game_Helper558538(a, a, b, -1, -1, (int)v4bc, v4e4);
        } while (v4e0[v8] == 0);
    } else {
        v8 = 0;
    }
    g_gameSlots[a][b].field38 = v8;
    if (v8 == 0) {
        v4d0 = Game_Helper42d790(a, b, 0x6d, 0);
        g_gameSlots[a][b].field36 = 0;
    } else if (v8 == 1) {
        v4cc = g_gameSlots[a][b].flags08;
        g_gameSlots[a][b].flags08 |= 0x40000;
        Game_Helper4143e0(a, 0, 1);
        if ((v4cc & 0x40000) == 0) {
            g_gameSlots[a][b].flags08 &= ~0x40000;
        }
        g_gameSlots[a][b].field36 = 0;
        g_game93a794 = -1;
    } else if (v8 == 2) {
        if (g_game8bd200 != 1) {
            Game_Helper56ca10(0x581d4c, 0x581d3c);
        }
        if (Game_Helper4c0efa(a, 2, a, 0x200, 0, 0, 0,
                              Game_Helper488cc4(a, b), 0, 0,
                              Game_Helper56c705(0x38e), -1, -1, -1, 0, 0, 0,
                              0x8aa920, 1, &v4c4) == 0) {
            g_game8c83ec = 1;
        } else {
            g_gameSlots[a][b].flags08 |= 0x10;
            Game_Helper44157b(a, 0, 1);
            g_gameSlots[a][b].field74 = v4c4;
            g_gameSlots[a][b].field78 = v4c0;
            g_gameSlots[a][b].field36 = 1;
        }
        g_game93a794 = -1;
    } else {
        g_game8c83ec = 1;
    }
    if (0 < g_game94d45c) {
        g_game94d45c--;
    }
    return v4d0;
}

/* Card script: the same four-way ability as 0x0042b270, on a card that also
 * swaps its own card id on entering play (kind 0x22) and restores it when it
 * leaves (kind 0x77). */
// FUNCTION: SHANDALAR 0x0042c3ae
int Game_CardScript42c3ae(int a, int b, int kind)
{
    int v4e4;
    int v4e0[4];
    int v4d0;
    int v4cc;
    int v4c8;
    int v4c4;
    int v4c0;
    char v4bc[1200];
    int vc;
    int v8;

    if (kind == 0x22) {
        g_gameSlots[a][b].field6c = Game_Helper56c705(0x1fc);
        g_gameSlots[a][b].field20 = 0;
        g_game8e3ed8[a]--;
        g_game8e3ed0[a]--;
        g_game97c858 = a;
        g_game97c854 = b;
        Game_Helper49f3c0((int)Game_Callback42d474, -1);
    }
    if (kind == 0x77 && g_game951bdc == b && g_game8e1aa8 == a) {
        g_gameSlots[a][b].field6c = Game_Helper56c705(0x1fc);
        return 0;
    }
    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x10) == 0
                && ((g_gameSlots[a][b].flags08 & 0x30000) == 0
                    || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0)
            || (g_game8c776c == a || (g_game93a934 & 2) != 0)
                   && g_game8bd200 != 1 && g_game939410 == 0
                   && Game_Helper441699(a, 7, 1) != 0) {
            if (g_game8c7468 == a && (g_game93a934 & 2) == 0
                && 0 < g_game94d45c) {
                g_game8c7420 |= 3;
            }
            return 1;
        }
        return 0;
    }
    if (kind != 0x6d) {
        if (kind == 0x72
            && g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field6c != -1) {
            v8 = g_gameSlots[a][b].field38;
            if (v8 != 0) {
                if (v8 == 1) {
                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field6c =
                        Game_Helper56c705(0x38e);
                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field20 =
                        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field6c;
                    g_game8e3ed0[g_game94f280]++;
                    g_game8e3ed8[g_game94f280]++;
                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].flags08 |= 2;
                    Game_Helper40e3dc(g_game94f280, g_game930f5c, 0x6c,
                                      1 - g_game94f280, -1);
                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].flags08 |= 0x80;
                    Game_Helper40e3dc(g_game94f280, g_game930f5c, 0x71,
                                      1 - g_game94f280, -1);
                } else if (v8 == 2 && g_gameSlots[a][b].field36 != 0) {
                    v4c4 = g_gameSlots[a][b].field74;
                    v4c0 = g_gameSlots[a][b].field78;
                    if (Game_Helper4bea85(v4c4, v4c0, 0, a, 2, 2, 0x200, 0, 0, 0,
                                          Game_Helper488cc4(a, b), 0, 0,
                                          Game_Helper56c705(0x38e), -1, -1, -1,
                                          0, 0, 0) == 0) {
                        g_game8c83ec = 1;
                    } else {
                        vc = Game_Helper4a9910(g_game94f280, g_game930f5c,
                                               g_game8bcff8, v4c4, v4c0);
                        if (vc != -1) {
                            g_gameSlots[a][vc].field12 = 1;
                            g_gameSlots[a][vc].field1c = 1;
                        }
                    }
                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
                }
            }
        }
        if (kind == 0x7f) {
            return Game_Helper42d790(a, b, 0x7f, 0);
        }
        if (kind == 1) {
            return Game_Helper42d790(a, b, 1, 0);
        }
        if (kind == 0x71) {
            return Game_Helper42d790(a, b, 0x71, 0);
        }
        return 0;
    }
    v4d0 = 0;
    if (g_game8bd200 != 1) {
        Game_Helper56ca10(0x581d68, 0x581d58);
    }
    v4e4 = 3;
    if ((g_gameSlots[a][b].flags08 & 0x10) == 0
        && ((g_gameSlots[a][b].flags08 & 0x30000) == 0
            || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0)) {
        if (g_game8bd200 != 1) {
            sprintf(v4bc, (char *)0x581d74, (char *)0x8aaa4c);
        }
        v4e0[0] = 1;
    } else {
        if (g_game8bd200 != 1) {
            sprintf(v4bc, (char *)0x581d7c, (char *)0x8aaa4c);
        }
        v4e0[0] = 0;
    }
    if (Game_Helper441699(a, 7, 1) == 0) {
        if (g_game8bd200 != 1) {
            sprintf(v4bc + strlen(v4bc), (char *)0x581d8c, (char *)0x8aab78);
        }
        v4e0[1] = 0;
    } else {
        if (g_game8bd200 != 1) {
            sprintf(v4bc + strlen(v4bc), (char *)0x581d84, (char *)0x8aab78);
        }
        v4e0[1] = 1;
    }
    if ((g_gameSlots[a][b].flags08 & 0x10) == 0
        && ((g_gameSlots[a][b].flags08 & 0x30000) == 0
            || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0)
        && Game_Helper4be650(0, 0, a, 2, 2, 0x200, 0, 0, 0,
                             Game_Helper488cc4(a, b), 0, 0,
                             Game_Helper56c705(0x38e), -1, -1, -1, 0, 0, 0) != 0) {
        if (g_game8bd200 != 1) {
            sprintf(v4bc + strlen(v4bc), (char *)0x581d94, (char *)0x8aaca4);
        }
        if (g_game94d464 < 0x1e && 0x16 < g_game94d464) {
            v4e4 = 2;
        }
        v4e0[2] = 1;
    } else {
        if (g_game8bd200 != 1) {
            sprintf(v4bc + strlen(v4bc), (char *)0x581d9c, (char *)0x8aaca4);
        }
        v4e0[2] = 0;
    }
    v4e0[3] = 1;
    if (g_game8bd200 != 1) {
        sprintf(v4bc + strlen(v4bc), (char *)0x581da4, (char *)0x8aadd0);
    }
    if (g_game94cf5c == 0) {
        do {
            v8 = Game_Helper558538(a, a, b, -1, -1, (int)v4bc, v4e4);
        } while (v4e0[v8] == 0);
    } else {
        v8 = 0;
    }
    g_gameSlots[a][b].field38 = v8;
    if (v8 == 0) {
        v4d0 = Game_Helper42d790(a, b, 0x6d, 0);
        g_gameSlots[a][b].field36 = 0;
    } else if (v8 == 1) {
        v4cc = g_gameSlots[a][b].flags08;
        g_gameSlots[a][b].flags08 |= 0x40000;
        Game_Helper4143e0(a, 0, 1);
        if ((v4cc & 0x40000) == 0) {
            g_gameSlots[a][b].flags08 &= ~0x40000;
        }
        g_gameSlots[a][b].field36 = 0;
        g_game93a794 = -1;
    } else if (v8 == 2) {
        if (g_game8bd200 != 1) {
            Game_Helper56ca10(0x581db8, 0x581da8);
        }
        if (Game_Helper4c0efa(a, 2, a, 0x200, 0, 0, 0,
                              Game_Helper488cc4(a, b), 0, 0,
                              Game_Helper56c705(0x38e), -1, -1, -1, 0, 0, 0,
                              0x8aa920, 1, &v4c4) == 0) {
            g_game8c83ec = 1;
        } else {
            g_gameSlots[a][b].flags08 |= 0x10;
            Game_Helper44157b(a, 0, 1);
            g_gameSlots[a][b].field74 = v4c4;
            g_gameSlots[a][b].field78 = v4c0;
            g_gameSlots[a][b].field36 = 1;
        }
        g_game93a794 = -1;
    } else {
        g_game8c83ec = 1;
    }
    if (0 < g_game94d45c) {
        g_game94d45c--;
    }
    return v4d0;
}
