#include "global.h"
#include "reshow_battle_screen.h"
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
#include "constants/songs.h"
#include "sound.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "text.h"
#include "ewram.h"

EWRAM_DATA u8 gBattlePartyCurrentOrder[3] = {};
EWRAM_DATA u8 gUnknown_02038473 = 0;

extern u8 sub_806BD58(u8, u8);
extern void PartyMenuPrintMonsLevelOrStatus(void);
extern void nullsub_13(void);
extern void CB2_SetUpReshowBattleScreenAfterMenu(void);
extern void sub_80A6DCC(void);
extern u8 *sub_8040D08();

extern u8 gPlayerPartyCount;
extern u8 gBattlersCount;
extern u16 gBattlerPartyIndexes[];
extern u8 gBattlerInMenuId;
extern u8 gUnknown_0202E8F4;
extern u8 gSelectedMonPartyId;
extern u8 gPartyMenuMessage_IsPrinting;
extern void (*gPokemonItemUseCallback)();  //don't know types yet
extern struct PokemonStorage gPokemonStorage;

static void BufferBattlePartyOrder(u8[3], u8);
static void BufferBattlePartyOrderBySide(u8[3], u8, u32);
static void UpdatePartyToFieldOrder(void);
static void Task_ClosePartyMenu(u8);
static void Task_ClosePartyMenuAndSetCB2(u8);
static void Task_80952E4(u8);
static void Task_8095330(u8);
static void Task_809538C(void);
static void Task_HandlePopupMenuInput(u8);
static void Task_BattlePartyMenuSummary(u8 taskId);
static void Task_BattlePartyMenuShift(u8 taskId);
static void Task_BattlePartyMenuCancel(u8 taskId);

static const struct MenuAction2 sBattlePartyMenuActions[] =
{
    {gText_Summary5,             Task_BattlePartyMenuSummary},
    {gText_Cancel2, Task_BattlePartyMenuCancel},
    {gText_Shift,               Task_BattlePartyMenuShift},
    {gText_SendOut,             Task_BattlePartyMenuShift},
};
static const u8 sPartyMenuAction_ShiftSummaryCancel[] = {2, 0, 1};  //SHIFT, SUMMARY, CANCEL
static const u8 sPartyMenuAction_SendOutSummaryCancel[] = {3, 0, 1};  //SEND OUT, SUMMARY, CANCEL
static const u8 sPartyMenuAction_SummaryCancel[] = {0, 1};     //SUMMARY, CANCEL
static const struct PartyPopupMenu sBattlePartyPopupMenus[] =
{
    {ARRAY_COUNT(sPartyMenuAction_ShiftSummaryCancel), 9, sPartyMenuAction_ShiftSummaryCancel},
    {ARRAY_COUNT(sPartyMenuAction_SendOutSummaryCancel), 9, sPartyMenuAction_SendOutSummaryCancel},
    {ARRAY_COUNT(sPartyMenuAction_SummaryCancel), 9, sPartyMenuAction_SummaryCancel},
};

void unref_sub_8094928(struct PokemonStorage *ptr)
{
    *ptr = gPokemonStorage;
}

void unref_sub_8094940(struct PokemonStorage *ptr)
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

static void BufferBattlePartyOrder(u8 partyBattleOrder[3], u8 flankId)
{
    int i;
    u32 pos;
    u8 partyIds[6];
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
            pos = 1;
            *partyIds = gBattlerPartyIndexes[GetBattlerAtPosition(0)];
            for (i = 0; i <= 5; i++)
                if (i != *partyIds)
                    partyIds[pos++] = i;
        }
        else
        {
            pos = 2;
            *partyIds = gBattlerPartyIndexes[GetBattlerAtPosition(0)];
            partyIds[1] = gBattlerPartyIndexes[GetBattlerAtPosition(2)];
            for (i = 0; i <= 5; i++)
                if ((i != *partyIds) && (i != partyIds[1]))
                    partyIds[pos++] = i;
        }
        for (i = 0; i <= 2; i++)
            partyBattleOrder[i] = (partyIds[i << 1] << 4) | partyIds[(i << 1) + 1];
    }
}

