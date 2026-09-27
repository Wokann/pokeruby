#include "global.h"
#include "item_use.h"
#include "battle.h"
#include "berry.h"
#include "bike.h"
#include "coins.h"
#include "data2.h"
#include "event_data.h"
#include "field_effect.h"
#include "field_fadetransition.h"
#include "field_player_avatar.h"
#include "field_weather.h"
#include "fieldmap.h"
#include "item.h"
#include "item_menu.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "mail.h"
#include "main.h"
#include "event_object_lock.h"
#include "menu.h"
#include "menu_helpers.h"
#include "metatile_behavior.h"
#include "palette.h"
#include "party_menu.h"
#include "pokeblock.h"
#include "pokemon_item_effect.h"
#include "pokemon_menu.h"
#include "overworld.h"
#include "rom_8094928.h"
#include "script.h"
#include "constants/songs.h"
#include "sound.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "constants/event_bg.h"
#include "constants/event_objects.h"
#include "constants/item_effects.h"
#include "constants/map_types.h"
#include "constants/species.h"
#include "constants/vars.h"

extern void (*gFieldItemUseCallback)(u8);
extern void (*gFieldCallback)(void);
extern void (*gPokemonItemUseCallback)(u8, u16, TaskFunc);

extern u8 gPokemonItemUseType;
extern u8 gLastFieldPokeMenuOpened;
extern u8 gBattlerInMenuId;

extern u8 BerryTree_EventScript_ItemUsePlantBerry[];
extern u8 BerryTree_EventScript_ItemUseWailmerPail[];

extern u16 gBattlerPartyIndexes[];

extern u16 gBattleTypeFlags;

#define tUsingRegisteredKeyItem data[2]
#define tCallbackHigh           data[8]
#define tCallbackLow            data[9]
#define tEnigmaBerryType        data[15]
#define tItemUseTimer           data[15]

static void ItemUseOnFieldCB_Berry(u8);
static void ItemUseOnFieldCB_WailmerPailBerry(u8);
static void UseTMHM(u8);
static void UseTMHMYesNo(u8);
static void BootUpSoundTMHM(u8);
static void Task_ShowTMHMContainedMessage(u8);

static const u8 sSSTidalBetaString[] = _("この　チケットで　ふねに　のりほうだい\nはやく　のってみたいな");
static const u8 sSSTidalBetaString2[] = _("この　チケットで　ふねに　のりほうだい\nはやく　のってみたいな");

static const u8 *const sSSTidalBetaStrings[] =
{
    sSSTidalBetaString,
    sSSTidalBetaString2,
};

static const MainCallback sItemUseCallbacks[] =
{
    [ITEM_USE_PARTY_MENU - 1]  = sub_808B020,
    [ITEM_USE_FIELD - 1]       = CB2_ReturnToField,
    [ITEM_USE_PBLOCK_CASE - 1] = sub_810B96C,
};

static const u8 sClockwiseDirections[] = { DIR_NORTH, DIR_EAST, DIR_SOUTH, DIR_WEST };

static const struct YesNoFuncTable sUseTMHMYesNoFuncTable =
{
    .yesFunc = UseTMHM,
    .noFunc = CleanUpItemMenuMessage,
};

void SetUpItemUseCallback(u8 taskId)
{
    u8 itemUseTypeIndex;

    if (gSpecialVar_ItemId == ITEM_ENIGMA_BERRY)
        itemUseTypeIndex = gTasks[taskId].tEnigmaBerryType - 1;
    else
        itemUseTypeIndex = ItemId_GetType(gSpecialVar_ItemId) - 1;

    gTasks[taskId].tCallbackHigh = (u32)sItemUseCallbacks[itemUseTypeIndex] >> 16;
    gTasks[taskId].tCallbackLow = (u32)sItemUseCallbacks[itemUseTypeIndex];
    gTasks[taskId].func = HandleItemMenuPaletteFade;
}

void ItemMenu_ConfirmNormalFade(u8 var)
{
    SetUpItemUseCallback(var);
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
}

void ItemMenu_ConfirmComplexFade(u8 var)
{
    SetUpItemUseCallback(var);
    FadeScreen(1, 0);
}

void SetUpItemUseOnFieldCallback(u8 taskId)
{
    if (gTasks[taskId].tUsingRegisteredKeyItem != 1)
    {
        gFieldCallback = ExecuteItemUseFromBlackPalette;
        ItemMenu_ConfirmNormalFade(taskId);
    }
    else
    {
        gFieldItemUseCallback(taskId);
    }
}

static void DisplayCannotUseItemMessage(u8 taskId, u8 isUsingRegisteredKeyItemOnField, const u8 *text)
{
    StringExpandPlaceholders(gStringVar4, text);

    switch (isUsingRegisteredKeyItemOnField)
    {
    case 0: // Item Menu
        Menu_EraseWindowRect(0, 13, 13, 20);
        DisplayItemMessageOnField(taskId, gStringVar4, CleanUpItemMenuMessage, 1);
        break;
    default: // Field
        DisplayItemMessageOnField(taskId, gStringVar4, CleanUpOverworldMessage, 0);
        break;
    }
}

