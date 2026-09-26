#include "global.h"
#include "field_special_scene.h"
#include "event_data.h"
#include "field_camera.h"
#include "field_fadetransition.h"
#include "event_object_movement.h"
#include "field_specials.h"
#include "fieldmap.h"
#include "main.h"
#include "palette.h"
#include "overworld.h"
#include "script.h"
#include "script_movement.h"
#include "constants/songs.h"
#include "constants/metatile_labels.h"
#include "sound.h"
#include "sprite.h"
#include "task.h"
#include "constants/event_objects.h"

#define BOX1_X_OFFSET  3
#define BOX1_Y_OFFSET  3
#define BOX2_X_OFFSET  0
#define BOX2_Y_OFFSET -3
#define BOX3_X_OFFSET -3
#define BOX3_Y_OFFSET  0

#define SECONDS(value) ((signed) (60.0 * value + 0.5))

// TODO: Move somewhere else
enum
{
    STEP_17 = 0x17,
    STEP_18,
    STEP_END = 0xFE,
};

const u32 gObjectEventPic_MovingBox[] = INCBIN_U32("graphics/object_events/pics/misc/moving_box.4bpp");
const u16 gObjectEventPalette19[] = INCBIN_U16("graphics/object_events/palettes/19.gbapal");

static const s8 sTruckCamera_HorizontalTable[] =
{
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    2,
    2,
    2,
    2,
    2,
    2,
    -1,
    -1,
    -1,
    0,
};

static const u8 sSSTidalSailEastMovementScript[] =
{
    STEP_18,
    STEP_END,
};

static const u8 sSSTidalSailWestMovementScript[] =
{
    STEP_17,
    STEP_END,
};

// porthole states
enum
{
    INIT_PORTHOLE,
    IDLE_CHECK,
    EXECUTE_MOVEMENT,
    EXIT_PORTHOLE,
};

s16 GetTruckCameraBobbingY(int time)
{
    if (!(time % 120))
        return -1;
    else if ((time % 10) <= 4)
        return 1;

    return 0;
}

s16 GetTruckBoxYMovement(int time)
{
    if (!((time + 120) % 180))
        return -1;

    return 0;
}

#define tTimer data[0]
void Task_Truck1(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    s16 cameraYpan, cameraXpan = 0;
    s16 yBox1, yBox2, yBox3;

    yBox1 = GetTruckBoxYMovement(tTimer + 30) * 4; // top box.
    SetObjectEventSpritePosByLocalIdAndMap(LOCALID_TRUCK_BOX_TOP, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup, BOX1_X_OFFSET - cameraXpan, BOX1_Y_OFFSET + yBox1);
    yBox2 = GetTruckBoxYMovement(tTimer) * 2; // bottom left box.
    SetObjectEventSpritePosByLocalIdAndMap(LOCALID_TRUCK_BOX_BOTTOM_L, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup, BOX2_X_OFFSET - cameraXpan, BOX2_Y_OFFSET + yBox2);
    yBox3 = GetTruckBoxYMovement(tTimer) * 4; // bottom right box.
    SetObjectEventSpritePosByLocalIdAndMap(LOCALID_TRUCK_BOX_BOTTOM_R, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup, BOX3_X_OFFSET - cameraXpan, BOX3_Y_OFFSET + yBox3);

    if (++tTimer == SECONDS(500)) // this will never run
        tTimer = 0; // reset the timer if it gets stuck.

    // this also matches with directly calling GetTruckCameraBobbingY within SetCameraPanning, but this is consistent with a later function that requires a temp variable.
    cameraYpan = GetTruckCameraBobbingY(tTimer);
    SetCameraPanning(cameraXpan, cameraYpan);
}
#undef tTimer

#define tTimerHorizontal data[0]
#define tMoveStep        data[1]
#define tTimerVertical   data[2]
void Task_Truck2(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    s16 cameraYpan;
    s16 cameraXpan;
    s16 yBox1, yBox2, yBox3;

    tTimerHorizontal++;
    tTimerVertical++;

    if (tTimerHorizontal > 5)
    {
        tTimerHorizontal = 0;
        tMoveStep++;
    }
    if ((u16)tMoveStep == ARRAY_COUNT(sTruckCamera_HorizontalTable))
    {
        DestroyTask(taskId);
    }
    else
    {
        if (sTruckCamera_HorizontalTable[tMoveStep] == 2)
            gTasks[taskId].func = Task_Truck3;

        cameraXpan = sTruckCamera_HorizontalTable[tMoveStep];
        cameraYpan = GetTruckCameraBobbingY(tTimerVertical);
        SetCameraPanning(cameraXpan, cameraYpan);
        yBox1 = GetTruckBoxYMovement(tTimerVertical + 30) * 4;
        SetObjectEventSpritePosByLocalIdAndMap(LOCALID_TRUCK_BOX_TOP, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup, BOX1_X_OFFSET - cameraXpan, BOX1_Y_OFFSET + yBox1);
        yBox2 = GetTruckBoxYMovement(tTimerVertical) * 2;
        SetObjectEventSpritePosByLocalIdAndMap(LOCALID_TRUCK_BOX_BOTTOM_L, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup, BOX2_X_OFFSET - cameraXpan, BOX2_Y_OFFSET + yBox2);
        yBox3 = GetTruckBoxYMovement(tTimerVertical) * 4;
        SetObjectEventSpritePosByLocalIdAndMap(LOCALID_TRUCK_BOX_BOTTOM_R, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup, BOX3_X_OFFSET - cameraXpan, BOX3_Y_OFFSET + yBox3);
    }
}

