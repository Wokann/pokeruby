#include "global.h"
#include "reshow_battle_screen.h"
#include "battle_party_menu.h"
#include "battle.h"
#include "item_menu.h"
#include "item_use.h"
#include "main.h"
#include "menu.h"
#include "menu_helpers.h"
#include "palette.h"
#include "party_menu.h"
#include "pokemon.h"
#include "pokemon_summary_screen.h"
#include "battle_anim.h"
#include "constants/item_effects.h"
#include "constants/songs.h"
#include "sound.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "text.h"
#include "ewram.h"

EWRAM_DATA u8 gBattlePartyCurrentOrder[PARTY_SIZE / 2] = {};
EWRAM_DATA u8 gBattlePartyMenuAction = 0;

extern u8 CreatePartyMenuCursor(u8, u8);
extern void PartyMenuPrintMonsLevelOrStatus(void);
extern void PartyMenuNoOpSetupStep(void);
extern void CB2_BagMenuFromBattle(void);
extern void SetMonPreventsSwitchingString(void);

extern u8 gPlayerPartyCount;
extern u8 gBattlersCount;
extern u16 gBattlerPartyIndexes[];
extern u8 gBattlerInMenuId;
extern u8 gPartyMenuActionResult;
extern u8 gPartyMenuMessage_IsPrinting;
extern void (*gPokemonItemUseCallback)(u8 taskId, u16 itemId, TaskFunc taskFunc);
extern struct PokemonStorage gPokemonStorage;
void SwitchPartyMonSlots(u8 slot, u8 slot2);
u8 GetPartyIdFromBattlePartyId(u8 battlePartyId);

static void BufferBattlePartyOrder(u8 partyBattleOrder[PARTY_SIZE / 2], u8 flankId);
static void BufferBattlePartyOrderBySide(u8 partyBattleOrder[PARTY_SIZE / 2], u8 flankId, u8 battler);
static void UpdatePartyToFieldOrder(void);
static void Task_BeginExitBattlePartyMenu(u8);
static void Task_ExitBattlePartyMenuToBattle(u8);
static void Task_ExitBattlePartyMenuAfterItem(u8);
static void Task_ExitBattlePartyMenuToBag(u8);
static void CB2_InitBattlePartyMenuAfterSummary(void);
static void Task_HandlePopupMenuInput(u8);
static void Task_BattlePartyMenuSummary(u8 taskId);
static void Task_BattlePartyMenuShift(u8 taskId);
static void Task_BattlePartyMenuCancel(u8 taskId);

#define tPopupMenuType      data[4]
#define tSavedPopupMenuType data[5]

enum
{
    BATTLE_PARTY_MENU_ACTION_SUMMARY,
    BATTLE_PARTY_MENU_ACTION_CANCEL,
    BATTLE_PARTY_MENU_ACTION_SHIFT,
    BATTLE_PARTY_MENU_ACTION_SEND_OUT,
};

enum
{
    BATTLE_PARTY_POPUP_SHIFT,
    BATTLE_PARTY_POPUP_SEND_OUT,
    BATTLE_PARTY_POPUP_SUMMARY,
};

enum
{
    BATTLE_PARTY_MENU_SETUP_MON_ICONS,
    BATTLE_PARTY_MENU_SETUP_LOAD_HELD_ITEM_GFX,
    BATTLE_PARTY_MENU_SETUP_HELD_ITEM_ICONS,
    BATTLE_PARTY_MENU_SETUP_CURSOR,
    BATTLE_PARTY_MENU_SETUP_LEVELS,
    BATTLE_PARTY_MENU_SETUP_NICKNAMES,
    BATTLE_PARTY_MENU_SETUP_HP,
    BATTLE_PARTY_MENU_SETUP_UNUSED,
    BATTLE_PARTY_MENU_SETUP_HP_BARS,
    BATTLE_PARTY_MENU_SETUP_BACKGROUNDS,
    BATTLE_PARTY_MENU_SETUP_READY,
};