void DisplayDadsAdviceCannotUseItemMessage(u8 taskId, u8 isUsingRegisteredKeyItemOnField)
{
    DisplayCannotUseItemMessage(taskId, isUsingRegisteredKeyItemOnField, gOtherText_DadsAdvice);
}

void DisplayCannotDismountBikeMessage(u8 taskId, u8 isUsingRegisteredKeyItemOnField)
{
    DisplayCannotUseItemMessage(taskId, isUsingRegisteredKeyItemOnField, gOtherText_CantGetOffBike);
}

u8 CheckIfItemIsTMHMOrEvolutionStone(u16 itemId)
{
    if (ItemId_GetFieldFunc(itemId) == ItemUseOutOfBattle_TMHM)
        return ITEM_IS_TM_HM;
    else if (ItemId_GetFieldFunc(itemId) == ItemUseOutOfBattle_EvolutionStone)
        return ITEM_IS_EVOLUTION_STONE;
    else
        return ITEM_IS_OTHER;
}

void Task_ReadMailFromBag(u8 taskId)
{
    struct MailStruct mailStruct;

    if (!gPaletteFade.active)
    {
        mailStruct.itemId = gSpecialVar_ItemId;
        ReadMail(&mailStruct, sub_80A5D04, 0);
        DestroyTask(taskId);
    }
}

void ItemUseOutOfBattle_Mail(u8 taskId)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = Task_ReadMailFromBag;
}

void ItemUseOutOfBattle_Bike(u8 taskId)
{
    s16 x, y;
    u8 tileBehavior;

    PlayerGetDestCoords(&x, &y);
    tileBehavior = MapGridGetMetatileBehaviorAt(x, y);

    if (FlagGet(FLAG_SYS_CYCLING_ROAD) == TRUE
        || MetatileBehavior_IsVerticalRail(tileBehavior) == TRUE
        || MetatileBehavior_IsHorizontalRail(tileBehavior) == TRUE
        || MetatileBehavior_IsIsolatedVerticalRail(tileBehavior) == TRUE
        || MetatileBehavior_IsIsolatedHorizontalRail(tileBehavior) == TRUE)
    {
        DisplayCannotDismountBikeMessage(taskId, gTasks[taskId].tUsingRegisteredKeyItem);
    }
    else
    {
        if (Overworld_IsBikingAllowed() == TRUE && IsBikingDisallowedByPlayer() == FALSE)
        {
            gFieldItemUseCallback = (void *)ItemUseOnFieldCB_Bike;
            SetUpItemUseOnFieldCallback(taskId);
        }
        else
            DisplayDadsAdviceCannotUseItemMessage(taskId, gTasks[taskId].tUsingRegisteredKeyItem);
    }
}

void ItemUseOnFieldCB_Bike(u8 taskId)
{
    if (ItemId_GetSecondaryId(gSpecialVar_ItemId) == 0)
        GetOnOffBike(PLAYER_AVATAR_FLAG_MACH_BIKE);
    if (ItemId_GetSecondaryId(gSpecialVar_ItemId) == 1)
        GetOnOffBike(PLAYER_AVATAR_FLAG_ACRO_BIKE);

    ScriptUnfreezeObjectEvents();
    UnlockPlayerFieldControls();
    DestroyTask(taskId);
}

bool32 CanFish(void)
{
    s16 x, y;
    u16 tileBehavior;

    GetXYCoordsOneStepInFrontOfPlayer(&x, &y);
    tileBehavior = MapGridGetMetatileBehaviorAt(x, y);

    if (MetatileBehavior_IsWaterfall(tileBehavior))
        return FALSE;

    if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_UNDERWATER))
        return FALSE;

    if (!TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_SURFING))
    {
        if (IsPlayerFacingSurfableFishableWater())
            return TRUE;
    }
    else
    {
        if (MetatileBehavior_IsSurfableWaterOrUnderwater(tileBehavior) && MapGridGetCollisionAt(x, y) == 0)
            return TRUE;
        if (MetatileBehavior_IsBridge(tileBehavior) == TRUE)
            return TRUE;
    }

    return FALSE;
}

void ItemUseOutOfBattle_Rod(u8 taskId)
{
    if (CanFish() == TRUE)
    {
        gFieldItemUseCallback = (void *)ItemUseOnFieldCB_Rod;
        SetUpItemUseOnFieldCallback(taskId);
    }
    else
        DisplayDadsAdviceCannotUseItemMessage(taskId, gTasks[taskId].tUsingRegisteredKeyItem);
}

void ItemUseOnFieldCB_Rod(u8 taskId)
{
    StartFishing(ItemId_GetSecondaryId(gSpecialVar_ItemId));
    DestroyTask(taskId);
}

