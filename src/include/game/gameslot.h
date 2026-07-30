/* Declaration surface for the game layer.
 *
 * ADOPTED WHOLESALE from the archived attempt (salvage/decomp/include/game/
 * gameslot.h) rather than re-derived: MSVC 4.2 instruction selection at /Od
 * depends on the set and order of declarations in scope (measured in the
 * archive, re-confirmed here: the sparse first version of this header
 * flipped cmp operand order and array-index term order in four functions).
 * The declarations are names, types and offsets established from the
 * binary; the archive is their provenance, and this header is the tuned
 * configuration its 100% scores were measured under. */


#ifndef GAME_SLOT_H
#define GAME_SLOT_H

/* Records of 300 bytes in rows of 151, at 0x008ca270.
 *
 * Both numbers are read out of the index arithmetic rather than guessed: the
 * column index is multiplied by 300 and the row index by 45300, and
 * 45300 / 300 is exactly 151.
 *
 * The base is 0x008ca270. It was first recorded as 0x008ca260, on the reasoning
 * that 0x008ca260 is the lowest displacement in the cluster -- which was the
 * right question asked carelessly. 0x008ca260 and 0x008ca264 are referenced 37
 * and 2 times and **never with an index register**; every displacement from
 * 0x008ca270 upward is always indexed. They are two scalars that happen to sit
 * in front of the table, not fields of it.
 *
 * The corroboration is a memcpy: 0x0040f77c copies 300 bytes starting at
 * 0x008ca270, and a whole-record copy has to begin at a record boundary.
 *
 * This is the second time a table's base has been off in this repo (SidFont, by
 * 132 bytes). The test that settles it is cheap -- ask whether the low
 * displacements are ever indexed -- and it is worth running every time. */
#define GAME_SLOT_BYTES        300
#define GAME_SLOTS_PER_OWNER   151
/* The row count is NOT established. Nothing bounds it in the code read so far;
 * eight is enough to link and is the one number here that is a placeholder. */
#define GAME_SLOT_OWNERS       8

typedef struct GameSlot {
    int   field00;                   /* +0x000                                */
    int   field04;                   /* +0x004                                */
    int   flags08;                   /* +0x008  bit 2 tested; 0x1002 or 2 set  */
    unsigned char field0c;           /* +0x00c                                */
    unsigned char unknown00d[1];     /* +0x00d                                */
    short field0e;                   /* +0x00e                                */
    short field10;                   /* +0x010                                */
    short field12;                   /* +0x012                                */
    int   field14;                   /* +0x014                                */
    int   flags18;                   /* +0x018  0x100000 / 0x40000 set         */
    short field1c;                   /* +0x01c                                */
    unsigned char field1e;           /* +0x01e  copied from the source slot    */
    signed char field1f;             /* +0x01f  movsx loaded, byte stored      */
    int   field20;                   /* +0x020                                */
    signed char field24;             /* +0x024  compared against -1            */
    unsigned char unknown025[3];     /* +0x025                                */
    int   field28;                   /* +0x028  bit 0x20 tested as a byte      */
    unsigned char field2c;           /* +0x02c                                */
    unsigned char unknown02d[7];     /* +0x02d                                */
    short field34;                   /* +0x034                                */
    unsigned char field36;           /* +0x036                                */
    unsigned char field37;           /* +0x037                                */
    int   field38;                   /* +0x038                                */
    int   field3c;                   /* +0x03c                                */
    unsigned char unknown040[4];     /* +0x040                                */
    unsigned char field44;           /* +0x044                                */
    unsigned char unknown045[3];     /* +0x045                                */
    int   field48;                   /* +0x048                                */
    int   field4c;                   /* +0x04c  card record's flags byte       */
    unsigned char field50;           /* +0x050                                */
    unsigned char unknown051[3];     /* +0x051                                */
    int   field54;                   /* +0x054                                */
    unsigned char field58;           /* +0x058                                */
    unsigned char unknown059[3];     /* +0x059                                */
    int   field5c;                   /* +0x05c                                */
    unsigned char unknown060[4];     /* +0x060                                */
    int   field64;                   /* +0x064                                */
    unsigned char field68;           /* +0x068                                */
    unsigned char unknown069[3];     /* +0x069                                */
    int   field6c;                   /* +0x06c                                */
    unsigned char unknown070[4];     /* +0x070                                */
    int   field74;                   /* +0x074                                */
    int   field78;                   /* +0x078                                */
    unsigned char unknown07c[0x90];  /* +0x07c                                */
    int   field10c;                  /* +0x10c  the row a copy came from       */
    int   field110;                  /* +0x110  the column a copy came from    */
    unsigned char unknown114[8];     /* +0x114                                */
    int   field11c;                  /* +0x11c                                */
    unsigned char unknown120[4];     /* +0x120                                */
    /* Five one-byte members, each read with movsx and written as a byte -- so
     * signed char fields, not a narrowed wider operation. */
    signed char field124;            /* +0x124                                */
    signed char field125;            /* +0x125                                */
    signed char field126;            /* +0x126                                */
    signed char field127;            /* +0x127                                */
    unsigned char unknown128[1];     /* +0x128                                */
    signed char field129;            /* +0x129                                */
    unsigned char unknown12a[GAME_SLOT_BYTES - 0x12a];
} GameSlot;

