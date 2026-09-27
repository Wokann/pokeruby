#include "global.h"
#include "cable_club.h"
#include "fieldmap.h"
#include "field_door.h"
#include "field_effect.h"
#include "field_fadetransition.h"
#include "event_object_movement.h"
#include "field_player_avatar.h"
#include "field_special_scene.h"
#include "field_weather.h"
#include "fldeff_flash.h"
#include "link.h"
#include "main.h"
#include "event_object_lock.h"
#include "metatile_behavior.h"
#include "palette.h"
#include "overworld.h"
#include "script.h"
#include "sound.h"
#include "start_menu.h"
#include "task.h"
#include "new_game.h"
#include "constants/event_object_movement.h"
#include "constants/event_objects.h"
#include "constants/songs.h"

void Task_ExitDoor(u8);
void Task_ExitNonAnimDoor(u8);
void Task_ExitNonDoor(u8);
void task0A_fade_n_map_maybe(u8);
void sub_808115C(u8);

void FillPalBufferWhite(void)
{
    CpuFastFill16(RGB_WHITE, gPlttBufferFaded, PLTT_SIZE);
}

void FillPalBufferBlack(void)
{
    CpuFastFill16(RGB_BLACK, gPlttBufferFaded, PLTT_SIZE);
}

void WarpFadeInScreen(void)
{
    u8 previousMapType = GetLastUsedWarpMapType();
    switch (GetMapPairFadeFromType(previousMapType, Overworld_GetMapTypeOfSaveblockLocation()))
    {
    case 0:
        FadeScreen(FADE_FROM_BLACK, 0);
        FillPalBufferBlack();
        break;
    case 1:
        FadeScreen(FADE_FROM_WHITE, 0);
        FillPalBufferWhite();
    }
}

void FadeInFromBlack(void)
{
    FadeScreen(FADE_FROM_BLACK, 0);
    FillPalBufferBlack();
}

void WarpFadeOutScreen(void)
{
    u8 currentMapType = Overworld_GetMapTypeOfSaveblockLocation();
    switch (GetMapPairFadeToType(currentMapType, GetDestinationWarpMapHeader()->mapType))
    {
    case 0:
        FadeScreen(FADE_TO_BLACK, 0);
        break;
    case 1:
        FadeScreen(FADE_TO_WHITE, 0);
    }
}

void SetPlayerVisibility(u8 arg)
{
    sub_8059B88(!arg);
}

void Task_WaitForFadeAndEnableScriptCtx(u8 taskID)
{
    if (WaitForWeatherFadeIn() == TRUE)
    {
        DestroyTask(taskID);
        ScriptContext_Enable();
    }
}

void FieldCB_ContinueScriptHandleMusic(void)
{
    LockPlayerFieldControls();
    Overworld_PlaySpecialMapMusic();
    FadeInFromBlack();
    CreateTask(Task_WaitForFadeAndEnableScriptCtx, 10);
}

void FieldCB_ContinueScript(void)
{
    LockPlayerFieldControls();
    FadeInFromBlack();
    CreateTask(Task_WaitForFadeAndEnableScriptCtx, 10);
}

void Task_ReturnToFieldCableLink(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->data[0])
    {
    case 0:
        task->data[1] = CreateTask_ReestablishCableClubLink();
        task->data[0]++;
        break;
    case 1:
        if (gTasks[task->data[1]].isActive != TRUE)
        {
            WarpFadeInScreen();
            task->data[0]++;
        }
        break;
    case 2:
        if (WaitForWeatherFadeIn() == TRUE)
        {
            UnlockPlayerFieldControls();
            DestroyTask(taskId);
        }
    }
}

void FieldCB_ReturnToFieldCableLink(void)
{
    LockPlayerFieldControls();
    Overworld_PlaySpecialMapMusic();
    FillPalBufferBlack();
    CreateTask(Task_ReturnToFieldCableLink, 10);
}

void Task_ReturnToFieldWirelessLink(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->data[0])
    {
    case 0:
        SetLinkStandbyCallback();
        task->data[0]++;
        break;
    case 1:
        if (IsLinkTaskFinished())
        {
            WarpFadeInScreen();
            task->data[0]++;
        }
        break;
    case 2:
        if (WaitForWeatherFadeIn() == TRUE)
        {
            StartSendingKeysToLink();
            UnlockPlayerFieldControls();
            DestroyTask(taskId);
        }
    }
}

