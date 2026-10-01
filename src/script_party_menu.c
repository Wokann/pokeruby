#include "global.h"
#include "battle_party_menu.h"
#include "choose_party.h"
#include "contest.h"
#include "data2.h"
#include "party_menu.h"
#include "field_fadetransition.h"
#include "palette.h"
#include "pokemon.h"
#include "pokemon_summary_screen.h"
#include "overworld.h"
#include "script.h"
#include "script_party_menu.h"
#include "constants/moves.h"
#include "constants/party_menu.h"
#include "constants/songs.h"
#include "sound.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "ewram.h"

extern u8 gPlayerPartyCount;
extern u16 gSpecialVar_0x8004;
extern u16 gSpecialVar_0x8005;
extern u8 gContestMonPartyIndex;
extern u16 gSpecialVar_Result;

extern void (*gFieldCallback)(void);

void ChooseContestMon(void)
{
    u8 taskId;

    LockPlayerFieldControls();
    taskId = CreateTask((void *)OpenPartyMenuFromScriptContext, 0xA);
    gTasks[taskId].data[0] = PARTY_MENU_TYPE_CONTEST;
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
}

void ChoosePartyMon(void)
{
    u8 taskId;

    LockPlayerFieldControls();
    taskId = CreateTask((void *)OpenPartyMenuFromScriptContext, 0xA);
    gTasks[taskId].data[0] = PARTY_MENU_TYPE_IN_GAME_TRADE;
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
}

void ChooseMonForMoveRelearner(void)
{
    u8 taskId;

    LockPlayerFieldControls();
    taskId = CreateTask((void *)OpenPartyMenuFromScriptContext, 0xA);
    gTasks[taskId].data[0] = PARTY_MENU_TYPE_MOVE_RELEARNER;
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
}

void OpenPartyMenuFromScriptContext(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        gPaletteFade.bufferTransferDisabled = 1;
        OpenPartyMenu((u8) gTasks[taskId].data[0], 0);
        DestroyTask(taskId);
    }
}

bool8 SetupContestPartyMenu(void)
{
    switch (gPartyMenu.pmSetupState)
    {
    case 0:
        if (gPartyMenu.pmMonIndex < gPlayerPartyCount)
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
        break;
    case 1:
        LoadHeldItemIconGraphics();
        gPartyMenu.pmSetupState++;
        break;
    case 2:
        CreatePartyMenuHeldItemIcons(gPartyMenu.menuHandlerTaskId);
        gPartyMenu.pmSetupState++;
        break;
    case 3:
        if (CreatePartyMenuCursor(gPartyMenu.menuHandlerTaskId, gPartyMenu.pmMonIndex) != 1)
        {
            gPartyMenu.pmMonIndex++;
            break;
        }
        else
        {
            gPartyMenu.pmMonIndex = 0;
            gPartyMenu.pmSetupState++;
            break;
        }
    case 4:
        PartyMenuPrintMonsLevelOrStatus();
        gPartyMenu.pmSetupState++;
        break;
    case 5:
        PrintPartyMenuMonNicknames();
        gPartyMenu.pmSetupState++;
        break;
    case 6:
        DrawContestPartyMonStatus();
        gPartyMenu.pmSetupState++;
        break;
    case 7: // the only case that can return true.
        if (DrawPartyMonBackground(gPartyMenu.pmMonIndex) != 1)
        {
            gPartyMenu.pmMonIndex++;
            break;
        }
        else
        {
            gPartyMenu.pmMonIndex = 0;
            gPartyMenu.pmSetupState = 0;
            return TRUE;
        }
    }
    return FALSE;
}

void DrawContestPartyMonStatus(void)
{
    u8 i;

    for (i = 0; i < gPlayerPartyCount; i++)
    {
        switch (GetContestEntryEligibility(&gPlayerParty[i]))
        {
        case 0:
        case 3:
        case 4:
            DrawMonDescriptorStatus(i, 0x7E);
            break;
        case 1:
        case 2:
            DrawMonDescriptorStatus(i, 0x70);
            break;
        }
    }
}

void HandleSelectPartyMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        switch (HandleDefaultPartyMenuInput(taskId))
        {
        case A_BUTTON:
            PlaySE(SE_SELECT);
            gContestMonPartyIndex = GetCursorSelectionMonId(taskId);
            gSpecialVar_0x8004 = gContestMonPartyIndex;
            StartPartyMenuExitToField(taskId);
            break;
        case B_BUTTON:
            PlaySE(SE_SELECT);
            gContestMonPartyIndex = PARTY_NOTHING_CHOSEN;
            gSpecialVar_0x8004 = PARTY_NOTHING_CHOSEN;
            StartPartyMenuExitToField(taskId);
            break;
        }
    }
}