static const struct MenuAction2 sBattlePartyMenuActions[] =
{
    {gText_Summary5,             Task_BattlePartyMenuSummary},
    {gText_Cancel, Task_BattlePartyMenuCancel},
    {gText_Shift,               Task_BattlePartyMenuShift},
    {gText_SendOut,             Task_BattlePartyMenuShift},
};
static const u8 sBattlePartyMenuShiftActions[] =
{
    BATTLE_PARTY_MENU_ACTION_SHIFT,
    BATTLE_PARTY_MENU_ACTION_SUMMARY,
    BATTLE_PARTY_MENU_ACTION_CANCEL,
};
static const u8 sBattlePartyMenuSendOutActions[] =
{
    BATTLE_PARTY_MENU_ACTION_SEND_OUT,
    BATTLE_PARTY_MENU_ACTION_SUMMARY,
    BATTLE_PARTY_MENU_ACTION_CANCEL,
};
static const u8 sBattlePartyMenuSummaryActions[] =
{
    BATTLE_PARTY_MENU_ACTION_SUMMARY,
    BATTLE_PARTY_MENU_ACTION_CANCEL,
};
static const struct PartyPopupMenu sBattlePartyPopupMenus[] =
{
    {ARRAY_COUNT(sBattlePartyMenuShiftActions), 9, sBattlePartyMenuShiftActions},
    {ARRAY_COUNT(sBattlePartyMenuSendOutActions), 9, sBattlePartyMenuSendOutActions},
    {ARRAY_COUNT(sBattlePartyMenuSummaryActions), 9, sBattlePartyMenuSummaryActions},
};

void CopyPokemonStorageToBuffer(struct PokemonStorage *ptr)
{
    *ptr = gPokemonStorage;
}

void RestorePokemonStorageFromBuffer(struct PokemonStorage *ptr)
{
    gPokemonStorage = *ptr;
}

void BufferBattlePartyCurrentOrder(void)
{
    BufferBattlePartyOrder(gBattlePartyCurrentOrder, GetPlayerFlankId());
}

void BufferBattlePartyCurrentOrderBySide(u8 battler, u8 flankId)
{
    BufferBattlePartyOrderBySide(gBattleStruct->battlerPartyOrders[battler], flankId, battler);
}

static void BufferBattlePartyOrder(u8 partyBattleOrder[PARTY_SIZE / 2], u8 flankId)
{
    int i;
    u32 partyIdIndex;
    u8 partyIndexes[PARTY_SIZE];
    if (IsLinkDoubleBattle() == TRUE)
    {
        if (flankId)
        {
            *partyBattleOrder = 0x30;
            partyBattleOrder[1] = 0x45;
            partyBattleOrder[2] = 0x12;
        }
        else
        {
            *partyBattleOrder = 0x03;
            partyBattleOrder[1] = 0x12;
            partyBattleOrder[2] = 0x45;
        }
    }
    else
    {
        if (!IsDoubleBattle())
        {
            partyIdIndex = 1;
            *partyIndexes = gBattlerPartyIndexes[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)];
            for (i = 0; i < PARTY_SIZE; i++)
                if (i != *partyIndexes)
                    partyIndexes[partyIdIndex++] = i;
        }
        else
        {
            partyIdIndex = 2;
            *partyIndexes = gBattlerPartyIndexes[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)];
            partyIndexes[1] = gBattlerPartyIndexes[GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT)];
            for (i = 0; i < PARTY_SIZE; i++)
                if (i != *partyIndexes && i != partyIndexes[1])
                    partyIndexes[partyIdIndex++] = i;
        }
        for (i = 0; i <= 2; i++)
            partyBattleOrder[i] = (partyIndexes[i << 1] << 4) | partyIndexes[(i << 1) + 1];
    }
}

static void BufferBattlePartyOrderBySide(u8 partyBattleOrder[PARTY_SIZE / 2], u8 flankId, u8 battler)
{
    int i;
    int leftBattler;
    int rightBattler;
    u32 partyIdIndex;
    u8 partyIndexes[PARTY_SIZE];

    if (GetBattlerSide(battler) == B_SIDE_PLAYER)
    {
        leftBattler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        rightBattler = GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT);
    }
    else
    {
        leftBattler = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
        rightBattler = GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT);
    }
    if (IsLinkDoubleBattle() == TRUE)
    {
        if (flankId)
        {
            *partyBattleOrder = 0x30;
            partyBattleOrder[1] = 0x45;
            partyBattleOrder[2] = 0x12;
        }
        else
        {
            *partyBattleOrder = 0x03;
            partyBattleOrder[1] = 0x12;
            partyBattleOrder[2] = 0x45;
        }
    }
    else
    {
        if (!IsDoubleBattle())
        {
            partyIdIndex = 1;
            *partyIndexes = gBattlerPartyIndexes[leftBattler];
            for (i = 0; i < PARTY_SIZE; i++)
                if (i != *partyIndexes)
                    partyIndexes[partyIdIndex++] = i;
        }
        else
        {
            partyIdIndex = 2;
            *partyIndexes = gBattlerPartyIndexes[leftBattler];
            partyIndexes[1] = gBattlerPartyIndexes[rightBattler];
            for (i = 0; i < PARTY_SIZE; i++)
                if (i != *partyIndexes && i != partyIndexes[1])
                    partyIndexes[partyIdIndex++] = i;
        }
        for (i = 0; i <= 2; i++)
            partyBattleOrder[i] = (partyIndexes[i << 1] << 4) | partyIndexes[(i << 1) + 1];
    }
}

