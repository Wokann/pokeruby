#include "global.h"
#include "dma3.h"
#include "shop.h"
#include "decompress.h"
#include "field_screen_effect.h"
#include "field_weather.h"
#include "item_menu.h"
#include "main.h"
#include "menu.h"
#include "menu_helpers.h"
#include "money.h"
#include "palette.h"
#include "script.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "tv.h"
#include "scanline_effect.h"
#include "event_object_movement.h"
#include "field_player_avatar.h"
#include "fieldmap.h"
#include "item.h"
#include "decoration.h"
#include "constants/items.h"
#include "constants/songs.h"
#include "overworld.h"
#include "decoration_inventory.h"
#include "field_camera.h"
#include "ewram.h"

extern bool8 SellMenu_QuantityRoller(u8, u8);

extern u8 gShopMenu_Gfx[];
extern u16 gShopMenu_Tilemap[];
extern u16 gShopMenu_Pal[16];

enum
{
    MART_TYPE_NORMAL, // normal mart
    MART_TYPE_DECOR,
    MART_TYPE_DECOR2,
};

// shop view window NPC info enum
enum
{
    OBJ_EVENT_ID,
    X_COORD,
    Y_COORD,
    ANIM_NUM
};

// shop options
enum
{
    SHOP_BUY,
    SHOP_SELL,
    SHOP_EXIT
};

struct MartInfo
{
    /* 0x0 */ void (* callback) (void);
    /* 0x4 */ const u16 *itemList;
    /* 0x8 */ u8 itemCount; // how many unique items are there for sale?
    /* 0x9 */ u8 cursor; // this shows the on-screen true index of the cursor and not the current item selected.
    /* 0xA */ u8 numChoices; // how many options does the mart have? can be either 2 or 1 (BUY/SELL vs BUY)
    /* 0xB */ u8 scrollOffset; // when your cursor is far down, there are choices that have scrolled up past the top. this is the count of that.
    /* 0xC */ u8 martType; // 0-2. 0 is normal mart while 1-2 seem to be decor shops or non-stackable purchases in general.
    /* 0xD */ u8 maxQuantity; // maximum purchase quantity allowed by the player's budget.
};

static void Shop_DisplayPriceInList(int firstItemId, int lastItemId, bool32 hasControlCode);
static void Shop_PrintItemDescText(void);
static void BuyMenuReturnToItemList(u8 taskId);
static void ExitBuyMenu(u8 taskId);
static void Task_ExitBuyMenu(u8 taskId);
static void RecordItemPurchase(u8 taskId);
static void Task_HandleShopMenuBuy(u8 taskId);
static void Task_HandleShopMenuSell(u8 taskId);
static void Task_HandleShopMenuQuit(u8 taskId);
static void BuyMenuTryMakePurchase(u8 taskId);
static void Task_CancelItemPurchase(u8 taskId);
static void Task_ShopMenu(u8 taskId);
static void Task_GoToBuyOrSellMenu(u8 taskId);
static void CB2_InitBuyMenu(void);
static void sub_80B3240(void);
static void BuyMenuAddScrollIndicatorArrows(void);
static void BuyMenuDrawMapGraphics(void);
static void Shop_InitMenus(int, int);
static void Shop_PrintItemDesc(void);
static void Task_BuyMenu(u8 taskId);
static void BuyMenuCollectObjectEventData(void);
static void BuyMenuDrawObjectEvents(void);

// iwram
static struct MartInfo sMartInfo;

// ewram
EWRAM_DATA u32 gMartTotalCost = 0;
EWRAM_DATA s16 gMartViewportObjects[16][4] = {0};
EWRAM_DATA struct ItemSlot gMartPurchaseHistory[3] = {0};
static EWRAM_DATA u8 sPurchaseHistoryId = 0;

EWRAM_DATA u8 gEscalatorAnim_TaskId = 0; // This really should be in fldeff_escalator, but being in a new file aligns the ewram, which doesnt match the ROM.

// rodata
static const struct MenuAction2 sShopMenuActions_BuySellQuit[] =
{
    { gText_ShopBuy, Task_HandleShopMenuBuy },
    { gText_ShopSell, Task_HandleShopMenuSell },
    { gText_ShopQuit, Task_HandleShopMenuQuit },
};

static const u8 gMartBuySellOptionList[] = {SHOP_BUY, SHOP_SELL, SHOP_EXIT};
static const u8 gMartBuyNoSellOptionList[] = {SHOP_BUY, SHOP_EXIT};

static const u16 gUnusedMartArray[] = {0x2, 0x3, 0x4, 0xD, 0x121, 0xE, 0xE, 0xE, 0xE, 0xE, 0xE, 0x0, 0x0};

static const struct YesNoFuncTable sShopPurchaseYesNoFuncs[] =
{
    BuyMenuTryMakePurchase,
    Task_CancelItemPurchase
};

static u8 CreateShopMenu(u8 martType)
{
    LockPlayerFieldControls();
    sMartInfo.martType = martType;
    sMartInfo.cursor = 0;

    if (martType == MART_TYPE_NORMAL)
    {
        sMartInfo.numChoices = 2;
        Menu_DrawStdWindowFrame(0, 0, 10, 7);
        Menu_PrintItemsReordered(1, 1, 3, sShopMenuActions_BuySellQuit, gMartBuySellOptionList);
    }
    else
    {
        sMartInfo.numChoices = 1;
        Menu_DrawStdWindowFrame(0, 0, 10, 5);
        Menu_PrintItemsReordered(1, 1, 2, sShopMenuActions_BuySellQuit, gMartBuyNoSellOptionList);
    }
    InitMenu(0, 1, 1, sMartInfo.numChoices + 1, 0, 9); // add 1 for cancel

    return CreateTask(Task_ShopMenu, 8);
}

static void SetShopMenuCallback(void *callback)
{
    sMartInfo.callback = callback;
}

static void SetShopItemsForSale(const u16 *items)
{
    u16 i = 0;

    sMartInfo.itemList = items;
    sMartInfo.itemCount = 0;

    while (sMartInfo.itemList[i])
    {
        sMartInfo.itemCount++;
        i++;
    }
}

