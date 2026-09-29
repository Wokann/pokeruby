#include "global.h"
#include "easy_chat.h"
#include "pokemon.h"
#include "pokemon_menu.h"
#include "party_menu.h"
#include "palette.h"
#include "menu.h"
#include "mail_data.h"
#include "sound.h"
#include "main.h"
#include "overworld.h"
#include "menu_helpers.h"
#include "pokemon_summary_screen.h"
#include "data2.h"
#include "strings.h"
#include "item_use.h"
#include "item.h"
#include "event_data.h"
#include "mail.h"
#include "field_player_avatar.h"
#include "fldeff_softboiled.h"
#include "braille_puzzles.h"
#include "field_fadetransition.h"
#include "field_weather.h"
#include "field_effect.h"
#include "fldeff.h"
#include "field_control_avatar.h"
#include "metatile_behavior.h"
#include "fieldmap.h"
#include "item_menu.h"
#include "player_pc.h"
#include "ewram.h"
#include "script.h"
#include "constants/field_effects.h"
#include "constants/moves.h"
#include "constants/songs.h"

/*
Pokemon menu:
    The menu that appears when you
    click on a pokemon in
    overworld 'pokemon' menu
*/

struct FieldMoveCursorCallback
{
    bool8 (*fieldMoveFunc)(void);
    u8 msgId;
};

extern u8 gIsDebugMode;
extern u8 gUnknown_0202E8F4;
extern u8 gUnknown_0202E8F5;
extern bool8 gPartyMenuMessage_IsPrinting;
extern u8 gPokemonItemUseType;
extern u16 gUnknown_0202E8F8;
extern void (*gPokemonItemUseCallback)(u8 taskId, u16 itemId, TaskFunc callback);
extern TaskFunc gTaskFuncForLinkRecv;

void CB2_ReturnToPartyMenuFromWritingMail(void);
void CB2_BagMenuFromParty(void);
void CB2_OpenFlyMap(void);
bool8 SetUpFieldMove_Cut(void);
bool8 SetUpFieldMove_Flash(void);
bool8 SetUpFieldMove_Strength(void);
bool8 SetUpFieldMove_Teleport(void);
bool8 SetUpFieldMove_SecretPower(void);
bool8 SetUpFieldMove_SoftBoiled(void);
bool8 SetUpFieldMove_SoftBoiled(void);
bool8 SetUpFieldMove_SweetScent(void);

#define FIELD_MOVE_LIST_END 0xFF // Ruby's original sentinel prevents a field move from using index 0xFF.

// this file's functions
static void CB2_ReturnToPartyMenuFromReadingMail(void);
static void CB2_ReturnToPartyOrBagMenuFromWritingMail(void);
static void Task_HandleFieldPartyMenuInput(u8 taskId);
static void Task_CloseFieldPartyMenu(u8 taskId);
static void Task_DisplayGaveItemMessage(u8 taskId);
static void Task_ResetFieldPartyMenu(u8 taskId);
static void Task_HandleMailSubmenuInput(u8 taskId);
static void Task_OpenMailReader(u8 taskId);
static void Task_WaitForFieldMoveErrorMessage(u8 taskId);
static void Task_WaitForFieldMoveMessage(u8 taskId);
static void Task_ReturnToFieldAfterFieldMove(u8 taskId);
static void FieldCallback_AfterFadeInFromMenu(u8 taskId);
static void Task_FadeOutToBagMenu(u8 taskId);
static void Task_HandleBagGiveItemResult(u8 taskId);
static void Task_FadeOutToMailbox(u8 taskId);
static void Task_ReturnToBagMenu(u8 taskId);
static void Task_UseItemOnAllPartyMons(u8 taskId);
static void Task_WriteMailToGiveMonFromBag(u8 taskId);
static void Task_DisplayGaveItemFromBagMessage(u8 taskId);
static void Task_WaitForGaveItemFromBagMessage(u8 taskId);
static void Task_WaitForLinkRecv(u8 taskId);
static void PokemonMenu_Summary(u8 taskId);
static void PokemonMenu_Switch(u8 taskId);
static void PokemonMenu_Item(u8 taskId);
static void PokemonMenu_Cancel(u8 taskId);
static void PokemonMenu_GiveItem(u8 taskId);
static void PokemonMenu_TakeItem(u8 taskId);
static void PokemonMenu_TakeMail(u8 taskId);
static void PokemonMenu_Mail(u8 taskId);
static void PokemonMenu_ReadMail(u8 taskId);
static void PokemonMenu_CancelSubmenu(u8 taskId);
static void PokemonMenu_FieldMove(u8 taskId);
static bool8 SetUpFieldMove_Waterfall(void);
static bool8 SetUpFieldMove_Surf(void);
static bool8 SetUpFieldMove_Fly(void);
static bool8 SetUpFieldMove_Dive(void);

// ewram data

EWRAM_DATA static u8 sFieldPartyMenuCursorPos = 0;
EWRAM_DATA static u8 sFieldPartyMenuOptionCount = 0;
EWRAM_DATA static u8 sFieldPartyMenuOptionIds[8] = {0}; // 4 possible field moves and 4 default options

// iwram common
u8 gLastFieldPokeMenuOpened;
void (*gPostMenuFieldCallback)(void);

// const data

