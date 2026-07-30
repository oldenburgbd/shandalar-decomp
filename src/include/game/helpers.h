/* Declaration surface adopted from the archived attempt (salvage/decomp/
 * include/game/helpers.h) -- see the provenance note in gameslot.h: MSVC 4.2
 * codegen depends on the declaration set, and this is the measured-good one. */

/* What the reconstructed card-layer helpers reference.
 *
 * Deliberately NOT in gameslot.h. Every cards*.c includes that, and putting
 * these there re-rolled codegen for 324 already-scoring functions -- a loss
 * that cancelled everything the helper bodies gained. Nothing here is needed
 * outside src/game/helpers*.c.
 */
#ifndef GAME_HELPERS_H
#define GAME_HELPERS_H

#include "game/gameslot.h"

extern int g_game589a38;
extern int g_game5ae0a0[32];   /* extent unknown */
extern int g_game5b2200[32];   /* extent unknown */
extern int g_game5b2dd0[32];   /* extent unknown */
extern int g_game74842c;
extern int g_game74b868;
extern int g_game74cdb4;
extern int g_game7be9e8[32];   /* extent unknown */
extern int g_game7bea98;
extern int g_game7bea9c;
extern int g_game7bf4c8;
extern int g_game7bfe7a;
extern int g_game8a940c[32][500];   /* extent unknown */
extern int g_game8bd38c;
extern int g_game8c6ae8;
extern int g_game8c6aec;
extern int g_game8c8650[32];   /* extent unknown */
extern int g_game8e4060;
extern int g_game939ce8;
extern int g_game939d24;
extern int g_game93a77c[32];   /* extent unknown */
extern int g_game97f1c0[32];   /* extent unknown */
int Game_Callback48889d(void);
int Game_Helper40d0cb(int a1, int a2, int a3, int a4);
int Game_Helper40d146(int a1, int a2, int a3, int a4, int a5, int a6);
int Game_Helper40e1f5(int a1);
int Game_Helper40e646(void);
int Game_Helper40e6fc(void);
int Game_Helper4105e0(int a1, int a2, int a3, int a4);
int Game_Helper410f46(int a1, int a2, int a3, int a4);
int Game_Helper419dc4(int a1, int a2);
int Game_Helper450813(int a1, int a2, int a3, int a4);
int Game_Helper451c24(int a1);
int Game_Helper46e159(int a1, int a2);
int Game_Helper4884c1(int a1, int a2);
int Game_Helper488638(int a1, int a2);
int Game_Helper488789(int a1, int a2);
int Game_Helper4c5bf6(int a1, int a2);
int Game_Helper53c35e(int a1, int a2, int a3);
int Game_Helper53cb05(int a1, int a2, int a3, int a4, int a5);
int Game_Helper5583c5(int a1, int a2, int a3, int a4, int a5, int a6);
int Game_Helper55840e(int a1, int a2, int a3, int a4, int a5, int a6);
int Game_Helper56bfa4(int a1);
int Game_Helper40a4b8(int a1, int a2);
int Game_Helper40d414(int a1, int a2, int a3, int a4);
int Game_Helper410efc(int a1, int a2, int a3, int a4);
int Game_Helper419d01(int a1);
int Game_Helper419e85(int a1, int a2, int a3);
int Game_Helper44157b(int a1, int a2, int a3);
int Game_Helper46db2e(void);
int Game_Helper4a008c(int a1);
int Game_Helper4c442a(void);
int Game_Helper4d646a(int a1, int a2);
int Game_Helper512898(int a1, int a2);
int Game_Helper512a01(int a1, int a2, int a3);
int Game_Helper512b06(int a1, int a2);
int Game_Helper558321(int a1, int a2, int a3, int a4);
int Game_Helper558356(int a1);
int Game_Helper558828(int a1, int a2);
int Game_Helper558941(int a1, int a2, int a3);
int Game_Helper5589ed(int a1, int a2, int a3, int a4, int a5);
int Game_Helper56a3a7(int a1, int a2, int a3, int a4, int a5, int a6, int a7);
int Game_Helper56a45f(int a1, int a2, int a3, int a4, int a5, int a6, int a7);
int Game_Helper56bf4c(int a1);
int Game_Helper56c496(int a1, int a2);
int Game_Helper56c91f(int a1, int a2);
int Game_Helper56c931(int a1, int a2);

extern int g_game7bfee4;
extern int g_game8c83f4;
int Game_Callback46dbed(void);
int Game_Helper46dbed(int a1, int a2);
int Game_Helper485eb8(int a1, int a2);

extern int g_game8e2e10[32][16];   /* extent unknown */
int Game_Helper440b8a(int a1);
int Game_Helper485185(int a1, int a2, int a3);
int Game_Helper4869b0(void);

#endif /* GAME_HELPERS_H */
