#include "global.h"
#include "task.h"
#include "main.h"
#include "constants/vars.h"
#include "bike.h"
#include "item.h"
#include "constants/items.h"
#include "constants/field_tasks.h"
#include "event_data.h"
#include "overworld.h"
#include "clock.h"
#include "script.h"
#include "field_special_scene.h"
#include "field_effect_helpers.h"
#include "secret_base.h"
#include "metatile_behavior.h"
#include "fieldmap.h"
#include "field_player_avatar.h"
#include "field_camera.h"
#include "constants/songs.h"
#include "constants/metatile_labels.h"
#include "sound.h"
#include "field_tasks.h"

struct PacifidlogMetatileOffsets
{
    s8 x;
    s8 y;
    u16 metatileId;
};

static void DummyPerStepCallback(u8);
static void AshGrassPerStepCallback(u8);
static void FortreeBridgePerStepCallback(u8);
static void PacifidlogBridgePerStepCallback(u8);
static void SootopolisGymIcePerStepCallback(u8);
static void CrackedFloorPerStepCallback(u8);

static const TaskFunc sPerStepCallbacks[] =
{
    [STEP_CB_DUMMY]             = DummyPerStepCallback,
    [STEP_CB_ASH]               = AshGrassPerStepCallback,
    [STEP_CB_FORTREE_BRIDGE]    = FortreeBridgePerStepCallback,
    [STEP_CB_PACIFIDLOG_BRIDGE] = PacifidlogBridgePerStepCallback,
    [STEP_CB_SOOTOPOLIS_ICE]    = SootopolisGymIcePerStepCallback,
    [STEP_CB_TRUCK]             = EndTruckSequence,
    [STEP_CB_SECRET_BASE]       = SecretBasePerStepCallback,
    [STEP_CB_CRACKED_FLOOR]     = CrackedFloorPerStepCallback
};

#define tCallbackId data[0]

static void Task_RunPerStepCallback(u8 taskId)
{
    int idx = gTasks[taskId].tCallbackId;
    sPerStepCallbacks[idx](taskId);
}

#define tState           data[0]
#define tAmbientCryState data[1]
#define tAmbientCryDelay data[2]

#define TIME_UPDATE_INTERVAL (1 << 12)

static void RunTimeBasedEvents(s16 *data)
{
    switch (tState)
    {
        case 0:
            if (gMain.vblankCounter1 & TIME_UPDATE_INTERVAL)
            {
                DoTimeBasedEvents();
                tState++;
            }
            break;
        case 1:
            if (!(gMain.vblankCounter1 & TIME_UPDATE_INTERVAL))
            {
                tState--;
            }
            break;
    }
}

static void Task_RunTimeBasedEvents(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    if (!ArePlayerFieldControlsLocked())
    {
        RunTimeBasedEvents(data);
        UpdateAmbientCry(&tAmbientCryState, (u16 *)&tAmbientCryDelay);
    }
}

#undef tState

static void Task_MuddySlope(u8);

void SetUpFieldTasks(void)
{
    if (!FuncIsActiveTask(Task_RunPerStepCallback))
    {
        u8 taskId = CreateTask(Task_RunPerStepCallback, 0x50);
        gTasks[taskId].tCallbackId = STEP_CB_DUMMY;
    }
    if (!FuncIsActiveTask(Task_MuddySlope))
    {
        CreateTask(Task_MuddySlope, 0x50);
    }
    if (!FuncIsActiveTask(Task_RunTimeBasedEvents))
    {
        CreateTask(Task_RunTimeBasedEvents, 0x50);
    }
}

void ActivatePerStepCallback(u8 callbackId)
{
    s16 *dataPointer;
    s16 *dataStart;
    s16 zero;
    u8 taskId = FindTaskIdByFunc(Task_RunPerStepCallback);
    if (taskId != TASK_NONE)
    {
        dataStart = gTasks[taskId].data;
        zero = 0;
        dataPointer = &dataStart[NUM_TASK_DATA - 1];
        do
        {
            *dataPointer-- = zero;
        } while ((int)dataPointer >= (int)dataStart);
        if (callbackId >= ARRAY_COUNT(sPerStepCallbacks))
        {
            *dataStart = STEP_CB_DUMMY;
        }
        else
        {
            *dataStart = callbackId;
        }
    }
}

