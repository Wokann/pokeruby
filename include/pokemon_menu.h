#ifndef GUARD_POKEMON_MENU_H
#define GUARD_POKEMON_MENU_H

#include "menu.h"

#define POKEMENU_FIRST_FIELD_MOVE_ID 10

enum
{
    FIELD_MOVE_CUT,
    FIELD_MOVE_FLASH,
    FIELD_MOVE_ROCK_SMASH,
    FIELD_MOVE_STRENGTH,
    FIELD_MOVE_SURF,
    FIELD_MOVE_FLY,
    FIELD_MOVE_DIVE,
    FIELD_MOVE_WATERFALL,
    FIELD_MOVE_TELEPORT,
    FIELD_MOVE_DIG,
    FIELD_MOVE_SECRET_POWER,
    FIELD_MOVE_MILK_DRINK,
    FIELD_MOVE_SOFT_BOILED,
    FIELD_MOVE_SWEET_SCENT,
    FIELD_MOVES_COUNT,
};

enum
{
    POKEMENU_SUMMARY,           // 0
    POKEMENU_SWITCH,            // 1
    POKEMENU_ITEM,              // 2
    POKEMENU_CANCEL,            // 3
    POKEMENU_GIVE_ITEM,         // 4
    POKEMENU_TAKE_ITEM,         // 5
    POKEMENU_TAKE_MAIL,         // 6
    POKEMENU_MAIL,              // 7
    POKEMENU_READ_MAIL,         // 8
    POKEMENU_CANCEL_SUBMENU,    // 9
    POKEMENU_CUT,               // 10
	POKEMENU_FLASH,             // 11
	POKEMENU_ROCK_SMASH,        // 12
	POKEMENU_STRENGTH,          // 13
	POKEMENU_SURF,              // 14
	POKEMENU_FLY,               // 15
	POKEMENU_DIVE,              // 16
	POKEMENU_WATERFALL,         // 17
	POKEMENU_TELEPORT,          // 18
	POKEMENU_DIG,               // 19
	POKEMENU_SECRET_POWER,      // 20
	POKEMENU_MILK_DRINK,        // 21
	POKEMENU_SOFT_BOILED,       // 22
	POKEMENU_SWEET_SCENT,       // 23
};

extern u8 gLastFieldPokeMenuOpened;
extern void (*gPostMenuFieldCallback)(void);

void HandleDefaultPartyMenu(u8 taskId);
void SetTaskToWaitForLinkRecv(u32 taskId);
void CB2_OpenPartyMenuFromStartMenu(void);
void DrawPartyPopupMenu(u8 left, u8 top, u8 width, u8 numChoices, const struct MenuAction2 *menuActions, const u8 *items);
void DoPokemonMenu_Switch(u8 taskId);
void FieldCallback_PrepareFadeInFromMenu(void);
void CB2_ReturnToPartyMenuFromFlyMap(void);
void CB2_OpenPartyMenuForItemUse(void);
void Task_HandlePartyMenuItemUseInput(u8 taskId);
void Task_ExitPartyMenuToBag(u8 taskId);
void CB2_ReturnToPartyMenuForMoveLearning(void);
void Debug_TryUseWaterfall(void);
void CB2_ReturnToPartyMenuFromBag(void);

#endif // GUARD_POKEMON_MENU_H
