#include "global.h"
#include "event_data.h"
#include "field_fadetransition.h"
#include "main.h"
#include "menu.h"
#include "international_string_util.h"
#include "palette.h"
#include "party_menu.h"
#include "pokemon_menu.h"
#include "field_weather.h"
#include "pokemon.h"
#include "pokemon_summary_screen.h"
#include "overworld.h"
#include "script.h"
#include "constants/songs.h"
#include "sound.h"
#include "strings.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "ewram.h"

extern u8 gPlayerPartyCount;
extern u8 gLastFieldPokeMenuOpened;
extern struct MultiBattlePokemonTx gMultiPartnerParty[3];
extern u8 gPartyMenuMessage_IsPrinting;
extern const u16 gBattleTowerBannedSpecies[];

EWRAM_DATA u8 gSelectedOrderFromParty[3] = {0};

extern u8 CreatePartyMenuCursor(u8, u8);
extern void PartyMenuPrintMonsLevelOrStatus(void);
extern void DrawMonDescriptorStatus(u8, u8);
extern void PartyMenuPrintHP();
extern bool8 MenuHelpers_IsLinkActive(void);

static void ClearSelectedPartyOrder(void);
static bool8 IsMonAllowedInBattleTower(struct Pokemon *);
static void Task_HandleBattleTowerEntryPopup(u8);
static void Task_CloseChoosePartyMenu(u8);
static void Task_ValidateBattleTowerParty(u8);
static void Task_WaitBattleTowerPartyFullMessage(u8);
static void BattleTowerEntryMenuCallback_Exit(u8);
static void CreateLinkMultiBattlePartyMonIcons(u8);
static void CreateLinkMultiBattleHeldItemIcons(u8);
static void PrintLinkMultiBattlePlayerPartyInfo(void);
static void Task_WaitLinkMultiPartnerIcons(u8);
static void Task_PrintLinkMultiPartnerPartyInfo(u8);
static void Task_DelayLinkMultiPartyMenuExit(u8);
static void Task_HandleDaycareStoragePopup(u8);
void StartPartyMenuExitToField(u8);
static void Task_WaitPartyMenuExitFade(u8);
static void FieldCallback_FadeFromPartyMenu(void);

void InitChooseHalfPartyForBattle(void)
{
    ClearSelectedPartyOrder();
    gPartyMenu.battleTowerSelectionMode = 0;
    OpenPartyMenu(PARTY_MENU_TYPE_BATTLE_TOWER, 0);
}

void InitChooseBattleTowerParty(void)
{
    ClearSelectedPartyOrder();
    gPartyMenu.battleTowerSelectionMode = 1;
    OpenPartyMenu(PARTY_MENU_TYPE_BATTLE_TOWER, 0);
}

static void ClearSelectedPartyOrder(void)
{
    u8 i;

    for (i = 0; i < 3; i++)
        gSelectedOrderFromParty[i] = 0;
}

bool8 SetupBattleTowerPartyMenu(void)
{
    u8 i;

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
        if (CreatePartyMenuCursor(gPartyMenu.menuHandlerTaskId, gPartyMenu.pmMonIndex) == 1)
        {
            gPartyMenu.pmMonIndex = 0;
            gPartyMenu.pmSetupState++;
        }
        else
        {
            gPartyMenu.pmMonIndex++;
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
        for (i = 0; i < gPlayerPartyCount; i++)
        {
            u8 j;

            for (j = 0; j < 3; j++)
            {
                if (gSelectedOrderFromParty[j] == i + 1)
                {
                    DrawMonDescriptorStatus(i, j * 14 + 0x1C);
                    break;
                }
            }
            if (j == 3)
            {
                if (IsMonAllowedInBattleTower(&gPlayerParty[i]) == TRUE)
                    DrawMonDescriptorStatus(i, 0x70);
                else
                    DrawMonDescriptorStatus(i, 0x7E);
            }
        }
        gPartyMenu.pmSetupState++;
        break;
    case 7:
        if (DrawPartyMonBackground(gPartyMenu.pmMonIndex) == 1)
        {
            gPartyMenu.pmMonIndex = 0;
            gPartyMenu.pmSetupState = 0;
            return TRUE;
        }
        else
        {
            gPartyMenu.pmMonIndex++;
        }
        break;
    }
    return FALSE;
}

