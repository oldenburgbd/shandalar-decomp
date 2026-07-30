/* host/shell.c -- the runnable host for the reconstruction.
 *
 * NOT A DECOMPILATION. Nothing here carries a FUNCTION marker and nothing here
 * is scored. This file exists so that the reconstructed code can be *run*
 * rather than only measured.
 *
 * WHY THIS IS NOT WinMain
 *
 * The original's WinMain is at 0x004cdd70 (2,195 bytes, /Od) and its window
 * setup is reproduced below value for value -- the class name, the class style,
 * the icon and cursor, the black stock brush, the window style and the extended
 * style all come from that function's disassembly and not from taste. What is
 * NOT reproduced is everything WinMain does around them: it loads AdvStrings.txt
 * and AdvBlocks.txt and bails to a MessageBox if either fails, opens three CVL
 * archives, registers six TrueType faces, starts a multimedia timer, and spawns
 * the game on a worker thread at 0x0046e6f0 before pumping messages. None of
 * that is reconstructed yet, so a faithful WinMain would exit at its first gate.
 *
 * So this drives the same display path directly:
 *
 *     Sid_CreateScreenPage   0x00578e80   page 0, the palette, the screen DC
 *     Sid_AllocPage          0x00579050   an 8bpp offscreen DIB page
 *     Sid_OpenFileCore       0x0057c580   decode a .pic into that page
 *     Sid_SetPalette         0x0057a060   install the image's palette
 *     Sid_PutLine            0x00579f10   one decoded row at a time
 *     Sid_BlitPageToScreen   0x00579400   page 1 -> the screen
 *
 * Every one of those is reconstructed code. The only non-reconstructed thing in
 * the chain is MPS_CODE, which is hand-written assembly and never a target --
 * see host/mps.c. So what appears on screen is produced by the decompilation,
 * not by a viewer written alongside it.
 *
 *     build\Shandalar.exe [picture.pic]
 *
 * defaulting to Title.pic. The game directory comes from SHANDALAR_GAME_DIR,
 * matching the rest of the tooling. The install is only ever opened for reading.
 */

#include <direct.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "sidlib/sidcore.h"
#include "sidlib/sidfileio.h"
#include "sidlib/sidgfx.h"

/* Read off the original's RegisterClassA block at 0x004cdfd6..0x004ce036 and
 * its CreateWindowExA at 0x004ce2f6. The class style is 0x23 and the window
 * style 0x80000000; both are spelled out here so the values stay checkable
 * against the disassembly. */
#define HOST_CLASS_NAME  "ShandalarMainClass"     /* 0x00589fdc / 0x0058a08c */
#define HOST_WINDOW_NAME "Magic: Shandalar"       /* 0x0058a078             */
#define HOST_CLASS_STYLE (CS_VREDRAW | CS_HREDRAW | CS_OWNDC)          /* 0x23 */
#define HOST_WINDOW_STYLE WS_POPUP                                     /* 0x80000000 */
#define HOST_EX_STYLE    WS_EX_TOPMOST                                 /* 0x08 */

/* The page the picture is decoded into. Page 0 is the screen. */
#define HOST_PAGE 1

static int g_hostWidth = 640;
static int g_hostHeight = 480;
static int g_hostLoaded;

/* Stage tracing. A WINDOWS-subsystem process has no console, and the failure
 * mode being chased here is an access violation inside reconstructed code, so
 * the trace has to survive the process dying: append and flush every line.
 * SHANDALAR_HOST_LOG names the file; unset means no tracing at all. */
static void host_trace(const char *fmt, long a, long b)
{
    const char *path = getenv("SHANDALAR_HOST_LOG");
    FILE *fp;

    if (path == NULL || path[0] == '\0') {
        return;
    }
    fp = fopen(path, "a");
    if (fp == NULL) {
        return;
    }
    fprintf(fp, fmt, a, b);
    fputc('\n', fp);
    fclose(fp);
}

/* ---------------------------------------------------------------------------
 * The image extent has to be known before the window and the page are made,
 * because Sid_PutLine memcpy's g_sidImageWidth bytes into a row of the page and
 * would run off the end of a page sized to a guess. This reads the block chain
 * far enough to find the image header and nothing more -- the decode itself is
 * still done by the reconstructed path.
 * ------------------------------------------------------------------------ */
