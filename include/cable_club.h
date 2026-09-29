#ifndef GUARD_CABLE_CLUB_H
#define GUARD_CABLE_CLUB_H

#include "task.h"
#include "constants/cable_club.h"

void CreateTask_EnterCableClubSeat(TaskFunc followupFunc);
u8 CreateTask_ReestablishCableClubLink(void);
void Task_WaitForLinkPlayerConnection(u8 taskId);
bool32 GetLinkTrainerCardColor(u8 linkPlayerIndex);
#if DEBUG
void Debug_SetLinkStateFlag(u8 flagId);
#endif

#endif // GUARD_CABLE_CLUB_H
