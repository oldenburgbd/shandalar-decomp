/* Shandalar game code, 0x0041xxxx range -- the mana-tapping / AI-evaluation
 * cluster, newly derived from Ghidra's decompilation of orig/Shandalar.exe.
 *
 * Nothing here was reconstructed before. The functions are, in address order:
 *   0x004143e0  Game_Helper4143e0  -- pay a mana cost (already declared in
 *                                     gameslot.h under that name)
 *   0x00415fd7  Game_Helper415fd7  -- tap sources matching a colour mask
 *   0x00419599  Game_Helper419599  -- a card script (kind 0x71/0x73/0x6d/0x72)
 *   0x0041ab5e  Game_Helper41ab5e  -- 20-way random-effect dispatcher
 *   0x0041c208  Game_Helper41c208  -- 16-way random-effect dispatcher
 *   0x0041d33b  Game_Helper41d33b  -- fills the per-slot AI evaluation table
 *   0x0041e0d9  Game_Helper41e0d9  -- builds the attacker/blocker work arrays
 *   0x0041f5e3  Game_Helper41f5e3  -- searches attack subsets, returns a mask
 *
 * Translation notes that apply throughout:
 *
 *  - Byte-granular reads of a wider field are written against the wider field,
 *    which is what the existing cards*.c do: Ghidra's
 *    `(&DAT_008ca27a)[..] & 3` is `flags08 & 0x30000`, `(&DAT_00594245)[..] &
 *    0x10` is `flags3c & 0x1000`, and so on.
 *  - DAT_008e26a8 / DAT_008e26ac are g_game8e2690[6] / [7]: 0x008e2690 + 6*4
 *    and + 7*4, and the surrounding loops walk the same array with an index.
 *  - Address literals (string constants, the 0x008aa920 out-buffer, the
 *    0x0078cf10 block) are passed as plain ints, as elsewhere in this tree.
 *  - Ghidra splits a nested __cdecl call's argument list across the two
 *    callees. Recombined here from the stack cleanup, the way helpers4e.c
 *    already does for the same 0x004be650 / 0x004bea85 / 0x004c0efa shapes:
 *    Game_Helper488cc4 takes two arguments and the rest belong to the outer
 *    call.
 *  - Locals keep Ghidra's declaration order and are named after their Ghidra
 *    frame offsets (v138, vd8, ...), so the mapping back to the decompilation
 *    is mechanical. Where Ghidra carried a value in a register across
 *    statements that /Od cannot, an extra local is added at the end of the
 *    list and marked.
 *  - Locals read before any write, and index expressions that run past the end
 *    of a Ghidra-split array, are reproduced as they are: they are how the
 *    original behaves.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "game/helpers.h"

/* Referenced by this TU only; not in the shared headers. Addresses from the
   disassembly. */

/* Scalars. */
extern int g_game74b8b8;          /* 0x0074b8b8 */
extern int g_game74b15c;          /* 0x0074b15c */
extern int g_game77e6cc;          /* 0x0077e6cc */
extern int g_game780690;          /* 0x00780690 */
extern int g_game7658c0;          /* 0x007658c0 */
extern int g_game94cf5c;          /* 0x0094cf5c */
extern int g_game789900;          /* 0x00789900 -- also in gamefns.h */
extern int g_game8f3224;          /* 0x008f3224 */
extern int g_game8e3f20;          /* 0x008e3f20 */
extern int g_game581068;          /* 0x00581068 */
extern int g_game97f1b0;          /* 0x0097f1b0 */
extern int g_game97db20;          /* 0x0097db20 */
extern int g_game97db24;          /* 0x0097db24 */
extern int g_game97db28;          /* 0x0097db28 */
extern int g_game97db2c;          /* 0x0097db2c */
extern int g_game97db38;          /* 0x0097db38 */

/* Tables reached with a constant stride of four. */
extern int g_game580fd8[32];      /* 0x00580fd8 */
extern int g_game581808[32];      /* 0x00581808 */
extern int g_game7beaa0[8];       /* 0x007beaa0 */
extern int g_game7beaa8[8];       /* 0x007beaa8 */

/* The combat work area at 0x005a7610. Twenty-one parallel arrays of sixteen
   ints, packed 0x40 apart, with scalars in the gaps; the two seven-element
   ones are bounded by the scalar that follows them (and by the 0x1c memsets
   at 0x0041e0d9). */
extern int g_game5a7610[16];      /* 0x005a7610 */
extern int g_game5a7650[16];      /* 0x005a7650 */
extern int g_game5a7690[16];      /* 0x005a7690 */
extern int g_game5a76d0[16];      /* 0x005a76d0 */
extern int g_game5a7790[16];      /* 0x005a7790 */
extern int g_game5a7818;          /* 0x005a7818 */
extern int g_game5a7820[16];      /* 0x005a7820 */
extern int g_game5a7860[16];      /* 0x005a7860 */
extern int g_game5a78a0[16];      /* 0x005a78a0 */
extern int g_game5a78e0;          /* 0x005a78e0 */
extern int g_game5a78e8[16];      /* 0x005a78e8 */
extern int g_game5a7930[16];      /* 0x005a7930 */
extern int g_game5a79f0;          /* 0x005a79f0 */
extern int g_game5a79f4;          /* 0x005a79f4 */
extern int g_game5a7a78;          /* 0x005a7a78 */
extern int g_game5a7a80[16];      /* 0x005a7a80 */
extern int g_game5a7ac0[16];      /* 0x005a7ac0 */
extern int g_game5a7b00[7];       /* 0x005a7b00 */
extern int g_game5a7b1c;          /* 0x005a7b1c */
extern int g_game5a7b20[16];      /* 0x005a7b20 */
extern int g_game5a7b60[16];      /* 0x005a7b60 */
extern int g_game5a7ba0;          /* 0x005a7ba0 */
extern int g_game5a7ba4;          /* 0x005a7ba4 */
extern int g_game5a7ba8[7];       /* 0x005a7ba8 */
extern int g_game5a8010[16];      /* 0x005a8010 */
extern int g_game5a8050[16];      /* 0x005a8050 */
extern int g_game5a8090;          /* 0x005a8090 */
extern int g_game5a8098[16];      /* 0x005a8098 */
extern int g_game5a80d8[16];      /* 0x005a80d8 */
extern int g_game5a8118;          /* 0x005a8118 */
extern int g_game5a811c;          /* 0x005a811c */
extern int g_game5a8120[16];      /* 0x005a8120 */

/* The per-slot AI evaluation table at 0x0097c860. Sixteen-byte records in rows
   of 150: the column index is scaled by 0x10 and the row by 0x960, and
   0x960 / 0x10 is 150. 0x0041d33b clears the whole thing with a single
   memset of 0x12c0 == 2 * 0x960, which is what fixes the row count at two. */
typedef struct GameEvalRec {
    int          field00;    /* +0x0  power-ish accumulator                  */
    int          field04;    /* +0x4  toughness-ish accumulator              */
    unsigned int flags08;    /* +0x8  ability bits, OR-accumulated           */
    int          field0c;    /* +0xc  cached evaluation, 0 until computed    */
} GameEvalRec;

extern GameEvalRec g_game97c860[2][150];

/* Functions this TU calls that no header declares. Argument counts are from
   the call sites' stack cleanup. */
int Game_Helper4155ee(int a1, int *a2, int *a3, int a4);
int Game_Helper415794(int a1, int *a2, int *a3, int a4, int *a5, int a6);
int Game_Helper41598e(int a1, int *a2, int *a3, int a4, int *a5, int a6);
int Game_Helper415b89(int *a1, int a2, int a3);
int Game_Helper415c24(int *a1);
int Game_Helper415c7c(int a1, int *a2, int a3, int a4);
int Game_Helper416b1c(int a1, int a2, int a3);
int Game_Helper416d5a(int a1, int a2);
int Game_Helper416ff1(int *a1, int a2, int a3, int *a4, int a5, int a6, int a7, int *a8, int *a9);
int Game_Helper417061(int *a1, int a2, int *a3, int a4, int a5, int a6, int a7);
int Game_Helper41728a(int *a1);
int Game_Helper419310(int a1, int a2, int a3);
int Game_Helper41d290(int a1, int a2);
int Game_Helper421578(int a1);
int Game_Helper4257a4(int a1);
int Game_Helper425a82(int a1, int a2);
int Game_Helper425c7e(int a1, int a2);
int Game_Helper40d614(void);
int Game_Helper40fd9f(void);
int Game_Helper411b19(int a1, int a2, int a3, int a4);
int Game_Helper451e70(void);
int Game_Helper46ca26(int a1);
int Game_Helper4c4457(int a1);
int Game_Helper507a50(int a1, int a2, int a3, int a4, int a5, int a6, int a7);
int Game_Helper5225a9(int a1);
int Game_Helper53f510(int a1, int a2, int a3);
/* Declared in gamefns.h, which this TU does not include. */
int Game_Fn40a561(int a1, int a2);
int Game_Fn4c3b83(void);
int Game_Fn4c3e34(void);
int Game_Fn51dcf2(int a1, int a2, int a3);

/* Defined further down this file; 0x004143e0 calls it. */
int Game_Helper415fd7(int a1, int *a2, int *a3, int a4, int a5);

/* Card scripts 0x0041d33b compares a card's +0x34 handler against. Declared
   here rather than in gameslot.h, per the note there: a script that has to be
   named outside its own cards*.c gets a declaration in the one file that
   needs it. */
int Game_CardScript48b2c1(int a, int b, int kind);
int Game_CardScript494312(int a, int b, int kind);
int Game_CardScript494f82(int a, int b, int kind);
int Game_CardScript4dcace(int a, int b, int kind);
int Game_CardScript4dd9ef(int a, int b, int kind);
int Game_CardScript4de7f8(int a, int b, int kind);
int Game_CardScript4df65e(int a, int b, int kind);
int Game_CardScript4e8d27(int a, int b, int kind);
int Game_CardScript4e9ad4(int a, int b, int kind);
int Game_CardScript4ea2d4(int a, int b, int kind);
int Game_CardScript4ea30b(int a, int b, int kind);
int Game_CardScript4ea342(int a, int b, int kind);
int Game_CardScript4ea379(int a, int b, int kind);
int Game_CardScript4ea3b0(int a, int b, int kind);

/* Pays a3 mana of colour a2 for player a1: normalises the pool against the
 * doubling at g_game8e2690[7], runs the four tap passes (0x004155ee,
 * 0x00415794, 0x0041598e and this file's 0x00415fd7) until 0x00415b89 says
 * the cost is covered, and otherwise hands control to the interactive picker
 * at 0x00507a50. On failure (g_game8c83ec == 1) it refunds through
 * 0x0041728a / 0x004415c5. Returns the accumulated value in v10. */
