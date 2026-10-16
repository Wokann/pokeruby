#include "global.h"
#include "choose_party.h"
#include "contest.h"
#include "data.h"
#include "event_data.h"
#include "party_menu.h"
#include "field_screen_effect.h"
#include "palette.h"
#include "pokemon.h"
#include "pokemon_summary_screen.h"
#include "overworld.h"
#include "script.h"
#include "script_pokemon_80F9.h"
#include "constants/songs.h"
#include "sound.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "ewram.h"


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
    gTasks[taskId].data[0] = PARTY_MENU_TYPE_CHOOSE_MON;
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
    switch (ePartyMenu2.pmSetupState)
    {
    case 0:
        if (ePartyMenu2.pmMonIndex < gPlayerPartyCount)
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
        if (sub_806BD58(ePartyMenu2.menuHandlerTaskId, ePartyMenu2.pmMonIndex) != 1)
        {
            ePartyMenu2.pmMonIndex++;
            break;
        }
        else
        {
            ePartyMenu2.pmMonIndex = 0;
            ePartyMenu2.pmSetupState++;
            break;
        }
    case 4:
        PartyMenuPrintMonsLevelOrStatus();
        ePartyMenu2.pmSetupState++;
        break;
    case 5:
        PrintPartyMenuMonNicknames();
        ePartyMenu2.pmSetupState++;
        break;
    case 6:
        sub_80F9C00();
        ePartyMenu2.pmSetupState++;
        break;
    case 7: // the only case that can return true.
        if (DrawPartyMonBackground(ePartyMenu2.pmMonIndex) != 1)
        {
            ePartyMenu2.pmMonIndex++;
            break;
        }
        else
        {
            ePartyMenu2.pmMonIndex = 0;
            ePartyMenu2.pmSetupState = 0;
            return TRUE;
        }
    }
    return FALSE;
}

void sub_80F9C00(void)
{
    u8 slot;

    for (slot = 0; slot < gPlayerPartyCount; slot++)
    {
        switch (GetContestEntryEligibility(&gPlayerParty[slot]))
        {
        case 0:
        case 3:
        case 4:
            DrawMonDescriptorStatus(slot, 0x7E);
            break;
        case 1:
        case 2:
            DrawMonDescriptorStatus(slot, 0x70);
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
            sub_8123138(taskId);
            break;
        case B_BUTTON:
            PlaySE(SE_SELECT);
            gContestMonPartyIndex = 0xFF;
            gSpecialVar_0x8004 = 0xFF;
            sub_8123138(taskId);
            break;
        }
    }
}

bool8 SetupMoveTutorPartyMenu(void)
{
    switch (ePartyMenu2.pmSetupState)
    {
    case 0:
        if (ePartyMenu2.pmMonIndex < gPlayerPartyCount)
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
        if (sub_806BD58(ePartyMenu2.menuHandlerTaskId, ePartyMenu2.pmMonIndex) != 1)
        {
            ePartyMenu2.pmMonIndex++;
        }
        else
        {
            ePartyMenu2.pmMonIndex = 0;
            ePartyMenu2.pmSetupState++;
        }
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
        sub_80F9E1C();
        ePartyMenu2.pmSetupState++;
        break;
    case 7: // the only case that can return true.
        if (DrawPartyMonBackground(ePartyMenu2.pmMonIndex) != 1)
        {
            ePartyMenu2.pmMonIndex++;
        }
        else
        {
            ePartyMenu2.pmMonIndex = 0;
            ePartyMenu2.pmSetupState = 0;
            return TRUE;
        }
    }

    return FALSE;
}

void sub_80F9E1C(void)
{
    u8 slot;

    for (slot = 0; slot < gPlayerPartyCount; slot++)
    {
        if (!GetNumberOfRelearnableMoves(&gPlayerParty[slot]))
            DrawMonDescriptorStatus(slot, 0x9A);
        else
            DrawMonDescriptorStatus(slot, 0x8C);
    }
}

void HandleMoveTutorPartyMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        switch (HandleDefaultPartyMenuInput(taskId))
        {
        case A_BUTTON:
            PlaySE(SE_SELECT);
            gSpecialVar_0x8004 = GetCursorSelectionMonId(taskId);
            gSpecialVar_0x8005 = GetNumberOfRelearnableMoves(&gPlayerParty[gSpecialVar_0x8004]);
            sub_8123138(taskId);
            break;
        case B_BUTTON:
            PlaySE(SE_SELECT);
            gSpecialVar_0x8004 = 0xFF;
            sub_8123138(taskId);
        }
    }
}

void MoveDeleterChooseMoveToForget(void)
{
    ShowSelectMovePokemonSummaryScreen(&gPlayerParty[0], gSpecialVar_0x8004, gPlayerPartyCount - 1, CB2_ReturnToField, 0);
    pssData.mode = PSS_MODE_MOVE_DELETER;
    gFieldCallback = FieldCB_ContinueScriptHandleMusic;
}

void GetNumMovesSelectedMonHas(void) // count pokemon moves
{
    u8 i;

    gSpecialVar_Result = 0;

    for (i = 0; i < 4; i++) // checks MOVE1-MOVE4
        if (GetMonData(&gPlayerParty[gSpecialVar_0x8004], MON_DATA_MOVE1 + i))
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

    u8 ppBonusMove1 = (ppBonuses & gPPUpGetMask[slotTo]) >> (slotTo * 2);
    u8 ppBonusMove2 = (ppBonuses & gPPUpGetMask[slotFrom]) >> (slotFrom * 2);

    ppBonuses &= ~gPPUpGetMask[slotTo];
    ppBonuses &= ~gPPUpGetMask[slotFrom];
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

    SetMonMoveSlot(&gPlayerParty[gSpecialVar_0x8004], 0, gSpecialVar_0x8005);
    RemoveMonPPBonus(&gPlayerParty[gSpecialVar_0x8004], gSpecialVar_0x8005);

    for (i = gSpecialVar_0x8005; i < 3; i++)
        ShiftMoveSlot(&gPlayerParty[gSpecialVar_0x8004], i, i + 1);
}

void IsSelectedMonEgg(void)
{
    struct Pokemon *party = &gPlayerParty[gSpecialVar_0x8004];
    gSpecialVar_Result = 0;

    if (GetMonData(party, MON_DATA_IS_EGG))
        gSpecialVar_Result = 1;
}
