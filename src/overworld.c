#include "global.h"
#include "overworld.h"
#include "battle_setup.h"
#include "berry.h"
#include "cable_club.h"
#include "clock.h"
#include "constants/map_types.h"
#include "constants/maps.h"
#include "constants/songs.h"
#include "constants/species.h"
#include "contest_util.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_camera.h"
#include "field_control_avatar.h"
#include "field_effect.h"
#include "field_fadetransition.h"
#include "field_message_box.h"
#include "field_player_avatar.h"
#include "field_screen_effect.h"
#include "field_special_scene.h"
#include "field_specials.h"
#include "field_tasks.h"
#include "field_weather.h"
#include "fieldmap.h"
#include "fldeff_flash.h"
#include "heal_location.h"
#include "link.h"
#include "load_save.h"
#include "m4a.h"
#include "main.h"
#include "map_name_popup.h"
#include "menu.h"
#include "metatile_behavior.h"
#include "new_game.h"
#include "palette.h"
#include "play_time.h"
#include "random.h"
#include "roamer.h"
#include "rotating_gate.h"
#include "safari_zone.h"
#include "scanline_effect.h"
#include "script.h"
#include "secret_base.h"
#include "sound.h"
#include "start_menu.h"
#include "task.h"
#include "tileset_anim.h"
#include "time_events.h"
#include "tv.h"
#include "wild_encounter.h"

#ifdef SAPPHIRE
#define LEGENDARY_MUSIC MUS_WEATHER_KYOGRE  // Heavy Rain
#else
#define LEGENDARY_MUSIC MUS_WEATHER_GROUDON // Drought
#endif

#define PLAYER_LINK_STATE_IDLE 0x80
#define PLAYER_LINK_STATE_BUSY 0x81
#define PLAYER_LINK_STATE_READY 0x82
#define PLAYER_LINK_STATE_EXITING_ROOM 0x83

#define FACING_NONE 0
#define FACING_UP 1
#define FACING_DOWN 2
#define FACING_LEFT 3
#define FACING_RIGHT 4
#define FACING_FORCED_UP 7
#define FACING_FORCED_DOWN 8
#define FACING_FORCED_LEFT 9
#define FACING_FORCED_RIGHT 10

extern u8 gUnknown_020297ED;
extern u16 gTotalCameraPixelOffsetY;
extern u16 gTotalCameraPixelOffsetX;

extern u8 EventScript_WhiteOut[];
extern u8 EventScript_ResetMrBriney[];
extern u8 SingleBattleColosseum_EventScript_PlayerSpot0[];
extern u8 SingleBattleColosseum_EventScript_PlayerSpot1[];
extern u8 DoubleBattleColosseum_EventScript_PlayerSpot0[];
extern u8 DoubleBattleColosseum_EventScript_PlayerSpot1[];
extern u8 DoubleBattleColosseum_EventScript_PlayerSpot2[];
extern u8 DoubleBattleColosseum_EventScript_PlayerSpot3[];
extern u8 TradeCenter_EventScript_Chair0[];
extern u8 TradeCenter_EventScript_Chair1[];
extern u8 RecordCorner_EventScript_PlayerSpot0[];
extern u8 RecordCorner_EventScript_PlayerSpot1[];
extern u8 RecordCorner_EventScript_PlayerSpot2[];
extern u8 RecordCorner_EventScript_PlayerSpot3[];
extern u8 CableClub_EventScript_ReadTrainerCard[];
extern u8 CableClub_EventScript_ReadTrainerCardColored[];
extern u8 CableClub_EventScript_TooBusyToNotice[];
extern u8 EventScript_ConfirmLeaveCableClubRoom[];
extern u8 EventScript_TerminateLink[];
extern u8 EventScript_DoLinkRoomExit[];

extern struct MapLayout * const gMapLayouts[];
extern struct MapHeader * const * const gMapGroups[];
extern s32 gMaxFlashLevel;

u16 gHeldKeyCodeToSend;
void (*gFieldCallback)(void);
u8 gLocalLinkPlayerId;
u8 gFieldLinkPlayerCount;

EWRAM_DATA struct WarpData gLastUsedWarp = {0};
EWRAM_DATA struct WarpData gWarpDestination = {0};
EWRAM_DATA struct WarpData gFixedDiveWarp = {0};
EWRAM_DATA struct WarpData gFixedHoleWarp = {0};
EWRAM_DATA struct InitialPlayerAvatarState gInitialPlayerAvatarState = {0};
EWRAM_DATA static u16 sAmbientCrySpecies = 0;
EWRAM_DATA static bool8 sIsAmbientCryWaterMon = FALSE;
EWRAM_DATA struct LinkPlayerObjectEvent gLinkPlayerObjectEvents[4] = {0};

static u8 sPlayerLinkStates[4];
static u16 (*sPlayerKeyInterceptCallback)(u32);
static u8 sReceivingFromLink;

static void CB2_Overworld(void);
static u8 GetAdjustedInitialTransitionFlags(struct InitialPlayerAvatarState*, u16, u8);
static u8 GetAdjustedInitialDirection(struct InitialPlayerAvatarState*, u8, u16, u8);
static bool32 sub_805483C(u8*);
static void c2_80567AC(void);
static void Overworld_ResetStateAfterWhiteOut(void);
static void InitOverworldGraphicsRegisters(void);
static void ChooseAmbientCrySpecies(void);
static void SetFieldVBlankCallback(void);
static void VBlankCB_Field(void);
static void SpawnLinkPlayerObjectEvent(u8, s16, s16, u8);
static void InitLinkPlayerObjectEventPos(struct ObjectEvent *, s16, s16);
static u8 GetLinkPlayerIdAt(s16, s16);
static u8 FlipVerticalAndClearForced(u8, u8);
static u8 LinkPlayerGetCollision(u8, u8, s16, s16);
static void CreateLinkPlayerSprite(u8);
static void SpriteCB_LinkPlayer(struct Sprite *);
static u16 GetDirectionForEventScript(const u8 *);
static void RunInteractLocalPlayerScript(const u8 *);
static void InitMenuBasedScript(const u8 *);

static const struct WarpData sDummyWarpData =
{
    .mapGroup = -1,
    .mapNum = -1,
    .warpId = -1,
    .x = -1,
    .y = -1,
};

static const u8 sUnusedData[] =
{
    0xB0, 0x04, 0x00, 0x00,
    0x10, 0x0E, 0x00, 0x00,
    0xB0, 0x04, 0x00, 0x00,
    0x60, 0x09, 0x00, 0x00,
    0x32, 0x00, 0x00, 0x00,
    0x50, 0x00, 0x00, 0x00,
    0xD4, 0xFF, 0xFF, 0xFF,
    0x2C, 0x00, 0x00, 0x00,
};

const struct UCoords32 gDirectionToVectors[] =
{
    { 0,  0}, // DIR_NONE
    { 0,  1}, // DIR_SOUTH
    { 0, -1}, // DIR_NORTH
    {-1,  0}, // DIR_WEST
    { 1,  0}, // DIR_EAST
    {-1,  1}, // DIR_SOUTHWEST
    { 1,  1}, // DIR_SOUTHEAST
    {-1, -1}, // DIR_NORTHWEST
    { 1, -1}, // DIR_NORTHEAST
};

static const struct ScanlineEffectParams sFlashEffectParams =
{
    (void *)REG_ADDR_WIN0H,
    ((DMA_ENABLE | DMA_START_HBLANK | DMA_REPEAT | DMA_DEST_RELOAD) << 16) | 1,
    1,
    0,
};

static u8 MovementEventModeCB_Normal(struct LinkPlayerObjectEvent *, struct ObjectEvent *, u8);
static u8 MovementEventModeCB_Ignored(struct LinkPlayerObjectEvent *, struct ObjectEvent *, u8);
static u8 MovementEventModeCB_Scripted(struct LinkPlayerObjectEvent *, struct ObjectEvent *, u8);

static u8 (*const sLinkPlayerMovementModes[])(struct LinkPlayerObjectEvent *, struct ObjectEvent *, u8) =
{
    MovementEventModeCB_Normal,
    MovementEventModeCB_Ignored,
    MovementEventModeCB_Scripted,
};

static u8 FacingHandler_DoNothing(struct LinkPlayerObjectEvent *, struct ObjectEvent *, u8);
static u8 FacingHandler_DpadMovement(struct LinkPlayerObjectEvent *, struct ObjectEvent *, u8);
static u8 FacingHandler_ForcedFacingChange(struct LinkPlayerObjectEvent *, struct ObjectEvent *, u8);

static u8 (*const sLinkPlayerFacingHandlers[])(struct LinkPlayerObjectEvent *, struct ObjectEvent *, u8) =
{
    FacingHandler_DoNothing,
    FacingHandler_DpadMovement,
    FacingHandler_DpadMovement,
    FacingHandler_DpadMovement,
    FacingHandler_DpadMovement,
    FacingHandler_DoNothing,
    FacingHandler_DoNothing,
    FacingHandler_ForcedFacingChange,
    FacingHandler_ForcedFacingChange,
    FacingHandler_ForcedFacingChange,
    FacingHandler_ForcedFacingChange,
};

static void MovementStatusHandler_EnterFreeMode(struct LinkPlayerObjectEvent *, struct ObjectEvent *);
static void MovementStatusHandler_TryAdvanceScript(struct LinkPlayerObjectEvent *, struct ObjectEvent *);

static void (*const sMovementStatusHandler[])(struct LinkPlayerObjectEvent *, struct ObjectEvent *) =
{
    MovementStatusHandler_EnterFreeMode,
    MovementStatusHandler_TryAdvanceScript,
};


static void DoWhiteOut(void)
{
    RunScriptImmediately(EventScript_WhiteOut);
    gSaveBlock1.money /= 2;
    HealPlayerParty();
    Overworld_ResetStateAfterWhiteOut();
    Overworld_SetWarpDestToLastHealLoc();
    WarpIntoMap();
}

void Overworld_ResetStateAfterFly(void)
{
    ResetInitialPlayerAvatarState();
    FlagClear(FLAG_SYS_CYCLING_ROAD);
    FlagClear(FLAG_SYS_CRUISE_MODE);
    FlagClear(FLAG_SYS_SAFARI_MODE);
    FlagClear(FLAG_SYS_USE_STRENGTH);
    FlagClear(FLAG_SYS_USE_FLASH);
}

void Overworld_ResetStateAfterTeleport(void)
{
    ResetInitialPlayerAvatarState();
    FlagClear(FLAG_SYS_CYCLING_ROAD);
    FlagClear(FLAG_SYS_CRUISE_MODE);
    FlagClear(FLAG_SYS_SAFARI_MODE);
    FlagClear(FLAG_SYS_USE_STRENGTH);
    FlagClear(FLAG_SYS_USE_FLASH);
    RunScriptImmediately(EventScript_ResetMrBriney);
}

void Overworld_ResetStateAfterDigEscRope(void)
{
    ResetInitialPlayerAvatarState();
    FlagClear(FLAG_SYS_CYCLING_ROAD);
    FlagClear(FLAG_SYS_CRUISE_MODE);
    FlagClear(FLAG_SYS_SAFARI_MODE);
    FlagClear(FLAG_SYS_USE_STRENGTH);
    FlagClear(FLAG_SYS_USE_FLASH);
}

static void Overworld_ResetStateAfterWhiteOut(void)
{
    ResetInitialPlayerAvatarState();
    FlagClear(FLAG_SYS_CYCLING_ROAD);
    FlagClear(FLAG_SYS_CRUISE_MODE);
    FlagClear(FLAG_SYS_SAFARI_MODE);
    FlagClear(FLAG_SYS_USE_STRENGTH);
    FlagClear(FLAG_SYS_USE_FLASH);
}

static void sub_805308C(void)
{
    FlagClear(FLAG_SYS_SAFARI_MODE);
    ChooseAmbientCrySpecies();
    ResetCyclingRoadChallengeData();
    UpdateLocationHistoryForRoamer();
    RoamerMoveToOtherLocationSet();
}

void ResetGameStats(void)
{
    s32 i;

    for (i = 0; i < NUM_GAME_STATS; i++)
        gSaveBlock1.gameStats[i] = 0;
}

