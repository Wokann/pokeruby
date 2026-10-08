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
#include "script_pokemon_util.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_camera.h"
#include "field_control_avatar.h"
#include "field_effect.h"
#include "field_screen_effect.h"
#include "field_message_box.h"
#include "field_player_avatar.h"
#include "field_special_scene.h"
#include "field_specials.h"
#include "field_tasks.h"
#include "field_weather.h"
#include "fieldmap.h"
#include "fldeff_flash.h"
#include "heal_location.h"
#include "io_reg.h"
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
#include "tileset_anims.h"
#include "time_events.h"
#include "tv.h"
#include "wild_encounter.h"

static void SetKeyInterceptCallback(u16 (*func)(u32));
static void SpawnLinkPlayers(void);
static void CreateLinkPlayerSprites(void);
static void UpdateAllLinkPlayers(u16 *keys, int selfId);
static void UpdateHeldKeyCode(u16 key);
static void CB1_OverworldLink(void);
static u16 KeyInterCB_SelfIdle(u32 key);
static u16 KeyInterCB_Idle(u32 key);
static u16 KeyInterCB_DeferToRecvQueue(u32 key);
static u16 KeyInterCB_DeferToSendQueue(u32 key);
static u16 KeyInterCB_ExitingSeat(u32 key);
static u16 KeyInterCB_Ready(u32 linkPlayerId);
static u16 KeyInterCB_WaitForPlayersToExit(u32 a1);


static u16 GetCenterScreenMetatileBehavior(void);
static void CB2_LoadMap2(void);
static void CB2_ReturnToFieldLocal(void);
static void CB2_ReturnToFieldLink(void);
static void FieldClearVBlankHBlankCallbacks(void);
static void InitObjectEventsLink(void);
static void SetCameraToTrackPlayer(void);
static void SetCameraToTrackGuestPlayer(void);
static void SetCameraToTrackGuestPlayer_2(void);
static void OffsetCameraFocusByLinkPlayerId(void);
static void ResetAllPlayerLinkStates(void);
static u16 GetDirectionForDpadKey(u16 key);
static void ResetPlayerHeldKeys(u16 *keys);
static u16 KeyInterCB_DeferToEventScript(u32 key);
static void InitLinkPlayerQueueScript(void);
static u8 GetSpriteForLinkedPlayer(u8 linkPlayerId);
static void GetLinkPlayerCoords(u8 linkPlayerId, u16 *x, u16 *y);
static u8 GetLinkPlayerFacingDirection(u8 linkPlayerId);
static u8 GetLinkPlayerElevation(u8 linkPlayerId);
static void SetPlayerFacingDirection(u8 linkPlayerId, u8 facing);
static void InitLinkRoomStartMenuScript(void);
static void RunConfirmLeaveCableClubScript(void);
static void RunTerminateLinkScript(void);

#ifdef SAPPHIRE
#define LEGENDARY_MUSIC MUS_WEATHER_KYOGRE  // Heavy Rain
#else
#define LEGENDARY_MUSIC MUS_WEATHER_GROUDON // Drought
#endif


extern u8 EventScript_WhiteOut[];
extern u8 EventScript_ResetMrBriney[];
extern u8 SingleBattleColosseum_EventScript_1A436F[];
extern u8 SingleBattleColosseum_EventScript_1A4379[];
extern u8 DoubleBattleColosseum_EventScript_1A4383[];
extern u8 DoubleBattleColosseum_EventScript_1A439E[];
extern u8 DoubleBattleColosseum_EventScript_1A43B9[];
extern u8 DoubleBattleColosseum_EventScript_1A43D4[];
extern u8 TradeCenter_EventScript_1A43F0[];
extern u8 TradeCenter_EventScript_1A43FA[];
extern u8 RecordCorner_EventScript_1A4418[];
extern u8 RecordCorner_EventScript_1A442D[];
extern u8 RecordCorner_EventScript_1A4442[];
extern u8 RecordCorner_EventScript_1A4457[];
extern u8 TradeRoom_ReadTrainerCard1[];
extern u8 TradeRoom_ReadTrainerCard2[];
extern u8 TradeRoom_TooBusyToNotice[];
extern u8 TradeRoom_PromptToCancelLink[];
extern u8 TradeRoom_TerminateLink[];
extern u8 gUnknown_081A4508[];

extern struct MapLayout * const gMapLayouts[];
extern struct MapHeader * const * const gMapGroups[];

u16 gHeldKeyCodeToSend;
void (*gFieldCallback)(void);
u8 gLocalLinkPlayerId;
u8 gFieldLinkPlayerCount;

EWRAM_DATA struct WarpData gLastUsedWarp = {0};
EWRAM_DATA static struct WarpData sWarpDestination = {0};
EWRAM_DATA static struct WarpData sFixedDiveWarp = {0};
EWRAM_DATA static struct WarpData sFixedHoleWarp = {0};
EWRAM_DATA static struct InitialPlayerAvatarState sInitialPlayerAvatarState = {0};
EWRAM_DATA static u16 sAmbientCrySpecies = 0;
EWRAM_DATA static bool8 sIsAmbientCryWaterMon = FALSE;
EWRAM_DATA struct LinkPlayerObjectEvent gLinkPlayerObjectEvents[4] = {0};

static u8 sPlayerLinkStates[4];
static u16 (*sPlayerKeyInterceptCallback)(u32);
static u8 sReceivingFromLink;

struct CableClubPlayer
{
    u8 playerId;
    u8 isLocalPlayer;
    u8 movementMode;
    u8 facing;
    struct MapPosition pos;
    u16 metatileBehavior;
};

static void HandleLinkPlayerKeyInput(u32 playerId, u16 key, struct CableClubPlayer *trainer, u16 *forceFacing);
static void LoadCableClubPlayer(int linkPlayerId, int myPlayerId, struct CableClubPlayer *trainer);
static bool32 IsCableClubPlayerUnfrozen(struct CableClubPlayer *player);
static bool32 CanCableClubPlayerPressStart(struct CableClubPlayer *player);
static const u8 *TryGetTileEventScript(struct CableClubPlayer *player);
static bool32 PlayerIsAtSouthExit(struct CableClubPlayer *player);
static const u8 *sub_805568C(struct CableClubPlayer *player);

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
static u16 sub_8055758(const u8 *);
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
    SetWarpDestinationToLastHealLocation();
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

