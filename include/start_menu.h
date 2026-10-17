#ifndef GUARD_START_MENU_H
#define GUARD_START_MENU_H

#include "task.h"

extern bool8 (*gMenuCallback)(void);

//Public functions
void SetDexPokemonPokenavFlags(void);
void CreateStartMenuTask(TaskFunc followupFunc);
void Task_ShowStartMenu(u8 taskId);
void ShowStartMenu(void);
void SaveGame(void);
void CB2_SetUpSaveAfterLinkBattle(void);
void AppendToList(u8 *list, u8 *pos, u32 newEntry);
void debug_sub_8075DB4(struct BattleTowerEReaderTrainer *ereaderTrainer, const u8 *b, u32 trainerId);
bool8 debug_sub_8075C30(void);

#if DEBUG

#if (ENGLISH && REVISION == 0)
#define TYPE u8
#else
#define TYPE u32
#endif

extern TYPE gUnknown_Debug_03004BD0;
// TODO: see if this is in rev1+
#if (ENGLISH && REVISION == 0)
extern TYPE gUnknown_Debug_Murakawa2;
#endif

#undef TYPE
#endif // DEBUG

#endif // GUARD_START_MENU_H
