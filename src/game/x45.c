/* Shandalar, 0x0045xxxx: a blocking prompt round-trip, one card script and
 * the attack-window procedure. Derived from Ghidra's decompilation of
 * orig/Shandalar.exe; names are addresses because nothing establishes meaning.
 *
 * Address literals passed where a pointer is wanted (0x583f98, 0x585864, ...)
 * are pointers into the original image's .rdata -- string constants and
 * format strings. Pushing the literal reproduces the original instruction
 * without carrying the copyrighted data, which is the convention cards43.c
 * established.
 *
 * The 0x19c-byte records reached through the window's extra long are kept as
 * the raw index arithmetic the disassembly shows rather than a struct: the
 * shape is what /Od emitted, and a struct would re-roll the lea chains. The
 * layout the arithmetic proves is
 *     +0x000 int   key
 *     +0x004 HWND  a[50]
 *     +0x0cc int   na
 *     +0x0d0 HWND  b[50]
 *     +0x198 int   nb          -- 0x19c total, 100 records (malloc 0xa0f0).
 */

#include <windows.h>
#include <stdlib.h>
#include <string.h>

#include "game/gameslot.h"

/* Referenced by this TU only; not in the shared headers. Addresses from the
   disassembly. */

/* 0x00450233 */
extern int g_game7beac0;              /* char[]: a prompt-name prefix        */
extern CRITICAL_SECTION g_game79fea0;
extern int g_game8bcf78;              /* HWND                                */
extern int g_game8bcfec;              /* HWND                                */
extern int g_game8e3304;              /* HWND                                */
extern int g_game951960;
extern int g_game93a8c0;
extern int g_game74b8b8;
extern int g_game7658c0;
extern int g_game74b15c;
extern int g_game780690;
/* The record the 0x403 round-trip stages into; a dword count and nine dwords
   followed by a byte flag at +0x28. */
extern int g_game8e3350;
extern int g_game8e3354;
extern int g_game8e3358;
extern int g_game8e335c;
extern int g_game8e3360;
extern int g_game8e3364;
extern int g_game8e3368;
extern int g_game8e336c;
extern int g_game8e3370;
extern int g_game8e3374;
extern unsigned char g_game8e3378;

/* 0x0045cdc8 */
extern int g_game57f014;              /* GWL offset, loaded not immediate    */
extern int g_game57f018;              /* GWL offset, loaded not immediate    */
extern int g_game585734;
extern int g_game78e5f0;              /* char[]: help-file directory prefix  */
extern int g_game8bd390;
extern int g_game8c7424;
extern int g_game8e31c8;
extern int g_game8e33e0;              /* char[]: art directory prefix        */
extern int g_game930acc;
extern int g_game939160;              /* HINSTANCE                           */
extern int g_game94d638;
extern int g_game94e8d0;              /* HWND                                */
extern int g_game951be4;              /* HWND                                */
extern int g_game5a8fd8;
extern int g_game5a8fe4;
extern int g_game5a8fe8;              /* HWND                                */
extern int g_game5a8ff0;              /* char[]: menu item text              */
extern int g_game5a900c;
extern int g_game5a9010;
extern int g_game5a9014;
extern int g_game5a9018;
extern int g_game5a9020;              /* char[]: menu item text              */
extern int g_game5a905c;
extern int g_game5a9060;              /* HMENU                               */
extern int g_game5a906c;
extern int g_game5a9070;
extern int g_game5a907c;
extern int g_game5a9080;
extern int g_game5a9084;
extern int g_game5a90ac;
extern int g_game5a90b0;

/* Names the other module headers already give these addresses. */
int Game_Helper452355(int a1, int a2);
int Game_Helper558356(int a1);
int Game_Helper56c91f(int a1, int a2);
int Game_Helper56c931(int a1, int a2);
int Game_Fn56c943(int a1);
int Game_Fn56c94e(int a1);
int Sid_WindowMatchesKey(void *hwnd, const int *key);
int Sid_WindowHasId(void *hwnd, int id);
int Sid_GetWindowId(void *hwnd);
long Sid_GetWindowExtra8(void *hwnd);

/* Not reconstructed anywhere yet. */
int Game_Fn451c62(int a1);
int Game_Fn45225b(int a1, int a2);
int Game_Fn452442(int a1, int a2);
int Game_Fn452516(int a1, int a2);
int Game_Fn452743(void *a1, int a2, int a3);
int Game_Fn452cce(int a1, int a2);
int Game_Fn453419(int a1, int a2);
int Game_Fn453d4d(void *a1, int a2);
int Game_Fn4607dd(void *a1, void *a2, int a3);
int Game_Fn4608d2(void *a1);
int Game_Fn4624cf(void *a1, void *a2, int a3, void *a4, void *a5);
int Game_Fn46469a(void *a1);
int Game_Fn4649c0(void *a1, void *a2, void *a3);
int Game_Fn464b72(void *a1, void *a2, void *a3);
int Game_Fn465efa(void *a1, unsigned int a2, int a3, int a4);
int Game_Fn466a34(void);
int Game_Fn466d5f(void *a1, unsigned int a2, int a3, int a4);
int Game_Fn4c8b20(char *a1);
int Game_Fn50755e(void *a1, void *a2);
int Game_Fn50775c(void *a1, void *a2, int a3, void *a4);


/* Blocking prompt round-trip. Repaints, drops the pending selection, sends
 * the staged request to the window at 0x008e3304 and waits; in the replay
 * role (a1 != g_game8c776c with bit 2 of g_game93a934 set) it reads the
 * recorded answer back out of the 0x008e3350 block instead. A -5 answer means
 * the thread is being torn down, so it posts and exits. */
