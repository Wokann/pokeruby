#include "global.h"
#include "fldeff_misc.h"
#include "event_object_movement.h"
#include "field_player_avatar.h"
#include "script.h"
#include "task.h"

static void Task_WateringBerryTreeAnim_Start(u8);
static void Task_WateringBerryTreeAnim_Continue(u8);
static void Task_WateringBerryTreeAnim_End(u8);

static void Task_WateringBerryTreeAnim(u8 taskId)
{
    gTasks[taskId].func = Task_WateringBerryTreeAnim_Start;
}

static void Task_WateringBerryTreeAnim_Start(u8 taskId)
{
    struct ObjectEvent *playerObjEvent = &gObjectEvents[gPlayerAvatar.objectEventId];
    if (!ObjectEventIsMovementOverridden(playerObjEvent)
        || ObjectEventClearHeldMovementIfFinished(playerObjEvent))
    {
        SetPlayerAvatarWatering(GetPlayerFacingDirection());
        ObjectEventSetHeldMovement(playerObjEvent, GetWalkInPlaceNormalMovementAction(GetPlayerFacingDirection()));
        gTasks[taskId].func = Task_WateringBerryTreeAnim_Continue;
    }
}

static void Task_WateringBerryTreeAnim_Continue(u8 taskId)
{
    struct ObjectEvent *playerObjEvent = &gObjectEvents[gPlayerAvatar.objectEventId];
    if (ObjectEventClearHeldMovementIfFinished(playerObjEvent))
    {
        s16 value = gTasks[taskId].data[1]++;
        
        if (value < 10)
        {
            ObjectEventSetHeldMovement(playerObjEvent, GetWalkInPlaceNormalMovementAction(GetPlayerFacingDirection()));
        }
        else
        {
            gTasks[taskId].func = Task_WateringBerryTreeAnim_End;
        }
    }
}

static void Task_WateringBerryTreeAnim_End(u8 taskId)
{
    SetPlayerAvatarTransitionFlags(GetPlayerAvatarFlags());
    DestroyTask(taskId);
    ScriptContext_Enable();
}

void DoWateringBerryTreeAnim(void)
{
    CreateTask(Task_WateringBerryTreeAnim, 80);
}