// FUNCTION: SHANDALAR 0x004143e0
int Game_Helper4143e0(int a1, int a2, int a3)
{
    int vd8;
    int vd4[8];
    int vb4;
    int vb0[8];
    int v90;
    int v8c;
    int v88;
    int v84;
    int v80;
    unsigned int v7c;
    unsigned int v78;
    int v74;
    int v70;
    int v6c;
    int v68;
    int v64;
    int v60;
    int v5c;
    int v58;
    int v54;
    int v50[7];
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

    if ((g_game8c83f9 & 0x4) == 0) {
        if (0 < g_game8e2690[7]) {
            if (a2 == 0) {
                if (a3 != -1) {
                    a3 = a3 + g_game8e2690[7] * -2;
                    if (a3 < 2) {
                        a3 = 1;
                    }
                }
            } else {
                if (0 < g_game8e2690[0]) {
                    g_game8e2690[0] = g_game8e2690[0] + g_game8e2690[7] * -2;
                    if (g_game8e2690[0] < 2) {
                        g_game8e2690[0] = 1;
                    }
                }
            }
        }
        g_game8e2690[a2] = g_game8e2690[a2] + a3;
        v10 = 0;
        for (v1c = 0; v1c < 7; v1c++) {
            v50[v1c] = 0;
        }
        v24 = Game_Helper441699(a1, 6, 1) - Game_Helper441699(a1, 7, 1);
        v8 = 1;
        if (g_game74b8b8 == 0
            && (g_game8c7468 != a1 || (g_game93a934 & 0x2) != 0)
            && g_game8bd200 != 1
            && g_game939410 == 0) {
            v28 = 0;
            v2c = 0;
            v34 = 0;
            v54 = 0;
        } else {
            v54 = 1;
            v34 = 1;
            v2c = 1;
            if ((g_game8c7468 == a1 && (g_game93a934 & 0x2) == 0)
                || g_game8bd200 == 1
                || g_game939410 != 0) {
                v28 = 1;
            } else {
                v28 = 0;
            }
        }
        vc = 1;
        if (g_game8c7468 == a1 && (g_game93a934 & 0x2) == 0) {
            for (v1c = 0; v1c < 7; v1c++) {
                if (g_game8e2690[v1c] == -1) {
                    v64 = Game_Helper441699(a1, v1c, 1);
                    if (g_game8bd200 == 1) {
                        if (Game_Helper522508(3) == 0 || v64 < 2) {
                            v60 = v64;
                        } else {
                            v60 = Game_Helper522508(v64 - 1);
                            v60 = v60 + 1;
                        }
                        if (g_game8e26bc != -1 && g_game8e26bc <= v60) {
                            v60 = g_game8e26bc;
                        }
                        g_game94d46c = v60;
                        Game_Helper4c4181();
                    } else {
                        Game_Helper4c42f8();
                        if (g_game94d46c == 99) {
                            g_game94d46c = 0;
                        }
                        v60 = g_game94d46c;
                    }
                }
            }
            g_game8e26bc = v60;
        }
        if (g_game8e2690[7] < 1) {
            g_game8c8274 = 0;
        } else {
            g_game8c8274 = g_game8e2690[7] * 2;
        }
        if (g_game8e26bc == 0) {
            for (v1c = 0; v1c < 7; v1c++) {
                if (g_game8e2690[v1c] == -1) {
                    g_game8e2690[v1c] = 0;
                }
            }
        }
        v58 = 0;
        for (v1c = 0; v1c < 7; v1c++) {
            if (g_game8e2690[v1c] == -1) {
                v58 = 1;
            }
        }
        if (v8 != 0
            && Game_Helper415b89(g_game8e2690, g_game8c8274, g_game8e26bc) == 0) {
            Game_Helper4155ee(a1, v50, &v10, v24);
            Game_Helper451e70();
        }
        if (v54 != 0
            && Game_Helper415b89(g_game8e2690, g_game8c8274, g_game8e26bc) == 0
            && v58 != 0) {
            Game_Helper415794(a1, v50, &v10, v24, &g_game8c8274, g_game8e26bc);
            Game_Helper451e70();
        }
        if (v34 != 0
            && Game_Helper415b89(g_game8e2690, g_game8c8274, g_game8e26bc) == 0) {
            Game_Helper41598e(a1, v50, &v10, v24, &g_game8c8274, g_game8e26bc);
            Game_Helper451e70();
        }
        if (v2c != 0
            && Game_Helper415b89(g_game8e2690, g_game8c8274, g_game8e26bc) == 0) {
            if (Game_Helper415b89(g_game8e2690, g_game8c8274, g_game8e26bc) == 0) {
                Game_Helper415fd7(a1, v50, &v10, 0x1e, vc);
            }
            if (Game_Helper415b89(g_game8e2690, g_game8c8274, g_game8e26bc) == 0) {
                Game_Helper415fd7(a1, v50, &v10, 0x1c, vc);
            }
        }
        if (v28 != 0
            && Game_Helper415b89(g_game8e2690, g_game8c8274, g_game8e26bc) == 0) {
            if (Game_Helper415b89(g_game8e2690, g_game8c8274, g_game8e26bc) == 0) {
                Game_Helper415fd7(a1, v50, &v10, 0x14, vc);
            }
            if (Game_Helper415b89(g_game8e2690, g_game8c8274, g_game8e26bc) == 0) {
                Game_Helper415fd7(a1, v50, &v10, 4, vc);
            }
            if (Game_Helper415b89(g_game8e2690, g_game8c8274, g_game8e26bc) == 0) {
                Game_Helper415fd7(a1, v50, &v10, 0x1a, vc);
            }
            if (Game_Helper415b89(g_game8e2690, g_game8c8274, g_game8e26bc) == 0) {
                Game_Helper415fd7(a1, v50, &v10, 0x18, vc);
            }
            if (Game_Helper415b89(g_game8e2690, g_game8c8274, g_game8e26bc) == 0) {
                Game_Helper415fd7(a1, v50, &v10, 0x10, vc);
            }
            if (Game_Helper415b89(g_game8e2690, g_game8c8274, g_game8e26bc) == 0) {
                Game_Helper415fd7(a1, v50, &v10, 0, vc);
            }
        }
        if ((g_game8c7468 == a1 && (g_game93a934 & 0x2) == 0)
            || g_game8bd200 == 1
            || g_game939410 != 0) {
            if (Game_Helper415b89(g_game8e2690, g_game8c8274, g_game8e26bc) == 0
                && v58 == 0) {
                g_game8c83ec = 1;
            }
        } else {
            if (Game_Helper415b89(g_game8e2690, g_game8c8274, g_game8e26bc) == 0) {
                v5c = 0;
                while (v5c == 0
                       && Game_Helper415b89(g_game8e2690, g_game8c8274, g_game8e26bc) == 0) {
                    v68 = 1;
                    for (v1c = 0; v1c < 7; v1c++) {
                        if (0 < g_game8e2690[v1c]) {
                            v68 = 0;
                        }
                    }
                    Game_Helper415c7c(0x78cf10, g_game8e2690, g_game8c8274, g_game8e26bc);
                    v30 = Game_Helper507a50(a1, a1, a1, 0, 0, 0x78cf10,
                                            v68 == 0 ? 3 : 1);
                    if (g_game77e6cc == -1 && (v30 == -1 || v30 == -2)) {
                        if (g_game780690 == -2) {
                            if (g_game74d26c == -1 && g_game74f1e0 == -1) {
                                if (v30 == -1) {
                                    g_game8c83ec = 1;
                                }
                                v5c = 1;
                            } else {
                                if (v68 != 0) {
                                    v5c = 1;
                                }
                            }
                        } else {
                            if (g_game780690 == -3
                                && g_game74b15c == a1
                                && g_game7658c0 != -1) {
                                if (0 < g_game7806a0[a1][g_game7658c0]
                                    && (g_game8e2690[g_game7658c0] != 0
                                        || g_game8e2690[0] != 0
                                        || g_game8e2690[6] != 0)
                                    && (g_game7658c0 != 6 || g_game8e2690[6] != 0)) {
                                    if (g_game8e2690[g_game7658c0] == 0) {
                                        if (g_game8e2690[6] == 0) {
                                            v6c = 0;
                                        } else {
                                            v6c = 6;
                                        }
                                    } else {
                                        v6c = g_game7658c0;
                                    }
                                    v70 = Game_Helper417061(g_game8e2690, v6c,
                                                            g_game7806a0[a1],
                                                            g_game7658c0, g_game74b8b8,
                                                            g_game8e26bc, g_game8c8274);
                                    Game_Helper416ff1(g_game8e2690, v6c, v70,
                                                      &g_game8c8274, g_game8e26bc, a1,
                                                      g_game7658c0, v50, &v10);
                                    Game_Helper451e70();
                                }
                                if (0 < g_game7806a0[a1][g_game7658c0]) {
                                    v78 = 0;
                                    v20 = 0;
                                    while (v20 < 10 && g_game74cdc0[a1][v20] != -1) {
                                        v80 = (unsigned short)g_game74cdc0[a1][v20];
                                        if (g_game7658c0 == v80) {
                                            v84 = (unsigned int)g_game74cdc0[a1][v20] >> 0x10;
                                            v78 = v78 | 1 << (v84 & 0x1f);
                                        }
                                        v20++;
                                        v84 = (unsigned int)g_game74cdc0[a1][v20 - 1] >> 0x10;
                                    }
                                    v7c = 0;
                                    for (v1c = 0; v1c < 7; v1c++) {
                                        if (g_game8e2690[v1c] != 0) {
                                            v7c = v7c | 1 << (v1c & 0x1f);
                                        }
                                    }
                                    v78 = v78 & v7c;
                                    if (v78 != 0) {
                                        v74 = 0;
                                        for (v1c = 0; v1c < 7; v1c++) {
                                            if ((v78 & 1 << (v1c & 0x1f)) != 0) {
                                                v74 = v74 + 1;
                                            }
                                        }
                                        if (v74 == 1) {
                                            v88 = Game_Helper40dffd(v78);
                                        } else {
                                            Game_Helper56ca10(0x8aa4f0, 0x580db0);
                                            v88 = Game_Helper5589ed(a1, 0x8aa920, 1,
                                                                    g_game7658c0, v78);
                                        }
                                        v8c = Game_Helper417061(g_game8e2690, v88,
                                                                g_game7806a0[a1],
                                                                g_game7658c0,
                                                                g_game74b8b8,
                                                                g_game8e26bc,
                                                                g_game8c8274);
                                        Game_Helper416ff1(g_game8e2690, v88, v8c,
                                                          &g_game8c8274, g_game8e26bc,
                                                          a1, g_game7658c0, v50, &v10);
                                        Game_Helper451e70();
                                    }
                                }
                            }
                        }
                    } else if (g_game77e6cc == -1 || v30 != -1) {
                        v14 = g_gameSlots[a1][v30].field6c;
                        if ((g_gameCards[v14].flags3c & 0x1000) != 0
                            && ((g_gameCards[v14].flags & 0x20) != 0
                                || ((g_gameSlots[a1][v30].flags08 & 0x2) != 0
                                    && (g_gameSlots[a1][v30].flags08 & 0x800000) == 0
                                    && (g_gameSlots[a1][v30].flags08 & 0x10) == 0
                                    && ((g_gameSlots[a1][v30].flags08 & 0x30000) == 0
                                        || (g_gameCards[g_gameSlots[a1][v30].field6c].flags & 0x2) == 0)))) {
                            for (vd8 = 0; vd8 < 8; vd8++) {
                                vd4[vd8] = g_game7806a0[a1][vd8];
                            }
                            vb4 = g_game8c8274;
                            g_game8c8274 = 0;
                            v90 = g_game8e26bc;
                            g_game8e26bc = -1;
                            for (vd8 = 0; vd8 < 7; vd8++) {
                                vb0[vd8] = g_game8e2690[vd8];
                                g_game8e2690[vd8] = 0;
                            }
                            if ((g_gameSlots[a1][v30].flags08 & 0x800002) == 2) {
                                if ((g_gameCards[v14].flags & 0x1) != 0
                                    || Game_Helper40e3dc(a1, v30, 0x73, 1 - a1, -1) != 0) {
                                    Game_LogEvent(a1, v30, 0x72, a1, 0);
                                    g_game94cf5c = Game_Helper415c24(vb0);
                                    g_game93a794 = -1;
                                    v18 = g_gameSlots[a1][v30].flags08 & 0x10;
                                    Game_Helper40e3dc(a1, v30, 0x6d, 1 - a1, -1);
                                    g_game94cf5c = 0;
                                    if (g_game8c83ec == 1) {
                                        g_game8c83ec = 0;
                                        Game_Helper41040d();
                                    } else {
                                        if (v18 == 0
                                            && (g_gameSlots[a1][v30].flags08 & 0x10) != 0) {
                                            Game_Helper40e158(a1, v30, 0x81);
                                        }
                                        if (g_game8bd200 != 1) {
                                            Game_Helper40ebb9(0x12);
                                        }
                                        Game_Helper40fd9f();
                                        Game_Helper558aea(0, 0xff);
                                    }
                                }
                            } else {
                                Game_Fn40a561(a1, v30);
                                Game_Helper558aea(0, 0xff);
                            }
                            g_game8c8274 = vb4;
                            g_game8e26bc = v90;
                            for (vd8 = 0; vd8 < 7; vd8++) {
                                g_game8e2690[vd8] = vb0[vd8];
                            }
                            for (vd8 = 0; vd8 < 8; vd8++) {
                                g_game7806a0[a1][vd8] = g_game7806a0[a1][vd8] - vd4[vd8];
                            }
                            v24 = 0;
                            Game_Helper4155ee(a1, v50, &v10, 0);
                            Game_Helper415794(a1, v50, &v10, v24, &g_game8c8274, g_game8e26bc);
                            Game_Helper41598e(a1, v50, &v10, v24, &g_game8c8274, g_game8e26bc);
                            for (vd8 = 0; vd8 < 8; vd8++) {
                                g_game7806a0[a1][vd8] = g_game7806a0[a1][vd8] + vd4[vd8];
                            }
                            Game_Helper451e70();
                        }
                    }
                }
            }
        }
    }
    if (g_game8c83ec == 1) {
        Game_Helper41728a(v50);
        for (v1c = 0; v1c < 7; v1c++) {
            Game_Helper4415c5(a1, v1c, v50[v1c]);
            v50[v1c] = 0;
        }
        v10 = 0;
        g_game8c8274 = 0;
    }
    for (v1c = 0; v1c < 8; v1c++) {
        g_game8e2690[v1c] = 0;
    }
    g_game8e26bc = -1;
    return v10;
}

