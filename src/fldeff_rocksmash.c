#include "global.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_effect.h"
#include "field_player_avatar.h"
#include "item_use.h"
#include "overworld.h"
#include "pokemon_menu.h"
#include "fldeff.h"
#include "script.h"
#include "sound.h"
#include "sprite.h"
#include "task.h"
#include "constants/event_object_movement.h"
#include "constants/event_objects.h"
#include "constants/field_effects.h"
#include "constants/map_types.h"
#include "constants/songs.h"

extern u8 EventScript_UseRockSmash[];

EWRAM_DATA struct MapPosition gPlayerFacingPosition = {0};

static void Task_DoFieldMove_Init(u8);
static void Task_DoFieldMove_ShowMonAfterPose(u8);
static void Task_DoFieldMove_WaitForMon(u8);
static void Task_DoFieldMove_RunFunc(u8);
static void FieldCallback_RockSmash(void);
static void FieldMove_RockSmash(void);

bool8 CheckObjectGraphicsInFrontOfPlayer(u8 graphicsId)
{
    u8 objEventId;

    GetXYCoordsOneStepInFrontOfPlayer(&gPlayerFacingPosition.x, &gPlayerFacingPosition.y);
    gPlayerFacingPosition.elevation = PlayerGetElevation();
    objEventId = GetObjectEventIdByXYZ(gPlayerFacingPosition.x, gPlayerFacingPosition.y, gPlayerFacingPosition.elevation);
    if (gObjectEvents[objEventId].graphicsId != graphicsId)
    {
        return FALSE;
    }
    else
    {
        gSpecialVar_LastTalked = gObjectEvents[objEventId].localId;
        return TRUE;
    }
}

u8 CreateFieldMoveTask(void)
{
    GetXYCoordsOneStepInFrontOfPlayer(&gPlayerFacingPosition.x, &gPlayerFacingPosition.y);
    return CreateTask(Task_DoFieldMove_Init, 8);
}

static void Task_DoFieldMove_Init(u8 taskId)
{
    u8 objEventId;

    LockPlayerFieldControls();
    gPlayerAvatar.preventStep = TRUE;
    objEventId = gPlayerAvatar.objectEventId;
    if (!ObjectEventIsMovementOverridden(&gObjectEvents[objEventId])
     || ObjectEventClearHeldMovementIfFinished(&gObjectEvents[objEventId]))
    {
        if (gMapHeader.mapType == MAP_TYPE_UNDERWATER)
        {
            FieldEffectStart(FLDEFF_FIELD_MOVE_SHOW_MON_INIT);
            gTasks[taskId].func = Task_DoFieldMove_WaitForMon;
        }
        else
        {
            SetPlayerAvatarFieldMove();
            ObjectEventSetHeldMovement(&gObjectEvents[objEventId], MOVEMENT_ACTION_START_ANIM_IN_DIRECTION);
            gTasks[taskId].func = Task_DoFieldMove_ShowMonAfterPose;
        }
    }
}

static void Task_DoFieldMove_ShowMonAfterPose(u8 taskId)
{
    if (ObjectEventCheckHeldMovementStatus(&gObjectEvents[gPlayerAvatar.objectEventId]) == TRUE)
    {
        FieldEffectStart(FLDEFF_FIELD_MOVE_SHOW_MON_INIT);
        gTasks[taskId].func = Task_DoFieldMove_WaitForMon;
    }
}

static void Task_DoFieldMove_WaitForMon(u8 taskId)
{
    if (!FieldEffectActiveListContains(FLDEFF_FIELD_MOVE_SHOW_MON))
    {
        gFieldEffectArguments[1] = GetPlayerFacingDirection();
        if (gFieldEffectArguments[1] == DIR_SOUTH)
            gFieldEffectArguments[2] = 0;
        if (gFieldEffectArguments[1] == DIR_NORTH)
            gFieldEffectArguments[2] = 1;
        if (gFieldEffectArguments[1] == DIR_WEST)
            gFieldEffectArguments[2] = 2;
        if (gFieldEffectArguments[1] == DIR_EAST)
            gFieldEffectArguments[2] = 3;
        ObjectEventSetGraphicsId(&gObjectEvents[gPlayerAvatar.objectEventId], GetPlayerAvatarGraphicsIdByCurrentState());
        StartSpriteAnim(&gSprites[gPlayerAvatar.spriteId], gFieldEffectArguments[2]);
        FieldEffectActiveListRemove(FLDEFF_FIELD_MOVE_SHOW_MON);
        gTasks[taskId].func = Task_DoFieldMove_RunFunc;
    }
}

static void Task_DoFieldMove_RunFunc(u8 taskId)
{
    void (*fieldMoveFunc)(void) = (void (*)(void))(((u16)gTasks[taskId].data[8] << 16) | (u16)gTasks[taskId].data[9]);

    fieldMoveFunc();
    gPlayerAvatar.preventStep = FALSE;
    DestroyTask(taskId);
}

#if DEBUG
void Debug_UseRockSmashInFrontOfPlayer(void)
{
    if (CheckObjectGraphicsInFrontOfPlayer(OBJ_EVENT_GFX_BREAKABLE_ROCK) == TRUE)
    {
        gLastFieldPokeMenuOpened = 0;
        FieldCallback_RockSmash();
    }
    else
    {
        UnlockPlayerFieldControls();
    }
}
#endif

bool8 SetUpFieldMove_RockSmash(void)
{
    if (CheckObjectGraphicsInFrontOfPlayer(OBJ_EVENT_GFX_BREAKABLE_ROCK) == TRUE)
    {
        gFieldCallback = FieldCallback_PrepareFadeInFromMenu;
        gPostMenuFieldCallback = FieldCallback_RockSmash;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static void FieldCallback_RockSmash(void)
{
    gFieldEffectArguments[0] = gLastFieldPokeMenuOpened;
    ScriptContext_SetupScript(EventScript_UseRockSmash);
}

bool8 FldEff_UseRockSmash(void)
{
    u8 taskId = CreateFieldMoveTask();

    gTasks[taskId].data[8] = (u32)FieldMove_RockSmash >> 16;
    gTasks[taskId].data[9] = (u32)FieldMove_RockSmash;
    IncrementGameStat(GAME_STAT_USED_ROCK_SMASH);
    return FALSE;
}

static void FieldMove_RockSmash(void)
{
    PlaySE(SE_M_ROCK_THROW);
    FieldEffectActiveListRemove(FLDEFF_USE_ROCK_SMASH);
    ScriptContext_Enable();
}