static const struct MenuAction2 sPokemonMenuActions[] =
{
    {OtherText_Summary,             PokemonMenu_Summary},
    {OtherText_Switch2,             PokemonMenu_Switch},
    {OtherText_Item,                PokemonMenu_Item},
    {gOtherText_CancelNoTerminator, PokemonMenu_Cancel},
    {OtherText_Give2,               PokemonMenu_GiveItem},
    {OtherText_Take2,               PokemonMenu_TakeItem},
    {OtherText_Take,                PokemonMenu_TakeMail},
    {OtherText_Mail,                PokemonMenu_Mail},
    {OtherText_Read2,               PokemonMenu_ReadMail},
    {gOtherText_CancelNoTerminator, PokemonMenu_CancelSubmenu},
    {gMoveNames[MOVE_CUT],          PokemonMenu_FieldMove},
    {gMoveNames[MOVE_FLASH],        PokemonMenu_FieldMove},
    {gMoveNames[MOVE_ROCK_SMASH],   PokemonMenu_FieldMove},
    {gMoveNames[MOVE_STRENGTH],     PokemonMenu_FieldMove},
    {gMoveNames[MOVE_SURF],         PokemonMenu_FieldMove},
    {gMoveNames[MOVE_FLY],          PokemonMenu_FieldMove},
    {gMoveNames[MOVE_DIVE],         PokemonMenu_FieldMove},
    {gMoveNames[MOVE_WATERFALL],    PokemonMenu_FieldMove},
    {gMoveNames[MOVE_TELEPORT],     PokemonMenu_FieldMove},
    {gMoveNames[MOVE_DIG],          PokemonMenu_FieldMove},
    {gMoveNames[MOVE_SECRET_POWER], PokemonMenu_FieldMove},
    {gMoveNames[MOVE_MILK_DRINK],   PokemonMenu_FieldMove},
    {gMoveNames[MOVE_SOFT_BOILED],  PokemonMenu_FieldMove},
    {gMoveNames[MOVE_SWEET_SCENT],  PokemonMenu_FieldMove},
};

static const u16 sFieldMoves[] =
{
    MOVE_CUT, MOVE_FLASH, MOVE_ROCK_SMASH, MOVE_STRENGTH,
    MOVE_SURF, MOVE_FLY, MOVE_DIVE, MOVE_WATERFALL,
    MOVE_TELEPORT, MOVE_DIG, MOVE_SECRET_POWER, MOVE_MILK_DRINK,
    MOVE_SOFT_BOILED, MOVE_SWEET_SCENT, FIELD_MOVE_LIST_END,
};

static const u8 sItemSubmenuActions[] = {4, 5, 9, 0};
static const struct PartyPopupMenu sItemSubmenu = {3, 6, sItemSubmenuActions};

static const u8 sMailSubmenuActions[] = {8, 6, 9, 0};
static const struct PartyPopupMenu sMailSubmenu = {3, 9, sMailSubmenuActions};

static const struct FieldMoveCursorCallback sFieldMoveCursorCallbacks[] =
{
    {SetUpFieldMove_Cut,            0x6},
    {SetUpFieldMove_Flash,          0x9},
    {SetUpFieldMove_RockSmash,      0x9},
    {SetUpFieldMove_Strength,       0x9},
    {SetUpFieldMove_Surf,           0x7},
    {SetUpFieldMove_Fly,            0x9},
    {SetUpFieldMove_Dive,           0x9},
    {SetUpFieldMove_Waterfall,      0x9},
    {SetUpFieldMove_Teleport,       0x9},
    {SetUpFieldMove_Dig,            0x9},
    {SetUpFieldMove_SecretPower,    0x9},
    {SetUpFieldMove_SoftBoiled,     0x10},
    {SetUpFieldMove_SoftBoiled,     0x10},
    {SetUpFieldMove_SweetScent,     0x9},
};

void CB2_OpenPartyMenuFromStartMenu(void)
{
    gPaletteFade.bufferTransferDisabled = 1;
    OpenPartyMenu(PARTY_MENU_TYPE_STANDARD, 0);
}

static void BuildFieldPartyMenuOptions(void)
{
    sFieldPartyMenuOptionCount = 0;
    // if checking pokemon is an egg, we can't give it an item and it doesn't know any move
    if (GetMonData(&gPlayerParty[gLastFieldPokeMenuOpened], MON_DATA_IS_EGG))
    {
        AppendToList(sFieldPartyMenuOptionIds, &sFieldPartyMenuOptionCount, POKEMENU_SUMMARY);
        AppendToList(sFieldPartyMenuOptionIds, &sFieldPartyMenuOptionCount, POKEMENU_SWITCH);
        AppendToList(sFieldPartyMenuOptionIds, &sFieldPartyMenuOptionCount, POKEMENU_CANCEL);
    }
    else
    {
        u16 moveId, fieldMoveId;

        for (moveId = 0; moveId < 4; moveId++) // 4, max number of possible field moves
        {
            for (fieldMoveId = 0; sFieldMoves[fieldMoveId] != FIELD_MOVE_LIST_END; fieldMoveId++)
            {
                if (GetMonData(&gPlayerParty[gLastFieldPokeMenuOpened], MON_DATA_MOVE1 + moveId) == sFieldMoves[fieldMoveId])
                {
                    u8 menuAction = fieldMoveId + POKEMENU_FIRST_FIELD_MOVE_ID;

                    AppendToList(sFieldPartyMenuOptionIds, &sFieldPartyMenuOptionCount, menuAction);
                    break;
                }
            }
        }
        AppendToList(sFieldPartyMenuOptionIds, &sFieldPartyMenuOptionCount, POKEMENU_SUMMARY);

        // can't switch a pokemon if it's the only one in the party
        if (GetMonData(&gPlayerParty[1], MON_DATA_SPECIES) != 0)
            AppendToList(sFieldPartyMenuOptionIds, &sFieldPartyMenuOptionCount, POKEMENU_SWITCH);

        if (ItemIsMail(GetMonData(&gPlayerParty[gLastFieldPokeMenuOpened], MON_DATA_HELD_ITEM)))
            AppendToList(sFieldPartyMenuOptionIds, &sFieldPartyMenuOptionCount, POKEMENU_MAIL);
        else
            AppendToList(sFieldPartyMenuOptionIds, &sFieldPartyMenuOptionCount, POKEMENU_ITEM);

        AppendToList(sFieldPartyMenuOptionIds, &sFieldPartyMenuOptionCount, POKEMENU_CANCEL);
    }
}

static void DrawPartyPopupMenuInternal(u8 left, u8 top, u8 width, u8 numChoices, const struct MenuAction2 *menuActions, const u8 *items, u8 promptTextId)
{
    PrintPartyMenuPromptText(5, promptTextId);
    Menu_DrawStdWindowFrame(left, top, left + width, (numChoices * 2) + top + 1);
    Menu_PrintItemsReordered(left + 1, top + 1, numChoices, menuActions, items);
}