/* One tap pass for player a1 over the slots whose kind a4 selects. Three
 * sweeps: exact colour matches, the colourless pool, and the "any colour"
 * entries; each candidate that survives 0x00416b1c and the 0x0073 query is
 * tapped through 0x00416d5a and the pool re-summed. When the player is the
 * one holding g_game93a934 bit 1 the sweeps are skipped and the choice comes
 * from g_game7bea9c instead. Always returns 1.
 *
 * a2 is the seven-entry per-colour accumulator and a3 the running total,
 * both owned by the caller at 0x004143e0. */
// FUNCTION: SHANDALAR 0x00415fd7
int Game_Helper415fd7(int a1, int *a2, int *a3, int a4, int a5)
{
    int v28;
    int v20;
    int v18;
    int v14;
    int vc;
    /* Ghidra's bVar1/bVar2; /Od keeps both on the stack. */
    int v8;
    int v4;

    v8 = 0;
    for (v14 = 0; v14 < 7; v14++) {
        if (g_game8e2690[v14] == -1) {
            v8 = 1;
        }
    }
    if (g_game8c7468 == a1 && (g_game93a934 & 0x2) != 0) {
        vc = 0;
        while (vc != -1) {
            Game_Helper56c91f(a1, 0x10);
            vc = g_game7bea9c;
            if (g_game7bea9c != -1 && Game_Helper416d5a(a1, g_game7bea9c) != 0) {
                Game_Helper4155ee(a1, a2, a3,
                                  Game_Helper441699(a1, 6, 1) - Game_Helper441699(a1, 7, 1));
                Game_Helper415794(a1, a2, a3,
                                  Game_Helper441699(a1, 6, 1) - Game_Helper441699(a1, 7, 1),
                                  &g_game8c8274, g_game8e26bc);
                Game_Helper41598e(a1, a2, a3,
                                  Game_Helper441699(a1, 6, 1) - Game_Helper441699(a1, 7, 1),
                                  &g_game8c8274, g_game8e26bc);
            }
        }
        Game_Helper451e70();
    } else {
        if (Game_Helper415b89(g_game8e2690, g_game8c8274, g_game8e26bc) == 0) {
            for (vc = 0; vc < g_game78e970[a1]; vc++) {
                if (Game_Helper416b1c(a1, vc, a4) != 0
                    && Game_Helper40e3dc(a1, vc, 0x73, 1 - a1, -1) != 0) {
                    v4 = 0;
                    for (v14 = 0; v14 < 7; v14++) {
                        if (g_game8e2690[v14] < 1
                            || (1 << (v14 & 0x1f) & (signed char)g_gameSlots[a1][vc].field58) == 0) {
                            if (g_game8e2690[v14] == -1
                                && g_game8c8274 < g_game8e26bc
                                && g_game8e26bc != -1
                                && (1 << (v14 & 0x1f) & (signed char)g_gameSlots[a1][vc].field58) != 0) {
                                v4 = 1;
                            }
                        } else {
                            v4 = 1;
                        }
                    }
                    if (a5 != 0 && v4 == 0) {
                        v18 = 0;
                        while (v18 < 10 && g_game74cdc0[a1][v18] != -1) {
                            v20 = (unsigned char)g_game74cdc0[a1][v18];
                            if (g_game8e2690[(unsigned int)g_game74cdc0[a1][v18] >> 0x10] < 1
                                || (1 << (v20 & 0x1f) & (signed char)g_gameSlots[a1][vc].field58) == 0) {
                                if (g_game8e2690[(unsigned int)g_game74cdc0[a1][v18] >> 0x10] == -1
                                    && g_game8c8274 < g_game8e26bc
                                    && g_game8e26bc != -1
                                    && (1 << (v20 & 0x1f) & (signed char)g_gameSlots[a1][vc].field58) != 0) {
                                    v4 = 1;
                                }
                            } else {
                                v4 = 1;
                            }
                            v18++;
                        }
                    }
                    if (v4 != 0) {
                        if ((g_game93a934 & 0x2) != 0) {
                            g_game7bea98 = 0x10;
                            g_game7bea9c = vc;
                            Game_Helper56c931(a1, 0x10);
                            Sleep(0x32);
                        }
                        if (Game_Helper416d5a(a1, vc) != 0) {
                            Game_Helper4155ee(a1, a2, a3,
                                              Game_Helper441699(a1, 6, 1) - Game_Helper441699(a1, 7, 1));
                            Game_Helper415794(a1, a2, a3,
                                              Game_Helper441699(a1, 6, 1) - Game_Helper441699(a1, 7, 1),
                                              &g_game8c8274, g_game8e26bc);
                            Game_Helper41598e(a1, a2, a3,
                                              Game_Helper441699(a1, 6, 1) - Game_Helper441699(a1, 7, 1),
                                              &g_game8c8274, g_game8e26bc);
                        }
                    }
                }
            }
        }
        if (Game_Helper415b89(g_game8e2690, g_game8c8274, g_game8e26bc) == 0
            && (g_game8e2690[0] != 0 || g_game8e2690[6] != 0)) {
            for (vc = 0; vc < g_game78e970[a1]; vc++) {
                if (Game_Helper416b1c(a1, vc, a4) != 0
                    && Game_Helper40e3dc(a1, vc, 0x73, 1 - a1, -1) != 0) {
                    v4 = 0;
                    if (g_game8e2690[0] < 1) {
                        if (g_game8e2690[0] == -1
                            && g_game8c8274 < g_game8e26bc
                            && g_game8e26bc != -1) {
                            v4 = 1;
                        } else {
                            if (g_game8e2690[6] < 1) {
                                if (g_game8e2690[6] == -1
                                    && g_game8c8274 < g_game8e26bc
                                    && g_game8e26bc != -1) {
                                    v4 = 1;
                                }
                            } else {
                                v4 = 1;
                            }
                        }
                    } else {
                        v4 = 1;
                    }
                    if (g_gameSlots[a1][vc].field58 == '@' && g_game8e2690[6] == 0) {
                        v4 = 0;
                    }
                    if (v4 != 0) {
                        if ((g_game93a934 & 0x2) != 0) {
                            g_game7bea98 = 0x10;
                            g_game7bea9c = vc;
                            Game_Helper56c931(a1, 0x10);
                            Sleep(0x32);
                        }
                        if (Game_Helper416d5a(a1, vc) != 0) {
                            Game_Helper4155ee(a1, a2, a3,
                                              Game_Helper441699(a1, 6, 1) - Game_Helper441699(a1, 7, 1));
                            Game_Helper415794(a1, a2, a3,
                                              Game_Helper441699(a1, 6, 1) - Game_Helper441699(a1, 7, 1),
                                              &g_game8c8274, g_game8e26bc);
                            Game_Helper41598e(a1, a2, a3,
                                              Game_Helper441699(a1, 6, 1) - Game_Helper441699(a1, 7, 1),
                                              &g_game8c8274, g_game8e26bc);
                        }
                    }
                }
            }
        }
        if (Game_Helper415b89(g_game8e2690, g_game8c8274, g_game8e26bc) == 0
            && v8 != 0
            && g_game8e26bc == -1) {
            for (vc = 0; vc < g_game78e970[a1]; vc++) {
                if (Game_Helper416b1c(a1, vc, a4) != 0
                    && Game_Helper40e3dc(a1, vc, 0x73, 1 - a1, -1) != 0) {
                    v8 = 0;
                    for (v14 = 1; v14 < 7; v14++) {
                        if (g_game8e26bc == -1
                            && g_game8e2690[v14] == -1
                            && (1 << (v14 & 0x1f) & (signed char)g_gameSlots[a1][vc].field58) != 0) {
                            v8 = 1;
                        } else {
                            if (g_game8e26bc == -1 && g_game8e2690[0] == -1) {
                                v8 = 1;
                            } else {
                                if (g_game8e26bc == -1 && g_game8e2690[6] == -1) {
                                    v8 = 1;
                                }
                            }
                        }
                    }
                    if (g_gameSlots[a1][vc].field58 == '@' && g_game8e2690[6] != -1) {
                        v8 = 0;
                    }
                    if (a5 != 0 && v8 == 0) {
                        v18 = 0;
                        while (v18 < 10 && g_game74cdc0[a1][v18] != -1) {
                            if (g_game8e26bc == -1
                                && g_game8e2690[(unsigned int)g_game74cdc0[a1][v18] >> 0x10] == -1) {
                                v28 = (unsigned char)g_game74cdc0[a1][v18];
                                if ((1 << (v28 & 0x1f) & (signed char)g_gameSlots[a1][vc].field58) != 0) {
                                    v8 = 1;
                                }
                            }
                            v18++;
                        }
                    }
                    if (v8 != 0) {
                        if ((g_game93a934 & 0x2) != 0) {
                            g_game7bea98 = 0x10;
                            g_game7bea9c = vc;
                            Game_Helper56c931(a1, 0x10);
                            Sleep(0x32);
                        }
                        if (Game_Helper416d5a(a1, vc) != 0) {
                            Game_Helper4155ee(a1, a2, a3,
                                              Game_Helper441699(a1, 6, 1) - Game_Helper441699(a1, 7, 1));
                            Game_Helper415794(a1, a2, a3,
                                              Game_Helper441699(a1, 6, 1) - Game_Helper441699(a1, 7, 1),
                                              &g_game8c8274, g_game8e26bc);
                            Game_Helper41598e(a1, a2, a3,
                                              Game_Helper441699(a1, 6, 1) - Game_Helper441699(a1, 7, 1),
                                              &g_game8c8274, g_game8e26bc);
                        }
                    }
                }
            }
        }
        if ((g_game93a934 & 0x2) != 0) {
            g_game7bea98 = 0x10;
            g_game7bea9c = -1;
            Game_Helper56c931(a1, 0x10);
        }
        Game_Helper451e70();
    }
    return 1;
}

/* A card script: kind 0x71 replays the opponent's last choice, 0x73 is the
 * castability test, 0x6d pays 3 mana of colour 2 and picks a target, 0x72
 * resolves it and 0x90 clears the pending flag. */
// FUNCTION: SHANDALAR 0x00419599
int Game_Helper419599(int a1, int a2, int a3)
{
    int v14;
    int v10;
    int vc;
    int v8;

    if (a3 == 0x71) {
        v8 = g_game7a01e0;
        Game_Helper419e85(a1, a2, Game_Helper419d01(1 - a1));
        g_game7a01e0 = v8;
    }
    if (a3 == 0x73) {
        if (((g_gameSlots[a1][a2].flags08 & 0x30000) == 0
             || (g_gameCards[g_gameSlots[a1][a2].field6c].flags & 0x2) == 0)
            && (g_gameSlots[a1][a2].flags08 & 0x10) == 0
            && Game_Helper441699(a1, 3, 2) != 0) {
            if (Game_Helper4be650(0, 0, a1, 2, 2, 0x200, 2, 0, 0,
                                  Game_Helper488cc4(a1, a2), 0, 0, -1,
                                  g_gameSlots[(signed char)g_gameSlots[a1][a2].field0c][g_gameSlots[a1][a2].field48].field38,
                                  -1, -1, 0x10, 0, 0) != 0) {
                return 1;
            }
        }
    } else if (a3 == 0x90) {
        Game_Helper4c4263(0);
    } else {
        if (a3 == 0x6d
            && (g_gameSlots[a1][a2].flags08 & 0x20010) == 0
            && Game_Helper419310(a1, a2,
                                 g_gameSlots[(signed char)g_gameSlots[a1][a2].field0c][g_gameSlots[a1][a2].field48].field38) != 0) {
            Game_Helper4143e0(a1, 3, 2);
            if (g_game8c83ec != 1) {
                Game_Helper56ca10(0x5810c8, 0x5810bc);
                if (Game_Helper4c0efa(a1, 2, 1 - a1, 0x200, 2, 0, 0,
                                      Game_Helper488cc4(a1, a2), 0, 0, -1,
                                      g_gameSlots[(signed char)g_gameSlots[a1][a2].field0c][g_gameSlots[a1][a2].field48].field38,
                                      -1, -1, 0x10, 0, 0, 0x8aa920, 1, (int)&v14) == 0) {
                    g_game8c83ec = 1;
                } else {
                    g_gameSlots[a1][a2].field78 = v10;
                    g_gameSlots[a1][a2].field74 = v14;
                    g_gameSlots[a1][a2].field36 = 1;
                    vc = g_game8e44b8[g_gameCards[g_gameSlots[v14][v10].field6c].mask][0];
                    g_gameSlots[a1][a2].flags08 |= 0x10;
                }
            }
        }
        if (a3 == 0x72) {
            if (Game_Helper4bea85(g_gameSlots[a1][a2].field74, g_gameSlots[a1][a2].field78,
                                  0, a1, 2, 2, 0x200, 2, 0, 0,
                                  Game_Helper488cc4(a1, a2), 0, 0, -1,
                                  g_gameSlots[(signed char)g_gameSlots[a1][a2].field0c][g_gameSlots[a1][a2].field48].field38,
                                  -1, -1, 0x10, 0, 0) == 0) {
                g_game8c83ec = 1;
            } else {
                if (g_game8bd200 != 1) {
                    Game_Helper40ebb9(0x2e);
                }
                Game_Helper46d8c1(g_gameSlots[a1][a2].field74, g_gameSlots[a1][a2].field78, 1);
            }
            g_gameSlots[g_gameSlots[a1][a2].field10c][g_gameSlots[a1][a2].field110].field36 = 0;
        }
    }
    return 0;
}

