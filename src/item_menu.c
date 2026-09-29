#include "global.h"
#include "berry.h"
#include "berry_tag_screen.h"
#include "data2.h"
#include "decompress.h"
#include "field_effect.h"
#include "event_object_movement.h"
#include "field_player_avatar.h"
#include "graphics.h"
#include "item.h"
#include "constants/items.h"
#include "item_menu.h"
#include "item_use.h"
#include "link.h"
#include "mail.h"
#include "main.h"
#include "map_name_popup.h"
#include "menu.h"
#include "menu_cursor.h"
#include "menu_helpers.h"
#include "money.h"
#include "palette.h"
#include "party_menu.h"
#include "player_pc.h"
#include "pokemon_menu.h"
#include "overworld.h"
#include "script.h"
#include "constants/songs.h"
#include "sound.h"
#include "sprite.h"
#include "strings.h"
#include "strings2.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "scanline_effect.h"
#include "menu_helpers.h"
#include "ewram.h"
#include "berry_blender.h"
#include "field_fadetransition.h"
#include "shop.h"
#include "field_weather.h"
#include "pokemon_storage_system.h"
#include "event_object_lock.h"

struct BagDescriptionPrinter
{
    u8 state;
    u8 textLine;
    u8 messageType;
};

struct BagItemListPrinter
{
    u8 state;
    u8 nextRow;
    u8 isShortList;
};

struct DeferredBagItemMessage
{
    u8 isPending;
    u8 taskId;
    u16 messageMode;
    const u8 *text;
    TaskFunc callback;
};

struct ItemMenuOptionsPrinter
{
    u8 state;
    u8 nextOption;
    u8 isGridLayout;
    u8 topRow;
};

struct BagItemListRedrawState
{
    u8 isPending;
};

enum
{
    BAG_POCKET_ITEMS,       // 0
    BAG_POCKET_POKE_BALLS,  // 1
    BAG_POCKET_TMs_HMs,     // 2
    BAG_POCKET_BERRIES,     // 3
    BAG_POCKET_KEY_ITEMS,   // 4
};

enum
{
    ITEMMENULOCATION_FIELD,
    ITEMMENULOCATION_BATTLE,
    ITEMMENULOCATION_PARTY,
    ITEMMENULOCATION_SHOP,
    ITEMMENULOCATION_BERRY_TREE,
    ITEMMENULOCATION_BERRY_BLENDER,
    ITEMMENULOCATION_ITEMPC,
    ITEMMENULOCATION_WALLY,
};

enum
{
    ITEM_ACTION_USE,
    ITEM_ACTION_TOSS,
    ITEM_ACTION_CANCEL,
    ITEM_ACTION_REGISTER,
    ITEM_ACTION_BATTLE_USE,
    ITEM_ACTION_BATTLE_CANCEL,
    ITEM_ACTION_GIVE,
    ITEM_ACTION_CHECK_TAG,
    ITEM_ACTION_DUMMY,
    ITEM_ACTION_CONFIRM,
};

#define ITEM_MENU_FIELD_CANCEL_CHOICE 3
#define ITEM_MENU_BERRY_CANCEL_CHOICE 5

#define TAG_BAG_GFX 30000
#define TAG_BAG_POCKET_SWITCH_BALL 8
#define TAG_BERRY_PIC 30020
#define TAG_BERRY_CHECK_CIRCLE 10000

// ewram
EWRAM_DATA static struct BagDescriptionPrinter sBagDescriptionPrinter = {0};
EWRAM_DATA static struct BagItemListPrinter sBagItemListPrinter = {0};
EWRAM_DATA static struct DeferredBagItemMessage sDeferredBagItemMessage = {0};
EWRAM_DATA static struct ItemMenuOptionsPrinter sItemMenuOptionsPrinter = {0};
EWRAM_DATA static struct BagItemListRedrawState sBagItemListRedrawState = {0};  // There are 3 bytes of padding after this, so I assume it's a struct
EWRAM_DATA static u8 sIsBagPocketSwitching = 0;
EWRAM_DATA static s8 sCurrentBagPocket = 0;
EWRAM_DATA static u8 sBagPocketSwitchStep = 0;
EWRAM_DATA static s8 sBagSpriteTargetPocket = 0;
EWRAM_DATA static s8 sBagSpriteShakeRequested = 0;
EWRAM_DATA u16 gSpecialVar_ItemId = 0;
EWRAM_DATA u8 gCurSelectedItemSlotIndex = 0;
EWRAM_DATA u8 gPokemonItemUseType = 0;
EWRAM_DATA static u8 sBagDescriptionLine = 0;
EWRAM_DATA static u8 sBagMenuTaskId = 0;
EWRAM_DATA static u8 sItemMenuOptionCount = 0;
EWRAM_DATA static u8 sSwitchPocketBallSpriteId ALIGNED(4) = 0;  // HACK: why is there a space before this variable?

// bss
static u8 sItemMenuSelection;
static u8 sBagMenuLocation;
static const u8 *sItemMenuActionList;

// common
void (*gFieldItemUseCallback)(u8) = NULL;
extern u16 gBattle_BG1_Y;
struct PocketScrollState gBagPocketScrollStates[5];
struct ItemSlot *gCurrentBagPocketItemSlots;  // selected pocket item slots
extern const u8 Event_NoRegisteredItem[];

static const struct CompressedSpriteSheet sMaleBagSpriteSheet;
static const struct CompressedSpriteSheet sFemaleBagSpriteSheet;
static const struct CompressedSpritePalette sBagSpritePalette;

const struct BagPocket gBagPockets[POCKETS_COUNT] =
{
    {gSaveBlock1.bagPocket_Items, 20},
    {gSaveBlock1.bagPocket_PokeBalls, 16},
    {gSaveBlock1.bagPocket_TMHM, 64},
    {gSaveBlock1.bagPocket_Berries, 46},
    {gSaveBlock1.bagPocket_KeyItems, 20},
};

static void HandlePopupMenuAction_UseOnField(u8);
static void HandlePopupMenuAction_Toss(u8);
static void ItemMenu_Cancel(u8);
static void HandlePopupMenuAction_Register(u8);
static void HandlePopupMenuAction_UseInBattle(u8);
static void ItemMenu_CancelBattle(u8);
static void HandlePopupMenuAction_Give(u8);
static void HandlePopupMenuAction_CheckTag(u8);
static void HandlePopupMenuAction_Confirm(u8);

static const struct MenuAction2 sItemMenuActions[] =
{
    {OtherText_Use,                   HandlePopupMenuAction_UseOnField},
    {OtherText_Toss,                  HandlePopupMenuAction_Toss},
    {gText_Cancel,   ItemMenu_Cancel},
    {OtherText_Register,              HandlePopupMenuAction_Register},
    {OtherText_Use,                   HandlePopupMenuAction_UseInBattle},
    {gText_Cancel,   ItemMenu_CancelBattle},
    {OtherText_Give2,                 HandlePopupMenuAction_Give},
    {OtherText_CheckTag,              HandlePopupMenuAction_CheckTag},
    {gOtherText_CancelWithTerminator, NULL},
    {OtherText_Confirm,               HandlePopupMenuAction_Confirm},
};

static const u8 sItemMenuChoicesTable[][6] =
{
    [BAG_POCKET_ITEMS] =
    {
        ITEM_ACTION_USE,
        ITEM_ACTION_TOSS,
        ITEM_ACTION_GIVE,
        ITEM_ACTION_CANCEL,
    },

    [BAG_POCKET_POKE_BALLS] =
    {
        ITEM_ACTION_GIVE,
        ITEM_ACTION_TOSS,
        ITEM_ACTION_DUMMY,
        ITEM_ACTION_CANCEL,
    },

    [BAG_POCKET_TMs_HMs] =
    {
        ITEM_ACTION_USE,
        ITEM_ACTION_DUMMY,
        ITEM_ACTION_GIVE,
        ITEM_ACTION_CANCEL,
    },

    [BAG_POCKET_BERRIES] =
    {
        ITEM_ACTION_CHECK_TAG,
        ITEM_ACTION_USE,
        ITEM_ACTION_TOSS,
        ITEM_ACTION_DUMMY,
        ITEM_ACTION_GIVE,
        ITEM_ACTION_CANCEL
    },

    [BAG_POCKET_KEY_ITEMS] =
    {
        ITEM_ACTION_USE,
        ITEM_ACTION_DUMMY,
        ITEM_ACTION_REGISTER,
        ITEM_ACTION_CANCEL,
    },
};

const u8 sLinkItemMenuChoicesTable[][2] =
{
    {ITEM_ACTION_GIVE,   ITEM_ACTION_CANCEL},
    {ITEM_ACTION_GIVE,   ITEM_ACTION_CANCEL},
    {ITEM_ACTION_GIVE,   ITEM_ACTION_CANCEL},
    {ITEM_ACTION_GIVE,   ITEM_ACTION_CANCEL},
    {ITEM_ACTION_CANCEL, ITEM_ACTION_USE},
};

const u8 sBerryBlenderItemMenuActions[] =
{
    ITEM_ACTION_CHECK_TAG,
    ITEM_ACTION_CONFIRM,
    ITEM_ACTION_DUMMY,
    ITEM_ACTION_CANCEL,
};

static void OnItemSelect_Normal(u8);
static void OnBagClose_Field(u8);
static void OnBagClose_Party(u8);
static void OnItemSelect_Party(u8);
static void OnBagClose_Shop(u8);
static void OnItemSelect_Shop(u8);
static void OnBagClose_BerryTree(u8);
static void OnItemSelect_BerryTree(u8);
static void OnBagClose_BerryBlender(u8);
static void OnBagClose_ItemPC(u8);
static void OnItemSelect_ItemPC(u8);
static void OnItemSelect_Battle(u8);
static void OnBagClose_Battle(u8);

struct BagMenuLocationCallbacks
{
    TaskFunc onItemSelect;
    TaskFunc onBagClose;
};

static const struct BagMenuLocationCallbacks sBagMenuLocationCallbacks[] =
{
    [ITEMMENULOCATION_FIELD]         = {OnItemSelect_Normal, OnBagClose_Field},
    [ITEMMENULOCATION_BATTLE]        = {OnItemSelect_Battle, OnBagClose_Battle},
    [ITEMMENULOCATION_PARTY]         = {OnItemSelect_Party, OnBagClose_Party},
    [ITEMMENULOCATION_SHOP]          = {OnItemSelect_Shop, OnBagClose_Shop},
    [ITEMMENULOCATION_BERRY_TREE]    = {OnItemSelect_BerryTree, OnBagClose_BerryTree},
    [ITEMMENULOCATION_BERRY_BLENDER] = {OnItemSelect_Normal, OnBagClose_BerryBlender},
    [ITEMMENULOCATION_ITEMPC]        = {OnItemSelect_ItemPC, OnBagClose_ItemPC},
};

static void TossItemYes(u8);
static void TossItemNo(u8);

static const struct YesNoFuncTable sTossItemYesNoFuncs = {TossItemYes, TossItemNo};

static void SellItemYes(u8);
static void SellItemNo(u8);

const struct YesNoFuncTable sSellItemYesNoFuncs = {SellItemYes, SellItemNo};

void BagMenu_InitBGs(void);
static bool8 LoadBagGraphicsMultistep(void);
static void ClampBagScrollPositions(void);
static void LoadBagMenuTextPalettes(void);
static void FillBagItemListRows(u16 *, u8, u8, u8);
static void CopyBagPocketNameTilemap(u16 *, u8);
static void CopyBagPocketNameTilemapWithScroll(u16 *, u8, u8, s8);
static void DrawPocketIndicatorDots(u16 *, u8);
static void SortItemSlots(struct BagPocket);
static void UpdateAllBagPockets(void);
static void SetBagPocketCursorMax(u8);
static void SetAllBagPocketCursorMax(void);
static void RemoveSelectIconFromRegisteredItem(void);
static void PrintBagItemListRange(u16, int, int);
static void ItemListMenu_InitDescription(s16);
static void ItemListMenu_ChangeDescription(s16, int);
static void Task_HandleBagPocketSwitchInput(u8);
static void Task_BagMenu_HandleInput(u8);
static void Task_StartBagMenuFadeOut(u8);
static void DisplayNoPokemonForItemUse(u8);
static void Task_CallItemUseOnFieldCallback(u8);
static void Task_CloseSellErrorMessage(u8);
static void Task_SetSellQuantity(u8);
static void Task_BuyHowManyDialogueHandleInput(u8);
static void BuyMenuPrintItemQuantityAndPrice(u8);
static void BuyMenuDisplayMessage(u16, u16);
static void DisplaySellMoneyWindow(void);
static void AddSaleMoney(u16, u8);
static void SetBerryBlenderItemMenuActions(void);
static void Task_WallyTutorialBagMenu(u8);
static void ItemListMenu_InitMenu(void);
static void SetBagListCursorPosition(void);
static void HideBagListCursor(void);
static void DestroyBagListCursor(void);
static void UpdateBagMenu(void);
static void UpdateBagItemDescription(void);
static void ResetBagDescriptionPrinter(void);
static void StartBagDescriptionPrinter(u8);
static void ClearBagItemDescription(void);
static void UpdateBagItemList(void);
static void ResetBagItemListPrinter(void);
static void StartBagItemListPrinter(void);
static void ClearBagItemList(void);
static void ResetDeferredBagItemMessage(void);
static void DisplayCannotUseItemMessage(int, const u8 *, TaskFunc, int);
static void UpdateItemMenuOptions(void);
static void ResetItemMenuOptionsPrinter(void);
static void StartItemMenuOptionsPrinter(int, int);
static bool32 IsItemMenuOptionsPrinterFinished(void);
static void ResetBagItemListRedrawFlag(void);
static bool32 ShouldDelayBagItemListPrinter(void);
static void ResumeItemMenuOptionsAfterList(void);
static int GetBagDescriptionPrinterState(void);
static void RunDeferredBagItemMessageAfterList(void);
static int PrepareBagItemMessage(void);
static int PrepareItemMenuOptionsPrinter(void);
static void ResumeBagDescriptionPrinterAfterOptions(void);
static void SetBagItemListRedrawFlag(void);
static int ConsumeBagItemListRedrawFlag(void);
static void SpriteCB_BagVisual(struct Sprite *);
static void SpriteCB_BagVisualSwitchingPockets(struct Sprite *);
static void SpriteCB_ShakeBagSprite(struct Sprite *);
static void ResetBagSpriteShake(struct Sprite *);
static void CreateBagSprite(void);
static void SpriteCB_SwitchPocketRotatingBallContinue(struct Sprite *);
static void CreateBagPokeballSprite(u8);
static void RemoveSwitchPocketRotatingBallSprite(void);

static void CB2_BagMenuRun(void)
{
    AnimateSprites();
    BuildOamBuffer();
    RunTasks();
    UpdateBagMenu();
    UpdatePaletteFade();
}

static void VBlankCB_BagMenuRun(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    DmaCopy16Defvars(3, gBGTilemapBuffers[1], (void *)(VRAM + 0x2000), 0x800);
    DmaCopy16Defvars(3, gBGTilemapBuffers[2], (void *)(VRAM + 0x6000), 0x800);
}

static bool8 SetupBagMultistep(void)
{
    u32 index;
    u16 savedIme;

    switch (gMain.state)
    {
    case 0:
        ClearVideoCallbacks();
        BagMenu_InitBGs();
        gMain.state++;
        break;
    case 1:
        ScanlineEffect_Stop();
        gMain.state++;
        break;
    case 2:
        FreeAndReserveObjectSpritePalettes();
        gMain.state++;
        break;
    case 3:
        ClearBGTilemapBuffers();
        ewramBagSetupStep = 0;
        gMain.state++;
        break;
    case 4:
        ResetPaletteFade();
        gPaletteFade.bufferTransferDisabled = TRUE;
        gMain.state++;
        break;
    case 5:
        ResetSpriteData();
        gMain.state++;
        break;
    case 6:
        if (LoadBagGraphicsMultistep() == FALSE)
            break;
        gMain.state++;
        break;
    case 7:
        Text_LoadWindowTemplate(&gMenuWindowTemplate);
        gMain.state++;
        break;
    case 8:
        MultistepInitMenuWindowBegin(&gMenuWindowTemplate);
        gMain.state++;
        break;
    case 9:
        if (!MultistepInitMenuWindowContinue())
            break;
        gMain.state++;
        break;
    case 10:
        ClearVerticalScrollIndicatorPalettes();
        LoadScrollIndicatorPalette();
        CreateVerticalScrollIndicators(TOP_ARROW, 172, 12);
        CreateVerticalScrollIndicators(BOTTOM_ARROW, 172, 148);
        CreateVerticalScrollIndicators(LEFT_ARROW, 28, 88);
        CreateVerticalScrollIndicators(RIGHT_ARROW, 100, 88);
        SetVerticalScrollIndicatorPriority(TOP_ARROW, 2);
        SetVerticalScrollIndicatorPriority(BOTTOM_ARROW, 2);
        SetVerticalScrollIndicatorPriority(LEFT_ARROW, 2);
        SetVerticalScrollIndicatorPriority(RIGHT_ARROW, 2);
        if (sBagMenuLocation == ITEMMENULOCATION_BERRY_TREE || sBagMenuLocation == ITEMMENULOCATION_BERRY_BLENDER)
        {
            SetVerticalScrollIndicators(LEFT_ARROW, INVISIBLE);
            SetVerticalScrollIndicators(RIGHT_ARROW, INVISIBLE);
        }
        gMain.state++;
        break;
    case 11:
        sBagPocketSwitchStep = 16;
        CopyBagPocketNameTilemap(gBGTilemapBuffers[2], sCurrentBagPocket + 1);
        DrawPocketIndicatorDots(gBGTilemapBuffers[2], sCurrentBagPocket);
        UpdateAllBagPockets();
        SortItemSlots(gBagPockets[2]);
        SortItemSlots(gBagPockets[3]);
        SetAllBagPocketCursorMax();
        gCurrentBagPocketItemSlots = gBagPockets[sCurrentBagPocket].itemSlots;
        ClampBagScrollPositions();
        gMain.state++;
        break;
    case 12:
        PrintBagItemListRange(0xFFFF, 0, 7);
        index = gBagPocketScrollStates[sCurrentBagPocket].scrollTop + gBagPocketScrollStates[sCurrentBagPocket].cursorPos;
        ItemListMenu_InitDescription(gCurrentBagPocketItemSlots[index].itemId);
        ItemListMenu_InitMenu();
        sBagSpriteTargetPocket = sCurrentBagPocket + 1;
        sBagSpriteShakeRequested = 0;
        gMain.state++;
        break;
    case 13:
        CreateBagSprite();
        CreateBagPokeballSprite(0);
        LoadBagMenuTextPalettes();
        gMain.state++;
        break;
    case 14:
        savedIme = REG_IME;
        REG_IME = 0;
        REG_IE |= INTR_FLAG_VBLANK;
        REG_IME = savedIme;
        REG_DISPSTAT |= DISPSTAT_VBLANK_INTR;
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        gPaletteFade.bufferTransferDisabled = FALSE;
        gMain.state++;
        break;
    case 15:
        if (Overworld_IsRecvQueueAtMax() == TRUE)
            break;
        gMain.state++;
        break;
    case 16:
        SetVBlankCallback(VBlankCB_BagMenuRun);
        SetMainCallback2(CB2_BagMenuRun);
        ResetBagDescriptionPrinter();
        ResetBagItemListPrinter();
        ResetDeferredBagItemMessage();
        ResetItemMenuOptionsPrinter();
        ResetBagItemListRedrawFlag();
#if DEBUG
        if (gLinkOpen == TRUE)
            debug_sub_8008218((void *)(VRAM + 0xF5E0), 0x8000, (void *)(VRAM + 0xF800), 0);
#endif
        return TRUE;
    }
    return FALSE;
}