void DrawPartyPopupMenu(u8 left, u8 top, u8 width, u8 numChoices, const struct MenuAction2 *menuActions, const u8 *items)
{
    DrawPartyPopupMenuInternal(left, top, width, numChoices, menuActions, items, 1);
}

static void ShowFieldPartyMenu(u8 cursorPos)
{
    u32 top = (u8)(18 - (sFieldPartyMenuOptionCount << 1));

    DrawPartyPopupMenuInternal(19, top, 10, sFieldPartyMenuOptionCount, sPokemonMenuActions, sFieldPartyMenuOptionIds, 3);
    top |= 1;
    InitMenu(0, 20, top, sFieldPartyMenuOptionCount, cursorPos, 9);
}

void HandleDefaultPartyMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        switch (HandleDefaultPartyMenuInput(taskId))
        {
        case A_BUTTON:
            PlaySE(SE_SELECT);
            gLastFieldPokeMenuOpened = GetCursorSelectionMonId(taskId);
            GetMonNickname(&gPlayerParty[gLastFieldPokeMenuOpened], gStringVar1);
            BuildFieldPartyMenuOptions();
            sFieldPartyMenuCursorPos = 0;
            ShowFieldPartyMenu(0);
            gTasks[taskId].func = Task_HandleFieldPartyMenuInput;
            SetTaskToWaitForLinkRecv(taskId);
            break;
        case B_BUTTON:
            PlaySE(SE_SELECT);
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            gTasks[taskId].func = Task_CloseFieldPartyMenu;
            break;
        }
    }
}

static void Task_HandleFieldPartyMenuInput(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        if (JOY_REPT(DPAD_ANY) == DPAD_UP)
        {
            PlaySE(SE_SELECT);
            sFieldPartyMenuCursorPos = Menu_MoveCursor(-1);
            SetTaskToWaitForLinkRecv(taskId);
        }
        else if (JOY_REPT(DPAD_ANY) == DPAD_DOWN)
        {
            PlaySE(SE_SELECT);
            sFieldPartyMenuCursorPos = Menu_MoveCursor(1);
            SetTaskToWaitForLinkRecv(taskId);
        }
        else if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            sPokemonMenuActions[sFieldPartyMenuOptionIds[sFieldPartyMenuCursorPos]].func(taskId);
            SetTaskToWaitForLinkRecv(taskId);
        }
        else if (JOY_NEW(B_BUTTON))
        {
            PokemonMenu_Cancel(taskId);
            SetTaskToWaitForLinkRecv(taskId);
        }
    }
}

static void Task_CloseFieldPartyMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        gLastFieldPokeMenuOpened = 0;
        SetMainCallback2(CB2_ReturnToFieldWithOpenMenu);
        DestroyTask(taskId);
    }
}

static void ShowFieldPartyMenuForSelectedMon(void)
{
    GetMonNickname(&gPlayerParty[gLastFieldPokeMenuOpened], gStringVar1);
    BuildFieldPartyMenuOptions();
    sFieldPartyMenuCursorPos = 0;
    ShowFieldPartyMenu(0);
}

static void CB2_InitPartyMenuFromSummaryScreen(void)
{
    do
    {
        if (InitPartyMenu() == TRUE)
        {
            sub_806C994(ePartyMenu2.menuHandlerTaskId, gLastViewedMonIndex);
            ChangePartyMenuSelection(ePartyMenu2.menuHandlerTaskId, 0);
            gLastFieldPokeMenuOpened = gLastViewedMonIndex;
            ShowFieldPartyMenuForSelectedMon();
            SetMainCallback2(CB2_PartyMenuMain);
            break;
        }
    } while (MenuHelpers_IsLinkActive() != TRUE);
}

static void CB2_ReturnToPartyMenuFromSummaryScreen(void)
{
    gPaletteFade.bufferTransferDisabled = 1;
    SetPartyMenuSettings(PARTY_MENU_TYPE_STANDARD, 0xFF, Task_HandleFieldPartyMenuInput, 5);
    SetMainCallback2(CB2_InitPartyMenuFromSummaryScreen);
}

static void Task_OpenSummaryScreen(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        u8 spriteId = gSprites[gTasks[taskId].data[3] >> 8].data[0];

        DestroyTask(taskId);
        ePartyMenu2.unk262 = 1;
        ShowPokemonSummaryScreen(gPlayerParty, spriteId, gPlayerPartyCount - 1, CB2_ReturnToPartyMenuFromSummaryScreen, PSS_MODE_NORMAL);
    }
}

static void PokemonMenu_Summary(u8 taskId)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = Task_OpenSummaryScreen;
}

void DoPokemonMenu_Switch(u8 taskId)
{
    SetTaskFuncWithFollowupFunc(taskId, SetupDefaultPartyMenuSwitchPokemon, HandleDefaultPartyMenu);
    Menu_EraseWindowRect(19, 0, 29, 19);
}

static void PokemonMenu_Switch(u8 taskId)
{
    Menu_DestroyCursor();
    ePartyMenu.unkC = sub_806CD5C;
    ePartyMenu.array[53553] = 1;
    DoPokemonMenu_Switch(taskId);
}

static void Task_HandleItemSubmenuInput(u8 taskId)
{
    if (gMain.newKeys == DPAD_UP && sFieldPartyMenuCursorPos != 0)
    {
        sFieldPartyMenuCursorPos = Menu_MoveCursor(-1);
        PlaySE(SE_SELECT);
    }
    if (gMain.newKeys == DPAD_DOWN && sFieldPartyMenuCursorPos != 2)
    {
        sFieldPartyMenuCursorPos = Menu_MoveCursor(1);
        PlaySE(SE_SELECT);
    }
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        PartyMenuGetPopupMenuFunc(0, &sItemSubmenu, sPokemonMenuActions, sFieldPartyMenuCursorPos)(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        ClosePartyPopupMenu(0, &sItemSubmenu);
        PokemonMenu_CancelSubmenu(taskId);
    }
}

