#ifndef GUARD_SCRIPT_MENU_H
#define GUARD_SCRIPT_MENU_H

#include "constants/script_menu.h"

extern const u8 *const gStdStrings[];

bool8 ScriptMenu_Multichoice(u8 left, u8 top, u8 multichoiceId, bool8 ignoreBPress);
bool8 ScriptMenu_MultichoiceWithDefault(u8 left, u8 top, u8 multichoiceId, bool8 ignoreBPress, u8 defaultChoice);
bool8 Multichoice(u8 left, u8 top, u8 multichoiceId, u8 ignoreBPress);
bool8 ScriptMenu_YesNo(u8 left, u8 top);
bool8 ScriptMenu_MultichoiceGrid(u8 left, u8 top, u8 multichoiceId, bool8 ignoreBPress, u8 columnCount);
bool8 ScriptMenu_CreatePCMultichoice(void);
void ScriptMenu_DisplayPCStartupPrompt(void);
bool8 ScriptMenu_ShowPokemonPic(u16 species, u8 x, u8 y);
bool8 (*ScriptMenu_HidePokemonPic(void))(void);

#endif // GUARD_SCRIPT_MENU_H
