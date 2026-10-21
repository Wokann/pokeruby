#include "global.h"
#include "event_data.h"
#include "event_object_lock.h"
#include "event_object_movement.h"
#include "field_player_avatar.h"
#include "field_screen_effect.h"
#include "script_movement.h"
#include "task.h"
#include "constants/event_objects.h"

bool8 IsPlayerStandingStill(void)
{
    if (gPlayerAvatar.tileTransitionState == T_TILE_TRANSITION)
        return FALSE;
    else
        return TRUE;
}

static void Task_FreezePlayer(u8 taskId)
{
    if (IsPlayerStandingStill())
    {
        PlayerFreeze();
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
        StopPlayerAvatar();
        return TRUE;
    }
}

void FreezeObjects_WaitForPlayer(void)
{
    FreezeObjectEvents();
    CreateTask(Task_FreezePlayer, 80);
}

#define tPlayerFrozen data[0]
#define tObjectFrozen data[1]

static void Task_FreezeSelectedObjectAndPlayer(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    if (!task->tPlayerFrozen && IsPlayerStandingStill() == TRUE)
    {
        PlayerFreeze();
        task->tPlayerFrozen = TRUE;
    }
    if (!task->tObjectFrozen && !gObjectEvents[gSelectedObjectEvent].singleMovementActive)
    {
        FreezeObjectEvent(&gObjectEvents[gSelectedObjectEvent]);
        task->tObjectFrozen = TRUE;
    }
    if (task->tPlayerFrozen && task->tObjectFrozen)
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
        StopPlayerAvatar();
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
        gTasks[taskId].tObjectFrozen = TRUE;
    }
}

#undef tPlayerFrozen
#undef tObjectFrozen

void ScriptUnfreezeObjectEvents(void)
{
    u8 playerObjectId = GetObjectEventIdByLocalIdAndMap(LOCALID_PLAYER, 0, 0);
    ObjectEventClearHeldMovementIfFinished(&gObjectEvents[playerObjectId]);
    ScriptMovement_UnfreezeObjectEvents();
    UnfreezeObjectEvents();
}

void unref_sub_8064E5C(void)
{
    u8 playerObjectId;

    if (gObjectEvents[gSelectedObjectEvent].active)
        ObjectEventClearHeldMovementIfFinished(&gObjectEvents[gSelectedObjectEvent]);
    playerObjectId = GetObjectEventIdByLocalIdAndMap(LOCALID_PLAYER, 0, 0);
    ObjectEventClearHeldMovementIfFinished(&gObjectEvents[playerObjectId]);
    ScriptMovement_UnfreezeObjectEvents();
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