void ItemUseOutOfBattle_Itemfinder(u8 taskId)
{
    IncrementGameStat(GAME_STAT_USED_ITEMFINDER);
    gFieldItemUseCallback = (void *)ItemUseOnFieldCB_Itemfinder;
    SetUpItemUseOnFieldCallback(taskId);
}

void ItemUseOnFieldCB_Itemfinder(u8 taskId)
{
    if (ItemfinderCheckForHiddenItems(gMapHeader.events, taskId) == TRUE)
        gTasks[taskId].func = Task_UseItemfinder;
    else
        DisplayItemMessageOnField(taskId, gOtherText_NoResponse, Task_CloseItemfinderMessage, 0);
}

#define tItemDistanceX   data[0]
#define tItemDistanceY   data[1]
#define tItemFound       data[2]
#define tCounter         data[3]
#define tItemfinderBeeps data[4]
#define tFacingDir       data[5]

void Task_UseItemfinder(u8 taskId)
{
    u8 playerDir;
    u8 playerDirToItem;
    u8 i;
    s16 *data = gTasks[taskId].data;

    if (tCounter == 0)
    {
        if (tItemfinderBeeps == 4)
        {
            playerDirToItem = GetDirectionToHiddenItem(tItemDistanceX, tItemDistanceY);
            if (playerDirToItem != DIR_NONE)
            {
                PlayerFaceHiddenItem(sClockwiseDirections[playerDirToItem - 1]);
                gTasks[taskId].func = Task_HiddenItemNearby;
            }
            else // The player is standing on the hidden item.
            {
                playerDir = GetPlayerFacingDirection();

                // rotate player clockwise depending on current direction.
                for (i = 0; i < ARRAY_COUNT(sClockwiseDirections); i++)
                    if (playerDir == sClockwiseDirections[i])
                        tFacingDir = (i + 1) & 3;

                gTasks[taskId].func = Task_StandingOnHiddenItem;
                tCounter = 0;
                tItemFound = FALSE;
            }
            return;
        }
        PlaySE(SE_ITEMFINDER); // play the itemfinder jingle 4 times before executing the itemfinder.
        tItemfinderBeeps++;
    }
    tCounter = (tCounter + 1) & 0x1F;
}

void Task_CloseItemfinderMessage(u8 taskId)
{
    Menu_EraseWindowRect(0, 14, 29, 19);
    ScriptUnfreezeObjectEvents();
    UnlockPlayerFieldControls();
    DestroyTask(taskId);
}

bool8 ItemfinderCheckForHiddenItems(const struct MapEvents *events, u8 taskId)
{
    int distanceX, distanceY;
    u16 x, y;
    s16 newDistanceX, newDistanceY, i;

    PlayerGetDestCoords(&x, &y);
    gTasks[taskId].tItemFound = FALSE;

    for (i = 0; i < events->bgEventCount; i++)
    {
        if ((events->bgEvents[i].kind == BG_EVENT_HIDDEN_ITEM) && !FlagGet(events->bgEvents[i].bgUnion.hiddenItem.hiddenItemId + FLAG_HIDDEN_ITEMS_START))
        {
            // do a distance lookup of each item so long as the index remains less than the objects on the current map.
            distanceX = (u16)events->bgEvents[i].x + 7;
            newDistanceX = distanceX - x;
            distanceY = (u16)events->bgEvents[i].y + 7;
            newDistanceY = distanceY - y;

            // is item in range?
            if ((u16)(newDistanceX + 7) < 15 && (newDistanceY >= -5) && (newDistanceY < 6))
                SetDistanceOfClosestHiddenItem(taskId, newDistanceX, newDistanceY); // send coordinates of the item relative to the player
        }
    }
    CheckForHiddenItemsInMapConnection(taskId);

    // hidden item detected?
    if (gTasks[taskId].tItemFound == TRUE)
        return TRUE;
    else
        return FALSE;
}

bool8 IsHiddenItemPresentAtCoords(const struct MapEvents *events, s16 x, s16 y)
{
    u8 bgEventCount = events->bgEventCount;
    const struct BgEvent *bgEvent = events->bgEvents;
    int i;

    for (i = 0; i < bgEventCount; i++)
    {
        if (bgEvent[i].kind == BG_EVENT_HIDDEN_ITEM && x == (u16)bgEvent[i].x && y == (u16)bgEvent[i].y) // hidden item and coordinates matches x and y passed?
        {
            if (!FlagGet(bgEvent[i].bgUnion.hiddenItem.hiddenItemId + FLAG_HIDDEN_ITEMS_START))
                return TRUE;
            else
                return FALSE;
        }
    }
    return FALSE;
}

