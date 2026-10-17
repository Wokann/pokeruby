#ifndef GUARD_POKEMON_MENU_H
#define GUARD_POKEMON_MENU_H

#include "main.h"
#include "menu.h"

#define MENU_FIELD_MOVES 10

enum
{
    MENU_SUMMARY,               // 0
    MENU_SWITCH,                // 1
    MENU_ITEM,                  // 2
    MENU_CANCEL1,               // 3
    MENU_GIVE,                  // 4
    MENU_TAKE_ITEM,             // 5
    MENU_TAKE_MAIL,             // 6
    MENU_MAIL,                  // 7
    MENU_READ,                  // 8
    MENU_CANCEL2,               // 9
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

void HandleDefaultPartyMenu(u8 taskId);
void sub_808B5B4(u32 taskId);
void sub_8089C50(u8 arg0, u8 arg1, u8 arg2, u8 noOfOptions, const struct MenuAction2 *menuActions, const u8 *order);
void DoPokemonMenu_Switch(u8 taskId);
void sub_808B0C0(u8 taskId);
void sub_808B508(u8 taskId);
void CB2_ReturnToPartyMenuWhileLearningMove(void);
void debug_sub_80986AC(void);
void sub_808A3F8(void);

#endif // GUARD_POKEMON_MENU_H
