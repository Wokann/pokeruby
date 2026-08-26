#ifndef GUARD_EVENT_OBJECT_LOCK_H
#define GUARD_EVENT_OBJECT_LOCK_H

bool8 IsPlayerStandingStill(void);
void Task_FreezePlayer(u8 taskId);
bool8 IsFreezePlayerFinished(void);
void FreezeObjects_WaitForPlayer(void);
void Task_FreezeSelectedObjectAndPlayer(u8 taskId);
bool8 IsFreezeSelectedObjectAndPlayerFinished(void);
void FreezeObjects_WaitForPlayerAndSelected(void);
void ScriptUnfreezeObjectEvents(void);
void UnlockPlayerAndSelectedObject(void);
void Script_FacePlayer(void);
void Script_ClearHeldMovement(void);

#endif // GUARD_EVENT_OBJECT_LOCK_H
