/* Shandalar game code, 0x004b8102 - 0x004bea85. Derived from Ghidra's
 * decompilation of orig/Shandalar.exe; names are addresses because nothing
 * establishes what these do.
 *
 * 0x004b8102 and 0x004bea85 keep the names and signatures the shared headers
 * already fix for them (all-int parameter lists); where the original narrowed
 * an argument to a byte the int declaration is a known residual and the mask
 * is applied to the int instead.
 *
 * Address literals passed as plain ints (0x589174, 0x8aa920, ...) are pointers
 * into the original image's data -- prompt strings, reason strings, and the
 * scratch buffers at 0x008aa920 / 0x008aab78. Pushing the literal reproduces
 * the original instruction without carrying the copyrighted data.
 */

#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "game/gamefns.h"

/* Referenced by this TU only; not in the shared headers. Addresses from the
 * disassembly. */

/* 0x0074b8b8 -- one when the player has asked for the rest of a repeated
 * choice to be filled in automatically. */
extern int g_game74b8b8;

/* 0x007496a0: 180-byte records, one per portrait slot; only the sprite
 * pointer at +0x00 is touched here. 0x00749310 / 0x00749390 / 0x00748890 are
 * parallel int arrays (frame width, height, and a derived height cap).
 * Extents unknown -- large enough to index. */
extern void *g_game7496a0[32][45];
extern int g_game749310[32];
extern int g_game749390[32];
extern int g_game748890[32];

/* Card masks compared against in the 0x004bea85 predicate. */
extern int g_game7a0910;
extern int g_game7beaf4;
extern int g_game93a7dc;

/* Declared in game/helpers.h and sidlib/sidsprite.h, which this TU
 * deliberately does not include -- one header per module, and every extra
 * declaration in scope re-rolls MSVC 4.2's register allocation for the whole
 * unit. Repeated here with the established names and signatures. */
int Game_Helper4c442a(void);
int Game_Helper558356(int a1);
int Spr_LoadFile(void **frames, const char *path);

/* Not named by any header. Argument counts are from the call sites. */
int Game_Fn486f9a(int a1, int a2, int a3);
int Game_Fn4c0dd4(int a1, int a2);
int Game_Fn4c1695(int a1, int a2);

/* Collects up to `a4` targets into the slot's field74/field78 pair array on
 * event 0x6c and spends them on event 0x71; event 0x74 reports whether the
 * effect is available at all. field36 is the count of targets gathered, and
 * 0x13 is where the pair array runs out. */
