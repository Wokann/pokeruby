#ifndef GUARD_ROULETTE_H
#define GUARD_ROULETTE_H

#include "task.h"

struct UnkStruct1
{
	u16 var00;
	u16 var02;
	u8 var04;
	u8 var05;
	u8 var06;
	s8 var07_0:5;
	s8 var07_5:2;
	s8 var07_7:1;
}; // size: 8

struct UnkStruct3
{
	u8 var00_0:7;
	u8 var00_7:1;
	u8 var01;
	s8 var02;
	s8 var03;
    struct UnkStruct1 var04;
}; // size: 12

struct UnkStruct0
{
	u8 var00;
	u8 var01;
	u16 var02; //flag for each UnkStruct3
	struct UnkStruct3 var04[16];
}; // size: 196

struct Roulette /* ewram + 0x19000 */
{
    u8 var00;
    u8 var01;
    u8 var02;
    u8 var03_0:5;
    u8 var03_5:1;
    u8 var03_6:1;
    u8 var03_7:1;
    u8 var04_0:2;
    u8 var04_2:5;
    u8 var04_7:1;
    u32 var08;
    u8 var0C[6];
    u8 var12[4];
    u8 var16[3];
    u8 var19;
    u8 var1A_0:4;
    u8 var1A_4:4;
    u8 var1B[6];
    u8 var21;
    u8 var22;
    u8 var23;
    s16 var24;
    s16 var26;
    s16 var28;
    s16 var2A;
    struct OamMatrix var2C;
    u16 var34;
    struct Sprite *var38;
    u8 var3C[0x40]; // Sprite IDs
    u8 var7C;
    u8 var7D;
    u8 var7E;
    u8 var7F;
    s16 var80;
    s16 var82;
    s16 var84;
    s16 var86;
    float var88;
    float var8C;
    float var90;
    float var94;
    float var98;
    float var9C;
    float varA0;
    u8 varA4;
    u8 varA5;
    u8 v51[2];
    u16 varA8;
    u16 varAA;
    TaskFunc varAC;
    u8 v46[4];
    TaskFunc varB4;
    struct UnkStruct0 varB8;
}; // size: 0x17C

struct RouletteTable
{
    u8 minBet;
    u8 randDistanceHigh;
    u8 randDistanceLow;
    u8 wheelSpeed;
    u8 wheelDelay;
    u8 v[3];
    u16 shroomishStartAngle;
    u16 shroomishDropAngle;
    u16 shroomishFallSlowdown;
    u16 v13[1];
    u16 taillowBaseDropDelay;
    u16 taillowRightStartAngle;
    u16 taillowLeftStartAngle;
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

extern const struct GridSelection sGridSelections[];
extern const struct RouletteSlot sRouletteSlots[];
extern const struct RouletteTable sRouletteTables[];

s16 sub_81174C4(s16, s16);
s16 sub_81174E0(s16);
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
