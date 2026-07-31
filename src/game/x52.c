/* Sealed-deck / ladder tournament screens and the random-encounter handler,
 * 0x0052xxxx. Newly derived from Ghidra's decompilation
 * (reports/ghidra/decomp/*.c); nothing here comes from the archive.
 *
 * Two of the four are Win32 dialog procedures. Both were checked against the
 * bytes and both end `pop edi/esi/ebx; leave; ret 0x10`, so both are __stdcall
 * with four arguments -- i.e. real DLGPROCs, not the __cdecl Ghidra assumed:
 *     0x005295f8  ... 5f 5e 5b c9 c2 10 00     (tail of 0x00527d6b)
 *     0x0052cef5  ... 5f 5e 5b c9 c2 10 00     (tail of 0x0052a3d9)
 * The other two end in a bare `ret` (0xc3) and are __cdecl.
 *
 * Both dialog procedures are written as `switch (msg)`. Ghidra renders them as
 * a nest of `if (msg < K)` tests, but that nest IS the compiler's binary
 * decision tree: the disassembly compares a stack copy of the message against
 * midpoints (e.g. at 0x0052ce9b, `cmp [ebp-0x418], 0x311 / ja` then
 * `cmp ..., 0x30f / jae`). A hand-written if/else-if chain would compile to a
 * linear sequence of compares instead, so `switch` is the shape to use. Case
 * ORDER within the switch is not established -- ascending here.
 *
 * Address literals (0x58e8b0, 0x58ea28, ...) are pointers to string data in
 * the original image. Pushing the literal reproduces the original instruction
 * without carrying the data, as in cards43.c.
 *
 * Two functions from this bucket are deliberately NOT here; see the note at
 * the bottom of the file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <windows.h>

#include "game/gamefns.h"

/* ------------------------------------------------------------------------
 * Referenced by this TU only; not in the shared headers. Addresses from the
 * disassembly. Scalars are declared `int` and byte-addressed buffers as
 * `char[]`, matching how each is indexed at its use sites; array extents are
 * unknown and only need to be large enough to index.
 * ------------------------------------------------------------------------ */

/* Named in Ghidra's symbol table, kept as-is. */
extern int Gold;

/* Ladder/tournament state block. Installed from the dialog's lParam at
   WM_INITDIALOG and indexed as ints; records are 0x4aa ints (0x12a8 bytes),
   the same stride the pack setup uses. */
extern int *g_game60477c;

extern char *g_game951964;
extern char *g_game77e180;
extern char *g_game77e184;
extern char *g_game77e188;
extern char *g_game77e18c;
extern char *g_game77e190;
extern char *g_game77e194;
extern char *g_game77e198;
extern char *g_game77e19c;
extern char *g_game77e1a0[];
extern char *g_game77e1b8;
extern char *g_game77e1bc;
extern char *g_game77e1c0;
extern char *g_game77e1c8;

extern char g_game591a08[];
extern char g_game591a12[];
extern char g_game591a44[];
extern char g_game591a76[];
extern char g_game591a77[];
extern char g_game591a7c[];
extern char g_game604640[];
extern char g_game604758[];
extern char g_game6045c0[];
extern char g_game6046f0[];
extern char g_game6047b0[];
extern char g_game604850[];
extern char g_game604918[];
extern char g_game604a44[];
extern char g_game604b58[];
extern char g_game60518c[];
extern char g_game605af4[];
extern char g_game60563c[];
extern char g_game64f6c0[];
extern char g_game64fae0[];
extern char g_game64fbe0[];
extern char g_game64fc18[];
extern char g_game64fc88[];
extern char g_game64fe70[];
extern char g_game64fed8[];
extern char g_game64ff40[];
extern char g_game746b20[];
extern char g_game77d090[];
extern char g_game78cf10[];
extern char g_game78e5f0[];
extern char g_game79ff90[];
extern char g_game7a01f0[];
extern char g_game7a0770[];
extern char g_game7bfef0[];
extern char g_game7bffb8[];
extern char g_game7c00e4[];
extern char g_game7c01f0[];
extern char g_game8aa4f0[];
extern char g_game8aa920[];
extern char g_game8aaa4c[];
extern char g_game8aab78[];
extern char g_game8aaca4[];
extern char g_game8aadd0[];
extern char g_game8aaefc[];
extern char g_game8ab028[];
extern char g_game8ab154[];
extern char g_game8ab280[];
extern char g_game8bd3a0[];
extern char g_game8bd5b3[];

extern HANDLE g_game64f6a8;
extern HANDLE g_game64f6b4;
extern HANDLE g_game650034;
extern HANDLE g_game650044;
extern HANDLE g_game6046dc;
extern HANDLE g_game6046e0;
extern HANDLE g_game604638;
extern HANDLE g_game604848;
extern HANDLE g_game650030;
extern HANDLE g_game64fc7c;
extern HDC    g_game604774;
extern HDC    g_game64fb44;
extern HWND   g_game8a92b0;

extern int g_game58e1a4;
extern int g_game58e1a8;
extern int g_game58edb8[];
extern int g_game5863b4;
extern int g_game59120c;
extern int g_game593934;
extern int g_game604628;
extern int g_game60462c;
extern int g_game604634;
extern int g_game6045b8;
extern int g_game6046e4;
extern int g_game6046e8;
extern int g_game604754;
extern int g_game604778;
extern int g_game604780;
extern int g_game604784;
extern int g_game604788;
extern int g_game604798;
extern int g_game60479c;
extern int g_game6047a0;
extern int g_game6047a4;
extern int g_game604b4c;
extern int g_game604b50;
extern int g_game604b54;
extern int g_game605188[];
extern int g_game64f250;
extern int g_game64f254;
extern int g_game64f258;
extern int g_game64f25c;
extern int g_game64f260[];
extern int g_game64f268;
extern int g_game64f26c[];
extern int g_game64f27c;
extern int g_game64f280;
extern int g_game64f284;
extern int g_game64f288;
extern int g_game64f28c[];
extern int g_game64f688;
extern int g_game64f68c;
extern int g_game64f6a4;
extern int g_game64f6ac;
extern int g_game64f6b0;
extern int g_game64f6b8;
extern int g_game64fad0;
extern int g_game64fb4c;
extern int g_game64fb50;
extern int g_game64fb54;
extern int g_game64fb58;
extern int g_game64fb68;
extern int g_game64fb70;
extern int g_game64fb78;
extern int g_game64fb98;
extern int g_game64fba8;
extern int g_game64fbb0;
extern int g_game64fbc0;
extern int g_game64fbc8;
extern int g_game64fbdc;
extern int g_game64fc80;
extern int g_game64fc84;
extern int g_game64fe6c;
extern int g_game64ffa4;
extern int g_game64ffa8;
extern int g_game64ffac;
extern int g_game64ffb0;
extern int g_game64ffb4;
extern int g_game64ffb8;
extern int g_game64ffc0;
extern int g_game64ffc4;
extern int g_game650018;
extern int g_game65001c;
extern int g_game650020;
extern int g_game650028;
extern int g_game65002c;
extern int g_game650038;
extern int g_game65003c;
extern int g_game650040;
extern int g_game78990c[];
extern int g_game789910[];
extern int g_game78cf00;
extern int g_game7bf604;
extern int g_game7bf608;
extern int g_game7bf60c;
extern int g_game7bf610;
extern int g_game7bf614;
extern int g_game8c6af4;
extern int g_game8c7d50;
extern int g_game93a470;
extern int g_game93a474[];
extern int g_game93a47c;
extern int g_game93a480[];
extern int g_game93a490;
extern int g_game93a494;
extern int g_game93a498;
extern int g_game93a49c;
extern int g_game93a4a0;
extern int g_game93a7e0;

/* Functions this TU calls that the shared headers do not declare. Argument
   counts are from the call sites' `add esp, N`. */
int Game_Helper56bfa4(int a1);
int Game_Fn428b05(int a1);
int Game_Fn464d0b(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9);
int Game_Fn46469a(HDC hdc);
int Game_Fn466737(HWND hwnd);
int Game_Fn466d5f(HWND a1, unsigned int a2, int a3, int a4);
int Game_Fn4a5d72(void);
int Game_Fn4bb1cf(int a1);
int Game_Fn4ed7c8(int a1);
int Game_Fn4f2232(void);
int Game_Fn4f8613(int a1);
int Game_Fn508970(int a1);
int Game_Fn527664(int a1, int a2, int a3, int a4, int a5, int a6, int a7);
int Game_Fn527b73(HWND a1, int *a2);
int Game_Fn527d34(HWND a1, int a2, int a3);
int Game_Fn529608(int a1, int a2, int a3, int a4, int a5, int a6);
int Game_Fn529686(int a1, int a2, int a3, int a4, int a5);
int Game_Fn529882(int a1, int a2, int a3);
int Game_Fn529932(int a1, int a2, int a3, int a4);
int Game_Fn529a88(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12, int a13);
int Game_Fn529ce8(int a1, int a2, int a3, int a4, int a5);
int Game_Fn529d5b(int a1, int a2, int a3, int a4);
int Game_Fn52a070(int a1, int a2, int a3, int a4, int a5, int a6, int a7);
int Game_Fn52a1ed(int *a1, HWND a2);
int Game_Fn52a3b0(HWND a1, int a2);
int Game_Fn52cefc(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12);
int Game_Fn52d15f(int a1, int a2, int a3, int a4, int a5, int a6);
int Game_Fn52d1ea(HWND a1);
int Game_Fn52d559(int a1, int a2, int a3);
int Game_Fn52d5db(int a1, int a2, int a3, int a4, int a5, int a6, int a7);
int Game_Fn52dcf5(int a1, int a2, int a3, int a4, int a5, int a6);
int Game_Fn52df3a(HWND a1, int a2);
int Game_Fn52e95f(HWND a1, int *a2, int a3);
int Game_Fn52f782(void);
int Game_Fn530770(void);
int Game_Fn5307dd(int a1, int a2);
int Game_Fn530b90(int a1, int a2);
int Game_Fn53114a(int a1);
int Game_Fn534530(void);
int Game_Fn56bcf7(int a1, int a2);
int Game_Fn56bd9d(int a1);
int Game_Fn56c0e5(int a1, int a2, int a3);
int Game_Fn56c5ea(int a1);
int Game_Fn56cf20(int a1, int a2, int a3);
int Game_Fn56d081(void);

/* The two worker threads this file starts. */
unsigned int __stdcall Game_Fn52783b(void *arg);
unsigned int __stdcall Game_Fn52eba4(void *arg);


/* Builds the opponent roster for a sealed-deck event and starts the deck
 * builder thread; a1 == 0 is a fresh event, otherwise the saved block is
 * restored. The returned local is never assigned -- the original picked up
 * whatever was in that stack slot.
 *
 * Local layout is the one the bytes show, not Ghidra's: the frame is
 * [ebp-4] .. [ebp-0x18], six slots, and Ghidra's names for this function are
 * uniformly four bytes off (its `local_8` is [ebp-4]) and omit [ebp-0xc]
 * entirely. Declaration order below puts the thread-id out-parameter at
 * [ebp-4], which is what the disassembly requires. */
