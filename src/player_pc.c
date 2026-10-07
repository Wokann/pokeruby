#include "global.h"
#include "player_pc.h"
#include "decoration.h"
#include "field_screen_effect.h"
#include "field_weather.h"
#include "item.h"
#include "item_menu.h"
#include "constants/items.h"
#include "main.h"
#include "menu.h"
#include "menu_helpers.h"
#include "palette.h"
#include "script.h"
#include "sound.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "constants/songs.h"
#include "name_string_util.h"
#include "mail.h"
#include "overworld.h"
#include "ewram.h"

// player PC menu options
enum
{
    MENU_ITEMSTORAGE,
    MENU_MAILBOX,
    MENU_DECORATION,
    MENU_TURNOFF
};

// item storage menus
enum
{
    MENU_WITHDRAW,
    MENU_DEPOSIT,
    MENU_TOSS,
    MENU_EXIT
};

// special item description handlers
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

#define tUsedSlots data[2]
#define tQuantity data[3]

// this is potentially an ewram access occuring in high ewram. TODO: investigate this further.
#define NEW_GAME_PC_ITEMS(i, type) ((u16)((u16 *)sNewGamePCItems + type)[i * 2])

// defined and used in the above macro
enum
{
    PC_ITEM_ID,
    PC_QUANTITY
};

extern void CB2_ShowPartyMenuForItemUse(void);

static EWRAM_DATA const u8 *sTopMenuOptionOrder = NULL;

static u8 sTopMenuNumOptions;

extern u8 gPokemonItemUseType;

// event scripts
extern u8 gBrendanHouse_TurnPCOff[];
extern u8 gMayHouse_TurnPCOff[];

extern void (*gFieldCallback)(void);

static void InitPlayerPCMenu(u8 taskId);
static void PlayerPCProcessMenuInput(u8 taskId);
static void InitItemStorageMenu(u8);
static void ItemStorageMenuPrint(const u8 *);
static void ItemStorageMenuProcessInput(u8);
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
static void ItemStorage_PrintItemPcResponse(u16);
static void ItemStorage_DrawBothListAndDescription(u8);
static void ItemStorage_CreateListMenu(u8, u8);
static void ItemStorage_LoadPalette(void);
static u8 GetMailboxMailCount(void);
static void Mailbox_CompactMailList(void);
static void Mailbox_DrawMailboxMenu(u8);
static void Mailbox_ProcessInput(u8);
static void Mailbox_CloseScrollIndicators(void);
static void Mailbox_PrintWhatToDoWithPlayerMailText(u8);
static void Mailbox_TurnOff(u8);
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
static void ItemStorage_Exit(u8);
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

static const struct YesNoFuncTable ItemTossYesNoFuncs[] = // ItemTossYesNoFuncs
{
    ItemStorage_TossItemYes,
    ItemStorage_TossItemNo
};

static const struct YesNoFuncTable ResumeFromWithdrawYesNoFuncList[] = // ResumeFromWithdrawYesNoFuncList
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

static const struct MenuAction2 gMailboxMailOptions[] =
{
    { gText_Read, Mailbox_DoMailRead },
    { gText_MoveToBag, Mailbox_MoveToBag },
    { gText_Give2, Mailbox_Give },
    { gText_Cancel2, Mailbox_Cancel }
};

static const u8 gNonSelectedItemFormattedText[] = _("{STR_VAR_1}{CLEAR_TO 80}");
static const u8 gSelectedItemFormattedText[] = _("{COLOR RED}{STR_VAR_1}{CLEAR_TO 80}");
static const u8 gNonSelectedItemQuantityFormatText[] = _("{STR_VAR_1}");
static const u8 gSelectedItemQuantityFormatText[] = _("{COLOR RED}{STR_VAR_1}");
static const u8 gUnknown_08406330[] = _("{CLEAR_TO 32}");

extern u8 *gUnknown_02039314;
extern struct MenuAction gUnknown_08406298[];

extern u8 gUnknown_084062B8[];
extern u8 gUnknown_084062BC[];
extern u8 gUnknown_0840632A[];
extern u8 gUnknown_08406327[];
extern u8 gUnknown_0840631E[];
extern u8 gUnknown_08406318[];

extern u8 gUnknown_030007B4;
extern u8 unk_201FE00[];

extern u8 gUnknown_08152850;
extern u8 gUnknown_08152C75;

extern u32 gUnknown_08406288[];
extern const struct MenuAction gUnknown_084062C0[];
extern const struct YesNoFuncTable gUnknown_084062E0;

