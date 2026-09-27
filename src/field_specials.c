#include "global.h"
#include "field_specials.h"
#include "diploma.h"
#include "fieldmap.h"
#include "event_data.h"
#include "battle_tower.h"
#include "event_object_movement.h"
#include "region_map.h"
#include "field_region_map.h"
#include "field_message_box.h"
#include "field_camera.h"
#include "field_player_avatar.h"
#include "main.h"
#include "constants/maps.h"
#include "overworld.h"
#include "script.h"
#include "constants/songs.h"
#include "string_util.h"
#include "strings.h"
#include "pokeblock.h"
#include "text.h"
#include "wallclock.h"
#include "tv.h"
#include "rtc.h"
#include "link.h"
#include "constants/songs.h"
#include "sound.h"
#include "menu.h"
#include "starter_choose.h"
#include "menu_helpers.h"
#include "battle_tower.h"
#include "field_weather.h"
#include "pokemon_summary_screen.h"
#include "random.h"
#include "constants/abilities.h"
#include "constants/event_object_movement.h"
#include "constants/event_objects.h"
#include "constants/field_specials.h"
#include "constants/metatile_labels.h"
#include "constants/moves.h"
#include "constants/species.h"
#include "constants/weather.h"

#if ENGLISH
#define CHAR_DECIMAL_SEPARATOR CHAR_PERIOD
#elif GERMAN
#define CHAR_DECIMAL_SEPARATOR CHAR_COMMA
#endif

extern struct WarpData gLastUsedWarp;
extern u8 gBattleOutcome;
extern u16 gSpecialVar_0x8004;
extern u16 gSpecialVar_0x8005;
extern u16 gSpecialVar_Result;

EWRAM_DATA bool8 gBikeCyclingChallenge = FALSE;
EWRAM_DATA u8 gBikeCollisions = 0;
static EWRAM_DATA u32 sBikeCyclingTimer = 0;
static EWRAM_DATA u8 sSlidingDoorNextFrameCounter = 0;
static EWRAM_DATA u8 sSlidingDoorFrame = 0;
EWRAM_DATA u8 gScrollableMenuItemCount = 0;
EWRAM_DATA u8 gScrollableMenuSelection = 0;
EWRAM_DATA u8 gScrollableMenuArrowFlags = 0;

static void RecordCyclingRoadResults(u32, u8);

static struct ElevatorMenu sElevatorMenuItems[20];

void Special_ShowDiploma(void)
{
    SetMainCallback2(CB2_ShowDiploma);
    LockPlayerFieldControls();
}

void Special_ViewWallClock(void)
{
    gMain.savedCallback = CB2_ReturnToField;
    SetMainCallback2(CB2_ViewWallClock);
    LockPlayerFieldControls();
}

void ResetCyclingRoadChallengeData(void)
{
    gBikeCyclingChallenge = FALSE;
    gBikeCollisions = 0;
    sBikeCyclingTimer = 0;
}

void Special_BeginCyclingRoadChallenge(void)
{
    gBikeCyclingChallenge = TRUE;
    gBikeCollisions = 0;
    sBikeCyclingTimer = gMain.vblankCounter1;
}

u16 GetPlayerAvatarBike(void)
{
    if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_ACRO_BIKE))
        return 1;
    if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_MACH_BIKE))
        return 2;
    return 0;
}

static void DetermineCyclingRoadResults(u32 numFrames, u8 numBikeCollisions)
{
    u8 result;

    if (numBikeCollisions <= 99)
    {
        ConvertIntToDecimalStringN(gStringVar1, numBikeCollisions, STR_CONV_MODE_LEFT_ALIGN, 2);
        StringAppend(gStringVar1, gOtherText_Times);
    }
    else
    {
        StringCopy(gStringVar1, gOtherText_99Times);
    }

    if (numFrames < 3600)
    {
        ConvertIntToDecimalStringN(gStringVar2, numFrames / 60, STR_CONV_MODE_RIGHT_ALIGN, 2);
        gStringVar2[2] = CHAR_DECIMAL_SEPARATOR;
        ConvertIntToDecimalStringN(&gStringVar2[3], ((numFrames % 60) * 100) / 60, STR_CONV_MODE_LEADING_ZEROS, 2);
        StringAppend(gStringVar2, gOtherText_Seconds);
    }
    else
    {
        StringCopy(gStringVar2, gOtherText_1Minute);
    }

    result = 0;
    if (numBikeCollisions == 0)
    {
        result = 5;
    }
    else if (numBikeCollisions < 4)
    {
        result = 4;
    }
    else if (numBikeCollisions < 10)
    {
        result = 3;
    }
    else if (numBikeCollisions < 20)
    {
        result = 2;
    }
    else if (numBikeCollisions < 100)
    {
        result = 1;
    }

    if (numFrames / 60 <= 10)
    {
        result += 5;
    }
    else if (numFrames / 60 <= 15)
    {
        result += 4;
    }
    else if (numFrames / 60 <= 20)
    {
        result += 3;
    }
    else if (numFrames / 60 <= 40)
    {
        result += 2;
    }
    else if (numFrames / 60 < 60)
    {
        result += 1;
    }


    gSpecialVar_Result = result;
}

void FinishCyclingRoadChallenge(void) {
    const u32 numFrames = gMain.vblankCounter1 - sBikeCyclingTimer;

    DetermineCyclingRoadResults(numFrames, gBikeCollisions);
    RecordCyclingRoadResults(numFrames, gBikeCollisions);
}

static void RecordCyclingRoadResults(u32 numFrames, u8 numBikeCollisions) {
    u16 low = VarGet(VAR_CYCLING_ROAD_RECORD_TIME_L);
    u16 high = VarGet(VAR_CYCLING_ROAD_RECORD_TIME_H);
    u32 framesRecord = low + (high << 16);

    if (framesRecord > numFrames || framesRecord == 0)
    {
        VarSet(VAR_CYCLING_ROAD_RECORD_TIME_L, numFrames);
        VarSet(VAR_CYCLING_ROAD_RECORD_TIME_H, numFrames >> 16);
        VarSet(VAR_CYCLING_ROAD_RECORD_COLLISIONS, numBikeCollisions);
    }
}

u16 GetRecordedCyclingRoadResults(void) {
    u16 low = VarGet(VAR_CYCLING_ROAD_RECORD_TIME_L);
    u16 high = VarGet(VAR_CYCLING_ROAD_RECORD_TIME_H);
    u32 framesRecord = low + (high << 16);

    if (framesRecord == 0)
    {
        return FALSE;
    }

    DetermineCyclingRoadResults(framesRecord, VarGet(VAR_CYCLING_ROAD_RECORD_COLLISIONS));
    return TRUE;
}

void UpdateCyclingRoadState(void) {
    if (gLastUsedWarp.mapNum == MAP_NUM(MAP_ROUTE110_SEASIDE_CYCLING_ROAD_NORTH_ENTRANCE) && gLastUsedWarp.mapGroup == MAP_GROUP(MAP_ROUTE110_SEASIDE_CYCLING_ROAD_NORTH_ENTRANCE))
    {
        return;
    }

    if (VarGet(VAR_CYCLING_CHALLENGE_STATE) == 2 || VarGet(VAR_CYCLING_CHALLENGE_STATE) == 3)
    {
        VarSet(VAR_CYCLING_CHALLENGE_STATE, 0);
        Overworld_SetSavedMusic(MUS_DUMMY);
    }
}

void SetSSTidalFlag(void)
{
    FlagSet(FLAG_SYS_CRUISE_MODE);
    *GetVarPointer(VAR_CRUISE_STEP_COUNT) = 0;
}

void ResetSSTidalFlag(void)
{
    FlagClear(FLAG_SYS_CRUISE_MODE);
}

bool32 CountSSTidalStep(u16 delta)
{
    if (!FlagGet(FLAG_SYS_CRUISE_MODE) || (*GetVarPointer(VAR_CRUISE_STEP_COUNT) += delta) <= 0xcc)
    {
        return FALSE;
    }
    return TRUE;
}

u8 GetSSTidalLocation(s8 *mapGroup, s8 *mapNum, s16 *x, s16 *y)
{
    u16 *varCruiseStepCount = GetVarPointer(VAR_CRUISE_STEP_COUNT);
    switch (*GetVarPointer(VAR_SS_TIDAL_STATE))
    {
        case SS_TIDAL_BOARD_SLATEPORT:
        case SS_TIDAL_LAND_SLATEPORT:
            return SS_TIDAL_LOCATION_SLATEPORT;
        case SS_TIDAL_HALFWAY_LILYCOVE:
        case SS_TIDAL_EXIT_CURRENTS_RIGHT:
            return SS_TIDAL_LOCATION_ROUTE131;
        case SS_TIDAL_LAND_LILYCOVE:
        case SS_TIDAL_BOARD_LILYCOVE:
            return SS_TIDAL_LOCATION_LILYCOVE;
        case SS_TIDAL_DEPART_LILYCOVE:
        case SS_TIDAL_EXIT_CURRENTS_LEFT:
            return SS_TIDAL_LOCATION_ROUTE124;
        case SS_TIDAL_DEPART_SLATEPORT:
            if (*varCruiseStepCount < 60)
            {
                *mapNum = MAP_NUM(MAP_ROUTE134);
                *x = *varCruiseStepCount + 19;
            }
            else if (*varCruiseStepCount < 140)
            {
                *mapNum = MAP_NUM(MAP_ROUTE133);
                *x = *varCruiseStepCount - 60;
            }
            else
            {
                *mapNum = MAP_NUM(MAP_ROUTE132);
                *x = *varCruiseStepCount - 140;
            }
            break;
        case SS_TIDAL_HALFWAY_SLATEPORT:
            if (*varCruiseStepCount < 66)
            {
                *mapNum = MAP_NUM(MAP_ROUTE132);
                *x = 65 - *varCruiseStepCount;
            }
            else if (*varCruiseStepCount < 146) {
                *mapNum = MAP_NUM(MAP_ROUTE133);
                *x = 145 - *varCruiseStepCount;
            }
            else
            {
                *mapNum = MAP_NUM(MAP_ROUTE134);
                *x = 224 - *varCruiseStepCount;
            }
            break;
    }
    *mapGroup = MAP_GROUP(MAP_ROUTE132);
    *y = 20;
    return SS_TIDAL_LOCATION_CURRENTS;
}