static void Task_ShopMenu(u8 taskId)
{
    const u8 taskIdConst = taskId; // why is a local const needed to match?

    if (JOY_REPEAT(DPAD_UP))
    {
        if (sMartInfo.cursor) // can move cursor up?
        {
            PlaySE(SE_SELECT);
            sMartInfo.cursor = Menu_MoveCursor(-1);
        }
    }
    else if (JOY_REPEAT(DPAD_DOWN))
    {
        if (sMartInfo.cursor != sMartInfo.numChoices) // can move cursor down?
        {
            PlaySE(SE_SELECT);
            sMartInfo.cursor = Menu_MoveCursor(1);
        }
    }
    else if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (sMartInfo.martType == MART_TYPE_NORMAL)
            sShopMenuActions_BuySellQuit[gMartBuySellOptionList[sMartInfo.cursor]].func(taskIdConst);
        else
            sShopMenuActions_BuySellQuit[gMartBuyNoSellOptionList[sMartInfo.cursor]].func(taskIdConst);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Task_HandleShopMenuQuit(taskIdConst);
    }
}

static void Task_HandleShopMenuBuy(u8 taskId)
{
    gTasks[taskId].data[8] = (u32)CB2_InitBuyMenu >> 16;
    gTasks[taskId].data[9] = (u32)CB2_InitBuyMenu;
    gTasks[taskId].func = Task_GoToBuyOrSellMenu;
    FadeScreen(1, 0);
}

static void Task_HandleShopMenuSell(u8 taskId)
{
    gTasks[taskId].data[8] = (u32)CB2_GoToSellMenu >> 16;
    gTasks[taskId].data[9] = (u32)CB2_GoToSellMenu;
    gTasks[taskId].func = Task_GoToBuyOrSellMenu;
    FadeScreen(1, 0);
}

static void Task_HandleShopMenuQuit(u8 taskId)
{
    Menu_DestroyCursor();
    Menu_EraseWindowRect(0, 0, 11, 8);
    TryPutSmartShopperOnAir();
    UnlockPlayerFieldControls();
    DestroyTask(taskId);

    if (sMartInfo.callback)
        sMartInfo.callback(); // run the callback if it exists.
}

static void Task_GoToBuyOrSellMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2((void *)((u16)gTasks[taskId].data[8] << 16 | (u16)gTasks[taskId].data[9]));
        DestroyTask(taskId);
    }
}

static void ShowShopMenuAfterExitingBuyOrSellMenu(u8 taskId)
{
    CreateShopMenu(sMartInfo.martType);
    DestroyTask(taskId);
}

static void Task_ReturnToShopMenu(u8 taskId)
{
    if (IsWeatherNotFadingIn() == 1)
    {
        if (sMartInfo.martType == MART_TYPE_DECOR2)
            DisplayItemMessageOnField(taskId, gText_CanIHelpWithAnythingElse, ShowShopMenuAfterExitingBuyOrSellMenu, 0);
        else
            DisplayItemMessageOnField(taskId, gText_AnythingElseICanHelp, ShowShopMenuAfterExitingBuyOrSellMenu, 0);
    }
}

void MapPostLoadHook_ReturnToShopMenu(void)
{
    FadeInFromBlack();
    CreateTask(Task_ReturnToShopMenu, 0x8);
}

void Shop_RunExitSellMenuTask(u8 taskId)
{
    Task_ReturnToShopMenu(taskId);
}

// unused
void Shop_LoadExitSellMenuTask(u8 taskId)
{
    gTasks[taskId].func = Task_ReturnToShopMenu;
}

static void CB2_BuyMenu(void)
{
    AnimateSprites();
    BuildOamBuffer();
    RunTasks();
    UpdatePaletteFade();
}

static void VBlankCB_BuyMenu(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    DmaCopy16Defvars(3, gBGTilemapBuffers[1], (void *)(VRAM + 0xE800), 0x800);
    DmaCopy16Defvars(3, gBGTilemapBuffers[2], (void *)(VRAM + 0xE000), 0x800);
    DmaCopy16Defvars(3, gBGTilemapBuffers[3], (void *)(VRAM + 0xF000), 0x800);
}

static void CB2_InitBuyMenu(void)
{
    SetVBlankHBlankCallbacksToNull();
    ScanlineEffect_Stop();
    REG_BG1HOFS = 0;
    REG_BG1VOFS = 0;
    REG_BG2HOFS = 0;
    REG_BG2VOFS = 0;
    REG_BG3HOFS = 0;
    REG_BG3VOFS = 0;
    gPaletteFade.bufferTransferDisabled = 1;

    Dma3FillLarge32_(0, (void*)OAM, OAM_SIZE);
    LZDecompressVram(gShopMenu_Gfx, (void*)(VRAM + 0x7C00));
    LZDecompressWram(gShopMenu_Tilemap, ewram18000);
    LoadCompressedPalette(gShopMenu_Pal, 0xC0, sizeof(gShopMenu_Pal));
    FreeAllSpritePalettes();
    ResetPaletteFade();
    ResetSpriteData();
    ResetTasks();
    Text_LoadWindowTemplate(&gWindowTemplate_81E6DFC);
    InitMenuWindow(&gWindowTemplate_81E6DFC);
    BuyMenuDrawMapGraphics();
    sMartInfo.cursor = 0;
    sMartInfo.scrollOffset = 0;
    Menu_EraseWindowRect(0, 0, 0x20, 0x20);
    DrawMoneyBox(gSaveBlock1.money, 0, 0);
    Shop_InitMenus(0, 7);
    Shop_PrintItemDesc();
    BuyMenuAddScrollIndicatorArrows();
    CreateTask(Task_BuyMenu, 0x8);
    sub_80B3240();
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
    gPaletteFade.bufferTransferDisabled = 0;
    SetVBlankCallback(VBlankCB_BuyMenu);
    SetMainCallback2(CB2_BuyMenu);
}

static void sub_80B3240(void)
{
    u16 colors[2] = {RGB(14, 15, 16), RGB_WHITE};

    LoadPalette(&colors[1], 0xD1, sizeof colors[1]);
    LoadPalette(&colors[0], 0xD8, sizeof colors[0]);
}