static bool8 IsMonAllowedInBattleTower(struct Pokemon *pkmn)
{
    u16 species;
    s32 i = 0;

    if (GetMonData(pkmn, MON_DATA_IS_EGG))
        return FALSE;

    if (gPartyMenu.battleTowerSelectionMode == 0)
    {
        if (GetMonData(pkmn, MON_DATA_HP) == 0)
            return FALSE;
        else
            return TRUE;
    }

    if ((gSaveBlock2.battleTower.battleTowerLevelType) == 0
     && GetMonData(pkmn, MON_DATA_LEVEL) > 50)
        return FALSE;

    // Check if the pkmn is in the ban list
    species = GetMonData(pkmn, MON_DATA_SPECIES);
    while (gBattleTowerBannedSpecies[i] != 0xFFFF)
    {
        if (gBattleTowerBannedSpecies[i] == species)
            return FALSE;
        i++;
    }
    return TRUE;
}

static u8 CheckBattleTowerEntriesAndGetMessage(void)
{
    u8 i;

    if (gPartyMenu.battleTowerSelectionMode == 0)
        return 0xFF;
    if (gSelectedOrderFromParty[2] == 0)
        return 0x11;
    for (i = 0; i < 2; i++)
    {
        u8 j;

        gPartyMenu.pmBattleTowerSpecies = GetMonData(&gPlayerParty[gSelectedOrderFromParty[i] - 1], MON_DATA_SPECIES);
        gPartyMenu.pmBattleTowerHeldItem = GetMonData(&gPlayerParty[gSelectedOrderFromParty[i] - 1], MON_DATA_HELD_ITEM);
        for (j = i + 1; j < 3; j++)
        {
            if (gPartyMenu.pmBattleTowerSpecies == GetMonData(&gPlayerParty[gSelectedOrderFromParty[j] - 1], MON_DATA_SPECIES))
                return 0x12;
            if (gPartyMenu.pmBattleTowerHeldItem != 0 &&
                gPartyMenu.pmBattleTowerHeldItem == GetMonData(&gPlayerParty[gSelectedOrderFromParty[j] - 1], MON_DATA_HELD_ITEM))
                return 0x13;
        }
    }
    return 0xFF;
}

//------------------------------------------------------------------------------
// Battle Tower Entry Menu
//------------------------------------------------------------------------------

static void BattleTowerEntryMenuCallback_Summary(u8);
static void BattleTowerEntryMenuCallback_Enter(u8);
static void BattleTowerEntryMenuCallback_NoEntry(u8);
static void BattleTowerEntryMenuCallback_Exit(u8);

static const struct MenuAction2 sBattleTowerEntryMenuItems[] =
{
    {gText_Summary5, BattleTowerEntryMenuCallback_Summary},
    {gText_Enter, BattleTowerEntryMenuCallback_Enter},
    {gText_NoEntry, BattleTowerEntryMenuCallback_NoEntry},
    {gText_Exit, BattleTowerEntryMenuCallback_Exit},
};

static const u8 sBattleTowerEntryActions_Eligible[] = {1, 0, 3};
static const u8 sBattleTowerEntryActions_Selected[] = {2, 0, 3};
static const u8 sBattleTowerEntryActions_Ineligible[] = {0, 3};

static const struct PartyPopupMenu sBattleTowerEntryMenu[] =
{
    {ARRAY_COUNT(sBattleTowerEntryActions_Eligible), 9, sBattleTowerEntryActions_Eligible},
    {ARRAY_COUNT(sBattleTowerEntryActions_Selected), 9, sBattleTowerEntryActions_Selected},
    {ARRAY_COUNT(sBattleTowerEntryActions_Ineligible), 9, sBattleTowerEntryActions_Ineligible},
};


