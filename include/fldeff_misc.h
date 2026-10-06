#ifndef GUARD_FLDEFF_MISC_H
#define GUARD_FLDEFF_MISC_H

bool8 SetUpFieldMove_SecretPower(void);
bool8 FldEff_UseSecretPowerCave(void);
bool8 FldEff_SecretPowerCave(void);
bool8 FldEff_UseSecretPowerTree(void);
bool8 FldEff_SecretPowerTree(void);
bool8 FldEff_UseSecretPowerShrub(void);
bool8 FldEff_SecretPowerShrub(void);

extern const struct SpritePalette gSpritePalette_SecretPower_Cave;
extern const struct SpritePalette gSpritePalette_SecretPower_Plant;

extern const u8 gSandPillar0_Gfx[];
extern const u8 gSandPillar1_Gfx[];
extern const u8 gSandPillar2_Gfx[];

#endif //GUARD_FLDEFF_MISC_H