void IncrementGameStat(u8 index)
{
    if (index < NUM_GAME_STATS)
    {
        if (gSaveBlock1.gameStats[index] < 0xFFFFFF)
            gSaveBlock1.gameStats[index]++;
        else
            gSaveBlock1.gameStats[index] = 0xFFFFFF;
    }
}

u32 GetGameStat(u8 stat)
{
    if (stat >= NUM_GAME_STATS)
        return 0;

    return gSaveBlock1.gameStats[stat];
}

void SetGameStat(u8 stat, u32 value)
{
    if (stat < NUM_GAME_STATS)
        gSaveBlock1.gameStats[stat] = value;
}

void LoadObjEventTemplatesFromHeader(void)
{
    // Clear object event templates
    CpuFill32(0, gSaveBlock1.objectEventTemplates, sizeof(gSaveBlock1.objectEventTemplates));

    // Copy map header events to save block
    CpuCopy32(gMapHeader.events->objectEvents,
              gSaveBlock1.objectEventTemplates,
              gMapHeader.events->objectEventCount * sizeof(struct ObjectEventTemplate));
}

static void LoadSaveblockObjEventScripts(void)
{
    struct ObjectEventTemplate *objectEventTemplates = gSaveBlock1.objectEventTemplates;
    s32 i;

    for (i = 0; i < 64; i++)
        objectEventTemplates[i].script = gMapHeader.events->objectEvents[i].script;
}

void Overworld_SetObjEventTemplateCoords(u8 localId, s16 x, s16 y)
{
    s32 i;
    for (i = 0; i < 64; i++)
    {
        struct ObjectEventTemplate *objectEventTemplate = &gSaveBlock1.objectEventTemplates[i];
        if (objectEventTemplate->localId == localId)
        {
            objectEventTemplate->x = x;
            objectEventTemplate->y = y;
            return;
        }
    }
}

void Overworld_SetObjEventTemplateMovementType(u8 localId, u8 movementType)
{
    s32 i;

    for (i = 0; i < 64; i++)
    {
        struct ObjectEventTemplate *objectEventTemplate = &gSaveBlock1.objectEventTemplates[i];
        if (objectEventTemplate->localId == localId)
        {
            objectEventTemplate->movementType = movementType;
            return;
        }
    }
}

static void mapdata_load_assets_to_gpu_and_full_redraw(void)
{
    ResetFieldCamera();
    CopyMapTilesetsToVram(gMapHeader.mapLayout);
    LoadMapTilesetPalettes(gMapHeader.mapLayout);
    DrawWholeMapView();
    cur_mapheader_run_tileset_funcs_after_some_cpuset();
}

static struct MapLayout *GetMapLayout(void)
{
    u16 mapLayoutId = gSaveBlock1.mapLayoutId;
    if (mapLayoutId)
        return gMapLayouts[mapLayoutId - 1];
    return NULL;
}

static void ApplyCurrentWarp(void)
{
    gLastUsedWarp = gSaveBlock1.location;
    gSaveBlock1.location = gWarpDestination;
    gFixedDiveWarp = sDummyWarpData;
    gFixedHoleWarp = sDummyWarpData;
}

static void SetWarpData(struct WarpData *warp, s8 mapGroup, s8 mapNum, s8 warpId, s8 x, s8 y)
{
    warp->mapGroup = mapGroup;
    warp->mapNum = mapNum;
    warp->warpId = warpId;
    warp->x = x;
    warp->y = y;
}

static bool32 IsDummyWarp(struct WarpData *warp)
{
    if (warp->mapGroup != -1)
        return FALSE;
    if (warp->mapNum != -1)
        return FALSE;
    if (warp->warpId != -1)
        return FALSE;
    if (warp->x != -1)
        return FALSE;
    if (warp->y != -1)
        return FALSE;
    return TRUE;
}

struct MapHeader *const Overworld_GetMapHeaderByGroupAndId(u16 mapGroup, u16 mapNum)
{
    return gMapGroups[mapGroup][mapNum];
}

struct MapHeader *const GetDestinationWarpMapHeader(void)
{
    return Overworld_GetMapHeaderByGroupAndId(gWarpDestination.mapGroup, gWarpDestination.mapNum);
}

static void LoadCurrentMapData(void)
{
    gMapHeader = *Overworld_GetMapHeaderByGroupAndId(gSaveBlock1.location.mapGroup, gSaveBlock1.location.mapNum);
    gSaveBlock1.mapLayoutId = gMapHeader.mapLayoutId;
    gMapHeader.mapLayout = GetMapLayout();
}

static void LoadSaveblockMapHeader(void)
{
    gMapHeader = *Overworld_GetMapHeaderByGroupAndId(gSaveBlock1.location.mapGroup, gSaveBlock1.location.mapNum);
    gMapHeader.mapLayout = GetMapLayout();
}

static void SetPlayerCoordsFromWarp(void)
{
    if (gSaveBlock1.location.warpId >= 0 && gSaveBlock1.location.warpId < gMapHeader.events->warpCount)
    {
        gSaveBlock1.pos.x = gMapHeader.events->warps[gSaveBlock1.location.warpId].x;
        gSaveBlock1.pos.y = gMapHeader.events->warps[gSaveBlock1.location.warpId].y;
    }
    else if (gSaveBlock1.location.x >= 0 && gSaveBlock1.location.y >= 0)
    {
        gSaveBlock1.pos.x = gSaveBlock1.location.x;
        gSaveBlock1.pos.y = gSaveBlock1.location.y;
    }
    else
    {
        gSaveBlock1.pos.x = gMapHeader.mapLayout->width / 2;
        gSaveBlock1.pos.y = gMapHeader.mapLayout->height / 2;
    }
}

void WarpIntoMap(void)
{
    ApplyCurrentWarp();
    LoadCurrentMapData();
    SetPlayerCoordsFromWarp();
}

void Overworld_SetWarpDestination(s8 mapGroup, s8 mapNum, s8 warpId, s8 x, s8 y)
{
    SetWarpData(&gWarpDestination, mapGroup, mapNum, warpId, x, y);
}

void warp1_set_2(s8 mapGroup, s8 mapNum, s8 warpId)
{
    Overworld_SetWarpDestination(mapGroup, mapNum, warpId, -1, -1);
}

void saved_warp2_set(int unused, s8 mapGroup, s8 mapNum, s8 warpId)
{
    SetWarpData(&gSaveBlock1.dynamicWarp, mapGroup, mapNum, warpId, gSaveBlock1.pos.x, gSaveBlock1.pos.y);
}

void saved_warp2_set_2(int unused, s8 mapGroup, s8 mapNum, s8 warpId, s8 x, s8 y)
{
    SetWarpData(&gSaveBlock1.dynamicWarp, mapGroup, mapNum, warpId, x, y);
}

void copy_saved_warp2_bank_and_enter_x_to_warp1(u8 unused)
{
    gWarpDestination = gSaveBlock1.dynamicWarp;
}

void sub_8053538(u8 a1)
{
    const struct HealLocation *warp = GetHealLocation(a1);

    if (warp)
        Overworld_SetWarpDestination(warp->mapGroup, warp->mapNum, -1, warp->x, warp->y);
}

void Overworld_SetWarpDestToLastHealLoc(void)
{
    gWarpDestination = gSaveBlock1.lastHealLocation;
}

void Overworld_SetHealLocationWarp(u8 healLocationId)
{
    const struct HealLocation *healLocation = GetHealLocation(healLocationId);

    if (healLocation != NULL)
        SetWarpData(&gSaveBlock1.lastHealLocation, healLocation->mapGroup, healLocation->mapNum, -1, healLocation->x, healLocation->y);
}

void sub_80535C4(s16 a1, s16 a2)
{
    u8 currMapType = Overworld_GetMapTypeOfSaveblockLocation();
    u8 destMapType = GetMapTypeByGroupAndId(gWarpDestination.mapGroup, gWarpDestination.mapNum);
    if (is_map_type_1_2_3_5_or_6(currMapType) && is_map_type_1_2_3_5_or_6(destMapType) != TRUE)
        sub_805363C(gSaveBlock1.location.mapGroup, gSaveBlock1.location.mapNum, -1, a1 - 7, a2 - 6);
}

void sub_805363C(s8 mapGroup, s8 mapNum, s8 warpId, s8 x, s8 y)
{
    SetWarpData(&gSaveBlock1.warp4, mapGroup, mapNum, warpId, x, y);
}

void sub_8053678(void)
{
    gWarpDestination = gSaveBlock1.warp4;
}

void SetFixedDiveWarp(s8 mapGroup, s8 mapNum, s8 warpId, s8 x, s8 y)
{
    SetWarpData(&gFixedDiveWarp, mapGroup, mapNum, warpId, x, y);
}

static void SetFixedDiveWarpAsDestination(void)
{
    gWarpDestination = gFixedDiveWarp;
}

void SetFixedHoleWarp(s8 mapGroup, s8 mapNum, s8 warpId, s8 x, s8 y)
{
    SetWarpData(&gFixedHoleWarp, mapGroup, mapNum, warpId, x, y);
}

void SetFixedHoleWarpAsDestination(s16 x, s16 y)
{
    if (IsDummyWarp(&gFixedHoleWarp) == TRUE)
        gWarpDestination = gLastUsedWarp;
    else
        Overworld_SetWarpDestination(gFixedHoleWarp.mapGroup, gFixedHoleWarp.mapNum, -1, x, y);
}

void sub_8053778(void)
{
    gWarpDestination = gSaveBlock1.warp1;
}

void unref_sub_8053790(s8 mapGroup, s8 mapNum, s8 warpId, s8 x, s8 y)
{
    SetWarpData(&gSaveBlock1.warp1, mapGroup, mapNum, warpId, x, y);
}

void SetContinueGameWarpToHealLocation(u8 a1)
{
    const struct HealLocation *warp = GetHealLocation(a1);
    if (warp)
        SetWarpData(&gSaveBlock1.warp1, warp->mapGroup, warp->mapNum, -1, warp->x, warp->y);
}

void gpu_sync_bg_hide()
{
    gSaveBlock1.warp1 = gSaveBlock1.dynamicWarp;
}

const struct MapConnection *GetMapConnection(u8 dir)
{
    s32 i;
    s32 count = gMapHeader.connections->count;
    const struct MapConnection *connection = gMapHeader.connections->connections;

    if (connection == NULL)
        return NULL;

    for(i = 0; i < count; i++, connection++)
        if (connection->direction == dir)
            return connection;

    return NULL;
}

static bool8 SetDiveWarp(u8 direction, u16 x, u16 y)
{
    const struct MapConnection *connection = GetMapConnection(direction);

    if (connection != NULL)
    {
        Overworld_SetWarpDestination(connection->mapGroup, connection->mapNum, -1, x, y);
    }
    else
    {
        RunOnDiveWarpMapScript();
        if (IsDummyWarp(&gFixedDiveWarp))
            return FALSE;

        SetFixedDiveWarpAsDestination();
    }

    return TRUE;
}

bool8 SetDiveWarpEmerge(u16 x, u16 y)
{
    return SetDiveWarp(CONNECTION_EMERGE, x, y);
}

bool8 SetDiveWarpDive(u16 x, u16 y)
{
    return SetDiveWarp(CONNECTION_DIVE, x, y);
}

void LoadMapFromCameraTransition(u8 mapGroup, u8 mapNum)
{
    s32 paletteIndex;

    Overworld_SetWarpDestination(mapGroup, mapNum, -1, -1, -1);
    sub_8053F0C();
    ApplyCurrentWarp();
    LoadCurrentMapData();
    LoadObjEventTemplatesFromHeader();
    ClearTempFieldEventData();
    ResetCyclingRoadChallengeData();
    RestartWildEncounterImmunitySteps();
    TryUpdateRandomTrainerRematches(mapGroup, mapNum);
    DoTimeBasedEvents();
    SetSavedWeatherFromCurrMapHeader();
    ChooseAmbientCrySpecies();
    SetDefaultFlashLevel();
    Overworld_ClearSavedMusic();
    RunOnTransitionMapScript();
    InitMap();
    CopySecondaryTilesetToVram(gMapHeader.mapLayout);
    LoadSecondaryTilesetPalette(gMapHeader.mapLayout);

    for (paletteIndex = 6; paletteIndex < 12; paletteIndex++)
        ApplyWeatherColorMapToPal(paletteIndex);

    sub_8072ED0();
    UpdateLocationHistoryForRoamer();
    RoamerMove();
    DoCurrentWeather();
    ResetFieldTasksArgs();
    RunOnResumeMapScript();
    ShowMapNamePopup();
}