static void Task_ShowItemSubmenu(u8 taskId)
{
    ShowPartyPopupMenu(0, &sItemSubmenu, (void *)sPokemonMenuActions, 0);
    PrintPartyMenuPromptText(0xD, 2);
    gTasks[taskId].func = Task_HandleItemSubmenuInput;
}

static void PokemonMenu_Item(u8 taskId)
{
    Menu_DestroyCursor();
    sFieldPartyMenuCursorPos = 0;
    Menu_EraseWindowRect(19, 0, 29, 19);
    gTasks[taskId].func = Task_ShowItemSubmenu;
}

static void Task_WriteMailToGiveMon(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        u8 mailId = GetMonData(&gPlayerParty[GetCursorSelectionMonId(taskId)], MON_DATA_MAIL);

        DestroyTask(taskId);
        DoEasyChatScreen(4, gSaveBlock1.mail[mailId].words, CB2_ReturnToPartyMenuFromWritingMail, 3);
    }
}

static void Task_UpdateHeldItemSprite(u8 taskId)
{
    if (gPartyMenuMessage_IsPrinting != 1)
    {
        SetHeldItemIconVisibility(taskId, GetCursorSelectionMonId(taskId));
        PrintPartyMenuPromptText(0, 0);
        gTasks[taskId].func = HandleDefaultPartyMenu;
    }
}

static void Task_HandleGiveHeldItemResult(u8 taskId)
{
    if (ItemIsMail(gSpecialVar_ItemId) && gUnknown_0202E8F4 != 0)
    {
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        gTasks[taskId].func = Task_WriteMailToGiveMon;
    }
    else
    {
        Menu_EraseWindowRect(0, 0, 29, 19);
        PrintPartyMenuPromptText(0, 0);
        gTasks[taskId].func = HandleDefaultPartyMenu;
    }
}

static void Task_GiveHeldItemFromBag(u8 taskId)
{
    if (!gPaletteFade.active)
        PartyMenuTryGiveMonHeldItem(taskId, gSpecialVar_ItemId, Task_HandleGiveHeldItemResult);
}

static void Task_WriteMailToGiveMonFromFieldBag(u8 taskId)
{
    u8 mailId = GetMonData(&gPlayerParty[GetCursorSelectionMonId(taskId)], MON_DATA_MAIL);

    DestroyTask(taskId);
    DoEasyChatScreen(4, gSaveBlock1.mail[mailId].words, CB2_ReturnToPartyMenuFromWritingMail, 3);
}

static void Task_GiveMailFromFieldBag(u8 taskId)
{
    PartyMenuTryGiveMonHeldItem(taskId, gSpecialVar_ItemId, Task_WriteMailToGiveMonFromFieldBag);
}

static void CB2_RunTasksForFieldPartyMenu(void)
{
    RunTasks();
}

static void CB2_InitPartyMenuForFieldBagItem(void)
{
    while (1)
    {
        if (InitPartyMenu() == TRUE)
        {
            sub_806C994(ePartyMenu2.menuHandlerTaskId, gLastFieldPokeMenuOpened);
            ChangePartyMenuSelection(ePartyMenu2.menuHandlerTaskId, 0);
            SetMainCallback2(CB2_PartyMenuMain);
            break;
        }
        if (MenuHelpers_IsLinkActive() == TRUE)
            break;
    }
}

static void CB2_InitFieldPartyMenuAfterBag(void)
{
    while (1)
    {
        if (InitPartyMenu() == TRUE)
        {
            sub_806C994(ePartyMenu2.menuHandlerTaskId, gLastFieldPokeMenuOpened);
            ChangePartyMenuSelection(ePartyMenu2.menuHandlerTaskId, 0);
            ePartyMenu2.unk262 = 3;
            ShowFieldPartyMenuForSelectedMon();
            SetMainCallback2(CB2_PartyMenuMain);
            break;
        }
        if (MenuHelpers_IsLinkActive() == TRUE)
            break;
    }
}

void CB2_ReturnToPartyMenuFromBag(void)
{
    if (ItemIsMail(gSpecialVar_ItemId))
    {
        u8 taskId = CreateTask(Task_GiveMailFromFieldBag, 0);

        gPaletteFade.bufferTransferDisabled = 1;
        CreatePartyMenuCursor(taskId, 0);
        sub_806C994(taskId, gLastFieldPokeMenuOpened);
        ChangePartyMenuSelection(taskId, 0);
        if (!(bool8)(GetMonData(&gPlayerParty[GetCursorSelectionMonId(taskId)], MON_DATA_HELD_ITEM)))
        {
            SetMainCallback2(CB2_RunTasksForFieldPartyMenu);
            return;
        }
        else
            DestroyTask(taskId);
    }
    gPaletteFade.bufferTransferDisabled = 1;
    if (gSpecialVar_ItemId)
    {
        SetPartyMenuSettings(PARTY_MENU_TYPE_STANDARD, 0xFF, Task_GiveHeldItemFromBag, 0xFF);
        SetMainCallback2(CB2_InitPartyMenuForFieldBagItem);
    }
    else
    {
        SetPartyMenuSettings(PARTY_MENU_TYPE_STANDARD, 0xFF, Task_HandleFieldPartyMenuInput, 5);
        SetMainCallback2(CB2_InitFieldPartyMenuAfterBag);
    }
}

static void CB2_InitPartyMenuAfterWritingMail(void)
{
    while (1)
    {
        if (InitPartyMenu() == TRUE)
        {
            sub_806C994(ePartyMenu2.menuHandlerTaskId, gLastFieldPokeMenuOpened);
            ChangePartyMenuSelection(ePartyMenu2.menuHandlerTaskId, 0);
            SetMainCallback2(CB2_PartyMenuMain);
            break;
        }
        if (MenuHelpers_IsLinkActive() == TRUE)
            break;
    }
}

