#ifndef GUARD_POKEBLOCK_H
#define GUARD_POKEBLOCK_H

enum
{
    PBLOCK_CLR_RED = 1,
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

extern u8 gPokeblockMonId;
extern s16 gPokeblockGain;

extern const s8 gPokeblockFlavorCompatibilityTable[];
extern const u8 *const gPokeblockNames[];
extern const struct CompressedSpriteSheet gPokeblockCase_SpriteSheet;
extern const struct CompressedSpritePalette gPokeblockCase_SpritePal;

void sub_810B96C(void);
u8 CreatePokeblockCaseSprite(s16 x, s16 y, u8 subpriority);
u8 GetHighestPokeblocksFlavorLevel(struct Pokeblock *pokeblock);
s16 GetPokeblockData(const struct Pokeblock *pokeblock, u8 field);
u8 GetPokeblocksFeel(struct Pokeblock *pokeblock);
void sub_810BA7C(u8 caseId);
void OpenPokeblockCaseInBattle(void);
void ClearPokeblocks(void);
s8 GetFirstFreePokeblockSlot(void);
bool8 TryClearPokeblock(u8 pokeblockIdx);
s16 PokeblockGetGain(u8 nature, const struct Pokeblock *pokeblock);
bool8 CopyMonFavoritePokeblockName(u8 nature, u8 *dest);
void PokeblockCopyName(struct Pokeblock *pokeblock, u8 *dest);
void PreparePokeblockFeedScene(void);
bool8 GivePokeblock(const struct Pokeblock *pokeblock);

#include "main.h"

void ChooseMonToGivePokeblock(struct Pokeblock *pokeblock, MainCallback callback);
u8 GetPartyIdFromSelectionId_(u8 selectionId);

#endif // GUARD_POKEBLOCK_H
