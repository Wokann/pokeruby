#ifndef GUARD_EVENT_OBJECT_LOCK_H
#define GUARD_EVENT_OBJECT_LOCK_H

bool8 IsPlayerStandingStill(void);
void Task_FreezePlayer(u8 taskId);
bool8 IsFreezePlayerFinished(void);
void ScriptFreezeObjectEvents(void);
void Task_FreezeSelectedObjectAndPlayer(u8 taskId);
bool8 IsFreezeSelectedObjectAndPlayerFinished(void);
void LockSelectedObjectEvent(void);
void ScriptUnfreezeObjectEvents(void);
void unref_sub_8064E5C(void);
void Script_FacePlayer(void);
void Script_ClearHeldMovement(void);

#endif // GUARD_EVENT_OBJECT_LOCK_H
