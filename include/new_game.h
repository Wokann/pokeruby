#ifndef GUARD_NEW_GAME_H
#define GUARD_NEW_GAME_H

extern bool8 gDifferentSaveFile;
extern u8 gUnknown_020297ED;

void SetTrainerId(u32 trainerId, u8 *dst);
void CopyTrainerId(u8 *dst, u8 *src);
void ResetContestAndMuseumWinners(void);
void ZeroBattleTowerData(void);
void Sav2_ClearSetDefault(void);
void ResetMenuAndMonGlobals(void);
void NewGameInitData(void);
void debug_sub_8057508(bool32 arg0);

#if DEBUG
void debug_sub_8052E04();
#endif

#endif // GUARD_NEW_GAME_H