bool8 IsHiddenItemPresentInConnection(const struct MapConnection *connection, int x, int y)
{
    const struct MapHeader *mapHeader;
    u16 localX, localY;
    u32 localOffset;
    s32 localLength;

    mapHeader = GetMapHeaderFromConnection(connection);

    switch (connection->direction)
    {
    // same weird temp variable behavior seen in IsHiddenItemPresentAtCoords
    case 2:
        localOffset = connection->offset + 7;
        localX = x - localOffset;
        localLength = mapHeader->mapLayout->height - 7;
        localY = localLength + y; // additions are reversed for some reason
        break;
    case 1:
        localOffset = connection->offset + 7;
        localX = x - localOffset;
        localLength = gMapHeader.mapLayout->height + 7;
        localY = y - localLength;
        break;
    case 3:
        localLength = mapHeader->mapLayout->width - 7;
        localX = localLength + x; // additions are reversed for some reason
        localOffset = connection->offset + 7;
        localY = y - localOffset;
        break;
    case 4:
        localLength = gMapHeader.mapLayout->width + 7;
        localX = x - localLength;
        localOffset = connection->offset + 7;
        localY = y - localOffset;
        break;
    default:
        return FALSE;
    }
    return IsHiddenItemPresentAtCoords(mapHeader->events, localX, localY);
}

void CheckForHiddenItemsInMapConnection(u8 taskId)
{
    s16 playerX, playerY;
    s16 x, y;
    s16 width = gMapHeader.mapLayout->width + 7;
    s16 height = gMapHeader.mapLayout->height + 7;

    s16 minX = 7;
    s16 minY = 7;

    PlayerGetDestCoords(&playerX, &playerY);

    for (x = playerX - 7; x <= playerX + 7; x++)
    {
        for (y = playerY - 5; y <= playerY + 5; y++)
        {
            if (minX > x
             || x >= width
             || minY > y
             || y >= height)
            {
                const struct MapConnection *conn = GetMapConnectionAtPos(x, y);
                if (conn && IsHiddenItemPresentInConnection(conn, x, y) == TRUE)
                    SetDistanceOfClosestHiddenItem(taskId, x - playerX, y - playerY);
            }
        }
    }
}

void SetDistanceOfClosestHiddenItem(u8 taskId, s16 itemDistanceX, s16 itemDistanceY)
{
    s16 *data = gTasks[taskId].data;
    s16 oldItemAbsX, oldItemAbsY, newItemAbsX, newItemAbsY;

    if (tItemFound == FALSE)
    {
        tItemDistanceX = itemDistanceX;
        tItemDistanceY = itemDistanceY;
        tItemFound = TRUE;
    }
    else
    {
        // Compare the distances of the previously found item and the new item.
        if (tItemDistanceX < 0)
            oldItemAbsX = tItemDistanceX * -1; // item is to the left
        else
            oldItemAbsX = tItemDistanceX; // item is to the right

        if (tItemDistanceY < 0)
            oldItemAbsY = tItemDistanceY * -1; // item is to the north
        else
            oldItemAbsY = tItemDistanceY; // item is to the south

        if (itemDistanceX < 0)
            newItemAbsX = itemDistanceX * -1;
        else
            newItemAbsX = itemDistanceX;

        if (itemDistanceY < 0)
            newItemAbsY = itemDistanceY * -1;
        else
            newItemAbsY = itemDistanceY;

        if (oldItemAbsX + oldItemAbsY > newItemAbsX + newItemAbsY)
        {
            tItemDistanceX = itemDistanceX;
            tItemDistanceY = itemDistanceY;
        }
        else
        {
            if (oldItemAbsX + oldItemAbsY == newItemAbsX + newItemAbsY && (oldItemAbsY > newItemAbsY || (oldItemAbsY == newItemAbsY && tItemDistanceY < itemDistanceY)))
            {
                tItemDistanceX = itemDistanceX;
                tItemDistanceY = itemDistanceY;
            }
        }
    }
}

u8 GetDirectionToHiddenItem(s16 itemDistanceX, s16 itemDistanceY)
{
    s16 absX, absY;

    if (itemDistanceX == 0 && itemDistanceY == 0)
        return DIR_NONE; // player is standing on the item.

    // get absolute X distance.
    if (itemDistanceX < 0)
        absX = itemDistanceX * -1;
    else
        absX = itemDistanceX;

    // get absolute Y distance.
    if (itemDistanceY < 0)
        absY = itemDistanceY * -1;
    else
        absY = itemDistanceY;

    if (absX > absY)
    {
        if (itemDistanceX < 0)
            return DIR_EAST;
        else
            return DIR_NORTH;
    }
    else
    {
        if (absX < absY)
        {
            if (itemDistanceY < 0)
                return DIR_SOUTH;
            else
                return DIR_WEST;
        }
        if (absX == absY)
        {
            if (itemDistanceY < 0)
                return DIR_SOUTH;
            else
                return DIR_WEST;
        }
        return DIR_NONE; // should never get here. return something so it doesnt crash.
    }
}

void PlayerFaceHiddenItem(u8 direction)
{
    ObjectEventClearHeldMovementIfFinished(&gObjectEvents[GetObjectEventIdByLocalIdAndMap(LOCALID_PLAYER, 0, 0)]);
    ObjectEventClearHeldMovement(&gObjectEvents[GetObjectEventIdByLocalIdAndMap(LOCALID_PLAYER, 0, 0)]);
    UnfreezeObjectEvent(&gObjectEvents[GetObjectEventIdByLocalIdAndMap(LOCALID_PLAYER, 0, 0)]);
    PlayerTurnInPlace(direction);
}

