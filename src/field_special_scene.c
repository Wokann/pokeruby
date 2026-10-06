#include "global.h"
#include "event_data.h"
#include "field_camera.h"
#include "field_screen_effect.h"
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

#define SECONDS(value) ((signed) (60.0 * value + 0.5))

// TODO: Move somewhere else
enum
{
    STEP_17 = 0x17,
    STEP_18,
    STEP_END = 0xFE,
};

const u32 gObjectEventPic_MovingBox[] = INCBIN_U32("graphics/object_events/pics/misc/moving_box.4bpp");
const u16 gObjectEventPal_MovingBox[] = INCBIN_U16("graphics/object_events/palettes/moving_box.gbapal");

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

static void Task_Truck3(u8);

static s16 GetTruckCameraBobbingY(int time)
{
    if (!(time % 120))
        return -1;
    else if ((time % 10) <= 4)
        return 1;

    return 0;
}

static s16 GetTruckBoxYMovement(int time) // for the box movement?
{
    if (!((time + 120) % 180))
        return -1;

    return 0;
}

static void Task_Truck1(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    s16 cameraYpan, cameraXpan = 0;
    s16 yBox1, yBox2, yBox3;

    yBox1 = GetTruckBoxYMovement(data[0] + 30) * 4; // top box.
    SetObjectEventSpritePosByLocalIdAndMap(LOCALID_TRUCK_BOX_TOP, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup, 3 - cameraXpan, yBox1 + 3);
    yBox2 = GetTruckBoxYMovement(data[0]) * 2; // bottom left box.
    SetObjectEventSpritePosByLocalIdAndMap(LOCALID_TRUCK_BOX_BOTTOM_L, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup, -cameraXpan, yBox2 - 3);
    yBox3 = GetTruckBoxYMovement(data[0]) * 4; // bottom right box.
    SetObjectEventSpritePosByLocalIdAndMap(LOCALID_TRUCK_BOX_BOTTOM_R, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup, -3 - cameraXpan, yBox3);

    if (++data[0] == SECONDS(500)) // this will never run
        data[0] = 0; // reset the timer if it gets stuck.

    // this also matches with directly calling GetTruckCameraBobbingY within SetCameraPanning, but this is consistent with a later function that requires a temp variable.
    cameraYpan = GetTruckCameraBobbingY(data[0]);
    SetCameraPanning(cameraXpan, cameraYpan);
}

static void Task_Truck2(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    s16 cameraYpan;
    s16 cameraXpan;
    s16 yBox1, yBox2, yBox3;

    data[0]++;
    data[2]++;

    if (data[0] > 5)
    {
        data[0] = 0;
        data[1]++;
    }
    if ((u16)data[1] == 19)
    {
        DestroyTask(taskId);
    }
    else
    {
        if (sTruckCamera_HorizontalTable[data[1]] == 2)
            gTasks[taskId].func = Task_Truck3;

        cameraXpan = sTruckCamera_HorizontalTable[data[1]];
        cameraYpan = GetTruckCameraBobbingY(data[2]);
        SetCameraPanning(cameraXpan, cameraYpan);
        yBox1 = GetTruckBoxYMovement(data[2] + 30) * 4;
        SetObjectEventSpritePosByLocalIdAndMap(LOCALID_TRUCK_BOX_TOP, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup, 3 - cameraXpan, yBox1 + 3);
        yBox2 = GetTruckBoxYMovement(data[2]) * 2;
        SetObjectEventSpritePosByLocalIdAndMap(LOCALID_TRUCK_BOX_BOTTOM_L, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup, -cameraXpan, yBox2 - 3);
        yBox3 = GetTruckBoxYMovement(data[2]) * 4;
        SetObjectEventSpritePosByLocalIdAndMap(LOCALID_TRUCK_BOX_BOTTOM_R, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup, -3 - cameraXpan, yBox3);
    }
}