void ResetFieldTasksArgs(void)
{
    u8 taskId;
    s16 *data;
    taskId = FindTaskIdByFunc(Task_RunPerStepCallback);
    if (taskId != TASK_NONE)
    {
        data = gTasks[taskId].data;
    }
    taskId = FindTaskIdByFunc(Task_RunTimeBasedEvents);
    if (taskId != TASK_NONE)
    {
        data = gTasks[taskId].data;
        tAmbientCryState = 0;
        tAmbientCryDelay = 0;
    }
}

#undef tAmbientCryState
#undef tAmbientCryDelay

static const struct PacifidlogMetatileOffsets sHalfSubmergedBridgeMetatileOffsets[] =
{
    { 0,  0, METATILE_ID(Pacifidlog, HalfSubmergedLogs_VerticalTop)}, {0, 1, METATILE_ID(Pacifidlog, HalfSubmergedLogs_VerticalBottom)},
    { 0, -1, METATILE_ID(Pacifidlog, HalfSubmergedLogs_VerticalTop)}, {0, 0, METATILE_ID(Pacifidlog, HalfSubmergedLogs_VerticalBottom)},
    { 0,  0, METATILE_ID(Pacifidlog, HalfSubmergedLogs_HorizontalLeft)}, {1, 0, METATILE_ID(Pacifidlog, HalfSubmergedLogs_HorizontalRight)},
    {-1,  0, METATILE_ID(Pacifidlog, HalfSubmergedLogs_HorizontalLeft)}, {0, 0, METATILE_ID(Pacifidlog, HalfSubmergedLogs_HorizontalRight)}
};

static const struct PacifidlogMetatileOffsets sFullySubmergedBridgeMetatileOffsets[] =
{
    { 0,  0, METATILE_ID(Pacifidlog, SubmergedLogs_VerticalTop)}, {0, 1, METATILE_ID(Pacifidlog, SubmergedLogs_VerticalBottom)},
    { 0, -1, METATILE_ID(Pacifidlog, SubmergedLogs_VerticalTop)}, {0, 0, METATILE_ID(Pacifidlog, SubmergedLogs_VerticalBottom)},
    { 0,  0, METATILE_ID(Pacifidlog, SubmergedLogs_HorizontalLeft)}, {1, 0, METATILE_ID(Pacifidlog, SubmergedLogs_HorizontalRight)},
    {-1,  0, METATILE_ID(Pacifidlog, SubmergedLogs_HorizontalLeft)}, {0, 0, METATILE_ID(Pacifidlog, SubmergedLogs_HorizontalRight)}
};

static const struct PacifidlogMetatileOffsets sFloatingBridgeMetatileOffsets[] =
{
    { 0,  0, METATILE_ID(Pacifidlog, FloatingLogs_VerticalTop)}, {0, 1, METATILE_ID(Pacifidlog, FloatingLogs_VerticalBottom)},
    { 0, -1, METATILE_ID(Pacifidlog, FloatingLogs_VerticalTop)}, {0, 0, METATILE_ID(Pacifidlog, FloatingLogs_VerticalBottom)},
    { 0,  0, METATILE_ID(Pacifidlog, FloatingLogs_HorizontalLeft)}, {1, 0, METATILE_ID(Pacifidlog, FloatingLogs_HorizontalRight)},
    {-1,  0, METATILE_ID(Pacifidlog, FloatingLogs_HorizontalLeft)}, {0, 0, METATILE_ID(Pacifidlog, FloatingLogs_HorizontalRight)}
};

static void DummyPerStepCallback(u8 taskId) {}