// FUNCTION: SHANDALAR 0x0052704a
int Game_Fn52704a(int a1)
{
    unsigned int v4;
    int v8;
    int vc;
    int v10;
    int v14;
    int v18;

    g_game8c6af4 = 10;
    if (a1 == 0) {
        g_game951964 = g_game604850;
        g_game64f250 = 1 << (g_game93a470 & 0x1f);
        g_game64f254 = g_game93a470;
        g_game64f258 = g_game93a498;
        g_game64f25c = g_game93a49c;
        g_game64f268 = g_game93a47c;
        for (v8 = 0; v8 < g_game93a47c; v8++) {
            g_game64f260[v8] = g_game93a474[v8];
        }
        g_game64f27c = g_game93a490;
        for (v8 = 0; v8 < g_game93a490; v8++) {
            g_game64f26c[v8] = g_game93a480[v8];
        }
        g_game64f280 = g_game93a494;
        g_game64f284 = g_game93a4a0;
        Game_Fn52f782();
        for (v8 = 0; v8 < g_game64f250; v8++) {
            vc = v8 * 0x12a8;
            if (v8 == 0) {
                Game_Fn56c990();
                strcpy(g_game604850, g_game7a0770);
                strcpy(g_game604918, g_game8bd5b3);
                sprintf(g_game604a44, (const char *)0x58e8b0, g_game78e5f0, g_game8bd3a0);
                g_game604b4c = 0;
            } else {
                v14 = rand();
                while (v14 = v14 % g_game8c7d50, g_game7bfef0[v14 * 0x12a8] == '\0') {
                    v14++;
                }
                strcpy(g_game604850 + vc, g_game7bfef0 + v14 * 0x12a8);
                strcpy(g_game604918 + vc, g_game7bffb8 + v14 * 0x12a8);
                sprintf(g_game604a44 + vc, (const char *)0x58e8c4, g_game79ff90, g_game7c00e4 + v14 * 0x12a8);
                *(int *)((char *)&g_game604b4c + vc) = 0;
                *(int *)((char *)&g_game604b50 + vc) = *(int *)(g_game7c01f0 + v14 * 0x12a8);
                *(int *)((char *)&g_game604b54 + vc) = 0;
                strcpy(g_game7bfef0 + v14 * 0x12a8, (char *)0x58e8d4);
            }
            g_game605188[v8 * 0x4aa] = g_game64f268 + g_game64f27c;
            Game_Fn527664((int)(g_game604b58 + vc), (int)(g_game60518c + vc), (int)(g_game60563c + vc),
                          g_game64f268, (int)g_game64f260, g_game64f27c, (int)g_game64f26c);
        }
        g_game64f688 = 0;
        InitializeCriticalSection((LPCRITICAL_SECTION)&g_game64f68c);
        EnterCriticalSection((LPCRITICAL_SECTION)&g_game64f68c);
        for (v8 = 0; v8 < 0xff; v8++) {
            g_game64f28c[v8] = -1;
        }
        for (v8 = 0; v8 < g_game64f250; v8++) {
            v18 = rand();
            while (v18 = v18 % g_game64f250, g_game64f28c[v18] != -1) {
                v18++;
            }
            g_game64f28c[v18] = v8;
        }
        LeaveCriticalSection((LPCRITICAL_SECTION)&g_game64f68c);
        g_game64f288 = 1;
        g_game64f6ac = 0;
        g_game64f6b0 = 0;
        for (v8 = 0; v8 < g_game64f250; v8++) {
            *(int *)(g_game605af4 + v8 * 0x12a8) = 0;
        }
        g_game64f6a8 = (HANDLE)_beginthreadex((void *)0, 0, Game_Fn52783b, g_game604850, 0, &v4);
        SetThreadPriority(g_game64f6a8, -1);
        Game_Fn527d34(g_game8a92b0, (int)g_game604b58, g_game605188[0]);
    } else {
        memcpy(g_game604850, g_game951964, 0x4ae64);
        g_game64f6a8 = (HANDLE)0;
        g_game64f6a4 = 0;
        for (v8 = 0; v8 < g_game64f250; v8++) {
            *(int *)((char *)&g_game604b4c + v8 * 0x12a8) = 0;
        }
        InitializeCriticalSection((LPCRITICAL_SECTION)&g_game64f68c);
        g_game951964 = g_game604850;
        if (g_game64f6ac == 0 && g_game64f6b0 == 0) {
            g_game64f6a8 = (HANDLE)_beginthreadex((void *)0, 0, Game_Fn52783b, g_game604850, 0, &v4);
            SetThreadPriority(g_game64f6a8, -1);
        }
    }
    Game_Fn52a3b0(g_game8a92b0, (int)g_game604850);
    for (v8 = 0; v8 < g_game64f250; v8++) {
        vc = v8 * 0x12a8;
        if (*(int *)((char *)&g_game604b4c + vc) != 0) {
            Game_Helper46537b(*(int *)((char *)&g_game604b4c + vc));
            *(int *)((char *)&g_game604b4c + vc) = 0;
        }
    }
    DeleteCriticalSection((LPCRITICAL_SECTION)&g_game64f68c);
    return v10;
}


/* Foil-pack screen dialog procedure. Local names carry Ghidra's frame offsets
 * (its frame base is offset from the real one, so these are labels, not
 * measured slots). */
