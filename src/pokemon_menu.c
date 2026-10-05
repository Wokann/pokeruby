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
#include "data.h"
#include "strings.h"
#include "item_use.h"
#include "item.h"
#include "event_data.h"
#include "mail.h"
#include "field_player_avatar.h"
#include "fldeff_softboiled.h"
#include "braille_puzzles.h"
#include "field_screen_effect.h"
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

struct PokeMenuFieldMoveFunc
{
    bool8 (*fieldMoveFunc)(void);
    u8 msgId;
};

extern u8 gUnknown_020297ED;
extern u8 gUnknown_0202E8F4;
extern u8 gSelectedMonPartyId;
extern bool8 gPartyMenuMessage_IsPrinting;
extern u8 gPokemonItemUseType;
extern u16 gUnknown_0202E8F8;
extern void (*gPokemonItemUseCallback)(u8 taskID, u16 itemID, TaskFunc func);
extern TaskFunc gUnknown_03005CF0;

static void CB2_ReturnToPartyMenuFromWritingMail(void);
void sub_80A61D0(void);
void CB2_InitFlyRegionMap(void);
bool8 SetUpFieldMove_Flash(void);
bool8 SetUpFieldMove_RockSmash(void);
bool8 SetUpFieldMove_Strength(void);
bool8 SetUpFieldMove_Teleport(void);
bool8 SetUpFieldMove_Dig(void);
bool8 SetUpFieldMove_SecretPower(void);
bool8 SetUpFieldMove_SoftBoiled(void);
bool8 SetUpFieldMove_SoftBoiled(void);
bool8 SetUpFieldMove_SweetScent(void);

#define sFieldMovesTerminator 0xFF // note: should be changed to 0xFFFF, because currently it makes it impossible to add a field move with 0xFF index

// this file's functions
static void CB2_ReturnToPartyMenuFromReadingMail(void);
static void CB2_ReturnToPartyOrBagMenuFromWritingMail(void);
static void Task_HandleSelectionMenuInput(u8 taskId);
static void sub_8089E4C(u8 taskId);
static void Task_DisplayGaveMailFromPartyMessage(u8 taskId);
static void sub_808A8D4(u8 taskId);
static void sub_808A73C(u8 taskId);
static void sub_808A848(u8 taskId);
static void sub_808AAF0(u8 taskId);
static void Task_CancelAfterAorBPress(u8 taskId);
static void sub_808AB34(u8 taskId);
static void Task_FieldMoveWaitForFade(u8 taskId);
static void sub_808B224(u8 taskId);
static void sub_808B2EC(u8 taskId);
static void sub_808B2B4(u8 taskId);
static void sub_808B25C(u8 taskId);
static void sub_808B1EC(u8 taskId);
static void sub_808B338(u8 taskId);
static void Task_DisplayGaveMailFromBagMessage(u8 taskId);
static void sub_808B4EC(u8 taskId);
static void sub_808B5E4(u8 taskId);
static void CursorCb_Summary(u8 taskId);
static void CursorCb_Switch(u8 taskId);
static void CursorCb_Item(u8 taskId);
static void CursorCb_Cancel1(u8 taskId);
static void CursorCb_Give(u8 taskId);
static void CursorCb_TakeItem(u8 taskId);
static void CursorCb_TakeMail(u8 taskId);
static void CursorCb_Mail(u8 taskId);
static void CursorCb_Read(u8 taskId);
static void CursorCb_Cancel2(u8 taskId);
static void CursorCb_FieldMove(u8 taskId);
static bool8 SetUpFieldMove_Waterfall(void);
static bool8 SetUpFieldMove_Surf(void);
static bool8 SetUpFieldMove_Fly(void);
static bool8 SetUpFieldMove_Dive(void);

// ewram data

EWRAM_DATA static u8 sPokeMenuCursorPos = 0;
EWRAM_DATA static u8 sPokeMenuOptionsNo = 0;
EWRAM_DATA static u8 sPokeMenuOptionsOrder[8] = {0}; // 4 possible field moves and 4 default options

// iwram common
u8 gLastFieldPokeMenuOpened;
void (*gPostMenuFieldCallback)(void);

// const data