u8 GetLinkPartnerNames(void)
{
    u8 i;
    u8 j = 0;
    u8 myLinkPlayerNumber = GetSavedMultiplayerId();
    u8 nLinkPlayers = GetSavedPlayerCount();
    for (i=0; i<nLinkPlayers; i++)
    {
        if (myLinkPlayerNumber != i)
        {
            StringCopy(gTVStringVarPtrs[j], gLinkPlayers[i].name);
            j++;
        }
    }
    return nLinkPlayers;
}

void SpawnLinkPartnerObjectEvent(void)
{
    u8 j = 0;
    s16 x = 0;
    s16 y = 0;
    u8 movementTypes[] = {
        MOVEMENT_TYPE_FACE_UP,
        MOVEMENT_TYPE_FACE_LEFT,
        MOVEMENT_TYPE_FACE_DOWN,
        MOVEMENT_TYPE_FACE_RIGHT,
    };
    s8 coordOffsets[][2] = {
        { 0,  1},
        { 1,  0},
        { 0, -1},
        {-1,  0}
    };
    u8 myLinkPlayerNumber;
    u8 playerFacingDirection;
    u8 linkSpriteId;
    u8 i;

    myLinkPlayerNumber = GetSavedMultiplayerId();
    playerFacingDirection = GetPlayerFacingDirection();
    switch (playerFacingDirection)
    {
        case DIR_WEST:
            j = 2;
            x = gSaveBlock1.pos.x - 1;
            y = gSaveBlock1.pos.y;
            break;
        case DIR_NORTH:
            j = 1;
            x = gSaveBlock1.pos.x;
            y = gSaveBlock1.pos.y - 1;
            break;
        case DIR_EAST:
            x = gSaveBlock1.pos.x + 1;
            y = gSaveBlock1.pos.y;
            break;
        case DIR_SOUTH:
            j = 3;
            x = gSaveBlock1.pos.x;
            y = gSaveBlock1.pos.y + 1;
    }
    for (i=0; i<gSpecialVar_0x8004; i++)
    {
        if (myLinkPlayerNumber != i)
        {
            linkSpriteId = GetRivalAvatarGraphicsIdByStateIdAndGender(PLAYER_AVATAR_STATE_NORMAL, gLinkPlayers[i].gender);
            SpawnSpecialObjectEventParameterized(linkSpriteId, movementTypes[j], 0xf0 - i, coordOffsets[j][0] + x + 7, coordOffsets[j][1] + y + 7, 0);
            j++;
            if (j == 4)
            {
                j = 0;
            }
        }
    }
}

struct Coords8 {
    u8 x;
    u8 y;
};

const struct Coords8 sMauvilleGymSwitchCoords[3] = {
    { 7, 16},
    {15, 18},
    {11, 22}
};

void MauvilleGymPressSwitch(void)
{
    u8 i;
    for (i = 0; i < ARRAY_COUNT(sMauvilleGymSwitchCoords); i++)
    {
        if (i == gSpecialVar_0x8004)
        {
            MapGridSetMetatileIdAt(sMauvilleGymSwitchCoords[i].x, sMauvilleGymSwitchCoords[i].y, METATILE_ID(MauvilleGym, PressedSwitch));
        }
        else
        {
            MapGridSetMetatileIdAt(sMauvilleGymSwitchCoords[i].x, sMauvilleGymSwitchCoords[i].y, METATILE_ID(MauvilleGym, RaisedSwitch));
        }
    }
}

void MauvilleGymSetDefaultBarriers(void)
{
    int x, y;
    for (y = 12; y < 24; y++)
    {
        for (x = 7; x < 16; x++)
        {
            switch (MapGridGetMetatileIdAt(x, y))
            {
                case METATILE_ID(MauvilleGym, GreenBeamH1_On):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, GreenBeamH1_Off));
                    break;
                case METATILE_ID(MauvilleGym, GreenBeamH2_On):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, GreenBeamH2_Off));
                    break;
                case METATILE_ID(MauvilleGym, GreenBeamH3_On):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, GreenBeamH3_Off));
                    break;
                case METATILE_ID(MauvilleGym, GreenBeamH4_On):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, GreenBeamH4_Off));
                    break;
                case METATILE_ID(MauvilleGym, GreenBeamH1_Off):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, GreenBeamH1_On));
                    break;
                case METATILE_ID(MauvilleGym, GreenBeamH2_Off):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, GreenBeamH2_On));
                    break;
                case METATILE_ID(MauvilleGym, GreenBeamH3_Off):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, GreenBeamH3_On) | MAPGRID_COLLISION_MASK);
                    break;
                case METATILE_ID(MauvilleGym, GreenBeamH4_Off):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, GreenBeamH4_On) | MAPGRID_COLLISION_MASK);
                    break;
                case METATILE_ID(MauvilleGym, RedBeamH1_On):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, RedBeamH1_Off));
                    break;
                case METATILE_ID(MauvilleGym, RedBeamH2_On):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, RedBeamH2_Off));
                    break;
                case METATILE_ID(MauvilleGym, RedBeamH3_On):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, RedBeamH3_Off));
                    break;
                case METATILE_ID(MauvilleGym, RedBeamH4_On):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, RedBeamH4_Off));
                    break;
                case METATILE_ID(MauvilleGym, RedBeamH1_Off):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, RedBeamH1_On));
                    break;
                case METATILE_ID(MauvilleGym, RedBeamH2_Off):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, RedBeamH2_On));
                    break;
                case METATILE_ID(MauvilleGym, RedBeamH3_Off):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, RedBeamH3_On) | MAPGRID_COLLISION_MASK);
                    break;
                case METATILE_ID(MauvilleGym, RedBeamH4_Off):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, RedBeamH4_On) | MAPGRID_COLLISION_MASK);
                    break;
                case METATILE_ID(MauvilleGym, GreenBeamV1_On):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, PoleBottom_On) | MAPGRID_COLLISION_MASK);
                    break;
                case METATILE_ID(MauvilleGym, GreenBeamV2_On):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, FloorTile));
                    break;
                case METATILE_ID(MauvilleGym, RedBeamV1_On):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, PoleBottom_Off) | MAPGRID_COLLISION_MASK);
                    break;
                case METATILE_ID(MauvilleGym, RedBeamV2_On):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, FloorTile));
                    break;
                case METATILE_ID(MauvilleGym, PoleBottom_On):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, GreenBeamV1_On) | MAPGRID_COLLISION_MASK);
                    break;
                case METATILE_ID(MauvilleGym, FloorTile):
                    if (MapGridGetMetatileIdAt(x, y - 1) == METATILE_ID(MauvilleGym, GreenBeamV1_On))
                    {
                        MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, GreenBeamV2_On) | MAPGRID_COLLISION_MASK);
                    }
                    else
                    {
                        MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, RedBeamV2_On) | MAPGRID_COLLISION_MASK);
                    }
                    break;
                case METATILE_ID(MauvilleGym, PoleBottom_Off):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, RedBeamV1_On) | MAPGRID_COLLISION_MASK);
                    break;
                case METATILE_ID(MauvilleGym, PoleTop_Off):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, PoleTop_On) | MAPGRID_COLLISION_MASK);
                    break;
                case METATILE_ID(MauvilleGym, PoleTop_On):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, PoleTop_Off));
                    break;
            }
        }
    }
}

void MauvilleGymDeactivatePuzzle(void)
{
    int i, x, y;
    const struct Coords8 *switchCoords = sMauvilleGymSwitchCoords;
    for (i = ARRAY_COUNT(sMauvilleGymSwitchCoords) - 1; i >= 0; i--)
    {
        MapGridSetMetatileIdAt(switchCoords->x, switchCoords->y, METATILE_ID(MauvilleGym, PressedSwitch));
        switchCoords++;
    }
    for (y = 12; y < 24; y++)
    {
        for (x = 7; x < 16; x++)
        {
            switch (MapGridGetMetatileIdAt(x, y))
            {
                case METATILE_ID(MauvilleGym, GreenBeamH1_On):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, GreenBeamH1_Off));
                    break;
                case METATILE_ID(MauvilleGym, GreenBeamH2_On):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, GreenBeamH2_Off));
                    break;
                case METATILE_ID(MauvilleGym, GreenBeamH3_On):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, GreenBeamH3_Off));
                    break;
                case METATILE_ID(MauvilleGym, GreenBeamH4_On):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, GreenBeamH4_Off));
                    break;
                case METATILE_ID(MauvilleGym, RedBeamH1_On):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, RedBeamH1_Off));
                    break;
                case METATILE_ID(MauvilleGym, RedBeamH2_On):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, RedBeamH2_Off));
                    break;
                case METATILE_ID(MauvilleGym, RedBeamH3_On):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, RedBeamH3_Off));
                    break;
                case METATILE_ID(MauvilleGym, RedBeamH4_On):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, RedBeamH4_Off));
                    break;
                case METATILE_ID(MauvilleGym, GreenBeamV1_On):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, PoleBottom_On) | MAPGRID_COLLISION_MASK);
                    break;
                case METATILE_ID(MauvilleGym, RedBeamV1_On):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, PoleBottom_Off) | MAPGRID_COLLISION_MASK);
                    break;
                case METATILE_ID(MauvilleGym, GreenBeamV2_On):
                case METATILE_ID(MauvilleGym, RedBeamV2_On):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, FloorTile));
                    break;
                case METATILE_ID(MauvilleGym, PoleTop_On):
                    MapGridSetMetatileIdAt(x, y, METATILE_ID(MauvilleGym, PoleTop_Off));
                    break;
            }
        }
    }
}