// FUNCTION: SHANDALAR 0x00450233
int Game_Fn450233(int a1, int a2, char *a3, int a4, int a5, int a6, int a7,
                  int a8, int *a9, int *a10, int a11, int a12)
{
    POINT v3d8;
    HWND v3d0;
    POINT v3cc;
    HWND v3c4;
    char v3c0[200];
    char v2f8[500];
    int v104;
    int v100;
    int vfc;
    int vf4;
    unsigned int vf0;
    int vec;
    int ve8;
    int ve4;
    int ve0;
    int vdc;
    int vd8;
    char vd4[200];
    int vc;
    int v8;

    vf0 = 0;
    if (a3 == 0) {
        strcpy(v3c0, (char *)0x583f98);
        strcpy(v2f8, (char *)&g_game7beac0);
    } else {
        strcpy(v3c0, a3);
        strcpy(v2f8, (char *)&g_game7beac0);
        strcat(v2f8, a3);
    }
    EnterCriticalSection(&g_game79fea0);
    if (g_game7bfe74 != -1) {
        if ((g_game93a934 & 0x2) == 0) {
            g_game7bfe74 = -1;
        }
        if (IsWindowVisible((HWND)g_game8bcfec) == 0) {
            InvalidateRect((HWND)g_game8bcf78, (RECT *)0, 1);
        } else {
            InvalidateRect((HWND)g_game8bcfec, (RECT *)0, 1);
        }
    }
    LeaveCriticalSection(&g_game79fea0);
    Game_Helper558aea(0, 0xff);
    GetCursorPos(&v3cc);
    v3c4 = WindowFromPoint(v3cc);
    SendMessageA(v3c4, 0x20, (WPARAM)v3c4, 0x2000001);
    vec = a2;
    ve8 = a5;
    ve4 = a6;
    ve0 = a7;
    vdc = a8;
    vc = a11;
    v8 = a12;
    strcpy(vd4, v3c0);
    vd8 = a4;
    if (a1 == g_game8c776c || (g_game93a934 & 0x2) == 0) {
        if (g_game951960 != 0) {
            Game_Helper40ebb9(0x25);
            g_game951960 = 0;
        }
        if (g_game7bf0ec == 0 || g_game94d464 != 0xa || g_game93a8c0 != 0) {
            vf4 = SendMessageA((HWND)g_game8e3304, 0x403, (WPARAM)&vec, (LPARAM)&v104);
        } else {
            vf4 = 0;
            v104 = -2;
            vfc = -2;
            v100 = -1;
            vf0 = 0;
            g_game74d26c = -1;
            g_game74f1e0 = -1;
            g_game74b8b8 = 0;
            g_game7658c0 = 0;
            g_game74b15c = 0;
        }
        if ((g_game93a934 & 0x2) != 0) {
            g_game8e3350 = 0xc;
            g_game8e3354 = vf4;
            g_game8e3358 = v104;
            g_game8e3378 = 1 - (char)vf0;
            if (g_game74d26c == -1) {
                g_game8e335c = g_game74d26c;
            } else {
                g_game8e335c = 1 - g_game74d26c;
            }
            g_game8e3360 = g_game74f1e0;
            g_game8e3364 = vfc;
            if (vf4 == 0) {
                g_game8e3368 = v100;
            } else {
                g_game8e3368 = 1 - v100;
            }
            g_game8e336c = g_game74b8b8;
            g_game8e3370 = g_game7658c0;
            g_game8e3374 = 1 - g_game74b15c;
            Game_Helper56c931(a1, 0xc);
            Sleep(0xfa);
            Game_Fn56c943(a1);
        }
    } else {
        Game_Helper558356((int)&g_game7beac0);
        g_game951960 = 1;
        Game_Helper56c91f(a1, 0xc);
        vf4 = g_game8e3354;
        v104 = g_game8e3358;
        g_game77f440 = g_game8e335c;
        g_game74ccc4 = g_game8e3360;
        vfc = g_game8e3364;
        v100 = g_game8e3368;
        g_game74b8b8 = g_game8e336c;
        g_game7658c0 = g_game8e3370;
        g_game74b15c = g_game8e3374;
        vf0 = g_game8e3378;
        Game_Fn56c94e(a1);
        Game_Helper558356(0x583f9c);
    }
    *a9 = v104;
    *a10 = v100;
    a10[1] = vfc;
    Game_Fn451c62(0);
    if (v104 != -5) {
        GetCursorPos(&v3d8);
        v3d0 = WindowFromPoint(v3d8);
        SendMessageA(v3d0, 0x20, (WPARAM)v3d0, 0x2000001);
        EnterCriticalSection(&g_game79fea0);
        if (g_game780690 == -2) {
            if (g_game74f1e0 == -1) {
                if ((g_game93a934 & 0x2) == 0) {
                    g_game7bfe74 = -1;
                    g_game7beb20 = -1;
                } else {
                    g_game7beb20 = g_game74d26c;
                    g_game7bfe74 = g_game74f1e0;
                }
            } else {
                g_game7beb20 = g_game74d26c;
                g_game7bfe74 = g_game74f1e0;
            }
        } else if ((g_game93a934 & 0x2) == 0) {
            g_game7bfe74 = -1;
            g_game7beb20 = -1;
        } else {
            g_game7beb20 = g_game74d26c;
            g_game7bfe74 = g_game74f1e0;
        }
        if (IsWindowVisible((HWND)g_game8bcfec) == 0) {
            InvalidateRect((HWND)g_game8bcf78, (RECT *)0, 1);
        } else {
            InvalidateRect((HWND)g_game8bcfec, (RECT *)0, 1);
        }
        LeaveCriticalSection(&g_game79fea0);
        return vf4;
    }
    PostMessageA((HWND)g_game8e3304, 0x401, vf0, 0);
    /* Does not return. */
    ExitThread(vf0);
}

/* Card script. Handles the "may not be blocked except by" family: on the
 * 0x7d/0x7e attack-legality pair it walks the opponent's row looking for a
 * bigger field34 than the source slot's, and it maintains the +0x38 / +0x0c /
 * +0x48 attachment triple on 0x72, 0x77 and every other event. */
// FUNCTION: SHANDALAR 0x0045aef3
int Game_CardScript45aef3(int a, int b, int kind)
{
    short iv;
    int ig;
    int ih;
    int ii;
    int v8;

    if (kind == 0x82
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_gameSlots[a][b].field11c &= 0xfffffffd;
    }
    if (g_game94d464 == 1
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        if (kind == 0x7d
            && (g_gameSlots[a][b].field11c & 0x1) != 0
            && (g_gameSlots[a][b].field11c & 0x2) == 0
            && (g_game939e64 & g_gameCards[g_gameSlots[a][b].field6c].flags) == 0) {
            if ((g_game8c7468 == a && (g_game93a934 & 0x2) == 0)
                || g_game8bd200 == 1
                || g_game939410 != 0) {
                if ((signed char)g_gameSlots[a][b].field0c == -1
                    || (signed char)g_gameSlots[a][b].field0c == g_game8c7468) {
                    g_game7a01e0 |= 2;
                } else {
                    iv = g_gameSlots[(signed char)g_gameSlots[(signed char)g_gameSlots[a][b].field0c][g_gameSlots[a][b].field48].field50][g_gameSlots[(signed char)g_gameSlots[a][b].field0c][g_gameSlots[a][b].field48].field04].field34;
                    ih = 1 - a;
                    v8 = 0;
                    while (v8 < g_game78e970[ih] && g_game7a01e0 == 0) {
                        ig = Game_Helper411d90(ih, v8);
                        if (ig != 0
                            && (g_gameCards[g_gameSlots[ih][v8].field6c].flags & 0x2) != 0
                            && iv < g_gameSlots[ih][v8].field34) {
                            g_game7a01e0 |= 2;
                        }
                        v8++;
                    }
                }
            } else {
                g_game7a01e0 |= 1;
            }
        }
        if (kind == 0x7e) {
            g_gameSlots[a][b].field11c |= 2;
        }
    }
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_gameSlots[a][b].field38 = -1;
    }
    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x10) == 0
            && ((g_gameSlots[a][b].flags08 & 0x30000) == 0
                || (g_gameCards[g_gameSlots[a][b].field6c].flags & 0x2) == 0)) {
            ih = Game_Helper441699(a, 7, 3);
            if (ih != 0) {
                ih = Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
                if (ih != 0) {
                    return 1;
                }
            }
        }
    } else {
        if (kind == 0x6d) {
            Game_Helper4143e0(a, 0, 3);
            if (g_game8c83ec != 1) {
                Game_Helper56ca10(0x585724, 0x585714);
                ih = Game_Helper49f87b(a, 2, b);
                if (ih == 0) {
                    g_game8c83ec = 1;
                } else {
                    g_gameSlots[a][b].flags08 |= 0x10;
                }
            }
        }
        if (kind == 0x72
            && g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field6c != -1) {
            ii = Game_Helper4bea85(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0);
            if (ii == 0) {
                g_game8c83ec = 1;
            } else {
                g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field0c = g_gameSlots[a][b].field74;
                g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field48 = g_gameSlots[a][b].field78;
                g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 = g_gameSlots[a][b].field78;
                Game_Helper4885e4(g_gameSlots[a][b].field74, g_gameSlots[a][b].field78);
            }
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
        }
        if (g_game94d178 == 0xd4
            && g_game951bdc == b
            && g_game8e1aa8 == a
            && a == g_game8c8afc
            && g_gameSlots[a][b].field38 != -1
            && (signed char)g_gameSlots[a][b].field0c != -1
            && g_gameSlots[a][b].field48 != -1
            && g_game7bf4b8 == a
            && g_game8e2fc4 == b) {
            if (kind == 0x7d) {
                g_game7a01e0 |= 2;
            }
            if (kind == 0x7e) {
                Game_Helper488474((signed char)g_gameSlots[a][b].field0c, g_gameSlots[a][b].field48);
            }
        }
        if (kind == 0x77) {
            if (g_game951bdc == b
                && g_game8e1aa8 == a
                && g_gameSlots[a][b].field38 != -1) {
                Game_Helper488474((signed char)g_gameSlots[a][b].field0c, g_gameSlots[a][b].field48);
                g_gameSlots[a][b].field38 = -1;
            }
            if ((signed char)g_gameSlots[a][b].field0c == g_game8e1aa8
                && g_gameSlots[a][b].field48 == g_game951bdc) {
                g_gameSlots[a][b].field38 = -1;
                g_gameSlots[a][b].field48 = g_gameSlots[a][b].field38;
                g_gameSlots[a][b].field0c = g_gameSlots[a][b].field48;
            }
        }
        if (g_gameSlots[a][b].field38 != -1
            && (g_gameSlots[a][b].flags08 & 0x10) == 0) {
            g_gameSlots[a][b].field38 = -1;
            Game_Helper488474((signed char)g_gameSlots[a][b].field0c, g_gameSlots[a][b].field48);
            g_gameSlots[a][b].field48 = -1;
            g_gameSlots[a][b].field0c = g_gameSlots[a][b].field48;
        }
    }
    return 0;
}