static const struct MenuAction2 sCursorOptions[] =
{
    {gText_Summary5,             CursorCb_Summary},
    {gText_Switch2,             CursorCb_Switch},
    {gText_Item,                CursorCb_Item},
    {gText_Cancel2, CursorCb_Cancel1},
    {gMenuText_Give,               CursorCb_Give},
    {gText_Take,               CursorCb_TakeItem},
    {gText_Take2,                CursorCb_TakeMail},
    {gText_Mail,                CursorCb_Mail},
    {gText_Read2,               CursorCb_Read},
    {gText_Cancel2, CursorCb_Cancel2},
    {gMoveNames[MOVE_CUT],          CursorCb_FieldMove},
    {gMoveNames[MOVE_FLASH],        CursorCb_FieldMove},
    {gMoveNames[MOVE_ROCK_SMASH],   CursorCb_FieldMove},
    {gMoveNames[MOVE_STRENGTH],     CursorCb_FieldMove},
    {gMoveNames[MOVE_SURF],         CursorCb_FieldMove},
    {gMoveNames[MOVE_FLY],          CursorCb_FieldMove},
    {gMoveNames[MOVE_DIVE],         CursorCb_FieldMove},
    {gMoveNames[MOVE_WATERFALL],    CursorCb_FieldMove},
    {gMoveNames[MOVE_TELEPORT],     CursorCb_FieldMove},
    {gMoveNames[MOVE_DIG],          CursorCb_FieldMove},
    {gMoveNames[MOVE_SECRET_POWER], CursorCb_FieldMove},
    {gMoveNames[MOVE_MILK_DRINK],   CursorCb_FieldMove},
    {gMoveNames[MOVE_SOFT_BOILED],  CursorCb_FieldMove},
    {gMoveNames[MOVE_SWEET_SCENT],  CursorCb_FieldMove},
};

static const u16 sFieldMoves[] =
{
    MOVE_CUT, MOVE_FLASH, MOVE_ROCK_SMASH, MOVE_STRENGTH,
    MOVE_SURF, MOVE_FLY, MOVE_DIVE, MOVE_WATERFALL,
    MOVE_TELEPORT, MOVE_DIG, MOVE_SECRET_POWER, MOVE_MILK_DRINK,
    MOVE_SOFT_BOILED, MOVE_SWEET_SCENT, sFieldMovesTerminator,
};

static const u8 sPartyMenuAction_GiveTakeItemCancel[] = {4, 5, 9, 0};
static const struct PartyPopupMenu sUnknown_0839F578 = {3, 6, sPartyMenuAction_GiveTakeItemCancel};

static const u8 sPartyMenuAction_ReadTakeMailCancel[] = {8, 6, 9, 0};
static const struct PartyPopupMenu sUnknown_0839F584 = {3, 9, sPartyMenuAction_ReadTakeMailCancel};

static const struct PokeMenuFieldMoveFunc sFieldMoveCursorCallbacks[] =
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

void CB2_PartyMenuFromStartMenu(void)
{
    gPaletteFade.bufferTransferDisabled = 1;
    OpenPartyMenu(PARTY_MENU_TYPE_STANDARD, 0);
}

static void SetPartyMonFieldSelectionActions(void)
{
    sPokeMenuOptionsNo = 0;
    // if checking pokemon is an egg, we can't give it an item and it doesn't know any move
    if (GetMonData(&gPlayerParty[gLastFieldPokeMenuOpened], MON_DATA_IS_EGG))
    {
        AppendToList(sPokeMenuOptionsOrder, &sPokeMenuOptionsNo, MENU_SUMMARY);
        AppendToList(sPokeMenuOptionsOrder, &sPokeMenuOptionsNo, MENU_SWITCH);
        AppendToList(sPokeMenuOptionsOrder, &sPokeMenuOptionsNo, MENU_CANCEL1);
    }
    else
    {
        u16 i, j;
        for (i = 0; i < 4; i++) // 4, max number of possible field moves
        {
            for (j = 0; sFieldMoves[j] != sFieldMovesTerminator; j++)
            {
                if (GetMonData(&gPlayerParty[gLastFieldPokeMenuOpened], MON_DATA_MOVE1 + i) == sFieldMoves[j])
                {
                    u8 fieldID = j + MENU_FIELD_MOVES;
                    AppendToList(sPokeMenuOptionsOrder, &sPokeMenuOptionsNo, fieldID);
                    break;
                }
            }
        }
        AppendToList(sPokeMenuOptionsOrder, &sPokeMenuOptionsNo, MENU_SUMMARY);

        // can't switch a pokemon if it's the only one in the party
        if (GetMonData(&gPlayerParty[1], MON_DATA_SPECIES) != 0)
            AppendToList(sPokeMenuOptionsOrder, &sPokeMenuOptionsNo, MENU_SWITCH);

        if (ItemIsMail(GetMonData(&gPlayerParty[gLastFieldPokeMenuOpened], MON_DATA_HELD_ITEM)))
            AppendToList(sPokeMenuOptionsOrder, &sPokeMenuOptionsNo, MENU_MAIL);
        else
            AppendToList(sPokeMenuOptionsOrder, &sPokeMenuOptionsNo, MENU_ITEM);

        AppendToList(sPokeMenuOptionsOrder, &sPokeMenuOptionsNo, MENU_CANCEL1);
    }
}