void SwitchPartyOrderLinkMulti(u8 battler, u8 partyId, u8 partySlot)
{
    s32 i;
    s32 j;
    u8 partyIds[PARTY_SIZE];
    u8 partyIdBuffer;
    u8 selectedPartyId = 0;

    if (IsLinkDoubleBattle())
    {
        u8 *partyBattleOrder = gBattleStruct->battlerPartyOrders[battler];

        for (i = 0, j = 0; i < 3; i++)
        {
            partyIds[j++] = partyBattleOrder[i] >> 4;
            partyIds[j++] = partyBattleOrder[i] & 0xF;
        }
        partyIdBuffer = partyIds[partySlot];
        for (i = 0; i < PARTY_SIZE; i++)
        {
            if (partyIds[i] == partyId)
            {
                selectedPartyId = partyIds[i];
                partyIds[i] = partyIdBuffer;
                break;
            }
        }
        if (i != PARTY_SIZE)
        {
            partyIds[partySlot] = selectedPartyId;

            partyBattleOrder[0] = (partyIds[0] << 4) | partyIds[1];
            partyBattleOrder[1] = (partyIds[2] << 4) | partyIds[3];
            partyBattleOrder[2] = (partyIds[4] << 4) | partyIds[5];
        }
    }
}

u8 GetPartyIdFromBattleSlot(u8 slot)
{
    u8 retVal;
    u8 modResult = slot & 1;

    slot /= 2;
    if (modResult)
        retVal = gBattlePartyCurrentOrder[slot] & 0xF;
    else
        retVal = gBattlePartyCurrentOrder[slot] >> 4;
    return retVal;
}

void SetPartyIdAtBattleSlot(u8 slot, u8 setVal)
{
    u8 modResult = slot & 1;

    slot /= 2;
    if (modResult)
        gBattlePartyCurrentOrder[slot] = (gBattlePartyCurrentOrder[slot] & 0xF0) | setVal;
    else
        gBattlePartyCurrentOrder[slot] = (gBattlePartyCurrentOrder[slot] & 0xF) | (setVal << 4);
}

void SwitchPartyMonSlots(u8 slot, u8 slot2)
{
    u8 partyId = GetPartyIdFromBattleSlot(slot);

    SetPartyIdAtBattleSlot(slot, GetPartyIdFromBattleSlot(slot2));
    SetPartyIdAtBattleSlot(slot2, partyId);
}

u8 GetPartyIdFromBattlePartyId(u8 battlePartyId)
{
    u8 i, partySlot;

    for (i = 0, partySlot = 0; i < ARRAY_COUNT(gBattlePartyCurrentOrder); i++)
    {
        if ((gBattlePartyCurrentOrder[i] >> 4) == battlePartyId)
            return partySlot;
        partySlot++;
        if ((gBattlePartyCurrentOrder[i] & 0xF) == battlePartyId)
            return partySlot;
        partySlot++;
    }
    return 0;
}

void UpdatePartyToBattleOrder(void)
{
    u8 i;

    memcpy(gPartyMenu.partyBuffer, gPlayerParty, sizeof(gPlayerParty));
    for (i = 0; i < PARTY_SIZE; i++)
    {
        u8 partyId = GetPartyIdFromBattlePartyId(i);

        memcpy(&gPlayerParty[partyId], &gPartyMenu.partyBuffer[i], sizeof(struct Pokemon));
    }
}

static void UpdatePartyToFieldOrder(void)
{
    struct Pokemon partyBuffer[PARTY_SIZE];
    u8 i;

    memcpy(partyBuffer, gPlayerParty, sizeof(gPlayerParty));
    for (i = 0; i < PARTY_SIZE; i++)
    {
        u8 partyId = GetPartyIdFromBattleSlot(i);

        memcpy(&gPlayerParty[partyId], &partyBuffer[i], sizeof(struct Pokemon));
    }
}