static bool8 HasPartySlotAlreadyBeenSelected(u8 partyMember)
{
    u8 i;

    for (i = 0; i < 3; i++)
    {
        if (gSelectedOrderFromParty[i] == partyMember)
            return TRUE;
    }
    return FALSE;
}

static void ShowBattleTowerEntryPopup(u8 taskId)
{
    PrintPartyMenuPromptText(PARTY_MSG_DO_WHAT_WITH_MON, PARTY_MENU_PROMPT_WINDOW_NARROW);
    if (IsMonAllowedInBattleTower(&gPlayerParty[gLastFieldPokeMenuOpened]) == TRUE)
    {
        if (HasPartySlotAlreadyBeenSelected(gLastFieldPokeMenuOpened + 1) == TRUE)
        {
            gTasks[taskId].data[4] = 1;
            ShowPartyPopupMenu(1, sBattleTowerEntryMenu, sBattleTowerEntryMenuItems, 0);
        }
        else
        {
            gTasks[taskId].data[4] = 0;
            ShowPartyPopupMenu(0, sBattleTowerEntryMenu, sBattleTowerEntryMenuItems, 0);
        }
    }
    else
    {
        gTasks[taskId].data[4] = 2;
        ShowPartyPopupMenu(2, sBattleTowerEntryMenu, sBattleTowerEntryMenuItems, 0);
    }
}

void HandleBattleTowerPartyMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        switch (HandleBattleTowerPartyMenuInput(taskId))
        {
        case A_BUTTON:
            PlaySE(SE_SELECT);
            gLastFieldPokeMenuOpened = GetCursorSelectionMonId(taskId);
            if (gLastFieldPokeMenuOpened != 6)
            {
                GetMonNickname(&gPlayerParty[gLastFieldPokeMenuOpened], gStringVar1);
                ShowBattleTowerEntryPopup(taskId);
                gTasks[taskId].func = Task_HandleBattleTowerEntryPopup;
            }
            else
            {
                gTasks[taskId].func = Task_ValidateBattleTowerParty;
            }
            SetTaskToWaitForLinkRecv(taskId);
            break;
        case B_BUTTON:
            PlaySE(SE_SELECT);
            ClearSelectedPartyOrder();
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            gTasks[taskId].func = Task_CloseChoosePartyMenu;
            break;
        }
    }
}

// Handle input
static void Task_HandleBattleTowerEntryPopup(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        if (JOY_REPT(0x40))
        {
            if (Menu_GetCursorPos() != 0)
            {
                PlaySE(SE_SELECT);
                Menu_MoveCursor(-1);
            }
            return;
        }
        if (JOY_REPT(0x80))
        {
            if (Menu_GetCursorPos() != 3)
            {
                PlaySE(SE_SELECT);
                Menu_MoveCursor(1);
            }
            return;
        }
        if (JOY_NEW(A_BUTTON))
        {
            TaskFunc popupMenuFunc;

            PlaySE(SE_SELECT);
            popupMenuFunc = PartyMenuGetPopupMenuFunc(
              gTasks[taskId].data[4],
              sBattleTowerEntryMenu,
              sBattleTowerEntryMenuItems,
              Menu_GetCursorPos());
            popupMenuFunc(taskId);
            return;
        }
        if (JOY_NEW(B_BUTTON))
        {
            BattleTowerEntryMenuCallback_Exit(taskId);
            return;
        }
    }
}

// Return from menu?
static void Task_CloseChoosePartyMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(gMain.savedCallback);
        DestroyTask(taskId);
    }
}

// Wait for A or B press
static void Task_WaitBattleTowerPromptInput(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
        BattleTowerEntryMenuCallback_Exit(taskId);
}

static void Task_ValidateBattleTowerParty(u8 taskId)
{
    u8 val = CheckBattleTowerEntriesAndGetMessage();

    if (val != 0xFF)
    {
        PrintPartyMenuPromptText(val, 0);
        gTasks[taskId].func = Task_WaitBattleTowerPromptInput;
    }
    else
    {
        if (gSelectedOrderFromParty[0] != 0)
        {
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            gTasks[taskId].func = Task_CloseChoosePartyMenu;
        }
        else
        {
            PlaySE(SE_FAILURE);
            PrintPartyMenuPromptText(PARTY_MSG_NO_MON_FOR_BATTLE, PARTY_MENU_PROMPT_WINDOW_STANDARD);
            gTasks[taskId].func = Task_WaitBattleTowerPromptInput;
        }
    }
}

