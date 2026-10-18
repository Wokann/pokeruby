#include "global.h"
#include "constants/rgb.h"
#include "item_use.h"
#include "battle.h"
#include "berry.h"
#include "bike.h"
#include "coins.h"
#include "data.h"
#include "event_data.h"
#include "event_scripts.h"
#include "field_effect.h"
#include "field_screen_effect.h"
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
#include "pokemon.h"
#include "pokemon_menu.h"
#include "overworld.h"
#include "script.h"
#include "constants/songs.h"
#include "sound.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "constants/event_bg.h"
#include "constants/event_objects.h"
#include "constants/map_types.h"
#include "constants/species.h"
#include "constants/vars.h"
extern void (*gFieldItemUseCallback)(u8);
extern void (*gPokemonItemUseCallback)(u8, u16, TaskFunc);

extern u8 gPokemonItemUseType;
extern u8 gLastFieldPokeMenuOpened;



static void SetUpItemUseCallback(u8);
static void SetUpItemUseOnFieldCallback(u8);
static void DisplayDadsAdviceCannotUseItemMessage(u8, bool8);
static void DisplayCannotDismountBikeMessage(u8, bool8);
static void ItemUseOnFieldCB_Bike(u8);
static bool32 CanFish(void);
static void ItemUseOnFieldCB_Rod(u8);
static void ItemUseOnFieldCB_Itemfinder(u8);
static void Task_UseItemfinder(u8);
static void Task_CloseItemfinderMessage(u8);
static bool8 ItemfinderCheckForHiddenItems(const struct MapEvents *, u8);
static bool8 IsHiddenItemPresentAtCoords(const struct MapEvents *, s16, s16);
static bool8 IsHiddenItemPresentInConnection(const struct MapConnection *, int, int);
static void CheckForHiddenItemsInMapConnection(u8);
static void SetDistanceOfClosestHiddenItem(u8, s16, s16);
static u8 GetDirectionToHiddenItem(s16, s16);
static void PlayerFaceHiddenItem(u8);
static void Task_HiddenItemNearby(u8);
static void Task_StandingOnHiddenItem(u8);
static void Task_CloseStatIncreaseMessage(u8);
static void Task_UseStatIncreaseItem(u8);
static void ItemUseInBattle_ShowPartyMenu(u8);
static void ItemUseOnFieldCB_Berry(u8);
static void ItemUseOnFieldCB_WailmerPailBerry(u8);
static void UseTMHM(u8);
static void UseTMHMYesNo(u8);
static void BootUpSoundTMHM(u8);
static void Task_ShowTMHMContainedMessage(u8);

static const u8 gSSTidalBetaString[] = _("この　チケットで　ふねに　のりほうだい\nはやく　のってみたいな");
static const u8 gSSTidalBetaString2[] = _("この　チケットで　ふねに　のりほうだい\nはやく　のってみたいな");

static const u8 *const sSSTidalBetaStrings[] =
{
    gSSTidalBetaString,
    gSSTidalBetaString2,
};

static const MainCallback sItemUseCallbacks[] =
{
    CB2_ShowPartyMenuForItemUse,
    CB2_ReturnToField,
    sub_810B96C,
};

static const u8 sClockwiseDirections[] = { DIR_NORTH, DIR_EAST, DIR_SOUTH, DIR_WEST };

static const struct YesNoFuncTable sUseTMHMYesNoFuncTable =
{
    .yesFunc = UseTMHM,
    .noFunc = CloseItemMessage,
};