void CB2_ReturnToPartyMenuFromWritingMail(void)
{
    gPaletteFade.bufferTransferDisabled = 1;
    if (gSpecialVar_Result == 0)
    {
        if (gUnknown_0202E8F8)
            RemoveBagItem(gUnknown_0202E8F8, 1);
        AddBagItem(GetMonData(&gPlayerParty[gLastFieldPokeMenuOpened], MON_DATA_HELD_ITEM), 1);
        TakeMailFromMon(&gPlayerParty[gLastFieldPokeMenuOpened]);
        SetMonData(&gPlayerParty[gLastFieldPokeMenuOpened], MON_DATA_HELD_ITEM, (void*) &gUnknown_0202E8F8);
        SetPartyMenuSettings(PARTY_MENU_TYPE_STANDARD, 0xFF, HandleDefaultPartyMenu, 0);
    }
    else
        SetPartyMenuSettings(PARTY_MENU_TYPE_STANDARD, 0xFF, Task_DisplayGaveItemMessage, 0xFF);
    SetMainCallback2(CB2_InitPartyMenuAfterWritingMail);
}

static void Task_DisplayGaveItemMessage(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        DisplayGiveHeldItemMessage(gLastFieldPokeMenuOpened, gSpecialVar_ItemId, 0);
        gTasks[taskId].func = Task_UpdateHeldItemSprite;
    }
}

static void Task_OpenBagMenuFromParty(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(CB2_BagMenuFromParty);
        DestroyTask(taskId);
    }
}

static void PokemonMenu_GiveItem(u8 taskId)
{
    gUnknown_0202E8F5 = GetCursorSelectionMonId(taskId);
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = Task_OpenBagMenuFromParty;
}

static void Task_ResetFieldPartyMenuAfterItemAction(u8 taskId)
{
    Task_ResetFieldPartyMenu(taskId);
}

static void PokemonMenu_TakeItem(u8 taskId)
{
    Menu_DestroyCursor();
    Menu_EraseWindowRect(19, 0, 29, 19);
    PartyMenuEraseMsgBoxAndFrame();
    PartyMenuTryGiveMonHeldItem_806ECE8(taskId, Task_ResetFieldPartyMenuAfterItemAction);
}

static void PokemonMenu_TakeMail(u8 taskId)
{
    Menu_DestroyCursor();
    Menu_EraseWindowRect(19, 0, 29, 19);
    PartyMenuEraseMsgBoxAndFrame();
    DoTakeMail(taskId, Task_ResetFieldPartyMenuAfterItemAction);
}

static void PokemonMenu_Mail(u8 taskId)
{
    Menu_DestroyCursor();
    sFieldPartyMenuCursorPos = 0;
    Menu_EraseWindowRect(19, 0, 29, 19);
    ShowPartyPopupMenu(0, &sMailSubmenu, (void *)sPokemonMenuActions, 0);
    gTasks[taskId].func = Task_HandleMailSubmenuInput;
}

static void Task_HandleMailSubmenuInput(u8 taskId)
{
    if (gMain.newAndRepeatedKeys == DPAD_UP)
    {
        PlaySE(SE_SELECT);
        if (sFieldPartyMenuCursorPos == 0)
            sFieldPartyMenuCursorPos = Menu_MoveCursor(sMailSubmenu.numChoices - 1);
        else
            sFieldPartyMenuCursorPos = Menu_MoveCursor(-1);
    }
    if (gMain.newAndRepeatedKeys == DPAD_DOWN)
    {
        PlaySE(SE_SELECT);
        if (sFieldPartyMenuCursorPos == sMailSubmenu.numChoices - 1)
            sFieldPartyMenuCursorPos = Menu_MoveCursor(1 - sMailSubmenu.numChoices);
        else
            sFieldPartyMenuCursorPos = Menu_MoveCursor(1);
    }
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        PartyMenuGetPopupMenuFunc(0, &sMailSubmenu, sPokemonMenuActions, sFieldPartyMenuCursorPos)(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        ClosePartyPopupMenu(0, &sMailSubmenu);
        PokemonMenu_Cancel(taskId);
    }
}

static void PokemonMenu_ReadMail(u8 taskId)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = Task_OpenMailReader;
}

static void Task_OpenMailReader(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        u8 mailId = GetMonData(&gPlayerParty[GetCursorSelectionMonId(taskId)], MON_DATA_MAIL);

        DestroyTask(taskId);
        ReadMail(&gSaveBlock1.mail[mailId], CB2_ReturnToPartyMenuFromReadingMail, 1);
    }
}

static void CB2_ReturnToPartyMenuFromReadingMail(void)
{
    gLastViewedMonIndex = gLastFieldPokeMenuOpened;
    ePartyMenu2.unk262 = 4;
    CB2_ReturnToPartyMenuFromSummaryScreen();
}

static void Task_ResetFieldPartyMenu(u8 taskId)
{
    sFieldPartyMenuCursorPos = 0;
    Menu_EraseWindowRect(19, 0, 29, 19);
    PrintPartyMenuPromptText(0, 0);
    gTasks[taskId].func = HandleDefaultPartyMenu;
}

static void PokemonMenu_Cancel(u8 taskId)
{
    Menu_DestroyCursor();
    PlaySE(SE_SELECT);
    Task_ResetFieldPartyMenu(taskId);
}

static void PokemonMenu_CancelSubmenu(u8 taskId)
{
    Menu_DestroyCursor();
    PlaySE(SE_SELECT);
    Menu_EraseWindowRect(19, 0, 29, 19);
    PartyMenuEraseMsgBoxAndFrame();
    ShowFieldPartyMenu(sFieldPartyMenuCursorPos);
    gTasks[taskId].func = Task_HandleFieldPartyMenuInput;
}

#define IS_SOFTBOILED_MILKDRINK(fieldMoveId) ((fieldMoveId) == FIELD_MOVE_MILK_DRINK || (fieldMoveId) == FIELD_MOVE_SOFT_BOILED)
#define IS_SURF(fieldMoveId)                 ((fieldMoveId) == FIELD_MOVE_SURF)
#define IS_FLY(fieldMoveId)                  ((fieldMoveId) == FIELD_MOVE_FLY)

#define tFieldMoveId data[11]

