/* Ordinary Shandalar game code (gamewire-reconstructed): plain .text
 * functions, neither card scripts nor their helpers. Address-derived names;
 * reccmp is the correctness check. */
#include "game/gamefns.h"

// FUNCTION: SHANDALAR 0x005000fb
int Game_Fn5000fb(int a1)
{
    g_game746ef0[a1] = 0;
    g_game5b80e0[a1] = 1;
    return 0;
}

// FUNCTION: SHANDALAR 0x00500129
int Game_Fn500129(int a1)
{
    g_game5b80e0[g_game58b584] = a1;
    return 1;
}

// FUNCTION: SHANDALAR 0x0050014e
int Game_Fn50014e(int a1, int a2, int a3)
{
    int iv;   /* [ebp-4] */
    int ig;   /* [ebp-8] */

    g_game747ee0 = 1;
    ig = g_game746ef0[a3];
    iv = 0;
    while (iv < a2) {
        g_game746f40[a3][ig] = iv * 84 + a1;
        ig++;
        iv++;
    }
    g_game746ef0[a3] += a2;
    g_game747ee0 = 0;
    return g_game746ef0[a3];
}

// FUNCTION: SHANDALAR 0x00500321
int Game_Fn500321(void)
{
    g_game58b584++;
    g_game58b954 = -1;
    g_game58b958 = -1;
    Game_Fn5000fb(g_game58b584);
    return g_game58b584;
}

// FUNCTION: SHANDALAR 0x0050035e
int Game_Fn50035e(void)
{
    Game_Fn5000fb(g_game58b584);
    if (g_game58b584 == 0) {
        g_game58b584 = 0;
    } else {
        g_game58b584--;
    }
    g_game58b954 = -1;
    g_game58b958 = -1;
    return g_game58b584;
}

// FUNCTION: SHANDALAR 0x00500ee0
int Game_Fn500ee0(void)
{
    int iv;   /* [ebp-4] */

    return 0;
}

// FUNCTION: SHANDALAR 0x005019ad
int Game_Fn5019ad(int a1)
{
    int iv;   /* [ebp-4] */

    g_game602fb4 = 0;
    g_game5b8138 = g_game602fb4;
    Game_Helper46ed33();
    iv = Game_Helper501b7d();
    if (iv != -1) {
        if (a1 == -1) {
            Game_Fn46ed03();
            a1 = Game_Fn412bff(0x78cf10, 0x30, 0x20);
            Game_Helper46ed33();
        }
        if (a1 != -1) {
            g_game74cfe4 = a1;
            g_game58c03f = Game_Fn4ece40(a1);
            if (Game_Fn501aec(0x58c038) != 0) {
                if (g_game602fb4 == 0) {
                    Game_Helper57e820(0x78cf10, 0x58c190);
                } else {
                    Game_Helper57e820(0x78cf10, 0x58c1a8);
                    Game_Helper5797e0(g_game5832b4, 0x40, 0x7f, 0xc0, 0x22, 0xc);
                }
                if (g_game602fb4 == 0xd) {
                    Game_Helper57e826(0x78cf10, 0x58c1bc);
                }
                if (g_game602fb4 == 0x1c) {
                    Game_Helper57e826(0x78cf10, 0x58c1d4);
                }
                Game_Helper57e826(0x78cf10, 0x58c1e4);
            }
        }
    }
    return Game_Fn46ed03();
}

// FUNCTION: SHANDALAR 0x00501aec
int Game_Fn501aec(int a1)
{
    Game_Helper57e820(0x78cf10, 0x58c200);
    Game_Helper57e826(0x78cf10, 0x58c204);
    Game_Helper5020fe(a1);
    return 1;
}

// FUNCTION: SHANDALAR 0x00501e32
int Game_Fn501e32(int a1)
{
    return 0;
}

// FUNCTION: SHANDALAR 0x0050318e
int Game_Fn50318e(void)
{
    return Game_Helper4a8da8(1);
}

// FUNCTION: SHANDALAR 0x005031a8
int Game_Fn5031a8(void)
{
    return Game_Helper4a8da8(0);
}

// FUNCTION: SHANDALAR 0x005081e0
int Game_Fn5081e0(int a1)
{
    return g_game780820[a1];
}

// FUNCTION: SHANDALAR 0x00508bf3
int Game_Fn508bf3(int a1, int a2, int a3, int a4, int a5, int a6)
{
    if (a3 < a1
        && a5 + a3 > a1
        && a4 < a2
        && a6 + a4 > a2) {
        return 1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0050a56d
int Game_Fn50a56d(void)
{
    int iv;   /* [ebp-4] */
    int ig;   /* [ebp-8] */

    ig = 1;
    for (iv = 0; iv < 6; iv++) {
        if ((g_game789904 & 0x13) != 0) {
            ig++;
        }
    }
    return ig;
}
