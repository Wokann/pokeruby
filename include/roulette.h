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

extern const struct GridSelection gGridSelections[];
extern const struct RouletteSlot gRouletteSlots[];
extern const struct RouletteTable gRouletteTables[];

void PlayRoulette(void);

void sub_8117838(u8 free);
void CreateGridSprites(void);
void ShowHideGridIcons(u8 hideAll, u8 hideSquare);
void CreateGridBallSprites(void);
void ShowHideGridBalls(u8 hideAll, u8 hideBallId);
void ShowHideWinSlotCursor(u8 selectionId);
void CreateWheelIconSprites(void);
void CreateInterfaceSprites(void);
void SetCreditDigits(u16 num);
void SetMultiplierSprite(u8 selectionId);
void SetBallCounterNumLeft(u8 numBalls);
void SpriteCB_GridSquare(struct Sprite *sprite);
void CreateWheelCenterSprite(void);
void CreateWheelBallSprites(void);
void HideWheelBalls(void);
void SpriteCB_RollBall_Start(struct Sprite *sprite);

#endif