bool8 SetupMoveRelearnerPartyMenu(void)
{
    switch (gPartyMenu.pmSetupState)
    {
    case 0:
        if (gPartyMenu.pmMonIndex < gPlayerPartyCount)
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
        break;
    case 1:
        LoadHeldItemIconGraphics();
        gPartyMenu.pmSetupState++;
        break;
    case 2:
        CreatePartyMenuHeldItemIcons(gPartyMenu.menuHandlerTaskId);
        gPartyMenu.pmSetupState++;
        break;
    case 3:
        if (CreatePartyMenuCursor(gPartyMenu.menuHandlerTaskId, gPartyMenu.pmMonIndex) != 1)
        {
            gPartyMenu.pmMonIndex++;
        }
        else
        {
            gPartyMenu.pmMonIndex = 0;
            gPartyMenu.pmSetupState++;
        }
        break;
    case 4:
        PartyMenuPrintMonsLevelOrStatus();
        gPartyMenu.pmSetupState++;
        break;
    case 5:
        PrintPartyMenuMonNicknames();
        gPartyMenu.pmSetupState++;
        break;
    case 6:
        DrawMoveRelearnerPartyMonStatus();
        gPartyMenu.pmSetupState++;
        break;
    case 7: // the only case that can return true.
        if (DrawPartyMonBackground(gPartyMenu.pmMonIndex) != 1)
        {
            gPartyMenu.pmMonIndex++;
        }
        else
        {
            gPartyMenu.pmMonIndex = 0;
            gPartyMenu.pmSetupState = 0;
            return TRUE;
        }
    }

    return FALSE;
}

void DrawMoveRelearnerPartyMonStatus(void)
{
    u8 i;

    for (i = 0; i < gPlayerPartyCount; i++)
    {
        if (!GetNumberOfRelearnableMoves(&gPlayerParty[i]))
            DrawMonDescriptorStatus(i, 0x9A);
        else
            DrawMonDescriptorStatus(i, 0x8C);
    }
}

void HandleMoveRelearnerPartyMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        switch (HandleDefaultPartyMenuInput(taskId))
        {
        case A_BUTTON:
            PlaySE(SE_SELECT);
            gSpecialVar_0x8004 = GetCursorSelectionMonId(taskId);
            gSpecialVar_0x8005 = GetNumberOfRelearnableMoves(&gPlayerParty[gSpecialVar_0x8004]);
            StartPartyMenuExitToField(taskId);
            break;
        case B_BUTTON:
            PlaySE(SE_SELECT);
            gSpecialVar_0x8004 = PARTY_NOTHING_CHOSEN;
            StartPartyMenuExitToField(taskId);
        }
    }
}

void MoveDeleterChooseMoveToForget(void)
{
    ShowSelectMovePokemonSummaryScreen(&gPlayerParty[0], gSpecialVar_0x8004, gPlayerPartyCount - 1, CB2_ReturnToField, 0);
    sMonSummaryScreen->mode = PSS_MODE_MOVE_DELETER;
    gFieldCallback = FieldCB_ContinueScriptHandleMusic;
}

void GetNumMovesSelectedMonHas(void)
{
    u8 i;

    gSpecialVar_Result = 0;

    for (i = 0; i < MAX_MON_MOVES; i++)
        if (GetMonData(&gPlayerParty[gSpecialVar_0x8004], MON_DATA_MOVE1 + i) != MOVE_NONE)
            gSpecialVar_Result++;
}

void BufferMoveDeleterNicknameAndMove(void)
{
    struct Pokemon *mon = &gPlayerParty[gSpecialVar_0x8004];
    u16 move = GetMonData(mon, MON_DATA_MOVE1 + gSpecialVar_0x8005);

    GetMonNickname(mon, gStringVar1);
    StringCopy(gStringVar2, gMoveNames[move]);
}

static void ShiftMoveSlot(struct Pokemon *mon, u8 slotTo, u8 slotFrom)
{
    u16 move1 = GetMonData(mon, MON_DATA_MOVE1 + slotTo);
    u16 move0 = GetMonData(mon, MON_DATA_MOVE1 + slotFrom);
    u8 pp1 = GetMonData(mon, MON_DATA_PP1 + slotTo);
    u8 pp0 = GetMonData(mon, MON_DATA_PP1 + slotFrom);
    u8 ppBonuses = GetMonData(mon, MON_DATA_PP_BONUSES);
    u8 ppBonusMask1 = gPPUpGetMask[slotTo];
    u8 ppBonusMove1 = (ppBonuses & ppBonusMask1) >> (slotTo * 2);
    u8 ppBonusMask2 = gPPUpGetMask[slotFrom];
    u8 ppBonusMove2 = (ppBonuses & ppBonusMask2) >> (slotFrom * 2);

    ppBonuses &= ~ppBonusMask1;
    ppBonuses &= ~ppBonusMask2;
    ppBonuses |= (ppBonusMove1 << (slotFrom * 2)) + (ppBonusMove2 << (slotTo * 2));
    SetMonData(mon, MON_DATA_MOVE1 + slotTo, &move0);
    SetMonData(mon, MON_DATA_MOVE1 + slotFrom, &move1);
    SetMonData(mon, MON_DATA_PP1 + slotTo, &pp0);
    SetMonData(mon, MON_DATA_PP1 + slotFrom, &pp1);
    SetMonData(mon, MON_DATA_PP_BONUSES, &ppBonuses);
}

void MoveDeleterForgetMove(void)
{
    u16 i;

    SetMonMoveSlot(&gPlayerParty[gSpecialVar_0x8004], MOVE_NONE, gSpecialVar_0x8005);
    RemoveMonPPBonus(&gPlayerParty[gSpecialVar_0x8004], gSpecialVar_0x8005);

    for (i = gSpecialVar_0x8005; i < MAX_MON_MOVES - 1; i++)
        ShiftMoveSlot(&gPlayerParty[gSpecialVar_0x8004], i, i + 1);
}

void IsSelectedMonEgg(void)
{
    struct Pokemon *mon = &gPlayerParty[gSpecialVar_0x8004];

    gSpecialVar_Result = FALSE;

    if (GetMonData(mon, MON_DATA_IS_EGG))
        gSpecialVar_Result = TRUE;
}