static void UpdateMiscOverworldStates(void)
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

u32 GetGameStat(u8 index)
{
    if (index >= NUM_GAME_STATS)
        return 0;

    return gSaveBlock1.gameStats[index];
}

void SetGameStat(u8 index, u32 value)
{
    if (index < NUM_GAME_STATS)
        gSaveBlock1.gameStats[index] = value;
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

void SetObjEventTemplateCoords(u8 localId, s16 x, s16 y)
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

void SetObjEventTemplateMovementType(u8 localId, u8 movementType)
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
    move_tilemap_camera_to_upper_left_corner();
    CopyMapTilesetsToVram(gMapHeader.mapLayout);
    LoadMapTilesetPalettes(gMapHeader.mapLayout);
    DrawWholeMapView();
    InitTilesetAnimations();
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
    gSaveBlock1.location = sWarpDestination;
    sFixedDiveWarp = sDummyWarpData;
    sFixedHoleWarp = sDummyWarpData;
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
    return Overworld_GetMapHeaderByGroupAndId(sWarpDestination.mapGroup, sWarpDestination.mapNum);
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

void SetWarpDestination(s8 mapGroup, s8 mapNum, s8 warpId, s8 x, s8 y)
{
    SetWarpData(&sWarpDestination, mapGroup, mapNum, warpId, x, y);
}

void SetWarpDestinationToMapWarp(s8 mapGroup, s8 mapNum, s8 warpId)
{
    SetWarpDestination(mapGroup, mapNum, warpId, -1, -1);
}

void SetDynamicWarp(int unused, s8 mapGroup, s8 mapNum, s8 warpId)
{
    SetWarpData(&gSaveBlock1.dynamicWarp, mapGroup, mapNum, warpId, gSaveBlock1.pos.x, gSaveBlock1.pos.y);
}

void SetDynamicWarpWithCoords(int unused, s8 mapGroup, s8 mapNum, s8 warpId, s8 x, s8 y)
{
    SetWarpData(&gSaveBlock1.dynamicWarp, mapGroup, mapNum, warpId, x, y);
}

void SetWarpDestinationToDynamicWarp(u8 unusedWarpId)
{
    sWarpDestination = gSaveBlock1.dynamicWarp;
}

void SetWarpDestinationToHealLocation(u8 healLocationId)
{
    const struct HealLocation *healLocation = GetHealLocation(healLocationId);

    if (healLocation)
        SetWarpDestination(healLocation->mapGroup, healLocation->mapNum, -1, healLocation->x, healLocation->y);
}

void SetWarpDestinationToLastHealLocation(void)
{
    sWarpDestination = gSaveBlock1.lastHealLocation;
}

void SetLastHealLocationWarp(u8 healLocationId)
{
    const struct HealLocation *healLocation = GetHealLocation(healLocationId);

    if (healLocation != NULL)
        SetWarpData(&gSaveBlock1.lastHealLocation, healLocation->mapGroup, healLocation->mapNum, -1, healLocation->x, healLocation->y);
}

void UpdateEscapeWarp(s16 x, s16 y)
{
    u8 currMapType = GetCurrentMapType();
    u8 destMapType = GetMapTypeByGroupAndId(sWarpDestination.mapGroup, sWarpDestination.mapNum);
    if (IsMapTypeOutdoors(currMapType) && IsMapTypeOutdoors(destMapType) != TRUE)
        SetEscapeWarp(gSaveBlock1.location.mapGroup, gSaveBlock1.location.mapNum, -1, x - 7, y - 6);
}

void SetEscapeWarp(s8 mapGroup, s8 mapNum, s8 warpId, s8 x, s8 y)
{
    SetWarpData(&gSaveBlock1.escapeWarp, mapGroup, mapNum, warpId, x, y);
}

void SetWarpDestinationToEscapeWarp(void)
{
    sWarpDestination = gSaveBlock1.escapeWarp;
}

void SetFixedDiveWarp(s8 mapGroup, s8 mapNum, s8 warpId, s8 x, s8 y)
{
    SetWarpData(&sFixedDiveWarp, mapGroup, mapNum, warpId, x, y);
}

static void SetWarpDestinationToDiveWarp(void)
{
    sWarpDestination = sFixedDiveWarp;
}

void SetFixedHoleWarp(s8 mapGroup, s8 mapNum, s8 warpId, s8 x, s8 y)
{
    SetWarpData(&sFixedHoleWarp, mapGroup, mapNum, warpId, x, y);
}

void SetWarpDestinationToFixedHoleWarp(s16 x, s16 y)
{
    if (IsDummyWarp(&sFixedHoleWarp) == TRUE)
        sWarpDestination = gLastUsedWarp;
    else
        SetWarpDestination(sFixedHoleWarp.mapGroup, sFixedHoleWarp.mapNum, -1, x, y);
}

static void SetWarpDestinationToContinueGameWarp(void)
{
    sWarpDestination = gSaveBlock1.continueGameWarp;
}

void SetContinueGameWarp(s8 mapGroup, s8 mapNum, s8 warpId, s8 x, s8 y)
{
    SetWarpData(&gSaveBlock1.continueGameWarp, mapGroup, mapNum, warpId, x, y);
}

void SetContinueGameWarpToHealLocation(u8 healLocationId)
{
    const struct HealLocation *healLocation = GetHealLocation(healLocationId);
    if (healLocation)
        SetWarpData(&gSaveBlock1.continueGameWarp, healLocation->mapGroup, healLocation->mapNum, -1, healLocation->x, healLocation->y);
}

void SetContinueGameWarpToDynamicWarp()
{
    gSaveBlock1.continueGameWarp = gSaveBlock1.dynamicWarp;
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
        SetWarpDestination(connection->mapGroup, connection->mapNum, -1, x, y);
    }
    else
    {
        RunOnDiveWarpMapScript();
        if (IsDummyWarp(&sFixedDiveWarp))
            return FALSE;

        SetWarpDestinationToDiveWarp();
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

    SetWarpDestination(mapGroup, mapNum, -1, -1, -1);
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

    InitSecondaryTilesetAnimation();
    UpdateLocationHistoryForRoamer();
    RoamerMove();
    DoCurrentWeather();
    ResetFieldTasksArgs();
    RunOnResumeMapScript();
    ShowMapNamePopup();
}

void sub_8053994(u32 a1)
{
    bool8 isOutdoors;
    bool8 isIndoors;

    LoadCurrentMapData();
    LoadObjEventTemplatesFromHeader();
    isOutdoors = IsMapTypeOutdoors(gMapHeader.mapType);
    isIndoors = IsMapTypeIndoors(gMapHeader.mapType);
    ClearTempFieldEventData();
    ResetCyclingRoadChallengeData();
    RestartWildEncounterImmunitySteps();
    TryUpdateRandomTrainerRematches(gSaveBlock1.location.mapGroup, gSaveBlock1.location.mapNum);
    if (a1 != 1)
        DoTimeBasedEvents();
    SetSavedWeatherFromCurrMapHeader();
    ChooseAmbientCrySpecies();
    if (isOutdoors)
        FlagClear(FLAG_SYS_USE_FLASH);
    SetDefaultFlashLevel();
    Overworld_ClearSavedMusic();
    RunOnTransitionMapScript();
    UpdateLocationHistoryForRoamer();
    RoamerMoveToOtherLocationSet();
    InitMap();
    if (a1 != 1 && isIndoors)
    {
        UpdateTVScreensOnMap(gBackupMapLayout.width, gBackupMapLayout.height);
        InitSecretBaseAppearance(TRUE);
    }
}

void ResetInitialPlayerAvatarState(void)
{
    sInitialPlayerAvatarState.direction = DIR_SOUTH;
    sInitialPlayerAvatarState.transitionFlags = PLAYER_AVATAR_FLAG_ON_FOOT;
}

void StoreInitialPlayerAvatarState(void)
{
    sInitialPlayerAvatarState.direction = GetPlayerFacingDirection();

    if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_MACH_BIKE))
        sInitialPlayerAvatarState.transitionFlags = PLAYER_AVATAR_FLAG_MACH_BIKE;
    else if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_ACRO_BIKE))
        sInitialPlayerAvatarState.transitionFlags = PLAYER_AVATAR_FLAG_ACRO_BIKE;
    else if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_SURFING))
        sInitialPlayerAvatarState.transitionFlags = PLAYER_AVATAR_FLAG_SURFING;
    else if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_UNDERWATER))
        sInitialPlayerAvatarState.transitionFlags = PLAYER_AVATAR_FLAG_UNDERWATER;
    else
        sInitialPlayerAvatarState.transitionFlags = PLAYER_AVATAR_FLAG_ON_FOOT;
}

