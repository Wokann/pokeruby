#ifndef GUARD_NEW_GAME_H
#define GUARD_NEW_GAME_H

extern u8 gIsDebugMode;

void SetTrainerId(u32 trainerId, u8 *dst);
void CopyTrainerId(u8 *dst, u8 *src);
void InitPlayerTrainerId(void);
void SetDefaultOptions(void);
void ClearPokedexFlags(void);
void ClearAllContestWinnerPics(void);
void WarpToTruck(void);
void Sav2_ClearSetDefault(void);
void ResetMenuAndMonGlobals(void);
void NewGameInitData(void);
void Debug_NewGameInit(bool32 setKroteNickname);

#if DEBUG
void DebugSetNextWarpDestination();
#endif

#endif // GUARD_NEW_GAME_H
