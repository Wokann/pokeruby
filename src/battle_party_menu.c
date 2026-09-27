#include "global.h"
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
#include "rom_8077ABC.h"
#include "constants/songs.h"
#include "sound.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "text.h"
#include "ewram.h"

EWRAM_DATA u8 gBattlePartyCurrentOrder[3] = {};
EWRAM_DATA u8 gBattlePartyMenuAction = 0;

extern u8 sub_806BD58(u8, u8);
extern void PartyMenuPrintMonsLevelOrStatus(void);
extern void nullsub_13(void);
extern void sub_802E414(void);
extern void sub_80A6DCC(void);
extern u8 *sub_8040D08();
extern void SetMonPreventsSwitchingString(void);
extern void ReshowBattleScreenDummy();
extern u8 sub_803FBBC(void);

extern u8 gPlayerPartyCount;
extern u8 gBattlersCount;
extern u16 gBattlerPartyIndexes[];
extern u8 gBattlerInMenuId;
extern u8 gUnknown_0202E8F4;
extern u8 gUnknown_0202E8F5;
extern u8 gPartyMenuMessage_IsPrinting;
extern u8 gUnknown_020384F0;
extern void (*gPokemonItemUseCallback)();  //don't know types yet
extern struct PokemonStorage gPokemonStorage;
extern void ReshowBattleScreenDummy();

void SwitchPartyMonSlots(u8, u8);
u8 GetPartyIdFromBattlePartyId(u8);

static void BufferBattlePartyOrder(u8[3], u8);
static void BufferBattlePartyOrderBySide(u8[3], u8, u32);
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

static const struct MenuAction2 sBattlePartyMenuActions[] =
{
    {OtherText_Summary,             Task_BattlePartyMenuSummary},
    {gOtherText_CancelNoTerminator, Task_BattlePartyMenuCancel},
    {OtherText_Shift,               Task_BattlePartyMenuShift},
    {OtherText_SendOut,             Task_BattlePartyMenuShift},
};
static const u8 sBattlePartyMenuShiftActions[] = {2, 0, 1};  //SHIFT, SUMMARY, CANCEL
static const u8 sBattlePartyMenuSendOutActions[] = {3, 0, 1};  //SEND OUT, SUMMARY, CANCEL
static const u8 sBattlePartyMenuSummaryActions[] = {0, 1};     //SUMMARY, CANCEL
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
    BufferBattlePartyOrder(gBattlePartyCurrentOrder, sub_803FBBC());
}

void BufferBattlePartyCurrentOrderBySide(u8 arg1, u8 arg2)
{
    BufferBattlePartyOrderBySide(gBattleStruct->unk1606C[arg1], arg2, arg1);
}

static void BufferBattlePartyOrder(u8 arg[3], u8 player_number)
{
    int i;
    u32 pos;
    u8 temp[6];
    if (IsLinkDoubleBattle() == TRUE)
    {
        if (player_number)
        {
            *arg = 0x30;
            arg[1] = 0x45;
            arg[2] = 0x12;
        }
        else
        {
            *arg = 0x03;
            arg[1] = 0x12;
            arg[2] = 0x45;
        }
    }
    else
    {
        if (!IsDoubleBattle())
        {
            pos = 1;
            *temp = gBattlerPartyIndexes[GetBattlerAtPosition(0)];
            for (i = 0; i <= 5; i++)
                if (i != *temp)
                    temp[pos++] = i;
        }
        else
        {
            pos = 2;
            *temp = gBattlerPartyIndexes[GetBattlerAtPosition(0)];
            temp[1] = gBattlerPartyIndexes[GetBattlerAtPosition(2)];
            for (i = 0; i <= 5; i++)
                if ((i != *temp) && (i != temp[1]))
                    temp[pos++] = i;
        }
        for (i = 0; i <= 2; i++)
            arg[i] = (temp[i << 1] << 4) | temp[(i << 1) + 1];
    }
}