// CB2 for menu?
static void CB2_ReturnToBattleTowerPartyMenu(void)
{
    while (1)
    {
        if (InitPartyMenu() == TRUE)
        {
            SetPartyMenuCursorSelection(gPartyMenu.menuHandlerTaskId, gLastViewedMonIndex);
            ChangeBattleTowerPartyMenuSelection(gPartyMenu.menuHandlerTaskId, 0);
            GetMonNickname(&gPlayerParty[gLastViewedMonIndex], gStringVar1);
            gLastFieldPokeMenuOpened = gLastViewedMonIndex;
            ShowBattleTowerEntryPopup(gPartyMenu.menuHandlerTaskId);
            SetMainCallback2(CB2_PartyMenuMain);
            break;
        }
        if (MenuHelpers_IsLinkActive() == 1)
            break;
    }
}

static void CB2_InitBattleTowerSummaryReturn(void)
{
    gPaletteFade.bufferTransferDisabled = TRUE;
    SetPartyMenuSettings(PARTY_MENU_TYPE_BATTLE_TOWER, 0xFF, Task_HandleBattleTowerEntryPopup, 5);
    SetMainCallback2(CB2_ReturnToBattleTowerPartyMenu);
}

// Wait for fade, then show summary screen
static void Task_OpenBattleTowerSummary(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        u8 summaryMonIndex = gSprites[gTasks[taskId].data[3] >> 8].data[0];

        DestroyTask(taskId);
        gPartyMenu.unusedResumeAction = 1;
        ShowPokemonSummaryScreen(gPlayerParty, summaryMonIndex, gPlayerPartyCount - 1, CB2_InitBattleTowerSummaryReturn, SUMMARY_MODE_NORMAL);
    }
}

// Summary callback?
static void BattleTowerEntryMenuCallback_Summary(u8 taskId)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = Task_OpenBattleTowerSummary;
}

static void BattleTowerEntryMenuCallback_Enter(u8 taskId)
{
    u8 i;

    for (i = 0; i < 3; i++)
    {
        if (gSelectedOrderFromParty[i] == 0)
        {
            gSelectedOrderFromParty[i] = gLastFieldPokeMenuOpened + 1;
            DrawMonDescriptorStatus(gLastFieldPokeMenuOpened, i * 14 + 0x1C);
            if (i == 2)
                SelectBattleTowerOKButton(taskId);
            BattleTowerEntryMenuCallback_Exit(taskId);
            return;
        }
    }
    PlaySE(SE_FAILURE);
    Menu_EraseWindowRect(20, 10, 29, 19);
    Menu_DestroyCursor();
    PartyMenuEraseMsgBoxAndFrame();
    DisplayPartyMenuMessage(gOtherText_NoMoreThreePoke, 1);
    gTasks[taskId].func = Task_WaitBattleTowerPartyFullMessage;
}

static void Task_WaitBattleTowerPartyFullMessage(u8 taskId)
{
    if (gPartyMenuMessage_IsPrinting == 1)
        return;

    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        Menu_EraseWindowRect(0, 14, 29, 19);
        Menu_DestroyCursor();
        BattleTowerEntryMenuCallback_Exit(taskId);
    }
}