static void BuyMenuAddScrollIndicatorArrows(void)
{
    ClearVerticalScrollIndicatorPalettes();

    if (sMartInfo.itemCount >= 8)
    {
        CreateVerticalScrollIndicators(TOP_ARROW, 172, 12);
        CreateVerticalScrollIndicators(BOTTOM_ARROW, 172, 148);
        SetVerticalScrollIndicators(TOP_ARROW, INVISIBLE);
    }
}

static void Shop_TryDrawVerticalScrollIndicators(void)
{
    if (sMartInfo.scrollOffset == 0)
        SetVerticalScrollIndicators(TOP_ARROW, INVISIBLE);
    else
        SetVerticalScrollIndicators(TOP_ARROW, VISIBLE);

    if (sMartInfo.scrollOffset + 7 >= sMartInfo.itemCount)
        SetVerticalScrollIndicators(BOTTOM_ARROW, INVISIBLE);
    else
        SetVerticalScrollIndicators(BOTTOM_ARROW, VISIBLE);
}

// what is the point of this function? the tiles always get overwritten by BuyMenuDrawTextboxBG.
static void BuyMenuDrawTextboxBG_Old(u16 *array, s16 offset1, s16 offset2)
{
    array[offset1 + offset2] = 0xC3E1;
    array[offset1 + offset2 + 1] = 0xC3E1;
}

static void BuyMenuDrawMapMetatileLayer(u16 *dest, s16 offset1, s16 offset2, u16 *src)
{
    // This function draws a whole 2x2 metatile.
    dest[offset1 + offset2] = src[0]; // top left
    dest[offset1 + offset2 + 1] = src[1]; // top right
    dest[offset1 + offset2 + 32] = src[2]; // bottom left
    dest[offset1 + offset2 + 33] = src[3]; // bottom right
}

static void BuyMenuDrawMapMetatile(s16 x, s16 y, u16 *src, u8 metatileLayerType)
{
    x <<= 1;
    y = (y <<6) + 0x40;

    switch (metatileLayerType)
    {
    case 0:
        BuyMenuDrawMapMetatileLayer(gBGTilemapBuffers[2], x, y, src);
        BuyMenuDrawMapMetatileLayer(gBGTilemapBuffers[1], x, y, src + 4);
        break;
    case 1:
        BuyMenuDrawMapMetatileLayer(gBGTilemapBuffers[3], x, y, src);
        BuyMenuDrawMapMetatileLayer(gBGTilemapBuffers[2], x, y, src + 4);
        break;
    case 2:
        BuyMenuDrawMapMetatileLayer(gBGTilemapBuffers[3], x, y, src);
        BuyMenuDrawMapMetatileLayer(gBGTilemapBuffers[1], x, y, src + 4);
        break;
    }
}

// used to draw the border tiles around the viewport.
static void BuyMenuDrawMapPartialMetatile(s16 x, s16 y, u16 *src)
{
    x <<=1;
    y = (y << 6) + 0x40;

    BuyMenuDrawMapMetatileLayer(gBGTilemapBuffers[3], x, y, src);
    BuyMenuDrawMapMetatileLayer(gBGTilemapBuffers[2], x, y, src + 4);
}

static void BuyMenuDrawMapBg(void)
{
    s16 facingX, facingY;
    s16 x, y;

    GetXYCoordsOneStepInFrontOfPlayer(&facingX, &facingY);
    facingX -= 3;
    facingY -= 3;

    for (y = 0; y < 6; y++)
    {
        for (x = 0; x < 7; x++)
        {
            u16 metatileId = MapGridGetMetatileIdAt(facingX + x, facingY + y);

            if (y != 5 && x != 6)
            {
                u8 metatileLayerType = MapGridGetMetatileLayerTypeAt(facingX + x, facingY + y);

                if (metatileId < 512)
                    BuyMenuDrawMapMetatile(x, y, (u16 *)gMapHeader.mapLayout->primaryTileset->metatiles + metatileId * 8, metatileLayerType);
                else
                    BuyMenuDrawMapMetatile(x, y, (u16 *)gMapHeader.mapLayout->secondaryTileset->metatiles + (metatileId - 512) * 8, metatileLayerType);
            }
            else
            {
                if (metatileId < 512)
                    BuyMenuDrawMapPartialMetatile(x, y, (u16 *)gMapHeader.mapLayout->primaryTileset->metatiles + metatileId * 8);
                else
                    BuyMenuDrawMapPartialMetatile(x, y, (u16 *)gMapHeader.mapLayout->secondaryTileset->metatiles + (metatileId - 512) * 8);
            }

            if (y == 0 && x != 0 && x != 6)
                BuyMenuDrawTextboxBG_Old(gBGTilemapBuffers[1], x * 2, 64);
        }
    }
}

static void BuyMenuDrawMapGraphics(void)
{
    ClearBGTilemapBuffers();
    BuyMenuCollectObjectEventData();
    BuyMenuDrawObjectEvents();
    BuyMenuDrawMapBg();
}

static void BuyMenuCollectObjectEventData(void)
{
    s16 facingX, facingY;
    u8 playerHeight;
    u8 y, x;
    u8 numObjects = 0;

    GetXYCoordsOneStepInFrontOfPlayer(&facingX, &facingY);
    playerHeight = PlayerGetElevation();
    for (y = 0; y < 16; y++)
        gMartViewportObjects[y][OBJ_EVENT_ID] = 16;
    for (y = 0; y < 5; y++)
    {
        for (x = 0; x < 7; x++)
        {
            u8 objEventId = GetObjectEventIdByPosition(facingX - 3 + x, facingY - 2 + y, playerHeight);

            if (objEventId != 16)
            {
                gMartViewportObjects[numObjects][OBJ_EVENT_ID] = objEventId;
                gMartViewportObjects[numObjects][X_COORD] = x;
                gMartViewportObjects[numObjects][Y_COORD] = y;
                if (gObjectEvents[objEventId].facingDirection == DIR_SOUTH)
                    gMartViewportObjects[numObjects][ANIM_NUM] = 0;
                if (gObjectEvents[objEventId].facingDirection == DIR_NORTH)
                    gMartViewportObjects[numObjects][ANIM_NUM] = 1;
                if (gObjectEvents[objEventId].facingDirection == DIR_WEST)
                    gMartViewportObjects[numObjects][ANIM_NUM] = 2;
                if (gObjectEvents[objEventId].facingDirection == DIR_EAST)
                    gMartViewportObjects[numObjects][ANIM_NUM] = 3;
                numObjects++;
            }
        }
    }
}