static void sub_8089BDC(u8 arg0, u8 arg1, u8 arg2, u8 noOfOptions, const struct MenuAction2 *menuActions, const u8 *order, u8 arg6)
{
    PrintPartyMenuPromptText(5, arg6);
    Menu_DrawStdWindowFrame(arg0, arg1, arg0 + arg2, (noOfOptions * 2) + arg1 + 1);
    Menu_PrintItemsReordered(arg0 + 1, arg1 + 1, noOfOptions, menuActions, order);
}

void sub_8089C50(u8 arg0, u8 arg1, u8 arg2, u8 noOfOptions, const struct MenuAction2 *menuActions, const u8 *order)
{
    sub_8089BDC(arg0, arg1, arg2, noOfOptions, menuActions, order, 1);
}

static void sub_8089C7C(u8 arg0)
{
    u32 r4 = (u8)(18 - (sPokeMenuOptionsNo << 1));

    sub_8089BDC(19, r4, 10, sPokeMenuOptionsNo, sCursorOptions, sPokeMenuOptionsOrder, 3);
    r4 |= 1;
    InitMenu(0, 20, r4, sPokeMenuOptionsNo, arg0, 9);
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
            SetPartyMonFieldSelectionActions();
            sPokeMenuCursorPos = 0;
            sub_8089C7C(0);
            gTasks[taskId].func = Task_HandleSelectionMenuInput;
            sub_808B5B4(taskId);
            break;
        case B_BUTTON:
            PlaySE(SE_SELECT);
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            gTasks[taskId].func = sub_8089E4C;
            break;
        }
    }
}

static void Task_HandleSelectionMenuInput(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        if (JOY_REPT(DPAD_ANY) == DPAD_UP)
        {
            PlaySE(SE_SELECT);
            sPokeMenuCursorPos = Menu_MoveCursor(-1);
            sub_808B5B4(taskId);
        }
        else if (JOY_REPT(DPAD_ANY) == DPAD_DOWN)
        {
            PlaySE(SE_SELECT);
            sPokeMenuCursorPos = Menu_MoveCursor(1);
            sub_808B5B4(taskId);
        }
        else if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            sCursorOptions[sPokeMenuOptionsOrder[sPokeMenuCursorPos]].func(taskId);
            sub_808B5B4(taskId);
        }
        else if (JOY_NEW(B_BUTTON))
        {
            CursorCb_Cancel1(taskId);
            sub_808B5B4(taskId);
        }
    }
}

static void sub_8089E4C(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        gLastFieldPokeMenuOpened = 0;
        SetMainCallback2(CB2_ReturnToFieldWithOpenMenu);
        DestroyTask(taskId);
    }
}

static void sub_8089E84(void)
{
    GetMonNickname(&gPlayerParty[gLastFieldPokeMenuOpened], gStringVar1);
    SetPartyMonFieldSelectionActions();
    sPokeMenuCursorPos = 0;
    sub_8089C7C(0);
}

static void sub_8089EBC(void)
{
    do
    {
        if (InitPartyMenu() == TRUE)
        {
            sub_806C994(ePartyMenu2.menuHandlerTaskId, gLastViewedMonIndex);
            ChangePartyMenuSelection(ePartyMenu2.menuHandlerTaskId, 0);
            gLastFieldPokeMenuOpened = gLastViewedMonIndex;
            sub_8089E84();
            SetMainCallback2(CB2_PartyMenuMain);
            break;
        }
    } while (sub_80F9344() != TRUE);
}

static void CB2_ReturnToPartyMenuFromSummaryScreen(void)
{
    gPaletteFade.bufferTransferDisabled = 1;
    SetPartyMenuSettings(PARTY_MENU_TYPE_STANDARD, 0xFF, Task_HandleSelectionMenuInput, 5);
    SetMainCallback2(sub_8089EBC);
}