void Task_HiddenItemNearby(u8 taskId)
{
    if (ObjectEventCheckHeldMovementStatus(&gObjectEvents[GetObjectEventIdByLocalIdAndMap(LOCALID_PLAYER, 0, 0)]) == TRUE)
        DisplayItemMessageOnField(taskId, gOtherText_ItemfinderResponding, Task_CloseItemfinderMessage, 0);
}

void Task_StandingOnHiddenItem(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    if (ObjectEventCheckHeldMovementStatus(&gObjectEvents[GetObjectEventIdByLocalIdAndMap(LOCALID_PLAYER, 0, 0)]) == TRUE
    || tItemFound == FALSE)
    {
        PlayerFaceHiddenItem(sClockwiseDirections[tFacingDir]);
        tItemFound = TRUE;
        tFacingDir = (tFacingDir + 1) & 3;
        tCounter++;

        if (tCounter == 4)
            DisplayItemMessageOnField(taskId, gOtherText_ItemfinderItemUnderfoot, Task_CloseItemfinderMessage, 0);
    }
}

#undef tItemDistanceX
#undef tItemDistanceY
#undef tItemFound
#undef tCounter
#undef tItemfinderBeeps
#undef tFacingDir

void ItemUseOutOfBattle_PokeblockCase(u8 taskId)
{
    if (sub_80F9344() == TRUE)
    {
        DisplayDadsAdviceCannotUseItemMessage(taskId, gTasks[taskId].tUsingRegisteredKeyItem);
    }
    else if (gTasks[taskId].tUsingRegisteredKeyItem != TRUE)
    {
        sub_810BA7C(0);
        ItemMenu_ConfirmNormalFade(taskId);
    }
    else
    {
        gFieldCallback = (void *)sub_8080E28;
        sub_810BA7C(1);
        ItemMenu_ConfirmComplexFade(taskId);
    }
}

void ItemUseOutOfBattle_CoinCase(u8 taskId)
{
    ConvertIntToDecimalStringN(gStringVar1, GetCoins(), 0, 4);
    StringExpandPlaceholders(gStringVar4, gOtherText_Coins3);

    if (!gTasks[taskId].tUsingRegisteredKeyItem)
    {
        Menu_EraseWindowRect(0, 13, 13, 20);
        DisplayItemMessageOnField(taskId, gStringVar4, CleanUpItemMenuMessage, 1);
    }
    else
    {
        DisplayItemMessageOnField(taskId, gStringVar4, CleanUpOverworldMessage, 0);
    }
}

static void SSTicketWaitForAButtonPress(u8 taskId)
{
    if (JOY_NEW(A_BUTTON))
        CleanUpItemMenuMessage(taskId);
}

static void SSTicketWaitForAButtonPress2(u8 taskId)
{
    if (JOY_NEW(A_BUTTON))
        CleanUpOverworldMessage(taskId);
}

// unused
void ItemUseOutOfBattle_SSTicket(u8 taskId)
{
    if (gTasks[taskId].tUsingRegisteredKeyItem == 0)
    {
        Menu_EraseWindowRect(0, 13, 13, 20);
        DisplayItemMessageOnField(taskId, sSSTidalBetaStrings[ItemId_GetSecondaryId(gSpecialVar_ItemId)], SSTicketWaitForAButtonPress, 1);
    }
    else
    {
        DisplayItemMessageOnField(taskId, sSSTidalBetaStrings[ItemId_GetSecondaryId(gSpecialVar_ItemId)], SSTicketWaitForAButtonPress2, 0);
    }
}

void ItemUseOutOfBattle_Berry(u8 taskId)
{
    if (IsPlayerFacingUnplantedSoil() == TRUE)
    {
        gFieldItemUseCallback = ItemUseOnFieldCB_Berry;
        gFieldCallback = ExecuteItemUseFromBlackPalette;
        gTasks[taskId].tCallbackHigh = (u32)CB2_ReturnToField >> 16;
        gTasks[taskId].tCallbackLow = (u32)CB2_ReturnToField;
        gTasks[taskId].func = HandleItemMenuPaletteFade;
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 0x10, RGB(0, 0, 0));
    }
    else
    {
        ItemId_GetFieldFunc(gSpecialVar_ItemId)(taskId);
    }
}

static void ItemUseOnFieldCB_Berry(u8 taskId)
{
    RemoveBagItem(gSpecialVar_ItemId, 1);
    LockPlayerFieldControls();
    ScriptContext_SetupScript(BerryTree_EventScript_ItemUsePlantBerry);
    DestroyTask(taskId);
}

void ItemUseOutOfBattle_WailmerPail(u8 taskId)
{
    if (TryToWaterBerryTree() == TRUE)
    {
        gFieldItemUseCallback = ItemUseOnFieldCB_WailmerPailBerry;
        SetUpItemUseOnFieldCallback(taskId);
    }
    else
    {
        DisplayDadsAdviceCannotUseItemMessage(taskId, gTasks[taskId].tUsingRegisteredKeyItem);
    }
}

