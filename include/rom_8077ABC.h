#ifndef GUARD_ROM_8077ABC_H
#define GUARD_ROM_8077ABC_H

#include "sprite.h"
#include "task.h"

struct BattleAnimBgData
{
    u8 *bgTiles;
    u8 *bgTilemap;
    u8 paletteId;
};

struct TransformStatus
{
    u8 unk0;
    u16 species;
};

u8 GetBattlerSpriteCoord(u8, u8);
u8 GetBattlerSpriteFinal_Y(u8 slot, u16 species, u8 a3);
u8 GetAnimBattlerSpriteId(u8 bank);
void StoreSpriteCallbackInData6(struct Sprite *sprite, void(*callback)(struct Sprite *));
void sub_8078314(struct Sprite *sprite);
void TranslateSpriteOverDuration(struct Sprite *sprite);
void TranslateMonBGUntil(struct Sprite *sprite);
void TranslateMonBGSubPixelUntil(struct Sprite *sprite);
u8 GetBattlerSide(u8);
u8 GetBattlerSide(u8);
u8 GetBattlerSide(u8 side);
u8 GetBattlerSide(u8 slot);
u8 GetBattlerSide(u8);
u8 GetBattlerPosition(u8);
u8 GetBattlerPosition(u8 slot);
u8 GetBattlerAtPosition(u8);
u8 GetBattlerAtPosition(u8);
u8 GetBattlerAtPosition(u8 state);
bool8 IsBankSpritePresent(u8);
bool8 IsDoubleBattle();
u8 IsDoubleBattle(void);
bool8 IsDoubleBattle(void);
void GetBattleAnimBg1Data(struct BattleAnimBgData *animBg);
u8 sub_80789BC();
void InitSpriteDataForLinearTranslation(struct Sprite *sprite);
void InitAnimLinearTranslation(struct Sprite *sprite);
bool8 AnimTranslateLinear(struct Sprite *sprite);
void AnimTranslateLinear_WithFollowup(struct Sprite *sprite);
void sub_8078D44(struct Sprite *sprite);
void SetSpriteRotScale(u8 spriteId, s16, s16, u16);
bool8 ShouldRotScaleSpeciesBeFlipped(void);
void PrepareBattlerSpriteForRotScale(u8 spriteId, u8);
void ResetSpriteRotScale(u8 spriteId);
void SetBattlerSpriteYOffsetFromRotation(u8 spriteId);
void sub_8079518(struct Sprite *sprite);
void sub_8079534(struct Sprite *sprite);
void AnimTask_AlphaFadeIn(u8 taskId);
void AnimTask_BlendMonInAndOutSetup(struct Task *task);
void AnimTask_BlendMonInAndOutStep(u8 taskId);
void SetBattlerSpriteYOffsetFromYScale(u8 spriteId);
u16 GetBattlerYDeltaFromSpriteId(u8 spriteId);
void StorePointerInVars(s16 *lo, s16 *hi, const void *ptr);
void *LoadPointerFromVars(s16 lo, s16 hi);
// u8 a2 := u8 sprite
void sub_8079C08(struct Task *task, u8 a2, s16 a3, s16 a4, s16 a5, s16 a6, u16 a7);
u8 sub_8079C74(struct Task *task);
void UpdateBattlerSpritePriorities();
u8 GetBattlerSpriteSubpriority(u8 bank);
u8 GetBattlerSpriteBGPriorityRank(u8 battler);
void sub_807A784(u8 taskId);
void sub_807A850(struct Task *task, u8 taskId);
void sub_807A8D4(struct Sprite *sprite);
void sub_807A960(struct Sprite *sprite);
void sub_8078A34(struct Sprite *sprite);
void InitSpritePosToAnimAttacker(struct Sprite *sprite, bool8 respectMonPicOffsets);
void sub_8078764(struct Sprite *sprite, bool8);
void StartAnimLinearTranslation(struct Sprite *sprite);
void sub_8078D60(struct Sprite *sprite);
void InitAnimArcTranslation(struct Sprite *sprite);
void sub_8078D8C(struct Sprite *sprite);
void WaitAnimForDuration(struct Sprite *sprite);
void sub_8078CC0(struct Sprite *sprite);
void RunStoredCallbackWhenAnimEnds(struct Sprite *sprite);
void TranslateSpriteLinearAndFlicker(struct Sprite *sprite);
void DestroyAnimSpriteAndDisableBlend(struct Sprite *sprite);
void SetSpriteCoordsToAnimAttackerCoords(struct Sprite *sprite);
void sub_8078394(struct Sprite *sprite);
void RunStoredCallbackWhenAffineAnimEnds(struct Sprite *sprite);
void sub_8078278(struct Sprite *sprite);
void InitAnimLinearTranslationWithSpeedAndPos(struct Sprite *sprite);
void sub_8078114(struct Sprite *sprite);
void sub_8078174(struct Sprite *sprite);
void AnimSpriteOnMonPos(struct Sprite *sprite);
void SetAverageBattlerPositions(u8 slot, u8 a2, s16 *a3, s16 *a4);
u8 GetBattlerSpriteBGPriority(u8 slot);
s16 GetBattlerSpriteCoordAttr(u8 slot, u8 a2);
u16 ArcTan2Neg(s16 a, s16 b);
void TrySetSpriteRotScale(struct Sprite *sprite, bool8 recalcCenterVector, s16 xScale, s16 yScale, u16 rotation);
void SetAnimSpriteInitialXOffset(struct Sprite *sprite, s16 xOffset);
u8 GetBattlerSpriteCoord2(u8 battler, u8 coordType);
u32 GetBattlePalettesMask(bool8 battleBackground, bool8 attacker, bool8 target, bool8 attackerPartner, bool8 targetPartner, bool8 anim1, bool8 anim2);
u32 GetBattleMonSpritePalettesMask(u8 playerLeft, u8 playerRight, u8 opponentLeft, u8 opponentRight);
s16 duplicate_obj_of_side_rel2move_in_transparent_mode(u8 a1);
void obj_delete_but_dont_free_vram(struct Sprite *sprite);
void SetGrayscaleOrOriginalPalette(u16 paletteNum, bool8 restoreOriginalColor);
void PrepareAffineAnimInTaskData(struct Task *task, u8 a2, const void *a3);
bool8 RunAffineAnimFromTaskData(struct Task *task);
u8 GetBattlerYCoordWithElevation(u8 battler);
void DestroySpriteAndMatrix(struct Sprite *sprite);
bool8 TranslateAnimArc(struct Sprite *sprite);
bool8 sub_8078CE8(struct Sprite *sprite);
void SetSpritePrimaryCoordsFromSecondaryCoords(struct Sprite *sprite);
void InitAnimLinearTranslationWithSpeed(struct Sprite *sprite);
void TranslateAnimSpriteToTargetMonLocation(struct Sprite *sprite);
void UpdateAnimBg3ScreenSize(bool8 largeScreenSize);
void SetBattlerSpriteYOffsetFromOtherYScale(u8 spriteId, u8 otherSpriteId);
u8 CreateInvisibleSpriteCopy(int battler, u8 spriteId, int species);
void sub_80794A8(struct Sprite *sprite);
void sub_807A9BC(struct Sprite *sprite);
void GetBgDataForTransform(struct BattleAnimBgData *animBg, u8 battler);
u8 sub_8079F44(u16 species, bool8 isBackpic, u8 a3, s16 a4, s16 a5, u8 a6, u32 a7, u32 a8);
void ResetSpriteRotScale_PreserveAffine(struct Sprite *sprite);
void DestroySpriteAndFreeResources_(struct Sprite *sprite);
void sub_8078634(u8 task);
u8 sub_80793A8(u8);

#endif // GUARD_ROM_8077ABC_H
