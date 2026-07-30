/* Shandalar game layer: card behaviour scripts, cards48.
 *
 * One of several translation units, split by address range. THIS IS NOT
 * COSMETIC: MSVC re-rolls codegen across a whole translation unit when
 * anything in it changes, so a single cards.c meant every new script perturbed
 * every earlier one -- 0x00517888 fell from 100% to 60.78% in one batch
 * without being touched. Smaller units confine that blast radius.
 *
 * It is also the closer match to the original, which evidently had one source
 * file per card: three byte-identical 18-byte handlers sit at three different
 * addresses rather than being one function reused.
 *
 * The shared CUR/SRC macros live in game/gameslot.h. */

#include "game/gameslot.h"

/* Same shape as 0x0043b476 but indexed with `a`, and answering two kinds
 * instead of one. */
// FUNCTION: SHANDALAR 0x0047903d
int Game_CardScript47903d(int a, int b, int kind)
{
    if (g_game951bdc == b
        && g_game8e1aa8 == a
        && g_game77ed80[a][Game_Helper4ba309(a, b, 3)] > 0) {
        if (kind == 0x32) {
            g_game7a01e0++;
        }
        if (kind == 0x33) {
            g_game7a01e0 += 2;
        }
    }
    return 0;
}

/* Guards once, then answers three different kinds in sequence -- and the last
 * two do the same thing, written out twice. */
// FUNCTION: SHANDALAR 0x00485094
int Game_CardScript485094(int a, int b, int kind)
{
    if (g_gameSlots[a][b].field04 == g_game951bdc
        && g_gameSlots[a][b].field50 == g_game8e1aa8
        && g_game951bdc != -1) {
        if (kind == 0x3c) {
            g_game7a01e0 = g_gameSlots[a][b].field20;
        }
        if (kind == 0x32) {
            g_game7a01e0++;
        }
        if (kind == 0x33) {
            g_game7a01e0++;
        }
    }
    return 0;
}

/* --- the empty ones ------------------------------------------------------
 *
 * Three separate functions with identical bodies: set up a frame, save three
 * registers it does not use, return 0. 0x00488ca0 is the handler for 88 of the
 * 722 cards that have one, so it is the table's "this card does nothing"
 * entry; the other two are one card each and are byte-identical to it rather
 * than being the same function reused, which is what you get when the source
 * has one such stub per card file. */

// FUNCTION: SHANDALAR 0x00488ca0
int Game_CardScript488ca0(int a, int b, int kind)
{
    return 0;
}

// FUNCTION: SHANDALAR 0x00488cb2
int Game_CardScript488cb2(int a, int b, int kind)
{
    return 0;
}

/* Handler for kind 0x6e. Checks that the slot it was invoked on still matches
 * all three arguments, then clears its field38 if the card behind its *source*
 * slot has bit 1 of flags set.
 *
 * `a`, `b` and `kind` are named for their shape: `kind` is the value every
 * script tests first against its own constant, and the other two are matched
 * against slot fields. Two call sites are visible in the binary and neither
 * establishes meaning -- one passes literals (0, 0x94, 0x22), the other
 * forwards its own arguments.
 *
 * The two-ids choice in the middle -- take field6c unless it is -1, then take
 * field3c -- is the third place this appears (Game_AllocSlot and Game_LogEvent
 * are the others), each time written out again rather than shared. */
// FUNCTION: SHANDALAR 0x0048b2c1
int Game_CardScript48b2c1(int a, int b, int kind)
{
    int card;

    if (kind == 0x6e
        && CUR.field6c == g_game93a7d4
        && CUR.field04 == b
        && (signed char)CUR.field50 == a) {

        if (SRC.field6c != -1) {
            card = SRC.field6c;
        } else {
            card = SRC.field3c;
        }

        if ((g_gameCards[card].flags & 2) != 0) {
            CUR.field38 = 0;
        }
    }
    return 0;
}

/* ...and this one asks about the other pair of globals, passes `b` where the
 * other passes -1, and wants more than 1. */
// FUNCTION: SHANDALAR 0x0048b5a9
int Game_CardScript48b5a9(int a, int b, int kind)
{
    if (kind == 0x78 && g_game951bdc == b && g_game8e1aa8 == a) {
        if (Game_Helper40d7b2(g_game94e928, g_game8e26b0, 0x32, b) > 1) {
            g_game7a01e0 = 1;
        }
    }
    return 0;
}

/* --- behind the wider stub set -------------------------------------------
 *
 * A NOTE FOR WHOEVER TAKES 0x004b47b1: it passes 0x004b484b to
 * Game_Helper488b70, and 0x004b484b disassembles as a function prologue --
 * `push ebp / mov ebp,esp / push ebx,esi,edi / mov eax,[ebp+0x14]`, so at
 * least four arguments. The helpers take CALLBACKS, which means some of the
 * 608 "scripts" in the card table are really continuations passed to them.
 * That function is not in the card table itself and would need its own stub
 * plus a cast at the call site. */

/* A pair, differing only in which constant they pass and what they compare the
 * result against: 0x32 either way, but this one asks the helper about the
 * current row/column and wants more than 2. */