void sub_8053994(u32 a1)
{
    bool8 v2;
    bool8 v3;

    LoadCurrentMapData();
    LoadObjEventTemplatesFromHeader();
    v2 = is_map_type_1_2_3_5_or_6(gMapHeader.mapType);
    v3 = Overworld_MapTypeIsIndoors(gMapHeader.mapType);
    ClearTempFieldEventData();
    ResetCyclingRoadChallengeData();
    RestartWildEncounterImmunitySteps();
    TryUpdateRandomTrainerRematches(gSaveBlock1.location.mapGroup, gSaveBlock1.location.mapNum);
    if (a1 != 1)
        DoTimeBasedEvents();
    SetSavedWeatherFromCurrMapHeader();
    ChooseAmbientCrySpecies();
    if (v2)
        FlagClear(FLAG_SYS_USE_FLASH);
    SetDefaultFlashLevel();
    Overworld_ClearSavedMusic();
    RunOnTransitionMapScript();
    UpdateLocationHistoryForRoamer();
    RoamerMoveToOtherLocationSet();
    InitMap();
    if (a1 != 1 && v3)
    {
        UpdateTVScreensOnMap(gBackupMapLayout.width, gBackupMapLayout.height);
        InitSecretBaseAppearance(TRUE);
    }
}

void ResetInitialPlayerAvatarState(void)
{
    gInitialPlayerAvatarState.direction = DIR_SOUTH;
    gInitialPlayerAvatarState.transitionFlags = PLAYER_AVATAR_FLAG_ON_FOOT;
}

void StoreInitialPlayerAvatarState(void)
{
    gInitialPlayerAvatarState.direction = GetPlayerFacingDirection();

    if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_MACH_BIKE))
        gInitialPlayerAvatarState.transitionFlags = PLAYER_AVATAR_FLAG_MACH_BIKE;
    else if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_ACRO_BIKE))
        gInitialPlayerAvatarState.transitionFlags = PLAYER_AVATAR_FLAG_ACRO_BIKE;
    else if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_SURFING))
        gInitialPlayerAvatarState.transitionFlags = PLAYER_AVATAR_FLAG_SURFING;
    else if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_UNDERWATER))
        gInitialPlayerAvatarState.transitionFlags = PLAYER_AVATAR_FLAG_UNDERWATER;
    else
        gInitialPlayerAvatarState.transitionFlags = PLAYER_AVATAR_FLAG_ON_FOOT;
}

static struct InitialPlayerAvatarState *GetInitialPlayerAvatarState(void)
{
    struct InitialPlayerAvatarState playerStruct;
    u8 mapType = Overworld_GetMapTypeOfSaveblockLocation();
    u16 metatileBehavior = GetCenterScreenMetatileBehavior();
    u8 transitionFlags = GetAdjustedInitialTransitionFlags(&gInitialPlayerAvatarState, metatileBehavior, mapType);
    playerStruct.transitionFlags = transitionFlags;
    playerStruct.direction = GetAdjustedInitialDirection(&gInitialPlayerAvatarState, transitionFlags, metatileBehavior, mapType);
    gInitialPlayerAvatarState = playerStruct;
    return &gInitialPlayerAvatarState;
}

static u8 GetAdjustedInitialTransitionFlags(struct InitialPlayerAvatarState *playerStruct, u16 metatileBehavior, u8 mapType)
{
    if (mapType != MAP_TYPE_INDOOR && FlagGet(FLAG_SYS_CRUISE_MODE))
        return PLAYER_AVATAR_FLAG_ON_FOOT;
    if (mapType == MAP_TYPE_UNDERWATER)
        return PLAYER_AVATAR_FLAG_UNDERWATER;
    if (MetatileBehavior_IsSurfableWaterOrUnderwater(metatileBehavior) == TRUE)
        return PLAYER_AVATAR_FLAG_SURFING;
    if (Overworld_IsBikingAllowed() != TRUE)
        return PLAYER_AVATAR_FLAG_ON_FOOT;
    if (playerStruct->transitionFlags == PLAYER_AVATAR_FLAG_MACH_BIKE)
        return PLAYER_AVATAR_FLAG_MACH_BIKE;
    if (playerStruct->transitionFlags != PLAYER_AVATAR_FLAG_ACRO_BIKE)
        return PLAYER_AVATAR_FLAG_ON_FOOT;
    return PLAYER_AVATAR_FLAG_ACRO_BIKE;
}

static u8 GetAdjustedInitialDirection(struct InitialPlayerAvatarState *playerStruct, u8 transitionFlags, u16 metatileBehavior, u8 mapType)
{
    if (FlagGet(FLAG_SYS_CRUISE_MODE) && mapType == MAP_TYPE_6)
        return DIR_EAST;
    if (MetatileBehavior_IsDeepSouthWarp(metatileBehavior) == TRUE)
        return DIR_NORTH;
    if (MetatileBehavior_IsNonAnimDoor(metatileBehavior) == TRUE || MetatileBehavior_IsDoor(metatileBehavior) == TRUE)
        return DIR_SOUTH;
    if (MetatileBehavior_IsSouthArrowWarp(metatileBehavior) == TRUE)
        return DIR_NORTH;
    if (MetatileBehavior_IsNorthArrowWarp(metatileBehavior) == TRUE)
        return DIR_SOUTH;
    if (MetatileBehavior_IsWestArrowWarp(metatileBehavior) == TRUE)
        return DIR_EAST;
    if (MetatileBehavior_IsEastArrowWarp(metatileBehavior) == TRUE)
        return DIR_WEST;
    if ((playerStruct->transitionFlags == PLAYER_AVATAR_FLAG_UNDERWATER && transitionFlags == PLAYER_AVATAR_FLAG_SURFING)
     || (playerStruct->transitionFlags == PLAYER_AVATAR_FLAG_SURFING && transitionFlags == PLAYER_AVATAR_FLAG_UNDERWATER))
        return playerStruct->direction;
    if (MetatileBehavior_IsLadder(metatileBehavior) == TRUE)
        return playerStruct->direction;
    return DIR_SOUTH;
}

u16 GetCenterScreenMetatileBehavior(void)
{
    return MapGridGetMetatileBehaviorAt(gSaveBlock1.pos.x + 7, gSaveBlock1.pos.y + 7);
}

bool32 Overworld_IsBikingAllowed(void)
{
    // is player in cycling road entrance?
    if (gSaveBlock1.location.mapGroup == MAP_GROUP(MAP_ROUTE110_SEASIDE_CYCLING_ROAD_SOUTH_ENTRANCE)
     && (gSaveBlock1.location.mapNum == MAP_NUM(MAP_ROUTE110_SEASIDE_CYCLING_ROAD_SOUTH_ENTRANCE)
      || gSaveBlock1.location.mapNum == MAP_NUM(MAP_ROUTE110_SEASIDE_CYCLING_ROAD_NORTH_ENTRANCE)))
        return TRUE;

    // is player indoor, in a secret base, or underwater?
    if (gMapHeader.mapType == MAP_TYPE_INDOOR)
        return FALSE;
    if (gMapHeader.mapType == MAP_TYPE_SECRET_BASE)
        return FALSE;
    if (gMapHeader.mapType == MAP_TYPE_UNDERWATER)
        return FALSE;

    // Thou shalt not bike on the sacred resting grounds of Kyogre/Groudon.
    if (gSaveBlock1.location.mapGroup == MAP_GROUP(MAP_SEAFLOOR_CAVERN_ROOM9)
     && gSaveBlock1.location.mapNum == MAP_NUM(MAP_SEAFLOOR_CAVERN_ROOM9))
        return FALSE;
    if (gSaveBlock1.location.mapGroup == MAP_GROUP(MAP_CAVE_OF_ORIGIN_B4F)
     && gSaveBlock1.location.mapNum == MAP_NUM(MAP_CAVE_OF_ORIGIN_B4F))
        return FALSE;

    return TRUE;
}

void SetDefaultFlashLevel(void)
{
    if (!gMapHeader.cave)
        gSaveBlock1.flashLevel = 0;
    else if (FlagGet(FLAG_SYS_USE_FLASH))
        gSaveBlock1.flashLevel = 1;
    else
        gSaveBlock1.flashLevel = gMaxFlashLevel;
}

void Overworld_SetFlashLevel(s32 flashLevel)
{
    if (flashLevel < 0 || flashLevel > gMaxFlashLevel)
        flashLevel = 0;
    gSaveBlock1.flashLevel = flashLevel;
}

u8 Overworld_GetFlashLevel(void)
{
    return gSaveBlock1.flashLevel;
}

void sub_8053D14(u16 mapLayoutId)
{
    gSaveBlock1.mapLayoutId = mapLayoutId;
    gMapHeader.mapLayout = GetMapLayout();
}

static bool16 ShouldLegendaryMusicPlayAtLocation(struct WarpData *warp)
{
    if (!FlagGet(FLAG_SYS_WEATHER_CTRL))
        return FALSE;
    if (warp->mapGroup == 0)
    {
        switch (warp->mapNum)
        {
        case MAP_NUM(MAP_LILYCOVE_CITY):
        case MAP_NUM(MAP_MOSSDEEP_CITY):
        case MAP_NUM(MAP_SOOTOPOLIS_CITY):
        case MAP_NUM(MAP_EVER_GRANDE_CITY):
            return TRUE;
        case MAP_NUM(MAP_ROUTE124):
        case MAP_NUM(MAP_ROUTE125):
        case MAP_NUM(MAP_ROUTE126):
        case MAP_NUM(MAP_ROUTE127):
        case MAP_NUM(MAP_ROUTE128):
            return TRUE;
        }
    }
    return FALSE;
}

static bool16 IsInfiltratedWeatherInstitute(struct WarpData *warp)
{
    if (VarGet(VAR_WEATHER_INSTITUTE_STATE))
        return FALSE;
    if (warp->mapGroup != MAP_GROUP(MAP_ROUTE119_WEATHER_INSTITUTE_1F))
        return FALSE;
    if (warp->mapNum == MAP_NUM(MAP_ROUTE119_WEATHER_INSTITUTE_1F)
     || warp->mapNum == MAP_NUM(MAP_ROUTE119_WEATHER_INSTITUTE_2F))
        return TRUE;
    return FALSE;
}

static u16 GetLocationMusic(struct WarpData *warp)
{
    if (ShouldLegendaryMusicPlayAtLocation(warp) == TRUE)
        return LEGENDARY_MUSIC;
    else if (IsInfiltratedWeatherInstitute(warp) == TRUE)
        return MUS_MT_CHIMNEY;
    else
        return Overworld_GetMapHeaderByGroupAndId(warp->mapGroup, warp->mapNum)->music;
}

u16 GetCurrLocationDefaultMusic(void)
{
    u16 music;

    // Play the desert music only when the sandstorm is active on Route 111.
    if (gSaveBlock1.location.mapGroup == MAP_GROUP(MAP_ROUTE111)
     && gSaveBlock1.location.mapNum == MAP_NUM(MAP_ROUTE111)
     && GetSavedWeather() == 8)
        return MUS_ROUTE111;

    music = GetLocationMusic(&gSaveBlock1.location);
    if (music != 0x7FFF)
    {
        return music;
    }
    else
    {
        if (gSaveBlock1.pos.x < 24)
            return MUS_ROUTE110;
        else
            return MUS_ROUTE119;
    }
}

u16 GetWarpDestinationMusic(void)
{
    u16 music = GetLocationMusic(&gWarpDestination);
    if (music != 0x7FFF)
    {
        return music;
    }
    else
    {
        if (gSaveBlock1.location.mapGroup == MAP_GROUP(MAP_MAUVILLE_CITY)
         && gSaveBlock1.location.mapNum == MAP_NUM(MAP_MAUVILLE_CITY))
            return MUS_ROUTE110;
        else
            return MUS_ROUTE119;
    }
}