// FUNCTION: SHANDALAR 0x004b8102
int Game_Helper4b8102(int a1, int a2, int a3, int a4)
{
    signed char cv;
    int iv;
    char buf[300];
    int ig;
    int ih;
    int ii;
    int ij;
    int ik;

    if (a3 == 0x74) {
        if (a1 == g_game8c7468
            && (g_game93a934 & 0x2) == 0
            && a4 == 0) {
            return 0;
        }
        if ((g_game8c83f8 & 0x4) == 0) {
            return 1;
        }
        return 99;
    }
    if (a3 == 0x6c
        && a2 == g_game951bdc
        && a1 == g_game8e1aa8) {
        g_game94d63c += -0x60;
        if ((g_game8c83f8 & 0x4) == 0) {
            Game_Helper56ca10(0x589174, 0x589164);
            if (Game_Helper4c0efa(a1, 2, a1, 0x1000, 0, 0, 0, 0, 0, 0,
                                  -1, -1, -1, -1, 0, 0, 0,
                                  0x8aa920, 1, (int)&ig) == 0) {
                g_game8c83ec = 1;
            } else {
                g_gameSlots[a1][a2].field74 = ig;
                g_gameSlots[a1][a2].field78 = ih;
                g_gameSlots[a1][a2].field36 = 1;
                g_gameSlots[a1][a2].field38 = a4;
            }
        } else {
            g_gameSlots[a1][a2].field36 = 0;
            cv = -1;
            ik = 0;
            g_game8c83ec = -1;
            ii = 0;
            while ((signed char)g_gameSlots[a1][a2].field36 < a4
                   && ik == 0
                   && g_game8c83ec != 1
                   && ii == 0) {
                Game_Helper56ca10(0x589190, 0x589180);
                sprintf(buf, (char *)0x8aaa4c,
                        (signed char)g_gameSlots[a1][a2].field36 + 1, a4);
                if (Game_Helper4c0efa(a1, 2, a1, 0x200, 0, 0, 0, 0, 0, 0,
                                      g_game93a7d4, -1, -1, -1, 0, 0, 0,
                                      (int)buf, 3, (int)&ig) == 0) {
                    if (ih == -1) {
                        g_game8c83ec = 1;
                    } else {
                        ik = 1;
                    }
                } else if (((signed char)g_gameSlots[ig][ih].field50 == cv
                            && g_gameSlots[ig][ih].field04 == iv)
                           || cv == -1) {
                    cv = (signed char)g_gameSlots[ig][ih].field50;
                    iv = g_gameSlots[ig][ih].field04;
                    g_gameSlots[ig][ih].flags08 |= 0x200000;
                    Game_Helper558aea(0, 0x20);
                    (&g_gameSlots[a1][a2].field74)[(signed char)g_gameSlots[a1][a2].field36 * 2] = ig;
                    (&g_gameSlots[a1][a2].field78)[(signed char)g_gameSlots[a1][a2].field36 * 2] = ih;
                    g_gameSlots[a1][a2].field36++;
                    if (g_gameSlots[a1][a2].field36 == 0x13) {
                        ii++;
                    }
                    if (g_game74b8b8 == 1) {
                        while ((signed char)g_gameSlots[a1][a2].field36 < a4
                               && ii == 0) {
                            (&g_gameSlots[a1][a2].field74)[(signed char)g_gameSlots[a1][a2].field36 * 2] = ig;
                            (&g_gameSlots[a1][a2].field78)[(signed char)g_gameSlots[a1][a2].field36 * 2] = ih;
                            g_gameSlots[a1][a2].field36++;
                            if (g_gameSlots[a1][a2].field36 == 0x13) {
                                ii = 1;
                            }
                        }
                    }
                } else if (g_game8bd200 == 1) {
                    Game_Helper4c442a();
                } else {
                    Game_Helper56ca10(0x5891ac, 0x58919c);
                    Game_Helper558356(0x8aab78);
                    Sleep(0x9c4);
                    Game_Helper558356(0x5891b8);
                }
            }
            for (ij = 0; ij < (signed char)g_gameSlots[a1][a2].field36; ij++) {
                g_gameSlots[(&g_gameSlots[a1][a2].field74)[ij * 2]][(&g_gameSlots[a1][a2].field78)[ij * 2]].flags08 &= ~0x300000;
            }
            if (ii != 0) {
                ii = 0;
                if (g_game8bd200 != 1) {
                    Game_Fn56c804(0x5891bc);
                }
            }
        }
        if (g_game8c83ec == 1) {
            g_gameSlots[a1][a2].field36 = 0;
        }
    }
    if (a3 == 0x71) {
        while (0 < (signed char)g_gameSlots[a1][a2].field36) {
            g_gameSlots[a1][a2].field36--;
            ig = (&g_gameSlots[a1][a2].field74)[(signed char)g_gameSlots[a1][a2].field36 * 2];
            ih = (&g_gameSlots[a1][a2].field78)[(signed char)g_gameSlots[a1][a2].field36 * 2];
            if ((g_game8c83f8 & 0x4) == 0) {
                Game_Helper4883aa(ig, g_gameSlots[a1][a2].field38, a1, a2);
            } else {
                if (Game_Helper4bea85(ig, ih, 0, a1, 2, 2, 0x200, 0, 0, 0, 0, 0, 0,
                                      g_game93a7d4, -1, -1, -1, 0, 0, 0) == 0) {
                    g_game8c83ec = 1;
                } else if (g_gameSlots[ig][ih].field38 != 0) {
                    g_gameSlots[ig][ih].field38--;
                }
            }
        }
        Game_Helper46d8c1(a1, a2, 2);
    }
    return 0;
}

/* Loads a portrait/silhouette sprite pair for the character kind in a1 into
 * records a2 and a3, then derives the two frame extents and a capped height
 * from the first frame's header (+4 width, +6 height, +10 nominal height).
 * An unrecognised kind clears the record. The sprite name arguments are the
 * addresses of the original's file-name strings. */
