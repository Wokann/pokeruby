#include "global.h"
#include "constants/decorations.h"
#include "decoration.h"
#include "decoration_inventory.h"
#include "event_data.h"
#include "main.h"
#include "mauville_old_man.h"
#include "menu.h"
#include "menu_helpers.h"
#include "script.h"
#include "constants/songs.h"
#include "sound.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"

extern u16 gSpecialVar_0x8004;
extern u16 gSpecialVar_0x8005;
extern u16 gSpecialVar_0x8006;

static const u8 * const sDefaultTraderNames[] =
{
    SecretBaseText_Tristan,
    SecretBaseText_Philip,
    SecretBaseText_Dennis,
    SecretBaseText_Roberto,
};

static const u8 sDefaultTraderDecorations[] =
{
    DECOR_DUSKULL_DOLL,
    DECOR_BALL_CUSHION,
    DECOR_TIRE,
    DECOR_PRETTY_FLOWERS,
};

static void SortTraderDecorations(void)
{
    u8 i, j;
    u8 buffer[12];
    struct MauvilleOldManTrader *trader = &gSaveBlock1.oldMan.trader;

    for (i = 0; i < NUM_TRADER_ITEMS - 1; i++)
    {
        for (j = i + 1; j < NUM_TRADER_ITEMS; j++)
        {
            if (trader->decorations[i] == 0)
            {
                u8 temp = trader->decorations[i];
                trader->decorations[i] = trader->decorations[j];
                trader->decorations[j] = temp;
                StringCopy(buffer, trader->playerNames[i]);
                StringCopy(trader->playerNames[i], trader->playerNames[j]);
                StringCopy(trader->playerNames[j], buffer);
            }
        }
    }
}

void TraderSetup(void)
{
    u8 i;
    struct MauvilleOldManTrader *trader = &gSaveBlock1.oldMan.trader;

    trader->id = MAUVILLE_MAN_TRADER;
    trader->alreadyTraded = FALSE;

    for (i = 0; i < NUM_TRADER_ITEMS; i++)
    {
        StringCopy(trader->playerNames[i], sDefaultTraderNames[i]);
        trader->decorations[i] = sDefaultTraderDecorations[i];
    }

    SortTraderDecorations();
}

void Trader_ResetFlag(void)
{
    struct MauvilleOldManTrader *trader = &gSaveBlock1.oldMan.trader;
    trader->alreadyTraded = FALSE;
}

void SetRecycledDecoration(u8 decoration)
{
    VarSet(VAR_RECYCLE_GOODS, decoration);
}

void CreateAvailableDecorationsMenu(u8 taskId)
{
    u8 i;
    u8 numChoices = 1;
    u8 numDecorations = 0;
    struct MauvilleOldManTrader *trader = &gSaveBlock1.oldMan.trader;

    for (i = 0; i < NUM_TRADER_ITEMS; i++)
    {
        if (trader->decorations[i])
        {
            numChoices++;
        }
    }

    Menu_DrawStdWindowFrame(0, 1, 12, numChoices * 2 + 2);

    for (i = 0; i < NUM_TRADER_ITEMS; i++)
    {
        if (trader->decorations[i])
        {
            if (trader->decorations[i] > DECOR_REGISTEEL_DOLL)
            {
                Menu_PrintText(gOtherText_FiveQuestions, 1, numDecorations * 2 + 2);
            }
            else
            {
                Menu_PrintText(gDecorations[trader->decorations[i]].name, 1, numDecorations * 2 + 2);
            }

            numDecorations++;
        }
    }

    Menu_PrintText(gOtherText_CancelNoTerminator, 1, numDecorations * 2 + 2);
    InitMenu(0, 1, 2, numChoices, 0, 11);
    gTasks[taskId].data[1] = numDecorations;
}

void Task_BufferDecorSelectionAndCloseWindow(u8 taskId, u8 decorationId)
{
    if (decorationId > DECOR_REGISTEEL_DOLL)
    {
        gSpecialVar_0x8004 = 0xFFFF;
    }
    else
    {
        gSpecialVar_0x8004 = decorationId;
    }

    Menu_DestroyCursor();
    Menu_EraseWindowRect(0, 1, 12, 12);
    DestroyTask(taskId);
    ScriptContext_Enable();
}