static void SetUpItemUseCallback(u8 taskId)
{
    u8 type;

    if (gSpecialVar_ItemId == ITEM_ENIGMA_BERRY)
        type = gTasks[taskId].data[15] - 1;
    else
        type = GetItemType(gSpecialVar_ItemId) - 1;

    gTasks[taskId].data[8] = (u32)sItemUseCallbacks[type] >> 16;
    gTasks[taskId].data[9] = (u32)sItemUseCallbacks[type];
    gTasks[taskId].func = Task_CloseBagMenu;
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

static void SetUpItemUseOnFieldCallback(u8 taskId)
{
    if (gTasks[taskId].data[2] != 1)
    {
        gFieldCallback = ExecuteItemUseFromBlackPalette;
        ItemMenu_ConfirmNormalFade(taskId);
    }
    else
    {
        gFieldItemUseCallback(taskId);
    }
}

static void DisplayCannotUseItemMessage(u8 taskId, bool8 isUsingRegisteredKeyItemOnField, const u8 *str)
{
    StringExpandPlaceholders(gStringVar4, str);

    switch (isUsingRegisteredKeyItemOnField)
    {
    case 0: // Item Menu
        Menu_EraseWindowRect(0, 13, 13, 20);
        DisplayItemMessageOnField(taskId, gStringVar4, CloseItemMessage, 1);
        break;
    default: // Field
        DisplayItemMessageOnField(taskId, gStringVar4, CleanUpOverworldMessage, 0);
        break;
    }
}

static void DisplayDadsAdviceCannotUseItemMessage(u8 taskId, bool8 isUsingRegisteredKeyItemOnField)
{
    DisplayCannotUseItemMessage(taskId, isUsingRegisteredKeyItemOnField, gText_DadsAdvice);
}

static void DisplayCannotDismountBikeMessage(u8 taskId, bool8 isUsingRegisteredKeyItemOnField)
{
    DisplayCannotUseItemMessage(taskId, isUsingRegisteredKeyItemOnField, gText_CantDismountBike);
}

u8 CheckIfItemIsTMHMOrEvolutionStone(u16 itemId)
{
    if (GetItemFieldFunc(itemId) == ItemUseOutOfBattle_TMHM)
        return 1;
    else if (GetItemFieldFunc(itemId) == ItemUseOutOfBattle_EvolutionStone)
        return 2;
    else
        return 0;
}

void ItemMenu_ReadMail(u8 taskId)
{
    struct Mail mailStruct;

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
    gTasks[taskId].func = ItemMenu_ReadMail;
}

void ItemUseOutOfBattle_Bike(u8 taskId)
{
    s16 coordsX, coordsY;
    u8 tileBehavior;

    PlayerGetDestCoords(&coordsX, &coordsY);
    tileBehavior = MapGridGetMetatileBehaviorAt(coordsX, coordsY);

    if (FlagGet(FLAG_SYS_CYCLING_ROAD) == TRUE
        || MetatileBehavior_IsVerticalRail(tileBehavior) == TRUE
        || MetatileBehavior_IsHorizontalRail(tileBehavior) == TRUE
        || MetatileBehavior_IsIsolatedVerticalRail(tileBehavior) == TRUE
        || MetatileBehavior_IsIsolatedHorizontalRail(tileBehavior) == TRUE)
    {
        DisplayCannotDismountBikeMessage(taskId, gTasks[taskId].data[2]);
    }
    else
    {
        if (Overworld_IsBikingAllowed() == TRUE && IsBikingDisallowedByPlayer() == FALSE)
        {
            gFieldItemUseCallback = (void *)ItemUseOnFieldCB_Bike;
            SetUpItemUseOnFieldCallback(taskId);
        }
        else
            DisplayDadsAdviceCannotUseItemMessage(taskId, gTasks[taskId].data[2]);
    }
}

static void ItemUseOnFieldCB_Bike(u8 taskId)
{
    if (GetItemSecondaryId(gSpecialVar_ItemId) == 0)
        GetOnOffBike(PLAYER_AVATAR_FLAG_MACH_BIKE);
    if (GetItemSecondaryId(gSpecialVar_ItemId) == 1)
        GetOnOffBike(PLAYER_AVATAR_FLAG_ACRO_BIKE);

    ScriptUnfreezeObjectEvents();
    UnlockPlayerFieldControls();
    DestroyTask(taskId);
}

static bool32 CanFish(void)
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
        if (MetatileBehavior_IsBridgeOverWaterNoEdge(tileBehavior) == TRUE)
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
        DisplayDadsAdviceCannotUseItemMessage(taskId, gTasks[taskId].data[2]);
}