/* The attack window's procedure. Keeps a malloc'd table of up to 100
 * attacker/blocker groups in the window's extra longs, owns the sword-shield
 * and minimised child windows, paints its own non-client frame (0x85/0x86) and
 * implements the 0x400-0x437 private protocol the game layer drives it with. */
// FUNCTION: SHANDALAR 0x0045cdc8
int Game_Fn45cdc8(int a1, unsigned int a2, int a3, int a4)
{
    int *iv;
    int ig;
    int ih;
    LONG ii;
    HWND ij;
    HWND ik;
    HBRUSH im;
    HGDIOBJ ip;
    unsigned int iq;
    UINT ir;
    HMENU is;
    WPARAM it;
    HINSTANCE iw;
    LPARAM ix;
    LPVOID iy;
    int v598;
    POINT v594;
    RECT v58c;
    unsigned int v57c;
    int v574;
    HGDIOBJ v570;
    RECT v56c;
    HBITMAP v55c;
    char v558[100];
    HBRUSH v4f4;
    HDC v4f0;
    int v4ec[6];    /* BITMAP; [1] is local_4e8, [2] is local_4e4 */
    int v4d4;
    int v4d0;
    int v4cc;
    int v4c8;
    int v4c4;
    int v4c0;
    unsigned int v4bc;
    RECT v4b8;
    RECT v4a8;
    HGDIOBJ v498;
    RECT v494;
    int v484;
    HGDIOBJ v480;
    HWND v47c;
    int v474;
    unsigned int v470;
    RECT v46c;
    int v45c;
    HWND v454;
    int v450;
    unsigned int v44c;
    unsigned int v448;
    int v444;
    RECT v440;
    RECT v430;
    HWND v420;
    unsigned int v41c;
    unsigned int v418;
    char v414[264];
    HWND v30c;
    int v308[6];    /* BITMAP; [1] is local_304 */
    RECT v2f0;
    RECT v2e0;
    LRESULT v2d0;
    HWND v2cc;
    char v2c8[264];
    DWORD v1c0;
    int v1bc;
    int v1b8[6];    /* BITMAP; [1] is local_1b4, [2] is local_1b0 */
    int v1a0;
    int v19c;
    int v198;
    RECT v194;
    LPARAM v184;
    int v180[2];
    int v178;
    /* v174 and v170 are a two-int pair passed by address; they must stay
       adjacent and in this order. */
    int v174;
    int v170;
    int v16c;
    unsigned int v168;
    HWND v164;
    int v160;
    int v15c;
    int v158;
    unsigned int v154;
    unsigned int v150;
    int v14c;
    unsigned int v148;
    HWND v144;
    unsigned int v140;
    int v13c;
    int v138;
    HWND v134;
    HWND v130;
    int v12c;
    int v128;
    HWND v124;
    unsigned int v120;
    int v118;
    int v114;
    HWND v110;
    unsigned int v10c;
    int v108;
    int v104;
    int v100;
    HWND vfc;
    int vf8;
    int vf4;
    int vf0;
    int vec;
    HWND ve8;
    unsigned int ve4;
    int ve0;
    int vdc;
    int vd8;
    int vd4;
    int vd0;
    int vcc;
    HWND vc8;
    RECT vc4;
    HWND vb4;
    int vb0;
    int vac;
    int va8;
    HWND va4;
    HWND va0;
    int v9c;
    int v98;
    HWND v94;
    int v90;
    HWND v8c;
    HWND v88;
    int v84;
    int v80;
    int v7c;
    unsigned int v78;
    int v74;
    HWND v70;
    HWND v6c;
    int v68;
    int v64;
    int v60;
    int v5c;
    int v58;
    int v54;
    int v50;
    HWND v4c;
    int v48;
    int v44;
    int v40;
    POINT v3c;
    int v34;
    int v30;
    HWND v2c;
    int v28;
    RECT v24;
    int v14;
    HWND v10;
    void *vc;
    LONG v8;

    switch (a2) {
    case 0x464:
        v2c = (HWND)a3;
        vc = (void *)GetWindowLongA((HWND)a1, g_game57f014);
        v8 = GetWindowLongA((HWND)a1, g_game57f018);
        v10 = GetDlgItem((HWND)a1, 0);
        v14 = 0;
        v30 = 0;
        while (v30 < v8 && v14 == 0) {
            if (*(HWND *)((int)vc + v30 * 0x19c) == v2c) {
                v14 = 1;
                v28 = 5000;
                v40 = -5000;
                for (v34 = 0; v34 < *(int *)((int)vc + 0xcc + v30 * 0x19c); v34 = v34 + 1) {
                    GetWindowRect(*(HWND *)(v34 * 4 + v30 * 0x19c + 4 + (int)vc), &v24);
                    if (v24.left < v28) {
                        v28 = v24.left;
                    }
                    if (v40 < v24.right) {
                        v40 = v24.right;
                    }
                }
                for (v34 = 0; v34 < *(int *)((int)vc + 0x198 + v30 * 0x19c); v34 = v34 + 1) {
                    GetWindowRect(*(HWND *)(v34 * 4 + v30 * 0x19c + 0xd0 + (int)vc), &v24);
                    if (v24.left < v28) {
                        v28 = v24.left;
                    }
                    if (v40 < v24.right) {
                        v40 = v24.right;
                    }
                }
            }
            v30 = v30 + 1;
        }
        if (v14 != 0) {
            GetWindowRect((HWND)a1, &v24);
            if (v28 < v24.left) {
                v3c.x = v28;
                v3c.y = 0;
                ScreenToClient((HWND)a1, &v3c);
            } else if (v24.right < v40) {
                v3c.x = v28;
                v3c.y = 0;
                ScreenToClient((HWND)a1, &v3c);
            }
        }
        return 0;
    case 6:
        if (((unsigned int)a3 & 0xffff) == 1 || ((unsigned int)a3 & 0xffff) == 2) {
            SendMessageA((HWND)g_game951be4, 0x86, 1, 0);
        } else {
            SendMessageA((HWND)g_game951be4, 0x86, 0, 0);
        }
        iq = DefWindowProcA((HWND)a1, 6, (WPARAM)a3, (LPARAM)a4);
        return iq;
    case 1:
        v8 = 0;
        SetWindowLongA((HWND)a1, g_game57f018, 0);
        vc = malloc(0xa0f0);
        SetWindowLongA((HWND)a1, g_game57f014, (LONG)vc);
        v2cc = CreateWindowExA(0, (char *)0x585864, (char *)0x585860, 0x50000000, 0, 0,
                               0, 0, (HWND)a1, (HMENU)0, (HINSTANCE)g_game939160, (LPVOID)0);
        if (v2cc != (HWND)0) {
            SendMessageA(v2cc, 0x464, g_game5a900c, 1);
            SendMessageA(v2cc, 0x466, g_game5a8fd8, g_game585734);
        }
        iy = (LPVOID)0;
        is = (HMENU)0;
        iw = (HINSTANCE)g_game939160;
        ik = GetParent((HWND)a1);
        g_game951be4 = (int)CreateWindowExA(0, (char *)0x585884, (char *)0x585880, 0x80c00000,
                                           0, 0, 0, 0, ik, is, iw, iy);
        g_game5a8fe8 = (int)CreateWindowExA(0, (char *)0x58589c, (char *)0x585898, 0x80000000,
                                           0, 0, 0, 0, (HWND)a1, (HMENU)0,
                                           (HINSTANCE)g_game939160, (LPVOID)0);
        if (vc != (void *)0 && v2cc != (HWND)0
            && (HWND)g_game951be4 != (HWND)0 && (HWND)g_game5a8fe8 != (HWND)0) {
            return 0;
        }
        if (vc != (void *)0) {
            free(vc);
        }
        return -1;
    case 2:
        vc = (void *)GetWindowLongA((HWND)a1, g_game57f014);
        free(vc);
        return 0;
    case 0x14:
        v30c = (HWND)a3;
        Game_Fn46469a((void *)a3);
        GetClientRect((HWND)a1, &v2e0);
        v2d0 = SendDlgItemMessageA((HWND)a1, 0, 0xe1, 0, 0);
        if ((HANDLE)g_game5a90b0 == (HANDLE)0) {
            strcpy(v414, (char *)&g_game8e33e0);
            strcat(v414, (char *)0x5858ac);
            g_game5a90b0 = Game_Fn4c8b20(v414);
        }
        if ((HANDLE)g_game5a90b0 == (HANDLE)0) {
            im = GetStockObject(4);
            FillRect((HDC)v30c, &v2e0, im);
        } else {
            CopyRect(&v2f0, &v2e0);
            GetObjectA((HGDIOBJ)g_game5a90b0, 0x18, v308);
            v2f0.left = -(v2d0 % v308[1]);
            Game_Fn464b72(v30c, &v2f0, (void *)g_game5a90b0);
        }
        return 1;
    case 0x10:
        ShowWindow((HWND)a1, 0);
        ShowWindow((HWND)g_game5a8fe8, 0);
        return 0;
    case 0x20:
        iq = Game_Fn465efa((void *)a1, 0x20, a3, a4);
        return iq;
    case 0x18:
        if (a3 == 0) {
            ShowWindow((HWND)g_game951be4, 0);
        } else {
            ShowWindow((HWND)g_game951be4, 5);
        }
        PostMessageA((HWND)g_game94e8d0, 0x403, 0, 0);
        iq = DefWindowProcA((HWND)a1, 0x18, (WPARAM)a3, (LPARAM)a4);
        return iq;
    case 0xa1:
        v47c = (HWND)a3;
        if (a3 == 8) {
            SendMessageA((HWND)a1, 0x111, 0x65, 0);
            return 0;
        }
        iq = DefWindowProcA((HWND)a1, 0xa1, (WPARAM)a3, (LPARAM)a4);
        return iq;
    case 0x83:
        v454 = (HWND)a4;
        v45c = *(int *)a4;
        iq = DefWindowProcA((HWND)a1, a2, (WPARAM)a3, (LPARAM)a4);
        *(int *)v454 = v45c;
        return iq;
    case 0x84:
        v470 = DefWindowProcA((HWND)a1, a2, (WPARAM)a3, (LPARAM)a4);
        if (v470 != 2) {
            return v470;
        }
        v474 = GetSystemMetrics(0x1e);
        GetClientRect((HWND)a1, &v46c);
        MapWindowPoints((HWND)a1, (HWND)0, (LPPOINT)&v46c, 2);
        return 8;
    case 0x85:
    case 0x86:
        GetWindowRect((HWND)a1, &v494);
        OffsetRect(&v494, -v494.left, -v494.top);
        if (v494.right != v494.left && v494.bottom != v494.top) {
            v4bc = (unsigned int)(a2 != 0x85);
            v4f0 = GetWindowDC((HWND)a1);
            if (v4f0 == (HDC)0) {
                return v4bc;
            }
            Game_Fn46469a(v4f0);
            GetWindowRect((HWND)a1, &v494);
            GetClientRect((HWND)a1, &v56c);
            MapWindowPoints((HWND)a1, (HWND)0, (LPPOINT)&v56c, 2);
            OffsetRect(&v56c, -v494.left, -v494.top);
            OffsetRect(&v494, -v494.left, -v494.top);
            GetWindowTextA((HWND)a1, v558, 100);
            v574 = v494.right - v56c.right;
            v4c4 = v494.bottom - v56c.bottom;
            Game_Fn453d4d(&v484, 0);
            if (v484 == 0) {
                v498 = (HGDIOBJ)g_game5a9018;
                v570 = (HGDIOBJ)g_game5a9084;
                v480 = (HGDIOBJ)g_game5a8fe4;
                v4f4 = (HBRUSH)g_game5a905c;
            } else {
                v498 = (HGDIOBJ)g_game5a906c;
                v570 = (HGDIOBJ)g_game5a9070;
                v480 = (HGDIOBJ)g_game5a9010;
                v4f4 = (HBRUSH)g_game5a907c;
            }
            SelectObject(v4f0, v570);
            v4c8 = 0;
            MoveToEx(v4f0, 0, 0, (LPPOINT)0);
            LineTo(v4f0, v494.right + -1, v4c8);
            SelectObject(v4f0, v498);
            v4c8 = 1;
            for (v4cc = 1; v4cc <= v4c4 + -2; v4cc = v4cc + 1) {
                MoveToEx(v4f0, 0, v4c8, (LPPOINT)0);
                LineTo(v4f0, (v494.right - v574) + 1, v4c8);
                v4c8 = v4c8 + 1;
            }
            SelectObject(v4f0, v570);
            v4c8 = v4c4 + -1;
            MoveToEx(v4f0, 0, v4c8, (LPPOINT)0);
            LineTo(v4f0, v56c.right + 1, v4c8);
            ip = GetStockObject(7);
            SelectObject(v4f0, ip);
            v4c0 = v494.right + -1;
            MoveToEx(v4f0, v4c0, 0, (LPPOINT)0);
            LineTo(v4f0, v4c0, v494.bottom);
            SelectObject(v4f0, v480);
            v4c0 = v494.right + -2;
            for (v4cc = 1; v4cc <= v574 + -2; v4cc = v4cc + 1) {
                MoveToEx(v4f0, v4c0, 1, (LPPOINT)0);
                LineTo(v4f0, v4c0, v494.bottom + -1);
                v4c0 = v4c0 + -1;
            }
            SelectObject(v4f0, v570);
            v4c0 = v56c.right;
            MoveToEx(v4f0, v56c.right, v4c4 + -1, (LPPOINT)0);
            LineTo(v4f0, v4c0, v56c.bottom + 1);
            ip = GetStockObject(7);
            SelectObject(v4f0, ip);
            v4c8 = v494.bottom + -1;
            MoveToEx(v4f0, 0, v4c8, (LPPOINT)0);
            LineTo(v4f0, v494.right, v4c8);
            SelectObject(v4f0, v480);
            v4c8 = v494.bottom + -2;
            for (v4cc = 1; v4cc <= v4c4 + -2; v4cc = v4cc + 1) {
                MoveToEx(v4f0, 0, v4c8, (LPPOINT)0);
                LineTo(v4f0, v494.right + -1, v4c8);
                v4c8 = v4c8 + -1;
            }
            SelectObject(v4f0, v570);
            v4c8 = v494.bottom - v4c4;
            MoveToEx(v4f0, 0, v4c8, (LPPOINT)0);
            LineTo(v4f0, v494.right + -2, v4c8);
            SelectObject(v4f0, v570);
            v4c8 = v56c.top + -1;
            MoveToEx(v4f0, 0, v4c8, (LPPOINT)0);
            LineTo(v4f0, v56c.right + 1, v4c8);
            SetRect(&v4a8, 0, v4c4, v56c.right, v56c.top + -1);
            FillRect(v4f0, &v4a8, v4f4);
            SetBkMode(v4f0, 1);
            OffsetRect(&v4a8, 1, 1);
            SetTextColor(v4f0, g_game5a90ac);
            DrawTextA(v4f0, v558, -1, &v4a8, 0x24);
            OffsetRect(&v4a8, -1, -1);
            SetTextColor(v4f0, g_game5a9014);
            DrawTextA(v4f0, v558, -1, &v4a8, 0x24);
            v55c = LoadBitmapA((HINSTANCE)0, (LPCSTR)0x7fed);
            GetObjectA(v55c, 0x18, v4ec);
            v4d0 = v4ec[1];
            v4d4 = v4ec[2];
            SetRect(&v4b8, v56c.right - v4ec[1], v56c.top - v4ec[2], v56c.right, v56c.top);
            Game_Fn4649c0(v4f0, &v4b8, (void *)g_game5a9080);
            DeleteObject(v55c);
            ReleaseDC((HWND)a1, v4f0);
            return v4bc;
        }
        iq = DefWindowProcA((HWND)a1, a2, (WPARAM)a3, (LPARAM)a4);
        return iq;
    case 0x111:
        iq = (unsigned int)a3 & 0xffff;
        if (iq == 100) {
            v1c0 = 0xbbc;
            strcpy(v2c8, (char *)&g_game78e5f0);
            strcat(v2c8, (char *)0x585854);
            WinHelpA((HWND)g_game8e3304, v2c8, 1, v1c0);
        } else if (iq == 0x65) {
            ShowWindow((HWND)a1, 0);
            UpdateWindow((HWND)g_game8e31c8);
            GetWindowRect((HWND)g_game8bcfec, &v194);
            v198 = v194.left;
            v1a0 = v194.right - v194.left;
            if ((HANDLE)g_game5a9080 == (HANDLE)0) {
                v1bc = v1a0 * 2;
            } else {
                GetObjectA((HGDIOBJ)g_game5a9080, 0x18, v1b8);
                v1bc = (v1b8[2] * v1a0) / v1b8[1];
            }
            v19c = (v194.bottom - v194.top) / 2 + v1bc / 2 + 2;
            MoveWindow((HWND)g_game5a8fe8, v198, v19c, v1a0, v1bc, 1);
            ShowWindow((HWND)g_game5a8fe8, 5);
            BringWindowToTop((HWND)g_game5a8fe8);
            SendMessageA((HWND)g_game94e8d0, 0x403, 0, 0);
        } else if (iq == 0x66) {
            ShowWindow((HWND)g_game5a8fe8, 0);
            ShowWindow((HWND)a1, 5);
            SendMessageA((HWND)g_game94e8d0, 0x403, 0, 0);
            Game_Fn466a34();
        }
        return 0;
    case 0xa4:
    case 0x204:
        v594.x = (unsigned int)a4 & 0xffff;
        v594.y = (unsigned int)a4 >> 0x10;
        if (a2 == 0x204) {
            ClientToScreen((HWND)a1, &v594);
        }
        SetRect(&v58c, v594.x, v594.y, v594.x + 1, v594.y + 1);
        TrackPopupMenu((HMENU)g_game5a9060, 2, v594.x, v594.y, 0, (HWND)a1, &v58c);
        return 0;
    case 0x11f:
        if ((unsigned int)a3 >> 0x10 == 0xffff && a4 == 0) {
            v598 = GetMenuItemCount((HMENU)g_game5a9060);
            while (v598 != 0) {
                DeleteMenu((HMENU)g_game5a9060, 0, 0x400);
                v598 = v598 + -1;
            }
        }
        return 0;
    case 0x112:
        v57c = (unsigned int)a3 & 0xfff0;
        if (v57c == 0xf010) {
            return 0;
        }
        iq = DefWindowProcA((HWND)a1, 0x112, (WPARAM)a3, (LPARAM)a4);
        return iq;
    case 0x114:
        v420 = GetDlgItem((HWND)a1, 0);
        SendMessageA(v420, 0xe3, (WPARAM)&v448, (LPARAM)&v41c);
        v44c = SendMessageA(v420, 0xe1, 0, 0);
        v444 = g_game8bd390;
        GetClientRect((HWND)a1, &v430);
        v450 = v430.right;
        switch ((unsigned int)a3 & 0xffff) {
        case 0:
            v418 = v44c - v444;
            break;
        case 1:
            v418 = v44c + v444;
            break;
        case 2:
            v418 = v44c - v430.right;
            break;
        case 3:
            v418 = v44c + v430.right;
            break;
        case 4:
        case 5:
            v418 = (unsigned int)a3 >> 0x10;
            break;
        case 6:
            v418 = v448;
            break;
        case 7:
            v418 = v41c;
            break;
        default:
            v418 = v44c;
        }
        if ((int)v418 < (int)v448) {
            v418 = v448;
        }
        if ((int)v41c < (int)v418) {
            v418 = v41c;
        }
        if (v418 != v44c) {
            SendMessageA(v420, 0xe0, v418, 1);
            GetWindowRect(v420, &v440);
            MapWindowPoints((HWND)0, (HWND)a1, (LPPOINT)&v440, 2);
            ScrollWindow((HWND)a1, v44c - v418, 0, (RECT *)0, (RECT *)0);
            MoveWindow(v420, v440.left, v440.top, v440.right - v440.left,
                       v440.bottom - v440.top, 0);
            UpdateWindow((HWND)a1);
        }
        return 0;
    case 0x116:
        AppendMenuA((HMENU)g_game5a9060, 0, 0x65, (char *)&g_game5a8ff0);
        AppendMenuA((HMENU)g_game5a9060, 0, 100, (char *)&g_game5a9020);
        return 0;
    case 0x201:
        return 0;
    case 0x400:
    case 0x401:
        vc = (void *)GetWindowLongA((HWND)a1, g_game57f014);
        v8 = GetWindowLongA((HWND)a1, g_game57f018);
        v88 = (HWND)a3;
        if (a3 == 0) {
            return 0;
        }
        Game_Fn4624cf((void *)a1, (void *)a3, 0, &v8c, &v84);
        if (v8c == (HWND)0
            || ((v84 == 0 || a2 != 0x400) && (v84 != 0 || a2 != 0x401))) {
            v7c = 0;
        } else {
            v7c = 1;
        }
        if (v7c == 0) {
            v8c = CreateWindowExA(0, (char *)0x58581c, (char *)0x58580c, 0x54000000,
                                  0, 0, 0, 0, (HWND)a1, (HMENU)1,
                                  (HINSTANCE)g_game939160, (LPVOID)v88);
            if (v8c == (HWND)0) {
                return 0;
            }
            if (a2 == 0x400) {
                v80 = Game_Fn45225b(*(int *)v88, ((int *)v88)[1]);
                if (v80 == -1) {
                    v80 = ((int *)v88)[1];
                }
            } else {
                v80 = Game_Fn45225b(*(int *)v88, ((int *)v88)[1]);
            }
            v90 = 0;
            v7c = 0;
            while (v90 < v8 && v7c == 0) {
                if (*(int *)((int)vc + v90 * 0x19c) == v80) {
                    v7c = 1;
                    if (a2 == 0x400 && *(int *)((int)vc + 0xcc + v90 * 0x19c) < 0x32) {
                        *(HWND *)(v90 * 0x19c + *(int *)((int)vc + 0xcc + v90 * 0x19c) * 4 + 4
                                  + (int)vc) = v8c;
                        iv = (int *)((int)vc + 0xcc + v90 * 0x19c);
                        *iv = *iv + 1;
                    } else {
                        if (a2 != 0x401 || 0x31 < *(int *)((int)vc + 0x198 + v90 * 0x19c)) {
                            DestroyWindow(v8c);
                            return 0;
                        }
                        *(HWND *)(v90 * 0x19c + *(int *)((int)vc + 0x198 + v90 * 0x19c) * 4
                                  + 0xd0 + (int)vc) = v8c;
                        iv = (int *)((int)vc + 0x198 + v90 * 0x19c);
                        *iv = *iv + 1;
                    }
                }
                v90 = v90 + 1;
            }
            if (v7c == 0) {
                if (99 < v8) {
                    DestroyWindow(v8c);
                    return 0;
                }
                *(int *)((int)vc + v8 * 0x19c) = v80;
                if (a2 == 0x400) {
                    *(HWND *)((int)vc + 4 + v8 * 0x19c) = v8c;
                    *(int *)((int)vc + 0xcc + v8 * 0x19c) = 1;
                    *(int *)((int)vc + 0x198 + v8 * 0x19c) = 0;
                } else {
                    *(HWND *)((int)vc + 0xd0 + v8 * 0x19c) = v8c;
                    *(int *)((int)vc + 0x198 + v8 * 0x19c) = 1;
                    *(int *)((int)vc + 0xcc + v8 * 0x19c) = 0;
                }
                v8 = v8 + 1;
                SetWindowLongA((HWND)a1, g_game57f018, v8);
            }
            BringWindowToTop(v8c);
        }
        return 1;
    case 0x30f:
    case 0x310:
    case 0x311:
        iq = Game_Fn466d5f((void *)a1, a2, a3, a4);
        return iq;
    case 0x402:
    case 0x403:
        vc = (void *)GetWindowLongA((HWND)a1, g_game57f014);
        v8 = GetWindowLongA((HWND)a1, g_game57f018);
        vfc = (HWND)a3;
        if (a3 == 0) {
            ve8 = (HWND)a4;
        } else {
            Game_Fn4624cf((void *)a1, (void *)a3, 0, &ve8, 0);
        }
        if (ve8 != (HWND)0) {
            ik = GetParent(ve8);
            if (ik == (HWND)a1) {
                for (vec = 0; vec < v8; vec = vec + 1) {
                    ve4 = 0;
                    vf0 = 0;
                    while (vf0 < *(int *)((int)vc + 0xcc + vec * 0x19c) && ve4 == 0) {
                        if (a2 == 0x403
                            && *(HWND *)(vf0 * 4 + vec * 0x19c + 4 + (int)vc) == ve8) {
                            ve4 = 1;
                            Game_Fn4607dd(ve8, (void *)(vec * 0x19c + (int)vc + 4),
                                          *(int *)((int)vc + 0xcc + vec * 0x19c));
                            vf8 = 0;
                            for (vf4 = 0; vf4 < *(int *)((int)vc + 0xcc + vec * 0x19c); vf4 = vf4 + 1) {
                                if (*(int *)(vf4 * 4 + vec * 0x19c + 4 + (int)vc) != 0) {
                                    *(int *)(vf8 * 4 + vec * 0x19c + 4 + (int)vc) =
                                        *(int *)(vf4 * 4 + vec * 0x19c + 4 + (int)vc);
                                    vf8 = vf8 + 1;
                                }
                            }
                            *(int *)((int)vc + 0xcc + vec * 0x19c) = vf8;
                        } else if (a2 == 0x402
                                   && *(HWND *)(vf0 * 4 + vec * 0x19c + 0xd0 + (int)vc) == ve8) {
                            ve4 = 1;
                            Game_Fn4607dd(ve8, (void *)(vec * 0x19c + (int)vc + 0xd0),
                                          *(int *)((int)vc + 0x198 + vec * 0x19c));
                            vf8 = 0;
                            for (vf4 = 0; vf4 < *(int *)((int)vc + 0x198 + vec * 0x19c); vf4 = vf4 + 1) {
                                if (*(int *)(vf4 * 4 + vec * 0x19c + 0xd0 + (int)vc) != 0) {
                                    *(int *)(vf8 * 4 + vec * 0x19c + 0xd0 + (int)vc) =
                                        *(int *)(vf4 * 4 + vec * 0x19c + 0xd0 + (int)vc);
                                    vf8 = vf8 + 1;
                                }
                            }
                            *(int *)((int)vc + 0x198 + vec * 0x19c) = vf8;
                        }
                        vf0 = vf0 + 1;
                    }
                }
                return ve4;
            }
        }
        return 0;
    case 0x404:
        vc = (void *)GetWindowLongA((HWND)a1, g_game57f014);
        v8 = GetWindowLongA((HWND)a1, g_game57f018);
        v134 = (HWND)a3;
        v130 = (HWND)a4;
        if (a4 != 0 && a3 != -1) {
            v140 = 0;
            for (v138 = 0; v138 < v8; v138 = v138 + 1) {
                if (*(HWND *)((int)vc + v138 * 0x19c) == v134) {
                    for (v13c = 0; v13c < *(int *)((int)vc + 0xcc + v138 * 0x19c); v13c = v13c + 1) {
                        ih = (int)Sid_GetWindowExtra8(*(void **)(v13c * 4 + v138 * 0x19c + 4 + (int)vc));
                        if (ih == 0) {
                            ((int *)v130)[v140] =
                                *(int *)(v13c * 4 + v138 * 0x19c + 4 + (int)vc);
                            v140 = v140 + 1;
                        }
                    }
                }
            }
            return v140;
        }
        return 0;
    case 0x405:
        vc = (void *)GetWindowLongA((HWND)a1, g_game57f014);
        v8 = GetWindowLongA((HWND)a1, g_game57f018);
        v124 = (HWND)a3;
        if (a3 == 0) {
            return -1;
        }
        ik = GetParent((HWND)a3);
        if (ik != (HWND)a1) {
            return -1;
        }
        ig = 0;
        for (v128 = 0; v128 < v8; v128 = v128 + 1) {
            v12c = 0;
            while (v12c < *(int *)((int)vc + 0xcc + v128 * 0x19c) && ig == 0) {
                if (*(HWND *)(v12c * 4 + v128 * 0x19c + 4 + (int)vc) == v124) {
                    ig = 1;
                    v120 = 1;
                }
                v12c = v12c + 1;
            }
            v12c = 0;
            while (v12c < *(int *)((int)vc + 0x198 + v128 * 0x19c) && ig == 0) {
                if (*(HWND *)(v12c * 4 + v128 * 0x19c + 0xd0 + (int)vc) == v124) {
                    ig = 1;
                    v120 = 0;
                }
                v12c = v12c + 1;
            }
        }
        if (ig != 0) {
            return v120;
        }
        return -1;
    case 0x406:
        vc = (void *)GetWindowLongA((HWND)a1, g_game57f014);
        v8 = GetWindowLongA((HWND)a1, g_game57f018);
        va0 = (HWND)a3;
        v94 = (HWND)a4;
        if (a3 == 0 || a4 == 0) {
            return 0;
        }
        ik = GetParent((HWND)a4);
        if (ik != (HWND)a1) {
            return 0;
        }
        ih = Game_Fn4624cf((void *)a1, va0, 0, 0, 0);
        if (ih != 0) {
            return 1;
        }
        va8 = 0;
        vac = 0;
        while (vac < v8 && va8 == 0) {
            vb0 = 0;
            while (vb0 < *(int *)((int)vc + 0xcc + vac * 0x19c) && va8 == 0) {
                if (*(HWND *)(vac * 0x19c + vb0 * 4 + 4 + (int)vc) == v94) {
                    va8 = 1;
                    v98 = vac;
                    v9c = 1;
                }
                vb0 = vb0 + 1;
            }
            vb0 = 0;
            while (vb0 < *(int *)((int)vc + 0x198 + vac * 0x19c) && va8 == 0) {
                if (*(HWND *)(vac * 0x19c + vb0 * 4 + 0xd0 + (int)vc) == v94) {
                    va8 = 1;
                    v98 = vac;
                    v9c = 0;
                }
                vb0 = vb0 + 1;
            }
            vac = vac + 1;
        }
        if (va8 == 0) {
            return 0;
        }
        va4 = CreateWindowExA(0, (char *)0x585840, (char *)0x585830, 0x54000000,
                              0, 0, 0, 0, (HWND)a1, (HMENU)1,
                              (HINSTANCE)g_game939160, (LPVOID)va0);
        if (va4 == (HWND)0) {
            return 0;
        }
        SendMessageA(va4, 0x402, (WPARAM)v94, 0);
        if (v9c == 0 || 0x31 < *(int *)((int)vc + 0xcc + v98 * 0x19c)) {
            if (v9c != 0 || 0x31 < *(int *)((int)vc + 0x198 + v98 * 0x19c)) {
                DestroyWindow(va4);
                return 0;
            }
            *(HWND *)(v98 * 0x19c + *(int *)((int)vc + 0x198 + v98 * 0x19c) * 4 + 0xd0
                      + (int)vc) = va4;
            iv = (int *)((int)vc + 0x198 + v98 * 0x19c);
            *iv = *iv + 1;
        } else {
            *(HWND *)(v98 * 0x19c + *(int *)((int)vc + 0xcc + v98 * 0x19c) * 4 + 4
                      + (int)vc) = va4;
            iv = (int *)((int)vc + 0xcc + v98 * 0x19c);
            *iv = *iv + 1;
        }
        return 1;
    case 0x40c:
        vc = (void *)GetWindowLongA((HWND)a1, g_game57f014);
        v8 = GetWindowLongA((HWND)a1, g_game57f018);
        ShowWindow((HWND)a1, 0);
        ShowWindow((HWND)g_game5a8fe8, 0);
        for (v100 = 0; v100 < v8; v100 = v100 + 1) {
            for (v104 = 0; v104 < *(int *)((int)vc + 0xcc + v100 * 0x19c); v104 = v104 + 1) {
                DestroyWindow(*(HWND *)(v104 * 4 + v100 * 0x19c + 4 + (int)vc));
            }
            *(int *)((int)vc + 0xcc + v100 * 0x19c) = 0;
            for (v104 = 0; v104 < *(int *)((int)vc + 0x198 + v100 * 0x19c); v104 = v104 + 1) {
                DestroyWindow(*(HWND *)(v104 * 4 + v100 * 0x19c + 0xd0 + (int)vc));
            }
            *(int *)((int)vc + 0x198 + v100 * 0x19c) = 0;
        }
        v8 = 0;
        SetWindowLongA((HWND)a1, g_game57f018, 0);
        ix = 1;
        it = 0;
        ir = 0xe0;
        ik = GetDlgItem((HWND)a1, 0);
        SendMessageA(ik, ir, it, ix);
        return 0;
    case 0x40e:
    case 0x40f:
        vc = (void *)GetWindowLongA((HWND)a1, g_game57f014);
        v8 = GetWindowLongA((HWND)a1, g_game57f018);
        v110 = (HWND)a3;
        if (a3 == 0) {
            v108 = 0;
        } else {
            v108 = 0;
            for (v114 = 0; v114 < v8; v114 = v114 + 1) {
                v118 = 0;
                while (v118 < *(int *)((int)vc + 0xcc + v114 * 0x19c) && v108 == 0) {
                    ih = Sid_WindowMatchesKey(*(void **)(v118 * 4 + v114 * 0x19c + 4 + (int)vc),
                                              (const int *)v110);
                    if (ih != 0) {
                        v108 = 1;
                        if (a2 == 0x40e) {
                            v10c = Sid_GetWindowId(*(void **)(v118 * 4 + v114 * 0x19c + 4 + (int)vc));
                        } else {
                            v10c = *(unsigned int *)(v118 * 4 + v114 * 0x19c + 4 + (int)vc);
                        }
                    }
                    v118 = v118 + 1;
                }
                v118 = 0;
                while (v118 < *(int *)((int)vc + 0x198 + v114 * 0x19c) && v108 == 0) {
                    ih = Sid_WindowMatchesKey(*(void **)(v118 * 4 + v114 * 0x19c + 0xd0 + (int)vc),
                                              (const int *)v110);
                    if (ih != 0) {
                        v108 = 1;
                        if (a2 == 0x40e) {
                            v10c = Sid_GetWindowId(*(void **)(v118 * 4 + v114 * 0x19c + 0xd0 + (int)vc));
                        } else {
                            v10c = *(unsigned int *)(v118 * 4 + v114 * 0x19c + 0xd0 + (int)vc);
                        }
                    }
                    v118 = v118 + 1;
                }
            }
        }
        if (v108 != 0) {
            return v10c;
        }
        if (a2 == 0x40e) {
            return -1;
        }
        return 0;
    case 0x410:
        vc = (void *)GetWindowLongA((HWND)a1, g_game57f014);
        v8 = GetWindowLongA((HWND)a1, g_game57f018);
        vc8 = (HWND)a3;
        if (a3 == 0) {
            return 0;
        }
        vdc = 5;
        ve0 = g_game94d638;
        GetWindowRect((HWND)a3, &vc4);
        MapWindowPoints((HWND)0, (HWND)a1, (LPPOINT)&vc4, 2);
        vcc = vc4.left + vdc;
        vd0 = vc4.top - ve0;
        vb4 = vc8;
        for (vd4 = 0; vd4 < v8; vd4 = vd4 + 1) {
            for (vd8 = 0; vd8 < *(int *)((int)vc + 0xcc + vd4 * 0x19c); vd8 = vd8 + 1) {
                ik = (HWND)Sid_GetWindowExtra8(*(void **)(vd8 * 4 + vd4 * 0x19c + 4 + (int)vc));
                if (ik == vc8) {
                    SetWindowPos(*(HWND *)(vd8 * 4 + vd4 * 0x19c + 4 + (int)vc), vb4,
                                 vcc, vd0, g_game8bd390, g_game8e3300[0][0], 0);
                    vd0 = vd0 - ve0;
                    vb4 = *(HWND *)(vd8 * 4 + vd4 * 0x19c + 4 + (int)vc);
                }
            }
            for (vd8 = 0; vd8 < *(int *)((int)vc + 0x198 + vd4 * 0x19c); vd8 = vd8 + 1) {
                ik = (HWND)Sid_GetWindowExtra8(*(void **)(vd8 * 4 + vd4 * 0x19c + 0xd0 + (int)vc));
                if (ik == vc8) {
                    SetWindowPos(*(HWND *)(vd8 * 4 + vd4 * 0x19c + 0xd0 + (int)vc), vb4,
                                 vcc, vd0, g_game8bd390, g_game8e3300[0][0], 0);
                    vd0 = vd0 - ve0;
                    vb4 = *(HWND *)(vd8 * 4 + vd4 * 0x19c + 0xd0 + (int)vc);
                }
            }
        }
        return 0;
    case 0x411:
        vc = (void *)GetWindowLongA((HWND)a1, g_game57f014);
        ii = GetWindowLongA((HWND)a1, g_game57f018);
        v78 = 0;
        for (v74 = 0; v74 < ii; v74 = v74 + 1) {
            if (*(int *)((int)vc + 0xcc + v74 * 0x19c) != 0) {
                v78 = v78 + 1;
            }
        }
        return v78;
    case 0x412:
        v164 = (HWND)a3;
        v154 = 0;
        for (v14c = 0; v14c < 2; v14c = v14c + 1) {
            for (v16c = 0; v16c < 0x96; v16c = v16c + 1) {
                v174 = v14c;
                v170 = v16c;
                v158 = Game_Helper452355(v14c, v16c);
                v15c = Game_Fn452442(v14c, v16c);
                v150 = Game_Fn453419(v14c, v16c);
                Game_Fn4624cf((void *)a1, &v174, 0, &v178, &v160);
                ih = Game_Fn50775c((void *)g_game8c7424, &v174, 0, &v144);
                if (ih == 0) {
                    Game_Fn50775c((void *)g_game8e31c8, &v174, 0, &v144);
                }
                if (g_game92e95c != v158 && v15c != 2) {
                    if (v158 == -1 || v15c != 1
                        || ((v150 & 0x10000) != 0 && g_game930acc == 0)) {
                        if (v178 != 0) {
                            if (v160 == 0) {
                                iq = SendMessageA((HWND)a1, 0x402, 0, v178);
                                v154 = v154 | iq;
                            } else {
                                iq = SendMessageA((HWND)a1, 0x403, 0, v178);
                                v154 = v154 | iq;
                            }
                        }
                        ix = 1;
                        ir = 0x402;
                        ik = v144;
                        ij = GetParent(v144);
                        SendMessageA(ij, ir, (WPARAM)ik, ix);
                    } else {
                        v148 = Game_Fn452516(v14c, v16c);
                        v168 = Game_Fn452cce(v14c, v16c);
                        if ((v148 & 0x10) == 0) {
                            if (((v148 & 0x8) == 0 || (v168 & 0x4) != 0 || (v168 & 0x40) != 0)
                                && (v168 & 0x8) == 0) {
                                if (v178 != 0 && v160 == 0) {
                                    iq = SendMessageA((HWND)a1, 0x402, 0, v178);
                                    v154 = v154 | iq;
                                    ix = 1;
                                    ir = 0x402;
                                    ik = v144;
                                    ij = GetParent(v144);
                                    SendMessageA(ij, ir, (WPARAM)ik, ix);
                                }
                            } else if (v178 == 0) {
                                SendMessageA((HWND)a1, 0x401, (WPARAM)&v174, 0);
                                ik = v144;
                                ij = GetParent(v144);
                                Game_Fn50755e(ij, ik);
                                v154 = 1;
                                ix = 0;
                                ir = 0x402;
                                ik = v144;
                                ij = GetParent(v144);
                                SendMessageA(ij, ir, (WPARAM)ik, ix);
                            }
                            if ((v148 & 0x4) == 0 && (v168 & 0x4) == 0 && (v168 & 0x40) == 0) {
                                if (v178 != 0 && v160 != 0) {
                                    iq = SendMessageA((HWND)a1, 0x403, 0, v178);
                                    v154 = v154 | iq;
                                    ix = 1;
                                    ir = 0x402;
                                    ik = v144;
                                    ij = GetParent(v144);
                                    SendMessageA(ij, ir, (WPARAM)ik, ix);
                                }
                            } else if (v178 == 0) {
                                SendMessageA((HWND)a1, 0x400, (WPARAM)&v174, 0);
                                ik = v144;
                                ij = GetParent(v144);
                                Game_Fn50755e(ij, ik);
                                v154 = 1;
                                ix = 0;
                                ir = 0x402;
                                ik = v144;
                                ij = GetParent(v144);
                                SendMessageA(ij, ir, (WPARAM)ik, ix);
                            }
                        } else {
                            ih = Game_Fn4624cf((void *)a1, &v174, 0, 0, 0);
                            if (ih == 0) {
                                Game_Fn452743(v180, v14c, v16c);
                                ih = Game_Fn4624cf((void *)a1, v180, 0, &v184, 0);
                                if (ih != 0) {
                                    SendMessageA((HWND)a1, 0x406, (WPARAM)&v174, v184);
                                    v154 = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
        if (v154 != 0 || v164 != (HWND)0) {
            Game_Fn4608d2((void *)a1);
        }
        return 0;
    case 0x432:
        vc = (void *)GetWindowLongA((HWND)a1, g_game57f014);
        v8 = GetWindowLongA((HWND)a1, g_game57f018);
        for (v50 = 0; v50 < v8; v50 = v50 + 1) {
            for (v54 = 0; v54 < *(int *)((int)vc + 0xcc + v50 * 0x19c); v54 = v54 + 1) {
                SendMessageA(*(HWND *)(v54 * 4 + v50 * 0x19c + 4 + (int)vc), 0x432, 0, 0);
            }
            for (v54 = 0; v54 < *(int *)((int)vc + 0x198 + v50 * 0x19c); v54 = v54 + 1) {
                SendMessageA(*(HWND *)(v54 * 4 + v50 * 0x19c + 0xd0 + (int)vc), 0x432, 0, 0);
            }
        }
        return 0;
    case 0x433:
    case 0x434:
        vc = (void *)GetWindowLongA((HWND)a1, g_game57f014);
        v8 = GetWindowLongA((HWND)a1, g_game57f018);
        v4c = (HWND)a3;
        for (v44 = 0; v44 < v8; v44 = v44 + 1) {
            for (v48 = 0; v48 < *(int *)((int)vc + 0xcc + v44 * 0x19c); v48 = v48 + 1) {
                ih = Sid_WindowHasId(*(void **)(v48 * 4 + v44 * 0x19c + 4 + (int)vc), (int)v4c);
                if (ih != 0) {
                    InvalidateRect(*(HWND *)(v48 * 4 + v44 * 0x19c + 4 + (int)vc),
                                   (RECT *)0, 0);
                }
            }
            for (v48 = 0; v48 < *(int *)((int)vc + 0x198 + v44 * 0x19c); v48 = v48 + 1) {
                ih = Sid_WindowHasId(*(void **)(v48 * 4 + v44 * 0x19c + 0xd0 + (int)vc), (int)v4c);
                if (ih != 0) {
                    InvalidateRect(*(HWND *)(v48 * 4 + v44 * 0x19c + 0xd0 + (int)vc),
                                   (RECT *)0, 0);
                }
            }
        }
        return 0;
    case 0x435:
        vc = (void *)GetWindowLongA((HWND)a1, g_game57f014);
        v8 = GetWindowLongA((HWND)a1, g_game57f018);
        for (v58 = 0; v58 < v8; v58 = v58 + 1) {
            for (v5c = 0; v5c < *(int *)((int)vc + 0xcc + v58 * 0x19c); v5c = v5c + 1) {
                InvalidateRect(*(HWND *)(v5c * 4 + v58 * 0x19c + 4 + (int)vc), (RECT *)0, 0);
            }
            for (v5c = 0; v5c < *(int *)((int)vc + 0x198 + v58 * 0x19c); v5c = v5c + 1) {
                InvalidateRect(*(HWND *)(v5c * 4 + v58 * 0x19c + 0xd0 + (int)vc), (RECT *)0, 0);
            }
        }
        return 0;
    case 0x436:
        vc = (void *)GetWindowLongA((HWND)a1, g_game57f014);
        v8 = GetWindowLongA((HWND)a1, g_game57f018);
        v6c = (HWND)a3;
        v70 = (HWND)a4;
        if (a3 == 0) {
            return 0;
        }
        v60 = 0;
        for (v64 = 0; v64 < v8; v64 = v64 + 1) {
            for (v68 = 0; v68 < *(int *)((int)vc + 0xcc + v64 * 0x19c); v68 = v68 + 1) {
                ih = Sid_WindowMatchesKey(*(void **)(v68 * 4 + v64 * 0x19c + 4 + (int)vc),
                                          (const int *)v6c);
                if (ih != 0) {
                    v60 = 1;
                    if (v70 == (HWND)0) {
                        InvalidateRect(*(HWND *)(v68 * 4 + v64 * 0x19c + 4 + (int)vc),
                                       (RECT *)0, 0);
                    } else {
                        SendMessageA(*(HWND *)(v68 * 4 + v64 * 0x19c + 4 + (int)vc), 0x432, 0, 0);
                    }
                }
            }
            for (v68 = 0; v68 < *(int *)((int)vc + 0x198 + v64 * 0x19c); v68 = v68 + 1) {
                ih = Sid_WindowMatchesKey(*(void **)(v68 * 4 + v64 * 0x19c + 0xd0 + (int)vc),
                                          (const int *)v6c);
                if (ih != 0) {
                    v60 = 1;
                    if (v70 == (HWND)0) {
                        InvalidateRect(*(HWND *)(v68 * 4 + v64 * 0x19c + 0xd0 + (int)vc),
                                       (RECT *)0, 0);
                    } else {
                        SendMessageA(*(HWND *)(v68 * 4 + v64 * 0x19c + 0xd0 + (int)vc), 0x432, 0, 0);
                    }
                }
            }
        }
        return 0;
    case 0x437:
        return 0;
    }
    iq = DefWindowProcA((HWND)a1, a2, (WPARAM)a3, (LPARAM)a4);
    return iq;
}
