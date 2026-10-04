#ifndef GUARD_FIELD_EFFECTS_H
#define GUARD_FIELD_EFFECTS_H

#include "sprite.h"
#include "task.h"

extern const struct CompressedSpritePalette gTrainerFrontPicPaletteTable[2];
extern const struct CompressedSpriteSheet gTrainerFrontPicTable[2];

extern u8 gLastFieldPokeMenuOpened;

u32 FieldEffectStart(u8 id);
bool8 FieldEffectCmd_loadtiles(u8 **script, u32 *val);
bool8 FieldEffectCmd_loadfadedpal(u8 **script, u32 *val);
bool8 FieldEffectCmd_loadpal(u8 **script, u32 *val);
bool8 FieldEffectCmd_callnative(u8 **script, u32 *val);
bool8 FieldEffectCmd_end(u8 **script, u32 *val);
bool8 FieldEffectCmd_loadgfx_callnative(u8 **script, u32 *val);
bool8 FieldEffectCmd_loadtiles_callnative(u8 **script, u32 *val);
bool8 FieldEffectCmd_loadfadedpal_callnative(u8 **script, u32 *val);
u32 FieldEffectScript_ReadWord(u8 **script);
void FieldEffectScript_LoadTiles(u8 **script);
void FieldEffectScript_LoadFadedPalette(u8 **script);
void FieldEffectScript_LoadPalette(u8 **script);
void FieldEffectScript_CallNative(u8 **script, u32 *val);
void FieldEffectFreeGraphicsResources(struct Sprite *sprite);
void FieldEffectStop(struct Sprite *sprite, u8 id);
void FieldEffectFreeTilesIfUnused(u16 tileStart);
void FieldEffectFreePaletteIfUnused(u8 paletteNum);
void FieldEffectActiveListClear(void);
void FieldEffectActiveListAdd(u8 id);
void FieldEffectActiveListRemove(u8 id);
bool8 FieldEffectActiveListContains(u8 id);

void ReturnToFieldFromFlyMapSelect(void);

void FieldCB_FallWarpExit(void);

void StartEscalatorWarp(u8 metatileBehavior, u8 priority);

void StartLavaridgeGymB1FWarp(u8 priority);

void StartLavaridgeGym1FWarp(u8 priority);

u8 CreateTrainerSprite(u8 trainerSpriteID, s16 x, s16 y, u8 subpriority, u8 *buffer);
void LoadTrainerGfx_TrainerCard(u8 gender, u16 palOffset, u8 *dest);
u8 AddNewGameBirchObject(s16 x, s16 y, u8 subpriority);
u8 CreateMonSprite_PicBox(u16 species, s16 x, s16 y, u8 subpriority);
void FreeResourcesAndDestroySprite(struct Sprite *sprite);
void MultiplyInvertedPaletteRGBComponents(u16 i, u8 r, u8 g, u8 b);
void StartEscapeRopeFieldEffect(void);
void FldEff_TeleportWarpOut(void);
void MultiplyPaletteRGBComponents(u16 i, u8 r, u8 g, u8 b);

void SpriteCB_AshLaunch(struct Sprite *sprite);
void SpriteCB_AshPuff(struct Sprite *sprite);
extern s32 gFieldEffectArguments[8];

#endif // GUARD_FIELD_EFFECTS_H
