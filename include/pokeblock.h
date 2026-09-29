#ifndef GUARD_POKEBLOCK_H
#define GUARD_POKEBLOCK_H

#define POKEBLOCK_MAX_FEEL 99

enum
{
    PBLOCK_CLR_NONE,
    PBLOCK_CLR_RED,
    PBLOCK_CLR_BLUE,
    PBLOCK_CLR_PINK,
    PBLOCK_CLR_GREEN,
    PBLOCK_CLR_YELLOW,
    PBLOCK_CLR_PURPLE,
    PBLOCK_CLR_INDIGO,
    PBLOCK_CLR_BROWN,
    PBLOCK_CLR_LITE_BLUE,
    PBLOCK_CLR_OLIVE,
    PBLOCK_CLR_GRAY,
    PBLOCK_CLR_BLACK,
    PBLOCK_CLR_WHITE,
    PBLOCK_CLR_GOLD,
};

enum
{
    PBLOCK_COLOR,
    PBLOCK_SPICY,
    PBLOCK_DRY,
    PBLOCK_SWEET,
    PBLOCK_BITTER,
    PBLOCK_SOUR,
    PBLOCK_FEEL,
};

// Ruby uses separate Bag and field exit callbacks for the Pokeblock Case.
enum
{
    PBLOCK_CASE_BAG,
    PBLOCK_CASE_FIELD,
    PBLOCK_CASE_BATTLE,
    PBLOCK_CASE_FEEDER,
};

extern const s8 gPokeblockFlavorCompatibilityTable[];
extern const u8 *const gPokeblockNames[];
extern const struct CompressedSpriteSheet gPokeblockCase_SpriteSheet;
extern const struct CompressedSpritePalette gPokeblockCase_SpritePal;

void CB2_InitPokeblockMenu(void);
u8 CreatePokeblockCaseSprite(s16, s16, u8);
u8 GetHighestPokeblocksFlavorLevel(const struct Pokeblock *);
s16 GetPokeblockData(const struct Pokeblock *, u8);
u8 GetPokeblocksFeel(const struct Pokeblock *);
void SetPokeblockCaseContext(u8);
void ClearPokeblocks(void);
bool8 TryClearPokeblock(u8);
s16 PokeblockGetGain(u8, const struct Pokeblock *);
u8 CopyMonFavoritePokeblockName(u8, u8*);
void PokeblockCopyName(const struct Pokeblock *pokeblock, u8 *dest);
void PreparePokeblockFeedScene(void);
bool8 AddPokeblock(const struct Pokeblock *);

#include "main.h"

void ChooseMonToGivePokeblock(struct Pokeblock *, MainCallback);

#endif // GUARD_POKEBLOCK_H
