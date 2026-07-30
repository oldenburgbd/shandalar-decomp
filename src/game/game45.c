/* Shandalar game code, addresses 0x0045xxxx. Reconstructed against the
 * original binary; formulations follow the measured-good salvage text.
 */
#include "game/gamefns.h"

// FUNCTION: SHANDALAR 0x004509ae
int Game_Fn4509ae(int a1, int a2)
{
    return 0;
}

// FUNCTION: SHANDALAR 0x004509be
int Game_Fn4509be(int a1)
{
    return 0;
}

// FUNCTION: SHANDALAR 0x00451b3e
int Game_Fn451b3e(int a1, int a2)
{
    return 0;
}

// FUNCTION: SHANDALAR 0x00451b4e
int Game_Fn451b4e(void)
{
    return 0;
}

// FUNCTION: SHANDALAR 0x00451f3b
int Game_Fn451f3b(int a1, int a2)
{
    if (a1 != 0
        && a1 != 1) {
        return 1;
    }
    if (a2 < 0 || a2 > 0x96) {
        return 1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00452973
int Game_Fn452973(int a1, int a2)
{
    int iv;   /* [ebp-4] */

    if (Game_Fn451f3b(a1, a2) != 0) {
        return 0;
    }
    iv = Game_Helper452355(a1, a2);
    if (iv == -1) {
        return 0;
    }
    return g_gameCards[iv].flags;
}

// FUNCTION: SHANDALAR 0x00452e85
int Game_Fn452e85(void)
{
    return 0;
}

// FUNCTION: SHANDALAR 0x00452efb
int Game_Fn452efb(int a1, int a2)
{
    int iv;   /* [ebp-4] */
    int ig;   /* [ebp-8] */

    if (Game_Fn451f3b(a1, a2) != 0) {
        return 0;
    }
    iv = Game_Helper452355(a1, a2);
    if (iv == -1) {
        return 0;
    }
    ig = g_gameCards[iv].flags3c & 0x1000;
    return ig;
}

// FUNCTION: SHANDALAR 0x00453740
int Game_Fn453740(int a1)
{
    if (a1 != 0
        && a1 != 1) {
        return 1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00454311
int Game_Fn454311(int a1, int a2)
{
    int iv;   /* [ebp-4] */

    iv = Game_Helper453f42(a1, a2);
    if (iv != 0) {
        return Game_Helper57e826(0x78cf10, iv);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00450836
int Game_Fn450836(int a1)
{
    return 0;
}

// FUNCTION: SHANDALAR 0x00451ab2
int Game_Fn451ab2(int a1)
{
    int iv;   /* [ebp-4] */

    iv = a1 * 60 / 0x3e8;
    return a1 * 60 / 0x3e8;
}