void FieldCB_ReturnToFieldWirelessLink(void)
{
    LockPlayerFieldControls();
    Overworld_PlaySpecialMapMusic();
    FillPalBufferBlack();
    CreateTask(Task_ReturnToFieldWirelessLink, 10);
}

void SetUpWarpExitTask(void)
{
    s16 x, y;
    u8 behavior;
    TaskFunc func;
    PlayerGetDestCoords(&x, &y);
    behavior = MapGridGetMetatileBehaviorAt(x, y);
    if (MetatileBehavior_IsDoor(behavior) == TRUE)
        func = Task_ExitDoor;
    else if (MetatileBehavior_IsNonAnimDoor(behavior) == TRUE)
        func = Task_ExitNonAnimDoor;
    else
        func = Task_ExitNonDoor;
    CreateTask(func, 10);
}

void FieldCB_DefaultWarpExit(void)
{
    Overworld_PlaySpecialMapMusic();
    WarpFadeInScreen();
    SetUpWarpExitTask();
    LockPlayerFieldControls();
}

void FieldCB_WarpExitFadeFromBlack(void)
{
    Overworld_PlaySpecialMapMusic();
    FadeInFromBlack();
    SetUpWarpExitTask();
    LockPlayerFieldControls();
}

void sub_8080B78(void)
{
    Overworld_PlaySpecialMapMusic();
    WarpFadeInScreen();
    PlaySE(SE_WARP_OUT);
    CreateTask(Task_ExitNonDoor, 10);
    LockPlayerFieldControls();
}

void Task_ExitDoor(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    s16 *x = &task->data[2];
    s16 *y = &task->data[3];

    switch (task->data[0])
    {
    case 0:
        SetPlayerVisibility(0);
        FreezeObjectEvents();
        PlayerGetDestCoords(x, y);
        FieldSetDoorOpened(*x, *y);
        task->data[0] = 1;
        break;
    case 1:
        if (WaitForWeatherFadeIn())
        {
            u8 objEventId;
            SetPlayerVisibility(1);
            objEventId = GetObjectEventIdByLocalIdAndMap(LOCALID_PLAYER, 0, 0);
            ObjectEventSetHeldMovement(&gObjectEvents[objEventId], MOVEMENT_ACTION_WALK_NORMAL_DOWN);
            task->data[0] = 2;
        }
        break;
    case 2:
        if (IsPlayerStandingStill())
        {
            u8 objEventId;
            task->data[1] = FieldAnimateDoorClose(*x, *y);
            objEventId = GetObjectEventIdByLocalIdAndMap(LOCALID_PLAYER, 0, 0);
            ObjectEventClearHeldMovementIfFinished(&gObjectEvents[objEventId]);
            task->data[0] = 3;
        }
        break;
    case 3:
        if (task->data[1] < 0 || gTasks[task->data[1]].isActive != TRUE)
        {
            UnfreezeObjectEvents();
            task->data[0] = 4;
        }
        break;
    case 4:
        UnlockPlayerFieldControls();
        DestroyTask(taskId);
        break;
    }
}

void Task_ExitNonAnimDoor(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    s16 *x = &task->data[2];
    s16 *y = &task->data[3];

    switch (task->data[0])
    {
    case 0:
        SetPlayerVisibility(0);
        FreezeObjectEvents();
        PlayerGetDestCoords(x, y);
        task->data[0] = 1;
        break;
    case 1:
        if (WaitForWeatherFadeIn())
        {
            u8 objEventId;
            SetPlayerVisibility(1);
            objEventId = GetObjectEventIdByLocalIdAndMap(LOCALID_PLAYER, 0, 0);
            ObjectEventSetHeldMovement(&gObjectEvents[objEventId], GetWalkNormalMovementAction(GetPlayerFacingDirection()));
            task->data[0] = 2;
        }
        break;
    case 2:
        if (IsPlayerStandingStill())
        {
            UnfreezeObjectEvents();
            task->data[0] = 3;
        }
        break;
    case 3:
        UnlockPlayerFieldControls();
        DestroyTask(taskId);
        break;
    }
}