static void Task_PetalburgGymSlideOpenRoomDoors(u8);
static void PetalburgGymSetDoorMetatiles(u8, u16);
static const bool8 sSlidingDoorNextFrameDelay[] = {0, 1, 1, 1, 1};

static const u16 sPetalburgGymSlidingDoorMetatiles[] = {
    METATILE_ID(PetalburgGym, SlidingDoor_Frame0),
    METATILE_ID(PetalburgGym, SlidingDoor_Frame1),
    METATILE_ID(PetalburgGym, SlidingDoor_Frame2),
    METATILE_ID(PetalburgGym, SlidingDoor_Frame3),
    METATILE_ID(PetalburgGym, SlidingDoor_Frame4),
};

void PetalburgGymSlideOpenRoomDoors(void)
{
    sSlidingDoorNextFrameCounter = 0;
    sSlidingDoorFrame = 0;
    PlaySE(SE_UNLOCK);
    CreateTask(Task_PetalburgGymSlideOpenRoomDoors, 8);
}

static void Task_PetalburgGymSlideOpenRoomDoors(u8 taskId)
{
    if (sSlidingDoorNextFrameDelay[sSlidingDoorFrame] == sSlidingDoorNextFrameCounter)
    {
        PetalburgGymSetDoorMetatiles(gSpecialVar_0x8004, sPetalburgGymSlidingDoorMetatiles[sSlidingDoorFrame]);
        sSlidingDoorNextFrameCounter = 0;
        if ((++sSlidingDoorFrame) == 5)
        {
            DestroyTask(taskId);
            ScriptContext_Enable();
        }
    }
    else
    {
        sSlidingDoorNextFrameCounter++;
    }
}

static void PetalburgGymSetDoorMetatiles(u8 roomNumber, u16 metatileId)
{
    u16 doorCoordsX[4];
    u16 doorCoordsY[4];
    u8 i;
    u8 nDoors = 0;
    switch (roomNumber)
    {
        case 1:
            nDoors = 2;
            doorCoordsX[0] = 1;
            doorCoordsX[1] = 7;
            doorCoordsY[0] = 0x68;
            doorCoordsY[1] = 0x68;
            break;
        case 2:
            nDoors = 2;
            doorCoordsX[0] = 1;
            doorCoordsX[1] = 7;
            doorCoordsY[0] = 0x4e;
            doorCoordsY[1] = 0x4e;
            break;
        case 3:
            nDoors = 2;
            doorCoordsX[0] = 1;
            doorCoordsX[1] = 7;
            doorCoordsY[0] = 0x5b;
            doorCoordsY[1] = 0x5b;
            break;
        case 4:
            nDoors = 1;
            doorCoordsX[0] = 7;
            doorCoordsY[0] = 0x27;
            break;
        case 5:
            nDoors = 2;
            doorCoordsX[0] = 1;
            doorCoordsX[1] = 7;
            doorCoordsY[0] = 0x34;
            doorCoordsY[1] = 0x34;
            break;
        case 6:
            nDoors = 1;
            doorCoordsX[0] = 1;
            doorCoordsY[0] = 0x41;
            break;
        case 7:
            nDoors = 1;
            doorCoordsX[0] = 7;
            doorCoordsY[0] = 0xd;
            break;
        case 8:
            nDoors = 1;
            doorCoordsX[0] = 1;
            doorCoordsY[0] = 0x1a;
            break;
    }

    for (i = 0; i < nDoors; i++)
    {
        MapGridSetMetatileIdAt(doorCoordsX[i] + 7, doorCoordsY[i] + 7, metatileId | 0xc00);
        MapGridSetMetatileIdAt(doorCoordsX[i] + 7, doorCoordsY[i] + 8, (metatileId + 8) | 0xc00);
    }

    DrawWholeMapView();
}

void PetalburgGymUnlockRoomDoors(void)
{
    PetalburgGymSetDoorMetatiles(gSpecialVar_0x8004, sPetalburgGymSlidingDoorMetatiles[4]);
}

void ShowFieldMessageStringVar4(void)
{
    ShowFieldMessage(gStringVar4);
}

void StorePlayerCoordsInVars(void)
{
    gSpecialVar_0x8004 = gSaveBlock1.pos.x;
    gSpecialVar_0x8005 = gSaveBlock1.pos.y;
}

u8 GetPlayerTrainerIdOnesDigit(void)
{
    return (u16)((gSaveBlock2.playerTrainerId[1] << 8) | gSaveBlock2.playerTrainerId[0]) % 10;
}

void GetPlayerBigGuyGirlString(void)
{
    if (gSaveBlock2.playerGender == MALE)
    {
        StringCopy(gStringVar1, gOtherText_BigGuy);
    }
    else
    {
        StringCopy(gStringVar1, gOtherText_BigGirl);
    }
}

void GetRivalSonDaughterString(void)
{
    if (gSaveBlock2.playerGender == MALE)
    {
        StringCopy(gStringVar1, gOtherText_Daughter);
    }
    else
    {
        StringCopy(gStringVar1, gOtherText_Son);
    }
}

u8 GetBattleOutcome(void)
{
    return gBattleOutcome;
}

void CableCarWarp(void)
{
    if (gSpecialVar_0x8004 != 0)
    {
        Overworld_SetWarpDestination(MAP_GROUP(MAP_ROUTE112_CABLE_CAR_STATION), MAP_NUM(MAP_ROUTE112_CABLE_CAR_STATION), -1, 6, 4);
    }
    else
    {
        Overworld_SetWarpDestination(MAP_GROUP(MAP_MT_CHIMNEY_CABLE_CAR_STATION), MAP_NUM(MAP_MT_CHIMNEY_CABLE_CAR_STATION), -1, 6, 4);
    }
}

void SetHiddenItemFlag(void)
{
    FlagSet(gSpecialVar_0x8004);
}

u16 GetWeekCount(void)
{
    u16 weekCount = gLocalTime.days / 7;
    if (weekCount > 9999)
    {
        weekCount = 9999;
    }
    return weekCount;
}

u8 GetLeadMonFriendshipScore(void)
{
    struct Pokemon *pokemon = &gPlayerParty[GetLeadMonIndex()];
    if (GetMonData(pokemon, MON_DATA_FRIENDSHIP) == 255)
    {
        return 6;
    }
    if (GetMonData(pokemon, MON_DATA_FRIENDSHIP) >= 200)
    {
        return 5;
    }
    if (GetMonData(pokemon, MON_DATA_FRIENDSHIP) >= 150)
    {
        return 4;
    }
    if (GetMonData(pokemon, MON_DATA_FRIENDSHIP) >= 100)
    {
        return 3;
    }
    if (GetMonData(pokemon, MON_DATA_FRIENDSHIP) >= 50)
    {
        return 2;
    }
    if (GetMonData(pokemon, MON_DATA_FRIENDSHIP) >= 1)
    {
        return 1;
    }
    return 0;
}

void CB2_FieldShowRegionMap(void)
{
    FieldInitRegionMap(CB2_ReturnToFieldContinueScriptPlayMapMusic);
}

void FieldShowRegionMap(void)
{
    SetMainCallback2(CB2_FieldShowRegionMap);
}

static void Task_PCTurnOnEffect(u8);
static void PCTurnOffEffect_0(struct Task *);
static void PCTurnOffEffect_1(s16, s8, s8);
static void PCTurnOffEffect(void);

void DoPCTurnOnEffect(void)
{
    if (FuncIsActiveTask(Task_PCTurnOnEffect) != TRUE)
    {
        u8 taskId = CreateTask(Task_PCTurnOnEffect, 8);
        gTasks[taskId].data[0] = 0;
        gTasks[taskId].data[1] = taskId;
        gTasks[taskId].data[2] = 0;
        gTasks[taskId].data[3] = 0;
        gTasks[taskId].data[4] = 0;
    }
}

static void Task_PCTurnOnEffect(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    if (task->data[0] == 0)
    {
        PCTurnOffEffect_0(task);
    }
}

static void PCTurnOffEffect_0(struct Task *task)
{
    u8 playerDirectionLowerNybble;
    s8 dx = 0;
    s8 dy = 0;
    if (task->data[3] == 6)
    {
        task->data[3] = 0;
        playerDirectionLowerNybble = GetPlayerFacingDirection();
        switch (playerDirectionLowerNybble)
        {
            case DIR_NORTH:
                dx = 0;
                dy = -1;
                break;
            case DIR_WEST:
                dx = -1;
                dy = -1;
                break;
            case DIR_EAST:
                dx = 1;
                dy = -1;
                break;
        }
        PCTurnOffEffect_1(task->data[4], dx, dy);
        DrawWholeMapView();
        task->data[4] ^= 1;
        if ((++task->data[2]) == 5)
        {
            DestroyTask(task->data[1]);
        }
    }
    task->data[3]++;
}

static void PCTurnOffEffect_1(s16 flag, s8 dx, s8 dy)
{
    u16 tileId = 0;
    if (flag != 0)
    {
        if (gSpecialVar_0x8004 == PC_LOCATION_OTHER)
        {
            tileId = METATILE_ID(Building, PC_Off);
        }
        else if (gSpecialVar_0x8004 == PC_LOCATION_BRENDANS_HOUSE)
        {
            tileId = METATILE_ID(BrendansMaysHouse, BrendanPC_Off);
        }
        else if (gSpecialVar_0x8004 == PC_LOCATION_MAYS_HOUSE)
        {
            tileId = METATILE_ID(BrendansMaysHouse, MayPC_Off);
        }
    }
    else
    {
        if (gSpecialVar_0x8004 == PC_LOCATION_OTHER)
        {
            tileId = METATILE_ID(Building, PC_On);
        }
        else if (gSpecialVar_0x8004 == PC_LOCATION_BRENDANS_HOUSE)
        {
            tileId = METATILE_ID(BrendansMaysHouse, BrendanPC_On);
        }
        else if (gSpecialVar_0x8004 == PC_LOCATION_MAYS_HOUSE)
        {
            tileId = METATILE_ID(BrendansMaysHouse, MayPC_On);
        }
    }
    MapGridSetMetatileIdAt(gSaveBlock1.pos.x + dx + 7, gSaveBlock1.pos.y + dy + 7, tileId | 0xc00);
}