static void BuyMenuDrawObjectEvents(void)
{
    u8 i;

    for (i = 0; i < 16; i++) // max objects?
    {
        if (gMartViewportObjects[i][OBJ_EVENT_ID] == 16)
            continue;

        StartSpriteAnim(&gSprites[CreateObjectGraphicsSprite(
            gObjectEvents[gMartViewportObjects[i][OBJ_EVENT_ID]].graphicsId,
            SpriteCallbackDummy,
            (u16)gMartViewportObjects[i][X_COORD] * 16 + 8,
            (u16)gMartViewportObjects[i][Y_COORD] * 16 + 32,
            2)],
            gMartViewportObjects[i][ANIM_NUM]);
    }
}

static void BuyMenuCopyMenuBgToBg1TilemapBuffer(void)
{
    s16 i;
    
    for (i = 0; i < 0x400; i++)
    {
        if (ewram18000[i] != 0)
            gBGTilemapBuffers[1][i] = ewram18000[i] + 0xC3E0;
    }
}

static void Shop_InitMenus(int firstItemId, int lastItemId)
{
    BuyMenuCopyMenuBgToBg1TilemapBuffer();
    Shop_DisplayPriceInList(firstItemId, lastItemId, 0);
    InitMenu(0, 0xE, 0x2, 0x8, sMartInfo.cursor, 0xF);
}

// after printing the item quantity and price, restore the textbox tiles before the Yes/No prompt.
static void BuyMenuDrawTextboxBG_Restore(void)
{
    u16 i, j;

    for (i = 0; i < 8; i++)
        for (j = 0; j < 14; j++)
            gBGTilemapBuffers[1][32 * (i + 12) + j] = ewram18000[0x180 + 32 * i + j] + 0xC3E0;
}

static void Shop_PrintItemDesc(void)
{
    Shop_PrintItemDescText();
}

#define tItemCount data[1]

static void BuyMenuPrintItemQuantityAndPrice(u8 taskId)
{
    u16 itemListIndex = sMartInfo.scrollOffset + sMartInfo.cursor;
    u16 itemId = sMartInfo.itemList[itemListIndex];
    u32 price = (GetItemPrice(itemId) >> IsPokeNewsActive(1));

    PrintMoneyAmount(gTasks[taskId].tItemCount * price, 6, 6, 11);
    gStringVar1[0] = EXT_CTRL_CODE_BEGIN;
    gStringVar1[1] = 0x14;
    gStringVar1[2] = 0x6;
    ConvertIntToDecimalStringN(&gStringVar1[3], gTasks[taskId].tItemCount, 1, 2);
    Menu_PrintText(gText_xVar1, 1, 11);
    sub_80A3FA0(gBGTilemapBuffers[1], 1, 11, 12, 2, 0xC3E1);
}

static void Shop_DisplayNormalPriceInList(u16 itemId, u8 y, bool32 hasControlCode)
{
    u8 *stringPtr = gStringVar1;

    if (hasControlCode)
    {
        stringPtr[0] = EXT_CTRL_CODE_BEGIN;
        stringPtr[1] = 0x1;
        stringPtr[2] = 0x2;
        stringPtr += 3;
    }

    CopyItemName(itemId, stringPtr);

    sub_8072A18(&gStringVar1[0], 0x70, y << 3, 0x58, 0x1);
    stringPtr = gStringVar1;

    if (hasControlCode)
        stringPtr = &gStringVar1[3];

    GetMoneyAmountText(stringPtr, (GetItemPrice(itemId) >> IsPokeNewsActive(1)), 0x4);
    Menu_PrintTextPixelCoords(&gStringVar1[0], 0xCA, y << 3, 1);
}

static void Shop_DisplayDecorationPriceInList(u16 itemId, u8 y, bool32 hasControlCode)
{
    u8 *stringPtr = gStringVar1;

    if (hasControlCode)
    {
        stringPtr[0] = EXT_CTRL_CODE_BEGIN;
        stringPtr[1] = 0x1;
        stringPtr[2] = 0x2;
        stringPtr += 3;
    }

    StringCopy(stringPtr, gDecorations[itemId].name);
    sub_8072A18(&gStringVar1[0], 0x70, y << 3, 0x58, 0x1);
    stringPtr = gStringVar1;

    if (hasControlCode)
        stringPtr = &gStringVar1[3];

    // some names are the maximum string length for a shop item. Because there is no room for
    // a 6 character price (including the currency), a sprite is instead used for anything that
    // is the maximum decoration price in order to fit it on screen.
    if (gDecorations[itemId].price == 10000)
    {
        Draw10000Sprite(0x19, y, hasControlCode);
    }
    else
    {
        GetMoneyAmountText(stringPtr, gDecorations[itemId].price, 0x4);
        Menu_PrintTextPixelCoords(&gStringVar1[0], 0xCA, y << 3, 0x1);
    }
}

static void Shop_DisplayPriceInList(int firstItemId, int lastItemId, bool32 hasControlCode)
{
    u8 i;

    for (i = firstItemId; i <= lastItemId && sMartInfo.scrollOffset + i < sMartInfo.itemCount; i++)
    {
        if (sMartInfo.martType == MART_TYPE_NORMAL)
            Shop_DisplayNormalPriceInList(sMartInfo.itemList[sMartInfo.scrollOffset + i], (i << 1) + 2, hasControlCode);
        else
            Shop_DisplayDecorationPriceInList(sMartInfo.itemList[sMartInfo.scrollOffset + i], (i << 1) + 2, hasControlCode);
    }

    if (i != 8 && sMartInfo.scrollOffset + i == sMartInfo.itemCount)
    {
        Menu_BlankWindowRect(0xE, (i << 1) + 2, 0x1C, (i << 1) + 3);
        Menu_PrintText(gText_Cancel2, 0xE, (i << 1) + 2);
    }
}