static void BattleTowerEntryMenuCallback_NoEntry(u8 taskId)
{
    u8 i;

    for (i = 0; i < 3; i++)
    {
        if (gSelectedOrderFromParty[i] == gLastFieldPokeMenuOpened + 1)
        {
            gSelectedOrderFromParty[i] = 0;
            switch (i)
            {
            case 0:
                gSelectedOrderFromParty[0] = gSelectedOrderFromParty[1];
                gSelectedOrderFromParty[1] = gSelectedOrderFromParty[2];
                gSelectedOrderFromParty[2] = 0;
                break;
            case 1:
                gSelectedOrderFromParty[1] = gSelectedOrderFromParty[2];
                gSelectedOrderFromParty[2] = 0;
                break;
            }
            break;  // exit loop
        }
    }
    DrawMonDescriptorStatus(gLastFieldPokeMenuOpened, 0x70);
    if (gSelectedOrderFromParty[0] != 0)
        DrawMonDescriptorStatus(gSelectedOrderFromParty[0] - 1, 0x1C);
    if (gSelectedOrderFromParty[1] != 0)
        DrawMonDescriptorStatus(gSelectedOrderFromParty[1] - 1, 0x2A);
    BattleTowerEntryMenuCallback_Exit(taskId);
}

static void Task_CloseBattleTowerEntryPopup(u8 taskId)
{
    Menu_EraseWindowRect(20, 10, 29, 19);
    Menu_DestroyCursor();
    PrintPartyMenuPromptText(PARTY_MSG_CHOOSE_MON, PARTY_MENU_PROMPT_WINDOW_STANDARD);
    gTasks[taskId].func = HandleBattleTowerPartyMenu;
}

static void BattleTowerEntryMenuCallback_Exit(u8 taskId)
{
    PlaySE(SE_SELECT);
    Task_CloseBattleTowerEntryPopup(taskId);
}

#if DEBUG

void Debug_CopyLastThreePartyMonsToMultiPartnerParty(void)
{
    u8 i;
    
    memset(gMultiPartnerParty, 0, sizeof(gMultiPartnerParty));
    for (i = 0; i < 3; i++)
    {
        gMultiPartnerParty[i].species = GetMonData(&gPlayerParty[3 + i], MON_DATA_SPECIES_OR_EGG);
        if (gMultiPartnerParty[i].species != 0)
        {
            gMultiPartnerParty[i].level = GetMonData(&gPlayerParty[3 + i], MON_DATA_LEVEL);
            gMultiPartnerParty[i].hp = GetMonData(&gPlayerParty[3 + i], MON_DATA_HP);
            gMultiPartnerParty[i].maxhp = GetMonData(&gPlayerParty[3 + i], MON_DATA_MAX_HP);
            gMultiPartnerParty[i].status = GetMonData(&gPlayerParty[3 + i], MON_DATA_STATUS);
            gMultiPartnerParty[i].heldItem = GetMonData(&gPlayerParty[3 + i], MON_DATA_HELD_ITEM);
            gMultiPartnerParty[i].personality = GetMonData(&gPlayerParty[3 + i], MON_DATA_PERSONALITY);
            gMultiPartnerParty[i].gender = GetMonGender(&gPlayerParty[3 + i]);
            GetMonData(&gPlayerParty[3 + i], MON_DATA_NICKNAME, gMultiPartnerParty[i].nickname);
            StripExtCtrlCodes(gMultiPartnerParty[i].nickname);
            gMultiPartnerParty[i].language = GetMonData(&gPlayerParty[3 + i], MON_DATA_LANGUAGE);
        }
    }
}

#endif

bool8 SetupLinkMultiBattlePartyMenu(void)
{
    switch (gPartyMenu.pmSetupState)
    {
    case 0:
        CreateLinkMultiBattlePartyMonIcons(gPartyMenu.menuHandlerTaskId);
        gPartyMenu.pmSetupState++;
        break;
    case 1:
        LoadHeldItemIconGraphics();
        gPartyMenu.pmSetupState++;
        break;
    case 2:
        CreateLinkMultiBattleHeldItemIcons(gPartyMenu.menuHandlerTaskId);
        gPartyMenu.pmSetupState++;
        break;
    case 3:
        PrintLinkMultiBattlePlayerPartyInfo();
        gPartyMenu.pmSetupState++;
        break;
    case 4:
        DrawLinkMultiBattlePartyMonBackgrounds();
        return TRUE;
    }
    return FALSE;
}

