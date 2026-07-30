/* Deck.exe -- the deck builder launcher.
 *
 * The only author-written code in the program: 112 bytes at 0x00401010 ahead
 * of WinMainCRTStartup; everything else in .text is linked CRT startup.
 *
 * Single-instance guard with a shell protocol: if the game window already
 * exists, post WM_USER wParam 3 to it and exit so the running game opens the
 * builder itself. The `/MTGshell` switch marks "the game launched me" and
 * breaks the loop. Debug build (/Od), per the embedded build path
 * D:\NEWMAGIC\sources\deckexe\Debug\. */
#include <windows.h>
#include <string.h>

/* Exported by DECKDLL.dll, reached through an import thunk at 0x004010aa in
 * the original. The call site pushes three args and cleans nothing, so it is
 * __stdcall. Stubbed rather than imported so the measurement stays within one
 * binary; the call is `call rel32` either way. */
int __stdcall DeckBuilderMain(int a1, int a2, int a3);

// STUB: DECK 0x004010aa
int __stdcall DeckBuilderMain(int a1, int a2, int a3)
{
    (void)a1; (void)a2; (void)a3;
    return 0;
}

// FUNCTION: DECK 0x00401010
int __stdcall Deck_WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                           char *lpCmdLine, int nCmdShow)
{
    HWND hwnd;

    (void)hInstance;
    (void)hPrevInstance;
    (void)nCmdShow;

    hwnd = FindWindowA(NULL, "Magic: The Gathering");
    if (hwnd != NULL && _strnicmp(lpCmdLine, "/MTGshell", 9) != 0) {
        PostMessageA(hwnd, WM_USER, 3, 0);
        return 0;
    }
    DeckBuilderMain(0, 2, 1);
    return 0;
}
