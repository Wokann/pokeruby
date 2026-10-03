#include "global.h"
#include "player_pc.h"
#include "decoration.h"
#include "field_fadetransition.h"
#include "field_weather.h"
#include "item.h"
#include "item_menu.h"
#include "constants/items.h"
#include "main.h"
#include "menu.h"
#include "menu_helpers.h"
#include "palette.h"
#include "pokemon_menu.h"
#include "script.h"
#include "sound.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "constants/songs.h"
#include "international_string_util.h"
#include "mail.h"
#include "overworld.h"
#include "ewram.h"

enum
{
    MENU_ITEMSTORAGE,
    MENU_MAILBOX,
    MENU_DECORATION,
    MENU_TURNOFF
};

enum
{
    MENU_WITHDRAW,
    MENU_DEPOSIT,
    MENU_TOSS,
    MENU_EXIT
};

enum
{
    MSG_SWITCH_WHICH_ITEM = 0xFFF7,
    MSG_OKAY_TO_THROW_AWAY,
    MSG_TOO_IMPORTANT,
    MSG_NO_MORE_ROOM,
    MSG_THREW_AWAY_ITEM,
    MSG_HOW_MANY_TO_TOSS,
    MSG_WITHDREW_ITEM,
    MSG_HOW_MANY_TO_WITHDRAW,
    MSG_GO_BACK_TO_PREV
};

#define tItemStorageCursorPos   data[0]
#define tItemStorageItemsAbove  data[1]
#define tItemStorageItemCount   data[2]
#define tItemStorageQuantity    data[3]
#define tItemStoragePageItems   data[4]
#define tItemStorageMenuAction  data[6]
#define tItemStorageSwapItemPos data[8]
#define tItemStorageIsSwitching data[9]

// Ruby's matching layout requires a linear ItemSlot view of this table.
#define GET_NEW_GAME_PC_ITEM(i, field) ((u16)((u16 *)sNewGamePCItems + field)[i * 2])

enum
{
    PC_ITEM_ID,
    PC_QUANTITY
};

static EWRAM_DATA const u8 *sTopMenuOptionOrder = NULL;

static u8 sTopMenuNumOptions;

// event scripts
extern u8 LittlerootTown_BrendansHouse_2F_EventScript_TurnOffPlayerPC[];
extern u8 LittlerootTown_MaysHouse_2F_EventScript_TurnOffPlayerPC[];

static void InitPlayerPCMenu(u8 taskId);
static void PlayerPCProcessMenuInput(u8 taskId);
static void InitItemStorageMenu(u8 menuOption);
static void ItemStorageMenuPrint(const u8 *);
static void ItemStorageMenuProcessInput(u8 taskId);
static void ItemStorage_ProcessInput(u8);
static void SetPlayerPCListCount(u8);
static void ItemStorage_DoItemAction(u8);
static void ItemStorage_ExitItemList(u8);
static void ItemStorage_HandleQuantityRolling(u8);
static void ItemStorage_DoItemWithdraw(u8);
static void ItemStorage_DoItemToss(u8);
static void ItemStorage_HandleRemoveItem(u8);
static void ItemStorage_HandleErrorMessageInput(u8);
static void ItemStorage_ReturnToListInput(u8);
static void ItemStorage_FinishItemSwap(u8, bool8);
static void ItemStorage_DrawItemList(u8);
static void ItemStorage_PrintDescription(u16);
static void ItemStorage_DrawBothListAndDescription(u8);
static void ItemStorage_DrawItemListMenu(u8, u8);
static void ItemStorage_LoadPalette(void);
static u8 GetMailboxMailCount(void);
static void Mailbox_CompactMailList(void);
static void Mailbox_DrawMailboxMenu(u8);
static void Mailbox_ProcessInput(u8);
static void Mailbox_CloseScrollIndicators(void);
static void Mailbox_PrintWhatToDoWithPlayerMailText(u8);
static void Mailbox_ExitToPlayerPC(u8);
static void Mailbox_PrintMailOptions(u8);
static void Mailbox_MailOptionsProcessInput(u8);
static void Mailbox_FadeAndReadMail(u8);
static void Mailbox_ReturnToFieldFromReadMail(void);
static void Mailbox_AskConfirmMoveToBag(u8);
static void Mailbox_DoGiveMailPokeMenu(u8);
static void Mailbox_NoPokemonForMail(u8);
static void Mailbox_Cancel(u8);
static void Mailbox_DrawMailMenuAndDoProcessInput(u8);
static void PlayerPC_ItemStorage(u8 taskId);
static void PlayerPC_Mailbox(u8 taskId);
static void PlayerPC_Decoration(u8 taskId);
static void PlayerPC_TurnOff(u8 taskId);
static void ItemStorage_Withdraw(u8);
static void ItemStorage_Deposit(u8);
static void ItemStorage_Toss(u8);
static void ItemStorage_Exit(u8 taskId);
static void ItemStorage_TossItemYes(u8);
static void ItemStorage_TossItemNo(u8);
static void Mailbox_DoMailMoveToBag(u8);
static void Mailbox_CancelMoveToBag(u8);
static void Mailbox_DoMailRead(u8);
static void Mailbox_MoveToBag(u8);
static void Mailbox_Give(u8);
static void Mailbox_Cancel(u8);

static const u8 *const sItemStorage_OptionDescriptions[] =
{
    gText_TakeOutItemsFromPC,
    gText_StoreItemsInPC,
    gText_ThrowAwayItemsInPC,
    gText_GoBackPrevMenu
};

static const struct MenuAction2 sPlayerPCMenuActions[] =
{
    { gText_ItemStorage, PlayerPC_ItemStorage },
    { gText_Mailbox, PlayerPC_Mailbox },
    { gText_Decoration, PlayerPC_Decoration },
    { gText_TurnOff, PlayerPC_TurnOff }
};

static const u8 sBedroomPC_OptionOrder[] =
{
    MENU_ITEMSTORAGE,
    MENU_MAILBOX,
    MENU_DECORATION,
    MENU_TURNOFF
};

static const u8 sPlayerPC_OptionOrder[] =
{
    MENU_ITEMSTORAGE,
    MENU_MAILBOX,
    MENU_TURNOFF
};

static const struct MenuAction2 sItemStorage_MenuActions[] =
{
    { gText_WithdrawItem, ItemStorage_Withdraw },
    { gText_DepositItem, ItemStorage_Deposit },
    { gText_TossItem, ItemStorage_Toss },
    { gText_Exit, ItemStorage_Exit }
};

static const struct YesNoFuncTable sItemTossYesNoFuncs[] =
{
    ItemStorage_TossItemYes,
    ItemStorage_TossItemNo
};