static void CreateLinkMultiBattlePartyMonIcons(u8 a)
{
    u8 i;

    for (i = 0; i < 3; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) != 0)
            CreatePartyMenuMonIcon(a, i, 3, &gPlayerParty[i]);
        if (gMultiPartnerParty[i].species != 0)
        {
            CreateMonIcon_LinkMultiBattle(a, i + 3, 3, &gMultiPartnerParty[i]);
            OffsetLinkMultiBattlePartnerMonIcon(a, i + 3);
        }
    }
}

static void CreateLinkMultiBattleHeldItemIcons(u8 a)
{
    u8 i;

    for (i = 0; i < 3; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) != 0)
        {
            u16 item = GetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM);

            CreatePartyMenuHeldItemIcon(a, i, item);
        }
        if (gMultiPartnerParty[i].species != 0)
            CreatePartyMenuHeldItemIcon(a, i + 3, gMultiPartnerParty[i].heldItem);
    }
}

static void PrintLinkMultiBattlePlayerPartyInfo(void)
{
    u8 i;

    for (i = 0; i < 3; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) != 0)
        {
            u8 ailment;

            PartyMenuPrintHP(i, 3, &gPlayerParty[i]);
            ailment = GetMonAilment(&gPlayerParty[i]);
            if (ailment && ailment != AILMENT_PKRS)
                PartyMenuPutStatusTilemap(i, 3, ailment - 1);
            else
                PartyMenuPrintLevel(i, 3, &gPlayerParty[i]);
            PartyMenuPrintGenderIcon(i, 3, &gPlayerParty[i]);
            PrintPartyMenuMonNickname(i, 3, &gPlayerParty[i]);
            PartyMenuDrawHPBar(i, 3, &gPlayerParty[i]);
        }
    }
}

void HandleLinkMultiBattlePartyMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        gTasks[taskId].data[0] = 30;
        StartLinkMultiBattlePartnerMonIconSlide(taskId, gMultiPartnerParty[0].species, 0);
        StartLinkMultiBattlePartnerMonIconSlide(taskId, gMultiPartnerParty[1].species, 1);
        StartLinkMultiBattlePartnerMonIconSlide(taskId, gMultiPartnerParty[2].species, 2);
        gTasks[taskId].func = Task_WaitLinkMultiPartnerIcons;
        gPartyMenu.backgroundRedrawState = 1;
    }
}

static void Task_WaitLinkMultiPartnerIcons(u8 taskId)
{
    UpdateLinkMultiBattlePartnerPartyBackgrounds(taskId, gMultiPartnerParty[1].species, gMultiPartnerParty[2].species);
    if (gTasks[taskId].data[0] == 0)
    {
        gTasks[taskId].func = Task_PrintLinkMultiPartnerPartyInfo;
        gPartyMenu.backgroundRedrawState = 2;
        PlaySE(SE_M_HARDEN);
    }
}

static void Task_PrintLinkMultiPartnerPartyInfo(u8 taskId)
{
    u8 i;

    for (i = 0; i < 3; i++)
    {
        if (gMultiPartnerParty[i].species != 0)
        {
            u8 ailment;

            PartyMenuDoPrintHP(i + 3, 3, gMultiPartnerParty[i].hp, gMultiPartnerParty[i].maxhp);
            if (gMultiPartnerParty[i].hp == 0)
                ailment = AILMENT_FNT;
            else
                ailment = GetAilmentFromStatus(gMultiPartnerParty[i].status);

            if (ailment != AILMENT_NONE)
                PartyMenuPutStatusTilemap(i + 3, 3, ailment - 1);
            else
                PartyMenuDoPrintLevel(i + 3, 3, gMultiPartnerParty[i].level);
            PartyMenuDoPrintGenderIcon(gMultiPartnerParty[i].species, gMultiPartnerParty[i].gender, 3, i + 3, gMultiPartnerParty[i].nickname);
            StringCopy(gStringVar1, gMultiPartnerParty[i].nickname);
            StringGet_Nickname(gStringVar1);
            ConvertInternationalPlayerName(gStringVar1);
            PartyMenuDoPrintMonNickname(i + 3, 3, gStringVar1);
            PartyMenuDoDrawHPBar(i + 3, 3, gMultiPartnerParty[i].hp, gMultiPartnerParty[i].maxhp);
        }
    }
    gTasks[taskId].func = Task_DelayLinkMultiPartyMenuExit;
    gTasks[taskId].data[0] = 0;
}

