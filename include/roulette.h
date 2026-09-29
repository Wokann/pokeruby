#ifndef GUARD_ROULETTE_H
#define GUARD_ROULETTE_H

#include "task.h"

#define FLASHUTIL_USE_EXISTING_COLOR (1 << 15)

struct RouletteFlashSettings
{
	u16 color;
	u16 paletteOffset;
	u8 numColors;
	u8 delay;
	s8 unk6;
	s8 numFadeCycles:5;
	s8 unk7_5:2;
	s8 colorDeltaDir:1;
}; // size: 8

struct RouletteFlashPalette
{
	u8 state:7;
	u8 available:1;
	u8 delayCounter;
	s8 fadeCycleCounter;
	s8 colorDelta;
    struct RouletteFlashSettings settings;
}; // size: 12

struct RouletteFlashUtil
{
	u8 enabled;
	u8 unused;
	u16 flags;
	struct RouletteFlashPalette palettes[16];
}; // size: 196

struct Roulette /* ewram + 0x19000 */
{
    u8 unk0;
    u8 shroomishShadowTimer;
    u8 partySpeciesFlags;
    bool8 useTaillow:5;
    bool8 ballStuck:1;
    bool8 ballUnstuck:1;
    bool8 ballRolling:1;
    u8 tableId:2;
    u8 unused:5;
    bool8 isSpecialRate:1;
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
    u8 spriteIds[0x40];
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

struct Shroomish
{
    u16 startAngle;
    u16 dropAngle;
    u16 fallSlowdown;
};

struct Taillow
{
    u16 baseDropDelay;
    u16 rightStartAngle;
    u16 leftStartAngle;
};

struct RouletteTable
{
    u8 minBet;
    u8 randDistanceHigh;
    u8 randDistanceLow;
    u8 wheelSpeed;
    u8 wheelDelay;
    struct Shroomish shroomish;
    struct Taillow taillow;
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
    u32 flag;
    u32 inSelectionFlags;
    u16 flashFlags;
};

struct RouletteSlot
{
    u8 id1;
    u8 id2;
    u8 gridSquare;
    u32 flag;
};

extern const struct GridSelection sGridSelections[];
extern const struct RouletteSlot sRouletteSlots[];
extern const struct RouletteTable sRouletteTables[];

void PlayRoulette(void);

void LoadOrFreeMiscSpritePalettesAndSheets(u8);
u8 CreateWheelIconSprite(const struct SpriteTemplate *, u8, u16 *);
void CreateGridSprites(void);
void DestroyGridSprites(void);
void ShowHideGridIcons(u8, u8);
void CreateGridBallSprites(void);
void ShowHideGridBalls(u8, u8);
void ShowHideWinSlotCursor(u8);
void CreateWheelIconSprites(void);
void SpriteCB_WheelIcon(struct Sprite *);
void CreateInterfaceSprites(void);
void SetCreditDigits(u16);
u8 GetMultiplierAnimId(u8);
void SetMultiplierSprite(u8);
void SetBallCounterNumLeft(u8);
void SpriteCB_GridSquare(struct Sprite *);
void CreateWheelCenterSprite(void);
void SpriteCB_WheelCenter(struct Sprite *);
void CreateWheelBallSprites(void);
void HideWheelBalls(void);
void SpriteCB_RollBall_Start(struct Sprite *);

#endif
