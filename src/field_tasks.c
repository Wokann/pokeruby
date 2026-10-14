#include "global.h"
#include "task.h"
#include "main.h"
#include "constants/vars.h"
#include "bike.h"
#include "item.h"
#include "constants/items.h"
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
    DummyPerStepCallback,
    AshGrassPerStepCallback,
    FortreeBridgePerStepCallback,
    PacifidlogBridgePerStepCallback,
    SootopolisGymIcePerStepCallback,
    EndTruckSequence,
    SecretBasePerStepCallback,
    CrackedFloorPerStepCallback
};

static void Task_RunPerStepCallback(u8 taskId)
{
    int idx = gTasks[taskId].data[0];
    sPerStepCallbacks[idx](taskId);
}

#define tState           data[0]
#define tAmbientCryState data[1]
#define tAmbientCryDelay data[2]

static void RunTimeBasedEvents(s16 *data)
{
    switch (tState)
    {
        case 0:
            if (gMain.vblankCounter1 & 0x1000)
            {
                DoTimeBasedEvents();
                tState++;
            }
            break;
        case 1:
            if (!(gMain.vblankCounter1 & 0x1000))
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
#undef tAmbientCryState
#undef tAmbientCryDelay

static void Task_MuddySlope(u8);

void SetUpFieldTasks(void)
{
    if (!FuncIsActiveTask(Task_RunPerStepCallback))
    {
        u8 taskId = CreateTask(Task_RunPerStepCallback, 0x50);
        gTasks[taskId].data[0] = 0;
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
    if (taskId != 0xff)
    {
        dataStart = gTasks[taskId].data;
        zero = 0;
        dataPointer = &dataStart[15];
        do
        {
            *dataPointer-- = zero;
        } while ((int)dataPointer >= (int)dataStart);
        if (callbackId >= ARRAY_COUNT(sPerStepCallbacks))
        {
            *dataStart = 0;
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
    s16 *taskData;
    taskId = FindTaskIdByFunc(Task_RunPerStepCallback);
    if (taskId != 0xff)
    {
        taskData = gTasks[taskId].data;
    }
    taskId = FindTaskIdByFunc(Task_RunTimeBasedEvents);
    if (taskId != 0xff)
    {
        taskData = gTasks[taskId].data;
        taskData[1] = 0;
        taskData[2] = 0;
    }
}

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

static void PacifidlogBridgePerStepCallback(u8 taskId)
{
    s16 *data;
    s16 x, y;
    data = gTasks[taskId].data;
    PlayerGetDestCoords(&x, &y);
    switch (data[1])
    {
        case 0:
            data[2] = x;
            data[3] = y;
            TrySetLogBridgeFullySubmerged(x, y, TRUE);
            data[1] = 1;
            break;
        case 1:
            if (x != data[2] || y != data[3])
            {
                if (ShouldRaisePacifidlogLogs(x, y, data[2], data[3]))
                {
                    TrySetLogBridgeHalfSubmerged(data[2], data[3], TRUE);
                    TrySetLogBridgeFloating(data[2], data[3], FALSE);
                    data[4] = data[2];
                    data[5] = data[3];
                    data[1] = 2;
                    data[6] = 8;
                }
                else
                {
                    data[4] = -1;
                    data[5] = -1;
                }
                if (ShouldSinkPacifidlogLogs(x, y, data[2], data[3]))
                {
                    TrySetLogBridgeHalfSubmerged(x, y, TRUE);
                    data[1] = 2;
                    data[6] = 8;
                }
                data[2] = x;
                data[3] = y;
                if (MetatileBehavior_IsPacifidlogLog(MapGridGetMetatileBehaviorAt(x, y)))
                {
                    PlaySE(SE_PUDDLE);
                }
            }
            break;
        case 2:
            if ((--data[6]) == 0)
            {
                TrySetLogBridgeFullySubmerged(x, y, TRUE);
                if (data[4] != -1 && data[5] != -1)
                {
                    TrySetLogBridgeFloating(data[4], data[5], TRUE);
                }
                data[1] = 1;
            }
            break;
    }
}

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

static void FortreeBridgePerStepCallback(u8 taskId)
{
    bool8 isFortreeBridgeCur;
    bool8 isFortreeBridgePrev;
    u8 elevation, onBridgeElevation;
    s16 x, y, prevX, prevY;
    s16 *data = gTasks[taskId].data;
    PlayerGetDestCoords(&x, &y);
    switch (data[1])
    {
        default:
            break;
        case 0:
            data[2] = x;
            data[3] = y;
            if (MetatileBehavior_IsFortreeBridge(MapGridGetMetatileBehaviorAt(x, y)))
            {
                TryLowerFortreeBridge(x, y);
                CurrentMapDrawMetatileAt(x, y);
            }
            data[1] = 1;
            break;
        case 1:
            prevX = data[2];
            prevY = data[3];
            if (x == prevX && y == prevY)
            {
                break;
            }
            isFortreeBridgeCur = MetatileBehavior_IsFortreeBridge(MapGridGetMetatileBehaviorAt(x, y));
            isFortreeBridgePrev = MetatileBehavior_IsFortreeBridge(MapGridGetMetatileBehaviorAt(prevX, prevY));
            elevation = PlayerGetElevation();
            onBridgeElevation = 0;
            if ((u8)(elevation & 1) == 0)
            {
                onBridgeElevation = 1;
            }
            if (onBridgeElevation && (isFortreeBridgeCur == 1 || isFortreeBridgePrev == 1))
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
            data[4] = prevX;
            data[5] = prevY;
            data[2] = x;
            data[3] = y;
            if (!isFortreeBridgePrev)
            {
                break;
            }
            data[6] = 16;
            data[1] = 2;
            // fallthrough
        case 2:
            data[6]--;
            prevX = data[4];
            prevY = data[5];
            switch (data[6] % 7)
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
            if (data[6] == 0)
            {
                data[1] = 1;
            }
            break;
    }
}

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

static bool32 CoordInIcePuzzleRegion(s16 x, s16 y)
{
    if ((u16)(x - 3) < 11 && (u16)(y - 6) < 14 && sSootopolisGymIceRowVars[y])
    {
        return TRUE;
    }
    return FALSE;
}

static void MarkIcePuzzleCoordVisited(s16 x, s16 y)
{
    if (CoordInIcePuzzleRegion(x, y))
    {
        *GetVarPointer(sSootopolisGymIceRowVars[y]) |= (1 << (x - 3));
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
    if (((1 << 16) << (x - 3)) & var)
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
                MapGridSetMetatileIdAt(x + 7, y + 7, METATILE_ID(SootopolisGym, Ice_Cracked));
            }
        }
    }
}

static void SootopolisGymIcePerStepCallback(u8 taskId)
{
    s16 x, y;
    u16 tileBehavior;
    u16 *iceStepCount;
    s16 *data = gTasks[taskId].data;
    switch (data[1])
    {
        case 0:
            PlayerGetDestCoords(&x, &y);
            data[2] = x;
            data[3] = y;
            data[1] = 1;
            break;
        case 1:
            PlayerGetDestCoords(&x, &y);
            if (x != data[2] || y != data[3])
            {
                data[2] = x;
                data[3] = y;
                tileBehavior = MapGridGetMetatileBehaviorAt(x, y);
                iceStepCount = GetVarPointer(VAR_ICE_STEP_COUNT);
                if (MetatileBehavior_IsThinIce(tileBehavior) == TRUE)
                {
                    (*iceStepCount)++;
                    data[6] = 4;
                    data[1] = 2;
                    data[4] = x;
                    data[5] = y;
                }
                else if (MetatileBehavior_IsCrackedIce(tileBehavior) == TRUE)
                {
                    *iceStepCount = 0;
                    data[6] = 4;
                    data[1] = 3;
                    data[4] = x;
                    data[5] = y;
                }
            }
            break;
        case 2:
            if (data[6] != 0)
            {
                data[6]--;
            }
            else
            {
                x = data[4];
                y = data[5];
                PlaySE(SE_ICE_CRACK);
                MapGridSetMetatileIdAt(x, y, METATILE_ID(SootopolisGym, Ice_Cracked));
                CurrentMapDrawMetatileAt(x, y);
                MarkIcePuzzleCoordVisited(x - 7, y - 7);
                data[1] = 1;
            }
            break;
        case 3:
            if (data[6] != 0)
            {
                data[6]--;
            }
            else
            {
                x = data[4];
                y = data[5];
                PlaySE(SE_ICE_BREAK);
                MapGridSetMetatileIdAt(x, y, METATILE_ID(SootopolisGym, Ice_Broken));
                CurrentMapDrawMetatileAt(x, y);
                data[1] = 1;
            }
            break;
    }
}

static void AshGrassPerStepCallback(u8 taskId)
{
    s16 x, y;
    u16 *ashGatherCount;
    s16 *data = gTasks[taskId].data;
    PlayerGetDestCoords(&x, &y);
    if (x != data[1] || y != data[2])
    {
        data[1] = x;
        data[2] = y;
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

static void SetCrackedFloorHoleMetatile(s16 x, s16 y)
{
    MapGridSetMetatileIdAt(x, y, MapGridGetMetatileIdAt(x, y) == 0x22f ? 0x206 : 0x237);
    CurrentMapDrawMetatileAt(x, y);
}

static void CrackedFloorPerStepCallback(u8 taskId)
{
    s16 x, y;
    u16 behavior;
    s16 *data = gTasks[taskId].data;
    PlayerGetDestCoords(&x, &y);
    behavior = MapGridGetMetatileBehaviorAt(x, y);
    if (data[4] != 0 && (--data[4]) == 0)
    {
        SetCrackedFloorHoleMetatile(data[5], data[6]);
    }
    if (data[7] != 0 && (--data[7]) == 0)
    {
        SetCrackedFloorHoleMetatile(data[8], data[9]);
    }
    if (MetatileBehavior_IsCrackedFloorHole(behavior))
    {
        VarSet(VAR_ICE_STEP_COUNT, 0); // this var does double duty
    }
    if ((x != data[2] || y != data[3]))
    {
        data[2] = x;
        data[3] = y;
        if (MetatileBehavior_IsCrackedFloor(behavior))
        {
            if (GetPlayerSpeed() != 4)
            {
                VarSet(VAR_ICE_STEP_COUNT, 0); // this var does double duty
            }
            if (data[4] == 0)
            {
                data[4] = 3;
                data[5] = x;
                data[6] = y;
            }
            else if (data[7] == 0)
            {
                data[7] = 3;
                data[8] = x;
                data[9] = y;
            }
        }
    }
}

static const u16 sMuddySlopeAnimationMetatiles[] = {
    METATILE_ID(General, MuddySlope_Frame0),
    METATILE_ID(General, MuddySlope_Frame3),
    METATILE_ID(General, MuddySlope_Frame2),
    METATILE_ID(General, MuddySlope_Frame1)
};

static void SetMuddySlopeAnimatedMetatile(s16 *counter, s16 x, s16 y)
{
    u16 metatileId;
    if (--(*counter) == 0)
        metatileId = METATILE_ID(General, MuddySlope_Frame0);
    else
        metatileId = sMuddySlopeAnimationMetatiles[*counter / 8];

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
    switch (data[1])
    {
        case 0:
            data[0] = mapId;
            data[2] = x;
            data[3] = y;
            data[1] = 1;
            data[4] = 0;
            data[7] = 0;
            data[10] = 0;
            data[13] = 0;
            break;
        case 1:
            if (data[2] != x || data[3] != y)
            {
                data[2] = x;
                data[3] = y;
                if (MetatileBehavior_IsMuddySlope(MapGridGetMetatileBehaviorAt(x, y)))
                {
                    for (i = 4; i < 14; i += 3)
                    {
                        if (data[i] == 0)
                        {
                            data[i] = 32;
                            data[i + 1] = x;
                            data[i + 2] = y;
                            break;
                        }
                    }
                }
            }
            break;
    }

    if (gCamera.active && mapId != data[0])
    {
        data[0] = mapId;
        cameraOffsetX = gCamera.x;
        cameraOffsetY = gCamera.y;
    }
    else
    {
        cameraOffsetX = 0;
        cameraOffsetY = 0;
    }

    for (i = 4; i < 14; i += 3)
    {
        if (data[i])
        {
            data[i + 1] -= cameraOffsetX;
            data[i + 2] -= cameraOffsetY;
            SetMuddySlopeAnimatedMetatile(&data[i], data[i + 1], data[i + 2]);
        }
    }
}