static void Shop_PrintItemDescText(void)
{
    if (sMartInfo.scrollOffset + sMartInfo.cursor != sMartInfo.itemCount)
    {
        if (sMartInfo.martType == MART_TYPE_NORMAL)
        {
            sub_8072AB0(GetItemDescription(sMartInfo.itemList[sMartInfo.scrollOffset + sMartInfo.cursor]),
                0x4, 0x68, 0x68, 0x30, 0);
        }
        else
        {
            sub_8072AB0(gDecorations[sMartInfo.itemList[sMartInfo.scrollOffset + sMartInfo.cursor]].description,
                0x4, 0x68, 0x68, 0x30, 0);
        }
    }
    else
    {
        sub_8072AB0(gText_QuitShopping, 0x4, 0x68, 0x68, 0x30, 0);
    }
}

static void Task_ReturnToItemListAfterItemPurchase(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        Shop_DisplayPriceInList(sMartInfo.cursor, sMartInfo.cursor, 0);
        PlaySE(SE_SELECT);

        if (sMartInfo.itemList[sMartInfo.scrollOffset + sMartInfo.cursor] == ITEM_POKE_BALL && gTasks[taskId].tItemCount >= 10 && AddBagItem(ITEM_PREMIER_BALL, 1) == TRUE)
            DisplayItemMessageOnField(taskId, gText_ThrowInPremierBall, BuyMenuReturnToItemList, 0xC3E1);
        else
            BuyMenuReturnToItemList(taskId);
    }
}

static void BuyMenuSubtractMoney(u8 taskId)
{
    IncrementGameStat(GAME_STAT_SHOPPED);
    RemoveMoney(&gSaveBlock1.money, gMartTotalCost);
    PlaySE(SE_SHOP);
    ChangeAmountInMoneyBox(gSaveBlock1.money, 0, 0);
    gTasks[taskId].func = Task_ReturnToItemListAfterItemPurchase;
}

static void Shop_DoPricePrintAndReturnToBuyMenu(u8 taskId)
{
    Shop_DisplayPriceInList(sMartInfo.cursor, sMartInfo.cursor, 0);
    BuyMenuReturnToItemList(taskId);
}

static void BuyMenuTryMakePurchase(u8 taskId)
{
    Menu_EraseWindowRect(0x7, 0x8, 0xD, 0xD);
    sub_80A3FA0(gBGTilemapBuffers[1], 8, 9, 4, 4, 0);
    BuyMenuDrawTextboxBG_Restore();
    BuyMenuDrawMapBg();

    if (IsEnoughMoney(gSaveBlock1.money, gMartTotalCost))
    {
        if (sMartInfo.martType == MART_TYPE_NORMAL)
        {
            if (AddBagItem(sMartInfo.itemList[sMartInfo.scrollOffset + sMartInfo.cursor], gTasks[taskId].tItemCount))
            {
                DisplayItemMessageOnField(taskId, gText_HereYouGoThankYou, BuyMenuSubtractMoney, 0xC3E1);
                RecordItemPurchase(taskId);
            }
            else
            {
                DisplayItemMessageOnField(taskId, gText_NoMoreRoomForThis, Shop_DoPricePrintAndReturnToBuyMenu, 0xC3E1);
            }
        }
        else // a normal mart is only type 0, so types 1 and 2 are decoration marts.
        {
            if (DecorationAdd(sMartInfo.itemList[sMartInfo.scrollOffset + sMartInfo.cursor]))
            {
                if (sMartInfo.martType == MART_TYPE_DECOR)
                    DisplayItemMessageOnField(taskId, gText_ThankYouIllSendItHome, BuyMenuSubtractMoney, 0xC3E1);
                else
                    DisplayItemMessageOnField(taskId, gText_ThanksIllSendItHome, BuyMenuSubtractMoney, 0xC3E1);
            }
            else
            {
                StringExpandPlaceholders(gStringVar4, gText_SpaceForVar1Full);
                DisplayItemMessageOnField(taskId, gStringVar4, Shop_DoPricePrintAndReturnToBuyMenu, 0xC3E1);
            }
        }
    }
    else
    {
        DisplayItemMessageOnField(taskId, gText_YouDontHaveMoney, Shop_DoPricePrintAndReturnToBuyMenu, 0xC3E1);
    }
}

static void BuyMenuConfirmPurchase(u8 taskId)
{
    DisplayYesNoMenu(7, 8, 1);
    sub_80A3FA0(gBGTilemapBuffers[1], 8, 9, 4, 4, 0xC3E1);
    DoYesNoFuncWithChoice(taskId, sShopPurchaseYesNoFuncs);
}

static void Task_CancelItemPurchase(u8 taskId)
{
    Shop_DisplayPriceInList(sMartInfo.cursor, sMartInfo.cursor, 0);
    Menu_EraseWindowRect(0x7, 0x8, 0xD, 0xD);
    sub_80A3FA0(gBGTilemapBuffers[1], 0x8, 0x9, 0x4, 0x4, 0);
    BuyMenuReturnToItemList(taskId);
}

static void Task_BuyHowManyDialogueHandleInput(u8 taskId)
{
    if (SellMenu_QuantityRoller(taskId, sMartInfo.maxQuantity) == TRUE)
        BuyMenuPrintItemQuantityAndPrice(taskId);

    if (JOY_NEW(A_BUTTON))
    {
        gMartTotalCost = (GetItemPrice(sMartInfo.itemList[sMartInfo.scrollOffset + sMartInfo.cursor]) >> IsPokeNewsActive(1)) * gTasks[taskId].tItemCount; // set total cost of your purchase.
        Menu_EraseWindowRect(0, 0xA, 0xD, 0xD);
        sub_80A3FA0(gBGTilemapBuffers[1], 0x1, 0xB, 0xC, 0x2, 0);
        BuyMenuDrawTextboxBG_Restore();
        BuyMenuDrawMapBg();
        CopyItemName(sMartInfo.itemList[sMartInfo.scrollOffset + sMartInfo.cursor], gStringVar1);
        ConvertIntToDecimalStringN(gStringVar2, gTasks[taskId].tItemCount, 0, 0x2);
        ConvertIntToDecimalStringN(gStringVar3, gMartTotalCost, 0, 0x8);
        StringExpandPlaceholders(gStringVar4, gText_Var1AndYouWantedVar2);
        DisplayItemMessageOnField(taskId, gStringVar4, BuyMenuConfirmPurchase, 0xC3E1);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        Shop_DisplayPriceInList(sMartInfo.cursor, sMartInfo.cursor, 0);
        BuyMenuReturnToItemList(taskId);
    }
}