static const struct YesNoFuncTable sMailboxMoveToBagYesNoFuncs[] =
{
    Mailbox_DoMailMoveToBag,
    Mailbox_CancelMoveToBag
};

// the use of this struct is meant to be an ItemSlot struct, but NewGameInitPCItems refuses to match without a weird pointer access.
static const struct ItemSlot sNewGamePCItems[] =
{
    { ITEM_POTION, 1 },
    { ITEM_NONE, 0 }
};

static const struct MenuAction2 sMailboxMailOptions[] =
{
    { gText_Read, Mailbox_DoMailRead },
    { gText_MoveToBag, Mailbox_MoveToBag },
    { gText_Give2, Mailbox_Give },
    { gText_Cancel, Mailbox_Cancel }
};

static const u8 sUnselectedItemNameText[] = _("{STR_VAR_1}{CLEAR_TO 80}");
static const u8 sSelectedItemNameText[] = _("{COLOR RED}{STR_VAR_1}{CLEAR_TO 80}");
static const u8 sUnselectedItemQuantityText[] = _("{STR_VAR_1}");
static const u8 sSelectedItemQuantityText[] = _("{COLOR RED}{STR_VAR_1}");
static const u8 sEmptyItemQuantityText[] = _("{CLEAR_TO 32}");

void NewGameInitPCItems(void)
{
    u8 i;

    // because Game Freak don't know how to use a struct or a 2d array
    for(i = 0, ClearItemSlots(gSaveBlock1.pcItems, ARRAY_COUNT(gSaveBlock1.pcItems)); GET_NEW_GAME_PC_ITEM(i, PC_ITEM_ID) && GET_NEW_GAME_PC_ITEM(i, PC_QUANTITY) &&
        AddPCItem(GET_NEW_GAME_PC_ITEM(i, PC_ITEM_ID), GET_NEW_GAME_PC_ITEM(i, PC_QUANTITY)) == TRUE; i++)
            ;
}

void BedroomPC(void)
{
    sTopMenuOptionOrder = sBedroomPC_OptionOrder;
    sTopMenuNumOptions = 4;
    DisplayItemMessageOnField(CreateTask(TaskDummy, 0), gText_WhatWouldYouLike, InitPlayerPCMenu, 0);
}

void PlayerPC(void)
{
    sTopMenuOptionOrder = sPlayerPC_OptionOrder;
    sTopMenuNumOptions = 3;
    DisplayItemMessageOnField(CreateTask(TaskDummy, 0), gText_WhatWouldYouLike, InitPlayerPCMenu, 0);
}

static void InitPlayerPCMenu(u8 taskId)
{
    Menu_DrawStdWindowFrame(0, 0, 10, sTopMenuNumOptions * 2 + 1);
    Menu_PrintItemsReordered(1, 1, sTopMenuNumOptions, sPlayerPCMenuActions, sTopMenuOptionOrder);
    InitMenu(0, 1, 1, sTopMenuNumOptions, 0, 9);
    TASK.FUNC = PlayerPCProcessMenuInput;
}