void Task_ExitNonDoor(u8 taskId)
{
    switch (gTasks[taskId].data[0])
    {
    case 0:
        FreezeObjectEvents();
        LockPlayerFieldControls();
        gTasks[taskId].data[0]++;
        break;
    case 1:
        if (WaitForWeatherFadeIn())
        {
            UnfreezeObjectEvents();
            UnlockPlayerFieldControls();
            DestroyTask(taskId);
        }
        break;
    }
}

void Task_WaitForFadeShowStartMenu(u8 taskId)
{
    if (WaitForWeatherFadeIn() == TRUE)
    {
        DestroyTask(taskId);
        CreateTask(sub_80712B4, 80);
    }
}

void ReturnToFieldOpenStartMenu(void)
{
    FadeInFromBlack();
    CreateStartMenuTask(Task_WaitForFadeShowStartMenu);
    LockPlayerFieldControls();
}

void Task_ReturnToFieldNoScript(u8 taskId)
{
    if (WaitForWeatherFadeIn() == 1)
    {
        UnlockPlayerFieldControls();
        DestroyTask(taskId);
        ScriptUnfreezeObjectEvents();
    }
}

void FieldCB_ReturnToFieldNoScript(void)
{
    LockPlayerFieldControls();
    FadeInFromBlack();
    CreateTask(Task_ReturnToFieldNoScript, 10);
}

void FieldCB_ReturnToFieldNoScriptCheckMusic(void)
{
    LockPlayerFieldControls();
    Overworld_PlaySpecialMapMusic();
    FadeInFromBlack();
    CreateTask(Task_ReturnToFieldNoScript, 10);
}

static bool32 PaletteFadeActive(void)
{
    return gPaletteFade.active;
}

bool32 WaitForWeatherFadeIn(void)
{
    if (IsWeatherNotFadingIn() == TRUE)
        return TRUE;
    else
        return FALSE;
}

void DoWarp(void)
{
    LockPlayerFieldControls();
    TryFadeOutOldMapMusic();
    WarpFadeOutScreen();
    PlayRainSoundEffect();
    PlaySE(SE_EXIT);
    gFieldCallback = FieldCB_DefaultWarpExit;
    CreateTask(task0A_fade_n_map_maybe, 10);
}

void DoDiveWarp(void)
{
    LockPlayerFieldControls();
    TryFadeOutOldMapMusic();
    WarpFadeOutScreen();
    PlayRainSoundEffect();
    gFieldCallback = FieldCB_DefaultWarpExit;
    CreateTask(task0A_fade_n_map_maybe, 10);
}

void DoDoorWarp(void)
{
    LockPlayerFieldControls();
    gFieldCallback = FieldCB_DefaultWarpExit;
    CreateTask(sub_808115C, 10);
}

void DoFallWarp(void)
{
    DoDiveWarp();
    gFieldCallback = FieldCB_FallWarpExit;
}

void sub_8080F2C(u8 metatileBehavior)
{
    LockPlayerFieldControls();
    sub_8086A2C(metatileBehavior, 10);
}

void sub_8080F48(void)
{
    LockPlayerFieldControls();
    StartLavaridgeGymB1FWarp(10);
}

void sub_8080F58(void)
{
    LockPlayerFieldControls();
    StartLavaridgeGym1FWarp(10);
}

void sub_8080F68(void)
{
    LockPlayerFieldControls();
    TryFadeOutOldMapMusic();
    WarpFadeOutScreen();
    PlaySE(SE_WARP_IN);
    CreateTask(task0A_fade_n_map_maybe, 10);
    gFieldCallback = sub_8080B78;
}

void DoPortholeWarp(void)
{
    LockPlayerFieldControls();
    WarpFadeOutScreen();
    CreateTask(task0A_fade_n_map_maybe, 10);
    gFieldCallback = FieldCB_ShowPortholeView;
}

static void WaitCableClubWarp(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->data[0])
    {
    case 0:
        LockPlayerFieldControls();
        task->data[0]++;
        break;
    case 1:
        if (!PaletteFadeActive() && BGMusicStopped())
            task->data[0]++;
        break;
    case 2:
        WarpIntoMap();
        SetMainCallback2(sub_8054588);
        DestroyTask(taskId);
        break;
    }
}

void DoCableClubWarp(void)
{
    LockPlayerFieldControls();
    TryFadeOutOldMapMusic();
    WarpFadeOutScreen();
    PlaySE(SE_EXIT);
    CreateTask(WaitCableClubWarp, 10);
}