// set the item count in the mart info to the maximum allowed by the player's budget.
static void Task_BuyHowManyDialogueInit(u8 taskId)
{
    u16 maxQuantity;

    gTasks[taskId].tItemCount = 1;
    Menu_DrawStdWindowFrame(0, 0xA, 0xD, 0xD);
    BuyMenuPrintItemQuantityAndPrice(taskId);

    maxQuantity = gSaveBlock1.money / (GetItemPrice(sMartInfo.itemList[sMartInfo.scrollOffset + sMartInfo.cursor]) >> IsPokeNewsActive(1));
    if (maxQuantity > 99)
        sMartInfo.maxQuantity = 99;
    else
        sMartInfo.maxQuantity = maxQuantity;

    gTasks[taskId].func = Task_BuyHowManyDialogueHandleInput;
}

#ifdef NONMATCHING
static void Shop_MoveItemListUp(void)
{
    u16 *r1;
    u16 *r2;
    register u8 *r10 asm("r10");
    s32 i;
    s32 j;
    struct Window *r8 = &gMenuWindow;
    
    r1 = r8->tilemap;
    r1 += 0x1EF;
    r2 = r1;
    r2 += 64;
    r10 = r8->tileData;
    
    for (i = 0; i < 14; i++)
    {
        for (j = 0; j < 15; j++)
        {
            if ((r1[j] & 0x3FF) <= r8->tileDataStartOffset + 1)
                r2[j] = r8->tileDataStartOffset + 1;
            else
                r2[j] = r1[j] + 0x3C;
        }
        
        r1 -= 32;
        r2 -= 32;
    }
    
    {
        u8 *r1 = r10 + 0x3A20;
        u8 *r2 = r1 + 0x780;
        for (i = 0; i < 14; i++)
        {
            DmaCopy16(3, r1, r2, 0x1E0);
            r2 -= 0x3C0;
            r1 -= 0x3C0;
        }
    }
}
#else
NAKED
static void Shop_MoveItemListUp(void)
{
    asm(".syntax unified\n\
    push {r4-r7,lr}\n\
    mov r7, r10\n\
    mov r6, r9\n\
    mov r5, r8\n\
    push {r5-r7}\n\
    sub sp, 0x4\n\
    ldr r0, _080B4020 @ =gMenuWindow\n\
    mov r8, r0\n\
    ldr r1, [r0, 0x28]\n\
    ldr r3, _080B4024 @ =0x000003de\n\
    adds r1, r3\n\
    adds r2, r1, 0\n\
    adds r2, 0x80\n\
    ldr r7, [r0, 0x24]\n\
    mov r10, r7\n\
    ldr r0, _080B4028 @ =0x000003ff\n\
    mov r9, r0\n\
    movs r6, 0xD\n\
_080B3FAC:\n\
    adds r3, r2, 0\n\
    subs r3, 0x40\n\
    str r3, [sp]\n\
    movs r7, 0x40\n\
    negs r7, r7\n\
    adds r7, r1\n\
    mov r12, r7\n\
    adds r3, r2, 0\n\
    adds r4, r1, 0\n\
    movs r5, 0xE\n\
_080B3FC0:\n\
    ldrh r2, [r4]\n\
    mov r1, r9\n\
    ands r1, r2\n\
    mov r7, r8\n\
    ldrh r0, [r7, 0x1A]\n\
    adds r0, 0x1\n\
    cmp r1, r0\n\
    ble _080B3FD4\n\
    adds r0, r2, 0\n\
    adds r0, 0x3C\n\
_080B3FD4:\n\
    strh r0, [r3]\n\
    adds r3, 0x2\n\
    adds r4, 0x2\n\
    subs r5, 0x1\n\
    cmp r5, 0\n\
    bge _080B3FC0\n\
    ldr r2, [sp]\n\
    mov r1, r12\n\
    subs r6, 0x1\n\
    cmp r6, 0\n\
    bge _080B3FAC\n\
    ldr r1, _080B402C @ =0x00003a20\n\
    add r1, r10\n\
    movs r0, 0xF0\n\
    lsls r0, 3\n\
    adds r2, r1, r0\n\
    ldr r3, _080B4030 @ =0x040000d4\n\
    ldr r5, _080B4034 @ =0x800000f0\n\
    ldr r4, _080B4038 @ =0xfffffc40\n\
    movs r6, 0xD\n\
_080B3FFC:\n\
    str r1, [r3]\n\
    str r2, [r3, 0x4]\n\
    str r5, [r3, 0x8]\n\
    ldr r0, [r3, 0x8]\n\
    adds r2, r4\n\
    adds r1, r4\n\
    subs r6, 0x1\n\
    cmp r6, 0\n\
    bge _080B3FFC\n\
    add sp, 0x4\n\
    pop {r3-r5}\n\
    mov r8, r3\n\
    mov r9, r4\n\
    mov r10, r5\n\
    pop {r4-r7}\n\
    pop {r0}\n\
    bx r0\n\
    .align 2, 0\n\
_080B4020: .4byte gMenuWindow\n\
_080B4024: .4byte 0x000003de\n\
_080B4028: .4byte 0x000003ff\n\
_080B402C: .4byte 0x00003a20\n\
_080B4030: .4byte 0x040000d4\n\
_080B4034: .4byte 0x800000f0\n\
_080B4038: .4byte 0xfffffc40\n\
    .syntax divided");
}
#endif