/* Applies random effect a3 (0..0x13, anything else is the error case) to the
 * slot the caller stashed in field74/field78 of [a1][a2]. Most arms allocate
 * an auxiliary slot through 0x004a9910 and stamp it; effect 0xd asks the
 * player a yes/no question and effect 0x13 drains a counter. Every arm that
 * allocated a slot tags it at the end with the effect's id from
 * g_game580fd8. */
// FUNCTION: SHANDALAR 0x0041ab5e
int Game_Helper41ab5e(int a1, int a2, int a3)
{
    int iv;
    int ig;
    char v138[300];
    int vc;
    int v8;

    if (Game_Helper4bea85(g_gameSlots[a1][a2].field74, g_gameSlots[a1][a2].field78,
                          0, a1, 2, 2, 0x200, 2, 0, 0,
                          Game_Helper488cc4(a1, a2), 0, 0, -1, -1, -1, -1, 0, 0, 0) == 0) {
        g_game8c83ec = 1;
    } else {
        ig = g_gameSlots[a1][a2].field74;
        iv = g_gameSlots[a1][a2].field78;
        vc = -1;
        if (-1 < a3 && a3 < 0x14 && a3 != 0xd && a3 != 1) {
            Game_Helper56ca10(0x5810ec, 0x5810d4);
            strcpy(v138, (char *)0x5810f8);
            strcat(v138, (char *)(0x8aa920 + a3 * 300));
            Game_Helper558538(a1, a1, a2, ig, iv, (int)v138, 0);
            if (g_game8bd200 != 1) {
                Game_Helper40ebb9(0x30);
            }
        }
        switch (a3) {
        case 0:
            vc = Game_Helper4a9910(g_game94f280, g_game930f5c, g_game8c8b44, ig, iv);
            if (vc != -1) {
                g_gameSlots[a1][vc].field38 = 0x80;
                g_gameSlots[a1][vc].field12 = g_gameSlots[ig][iv].field34;
                g_gameSlots[a1][vc].flags18 |= 0x4000;
            }
            break;
        case 1:
            if (g_gameSlots[ig][iv].field34 < 3) {
                Game_Helper56ca10(0x581144, 0x58112c);
                sprintf(v138, (char *)0x581150, (char *)0x8aa920);
                Game_Helper558538(a1, a1, a2, ig, iv, (int)v138, 0);
                vc = Game_Helper4a9910(g_game94f280, g_game930f5c, g_game7bfe7c, ig, iv);
                if (g_game8bd200 != 1) {
                    Game_Helper40ebb9(0x30);
                }
            } else {
                g_game8c83ec = 1;
                Game_Helper56ca10(0x58116c, 0x581154);
                sprintf(v138, (char *)0x581178, (char *)0x8aaa4c);
                Game_Helper558538(a1, a1, a2, ig, iv, (int)v138, 0);
            }
            break;
        case 2:
            vc = Game_Helper4a9910(g_game94f280, g_game930f5c, g_game8bcff8, ig, iv);
            if (vc != -1) {
                g_gameSlots[a1][vc].field12 = 4;
                g_gameSlots[a1][vc].field1c = -Game_Helper4ecec6(4, 0, g_gameSlots[ig][iv].field0e - 1);
            }
            break;
        case 3:
            g_gameSlots[ig][iv].field1e = 1 << (Game_Helper4ba392(a1, a2, 3) & 0x1f);
            break;
        case 4:
            g_gameSlots[ig][iv].field1e = 1 << (Game_Helper4ba392(a1, a2, 5) & 0x1f);
            break;
        case 5:
            g_gameSlots[ig][iv].field1e = 1 << (Game_Helper4ba392(a1, a2, 4) & 0x1f);
            break;
        case 6:
            Game_Helper4ba5a1(ig, iv, 3, g_game94f280, g_game930f5c);
            break;
        case 7:
            vc = Game_Helper4a9910(g_game94f280, g_game930f5c, g_game8aa734, ig, iv);
            if (vc != -1) {
                g_gameSlots[a1][vc].field28 = 0;
                g_gameSlots[a1][vc].field38 = 0x20;
            }
            break;
        case 8:
            vc = Game_Helper4a9910(g_game94f280, g_game930f5c, g_game8bcff8, ig, iv);
            if (vc != -1) {
                g_gameSlots[a1][vc].field12 = 3;
                g_gameSlots[a1][vc].field1c = 3;
            }
            break;
        case 9:
            vc = Game_Helper4a9910(g_game94f280, g_game930f5c, g_game8aa734, ig, iv);
            if (vc != -1) {
                g_gameSlots[a1][vc].field38 = 0x40;
            }
            g_gameSlots[ig][iv].field28 = 0x8000000;
            break;
        case 10:
            g_gameSlots[ig][iv].field1e = 1 << (Game_Helper4ba392(a1, a2, 1) & 0x1f);
            break;
        case 0xb:
            g_gameSlots[ig][iv].field1e = 1 << (Game_Helper4ba392(a1, a2, 2) & 0x1f);
            break;
        case 0xc:
            vc = Game_Helper4a9910(g_game94f280, g_game930f5c, g_game8c8b44, ig, iv);
            if (vc != -1) {
                g_gameSlots[a1][vc].flags18 |= 0x800000;
            }
            g_gameSlots[ig][iv].field28 = 0x8000000;
            break;
        case 0xd:
            Game_Helper56ca10(0x581114, 0x5810fc);
            sprintf(v138, (char *)0x581120, (char *)0x8aa920, (char *)0x8aaa4c,
                    (char *)0x8aab78);
            if (Game_Helper558538(a1, a1, a2, ig, iv, (int)v138,
                                  (g_gameSlots[a1][a2].flags08 & 0x10) >> 4) == 0) {
                Game_Helper4b183c(ig, iv);
            } else {
                g_gameSlots[ig][iv].flags08 &= ~0x10;
            }
            if (g_game8bd200 != 1) {
                Game_Helper40ebb9(0x30);
            }
            break;
        case 0xe:
            vc = Game_Helper4a9910(g_game94f280, g_game930f5c, g_game8bcff8, ig, iv);
            if (vc != -1) {
                g_gameSlots[a1][vc].field12 = -2;
                g_gameSlots[a1][vc].field1c = 0;
            }
            break;
        case 0xf:
            Game_Helper4ba41b(ig, iv);
            break;
        case 0x10:
            Game_Helper4ba5a1(ig, iv, 1, g_game94f280, g_game930f5c);
            break;
        case 0x11:
            for (v8 = 0; v8 < 2; v8++) {
                for (vc = 0; vc < g_game78e970[v8]; vc++) {
                    if (g_gameSlots[v8][vc].field6c == g_game8e3f20
                        && Game_Helper411d90(v8, vc) != 0
                        && (signed char)g_gameSlots[v8][vc].field50 == ig
                        && g_gameSlots[v8][vc].field04 == iv) {
                        g_gameSlots[v8][vc].flags18 &= ~0x1000000;
                    }
                }
            }
            vc = Game_Helper4a9910(g_game94f280, g_game930f5c, g_game8e3f20, ig, iv);
            if (vc != -1) {
                g_gameSlots[a1][vc].flags18 |= 0x1000000;
                g_gameSlots[a1][vc].field12 =
                    -(*(unsigned short *)&g_gameCards[g_gameSlots[ig][iv].field6c].unknown02d[1] & 0xbfff);
                g_gameSlots[a1][vc].field1c =
                    2 - (*(unsigned short *)&g_gameCards[g_gameSlots[ig][iv].field6c].unknown02d[3] & 0xbfff);
            }
            break;
        case 0x12:
            Game_Helper4883aa(ig, Game_Helper40d7b2(ig, iv, 0x32, -1),
                              g_game94f280, g_game930f5c);
            Game_Helper46d8c1(ig, iv, 4);
            break;
        case 0x13:
            if (g_game8bd200 != 1) {
                Game_Helper40ebb9(0x38);
                Sleep(0xdac);
            }
            g_gameSlots[ig][iv].field1c = g_gameSlots[ig][iv].field1c - 1;
            g_gameSlots[ig][iv].field00 = g_gameSlots[ig][iv].field00 + 0x1000000;
            if (g_game8bd200 != 1) {
                Game_Helper40ebb9(0x37);
            }
            break;
        default:
            Game_Helper56ca10(0x581190, 0x58117c);
            sprintf(v138, (char *)0x58119c, (char *)0x8aa920);
            Game_Helper558538(a1, a1, a2, ig, iv, (int)v138, 0);
        }
        if (vc != -1) {
            g_gameSlots[a1][vc].field64 =
                Game_CombineMask(g_game580fd8[a3], a1, a2) << 0x10 | g_game580fd8[a3];
        }
    }
    g_gameSlots[g_gameSlots[a1][a2].field10c][g_gameSlots[a1][a2].field110].field36 = 0;
    return 0;
}

/* The second random-effect dispatcher: sixteen arms, each announcing itself
 * through 0x00558538 before acting on the slot in field74/field78 of
 * [a1][a2]. Ends with the 0x00558aea repaint. */