static void ItemUseOnFieldCB_Rod(u8 taskId)
{
    StartFishing(GetItemSecondaryId(gSpecialVar_ItemId));
    DestroyTask(taskId);
}

void ItemUseOutOfBattle_Itemfinder(u8 taskId)
{
    IncrementGameStat(GAME_STAT_USED_ITEMFINDER);
    gFieldItemUseCallback = (void *)ItemUseOnFieldCB_Itemfinder;
    SetUpItemUseOnFieldCallback(taskId);
}

static void ItemUseOnFieldCB_Itemfinder(u8 taskId)
{
    if (ItemfinderCheckForHiddenItems(gMapHeader.events, taskId) == TRUE)
        gTasks[taskId].func = Task_UseItemfinder;
    else
        DisplayItemMessageOnField(taskId, gText_ItemFinderNothing, Task_CloseItemfinderMessage, 0);
}

static void Task_UseItemfinder(u8 taskId)
{
    u8 playerDir;
    u8 playerDirToItem;
    u8 i;
    s16 *data = gTasks[taskId].data;

    if (!data[3])
    {
        if (data[4] == 4)
        {
            playerDirToItem = GetDirectionToHiddenItem(data[0], data[1]);
            if (playerDirToItem)
            {
                PlayerFaceHiddenItem(sClockwiseDirections[playerDirToItem - 1]);
                gTasks[taskId].func = Task_HiddenItemNearby;
            }
            else // player is above hidden item.
            {
                playerDir = GetPlayerFacingDirection();

                // rotate player clockwise depending on current direction.
                for (i = 0; i < 4; i++)
                    if (playerDir == sClockwiseDirections[i])
                        data[5] = (i + 1) & 3;

                gTasks[taskId].func = Task_StandingOnHiddenItem;
                data[3] = 0;
                data[2] = 0;
            }
            return;
        }
        PlaySE(SE_ITEMFINDER); // play the itemfinder jingle 4 times before executing the itemfinder.
        data[4]++;
    }
    data[3] = (data[3] + 1) & 0x1F;
}

static void Task_CloseItemfinderMessage(u8 taskId)
{
    Menu_EraseWindowRect(0, 14, 29, 19);
    ScriptUnfreezeObjectEvents();
    UnlockPlayerFieldControls();
    DestroyTask(taskId);
}

static bool8 ItemfinderCheckForHiddenItems(const struct MapEvents *events, u8 taskId)
{
    int distanceX, distanceY;
    u16 playerX, playerY;
    s16 newDistanceX, newDistanceY, i;

    PlayerGetDestCoords(&playerX, &playerY);
    gTasks[taskId].data[2] = FALSE;

    for (i = 0; i < events->bgEventCount; i++)
    {
        if ((events->bgEvents[i].kind == BG_EVENT_HIDDEN_ITEM) && !FlagGet(events->bgEvents[i].bgUnion.hiddenItem.hiddenItemId + FLAG_HIDDEN_ITEMS_START))
        {
            // do a distance lookup of each item so long as the index remains less than the objects on the current map.
            distanceX = (u16)events->bgEvents[i].x + 7;
            newDistanceX = distanceX - playerX;
            distanceY = (u16)events->bgEvents[i].y + 7;
            newDistanceY = distanceY - playerY;

            // is item in range?
            if ((u16)(newDistanceX + 7) < 15 && (newDistanceY >= -5) && (newDistanceY < 6))
                SetDistanceOfClosestHiddenItem(taskId, newDistanceX, newDistanceY); // send coordinates of the item relative to the player
        }
    }
    CheckForHiddenItemsInMapConnection(taskId);

    // hidden item detected?
    if (gTasks[taskId].data[2] == TRUE)
        return TRUE;
    else
        return FALSE;
}

static bool8 IsHiddenItemPresentAtCoords(const struct MapEvents *events, s16 x, s16 y)
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

