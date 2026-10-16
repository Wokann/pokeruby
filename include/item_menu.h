#ifndef GUARD_ITEM_MENU_H
#define GUARD_ITEM_MENU_H

#include "item.h"
#include "menu_helpers.h"
#include "string_util.h"

enum
{
    ITEMMENULOCATION_FIELD,
    ITEMMENULOCATION_BATTLE,
    ITEMMENULOCATION_PARTY,
    ITEMMENULOCATION_SHOP,
    ITEMMENULOCATION_BERRY_TREE,
    RETURN_TO_FIELD_5,
    ITEMMENULOCATION_ITEMPC,
    ITEMMENULOCATION_WALLY,
};

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
    u8 cursorPosition;
    u8 scrollPosition;
    u8 numSlots;
    u8 cursorMax;
};

extern struct PocketScrollState gBagPocketScrollStates[];
extern struct ItemSlot *gCurrentBagPocketItemSlots;
extern u16 gSpecialVar_ItemId;

void sub_80A3E0C(void);
void sub_80A3FA0(u16 *a, u8 b, u8 c, u8 d, u8 e, u16 f);
void sub_80A4164(u8 *, u16, enum StringConvertMode, u8);
void sub_80A418C(u16 a, enum StringConvertMode b, u8 c, u8 d, u8 e);
void CB2_ReturnToBagMenuPocket(void);
void CleanUpOverworldMessage(u8);
void ExecuteItemUseFromBlackPalette(void);
void sub_80A5D04(void);
bool32 UseRegisteredKeyItemOnField(void);
void sub_80A7094(u8);

void CB2_BagMenuFromStartMenu(void);
void CB2_BagMenuFromBattle(void);
void CB2_GoToSellMenu(void);
void CB2_GoToItemDepositMenu(void);
void ChooseBerryForMachine(void);
void CB2_ChooseBerry(void);
void ResetBagScrollPositions(void);
void DoWallyTutorialBagMenu(void);
void Task_CloseBagMenu(u8 taskId);
void DisplayItemMessageOnField(u8 taskId, const u8 *string, TaskFunc taskFunc, u16 tile);
void CloseItemMessage(u8 taskId);

#endif // GUARD_ITEM_MENU_H