void SwitchAliveMonIntoLeadSlot(void)
{
    u8 i;
    u8 leadPartyId;

    for (i = 1; i < PARTY_SIZE; i++)
    {
        u8 partyId = GetPartyIdFromBattleSlot(i);

        if (GetMonData(&gPlayerParty[partyId], MON_DATA_SPECIES) != 0
         && GetMonData(&gPlayerParty[partyId], MON_DATA_HP) != 0)
        {
            leadPartyId = GetPartyIdFromBattleSlot(0);
            SwitchPartyMonSlots(0, i);
            SwapPokemon(&gPlayerParty[leadPartyId], &gPlayerParty[partyId]);
            break;
        }
    }
}

void OpenBattlePartyMenuWithAction(u8 action)
{
    gPaletteFade.bufferTransferDisabled = TRUE;
    gBattlePartyMenuAction = action;
    ReshowBattleScreenDummy();
    UpdatePartyToBattleOrder();
    OpenPartyMenu(PARTY_MENU_TYPE_BATTLE, 0xFF);
}

void OpenBattlePartyMenuForItemUse(void)
{
    OpenBattlePartyMenuWithAction(PARTY_ACTION_USE_ITEM);
}

bool8 SetUpBattlePartyMenu(void)
{
    switch (gPartyMenu.pmSetupState)
    {
    case BATTLE_PARTY_MENU_SETUP_MON_ICONS:
        if (IsLinkDoubleBattle() == TRUE)
        {
            if (gPartyMenu.pmMonIndex != PARTY_SIZE)
            {
                TryCreatePartyMenuMonIcon(gPartyMenu.menuHandlerTaskId,
                    gPartyMenu.pmMonIndex, &gPlayerParty[gPartyMenu.pmMonIndex]);
                gPartyMenu.pmMonIndex++;
            }
            else
            {
                gPartyMenu.pmMonIndex = 0;
                gPartyMenu.pmSetupState++;
            }
        }
        else
        {
            if (gPartyMenu.pmMonIndex < PARTY_SIZE)
            {
                TryCreatePartyMenuMonIcon(gPartyMenu.menuHandlerTaskId,
                    gPartyMenu.pmMonIndex, &gPlayerParty[gPartyMenu.pmMonIndex]);
                gPartyMenu.pmMonIndex++;
            }
            else
            {
                gPartyMenu.pmMonIndex = 0;
                gPartyMenu.pmSetupState++;
            }
        }
        break;
    case BATTLE_PARTY_MENU_SETUP_LOAD_HELD_ITEM_GFX:
        LoadHeldItemIconGraphics();
        gPartyMenu.pmSetupState++;
        break;
    case BATTLE_PARTY_MENU_SETUP_HELD_ITEM_ICONS:
        CreatePartyMenuHeldItemIcons(gPartyMenu.menuHandlerTaskId);
        gPartyMenu.pmSetupState++;
        break;
    case BATTLE_PARTY_MENU_SETUP_CURSOR:
        if (CreatePartyMenuCursor(gPartyMenu.menuHandlerTaskId, gPartyMenu.pmMonIndex) == 1)
        {
            gPartyMenu.pmMonIndex = 0;
            gPartyMenu.pmSetupState++;
        }
        else
            gPartyMenu.pmMonIndex++;
        break;
    case BATTLE_PARTY_MENU_SETUP_LEVELS:
        PartyMenuPrintMonsLevelOrStatus();
        gPartyMenu.pmSetupState++;
        break;
    case BATTLE_PARTY_MENU_SETUP_NICKNAMES:
        PrintPartyMenuMonNicknames();
        gPartyMenu.pmSetupState++;
        break;
    case BATTLE_PARTY_MENU_SETUP_HP:
        PartyMenuTryPrintMonsHP();
        gPartyMenu.pmSetupState++;
        break;
    case BATTLE_PARTY_MENU_SETUP_UNUSED:
        PartyMenuNoOpSetupStep();
        gPartyMenu.pmSetupState++;
        break;
    case BATTLE_PARTY_MENU_SETUP_HP_BARS:
        PartyMenuDrawHPBars();
        gPartyMenu.pmSetupState++;
        break;
    case BATTLE_PARTY_MENU_SETUP_BACKGROUNDS:
        if (DrawPartyMonBackground(gPartyMenu.pmMonIndex) == 1)
        {
            gPartyMenu.pmMonIndex = 0;
            gPartyMenu.pmSetupState++;
        }
        else
            gPartyMenu.pmMonIndex++;
        break;
    case BATTLE_PARTY_MENU_SETUP_READY:
        if (gBattlePartyMenuAction == PARTY_ACTION_USE_ITEM)
        {
            if (GetItemEffectType(gSpecialVar_ItemId) == ITEM_EFFECT_SACRED_ASH)
                gPartyMenu.promptTextId = PARTY_MSG_NONE;
            else
                gPartyMenu.promptTextId = PARTY_MSG_USE_ON_WHICH_MON;
        }
        return TRUE;
    }
    return FALSE;
}