static const struct PacifidlogMetatileOffsets *GetPacifidlogBridgeMetatileOffsets(const struct PacifidlogMetatileOffsets *offsets, u16 metatileBehavior)
{
    if (MetatileBehavior_IsPacifidlogVerticalLogTop(metatileBehavior))
    {
        return &offsets[0 * 2];
    }
    else if (MetatileBehavior_IsPacifidlogVerticalLogBottom(metatileBehavior))
    {
        return &offsets[1 * 2];
    }
    else if (MetatileBehavior_IsPacifidlogHorizontalLogLeft(metatileBehavior))
    {
        return &offsets[2 * 2];
    }
    else if (MetatileBehavior_IsPacifidlogHorizontalLogRight(metatileBehavior))
    {
        return &offsets[3 * 2];
    }
    else
    {
        return NULL;
    }
}

static void TrySetPacifidlogBridgeMetatiles(const struct PacifidlogMetatileOffsets *offsets, s16 x, s16 y, bool32 redrawMap)
{
    offsets = GetPacifidlogBridgeMetatileOffsets(offsets, MapGridGetMetatileBehaviorAt(x, y));
    if (offsets)
    {
        MapGridSetMetatileIdAt(x + offsets[0].x, y + offsets[0].y, offsets[0].metatileId);
        if (redrawMap)
        {
            CurrentMapDrawMetatileAt(x + offsets[0].x, y + offsets[0].y);
        }
        MapGridSetMetatileIdAt(x + offsets[1].x, y + offsets[1].y, offsets[1].metatileId);
        if (redrawMap)
        {
            CurrentMapDrawMetatileAt(x + offsets[1].x, y + offsets[1].y);
        }
    }
}

static void TrySetLogBridgeHalfSubmerged(s16 x, s16 y, bool32 redrawMap)
{
    TrySetPacifidlogBridgeMetatiles(sHalfSubmergedBridgeMetatileOffsets, x, y, redrawMap);
}

static void TrySetLogBridgeFullySubmerged(s16 x, s16 y, bool32 redrawMap)
{
    TrySetPacifidlogBridgeMetatiles(sFullySubmergedBridgeMetatileOffsets, x, y, redrawMap);
}

static void TrySetLogBridgeFloating(s16 x, s16 y, bool32 redrawMap)
{
    TrySetPacifidlogBridgeMetatiles(sFloatingBridgeMetatileOffsets, x, y, redrawMap);
}

static bool32 ShouldRaisePacifidlogLogs(s16 newX, s16 newY, s16 oldX, s16 oldY)
{
    s8 oldBehavior = MapGridGetMetatileBehaviorAt(oldX, oldY);
    if (MetatileBehavior_IsPacifidlogVerticalLogTop(oldBehavior))
    {
        if (newY > oldY)
        {
            return FALSE;
        }
    }
    else if (MetatileBehavior_IsPacifidlogVerticalLogBottom(oldBehavior))
    {
        if (newY < oldY)
        {
            return FALSE;
        }
    }
    else if (MetatileBehavior_IsPacifidlogHorizontalLogLeft(oldBehavior))
    {
        if (newX > oldX)
        {
            return FALSE;
        }
    }
    else if (MetatileBehavior_IsPacifidlogHorizontalLogRight(oldBehavior))
    {
        if (newX < oldX)
        {
            return FALSE;
        }
    }
    return TRUE;
}

static bool32 ShouldSinkPacifidlogLogs(s16 newX, s16 newY, s16 oldX, s16 oldY)
{
    s8 newBehavior = MapGridGetMetatileBehaviorAt(newX, newY);
    if (MetatileBehavior_IsPacifidlogVerticalLogTop(newBehavior))
    {
        if (newY < oldY)
        {
            return FALSE;
        }
    }
    else if (MetatileBehavior_IsPacifidlogVerticalLogBottom(newBehavior))
    {
        if (newY > oldY)
        {
            return FALSE;
        }
    }
    else if (MetatileBehavior_IsPacifidlogHorizontalLogLeft(newBehavior))
    {
        if (newX < oldX)
        {
            return FALSE;
        }
    }
    else if (MetatileBehavior_IsPacifidlogHorizontalLogRight(newBehavior))
    {
        if (newX > oldX)
        {
            return FALSE;
        }
    }
    return TRUE;
}