void Task_HandleGetDecorationMenuInput(u8 taskId)
{
    struct MauvilleOldManTrader *trader = &gSaveBlock1.oldMan.trader;

    if (JOY_NEW(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        Menu_MoveCursor(-1);
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        Menu_MoveCursor(1);
    }
    else if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        gSpecialVar_0x8005 = Menu_GetCursorPos();
        if (gTasks[taskId].data[1] == gSpecialVar_0x8005)
        {
            Task_BufferDecorSelectionAndCloseWindow(taskId, 0);
        }
        else
        {
            StringCopy(gStringVar1, trader->playerNames[gSpecialVar_0x8005]);
            Task_BufferDecorSelectionAndCloseWindow(taskId, trader->decorations[gSpecialVar_0x8005]);
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Task_BufferDecorSelectionAndCloseWindow(taskId, 0);
    }
}

void GetTraderTradedFlag(void)
{
    struct MauvilleOldManTrader *trader = &gSaveBlock1.oldMan.trader;
    gSpecialVar_Result = trader->alreadyTraded;
}

void DoesPlayerHaveNoDecorations(void)
{
    u8 i;

    for (i = 0; i < 8; i++)
    {
        if (GetNumDecorationsInInventoryCategory(i))
        {
            gSpecialVar_Result = FALSE;
            return;
        }
    }
    gSpecialVar_Result = TRUE;
}

void IsDecorationCategoryFull(void)
{
    gSpecialVar_Result = FALSE;
    if (gDecorations[gSpecialVar_0x8004].category != gDecorations[gSpecialVar_0x8006].category
        && FindFreeDecorationInventorySlot(gDecorations[gSpecialVar_0x8004].category) == -1)
    {
        CopyDecorationCategoryName(gStringVar2, gDecorations[gSpecialVar_0x8004].category);
        gSpecialVar_Result = TRUE;
    }
}

void TraderShowDecorationMenu(void)
{
    CreateTask(ShowDecorationCategoriesWindow, 0);
}

void DecorationItemsMenuAction_Trade(u8 taskId)
{
    Menu_DestroyCursor();
    Menu_EraseWindowRect(0, 0, 29, 19);
    DestroyVerticalScrollIndicator(TOP_ARROW);
    DestroyVerticalScrollIndicator(BOTTOM_ARROW);
    DestroyDecorationMarkerSprites(gUnknown_020388F7, 8);
    BuyMenuFreeMemory();
    if (IsSelectedDecorationUnused() == TRUE)
    {
        gSpecialVar_0x8006 = gUnknown_020388D0[gUnknown_020388F5];
        StringCopy(gStringVar3, gDecorations[gSpecialVar_0x8004].name);
        StringCopy(gStringVar2, gDecorations[gSpecialVar_0x8006].name);
    }
    else
    {
        gSpecialVar_0x8006 = 0xFFFF;
    }
    DestroyTask(taskId);
    ScriptContext_Enable();
}

void ExitTraderMenu(u8 taskId)
{
    Menu_DestroyCursor();
    Menu_EraseWindowRect(0, 0, 29, 19);
    gSpecialVar_0x8006 = 0;
    DestroyTask(taskId);
    ScriptContext_Enable();
}

void TraderDoDecorationTrade(void)
{
    struct MauvilleOldManTrader *trader = &gSaveBlock1.oldMan.trader;

    RemoveDecorationFromInventory(gSpecialVar_0x8006);
    AddDecoration(gSpecialVar_0x8004);
    StringCopy(trader->playerNames[gSpecialVar_0x8005], gSaveBlock2.playerName);
    trader->decorations[gSpecialVar_0x8005] = gSpecialVar_0x8006;
    SortTraderDecorations();
    trader->alreadyTraded = TRUE;
}

void TraderMenuGetDecoration(void)
{
    u8 taskId = CreateTask(Task_HandleGetDecorationMenuInput, 0);
    CreateAvailableDecorationsMenu(taskId);
}