static bool8 TrySetupBagMenu(void)
{
    while (1)
    {
        if (SetupBagMultistep() == TRUE)
        {
            sItemMenuSelection = 0;
            ResetTasks();
            return TRUE;
        }
        if (MenuHelpers_IsLinkActive() == TRUE)
            break;
    }
    return FALSE;
}


void BagMenu_InitBGs(void)
{
    ResetVramOamAndBgCntRegs();
    REG_BG2CNT = BGCNT_PRIORITY(2) | BGCNT_CHARBASE(1) | BGCNT_SCREENBASE(12) | BGCNT_16COLOR | BGCNT_TXT256x256;
    REG_BG1CNT = BGCNT_PRIORITY(1) | BGCNT_CHARBASE(1) | BGCNT_SCREENBASE(4)  | BGCNT_16COLOR | BGCNT_TXT256x256;
    REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG1_ON | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
    REG_BLDCNT = 0;
}

static bool8 LoadBagGraphicsMultistep(void)
{
    switch (ewramBagSetupStep)
    {
    case 0:
        LZDecompressVram(gBagScreen_Gfx, (void *)(VRAM + 0x4000));
        ewramBagSetupStep++;
        break;
    case 1:
        CpuCopy16(gBagScreen_Tilemap, gBGTilemapBuffers[2], 0x800);
        ewramBagSetupStep++;
        break;
    case 2:
        if (gSaveBlock2.playerGender == MALE || sBagMenuLocation == ITEMMENULOCATION_WALLY)
            LoadCompressedPalette(gBagScreenMale_Pal, 0, 64);
        else
            LoadCompressedPalette(gBagScreenFemale_Pal, 0, 64);
        ewramBagSetupStep++;
        break;
    case 3:
        if (gSaveBlock2.playerGender == MALE || sBagMenuLocation == ITEMMENULOCATION_WALLY)
            LoadCompressedSpriteSheet(&sMaleBagSpriteSheet);
        else
            LoadCompressedSpriteSheet(&sFemaleBagSpriteSheet);
        ewramBagSetupStep++;
        break;
    case 4:
        LoadCompressedSpritePalette(&sBagSpritePalette);
        ewramBagSetupStep = 0;
        return TRUE;
    }
    return FALSE;
}

static void ClampBagScrollPositions(void)
{
    u8 i;

    for (i = 0; i < 5; i++)
    {
        u8 r3;

        if (sBagMenuLocation == ITEMMENULOCATION_BERRY_BLENDER)
            r3 = gBagPocketScrollStates[i].numSlots - 1;
        else
            r3 = gBagPocketScrollStates[i].numSlots;

        if (gBagPocketScrollStates[i].scrollTop != 0)
        {
            if (gBagPocketScrollStates[i].scrollTop + gBagPocketScrollStates[i].cursorMax > r3)
                gBagPocketScrollStates[i].scrollTop = r3 - gBagPocketScrollStates[i].cursorMax;
        }
        else
        {
            if (gBagPocketScrollStates[i].cursorPos > r3)
                gBagPocketScrollStates[i].cursorPos = r3;
        }
    }
}

void ResetBagScrollPositions(void)
{
    u16 i;

    for (i = 0; i < POCKETS_COUNT; i++)
    {
        gBagPocketScrollStates[i].cursorPos = 0;
        gBagPocketScrollStates[i].scrollTop = 0;
        gBagPocketScrollStates[i].numSlots = 0;
        gBagPocketScrollStates[i].cursorMax = 0;
    }
    sCurrentBagPocket = 0;
}

static void ClearBgTilemapRect(u16 *tilemap, u8 left, u8 top, u8 width, u8 height)
{
    u16 i;
    u16 j;

    for (i = top; i <= top + height; i++)
    {
        for (j = left; j <= left + width; j++)
        {
            u32 index = j + i * 32;

            tilemap[index] = 0;
        }
    }
}

void ClearBag(void)
{
    u16 i;

    for (i = 0; i < POCKETS_COUNT; i++)
        ClearItemSlots(gBagPockets[i].itemSlots, gBagPockets[i].capacity);
    ResetBagScrollPositions();
}

#if DEBUG

const struct ItemSlot sDebugBagItems[60] =
{
    {ITEM_POKE_BALL,      10},
    {ITEM_ULTRA_BALL,     15},
    {ITEM_GREAT_BALL,     20},
    {ITEM_MASTER_BALL,    25},
    {ITEM_ACRO_BIKE,      1 },
    {ITEM_OLD_ROD,        1 },
    {ITEM_GOOD_ROD,       1 },
    {ITEM_SUPER_ROD,      1 },
    {ITEM_POKEBLOCK_CASE, 1 },
    {ITEM_WAILMER_PAIL,   1 },
    {ITEM_ITEMFINDER,     1 },
    {ITEM_NONE,           1 },
};

void debug_sub_80A3714(void)
{
    u16 arr[5];
    u16 i;

    for (i = 0; i < 5; i++)
        arr[i] = 0;
    for (i = 0; i < ARRAY_COUNT(sDebugBagItems); i++)
    {
        u16 pocket;

        if (sDebugBagItems[i].itemId == ITEM_NONE)
            break;
        pocket = GetItemPocket(sDebugBagItems[i].itemId) - 1;
        gBagPockets[pocket].itemSlots[arr[pocket]] = sDebugBagItems[i];
        arr[pocket]++;
    }
}

#endif

static void LoadBagMenuTextPalettes(void)
{
    const u16 colors[2] = {RGB(14, 15, 16), RGB_WHITE};

    LoadPalette(&colors[1], 0xD1, sizeof(colors[1]));
    LoadPalette(&colors[0], 0xD8, sizeof(colors[0]));
}

static void SetFieldItemMenuActions(void)
{
    if (sBagMenuLocation == ITEMMENULOCATION_FIELD)
    {
        sItemMenuActionList = sItemMenuChoicesTable[sCurrentBagPocket];
        if (sCurrentBagPocket != BAG_POCKET_BERRIES)
            sItemMenuOptionCount = 4;
        else
            sItemMenuOptionCount = 6;
    }
}

static void Task_EnableBagMenuInput(u8 taskId)
{
    gTasks[taskId].func = Task_BagMenu_HandleInput;
    StartVerticalScrollIndicators(TOP_ARROW);
    StartVerticalScrollIndicators(BOTTOM_ARROW);
    StartVerticalScrollIndicators(LEFT_ARROW);
    StartVerticalScrollIndicators(RIGHT_ARROW);
}

static void Task_AnimateBagPocketSwitch(u8 taskId)
{
    u8 r5 = sBagPocketSwitchStep;

    if (r5 < 16)
    {
        sBagPocketSwitchStep++;
        FillBagItemListRows(gBGTilemapBuffers[2], r5, sBagPocketSwitchStep, 0x4F);
        CopyBagPocketNameTilemapWithScroll(gBGTilemapBuffers[2], sCurrentBagPocket + 1, sBagPocketSwitchStep / 2, gTasks[taskId].data[5]);
        sIsBagPocketSwitching = 1;
        PrintBagItemListRange(taskId, r5 / 2, sBagPocketSwitchStep / 2 - 1);
        sIsBagPocketSwitching = 0;
        return;
    }

    if (gLinkOpen == TRUE)
    {
        u32 index = gBagPocketScrollStates[sCurrentBagPocket].scrollTop + gBagPocketScrollStates[sCurrentBagPocket].cursorPos;

        ItemListMenu_ChangeDescription(gCurrentBagPocketItemSlots[index].itemId, sBagDescriptionLine);
        sBagDescriptionLine++;
    }
    else
    {
        while (sBagDescriptionLine < 3)
        {
            u32 index = gBagPocketScrollStates[sCurrentBagPocket].scrollTop + gBagPocketScrollStates[sCurrentBagPocket].cursorPos;

            ItemListMenu_ChangeDescription(gCurrentBagPocketItemSlots[index].itemId, sBagDescriptionLine);
            sBagDescriptionLine++;
        }
    }
    if (sBagDescriptionLine == 3)
    {
        sBagPocketSwitchStep = 16;
        sBagSpriteTargetPocket = sCurrentBagPocket + 1;
        Task_EnableBagMenuInput(FindTaskIdByFunc(Task_HandleBagPocketSwitchInput));
        DestroyTask(taskId);
        ItemListMenu_InitMenu();
        SetVerticalScrollIndicators(LEFT_ARROW, VISIBLE);
        SetVerticalScrollIndicators(RIGHT_ARROW, VISIBLE);
    }
}

static void CopyBagScreenTilemap(u16 *a)
{
    CpuCopy16(gBagScreen_Tilemap, a, 0x800);
}

static void FillBagItemListRows(u16 *a, u8 b, u8 c, u8 d)
{
    u16 *dst = a + 14 + (b + 2) * 32;
    u16 i;

    while (c > b++)
    {
        for (i = 0; i < 15; i++)
            dst[i] = d;
        dst += 32;
    }
}

static void CopyBagPocketNameTilemap(u16 *a, u8 b)
{
    u8 var = b * 2;

    CopyTilemapRectFixedWidth((void *)a, 4, 10, gBagScreenLabels_Tilemap, 0, var, 8, 2);
}

static void CopyBagPocketNameTilemapWithScroll(u16 *a, u8 b, u8 c, s8 d)
{
    u16 r2 = b * 2;
    u16 r7;

    if (d == -1)
    {
        r7 = (b + 1) * 2;
        if (b == 5)
            r7 = 2;

        CopyTilemapRectFixedWidth((void *)a, 4, 10, gBagScreenLabels_Tilemap, 8 - c, r2, c, 2);
        CopyTilemapRectFixedWidth((void *)a, c + 4, 10, gBagScreenLabels_Tilemap, 0, r7, 8 - c, 2);
    }
    else if (d == 1)
    {
        r7 = (b - 1) * 2;
        if (b == 1)
            r7 = 10;

        CopyTilemapRectFixedWidth((void *)a, 4, 10, gBagScreenLabels_Tilemap, c, r7, 8 - c, 2);
        CopyTilemapRectFixedWidth((void *)a, 12 - c, 10, gBagScreenLabels_Tilemap, 0, r2, c, 2);
    }
}

// Draws dots under the bag sprite, showing which pocket is selected
static void DrawPocketIndicatorDots(u16 *tileMapBuffer, u8 selectedPocket)
{
    u8 i;

    for (i = 0; i < POCKETS_COUNT; i++)
    {
        if (i == selectedPocket)
            tileMapBuffer[0x125 + i] = 0x107D;
        else
            tileMapBuffer[0x125 + i] = 0x107C;
    }
}

static void ChangePocket(u16 *tileMapBuffer, s8 delta)
{
    u8 taskId;

    sCurrentBagPocket += delta;
    // Wrap around
    if (sCurrentBagPocket >= POCKETS_COUNT)
        sCurrentBagPocket = 0;
    if (sCurrentBagPocket < 0)
        sCurrentBagPocket = POCKETS_COUNT - 1;

    ClearBagItemList();
    ClearBagItemDescription();
    gCurrentBagPocketItemSlots = gBagPockets[sCurrentBagPocket].itemSlots;
    SetBagPocketCursorMax(sCurrentBagPocket);
    sBagPocketSwitchStep = 0;
    CopyBagScreenTilemap(tileMapBuffer);
    FillBagItemListRows(tileMapBuffer, 0, 16, 3);
    CopyBagPocketNameTilemap(tileMapBuffer, 0);
    sBagSpriteTargetPocket = 6;
    sBagDescriptionLine = 0;
    taskId = FindTaskIdByFunc(Task_AnimateBagPocketSwitch);
    if (taskId == 0xFF)
        taskId = CreateTask(Task_AnimateBagPocketSwitch, 8);
    gTasks[taskId].data[5] = delta;
}

static void SwapItemSlots(struct ItemSlot *a, struct ItemSlot *b)
{
    struct ItemSlot temp = *a;

    *a = *b;
    *b = temp;
}

static void RemoveEmptyItemSlots(struct BagPocket pocket)
{
    u16 i;
    u16 j;

    for (i = 0; i < pocket.capacity - 1; i++)
    {
        for (j = i + 1; j < pocket.capacity; j++)
        {
            if (pocket.itemSlots[i].quantity == 0)
                SwapItemSlots(&pocket.itemSlots[i], &pocket.itemSlots[j]);
        }
    }
}

static void SortItemSlots(struct BagPocket pocket)
{
    u16 i;
    u16 j;

    for (i = 0; i < pocket.capacity; i++)
    {
        for (j = i + 1; j < pocket.capacity; j++)
        {
            if (pocket.itemSlots[i].quantity != 0 && pocket.itemSlots[j].quantity != 0
             && pocket.itemSlots[i].itemId > pocket.itemSlots[j].itemId)
                SwapItemSlots(&pocket.itemSlots[i], &pocket.itemSlots[j]);
        }
    }
}

static void UpdateBagPocket(u8 pocketNum)
{
    u16 i;

    gBagPocketScrollStates[pocketNum].numSlots = 0;
    for (i = 0; i < gBagPockets[pocketNum].capacity; i++)
    {
        if (gBagPockets[pocketNum].itemSlots[i].quantity != 0)
            gBagPocketScrollStates[pocketNum].numSlots++;
    }
    RemoveEmptyItemSlots(gBagPockets[pocketNum]);
}

static void UpdateAllBagPockets(void)
{
    u8 i;

    for (i = 0; i < POCKETS_COUNT; i++)
        UpdateBagPocket(i);
}

static void SetBagPocketCursorMax(u8 pocketNum)
{
    if (gBagPocketScrollStates[pocketNum].numSlots >= 7)
        gBagPocketScrollStates[pocketNum].cursorMax = 7;
    else
        gBagPocketScrollStates[pocketNum].cursorMax = gBagPocketScrollStates[pocketNum].numSlots;
}

static void SetAllBagPocketCursorMax(void)
{
    u8 i;

    for (i = 0; i < POCKETS_COUNT; i++)
        SetBagPocketCursorMax(i);
}

static void RemoveCurrentBagItemQuantity(u8 taskId)
{
    u32 index = gBagPocketScrollStates[sCurrentBagPocket].scrollTop + gBagPocketScrollStates[sCurrentBagPocket].cursorPos;

    gCurrentBagPocketItemSlots[index].quantity -= gTasks[taskId].data[1];
    if (gCurrentBagPocketItemSlots[index].quantity == 0)  // item slot will be removed if the quantity is zero
    {
        // Un-register the item if registered
        if (gSaveBlock1.registeredItem == gCurrentBagPocketItemSlots[index].itemId)
        {
            RemoveSelectIconFromRegisteredItem();
            gSaveBlock1.registeredItem = ITEM_NONE;
        }

        gCurrentBagPocketItemSlots[index].itemId = ITEM_NONE;
        if (gBagPocketScrollStates[sCurrentBagPocket].scrollTop + 7 == gBagPocketScrollStates[sCurrentBagPocket].numSlots
         && gBagPocketScrollStates[sCurrentBagPocket].scrollTop != 0)
            gBagPocketScrollStates[sCurrentBagPocket].scrollTop--;
        UpdateBagPocket(sCurrentBagPocket);
    }
    SetBagPocketCursorMax(sCurrentBagPocket);
}

void UpdateCurrentBagItemSlot(void)
{
    u32 index = gBagPocketScrollStates[sCurrentBagPocket].scrollTop + gBagPocketScrollStates[sCurrentBagPocket].cursorPos;

    if (gCurrentBagPocketItemSlots[index].quantity == 0)
    {
        gCurrentBagPocketItemSlots[index].itemId = 0;
        if (gBagPocketScrollStates[sCurrentBagPocket].scrollTop + 7 == gBagPocketScrollStates[sCurrentBagPocket].numSlots
         && gBagPocketScrollStates[sCurrentBagPocket].scrollTop != 0)
            gBagPocketScrollStates[sCurrentBagPocket].scrollTop--;
        UpdateBagPocket(sCurrentBagPocket);
    }
    SetBagPocketCursorMax(sCurrentBagPocket);
}

static void BagItemSwapNoop(void)
{
}