#define tState    data[1]
#define tPrevX    data[2]
#define tPrevY    data[3]
#define tToRaiseX data[4]
#define tToRaiseY data[5]
#define tDelay    data[6]

static void PacifidlogBridgePerStepCallback(u8 taskId)
{
    s16 *data;
    s16 x, y;
    data = gTasks[taskId].data;
    PlayerGetDestCoords(&x, &y);
    switch (tState)
    {
        case 0:
            tPrevX = x;
            tPrevY = y;
            TrySetLogBridgeFullySubmerged(x, y, TRUE);
            tState = 1;
            break;
        case 1:
            if (x != tPrevX || y != tPrevY)
            {
                if (ShouldRaisePacifidlogLogs(x, y, tPrevX, tPrevY))
                {
                    TrySetLogBridgeHalfSubmerged(tPrevX, tPrevY, TRUE);
                    TrySetLogBridgeFloating(tPrevX, tPrevY, FALSE);
                    tToRaiseX = tPrevX;
                    tToRaiseY = tPrevY;
                    tState = 2;
                    tDelay = 8;
                }
                else
                {
                    tToRaiseX = -1;
                    tToRaiseY = -1;
                }
                if (ShouldSinkPacifidlogLogs(x, y, tPrevX, tPrevY))
                {
                    TrySetLogBridgeHalfSubmerged(x, y, TRUE);
                    tState = 2;
                    tDelay = 8;
                }
                tPrevX = x;
                tPrevY = y;
                if (MetatileBehavior_IsPacifidlogLog(MapGridGetMetatileBehaviorAt(x, y)))
                {
                    PlaySE(SE_PUDDLE);
                }
            }
            break;
        case 2:
            if ((--tDelay) == 0)
            {
                TrySetLogBridgeFullySubmerged(x, y, TRUE);
                if (tToRaiseX != -1 && tToRaiseY != -1)
                {
                    TrySetLogBridgeFloating(tToRaiseX, tToRaiseY, TRUE);
                }
                tState = 1;
            }
            break;
    }
}

#undef tState
#undef tPrevX
#undef tPrevY
#undef tToRaiseX
#undef tToRaiseY
#undef tDelay

static void TryLowerFortreeBridge(s16 x, s16 y)
{
    u8 elevation = PlayerGetElevation();
    if (!(elevation & 0x01))
    {
        switch (MapGridGetMetatileIdAt(x, y))
        {
            case METATILE_ID(Fortree, BridgeOverGrass_Raised):
                MapGridSetMetatileIdAt(x, y, METATILE_ID(Fortree, BridgeOverGrass_Lowered));
                break;
            case METATILE_ID(Fortree, BridgeOverTrees_Raised):
                MapGridSetMetatileIdAt(x, y, METATILE_ID(Fortree, BridgeOverTrees_Lowered));
                break;
        }
    }
}

static void TryRaiseFortreeBridge(s16 x, s16 y)
{
    u8 elevation = PlayerGetElevation();
    if (!(elevation & 0x01))
    {
        switch (MapGridGetMetatileIdAt(x, y))
        {
            case METATILE_ID(Fortree, BridgeOverGrass_Lowered):
                MapGridSetMetatileIdAt(x, y, METATILE_ID(Fortree, BridgeOverGrass_Raised));
                break;
            case METATILE_ID(Fortree, BridgeOverTrees_Lowered):
                MapGridSetMetatileIdAt(x, y, METATILE_ID(Fortree, BridgeOverTrees_Raised));
                break;
        }
    }
}

#define tState      data[1]
#define tPrevX      data[2]
#define tPrevY      data[3]
#define tOldBridgeX data[4]
#define tOldBridgeY data[5]
#define tBounceTime data[6]