void NewGameInitPCItems(void)
{
    u8 i;

    // because Game Freak don't know how to use a struct or a 2d array
    for(i = 0, ClearItemSlots(gSaveBlock1.pcItems, ARRAY_COUNT(gSaveBlock1.pcItems)); NEW_GAME_PC_ITEMS(i, PC_ITEM_ID) && NEW_GAME_PC_ITEMS(i, PC_QUANTITY) &&
        AddPCItem(NEW_GAME_PC_ITEMS(i, PC_ITEM_ID), NEW_GAME_PC_ITEMS(i, PC_QUANTITY)) == TRUE; i++)
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
    if (JOY_REPEAT(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        Menu_MoveCursor(-1);
    }
    else if (JOY_REPEAT(DPAD_DOWN))
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

void ReshowPlayerPC(u8 var)
{
    DisplayItemMessageOnField(var, gText_WhatWouldYouLike, InitPlayerPCMenu, 0);
}

static void PlayerPC_ItemStorage(u8 taskId)
{
    InitItemStorageMenu(MENU_WITHDRAW);
    TASK.FUNC = ItemStorageMenuProcessInput;
}

static void PlayerPC_Mailbox(u8 taskId)
{
    Menu_EraseWindowRect(0, 0, 10, 9);
    eMailboxInfo.count = GetMailboxMailCount();

    if (eMailboxInfo.count == 0)
        DisplayItemMessageOnField(taskId, gText_NoMailHere, ReshowPlayerPC, 0);
    else
    {
        eMailboxInfo.cursorPos = 0;
        eMailboxInfo.itemsAbove = 0;
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
            ScriptContext_SetupScript(gBrendanHouse_TurnPCOff);
        else
            ScriptContext_SetupScript(gMayHouse_TurnPCOff);
    }
    else
    {
        Menu_EraseWindowRect(0, 0, 10, 9);
        ScriptContext_Enable();
    }
    DestroyTask(taskId);
}

static void InitItemStorageMenu(u8 var)
{
    Menu_EraseWindowRect(0, 0, 10, 9);
    Menu_DrawStdWindowFrame(0, 0, 11, 9);
    Menu_PrintItems(1, 1, 4, (struct MenuAction *)sItemStorage_MenuActions);
    InitMenu(0, 1, 1, 4, var, 10);
    ItemStorageMenuPrint(sItemStorage_OptionDescriptions[var]);
}

static void ItemStorageMenuPrint(const u8 *textPtr)
{
    Menu_BlankWindowRect(2, 15, 27, 18);
    Menu_PrintText(textPtr, 2, 15);
}

static void ItemStorageMenuProcessInput(u8 taskId)
{
    if (JOY_REPEAT(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        Menu_MoveCursor(-1);
        ItemStorageMenuPrint(sItemStorage_OptionDescriptions[Menu_GetCursorPos()]);
    }
    else if (JOY_REPEAT(DPAD_DOWN))
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

void ItemStorage_ReturnToMenuAfterDeposit(void)
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
    tUsedSlots = CountUsedPCItemSlots();

    if (tUsedSlots != 0)
    {
        Menu_EraseWindowRect(0, 14, 29, 19);
        CURRENT_ITEM_STORAGE_MENU = MENU_WITHDRAW;
        PAGE_INDEX = 0;
        ITEMS_ABOVE_TOP = 0;
        SetPlayerPCListCount(taskId);
        ItemStorage_CreateListMenu(taskId, 0);
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
    tUsedSlots = CountUsedPCItemSlots();

    if (tUsedSlots)
    {
        Menu_EraseWindowRect(0, 14, 29, 19);
        CURRENT_ITEM_STORAGE_MENU = MENU_TOSS;
        PAGE_INDEX = 0;
        ITEMS_ABOVE_TOP = 0;
        SetPlayerPCListCount(taskId);
        ItemStorage_CreateListMenu(taskId, 2);
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

    if (tUsedSlots > 7) // we have a full page, so set the num of page items appropriately.
        NUM_PAGE_ITEMS = 8;
    else
        NUM_PAGE_ITEMS = tUsedSlots + 1; // there are not enough items to fill a full page; take the # of items and add 1 for the cancel button.

    if (eMailboxInfo.count > 7)
        eMailboxInfo.pageItems = 8;
    else
        eMailboxInfo.pageItems = eMailboxInfo.count + 1;
}

static void ItemStorage_ProcessInput(u8 taskId)
{
    s16 *data = TASK.data;
    s16 trueIndex;

    if (JOY_REPEAT(DPAD_UP))
    {
        if(PAGE_INDEX != 0) // did the cursor move physically upwards?
        {
            PlaySE(SE_SELECT);
            PAGE_INDEX = Menu_MoveCursor(-1);
            trueIndex = ITEMS_ABOVE_TOP + PAGE_INDEX;
            if (SWITCH_MODE_ACTIVE == FALSE) // are we not currently switching items?
            {
                if (trueIndex == tUsedSlots) // if the cursor is on top of cancel, print the go back to prev description.
                {
                    ItemStorage_PrintItemPcResponse(MSG_GO_BACK_TO_PREV);
                }
                else
                {
                    ItemStorage_PrintItemPcResponse(gSaveBlock1.pcItems[trueIndex].itemId);
                }
            }
        }
        else // the page cursor is at the top. but we may not be at the top of the true index list, so do another check.
        {
            if (ITEMS_ABOVE_TOP == 0) // did the cursor not move due to being at the top of the list?
                return;

            PlaySE(SE_SELECT);
            ITEMS_ABOVE_TOP--;
            ItemStorage_DrawBothListAndDescription(taskId);

            if (SWITCH_MODE_ACTIVE != FALSE)
                Menu_MoveCursor(0); // don't move the cursor. it's at the top of the page index, but not the true index.
        }
    }
    else if (JOY_REPEAT(DPAD_DOWN)) // _0813A306
    {
        if(PAGE_INDEX != NUM_PAGE_ITEMS - 1)
        {
            PlaySE(SE_SELECT);
            PAGE_INDEX = Menu_MoveCursor(1);
            trueIndex = ITEMS_ABOVE_TOP + PAGE_INDEX;

            if(SWITCH_MODE_ACTIVE != FALSE)
                return;

            if (trueIndex == tUsedSlots)
                ItemStorage_PrintItemPcResponse(MSG_GO_BACK_TO_PREV); // probably further down
            else
                ItemStorage_PrintItemPcResponse(gSaveBlock1.pcItems[trueIndex].itemId);
        }
        else if(ITEMS_ABOVE_TOP + PAGE_INDEX != tUsedSlots)
        {
            PlaySE(SE_SELECT);
            ITEMS_ABOVE_TOP++;
            ItemStorage_DrawBothListAndDescription(taskId);

            if (SWITCH_MODE_ACTIVE != FALSE)
                Menu_MoveCursor(0);
        }
    }
    else if (JOY_NEW(SELECT_BUTTON)) // _0813A3A0
    {
        if (SWITCH_MODE_ACTIVE == FALSE)
        {
            if (PAGE_INDEX + ITEMS_ABOVE_TOP != tUsedSlots) // you cannot swap the Cancel button.
            {
                PlaySE(SE_SELECT);
                SWITCH_MODE_ACTIVE = TRUE;
                SWAP_ITEM_INDEX = ITEMS_ABOVE_TOP + PAGE_INDEX;
                ItemStorage_PrintItemPcResponse(MSG_SWITCH_WHICH_ITEM);
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
        if(SWITCH_MODE_ACTIVE == FALSE)
        {
            if(ITEMS_ABOVE_TOP + PAGE_INDEX != tUsedSlots)
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
        if(SWITCH_MODE_ACTIVE == FALSE)
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
    BuyMenuFreeMemory();
    DestroyVerticalScrollIndicator(TOP_ARROW);
    DestroyVerticalScrollIndicator(BOTTOM_ARROW);
    Menu_EraseWindowRect(0, 0, 29, 19);
    Menu_DisplayDialogueFrame();
    InitItemStorageMenu(TASK.CURRENT_ITEM_STORAGE_MENU);
    TASK.FUNC = ItemStorageMenuProcessInput;
}

static void ItemStorage_DoItemAction(u8 taskId)
{
    s16 *data = TASK.data;
    u8 trueIndex = PAGE_INDEX + ITEMS_ABOVE_TOP;

    PauseVerticalScrollIndicator(TOP_ARROW);
    PauseVerticalScrollIndicator(BOTTOM_ARROW);

    if(CURRENT_ITEM_STORAGE_MENU == MENU_WITHDRAW)
    {
        if(gSaveBlock1.pcItems[trueIndex].quantity == 1)
        {
            tQuantity = 1;
            ItemStorage_DoItemWithdraw(taskId);
            return;
        }
        else // _0813A50C
        {
            ItemStorage_PrintItemPcResponse(MSG_HOW_MANY_TO_WITHDRAW);
        }
    }
    else if(gSaveBlock1.pcItems[trueIndex].quantity == 1) // _0813A518
    {
        tQuantity = 1;
        ItemStorage_DoItemToss(taskId);
        return;
    }
    else
    {
        ItemStorage_PrintItemPcResponse(MSG_HOW_MANY_TO_TOSS);
    }
    tQuantity = 1;
    Menu_DrawStdWindowFrame(6, 8, 13, 11);
    sub_80A418C(tQuantity, STR_CONV_MODE_RIGHT_ALIGN, 8, 9, 3);
    TASK.FUNC = ItemStorage_HandleQuantityRolling;
}

static void ItemStorage_HandleQuantityRolling(u8 taskId)
{
    s16 *data = TASK.data;
    u8 trueIndex = PAGE_INDEX + ITEMS_ABOVE_TOP;

    if (JOY_REPEAT(DPAD_UP))
    {
        if(tQuantity != gSaveBlock1.pcItems[trueIndex].quantity)
            tQuantity++;
        else
            tQuantity = 1; // you are at the max amount of items you have when you press Up, set your quantity back to 1.

        sub_80A418C(tQuantity, STR_CONV_MODE_RIGHT_ALIGN, 8, 9, 3); // print quantity?
    }
    else if (JOY_REPEAT(DPAD_DOWN))
    {
        if(tQuantity != 1)
            tQuantity--;
        else
            tQuantity = gSaveBlock1.pcItems[trueIndex].quantity; // you are at 0 when you press down, set your quantity to the amount you have.

        sub_80A418C(tQuantity, STR_CONV_MODE_RIGHT_ALIGN, 8, 9, 3); // print quantity?
    }
    else if (JOY_REPEAT(DPAD_LEFT)) // reduce by 10.
    {
        tQuantity -= 10;

        if(tQuantity <= 0)
            tQuantity = 1; // dont underflow or allow 0!

        sub_80A418C(tQuantity, STR_CONV_MODE_RIGHT_ALIGN, 8, 9, 3); // print quantity?
    }
    else if (JOY_REPEAT(DPAD_RIGHT)) // add 10.
    {
        tQuantity += 10;

        if(tQuantity > gSaveBlock1.pcItems[trueIndex].quantity)
            tQuantity = gSaveBlock1.pcItems[trueIndex].quantity; // dont overflow!

        sub_80A418C(tQuantity, STR_CONV_MODE_RIGHT_ALIGN, 8, 9, 3); // print quantity?
    }
    else if (JOY_NEW(A_BUTTON)) // confirm quantity.
    {
        PlaySE(SE_SELECT);
        Menu_EraseWindowRect(6, 6, 0xD, 0xB);

        if(CURRENT_ITEM_STORAGE_MENU == MENU_WITHDRAW)
            ItemStorage_DoItemWithdraw(taskId);
        else
            ItemStorage_DoItemToss(taskId);
    }
    else if (JOY_NEW(B_BUTTON)) // cancel quantity.
    {
        PlaySE(SE_SELECT);
        Menu_EraseWindowRect(6, 6, 0xD, 0xB);
        StartVerticalScrollIndicators(TOP_ARROW);
        StartVerticalScrollIndicators(BOTTOM_ARROW);
        ItemStorage_PrintItemPcResponse(gSaveBlock1.pcItems[ITEMS_ABOVE_TOP + PAGE_INDEX].itemId); // why not use trueIndex?
        TASK.FUNC = ItemStorage_ProcessInput;
    }
}

static void ItemStorage_DoItemWithdraw(u8 taskId)
{
    s16 *data = TASK.data;
    u8 trueIndex = PAGE_INDEX + ITEMS_ABOVE_TOP;

    if(AddBagItem(gSaveBlock1.pcItems[trueIndex].itemId, tQuantity) == TRUE) // add item works.
    {
        CopyItemName(gSaveBlock1.pcItems[trueIndex].itemId, gStringVar1);
        ConvertIntToDecimalStringN(gStringVar2, tQuantity, 0, 3);
        ItemStorage_PrintItemPcResponse(MSG_WITHDREW_ITEM);
        TASK.FUNC = ItemStorage_HandleRemoveItem;
    }
    else
    {
        tQuantity = 0;
        ItemStorage_PrintItemPcResponse(MSG_NO_MORE_ROOM);
        TASK.FUNC = ItemStorage_HandleErrorMessageInput;
    }
}

static void ItemStorage_DoItemToss(u8 taskId)
{
    s16 *data = TASK.data;
    u8 pos = PAGE_INDEX + ITEMS_ABOVE_TOP;

    if(GetItemImportance(gSaveBlock1.pcItems[pos].itemId) == FALSE)
    {
        CopyItemName(gSaveBlock1.pcItems[pos].itemId, gStringVar1);
        ConvertIntToDecimalStringN(gStringVar2, tQuantity, 0, 3);
        ItemStorage_PrintItemPcResponse(MSG_OKAY_TO_THROW_AWAY);
        DisplayYesNoMenu(7, 6, 1);
        DoYesNoFuncWithChoice(taskId, (struct YesNoFuncTable *)&ItemTossYesNoFuncs);
    }
    else
    {
        tQuantity = 0;
        ItemStorage_PrintItemPcResponse(MSG_TOO_IMPORTANT);
        TASK.FUNC = ItemStorage_HandleRemoveItem;
    }
}

static void ItemStorage_TossItemYes(u8 taskId)
{
    Menu_EraseWindowRect(0x6, 0x6, 0xD, 0xB);
    ItemStorage_PrintItemPcResponse(MSG_THREW_AWAY_ITEM);
    TASK.FUNC = ItemStorage_HandleRemoveItem;
}

static void ItemStorage_TossItemNo(u8 taskId)
{
    s16 *data = TASK.data;

    Menu_EraseWindowRect(0x6, 0x6, 0xD, 0xB);
    InitMenu(0, 16, 2, NUM_PAGE_ITEMS, PAGE_INDEX, 0xD);
    StartVerticalScrollIndicators(TOP_ARROW);
    StartVerticalScrollIndicators(BOTTOM_ARROW);
    ItemStorage_PrintItemPcResponse(gSaveBlock1.pcItems[ITEMS_ABOVE_TOP + PAGE_INDEX].itemId);
    TASK.FUNC = ItemStorage_ProcessInput;
}

static void ItemStorage_HandleRemoveItem(u8 taskId)
{
    s16 *data = TASK.data;
    s16 oldNumItems;

    if (JOY_NEW(A_BUTTON) || gMain.newKeys == B_BUTTON)
    {
        RemovePCItem(PAGE_INDEX + ITEMS_ABOVE_TOP, tQuantity);
        oldNumItems = tUsedSlots;
        tUsedSlots = CountUsedPCItemSlots();

        if(oldNumItems != tUsedSlots && oldNumItems < NUM_PAGE_ITEMS + ITEMS_ABOVE_TOP && ITEMS_ABOVE_TOP != 0)
            ITEMS_ABOVE_TOP--;

        SetPlayerPCListCount(taskId);
        ItemStorage_ReturnToListInput(taskId);
        InitMenu(0, 16, 2, NUM_PAGE_ITEMS, PAGE_INDEX, 0xD);
    }
}

static void ItemStorage_HandleErrorMessageInput(u8 taskId)
{
    s16 *data = TASK.data;

    if (JOY_NEW(A_BUTTON) || gMain.newKeys == B_BUTTON)
    {
        ItemStorage_PrintItemPcResponse(gSaveBlock1.pcItems[ITEMS_ABOVE_TOP + PAGE_INDEX].itemId);
        StartVerticalScrollIndicators(TOP_ARROW);
        StartVerticalScrollIndicators(BOTTOM_ARROW);
        TASK.FUNC = ItemStorage_ProcessInput;
    }
}

static void ItemStorage_ReturnToListInput(u8 taskId)
{
    Menu_EraseWindowRect(0x6, 0x6, 0xD, 0xB);
    StartVerticalScrollIndicators(TOP_ARROW);
    StartVerticalScrollIndicators(BOTTOM_ARROW);
    ItemStorage_DrawBothListAndDescription(taskId);
    TASK.FUNC = ItemStorage_ProcessInput;
}

static void ItemStorage_FinishItemSwap(u8 taskId, bool8 canceled)
{
    s16 *data = TASK.data;
    u8 trueIndex = ITEMS_ABOVE_TOP + PAGE_INDEX;

    SWITCH_MODE_ACTIVE = FALSE;

    if((u8)tUsedSlots > trueIndex && (u8)SWAP_ITEM_INDEX != trueIndex && canceled == FALSE)
    {
        struct ItemSlot itemSlot = gSaveBlock1.pcItems[SWAP_ITEM_INDEX]; // backup the itemSlot before swapping the two.

        gSaveBlock1.pcItems[SWAP_ITEM_INDEX] = gSaveBlock1.pcItems[trueIndex];
        gSaveBlock1.pcItems[trueIndex] = itemSlot;
        return;
    }
    else if(trueIndex == tUsedSlots)
    {
        ItemStorage_PrintItemPcResponse(MSG_GO_BACK_TO_PREV);
    }
    else
    {
        ItemStorage_PrintItemPcResponse(gSaveBlock1.pcItems[trueIndex].itemId);
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
    if(SWAP_ITEM_INDEX - ITEMS_ABOVE_TOP <= 0) { // this check is arbitrary and used to generate the correct assembly using the subtraction, which is what matters. the 0 check doesn't.
        //gSaveBlock1.pcItems[SWAP_ITEM_INDEX].quantity += 100;
        gSaveBlock1.pcItems[SWAP_ITEM_INDEX].quantity += 0; // do not enforce item cap.
    }
}

static void ItemStorage_DrawItemQuantity(u16 arg1, enum StringConvertMode arg2, u8 arg3, u8 arg4, int isSwapSelected)
{
    sub_80A4164(gStringVar1, arg1, arg2, arg4);

    if(isSwapSelected != FALSE)
        Menu_PrintText(gSelectedItemQuantityFormatText, 0x1A, arg3);
    else
        Menu_PrintText(gNonSelectedItemQuantityFormatText, 0x1A, arg3);
}

static void ItemStorage_DrawItemVoidQuantity(u8 var)
{
    Menu_PrintText(gUnknown_08406330, 0x19, var);
}

static void ItemStorage_DrawItemName(struct ItemSlot *itemSlot, u8 var, int isSwapSelected)
{
    CopyItemName(itemSlot->itemId, gStringVar1);

    if(isSwapSelected != FALSE)
        Menu_PrintText(gSelectedItemFormattedText, 16, var);
    else
        Menu_PrintText(gNonSelectedItemFormattedText, 16, var);
}

static void ItemStorage_DrawNormalItemEntry(struct ItemSlot *itemSlot, u8 var, int var2)
{
    ItemStorage_DrawItemName(itemSlot, var, var2);
    ItemStorage_DrawItemQuantity(itemSlot->quantity, STR_CONV_MODE_RIGHT_ALIGN, var, 3, var2);
}

static void ItemStorage_DrawKeyItemEntry(struct ItemSlot *itemSlot, u8 var, int var2)
{
    ItemStorage_DrawItemName(itemSlot, var, var2);
    ItemStorage_DrawItemVoidQuantity(var);
}

static void ItemStorage_DrawTMHMEntry(struct ItemSlot *itemSlot, u8 var, int var2)
{
    ItemStorage_DrawItemName(itemSlot, var, var2);

    if(itemSlot->itemId < ITEM_HM01_CUT)
        ItemStorage_DrawItemQuantity(itemSlot->quantity, STR_CONV_MODE_RIGHT_ALIGN, var, 3, var2);
    else
        ItemStorage_DrawItemVoidQuantity(var); // HMs do not have a quantity.
}

static void ItemStorage_DrawItemList(u8 taskId)
{
    s16 *data = TASK.data;
    int tempArg;
    u16 i;
    u16 yCoord = 0;

    for (i = ITEMS_ABOVE_TOP; i < ITEMS_ABOVE_TOP + NUM_PAGE_ITEMS; i++)
    {
        yCoord = (i - ITEMS_ABOVE_TOP) * 2;

        if (i == tUsedSlots)
        {
            sub_8072A18(gText_Cancel2, 0x80, (yCoord + 2) * 8, 0x68, 1);
            break;
        }
        else
        {
            tempArg = 0;

            if (SWITCH_MODE_ACTIVE != FALSE && i == SWAP_ITEM_INDEX)
                tempArg = 1;

            switch (GetPocketByItemId(gSaveBlock1.pcItems[i].itemId) - 1)
            {
            case 0:
            case 1:
            case 3:
                ItemStorage_DrawNormalItemEntry((struct ItemSlot *)&gSaveBlock1.pcItems[i], yCoord + 2, tempArg);
                break;
            case 4:
                ItemStorage_DrawKeyItemEntry((struct ItemSlot *)&gSaveBlock1.pcItems[i], yCoord + 2, tempArg);
                break;
            case 2:
                ItemStorage_DrawTMHMEntry((struct ItemSlot *)&gSaveBlock1.pcItems[i], yCoord + 2, tempArg);
                break;
            }
        }
    }

    if (i - ITEMS_ABOVE_TOP < 8)
        Menu_BlankWindowRect(16, yCoord + 4, 0x1C, 0x12);

    if (ITEMS_ABOVE_TOP != 0)
        CreateVerticalScrollIndicators(TOP_ARROW, 0xB8, 8);
    else
        DestroyVerticalScrollIndicator(TOP_ARROW);

    if (ITEMS_ABOVE_TOP + NUM_PAGE_ITEMS <= tUsedSlots)
        CreateVerticalScrollIndicators(BOTTOM_ARROW, 0xB8, 0x98);
    else
        DestroyVerticalScrollIndicator(BOTTOM_ARROW);
}

static void ItemStorage_PrintItemPcResponse(u16 itemId)
{
    const u8 *string;

    switch(itemId)
    {
        case MSG_GO_BACK_TO_PREV:
            string = gText_GoBackPrevMenu;
            break;
        case MSG_HOW_MANY_TO_WITHDRAW:
            string = gOtherText_HowManyToWithdraw;
            break;
        case MSG_WITHDREW_ITEM:
            string = gText_WithdrawXItems;
            break;
        case MSG_HOW_MANY_TO_TOSS:
            string = gOtherText_HowManyToToss;
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
            string = gOtherText_SwitchWhichItem;
            break;
        default:
            string = GetItemDescription(itemId);
            break;
    }

    sub_8072AB0(string, 8, 0x68, 0x68, 0x30, 1);
}

static void ItemStorage_DrawBothListAndDescription(u8 taskId)
{
    s16 *data = TASK.data;
    s16 trueIndex = ITEMS_ABOVE_TOP + PAGE_INDEX;

    ItemStorage_DrawItemList(taskId);

    if(SWITCH_MODE_ACTIVE == FALSE)
    {
        if(trueIndex == tUsedSlots)
            ItemStorage_PrintItemPcResponse(MSG_GO_BACK_TO_PREV);
        else
            ItemStorage_PrintItemPcResponse(gSaveBlock1.pcItems[trueIndex].itemId);
    }
}

static void ItemStorage_CreateListMenu(u8 taskId, u8 var)
{
    s16 *data = TASK.data;

    ClearVerticalScrollIndicatorPalettes();
    LoadScrollIndicatorPalette();
    ItemStorage_LoadPalette();
    Menu_DrawStdWindowFrame(0xF, 0, 0x1D, 0x13);
    Menu_DrawStdWindowFrame(0, 0xC, 0xE, 0x13);
    Menu_DrawStdWindowFrame(0, 0, 0xB, 3);
    ItemStorage_PrintItemPcResponse(gSaveBlock1.pcItems[0].itemId);
    Menu_PrintText(sItemStorage_MenuActions[var].text, 1, 1);
    ItemStorage_DrawItemList(taskId);
    InitMenu(0, 0x10, 2, NUM_PAGE_ITEMS, PAGE_INDEX, 0xD);
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
    u8 mailInPC, i;

    for(mailInPC = 0, i = 6; i < 16; i++)
        if(gSaveBlock1.mail[i].itemId != 0)
            mailInPC++;

    return mailInPC;
}

static void Mailbox_CompactMailList(void)
{
    struct Mail mailBuffer;
    u8 i, j;

    for (i=6; i<15; i++)
    {
        for (j=i+1; j<16; j++)
        {
            if (gSaveBlock1.mail[i].itemId == 0)
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

    for (i = eMailboxInfo.itemsAbove; i < eMailboxInfo.itemsAbove + eMailboxInfo.pageItems; i++)
    {
        yCoord = (i - eMailboxInfo.itemsAbove) * 2;
        Menu_BlankWindowRect(0x15, yCoord + 2, 0x1C, yCoord + 3);
        if (i == eMailboxInfo.count)
        {
            Menu_PrintText(gText_Cancel2, 0x15, yCoord + 2);
            break;
        }
        else
        {
            StringCopy(gStringVar1, gSaveBlock1.mail[i + 6].playerName);
            SanitizeNameString(gStringVar1);
            Menu_PrintText(gStringVar1, 0x15, yCoord + 2);
        }
    }

    if (i - eMailboxInfo.itemsAbove != 8)
        Menu_BlankWindowRect(0x15, yCoord + 4, 0x1C, 0x12);

    if (eMailboxInfo.itemsAbove != 0)
        CreateVerticalScrollIndicators(TOP_ARROW, 0xC8, 8);
    else
        DestroyVerticalScrollIndicator(TOP_ARROW);

    if (eMailboxInfo.itemsAbove + eMailboxInfo.pageItems <= eMailboxInfo.count)
        CreateVerticalScrollIndicators(BOTTOM_ARROW, 0xC8, 0x98);
    else
        DestroyVerticalScrollIndicator(BOTTOM_ARROW);
}

static void Mailbox_DrawMailboxMenu(u8 taskId)
{
    ClearVerticalScrollIndicatorPalettes();
    LoadScrollIndicatorPalette();
    Menu_EraseWindowRect(0, 0, 0x1D, 0x13);
    Menu_DrawStdWindowFrame(0, 0, 0x8, 0x3);
    Menu_PrintText(gText_Mailbox, 1, 1);
    Menu_DrawStdWindowFrame(0x14, 0, 0x1D, 0x13);
    Mailbox_DrawMailList(taskId);
    InitMenu(0, 0x15, 2, eMailboxInfo.pageItems, eMailboxInfo.cursorPos, 8);
}

// Mailbox_ProcessInput
static void Mailbox_ProcessInput(u8 taskId)
{
    if(!gPaletteFade.active)
    {
        if (JOY_REPEAT(DPAD_UP))
        {
            if(eMailboxInfo.cursorPos != 0)
            {
                PlaySE(SE_SELECT);
                eMailboxInfo.cursorPos = Menu_MoveCursor(-1);
            }
            else if(eMailboxInfo.itemsAbove != 0)
            {
                PlaySE(SE_SELECT);
                eMailboxInfo.itemsAbove--;
                Mailbox_DrawMailList(taskId);
            }
        }
        else if (JOY_REPEAT(DPAD_DOWN))
        {
            if(eMailboxInfo.cursorPos != eMailboxInfo.pageItems - 1)
            {
                PlaySE(SE_SELECT);
                eMailboxInfo.cursorPos = Menu_MoveCursor(1);
            }
            else if(eMailboxInfo.itemsAbove + eMailboxInfo.cursorPos != eMailboxInfo.count)
            {
                PlaySE(SE_SELECT);
                eMailboxInfo.itemsAbove++;
                Mailbox_DrawMailList(taskId);
            }
        }
        else if (JOY_NEW(A_BUTTON))
        {
            Menu_DestroyCursor();
            PlaySE(SE_SELECT);

            if(eMailboxInfo.itemsAbove + eMailboxInfo.cursorPos == eMailboxInfo.count)
            {
                Mailbox_TurnOff(taskId);
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
            Mailbox_TurnOff(taskId);
        }
    }
}

static void Mailbox_CloseScrollIndicators(void)
{
    BuyMenuFreeMemory();
    DestroyVerticalScrollIndicator(TOP_ARROW);
    DestroyVerticalScrollIndicator(BOTTOM_ARROW);
}

static void Mailbox_PrintWhatToDoWithPlayerMailText(u8 taskId)
{
    Menu_EraseWindowRect(0, 0, 0x1D, 0x13);
    StringCopy(gStringVar1, gSaveBlock1.mail[eMailboxInfo.itemsAbove + 6 + eMailboxInfo.cursorPos].playerName);
    SanitizeNameString(gStringVar1);
    StringExpandPlaceholders(gStringVar4, gText_WhatToDoWithVar1sMail);
    DisplayItemMessageOnField(taskId, gStringVar4, Mailbox_PrintMailOptions, 0);
}

static void Mailbox_ReturnToPlayerPC(u8 taskId)
{
    Menu_EraseWindowRect(0, 0, 0x1D, 0x13);
    ReshowPlayerPC(taskId);
}

static void Mailbox_TurnOff(u8 taskId)
{
    Mailbox_CloseScrollIndicators();
    TASK.FUNC = Mailbox_ReturnToPlayerPC;
}

static void Mailbox_PrintMailOptions(u8 taskId) // Mailbox_PrintMailOptions
{
    Menu_DrawStdWindowFrame(0, 0, 0xC, 0x9);
    Menu_PrintItems(1, 1, 4, (struct MenuAction *)gMailboxMailOptions);
    InitMenu(0, 1, 1, 4, 0, 0xB);
    TASK.FUNC = Mailbox_MailOptionsProcessInput;
}

static void Mailbox_MailOptionsProcessInput(u8 taskId)
{
    if (JOY_REPEAT(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        Menu_MoveCursor(-1);
    }
    else if (JOY_REPEAT(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        Menu_MoveCursor(1);
    }
    else if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        gMailboxMailOptions[Menu_GetCursorPos()].func(taskId);
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
        ReadMail(&gSaveBlock1.mail[eMailboxInfo.itemsAbove + 6 + eMailboxInfo.cursorPos], Mailbox_ReturnToFieldFromReadMail, 1);
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

static const u8 gHighlightedMoveToBagFormatText[] = _("{COLOR RED}{STR_VAR_1}");

static void Mailbox_MoveToBag(u8 taskId)
{
    Menu_DestroyCursor();
    StringCopy(gStringVar1, gText_MoveToBag);
    Menu_PrintText(gHighlightedMoveToBagFormatText, 1, 3); // gHighlightedMoveToBagFormatText
    DisplayItemMessageOnField(taskId, gText_MessageWillBeLost, Mailbox_AskConfirmMoveToBag, 0);
}

static void Mailbox_AskConfirmMoveToBag(u8 taskId)
{
    DisplayYesNoMenu(0x14, 0x8, 0x1);
    DoYesNoFuncWithChoice(taskId, (struct YesNoFuncTable *)&ResumeFromWithdrawYesNoFuncList);
}

static void Mailbox_DoMailMoveToBag(u8 taskId)
{
    struct Mail *mail = &gSaveBlock1.mail[eMailboxInfo.itemsAbove + 6 + eMailboxInfo.cursorPos];

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

        eMailboxInfo.count--;

        if(eMailboxInfo.count < eMailboxInfo.pageItems + eMailboxInfo.itemsAbove && eMailboxInfo.itemsAbove != 0)
            eMailboxInfo.itemsAbove--;

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
        SetMainCallback2(CB2_ShowPartyMenuForItemUse);
        gPokemonItemUseType = ITEM_USE_GIVE_MAIL;
        DestroyTask(taskId);
    }
}

static void Mailbox_UpdateMailListAfterDeposit(void)
{
    u8 taskId = CreateTask(Mailbox_HandleReturnToProcessInput, 0);
    u8 prevCount = eMailboxInfo.count;

    eMailboxInfo.count = GetMailboxMailCount();
    Mailbox_CompactMailList();

    if(prevCount != eMailboxInfo.count && eMailboxInfo.count < eMailboxInfo.pageItems + eMailboxInfo.itemsAbove && eMailboxInfo.itemsAbove != 0) // did the count update?
        eMailboxInfo.itemsAbove--;

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