void DoPCTurnOffEffect(void)
{
    PCTurnOffEffect();
}

static void PCTurnOffEffect(void)
{
    s8 dx = 0;
    s8 dy = 0;
    u16 tileId = 0;
    u8 playerDirectionLowerNybble = GetPlayerFacingDirection();
    switch (playerDirectionLowerNybble)
    {
        case DIR_NORTH:
            dx = 0;
            dy = -1;
            break;
        case DIR_WEST:
            dx = -1;
            dy = -1;
            break;
        case DIR_EAST:
            dx = 1;
            dy = -1;
            break;
    }
    if (gSpecialVar_0x8004 == PC_LOCATION_OTHER)
    {
        tileId = METATILE_ID(Building, PC_Off);
    }
    else if (gSpecialVar_0x8004 == PC_LOCATION_BRENDANS_HOUSE)
    {
        tileId = METATILE_ID(BrendansMaysHouse, BrendanPC_Off);
    }
    else if (gSpecialVar_0x8004 == PC_LOCATION_MAYS_HOUSE)
    {
        tileId = METATILE_ID(BrendansMaysHouse, MayPC_Off);
    }
    MapGridSetMetatileIdAt(gSaveBlock1.pos.x + dx + 7, gSaveBlock1.pos.y + dy + 7, tileId | MAPGRID_COLLISION_MASK);
    DrawWholeMapView();
}

static void Task_LotteryCornerComputerEffect(u8);
static void LotteryCornerComputerEffect(struct Task *);

void DoLotteryCornerComputerEffect(void)
{
    if (FuncIsActiveTask(Task_LotteryCornerComputerEffect) != TRUE)
    {
        u8 taskId = CreateTask(Task_LotteryCornerComputerEffect, 8);
        gTasks[taskId].data[0] = 0;
        gTasks[taskId].data[1] = taskId;
        gTasks[taskId].data[2] = 0;
        gTasks[taskId].data[3] = 0;
        gTasks[taskId].data[4] = 0;
    }
}

static void Task_LotteryCornerComputerEffect(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    if (task->data[0] == 0)
    {
        LotteryCornerComputerEffect(task);
    }
}

static void LotteryCornerComputerEffect(struct Task *task)
{
    if (task->data[3] == 6)
    {
        task->data[3] = 0;
        if (task->data[4] != 0)
        {
            MapGridSetMetatileIdAt(18, 8, METATILE_ID(Shop, Laptop1_Normal) | MAPGRID_COLLISION_MASK);
            MapGridSetMetatileIdAt(18, 9, METATILE_ID(Shop, Laptop2_Normal) | MAPGRID_COLLISION_MASK);
        }
        else
        {
            MapGridSetMetatileIdAt(18, 8, METATILE_ID(Shop, Laptop1_Flash) | MAPGRID_COLLISION_MASK);
            MapGridSetMetatileIdAt(18, 9, METATILE_ID(Shop, Laptop2_Flash) | MAPGRID_COLLISION_MASK);
        }
        DrawWholeMapView();
        task->data[4] ^= 1;
        if ((++task->data[2]) == 5)
        {
            DestroyTask(task->data[1]);
        }
    }
    task->data[3]++;
}

void EndLotteryCornerComputerEffect(void)
{
    MapGridSetMetatileIdAt(18, 8, METATILE_ID(Shop, Laptop1_Normal) | MAPGRID_COLLISION_MASK);
    MapGridSetMetatileIdAt(18, 9, METATILE_ID(Shop, Laptop2_Normal) | MAPGRID_COLLISION_MASK);
    DrawWholeMapView();
}

static void ShowElevatorMenu(void);
void ShowDeptStoreElevatorFloorSelect(void);
void Task_ElevatorMenu(u8);
bool8 ScrollElevatorMenu(u8, u8);
void UpdateElevatorMenuScrollIndicators(u8, u8);
void MoveElevator(void);
static void Task_MoveElevator(u8);
void FinishScrollableMenuTask(u8);
void CreateElevatorMenuTopArrow(void);
void CreateElevatorMenuBottomArrow(void);
static void MoveElevatorWindowLights(void);
static void Task_MoveElevatorWindowLights(u8);
static void UpdateElevatorWindowLights(struct Task *);
void CloseScrollableMenu(void);

static const u8 *const sDeptStoreFloorNames[] = {
    OtherText_1F,
    OtherText_2F,
    OtherText_3F,
    OtherText_4F,
    OtherText_5F,
    OtherText_6F,
    OtherText_7F,
    OtherText_8F,
    OtherText_9F,
    OtherText_10F,
    OtherText_11F,
    OtherText_B1F,
    OtherText_B2F,
    OtherText_B3F,
    OtherText_B4F,
    OtherText_Rooftop
};

void SetDeptStoreFloor(void)
{
    u8 deptStoreFloor;
    switch (gSaveBlock1.dynamicWarp.mapNum)
    {
        case MAP_NUM(MAP_LILYCOVE_CITY_DEPARTMENT_STORE_1F):
            deptStoreFloor = 0;
            break;
        case MAP_NUM(MAP_LILYCOVE_CITY_DEPARTMENT_STORE_2F):
            deptStoreFloor = 1;
            break;
        case MAP_NUM(MAP_LILYCOVE_CITY_DEPARTMENT_STORE_3F):
            deptStoreFloor = 2;
            break;
        case MAP_NUM(MAP_LILYCOVE_CITY_DEPARTMENT_STORE_4F):
            deptStoreFloor = 3;
            break;
        case MAP_NUM(MAP_LILYCOVE_CITY_DEPARTMENT_STORE_5F):
            deptStoreFloor = 4;
            break;
        case MAP_NUM(MAP_LILYCOVE_CITY_DEPARTMENT_STORE_ROOFTOP):
            deptStoreFloor = 15;
            break;
        default:
            deptStoreFloor = 0;
            break;
    }
    VarSet(VAR_DEPT_STORE_FLOOR, deptStoreFloor);
}

void ScriptAddElevatorMenuItem(u8 a0, u8 a1, u8 a2, u8 a3)
{
    u8 i;
    if (gSpecialVar_0x8004 == 0)
    {
        for (i = 0; i < 20; i++)
        {
            sElevatorMenuItems[i].var0 = 16;
        }
    }
    sElevatorMenuItems[gSpecialVar_0x8004].var0 = a0;
    sElevatorMenuItems[gSpecialVar_0x8004].var1 = a1;
    sElevatorMenuItems[gSpecialVar_0x8004].var2 = a2;
    sElevatorMenuItems[gSpecialVar_0x8004].var3 = a3;
    gSpecialVar_0x8004++;
}

void ScriptShowElevatorMenu(void)
{
    u8 i = 0;
    gScrollableMenuItemCount = 0;
    gScrollableMenuSelection = 0;
    ScriptAddElevatorMenuItem(16, 0, 0, 0);
    while (sElevatorMenuItems[i].var0 != 16)
    {
        gScrollableMenuItemCount++;
        i++;
    }
    ShowElevatorMenu();
}

static void ShowElevatorMenu(void)
{
    u8 i;
    LockPlayerFieldControls();
    if (gScrollableMenuItemCount > 5)
    {
        Menu_DrawStdWindowFrame(0, 0, 8, 11);
        InitMenu(0, 1, 1, 5, 0, 7);
        gScrollableMenuArrowFlags = 0;
        ClearVerticalScrollIndicatorPalettes();
        LoadScrollIndicatorPalette();
        CreateElevatorMenuBottomArrow();
    }
    else
    {
        Menu_DrawStdWindowFrame(0, 0, 8, 2 * gScrollableMenuItemCount + 1);
        InitMenu(0, 1, 1, gScrollableMenuItemCount, 0, 7);
    }
    for (i = 0; i < 5 && sElevatorMenuItems[i].var0 != 16; i ++)
    {
        Menu_PrintText(sDeptStoreFloorNames[sElevatorMenuItems[i].var0], 1, 2 * i + 1);
    }
    ShowDeptStoreElevatorFloorSelect();
    CreateTask(Task_ElevatorMenu, 8);
}

void ShowDeptStoreElevatorFloorSelect(void)
{
    Menu_DrawStdWindowFrame(20, 0, 29, 5);
    MenuPrint_Centered(gOtherText_NowOn, 21, 1, 64);
    MenuPrint_Centered(sDeptStoreFloorNames[gSpecialVar_0x8005], 21, 3, 64);
}

