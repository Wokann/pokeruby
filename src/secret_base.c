#include "global.h"
#include "secret_base.h"
#include "decoration.h"
#include "event_data.h"
#include "field_camera.h"
#include "field_effect.h"
#include "field_fadetransition.h"
#include "field_player_avatar.h"
#include "field_specials.h"
#include "field_weather.h"
#include "fieldmap.h"
#include "fldeff_decoration.h"
#include "link.h"
#include "main.h"
#include "sound.h"
#include "constants/songs.h"
#include "constants/maps.h"
#include "map_name_popup.h"
#include "menu.h"
#include "menu_helpers.h"
#include "metatile_behavior.h"
#include "new_game.h"
#include "palette.h"
#include "pokemon.h"
#include "overworld.h"
#include "script.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "text.h"
#include "constants/event_bg.h"
#include "constants/event_objects.h"
#include "constants/decorations.h"
#include "constants/items.h"
#include "constants/map_types.h"
#include "constants/metatile_labels.h"
#include "constants/moves.h"
#include "constants/species.h"
#include "constants/vars.h"


static void FinalizeRegistryMenu(u8 taskId);
static void HandleRegistryMenuInput(u8 taskId);
static u8 GetRegisteredSecretBaseIndex(u8 registeredBaseIndex);
static void ShowRegistryMenuActions(u8 taskId);
static void HandleRegistryMenuActionsInput(u8 taskId);
static void ShowRegistryMenuDeleteConfirmation(u8 taskId);
static void ShowRegistryMenuDeleteYesNo(u8 taskId);
static void DeleteRegistry_Yes(u8 taskId);
static void DeleteRegistry_No(u8 taskId);
static void ReturnToMainRegistryMenu(u8 taskId);
static void GoToSecretBasePCRegisterMenu(u8 taskId);
static void Task_ShowSecretBaseRegistryMenu(u8 taskId);
static u8 GetSecretBaseOwnerType(u8 secretBaseIndex);

extern u8 SecretBase_EventScript_PCCancel[];
extern u8 SecretBase_EventScript_ShowRegisterMenu[];
EWRAM_DATA u8 sCurSecretBaseId = 0;

struct SecretBaseEntranceMetatiles
{
    u16 closedMetatileId;
    u16 openMetatileId;
};

enum
{
    UNREGISTERED,
    REGISTERED,
    NEW,
};

static const struct SecretBaseEntranceMetatiles sSecretBaseEntranceMetatiles[] = {
    {0x26,  0x36},
    {0x27,  0x37},
    {0x1a0, 0x1a1},
    {0x1a8, 0x1a9},
    {0x1b0, 0x1b1},
    {0x208, 0x210},
    {0x271, 0x278}
};


const u8 sSecretBaseEntrancePositions[] = {
    MAP_NUM(MAP_SECRET_BASE_RED_CAVE1),     0,  1,  3,
    MAP_NUM(MAP_SECRET_BASE_RED_CAVE2),     0,  5,  9,
    MAP_NUM(MAP_SECRET_BASE_RED_CAVE3),     0,  1,  3,
    MAP_NUM(MAP_SECRET_BASE_RED_CAVE4),     0,  7, 13,
    MAP_NUM(MAP_SECRET_BASE_BROWN_CAVE1),   0,  2,  3,
    MAP_NUM(MAP_SECRET_BASE_BROWN_CAVE2),   0,  9,  2,
    MAP_NUM(MAP_SECRET_BASE_BROWN_CAVE3),   0, 13,  4,
    MAP_NUM(MAP_SECRET_BASE_BROWN_CAVE4),   0,  1,  2,
    MAP_NUM(MAP_SECRET_BASE_BLUE_CAVE1),    0,  1,  3,
    MAP_NUM(MAP_SECRET_BASE_BLUE_CAVE2),    0,  1,  2,
    MAP_NUM(MAP_SECRET_BASE_BLUE_CAVE3),    0,  3, 15,
    MAP_NUM(MAP_SECRET_BASE_BLUE_CAVE4),    0,  3, 14,
    MAP_NUM(MAP_SECRET_BASE_YELLOW_CAVE1),  0,  9,  3,
    MAP_NUM(MAP_SECRET_BASE_YELLOW_CAVE2),  0,  8,  7,
    MAP_NUM(MAP_SECRET_BASE_YELLOW_CAVE3),  0,  3,  6,
    MAP_NUM(MAP_SECRET_BASE_YELLOW_CAVE4),  0,  5,  9,
    MAP_NUM(MAP_SECRET_BASE_TREE1),         0,  2,  3,
    MAP_NUM(MAP_SECRET_BASE_TREE2),         0,  5,  6,
    MAP_NUM(MAP_SECRET_BASE_TREE3),         0, 15,  3,
    MAP_NUM(MAP_SECRET_BASE_TREE4),         0,  4, 10,
    MAP_NUM(MAP_SECRET_BASE_SHRUB1),        0,  3,  3,
    MAP_NUM(MAP_SECRET_BASE_SHRUB2),        0,  1,  2,
    MAP_NUM(MAP_SECRET_BASE_SHRUB3),        0,  7,  8,
    MAP_NUM(MAP_SECRET_BASE_SHRUB4),        0,  9,  6
};

const struct MenuAction2 sRegistryMenuActions[] = {
    {gText_DelRegist, ShowRegistryMenuDeleteConfirmation},
    {gText_Exit, ReturnToMainRegistryMenu}
};

const struct YesNoFuncTable sDeleteRegistryYesNoFuncs = {
    DeleteRegistry_Yes,
    DeleteRegistry_No
};

static const u8 sSecretBaseOwnerGfxIds[] = {
    // Male
    OBJ_EVENT_GFX_YOUNGSTER,
    OBJ_EVENT_GFX_BUG_CATCHER,
    OBJ_EVENT_GFX_RICH_BOY,
    OBJ_EVENT_GFX_CAMPER,
    OBJ_EVENT_GFX_MAN_3,
    // Female
    OBJ_EVENT_GFX_LASS,
    OBJ_EVENT_GFX_GIRL_3,
    OBJ_EVENT_GFX_WOMAN_2,
    OBJ_EVENT_GFX_PICNICKER,
    OBJ_EVENT_GFX_WOMAN_7,
};

extern u8 SecretBase_EventScript_Enter[];
extern u8 SecretBase_Text_Trainer0Defeated[];
extern u8 SecretBase_Text_Trainer1Defeated[];
extern u8 SecretBase_Text_Trainer2Defeated[];
extern u8 SecretBase_Text_Trainer3Defeated[];
extern u8 SecretBase_Text_Trainer4Defeated[];
extern u8 SecretBase_Text_Trainer5Defeated[];
extern u8 SecretBase_Text_Trainer6Defeated[];
extern u8 SecretBase_Text_Trainer7Defeated[];
extern u8 SecretBase_Text_Trainer8Defeated[];
extern u8 SecretBase_Text_Trainer9Defeated[];


void ClearSecretBase(struct SecretBaseRecord *secretBase)
{
    u16 i;
    u16 j;

    secretBase->secretBaseId = 0;
    for (i = 0; i < OT_NAME_LENGTH; i++)
        secretBase->playerName[i] = 0xff;
    for (i = 0; i < TRAINER_ID_LENGTH; i++)
        secretBase->trainerId[i] = 0x00;
    secretBase->numSecretBasesReceived = 0;
    secretBase->numTimesEntered = 0;
    secretBase->unused = 0;
    secretBase->toRegister = 0;
    secretBase->gender = 0;
    secretBase->battledOwnerToday = 0;
    secretBase->registryStatus = 0;
    for (i = 0; i < DECOR_MAX_SECRET_BASE; i++)
    {
        secretBase->decorations[i] = 0;
        secretBase->decorationPos[i] = 0;
    }
    for (i = 0; i < PARTY_SIZE; i++)
    {
        for (j = 0; j < MAX_MON_MOVES; j++)
            secretBase->party.moves[i * MAX_MON_MOVES + j] = 0;
        secretBase->party.personality[i] = 0;
        secretBase->party.EVs[i] = 0;
        secretBase->party.species[i] = 0;
        secretBase->party.heldItems[i] = 0;
        secretBase->party.levels[i] = 0;
    }
}

