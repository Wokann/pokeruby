#include "global.h"
#include "overworld.h"
#include "sprite.h"
#include "script.h"
#include "strings.h"
#include "task.h"
#include "scanline_effect.h"
#include "text.h"
#include "main.h"
#include "menu.h"
#include "field_fadetransition.h"
#include "palette.h"
#include "graphics.h"
#include "decompress.h"
#include "menu_helpers.h"
#include "battle.h"
#include "item_menu.h"
#include "item_use.h"
#include "item.h"
#include "constants/items.h"
#include "constants/berry.h"
#include "sound.h"
#include "constants/songs.h"
#include "safari_zone.h"
#include "event_data.h"
#include "pokeblock.h"
#include "ewram.h"

struct PokeblockMenuState
{
    u8 selectedRow;
    u8 scrollOffset;
    u8 itemsNo;
    u8 maxShowed;
};

enum
{
    PKBL_USE_ON_FIELD,
    PKBL_TOSS,
    PKBL_CANCEL,
    PKBL_USE_IN_BATTLE,
    PKBL_USE_ON_FEEDER,
};

static EWRAM_DATA u8 sPokeblockCaseContext = 0;
static EWRAM_DATA struct PokeblockMenuState sPokeblockMenuState = {0};
static EWRAM_DATA u8 sNumPokeblockActions = 0;

// function declarations

// sPokeblockMenuActions
static void PokeblockAction_UseOnField(u8);
static void PokeblockAction_Toss(u8);
static void PokeblockAction_Cancel(u8);
static void PokeblockAction_UseInBattle(u8);
static void PokeblockAction_UseOnPokeblockFeeder(u8);

// sTossYesNoFuncTable
static void TossedPokeblockMessage(u8);
static void CloseTossPokeblockWindow(u8);

// InitPokeblockMenu
static bool8 LoadPokeblockMenuGfx(void);
static void CompactPokeblockSlots(void);
static void SetMenuItemsCountAndMaxShowed(void);
static void DrawPokeblockMenuTitleText(void);
static void DrawPokeblockInfoLabels(void);
static void UpdatePokeblockList(u8);

// CB2_InitPokeblockMenu
static void Task_HandlePokeblockMenuInput(u8);

// UpdatePokeblockList
static void DrawPokeblockInfo(bool8);

// OnPokeblockMenuCursorMoved
static void SpriteCB_ShakePokeblockCase(struct Sprite *);

// Task_HandlePokeblockMenuInput
static void Task_HandlePokeblocksSwapInput(u8);
static void FadePaletteAndSetTaskToClosePokeblockCase(u8);
static void ShowPokeblockActionsWindow(u8);

// Task_HandlePokeblocksSwapInput
static void DrawPokeblockSwapSelection(u8, u8);
static void SwapPokeblockMenuItems(u8);

// ShowPokeblockActionsWindow
static void Task_HandlePokeblockActionsInput(u8);

// ShowTossPokeblockPrompt
static void CreateTossPokeblockYesNoMenu(u8);

// TossedPokeblockMessage
static void RefreshPokeblockListAfterToss(u8);

static const u8 *sPokeblockActionIds;

// rodata

#define TAG_POKEBLOCK_CASE 14800
#define tSwapPokeblockId data[0]

const s8 gPokeblockFlavorCompatibilityTable[] =
{
    // Cool, Beauty, Cute, Smart, Tough
          0,      0,    0,     0,     0, // Hardy
          1,      0,    0,     0,    -1, // Lonely
          1,      0,   -1,     0,     0, // Brave
          1,     -1,    0,     0,     0, // Adamant
          1,      0,    0,    -1,     0, // Naughty
         -1,      0,    0,     0,     1, // Bold
          0,      0,    0,     0,     0, // Docile
          0,      0,   -1,     0,     1, // Relaxed
          0,     -1,    0,     0,     1, // Impish
          0,      0,    0,    -1,     1, // Lax
         -1,      0,    1,     0,     0, // Timid
          0,      0,    1,     0,    -1, // Hasty
          0,      0,    0,     0,     0, // Serious
          0,     -1,    1,     0,     0, // Jolly
          0,      0,    1,    -1,     0, // Naive
         -1,      1,    0,     0,     0, // Modest
          0,      1,    0,     0,    -1, // Mild
          0,      1,   -1,     0,     0, // Quiet
          0,      0,    0,     0,     0, // Bashful
          0,      1,    0,    -1,     0, // Rash
         -1,      0,    0,     1,     0, // Calm
          0,      0,    0,     1,    -1, // Gentle
          0,      0,   -1,     1,     0, // Sassy
          0,     -1,    0,     1,     0, // Careful
          0,      0,    0,     0,     0  // Quirky
};

void (*const sPokeblockCaseExitCallbacks[])(void) =
{
    CB2_ReturnToBagMenu,
    CB2_ReturnToField,
    CB2_SetUpReshowBattleScreenAfterMenu2,
    CB2_ReturnToField
};

const u8 *const gPokeblockNames[] =
{
    NULL,
    ContestStatsText_RedPokeBlock,
    ContestStatsText_BluePokeBlock,
    ContestStatsText_PinkPokeBlock,
    ContestStatsText_GreenPokeBlock,
    ContestStatsText_YellowPokeBlock,
    ContestStatsText_PurplePokeBlock,
    ContestStatsText_IndigoPokeBlock,
    ContestStatsText_BrownPokeBlock,
    ContestStatsText_LiteBluePokeBlock,
    ContestStatsText_OlivePokeBlock,
    ContestStatsText_GrayPokeBlock,
    ContestStatsText_BlackPokeBlock,
    ContestStatsText_WhitePokeBlock,
    ContestStatsText_GoldPokeBlock
};