static void SwapBagItemSlotsByIndex(u8 a, u8 b)
{
    struct ItemSlot temp = gCurrentBagPocketItemSlots[a];

    gCurrentBagPocketItemSlots[a] = gCurrentBagPocketItemSlots[b];
    gCurrentBagPocketItemSlots[b] = temp;
}

static void FinishBagItemSwap(u8 taskId)
{
    PlaySE(SE_SELECT);
    BagItemSwapNoop();
    SwapBagItemSlotsByIndex(
      gTasks[taskId].data[10] - 1,
      gBagPocketScrollStates[sCurrentBagPocket].cursorPos + gBagPocketScrollStates[sCurrentBagPocket].scrollTop);
    gTasks[taskId].data[10] = 0;
    StartBagItemListPrinter();
    StartBagDescriptionPrinter(0);
    ItemListMenu_InitMenu();
}

static void CancelBagItemSwap(u8 taskId)
{
    u8 r2;

    PlaySE(SE_SELECT);
    BagItemSwapNoop();
    r2 = gTasks[taskId].data[10] - gBagPocketScrollStates[sCurrentBagPocket].scrollTop - 1;
    gTasks[taskId].data[10] = 0;
    if (r2 < 8)
        PrintBagItemListRange(taskId, r2, r2);
    StartBagDescriptionPrinter(0);
}

static void StartBagItemSwap(u8 taskId)
{
    gTasks[taskId].data[10] = gBagPocketScrollStates[sCurrentBagPocket].scrollTop + gBagPocketScrollStates[sCurrentBagPocket].cursorPos + 1;
    PrintBagItemListRange(taskId, gBagPocketScrollStates[sCurrentBagPocket].cursorPos, gBagPocketScrollStates[sCurrentBagPocket].cursorPos);
    StartBagDescriptionPrinter(1);
}

void FillBgTilemapRect(u16 *tilemap, u8 left, u8 top, u8 width, u8 height, u16 tile)
{
    s16 i;
    s16 j;

    for (i = top; i < top + height; i++)
    {
        for (j = left; j < left + width; j++)
        {
            u32 index = j + i * 32;

            tilemap[index] = tile;
        }
    }
}

static void FillBgTilemapRectWithOne(u16 *tilemap, u8 left, u8 top, u8 width, u8 height)
{
    FillBgTilemapRect(tilemap, left, top, width, height, 1);
}

static void DrawSelectIcon(u32 itemPos)
{
    u16 *ptr1 = gBGTilemapBuffers[2] + 0x5A;

    ptr1 += itemPos * 64;
    ptr1[0] = 0x5A;
    ptr1[1] = 0x5B;
    ptr1[2] = 0x5C;
    ptr1[32] = 0x6A;
    ptr1[33] = 0x6B;
    ptr1[34] = 0x6C;
}

static void MoveSelectIcon(u8 itemPos)
{
    u16 i;
    u16 *ptr;

    ptr = gBGTilemapBuffers[2] + 0x5A;
    for (i = 0; i < 16; i++)
    {
        ptr[0] = 0x4F;
        ptr[1] = 0x4F;
        ptr[2] = 0x4F;
        ptr += 32;
    }

    ptr = gBGTilemapBuffers[2] + 0x5A + itemPos * 64;
    ptr[0] = 0x5A;
    ptr[1] = 0x5B;
    ptr[2] = 0x5C;
    ptr[32] = 0x6A;
    ptr[33] = 0x6B;
    ptr[34] = 0x6C;
}

static void EraseSelectIcon(u8 itemPos)
{
    u16 *ptr = gBGTilemapBuffers[2] + 0x5A;

    ptr += itemPos * 64;
    ptr[0] = 0x4F;
    ptr[1] = 0x4F;
    ptr[2] = 0x4F;
    ptr[32] = 0x4F;
    ptr[33] = 0x4F;
    ptr[34] = 0x4F;
}

static void RemoveSelectIconFromRegisteredItem(void)
{
    u8 i;

    for (i = 0; i < 8; i++)
    {
        if (gBagPocketScrollStates[sCurrentBagPocket].scrollTop + i == gBagPocketScrollStates[sCurrentBagPocket].numSlots)
            break;
        if (gCurrentBagPocketItemSlots[gBagPocketScrollStates[sCurrentBagPocket].scrollTop + i].itemId == gSaveBlock1.registeredItem)
        {
            EraseSelectIcon(i);
            break;
        }
    }
}

static void AddSelectIconToRegisteredItem(void)
{
    RemoveSelectIconFromRegisteredItem();
    MoveSelectIcon(gBagPocketScrollStates[sCurrentBagPocket].cursorPos);
}

void ConvertIntToItemQuantityString(u8 *dest, u16 value, enum StringConvertMode mode, u8 digits)
{
    *dest++ = CHAR_MULT_SIGN;
    dest[0] = EXT_CTRL_CODE_BEGIN;
    dest[1] = 0x14;
    dest[2] = 6;
    dest += 3;
    ConvertIntToDecimalStringN(dest, value, mode, digits);
}

void PrintItemQuantity(u16 value, enum StringConvertMode mode, u8 x, u8 y, u8 digits)
{
    ConvertIntToItemQuantityString(gStringVar1, value, mode, digits);
    Menu_PrintText(gStringVar1, x, y);
}

static void RefreshBagItemList(u8 taskId)
{
    StartBagItemListPrinter();
}

static void FormatBagListItemWithNumber(u8 *a, u16 b, const u8 *c, u16 d, u8 e)
{
    a[0] = EXT_CTRL_CODE_BEGIN;
    a[1] = 0x13;
    a[2] = 8;
    a += 3;
    a = ConvertIntToDecimalStringN(a, b, STR_CONV_MODE_LEADING_ZEROS, 2);
    a[0] = EXT_CTRL_CODE_BEGIN;
    a[1] = 0x13;
    a[2] = 0x18;
    a += 3;
    a = AlignStringInMenuWindow(a, c, 0x78 - (e + 1) * 6, 0);
    *a++ = CHAR_MULT_SIGN;
    AlignInt1InMenuWindow(a, d, 0x78, 1);
}

static u8 *SetItemSwapTextColor(u8 taskId, u8 *text, u8 itemSlot)
{
    if (gTasks[taskId].data[10] - gBagPocketScrollStates[sCurrentBagPocket].scrollTop - 1 == itemSlot)
    {
        text[0] = EXT_CTRL_CODE_BEGIN;
        text[1] = 1;
        text[2] = TEXT_COLOR_RED;
        text += 3;
    }
    return text;
}

static bool8 PrintCloseBagListEntry(u8 itemPos, int b)
{
    u8 r5;
    u16 *ptr;
    struct BagItemListPrinter *printer = &sBagItemListPrinter;

    if (gBagPocketScrollStates[sCurrentBagPocket].scrollTop + itemPos > gBagPocketScrollStates[sCurrentBagPocket].numSlots)
        return TRUE;
    if (gBagPocketScrollStates[sCurrentBagPocket].scrollTop + itemPos == gBagPocketScrollStates[sCurrentBagPocket].numSlots)
    {
        if (sBagMenuLocation == ITEMMENULOCATION_BERRY_BLENDER)
            return TRUE;
        r5 = itemPos * 2 + 2;
        AlignStringInMenuWindow(gStringVar1, gOtherText_CloseBag, 0x78, 0);
        Menu_PrintText(gStringVar1, 14, r5);
        ptr = gBGTilemapBuffers[2] + 14 + r5 * 32;
        ptr[0] = 0x4F;
        ptr[1] = 0x4F;
        ptr[32] = 0x4F;
        ptr[33] = 0x4F;
        if (itemPos == 7)
            return TRUE;
        if ((b == 1 && printer->isShortList != 0) || b == 2)
            Menu_BlankWindowRect(14, r5 + 2, 29, 13);
        else
            Menu_BlankWindowRect(14, r5 + 2, 29, 17);
        return TRUE;
    }
    return FALSE;
}

static void PrintStandardBagItems(u16 a, int b, int c, int d)
{
    u8 i;

    for (i = b; i <= c; i++)
    {
        u8 r4;
        u8 r5;
        u8 *text;

        if (PrintCloseBagListEntry(i, d) == TRUE)
            break;
        r4 = gBagPocketScrollStates[sCurrentBagPocket].scrollTop + i;
        r5 = i * 2 + 2;
        text = gStringVar1;
        text = SetItemSwapTextColor(a, text, i);
        text = AlignStringInMenuWindow(text, GetItemName(gCurrentBagPocketItemSlots[r4].itemId), 0x66, 0);
        *text++ = CHAR_MULT_SIGN;
        AlignInt1InMenuWindow(text, gCurrentBagPocketItemSlots[r4].quantity, 0x78, 1);
        Menu_PrintText(gStringVar1, 14, r5);
    }
}

static void PrintKeyItems(u16 a, int b, int c, int d)
{
    u8 i;

    for (i = b; i <= c; i++)
    {
        u8 slot;
        u8 r5;
        u8 *text;

        if (PrintCloseBagListEntry(i, d) == TRUE)
            break;
        slot = gBagPocketScrollStates[sCurrentBagPocket].scrollTop + i;
        r5 = i * 2 + 2;
        text = gStringVar1;
        text = SetItemSwapTextColor(a, text, i);
#if ENGLISH
        AlignStringInMenuWindow(text, GetItemName(gCurrentBagPocketItemSlots[slot].itemId), 0x60, 0);
#else
        AlignStringInMenuWindow(text, GetItemName(gCurrentBagPocketItemSlots[slot].itemId), 0x63, 0);
#endif
        Menu_PrintText(gStringVar1, 14, r5);
        if (sIsBagPocketSwitching)
        {
            if (gCurrentBagPocketItemSlots[slot].itemId == gSaveBlock1.registeredItem)
                DrawSelectIcon(i);
        }
        else
        {
            if (gCurrentBagPocketItemSlots[slot].itemId == gSaveBlock1.registeredItem)
                MoveSelectIcon(i);
            else
                EraseSelectIcon(i);
        }
    }
}

static void PrintTMHMItems(u16 taskId, int topItemOffset, int bottomItemOffset, int d)
{
    u8 i;

    for (i = topItemOffset; i <= bottomItemOffset; i++)
    {
        u8 slot;
        u8 y;
        u16 tilemapOffset;
        u8 *text;

        if (PrintCloseBagListEntry(i, d) == TRUE)
            break;

        slot = gBagPocketScrollStates[sCurrentBagPocket].scrollTop + i;
        y = i * 2 + 2;
        tilemapOffset = y * 32 + 14;
        text = gStringVar1;
        text = SetItemSwapTextColor(taskId, text, i);

        if (gCurrentBagPocketItemSlots[slot].itemId < ITEM_HM01_CUT)
        {
            const u8 *moveName;
            gBGTilemapBuffers[2][tilemapOffset + 0] = 0x59;
            gBGTilemapBuffers[2][tilemapOffset + 1] = 0x4F;
            gBGTilemapBuffers[2][tilemapOffset + 32] = 0x69;
            gBGTilemapBuffers[2][tilemapOffset + 33] = 0x4F;
            moveName = gMoveNames[ItemIdToBattleMoveId(gCurrentBagPocketItemSlots[slot].itemId)];
            FormatBagListItemWithNumber(text, gCurrentBagPocketItemSlots[slot].itemId - (ITEM_TM01_FOCUS_PUNCH - 1), moveName, gCurrentBagPocketItemSlots[slot].quantity, 2);
        }
        else
        {
            const u8 *moveName;
            gBGTilemapBuffers[2][tilemapOffset + 0] = 0x105D;
            gBGTilemapBuffers[2][tilemapOffset + 1] = 0x105E;
            gBGTilemapBuffers[2][tilemapOffset + 32] = 0x106D;
            gBGTilemapBuffers[2][tilemapOffset + 33] = 0x106E;
            text[0] = EXT_CTRL_CODE_BEGIN;
            text[1] = 0x13;
            text[2] = 0x11;
            text += 3;
            text = ConvertIntToDecimalStringSimple(text, gCurrentBagPocketItemSlots[slot].itemId - (ITEM_HM01_CUT - 1));
            text[0] = EXT_CTRL_CODE_BEGIN;
            text[1] = 0x13;
            text[2] = 0x18;
            text += 3;
            moveName = gMoveNames[ItemIdToBattleMoveId(gCurrentBagPocketItemSlots[slot].itemId)];
            AlignStringInMenuWindow(text, moveName, 0x78, 0);
        }

        Menu_PrintText(gStringVar1, 14, y);
    }
}

static void PrintBerryItems(u16 taskId, int b, int c, int d)
{
    u8 i;

    for (i = b; i <= c; i++)
    {
        u8 r4;
        u8 r5;
        u8 *text;
        register int var asm("r0");

        if (PrintCloseBagListEntry(i, d) == TRUE)
            break;
        r4 = gBagPocketScrollStates[sCurrentBagPocket].scrollTop + i;
        r5 = i * 2 + 2;

        var = 14 + r5 * 32;
        gBGTilemapBuffers[2][var] = 0x59;
        var += 32;
        gBGTilemapBuffers[2][var] = 0x69;

        text = gStringVar1;
        text = SetItemSwapTextColor(taskId, text, i);
        CopyItemName(gCurrentBagPocketItemSlots[r4].itemId, gStringVar2);
        FormatBagListItemWithNumber(text, gCurrentBagPocketItemSlots[r4].itemId - 0x84, gStringVar2, gCurrentBagPocketItemSlots[r4].quantity, 3);
        Menu_PrintText(gStringVar1, 14, r5);
    }
}

static void PrintBagItemList(u16 taskId, int topItemOffset, int bottomItemOffset, int d)
{
    switch (sCurrentBagPocket)
    {
    case BAG_POCKET_ITEMS:
    case BAG_POCKET_POKE_BALLS:
        PrintStandardBagItems(taskId, topItemOffset, bottomItemOffset, d);
        break;
    case BAG_POCKET_KEY_ITEMS:
        PrintKeyItems(taskId, topItemOffset, bottomItemOffset, d);
        break;
    case BAG_POCKET_TMs_HMs:
        PrintTMHMItems(taskId, topItemOffset, bottomItemOffset, d);
        break;
    case BAG_POCKET_BERRIES:
        PrintBerryItems(taskId, topItemOffset, bottomItemOffset, d);
        break;
    }
    if (gBagPocketScrollStates[sCurrentBagPocket].scrollTop != 0)
        SetVerticalScrollIndicators(TOP_ARROW, VISIBLE);
    else
        SetVerticalScrollIndicators(TOP_ARROW, INVISIBLE);
    if ((sBagMenuLocation != ITEMMENULOCATION_BERRY_BLENDER && gBagPocketScrollStates[sCurrentBagPocket].scrollTop + 8 < gBagPocketScrollStates[sCurrentBagPocket].numSlots + 1)
     || (sBagMenuLocation == ITEMMENULOCATION_BERRY_BLENDER && gBagPocketScrollStates[sCurrentBagPocket].scrollTop + 8 < gBagPocketScrollStates[sCurrentBagPocket].numSlots))
        SetVerticalScrollIndicators(BOTTOM_ARROW, VISIBLE);
    else
        SetVerticalScrollIndicators(BOTTOM_ARROW, INVISIBLE);
}

static void PrintBagItemListRange(u16 taskId, int topItemOffset, int bottomItemOffset)
{
    PrintBagItemList(taskId, topItemOffset, bottomItemOffset, 0);
}

static void PrintBagItemListForMessage(u16 taskId)
{
    PrintBagItemList(taskId, 0, 5, 2);
}

static void ItemListMenu_InitDescription(s16 itemId)
{
    u8 r5;

    if (gBagPocketScrollStates[sCurrentBagPocket].scrollTop + gBagPocketScrollStates[sCurrentBagPocket].cursorPos == gBagPocketScrollStates[sCurrentBagPocket].numSlots)
    {
        r5 = Menu_PrintTextWithNewlineIndent(gOtherText_ReturnTo, 4, 0x68, 0x68, 1);
        r5 += Menu_PrintTextWithNewlineIndent(gBagMenuReturnToTexts[sBagMenuLocation], 4, 0x78, 0x68, 1);
    }
    else
    {
        r5 = Menu_PrintTextWithNewlineIndent(GetItemDescription(itemId), 4, 0x68, 0x68, 1);
    }

    if (r5 < 3)
        Menu_EraseWindowRect(0, 13 + r5 * 2, 13, 20);
}

static void ItemListMenu_ChangeDescription(s16 itemId, int b)
{
    u8 description[100];

    if (gBagPocketScrollStates[sCurrentBagPocket].scrollTop + gBagPocketScrollStates[sCurrentBagPocket].cursorPos == gBagPocketScrollStates[sCurrentBagPocket].numSlots)
    {
        if (b == 0)
        {
            Menu_EraseWindowRect(0, 13, 13, 20);
            Menu_PrintTextPixelCoords(gOtherText_ReturnTo, 4, 0x68, 0);
        }
        else if (b == 1)
        {
            Menu_PrintTextPixelCoords(gBagMenuReturnToTexts[sBagMenuLocation], 4, 0x78, 0);
        }
    }
    else
    {
        if (b == 0)
            Menu_EraseWindowRect(0, 13, 13, 20);
        if (CopyItemDescriptionLine(description, itemId, b))
            Menu_PrintTextPixelCoords(description, 4, 104 + b * 16, 0);
    }
}

static bool32 CopyTextLine(u8 *destination, const u8 *source, u32 c)
{
    u32 r4 = c + 1;
    const u8 *src = source;
    u8 *dst = destination;

    while (1)
    {
        if (*src == CHAR_NEWLINE || *src == EOS)
        {
            r4--;
            if (r4 == 0)
            {
                *dst = EOS;
                return TRUE;
            }
            if (*src == EOS)
                return FALSE;
            // got a new line - reset dst pointer
            dst = destination;
            src++;
        }
        else
        {
            *dst++ = *src++;
        }
    }
}

