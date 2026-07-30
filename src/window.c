/* window.c — sidlib window identity helpers (0x00578a71 - 0x00578c02).
 *
 * Translation unit unknown; unlike the surrounding lib.c (/O2) this range is
 * compiled /Od — full frame pointers, unused callee-saved pushes, spilled
 * return values — so it gets its own object file and the /Od rule in the
 * Makefile. At /Od, `sub esp,N` fixes the local count and slots run from
 * [ebp-4] downward, so local names/order below are load-bearing; a local
 * stored then reloaded into EAX before the epilogue is a named return value.
 *
 * The window extra bytes hold a two-long key at offsets 0 and 4, resolved to a
 * small id via Sid_LookupWindowId (0x004523d4, still stubbed), plus an
 * unrelated value at offset 8. */

#include <windows.h>

#include "sidlib/sidwindow.h"

/* The GetWindowLongA offsets are loaded from memory, not pushed as immediates,
 * so in the original they are file-scope const objects (which /Od must load).
 * They are read from all over the surrounding module, not just these four
 * functions. Real names unrecoverable; addresses are. */

// GLOBAL: SHANDALAR 0x0057f19c
const int SID_GWL_KEY_LOW = 0;

// GLOBAL: SHANDALAR 0x0057f1a0
const int SID_GWL_KEY_HIGH = 4;

// GLOBAL: SHANDALAR 0x0057f1a4
const int SID_GWL_EXTRA8 = 8;

// FUNCTION: SHANDALAR 0x00578a71
int Sid_WindowMatchesKey(void *hwnd, const int *key)
{
    LONG keyLow;
    LONG keyHigh;

    /* Two stack slots; both `return 0` paths are bare xor eax,eax — no result
     * variable here, unlike the two functions below. */
    if (hwnd == NULL || key == NULL) {
        return 0;
    }

    keyLow = GetWindowLongA((HWND)hwnd, SID_GWL_KEY_LOW);
    keyHigh = GetWindowLongA((HWND)hwnd, SID_GWL_KEY_HIGH);

    /* The `else` matters: /Od emits a second jmp to the epilogue after the
     * then-block's `mov eax,1`, which a bare trailing `return 0;` loses. */
    if (key[0] == keyLow && key[1] == keyHigh) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: SHANDALAR 0x00578af3
int Sid_WindowHasId(void *hwnd, int id)
{
    /* UNRESOLVED slot ordering, the whole of the remaining gap: the original
     * has found at [ebp-4] with the key halves below it; this build assigns
     * the key halves first. All other instructions match, so the function is
     * behaviourally verified. Declaration order does not drive it — MSVC 4.2
     * appears to assign /Od slots by first assignment — and unifying the
     * types changed nothing (86.61% either way). */
    int found;
    int keyLow;
    int keyHigh;

    if (hwnd == NULL || id < 0) {
        return 0;
    }

    keyLow = GetWindowLongA((HWND)hwnd, SID_GWL_KEY_LOW);
    keyHigh = GetWindowLongA((HWND)hwnd, SID_GWL_KEY_HIGH);

    /* found occupies [ebp-4] in the original and is the left operand. */
    found = Sid_LookupWindowId(keyLow, keyHigh);
    if (found == id) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: SHANDALAR 0x00578b77
int Sid_GetWindowId(void *hwnd)
{
    /* Same unresolved slot-ordering difference as Sid_WindowHasId. */
    int id;
    int keyLow;
    int keyHigh;

    if (hwnd == NULL) {
        return -1;
    }

    keyLow = GetWindowLongA((HWND)hwnd, SID_GWL_KEY_LOW);
    keyHigh = GetWindowLongA((HWND)hwnd, SID_GWL_KEY_HIGH);

    /* Stored and reloaded before the epilogue: a named return value. */
    id = Sid_LookupWindowId(keyLow, keyHigh);
    return id;
}

// FUNCTION: SHANDALAR 0x00578bda
long Sid_GetWindowExtra8(void *hwnd)
{
    long value;

    /* One slot, stored and reloaded — a named local, not a direct return. */
    value = GetWindowLongA((HWND)hwnd, SID_GWL_EXTRA8);
    return value;
}