const struct MenuAction2 sPokeblockMenuActions[] =
{
    {OtherText_Use,     PokeblockAction_UseOnField},
    {OtherText_Toss,    PokeblockAction_Toss},
    {gText_Cancel, PokeblockAction_Cancel},
    {OtherText_Use,     PokeblockAction_UseInBattle},
    {OtherText_Use,     PokeblockAction_UseOnPokeblockFeeder},
};

const u8 sActionsOnField[] = {PKBL_USE_ON_FIELD, PKBL_TOSS, PKBL_CANCEL};
const u8 sActionsInBattle[] = {PKBL_USE_IN_BATTLE, PKBL_CANCEL};
const u8 sActionsOnPokeblockFeeder[] = {PKBL_USE_ON_FEEDER, PKBL_CANCEL};

const struct YesNoFuncTable sTossYesNoFuncTable = {TossedPokeblockMessage, CloseTossPokeblockWindow};

const u8 gUnusedPokeblockData[] = {0x16, 0x17, 0x18, 0x21, 0x2f};

const struct OamData sOamData_PokeblockCase =
{
    .size = 3,
    .priority = 2
};

const union AnimCmd sSpriteAnim_PokeblockCase[] =
{
    ANIMCMD_FRAME(.imageValue = 0, .duration = 0),
    ANIMCMD_END
};

const union AnimCmd *const sSpriteAnimTable_PokeblockCase[] =
{
    sSpriteAnim_PokeblockCase
};

const union AffineAnimCmd sAffineAnim_PokeblockCaseShake[] =
{
    AFFINEANIMCMD_FRAME(0, 0, -2,  2),
    AFFINEANIMCMD_FRAME(0, 0,  2,  4),
    AFFINEANIMCMD_FRAME(0, 0, -2,  4),
    AFFINEANIMCMD_FRAME(0, 0,  2,  2),
    AFFINEANIMCMD_END
};

const union AffineAnimCmd *const sAffineAnims_PokeblockCaseShake[] =
{
    sAffineAnim_PokeblockCaseShake
};

const struct CompressedSpriteSheet gPokeblockCase_SpriteSheet =
{
    gMenuPokeblockDevice_Gfx,
    0x800,
    TAG_POKEBLOCK_CASE
};

const struct CompressedSpritePalette gPokeblockCase_SpritePal =
{
    gMenuPokeblockDevice_Pal,
    TAG_POKEBLOCK_CASE
};

const struct SpriteTemplate sSpriteTemplate_PokeblockCase =
{
    TAG_POKEBLOCK_CASE,
    TAG_POKEBLOCK_CASE,
    &sOamData_PokeblockCase,
    sSpriteAnimTable_PokeblockCase,
    NULL,
    gDummySpriteAffineAnimTable,
    SpriteCallbackDummy
};

const struct Pokeblock sFavoritePokeblocksTable[] =
{
    { PBLOCK_CLR_RED,      20,  0,  0,  0,  0, 20 },
    { PBLOCK_CLR_BLUE,      0, 20,  0,  0,  0, 20 },
    { PBLOCK_CLR_PINK,      0,  0, 20,  0,  0, 20 },
    { PBLOCK_CLR_GREEN,     0,  0,  0, 20,  0, 20 },
    { PBLOCK_CLR_YELLOW,    0,  0,  0,  0, 20, 20 },
#if DEBUG
    { PBLOCK_CLR_PURPLE,   20,  0, 20,  0,  0, 20 },
    { PBLOCK_CLR_INDIGO,    0, 20,  0, 20,  0, 20 },
    { PBLOCK_CLR_BROWN,     0,  0, 20,  0, 20, 20 },
    { PBLOCK_CLR_LITE_BLUE, 20,  0,  0, 20,  0, 20 },
    { PBLOCK_CLR_OLIVE,     0, 20,  0,  0, 20, 20 },
    { PBLOCK_CLR_GRAY,      0,  2,  0,  2,  2,  0 },
	{ PBLOCK_CLR_BLACK,     3,  3,  3,  4,  3,  0 },
    { PBLOCK_CLR_WHITE,     1,  1,  1,  1,  1,  1 },
    { PBLOCK_CLR_GOLD,     20,  0,  0,  0,  0, 20 },
	{ 0 },
#endif
};

// text

static void CB2_PokeblockMenu(void)
{
    AnimateSprites();
    BuildOamBuffer();
    RunTasks();
    UpdatePaletteFade();
}

static void VBlankCB_PokeblockMenu(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    DmaCopy16Defvars(3, gBGTilemapBuffers[2], (void *)(VRAM + 0x7800), sizeof gBGTilemapBuffers[2]);
}

