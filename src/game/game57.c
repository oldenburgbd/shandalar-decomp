/* Ordinary Shandalar game code (gamewire-reconstructed): plain .text
 * functions, neither card scripts nor their helpers. Address-derived names;
 * reccmp is the correctness check. */
#include "game/gamefns.h"

// FUNCTION: SHANDALAR 0x00573745
int Game_Fn573745(int a1, int a2, int a3)
{
    int iv[4];   /* [ebp-16] */

    if ((a3 & 0x1) != 0
        && (a3 & 0x2) != 0) {
        Game_Helper57378d(iv, a2);
        return Game_Helper464c8a(a1, iv, g_game73226c);
    }
    return 0;
}

// FUNCTION: SHANDALAR 0x00573868
int Game_Fn573868(int a1)
{
    int iv;   /* [ebp-4] */

    if (a1 == 0x1d7) {
        iv = 0;
    } else {
        if (a1 == 0x244) {
            iv = 1;
        } else {
            if (a1 == 0x248) {
                iv = 2;
            } else {
                if (a1 == 0x296) {
                    iv = 3;
                } else {
                    if (a1 == 0x2fd) {
                        iv = 4;
                    } else {
                        if (a1 == 0x354) {
                            iv = 5;
                        } else {
                            if (a1 == 0x1e4) {
                                iv = 6;
                            } else {
                                if (a1 == 0x26) {
                                    iv = 6;
                                } else {
                                    if (a1 == 0xf6) {
                                        iv = 7;
                                    } else {
                                        if (a1 == 0x34) {
                                            iv = 8;
                                        } else {
                                            if (a1 == 0x120) {
                                                iv = 9;
                                            } else {
                                                if (a1 == 0x7b) {
                                                    iv = 0xa;
                                                } else {
                                                    if (a1 == 0x80) {
                                                        iv = 0xb;
                                                    } else {
                                                        if (a1 == 0x5e) {
                                                            iv = 0xc;
                                                        } else {
                                                            if (a1 == 0x353) {
                                                                iv = 0xc;
                                                            } else {
                                                                if (a1 == 0x92) {
                                                                    iv = 0xd;
                                                                } else {
                                                                    if (a1 == 0x2e0) {
                                                                        iv = 0xe;
                                                                    } else {
                                                                        if (a1 == 0xd7) {
                                                                            iv = 0xf;
                                                                        } else {
                                                                            if (a1 == 0xdc) {
                                                                                iv = 0x10;
                                                                            } else {
                                                                                if (a1 == 0x367) {
                                                                                    iv = 0x11;
                                                                                } else {
                                                                                    if (a1 == 0x21a) {
                                                                                        iv = 0x12;
                                                                                    } else {
                                                                                        if (a1 == 0x216) {
                                                                                            iv = 0x13;
                                                                                        } else {
                                                                                            if (a1 == 0xf8) {
                                                                                                iv = 6;
                                                                                            } else {
                                                                                                if (a1 == 0x195) {
                                                                                                    iv = 0x17;
                                                                                                } else {
                                                                                                    if (a1 == 0x1e2) {
                                                                                                        iv = 0xc;
                                                                                                    } else {
                                                                                                        if (a1 == 0xcf) {
                                                                                                            iv = 0x14;
                                                                                                        } else {
                                                                                                            if (a1 == 0x1b3) {
                                                                                                                iv = 0x10;
                                                                                                            } else {
                                                                                                                iv = -1;
                                                                                                            }
                                                                                                        }
                                                                                                    }
                                                                                                }
                                                                                            }
                                                                                        }
                                                                                    }
                                                                                }
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return iv;
}

// FUNCTION: SHANDALAR 0x00573c2a
int Game_Fn573c2a(int a1, int a2, int a3)
{
    return 0;
}

// FUNCTION: SHANDALAR 0x00578873
int Game_Fn578873(void)
{
    return 0;
}

// FUNCTION: SHANDALAR 0x00578319
int Game_Fn578319(int a1, int a2)
{
    return Game_Helper577b8f(a1, a2);
}