void Overworld_ResetMapMusic(void)
{
    ResetMapMusic();
}

void Overworld_PlaySpecialMapMusic(void)
{
    u16 music = GetCurrLocationDefaultMusic();

    if (music != LEGENDARY_MUSIC)
    {
        if (gSaveBlock1.savedMusic)
            music = gSaveBlock1.savedMusic;
        else if (Overworld_GetMapTypeOfSaveblockLocation() == MAP_TYPE_UNDERWATER)
            music = MUS_UNDERWATER;
        else if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_SURFING))
            music = MUS_SURF;
    }

    if (music != GetCurrentMapMusic())
        PlayNewMapMusic(music);
}

void Overworld_SetSavedMusic(u16 songNum)
{
    gSaveBlock1.savedMusic = songNum;
}

void Overworld_ClearSavedMusic(void)
{
    gSaveBlock1.savedMusic = 0;
}

void sub_8053F0C(void)
{
    if (FlagGet(FLAG_DONT_TRANSITION_MUSIC) != TRUE)
    {
        u16 newMusic = GetWarpDestinationMusic();
        u16 currentMusic = GetCurrentMapMusic();
        if (newMusic != LEGENDARY_MUSIC)
        {
            if (currentMusic == MUS_UNDERWATER || currentMusic == MUS_SURF)
                return;
            if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_SURFING))
                newMusic = MUS_SURF;
        }
        if (newMusic != currentMusic)
        {
            if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_MACH_BIKE | PLAYER_AVATAR_FLAG_ACRO_BIKE))
                FadeOutAndFadeInNewMapMusic(newMusic, 4, 4);
            else
                FadeOutAndPlayNewMapMusic(newMusic, 8);
        }
    }
}

void Overworld_ChangeMusicToDefault(void)
{
    u16 currentMusic = GetCurrentMapMusic();
    if (currentMusic != GetCurrLocationDefaultMusic())
        FadeOutAndPlayNewMapMusic(GetCurrLocationDefaultMusic(), 8);
}

void Overworld_ChangeMusicTo(u16 newMusic)
{
    u16 currentMusic = GetCurrentMapMusic();
    if (currentMusic != newMusic && currentMusic != LEGENDARY_MUSIC)
        FadeOutAndPlayNewMapMusic(newMusic, 8);
}

u8 GetMapMusicFadeoutSpeed(void)
{
    const struct MapHeader *mapHeader = GetDestinationWarpMapHeader();
    if (Overworld_MapTypeIsIndoors(mapHeader->mapType) == TRUE)
        return 2;
    else
        return 4;
}

void TryFadeOutOldMapMusic(void)
{
    u16 music = GetWarpDestinationMusic();
    if (FlagGet(FLAG_DONT_TRANSITION_MUSIC) != TRUE && music != GetCurrentMapMusic())
    {
        u8 speed = GetMapMusicFadeoutSpeed();
        FadeOutMapMusic(speed);
    }
}

bool8 BGMusicStopped(void)
{
    return IsNotWaitingForBGMStop();
}

void Overworld_FadeOutMapMusic(void)
{
    FadeOutMapMusic(4);
}

static void PlayAmbientCry(void)
{
    s16 x, y;
    s8 pan;
    s8 volume;

    PlayerGetDestCoords(&x, &y);
    if (sIsAmbientCryWaterMon == TRUE
     && !MetatileBehavior_IsSurfableWaterOrUnderwater(MapGridGetMetatileBehaviorAt(x, y)))
        return;
    pan = (Random() % 88) + 212;
    volume = (Random() % 30) + 50;
    PlayCry2(sAmbientCrySpecies, pan, volume, 1);
}

void UpdateAmbientCry(s16 *state, u16 *delayCounter)
{
    switch (*state)
    {
    case 0:
        if (sAmbientCrySpecies == SPECIES_NONE)
            *state = 4;
        else
            *state = 1;
        break;
    case 1:
        *delayCounter = (Random() % 2400) + 1200;
        *state = 3;
        break;
    case 2:
        *delayCounter = (Random() % 1200) + 1200;
        *state = 3;
        break;
    case 3:
        (*delayCounter)--;
        if (*delayCounter == 0)
        {
            PlayAmbientCry();
            *state = 2;
        }
        break;
    case 4:
        break;
    }
}

static void ChooseAmbientCrySpecies(void)
{
    if ((gSaveBlock1.location.mapGroup == MAP_GROUP(MAP_ROUTE130)
     && gSaveBlock1.location.mapNum == MAP_NUM(MAP_ROUTE130))
     && !IsMirageIslandPresent())
    {
        // Only play water pokemon cries on this route
        // when Mirage Island is not present
        sIsAmbientCryWaterMon = TRUE;
        sAmbientCrySpecies = GetLocalWaterMon();
    }
    else
    {
        sAmbientCrySpecies = GetLocalWildMon(&sIsAmbientCryWaterMon);
    }
}

u8 GetMapTypeByGroupAndId(s8 mapGroup, s8 mapNum)
{
    return Overworld_GetMapHeaderByGroupAndId(mapGroup, mapNum)->mapType;
}

u8 GetMapTypeByWarpData(struct WarpData *warp)
{
    return GetMapTypeByGroupAndId(warp->mapGroup, warp->mapNum);
}

u8 Overworld_GetMapTypeOfSaveblockLocation(void)
{
    return GetMapTypeByWarpData(&gSaveBlock1.location);
}

u8 GetLastUsedWarpMapType(void)
{
    return GetMapTypeByWarpData(&gLastUsedWarp);
}

bool8 is_map_type_1_2_3_5_or_6(u8 mapType)
{
    if (mapType == MAP_TYPE_ROUTE
     || mapType == MAP_TYPE_TOWN
     || mapType == MAP_TYPE_UNDERWATER
     || mapType == MAP_TYPE_CITY
     || mapType == MAP_TYPE_6)
        return TRUE;
    else
        return FALSE;
}

bool8 Overworld_MapTypeAllowsTeleportAndFly(u8 mapType)
{
    if (mapType == MAP_TYPE_ROUTE
     || mapType == MAP_TYPE_TOWN
     || mapType == MAP_TYPE_6
     || mapType == MAP_TYPE_CITY)
        return TRUE;
    else
        return FALSE;
}

bool8 Overworld_MapTypeIsIndoors(u8 mapType)
{
    if (mapType == MAP_TYPE_INDOOR
     || mapType == MAP_TYPE_SECRET_BASE)
        return TRUE;
    else
        return FALSE;
}

u8 unref_sub_8054260(void)
{
    return Overworld_GetMapHeaderByGroupAndId(gSaveBlock1.dynamicWarp.mapGroup, gSaveBlock1.dynamicWarp.mapNum)->regionMapSectionId;
}

u8 sav1_map_get_name(void)
{
    return Overworld_GetMapHeaderByGroupAndId(gSaveBlock1.location.mapGroup, gSaveBlock1.location.mapNum)->regionMapSectionId;
}

u8 sav1_map_get_battletype(void)
{
    return Overworld_GetMapHeaderByGroupAndId(gSaveBlock1.location.mapGroup, gSaveBlock1.location.mapNum)->battleType;
}

#if DEBUG

void debug_sub_8076B68(void);

void debug_sub_80589D8(void);

void debug_sub_8058A50(void);

void CB2_InitTestMenu(void)
{
    m4aSoundVSyncOff();
    SetVBlankCallback(NULL);
    DmaFill32(3, 0, (void *) VRAM, VRAM_SIZE);
    DmaFill32(3, 0, (void *) PLTT, PLTT_SIZE);
    ResetPaletteFade();
    ResetSpriteData();
    ResetTasks();
    ScanlineEffect_Stop();
    Text_LoadWindowTemplate(&gMenuTextWindowTemplate);
    InitMenuWindow(&gMenuTextWindowTemplate);
    debug_sub_8076B68();
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
    REG_IE |= 1;
    REG_DISPCNT = DISPCNT_OBJ_ON | DISPCNT_BG0_ON | DISPCNT_OBJ_1D_MAP;
    m4aSoundVSyncOn();
    SetVBlankCallback(debug_sub_8058A50);
    m4aSongNumStart(0x19D);
    SetMainCallback2(debug_sub_80589D8);
}

void debug_sub_80589D8(void)
{
    if (UpdatePaletteFade())
        return;

    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
}

void debug_sub_80589F4(void)
{
    if (UpdatePaletteFade())
        return;

    SetVBlankCallback(NULL);

    DmaFill32(3, 0, (void *) VRAM, VRAM_SIZE);
    DmaFill32(3, 0, (void *) PLTT, PLTT_SIZE);

    SetMainCallback2(gMain.savedCallback);
}

void debug_sub_8058A50(void)
{
    ProcessSpriteCopyRequests();
    LoadOam();
    TransferPlttBuffer();
}

#endif

void ResetSafariZoneFlag_(void)
{
    ResetSafariZoneFlag();
}

bool32 is_c1_link_related_active(void)
{
    if (gMain.callback1 == CB1_OverworldLink)
        return TRUE;
    else
        return FALSE;
}

static void DoCB1_Overworld(u16 newKeys, u16 heldKeys)
{
    struct FieldInput fieldInput;

    UpdatePlayerAvatarTransitionState();
    ClearPlayerFieldInput(&fieldInput);
    GetPlayerFieldInput(&fieldInput, newKeys, heldKeys);
    if (!ArePlayerFieldControlsLocked())
    {
        if (ProcessPlayerFieldInput(&fieldInput) == 1)
        {
            LockPlayerFieldControls();
            HideMapNamePopup();
        }
        else
        {
            player_step(fieldInput.dpadDirection, newKeys, heldKeys);
        }
    }
}

static void CB1_Overworld(void)
{
    if (gMain.callback2 == CB2_Overworld)
        DoCB1_Overworld(gMain.newKeys, gMain.heldKeys);
}

static void OverworldBasic(void)
{
    ScriptContext_RunScript();
    RunTasks();
    AnimateSprites();
    CameraUpdate();
    UpdateCameraPanning();
    BuildOamBuffer();
    UpdatePaletteFade();
    sub_8072EDC();
}

// This CB2 is used when starting
void CB2_OverworldBasic(void)
{
    OverworldBasic();
}

static void CB2_Overworld(void)
{
    int fading = (gPaletteFade.active != 0);
    if (fading)
        SetVBlankCallback(NULL);
    OverworldBasic();
    if (fading)
        SetFieldVBlankCallback();
}

void SetMainCallback1(MainCallback cb)
{
    gMain.callback1 = cb;
}

void SetKeyInterceptCallback(u16 (*a1)(u32))
{
    sPlayerKeyInterceptCallback = a1;
}

static void RunFieldCallback(void)
{
    if (gFieldCallback != NULL)
        gFieldCallback();
    else
        FieldCB_DefaultWarpExit();
    gFieldCallback = NULL;
}

void CB2_NewGame(void)
{
    FieldClearVBlankHBlankCallbacks();
    StopMapMusic();
    ResetSafariZoneFlag_();
    NewGameInitData();
    ResetInitialPlayerAvatarState();
    PlayTimeCounter_Start();
    ScriptContext_Init();
    UnlockPlayerFieldControls();
    gFieldCallback = ExecuteTruckSequence;
    do_load_map_stuff_loop(&gMain.state);
    SetFieldVBlankCallback();
    SetMainCallback1(CB1_Overworld);
    SetMainCallback2(CB2_Overworld);
}

#if DEBUG

void debug_sub_8058C00(void)
{
    FieldClearVBlankHBlankCallbacks();
    StopMapMusic();
    ResetSafariZoneFlag_();
    ResetInitialPlayerAvatarState();
    PlayTimeCounter_Start();
    ScriptContext_Init();
    UnlockPlayerFieldControls();

    if (JOY_HELD(R_BUTTON))
        gFieldCallback = ExecuteTruckSequence;
    else
        gFieldCallback = FieldCB_WarpExitFadeFromBlack;

    do_load_map_stuff_loop(&gMain.state);
    SetFieldVBlankCallback();
    SetMainCallback1(CB1_Overworld);
    SetMainCallback2(CB2_Overworld);
}