// FUNCTION: SHANDALAR 0x00527d6b
HGDIOBJ __stdcall Game_Fn527d6b(HWND a1, unsigned int a2, int a3, int a4)
{
    CHAR v4b4[100];
    RECT v450;
    LOGFONTA v440;
    HDC v404;
    int v400;
    HFONT v3fc;
    int v3f8;
    int v3f4;
    HGDIOBJ v3f0;
    RECT v3ec;
    SIZE v3dc;
    HANDLE v3d4;
    HDC v3d0;
    HDC v3cc;
    PAINTSTRUCT v3c8;
    int v388;
    RECT v384;
    RECT v374;
    int v364;
    int v360;
    char v35c[264];
    ULONG_PTR v254;
    POINT v250;
    HDC v248;
    RECT v244;
    int v234;
    int v230;
    int v22c;
    RECT v228;
    WPARAM v218;
    int v214;
    MSG v210;
    POINT v1f4;
    HDC v1ec;
    RECT v1e8;
    int v1d8;
    BOOL v1d4;
    HWND v1d0;
    HWND v1cc;
    int v1c8;
    HDC v1c4;
    HWND v1bc;
    HDC v1b8;
    HDC v1b4;
    RECT v1b0;
    unsigned int v1a0;
    RECT v19c;
    char v18c[264];
    int v84;
    int v80;
    int v7c;
    int v78;
    LOGFONTA v74;
    int v38;
    BITMAP v34;
    HANDLE v1c;
    HFONT v18;
    RECT v14;
    HWND vw;
    HBRUSH vb;
    HGDIOBJ vo;
    UINT vt;

    switch (a2) {
    case 2:
        strcpy(v18c, g_game78e5f0);
        strcat(v18c, (const char *)0x58e91c);
        WinHelpA(g_game8a92b0, v18c, 2, 0);
        DeleteDC(g_game604774);
        Game_Fn529ce8((int)g_game650034, g_game64f6b8, g_game604778, g_game64fbc0, g_game64ffb4);
        DeleteObject(g_game604638);
        DeleteObject(g_game6046dc);
        DeleteObject(g_game650030);
        return (HGDIOBJ)0;

    case 5:
        v3d4 = (HANDLE)SendDlgItemMessageA(a1, 1, 0x31, 0, 0);
        GetObjectA(v3d4, 0x3c, &v440);
        GetClientRect(a1, &v3ec);
        v440.lfHeight = (v3ec.bottom - v3ec.top) / 0x28;
        if (v440.lfHeight % 2 != 0) {
            v440.lfHeight = v440.lfHeight + 1;
        }
        v3fc = CreateFontIndirectA(&v440);
        SendDlgItemMessageA(a1, 1, 0x30, (WPARAM)v3fc, 0);
        DeleteObject(v3d4);
        v404 = GetDC(a1);
        v3f0 = (HGDIOBJ)SendDlgItemMessageA(a1, 1, 0x31, 0, 0);
        SelectObject(v404, v3f0);
        GetTextExtentPoint32A(v404, (LPCSTR)0x58e93c, 1, &v3dc);
        v3f4 = v3dc.cx;
        vw = GetDlgItem(a1, 1);
        GetWindowTextA(vw, v4b4, 100);
        v3f8 = v3f4 * 2;
        SetRect(&v450, 0, 0, v3f8, 10);
        v400 = DrawTextA(v404, v4b4, -1, &v450, 0x401);
        Game_Fn4ceb20((int)a1, (int)g_game650034, (int)&g_game604798, (int)&v3ec);
        SetWindowPos(vw, (HWND)0, v3ec.left + 5, (v3ec.bottom - 5) - v400, v3f8, v400, 4);
        ReleaseDC(a1, v404);
        InvalidateRect(a1, (RECT *)0, 1);
        GetWindowRect(a1, &v3ec);
        MoveWindow(g_game8a92b0, v3ec.left, v3ec.top, v3ec.right - v3ec.left,
                   v3ec.bottom - v3ec.top, 0);
        return (HGDIOBJ)1;

    case 0xf:
        if (g_game64ffa4 == -1) {
            v364 = 0;
        } else {
            v364 = g_game64ffa4 * 0x108 + g_game604754;
        }
        Game_Fn4a5d72();
        EnterCriticalSection((LPCRITICAL_SECTION)&g_game93a7e0);
        v3cc = (HDC)g_game7a00a0;
        v388 = SaveDC((HDC)g_game7a00a0);
        Game_Fn46469a(v3cc);
        GetClientRect(a1, &v374);
        GetUpdateRect(a1, &v384, 0);
        IntersectClipRect(v3cc, v384.left, v384.top, v384.right, v384.bottom);
        v3d0 = BeginPaint(a1, &v3c8);
        Game_Fn529608((int)v3cc, (int)&v374, (int)g_game604774, (int)g_game650034,
                      g_game64fc80, g_game6046e8);
        Game_Fn4ceb15((int)v3cc, (int)a1, (int)g_game650034);
        SetBkMode(v3cc, 1);
        SelectObject(v3cc, g_game604638);
        SetTextColor(v3cc, g_game64fb50);
        DrawTextA(v3cc, g_game64fc18, -1, (LPRECT)&g_game604798, 1);
        SelectObject(v3cc, g_game6046dc);
        SetTextColor(v3cc, g_game650038);
        DrawTextA(v3cc, g_game604640, -1, (LPRECT)&g_game64fad0, 0);
        DrawTextA(v3cc, g_game6047b0, -1, (LPRECT)&g_game64fb78, 0);
        SelectObject(v3cc, g_game650030);
        SetTextColor(v3cc, g_game650038);
        DrawTextA(v3cc, g_game64fe70, -1, (LPRECT)&g_game64fb98, 0x29);
        SetTextColor(v3cc, g_game64fb4c);
        DrawTextA(v3cc, g_game64fae0, -1, (LPRECT)&g_game64fb58, 0x29);
        DrawTextA(v3cc, g_game64fed8, -1, (LPRECT)&g_game64fbb0, 2);
        Game_Fn529686((int)v3cc, g_game64fe6c, (int)&g_game64ffc0, g_game64ffa4, g_game64f6b8);
        Game_Fn529882((int)v3cc, (int)&g_game604780, v364);
        Game_Fn529932((int)v3cc, v364, (int)&g_game604628, (int)&g_game604628);
        RestoreDC((HDC)g_game7a00a0, v388);
        v3cc = v3d0;
        Game_Fn46469a(v3d0);
        GetClientRect(a1, &v374);
        BitBlt(v3cc, v384.left, v384.top, v384.right - v384.left, v384.bottom - v384.top,
               (HDC)g_game7a00a0, v384.left, v384.top, 0xcc0020);
        EndPaint(a1, &v3c8);
        LeaveCriticalSection((LPCRITICAL_SECTION)&g_game93a7e0);
        return (HGDIOBJ)1;

    case 0x14:
        return (HGDIOBJ)1;

    case 0x2b:
        v1d0 = (HWND)a4;
        if (((int *)a4)[1] == 1) {
            Game_Fn466078(a4, g_game604778, g_game64fbc0, g_game64ffb4, g_game64ffac, 0);
        } else {
            Game_Fn466049(a4, g_game604778, g_game64fbc0, g_game64ffb4, g_game64ffac, 0);
        }
        return (HGDIOBJ)1;

    case 0x53:
        if (g_game64ffa4 != -1
            && *(int *)(g_game604754 + 0xc + g_game64ffa4 * 0x108) != 0
            && *(int *)(g_game604754 + 0x104 + g_game64ffa4 * 0x108) != -1) {
            v360 = *(int *)(g_game604754 + 0x104 + g_game64ffa4 * 0x108);
            v254 = ((int *)0x8e44a0)[*(int *)(v360 * 4 + g_game64ffa4 * 0x108 + 0x10 + g_game604754) * 0x26];
            if (v254 != 0xffffffff) {
                strcpy(v35c, g_game78e5f0);
                strcat(v35c, (const char *)0x58e930);
                WinHelpA((HWND)g_game8e3304, v35c, 1, v254);
            }
        }
        return (HGDIOBJ)1;

    case 0x7e:
        Game_Fn529ce8((int)g_game650034, g_game64f6b8, g_game604778, g_game64fbc0, g_game64ffb4);
        Game_Fn529a88((int)&g_game650034, (int)&g_game64f6b8, (int)&g_game604798,
                      (int)&g_game64ffc0, (int)&g_game604780, (int)&g_game604628,
                      (int)&g_game64fb50, (int)&g_game650038, (int)&g_game64fb4c,
                      (int)&g_game604778, (int)&g_game64fbc0, (int)&g_game64ffb4,
                      (int)&g_game64ffac);
        return (HGDIOBJ)0;

    case 0x110:
        v38 = 1;
        SetWindowTextA(a1, g_game7a01f0);
        Game_Fn529a88((int)&g_game650034, (int)&g_game64f6b8, (int)&g_game604798,
                      (int)&g_game64ffc0, (int)&g_game604780, (int)&g_game604628,
                      (int)&g_game64fb50, (int)&g_game650038, (int)&g_game64fb4c,
                      (int)&g_game604778, (int)&g_game64fbc0, (int)&g_game64ffb4,
                      (int)&g_game64ffac);
        if (g_game650034 != (HANDLE)0) {
            GetObjectA(g_game650034, 0x18, &v34);
            g_game64fc80 = v34.bmWidth;
            g_game6046e8 = v34.bmHeight;
            g_game604774 = CreateCompatibleDC((HDC)0);
            SelectObject(g_game604774, g_game650034);
        }
        v1c = (HANDLE)SendMessageA(a1, 0x31, 0, 0);
        GetObjectA(v1c, 0x3c, &v74);
        v74.lfHeight = 0x16;
        v74.lfItalic = '\x01';
        v74.lfWeight = 700;
        g_game604638 = CreateFontIndirectA(&v74);
        GetObjectA(v1c, 0x3c, &v74);
        v74.lfHeight = 0x10;
        g_game6046dc = CreateFontIndirectA(&v74);
        GetObjectA(v1c, 0x3c, &v74);
        v74.lfHeight = 0x16;
        g_game650030 = CreateFontIndirectA(&v74);
        GetObjectA(v1c, 0x3c, &v74);
        v18 = CreateFontIndirectA(&v74);
        SendDlgItemMessageA(a1, 1, 0x30, (WPARAM)v18, 0);
        g_game64ffa4 = -1;
        g_game64ffb8 = 3;
        g_game64fb54 = 2;
        Game_Helper56cc4d((int)g_game8aa4f0, 0x58e8e8);
        strcpy(g_game64fc18, g_game8aa920);
        v7c = 0;
        for (v78 = 0; v78 < g_game64fe6c; v78++) {
            if (*(int *)(g_game604754 + v78 * 0x108) != 0) {
                v7c++;
            }
        }
        sprintf(g_game604640, g_game8aaa4c, v7c);
        sprintf(g_game6047b0, g_game8aab78, g_game64fe6c - v7c);
        for (v78 = 0; v78 < g_game64fe6c; v78++) {
            if (*(int *)(g_game604754 + v78 * 0x108) == 0) {
                sprintf(g_game6047b0 + strlen(g_game6047b0), (const char *)0x58e910,
                        g_game746b20 + *(int *)(g_game604754 + 4 + v78 * 0x108) * 0x48);
            } else {
                sprintf(g_game604640 + strlen(g_game604640), (const char *)0x58e904,
                        g_game746b20 + *(int *)(g_game604754 + 4 + v78 * 0x108) * 0x48);
            }
        }
        strcpy(g_game64fe70, g_game8aaca4);
        strcpy(g_game64fae0, g_game8aadd0);
        strcpy(g_game64fed8, g_game8aaefc);
        SetDlgItemTextA(a1, 1, g_game8ab028);
        SetRect((LPRECT)&g_game64fb98, g_game64ffc0, g_game64ffc4 - 0x19, g_game650018,
                g_game64ffc4 - 5);
        SetRect((LPRECT)&g_game64fb58, g_game604780, g_game604784 - 0x19, g_game604788,
                g_game604784 - 5);
        SetRect((LPRECT)&g_game64fbb0, g_game6047a0 + 10, g_game60462c, g_game604628 - 10,
                g_game604634);
        v84 = ((g_game6047a0 - g_game604798) * 0x28) / 100;
        v80 = ((g_game6047a0 - g_game604798) * 10) / 100;
        SetRect((LPRECT)&g_game64fad0, g_game604798 + v80, g_game60479c + 0x19,
                g_game604798 + v80 + v84, g_game6047a4);
        SetRect((LPRECT)&g_game64fb78, (g_game6047a0 - v80) - v84, g_game60479c + 0x19,
                g_game6047a0 - v80, g_game6047a4);
        vw = GetDlgItem(a1, 1);
        SetFocus(vw);
        SendMessageA(a1, 0x401, 1, 0);
        Game_Fn466737(a1);
        GetWindowRect(g_game8a92b0, &v14);
        MoveWindow(a1, v14.left, v14.top, v14.right - v14.left, v14.bottom - v14.top, 1);
        if (v38 == 0) {
            EndDialog(a1, -1);
        }
        return (HGDIOBJ)0;

    case 0x111:
        v1a0 = (unsigned int)a3 & 0xffff;
        if (v1a0 == 1) {
            GetWindowRect(a1, &v1b0);
            ShowWindow(a1, 0);
            v1b4 = CreateDCA((LPCSTR)0x58e928, (LPCSTR)0, (LPCSTR)0, (DEVMODEA *)0);
            vb = GetStockObject(4);
            FillRect(v1b4, &v1b0, vb);
            DeleteDC(v1b4);
            EndDialog(a1, 0);
        } else if (0x67e < v1a0 && (int)v1a0 <= g_game64fe6c + 0x67f) {
            if (g_game64ffa4 != -1) {
                CopyRect(&v19c, (RECT *)(&g_game64ffc0 + g_game64ffa4 * 4));
                InflateRect(&v19c, 10, 10);
                Game_Fn4ceb0a((int)a1, (int)g_game650034, (int)&v19c);
            }
            g_game64ffa4 = v1a0 - 0x67f;
            CopyRect(&v19c, (RECT *)(&g_game64ffc0 + g_game64ffa4 * 4));
            InflateRect(&v19c, 10, 10);
            Game_Fn4ceb0a((int)a1, (int)g_game650034, (int)&v19c);
            Game_Fn4ceb0a((int)a1, (int)g_game650034, (int)&g_game604780);
            Game_Fn4ceb0a((int)a1, (int)g_game650034, (int)&g_game604628);
        }
        return (HGDIOBJ)1;

    case 0x135:
    case 0x138:
        v1c4 = (HDC)a3;
        Game_Fn46469a((HDC)a3);
        v1cc = (HWND)a4;
        v1c8 = GetDlgCtrlID((HWND)a4);
        if (v1c8 == 0x67c) {
            SetTextColor(v1c4, g_game64fb50);
        } else if (v1c8 == 0x682 || v1c8 == 0x681) {
            SetTextColor(v1c4, g_game64fb4c);
        } else {
            SetTextColor(v1c4, g_game650038);
        }
        SetBkMode(v1c4, 1);
        vo = GetStockObject(5);
        return vo;

    case 0x201:
        v1f4.x = (unsigned int)a4 & 0xffff;
        v1f4.y = (unsigned int)a4 >> 0x10;
        v1ec = GetDC(a1);
        Game_Fn4ceb15((int)v1ec, (int)a1, (int)g_game650034);
        DPtoLP(v1ec, &v1f4, 1);
        ReleaseDC(a1, v1ec);
        vt = GetDoubleClickTime();
        Sleep(vt);
        v1d4 = PeekMessageA(&v210, a1, 0x203, 0x203, 1);
        SetRect(&v1e8, g_game64ffc0, g_game64ffc4, g_game650018, g_game65001c);
        InflateRect(&v1e8, 10, 10);
        v1d8 = 0;
        if (PtInRect(&v1e8, v1f4) == 0) {
            if (PtInRect((RECT *)&g_game604780, v1f4) == 0 || g_game64ffa4 == -1) {
                if (PtInRect((RECT *)&g_game604628, v1f4) != 0
                    && g_game64ffa4 != -1
                    && *(int *)(g_game604754 + 0x104 + g_game64ffa4 * 0x108) != -1) {
                    v1d8 = 1;
                    v234 = -1;
                    v230 = 0;
                    while (v230 < 8 && v234 == -1) {
                        if (Game_Fn529d5b((int)&v228, v230,
                                          *(int *)(g_game604754 + 0x104 + g_game64ffa4 * 0x108) + 1,
                                          (int)&g_game604628) != 0
                            && PtInRect(&v228, v1f4) != 0) {
                            v234 = *(int *)(g_game604754 + 0x104 + g_game64ffa4 * 0x108) - v230;
                        }
                        v230++;
                    }
                    if (v234 == -1) {
                        v22c = 4;
                    } else if (*(int *)(g_game604754 + 0x104 + g_game64ffa4 * 0x108) == v234) {
                        v22c = 1;
                    } else {
                        v22c = *(int *)(g_game604754 + 0x104 + g_game64ffa4 * 0x108) - v234;
                    }
                    Game_Fn52a070((int)a1, g_game64ffa4 * 0x108 + g_game604754, 1, v22c,
                                  (int)g_game650034, (int)&g_game604780, (int)&g_game604628);
                }
            } else {
                v1d8 = 1;
                if (*(int *)(g_game604754 + 0xc + g_game64ffa4 * 0x108) == 0) {
                    *(int *)(g_game604754 + 0x104 + g_game64ffa4 * 0x108) = -1;
                    *(int *)(g_game604754 + 0xc + g_game64ffa4 * 0x108) = 1;
                    Game_Fn4ceb0a((int)a1, (int)g_game650034,
                                  (int)(&g_game64ffc0 + g_game64ffa4 * 4));
                    Game_Fn4ceb0a((int)a1, (int)g_game650034, (int)&g_game604780);
                    if (*(int *)(g_game604754 + g_game64ffa4 * 0x108) == 0) {
                        Game_Helper40ebb9(0x41);
                    } else {
                        Game_Helper40ebb9(0x42);
                    }
                }
                Game_Fn52a070((int)a1, g_game64ffa4 * 0x108 + g_game604754, 0,
                              (-(int)(v1d4 == 0) & 0xfffffffd) + 4, (int)g_game650034,
                              (int)&g_game604780, (int)&g_game604628);
            }
        } else {
            v1d8 = 1;
            v218 = 0;
            v214 = 0;
            while (v214 < g_game64fe6c && v218 == 0) {
                if (PtInRect((RECT *)(&g_game64ffc0 + v214 * 4), v1f4) != 0) {
                    v218 = v214 + 0x67f;
                }
                v214++;
            }
            if (v218 != 0) {
                SendMessageA(a1, 0x111, v218, 0);
            }
        }
        if (v1d8 == 0) {
            SendMessageA(a1, 0x112, 0xf012, 0);
            GetWindowRect(a1, &v244);
            SetWindowPos(g_game8a92b0, (HWND)0, v244.left, v244.top, 0, 0, 5);
        }
        return (HGDIOBJ)1;

    case 0x206:
        v250.x = (unsigned int)a4 & 0xffff;
        v250.y = (unsigned int)a4 >> 0x10;
        v248 = GetDC(a1);
        Game_Fn4ceb15((int)v248, (int)a1, (int)g_game650034);
        DPtoLP(v248, &v250, 1);
        ReleaseDC(a1, v248);
        if (PtInRect((RECT *)&g_game604780, v250) == 0 || g_game64ffa4 == -1) {
            if (PtInRect((RECT *)&g_game604628, v250) != 0
                && g_game64ffa4 != -1
                && *(int *)(g_game604754 + 0xc + g_game64ffa4 * 0x108) != 0) {
                Game_Fn52a070((int)a1, g_game64ffa4 * 0x108 + g_game604754, 3, 0,
                              (int)g_game650034, (int)&g_game604780, (int)&g_game604628);
            }
        } else {
            if (*(int *)(g_game604754 + 0xc + g_game64ffa4 * 0x108) == 0) {
                *(int *)(g_game604754 + 0x104 + g_game64ffa4 * 0x108) = -1;
                *(int *)(g_game604754 + 0xc + g_game64ffa4 * 0x108) = 1;
                Game_Fn4ceb0a((int)a1, (int)g_game650034,
                              (int)(&g_game64ffc0 + g_game64ffa4 * 4));
                Game_Fn4ceb0a((int)a1, (int)g_game650034, (int)&g_game604780);
            }
            Game_Fn52a070((int)a1, g_game64ffa4 * 0x108 + g_game604754, 2, 0,
                          (int)g_game650034, (int)&g_game604780, (int)&g_game604628);
        }
        return (HGDIOBJ)1;

    case 0x30f:
    case 0x310:
    case 0x311:
        return (HGDIOBJ)Game_Fn466d5f(a1, a2, a3, a4);

    case 0x4c8:
        v1b8 = (HDC)a3;
        v1bc = (HWND)a4;
        if (a3 != 0) {
            SendMessageA(a1, 0x401, (WPARAM)a3, 0);
        }
        if (v1b8 != (HDC)0) {
            InvalidateRect((HWND)v1b8, (RECT *)0, 1);
        }
        if (v1bc != (HWND)0) {
            InvalidateRect(v1bc, (RECT *)0, 1);
        }
        return (HGDIOBJ)0;
    }
    return (HGDIOBJ)0;
}


