#include "global.h"
#include "constants/rgb.h"
#include "battle.h"
#include "battle_controllers.h"
#include "event_scripts.h"
#include "item_menu_icons.h"
#include "berry.h"
#include "berry_tag_screen.h"
#include "data.h"
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
#include "field_screen_effect.h"
#include "shop.h"
#include "field_weather.h"
#include "pokemon_storage_system.h"
#include "event_object_lock.h"
struct UnknownStruct2
{
    u8 unk0;
    u8 textLine;
    u8 unk2;
};

struct UnknownStruct3
{
    u8 unk0;
    u8 unk1;
    u8 unk2;
};

struct UnknownStruct4
{
    u8 unk0;
    u8 unk1;
    u16 unk2;
    const u8 *unk4;
    TaskFunc unk8;
};

struct UnknownStruct5
{
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
};

struct UnknownStruct6
{
    u8 unk0;
};

enum
{
    ACTION_USE,      // 0
    ACTION_TOSS,       // 1
    ACTION_CANCEL,   // 2
    ACTION_REGISTER,   // 3
    ACTION_BATTLE_USE,      // 4
    ITEM_ACTION_CANCEL_5,   // 5
    ACTION_GIVE,       // 6
    ACTION_CHECK_TAG,  // 7
    ACTION_DUMMY,       // 8
    ACTION_CONFIRM,    // 9
};

// ewram
EWRAM_DATA static struct UnknownStruct2 gUnknown_0203853C = {0};
EWRAM_DATA static struct UnknownStruct3 gUnknown_02038540 = {0};
EWRAM_DATA static struct UnknownStruct4 gUnknown_02038544 = {0};
EWRAM_DATA static struct UnknownStruct5 gUnknown_02038550 = {0};
EWRAM_DATA static struct UnknownStruct6 gUnknown_02038554 = {0};  // There are 3 bytes of padding after this, so I assume it's a struct
EWRAM_DATA static u8 gUnknown_02038558 = 0;
EWRAM_DATA static s8 sCurrentBagPocket = 0;
EWRAM_DATA static u8 gUnknown_0203855A = 0;
EWRAM_DATA static s8 gUnknown_0203855B = 0;
EWRAM_DATA static s8 gUnknown_0203855C = 0;
EWRAM_DATA u16 gSpecialVar_ItemId = 0;
EWRAM_DATA u8 gCurSelectedItemSlotIndex = 0;
EWRAM_DATA u8 gPokemonItemUseType = 0;
EWRAM_DATA static u8 gUnknown_02038562 = 0;
EWRAM_DATA static u8 gUnknown_02038563 = 0;
EWRAM_DATA static u8 gUnknown_02038564 = 0;
EWRAM_DATA static u8 sPokeballSpriteId ALIGNED(4) = 0;  // HACK: why is there a space before this variable?

// bss
static u8 sPopupMenuSelection;
static u8 sReturnLocation;
static const u8 *sPopupMenuActionList;

// common
void (*gFieldItemUseCallback)(u8) = NULL;
struct PocketScrollState gBagPocketScrollStates[5];
struct ItemSlot *gCurrentBagPocketItemSlots;  // selected pocket item slots


const struct BagPocket gBagPockets[POCKETS_COUNT] =
{
    {gSaveBlock1.bagPocket_Items, 20},
    {gSaveBlock1.bagPocket_PokeBalls, 16},
    {gSaveBlock1.bagPocket_TMHM, 64},
    {gSaveBlock1.bagPocket_Berries, 46},
    {gSaveBlock1.bagPocket_KeyItems, 20},
};

static void ItemMenu_UseOutOfBattle(u8);
static void ItemMenu_Toss(u8);
static void ItemMenu_Cancel(u8);
static void ItemMenu_Register(u8);
static void ItemMenu_UseInBattle(u8);
static void sub_80A7124(u8);
static void ItemMenu_Give(u8);
static void ItemMenu_CheckTag(u8);
static void HandlePopupMenuAction_Confirm(u8);

static const struct MenuAction2 sItemMenuActions[] =
{
    {gMenuText_Use,                   ItemMenu_UseOutOfBattle},
    {gMenuText_Toss,                  ItemMenu_Toss},
    {gText_Cancel2,   ItemMenu_Cancel},
    {gMenuText_Register,              ItemMenu_Register},
    {gMenuText_Use,                   ItemMenu_UseInBattle},
    {gText_Cancel2,   sub_80A7124},
    {gMenuText_Give,                 ItemMenu_Give},
    {gMenuText_CheckTag,              ItemMenu_CheckTag},
    {gOtherText_CancelWithTerminator, NULL},
    {gMenuText_Confirm,               HandlePopupMenuAction_Confirm},
};

static const u8 sItemPopupMenuChoicesTable[][6] =
{
    [ITEMS_POCKET] =
    {
        ACTION_USE,
        ACTION_TOSS,
        ACTION_GIVE,
        ACTION_CANCEL,
    },

    [BALLS_POCKET] =
    {
        ACTION_GIVE,
        ACTION_TOSS,
        ACTION_DUMMY,
        ACTION_CANCEL,
    },

    [TMHM_POCKET] =
    {
        ACTION_USE,
        ACTION_DUMMY,
        ACTION_GIVE,
        ACTION_CANCEL,
    },

    [BERRIES_POCKET] =
    {
        ACTION_CHECK_TAG,
        ACTION_USE,
        ACTION_TOSS,
        ACTION_DUMMY,
        ACTION_GIVE,
        ACTION_CANCEL
    },

    [KEYITEMS_POCKET] =
    {
        ACTION_USE,
        ACTION_DUMMY,
        ACTION_REGISTER,
        ACTION_CANCEL,
    },
};

const u8 gUnknown_083C16AE[][2] =
{
    {6, 2},
    {6, 2},
    {6, 2},
    {6, 2},
    {2, 0},
};

const u8 gUnknown_083C16B8[] = {7, 9, 8, 2};

static void Task_ItemContext_Normal(u8);
static void OnBagClose_Field0(u8);
static void OnBagClose_PkmnList(u8);
static void Task_ItemContext_GiveToParty(u8);
static void OnBagClose_Shop(u8);
static void Task_ItemContext_Sell(u8);
static void OnBagClose_Field4(u8);
static void OnItemSelect_Field4(u8);
static void OnBagClose_Field5(u8);
static void OnBagClose_PC(u8);
static void Task_ItemContext_Deposit(u8);
static void OnItemSelect_Battle(u8);
static void OnBagClose_Battle(u8);

static const struct {TaskFunc onItemSelect; TaskFunc onBagClose;} sItemSelectFuncs[] =
{
    [ITEMMENULOCATION_FIELD]   = {Task_ItemContext_Normal, OnBagClose_Field0},
    [ITEMMENULOCATION_BATTLE]    = {OnItemSelect_Battle, OnBagClose_Battle},
    [ITEMMENULOCATION_PARTY] = {Task_ItemContext_GiveToParty, OnBagClose_PkmnList},
    [ITEMMENULOCATION_SHOP]      = {Task_ItemContext_Sell, OnBagClose_Shop},
    [ITEMMENULOCATION_BERRY_TREE]   = {OnItemSelect_Field4, OnBagClose_Field4},
    [RETURN_TO_FIELD_5]   = {Task_ItemContext_Normal, OnBagClose_Field5},
    [ITEMMENULOCATION_ITEMPC]        = {Task_ItemContext_Deposit, OnBagClose_PC},
};

static void ConfirmToss(u8);
static void CancelToss(u8);

static const struct YesNoFuncTable sYesNoTossFunctions = {ConfirmToss, CancelToss};

static void ConfirmSell(u8);
static void CancelSell(u8);

static const struct YesNoFuncTable sYesNoSellItemFunctions = {ConfirmSell, CancelSell};

static void BagMenu_InitBGs(void);
static bool8 LoadBagMenu_Graphics(void);
static void sub_80A362C(void);
static void sub_80A3740(void);
static void sub_80A396C(u16 *, u8, u8, u8);
static void sub_80A39B8(u16 *, u8);
static void sub_80A39E4(u16 *, u8, u8, s8);
static void DrawPocketIndicatorDots(u16 *, u8);
static void SortBerriesOrTMHMs(struct BagPocket);
static void UpdatePocketItemLists(void);
static void sub_80A3D24(u8);
static void sub_80A3D40(void);
static void RemoveSelectIconFromRegisteredItem(void);
static void sub_80A48E8(u16, int, int);
static void ItemListMenu_InitDescription(s16);
static void ItemListMenu_ChangeDescription(s16, int);
static void sub_80A4F68(u8);
static void Task_BagMenu_HandleInput(u8);
static void sub_80A5AE4(u8);
static void PrintThereIsNoPokemon(u8);
static void Task_CallItemUseOnFieldCallback(u8);
static void sub_80A6444(u8);
static void InitSellHowManyInput(u8);
static void Task_ChooseHowManyToSell(u8);
static void BuyMenuPrintItemQuantityAndPrice(u8);
static void BuyMenuDisplayMessage(u16, u16);
static void sub_80A683C(void);
static void sub_80A6870(u16, u8);
static void sub_80A699C(void);
static void Task_WallyTutorialBagMenu(u8);
static void ItemListMenu_InitMenu(void);
static void sub_80A73C0(void);
static void sub_80A73F0(void);
static void sub_80A73FC(void);
static void sub_80A740C(void);
static void sub_80A7420(void);
static void sub_80A751C(void);
static void sub_80A7528(u8);
static void sub_80A7590(void);
static void sub_80A75E4(void);
static void sub_80A7630(void);
static void sub_80A763C(void);
static void sub_80A76A0(void);
static void sub_80A770C(void);
static void DisplayCannotUseItemMessage(int, const u8 *, TaskFunc, int);
static void sub_80A7768(void);
static void sub_80A7828(void);
static void sub_80A7834(int, int);
static bool32 sub_80A78A0(void);
static void sub_80A78B8(void);
static bool32 sub_80A78C4(void);
static void sub_80A78E8(void);
static int sub_80A78F4(void);
static void sub_80A7918(void);
static int sub_80A7924(void);
static int sub_80A7958(void);
static void sub_80A7970(void);
static void sub_80A797C(void);
static int sub_80A7988(void);
static void sub_80A79B4(struct Sprite *);
static void sub_80A79EC(struct Sprite *);
static void sub_80A7A94(struct Sprite *);
static void sub_80A7AE4(struct Sprite *);
static void CreateBagSprite(void);
static void SpriteCB_SwitchPocketRotatingBallContinue(struct Sprite *);
static void CreateBagPokeballSprite(u8);
static void sub_80A7C64(void);