static bool8 IsHiddenItemPresentInConnection(const struct MapConnection *connection, int x, int y)
{
    const struct MapHeader *connectionHeader;
    u16 connectionX, connectionY;
    u32 localOffset;
    s32 localLength;

    connectionHeader = GetMapHeaderFromConnection(connection);

    switch (connection->direction)
    {
    // same weird temp variable behavior seen in IsHiddenItemPresentAtCoords
    case 2:
        localOffset = connection->offset + 7;
        connectionX = x - localOffset;
        localLength = connectionHeader->mapLayout->height - 7;
        connectionY = localLength + y; // additions are reversed for some reason
        break;
    case 1:
        localOffset = connection->offset + 7;
        connectionX = x - localOffset;
        localLength = gMapHeader.mapLayout->height + 7;
        connectionY = y - localLength;
        break;
    case 3:
        localLength = connectionHeader->mapLayout->width - 7;
        connectionX = localLength + x; // additions are reversed for some reason
        localOffset = connection->offset + 7;
        connectionY = y - localOffset;
        break;
    case 4:
        localLength = gMapHeader.mapLayout->width + 7;
        connectionX = x - localLength;
        localOffset = connection->offset + 7;
        connectionY = y - localOffset;
        break;
    default:
        return FALSE;
    }
    return IsHiddenItemPresentAtCoords(connectionHeader->events, connectionX, connectionY);
}

static void CheckForHiddenItemsInMapConnection(u8 taskId)
{
    s16 playerX, playerY;
    s16 x, y;
    s16 width = gMapHeader.mapLayout->width + 7;
    s16 height = gMapHeader.mapLayout->height + 7;

    s16 var1 = 7;
    s16 var2 = 7;

    PlayerGetDestCoords(&playerX, &playerY);

    for (x = playerX - 7; x <= playerX + 7; x++)
    {
        for (y = playerY - 5; y <= playerY + 5; y++)
        {
            if (var1 > x
             || x >= width
             || var2 > y
             || y >= height)
            {
                const struct MapConnection *conn = GetMapConnectionAtPos(x, y);
                if (conn && IsHiddenItemPresentInConnection(conn, x, y) == TRUE)
                    SetDistanceOfClosestHiddenItem(taskId, x - playerX, y - playerY);
            }
        }
    }
}

static void SetDistanceOfClosestHiddenItem(u8 taskId, s16 itemDistanceX, s16 itemDistanceY)
{
    s16 *data = gTasks[taskId].data;
    s16 oldItemAbsX, oldItemAbsY, newItemAbsX, newItemAbsY;

    if (data[2] == FALSE)
    {
        data[0] = itemDistanceX;
        data[1] = itemDistanceY;
        data[2] = TRUE;
    }
    else
    {
        // Other items have been found, check if this one is closer
        if (data[0] < 0)
            oldItemAbsX = data[0] * -1; // item is to the left
        else
            oldItemAbsX = data[0]; // item is to the right

        if (data[1] < 0)
            oldItemAbsY = data[1] * -1; // item is to the north
        else
            oldItemAbsY = data[1]; // item is to the south

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
            data[0] = itemDistanceX;
            data[1] = itemDistanceY;
        }
        else
        {
            if (oldItemAbsX + oldItemAbsY == newItemAbsX + newItemAbsY && (oldItemAbsY > newItemAbsY || (oldItemAbsY == newItemAbsY && data[1] < itemDistanceY)))
            {
                data[0] = itemDistanceX;
                data[1] = itemDistanceY;
            }
        }
    }
}

static u8 GetDirectionToHiddenItem(s16 itemDistanceX, s16 itemDistanceY)
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

static void PlayerFaceHiddenItem(u8 direction)
{
    ObjectEventClearHeldMovementIfFinished(&gObjectEvents[GetObjectEventIdByLocalIdAndMap(LOCALID_PLAYER, 0, 0)]);
    ObjectEventClearHeldMovement(&gObjectEvents[GetObjectEventIdByLocalIdAndMap(LOCALID_PLAYER, 0, 0)]);
    UnfreezeObjectEvent(&gObjectEvents[GetObjectEventIdByLocalIdAndMap(LOCALID_PLAYER, 0, 0)]);
    PlayerTurnInPlace(direction);
}