extern GameSlot g_gameSlots[GAME_SLOT_OWNERS][GAME_SLOTS_PER_OWNER];

/* A second table at 0x00594208, indexed by the value this function calls a
 * card. Records are 72 bytes: the index is shifted left three and then
 * multiplied by nine via `[eax + eax*8]`. Reading only the second half of that
 * pair gives nine, which is what a disassembly listing with the shifts filtered
 * out looks like -- the filter that made the control flow readable also hid the
 * stride.
 *
 * The base was first recorded as 0x0059422c, which is the first field anything
 * reconstructed here happened to touch -- the same mistake as SidFont and the
 * slot table, now three for three. The test that settles it: sweep .text for
 * every `[reg + reg*8 + disp]` in this range and take the lowest displacement.
 * That is 0x00594208, and the highest is 0x0059424c read as a dword, which ends
 * at 0x00594250 -- exactly 72 bytes after 0x00594208. A record whose last field
 * closes flush on the stride is a boundary, not a coincidence.
 *
 * Nothing below 0x00594208 is ever indexed, so the front of the record is
 * either unused or only touched by code not yet read; the base could still be
 * 72 lower. The flush-fit is what argues it is not. */
#define GAME_CARD_BYTES 72

/* Every card record carries a code pointer at +0x34, and it is how the game
 * layer runs card behaviour: 23 `call dword ptr [eax + eax*8 + 0x0059423c]`
 * sites, plus 50 `cmp` sites that test a card's handler against a particular
 * function's address. All 52 immediates compared against it are .text
 * addresses, so this is a dispatch table, not data that happens to look like
 * one.
 *
 * The signature is read off the call sites: three arguments pushed and
 * `add esp, 0xc` afterwards, so __cdecl with three ints. At 0x0040e2fa the
 * caller indexes the card table with a slot's field6c, which is what makes
 * field6c a card id rather than an opaque number.
 *
 * THE TABLE IS STATICALLY INITIALISED IN THE IMAGE, so it -- not the code --
 * is the authoritative list of handlers. Scanning .text for named addresses
 * finds 23; reading the +0x34 pointer out of all 1,009 records finds 722 cards
 * carrying 608 DISTINCT SCRIPTS, together 603,363 bytes, or 38.6% of
 * Shandalar.exe's .text. The card layer is the largest single thing in the
 * binary by a wide margin.
 *
 * 0x00488ca0 is the handler for 88 of those cards and does nothing at all,
 * which makes it the table's null entry.
 *
 * `ghidra-work/scripts/cardscripts.py` regenerates the ranking. */
typedef int (*GameCardScript)(int a, int b, int kind);