static void ItemUseOnFieldCB_WailmerPailBerry(u8 taskId)
{
    LockPlayerFieldControls();
    ScriptContext_SetupScript(BerryTree_EventScript_ItemUseWailmerPail);
    DestroyTask(taskId);
}

static void SetPokemonItemUseAndFadeOut(u8 taskId)
{
    gPokemonItemUseType = ITEM_USE_SINGLE_MON;
    ItemMenu_ConfirmNormalFade(taskId);
}

void ItemUseOutOfBattle_Medicine(u8 taskId)
{
    gPokemonItemUseCallback = UseMedicine;
    SetPokemonItemUseAndFadeOut(taskId);
}

void ItemUseOutOfBattle_SacredAsh(u8 taskId)
{
    u8 i;

    gLastFieldPokeMenuOpened = 0;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) != SPECIES_NONE && GetMonData(&gPlayerParty[i], MON_DATA_HP) == 0)
        {
            gLastFieldPokeMenuOpened = i;
            break;
        }
    }
    gPokemonItemUseCallback = DoSacredAshItemEffect;
    gPokemonItemUseType = ITEM_USE_ALL_MONS;
    ItemMenu_ConfirmNormalFade(taskId);
}

void ItemUseOutOfBattle_PPRecovery(u8 taskId)
{
    gPokemonItemUseCallback = DoPPRecoveryItemEffect;
    SetPokemonItemUseAndFadeOut(taskId);
}

void ItemUseOutOfBattle_PPUp(u8 taskId)
{
    gPokemonItemUseCallback = DoPPUpItemEffect;
    SetPokemonItemUseAndFadeOut(taskId);
}

void ItemUseOutOfBattle_RareCandy(u8 taskId)
{
    gPokemonItemUseCallback = DoRareCandyItemEffect;
    SetPokemonItemUseAndFadeOut(taskId);
}

void ItemUseOutOfBattle_TMHM(u8 taskId)
{
    Menu_EraseWindowRect(0, 13, 13, 20);

    if (gSpecialVar_ItemId >= ITEM_HM01_CUT)
        DisplayItemMessageOnField(taskId, gOtherText_BootedHM, BootUpSoundTMHM, 1); // HM
    else
        DisplayItemMessageOnField(taskId, gOtherText_BootedTM, BootUpSoundTMHM, 1); // TM
}

static void BootUpSoundTMHM(u8 taskId)
{
    PlaySE(SE_PC_LOGIN);
    gTasks[taskId].func = Task_ShowTMHMContainedMessage;
}

static void Task_ShowTMHMContainedMessage(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        StringCopy(gStringVar1, gMoveNames[ItemIdToBattleMoveId(gSpecialVar_ItemId)]);
        StringExpandPlaceholders(gStringVar4, gOtherText_ContainsMove);
        DisplayItemMessageOnField(taskId, gStringVar4, UseTMHMYesNo, 1);
    }
}

static void UseTMHMYesNo(u8 taskId)
{
    DisplayYesNoMenu(7, 7, 1);
    sub_80A3FA0(gBGTilemapBuffers[1], 8, 8, 5, 4, 1);
    DoYesNoFuncWithChoice(taskId, &sUseTMHMYesNoFuncTable);
}

static void UseTMHM(u8 taskId)
{
    gPokemonItemUseCallback = TeachMonTMMove;
    SetPokemonItemUseAndFadeOut(taskId);
}

static void RemoveUsedItem(void)
{
    RemoveBagItem(gSpecialVar_ItemId, 1);
    sub_80A3E0C();
    CopyItemName(gSpecialVar_ItemId, gStringVar2);
    StringExpandPlaceholders(gStringVar4, gOtherText_UsedItem);
}

void ItemUseOutOfBattle_Repel(u8 taskId)
{
    if (VarGet(VAR_REPEL_STEP_COUNT) == 0)
    {
        VarSet(VAR_REPEL_STEP_COUNT, ItemId_GetHoldEffectParam(gSpecialVar_ItemId));
        RemoveUsedItem();
        DisplayItemMessageOnField(taskId, gStringVar4, CleanUpItemMenuMessage, 1);
    }
    else
    {
        DisplayItemMessageOnField(taskId, gOtherText_RepelLingers, CleanUpItemMenuMessage, 1);
    }
}

static void PrepareFluteUseMessage(void)
{
    sub_80A3E0C();
    CopyItemName(gSpecialVar_ItemId, gStringVar2);
}

static void Task_UsedBlackWhiteFlute(u8 taskId)
{
    if(++gTasks[taskId].tItemUseTimer > 7)
    {
        PlaySE(SE_GLASS_FLUTE);
        DisplayItemMessageOnField(taskId, gStringVar4, CleanUpItemMenuMessage, 1);
    }
}

