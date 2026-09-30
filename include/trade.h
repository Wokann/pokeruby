
#ifndef GUARD_TRADE_H
#define GUARD_TRADE_H

struct TradeAnimState {
    /*0x0000*/ u8 filler_0000;
    /*0x0004*/ struct Window window;
    /*0x0034*/ u8 textWindowBaseTileNum;
    /*0x0038*/ struct Pokemon tempMon;
    /*0x009c*/ u8 playerFinishStatus;
    /*0x009d*/ u8 partnerFinishStatus;
    /*0x009e*/ u16 linkData[10];
    /*0x00b2*/ u8 linkTimeoutZero1;
    /*0x00b3*/ u8 linkTimeoutZero2;
    /*0x00b4*/ u16 linkTimeoutTimer;
    /*0x00b6*/ u16 unused_00B6;
    // Sprite indices
    /*0x00b8*/ u8 monSpriteIds[2];
    /*0x00ba*/ u8 connectionSpriteId1;
    /*0x00bb*/ u8 connectionSpriteId2;
    /*0x00bc*/ u8 cableEndSpriteId;
    /*0x00bd*/ u8 scheduleLinkTransfer;
    // Timer
    /*0x00c0*/ u32 timer;
    // Scene index
    /*0x00c4*/ u16 state;
    /*0x00c6*/ u8 filler_00c6[0x3c];
    /*0x0102*/ u8 releasePokeballSpriteId;
    /*0x0103*/ u8 bouncingPokeballSpriteId;
    /*0x0104*/ u16 texX;
    /*0x0106*/ u16 texY;
    /*0x0108*/ u16 unused_0108;
    /*0x010a*/ u16 unused_010A;
    /*0x010c*/ u16 scrX;
    /*0x010e*/ s16 scrY;
    /*0x0110*/ s16 bg1vofs;
    /*0x0112*/ s16 bg1hofs;
    /*0x0114*/ s16 bg2vofs;
    /*0x0116*/ s16 bg2hofs;
    /*0x0118*/ u16 sXY;
    /*0x011a*/ u16 gbaScale;
    /*0x011c*/ u16 alpha;
    /*0x011e*/ u8 isLinkTrade;
    /*0x0120*/ u16 monSpecies[2];
    /*0x0124*/ u16 cachedMapMusic;
};

extern struct TradeAnimState *gTradeAnim;
extern const u16 gTradeGba2_Pal[][16];
extern const u32 gTradeGba_Gfx[];
extern const u16 gTradePlatform_Tilemap[];

void CB2_StartCreateTradeMenu(void);
void CB2_InitInGameTrade(void);
void DoInGameTradeScene(void);
extern const u8 gText_MaleSymbol4[2];
extern const u8 gText_FemaleSymbol4[2];
extern const u8 gText_GenderlessSymbol[2];

void LoadTradeAnimGfx(void);

#endif //POKERUBY_TRADE_H