static void BufferBattlePartyOrderBySide(u8 partyBattleOrder[3], u8 flankId, u32 battler)
{
    int i, j;
    u8 partyIndexes[6];
    if (!GetBattlerSide(battler))
    {
        i = GetBattlerAtPosition(0);
        j = GetBattlerAtPosition(2);
    }
    else
    {
        i = GetBattlerAtPosition(1);
        j = GetBattlerAtPosition(3);
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
            int pos = 1;
            *partyIndexes = gBattlerPartyIndexes[i];
            for (i = 0; i <= 5; i++)
                if (i != *partyIndexes)
                    partyIndexes[pos++] = i;
        }
        else
        {
            int pos = 2;
            *partyIndexes = gBattlerPartyIndexes[i];
            partyIndexes[1] = gBattlerPartyIndexes[j];
            for (i = 0; i <= 5; i++)
                if ((i != *partyIndexes) && (i != partyIndexes[1]))
                    partyIndexes[pos++] = i;
        }
        for (i = 0; i <= 2; i++)
            partyBattleOrder[i] = (partyIndexes[i << 1] << 4) | partyIndexes[(i << 1) + 1];
    }
}

void SwitchPartyOrderLinkMulti(u8 battler, u8 slot, u8 slot2)
{
    s32 i;
    s32 j;
    u8 partyIds[6];
    u8 partyIdBuffer;
    u8 tempSlot = 0;

    if (IsLinkDoubleBattle())
    {
        u8 *partyBattleOrder = gBattleStruct->battlerPartyOrders[battler];

        for (i = 0, j = 0; i < 3; i++)
        {
            partyIds[j++] = partyBattleOrder[i] >> 4;
            partyIds[j++] = partyBattleOrder[i] & 0xF;
        }
        partyIdBuffer = partyIds[slot2];
        for (i = 0; i < 6; i++)
        {
            if (partyIds[i] == slot)
            {
                tempSlot = partyIds[i];
                partyIds[i] = partyIdBuffer;
                break;
            }
        }
        if (i != 6)
        {
            partyIds[slot2] = tempSlot;

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

static void SetPartyIdAtBattleSlot(u8 slot, u8 setVal)
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
    u8 r1 = GetPartyIdFromBattleSlot(slot2);

    SetPartyIdAtBattleSlot(slot, r1);
    SetPartyIdAtBattleSlot(slot2, partyId);
}

u8 GetPartyIdFromBattlePartyId(u8 battlePartyId)
{
    u8 i;
    u8 j;

    for (i = 0, j = 0; i < 3; i++)
    {
        if ((gBattlePartyCurrentOrder[i] >> 4) == battlePartyId)
            return j;
        j++;
        if ((gBattlePartyCurrentOrder[i] & 0xF) == battlePartyId)
            return j;
        j++;
    }
    return 0;
}

static void UpdatePartyToBattleOrder(void)
{
    u8 i;

    memcpy(ePartyMenu2.unk0, gPlayerParty, sizeof(gPlayerParty));
    for (i = 0; i < 6; i++)
    {
        u8 n = GetPartyIdFromBattlePartyId(i);

        memcpy(&gPlayerParty[n], &ePartyMenu2.unk0[i], sizeof(struct Pokemon));
    }
}

static void UpdatePartyToFieldOrder(void)
{
    struct Pokemon partyBuffer[6];
    u8 i;

    memcpy(partyBuffer, gPlayerParty, sizeof(gPlayerParty));
    for (i = 0; i < 6; i++)
    {
        u8 n = GetPartyIdFromBattleSlot(i);

        memcpy(&gPlayerParty[n], &partyBuffer[i], sizeof(struct Pokemon));
    }
}

static void UNUSED SwitchAliveMonIntoLeadSlot(void)
{
    u8 i;
    u8 partyId;

    for (i = 1; i < 6; i++)
    {
        u8 n = GetPartyIdFromBattleSlot(i);

        if (GetMonData(&gPlayerParty[n], MON_DATA_SPECIES) != 0
         && GetMonData(&gPlayerParty[n], MON_DATA_HP) != 0)
        {
            partyId = GetPartyIdFromBattleSlot(0);
            SwitchPartyMonSlots(0, i);
            SwapPokemon(&gPlayerParty[partyId], &gPlayerParty[n]);
            break;
        }
    }
}

void OpenPartyMenuInBattle(u8 partyAction)
{
    gPaletteFade.bufferTransferDisabled = TRUE;
    gUnknown_02038473 = partyAction;
    ReshowBattleScreenDummy();
    UpdatePartyToBattleOrder();
    OpenPartyMenu(PARTY_MENU_TYPE_BATTLE, 0xFF);
}

void ChooseMonForInBattleItem(void)
{
    OpenPartyMenuInBattle(3);
}

bool8 SetUpBattlePartyMenu(void)
{
    switch (ePartyMenu2.pmSetupState)
    //switch (ePartyMenu2.unk264[0])
    {
    case 0:
        //TODO: try to get rid of this duplicate code
        if (IsLinkDoubleBattle() == TRUE)
        {
            if (ePartyMenu2.pmMonIndex != 6)
            {
                TryCreatePartyMenuMonIcon(ePartyMenu2.menuHandlerTaskId,
                    ePartyMenu2.pmMonIndex, &gPlayerParty[ePartyMenu2.pmMonIndex]);
                ePartyMenu2.pmMonIndex++;
            }
            else
            {
                ePartyMenu2.pmMonIndex = 0;
                ePartyMenu2.pmSetupState++;
            }
        }
        else
        {
            if (ePartyMenu2.pmMonIndex < 6)
            {
                TryCreatePartyMenuMonIcon(ePartyMenu2.menuHandlerTaskId,
                    ePartyMenu2.pmMonIndex, &gPlayerParty[ePartyMenu2.pmMonIndex]);
                ePartyMenu2.pmMonIndex++;
            }
            else
            {
                ePartyMenu2.pmMonIndex = 0;
                ePartyMenu2.pmSetupState++;
            }
        }
        break;
    case 1:
        LoadHeldItemIcons();
        ePartyMenu2.pmSetupState++;
        break;
    case 2:
        CreateHeldItemIcons_806DC34(ePartyMenu2.menuHandlerTaskId);
        ePartyMenu2.pmSetupState++;
        break;
    case 3:
        if (sub_806BD58(ePartyMenu2.menuHandlerTaskId, ePartyMenu2.pmMonIndex) == 1)
        {
            ePartyMenu2.pmMonIndex = 0;
            ePartyMenu2.pmSetupState++;
        }
        else
            ePartyMenu2.pmMonIndex++;
        break;
    case 4:
        PartyMenuPrintMonsLevelOrStatus();
        ePartyMenu2.pmSetupState++;
        break;
    case 5:
        PrintPartyMenuMonNicknames();
        ePartyMenu2.pmSetupState++;
        break;
    case 6:
        PartyMenuTryPrintMonsHP();
        ePartyMenu2.pmSetupState++;
        break;
    case 7:
        nullsub_13();
        ePartyMenu2.pmSetupState++;
        break;
    case 8:
        PartyMenuDrawHPBars();
        ePartyMenu2.pmSetupState++;
        break;
    case 9:
        if (DrawPartyMonBackground(ePartyMenu2.pmMonIndex) == 1)
        {
            ePartyMenu2.pmMonIndex = 0;
            ePartyMenu2.pmSetupState++;
        }
        else
            ePartyMenu2.pmMonIndex++;
        break;
    case 10:
        if (gUnknown_02038473 == 3)
        {
            if (GetItemEffectType(gSpecialVar_ItemId) == 10)
                ePartyMenu2.promptTextId = 0xFF;
            else
                ePartyMenu2.promptTextId = 3;
        }
        return TRUE;
    }
    return FALSE;
}

static void sub_8095050(u8 a, u8 b)
{
    if (!GetMonData(&gPlayerParty[b], MON_DATA_IS_EGG))
    {
        if (gUnknown_02038473 == 1)
        {
            gTasks[ePartyMenu2.menuHandlerTaskId].data[4] = 1;
            gTasks[ePartyMenu2.menuHandlerTaskId].data[5] = 1;
        }
        else
        {
            gTasks[ePartyMenu2.menuHandlerTaskId].data[4] = 0;
            gTasks[ePartyMenu2.menuHandlerTaskId].data[5] = 0;
        }
    }
    else
    {
        gTasks[ePartyMenu2.menuHandlerTaskId].data[4] = 2;
        gTasks[ePartyMenu2.menuHandlerTaskId].data[5] = 2;
    }

    ShowPartyPopupMenu(gTasks[a].data[4], sBattlePartyPopupMenus, sBattlePartyMenuActions, 0);
}

void HandleBattlePartyMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        if (gUnknown_02038473 == 3 && GetItemEffectType(gSpecialVar_ItemId) == 10)
        {
            gPokemonItemUseCallback(taskId, gSpecialVar_ItemId, Task_80952E4);
            return;
        }

        switch (HandleDefaultPartyMenuInput(taskId))
        {
        case A_BUTTON:
            if (gUnknown_02038473 == 3)
            {
                if (GetMonData(&gPlayerParty[GetCursorSelectionMonId(taskId)], MON_DATA_IS_EGG))
                    PlaySE(SE_FAILURE);
                else
                {
                    PartyMenuEraseMsgBoxAndFrame();
                    gPokemonItemUseCallback(taskId, gSpecialVar_ItemId, Task_80952E4);
                }
            }
            else
            {
                PlaySE(SE_SELECT);
                GetMonNickname(&gPlayerParty[GetCursorSelectionMonId(taskId)], gStringVar1);
                sub_8095050(taskId, GetCursorSelectionMonId(taskId));
                SetTaskFuncWithFollowupFunc(taskId, Task_HandlePopupMenuInput, HandleBattlePartyMenu);
            }
            break;
        case B_BUTTON:
            if (gUnknown_02038473 == 1)
                PlaySE(SE_FAILURE);
            else
            {
                PlaySE(SE_SELECT);
                if (gUnknown_02038473 == 3)
                {
                    gUnknown_0202E8F4 = 0;
                    gTasks[taskId].func = Task_80952E4;
                }
                else
                {
                    gUnknown_0202E8F4 = 0;
                    gTasks[taskId].func = Task_ClosePartyMenu;
                }
            }
            break;
        }
    }
}