static void PrintBagDescriptionLine(const u8 *text, u32 line)
{
    u8 buffer[100];

    if (line == 0)
        Menu_EraseWindowRect(0, 13, 13, 20);
    if (CopyTextLine(buffer, text, line))
        Menu_PrintTextPixelCoords(buffer, 4, 104 + line * 16, 0);
}

static void StartBagSpriteShake(u8 taskId)
{
    if (gTasks[taskId].data[10] == 0)
        StartBagDescriptionPrinter(0);
    PlaySE(SE_SELECT);
    sBagSpriteShakeRequested = 1;
}

static void ScrollBagItemList(s8 a, u8 b)
{
    gBagPocketScrollStates[sCurrentBagPocket].scrollTop += a;
    Menu_MoveCursor(0);
    SetBagListCursorPosition();
    StartBagItemListPrinter();
    StartBagSpriteShake(b);
}

static void MoveBagItemCursor(s8 delta, u8 b)
{
    gBagPocketScrollStates[sCurrentBagPocket].cursorPos = Menu_MoveCursor(delta);
    SetBagListCursorPosition();
    StartBagSpriteShake(b);
}

static const u8 *GetFirstItemMenuActionText(u16 itemId)
{
    if (TestPlayerAvatarFlags(6))
    {
        if (itemId == ITEM_MACH_BIKE || itemId == ITEM_ACRO_BIKE)
            return gOtherText_Walk;
    }
    if (ItemIsMail(itemId) == TRUE)
        return gOtherText_Check;
    return sItemMenuActions[sItemMenuActionList[0]].text;
}

static void DrawFieldItemMenu(u16 *a)
{
    u8 i;

    if (sItemMenuOptionCount == 4)
    {
        Menu_DrawStdWindowFrame(0, 7, 13, 12);
        FillBgTilemapRectWithOne(a, 1, 8, 12, 4);
        if (MenuHelpers_IsLinkActive() == TRUE && sBagMenuLocation == ITEMMENULOCATION_BERRY_BLENDER)
        {
            StartItemMenuOptionsPrinter(1, 0);
        }
        else
        {
            for (i = 0; i < sItemMenuOptionCount; i++)
            {
                const u8 *text;

                if (i == 0)
                    text = GetFirstItemMenuActionText(gSpecialVar_ItemId);
                else
                    text = sItemMenuActions[sItemMenuActionList[i]].text;
                Menu_PrintText(text, 1 + (i / 2) * 6, 8 + (i % 2) * 2);
            }
        }
        if (sBagMenuLocation == ITEMMENULOCATION_BERRY_BLENDER)
            InitMenu(0, 1, 8, sItemMenuOptionCount, sItemMenuSelection, 1);
        else
            InitMenu(0, 1, 8, sItemMenuOptionCount, 0, 1);
        sub_8072DCC(0x2F);
    }
    else
    {
        Menu_DrawStdWindowFrame(0, 5, 13, 12);
        FillBgTilemapRectWithOne(a, 1, 6, 12, 6);
        for (i = 0; i < sItemMenuOptionCount; i++)
            Menu_PrintText(sItemMenuActions[sItemMenuActionList[i]].text, 1 + (i / 3) * 6, 6 + (i % 3) * 2);
        InitMenu(0, 1, 6, sItemMenuOptionCount, 0, 1);
        sub_8072DCC(0x2F);
    }
    StartBagDescriptionPrinter(2);
}

static void CloseItemMenu(u16 *a)
{
    DestroyBagListCursor();
    ClearBgTilemapRect(a, 0, 6, 13, 6);
    Menu_EraseWindowRect(0, 5, 13, 12);
    ClearBagItemDescription();
}

static void DrawItemQuantityWindow(u8 taskId, u8 b, u8 c, u8 d, u8 e, u8 digits)
{
    gTasks[taskId].data[1] = 1;
    gTasks[taskId].data[2] = b + 2;
    gTasks[taskId].data[3] = c + 1;
    Menu_DrawStdWindowFrame(b, c, b + d, c + e);
    FillBgTilemapRectWithOne(gBGTilemapBuffers[1], b + 1, c + 1, d - 1, e - 1);
    PrintItemQuantity(1, 1, b + 2, c + 1, digits);
}

static void SwitchBagPocket(s8 delta, u8 b)
{
    PlaySE(SE_SELECT);
    SetVerticalScrollIndicators(TOP_ARROW, INVISIBLE);
    SetVerticalScrollIndicators(BOTTOM_ARROW, INVISIBLE);
    SetVerticalScrollIndicators(LEFT_ARROW, INVISIBLE);
    SetVerticalScrollIndicators(RIGHT_ARROW, INVISIBLE);
    PauseVerticalScrollIndicator(TOP_ARROW);
    PauseVerticalScrollIndicator(BOTTOM_ARROW);
    PauseVerticalScrollIndicator(LEFT_ARROW);
    PauseVerticalScrollIndicator(RIGHT_ARROW);
    ChangePocket(gBGTilemapBuffers[2], delta);
    DrawPocketIndicatorDots(gBGTilemapBuffers[2], sCurrentBagPocket);
    SetFieldItemMenuActions();
    RemoveSwitchPocketRotatingBallSprite();
    CreateBagPokeballSprite(b);
}

static bool8 HandleBagPocketSwitchInput(void)
{
    if (sBagMenuLocation == ITEMMENULOCATION_BERRY_BLENDER || sBagMenuLocation == ITEMMENULOCATION_BERRY_TREE)
        return FALSE;
    if (gMain.newKeys == DPAD_RIGHT || GetLRKeysPressed() == 2)
    {
        SwitchBagPocket(1, 2);
        return TRUE;
    }
    if (gMain.newKeys == DPAD_LEFT || GetLRKeysPressed() == 1)
    {
        SwitchBagPocket(-1, 1);
        return TRUE;
    }
    return FALSE;
}

static void Task_HandleBagPocketSwitchInput(u8 taskId)
{
    HandleBagPocketSwitchInput();
}

static bool8 HandleBagListInput(u8 a)
{
    bool8 retVal = FALSE;

    if (JOY_REPT(DPAD_ANY) == DPAD_UP)
    {
        if (gBagPocketScrollStates[sCurrentBagPocket].cursorPos != 0)
            MoveBagItemCursor(-1, a);
        else if (gBagPocketScrollStates[sCurrentBagPocket].scrollTop != 0)
            ScrollBagItemList(-1, a);
        if (gLinkOpen == TRUE && gMain.keyRepeatCounter == 5)
            gMain.keyRepeatCounter = 11;
        retVal = TRUE;
    }
    else if (JOY_REPT(DPAD_ANY) == DPAD_DOWN)
    {
        if (sBagMenuLocation != ITEMMENULOCATION_BERRY_BLENDER)
        {
            if (gBagPocketScrollStates[sCurrentBagPocket].cursorPos == gBagPocketScrollStates[sCurrentBagPocket].cursorMax)
            {
                if (gBagPocketScrollStates[sCurrentBagPocket].scrollTop + gBagPocketScrollStates[sCurrentBagPocket].cursorPos != gBagPocketScrollStates[sCurrentBagPocket].numSlots)
                    ScrollBagItemList(1, a);
            }
            else
            {
                MoveBagItemCursor(1, a);
            }
        }
        else
        {
            if (gBagPocketScrollStates[sCurrentBagPocket].scrollTop + gBagPocketScrollStates[sCurrentBagPocket].cursorPos != gBagPocketScrollStates[sCurrentBagPocket].numSlots - 1)
            {
                if (gBagPocketScrollStates[sCurrentBagPocket].cursorPos != gBagPocketScrollStates[sCurrentBagPocket].cursorMax)
                    MoveBagItemCursor(1, a);
                else
                    ScrollBagItemList(1, a);
            }
        }
        retVal = TRUE;
        if (gLinkOpen == TRUE && gMain.keyRepeatCounter == 5)
            gMain.keyRepeatCounter = 11;
    }
    else
    {
        if (gTasks[a].data[10] == 0 && HandleBagPocketSwitchInput() == TRUE)
        {
            HideBagListCursor();
            gTasks[a].func = Task_HandleBagPocketSwitchInput;
            retVal = TRUE;
        }
        else
        {
            SetBagListCursorPosition();
        }
    }
    return retVal;
}

static void Task_BagMenu_HandleInput(u8 taskId)
{
    s16 *taskData = gTasks[taskId].data;

    if (!gPaletteFade.active)
    {
        if (HandleBagListInput(taskId) == TRUE)
        {
            SetTaskToWaitForLinkRecv(taskId);
            return;
        }

        if (JOY_NEW(SELECT_BUTTON)
         && !(sCurrentBagPocket == BAG_POCKET_BERRIES || sCurrentBagPocket == BAG_POCKET_TMs_HMs)
         && (sBagMenuLocation == ITEMMENULOCATION_FIELD || sBagMenuLocation == ITEMMENULOCATION_BATTLE))
        {
            if (taskData[10] == 0)
            {
                if (gBagPocketScrollStates[sCurrentBagPocket].scrollTop + gBagPocketScrollStates[sCurrentBagPocket].cursorPos != gBagPocketScrollStates[sCurrentBagPocket].numSlots)
                {
                    PlaySE(SE_SELECT);
                    StartBagItemSwap(taskId);
                }
                SetTaskToWaitForLinkRecv(taskId);
            }
            else
            {
                if (gBagPocketScrollStates[sCurrentBagPocket].scrollTop + gBagPocketScrollStates[sCurrentBagPocket].cursorPos != gBagPocketScrollStates[sCurrentBagPocket].numSlots)
                {
                    PlaySE(SE_SELECT);
                    FinishBagItemSwap(taskId);
                }
                else
                {
                    CancelBagItemSwap(taskId);
                }
                SetTaskToWaitForLinkRecv(taskId);
            }
            return;
        }

        if (JOY_NEW(A_BUTTON))
        {
            if (gBagPocketScrollStates[sCurrentBagPocket].scrollTop + gBagPocketScrollStates[sCurrentBagPocket].cursorPos == gBagPocketScrollStates[sCurrentBagPocket].numSlots)
            {
                if (taskData[10] == 0)
                {
                    gSpecialVar_ItemId = 0;
                    sBagMenuLocationCallbacks[sBagMenuLocation].onBagClose(taskId);
                }
                else
                {
                    CancelBagItemSwap(taskId);
                }
                SetTaskToWaitForLinkRecv(taskId);
            }
            else
            {
                if (taskData[10] == 0)
                {
                    PlaySE(SE_SELECT);
                    gCurSelectedItemSlotIndex = gBagPocketScrollStates[sCurrentBagPocket].scrollTop + gBagPocketScrollStates[sCurrentBagPocket].cursorPos;
                    gSpecialVar_ItemId = gCurrentBagPocketItemSlots[gCurSelectedItemSlotIndex].itemId;
                    sBagMenuLocationCallbacks[sBagMenuLocation].onItemSelect(taskId);
                    StopVerticalScrollIndicators(TOP_ARROW);
                    StopVerticalScrollIndicators(BOTTOM_ARROW);
                    StopVerticalScrollIndicators(LEFT_ARROW);
                    StopVerticalScrollIndicators(RIGHT_ARROW);
                    SetBagItemListRedrawFlag();
                }
                else
                {
                    FinishBagItemSwap(taskId);
                }
                SetTaskToWaitForLinkRecv(taskId);
            }
            return;
        }

        if (JOY_NEW(B_BUTTON))
        {
            if (taskData[10] == 0)
            {
                if (sBagMenuLocation != ITEMMENULOCATION_BERRY_BLENDER)
                {
                    gSpecialVar_ItemId = 0;
                    sBagMenuLocationCallbacks[sBagMenuLocation].onBagClose(taskId);
                }
            }
            else
            {
                CancelBagItemSwap(taskId);
            }
            SetTaskToWaitForLinkRecv(taskId);
            return;
        }
    }
}

bool8 SellMenu_QuantityRoller(u8 taskId, u16 b)
{
    s16 *taskData = gTasks[taskId].data;

    if (JOY_REPT(DPAD_ANY) == DPAD_UP)
    {
        if (taskData[1] != b)
            taskData[1]++;
        else
            taskData[1] = 1;
        return TRUE;
    }

    if (JOY_REPT(DPAD_ANY) == DPAD_DOWN)
    {
        if (taskData[1] != 1)
            taskData[1]--;
        else
            taskData[1] = b;
        return TRUE;
    }

    if (JOY_REPT(DPAD_ANY) == DPAD_RIGHT)
    {
        if (taskData[1] + 10 < b)
            taskData[1] += 10;
        else
            taskData[1] = b;
        return TRUE;
    }

    if (JOY_REPT(DPAD_ANY) == DPAD_LEFT)
    {
        if (taskData[1] > 10)
            taskData[1] -= 10;
        else
            taskData[1] = 1;
        return TRUE;
    }

    return FALSE;
}

static bool8 UpdateItemQuantity(u8 taskId)
{
    s16 *taskData = gTasks[taskId].data;

    if (SellMenu_QuantityRoller(taskId, gCurrentBagPocketItemSlots[gCurSelectedItemSlotIndex].quantity) == TRUE)
    {
        // if (sCurrentBagPocket == BAG_POCKET_BERRIES)  Can't get it to match this way
        if (sCurrentBagPocket + 1 == BAG_POCKET_BERRIES + 1)
            PrintItemQuantity(taskData[1], 1, taskData[2], taskData[3], 3);
        else
            PrintItemQuantity(taskData[1], 1, taskData[2], taskData[3], 2);
        return TRUE;
    }
    return FALSE;
}

static void CB2_InitBagMenuFromStartMenu(void)
{
    if (TrySetupBagMenu() == TRUE)
    {
        SetFieldItemMenuActions();
        sBagMenuTaskId = CreateTask(Task_BagMenu_HandleInput, 0);
    }
}

void CB2_BagMenuFromStartMenu(void)
{
    sBagMenuLocation = ITEMMENULOCATION_FIELD;
    SetMainCallback2(CB2_InitBagMenuFromStartMenu);
}

static void Task_HandleFieldItemMenu(u8 taskId)
{
    TaskFunc r5 = NULL;
    if (IsItemMenuOptionsPrinterFinished())
    {
        while (1)
        {
            if (JOY_REPT(DPAD_ANY) == DPAD_UP)
            {
                if ((sItemMenuSelection & 1) && sItemMenuActionList[sItemMenuSelection - 1] != ITEM_ACTION_DUMMY)
                {
                    PlaySE(SE_SELECT);
                    sItemMenuSelection = MoveMenuCursor3(-1);
                }
                break;
            }
            if (JOY_REPT(DPAD_ANY) == DPAD_DOWN)
            {
                if ((!(sItemMenuSelection & 1)) && sItemMenuActionList[sItemMenuSelection + 1] != ITEM_ACTION_DUMMY)
                {
                    PlaySE(SE_SELECT);
                    sItemMenuSelection = MoveMenuCursor3(1);
                }
                break;
            }
            if (JOY_REPT(DPAD_ANY) == DPAD_LEFT)
            {
                if ((sItemMenuSelection >= 2) && sItemMenuActionList[sItemMenuSelection - 2] != ITEM_ACTION_DUMMY)
                {
                    PlaySE(SE_SELECT);
                    sItemMenuSelection = MoveMenuCursor3(-2);
                }
                break;
            }
            if (JOY_REPT(DPAD_ANY) == DPAD_RIGHT)
            {
                if ((sItemMenuSelection < 2) && sItemMenuActionList[sItemMenuSelection + 2] != ITEM_ACTION_DUMMY)
                {
                    PlaySE(SE_SELECT);
                    sItemMenuSelection = MoveMenuCursor3(2);
                }
                break;
            }
            if (JOY_NEW(A_BUTTON))
            {
                gTasks[taskId].data[10] = 0;
                CloseItemMenu(gBGTilemapBuffers[1]);
                r5 = sItemMenuActions[sItemMenuActionList[sItemMenuSelection]].func;
                r5(taskId);
                break;
            }
            if (JOY_NEW(B_BUTTON))
            {
                gTasks[taskId].data[10] = 0;
                PrintBagItemListRange(taskId, gBagPocketScrollStates[sCurrentBagPocket].cursorPos, gBagPocketScrollStates[sCurrentBagPocket].cursorPos);
                CloseItemMenu(gBGTilemapBuffers[1]);
            r5 = sItemMenuActions[sItemMenuActionList[ITEM_MENU_FIELD_CANCEL_CHOICE]].func;
                r5(taskId);
                break;
            }
            break;
        }
    }
    if (r5 == NULL)
    {
        if (sBagMenuLocation == ITEMMENULOCATION_BERRY_BLENDER)
        {
            if (sItemMenuSelection == 0)
                sub_8072DDC(12);
            else if (sItemMenuSelection < 2)
                sub_8072DCC(47);
            else
                sub_8072DCC(48);
        }
        else
        {
            if (sItemMenuSelection < 2)
                sub_8072DCC(47);
            else
                sub_8072DCC(48);
        }
    }
}