void Task_ElevatorMenu(u8 taskId)
{
    u8 curMenuPos;
    if (gMain.newKeys == DPAD_UP && gScrollableMenuSelection != 0)
    {
        gScrollableMenuSelection--;
        curMenuPos = Menu_GetCursorPos();
        Menu_MoveCursorNoWrap(-1);
        ScrollElevatorMenu(curMenuPos, DPAD_UP);
    }
    if (gMain.newKeys == DPAD_DOWN && gScrollableMenuSelection != gScrollableMenuItemCount - 1)
    {
        gScrollableMenuSelection++;
        curMenuPos = Menu_GetCursorPos();
        Menu_MoveCursorNoWrap(+1);
        ScrollElevatorMenu(curMenuPos, DPAD_DOWN);
    }
    if (JOY_NEW(A_BUTTON))
    {
        saved_warp2_set_2(0, sElevatorMenuItems[gScrollableMenuSelection].var1, sElevatorMenuItems[gScrollableMenuSelection].var2, -1, 2, 1);
        if (gSpecialVar_0x8005 == gScrollableMenuSelection)
        {
            gSpecialVar_Result = 0;
            PlaySE(SE_SELECT);
            Menu_EraseWindowRect(0, 0, 29, 12);
            FinishScrollableMenuTask(taskId);
        }
        else
        {
            gSpecialVar_Result = 1;
            gSpecialVar_0x8005 = gScrollableMenuSelection;
            MoveElevator();
            ObjectEventTurnByLocalIdAndMap(gSpecialVar_LastTalked, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup, DIR_SOUTH);
            CloseScrollableMenu();
            Menu_EraseScreen();
            DestroyTask(taskId);
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        gSpecialVar_Result = 0;
        PlaySE(SE_SELECT);
        CloseScrollableMenu();
        Menu_EraseWindowRect(0, 0, 29, 12);
        FinishScrollableMenuTask(taskId);
    }
}

/* Removing the NONMATCHING block will swap the roles of r4 and r5 throughout.
Could possibly be fixed by writing code which increases the amount of references to newPos,
or decreasing the amount of references to i.*/
bool8 ScrollElevatorMenu(u8 prevMenuPos, u8 dpadInput)
{
    u8 i;
    bool8 flag = 0;
    u8 newPos = 0;
    if (gScrollableMenuItemCount < 5)
    {
        return FALSE;
    }
    if (dpadInput == DPAD_UP)
    {
        if (prevMenuPos == 0)
        {
            newPos = gScrollableMenuSelection;
            flag = 1;
        }
    }
    else if (dpadInput == DPAD_DOWN)
    {
        if (prevMenuPos == 4)
        {
            newPos = gScrollableMenuSelection - 4;
            flag = 1;
        }
    }
    if (flag)
    {
        UpdateElevatorMenuScrollIndicators(newPos, 5);
        Menu_BlankWindowRect(2, 1, 7, 10);
        for (i=0; i<5 && sElevatorMenuItems[newPos].var0 != 16; newPos++, i++)
        {
            Menu_PrintText(sDeptStoreFloorNames[sElevatorMenuItems[newPos].var0], 1, i * 2 + 1);
#ifndef NONMATCHING
            asm(""::"r"(newPos));
            asm(""::"r"(newPos));
            asm(""::"r"(newPos));
#endif
        }
    }
    return flag;
}

void UpdateElevatorMenuScrollIndicators(u8 newPos, u8 maxItems)
{
    if (newPos == 0)
    {
        gScrollableMenuArrowFlags ^= 0x02;
        DestroyVerticalScrollIndicator(TOP_ARROW);
    }
    else
    {
        CreateElevatorMenuTopArrow();
    }
    if (newPos + maxItems < gScrollableMenuItemCount)
    {
        CreateElevatorMenuBottomArrow();
    }
    else if (newPos + maxItems == gScrollableMenuItemCount)
    {
        gScrollableMenuArrowFlags ^= 0x01;
        DestroyVerticalScrollIndicator(BOTTOM_ARROW);
    }
}

void MoveElevator(void)
{
    u8 taskId = CreateTask(Task_MoveElevator, 9);
    gTasks[taskId].data[0] = 1;
    gTasks[taskId].data[1] = 0;
    gTasks[taskId].data[2] = 0;
    gTasks[taskId].data[3] = 0;
    gTasks[taskId].data[4] = 1;
    gTasks[taskId].data[5] = 3;
    SetCameraPanningCallback(NULL);
    MoveElevatorWindowLights();
    PlaySE(SE_ELEVATOR);
}

static void Task_MoveElevator(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    task->data[1] ++;
    if (task->data[1] % task->data[5] == 0)
    {
        task->data[1] = 0;
        task->data[2] ++;
        if (task->data[3] == 0)
        {
            task->data[4] = -task->data[4];
            SetCameraPanning(0, task->data[4]);
            if (task->data[2] == 23)
            {
                PlaySE(SE_DING_DONG);
                FinishScrollableMenuTask(taskId);
                InstallCameraPanAheadCallback();
            }
        }
    }
}

void FinishScrollableMenuTask(u8 taskId)
{
    DestroyTask(taskId);
    ScriptContext_Enable();
}

void CreateElevatorMenuTopArrow(void)
{
    if (gScrollableMenuArrowFlags >> 1 != 1)
    {
        gScrollableMenuArrowFlags |= 0x2;
        CreateVerticalScrollIndicators(TOP_ARROW, 0x24, 0x08);
    }
}

void CreateElevatorMenuBottomArrow(void)
{
    if ((gScrollableMenuArrowFlags & 1) == 0)
    {
        gScrollableMenuArrowFlags |= 0x1;
        CreateVerticalScrollIndicators(BOTTOM_ARROW, 0x24, 0x48);
    }
}

static void MoveElevatorWindowLights(void)
{
    if (FuncIsActiveTask(Task_MoveElevatorWindowLights) != TRUE)
    {
        u8 taskId = CreateTask(Task_MoveElevatorWindowLights, 8);
        gTasks[taskId].data[0] = 0;
        gTasks[taskId].data[1] = taskId;
        gTasks[taskId].data[2] = 0;
        gTasks[taskId].data[3] = 0;
        gTasks[taskId].data[4] = 0;
    }
}

static void Task_MoveElevatorWindowLights(u8 taskId)
{
    UpdateElevatorWindowLights(&gTasks[taskId]);
}

static void UpdateElevatorWindowLights(struct Task *task)
{
    if (task->data[3] == 8)
    {
        task->data[3] = 0;
        if (task->data[4] != 0)
        {
            MapGridSetMetatileIdAt( 7,  7, METATILE_BattleTower_Elevator_Top0 | MAPGRID_COLLISION_MASK);
            MapGridSetMetatileIdAt( 8,  7, METATILE_BattleTower_Elevator_Top1 | MAPGRID_COLLISION_MASK);
            MapGridSetMetatileIdAt( 9,  7, METATILE_BattleTower_Elevator_Top1 | MAPGRID_COLLISION_MASK);
            MapGridSetMetatileIdAt(10,  7, METATILE_BattleTower_Elevator_Top2 | MAPGRID_COLLISION_MASK);
            MapGridSetMetatileIdAt( 7,  8, METATILE_BattleTower_Elevator_Mid0 | MAPGRID_COLLISION_MASK);
            MapGridSetMetatileIdAt( 8,  8, METATILE_BattleTower_Elevator_Mid1 | MAPGRID_COLLISION_MASK);
            MapGridSetMetatileIdAt( 9,  8, METATILE_BattleTower_Elevator_Mid1 | MAPGRID_COLLISION_MASK);
            MapGridSetMetatileIdAt(10,  8, METATILE_BattleTower_Elevator_Mid2 | MAPGRID_COLLISION_MASK);
            MapGridSetMetatileIdAt( 7,  9, METATILE_BattleTower_Elevator_Bottom0 | MAPGRID_COLLISION_MASK);
            MapGridSetMetatileIdAt( 8,  9, METATILE_BattleTower_Elevator_Bottom1 | MAPGRID_COLLISION_MASK);
            MapGridSetMetatileIdAt( 9,  9, METATILE_BattleTower_Elevator_Bottom1 | MAPGRID_COLLISION_MASK);
            MapGridSetMetatileIdAt(10,  9, METATILE_BattleTower_Elevator_Bottom2 | MAPGRID_COLLISION_MASK);
        }
        else
        {
            MapGridSetMetatileIdAt( 7,  7, (METATILE_BattleTower_Elevator_Top0 + 3) | MAPGRID_COLLISION_MASK);
            MapGridSetMetatileIdAt( 8,  7, (METATILE_BattleTower_Elevator_Top1 + 3) | MAPGRID_COLLISION_MASK);
            MapGridSetMetatileIdAt( 9,  7, (METATILE_BattleTower_Elevator_Top1 + 3) | MAPGRID_COLLISION_MASK);
            MapGridSetMetatileIdAt(10,  7, (METATILE_BattleTower_Elevator_Top2 + 3) | MAPGRID_COLLISION_MASK);
            MapGridSetMetatileIdAt( 7,  8, (METATILE_BattleTower_Elevator_Mid0 + 3) | MAPGRID_COLLISION_MASK);
            MapGridSetMetatileIdAt( 8,  8, (METATILE_BattleTower_Elevator_Mid1 + 3) | MAPGRID_COLLISION_MASK);
            MapGridSetMetatileIdAt( 9,  8, (METATILE_BattleTower_Elevator_Mid1 + 3) | MAPGRID_COLLISION_MASK);
            MapGridSetMetatileIdAt(10,  8, (METATILE_BattleTower_Elevator_Mid2 + 3) | MAPGRID_COLLISION_MASK);
            MapGridSetMetatileIdAt( 7,  9, (METATILE_BattleTower_Elevator_Bottom0 + 3) | MAPGRID_COLLISION_MASK);
            MapGridSetMetatileIdAt( 8,  9, (METATILE_BattleTower_Elevator_Bottom1 + 3) | MAPGRID_COLLISION_MASK);
            MapGridSetMetatileIdAt( 9,  9, (METATILE_BattleTower_Elevator_Bottom1 + 3) | MAPGRID_COLLISION_MASK);
            MapGridSetMetatileIdAt(10,  9, (METATILE_BattleTower_Elevator_Bottom2 + 3) | MAPGRID_COLLISION_MASK);
        }
        DrawWholeMapView();
        task->data[4] ^= 1;
        task->data[2]++;
        if (task->data[2] == 8)
        {
            DestroyTask(task->data[1]);
        }
    }
    task->data[3]++;
}

void CloseScrollableMenu(void)
{
    if ((gScrollableMenuArrowFlags & 1) != 0)
    {
        DestroyVerticalScrollIndicator(BOTTOM_ARROW);
    }
    if ((gScrollableMenuArrowFlags >> 1) == 1)
    {
        DestroyVerticalScrollIndicator(TOP_ARROW);
    }
    BuyMenuFreeMemory();
}

void SetTrickHouseNuggetFlag(void)
{
    u16 *specVar = &gSpecialVar_0x8004;
    u16 flag = FLAG_HIDDEN_ITEM_TRICK_HOUSE_NUGGET;
    *specVar = flag;
    FlagSet(flag);
}

void ResetTrickHouseNuggetFlag(void)
{
    u16 *specVar = &gSpecialVar_0x8004;
    u16 flag = FLAG_HIDDEN_ITEM_TRICK_HOUSE_NUGGET;
    *specVar = flag;
    FlagClear(flag);
}

bool8 CheckLeadMonCool(void)
{
    if (GetMonData(&gPlayerParty[GetLeadMonIndex()], MON_DATA_COOL) < 200)
    {
        return FALSE;
    }
    return TRUE;
}

bool8 CheckLeadMonBeauty(void)
{
    if (GetMonData(&gPlayerParty[GetLeadMonIndex()], MON_DATA_BEAUTY) < 200)
    {
        return FALSE;
    }
    return TRUE;
}

bool8 CheckLeadMonCute(void)
{
    if (GetMonData(&gPlayerParty[GetLeadMonIndex()], MON_DATA_CUTE) < 200)
    {
        return FALSE;
    }
    return TRUE;
}

bool8 CheckLeadMonSmart(void)
{
    if (GetMonData(&gPlayerParty[GetLeadMonIndex()], MON_DATA_SMART) < 200)
    {
        return FALSE;
    }
    return TRUE;
}

bool8 CheckLeadMonTough(void)
{
    if (GetMonData(&gPlayerParty[GetLeadMonIndex()], MON_DATA_TOUGH) < 200)
    {
        return FALSE;
    }
    return TRUE;
}

void IsGrassTypeInParty(void)
{
    u8 i;
    u16 species;
    struct Pokemon *pokemon;
    for (i=0; i<PARTY_SIZE; i++)
    {
        pokemon = &gPlayerParty[i];
        if (GetMonData(pokemon, MON_DATA_SANITY_BIT2) && !GetMonData(pokemon, MON_DATA_IS_EGG))
        {
            species = GetMonData(pokemon, MON_DATA_SPECIES);
            if (gBaseStats[species].type1 == TYPE_GRASS || gBaseStats[species].type2 == TYPE_GRASS)
            {
                gSpecialVar_Result = TRUE;
                return;
            }
        }
    }
    gSpecialVar_Result = FALSE;
}

const u8 *const sGlassWorkshopMenuItems[] = {
    OtherText_BlueFlute,
    OtherText_YellowFlute,
    OtherText_RedFlute,
    OtherText_WhiteFlute,
    OtherText_BlackFlute,
    OtherText_PrettyChair,
    OtherText_PrettyDesk,
    gOtherText_CancelNoTerminator
};

void Task_GlassWorkshopMenu(u8);
bool8 ScrollGlassWorkshopMenu(u8, u8);
void CreateGlassWorkshopBottomArrow(void);
void GlassWorkshopUpdateScrollIndicators(u8, u8);

void ShowGlassWorkshopMenu(void)
{
    u8 i;
    LockPlayerFieldControls();
    Menu_DrawStdWindowFrame(0, 0, 10, 11);
    InitMenu(0, 1, 1, 5, 0, 9);
    gScrollableMenuArrowFlags = 0;
    ClearVerticalScrollIndicatorPalettes();
    LoadScrollIndicatorPalette();
    CreateGlassWorkshopBottomArrow();
    for (i=0; i<5; i++)
    {
        Menu_PrintText(sGlassWorkshopMenuItems[i], 1, 2 * i + 1);
    }
    gScrollableMenuSelection = 0;
    gScrollableMenuItemCount = ARRAY_COUNT(sGlassWorkshopMenuItems);
    CreateTask(Task_GlassWorkshopMenu, 8);
}

void Task_GlassWorkshopMenu(u8 taskId)
{
    u8 prevCursorPos;
    if (gMain.newKeys == DPAD_UP && gScrollableMenuSelection != 0)
    {
        gScrollableMenuSelection--;
        prevCursorPos = Menu_GetCursorPos();
        Menu_MoveCursorNoWrap(-1);
        ScrollGlassWorkshopMenu(prevCursorPos, DPAD_UP);
    }
    if (gMain.newKeys == DPAD_DOWN && gScrollableMenuSelection != gScrollableMenuItemCount - 1)
    {
        gScrollableMenuSelection++;
        prevCursorPos = Menu_GetCursorPos();
        Menu_MoveCursorNoWrap(1);
        ScrollGlassWorkshopMenu(prevCursorPos, DPAD_DOWN);
    }
    if (JOY_NEW(A_BUTTON))
    {
        Menu_DestroyCursor();
        gSpecialVar_Result = gScrollableMenuSelection;
        PlaySE(SE_SELECT);
        CloseScrollableMenu();
        Menu_EraseWindowRect(0, 0, 29, 12);
        FinishScrollableMenuTask(taskId);
    }
    if (JOY_NEW(B_BUTTON))
    {
        Menu_DestroyCursor();
        gSpecialVar_Result = 0x7f;
        PlaySE(SE_SELECT);
        CloseScrollableMenu();
        Menu_EraseWindowRect(0, 0, 29, 12);
        FinishScrollableMenuTask(taskId);
    }
}

/* Removing the NONMATCHING block will swap the roles of r4 and r5 throughout.
Could possibly be fixed by writing code which increases the amount of references to newPos,
or decreasing the amount of references to i.*/
bool8 ScrollGlassWorkshopMenu(u8 prevCursorPos, u8 dpadInput)
{
    u8 i;
    u8 flag = 0;
    u8 newPos = 0;
    if (gScrollableMenuItemCount < 5)
    {
        return FALSE;
    }
    if (dpadInput == DPAD_UP)
    {
        if (prevCursorPos == 0)
        {
            newPos = gScrollableMenuSelection;
            flag = TRUE;
        }
    }
    else if (dpadInput == DPAD_DOWN)
    {
        if (prevCursorPos == 4)
        {
            newPos = gScrollableMenuSelection - 4;
            flag = TRUE;
        }
    }
    if (flag)
    {
        GlassWorkshopUpdateScrollIndicators(newPos, 5);
        Menu_BlankWindowRect(2, 1, 9, 10);
        for (i=0; i<5; newPos++, i++)
        {
            Menu_PrintText(sGlassWorkshopMenuItems[newPos], 1, 2 * i + 1);
#ifndef NONMATCHING
            asm(""::"r"(newPos));
            asm(""::"r"(newPos));
            asm(""::"r"(newPos));
#endif
        }
    }
    return flag;
}

void CreateGlassWorkshopTopArrow(void)
{
    if (gScrollableMenuArrowFlags >> 1 != 1)
    {
        gScrollableMenuArrowFlags |= 0x02;
        CreateVerticalScrollIndicators(TOP_ARROW, 0x2c, 0x08);
    }
}

void CreateGlassWorkshopBottomArrow(void)
{
    if (!(gScrollableMenuArrowFlags & 0x01))
    {
        gScrollableMenuArrowFlags |= 0x01;
        CreateVerticalScrollIndicators(BOTTOM_ARROW, 0x2c, 0x58);
    }
}

void GlassWorkshopUpdateScrollIndicators(u8 newPos, u8 maxItems)
{
    if (newPos == 0)
    {
        gScrollableMenuArrowFlags ^= 0x02;
        DestroyVerticalScrollIndicator(TOP_ARROW);
    }
    else
    {
        CreateGlassWorkshopTopArrow();
    }
    if (newPos + maxItems < gScrollableMenuItemCount)
    {
        CreateGlassWorkshopBottomArrow();
    }
    else if (newPos + maxItems == gScrollableMenuItemCount)
    {
        gScrollableMenuArrowFlags ^= 0x01;
        DestroyVerticalScrollIndicator(BOTTOM_ARROW);
    }
}

void SpawnCameraObject(void)
{
    u8 objectEventId = SpawnSpecialObjectEventParameterized(7, MOVEMENT_TYPE_FACE_DOWN, LOCALID_CAMERA, gSaveBlock1.pos.x + 7, gSaveBlock1.pos.y + 7, 3);
    gObjectEvents[objectEventId].invisible = TRUE;
    CameraObjectSetFollowedObjectId(gObjectEvents[objectEventId].spriteId);
}

void RemoveCameraObject(void)
{
    CameraObjectSetFollowedObjectId(GetPlayerAvatarSpriteId());
    RemoveObjectEventByLocalIdAndMap(LOCALID_CAMERA, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup);
}

u8 GetPokeblockNameByMonNature(void)
{
    return CopyMonFavoritePokeblockName(GetNature(&gPlayerParty[GetLeadMonIndex()]), gStringVar1);
}

void GetSecretBaseNearbyMapName(void)
{
    GetMapName(gStringVar1, VarGet(VAR_SECRET_BASE_MAP), 0);
}

u16 GetBattleTowerSinglesStreak(void)
{
    return GetGameStat(GAME_STAT_BATTLE_TOWER_BEST_STREAK);
}

void BufferEReaderTrainerName(void)
{
    SetEReaderTrainerName(gStringVar1);
}

static const u8 sSlotMachineRandomSeeds[] = {12, 2, 4, 5, 1, 8, 7, 11, 3, 10, 9, 6};
static const u8 sSlotMachineIds[] = {0, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 5};
static const u8 sSlotMachineServiceDayIds[] = {3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5};

u16 GetSlotMachineId(void)
{
    u32 rnd = gSaveBlock1.dewfordTrends[0].trendiness + gSaveBlock1.dewfordTrends[0].rand + sSlotMachineRandomSeeds[gSpecialVar_0x8004];
    if (GetPriceReduction(2))
    {
        return sSlotMachineServiceDayIds[rnd % 12];
    }
    return sSlotMachineIds[rnd % 12];
}

bool8 FoundAbandonedShipRoom1Key(void)
{
    u16 *specVar = &gSpecialVar_0x8004;
    u16 flag = FLAG_HIDDEN_ITEM_ABANDONED_SHIP_RM_1_KEY;
    *specVar = flag;
    if (!FlagGet(flag))
    {
        return FALSE;
    }
    return TRUE;
}

bool8 FoundAbandonedShipRoom2Key(void)
{
    u16 *specVar = &gSpecialVar_0x8004;
    u16 flag = FLAG_HIDDEN_ITEM_ABANDONED_SHIP_RM_2_KEY;
    *specVar = flag;
    if (!FlagGet(flag))
    {
        return FALSE;
    }
    return TRUE;
}

bool8 FoundAbandonedShipRoom4Key(void)
{
    u16 *specVar = &gSpecialVar_0x8004;
    u16 flag = FLAG_HIDDEN_ITEM_ABANDONED_SHIP_RM_4_KEY;
    *specVar = flag;
    if (!FlagGet(flag))
    {
        return FALSE;
    }
    return TRUE;
}

bool8 FoundAbandonedShipRoom6Key(void)
{
    u16 *specVar = &gSpecialVar_0x8004;
    u16 flag = FLAG_HIDDEN_ITEM_ABANDONED_SHIP_RM_6_KEY;
    *specVar = flag;
    if (!FlagGet(flag))
    {
        return FALSE;
    }
    return TRUE;
}

bool8 LeadMonHasEffortRibbon(void)
{
    return GetMonData(&gPlayerParty[GetLeadMonIndex()], MON_DATA_EFFORT_RIBBON, NULL);
}

void GiveLeadMonEffortRibbon(void)
{
    bool8 ribbonSet;
    IncrementGameStat(GAME_STAT_RECEIVED_RIBBONS);
    FlagSet(FLAG_SYS_RIBBON_GET);
    ribbonSet = TRUE;
    SetMonData(&gPlayerParty[GetLeadMonIndex()], MON_DATA_EFFORT_RIBBON, &ribbonSet);
}

bool8 Special_AreLeadMonEVsMaxedOut(void)
{
    if (GetMonEVCount(&gPlayerParty[GetLeadMonIndex()]) >= 510)
    {
        return TRUE;
    }
    return FALSE;
}

u8 TryUpdateRusturfTunnelState(void)
{
    if (!FlagGet(FLAG_RUSTURF_TUNNEL_OPENED) && gSaveBlock1.location.mapGroup == MAP_GROUP(MAP_RUSTURF_TUNNEL) && gSaveBlock1.location.mapNum == MAP_NUM(MAP_RUSTURF_TUNNEL))
    {
        if (FlagGet(FLAG_HIDE_RUSTURF_TUNNEL_ROCK_1))
        {
            VarSet(VAR_RUSTURF_TUNNEL_STATE, 4);
            return TRUE;
        }
        else if (FlagGet(FLAG_HIDE_RUSTURF_TUNNEL_ROCK_2))
        {
            VarSet(VAR_RUSTURF_TUNNEL_STATE, 5);
            return TRUE;
        }
    }
    return FALSE;
}

void SetShoalItemFlag(u16 v0)
{
    FlagSet(0x85f);
}

void LoadWallyZigzagoon(void)
{
    u16 monData;
    CreateMon(&gPlayerParty[0], SPECIES_ZIGZAGOON, 7, 0x20, FALSE, 0, FALSE, 0);
    monData = TRUE;
    SetMonData(&gPlayerParty[0], MON_DATA_ALT_ABILITY, &monData);
    monData = MOVE_TACKLE;
    SetMonData(&gPlayerParty[0], MON_DATA_MOVE1, &monData);
    monData = MOVE_NONE;
    SetMonData(&gPlayerParty[0], MON_DATA_MOVE2, &monData);
    SetMonData(&gPlayerParty[0], MON_DATA_MOVE3, &monData);
    SetMonData(&gPlayerParty[0], MON_DATA_MOVE4, &monData);
}

bool8 IsStarterInParty(void)
{
    u8 i;
    u16 starter = GetStarterPokemon(VarGet(VAR_STARTER_MON));
    u8 partyCount = CalculatePlayerPartyCount();
    for (i = 0; i < partyCount; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES2, NULL) == starter)
        {
            return TRUE;
        }
    }
    return FALSE;
}