static int host_pic_size(const char *path, int *width, int *height)
{
    FILE *fp;
    unsigned char hdr[9];
    long off = 0;

    fp = fopen(path, "rb");
    if (fp == NULL) {
        return 0;
    }

    for (;;) {
        long length;

        if (fseek(fp, off, SEEK_SET) != 0 || fread(hdr, 1, 4, fp) != 4) {
            break;
        }
        length = (long)hdr[2] | ((long)hdr[3] << 8);

        if (hdr[0] == 'X' && (hdr[1] == '0' || hdr[1] == '1')) {
            if (fread(hdr, 1, 5, fp) != 5) {
                break;
            }
            *width = (int)hdr[0] | ((int)hdr[1] << 8);
            *height = (int)hdr[2] | ((int)hdr[3] << 8);
            fclose(fp);
            return (*width > 0 && *height > 0);
        }
        if (hdr[0] != 'M' || (hdr[1] != '0' && hdr[1] != '1')) {
            break;
        }
        off += 4 + length;      /* palette blocks are always under 64 KB */
    }

    fclose(fp);
    return 0;
}

/* Reconstructed code cannot be instrumented -- adding a trace call to lib.c
 * would re-roll MSVC's register allocation across that whole translation unit
 * and move every score in it. So faults inside it are located from the outside:
 * this records the faulting address, which `build\host.map` then resolves to a
 * function. */
static LONG WINAPI HostExceptionFilter(EXCEPTION_POINTERS *ep)
{
    host_trace("EXCEPTION %08lx at %08lx",
               (long)ep->ExceptionRecord->ExceptionCode,
               (long)ep->ExceptionRecord->ExceptionAddress);
    return EXCEPTION_EXECUTE_HANDLER;
}