static void FortreeBridgePerStepCallback(u8 taskId)
{
    bool8 isFortreeBridgeCur;
    bool8 isFortreeBridgePrev;
    u8 elevation, onBridgeElevation;
    s16 x, y, prevX, prevY;
    s16 *data = gTasks[taskId].data;
    PlayerGetDestCoords(&x, &y);
    switch (tState)
    {
        default:
            break;
        case 0:
            tPrevX = x;
            tPrevY = y;
            if (MetatileBehavior_IsFortreeBridge(MapGridGetMetatileBehaviorAt(x, y)))
            {
                TryLowerFortreeBridge(x, y);
                CurrentMapDrawMetatileAt(x, y);
            }
            tState = 1;
            break;
        case 1:
            prevX = tPrevX;
            prevY = tPrevY;
            if (x == prevX && y == prevY)
            {
                break;
            }
            isFortreeBridgeCur = MetatileBehavior_IsFortreeBridge(MapGridGetMetatileBehaviorAt(x, y));
            isFortreeBridgePrev = MetatileBehavior_IsFortreeBridge(MapGridGetMetatileBehaviorAt(prevX, prevY));
            elevation = PlayerGetElevation();
            onBridgeElevation = FALSE;
            if ((u8)(elevation & 1) == 0)
            {
                onBridgeElevation = TRUE;
            }
            if (onBridgeElevation && (isFortreeBridgeCur == TRUE || isFortreeBridgePrev == TRUE))
            {
                PlaySE(SE_BRIDGE_WALK);
            }
            if (isFortreeBridgePrev)
            {
                TryRaiseFortreeBridge(prevX, prevY);
                CurrentMapDrawMetatileAt(prevX, prevY);
                TryLowerFortreeBridge(x, y);
                CurrentMapDrawMetatileAt(x, y);
            }
            tOldBridgeX = prevX;
            tOldBridgeY = prevY;
            tPrevX = x;
            tPrevY = y;
            if (!isFortreeBridgePrev)
            {
                break;
            }
            tBounceTime = 16;
            tState = 2;
            // fallthrough
        case 2:
            tBounceTime--;
            prevX = tOldBridgeX;
            prevY = tOldBridgeY;
            switch (tBounceTime % 7)
            {
                case 0:
                    CurrentMapDrawMetatileAt(prevX, prevY);
                case 1:
                case 2:
                case 3:
                    break;
                case 4:
                    TryLowerFortreeBridge(prevX, prevY);
                    CurrentMapDrawMetatileAt(prevX, prevY);
                    TryRaiseFortreeBridge(prevX, prevY);
                case 5:
                case 6:
                case 7:
                    break;
            }
            if (tBounceTime == 0)
            {
                tState = 1;
            }
            break;
    }
}

#undef tState
#undef tPrevX
#undef tPrevY
#undef tOldBridgeX
#undef tOldBridgeY
#undef tBounceTime

static const u16 sSootopolisGymIceRowVars[] =
{
    0,
    0,
    0,
    0,
    0,
    0,
    VAR_TEMP_1,
    VAR_TEMP_2,
    VAR_TEMP_3,
    VAR_TEMP_4,
    0,
    0,
    VAR_TEMP_5,
    VAR_TEMP_6,
    VAR_TEMP_7,
    0,
    0,
    VAR_TEMP_8,
    VAR_TEMP_9,
    VAR_TEMP_A,
    0,
    0,
    0,
    0,
    0,
    0
};

// Boundaries of the ice puzzle in Sootopolis Gym
#define ICE_PUZZLE_L 3
#define ICE_PUZZLE_R 13
#define ICE_PUZZLE_T 6
#define ICE_PUZZLE_B 19

#define ICE_PUZZLE_WIDTH  (ICE_PUZZLE_R - ICE_PUZZLE_L + 1)
#define ICE_PUZZLE_HEIGHT (ICE_PUZZLE_B - ICE_PUZZLE_T + 1)