static void sub_8089F44(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        u8 spriteID = gSprites[gTasks[taskId].data[3] >> 8].data[0];
        DestroyTask(taskId);
        ePartyMenu2.unk262 = 1;
        ShowPokemonSummaryScreen(gPlayerParty, spriteID, gPlayerPartyCount - 1, CB2_ReturnToPartyMenuFromSummaryScreen, PSS_MODE_NORMAL);
    }
}

static void CursorCb_Summary(u8 taskId)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = sub_8089F44;
}

void DoPokemonMenu_Switch(u8 taskId)
{
    SetTaskFuncWithFollowupFunc(taskId, SetupDefaultPartyMenuSwitchPokemon, HandleDefaultPartyMenu);
    Menu_EraseWindowRect(19, 0, 29, 19);
}

static void CursorCb_Switch(u8 taskId)
{
    Menu_DestroyCursor();
    ePartyMenu.unkC = sub_806CD5C;
    ePartyMenu.array[53553] = 1;
    DoPokemonMenu_Switch(taskId);
}

static void sub_808A060(u8 taskId)
{
    if (gMain.newKeys == DPAD_UP && sPokeMenuCursorPos != 0)
    {
        sPokeMenuCursorPos = Menu_MoveCursor(-1);
        PlaySE(SE_SELECT);
    }
    if (gMain.newKeys == DPAD_DOWN && sPokeMenuCursorPos != 2)
    {
        sPokeMenuCursorPos = Menu_MoveCursor(1);
        PlaySE(SE_SELECT);
    }
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        PartyMenuGetPopupMenuFunc(0, &sUnknown_0839F578, sCursorOptions, sPokeMenuCursorPos)(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        ClosePartyPopupMenu(0, &sUnknown_0839F578);
        CursorCb_Cancel2(taskId);
    }
}

static void sub_808A100(u8 taskId)
{
    ShowPartyPopupMenu(0, &sUnknown_0839F578, (void*)(sCursorOptions), 0);
    PrintPartyMenuPromptText(0xD, 2);
    gTasks[taskId].func = sub_808A060;
}

static void CursorCb_Item(u8 taskId)
{
    Menu_DestroyCursor();
    sPokeMenuCursorPos = 0;
    Menu_EraseWindowRect(19, 0, 29, 19);
    gTasks[taskId].func = sub_808A100;
}

static void sub_808A180(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        u8 mail = GetMonData(&gPlayerParty[GetCursorSelectionMonId(taskId)], MON_DATA_MAIL);
        DestroyTask(taskId);
        sub_80E62A0(4, gSaveBlock1.mail[mail].words, CB2_ReturnToPartyMenuFromWritingMail, 3);
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

static void sub_808A228(u8 taskId)
{
    if (ItemIsMail(gSpecialVar_ItemId) && gUnknown_0202E8F4 != 0)
    {
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        gTasks[taskId].func = sub_808A180;
    }
    else
    {
        Menu_EraseWindowRect(0, 0, 29, 19);
        PrintPartyMenuPromptText(0, 0);
        gTasks[taskId].func = HandleDefaultPartyMenu;
    }
}

static void sub_808A2AC(u8 taskId)
{
    if (!gPaletteFade.active)
        PartyMenuTryGiveMonHeldItem(taskId, gSpecialVar_ItemId, sub_808A228);
}

static void sub_808A2DC(u8 taskId)
{
    u8 mail = GetMonData(&gPlayerParty[GetCursorSelectionMonId(taskId)], MON_DATA_MAIL);
    DestroyTask(taskId);
    sub_80E62A0(4, gSaveBlock1.mail[mail].words, CB2_ReturnToPartyMenuFromWritingMail, 3);
}

static void sub_808A330(u8 taskId)
{
    PartyMenuTryGiveMonHeldItem(taskId, gSpecialVar_ItemId, sub_808A2DC);
}

static void sub_808A34C(void)
{
    RunTasks();
}

static void sub_808A358(void)
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
        if (sub_80F9344() == TRUE)
            break;
    }
}

static void sub_808A3A4(void)
{
    while (1)
    {
        if (InitPartyMenu() == TRUE)
        {
            sub_806C994(ePartyMenu2.menuHandlerTaskId, gLastFieldPokeMenuOpened);
            ChangePartyMenuSelection(ePartyMenu2.menuHandlerTaskId, 0);
            ePartyMenu2.unk262 = 3;
            sub_8089E84();
            SetMainCallback2(CB2_PartyMenuMain);
            break;
        }
        if (sub_80F9344() == TRUE)
            break;
    }
}