static void BufferBattlePartyOrderBySide(u8 arg[3], u8 player_number, u32 arg3)
{
    int i, j;
    u8 temp[6];
    if (!GetBattlerSide(arg3))
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
        if (player_number)
        {
            *arg = 0x30;
            arg[1] = 0x45;
            arg[2] = 0x12;
        }
        else
        {
            *arg = 0x03;
            arg[1] = 0x12;
            arg[2] = 0x45;
        }
    }
    else
    {
        if (!IsDoubleBattle())
        {
            int pos = 1;
            *temp = gBattlerPartyIndexes[i];
            for (i = 0; i <= 5; i++)
                if (i != *temp)
                    temp[pos++] = i;
        }
        else
        {
            int pos = 2;
            *temp = gBattlerPartyIndexes[i];
            temp[1] = gBattlerPartyIndexes[j];
            for (i = 0; i <= 5; i++)
                if ((i != *temp) && (i != temp[1]))
                    temp[pos++] = i;
        }
        for (i = 0; i <= 2; i++)
            arg[i] = (temp[i << 1] << 4) | temp[(i << 1) + 1];
    }
}

void SwitchPartyOrderLinkMulti(u8 a, u8 b, u8 c)
{
    s32 i;
    s32 j;
    u8 temp[6];
    u8 r3;
    u8 r7 = 0;

    if (IsLinkDoubleBattle())
    {
        u8 *arr = gBattleStruct->unk1606C[a];

        for (i = 0, j = 0; i < 3; i++)
        {
            temp[j++] = arr[i] >> 4;
            temp[j++] = arr[i] & 0xF;
        }
        r3 = temp[c];
        for (i = 0; i < 6; i++)
        {
            if (temp[i] == b)
            {
                r7 = temp[i];
                temp[i] = r3;
                break;
            }
        }
        if (i != 6)
        {
            temp[c] = r7;

            arr[0] = (temp[0] << 4) | temp[1];
            arr[1] = (temp[2] << 4) | temp[3];
            arr[2] = (temp[4] << 4) | temp[5];
        }
    }
}

u8 GetPartyIdFromBattleSlot(u8 monIndex)
{
    u8 retVal;
    u8 val = monIndex & 1;

    monIndex /= 2;
    if (val)
        retVal = gBattlePartyCurrentOrder[monIndex] & 0xF;
    else
        retVal = gBattlePartyCurrentOrder[monIndex] >> 4;
    return retVal;
}

void SetPartyIdAtBattleSlot(u8 a, u8 b)
{
    u8 val = a & 1;

    a /= 2;
    if (val)
        gBattlePartyCurrentOrder[a] = (gBattlePartyCurrentOrder[a] & 0xF0) | b;
    else
        gBattlePartyCurrentOrder[a] = (gBattlePartyCurrentOrder[a] & 0xF) | (b << 4);
}

void SwitchPartyMonSlots(u8 a, u8 b)
{
    u8 r4 = GetPartyIdFromBattleSlot(a);
    u8 r1 = GetPartyIdFromBattleSlot(b);

    SetPartyIdAtBattleSlot(a, r1);
    SetPartyIdAtBattleSlot(b, r4);
}

u8 GetPartyIdFromBattlePartyId(u8 a)
{
    u8 i;
    u8 r2;

    for (i = 0, r2 = 0; i < 3; i++)
    {
        if ((gBattlePartyCurrentOrder[i] >> 4) == a)
            return r2;
        r2++;
        if ((gBattlePartyCurrentOrder[i] & 0xF) == a)
            return r2;
        r2++;
    }
    return 0;
}

void UpdatePartyToBattleOrder(void)
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
    struct Pokemon temp[6];
    u8 i;

    memcpy(temp, gPlayerParty, sizeof(gPlayerParty));
    for (i = 0; i < 6; i++)
    {
        u8 n = GetPartyIdFromBattleSlot(i);

        memcpy(&gPlayerParty[n], &temp[i], sizeof(struct Pokemon));
    }
}