typedef struct GameCard {
    signed char   field00;           /* +0x00  compared against -1             */
    unsigned char unknown001[0x23];  /* +0x01                                  */
    int   mask;                      /* +0x24  compared against three globals  */
    unsigned char flags;             /* +0x28  bits 0x40 and 0x02 tested       */
    signed char field29;             /* +0x29  compared against 0              */
    unsigned char unknown02a[1];     /* +0x2a                                  */
    unsigned char field2b;           /* +0x2b                                  */
    unsigned char field2c;           /* +0x2c                                  */
    unsigned char unknown02d[7];     /* +0x2d                                  */
    GameCardScript script;           /* +0x34  card behaviour; see above       */
    int   field38;                   /* +0x38                                  */
    int   flags3c;                   /* +0x3c  0x1001/0x18/0x40 tested wide,   */
                                     /*        and byte +0x3d against 0x10     */
    unsigned char unknown040[GAME_CARD_BYTES - 0x40];
} GameCard;

/* The row count here is not established either; the table has to be big enough
 * to index with whatever field4c/field7c hold. */
extern GameCard g_gameCards[1024];

/* Globals this module reads. Named by address because nothing establishes what
 * any of them mean. */
/* The two scalars immediately before the table -- never indexed, so not part
 * of it. */
extern int g_game8ca260;
extern int g_game8ca264;

extern int g_game93a7d4;
extern int g_game92e95c;
extern int g_game94d464;
extern int g_game7a04c4;
extern int g_game8bcfe8;
extern int g_game8e32fc;
extern int g_game8c83f8;

/* Returns a free column in the owner's row, or -1. */
int Game_FindFreeSlot(int owner, int arg);
/* Combines a card mask with two ids. Sits immediately before the predicate at
 * 0x00466fe6, so the two are in the same object file. */
int Game_CombineMask(int mask, int a, int b);

/* --- the event log ------------------------------------------------------ */

/* Five parallel arrays and a count, appended to together and capped at 32.
 * Nothing here establishes what they record; the shapes are what the indexing
 * proves -- three arrays of one int, two of a pair. */
#define GAME_EVENT_MAX 32

typedef struct GameEventPair {
    int a;
    int b;
} GameEventPair;

extern int           g_gameEventCount;              /* 0x008c6ae4 */
extern int           g_gameEventWord[GAME_EVENT_MAX];   /* 0x0094d2b0 */
extern GameEventPair g_gameEventWhere[GAME_EVENT_MAX];  /* 0x00939310 */
extern GameEventPair g_gameEventWhat[GAME_EVENT_MAX];   /* 0x0093a940 */
extern int           g_gameEventMode[GAME_EVENT_MAX];   /* 0x008a9230 */
extern int           g_gameEventExtra[GAME_EVENT_MAX];  /* 0x007be4e0 */

extern int g_game7befec;
extern int g_game94d178;
extern int g_game8bd200;

/* memcpy-shaped: (dst, src, n). Not reconstructed. */
void Game_CopyBlock(void *dst, const void *src, int n);

/* --- the reset ----------------------------------------------------------- */

/* Tables the reset clears. The two-dimensional shapes are what the index
 * arithmetic proves: a row stride of 32 bytes over eight ints, and rows of 204
 * and 44 bytes reached through the usual lea chains (5x, 41x, 205x, -x for
 * 204; 5x, 45x, -x for 44). Only element zero of the last two is ever written
 * here, so their widths are stride-derived and their contents unknown. */
extern int g_game7bfee0[2];
extern int g_game7806a0[2][8];
extern int g_game77ed80[2][8];
extern int g_game77d170[2][8];
extern int g_game74c790[2][51];
extern int g_game74cdc0[2][11];
extern int g_game7806e0[2];
extern int g_game8e2690[8];
extern int g_game7be740[8];

/* The index the card table is walked from -- the reset clears sixteen records
 * starting here. */
extern int g_game73c00c;

/* Two indices into g_game7be740, set at the top of the reset and used at the
 * bottom. */
extern int g_game8c776c;
extern int g_game8c7468;

