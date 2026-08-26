#include "global.h"
#include "event_object_lock.h"
#include "event_object_movement.h"
#include "field_player_avatar.h"
#include "script_movement.h"
#include "task.h"
#include "constants/event_objects.h"

extern u16 gSpecialVar_Facing;
u8 gSelectedObjectEvent;

bool8 IsPlayerStandingStill(void)
{
    if (gPlayerAvatar.tileTransitionState == T_TILE_TRANSITION)
        return FALSE;
    else
        return TRUE;
}

void Task_FreezePlayer(u8 taskId)
{
    if (IsPlayerStandingStill())
    {
        sub_80594C0();
        DestroyTask(taskId);
    }
}

bool8 IsFreezePlayerFinished(void)
{
    if (FuncIsActiveTask(Task_FreezePlayer))
    {
        return FALSE;
    }
    else
    {
        sub_80597F4();
        return TRUE;
    }
}

void FreezeObjects_WaitForPlayer(void)
{
    FreezeObjectEvents();
    CreateTask(Task_FreezePlayer, 80);
}

void Task_FreezeSelectedObjectAndPlayer(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    if (!task->data[0] && IsPlayerStandingStill() == TRUE)
    {
        sub_80594C0();
        task->data[0] = 1;
    }
    if (!task->data[1] && !gObjectEvents[gSelectedObjectEvent].singleMovementActive)
    {
        FreezeObjectEvent(&gObjectEvents[gSelectedObjectEvent]);
        task->data[1] = 1;
    }
    if (task->data[0] && task->data[1])
        DestroyTask(taskId);
}

bool8 IsFreezeSelectedObjectAndPlayerFinished(void)
{
    if (FuncIsActiveTask(Task_FreezeSelectedObjectAndPlayer))
    {
        return FALSE;
    }
    else
    {
        sub_80597F4();
        return TRUE;
    }
}

void FreezeObjects_WaitForPlayerAndSelected(void)
{
    u8 taskId;
    FreezeObjectEventsExceptOne(gSelectedObjectEvent);
    taskId = CreateTask(Task_FreezeSelectedObjectAndPlayer, 80);
    if (!gObjectEvents[gSelectedObjectEvent].singleMovementActive)
    {
        FreezeObjectEvent(&gObjectEvents[gSelectedObjectEvent]);
        gTasks[taskId].data[1] = 1;
    }
}

void ScriptUnfreezeObjectEvents(void)
{
    u8 playerObjectId = GetObjectEventIdByLocalIdAndMap(LOCALID_PLAYER, 0, 0);
    ObjectEventClearHeldMovementIfFinished(&gObjectEvents[playerObjectId]);
    sub_80A2178();
    UnfreezeObjectEvents();
}

void UnlockPlayerAndSelectedObject(void)
{
    u8 playerObjectId;

    if (gObjectEvents[gSelectedObjectEvent].active)
        ObjectEventClearHeldMovementIfFinished(&gObjectEvents[gSelectedObjectEvent]);
    playerObjectId = GetObjectEventIdByLocalIdAndMap(LOCALID_PLAYER, 0, 0);
    ObjectEventClearHeldMovementIfFinished(&gObjectEvents[playerObjectId]);
    sub_80A2178();
    UnfreezeObjectEvents();
}

void Script_FacePlayer(void)
{
    ObjectEventFaceOppositeDirection(&gObjectEvents[gSelectedObjectEvent], gSpecialVar_Facing);
}

void Script_ClearHeldMovement(void)
{
    ObjectEventClearHeldMovementIfActive(&gObjectEvents[gSelectedObjectEvent]);
}