static bool8 InitPokeblockMenu(void)
{
    u16 ime;
    switch (gMain.state)
    {
        case  0:
            SetVBlankHBlankCallbacksToNull();
            ResetVramOamAndBgCntRegs();
            REG_BG2CNT = BGCNT_SCREENBASE(15) | BGCNT_CHARBASE(2) | BGCNT_PRIORITY(2);
            REG_BLDCNT = 0;
            gMain.state++;
            break;
        case  1:
            ScanlineEffect_Stop();
            gMain.state++;
            break;
        case  2:
            ResetPaletteFade();
            gPaletteFade.bufferTransferDisabled = TRUE;
            gMain.state++;
            break;
        case  3:
            ResetSpriteData();
            gMain.state++;
            break;
        case  4:
            if (sPokeblockCaseContext != PBLOCK_CASE_BATTLE)
            {
                ResetTasks();
            }
            gMain.state++;
            break;
        case  5:
            Text_LoadWindowTemplate(&gPokeblockCaseWindowTemplate);
            gMain.state++;
            break;
        case  6:
            Text_LoadWindowTemplate(&gPokeblockWindowTemplate);
            gMain.state++;
            break;
        case  7:
            MultistepInitMenuWindowBegin(&gPokeblockCaseWindowTemplate);
            gMain.state++;
            break;
        case  8:
            if (MultistepInitMenuWindowContinue())
            {
                gMain.state++;
            }
            break;
        case  9:
            MultistepInitMenuWindowBegin(&gPokeblockWindowTemplate);
            gMain.state++;
            break;
        case 10:
            if (MultistepInitMenuWindowContinue())
            {
                ePokeblockGfxState = 0;
                gMain.state++;
            }
            break;
        case 11:
            if (LoadPokeblockMenuGfx())
            {
                gMain.state++;
            }
            break;
        case 12:
            ResetScrollIndicatorState();
            LoadScrollIndicatorPalette();
            CreateScrollIndicatorArrow(TOP_ARROW, 0xb0, 0x08);
            CreateScrollIndicatorArrow(BOTTOM_ARROW, 0xb0, 0x98);
            gMain.state++;
            break;
        case 13:
            ePokeblockCaseSpriteId = CreatePokeblockCaseSprite(0x38, 0x40, 0);
            gMain.state++;
            break;
        case 14:
            CompactPokeblockSlots();
            SetMenuItemsCountAndMaxShowed();
            gMain.state++;
            break;
        case 15:
            DrawPokeblockMenuTitleText();
            DrawPokeblockInfoLabels();
            UpdatePokeblockList(sPokeblockMenuState.scrollOffset);
            gMain.state++;
            break;
        case 16:
            ime = REG_IME;
            REG_IME = 0;
            REG_IE |= INTR_FLAG_VBLANK;
            REG_IME = ime;
            REG_DISPSTAT |= DISPSTAT_VBLANK_INTR;
            SetVBlankCallback(VBlankCB_PokeblockMenu);
            REG_DISPCNT = DISPCNT_OBJ_ON | DISPCNT_BG2_ON | DISPCNT_BG1_ON | DISPCNT_BG0_ON | DISPCNT_OBJ_1D_MAP;
            gMain.state++;
            break;
        case 17:
            if (Overworld_IsRecvQueueAtMax() != TRUE)
            {
                gMain.state++;
            }
            break;
        case 18:
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
            gPaletteFade.bufferTransferDisabled = FALSE;
            SetMainCallback2(CB2_PokeblockMenu);
            return TRUE;
    }
    return FALSE;
}

void CB2_InitPokeblockMenu(void)
{
    do {
        if (InitPokeblockMenu() == TRUE)
        {
            CreateTask(Task_HandlePokeblockMenuInput, 0);
            break;
        }
    } while (MenuHelpers_IsLinkActive() != TRUE);
}

static bool8 LoadPokeblockMenuGfx(void)
{
    switch (ePokeblockGfxState)
    {
        case 0:
            LZDecompressVram(gMenuPokeblock_Gfx, BG_CHAR_ADDR(2));
            ePokeblockGfxState++;
            break;
        case 1:
            LZDecompressWram(gMenuPokeblock_Tilemap, gBGTilemapBuffers[2]);
            ePokeblockGfxState++;
            break;
        case 2:
            LoadCompressedPalette(gMenuPokeblock_Pal, 0, 0xc0);
            ePokeblockGfxState++;
            break;
        case 3:
            LoadCompressedSpriteSheet(&gPokeblockCase_SpriteSheet);
            ePokeblockGfxState++;
            break;
        case 4:
            LoadCompressedSpritePalette(&gPokeblockCase_SpritePal);
            ePokeblockGfxState = 0;
            return TRUE;
    }
    return FALSE;
}

u8 CreatePokeblockCaseSprite(s16 x, s16 y, u8 subpriority)
{
    return CreateSprite(&sSpriteTemplate_PokeblockCase, x, y, subpriority);
}

void SetPokeblockCaseContext(u8 caseContext)
{
    sPokeblockCaseContext = caseContext;
    switch (sPokeblockCaseContext)
    {
        default:
            sPokeblockActionIds = sActionsOnField;
            sNumPokeblockActions = sizeof sActionsOnField;
            break;
        case PBLOCK_CASE_BATTLE:
            sPokeblockActionIds = sActionsInBattle;
            sNumPokeblockActions = sizeof sActionsInBattle;
            break;
        case PBLOCK_CASE_FEEDER:
            sPokeblockActionIds = sActionsOnPokeblockFeeder;
            sNumPokeblockActions = sizeof sActionsOnPokeblockFeeder;
            break;
    }
}

void OpenPokeblockCaseInBattle(void)
{
    SetPokeblockCaseContext(PBLOCK_CASE_BATTLE);
    SetMainCallback2(CB2_InitPokeblockMenu);
}