// FUNCTION: SHANDALAR 0x0041c208
int Game_Helper41c208(int a1, int a2, int a3)
{
    int iv;
    int ig;
    char v144[300];
    int v18;
    int v14;
    int v10;
    int vc;
    int v8;

    ig = g_gameSlots[a1][a2].field74;
    iv = g_gameSlots[a1][a2].field78;
    Game_Helper56ca10(0x5811d0, 0x5811c0);
    switch (a3) {
    case 0:
        sprintf(v144, (char *)0x5811dc, (char *)0x8aa920);
        Game_Helper558538(a1, a1, a2, ig, iv, (int)v144, 0);
        Game_Helper4ba41b(ig, iv);
        break;
    case 1:
        if ((g_game93a934 & 0x2) == 0) {
            vc = Game_Helper522508(2);
        } else {
            vc = Game_Helper5225a9(a1);
        }
        if (vc == 0) {
            sprintf(v144, (char *)0x5811e4, (char *)0x8aab78);
        } else {
            sprintf(v144, (char *)0x5811e0, (char *)0x8aaa4c);
        }
        Game_Helper558538(a1, a1, a2, ig, iv, (int)v144, 0);
        if (vc == 0) {
            if ((g_gameSlots[ig][iv].flags08 & 0x10) == 0) {
                g_gameSlots[ig][iv].flags08 |= 0x10;
                if ((g_gameCards[g_gameSlots[ig][iv].field6c].flags & 0x1) != 0) {
                    g_game93a794 = -1;
                }
                Game_Helper40e158(ig, iv, 0x81);
            }
        } else {
            g_gameSlots[ig][iv].flags08 &= ~0x10;
        }
        break;
    case 2:
        sprintf(v144, (char *)0x5811e8, (char *)0x8aaca4);
        Game_Helper558538(a1, a1, a2, ig, iv, (int)v144, 0);
        Game_Helper49914d(a1, a2, 0x71, 4);
        break;
    case 3:
        sprintf(v144, (char *)0x5811ec, (char *)0x8aadd0);
        Game_Helper558538(a1, a1, a2, ig, iv, (int)v144, 0);
        Game_Helper409a0b(ig);
        Game_Helper409a0b(ig);
        Game_Helper409a0b(ig);
        break;
    case 4:
        sprintf(v144, (char *)0x581240, (char *)0x8ab85c);
        Game_Helper558538(a1, a1, a2, -1, -1, (int)v144, 0);
        g_game94f280 = a1;
        Game_Helper5218f3(a1, a2);
        g_game94f280 = -1;
        break;
    case 5:
        sprintf(v144, (char *)0x5811f0, (char *)0x8aaefc);
        Game_Helper558538(a1, a1, a2, ig, iv, (int)v144, 0);
        Game_Helper4883aa(ig,
                          (signed char)g_gameCards[g_gameSlots[ig][iv].field6c].field2b
                              + Game_Helper4ecec6((signed char)g_gameCards[g_gameSlots[ig][iv].field6c].field2c, 0, 99),
                          g_game94f280, g_game930f5c);
        Game_Helper46d8c1(ig, iv, 2);
        break;
    case 6:
        sprintf(v144, (char *)0x58120c, (char *)0x8ab730);
        Game_Helper558538(a1, a1, a2, -1, -1, (int)v144, 0);
        Game_Helper56ca10(0x581228, 0x581210);
        sprintf(v144, (char *)0x581234, (char *)0x8aa920, (char *)0x8aaa4c,
                (char *)0x8aab78);
        vc = Game_Helper558538(a1, a1, a2, -1, -1, (int)v144, 1);
        ig = Game_Helper53f510(a1, g_game8f3224, 1);
        if (ig == vc) {
            v14 = Game_Helper56ae5d(a1, Game_Helper56c705(0x37a));
            if (v14 != -1) {
                Game_Helper4d1c03(a1, v14);
                g_gameSlots[a1][v14].flags18 |= 0x10;
            }
        } else {
            Game_Helper4bab38(a1, 5, a1, a2);
        }
        break;
    case 7:
        sprintf(v144, (char *)0x5811f4, (char *)0x8ab028);
        Game_Helper558538(a1, a1, a2, ig, iv, (int)v144, 0);
        Game_Helper46d8c1(ig, iv, 1);
        break;
    case 8:
        sprintf(v144, (char *)0x5811f8, (char *)0x8ab154);
        Game_Helper558538(a1, a1, a2, ig, iv, (int)v144, 0);
        Game_Helper4883aa(ig, 3, g_game94f280, g_game930f5c);
        break;
    case 9:
        sprintf(v144, (char *)0x5811fc, (char *)0x8ab280);
        Game_Helper558538(a1, a1, a2, ig, iv, (int)v144, 0);
        Game_Helper46d8c1(ig, iv, 1);
        break;
    case 10:
        sprintf(v144, (char *)0x581244, (char *)0x8ab988);
        Game_Helper558538(a1, a1, a2, ig, iv, (int)v144, 0);
        Game_Helper409ee8(ig, 0, 0);
        break;
    case 0xb:
        sprintf(v144, (char *)0x581200, (char *)0x8ab3ac);
        Game_Helper558538(a1, a1, a2, ig, iv, (int)v144, 0);
        for (v14 = 0; v14 < 2; v14++) {
            v18 = g_game8a9410[ig][0];
            if (v18 != -1) {
                Game_Helper56c496(ig, 0);
                v8 = Game_Helper56ae5d(ig, v18);
                if (v8 != -1) {
                    Game_Helper46e4d0(ig, v8);
                    g_gameSlots[ig][v8].field6c = -1;
                }
            }
            if (g_game8bd200 != 1) {
                Game_Helper40ebb9(0x18);
            }
        }
        break;
    case 0xc:
        sprintf(v144, (char *)0x581204, (char *)0x8ab4d8);
        Game_Helper558538(a1, a1, a2, -1, -1, (int)v144, 0);
        if ((g_game93a934 & 0x2) == 0) {
            v10 = Game_Helper522508(2);
        } else {
            v10 = Game_Helper5225a9(a1);
        }
        v14 = Game_Helper56ae5d(v10, Game_Helper56c705(0x375));
        if (v14 != -1) {
            Game_Helper4d1c03(v10, v14);
            g_gameSlots[v10][v14].flags18 |= 0x10;
        }
        break;
    case 0xd:
        sprintf(v144, (char *)0x581208, (char *)0x8ab604);
        Game_Helper558538(a1, a1, a2, -1, -1, (int)v144, 0);
        Game_Helper49f3c0((int)Game_Fn51dcf2, -1);
        break;
    case 0xe:
        sprintf(v144, (char *)0x581248, (char *)0x8abab4);
        Game_Helper558538(a1, a1, a2, -1, -1, (int)v144, 0);
        v14 = Game_Helper4a9910(a1, a2, g_game7a0230, -1, -1);
        if (v14 != -1) {
            g_gameSlots[a1][v14].field64 = g_game581068;
        }
        break;
    case 0xf:
        sprintf(v144, (char *)0x58124c, (char *)0x8abbe0);
        Game_Helper558538(a1, a1, a2, ig, iv, (int)v144, 0);
        v14 = Game_Helper409a0b(ig);
        Game_Helper56ca10(0x581260, 0x581250);
        Game_Helper558538(a1, a1, a2, ig, v14, 0x8aa920, 0);
        if ((g_gameCards[g_gameSlots[ig][v14].field6c].flags & 0x1) == 0) {
            Game_Helper46e4d0(ig, v14);
            g_gameSlots[ig][v14].field6c = -1;
            g_game8e3ec8[ig] = g_game8e3ec8[ig] - 1;
            if (g_game8bd200 != 1) {
                Game_Helper40ebb9(0x18);
            }
        }
        break;
    default:
        sprintf(v144, (char *)0x58126c, (char *)0x8abd0c);
        Game_Helper558538(a1, a1, a2, ig, iv, (int)v144, 0);
    }
    Game_Helper558aea(0, 0xff);
    return 0;
}

/* Rebuilds the per-slot AI evaluation table for both sides with the UI muted
 * (g_game8bd200 / g_game97f1b0 forced to 1 and restored on exit). For every
 * live slot it recognises a handful of card scripts by their +0x34 pointer and
 * pre-seeds their contribution, then queries the slot itself with kinds 0x32,
 * 0x33 and 0x34 and folds the answers into g_game97c860. The opponent's pass
 * also accumulates the g_game97db24 ability summary. */
// FUNCTION: SHANDALAR 0x0041d33b
void Game_Helper41d33b(int a1)
{
    int v1;
    int v2;
    int v3;
    int v4;
    int v5;
    int v6;
    int v7;
    int v9;
    int v14;

    v4 = g_game97f1b0;
    v3 = g_game8bd200;
    v5 = 1 - a1;
    g_game8bd200 = 1;
    g_game97f1b0 = 1;
    g_game97db20 = 0;
    memset(g_game97c860, 0, 0x12c0);
    for (v14 = 0; v14 < g_game78e970[a1]; v14++) {
        if (Game_Helper411d90(a1, v14) != 0) {
            v6 = g_gameSlots[a1][v14].field6c;
            v7 = (signed char)g_gameSlots[a1][v14].field50;
            v1 = g_gameSlots[a1][v14].field04;
            if (g_gameCards[v6].script == Game_CardScript4e8d27
                && Game_Helper441699(v7, 3, 1) != 0) {
                g_game97c860[v7][v1].flags08 |= 0x200;
                g_gameSlots[v7][v1].field28 |= 0x200;
            } else {
                if (g_gameCards[v6].script == Game_CardScript4e9ad4
                    && Game_Helper441699(v7, 4, 3) != 0) {
                    g_game97c860[v7][v1].flags08 |= 0x200;
                    g_gameSlots[v7][v1].field28 |= 0x200;
                }
            }
            if (g_gameCards[v6].script == Game_CardScript4dd9ef) {
                g_game97c860[v7][v1].field04 =
                    g_game97c860[v7][v1].field04 + Game_Helper441699(v7, 5, 1);
            } else if (g_gameCards[v6].script == Game_CardScript4df65e) {
                g_game97c860[v7][v1].field00 =
                    g_game97c860[v7][v1].field00 + Game_Helper441699(v7, 4, 1);
            } else if (g_gameCards[v6].script == Game_CardScript4de7f8) {
                g_game97c860[v7][v1].field00 =
                    g_game97c860[v7][v1].field00 + Game_Helper441699(v7, 5, 1);
                g_game97c860[v7][v1].field04 =
                    g_game97c860[v7][v1].field04 + Game_Helper441699(v7, 5, 1);
            } else if (g_gameCards[v6].script == Game_CardScript48b2c1) {
                g_game97c860[a1][v14].flags08 |= 0x1ff800;
            } else if (g_gameCards[v6].script == Game_CardScript494f82) {
                g_game97c860[a1][v14].flags08 |= 0x80000000;
            } else if (g_gameCards[v6].script == Game_CardScript4dcace) {
                g_game97c860[v7][v1].flags08 |= 0x1ff800;
                g_game97c860[v7][v1].field00 = 0xffffff9d;
            }
            g_gameSlots[a1][v14].field28 |= 0xe000000;
            v2 = g_gameSlots[a1][v14].flags08;
            if (g_game8c7468 == a1) {
                if ((g_gameSlots[a1][v14].flags08 & 0x2000) == 0) {
                    g_gameSlots[a1][v14].flags08 |= 0x14;
                } else {
                    g_gameSlots[a1][v14].flags08 |= 0x4;
                }
            }
            g_game97db28 = Game_Helper40d7b2(a1, v14, 0x32, -1);
            g_game97db38 = Game_Helper40d7b2(a1, v14, 0x33, -1);
            g_game97db34 = Game_Helper40d7b2(a1, v14, 0x34, -1);
            v9 = Game_Helper40dffd((signed char)g_gameCards[v6].unknown02a[0]);
            if ((g_game97db34 & 0x200) != 0
                && Game_Helper441699(a1, v9, 1) == 0) {
                g_game97db34 = g_game97db34 & ~0x200;
            }
            if (g_game8c776c == a1) {
                Game_Helper40e158(a1, v14, 0x8c);
            }
            g_game97c860[a1][v14].field00 = g_game97c860[a1][v14].field00 + g_game97db28;
            g_game97c860[a1][v14].field04 = g_game97c860[a1][v14].field04 + g_game97db38;
            g_game97c860[a1][v14].flags08 |= g_game97db34;
            g_gameSlots[a1][v14].flags08 = v2;
        }
    }
    g_game97db24 = 0;
    for (v14 = 0; v14 < g_game78e970[v5]; v14++) {
        if (Game_Helper411d90(v5, v14) != 0) {
            v6 = g_gameSlots[v5][v14].field6c;
            v7 = (signed char)g_gameSlots[v5][v14].field50;
            v1 = g_gameSlots[v5][v14].field04;
            if (g_gameCards[v6].script == Game_CardScript4e8d27
                && Game_Helper441699(v7, 3, 1) != 0) {
                g_game97c860[v7][v1].flags08 |= 0x200;
                g_gameSlots[v7][v1].field28 |= 0x200;
            } else {
                if (g_gameCards[v6].script == Game_CardScript4e9ad4
                    && Game_Helper441699(v7, 4, 3) != 0) {
                    g_game97c860[v7][v1].flags08 |= 0x200;
                    g_gameSlots[v7][v1].field28 |= 0x200;
                }
            }
            if (g_gameCards[v6].script == Game_CardScript4dd9ef) {
                g_game97c860[v7][v1].field04 =
                    g_game97c860[v7][v1].field04 + Game_Helper441699(v7, 5, 1);
            } else if (g_gameCards[v6].script == Game_CardScript4df65e) {
                g_game97c860[v7][v1].field00 =
                    g_game97c860[v7][v1].field00 + Game_Helper441699(v7, 4, 1);
            } else if (g_gameCards[v6].script == Game_CardScript4de7f8) {
                g_game97c860[v7][v1].field00 =
                    g_game97c860[v7][v1].field00 + Game_Helper441699(v7, 5, 1);
                g_game97c860[v7][v1].field04 =
                    g_game97c860[v7][v1].field04 + Game_Helper441699(v7, 5, 1);
            } else if (g_gameCards[v6].script == Game_CardScript494312
                       && (g_gameSlots[v5][v14].flags08 & 0x30000) == 0) {
                g_game97db20 = g_game97db20 + 1;
            } else if (g_gameCards[v6].script == Game_CardScript48b2c1) {
                g_game97c860[v5][v14].flags08 |= 0x1ff800;
            } else if (g_gameCards[v6].script == Game_CardScript494f82) {
                g_game97c860[v5][v14].flags08 |= 0x80000000;
            } else if (g_gameCards[v6].script == Game_CardScript4dcace) {
                g_game97c860[v7][v1].flags08 |= 0x1ff800;
                g_game97c860[v7][v1].field00 = 0xffffff9d;
            }
            g_gameSlots[v5][v14].field28 |= 0xe000000;
            v2 = g_gameSlots[v5][v14].flags08;
            g_gameSlots[v5][v14].flags08 |= 0x8;
            g_game97db28 = Game_Helper40d7b2(v5, v14, 0x32, -1);
            g_game97db38 = Game_Helper40d7b2(v5, v14, 0x33, -1);
            g_game97db34 = Game_Helper40d7b2(v5, v14, 0x34, -1);
            v9 = Game_Helper40dffd((signed char)g_gameCards[v6].unknown02a[0]);
            if ((g_game97db34 & 0x200) != 0
                && Game_Helper441699(v5, v9, 1) == 0) {
                g_game97db34 = g_game97db34 & ~0x200;
            }
            if (v5 == g_game8c776c) {
                Game_Helper40e158(v5, v14, 0x8c);
            }
            g_game97c860[v5][v14].field00 = g_game97c860[v5][v14].field00 + g_game97db28;
            g_game97c860[v5][v14].field04 = g_game97c860[v5][v14].field04 + g_game97db38;
            g_game97c860[v5][v14].flags08 |= g_game97db34;
            g_gameSlots[v5][v14].flags08 = v2;
            v6 = g_gameSlots[v5][v14].field6c;
            if (g_gameCards[v6].script == Game_CardScript4ea2d4) {
                g_game97db24 = g_game97db24 | 0x2;
            }
            if (g_gameCards[v6].script == Game_CardScript4ea379) {
                g_game97db24 = g_game97db24 | 0x4;
            }
            if (g_gameCards[v6].script == Game_CardScript4ea3b0) {
                g_game97db24 = g_game97db24 | 0x8;
            }
            if (g_gameCards[v6].script == Game_CardScript4ea342) {
                g_game97db24 = g_game97db24 | 0x10;
            }
            if (g_gameCards[v6].script == Game_CardScript4ea30b) {
                g_game97db24 = g_game97db24 | 0x20;
            }
        }
    }
    if (g_game97db24 == 0) {
        g_game97db2c = 0;
    } else {
        g_game97db2c = Game_Helper441699(v5, 7, 0);
    }
    Game_Helper40d614();
    g_game8bd200 = v3;
    g_game97f1b0 = v4;
    return;
}

