#ifndef GUARD_POKEMON_ICON_H
#define GUARD_POKEMON_ICON_H

extern const u8 *const gMonIconTable[];
extern const u8 gMonIconPaletteIndices[];

u8 CreateMonIcon(u16 species, void (*callback)(struct Sprite *), s16 x, s16 y, u8 subpriority, u32 personality);
u8 UpdateMonIconFrame(struct Sprite *sprite);
u8 CreateMonIconNoPersonality(u16 species, void (*callback)(struct Sprite *), s16 x, s16 y, u8 subpriority);
u16 GetUnownLetterByPersonality(u32 personality);
u16 GetIconSpeciesNoPersonality(u16 species);
void FreeAndDestroyMonIconSprite(struct Sprite *sprite);
void LoadMonIconPalettes(void);
void LoadMonIconPalette(u16 species);
void FreeMonIconPalette(u16 species);
void SpriteCB_MonIcon(struct Sprite *sprite);
void SetPartyHPBarSprite(struct Sprite *sprite, u8 animNum);
const u8 *GetMonIconPtr(u16 species, u32 personality);
u16 GetIconSpecies(u16 species, u32 personality);

#endif // GUARD_POKEMON_ICON_H