static void Task_Truck3(u8 taskId)
{
   s16 *data = gTasks[taskId].data;
   s16 cameraXpan;
   s16 cameraYpan;

   data[0]++;

   if (data[0] > 5)
   {
       data[0] = 0;
       data[1]++;
   }

   if ((u16)data[1] == 19)
   {
       DestroyTask(taskId);
   }
   else
   {
       cameraXpan = sTruckCamera_HorizontalTable[data[1]];
       cameraYpan = 0;
       SetCameraPanning(cameraXpan, cameraYpan);
       SetObjectEventSpritePosByLocalIdAndMap(LOCALID_TRUCK_BOX_TOP, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup, 3 - cameraXpan, cameraYpan + 3);
       SetObjectEventSpritePosByLocalIdAndMap(LOCALID_TRUCK_BOX_BOTTOM_L, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup, -cameraXpan, cameraYpan - 3);
       SetObjectEventSpritePosByLocalIdAndMap(LOCALID_TRUCK_BOX_BOTTOM_R, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup, -3 - cameraXpan, cameraYpan);
   }
}

static void Task_HandleTruckSequence(u8 taskId)
{
   s16 *data = gTasks[taskId].data;

    switch (data[0])
    {
        /*
        Each case has a timer which is handled with data[1], incrementing
        until it reaches the if function's condition, which sets the next task up.
        */
    case 0:
        data[1]++;
        if (data[1] == SECONDS(1.5))
        {
            SetCameraPanningCallback(0);
            data[1] = 0; // reset the timer.
            data[2] = CreateTask(Task_Truck1, 0xA);
            data[0] = 1; // run the next case.
            PlaySE(SE_TRUCK_MOVE);
        }
        break;
    case 1:
        data[1]++;
        if (data[1] == SECONDS(2.5))
        {
            FadeInFromBlack();
            data[1] = 0;
            data[0] = 2;
        }
        break;
    case 2:
        data[1]++;
        if (!gPaletteFade.active && data[1] > SECONDS(5))
        {
            data[1] = 0;
            DestroyTask(data[2]);
            data[3] = CreateTask(Task_Truck2, 0xA);
            data[0] = 3;
            PlaySE(SE_TRUCK_STOP);
        }
        break;
    case 3:
        if (!gTasks[data[3]].isActive) // is Truck2 no longer active (is Truck3 active?)
        {
            InstallCameraPanAheadCallback();
            data[1] = 0;
            data[0] = 4;
        }
        break;
    case 4:
        data[1]++;
        if (data[1] == 90)
        {
            PlaySE(SE_TRUCK_UNLOAD);
            data[1] = 0;
            data[0] = 5;
        }
        break;
    case 5:
        data[1]++;
        if (data[1] == 120)
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
        SetObjectEventSpritePosByLocalIdAndMap(LOCALID_TRUCK_BOX_TOP, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup, 3, 3);
        SetObjectEventSpritePosByLocalIdAndMap(LOCALID_TRUCK_BOX_BOTTOM_L, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup, 0, -3);
        SetObjectEventSpritePosByLocalIdAndMap(LOCALID_TRUCK_BOX_BOTTOM_R, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup, -3, 0);
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
        SetWarpDestination(mapGroup, mapNum, -1, x, y);
        return TRUE;
    }
}

void Task_HandlePorthole(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    u16 *cruiseState = GetVarPointer(VAR_SS_TIDAL_STATE);
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
            if (*cruiseState == 2)
                *cruiseState = 9;
            else
                *cruiseState = 10;
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
        if (*cruiseState == 2) // which direction?
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
        SetWarpDestinationToDynamicWarp(0);
        DoDiveWarp();
        DestroyTask(taskId);
        break;
    }
}

static void ShowSSTidalWhileSailing(void)
{
    u8 spriteId = CreateObjectGraphicsSprite(0x8C, SpriteCallbackDummy, 112, 80, 0);

    gSprites[spriteId].coordOffsetEnabled = FALSE;

    if (VarGet(VAR_SS_TIDAL_STATE) == 2)
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
    FadeInFromBlack();
    CreateTask(Task_HandlePorthole, 80);
    LockPlayerFieldControls();
}

void LookThroughPorthole(void)
{
    FlagSet(FLAG_SYS_CRUISE_MODE);
    FlagSet(FLAG_DONT_TRANSITION_MUSIC);
    FlagSet(FLAG_HIDE_MAP_NAME_POPUP);
    SetDynamicWarp(0, gSaveBlock1.location.mapGroup, gSaveBlock1.location.mapNum, -1);
    TrySetPortholeWarpDestination();
    DoPortholeWarp();
}