/* Fills the 0x005a76xx combat work area from the evaluation table: one pass
 * collecting a1's attackers (with any slot that field24 bands to another
 * folded into that band's entry), one pass collecting the defender's
 * blockers, and a final cross pass recording, per attacker, which blockers
 * 0x0040d146 says can meet it. Each entry also carries a re-evaluated cost,
 * halved or trebled depending on the 0x200 ability bit. */
// FUNCTION: SHANDALAR 0x0041e0d9
void Game_Helper41e0d9(int a1)
{
    int vc8[16];
    unsigned int v88;
    int v84;
    int v80;
    int v7c;
    int v78;
    int v74;
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
    /* Ghidra's uVar5 and its sort/swap register temp: both live across
       statements, so /Od gives each a slot. */
    int v4;
    int t0;

    g_game5a7a78 = 1 - a1;
    if (g_game5a7ba0 == 0) {
        Game_Helper4c5bf6((int)&v8, (int)&v14);
        if (g_game5a7a78 == 1) {
            g_game5a811c = v8;
        } else {
            g_game5a811c = v14;
        }
        g_game5a79f4 = 0;
        g_game5a7818 = 0;
        g_game5a8118 = 0;
        g_game5a7b1c = 0;
        v68 = g_game8bd200;
        g_game8bd200 = 1;
        g_game97f1b0 = 1;
        Game_Fn4c3b83();
        v60 = g_game94d63c;
        g_game94d63c = 0;
        Game_Helper40e1f5(199);
        Game_Helper46ca26(a1);
        v84 = Game_Helper4c4457(a1);
        v84 = g_game94d63c + v84;
        for (v6c = 0; v6c < g_game78e970[a1]; v6c++) {
            g_gameSlots[a1][v6c].flags08 &= ~0x4;
        }
        Game_Helper4257a4(a1);
        Game_Fn4c3e34();
        g_game94d63c = v60;
        for (v6c = 0; v6c < g_game78e970[a1]; v6c++) {
            v64 = g_gameSlots[a1][v6c].field6c;
            if (v64 != -1
                && (g_gameSlots[a1][v6c].flags08 & 0x4) != 0
                && (g_gameSlots[a1][v6c].field24 == -1
                    || g_gameSlots[a1][v6c].field24 == v6c)) {
                v7c = Game_Helper40dffd((signed char)g_gameCards[v64].unknown02a[0]);
                v74 = g_game97c860[a1][v6c].field00;
                g_game5a76d0[g_game5a7b1c] = v6c;
                g_game5a8010[g_game5a7b1c] = g_game7beb10[a1] + v74;
                g_game5a7820[g_game5a7b1c] = g_game97c860[a1][v6c].field04 + g_game7beb18[a1];
                g_game5a7610[g_game5a7b1c] = g_game97c860[a1][v6c].flags08;
                if (Game_Helper441699(a1, v7c, 1) == 0) {
                    g_game5a7610[g_game5a7b1c] = g_game5a7610[g_game5a7b1c] & ~0x200;
                }
                g_game97db30 = 0;
                Game_Helper40e158(a1, v6c, 0x8a);
                g_game5a7b20[g_game5a7b1c] = g_game97db30;
                if ((g_gameCards[v64].flags3c & 0x8) != 0) {
                    g_game5a8010[g_game5a7b1c] = g_game5a8010[g_game5a7b1c]
                        + g_gameCards[v64].script(a1, v6c, 0x39);
                }
                if ((g_gameCards[v64].flags3c & 0x10) != 0) {
                    g_game5a7820[g_game5a7b1c] = g_game5a7820[g_game5a7b1c]
                        + g_gameCards[v64].script(a1, v6c, 0x3a);
                }
                if (g_game97c860[a1][v6c].field0c == 0) {
                    g_game97c860[a1][v6c].field0c = Game_Helper41d290(a1, v6c);
                }
                vc = g_game97c860[a1][v6c].field0c;
                v10 = (abs((signed char)g_gameCards[v64].field2c)
                       + (signed char)g_gameCards[v64].field2b - vc) + v84;
                if ((g_game5a7610[g_game5a7b1c] & 0x200) != 0) {
                    if (Game_Helper441699(a1, v7c, 1) == 0) {
                        v10 = v10 << 1;
                    } else {
                        v10 = v10 / 3;
                    }
                }
                *(int *)g_gameSlots[a1][v6c].unknown060 = v10;   /* +0x60, no named member */
                g_game5a8050[g_game5a7b1c] = v10;
                g_game5a7820[g_game5a7b1c] =
                    g_game5a7820[g_game5a7b1c] - g_gameSlots[a1][v6c].field10;
                g_game5a78e8[g_game5a7b1c] = 0;
                if (g_gameSlots[a1][v6c].field1f != 0) {
                    g_game5a7818 = g_game5a7818 | 1 << (g_game5a7b1c & 0x1f);
                }
                g_game5a7b1c = g_game5a7b1c + 1;
                if (g_game8bd200 == 1 && 6 < g_game5a7b1c) {
                    break;
                }
            }
        }
        for (v6c = 0; v6c < g_game78e970[a1]; v6c++) {
            v64 = g_gameSlots[a1][v6c].field6c;
            if (v64 != -1
                && (g_gameSlots[a1][v6c].flags08 & 0x4) != 0
                && g_gameSlots[a1][v6c].field24 != -1
                && g_gameSlots[a1][v6c].field24 != v6c) {
                v58 = -1;
                for (v70 = 0; v70 < g_game5a7b1c; v70++) {
                    if (g_gameSlots[a1][v6c].field24 == g_game5a76d0[v70]) {
                        v58 = v70;
                        break;
                    }
                }
                if (v58 != -1) {
                    v7c = Game_Helper40dffd((signed char)g_gameCards[v64].unknown02a[0]);
                    g_game5a7b60[g_game5a8118] = v6c;
                    v74 = g_game97c860[a1][v6c].field00;
                    g_game5a8010[v58] = g_game5a8010[v58] + v74;
                    g_game5a7820[v58] = g_game5a7820[v58] + g_game97c860[a1][v6c].field04;
                    v88 = g_game97c860[a1][v6c].flags08 & 0x200 | g_game5a7610[v58] & 0x200;
                    g_game5a7610[v58] = g_game5a7610[v58] & g_game97c860[a1][v6c].flags08;
                    g_game5a7610[v58] = g_game5a7610[v58] | v88;
                    if ((g_gameCards[v64].flags3c & 0x8) != 0) {
                        g_game5a8010[v58] = g_game5a8010[v58]
                            + g_gameCards[v64].script(a1, v6c, 0x39);
                    }
                    if ((g_gameCards[v64].flags3c & 0x10) != 0) {
                        g_game5a7820[v58] = g_game5a7820[v58]
                            + g_gameCards[v64].script(a1, v6c, 0x3a);
                    }
                    if (g_game97c860[a1][v6c].field0c == 0) {
                        g_game97c860[a1][v6c].field0c = Game_Helper41d290(a1, v6c);
                    }
                    vc = g_game97c860[a1][v6c].field0c;
                    v10 = (abs((signed char)g_gameCards[v64].field2c)
                           + (signed char)g_gameCards[v64].field2b - vc) + v84;
                    if ((g_game5a7610[v58] & 0x200) != 0) {
                        if (Game_Helper441699(a1, v7c, 1) == 0) {
                            v10 = v10 << 1;
                        } else {
                            v10 = v10 / 3;
                        }
                    }
                    *(int *)g_gameSlots[a1][v6c].unknown060 = v10;   /* +0x60, no named member */
                    if (v10 < g_game5a8050[v58]) {
                        g_game5a8050[v58] = v10;
                    }
                    g_game5a7820[v58] = g_game5a7820[v58] - g_gameSlots[a1][v6c].field10;
                    g_game5a78e8[v58] = 0;
                    if (g_gameSlots[a1][v6c].field1f != 0) {
                        g_game5a7818 = g_game5a7818 | 1 << (v58 & 0x1f);
                    }
                    g_game5a8118 = g_game5a8118 + 1;
                }
            }
        }
        v5c = 0;
        for (v6c = 0; v6c < g_game78e970[g_game5a7a78]; v6c++) {
            v64 = g_gameSlots[g_game5a7a78][v6c].field6c;
            if (v64 != -1
                && ((g_gameCards[v64].flags & 0x2) != 0
                    || (g_gameSlots[g_game5a7a78][v6c].flags08 & 0x2000000) != 0)
                && (g_gameSlots[g_game5a7a78][v6c].flags08 & 0x800012) == 2
                && g_gameSlots[g_game5a7a78][v6c].field24 == -1) {
                v54[v5c] = v6c;
                v5c = v5c + 1;
            }
            if (g_game8bd200 == 1 && 0xf < v5c) {
                break;
            }
        }
        if (g_game8bd200 == 1 && 6 < v5c) {
            for (v6c = 0; v6c < v5c; v6c++) {
                vc8[v6c] = Game_Helper425c7e(g_game5a7a78, v54[v6c]);
            }
            for (v6c = 0; v6c < v5c; v6c++) {
                for (v70 = v6c; v70 < v5c; v70++) {
                    if (vc8[v6c] < vc8[v70]) {
                        t0 = vc8[v6c];
                        vc8[v6c] = vc8[v70];
                        vc8[v70] = t0;
                        t0 = v54[v6c];
                        v54[v6c] = v54[v70];
                        v54[v70] = t0;
                    }
                }
            }
            if (6 < v5c) {
                v5c = 7;
            }
            for (v6c = 0; v6c < v5c; v6c++) {
                for (v70 = v6c; v70 < v5c; v70++) {
                    if (v54[v70] < v54[v6c]) {
                        t0 = v54[v6c];
                        v54[v6c] = v54[v70];
                        v54[v70] = t0;
                    }
                }
            }
        }
        g_game5a7ba4 = 0;
        for (v80 = 0; v80 < v5c; v80++) {
            v6c = v54[v80];
            v64 = g_gameSlots[g_game5a7a78][v6c].field6c;
            v7c = Game_Helper40dffd((signed char)g_gameCards[v64].unknown02a[0]);
            g_gameSlots[g_game5a7a78][v6c].flags08 |= 0x8;
            v74 = g_game97c860[g_game5a7a78][v6c].field00;
            g_game5a80d8[g_game5a7ba4] = v6c;
            g_game5a8098[g_game5a7ba4] = g_game7beb10[g_game5a7a78] + v74;
            g_game5a7860[g_game5a7ba4] = g_game97c860[g_game5a7a78][v6c].field04
                + g_game7beb18[g_game5a7a78];
            g_game5a7650[g_game5a7ba4] = g_game97c860[g_game5a7a78][v6c].flags08;
            if (Game_Helper441699(g_game5a7a78, v7c, 1) == 0) {
                g_game5a7650[g_game5a7ba4] = g_game5a7650[g_game5a7ba4] & ~0x200;
            }
            g_game97db30 = 0;
            Game_Helper40e158(g_game5a7a78, v6c, 0x8b);
            g_game5a7ac0[g_game5a7b1c] = g_game97db30;
            if (g_game5a7a78 == g_game8c776c) {
                if ((g_gameCards[v64].flags3c & 0x8) != 0) {
                    g_game5a8098[g_game5a7ba4] = g_game5a8098[g_game5a7ba4]
                        + g_gameCards[v64].script(g_game5a7a78, v6c, 0x39);
                }
                if ((g_gameCards[v64].flags3c & 0x10) != 0) {
                    g_game5a7860[g_game5a7ba4] = g_game5a7860[g_game5a7ba4]
                        + g_gameCards[v64].script(g_game5a7a78, v6c, 0x3a);
                }
            }
            if (g_game97c860[g_game5a7a78][v6c].field0c == 0) {
                Game_Fn4c3b83();
                v60 = g_game94d63c;
                g_game94d63c = 0;
                g_gameSlots[g_game5a7a78][v6c].flags18 |= 0x8;
                Game_Helper46d8c1(g_game5a7a78, v6c, 2);
                Game_Helper40e1f5(199);
                Game_Helper46ca26(a1);
                vc = Game_Helper4c4457(a1);
                vc = g_game94d63c + vc;
                Game_Fn4c3e34();
                g_game97c860[g_game5a7a78][v6c].field0c = vc;
            }
            vc = g_game97c860[g_game5a7a78][v6c].field0c;
            v10 = vc - v84;
            if ((g_game5a7650[g_game5a7ba4] & 0x200) != 0) {
                if (Game_Helper441699(g_game5a7a78, v7c, 1) == 0) {
                    v10 = v10 << 1;
                } else {
                    v10 = v10 / 5;
                }
            }
            g_game5a8120[g_game5a7ba4] = v10;
            *(int *)g_gameSlots[g_game5a7a78][v6c].unknown060 = v10;   /* +0x60 */
            g_game5a7860[g_game5a7ba4] = g_game5a7860[g_game5a7ba4]
                - g_gameSlots[g_game5a7a78][v6c].field10;
            g_gameSlots[g_game5a7a78][v6c].flags08 &= ~0x8;
            for (v70 = 0; v70 < g_game5a7b1c; v70++) {
                if (Game_Helper40d146(g_game5a7a78, v6c, a1, g_game5a76d0[v70],
                                      g_game5a7610[v70], g_game5a811c) == 0) {
                    if (g_game5a8118 != 0) {
                        for (v78 = 0; v78 < g_game5a8118; v78++) {
                            if (g_gameSlots[a1][g_game5a7b60[v78]].field24 == g_game5a76d0[v70]
                                && Game_Helper40d146(g_game5a7a78, v6c, a1,
                                                     g_game5a7b60[v78], g_game5a7610[v70],
                                                     g_game5a811c) != 0) {
                                g_game5a78e8[v70] =
                                    g_game5a78e8[v70] | 1 << (g_game5a7ba4 & 0x1f);
                            }
                        }
                    }
                } else {
                    g_game5a78e8[v70] = g_game5a78e8[v70] | 1 << (g_game5a7ba4 & 0x1f);
                }
            }
            if (g_gameSlots[g_game5a7a78][v6c].field1f != 0) {
                g_game5a79f4 = g_game5a79f4 | 1 << (g_game5a7ba4 & 0x1f);
            }
            g_gameSlots[g_game5a7a78][v6c].flags08 &= ~0x8;
            g_game5a7690[g_game5a7ba4] =
                g_game7beaa8[g_game5a7a78] * g_game5a8120[g_game5a7ba4]
                / Game_Helper4ecec6(g_game5a7860[g_game5a7ba4] + 1, 1, 99);
            g_game5a7ba4 = g_game5a7ba4 + 1;
            if (g_game8bd200 == 1 && 6 < g_game5a7ba4) {
                break;
            }
        }
        memset(g_game5a7b00, 0, 0x1c);
        memset(g_game5a7ba8, 0, 0x1c);
        for (v6c = 0; v6c < g_game5a7b1c; v6c++) {
            for (v70 = 0; v70 < g_game5a7ba4; v70++) {
                v4 = Game_Helper411b19(a1, g_game5a76d0[v6c], g_game5a7a78,
                                       g_game5a80d8[v70]);
                if ((v4 & 1) != 0) {
                    g_game5a7b00[v6c] = g_game5a7b00[v6c] | 1 << (v70 & 0x1f);
                }
                if ((v4 & 2) != 0) {
                    g_game5a7ba8[v70] = g_game5a7ba8[v70] | 1 << (v6c & 0x1f);
                }
            }
        }
        g_game8bd200 = v68;
    }
    g_game5a7ba0 = 0;
    g_game97f1b0 = 0;
    g_game5a78e0 = g_game581808[g_game5a7b1c];
    return;
}