void ClearSecretBaseAt(u8 secretBaseIndex)
{
    ClearSecretBase(&gSaveBlock1.secretBases[secretBaseIndex]);
}

void ClearSecretBases(void)
{
    u16 secretBaseIndex;

    for (secretBaseIndex = 0; secretBaseIndex < SECRET_BASES_COUNT; secretBaseIndex++)
        ClearSecretBaseAt(secretBaseIndex);
}

void SetCurSecretBaseId(void)
{
    sCurSecretBaseId = gSpecialVar_0x8004;
}

void TrySetCurSecretBaseIndex(void)
{
    u16 secretBaseIndex;

    gSpecialVar_Result = FALSE;
    for (secretBaseIndex = 0; secretBaseIndex < SECRET_BASES_COUNT; secretBaseIndex++)
    {
        if (sCurSecretBaseId == gSaveBlock1.secretBases[secretBaseIndex].secretBaseId)
        {
            gSpecialVar_Result = TRUE;
            VarSet(VAR_CURRENT_SECRET_BASE, secretBaseIndex);
            break;
        }
    }
}

void CheckPlayerHasSecretBase(void)
{
    if (gSaveBlock1.secretBases[0].secretBaseId)
        gSpecialVar_Result = TRUE;
    else
        gSpecialVar_Result = FALSE;
}

u8 GetSecretBaseTypeInFrontOfPlayer_(void)
{
    s16 x, y;
    s16 metatileBehavior;

    GetXYCoordsOneStepInFrontOfPlayer(&x, &y);
    metatileBehavior = MapGridGetMetatileBehaviorAt(x, y) & 0xFFF;
    if (metatileBehavior == 0x90 || metatileBehavior == 0x91)
        return 1;
    else if (metatileBehavior == 0x92 || metatileBehavior == 0x93)
        return 2;
    else if (metatileBehavior == 0x9a || metatileBehavior == 0x9b)
        return 3;
    else if (metatileBehavior == 0x94 || metatileBehavior == 0x95)
        return 4;
    else if (metatileBehavior == 0x96 || metatileBehavior == 0x97 || metatileBehavior == 0x9c || metatileBehavior == 0x9d)
        return 5;
    else if (metatileBehavior == 0x98 || metatileBehavior == 0x99)
        return 6;
    return 0;
}

void GetSecretBaseTypeInFrontOfPlayer(void)
{
    gSpecialVar_0x8007 = GetSecretBaseTypeInFrontOfPlayer_();
}

s16 FindMetatileIdInMap(const u16 *map, u8 metatileId)
{
    u16 index;

    for (index = 0; index < 0x200; index++)
    {
        if ((map[index] & 0xFFF) == metatileId)
            return index;
    }
    return -1;
}

void FindMetatileIdMapCoords(s16 *xPtr, s16 *yPtr, u16 metatileId)
{
    s16 x, y;

    for (y=0; y<gMapHeader.mapLayout->height; y++)
    {
        for (x=0; x<gMapHeader.mapLayout->width; x++)
        {
            if ((gMapHeader.mapLayout->map[y * gMapHeader.mapLayout->width + x] & 0x3ff) == metatileId)
            {
                *xPtr = x;
                *yPtr = y;
                return;
            }
        }
    }
}

void ToggleSecretBaseEntranceMetatile(void)
{
    s16 x, y;
    s16 metatileId;
    u16 i;

    GetXYCoordsOneStepInFrontOfPlayer(&x, &y);
    metatileId = MapGridGetMetatileIdAt(x, y);
    for (i = 0; i < ARRAY_COUNT(sSecretBaseEntranceMetatiles); i++)
    {
        if (sSecretBaseEntranceMetatiles[i].closedMetatileId == metatileId)
        {
            MapGridSetMetatileIdAt(x, y, sSecretBaseEntranceMetatiles[i].openMetatileId | MAPGRID_IMPASSABLE);
            CurrentMapDrawMetatileAt(x, y);
            return;
        }
    }
    for (i = 0; i < ARRAY_COUNT(sSecretBaseEntranceMetatiles); i++)
    {
        if (sSecretBaseEntranceMetatiles[i].openMetatileId == metatileId)
        {
            MapGridSetMetatileIdAt(x, y, sSecretBaseEntranceMetatiles[i].closedMetatileId | MAPGRID_IMPASSABLE);
            CurrentMapDrawMetatileAt(x, y);
            return;
        }
    }
}

u8 GetNameLength(const u8 *secretBaseOwnerName)
{
    u8 nameLength;

    for (nameLength = 0; nameLength < OT_NAME_LENGTH; nameLength++)
    {
        if (secretBaseOwnerName[nameLength] == EOS)
            return nameLength;
    }
    return OT_NAME_LENGTH;
}

void SetPlayerSecretBase(void)
{
    u8 nameLength;
    u16 trainerIdIndex;

    gSaveBlock1.secretBases[0].secretBaseId = sCurSecretBaseId;
    for (trainerIdIndex = 0; trainerIdIndex < TRAINER_ID_LENGTH; trainerIdIndex++)
        gSaveBlock1.secretBases[0].trainerId[trainerIdIndex] = gSaveBlock2.playerTrainerId[trainerIdIndex];
    VarSet(VAR_CURRENT_SECRET_BASE, 0);
    nameLength = GetNameLength(gSaveBlock2.playerName);
    memset(gSaveBlock1.secretBases[0].playerName, 0xFF, OT_NAME_LENGTH);
    StringCopyN(gSaveBlock1.secretBases[0].playerName, gSaveBlock2.playerName, nameLength);
    gSaveBlock1.secretBases[0].gender = gSaveBlock2.playerGender;
    VarSet(VAR_SECRET_BASE_MAP, gMapHeader.regionMapSectionId);
}