extern int g_game8e26bc;
extern int g_game78e5d8;
extern int g_game74f1e0;
extern int g_game77f440;
extern int g_game74d26c;
extern int g_game74ccc4;
extern int g_game77f600;
extern int g_game7a81a8;
extern int g_game7beb20;
extern int g_game7bfe74;
extern int g_game7beb04;
extern int g_game7bfe80;
extern int g_game7bf0ec;
extern int g_game78cf04;
extern int g_game939bf4;
extern int g_game78e5e0;
extern int g_game77cf1c;
extern int g_game77edc0;
extern int g_game92f414;
extern int g_game8bcf6c;
extern int g_game7beff0;
extern int g_game93a794;
extern int g_game939410;
extern int g_game8c759c;
extern int g_game93022c;
extern int g_game77e590;
extern int g_game94e964;
extern int g_game93a798;
extern int g_game94e968;
extern int g_game74c960;
extern int g_game94d640;
extern int g_game8e266c;
extern int g_game92e820;
extern int g_game8e2688;
extern int g_game8e2674;
extern int g_game939304;
extern int g_game7a82dc;






/* --- card scripts --------------------------------------------------------- */

/* Generated by cardgen; see ghidra-work/scripts. */
extern int g_game8c7d54;
extern int g_game8c8b44;
extern int g_game8e3104;
int Game_Callback477b57(void);

/* Defined in stubs.c for the helper layer; the card scripts reached them
 * too, and helpers.h is not on their include path. */
extern int g_game74b868;
extern int g_game939ce8;

/* Generated by cardgen; see ghidra-work/scripts. */
extern int g_game77d160[32][8];   /* extent unknown */
extern int g_game7a01dc;
extern int g_game7bfe78;
extern int g_game8c824c;
extern int g_game8e3ed4;
extern int g_game94d650[32][8];   /* extent unknown */
extern int g_game97c85c;
int Game_Callback4d3f22(void);
int Game_Callback4f9d1d(void);
int Game_Helper40f77c(int a1, int a2, int a3, int a4, int a5);
int Game_Helper474721(int a1, int a2, int a3, int a4);
int Game_Helper4ba5a1(int a1, int a2, int a3, int a4, int a5);
int Game_Helper5589ed(int a1, int a2, int a3, int a4, int a5);
int Game_Helper56ae5d(int a1, int a2);

/* Generated by cardgen; see ghidra-work/scripts. */
extern int g_game8c72b4;
extern int g_game8e3edc;

/* Generated by cardgen; see ghidra-work/scripts. */
extern int g_game7bf0e4;
extern int g_game7bf4c4;
extern int g_game7bfe7c;
extern int g_game8aa730;
extern int g_game8bcff0;
extern int g_game8e330c;
extern int g_game8e3ed8[32];   /* extent unknown */
extern int g_game939e64;
extern int g_game94d46c;
int Game_Callback44ffd0(void);
int Game_Callback4b03cd(void);
int Game_Helper40c4e1(int a1, int a2);
int Game_Helper40e3dc(int a1, int a2, int a3, int a4, int a5);
int Game_Helper41a252(int a1, int a2, int a3);
int Game_Helper41a64f(int a1, int a2);
int Game_Helper4412ec(int a1, int a2, int a3);
int Game_Helper44818c(int a1, int a2);
int Game_Helper476297(int a1, int a2, int a3);
int Game_Helper487ce0(int a1, int a2, int a3, int a4);
int Game_Helper49f31f(int a1, int a2);
int Game_Helper4b8102(int a1, int a2, int a3, int a4);
int Game_Helper4c0efa(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12, int a13, int a14, int a15, int a16, int a17, int a18, int a19, int a20);
int Game_Helper4c4181(void);
int Game_Helper4c42f8(void);
int Game_Helper4d34d6(int a1, int a2, int a3, int a4);
int Game_Helper4e4fa0(int a1, int a2, int a3, int a4);
int Game_Helper4e5571(int a1, int a2, int a3, int a4, int a5);
int Game_Helper4e5bfd(int a1, int a2, int a3, int a4);
int Game_Helper4ea3e7(int a1, int a2, int a3, int a4);
int Game_Helper4ec5d7(int a1, int a2);
int Game_Helper5179e0(int a1, int a2, int a3, int a4);
int Game_Helper518a81(int a1, int a2, int a3, int a4);
int Game_Helper51be77(int a1, int a2, int a3, int a4);
int Game_Helper51ef70(int a1, int a2, int a3, int a4);
int Game_Helper521cd0(int a1, int a2, int a3, int a4);
int Game_Helper522535(int a1, int a2);
int Game_Helper555944(int a1, int a2, int a3, int a4);
int Game_Helper57e820(int a1, int a2);
int Game_Helper57e826(int a1, int a2);

