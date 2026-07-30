/* Functions exported by Shandalar.exe.
 *
 * DECKDLL.dll's DeckBuilderMain resolves these by name via
 * GetModuleHandleA(NULL) + GetProcAddress, so they carry their original names.
 * All five also exist in Magic.exe (0x004a5929, 0x004a59ad, 0x004a59ea,
 * 0x004a5a1b, 0x004e1de1), so the code is known-shared between the EXEs.
 *
 * CardIDFromType returns the +0x24 field of a GameCard, which makes that field
 * the card's ID and the array index its "type"; the field keeps the name
 * `mask` for now. */
#include "game/gameslot.h"

// GLOBAL: SHANDALAR 0x0097f1c0
extern int deck[4096];   /* extent unknown; indexed by card type */

// FUNCTION: SHANDALAR 0x00557aa9
int CardTypeFromID(int a1)
{
    int iv;   /* [ebp-4] */
    int ig;   /* [ebp-8] */

    if (a1 == -1) {
        return -1;
    } else {
        iv = -1;
        ig = 0;
        for (; g_gameCards[ig].mask != -1 && iv == -1; ig++) {
            if (g_gameCards[ig].mask == a1) {
                iv = ig;
            }
        }
        return iv;
    }
}

// FUNCTION: SHANDALAR 0x00557b2d
int CardIDFromType(int a1)
{
    if (a1 == -1) {
        return -1;
    } else {
        a1 &= 0xfff;
        return g_gameCards[a1].mask;
    }
}

// FUNCTION: SHANDALAR 0x00557b6a
int CardInDeck(int a1)
{
    if (a1 == -1) {
        return -1;
    } else {
        return a1 & 0x4000;
    }
}

// FUNCTION: SHANDALAR 0x00557b9b
/* Void: eax happens to hold the array index on both arms, which is what /Od
 * leaves behind. The clear arm really is `&= 0x8fff` (drops bits 12-13 too),
 * exactly as the original has it. */
void SetCardInDeck(int a1, int a2)
{
    if (a2 == 1) {
        deck[a1] |= 0x4000;
    } else {
        deck[a1] &= 0x8fff;
    }
}