// FUNCTION: SHANDALAR 0x0048b610
int Game_CardScript48b610(int a, int b, int kind)
{
    if (kind == 0x78 && b == g_game8e26b0 && a == g_game94e928) {
        if (Game_Helper40d7b2(g_game8e1aa8, g_game951bdc, 0x32, -1) > 2) {
            g_game7a01e0 = 1;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0048c465
int Game_CardScript48c465(int a, int b, int kind)
{
    if (kind == 0x33 && g_game951bdc == b && g_game8e1aa8 == a) {
        if ((g_gameSlots[a][b].flags08 & 0x10) == 0) {
            g_game7a01e0 += 3;
        }
    }
    return 0;
}


/* The mirror of 0x0048b610: same guards, but it requires the card byte to be
 * NON-zero and then masks bit 5 out of the helper's answer. */
// FUNCTION: SHANDALAR 0x0048b673
int Game_CardScript48b673(int a, int b, int kind)
{
    if (kind == 0x78
        && b == g_game8e26b0
        && a == g_game94e928
        && g_gameCards[CUR.field6c].field29 != 0) {
        if ((Game_Helper40d7b2(g_game8e1aa8, g_game951bdc, 0x34, -1) & 0x20) == 0) {
            g_game7a01e0 = 1;
        }
    }
    return 0;
}


/* --- generated by cardgen (ghidra-work/scripts/cardgen.py) ---------------
 * Reconstructed by pattern-matching the disassembly, not by hand. Every one
 * of these was produced without reading it; the reccmp score is the check.
 * Names are addresses because nothing here establishes meaning. */

// FUNCTION: SHANDALAR 0x0048b9cb
int Game_CardScript48b9cb(int a, int b, int kind)
{
    if (kind == 0x6e
        && g_gameSlots[g_game8e1aa8][g_game951bdc].field6c == g_game93a7d4) {
        if (g_gameSlots[g_game8e1aa8][g_game951bdc].field04 == -1
            && (signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field0c == a
            && g_gameSlots[g_game8e1aa8][g_game951bdc].field48 == b
            && g_gameSlots[g_game8e1aa8][g_game951bdc].field38 != 0) {
            g_gameSlots[a][b].field0c = g_game8e1aa8;
            g_gameSlots[a][b].field48 = g_game951bdc;
        }
        return 0;
    }
    if (g_game94d178 == 0xd7
        && b == g_game951bdc
        && a == g_game8e1aa8
        && (signed char)g_gameSlots[a][b].field0c != -1
        && a == g_game8c8afc) {
        if (kind == 0x7d) {
            g_game7a01e0 |= 2;
        }
        if (kind == 0x7e) {
            g_game7bfee0[(signed char)g_gameSlots[a][b].field0c] += 2;
            g_gameSlots[a][b].field0c = 0xff;
            Game_Helper403eeb();
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0048bc78
int Game_CardScript48bc78(int a, int b, int kind)
{
    if (kind == 0x6e
        && g_gameSlots[g_game8e1aa8][g_game951bdc].field6c == g_game93a7d4) {
        if (g_gameSlots[g_game8e1aa8][g_game951bdc].field04 == -1
            && (signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field0c == a
            && g_gameSlots[g_game8e1aa8][g_game951bdc].field48 == b
            && g_gameSlots[g_game8e1aa8][g_game951bdc].field38 != 0) {
            g_gameSlots[a][b].field0c = g_game8e1aa8;
            g_gameSlots[a][b].field48 = g_game951bdc;
        }
        return 0;
    }
    if (g_game94d178 == 0xd7
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && (signed char)g_gameSlots[a][b].field0c != -1
        && a == g_game8c8afc) {
        if (kind == 0x7d) {
            g_game7a01e0 |= 2;
        }
        if (kind == 0x7e) {
            g_game7bfee0[(signed char)g_gameSlots[a][b].field0c]++;
            g_gameSlots[a][b].field0c = 0xff;
            Game_Helper403eeb();
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0048bf26
int Game_CardScript48bf26(int a, int b, int kind)
{
    if (kind == 0x6e
        && g_gameSlots[g_game8e1aa8][g_game951bdc].field6c == g_game93a7d4) {
        if (g_gameSlots[g_game8e1aa8][g_game951bdc].field04 == -1
            && (signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field0c == a
            && g_gameSlots[g_game8e1aa8][g_game951bdc].field48 == b
            && g_gameSlots[g_game8e1aa8][g_game951bdc].field38 != 0) {
            g_gameSlots[a][b].field0c = g_game8e1aa8;
            g_gameSlots[a][b].field48 = g_game951bdc;
        }
        return 0;
    }
    if (g_game94d178 == 0xd7
        && b == g_game951bdc
        && a == g_game8e1aa8
        && (signed char)g_gameSlots[a][b].field0c != -1
        && a == g_game8c8afc) {
        if (kind == 0x7d) {
            g_game7a01e0 |= 2;
        }
        if (kind == 0x7e) {
            g_gameSlots[a][b].field4c = (signed char)g_gameSlots[a][b].field0c;
            Game_Helper48c204(a, b);
            g_gameSlots[a][b].field0c = 0xff;
        }
    }
    return 0;
}


/* --- generated by cardgen (ghidra-work/scripts/cardgen.py) ---------------
 * Reconstructed by pattern-matching the disassembly, not by hand. Every one
 * of these was produced without reading it; the reccmp score is the check.
 * Names are addresses because nothing here establishes meaning. */

// FUNCTION: SHANDALAR 0x00472df4
int Game_CardScript472df4(int a, int b, int kind)
{
    if (g_gameSlots[g_game8e1aa8][g_game951bdc].field6c == g_game93a7d4
        && g_gameSlots[g_game8e1aa8][g_game951bdc].field38 != 0
        && g_gameSlots[g_game8e1aa8][g_game951bdc].field04 == b
        && (signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field50 == a
        && ((signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field1e & 0x40) != 0) {
        g_gameSlots[g_game8e1aa8][g_game951bdc].field38 = 0;
    }
    return 0;
}


// FUNCTION: SHANDALAR 0x0047ffee
int Game_CardScript47ffee(int a, int b, int kind)
{
    if (kind == 0x71
        && Game_Helper488b70(a, b, (int)Game_Callback48044a, a) != -1) {
        g_gameSlots[a][b].field4c = 1;
    }
    if (g_game94d178 == 0xe1
        && g_gameSlots[a][b].field4c == 0
        && (g_gameSlots[a][b].flags08 & 0x10) == 0
        && g_game951bdc == b
        && a == g_game8e1aa8
        && a == g_game8c8afc
        && a != g_game7a82e0
        && Game_Helper411d90(a, b) != 0) {
        if (g_game94d464 != 0x1a) {
            if (g_game94d464 == 0x19) {
                g_gameSlots[a][b].field38 = kind;
                if (kind == 0x7d
                    && Game_Helper488b70(a, b, (int)Game_Callback480238, -1) != -1) {
                    g_game7a01e0 |= 2;
                }
                if (kind == 0x7e) {
                    Game_Helper488b70(a, b, (int)Game_Callback480238, -1);
                    Game_Helper488b70(a, b, (int)Game_Callback4803cc, a);
                }
            }
            if (kind == 0x22
                && g_game951bdc == b
                && a == g_game8e1aa8) {
                g_gameSlots[a][b].field4c = 0;
            }
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00483690
int Game_CardScript483690(int a, int b, int kind)
{
    if (g_game94d178 != 0xcc) {
        if (kind == 0xc7
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            if (kind == 0x7d) {
                g_game7a01e0 |= 2;
            }
            if (kind != 0x7e) {
                if (kind == 0xc7) {
                    if ((signed char)g_gameSlots[a][b].field50 != -1) {
                        Game_AllocSlot((signed char)g_gameSlots[a][b].field50, g_gameSlots[a][b].field04, 1, (signed char)g_gameSlots[a][b].field0c, g_gameSlots[a][b].field48);
                    }
                    Game_Helper46d8c1(a, b, 1);
                }
            }
        }
    }
    return 0;
}


// FUNCTION: SHANDALAR 0x0048b71c
int Game_CardScript48b71c(int a, int b, int kind)
{
    if (kind == 0x6e
        && g_gameSlots[g_game8e1aa8][g_game951bdc].field6c == g_game93a7d4) {
        if (g_gameSlots[g_game8e1aa8][g_game951bdc].field04 == -1
            && (signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field0c == a
            && g_gameSlots[g_game8e1aa8][g_game951bdc].field48 == b
            && g_gameSlots[g_game8e1aa8][g_game951bdc].field38 != 0) {
            g_gameSlots[a][b].field0c = g_game8e1aa8;
            g_gameSlots[a][b].field48 = g_game951bdc;
        }
        return 0;
    }
    if (g_game94d178 == 0xd7
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && (signed char)g_gameSlots[a][b].field0c != -1
        && a == g_game8c8afc) {
        if (kind == 0x7d) {
            g_game7a01e0 |= 2;
        }
        if (kind == 0x7e) {
            Game_Helper409ee8((signed char)g_gameSlots[a][b].field0c, 1, 0);
            g_gameSlots[a][b].field0c = 0xff;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0048faf7
int Game_CardScript48faf7(int a, int b, int kind)
{
    int v4;

    if (kind == 0x71) {
        v4 = Game_Helper4a9910(a, b, g_game951968, a, b);
        if (v4 != -1) {
            g_gameSlots[a][v4].field38 = 1;
            g_gameSlots[a][v4].field4c = 0x10e;
            g_gameSlots[a][v4].flags18 = 0x10020;
            g_gameSlots[a][v4].flags18 |= 0x2000000;
            g_gameSlots[a][b].field0c = a;
            g_gameSlots[a][b].field48 = v4;
        }
        return 0;
    }
    if (kind != 0x32) {
        if (kind == 0x33
            && g_game951bdc == b
            && g_game8e1aa8 == a) {
            if (g_game7a82e0 == a) {
                g_gameSlots[(signed char)g_gameSlots[a][b].field0c][g_gameSlots[a][b].field48].field4c |= 2;
            } else {
                g_gameSlots[(signed char)g_gameSlots[a][b].field0c][g_gameSlots[a][b].field48].field4c &= ~2;
            }
            return 0;
        }
    }
    return 0;
}


/* --- generated by cardgen (ghidra-work/scripts/cardgen.py) ---------------
 * Reconstructed by pattern-matching the disassembly, not by hand. Every one
 * of these was produced without reading it; the reccmp score is the check.
 * Names are addresses because nothing here establishes meaning. */

// FUNCTION: SHANDALAR 0x004764bc
int Game_CardScript4764bc(int a, int b, int kind)
{
    int v4;

    if (kind == 0x71) {
        v4 = Game_Helper4a9910(a, b, g_game951968, a, b);
        if (v4 != -1) {
            g_gameSlots[a][v4].field4c = 0x10020e;
            g_gameSlots[a][v4].flags18 = 0x10020;
            g_gameSlots[a][v4].field38 = 0x40;
            g_gameSlots[a][b].field0c = a;
            g_gameSlots[a][b].field48 = v4;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004790b4
int Game_CardScript4790b4(int a, int b, int kind)
{
    if (kind == 0x71
        && Game_Helper488b70(a, b, (int)Game_Callback48044a, a) != -1) {
        g_gameSlots[a][b].field4c = 1;
    }
    if (g_game94d178 == 0xe1
        && g_gameSlots[a][b].field4c == 0
        && (g_gameSlots[a][b].flags08 & 0x10) == 0
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && g_game8c8afc == a) {
        g_gameSlots[a][b].field38 = kind;
        if (kind == 0x7d
            && Game_Helper488b70(a, b, (int)Game_Callback480238, -1) != -1) {
            g_game7a01e0 |= 2;
        }
        if (kind == 0x7e) {
            Game_Helper488b70(a, b, (int)Game_Callback480238, -1);
            Game_Helper488b70(a, b, (int)Game_Callback4803cc, a);
        }
    }
    if (kind == 0x22
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_gameSlots[a][b].field4c = 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00483817
int Game_CardScript483817(int a, int b, int kind)
{
    if (kind == 0x34
        && g_gameSlots[a][b].field04 == g_game951bdc
        && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8
        && g_game951bdc != -1) {
        g_game7a01e0 |= g_gameSlots[a][b].field38;
    }
    if (g_game94d464 == 4
        && g_game7a82e0 == a
        && g_gameSlots[a][b].field4c == 0) {
        g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field28 |= 0x8000000;
        Game_Helper46d8c1(a, b, 1);
    }
    if (kind == 0x22) {
        g_gameSlots[a][b].field4c = 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00484619
int Game_CardScript484619(int a, int b, int kind)
{
    if (g_gameSlots[(signed char)g_gameSlots[a][b].field0c][g_gameSlots[a][b].field48].field38 != g_gameSlots[a][b].field38) {
        Game_Helper46d8c1(a, b, 2);
    }
    if (kind == 0x3c
        && g_gameSlots[a][b].field04 == g_game951bdc
        && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8
        && g_game951bdc != -1
        && (g_game8c83fa & 0x2) != 0) {
        if (Game_Helper48808b(g_game7a01e0, g_gameSlots[a][b].field38) == 0 || Game_Helper411d90(g_game8e1aa8, g_game951bdc) != 0) {
            g_game7a01e0 = g_gameSlots[a][b].field20;
            g_gameSlots[g_game8e1aa8][g_game951bdc].flags18 |= 0x40;
            return 0;
            Game_Helper46d8c1(a, b, 2);
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0048a4aa
int Game_CardScript48a4aa(int a, int b, int kind)
{
    int v4;

    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x20010) == 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x6d) {
        g_gameSlots[a][b].flags08 |= 0x10;
        return 0;
    }
    if (kind == 0x72) {
        v4 = Game_Helper409a0b(a);
        if (g_game8bd200 != 1) {
            Game_Helper56ca10(0x586d5c, 0x586d54);
            Game_Helper558538(a, a, b, a, v4, 0x8aa920, 0);
        }
        if ((g_gameCards[g_gameSlots[a][v4].field6c].flags & 0x1) == 0) {
            Game_Helper46e4d0(a, v4);
            g_gameSlots[a][v4].field6c = -1;
            g_game8e3ec8[a]--;
            if (g_game8bd200 != 1) {
                Game_Helper40ebb9(0x18);
            }
        }
        return 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0048f983
int Game_CardScript48f983(int a, int b, int kind)
{
    int v4;

    if (kind == 0x71) {
        v4 = Game_Helper4a9910(a, b, g_game951968, a, b);
        if (v4 != -1) {
            g_gameSlots[a][v4].field38 = 1;
            g_gameSlots[a][v4].field4c = 0x10d;
            g_gameSlots[a][v4].flags18 = 0x10020;
            g_gameSlots[a][v4].flags18 |= 0x2000000;
            g_gameSlots[a][b].field0c = a;
            g_gameSlots[a][b].field48 = v4;
        }
    }
    return 0;
}


/* --- generated by cardgen (ghidra-work/scripts/cardgen.py) ---------------
 * Reconstructed by pattern-matching the disassembly, not by hand. Every one
 * of these was produced without reading it; the reccmp score is the check.
 * Names are addresses because nothing here establishes meaning. */

// FUNCTION: SHANDALAR 0x0047d0aa
int Game_CardScript47d0aa(int a, int b, int kind)
{
    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x20010) == 0
            && Game_Helper4be650(0, 0, a, a, a, 0x200, 0x40, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x6d) {
        Game_Helper56ca10(0x586b18, 0x586b08);
        if (Game_Helper4a0403(a) != 0) {
            g_gameSlots[a][b].flags08 |= 0x10;
        } else {
            g_game8c83ec = 1;
        }
    }
    if (kind == 0x72) {
        Game_Helper409a0b(a);
    }
    return 0;
}


/* --- generated by cardgen (ghidra-work/scripts/cardgen.py) ---------------
 * Reconstructed by pattern-matching the disassembly, not by hand. Every one
 * of these was produced without reading it; the reccmp score is the check.
 * Names are addresses because nothing here establishes meaning. */

// FUNCTION: SHANDALAR 0x00472d56
int Game_CardScript472d56(int a, int b, int kind)
{
    Game_CardScript472df4(a, b, kind);
    if (kind == 0x78
        && b == g_game8e26b0
        && a == g_game94e928
        && (g_gameCards[g_gameSlots[g_game8e1aa8][g_game951bdc].field6c].flags & 0x40) != 0) {
        g_game7a01e0 = 1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00473a91
int Game_CardScript473a91(int a, int b, int kind)
{
    if (g_game94d178 == 0xd3
        && g_game951bdc == b
        && a == g_game8e1aa8
        && a == g_game8c8afc
        && (g_gameSlots[a][b].flags08 & 0x20) == 0
        && a != g_game7bf4b8
        && g_gameSlots[g_game7bf4b8][g_game8e2fc4].field6c != -1
        && (g_gameCards[g_gameSlots[g_game7bf4b8][g_game8e2fc4].field6c].flags & 0x40) != 0) {
        if (kind == 0x7d) {
            g_game7a01e0 |= 2;
        }
        if (kind == 0x7e) {
            g_gameSlots[a][b].field12++;
            g_gameSlots[a][b].field1c++;
            Game_Helper49f61a(a, b, 1);
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004765fe
int Game_CardScript4765fe(int a, int b, int kind)
{
    int v4;

    if (kind == 0x73) {
        if (g_game94d464 == 4
            && g_game8c83f0[1 - a] > g_game8c83f0[a]
            && g_game7a82e0 == g_game77c57c
            && g_game7a82e0 == a
            && (g_gameSlots[a][b].field38 & 0x1) == 0) {
            g_gameSlots[a][b].field5c |= 0x101;
            g_game8c7420 |= 3;
            return 1;
        }
        return 0;
    }
    if (kind == 4
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_gameSlots[a][b].field38 |= 1;
        g_game7be9ac = 1;
        g_game7a01e0 |= 1;
    }
    if (kind == 0x86
        && g_game8c83f0[1 - a] > g_game8c83f0[a]) {
        v4 = Game_Helper4d40a4(g_game94f280, g_game930f5c);
        if (v4 != -1) {
            g_gameSlots[1 - a][v4].field38 |= 1;
        }
    }
    if (kind == 0xc7
        && g_game94d464 == 0x1f
        && 1 - a == g_game7a82e0
        && g_game8c83f0[1 - a] > g_game8c83f0[a]) {
        v4 = Game_Helper4d40a4(a, b);
    }
    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && g_game8bd200 == 1
        && g_game8c7468 == a
        && g_game8c83f0[1 - a] > g_game8c83f0[a]) {
        v4 = Game_Helper4d40a4(a, b);
    }
    if (kind == 0x22) {
        g_gameSlots[a][b].field38 &= ~1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004768e5
int Game_CardScript4768e5(int a, int b, int kind)
{
    int v4;

    if (kind == 0x1a
        && g_game94d464 == 0x17
        && (signed char)g_gameSlots[a][b].field24 != -1
        && g_game7a82e0 != a
        && g_gameCards[g_gameSlots[a][b].field6c].script != (int)Game_Callback48592f) {
        v4 = Game_Helper4a9910(a, b, g_game8bcff8, a, b);
        if (v4 != -1) {
            g_gameSlots[a][v4].field12 = 2;
            g_gameSlots[a][v4].field1c = 2;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00478a33
int Game_CardScript478a33(int a, int b, int kind)
{
    if (kind == 0x78
        && g_game8e26b0 == b
        && g_game94e928 == a
        && (signed char)g_gameCards[g_gameSlots[g_game8e1aa8][g_game951bdc].field6c].field29 == 0) {
        g_game7a01e0 = 1;
    }
    if (kind == 0x89
        && (g_gameSlots[a][b].flags08 & 0x8000) == 0) {
        g_gameSlots[a][b].flags08 |= 0x8000;
        if (Game_Helper40cadc(a, b) != 0) {
            g_game8ca260 = 1;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00478b53
int Game_CardScript478b53(int a, int b, int kind)
{
    if ((g_game94d178 == 0xcd || kind == 0xc7)
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && g_game8e3efc != 0
        && a == g_game8c8afc) {
        if (kind == 0x7d) {
            g_game7a01e0 |= 2;
        }
        if (kind == 0x7e || kind == 0xc7) {
            Game_Helper49f61a(a, b, g_game8e3efc);
            g_gameSlots[a][b].field12 = g_gameSlots[a][b].field12 + g_game8e3efc;
            g_gameSlots[a][b].field1c = g_gameSlots[a][b].field1c + g_game8e3efc;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0047cfdc
int Game_CardScript47cfdc(int a, int b, int kind)
{
    int v4;

    if (kind == 0x77
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && (g_gameSlots[a][b].flags08 & 0x20) == 0
        && (signed char)g_gameSlots[a][b].field68 != 4) {
        v4 = Game_Helper4a9910(a, b, g_game8c6aa4, -1, -1);
        g_game94d63c += 0x60;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0047d770
int Game_CardScript47d770(int a, int b, int kind)
{
    if (kind == 0x73) {
        if (g_game94d464 == 4
            && g_game7a82e0 == g_game77c57c
            && a == g_game7a82e0
            && (g_gameSlots[a][b].field38 & 0x1) == 0) {
            g_gameSlots[a][b].field5c |= 0x101;
            g_game8c7420 |= 3;
            return 1;
        }
        return 0;
    }
    if (kind == 4
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        g_gameSlots[a][b].field38 |= 1;
        g_game7be9ac = 1;
        g_game7a01e0 |= 1;
    }
    if (kind == 0x86) {
        Game_Helper4bab38(a, 1, g_game94f280, g_game930f5c);
    }
    if (kind == 0xc7
        && g_game94d464 == 0x1f
        && a != g_game7a82e0) {
        Game_Helper4bab38(a, 1, a, b);
    }
    if (kind == 0x22) {
        g_gameSlots[a][b].field38 &= ~1;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004808e9
int Game_CardScript4808e9(int a, int b, int kind)
{
    int v4;

    if (kind == 0x87
        && Game_Helper4be650(0, 0, a, a, a, 0x200, 0x40, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
        g_game7a01e0 |= 1;
    }
    if (kind == 0x85
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && g_gameSlots[a][b].field38 == 0
        && g_game7a82e0 == a
        && g_game77c57c == a) {
        g_gameSlots[a][b].field5c |= 0x101;
    }
    if (kind == 4
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_gameSlots[a][b].field38++;
        if (Game_Helper4be650(0, 0, a, a, a, 0x200, 0x40, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
            Game_Helper56ca10(0x586bec, 0x586bdc);
            v4 = Game_Helper4a0403(a);
        } else {
            g_game7a01e0 |= 1;
        }
    }
    if (kind == 0x86) {
        Game_Helper56ca10(0x586c0c, 0x586bfc);
        Game_Helper558538(a, a, b, -1, -1, 0x8aa920 + 0x12c, 0);
        Game_Helper4bab38(a, 2, g_game94f280, g_game930f5c);
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].flags08 |= 0x10;
    }
    if (kind == 0xc7
        && Game_Helper4be650(0, 0, a, a, a, 0x200, 0x40, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
        Game_Helper4bab38(a, 2, a, b);
        g_gameSlots[a][b].flags08 |= 0x10;
    }
    if ((kind == 0x22 || kind == 0xc7)
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_gameSlots[a][b].field38 = 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00484044
int Game_CardScript484044(int a, int b, int kind)
{
    if (kind == 0x82
        && g_gameSlots[a][b].field04 == g_game951bdc
        && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8
        && g_game951bdc != -1) {
        g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field11c &= ~3;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0048416b
int Game_CardScript48416b(int a, int b, int kind)
{
    int v4;

    if (kind == 0x89
        && (g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags08 & 0x8000) == 0
        && (g_gameCards[g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field6c].flags & 0x2) != 0) {
        g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags08 |= 0x8000;
        if (Game_Helper40cadc((signed char)g_gameSlots[a][b].field50, g_gameSlots[a][b].field04) != 0) {
            g_game8ca260 = 1;
        }
    }
    if ((kind == 0x22 || kind == 0xc7)
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        v4 = g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field6c;
        if (Game_Helper411d90((signed char)g_gameSlots[a][b].field50, g_gameSlots[a][b].field04) != 0
            && (g_gameCards[v4].flags & 0x2) != 0
            && (signed char)g_gameCards[v4].field29 != 0
            && (g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags08 & 0x30040) == 0) {
            Game_Helper46d8c1((signed char)g_gameSlots[a][b].field50, g_gameSlots[a][b].field04, 2);
        }
        Game_Helper46d8c1(a, b, 2);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00484b5d
int Game_CardScript484b5d(int a, int b, int kind)
{
    int v4;

    if (kind == 0x22) {
        v4 = Game_FindFreeSlot(a, Game_Helper56c705(0x37c));
        if (v4 != -1) {
            Game_Helper4d1c03(a, v4);
            g_gameSlots[a][v4].field1e = 2 << Game_Helper4ba392((signed char)g_gameSlots[a][b].field0c, g_gameSlots[a][b].field48, 4) - 1;
            g_gameSlots[a][v4].flags18 |= 0x10;
            if ((g_gameSlots[a][b].flags18 & 0x4) != 0) {
                g_gameSlots[a][v4].flags18 |= 4;
                g_gameSlots[a][v4].field44 = g_gameSlots[a][b].field44;
            }
        }
        Game_Helper46d8c1(a, b, 4);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0048d477
int Game_CardScript48d477(int a, int b, int kind)
{
    if (kind == 0x80) {
        if ((g_game8c83f9 & 0x2) != 0
            && (signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field50 == a
            && g_gameSlots[g_game8e1aa8][g_game951bdc].field04 == b
            && g_gameSlots[a][b].field10 > 0) {
            g_gameSlots[a][b].field38 = 1;
        }
        return 0;
    }
    if (g_game94d178 == 0xd7
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && g_gameSlots[a][b].field10 > 0) {
        g_gameSlots[a][b].field38 = 1;
    }
    if ((g_game94d178 == 0xcd || kind == 0xc7)
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && g_gameSlots[a][b].field38 != 0
        && a == g_game8c8afc) {
        if (kind == 0x7d) {
            g_game7a01e0 |= 2;
        }
        if (kind == 0x7e || kind == 0xc7) {
            g_gameSlots[a][b].field12++;
            g_gameSlots[a][b].field1c++;
            Game_Helper49f61a(a, b, 1);
            g_gameSlots[a][b].field38 = 0;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0048f64a
int Game_CardScript48f64a(int a, int b, int kind)
{
    int v4;

    if (kind == 0x34
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        g_game7a01e0 |= 0x800 << Game_Helper4ba392(a, b, 1) - 1;
        v4 = g_game7a01e0;
        Game_Helper49c6b1(a, b, 1);
        g_game7a01e0 = v4;
        return 0;
    }
    if (kind == 0x6e
        && g_gameSlots[g_game8e1aa8][g_game951bdc].field6c == g_game93a7d4) {
        if (g_gameSlots[g_game8e1aa8][g_game951bdc].field04 == -1
            && (signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field0c == a
            && g_gameSlots[g_game8e1aa8][g_game951bdc].field48 == b
            && g_gameSlots[g_game8e1aa8][g_game951bdc].field38 != 0) {
            g_gameSlots[a][b].field38 = 1;
        }
        return 0;
    }
    if ((g_game94d178 == 0xcd || kind == 0xc7)
        && b == g_game951bdc
        && a == g_game8e1aa8
        && g_gameSlots[a][b].field38 != 0
        && a == g_game8c8afc) {
        if (kind == 0x7d) {
            g_game7a01e0 |= 2;
        }
        if (kind == 0x7e || kind == 0xc7) {
            Game_Helper49f483(a, b);
            g_gameSlots[a][b].field12++;
            g_gameSlots[a][b].field1c++;
            g_gameSlots[a][b].field38 = 0;
        }
    }
    return 0;
}


/* --- generated by cardgen (ghidra-work/scripts/cardgen.py) ---------------
 * Reconstructed by pattern-matching the disassembly, not by hand. Every one
 * of these was produced without reading it; the reccmp score is the check.
 * Names are addresses because nothing here establishes meaning. */

// FUNCTION: SHANDALAR 0x0047360e
int Game_CardScript47360e(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x6e
        && g_gameSlots[g_game8e1aa8][g_game951bdc].field6c == g_game93a7d4) {
        if (g_gameSlots[(signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field0c][g_gameSlots[g_game8e1aa8][g_game951bdc].field48].field6c != -1) {
            v8 = g_gameSlots[(signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field0c][g_gameSlots[g_game8e1aa8][g_game951bdc].field48].field6c;
        } else {
            v8 = g_gameSlots[(signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field0c][g_gameSlots[g_game8e1aa8][g_game951bdc].field48].field3c;
        }
        if (g_gameCards[v8].mask == 0x197) {
            if ((signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field50 == a
                && g_gameSlots[g_game8e1aa8][g_game951bdc].field04 == b) {
                g_gameSlots[g_game8e1aa8][g_game951bdc].field38 = 0;
            }
            if (g_game7a82e0 == a
                && (signed char)g_gameSlots[a][b].field24 != -1) {
                for (v4 = 0; g_game78e970[a] > v4; v4++) {
                    if (Game_Helper411d90(a, v4) != 0
                        && (signed char)g_gameSlots[a][b].field24 == (signed char)g_gameSlots[a][v4].field24
                        && (signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field50 == a
                        && g_gameSlots[g_game8e1aa8][g_game951bdc].field04 == v4) {
                        g_gameSlots[g_game8e1aa8][g_game951bdc].field38 = 0;
                    }
                }
            }
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004792c1
int Game_CardScript4792c1(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (kind == 0x1a
        && a == g_game7a82e0
        && (g_gameSlots[a][b].flags08 & 0x4) != 0) {
        ih = 1;
        ig = 1 - a;
        for (iv = 0; g_game78e970[ig] > iv; iv++) {
            if (Game_Helper411d90(ig, iv) != 0
                && (signed char)g_gameSlots[ig][iv].field24 == b) {
                ih = 0;
                break;
            }
        }
        if (ih != 0) {
            Game_Helper4883aa(a, 2, a, b);
        }
    }
    Game_CardScript49959d(a, b, kind);
    if (kind == 0x8a
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        g_game97db30 -= 0xc;
        return 0;
    }
    if (kind == 0xc7) {
        return 0;
        g_game94d63c += 0x30;
    }
    return 0;
}


/* --- generated by cardgen (ghidra-work/scripts/cardgen.py) ---------------
 * Reconstructed by pattern-matching the disassembly, not by hand. Every one
 * of these was produced without reading it; the reccmp score is the check.
 * Names are addresses because nothing here establishes meaning. */

// FUNCTION: SHANDALAR 0x00475328
int Game_CardScript475328(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x20010) == 0
            && Game_Helper4be650(0, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 1, 0, 0) != 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x90) {
        Game_Helper4c4263(0);
        return 0;
    }
    if (kind == 0x6d) {
        Game_Helper56ca10(0x586920, 0x586910);
        if (Game_Helper4c0efa(a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 1, 0, 0, 0x8aa920, 1, &v8) != 0) {
            g_gameSlots[a][b].field74 = v8;
            g_gameSlots[a][b].field78 = v4;
            g_gameSlots[a][b].field36 = 1;
            g_gameSlots[a][b].flags08 |= 0x10;
        } else {
            g_game8c83ec = 1;
        }
    }
    if (kind == 0x72) {
        v8 = g_gameSlots[a][b].field74;
        v4 = g_gameSlots[a][b].field78;
        if (Game_Helper4bea85(v8, v4, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 1, 0, 0) != 0) {
            Game_Helper46d8c1(v8, v4, 2);
        } else {
            g_game8c83ec = 1;
        }
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00475df0
int Game_CardScript475df0(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (kind == 0x6c
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && a == g_game8c7468
        && (g_game93a934 & 0x2) == 0) {
        Game_Helper476297(a, b, &ih);
        if (g_gameSlots[ih][ig].field6c != -1
            && g_gameSlots[ih][ig].field34 + g_gameSlots[ih][ig].field34 >= g_game8c83f0[a]) {
            g_game94d63c -= 0x30;
        }
    }
    if (kind == 0x73) {
        if (g_game94d464 == 4
            && g_game7a82e0 == g_game77c57c
            && a == g_game7a82e0
            && (g_gameSlots[a][b].field38 & 0x1) == 0
            && Game_Helper4be650(0, 0, a, 1 - a, 1 - a, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0x80, 0, 0) != 0) {
            g_gameSlots[a][b].field5c |= 0x101;
            g_game8c7420 |= 3;
            return 1;
        }
        return 0;
    }
    if (kind == 4
        && g_game951bdc == b
        && g_game8e1aa8 == a) {
        g_gameSlots[a][b].field38 |= 1;
        g_game7be9ac = 1;
        g_game7a01e0 |= 1;
    }
    if (kind == 0x86
        && Game_Helper4be650(0, 0, a, 1 - a, 1 - a, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0x80, 0, 0) != 0) {
        if (a == g_game8c776c || (g_game93a934 & 0x2) != 0) {
            Game_Helper56ca10(0x586988, 0x586978);
            Game_Helper4c0efa(a, 1 - a, 1 - a, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0x80, 0, 0, 0x8aa920, 0, &ih);
        } else {
            Game_Helper476297(g_game94f280, g_game930f5c, &ih);
        }
        if (ig != -1) {
            iv = Game_Helper4a9910(g_game94f280, g_game930f5c, g_game8aa730, ih, ig);
            if (iv != -1) {
                g_gameSlots[a][iv].field38 = 1 << Game_Helper4ba309(a, b, 3) - 1;
                g_gameSlots[a][iv].field4c = 1;
            }
        }
    }
    if (kind == 0x22) {
        g_gameSlots[a][b].field38 &= ~1;
    }
    if (kind == 0xc7
        && g_game94d464 == 0x1f
        && g_game77ed80[a][Game_Helper4ba309(a, b, 3)] == 0) {
        g_game94d63c += 0x60;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00478cc1
int Game_CardScript478cc1(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x20010) == 0
            && Game_Helper4be650(0, 0, a, 2, 1 - a, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0x40, 0, 0) != 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x90) {
        Game_Helper4c4263(0);
        return 0;
    }
    if (kind == 0x6d) {
        Game_Helper56ca10(0x586a04, 0x5869f4);
        if (Game_Helper4c0efa(a, 2, 1 - a, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0x40, 0, 0, 0x8aa920, 1, &v8) != 0) {
            g_gameSlots[a][b].field74 = v8;
            g_gameSlots[a][b].field78 = v4;
            g_gameSlots[a][b].field36 = 1;
            g_gameSlots[a][b].flags08 |= 0x10;
        } else {
            g_game8c83ec = 1;
        }
    }
    if (kind == 0x72) {
        v8 = g_gameSlots[a][b].field74;
        v4 = g_gameSlots[a][b].field78;
        if (Game_Helper4bea85(v8, v4, 0, a, 2, 2, 0x200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0x40, 0, 0) != 0) {
            Game_Helper46d8c1(v8, v4, 2);
        } else {
            g_game8c83ec = 1;
        }
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0047c106
int Game_CardScript47c106(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x20010) == 0
            && Game_Helper4be650(0, 0, a, a | 4, a | 4, 0x200, 0x40, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0) != 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x90) {
        Game_Helper4c4263(0);
        return 0;
    }
    if (kind == 0x6d) {
        Game_Helper56ca10(0x586ad8, 0x586ac4);
        if (Game_Helper4c0efa(a, a | 4, a | 4, 0x200, 0x40, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 0, &v8) != 0) {
            g_gameSlots[a][b].field74 = v8;
            g_gameSlots[a][b].field78 = v4;
            if (g_game8bd200 != 1) {
                Game_Helper40ebb9(0xf);
            }
            Game_Helper46d8c1(v8, v4, 3);
            g_gameSlots[a][b].flags08 |= 0x10;
        } else {
            g_game8c83ec = 1;
        }
    }
    if (kind == 0x72) {
        Game_Helper4415c5(a, Game_Helper4ba392(a, b, 1), (signed char)g_gameCards[g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].field3c].field2b + (signed char)g_gameCards[g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].field3c].field2c);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0047d2b6
int Game_CardScript47d2b6(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (b == g_game951bdc
        && a == g_game8e1aa8
        && g_game77ed80[a][7] == 0) {
        Game_Helper46d8c1(a, b, 2);
    }
    if (kind == 0x6c
        && b == g_game951bdc
        && a == g_game8e1aa8
        && g_game77ed80[a][7] < 2) {
        g_game94d63c -= 0x60;
    }
    if (kind == 0x87
        && g_game77ed80[a][7] == 0) {
        g_game7a01e0 |= 1;
    }
    if (kind == 0x85
        && b == g_game951bdc
        && a == g_game8e1aa8
        && g_gameSlots[a][b].field38 == 0
        && a == g_game7a82e0
        && a == g_game77c57c) {
        g_gameSlots[a][b].field5c |= 0x101;
    }
    if (kind == 4
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        g_gameSlots[a][b].field38++;
        if (g_game77ed80[a][7] != 0) {
            if (a == g_game8c7468
                && (g_game93a934 & 0x2) == 0) {
                Game_Helper487ce0(a, &ih, 0, 1 << Game_Helper4ba309(a, b, 2));
            } else {
                Game_Helper56ca10(0x586b38, 0x586b28);
                Game_Helper4c0efa(a, a, a, 0x200, 1, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 0, &ih);
            }
            if (g_game8bd200 != 1) {
                Game_Helper40ebb9(0xf);
            }
            if (Game_Helper488147(ih, ig, Game_Helper4ba309(a, b, 2)) != 0) {
                Game_Helper4bab38(a, 3, a, b);
            }
            Game_Helper46d8c1(ih, ig, 3);
        } else {
            g_game7a01e0 |= 1;
        }
    }
    if ((kind == 0x22 || kind == 0xc7)
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        g_gameSlots[a][b].field38 = 0;
    }
    if (kind == 0xc7
        && g_game94d464 == 0x1f
        && a != g_game7a82e0
        && a == g_game8c7468) {
        for (iv = 0; g_game78e970[a] > iv; iv++) {
            if (Game_Helper411d90(a, iv) != 0
                && (g_gameCards[g_gameSlots[a][iv].field6c].flags & 0x1) != 0) {
                Game_Helper46d8c1(a, iv, 2);
            }
        }
        if (g_game77ed80[a][Game_Helper4ba309(a, b, 2)] == g_game77ed80[a][7]) {
            Game_Helper4bab38(a, 3, a, b);
        }
    }
    if (kind == 0x8a
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        g_game97db30 -= 0x24;
    }
    if (kind == 0x8b
        && b == g_game951bdc
        && a == g_game8e1aa8) {
        g_game97db30 += 0x24;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00488e68
int Game_CardScript488e68(int a, int b, int kind)
{
    int iv;
    int ig[75];
    int ih;
    int ii;

    if ((g_game94d178 == 0xc9 || kind == 0xc7)
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && a == g_game7a82e0
        && a == g_game8c8afc) {
        if (kind == 0x7d) {
            g_game7a01e0 |= 2;
        }
        if (kind == 0x7e || kind == 0xc7) {
            if (g_game8bd200 != 1) {
                Game_Helper40ebb9(0x35);
            }
            if ((g_game93a934 & 0x2) != 0) {
                ih = Game_Helper522535(a, 5);
            } else {
                ih = Game_Helper522508(5);
            }
            iv = ih + 1;
            g_gameSlots[a][b].field1e = 1 << iv;
            Game_Helper56ca10(0x586cc4, 0x586cb0);
            Game_Helper57e820(ig, 0x586cd0);
            Game_Helper57e826(ig, iv * 300 + 0x8aa920);
            Game_Helper558538(a, a, b, -1, -1, ig, 0);
        }
    }
    if (kind == 0x73) {
        if (g_gameSlots[a][b].field38 == 0
            && Game_Helper441699(a, 7, 2) != 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x6d
        && Game_Helper441699(a, 7, 2) != 0) {
        Game_Helper4143e0(a, 0, 2);
        return 0;
    }
    if (kind == 0x72) {
        if (g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field6c != -1) {
            if (g_game8bd200 != 1) {
                Game_Helper40ebb9(0x35);
            }
            if ((g_game93a934 & 0x2) != 0) {
                ii = Game_Helper522535(g_game94f280, 5);
            } else {
                ii = Game_Helper522508(5);
            }
            iv = ii + 1;
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field1e = 1 << iv;
            if (g_game8bd200 != 1) {
                Game_Helper56ca10(0x586ce8, 0x586cd4);
                Game_Helper57e820(ig, 0x586cf4);
                Game_Helper57e826(ig, iv * 300 + 0x8aa920);
                Game_Helper558538(a, a, b, -1, -1, ig, 0);
            }
            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 |= 1 << g_game7a82e0;
            return 0;
        }
        g_game8c83ec = 1;
        return 0;
    }
    if (g_game951bdc == b
        && g_game8e1aa8 == a) {
        kind = 1;
    } else {
        kind = 0;
    }
    if (kind != 0) {
        g_gameSlots[a][b].field38 = 0;
        return 0;
    }
    return 0;
}


/* --- generated by cardgen (ghidra-work/scripts/cardgen.py) ---------------
 * Reconstructed by pattern-matching the disassembly, not by hand. Every one
 * of these was produced without reading it; the reccmp score is the check.
 * Names are addresses because nothing here establishes meaning. */

// FUNCTION: SHANDALAR 0x0047dd1e
int Game_CardScript47dd1e(int a, int b, int kind)
{
    int v4;
    int v8;

    if (g_game94d178 == 0xdf
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && a == g_game8c8afc
        && g_game7a82e0 != a
        && g_game7bf4b8 == a
        && g_game8e2fc4 == b) {
        if (kind == 0x7d) {
            g_game7a01e0 |= 2;
        }
        if (kind == 0x7e) {
            v8 = Game_Helper4ba25e(g_gameSlots[a][b].field6c);
            if (v8 != -1) {
                g_gameCards[v8].script = (int)Game_Callback48592f;
                g_gameCards[v8].flags3c = 0;
                g_gameCards[v8].mask = g_game8e32fc;
                v4 = Game_FindFreeSlot(a, v8);
                if (v4 != -1) {
                    g_gameSlots[a][v4].flags08 = g_gameSlots[a][b].flags08 & 0xfffffff7;
                    g_gameSlots[a][v4].field28 = g_gameSlots[a][b].field28;
                    g_gameSlots[a][v4].flags18 = 0x8000008;
                    g_gameSlots[a][v4].field38 = 2;
                    g_gameSlots[a][v4].field0c = a;
                    g_gameSlots[a][v4].field48 = b;
                    g_gameSlots[a][v4].field64 = g_gameCards[g_gameSlots[a][b].field6c].mask;
                }
            }
        }
    }
    if (g_game94d178 == 0xd4
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && a == g_game8c8afc
        && g_game7bf4b8 == a
        && g_game8e2fc4 == b) {
        if (kind == 0x7d) {
            g_game7a01e0 |= 2;
        }
        if (kind == 0x7e
            && (signed char)g_gameSlots[a][b].field24 != -1) {
            for (v8 = 0; g_game78e970[a] > v8; v8++) {
                if (Game_Helper411d90(a, v8) != 0
                    && g_gameCards[g_gameSlots[a][v8].field6c].script == (int)Game_Callback48592f
                    && (signed char)g_gameSlots[a][v8].field0c == a
                    && g_gameSlots[a][v8].field48 == b) {
                    Game_Helper46d8c1(a, v8, 2);
                }
            }
        }
    }
    if (kind == 0x77
        && g_game951bdc == b
        && g_game8e1aa8 == a
        && (signed char)g_gameSlots[a][b].field24 != -1) {
        for (v8 = 0; g_game78e970[a] > v8; v8++) {
            if (Game_Helper411d90(a, v8) != 0
                && g_gameCards[g_gameSlots[a][v8].field6c].script == (int)Game_Callback48592f
                && (signed char)g_gameSlots[a][v8].field0c == a
                && g_gameSlots[a][v8].field48 == b) {
                Game_Helper46d8c1(a, v8, 2);
            }
        }
    }
    return 0;
}


/* --- generated by cardgen (ghidra-work/scripts/cardgen.py) ---------------
 * Reconstructed by pattern-matching the disassembly, not by hand. Every one
 * of these was produced without reading it; the reccmp score is the check.
 * Names are addresses because nothing here establishes meaning. */

// FUNCTION: SHANDALAR 0x00470b00
int Game_CardScript470b00(int a, int b, int kind)
{
    int v4;

    if (kind == 0x3c) {
        g_gameSlots[a][b].field1f = (signed char)g_gameSlots[a][b].field1f | 0x3f;
        return 0;
    }
    if (kind == 0x77 && g_game951bdc == b && g_game8e1aa8 == a && (g_gameSlots[a][b].flags08 & 0x20) == 0 && (signed char)g_gameSlots[a][b].field68 != 4 && (g_game7a82e0 == a && (g_gameSlots[a][b].flags08 & 0x4) != 0 || g_game7a82e0 != a && (signed char)g_gameSlots[a][b].field24 != -1)) {
        v4 = Game_Helper56ae5d(a, g_game94d474);
        if (v4 != -1) {
            g_gameSlots[a][v4].field3c = g_gameSlots[a][b].field6c;
            g_gameSlots[a][v4].flags08 |= 2;
            g_gameSlots[a][v4].field64 = 0x187;
            g_gameSlots[a][v4].field14 = 0xd5;
            g_gameSlots[a][v4].field68 = 2;
            g_gameSlots[a][v4].field0c = a;
            g_gameSlots[a][v4].field48 = b;
            g_gameSlots[a][v4].field04 = (signed char)g_gameSlots[a][b].field24;
            if ((g_gameSlots[a][b].flags08 & 0x4) != 0) {
                g_gameSlots[a][v4].field38 = 0xfffffffe;
            } else {
                g_gameSlots[a][v4].field38 = (signed char)g_gameSlots[a][b].field24;
            }
            Game_Helper4111b1(a, v4);
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00472f6b
int Game_CardScript472f6b(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (kind == 0x6c && g_game8e1aa8 == a && (g_gameCards[g_gameSlots[g_game8e1aa8][g_game951bdc].field6c].flags & 0x40) != 0) {
        g_game94d63c += 0x30;
    }
    if (kind == 0x73) {
        return Game_Helper4be650(0, 0, a, a, a, 0x200, 0x40, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0);
    } else {
        if (kind == 0x90) {
            Game_Helper4c4263(0);
            return 0;
        } else {
            if (kind == 0x6d) {
                Game_Helper56ca10(0x586818, 0x586810);
                if (Game_Helper4c0efa(a, a, a, 0x200, 0x40, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &ih) != 0) {
                    if (g_game8bd200 != 1) {
                        Game_Helper40ebb9(0xf);
                    }
                    Game_Helper46d8c1(ih, ig, 3);
                    g_gameSlots[a][b].field74 = a;
                    g_gameSlots[a][b].field78 = b;
                    g_gameSlots[a][b].field36 = 1;
                    if (g_gameSlots[a][b].field38 == 0) {
                        g_gameSlots[a][b].field38 |= 0x80000;
                    }
                } else {
                    g_game8c83ec = 1;
                }
            }
            if (kind == 0x72) {
                if (g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field6c != -1) {
                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 += 2;
                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 += 0x200;
                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
                    if ((g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 & 0x80000) != 0) {
                        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 &= ~0x80000;
                        iv = Game_Helper4a9910(g_game94f280, g_game930f5c, g_game8bcff8, g_game94f280, g_game930f5c);
                        if (iv != -1) {
                            g_gameSlots[a][iv].field38 |= 0x80000;
                        }
                    }
                } else {
                    g_game8c83ec = 1;
                }
            }
            if (kind == 0x22 || kind == 0xc7) {
                g_gameSlots[a][b].field38 = 0;
            }
            if (kind == 0xc7 && g_game94d464 == 0x1f && a == g_game8c7468) {
                g_game94d63c += g_game8e3edc * 24;
            }
            return 0;
        }
    }
}

// FUNCTION: SHANDALAR 0x0047444f
int Game_CardScript47444f(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x73) {
        if ((g_gameSlots[a][b].flags08 & 0x20010) == 0) {
            return 1;
        }
        return 0;
    }
    if (kind == 0x90) {
        Game_Helper4c4263(1);
        return 0;
    }
    if (kind == 0x6d) {
        Game_Helper56ca10(0x586854, 0x586840);
        Game_Helper498e8a(a, b, 1);
        if (g_game8c83ec != 1) {
            g_gameSlots[a][b].flags08 |= 0x10;
        }
    }
    if (kind == 0x72) {
        Game_Helper49914d(a, b, kind, 1);
        if (g_game8c776c == a && (g_game93a934 & 0x2) == 0 || g_game8bd200 == 1) {
            Game_Helper474721(g_game94f280, g_game930f5c, a, &v8);
            Game_Helper56ca10(0x586878, 0x586864);
            Game_Helper558538(1 - a, a, b, v8, v4, 0x8aa920 + 0x12c, 0);
        } else {
            Game_Helper56ca10(0x58689c, 0x586888);
            Game_Helper4c0efa(1 - a, 2, a, 0x1200, 2, 0, 0, Game_Helper488cc4(a, b), 0, 0, -1, -1, -1, -1, 0, 0, 0, 0x8aa920, 1, &v8);
        }
        if (v8 != -1) {
            if (v4 == -1) {
                Game_Helper4bab38(v8, 1, g_game94f280, g_game930f5c);
            } else {
                Game_Helper4ba5a1(v8, v4, 1, g_game94f280, g_game930f5c);
            }
        }
        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00476ddc
int Game_CardScript476ddc(int a, int b, int kind)
{
    int v4;

    if (kind == 1) {
        g_game94d660[a][0]++;
    }
    if (kind == 0x6c && b == g_game951bdc && a == g_game8e1aa8) {
        g_gameSlots[a][b].field4c = 0;
        g_gameSlots[a][b].field38 = g_gameSlots[a][b].field4c;
    }
    if (kind == 0x73) {
        return Game_Helper441699(a, 4, 1);
    } else {
        if (kind == 0x90) {
            Game_Helper4c42ae(0);
            return 0;
        } else {
            if (kind == 0x6d) {
                if (Game_Helper441699(a, 4, 1) != 0) {
                    if (a == g_game7a82e0) {
                        Game_Helper4143e0(a, 4, -1);
                        if (g_game8c8274 > 0) {
                            g_gameSlots[a][b].field4c = g_game8c8274;
                        } else {
                            g_game8c83ec = 1;
                        }
                    } else {
                        Game_Helper4143e0(a, 4, 1);
                        g_gameSlots[a][b].field4c = 1;
                    }
                    if (g_game8c83ec != 1) {
                        g_gameSlots[a][b].field74 = a;
                        g_gameSlots[a][b].field78 = b;
                        g_gameSlots[a][b].field36 = 1;
                        if (g_gameSlots[a][b].field38 == 0) {
                            g_gameSlots[a][b].field38 |= 0x80000;
                        }
                    } else {
                        g_gameSlots[a][b].field4c = 0;
                    }
                }
            }
            if (kind == 0x72) {
                if (g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field6c != -1) {
                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 += (g_gameSlots[a][b].field4c & 0xff) * 256;
                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
                    if ((g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 & 0x80000) != 0) {
                        g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 &= ~0x80000;
                        v4 = Game_Helper4a9910(g_game94f280, g_game930f5c, g_game8bcff8, g_game94f280, g_game930f5c);
                        if (v4 != -1) {
                            g_gameSlots[a][v4].field38 |= 0x80000;
                        }
                    }
                } else {
                    g_game8c83ec = 1;
                }
            }
            if (kind == 0x3a) {
                return Game_Helper441699(a, 4, 1);
            } else {
                if (kind == 0x8f && g_game7806a0[a][4] != 0) {
                    g_game7a01e0 |= 1;
                }
                if (kind == 0xc7) {
                    if (a == g_game8c7468) {
                        g_game94d63c += (g_game77ed80[a][4] + g_game77ed80[a][4] * 2 + 3) * 4;
                    } else {
                        g_game94d63c -= -(0 - (g_game77ed80[a][4] + g_game77ed80[a][4] * 2 + 3) * 4);
                    }
                }
                if (kind == 0x22 || kind == 0xc7) {
                    g_gameSlots[a][b].field4c = 0;
                    g_gameSlots[a][b].field38 = g_gameSlots[a][b].field4c;
                }
                return 0;
            }
        }
    }
}

// FUNCTION: SHANDALAR 0x0047d1d2
int Game_CardScript47d1d2(int a, int b, int kind)
{
    int v4;

    if (g_game951bdc == b && g_game8e1aa8 == a) {
        if (g_game77ed80[a][Game_Helper4ba309(a, b, 1)] > 0) {
            if (kind == 0x32) {
                g_game7a01e0++;
            }
            if (kind == 0x33) {
                g_game7a01e0++;
            }
        }
    }
    if (kind == 1) {
        v4 = Game_Helper4ba309(a, b, 1);
        g_game94d650[a][v4] += 2;
    }
    if (kind == 0x73 || kind == 0x6d || kind == 0x72) {
        return Game_Helper4909b0(a, b, kind, 1, 1);
    } else {
        return 0;
    }
}

// FUNCTION: SHANDALAR 0x004829d8
int Game_CardScript4829d8(int a, int b, int kind)
{
    if (kind == 0x77 && g_game951bdc == b && g_game8e1aa8 == a && g_gameSlots[a][b].field04 != -1) {
        if ((g_gameSlots[a][b].flags18 & 0x1000000) != 0) {
            if ((signed char)g_gameSlots[a][b].field0c != -1) {
                g_gameSlots[(signed char)g_gameSlots[a][b].field0c][g_gameSlots[a][b].field48].flags18 |= 0x1000000;
                if ((signed char)g_gameSlots[a][b].field0c != (signed char)g_gameSlots[a][b].field50) {
                    Game_Helper4d40a4((signed char)g_gameSlots[a][b].field50, g_gameSlots[a][b].field04);
                }
            } else {
                if (g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field6c != -1 && ((g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags08 & 0x400000) != 0 && (signed char)g_gameSlots[a][b].field50 == g_game8c776c || (g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags08 & 0x400000) == 0 && (signed char)g_gameSlots[a][b].field50 == g_game8c7468)) {
                    Game_Helper4d40a4((signed char)g_gameSlots[a][b].field50, g_gameSlots[a][b].field04);
                }
            }
        } else {
            Game_Helper49f3c0((int)Game_Callback4d3f22, -1);
        }
    }
    if (kind == 0x8a && g_gameSlots[a][b].field04 == g_game951bdc && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8 && g_game951bdc != -1) {
        g_game97db30 -= 0x18;
    }
    if (kind == 0x8b && g_gameSlots[a][b].field04 == g_game951bdc && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8 && g_game951bdc != -1) {
        g_game97db30 += 0x18;
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00484d55
int Game_CardScript484d55(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x21 && (g_game94d464 == 0x1a || g_game94d464 == 0x19) && g_gameSlots[g_game8e1aa8][g_game951bdc].field6c == g_game93a7d4 && g_gameSlots[g_game8e1aa8][g_game951bdc].field38 != 0) {
        v8 = (signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field50;
        v4 = g_gameSlots[g_game8e1aa8][g_game951bdc].field04;
        if (v8 == g_game7a82e0) {
            if ((g_gameSlots[v8][v4].flags08 & 0x4) != 0 && (signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field0c == (signed char)g_gameSlots[a][b].field50 && g_gameSlots[g_game8e1aa8][g_game951bdc].field48 == g_gameSlots[a][b].field04) {
                g_gameSlots[g_game8e1aa8][g_game951bdc].field38 = 0;
            }
        } else {
            if ((signed char)g_gameSlots[a][b].field50 == v8 && g_gameSlots[a][b].field04 == v4) {
                g_gameSlots[g_game8e1aa8][g_game951bdc].field38 = 0;
            }
        }
    }
    if (kind == 0x22 || kind == 0xc7) {
        Game_Helper46d8c1(a, b, 1);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0048d76b
int Game_CardScript48d76b(int a, int b, int kind)
{
    int v4;

    if (kind == 0x6c && g_game951bdc == b && g_game8e1aa8 == a) {
        g_gameSlots[a][b].field4c = 0;
        g_gameSlots[a][b].field38 = g_gameSlots[a][b].field4c;
        return 0;
    } else {
        if (kind == 0x73) {
            return Game_Helper441699(a, 7, 1);
        } else {
            if (kind == 0x90) {
                Game_Helper4c42ae(0);
                return 0;
            } else {
                if (kind == 0x6d) {
                    if (Game_Helper441699(a, 7, 1) != 0) {
                        if (g_game7a82e0 == a) {
                            Game_Helper4143e0(a, 0, -1);
                            if (g_game8c8274 > 0) {
                                g_gameSlots[a][b].field4c = g_game8c8274;
                            } else {
                                g_game8c83ec = 1;
                            }
                        } else {
                            Game_Helper4143e0(a, 0, 1);
                            g_gameSlots[a][b].field4c = 1;
                        }
                        if (g_game8c83ec != 1) {
                            g_gameSlots[a][b].field74 = a;
                            g_gameSlots[a][b].field78 = b;
                            g_gameSlots[a][b].field36 = 1;
                            if (g_gameSlots[a][b].field38 == 0) {
                                g_gameSlots[a][b].field38 |= 0x80000;
                            }
                        } else {
                            g_gameSlots[a][b].field4c = 0;
                        }
                    }
                    return 0;
                } else {
                    if (kind == 0x72) {
                        if (g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field6c != -1) {
                            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 += g_gameSlots[a][b].field4c & 0xff;
                            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 += (g_gameSlots[a][b].field4c & 0xff) * 256;
                            g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
                            if ((g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 & 0x80000) != 0) {
                                g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 &= ~0x80000;
                                v4 = Game_Helper4a9910(g_game94f280, g_game930f5c, g_game8bcff8, g_game94f280, g_game930f5c);
                                if (v4 != -1) {
                                    g_gameSlots[a][v4].field12 = 1;
                                    g_gameSlots[a][v4].field1c = 1;
                                    g_gameSlots[a][v4].field38 |= 0x80000;
                                }
                            }
                        } else {
                            g_game8c83ec = 1;
                        }
                        return 0;
                    } else {
                        if (kind == 0x39) {
                            return Game_Helper441699(a, 7, 1);
                        } else {
                            if (kind == 0x3a) {
                                return Game_Helper441699(a, 7, 1);
                            } else {
                                if (kind == 0x8f) {
                                    g_game7a01e0 |= 1;
                                    return 0;
                                } else {
                                    if (kind == 0xc7) {
                                        if (g_game8c7468 == a) {
                                            g_game94d63c += (g_game77ed80[a][7] + g_game77ed80[a][7] * 2 + 6) * 4;
                                        } else {
                                            g_game94d63c -= -(0 - (g_game77ed80[a][7] + g_game77ed80[a][7] * 2 + 6) * 4);
                                        }
                                    }
                                    if (kind == 0x22 || kind == 0xc7) {
                                        g_gameSlots[a][b].field4c = 0;
                                        g_gameSlots[a][b].field38 = g_gameSlots[a][b].field4c;
                                    }
                                    return 0;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

// FUNCTION: SHANDALAR 0x0048e066
int Game_CardScript48e066(int a, int b, int kind)
{
    int v4;

    if (kind == 1) {
        g_game94d660[a][0]++;
        return 0;
    } else {
        if (kind == 0x6c && g_game951bdc == b && a == g_game8e1aa8) {
            g_gameSlots[a][b].field4c = 0;
            g_gameSlots[a][b].field38 = g_gameSlots[a][b].field4c;
            return 0;
        } else {
            if (kind == 0x73) {
                return Game_Helper441699(a, 4, 1);
            } else {
                if (kind == 0x90) {
                    Game_Helper4c42ae(0);
                    return 0;
                } else {
                    if (kind == 0x6d) {
                        if (Game_Helper441699(a, 4, 1) != 0) {
                            if (a == g_game7a82e0) {
                                Game_Helper4143e0(a, 4, -1);
                                if (g_game8c8274 > 0) {
                                    g_gameSlots[a][b].field4c = g_game8c8274;
                                } else {
                                    g_game8c83ec = 1;
                                }
                            } else {
                                Game_Helper4143e0(a, 4, 1);
                                g_gameSlots[a][b].field4c = 1;
                            }
                            if (g_game8c83ec != 1) {
                                g_gameSlots[a][b].field74 = a;
                                g_gameSlots[a][b].field78 = b;
                                g_gameSlots[a][b].field36 = 1;
                                if (g_gameSlots[a][b].field38 == 0) {
                                    g_gameSlots[a][b].field38 |= 0x80000;
                                }
                            } else {
                                g_gameSlots[a][b].field4c = 0;
                            }
                        }
                        return 0;
                    } else {
                        if (kind == 0x72) {
                            if (g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field6c != -1) {
                                g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 += g_gameSlots[a][b].field4c & 0xff;
                                g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field36 = 0;
                                if ((g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 & 0x80000) != 0) {
                                    g_gameSlots[g_gameSlots[a][b].field10c][g_gameSlots[a][b].field110].field38 &= ~0x80000;
                                    v4 = Game_Helper4a9910(g_game94f280, g_game930f5c, g_game8bcff8, g_game94f280, g_game930f5c);
                                    if (v4 != -1) {
                                        g_gameSlots[a][v4].field38 |= 0x80000;
                                    }
                                }
                            } else {
                                g_game8c83ec = 1;
                            }
                            return 0;
                        } else {
                            if (kind == 0x39) {
                                return Game_Helper441699(a, 4, 1);
                            } else {
                                if (kind == 0x8f && g_game7806a0[a][4] != 0) {
                                    g_game7a01e0 |= 1;
                                    return 0;
                                } else {
                                    if (kind == 0xc7) {
                                        if (a == g_game8c7468) {
                                            g_game94d63c += (g_game77ed80[a][4] + g_game77ed80[a][4] * 2 + 3) * 4;
                                        } else {
                                            g_game94d63c -= -(0 - (g_game77ed80[a][4] + g_game77ed80[a][4] * 2 + 3) * 4);
                                        }
                                    }
                                    if (kind == 0x22 || kind == 0xc7) {
                                        g_gameSlots[a][b].field4c = 0;
                                        g_gameSlots[a][b].field38 = g_gameSlots[a][b].field4c;
                                    }
                                    return 0;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}


/* --- generated by cardgen (ghidra-work/scripts/cardgen.py) ---------------
 * Reconstructed by pattern-matching the disassembly, not by hand. Every one
 * of these was produced without reading it; the reccmp score is the check.
 * Names are addresses because nothing here establishes meaning. */

// FUNCTION: SHANDALAR 0x00474f5d
int Game_CardScript474f5d(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;

    if (kind == 0x78 && g_game8e26b0 == b && g_game94e928 == a) {
        iv = 0;
        ig = 0;
        for (; g_game78e970[1 - a] > iv && ig == 0; iv++) {
            if (Game_Helper411d90(1 - a, iv) != 0) {
                if (g_gameCards[g_gameSlots[1 - a][iv].field6c].mask == 0x197) {
                    ig = 1;
                }
            }
        }
        if (ig != 0) {
            g_game7a01e0 = 1;
        }
    }
    if (kind == 0x6e && g_gameSlots[g_game8e1aa8][g_game951bdc].field6c == g_game93a7d4) {
        if (g_gameSlots[(signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field0c][g_gameSlots[g_game8e1aa8][g_game951bdc].field48].field6c != -1) {
            ih = g_gameSlots[(signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field0c][g_gameSlots[g_game8e1aa8][g_game951bdc].field48].field6c;
        } else {
            ih = g_gameSlots[(signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field0c][g_gameSlots[g_game8e1aa8][g_game951bdc].field48].field3c;
        }
        if (g_gameCards[ih].mask == 0x197 && (signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field50 == a && g_gameSlots[g_game8e1aa8][g_game951bdc].field04 == b) {
            g_gameSlots[g_game8e1aa8][g_game951bdc].field38 = 0;
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004775a8
int Game_CardScript4775a8(int a, int b, int kind)
{
    int v4;
    int v8;

    if (kind == 0x71 || (g_gameSlots[a][b].flags08 & 0x10) != 0) {
        g_gameSlots[a][b].field38 = 1;
    }
    if (kind == 0x3c) {
        if (Game_Helper411d90(a, b) != 0) {
            if (a == g_game8e1aa8 && (g_gameCards[g_game7a01e0].flags & 0x42) == 0x40) {
                if ((g_game8c83fa & 0x2) != 0) {
                    if ((g_gameSlots[g_game8e1aa8][g_game951bdc].flags18 & 0x4000000) == 0) {
                        v8 = Game_Helper4a9910(a, b, g_game8e3104, g_game8e1aa8, g_game951bdc);
                        if (v8 != -1) {
                            g_gameSlots[a][v8].field74 = -1;
                            g_gameSlots[a][v8].field78 = -1;
                            g_gameSlots[a][v8].flags18 |= 0x10020;
                            g_gameSlots[g_game8e1aa8][g_game951bdc].flags18 |= 0x4000000;
                        }
                    }
                } else {
                    g_game8c83f8 |= 0x10000;
                }
            }
        }
    }
    if (kind == 0x34 && (g_gameSlots[a][b].flags08 & 0x10) == 0) {
        if (a == g_game8e1aa8 && (g_gameCards[g_gameSlots[g_game8e1aa8][g_game951bdc].field6c].flags & 0x42) == 0x40) {
            g_game7a01e0 |= 0x20000;
        }
        if (g_gameSlots[a][b].field38 != 0) {
            g_gameSlots[a][b].field38 = 0;
            Game_Helper488b70(a, b, (int)Game_Callback477b57, -1);
        }
    }
    if (kind == 0x77 && (g_gameSlots[a][b].flags08 & 0x10) == 0 && (signed char)g_gameSlots[g_game8e1aa8][g_game951bdc].field68 == 2) {
        if (Game_Helper411d90(g_game8e1aa8, g_game951bdc) != 0) {
            if (a == g_game8e1aa8 && (g_gameCards[g_gameSlots[g_game8e1aa8][g_game951bdc].field6c].flags & 0x42) == 0x40) {
                g_gameSlots[g_game8e1aa8][g_game951bdc].field68 = 0;
                g_gameSlots[g_game8e1aa8][g_game951bdc].field14 = 0;
                g_game7a01e0++;
            }
        }
    }
    if (kind == 0x77 && b == g_game951bdc && a == g_game8e1aa8) {
        v4 = 0;
        for (; g_game78e970[a] > v4; v4++) {
            if (g_gameSlots[a][v4].field6c == g_game8e3104) {
                if ((signed char)g_gameSlots[a][v4].field0c == a) {
                    if (g_gameSlots[a][v4].field48 == b) {
                        Game_Helper46d8c1(a, v4, 2);
                    }
                }
            }
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00480feb
int Game_CardScript480feb(int a, int b, int kind)
{
    int v4;
    int v8;

    if ((signed char)g_gameSlots[a][b].field0c != a && (g_gameSlots[a][b].flags18 & 0x80) == 0) {
        Game_Helper46d8c1(a, b, 2);
    }
    if (kind == 0x77 && b == g_game951bdc && a == g_game8e1aa8 || kind == 0x81 && (signed char)g_gameSlots[a][b].field0c == g_game8e1aa8 && g_gameSlots[a][b].field48 == g_game951bdc) {
        v4 = 0;
        v8 = 0;
        for (; g_game78e970[a] > v4 && v8 == 0; v4++) {
            if (g_gameSlots[a][v4].field6c == g_game8e3104 && b != v4 && (signed char)g_gameSlots[a][v4].field50 == (signed char)g_gameSlots[a][b].field50 && g_gameSlots[a][b].field04 == g_gameSlots[a][v4].field04) {
                v8 = 1;
            }
        }
        if (v8 == 0) {
            if (kind == 0x77) {
                g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].flags18 &= ~0x4000000;
                g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field28 &= ~0x20000;
            }
            if (g_gameSlots[a][b].field78 != -1) {
                g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].flags18 |= 0x1000000;
                if ((signed char)g_gameSlots[a][b].field50 != g_gameSlots[a][b].field74) {
                    v4 = Game_Helper4d40a4((signed char)g_gameSlots[a][b].field50, g_gameSlots[a][b].field04);
                    g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].field50 = g_gameSlots[a][b].field50;
                    g_gameSlots[g_gameSlots[a][b].field74][g_gameSlots[a][b].field78].field04 = g_gameSlots[a][b].field04;
                }
            }
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x0048486e
int Game_CardScript48486e(int a, int b, int kind)
{
    int v4;

    if (kind == 0x78 && g_gameSlots[a][b].field04 == g_game951bdc && (signed char)g_gameSlots[a][b].field50 == g_game8e1aa8 && g_game951bdc != -1 && (g_gameSlots[(signed char)g_gameSlots[a][b].field50][g_gameSlots[a][b].field04].field28 & 0x20) == 0) {
        v4 = 0;
        for (; g_game78e970[g_game94e928] > v4 && g_game7a01e0 == 0; v4++) {
            if (g_gameSlots[g_game94e928][v4].field6c == g_game8c7d54 && (signed char)g_gameSlots[g_game94e928][v4].field50 == g_game94e928 && g_gameSlots[g_game94e928][v4].field04 == g_game8e26b0 && g_gameSlots[a][b].field4c != g_gameSlots[g_game94e928][v4].field4c) {
                g_game7a01e0 = 1;
            }
        }
    }
    if (g_game94d178 == 0xcc && g_game951bdc == b && a == g_game8e1aa8 && a == g_game8c8afc) {
        if (kind == 0x7d) {
            g_game7a01e0 |= 2;
        }
        if (kind == 0x7e) {
            Game_Helper46d8c1(a, b, 1);
        }
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x004875d7
int Game_CardScript4875d7(int a, int b, int kind)
{
    int iv;
    int ig;
    int ih;
    int ii;

    if (kind == 0xc7 && g_game94d464 == 0x22 && b == g_game951bdc && a == g_game8e1aa8) {
        ih = 0;
        ii = 1 - a;
        iv = 0;
        for (; g_game78e970[ii] > ih; ih++) {
            if ((1 << Game_Helper4ba392(a, b, 4) & 0x15) != 0) {
                if (Game_Helper411d90(ii, ih) != 0) {
                    iv += 0x18;
                }
            }
        }
        if (iv != 0) {
            g_game94d63c += iv + 0x18;
        }
    }
    if (kind == 0x34 && b == g_game951bdc && a == g_game8e1aa8) {
        g_game7a01e0 |= 0x800 << Game_Helper4ba392(a, b, 4) - 1;
        ig = g_game7a01e0;
        Game_Helper49c6b1(a, b, 4);
        g_game7a01e0 = ig;
    }
    return 0;
}
