#ifndef GUARD_ITEM_MENU_H
#define GUARD_ITEM_MENU_H

#include "string_util.h"

enum
{
	ITEM_USE_SINGLE_MON,
	ITEM_USE_GIVE_ITEM,
	ITEM_USE_UNUSED,
	ITEM_USE_GIVE_MAIL,
	ITEM_USE_ALL_MONS,
};

struct PocketScrollState
{
    u8 cursorPos;
    u8 scrollTop;
    u8 numSlots;
    u8 cursorMax;
};

extern struct PocketScrollState gBagPocketScrollStates[];
extern struct ItemSlot *gCurrentBagPocketItemSlots;
extern u8 gPokemonItemUseType;

void ResetBagScrollPositions(void);
void ClearBag(void);
void UpdateCurrentBagItemSlot(void);
void FillBgTilemapRect(u16 *tilemap, u8 left, u8 top, u8 width, u8 height, u16 tile);
void ConvertIntToItemQuantityString(u8 *dst, u16 value, enum StringConvertMode mode, u8 digits);
void PrintItemQuantity(u16 value, enum StringConvertMode mode, u8 x, u8 y, u8 digits);
void CB2_BagMenuFromStartMenu(void);
void HandleItemMenuPaletteFade(u8);
void CB2_ReturnToBagMenu(void);
void CleanUpItemMenuMessage(u8);
void CleanUpOverworldMessage(u8);
void ExecuteItemUseFromBlackPalette(void);
void CB2_ReturnToBagMenuAfterMail(void);
void CB2_GoToSellMenu(void);
void CB2_ChooseBerry(void);
void CB2_GoToItemDepositMenu(void);
bool32 UseRegisteredKeyItem(void);
void CB2_BagMenuFromBattle(void);
void CloseBagMenuForBattle(u8);
u8 CreateBerryTagSprite(u8 berryId, s16 x, s16 y);
void FreeBerryTagSpritePalettes(void);
u8 CreateBerryFlavorCircleSprite(s16 x);

#endif // GUARD_ITEM_MENU_H