static void PokemonMenu_FieldMove(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    Menu_DestroyCursor();
    tFieldMoveId = sFieldPartyMenuOptionIds[sFieldPartyMenuCursorPos] - POKEMENU_FIRST_FIELD_MOVE_ID;
    if (MenuHelpers_IsLinkActive() == TRUE)
    {
        Menu_EraseWindowRect(19, 0, 29, 19);
        if (IS_SOFTBOILED_MILKDRINK(tFieldMoveId))
            PrintPartyMenuPromptText(9, 0);
        else
            PrintPartyMenuPromptText(sFieldMoveCursorCallbacks[tFieldMoveId].msgId, 0);
        gTasks[taskId].func = Task_WaitForFieldMoveMessage;
    }
    else if (tFieldMoveId <= FIELD_MOVE_WATERFALL && FlagGet(FLAG_BADGE01_GET + tFieldMoveId) != TRUE)
    {
        // can't use a field HM move without a proper badge
        Menu_EraseWindowRect(19, 0, 29, 19);
        PartyMenuEraseMsgBoxAndFrame();
        DisplayPartyMenuMessage(gOtherText_CantBeUsedBadge, 1);
        gTasks[taskId].func = Task_WaitForFieldMoveErrorMessage;
    }
    else
    {
        if (sFieldMoveCursorCallbacks[tFieldMoveId].fieldMoveFunc() == TRUE)
        {
            sFieldPartyMenuCursorPos = 0;
            if (!IS_SOFTBOILED_MILKDRINK(tFieldMoveId))
            {
                gTasks[taskId].func = Task_ReturnToFieldAfterFieldMove;
                BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            }
            else
                ChooseMonForSoftboiled(taskId);
        }
        else
        {
            Menu_EraseWindowRect(19, 0, 29, 19);
            if (IS_SURF(tFieldMoveId) && TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_SURFING))
                PrintPartyMenuPromptText(8, 0);
            else
                PrintPartyMenuPromptText(sFieldMoveCursorCallbacks[tFieldMoveId].msgId, 0);
            gTasks[taskId].func = Task_WaitForFieldMoveMessage;
        }
    }
}

static void Task_WaitForFieldMoveErrorMessage(u8 taskId)
{
    if (gPartyMenuMessage_IsPrinting != 1 && (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON)))
    {
        Menu_EraseWindowRect(0, 14, 29, 19);
        PokemonMenu_Cancel(taskId);
    }
}

static void Task_ReturnToFieldAfterFieldMove(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        if (!IS_FLY(gTasks[taskId].tFieldMoveId) || ShouldDoBrailleFlyEffect())
            SetMainCallback2(CB2_ReturnToField);
        else
            SetMainCallback2(CB2_OpenFlyMap);
        DestroyTask(taskId);
    }
}

#undef tFieldMoveId

void FieldCallback_PrepareFadeInFromMenu(void)
{
    FadeInFromBlack();
    CreateTask(FieldCallback_AfterFadeInFromMenu, 8);
}

static void FieldCallback_AfterFadeInFromMenu(u8 taskId)
{
    if (IsWeatherNotFadingIn() == TRUE)
    {
        gFieldEffectArguments[0] = GetMonData(&gPlayerParty[gLastFieldPokeMenuOpened], MON_DATA_SPECIES);
        gPostMenuFieldCallback();
        DestroyTask(taskId);
    }
}

static void Task_WaitForFieldMoveMessage(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        Menu_EraseWindowRect(1, 17, 28, 18);
        PokemonMenu_Cancel(taskId);
    }
}

static void FieldCallback_Surf(void)
{
    gFieldEffectArguments[0] = gLastFieldPokeMenuOpened;
    FieldEffectStart(FLDEFF_USE_SURF);
}

static bool8 SetUpFieldMove_Surf(void)
{
    if (PartyHasMonWithSurf() == TRUE && IsPlayerFacingSurfableFishableWater() == TRUE)
    {
        gFieldCallback = FieldCallback_PrepareFadeInFromMenu;
        gPostMenuFieldCallback = FieldCallback_Surf;
        return TRUE;
    }
    else
        return FALSE;
}

static void FieldCallback_Fly(void)
{
    gFieldEffectArguments[0] = gLastFieldPokeMenuOpened;
    FieldEffectStart(FLDEFF_USE_FLY);
}

static bool8 SetUpFieldMove_Fly(void)
{
    if (ShouldDoBrailleFlyEffect())
    {
        gFieldCallback = FieldCallback_PrepareFadeInFromMenu;
        gPostMenuFieldCallback = DoBrailleFlyEffect;
        return TRUE;
    }
    if (Overworld_MapTypeAllowsTeleportAndFly(gMapHeader.mapType) == TRUE)
    {
        gFieldCallback = FieldCallback_PrepareFadeInFromMenu;
        gPostMenuFieldCallback = FieldCallback_Fly;
        return TRUE;
    }
    return FALSE;
}

static void CB2_InitPartyMenuAfterFlyMap(void)
{
    while (1)
    {
        if (InitPartyMenu() == TRUE)
        {
            sub_806C994(ePartyMenu2.menuHandlerTaskId, gLastFieldPokeMenuOpened);
            ChangePartyMenuSelection(ePartyMenu2.menuHandlerTaskId, 0);
            SetMainCallback2(CB2_PartyMenuMain);
            break;
        }
        if (MenuHelpers_IsLinkActive() == TRUE)
            break;
    }
}

void CB2_ReturnToPartyMenuFromFlyMap(void)
{
    gPaletteFade.bufferTransferDisabled = 1;
    SetPartyMenuSettings(PARTY_MENU_TYPE_STANDARD, 0xFF, HandleDefaultPartyMenu, 0);
    SetMainCallback2(CB2_InitPartyMenuAfterFlyMap);
}

u16 GetFieldMoveMonSpecies(void)
{
    return GetMonData(&gPlayerParty[gLastFieldPokeMenuOpened], MON_DATA_SPECIES);
}

static void FieldCallback_Dive(void)
{
    gFieldEffectArguments[0] = gLastFieldPokeMenuOpened;
    FieldEffectStart(FLDEFF_USE_DIVE);
}