void sub_808A3F8(void)
{
    if (ItemIsMail(gSpecialVar_ItemId))
    {
        u8 taskId = CreateTask(sub_808A330, 0);
        gPaletteFade.bufferTransferDisabled = 1;
        sub_806BD58(taskId, 0);
        sub_806C994(taskId, gLastFieldPokeMenuOpened);
        ChangePartyMenuSelection(taskId, 0);
        if (!(bool8)(GetMonData(&gPlayerParty[GetCursorSelectionMonId(taskId)], MON_DATA_HELD_ITEM)))
        {
            SetMainCallback2(sub_808A34C);
            return;
        }
        else
            DestroyTask(taskId);
    }
    gPaletteFade.bufferTransferDisabled = 1;
    if (gSpecialVar_ItemId)
    {
        SetPartyMenuSettings(PARTY_MENU_TYPE_STANDARD, 0xFF, sub_808A2AC, 0xFF);
        SetMainCallback2(sub_808A358);
    }
    else
    {
        SetPartyMenuSettings(PARTY_MENU_TYPE_STANDARD, 0xFF, Task_HandleSelectionMenuInput, 5);
        SetMainCallback2(sub_808A3A4);
    }
}

static void sub_808A4D4(void)
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
        if (sub_80F9344() == TRUE)
            break;
    }
}

static void CB2_ReturnToPartyMenuFromWritingMail(void)
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
        SetPartyMenuSettings(PARTY_MENU_TYPE_STANDARD, 0xFF, Task_DisplayGaveMailFromPartyMessage, 0xFF);
    SetMainCallback2(sub_808A4D4);
}

static void Task_DisplayGaveMailFromPartyMessage(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        DisplayGiveHeldItemMessage(gLastFieldPokeMenuOpened, gSpecialVar_ItemId, 0);
        gTasks[taskId].func = Task_UpdateHeldItemSprite;
    }
}

static void sub_808A604(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sub_80A61D0);
        DestroyTask(taskId);
    }
}

static void CursorCb_Give(u8 taskId)
{
    gSelectedMonPartyId = GetCursorSelectionMonId(taskId);
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = sub_808A604;
}

static void sub_808A678(u8 taskId)
{
    sub_808A8D4(taskId);
}

static void CursorCb_TakeItem(u8 taskId)
{
    Menu_DestroyCursor();
    Menu_EraseWindowRect(19, 0, 29, 19);
    PartyMenuEraseMsgBoxAndFrame();
    PartyMenuTryGiveMonHeldItem_806ECE8(taskId, sub_808A678);
}

static void CursorCb_TakeMail(u8 taskId)
{
    Menu_DestroyCursor();
    Menu_EraseWindowRect(19, 0, 29, 19);
    PartyMenuEraseMsgBoxAndFrame();
    DoTakeMail(taskId, sub_808A678);
}

static void CursorCb_Mail(u8 taskId)
{
    Menu_DestroyCursor();
    sPokeMenuCursorPos = 0;
    Menu_EraseWindowRect(19, 0, 29, 19);
    ShowPartyPopupMenu(0, &sUnknown_0839F584, (void*) sCursorOptions, 0);
    gTasks[taskId].func = sub_808A73C;
}

static void sub_808A73C(u8 taskId)
{
    if (gMain.newAndRepeatedKeys == DPAD_UP)
    {
        PlaySE(SE_SELECT);
        if (sPokeMenuCursorPos == 0)
            sPokeMenuCursorPos = Menu_MoveCursor(sUnknown_0839F584.numChoices - 1);
        else
            sPokeMenuCursorPos = Menu_MoveCursor(-1);
    }
    if (gMain.newAndRepeatedKeys == DPAD_DOWN)
    {
        PlaySE(SE_SELECT);
        if (sPokeMenuCursorPos == sUnknown_0839F584.numChoices - 1)
            sPokeMenuCursorPos = Menu_MoveCursor(1 - sUnknown_0839F584.numChoices);
        else
            sPokeMenuCursorPos = Menu_MoveCursor(1);
    }
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        PartyMenuGetPopupMenuFunc(0, &sUnknown_0839F584, sCursorOptions, sPokeMenuCursorPos)(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        ClosePartyPopupMenu(0, &sUnknown_0839F584);
        CursorCb_Cancel1(taskId);
    }
}

static void CursorCb_Read(u8 taskId)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = sub_808A848;
}