static void ShowBattlePartyPopupMenu(u8 taskId, u8 monIndex)
{
    if (!GetMonData(&gPlayerParty[monIndex], MON_DATA_IS_EGG))
    {
        if (gBattlePartyMenuAction == PARTY_ACTION_SEND_OUT)
        {
            gTasks[gPartyMenu.menuHandlerTaskId].tPopupMenuType = BATTLE_PARTY_POPUP_SEND_OUT;
            gTasks[gPartyMenu.menuHandlerTaskId].tSavedPopupMenuType = BATTLE_PARTY_POPUP_SEND_OUT;
        }
        else
        {
            gTasks[gPartyMenu.menuHandlerTaskId].tPopupMenuType = BATTLE_PARTY_POPUP_SHIFT;
            gTasks[gPartyMenu.menuHandlerTaskId].tSavedPopupMenuType = BATTLE_PARTY_POPUP_SHIFT;
        }
    }
    else
    {
        gTasks[gPartyMenu.menuHandlerTaskId].tPopupMenuType = BATTLE_PARTY_POPUP_SUMMARY;
        gTasks[gPartyMenu.menuHandlerTaskId].tSavedPopupMenuType = BATTLE_PARTY_POPUP_SUMMARY;
    }

    ShowPartyPopupMenu(gTasks[taskId].tPopupMenuType, sBattlePartyPopupMenus, sBattlePartyMenuActions, 0);
}

void HandleBattlePartyMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        if (gBattlePartyMenuAction == PARTY_ACTION_USE_ITEM
         && GetItemEffectType(gSpecialVar_ItemId) == ITEM_EFFECT_SACRED_ASH)
        {
            gPokemonItemUseCallback(taskId, gSpecialVar_ItemId, Task_ExitBattlePartyMenuAfterItem);
            return;
        }

        switch (HandleDefaultPartyMenuInput(taskId))
        {
        case A_BUTTON:
            if (gBattlePartyMenuAction == PARTY_ACTION_USE_ITEM)
            {
                if (GetMonData(&gPlayerParty[GetCursorSelectionMonId(taskId)], MON_DATA_IS_EGG))
                    PlaySE(SE_FAILURE);
                else
                {
                    PartyMenuEraseMsgBoxAndFrame();
                    gPokemonItemUseCallback(taskId, gSpecialVar_ItemId, Task_ExitBattlePartyMenuAfterItem);
                }
            }
            else
            {
                PlaySE(SE_SELECT);
                GetMonNickname(&gPlayerParty[GetCursorSelectionMonId(taskId)], gStringVar1);
                ShowBattlePartyPopupMenu(taskId, GetCursorSelectionMonId(taskId));
                SetTaskFuncWithFollowupFunc(taskId, Task_HandlePopupMenuInput, HandleBattlePartyMenu);
            }
            break;
        case B_BUTTON:
            if (gBattlePartyMenuAction == PARTY_ACTION_SEND_OUT)
                PlaySE(SE_FAILURE);
            else
            {
                PlaySE(SE_SELECT);
                if (gBattlePartyMenuAction == PARTY_ACTION_USE_ITEM)
                {
                    gPartyMenuActionResult = 0;
                    gTasks[taskId].func = Task_ExitBattlePartyMenuAfterItem;
                }
                else
                {
                    gPartyMenuActionResult = 0;
                    gTasks[taskId].func = Task_BeginExitBattlePartyMenu;
                }
            }
            break;
        }
    }
}

static void Task_BeginExitBattlePartyMenu(u8 taskId)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = Task_ExitBattlePartyMenuToBattle;
}

