/* Deck.exe -- the deck builder launcher.
 *
 * The whole of what the authors wrote for this program: 112 bytes at
 * 0x00401010, between the section start and WinMainCRTStartup at 0x004010b0.
 * Everything else in its 5,252-byte .text is CRT startup linked from the
 * library, which is not ours to reconstruct.
 *
 * It is a single-instance guard with a shell protocol. If the game is already
 * running, Deck.exe does not open a second deck builder -- it posts WM_USER
 * with wParam 3 to the existing window and exits, letting the running game open
 * the builder itself. The `/MTGshell` switch is how the game says "I launched
 * you", which is what stops that from bouncing back and forth forever.
 *
 * Three facts about the product come out of this and are recorded in
 * docs/formats: the switch, the message number, and DeckBuilderMain's arity.
 * The binary carries its own build path, D:\NEWMAGIC\sources\deckexe\Debug\,
 * so this was a Debug build -- /Od, the same flags the game layer uses.
 */
#include <windows.h>
#include <string.h>

/* Exported by DECKDLL.dll; the original reaches it through an import thunk at
 * 0x004010aa. Arity is what the call site pushes, and nothing cleans the stack
 * afterwards, so it is __stdcall.
 *
 * Stubbed here rather than imported: this build is one DLL, and linking against
 * the real DECKDLL would tie the measurement to a second binary. The call site
 * is `call rel32` either way -- to a thunk in the original, to the stub here. */
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