static void Task_HiddenItemNearby(u8 taskId)
{
    if (ObjectEventCheckHeldMovementStatus(&gObjectEvents[GetObjectEventIdByLocalIdAndMap(LOCALID_PLAYER, 0, 0)]) == TRUE)
        DisplayItemMessageOnField(taskId, gText_ItemFinderNearby, Task_CloseItemfinderMessage, 0);
}

static void Task_StandingOnHiddenItem(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    if (ObjectEventCheckHeldMovementStatus(&gObjectEvents[GetObjectEventIdByLocalIdAndMap(LOCALID_PLAYER, 0, 0)]) == TRUE
    || data[2] == FALSE)
    {
        PlayerFaceHiddenItem(sClockwiseDirections[data[5]]);
        data[2] = 1;
        data[5] = (data[5] + 1) & 3;
        data[3]++;

        if (data[3] == 4)
            DisplayItemMessageOnField(taskId, gText_ItemFinderOnTop, Task_CloseItemfinderMessage, 0);
    }
}

void ItemUseOutOfBattle_PokeblockCase(u8 taskId)
{
    if (MenuHelpers_IsLinkActive() == TRUE)
    {
        DisplayDadsAdviceCannotUseItemMessage(taskId, gTasks[taskId].data[2]);
    }
    else if (gTasks[taskId].data[2] != TRUE)
    {
        sub_810BA7C(0);
        ItemMenu_ConfirmNormalFade(taskId);
    }
    else
    {
        gFieldCallback = (void *)FieldCB_ReturnToFieldNoScript;
        sub_810BA7C(1);
        ItemMenu_ConfirmComplexFade(taskId);
    }
}

void ItemUseOutOfBattle_CoinCase(u8 taskId)
{
    ConvertIntToDecimalStringN(gStringVar1, GetCoins(), 0, 4);
    StringExpandPlaceholders(gStringVar4, gText_CoinCase);

    if (!gTasks[taskId].data[2])
    {
        Menu_EraseWindowRect(0, 13, 13, 20);
        DisplayItemMessageOnField(taskId, gStringVar4, CloseItemMessage, 1);
    }
    else
    {
        DisplayItemMessageOnField(taskId, gStringVar4, CleanUpOverworldMessage, 0);
    }
}

static void SSTicketWaitForAButtonPress(u8 taskId)
{
    if (JOY_NEW(A_BUTTON))
        CloseItemMessage(taskId);
}

static void SSTicketWaitForAButtonPress2(u8 taskId)
{
    if (JOY_NEW(A_BUTTON))
        CleanUpOverworldMessage(taskId);
}

// unused
void ItemUseOutOfBattle_SSTicket(u8 taskId)
{
    if (gTasks[taskId].data[2] == 0)
    {
        Menu_EraseWindowRect(0, 13, 13, 20);
        DisplayItemMessageOnField(taskId, sSSTidalBetaStrings[GetItemSecondaryId(gSpecialVar_ItemId)], SSTicketWaitForAButtonPress, 1);
    }
    else
    {
        DisplayItemMessageOnField(taskId, sSSTidalBetaStrings[GetItemSecondaryId(gSpecialVar_ItemId)], SSTicketWaitForAButtonPress2, 0);
    }
}

void ItemUseOutOfBattle_Berry(u8 taskId)
{
    if (IsPlayerFacingEmptyBerryTreePatch() == TRUE)
    {
        gFieldItemUseCallback = ItemUseOnFieldCB_Berry;
        gFieldCallback = ExecuteItemUseFromBlackPalette;
        gTasks[taskId].data[8] = (u32)CB2_ReturnToField >> 16;
        gTasks[taskId].data[9] = (u32)CB2_ReturnToField;
        gTasks[taskId].func = Task_CloseBagMenu;
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 0x10, RGB(0, 0, 0));
    }
    else
    {
        GetItemFieldFunc(gSpecialVar_ItemId)(taskId);
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
        DisplayDadsAdviceCannotUseItemMessage(taskId, gTasks[taskId].data[2]);
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

    if (gSpecialVar_ItemId >= ITEM_HM_CUT)
        DisplayItemMessageOnField(taskId, gText_BootedUpHM, BootUpSoundTMHM, 1); // HM
    else
        DisplayItemMessageOnField(taskId, gText_BootedUpTM, BootUpSoundTMHM, 1); // TM
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
        StringExpandPlaceholders(gStringVar4, gText_TMHMContainedVar1);
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
    StringExpandPlaceholders(gStringVar4, gText_PlayerUsedVar2);
}