bool8 ScriptCheckFreePokemonStorageSpace(void)
{
    u16 i, j;
    for (i=0; i<14; i++)
    {
        for (j=0; j<30; j++)
        {
            if (GetBoxMonData(&gPokemonStorage.boxes[i][j], MON_DATA_SPECIES, NULL) == SPECIES_NONE)
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}

bool8 IsPokerusInParty(void)
{
    if (!CheckPartyPokerus(gPlayerParty, 0x3f))
    {
        return FALSE;
    }
    return TRUE;
}

static void Task_ShakeCamera(u8);
static void StopCameraShake(u8);

void ShakeCamera(void)
{
    u8 taskId = CreateTask(Task_ShakeCamera, 9);
    gTasks[taskId].data[0] = gSpecialVar_0x8005;
    gTasks[taskId].data[1] = 0;
    gTasks[taskId].data[2] = 0;
    gTasks[taskId].data[3] = 0;
    gTasks[taskId].data[4] = gSpecialVar_0x8004;
    gTasks[taskId].data[5] = 5;
    SetCameraPanningCallback(NULL);
    PlaySE(SE_M_STRENGTH);
}

static void Task_ShakeCamera(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    task->data[1]++;
    if ((task->data[1] % task->data[5]) == 0)
    {
        task->data[1] = 0;
        task->data[2]++;
        if (task->data[3] == 0)
        {
            task->data[0] = -task->data[0];
            task->data[4] = -task->data[4];
            SetCameraPanning(task->data[0], task->data[4]);
            if (task->data[2] == 8)
            {
                StopCameraShake(taskId);
                InstallCameraPanAheadCallback();
            }
        }
    }
}

static void StopCameraShake(u8 taskId)
{
    DestroyTask(taskId);
    ScriptContext_Enable();
}

bool8 FoundBlackGlasses(void)
{
    return FlagGet(FLAG_HIDDEN_ITEM_ROUTE_116_BLACK_GLASSES);
}

void SetRoute119Weather(void)
{
    if (is_map_type_1_2_3_5_or_6(GetLastUsedWarpMapType()) != TRUE)
    {
        SetSavedWeather(WEATHER_ROUTE119_CYCLE);
    }
}

void SetRoute123Weather(void)
{
    if (is_map_type_1_2_3_5_or_6(GetLastUsedWarpMapType()) != TRUE)
    {
        SetSavedWeather(WEATHER_ROUTE123_CYCLE);
    }
}

u8 GetLeadMonIndex(void)
{
    u8 i;
    u8 partyCount = CalculatePlayerPartyCount();
    for (i = 0; i < partyCount; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES2, NULL) != SPECIES_EGG && GetMonData(&gPlayerParty[i], MON_DATA_SPECIES2, NULL) != 0)
            return i;
    }

    return 0;
}