// FUNCTION: SHANDALAR 0x004bc6d3
void Game_Fn4bc6d3(int a1, int a2, int a3)
{
    int iv;
    int ig;

    ig = 1;
    if (g_game7496a0[a2][0] == 0) {
        switch (a1) {
        case 1:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x5894d4));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x5894e0));
            break;
        case 2:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x5894ec));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x5894f8));
            break;
        case 3:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x589504));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589510));
            break;
        case 4:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x58951c));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589528));
            break;
        case 5:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x589534));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589540));
            break;
        case 6:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x58954c));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589558));
            break;
        case 8:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x589564));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589570));
            break;
        case 9:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x58957c));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589588));
            break;
        case 10:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x589594));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x5895a0));
            break;
        case 0xb:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x5895ac));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x5895b8));
            break;
        case 0xc:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x5895c4));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x5895d0));
            break;
        case 0xd:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x5895dc));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x5895e8));
            break;
        case 0xf:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x5895f4));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589600));
            break;
        case 0x10:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x58960c));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589618));
            break;
        case 0x11:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x589624));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589630));
            break;
        case 0x12:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x58963c));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589648));
            break;
        case 0x13:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x589654));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589660));
            break;
        case 0x14:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x58966c));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589678));
            break;
        case 0x16:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x589684));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589690));
            break;
        case 0x17:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x58969c));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x5896a8));
            break;
        case 0x18:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x5896b4));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x5896c0));
            break;
        case 0x19:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x5896cc));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x5896d8));
            break;
        case 0x1a:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x5896e4));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x5896f0));
            break;
        case 0x1b:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x5896fc));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589708));
            break;
        case 0x1d:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x589714));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589720));
            break;
        case 0x1e:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x58972c));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589738));
            break;
        case 0x1f:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x589744));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589750));
            break;
        case 0x20:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x58975c));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589768));
            break;
        case 0x21:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x589774));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589780));
            break;
        case 0x22:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x58978c));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589798));
            break;
        case 0x23:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x5897a4));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x5897b0));
            break;
        case 0x24:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x5897bc));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x5897c8));
            break;
        case 0x25:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x5897d4));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x5897e0));
            break;
        case 0x26:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x5897ec));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x5897f8));
            break;
        case 0x27:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x589804));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589810));
            break;
        case 0x28:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x58981c));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589828));
            break;
        case 0x29:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x589834));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589840));
            break;
        case 0x2a:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x58984c));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589858));
            break;
        case 0x2b:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x589864));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589870));
            break;
        case 0x2c:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x58987c));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589888));
            break;
        case 0x2d:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x589894));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x5898a0));
            break;
        case 0x2e:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x5898ac));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x5898b8));
            break;
        case 0x2f:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x5898c4));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x5898d0));
            break;
        case 0x30:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x5898dc));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x5898e8));
            break;
        case 0x31:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x5898f4));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589900));
            break;
        case 0x32:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x58990c));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589918));
            break;
        case 0x33:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x589924));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589930));
            break;
        case 0x34:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x58993c));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589948));
            break;
        case 0x35:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x589954));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589960));
            break;
        case 0x36:
            Spr_LoadFile(&g_game7496a0[a2][0], (const char *)Game_Fn4be4c4(0x58996c));
            Spr_LoadFile(&g_game7496a0[a3][0], (const char *)Game_Fn4be4c4(0x589978));
            break;
        default:
            ig = 0;
            break;
        }
        if (ig != 0) {
            iv = (int)g_game7496a0[a2][0];
            g_game749310[a2] = *(short *)(iv + 4);
            g_game749390[a2] = *(short *)(iv + 6);
            g_game748890[a2] = *(short *)(iv + 10);
            if (g_game749390[a2] < g_game748890[a2]) {
                g_game748890[a2] = (g_game749390[a2] * 2) / 3;
            }
            g_game749310[a3] = g_game749310[a2];
            g_game749390[a3] = g_game749390[a2];
            g_game748890[a3] = g_game748890[a2];
        } else {
            g_game7496a0[a2][0] = 0;
        }
    }
}