#endif

void CB2_WhiteOut(void)
{
    u8 val;
    gMain.state++;
    if (gMain.state >= 120)
    {
        FieldClearVBlankHBlankCallbacks();
        StopMapMusic();
        ResetSafariZoneFlag_();
        DoWhiteOut();
        ResetInitialPlayerAvatarState();
        ScriptContext_Init();
        UnlockPlayerFieldControls();
        gFieldCallback = FieldCB_WarpExitFadeFromBlack;
        val = 0;
        do_load_map_stuff_loop(&val);
        SetFieldVBlankCallback();
        SetMainCallback1(CB1_Overworld);
        SetMainCallback2(CB2_Overworld);
    }
}

void CB2_LoadMap(void)
{
    FieldClearVBlankHBlankCallbacks();
    ScriptContext_Init();
    UnlockPlayerFieldControls();
    SetMainCallback1(NULL);
    SetMainCallback2(CB2_DoChangeMap);
    gMain.savedCallback = CB2_LoadMap2;
}

void CB2_LoadMap2(void)
{
    do_load_map_stuff_loop(&gMain.state);
    SetFieldVBlankCallback();
    SetMainCallback1(CB1_Overworld);
    SetMainCallback2(CB2_Overworld);
}

void CB2_ReturnToFieldContestHall(void)
{
    if (!gMain.state)
    {
        FieldClearVBlankHBlankCallbacks();
        ScriptContext_Init();
        UnlockPlayerFieldControls();
        SetMainCallback1(NULL);
    }
    if (sub_805493C(&gMain.state, 1))
    {
        SetFieldVBlankCallback();
        SetMainCallback1(CB1_Overworld);
        SetMainCallback2(CB2_Overworld);
    }
}

void sub_8054588(void)
{
    FieldClearVBlankHBlankCallbacks();
    gFieldCallback = FieldCB_ReturnToFieldWirelessLink;
    SetMainCallback2(c2_80567AC);
}

static void c2_80567AC(void)
{
    if (sub_805483C(&gMain.state))
    {
        SetFieldVBlankCallback();
        SetMainCallback1(CB1_OverworldLink);
        SetKeyInterceptCallback(KeyInterCB_SelfIdle);
        SetMainCallback2(CB2_Overworld);
    }
}

void CB2_ReturnToField(void)
{
    if (is_c1_link_related_active() == TRUE)
    {
        SetMainCallback2(CB2_ReturnToFieldLink);
    }
    else
    {
        FieldClearVBlankHBlankCallbacks();
        SetMainCallback2(CB2_ReturnToFieldLocal);
    }
}

void CB2_ReturnToFieldLocal(void)
{
    if (sub_8054A4C(&gMain.state))
    {
        SetFieldVBlankCallback();
        SetMainCallback2(CB2_Overworld);
    }
}

void CB2_ReturnToFieldLink(void)
{
    if (!Overworld_IsRecvQueueAtMax() && sub_8054A9C(&gMain.state))
        SetMainCallback2(CB2_Overworld);
}

void sub_805465C(void)
{
    FieldClearVBlankHBlankCallbacks();
    StopMapMusic();
    ResetAllPlayerLinkStates();
    SetMainCallback1(CB1_OverworldLink);
    SetKeyInterceptCallback(KeyInterCB_SelfIdle);
    gFieldCallback = FieldCB_ReturnToFieldCableLink;
    ScriptContext_Init();
    UnlockPlayerFieldControls();
    CB2_ReturnToField();
}

void CB2_ReturnToFieldWithOpenMenu(void)
{
    FieldClearVBlankHBlankCallbacks();
    gFieldCallback = ReturnToFieldOpenStartMenu;
    CB2_ReturnToField();
}

void CB2_ReturnToFieldContinueScript(void)
{
    FieldClearVBlankHBlankCallbacks();
    gFieldCallback = FieldCB_ContinueScript;
    CB2_ReturnToField();
}

void CB2_ReturnToFieldContinueScriptPlayMapMusic(void)
{
    FieldClearVBlankHBlankCallbacks();
    gFieldCallback = FieldCB_ContinueScriptHandleMusic;
    CB2_ReturnToField();
}

void sub_80546F0(void)
{
    FieldClearVBlankHBlankCallbacks();
    gFieldCallback = FieldCB_WarpExitFadeFromBlack;
    CB2_ReturnToField();
}

void sub_805470C(void)
{
    if (gMapHeader.flags == 1 && sub_80BBB24() == 1)
        ShowMapNamePopup();
    FieldCB_WarpExitFadeFromBlack();
}

void CB2_ContinueSavedGame(void)
{
    FieldClearVBlankHBlankCallbacks();
    StopMapMusic();
#if DEBUG
    if (JOY_HELD(R_BUTTON))
        gUnknown_020297ED = TRUE;
#endif
    ResetSafariZoneFlag_();
    LoadSaveblockMapHeader();
    LoadSaveblockObjEventScripts();
    UnfreezeObjectEvents();
    DoTimeBasedEvents();
    sub_805308C();
    InitMapFromSavedGame();
    PlayTimeCounter_Start();
    ScriptContext_Init();
    UnlockPlayerFieldControls();
    if (GetContinueGameWarpStatus() == 1)
    {
        ClearContinueGameWarpStatus();
        sub_8053778();
        WarpIntoMap();
        SetMainCallback2(CB2_LoadMap);
    }
    else
    {
        gFieldCallback = sub_805470C;
        SetMainCallback1(CB1_Overworld);
        CB2_ReturnToField();
    }
}

void FieldClearVBlankHBlankCallbacks(void)
{
    u16 savedIme = REG_IME;
    REG_IME = 0;
    REG_IE &= ~INTR_FLAG_HBLANK;
    REG_IE |= INTR_FLAG_VBLANK;
    REG_IME = savedIme;
    SetVBlankCallback(NULL);
    SetHBlankCallback(NULL);
}

static void SetFieldVBlankCallback(void)
{
    SetVBlankCallback(VBlankCB_Field);
}

static void VBlankCB_Field(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    ScanlineEffect_InitHBlankDmaTransfer();
    FieldUpdateBgTilemapScroll();
    TransferPlttBuffer();
    sub_8072E74();
}

static void InitCurrentFlashLevelScanlineEffect(void)
{
    u8 flashLevel = Overworld_GetFlashLevel();
    if (flashLevel)
    {
        WriteFlashScanlineEffectBuffer(flashLevel);
        ScanlineEffect_SetParams(sFlashEffectParams);
    }
}

static bool32 sub_805483C(u8 *state)
{
    switch (*state)
    {
    case 0:
        FieldClearVBlankHBlankCallbacks();
        ScriptContext_Init();
        UnlockPlayerFieldControls();
        ResetAllPlayerLinkStates();
        sub_8054BA8();
        (*state)++;
        break;
    case 1:
        sub_8053994(1);
        (*state)++;
        break;
    case 2:
        sub_8054D4C(1);
        (*state)++;
        break;
    case 3:
        sub_8054E98();
        sub_8054D90();
        sub_8054EC8();
        sub_8054E60();
        (*state)++;
        break;
    case 4:
        InitCurrentFlashLevelScanlineEffect();
        InitOverworldGraphicsRegisters();
        Text_LoadWindowTemplate(&gWindowTemplate_81E6C3C);
        InitMenuWindow(&gMenuTextWindowTemplate);
        (*state)++;
        break;
    case 5:
        ResetFieldCamera();
        (*state)++;
        break;
    case 6:
        CopyPrimaryTilesetToVram(gMapHeader.mapLayout);
        (*state)++;
        break;
    case 7:
        CopySecondaryTilesetToVram(gMapHeader.mapLayout);
        (*state)++;
        break;
    case 8:
        LoadMapTilesetPalettes(gMapHeader.mapLayout);
        (*state)++;
        break;
    case 9:
        DrawWholeMapView();
        (*state)++;
        break;
    case 10:
        cur_mapheader_run_tileset_funcs_after_some_cpuset();
        (*state)++;
        break;
    case 12:
        RunFieldCallback();
        (*state)++;
        break;
    case 11:
        (*state)++;
        break;
    case 13:
        return 1;
    }
    return 0;
}

bool32 sub_805493C(u8 *state, u32 a2)
{
    switch (*state)
    {
    case 0:
        FieldClearVBlankHBlankCallbacks();
        sub_8053994(a2);
        (*state)++;
        break;
    case 1:
        sub_8054BA8();
        (*state)++;
        break;
    case 2:
        sub_8054D4C(a2);
        (*state)++;
        break;
    case 3:
        mli4_mapscripts_and_other();
        sub_8054E34();
        (*state)++;
        break;
    case 4:
        InitCurrentFlashLevelScanlineEffect();
        InitOverworldGraphicsRegisters();
        Text_LoadWindowTemplate(&gWindowTemplate_81E6C3C);
        InitMenuWindow(&gMenuTextWindowTemplate);
        (*state)++;
        break;
    case 5:
        ResetFieldCamera();
        (*state)++;
        break;
    case 6:
        CopyPrimaryTilesetToVram(gMapHeader.mapLayout);
        (*state)++;
        break;
    case 7:
        CopySecondaryTilesetToVram(gMapHeader.mapLayout);
        (*state)++;
        break;
    case 8:
        LoadMapTilesetPalettes(gMapHeader.mapLayout);
        (*state)++;
        break;
    case 9:
        DrawWholeMapView();
        (*state)++;
        break;
    case 10:
        cur_mapheader_run_tileset_funcs_after_some_cpuset();
        (*state)++;
        break;
    case 11:
        if (gMapHeader.flags == 1 && sub_80BBB24() == 1)
            ShowMapNamePopup();
        (*state)++;
        break;
    case 12:
        RunFieldCallback();
        (*state)++;
        break;
    case 13:
        return 1;
    }
    return 0;
}

bool32 sub_8054A4C(u8 *state)
{
    switch (*state)
    {
    case 0:
        sub_8054BA8();
        sub_8054D4C(0);
        sub_8054E20();
        sub_8054E34();
        (*state)++;
        break;
    case 1:
        sub_8054C2C();
        (*state)++;
        break;
    case 2:
        RunFieldCallback();
        (*state)++;
        break;
    case 3:
        return 1;
    }
    return 0;
}

bool32 sub_8054A9C(u8 *state)
{
    switch (*state)
    {
    case 0:
        FieldClearVBlankHBlankCallbacks();
        sub_8054BA8();
        (*state)++;
        break;
    case 1:
        sub_8054D4C(1);
        (*state)++;
        break;
    case 2:
        sub_8054F48();
        sub_8054E20();
        sub_8054E7C();
        (*state)++;
        break;
    case 3:
        InitCurrentFlashLevelScanlineEffect();
        InitOverworldGraphicsRegisters();
        Text_LoadWindowTemplate(&gWindowTemplate_81E6C3C);
        InitMenuWindow(&gMenuTextWindowTemplate);
        (*state)++;
        break;
    case 4:
        ResetFieldCamera();
        (*state)++;
        break;
    case 5:
        CopyPrimaryTilesetToVram(gMapHeader.mapLayout);
        (*state)++;
        break;
    case 6:
        CopySecondaryTilesetToVram(gMapHeader.mapLayout);
        (*state)++;
        break;
    case 7:
        LoadMapTilesetPalettes(gMapHeader.mapLayout);
        (*state)++;
        break;
    case 8:
        DrawWholeMapView();
        (*state)++;
        break;
    case 9:
        cur_mapheader_run_tileset_funcs_after_some_cpuset();
        (*state)++;
        break;
    case 12:
        RunFieldCallback();
        (*state)++;
        break;
    case 10:
    case 11:
        (*state)++;
        break;
    case 13:
        SetFieldVBlankCallback();
        (*state)++;
        return 1;
    }
    return 0;
}

void do_load_map_stuff_loop(u8 *a1)
{
    while (!sub_805493C(a1, 0))
        ;
}

void sub_8054BA8(void)
{
    REG_DISPCNT = 0;

    ScanlineEffect_Stop();

    DmaClear16(3, PLTT + 2, PLTT_SIZE - 2);
    DmaFill16Large(3, 0, (void *)(VRAM + 0x0), 0x18000, 0x1000);
    ResetOamRange(0, 128);
    LoadOam();
}