void OpenPokeblockCaseOnFeeder(void)
{
    SetPokeblockCaseContext(PBLOCK_CASE_FEEDER);
    SetMainCallback2(CB2_InitPokeblockMenu);
}

#if DEBUG
void Debug_FillPokeblockCase(void)
{
    u8 i;

    for (i = 0; i < POKEBLOCKS_COUNT && sFavoritePokeblocksTable[i].color != PBLOCK_CLR_NONE; i++)
        gSaveBlock1.pokeblocks[i] = sFavoritePokeblocksTable[i];
}
#endif

static void DrawPokeblockMenuTitleText(void)
{
    BasicInitMenuWindow(&gPokeblockCaseWindowTemplate);
    MenuPrint_Centered(GetItemName(ITEM_POKEBLOCK_CASE), 2, 1, 0x48);
}

static void DrawPokeblockInfoLabels(void)
{
    BasicInitMenuWindow(&gPokeblockCaseWindowTemplate);
    Menu_PrintText(gContestStatsText_Spicy,   2, 13);
    Menu_PrintText(gContestStatsText_Dry,     2, 15);
    Menu_PrintText(gContestStatsText_Sweet,   2, 17);
    Menu_PrintText(gContestStatsText_Bitter,  8, 13);
    Menu_PrintText(gContestStatsText_Sour,    8, 15);
}

static void PrintPokeblockList(u8 scrollOffset)
{
    u8 i;
    u8 y;
    u8 *text;
    BasicInitMenuWindow(&gPokeblockCaseWindowTemplate);
    for (i = scrollOffset; i <= scrollOffset + 8; i++)
    {
        y = (i - scrollOffset) << 1;
        if (i == sPokeblockMenuState.itemsNo)
        {
            text = AlignStringInMenuWindow(gStringVar1, gContestStatsText_StowCase, 0x78, 0);
            Menu_PrintText(gStringVar1, 15, y + 1);
            if (i != scrollOffset + 8)
            {
                Menu_EraseWindowRect(15, y + 3, 29, 18);
            }
            break;
        }
        text = AlignStringInMenuWindow(gStringVar1, gPokeblockNames[gSaveBlock1.pokeblocks[i].color], 0x5e, 0);
        text[0] = EXT_CTRL_CODE_BEGIN;
        text[1] = 0x14;
        text[2] = 0x06;
        text += 3;
        ConvertIntToDecimalStringN(text, GetHighestPokeblocksFlavorLevel(&gSaveBlock1.pokeblocks[i]), STR_CONV_MODE_RIGHT_ALIGN, 3);
        Menu_PrintText(gStringVar1, 15, y + 1);
    }
}

static void UpdatePokeblockList(u8 scrollOffset)
{
    PrintPokeblockList(scrollOffset);
    DrawPokeblockInfo(FALSE);
}

static void CompactPokeblockSlots(void)
{
    u16 i, j;
    struct Pokeblock temp;
    for (i = 0; i < POKEBLOCKS_COUNT - 1; i++)
    {
        for (j = i + 1; j < POKEBLOCKS_COUNT; j++)
        {
            if (gSaveBlock1.pokeblocks[i].color == PBLOCK_CLR_NONE)
            {
                temp = gSaveBlock1.pokeblocks[i];
                gSaveBlock1.pokeblocks[i] = gSaveBlock1.pokeblocks[j];
                gSaveBlock1.pokeblocks[j] = temp;
            }
        }
    }
}

static void SetMenuItemsCountAndMaxShowed(void)
{
    u8 i;
    sPokeblockMenuState.itemsNo = 0;
    for (i = 0; i < POKEBLOCKS_COUNT; i++)
    {
        if (gSaveBlock1.pokeblocks[i].color != PBLOCK_CLR_NONE)
            sPokeblockMenuState.itemsNo++;
    }
    if (sPokeblockMenuState.itemsNo < 8)
    {
        sPokeblockMenuState.maxShowed = sPokeblockMenuState.itemsNo;
    }
    else
    {
        sPokeblockMenuState.maxShowed = 8;
    }
    if (sPokeblockMenuState.scrollOffset + 8 > sPokeblockMenuState.itemsNo && sPokeblockMenuState.scrollOffset != 0)
    {
        sPokeblockMenuState.scrollOffset--;
    }
}

static void DrawPokeblockMenuHighlight(u16 tileNum, u16 unusedBottomTileNum)
{
    u8 i;
    int y;
    for (i=0; i<14; i++)
    {
        gBGTilemapBuffers[2][(2 * sPokeblockMenuState.selectedRow + 1) * 32 + (y = i + 15)] = tileNum;
        gBGTilemapBuffers[2][(2 * sPokeblockMenuState.selectedRow + 2) * 32 + y] = tileNum;
    }
}

