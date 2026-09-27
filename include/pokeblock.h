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
    PBLOCK_CLR_LITEBLUE,
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

void CB2_InitPokeblockMenu(void);
u8 CreatePokeblockCaseSprite(s16, s16, u8);
u8 GetHighestPokeblocksFlavorLevel(struct Pokeblock *);
s16 GetPokeblockData(const struct Pokeblock *, u8);
u8 GetPokeblocksFeel(struct Pokeblock *);
void SetPokeblockCaseContext(u8);
void ClearPokeblocks(void);
bool8 PokeblockClearIfExists(u8);
s16 PokeblockGetGain(u8, const struct Pokeblock *);
u8 CopyMonFavoritePokeblockName(u8, u8*);
void PokeblockCopyName(struct Pokeblock *pokeblock, u8 *dest);
void CB2_PreparePokeblockFeedScene(void);
bool8 GivePokeblock(const struct Pokeblock *);

#include "main.h"

void sub_8136130(struct Pokeblock *, MainCallback);

#endif // GUARD_POKEBLOCK_H
