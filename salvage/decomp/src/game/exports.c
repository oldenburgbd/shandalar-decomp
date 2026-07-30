/* The functions Shandalar.exe EXPORTS.
 *
 * Found by reconstructing Deck.exe and then reading DECKDLL.dll: DeckBuilderMain
 * resolves its host callbacks with GetModuleHandleA(NULL) + GetProcAddress, by
 * name, so the deck builder calls back into whichever EXE loaded it. The names
 * it asks for are in the export table of both Shandalar.exe and Magic.exe --
 * eleven symbols that arrive already named, which is rare here.
 *
 * All five functions exist in both EXEs (Magic at 0x004a5929, 0x004a59ad,
 * 0x004a59ea, 0x004a5a1b, 0x004e1de1), so each one reconstructed here is
 * guaranteed shared rather than hoped to be.
 *
 * The card table settles a name too. CardIDFromType returns the +0x24 field of
 * a GameCard, so that field is the card's ID and the array index is its "type";
 * it was called `mask` on the evidence of being compared against three globals.
 * Renaming it is left for a pass that can measure the churn.
 */
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
/* Returns nothing. Both arms leave eax holding the card type only because that
 * is the array index they just computed -- which is what a void function looks
 * like at /Od, and why the generator offered `return a1`.
 *
 * The clear path is `&= 0x8fff`, not `&= ~0x4000`: it drops bits 12 and 13
 * along with the in-deck bit. Kept as the original has it. */
void SetCardInDeck(int a1, int a2)
{
    if (a2 == 1) {
        deck[a1] |= 0x4000;
    } else {
        deck[a1] &= 0x8fff;
    }
}