static bool32 CoordInIcePuzzleRegion(s16 x, s16 y)
{
    if ((u16)(x - ICE_PUZZLE_L) < ICE_PUZZLE_WIDTH && (u16)(y - ICE_PUZZLE_T) < ICE_PUZZLE_HEIGHT && sSootopolisGymIceRowVars[y])
    {
        return TRUE;
    }
    return FALSE;
}

static void MarkIcePuzzleCoordVisited(s16 x, s16 y)
{
    if (CoordInIcePuzzleRegion(x, y))
    {
        *GetVarPointer(sSootopolisGymIceRowVars[y]) |= (1 << (x - ICE_PUZZLE_L));
    }
}

static bool32 IsIcePuzzleCoordVisited(s16 x, s16 y)
{
    u32 var;
    if (!CoordInIcePuzzleRegion(x, y))
    {
        return FALSE;
    }
    var = VarGet(sSootopolisGymIceRowVars[y]) << 16;
    if (((1 << 16) << (x - ICE_PUZZLE_L)) & var)
    {
        return TRUE;
    }
    return FALSE;
}

void SetSootopolisGymCrackedIceMetatiles(void)
{
    s32 x, y;
    s32 width = gMapHeader.mapLayout->width;
    s32 height = gMapHeader.mapLayout->height;
    for (x = 0; x < width; x++)
    {
        for (y = 0; y < height; y++)
        {
            if (IsIcePuzzleCoordVisited(x, y) == TRUE)
            {
                MapGridSetMetatileIdAt(x + MAP_OFFSET, y + MAP_OFFSET, METATILE_ID(SootopolisGym, Ice_Cracked));
            }
        }
    }
}

#define tState data[1]
#define tPrevX data[2]
#define tPrevY data[3]
#define tIceX  data[4]
#define tIceY  data[5]
#define tDelay data[6]

static void SootopolisGymIcePerStepCallback(u8 taskId)
{
    s16 x, y;
    u16 tileBehavior;
    u16 *iceStepCount;
    s16 *data = gTasks[taskId].data;
    switch (tState)
    {
        case 0:
            PlayerGetDestCoords(&x, &y);
            tPrevX = x;
            tPrevY = y;
            tState = 1;
            break;
        case 1:
            PlayerGetDestCoords(&x, &y);
            if (x != tPrevX || y != tPrevY)
            {
                tPrevX = x;
                tPrevY = y;
                tileBehavior = MapGridGetMetatileBehaviorAt(x, y);
                iceStepCount = GetVarPointer(VAR_ICE_STEP_COUNT);
                if (MetatileBehavior_IsThinIce(tileBehavior) == TRUE)
                {
                    (*iceStepCount)++;
                    tDelay = 4;
                    tState = 2;
                    tIceX = x;
                    tIceY = y;
                }
                else if (MetatileBehavior_IsCrackedIce(tileBehavior) == TRUE)
                {
                    *iceStepCount = 0;
                    tDelay = 4;
                    tState = 3;
                    tIceX = x;
                    tIceY = y;
                }
            }
            break;
        case 2:
            if (tDelay != 0)
            {
                tDelay--;
            }
            else
            {
                x = tIceX;
                y = tIceY;
                PlaySE(SE_ICE_CRACK);
                MapGridSetMetatileIdAt(x, y, METATILE_ID(SootopolisGym, Ice_Cracked));
                CurrentMapDrawMetatileAt(x, y);
                MarkIcePuzzleCoordVisited(x - MAP_OFFSET, y - MAP_OFFSET);
                tState = 1;
            }
            break;
        case 3:
            if (tDelay != 0)
            {
                tDelay--;
            }
            else
            {
                x = tIceX;
                y = tIceY;
                PlaySE(SE_ICE_BREAK);
                MapGridSetMetatileIdAt(x, y, METATILE_ID(SootopolisGym, Ice_Broken));
                CurrentMapDrawMetatileAt(x, y);
                tState = 1;
            }
            break;
    }
}

#undef tState
#undef tPrevX
#undef tPrevY
#undef tIceX
#undef tIceY
#undef tDelay

