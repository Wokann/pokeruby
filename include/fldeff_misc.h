#ifndef GUARD_FLDEFF_MISC_H
#define GUARD_FLDEFF_MISC_H

bool8 SetUpFieldMove_SecretPower(void);
bool8 FldEff_UseSecretPowerCave(void);
bool8 FldEff_SecretPowerCave(void);
bool8 FldEff_UseSecretPowerTree(void);
bool8 FldEff_SecretPowerTree(void);
bool8 FldEff_UseSecretPowerShrub(void);
bool8 FldEff_SecretPowerShrub(void);
u32 FldEff_SecretBasePCTurnOn(void);
void DoSecretBasePCTurnOffEffect(void);
void PopSecretBaseBalloon(s16 metatileId, s16 x, s16 y);
bool8 FldEff_Nop47(void);
bool8 FldEff_Nop48(void);
void ShatterSecretBaseBreakableDoor(s16 x, s16 y);
void PlaySecretBaseMusicNoteMatSound(s16 metatileId);
void DoSecretBaseGlitterMatSparkle(void);
bool8 FldEff_SandPillar(void);

extern const struct SpritePalette gSpritePalette_SecretPower_Cave;
extern const struct SpritePalette gSpritePalette_SecretPower_Plant;
extern const struct SpritePalette gSpritePalette_SandPillar;
extern const u16 gTilesetPalettes_SecretBase[][16];

extern const u8 gSandPillar0_Gfx[];
extern const u8 gSandPillar1_Gfx[];
extern const u8 gSandPillar2_Gfx[];

#endif //GUARD_FLDEFF_MISC_H
