#ifndef GUARD_POKEDEX_H
#define GUARD_POKEDEX_H

#include "sprite.h"

enum
{
    DEX_MODE_HOENN,
    DEX_MODE_NATIONAL
};

void ResetPokedex(void);
void ResetPokedexScrollPositions(void);
void CB2_OpenPokedex(void);
u8 DisplayCaughtMonDexPage(u16 dexNum, u32 otId, u32 personality);
const u8 *GetPokedexCategoryName(u16 dexNum);
u16 GetPokedexHeightWeight(u16 dexNum, u8 data);

enum
{
    FLAG_GET_SEEN,
    FLAG_GET_CAUGHT,
    FLAG_SET_SEEN,
    FLAG_SET_CAUGHT
};

s8 GetSetPokedexFlag(u16 nationalDexNo, u8 caseID);

u16 GetNationalPokedexCount(u8 caseID);
u16 GetHoennPokedexCount(u8 caseID);
bool8 HasAllHoennMons(void);
bool16 HasAllMons(void);

extern u8 gUnusedPokedexU8;
extern void (*gPokedexVBlankCB)(void);

struct PokedexEntry
{
    /*0x00*/ u8 categoryName[12];
    /*0x0C*/ u16 height; //in decimeters
    /*0x0E*/ u16 weight; //in hectograms
    /*0x10*/ const u8 *descriptionPage1;
    /*0x14*/ const u8 *descriptionPage2;
    /*0x18*/ u16 unused;
    /*0x1A*/ u16 pokemonScale;
    /*0x1C*/ u16 pokemonOffset;
    /*0x1E*/ u16 trainerScale;
    /*0x20*/ u16 trainerOffset;
};  /*size = 0x24*/

extern const struct PokedexEntry gPokedexEntries[];

u16 CreateSizeScreenTrainerPic(u16 gender, s16 x, s16 y, u16 paletteSlot);
u16 CreateMonSpriteFromNationalDexNumber(u16 nationalNum, s16 x, s16 y, u16 paletteSlot);

void DrawFootprint(u16 dexNum, u16 charBase, u16 tileNum);

#endif // GUARD_POKEDEX_H