void Task_Truck3(u8 taskId)
{
   s16 *data = gTasks[taskId].data;
   s16 cameraXpan;
   s16 cameraYpan;

   tTimerHorizontal++;

   if (tTimerHorizontal > 5)
   {
       tTimerHorizontal = 0;
       tMoveStep++;
   }

   if ((u16)tMoveStep == ARRAY_COUNT(sTruckCamera_HorizontalTable))
   {
       DestroyTask(taskId);
   }
   else
   {
       cameraXpan = sTruckCamera_HorizontalTable[tMoveStep];
       cameraYpan = 0;
       SetCameraPanning(cameraXpan, cameraYpan);
       SetObjectEventSpritePosByLocalIdAndMap(LOCALID_TRUCK_BOX_TOP, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup, BOX1_X_OFFSET - cameraXpan, BOX1_Y_OFFSET + cameraYpan);
       SetObjectEventSpritePosByLocalIdAndMap(LOCALID_TRUCK_BOX_BOTTOM_L, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup, BOX2_X_OFFSET - cameraXpan, BOX2_Y_OFFSET + cameraYpan);
       SetObjectEventSpritePosByLocalIdAndMap(LOCALID_TRUCK_BOX_BOTTOM_R, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup, BOX3_X_OFFSET - cameraXpan, BOX3_Y_OFFSET + cameraYpan);
   }
}
#undef tTimerHorizontal
#undef tMoveStep
#undef tTimerVertical

#define tState   data[0]
#define tTimer   data[1]
#define tTaskId1 data[2]
#define tTaskId2 data[3]
void Task_HandleTruckSequence(u8 taskId)
{
   s16 *data = gTasks[taskId].data;

    switch (tState)
    {
        /*
        Each case has a timer which is handled with data[1], incrementing
        until it reaches the if function's condition, which sets the next task up.
        */
    case 0:
        tTimer++;
        if (tTimer == SECONDS(1.5))
        {
            SetCameraPanningCallback(0);
            tTimer = 0; // reset the timer.
            tTaskId1 = CreateTask(Task_Truck1, 0xA);
            tState = 1; // run the next case.
            PlaySE(SE_TRUCK_MOVE);
        }
        break;
    case 1:
        tTimer++;
        if (tTimer == SECONDS(2.5))
        {
            pal_fill_black();
            tTimer = 0;
            tState = 2;
        }
        break;
    case 2:
        tTimer++;
        if (!gPaletteFade.active && tTimer > SECONDS(5))
        {
            tTimer = 0;
            DestroyTask(tTaskId1);
            tTaskId2 = CreateTask(Task_Truck2, 0xA);
            tState = 3;
            PlaySE(SE_TRUCK_STOP);
        }
        break;
    case 3:
        if (!gTasks[tTaskId2].isActive) // is Truck2 no longer active (is Truck3 active?)
        {
            InstallCameraPanAheadCallback();
            tTimer = 0;
            tState = 4;
        }
        break;
    case 4:
        tTimer++;
        if (tTimer == 90)
        {
            PlaySE(SE_TRUCK_UNLOAD);
            tTimer = 0;
            tState = 5;
        }
        break;
    case 5:
        tTimer++;
        if (tTimer == 120)
        {
            MapGridSetMetatileIdAt(11, 8, METATILE_ID(InsideOfTruck, ExitLight_Top));
            MapGridSetMetatileIdAt(11, 9, METATILE_ID(InsideOfTruck, ExitLight_Mid));
            MapGridSetMetatileIdAt(11, 10, METATILE_ID(InsideOfTruck, ExitLight_Bottom));
            DrawWholeMapView();
            PlaySE(SE_TRUCK_DOOR);
            DestroyTask(taskId);
            UnlockPlayerFieldControls();
        }
        break;
    }
}
#undef tState
#undef tTimer
#undef tTaskId1
#undef tTaskId2