static void DrawPokeblockInfo(bool8 isSwapSelection)
{
    u8 i;
    u16 tilemapOffset;
    if (!isSwapSelection)
    {
        DrawPokeblockMenuHighlight(0x1005, 0x1014);
    }
    else
    {
        DrawPokeblockMenuHighlight(0x2005, 0x2014);
    }
    if (sPokeblockMenuState.scrollOffset)
    {
        SetScrollIndicatorArrowInvisibility(TOP_ARROW, VISIBLE);
    }
    else
    {
        SetScrollIndicatorArrowInvisibility(TOP_ARROW, INVISIBLE);
    }
    if (sPokeblockMenuState.itemsNo > sPokeblockMenuState.maxShowed && sPokeblockMenuState.scrollOffset + sPokeblockMenuState.maxShowed != sPokeblockMenuState.itemsNo)
    {
        SetScrollIndicatorArrowInvisibility(BOTTOM_ARROW, VISIBLE);
    }
    else
    {
        SetScrollIndicatorArrowInvisibility(BOTTOM_ARROW, INVISIBLE);
    }
    for (i=0; i<5; i++)
    {
        tilemapOffset = ((i % 3) << 6) + 0x1a1 + (i / 3) * 6;
        if (sPokeblockMenuState.selectedRow + sPokeblockMenuState.scrollOffset != sPokeblockMenuState.itemsNo)
        {
            if (GetPokeblockData(&gSaveBlock1.pokeblocks[sPokeblockMenuState.selectedRow + sPokeblockMenuState.scrollOffset], i + 1) > 0)
            {
                gBGTilemapBuffers[2][tilemapOffset] = (i << 12) + 23;
                gBGTilemapBuffers[2][tilemapOffset + 32] = (i << 12) + 24;
            }
            else
            {
                gBGTilemapBuffers[2][tilemapOffset] = 15;
                gBGTilemapBuffers[2][tilemapOffset + 32] = 15;
            }
        }
        else
        {
            gBGTilemapBuffers[2][tilemapOffset] = 15;
            gBGTilemapBuffers[2][tilemapOffset + 32] = 15;
        }
    }
    BasicInitMenuWindow(&gPokeblockCaseWindowTemplate);
    if (sPokeblockMenuState.selectedRow + sPokeblockMenuState.scrollOffset != sPokeblockMenuState.itemsNo)
    {
        AlignInt1InMenuWindow(gStringVar1, GetPokeblocksFeel(&gSaveBlock1.pokeblocks[sPokeblockMenuState.selectedRow + sPokeblockMenuState.scrollOffset]), 16, 1);
        Menu_PrintText(gStringVar1, 11, 17);
    }
    else
    {
        Menu_EraseWindowRect(11, 17, 12, 18);
    }
}

static void OnPokeblockMenuCursorMoved(bool8 isSwapSelection)
{
    PlaySE(SE_SELECT);
    gSprites[ePokeblockCaseSpriteId].callback = SpriteCB_ShakePokeblockCase;
    DrawPokeblockInfo(isSwapSelection);
}

static void Task_HandlePokeblockMenuInput(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        if (JOY_REPT(DPAD_UP))
        {
            if (sPokeblockMenuState.selectedRow != 0)
            {
                DrawPokeblockMenuHighlight(5, 20);
                sPokeblockMenuState.selectedRow--;
                OnPokeblockMenuCursorMoved(FALSE);
            }
            else if (sPokeblockMenuState.scrollOffset != 0)
            {
                sPokeblockMenuState.scrollOffset--;
                PrintPokeblockList(sPokeblockMenuState.scrollOffset);
                OnPokeblockMenuCursorMoved(FALSE);
            }
        }
        else if (JOY_REPT(DPAD_DOWN))
        {
            if (sPokeblockMenuState.selectedRow != sPokeblockMenuState.maxShowed)
            {
                DrawPokeblockMenuHighlight(5, 20);
                sPokeblockMenuState.selectedRow++;
                OnPokeblockMenuCursorMoved(FALSE);
            }
            else if (sPokeblockMenuState.scrollOffset + sPokeblockMenuState.selectedRow != sPokeblockMenuState.itemsNo)
            {
                sPokeblockMenuState.scrollOffset++;
                PrintPokeblockList(sPokeblockMenuState.scrollOffset);
                OnPokeblockMenuCursorMoved(FALSE);
            }
        }
        else if (JOY_NEW(SELECT_BUTTON))
        {
            if (sPokeblockMenuState.scrollOffset + sPokeblockMenuState.selectedRow != sPokeblockMenuState.itemsNo)
            {
                PlaySE(SE_SELECT);
                DrawPokeblockInfo(TRUE);
                gTasks[taskId].tSwapPokeblockId = sPokeblockMenuState.scrollOffset + sPokeblockMenuState.selectedRow;
                gTasks[taskId].func = Task_HandlePokeblocksSwapInput;
            }
        }
        else if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            if (sPokeblockMenuState.scrollOffset + sPokeblockMenuState.selectedRow == sPokeblockMenuState.itemsNo)
            {
                gSpecialVar_Result = 0xffff;
                FadePaletteAndSetTaskToClosePokeblockCase(taskId);
            }
            else
            {
                ShowPokeblockActionsWindow(taskId);
            }
        }
        else if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            gSpecialVar_Result = 0xffff;
            FadePaletteAndSetTaskToClosePokeblockCase(taskId);
        }
    }
}

