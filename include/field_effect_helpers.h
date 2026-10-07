#ifndef GUARD_FIELD_EFFECT_HELPERS_H
#define GUARD_FIELD_EFFECT_HELPERS_H

extern const struct SpriteTemplate *const gFieldEffectObjectTemplatePointers[];

void StartRevealDisguise(struct ObjectEvent *objectEvent);
bool8 UpdateRevealDisguise(struct ObjectEvent *objectEvent);
void UpdateShadowFieldEffect(struct Sprite *);
void UpdateShortGrassFieldEffect(struct Sprite *);
void UpdateFootprintsTireTracksFieldEffect(struct Sprite *);
void UpdateSplashFieldEffect(struct Sprite *);
void UpdateHotSpringsWaterFieldEffect(struct Sprite *);
void UpdateAshFieldEffect(struct Sprite *);
void UpdateSurfBlobFieldEffect(struct Sprite *);
void UpdateSandPileFieldEffect(struct Sprite *);
void UpdateBubblesFieldEffect(struct Sprite *);
void UpdateDisguiseFieldEffect(struct Sprite *);
void UpdateSparkleFieldEffect(struct Sprite *);
void UpdateJumpImpactEffect(struct Sprite *);
void WaitFieldEffectSpriteAnim(struct Sprite *);
void UpdateTallGrassFieldEffect(struct Sprite *);
void UpdateLongGrassFieldEffect(struct Sprite *);
u8 CreateWarpArrowSprite(void);
void SetSpriteInvisible(u8 spriteId);
void ShowWarpArrowSprite(u8 spriteId, u8 direction, s16 x, s16 y);
void SetSurfBlob_BobState(u8 spriteId, u8 state);
void SetSurfBlob_DontSyncAnim(u8 spriteId, u8 dontSync);
void SetSurfBlob_PlayerOffset(u8 spriteId, u8 hasOffset, s16 offset);
u8 StartUnderwaterSurfBlobBobbing(u8 blobSpriteId);
void StartAshFieldEffect(s16 x, s16 y, u16 metatileId, s16 delay);
void SetUpReflection(struct ObjectEvent *objectEvent, struct Sprite *sprite, u8 stillReflection);
u32 StartFieldEffectForObjectEvent(u8 fieldEffectId, struct ObjectEvent *objectEvent);
u8 FindTallGrassFieldEffectSpriteId(u8 localId, u8 mapNum, u8 mapGroup, s16 x, s16 y);

#endif // GUARD_FIELD_EFFECT_HELPERS_H