/* Ladder-tournament screen dialog procedure. The state block arrives as the
 * WM_INITDIALOG lParam and is kept in g_game60477c; per-opponent records are
 * 0x4aa ints apart and the fields past 0x12a80 are the event's own counters,
 * thread handles and critical section. */
// FUNCTION: SHANDALAR 0x0052a3d9
HGDIOBJ __stdcall Game_Fn52a3d9(HWND a1, unsigned int a2, int a3, int a4)
{
    int v3fc;
    int v3f4;
    int v3f0;
    int v3ec;
    LOGFONTA v3e8;
    HFONT v3ac;
    int v3a8;
    HWND v3a4[20];
    int v354;
    RECT v350;
    HFONT v340;
    int v33c;
    int *v338;
    HWND v334;
    int v330;
    HWND v32c;
    HWND v324;
    HWND v320;
    int v31c;
    int v318;
    int v314;
    int v310;
    int v30c;
    int v308;
    BITMAP v304;
    int v2fc;
    int v2ec;
    int v2e8;
    int v2e4;
    int v2e0;
    int v2dc;
    int v2d8;
    RECT v2d4;
    RECT v2c4;
    int v2b4;
    int v2b0;
    int *v2ac;
    int v2a8;
    int v2a4;
    int v2a0;
    SIZE v29c;
    int v294;
    int v290;
    int v28c;
    int v288;
    int v284;
    HDC v280;
    HDC v27c;
    PAINTSTRUCT v278;
    int v238;
    HRGN v234;
    RECT v230;
    RECT v220;
    int v210;
    unsigned int v20c;
    unsigned int v208;
    RECT v204;
    int v1f4;
    RECT v1f0;
    unsigned int v1e0;
    unsigned int v1dc;
    RECT v1d8;
    HANDLE v1c8;
    char v1c4[300];
    unsigned int v98;
    int v94;
    HANDLE v90;
    int v8c;
    int v88;
    unsigned int v84;
    unsigned int v80;
    HWND v7c;
    HGDIOBJ v78;
    unsigned int v74;
    unsigned int v70;
    LOGFONTA v6c;
    BITMAP v30;
    HFONT v18;
    RECT v14;
    int vi;
    HWND vw;
    HBRUSH vb;
    HGDIOBJ vo;
    DWORD vd;

    switch (a2) {
    case 2:
        DeleteDC(g_game64fb44);
        Game_Fn52d15f((int)g_game64f6b4, (int)g_game650044, (int)g_game64fc7c, g_game64fbdc,
                      g_game650040, g_game650020);
        DeleteObject(g_game604848);
        DeleteObject(g_game6046e0);
        v78 = (HGDIOBJ)SendDlgItemMessageA(a1, 0x718, 0x31, 0, 0);
        SendDlgItemMessageA(a1, 0x718, 0x30, 0, 0);
        SendDlgItemMessageA(a1, 0x693, 0x30, 0, 0);
        SendDlgItemMessageA(a1, 0x691, 0x30, 0, 0);
        SendDlgItemMessageA(a1, 1, 0x30, 0, 0);
        SendDlgItemMessageA(a1, 0x692, 0x30, 0, 0);
        DeleteObject(v78);
        KillTimer(a1, 1);
        return (HGDIOBJ)0;

    case 5:
        GetClientRect(a1, &v350);
        v340 = (HFONT)g_game604848;
        GetObjectA(g_game604848, 0x3c, &v3e8);
        v3e8.lfHeight = (v350.bottom - v350.top) / 0x1c;
        if (v3e8.lfHeight % 2 != 0) {
            v3e8.lfHeight = v3e8.lfHeight + 1;
        }
        g_game604848 = CreateFontIndirectA(&v3e8);
        v3ac = (HFONT)g_game604848;
        DeleteObject(v340);
        v340 = (HFONT)g_game6046e0;
        GetObjectA(g_game6046e0, 0x3c, &v3e8);
        v3e8.lfHeight = (v350.bottom - v350.top) / 0x30;
        if (v3e8.lfHeight % 2 != 0) {
            v3e8.lfHeight = v3e8.lfHeight + 1;
        }
        g_game6046e0 = CreateFontIndirectA(&v3e8);
        v3ac = (HFONT)g_game6046e0;
        DeleteObject(v340);
        v354 = 0;
        v3a4[v354] = GetDlgItem(a1, 0x718);
        v354++;
        v3a4[v354] = GetDlgItem(a1, 0x693);
        v354++;
        v3a4[v354] = GetDlgItem(a1, 0x691);
        v354++;
        v3a4[v354] = GetDlgItem(a1, 1);
        v354++;
        v3a4[v354] = GetDlgItem(a1, 0x692);
        v354++;
        LockWindowUpdate(a1);
        v340 = (HFONT)SendMessageA(v3a4[0], 0x31, 0, 0);
        GetObjectA(v340, 0x3c, &v3e8);
        GetClientRect(a1, &v350);
        v3e8.lfHeight = (v350.bottom - v350.top) / 0x28;
        if (v3e8.lfHeight % 2 != 0) {
            v3e8.lfHeight = v3e8.lfHeight + 1;
        }
        v3ac = CreateFontIndirectA(&v3e8);
        for (v3a8 = 0; v3a8 < v354; v3a8++) {
            SendMessageA(v3a4[v3a8], 0x30, (WPARAM)v3ac, 0);
        }
        Game_Fn52d1ea(a1);
        DeleteObject(v340);
        vi = g_game60477c[0x12a81];
        v3f4 = g_game60477c[0x12a80];
        if (0x1f < v3f4) {
            v3f4 = 0x20;
        }
        v3fc = 0;
        for (v3ec = 1; v3ec <= vi; v3ec++) {
            for (v3f0 = 0; v3f0 < v3f4 / 2; v3f0 += 2) {
                Game_Fn52d5db((int)a1, (int)g_game6046e0, (int)g_game60477c, v3ec, v3f0,
                              (int)(g_game64f6c0 + v3fc * 0x10),
                              (int)(g_game64f6c0 + (v3fc + 1) * 0x10));
                v3fc += 2;
            }
            for (v3f0 = 0; v3f0 < v3f4 / 2; v3f0 += 2) {
                Game_Fn52d5db((int)a1, (int)g_game6046e0, (int)g_game60477c, v3ec,
                              v3f0 + v3f4 / 2,
                              (int)(g_game64f6c0 + v3fc * 0x10),
                              (int)(g_game64f6c0 + (v3fc + 1) * 0x10));
                v3fc += 2;
            }
            v3f4 = v3f4 / 2;
        }
        Game_Fn52d5db((int)a1, (int)g_game6046e0, (int)g_game60477c, v3ec, 0,
                      (int)(g_game64f6c0 + v3fc * 0x10),
                      (int)(g_game64f6c0 + (v3fc + 1) * 0x10));
        InvalidateRect(a1, (RECT *)0, 1);
        LockWindowUpdate((HWND)0);
        GetWindowRect(a1, &v350);
        MoveWindow(g_game8a92b0, v350.left, v350.top, v350.right - v350.left,
                   v350.bottom - v350.top, 0);
        return (HGDIOBJ)1;

    case 0xf:
        v234 = CreateRectRgn(0, 0, 1, 1);
        GetUpdateRgn(a1, v234, 0);
        EnterCriticalSection((LPCRITICAL_SECTION)&g_game93a7e0);
        v27c = (HDC)g_game7a00a0;
        v238 = SaveDC((HDC)g_game7a00a0);
        Game_Fn46469a(v27c);
        GetClientRect(a1, &v220);
        GetUpdateRect(a1, &v230, 0);
        IntersectClipRect(v27c, v230.left, v230.top, v230.right, v230.bottom);
        v280 = BeginPaint(a1, &v278);
        if (g_game64f6b4 == (HANDLE)0) {
            vb = GetStockObject(2);
            FillRect(v27c, &v220, vb);
        } else {
            StretchBlt(v27c, v220.left, v220.top, v220.right - v220.left,
                       v220.bottom - v220.top, g_game64fb44, 0, 0, g_game64ffb0,
                       g_game65003c, 0xcc0020);
        }
        if (g_game64fba8 != 0) {
            SelectObject(v27c, g_game604848);
            GetTextExtentPoint32A(v27c, (LPCSTR)0x58ea40, 3, &v29c);
            GetClientRect(a1, &v220);
            v220.bottom = v29c.cy;
            OffsetRect(&v220, 0, 5);
            SetBkMode(v27c, 1);
            SetTextColor(v27c, g_game64fb68);
            DrawTextA(v27c, g_game64ff40, -1, &v220, 0x25);
            OffsetRect(&v220, -2, -2);
            SetTextColor(v27c, g_game6046e4);
            DrawTextA(v27c, g_game64ff40, -1, &v220, 0x25);
            if (g_game60477c[0x12b96] == 0
                || (vd = WaitForSingleObject((HANDLE)g_game60477c[0x12b96], 0), vd != 0x102)) {
                if (g_game60477c[0x12b95] != 0
                    && (vd = WaitForSingleObject((HANDLE)g_game60477c[0x12b95], 0), vd == 0x102)
                    && g_game64ffa8 != 0) {
                    OffsetRect(&v220, 0, v29c.cy);
                    SelectObject(v27c, g_game6046e0);
                    SetBkMode(v27c, 1);
                    SetTextColor(v27c, g_game64fb68);
                    DrawTextA(v27c, g_game6046f0, -1, &v220, 0x25);
                    OffsetRect(&v220, -1, -1);
                    SetTextColor(v27c, g_game64fc84);
                    DrawTextA(v27c, g_game6046f0, -1, &v220, 0x25);
                }
            } else if (g_game64ffa8 != 0) {
                OffsetRect(&v220, 0, v29c.cy);
                SelectObject(v27c, g_game6046e0);
                SetBkMode(v27c, 1);
                SetTextColor(v27c, g_game64fb68);
                DrawTextA(v27c, g_game6045c0, -1, &v220, 0x25);
                OffsetRect(&v220, -1, -1);
                SetTextColor(v27c, g_game64fc84);
                DrawTextA(v27c, g_game6045c0, -1, &v220, 0x25);
            }
            SelectObject(v27c, g_game6046e0);
            GetTextExtentPoint32A(v27c, (LPCSTR)0x58ea44, 3, &v29c);
            v290 = v29c.cy;
            SetTextColor(v27c, g_game6045b8);
            SetBkMode(v27c, 1);
            GetObjectA(g_game650044, 0x18, &v304);
            v2b4 = v304.bmWidth;
            v2dc = v2fc / 6;
            GetObjectA(g_game64fc7c, 0x18, &v304);
            v310 = v304.bmWidth;
            v2d8 = v2fc / 2;
            v30c = g_game60477c[0x12a80];
            if (0x1f < v30c) {
                v30c = 0x20;
            }
            v318 = g_game60477c[0x12a81];
            v314 = 0;
            EnterCriticalSection((LPCRITICAL_SECTION)(g_game60477c + 0x12b8f));
            for (v2a8 = 1; v2a8 <= v318; v2a8++) {
                v308 = Game_Fn52d559(g_game60477c[0x12a80], v2a8, 0);
                v284 = 0;
                v294 = (v2a8 - 1) * v2dc;
                v2e0 = 0;
                v2a0 = v2dc * 5;
                SetTextAlign(v27c, 8);
                for (v2e8 = 0; v2e8 < v30c / 2; v2e8 += 2) {
                    vi = v314 * 0x10;
                    v314++;
                    CopyRect(&v2d4, (RECT *)(g_game64f6c0 + vi));
                    vi = v314 * 0x10;
                    v314++;
                    CopyRect(&v2c4, (RECT *)(g_game64f6c0 + vi));
                    if (RectInRegion(v234, &v2d4) != 0) {
                        Game_Fn464d0b((int)v27c, (int)&v2d4, (int)g_game650044, v2b4, v2dc,
                                      v284, v294, v2e0, v2a0);
                    }
                    if (RectInRegion(v234, &v2c4) != 0) {
                        Game_Fn464d0b((int)v27c, (int)&v2c4, (int)g_game650044, v2b4, v2dc,
                                      v284, v294, v2e0, v2a0);
                    }
                    v2e4 = v2d4.left + ((v2d4.right - v2d4.left) * 8) / 100;
                    v2b0 = v2d4.right - v2d4.left;
                    v28c = v2d4.bottom - ((v2d4.bottom - v2d4.top) - v290) / 2;
                    v288 = v2c4.bottom - ((v2d4.bottom - v2d4.top) - v290) / 2;
                    v2a4 = g_game60477c[v308 + 0x12a8f];
                    v308++;
                    if (v2a4 != -1 && g_game60477c[v2a4 * 0x4aa + 0x4a9] != 0) {
                        v2ac = g_game60477c + v2a4 * 0x4aa;
                        SetTextColor(v27c, g_game64fb68);
                        TextOutA(v27c, v2e4, v28c, (LPCSTR)v2ac, strlen((char *)v2ac));
                        if (EqualRect(&v2d4, (RECT *)&g_game64fbc8) == 0) {
                            SetTextColor(v27c, g_game6045b8);
                        } else {
                            SetTextColor(v27c, g_game64fc84);
                        }
                        TextOutA(v27c, v2e4 - 1, v28c - 1, (LPCSTR)v2ac, strlen((char *)v2ac));
                    }
                    if (v2c4.right != v2c4.left && -1 < v2c4.right - v2c4.left) {
                        v2a4 = g_game60477c[v308 + 0x12a8f];
                        v308++;
                        if (v2a4 != -1 && g_game60477c[v2a4 * 0x4aa + 0x4a9] != 0) {
                            v2ac = g_game60477c + v2a4 * 0x4aa;
                            SetTextColor(v27c, g_game64fb68);
                            TextOutA(v27c, v2e4 + 2, v288, (LPCSTR)v2ac, strlen((char *)v2ac));
                            if (EqualRect(&v2c4, (RECT *)&g_game64fbc8) == 0) {
                                SetTextColor(v27c, g_game6045b8);
                            } else {
                                SetTextColor(v27c, g_game64fc84);
                            }
                            TextOutA(v27c, v2e4 + 1, v288 - 1, (LPCSTR)v2ac, strlen((char *)v2ac));
                        }
                    }
                }
                SetTextAlign(v27c, 10);
                for (v2e8 = 0; v2e8 < v30c / 2; v2e8 += 2) {
                    vi = v314 * 0x10;
                    v314++;
                    CopyRect(&v2d4, (RECT *)(g_game64f6c0 + vi));
                    vi = v314 * 0x10;
                    v314++;
                    CopyRect(&v2c4, (RECT *)(g_game64f6c0 + vi));
                    if (RectInRegion(v234, &v2d4) != 0) {
                        Game_Fn464d0b((int)v27c, (int)&v2d4, (int)g_game650044, v2b4, v2dc,
                                      v284, v294, v2e0, v2a0);
                    }
                    if (RectInRegion(v234, &v2c4) != 0) {
                        Game_Fn464d0b((int)v27c, (int)&v2c4, (int)g_game650044, v2b4, v2dc,
                                      v284, v294, v2e0, v2a0);
                    }
                    v28c = v2d4.bottom - ((v2d4.bottom - v2d4.top) - v290) / 2;
                    v288 = v2c4.bottom - ((v2d4.bottom - v2d4.top) - v290) / 2;
                    v2e4 = v2d4.right - ((v2d4.right - v2d4.left) * 8) / 100;
                    v2b0 = v2d4.right - v2d4.left;
                    v2a4 = g_game60477c[v308 + 0x12a8f];
                    v308++;
                    if (v2a4 != -1 && g_game60477c[v2a4 * 0x4aa + 0x4a9] != 0) {
                        v2ac = g_game60477c + v2a4 * 0x4aa;
                        SetTextColor(v27c, g_game64fb68);
                        TextOutA(v27c, v2e4 - 2, v28c, (LPCSTR)v2ac, strlen((char *)v2ac));
                        if (EqualRect(&v2d4, (RECT *)&g_game64fbc8) == 0) {
                            SetTextColor(v27c, g_game6045b8);
                        } else {
                            SetTextColor(v27c, g_game64fc84);
                        }
                        TextOutA(v27c, v2e4 - 3, v28c - 1, (LPCSTR)v2ac, strlen((char *)v2ac));
                    }
                    if (v2c4.right != v2c4.left && -1 < v2c4.right - v2c4.left) {
                        v2a4 = g_game60477c[v308 + 0x12a8f];
                        v308++;
                        if (v2a4 != -1 && g_game60477c[v2a4 * 0x4aa + 0x4a9] != 0) {
                            v2ac = g_game60477c + v2a4 * 0x4aa;
                            SetTextColor(v27c, g_game64fb68);
                            TextOutA(v27c, v2e4, v288, (LPCSTR)v2ac, strlen((char *)v2ac));
                            if (EqualRect(&v2c4, (RECT *)&g_game64fbc8) == 0) {
                                SetTextColor(v27c, g_game6045b8);
                            } else {
                                SetTextColor(v27c, g_game64fc84);
                            }
                            TextOutA(v27c, v2e4 - 1, v288 - 1, (LPCSTR)v2ac, strlen((char *)v2ac));
                        }
                    }
                }
                v30c = v30c / 2;
            }
            vi = v314 * 0x10;
            v314++;
            CopyRect(&v2d4, (RECT *)(g_game64f6c0 + vi));
            vi = v314 * 0x10;
            v314++;
            CopyRect(&v2c4, (RECT *)(g_game64f6c0 + vi));
            v284 = 0;
            v294 = 0;
            v2e0 = 0;
            v2a0 = v2d8;
            Game_Fn464d0b((int)v27c, (int)&v2d4, (int)g_game64fc7c, v310, v2d8, 0, 0, 0, v2d8);
            v2e4 = v2d4.left + (v2d4.right - v2d4.left) / 2;
            v2ec = v2d4.bottom - ((v2d4.bottom - v2d4.top) - v290) / 2;
            v2b0 = v2d4.right - v2d4.left;
            SetTextAlign(v27c, 0xe);
            v2a4 = g_game60477c[v308 + 0x12a8f];
            v308++;
            if (v2a4 != -1 && g_game60477c[v2a4 * 0x4aa + 0x4a9] != 0) {
                v2ac = g_game60477c + v2a4 * 0x4aa;
                SetTextColor(v27c, g_game64fb68);
                TextOutA(v27c, v2e4, v2ec, (LPCSTR)v2ac, strlen((char *)v2ac));
                if (EqualRect(&v2d4, (RECT *)&g_game64fbc8) == 0) {
                    SetTextColor(v27c, g_game6045b8);
                } else {
                    SetTextColor(v27c, g_game64fc84);
                }
                TextOutA(v27c, v2e4 - 1, v2ec - 1, (LPCSTR)v2ac, strlen((char *)v2ac));
            }
            LeaveCriticalSection((LPCRITICAL_SECTION)(g_game60477c + 0x12b8f));
        }
        RestoreDC((HDC)g_game7a00a0, v238);
        v27c = v280;
        Game_Fn46469a(v280);
        GetClientRect(a1, &v220);
        BitBlt(v27c, 0, 0, v220.right, v220.bottom, (HDC)g_game7a00a0, 0, 0, 0xcc0020);
        LeaveCriticalSection((LPCRITICAL_SECTION)&g_game93a7e0);
        EndPaint(a1, &v278);
        DeleteObject(v234);
        return (HGDIOBJ)1;

    case 0x14:
        return (HGDIOBJ)1;

    case 0x2b:
        v338 = (int *)a4;
        vw = GetFocus();
        if (vw == (HWND)v338[5]) {
            v33c = g_game7bf614;
        } else if ((v338[4] & 2) == 0) {
            v33c = g_game7bf610;
        } else {
            v33c = 0x10000c6;
        }
        if (g_game7bf604 == 0) {
            Game_Fn466049((int)v338, (int)GetStockObject(2), g_game7bf608, g_game7bf60c,
                          v33c, 0);
        } else {
            Game_Fn4660a7((int)v338, g_game7bf604, g_game7bf608, g_game7bf60c, v33c, 0);
        }
        return (HGDIOBJ)1;

    case 0x7e:
        Game_Fn52d15f((int)g_game64f6b4, (int)g_game650044, (int)g_game64fc7c, g_game64fbdc,
                      g_game650040, g_game650020);
        Game_Fn52cefc((int)&g_game64f6b4, (int)&g_game6046e4, (int)&g_game64fb68,
                      (int)&g_game6045b8, (int)&g_game64fc84, (int)&g_game650044,
                      (int)&g_game64fc7c, (int)&g_game64fbdc, (int)&g_game650040,
                      (int)&g_game650020, (int)&g_game65002c, (int)&g_game64fb70);
        return (HGDIOBJ)0;

    case 0x110:
        SetWindowTextA(a1, g_game7a01f0);
        g_game64ffa8 = 0;
        g_game650028 = 0;
        SetTimer(a1, 3, 0x5dc, (TIMERPROC)0);
        GetClientRect(a1, &v14);
        vi = g_game650028;
        g_game650028 = g_game650028 + 1;
        SetRect((LPRECT)(g_game64fc88 + vi * 0x10), v14.left,
                v14.top + ((v14.bottom - v14.top) * 3) / 100, v14.right,
                v14.top + ((v14.bottom - v14.top) * 0x14) / 100);
        g_game650028 = g_game650028 + 1;
        v70 = GetWindowLongA(a1, -0x10);
        v70 = v70 | 0x2000000;
        SetWindowLongA(a1, -0x10, v70);
        Game_Fn52cefc((int)&g_game64f6b4, (int)&g_game6046e4, (int)&g_game64fb68,
                      (int)&g_game6045b8, (int)&g_game64fc84, (int)&g_game650044,
                      (int)&g_game64fc7c, (int)&g_game64fbdc, (int)&g_game650040,
                      (int)&g_game650020, (int)&g_game65002c, (int)&g_game64fb70);
        if (g_game64f6b4 != (HANDLE)0) {
            g_game64fb44 = CreateCompatibleDC((HDC)0);
            GetObjectA(g_game64f6b4, 0x18, &v30);
            g_game64ffb0 = v30.bmWidth;
            g_game65003c = v30.bmHeight;
            SelectObject(g_game64fb44, g_game64f6b4);
        }
        v18 = (HFONT)SendMessageA(a1, 0x31, 0, 0);
        GetObjectA(v18, 0x3c, &v6c);
        g_game604848 = CreateFontIndirectA(&v6c);
        GetObjectA(v18, 0x3c, &v6c);
        g_game6046e0 = CreateFontIndirectA(&v6c);
        GetObjectA(v18, 0x3c, &v6c);
        v18 = CreateFontIndirectA(&v6c);
        SendDlgItemMessageA(a1, 0x718, 0x30, (WPARAM)v18, 0);
        SendDlgItemMessageA(a1, 0x693, 0x30, (WPARAM)v18, 0);
        SendDlgItemMessageA(a1, 0x691, 0x30, (WPARAM)v18, 0);
        SendDlgItemMessageA(a1, 1, 0x30, (WPARAM)v18, 0);
        SendDlgItemMessageA(a1, 0x692, 0x30, (WPARAM)v18, 0);
        g_game60477c = (int *)a4;
        ((int *)a4)[0x12b95] = 0;
        SetTimer(a1, 1, 1000, (TIMERPROC)0);
        g_game64fba8 = 1;
        vi = 0;
        vw = GetDlgItem(a1, 0x69c);
        ShowWindow(vw, vi);
        vi = 0;
        vw = GetDlgItem(a1, 0x69d);
        ShowWindow(vw, vi);
        vi = 0;
        vw = GetDlgItem(a1, 0x69b);
        ShowWindow(vw, vi);
        vi = 0;
        vw = GetDlgItem(a1, 0x69a);
        ShowWindow(vw, vi);
        Game_Helper56ca10((int)g_game8aa4f0, 0x58e9c8);
        strcpy(g_game64ff40, g_game8aa920);
        SetDlgItemTextA(a1, 0x693, g_game8aaa4c);
        SetDlgItemTextA(a1, 0x691, g_game8aab78);
        SetDlgItemTextA(a1, 0x692, g_game8aaca4);
        SetDlgItemTextA(a1, 0x718, g_game8aadd0);
        SetDlgItemTextA(a1, 1, g_game8aaefc);
        strcpy(g_game64fbe0, g_game8aaefc);
        strcpy(g_game604758, g_game8ab280);
        Game_Helper56ca10((int)g_game8aa4f0, 0x58e9e0);
        strcpy(g_game6045c0, g_game8aa920);
        strcpy(g_game6046f0, g_game8aaa4c);
        vw = GetDlgItem(a1, 1);
        SetFocus(vw);
        SendMessageA(a1, 0x401, 1, 0);
        Game_Fn466737(a1);
        vw = GetParent(a1);
        GetWindowRect(vw, &v14);
        MoveWindow(a1, v14.left, v14.top, v14.right - v14.left, v14.bottom - v14.top, 1);
        if (g_game60477c[0x4a9] == 0) {
            SetTimer(a1, 2, 100, (TIMERPROC)0);
        } else if (g_game60477c[0x12b97] == 0) {
            if (g_game60477c[0x12b98] != 0) {
                vw = GetDlgItem(a1, 1);
                EnableWindow(vw, 0);
                g_game60477c[0x12b95] = (int)_beginthreadex((void *)0, 0, Game_Fn52eba4,
                                                            g_game60477c, 0, &v74);
                SetThreadPriority((HANDLE)g_game60477c[0x12b95], -1);
            }
        } else {
            vw = GetDlgItem(a1, 1);
            SendMessageA(a1, 0x111, 1, (LPARAM)vw);
        }
        return (HGDIOBJ)0;

    case 0x111:
        v84 = (unsigned int)a3 & 0xffff;
        v80 = (unsigned int)a3 >> 0x10;
        v7c = (HWND)a4;
        if (v84 < 0x692) {
            if (v84 == 0x691) {
                Game_Fn40f0f6();
                Game_Fn56d081();
                Game_Fn527b73(a1, g_game60477c);
                Game_Fn56cf20((int)g_game8a92b0, 0, 0);
            } else if (v84 == 1) {
                if (g_game60477c[0x12a81] < g_game60477c[0x12a8e]) {
                    EndDialog(a1, 0);
                } else if (g_game60477c[0x12b96] == 0
                           || (vd = WaitForSingleObject((HANDLE)g_game60477c[0x12b96], 0),
                               vd != 0x102)) {
                    if (g_game60477c[0x4a8] < g_game60477c[0x12a8d]) {
                        Game_Helper56ca10((int)g_game8aa4f0, 0x58ea10);
                        sprintf(v1c4, g_game8ab154, g_game60477c[0x12a8d]);
                        MessageBoxA(a1, v1c4, g_game7a01f0, 0x10);
                    } else {
                        EnableWindow(v7c, 0);
                        v94 = Game_Fn52e95f(a1, g_game60477c, g_game60477c[0x12a8e]);
                        g_game60477c[0x12b98] = 1;
                        g_game60477c[0x12b95] = (int)_beginthreadex((void *)0, 0,
                                                                    Game_Fn52eba4,
                                                                    g_game60477c, 0, &v98);
                        SetThreadPriority((HANDLE)g_game60477c[0x12b95], -1);
                        if (g_game60477c[0x12a8e] == g_game60477c[0x12a81] && v94 == 1) {
                            UpdateWindow(a1);
                            Game_Fn4ceb41((int)a1, 0x58ea28, (int)g_game79ff90, 0);
                        }
                    }
                } else {
                    Game_Helper56ca10((int)g_game8aa4f0, 0x58e9f8);
                    MessageBoxA(a1, g_game8ab028, g_game7a01f0, 0x10);
                }
            }
        } else if (v84 == 0x692) {
            vi = 0;
            vw = GetDlgItem(a1, 0x718);
            ShowWindow(vw, vi);
            vi = 0;
            vw = GetDlgItem(a1, 0x691);
            ShowWindow(vw, vi);
            vi = 0;
            vw = GetDlgItem(a1, 0x693);
            ShowWindow(vw, vi);
            vi = 0;
            vw = GetDlgItem(a1, 0x692);
            ShowWindow(vw, vi);
            vi = 0;
            vw = GetDlgItem(a1, 1);
            ShowWindow(vw, vi);
            UpdateWindow(a1);
            if (g_game60477c[0x12b95] != 0) {
                v1c8 = (HANDLE)g_game60477c[0x12b95];
                g_game60477c[0x12b95] = 0;
                WaitForSingleObject(v1c8, 0xffffffff);
                CloseHandle(v1c8);
            }
            if (g_game60477c[0x12b96] != 0) {
                v1c8 = (HANDLE)g_game60477c[0x12b96];
                g_game60477c[0x12b96] = 0;
                WaitForSingleObject(v1c8, 0xffffffff);
                CloseHandle(v1c8);
            }
            EndDialog(a1, 0);
        } else if (v84 == 0x693) {
            if (g_game60477c[0x12b96] != 0) {
                SuspendThread((HANDLE)g_game60477c[0x12b96]);
            }
            if (g_game60477c[0x12b95] != 0) {
                SuspendThread((HANDLE)g_game60477c[0x12b95]);
            }
            EnterCriticalSection((LPCRITICAL_SECTION)(g_game60477c + 0x12b8f));
            Game_Fn544e3e((int)a1);
            LeaveCriticalSection((LPCRITICAL_SECTION)(g_game60477c + 0x12b8f));
            if (g_game60477c[0x12b96] != 0) {
                ResumeThread((HANDLE)g_game60477c[0x12b96]);
            }
            if (g_game60477c[0x12b95] != 0) {
                ResumeThread((HANDLE)g_game60477c[0x12b95]);
            }
        } else if (v84 == 0x718
                   && (g_game60477c[0x12b96] == 0
                       || (vd = WaitForSingleObject((HANDLE)g_game60477c[0x12b96], 0),
                           vd != 0x102))) {
            if (g_game60477c[0x12b95] != 0) {
                v90 = (HANDLE)g_game60477c[0x12b95];
                g_game60477c[0x12b95] = 0;
                WaitForSingleObject(v90, 0xffffffff);
                CloseHandle(v90);
            }
            EnterCriticalSection((LPCRITICAL_SECTION)(g_game60477c + 0x12b8f));
            v88 = Game_Fn52d559(1 << (g_game60477c[0x12a81] & 0x1f), 2, 0);
            for (v8c = v88; v8c < 0xff; v8c++) {
                g_game60477c[v8c + 0x12a8f] = -1;
            }
            LeaveCriticalSection((LPCRITICAL_SECTION)(g_game60477c + 0x12b8f));
            g_game60477c[0x12b8e] = 1;
            InvalidateRect(a1, (RECT *)0, 1);
            g_game60477c[0x12a8e] = 1;
            g_game60477c[0x12b97] = 0;
            g_game60477c[0x12b98] = 0;
            SetDlgItemTextA(a1, 1, g_game64fbe0);
            vw = GetDlgItem(a1, 1);
            EnableWindow(vw, 1);
            vi = 5;
            vw = GetDlgItem(a1, 0x692);
            ShowWindow(vw, vi);
        }
        return (HGDIOBJ)1;

    case 0x113:
        if (a3 == 1) {
            EnterCriticalSection((LPCRITICAL_SECTION)(g_game60477c + 0x12b8f));
            if (g_game60477c[0x12b8e] != 0) {
                InvalidateRect(a1, (RECT *)0, 0);
                g_game60477c[0x12b8e] = 0;
            }
            LeaveCriticalSection((LPCRITICAL_SECTION)(g_game60477c + 0x12b8f));
            if (g_game60477c[0x12b95] != 0
                && (vd = WaitForSingleObject((HANDLE)g_game60477c[0x12b95], 0), vd == 0)) {
                CloseHandle((HANDLE)g_game60477c[0x12b95]);
                g_game60477c[0x12b95] = 0;
                g_game60477c[0x12b98] = 0;
                g_game60477c[0x12a8e] = g_game60477c[0x12a8e] + 1;
                vw = GetDlgItem(a1, 1);
                EnableWindow(vw, 1);
                if (g_game60477c[0x12a81] < g_game60477c[0x12a8e]) {
                    SetDlgItemTextA(a1, 1, g_game604758);
                    vi = 0;
                    vw = GetDlgItem(a1, 0x692);
                    ShowWindow(vw, vi);
                } else {
                    Game_Fn4f8613(0);
                }
            }
            return (HGDIOBJ)1;
        }
        if (a3 == 2) {
            KillTimer(a1, 2);
            Game_Fn40f0f6();
            Game_Fn56d081();
            g_game60477c[0x4a8] = 0;
            Game_Fn527b73(a1, g_game60477c);
            Game_Fn56cf20((int)g_game8a92b0, 0, 0);
            if (0 < g_game60477c[0x12a8c]) {
                Game_Fn52a1ed(g_game60477c, a1);
            }
            g_game60477c[0x4a9] = 1;
            InvalidateRect(a1, (RECT *)0, 0);
            Game_Fn4f8613(0);
            return (HGDIOBJ)1;
        }
        if (a3 == 3) {
            g_game64ffa8 = (g_game64ffa8 == 0);
            for (v31c = 0; v31c < g_game650028; v31c++) {
                InvalidateRect(a1, (RECT *)(g_game64fc88 + v31c * 0x10), 0);
                UpdateWindow(a1);
            }
            return (HGDIOBJ)1;
        }
        return (HGDIOBJ)0;

    case 0x135:
    case 0x138:
        v32c = (HWND)a3;
        Game_Fn46469a((HDC)a3);
        v334 = (HWND)a4;
        v330 = GetDlgCtrlID((HWND)a4);
        vw = GetFocus();
        if (vw == v334) {
            SetTextColor((HDC)v32c, g_game64fb70);
        } else {
            SetTextColor((HDC)v32c, g_game6045b8);
        }
        SetBkMode((HDC)v32c, 1);
        vo = GetStockObject(5);
        return vo;

    case 0x200:
        v1e0 = (unsigned int)a4 & 0xffff;
        v1dc = (unsigned int)a4 >> 0x10;
        v1f4 = Game_Fn52dcf5((int)a1, (int)g_game64f6c0, (int)g_game60477c, v1e0, v1dc,
                             (int)&v1d8);
        if (EqualRect(&v1d8, (RECT *)&g_game64fbc8) == 0) {
            if (IsRectEmpty((RECT *)&g_game64fbc8) == 0) {
                CopyRect(&v1f0, (RECT *)&g_game64fbc8);
                InflateRect(&v1f0, 0x32, 0);
                InvalidateRect(a1, &v1f0, 0);
                SetRectEmpty((LPRECT)&g_game64fbc8);
            }
            if (v1f4 != 0) {
                CopyRect((LPRECT)&g_game64fbc8, &v1d8);
                if (IsRectEmpty((RECT *)&g_game64fbc8) == 0) {
                    CopyRect(&v1f0, (RECT *)&g_game64fbc8);
                    InflateRect(&v1f0, 0x32, 0);
                    InvalidateRect(a1, &v1f0, 0);
                }
            }
        }
        return (HGDIOBJ)1;

    case 0x201:
        v20c = (unsigned int)a4 & 0xffff;
        v208 = (unsigned int)a4 >> 0x10;
        v210 = Game_Fn52dcf5((int)a1, (int)g_game64f6c0, (int)g_game60477c, v20c, v208, 0);
        if (v210 == 0) {
            SendMessageA(a1, 0x112, 0xf012, 0);
            GetWindowRect(a1, &v204);
            SetWindowPos(g_game8a92b0, (HWND)0, v204.left, v204.top, 0, 0, 5);
        } else {
            Game_Fn52df3a(a1, v210);
        }
        return (HGDIOBJ)1;

    case 0x30f:
    case 0x310:
    case 0x311:
        return (HGDIOBJ)Game_Fn466d5f(a1, a2, a3, a4);

    case 0x496:
        if (a3 == 0) {
            ShowWindow(a1, 0);
        } else {
            ShowWindow(a1, 5);
        }
        return (HGDIOBJ)1;

    case 0x4c8:
        v320 = (HWND)a3;
        v324 = (HWND)a4;
        SendMessageA(a1, 0x401, GetDlgCtrlID((HWND)a3), 0);
        if (v320 != (HWND)0) {
            InvalidateRect(v320, (RECT *)0, 1);
        }
        if (v324 != (HWND)0) {
            InvalidateRect(v324, (RECT *)0, 1);
        }
        return (HGDIOBJ)0;
    }
    return (HGDIOBJ)0;
}