void ItemUseOutOfBattle_BlackWhiteFlute(u8 taskId)
{
    if (gSpecialVar_ItemId == ITEM_WHITE_FLUTE)
    {
        FlagSet(FLAG_SYS_ENC_UP_ITEM);
        FlagClear(FLAG_SYS_ENC_DOWN_ITEM);
        PrepareFluteUseMessage();
        StringExpandPlaceholders(gStringVar4, gOtherText_UsedFlute);
        gTasks[taskId].func = Task_UsedBlackWhiteFlute;
        gTasks[taskId].tItemUseTimer = 0;
    }
    else if (gSpecialVar_ItemId == ITEM_BLACK_FLUTE)
    {
        FlagSet(FLAG_SYS_ENC_DOWN_ITEM);
        FlagClear(FLAG_SYS_ENC_UP_ITEM);
        PrepareFluteUseMessage();
        StringExpandPlaceholders(gStringVar4, gOtherText_UsedRepel);
        gTasks[taskId].func = Task_UsedBlackWhiteFlute;
        gTasks[taskId].tItemUseTimer = 0;
    }
}

void Task_UseDigEscapeRopeOnField(u8 taskId)
{
    ResetInitialPlayerAvatarState();
    StartEscapeRopeFieldEffect();
    DestroyTask(taskId);
}

static void ItemUseOnFieldCB_EscapeRope(u8 taskId)
{
    Overworld_ResetStateAfterDigEscRope();
    RemoveUsedItem();
    gTasks[taskId].data[0] = 0;
    DisplayItemMessageOnField(taskId, gStringVar4, Task_UseDigEscapeRopeOnField, 0);
}

bool8 CanUseDigOrEscapeRopeOnCurMap(void)
{
    if (gMapHeader.mapType == MAP_TYPE_UNDERGROUND)
        return TRUE;
    else
        return FALSE;
}

void ItemUseOutOfBattle_EscapeRope(u8 taskId)
{
    if (CanUseDigOrEscapeRopeOnCurMap() == TRUE)
    {
        gFieldItemUseCallback = ItemUseOnFieldCB_EscapeRope;
        SetUpItemUseOnFieldCallback(taskId);
    }
    else
    {
        DisplayDadsAdviceCannotUseItemMessage(taskId, gTasks[taskId].tUsingRegisteredKeyItem);
    }
}

void ItemUseOutOfBattle_EvolutionStone(u8 taskId)
{
    gPokemonItemUseCallback = DoEvolutionStoneItemEffect;
    SetPokemonItemUseAndFadeOut(taskId);
}

void ItemUseInBattle_PokeBall(u8 taskId)
{
    if (PlayerPartyAndPokemonStorageFull() == FALSE) // have room for mon?
    {
        RemoveBagItem(gSpecialVar_ItemId, 1);
        sub_80A7094(taskId);
    }
    else
    {
        Menu_EraseWindowRect(0, 13, 13, 20);
        DisplayItemMessageOnField(taskId, gOtherText_BoxIsFull, CleanUpItemMenuMessage, 1);
    }
}

void Task_CloseStatIncreaseMessage(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
        sub_80A7094(taskId);
}

void Task_UseStatIncreaseItem(u8 taskId)
{
    if(++gTasks[taskId].tItemUseTimer > 7)
    {
        PlaySE(SE_USE_ITEM);
        RemoveBagItem(gSpecialVar_ItemId, 1);
        DisplayItemMessageOnField(taskId, sub_803F378(gSpecialVar_ItemId), Task_CloseStatIncreaseMessage, 1);
    }
}

void ItemUseInBattle_StatIncrease(u8 taskId)
{
    u16 partyId = gBattlerPartyIndexes[gBattlerInMenuId];

    Menu_EraseWindowRect(0, 13, 13, 20);

    if (ExecuteTableBasedItemEffect_(&gPlayerParty[partyId], gSpecialVar_ItemId, partyId, 0) != FALSE)
    {
        DisplayItemMessageOnField(taskId, gOtherText_WontHaveAnyEffect, CleanUpItemMenuMessage, 1);
    }
    else
    {
        gTasks[taskId].func = Task_UseStatIncreaseItem;
        gTasks[taskId].tItemUseTimer = 0;
    }
}

void Task_CloseBagForBattleItem(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        sub_8094E4C();
        FreeAndReserveObjectSpritePalettes();
        DestroyTask(taskId);
    }
}

void ItemUseInBattle_ShowPartyMenu(u8 taskId)
{
    gTasks[taskId].func = Task_CloseBagForBattleItem;
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
}

void ItemUseInBattle_Medicine(u8 var)
{
    gPokemonItemUseCallback = UseMedicine;
    ItemUseInBattle_ShowPartyMenu(var);
}

void ItemUseInBattle_SacredAsh(u8 var)
{
    gPokemonItemUseCallback = DoSacredAshItemEffect;
    ItemUseInBattle_ShowPartyMenu(var);
}

void ItemUseInBattle_PPRecovery(u8 var)
{
    gPokemonItemUseCallback = DoPPRecoveryItemEffect;
    ItemUseInBattle_ShowPartyMenu(var);
}