void ItemUseOutOfBattle_Repel(u8 taskId)
{
    if (VarGet(VAR_REPEL_STEP_COUNT) == 0)
    {
        VarSet(VAR_REPEL_STEP_COUNT, GetItemHoldEffectParam(gSpecialVar_ItemId));
        RemoveUsedItem();
        DisplayItemMessageOnField(taskId, gStringVar4, CloseItemMessage, 1);
    }
    else
    {
        DisplayItemMessageOnField(taskId, gText_RepelEffectsLingered, CloseItemMessage, 1);
    }
}

static void sub_80CA07C(void)
{
    sub_80A3E0C();
    CopyItemName(gSpecialVar_ItemId, gStringVar2);
}

static void Task_UsedBlackWhiteFlute(u8 taskId)
{
    if(++gTasks[taskId].data[15] > 7)
    {
        PlaySE(SE_GLASS_FLUTE);
        DisplayItemMessageOnField(taskId, gStringVar4, CloseItemMessage, 1);
    }
}

void ItemUseOutOfBattle_BlackWhiteFlute(u8 taskId)
{
    if (gSpecialVar_ItemId == ITEM_WHITE_FLUTE)
    {
        FlagSet(FLAG_SYS_ENC_UP_ITEM);
        FlagClear(FLAG_SYS_ENC_DOWN_ITEM);
        sub_80CA07C();
        StringExpandPlaceholders(gStringVar4, gText_UsedVar2WildLured);
        gTasks[taskId].func = Task_UsedBlackWhiteFlute;
        gTasks[taskId].data[15] = 0;
    }
    else if (gSpecialVar_ItemId == ITEM_BLACK_FLUTE)
    {
        FlagSet(FLAG_SYS_ENC_DOWN_ITEM);
        FlagClear(FLAG_SYS_ENC_UP_ITEM);
        sub_80CA07C();
        StringExpandPlaceholders(gStringVar4, gText_UsedVar2WildRepelled);
        gTasks[taskId].func = Task_UsedBlackWhiteFlute;
        gTasks[taskId].data[15] = 0;
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
        DisplayDadsAdviceCannotUseItemMessage(taskId, gTasks[taskId].data[2]);
    }
}

void ItemUseOutOfBattle_EvolutionStone(u8 taskId)
{
    gPokemonItemUseCallback = DoEvolutionStoneItemEffect;
    SetPokemonItemUseAndFadeOut(taskId);
}

void ItemUseInBattle_PokeBall(u8 taskId)
{
    if (IsPlayerPartyAndPokemonStorageFull() == FALSE) // have room for mon?
    {
        RemoveBagItem(gSpecialVar_ItemId, 1);
        sub_80A7094(taskId);
    }
    else
    {
        Menu_EraseWindowRect(0, 13, 13, 20);
        DisplayItemMessageOnField(taskId, gText_BoxFull, CloseItemMessage, 1);
    }
}

static void Task_CloseStatIncreaseMessage(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
        sub_80A7094(taskId);
}

static void Task_UseStatIncreaseItem(u8 taskId)
{
    if(++gTasks[taskId].data[15] > 7)
    {
        PlaySE(SE_USE_ITEM);
        RemoveBagItem(gSpecialVar_ItemId, 1);
        DisplayItemMessageOnField(taskId, UseStatIncreaseItem(gSpecialVar_ItemId), Task_CloseStatIncreaseMessage, 1);
    }
}