/* Random overland encounter for region a1: picks three cards by colour, then
 * one of fourteen outcomes, and finally plays the sound and shows the card
 * that the outcome selected. The 0x0d..0x0e branch and the `switch` fall
 * through from case 0 into case 1 deliberately.
 *
 * `(g_game78cf00 >> 2) % 3` is the 64-bit idiv Ghidra renders as
 * CONCAT44(x >> 0x1f, x >> 2) % 3. */
// FUNCTION: SHANDALAR 0x0052fae0
void Game_Fn52fae0(int a1)
{
    int iv;
    int ig;
    int ih;
    int ii;
    int ij;
    int ik;
    int il[4];
    int im;
    int in;

    Game_Fn4ed7c8(g_game58edb8[a1]);
    Game_Helper42fe00(g_game5832b4);
    for (ik = 1; ik < 4; ik++) {
        do {
            if (Game_Helper522508(3) == 0) {
                ii = Game_Fn56bcf7(0x40, 1);
            } else {
                ii = Game_Fn56bcf7(0x3f, 1 << (a1 & 0x1f));
            }
            iv = Game_Fn56c5ea(ii);
        } while (iv != ik
                 || Game_Fn4bb1cf(ii) < 0
                 || (*((unsigned char *)&g_gameCards[ii].flags3c + 1) & 9) != 0
                 || Game_Fn428b05(ii) == 0);
        if (ik == 1 && Game_Helper522508(2) != 0) {
            ii = a1 - 1;
        }
        il[ik] = ii;
    }
    do {
        do {
            iv = Game_Helper522508(500);
        } while (deck[iv] == -1);
        ig = Game_Fn56c5ea(deck[iv] & 0xfff);
    } while (ig < 2 || Game_Fn428b05(iv) == 0);
    Game_Fn5616cb(1);
    do {
        if ((g_game78cf00 >> 2) % 3 != 0) {
            if (Game_Helper522508(2) == 0) {
                do {
                    do {
                        ik = Game_Helper522508(g_game73c00c - 0x39);
                        iv = Game_Fn56c0e5((signed char)g_gameCards[ik].unknown02a[0],
                                           1 << (a1 & 0x1f), 1);
                    } while (iv == 0);
                } while ((*((unsigned char *)&g_gameCards[ik].flags3c + 1) & 1) != 0
                         || Game_Fn428b05(ik) == 0);
            } else {
                ik = a1 - 1;
            }
            goto done;
        }
        ig = Game_Helper522508(0xe);
        ih = ig + 5;
        Game_Fn4290e2(5, ih);
        Game_Helper522508(2);
    } while (g_game5863b4 != -1 && (ih == 0xb || ih == 0x11));
    sprintf(g_game78cf10, g_game77e180, Game_Fn5307c3(ih));
    Game_Helper4ecfa2();
    Game_Fn412bff((int)g_game78cf10, 0x5a, 100);
    ik = -1;
    ih = Game_Helper522508(4);
    if (ih == 0) {
        ik = Game_Fn530770();
    }
    switch (ig) {
    case 0:
        im = Game_Fn4f2232();
        if (im != -1) {
            Game_Fn508970(im);
            break;
        }
        /* falls through */
    case 1:
        if (Game_Fn534530() != 0) {
            ik = il[3];
        }
        break;
    case 2:
        if (Game_Helper522508(2) == 0 && 0x7f < g_game59120c) {
            strcpy(g_game78cf10, g_game77e188);
            Gold = Gold / 2;
            ik = Game_Fn530770();
        } else {
            Game_Fn5626b0(0x58ee48, 0xf, 100, 100, 0);
            strcpy(g_game78cf10, g_game77e184);
            Gold = Gold + 500;
            ik = -1;
        }
        Game_Fn412bff((int)g_game78cf10, 0x5a, 100);
        break;
    case 3:
        do {
        } while (Game_Fn530b90(-1, -1) != 0);
        break;
    case 4:
        Game_Fn5626b0(0x58ee60, 0x97, 100, 100, 0);
        if (g_game59120c < 0x100) {
            Game_Fn5307dd(8, 2);
        } else {
            Game_Fn5307dd(0xd, 3);
        }
        break;
    case 5:
        Game_Fn5626b0(0x58ee7c, 0x97, 100, 100, 0);
        Game_Fn5307dd(0x12, 4);
        break;
    case 6:
        g_game5863b4 = Game_Helper56c705(0x1b4);
        Game_Fn5626b0(0x58ee98, 0x97, 100, 100, 0);
        Game_Fn5307dd(0xd, 2);
        break;
    case 7:
        Game_Fn5626b0(0x58eeb4, 0xf, 100, 100, 0);
        do {
            strcpy(g_game78cf10, g_game77e18c);
            strcat(g_game78cf10, g_game77e190);
            for (ij = 0; ij < 5; ij++) {
                sprintf(g_game78cf10 + strlen(g_game78cf10), g_game77e194,
                        g_game77d090 + (ij * 5 + 5) * 5, g_game789910[ij]);
            }
            iv = Game_Fn412bff((int)g_game78cf10, 0x5a, 100);
            if (iv < 1 || Gold < 200) {
                break;
            }
            Gold = Gold - 200;
            g_game78990c[iv] = g_game78990c[iv] + 1;
        } while (199 < Gold);
        break;
    case 8:
        in = Game_Helper522508(g_game593934 - 3);
        in = in + 1;
        g_game591a77[(g_game593934 - 1) * 0x8c] = 0x10;
        *(int *)(g_game591a7c + (g_game593934 - 1) * 0x8c) = *(int *)(g_game591a7c + in * 0x8c);
        strcpy(g_game591a12 + (g_game593934 - 1) * 0x8c, g_game591a12 + in * 0x8c);
        strcpy(g_game591a08 + (g_game593934 - 1) * 0x8c, g_game591a08 + in * 0x8c);
        strcpy(g_game591a44 + (g_game593934 - 1) * 0x8c, g_game591a44 + in * 0x8c);
        g_game591a76[(g_game593934 - 1) * 0x8c] = g_game591a76[in * 0x8c];
        Game_Fn5626b0(0x58eecc, 0x97, 100, 100, 0);
        Game_Fn5307dd(0x10, 3);
        break;
    case 9:
        if (Game_Fn534530() != 0) {
            ik = Game_Fn530770();
        }
        break;
    case 10:
        if (Game_Helper522508(2) == 0 && 0x7f < g_game59120c) {
            strcpy(g_game78cf10, g_game77e19c);
            for (ij = 0; ij < 5; ij++) {
                g_game789910[ij] = (g_game789910[ij] + 1) / 2;
            }
            ik = Game_Fn530770();
        } else {
            Game_Fn5626b0(0x58eee8, 0xf, 100, 100, 0);
            strcpy(g_game78cf10, g_game77e198);
            for (ij = 0; ij < 5; ij++) {
                g_game789910[ij] = g_game789910[ij] + 1;
            }
            ik = -1;
        }
        Game_Fn412bff((int)g_game78cf10, 0x5a, 100);
        break;
    case 0xb:
        ii = 0;
        do {
            iv = Game_Helper522508(5);
            if (g_game789910[iv] != 0) {
                break;
            }
            ii++;
        } while (ii < 99);
        if (g_game789910[iv] != 0) {
            Game_Helper564e70((int)g_game78cf10, 0x1000, (int)g_game77e1a0[0],
                              (int)(g_game77d090 + (iv * 5 + 5) * 5),
                              (int)g_game77e1a0[a1]);
            strcat(g_game78cf10, g_game77e1b8);
            ig = Game_Fn412bff((int)g_game78cf10, 0x5a, 100);
            if (ig == 1) {
                g_game789910[iv] = g_game789910[iv] - 1;
                Game_Fn53114a(a1);
            }
        }
        break;
    case 0xc:
        sprintf(g_game78cf10, g_game77e1bc,
                (char *)g_gameCards[deck[iv] & 0xfff].unknown001);
        strcat(g_game78cf10, g_game77e1c0);
        ig = Game_Fn412bff((int)g_game78cf10, 100, 100);
        if (ig == 1) {
            Game_Helper56bfa4(iv);
            g_game5863b4 = 5;
        }
        break;
    case 0xd:
        do {
        } while (Game_Fn530b90(0, -1) != 0);
        Game_Fn4ed0bd();
        break;
    }
done:
    if (ik != -1) {
        if ((g_gameCards[ik].flags & 0x2) == 0) {
            if ((g_gameCards[ik].flags & 0x40) == 0) {
                if ((g_gameCards[ik].flags & 0x4) == 0) {
                    if ((g_gameCards[ik].flags & 0x10) == 0) {
                        if ((g_gameCards[ik].flags & 0x20) == 0) {
                            if ((g_gameCards[ik].flags & 0x8) == 0) {
                                if ((g_gameCards[ik].flags & 0x1) == 0) {
                                    Game_Fn5626b0(0x58f034, 0xf, 100, 100, 0);
                                } else {
                                    switch (g_gameCards[ik].unknown02a[0]) {
                                    case 2:
                                        Game_Fn5626b0(0x58efa4, 0xf, 100, 100, 0);
                                        break;
                                    case 4:
                                        Game_Fn5626b0(0x58efbc, 0xf, 100, 100, 0);
                                        break;
                                    case 8:
                                        Game_Fn5626b0(0x58efd4, 0xf, 100, 100, 0);
                                        break;
                                    case 0x10:
                                        Game_Fn5626b0(0x58efec, 0xf, 100, 100, 0);
                                        break;
                                    case 0x20:
                                        Game_Fn5626b0(0x58f004, 0xf, 100, 100, 0);
                                        break;
                                    default:
                                        Game_Fn5626b0(0x58f01c, 0xf, 100, 100, 0);
                                        break;
                                    }
                                }
                            } else {
                                Game_Fn5626b0(0x58ef88, 0xf, 100, 100, 0);
                            }
                        } else {
                            Game_Fn5626b0(0x58ef6c, 0xf, 100, 100, 0);
                        }
                    } else {
                        Game_Fn5626b0(0x58ef50, 0xf, 100, 100, 0);
                    }
                } else {
                    Game_Fn5626b0(0x58ef34, 0xf, 100, 100, 0);
                }
            } else {
                Game_Fn5626b0(0x58ef18, 0xf, 100, 100, 0);
            }
        } else {
            Game_Fn5626b0(0x58ef00, 0xf, 100, 100, 0);
        }
        Game_Helper4ecfa2();
        Game_Fn5584d5(ik, 0xd0, (int)g_game77e1c8, 1);
        iv = Game_Fn56bd9d(ik);
        if (iv != -1) {
            deck[iv] |= 0x4000;
        }
        Game_Fn5597ca();
    }
    return;
}