static void Task_ClosePartyMenu(u8 taskId)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = Task_ClosePartyMenuAndSetCB2;
}

static void Task_ClosePartyMenuAndSetCB2(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        UpdatePartyToFieldOrder();
        DestroyTask(taskId);
        SetMainCallback2(CB2_SetUpReshowBattleScreenAfterMenu);
    }
}

static void Task_80952E4(u8 taskId)
{
    if (gUnknown_0202E8F4 != 0)
        Task_ClosePartyMenu(taskId);
    else
    {
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        gTasks[taskId].func = Task_8095330;
    }
}

static void Task_8095330(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        UpdatePartyToFieldOrder();
        DestroyTask(taskId);
        sub_80A6DCC();
    }
}

static void Task_809535C(void)
{
    gPaletteFade.bufferTransferDisabled = TRUE;
    SetPartyMenuSettings(PARTY_MENU_TYPE_BATTLE, 0xFF, HandleBattlePartyMenu, 5);
    SetMainCallback2(Task_809538C);
}

static void Task_809538C(void)
{
    do
    {
        if (InitPartyMenu() == TRUE)
        {
            sub_806C994(ePartyMenu2.menuHandlerTaskId, gLastViewedMonIndex);
            ChangePartyMenuSelection(ePartyMenu2.menuHandlerTaskId, 0);
            GetMonNickname(&gPlayerParty[gLastViewedMonIndex], gStringVar1);
            sub_8095050(ePartyMenu2.menuHandlerTaskId, gLastViewedMonIndex);
            SetTaskFuncWithFollowupFunc(
                ePartyMenu2.menuHandlerTaskId, Task_HandlePopupMenuInput, HandleBattlePartyMenu);
            SetMainCallback2(CB2_PartyMenuMain);
            return;
        }
    } while (sub_80F9344() != 1);
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
            func = PartyMenuGetPopupMenuFunc(gTasks[taskId].data[4],
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

static void Task_80954C0(u8 taskId)
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
        ePartyMenu2.unk262 = 1;
        ShowPokemonSummaryScreen(gPlayerParty, partySelection, gPlayerPartyCount - 1, Task_809535C, PSS_MODE_NO_MOVE_ORDER_EDIT);
    }
}