void TrySwitchFirstHealthyPartyMonToFront(void)
{
    u8 i;
    u8 r4;

    for (i = 1; i < 6; i++)
    {
        u8 n = GetPartyIdFromBattleSlot(i);

        if (GetMonData(&gPlayerParty[n], MON_DATA_SPECIES) != 0
         && GetMonData(&gPlayerParty[n], MON_DATA_HP) != 0)
        {
            r4 = GetPartyIdFromBattleSlot(0);
            SwitchPartyMonSlots(0, i);
            SwapPokemon(&gPlayerParty[r4], &gPlayerParty[n]);
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
    OpenBattlePartyMenuWithAction(3);
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
        LoadHeldItemIconGraphics();
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
        if (gBattlePartyMenuAction == 3)
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

static void ShowBattlePartyPopupMenu(u8 a, u8 b)
{
    if (!GetMonData(&gPlayerParty[b], MON_DATA_IS_EGG))
    {
        if (gBattlePartyMenuAction == 1)
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
        if (gBattlePartyMenuAction == 3 && GetItemEffectType(gSpecialVar_ItemId) == 10)
        {
            gPokemonItemUseCallback(taskId, gSpecialVar_ItemId, Task_ExitBattlePartyMenuAfterItem);
            return;
        }

        switch (HandleDefaultPartyMenuInput(taskId))
        {
        case A_BUTTON:
            if (gBattlePartyMenuAction == 3)
            {
                if (GetMonData(&gPlayerParty[sub_806CA38(taskId)], MON_DATA_IS_EGG))
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
                GetMonNickname(&gPlayerParty[sub_806CA38(taskId)], gStringVar1);
                ShowBattlePartyPopupMenu(taskId, sub_806CA38(taskId));
                SetTaskFuncWithFollowupFunc(taskId, Task_HandlePopupMenuInput, HandleBattlePartyMenu);
            }
            break;
        case B_BUTTON:
            if (gBattlePartyMenuAction == 1)
                PlaySE(SE_FAILURE);
            else
            {
                PlaySE(SE_SELECT);
                if (gBattlePartyMenuAction == 3)
                {
                    gUnknown_0202E8F4 = 0;
                    gTasks[taskId].func = Task_ExitBattlePartyMenuAfterItem;
                }
                else
                {
                    gUnknown_0202E8F4 = 0;
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
        SetMainCallback2(sub_802E414);
    }
}

static void Task_ExitBattlePartyMenuAfterItem(u8 taskId)
{
    if (gUnknown_0202E8F4 != 0)
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
        sub_80A6DCC();
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
            sub_806C994(ePartyMenu2.menuHandlerTaskId, gUnknown_020384F0);
            ChangePartyMenuSelection(ePartyMenu2.menuHandlerTaskId, 0);
            GetMonNickname(&gPlayerParty[gUnknown_020384F0], gStringVar1);
            ShowBattlePartyPopupMenu(ePartyMenu2.menuHandlerTaskId, gUnknown_020384F0);
            SetTaskFuncWithFollowupFunc(
                ePartyMenu2.menuHandlerTaskId, Task_HandlePopupMenuInput, HandleBattlePartyMenu);
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

static void Task_WaitBattlePartyErrorMessage(u8 taskId)
{
    if (gPartyMenuMessage_IsPrinting == 0)
        Task_BattlePartyMenuCancel(taskId);
}

static void Task_ShowSummaryScreen(u8 taskId)
{
    u8 partySelection = sub_806CA38(taskId);

    if (!gPaletteFade.active)
    {
        DestroyTask(taskId);
        ePartyMenu2.unk262 = 1;
        ShowPokemonSummaryScreen(gPlayerParty, partySelection, gPlayerPartyCount - 1, CB2_ReopenBattlePartyMenuAfterSummary, PSS_MODE_NO_MOVE_ORDER_EDIT);
    }
}

static void Task_BattlePartyMenuSummary(u8 taskId)
{
    sub_806CA38(taskId);  //an unused variable was probably set with this.
    gTasks[taskId].func = Task_ShowSummaryScreen;
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
}

static void Task_BattlePartyMenuShift(u8 taskId)
{
    u8 partySelection;
    u8 i;
    u8 r4;

    ClosePartyPopupMenu(gTasks[taskId].data[4], sBattlePartyPopupMenus);
    partySelection = sub_806CA38(taskId);
    if (IsLinkDoubleBattle() == TRUE && (partySelection == 1 || partySelection == 4 || partySelection == 5))
    {
        PartyMenuEraseMsgBoxAndFrame();
        StringCopy(gStringVar1, sub_8040D08());
        StringExpandPlaceholders(gStringVar4, gOtherText_CantSwitchPokeWithYours);
        DisplayPartyMenuMessage(gStringVar4, 0);
        gTasks[taskId].func = Task_WaitBattlePartyErrorMessage;
        return;
    }
    if (GetMonData(&gPlayerParty[partySelection], MON_DATA_HP) == 0)
    {
        PartyMenuEraseMsgBoxAndFrame();
        GetMonNickname(&gPlayerParty[partySelection], gStringVar1);
        StringExpandPlaceholders(gStringVar4, gOtherText_NoEnergyLeft);
        DisplayPartyMenuMessage(gStringVar4, 0);
        gTasks[taskId].func = Task_WaitBattlePartyErrorMessage;
        return;
    }
    for (i = 0; i < gBattlersCount; i++)
    {
        if (GetBattlerSide(i) == 0
         && GetPartyIdFromBattleSlot(partySelection) == gBattlerPartyIndexes[i])
        {
            PartyMenuEraseMsgBoxAndFrame();
            GetMonNickname(&gPlayerParty[partySelection], gStringVar1);
            StringExpandPlaceholders(gStringVar4, gOtherText_AlreadyBattle);
            DisplayPartyMenuMessage(gStringVar4, 0);
            gTasks[taskId].func = Task_WaitBattlePartyErrorMessage;
            return;
        }
    }
    if (GetMonData(&gPlayerParty[partySelection], MON_DATA_IS_EGG))
    {
        PartyMenuEraseMsgBoxAndFrame();
        StringExpandPlaceholders(gStringVar4, gOtherText_EGGCantBattle);
        DisplayPartyMenuMessage(gStringVar4, 0);
        gTasks[taskId].func = Task_WaitBattlePartyErrorMessage;
        return;
    }
    if (GetPartyIdFromBattleSlot(partySelection) == gBattleStruct->unk1609D)
    {
        PartyMenuEraseMsgBoxAndFrame();
        GetMonNickname(&gPlayerParty[partySelection], gStringVar1);
        StringExpandPlaceholders(gStringVar4, gOtherText_AlreadySelected);
        DisplayPartyMenuMessage(gStringVar4, 0);
        gTasks[taskId].func = Task_WaitBattlePartyErrorMessage;
        return;
    }
    if (gBattlePartyMenuAction == 4)
    {
        PartyMenuEraseMsgBoxAndFrame();
        SetMonPreventsSwitchingString();
        DisplayPartyMenuMessage(gStringVar4, 0);
        gTasks[taskId].func = Task_WaitBattlePartyErrorMessage;
        return;
    }
    if (gBattlePartyMenuAction == 2)
    {
        u8 r0;
        u8 r4 = gBattlerInMenuId;

        PartyMenuEraseMsgBoxAndFrame();
        r0 = GetPartyIdFromBattlePartyId(gBattlerPartyIndexes[r4]);
        GetMonNickname(&gPlayerParty[r0], gStringVar1);
        StringExpandPlaceholders(gStringVar4, gOtherText_CantBeSwitched);
        DisplayPartyMenuMessage(gStringVar4, 0);
        gTasks[taskId].func = Task_WaitBattlePartyErrorMessage;
        return;
    }
    gUnknown_0202E8F5 = GetPartyIdFromBattleSlot(partySelection);
    gUnknown_0202E8F4 = 1;
    r4 = GetPartyIdFromBattlePartyId(gBattlerPartyIndexes[gBattlerInMenuId]);
    SwitchPartyMonSlots(r4, partySelection);
    SwapPokemon(&gPlayerParty[r4], &gPlayerParty[partySelection]);
    gTasks[taskId].func = Task_BeginExitBattlePartyMenu;
}

static void Task_BattlePartyMenuCancel(u8 taskId)
{
    Menu_DestroyCursor();
    ClosePartyPopupMenu(gTasks[taskId].data[4], sBattlePartyPopupMenus);
    gTasks[taskId].data[4] = gTasks[taskId].data[5];
    PrintPartyMenuPromptText(0, 0);
    SwitchTaskToFollowupFunc(taskId);
}