#ifdef NONMATCHING
static void Shop_MoveItemListDown(void)
{
    u16 *r1;
    u16 *r2;
    u8 *r10;
    s32 i;
    s32 j;
    struct Window *r8 = &gMenuWindow;
    
    r1 = r8->tilemap;
    r1 += 0x4F;
    r2 = r1;
    r2 += 64;
    r10 = r8->tileData;
    
    for (i = 0; i < 14; i++)
    {
        for (j = 0; j < 15; j++)
        {
            if ((r1[j] & 0x3FF) <= r8->tileDataStartOffset + 1)
                r2[j] = r8->tileDataStartOffset + 1;
            else
                r2[j] = r1[j] + 0x3C;
        }
        
        r1 += 32;
        r2 += 32;
    }

    {
        register u8 *r1 asm("r1") = r10 + 0x960;
        register u8 *r2 asm("r2") = r1;
       
        r1 += 0x780;
        for (i = 0; i < 14; i++)
        {
            DmaCopy16(3, r1, r2, 0x1E0);
            r1 += 0x3C0;
            r2 += 0x3C0;
        }
    }
}
#else
NAKED
static void Shop_MoveItemListDown(void)
{
    asm(".syntax unified\n\
    push {r4-r7,lr}\n\
    mov r7, r10\n\
    mov r6, r9\n\
    mov r5, r8\n\
    push {r5-r7}\n\
    sub sp, 0x4\n\
    ldr r0, _080B40D8 @ =gMenuWindow\n\
    mov r8, r0\n\
    ldr r2, [r0, 0x28]\n\
    adds r1, r2, 0\n\
    adds r1, 0x9E\n\
    adds r2, r1, 0\n\
    adds r1, 0x80\n\
    ldr r3, [r0, 0x24]\n\
    mov r10, r3\n\
    ldr r7, _080B40DC @ =0x000003ff\n\
    mov r9, r7\n\
    movs r6, 0xD\n\
_080B4060:\n\
    adds r0, r2, 0\n\
    adds r0, 0x40\n\
    str r0, [sp]\n\
    movs r3, 0x40\n\
    adds r3, r1\n\
    mov r12, r3\n\
    adds r3, r2, 0\n\
    adds r4, r1, 0\n\
    movs r5, 0xE\n\
_080B4072:\n\
    ldrh r2, [r4]\n\
    mov r1, r9\n\
    ands r1, r2\n\
    mov r7, r8\n\
    ldrh r0, [r7, 0x1A]\n\
    adds r0, 0x1\n\
    cmp r1, r0\n\
    ble _080B4086\n\
    adds r0, r2, 0\n\
    subs r0, 0x3C\n\
_080B4086:\n\
    strh r0, [r3]\n\
    adds r3, 0x2\n\
    adds r4, 0x2\n\
    subs r5, 0x1\n\
    cmp r5, 0\n\
    bge _080B4072\n\
    ldr r2, [sp]\n\
    mov r1, r12\n\
    subs r6, 0x1\n\
    cmp r6, 0\n\
    bge _080B4060\n\
    movs r1, 0x96\n\
    lsls r1, 4\n\
    add r1, r10\n\
    adds r2, r1, 0\n\
    movs r0, 0xF0\n\
    lsls r0, 3\n\
    adds r1, r0\n\
    ldr r3, _080B40E0 @ =0x040000d4\n\
    ldr r5, _080B40E4 @ =0x800000f0\n\
    movs r4, 0xF0\n\
    lsls r4, 2\n\
    movs r6, 0xD\n\
_080B40B4:\n\
    str r1, [r3]\n\
    str r2, [r3, 0x4]\n\
    str r5, [r3, 0x8]\n\
    ldr r0, [r3, 0x8]\n\
    adds r2, r4\n\
    adds r1, r4\n\
    subs r6, 0x1\n\
    cmp r6, 0\n\
    bge _080B40B4\n\
    add sp, 0x4\n\
    pop {r3-r5}\n\
    mov r8, r3\n\
    mov r9, r4\n\
    mov r10, r5\n\
    pop {r4-r7}\n\
    pop {r0}\n\
    bx r0\n\
    .align 2, 0\n\
_080B40D8: .4byte gMenuWindow\n\
_080B40DC: .4byte 0x000003ff\n\
_080B40E0: .4byte 0x040000d4\n\
_080B40E4: .4byte 0x800000f0\n\
    .syntax divided");
}
#endif