static void Task_HandleBerryItemMenu(u8 taskId)
{
    TaskFunc r5 = NULL;

    while (1)
    {
        if (JOY_REPT(DPAD_ANY) == DPAD_UP)
        {
            if (sItemMenuSelection != 0 && sItemMenuActionList[sItemMenuSelection - 1] != ITEM_ACTION_DUMMY)
            {
                PlaySE(SE_SELECT);
                sItemMenuSelection = MoveMenuCursor3(-1);
            }
            break;
        }
        if (JOY_REPT(DPAD_ANY) == DPAD_DOWN)
        {
            if (sItemMenuSelection != sItemMenuOptionCount - 1 && sItemMenuSelection != 2 && sItemMenuActionList[sItemMenuSelection + 1] != ITEM_ACTION_DUMMY)
            {
                PlaySE(SE_SELECT);
                sItemMenuSelection = MoveMenuCursor3(1);
            }
            break;
        }
        if (JOY_REPT(DPAD_ANY) == DPAD_LEFT)
        {
            if ((sItemMenuSelection >= 3) && sItemMenuActionList[sItemMenuSelection - 3] != ITEM_ACTION_DUMMY)
            {
                PlaySE(SE_SELECT);
                sItemMenuSelection = MoveMenuCursor3(-3);
            }
            break;
        }
        if (JOY_REPT(DPAD_ANY) == DPAD_RIGHT)
        {
            if ((sItemMenuSelection < 3) && sItemMenuActionList[sItemMenuSelection + 3] != ITEM_ACTION_DUMMY)
            {
                PlaySE(SE_SELECT);
                sItemMenuSelection = MoveMenuCursor3(3);
            }
            break;
        }
        if (JOY_NEW(A_BUTTON))
        {
            gTasks[taskId].data[10] = 0;
            CloseItemMenu(gBGTilemapBuffers[1]);
            r5 = sItemMenuActions[sItemMenuActionList[sItemMenuSelection]].func;
            r5(taskId);
            break;
        }
        if (JOY_NEW(B_BUTTON))
        {
            gTasks[taskId].data[10] = 0;
            PrintBagItemListRange(taskId, gBagPocketScrollStates[sCurrentBagPocket].cursorPos, gBagPocketScrollStates[sCurrentBagPocket].cursorPos);
            CloseItemMenu(gBGTilemapBuffers[1]);
            r5 = sItemMenuActions[sItemMenuActionList[ITEM_MENU_BERRY_CANCEL_CHOICE]].func;
            r5(taskId);
            break;
        }
        break;
    }
    if (r5 == NULL)
    {
        if (sItemMenuSelection == 0)
        {
            sub_8072DDC(12);
        }
        else if (sItemMenuSelection < 3)
        {
            sub_8072DCC(47);
        }
        else
        {
            sub_8072DCC(48);
        }
    }
}

static void DrawLinkItemMenu(void)
{
    u8 r5;

    sItemMenuActionList = sLinkItemMenuChoicesTable[sCurrentBagPocket];
    if (sCurrentBagPocket == BAG_POCKET_KEY_ITEMS)
    {
        sItemMenuOptionCount = 1;
        r5 = 9;
    }
    else if (IsHoldingItemAllowed(gSpecialVar_ItemId) == 0)
    {
        sItemMenuActionList = sLinkItemMenuChoicesTable[4];
        sItemMenuOptionCount = 1;
        r5 = 9;
    }
    else
    {
        sItemMenuOptionCount = 2;
        r5 = 7;
    }
    FillBgTilemapRectWithOne(gBGTilemapBuffers[1], 7, r5 + 1, 6, sItemMenuOptionCount * 2);
    Menu_DrawStdWindowFrame(6, r5, 13, sItemMenuOptionCount * 2 + 1 + r5);
    StartItemMenuOptionsPrinter(0, r5);
    InitMenu(0, 7, r5 + 1, sItemMenuOptionCount, 0, 6);
}

static void Task_HandleLinkItemMenu(u8 taskId)
{
    if (IsItemMenuOptionsPrinterFinished() != 0)
    {
        if (JOY_REPT(DPAD_UP))
        {
            if (sItemMenuSelection != 0)
            {
                PlaySE(SE_SELECT);
                sItemMenuSelection = Menu_MoveCursor(-1);
            }
        }
        else if (JOY_REPT(DPAD_DOWN))
        {
            if (sItemMenuSelection != sItemMenuOptionCount - 1)
            {
                PlaySE(SE_SELECT);
                sItemMenuSelection = Menu_MoveCursor(1);
            }
        }
        else if (JOY_NEW(A_BUTTON))
        {
            gTasks[taskId].data[10] = 0;
            PrintBagItemListRange(taskId, gBagPocketScrollStates[sCurrentBagPocket].cursorPos, gBagPocketScrollStates[sCurrentBagPocket].cursorPos);
            CloseItemMenu(gBGTilemapBuffers[1]);
            sItemMenuActions[sItemMenuActionList[sItemMenuSelection]].func(taskId);
        }
        else if (JOY_NEW(B_BUTTON))
        {
            gTasks[taskId].data[10] = 0;
            CloseItemMenu(gBGTilemapBuffers[1]);
            sItemMenuActions[ITEM_ACTION_CANCEL].func(taskId);
        }
    }
}

static void OnBagClose_Field(u8 taskId)
{
    gTasks[taskId].data[8] = (u32)CB2_ReturnToFieldWithOpenMenu >> 16;
    gTasks[taskId].data[9] = (u32)CB2_ReturnToFieldWithOpenMenu;
    gLastFieldPokeMenuOpened = 0;
    Task_StartBagMenuFadeOut(taskId);
}

static void OnItemSelect_Normal(u8 taskId)
{
    sItemMenuSelection = 0;
    if (sBagMenuLocation == ITEMMENULOCATION_BERRY_BLENDER)
        sItemMenuSelection = 1;
    gTasks[taskId].data[10] = gBagPocketScrollStates[sCurrentBagPocket].scrollTop + gBagPocketScrollStates[sCurrentBagPocket].cursorPos + 1;
    PrintBagItemListRange(taskId, gBagPocketScrollStates[sCurrentBagPocket].cursorPos, gBagPocketScrollStates[sCurrentBagPocket].cursorPos);
    DestroyBagListCursor();
    if (MenuHelpers_IsLinkActive() == TRUE && sBagMenuLocation != ITEMMENULOCATION_BERRY_BLENDER)
    {
        DrawLinkItemMenu();
        gTasks[taskId].func = Task_HandleLinkItemMenu;
    }
    else
    {
        DrawFieldItemMenu(gBGTilemapBuffers[1]);
        if (sCurrentBagPocket != BAG_POCKET_BERRIES || sBagMenuLocation == ITEMMENULOCATION_BERRY_BLENDER)
            gTasks[taskId].func = Task_HandleFieldItemMenu;
        else
            gTasks[taskId].func = Task_HandleBerryItemMenu;
    }
}

static void Task_FadeOutBagMenu(u8 taskId)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = HandleItemMenuPaletteFade;
}

static void Task_StartBagMenuFadeOut(u8 taskId)
{
    PlaySE(SE_SELECT);
    Task_FadeOutBagMenu(taskId);
}

void HandleItemMenuPaletteFade(u8 taskId)
{
    s16 *taskData = gTasks[taskId].data;

    if (!gPaletteFade.active)
    {
        MainCallback cb = (MainCallback)((u16)taskData[8] << 16 | (u16)taskData[9]);

        SetMainCallback2(cb);
        FreeAndReserveObjectSpritePalettes();
        DestroyTask(taskId);
    }
}

void CB2_ReturnToBagMenu(void)
{
    while (1)
    {
        if (SetupBagMultistep() == TRUE)
        {
            ResetTasks();
            sBagMenuTaskId = CreateTask(Task_BagMenu_HandleInput, 0);
            break;
        }
        if (MenuHelpers_IsLinkActive() == TRUE)
            break;
    }
}

static void HandlePopupMenuAction_UseOnField(u8 taskId)
{
    if (GetItemFieldFunc(gSpecialVar_ItemId) != NULL)
    {
        PlaySE(SE_SELECT);
        if (CalculatePlayerPartyCount() == 0 && GetItemType(gSpecialVar_ItemId) == 1)
        {
            DisplayNoPokemonForItemUse(taskId);
        }
        else
        {
            gTasks[taskId].data[2] = 0;
            if (sCurrentBagPocket != BAG_POCKET_BERRIES)
                GetItemFieldFunc(gSpecialVar_ItemId)(taskId);
            else
                ItemUseOutOfBattle_Berry(taskId);
        }
    }
}

static void DisplayNoPokemonForItemUse(u8 taskId)
{
    DestroyBagListCursor();
    ClearBagItemDescription();
    DisplayCannotUseItemMessage(taskId, gText_NoPokemon, CleanUpItemMenuMessage, 1);
}

static void Task_ReturnToBagAfterItemMessage(u8 taskId)
{
    StartBagDescriptionPrinter(0);
    RefreshBagItemList(taskId);
    ItemListMenu_InitMenu();
    Task_EnableBagMenuInput(taskId);
}

void CleanUpItemMenuMessage(u8 taskId)
{
    ClearBgTilemapRect(gBGTilemapBuffers[1], 0, 0, 31, 31);
    Menu_EraseWindowRect(7, 7, 13, 12);
    Menu_EraseWindowRect(0, 14, 29, 19);
    gTasks[taskId].func = Task_ReturnToBagAfterItemMessage;
}

void CleanUpOverworldMessage(u8 taskId)
{
    Menu_EraseWindowRect(0, 13, 29, 19);
    DestroyTask(taskId);
    ScriptUnfreezeObjectEvents();
    UnlockPlayerFieldControls();
}

void ExecuteItemUseFromBlackPalette(void)
{
    FadeInFromBlack();
    CreateTask(Task_CallItemUseOnFieldCallback, 8);
}

static void Task_CallItemUseOnFieldCallback(u8 taskId)
{
    if (IsWeatherNotFadingIn() == TRUE)
        gFieldItemUseCallback(taskId);
}

void CB2_ReturnToBagMenuAfterMail(void)
{
    while (1)
    {
        if (SetupBagMultistep() == TRUE)
        {
            sBagMenuTaskId = CreateTask(Task_BagMenu_HandleInput, 0);
            break;
        }
        if (MenuHelpers_IsLinkActive() == TRUE)
            break;
    }
}

static void Task_ReturnToBagAfterItemToss(u8 taskId)
{
    gTasks[taskId].data[1] = 0;
    CloseItemMenu(gBGTilemapBuffers[1]);
    StartBagDescriptionPrinter(0);
    RefreshBagItemList(taskId);
    ItemListMenu_InitMenu();
    Task_EnableBagMenuInput(taskId);
}

static void DrawTossYesNoWindow(void)
{
    FillBgTilemapRectWithOne(gBGTilemapBuffers[1], 8, 8, 5, 4);
    DisplayYesNoMenu(7, 7, 1);
}

static void ShowTossItemConfirmation(u16 itemId, u16 quantity)
{
    CopyItemName(itemId, gStringVar1);
    if (quantity >= 100)
        ConvertIntToDecimalStringN(gStringVar2, quantity, STR_CONV_MODE_LEFT_ALIGN, 3);
    else
        ConvertIntToDecimalStringN(gStringVar2, quantity, STR_CONV_MODE_LEFT_ALIGN, 2);
    CloseItemMenu(gBGTilemapBuffers[1]);
    StartBagDescriptionPrinter(5);
    DrawTossYesNoWindow();
}

static void CloseTossItemConfirmation(void)
{
    CloseItemMenu(gBGTilemapBuffers[1]);
    Menu_EraseWindowRect(7, 6, 11, 13);
    StartBagDescriptionPrinter(4);
}

static void Task_WaitForTossMessage(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        gTasks[taskId].data[10] = 0;
        RefreshBagItemList(taskId);
        Task_ReturnToBagAfterItemToss(taskId);
    }
}

static void TossItemYes(u8 taskId)
{
    CloseTossItemConfirmation();
    RemoveCurrentBagItemQuantity(taskId);
    gTasks[taskId].func = Task_WaitForTossMessage;
}

static void TossItemNo(u8 taskId)
{
    Task_ReturnToBagAfterItemToss(taskId);
}

static void Task_ChooseItemTossQuantity(u8 taskId)
{
    if (UpdateItemQuantity(taskId) == TRUE)
        return;
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        ShowTossItemConfirmation(gSpecialVar_ItemId, gTasks[taskId].data[1]);
        DoYesNoFuncWithChoice(taskId, &sTossItemYesNoFuncs);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        Task_ReturnToBagAfterItemToss(taskId);
    }
}

static void HandlePopupMenuAction_Toss(u8 taskId)
{
    PlaySE(SE_SELECT);
    gTasks[taskId].func = Task_ChooseItemTossQuantity;
    StartBagDescriptionPrinter(3);
    // if (sCurrentBagPocket == BAG_POCKET_BERRIES)  Can't get it to match this way
    if (sCurrentBagPocket + 1 == BAG_POCKET_BERRIES + 1)
        DrawItemQuantityWindow(taskId, 6, 9, 7, 3, 3);
    else
        DrawItemQuantityWindow(taskId, 7, 9, 6, 3, 2);
}

static void ItemMenu_Cancel(u8 taskId)
{
    PlaySE(SE_SELECT);
    StartBagDescriptionPrinter(0);
    RefreshBagItemList(taskId);
    ItemListMenu_InitMenu();
    Task_EnableBagMenuInput(taskId);
}

static void HandlePopupMenuAction_Register(u8 taskId)
{
    PlaySE(SE_SELECT);
    if (gSaveBlock1.registeredItem == gSpecialVar_ItemId)
    {
        // Un-register the registered item
        RemoveSelectIconFromRegisteredItem();
        gSaveBlock1.registeredItem = 0;
    }
    else
    {
        AddSelectIconToRegisteredItem();
        gSaveBlock1.registeredItem = gSpecialVar_ItemId;
    }
    StartBagDescriptionPrinter(0);
    RefreshBagItemList(taskId);
    ItemListMenu_InitMenu();
    Task_EnableBagMenuInput(taskId);
}

static void Task_ReturnToBagAfterItemAction(u8 taskId)
{
    StartBagDescriptionPrinter(0);
    RefreshBagItemList(taskId);
    ItemListMenu_InitMenu();
    Task_EnableBagMenuInput(taskId);
}

static void Task_CloseItemUseMessage(u8 taskId)
{
    if (JOY_NEW(A_BUTTON))
    {
        ClearBgTilemapRect(gBGTilemapBuffers[1], 0, 0, 31, 31);
        Menu_EraseWindowRect(0, 14, 29, 19);
        gTasks[taskId].func = Task_ReturnToBagAfterItemAction;
    }
}

static void DisplayCannotBeHeldMessage(u8 taskId)
{
    DestroyBagListCursor();
    CopyItemName(gSpecialVar_ItemId, gStringVar1);
    StringExpandPlaceholders(gStringVar4, gOtherText_CantBeHeld);
    ClearBagItemDescription();
    DisplayCannotUseItemMessage(taskId, gStringVar4, Task_CloseItemUseMessage, 1);
}

static void HandlePopupMenuAction_Give(u8 taskId)
{
    PlaySE(SE_SELECT);
    if (IsWritingMailAllowed(gSpecialVar_ItemId) == 0)
    {
        DestroyBagListCursor();
        ClearBagItemDescription();
        DisplayCannotUseItemMessage(taskId, gOtherText_CantWriteMail, Task_CloseItemUseMessage, 1);
    }
    else if (GetItemImportance(gSpecialVar_ItemId) == 0)
    {
        if (CalculatePlayerPartyCount() == 0)
        {
            DisplayNoPokemonForItemUse(taskId);
        }
        else
        {
            gTasks[taskId].data[8] = (u32)CB2_OpenPartyMenuForItemUse >> 16;
            gTasks[taskId].data[9] = (u32)CB2_OpenPartyMenuForItemUse;
            gTasks[taskId].func = HandleItemMenuPaletteFade;
            gPokemonItemUseType = ITEM_USE_GIVE_ITEM;
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        }
    }
    else
    {
        DisplayCannotBeHeldMessage(taskId);
    }
}

static void HandlePopupMenuAction_CheckTag(u8 taskId)
{
    Task_StartBagMenuFadeOut(taskId);
    gTasks[taskId].data[8] = (u32)DoBerryTagScreen >> 16;
    gTasks[taskId].data[9] = (u32)DoBerryTagScreen;
}

static void CB2_InitBagMenuForParty(void)
{
    if (TrySetupBagMenu() == TRUE)
        sBagMenuTaskId = CreateTask(Task_BagMenu_HandleInput, 0);
}

void CB2_BagMenuFromParty(void)
{
    sBagMenuLocation = ITEMMENULOCATION_PARTY;
    SetMainCallback2(CB2_InitBagMenuForParty);
}

static void OnBagClose_Party(u8 taskId)
{
    gTasks[taskId].data[8] = (u32)CB2_ReturnToPartyMenuFromBag >> 16;
    gTasks[taskId].data[9] = (u32)CB2_ReturnToPartyMenuFromBag;
    Task_StartBagMenuFadeOut(taskId);
}

static void OnItemSelect_Party(u8 taskId)
{
    u8 r6 = sCurrentBagPocket + 1;

    if (IsWritingMailAllowed(gSpecialVar_ItemId) == 0)
    {
        DestroyBagListCursor();
        ClearBagItemDescription();
        DisplayCannotUseItemMessage(taskId, gOtherText_CantWriteMail, Task_CloseItemUseMessage, 1);
    }
    else if (IsHoldingItemAllowed(gSpecialVar_ItemId) == 0)
    {
        DestroyBagListCursor();
        CopyItemName(gSpecialVar_ItemId, gStringVar1);
        StringExpandPlaceholders(gStringVar4, gOtherText_CantBeHeldHere);
        ClearBagItemDescription();
        DisplayCannotUseItemMessage(taskId, gStringVar4, Task_CloseItemUseMessage, 1);
    }
    else if (r6 != 5 && GetItemImportance(gSpecialVar_ItemId) == 0)
    {
        gTasks[taskId].data[8] = (u32)CB2_ReturnToPartyMenuFromBag >> 16;
        gTasks[taskId].data[9] = (u32)CB2_ReturnToPartyMenuFromBag;
        Task_FadeOutBagMenu(taskId);
    }
    else
    {
        DisplayCannotBeHeldMessage(taskId);
    }
}

static void CB2_InitBagMenuForSell(void)
{
    if (TrySetupBagMenu() == TRUE)
        sBagMenuTaskId = CreateTask(Task_BagMenu_HandleInput, 0);
}

void CB2_GoToSellMenu(void)
{
    sBagMenuLocation = ITEMMENULOCATION_SHOP;
    SetMainCallback2(CB2_InitBagMenuForSell);
}

static void OnBagClose_Shop(u8 taskId)
{
    gFieldCallback = Shop_FadeReturnToMartMenu;
    gTasks[taskId].data[8] = (u32)CB2_ReturnToField >> 16;
    gTasks[taskId].data[9] = (u32)CB2_ReturnToField;
    Task_StartBagMenuFadeOut(taskId);
}