static void Task_ExitBattlePartyMenuToBattle(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        UpdatePartyToFieldOrder();
        DestroyTask(taskId);
        SetMainCallback2(CB2_SetUpReshowBattleScreenAfterMenu);
    }
}

static void Task_ExitBattlePartyMenuAfterItem(u8 taskId)
{
    if (gPartyMenuActionResult != 0)
        Task_BeginExitBattlePartyMenu(taskId);
    else
    {
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        gTasks[taskId].func = Task_ExitBattlePartyMenuToBag;
    }
}

static void Task_ExitBattlePartyMenuToBag(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        UpdatePartyToFieldOrder();
        DestroyTask(taskId);
        CB2_BagMenuFromBattle();
    }
}

static void CB2_ReopenBattlePartyMenuAfterSummary(void)
{
    gPaletteFade.bufferTransferDisabled = TRUE;
    SetPartyMenuSettings(PARTY_MENU_TYPE_BATTLE, 0xFF, HandleBattlePartyMenu, 5);
    SetMainCallback2(CB2_InitBattlePartyMenuAfterSummary);
}

static void CB2_InitBattlePartyMenuAfterSummary(void)
{
    do
    {
        if (InitPartyMenu() == TRUE)
        {
            SetPartyMenuCursorSelection(gPartyMenu.menuHandlerTaskId, gLastViewedMonIndex);
            ChangePartyMenuSelection(gPartyMenu.menuHandlerTaskId, 0);
            GetMonNickname(&gPlayerParty[gLastViewedMonIndex], gStringVar1);
            ShowBattlePartyPopupMenu(gPartyMenu.menuHandlerTaskId, gLastViewedMonIndex);
            SetTaskFuncWithFollowupFunc(
                gPartyMenu.menuHandlerTaskId, Task_HandlePopupMenuInput, HandleBattlePartyMenu);
            SetMainCallback2(CB2_PartyMenuMain);
            return;
        }
    } while (MenuHelpers_IsLinkActive() != 1);
}

static void Task_HandlePopupMenuInput(u8 taskId)
{
    TaskFunc func;

    if (!gPaletteFade.active)
    {
        if (JOY_REPT(DPAD_UP))
        {
            PlaySE(SE_SELECT);
            Menu_MoveCursor(-1);
            return;
        }
        if (JOY_REPT(DPAD_DOWN))
        {
            PlaySE(SE_SELECT);
            Menu_MoveCursor(1);
            return;
        }
        if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            func = PartyMenuGetPopupMenuFunc(gTasks[taskId].tPopupMenuType,
                               sBattlePartyPopupMenus,
                               sBattlePartyMenuActions,
                               Menu_GetCursorPos());
            func(taskId);
            return;
        }
        if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            Task_BattlePartyMenuCancel(taskId);
            return;
        }
    }
}

static void Task_WaitBattlePartyErrorMessage(u8 taskId)
{
    if (gPartyMenuMessage_IsPrinting == 0)
        Task_BattlePartyMenuCancel(taskId);
}

static void Task_ShowSummaryScreen(u8 taskId)
{
    u8 partySelection = GetCursorSelectionMonId(taskId);

    if (!gPaletteFade.active)
    {
        DestroyTask(taskId);
        gPartyMenu.unusedResumeAction = 1;
        ShowPokemonSummaryScreen(gPlayerParty, partySelection, gPlayerPartyCount - 1, CB2_ReopenBattlePartyMenuAfterSummary, SUMMARY_MODE_LOCK_MOVES);
    }
}

static void Task_BattlePartyMenuSummary(u8 taskId)
{
    GetCursorSelectionMonId(taskId);  // An unused variable was probably set with this.
    gTasks[taskId].func = Task_ShowSummaryScreen;
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
}