static void Task_DelayLinkMultiPartyMenuExit(u8 taskId)
{
    gTasks[taskId].data[0]++;
    if (gTasks[taskId].data[0] == 256)
    {
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        gTasks[taskId].func = Task_CloseChoosePartyMenu;
    }
}

// Exactly the same as SetupBattleTowerPartyMenu except for case 6
bool8 Unused_SetupBattleTowerPartyMenu(void)
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
        if (CreatePartyMenuCursor(gPartyMenu.menuHandlerTaskId, gPartyMenu.pmMonIndex) == 1)
        {
            gPartyMenu.pmMonIndex = 0;
            gPartyMenu.pmSetupState++;
        }
        else
        {
            gPartyMenu.pmMonIndex++;
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
        DrawPartyMonGenderDescriptorStatuses();
        gPartyMenu.pmSetupState++;
        break;
    case 7:
        if (DrawPartyMonBackground(gPartyMenu.pmMonIndex) == 1)
        {
            gPartyMenu.pmMonIndex = 0;
            gPartyMenu.pmSetupState = 0;
            return TRUE;
        }
        else
        {
            gPartyMenu.pmMonIndex++;
        }
        break;
    }
    return FALSE;
}

//------------------------------------------------------------------------------
// Daycare Pokemon Storage Menu
//------------------------------------------------------------------------------

static void DaycareStorageMenuCallback_Store(u8);
static void DaycareStorageMenuCallback_Summary(u8);
static void DaycareStorageMenuCallback_Exit(u8);

static const struct MenuAction2 sDaycareStorageMenuItems[] =
{
    {gText_Store, DaycareStorageMenuCallback_Store},
    {gText_Summary5, DaycareStorageMenuCallback_Summary},
    {gText_Exit, DaycareStorageMenuCallback_Exit},
};

static const u8 sDaycareStorageActions_NonEgg[] = {0, 1, 2};
static const u8 sDaycareStorageActions_Egg[] = {1, 2};

static const struct PartyPopupMenu sDaycareStorageMenus[] =
{
    {ARRAY_COUNT(sDaycareStorageActions_NonEgg), 9, sDaycareStorageActions_NonEgg},
    {ARRAY_COUNT(sDaycareStorageActions_Egg), 9, sDaycareStorageActions_Egg},
};

static void ShowDaycareStoragePopup(u8 taskId)
{
    if (!GetMonData(&gPlayerParty[gLastFieldPokeMenuOpened], MON_DATA_IS_EGG))
    {
        gTasks[taskId].data[4] = 0;
        ShowPartyPopupMenu(0, sDaycareStorageMenus, sDaycareStorageMenuItems, 0);
    }
    else
    {
        gTasks[taskId].data[4] = 1;
        ShowPartyPopupMenu(1, sDaycareStorageMenus, sDaycareStorageMenuItems, 0);
    }
}

void HandleDaycarePartyMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        switch (HandleDefaultPartyMenuInput(taskId))
        {
        case A_BUTTON:
            PlaySE(SE_SELECT);
            gLastFieldPokeMenuOpened = GetCursorSelectionMonId(taskId);
            GetMonNickname(&gPlayerParty[gLastFieldPokeMenuOpened], gStringVar1);
            ShowDaycareStoragePopup(taskId);
            gTasks[taskId].func = Task_HandleDaycareStoragePopup;
            break;
        case B_BUTTON:
            PlaySE(SE_SELECT);
            gLastFieldPokeMenuOpened = 0xFF;
            gSpecialVar_0x8004 = 0xFF;
            StartPartyMenuExitToField(taskId);
            break;
        }
    }
}