static void sub_808A848(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        u8 mail = GetMonData(&gPlayerParty[GetCursorSelectionMonId(taskId)], MON_DATA_MAIL);
        DestroyTask(taskId);
        HandleReadMail(&gSaveBlock1.mail[mail], CB2_ReturnToPartyMenuFromReadingMail, 1);
    }
}

static void CB2_ReturnToPartyMenuFromReadingMail(void)
{
    gLastViewedMonIndex = gLastFieldPokeMenuOpened;
    ePartyMenu2.unk262 = 4;
    CB2_ReturnToPartyMenuFromSummaryScreen();
}

static void sub_808A8D4(u8 taskId)
{
    sPokeMenuCursorPos = 0;
    Menu_EraseWindowRect(19, 0, 29, 19);
    PrintPartyMenuPromptText(0, 0);
    gTasks[taskId].func = HandleDefaultPartyMenu;
}

static void CursorCb_Cancel1(u8 taskId)
{
    Menu_DestroyCursor();
    PlaySE(SE_SELECT);
    sub_808A8D4(taskId);
}

static void CursorCb_Cancel2(u8 taskId)
{
    Menu_DestroyCursor();
    PlaySE(SE_SELECT);
    Menu_EraseWindowRect(19, 0, 29, 19);
    PartyMenuEraseMsgBoxAndFrame();
    sub_8089C7C(sPokeMenuCursorPos);
    gTasks[taskId].func = Task_HandleSelectionMenuInput;
}

#define IS_SOFTBOILED_MILKDRINK(ID)((ID == (POKEMENU_MILK_DRINK - MENU_FIELD_MOVES) || ID == (POKEMENU_SOFT_BOILED - MENU_FIELD_MOVES)))
#define IS_SURF(ID)((ID == (POKEMENU_SURF - MENU_FIELD_MOVES)))
#define IS_FLY(ID)((ID == (POKEMENU_FLY - MENU_FIELD_MOVES)))

#define tFieldMoveId data[11]

static void CursorCb_FieldMove(u8 taskId)
{
    s16* data = gTasks[taskId].data;

    Menu_DestroyCursor();
    tFieldMoveId = sPokeMenuOptionsOrder[sPokeMenuCursorPos] - MENU_FIELD_MOVES;
    if (sub_80F9344() == TRUE)
    {
        Menu_EraseWindowRect(19, 0, 29, 19);
        if (IS_SOFTBOILED_MILKDRINK(tFieldMoveId))
            PrintPartyMenuPromptText(9, 0);
        else
            PrintPartyMenuPromptText(sFieldMoveCursorCallbacks[tFieldMoveId].msgId, 0);
        gTasks[taskId].func = Task_CancelAfterAorBPress;
    }
    else if (tFieldMoveId <= 7 && FlagGet(FLAG_BADGE01_GET + tFieldMoveId) != TRUE)
    {
        // can't use a field HM move without a proper badge
        Menu_EraseWindowRect(19, 0, 29, 19);
        PartyMenuEraseMsgBoxAndFrame();
        DisplayPartyMenuMessage(gText_CantUseUntilNewBadge, 1);
        gTasks[taskId].func = sub_808AAF0;
    }
    else
    {
        if (sFieldMoveCursorCallbacks[tFieldMoveId].fieldMoveFunc() == TRUE)
        {
            sPokeMenuCursorPos = 0;
            if (!IS_SOFTBOILED_MILKDRINK(tFieldMoveId))
            {
                gTasks[taskId].func = sub_808AB34;
                BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            }
            else
                sub_8133D28(taskId);
        }
        else
        {
            Menu_EraseWindowRect(19, 0, 29, 19);
            if (IS_SURF(tFieldMoveId) && TestPlayerAvatarFlags(8))
                PrintPartyMenuPromptText(8, 0);
            else
                PrintPartyMenuPromptText(sFieldMoveCursorCallbacks[tFieldMoveId].msgId, 0);
            gTasks[taskId].func = Task_CancelAfterAorBPress;
        }
    }
}

static void sub_808AAF0(u8 taskId)
{
    if (gPartyMenuMessage_IsPrinting != 1 && (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON)))
    {
        Menu_EraseWindowRect(0, 14, 29, 19);
        CursorCb_Cancel1(taskId);
    }
}

static void sub_808AB34(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        if (!IS_FLY(gTasks[taskId].tFieldMoveId) || ShouldDoBrailleFlyEffect())
            SetMainCallback2(CB2_ReturnToField);
        else
            SetMainCallback2(CB2_InitFlyRegionMap);
        DestroyTask(taskId);
    }
}