/* The legality predicate for a slot: 192 callers. Answers "may (a1, a2) be
 * chosen for this effect", accumulating a reason string as it goes and
 * returning zero as soon as any clause objects. a3, when non-null, receives
 * the reasons (the first byte of the working buffer is skipped, so a leading
 * separator is dropped). a5/a6 are the two players' permission bytes, a7-a20
 * the clause masks.
 *
 * a5, a6 and a3 are declared int because game/gameslot.h already fixes this
 * function's signature as twenty ints; the original narrowed a5 and a6 to
 * bytes, which is a known residual here. */
// FUNCTION: SHANDALAR 0x004bea85
int Game_Helper4bea85(int a1, int a2, int a3, int a4, int a5, int a6, int a7,
                      int a8, int a9, int a10, int a11, int a12, int a13,
                      int a14, int a15, int a16, int a17, int a18, int a19,
                      int a20)
{
    int uv;
    int bv;
    int bw;
    int iv;
    int ig;
    int ih;
    int ii;
    int ij;
    int ik;
    int il;
    int im;
    int io;
    int ip;
    int iq;
    char buf[200];
    int ir;

    if (a1 == -1) {
        if (a3 != 0) {
            strcpy((char *)a3, (const char *)0x5899a0);
        }
        ir = 0;
    } else if (a1 == -1
               || a2 == -1
               || g_gameSlots[a1][a2].field6c != -1) {
        bw = 0;
        strcpy(buf, (const char *)0x5899a8);
        if (a2 == -1) {
            if (a7 == 0 || (a7 & 0x1000) != 0) {
                if (((a4 == g_game8c776c || (g_game93a934 & 0x2) != 0)
                     && (a5 & 0x2) != 0)
                    || (a4 == 1 && (a6 & 0x2) != 0)) {
                    iq = 1;
                    ip = 1;
                } else if (((a4 == g_game8c776c || (g_game93a934 & 0x2) != 0)
                            && (a5 & 0x1) != 0)
                           || (a4 == 1 && (a6 & 0x1) != 0)) {
                    iq = 0;
                    ip = 1;
                }
            } else {
                iq = 0;
                ip = 0;
            }
            if ((a1 == 0 && iq == 0) || (a1 == 1 && ip == 0)) {
                bw = 1;
                strcat(buf, (const char *)0x8c72c0);
            }
        } else {
            if (a4 == g_game8c776c || (g_game93a934 & 0x2) != 0) {
                if ((a5 & 0x4) == 0) {
                    if ((a5 & 0x2) == 0) {
                        if ((a5 & 0x1) == 0) {
                            im = 0;
                        } else {
                            im = 1;
                        }
                    } else {
                        im = -1;
                    }
                    if ((a5 & 0xc) == 0xc) {
                        io = im;
                    } else {
                        io = -1;
                    }
                } else {
                    if ((a5 & 0x2) == 0) {
                        if ((a5 & 0x1) == 0) {
                            io = 0;
                        } else {
                            io = 1;
                        }
                    } else {
                        io = -1;
                    }
                    if ((a5 & 0xc) == 0xc) {
                        im = io;
                    } else {
                        im = -1;
                    }
                }
            } else if ((a6 & 0x4) == 0) {
                if ((a6 & 0x2) == 0) {
                    if ((a6 & 0x1) == 0) {
                        im = 0;
                    } else {
                        im = 1;
                    }
                } else {
                    im = -1;
                }
                if ((a6 & 0xc) == 0xc) {
                    io = im;
                } else {
                    io = -1;
                }
            } else {
                if ((a6 & 0x2) == 0) {
                    if ((a6 & 0x1) == 0) {
                        io = 0;
                    } else {
                        io = 1;
                    }
                } else {
                    io = -1;
                }
                if ((a6 & 0xc) == 0xc) {
                    im = io;
                } else {
                    im = -1;
                }
            }
            bv = (g_gameSlots[a1][a2].flags08 & 0x800000) != 0;
            if (bv != 0) {
                strcat(buf, (const char *)0x92e830);
            }
            bw = (g_gameSlots[a1][a2].flags08 & 0x100000) != 0;
            if (bw != 0) {
                strcat(buf, (const char *)0x92e830);
            }
            bw = bw || bv;
            if (a7 != 0
                && (((g_gameSlots[a1][a2].flags08 & 0x2) != 0 && (a7 & 0x200) == 0)
                    || ((g_gameSlots[a1][a2].flags08 & 0x2) == 0 && (a7 & 0x100) == 0))) {
                bw = 1;
                strcat(buf, (const char *)0x930e30);
            }
            if (im != -1 && im != a1) {
                bw = 1;
                strcat(buf, (const char *)0x92fd20);
            }
            if (io != -1
                && (((g_gameSlots[a1][a2].flags08 & 0x1000) != 0 && io == 0)
                    || ((g_gameSlots[a1][a2].flags08 & 0x1000) == 0 && io != 0))) {
                bw = 1;
                strcat(buf, (const char *)0x7beec0);
            }
            if (a8 != 0) {
                if ((a18 & 0x200) == 0) {
                    il = g_gameSlots[a1][a2].field6c;
                } else {
                    il = g_gameSlots[a1][a2].field3c;
                }
                bv = 0;
                if ((a8 & g_gameCards[il].flags) != 0
                    || ((a8 & 0x2000000) != 0
                        && (g_gameSlots[a1][a2].flags08 & 0x2000000) != 0)) {
                    bv = 1;
                }
                if ((a8 & 0x80) != 0
                    && (g_gameCards[il].mask == g_game7a04c4
                        || g_gameCards[g_gameSlots[a1][a2].field6c].mask == g_game7a0910)) {
                    bv = 1;
                }
                if ((a8 & 0x1000) != 0
                    && (g_gameSlots[a1][a2].flags18 & 0x10) != 0) {
                    bv = 1;
                }
                if ((a8 & 0x2000) != 0 && g_gameCards[il].mask == g_game7beaf4) {
                    bv = 1;
                }
                if ((a8 & 0x4000) != 0 && g_gameCards[il].mask == g_game8bcfe8) {
                    bv = 1;
                }
                if ((a8 & 0x8000) != 0 && g_gameCards[il].mask == g_game93a7dc) {
                    bv = 1;
                }
                if (bv == 0) {
                    bw = 1;
                    strcat(buf, (const char *)0x8bdc00);
                }
            }
            if (a9 != 0) {
                if ((a18 & 0x200) == 0) {
                    ik = g_gameSlots[a1][a2].field6c;
                } else {
                    ik = g_gameSlots[a1][a2].field3c;
                }
                bv = (a9 & g_gameCards[ik].flags) != 0;
                if ((a9 & 0x80) != 0
                    && (g_gameCards[ik].mask == g_game7a04c4
                        || g_gameCards[ik].mask == g_game7a0910)) {
                    bv = 1;
                }
                if ((a9 & 0x1000) != 0
                    && (g_gameSlots[a1][a2].flags18 & 0x10) != 0) {
                    bv = 1;
                }
                if ((a9 & 0x2000) != 0 && g_gameCards[ik].mask == g_game7beaf4) {
                    bv = 1;
                }
                if ((a9 & 0x4000) != 0 && g_gameCards[ik].mask == g_game8bcfe8) {
                    bv = 1;
                }
                if ((a9 & 0x8000) != 0 && g_gameCards[ik].mask == g_game93a7dc) {
                    bv = 1;
                }
                if (bv != 0) {
                    bw = 1;
                    strcat(buf, (const char *)0x8bdc00);
                }
            }
            if (a10 != 0
                && (g_gameSlots[a1][a2].field28 & a10) != a10) {
                bw = 1;
                strcat(buf, (const char *)0x92e960);
            }
            if (a11 != 0
                && (a11 & g_gameSlots[a1][a2].field28) != 0) {
                bw = 1;
                strcat(buf, (const char *)0x92e960);
            }
            if (a12 != 0
                && (a12 & (signed char)g_gameSlots[a1][a2].field1e) == 0) {
                bw = 1;
                strcat(buf, (const char *)0x8e2540);
            }
            if (a13 != 0
                && (a13 & (signed char)g_gameSlots[a1][a2].field1e) != 0) {
                bw = 1;
                strcat(buf, (const char *)0x8e2540);
            }
            if (a14 != -1) {
                if ((a18 & 0x100) == 0) {
                    if ((a18 & 0x4) == 0) {
                        if (a14 < 5) {
                            if (Game_Helper488147(a1, a2, a14 + 1) == 0) {
                                bw = 1;
                                strcat(buf, (const char *)0x8e3f30);
                            }
                        } else if (g_gameSlots[a1][a2].field6c != a14
                                   && g_gameCards[g_gameSlots[a1][a2].field6c].mask != g_gameCards[a14].mask) {
                            bw = 1;
                            strcat(buf, (const char *)0x8e3f30);
                        }
                    } else {
                        if (Game_Fn4c16fb(g_gameSlots[a1][a2].field6c, a14) == 0) {
                            bw = 1;
                            strcat(buf, (const char *)0x8e3f30);
                        }
                    }
                } else {
                    if (Game_Fn486f9a(a1, a2, a14) != 0) {
                        bw = 1;
                        strcat(buf, (const char *)0x8e3f30);
                    }
                }
            }
            if (a15 != -1) {
                if ((a18 & 0x10) == 0) {
                    ij = a15;
                } else if (a15 == 7) {
                    ij = 8;
                } else if (a15 == 8) {
                    ij = 7;
                } else if (a15 == 0x10) {
                    ij = 0x11;
                } else if (a15 == 0x11) {
                    ij = 0x10;
                } else if (a15 == 0x49) {
                    ij = 0x4a;
                } else if (a15 == 0x4a) {
                    ij = 0x49;
                } else if (a15 == 0x56) {
                    ij = 0x57;
                } else if (a15 == 0x57) {
                    ij = 0x56;
                } else if (a15 == 0x59) {
                    ij = 0x5a;
                } else if (a15 == 0x5a) {
                    ij = 0x59;
                } else if (a15 == 0x69) {
                    ij = 0x6a;
                } else if (a15 == 0x6a) {
                    ij = 0x69;
                } else {
                    ij = a15;
                }
                if (g_game8e44b8[g_gameCards[g_gameSlots[a1][a2].field6c].mask][0] != a15
                    && g_game8e44b8[g_gameCards[g_gameSlots[a1][a2].field6c].mask][0] != ij) {
                    bw = 1;
                    strcat(buf, (const char *)0x8e2b70);
                }
            }
            if (a16 != -1) {
                uv = a16 & 0xfff;
                a16 = a16 & 0xf000;
                if ((a16 == 0 && g_gameSlots[a1][a2].field34 != uv)
                    || (a16 == 0x1000 && g_gameSlots[a1][a2].field34 < uv)
                    || (a16 == 0x2000 && uv < g_gameSlots[a1][a2].field34)) {
                    bw = 1;
                    strcat(buf, (const char *)0x798830);
                }
            }
            if (a17 != -1) {
                uv = a17 & 0xfff;
                a17 = a17 & 0xf000;
                if ((a17 == 0 && g_gameSlots[a1][a2].field0e != uv)
                    || (a17 == 0x1000 && g_gameSlots[a1][a2].field0e < uv)
                    || (a17 == 0x2000 && uv < g_gameSlots[a1][a2].field0e)) {
                    bw = 1;
                    strcat(buf, (const char *)0x7a0240);
                }
            }
            if (a18 != 0) {
                if ((a18 & 0x1) != 0
                    && g_game8e44b8[g_gameCards[g_gameSlots[a1][a2].field6c].mask][0] != 0xc5
                    && g_gameCards[g_gameSlots[a1][a2].field6c].field29 != 0) {
                    bw = 1;
                    strcat(buf, (const char *)0x8c7640);
                }
                if ((a18 & 0x80) != 0
                    && (g_game8e44b8[g_gameCards[g_gameSlots[a1][a2].field6c].mask][0] == 0xc5
                        || g_gameCards[g_gameSlots[a1][a2].field6c].field29 == 0)) {
                    bw = 1;
                    strcat(buf, (const char *)0x8c7640);
                }
                if ((a18 & 0x2) != 0
                    && (g_game8e2688 == -1
                        || a1 != g_game8e2688
                        || a2 != g_game8e2674
                        || (g_gameSlots[a1][a2].flags08 & 0x80) != 0)) {
                    bw = 1;
                    strcat(buf, (const char *)0x92ff80);
                }
                if ((a18 & 0x4) != 0 && Game_Fn4c1695(a1, a2) == 0) {
                    bw = 1;
                    strcat(buf, (const char *)0x951980);
                }
                if ((a18 & 0x8) != 0
                    && (g_gameCards[g_gameSlots[a1][a2].field6c].flags & 0x42) != 0x42) {
                    bw = 1;
                    strcat(buf, (const char *)0x8e2e90);
                }
                if ((a18 & 0x20) != 0
                    && ((signed char)g_gameSlots[a1][a2].field50 != a4
                        || g_gameSlots[a1][a2].field04 != -1)) {
                    bw = 1;
                    strcat(buf, (const char *)0x9304a0);
                }
                if ((a18 & 0x40) != 0
                    && g_gameCards[g_gameSlots[a1][a2].field6c].field29 != 6
                    && g_gameCards[g_gameSlots[a1][a2].field6c].field29 != 5) {
                    bw = 1;
                    strcat(buf, (const char *)0x5899ac);
                }
            }
            if (a19 != 0) {
                if ((a19 & 0x1) != 0
                    && (g_gameSlots[a1][a2].flags08 & 0x10) == 0) {
                    bw = 1;
                    strcat(buf, (const char *)0x9391b0);
                }
                if ((a19 & 0x2) != 0
                    && (g_gameSlots[a1][a2].flags08 & 0x4) == 0) {
                    bw = 1;
                    strcat(buf, (const char *)0x8aa740);
                }
                if ((a19 & 0x4) != 0
                    && (g_gameSlots[a1][a2].flags08 & 0x40) == 0) {
                    bw = 1;
                    strcat(buf, (const char *)0x930100);
                }
                if ((a19 & 0x8) != 0
                    && (g_gameSlots[a1][a2].flags08 & 0x200) == 0) {
                    bw = 1;
                    strcat(buf, (const char *)0x930bd0);
                }
                if ((a19 & 0x10) != 0
                    && (a1 == g_game7a82e0
                        || g_gameSlots[a1][a2].field24 == -1)) {
                    bw = 1;
                    strcat(buf, (const char *)0x7bf360);
                }
                if ((a19 & 0x20) != 0) {
                    bv = 0;
                    if (g_game94d464 < 0x15
                        || 0x1d < g_game94d464
                        || a1 == g_game7a82e0) {
                        bv = 1;
                    } else if (a1 == g_game7a82e0
                               || g_gameSlots[a1][a2].field24 == -1) {
                        bv = 1;
                    }
                    if ((g_gameSlots[a1][a2].flags08 & 0x4) == 0 && bv != 0) {
                        bw = 1;
                        strcat(buf, (const char *)0x8e09d0);
                    }
                }
                if ((a19 & 0x40) != 0) {
                    bv = 0;
                    for (ii = 0; ii < 2; ii++) {
                        for (ih = 0; ih < g_game78e970[ii]; ih++) {
                            if (g_gameSlots[ii][ih].field6c != -1
                                && (g_gameCards[g_gameSlots[ii][ih].field6c].flags & 0x4) != 0
                                && (signed char)g_gameSlots[ii][ih].field50 == a1
                                && g_gameSlots[ii][ih].field04 == a2) {
                                bv = 1;
                            }
                        }
                    }
                    if (bv == 0) {
                        bw = 1;
                        strcat(buf, (const char *)0x8e3510);
                    }
                }
                if ((a19 & 0x80) != 0
                    && (g_gameSlots[a1][a2].flags08 & 0x80) == 0) {
                    bw = 1;
                    strcat(buf, (const char *)0x94d180);
                }
                if ((a19 & 0x100) != 0
                    && ((g_gameSlots[a1][a2].flags08 & 0x80) == 0
                        || (g_gameSlots[a1][a2].flags08 & 0x20) != 0)) {
                    bw = 1;
                    strcat(buf, (const char *)0x8e27f0);
                }
                if ((a19 & 0x200) != 0 && Game_Fn4c0dd4(a1, a2) == 0) {
                    bw = 1;
                    strcat(buf, (const char *)0x7a7f40);
                }
                if ((a19 & 0x400) != 0
                    && (g_gameSlots[a1][a2].field11c & 0x1) == 0) {
                    bw = 1;
                    strcat(buf, (const char *)0x8c7d60);
                }
                if ((a19 & 0x800) != 0
                    && (g_gameSlots[a1][a2].field11c & 0x2) == 0) {
                    bw = 1;
                    strcat(buf, (const char *)0x8e2ca0);
                }
                if ((a19 & 0x1000) != 0
                    && (g_gameSlots[a1][a2].flags08 & 0x30000) == 0x30000) {
                    bw = 1;
                    strcat(buf, (const char *)0x5899bc);
                }
            }
            if (a20 != 0) {
                if ((a20 & 0x1) != 0
                    && (g_gameSlots[a1][a2].flags08 & 0x10) != 0) {
                    bw = 1;
                    strcat(buf, (const char *)0x9391b0);
                }
                if (((a20 & 0x2) != 0 || (a20 & 0x20) != 0)
                    && (g_gameSlots[a1][a2].flags08 & 0x4) != 0) {
                    bw = 1;
                    strcat(buf, (const char *)0x8aa740);
                }
                if ((a20 & 0x4) != 0
                    && (g_gameSlots[a1][a2].flags08 & 0x40) != 0) {
                    bw = 1;
                    strcat(buf, (const char *)0x930100);
                }
                if ((a20 & 0x8) != 0
                    && (g_gameSlots[a1][a2].flags08 & 0x200) != 0) {
                    bw = 1;
                    strcat(buf, (const char *)0x930bd0);
                }
                if (((a20 & 0x10) != 0 || (a20 & 0x20) != 0)
                    && g_gameSlots[a1][a2].field24 != -1
                    && a1 != g_game7a82e0) {
                    bw = 1;
                    strcat(buf, (const char *)0x7bf360);
                }
                if ((a20 & 0x40) != 0) {
                    bv = 0;
                    for (ig = 0; ig < 2; ig++) {
                        for (iv = 0; iv < g_game78e970[ig]; iv++) {
                            if (g_gameSlots[ig][iv].field6c != -1
                                && (g_gameCards[g_gameSlots[ig][iv].field6c].flags & 0x4) != 0
                                && (signed char)g_gameSlots[ig][iv].field50 == a1
                                && g_gameSlots[ig][iv].field04 == a2) {
                                bv = 1;
                            }
                        }
                    }
                    if (bv != 0) {
                        bw = 1;
                        strcat(buf, (const char *)0x8e3510);
                    }
                }
                if ((a20 & 0x80) != 0
                    && (g_gameSlots[a1][a2].flags08 & 0x80) != 0) {
                    bw = 1;
                    strcat(buf, (const char *)0x94d180);
                }
                if ((a20 & 0x100) != 0
                    && (g_gameSlots[a1][a2].flags08 & 0x80) != 0
                    && (g_gameSlots[a1][a2].flags08 & 0x20) == 0) {
                    bw = 1;
                    strcat(buf, (const char *)0x8e27f0);
                }
                if ((a20 & 0x200) != 0 && Game_Fn4c0dd4(a1, a2) != 0) {
                    bw = 1;
                    strcat(buf, (const char *)0x7a7f40);
                }
                if ((a20 & 0x400) != 0
                    && (g_gameSlots[a1][a2].field11c & 0x1) != 0) {
                    bw = 1;
                    strcat(buf, (const char *)0x8c7d60);
                }
                if ((a20 & 0x800) != 0
                    && (g_gameSlots[a1][a2].field11c & 0x2) != 0) {
                    bw = 1;
                    strcat(buf, (const char *)0x8e2ca0);
                }
                if ((a20 & 0x1000) != 0
                    && (g_gameSlots[a1][a2].flags08 & 0x30000) != 0) {
                    bw = 1;
                    strcat(buf, (const char *)0x5899d0);
                }
            }
        }
        ir = !bw;
        if (a3 != 0) {
            if (buf[0] == '\0') {
                *(char *)a3 = '\0';
            } else {
                strcpy((char *)a3, &buf[1]);
            }
        }
    } else {
        if (a3 != 0) {
            strcpy((char *)a3, (const char *)0x5899a4);
        }
        ir = 0;
    }
    return ir;
}