void sub_8054C2C(void)
{
    InitCurrentFlashLevelScanlineEffect();
    InitOverworldGraphicsRegisters();
    Text_LoadWindowTemplate(&gWindowTemplate_81E6C3C);
    InitMenuWindow(&gMenuTextWindowTemplate);
    mapdata_load_assets_to_gpu_and_full_redraw();
}

static void InitOverworldGraphicsRegisters(void)
{
    REG_MOSAIC = 0;
    REG_WININ = 7967;
    REG_WINOUT = 257;
    REG_WIN0H = 255;
    REG_WIN0V = 255;
    REG_WIN1H = -1;
    REG_WIN1V = -1;
    REG_BLDCNT = gBgLayerBlendTarget2Flags[1] | gBgLayerBlendTarget2Flags[2] | gBgLayerBlendTarget2Flags[3] | 0x1040;
    REG_BLDALPHA = 1805;
    *gBGHOffsetRegs[0] = 0;
    *gBGVOffsetRegs[0] = 0;
    *gBGControlRegs[0] = 0;
    *gBGHOffsetRegs[1] = 0;
    *gBGVOffsetRegs[1] = 0;
    *gBGControlRegs[1] = 7489;
    *gBGHOffsetRegs[2] = 0;
    *gBGVOffsetRegs[2] = 0;
    *gBGControlRegs[2] = 7234;
    *gBGHOffsetRegs[3] = 0;
    *gBGVOffsetRegs[3] = 0;
    *gBGControlRegs[3] = 7747;
    REG_DISPCNT = gBgLayerDisplayFlags[1] | 0x7060 | gBgLayerDisplayFlags[2] | gBgLayerDisplayFlags[0] | gBgLayerDisplayFlags[3];
}

void sub_8054D4C(u32 a1)
{
    ResetTasks();
    ResetSpriteData();
    ResetPaletteFade();
    ScanlineEffect_Clear();
    ResetCameraUpdateInfo();
    InstallCameraPanAheadCallback();
    InitObjectEventPalettes(0);
    FieldEffectActiveListClear();
    InitFieldMessageBox();
    StartWeather();
    ResumePausedWeather();
    if (!a1)
        SetUpFieldTasks();
    RunOnResumeMapScript();
}

void sub_8054D90(void)
{
    gTotalCameraPixelOffsetX = 0;
    gTotalCameraPixelOffsetY = 0;
    ResetObjectEvents();
    TrySpawnObjectEvents(0, 0);
    TryRunOnWarpIntoMapScript();
}

void mli4_mapscripts_and_other(void)
{
    s16 x, y;
    struct InitialPlayerAvatarState *initialPlayerAvatarState;
    gTotalCameraPixelOffsetX = 0;
    gTotalCameraPixelOffsetY = 0;
    ResetObjectEvents();
    GetCameraFocusCoords(&x, &y);
    initialPlayerAvatarState = GetInitialPlayerAvatarState();
    InitPlayerAvatar(x, y, initialPlayerAvatarState->direction, gSaveBlock2.playerGender);
    SetPlayerAvatarTransitionFlags(initialPlayerAvatarState->transitionFlags);
    ResetInitialPlayerAvatarState();
    TrySpawnObjectEvents(0, 0);
    ResetBerryTreeSparkleFlags();
    TryRunOnWarpIntoMapScript();
}

void sub_8054E20(void)
{
    SpawnObjectEventsOnReturnToField(0, 0);
    RotatingGate_InitPuzzleAndGraphics();
}

void sub_8054E34(void)
{
    gObjectEvents[gPlayerAvatar.objectEventId].trackedByCamera = 1;
    InitCameraUpdateCallback(gPlayerAvatar.spriteId);
}

void sub_8054E60(void)
{
    InitCameraUpdateCallback(GetSpriteForLinkedPlayer(gLocalLinkPlayerId));
}

void sub_8054E7C(void)
{
    InitCameraUpdateCallback(GetSpriteForLinkedPlayer(gLocalLinkPlayerId));
}

void sub_8054E98(void)
{
    u16 x, y;
    GetCameraFocusCoords(&x, &y);
    SetCameraFocusCoords(x + gLocalLinkPlayerId, y);
}

void sub_8054EC8(void)
{
    u16 i;
    u16 x, y;

    GetCameraFocusCoords(&x, &y);
    x -= gLocalLinkPlayerId;

    for (i = 0; i < gFieldLinkPlayerCount; i++)
    {
        SpawnLinkPlayerObjectEvent(i, i + x, y, gLinkPlayers[i].gender);
        CreateLinkPlayerSprite(i);
    }

    ResetPlayerHeldKeys(word_3002910);
}

void sub_8054F48(void)
{
    u16 i;
    for (i = 0; i < gFieldLinkPlayerCount; i++)
        CreateLinkPlayerSprite(i);
}

void ResetAllPlayerLinkStates(void)
{
    int i;
    for (i = 0; i < 4; i++)
        sPlayerLinkStates[i] = PLAYER_LINK_STATE_IDLE;
}

bool32 AreAllPlayersInLinkState(u16 a1)
{
    int i;
    int count = gFieldLinkPlayerCount;

    for (i = 0; i < count; i++)
        if (sPlayerLinkStates[i] != a1)
            return FALSE;
    return TRUE;
}

bool32 IsAnyPlayerInLinkState(u16 a1)
{
    int i;
    int count = gFieldLinkPlayerCount;

    for (i = 0; i < count; i++)
        if (sPlayerLinkStates[i] == a1)
            return TRUE;
    return FALSE;
}

void HandleLinkPlayerKeyInput(u32 a1, u16 a2, struct CableClubPlayer *a3, u16 *a4)
{
    const u8 *script;

    if (sPlayerLinkStates[a1] == PLAYER_LINK_STATE_IDLE)
    {
        script = TryGetTileEventScript(a3);
        if (script)
        {
            *a4 = GetDirectionForEventScript(script);
            sPlayerLinkStates[a1] = PLAYER_LINK_STATE_BUSY;
            if (a3->isLocalPlayer)
            {
                SetKeyInterceptCallback(KeyInterCB_DeferToEventScript);
                RunInteractLocalPlayerScript(script);
            }
            return;
        }
        if (IsAnyPlayerInLinkState(PLAYER_LINK_STATE_EXITING_ROOM) == 1)
        {
            sPlayerLinkStates[a1] = PLAYER_LINK_STATE_BUSY;
            if (a3->isLocalPlayer)
            {
                SetKeyInterceptCallback(KeyInterCB_DeferToEventScript);
                RunTerminateLinkScript();
            }
            return;
        }
        switch (a2)
        {
        case LINK_KEY_CODE_START_BUTTON:
            if (CanCableClubPlayerPressStart(a3))
            {
                sPlayerLinkStates[a1] = PLAYER_LINK_STATE_BUSY;
                if (a3->isLocalPlayer)
                {
                    SetKeyInterceptCallback(KeyInterCB_DeferToEventScript);
                    InitLinkRoomStartMenuScript();
                }
            }
            break;
        case LINK_KEY_CODE_DPAD_DOWN:
            if (PlayerIsAtSouthExit(a3) == 1)
            {
                sPlayerLinkStates[a1] = PLAYER_LINK_STATE_BUSY;
                if (a3->isLocalPlayer)
                {
                    SetKeyInterceptCallback(KeyInterCB_DeferToEventScript);
                    RunConfirmLeaveCableClubScript();
                }
            }
            break;
        case LINK_KEY_CODE_A_BUTTON:
            script = TryInteractWithPlayer(a3);
            if (script)
            {
                sPlayerLinkStates[a1] = PLAYER_LINK_STATE_BUSY;
                if (a3->isLocalPlayer)
                {
                    SetKeyInterceptCallback(KeyInterCB_DeferToEventScript);
                    InitMenuBasedScript(script);
                }
            }
            break;
        case LINK_KEY_CODE_HANDLE_RECV_QUEUE:
            if (IsCableClubPlayerUnfrozen(a3))
            {
                sPlayerLinkStates[a1] = PLAYER_LINK_STATE_BUSY;
                if (a3->isLocalPlayer)
                {
                    SetKeyInterceptCallback(KeyInterCB_DeferToRecvQueue);
                    InitLinkPlayerQueueScript();
                }
            }
            break;
        case LINK_KEY_CODE_HANDLE_SEND_QUEUE:
            if (IsCableClubPlayerUnfrozen(a3))
            {
                sPlayerLinkStates[a1] = PLAYER_LINK_STATE_BUSY;
                if (a3->isLocalPlayer)
                {
                    SetKeyInterceptCallback(KeyInterCB_DeferToSendQueue);
                    InitLinkPlayerQueueScript();
                }
            }
            break;
        }
    }

    switch (a2)
    {
    case LINK_KEY_CODE_EXIT_ROOM:
        sPlayerLinkStates[a1] = PLAYER_LINK_STATE_EXITING_ROOM;
        break;
    case LINK_KEY_CODE_READY:
        sPlayerLinkStates[a1] = PLAYER_LINK_STATE_READY;
        break;
    case LINK_KEY_CODE_IDLE:
        sPlayerLinkStates[a1] = PLAYER_LINK_STATE_IDLE;
        if (a3->isLocalPlayer)
            SetKeyInterceptCallback(KeyInterCB_SelfIdle);
        break;
    case LINK_KEY_CODE_EXIT_SEAT:
        if (sPlayerLinkStates[a1] == PLAYER_LINK_STATE_READY)
            sPlayerLinkStates[a1] = PLAYER_LINK_STATE_BUSY;
        break;
    }
}

void UpdateAllLinkPlayers(u16 *a1, int a2)
{
    struct CableClubPlayer st;
    int i;
    for (i = 0; i < 4; i++)
    {
        u16 v5 = a1[i];
        u16 v8 = 0;
        LoadCableClubPlayer(i, a2, &st);
        HandleLinkPlayerKeyInput(i, v5, &st, &v8);
        if (sPlayerLinkStates[i] == PLAYER_LINK_STATE_IDLE)
            v8 = GetDirectionForDpadKey(v5);
        SetPlayerFacingDirection(i, v8);
    }
}

void UpdateHeldKeyCode(u16 a1)
{
    if (a1 >= LINK_KEY_CODE_EMPTY && a1 < LINK_KEY_CODE_UNK_8)
        gHeldKeyCodeToSend = a1;
    else
        gHeldKeyCodeToSend = LINK_KEY_CODE_EMPTY;
}

u16 KeyInterCB_ReadButtons(u32 a1)
{
    if (JOY_HELD(DPAD_UP))
    {
        return LINK_KEY_CODE_DPAD_UP;
    }
    else if (JOY_HELD(DPAD_DOWN))
    {
        return LINK_KEY_CODE_DPAD_DOWN;
    }
    else if (JOY_HELD(DPAD_LEFT))
    {
        return LINK_KEY_CODE_DPAD_LEFT;
    }
    else if (JOY_HELD(DPAD_RIGHT))
    {
        return LINK_KEY_CODE_DPAD_RIGHT;
    }
    else if (JOY_NEW(START_BUTTON))
    {
        return LINK_KEY_CODE_START_BUTTON;
    }
    else if (JOY_NEW(A_BUTTON))
    {
        return LINK_KEY_CODE_A_BUTTON;
    }
    else
    {
        return LINK_KEY_CODE_EMPTY;
    }
}

u16 GetDirectionForDpadKey(u16 a1)
{
    switch (a1)
    {
    case LINK_KEY_CODE_DPAD_RIGHT:
        return 4;
    case LINK_KEY_CODE_DPAD_LEFT:
        return 3;
    case LINK_KEY_CODE_DPAD_UP:
        return 1;
    case LINK_KEY_CODE_DPAD_DOWN:
        return 2;
    default:
        return 0;
    }
}

void ResetPlayerHeldKeys(u16 *a1)
{
    int i;
    for (i = 0; i < 4; i++)
        a1[i] = LINK_KEY_CODE_EMPTY;
}