void sub_8081050(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    switch (data[0])
    {
    case 0:
        ClearLinkCallback_2();
        FadeScreen(FADE_TO_BLACK, 0);
        TryFadeOutOldMapMusic();
        PlaySE(SE_EXIT);
        data[0]++;
        break;
    case 1:
        if (!PaletteFadeActive() && BGMusicStopped())
        {
            SetCloseLinkCallback();
            data[0]++;
        }
        break;
    case 2:
        if (!gReceivedRemoteLinkPlayers)
        {
            WarpIntoMap();
            SetMainCallback2(CB2_LoadMap);
            DestroyTask(taskId);
        }
        break;
    }
}

void ReturnFromLinkRoom(void)
{
    CreateTask(sub_8081050, 10);
}

#if DEBUG

void debug_sub_80888D8()
{
    debug_sub_8052E04();
    DoWarp();
    LockPlayerFieldControls();
/*    asm("\
    PUSH    {LR}\n\
    BL      debug_sub_8052E04\n\
    BL      DoWarp\n\
    BL      LockPlayerFieldControls\n\
    POP     {R0}\n\
    BX      R0");*/
}

#endif

void task0A_fade_n_map_maybe(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->data[0])
    {
    case 0:
        FreezeObjectEvents();
        LockPlayerFieldControls();
        task->data[0]++;
        break;
    case 1:
        if (!PaletteFadeActive() && BGMusicStopped())
        {
            task->data[0]++;
        }
        break;
    case 2:
        WarpIntoMap();
        SetMainCallback2(CB2_LoadMap);
        DestroyTask(taskId);
        break;
    }
}

void sub_808115C(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    s16 *x = &task->data[2];
    s16 *y = &task->data[3];

    switch (task->data[0])
    {
    case 0:
        FreezeObjectEvents();
        PlayerGetDestCoords(x, y);
        PlaySE(GetDoorSoundEffect(*x, *y - 1));
        task->data[1] = FieldAnimateDoorOpen(*x, *y - 1);
        task->data[0] = 1;
        break;
    case 1:
        if (task->data[1] < 0 || gTasks[task->data[1]].isActive != TRUE)
        {
            u8 objEventId;
            objEventId = GetObjectEventIdByLocalIdAndMap(LOCALID_PLAYER, 0, 0);
            ObjectEventClearHeldMovementIfActive(&gObjectEvents[objEventId]);
            objEventId = GetObjectEventIdByLocalIdAndMap(LOCALID_PLAYER, 0, 0);
            ObjectEventSetHeldMovement(&gObjectEvents[objEventId], MOVEMENT_ACTION_WALK_NORMAL_UP);
            task->data[0] = 2;
        }
        break;
    case 2:
        if (IsPlayerStandingStill())
        {
            u8 objEventId;
            task->data[1] = FieldAnimateDoorClose(*x, *y - 1);
            objEventId = GetObjectEventIdByLocalIdAndMap(LOCALID_PLAYER, 0, 0);
            ObjectEventClearHeldMovementIfFinished(&gObjectEvents[objEventId]);
            SetPlayerVisibility(0);
            task->data[0] = 3;
        }
        break;
    case 3:
        if (task->data[1] < 0 || gTasks[task->data[1]].isActive != TRUE)
        {
            task->data[0] = 4;
        }
        break;
    case 4:
        TryFadeOutOldMapMusic();
        WarpFadeOutScreen();
        PlayRainSoundEffect();
        task->data[0] = 0;
        task->func = task0A_fade_n_map_maybe;
        break;
    }
}

void Task_DoContestHallWarp(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->data[0])
    {
    case 0:
        FreezeObjectEvents();
        LockPlayerFieldControls();
        task->data[0]++;
        break;
    case 1:
        if (!PaletteFadeActive() && BGMusicStopped())
        {
            task->data[0]++;
        }
        break;
    case 2:
        WarpIntoMap();
        SetMainCallback2(CB2_ReturnToFieldContestHall);
        DestroyTask(taskId);
        break;
    }
}

void DoContestHallWarp(void)
{
    LockPlayerFieldControls();
    TryFadeOutOldMapMusic();
    WarpFadeOutScreen();
    PlayRainSoundEffect();
    PlaySE(SE_EXIT);
    gFieldCallback = FieldCB_WarpExitFadeFromBlack;
    CreateTask(Task_DoContestHallWarp, 10);
}