/* NOT RECONSTRUCTED HERE -- 0x005231ec and 0x0052f012.
 *
 * Both were dropped because Ghidra's decompilation of them is structurally
 * wrong, not merely awkward, so translating it would produce code that cannot
 * be a match and whose shape nothing supports:
 *
 * 0x005231ec: the record writes go through a base pointer Ghidra does not
 *   model. At 0x005239dd the store is `mov ecx,[ebp-0x2c] / mov [ecx+0x6a],ax`
 *   -- a struct pointer in a local plus a field offset -- which Ghidra flattens
 *   to `(&DAT_008e450a)[i * 0x98]`. Its local set is also incomplete
 *   ([ebp-0x2c], [ebp-0x34], [ebp-0x48], [ebp-0x4c] are all missing) and its
 *   call arities are wrong: it renders `FUN_0052fac0` with one argument where
 *   0x00523a04's `add esp,8` and the existing Game_Fn52fac0(a1, a2) both say
 *   two, and it moves that second argument onto FUN_00524239 / FUN_005242ad.
 *   The `uVar11 | ... | uVar30` chains are really `|=` accumulation into a
 *   local (see 0x00523a9c, `or [ebp-0x34],eax`). Reconstructing this needs a
 *   0x98-byte record type derived from the disassembly, which is separate work.
 *
 * 0x0052f012: the decompilation is self-evidently broken -- `unaff_EBX` read
 *   as a loop bound, `iStackY_30 = 0x52f18f` (a return address stored into a
 *   local), and `auStackY_3178` / `aiStackY_3088` pseudo-locals, all signs
 *   that Ghidra lost ESP tracking across the two calls that take large
 *   by-value arguments. It also has 29 distinct frame slots against 13
 *   declared locals. There is no structure here to translate.
 */