static LRESULT CALLBACK HostWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    PAINTSTRUCT ps;

    switch (msg) {
    case WM_PAINT:
        BeginPaint(hwnd, &ps);
        if (g_hostLoaded) {
            /* Reconstructed: BitBlt from the offscreen page's DC to page 0's,
             * which is the DC taken from this window below. */
            Sid_BlitPageToScreen(HOST_PAGE);
        }
        EndPaint(hwnd, &ps);
        return 0;

    /* Only meaningful on a palettised display, which no longer exists in
     * practice -- Sid_SetPalette has already pushed the colours into each
     * page's DIB colour table, which is what makes the image correct at 32bpp.
     * Kept because it costs two lines and the original handles them. */
    case WM_QUERYNEWPALETTE:
    case WM_PALETTECHANGED:
        if (g_sidPalette != NULL && g_sidScreenDC != NULL) {
            SelectPalette((HDC)g_sidScreenDC, (HPALETTE)g_sidPalette, FALSE);
            RealizePalette((HDC)g_sidScreenDC);
            InvalidateRect(hwnd, NULL, FALSE);
        }
        return (msg == WM_QUERYNEWPALETTE);

    case WM_KEYDOWN:
        if (wp != VK_ESCAPE) {
            break;
        }
        /* fall through */
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        break;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

static void host_fail(const char *what, const char *detail)
{
    char msg[512];

    sprintf(msg, "%s\n\n%s", what, detail ? detail : "");
    MessageBoxA(NULL, msg, "Shandalar (decomp host)", MB_OK | MB_ICONERROR);
}

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nShow)
{
    WNDCLASSA wc;
    HWND hwnd;
    MSG msg;
    SidPaletteBlock palette;
    const char *dir;
    const char *pic;
    char path[MAX_PATH];

    (void)hPrev;

    SetUnhandledExceptionFilter(HostExceptionFilter);

    pic = (lpCmdLine != NULL && lpCmdLine[0] != '\0') ? lpCmdLine : "Title.pic";

    dir = getenv("SHANDALAR_GAME_DIR");
    if (dir == NULL || dir[0] == '\0') {
        host_fail("SHANDALAR_GAME_DIR is not set.",
                  "Point it at the game install, e.g.\r\n"
                  "set SHANDALAR_GAME_DIR=C:\\...\\MtG_DotP_SotA");
        return 1;
    }
    if (_chdir(dir) != 0) {
        host_fail("Could not enter the game directory.", dir);
        return 1;
    }

    /* Windows hands WinMain the raw command tail, which routinely carries
     * trailing whitespace. _open tolerates it; the trace and the error messages
     * should not show it. */
    strncpy(path, pic, sizeof(path) - 1);
    path[sizeof(path) - 1] = '\0';
    {
        int n = (int)strlen(path);
        while (n > 0 && (path[n - 1] == ' ' || path[n - 1] == '\t')) {
            path[--n] = '\0';
        }
    }

    if (_access(path, 4) != 0) {
        host_fail("Picture not found in the game directory.", path);
        return 1;
    }
    if (!host_pic_size(path, &g_hostWidth, &g_hostHeight)) {
        host_fail("Not a PICv3 image (no X0/X1 block).", path);
        return 1;
    }

    /* --- the window, as the original builds it ---------------------------- */
    memset(&wc, 0, sizeof(wc));
    wc.style = HOST_CLASS_STYLE;
    wc.lpfnWndProc = HostWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIconA(hInstance, MAKEINTRESOURCEA(101));
    wc.hCursor = LoadCursorA(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = HOST_CLASS_NAME;

    if (!RegisterClassA(&wc)) {
        host_fail("Couldn't register the classes", NULL);
        return 1;
    }

    hwnd = CreateWindowExA(HOST_EX_STYLE, HOST_CLASS_NAME, HOST_WINDOW_NAME,
                           HOST_WINDOW_STYLE, 0, 0, g_hostWidth, g_hostHeight,
                           NULL, NULL, hInstance, NULL);
    if (hwnd == NULL) {
        host_fail("CreateWindowEx failed.", NULL);
        return 1;
    }
    ShowWindow(hwnd, nShow);
    host_trace("window ok %ldx%ld", g_hostWidth, g_hostHeight);

    /* WinMain stores this DC in the same global at 0x004ce344, and
     * Sid_CreateScreenPage takes page 0's hdc straight out of it. */
    g_sidScreenDC = GetDC(hwnd);
    host_trace("got dc %ld %ld", (long)g_sidScreenDC, 0L);

    /* --- from here down, every call is reconstructed code ----------------- */
    if (Sid_CreateScreenPage(g_hostWidth, g_hostHeight, -1) == NULL) {
        host_fail("Sid_CreateScreenPage failed.", NULL);
        return 1;
    }
    host_trace("screen page ok, bpp %ld", (long)g_sidScreenBpp, 0L);

    /* Sid_AllocPage builds the page and returns it, but deliberately does NOT
     * publish it: the page table is written by a separate function, 0x00579310,
     * which frees whatever occupied the slot and then stores unconditionally.
     * Every call site in the original is the pair
     *
     *     p = Sid_AllocPage(i, w, h, 8);   /_ 0x00579050 _/
     *     Sid_SetPage(i, p);               /_ 0x00579310 _/
     *
     * (see the game thread at 0x0046ea97/0x0046eacd). 0x00579310 is not
     * reconstructed yet -- it is 227 bytes and is mostly the same teardown as
     * Sid_FreePage -- so the store is done here instead. The slot is empty on
     * this path, so the teardown half has nothing to do and only the store
     * matters. Reconstructing it is the natural next function to take.
     *
     * Everything downstream depends on this: Sid_PutLine, Sid_SetPalette and
     * Sid_BlitPageToScreen all reach the page through g_sidPages[]. */
    g_sidPages[HOST_PAGE] = Sid_AllocPage(HOST_PAGE, g_hostWidth, g_hostHeight, 8);
    if (g_sidPages[HOST_PAGE] == NULL) {
        host_fail("Sid_AllocPage failed.", NULL);
        return 1;
    }
    host_trace("alloc page ok, bits %ld", (long)g_sidPages[HOST_PAGE]->pBits, 0L);

    /* The page must exist before the file is opened: Sid_SetPalette pushes the
     * new colours into every allocated page's DIB colour table, and a page
     * created afterwards would keep the all-black table Sid_CreateScreenPage
     * realized. */
    memset(&palette, 0, sizeof(palette));
    host_trace("opening %s", (long)path, 0L);
    Sid_OpenFileCore(HOST_PAGE, 0, 0, path, &palette);
    host_trace("decoded %ldx%ld", (long)g_sidImageWidth, (long)g_sidImageHeight);
    g_hostLoaded = 1;

    /* Differential hook. SHANDALAR_HOST_DUMP names a file to write the decoded
     * page into, so the reconstruction's output can be diffed against the C#
     * oracle's decode of the same file rather than eyeballed. Layout:
     *
     *   int32 width, int32 height, 1024 bytes g_sidPaletteRgb, width*height indices
     *
     * The indices come straight out of the page's DIB bits, which is what
     * Sid_PutLine wrote row by row. */
    {
        const char *dump = getenv("SHANDALAR_HOST_DUMP");

        if (dump != NULL && dump[0] != '\0') {
            FILE *fp = fopen(dump, "wb");

            if (fp != NULL) {
                SidPage *pg = g_sidPages[HOST_PAGE];
                int row;

                fwrite(&g_hostWidth, sizeof(int), 1, fp);
                fwrite(&g_hostHeight, sizeof(int), 1, fp);
                fwrite(g_sidPaletteRgb, 1, sizeof(g_sidPaletteRgb), fp);
                for (row = 0; row < g_hostHeight; row++) {
                    fwrite(SID_PIXEL_AT(pg, 0, row), 1, (size_t)g_hostWidth, fp);
                }
                fclose(fp);
                host_trace("dumped %ld bytes of indices",
                           (long)g_hostWidth * g_hostHeight, 0L);
            }
            return 0;
        }
    }

    Sid_BlitPageToScreen(HOST_PAGE);
    host_trace("blit ok", 0L, 0L);
    InvalidateRect(hwnd, NULL, FALSE);

    while (GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    ReleaseDC(hwnd, (HDC)g_sidScreenDC);
    return (int)msg.wParam;
}