void ItemUseInBattle_StatIncrease(u8 taskId)
{
    u16 partyId = gBattlerPartyIndexes[gBattlerInMenuId];

    Menu_EraseWindowRect(0, 13, 13, 20);

    if (ExecuteTableBasedItemEffect(&gPlayerParty[partyId], gSpecialVar_ItemId, partyId, 0) != FALSE)
    {
        DisplayItemMessageOnField(taskId, gText_WontHaveEffect, CloseItemMessage, 1);
    }
    else
    {
        gTasks[taskId].func = Task_UseStatIncreaseItem;
        gTasks[taskId].data[15] = 0;
    }
}

void sub_80CA394(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        ChooseMonForInBattleItem();
        FreeAndReserveObjectSpritePalettes();
        DestroyTask(taskId);
    }
}

static void ItemUseInBattle_ShowPartyMenu(u8 taskId)
{
    gTasks[taskId].func = sub_80CA394;
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
}

void ItemUseInBattle_Medicine(u8 taskId)
{
    gPokemonItemUseCallback = UseMedicine;
    ItemUseInBattle_ShowPartyMenu(taskId);
}

void ItemUseInBattle_SacredAsh(u8 taskId)
{
    gPokemonItemUseCallback = DoSacredAshItemEffect;
    ItemUseInBattle_ShowPartyMenu(taskId);
}

void ItemUseInBattle_PPRecovery(u8 taskId)
{
    gPokemonItemUseCallback = DoPPRecoveryItemEffect;
    ItemUseInBattle_ShowPartyMenu(taskId);
}

void unref_sub_80CA448(u8 var)
{
    Menu_EraseWindowRect(0, 13, 13, 20);

    if (ExecuteTableBasedItemEffect_(0, gSpecialVar_ItemId, 0) == FALSE)
    {
        RemoveBagItem(gSpecialVar_ItemId, 1);
        GetMonNickname(&gPlayerParty[0], gStringVar1);
        StringExpandPlaceholders(gStringVar4, gText_PkmnSnappedOutOfConfusion);
        DisplayItemMessageOnField(var, gStringVar4, sub_80A7094, 1);
    }
    else
    {
        DisplayItemMessageOnField(var, gText_WontHaveEffect, CloseItemMessage, 1);
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
        DisplayDadsAdviceCannotUseItemMessage(taskId, gTasks[taskId].data[2]);
    }
}

void ItemUseOutOfBattle_EnigmaBerry(u8 taskId)
{
    switch (GetItemEffectType(gSpecialVar_ItemId) - 1)
    {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
        gTasks[taskId].data[15] = 1;
        ItemUseOutOfBattle_Medicine(taskId);
        break;
    case 9:
        gTasks[taskId].data[15] = 1;
        ItemUseOutOfBattle_SacredAsh(taskId);
        break;
    case 0:
        gTasks[taskId].data[15] = 1;
        ItemUseOutOfBattle_RareCandy(taskId);
        break;
    case 18:
    case 19:
        gTasks[taskId].data[15] = 1;
        ItemUseOutOfBattle_PPUp(taskId);
        break;
    case 20:
        gTasks[taskId].data[15] = 1;
        ItemUseOutOfBattle_PPRecovery(taskId);
        break;
    default:
        gTasks[taskId].data[15] = 4;
        ItemUseOutOfBattle_CannotUse(taskId);
    }
}

void ItemUseInBattle_EnigmaBerry(u8 taskId)
{
    switch (GetItemEffectType(gSpecialVar_ItemId))
    {
    case 0:
        ItemUseInBattle_StatIncrease(taskId);
        break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 11:
        ItemUseInBattle_Medicine(taskId);
        break;
    case 21:
        ItemUseInBattle_PPRecovery(taskId);
        break;
    default:
        ItemUseOutOfBattle_CannotUse(taskId);
    }
}

void ItemUseOutOfBattle_CannotUse(u8 taskId)
{
    DisplayDadsAdviceCannotUseItemMessage(taskId, gTasks[taskId].data[2]);
}