static void Task_HandleDaycareStoragePopup(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        if (JOY_REPT(0x40))
        {
            if (Menu_GetCursorPos() != 0)
            {
                PlaySE(SE_SELECT);
                Menu_MoveCursor(-1);
            }
            return;
        }
        if (JOY_REPT(0x80))
        {
            if (Menu_GetCursorPos() != 3)
            {
                PlaySE(SE_SELECT);
                Menu_MoveCursor(1);
            }
            return;
        }
        if (JOY_NEW(A_BUTTON))
        {
            TaskFunc popupMenuFunc;

            PlaySE(SE_SELECT);
            popupMenuFunc = PartyMenuGetPopupMenuFunc(
              gTasks[taskId].data[4],
              sDaycareStorageMenus,
              sDaycareStorageMenuItems,
              Menu_GetCursorPos());
            popupMenuFunc(taskId);
            return;
        }
        if (JOY_NEW(B_BUTTON))
        {
            DaycareStorageMenuCallback_Exit(taskId);
            return;
        }
    }
}

static void DaycareStorageMenuCallback_Store(u8 taskId)
{
    gSpecialVar_0x8004 = gLastFieldPokeMenuOpened;
    StartPartyMenuExitToField(taskId);
}

static void CB2_ReturnToDaycarePartyMenu(void)
{
    while (1)
    {
        if (InitPartyMenu() == TRUE)
        {
            SetPartyMenuCursorSelection(gPartyMenu.menuHandlerTaskId, gLastViewedMonIndex);
            ChangePartyMenuSelection(gPartyMenu.menuHandlerTaskId, 0);
            GetMonNickname(&gPlayerParty[gLastViewedMonIndex], gStringVar1);
            gLastFieldPokeMenuOpened = gLastViewedMonIndex;
            ShowDaycareStoragePopup(gPartyMenu.menuHandlerTaskId);
            SetMainCallback2(CB2_PartyMenuMain);
            break;
        }
        if (MenuHelpers_IsLinkActive() == 1)
            break;
    }
}

static void CB2_InitDaycareSummaryReturn(void)
{
    gPaletteFade.bufferTransferDisabled = TRUE;
    SetPartyMenuSettings(PARTY_MENU_TYPE_DAYCARE, 0xFF, Task_HandleDaycareStoragePopup, 5);
    SetMainCallback2(CB2_ReturnToDaycarePartyMenu);
}

static void Task_OpenDaycareSummary(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        u8 summaryMonIndex = gSprites[gTasks[taskId].data[3] >> 8].data[0];

        DestroyTask(taskId);
        gPartyMenu.unusedResumeAction = 1;
        ShowPokemonSummaryScreen(gPlayerParty, summaryMonIndex, gPlayerPartyCount - 1, CB2_InitDaycareSummaryReturn, SUMMARY_MODE_NORMAL);
    }
}

static void DaycareStorageMenuCallback_Summary(u8 taskId)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = Task_OpenDaycareSummary;
}

static void DaycareStorageMenuCallback_Exit(u8 taskId)
{
    PlaySE(SE_SELECT);
    Menu_EraseWindowRect(20, 10, 29, 19);
    Menu_DestroyCursor();
    PrintPartyMenuPromptText(PARTY_MSG_CHOOSE_MON_2, PARTY_MENU_PROMPT_WINDOW_STANDARD);
    gTasks[taskId].func = HandleDaycarePartyMenu;
}

void StartPartyMenuExitToField(u8 taskId)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = Task_WaitPartyMenuExitFade;
}

static void Task_WaitPartyMenuExitFade(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        gFieldCallback = FieldCallback_FadeFromPartyMenu;
        SetMainCallback2(CB2_ReturnToField);
        DestroyTask(taskId);
    }
}

// Do these last two functions really belong in here?

static void Task_PartyMenuWaitForFade(u8);

void FieldCallback_FadeFromPartyMenu(void)
{
    FadeInFromBlack();
    CreateTask(Task_PartyMenuWaitForFade, 10);
}

static void Task_PartyMenuWaitForFade(u8 taskId)
{
    if (IsWeatherNotFadingIn() == TRUE)
    {
        DestroyTask(taskId);
        UnlockPlayerFieldControls();
        ScriptContext_Enable();
    }
}
