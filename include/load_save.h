#ifndef GUARD_LOAD_SAVE_H
#define GUARD_LOAD_SAVE_H

extern bool32 gFlashMemoryPresent;

void CheckForFlashMemory(void);
bool32 GetContinueGameWarpStatus(void);
void ClearContinueGameWarpStatus(void);
void SetContinueGameWarpStatus(void);
void SetContinueGameWarpStatusToDynamicWarp(void);
void ClearContinueGameWarpStatus2(void);
void SavePlayerParty(void);
void LoadPlayerParty(void);
void SaveSerializedGame(void);
void LoadSerializedGame(void);
void LoadPlayerBag(void);
void SavePlayerBag(void);

#endif // GUARD_LOAD_SAVE_H
