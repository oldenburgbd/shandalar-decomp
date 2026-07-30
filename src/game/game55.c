/* Ordinary Shandalar game code in the 0x55xxxx range, reconstructed by
 * gamewire. Not card scripts and not their helpers -- the rest of .text.
 * Pattern-matched from the disassembly; the reccmp score is the check.
 * Address names only -- nothing here establishes meaning yet.
 */
#include "game/gamefns.h"

// FUNCTION: SHANDALAR 0x005501dc
int Game_Fn5501dc(int a1)
{
    return a1 * g_game5863b8 / 0x140;
}

// FUNCTION: SHANDALAR 0x005501fe
int Game_Fn5501fe(int a1)
{
    return a1 * g_game5863b8 / 0x280;
}

// FUNCTION: SHANDALAR 0x00557a3a
int Game_Fn557a3a(int a1, int a2)
{
    int iv;   /* [ebp-4] */

    if (a1 == -1 || a2 == -1) {
        return -1;
    }
    iv = Game_Helper5579fc(a1, a2);
    if (iv == -1) {
        return -1;
    }
    return g_gameCards[iv].mask;
}

// FUNCTION: SHANDALAR 0x005582ed
int Game_Fn5582ed(int a1, int a2)
{
    if (g_game8bd200 == 1) {
        return 0;
    }
    return Game_Helper450110(a1, a2);
}

// FUNCTION: SHANDALAR 0x0055837e
int Game_Fn55837e(int a1, int a2, int a3, int a4, int a5)
{
    if (g_game8bd200 == 1) {
        return 1;
    }
    return Game_Helper53a054(a1, 0, 0, a2, a3, a4, a5);
}

// FUNCTION: SHANDALAR 0x005584d5
int Game_Fn5584d5(int a1, int a2, int a3, int a4)
{
    if (g_game8bd200 == 1) {
        return 0;
    }
    if (g_game77e58c != 0) {
        return Game_Helper450925(a1, -1, -1, a2, a3, a4);
    }
    return Game_Helper42811a(a1, a2, a3, a4);
}

// FUNCTION: SHANDALAR 0x00558aa3
int Game_Fn558aa3(int a1)
{
    if (g_game8bd200 == 1) {
        return 0;
    }
    if (g_game77e58c != 0) {
        return Game_Fn4509be(a1);
    }
    return Game_Fn42804f(a1);
}

// FUNCTION: SHANDALAR 0x005597ca
int Game_Fn5597ca(void)
{
    if (g_game8bd200 == 1) {
        return 0;
    }
    if (g_game77e58c != 0) {
        return 0;
    }
    return Game_Helper4ed005();
}

// FUNCTION: SHANDALAR 0x00559e52
int Game_Fn559e52(void)
{
    int iv;   /* [ebp-4] */

    iv = Game_Helper559dcd();
    return (iv - g_game669390) * 100 / 0x151d;
}

// FUNCTION: SHANDALAR 0x0055d8fa
int Game_Fn55d8fa(int a1, int a2, int a3)
{
    return 0;
}

// FUNCTION: SHANDALAR 0x00550593
int Game_Fn550593(int a1, int a2)
{
    if (g_game746af8 > a1 || (8 << g_game746b00) + g_game746af8 <= a1) {
        return 0;
    }
    if (g_game746afc > a2 || (6 << g_game746b00) + g_game746afc <= a2) {
        return 0;
    }
    return 1;
}

// FUNCTION: SHANDALAR 0x005578d1
int Game_Fn5578d1(int a1, int a2)
{
    int iv;   /* [ebp-4] */

    iv = Game_Helper5579fc(a1, a2);
    if (iv != -1) {
        if ((g_gameCards[iv].flags3c & 0x20) != 0) {
            return Game_Helper557893(a1, a2) & 0xf;
        }
        return 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00558457
int Game_Fn558457(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9)
{
    if (g_game8bd200 == 1) {
        return 1;
    }
    return Game_Helper53a265(a1, a2, 0, a3, a4, a5, a6, a7, a8, a9);
}

// FUNCTION: SHANDALAR 0x005584ac
int Game_Fn5584ac(int a1)
{
    if (g_game8bd200 == 1) {
        return 0;
    }
    return Game_Fn450836(a1);
}

// FUNCTION: SHANDALAR 0x00558874
int Game_Fn558874(int a1, int a2)
{
    if (g_game8bd200 == 1) {
        return 0;
    }
    if (g_game77e58c != 0) {
        return Game_Fn4509ae(a1, a2);
    }
    return Game_Fn4288f1(a1, a2);
}

// FUNCTION: SHANDALAR 0x005588c3
int Game_Fn5588c3(int a1, int a2, int a3, int a4, int a5, int a6)
{
    if (g_game8bd200 == 1) {
        return a3;
    }
    return Game_Helper53bc1a(a1, a2, a3, a4, a5, a6);
}

// FUNCTION: SHANDALAR 0x00558908
int Game_Fn558908(int a1, int a2, int a3)
{
    if (g_game8bd200 == 1) {
        return a3;
    }
    return Game_Helper53c19a(a1, a2, a3);
}

// FUNCTION: SHANDALAR 0x00559781
int Game_Fn559781(int a1)
{
    if (g_game8bd200 == 1) {
        return 0;
    }
    if (g_game77e58c != 0) {
        return Game_Fn451b3e(a1, 1);
    }
    return Game_Helper56da03(a1);
}