/* Generated by cardgen; see ghidra-work/scripts. */
extern int g_game8bd394;
extern int g_game8c7e90;
extern int g_game8e291c;
extern int g_game93a864;
int Game_Callback4aea1d(void);
int Game_Helper44a96c(int a1, int a2);
int Game_Helper4909b0(int a1, int a2, int a3, int a4, int a5);
int Game_Helper49f710(int a1, int a2, int a3);
int Game_Helper4b183c(int a1, int a2);
int Game_Helper4ba41b(int a1, int a2);

/* Generated by cardgen; see ghidra-work/scripts. */
extern int g_game77d180[32][8];   /* extent unknown */
extern int g_game78e970[32];   /* extent unknown */
extern int g_game8a9410[32][500];   /* extent unknown */
extern int g_game8e3300[32][8];   /* extent unknown */
extern int g_game8e3320[32][8];   /* extent unknown */
extern int g_game94d660[32][8];   /* extent unknown */
extern int g_game94d930[32][500];   /* extent unknown */
int Game_Helper40dffd(int a1);
int Game_Helper417301(int a1, int a2, int a3, int a4);
int Game_Helper4419ca(int a1, int a2, int a3, int a4);
int Game_Helper44aa1f(int a1, int a2, int a3);
int Game_Helper490c6d(int a1, int a2);
int Game_Helper49fda3(int a1, int a2, int a3);
int Game_Helper4a011a(int a1, int a2, int a3);
int Game_Helper4e25e6(int a1, int a2, int a3);
int Game_Helper522508(int a1);
int Game_Helper56c1b5(int a1, int a2);
int Game_Helper56c4f7(int a1, int a2);

/* Generated by cardgen; see ghidra-work/scripts. */
extern int g_game7be9ac;
extern int g_game8aa734;
extern int g_game8bce34;
extern int g_game8bcff8;
extern int g_game8c6aa4;
extern int g_game8c8274;
extern int g_game8c83f0[32];   /* extent unknown */
extern int g_game8c83f9;
extern int g_game8e3efc;
extern int g_game94d45c;
extern int g_game94d474;
extern int g_game97db34;
int Game_Callback48592f(void);
int Game_Helper40cd94(int a1, int a2);
int Game_Helper40e158(int a1, int a2, int a3);
int Game_Helper41040d(void);
int Game_Helper42d790(int a1, int a2, int a3, int a4);
int Game_Helper440d44(int a1, int a2, int a3, int a4, int a5);
int Game_Helper485308(int a1);
int Game_Helper4882b5(int a1);
int Game_Helper488474(int a1, int a2);
int Game_Helper4885e4(int a1, int a2);
int Game_Helper488cc4(int a1, int a2);
int Game_Helper49bf0e(int a1, int a2);
int Game_Helper49f483(int a1, int a2);
int Game_Helper49f87b(int a1, int a2, int a3);
int Game_Helper49fb64(int a1);
int Game_Helper4bea85(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12, int a13, int a14, int a15, int a16, int a17, int a18, int a19, int a20);
int Game_Helper4d1c03(int a1, int a2);
int Game_Helper4dbdfc(int a1, int a2, int a3, int a4);
int Game_Helper5218f3(int a1, int a2);
int Game_Helper56c705(int a1);
int Game_Helper56cc4d(int a1, int a2);

/* Generated by cardgen; see ghidra-work/scripts. */
extern int g_game765420[32];   /* extent unknown */
extern int g_game77c57c;
extern int g_game7be608;
extern int g_game8c7420;
int Game_Callback4975e2(void);
int Game_Helper44147b(int a1, int a2, int a3);
int Game_Helper4a0403(int a1);
int Game_Helper4be650(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12, int a13, int a14, int a15, int a16, int a17, int a18, int a19);
int Game_Helper558aea(int a1, int a2);