u16 ScriptGetPartyMonSpecies(void)
{
    return GetMonData(&gPlayerParty[gSpecialVar_0x8004], MON_DATA_SPECIES2, NULL);
}

void TryInitBattleTowerAwardManObjectEvent(void)
{
    TryInitLocalObjectEvent(6);
}

u16 GetDaysUntilPacifidlogTMAvailable(void)
{
    u16 tmReceivedDay = VarGet(VAR_PACIFIDLOG_TM_RECEIVED_DAY);
    if (gLocalTime.days - tmReceivedDay >= 7)
    {
        return 0;
    }
    else if (gLocalTime.days < 0)
    {
        return 8;
    }
    return 7 - (gLocalTime.days - tmReceivedDay);
}

u16 SetPacifidlogTMReceivedDay(void)
{
    VarSet(VAR_PACIFIDLOG_TM_RECEIVED_DAY, gLocalTime.days);
    return gLocalTime.days;
}

bool8 MonOTNameNotPlayer(void)
{
    GetMonData(&gPlayerParty[gSpecialVar_0x8004], MON_DATA_OT_NAME, gStringVar1);
    if (!StringCompareWithoutExtCtrlCodes(gSaveBlock2.playerName, gStringVar1))
    {
        return FALSE;
    }
    return TRUE;
}

void BufferLottoTicketNumber(void)
{
    if (gSpecialVar_Result >= 10000)
    {
        sub_80BF088(0, gSpecialVar_Result);
    }
    else if (gSpecialVar_Result >= 1000)
    {
        gStringVar1[0] = CHAR_0;
        ConvertIntToDecimalStringN(gStringVar1 + 1, gSpecialVar_Result, 0, sub_80BF0B8(gSpecialVar_Result));
    }
    else if (gSpecialVar_Result >= 100)
    {
        gStringVar1[0] = CHAR_0;
        gStringVar1[1] = CHAR_0;
        ConvertIntToDecimalStringN(gStringVar1 + 2, gSpecialVar_Result, 0, sub_80BF0B8(gSpecialVar_Result));
    }
    else if (gSpecialVar_Result >= 10)
    {
        gStringVar1[0] = CHAR_0;
        gStringVar1[1] = CHAR_0;
        gStringVar1[2] = CHAR_0;
        ConvertIntToDecimalStringN(gStringVar1 + 3, gSpecialVar_Result, 0, sub_80BF0B8(gSpecialVar_Result));
    }
    else
    {
        gStringVar1[0] = CHAR_0;
        gStringVar1[1] = CHAR_0;
        gStringVar1[2] = CHAR_0;
        gStringVar1[3] = CHAR_0;
        ConvertIntToDecimalStringN(gStringVar1 + 4, gSpecialVar_Result, 0, sub_80BF0B8(gSpecialVar_Result));
    }
}

const u8 gFanClubCounterIncrements[] = {2, 1, 2, 1};
const u8 gFanClubMemberIdsForGainingFans[] = {8,  9, 10, 11, 12, 13, 14, 15};
const u8 gFanClubMemberIdsForLosingFans[] = {8, 13, 14, 11, 10, 12, 15,  9};

