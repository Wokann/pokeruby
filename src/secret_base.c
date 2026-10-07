#include "global.h"
#include "secret_base.h"
#include "decoration.h"
#include "event_data.h"
#include "event_scripts.h"
#include "field_camera.h"
#include "field_effect.h"
#include "field_screen_effect.h"
#include "field_player_avatar.h"
#include "field_specials.h"
#include "field_weather.h"
#include "fieldmap.h"
#include "fldeff_misc.h"
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
#include "sound.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "text.h"
#include "constants/event_bg.h"
#include "constants/event_objects.h"
#include "constants/decorations.h"
#include "constants/items.h"
#include "constants/map_types.h"
#include "constants/moves.h"
#include "constants/species.h"
#include "constants/vars.h"


static void ClearSecretBase(struct SecretBase *secretBase);
static void SetCurSecretBaseId(void);
static u8 GetSecretBaseTypeInFrontOfPlayer_(void);
static void FindMetatileIdMapCoords(s16 *x, s16 *y, u16 metatileId);
static u8 GetNameLength(const u8 *secretBaseOwnerName);
static void SetSecretBaseWarpDestination(void);
static void Task_EnterSecretBase(u8 taskId);
static void EnterNewlyCreatedSecretBase_WaitFadeIn(u8 taskId);
static void EnterNewlyCreatedSecretBase_StartFadeIn(void);
static void Task_EnterNewlyCreatedSecretBase(u8 taskId);
static void Task_WarpOutOfSecretBase(u8 taskId);
static void WarpOutOfSecretBase(void);
static bool8 IsSecretBaseRegistered(u8 secretBaseIdx);
static u8 GetAverageEVs(struct Pokemon *pokemon);
static void ClosePlayerSecretBaseEntrance(void);
static u8 GetNumRegisteredSecretBases(void);
static void Task_ShowSecretBaseRegistryMenu(u8 taskId);
static void BuildRegistryMenuItems(u8 taskId);
static void FinalizeRegistryMenu(u8 taskId);
static void HandleRegistryMenuInput(u8 taskId);
static u8 sub_80BC948(u8 a);
static void ShowRegistryMenuActions(u8 taskId);
static void HandleRegistryMenuActionsInput(u8 taskId);
static void ShowRegistryMenuDeleteConfirmation(u8 taskId);
static void ShowRegistryMenuDeleteYesNo(u8 taskId);
static void DeleteRegistry_Yes(u8 taskId);
static void DeleteRegistry_No(u8 taskId);
static void ReturnToMainRegistryMenu(u8 taskId);
static void GoToSecretBasePCRegisterMenu(u8 taskId);
static u8 GetSecretBaseOwnerType(u8 secretBaseIdx);
static void SaveSecretBase(u8 secretBaseIdx, struct SecretBase *secretBase);
static bool8 SecretBasesHaveSameTrainerId(struct SecretBase *secretBase1, struct SecretBase *secretBase2);
static bool8 SecretBasesHaveSameTrainerName(struct SecretBase *sbr1, struct SecretBase *sbr2);
static bool8 SecretBasesBelongToSamePlayer(struct SecretBase *secretBase1, struct SecretBase *secretBase2);
static s16 GetSecretBaseIndexFromId(u8 secretBaseId);
static u8 FindAvailableSecretBaseIndex(void);
static u8 FindUnregisteredSecretBaseIndex(void);
static u8 TrySaveFriendsSecretBase(struct SecretBase *secretBase);
static void SortSecretBasesByRegistryStatus(void);
static void TrySaveFriendsSecretBases(struct SecretBase *secretBases, u8 registryStatus);
static bool8 SecretBaseBelongsToPlayer(struct SecretBase *secretBase);
static void DeleteFirstOldBaseFromPlayerInRecordMixingFriendsRecords(struct SecretBase *basesA, struct SecretBase *basesB, struct SecretBase *basesC);
static bool8 ClearDuplicateOwnedSecretBase(struct SecretBase *secretBase, struct SecretBase *secretBases, u8 idx);
static void ClearDuplicateOwnedSecretBases(struct SecretBase *playersBases, struct SecretBase *friendsBasesA, struct SecretBase *friendsBasesB, struct SecretBase *friendsBasesC);
static void SaveRecordMixBases(struct SecretBase *basesA, struct SecretBase *basesB, struct SecretBase *basesC);

static EWRAM_DATA u8 sCurSecretBaseId = 0;