static void PlayerPCProcessMenuInput(u8 taskId)
{
    if (JOY_REPT(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        Menu_MoveCursor(-1);
    }
    else if (JOY_REPT(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        Menu_MoveCursor(1);
    }
    else if (JOY_NEW(A_BUTTON))
    {
        Menu_DestroyCursor();
        PlaySE(SE_SELECT);
        sPlayerPCMenuActions[sTopMenuOptionOrder[Menu_GetCursorPos()]].func(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        Menu_DestroyCursor();
        PlaySE(SE_SELECT);
        sPlayerPCMenuActions[sTopMenuNumOptions[sTopMenuOptionOrder - 1]].func(taskId); // run EXIT.
    }
}

void ReshowPlayerPC(u8 taskId)
{
    DisplayItemMessageOnField(taskId, gText_WhatWouldYouLike, InitPlayerPCMenu, 0);
}

static void PlayerPC_ItemStorage(u8 taskId)
{
    InitItemStorageMenu(MENU_WITHDRAW);
    TASK.FUNC = ItemStorageMenuProcessInput;
}

static void PlayerPC_Mailbox(u8 taskId)
{
    Menu_EraseWindowRect(0, 0, 10, 9);
    gPlayerPCItemPageInfo.count = GetMailboxMailCount();

    if (gPlayerPCItemPageInfo.count == 0)
        DisplayItemMessageOnField(taskId, gText_NoMailHere, ReshowPlayerPC, 0);
    else
    {
        gPlayerPCItemPageInfo.cursorPos = 0;
        gPlayerPCItemPageInfo.itemsAbove = 0;
        Mailbox_CompactMailList();
        SetPlayerPCListCount(taskId);
        Mailbox_DrawMailboxMenu(taskId);
        TASK.FUNC = Mailbox_ProcessInput;
    }
}

static void PlayerPC_Decoration(u8 taskId)
{
    Menu_EraseWindowRect(0, 0, 10, 9);
    DoPlayerRoomDecorationMenu(taskId);
}

static void PlayerPC_TurnOff(u8 taskId)
{
    if (sTopMenuNumOptions == 4) // if the option count is 4, we are at the bedroom PC and not player PC, so do gender specific handling.
    {
        Menu_EraseWindowRect(0, 0, 0x1D, 0x13);

        if (gSaveBlock2.playerGender == MALE)
            ScriptContext_SetupScript(LittlerootTown_BrendansHouse_2F_EventScript_TurnOffPlayerPC);
        else
            ScriptContext_SetupScript(LittlerootTown_MaysHouse_2F_EventScript_TurnOffPlayerPC);
    }
    else
    {
        Menu_EraseWindowRect(0, 0, 10, 9);
        ScriptContext_Enable();
    }
    DestroyTask(taskId);
}

static void InitItemStorageMenu(u8 menuOption)
{
    Menu_EraseWindowRect(0, 0, 10, 9);
    Menu_DrawStdWindowFrame(0, 0, 11, 9);
    Menu_PrintItems(1, 1, 4, (struct MenuAction *)sItemStorage_MenuActions);
    InitMenu(0, 1, 1, 4, menuOption, 10);
    ItemStorageMenuPrint(sItemStorage_OptionDescriptions[menuOption]);
}

static void ItemStorageMenuPrint(const u8 *textPtr)
{
    Menu_BlankWindowRect(2, 15, 27, 18);
    Menu_PrintText(textPtr, 2, 15);
}

static void ItemStorageMenuProcessInput(u8 taskId)
{
    if (JOY_REPT(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        Menu_MoveCursor(-1);
        ItemStorageMenuPrint(sItemStorage_OptionDescriptions[Menu_GetCursorPos()]);
    }
    else if (JOY_REPT(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        Menu_MoveCursor(1);
        ItemStorageMenuPrint(sItemStorage_OptionDescriptions[Menu_GetCursorPos()]);
    }
    else if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        sItemStorage_MenuActions[Menu_GetCursorPos()].func(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        Menu_DestroyCursor();
        PlaySE(SE_SELECT);
        sItemStorage_MenuActions[MENU_EXIT].func(taskId);
    }
}

static void Task_ItemStorage_Deposit(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        CB2_GoToItemDepositMenu();
        DestroyTask(taskId);
    }
}

static void ItemStorage_Deposit(u8 taskId)
{
    TASK.FUNC = Task_ItemStorage_Deposit;
    FadeScreen(1, 0);
}

static void ItemStorage_HandleReturnToProcessInput(u8 taskId)
{
    if (IsWeatherNotFadingIn() == TRUE)
        TASK.FUNC = ItemStorageMenuProcessInput;
}

void ItemStorage_ReshowAfterBagMenu(void)
{
    Menu_DisplayDialogueFrame();
    InitItemStorageMenu(MENU_DEPOSIT);
    CreateTask(ItemStorage_HandleReturnToProcessInput, 0);
    FadeInFromBlack();
}

static void ItemStorage_Withdraw(u8 taskId)
{
    s16 *data = TASK.data;

    Menu_DestroyCursor();
    Menu_EraseWindowRect(0, 0, 11, 9);
    tItemStorageItemCount = CountUsedPCItemSlots();

    if (tItemStorageItemCount != 0)
    {
        Menu_EraseWindowRect(0, 14, 29, 19);
        tItemStorageMenuAction = MENU_WITHDRAW;
        tItemStorageCursorPos = 0;
        tItemStorageItemsAbove = 0;
        SetPlayerPCListCount(taskId);
        ItemStorage_DrawItemListMenu(taskId, MENU_WITHDRAW);
        TASK.FUNC = ItemStorage_ProcessInput;
    }
    else
        DisplayItemMessageOnField(taskId, gText_NoItems, PlayerPC_ItemStorage, 0);
}

static void ItemStorage_Toss(u8 taskId)
{
    s16 *data = TASK.data;

    Menu_DestroyCursor();
    Menu_EraseWindowRect(0, 0, 11, 9);
    tItemStorageItemCount = CountUsedPCItemSlots();

    if (tItemStorageItemCount)
    {
        Menu_EraseWindowRect(0, 14, 29, 19);
        tItemStorageMenuAction = MENU_TOSS;
        tItemStorageCursorPos = 0;
        tItemStorageItemsAbove = 0;
        SetPlayerPCListCount(taskId);
        ItemStorage_DrawItemListMenu(taskId, MENU_TOSS);
        TASK.FUNC = ItemStorage_ProcessInput;
    }
    else
        DisplayItemMessageOnField(taskId, gText_NoItems, PlayerPC_ItemStorage, 0);
}

static void ItemStorage_Exit(u8 taskId)
{
    Menu_DestroyCursor();
    Menu_EraseWindowRect(0, 0, 11, 9);
    ReshowPlayerPC(taskId);
}

static void SetPlayerPCListCount(u8 taskId)
{
    s16 *data = TASK.data;

    if (tItemStorageItemCount > 7) // we have a full page, so set the num of page items appropriately.
        tItemStoragePageItems = 8;
    else
        tItemStoragePageItems = tItemStorageItemCount + 1; // there are not enough items to fill a full page; take the # of items and add 1 for the cancel button.

    if (gPlayerPCItemPageInfo.count > 7)
        gPlayerPCItemPageInfo.pageItems = 8;
    else
        gPlayerPCItemPageInfo.pageItems = gPlayerPCItemPageInfo.count + 1;
}

static void ItemStorage_ProcessInput(u8 taskId)
{
    s16 *data = TASK.data;
    s16 trueIndex;

    if (JOY_REPT(DPAD_UP))
    {
        if(tItemStorageCursorPos != 0) // did the cursor move physically upwards?
        {
            PlaySE(SE_SELECT);
            tItemStorageCursorPos = Menu_MoveCursor(-1);
            trueIndex = tItemStorageItemsAbove + tItemStorageCursorPos;
            if (tItemStorageIsSwitching == FALSE) // are we not currently switching items?
            {
                if (trueIndex == tItemStorageItemCount) // if the cursor is on top of cancel, print the go back to prev description.
                {
                    ItemStorage_PrintDescription(MSG_GO_BACK_TO_PREV);
                }
                else
                {
                    ItemStorage_PrintDescription(gSaveBlock1.pcItems[trueIndex].itemId);
                }
            }
        }
        else // the page cursor is at the top. but we may not be at the top of the true index list, so do another check.
        {
            if (tItemStorageItemsAbove == 0) // did the cursor not move due to being at the top of the list?
                return;

            PlaySE(SE_SELECT);
            tItemStorageItemsAbove--;
            ItemStorage_DrawBothListAndDescription(taskId);

            if (tItemStorageIsSwitching != FALSE)
                Menu_MoveCursor(0); // don't move the cursor. it's at the top of the page index, but not the true index.
        }
    }
    else if (JOY_REPT(DPAD_DOWN)) // _0813A306
    {
        if(tItemStorageCursorPos != tItemStoragePageItems - 1)
        {
            PlaySE(SE_SELECT);
            tItemStorageCursorPos = Menu_MoveCursor(1);
            trueIndex = tItemStorageItemsAbove + tItemStorageCursorPos;

            if(tItemStorageIsSwitching != FALSE)
                return;

            if (trueIndex == tItemStorageItemCount)
                ItemStorage_PrintDescription(MSG_GO_BACK_TO_PREV); // probably further down
            else
                ItemStorage_PrintDescription(gSaveBlock1.pcItems[trueIndex].itemId);
        }
        else if(tItemStorageItemsAbove + tItemStorageCursorPos != tItemStorageItemCount)
        {
            PlaySE(SE_SELECT);
            tItemStorageItemsAbove++;
            ItemStorage_DrawBothListAndDescription(taskId);

            if (tItemStorageIsSwitching != FALSE)
                Menu_MoveCursor(0);
        }
    }
    else if (JOY_NEW(SELECT_BUTTON)) // _0813A3A0
    {
        if (tItemStorageIsSwitching == FALSE)
        {
            if (tItemStorageCursorPos + tItemStorageItemsAbove != tItemStorageItemCount) // you cannot swap the Cancel button.
            {
                PlaySE(SE_SELECT);
                tItemStorageIsSwitching = TRUE;
                tItemStorageSwapItemPos = tItemStorageItemsAbove + tItemStorageCursorPos;
                ItemStorage_PrintDescription(MSG_SWITCH_WHICH_ITEM);
            }
            // _0813A3DC
            ItemStorage_DrawItemList(taskId);
        }
        else // _0813A3E8
        {
            PlaySE(SE_SELECT);
            ItemStorage_FinishItemSwap(taskId, FALSE);
            ItemStorage_DrawBothListAndDescription(taskId);
        }
    }
    else if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if(tItemStorageIsSwitching == FALSE)
        {
            if(tItemStorageItemsAbove + tItemStorageCursorPos != tItemStorageItemCount)
            {
                ItemStorage_DoItemAction(taskId);
            }
            else
            {
                ItemStorage_ExitItemList(taskId);
            }
        }
        else
        {
            ItemStorage_FinishItemSwap(taskId, FALSE);
            ItemStorage_DrawBothListAndDescription(taskId);
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        if(tItemStorageIsSwitching == FALSE)
        {
            Menu_DestroyCursor();
            ItemStorage_ExitItemList(taskId);
        }
        else
        {
            ItemStorage_FinishItemSwap(taskId, TRUE);
            ItemStorage_DrawBothListAndDescription(taskId);
        }
    }
}

static void ItemStorage_ExitItemList(u8 taskId)
{
    FreeScrollIndicatorPalette();
    DestroyScrollIndicatorArrow(TOP_ARROW);
    DestroyScrollIndicatorArrow(BOTTOM_ARROW);
    Menu_EraseWindowRect(0, 0, 29, 19);
    Menu_DisplayDialogueFrame();
    InitItemStorageMenu(TASK.tItemStorageMenuAction);
    TASK.FUNC = ItemStorageMenuProcessInput;
}

static void ItemStorage_DoItemAction(u8 taskId)
{
    s16 *data = TASK.data;
    u8 trueIndex = tItemStorageCursorPos + tItemStorageItemsAbove;

    PauseScrollIndicatorArrow(TOP_ARROW);
    PauseScrollIndicatorArrow(BOTTOM_ARROW);

    if(tItemStorageMenuAction == MENU_WITHDRAW)
    {
        if(gSaveBlock1.pcItems[trueIndex].quantity == 1)
        {
            tItemStorageQuantity = 1;
            ItemStorage_DoItemWithdraw(taskId);
            return;
        }
        else // _0813A50C
        {
            ItemStorage_PrintDescription(MSG_HOW_MANY_TO_WITHDRAW);
        }
    }
    else if(gSaveBlock1.pcItems[trueIndex].quantity == 1) // _0813A518
    {
        tItemStorageQuantity = 1;
        ItemStorage_DoItemToss(taskId);
        return;
    }
    else
    {
        ItemStorage_PrintDescription(MSG_HOW_MANY_TO_TOSS);
    }
    tItemStorageQuantity = 1;
    Menu_DrawStdWindowFrame(6, 8, 13, 11);
    PrintItemQuantity(tItemStorageQuantity, STR_CONV_MODE_RIGHT_ALIGN, 8, 9, 3);
    TASK.FUNC = ItemStorage_HandleQuantityRolling;
}

static void ItemStorage_HandleQuantityRolling(u8 taskId)
{
    s16 *data = TASK.data;
    u8 trueIndex = tItemStorageCursorPos + tItemStorageItemsAbove;

    if (JOY_REPT(DPAD_UP))
    {
        if(tItemStorageQuantity != gSaveBlock1.pcItems[trueIndex].quantity)
            tItemStorageQuantity++;
        else
            tItemStorageQuantity = 1; // you are at the max amount of items you have when you press Up, set your quantity back to 1.

        PrintItemQuantity(tItemStorageQuantity, STR_CONV_MODE_RIGHT_ALIGN, 8, 9, 3); // print quantity?
    }
    else if (JOY_REPT(DPAD_DOWN))
    {
        if(tItemStorageQuantity != 1)
            tItemStorageQuantity--;
        else
            tItemStorageQuantity = gSaveBlock1.pcItems[trueIndex].quantity; // you are at 0 when you press down, set your quantity to the amount you have.

        PrintItemQuantity(tItemStorageQuantity, STR_CONV_MODE_RIGHT_ALIGN, 8, 9, 3); // print quantity?
    }
    else if (JOY_REPT(DPAD_LEFT)) // reduce by 10.
    {
        tItemStorageQuantity -= 10;

        if(tItemStorageQuantity <= 0)
            tItemStorageQuantity = 1; // dont underflow or allow 0!

        PrintItemQuantity(tItemStorageQuantity, STR_CONV_MODE_RIGHT_ALIGN, 8, 9, 3); // print quantity?
    }
    else if (JOY_REPT(DPAD_RIGHT)) // add 10.
    {
        tItemStorageQuantity += 10;

        if(tItemStorageQuantity > gSaveBlock1.pcItems[trueIndex].quantity)
            tItemStorageQuantity = gSaveBlock1.pcItems[trueIndex].quantity; // dont overflow!

        PrintItemQuantity(tItemStorageQuantity, STR_CONV_MODE_RIGHT_ALIGN, 8, 9, 3); // print quantity?
    }
    else if (JOY_NEW(A_BUTTON)) // confirm quantity.
    {
        PlaySE(SE_SELECT);
        Menu_EraseWindowRect(6, 6, 0xD, 0xB);

        if(tItemStorageMenuAction == MENU_WITHDRAW)
            ItemStorage_DoItemWithdraw(taskId);
        else
            ItemStorage_DoItemToss(taskId);
    }
    else if (JOY_NEW(B_BUTTON)) // cancel quantity.
    {
        PlaySE(SE_SELECT);
        Menu_EraseWindowRect(6, 6, 0xD, 0xB);
        StartScrollIndicatorArrow(TOP_ARROW);
        StartScrollIndicatorArrow(BOTTOM_ARROW);
        ItemStorage_PrintDescription(gSaveBlock1.pcItems[tItemStorageItemsAbove + tItemStorageCursorPos].itemId); // why not use trueIndex?
        TASK.FUNC = ItemStorage_ProcessInput;
    }
}

static void ItemStorage_DoItemWithdraw(u8 taskId)
{
    s16 *data = TASK.data;
    u8 trueIndex = tItemStorageCursorPos + tItemStorageItemsAbove;

    if(AddBagItem(gSaveBlock1.pcItems[trueIndex].itemId, tItemStorageQuantity) == TRUE) // add item works.
    {
        CopyItemName(gSaveBlock1.pcItems[trueIndex].itemId, gStringVar1);
        ConvertIntToDecimalStringN(gStringVar2, tItemStorageQuantity, 0, 3);
        ItemStorage_PrintDescription(MSG_WITHDREW_ITEM);
        TASK.FUNC = ItemStorage_HandleRemoveItem;
    }
    else
    {
        tItemStorageQuantity = 0;
        ItemStorage_PrintDescription(MSG_NO_MORE_ROOM);
        TASK.FUNC = ItemStorage_HandleErrorMessageInput;
    }
}

static void ItemStorage_DoItemToss(u8 taskId)
{
    s16 *data = TASK.data;
    u8 itemIndex = tItemStorageCursorPos + tItemStorageItemsAbove;

    if(GetItemImportance(gSaveBlock1.pcItems[itemIndex].itemId) == FALSE)
    {
        CopyItemName(gSaveBlock1.pcItems[itemIndex].itemId, gStringVar1);
        ConvertIntToDecimalStringN(gStringVar2, tItemStorageQuantity, 0, 3);
        ItemStorage_PrintDescription(MSG_OKAY_TO_THROW_AWAY);
        DisplayYesNoMenu(7, 6, 1);
        DoYesNoFuncWithChoice(taskId, (struct YesNoFuncTable *)&sItemTossYesNoFuncs);
    }
    else
    {
        tItemStorageQuantity = 0;
        ItemStorage_PrintDescription(MSG_TOO_IMPORTANT);
        TASK.FUNC = ItemStorage_HandleRemoveItem;
    }
}

static void ItemStorage_TossItemYes(u8 taskId)
{
    Menu_EraseWindowRect(0x6, 0x6, 0xD, 0xB);
    ItemStorage_PrintDescription(MSG_THREW_AWAY_ITEM);
    TASK.FUNC = ItemStorage_HandleRemoveItem;
}

static void ItemStorage_TossItemNo(u8 taskId)
{
    s16 *data = TASK.data;

    Menu_EraseWindowRect(0x6, 0x6, 0xD, 0xB);
    InitMenu(0, 16, 2, tItemStoragePageItems, tItemStorageCursorPos, 0xD);
    StartScrollIndicatorArrow(TOP_ARROW);
    StartScrollIndicatorArrow(BOTTOM_ARROW);
    ItemStorage_PrintDescription(gSaveBlock1.pcItems[tItemStorageItemsAbove + tItemStorageCursorPos].itemId);
    TASK.FUNC = ItemStorage_ProcessInput;
}

static void ItemStorage_HandleRemoveItem(u8 taskId)
{
    s16 *data = TASK.data;
    s16 oldNumItems;

    if (JOY_NEW(A_BUTTON) || gMain.newKeys == B_BUTTON)
    {
        RemovePCItem(tItemStorageCursorPos + tItemStorageItemsAbove, tItemStorageQuantity);
        oldNumItems = tItemStorageItemCount;
        tItemStorageItemCount = CountUsedPCItemSlots();

        if(oldNumItems != tItemStorageItemCount && oldNumItems < tItemStoragePageItems + tItemStorageItemsAbove && tItemStorageItemsAbove != 0)
            tItemStorageItemsAbove--;

        SetPlayerPCListCount(taskId);
        ItemStorage_ReturnToListInput(taskId);
        InitMenu(0, 16, 2, tItemStoragePageItems, tItemStorageCursorPos, 0xD);
    }
}

static void ItemStorage_HandleErrorMessageInput(u8 taskId)
{
    s16 *data = TASK.data;

    if (JOY_NEW(A_BUTTON) || gMain.newKeys == B_BUTTON)
    {
        ItemStorage_PrintDescription(gSaveBlock1.pcItems[tItemStorageItemsAbove + tItemStorageCursorPos].itemId);
        StartScrollIndicatorArrow(TOP_ARROW);
        StartScrollIndicatorArrow(BOTTOM_ARROW);
        TASK.FUNC = ItemStorage_ProcessInput;
    }
}

static void ItemStorage_ReturnToListInput(u8 taskId)
{
    Menu_EraseWindowRect(0x6, 0x6, 0xD, 0xB);
    StartScrollIndicatorArrow(TOP_ARROW);
    StartScrollIndicatorArrow(BOTTOM_ARROW);
    ItemStorage_DrawBothListAndDescription(taskId);
    TASK.FUNC = ItemStorage_ProcessInput;
}

static void ItemStorage_FinishItemSwap(u8 taskId, bool8 switchModeDisabled)
{
    s16 *data = TASK.data;
    u8 trueIndex = tItemStorageItemsAbove + tItemStorageCursorPos;

    tItemStorageIsSwitching = FALSE;

    if((u8)tItemStorageItemCount > trueIndex && (u8)tItemStorageSwapItemPos != trueIndex && switchModeDisabled == FALSE)
    {
        struct ItemSlot itemSlot = gSaveBlock1.pcItems[tItemStorageSwapItemPos]; // backup the itemSlot before swapping the two.

        gSaveBlock1.pcItems[tItemStorageSwapItemPos] = gSaveBlock1.pcItems[trueIndex];
        gSaveBlock1.pcItems[trueIndex] = itemSlot;
        return;
    }
    else if(trueIndex == tItemStorageItemCount)
    {
        ItemStorage_PrintDescription(MSG_GO_BACK_TO_PREV);
    }
    else
    {
        ItemStorage_PrintDescription(gSaveBlock1.pcItems[trueIndex].itemId);
    }

    /*
    THEORY: This check produces essentially dead code, but it might have been working in an earlier build
    in which case it allows a programmer to easily duplicate items without the use of a debug menu.
    This gets the page index of the swap index for some reason. It is currently unknown what it would have been used
    for, but perhaps it was used to increase the quantity of the item without a debug menu.
    With the removal of a lot of the debug menus close to release, a programmer may have added this to
    help test things with a low key (such as planting a lot of duplicated berries, which requires this lazy "cheat")
    without bringing the relevent debug menus back. The commented out line is intentionally left in below to show
    what it may have looked like.
    */
    if(tItemStorageSwapItemPos - tItemStorageItemsAbove <= 0) { // this check is arbitrary and used to generate the correct assembly using the subtraction, which is what matters. the 0 check doesn't.
        //gSaveBlock1.pcItems[tItemStorageSwapItemPos].quantity += 100;
        gSaveBlock1.pcItems[tItemStorageSwapItemPos].quantity += 0; // do not enforce item cap.
    }
}

static void ItemStorage_DrawItemQuantity(u16 quantity, enum StringConvertMode conversionMode, u8 y, u8 maxDigits, int isSwapSelected)
{
    ConvertIntToItemQuantityString(gStringVar1, quantity, conversionMode, maxDigits);

    if(isSwapSelected != FALSE)
        Menu_PrintText(sSelectedItemQuantityText, 0x1A, y);
    else
        Menu_PrintText(sUnselectedItemQuantityText, 0x1A, y);
}

static void ItemStorage_DrawItemVoidQuantity(u8 y)
{
    Menu_PrintText(sEmptyItemQuantityText, 0x19, y);
}

static void ItemStorage_DrawItemName(struct ItemSlot *itemSlot, u8 y, int isSwapSelected)
{
    CopyItemName(itemSlot->itemId, gStringVar1);

    if(isSwapSelected != FALSE)
        Menu_PrintText(sSelectedItemNameText, 16, y);
    else
        Menu_PrintText(sUnselectedItemNameText, 16, y);
}

static void ItemStorage_DrawNormalItemEntry(struct ItemSlot *itemSlot, u8 y, int isSwapSelected)
{
    ItemStorage_DrawItemName(itemSlot, y, isSwapSelected);
    ItemStorage_DrawItemQuantity(itemSlot->quantity, STR_CONV_MODE_RIGHT_ALIGN, y, 3, isSwapSelected);
}

static void ItemStorage_DrawKeyItemEntry(struct ItemSlot *itemSlot, u8 y, int isSwapSelected)
{
    ItemStorage_DrawItemName(itemSlot, y, isSwapSelected);
    ItemStorage_DrawItemVoidQuantity(y);
}

static void ItemStorage_DrawTMHMEntry(struct ItemSlot *itemSlot, u8 y, int isSwapSelected)
{
    ItemStorage_DrawItemName(itemSlot, y, isSwapSelected);

    if(itemSlot->itemId < ITEM_HM01)
        ItemStorage_DrawItemQuantity(itemSlot->quantity, STR_CONV_MODE_RIGHT_ALIGN, y, 3, isSwapSelected);
    else
        ItemStorage_DrawItemVoidQuantity(y); // HMs do not have a quantity.
}

static void ItemStorage_DrawItemList(u8 taskId)
{
    s16 *data = TASK.data;
    int isSwapSelected;
    u16 i;
    u16 yCoord = 0;

    for (i = tItemStorageItemsAbove; i < tItemStorageItemsAbove + tItemStoragePageItems; i++)
    {
        yCoord = (i - tItemStorageItemsAbove) * 2;

        if (i == tItemStorageItemCount)
        {
            Menu_PrintTextWithNewlineIndent(gText_Cancel, 0x80, (yCoord + 2) * 8, 0x68, 1);
            break;
        }
        else
        {
            isSwapSelected = FALSE;

            if (tItemStorageIsSwitching != FALSE && i == tItemStorageSwapItemPos)
                isSwapSelected = TRUE;

            switch (GetPocketByItemId(gSaveBlock1.pcItems[i].itemId) - 1)
            {
            case 0:
            case 1:
            case 3:
                ItemStorage_DrawNormalItemEntry((struct ItemSlot *)&gSaveBlock1.pcItems[i], yCoord + 2, isSwapSelected);
                break;
            case 4:
                ItemStorage_DrawKeyItemEntry((struct ItemSlot *)&gSaveBlock1.pcItems[i], yCoord + 2, isSwapSelected);
                break;
            case 2:
                ItemStorage_DrawTMHMEntry((struct ItemSlot *)&gSaveBlock1.pcItems[i], yCoord + 2, isSwapSelected);
                break;
            }
        }
    }

    if (i - tItemStorageItemsAbove < 8)
        Menu_BlankWindowRect(16, yCoord + 4, 0x1C, 0x12);

    if (tItemStorageItemsAbove != 0)
        CreateScrollIndicatorArrow(TOP_ARROW, 0xB8, 8);
    else
        DestroyScrollIndicatorArrow(TOP_ARROW);

    if (tItemStorageItemsAbove + tItemStoragePageItems <= tItemStorageItemCount)
        CreateScrollIndicatorArrow(BOTTOM_ARROW, 0xB8, 0x98);
    else
        DestroyScrollIndicatorArrow(BOTTOM_ARROW);
}

static void ItemStorage_PrintDescription(u16 itemId)
{
    const u8 *string;

    switch(itemId)
    {
        case MSG_GO_BACK_TO_PREV:
            string = gText_GoBackPrevMenu;
            break;
        case MSG_HOW_MANY_TO_WITHDRAW:
            string = gText_WithdrawHowManyItems;
            break;
        case MSG_WITHDREW_ITEM:
            string = gText_WithdrawXItems;
            break;
        case MSG_HOW_MANY_TO_TOSS:
            string = gText_TossHowManyItems;
            break;
        case MSG_THREW_AWAY_ITEM:
            string = gText_ThrewAwayVar2Var1s;
            break;
        case MSG_NO_MORE_ROOM:
            string = gText_NoRoomInBag;
            break;
        case MSG_TOO_IMPORTANT:
            string = gText_TooImportantToToss;
            break;
        case MSG_OKAY_TO_THROW_AWAY:
            string = gText_ConfirmTossItems;
            break;
        case MSG_SWITCH_WHICH_ITEM:
            string = gText_SwitchWhichItem;
            break;
        default:
            string = GetItemDescription(itemId);
            break;
    }

    Menu_PrintTextWithNewlineIndentAndClear(string, 8, 0x68, 0x68, 0x30, 1);
}

static void ItemStorage_DrawBothListAndDescription(u8 taskId)
{
    s16 *data = TASK.data;
    s16 trueIndex = tItemStorageItemsAbove + tItemStorageCursorPos;

    ItemStorage_DrawItemList(taskId);

    if(tItemStorageIsSwitching == FALSE)
    {
        if(trueIndex == tItemStorageItemCount)
            ItemStorage_PrintDescription(MSG_GO_BACK_TO_PREV);
        else
            ItemStorage_PrintDescription(gSaveBlock1.pcItems[trueIndex].itemId);
    }
}

static void ItemStorage_DrawItemListMenu(u8 taskId, u8 menuOption)
{
    s16 *data = TASK.data;

    ResetScrollIndicatorState();
    LoadScrollIndicatorPalette();
    ItemStorage_LoadPalette();
    Menu_DrawStdWindowFrame(0xF, 0, 0x1D, 0x13);
    Menu_DrawStdWindowFrame(0, 0xC, 0xE, 0x13);
    Menu_DrawStdWindowFrame(0, 0, 0xB, 3);
    ItemStorage_PrintDescription(gSaveBlock1.pcItems[0].itemId);
    Menu_PrintText(sItemStorage_MenuActions[menuOption].text, 1, 1);
    ItemStorage_DrawItemList(taskId);
    InitMenu(0, 0x10, 2, tItemStoragePageItems, tItemStorageCursorPos, 0xD);
}

static void ItemStorage_LoadPalette(void)
{
    u16 arr[] = {
        0x5294,
        0x6B5A,
        0x7FFF
    };

    LoadPalette(&arr[2], 0xDF, 2);
    LoadPalette(&arr[1], 0xD1, 2);
    LoadPalette(&arr[0], 0xD8, 2);
}

static u8 GetMailboxMailCount(void)
{
    u8 i, j;

    for(i = 0, j = PARTY_SIZE; j < MAIL_COUNT; j++)
        if(gSaveBlock1.mail[j].itemId != ITEM_NONE)
            i++;

    return i;
}

static void Mailbox_CompactMailList(void)
{
    struct Mail mailBuffer;
    u8 i, j;

    for (i = PARTY_SIZE; i < MAIL_COUNT - 1; i++)
    {
        for (j = i + 1; j < MAIL_COUNT; j++)
        {
            if (gSaveBlock1.mail[i].itemId == ITEM_NONE)
            {
                mailBuffer = gSaveBlock1.mail[i];
                gSaveBlock1.mail[i] = gSaveBlock1.mail[j];
                gSaveBlock1.mail[j] = mailBuffer;
            }
        }
    }
}

static void Mailbox_DrawMailList(u8 taskId) // taskId is unused
{
    u16 yCoord = 0;
    u16 i;

    for (i = gPlayerPCItemPageInfo.itemsAbove; i < gPlayerPCItemPageInfo.itemsAbove + gPlayerPCItemPageInfo.pageItems; i++)
    {
        yCoord = (i - gPlayerPCItemPageInfo.itemsAbove) * 2;
        Menu_BlankWindowRect(0x15, yCoord + 2, 0x1C, yCoord + 3);
        if (i == gPlayerPCItemPageInfo.count)
        {
            Menu_PrintText(gText_Cancel, 0x15, yCoord + 2);
            break;
        }
        else
        {
            StringCopy(gStringVar1, gSaveBlock1.mail[i + 6].playerName);
            ConvertInternationalPlayerName(gStringVar1);
            Menu_PrintText(gStringVar1, 0x15, yCoord + 2);
        }
    }

    if (i - gPlayerPCItemPageInfo.itemsAbove != 8)
        Menu_BlankWindowRect(0x15, yCoord + 4, 0x1C, 0x12);

    if (gPlayerPCItemPageInfo.itemsAbove != 0)
        CreateScrollIndicatorArrow(TOP_ARROW, 0xC8, 8);
    else
        DestroyScrollIndicatorArrow(TOP_ARROW);

    if (gPlayerPCItemPageInfo.itemsAbove + gPlayerPCItemPageInfo.pageItems <= gPlayerPCItemPageInfo.count)
        CreateScrollIndicatorArrow(BOTTOM_ARROW, 0xC8, 0x98);
    else
        DestroyScrollIndicatorArrow(BOTTOM_ARROW);
}

static void Mailbox_DrawMailboxMenu(u8 taskId)
{
    ResetScrollIndicatorState();
    LoadScrollIndicatorPalette();
    Menu_EraseWindowRect(0, 0, 0x1D, 0x13);
    Menu_DrawStdWindowFrame(0, 0, 0x8, 0x3);
    Menu_PrintText(gText_Mailbox, 1, 1);
    Menu_DrawStdWindowFrame(0x14, 0, 0x1D, 0x13);
    Mailbox_DrawMailList(taskId);
    InitMenu(0, 0x15, 2, gPlayerPCItemPageInfo.pageItems, gPlayerPCItemPageInfo.cursorPos, 8);
}

// Mailbox_ProcessInput
static void Mailbox_ProcessInput(u8 taskId)
{
    if(!gPaletteFade.active)
    {
        if (JOY_REPT(DPAD_UP))
        {
            if(gPlayerPCItemPageInfo.cursorPos != 0)
            {
                PlaySE(SE_SELECT);
                gPlayerPCItemPageInfo.cursorPos = Menu_MoveCursor(-1);
            }
            else if(gPlayerPCItemPageInfo.itemsAbove != 0)
            {
                PlaySE(SE_SELECT);
                gPlayerPCItemPageInfo.itemsAbove--;
                Mailbox_DrawMailList(taskId);
            }
        }
        else if (JOY_REPT(DPAD_DOWN))
        {
            if(gPlayerPCItemPageInfo.cursorPos != gPlayerPCItemPageInfo.pageItems - 1)
            {
                PlaySE(SE_SELECT);
                gPlayerPCItemPageInfo.cursorPos = Menu_MoveCursor(1);
            }
            else if(gPlayerPCItemPageInfo.itemsAbove + gPlayerPCItemPageInfo.cursorPos != gPlayerPCItemPageInfo.count)
            {
                PlaySE(SE_SELECT);
                gPlayerPCItemPageInfo.itemsAbove++;
                Mailbox_DrawMailList(taskId);
            }
        }
        else if (JOY_NEW(A_BUTTON))
        {
            Menu_DestroyCursor();
            PlaySE(SE_SELECT);

            if(gPlayerPCItemPageInfo.itemsAbove + gPlayerPCItemPageInfo.cursorPos == gPlayerPCItemPageInfo.count)
            {
                Mailbox_ExitToPlayerPC(taskId);
            }
            else
            {
                Mailbox_CloseScrollIndicators();
                TASK.FUNC = Mailbox_PrintWhatToDoWithPlayerMailText;
            }
        }
        else if (JOY_NEW(B_BUTTON))
        {
            Menu_DestroyCursor();
            PlaySE(SE_SELECT);
            Mailbox_ExitToPlayerPC(taskId);
        }
    }
}

static void Mailbox_CloseScrollIndicators(void)
{
    FreeScrollIndicatorPalette();
    DestroyScrollIndicatorArrow(TOP_ARROW);
    DestroyScrollIndicatorArrow(BOTTOM_ARROW);
}

static void Mailbox_PrintWhatToDoWithPlayerMailText(u8 taskId)
{
    Menu_EraseWindowRect(0, 0, 0x1D, 0x13);
    StringCopy(gStringVar1, gSaveBlock1.mail[gPlayerPCItemPageInfo.itemsAbove + 6 + gPlayerPCItemPageInfo.cursorPos].playerName);
    ConvertInternationalPlayerName(gStringVar1);
    StringExpandPlaceholders(gStringVar4, gText_WhatToDoWithVar1sMail);
    DisplayItemMessageOnField(taskId, gStringVar4, Mailbox_PrintMailOptions, 0);
}

static void Mailbox_ReturnToPlayerPC(u8 taskId)
{
    Menu_EraseWindowRect(0, 0, 0x1D, 0x13);
    ReshowPlayerPC(taskId);
}

static void Mailbox_ExitToPlayerPC(u8 taskId)
{
    Mailbox_CloseScrollIndicators();
    TASK.FUNC = Mailbox_ReturnToPlayerPC;
}

static void Mailbox_PrintMailOptions(u8 taskId) // Mailbox_PrintMailOptions
{
    Menu_DrawStdWindowFrame(0, 0, 0xC, 0x9);
    Menu_PrintItems(1, 1, 4, (struct MenuAction *)sMailboxMailOptions);
    InitMenu(0, 1, 1, 4, 0, 0xB);
    TASK.FUNC = Mailbox_MailOptionsProcessInput;
}

static void Mailbox_MailOptionsProcessInput(u8 taskId)
{
    if (JOY_REPT(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        Menu_MoveCursor(-1);
    }
    else if (JOY_REPT(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        Menu_MoveCursor(1);
    }
    else if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        sMailboxMailOptions[Menu_GetCursorPos()].func(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Mailbox_Cancel(taskId);
    }
}

static void Mailbox_DoMailRead(u8 taskId)
{
    FadeScreen(1, 0);
    TASK.FUNC = Mailbox_FadeAndReadMail;
}

static void Mailbox_FadeAndReadMail(u8 taskId)
{
    if(!gPaletteFade.active)
    {
        ReadMail(&gSaveBlock1.mail[gPlayerPCItemPageInfo.itemsAbove + 6 + gPlayerPCItemPageInfo.cursorPos], Mailbox_ReturnToFieldFromReadMail, 1);
        DestroyTask(taskId);
    }
}

static void Mailbox_HandleReturnToProcessInput(u8 taskId) // Mailbox_HandleReturnToProcessInput
{
    if (IsWeatherNotFadingIn() == TRUE) // is black fade finished? why not gPaletteFade.active?
        TASK.FUNC = Mailbox_ProcessInput;
}

static void Mailbox_ReshowAfterMail(void)
{
    Mailbox_DrawMailboxMenu(CreateTask(Mailbox_HandleReturnToProcessInput, 0));
    FadeInFromBlack();
}

static void Mailbox_ReturnToFieldFromReadMail(void)
{
    gFieldCallback = Mailbox_ReshowAfterMail;
    SetMainCallback2(CB2_ReturnToField);
}

static const u8 sHighlightedMoveToBagText[] = _("{COLOR RED}{STR_VAR_1}");

static void Mailbox_MoveToBag(u8 taskId)
{
    Menu_DestroyCursor();
    StringCopy(gStringVar1, gText_MoveToBag);
    Menu_PrintText(sHighlightedMoveToBagText, 1, 3); // sHighlightedMoveToBagText
    DisplayItemMessageOnField(taskId, gText_MessageWillBeLost, Mailbox_AskConfirmMoveToBag, 0);
}

static void Mailbox_AskConfirmMoveToBag(u8 taskId)
{
    DisplayYesNoMenu(0x14, 0x8, 0x1);
    DoYesNoFuncWithChoice(taskId, (struct YesNoFuncTable *)&sMailboxMoveToBagYesNoFuncs);
}

static void Mailbox_DoMailMoveToBag(u8 taskId)
{
    struct Mail *mail = &gSaveBlock1.mail[gPlayerPCItemPageInfo.itemsAbove + PARTY_SIZE + gPlayerPCItemPageInfo.cursorPos];

    Menu_EraseWindowRect(0x14, 8, 0x1A, 0xD);

    if(AddBagItem(mail->itemId, 1) == FALSE)
    {
        DisplayItemMessageOnField(taskId, gText_BagIsFull, Mailbox_DrawMailMenuAndDoProcessInput, 0);
    }
    else
    {
        DisplayItemMessageOnField(taskId, gText_MailToBagMessageErased, Mailbox_DrawMailMenuAndDoProcessInput, 0);
        ClearMail(mail);
        Mailbox_CompactMailList();

        gPlayerPCItemPageInfo.count--;

        if(gPlayerPCItemPageInfo.count < gPlayerPCItemPageInfo.pageItems + gPlayerPCItemPageInfo.itemsAbove && gPlayerPCItemPageInfo.itemsAbove != 0)
            gPlayerPCItemPageInfo.itemsAbove--;

        SetPlayerPCListCount(taskId);
    }
}

static void Mailbox_CancelMoveToBag(u8 taskId) // Mailbox_CancelMoveToBag
{
    Menu_EraseWindowRect(0x14, 0x8, 0x1A, 0xD);
    Mailbox_DrawMailMenuAndDoProcessInput(taskId);
}

static void Mailbox_Give(u8 taskId)
{
    if(CalculatePlayerPartyCount() == 0)
        Mailbox_NoPokemonForMail(taskId); // cannot be reached normally
    else
    {
        FadeScreen(1, 0);
        TASK.FUNC = Mailbox_DoGiveMailPokeMenu;
    }
}

static void Mailbox_DoGiveMailPokeMenu(u8 taskId) // Mailbox_DoGiveMailPokeMenu
{
    if(!gPaletteFade.active)
    {
        SetMainCallback2(CB2_OpenPartyMenuForItemUse);
        gPokemonItemUseType = ITEM_USE_GIVE_MAIL;
        DestroyTask(taskId);
    }
}

static void Mailbox_UpdateMailListAfterDeposit(void)
{
    u8 taskId = CreateTask(Mailbox_HandleReturnToProcessInput, 0);
    u8 prevCount = gPlayerPCItemPageInfo.count;

    gPlayerPCItemPageInfo.count = GetMailboxMailCount();
    Mailbox_CompactMailList();

    if(prevCount != gPlayerPCItemPageInfo.count && gPlayerPCItemPageInfo.count < gPlayerPCItemPageInfo.pageItems + gPlayerPCItemPageInfo.itemsAbove && gPlayerPCItemPageInfo.itemsAbove != 0) // did the count update?
        gPlayerPCItemPageInfo.itemsAbove--;

    SetPlayerPCListCount(taskId);
    Mailbox_DrawMailboxMenu(taskId);
    FadeInFromBlack();
}

void Mailbox_ReturnToMailListAfterDeposit(void)
{
    gFieldCallback = Mailbox_UpdateMailListAfterDeposit;
    SetMainCallback2(CB2_ReturnToField);
}

// you always have at least 1 POKeMON and you cannot receive mail before you leave Littleroot: therefore this function cannot be reached normally.
static void Mailbox_NoPokemonForMail(u8 taskId)
{
    DisplayItemMessageOnField(taskId, gText_NoPokemon, Mailbox_DrawMailMenuAndDoProcessInput, 0);
}

static void Mailbox_Cancel(u8 taskId)
{
    Menu_DestroyCursor();
    Menu_EraseWindowRect(0, 0, 0xC, 0x9);
    Mailbox_DrawMailMenuAndDoProcessInput(taskId);
}

static void Mailbox_DrawMailMenuAndDoProcessInput(u8 taskId)
{
    Mailbox_DrawMailboxMenu(taskId);
    TASK.FUNC = Mailbox_ProcessInput;
}