bool8 DidPlayerGetFirstFans(void);
void TryLoseFansFromPlayTime(void);
void SetPlayerGotFirstFans(void);
void SetInitialFansOfPlayer(void);
u16 GetNumFansOfPlayerInTrainerFanClub(void);
int PlayerGainRandomTrainerFan(void);

void ResetFanClub(void)
{
    gSaveBlock1.vars[VAR_FANCLUB_FAN_COUNTER - VARS_START] = 0;
    gSaveBlock1.vars[VAR_FANCLUB_LOSE_FAN_TIMER - VARS_START] = 0;
}

void TryLoseFansFromPlayTimeAfterLinkBattle(void)
{
    if (DidPlayerGetFirstFans())
    {
        TryLoseFansFromPlayTime();
        gSaveBlock1.vars[VAR_FANCLUB_LOSE_FAN_TIMER - VARS_START] = gSaveBlock2.playTimeHours;
    }
}

void UpdateTrainerFanClubGameClear(void)
{
    if (!((gSaveBlock1.vars[VAR_FANCLUB_FAN_COUNTER - VARS_START] >> 7) & 1))
    {
        SetPlayerGotFirstFans();
        SetInitialFansOfPlayer();
        gSaveBlock1.vars[VAR_FANCLUB_LOSE_FAN_TIMER - VARS_START] = gSaveBlock2.playTimeHours;
        FlagClear(FLAG_HIDE_FANCLUB_OLD_LADY);
        FlagClear(FLAG_HIDE_FANCLUB_BOY);
        FlagClear(FLAG_HIDE_FANCLUB_LITTLE_BOY);
        FlagClear(FLAG_HIDE_FANCLUB_LADY);
        VarSet(VAR_LILYCOVE_FAN_CLUB_STATE, 1);
    }
}

u8 TryGainNewFanFromCounter(u8 incrementId)
{
    if (VarGet(VAR_LILYCOVE_FAN_CLUB_STATE) == 2)
    {
        if ((gSaveBlock1.vars[VAR_FANCLUB_FAN_COUNTER - VARS_START] & 0x7f) + gFanClubCounterIncrements[incrementId] >= 20)
        {
            if (GetNumFansOfPlayerInTrainerFanClub() < 3)
            {
                PlayerGainRandomTrainerFan();
                gSaveBlock1.vars[VAR_FANCLUB_FAN_COUNTER - VARS_START] &= 0xff80;
            }
            else
            {
                gSaveBlock1.vars[VAR_FANCLUB_FAN_COUNTER - VARS_START] = (gSaveBlock1.vars[VAR_FANCLUB_FAN_COUNTER - VARS_START] & 0xff80) | 20;
            }
        }
        else
        {
            gSaveBlock1.vars[VAR_FANCLUB_FAN_COUNTER - VARS_START] += gFanClubCounterIncrements[incrementId];
        }
    }
    return gSaveBlock1.vars[VAR_FANCLUB_FAN_COUNTER - VARS_START] & 0x7f;
}

int PlayerGainRandomTrainerFan(void)
{
    u8 i;
    int fanIndex = 0;
    for (i=0; i<8; i++)
    {
        if (!((gSaveBlock1.vars[VAR_FANCLUB_FAN_COUNTER - VARS_START] >> gFanClubMemberIdsForGainingFans[i]) & 0x01))
        {
            fanIndex = i;
            if (Random() & 1)
            {
                gSaveBlock1.vars[VAR_FANCLUB_FAN_COUNTER - VARS_START] |= (1 << gFanClubMemberIdsForGainingFans[i]);
                return fanIndex;
            }
        }
    }
    gSaveBlock1.vars[VAR_FANCLUB_FAN_COUNTER - VARS_START] |= (1 << gFanClubMemberIdsForGainingFans[fanIndex]);
    return fanIndex;
}

int PlayerLoseRandomTrainerFan(void)
{
    u8 i;
    int fanIndex = 0;
    if (GetNumFansOfPlayerInTrainerFanClub() == 1)
    {
        return 0;
    }
    for (i=0; i<8; i++)
    {
        if ((gSaveBlock1.vars[VAR_FANCLUB_FAN_COUNTER - VARS_START] >> gFanClubMemberIdsForLosingFans[i]) & 1)
        {
            fanIndex = i;
            if (Random() & 1)
            {
                gSaveBlock1.vars[VAR_FANCLUB_FAN_COUNTER - VARS_START] ^= (1 << gFanClubMemberIdsForLosingFans[i]);
                return fanIndex;
            }
        }
    }
    if ((gSaveBlock1.vars[VAR_FANCLUB_FAN_COUNTER - VARS_START] >> gFanClubMemberIdsForLosingFans[fanIndex]) & 1)
    {
        gSaveBlock1.vars[VAR_FANCLUB_FAN_COUNTER - VARS_START] ^= (1 << gFanClubMemberIdsForLosingFans[fanIndex]);
    }
    return fanIndex;
}

u16 GetNumFansOfPlayerInTrainerFanClub(void)
{
    u8 i;
    u8 numFans = 0;
    for (i = 0; i < 8; i++)
    {
        if ((gSaveBlock1.vars[VAR_FANCLUB_FAN_COUNTER - VARS_START] >> (i + 8)) & 1)
        {
            numFans++;
        }
    }

    return numFans;
}

void TryLoseFansFromPlayTime(void)
{
    u8 i = 0;
    if (gSaveBlock2.playTimeHours < 999)
    {
        while (1)
        {
            if (GetNumFansOfPlayerInTrainerFanClub() < 5)
            {
                gSaveBlock1.vars[VAR_FANCLUB_LOSE_FAN_TIMER - VARS_START] = gSaveBlock2.playTimeHours;
                break;
            }
            else if (i == 8)
            {
                break;
            }
            else if (gSaveBlock2.playTimeHours - gSaveBlock1.vars[VAR_FANCLUB_LOSE_FAN_TIMER - VARS_START] < 12)
            {
                return;
            }
            PlayerLoseRandomTrainerFan();
            gSaveBlock1.vars[VAR_FANCLUB_LOSE_FAN_TIMER - VARS_START] += 12;
            i++;
        }
    }
}

bool8 IsFanClubMemberFanOfPlayer(void)
{
    return (gSaveBlock1.vars[VAR_FANCLUB_FAN_COUNTER - VARS_START] >> gSpecialVar_0x8004) & 0x01;
}

void SetInitialFansOfPlayer(void)
{
    gSaveBlock1.vars[VAR_FANCLUB_FAN_COUNTER - VARS_START] |= 0x2000;
    gSaveBlock1.vars[VAR_FANCLUB_FAN_COUNTER - VARS_START] |= 0x100;
    gSaveBlock1.vars[VAR_FANCLUB_FAN_COUNTER - VARS_START] |= 0x400;
}

void BufferFanClubTrainerName_(void *, u8, u8);

void BufferFanClubTrainerName(void)
{
    u8 whichLinkTrainer = 0;
    u8 whichNPCTrainer = 0;
    switch (gSpecialVar_0x8004)
    {
        case 8:
            break;
        case 9:
            break;
        case 10:
            whichLinkTrainer = 0;
            whichNPCTrainer = 3;
            break;
        case 11:
            whichLinkTrainer = 0;
            whichNPCTrainer = 1;
            break;
        case 12:
            whichLinkTrainer = 1;
            whichNPCTrainer = 0;
            break;
        case 13:
            whichLinkTrainer = 0;
            whichNPCTrainer = 4;
            break;
        case 14:
            whichLinkTrainer = 1;
            whichNPCTrainer = 5;
            break;
        case 15:
            break;
    }
    BufferFanClubTrainerName_(gSaveBlock1.linkBattleRecords, whichLinkTrainer, whichNPCTrainer);
}

void BufferFanClubTrainerName_(void *linkRecords, u8 whichLinkTrainer, u8 whichNPCTrainer)
{
    u8 *record = (linkRecords + 16 * whichLinkTrainer);
    if (*record == EOS)
    {
        switch (whichNPCTrainer)
        {
            case 0:
                StringCopy(gStringVar1, gOtherText_Wallace);
                break;
            case 1:
                StringCopy(gStringVar1, gOtherText_Steven);
                break;
            case 2:
                StringCopy(gStringVar1, gOtherText_Brawly);
                break;
            case 3:
                StringCopy(gStringVar1, gOtherText_Winona);
                break;
            case 4:
                StringCopy(gStringVar1, gOtherText_Phoebe);
                break;
            case 5:
                StringCopy(gStringVar1, gOtherText_Glacia);
                break;
            default:
                StringCopy(gStringVar1, gOtherText_Wallace);
                break;
        }
    }
    else
    {
        StringCopyN(gStringVar1, record, 7);
        gStringVar1[7] = EOS;
        if (gStringVar1[0] == 0xfc && gStringVar1[1] == 0x15)
        {
            ConvertInternationalString(gStringVar1, 1);
        }
    }
}

void UpdateTrainerFansAfterLinkBattle(void)
{
    if (VarGet(VAR_LILYCOVE_FAN_CLUB_STATE) == 2)
    {
        TryLoseFansFromPlayTimeAfterLinkBattle();
        if (gBattleOutcome == 1)
        {
            PlayerGainRandomTrainerFan();
        }
        else
        {
            PlayerLoseRandomTrainerFan();
        }
    }
}

bool8 DidPlayerGetFirstFans(void)
{
    return (gSaveBlock1.vars[VAR_FANCLUB_FAN_COUNTER - VARS_START] >> 7) & 0x01;
}

void SetPlayerGotFirstFans(void)
{
    gSaveBlock1.vars[VAR_FANCLUB_FAN_COUNTER - VARS_START] |= 0x80;
}

u8 Script_TryGainNewFanFromCounter(void)
{
    return TryGainNewFanFromCounter(gSpecialVar_0x8004);
}