static void CB2_BagMenuRun(void)
{
    AnimateSprites();
    BuildOamBuffer();
    RunTasks();
    sub_80A740C();
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

static bool8 SetupBagMenu(void)
{
    u32 index;
    u16 savedIme;

    switch (gMain.state)
    {
    case 0:
        SetVBlankHBlankCallbacksToNull();
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
        if (LoadBagMenu_Graphics() == FALSE)
            break;
        gMain.state++;
        break;
    case 7:
        Text_LoadWindowTemplate(&gWindowTemplate_81E6DFC);
        gMain.state++;
        break;
    case 8:
        MultistepInitMenuWindowBegin(&gWindowTemplate_81E6DFC);
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
        CreateVerticalScrollIndicators(SCROLL_ARROW_UP, 172, 12);
        CreateVerticalScrollIndicators(SCROLL_ARROW_DOWN, 172, 148);
        CreateVerticalScrollIndicators(SCROLL_ARROW_LEFT, 28, 88);
        CreateVerticalScrollIndicators(SCROLL_ARROW_RIGHT, 100, 88);
        SetVerticalScrollIndicatorPriority(SCROLL_ARROW_UP, 2);
        SetVerticalScrollIndicatorPriority(SCROLL_ARROW_DOWN, 2);
        SetVerticalScrollIndicatorPriority(SCROLL_ARROW_LEFT, 2);
        SetVerticalScrollIndicatorPriority(SCROLL_ARROW_RIGHT, 2);
        if (sReturnLocation == ITEMMENULOCATION_BERRY_TREE || sReturnLocation == RETURN_TO_FIELD_5)
        {
            SetVerticalScrollIndicators(SCROLL_ARROW_LEFT, INVISIBLE);
            SetVerticalScrollIndicators(SCROLL_ARROW_RIGHT, INVISIBLE);
        }
        gMain.state++;
        break;
    case 11:
        gUnknown_0203855A = 16;
        sub_80A39B8(gBGTilemapBuffers[2], sCurrentBagPocket + 1);
        DrawPocketIndicatorDots(gBGTilemapBuffers[2], sCurrentBagPocket);
        UpdatePocketItemLists();
        SortBerriesOrTMHMs(gBagPockets[2]);
        SortBerriesOrTMHMs(gBagPockets[3]);
        sub_80A3D40();
        gCurrentBagPocketItemSlots = gBagPockets[sCurrentBagPocket].itemSlots;
        sub_80A362C();
        gMain.state++;
        break;
    case 12:
        sub_80A48E8(0xFFFF, 0, 7);
        index = gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + gBagPocketScrollStates[sCurrentBagPocket].cursorPosition;
        ItemListMenu_InitDescription(gCurrentBagPocketItemSlots[index].itemId);
        ItemListMenu_InitMenu();
        gUnknown_0203855B = sCurrentBagPocket + 1;
        gUnknown_0203855C = 0;
        gMain.state++;
        break;
    case 13:
        CreateBagSprite();
        CreateBagPokeballSprite(0);
        sub_80A3740();
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
        sub_80A751C();
        sub_80A7630();
        sub_80A770C();
        sub_80A7828();
        sub_80A78B8();
#if DEBUG
        if (gLinkOpen == TRUE)
            debug_sub_8008218((void *)(VRAM + 0xF5E0), 0x8000, (void *)(VRAM + 0xF800), 0);
#endif
        return TRUE;
    }
    return FALSE;
}

static bool8 sub_80A34B4(void)
{
    while (1)
    {
        if (SetupBagMenu() == TRUE)
        {
            sPopupMenuSelection = 0;
            ResetTasks();
            return TRUE;
        }
        if (MenuHelpers_IsLinkActive() == TRUE)
            break;
    }
    return FALSE;
}


static void BagMenu_InitBGs(void)
{
    ResetVramOamAndBgCntRegs();
    REG_BG2CNT = BGCNT_PRIORITY(2) | BGCNT_CHARBASE(1) | BGCNT_SCREENBASE(12) | BGCNT_16COLOR | BGCNT_TXT256x256;
    REG_BG1CNT = BGCNT_PRIORITY(1) | BGCNT_CHARBASE(1) | BGCNT_SCREENBASE(4)  | BGCNT_16COLOR | BGCNT_TXT256x256;
    REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG1_ON | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
    REG_BLDCNT = 0;
}

static bool8 LoadBagMenu_Graphics(void)
{
    switch (ewramBagSetupStep)
    {
    case 0:
        LZDecompressVram(gBagScreen_Gfx, (void *)(VRAM + 0x4000));
        ewramBagSetupStep++;
        break;
    case 1:
        CpuCopy16(gBagScreen_GfxTileMap, gBGTilemapBuffers[2], 0x800);
        ewramBagSetupStep++;
        break;
    case 2:
        if (gSaveBlock2.playerGender == MALE || sReturnLocation == ITEMMENULOCATION_WALLY)
            LoadCompressedPalette(gBagScreenMale_Pal, 0, 64);
        else
            LoadCompressedPalette(gBagScreenFemale_Pal, 0, 64);
        ewramBagSetupStep++;
        break;
    case 3:
        if (gSaveBlock2.playerGender == MALE || sReturnLocation == ITEMMENULOCATION_WALLY)
            LoadCompressedSpriteSheet(&gBagMaleSpriteSheet);
        else
            LoadCompressedSpriteSheet(&gBagFemaleSpriteSheet);
        ewramBagSetupStep++;
        break;
    case 4:
        LoadCompressedSpritePalette(&gBagPaletteTable);
        ewramBagSetupStep = 0;
        return TRUE;
    }
    return FALSE;
}

static void sub_80A362C(void)
{
    u8 i;

    for (i = 0; i < 5; i++)
    {
        u8 r3;

        if (sReturnLocation == RETURN_TO_FIELD_5)
            r3 = gBagPocketScrollStates[i].numSlots - 1;
        else
            r3 = gBagPocketScrollStates[i].numSlots;

        if (gBagPocketScrollStates[i].scrollPosition != 0)
        {
            if (gBagPocketScrollStates[i].scrollPosition + gBagPocketScrollStates[i].cursorMax > r3)
                gBagPocketScrollStates[i].scrollPosition = r3 - gBagPocketScrollStates[i].cursorMax;
        }
        else
        {
            if (gBagPocketScrollStates[i].cursorPosition > r3)
                gBagPocketScrollStates[i].cursorPosition = r3;
        }
    }
}

void ResetBagScrollPositions(void)
{
    u16 i;

    for (i = 0; i < POCKETS_COUNT; i++)
    {
        gBagPocketScrollStates[i].cursorPosition = 0;
        gBagPocketScrollStates[i].scrollPosition = 0;
        gBagPocketScrollStates[i].numSlots = 0;
        gBagPocketScrollStates[i].cursorMax = 0;
    }
    sCurrentBagPocket = 0;
}

static void sub_80A36B8(u16 *a, u8 b, u8 c, u8 d, u8 e)
{
    u16 i;
    u16 j;

    for (i = c; i <= c + e; i++)
    {
        for (j = b; j <= b + d; j++)
        {
            u32 index = j + i * 32;

            a[index] = 0;
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

const struct ItemSlot gUnknown_Debug_083EBC68[60] =
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
    for (i = 0; i < ARRAY_COUNT(gUnknown_Debug_083EBC68); i++)
    {
        u16 pocket;

        if (gUnknown_Debug_083EBC68[i].itemId == ITEM_NONE)
            break;
        pocket = GetItemPocket(gUnknown_Debug_083EBC68[i].itemId) - 1;
        gBagPockets[pocket].itemSlots[arr[pocket]] = gUnknown_Debug_083EBC68[i];
        arr[pocket]++;
    }
}

#endif

static void sub_80A3740(void)
{
    const u16 colors[2] = {RGB(14, 15, 16), RGB_WHITE};

    LoadPalette(&colors[1], 0xD1, sizeof(colors[1]));
    LoadPalette(&colors[0], 0xD8, sizeof(colors[0]));
}

static void sub_80A3770(void)
{
    if (sReturnLocation == ITEMMENULOCATION_FIELD)
    {
        sPopupMenuActionList = sItemPopupMenuChoicesTable[sCurrentBagPocket];
        if (sCurrentBagPocket != BERRIES_POCKET)
            gUnknown_02038564 = 4;
        else
            gUnknown_02038564 = 6;
    }
}

static void ReturnToItemList(u8 taskId)
{
    gTasks[taskId].func = Task_BagMenu_HandleInput;
    StartVerticalScrollIndicators(SCROLL_ARROW_UP);
    StartVerticalScrollIndicators(SCROLL_ARROW_DOWN);
    StartVerticalScrollIndicators(SCROLL_ARROW_LEFT);
    StartVerticalScrollIndicators(SCROLL_ARROW_RIGHT);
}

static void Task_SwitchBagPocket(u8 taskId)
{
    u8 r5 = gUnknown_0203855A;

    if (r5 < 16)
    {
        gUnknown_0203855A++;
        sub_80A396C(gBGTilemapBuffers[2], r5, gUnknown_0203855A, 0x4F);
        sub_80A39E4(gBGTilemapBuffers[2], sCurrentBagPocket + 1, gUnknown_0203855A / 2, gTasks[taskId].data[5]);
        gUnknown_02038558 = 1;
        sub_80A48E8(taskId, r5 / 2, gUnknown_0203855A / 2 - 1);
        gUnknown_02038558 = 0;
        return;
    }

    if (gLinkOpen == TRUE)
    {
        u32 index = gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + gBagPocketScrollStates[sCurrentBagPocket].cursorPosition;

        ItemListMenu_ChangeDescription(gCurrentBagPocketItemSlots[index].itemId, gUnknown_02038562);
        gUnknown_02038562++;
    }
    else
    {
        while (gUnknown_02038562 < 3)
        {
            u32 index = gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + gBagPocketScrollStates[sCurrentBagPocket].cursorPosition;

            ItemListMenu_ChangeDescription(gCurrentBagPocketItemSlots[index].itemId, gUnknown_02038562);
            gUnknown_02038562++;
        }
    }
    if (gUnknown_02038562 == 3)
    {
        gUnknown_0203855A = 16;
        gUnknown_0203855B = sCurrentBagPocket + 1;
        ReturnToItemList(FindTaskIdByFunc(sub_80A4F68));
        DestroyTask(taskId);
        ItemListMenu_InitMenu();
        SetVerticalScrollIndicators(SCROLL_ARROW_LEFT, VISIBLE);
        SetVerticalScrollIndicators(SCROLL_ARROW_RIGHT, VISIBLE);
    }
}

static void sub_80A3954(u16 *a)
{
    CpuCopy16(gBagScreen_GfxTileMap, a, 0x800);
}

static void sub_80A396C(u16 *a, u8 b, u8 c, u8 d)
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

static void sub_80A39B8(u16 *a, u8 b)
{
    u8 var = b * 2;

    sub_809D104((void *)a, 4, 10, gBagScreenLabels_Tilemap, 0, var, 8, 2);
}

static void sub_80A39E4(u16 *a, u8 b, u8 c, s8 d)
{
    u16 r2 = b * 2;
    u16 r7;

    if (d == -1)
    {
        r7 = (b + 1) * 2;
        if (b == 5)
            r7 = 2;

        sub_809D104((void *)a, 4, 10, gBagScreenLabels_Tilemap, 8 - c, r2, c, 2);
        sub_809D104((void *)a, c + 4, 10, gBagScreenLabels_Tilemap, 0, r7, 8 - c, 2);
    }
    else if (d == 1)
    {
        r7 = (b - 1) * 2;
        if (b == 1)
            r7 = 10;

        sub_809D104((void *)a, 4, 10, gBagScreenLabels_Tilemap, c, r7, 8 - c, 2);
        sub_809D104((void *)a, 12 - c, 10, gBagScreenLabels_Tilemap, 0, r2, c, 2);
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

    sub_80A76A0();
    sub_80A7590();
    gCurrentBagPocketItemSlots = gBagPockets[sCurrentBagPocket].itemSlots;
    sub_80A3D24(sCurrentBagPocket);
    gUnknown_0203855A = 0;
    sub_80A3954(tileMapBuffer);
    sub_80A396C(tileMapBuffer, 0, 16, 3);
    sub_80A39B8(tileMapBuffer, 0);
    gUnknown_0203855B = 6;
    gUnknown_02038562 = 0;
    taskId = FindTaskIdByFunc(Task_SwitchBagPocket);
    if (taskId == 0xFF)
        taskId = CreateTask(Task_SwitchBagPocket, 8);
    gTasks[taskId].data[5] = delta;
}

static void SwapItemSlots(struct ItemSlot *a, struct ItemSlot *b)
{
    struct ItemSlot temp = *a;

    *a = *b;
    *b = temp;
}

static void CompactItemsInBagPocket(struct BagPocket bagPocket)
{
    u16 i;
    u16 j;

    for (i = 0; i < bagPocket.capacity - 1; i++)
    {
        for (j = i + 1; j < bagPocket.capacity; j++)
        {
            if (bagPocket.itemSlots[i].quantity == 0)
                SwapItemSlots(&bagPocket.itemSlots[i], &bagPocket.itemSlots[j]);
        }
    }
}

static void SortBerriesOrTMHMs(struct BagPocket bagPocket)
{
    u16 i;
    u16 j;

    for (i = 0; i < bagPocket.capacity; i++)
    {
        for (j = i + 1; j < bagPocket.capacity; j++)
        {
            if (bagPocket.itemSlots[i].quantity != 0 && bagPocket.itemSlots[j].quantity != 0
             && bagPocket.itemSlots[i].itemId > bagPocket.itemSlots[j].itemId)
                SwapItemSlots(&bagPocket.itemSlots[i], &bagPocket.itemSlots[j]);
        }
    }
}

static void UpdatePocketItemList(u8 pocketId)
{
    u16 i;

    gBagPocketScrollStates[pocketId].numSlots = 0;
    for (i = 0; i < gBagPockets[pocketId].capacity; i++)
    {
        if (gBagPockets[pocketId].itemSlots[i].quantity != 0)
            gBagPocketScrollStates[pocketId].numSlots++;
    }
    CompactItemsInBagPocket(gBagPockets[pocketId]);
}

static void UpdatePocketItemLists(void)
{
    u8 i;

    for (i = 0; i < POCKETS_COUNT; i++)
        UpdatePocketItemList(i);
}

static void sub_80A3D24(u8 pocketNum)
{
    if (gBagPocketScrollStates[pocketNum].numSlots >= 7)
        gBagPocketScrollStates[pocketNum].cursorMax = 7;
    else
        gBagPocketScrollStates[pocketNum].cursorMax = gBagPocketScrollStates[pocketNum].numSlots;
}

static void sub_80A3D40(void)
{
    u8 i;

    for (i = 0; i < POCKETS_COUNT; i++)
        sub_80A3D24(i);
}

static void sub_80A3D5C(u8 taskId)
{
    u32 index = gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + gBagPocketScrollStates[sCurrentBagPocket].cursorPosition;

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
        if (gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + 7 == gBagPocketScrollStates[sCurrentBagPocket].numSlots
         && gBagPocketScrollStates[sCurrentBagPocket].scrollPosition != 0)
            gBagPocketScrollStates[sCurrentBagPocket].scrollPosition--;
        UpdatePocketItemList(sCurrentBagPocket);
    }
    sub_80A3D24(sCurrentBagPocket);
}

void sub_80A3E0C(void)
{
    u32 index = gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + gBagPocketScrollStates[sCurrentBagPocket].cursorPosition;

    if (gCurrentBagPocketItemSlots[index].quantity == 0)
    {
        gCurrentBagPocketItemSlots[index].itemId = 0;
        if (gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + 7 == gBagPocketScrollStates[sCurrentBagPocket].numSlots
         && gBagPocketScrollStates[sCurrentBagPocket].scrollPosition != 0)
            gBagPocketScrollStates[sCurrentBagPocket].scrollPosition--;
        UpdatePocketItemList(sCurrentBagPocket);
    }
    sub_80A3D24(sCurrentBagPocket);
}

static void nullsub_16(void)
{
}

static void sub_80A3E70(u8 a, u8 b)
{
    struct ItemSlot temp = gCurrentBagPocketItemSlots[a];

    gCurrentBagPocketItemSlots[a] = gCurrentBagPocketItemSlots[b];
    gCurrentBagPocketItemSlots[b] = temp;
}

static void DoItemSwap(u8 taskId)
{
    PlaySE(SE_SELECT);
    nullsub_16();
    sub_80A3E70(
      gTasks[taskId].data[10] - 1,
      gBagPocketScrollStates[sCurrentBagPocket].cursorPosition + gBagPocketScrollStates[sCurrentBagPocket].scrollPosition);
    gTasks[taskId].data[10] = 0;
    sub_80A763C();
    sub_80A7528(0);
    ItemListMenu_InitMenu();
}

static void CancelItemSwap(u8 taskId)
{
    u8 r2;

    PlaySE(SE_SELECT);
    nullsub_16();
    r2 = gTasks[taskId].data[10] - gBagPocketScrollStates[sCurrentBagPocket].scrollPosition - 1;
    gTasks[taskId].data[10] = 0;
    if (r2 < 8)
        sub_80A48E8(taskId, r2, r2);
    sub_80A7528(0);
}

static void StartItemSwap(u8 taskId)
{
    gTasks[taskId].data[10] = gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + gBagPocketScrollStates[sCurrentBagPocket].cursorPosition + 1;
    sub_80A48E8(taskId, gBagPocketScrollStates[sCurrentBagPocket].cursorPosition, gBagPocketScrollStates[sCurrentBagPocket].cursorPosition);
    sub_80A7528(1);
}

void sub_80A3FA0(u16 *a, u8 b, u8 c, u8 d, u8 e, u16 f)
{
    s16 i;
    s16 j;

    for (i = c; i < c + e; i++)
    {
        for (j = b; j < b + d; j++)
        {
            u32 index = j + i * 32;

            a[index] = f;
        }
    }
}

static void sub_80A4008(u16 *a, u8 b, u8 c, u8 d, u8 e)
{
    sub_80A3FA0(a, b, c, d, e, 1);
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
        if (gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + i == gBagPocketScrollStates[sCurrentBagPocket].numSlots)
            break;
        if (gCurrentBagPocketItemSlots[gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + i].itemId == gSaveBlock1.registeredItem)
        {
            EraseSelectIcon(i);
            break;
        }
    }
}

static void AddSelectIconToRegisteredItem(void)
{
    RemoveSelectIconFromRegisteredItem();
    MoveSelectIcon(gBagPocketScrollStates[sCurrentBagPocket].cursorPosition);
}

void sub_80A4164(u8 *dest, u16 value, enum StringConvertMode mode, u8 digits)
{
    *dest++ = CHAR_MULT_SIGN;
    dest[0] = EXT_CTRL_CODE_BEGIN;
    dest[1] = 0x14;
    dest[2] = 6;
    dest += 3;
    ConvertIntToDecimalStringN(dest, value, mode, digits);
}

void sub_80A418C(u16 value, enum StringConvertMode mode, u8 c, u8 d, u8 digits)
{
    sub_80A4164(gStringVar1, value, mode, digits);
    Menu_PrintText(gStringVar1, c, d);
}

static void sub_80A41D4(u8 taskId)
{
    sub_80A763C();
}

static void sub_80A41E0(u8 *a, u16 b, const u8 *c, u16 d, u8 e)
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

static u8 *sub_80A425C(u8 taskId, u8 *text, u8 itemSlot)
{
    if (gTasks[taskId].data[10] - gBagPocketScrollStates[sCurrentBagPocket].scrollPosition - 1 == itemSlot)
    {
        text[0] = EXT_CTRL_CODE_BEGIN;
        text[1] = 1;
        text[2] = TEXT_COLOR_RED;
        text += 3;
    }
    return text;
}

static bool8 sub_80A42B0(u8 itemPos, int b)
{
    u8 r5;
    u16 *ptr;
    struct UnknownStruct3 *r8 = &gUnknown_02038540;

    if (gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + itemPos > gBagPocketScrollStates[sCurrentBagPocket].numSlots)
        return TRUE;
    if (gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + itemPos == gBagPocketScrollStates[sCurrentBagPocket].numSlots)
    {
        if (sReturnLocation == RETURN_TO_FIELD_5)
            return TRUE;
        r5 = itemPos * 2 + 2;
        AlignStringInMenuWindow(gStringVar1, gText_CloseBag, 0x78, 0);
        Menu_PrintText(gStringVar1, 14, r5);
        ptr = gBGTilemapBuffers[2] + 14 + r5 * 32;
        ptr[0] = 0x4F;
        ptr[1] = 0x4F;
        ptr[32] = 0x4F;
        ptr[33] = 0x4F;
        if (itemPos == 7)
            return TRUE;
        if ((b == 1 && r8->unk2 != 0) || b == 2)
            Menu_BlankWindowRect(14, r5 + 2, 29, 13);
        else
            Menu_BlankWindowRect(14, r5 + 2, 29, 17);
        return TRUE;
    }
    return FALSE;
}

static void sub_80A4380(u16 a, int b, int c, int d)
{
    u8 i;

    for (i = b; i <= c; i++)
    {
        u8 r4;
        u8 r5;
        u8 *text;

        if (sub_80A42B0(i, d) == TRUE)
            break;
        r4 = gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + i;
        r5 = i * 2 + 2;
        text = gStringVar1;
        text = sub_80A425C(a, text, i);
        text = AlignStringInMenuWindow(text, GetItemName(gCurrentBagPocketItemSlots[r4].itemId), 0x66, 0);
        *text++ = CHAR_MULT_SIGN;
        AlignInt1InMenuWindow(text, gCurrentBagPocketItemSlots[r4].quantity, 0x78, 1);
        Menu_PrintText(gStringVar1, 14, r5);
    }
}

static void sub_80A444C(u16 a, int b, int c, int d)
{
    u8 i;

    for (i = b; i <= c; i++)
    {
        u8 slot;
        u8 r5;
        u8 *text;

        if (sub_80A42B0(i, d) == TRUE)
            break;
        slot = gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + i;
        r5 = i * 2 + 2;
        text = gStringVar1;
        text = sub_80A425C(a, text, i);
#if ENGLISH
        AlignStringInMenuWindow(text, GetItemName(gCurrentBagPocketItemSlots[slot].itemId), 0x60, 0);
#else
        AlignStringInMenuWindow(text, GetItemName(gCurrentBagPocketItemSlots[slot].itemId), 0x63, 0);
#endif
        Menu_PrintText(gStringVar1, 14, r5);
        if (gUnknown_02038558)
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

static void sub_80A4548(u16 taskId, int topItemOffset, int bottomItemOffset, int d)
{
    u8 i;

    for (i = topItemOffset; i <= bottomItemOffset; i++)
    {
        u8 slot;
        u8 y;
        u16 tilemapOffset;
        u8 *text;

        if (sub_80A42B0(i, d) == TRUE)
            break;

        slot = gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + i;
        y = i * 2 + 2;
        tilemapOffset = y * 32 + 14;
        text = gStringVar1;
        text = sub_80A425C(taskId, text, i);

        if (gCurrentBagPocketItemSlots[slot].itemId < ITEM_HM_CUT)
        {
            const u8 *moveName;
            gBGTilemapBuffers[2][tilemapOffset + 0] = 0x59;
            gBGTilemapBuffers[2][tilemapOffset + 1] = 0x4F;
            gBGTilemapBuffers[2][tilemapOffset + 32] = 0x69;
            gBGTilemapBuffers[2][tilemapOffset + 33] = 0x4F;
            moveName = gMoveNames[ItemIdToBattleMoveId(gCurrentBagPocketItemSlots[slot].itemId)];
            sub_80A41E0(text, gCurrentBagPocketItemSlots[slot].itemId - (ITEM_TM_FOCUS_PUNCH - 1), moveName, gCurrentBagPocketItemSlots[slot].quantity, 2);
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
            text = ConvertIntToDecimalStringSimple(text, gCurrentBagPocketItemSlots[slot].itemId - (ITEM_HM_CUT - 1));
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

static void sub_80A46FC(u16 taskId, int b, int c, int d)
{
    u8 i;

    for (i = b; i <= c; i++)
    {
        u8 r4;
        u8 r5;
        u8 *text;
        register int var asm("r0");

        if (sub_80A42B0(i, d) == TRUE)
            break;
        r4 = gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + i;
        r5 = i * 2 + 2;

        var = 14 + r5 * 32;
        gBGTilemapBuffers[2][var] = 0x59;
        var += 32;
        gBGTilemapBuffers[2][var] = 0x69;

        text = gStringVar1;
        text = sub_80A425C(taskId, text, i);
        CopyItemName(gCurrentBagPocketItemSlots[r4].itemId, gStringVar2);
        sub_80A41E0(text, gCurrentBagPocketItemSlots[r4].itemId - 0x84, gStringVar2, gCurrentBagPocketItemSlots[r4].quantity, 3);
        Menu_PrintText(gStringVar1, 14, r5);
    }
}

static void sub_80A47E8(u16 taskId, int topItemOffset, int bottomItemOffset, int d)
{
    switch (sCurrentBagPocket)
    {
    case ITEMS_POCKET:
    case BALLS_POCKET:
        sub_80A4380(taskId, topItemOffset, bottomItemOffset, d);
        break;
    case KEYITEMS_POCKET:
        sub_80A444C(taskId, topItemOffset, bottomItemOffset, d);
        break;
    case TMHM_POCKET:
        sub_80A4548(taskId, topItemOffset, bottomItemOffset, d);
        break;
    case BERRIES_POCKET:
        sub_80A46FC(taskId, topItemOffset, bottomItemOffset, d);
        break;
    }
    if (gBagPocketScrollStates[sCurrentBagPocket].scrollPosition != 0)
        SetVerticalScrollIndicators(SCROLL_ARROW_UP, VISIBLE);
    else
        SetVerticalScrollIndicators(SCROLL_ARROW_UP, INVISIBLE);
    if ((sReturnLocation != RETURN_TO_FIELD_5 && gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + 8 < gBagPocketScrollStates[sCurrentBagPocket].numSlots + 1)
     || (sReturnLocation == RETURN_TO_FIELD_5 && gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + 8 < gBagPocketScrollStates[sCurrentBagPocket].numSlots))
        SetVerticalScrollIndicators(SCROLL_ARROW_DOWN, VISIBLE);
    else
        SetVerticalScrollIndicators(SCROLL_ARROW_DOWN, INVISIBLE);
}

static void sub_80A48E8(u16 taskId, int topItemOffset, int bottomItemOffset)
{
    sub_80A47E8(taskId, topItemOffset, bottomItemOffset, 0);
}

static void sub_80A48F8(u16 taskId)
{
    sub_80A47E8(taskId, 0, 5, 2);
}

static void ItemListMenu_InitDescription(s16 itemId)
{
    u8 r5;

    if (gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + gBagPocketScrollStates[sCurrentBagPocket].cursorPosition == gBagPocketScrollStates[sCurrentBagPocket].numSlots)
    {
        r5 = sub_8072A18(gOtherText_ReturnTo, 4, 0x68, 0x68, 1);
        r5 += sub_8072A18(gBagMenu_ReturnToStrings[sReturnLocation], 4, 0x78, 0x68, 1);
    }
    else
    {
        r5 = sub_8072A18(GetItemDescription(itemId), 4, 0x68, 0x68, 1);
    }

    if (r5 < 3)
        Menu_EraseWindowRect(0, 13 + r5 * 2, 13, 20);
}

static void ItemListMenu_ChangeDescription(s16 itemId, int b)
{
    u8 description[100];

    if (gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + gBagPocketScrollStates[sCurrentBagPocket].cursorPosition == gBagPocketScrollStates[sCurrentBagPocket].numSlots)
    {
        if (b == 0)
        {
            Menu_EraseWindowRect(0, 13, 13, 20);
            Menu_PrintTextPixelCoords(gOtherText_ReturnTo, 4, 0x68, 0);
        }
        else if (b == 1)
        {
            Menu_PrintTextPixelCoords(gBagMenu_ReturnToStrings[sReturnLocation], 4, 0x78, 0);
        }
    }
    else
    {
        if (b == 0)
            Menu_EraseWindowRect(0, 13, 13, 20);
        if (ItemId_CopyDescription(description, itemId, b))
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

static void sub_80A4A98(const u8 *text, u32 line)
{
    u8 buffer[100];

    if (line == 0)
        Menu_EraseWindowRect(0, 13, 13, 20);
    if (CopyTextLine(buffer, text, line))
        Menu_PrintTextPixelCoords(buffer, 4, 104 + line * 16, 0);
}

static void sub_80A4ADC(u8 taskId)
{
    if (gTasks[taskId].data[10] == 0)
        sub_80A7528(0);
    PlaySE(SE_SELECT);
    gUnknown_0203855C = 1;
}

static void sub_80A4B14(s8 a, u8 b)
{
    gBagPocketScrollStates[sCurrentBagPocket].scrollPosition += a;
    Menu_MoveCursor(0);
    sub_80A73C0();
    sub_80A763C();
    sub_80A4ADC(b);
}

static void sub_80A4B58(s8 delta, u8 b)
{
    gBagPocketScrollStates[sCurrentBagPocket].cursorPosition = Menu_MoveCursor(delta);
    sub_80A73C0();
    sub_80A4ADC(b);
}

static const u8 *sub_80A4B90(u16 itemId)
{
    if (TestPlayerAvatarFlags(6))
    {
        if (itemId == ITEM_MACH_BIKE || itemId == ITEM_ACRO_BIKE)
            return gMenuText_Walk;
    }
    if (ItemIsMail(itemId) == TRUE)
        return gMenuText_Check;
    return sItemMenuActions[sPopupMenuActionList[0]].text;
}

static void sub_80A4BF0(u16 *a)
{
    u8 i;

    if (gUnknown_02038564 == 4)
    {
        Menu_DrawStdWindowFrame(0, 7, 13, 12);
        sub_80A4008(a, 1, 8, 12, 4);
        if (MenuHelpers_IsLinkActive() == TRUE && sReturnLocation == RETURN_TO_FIELD_5)
        {
            sub_80A7834(1, 0);
        }
        else
        {
            for (i = 0; i < gUnknown_02038564; i++)
            {
                const u8 *text;

                if (i == 0)
                    text = sub_80A4B90(gSpecialVar_ItemId);
                else
                    text = sItemMenuActions[sPopupMenuActionList[i]].text;
                Menu_PrintText(text, 1 + (i / 2) * 6, 8 + (i % 2) * 2);
            }
        }
        if (sReturnLocation == RETURN_TO_FIELD_5)
            InitMenu(0, 1, 8, gUnknown_02038564, sPopupMenuSelection, 1);
        else
            InitMenu(0, 1, 8, gUnknown_02038564, 0, 1);
        sub_8072DCC(0x2F);
    }
    else
    {
        Menu_DrawStdWindowFrame(0, 5, 13, 12);
        sub_80A4008(a, 1, 6, 12, 6);
        for (i = 0; i < gUnknown_02038564; i++)
            Menu_PrintText(sItemMenuActions[sPopupMenuActionList[i]].text, 1 + (i / 3) * 6, 6 + (i % 3) * 2);
        InitMenu(0, 1, 6, gUnknown_02038564, 0, 1);
        sub_8072DCC(0x2F);
    }
    sub_80A7528(2);
}

static void sub_80A4DA4(u16 *a)
{
    sub_80A73FC();
    sub_80A36B8(a, 0, 6, 13, 6);
    Menu_EraseWindowRect(0, 5, 13, 12);
    sub_80A7590();
}

static void sub_80A4DD8(u8 taskId, u8 b, u8 c, u8 d, u8 e, u8 digits)
{
    gTasks[taskId].data[1] = 1;
    gTasks[taskId].data[2] = b + 2;
    gTasks[taskId].data[3] = c + 1;
    Menu_DrawStdWindowFrame(b, c, b + d, c + e);
    sub_80A4008(gBGTilemapBuffers[1], b + 1, c + 1, d - 1, e - 1);
    sub_80A418C(1, 1, b + 2, c + 1, digits);
}

static void sub_80A4E8C(s8 delta, u8 b)
{
    PlaySE(SE_SELECT);
    SetVerticalScrollIndicators(SCROLL_ARROW_UP, INVISIBLE);
    SetVerticalScrollIndicators(SCROLL_ARROW_DOWN, INVISIBLE);
    SetVerticalScrollIndicators(SCROLL_ARROW_LEFT, INVISIBLE);
    SetVerticalScrollIndicators(SCROLL_ARROW_RIGHT, INVISIBLE);
    PauseVerticalScrollIndicator(SCROLL_ARROW_UP);
    PauseVerticalScrollIndicator(SCROLL_ARROW_DOWN);
    PauseVerticalScrollIndicator(SCROLL_ARROW_LEFT);
    PauseVerticalScrollIndicator(SCROLL_ARROW_RIGHT);
    ChangePocket(gBGTilemapBuffers[2], delta);
    DrawPocketIndicatorDots(gBGTilemapBuffers[2], sCurrentBagPocket);
    sub_80A3770();
    sub_80A7C64();
    CreateBagPokeballSprite(b);
}

static bool8 sub_80A4F0C(void)
{
    if (sReturnLocation == RETURN_TO_FIELD_5 || sReturnLocation == ITEMMENULOCATION_BERRY_TREE)
        return FALSE;
    if (gMain.newKeys == DPAD_RIGHT || GetLRKeysPressed() == 2)
    {
        sub_80A4E8C(1, 2);
        return TRUE;
    }
    if (gMain.newKeys == DPAD_LEFT || GetLRKeysPressed() == 1)
    {
        sub_80A4E8C(-1, 1);
        return TRUE;
    }
    return FALSE;
}

static void sub_80A4F68(u8 taskId)
{
    sub_80A4F0C();
}

static bool8 sub_80A4F74(u8 a)
{
    bool8 retVal = FALSE;

    if (JOY_REPEAT(DPAD_ANY) == DPAD_UP)
    {
        if (gBagPocketScrollStates[sCurrentBagPocket].cursorPosition != 0)
            sub_80A4B58(-1, a);
        else if (gBagPocketScrollStates[sCurrentBagPocket].scrollPosition != 0)
            sub_80A4B14(-1, a);
        if (gLinkOpen == TRUE && gMain.keyRepeatCounter == 5)
            gMain.keyRepeatCounter = 11;
        retVal = TRUE;
    }
    else if (JOY_REPEAT(DPAD_ANY) == DPAD_DOWN)
    {
        if (sReturnLocation != RETURN_TO_FIELD_5)
        {
            if (gBagPocketScrollStates[sCurrentBagPocket].cursorPosition == gBagPocketScrollStates[sCurrentBagPocket].cursorMax)
            {
                if (gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + gBagPocketScrollStates[sCurrentBagPocket].cursorPosition != gBagPocketScrollStates[sCurrentBagPocket].numSlots)
                    sub_80A4B14(1, a);
            }
            else
            {
                sub_80A4B58(1, a);
            }
        }
        else
        {
            if (gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + gBagPocketScrollStates[sCurrentBagPocket].cursorPosition != gBagPocketScrollStates[sCurrentBagPocket].numSlots - 1)
            {
                if (gBagPocketScrollStates[sCurrentBagPocket].cursorPosition != gBagPocketScrollStates[sCurrentBagPocket].cursorMax)
                    sub_80A4B58(1, a);
                else
                    sub_80A4B14(1, a);
            }
        }
        retVal = TRUE;
        if (gLinkOpen == TRUE && gMain.keyRepeatCounter == 5)
            gMain.keyRepeatCounter = 11;
    }
    else
    {
        if (gTasks[a].data[10] == 0 && sub_80A4F0C() == TRUE)
        {
            sub_80A73F0();
            gTasks[a].func = sub_80A4F68;
            retVal = TRUE;
        }
        else
        {
            sub_80A73C0();
        }
    }
    return retVal;
}

static void Task_BagMenu_HandleInput(u8 taskId)
{
    s16 *taskData = gTasks[taskId].data;

    if (!gPaletteFade.active)
    {
        if (sub_80A4F74(taskId) == TRUE)
        {
            sub_808B5B4(taskId);
            return;
        }

        if (JOY_NEW(SELECT_BUTTON)
         && !(sCurrentBagPocket == BERRIES_POCKET || sCurrentBagPocket == TMHM_POCKET)
         && (sReturnLocation == ITEMMENULOCATION_FIELD || sReturnLocation == ITEMMENULOCATION_BATTLE))
        {
            if (taskData[10] == 0)
            {
                if (gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + gBagPocketScrollStates[sCurrentBagPocket].cursorPosition != gBagPocketScrollStates[sCurrentBagPocket].numSlots)
                {
                    PlaySE(SE_SELECT);
                    StartItemSwap(taskId);
                }
                sub_808B5B4(taskId);
            }
            else
            {
                if (gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + gBagPocketScrollStates[sCurrentBagPocket].cursorPosition != gBagPocketScrollStates[sCurrentBagPocket].numSlots)
                {
                    PlaySE(SE_SELECT);
                    DoItemSwap(taskId);
                }
                else
                {
                    CancelItemSwap(taskId);
                }
                sub_808B5B4(taskId);
            }
            return;
        }

        if (JOY_NEW(A_BUTTON))
        {
            if (gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + gBagPocketScrollStates[sCurrentBagPocket].cursorPosition == gBagPocketScrollStates[sCurrentBagPocket].numSlots)
            {
                if (taskData[10] == 0)
                {
                    gSpecialVar_ItemId = 0;
                    sItemSelectFuncs[sReturnLocation].onBagClose(taskId);
                }
                else
                {
                    CancelItemSwap(taskId);
                }
                sub_808B5B4(taskId);
            }
            else
            {
                if (taskData[10] == 0)
                {
                    PlaySE(SE_SELECT);
                    gCurSelectedItemSlotIndex = gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + gBagPocketScrollStates[sCurrentBagPocket].cursorPosition;
                    gSpecialVar_ItemId = gCurrentBagPocketItemSlots[gCurSelectedItemSlotIndex].itemId;
                    sItemSelectFuncs[sReturnLocation].onItemSelect(taskId);
                    StopVerticalScrollIndicators(SCROLL_ARROW_UP);
                    StopVerticalScrollIndicators(SCROLL_ARROW_DOWN);
                    StopVerticalScrollIndicators(SCROLL_ARROW_LEFT);
                    StopVerticalScrollIndicators(SCROLL_ARROW_RIGHT);
                    sub_80A797C();
                }
                else
                {
                    DoItemSwap(taskId);
                }
                sub_808B5B4(taskId);
            }
            return;
        }

        if (JOY_NEW(B_BUTTON))
        {
            if (taskData[10] == 0)
            {
                if (sReturnLocation != RETURN_TO_FIELD_5)
                {
                    gSpecialVar_ItemId = 0;
                    sItemSelectFuncs[sReturnLocation].onBagClose(taskId);
                }
            }
            else
            {
                CancelItemSwap(taskId);
            }
            sub_808B5B4(taskId);
            return;
        }
    }
}

bool8 SellMenu_QuantityRoller(u8 taskId, u16 max)
{
    s16 *taskData = gTasks[taskId].data;

    if (JOY_REPEAT(DPAD_ANY) == DPAD_UP)
    {
        if (taskData[1] != max)
            taskData[1]++;
        else
            taskData[1] = 1;
        return TRUE;
    }

    if (JOY_REPEAT(DPAD_ANY) == DPAD_DOWN)
    {
        if (taskData[1] != 1)
            taskData[1]--;
        else
            taskData[1] = max;
        return TRUE;
    }

    if (JOY_REPEAT(DPAD_ANY) == DPAD_RIGHT)
    {
        if (taskData[1] + 10 < max)
            taskData[1] += 10;
        else
            taskData[1] = max;
        return TRUE;
    }

    if (JOY_REPEAT(DPAD_ANY) == DPAD_LEFT)
    {
        if (taskData[1] > 10)
            taskData[1] -= 10;
        else
            taskData[1] = 1;
        return TRUE;
    }

    return FALSE;
}

static bool8 sub_80A5350(u8 taskId)
{
    s16 *taskData = gTasks[taskId].data;

    if (SellMenu_QuantityRoller(taskId, gCurrentBagPocketItemSlots[gCurSelectedItemSlotIndex].quantity) == TRUE)
    {
        // if (sCurrentBagPocket == BAG_POCKET_BERRIES)  Can't get it to match this way
        if (sCurrentBagPocket + 1 == BERRIES_POCKET + 1)
            sub_80A418C(taskData[1], 1, taskData[2], taskData[3], 3);
        else
            sub_80A418C(taskData[1], 1, taskData[2], taskData[3], 2);
        return TRUE;
    }
    return FALSE;
}

static void sub_80A53CC(void)
{
    if (sub_80A34B4() == TRUE)
    {
        sub_80A3770();
        gUnknown_02038563 = CreateTask(Task_BagMenu_HandleInput, 0);
    }
}

void CB2_BagMenuFromStartMenu(void)
{
    sReturnLocation = ITEMMENULOCATION_FIELD;
    SetMainCallback2(sub_80A53CC);
}

static void sub_80A5414(u8 taskId)
{
    TaskFunc r5 = NULL;
    if (sub_80A78A0())
    {
        while (1)
        {
            if (JOY_REPEAT(DPAD_ANY) == DPAD_UP)
            {
                if ((sPopupMenuSelection & 1) && sPopupMenuActionList[sPopupMenuSelection - 1] != 8)
                {
                    PlaySE(SE_SELECT);
                    sPopupMenuSelection = MoveMenuCursor3(-1);
                }
                break;
            }
            if (JOY_REPEAT(DPAD_ANY) == DPAD_DOWN)
            {
                if ((!(sPopupMenuSelection & 1)) && sPopupMenuActionList[sPopupMenuSelection + 1] != 8)
                {
                    PlaySE(SE_SELECT);
                    sPopupMenuSelection = MoveMenuCursor3(1);
                }
                break;
            }
            if (JOY_REPEAT(DPAD_ANY) == DPAD_LEFT)
            {
                if ((sPopupMenuSelection >= 2) && sPopupMenuActionList[sPopupMenuSelection - 2] != 8)
                {
                    PlaySE(SE_SELECT);
                    sPopupMenuSelection = MoveMenuCursor3(-2);
                }
                break;
            }
            if (JOY_REPEAT(DPAD_ANY) == DPAD_RIGHT)
            {
                if ((sPopupMenuSelection < 2) && sPopupMenuActionList[sPopupMenuSelection + 2] != 8)
                {
                    PlaySE(SE_SELECT);
                    sPopupMenuSelection = MoveMenuCursor3(2);
                }
                break;
            }
            if (JOY_NEW(A_BUTTON))
            {
                gTasks[taskId].data[10] = 0;
                sub_80A4DA4(gBGTilemapBuffers[1]);
                r5 = sItemMenuActions[sPopupMenuActionList[sPopupMenuSelection]].func;
                r5(taskId);
                break;
            }
            if (JOY_NEW(B_BUTTON))
            {
                gTasks[taskId].data[10] = 0;
                sub_80A48E8(taskId, gBagPocketScrollStates[sCurrentBagPocket].cursorPosition, gBagPocketScrollStates[sCurrentBagPocket].cursorPosition);
                sub_80A4DA4(gBGTilemapBuffers[1]);
                r5 = sItemMenuActions[sPopupMenuActionList[3]].func;
                r5(taskId);
                break;
            }
            break;
        }
    }
    if (r5 == NULL)
    {
        if (sReturnLocation == RETURN_TO_FIELD_5)
        {
            if (sPopupMenuSelection == 0)
                sub_8072DDC(12);
            else if (sPopupMenuSelection < 2)
                sub_8072DCC(47);
            else
                sub_8072DCC(48);
        }
        else
        {
            if (sPopupMenuSelection < 2)
                sub_8072DCC(47);
            else
                sub_8072DCC(48);
        }
    }
}

static void sub_80A5600(u8 taskId)
{
    TaskFunc r5 = NULL;

    while (1)
    {
        if (JOY_REPEAT(DPAD_ANY) == DPAD_UP)
        {
            if (sPopupMenuSelection != 0 && sPopupMenuActionList[sPopupMenuSelection - 1] != 8)
            {
                PlaySE(SE_SELECT);
                sPopupMenuSelection = MoveMenuCursor3(-1);
            }
            break;
        }
        if (JOY_REPEAT(DPAD_ANY) == DPAD_DOWN)
        {
            if (sPopupMenuSelection != gUnknown_02038564 - 1 && sPopupMenuSelection != 2 && sPopupMenuActionList[sPopupMenuSelection + 1] != 8)
            {
                PlaySE(SE_SELECT);
                sPopupMenuSelection = MoveMenuCursor3(1);
            }
            break;
        }
        if (JOY_REPEAT(DPAD_ANY) == DPAD_LEFT)
        {
            if ((sPopupMenuSelection >= 3) && sPopupMenuActionList[sPopupMenuSelection - 3] != 8)
            {
                PlaySE(SE_SELECT);
                sPopupMenuSelection = MoveMenuCursor3(-3);
            }
            break;
        }
        if (JOY_REPEAT(DPAD_ANY) == DPAD_RIGHT)
        {
            if ((sPopupMenuSelection < 3) && sPopupMenuActionList[sPopupMenuSelection + 3] != 8)
            {
                PlaySE(SE_SELECT);
                sPopupMenuSelection = MoveMenuCursor3(3);
            }
            break;
        }
        if (JOY_NEW(A_BUTTON))
        {
            gTasks[taskId].data[10] = 0;
            sub_80A4DA4(gBGTilemapBuffers[1]);
            r5 = sItemMenuActions[sPopupMenuActionList[sPopupMenuSelection]].func;
            r5(taskId);
            break;
        }
        if (JOY_NEW(B_BUTTON))
        {
            gTasks[taskId].data[10] = 0;
            sub_80A48E8(taskId, gBagPocketScrollStates[sCurrentBagPocket].cursorPosition, gBagPocketScrollStates[sCurrentBagPocket].cursorPosition);
            sub_80A4DA4(gBGTilemapBuffers[1]);
            r5 = sItemMenuActions[sPopupMenuActionList[5]].func;
            r5(taskId);
            break;
        }
        break;
    }
    if (r5 == NULL)
    {
        if (sPopupMenuSelection == 0)
        {
            sub_8072DDC(12);
        }
        else if (sPopupMenuSelection < 3)
        {
            sub_8072DCC(47);
        }
        else
        {
            sub_8072DCC(48);
        }
    }
}

static void sub_80A57C4(void)
{
    u8 r5;

    sPopupMenuActionList = gUnknown_083C16AE[sCurrentBagPocket];
    if (sCurrentBagPocket == KEYITEMS_POCKET)
    {
        gUnknown_02038564 = 1;
        r5 = 9;
    }
    else if (IsHoldingItemAllowed(gSpecialVar_ItemId) == 0)
    {
        sPopupMenuActionList = gUnknown_083C16AE[4];
        gUnknown_02038564 = 1;
        r5 = 9;
    }
    else
    {
        gUnknown_02038564 = 2;
        r5 = 7;
    }
    sub_80A4008(gBGTilemapBuffers[1], 7, r5 + 1, 6, gUnknown_02038564 * 2);
    Menu_DrawStdWindowFrame(6, r5, 13, gUnknown_02038564 * 2 + 1 + r5);
    sub_80A7834(0, r5);
    InitMenu(0, 7, r5 + 1, gUnknown_02038564, 0, 6);
}

static void sub_80A5888(u8 taskId)
{
    if (sub_80A78A0() != 0)
    {
        if (JOY_REPEAT(DPAD_UP))
        {
            if (sPopupMenuSelection != 0)
            {
                PlaySE(SE_SELECT);
                sPopupMenuSelection = Menu_MoveCursor(-1);
            }
        }
        else if (JOY_REPEAT(DPAD_DOWN))
        {
            if (sPopupMenuSelection != gUnknown_02038564 - 1)
            {
                PlaySE(SE_SELECT);
                sPopupMenuSelection = Menu_MoveCursor(1);
            }
        }
        else if (JOY_NEW(A_BUTTON))
        {
            gTasks[taskId].data[10] = 0;
            sub_80A48E8(taskId, gBagPocketScrollStates[sCurrentBagPocket].cursorPosition, gBagPocketScrollStates[sCurrentBagPocket].cursorPosition);
            sub_80A4DA4(gBGTilemapBuffers[1]);
            sItemMenuActions[sPopupMenuActionList[sPopupMenuSelection]].func(taskId);
        }
        else if (JOY_NEW(B_BUTTON))
        {
            gTasks[taskId].data[10] = 0;
            sub_80A4DA4(gBGTilemapBuffers[1]);
            sItemMenuActions[2].func(taskId);
        }
    }
}

static void OnBagClose_Field0(u8 taskId)
{
    gTasks[taskId].data[8] = (u32)CB2_ReturnToFieldWithOpenMenu >> 16;
    gTasks[taskId].data[9] = (u32)CB2_ReturnToFieldWithOpenMenu;
    gLastFieldPokeMenuOpened = 0;
    sub_80A5AE4(taskId);
}

static void Task_ItemContext_Normal(u8 taskId)
{
    sPopupMenuSelection = 0;
    if (sReturnLocation == RETURN_TO_FIELD_5)
        sPopupMenuSelection = 1;
    gTasks[taskId].data[10] = gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + gBagPocketScrollStates[sCurrentBagPocket].cursorPosition + 1;
    sub_80A48E8(taskId, gBagPocketScrollStates[sCurrentBagPocket].cursorPosition, gBagPocketScrollStates[sCurrentBagPocket].cursorPosition);
    sub_80A73FC();
    if (MenuHelpers_IsLinkActive() == TRUE && sReturnLocation != RETURN_TO_FIELD_5)
    {
        sub_80A57C4();
        gTasks[taskId].func = sub_80A5888;
    }
    else
    {
        sub_80A4BF0(gBGTilemapBuffers[1]);
        if (sCurrentBagPocket != BERRIES_POCKET || sReturnLocation == RETURN_TO_FIELD_5)
            gTasks[taskId].func = sub_80A5414;
        else
            gTasks[taskId].func = sub_80A5600;
    }
}

static void Task_FadeAndCloseBagMenu(u8 taskId)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = Task_CloseBagMenu;
}

static void sub_80A5AE4(u8 taskId)
{
    PlaySE(SE_SELECT);
    Task_FadeAndCloseBagMenu(taskId);
}

void Task_CloseBagMenu(u8 taskId)
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

void CB2_ReturnToBagMenuPocket(void)
{
    while (1)
    {
        if (SetupBagMenu() == TRUE)
        {
            ResetTasks();
            gUnknown_02038563 = CreateTask(Task_BagMenu_HandleInput, 0);
            break;
        }
        if (MenuHelpers_IsLinkActive() == TRUE)
            break;
    }
}

static void ItemMenu_UseOutOfBattle(u8 taskId)
{
    if (GetItemFieldFunc(gSpecialVar_ItemId) != NULL)
    {
        PlaySE(SE_SELECT);
        if (CalculatePlayerPartyCount() == 0 && GetItemType(gSpecialVar_ItemId) == 1)
        {
            PrintThereIsNoPokemon(taskId);
        }
        else
        {
            gTasks[taskId].data[2] = 0;
            if (sCurrentBagPocket != BERRIES_POCKET)
                GetItemFieldFunc(gSpecialVar_ItemId)(taskId);
            else
                ItemUseOutOfBattle_Berry(taskId);
        }
    }
}

static void PrintThereIsNoPokemon(u8 taskId)
{
    sub_80A73FC();
    sub_80A7590();
    DisplayCannotUseItemMessage(taskId, gText_NoPokemon, CloseItemMessage, 1);
}

static void sub_80A5C24(u8 taskId)
{
    sub_80A7528(0);
    sub_80A41D4(taskId);
    ItemListMenu_InitMenu();
    ReturnToItemList(taskId);
}

void CloseItemMessage(u8 taskId)
{
    sub_80A36B8(gBGTilemapBuffers[1], 0, 0, 31, 31);
    Menu_EraseWindowRect(7, 7, 13, 12);
    Menu_EraseWindowRect(0, 14, 29, 19);
    gTasks[taskId].func = sub_80A5C24;
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

void sub_80A5D04(void)
{
    while (1)
    {
        if (SetupBagMenu() == TRUE)
        {
            gUnknown_02038563 = CreateTask(Task_BagMenu_HandleInput, 0);
            break;
        }
        if (MenuHelpers_IsLinkActive() == TRUE)
            break;
    }
}

static void sub_80A5D38(u8 taskId)
{
    gTasks[taskId].data[1] = 0;
    sub_80A4DA4(gBGTilemapBuffers[1]);
    sub_80A7528(0);
    sub_80A41D4(taskId);
    ItemListMenu_InitMenu();
    ReturnToItemList(taskId);
}

static void sub_80A5D78(void)
{
    sub_80A4008(gBGTilemapBuffers[1], 8, 8, 5, 4);
    DisplayYesNoMenu(7, 7, 1);
}

static void sub_80A5DA0(u16 itemId, u16 quantity)
{
    CopyItemName(itemId, gStringVar1);
    if (quantity >= 100)
        ConvertIntToDecimalStringN(gStringVar2, quantity, STR_CONV_MODE_LEFT_ALIGN, 3);
    else
        ConvertIntToDecimalStringN(gStringVar2, quantity, STR_CONV_MODE_LEFT_ALIGN, 2);
    sub_80A4DA4(gBGTilemapBuffers[1]);
    sub_80A7528(5);
    sub_80A5D78();
}

static void sub_80A5DF8(void)
{
    sub_80A4DA4(gBGTilemapBuffers[1]);
    Menu_EraseWindowRect(7, 6, 11, 13);
    sub_80A7528(4);
}

static void sub_80A5E1C(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        gTasks[taskId].data[10] = 0;
        sub_80A41D4(taskId);
        sub_80A5D38(taskId);
    }
}

static void ConfirmToss(u8 taskId)
{
    sub_80A5DF8();
    sub_80A3D5C(taskId);
    gTasks[taskId].func = sub_80A5E1C;
}

static void CancelToss(u8 taskId)
{
    sub_80A5D38(taskId);
}

static void Task_ChooseHowManyToToss(u8 taskId)
{
    if (sub_80A5350(taskId) == TRUE)
        return;
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        sub_80A5DA0(gSpecialVar_ItemId, gTasks[taskId].data[1]);
        DoYesNoFuncWithChoice(taskId, &sYesNoTossFunctions);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        sub_80A5D38(taskId);
    }
}

static void ItemMenu_Toss(u8 taskId)
{
    PlaySE(SE_SELECT);
    gTasks[taskId].func = Task_ChooseHowManyToToss;
    sub_80A7528(3);
    // if (sCurrentBagPocket == BAG_POCKET_BERRIES)  Can't get it to match this way
    if (sCurrentBagPocket + 1 == BERRIES_POCKET + 1)
        sub_80A4DD8(taskId, 6, 9, 7, 3, 3);
    else
        sub_80A4DD8(taskId, 7, 9, 6, 3, 2);
}

static void ItemMenu_Cancel(u8 taskId)
{
    PlaySE(SE_SELECT);
    sub_80A7528(0);
    sub_80A41D4(taskId);
    ItemListMenu_InitMenu();
    ReturnToItemList(taskId);
}

static void ItemMenu_Register(u8 taskId)
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
    sub_80A7528(0);
    sub_80A41D4(taskId);
    ItemListMenu_InitMenu();
    ReturnToItemList(taskId);
}

static void sub_80A6000(u8 taskId)
{
    sub_80A7528(0);
    sub_80A41D4(taskId);
    ItemListMenu_InitMenu();
    ReturnToItemList(taskId);
}

static void HandleErrorMessage(u8 taskId)
{
    if (JOY_NEW(A_BUTTON))
    {
        sub_80A36B8(gBGTilemapBuffers[1], 0, 0, 31, 31);
        Menu_EraseWindowRect(0, 14, 29, 19);
        gTasks[taskId].func = sub_80A6000;
    }
}

static void PrintItemCantBeHeld(u8 taskId)
{
    sub_80A73FC();
    CopyItemName(gSpecialVar_ItemId, gStringVar1);
    StringExpandPlaceholders(gStringVar4, gText_Var1CantBeHeld);
    sub_80A7590();
    DisplayCannotUseItemMessage(taskId, gStringVar4, HandleErrorMessage, 1);
}

static void ItemMenu_Give(u8 taskId)
{
    PlaySE(SE_SELECT);
    if (IsWritingMailAllowed(gSpecialVar_ItemId) == 0)
    {
        sub_80A73FC();
        sub_80A7590();
        DisplayCannotUseItemMessage(taskId, gText_CantWriteMail, HandleErrorMessage, 1);
    }
    else if (GetItemImportance(gSpecialVar_ItemId) == 0)
    {
        if (CalculatePlayerPartyCount() == 0)
        {
            PrintThereIsNoPokemon(taskId);
        }
        else
        {
            gTasks[taskId].data[8] = (u32)CB2_ShowPartyMenuForItemUse >> 16;
            gTasks[taskId].data[9] = (u32)CB2_ShowPartyMenuForItemUse;
            gTasks[taskId].func = Task_CloseBagMenu;
            gPokemonItemUseType = ITEM_USE_GIVE_ITEM;
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        }
    }
    else
    {
        PrintItemCantBeHeld(taskId);
    }
}

static void ItemMenu_CheckTag(u8 taskId)
{
    sub_80A5AE4(taskId);
    gTasks[taskId].data[8] = (u32)DoBerryTagScreen >> 16;
    gTasks[taskId].data[9] = (u32)DoBerryTagScreen;
}

static void sub_80A61A8(void)
{
    if (sub_80A34B4() == TRUE)
        gUnknown_02038563 = CreateTask(Task_BagMenu_HandleInput, 0);
}

void sub_80A61D0(void)
{
    sReturnLocation = ITEMMENULOCATION_PARTY;
    SetMainCallback2(sub_80A61A8);
}

static void OnBagClose_PkmnList(u8 taskId)
{
    gTasks[taskId].data[8] = (u32)sub_808A3F8 >> 16;
    gTasks[taskId].data[9] = (u32)sub_808A3F8;
    sub_80A5AE4(taskId);
}

static void Task_ItemContext_GiveToParty(u8 taskId)
{
    u8 r6 = sCurrentBagPocket + 1;

    if (IsWritingMailAllowed(gSpecialVar_ItemId) == 0)
    {
        sub_80A73FC();
        sub_80A7590();
        DisplayCannotUseItemMessage(taskId, gText_CantWriteMail, HandleErrorMessage, 1);
    }
    else if (IsHoldingItemAllowed(gSpecialVar_ItemId) == 0)
    {
        sub_80A73FC();
        CopyItemName(gSpecialVar_ItemId, gStringVar1);
        StringExpandPlaceholders(gStringVar4, gText_Var1CantBeHeldHere);
        sub_80A7590();
        DisplayCannotUseItemMessage(taskId, gStringVar4, HandleErrorMessage, 1);
    }
    else if (r6 != 5 && GetItemImportance(gSpecialVar_ItemId) == 0)
    {
        gTasks[taskId].data[8] = (u32)sub_808A3F8 >> 16;
        gTasks[taskId].data[9] = (u32)sub_808A3F8;
        Task_FadeAndCloseBagMenu(taskId);
    }
    else
    {
        PrintItemCantBeHeld(taskId);
    }
}

static void sub_80A62D8(void)
{
    if (sub_80A34B4() == TRUE)
        gUnknown_02038563 = CreateTask(Task_BagMenu_HandleInput, 0);
}

void CB2_GoToSellMenu(void)
{
    sReturnLocation = ITEMMENULOCATION_SHOP;
    SetMainCallback2(sub_80A62D8);
}

static void OnBagClose_Shop(u8 taskId)
{
    gFieldCallback = MapPostLoadHook_ReturnToShopMenu;
    gTasks[taskId].data[8] = (u32)CB2_ReturnToField >> 16;
    gTasks[taskId].data[9] = (u32)CB2_ReturnToField;
    sub_80A5AE4(taskId);
}

static void Task_ItemContext_Sell(u8 taskId)
{
    sub_80A7590();
    gTasks[taskId].data[10] = gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + gBagPocketScrollStates[sCurrentBagPocket].cursorPosition + 1;
    sub_80A48E8(taskId, gBagPocketScrollStates[sCurrentBagPocket].cursorPosition, gBagPocketScrollStates[sCurrentBagPocket].cursorPosition);
    sub_80A73FC();
    CopyItemName(gSpecialVar_ItemId, gStringVar2);
    if (GetItemPrice(gSpecialVar_ItemId) == 0)
    {
        StringExpandPlaceholders(gStringVar4, gText_CantBuyKeyItem);
        DisplayCannotUseItemMessage(taskId, gStringVar4, sub_80A6444, 1);
    }
    else
    {
        StringExpandPlaceholders(gStringVar4, gText_HowManyToSell);
        DisplayCannotUseItemMessage(taskId, gStringVar4, InitSellHowManyInput, 1);
    }
}

static void sub_80A640C(u8 taskId)
{
    gTasks[taskId].data[10] = 0;
    sub_80A7528(0);
    sub_80A41D4(taskId);
    ItemListMenu_InitMenu();
    ReturnToItemList(taskId);
}

static void sub_80A6444(u8 taskId)
{
    sub_80A36B8(gBGTilemapBuffers[1], 0, 0, 31, 31);
    Menu_EraseWindowRect(0, 14, 29, 19);
    gTasks[taskId].func = sub_80A640C;
}

static void InitSellHowManyInput(u8 taskId)
{
    gTasks[taskId].func = Task_ChooseHowManyToSell;
    sub_80A4008(gBGTilemapBuffers[1], 1, 11, 12, 2);
    Menu_DrawStdWindowFrame(0, 10, 13, 13);
    gTasks[taskId].data[1] = 1;
    gTasks[taskId].data[2] = 1;
    gTasks[taskId].data[3] = 11;
    // if (sCurrentBagPocket == BAG_POCKET_BERRIES)  Can't get it to match this way
    if (sCurrentBagPocket + 1 == BERRIES_POCKET + 1)
        sub_80A418C(1, 1, 1, 11, 3);
    else
        sub_80A418C(1, 1, 1, 11, 2);
    BuyMenuDisplayMessage(gSpecialVar_ItemId, 1);
    sub_80A683C();
}

static void WaitAfterItemSell(u8 taskId)
{
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        BuyMenuPrintItemQuantityAndPrice(taskId);
    }
}

static void sub_80A6548(u8 taskId)
{
    sub_80A48F8(taskId);
    gTasks[taskId].func = WaitAfterItemSell;
}

static void SellItem(u8 taskId)
{
    PlaySE(SE_SHOP);
    sub_80A6870(gSpecialVar_ItemId, gTasks[taskId].data[1]);
    gTasks[taskId].func = sub_80A6548;
}

static void ConfirmSell(u8 taskId)
{
    Menu_EraseWindowRect(7, 6, 13, 12);
    sub_80A36B8(gBGTilemapBuffers[1], 7, 6, 6, 6);
    CopyItemName(gSpecialVar_ItemId, gStringVar2);
    StringExpandPlaceholders(gStringVar4, gText_TurnedOverVar1ForVar2);
    DisplayCannotUseItemMessage(taskId, gStringVar4, SellItem, 1);
    sub_80A3D5C(taskId);
}

static void CancelSell(u8 taskId)
{
    Menu_EraseWindowRect(7, 6, 13, 12);
    sub_80A36B8(gBGTilemapBuffers[1], 7, 6, 6, 6);
    BuyMenuPrintItemQuantityAndPrice(taskId);
}

static void AskSellItems(u8 taskId)
{
    sub_80A5D78();
    DoYesNoFuncWithChoice(taskId, &sYesNoSellItemFunctions);
}

static void Task_ChooseHowManyToSell(u8 taskId)
{
    if (sub_80A5350(taskId) == TRUE)
    {
        Menu_EraseWindowRect(6, 11, 12, 11);
        BuyMenuDisplayMessage(gSpecialVar_ItemId, gTasks[taskId].data[1]);
    }
    else if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        Menu_EraseWindowRect(0, 10, 13, 13);
        sub_80A36B8(gBGTilemapBuffers[1], 0, 10, 13, 3);
        ConvertIntToDecimalStringN(gStringVar1, GetItemPrice(gSpecialVar_ItemId) / 2 * gTasks[taskId].data[1], STR_CONV_MODE_LEFT_ALIGN, 6);
        StringExpandPlaceholders(gStringVar4, gText_ICanPayVar1);
        DisplayCannotUseItemMessage(taskId, gStringVar4, AskSellItems, 1);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        BuyMenuPrintItemQuantityAndPrice(taskId);
    }
}

static void sub_80A6760(u8 taskId)
{
    gTasks[taskId].data[10] = 0;
    sub_80A7528(0);
    sub_80A41D4(taskId);
    ItemListMenu_InitMenu();
    ReturnToItemList(taskId);
}

static void BuyMenuPrintItemQuantityAndPrice(u8 taskId)
{
    sub_80A36B8(gBGTilemapBuffers[1], 0, 0, 31, 31);
    HideMoneyBox(0, 0);
    Menu_EraseWindowRect(0, 4, 13, 13);
    Menu_EraseWindowRect(0, 14, 29, 19);
    gTasks[taskId].func = sub_80A6760;
}

static void BuyMenuDisplayMessage(u16 itemId, u16 quantity)
{
    PrintMoneyAmount(GetItemPrice(itemId) / 2 * quantity, 6, 6, 11);
    ConvertIntToDecimalStringN(gStringVar1, GetItemPrice(itemId) / 2 * quantity, STR_CONV_MODE_LEFT_ALIGN, 6);
}

static void sub_80A683C(void)
{
    DrawMoneyBox(gSaveBlock1.money, 0, 0);
    sub_80A4008(gBGTilemapBuffers[1], 1, 1, 12, 2);
}

static void sub_80A6870(u16 itemId, u8 quantity)
{
    AddMoney(&gSaveBlock1.money, GetItemPrice(itemId) / 2 * quantity);
    ChangeAmountInMoneyBox(gSaveBlock1.money, 0, 0);
}

static void sub_80A68A4(void)
{
    if (sub_80A34B4() == TRUE)
        gUnknown_02038563 = CreateTask(Task_BagMenu_HandleInput, 0);
}

void CB2_ChooseBerry(void)
{
    sCurrentBagPocket = BERRIES_POCKET;
    sReturnLocation = ITEMMENULOCATION_BERRY_TREE;
    SetMainCallback2(sub_80A68A4);
}

static void OnBagClose_Field4(u8 taskId)
{
    gTasks[taskId].data[8] = (u32)CB2_ReturnToFieldContinueScript >> 16;
    gTasks[taskId].data[9] = (u32)CB2_ReturnToFieldContinueScript;
    sub_80A5AE4(taskId);
}

static void OnItemSelect_Field4(u8 taskId)
{
    gTasks[taskId].data[8] = (u32)CB2_ReturnToFieldContinueScript >> 16;
    gTasks[taskId].data[9] = (u32)CB2_ReturnToFieldContinueScript;
    Task_FadeAndCloseBagMenu(taskId);
}

static void sub_80A6940(void)
{
    while (1)
    {
        if (SetupBagMenu() == TRUE)
        {
            gUnknown_02038563 = CreateTask(Task_BagMenu_HandleInput, 0);
            sub_80A699C();
            break;
        }
        if (MenuHelpers_IsLinkActive() == TRUE)
            break;
    }
}

void ChooseBerryForMachine(void)
{
    sCurrentBagPocket = BERRIES_POCKET;
    sReturnLocation = RETURN_TO_FIELD_5;
    SetMainCallback2(sub_80A6940);
}

static void sub_80A699C(void)
{
    sPopupMenuActionList = gUnknown_083C16B8;
    gUnknown_02038564 = 4;
}

static void OnBagClose_Field5(u8 taskId)
{
    gTasks[taskId].data[8] = (u32)StartBlender >> 16;
    gTasks[taskId].data[9] = (u32)StartBlender;
    sub_80A5AE4(taskId);
}

static void HandlePopupMenuAction_Confirm(u8 taskId)
{
    gTasks[taskId].data[8] = (u32)StartBlender >> 16;
    gTasks[taskId].data[9] = (u32)StartBlender;
    Task_FadeAndCloseBagMenu(taskId);
}

static void sub_80A6A08(void)
{
    if (sub_80A34B4() == TRUE)
        gUnknown_02038563 = CreateTask(Task_BagMenu_HandleInput, 0);
}

void CB2_GoToItemDepositMenu(void)
{
    sReturnLocation = ITEMMENULOCATION_ITEMPC;
    SetMainCallback2(sub_80A6A08);
}

static void OnBagClose_PC(u8 taskId)
{
    gFieldCallback = ItemStorage_ReturnToMenuAfterDeposit;
    gTasks[taskId].data[8] = (u32)CB2_ReturnToField >> 16;
    gTasks[taskId].data[9] = (u32)CB2_ReturnToField;
    sub_80A5AE4(taskId);
}

static void sub_80A6A84(u8 taskId)
{
    s16 *taskData = gTasks[taskId].data;

    sub_80A4DA4(gBGTilemapBuffers[1]);
    CopyItemName(gSpecialVar_ItemId, gStringVar1);
    ConvertIntToDecimalStringN(gStringVar2, taskData[1], STR_CONV_MODE_LEFT_ALIGN, 3);
    Menu_EraseWindowRect(7, 6, 11, 13);
    sub_80A7528(7);
    sub_80A3D5C(taskId);
    gTasks[taskId].func = sub_80A5E1C;
}

static void WaitDepositErrorMessage(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        gTasks[taskId].data[10] = 0;
        sub_80A48E8(taskId, gBagPocketScrollStates[sCurrentBagPocket].cursorPosition, gBagPocketScrollStates[sCurrentBagPocket].cursorPosition);
        PlaySE(SE_SELECT);
        sub_80A5D38(taskId);
    }
}

static void TryDepositItem(u8 taskId)
{
    s16 *taskData = gTasks[taskId].data;

    if (GetItemImportance(gSpecialVar_ItemId) == 2)
    {
        gTasks[taskId].func = WaitDepositErrorMessage;
        sub_80A7528(9);
    }
    else
    {
        if (AddPCItem(gSpecialVar_ItemId, taskData[1]) == TRUE)
        {
            sub_80A6A84(taskId);
        }
        else
        {
            gTasks[taskId].func = WaitDepositErrorMessage;
            sub_80A7528(8);
        }
    }
}

static void Task_ChooseHowManyToDeposit(u8 taskId)
{
    if (sub_80A5350(taskId) == TRUE)
        return;
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        sub_80A4DA4(gBGTilemapBuffers[1]);
        TryDepositItem(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        gTasks[taskId].data[10] = 0;
        sub_80A48E8(taskId, gBagPocketScrollStates[sCurrentBagPocket].cursorPosition, gBagPocketScrollStates[sCurrentBagPocket].cursorPosition);
        PlaySE(SE_SELECT);
        sub_80A5D38(taskId);
    }
}

static void Task_ItemContext_Deposit(u8 taskId)
{
    gTasks[taskId].data[10] = gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + gBagPocketScrollStates[sCurrentBagPocket].cursorPosition + 1;
    sub_80A48E8(taskId, gBagPocketScrollStates[sCurrentBagPocket].cursorPosition, gBagPocketScrollStates[sCurrentBagPocket].cursorPosition);
    sub_80A73FC();
    PlaySE(SE_SELECT);
    sub_80A7590();
    if (sCurrentBagPocket != KEYITEMS_POCKET)
    {
        gTasks[taskId].func = Task_ChooseHowManyToDeposit;
        sub_80A7528(6);
        // if (sCurrentBagPocket == BAG_POCKET_BERRIES)  Can't get it to match this way
        if (sCurrentBagPocket + 1 == BERRIES_POCKET + 1)
            sub_80A4DD8(taskId, 6, 9, 7, 3, 3);
        else
            sub_80A4DD8(taskId, 7, 9, 6, 3, 2);
    }
    else
    {
        gTasks[taskId].data[1] = 1;
        TryDepositItem(taskId);
    }
}

bool32 UseRegisteredKeyItemOnField(void)
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
    ScriptContext_SetupScript(EventScript_SelectWithoutRegisteredItem);
    return TRUE;
}

static void sub_80A6D98(void)
{
    while (1)
    {
        if (SetupBagMenu() == TRUE)
        {
            gUnknown_02038563 = CreateTask(Task_BagMenu_HandleInput, 0);
            break;
        }
        if (MenuHelpers_IsLinkActive() == TRUE)
            break;
    }
}

void CB2_BagMenuFromBattle(void)
{
    sPopupMenuSelection = 0;
    sReturnLocation = ITEMMENULOCATION_BATTLE;
    SetMainCallback2(sub_80A6D98);
}

static void sub_80A6DF0(u16 *a)
{
    u8 r6 = (gUnknown_02038564 - 1) * 2;

    Menu_DrawStdWindowFrame(7, 9 - r6, 13, 12);
    sub_80A4008(a, 8, 10 - r6, 5, r6 + 2);
    if (gUnknown_02038564 == 1)
    {
        Menu_PrintText(sItemMenuActions[sPopupMenuActionList[0]].text, 8, 10);
    }
    else
    {
        Menu_PrintText(sItemMenuActions[sPopupMenuActionList[0]].text, 8, 8);
        Menu_PrintText(sItemMenuActions[sPopupMenuActionList[1]].text, 8, 10);
    }
    InitMenu(0, 8, 10 - r6, gUnknown_02038564, 0, 5);
    sub_80A7528(2);
}

static void sub_80A6EB8(u8 taskId)
{
    if (JOY_REPEAT(DPAD_ANY) == DPAD_UP)
    {
        if (sPopupMenuSelection == 1)
        {
            PlaySE(SE_SELECT);
            sPopupMenuSelection = Menu_MoveCursor(-1);
        }
    }
    else if (JOY_REPEAT(DPAD_ANY) == DPAD_DOWN)
    {
        if (sPopupMenuSelection + 1 < gUnknown_02038564)
        {
            PlaySE(SE_SELECT);
            sPopupMenuSelection = Menu_MoveCursor(1);
        }
    }
    else if (JOY_NEW(A_BUTTON))
    {
        gTasks[taskId].data[10] = 0;
        sub_80A48E8(taskId, gBagPocketScrollStates[sCurrentBagPocket].cursorPosition, gBagPocketScrollStates[sCurrentBagPocket].cursorPosition);
        sub_80A4DA4(gBGTilemapBuffers[1]);
        sItemMenuActions[sPopupMenuActionList[sPopupMenuSelection]].func(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        gTasks[taskId].data[10] = 0;
        sub_80A48E8(taskId, gBagPocketScrollStates[sCurrentBagPocket].cursorPosition, gBagPocketScrollStates[sCurrentBagPocket].cursorPosition);
        sub_80A4DA4(gBGTilemapBuffers[1]);
        sItemMenuActions[5].func(taskId);
    }
}

static const u8 sContextMenuItems_BattleUse[] = {4, 5};
static const u8 sContextMenuItems_Cancel[] = {5, 0};

static void sub_80A6FDC(void)
{
    if (GetItemBattleUsage(gSpecialVar_ItemId) != 0)
    {
        sPopupMenuActionList = sContextMenuItems_BattleUse;
        gUnknown_02038564 = 2;
    }
    else
    {
        sPopupMenuActionList = sContextMenuItems_Cancel;
        gUnknown_02038564 = 1;
    }
}

static void OnItemSelect_Battle(u8 taskId)
{
    sPopupMenuSelection = 0;
    sub_80A6FDC();
    gTasks[taskId].data[10] = gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + gBagPocketScrollStates[sCurrentBagPocket].cursorPosition + 1;
    sub_80A48E8(taskId, gBagPocketScrollStates[sCurrentBagPocket].cursorPosition, gBagPocketScrollStates[sCurrentBagPocket].cursorPosition);
    sub_80A73FC();
    sub_80A6DF0(gBGTilemapBuffers[1]);
    gTasks[taskId].func = sub_80A6EB8;
}

void sub_80A7094(u8 taskId)
{
    gTasks[taskId].data[8] = (u32)CB2_SetUpReshowBattleScreenAfterMenu2 >> 16;
    gTasks[taskId].data[9] = (u32)CB2_SetUpReshowBattleScreenAfterMenu2;
    gTasks[taskId].func = Task_CloseBagMenu;
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
}

static void OnBagClose_Battle(u8 taskId)
{
    PlaySE(SE_SELECT);
    sub_80A7094(taskId);
}

static void ItemMenu_UseInBattle(u8 taskId)
{
    if (GetItemBattleFunc(gSpecialVar_ItemId) != NULL)
    {
        PlaySE(SE_SELECT);
        GetItemBattleFunc(gSpecialVar_ItemId)(taskId);
    }
}

static void sub_80A7124(u8 taskId)
{
    PlaySE(SE_SELECT);
    sub_80A7528(0);
    sub_80A41D4(taskId);
    ItemListMenu_InitMenu();
    ReturnToItemList(taskId);
}

static void sub_80A7150(void)
{
    while (1)
    {
        if (SetupBagMenu() == TRUE)
        {
            gUnknown_02038563 = CreateTask(Task_WallyTutorialBagMenu, 0);
            gTasks[gUnknown_02038563].data[15] = 0;
            break;
        }
        if (MenuHelpers_IsLinkActive() == TRUE)
            break;
    }
}

void DoWallyTutorialBagMenu(void)
{
    u8 i;

    sPopupMenuSelection = 0;
    sCurrentBagPocket = ITEMS_POCKET;
    for (i = 0; i < 5; i++)
    {
        gBagPocketScrollStates[i].cursorPosition = 0;
        gBagPocketScrollStates[i].scrollPosition = 0;
    }

    // Save player's items
    memcpy(ewramSavedItemsPocket, gSaveBlock1.bagPocket_Items, sizeof(gSaveBlock1.bagPocket_Items));
    memcpy(ewramSavedPokeballsPocket, gSaveBlock1.bagPocket_PokeBalls, sizeof(gSaveBlock1.bagPocket_PokeBalls));

    // Add Wally's items to the bag
    ClearItemSlots(gSaveBlock1.bagPocket_Items, ARRAY_COUNT(gSaveBlock1.bagPocket_Items));
    ClearItemSlots(gSaveBlock1.bagPocket_PokeBalls, ARRAY_COUNT(gSaveBlock1.bagPocket_PokeBalls));
    AddBagItem(ITEM_POTION, 1);
    AddBagItem(ITEM_POKE_BALL, 1);

    sReturnLocation = ITEMMENULOCATION_WALLY;
    SetMainCallback2(sub_80A7150);
}

static void Task_WallyTutorialBagMenu(u8 taskId)
{
    s16 *taskData = gTasks[taskId].data;

    switch (taskData[15])
    {
    case 102:
        PlaySE(SE_SELECT);
        sub_80A4E8C(1, 2);
        break;
    case 204:
        PlaySE(SE_SELECT);
        StopVerticalScrollIndicators(SCROLL_ARROW_LEFT);
        StopVerticalScrollIndicators(SCROLL_ARROW_RIGHT);
        gSpecialVar_ItemId = ITEM_POKE_BALL;
        sPopupMenuActionList = sContextMenuItems_BattleUse;
        gUnknown_02038564 = 2;
        gTasks[taskId].data[10] = gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + gBagPocketScrollStates[sCurrentBagPocket].cursorPosition + 1;
        sub_80A48E8(taskId, gBagPocketScrollStates[sCurrentBagPocket].cursorPosition, gBagPocketScrollStates[sCurrentBagPocket].cursorPosition);
        sub_80A73FC();
        sub_80A6DF0(gBGTilemapBuffers[1]);
        break;
    case 306:
        PlaySE(SE_SELECT);
        sub_80A4DA4(gBGTilemapBuffers[1]);

        // Restore player's items
        memcpy(gSaveBlock1.bagPocket_Items, ewramSavedItemsPocket, sizeof(gSaveBlock1.bagPocket_Items));
        memcpy(gSaveBlock1.bagPocket_PokeBalls, ewramSavedPokeballsPocket, sizeof(gSaveBlock1.bagPocket_PokeBalls));

        taskData[8] = (u32)CB2_SetUpReshowBattleScreenAfterMenu2 >> 16;
        taskData[9] = (u32)CB2_SetUpReshowBattleScreenAfterMenu2;
        gTasks[taskId].func = Task_CloseBagMenu;
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        return;
    }
    taskData[15]++;
}

static void ItemListMenu_InitMenu(void)
{
    InitMenu(0, 14, 2, gBagPocketScrollStates[sCurrentBagPocket].cursorMax + 1, gBagPocketScrollStates[sCurrentBagPocket].cursorPosition, 0);
    CreateBlendedOutlineCursor(16, 0xFFFF, 12, 0x2D9F, 15);
    sub_80A73C0();
}

static void sub_80A73C0(void)
{
    MenuCursor_SetPos814AD7C(0x70, gBagPocketScrollStates[sCurrentBagPocket].cursorPosition * 16 + 16);
}

static void sub_80A73F0(void)
{
    sub_814ADC8();
}

static void sub_80A73FC(void)
{
    Menu_DestroyCursor();
    MenuCursor_Destroy814AD44();
}

static void sub_80A740C(void)
{
    sub_80A75E4();
    sub_80A7768();
    sub_80A7420();
#if DEBUG
    if (gLinkOpen == TRUE)
        debug_sub_8008264(gLink.recvQueue.count, 1, 1, 2, 0);
#endif
}

static void sub_80A7420(void)
{
    struct UnknownStruct2 *unkStruct = &gUnknown_0203853C;
    int index;

    switch (unkStruct->unk0)
    {
    case 3:
        unkStruct->unk0 = 2;
        break;
    case 2:
        switch (unkStruct->unk2)
        {
        case 0:
            index = gBagPocketScrollStates[sCurrentBagPocket].scrollPosition + gBagPocketScrollStates[sCurrentBagPocket].cursorPosition;
            ItemListMenu_ChangeDescription(gCurrentBagPocketItemSlots[index].itemId, unkStruct->textLine);
            break;
        case 1:
            sub_80A4A98(gOtherText_SwitchWhichItem, unkStruct->textLine);
            break;
        case 2:
            sub_80A4A98(gOtherText_WhatWillYouDo2, unkStruct->textLine);
            break;
        case 3:
            sub_80A4A98(gOtherText_HowManyToToss, unkStruct->textLine);
            break;
        case 4:
            sub_80A4A98(gText_ThrewAwayVar2Var1s, unkStruct->textLine);
            break;
        case 5:
            sub_80A4A98(gText_ConfirmTossItems, unkStruct->textLine);
            break;
        case 6:
            sub_80A4A98(gOtherText_HowManyToDeposit, unkStruct->textLine);
            break;
        case 7:
            sub_80A4A98(gText_DepositedVar2Var1s, unkStruct->textLine);
            break;
        case 8:
            sub_80A4A98(gText_NoRoomForItems, unkStruct->textLine);
            break;
        case 9:
            sub_80A4A98(gOtherText_CantStoreSomeoneItem, unkStruct->textLine);
            break;
        }
        unkStruct->textLine++;
        if (unkStruct->textLine == 3)
        {
            unkStruct->unk0 = 0;
            sub_80A7918();
        }
        break;
    }
}

static void sub_80A751C(void)
{
    gUnknown_0203853C.unk0 = 0;
}

static void sub_80A7528(u8 a)
{
    gUnknown_0203853C.textLine = 0;
    gUnknown_0203853C.unk2 = a;
    switch (sub_80A78F4())
    {
    case 0:
        gUnknown_0203853C.unk0 = 2;
        while (gUnknown_0203853C.unk0 != 0)
            sub_80A7420();
        break;
    case 1:
        gUnknown_0203853C.unk0 = 3;
        break;
    case 2:
        gUnknown_0203853C.unk0 = 1;
        break;
    }
}

static void sub_80A756C(void)
{
    if (gUnknown_0203853C.unk0 == 2 || gUnknown_0203853C.unk0 == 3)
        sub_80A7918();
    gUnknown_0203853C.unk0 = 0;
}

static void sub_80A7590(void)
{
    Menu_EraseWindowRect(0, 13, 13, 20);
    sub_80A756C();
}

static void sub_80A75A8(void)
{
    if (gUnknown_0203853C.unk0 == 2 || gUnknown_0203853C.unk0 == 3)
        gUnknown_0203853C.unk0 = 1;
}

static void sub_80A75C4(void)
{
    switch (gUnknown_0203853C.unk0)
    {
    case 1:
        gUnknown_0203853C.unk0 = 3;
        break;
    case 0:
        sub_80A7918();
        break;
    }
}

static void sub_80A75E4(void)
{
    if (gUnknown_02038540.unk0 == 1 && sub_80A7988() == 0)
    {
        int r1;

        sub_80A47E8(gUnknown_02038563, gUnknown_02038540.unk1, gUnknown_02038540.unk1, 1);
        gUnknown_02038540.unk1++;
        if (gUnknown_02038540.unk2 != 0)
            r1 = 5;
        else
            r1 = 7;
        if (r1 < gUnknown_02038540.unk1)
        {
            gUnknown_02038540.unk0 = 0;
            sub_80A78E8();
        }
    }
}

static void sub_80A7630(void)
{
    gUnknown_02038540.unk0 = 0;
}

static void sub_80A763C(void)
{
    struct UnknownStruct3 *r4 = &gUnknown_02038540;

    switch (sub_80A78C4())
    {
    case FALSE:
        sub_80A48E8(gUnknown_02038563, 0, 7);
        break;
    case TRUE:
        r4->unk0 = 1;
        r4->unk1 = 0;
        r4->unk2 = 0;
        break;
    }
}

static void sub_80A7678(void)
{
    if (gUnknown_02038540.unk0 == 1)
        sub_80A78E8();
    gUnknown_02038540.unk0 = 0;
}

static void sub_80A7694(void)
{
    gUnknown_02038540.unk2 = 1;
}

static void sub_80A76A0(void)
{
    Menu_EraseWindowRect(14, 2, 29, 18);
    sub_80A7678();
}

static bool32 sub_80A76B8(void)
{
    struct UnknownStruct3 *s = &gUnknown_02038540;

    return (s->unk0 == 0);
}

static bool32 sub_80A76D0(void)
{
    struct UnknownStruct3 *s = &gUnknown_02038540;

    return (s->unk1 > 5);
}

static void sub_80A76E8(void)
{
    if (gUnknown_02038544.unk0 == 1)
    {
        DisplayItemMessageOnField(
          gUnknown_02038544.unk1,
          gUnknown_02038544.unk4,
          gUnknown_02038544.unk8,
          gUnknown_02038544.unk2);
        gUnknown_02038544.unk0 = 0;
    }
}

static void sub_80A770C(void)
{
    gUnknown_02038544.unk0 = 0;
}

static void DisplayCannotUseItemMessage(int taskId, const u8 *string, TaskFunc callback, int d)
{
    struct UnknownStruct4 *r4 = &gUnknown_02038544;

    switch (sub_80A7924())
    {
    case 0:
        DisplayItemMessageOnField(taskId, string, callback, d);
        break;
    case 2:
        r4->unk0 = 1;
        r4->unk1 = taskId;
        r4->unk4 = string;
        r4->unk8 = callback;
        r4->unk2 = d;
        break;
    }
}

static void sub_80A7768(void)
{
    struct UnknownStruct5 *r4 = &gUnknown_02038550;

    if (r4->unk0 == 2)
    {
        if (r4->unk1 != 0)
        {
            if (r4->unk2 != 1)
            {
                const u8 *text = sItemMenuActions[sPopupMenuActionList[r4->unk1 - 1]].text;
                int var = r4->unk1 - 1;

                Menu_PrintText(text, 7, var * 2 + 1 + r4->unk3);
            }
            else
            {
                const u8 *text;
                int var;

                if (r4->unk1 == 1)
                    text = sub_80A4B90(gSpecialVar_ItemId);
                else
                    text = sItemMenuActions[sPopupMenuActionList[r4->unk1 - 1]].text;
                var = r4->unk1 - 1;
                Menu_PrintText(text, (var >> 1) * 6 + 1, (var & 1) * 2 + 8);
            }
            if (r4->unk1 == gUnknown_02038564)
            {
                r4->unk0 = 0;
                sub_80A7970();
            }
        }
        r4->unk1++;
    }
}

static void sub_80A7828(void)
{
    gUnknown_02038550.unk0 = 0;
}

static void sub_80A7834(int a, int b)
{
    struct UnknownStruct5 *r4 = &gUnknown_02038550;

    switch (sub_80A7958())
    {
    case 1:
        r4->unk0 = 2;
        r4->unk1 = 0;
        r4->unk2 = a;
        r4->unk3 = b;
        break;
    case 2:
        r4->unk0 = 1;
        r4->unk1 = 0;
        r4->unk2 = a;
        r4->unk3 = b;
        break;
    }
}

static void sub_80A7868(void)
{
    if (gUnknown_02038550.unk0 == 2)
        gUnknown_02038550.unk0 = 1;
}

static void sub_80A7880(void)
{
    switch (gUnknown_02038550.unk0)
    {
    case 1:
        gUnknown_02038550.unk0 = 2;
        break;
    case 0:
        sub_80A7970();
        break;
    }
}

static bool32 sub_80A78A0(void)
{
    struct UnknownStruct5 *r0 = &gUnknown_02038550;

    return (r0->unk0 == 0);
}

static void sub_80A78B8(void)
{
    gUnknown_02038554.unk0 = 0;
}

static bool32 sub_80A78C4(void)
{
    bool32 retVal;

    if (gLinkOpen == TRUE)
    {
        sub_80A7868();
        sub_80A75A8();
        retVal = TRUE;
    }
    else
    {
        retVal = FALSE;
    }
    return retVal;
}

static void sub_80A78E8(void)
{
    sub_80A7880();
}

static int sub_80A78F4(void)
{
    int retVal;

    if (gLinkOpen == TRUE)
    {
        if (sub_80A76B8() != 0)
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

static void sub_80A7918(void)
{
    sub_80A76E8();
}

static int sub_80A7924(void)
{
    if (gLinkOpen == TRUE && sub_80A76B8() == 0)
    {
        if (sub_80A76D0() != 0)
        {
            sub_80A7678();
            return 0;
        }
    }
    else
    {
        return 0;
    }
    sub_80A7694();
    return 2;
}

static int sub_80A7958(void)
{
    if (sub_80A76B8() == 0)
        return 2;
    sub_80A75A8();
    return 1;
}

static void sub_80A7970(void)
{
    sub_80A75C4();
}

static void sub_80A797C(void)
{
    gUnknown_02038554.unk0 = 1;
}

static int sub_80A7988(void)
{
    int val = gUnknown_02038554.unk0;

    gUnknown_02038554.unk0 = 0;
    return val;
}

static const u16 sRotatingBall_Pal[] = INCBIN_U16("graphics/bag/rotating_ball.gbapal");
static const u8 sRotatingBall_Gfx[] = INCBIN_U8("graphics/bag/rotating_ball.4bpp");

const u8 gSpriteImage_UnusedCherry[] = INCBIN_U8("graphics/unused/cherry.4bpp");
const u16 gSpritePalette_UnusedCherry[] = INCBIN_U16("graphics/unused/cherry.gbapal");

//------------------------------------------------------------------------------
// Bag Sprite
//------------------------------------------------------------------------------

static const struct OamData sBagOamData =
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

static const union AnimCmd sSpriteAnim_Bag_Closed[] =
{
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_Bag_Items[] =
{
    ANIMCMD_FRAME(64, 4),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_Bag_KeyItems[] =
{
    ANIMCMD_FRAME(128, 4),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_Bag_Pokeballs[] =
{
    ANIMCMD_FRAME(192, 4),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_Bag_TMsHMs[] =
{
    ANIMCMD_FRAME(256, 4),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_Bag_Berries[] =
{
    ANIMCMD_FRAME(320, 4),
    ANIMCMD_END,
};

static const union AnimCmd *const sBagSpriteAnimTable[] =
{
    sSpriteAnim_Bag_Closed,
    sSpriteAnim_Bag_Items,
    sSpriteAnim_Bag_Pokeballs,
    sSpriteAnim_Bag_TMsHMs,
    sSpriteAnim_Bag_Berries,
    sSpriteAnim_Bag_KeyItems,
};

static const union AffineAnimCmd sSpriteAffineAnim_BagShake[] =
{
    AFFINEANIMCMD_FRAME(0, 0, -2, 2),
    AFFINEANIMCMD_FRAME(0, 0, 2, 4),
    AFFINEANIMCMD_FRAME(0, 0, -2, 4),
    AFFINEANIMCMD_FRAME(0, 0, 2, 2),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd *const sBagAffineAnimCmds[] =
{
    sSpriteAffineAnim_BagShake,
};

const struct CompressedSpriteSheet gBagMaleSpriteSheet = {gBagMaleTiles, 0x3000, 30000};
const struct CompressedSpriteSheet gBagFemaleSpriteSheet = {gBagFemaleTiles, 0x3000, 30000};
const struct CompressedSpritePalette gBagPaletteTable = {gBagPalette, 30000};

static void sub_80A7998(struct Sprite *sprite)
{
    sprite->animNum = 0;
    sprite->data[0] = 0;
    sprite->data[1] = 0;
    sprite->data[2] = 0;
    sprite->data[3] = 0;
    sprite->data[4] = 0;
    sprite->callback = sub_80A79B4;
}

static void sub_80A79B4(struct Sprite *sprite)
{
    if (gUnknown_0203855B != -1)
        sub_80A79EC(sprite);
    if (gUnknown_0203855C != 0)
        sub_80A7A94(sprite);
}

static void sub_80A79EC(struct Sprite *sprite)
{
    switch (sprite->data[3])
    {
    case 0:
        if (gUnknown_0203855B != 6)
        {
            sprite->animBeginning = TRUE;
            sprite->animNum = gUnknown_0203855B;
            gUnknown_0203855B = -1;
        }
        else
        {
            sprite->animBeginning = TRUE;
            sprite->animNum = 0;
            sprite->y -= 4;
            sprite->data[0] = 4;
            sprite->data[3] = 1;
            sub_80A7AE4(sprite);
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
            gUnknown_0203855B = -1;
            sprite->data[1] = 0;
            sprite->data[3] = 0;
        }
        break;
    }
}

static void sub_80A7A94(struct Sprite *sprite)
{
    switch (sprite->data[4])
    {
    case 0:
        sprite->oam.affineMode = 1;
        sprite->affineAnims = sBagAffineAnimCmds;
        InitSpriteAffineAnim(sprite);
        sprite->data[4] = 1;
        break;
    case 1:
        sprite->data[2]++;
        if (sprite->data[2] == 12)
            sub_80A7AE4(sprite);
        break;
    }
}

static void sub_80A7AE4(struct Sprite *sprite)
{
    gUnknown_0203855C = 0;
    sprite->oam.affineMode = 0;
    sprite->data[2] = 0;
    sprite->data[4] = 0;
    FreeOamMatrix(sprite->oam.matrixNum);
}

static const struct SpriteTemplate sBagSpriteTemplate =
{
    .tileTag = 30000,
    .paletteTag = 30000,
    .oam = &sBagOamData,
    .anims = sBagSpriteAnimTable,
    .images = NULL,
    .affineAnims = sBagAffineAnimCmds,
    .callback = sub_80A7998,
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

static const union AffineAnimCmd sSpriteAffineAnim_RotatingBallRotation1[] =
{
    AFFINEANIMCMD_FRAME(0, 0, 8, 32),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sSpriteAffineAnim_RotatingBallRotation2[] =
{
    AFFINEANIMCMD_FRAME(0, 0, -8, 32),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd *const gSpriteAffineAnimTable_83C1D20[] =
{
    sSpriteAffineAnim_RotatingBallRotation1,
    sSpriteAffineAnim_RotatingBallRotation2,
};

static const struct SpriteFrameImage gSpriteImageTable_83C1D28[] =
{
    {sRotatingBall_Gfx, sizeof(sRotatingBall_Gfx)},
};

static const struct SpritePalette sRotatingBallPaletteTable = {sRotatingBall_Pal, 8};

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
                sprite->affineAnims = gSpriteAffineAnimTable_83C1D20;
            else
                sprite->affineAnims = gSpriteAffineAnimTable_83C1D20 + 1;
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
    .paletteTag = 8,
    .oam = &sRotatingBallOamData,
    .anims = sBagSpriteAnimTable,
    .images = gSpriteImageTable_83C1D28,
    .affineAnims = gSpriteAffineAnimTable_83C1D20,
    .callback = SpriteCB_SwitchPocketRotatingBallInit,
};

static void CreateBagPokeballSprite(u8 a)
{
    LoadSpritePalette(&sRotatingBallPaletteTable);
    sPokeballSpriteId = CreateSprite(&sRotatingBallSpriteTemplate, 16, 88, 0);
    gSprites[sPokeballSpriteId].data[7] = a;
}

static void sub_80A7C64(void)
{
    FreeSpritePaletteByTag(8);
    FreeOamMatrix(gSprites[sPokeballSpriteId].oam.matrixNum);
    DestroySprite(&gSprites[sPokeballSpriteId]);
}

static const struct OamData sBerryPicOamData =
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

static const union AnimCmd sAnim_BerryPic[] =
{
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END,
};

static const union AnimCmd *const sBerryPicSpriteAnimTable[] =
{
    sAnim_BerryPic,
};

static const struct SpriteFrameImage sBerryPicSpriteImageTable[] =
{
    {ewramBerryPic, 0x800},
};

static void SpriteCB_BerrySprite(struct Sprite *);
static const struct SpriteTemplate sBerryPicSpriteTemplate =
{
    .tileTag = 0xFFFF,
    .paletteTag = 30020,
    .oam = &sBerryPicOamData,
    .anims = sBerryPicSpriteAnimTable,
    .images = sBerryPicSpriteImageTable,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_BerrySprite,
};

static const union AffineAnimCmd sSpriteAffineAnim_BerryPicRotation1[] =
{
    AFFINEANIMCMD_FRAME(-1, -1, -3, 96),
    AFFINEANIMCMD_FRAME(0, 0, 0, 16),
    AFFINEANIMCMD_FRAME(-2, -2, -1, 64),
    AFFINEANIMCMD_FRAME(-8, 0, 0, 16),
    AFFINEANIMCMD_FRAME(0, -8, 0, 16),
    AFFINEANIMCMD_FRAME(256, 256, 0, 0),
    AFFINEANIMCMD_JUMP(0),
};

static const union AffineAnimCmd sSpriteAffineAnim_BerryPicRotation2[] =
{
    AFFINEANIMCMD_FRAME(-1, -1, 3, 96),
    AFFINEANIMCMD_FRAME(0, 0, 0, 16),
    AFFINEANIMCMD_FRAME(-2, -2, 1, 64),
    AFFINEANIMCMD_FRAME(-8, 0, 0, 16),
    AFFINEANIMCMD_FRAME(0, -8, 0, 16),
    AFFINEANIMCMD_FRAME(256, 256, 0, 0),
    AFFINEANIMCMD_JUMP(0),
};

static const union AffineAnimCmd *const sBerryPicRotatingAnimCmds[] =
{
    sSpriteAffineAnim_BerryPicRotation1,
    sSpriteAffineAnim_BerryPicRotation2,
};

static const struct SpriteTemplate sBerryPicRotatingSpriteTemplate =
{
    .tileTag = 0xFFFF,
    .paletteTag = 30020,
    .oam = &sBerryPicRotatingOamData,
    .anims = sBerryPicSpriteAnimTable,
    .images = sBerryPicSpriteImageTable,
    .affineAnims = sBerryPicRotatingAnimCmds,
    .callback = SpriteCallbackDummy,
};

struct CompressedTilesPal
{
    const u8 *lzPic;
    const u8 *lzPalette;
};

static const struct CompressedTilesPal sBerryPicTable[] =
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

static void ArrangeBerryGfx(const u8 *src, u8 *dest)
{
    u8 i;
    u8 j;

    memset(dest, 0, 0x800);
    dest += 0x100;
    for (i = 0; i < 6; i++)
    {
        dest += 32;
        for (j = 0; j < 6; j++)
        {
            memcpy(dest, src, 32);
            dest += 32;
            src += 32;
        }
        if (i != 5)
            dest += 32;
    }
}

static void LoadBerryGfx(u8 berryId)
{
    struct SpritePalette spritePal;

    if (berryId == 0x2A && IsEnigmaBerryValid() == TRUE)
    {
        ArrangeBerryGfx(gSaveBlock1.enigmaBerry.pic, ewramBerryPic);
        spritePal.data = gSaveBlock1.enigmaBerry.palette;
        spritePal.tag = 0x7544;
        LoadSpritePalette(&spritePal);
    }
    else
    {
        spritePal.data = (u16 *)sBerryPicTable[berryId].lzPalette;
        spritePal.tag = 0x7544;
        LoadCompressedSpritePalette((struct CompressedSpritePalette *)&spritePal);
        LZDecompressWram(sBerryPicTable[berryId].lzPic, ewramBerryPicTemp);
        ArrangeBerryGfx(ewramBerryPicTemp, ewramBerryPic);
    }
}

u8 CreateBerryTagSprite(u8 berryId, s16 x, s16 y)
{
    LoadBerryGfx(berryId);
    return CreateSprite(&sBerryPicSpriteTemplate, x, y, 0);
}

static void SpriteCB_BerrySprite(struct Sprite *sprite)
{
    sprite->y2 = -gBattle_BG1_Y;
}

void FreeBerryTagSpritePalette(void)
{
    FreeSpritePaletteByTag(0x7544);
    FreeSpritePaletteByTag(8);
}

u8 CreateSpinningBerrySprite(u8 berryId, u8 x, u8 y, bool8 startAffine)
{
    u8 spriteId;

    FreeSpritePaletteByTag(0x7544);
    LoadBerryGfx(berryId);
    spriteId = CreateSprite(&sBerryPicRotatingSpriteTemplate, x, y, 0);
    if (startAffine == TRUE)
        StartSpriteAffineAnim(&gSprites[spriteId], 1);
    return spriteId;
}

const struct CompressedSpriteSheet gBerryCheckCircleSpriteSheet = {gBerryCheckCircle_Gfx, 2048, 0x2710};

const struct CompressedSpritePalette gBerryCheckCirclePaletteTable = {gBerryCheck_Pal, 0x2710};

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

static const union AnimCmd sSpriteAnim_BerryCheckCircle[] =
{
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END,
};

static const union AnimCmd *const sBerryCheckCircleSpriteAnimTable[] =
{
    sSpriteAnim_BerryCheckCircle,
};

static const struct SpriteTemplate sBerryCheckCircleSpriteTemplate =
{
    .tileTag = 10000,
    .paletteTag = 10000,
    .oam = &sBerryCheckCircleOamData,
    .anims = sBerryCheckCircleSpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_BerrySprite,
};

u8 CreateBerryFlavorCircleSprite(s16 x)
{
    return CreateSprite(&sBerryCheckCircleSpriteTemplate, x, 99, 0);
}