struct SecretBaseEntranceMetatiles
{
    u16 closedMetatileId;
    u16 openMetatileId;
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


static const u8 sSecretBaseEntrancePositions[] = {
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

static const struct MenuAction2 sRegistryMenuActions[] = {
    {gText_DelRegist, ShowRegistryMenuDeleteConfirmation},
    {gOtherText_Exit, ReturnToMainRegistryMenu}
};

static const struct YesNoFuncTable sDeleteRegistryYesNoFuncs = {
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
    OBJ_EVENT_GFX_WOMAN_5,
};



static void ClearSecretBase(struct SecretBase *secretBase)
{
    u16 i;
    u16 j;

    secretBase->secretBaseId = 0;
    for (i = 0; i < OT_NAME_LENGTH; i++)
        secretBase->trainerName[i] = 0xff;
    for (i = 0; i < 4; i++)
        secretBase->trainerId[i] = 0x00;
    secretBase->numSecretBasesReceived = 0;
    secretBase->numTimesEntered = 0;
    secretBase->unused = 0;
    secretBase->toRegister = 0;
    secretBase->gender = 0;
    secretBase->battledOwnerToday = 0;
    secretBase->registryStatus = 0;
    for (i = 0; i < 16; i++)
    {
        secretBase->decorations[i] = 0;
        secretBase->decorationPositions[i] = 0;
    }
    for (i = 0; i < 6; i++)
    {
        for (j = 0; j < 4; j++)
            secretBase->party.moves[i * 4 + j] = 0;
        secretBase->party.personality[i] = 0;
        secretBase->party.EVs[i] = 0;
        secretBase->party.species[i] = 0;
        secretBase->party.heldItems[i] = 0;
        secretBase->party.levels[i] = 0;
    }
}

void ResetSecretBase(u8 idx)
{
    ClearSecretBase(&gSaveBlock1.secretBases[idx]);
}

void ClearSecretBases(void)
{
    u16 i;
    for (i = 0; i < SECRET_BASES_COUNT; i++)
        ResetSecretBase(i);
}

static void SetCurSecretBaseId(void)
{
    sCurSecretBaseId = gSpecialVar_0x8004;
}

void TrySetCurSecretBaseIndex(void)
{
    u16 i;

    gSpecialVar_Result = 0;
    for (i = 0; i < SECRET_BASES_COUNT; i++)
    {
        if (sCurSecretBaseId == gSaveBlock1.secretBases[i].secretBaseId)
        {
            gSpecialVar_Result = 1;
            VarSet(VAR_CURRENT_SECRET_BASE, i);
            break;
        }
    }
}

void CheckPlayerHasSecretBase(void)
{
    if (gSaveBlock1.secretBases[0].secretBaseId)
        gSpecialVar_Result = 1;
    else
        gSpecialVar_Result = 0;
}

static u8 GetSecretBaseTypeInFrontOfPlayer_(void)
{
    s16 x, y;
    s16 behavior;
    GetXYCoordsOneStepInFrontOfPlayer(&x, &y);
    behavior = MapGridGetMetatileBehaviorAt(x, y) & 0xFFF;
    if (behavior == 0x90 || behavior == 0x91)
        return 1;
    else if (behavior == 0x92 || behavior == 0x93)
        return 2;
    else if (behavior == 0x9a || behavior == 0x9b)
        return 3;
    else if (behavior == 0x94 || behavior == 0x95)
        return 4;
    else if (behavior == 0x96 || behavior == 0x97 || behavior == 0x9c || behavior == 0x9d)
        return 5;
    else if (behavior == 0x98 || behavior == 0x99)
        return 6;
    return 0;
}

void GetSecretBaseTypeInFrontOfPlayer(void)
{
    gSpecialVar_0x8007 = GetSecretBaseTypeInFrontOfPlayer_();
}

s16 unref_sub_80BB724(u16 *a0, u8 a1)
{
    u16 v2;

    for (v2 = 0; v2 < 0x200; v2++)
    {
        if ((a0[v2] & 0xFFF) == a1)
            return v2;
    }
    return -1;
}

static void FindMetatileIdMapCoords(s16 *x, s16 *y, u16 metatileId)
{
    s16 i, j;

    for (j=0; j<gMapHeader.mapLayout->height; j++)
    {
        for (i=0; i<gMapHeader.mapLayout->width; i++)
        {
            if ((gMapHeader.mapLayout->map[j * gMapHeader.mapLayout->width + i] & 0x3ff) == metatileId)
            {
                *x = i;
                *y = j;
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
    for (i = 0; i < 7; i++)
    {
        if (sSecretBaseEntranceMetatiles[i].closedMetatileId == metatileId)
        {
            MapGridSetMetatileIdAt(x, y, sSecretBaseEntranceMetatiles[i].openMetatileId | 0xc00);
            CurrentMapDrawMetatileAt(x, y);
            return;
        }
    }
    for (i = 0; i < 7; i++)
    {
        if (sSecretBaseEntranceMetatiles[i].openMetatileId == metatileId)
        {
            MapGridSetMetatileIdAt(x, y, sSecretBaseEntranceMetatiles[i].closedMetatileId | 0xc00);
            CurrentMapDrawMetatileAt(x, y);
            return;
        }
    }
}

static u8 GetNameLength(const u8 *secretBaseOwnerName)
{
    u8 i;

    for (i=0; i<7; i++)
    {
        if (secretBaseOwnerName[i] == EOS)
            return i;
    }
    return 7;
}

void SetPlayerSecretBase(void)
{
    u8 nameLength;
    u16 i;

    gSaveBlock1.secretBases[0].secretBaseId = sCurSecretBaseId;
    for (i=0; i<4; i++)
        gSaveBlock1.secretBases[0].trainerId[i] = gSaveBlock2.playerTrainerId[i];
    VarSet(VAR_CURRENT_SECRET_BASE, 0);
    nameLength = GetNameLength(gSaveBlock2.playerName);
    memset(gSaveBlock1.secretBases[0].trainerName, 0xFF, OT_NAME_LENGTH);
    StringCopyN(gSaveBlock1.secretBases[0].trainerName, gSaveBlock2.playerName, nameLength);
    gSaveBlock1.secretBases[0].gender = gSaveBlock2.playerGender;
    VarSet(VAR_SECRET_BASE_MAP, gMapHeader.regionMapSectionId);
}

void SetOccupiedSecretBaseEntranceMetatiles(const struct MapEvents *events)
{
    u16 bgId, i, j;
    s16 tile_id;

    for (bgId = 0; bgId < events->bgEventCount; bgId++)
    {
        if (events->bgEvents[bgId].kind == BG_EVENT_SECRET_BASE)
        {
            for (j = 0; j < SECRET_BASES_COUNT; j++)
            {
                if (gSaveBlock1.secretBases[j].secretBaseId == events->bgEvents[bgId].bgUnion.secretBaseId)
                {
                    tile_id = MapGridGetMetatileIdAt(events->bgEvents[bgId].x + 7, events->bgEvents[bgId].y + 7);
                    for (i = 0; i < 7; i++)
                    {
                        if (sSecretBaseEntranceMetatiles[i].closedMetatileId == tile_id)
                        {
                            MapGridSetMetatileIdAt(events->bgEvents[bgId].x + 7, events->bgEvents[bgId].y + 7, sSecretBaseEntranceMetatiles[i].openMetatileId | 0xc00);
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }
}

static void SetSecretBaseWarpDestination(void)
{
    s8 secretBaseGroup = 4 * (sCurSecretBaseId / 10);
    SetWarpDestinationToMapWarp(MAP_GROUP(MAP_SECRET_BASE_RED_CAVE1), sSecretBaseEntrancePositions[secretBaseGroup], sSecretBaseEntrancePositions[secretBaseGroup + 1]);
}

static void Task_EnterSecretBase(u8 taskId)
{
    u16 secretBaseIdx;
    switch (gTasks[taskId].data[0])
    {
    case 0:
        gTasks[taskId].data[0] = 1;
        break;
    case 1:
        if (!gPaletteFade.active)
            gTasks[taskId].data[0] = 2;
        break;
    case 2:
        secretBaseIdx = VarGet(VAR_CURRENT_SECRET_BASE);
        if (gSaveBlock1.secretBases[secretBaseIdx].numTimesEntered < 0xff)
            gSaveBlock1.secretBases[secretBaseIdx].numTimesEntered++;
        SetSecretBaseWarpDestination();
        WarpIntoMap();
        gFieldCallback = FieldCB_ContinueScriptHandleMusic;
        SetMainCallback2(CB2_LoadMap);
        DestroyTask(taskId);
        break;
    }
}

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

static void EnterNewlyCreatedSecretBase_WaitFadeIn(u8 taskId)
{
    ObjectEventTurn(&(gObjectEvents[gPlayerAvatar.objectEventId]), 2);
    if (IsWeatherNotFadingIn() == 1)
    {
        ScriptContext_Enable();
        DestroyTask(taskId);
    }
}

static void EnterNewlyCreatedSecretBase_StartFadeIn(void)
{
    s16 x, y;

    LockPlayerFieldControls();
    HideMapNamePopUpWindow();
    FindMetatileIdMapCoords(&x, &y, 0x220);
    MapGridSetMetatileIdAt(x + 7, y + 7, 0xe20);
    CurrentMapDrawMetatileAt(x + 7, y + 7);
    FadeInFromBlack();
    CreateTask(EnterNewlyCreatedSecretBase_WaitFadeIn, 0);
}

static void Task_EnterNewlyCreatedSecretBase(u8 taskId)
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
    u8 taskid = CreateTask(Task_EnterNewlyCreatedSecretBase, 0);
    gTasks[taskid].data[0] = 0;
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
    u16 secretBaseIdx;
    u16 x, y;

    if (CurMapIsSecretBase())
    {
        secretBaseIdx = VarGet(VAR_CURRENT_SECRET_BASE);
        for (x = 0; x < 16; x++)
        {
            if (gSaveBlock1.secretBases[secretBaseIdx].decorations[x] > 0
             && gSaveBlock1.secretBases[secretBaseIdx].decorations[x] <= 0x78
             && gDecorations[gSaveBlock1.secretBases[secretBaseIdx].decorations[x]].permission != DECORPERM_SPRITE)
            {
                ShowDecorationOnMap(
                    (gSaveBlock1.secretBases[secretBaseIdx].decorationPositions[x] >> 4) + 7,
                    (gSaveBlock1.secretBases[secretBaseIdx].decorationPositions[x] & 0xF) + 7,
                    gSaveBlock1.secretBases[secretBaseIdx].decorations[x]);
            }
        }
        if (secretBaseIdx != 0)
        {
            FindMetatileIdMapCoords(&x, &y, 0x220);
            MapGridSetMetatileIdAt(x + 7, y + 7, 0xe21);
        }
        else if (hidePC == 1 && VarGet(VAR_SECRET_BASE_INITIALIZED) == 1)
        {
            FindMetatileIdMapCoords(&x, &y, 0x220);
            MapGridSetMetatileIdAt(x + 7, y + 7, 0xe0a);
        }
    }
}

void InitSecretBaseDecorationSprites(void)
{
    u8 *decorations;
    u8 *decorationPositions;
    u8 i;
    u8 objectEventId = 0;
    u8 metatileBehavior;
    u8 permission;
    u8 numDecorations;
    u16 secretBaseIdx = VarGet(VAR_CURRENT_SECRET_BASE);

    if (!CurMapIsSecretBase())
    {
        decorations = gSaveBlock1.playerRoomDecor;
        decorationPositions = gSaveBlock1.playerRoomDecorPos;
        numDecorations = 12;
    }
    else
    {
        decorations = gSaveBlock1.secretBases[secretBaseIdx].decorations;
        decorationPositions = gSaveBlock1.secretBases[secretBaseIdx].decorationPositions;
        numDecorations = 16;
    }
    for (i = 0; i < numDecorations; i++)
    {
        if (decorations[i] != DECOR_NONE)
        {
            permission = gDecorations[decorations[i]].permission;
            if (permission == DECORPERM_SPRITE)
            {
                for (objectEventId = 0; objectEventId < gMapHeader.events->objectEventCount; objectEventId++)
                {
                    if (gMapHeader.events->objectEvents[objectEventId].flagId == gSpecialVar_0x8004 + 0xAE)
                        break;
                }
                if (objectEventId != gMapHeader.events->objectEventCount)
                {
                    gSpecialVar_0x8006 = decorationPositions[i] >> 4;
                    gSpecialVar_0x8007 = decorationPositions[i] & 0xF;
                    metatileBehavior = MapGridGetMetatileBehaviorAt(gSpecialVar_0x8006 + 7, gSpecialVar_0x8007 + 7);
                    if (MetatileBehavior_HoldsSmallDecoration(metatileBehavior) == TRUE || MetatileBehavior_HoldsLargeDecoration(metatileBehavior) == TRUE)
                    {
                        gSpecialVar_Result = VAR_OBJ_GFX_ID_0 + (gMapHeader.events->objectEvents[objectEventId].graphicsId - OBJ_EVENT_GFX_VAR_0);
                        VarSet(gSpecialVar_Result, gDecorations[decorations[i]].tiles[0]);
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
    int curBase = VarGet(VAR_CURRENT_SECRET_BASE);
    VarSet(VAR_OBJ_GFX_ID_F, sSecretBaseOwnerGfxIds[GetSecretBaseOwnerType(curBase)]);
}

void SetCurSecretBaseIdFromPosition(const struct MapPosition *position, const struct MapEvents *events)
{
    s16 i;

    for (i = 0; i < events->bgEventCount; i++)
    {
        if (events->bgEvents[i].kind == BG_EVENT_SECRET_BASE && position->x == events->bgEvents[i].x + 7
         && position->y == events->bgEvents[i].y + 7)
        {
            sCurSecretBaseId = events->bgEvents[i].bgUnion.secretBaseId;
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
    if (gSpecialVar_Result == 1)
        return FALSE;
    return TRUE;
}

static void Task_WarpOutOfSecretBase(u8 taskId)
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
        SetWarpDestinationToDynamicWarp(0x7E);
        WarpIntoMap();
        gFieldCallback = FieldCB_DefaultWarpExit;
        SetMainCallback2(CB2_LoadMap);
        UnlockPlayerFieldControls();
        DestroyTask(taskId);
        break;
    }
}

static void WarpOutOfSecretBase(void)
{
    CreateTask(Task_WarpOutOfSecretBase, 0);
    FadeScreen(1, 0);
}

void IsCurSecretBaseOwnedByAnotherPlayer(void)
{
    if (gSaveBlock1.secretBases[0].secretBaseId != sCurSecretBaseId)
        gSpecialVar_Result = 1;
    else
        gSpecialVar_Result = 0;
}

u8 sub_80BC14C(u8 sbid)
{
    s16 idx;

    for (idx = 0; idx < SECRET_BASES_COUNT; idx++)
    {
        if (gSaveBlock1.secretBases[idx].secretBaseId == sbid)
            return idx;
    }
    return 0;
}

u8 *GetSecretBaseName(u8 *dest, u8 secretBaseIdx)
{
    u8 local1;
    u8 *str;

    local1 = GetNameLength(gSaveBlock1.secretBases[secretBaseIdx].trainerName);

    str = StringCopyN(dest, gSaveBlock1.secretBases[secretBaseIdx].trainerName, local1);
    str[0] = EOS;

#if ENGLISH
    return StringAppend(dest, gText_ApostropheSBase);
#elif GERMAN
    return de_sub_8073174(dest, gText_ApostropheSBase);
#endif
}

u8 *GetSecretBaseMapName(u8 *dest)
{
    sCurSecretBaseId = gSaveBlock1.secretBases[VarGet(VAR_CURRENT_SECRET_BASE)].secretBaseId;
    return GetSecretBaseName(dest, VarGet(VAR_CURRENT_SECRET_BASE));
}

void CopyCurSecretBaseOwnerName_StrVar1(void)
{
    u8 *name = gSaveBlock1.secretBases[(u8)VarGet(VAR_CURRENT_SECRET_BASE)].trainerName;
    u8 *var1 = gStringVar1;
    u8 var2 = GetNameLength(name);
    u8 *var3 = StringCopyN(var1, name, var2);
    *var3 = EOS;
}

static bool8 IsSecretBaseRegistered(u8 secretBaseIdx)
{
    if (gSaveBlock1.secretBases[secretBaseIdx].registryStatus)
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
    u16 i;
    u16 partyId = 0;
    struct SecretBase *record = &gSaveBlock1.secretBases[0];
    for (i = 0; i < PARTY_SIZE; i++)
    {
        u16 moveIndex;
        for (moveIndex = 0; moveIndex < 4; moveIndex++)
            record->party.moves[i * 4 + moveIndex] = MOVE_NONE;

        record->party.species[i] = SPECIES_NONE;
        record->party.heldItems[i] = ITEM_NONE;
        record->party.levels[i] = 0;
        record->party.personality[i] = 0;
        record->party.EVs[i] = 0;

        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) != SPECIES_NONE && !GetMonData(&gPlayerParty[i], MON_DATA_IS_EGG))
        {
            for (moveIndex = 0; moveIndex < 4; moveIndex++)
                record->party.moves[partyId * 4 + moveIndex] = GetMonData(&gPlayerParty[i], MON_DATA_MOVE1 + moveIndex);

            record->party.species[partyId] = GetMonData(&(gPlayerParty[i]), MON_DATA_SPECIES);
            record->party.heldItems[partyId] = GetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM);
            record->party.levels[partyId] = GetMonData(&gPlayerParty[i], MON_DATA_LEVEL);
            record->party.personality[partyId] = GetMonData(&gPlayerParty[i], MON_DATA_PERSONALITY);
            record->party.EVs[partyId] = GetAverageEVs(&gPlayerParty[i]);
            partyId++;
        }
    }
}

void ClearAndLeaveSecretBase(void)
{
    u16 temp = gSaveBlock1.secretBases[0].numSecretBasesReceived;
    ResetSecretBase(0);
    gSaveBlock1.secretBases[0].numSecretBasesReceived = temp;
    WarpOutOfSecretBase();
}

void MoveOutOfSecretBase(void)
{
    IncrementGameStat(GAME_STAT_MOVED_SECRET_BASE);
    ClearAndLeaveSecretBase();
}

static void ClosePlayerSecretBaseEntrance(void)
{
    u16 i;
    const struct MapEvents *events = gMapHeader.events;

    for (i = 0; i < events->bgEventCount; i++)
    {
        if (events->bgEvents[i].kind == BG_EVENT_SECRET_BASE
         && gSaveBlock1.secretBases[0].secretBaseId == events->bgEvents[i].bgUnion.secretBaseId)
        {
            u16 j;
            s16 metatileId = MapGridGetMetatileIdAt(events->bgEvents[i].x + 7, events->bgEvents[i].y + 7);

            for (j = 0; j < 7; j++)
            {
                if (sSecretBaseEntranceMetatiles[j].openMetatileId == metatileId)
                {
                    MapGridSetMetatileIdAt(
                        events->bgEvents[i].x + 7,
                        events->bgEvents[i].y + 7,
                        sSecretBaseEntranceMetatiles[j].closedMetatileId | 0xc00);
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
    u16 temp;
    ClosePlayerSecretBaseEntrance();
    IncrementGameStat(GAME_STAT_MOVED_SECRET_BASE);

    temp = gSaveBlock1.secretBases[0].numSecretBasesReceived;
    ResetSecretBase(0);
    gSaveBlock1.secretBases[0].numSecretBasesReceived = temp;
}

static u8 GetNumRegisteredSecretBases(void)
{
    s16 i;
    u8 count = 0;

    for (i = 1; i < SECRET_BASES_COUNT; i++)
    {
        if (IsSecretBaseRegistered(i) == TRUE)
            count++;
    }
    return count;
}

void GetCurSecretBaseRegistrationValidity(void)
{
    if (IsSecretBaseRegistered(sub_80BC14C(sCurSecretBaseId)) == TRUE)
        gSpecialVar_Result = 1;
    else if (GetNumRegisteredSecretBases() > 9)
        gSpecialVar_Result = 2;
    else
        gSpecialVar_Result = 0;
}

void ToggleCurSecretBaseRegistry(void)
{
    gSaveBlock1.secretBases[sub_80BC14C(sCurSecretBaseId)].registryStatus ^= 1;
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

static void Task_ShowSecretBaseRegistryMenu(u8 taskId)
{
    s16 *data;

    LockPlayerFieldControls();
    ClearVerticalScrollIndicatorPalettes();
    LoadScrollIndicatorPalette();

    data = gTasks[taskId].data;
    data[0] = GetNumRegisteredSecretBases();
    if (data[0] != 0)
    {
        if (data[0] > 7)
            data[3] = 7;
        else
            data[3] = data[0];

        data[1] = 0;
        data[2] = 0;

        Menu_EraseWindowRect(0, 0, 29, 19);
        FinalizeRegistryMenu(taskId);

        gTasks[taskId].func = HandleRegistryMenuInput;
    }
    else
    {
        DisplayItemMessageOnField(taskId, gText_NoRegistry, GoToSecretBasePCRegisterMenu, 0);
    }
}

static void BuildRegistryMenuItems(u8 taskId)
{
    u8 i;
    s16 *data = gTasks[taskId].data;
    u8 m = 0;
    u8 n = 0;

    for (i = 1; i < SECRET_BASES_COUNT; i++)
    {
        if (m == data[2])
        {
            m = i;
            break;
        }

        if (IsSecretBaseRegistered(i) == TRUE)
            m++;
    }

    for (i = m; i < SECRET_BASES_COUNT; i++)
    {
        if (IsSecretBaseRegistered(i) == TRUE)
        {
            GetSecretBaseName(gStringVar1, i);
            Menu_BlankWindowRect(18, 2 * n + 2, 28, 2 * n + 3);
            Menu_PrintText(gStringVar1, 18, 2 * n + 2);
            if (++n == 8)
                break;
        }
    }

    if (n < 8)
    {
        Menu_BlankWindowRect(18, 2 * n + 2, 28, 2 * n + 3);
        Menu_PrintText(gOtherText_Exit, 18, 2 * n + 2);
        DestroyVerticalScrollIndicator(BOTTOM_ARROW);
        if (n != 7)
        {
            n++;
            n--;
            Menu_BlankWindowRect(18, n * 2 + 4, 28, 18);
        }
    }
    else
    {
        CreateVerticalScrollIndicators(BOTTOM_ARROW, 0xbc, 0x98);
    }

    if (data[2] == 0)
        DestroyVerticalScrollIndicator(TOP_ARROW);
    else
        CreateVerticalScrollIndicators(TOP_ARROW, 0xbc, 0x08);
}

static void FinalizeRegistryMenu(u8 taskId)
{
    u16 *data = gTasks[taskId].data;
    Menu_DrawStdWindowFrame(17, 0, 29, 19);
    InitMenu(0, 18, 2, data[3] + 1, data[1], 11);

    BuildRegistryMenuItems(taskId);
}

static void HandleRegistryMenuInput(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    if (JOY_REPEAT(DPAD_UP))
    {
        if (data[1])
        {
            PlaySE(SE_SELECT);
            data[1] = Menu_MoveCursor(-1);
        }
        else if (data[2])
        {
            PlaySE(SE_SELECT);
            data[2]--;
            BuildRegistryMenuItems(taskId);
        }
    }
    else if (JOY_REPEAT(DPAD_DOWN))
    {
        if (data[1] == data[3])
        {
            if (data[2] + data[1] != data[0])
            {
                PlaySE(SE_SELECT);
                data[2]++;
                BuildRegistryMenuItems(taskId);
            }
        }
        else
        {
            PlaySE(SE_SELECT);
            data[1] = Menu_MoveCursor(1);
        }
    }
    else if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (data[1] + data[2] == data[0])
        {
            Menu_DestroyCursor();
            Menu_EraseWindowRect(0, 0, 29, 19);
            GoToSecretBasePCRegisterMenu(taskId);
        }
        else
        {
            Menu_DestroyCursor();
            data[4] = sub_80BC948(data[1] + data[2]);
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

u8 sub_80BC948(u8 a)
{
    u8 secretBaseIndex;
    u8 count = 0;

    for (secretBaseIndex = 1; secretBaseIndex < SECRET_BASES_COUNT; secretBaseIndex++)
    {
        if (IsSecretBaseRegistered(secretBaseIndex) == TRUE)
        {
            if (a == count)
            {
                return secretBaseIndex;
            }
            
            count++;
        }
    }

    return 0;
}

static void ShowRegistryMenuActions(u8 taskId)
{
    PauseVerticalScrollIndicator(TOP_ARROW);
    PauseVerticalScrollIndicator(BOTTOM_ARROW);
    Menu_DrawStdWindowFrame(1, 0, 12, 5);
    Menu_PrintItems(2, 1, 2, (const struct MenuAction *)sRegistryMenuActions);
    InitMenu(0, 2, 1, 2, 0, 10);
    gTasks[taskId].func = HandleRegistryMenuActionsInput;
}

static void HandleRegistryMenuActionsInput(u8 taskId)
{
    if (JOY_REPEAT(DPAD_UP))
    {
        if (Menu_GetCursorPos())
        {
            PlaySE(SE_SELECT);
            Menu_MoveCursor(-1);
        }
    }
    else if (JOY_REPEAT(DPAD_DOWN))
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

static void ShowRegistryMenuDeleteConfirmation(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    DestroyVerticalScrollIndicator(TOP_ARROW);
    DestroyVerticalScrollIndicator(BOTTOM_ARROW);
    Menu_DestroyCursor();
    Menu_EraseWindowRect(0, 0, 29, 19);

    GetSecretBaseName(gStringVar1, data[4]);
    StringExpandPlaceholders(gStringVar4, gText_OkayToDeleteFromRegistry);
    DisplayItemMessageOnField(taskId, gStringVar4, ShowRegistryMenuDeleteYesNo, 0);
}

static void ShowRegistryMenuDeleteYesNo(u8 taskId)
{
    DisplayYesNoMenu(20, 8, 1);
    DoYesNoFuncWithChoice(taskId, &sDeleteRegistryYesNoFuncs);
}

void DeleteRegistry_Yes_Callback(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    Menu_EraseWindowRect(0, 0, 29, 19);

    gSaveBlock1.secretBases[data[4]].registryStatus = 0;
    data[0]--;

    if (data[2] > 0)
        data[2]--;

    if (data[0] < 8)
        data[3]--;

    FinalizeRegistryMenu(taskId);
    gTasks[taskId].func = HandleRegistryMenuInput;
}

static void DeleteRegistry_Yes(u8 taskId)
{
    Menu_EraseWindowRect(20, 8, 26, 13);
    DisplayItemMessageOnField(taskId, gText_RegisteredDataDeleted, DeleteRegistry_Yes_Callback, 0);
}

static void DeleteRegistry_No(u8 taskId)
{
    Menu_EraseWindowRect(0, 0, 29, 19);

    FinalizeRegistryMenu(taskId);
    gTasks[taskId].func = HandleRegistryMenuInput;
}

static void ReturnToMainRegistryMenu(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    InitMenu(0, 18, 2, data[3] + 1, data[1], 11);
    Menu_EraseWindowRect(1, 0, 12, 5);
    StartVerticalScrollIndicators(TOP_ARROW);
    StartVerticalScrollIndicators(BOTTOM_ARROW);

    gTasks[taskId].func = HandleRegistryMenuInput;
}

static void GoToSecretBasePCRegisterMenu(u8 taskId)
{
    u16 curBaseIndex = VarGet(VAR_CURRENT_SECRET_BASE);

    BuyMenuFreeMemory();
    DestroyVerticalScrollIndicator(TOP_ARROW);
    DestroyVerticalScrollIndicator(BOTTOM_ARROW);

    if (curBaseIndex == 0)
        ScriptContext_SetupScript(SecretBase_EventScript_PCCancel);
    else
        ScriptContext_SetupScript(SecretBase_EventScript_ShowRegisterMenu);

    DestroyTask(taskId);
}

static u8 GetSecretBaseOwnerType(u8 secretBaseIdx)
{
    return (gSaveBlock1.secretBases[secretBaseIdx].trainerName[OT_NAME_LENGTH] % 5)
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
void unref_sub_80BCD7C(u8 secretBaseIndex)
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

void sub_80BCE1C(void)
{
    u16 curBaseIndex = VarGet(VAR_CURRENT_SECRET_BASE);

    sub_810FB10(1);
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

void SecretBasePerStepCallback(u8 taskId)
{
    s16 x, y;
    u32 behavior;
    s16 *data = gTasks[taskId].data;

    switch (data[1])
    {
    case 0:
        PlayerGetDestCoords(&data[2], &data[3]);
        data[1] = 1;
        break;
    case 1:
        PlayerGetDestCoords(&x, &y);
        if (x != data[2] || y != data[3])
        {
            data[2] = x;
            data[3] = y;

            behavior = MapGridGetMetatileBehaviorAt(x, y);
            if (MetatileBehavior_IsSecretBaseGlitterMat(behavior) == TRUE)
                DoSecretBaseGlitterMatSparkle();
            else if (MetatileBehavior_IsSecretBaseBalloon(behavior) == TRUE)
                PopSecretBaseBalloon(MapGridGetMetatileIdAt(x, y), x, y);
            else if (MetatileBehavior_IsSecretBaseBreakableDoor(behavior) == TRUE)
                ShatterSecretBaseBreakableDoor(x, y);
            else if (MetatileBehavior_IsSecretBaseSoundMat(behavior) == TRUE)
                PlaySecretBaseMusicNoteMatSound(MapGridGetMetatileIdAt(x, y));
        }
        break;
    case 2:
        if (!FieldEffectActiveListContains(data[4]))
            data[1] = 1;
        break;
    }
}

static void SaveSecretBase(u8 secretBaseIdx, struct SecretBase *secretBase)
{
    gSaveBlock1.secretBases[secretBaseIdx] = *secretBase;
    gSaveBlock1.secretBases[secretBaseIdx].registryStatus = 2;
}

static bool8 SecretBasesHaveSameTrainerId(struct SecretBase *secretBase1, struct SecretBase *secretBase2)
{
    u8 i;

    for (i = 0; i < 4; i++)
    {
        if (secretBase1->trainerId[i] != secretBase2->trainerId[i])
            return FALSE;
    }

    return TRUE;
}

static bool8 SecretBasesHaveSameTrainerName(struct SecretBase *sbr1, struct SecretBase *sbr2)
{
    u8 i;

    for (i = 0; i < OT_NAME_LENGTH && (sbr1->trainerName[i] != 0xFF || sbr2->trainerName[i] != 0xFF); i++)
    {
        if (sbr1->trainerName[i] != sbr2->trainerName[i])
            return FALSE;
    }

    return TRUE;
}

static bool8 SecretBasesBelongToSamePlayer(struct SecretBase *secretBase1, struct SecretBase *secretBase2)
{
    if (secretBase1->gender == secretBase2->gender && SecretBasesHaveSameTrainerId(secretBase1, secretBase2) && SecretBasesHaveSameTrainerName(secretBase1, secretBase2))
        return TRUE;
    else
        return FALSE;
}

static s16 GetSecretBaseIndexFromId(u8 secretBaseId)
{
    s16 i;

    for (i = 0; i < SECRET_BASES_COUNT; i++)
    {
        if (gSaveBlock1.secretBases[i].secretBaseId == secretBaseId)
            return i;
    }

    return -1;
}

static u8 FindAvailableSecretBaseIndex(void)
{
    s16 i;

    for (i = 1; i < SECRET_BASES_COUNT; i++)
    {
        if (gSaveBlock1.secretBases[i].secretBaseId == 0)
            return i;
    }

    return 0;
}

static u8 FindUnregisteredSecretBaseIndex(void)
{
    s16 i;

    for (i = 1; i < SECRET_BASES_COUNT; i++)
    {
        if (gSaveBlock1.secretBases[i].registryStatus == 0 && gSaveBlock1.secretBases[i].toRegister == 0)
            return i;
    }

    return 0;
}

static u8 TrySaveFriendsSecretBase(struct SecretBase *secretBase)
{
    s16 index;

    if (secretBase->secretBaseId == 0)
        return 0;

    index = GetSecretBaseIndexFromId(secretBase->secretBaseId);
    if (index != 0)
    {
        if (index != -1)
        {
            if (gSaveBlock1.secretBases[index].toRegister == 1)
                return 0;
            if (gSaveBlock1.secretBases[index].registryStatus != 2
             || secretBase->toRegister == 1)
            {
                SaveSecretBase(index, secretBase);
                return index;
            }
        }
        else
        {
            index = FindAvailableSecretBaseIndex();
            if (index != 0)
            {
                SaveSecretBase(index, secretBase);
                return index;
            }
            index = FindUnregisteredSecretBaseIndex();
            if (index)
            {
                SaveSecretBase(index, secretBase);
                return index;
            }
        }
    }

    return 0;
}

static void SortSecretBasesByRegistryStatus(void)
{
    u8 i;
    u8 j;
    struct SecretBase temp;
    struct SecretBase *secretBases = gSaveBlock1.secretBases;

    for (i = 1; i < SECRET_BASES_COUNT - 1; i++)
    {
        for (j = i + 1; j < SECRET_BASES_COUNT; j++)
        {
            if ((!secretBases[i].registryStatus && secretBases[j].registryStatus == 1)
             || (secretBases[i].registryStatus == 2 && secretBases[j].registryStatus != 2))
            {
                temp = secretBases[i];
                secretBases[i] = secretBases[j];
                secretBases[j] = temp;
            }
        }
    }
}

static void TrySaveFriendsSecretBases(struct SecretBase *secretBases, u8 registryStatus)
{
    u16 i;

    for (i = 1; i < SECRET_BASES_COUNT; i++)
    {
        if (secretBases[i].registryStatus == registryStatus)
            TrySaveFriendsSecretBase(&secretBases[i]);
    }
}

static bool8 SecretBaseBelongsToPlayer(struct SecretBase *secretBase)
{
    u8 i;

    if (!secretBase->secretBaseId)
        return FALSE;

    if (secretBase->secretBaseId && secretBase->gender != gSaveBlock2.playerGender)
        return FALSE;

    // Check if the player's trainer Id matches the secret base's id.
    for (i = 0; i < 4; i++)
    {
        if (secretBase->trainerId[i] != gSaveBlock2.playerTrainerId[i])
            return FALSE;
    }

    for (i = 0; i < OT_NAME_LENGTH && (secretBase->trainerName[i] != 0xFF || gSaveBlock2.playerName[i] != 0xFF); i++)
    {
        if (secretBase->trainerName[i] != gSaveBlock2.playerName[i])
            return FALSE;
    }

    return TRUE;
}

static void DeleteFirstOldBaseFromPlayerInRecordMixingFriendsRecords(struct SecretBase *basesA, struct SecretBase *basesB, struct SecretBase *basesC)
{
    u8 i;
    u8 sbFlags = 0;

    for (i = 0; i < SECRET_BASES_COUNT; i++)
    {
        if ((sbFlags & 1) == 0)
        {
            if (SecretBaseBelongsToPlayer(&basesA[i]) == TRUE)
            {
                ClearSecretBase(&basesA[i]);
                sbFlags |= 1;
            }
        }

        if ((sbFlags & 2) == 0)
        {
            if (SecretBaseBelongsToPlayer(&basesB[i]) == TRUE)
            {
                ClearSecretBase(&basesB[i]);
                sbFlags |= 2;
            }
        }

        if ((sbFlags & 4) == 0)
        {
            if (SecretBaseBelongsToPlayer(&basesC[i]) == TRUE)
            {
                ClearSecretBase(&basesC[i]);
                sbFlags |= 4;
            }
        }

        if (sbFlags == 7)
            break;
    }
}

static bool8 ClearDuplicateOwnedSecretBase(struct SecretBase *secretBase, struct SecretBase *secretBases, u8 idx)
{
    u8 i;

    for (i = 0; i < SECRET_BASES_COUNT; i++)
    {
        if (secretBases[i].secretBaseId)
        {
            if (SecretBasesBelongToSamePlayer(secretBase, &secretBases[i]) == TRUE)
            {
                if (idx == 0)
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

static void ClearDuplicateOwnedSecretBases(struct SecretBase *playersBases, struct SecretBase *friendsBasesA, struct SecretBase *friendsBasesB, struct SecretBase *friendsBasesC)
{
    u8 i;

    for (i = 1; i < SECRET_BASES_COUNT; i++)
    {
        if (playersBases[i].secretBaseId)
        {
            if (playersBases[i].registryStatus == 1)
                playersBases[i].toRegister = 1;

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
            friendsBasesA[i].battledOwnerToday = 0;

            if (!ClearDuplicateOwnedSecretBase(&friendsBasesA[i], friendsBasesB, i))
                ClearDuplicateOwnedSecretBase(&friendsBasesA[i], friendsBasesC, i);
        }
    }

    for (i = 0; i < SECRET_BASES_COUNT; i++)
    {
        if (friendsBasesB[i].secretBaseId)
        {
            friendsBasesB[i].battledOwnerToday = 0;
            ClearDuplicateOwnedSecretBase(&friendsBasesB[i], friendsBasesC, i);
        }

        if (friendsBasesC[i].secretBaseId)
            friendsBasesC[i].battledOwnerToday = 0;
    }
}

static void SaveRecordMixBases(struct SecretBase *basesA, struct SecretBase *basesB, struct SecretBase *basesC)
{
    DeleteFirstOldBaseFromPlayerInRecordMixingFriendsRecords(basesA, basesB, basesC);
    ClearDuplicateOwnedSecretBases(gSaveBlock1.secretBases, basesA, basesB, basesC);

    TrySaveFriendsSecretBase(basesA);
    TrySaveFriendsSecretBase(basesB);
    TrySaveFriendsSecretBase(basesC);

    TrySaveFriendsSecretBases(basesA, 1);
    TrySaveFriendsSecretBases(basesB, 1);
    TrySaveFriendsSecretBases(basesC, 1);

    TrySaveFriendsSecretBases(basesA, 0);
    TrySaveFriendsSecretBases(basesB, 0);
    TrySaveFriendsSecretBases(basesC, 0);
} 

void ReceiveSecretBasesData(void *secretBases, u32 recordSize, u8 linkIdx)
{
    if (
#if DEBUG
     gUnknown_020297ED != 0 ||
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
            if (gSaveBlock1.secretBases[i].toRegister == 1)
            {
                gSaveBlock1.secretBases[i].registryStatus = 1;
                gSaveBlock1.secretBases[i].toRegister = 0;
            }
        }

        SortSecretBasesByRegistryStatus();

        for (i = 1; i < SECRET_BASES_COUNT; i++)
        {
            if (gSaveBlock1.secretBases[i].registryStatus == 2)
                gSaveBlock1.secretBases[i].registryStatus = 0;
        }

        if (gSaveBlock1.secretBases[0].numSecretBasesReceived != 0xFFFF)
            gSaveBlock1.secretBases[0].numSecretBasesReceived++;
    }
}