static void OnItemSelect_Shop(u8 taskId)
{
    ClearBagItemDescription();
    gTasks[taskId].data[10] = gBagPocketScrollStates[sCurrentBagPocket].scrollTop + gBagPocketScrollStates[sCurrentBagPocket].cursorPos + 1;
    PrintBagItemListRange(taskId, gBagPocketScrollStates[sCurrentBagPocket].cursorPos, gBagPocketScrollStates[sCurrentBagPocket].cursorPos);
    DestroyBagListCursor();
    CopyItemName(gSpecialVar_ItemId, gStringVar2);
    if (GetItemPrice(gSpecialVar_ItemId) == 0)
    {
        StringExpandPlaceholders(gStringVar4, gOtherText_CantBuyThat);
        DisplayCannotUseItemMessage(taskId, gStringVar4, Task_CloseSellErrorMessage, 1);
    }
    else
    {
        StringExpandPlaceholders(gStringVar4, gOtherText_HowManyToSell);
        DisplayCannotUseItemMessage(taskId, gStringVar4, Task_SetSellQuantity, 1);
    }
}

static void Task_ReturnToBagAfterSellError(u8 taskId)
{
    gTasks[taskId].data[10] = 0;
    StartBagDescriptionPrinter(0);
    RefreshBagItemList(taskId);
    ItemListMenu_InitMenu();
    Task_EnableBagMenuInput(taskId);
}

static void Task_CloseSellErrorMessage(u8 taskId)
{
    ClearBgTilemapRect(gBGTilemapBuffers[1], 0, 0, 31, 31);
    Menu_EraseWindowRect(0, 14, 29, 19);
    gTasks[taskId].func = Task_ReturnToBagAfterSellError;
}

static void Task_SetSellQuantity(u8 taskId)
{
    gTasks[taskId].func = Task_BuyHowManyDialogueHandleInput;
    FillBgTilemapRectWithOne(gBGTilemapBuffers[1], 1, 11, 12, 2);
    Menu_DrawStdWindowFrame(0, 10, 13, 13);
    gTasks[taskId].data[1] = 1;
    gTasks[taskId].data[2] = 1;
    gTasks[taskId].data[3] = 11;
    // if (sCurrentBagPocket == BAG_POCKET_BERRIES)  Can't get it to match this way
    if (sCurrentBagPocket + 1 == BAG_POCKET_BERRIES + 1)
        PrintItemQuantity(1, 1, 1, 11, 3);
    else
        PrintItemQuantity(1, 1, 1, 11, 2);
    BuyMenuDisplayMessage(gSpecialVar_ItemId, 1);
    DisplaySellMoneyWindow();
}

static void Task_ConfirmSell(u8 taskId)
{
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        BuyMenuPrintItemQuantityAndPrice(taskId);
    }
}

static void Task_ReturnToSellMenu(u8 taskId)
{
    PrintBagItemListForMessage(taskId);
    gTasks[taskId].func = Task_ConfirmSell;
}

static void Task_SellItem(u8 taskId)
{
    PlaySE(SE_SHOP);
    AddSaleMoney(gSpecialVar_ItemId, gTasks[taskId].data[1]);
    gTasks[taskId].func = Task_ReturnToSellMenu;
}

static void SellItemYes(u8 taskId)
{
    Menu_EraseWindowRect(7, 6, 13, 12);
    ClearBgTilemapRect(gBGTilemapBuffers[1], 7, 6, 6, 6);
    CopyItemName(gSpecialVar_ItemId, gStringVar2);
    StringExpandPlaceholders(gStringVar4, gOtherText_SoldItem);
    DisplayCannotUseItemMessage(taskId, gStringVar4, Task_SellItem, 1);
    RemoveCurrentBagItemQuantity(taskId);
}

static void SellItemNo(u8 taskId)
{
    Menu_EraseWindowRect(7, 6, 13, 12);
    ClearBgTilemapRect(gBGTilemapBuffers[1], 7, 6, 6, 6);
    BuyMenuPrintItemQuantityAndPrice(taskId);
}

static void AskToSellItem(u8 taskId)
{
    DrawTossYesNoWindow();
    DoYesNoFuncWithChoice(taskId, &sSellItemYesNoFuncs);
}

static void Task_BuyHowManyDialogueHandleInput(u8 taskId)
{
    if (UpdateItemQuantity(taskId) == TRUE)
    {
        Menu_EraseWindowRect(6, 11, 12, 11);
        BuyMenuDisplayMessage(gSpecialVar_ItemId, gTasks[taskId].data[1]);
    }
    else if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        Menu_EraseWindowRect(0, 10, 13, 13);
        ClearBgTilemapRect(gBGTilemapBuffers[1], 0, 10, 13, 3);
        ConvertIntToDecimalStringN(gStringVar1, GetItemPrice(gSpecialVar_ItemId) / 2 * gTasks[taskId].data[1], STR_CONV_MODE_LEFT_ALIGN, 6);
        StringExpandPlaceholders(gStringVar4, gOtherText_CanPay);
        DisplayCannotUseItemMessage(taskId, gStringVar4, AskToSellItem, 1);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        BuyMenuPrintItemQuantityAndPrice(taskId);
    }
}

static void Task_ReturnToBagAfterSell(u8 taskId)
{
    gTasks[taskId].data[10] = 0;
    StartBagDescriptionPrinter(0);
    RefreshBagItemList(taskId);
    ItemListMenu_InitMenu();
    Task_EnableBagMenuInput(taskId);
}

static void BuyMenuPrintItemQuantityAndPrice(u8 taskId)
{
    ClearBgTilemapRect(gBGTilemapBuffers[1], 0, 0, 31, 31);
    HideMoneyBox(0, 0);
    Menu_EraseWindowRect(0, 4, 13, 13);
    Menu_EraseWindowRect(0, 14, 29, 19);
    gTasks[taskId].func = Task_ReturnToBagAfterSell;
}

static void BuyMenuDisplayMessage(u16 itemId, u16 quantity)
{
    PrintMoneyAmount(GetItemPrice(itemId) / 2 * quantity, 6, 6, 11);
    ConvertIntToDecimalStringN(gStringVar1, GetItemPrice(itemId) / 2 * quantity, STR_CONV_MODE_LEFT_ALIGN, 6);
}

static void DisplaySellMoneyWindow(void)
{
    DrawMoneyBox(gSaveBlock1.money, 0, 0);
    FillBgTilemapRectWithOne(gBGTilemapBuffers[1], 1, 1, 12, 2);
}

static void AddSaleMoney(u16 itemId, u8 quantity)
{
    AddMoney(&gSaveBlock1.money, GetItemPrice(itemId) / 2 * quantity);
    ChangeAmountInMoneyBox(gSaveBlock1.money, 0, 0);
}

static void CB2_InitBerrySelectionBagMenu(void)
{
    if (TrySetupBagMenu() == TRUE)
        sBagMenuTaskId = CreateTask(Task_BagMenu_HandleInput, 0);
}

void CB2_ChooseBerry(void)
{
    sCurrentBagPocket = BAG_POCKET_BERRIES;
    sBagMenuLocation = ITEMMENULOCATION_BERRY_TREE;
    SetMainCallback2(CB2_InitBerrySelectionBagMenu);
}

static void OnBagClose_BerryTree(u8 taskId)
{
    gTasks[taskId].data[8] = (u32)CB2_ReturnToFieldContinueScript >> 16;
    gTasks[taskId].data[9] = (u32)CB2_ReturnToFieldContinueScript;
    Task_StartBagMenuFadeOut(taskId);
}

static void OnItemSelect_BerryTree(u8 taskId)
{
    gTasks[taskId].data[8] = (u32)CB2_ReturnToFieldContinueScript >> 16;
    gTasks[taskId].data[9] = (u32)CB2_ReturnToFieldContinueScript;
    Task_FadeOutBagMenu(taskId);
}

static void CB2_InitBerryBlenderBagMenu(void)
{
    while (1)
    {
        if (SetupBagMultistep() == TRUE)
        {
            sBagMenuTaskId = CreateTask(Task_BagMenu_HandleInput, 0);
            SetBerryBlenderItemMenuActions();
            break;
        }
        if (MenuHelpers_IsLinkActive() == TRUE)
            break;
    }
}

void ChooseBerryForBlender(void)
{
    sCurrentBagPocket = BAG_POCKET_BERRIES;
    sBagMenuLocation = ITEMMENULOCATION_BERRY_BLENDER;
    SetMainCallback2(CB2_InitBerryBlenderBagMenu);
}

static void SetBerryBlenderItemMenuActions(void)
{
    sItemMenuActionList = sBerryBlenderItemMenuActions;
    sItemMenuOptionCount = 4;
}

static void OnBagClose_BerryBlender(u8 taskId)
{
    gTasks[taskId].data[8] = (u32)StartBlender >> 16;
    gTasks[taskId].data[9] = (u32)StartBlender;
    Task_StartBagMenuFadeOut(taskId);
}

static void HandlePopupMenuAction_Confirm(u8 taskId)
{
    gTasks[taskId].data[8] = (u32)StartBlender >> 16;
    gTasks[taskId].data[9] = (u32)StartBlender;
    Task_FadeOutBagMenu(taskId);
}

static void CB2_InitItemDepositBagMenu(void)
{
    if (TrySetupBagMenu() == TRUE)
        sBagMenuTaskId = CreateTask(Task_BagMenu_HandleInput, 0);
}

void CB2_GoToItemDepositMenu(void)
{
    sBagMenuLocation = ITEMMENULOCATION_ITEMPC;
    SetMainCallback2(CB2_InitItemDepositBagMenu);
}

static void OnBagClose_ItemPC(u8 taskId)
{
    gFieldCallback = ItemStorage_ReshowAfterBagMenu;
    gTasks[taskId].data[8] = (u32)CB2_ReturnToField >> 16;
    gTasks[taskId].data[9] = (u32)CB2_ReturnToField;
    Task_StartBagMenuFadeOut(taskId);
}

static void FinishItemDeposit(u8 taskId)
{
    s16 *taskData = gTasks[taskId].data;

    CloseItemMenu(gBGTilemapBuffers[1]);
    CopyItemName(gSpecialVar_ItemId, gStringVar1);
    ConvertIntToDecimalStringN(gStringVar2, taskData[1], STR_CONV_MODE_LEFT_ALIGN, 3);
    Menu_EraseWindowRect(7, 6, 11, 13);
    StartBagDescriptionPrinter(7);
    RemoveCurrentBagItemQuantity(taskId);
    gTasks[taskId].func = Task_WaitForTossMessage;
}

static void Task_WaitForItemDepositMessage(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        gTasks[taskId].data[10] = 0;
        PrintBagItemListRange(taskId, gBagPocketScrollStates[sCurrentBagPocket].cursorPos, gBagPocketScrollStates[sCurrentBagPocket].cursorPos);
        PlaySE(SE_SELECT);
        Task_ReturnToBagAfterItemToss(taskId);
    }
}

static void TryDepositItem(u8 taskId)
{
    s16 *taskData = gTasks[taskId].data;

    if (GetItemImportance(gSpecialVar_ItemId) == 2)
    {
        gTasks[taskId].func = Task_WaitForItemDepositMessage;
        StartBagDescriptionPrinter(9);
    }
    else
    {
        if (AddPCItem(gSpecialVar_ItemId, taskData[1]) == TRUE)
        {
            FinishItemDeposit(taskId);
        }
        else
        {
            gTasks[taskId].func = Task_WaitForItemDepositMessage;
            StartBagDescriptionPrinter(8);
        }
    }
}

static void Task_ChooseItemDepositQuantity(u8 taskId)
{
    if (UpdateItemQuantity(taskId) == TRUE)
        return;
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        CloseItemMenu(gBGTilemapBuffers[1]);
        TryDepositItem(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        gTasks[taskId].data[10] = 0;
        PrintBagItemListRange(taskId, gBagPocketScrollStates[sCurrentBagPocket].cursorPos, gBagPocketScrollStates[sCurrentBagPocket].cursorPos);
        PlaySE(SE_SELECT);
        Task_ReturnToBagAfterItemToss(taskId);
    }
}

static void OnItemSelect_ItemPC(u8 taskId)
{
    gTasks[taskId].data[10] = gBagPocketScrollStates[sCurrentBagPocket].scrollTop + gBagPocketScrollStates[sCurrentBagPocket].cursorPos + 1;
    PrintBagItemListRange(taskId, gBagPocketScrollStates[sCurrentBagPocket].cursorPos, gBagPocketScrollStates[sCurrentBagPocket].cursorPos);
    DestroyBagListCursor();
    PlaySE(SE_SELECT);
    ClearBagItemDescription();
    if (sCurrentBagPocket != BAG_POCKET_KEY_ITEMS)
    {
        gTasks[taskId].func = Task_ChooseItemDepositQuantity;
        StartBagDescriptionPrinter(6);
        // if (sCurrentBagPocket == BAG_POCKET_BERRIES)  Can't get it to match this way
        if (sCurrentBagPocket + 1 == BAG_POCKET_BERRIES + 1)
            DrawItemQuantityWindow(taskId, 6, 9, 7, 3, 3);
        else
            DrawItemQuantityWindow(taskId, 7, 9, 6, 3, 2);
    }
    else
    {
        gTasks[taskId].data[1] = 1;
        TryDepositItem(taskId);
    }
}

bool32 UseRegisteredKeyItem(void)
{
    HideMapNamePopUpWindow();
    if (gSaveBlock1.registeredItem != ITEM_NONE)
    {
        if (CheckBagHasItem(gSaveBlock1.registeredItem, 1) == TRUE)
        {
            u8 taskId;

            LockPlayerFieldControls();
            FreezeObjectEvents();
            PlayerFreeze();
            StopPlayerAvatar();
            gSpecialVar_ItemId = gSaveBlock1.registeredItem;
            taskId = CreateTask(GetItemFieldFunc(gSaveBlock1.registeredItem), 8);
            gTasks[taskId].data[2] = 1;
            return TRUE;
        }
        gSaveBlock1.registeredItem = 0;
    }
    ScriptContext_SetupScript(Event_NoRegisteredItem);
    return TRUE;
}

static void CB2_InitBagMenuForBattle(void)
{
    while (1)
    {
        if (SetupBagMultistep() == TRUE)
        {
            sBagMenuTaskId = CreateTask(Task_BagMenu_HandleInput, 0);
            break;
        }
        if (MenuHelpers_IsLinkActive() == TRUE)
            break;
    }
}

void CB2_BagMenuFromBattle(void)
{
    sItemMenuSelection = 0;
    sBagMenuLocation = ITEMMENULOCATION_BATTLE;
    SetMainCallback2(CB2_InitBagMenuForBattle);
}

static void DrawBattleItemMenu(u16 *a)
{
    u8 r6 = (sItemMenuOptionCount - 1) * 2;

    Menu_DrawStdWindowFrame(7, 9 - r6, 13, 12);
    FillBgTilemapRectWithOne(a, 8, 10 - r6, 5, r6 + 2);
    if (sItemMenuOptionCount == 1)
    {
        Menu_PrintText(sItemMenuActions[sItemMenuActionList[0]].text, 8, 10);
    }
    else
    {
        Menu_PrintText(sItemMenuActions[sItemMenuActionList[0]].text, 8, 8);
        Menu_PrintText(sItemMenuActions[sItemMenuActionList[1]].text, 8, 10);
    }
    InitMenu(0, 8, 10 - r6, sItemMenuOptionCount, 0, 5);
    StartBagDescriptionPrinter(2);
}