#undef tFieldMoveId

void FieldCallback_PrepareFadeInFromMenu(void)
{
    FadeInFromBlack();
    CreateTask(Task_FieldMoveWaitForFade, 8);
}

static void Task_FieldMoveWaitForFade(u8 taskId)
{
    if (IsWeatherNotFadingIn() == TRUE)
    {
        gFieldEffectArguments[0] = GetMonData(&gPlayerParty[gLastFieldPokeMenuOpened], MON_DATA_SPECIES);
        gPostMenuFieldCallback();
        DestroyTask(taskId);
    }
}

static void Task_CancelAfterAorBPress(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        Menu_EraseWindowRect(1, 17, 28, 18);
        CursorCb_Cancel1(taskId);
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

static void sub_808AC8C(void)
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
        gPostMenuFieldCallback = sub_808AC8C;
        return TRUE;
    }
    return FALSE;
}

static void sub_808AD0C(void)
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
        if (sub_80F9344() == TRUE)
            break;
    }
}

void CB2_ReturnToPartyMenuFromFlyMap(void)
{
    gPaletteFade.bufferTransferDisabled = 1;
    SetPartyMenuSettings(PARTY_MENU_TYPE_STANDARD, 0xFF, HandleDefaultPartyMenu, 0);
    SetMainCallback2(sub_808AD0C);
}

static u16 UNUSED GetFieldMoveMonSpecies(void)
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
void debug_sub_80986AC(void)
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

static void sub_808AE8C(void)
{
    u8 i;
    u8 arg = gSpecialVar_ItemId - 33;
    for (i = 0; i < 6; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES))
        {
            sub_806D668(i);
            if (
#if DEBUG
             gUnknown_020297ED == 0 &&
#endif
             (GetMonData(&gPlayerParty[i], MON_DATA_IS_EGG) || !CanMonLearnTMHM(&gPlayerParty[i], arg)))
                DrawMonDescriptorStatus(i, 0x9A);
            else if (MonKnowsMove(&gPlayerParty[i], ItemIdToBattleMoveId(gSpecialVar_ItemId)))
                DrawMonDescriptorStatus(i, 0xA8);
            else
                DrawMonDescriptorStatus(i, 0x8C);
        }
    }
}

static void sub_808AF20(void)
{
    u8 i;
    for (i = 0; i < 6; i++)
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

static void sub_808AF80(void)
{
    while (1)
    {
        if (InitPartyMenu() == TRUE)
        {
            if (gPokemonItemUseType == ITEM_USE_SINGLE_MON)
            {
                switch (CheckIfItemIsTMHMOrEvolutionStone(gSpecialVar_ItemId))
                {
                case 1:
                    sub_808AE8C();
                    break;
                case 2:
                    sub_808AF20();
                    break;
                }
            }
            if (gLastFieldPokeMenuOpened > 5 || !GetMonData(&gPlayerParty[gLastFieldPokeMenuOpened], MON_DATA_SPECIES))
                gLastFieldPokeMenuOpened = 0;
            sub_806C994(ePartyMenu2.menuHandlerTaskId, gLastFieldPokeMenuOpened);
            ChangePartyMenuSelection(ePartyMenu2.menuHandlerTaskId, 0);
            SetMainCallback2(CB2_PartyMenuMain);
            break;
        }
        if (sub_80F9344() == TRUE)
            break;
    }
}

void CB2_ShowPartyMenuForItemUse(void)
{
    gPaletteFade.bufferTransferDisabled = 1;
    switch (gPokemonItemUseType)
    {
    case ITEM_USE_SINGLE_MON:
        if (CheckIfItemIsTMHMOrEvolutionStone(gSpecialVar_ItemId) == 1)
            SetPartyMenuSettings(PARTY_MENU_TYPE_STANDARD, 0, sub_808B0C0, 20);
        else
            SetPartyMenuSettings(PARTY_MENU_TYPE_STANDARD, 0, sub_808B0C0, 3);
        break;
    case ITEM_USE_ALL_MONS:
        SetPartyMenuSettings(PARTY_MENU_TYPE_STANDARD, 0, sub_808B1EC, 0xFF);
        break;
    case ITEM_USE_GIVE_ITEM:
    case ITEM_USE_GIVE_MAIL:
        SetPartyMenuSettings(PARTY_MENU_TYPE_STANDARD, 0, sub_808B0C0, 4);
        break;
    }
    SetMainCallback2(sub_808AF80);
}

void sub_808B0C0(u8 taskId)
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
                    gPokemonItemUseCallback(taskId, gSpecialVar_ItemId, sub_808B224);
                if (gPokemonItemUseType == ITEM_USE_GIVE_ITEM)
                {
                    PlaySE(SE_SELECT);
                    PartyMenuTryGiveMonHeldItem(taskId, gSpecialVar_ItemId, sub_808B2EC);
                }
                if (gPokemonItemUseType == ITEM_USE_GIVE_MAIL)
                {
                    PlaySE(SE_SELECT);
                    PartyMenuTryGiveMonMail(taskId, sub_808B2B4);
                }
            }
            break;
        case B_BUTTON:
            gLastFieldPokeMenuOpened = GetCursorSelectionMonId(taskId);
            PlaySE(SE_SELECT);
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            if (gPokemonItemUseType == ITEM_USE_SINGLE_MON || gPokemonItemUseType == ITEM_USE_GIVE_ITEM)
                gTasks[taskId].func = sub_808B25C;
            if (gPokemonItemUseType == ITEM_USE_GIVE_MAIL)
                gTasks[taskId].func = sub_808B2B4;
            break;
        }
    }
}

