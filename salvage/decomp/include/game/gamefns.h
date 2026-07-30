/* What the reconstructed ordinary game functions reference.
 *
 * Deliberately NOT in gameslot.h, which every cards*.c includes. See the note
 * in helpers.h: declarations added there re-roll codegen for everything that
 * already scores.
 */
#ifndef GAME_FNS_H
#define GAME_FNS_H

#include "game/gameslot.h"

/* Established names this layer reaches that live outside gameslot.h. */
extern int deck[4096];


extern int g_game580d80;
extern int g_game580dac;
extern int g_game581804;
extern int g_game581ea8;
extern int g_game5832b4;
extern int g_game5832dc;
extern int g_game583304;
extern int g_game583a74;
extern int g_game5862d8[32];   /* extent unknown */
extern int g_game586340[32];   /* extent unknown */
extern int g_game5863b0;
extern int g_game5863b8;
extern int g_game5863bc;
extern int g_game5863c0[32][4];   /* extent unknown */
extern int g_game586494;
extern int g_game587590[32][4];   /* extent unknown */
extern int g_game5875a0[32][4];   /* extent unknown */
extern int g_game589df0;
extern int g_game58b584;
extern int g_game58b954;
extern int g_game58b958;
extern int g_game58c03f;
extern int g_game5911f8;
extern int g_game5911fc;
extern int g_game5a1814;
extern int g_game5a6074;
extern int g_game5a6078;
extern int g_game5a6080[32][7];   /* extent unknown */
extern int g_game5a7520;
extern int g_game5a7c08[32][16];   /* extent unknown */
extern int g_game5a8b30;
extern int g_game5aa470;
extern int g_game5aa690;
extern int g_game5aae78;
extern int g_game5ace50;
extern int g_game5acff0;
extern int g_game5ad038;
extern int g_game5ae0a0[32];   /* extent unknown */
extern int g_game5ae4a0[32];   /* extent unknown */
extern int g_game5ae8a0;
extern int g_game5b0e68;
extern int g_game5b0e90;
extern int g_game5b1000;
extern int g_game5b1180;
extern int g_game5b1184;
extern int g_game5b1190;
extern int g_game5b21f8;
extern int g_game5b2200[32];   /* extent unknown */
extern int g_game5b2dd0[32];   /* extent unknown */
extern int g_game5b31d0[32];   /* extent unknown */
extern int g_game5b35d4;
extern int g_game5b3638;
extern int g_game5b3e90;
extern int g_game5b3e94;
extern int g_game5b3e98;
extern int g_game5b3e9c;
extern int g_game5b4140[32];   /* extent unknown */
extern int g_game5b4540[32];   /* extent unknown */
extern int g_game5b4940;
extern int g_game5b4948[32];   /* extent unknown */
extern int g_game5b4d48;
extern int g_game5b4d4c;
extern int g_game5b4d50;
extern int g_game5b5d00;
extern int g_game5b5e10;
extern int g_game5b6618;
extern int g_game5b66a8;
extern int g_game5b7650;
extern int g_game5b7b6c;
extern int g_game5b80e0[32];   /* extent unknown */
extern int g_game5b8138;
extern int g_game602fb4;
extern int g_game604420;
extern int g_game6506f0;
extern int g_game6506f4;
extern int g_game650700[32];   /* extent unknown */
extern int g_game650f40[32];   /* extent unknown */
extern int g_game651740[32];   /* extent unknown */
extern int g_game651f40[32];   /* extent unknown */
extern int g_game6527c0[32];   /* extent unknown */
extern int g_game669390;
extern int g_game6696f8;
extern int g_game73226c;
extern int g_game73bfa0[32];   /* extent unknown */
extern int g_game73e9d0;
extern int g_game73e9d4;
extern int g_game746ef0[32];   /* extent unknown */
extern int g_game746f40[32][50];   /* extent unknown */
extern int g_game747ee0;
extern int g_game748408;
extern int g_game74840c;
extern int g_game748428;
extern int g_game74842c;
extern int g_game74cfe4;
extern int g_game765e24;
extern int g_game77c020[32];   /* extent unknown */
extern int g_game77e58c;
extern int g_game780820[32];   /* extent unknown */
extern int g_game7817f0[32][4];   /* extent unknown */
extern int g_game7894f0;
extern int g_game7898f0;
extern int g_game7898f4;
extern int g_game7898f8;
extern int g_game789904;
extern int g_game789930;
extern int g_game789938;
extern int g_game78cee4;
extern int g_game78cefc;
extern int g_game78df68;
extern int g_game79ff88;
extern int g_game7a00a0;
extern int g_game7a090c;
extern int g_game7a09c4;
extern int g_game7bf4c8;
extern int g_game8bd344;
extern int g_game8bd35c;
extern int g_game8bd370;
extern int g_game8bd374;
extern int g_game8c8b50[32];   /* extent unknown */
extern int g_game8c8b51[32];   /* extent unknown */
extern int g_game8c8b52[32];   /* extent unknown */
extern int g_game8e0b00[32][500];   /* extent unknown */
extern int g_game8e1aa4;
extern int g_game939ce0;
extern int g_game939ce4;
extern int g_game93a780;
extern int g_game93a784;
extern int g_game93a788;
extern int g_game93a78c;
extern int g_game93a790;
extern int g_game93a938;
extern int g_game94d2ac[32];   /* extent unknown */
extern int g_game94d634;
extern int g_game94eab0[32];   /* extent unknown */
extern int g_game9523f0[32];   /* extent unknown */
extern int g_game97f1c1[32];   /* extent unknown */
extern int g_game982540;
extern int g_game986d94;
extern int g_game986d98;
extern int g_game986d9c;
int Game_Callback4c8bec(void);
int Game_Helper40a62b(int a1, int a2, int a3);
int Game_Helper40b26d(int a1, int a2);
int Game_Helper412c37(int a1, int a2);
int Game_Helper417dc6(int a1);
int Game_Helper4261ae(int a1);
int Game_Helper42811a(int a1, int a2, int a3, int a4);
int Game_Helper428615(int a1);
int Game_Helper42fe00(int a1);
int Game_Helper430ef4(int a1, int a2, int a3, int a4);
int Game_Helper43104f(int a1, int a2, int a3, int a4);
int Game_Helper4310e8(int a1);
int Game_Helper442f6a(void);
int Game_Helper4431d8(void);
int Game_Helper4432ff(void);
int Game_Helper450110(int a1, int a2);
int Game_Helper450925(int a1, int a2, int a3, int a4, int a5, int a6);
int Game_Helper452355(int a1, int a2);
int Game_Helper452ad9(int a1, int a2);
int Game_Helper453b6c(int a1, int a2);
int Game_Helper453f42(int a1, int a2);
int Game_Helper45cba4(int a1);
int Game_Helper46297d(void);
int Game_Helper46488b(int a1, int a2);
int Game_Helper464c8a(int a1, int a2, int a3);
int Game_Helper46537b(int a1);
int Game_Helper465764(void);
int Game_Helper4660d6(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8);
int Game_Helper468365(int a1, int a2);
int Game_Helper468498(int a1);
int Game_Helper46933c(int a1);
int Game_Helper46a674(int a1, int a2, int a3, int a4);
int Game_Helper46ed33(void);
int Game_Helper46ef07(int a1);
int Game_Helper4a8da8(int a1);
int Game_Helper4bac17(int a1);
int Game_Helper4c19a2(int a1);
int Game_Helper4c6c10(int a1);
int Game_Helper4c8f49(int a1);
int Game_Helper4cc0c1(int a1);
int Game_Helper4ccd9e(int a1);
int Game_Helper4cea4c(int a1, int a2);
int Game_Helper4ecfa2(void);
int Game_Helper4ed005(void);
int Game_Helper4f8400(int a1);
int Game_Helper501b7d(void);
int Game_Helper5020fe(int a1);
int Game_Helper5044ac(int a1);
int Game_Helper504be8(int a1);
int Game_Helper50d809(int a1);
int Game_Helper53a054(int a1, int a2, int a3, int a4, int a5, int a6, int a7);
int Game_Helper550dba(int a1);
int Game_Helper556385(int a1);
int Game_Helper5579fc(int a1, int a2);
int Game_Helper559dcd(void);
int Game_Helper559ff4(int a1);
int Game_Helper55a13b(int a1);
int Game_Helper562f92(int a1, int a2, int a3);
int Game_Helper564e70(int a1, int a2, int a3, int a4, int a5);
int Game_Helper56a515(int a1, int a2, int a3, int a4, int a5, int a6);
int Game_Helper56d133(int a1);
int Game_Helper56d167(void);
int Game_Helper56d194(int a1, int a2);
int Game_Helper56d4b0(int a1, int a2);
int Game_Helper56dc9c(int a1);
int Game_Helper56f8c1(void);
int Game_Helper57378d(int a1, int a2);
int Game_Helper57456b(int a1);
int Game_Helper578c04(void);
int Game_Helper578c40(void);
int Game_Helper578c60(void);
int Game_Helper579240(int a1);
int Game_Helper5795f0(int a1, int a2, int a3);
int Game_Helper5796c0(int a1, int a2, int a3, int a4, int a5, int a6);
int Game_Helper579760(int a1, int a2, int a3, int a4);
int Game_Helper5797e0(int a1, int a2, int a3, int a4, int a5, int a6);
int Game_Helper579890(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8);
int Game_Helper579bf0(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8);
int Game_Helper579e40(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10);
int Game_Helper57a060(int a1);
int Game_Helper57bfa0(int a1, int a2, int a3, int a4, int a5, int a6);
int Game_Helper57c580(int a1, int a2, int a3, int a4, int a5);
int Game_Helper57c7e0(int a1, int a2);
int Game_Helper57c820(int a1);
int Game_Helper57e832(int a1);
int Game_Helper57e83e(int a1, int a2, int a3);
int Game_Fn404677(void);
int Game_Fn40a561(int a1, int a2);
int Game_Fn40f0f6(void);
int Game_Fn40f132(void);
int Game_Fn4105b7(void);
int Game_Fn41116d(void);
int Game_Fn412bae(int a1, int a2, int a3);
int Game_Fn412bff(int a1, int a2, int a3);
int Game_Fn413796(int a1, int a2, int a3, int a4, int a5);
int Game_Fn413829(int a1, int a2, int a3, int a4, int a5, int a6);
int Game_Fn4170f8(void);
int Game_Fn41716b(int a1, int a2);
int Game_Fn4171b7(void);
int Game_Fn4171eb(int a1);
int Game_Fn418a2d(int a1);
int Game_Fn41d169(void);
int Game_Fn41d1ef(void);
int Game_Fn41d21d(void);
int Game_Fn424c13(void);
int Game_Fn426ddf(int a1);
int Game_Fn426efc(void);
int Game_Fn42804f(int a1);
int Game_Fn4288f1(int a1, int a2);
int Game_Fn4289b1(int a1, int a2, int a3, int a4);
int Game_Fn4290e2(int a1, int a2);
int Game_Fn42e1e2(void);
int Game_Fn42fde0(int a1);
int Game_Fn430994(void);
int Game_Fn430a96(int a1, int a2, int a3, int a4);
int Game_Fn430e00(int a1, int a2, int a3, int a4, int a5);
int Game_Fn430f69(int a1, int a2, int a3, int a4);
int Game_Fn430fb5(int a1, int a2, int a3, int a4);
int Game_Fn431002(int a1, int a2, int a3, int a4);
int Game_Fn4310a4(int a1, int a2, int a3, int a4);
int Game_Fn4311d2(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8);
int Game_Fn43125f(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8);
int Game_Fn4312ea(int a1, int a2, int a3, int a4, int a5, int a6);
int Game_Fn431351(int a1, int a2, int a3, int a4, int a5, int a6);
int Game_Fn4313cf(int a1, int a2, int a3, int a4, int a5, int a6);
int Game_Fn431436(int a1, int a2, int a3, int a4, int a5);
int Game_Fn43146b(int a1, int a2);
int Game_Fn4314ca(int a1, int a2);
int Game_Fn431526(int a1, int a2, int a3);
int Game_Fn431593(int a1, int a2, int a3);
int Game_Fn43174d(int a1, int a2, int a3);
int Game_Fn431859(int a1, int a2, int a3);
int Game_Fn433527(void);
int Game_Fn436ad2(void);
int Game_Fn441607(int a1, int a2, int a3);
int Game_Fn441b70(int a1);
int Game_Fn4432e0(void);
int Game_Fn446a57(void);
int Game_Fn4509ae(int a1, int a2);
int Game_Fn4509be(int a1);
int Game_Fn451b3e(int a1, int a2);
int Game_Fn451b4e(void);
int Game_Fn451f3b(int a1, int a2);
int Game_Fn452973(int a1, int a2);
int Game_Fn452e85(void);
int Game_Fn452efb(int a1, int a2);
int Game_Fn453740(int a1);
int Game_Fn454311(int a1, int a2);
int Game_Fn4615b1(int a1, int a2, int a3, int a4);
int Game_Fn4629be(int a1);
int Game_Fn465dac(int a1);
int Game_Fn466049(int a1, int a2, int a3, int a4, int a5, int a6);
int Game_Fn466078(int a1, int a2, int a3, int a4, int a5, int a6);
int Game_Fn4660a7(int a1, int a2, int a3, int a4, int a5, int a6);
int Game_Fn466d01(int a1);
int Game_Fn467f35(void);
int Game_Fn4683e4(int a1);
int Game_Fn46e64b(int a1, int a2);
int Game_Fn46ec92(void);
int Game_Fn46eca4(void);
int Game_Fn46ed03(void);
int Game_Fn46ed9c(int a1, int a2);
int Game_Fn46edb3(void);
int Game_Fn4a101f(int a1, int a2);
int Game_Fn4a84ab(void);
int Game_Fn4bb458(int a1);
int Game_Fn4be4c4(int a1);
int Game_Fn4c16fb(int a1, int a2);
int Game_Fn4c2501(int a1);
int Game_Fn4c38a2(void);
int Game_Fn4c3b83(void);
int Game_Fn4c3e34(void);
int Game_Fn4c40e5(void);
int Game_Fn4c4364(void);
int Game_Fn4c4415(void);
int Game_Fn4c69f3(int a1);
int Game_Fn4c8381(int a1);
int Game_Fn4c8bc5(int a1);
int Game_Fn4ce955(void);
int Game_Fn4ce97d(void);
int Game_Fn4cea02(void);
int Game_Fn4ceacb(void);
int Game_Fn4ceb0a(int a1, int a2, int a3);
int Game_Fn4ceb15(int a1, int a2, int a3);
int Game_Fn4ceb20(int a1, int a2, int a3, int a4);
int Game_Fn4ceb2b(int a1, int a2, int a3, int a4);
int Game_Fn4ceb36(void);
int Game_Fn4ceb41(int a1, int a2, int a3, int a4);
int Game_Fn4ece40(int a1);
int Game_Fn4ecefd(int a1);
int Game_Fn4ecf30(int a1, int a2);
int Game_Fn4ed0bd(void);
int Game_Fn4ed19f(void);
int Game_Fn4ed44b(int a1);
int Game_Fn4ed525(int a1);
int Game_Fn4f21bb(int a1, int a2);
int Game_Fn4f23bc(int a1, int a2);
int Game_Fn4f716f(void);
int Game_Fn5000fb(int a1);
int Game_Fn500129(int a1);
int Game_Fn50014e(int a1, int a2, int a3);
int Game_Fn500321(void);
int Game_Fn50035e(void);
int Game_Fn500ee0(void);
int Game_Fn5019ad(int a1);
int Game_Fn501aec(int a1);
int Game_Fn501e32(int a1);
int Game_Fn50318e(void);
int Game_Fn5031a8(void);
int Game_Fn5081e0(int a1);
int Game_Fn508bf3(int a1, int a2, int a3, int a4, int a5, int a6);
int Game_Fn50a56d(void);
int Game_Fn5224a0(int a1);
int Game_Fn5224d9(int a1);
int Game_Fn522660(void);
int Game_Fn522daa(void);
int Game_Fn52fac0(int a1, int a2);
int Game_Fn5307c3(int a1);
int Game_Fn530b7e(void);
int Game_Fn53133f(void);
int Game_Fn53182b(void);
int Game_Fn5318e3(void);
int Game_Fn536039(int a1, int a2, int a3);
int Game_Fn5377df(int a1, int a2, int a3, int a4);
int Game_Fn540ee8(int a1, int a2);
int Game_Fn544e3e(int a1);
int Game_Fn548eb1(int a1, int a2, int a3, int a4, int a5, int a6);
int Game_Fn54a880(void);
int Game_Fn54a89f(int a1, int a2, int a3, int a4, int a5);
int Game_Fn5501dc(int a1);
int Game_Fn5501fe(int a1);
int Game_Fn557a3a(int a1, int a2);
int Game_Fn5582ed(int a1, int a2);
int Game_Fn55837e(int a1, int a2, int a3, int a4, int a5);
int Game_Fn5584d5(int a1, int a2, int a3, int a4);
int Game_Fn558aa3(int a1);
int Game_Fn5597ca(void);
int Game_Fn559e52(void);
int Game_Fn55d8fa(int a1, int a2, int a3);
int Game_Fn561441(int a1);
int Game_Fn561465(int a1);
int Game_Fn5616aa(int a1);
int Game_Fn5616cb(int a1);
int Game_Fn5626b0(int a1, int a2, int a3, int a4, int a5);
int Game_Fn562736(int a1, int a2, int a3, int a4);
int Game_Fn56279e(int a1, int a2, int a3);
int Game_Fn5627ee(int a1, int a2, int a3);
int Game_Fn562835(int a1, int a2);
int Game_Fn562d03(void);
int Game_Fn56615f(int a1);
int Game_Fn566238(void);
int Game_Fn56c779(int a1);
int Game_Fn56c804(int a1);
int Game_Fn56c827(void);
int Game_Fn56c877(void);
int Game_Fn56c943(int a1);
int Game_Fn56c94e(int a1);
int Game_Fn56c959(int a1);
int Game_Fn56c964(int a1);
int Game_Fn56c96f(void);
int Game_Fn56c97a(int a1);
int Game_Fn56c985(void);
int Game_Fn56c990(void);
int Game_Fn56c99b(int a1, int a2, int a3);
int Game_Fn56c9b0(void);
int Game_Fn56d74e(void);
int Game_Fn573745(int a1, int a2, int a3);
int Game_Fn573868(int a1);
int Game_Fn573c2a(int a1, int a2, int a3);
int Game_Fn578873(void);

