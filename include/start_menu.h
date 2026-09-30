#ifndef GUARD_START_MENU_H
#define GUARD_START_MENU_H

extern u8 (*gMenuCallback)(void);

//Public functions
void CreateStartMenuTask(void (*func)(u8));
void Task_ShowStartMenu(u8 taskId);
void ShowStartMenu(void);
void SaveGame(void);
void CB2_SetUpSaveAfterLinkBattle(void);
void CreateDebugEReaderTrainer(struct BattleTowerEReaderTrainer *eReaderTrainer, const u8 *trainerName, u32 trainerId);
bool8 DebugMenu_StartDmaTimingTest(void);

#if DEBUG

#if (ENGLISH && REVISION == 0)
extern u8 gDebugTextModeDisplayEnabled;
extern u8 gDebugTextModeDisplaySuspended;
#else
extern u32 gDebugForceFlashError;
#endif
#endif // DEBUG

#endif // GUARD_START_MENU_H