void CB1_OverworldLink(void)
{
    u8 val = gLocalLinkPlayerId;
    UpdateAllLinkPlayers(word_3002910, val);
    UpdateHeldKeyCode(sPlayerKeyInterceptCallback(val));
    ResetPlayerHeldKeys(word_3002910);
}

u16 KeyInterCB_SelfIdle(u32 a1)
{
    if (ArePlayerFieldControlsLocked() == 1)
        return LINK_KEY_CODE_EMPTY;
    if (gLink.recvQueue.count > 4)
        return LINK_KEY_CODE_HANDLE_RECV_QUEUE;
    if (gLink.sendQueue.count <= 4)
        return KeyInterCB_ReadButtons(a1);
    return LINK_KEY_CODE_HANDLE_SEND_QUEUE;
}

u16 KeyInterCB_Idle(u32 a1)
{
    return LINK_KEY_CODE_EMPTY;
}

u16 KeyInterCB_DeferToEventScript(u32 a1)
{
    u16 retVal;
    if (ArePlayerFieldControlsLocked() == 1)
    {
        retVal = LINK_KEY_CODE_EMPTY;
    }
    else
    {
        retVal = LINK_KEY_CODE_IDLE;
        SetKeyInterceptCallback(KeyInterCB_Idle);
    }
    return retVal;
}

u16 KeyInterCB_DeferToRecvQueue(u32 a1)
{
    u16 retVal;
    if (gLink.recvQueue.count > 2)
    {
        retVal = LINK_KEY_CODE_EMPTY;
    }
    else
    {
        retVal = LINK_KEY_CODE_IDLE;
        UnlockPlayerFieldControls();
        SetKeyInterceptCallback(KeyInterCB_Idle);
    }
    return retVal;
}

u16 KeyInterCB_DeferToSendQueue(u32 a1)
{
    u16 retVal;
    if (gLink.sendQueue.count > 2)
    {
        retVal = LINK_KEY_CODE_EMPTY;
    }
    else
    {
        retVal = LINK_KEY_CODE_IDLE;
        UnlockPlayerFieldControls();
        SetKeyInterceptCallback(KeyInterCB_Idle);
    }
    return retVal;
}

u16 KeyInterCB_ExitingSeat(u32 a1)
{
    return LINK_KEY_CODE_EMPTY;
}

u16 KeyInterCB_Ready(u32 linkPlayerId)
{
    if (sPlayerLinkStates[linkPlayerId] == PLAYER_LINK_STATE_READY && JOY_NEW(B_BUTTON))
    {
        SetKeyInterceptCallback(KeyInterCB_ExitingSeat);
        return LINK_KEY_CODE_EXIT_SEAT;
    }
    else
    {
        return LINK_KEY_CODE_EMPTY;
    }
}

u16 KeyInterCB_SetReady(u32 a1)
{
    SetKeyInterceptCallback(KeyInterCB_Ready);
    return LINK_KEY_CODE_READY;
}

u16 KeyInterCB_SendNothing(u32 a1)
{
    return LINK_KEY_CODE_EMPTY;
}

u16 KeyInterCB_WaitForPlayersToExit(u32 a1)
{
    if (AreAllPlayersInLinkState(PLAYER_LINK_STATE_EXITING_ROOM) == TRUE)
    {
        ScriptContext_SetupScript(EventScript_DoLinkRoomExit);
        SetKeyInterceptCallback(KeyInterCB_SendNothing);
    }
    return LINK_KEY_CODE_EMPTY;
}

u16 KeyInterCB_SendExitRoomKey(u32 a1)
{
    SetKeyInterceptCallback(KeyInterCB_WaitForPlayersToExit);
    return LINK_KEY_CODE_EXIT_ROOM;
}

s32 GetCableClubPartnersReady(void)
{
    if (IsAnyPlayerInLinkState(PLAYER_LINK_STATE_EXITING_ROOM) == TRUE)
        return 2;
    if (sPlayerKeyInterceptCallback == KeyInterCB_Ready && sPlayerLinkStates[gLocalLinkPlayerId] != PLAYER_LINK_STATE_READY)
        return 0;
    if (sPlayerKeyInterceptCallback == KeyInterCB_ExitingSeat && sPlayerLinkStates[gLocalLinkPlayerId] == PLAYER_LINK_STATE_BUSY)
        return 2;
    return AreAllPlayersInLinkState(PLAYER_LINK_STATE_READY);
}

bool32 unref_sub_8055568(void)
{
    return IsAnyPlayerInLinkState(PLAYER_LINK_STATE_EXITING_ROOM);
}

u16 SetInCableClubSeat(void)
{
    SetKeyInterceptCallback(KeyInterCB_SetReady);
    return 0;
}

u16 SetLinkWaitingForScript(void)
{
    SetKeyInterceptCallback(KeyInterCB_DeferToEventScript);
    return 0;
}

u16 QueueExitLinkRoomKey(void)
{
    SetKeyInterceptCallback(KeyInterCB_SendExitRoomKey);
    return 0;
}

void LoadCableClubPlayer(int linkPlayerId, int a2, struct CableClubPlayer *a3)
{
    s16 x, y;

    a3->playerId = linkPlayerId;
    a3->isLocalPlayer = (linkPlayerId == a2) ? 1 : 0;
    a3->movementMode = gLinkPlayerObjectEvents[linkPlayerId].movementMode;
    a3->facing = GetLinkPlayerFacingDirection(linkPlayerId);
    GetLinkPlayerCoords(linkPlayerId, &x, &y);
    a3->pos.x = x;
    a3->pos.y = y;
    a3->pos.height = GetLinkPlayerElevation(linkPlayerId);
    a3->metatileBehavior = MapGridGetMetatileBehaviorAt(x, y);
}

bool32 IsCableClubPlayerUnfrozen(struct CableClubPlayer *a1)
{
    u8 v1 = a1->movementMode;
    if (v1 == MOVEMENT_MODE_SCRIPTED || v1 == MOVEMENT_MODE_FREE)
        return TRUE;
    else
        return FALSE;
}

bool32 CanCableClubPlayerPressStart(struct CableClubPlayer *a1)
{
    u8 v1 = a1->movementMode;
    if (v1 == MOVEMENT_MODE_SCRIPTED || v1 == MOVEMENT_MODE_FREE)
        return TRUE;
    else
        return FALSE;
}

const u8 *TryGetTileEventScript(struct CableClubPlayer *a1)
{
    if (a1->movementMode != MOVEMENT_MODE_SCRIPTED)
        return 0;
    return GetCoordEventScriptAtMapPosition(&a1->pos);
}

bool32 PlayerIsAtSouthExit(struct CableClubPlayer *a1)
{
    if (a1->movementMode != MOVEMENT_MODE_SCRIPTED && a1->movementMode != MOVEMENT_MODE_FREE)
        return FALSE;
    if (!MetatileBehavior_IsSouthArrowWarp(a1->metatileBehavior))
        return FALSE;
    if (a1->facing != DIR_SOUTH)
        return FALSE;
    return TRUE;
}

const u8 *TryInteractWithPlayer(struct CableClubPlayer *a1)
{
    struct MapPosition unkStruct;
    u8 linkPlayerId;

    if (a1->movementMode && a1->movementMode != MOVEMENT_MODE_SCRIPTED)
        return 0;

    unkStruct = a1->pos;
    unkStruct.x += gDirectionToVectors[a1->facing].x;
    unkStruct.y += gDirectionToVectors[a1->facing].y;
    unkStruct.height = 0;
    linkPlayerId = GetLinkPlayerIdAt(unkStruct.x, unkStruct.y);

    if (linkPlayerId != MAX_LINK_PLAYERS)
    {
        if (!a1->isLocalPlayer)
            return CableClub_EventScript_TooBusyToNotice;
        if (sPlayerLinkStates[linkPlayerId] != PLAYER_LINK_STATE_IDLE)
            return CableClub_EventScript_TooBusyToNotice;
        if (!GetLinkTrainerCardColor(linkPlayerId))
            return CableClub_EventScript_ReadTrainerCard;
        else
            return CableClub_EventScript_ReadTrainerCardColored;
    }

    return GetInteractedLinkPlayerScript(&unkStruct, a1->metatileBehavior, a1->facing);
}

static u16 GetDirectionForEventScript(const u8 *script)
{
    if (script == DoubleBattleColosseum_EventScript_PlayerSpot0)
        return FACING_FORCED_RIGHT;
    if (script == DoubleBattleColosseum_EventScript_PlayerSpot1)
        return FACING_FORCED_LEFT;
    if (script == DoubleBattleColosseum_EventScript_PlayerSpot2)
        return FACING_FORCED_RIGHT;
    if (script == DoubleBattleColosseum_EventScript_PlayerSpot3)
        return FACING_FORCED_LEFT;
    if (script == RecordCorner_EventScript_PlayerSpot0)
        return FACING_FORCED_RIGHT;
    if (script == RecordCorner_EventScript_PlayerSpot1)
        return FACING_FORCED_LEFT;
    if (script == RecordCorner_EventScript_PlayerSpot2)
        return FACING_FORCED_RIGHT;
    if (script == RecordCorner_EventScript_PlayerSpot3)
        return FACING_FORCED_LEFT;
    if (script == SingleBattleColosseum_EventScript_PlayerSpot0)
        return FACING_FORCED_RIGHT;
    if (script == SingleBattleColosseum_EventScript_PlayerSpot1)
        return FACING_FORCED_LEFT;
    if (script == TradeCenter_EventScript_Chair0)
        return FACING_FORCED_RIGHT;
    if (script == TradeCenter_EventScript_Chair1)
        return FACING_FORCED_LEFT;
    return FACING_NONE;
}

void InitLinkPlayerQueueScript(void)
{
    LockPlayerFieldControls();
}

void InitLinkRoomStartMenuScript(void)
{
    PlaySE(SE_WIN_OPEN);
    sub_8071310();
    LockPlayerFieldControls();
}

static void RunInteractLocalPlayerScript(const u8 *script)
{
    PlaySE(SE_SELECT);
    ScriptContext_SetupScript(script);
    LockPlayerFieldControls();
}

void RunConfirmLeaveCableClubScript(void)
{
    PlaySE(SE_WIN_OPEN);
    ScriptContext_SetupScript(EventScript_ConfirmLeaveCableClubRoom);
    LockPlayerFieldControls();
}

static void InitMenuBasedScript(const u8 *script)
{
    PlaySE(SE_SELECT);
    ScriptContext_SetupScript(script);
    LockPlayerFieldControls();
}

void RunTerminateLinkScript(void)
{
    ScriptContext_SetupScript(EventScript_TerminateLink);
    LockPlayerFieldControls();
}

bool32 Overworld_IsRecvQueueAtMax(void)
{
    if (!is_c1_link_related_active())
        return 0;
    if (gLink.recvQueue.count >= 3)
        sReceivingFromLink = 1;
    else
        sReceivingFromLink = 0;
    return sReceivingFromLink;
}

bool32 Overworld_RecvKeysFromLinkIsRunning(void)
{
    u8 temp;

    if (is_c1_link_related_active() != TRUE)
        return FALSE;

    if (IsSendingKeysToLink() != TRUE)
        return FALSE;

    if (sPlayerKeyInterceptCallback == KeyInterCB_DeferToRecvQueue)
        return TRUE;

    if (sPlayerKeyInterceptCallback != KeyInterCB_DeferToEventScript)
        return FALSE;

    temp = sReceivingFromLink;
    sReceivingFromLink = 0;

    if (temp == TRUE)
        return TRUE;

    if (gPaletteFade.active && gPaletteFade.softwareFadeFinishing)
        return TRUE;

    return FALSE;
}

bool32 Overworld_SendKeysToLinkIsRunning(void)
{
    if (is_c1_link_related_active() != TRUE)
        return FALSE;

    if (IsSendingKeysToLink() != TRUE)
        return FALSE;

    if (sPlayerKeyInterceptCallback == KeyInterCB_DeferToSendQueue)
        return TRUE;

    return FALSE;
}

bool32 IsSendingKeysOverCable(void)
{
    if (!IsSendingKeysToLink())
        return FALSE;
    return TRUE;
}