static void sub_808B1EC(u8 taskId)
{
    if (!gPaletteFade.active)
        gPokemonItemUseCallback(taskId, gSpecialVar_ItemId, sub_808B224);
}

static void sub_808B224(u8 taskId)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = sub_808B25C;
}

static void sub_808B25C(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sub_80A5B40);
        DestroyTask(taskId);
    }
}

static void sub_808B288(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(Mailbox_ReturnToMailListAfterDeposit);
        DestroyTask(taskId);
    }
}

static void sub_808B2B4(u8 taskId)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = sub_808B288;
}

static void sub_808B2EC(u8 taskId)
{
    if (gUnknown_0202E8F4 == 2)
    {
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        gTasks[taskId].func = sub_808B338;
    }
    else
        sub_808B224(taskId);
}

static void sub_808B338(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        u8 mail;

        gLastFieldPokeMenuOpened = GetCursorSelectionMonId(taskId);
        mail = GetMonData(&gPlayerParty[gLastFieldPokeMenuOpened], MON_DATA_MAIL);
        DestroyTask(taskId);
        sub_80E62A0(4, gSaveBlock1.mail[mail].words, CB2_ReturnToPartyOrBagMenuFromWritingMail, 3);
    }
}

static void sub_808B3A0(void)
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
        if (sub_80F9344() == TRUE)
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
        CreateTask(sub_808B25C, 5);
        gPaletteFade.bufferTransferDisabled = 0;
        callback = CB2_PartyMenuMain;
    }
    else
    {
        SetPartyMenuSettings(PARTY_MENU_TYPE_STANDARD, 0, Task_DisplayGaveMailFromBagMessage, 0xFF);
        callback = sub_808B3A0;
    }
    SetMainCallback2(callback);
}

static void Task_DisplayGaveMailFromBagMessage(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        DisplayGiveHeldItemMessage(gLastFieldPokeMenuOpened, gSpecialVar_ItemId, 1);
        gTasks[taskId].func = sub_808B4EC;
    }
}

static void sub_808B4EC(u8 taskId)
{
    if (gPartyMenuMessage_IsPrinting != 1)
        sub_808B224(taskId);
}

void sub_808B508(u8 taskId)
{
    sub_808B224(taskId);
}

static void sub_808B518(void)
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
        if (sub_80F9344() == TRUE)
            break;
    }
}

void CB2_ReturnToPartyMenuWhileLearningMove(void)
{
    gPaletteFade.bufferTransferDisabled = 1;
    if (GetMoveSlotToReplace() != 4)
        SetPartyMenuSettings(PARTY_MENU_TYPE_STANDARD, 0, TaughtMove, 0xFF);
    else
        SetPartyMenuSettings(PARTY_MENU_TYPE_STANDARD, 0, StopTryingToTeachMove_806F588, 0xFF);
    SetMainCallback2(sub_808B518);
}

void sub_808B5B4(u32 taskId)
{
    gUnknown_03005CF0 = gTasks[taskId].func;
    gTasks[taskId].func = sub_808B5E4;
    sub_808B5E4(taskId);
}

static void sub_808B5E4(u8 taskId)
{
    if (sub_8055870() != TRUE)
        gTasks[taskId].func = gUnknown_03005CF0;
}