/* Chooses which of a1's creatures attack. Collects the legal attackers, drops
 * the weakest when there are more than seven, then enumerates every subset of
 * up to seven of them: for each subset it publishes the choice into the
 * 0x005a76xx arrays, lets 0x00421578 and the 0x00c7 query score it, adds a
 * penalty when the projected damage taken exceeds the life total, and keeps
 * the cheapest subset. Finally it stamps the winning mask into flags08 bit 2
 * of the chosen slots, bands the "must attack together" group onto one slot
 * through field24, and returns the mask (also left in g_game8ca260). */
// FUNCTION: SHANDALAR 0x0041f5e3
unsigned int Game_Helper41f5e3(int a1)
{
    int v50c;
    int v508;
    int v504;
    int v500;
    /* Ghidra splits this one array at the base of the second memset; the two
       halves are contiguous and the first memset covers all sixteen ints. */
    int v4fc[16];
    unsigned int v4bc;
    int v4b8;
    int v4b4;
    int v4b0;
    int v4ac[16];
    int v46c[16];
    int v42c[16];
    char v3ec[152];
    unsigned int v354[16];
    int v314;
    unsigned int v310;
    unsigned int v30c;
    int v308;
    int v304;
    int v300;
    int v2fc;
    int v2f8;
    int v2f4;
    unsigned int v2f0;
    int v2ec;
    int v2e8;
    int v2e4[16];
    int v2a4;
    int v2a0;
    int v29c;
    int v298;
    int v294;
    int v290[16];
    int v250;
    int v24c;
    int v248[14];
    int v210[18];
    int v1c8;
    int v1c4[16];
    int v184;
    char v180[152];
    int ve8;
    int ve4[16];
    int va4;
    int va0;
    int v9c;
    int v98;
    int v94;
    int v90;
    unsigned int v8c;
    int v88[16];
    int v48[16];
    unsigned int v8;
    /* Ghidra's uVar1/uVar2/uVar4 and the divisor it rebuilds: values that live
       across statements, so /Od gives each a slot. */
    int t0;
    unsigned int t1;
    unsigned int t2;
    int t3;

    Game_Helper41d33b(a1);
    v2ec = g_game789900;
    g_game789900 = 2;
    v298 = g_game8bd200;
    if (g_game8bd200 == 1) {
        g_game78e5e0 = g_game78e5e0 ^ 2;
    }
    for (v2f4 = 0; v2f4 < g_game78e970[a1]; v2f4++) {
        g_gameSlots[a1][v2f4].flags08 &= ~0x4;
    }
    Game_Helper40e1f5(0x15);
    v310 = 0;
    v2e8 = 0;
    v294 = 0;
    v8c = 0;
    v2a0 = 0;
    v8 = 0xffffffff;
    memset(v4fc, 0, 0x40);
    for (v2f4 = 0; v2f4 < g_game78e970[a1]; v2f4++) {
        v250 = g_gameSlots[a1][v2f4].field6c;
        if (v250 != -1
            && ((g_gameCards[v250].flags & 0x2) != 0
                || (g_gameSlots[a1][v2f4].flags08 & 0x1000000) != 0)
            && (g_gameSlots[a1][v2f4].flags08 & 0x820012) == 2
            && Game_Helper40cadc(a1, v2f4) != 0) {
            if ((g_gameSlots[a1][v2f4].flags08 & 0x8000) != 0) {
                v8c = v8c | 1 << (v2a0 & 0x1f);
            }
            if ((g_game78e5e0 & 2) == 0
                || (g_gameSlots[a1][v2f4].flags08 & 0x8000) != 0
                || (g_game97c860[a1][v2f4].flags08 & 0x40) == 0) {
                ve4[v2a0] = Game_Helper425a82(a1, v2f4);
                v4fc[v2a0] = v2f4;
                v2a0 = v2a0 + 1;
                g_gameSlots[a1][v2f4].flags08 |= 0x4;
            } else {
                v2e8 = v2e8 + g_game97c860[a1][v2f4].field00;
                v8 = v8 & g_game97c860[a1][v2f4].flags08;
                v310 = v310 | g_game97c860[a1][v2f4].flags08 & 0x200;
                v290[v294] = v2f4;
                v294 = v294 + 1;
            }
        }
    }
    v8 = v8 | v310;
    if (7 < v2a0) {
        v500 = 0;
        for (v2f4 = 0; v2f4 < g_game78e970[g_game5a7a78]; v2f4++) {
            v250 = g_gameSlots[g_game5a7a78][v2f4].field6c;
            if (Game_Helper411d90(g_game5a7a78, v2f4) != 0
                && (g_gameCards[v250].flags & 0x2) != 0
                && (g_gameSlots[g_game5a7a78][v2f4].flags08 & 0x10) == 0) {
                v500 = v500 + 1;
            }
        }
        for (v2f4 = 0; v2f4 < 0x10; v2f4++) {
            v1c4[v2f4] = 0;
        }
        for (v2f4 = 0; v2f4 < v2a0; v2f4++) {
            v1c4[v2f4] = g_game97c860[a1][v4fc[v2f4]].field00;
        }
        while (v500 != 0) {
            v50c = 0;
            v508 = 0;
            for (v2f4 = 0; v2f4 < v2a0; v2f4++) {
                if (v50c < v1c4[v2f4]) {
                    v50c = v1c4[v2f4];
                    v508 = v2f4;
                }
            }
            v1c4[v508] = 0;
            v500 = v500 - 1;
        }
        v504 = 0;
        for (v2f4 = 0; v2f4 < v2a0; v2f4++) {
            v504 = v504 + v1c4[v2f4];
        }
        if (g_game8c83f0[1 - a1] <= v504) {
            for (v2f4 = 0; v2f4 < v2a0; v2f4++) {
                g_gameSlots[a1][v4fc[v2f4]].flags08 |= 0x4;
            }
            g_game8ca260 = (1 << (v2a0 & 0x1f)) - 1;
            return g_game8ca260;
        }
    }
    if (7 < v2a0) {
        for (v2f4 = 0; v2f4 < v2a0; v2f4++) {
            for (v304 = v2f4; v304 < v2a0; v304++) {
                if (ve4[v2f4] < ve4[v304]) {
                    t0 = ve4[v2f4];
                    ve4[v2f4] = ve4[v304];
                    ve4[v304] = t0;
                    t0 = v4fc[v2f4];
                    v4fc[v2f4] = v4fc[v304];
                    v4fc[v304] = t0;
                    t2 = 1 << (v2f4 & 0x1f) & v8c;
                    t1 = v8c & ~(1 << (v2f4 & 0x1f));
                    v8c = t1 & ~(1 << (v304 & 0x1f));
                    if (t2 != 0) {
                        v8c = v8c | 1 << (v304 & 0x1f);
                    }
                    if ((1 << (v304 & 0x1f) & t1) != 0) {
                        v8c = v8c | 1 << (v2f4 & 0x1f);
                    }
                }
            }
        }
        if (6 < v2a0) {
            v2a0 = 7;
        }
        for (v2f4 = 7; v2f4 < 0x10; v2f4++) {
            g_gameSlots[a1][v4fc[v2f4]].flags08 &= ~0x4;
        }
        memset(&v4fc[7], 0, 0x24);
        v8c = 0;
        v2a0 = 0;
        for (v2f4 = 0; v2f4 < g_game78e970[a1]; v2f4++) {
            if ((g_gameSlots[a1][v2f4].flags08 & 0x4) != 0) {
                if ((g_gameSlots[a1][v2f4].flags08 & 0x8000) != 0) {
                    v8c = v8c | 1 << (v2a0 & 0x1f);
                }
                v4fc[v2a0] = v2f4;
                v2a0 = v2a0 + 1;
            }
        }
    }
    Game_Helper41e0d9(a1);
    memcpy(v88, g_game5a8010, 0x40);
    memcpy(v48, g_game5a7820, 0x40);
    memcpy(v354, g_game5a7610, 0x40);
    memcpy(v2e4, g_game5a8050, 0x40);
    memcpy(v42c, g_game5a78e8, 0x40);
    memcpy(g_game5a7930, g_game5a7b20, 0x40);
    memcpy(g_game5a7790, g_game5a7ac0, 0x40);
    Game_Fn4c3b83();
    g_game8bd200 = 1;
    Game_Helper40e1f5(199);
    g_game8bd200 = v298;
    for (v2f4 = 0; v2f4 < 8; v2f4++) {
        g_game77d170[g_game5a7a78][v2f4] = g_game77ed80[g_game5a7a78][v2f4];
    }
    ve8 = 0;
    v90 = 0;
    for (v2f4 = 0; v2f4 < g_game78e970[g_game5a7a78]; v2f4++) {
        v250 = g_gameSlots[g_game5a7a78][v2f4].field6c;
        if (Game_Helper411d90(g_game5a7a78, v2f4) != 0
            && (g_gameCards[v250].flags & 0x2) != 0) {
            v4ac[v90] = g_game97c860[g_game5a7a78][v2f4].field00;
            v46c[v90] = g_game97c860[g_game5a7a78][v2f4].field04;
            if ((g_gameCards[v250].flags3c & 0x8) != 0) {
                v4ac[v90] = v4ac[v90] + g_gameCards[v250].script(g_game5a7a78, v2f4, 0x39);
            }
            if ((g_gameCards[v250].flags3c & 0x10) != 0) {
                v46c[v90] = v46c[v90] + g_gameCards[v250].script(g_game5a7a78, v2f4, 0x3a);
            }
            if (ve8 < v46c[v90]) {
                ve8 = v46c[v90];
            }
            v3ec[v2f4] = (char)v90;
            v90 = v90 + 1;
        }
    }
    v4b0 = -1;
    for (v2f4 = 0; v2f4 < v2a0; v2f4++) {
        if (v4b0 == -1
            && v88[v2f4] < ve8
            && ve8 <= v88[v2f4] + v2e8
            && (v354[v2f4] & v8) == v354[v2f4]) {
            v88[v2f4] = v88[v2f4] + v2e8;
            v4b0 = v4fc[v2f4];
        }
    }
    v1c8 = 0;
    for (v304 = 0; v304 < g_game78e970[a1]; v304++) {
        if (Game_Helper411d90(a1, v304) != 0
            && (g_gameCards[g_gameSlots[a1][v304].field6c].flags & 0x2) != 0) {
            v1c4[v1c8] = g_game97c860[a1][v304].field00;
            v248[v1c8] = g_game97c860[a1][v304].field04;
            ve4[v1c8] = (v1c4[v1c8] + 1) * (v248[v1c8] + 1);
            for (v30c = 0; (int)v30c < g_game5a7b1c; v30c++) {
                if (g_game5a76d0[v30c] == v304) {
                    ve4[v1c8] = g_game5a8050[v30c];
                }
            }
            v180[v304] = (char)v1c8;
            v1c8 = v1c8 + 1;
        }
    }
    Game_Fn4c3e34();
    v2a4 = 9999;
    Game_Helper4c5bf6((int)&v300, 0);
    for (v30c = 0; (int)v30c < 1 << (v2a0 & 0x1f); v30c++) {
        v308 = 1;
        g_game5a7b1c = 0;
        g_game5a7818 = 0;
        for (v2f4 = 0; v2f4 < v2a0; v2f4++) {
            g_gameSlots[a1][v4fc[v2f4]].flags08 &= ~0x4;
            v2fc = g_gameCards[g_gameSlots[a1][v4fc[v2f4]].field6c].mask;
            if ((v30c & 1 << (v2f4 & 0x1f)) == 0) {
                if (v2fc == 0x19f || v2fc == 0x84) {
                    v308 = 0;
                }
                if ((v8c & 1 << (v2f4 & 0x1f)) != 0) {
                    v308 = 0;
                }
            } else {
                g_gameSlots[a1][v4fc[v2f4]].flags08 |= 0x4;
                g_game5a76d0[g_game5a7b1c] = v4fc[v2f4];
                g_game5a8010[g_game5a7b1c] = v88[v2f4];
                g_game5a7820[g_game5a7b1c] = v48[v2f4];
                g_game5a7610[g_game5a7b1c] = v354[v2f4];
                g_game5a8050[g_game5a7b1c] = v2e4[v2f4];
                g_game5a78e8[g_game5a7b1c] = v42c[v2f4];
                g_game5a7b20[g_game5a7b1c] = g_game5a7930[v2f4];
                g_game5a7ac0[g_game5a7b1c] = g_game5a7790[v2f4];
                if (v2fc == 0x28 || v2fc == 0x98) {
                    g_game5a7818 = g_game5a7818 | 1 << (g_game5a7b1c & 0x1f);
                }
                g_game5a7b1c = g_game5a7b1c + 1;
            }
        }
        if (v308 != 0) {
            if (g_game97db20 != 0) {
                v9c = 0;
                for (v2f4 = 0; v2f4 < g_game5a7b1c; v2f4++) {
                    if ((g_gameSlots[a1][g_game5a76d0[v2f4]].flags08 & 0x2000) == 0) {
                        g_game5a7a80[v2f4] = v2f4;
                    } else {
                        g_game5a7a80[v2f4] = 99;
                        v9c = v9c - 1;
                    }
                    v9c = v9c + 1;
                }
                for (v2f4 = 0; v2f4 < g_game5a7b1c; v2f4++) {
                    for (v304 = v2f4; v304 < g_game5a7b1c; v304++) {
                        if (g_game5a7a80[v2f4] != 99
                            && g_game5a8010[g_game5a7a80[v2f4]] < g_game5a8010[g_game5a7a80[v304]]) {
                            t0 = g_game5a7a80[v304];
                            g_game5a7a80[v304] = g_game5a7a80[v2f4];
                            g_game5a7a80[v2f4] = t0;
                        }
                    }
                }
                for (v2f4 = 0; v2f4 < g_game5a7b1c; v2f4++) {
                    if (g_game5a7a80[v2f4] == 99) {
                        g_game5a78a0[v2f4] = 99;
                    } else {
                        g_game5a78a0[g_game5a7a80[v2f4]] = v2f4;
                    }
                }
            }
            g_game5a7ba0 = 1;
            Game_Helper421578(a1);
            Game_Fn4c3b83();
            g_game8bd200 = 1;
            Game_Helper40e1f5(199);
            g_game8bd200 = v298;
            v2f8 = 0;
            for (v98 = 0; v98 < 8; v98++) {
                v210[v98 * 2 + 3] = -1;
            }
            for (v2f4 = 0; v2f4 < g_game78e970[g_game5a7a78]; v2f4++) {
                v250 = g_gameSlots[g_game5a7a78][v2f4].field6c;
                if (Game_Helper411d90(g_game5a7a78, v2f4) != 0
                    && (g_gameCards[v250].flags & 0x2) != 0
                    && (g_gameSlots[g_game5a7a78][v2f4].flags08 & 0x400) != 0) {
                    v90 = (int)v3ec[v2f4];
                    v4b8 = v4ac[v3ec[v2f4]];
                    if ((g_game97c860[g_game5a7a78][v2f4].flags08 & 0x80000000) != 0) {
                        v4b8 = v4ac[v3ec[v2f4]] + 2;
                    }
                    for (v304 = 0; v304 < 8; v304++) {
                        if (v4b8 > v210[v304 * 2 + 3]) {
                            for (v98 = 7; v304 < v98; v98--) {
                                v210[v98 * 2 + 2] = v210[v98 * 2];
                                v210[v98 * 2 + 3] = v210[v98 * 2 + 1];
                            }
                            v210[v304 * 2 + 2] = v2f4;
                            v210[v304 * 2 + 3] = v4b8;
                            break;
                        }
                    }
                }
            }
            v98 = 0;
            while (v98 < 8 && v210[v98 * 2 + 3] != -1) {
                v2f4 = v210[v98 * 2 + 2];
                v24c = Game_Helper40d7b2(g_game5a7a78, v2f4, 0x34, -1);
                v90 = (int)v3ec[v2f4];
                v4b8 = v4ac[v90];
                if ((g_game97c860[g_game5a7a78][v2f4].flags08 & 0x80000000) != 0) {
                    v4b8 = v4ac[v90] + 2;
                }
                v4b4 = v46c[v90];
                v314 = 0;
                v4bc = 0;
                v1c8 = 0;
                va0 = 0x7fff;
                for (v304 = 0; v304 < g_game78e970[a1]; v304++) {
                    if (Game_Helper411d90(a1, v304) != 0
                        && (g_gameSlots[a1][v304].flags08 & 0x400) != 0
                        && (g_gameCards[g_gameSlots[a1][v304].field6c].flags & 0x2) != 0) {
                        v1c8 = (int)v180[v304];
                        va4 = v1c4[v1c8];
                        v184 = v248[v1c8];
                        if ((g_gameSlots[a1][v304].flags08
                             & (Game_Helper40cd94(a1, v304) == 0 ? 0xc : 0x8)) == 0) {
                            t1 = v4bc;
                            if (Game_Helper40d146(a1, v304, g_game5a7a78, v2f4, v24c,
                                                  v300) != 0) {
                                v4bc = v4bc | 1;
                                if (v4b8 < v184 || v4b4 <= va4) {
                                    v4bc = t1 | 3;
                                    g_gameSlots[a1][v304].flags08 |= 0x8;
                                    break;
                                }
                                if (ve4[v1c8] < va0) {
                                    va0 = ve4[v1c8];
                                    v314 = v304;
                                }
                            }
                        }
                    }
                }
                if ((v4bc & 2) == 0) {
                    t3 = g_game8c83f0[a1] - v4b8;
                    if (t3 < 2) {
                        t3 = 1;
                    }
                    v29c = g_game7beaa0[a1] * v4b8 * 0x18 / 4
                        / Game_Helper4ecec6(t3, 1, 99);
                    if (v4bc != 0) {
                        v94 = g_game7beaa8[a1] * va0 / 8;
                        if (v94 <= v29c) {
                            g_game5a8090 = g_game5a8090 + v94;
                            g_gameSlots[a1][v314].flags08 |= 0x8;
                            v98 = v98 + 1;
                            continue;
                        }
                    }
                    v2f8 = v2f8 + v4b8;
                    g_game5a8090 = g_game5a8090 + v29c;
                }
                v98 = v98 + 1;
            }
            Game_Fn4c3e34();
            if (g_game8c83f0[a1] <= v2f8
                && 0 < g_game8c83f0[a1]
                && 0 < g_game8c83f0[1 - a1]) {
                g_game5a8090 = g_game5a8090 + ((v2f8 - g_game8c83f0[a1]) + 2) * 0x80;
                g_game5a8090 = g_game5a8090 + g_game5a79f0;
            }
            if (g_game5a8090 < v2a4) {
                v2a4 = g_game5a8090;
                v2f0 = v30c;
            }
        }
    }
    if (v2f0 != 0 && (g_game7bfe78 & 0x2000000) != 0) {
        Game_Helper40e1f5(0x92);
    }
    for (v2f4 = 0; v2f4 < v2a0; v2f4++) {
        g_gameSlots[a1][v4fc[v2f4]].flags08 &= ~0x4;
        if ((v2f0 & 1 << (v2f4 & 0x1f)) != 0) {
            g_gameSlots[a1][v4fc[v2f4]].flags08 |= 0x4;
            if ((g_game7bfe78 & 0x80000) != 0) {
                g_game7bf4b8 = a1;
                g_game8e2fc4 = v4fc[v2f4];
                Game_Helper410efc(g_game7a82e0, 0xde, 0x8c7470, 0);
            }
            if (v294 != 0
                && v4b0 == -1
                && (v354[v2f4] & v8) == v354[v2f4]) {
                v4b0 = v4fc[v2f4];
            }
        }
    }
    if (v294 == 0
        || v4b0 == -1
        || (g_gameSlots[a1][v4b0].flags08 & 0x4) == 0) {
        if (v294 != 0 && ve8 == 0) {
            for (v2f4 = 0; v2f4 < v294; v2f4++) {
                g_gameSlots[a1][v290[v2f4]].flags08 |= 0x4;
                if ((g_game7bfe78 & 0x80000) != 0) {
                    g_game7bf4b8 = a1;
                    g_game8e2fc4 = v290[v2f4];
                    Game_Helper410efc(g_game7a82e0, 0xde, 0x8c7470, 0);
                }
            }
            v2f0 = 1;
        }
    } else {
        for (v2f4 = 0; v2f4 < v294; v2f4++) {
            g_gameSlots[a1][v290[v2f4]].field24 = (signed char)v4b0;
            g_gameSlots[a1][v290[v2f4]].flags08 |= 0x4;
            if ((g_game7bfe78 & 0x80000) != 0) {
                g_game7bf4b8 = a1;
                g_game8e2fc4 = v290[v2f4];
                Game_Helper410efc(g_game7a82e0, 0xde, 0x8c7470, 0);
            }
        }
        g_gameSlots[a1][v4b0].field24 = (signed char)v4b0;
    }
    g_game789900 = v2ec;
    g_game8ca260 = v2f0;
    return v2f0;
}