void ZeroLinkPlayerObjectEvent(struct LinkPlayerObjectEvent *linkPlayerObjEvent)
{
    memset(linkPlayerObjEvent, 0, sizeof(struct LinkPlayerObjectEvent));
}

void ClearLinkPlayerObjectEvents(void)
{
    memset(gLinkPlayerObjectEvents, 0, sizeof(gLinkPlayerObjectEvents));
}

static void ZeroObjectEvent(struct ObjectEvent *objEvent)
{
    memset(objEvent, 0, sizeof(struct ObjectEvent));
}

static void SpawnLinkPlayerObjectEvent(u8 linkPlayerId, s16 x, s16 y, u8 a4)
{
    u8 objEventId = GetFirstInactiveObjectEventId();
    struct LinkPlayerObjectEvent *linkPlayerObjEvent = &gLinkPlayerObjectEvents[linkPlayerId];
    struct ObjectEvent *objEvent = &gObjectEvents[objEventId];

    ZeroLinkPlayerObjectEvent(linkPlayerObjEvent);
    ZeroObjectEvent(objEvent);

    linkPlayerObjEvent->active = 1;
    linkPlayerObjEvent->linkPlayerId = linkPlayerId;
    linkPlayerObjEvent->objEventId = objEventId;
    linkPlayerObjEvent->movementMode = MOVEMENT_MODE_FREE;

    objEvent->active = 1;
    objEvent->singleMovementActive = a4;
    objEvent->range.as_byte = DIR_NORTH;
    objEvent->spriteId = MAX_SPRITES;

    InitLinkPlayerObjectEventPos(objEvent, x, y);
}

static void InitLinkPlayerObjectEventPos(struct ObjectEvent *objEvent, s16 x, s16 y)
{
    objEvent->currentCoords.x = x;
    objEvent->currentCoords.y = y;
    objEvent->previousCoords.x = x;
    objEvent->previousCoords.y = y;
    SetSpritePosToMapCoords(x, y, &objEvent->initialCoords.x, &objEvent->initialCoords.y);
    objEvent->initialCoords.x += 8;
    ObjectEventUpdateElevation(objEvent);
}

void SetLinkPlayerObjectRange(u8 linkPlayerId, u8 direction)
{
    if (gLinkPlayerObjectEvents[linkPlayerId].active)
    {
        u8 objEventId = gLinkPlayerObjectEvents[linkPlayerId].objEventId;
        struct ObjectEvent *objEvent = &gObjectEvents[objEventId];
        objEvent->range.as_byte = direction;
    }
}

void DestroyLinkPlayerObject(u8 linkPlayerId)
{
    struct LinkPlayerObjectEvent *linkPlayerObjEvent = &gLinkPlayerObjectEvents[linkPlayerId];
    u8 objEventId = linkPlayerObjEvent->objEventId;
    struct ObjectEvent *objEvent = &gObjectEvents[objEventId];
    if (objEvent->spriteId != MAX_SPRITES)
        DestroySprite(&gSprites[objEvent->spriteId]);
    linkPlayerObjEvent->active = 0;
    objEvent->active = 0;
}

u8 GetSpriteForLinkedPlayer(u8 linkPlayerId)
{
    u8 objEventId = gLinkPlayerObjectEvents[linkPlayerId].objEventId;
    struct ObjectEvent *objEvent = &gObjectEvents[objEventId];
    return objEvent->spriteId;
}

void GetLinkPlayerCoords(u8 linkPlayerId, u16 *x, u16 *y)
{
    u8 objEventId = gLinkPlayerObjectEvents[linkPlayerId].objEventId;
    struct ObjectEvent *objEvent = &gObjectEvents[objEventId];
    *x = objEvent->currentCoords.x;
    *y = objEvent->currentCoords.y;
}

u8 GetLinkPlayerFacingDirection(u8 linkPlayerId)
{
    u8 objEventId = gLinkPlayerObjectEvents[linkPlayerId].objEventId;
    struct ObjectEvent *objEvent = &gObjectEvents[objEventId];
    return objEvent->range.as_byte;
}

u8 GetLinkPlayerElevation(u8 linkPlayerId)
{
    u8 objEventId = gLinkPlayerObjectEvents[linkPlayerId].objEventId;
    struct ObjectEvent *objEvent = &gObjectEvents[objEventId];
    return objEvent->currentElevation;
}

s32 GetLinkPlayerObjectStepTimer(u8 linkPlayerId)
{
    u8 objEventId = gLinkPlayerObjectEvents[linkPlayerId].objEventId;
    struct ObjectEvent *objEvent = &gObjectEvents[objEventId];
    return 16 - (s8)objEvent->directionSequenceIndex;
}

static u8 GetLinkPlayerIdAt(s16 x, s16 y)
{
    u8 i;
    for (i = 0; i < MAX_LINK_PLAYERS; i++)
    {
        if (gLinkPlayerObjectEvents[i].active
         && (gLinkPlayerObjectEvents[i].movementMode == MOVEMENT_MODE_FREE || gLinkPlayerObjectEvents[i].movementMode == MOVEMENT_MODE_SCRIPTED))
        {
            struct ObjectEvent *objEvent = &gObjectEvents[gLinkPlayerObjectEvents[i].objEventId];
            if (objEvent->currentCoords.x == x && objEvent->currentCoords.y == y)
                return i;
        }
    }
    return MAX_LINK_PLAYERS;
}

void SetPlayerFacingDirection(u8 linkPlayerId, u8 facing)
{
    struct LinkPlayerObjectEvent *linkPlayerObjEvent = &gLinkPlayerObjectEvents[linkPlayerId];
    u8 objEventId = linkPlayerObjEvent->objEventId;
    struct ObjectEvent *objEvent = &gObjectEvents[objEventId];

    if (linkPlayerObjEvent->active)
    {
        if (facing > FACING_FORCED_RIGHT)
            objEvent->triggerGroundEffectsOnMove = 1;
        else
            sMovementStatusHandler[sLinkPlayerMovementModes[linkPlayerObjEvent->movementMode](linkPlayerObjEvent, objEvent, facing)](linkPlayerObjEvent, objEvent);
    }
}

static u8 MovementEventModeCB_Normal(struct LinkPlayerObjectEvent *linkPlayerObjEvent, struct ObjectEvent *objEvent, u8 direction)
{
    return sLinkPlayerFacingHandlers[direction](linkPlayerObjEvent, objEvent, direction);
}

static u8 MovementEventModeCB_Ignored(struct LinkPlayerObjectEvent *linkPlayerObjEvent, struct ObjectEvent *objEvent, u8 direction)
{
    return FACING_UP;
}

static u8 MovementEventModeCB_Scripted(struct LinkPlayerObjectEvent *linkPlayerObjEvent, struct ObjectEvent *objEvent, u8 direction)
{
    return sLinkPlayerFacingHandlers[direction](linkPlayerObjEvent, objEvent, direction);
}

static u8 FacingHandler_DoNothing(struct LinkPlayerObjectEvent *linkPlayerObjEvent, struct ObjectEvent *objEvent, u8 direction)
{
    return FALSE;
}

static u8 FacingHandler_DpadMovement(struct LinkPlayerObjectEvent *linkPlayerObjEvent, struct ObjectEvent *objEvent, u8 direction)
{
    s16 x, y;

    objEvent->range.as_byte = FlipVerticalAndClearForced(direction, objEvent->range.as_byte);
    ObjectEventMoveDestCoords(objEvent, objEvent->range.as_byte, &x, &y);

    if (LinkPlayerGetCollision(linkPlayerObjEvent->objEventId, objEvent->range.as_byte, x, y))
    {
        return 0;
    }
    else
    {
        objEvent->directionSequenceIndex = 16;
        ShiftObjectEventCoords(objEvent, x, y);
        ObjectEventUpdateElevation(objEvent);
        return 1;
    }
}

static u8 FacingHandler_ForcedFacingChange(struct LinkPlayerObjectEvent *linkPlayerObjEvent, struct ObjectEvent *objEvent, u8 direction)
{
    objEvent->range.as_byte = FlipVerticalAndClearForced(direction, objEvent->range.as_byte);
    return FALSE;
}

static void MovementStatusHandler_EnterFreeMode(struct LinkPlayerObjectEvent *linkPlayerObjEvent, struct ObjectEvent *objEvent)
{
    linkPlayerObjEvent->movementMode = MOVEMENT_MODE_FREE;
}

static void MovementStatusHandler_TryAdvanceScript(struct LinkPlayerObjectEvent *linkPlayerObjEvent, struct ObjectEvent *objEvent)
{
    objEvent->directionSequenceIndex--;
    linkPlayerObjEvent->movementMode = MOVEMENT_MODE_FROZEN;
    MoveCoords(objEvent->range.as_byte, &objEvent->initialCoords.x, &objEvent->initialCoords.y);
    if (!objEvent->directionSequenceIndex)
    {
        ShiftStillObjectEventCoords(objEvent);
        linkPlayerObjEvent->movementMode = MOVEMENT_MODE_SCRIPTED;
    }
}

static u8 FlipVerticalAndClearForced(u8 newFacing, u8 oldFacing)
{
    switch (newFacing - 1)
    {
    case FACING_UP - 1:
    case FACING_FORCED_UP - 1:
        return DIR_NORTH;
    case FACING_DOWN - 1:
    case FACING_FORCED_DOWN - 1:
        return DIR_SOUTH;
    case FACING_LEFT - 1:
    case FACING_FORCED_LEFT - 1:
        return DIR_WEST;
    case FACING_RIGHT - 1:
    case FACING_FORCED_RIGHT - 1:
        return DIR_EAST;
    }
    return oldFacing;
}

static u8 LinkPlayerGetCollision(u8 selfObjEventId, u8 direction, s16 x, s16 y)
{
    u8 i;
    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
    {
        if (i != selfObjEventId)
        {
            if ((gObjectEvents[i].currentCoords.x == x && gObjectEvents[i].currentCoords.y == y)
             || (gObjectEvents[i].previousCoords.x == x && gObjectEvents[i].previousCoords.y == y))
            {
                return 1;
            }
        }
    }
    return MapGridGetCollisionAt(x, y);
}

static void CreateLinkPlayerSprite(u8 linkPlayerId)
{
    struct LinkPlayerObjectEvent *linkPlayerObjEvent = &gLinkPlayerObjectEvents[linkPlayerId];
    u8 objEventId = linkPlayerObjEvent->objEventId;
    struct ObjectEvent *objEvent = &gObjectEvents[objEventId];
    struct Sprite *sprite;

    if (linkPlayerObjEvent->active)
    {
        u8 graphicsId = GetRivalAvatarGraphicsIdByStateIdAndGender(PLAYER_AVATAR_STATE_NORMAL, objEvent->singleMovementActive);
        objEvent->spriteId = AddPseudoObjectEvent(graphicsId, SpriteCB_LinkPlayer, 0, 0, 0);
        sprite = &gSprites[objEvent->spriteId];
        sprite->coordOffsetEnabled = TRUE;
        sprite->data[0] = linkPlayerId;
        objEvent->triggerGroundEffectsOnMove = FALSE;
    }
}

static void SpriteCB_LinkPlayer(struct Sprite *sprite)
{
    struct LinkPlayerObjectEvent *linkPlayerObjEvent = &gLinkPlayerObjectEvents[sprite->data[0]];
    struct ObjectEvent *objEvent = &gObjectEvents[linkPlayerObjEvent->objEventId];
    sprite->x = objEvent->initialCoords.x;
    sprite->y = objEvent->initialCoords.y;
    SetObjectSubpriorityByElevation(objEvent->previousElevation, sprite, 1);
    sprite->oam.priority = ElevationToPriority(objEvent->previousElevation);
    if (linkPlayerObjEvent->movementMode == MOVEMENT_MODE_FREE)
        StartSpriteAnim(sprite, GetFaceDirectionAnimNum(objEvent->range.as_byte));
    else
        StartSpriteAnimIfDifferent(sprite, GetMoveDirectionAnimNum(objEvent->range.as_byte));
    UpdateObjectEventSpriteVisibility(sprite, FALSE);
    if (objEvent->triggerGroundEffectsOnMove)
    {
        sprite->invisible = ((sprite->data[7] & 4) >> 2);
        sprite->data[7]++;
    }
}