static bool8 SetUpFieldMove_Dive(void)
{
    gFieldEffectArguments[1] = TrySetDiveWarp();
    if (gFieldEffectArguments[1])
    {
        gFieldCallback = FieldCallback_PrepareFadeInFromMenu;
        gPostMenuFieldCallback = FieldCallback_Dive;
        return TRUE;
    }
    else
        return FALSE;
}

static void FieldCallback_Waterfall(void)
{
    gFieldEffectArguments[0] = gLastFieldPokeMenuOpened;
    FieldEffectStart(FLDEFF_USE_WATERFALL);
}

static bool8 SetUpFieldMove_Waterfall(void)
{
    s16 x, y;

    GetXYCoordsOneStepInFrontOfPlayer(&x, &y);
    if (MetatileBehavior_IsWaterfall(MapGridGetMetatileBehaviorAt(x, y)) == TRUE
     && IsPlayerSurfingNorth() == TRUE)
    {
        gFieldCallback = FieldCallback_PrepareFadeInFromMenu;
        gPostMenuFieldCallback = FieldCallback_Waterfall;
        return TRUE;
    }
    else
        return FALSE;
}

#if DEBUG
void Debug_TryUseWaterfall(void)
{
    s16 x, y;

    GetXYCoordsOneStepInFrontOfPlayer(&x, &y);
    if (MetatileBehavior_IsWaterfall(MapGridGetMetatileBehaviorAt(x, y)) == TRUE
     && IsPlayerSurfingNorth() == TRUE)
        FieldCallback_Waterfall();
    else
        UnlockPlayerFieldControls();
}
#endif

static void UpdatePartyMenuForTMHM(void)
{
    u8 i;
    u8 tmhmIndex = gSpecialVar_ItemId - 33;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES))
        {
            sub_806D668(i);
            if (
#if DEBUG
             gIsDebugMode == 0 &&
#endif
             (GetMonData(&gPlayerParty[i], MON_DATA_IS_EGG) || !CanMonLearnTMHM(&gPlayerParty[i], tmhmIndex)))
                DrawMonDescriptorStatus(i, 0x9A);
            else if (pokemon_has_move(&gPlayerParty[i], ItemIdToBattleMoveId(gSpecialVar_ItemId)))
                DrawMonDescriptorStatus(i, 0xA8);
            else
                DrawMonDescriptorStatus(i, 0x8C);
        }
    }
}

static void UpdatePartyMenuForEvolutionItem(void)
{
    u8 i;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES))
        {
            if (GetMonData(&gPlayerParty[i], MON_DATA_IS_EGG) || !GetEvolutionTargetSpecies(&gPlayerParty[i], 3, gSpecialVar_ItemId))
            {
                sub_806D668(i);
                DrawMonDescriptorStatus(i, 0);
            }
        }
    }
}

static void CB2_InitPartyMenuForItemUse(void)
{
    while (1)
    {
        if (InitPartyMenu() == TRUE)
        {
            if (gPokemonItemUseType == ITEM_USE_SINGLE_MON)
            {
                switch (CheckIfItemIsTMHMOrEvolutionStone(gSpecialVar_ItemId))
                {
                case ITEM_IS_TM_HM:
                    UpdatePartyMenuForTMHM();
                    break;
                case ITEM_IS_EVOLUTION_STONE:
                    UpdatePartyMenuForEvolutionItem();
                    break;
                }
            }
            if (gLastFieldPokeMenuOpened >= PARTY_SIZE || !GetMonData(&gPlayerParty[gLastFieldPokeMenuOpened], MON_DATA_SPECIES))
                gLastFieldPokeMenuOpened = 0;
            sub_806C994(ePartyMenu2.menuHandlerTaskId, gLastFieldPokeMenuOpened);
            ChangePartyMenuSelection(ePartyMenu2.menuHandlerTaskId, 0);
            SetMainCallback2(CB2_PartyMenuMain);
            break;
        }
        if (MenuHelpers_IsLinkActive() == TRUE)
            break;
    }
}

void CB2_OpenPartyMenuForItemUse(void)
{
    gPaletteFade.bufferTransferDisabled = 1;
    switch (gPokemonItemUseType)
    {
    case ITEM_USE_SINGLE_MON:
        if (CheckIfItemIsTMHMOrEvolutionStone(gSpecialVar_ItemId) == ITEM_IS_TM_HM)
            SetPartyMenuSettings(PARTY_MENU_TYPE_STANDARD, 0, Task_HandlePartyMenuItemUseInput, 20);
        else
            SetPartyMenuSettings(PARTY_MENU_TYPE_STANDARD, 0, Task_HandlePartyMenuItemUseInput, 3);
        break;
    case ITEM_USE_ALL_MONS:
        SetPartyMenuSettings(PARTY_MENU_TYPE_STANDARD, 0, Task_UseItemOnAllPartyMons, 0xFF);
        break;
    case ITEM_USE_GIVE_ITEM:
    case ITEM_USE_GIVE_MAIL:
        SetPartyMenuSettings(PARTY_MENU_TYPE_STANDARD, 0, Task_HandlePartyMenuItemUseInput, 4);
        break;
    }
    SetMainCallback2(CB2_InitPartyMenuForItemUse);
}

void Task_HandlePartyMenuItemUseInput(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        switch (HandleDefaultPartyMenuInput(taskId))
        {
        case A_BUTTON:
            gLastFieldPokeMenuOpened = GetCursorSelectionMonId(taskId);
            if (GetMonData(&gPlayerParty[gLastFieldPokeMenuOpened], MON_DATA_IS_EGG))
                PlaySE(SE_FAILURE);
            else
            {
                PartyMenuEraseMsgBoxAndFrame();
                if (gPokemonItemUseType == ITEM_USE_SINGLE_MON)
                    gPokemonItemUseCallback(taskId, gSpecialVar_ItemId, Task_FadeOutToBagMenu);
                if (gPokemonItemUseType == ITEM_USE_GIVE_ITEM)
                {
                    PlaySE(SE_SELECT);
                    PartyMenuTryGiveMonHeldItem(taskId, gSpecialVar_ItemId, Task_HandleBagGiveItemResult);
                }
                if (gPokemonItemUseType == ITEM_USE_GIVE_MAIL)
                {
                    PlaySE(SE_SELECT);
                    PartyMenuTryGiveMonMail(taskId, Task_FadeOutToMailbox);
                }
            }
            break;
        case B_BUTTON:
            gLastFieldPokeMenuOpened = GetCursorSelectionMonId(taskId);
            PlaySE(SE_SELECT);
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            if (gPokemonItemUseType == ITEM_USE_SINGLE_MON || gPokemonItemUseType == ITEM_USE_GIVE_ITEM)
                gTasks[taskId].func = Task_ReturnToBagMenu;
            if (gPokemonItemUseType == ITEM_USE_GIVE_MAIL)
                gTasks[taskId].func = Task_FadeOutToMailbox;
            break;
        }
    }
}