extern int g_game58332c;
extern int g_game583354;
extern int g_game5863ac;
extern int g_game589de8;
extern int g_game590764;
extern int g_game590768;
extern int g_game59121c;
extern int g_game591220;
extern int g_game59126c;
extern int g_game591278[32];   /* extent unknown */
extern int g_game593c88[32];   /* extent unknown */
extern int g_game650f28;
extern int g_game6527b0;
extern int g_game73eaa0;
extern int g_game746af8;
extern int g_game746afc;
extern int g_game746b00;
extern int g_game7481d8;
extern int g_game749410;
extern int g_game781728;
extern int g_game78172c;
extern int g_game781730[32];   /* extent unknown */
extern int g_game781780[32];   /* extent unknown */
extern int g_game789900;
extern int g_game78e82c;
extern int g_game8aa72c;
extern int g_game8bcf74;
extern int g_game8c7e8c;
extern int g_game8e09c0;
extern int g_game8e2e10[32][16];   /* extent unknown */
extern int g_game8e448c;
extern int g_game930640;
extern int g_game930644;
extern int g_game951970;
int Game_Helper4a04a0(int a1, int a2);
int Game_Helper4a2cd1(int a1, int a2, int a3, int a4);
int Game_Helper4c357e(void);
int Game_Helper4f7fb9(int a1, int a2);
int Game_Helper50027e(int a1, int a2);
int Game_Helper5040da(int a1);
int Game_Helper50425c(int a1);
int Game_Helper53a265(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10);
int Game_Helper53bc1a(int a1, int a2, int a3, int a4, int a5, int a6);
int Game_Helper53c19a(int a1, int a2, int a3);
int Game_Helper544e8f(void);
int Game_Helper5472f1(void);
int Game_Helper5474e1(void);
int Game_Helper557893(int a1, int a2);
int Game_Helper559999(void);
int Game_Helper559e3d(void);
int Game_Helper56da03(int a1);
int Game_Helper577b8f(int a1, int a2);
int Game_Helper578c70(int a1, int a2, int a3);
int Game_Helper57b650(void);
int Game_Helper57b670(void);
int Game_Helper57c800(int a1, int a2);
int Game_Helper57c840(int a1, int a2);
int Game_Fn40d572(int a1, int a2);
int Game_Fn40eb87(int a1, int a2);
int Game_Fn412b80(void);
int Game_Fn42ffd0(void);
int Game_Fn430f2f(void);
int Game_Fn431148(void);
int Game_Fn431604(void);
int Game_Fn431663(void);
int Game_Fn4316d6(void);
int Game_Fn4318c7(void);
int Game_Fn441260(void);
int Game_Fn4412a2(void);
int Game_Fn450836(int a1);
int Game_Fn451ab2(void);
int Game_Fn46ed63(void);
int Game_Fn46edc5(void);
int Game_Fn4789d3(void);
int Game_Fn485297(void);
int Game_Fn487077(void);
int Game_Fn4a04bd(void);
int Game_Fn4a2dd0(void);
int Game_Fn4a8470(void);
int Game_Fn4c410a(void);
int Game_Fn4cdd1b(void);
int Game_Fn4e4f5c(void);
int Game_Fn4ed06d(void);
int Game_Fn4ed095(void);
int Game_Fn4ef07e(void);
int Game_Fn4f7f99(void);
int Game_Fn51dcf2(void);
int Game_Fn532809(void);
int Game_Fn544d9c(void);
int Game_Fn544ded(void);
int Game_Fn54cdbd(void);
int Game_Fn550593(void);
int Game_Fn5578d1(void);
int Game_Fn558457(void);
int Game_Fn5584ac(void);
int Game_Fn558874(void);
int Game_Fn5588c3(void);
int Game_Fn558908(void);
int Game_Fn559781(void);
int Game_Fn5629c3(int a1);
int Game_Fn566134(void);
int Game_Fn578319(void);

#endif /* GAME_FNS_H */