static void Task_BattlePartyMenuShift(u8 taskId)
{
    u8 partySelection;
    u8 i;
    u8 currentPartyId;

    ClosePartyPopupMenu(gTasks[taskId].tPopupMenuType, sBattlePartyPopupMenus);
    partySelection = GetCursorSelectionMonId(taskId);
    if (IsLinkDoubleBattle() == TRUE && (partySelection == 1 || partySelection == 4 || partySelection == 5))
    {
        PartyMenuEraseMsgBoxAndFrame();
        StringCopy(gStringVar1, GetTrainerPartnerName());
        StringExpandPlaceholders(gStringVar4, gText_CantSwitchWithAlly);
        DisplayPartyMenuMessage(gStringVar4, 0);
        gTasks[taskId].func = Task_WaitBattlePartyErrorMessage;
        return;
    }
    if (GetMonData(&gPlayerParty[partySelection], MON_DATA_HP) == 0)
    {
        PartyMenuEraseMsgBoxAndFrame();
        GetMonNickname(&gPlayerParty[partySelection], gStringVar1);
        StringExpandPlaceholders(gStringVar4, gText_PkmnHasNoEnergy);
        DisplayPartyMenuMessage(gStringVar4, 0);
        gTasks[taskId].func = Task_WaitBattlePartyErrorMessage;
        return;
    }
    for (i = 0; i < gBattlersCount; i++)
    {
        if (GetBattlerSide(i) == B_SIDE_PLAYER
         && GetPartyIdFromBattleSlot(partySelection) == gBattlerPartyIndexes[i])
        {
            PartyMenuEraseMsgBoxAndFrame();
            GetMonNickname(&gPlayerParty[partySelection], gStringVar1);
            StringExpandPlaceholders(gStringVar4, gText_PkmnAlreadyInBattle);
            DisplayPartyMenuMessage(gStringVar4, 0);
            gTasks[taskId].func = Task_WaitBattlePartyErrorMessage;
            return;
        }
    }
    if (GetMonData(&gPlayerParty[partySelection], MON_DATA_IS_EGG))
    {
        PartyMenuEraseMsgBoxAndFrame();
        StringExpandPlaceholders(gStringVar4, gText_EggCantBattle);
        DisplayPartyMenuMessage(gStringVar4, 0);
        gTasks[taskId].func = Task_WaitBattlePartyErrorMessage;
        return;
    }
    if (GetPartyIdFromBattleSlot(partySelection) == gBattleStruct->prevSelectedPartySlot)
    {
        PartyMenuEraseMsgBoxAndFrame();
        GetMonNickname(&gPlayerParty[partySelection], gStringVar1);
        StringExpandPlaceholders(gStringVar4, gText_PkmnAlreadySelected);
        DisplayPartyMenuMessage(gStringVar4, 0);
        gTasks[taskId].func = Task_WaitBattlePartyErrorMessage;
        return;
    }
    if (gBattlePartyMenuAction == PARTY_ACTION_ABILITY_PREVENTS)
    {
        PartyMenuEraseMsgBoxAndFrame();
        SetMonPreventsSwitchingString();
        DisplayPartyMenuMessage(gStringVar4, 0);
        gTasks[taskId].func = Task_WaitBattlePartyErrorMessage;
        return;
    }
    if (gBattlePartyMenuAction == PARTY_ACTION_CANT_SWITCH)
    {
        u8 battlerPartyId;
        u8 currentBattler = gBattlerInMenuId;

        PartyMenuEraseMsgBoxAndFrame();
        battlerPartyId = GetPartyIdFromBattlePartyId(gBattlerPartyIndexes[currentBattler]);
        GetMonNickname(&gPlayerParty[battlerPartyId], gStringVar1);
        StringExpandPlaceholders(gStringVar4, gText_PkmnCantSwitchOut);
        DisplayPartyMenuMessage(gStringVar4, 0);
        gTasks[taskId].func = Task_WaitBattlePartyErrorMessage;
        return;
    }
    gSelectedMonPartyId = GetPartyIdFromBattleSlot(partySelection);
    gPartyMenuActionResult = 1;
    currentPartyId = GetPartyIdFromBattlePartyId(gBattlerPartyIndexes[gBattlerInMenuId]);
    SwitchPartyMonSlots(currentPartyId, partySelection);
    SwapPokemon(&gPlayerParty[currentPartyId], &gPlayerParty[partySelection]);
    gTasks[taskId].func = Task_BeginExitBattlePartyMenu;
}

static void Task_BattlePartyMenuCancel(u8 taskId)
{
    Menu_DestroyCursor();
    ClosePartyPopupMenu(gTasks[taskId].tPopupMenuType, sBattlePartyPopupMenus);
    gTasks[taskId].tPopupMenuType = gTasks[taskId].tSavedPopupMenuType;
    PrintPartyMenuPromptText(PARTY_MSG_CHOOSE_MON, PARTY_MENU_PROMPT_WINDOW_STANDARD);
    SwitchTaskToFollowupFunc(taskId);
}