void ItemUseInBattle_UnusedConfusionCure(u8 var)
{
    Menu_EraseWindowRect(0, 13, 13, 20);

    if (ExecuteTableBasedItemEffect__(0, gSpecialVar_ItemId, 0) == FALSE)
    {
        RemoveBagItem(gSpecialVar_ItemId, 1);
        GetMonNickname(&gPlayerParty[0], gStringVar1);
        StringExpandPlaceholders(gStringVar4, gOtherText_SnapConfusion);
        DisplayItemMessageOnField(var, gStringVar4, sub_80A7094, 1);
    }
    else
    {
        DisplayItemMessageOnField(var, gOtherText_WontHaveAnyEffect, CleanUpItemMenuMessage, 1);
    }
}

void ItemUseInBattle_Escape(u8 taskId)
{
    Menu_EraseWindowRect(0, 13, 13, 20);

    if ((gBattleTypeFlags & BATTLE_TYPE_TRAINER) == FALSE)
    {
        RemoveUsedItem();
        DisplayItemMessageOnField(taskId, gStringVar4, sub_80A7094, 1);
    }
    else
    {
        DisplayDadsAdviceCannotUseItemMessage(taskId, gTasks[taskId].tUsingRegisteredKeyItem);
    }
}

void ItemUseOutOfBattle_EnigmaBerry(u8 taskId)
{
    switch (GetItemEffectType(gSpecialVar_ItemId))
    {
    case ITEM_EFFECT_HEAL_HP:
    case ITEM_EFFECT_CURE_POISON:
    case ITEM_EFFECT_CURE_SLEEP:
    case ITEM_EFFECT_CURE_BURN:
    case ITEM_EFFECT_CURE_FREEZE:
    case ITEM_EFFECT_CURE_PARALYSIS:
    case ITEM_EFFECT_CURE_ALL_STATUS:
    case ITEM_EFFECT_ATK_EV:
    case ITEM_EFFECT_HP_EV:
    case ITEM_EFFECT_SPATK_EV:
    case ITEM_EFFECT_SPDEF_EV:
    case ITEM_EFFECT_SPEED_EV:
    case ITEM_EFFECT_DEF_EV:
        gTasks[taskId].tEnigmaBerryType = ITEM_USE_PARTY_MENU;
        ItemUseOutOfBattle_Medicine(taskId);
        break;
    case ITEM_EFFECT_SACRED_ASH:
        gTasks[taskId].tEnigmaBerryType = ITEM_USE_PARTY_MENU;
        ItemUseOutOfBattle_SacredAsh(taskId);
        break;
    case ITEM_EFFECT_RAISE_LEVEL:
        gTasks[taskId].tEnigmaBerryType = ITEM_USE_PARTY_MENU;
        ItemUseOutOfBattle_RareCandy(taskId);
        break;
    case ITEM_EFFECT_PP_UP:
    case ITEM_EFFECT_PP_MAX:
        gTasks[taskId].tEnigmaBerryType = ITEM_USE_PARTY_MENU;
        ItemUseOutOfBattle_PPUp(taskId);
        break;
    case ITEM_EFFECT_HEAL_PP:
        gTasks[taskId].tEnigmaBerryType = ITEM_USE_PARTY_MENU;
        ItemUseOutOfBattle_PPRecovery(taskId);
        break;
    default:
        gTasks[taskId].tEnigmaBerryType = ITEM_USE_BAG_MENU;
        ItemUseOutOfBattle_CannotUse(taskId);
    }
}

void ItemUseInBattle_EnigmaBerry(u8 taskId)
{
    switch (GetItemEffectType(gSpecialVar_ItemId))
    {
    case ITEM_EFFECT_X_ITEM:
        ItemUseInBattle_StatIncrease(taskId);
        break;
    case ITEM_EFFECT_HEAL_HP:
    case ITEM_EFFECT_CURE_POISON:
    case ITEM_EFFECT_CURE_SLEEP:
    case ITEM_EFFECT_CURE_BURN:
    case ITEM_EFFECT_CURE_FREEZE:
    case ITEM_EFFECT_CURE_PARALYSIS:
    case ITEM_EFFECT_CURE_CONFUSION:
    case ITEM_EFFECT_CURE_INFATUATION:
    case ITEM_EFFECT_CURE_ALL_STATUS:
        ItemUseInBattle_Medicine(taskId);
        break;
    case ITEM_EFFECT_HEAL_PP:
        ItemUseInBattle_PPRecovery(taskId);
        break;
    default:
        ItemUseOutOfBattle_CannotUse(taskId);
    }
}

void ItemUseOutOfBattle_CannotUse(u8 taskId)
{
    DisplayDadsAdviceCannotUseItemMessage(taskId, gTasks[taskId].tUsingRegisteredKeyItem);
}

#undef tUsingRegisteredKeyItem
#undef tCallbackHigh
#undef tCallbackLow
#undef tEnigmaBerryType
#undef tItemUseTimer
