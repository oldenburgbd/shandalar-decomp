/* Card-script helpers in the 0x0046xxxx range, replicated from the
 * archived reconstruction whose per-function reccmp scores were verified.
 * Names are addresses; meaning is not yet established.
 */
#include "game/helpers.h"

// FUNCTION: SHANDALAR 0x0046db2e
int Game_Helper46db2e(void)
{
    if (g_game7bf4c8 == 0 || g_game7a82dc != 0) {
        return 0;
    }
    g_game7a82dc = 1;
    g_game8c83f8 |= 0x200;
    Game_Helper4105e0(0xfffffffe, g_game94d464, 0x9307c0, 0x70);
    g_game8c83f8 &= ~0x200;
    Game_Helper410efc(g_game7a82e0, 0xd6, 0x930d00, 0);
    Game_Helper410efc(g_game7a82e0, 0xd5, 0x8bd0d0, 0);
    g_game7bf4c8 = 0;
    g_game7a82dc = 0;
    Game_Helper558aea(0, 0xff);
    return 0;
}

// FUNCTION: SHANDALAR 0x0046e4d0
int Game_Helper46e4d0(int a1, int a2)
{
    int iv;
    int ig;
    int ih;

    iv = g_gameSlots[a1][a2].field3c;
    if ((g_gameSlots[a1][a2].flags08 & 0x1000) != 0) {
        ig = 1;
    } else {
        ig = 0;
    }
    g_game7be9e8[ig] |= g_gameCards[iv].flags;
    for (ih = 0; ih < 0x1f4; ih++) {
        if (g_game94d930[ig][ih] == -1) {
            g_game94d930[ig][ih] = iv;
            break;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0046d8c1
int Game_Helper46d8c1(int a1, int a2, int a3)
{
    int v4;

    if (a1 == -1 || a2 == -1) {
        return 0;
    } else {
        if ((g_gameSlots[a1][a2].flags18 & 0x80) != 0) {
            return 0;
        } else {
            g_gameSlots[a1][a2].flags18 |= 0x80;
            v4 = g_gameSlots[a1][a2].field6c;
            if (v4 == -1) {
                return 0;
            } else {
                if ((g_gameSlots[a1][a2].flags08 & 0x2) == 0) {
                    a3 = 3;
                }
                if ((g_gameSlots[a1][a2].flags18 & 0x8) != 0 || a3 == 3 || a3 == 4 || (g_gameCards[v4].flags & 0x43) == 0 || g_gameCards[v4].flags == 0x80) {
                    g_gameSlots[a1][a2].field68 = a3;
                    return Game_Helper46dbed(a1, a2);
                } else {
                    g_gameSlots[a1][a2].field68 = a3;
                    g_gameSlots[a1][a2].flags08 |= 2;
                    g_gameSlots[a1][a2].field14 = 0xd6;
                    g_game7bf4c8 = (int)Game_Callback46dbed;
                    return a1 * 45300;
                }
            }
        }
    }
}