#define tPrevX data[1]
#define tPrevY data[2]

static void AshGrassPerStepCallback(u8 taskId)
{
    s16 x, y;
    u16 *ashGatherCount;
    s16 *data = gTasks[taskId].data;
    PlayerGetDestCoords(&x, &y);
    if (x != tPrevX || y != tPrevY)
    {
        tPrevX = x;
        tPrevY = y;
        if (MetatileBehavior_IsAshGrass(MapGridGetMetatileBehaviorAt(x, y)))
        {
            if (MapGridGetMetatileIdAt(x, y) == METATILE_ID(Fallarbor, AshGrass))
            {
                StartAshFieldEffect(x, y, METATILE_ID(Fallarbor, NormalGrass), 4);
            }
            else
            {
                StartAshFieldEffect(x, y, METATILE_ID(Lavaridge, NormalGrass), 4);
            }
            if (CheckBagHasItem(ITEM_SOOT_SACK, 1))
            {
                ashGatherCount = GetVarPointer(VAR_ASH_GATHER_COUNT);
                if (*ashGatherCount < 9999)
                {
                    (*ashGatherCount)++;
                }
            }
        }
    }
}

#undef tPrevX
#undef tPrevY

static void SetCrackedFloorHoleMetatile(s16 x, s16 y)
{
    MapGridSetMetatileIdAt(x, y, MapGridGetMetatileIdAt(x, y) == METATILE_ID(Cave, CrackedFloor) ? METATILE_ID(Cave, CrackedFloor_Hole) : METATILE_ID(Pacifidlog, SkyPillar_CrackedFloor_Hole));
    CurrentMapDrawMetatileAt(x, y);
}

#define tPrevX       data[2]
#define tPrevY       data[3]
#define tFloor1Delay data[4]
#define tFloor1X     data[5]
#define tFloor1Y     data[6]
#define tFloor2Delay data[7]
#define tFloor2X     data[8]
#define tFloor2Y     data[9]

static void CrackedFloorPerStepCallback(u8 taskId)
{
    s16 x, y;
    u16 behavior;
    s16 *data = gTasks[taskId].data;
    PlayerGetDestCoords(&x, &y);
    behavior = MapGridGetMetatileBehaviorAt(x, y);
    if (tFloor1Delay != 0 && (--tFloor1Delay) == 0)
    {
        SetCrackedFloorHoleMetatile(tFloor1X, tFloor1Y);
    }
    if (tFloor2Delay != 0 && (--tFloor2Delay) == 0)
    {
        SetCrackedFloorHoleMetatile(tFloor2X, tFloor2Y);
    }
    if (MetatileBehavior_IsCrackedFloorHole(behavior))
    {
        VarSet(VAR_ICE_STEP_COUNT, 0); // this var does double duty
    }
    if ((x != tPrevX || y != tPrevY))
    {
        tPrevX = x;
        tPrevY = y;
        if (MetatileBehavior_IsCrackedFloor(behavior))
        {
            if (GetPlayerSpeed() != PLAYER_SPEED_FASTEST)
            {
                VarSet(VAR_ICE_STEP_COUNT, 0); // this var does double duty
            }
            if (tFloor1Delay == 0)
            {
                tFloor1Delay = 3;
                tFloor1X = x;
                tFloor1Y = y;
            }
            else if (tFloor2Delay == 0)
            {
                tFloor2Delay = 3;
                tFloor2X = x;
                tFloor2Y = y;
            }
        }
    }
}

#undef tPrevX
#undef tPrevY
#undef tFloor1Delay
#undef tFloor1X
#undef tFloor1Y
#undef tFloor2Delay
#undef tFloor2X
#undef tFloor2Y

#define tMapId data[0]
#define tState data[1]
#define tPrevX data[2]
#define tPrevY data[3]