static void Task_HandlePokeblocksSwapInput(u8 taskId)
{
    if (JOY_REPT(DPAD_UP))
    {
        if (sPokeblockMenuState.selectedRow != 0)
        {
            DrawPokeblockMenuHighlight(5, 20);
            sPokeblockMenuState.selectedRow--;
            OnPokeblockMenuCursorMoved(TRUE);
            DrawPokeblockSwapSelection(taskId, 1);
        }
        else if (sPokeblockMenuState.scrollOffset != 0)
        {
            DrawPokeblockSwapSelection(taskId, 0);
            sPokeblockMenuState.scrollOffset--;
            PrintPokeblockList(sPokeblockMenuState.scrollOffset);
            OnPokeblockMenuCursorMoved(TRUE);
            DrawPokeblockSwapSelection(taskId, 1);
        }
    }
    else if (JOY_REPT(DPAD_DOWN))
    {
        if (sPokeblockMenuState.selectedRow != sPokeblockMenuState.maxShowed)
        {
            DrawPokeblockMenuHighlight(5, 20);
            sPokeblockMenuState.selectedRow++;
            OnPokeblockMenuCursorMoved(TRUE);
            DrawPokeblockSwapSelection(taskId, 1);
        }
        else if (sPokeblockMenuState.scrollOffset + sPokeblockMenuState.selectedRow != sPokeblockMenuState.itemsNo)
        {
            DrawPokeblockSwapSelection(taskId, 0);
            sPokeblockMenuState.scrollOffset++;
            PrintPokeblockList(sPokeblockMenuState.scrollOffset);
            OnPokeblockMenuCursorMoved(TRUE);
            DrawPokeblockSwapSelection(taskId, 1);
        }
    }
    else if (JOY_NEW(A_BUTTON) || JOY_NEW(SELECT_BUTTON))
    {
        PlaySE(SE_SELECT);
        DrawPokeblockSwapSelection(taskId, 0);
        SwapPokeblockMenuItems(taskId);
        gTasks[taskId].func = Task_HandlePokeblockMenuInput;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        DrawPokeblockSwapSelection(taskId, 0);
        DrawPokeblockInfo(0);
        gTasks[taskId].func = Task_HandlePokeblockMenuInput;
    }
}

static void DrawPokeblockSwapSelection(u8 taskId, bool8 isSelected)
{
    u8 i;
    u32 tileX;
    s16 listRow;
    u16 tileNum = 0x1005;
    if (!isSelected)
    {
        tileNum = 0x0005;
    }
    listRow = gTasks[taskId].tSwapPokeblockId - sPokeblockMenuState.scrollOffset;
    if ((u16)listRow <= 8 && listRow != sPokeblockMenuState.selectedRow)
    {
        for (i=0; i<14; i++)
        {
            gBGTilemapBuffers[2][(2 * listRow + 1) * 32 + (tileX = i + 15)] = tileNum;
            gBGTilemapBuffers[2][(2 * listRow + 2) * 32 + tileX] = tileNum;
        }
    }
}

static void SwapPokeblockMenuItems(u8 taskId)
{
    struct Pokeblock temp;
    u8 selectedPokeblockId = sPokeblockMenuState.scrollOffset + sPokeblockMenuState.selectedRow;
    if (selectedPokeblockId == sPokeblockMenuState.itemsNo)
    {
        DrawPokeblockInfo(FALSE);
    }
    else
    {
        temp = gSaveBlock1.pokeblocks[selectedPokeblockId];
        gSaveBlock1.pokeblocks[selectedPokeblockId] = gSaveBlock1.pokeblocks[gTasks[taskId].tSwapPokeblockId];
        gSaveBlock1.pokeblocks[gTasks[taskId].tSwapPokeblockId] = temp;
        PrintPokeblockList(sPokeblockMenuState.scrollOffset);
        DrawPokeblockInfo(FALSE);
    }
}

static void FreePokeblockMenuResources(void)
{
    DestroyScrollIndicatorArrow(TOP_ARROW);
    DestroyScrollIndicatorArrow(BOTTOM_ARROW);
    FreeScrollIndicatorPalette();
}

static void Task_FreeDataAndExitPokeblockCase(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        if (sPokeblockCaseContext == PBLOCK_CASE_FEEDER)
        {
            gFieldCallback = FieldCB_ContinueScriptHandleMusic;
        }
        FreePokeblockMenuResources();
        SetMainCallback2(sPokeblockCaseExitCallbacks[sPokeblockCaseContext]);
        DestroyTask(taskId);
    }
}

static void FadePaletteAndSetTaskToClosePokeblockCase(u8 taskId)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    if (sPokeblockCaseContext > PBLOCK_CASE_FIELD)
    {
        gSpecialVar_ItemId = ITEM_NONE;
    }
    gTasks[taskId].func = Task_FreeDataAndExitPokeblockCase;
}

static void ShowPokeblockActionsWindow(u8 taskId)
{
    int menuYOffset = 0;
    if (sPokeblockCaseContext > PBLOCK_CASE_FIELD)
        menuYOffset = 2;
    StopScrollIndicatorArrow(TOP_ARROW);
    StopScrollIndicatorArrow(BOTTOM_ARROW);
    BasicInitMenuWindow(&gPokeblockWindowTemplate);
    Menu_DrawStdWindowFrame(7, menuYOffset + 4, 13, 11);
    Menu_PrintItemsReordered(8, menuYOffset + 5, sNumPokeblockActions, sPokeblockMenuActions, sPokeblockActionIds);
    InitMenu(0, 8, menuYOffset + 5, sNumPokeblockActions, 0, 5);
    gSpecialVar_ItemId = sPokeblockMenuState.selectedRow + sPokeblockMenuState.scrollOffset;
    gTasks[taskId].func = Task_HandlePokeblockActionsInput;
}

static void Task_HandlePokeblockActionsInput(u8 taskId)
{
    if (JOY_REPT(DPAD_UP))
    {
        if (Menu_GetCursorPos())
        {
            PlaySE(SE_SELECT);
            Menu_MoveCursor(-1);
        }
    }
    else if (JOY_REPT(DPAD_DOWN))
    {
        if (Menu_GetCursorPos() != sNumPokeblockActions - 1)
        {
            PlaySE(SE_SELECT);
            Menu_MoveCursor(+1);
        }
    }
    else if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        sPokeblockMenuActions[sPokeblockActionIds[Menu_GetCursorPos()]].func(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        PokeblockAction_Cancel(taskId);
    }
}

static void Task_OpenGivePokeblockPartyMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        FreePokeblockMenuResources();
        ChooseMonToGivePokeblock(&gSaveBlock1.pokeblocks[gSpecialVar_ItemId], CB2_InitPokeblockMenu);
        DestroyTask(taskId);
    }
}

static void PokeblockAction_UseOnField(u8 taskId)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = Task_OpenGivePokeblockPartyMenu;
}

static void ShowTossPokeblockPrompt(u8 taskId)
{
    BasicInitMenuWindow(&gPokeblockWindowTemplate);
    Menu_DestroyCursor();
    Menu_EraseWindowRect(7, 4, 13, 11);
    StringCopy(gStringVar1, gPokeblockNames[gSaveBlock1.pokeblocks[sPokeblockMenuState.selectedRow + sPokeblockMenuState.scrollOffset].color]);
    StringExpandPlaceholders(gStringVar4, gContestStatsText_ThrowAwayPrompt);
    DisplayItemMessageOnField(taskId, gStringVar4, CreateTossPokeblockYesNoMenu, 0);
}

static void PokeblockAction_Toss(u8 taskId)
{
    SetScrollIndicatorArrowInvisibility(BOTTOM_ARROW, INVISIBLE);
    gTasks[taskId].func = ShowTossPokeblockPrompt;
}

static void CreateTossPokeblockYesNoMenu(u8 taskId)
{
    DisplayYesNoMenu(7, 6, 1);
    DoYesNoFuncWithChoice(taskId, &sTossYesNoFuncTable);
}

static void TossedPokeblockMessage(u8 taskId)
{
    Menu_EraseWindowRect(7, 6, 13, 11);
    TryClearPokeblock(sPokeblockMenuState.selectedRow + sPokeblockMenuState.scrollOffset);
    StringExpandPlaceholders(gStringVar4, gContestStatsText_WasThrownAway);
    DisplayItemMessageOnField(taskId, gStringVar4, RefreshPokeblockListAfterToss, 0);
    CompactPokeblockSlots();
    SetMenuItemsCountAndMaxShowed();
}

static void CloseTossPokeblockWindow(u8 taskId)
{
    StartScrollIndicatorArrow(TOP_ARROW);
    StartScrollIndicatorArrow(BOTTOM_ARROW);
    if (sPokeblockMenuState.itemsNo > sPokeblockMenuState.maxShowed && sPokeblockMenuState.scrollOffset + sPokeblockMenuState.maxShowed != sPokeblockMenuState.itemsNo)
    {
        SetScrollIndicatorArrowInvisibility(BOTTOM_ARROW, VISIBLE);
    }
    BasicInitMenuWindow(&gPokeblockWindowTemplate);
    Menu_EraseWindowRect(7, 6, 13, 11);
    Menu_EraseWindowRect(0, 14, 29, 19);
    gTasks[taskId].func = Task_HandlePokeblockMenuInput;
}

static void Task_WaitTossMessageDismissal(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        CloseTossPokeblockWindow(taskId);
    }
}

static void RefreshPokeblockListAfterToss(u8 taskId)
{
    BasicInitMenuWindow(&gPokeblockCaseWindowTemplate);
    UpdatePokeblockList(sPokeblockMenuState.scrollOffset);
    SetScrollIndicatorArrowInvisibility(BOTTOM_ARROW, INVISIBLE);
    gTasks[taskId].func = Task_WaitTossMessageDismissal;
}

static void PokeblockAction_Cancel(u8 taskId)
{
    StartScrollIndicatorArrow(TOP_ARROW);
    StartScrollIndicatorArrow(BOTTOM_ARROW);
    Menu_DestroyCursor();
    Menu_EraseWindowRect(7, 4, 13, 11);
    gTasks[taskId].func = Task_HandlePokeblockMenuInput;
}

static void PokeblockAction_UseInBattle(u8 taskId)
{
    s16 gain = PokeblockGetGain(GetNature(&gEnemyParty[0]), &gSaveBlock1.pokeblocks[gSpecialVar_ItemId]);
    StringCopy(gBattleTextBuff1, gPokeblockNames[gSaveBlock1.pokeblocks[gSpecialVar_ItemId].color]);
    TryClearPokeblock(gSpecialVar_ItemId);
    gSpecialVar_ItemId = gSaveBlock1.pokeblocks[gSpecialVar_ItemId].color << 8;
    if (gain == 0)
    {
        gSpecialVar_ItemId += 1;
    }
    if (gain > 0)
    {
        gSpecialVar_ItemId += 2;
    }
    if (gain < 0)
    {
        gSpecialVar_ItemId += 3;
    }
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = Task_FreeDataAndExitPokeblockCase;
}

static void PokeblockAction_UseOnPokeblockFeeder(u8 taskId)
{
    SafariZoneActivatePokeblockFeeder(gSpecialVar_ItemId);
    StringCopy(gStringVar1, gPokeblockNames[gSaveBlock1.pokeblocks[gSpecialVar_ItemId].color]);
    gSpecialVar_Result = gSpecialVar_ItemId;
    TryClearPokeblock(gSpecialVar_ItemId);
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = Task_FreeDataAndExitPokeblockCase;
}

