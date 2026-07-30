/* Declaration surface adopted from the archived attempt (salvage/decomp/
 * include/sidlib/sidwindow.h); see gameslot.h provenance note. */

/* sidlib - MicroProse in-house 2D engine, window.c: window identity helpers.
 *
 * Original translation units (recovered from compiled-in assert strings):
 *   D:\NewMagic\sources\sidlib\{lib,text,sprite,Fileio,Pcxw}.c
 *
 * Written for Microsoft Visual C++ 4.2, which is C89 only: declarations must
 * precede statements, no // comments in translation units we want to match.
 *
 * ONE HEADER PER MODULE, DELIBERATELY. Every declaration visible to a
 * translation unit perturbs MSVC 4.2's register allocation across that whole
 * unit -- adding six prototypes to the old shared header once took Sid_SetPixel
 * from a 100% match to 31% without touching lib.c at all. Keeping each
 * module's declarations out of its neighbours' sight is what makes per-function
 * scores comparable between builds. Do not merge these back together, and do
 * not include a module header you do not actually need.
 */

#ifndef SIDLIB_WINDOW_H
#define SIDLIB_WINDOW_H

/* --- lib.c: window identity ---------------------------------------------- */

/* sidlib windows carry three longs in their extra bytes: a two-long key at
 * GWL offsets 0 and 4 that resolves to a small integer id, and an unrelated
 * value at offset 8.
 *
 * These four sit just below Sid_CreateScreenPage and above the last function
 * we can attribute by assert string, so assigning them to lib.c is an
 * inference from position, not evidence. */

/* Resolves a window key pair to its id. Lives outside sidlib at 0x004523d4
 * and is not yet reconstructed. */
int Sid_LookupWindowId(long keyLow, long keyHigh);

/* Non-zero if the window's stored key equals the two longs at `key`. */
int Sid_WindowMatchesKey(void *hwnd, const int *key);

/* Non-zero if the window's key resolves to `id`. Rejects negative ids. */
int Sid_WindowHasId(void *hwnd, int id);

/* The window's id, or -1 for a NULL window. */
int Sid_GetWindowId(void *hwnd);

/* The third extra long (GWL offset 8); purpose not yet established. */
long Sid_GetWindowExtra8(void *hwnd);

#endif /* SIDLIB_WINDOW_H */