#define SLOPE_DATA_START 4
#define SLOPE_DATA_END  (3 * SLOPE_DATA_SIZE + SLOPE_DATA_START) // 13, which is the last slope data start point
enum {
    SLOPE_TIME,
    SLOPE_X,
    SLOPE_Y,
    SLOPE_DATA_SIZE
};
#define tSlopeAnimTime(i) data[(i) * SLOPE_DATA_SIZE + SLOPE_DATA_START + SLOPE_TIME]

static const u16 sMuddySlopeAnimationMetatiles[] = {
    METATILE_ID(General, MuddySlope_Frame0),
    METATILE_ID(General, MuddySlope_Frame3),
    METATILE_ID(General, MuddySlope_Frame2),
    METATILE_ID(General, MuddySlope_Frame1)
};

#define SLOPE_ANIM_TIME 32
#define SLOPE_ANIM_STEP_TIME (SLOPE_ANIM_TIME / (int)ARRAY_COUNT(sMuddySlopeAnimationMetatiles))

static void SetMuddySlopeAnimatedMetatile(s16 *counter, s16 x, s16 y)
{
    u16 metatileId;
    if (--(counter[SLOPE_TIME]) == 0)
        metatileId = METATILE_ID(General, MuddySlope_Frame0);
    else
        metatileId = sMuddySlopeAnimationMetatiles[counter[SLOPE_TIME] / SLOPE_ANIM_STEP_TIME];

    MapGridSetMetatileIdAt(x, y, metatileId);
    CurrentMapDrawMetatileAt(x, y);

    // Immediately set the metatile back to the original muddy slope metatile
    // but don't actualy draw it on the screen. This is so the underlying metatile
    // behvior on the map is not changed.
    MapGridSetMetatileIdAt(x, y, METATILE_ID(General, MuddySlope_Frame0));
}

// Checks for the player traversing on muddy slope metatiles.
// When the player walks or slides on one, it executes a short animation to
// make it look like a small mudslide. A maximum of 4 mudslide animations can
// exist simultaneously.
static void Task_MuddySlope(u8 taskId)
{
    s16 x, y, cameraOffsetX, cameraOffsetY;
    int i;
    u16 mapId;
    s16 *data = gTasks[taskId].data;
    PlayerGetDestCoords(&x, &y);
    mapId = (gSaveBlock1.location.mapGroup << 8) | gSaveBlock1.location.mapNum;
    switch (tState)
    {
        case 0:
            tMapId = mapId;
            tPrevX = x;
            tPrevY = y;
            tState = 1;
            tSlopeAnimTime(0) = 0;
            tSlopeAnimTime(1) = 0;
            tSlopeAnimTime(2) = 0;
            tSlopeAnimTime(3) = 0;
            break;
        case 1:
            if (tPrevX != x || tPrevY != y)
            {
                tPrevX = x;
                tPrevY = y;
                if (MetatileBehavior_IsMuddySlope(MapGridGetMetatileBehaviorAt(x, y)))
                {
                    for (i = SLOPE_DATA_START; i <= SLOPE_DATA_END; i += SLOPE_DATA_SIZE)
                    {
                        if (data[i] == 0)
                        {
                            data[i + SLOPE_TIME] = SLOPE_ANIM_TIME;
                            data[i + SLOPE_X] = x;
                            data[i + SLOPE_Y] = y;
                            break;
                        }
                    }
                }
            }
            break;
    }

    if (gCamera.active && mapId != tMapId)
    {
        tMapId = mapId;
        cameraOffsetX = gCamera.x;
        cameraOffsetY = gCamera.y;
    }
    else
    {
        cameraOffsetX = 0;
        cameraOffsetY = 0;
    }

    for (i = SLOPE_DATA_START; i <= SLOPE_DATA_END; i += SLOPE_DATA_SIZE)
    {
        if (data[i + SLOPE_TIME])
        {
            data[i + SLOPE_X] -= cameraOffsetX;
            data[i + SLOPE_Y] -= cameraOffsetY;
            SetMuddySlopeAnimatedMetatile(&data[i + SLOPE_TIME], data[i + SLOPE_X], data[i + SLOPE_Y]);
        }
    }
}
