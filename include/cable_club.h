#ifndef GUARD_CABLE_CLUB_H
#define GUARD_CABLE_CLUB_H

#include "task.h"

void TryBattleLinkup(void);
void TryTradeLinkup(void);
void TryRecordMixLinkup(void);
void ValidateMixingGameLanguage(void);
void TryBerryBlenderLinkup(void);
void TryContestGModeLinkup(void);
u8 sub_8083664(void);
void sub_8083820(void);
void CleanupLinkRoomState(void);
void ExitLinkRoom(void);
void sub_8083A84(TaskFunc followupFunc);
void PlayerEnteredTradeSeat(void);
void sub_8083B80(void);
void ColosseumPlayerSpotTriggered(void);
void Script_ShowLinkTrainerCard(void);
bool32 sub_8083BF4(u8 linkPlayerIndex);
void sub_8083C50(u8 taskId);
#if DEBUG
void debug_sub_808B838(u8);
bool8 debug_sub_8138CC4(void);
#endif

#endif // GUARD_CABLE_CLUB_H