static struct InitialPlayerAvatarState *GetInitialPlayerAvatarState(void)
{
    struct InitialPlayerAvatarState playerStruct;
    u8 mapType = GetCurrentMapType();
    u16 metatileBehavior = GetCenterScreenMetatileBehavior();
    u8 transitionFlags = GetAdjustedInitialTransitionFlags(&sInitialPlayerAvatarState, metatileBehavior, mapType);
    playerStruct.transitionFlags = transitionFlags;
    playerStruct.direction = GetAdjustedInitialDirection(&sInitialPlayerAvatarState, transitionFlags, metatileBehavior, mapType);
    sInitialPlayerAvatarState = playerStruct;
    return &sInitialPlayerAvatarState;
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
    if (FlagGet(FLAG_SYS_CRUISE_MODE) && mapType == MAP_TYPE_OCEAN_ROUTE)
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

static u16 GetCenterScreenMetatileBehavior(void)
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

void SetFlashLevel(s32 flashLevel)
{
    if (flashLevel < 0 || flashLevel > gMaxFlashLevel)
        flashLevel = 0;
    gSaveBlock1.flashLevel = flashLevel;
}

u8 GetFlashLevel(void)
{
    return gSaveBlock1.flashLevel;
}

void SetCurrentMapLayout(u16 mapLayoutId)
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
        return MUS_DESERT;

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
    u16 music = GetLocationMusic(&sWarpDestination);
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
        else if (GetCurrentMapType() == MAP_TYPE_UNDERWATER)
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
    if (IsMapTypeIndoors(mapHeader->mapType) == TRUE)
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
    PlayCry_NormalNoDucking(sAmbientCrySpecies, pan, volume, 1);
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

u8 GetCurrentMapType(void)
{
    return GetMapTypeByWarpData(&gSaveBlock1.location);
}

u8 GetLastUsedWarpMapType(void)
{
    return GetMapTypeByWarpData(&gLastUsedWarp);
}

bool8 IsMapTypeOutdoors(u8 mapType)
{
    if (mapType == MAP_TYPE_ROUTE
     || mapType == MAP_TYPE_TOWN
     || mapType == MAP_TYPE_UNDERWATER
     || mapType == MAP_TYPE_CITY
     || mapType == MAP_TYPE_OCEAN_ROUTE)
        return TRUE;
    else
        return FALSE;
}

bool8 Overworld_MapTypeAllowsTeleportAndFly(u8 mapType)
{
    if (mapType == MAP_TYPE_ROUTE
     || mapType == MAP_TYPE_TOWN
     || mapType == MAP_TYPE_OCEAN_ROUTE
     || mapType == MAP_TYPE_CITY)
        return TRUE;
    else
        return FALSE;
}

bool8 IsMapTypeIndoors(u8 mapType)
{
    if (mapType == MAP_TYPE_INDOOR
     || mapType == MAP_TYPE_SECRET_BASE)
        return TRUE;
    else
        return FALSE;
}

u8 GetSavedWarpRegionMapSectionId(void)
{
    return Overworld_GetMapHeaderByGroupAndId(gSaveBlock1.dynamicWarp.mapGroup, gSaveBlock1.dynamicWarp.mapNum)->regionMapSectionId;
}

u8 GetCurrentRegionMapSectionId(void)
{
    return Overworld_GetMapHeaderByGroupAndId(gSaveBlock1.location.mapGroup, gSaveBlock1.location.mapNum)->regionMapSectionId;
}

u8 GetCurrentMapBattleScene(void)
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

static void ResetSafariZoneFlag_(void)
{
    ResetSafariZoneFlag();
}

bool32 IsOverworldLinkActive(void)
{
    if (gMain.callback1 == CB1_OverworldLink)
        return TRUE;
    else
        return FALSE;
}

static void DoCB1_Overworld(u16 newKeys, u16 heldKeys)
{
    struct FieldInput inputStruct;

    UpdatePlayerAvatarTransitionState();
    FieldClearPlayerInput(&inputStruct);
    GetPlayerFieldInput(&inputStruct, newKeys, heldKeys);
    if (!ArePlayerFieldControlsLocked())
    {
        if (ProcessPlayerFieldInput(&inputStruct) == 1)
        {
            LockPlayerFieldControls();
            HideMapNamePopUpWindow();
        }
        else
        {
            PlayerStep(inputStruct.dpadDirection, newKeys, heldKeys);
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
    UpdateTilesetAnimations();
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

static void SetKeyInterceptCallback(u16 (*func)(u32))
{
    sPlayerKeyInterceptCallback = func;
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
    u8 state;
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
        state = 0;
        do_load_map_stuff_loop(&state);
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
    SetMainCallback2(sub_810CC80);
    gMain.savedCallback = CB2_LoadMap2;
}

static void CB2_LoadMap2(void)
{
    do_load_map_stuff_loop(&gMain.state);
    SetFieldVBlankCallback();
    SetMainCallback1(CB1_Overworld);
    SetMainCallback2(CB2_Overworld);
}

void sub_8054534(void)
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
    gFieldCallback = sub_8080AC4;
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
    if (IsOverworldLinkActive() == TRUE)
    {
        SetMainCallback2(CB2_ReturnToFieldLink);
    }
    else
    {
        FieldClearVBlankHBlankCallbacks();
        SetMainCallback2(CB2_ReturnToFieldLocal);
    }
}

static void CB2_ReturnToFieldLocal(void)
{
    if (sub_8054A4C(&gMain.state))
    {
        SetFieldVBlankCallback();
        SetMainCallback2(CB2_Overworld);
    }
}

static void CB2_ReturnToFieldLink(void)
{
    if (!Overworld_IsRecvQueueAtMax() && sub_8054A9C(&gMain.state))
        SetMainCallback2(CB2_Overworld);
}

void CB2_ReturnToFieldFromMultiplayer(void)
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

void CB2_ReturnToFieldFadeFromBlack(void)
{
    FieldClearVBlankHBlankCallbacks();
    gFieldCallback = FieldCB_WarpExitFadeFromBlack;
    CB2_ReturnToField();
}

void sub_805470C(void)
{
    if (gMapHeader.showMapName == 1 && SecretBaseMapPopupEnabled() == 1)
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
    UpdateMiscOverworldStates();
    InitMapFromSavedGame();
    PlayTimeCounter_Start();
    ScriptContext_Init();
    UnlockPlayerFieldControls();
    if (GetSecretBase2Field_9() == 1)
    {
        ClearSecretBase2Field_9();
        SetWarpDestinationToContinueGameWarp();
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

static void FieldClearVBlankHBlankCallbacks(void)
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
    sub_8057A58();
    TransferPlttBuffer();
    TransferTilesetAnimsBuffer();
}

static void InitCurrentFlashLevelScanlineEffect(void)
{
    u8 flashLevel = GetFlashLevel();
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
        OffsetCameraFocusByLinkPlayerId();
        InitObjectEventsLink();
        SpawnLinkPlayers();
        SetCameraToTrackGuestPlayer();
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
        move_tilemap_camera_to_upper_left_corner();
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
        InitTilesetAnimations();
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
        SetCameraToTrackPlayer();
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
        move_tilemap_camera_to_upper_left_corner();
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
        InitTilesetAnimations();
        (*state)++;
        break;
    case 11:
        if (gMapHeader.showMapName == 1 && SecretBaseMapPopupEnabled() == 1)
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
        SetCameraToTrackPlayer();
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
        CreateLinkPlayerSprites();
        sub_8054E20();
        SetCameraToTrackGuestPlayer_2();
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
        move_tilemap_camera_to_upper_left_corner();
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
        InitTilesetAnimations();
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

void do_load_map_stuff_loop(u8 *state)
{
    while (!sub_805493C(state, 0))
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
    REG_BLDCNT = gOverworldBackgroundLayerFlags[1] | gOverworldBackgroundLayerFlags[2] | gOverworldBackgroundLayerFlags[3] | 0x1040;
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
    REG_DISPCNT = gUnknown_081E29D8[1] | 0x7060 | gUnknown_081E29D8[2] | gUnknown_081E29D8[0] | gUnknown_081E29D8[3];
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

static void InitObjectEventsLink(void)
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
    SetBerryTreesSeen();
    TryRunOnWarpIntoMapScript();
}

void sub_8054E20(void)
{
    sub_805B710(0, 0);
    RotatingGate_InitPuzzleAndGraphics();
}

static void SetCameraToTrackPlayer(void)
{
    gObjectEvents[gPlayerAvatar.objectEventId].trackedByCamera = 1;
    InitCameraUpdateCallback(gPlayerAvatar.spriteId);
}

static void SetCameraToTrackGuestPlayer(void)
{
    InitCameraUpdateCallback(GetSpriteForLinkedPlayer(gLocalLinkPlayerId));
}

static void SetCameraToTrackGuestPlayer_2(void)
{
    InitCameraUpdateCallback(GetSpriteForLinkedPlayer(gLocalLinkPlayerId));
}

static void OffsetCameraFocusByLinkPlayerId(void)
{
    u16 x, y;
    GetCameraFocusCoords(&x, &y);
    SetCameraFocusCoords(x + gLocalLinkPlayerId, y);
}

static void SpawnLinkPlayers(void)
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

    ResetPlayerHeldKeys(gLinkPartnersHeldKeys);
}

static void CreateLinkPlayerSprites(void)
{
    u16 i;
    for (i = 0; i < gFieldLinkPlayerCount; i++)
        CreateLinkPlayerSprite(i);
}

static void ResetAllPlayerLinkStates(void)
{
    int i;
    for (i = 0; i < 4; i++)
        sPlayerLinkStates[i] = 0x80;
}

static bool32 AreAllPlayersInLinkState(u16 state)
{
    int i;
    int count = gFieldLinkPlayerCount;

    for (i = 0; i < count; i++)
        if (sPlayerLinkStates[i] != state)
            return FALSE;
    return TRUE;
}

static bool32 IsAnyPlayerInLinkState(u16 state)
{
    int i;
    int count = gFieldLinkPlayerCount;

    for (i = 0; i < count; i++)
        if (sPlayerLinkStates[i] == state)
            return TRUE;
    return FALSE;
}

static void HandleLinkPlayerKeyInput(u32 playerId, u16 key, struct CableClubPlayer *trainer, u16 *forceFacing)
{
    const u8 *script;

    if (sPlayerLinkStates[playerId] == 0x80)
    {
        script = TryGetTileEventScript(trainer);
        if (script)
        {
            *forceFacing = sub_8055758(script);
            sPlayerLinkStates[playerId] = 0x81;
            if (trainer->isLocalPlayer)
            {
                SetKeyInterceptCallback(KeyInterCB_DeferToEventScript);
                RunInteractLocalPlayerScript(script);
            }
            return;
        }
        if (IsAnyPlayerInLinkState(0x83) == 1)
        {
            sPlayerLinkStates[playerId] = 0x81;
            if (trainer->isLocalPlayer)
            {
                SetKeyInterceptCallback(KeyInterCB_DeferToEventScript);
                RunTerminateLinkScript();
            }
            return;
        }
        switch (key)
        {
        case 24:
            if (CanCableClubPlayerPressStart(trainer))
            {
                sPlayerLinkStates[playerId] = 0x81;
                if (trainer->isLocalPlayer)
                {
                    SetKeyInterceptCallback(KeyInterCB_DeferToEventScript);
                    InitLinkRoomStartMenuScript();
                }
            }
            break;
        case 18:
            if (PlayerIsAtSouthExit(trainer) == 1)
            {
                sPlayerLinkStates[playerId] = 0x81;
                if (trainer->isLocalPlayer)
                {
                    SetKeyInterceptCallback(KeyInterCB_DeferToEventScript);
                    RunConfirmLeaveCableClubScript();
                }
            }
            break;
        case 25:
            script = sub_805568C(trainer);
            if (script)
            {
                sPlayerLinkStates[playerId] = 0x81;
                if (trainer->isLocalPlayer)
                {
                    SetKeyInterceptCallback(KeyInterCB_DeferToEventScript);
                    InitMenuBasedScript(script);
                }
            }
            break;
        case 27:
            if (IsCableClubPlayerUnfrozen(trainer))
            {
                sPlayerLinkStates[playerId] = 0x81;
                if (trainer->isLocalPlayer)
                {
                    SetKeyInterceptCallback(KeyInterCB_DeferToRecvQueue);
                    InitLinkPlayerQueueScript();
                }
            }
            break;
        case 28:
            if (IsCableClubPlayerUnfrozen(trainer))
            {
                sPlayerLinkStates[playerId] = 0x81;
                if (trainer->isLocalPlayer)
                {
                    SetKeyInterceptCallback(KeyInterCB_DeferToSendQueue);
                    InitLinkPlayerQueueScript();
                }
            }
            break;
        }
    }

    switch (key)
    {
    case 23:
        sPlayerLinkStates[playerId] = 0x83;
        break;
    case 22:
        sPlayerLinkStates[playerId] = 0x82;
        break;
    case 26:
        sPlayerLinkStates[playerId] = 0x80;
        if (trainer->isLocalPlayer)
            SetKeyInterceptCallback(KeyInterCB_SelfIdle);
        break;
    case 29:
        if (sPlayerLinkStates[playerId] == 0x82)
            sPlayerLinkStates[playerId] = 0x81;
        break;
    }
}

static void UpdateAllLinkPlayers(u16 *keys, int selfId)
{
    struct CableClubPlayer trainer;
    int i;
    for (i = 0; i < 4; i++)
    {
        u16 key = keys[i];
        u16 setFacing = 0;
        LoadCableClubPlayer(i, selfId, &trainer);
        HandleLinkPlayerKeyInput(i, key, &trainer, &setFacing);
        if (sPlayerLinkStates[i] == 0x80)
            setFacing = GetDirectionForDpadKey(key);
        SetPlayerFacingDirection(i, setFacing);
    }
}

static void UpdateHeldKeyCode(u16 key)
{
    if (key >= 17 && key < 30)
        gHeldKeyCodeToSend = key;
    else
        gHeldKeyCodeToSend = 17;
}

static u16 KeyInterCB_ReadButtons(u32 key)
{
    if (JOY_HELD(DPAD_UP))
    {
        return 19;
    }
    else if (JOY_HELD(DPAD_DOWN))
    {
        return 18;
    }
    else if (JOY_HELD(DPAD_LEFT))
    {
        return 20;
    }
    else if (JOY_HELD(DPAD_RIGHT))
    {
        return 21;
    }
    else if (JOY_NEW(START_BUTTON))
    {
        return 24;
    }
    else if (JOY_NEW(A_BUTTON))
    {
        return 25;
    }
    else
    {
        return 17;
    }
}

static u16 GetDirectionForDpadKey(u16 key)
{
    switch (key)
    {
    case 21:
        return 4;
    case 20:
        return 3;
    case 19:
        return 1;
    case 18:
        return 2;
    default:
        return 0;
    }
}

static void ResetPlayerHeldKeys(u16 *keys)
{
    int i;
    for (i = 0; i < 4; i++)
        keys[i] = 17;
}

static void CB1_OverworldLink(void)
{
    u8 selfId = gLocalLinkPlayerId;
    UpdateAllLinkPlayers(gLinkPartnersHeldKeys, selfId);
    UpdateHeldKeyCode(sPlayerKeyInterceptCallback(selfId));
    ResetPlayerHeldKeys(gLinkPartnersHeldKeys);
}

static u16 KeyInterCB_SelfIdle(u32 key)
{
    if (ArePlayerFieldControlsLocked() == 1)
        return 17;
    if (gLink.recvQueue.count > 4)
        return 27;
    if (gLink.sendQueue.count <= 4)
        return KeyInterCB_ReadButtons(key);
    return 28;
}

static u16 KeyInterCB_Idle(u32 key)
{
    return 17;
}

static u16 KeyInterCB_DeferToEventScript(u32 key)
{
    u16 retVal;
    if (ArePlayerFieldControlsLocked() == 1)
    {
        retVal = 17;
    }
    else
    {
        retVal = 26;
        SetKeyInterceptCallback(KeyInterCB_Idle);
    }
    return retVal;
}

static u16 KeyInterCB_DeferToRecvQueue(u32 key)
{
    u16 retVal;
    if (gLink.recvQueue.count > 2)
    {
        retVal = 17;
    }
    else
    {
        retVal = 26;
        UnlockPlayerFieldControls();
        SetKeyInterceptCallback(KeyInterCB_Idle);
    }
    return retVal;
}

static u16 KeyInterCB_DeferToSendQueue(u32 key)
{
    u16 retVal;
    if (gLink.sendQueue.count > 2)
    {
        retVal = 17;
    }
    else
    {
        retVal = 26;
        UnlockPlayerFieldControls();
        SetKeyInterceptCallback(KeyInterCB_Idle);
    }
    return retVal;
}

static u16 KeyInterCB_ExitingSeat(u32 key)
{
    return 17;
}

static u16 KeyInterCB_Ready(u32 linkPlayerId)
{
    if (sPlayerLinkStates[linkPlayerId] == 0x82 && JOY_NEW(B_BUTTON))
    {
        SetKeyInterceptCallback(KeyInterCB_ExitingSeat);
        return 29;
    }
    else
    {
        return 17;
    }
}

static u16 KeyInterCB_SetReady(u32 key)
{
    SetKeyInterceptCallback(KeyInterCB_Ready);
    return 22;
}

static u16 KeyInterCB_SendNothing(u32 key)
{
    return 17;
}

static u16 KeyInterCB_WaitForPlayersToExit(u32 a1)
{
    if (AreAllPlayersInLinkState(0x83) == TRUE)
    {
        ScriptContext_SetupScript(gUnknown_081A4508);
        SetKeyInterceptCallback(KeyInterCB_SendNothing);
    }
    return 17;
}

static u16 KeyInterCB_SendExitRoomKey(u32 key)
{
    SetKeyInterceptCallback(KeyInterCB_WaitForPlayersToExit);
    return 23;
}

s32 GetCableClubPartnersReady(void)
{
    if (IsAnyPlayerInLinkState(0x83) == TRUE)
        return 2;
    if (sPlayerKeyInterceptCallback == KeyInterCB_Ready && sPlayerLinkStates[gLocalLinkPlayerId] != 0x82)
        return 0;
    if (sPlayerKeyInterceptCallback == KeyInterCB_ExitingSeat && sPlayerLinkStates[gLocalLinkPlayerId] == 0x81)
        return 2;
    return AreAllPlayersInLinkState(0x82);
}

static bool32 UNUSED IsAnyPlayerExitingCableClub(void)
{
    return IsAnyPlayerInLinkState(0x83);
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

static void LoadCableClubPlayer(int linkPlayerId, int myPlayerId, struct CableClubPlayer *trainer)
{
    s16 x, y;

    trainer->playerId = linkPlayerId;
    trainer->isLocalPlayer = (linkPlayerId == myPlayerId) ? 1 : 0;
    trainer->movementMode = gLinkPlayerObjectEvents[linkPlayerId].movementMode;
    trainer->facing = GetLinkPlayerFacingDirection(linkPlayerId);
    GetLinkPlayerCoords(linkPlayerId, &x, &y);
    trainer->pos.x = x;
    trainer->pos.y = y;
    trainer->pos.elevation = GetLinkPlayerElevation(linkPlayerId);
    trainer->metatileBehavior = MapGridGetMetatileBehaviorAt(x, y);
}

static bool32 IsCableClubPlayerUnfrozen(struct CableClubPlayer *player)
{
    u8 mode = player->movementMode;
    if (mode == 2 || mode == 0)
        return TRUE;
    else
        return FALSE;
}

static bool32 CanCableClubPlayerPressStart(struct CableClubPlayer *player)
{
    u8 mode = player->movementMode;
    if (mode == 2 || mode == 0)
        return TRUE;
    else
        return FALSE;
}

static const u8 *TryGetTileEventScript(struct CableClubPlayer *player)
{
    if (player->movementMode != 2)
        return 0;
    return GetCoordEventScriptAtMapPosition(&player->pos);
}

static bool32 PlayerIsAtSouthExit(struct CableClubPlayer *player)
{
    if (player->movementMode != 2 && player->movementMode != 0)
        return FALSE;
    if (!MetatileBehavior_IsSouthArrowWarp(player->metatileBehavior))
        return FALSE;
    if (player->facing != 1)
        return FALSE;
    return TRUE;
}

static const u8 *sub_805568C(struct CableClubPlayer *player)
{
    struct MapPosition otherPlayerPos;
    u8 linkPlayerId;

    if (player->movementMode && player->movementMode != 2)
        return 0;

    otherPlayerPos = player->pos;
    otherPlayerPos.x += gDirectionToVectors[player->facing].x;
    otherPlayerPos.y += gDirectionToVectors[player->facing].y;
    otherPlayerPos.elevation = 0;
    linkPlayerId = GetLinkPlayerIdAt(otherPlayerPos.x, otherPlayerPos.y);

    if (linkPlayerId != 4)
    {
        if (!player->isLocalPlayer)
            return TradeRoom_TooBusyToNotice;
        if (sPlayerLinkStates[linkPlayerId] != 0x80)
            return TradeRoom_TooBusyToNotice;
        if (!GetLinkTrainerCardColor(linkPlayerId))
            return TradeRoom_ReadTrainerCard1;
        else
            return TradeRoom_ReadTrainerCard2;
    }

    return GetInteractedLinkPlayerScript(&otherPlayerPos, player->metatileBehavior, player->facing);
}

static u16 sub_8055758(const u8 *script)
{
    if (script == DoubleBattleColosseum_EventScript_1A4383)
        return 10;
    if (script == DoubleBattleColosseum_EventScript_1A439E)
        return 9;
    if (script == DoubleBattleColosseum_EventScript_1A43B9)
        return 10;
    if (script == DoubleBattleColosseum_EventScript_1A43D4)
        return 9;
    if (script == RecordCorner_EventScript_1A4418)
        return 10;
    if (script == RecordCorner_EventScript_1A442D)
        return 9;
    if (script == RecordCorner_EventScript_1A4442)
        return 10;
    if (script == RecordCorner_EventScript_1A4457)
        return 9;
    if (script == SingleBattleColosseum_EventScript_1A436F)
        return 10;
    if (script == SingleBattleColosseum_EventScript_1A4379)
        return 9;
    if (script == TradeCenter_EventScript_1A43F0)
        return 10;
    if (script == TradeCenter_EventScript_1A43FA)
        return 9;
    return 0;
}

static void InitLinkPlayerQueueScript(void)
{
    LockPlayerFieldControls();
}

static void InitLinkRoomStartMenuScript(void)
{
    PlaySE(SE_WIN_OPEN);
    ShowStartMenu();
    LockPlayerFieldControls();
}

static void RunInteractLocalPlayerScript(const u8 *script)
{
    PlaySE(SE_SELECT);
    ScriptContext_SetupScript(script);
    LockPlayerFieldControls();
}

static void RunConfirmLeaveCableClubScript(void)
{
    PlaySE(SE_WIN_OPEN);
    ScriptContext_SetupScript(TradeRoom_PromptToCancelLink);
    LockPlayerFieldControls();
}

static void InitMenuBasedScript(const u8 *script)
{
    PlaySE(SE_SELECT);
    ScriptContext_SetupScript(script);
    LockPlayerFieldControls();
}

static void RunTerminateLinkScript(void)
{
    ScriptContext_SetupScript(TradeRoom_TerminateLink);
    LockPlayerFieldControls();
}

bool32 Overworld_IsRecvQueueAtMax(void)
{
    if (!IsOverworldLinkActive())
        return 0;
    if (gLink.recvQueue.count >= 3)
        sReceivingFromLink = 1;
    else
        sReceivingFromLink = 0;
    return sReceivingFromLink;
}

bool32 sub_80558AC(void)
{
    u8 temp;

    if (IsOverworldLinkActive() != TRUE)
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

bool32 sub_8055910(void)
{
    if (IsOverworldLinkActive() != TRUE)
        return FALSE;

    if (IsSendingKeysToLink() != TRUE)
        return FALSE;

    if (sPlayerKeyInterceptCallback == KeyInterCB_DeferToSendQueue)
        return TRUE;

    return FALSE;
}

bool32 sub_8055940(void)
{
    if (!IsSendingKeysToLink())
        return FALSE;
    return TRUE;
}

static void ZeroLinkPlayerObjectEvent(struct LinkPlayerObjectEvent *linkPlayerObjEvent)
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

static void SpawnLinkPlayerObjectEvent(u8 linkPlayerId, s16 x, s16 y, u8 gender)
{
    u8 objEventId = GetFirstInactiveObjectEventId();
    struct LinkPlayerObjectEvent *linkPlayerObjEvent = &gLinkPlayerObjectEvents[linkPlayerId];
    struct ObjectEvent *objEvent = &gObjectEvents[objEventId];

    ZeroLinkPlayerObjectEvent(linkPlayerObjEvent);
    ZeroObjectEvent(objEvent);

    linkPlayerObjEvent->active = 1;
    linkPlayerObjEvent->linkPlayerId = linkPlayerId;
    linkPlayerObjEvent->objEventId = objEventId;
    linkPlayerObjEvent->movementMode = 0;

    objEvent->active = 1;
    objEvent->singleMovementActive = gender;
    objEvent->range.as_byte = 2;
    objEvent->spriteId = 64;

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

static void UNUSED SetLinkPlayerObjectRange(u8 linkPlayerId, u8 dir)
{
    if (gLinkPlayerObjectEvents[linkPlayerId].active)
    {
        u8 objEventId = gLinkPlayerObjectEvents[linkPlayerId].objEventId;
        struct ObjectEvent *objEvent = &gObjectEvents[objEventId];
        objEvent->range.as_byte = dir;
    }
}

static void UNUSED DestroyLinkPlayerObject(u8 linkPlayerId)
{
    struct LinkPlayerObjectEvent *linkPlayerObjEvent = &gLinkPlayerObjectEvents[linkPlayerId];
    u8 objEventId = linkPlayerObjEvent->objEventId;
    struct ObjectEvent *objEvent = &gObjectEvents[objEventId];
    if (objEvent->spriteId != 64 )
        DestroySprite(&gSprites[objEvent->spriteId]);
    linkPlayerObjEvent->active = 0;
    objEvent->active = 0;
}

static u8 GetSpriteForLinkedPlayer(u8 linkPlayerId)
{
    u8 objEventId = gLinkPlayerObjectEvents[linkPlayerId].objEventId;
    struct ObjectEvent *objEvent = &gObjectEvents[objEventId];
    return objEvent->spriteId;
}

static void GetLinkPlayerCoords(u8 linkPlayerId, u16 *x, u16 *y)
{
    u8 objEventId = gLinkPlayerObjectEvents[linkPlayerId].objEventId;
    struct ObjectEvent *objEvent = &gObjectEvents[objEventId];
    *x = objEvent->currentCoords.x;
    *y = objEvent->currentCoords.y;
}

static u8 GetLinkPlayerFacingDirection(u8 linkPlayerId)
{
    u8 objEventId = gLinkPlayerObjectEvents[linkPlayerId].objEventId;
    struct ObjectEvent *objEvent = &gObjectEvents[objEventId];
    return objEvent->range.as_byte;
}

static u8 GetLinkPlayerElevation(u8 linkPlayerId)
{
    u8 objEventId = gLinkPlayerObjectEvents[linkPlayerId].objEventId;
    struct ObjectEvent *objEvent = &gObjectEvents[objEventId];
    return objEvent->currentElevation;
}

static s32 UNUSED GetLinkPlayerObjectStepTimer(u8 linkPlayerId)
{
    u8 objEventId = gLinkPlayerObjectEvents[linkPlayerId].objEventId;
    struct ObjectEvent *objEvent = &gObjectEvents[objEventId];
    return 16 - (s8)objEvent->directionSequenceIndex;
}

static u8 GetLinkPlayerIdAt(s16 x, s16 y)
{
    u8 i;
    for (i = 0; i < 4; i++)
    {
        if (gLinkPlayerObjectEvents[i].active
         && (gLinkPlayerObjectEvents[i].movementMode == 0 || gLinkPlayerObjectEvents[i].movementMode == 2))
        {
            struct ObjectEvent *objEvent = &gObjectEvents[gLinkPlayerObjectEvents[i].objEventId];
            if (objEvent->currentCoords.x == x && objEvent->currentCoords.y == y)
                return i;
        }
    }
    return 4;
}

static void SetPlayerFacingDirection(u8 linkPlayerId, u8 facing)
{
    struct LinkPlayerObjectEvent *linkPlayerObjEvent = &gLinkPlayerObjectEvents[linkPlayerId];
    u8 objEventId = linkPlayerObjEvent->objEventId;
    struct ObjectEvent *objEvent = &gObjectEvents[objEventId];

    if (linkPlayerObjEvent->active)
    {
        if (facing > 10)
            objEvent->triggerGroundEffectsOnMove = 1;
        else
            sMovementStatusHandler[sLinkPlayerMovementModes[linkPlayerObjEvent->movementMode](linkPlayerObjEvent, objEvent, facing)](linkPlayerObjEvent, objEvent);
    }
}

static u8 MovementEventModeCB_Normal(struct LinkPlayerObjectEvent *linkPlayerObjEvent, struct ObjectEvent *objEvent, u8 dir)
{
    return sLinkPlayerFacingHandlers[dir](linkPlayerObjEvent, objEvent, dir);
}

static u8 MovementEventModeCB_Ignored(struct LinkPlayerObjectEvent *linkPlayerObjEvent, struct ObjectEvent *objEvent, u8 dir)
{
    return 1;
}

static u8 MovementEventModeCB_Scripted(struct LinkPlayerObjectEvent *linkPlayerObjEvent, struct ObjectEvent *objEvent, u8 dir)
{
    return sLinkPlayerFacingHandlers[dir](linkPlayerObjEvent, objEvent, dir);
}

static u8 FacingHandler_DoNothing(struct LinkPlayerObjectEvent *linkPlayerObjEvent, struct ObjectEvent *objEvent, u8 dir)
{
    return 0;
}

static u8 FacingHandler_DpadMovement(struct LinkPlayerObjectEvent *linkPlayerObjEvent, struct ObjectEvent *objEvent, u8 dir)
{
    s16 x, y;

    objEvent->range.as_byte = FlipVerticalAndClearForced(dir, objEvent->range.as_byte);
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

static u8 FacingHandler_ForcedFacingChange(struct LinkPlayerObjectEvent *linkPlayerObjEvent, struct ObjectEvent *objEvent, u8 dir)
{
    objEvent->range.as_byte = FlipVerticalAndClearForced(dir, objEvent->range.as_byte);
    return 0;
}

static void MovementStatusHandler_EnterFreeMode(struct LinkPlayerObjectEvent *linkPlayerObjEvent, struct ObjectEvent *objEvent)
{
    linkPlayerObjEvent->movementMode = 0;
}

static void MovementStatusHandler_TryAdvanceScript(struct LinkPlayerObjectEvent *linkPlayerObjEvent, struct ObjectEvent *objEvent)
{
    objEvent->directionSequenceIndex--;
    linkPlayerObjEvent->movementMode = 1;
    MoveCoords(objEvent->range.as_byte, &objEvent->initialCoords.x, &objEvent->initialCoords.y);
    if (!objEvent->directionSequenceIndex)
    {
        ShiftStillObjectEventCoords(objEvent);
        linkPlayerObjEvent->movementMode = 2;
    }
}

static u8 FlipVerticalAndClearForced(u8 newFacing, u8 oldFacing)
{
    switch (newFacing - 1)
    {
    case 0:
    case 6:
        return 2;
    case 1:
    case 7:
        return 1;
    case 2:
    case 8:
        return 3;
    case 3:
    case 9:
        return 4;
    }
    return oldFacing;
}

static u8 LinkPlayerGetCollision(u8 selfObjEventId, u8 direction, s16 x, s16 y)
{
    u8 i;
    for (i = 0; i < 16; i++)
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
        u8 val = GetRivalAvatarGraphicsIdByStateIdAndGender(PLAYER_AVATAR_STATE_NORMAL, objEvent->singleMovementActive);
        objEvent->spriteId = CreateObjectGraphicsSprite(val, SpriteCB_LinkPlayer, 0, 0, 0);
        sprite = &gSprites[objEvent->spriteId];
        sprite->coordOffsetEnabled = TRUE;
        sprite->data[0] = linkPlayerId;
        objEvent->triggerGroundEffectsOnMove = 0;
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
    if (!linkPlayerObjEvent->movementMode)
        StartSpriteAnim(sprite, GetFaceDirectionAnimNum(objEvent->range.as_byte));
    else
        StartSpriteAnimIfDifferent(sprite, GetMoveDirectionAnimNum(objEvent->range.as_byte));
    UpdateObjectEventSpriteInvisibility(sprite, 0);
    if (objEvent->triggerGroundEffectsOnMove)
    {
        sprite->invisible = ((sprite->data[7] & 4) >> 2);
        sprite->data[7]++;
    }
}