/* Generated by cardgen; see ghidra-work/scripts. */
extern int g_game7bf4b8;
extern int g_game8e2fc4;
extern int g_game8e3ec0[32];   /* extent unknown */
extern int g_game8e3ec8[32];   /* extent unknown */
extern int g_game8e3ed0[32];   /* extent unknown */
extern int g_game8e3f00;
int Game_Callback4323cc(void);
int Game_Helper40ebb9(int a1);
int Game_Helper441539(int a1, int a2, int a3);
int Game_Helper4415c5(int a1, int a2, int a3);
int Game_Helper441651(int a1, int a2, int a3);
int Game_Helper46e4d0(int a1, int a2);
int Game_Helper48808b(int a1, int a2);
int Game_Helper488147(int a1, int a2, int a3);

/* Generated by cardgen; see ghidra-work/scripts. */
extern int g_game930f5c;
extern int g_game94f280;
extern int g_game951968;
int Game_Helper558538(int a1, int a2, int a3, int a4, int a5, int a6, int a7);

/* Generated by cardgen; see ghidra-work/scripts. */
extern int g_game8c83ec;
extern int g_game8c83fa;
extern int g_game8c8afc;
extern int g_game92f3cc;
extern int g_game94d63c;
extern int g_game97db30;
int Game_Callback480238(void);
int Game_Callback4803cc(void);
int Game_Callback48044a(void);
int Game_Callback4ae506(void);
int Game_Helper48c204(int a1, int a2);
int Game_Helper498e8a(int a1, int a2, int a3);
int Game_Helper49914d(int a1, int a2, int a3, int a4);
int Game_Helper49f577(int a1, int a2);
int Game_Helper49f7b5(int a1, int a2, int a3);
int Game_Helper49f838(int a1, int a2);
int Game_Helper4ba2ed(int a1);
int Game_Helper55912c(void);

/* NO PROTOTYPES FOR CARD SCRIPTS LIVE HERE, DELIBERATELY. Nothing calls them by
 * name -- they are reached only through the +0x34 pointer -- and reccmp needs
 * the definition and its marker, not a declaration. So a prototype here is pure
 * cost, and the cost is large: 28 of them, one per reconstructed script, dragged
 * this header's consumers down by 8 byte-exact functions and 630 matched bytes.
 * Deleting all 28 recovered every one of them (0x0048b2c1 came back from 33.64%
 * to 100%) with no change to any function body.
 *
 * Add the definition to a cards*.c and nothing else. If a script ever does need
 * to be named elsewhere, declare it in that one file, not here. */

/* The slot a script was invoked on, and the source slot it came from.
 *
 * Macros rather than a `GameSlot *` local because /Od re-evaluates the whole
 * index expression at every use -- three multiplies and two global loads, every
 * time -- and holding a pointer in a local emits something quite different. The
 * macro reproduces the original's shape; a pointer would be the tidier C and
 * the wrong machine code. */
#define CUR  g_gameSlots[g_game8e1aa8][g_game951bdc]
#define SRC  g_gameSlots[(signed char)CUR.field0c][CUR.field48]

/* The row and column that address the slot a card script operates on. Both are
 * globals rather than arguments: a script reads g_gameSlots[row][col] to find
 * out what it was invoked on. */
extern int g_game8e1aa8;   /* row    */
extern int g_game951bdc;   /* column */

extern int g_game7a01e0;
extern int g_game7a82e0;
extern int g_game8e26b0;
extern int g_game94e928;
extern int g_game7a0230;
extern int g_game93a934;
extern int g_game7beb10[8];
extern int g_game7beb18[8];

/* Written immediately before a callback is handed to Game_Helper49f3c0, so
 * they are how the caller passes it its subject -- two pairs, one per script. */
extern int g_game5aa620;
extern int g_game5aa61c;
extern int g_game5aa4e8;
extern int g_game5aa4ec;

/* Each of these sits immediately after the script that passes it -- 0x004b3b46
 * begins one byte past 0x004b3a96's `ret`. Card script and its callback are
 * adjacent, which is what one source file per card produces. */
int Game_Callback4b3b46();
int Game_Callback4b39b5();
extern int g_game7a00a4;

