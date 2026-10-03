#ifndef GUARD_EVENT_OBJECT_LOCK_H
#define GUARD_EVENT_OBJECT_LOCK_H

bool8 IsPlayerStandingStill(void);
bool8 IsFreezePlayerFinished(void);
void FreezeObjects_WaitForPlayer(void);
bool8 IsFreezeSelectedObjectAndPlayerFinished(void);
void FreezeObjects_WaitForPlayerAndSelected(void);
void ScriptUnfreezeObjectEvents(void);
void unref_sub_8064E5C(void);
void Script_FacePlayer(void);
void Script_ClearHeldMovement(void);

#endif // GUARD_EVENT_OBJECT_LOCK_H
