/* window.c — sidlib window identity helpers (0x00578a71 - 0x00578c02).
 *
 * TRANSLATION UNIT UNKNOWN. No assert string attributes this range, and unlike
 * lib.c it is compiled WITHOUT optimisation: full frame pointers, callee-saved
 * registers pushed but unused, the return value spilled to a stack slot and
 * reloaded, a `jmp` to the following instruction, and long-form conditional
 * jumps. lib.c immediately above and below it is /O2 (Sid_BuildBitmapInfo
 * matches byte-for-byte at /O2), so this is a separate object file built with
 * different flags — see the /Od rule for this file in the Makefile.
 *
 * The unoptimised build makes two things directly readable from the machine
 * code, and both are used below:
 *
 *   - `sub esp,N` gives the exact number of local slots, and /Od assigns them
 *     from [ebp-4] downward in declaration order. That fixes both how many
 *     locals each function has and what order they were declared in.
 *   - A local that is stored and then immediately reloaded into EAX before the
 *     epilogue is a named return value (`x = f(); return x;`), whereas a bare
 *     `mov eax,imm; jmp epilogue` is a direct `return imm;`. So the presence or
 *     absence of a result variable is visible too.
 *
 * The window extra bytes hold a two-long key at offsets 0 and 4 that resolves
 * to a small integer id via Sid_LookupWindowId (0x004523d4, outside sidlib and
 * still stubbed), plus an unrelated value at offset 8.
 */

#include <windows.h>

#include "sidlib/sidwindow.h"

/* The GetWindowLongA offsets are read from memory rather than pushed as
 * immediates, so in the original they are file-scope objects, not literals or
 * macros. That is ordinary C: a `const int` at file scope is an object and not
 * a constant expression, so an unoptimised build must load it. Their real names
 * are not recoverable; the addresses are.
 *
 * They are read from all over the surrounding module (0x005745bc onwards), not
 * just these four functions, which is further evidence that the /Od translation
 * unit is large and that this file is only a corner of it. */

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

    /* Two stack slots only, and both `return 0` paths are a bare
     * `xor eax,eax; jmp epilogue` — so there is no result variable here,
     * unlike the two functions below. */
    if (hwnd == NULL || key == NULL) {
        return 0;
    }

    keyLow = GetWindowLongA((HWND)hwnd, SID_GWL_KEY_LOW);
    keyHigh = GetWindowLongA((HWND)hwnd, SID_GWL_KEY_HIGH);

    /* The `else` is load-bearing. The original emits two consecutive `jmp`s to
     * the epilogue after `mov eax,1`: the first is the `return 1`, the second
     * is the jump over the else block that /Od emits at the end of every
     * then-block and never peepholes away, even when the then-block has
     * already returned. Writing this as a bare `return 0;` after the if
     * produces only one jump and loses five bytes. */
    if (key[0] == keyLow && key[1] == keyHigh) {
        return 1;
    } else {
        return 0;
    }
}

// FUNCTION: SHANDALAR 0x00578af3
int Sid_WindowHasId(void *hwnd, int id)
{
    /* UNRESOLVED, and the whole of the remaining 16%. The original puts found
     * at [ebp-4] with the key halves below it at [ebp-8] and [ebp-0xc]; our
     * build puts the key halves first and pushes found to [ebp-0xc]. Every
     * instruction matches apart from those displacements, so this function is
     * behaviourally verified either way.
     *
     * Declaration order does not drive it: found is declared first here and
     * still lands last. MSVC 4.2 appears to assign /Od slots in order of first
     * assignment, which would mean the original assigned found before the two
     * GetWindowLongA calls — and there is no store to [ebp-4] before them.
     * Making all three the same type was tried, on the theory that MSVC groups
     * locals by type before assigning slots; it changed nothing (86.61% either
     * way). The types are left uniform because they are compared against an
     * int, not because it fixed anything. */
    int found;
    int keyLow;
    int keyHigh;

    if (hwnd == NULL || id < 0) {
        return 0;
    }

    keyLow = GetWindowLongA((HWND)hwnd, SID_GWL_KEY_LOW);
    keyHigh = GetWindowLongA((HWND)hwnd, SID_GWL_KEY_HIGH);

    /* `found` occupies [ebp-4], so it is declared before the two key halves,
     * and the comparison has it as the left operand. */
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
    /* Same unresolved slot-ordering difference as Sid_WindowHasId above, and
     * again it is the entire remaining gap. */
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

    /* One stack slot, stored and reloaded — again a named local rather than
     * `return GetWindowLongA(...)` directly. */
    value = GetWindowLongA((HWND)hwnd, SID_GWL_EXTRA8);
    return value;
}