static void SpriteCB_ShakePokeblockCase(struct Sprite *sprite)
{
    if (sprite->data[0] > 1)
    {
        sprite->data[0] = 0;
    }
    switch (sprite->data[0])
    {
        case 0:
            sprite->oam.affineMode = 1;
            sprite->affineAnims = sAffineAnims_PokeblockCaseShake;
            InitSpriteAffineAnim(sprite);
            sprite->data[0] = 1;
            sprite->data[1] = 0;
            break;
        case 1:
            if (++sprite->data[1] > 11)
            {
                sprite->oam.affineMode = 0;
                sprite->data[0] = 0;
                sprite->data[1] = 0;
                FreeOamMatrix(sprite->oam.matrixNum);
                sprite->callback = SpriteCallbackDummy;
            }
            break;
    }
}

static void ClearPokeblock(u8 pokeblockIdx)
{
    gSaveBlock1.pokeblocks[pokeblockIdx].color  = PBLOCK_CLR_NONE;
    gSaveBlock1.pokeblocks[pokeblockIdx].spicy  = 0;
    gSaveBlock1.pokeblocks[pokeblockIdx].dry    = 0;
    gSaveBlock1.pokeblocks[pokeblockIdx].sweet  = 0;
    gSaveBlock1.pokeblocks[pokeblockIdx].bitter = 0;
    gSaveBlock1.pokeblocks[pokeblockIdx].sour   = 0;
    gSaveBlock1.pokeblocks[pokeblockIdx].feel   = 0;
}

void ClearPokeblocks(void)
{
    u8 pokeblockIdx;
    for (pokeblockIdx=0; pokeblockIdx<ARRAY_COUNT(gSaveBlock1.pokeblocks); pokeblockIdx++)
    {
        ClearPokeblock(pokeblockIdx);
    }
}

u8 GetHighestPokeblocksFlavorLevel(const struct Pokeblock *pokeblock)
{
    u8 contestStat;
    u8 maxRating;
    u8 rating = GetPokeblockData(pokeblock, 1);
    for (contestStat = PBLOCK_SPICY; contestStat < FLAVOR_COUNT; contestStat++)
    {
        maxRating = GetPokeblockData(pokeblock, contestStat + 1);
        if (rating < maxRating)
        {
            rating = maxRating;
        }
    }
    return rating;
}

u8 GetPokeblocksFeel(const struct Pokeblock *pokeblock)
{
    u8 feel = GetPokeblockData(pokeblock, 6);
    if (feel > POKEBLOCK_MAX_FEEL)
        feel = POKEBLOCK_MAX_FEEL;
    return feel;
}

s8 GetFirstFreePokeblockSlot(void)
{
    u8 i;
    for (i = 0; i < ARRAY_COUNT(gSaveBlock1.pokeblocks); i++)
    {
        if (gSaveBlock1.pokeblocks[i].color == PBLOCK_CLR_NONE)
        {
            return i;
        }
    }
    return -1;
}

bool8 AddPokeblock(const struct Pokeblock *pokeblock)
{
    s8 slot = GetFirstFreePokeblockSlot();
    if (slot == -1)
    {
        return FALSE;
    }
    gSaveBlock1.pokeblocks[slot] = *pokeblock;
    return TRUE;
}

bool8 TryClearPokeblock(u8 pokeblockIdx)
{
    if (gSaveBlock1.pokeblocks[pokeblockIdx].color == PBLOCK_CLR_NONE)
    {
        return FALSE;
    }
    ClearPokeblock(pokeblockIdx);
    return TRUE;
}

s16 GetPokeblockData(const struct Pokeblock *pokeblock, u8 field)
{
    if (field == PBLOCK_COLOR)
        return pokeblock->color;
    if (field == PBLOCK_SPICY)
        return pokeblock->spicy;
    if (field == PBLOCK_DRY)
        return pokeblock->dry;
    if (field == PBLOCK_SWEET)
        return pokeblock->sweet;
    if (field == PBLOCK_BITTER)
        return pokeblock->bitter;
    if (field == PBLOCK_SOUR)
        return pokeblock->sour;
    if (field == PBLOCK_FEEL)
        return pokeblock->feel;
    return 0;
}

s16 PokeblockGetGain(u8 nature, const struct Pokeblock *pokeblock)
{
    u8 flavor;
    s16 curGain;
    s16 totalGain = 0;
    for (flavor=0; flavor<5; flavor++)
    {
        curGain = GetPokeblockData(pokeblock, flavor + 1);
        if (curGain > 0)
        {
            totalGain += curGain * gPokeblockFlavorCompatibilityTable[5 * nature + flavor];
        }
    }
    return totalGain;
}

void PokeblockCopyName(const struct Pokeblock *pokeblock, u8 *dest)
{
    u8 color = GetPokeblockData(pokeblock, PBLOCK_COLOR);
    StringCopy(dest, gPokeblockNames[color]);
}

bool8 CopyMonFavoritePokeblockName(u8 nature, u8 *dest)
{
    u8 flavor;
    for (flavor = 0; flavor < FLAVOR_COUNT; flavor++)
    {
        if (PokeblockGetGain(nature, &sFavoritePokeblocksTable[flavor]) > 0)
        {
            StringCopy(dest, gPokeblockNames[flavor + 1]);
            return TRUE;
        }
    }
    return FALSE;
}
