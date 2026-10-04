#ifndef GUARD_CABLE_CLUB_H
#define GUARD_CABLE_CLUB_H

#include "task.h"

void TryBattleLinkup(u8 arg0);
void TryTradeLinkup(void);
void TryRecordMixLinkup(void);
void ValidateMixingGameLanguage(void);
void TryBerryBlenderLinkup(void);
void TryContestGModeLinkup(void);
u8 CreateTask_ReestablishCableClubLink(void);
void CableClubSaveGame(void);
void CleanupLinkRoomState(void);
void ExitLinkRoom(void);
void CreateTask_EnterCableClubSeat(TaskFunc followupFunc);
void PlayerEnteredTradeSeat(void);
void Script_StartWiredTrade(void);
void ColosseumPlayerSpotTriggered(void);
void Script_ShowLinkTrainerCard(void);
bool32 GetLinkTrainerCardColor(u8 linkPlayerIndex);
void Task_WaitForLinkPlayerConnection(u8 taskId);
#if DEBUG
void debug_sub_808B838(u8);
bool8 debug_sub_8138CC4(void);
#endif

#endif // GUARD_CABLE_CLUB_H