static void Task_UseItemOnAllPartyMons(u8 taskId)
{
    if (!gPaletteFade.active)
        gPokemonItemUseCallback(taskId, gSpecialVar_ItemId, Task_FadeOutToBagMenu);
}

static void Task_FadeOutToBagMenu(u8 taskId)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = Task_ReturnToBagMenu;
}

static void Task_ReturnToBagMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(CB2_ReturnToBagMenu);
        DestroyTask(taskId);
    }
}

static void Task_ReturnToMailbox(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(Mailbox_ReturnToMailListAfterDeposit);
        DestroyTask(taskId);
    }
}

static void Task_FadeOutToMailbox(u8 taskId)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = Task_ReturnToMailbox;
}

static void Task_HandleBagGiveItemResult(u8 taskId)
{
    if (gUnknown_0202E8F4 == 2)
    {
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        gTasks[taskId].func = Task_WriteMailToGiveMonFromBag;
    }
    else
        Task_FadeOutToBagMenu(taskId);
}

static void Task_WriteMailToGiveMonFromBag(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        u8 mailId;

        gLastFieldPokeMenuOpened = GetCursorSelectionMonId(taskId);
        mailId = GetMonData(&gPlayerParty[gLastFieldPokeMenuOpened], MON_DATA_MAIL);
        DestroyTask(taskId);
        DoEasyChatScreen(4, gSaveBlock1.mail[mailId].words, CB2_ReturnToPartyOrBagMenuFromWritingMail, 3);
    }
}

static void CB2_InitPartyMenuAfterBagMail(void)
{
    while (1)
    {
        if (InitPartyMenu() == TRUE)
        {
            sub_806C994(ePartyMenu2.menuHandlerTaskId, gLastFieldPokeMenuOpened);
            ChangePartyMenuSelection(ePartyMenu2.menuHandlerTaskId, 0);
            SetMainCallback2(CB2_PartyMenuMain);
            break;
        }
        if (MenuHelpers_IsLinkActive() == TRUE)
            break;
    }
}

static void CB2_ReturnToPartyOrBagMenuFromWritingMail(void)
{
    IntrCallback callback;

    gPaletteFade.bufferTransferDisabled = 1;
    if (gSpecialVar_Result == 0)
    {
        if (gUnknown_0202E8F8)
            RemoveBagItem(gUnknown_0202E8F8, 1);
        AddBagItem(GetMonData(&gPlayerParty[gLastFieldPokeMenuOpened], MON_DATA_HELD_ITEM), 1);
        TakeMailFromMon(&gPlayerParty[gLastFieldPokeMenuOpened]);
        SetMonData(&gPlayerParty[gLastFieldPokeMenuOpened], MON_DATA_HELD_ITEM, (void*) &gUnknown_0202E8F8);
        CreateTask(Task_ReturnToBagMenu, 5);
        gPaletteFade.bufferTransferDisabled = 0;
        callback = CB2_PartyMenuMain;
    }
    else
    {
        SetPartyMenuSettings(PARTY_MENU_TYPE_STANDARD, 0, Task_DisplayGaveItemFromBagMessage, 0xFF);
        callback = CB2_InitPartyMenuAfterBagMail;
    }
    SetMainCallback2(callback);
}

static void Task_DisplayGaveItemFromBagMessage(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        DisplayGiveHeldItemMessage(gLastFieldPokeMenuOpened, gSpecialVar_ItemId, 1);
        gTasks[taskId].func = Task_WaitForGaveItemFromBagMessage;
    }
}

static void Task_WaitForGaveItemFromBagMessage(u8 taskId)
{
    if (gPartyMenuMessage_IsPrinting != 1)
        Task_FadeOutToBagMenu(taskId);
}

void Task_ExitPartyMenuToBag(u8 taskId)
{
    Task_FadeOutToBagMenu(taskId);
}

static void CB2_InitPartyMenuForMoveLearning(void)
{
    while (1)
    {
        if (InitPartyMenu() == TRUE)
        {
            sub_806C994(ePartyMenu2.menuHandlerTaskId, gLastViewedMonIndex);
            ChangePartyMenuSelection(ePartyMenu2.menuHandlerTaskId, 0);
            SetMainCallback2(CB2_PartyMenuMain);
            break;
        }
        if (MenuHelpers_IsLinkActive() == TRUE)
            break;
    }
}

void CB2_ReturnToPartyMenuForMoveLearning(void)
{
    gPaletteFade.bufferTransferDisabled = 1;
    if (GetMoveSlotToReplace() != 4)
        SetPartyMenuSettings(PARTY_MENU_TYPE_STANDARD, 0, TaughtMove, 0xFF);
    else
        SetPartyMenuSettings(PARTY_MENU_TYPE_STANDARD, 0, StopTryingToTeachMove_806F588, 0xFF);
    SetMainCallback2(CB2_InitPartyMenuForMoveLearning);
}

void SetTaskToWaitForLinkRecv(u32 taskId)
{
    gTaskFuncForLinkRecv = gTasks[taskId].func;
    gTasks[taskId].func = Task_WaitForLinkRecv;
    Task_WaitForLinkRecv(taskId);
}

static void Task_WaitForLinkRecv(u8 taskId)
{
    if (Overworld_IsRecvQueueAtMax() != TRUE)
        gTasks[taskId].func = gTaskFuncForLinkRecv;
}
