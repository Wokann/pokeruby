#ifndef GUARD_ROULETTE_H
#define GUARD_ROULETTE_H

#include "task.h"
#include "palette_util.h"

struct Roulette /* ewram + 0x19000 */
{
    u8 unk0;
    u8 shroomishShadowTimer;
    u8 partySpeciesFlags;
    u8 useTaillow:5;
    u8 ballStuck:1;
    u8 ballUnstuck:1;
    u8 ballRolling:1;
    u8 tableId:2;
    u8 unused:5;
    u8 isSpecialRate:1;
    u32 hitFlags;
    u8 hitSquares[6];
    u8 pokeHits[4];
    u8 colorHits[3];
    u8 minBet;
    u8 curBallNum:4;
    u8 unk1:4;
    u8 betSelection[6];
    u8 wheelDelayTimer;
    u8 wheelSpeed;
    u8 wheelDelay;
    s16 wheelAngle;
    s16 gridX;
    s16 selectionRectDrawState;
    s16 updateGridHighlight;
    struct OamMatrix wheelRotation;
    u16 shroomishShadowAlpha;
    struct Sprite *ball;
    u8 spriteIds[0x40]; // Sprite IDs
    u8 curBallSpriteId;
    u8 ballState;
    u8 hitSlot;
    u8 stuckHitSlot;
    s16 ballTravelDist;
    s16 ballTravelDistFast;
    s16 ballTravelDistMed;
    s16 ballTravelDistSlow;
    float ballAngle;
    float ballAngleSpeed;
    float ballAngleAccel;
    float ballDistToCenter;
    float ballFallSpeed;
    float ballFallAccel;
    float varA0;
    u8 playTaskId;
    u8 spinTaskId;
    u8 filler_1[2];
    u16 taskWaitDelay;
    u16 taskWaitKey;
    TaskFunc nextTask;
    u8 filler_2[4];
    TaskFunc prevTask;
    struct RouletteFlashUtil flashUtil;
}; // size: 0x17C

struct RouletteTable
{
    u8 minBet;
    u8 randDistanceHigh;
    u8 randDistanceLow;
    u8 wheelSpeed;
    u8 wheelDelay;
    u8 v[3];
    u16 var08;
    u16 var0A;
    u16 var0C;
    u16 v13[1];
    u16 var10;
    u16 var12;
    u16 var14;
    u8 v1[2];
    u16 ballSpeed;
    u16 baseTravelDist;
    float var1C;
};

struct GridSelection
{
    u8 spriteIdOffset;
    u8 baseMultiplier:4;
    u8 column:4;
    u8 row;
    u8 x;
    u8 y;
    u8 var05;
    u8 tilemapOffset;
    u8 var07;
    u32 flag;
    u32 inSelectionFlags;
    u16 flashFlags;
    u16 var12;
};

struct RouletteSlot
{
    u8 id1;
    u8 id2;
    u8 gridSquare;
    u32 flag;
};

extern const struct GridSelection gUnknown_083F8C00[];
extern const struct RouletteSlot gUnknown_083F8D90[];
extern const struct RouletteTable gUnknown_083F8DF4[];

void PlayRoulette(void);

void sub_8117838(u8);
u8 sub_8117890(const struct SpriteTemplate *, u8, u16 *);
void sub_8117900(void);
void unref_sub_8117A74(void);
void sub_8117AA8(u8, u8);
void sub_8117BBC(void);
void sub_8117C60(u8, u8);
void sub_8117D68(u8);
void sub_8117DF4(void);
void sub_8117E98(struct Sprite *);
void sub_8117F2C(void);
void sub_81180F4(u16);
u8 sub_81181E8(u8);
void sub_811829C(u8);
void sub_81182F8(u8);
void sub_81184CC(struct Sprite *);
void sub_81184D8(void);
void sub_8118554(struct Sprite *);
void sub_811857C(void);
void sub_81185E8(void);
void sub_81191F4(struct Sprite *);

#endif
