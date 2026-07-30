/* Card-script helpers in the 0x0040xxxx range, replicated from the
 * archived reconstruction whose per-function reccmp scores were verified.
 * Names are addresses; meaning is not yet established.
 */
#include "game/helpers.h"

// FUNCTION: SHANDALAR 0x0040a4b8
int Game_Helper40a4b8(int a1, int a2)
{
    Game_Helper40e3dc(a1, a2, 0x8d, 1 - a1, -1);
    Game_Helper46e4d0(a1, a2);
    g_gameSlots[a1][a2].field6c = -1;
    Game_Helper558321(a1, a2, 0xb, 1);
    if (g_game8bd200 != 1) {
        Game_Helper40ebb9(0x18);
    }
    g_game8e3ec8[a1]--;
    return 0;
}

// FUNCTION: SHANDALAR 0x0040c4e1
int Game_Helper40c4e1(int a1, int a2)
{
    if (g_gameSlots[a1][a2].field6c == -1) {
        return 0;
    }
    if ((g_gameSlots[a1][a2].flags08 & 0x22) != 0) {
        return 0;
    }
    return 1;
}

// FUNCTION: SHANDALAR 0x0040cd94
int Game_Helper40cd94(int a1, int a2)
{
    if ((g_gameSlots[a1][a2].flags08 & 0x2000) != 0) {
        return 1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0040d414
int Game_Helper40d414(int a1, int a2, int a3, int a4)
{
    int iv;
    int ig;
    int ih;

    ih = g_gameSlots[a1][a2].flags08;
    ig = (signed char)g_gameSlots[a1][a2].field24;
    g_gameSlots[a1][a2].flags08 &= ~8;
    g_gameSlots[a1][a2].field24 = 0xff;
    iv = Game_Helper40d0cb(a1, a2, a3, a4);
    if (iv == 0) {
        g_gameSlots[a1][a2].flags08 = ih;
        g_gameSlots[a1][a2].field24 = ig;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0040dffd
int Game_Helper40dffd(int a1)
{
    if ((a1 & 0x2) != 0) {
        return 1;
    }
    if ((a1 & 0x4) != 0) {
        return 2;
    }
    if ((a1 & 0x8) != 0) {
        return 3;
    }
    if ((a1 & 0x10) != 0) {
        return 4;
    }
    if ((a1 & 0x20) != 0) {
        return 5;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0040e158
int Game_Helper40e158(int a1, int a2, int a3)
{
    int v4;
    int v8;

    Game_Helper40e646();
    g_game7a01e0 = 0;
    g_game8e1aa8 = a1;
    g_game951bdc = a2;
    g_game94e928 = 1 - a1;
    g_game8e26b0 = -1;
    v4 = g_game94d178;
    if (a3 != 0x7d
        && a3 != 0x7e) {
        g_game94d178 = -1;
    }
    Game_Helper40e1f5(a3);
    g_game8e1aa8 = -1;
    g_game94d178 = v4;
    v8 = g_game7a01e0;
    Game_Helper40e6fc();
    return 0;
}

// FUNCTION: SHANDALAR 0x00403eeb
int Game_Helper403eeb(void)
{
    int v4;
    int v8;

    if (g_game8c83f0[0] <= 0 || g_game8c83f4 <= 0) {
        v8 = 1;
    } else {
        if (g_game7bfee0[0] >= 0xa || g_game7bfee4 >= 0xa) {
            v8 = 1;
        } else {
            v8 = 0;
        }
    }
    if (v8 != 0) {
        if (g_game8bd200 == 1) {
            if (g_game7bfee0[0] >= 0xa) {
                g_game8c83f0[0] = 0xffffff9d;
            }
            if (g_game7bfee4 >= 0xa) {
                g_game8c83f4 = 0xffffff9d;
            }
            v4 = 0;
        } else {
            Game_Helper558aea(0, 0xff);
            v4 = 1;
        }
    } else {
        v4 = 0;
    }
    return v4;
}