void ExecuteTruckSequence(void)
{
    MapGridSetMetatileIdAt(11, 8, METATILE_ID(InsideOfTruck, DoorClosedFloor_Top));
    MapGridSetMetatileIdAt(11, 9, METATILE_ID(InsideOfTruck, DoorClosedFloor_Mid));
    MapGridSetMetatileIdAt(11, 10, METATILE_ID(InsideOfTruck, DoorClosedFloor_Bottom));
    DrawWholeMapView();
    LockPlayerFieldControls();
    CpuFastFill(0, gPlttBufferFaded, 0x400);
    CreateTask(Task_HandleTruckSequence, 0xA);
}

void EndTruckSequence(u8 taskId)
{
    if (!FuncIsActiveTask(Task_HandleTruckSequence))
    {
        SetObjectEventSpritePosByLocalIdAndMap(LOCALID_TRUCK_BOX_TOP, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup, BOX1_X_OFFSET, BOX1_Y_OFFSET);
        SetObjectEventSpritePosByLocalIdAndMap(LOCALID_TRUCK_BOX_BOTTOM_L, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup, BOX2_X_OFFSET, BOX2_Y_OFFSET);
        SetObjectEventSpritePosByLocalIdAndMap(LOCALID_TRUCK_BOX_BOTTOM_R, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup, BOX3_X_OFFSET, BOX3_Y_OFFSET);
    }
}

bool8 TrySetPortholeWarpDestination(void)
{
    s8 mapGroup, mapNum;
    s16 x, y;

    if (GetSSTidalLocation(&mapGroup, &mapNum, &x, &y))
    {
        return FALSE;
    }
    else
    {
        Overworld_SetWarpDestination(mapGroup, mapNum, -1, x, y);
        return TRUE;
    }
}

void Task_HandlePorthole(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    u16 *var = GetVarPointer(VAR_PORTHOLE_STATE);
    struct WarpData *location = &gSaveBlock1.location;

    switch (data[0])
    {
    case INIT_PORTHOLE: // finish fading before making porthole finish.
        if (!gPaletteFade.active)
        {
            data[1] = 0;
            data[0] = EXECUTE_MOVEMENT; // execute movement before checking if should be exited. strange?
        }
        break;
    case IDLE_CHECK: // idle and move.
        if (JOY_NEW(A_BUTTON))
            data[1] = 1;
        if (!ScriptMovement_IsObjectMovementFinished(LOCALID_PLAYER, location->mapNum, location->mapGroup))
            return;
        if (CountSSTidalStep(1) == TRUE)
        {
            if (*var == 2)
                *var = 9;
            else
                *var = 10;
            data[0] = 3;
            return;
        }
        data[0] = 2;
    case EXECUTE_MOVEMENT: // execute movement.
        if (data[1])
        {
            data[0] = EXIT_PORTHOLE; // exit porthole.
            return;
        }
        // run this once.
        if (*var == 2) // which direction?
        {
            ScriptMovement_StartObjectMovementScript(LOCALID_PLAYER, location->mapNum, location->mapGroup, sSSTidalSailEastMovementScript);
            data[0] = IDLE_CHECK; // run case 1.
        }
        else
        {
            ScriptMovement_StartObjectMovementScript(LOCALID_PLAYER, location->mapNum, location->mapGroup, sSSTidalSailWestMovementScript);
            data[0] = IDLE_CHECK; // run case 1.
        }
        break;
    case EXIT_PORTHOLE: // exit porthole.
        FlagClear(FLAG_DONT_TRANSITION_MUSIC);
        FlagClear(FLAG_HIDE_MAP_NAME_POPUP);
        copy_saved_warp2_bank_and_enter_x_to_warp1(0);
        DoDiveWarp();
        DestroyTask(taskId);
        break;
    }
}

static void ShowSSTidalWhileSailing(void)
{
    u8 spriteId = AddPseudoObjectEvent(0x8C, SpriteCallbackDummy, 112, 80, 0);

    gSprites[spriteId].coordOffsetEnabled = FALSE;

    if (VarGet(VAR_PORTHOLE_STATE) == 2)
    {
        StartSpriteAnim(&gSprites[spriteId], GetFaceDirectionAnimNum(4));
    }
    else
    {
        StartSpriteAnim(&gSprites[spriteId], GetFaceDirectionAnimNum(3));
    }
}

void FieldCB_ShowPortholeView(void)
{
    ShowSSTidalWhileSailing();
    gObjectEvents[gPlayerAvatar.objectEventId].invisible = TRUE;
    pal_fill_black();
    CreateTask(Task_HandlePorthole, 80);
    LockPlayerFieldControls();
}

void LookThroughPorthole(void)
{
    FlagSet(FLAG_SYS_CRUISE_MODE);
    FlagSet(FLAG_DONT_TRANSITION_MUSIC);
    FlagSet(FLAG_HIDE_MAP_NAME_POPUP);
    saved_warp2_set(0, gSaveBlock1.location.mapGroup, gSaveBlock1.location.mapNum, -1);
    TrySetPortholeWarpDestination();
    DoPortholeWarp();
}