static void Task_BattlePartyMenuSummary(u8 taskId)
{
    GetCursorSelectionMonId(taskId);  //an unused variable was probably set with this.
    gTasks[taskId].func = Task_ShowSummaryScreen;
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
}

static void Task_BattlePartyMenuShift(u8 taskId)
{
    u8 partySelection;
    u8 i;
    u8 r4;

    ClosePartyPopupMenu(gTasks[taskId].data[4], sBattlePartyPopupMenus);
    partySelection = GetCursorSelectionMonId(taskId);
    if (IsLinkDoubleBattle() == TRUE && (partySelection == 1 || partySelection == 4 || partySelection == 5))
    {
        PartyMenuEraseMsgBoxAndFrame();
        StringCopy(gStringVar1, sub_8040D08());
        StringExpandPlaceholders(gStringVar4, gText_CantSwitchWithAlly);
        DisplayPartyMenuMessage(gStringVar4, 0);
        gTasks[taskId].func = Task_80954C0;
        return;
    }
    if (GetMonData(&gPlayerParty[partySelection], MON_DATA_HP) == 0)
    {
        PartyMenuEraseMsgBoxAndFrame();
        GetMonNickname(&gPlayerParty[partySelection], gStringVar1);
        StringExpandPlaceholders(gStringVar4, gText_PkmnHasNoEnergy);
        DisplayPartyMenuMessage(gStringVar4, 0);
        gTasks[taskId].func = Task_80954C0;
        return;
    }
    for (i = 0; i < gBattlersCount; i++)
    {
        if (GetBattlerSide(i) == 0
         && GetPartyIdFromBattleSlot(partySelection) == gBattlerPartyIndexes[i])
        {
            PartyMenuEraseMsgBoxAndFrame();
            GetMonNickname(&gPlayerParty[partySelection], gStringVar1);
            StringExpandPlaceholders(gStringVar4, gText_PkmnAlreadyInBattle);
            DisplayPartyMenuMessage(gStringVar4, 0);
            gTasks[taskId].func = Task_80954C0;
            return;
        }
    }
    if (GetMonData(&gPlayerParty[partySelection], MON_DATA_IS_EGG))
    {
        PartyMenuEraseMsgBoxAndFrame();
        StringExpandPlaceholders(gStringVar4, gText_EggCantBattle);
        DisplayPartyMenuMessage(gStringVar4, 0);
        gTasks[taskId].func = Task_80954C0;
        return;
    }
    if (GetPartyIdFromBattleSlot(partySelection) == gBattleStruct->prevSelectedPartySlot)
    {
        PartyMenuEraseMsgBoxAndFrame();
        GetMonNickname(&gPlayerParty[partySelection], gStringVar1);
        StringExpandPlaceholders(gStringVar4, gText_PkmnAlreadySelected);
        DisplayPartyMenuMessage(gStringVar4, 0);
        gTasks[taskId].func = Task_80954C0;
        return;
    }
    if (gUnknown_02038473 == 4)
    {
        PartyMenuEraseMsgBoxAndFrame();
        SetMonPreventsSwitchingString();
        DisplayPartyMenuMessage(gStringVar4, 0);
        gTasks[taskId].func = Task_80954C0;
        return;
    }
    if (gUnknown_02038473 == 2)
    {
        u8 r0;
        u8 r4 = gBattlerInMenuId;

        PartyMenuEraseMsgBoxAndFrame();
        r0 = GetPartyIdFromBattlePartyId(gBattlerPartyIndexes[r4]);
        GetMonNickname(&gPlayerParty[r0], gStringVar1);
        StringExpandPlaceholders(gStringVar4, gText_PkmnCantSwitchOut);
        DisplayPartyMenuMessage(gStringVar4, 0);
        gTasks[taskId].func = Task_80954C0;
        return;
    }
    gSelectedMonPartyId = GetPartyIdFromBattleSlot(partySelection);
    gUnknown_0202E8F4 = 1;
    r4 = GetPartyIdFromBattlePartyId(gBattlerPartyIndexes[gBattlerInMenuId]);
    SwitchPartyMonSlots(r4, partySelection);
    SwapPokemon(&gPlayerParty[r4], &gPlayerParty[partySelection]);
    gTasks[taskId].func = Task_ClosePartyMenu;
}

static void Task_BattlePartyMenuCancel(u8 taskId)
{
    Menu_DestroyCursor();
    ClosePartyPopupMenu(gTasks[taskId].data[4], sBattlePartyPopupMenus);
    gTasks[taskId].data[4] = gTasks[taskId].data[5];
    PrintPartyMenuPromptText(0, 0);
    SwitchTaskToFollowupFunc(taskId);
}