static void Task_BuyMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        if (JOY_REPEAT(DPAD_ANY) == DPAD_UP) // only up can be pressed
        {
            if (sMartInfo.cursor == 0)
            {
                if (sMartInfo.scrollOffset == 0) // if there are no choices above, dont bother
                    return;

                PlaySE(SE_SELECT);
                sMartInfo.scrollOffset--; // since cursor is at the top and there are choices above the top, scroll the menu up by updating scrollOffset.
                Shop_MoveItemListUp();
                Shop_DisplayPriceInList(0, 0, 0);
                Shop_PrintItemDescText();
                Shop_TryDrawVerticalScrollIndicators();
            }
            else // if the cursor is not 0, scrollOffset cannot be updated yet since the cursor is at the top of the menu, so update cursor.
            {
                PlaySE(SE_SELECT);
                sMartInfo.cursor = Menu_MoveCursor(-1); // move cursor up
                Shop_PrintItemDescText();
            }
        }
        else if (JOY_REPEAT(DPAD_ANY) == DPAD_DOWN) // only down can be pressed
        {
            if (sMartInfo.cursor == 7) // are you at the bottom of the menu?
            {
                if (sMartInfo.scrollOffset + sMartInfo.cursor == sMartInfo.itemCount) // are you at cancel?
                    return;

                PlaySE(SE_SELECT);
                sMartInfo.scrollOffset++;
                Shop_MoveItemListDown();
                Shop_DisplayPriceInList(7, 7, 0);
                Shop_PrintItemDescText();
                Shop_TryDrawVerticalScrollIndicators();
            }
            else if (sMartInfo.cursor != sMartInfo.itemCount)
            {
                PlaySE(SE_SELECT);
                sMartInfo.cursor = Menu_MoveCursor(1);
                Shop_PrintItemDescText();
            }
        }
        else if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);

            if (sMartInfo.scrollOffset + sMartInfo.cursor != sMartInfo.itemCount) // did you not hit CANCEL?
            {
                PauseVerticalScrollIndicator(TOP_ARROW);
                PauseVerticalScrollIndicator(BOTTOM_ARROW);
                SetVerticalScrollIndicators(BOTTOM_ARROW, INVISIBLE);
                Shop_DisplayPriceInList(sMartInfo.cursor, sMartInfo.cursor, 1);
                Menu_DestroyCursor();
                Menu_EraseWindowRect(0, 0xC, 0xD, 0x13);

                if (sMartInfo.martType == MART_TYPE_NORMAL)
                {
                    gMartTotalCost = (GetItemPrice(sMartInfo.itemList[sMartInfo.scrollOffset + sMartInfo.cursor]) >> IsPokeNewsActive(1)); // set 1x price
                    if (!IsEnoughMoney(gSaveBlock1.money, gMartTotalCost))
                    {
                        DisplayItemMessageOnField(taskId, gText_YouDontHaveMoney, Shop_DoPricePrintAndReturnToBuyMenu, 0xC3E1); // tail merge
                    }
                    else // _080B42BA
                    {
                        CopyItemName(sMartInfo.itemList[sMartInfo.scrollOffset + sMartInfo.cursor], gStringVar1);
                        StringExpandPlaceholders(gStringVar4, gText_Var1CertainlyHowMany);
                        DisplayItemMessageOnField(taskId, gStringVar4, Task_BuyHowManyDialogueInit, 0xC3E1);
                    }
                }
                else // _080B428C
                {
                    gMartTotalCost = gDecorations[sMartInfo.itemList[sMartInfo.scrollOffset + sMartInfo.cursor]].price;

                    if (!IsEnoughMoney(gSaveBlock1.money, gMartTotalCost))
                    {
                        DisplayItemMessageOnField(taskId, gText_YouDontHaveMoney, Shop_DoPricePrintAndReturnToBuyMenu, 0xC3E1); // tail merge
                    }
                    else
                    {    
                        StringCopy(gStringVar1, gDecorations[sMartInfo.itemList[sMartInfo.scrollOffset + sMartInfo.cursor]].name);
                        ConvertIntToDecimalStringN(gStringVar2, gMartTotalCost, 0, 0x8);

                        if (sMartInfo.martType == MART_TYPE_DECOR)
                            StringExpandPlaceholders(gStringVar4, gText_Var1IsItThatllBeVar2);
                        else
                            StringExpandPlaceholders(gStringVar4, gText_YouWantedVar1ThatllBeVar2);
                        DisplayItemMessageOnField(taskId, gStringVar4, BuyMenuConfirmPurchase, 0xC3E1);
                    }
                }
            }
            else
            {
                ExitBuyMenu(taskId);
            }
        }
        else if (JOY_NEW(B_BUTTON)) // go back to buy/sell/exit menu
        {
            PlaySE(SE_SELECT);
            ExitBuyMenu(taskId);
        }
    }
}

static void BuyMenuReturnToItemList(u8 taskId)
{
    Menu_EraseWindowRect(0, 0xE, 0x1D, 0x13);
    Menu_EraseWindowRect(0, 0xA, 0xD, 0xD);
    sub_80A3FA0(gBGTilemapBuffers[1], 0x1, 0xB, 0xC, 0x2, 0);
    BuyMenuDrawMapBg();
    Shop_InitMenus(6, 7);
    Shop_PrintItemDesc();
    StartVerticalScrollIndicators(TOP_ARROW);
    StartVerticalScrollIndicators(BOTTOM_ARROW);
    Shop_TryDrawVerticalScrollIndicators();
    gTasks[taskId].func = Task_BuyMenu;
}

static void ExitBuyMenu(u8 taskId)
{
    gFieldCallback = MapPostLoadHook_ReturnToShopMenu;
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = Task_ExitBuyMenu;
}

static void Task_ExitBuyMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        HideMoneyBox(0, 0);
        BuyMenuFreeMemory();
        SetMainCallback2(CB2_ReturnToField);
        DestroyTask(taskId);
    }
}

static void RecordItemPurchase(u8 taskId)
{
    u16 i;

    for (i = 0; i < 3; i++)
    {
        if (gMartPurchaseHistory[i].itemId == sMartInfo.itemList[sMartInfo.scrollOffset + sMartInfo.cursor]
         && gMartPurchaseHistory[i].quantity != 0)
        {
            if (gMartPurchaseHistory[i].quantity + gTasks[taskId].tItemCount > 255)
                gMartPurchaseHistory[i].quantity = 255;
            else
                gMartPurchaseHistory[i].quantity += gTasks[taskId].tItemCount;
            return;
        }
    }

    if (sPurchaseHistoryId < 3)
    {
        gMartPurchaseHistory[sPurchaseHistoryId].itemId = sMartInfo.itemList[sMartInfo.scrollOffset + sMartInfo.cursor];
        gMartPurchaseHistory[sPurchaseHistoryId].quantity = gTasks[taskId].tItemCount;
        sPurchaseHistoryId++;
    }
}

#undef tItemCount

static void ClearItemPurchases(void)
{
    sPurchaseHistoryId = 0;
    ClearItemSlots(gMartPurchaseHistory, 3);
}

void CreatePokemartMenu(const u16 *itemsForSale)
{
    CreateShopMenu(MART_TYPE_NORMAL);
    SetShopItemsForSale(itemsForSale);
    ClearItemPurchases();
    SetShopMenuCallback(ScriptContext_Enable);
}

void CreateDecorationShop1Menu(const u16 *itemsForSale)
{
    CreateShopMenu(MART_TYPE_DECOR);
    SetShopItemsForSale(itemsForSale);
    SetShopMenuCallback(ScriptContext_Enable);
}

void CreateDecorationShop2Menu(const u16 *itemsForSale)
{
    CreateShopMenu(MART_TYPE_DECOR2);
    SetShopItemsForSale(itemsForSale);
    SetShopMenuCallback(ScriptContext_Enable);
}

#if DEBUG

void debug_sub_80C2818(void)
{
    CreateShopMenu(MART_TYPE_NORMAL);
    SetShopItemsForSale(gUnusedMartArray);
    SetShopMenuCallback(NULL);
}

#endif