void SetOccupiedSecretBaseEntranceMetatiles(const struct MapEvents *events)
{
    u16 bgEventIndex;
    u16 entranceIndex;
    u16 secretBaseIndex;
    s16 metatileId;

    for (bgEventIndex = 0; bgEventIndex < events->bgEventCount; bgEventIndex++)
    {
        if (events->bgEvents[bgEventIndex].kind == BG_EVENT_SECRET_BASE)
        {
            for (secretBaseIndex = 0; secretBaseIndex < SECRET_BASES_COUNT; secretBaseIndex++)
            {
                if (gSaveBlock1.secretBases[secretBaseIndex].secretBaseId == events->bgEvents[bgEventIndex].bgUnion.secretBaseId)
                {
                    metatileId = MapGridGetMetatileIdAt(events->bgEvents[bgEventIndex].x + MAP_OFFSET, events->bgEvents[bgEventIndex].y + MAP_OFFSET);
                    for (entranceIndex = 0; entranceIndex < ARRAY_COUNT(sSecretBaseEntranceMetatiles); entranceIndex++)
                    {
                        if (sSecretBaseEntranceMetatiles[entranceIndex].closedMetatileId == metatileId)
                        {
                            MapGridSetMetatileIdAt(events->bgEvents[bgEventIndex].x + MAP_OFFSET, events->bgEvents[bgEventIndex].y + MAP_OFFSET, sSecretBaseEntranceMetatiles[entranceIndex].openMetatileId | MAPGRID_IMPASSABLE);
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }
}

void SetSecretBaseWarpDestination(void)
{
    s8 secretBaseGroup = 4 * (sCurSecretBaseId / 10);

    SetWarpDestinationToMapWarp(MAP_GROUP(MAP_SECRET_BASE_RED_CAVE1), sSecretBaseEntrancePositions[secretBaseGroup], sSecretBaseEntrancePositions[secretBaseGroup + 1]);
}

#define tState data[0]

void Task_EnterSecretBase(u8 taskId)
{
    u16 secretBaseIndex;

    switch (gTasks[taskId].tState)
    {
    case 0:
        gTasks[taskId].tState = 1;
        break;
    case 1:
        if (!gPaletteFade.active)
            gTasks[taskId].tState = 2;
        break;
    case 2:
        secretBaseIndex = VarGet(VAR_CURRENT_SECRET_BASE);
        if (gSaveBlock1.secretBases[secretBaseIndex].numTimesEntered < 0xff)
            gSaveBlock1.secretBases[secretBaseIndex].numTimesEntered++;
        SetSecretBaseWarpDestination();
        WarpIntoMap();
        gFieldCallback = FieldCB_ContinueScriptHandleMusic;
        SetMainCallback2(CB2_LoadMap);
        DestroyTask(taskId);
        break;
    }
}

#undef tState

void EnterSecretBase(void)
{
    CreateTask(Task_EnterSecretBase, 0);
    FadeScreen(1, 0);
    SetDynamicWarp(0, gSaveBlock1.location.mapGroup, gSaveBlock1.location.mapNum, -1);
}

bool8 SecretBaseMapPopupEnabled(void)
{
    if (gMapHeader.mapType == MAP_TYPE_SECRET_BASE && VarGet(VAR_INIT_SECRET_BASE) == 0)
        return FALSE;
    return TRUE;
}

void EnterNewlyCreatedSecretBase_WaitFadeIn(u8 taskId)
{
    ObjectEventTurn(&(gObjectEvents[gPlayerAvatar.objectEventId]), 2);
    if (IsWeatherNotFadingIn() == 1)
    {
        ScriptContext_Enable();
        DestroyTask(taskId);
    }
}

void EnterNewlyCreatedSecretBase_StartFadeIn(void)
{
    s16 x, y;

    LockPlayerFieldControls();
    HideMapNamePopUpWindow();
    FindMetatileIdMapCoords(&x, &y, METATILE_SecretBase_PC);
    MapGridSetMetatileIdAt(x + MAP_OFFSET, y + MAP_OFFSET, METATILE_SecretBase_PC | MAPGRID_IMPASSABLE);
    CurrentMapDrawMetatileAt(x + MAP_OFFSET, y + MAP_OFFSET);
    FadeInFromBlack();
    CreateTask(EnterNewlyCreatedSecretBase_WaitFadeIn, 0);
}

void Task_EnterNewlyCreatedSecretBase(u8 taskId)
{
    s8 secretBaseGroup;

    if (!gPaletteFade.active)
    {
        secretBaseGroup = 4 * (sCurSecretBaseId / 10);
        SetWarpDestination(gSaveBlock1.location.mapGroup, gSaveBlock1.location.mapNum, -1, sSecretBaseEntrancePositions[secretBaseGroup + 2], sSecretBaseEntrancePositions[secretBaseGroup + 3]);
        WarpIntoMap();
        gFieldCallback = EnterNewlyCreatedSecretBase_StartFadeIn;
        SetMainCallback2(CB2_LoadMap);
        DestroyTask(taskId);
    }
}

void EnterNewlyCreatedSecretBase(void)
{
    u8 taskId = CreateTask(Task_EnterNewlyCreatedSecretBase, 0);

    gTasks[taskId].data[0] = 0;
    FadeScreen(1, 0);
}

bool8 CurMapIsSecretBase(void)
{
    if (gSaveBlock1.location.mapGroup == MAP_GROUP(MAP_SECRET_BASE_SHRUB4) && (u8)(gSaveBlock1.location.mapNum) <= MAP_NUM(MAP_SECRET_BASE_SHRUB4))
        return TRUE;
    return FALSE;
}

void InitSecretBaseAppearance(bool8 hidePC)
{
    u16 secretBaseIndex;
    u16 x, y;

    if (CurMapIsSecretBase())
    {
        secretBaseIndex = VarGet(VAR_CURRENT_SECRET_BASE);
        for (x = 0; x < DECOR_MAX_SECRET_BASE; x++)
        {
            if (gSaveBlock1.secretBases[secretBaseIndex].decorations[x] > 0
             && gSaveBlock1.secretBases[secretBaseIndex].decorations[x] <= 0x78
             && gDecorations[gSaveBlock1.secretBases[secretBaseIndex].decorations[x]].permission != DECORPERM_SPRITE)
            {
                ShowDecorationOnMap(
                    (gSaveBlock1.secretBases[secretBaseIndex].decorationPos[x] >> 4) + MAP_OFFSET,
                    (gSaveBlock1.secretBases[secretBaseIndex].decorationPos[x] & 0xF) + MAP_OFFSET,
                    gSaveBlock1.secretBases[secretBaseIndex].decorations[x]);
            }
        }
        if (secretBaseIndex != 0)
        {
            FindMetatileIdMapCoords(&x, &y, METATILE_SecretBase_PC);
            MapGridSetMetatileIdAt(x + MAP_OFFSET, y + MAP_OFFSET, METATILE_SecretBase_RegisterPC | MAPGRID_IMPASSABLE);
        }
        else if (hidePC == TRUE && VarGet(VAR_SECRET_BASE_INITIALIZED) == TRUE)
        {
            FindMetatileIdMapCoords(&x, &y, METATILE_SecretBase_PC);
            MapGridSetMetatileIdAt(x + MAP_OFFSET, y + MAP_OFFSET, METATILE_SecretBase_Ground | MAPGRID_IMPASSABLE);
        }
    }
}

void InitSecretBaseDecorationSprites(void)
{
    u8 *decorations;
    u8 *decorationPositions;
    u8 decorationIndex;
    u8 objectEventId = 0;
    u8 metatileBehavior;
    u8 permission;
    u8 numDecorations;
    u16 secretBaseIndex = VarGet(VAR_CURRENT_SECRET_BASE);

    if (!CurMapIsSecretBase())
    {
        decorations = gSaveBlock1.playerRoomDecor;
        decorationPositions = gSaveBlock1.playerRoomDecorPos;
        numDecorations = DECOR_MAX_PLAYERS_HOUSE;
    }
    else
    {
        decorations = gSaveBlock1.secretBases[secretBaseIndex].decorations;
        decorationPositions = gSaveBlock1.secretBases[secretBaseIndex].decorationPos;
        numDecorations = DECOR_MAX_SECRET_BASE;
    }
    for (decorationIndex = 0; decorationIndex < numDecorations; decorationIndex++)
    {
        if (decorations[decorationIndex] != DECOR_NONE)
        {
            permission = gDecorations[decorations[decorationIndex]].permission;
            if (permission == DECORPERM_SPRITE)
            {
                for (objectEventId = 0; objectEventId < gMapHeader.events->objectEventCount; objectEventId++)
                {
                    if (gMapHeader.events->objectEvents[objectEventId].flagId == gSpecialVar_0x8004 + 0xAE)
                        break;
                }
                if (objectEventId != gMapHeader.events->objectEventCount)
                {
                    gSpecialVar_0x8006 = decorationPositions[decorationIndex] >> 4;
                    gSpecialVar_0x8007 = decorationPositions[decorationIndex] & 0xF;
                    metatileBehavior = MapGridGetMetatileBehaviorAt(gSpecialVar_0x8006 + MAP_OFFSET, gSpecialVar_0x8007 + MAP_OFFSET);
                    if (MetatileBehavior_IsSecretBaseLargeMatEdge(metatileBehavior) == TRUE || MetatileBehavior_IsLargeMatCenter(metatileBehavior) == TRUE)
                    {
                        gSpecialVar_Result = VAR_OBJ_GFX_ID_0 + (gMapHeader.events->objectEvents[objectEventId].graphicsId - OBJ_EVENT_GFX_VAR_0);
                        VarSet(gSpecialVar_Result, gDecorations[decorations[decorationIndex]].tiles[0]);
                        gSpecialVar_Result = gMapHeader.events->objectEvents[objectEventId].localId;
                        FlagClear(gSpecialVar_0x8004 + 0xAE);
                        TrySpawnObjectEvent(gSpecialVar_Result, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup);
                        TryMoveObjectEventToMapCoords(gSpecialVar_Result, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup, gSpecialVar_0x8006, gSpecialVar_0x8007);
                        TryOverrideObjectEventTemplateCoords(gSpecialVar_Result, gSaveBlock1.location.mapNum, gSaveBlock1.location.mapGroup);
                        gSpecialVar_0x8004 ++;
                    }
                }
            }
        }
    }
}

void SetSecretBaseOwnerGfxId(void)
{
    int secretBaseIndex = VarGet(VAR_CURRENT_SECRET_BASE);

    VarSet(VAR_OBJ_GFX_ID_F, sSecretBaseOwnerGfxIds[GetSecretBaseOwnerType(secretBaseIndex)]);
}

void SetCurSecretBaseIdFromPosition(const struct MapPosition *position, const struct MapEvents *events)
{
    s16 bgEventIndex;

    for (bgEventIndex = 0; bgEventIndex < events->bgEventCount; bgEventIndex++)
    {
        if (events->bgEvents[bgEventIndex].kind == BG_EVENT_SECRET_BASE && position->x == events->bgEvents[bgEventIndex].x + MAP_OFFSET
         && position->y == events->bgEvents[bgEventIndex].y + MAP_OFFSET)
        {
            sCurSecretBaseId = events->bgEvents[bgEventIndex].bgUnion.secretBaseId;
            break;
        }
    }
}

void WarpIntoSecretBase(const struct MapPosition *position, const struct MapEvents *events)
{
    SetCurSecretBaseIdFromPosition(position, events);
    TrySetCurSecretBaseIndex();
    ScriptContext_SetupScript(SecretBase_EventScript_Enter);
}

bool8 TrySetCurSecretBase(void)
{
    SetCurSecretBaseId();
    TrySetCurSecretBaseIndex();
    if (gSpecialVar_Result == TRUE)
        return FALSE;
    return TRUE;
}

void Task_WarpOutOfSecretBase(u8 taskId)
{
    switch (gTasks[taskId].data[0])
    {
    case 0:
        LockPlayerFieldControls();
        gTasks[taskId].data[0] = 1;
        break;
    case 1:
        if (!gPaletteFade.active)
            gTasks[taskId].data[0] = 2;
        break;
    case 2:
        SetWarpDestinationToDynamicWarp(WARP_ID_SECRET_BASE);
        WarpIntoMap();
        gFieldCallback = FieldCB_DefaultWarpExit;
        SetMainCallback2(CB2_LoadMap);
        UnlockPlayerFieldControls();
        DestroyTask(taskId);
        break;
    }
}

void WarpOutOfSecretBase(void)
{
    CreateTask(Task_WarpOutOfSecretBase, 0);
    FadeScreen(1, 0);
}

void IsCurSecretBaseOwnedByAnotherPlayer(void)
{
    if (gSaveBlock1.secretBases[0].secretBaseId != sCurSecretBaseId)
        gSpecialVar_Result = TRUE;
    else
        gSpecialVar_Result = FALSE;
}

u8 GetCurrentSecretBaseIndex(u8 secretBaseId)
{
    s16 secretBaseIndex;

    for (secretBaseIndex = 0; secretBaseIndex < SECRET_BASES_COUNT; secretBaseIndex++)
    {
        if (gSaveBlock1.secretBases[secretBaseIndex].secretBaseId == secretBaseId)
            return secretBaseIndex;
    }
    return 0;
}

u8 *GetSecretBaseName(u8 *dest, u8 secretBaseIdx)
{
    u8 nameLength;
    u8 *str;

    nameLength = GetNameLength(gSaveBlock1.secretBases[secretBaseIdx].playerName);

    str = StringCopyN(dest, gSaveBlock1.secretBases[secretBaseIdx].playerName, nameLength);
    str[0] = EOS;

#if ENGLISH
    return StringAppend(dest, gText_ApostropheSBase);
#elif GERMAN
    return GermanStringFormatName(dest, gText_ApostropheSBase);
#endif
}

u8 *GetSecretBaseMapName(u8 *dest)
{
    sCurSecretBaseId = gSaveBlock1.secretBases[VarGet(VAR_CURRENT_SECRET_BASE)].secretBaseId;
    return GetSecretBaseName(dest, VarGet(VAR_CURRENT_SECRET_BASE));
}

void CopyCurSecretBaseOwnerName_StrVar1(void)
{
    u8 *trainerName = gSaveBlock1.secretBases[(u8)VarGet(VAR_CURRENT_SECRET_BASE)].playerName;
    u8 *dest = gStringVar1;
    u8 nameLength = GetNameLength(trainerName);
    u8 *stringEnd = StringCopyN(dest, trainerName, nameLength);

    *stringEnd = EOS;
}

bool8 IsSecretBaseRegistered(u8 secretBaseIndex)
{
    if (gSaveBlock1.secretBases[secretBaseIndex].registryStatus)
        return TRUE;
    else
        return FALSE;
}

static u8 GetAverageEVs(struct Pokemon *pokemon)
{
    u16 evTotal = GetMonData(pokemon, MON_DATA_HP_EV);

    evTotal += GetMonData(pokemon, MON_DATA_ATK_EV);
    evTotal += GetMonData(pokemon, MON_DATA_DEF_EV);
    evTotal += GetMonData(pokemon, MON_DATA_SPEED_EV);
    evTotal += GetMonData(pokemon, MON_DATA_SPATK_EV);
    evTotal += GetMonData(pokemon, MON_DATA_SPDEF_EV);
    return (u8)(evTotal / 6);
}

void SetPlayerSecretBaseParty(void)
{
    u16 partyIndex;
    u16 partyId = 0;
    struct SecretBaseRecord *secretBase = &gSaveBlock1.secretBases[0];

    for (partyIndex = 0; partyIndex < PARTY_SIZE; partyIndex++)
    {
        u16 moveIndex;

        for (moveIndex = 0; moveIndex < MAX_MON_MOVES; moveIndex++)
            secretBase->party.moves[partyIndex * MAX_MON_MOVES + moveIndex] = MOVE_NONE;

        secretBase->party.species[partyIndex] = SPECIES_NONE;
        secretBase->party.heldItems[partyIndex] = ITEM_NONE;
        secretBase->party.levels[partyIndex] = 0;
        secretBase->party.personality[partyIndex] = 0;
        secretBase->party.EVs[partyIndex] = 0;

        if (GetMonData(&gPlayerParty[partyIndex], MON_DATA_SPECIES) != SPECIES_NONE && !GetMonData(&gPlayerParty[partyIndex], MON_DATA_IS_EGG))
        {
            for (moveIndex = 0; moveIndex < MAX_MON_MOVES; moveIndex++)
                secretBase->party.moves[partyId * MAX_MON_MOVES + moveIndex] = GetMonData(&gPlayerParty[partyIndex], MON_DATA_MOVE1 + moveIndex);

            secretBase->party.species[partyId] = GetMonData(&gPlayerParty[partyIndex], MON_DATA_SPECIES);
            secretBase->party.heldItems[partyId] = GetMonData(&gPlayerParty[partyIndex], MON_DATA_HELD_ITEM);
            secretBase->party.levels[partyId] = GetMonData(&gPlayerParty[partyIndex], MON_DATA_LEVEL);
            secretBase->party.personality[partyId] = GetMonData(&gPlayerParty[partyIndex], MON_DATA_PERSONALITY);
            secretBase->party.EVs[partyId] = GetAverageEVs(&gPlayerParty[partyIndex]);
            partyId++;
        }
    }
}

void ClearAndLeaveSecretBase(void)
{
    u16 numSecretBasesReceived = gSaveBlock1.secretBases[0].numSecretBasesReceived;

    ClearSecretBaseAt(0);
    gSaveBlock1.secretBases[0].numSecretBasesReceived = numSecretBasesReceived;
    WarpOutOfSecretBase();
}

void MoveOutOfSecretBase(void)
{
    IncrementGameStat(GAME_STAT_MOVED_SECRET_BASE);
    ClearAndLeaveSecretBase();
}

void ClosePlayerSecretBaseEntrance(void)
{
    u16 bgEventIndex;
    const struct MapEvents *mapEvents = gMapHeader.events;

    for (bgEventIndex = 0; bgEventIndex < mapEvents->bgEventCount; bgEventIndex++)
    {
        if (mapEvents->bgEvents[bgEventIndex].kind == BG_EVENT_SECRET_BASE
         && gSaveBlock1.secretBases[0].secretBaseId == mapEvents->bgEvents[bgEventIndex].bgUnion.secretBaseId)
        {
            u16 entranceIndex;
            s16 metatileId = MapGridGetMetatileIdAt(mapEvents->bgEvents[bgEventIndex].x + MAP_OFFSET, mapEvents->bgEvents[bgEventIndex].y + MAP_OFFSET);

            for (entranceIndex = 0; entranceIndex < ARRAY_COUNT(sSecretBaseEntranceMetatiles); entranceIndex++)
            {
                if (sSecretBaseEntranceMetatiles[entranceIndex].openMetatileId == metatileId)
                {
                    MapGridSetMetatileIdAt(
                        mapEvents->bgEvents[bgEventIndex].x + MAP_OFFSET,
                        mapEvents->bgEvents[bgEventIndex].y + MAP_OFFSET,
                        sSecretBaseEntranceMetatiles[entranceIndex].closedMetatileId | MAPGRID_IMPASSABLE);
                    break;
                }
            }

            DrawWholeMapView();
            break;
        }
    }
}

void MoveOutOfSecretBaseFromOutside(void)
{
    u16 numSecretBasesReceived;

    ClosePlayerSecretBaseEntrance();
    IncrementGameStat(GAME_STAT_MOVED_SECRET_BASE);

    numSecretBasesReceived = gSaveBlock1.secretBases[0].numSecretBasesReceived;
    ClearSecretBaseAt(0);
    gSaveBlock1.secretBases[0].numSecretBasesReceived = numSecretBasesReceived;
}

u8 GetNumRegisteredSecretBases(void)
{
    s16 secretBaseIndex;
    u8 count = 0;

    for (secretBaseIndex = 1; secretBaseIndex < SECRET_BASES_COUNT; secretBaseIndex++)
    {
        if (IsSecretBaseRegistered(secretBaseIndex) == TRUE)
            count++;
    }
    return count;
}

void GetCurSecretBaseRegistrationValidity(void)
{
    if (IsSecretBaseRegistered(GetCurrentSecretBaseIndex(sCurSecretBaseId)) == TRUE)
        gSpecialVar_Result = 1;
    else if (GetNumRegisteredSecretBases() > 9)
        gSpecialVar_Result = 2;
    else
        gSpecialVar_Result = 0;
}

void ToggleCurSecretBaseRegistry(void)
{
    gSaveBlock1.secretBases[GetCurrentSecretBaseIndex(sCurSecretBaseId)].registryStatus ^= 1;
    FlagSet(FLAG_SECRET_BASE_REGISTRY_ENABLED);
}

void ShowSecretBaseDecorationMenu(void)
{
    CreateTask(DoSecretBaseDecorationMenu, 0);
}

void ShowSecretBaseRegistryMenu(void)
{
    CreateTask(Task_ShowSecretBaseRegistryMenu, 0);
}

#define tNumBases          data[0]
#define tSelectedRow       data[1]
#define tScrollOffset      data[2]
#define tMaxShownItems     data[3]
#define tSelectedBaseIndex data[4]

static void Task_ShowSecretBaseRegistryMenu(u8 taskId)
{
    s16 *data;

    LockPlayerFieldControls();
    ResetScrollIndicatorState();
    LoadScrollIndicatorPalette();

    data = gTasks[taskId].data;
    tNumBases = GetNumRegisteredSecretBases();
    if (tNumBases != 0)
    {
        if (tNumBases > 7)
            tMaxShownItems = 7;
        else
            tMaxShownItems = tNumBases;

        tSelectedRow = 0;
        tScrollOffset = 0;

        Menu_EraseWindowRect(0, 0, 29, 19);
        FinalizeRegistryMenu(taskId);

        gTasks[taskId].func = HandleRegistryMenuInput;
    }
    else
    {
        DisplayItemMessageOnField(taskId, gText_NoRegistry, GoToSecretBasePCRegisterMenu, 0);
    }
}

void BuildRegistryMenuItems(u8 taskId)
{
    u8 secretBaseIndex;
    s16 *data = gTasks[taskId].data;
    u8 firstVisibleBaseIndex = 0;
    u8 displayedCount = 0;

    for (secretBaseIndex = 1; secretBaseIndex < SECRET_BASES_COUNT; secretBaseIndex++)
    {
        if (firstVisibleBaseIndex == tScrollOffset)
        {
            firstVisibleBaseIndex = secretBaseIndex;
            break;
        }

        if (IsSecretBaseRegistered(secretBaseIndex) == TRUE)
            firstVisibleBaseIndex++;
    }

    for (secretBaseIndex = firstVisibleBaseIndex; secretBaseIndex < SECRET_BASES_COUNT; secretBaseIndex++)
    {
        if (IsSecretBaseRegistered(secretBaseIndex) == TRUE)
        {
            GetSecretBaseName(gStringVar1, secretBaseIndex);
            Menu_BlankWindowRect(18, 2 * displayedCount + 2, 28, 2 * displayedCount + 3);
            Menu_PrintText(gStringVar1, 18, 2 * displayedCount + 2);
            if (++displayedCount == 8)
                break;
        }
    }

    if (displayedCount < 8)
    {
        Menu_BlankWindowRect(18, 2 * displayedCount + 2, 28, 2 * displayedCount + 3);
        Menu_PrintText(gText_Exit, 18, 2 * displayedCount + 2);
        DestroyScrollIndicatorArrow(BOTTOM_ARROW);
        if (displayedCount != 7)
        {
            displayedCount++;
            displayedCount--;
            Menu_BlankWindowRect(18, displayedCount * 2 + 4, 28, 18);
        }
    }
    else
    {
        CreateScrollIndicatorArrow(BOTTOM_ARROW, 0xbc, 0x98);
    }

    if (tScrollOffset == 0)
        DestroyScrollIndicatorArrow(TOP_ARROW);
    else
        CreateScrollIndicatorArrow(TOP_ARROW, 0xbc, 0x08);
}

void FinalizeRegistryMenu(u8 taskId)
{
    u16 *data = gTasks[taskId].data;

    Menu_DrawStdWindowFrame(17, 0, 29, 19);
    InitMenu(0, 18, 2, tMaxShownItems + 1, tSelectedRow, 11);

    BuildRegistryMenuItems(taskId);
}

void HandleRegistryMenuInput(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    if (JOY_REPT(DPAD_UP))
    {
        if (tSelectedRow)
        {
            PlaySE(SE_SELECT);
            tSelectedRow = Menu_MoveCursor(-1);
        }
        else if (tScrollOffset)
        {
            PlaySE(SE_SELECT);
            tScrollOffset--;
            BuildRegistryMenuItems(taskId);
        }
    }
    else if (JOY_REPT(DPAD_DOWN))
    {
        if (tSelectedRow == tMaxShownItems)
        {
            if (tScrollOffset + tSelectedRow != tNumBases)
            {
                PlaySE(SE_SELECT);
                tScrollOffset++;
                BuildRegistryMenuItems(taskId);
            }
        }
        else
        {
            PlaySE(SE_SELECT);
            tSelectedRow = Menu_MoveCursor(1);
        }
    }
    else if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (tSelectedRow + tScrollOffset == tNumBases)
        {
            Menu_DestroyCursor();
            Menu_EraseWindowRect(0, 0, 29, 19);
            GoToSecretBasePCRegisterMenu(taskId);
        }
        else
        {
            Menu_DestroyCursor();
            tSelectedBaseIndex = GetRegisteredSecretBaseIndex(tSelectedRow + tScrollOffset);
            ShowRegistryMenuActions(taskId);
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Menu_DestroyCursor();
        Menu_EraseWindowRect(0, 0, 29, 19);
        GoToSecretBasePCRegisterMenu(taskId);
    }
}

u8 GetRegisteredSecretBaseIndex(u8 registeredBaseIndex)
{
    u8 secretBaseIndex;
    u8 count = 0;

    for (secretBaseIndex = 1; secretBaseIndex < SECRET_BASES_COUNT; secretBaseIndex++)
    {
        if (IsSecretBaseRegistered(secretBaseIndex) == TRUE)
        {
            if (registeredBaseIndex == count)
            {
                return secretBaseIndex;
            }
            
            count++;
        }
    }

    return 0;
}

void ShowRegistryMenuActions(u8 taskId)
{
    PauseScrollIndicatorArrow(TOP_ARROW);
    PauseScrollIndicatorArrow(BOTTOM_ARROW);
    Menu_DrawStdWindowFrame(1, 0, 12, 5);
    Menu_PrintItems(2, 1, 2, (const struct MenuAction *)sRegistryMenuActions);
    InitMenu(0, 2, 1, 2, 0, 10);
    gTasks[taskId].func = HandleRegistryMenuActionsInput;
}

void HandleRegistryMenuActionsInput(u8 taskId)
{
    if (JOY_REPT(DPAD_UP))
    {
        if (Menu_GetCursorPos())
        {
            PlaySE(SE_SELECT);
            Menu_MoveCursor(-1);
        }
    }
    else if (JOY_REPT(DPAD_DOWN))
    {
        if (Menu_GetCursorPos() != 1)
        {
            PlaySE(SE_SELECT);
            Menu_MoveCursor(1);
        }
    }
    else if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        sRegistryMenuActions[Menu_GetCursorPos()].func(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        ReturnToMainRegistryMenu(taskId);
    }
}

void ShowRegistryMenuDeleteConfirmation(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    DestroyScrollIndicatorArrow(TOP_ARROW);
    DestroyScrollIndicatorArrow(BOTTOM_ARROW);
    Menu_DestroyCursor();
    Menu_EraseWindowRect(0, 0, 29, 19);

    GetSecretBaseName(gStringVar1, tSelectedBaseIndex);
    StringExpandPlaceholders(gStringVar4, gText_OkayToDeleteFromRegistry);
    DisplayItemMessageOnField(taskId, gStringVar4, ShowRegistryMenuDeleteYesNo, 0);
}

void ShowRegistryMenuDeleteYesNo(u8 taskId)
{
    DisplayYesNoMenu(20, 8, 1);
    DoYesNoFuncWithChoice(taskId, &sDeleteRegistryYesNoFuncs);
}

void DeleteRegistry_Yes_Callback(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    Menu_EraseWindowRect(0, 0, 29, 19);

    gSaveBlock1.secretBases[tSelectedBaseIndex].registryStatus = 0;
    tNumBases--;

    if (tScrollOffset > 0)
        tScrollOffset--;

    if (tNumBases < 8)
        tMaxShownItems--;

    FinalizeRegistryMenu(taskId);
    gTasks[taskId].func = HandleRegistryMenuInput;
}

void DeleteRegistry_Yes(u8 taskId)
{
    Menu_EraseWindowRect(20, 8, 26, 13);
    DisplayItemMessageOnField(taskId, gText_RegisteredDataDeleted, DeleteRegistry_Yes_Callback, 0);
}

void DeleteRegistry_No(u8 taskId)
{
    Menu_EraseWindowRect(0, 0, 29, 19);

    FinalizeRegistryMenu(taskId);
    gTasks[taskId].func = HandleRegistryMenuInput;
}

void ReturnToMainRegistryMenu(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    InitMenu(0, 18, 2, tMaxShownItems + 1, tSelectedRow, 11);
    Menu_EraseWindowRect(1, 0, 12, 5);
    StartScrollIndicatorArrow(TOP_ARROW);
    StartScrollIndicatorArrow(BOTTOM_ARROW);

    gTasks[taskId].func = HandleRegistryMenuInput;
}

void GoToSecretBasePCRegisterMenu(u8 taskId)
{
    u16 curBaseIndex = VarGet(VAR_CURRENT_SECRET_BASE);

    FreeScrollIndicatorPalette();
    DestroyScrollIndicatorArrow(TOP_ARROW);
    DestroyScrollIndicatorArrow(BOTTOM_ARROW);

    if (curBaseIndex == 0)
        ScriptContext_SetupScript(SecretBase_EventScript_PCCancel);
    else
        ScriptContext_SetupScript(SecretBase_EventScript_ShowRegisterMenu);

    DestroyTask(taskId);
}

#undef tNumBases
#undef tSelectedRow
#undef tScrollOffset
#undef tMaxShownItems
#undef tSelectedBaseIndex

static u8 GetSecretBaseOwnerType(u8 secretBaseIdx)
{
    return (gSaveBlock1.secretBases[secretBaseIdx].playerName[OT_NAME_LENGTH] % 5)
        + gSaveBlock1.secretBases[secretBaseIdx].gender * 5;
}

const u8 *GetSecretBaseTrainerLoseText(void)
{
    u8 ownerType = GetSecretBaseOwnerType(VarGet(VAR_CURRENT_SECRET_BASE));
    if (ownerType == 0) return SecretBase_Text_Trainer0Defeated;
    if (ownerType == 1) return SecretBase_Text_Trainer1Defeated;
    if (ownerType == 2) return SecretBase_Text_Trainer2Defeated;
    if (ownerType == 3) return SecretBase_Text_Trainer3Defeated;
    if (ownerType == 4) return SecretBase_Text_Trainer4Defeated;
    if (ownerType == 5) return SecretBase_Text_Trainer5Defeated;
    if (ownerType == 6) return SecretBase_Text_Trainer6Defeated;
    if (ownerType == 7) return SecretBase_Text_Trainer7Defeated;
    if (ownerType == 8) return SecretBase_Text_Trainer8Defeated;
    return SecretBase_Text_Trainer9Defeated;
}

// Debugging function to test secret base battles.
void DebugInitSecretBaseParty(u8 secretBaseIndex)
{
    u16 i;

    for (i = 0; i < 1; i++)
    {
        gSaveBlock1.secretBases[secretBaseIndex].party.personality[i] = i + 1;
        gSaveBlock1.secretBases[secretBaseIndex].party.moves[i * 4] = i + 1;
        gSaveBlock1.secretBases[secretBaseIndex].party.species[i] = SPECIES_TREECKO;
        gSaveBlock1.secretBases[secretBaseIndex].party.heldItems[i] = i + 1;
        gSaveBlock1.secretBases[secretBaseIndex].party.levels[i] = i + 5;
        gSaveBlock1.secretBases[secretBaseIndex].party.EVs[i] = i * 5;
    }
}

// Ruby builds the record-mixed secret-base owner's party here. Emerald's
// corresponding special configures its different battle flow instead.
void PrepareSecretBaseTrainerBattle(void)
{
    u16 curBaseIndex = VarGet(VAR_CURRENT_SECRET_BASE);

    TryGainNewFanFromCounter(1);
    CreateSecretBaseEnemyParty(&gSaveBlock1.secretBases[curBaseIndex]);
}

void SetBattledOwnerFromResult(void)
{
    gSaveBlock1.secretBases[VarGet(VAR_CURRENT_SECRET_BASE)].battledOwnerToday = gSpecialVar_Result;
}

void GetSecretBaseOwnerAndState(void)
{
    u16 secretBaseIdx = VarGet(VAR_CURRENT_SECRET_BASE);

    if (!FlagGet(FLAG_DAILY_SECRET_BASE))
    {
        u8 i;

        for (i = 0; i < SECRET_BASES_COUNT; i++)
            gSaveBlock1.secretBases[i].battledOwnerToday = 0;

        FlagSet(FLAG_DAILY_SECRET_BASE);
    }

    gSpecialVar_0x8004 = GetSecretBaseOwnerType(secretBaseIdx);
    gSpecialVar_Result = gSaveBlock1.secretBases[secretBaseIdx].battledOwnerToday;
}

#define tStepCb  data[0]
#define tState   data[1]
#define tPlayerX data[2]
#define tPlayerY data[3]
#define tFldEff  data[4]

void SecretBasePerStepCallback(u8 taskId)
{
    s16 x, y;
    u32 metatileBehavior;
    s16 *data = gTasks[taskId].data;

    switch (tState)
    {
    case 0:
        PlayerGetDestCoords(&tPlayerX, &tPlayerY);
        tState = 1;
        break;
    case 1:
        PlayerGetDestCoords(&x, &y);
        if (x != tPlayerX || y != tPlayerY)
        {
            tPlayerX = x;
            tPlayerY = y;

            metatileBehavior = MapGridGetMetatileBehaviorAt(x, y);
            if (MetatileBehavior_IsSecretBaseGlitterMat(metatileBehavior) == TRUE)
                DoSecretBaseGlitterMatSparkle();
            else if (MetatileBehavior_IsSecretBaseBalloon(metatileBehavior) == TRUE)
                PopSecretBaseBalloon(MapGridGetMetatileIdAt(x, y), x, y);
            else if (MetatileBehavior_IsSecretBaseBreakableDoor(metatileBehavior) == TRUE)
                ShatterSecretBaseBreakableDoor(x, y);
            else if (MetatileBehavior_IsSecretBaseSoundMat(metatileBehavior) == TRUE)
                PlaySecretBaseMusicNoteMatSound(MapGridGetMetatileIdAt(x, y));
        }
        break;
    case 2:
        if (!FieldEffectActiveListContains(tFldEff))
            tState = 1;
        break;
    }
}

#undef tStepCb
#undef tState
#undef tPlayerX
#undef tPlayerY
#undef tFldEff

void SaveSecretBase(u8 secretBaseIndex, struct SecretBaseRecord *secretBase)
{
    gSaveBlock1.secretBases[secretBaseIndex] = *secretBase;
    gSaveBlock1.secretBases[secretBaseIndex].registryStatus = NEW;
}

bool8 SecretBasesHaveSameTrainerId(struct SecretBaseRecord *secretBase1, struct SecretBaseRecord *secretBase2)
{
    u8 trainerIdIndex;

    for (trainerIdIndex = 0; trainerIdIndex < TRAINER_ID_LENGTH; trainerIdIndex++)
    {
        if (secretBase1->trainerId[trainerIdIndex] != secretBase2->trainerId[trainerIdIndex])
            return FALSE;
    }

    return TRUE;
}

bool8 SecretBasesHaveSameTrainerName(struct SecretBaseRecord *secretBase1, struct SecretBaseRecord *secretBase2)
{
    u8 nameIndex;

    for (nameIndex = 0; nameIndex < OT_NAME_LENGTH && (secretBase1->playerName[nameIndex] != 0xFF || secretBase2->playerName[nameIndex] != 0xFF); nameIndex++)
    {
        if (secretBase1->playerName[nameIndex] != secretBase2->playerName[nameIndex])
            return FALSE;
    }

    return TRUE;
}

bool8 SecretBasesBelongToSamePlayer(struct SecretBaseRecord *secretBase1, struct SecretBaseRecord *secretBase2)
{
    if (secretBase1->gender == secretBase2->gender && SecretBasesHaveSameTrainerId(secretBase1, secretBase2) && SecretBasesHaveSameTrainerName(secretBase1, secretBase2))
        return TRUE;
    else
        return FALSE;
}

s16 GetSecretBaseIndexFromId(u8 secretBaseId)
{
    s16 secretBaseIndex;

    for (secretBaseIndex = 0; secretBaseIndex < SECRET_BASES_COUNT; secretBaseIndex++)
    {
        if (gSaveBlock1.secretBases[secretBaseIndex].secretBaseId == secretBaseId)
            return secretBaseIndex;
    }

    return -1;
}

u8 FindAvailableSecretBaseIndex(void)
{
    s16 secretBaseIndex;

    for (secretBaseIndex = 1; secretBaseIndex < SECRET_BASES_COUNT; secretBaseIndex++)
    {
        if (gSaveBlock1.secretBases[secretBaseIndex].secretBaseId == 0)
            return secretBaseIndex;
    }

    return 0;
}

u8 FindUnregisteredSecretBaseIndex(void)
{
    s16 secretBaseIndex;

    for (secretBaseIndex = 1; secretBaseIndex < SECRET_BASES_COUNT; secretBaseIndex++)
    {
        if (gSaveBlock1.secretBases[secretBaseIndex].registryStatus == UNREGISTERED && gSaveBlock1.secretBases[secretBaseIndex].toRegister == FALSE)
            return secretBaseIndex;
    }

    return 0;
}

u8 TrySaveFriendsSecretBase(struct SecretBaseRecord *secretBase)
{
    s16 secretBaseIndex;

    if (secretBase->secretBaseId == 0)
        return 0;

    secretBaseIndex = GetSecretBaseIndexFromId(secretBase->secretBaseId);
    if (secretBaseIndex != 0)
    {
        if (secretBaseIndex != -1)
        {
            if (gSaveBlock1.secretBases[secretBaseIndex].toRegister == TRUE)
                return 0;
            if (gSaveBlock1.secretBases[secretBaseIndex].registryStatus != NEW
             || secretBase->toRegister == TRUE)
            {
                SaveSecretBase(secretBaseIndex, secretBase);
                return secretBaseIndex;
            }
        }
        else
        {
            secretBaseIndex = FindAvailableSecretBaseIndex();
            if (secretBaseIndex != 0)
            {
                SaveSecretBase(secretBaseIndex, secretBase);
                return secretBaseIndex;
            }
            secretBaseIndex = FindUnregisteredSecretBaseIndex();
            if (secretBaseIndex)
            {
                SaveSecretBase(secretBaseIndex, secretBase);
                return secretBaseIndex;
            }
        }
    }

    return 0;
}

void SortSecretBasesByRegistryStatus(void)
{
    u8 i;
    u8 j;
    struct SecretBaseRecord temp;
    struct SecretBaseRecord *secretBases = gSaveBlock1.secretBases;

    for (i = 1; i < SECRET_BASES_COUNT - 1; i++)
    {
        for (j = i + 1; j < SECRET_BASES_COUNT; j++)
        {
            if ((!secretBases[i].registryStatus && secretBases[j].registryStatus == REGISTERED)
             || (secretBases[i].registryStatus == NEW && secretBases[j].registryStatus != NEW))
            {
                temp = secretBases[i];
                secretBases[i] = secretBases[j];
                secretBases[j] = temp;
            }
        }
    }
}

void TrySaveFriendsSecretBases(struct SecretBaseRecord *secretBases, u8 registryStatus)
{
    u16 i;

    for (i = 1; i < SECRET_BASES_COUNT; i++)
    {
        if (secretBases[i].registryStatus == registryStatus)
            TrySaveFriendsSecretBase(&secretBases[i]);
    }
}

bool8 SecretBaseBelongsToPlayer(struct SecretBaseRecord *secretBase)
{
    u8 index;

    if (!secretBase->secretBaseId)
        return FALSE;

    if (secretBase->secretBaseId && secretBase->gender != gSaveBlock2.playerGender)
        return FALSE;

    // Check if the player's trainer Id matches the secret base's id.
    for (index = 0; index < TRAINER_ID_LENGTH; index++)
    {
        if (secretBase->trainerId[index] != gSaveBlock2.playerTrainerId[index])
            return FALSE;
    }

    for (index = 0; index < OT_NAME_LENGTH && (secretBase->playerName[index] != 0xFF || gSaveBlock2.playerName[index] != 0xFF); index++)
    {
        if (secretBase->playerName[index] != gSaveBlock2.playerName[index])
            return FALSE;
    }

    return TRUE;
}

void DeleteFirstOldBaseFromPlayerInRecordMixingFriendsRecords(struct SecretBaseRecord *friendsBasesA, struct SecretBaseRecord *friendsBasesB, struct SecretBaseRecord *friendsBasesC)
{
    u8 i;
    u8 clearedRecords = 0;

    for (i = 0; i < SECRET_BASES_COUNT; i++)
    {
        if ((clearedRecords & 1) == 0)
        {
            if (SecretBaseBelongsToPlayer(&friendsBasesA[i]) == TRUE)
            {
                ClearSecretBase(&friendsBasesA[i]);
                clearedRecords |= 1;
            }
        }

        if ((clearedRecords & 2) == 0)
        {
            if (SecretBaseBelongsToPlayer(&friendsBasesB[i]) == TRUE)
            {
                ClearSecretBase(&friendsBasesB[i]);
                clearedRecords |= 2;
            }
        }

        if ((clearedRecords & 4) == 0)
        {
            if (SecretBaseBelongsToPlayer(&friendsBasesC[i]) == TRUE)
            {
                ClearSecretBase(&friendsBasesC[i]);
                clearedRecords |= 4;
            }
        }

        if (clearedRecords == 7)
            break;
    }
}

bool8 ClearDuplicateOwnedSecretBase(struct SecretBaseRecord *secretBase, struct SecretBaseRecord *secretBases, u8 secretBaseIndex)
{
    u8 i;

    for (i = 0; i < SECRET_BASES_COUNT; i++)
    {
        if (secretBases[i].secretBaseId)
        {
            if (SecretBasesBelongToSamePlayer(secretBase, &secretBases[i]) == TRUE)
            {
                if (secretBaseIndex == 0)
                {
                    ClearSecretBase(&secretBases[i]);
                    return FALSE;
                }

                if (secretBase->numSecretBasesReceived > secretBases[i].numSecretBasesReceived)
                {
                    ClearSecretBase(&secretBases[i]);
                    return FALSE;
                }

                secretBases[i].toRegister = secretBase->toRegister;

                ClearSecretBase(secretBase);
                return TRUE;
            }
        }
    }

    return FALSE;
}

void ClearDuplicateOwnedSecretBases(struct SecretBaseRecord *playersBases, struct SecretBaseRecord *friendsBasesA, struct SecretBaseRecord *friendsBasesB, struct SecretBaseRecord *friendsBasesC)
{
    u8 i;

    for (i = 1; i < SECRET_BASES_COUNT; i++)
    {
        if (playersBases[i].secretBaseId)
        {
            if (playersBases[i].registryStatus == REGISTERED)
                playersBases[i].toRegister = TRUE;

            if (!ClearDuplicateOwnedSecretBase(&playersBases[i], friendsBasesA, i))
            {
                if (!ClearDuplicateOwnedSecretBase(&playersBases[i], friendsBasesB, i))
                    ClearDuplicateOwnedSecretBase(&playersBases[i], friendsBasesC, i);
            }
        }
    }

    for (i = 0; i < SECRET_BASES_COUNT; i++)
    {
        if (friendsBasesA[i].secretBaseId)
        {
            friendsBasesA[i].battledOwnerToday = FALSE;

            if (!ClearDuplicateOwnedSecretBase(&friendsBasesA[i], friendsBasesB, i))
                ClearDuplicateOwnedSecretBase(&friendsBasesA[i], friendsBasesC, i);
        }
    }

    for (i = 0; i < SECRET_BASES_COUNT; i++)
    {
        if (friendsBasesB[i].secretBaseId)
        {
            friendsBasesB[i].battledOwnerToday = FALSE;
            ClearDuplicateOwnedSecretBase(&friendsBasesB[i], friendsBasesC, i);
        }

        if (friendsBasesC[i].secretBaseId)
            friendsBasesC[i].battledOwnerToday = FALSE;
    }
}

void SaveRecordMixBases(struct SecretBaseRecord *friendsBasesA, struct SecretBaseRecord *friendsBasesB, struct SecretBaseRecord *friendsBasesC)
{
    DeleteFirstOldBaseFromPlayerInRecordMixingFriendsRecords(friendsBasesA, friendsBasesB, friendsBasesC);
    ClearDuplicateOwnedSecretBases(gSaveBlock1.secretBases, friendsBasesA, friendsBasesB, friendsBasesC);

    TrySaveFriendsSecretBase(friendsBasesA);
    TrySaveFriendsSecretBase(friendsBasesB);
    TrySaveFriendsSecretBase(friendsBasesC);

    TrySaveFriendsSecretBases(friendsBasesA, REGISTERED);
    TrySaveFriendsSecretBases(friendsBasesB, REGISTERED);
    TrySaveFriendsSecretBases(friendsBasesC, REGISTERED);

    TrySaveFriendsSecretBases(friendsBasesA, UNREGISTERED);
    TrySaveFriendsSecretBases(friendsBasesB, UNREGISTERED);
    TrySaveFriendsSecretBases(friendsBasesC, UNREGISTERED);
} 

void ReceiveSecretBasesData(void *secretBases, u32 recordSize, u8 linkIdx)
{
    if (
#if DEBUG
     gIsDebugMode != 0 ||
#endif
     FlagGet(FLAG_RECEIVED_SECRET_POWER))
    {
        u16 i;

        u8 numLinkedPlayers = GetLinkPlayerCount();
        switch (numLinkedPlayers)
        {
        case 2:
            memset(secretBases + recordSize * 2, 0, recordSize);
            memset(secretBases + recordSize * 3, 0, recordSize);
            break;
        case 3:
            memset(secretBases + recordSize * 3, 0, recordSize);
            break;
        }

        switch (linkIdx)
        {
        case 0:
            SaveRecordMixBases(secretBases + recordSize, secretBases + recordSize * 2, secretBases + recordSize * 3);
            break;
        case 1:
            SaveRecordMixBases(secretBases + recordSize * 2, secretBases + recordSize * 3, secretBases);
            break;
        case 2:
            SaveRecordMixBases(secretBases + recordSize * 3, secretBases, secretBases + recordSize);
            break;
        case 3:
            SaveRecordMixBases(secretBases, secretBases + recordSize, secretBases + recordSize * 2);
            break;
        }

        for (i = 1; i < SECRET_BASES_COUNT; i++)
        {
            if (gSaveBlock1.secretBases[i].toRegister == TRUE)
            {
                gSaveBlock1.secretBases[i].registryStatus = REGISTERED;
                gSaveBlock1.secretBases[i].toRegister = FALSE;
            }
        }

        SortSecretBasesByRegistryStatus();

        for (i = 1; i < SECRET_BASES_COUNT; i++)
        {
            if (gSaveBlock1.secretBases[i].registryStatus == NEW)
                gSaveBlock1.secretBases[i].registryStatus = UNREGISTERED;
        }

        if (gSaveBlock1.secretBases[0].numSecretBasesReceived != 0xFFFF)
            gSaveBlock1.secretBases[0].numSecretBasesReceived++;
    }
}