static void Task_HandleBattleItemMenu(u8 taskId)
{
    if (JOY_REPT(DPAD_ANY) == DPAD_UP)
    {
        if (sItemMenuSelection == 1)
        {
            PlaySE(SE_SELECT);
            sItemMenuSelection = Menu_MoveCursor(-1);
        }
    }
    else if (JOY_REPT(DPAD_ANY) == DPAD_DOWN)
    {
        if (sItemMenuSelection + 1 < sItemMenuOptionCount)
        {
            PlaySE(SE_SELECT);
            sItemMenuSelection = Menu_MoveCursor(1);
        }
    }
    else if (JOY_NEW(A_BUTTON))
    {
        gTasks[taskId].data[10] = 0;
        PrintBagItemListRange(taskId, gBagPocketScrollStates[sCurrentBagPocket].cursorPos, gBagPocketScrollStates[sCurrentBagPocket].cursorPos);
        CloseItemMenu(gBGTilemapBuffers[1]);
        sItemMenuActions[sItemMenuActionList[sItemMenuSelection]].func(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        gTasks[taskId].data[10] = 0;
        PrintBagItemListRange(taskId, gBagPocketScrollStates[sCurrentBagPocket].cursorPos, gBagPocketScrollStates[sCurrentBagPocket].cursorPos);
        CloseItemMenu(gBGTilemapBuffers[1]);
        sItemMenuActions[ITEM_ACTION_BATTLE_CANCEL].func(taskId);
    }
}

static const u8 sBattleItemMenuActions[] = {ITEM_ACTION_BATTLE_USE, ITEM_ACTION_BATTLE_CANCEL};
static const u8 sBattleItemMenuCancelAction[] = {ITEM_ACTION_BATTLE_CANCEL, ITEM_ACTION_USE};

static void SetBattleItemMenuActions(void)
{
    if (GetItemBattleUsage(gSpecialVar_ItemId) != 0)
    {
        sItemMenuActionList = sBattleItemMenuActions;
        sItemMenuOptionCount = 2;
    }
    else
    {
        sItemMenuActionList = sBattleItemMenuCancelAction;
        sItemMenuOptionCount = 1;
    }
}

static void OnItemSelect_Battle(u8 taskId)
{
    sItemMenuSelection = 0;
    SetBattleItemMenuActions();
    gTasks[taskId].data[10] = gBagPocketScrollStates[sCurrentBagPocket].scrollTop + gBagPocketScrollStates[sCurrentBagPocket].cursorPos + 1;
    PrintBagItemListRange(taskId, gBagPocketScrollStates[sCurrentBagPocket].cursorPos, gBagPocketScrollStates[sCurrentBagPocket].cursorPos);
    DestroyBagListCursor();
    DrawBattleItemMenu(gBGTilemapBuffers[1]);
    gTasks[taskId].func = Task_HandleBattleItemMenu;
}

void CloseBagMenuForBattle(u8 taskId)
{
    gTasks[taskId].data[8] = (u32)CB2_SetUpReshowBattleScreenAfterMenu2 >> 16;
    gTasks[taskId].data[9] = (u32)CB2_SetUpReshowBattleScreenAfterMenu2;
    gTasks[taskId].func = HandleItemMenuPaletteFade;
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
}

static void OnBagClose_Battle(u8 taskId)
{
    PlaySE(SE_SELECT);
    CloseBagMenuForBattle(taskId);
}

static void HandlePopupMenuAction_UseInBattle(u8 taskId)
{
    if (GetItemBattleFunc(gSpecialVar_ItemId) != NULL)
    {
        PlaySE(SE_SELECT);
        GetItemBattleFunc(gSpecialVar_ItemId)(taskId);
    }
}

static void ItemMenu_CancelBattle(u8 taskId)
{
    PlaySE(SE_SELECT);
    StartBagDescriptionPrinter(0);
    RefreshBagItemList(taskId);
    ItemListMenu_InitMenu();
    Task_EnableBagMenuInput(taskId);
}

static void CB2_InitWallyTutorialBagMenu(void)
{
    while (1)
    {
        if (SetupBagMultistep() == TRUE)
        {
            sBagMenuTaskId = CreateTask(Task_WallyTutorialBagMenu, 0);
            gTasks[sBagMenuTaskId].data[15] = 0;
            break;
        }
        if (MenuHelpers_IsLinkActive() == TRUE)
            break;
    }
}

void DoWallyTutorialBagMenu(void)
{
    u8 i;

    sItemMenuSelection = 0;
    sCurrentBagPocket = BAG_POCKET_ITEMS;
    for (i = 0; i < 5; i++)
    {
        gBagPocketScrollStates[i].cursorPos = 0;
        gBagPocketScrollStates[i].scrollTop = 0;
    }

    // Save player's items
    memcpy(ewramSavedItemsPocket, gSaveBlock1.bagPocket_Items, sizeof(gSaveBlock1.bagPocket_Items));
    memcpy(ewramSavedPokeballsPocket, gSaveBlock1.bagPocket_PokeBalls, sizeof(gSaveBlock1.bagPocket_PokeBalls));

    // Add Wally's items to the bag
    ClearItemSlots(gSaveBlock1.bagPocket_Items, ARRAY_COUNT(gSaveBlock1.bagPocket_Items));
    ClearItemSlots(gSaveBlock1.bagPocket_PokeBalls, ARRAY_COUNT(gSaveBlock1.bagPocket_PokeBalls));
    AddBagItem(ITEM_POTION, 1);
    AddBagItem(ITEM_POKE_BALL, 1);

    sBagMenuLocation = ITEMMENULOCATION_WALLY;
    SetMainCallback2(CB2_InitWallyTutorialBagMenu);
}

static void Task_WallyTutorialBagMenu(u8 taskId)
{
    s16 *taskData = gTasks[taskId].data;

    switch (taskData[15])
    {
    case 102:
        PlaySE(SE_SELECT);
        SwitchBagPocket(1, 2);
        break;
    case 204:
        PlaySE(SE_SELECT);
        StopVerticalScrollIndicators(LEFT_ARROW);
        StopVerticalScrollIndicators(RIGHT_ARROW);
        gSpecialVar_ItemId = ITEM_POKE_BALL;
        sItemMenuActionList = sBattleItemMenuActions;
        sItemMenuOptionCount = 2;
        gTasks[taskId].data[10] = gBagPocketScrollStates[sCurrentBagPocket].scrollTop + gBagPocketScrollStates[sCurrentBagPocket].cursorPos + 1;
        PrintBagItemListRange(taskId, gBagPocketScrollStates[sCurrentBagPocket].cursorPos, gBagPocketScrollStates[sCurrentBagPocket].cursorPos);
        DestroyBagListCursor();
        DrawBattleItemMenu(gBGTilemapBuffers[1]);
        break;
    case 306:
        PlaySE(SE_SELECT);
        CloseItemMenu(gBGTilemapBuffers[1]);

        // Restore player's items
        memcpy(gSaveBlock1.bagPocket_Items, ewramSavedItemsPocket, sizeof(gSaveBlock1.bagPocket_Items));
        memcpy(gSaveBlock1.bagPocket_PokeBalls, ewramSavedPokeballsPocket, sizeof(gSaveBlock1.bagPocket_PokeBalls));

        taskData[8] = (u32)CB2_SetUpReshowBattleScreenAfterMenu2 >> 16;
        taskData[9] = (u32)CB2_SetUpReshowBattleScreenAfterMenu2;
        gTasks[taskId].func = HandleItemMenuPaletteFade;
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        return;
    }
    taskData[15]++;
}

static void ItemListMenu_InitMenu(void)
{
    InitMenu(0, 14, 2, gBagPocketScrollStates[sCurrentBagPocket].cursorMax + 1, gBagPocketScrollStates[sCurrentBagPocket].cursorPos, 0);
    CreateBlendedOutlineCursor(16, 0xFFFF, 12, 0x2D9F, 15);
    SetBagListCursorPosition();
}

static void SetBagListCursorPosition(void)
{
    SetBlendedOutlineCursorPosition(0x70, gBagPocketScrollStates[sCurrentBagPocket].cursorPos * 16 + 16);
}

static void HideBagListCursor(void)
{
    HideBlendedOutlineCursor();
}

static void DestroyBagListCursor(void)
{
    Menu_DestroyCursor();
    DestroyBlendedOutlineCursor();
}

static void UpdateBagMenu(void)
{
    UpdateBagItemList();
    UpdateItemMenuOptions();
    UpdateBagItemDescription();
#if DEBUG
    if (gLinkOpen == TRUE)
        debug_sub_8008264(gLink.recvQueue.count, 1, 1, 2, 0);
#endif
}

static void UpdateBagItemDescription(void)
{
    struct BagDescriptionPrinter *printer = &sBagDescriptionPrinter;
    int index;

    switch (printer->state)
    {
    case 3:
        printer->state = 2;
        break;
    case 2:
        switch (printer->messageType)
        {
        case 0:
            index = gBagPocketScrollStates[sCurrentBagPocket].scrollTop + gBagPocketScrollStates[sCurrentBagPocket].cursorPos;
            ItemListMenu_ChangeDescription(gCurrentBagPocketItemSlots[index].itemId, printer->textLine);
            break;
        case 1:
            PrintBagDescriptionLine(gText_SwitchWhichItem, printer->textLine);
            break;
        case 2:
            PrintBagDescriptionLine(gOtherText_WhatWillYouDo2, printer->textLine);
            break;
        case 3:
            PrintBagDescriptionLine(gText_TossHowManyItems, printer->textLine);
            break;
        case 4:
            PrintBagDescriptionLine(gText_ThrewAwayVar2Var1s, printer->textLine);
            break;
        case 5:
            PrintBagDescriptionLine(gText_ConfirmTossItems, printer->textLine);
            break;
        case 6:
            PrintBagDescriptionLine(gOtherText_HowManyToDeposit, printer->textLine);
            break;
        case 7:
            PrintBagDescriptionLine(gOtherText_DepositedItems, printer->textLine);
            break;
        case 8:
            PrintBagDescriptionLine(gOtherText_NoRoomForItems, printer->textLine);
            break;
        case 9:
            PrintBagDescriptionLine(gOtherText_CantStoreSomeoneItem, printer->textLine);
            break;
        }
        printer->textLine++;
        if (printer->textLine == 3)
        {
            printer->state = 0;
            RunDeferredBagItemMessageAfterList();
        }
        break;
    }
}

static void ResetBagDescriptionPrinter(void)
{
    sBagDescriptionPrinter.state = 0;
}

static void StartBagDescriptionPrinter(u8 a)
{
    sBagDescriptionPrinter.textLine = 0;
    sBagDescriptionPrinter.messageType = a;
    switch (GetBagDescriptionPrinterState())
    {
    case 0:
        sBagDescriptionPrinter.state = 2;
        while (sBagDescriptionPrinter.state != 0)
            UpdateBagItemDescription();
        break;
    case 1:
        sBagDescriptionPrinter.state = 3;
        break;
    case 2:
        sBagDescriptionPrinter.state = 1;
        break;
    }
}

static void StopBagDescriptionPrinter(void)
{
    if (sBagDescriptionPrinter.state == 2 || sBagDescriptionPrinter.state == 3)
        RunDeferredBagItemMessageAfterList();
    sBagDescriptionPrinter.state = 0;
}

static void ClearBagItemDescription(void)
{
    Menu_EraseWindowRect(0, 13, 13, 20);
    StopBagDescriptionPrinter();
}

static void DelayBagDescriptionPrinter(void)
{
    if (sBagDescriptionPrinter.state == 2 || sBagDescriptionPrinter.state == 3)
        sBagDescriptionPrinter.state = 1;
}

static void ResumeBagDescriptionPrinter(void)
{
    switch (sBagDescriptionPrinter.state)
    {
    case 1:
        sBagDescriptionPrinter.state = 3;
        break;
    case 0:
        RunDeferredBagItemMessageAfterList();
        break;
    }
}

static void UpdateBagItemList(void)
{
    if (sBagItemListPrinter.state == 1 && ConsumeBagItemListRedrawFlag() == 0)
    {
        int r1;

        PrintBagItemList(sBagMenuTaskId, sBagItemListPrinter.nextRow, sBagItemListPrinter.nextRow, 1);
        sBagItemListPrinter.nextRow++;
        if (sBagItemListPrinter.isShortList != 0)
            r1 = 5;
        else
            r1 = 7;
        if (r1 < sBagItemListPrinter.nextRow)
        {
            sBagItemListPrinter.state = 0;
            ResumeItemMenuOptionsAfterList();
        }
    }
}

static void ResetBagItemListPrinter(void)
{
    sBagItemListPrinter.state = 0;
}

static void StartBagItemListPrinter(void)
{
    struct BagItemListPrinter *printer = &sBagItemListPrinter;

    switch (ShouldDelayBagItemListPrinter())
    {
    case FALSE:
        PrintBagItemListRange(sBagMenuTaskId, 0, 7);
        break;
    case TRUE:
        printer->state = 1;
        printer->nextRow = 0;
        printer->isShortList = 0;
        break;
    }
}

static void StopBagItemListPrinter(void)
{
    if (sBagItemListPrinter.state == 1)
        ResumeItemMenuOptionsAfterList();
    sBagItemListPrinter.state = 0;
}

static void SetBagItemListPrinterShort(void)
{
    sBagItemListPrinter.isShortList = 1;
}

static void ClearBagItemList(void)
{
    Menu_EraseWindowRect(14, 2, 29, 18);
    StopBagItemListPrinter();
}

static bool32 IsBagItemListPrinterFinished(void)
{
    struct BagItemListPrinter *printer = &sBagItemListPrinter;

    return (printer->state == 0);
}

static bool32 HasBagItemListPrinterPassedVisibleRows(void)
{
    struct BagItemListPrinter *printer = &sBagItemListPrinter;

    return (printer->nextRow > 5);
}

static void RunDeferredBagItemMessage(void)
{
    if (sDeferredBagItemMessage.isPending == 1)
    {
        DisplayItemMessageOnField(
          sDeferredBagItemMessage.taskId,
          sDeferredBagItemMessage.text,
          sDeferredBagItemMessage.callback,
          sDeferredBagItemMessage.messageMode);
        sDeferredBagItemMessage.isPending = 0;
    }
}

static void ResetDeferredBagItemMessage(void)
{
    sDeferredBagItemMessage.isPending = 0;
}

static void DisplayCannotUseItemMessage(int a, const u8 *b, TaskFunc func, int d)
{
    struct DeferredBagItemMessage *message = &sDeferredBagItemMessage;

    switch (PrepareBagItemMessage())
    {
    case 0:
        DisplayItemMessageOnField(a, b, func, d);
        break;
    case 2:
        message->isPending = 1;
        message->taskId = a;
        message->text = b;
        message->callback = func;
        message->messageMode = d;
        break;
    }
}

static void UpdateItemMenuOptions(void)
{
    struct ItemMenuOptionsPrinter *printer = &sItemMenuOptionsPrinter;

    if (printer->state == 2)
    {
        if (printer->nextOption != 0)
        {
            if (printer->isGridLayout != 1)
            {
                const u8 *text = sItemMenuActions[sItemMenuActionList[printer->nextOption - 1]].text;
                int var = printer->nextOption - 1;

                Menu_PrintText(text, 7, var * 2 + 1 + printer->topRow);
            }
            else
            {
                const u8 *text;
                int var;

                if (printer->nextOption == 1)
                    text = GetFirstItemMenuActionText(gSpecialVar_ItemId);
                else
                    text = sItemMenuActions[sItemMenuActionList[printer->nextOption - 1]].text;
                var = printer->nextOption - 1;
                Menu_PrintText(text, (var >> 1) * 6 + 1, (var & 1) * 2 + 8);
            }
            if (printer->nextOption == sItemMenuOptionCount)
            {
                printer->state = 0;
                ResumeBagDescriptionPrinterAfterOptions();
            }
        }
        printer->nextOption++;
    }
}

static void ResetItemMenuOptionsPrinter(void)
{
    sItemMenuOptionsPrinter.state = 0;
}

static void StartItemMenuOptionsPrinter(int a, int b)
{
    struct ItemMenuOptionsPrinter *printer = &sItemMenuOptionsPrinter;

    switch (PrepareItemMenuOptionsPrinter())
    {
    case 1:
        printer->state = 2;
        printer->nextOption = 0;
        printer->isGridLayout = a;
        printer->topRow = b;
        break;
    case 2:
        printer->state = 1;
        printer->nextOption = 0;
        printer->isGridLayout = a;
        printer->topRow = b;
        break;
    }
}

static void DelayItemMenuOptionsPrinter(void)
{
    if (sItemMenuOptionsPrinter.state == 2)
        sItemMenuOptionsPrinter.state = 1;
}

static void ResumeItemMenuOptionsPrinter(void)
{
    switch (sItemMenuOptionsPrinter.state)
    {
    case 1:
        sItemMenuOptionsPrinter.state = 2;
        break;
    case 0:
        ResumeBagDescriptionPrinterAfterOptions();
        break;
    }
}

static bool32 IsItemMenuOptionsPrinterFinished(void)
{
    struct ItemMenuOptionsPrinter *printer = &sItemMenuOptionsPrinter;

    return (printer->state == 0);
}

static void ResetBagItemListRedrawFlag(void)
{
    sBagItemListRedrawState.isPending = 0;
}

static bool32 ShouldDelayBagItemListPrinter(void)
{
    bool32 retVal;

    if (gLinkOpen == TRUE)
    {
        DelayItemMenuOptionsPrinter();
        DelayBagDescriptionPrinter();
        retVal = TRUE;
    }
    else
    {
        retVal = FALSE;
    }
    return retVal;
}

static void ResumeItemMenuOptionsAfterList(void)
{
    ResumeItemMenuOptionsPrinter();
}

static int GetBagDescriptionPrinterState(void)
{
    int retVal;

    if (gLinkOpen == TRUE)
    {
        if (IsBagItemListPrinterFinished() != 0)
            retVal = 1;
        else
            retVal = 2;
    }
    else
    {
        retVal = 0;
    }
    return retVal;
}

static void RunDeferredBagItemMessageAfterList(void)
{
    RunDeferredBagItemMessage();
}

static int PrepareBagItemMessage(void)
{
    if (gLinkOpen == TRUE && IsBagItemListPrinterFinished() == 0)
    {
        if (HasBagItemListPrinterPassedVisibleRows() != 0)
        {
            StopBagItemListPrinter();
            return 0;
        }
    }
    else
    {
        return 0;
    }
    SetBagItemListPrinterShort();
    return 2;
}

static int PrepareItemMenuOptionsPrinter(void)
{
    if (IsBagItemListPrinterFinished() == 0)
        return 2;
    DelayBagDescriptionPrinter();
    return 1;
}

static void ResumeBagDescriptionPrinterAfterOptions(void)
{
    ResumeBagDescriptionPrinter();
}

static void SetBagItemListRedrawFlag(void)
{
    sBagItemListRedrawState.isPending = 1;
}

static int ConsumeBagItemListRedrawFlag(void)
{
    int val = sBagItemListRedrawState.isPending;

    sBagItemListRedrawState.isPending = 0;
    return val;
}

static const u16 sRotatingBallPalette[] = INCBIN_U16("graphics/unknown/83C170C.gbapal");
static const u8 sRotatingBallGfx[] = INCBIN_U8("graphics/interface/bag_spinner.4bpp");

const u8 gSpriteImage_UnusedCherry[] = INCBIN_U8("graphics/unused/cherry.4bpp");
const u16 gSpritePalette_UnusedCherry[] = INCBIN_U16("graphics/unused/cherry.gbapal");

//------------------------------------------------------------------------------
// Bag Sprite
//------------------------------------------------------------------------------

static const struct OamData sBagSpriteOamData =
{
    .y = 0,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 3,
    .tileNum = 0,
    .priority = 2,
    .paletteNum = 7,
    .affineParam = 0,
};

static const union AnimCmd sBagSpriteAnimSeq0[] =
{
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_END,
};

static const union AnimCmd sBagSpriteAnimSeq1[] =
{
    ANIMCMD_FRAME(64, 4),
    ANIMCMD_END,
};

static const union AnimCmd sBagSpriteAnimSeq5[] =
{
    ANIMCMD_FRAME(128, 4),
    ANIMCMD_END,
};

static const union AnimCmd sBagSpriteAnimSeq2[] =
{
    ANIMCMD_FRAME(192, 4),
    ANIMCMD_END,
};

static const union AnimCmd sBagSpriteAnimSeq3[] =
{
    ANIMCMD_FRAME(256, 4),
    ANIMCMD_END,
};

static const union AnimCmd sBagSpriteAnimSeq4[] =
{
    ANIMCMD_FRAME(320, 4),
    ANIMCMD_END,
};

static const union AnimCmd *const sBagSpriteAnimTable[] =
{
    sBagSpriteAnimSeq0,
    sBagSpriteAnimSeq1,
    sBagSpriteAnimSeq2,
    sBagSpriteAnimSeq3,
    sBagSpriteAnimSeq4,
    sBagSpriteAnimSeq5,
};

static const union AffineAnimCmd sBagSpriteAffineAnimSeq[] =
{
    AFFINEANIMCMD_FRAME(0, 0, -2, 2),
    AFFINEANIMCMD_FRAME(0, 0, 2, 4),
    AFFINEANIMCMD_FRAME(0, 0, -2, 4),
    AFFINEANIMCMD_FRAME(0, 0, 2, 2),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd *const sBagSpriteAffineAnimTable[] =
{
    sBagSpriteAffineAnimSeq,
};

static const struct CompressedSpriteSheet sMaleBagSpriteSheet = {gBagMaleTiles, 0x3000, TAG_BAG_GFX};
static const struct CompressedSpriteSheet sFemaleBagSpriteSheet = {gBagFemaleTiles, 0x3000, TAG_BAG_GFX};
static const struct CompressedSpritePalette sBagSpritePalette = {gBagPalette, TAG_BAG_GFX};

static void SpriteCB_InitBagVisual(struct Sprite *sprite)
{
    sprite->animNum = 0;
    sprite->data[0] = 0;
    sprite->data[1] = 0;
    sprite->data[2] = 0;
    sprite->data[3] = 0;
    sprite->data[4] = 0;
    sprite->callback = SpriteCB_BagVisual;
}

static void SpriteCB_BagVisual(struct Sprite *sprite)
{
    if (sBagSpriteTargetPocket != -1)
        SpriteCB_BagVisualSwitchingPockets(sprite);
    if (sBagSpriteShakeRequested != 0)
        SpriteCB_ShakeBagSprite(sprite);
}

static void SpriteCB_BagVisualSwitchingPockets(struct Sprite *sprite)
{
    switch (sprite->data[3])
    {
    case 0:
        if (sBagSpriteTargetPocket != 6)
        {
            sprite->animBeginning = TRUE;
            sprite->animNum = sBagSpriteTargetPocket;
            sBagSpriteTargetPocket = -1;
        }
        else
        {
            sprite->animBeginning = TRUE;
            sprite->animNum = 0;
            sprite->y -= 4;
            sprite->data[0] = 4;
            sprite->data[3] = 1;
            ResetBagSpriteShake(sprite);
        }
        break;
    case 1:
        if (sprite->data[0] != 0)
        {
            if (sprite->data[1] != 0)
            {
                sprite->y++;
                sprite->data[0]--;
            }
            sprite->data[1] = (sprite->data[1] + 1) & 1;
        }
        else
        {
            sBagSpriteTargetPocket = -1;
            sprite->data[1] = 0;
            sprite->data[3] = 0;
        }
        break;
    }
}

static void SpriteCB_ShakeBagSprite(struct Sprite *sprite)
{
    switch (sprite->data[4])
    {
    case 0:
        sprite->oam.affineMode = 1;
        sprite->affineAnims = sBagSpriteAffineAnimTable;
        InitSpriteAffineAnim(sprite);
        sprite->data[4] = 1;
        break;
    case 1:
        sprite->data[2]++;
        if (sprite->data[2] == 12)
            ResetBagSpriteShake(sprite);
        break;
    }
}

static void ResetBagSpriteShake(struct Sprite *sprite)
{
    sBagSpriteShakeRequested = 0;
    sprite->oam.affineMode = 0;
    sprite->data[2] = 0;
    sprite->data[4] = 0;
    FreeOamMatrix(sprite->oam.matrixNum);
}

static const struct SpriteTemplate sBagSpriteTemplate =
{
    .tileTag = TAG_BAG_GFX,
    .paletteTag = TAG_BAG_GFX,
    .oam = &sBagSpriteOamData,
    .anims = sBagSpriteAnimTable,
    .images = NULL,
    .affineAnims = sBagSpriteAffineAnimTable,
    .callback = SpriteCB_InitBagVisual,
};

static void CreateBagSprite(void)
{
    CreateSprite(&sBagSpriteTemplate, 58, 40, 0);
}

//------------------------------------------------------------------------------
// Pokeball Sprite
//------------------------------------------------------------------------------

static const struct OamData sRotatingBallOamData =
{
    .y = 0,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 4,
    .size = 1,
    .tileNum = 0,
    .priority = 2,
    .paletteNum = 8,
    .affineParam = 0,
};

static const union AffineAnimCmd sRotatingBallAffineAnim_Clockwise[] =
{
    AFFINEANIMCMD_FRAME(0, 0, 8, 32),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sRotatingBallAffineAnim_CounterClockwise[] =
{
    AFFINEANIMCMD_FRAME(0, 0, -8, 32),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd *const sRotatingBallAffineAnimTable[] =
{
    sRotatingBallAffineAnim_Clockwise,
    sRotatingBallAffineAnim_CounterClockwise,
};

static const struct SpriteFrameImage sRotatingBallSpriteImageTable[] =
{
    {sRotatingBallGfx, sizeof(sRotatingBallGfx)},
};

static const struct SpritePalette sRotatingBallSpritePalette = {sRotatingBallPalette, TAG_BAG_POCKET_SWITCH_BALL};

static void SpriteCB_SwitchPocketRotatingBallInit(struct Sprite *sprite)
{
    sprite->data[3] = 0;
    sprite->data[0] = 0;
    SpriteCB_SwitchPocketRotatingBallContinue(sprite);
    sprite->callback = SpriteCB_SwitchPocketRotatingBallContinue;
}

static void UpdateSwitchPocketRotatingBallCoords(struct Sprite *sprite)
{
    sprite->centerToCornerVecX = sprite->data[5] - ((sprite->data[0] + 1) & 1);
    sprite->centerToCornerVecY = sprite->data[6] - ((sprite->data[0] + 1) & 1);
}

static void SpriteCB_SwitchPocketRotatingBallContinue(struct Sprite *sprite)
{
    if (sprite->data[7] != 0)
    {
        switch (sprite->data[3])
        {
        case 0:
            sprite->oam.affineMode = 1;
            if (sprite->data[7] == 1)
                sprite->affineAnims = sRotatingBallAffineAnimTable;
            else
                sprite->affineAnims = sRotatingBallAffineAnimTable + 1;
            InitSpriteAffineAnim(sprite);
            sprite->data[3] = 1;
            sprite->data[5] = sprite->centerToCornerVecX;
            sprite->data[6] = sprite->centerToCornerVecY;
            UpdateSwitchPocketRotatingBallCoords(sprite);
            break;
        case 1:
            sprite->data[0]++;
            UpdateSwitchPocketRotatingBallCoords(sprite);
            if (sprite->data[0] == 32)
            {
                sprite->data[0] = 0;
                sprite->data[3] = 0;
                sprite->centerToCornerVecX = sprite->data[5];
                sprite->centerToCornerVecY = sprite->data[6];
                FreeOamMatrix(sprite->oam.matrixNum);
                sprite->oam.affineMode = 0;
                sprite->callback = SpriteCallbackDummy;
            }
            break;
        }
    }
}

static const struct SpriteTemplate sRotatingBallSpriteTemplate =
{
    .tileTag = 0xFFFF,
    .paletteTag = TAG_BAG_POCKET_SWITCH_BALL,
    .oam = &sRotatingBallOamData,
    .anims = sBagSpriteAnimTable,
    .images = sRotatingBallSpriteImageTable,
    .affineAnims = sRotatingBallAffineAnimTable,
    .callback = SpriteCB_SwitchPocketRotatingBallInit,
};

static void CreateBagPokeballSprite(u8 a)
{
    LoadSpritePalette(&sRotatingBallSpritePalette);
    sSwitchPocketBallSpriteId = CreateSprite(&sRotatingBallSpriteTemplate, 16, 88, 0);
    gSprites[sSwitchPocketBallSpriteId].data[7] = a;
}

static void RemoveSwitchPocketRotatingBallSprite(void)
{
    FreeSpritePaletteByTag(TAG_BAG_POCKET_SWITCH_BALL);
    FreeOamMatrix(gSprites[sSwitchPocketBallSpriteId].oam.matrixNum);
    DestroySprite(&gSprites[sSwitchPocketBallSpriteId]);
}

static const struct OamData sBerrySpriteOamData =
{
    .y = 0,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 3,
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 7,
    .affineParam = 0,
};

static const struct OamData sBerryPicRotatingOamData =
{
    .y = 0,
    .affineMode = 3,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 3,
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 7,
    .affineParam = 0,
};

static const union AnimCmd sBerrySpriteAnimSeq[] =
{
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END,
};

static const union AnimCmd *const sBerrySpriteAnimTable[] =
{
    sBerrySpriteAnimSeq,
};

static const struct SpriteFrameImage sBerrySpriteImageTable[] =
{
    {ewramBerryPic, 0x800},
};

static void SpriteCB_BerryTag(struct Sprite *);
static const struct SpriteTemplate sBerrySpriteTemplate =
{
    .tileTag = 0xFFFF,
    .paletteTag = TAG_BERRY_PIC,
    .oam = &sBerrySpriteOamData,
    .anims = sBerrySpriteAnimTable,
    .images = sBerrySpriteImageTable,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_BerryTag,
};

static const union AffineAnimCmd sBerryPicAffineAnim_Rotation1[] =
{
    AFFINEANIMCMD_FRAME(-1, -1, -3, 96),
    AFFINEANIMCMD_FRAME(0, 0, 0, 16),
    AFFINEANIMCMD_FRAME(-2, -2, -1, 64),
    AFFINEANIMCMD_FRAME(-8, 0, 0, 16),
    AFFINEANIMCMD_FRAME(0, -8, 0, 16),
    AFFINEANIMCMD_FRAME(256, 256, 0, 0),
    AFFINEANIMCMD_JUMP(0),
};

static const union AffineAnimCmd sBerryPicAffineAnim_Rotation2[] =
{
    AFFINEANIMCMD_FRAME(-1, -1, 3, 96),
    AFFINEANIMCMD_FRAME(0, 0, 0, 16),
    AFFINEANIMCMD_FRAME(-2, -2, 1, 64),
    AFFINEANIMCMD_FRAME(-8, 0, 0, 16),
    AFFINEANIMCMD_FRAME(0, -8, 0, 16),
    AFFINEANIMCMD_FRAME(256, 256, 0, 0),
    AFFINEANIMCMD_JUMP(0),
};

static const union AffineAnimCmd *const sBerryPicRotatingAffineAnimTable[] =
{
    sBerryPicAffineAnim_Rotation1,
    sBerryPicAffineAnim_Rotation2,
};

static const struct SpriteTemplate sBerryPicRotatingSpriteTemplate =
{
    .tileTag = 0xFFFF,
    .paletteTag = TAG_BERRY_PIC,
    .oam = &sBerryPicRotatingOamData,
    .anims = sBerrySpriteAnimTable,
    .images = sBerrySpriteImageTable,
    .affineAnims = sBerryPicRotatingAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

static const struct {const u8 *lzPic; const u8 *lzPalette;} sBerryPicTable[] =
{
    {gBerryPic_Cheri,  gBerryPalette_Cheri},
    {gBerryPic_Chesto, gBerryPalette_Chesto},
    {gBerryPic_Pecha,  gBerryPalette_Pecha},
    {gBerryPic_Rawst,  gBerryPalette_Rawst},
    {gBerryPic_Aspear, gBerryPalette_Aspear},
    {gBerryPic_Leppa,  gBerryPalette_Leppa},
    {gBerryPic_Oran,   gBerryPalette_Oran},
    {gBerryPic_Persim, gBerryPalette_Persim},
    {gBerryPic_Lum,    gBerryPalette_Lum},
    {gBerryPic_Sitrus, gBerryPalette_Sitrus},
    {gBerryPic_Figy,   gBerryPalette_Figy},
    {gBerryPic_Wiki,   gBerryPalette_Wiki},
    {gBerryPic_Mago,   gBerryPalette_Mago},
    {gBerryPic_Aguav,  gBerryPalette_Aguav},
    {gBerryPic_Iapapa, gBerryPalette_Iapapa},
    {gBerryPic_Razz,   gBerryPalette_Razz},
    {gBerryPic_Bluk,   gBerryPalette_Bluk},
    {gBerryPic_Nanab,  gBerryPalette_Nanab},
    {gBerryPic_Wepear, gBerryPalette_Wepear},
    {gBerryPic_Pinap,  gBerryPalette_Pinap},
    {gBerryPic_Pomeg,  gBerryPalette_Pomeg},
    {gBerryPic_Kelpsy, gBerryPalette_Kelpsy},
    {gBerryPic_Qualot, gBerryPalette_Qualot},
    {gBerryPic_Hondew, gBerryPalette_Hondew},
    {gBerryPic_Grepa,  gBerryPalette_Grepa},
    {gBerryPic_Tamato, gBerryPalette_Tamato},
    {gBerryPic_Cornn,  gBerryPalette_Cornn},
    {gBerryPic_Magost, gBerryPalette_Magost},
    {gBerryPic_Rabuta, gBerryPalette_Rabuta},
    {gBerryPic_Nomel,  gBerryPalette_Nomel},
    {gBerryPic_Spelon, gBerryPalette_Spelon},
    {gBerryPic_Pamtre, gBerryPalette_Pamtre},
    {gBerryPic_Watmel, gBerryPalette_Watmel},
    {gBerryPic_Durin,  gBerryPalette_Durin},
    {gBerryPic_Belue,  gBerryPalette_Belue},
    {gBerryPic_Liechi, gBerryPalette_Liechi},
    {gBerryPic_Ganlon, gBerryPalette_Ganlon},
    {gBerryPic_Salac,  gBerryPalette_Salac},
    {gBerryPic_Petaya, gBerryPalette_Petaya},
    {gBerryPic_Apicot, gBerryPalette_Apicot},
    {gBerryPic_Lansat, gBerryPalette_Lansat},
    {gBerryPic_Starf,  gBerryPalette_Starf},
    {gBerryPic_Enigma, gBerryPalette_Enigma},
};

static void ArrangeBerryGfx(const u8 *src, u8 *dst)
{
    u8 i;
    u8 j;

    memset(dst, 0, 0x800);
    dst += 0x100;
    for (i = 0; i < 6; i++)
    {
        dst += 32;
        for (j = 0; j < 6; j++)
        {
            memcpy(dst, src, 32);
            dst += 32;
            src += 32;
        }
        if (i != 5)
            dst += 32;
    }
}

static void LoadBerryGfx(u8 berryId)
{
    struct SpritePalette spritePal;

    if (berryId == 0x2A && IsEnigmaBerryValid() == TRUE)
    {
        ArrangeBerryGfx(gSaveBlock1.enigmaBerry.pic, ewramBerryPic);
        spritePal.data = gSaveBlock1.enigmaBerry.palette;
        spritePal.tag = TAG_BERRY_PIC;
        LoadSpritePalette(&spritePal);
    }
    else
    {
        spritePal.data = (u16 *)sBerryPicTable[berryId].lzPalette;
        spritePal.tag = TAG_BERRY_PIC;
        LoadCompressedSpritePalette((struct CompressedSpritePalette *)&spritePal);
        LZDecompressWram(sBerryPicTable[berryId].lzPic, ewramBerryPicTemp);
        ArrangeBerryGfx(ewramBerryPicTemp, ewramBerryPic);
    }
}

u8 CreateBerryTagSprite(u8 berryId, s16 x, s16 y)
{
    LoadBerryGfx(berryId);
    return CreateSprite(&sBerrySpriteTemplate, x, y, 0);
}

static void SpriteCB_BerryTag(struct Sprite *sprite)
{
    sprite->y2 = -gBattle_BG1_Y;
}

void FreeBerryTagSpritePalettes(void)
{
    FreeSpritePaletteByTag(TAG_BERRY_PIC);
    FreeSpritePaletteByTag(TAG_BAG_POCKET_SWITCH_BALL);
}

u8 CreateSpinningBerrySprite(u8 berryId, u8 x, u8 y, bool8 animate)
{
    u8 spriteId;

    FreeSpritePaletteByTag(TAG_BERRY_PIC);
    LoadBerryGfx(berryId);
    spriteId = CreateSprite(&sBerryPicRotatingSpriteTemplate, x, y, 0);
    if (animate == TRUE)
        StartSpriteAffineAnim(&gSprites[spriteId], 1);
    return spriteId;
}

const struct CompressedSpriteSheet gBerryCheckCircleSpriteSheet = {gBerryCheckCircle_Gfx, 2048, TAG_BERRY_CHECK_CIRCLE};

const struct CompressedSpritePalette gBerryCheckCirclePaletteTable = {gBerryCheck_Pal, TAG_BERRY_CHECK_CIRCLE};

static const struct OamData sBerryCheckCircleOamData =
{
    .y = 0,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 2,
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sBerryCheckCircleSpriteAnim[] =
{
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END,
};

static const union AnimCmd *const sBerryCheckCircleSpriteAnimTable[] =
{
    sBerryCheckCircleSpriteAnim,
};

static const struct SpriteTemplate sBerryCheckCircleSpriteTemplate =
{
    .tileTag = TAG_BERRY_CHECK_CIRCLE,
    .paletteTag = TAG_BERRY_CHECK_CIRCLE,
    .oam = &sBerryCheckCircleOamData,
    .anims = sBerryCheckCircleSpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_BerryTag,
};

u8 CreateBerryFlavorCircleSprite(s16 x)
{
    return CreateSprite(&sBerryCheckCircleSpriteTemplate, x, 99, 0);
}
