# Deck.exe — the deck builder launcher and its shell protocol

**Established 2026-07-29** by reconstructing the whole of `Deck.exe`'s hand-written code
(`src/deck/deck.c`, one function, 112 bytes, 100% matching).

`Deck.exe` is not the deck builder. It is a 5 KB launcher whose entire job is to decide whether
the deck builder should open *here* or in the already-running game:

```c
hwnd = FindWindowA(NULL, "Magic: The Gathering");
if (hwnd != NULL && _strnicmp(lpCmdLine, "/MTGshell", 9) != 0) {
    PostMessageA(hwnd, WM_USER, 3, 0);
    return 0;
}
DeckBuilderMain(0, 2, 1);
```

Everything else in its 5,252-byte `.text` is CRT startup linked from the library.

## The protocol

| fact | value |
| --- | --- |
| window the game registers | title `Magic: The Gathering`, found by title only (class is `NULL`) |
| "open the deck builder" message | `WM_USER` (0x0400), `wParam` 3, `lParam` 0 |
| switch meaning "the game launched me" | `/MTGshell`, matched case-insensitively on the first 9 characters |
| deck builder entry point | `DeckBuilderMain(0, 2, 1)`, `__stdcall`, exported from `DECKDLL.dll` |

The `/MTGshell` switch is what stops the protocol looping. Without it, a `Deck.exe` started by the
running game would find that game's window and post the message straight back to it. The prefix
match is why it is a *prefix* — the game appends whatever else it needs after the switch.

Note the guard is `hwnd != NULL && not /MTGshell`. Both paths fall through to `DeckBuilderMain`,
so `Deck.exe` run on its own, with no game running, simply opens the builder.

## Consequences for the port

- `DECKDLL.dll` is 189,830 bytes of `.text` and is reached through exactly one exported entry
  point. That makes it unusually approachable for its size: there is one way in.
- `WM_USER + n` is the game's own message space. `3` is the deck builder; other values are
  presumably other shells, and are worth watching for when `Magic.exe`'s window procedure is
  reconstructed.
- The binary carries `D:\NEWMAGIC\sources\deckexe\Debug\deck.pdb`, so it shipped as a **Debug**
  build — `/Od`, the same flags the game layer already uses, which is why the reconstruction
  matched byte-for-byte on the first attempt.

## Why the product metric does not show it

`progress.ps1` credits a binary for functions it can prove **byte-identical** to our recompiled
code. `Deck_WinMain` is not byte-identical: it bakes in our own string and import-thunk addresses.
It is identical after reccmp's address mapping, which is what the `DECK` target reports and the
product page cannot see. 112 bytes is not worth changing the metric over, but the same will be
true of any binary reconstructed from its own annotations rather than shared byte-for-byte with
`Shandalar.exe`, and that is worth knowing before anyone reads 0.00% as "nothing done".