/* Two of the arrays Game_ResetState clears are read by card scripts, which is
 * independent confirmation of the [2][8] shape derived there. */
extern int g_game77ed80[2][8];

/* --- what the scripts call ------------------------------------------------
 *
 * Six helpers, chosen by how much they unblock rather than by what they do:
 * stubbing exactly these six makes 87 more card scripts buildable, 44,474
 * bytes of them. Signatures come from the callees themselves -- the highest
 * [ebp+N] each touches gives the argument count, and all six end in a bare
 * `ret`, so all six are __cdecl. Every one leaves a value in eax.
 *
 * Nothing here establishes what any of them do, and the names say only where
 * they live. They are stubs so that call sites resolve; reconstructing them is
 * separate work. */
int Game_Helper411d90(int a, int b);
int Game_Helper46d8c1(int a, int b, int c);
int Game_Helper4a9910(int a, int b, int c, int d, int e);
int Game_Helper4ba309(int a, int b, int c);
int Game_Helper4ba392(int a, int b, int c);
int Game_Helper4bab38(int a, int b, int c, int d);

/* Stubs 7-26 of the same curve, argument counts taken from the CALL SITES
 * rather than the callees: a function that never touches its last argument
 * hides it from the [ebp+N] scan, but `add esp, N` after a __cdecl call cannot
 * lie. Every one of these is unanimous across all its sites -- 273 of 273 for
 * 0x00441699, 531 of 531 for 0x0056ca10 -- and every site cleans up, so all
 * twenty are __cdecl. 0x00403eeb takes none, which is why nothing cleans up
 * after its sixteen calls. */
int Game_Helper40cadc(int a1, int a2);
int Game_Helper40d7b2(int a1, int a2, int a3, int a4);
int Game_Helper403eeb(void);
int Game_Helper409a0b(int a1);
int Game_Helper409ee8(int a1, int a2, int a3);
int Game_Helper4111b1(int a1, int a2);
int Game_Helper4143e0(int a1, int a2, int a3);
int Game_Helper441699(int a1, int a2, int a3);
int Game_Helper4883aa(int a1, int a2, int a3, int a4);
/* The third argument is always the address of a function; kept as int with a
 * cast at each call site, because typing it as a pointer perturbed codegen
 * across every card TU (measured: -487 matched bytes). */
int Game_Helper488b70(int a1, int a2, int a3, int a4);

/* Callbacks handed to it. Real functions, all __cdecl with a bare ret; the
 * argument counts above are lower bounds from their [ebp+N] use. */
int Game_Callback440b07();
int Game_Callback480f45();
int Game_Callback4b484b();
int Game_Helper498365(int a1, int a2, int a3);
int Game_Helper49c6b1(int a1, int a2, int a3);
int Game_Helper49f3c0(int a1, int a2);
int Game_Helper49f61a(int a1, int a2, int a3);
int Game_Helper4ba25e(int a1);
int Game_Helper4c42ae(int a1);
int Game_Helper4c4263(int a1);
int Game_Helper4d40a4(int a1, int a2);
int Game_Helper4ecec6(int a1, int a2, int a3);
int Game_Helper56ca10(int a1, int a2);

/* A third table, at 0x008e44b8, indexed by a card's mask. Records are 152
 * bytes: the index is multiplied by 19 through the usual lea pair and then
 * scaled by 8 in the addressing mode. Only the dword at +0x00 is read here,
 * and only to compare it against 0x70. Nothing establishes what it holds; the
 * row count is a placeholder large enough to index. */
extern int g_game8e44b8[512][38];


/* FUNCTION: SHANDALAR 0x00409680 */
void Game_ResetState(void);

/* FUNCTION: SHANDALAR 0x0046c6cf */
int Game_Predicate46c6cf(void);

/* FUNCTION: SHANDALAR 0x0040f77c */
int Game_LogEvent(int row, int col, int kind, int arg, int extra);

/* FUNCTION: SHANDALAR 0x004ba5a1 */
int Game_AllocSlot(int a, int b, int c, int owner, int source);

#endif /* GAME_SLOT_H */
