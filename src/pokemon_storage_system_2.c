
// Includes
#include "global.h"
#include "decompress.h"
#include "constants/songs.h"
#include "constants/species.h"
#include "data2.h"
#include "sound.h"
#include "ewram.h"
#include "task.h"
#include "main.h"
#include "palette.h"
#include "menu.h"
#include "graphics.h"
#include "strings.h"
#include "string_util.h"
#include "pokemon_summary_screen.h"
#include "mail_data.h"
#include "naming_screen.h"
#include "pokemon_storage_system.h"

// Static type declarations

// Static RAM declarations

EWRAM_DATA u8 gPokeStorageInPartyMenu = 0;
EWRAM_DATA u8 gPokeStorageBoxOption = 0;
EWRAM_DATA u8 gPokeStorageDepositBoxId = 0;
EWRAM_DATA u8 gPokeStorageScreenToReshow = 0;

// Static ROM declarations

static void CB2_InitPokeStorage(void);
static void VBlankCB_PokeStorage(void);
static void CB2_PokeStorage(void);
static void SetPokeStorageCallback(void (*func)(void));
static void Task_ShowPokeStorage(void);
static void Task_ReshowPokeStorage(void);
static void Task_PokeStorageMain(void);
static void Task_ShowPartyPokemon(void);
static void Task_HidePartyPokemon(void);
static void Task_OnSelectedMon(void);
static void Task_MoveMon(void);
static void Task_PlaceMon(void);
static void Task_ShiftMon(void);
static void Task_WithdrawMon(void);
static void Task_DepositMenu(void);
static void Task_ReleaseMon(void);
static void Task_ShowMonSummary(void);
static void Task_ShowMarkMenu(void);
static void Task_HandleMovingMonFromParty(void);
static void Task_HandleBoxOptions(void);
static void Task_HandleWallpapers(void);
static void Task_JumpBox(void);
static void Task_NameBox(void);
static void Task_OnCloseBoxPressed(void);
static void Task_OnBPressed(void);
static void SetScrollingBackground(void);
static void ScrollBackground(void);
static void InitPalettesAndSprites(void);
static void CreateMarkingComboSprite(void);
static void CreateWaveformSprites(void);
static void RefreshDisplayMonData(void);
static void StartDisplayMonMosaicEffect(void);
static void SpriteCB_DisplayMonMosaic(struct Sprite *sprite);
static void CreateDisplayMonSprite(void);
static void LoadDisplayMonGfx(u16 species, u32 pid);
static void PrintDisplayMonInfo(void);
static void UpdateWaveformAnimation(void);
static void InitSupplementalTilemaps(void);
static void SetUpHidePartyMenu(void);
static void SetPartySlotTilemaps(void);
static void UpdateCloseBoxButtonTilemap(bool8 flag);
static bool8 HidePartyMenu(void);
static void StartFlashingCloseBoxButton(void);
static void StopFlashingCloseBoxButton(void);
static void UpdateCloseBoxButtonFlash(void);
static void UpdatePartySlotColors(void);
static void SetUpDoShowPartyMenu(void);
static bool8 DoShowPartyMenu(void);
static void PrintMessage(u8 index);
static void ShowYesNoWindow(s8);
static void ClearBottomWindow(void);
static void AddWallpaperSetsMenu(void);
static void AddWallpapersMenu(u8 a0);

// .rodata

const u16 gPokemonStorageScrollingBGPalette[] = INCBIN_U16("graphics/pokemon_storage/scrolling_bg.gbapal");

const u16 gPokemonStorageScrollingBGTile[] = INCBIN_U16("graphics/pokemon_storage/scrolling_bg.4bpp");

const u8 gPokemonStorageScrollingBGTilemap[] = INCBIN_U8("graphics/pokemon_storage/scrolling_bg_map.bin.lz");

const u16 sUnknownTilemap[] = {
    0x1281,
    0x1282,
    0x1283,
    0x1284,
    0x1285,
    0x1286,
    0x1287,
    0x1288,
    0x128C,
    0x128D,
    0x128E,
    0x128F,
    0x1290,
    0x1291,
    0x1292,
    0x1293,
    0x0281,
    0x0282,
    0x0283,
    0x0284,
    0x0285,
    0x0286,
    0x0287,
    0x0288,
    0x028C,
    0x028D,
    0x028E,
    0x028F,
    0x0290,
    0x0291,
    0x0292,
    0x0293,
    0x12AD,
    0x12AE,
    0x12A8,
    0x12A8
};

const u16 WaveformPalette[] = INCBIN_U16("graphics/pokemon_storage/waveform.gbapal");

const u8 WaveformTiles[] = INCBIN_U8("graphics/pokemon_storage/waveform.4bpp");

const u16 gUnknown_083B6D74[] = INCBIN_U16("graphics/pokemon_storage/83B6D74.gbapal");

const u16 gUnknown_083B6D94[] = INCBIN_U16("graphics/pokemon_storage/83B6D94.gbapal");

struct PokemonStorageSystemData *const gPokemonStorageSystemPtr = &ePokemonStorageSystem;

u8 *const gPokeStorageTileBuffer = gTileBuffer;

static const struct SpritePalette gWaveformSpritePalette = {
    WaveformPalette, 0xdacd
};

// .text

void EnterPokeStorage(u8 whichMenu)
{
    gPokeStorageBoxOption = whichMenu;
    gPokemonStorageSystemPtr->boxOption = whichMenu;
    SetMainCallback2(CB2_InitPokeStorage);
}

static void ResetAllBgCoords(void)
{
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;
    REG_BG1HOFS = 0;
    REG_BG1VOFS = 0;
    REG_BG2VOFS = 0;
    REG_BG2HOFS = 0;
    REG_BG3HOFS = 0;
    REG_BG3VOFS = 0;
}

static void ResetForPokeStorage(void)
{
    ResetPaletteFade();
    ResetSpriteData();
    FreeSpriteTileRanges();
    FreeAllSpritePalettes();
    ResetTasks();
    gReservedSpriteTileCount = 0x280;
    UnkUtil_Init(&gPokemonStorageSystemPtr->unkUtil, gPokemonStorageSystemPtr->unkUtilData, 8);
    gKeyRepeatStartDelay = 20;
}

static void InitStartingPosData(void)
{
    ClearSavedCursorPos();
    gPokeStorageInPartyMenu = (gPokemonStorageSystemPtr->boxOption == PSS_OPTION_DEPOSIT ? TRUE : FALSE);
    gPokeStorageDepositBoxId = 0;
}

static void SetMonIconTransparency(void)
{
    REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG_ALL_ON | DISPCNT_OBJ_ON;
}

extern u8 gDebugPokeStorageActive;

static void CB2_InitPokeStorage(void)
{
    switch (gMain.state)
    {
    case 0:
        SetVBlankCallback(NULL);
        REG_DISPCNT = 0;
        ResetForPokeStorage();
#if DEBUG
        gDebugPokeStorageActive = 0;
#endif
        gMain.state++;
        break;
    case 1:
        Text_LoadWindowTemplate(&gWindowTemplate_81E6D00);
        gMain.state++;
        break;
    case 2:
        InitMenuWindow(&gWindowTemplate_81E6D00);
        Menu_EraseScreen();
        gMain.state++;
        break;
    case 3:
        ResetAllBgCoords();
        InitStartingPosData();
        gMain.state++;
        break;
    case 4:
        InitMonIconFields();
        InitCursor();
        gMain.state++;
        break;
    case 5:
        SetScrollingBackground();
        gMain.state++;
        break;
    case 6:
        InitPalettesAndSprites();
        gMain.state++;
        break;
    case 7:
        InitSupplementalTilemaps();
        gMain.state++;
        break;
    case 8:
        InitPokeStorageBox(gPokemonStorage.currentBox);
        gPokemonStorageSystemPtr->markMenu.baseTileTag = 0x000a;
        gPokemonStorageSystemPtr->markMenu.basePaletteTag = 0xdacb;
        InitMonMarkingsMenu(&gPokemonStorageSystemPtr->markMenu);
        BufferMonMarkingsMenuTiles();
        gMain.state++;
        break;
    case 9:
        SetMonIconTransparency();
        SetPokeStorageCallback(Task_ShowPokeStorage);
        SetMainCallback2(CB2_PokeStorage);
        SetVBlankCallback(VBlankCB_PokeStorage);
        gMain.state++;
        break;
    }
}

static void CB2_ReturnToPokeStorage(void)
{
    switch (gMain.state)
    {
    case 0:
        SetVBlankCallback(NULL);
        REG_DISPCNT = 0;
        gPokemonStorageSystemPtr->boxOption = gPokeStorageBoxOption;
        ResetForPokeStorage();
        if (gPokeStorageScreenToReshow == 1)
            LoadSavedMovingMon();
        if (gPokeStorageScreenToReshow == 0)
            SetSelectionAfterSummaryScreen();
        gMain.state++;
        break;
    case 1:
        Text_LoadWindowTemplate(&gWindowTemplate_81E6D00);
        gMain.state++;
        break;
    case 2:
        InitMenuWindow(&gWindowTemplate_81E6D00);
        Menu_EraseScreen();
        gMain.state++;
        break;
    case 3:
        ResetAllBgCoords();
        gMain.state++;
        break;
    case 4:
        InitMonIconFields();
        InitCursorOnReopen();
        gMain.state++;
        break;
    case 5:
        SetScrollingBackground();
        gMain.state++;
        break;
    case 6:
        InitPalettesAndSprites();
        gMain.state++;
        break;
    case 7:
        InitSupplementalTilemaps();
        gMain.state++;
        break;
    case 8:
        InitPokeStorageBox(gPokemonStorage.currentBox);
        gPokemonStorageSystemPtr->markMenu.baseTileTag = 0x000a;
        gPokemonStorageSystemPtr->markMenu.basePaletteTag = 0xdacb;
        InitMonMarkingsMenu(&gPokemonStorageSystemPtr->markMenu);
        BufferMonMarkingsMenuTiles();
        gMain.state++;
        break;
    case 9:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        SetVBlankCallback(VBlankCB_PokeStorage);
        gMain.state++;
        break;
    case 10:
        SetMonIconTransparency();
        SetPokeStorageCallback(Task_ReshowPokeStorage);
        SetMainCallback2(CB2_PokeStorage);
        gMain.state++;
        break;
    }
}

#if DEBUG

static void Debug_InitPokeStorage(void)
{
    gPokeStorageBoxOption = 0;
    gPokemonStorageSystemPtr->boxOption = PSS_OPTION_WITHDRAW;
    CB2_InitPokeStorage();
    if (gMain.callback2 == CB2_PokeStorage)
        gDebugPokeStorageActive = 1;
}

extern void (*gDebugPokeStorageReturnCallback)(void);
extern struct Pokemon * gDebugPokeStorageMon;

extern void CopyCursorMonToDebugMon(struct Pokemon *);

void Debug_OpenPokeStorageForMon(struct Pokemon *mon, void (*returnCallback)(void))
{
    gDebugPokeStorageReturnCallback = returnCallback;
    gDebugPokeStorageMon = mon;
    SetMainCallback2(Debug_InitPokeStorage);
}

static void Debug_ReturnFromPokeStorage(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        CopyCursorMonToDebugMon(gDebugPokeStorageMon);
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        gPokemonStorageSystemPtr->state++;
        break;
    case 1:
        if (!UpdatePaletteFade())
            gDebugPokeStorageReturnCallback();
        break;
    }
}

#endif

static void VBlankCB_PokeStorage(void)
{
    REG_BG2HOFS = gPokemonStorageSystemPtr->bg2_X;
    REG_BG3HOFS = gPokemonStorageSystemPtr->bg3_X;
    REG_BG3VOFS = gPokemonStorageSystemPtr->bg3_Y;
    LoadOam();
    ProcessSpriteCopyRequests();
    UnkUtil_Run();
    TransferPlttBuffer();
}

static void CB2_PokeStorage(void)
{
    gPokemonStorageSystemPtr->callback();
    ScrollBackground();
    UpdateCloseBoxButtonFlash();
    AnimateSprites();
    BuildOamBuffer();
}

static void CB2_ChangePokeStorageScreen(void)
{
    switch (gPokemonStorageSystemPtr->screenChangeType)
    {
    case 0:
        ShowPokemonSummaryScreen(gPokemonStorageSystemPtr->summaryMon.pokemon, gPokemonStorageSystemPtr->summaryStartPos, gPokemonStorageSystemPtr->summaryMaxPos, CB2_ReturnToPokeStorage, gPokemonStorageSystemPtr->summaryScreenMode);
        break;
    case 1:
        DoNamingScreen(NAMING_SCREEN_BOX, gPokemonStorage.boxNames[gPokemonStorage.currentBox], 0, 0, 0, CB2_ReturnToPokeStorage);
        break;
    }
}

static void SetPokeStorageCallback(void (*func)(void))
{
    gPokemonStorageSystemPtr->callback = func;
    gPokemonStorageSystemPtr->state = 0;
}

static void Task_ShowPokeStorage(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        BlendPalettes(0xFFFFFFFF, 16, RGB(0, 0, 0));
        gPokemonStorageSystemPtr->state++;
        break;
    case 1:
        PlaySE(SE_PC_LOGIN);
        gPokemonStorageSystemPtr->pcScreenEffect.tileTag = 14;
        gPokemonStorageSystemPtr->pcScreenEffect.paletteTag = 0xdad0;
        gPokemonStorageSystemPtr->pcScreenEffect.spriteSpeed = 0;
        gPokemonStorageSystemPtr->pcScreenEffect.revealSpeed = 0;
        StartPCScreenOpenEffect(&gPokemonStorageSystemPtr->pcScreenEffect);
        BlendPalettes(0xFFFFFFFF, 0, RGB(0, 0, 0));
        gPokemonStorageSystemPtr->state++;
        break;
    case 2:
        if (UpdatePCScreenOpenEffect())
            SetPokeStorageCallback(Task_PokeStorageMain);
        break;
    }
}

static void Task_ReshowPokeStorage(void)
{
    if (!UpdatePaletteFade())
        SetPokeStorageCallback(Task_PokeStorageMain);
}

static void Task_PokeStorageMain(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        switch (HandlePokeStorageInput())
        {
        case PSS_INPUT_MOVE_CURSOR:
            PlaySE(SE_SELECT);
            gPokemonStorageSystemPtr->state = 1;
            break;
        case PSS_INPUT_SHOW_PARTY:
            if (gPokemonStorageSystemPtr->boxOption != PSS_OPTION_MOVE_MONS)
            {
                PrintMessage(PC_TEXT_WHICH_ONE_WILL_TAKE);
                gPokemonStorageSystemPtr->state = 3;
            }
            else
            {
                ClearSavedCursorPos();
                SetPokeStorageCallback(Task_ShowPartyPokemon);
            }
            break;
        case PSS_INPUT_HIDE_PARTY:
            if (gPokemonStorageSystemPtr->boxOption == PSS_OPTION_MOVE_MONS)
            {
                if (IsMonBeingMoved() && ItemIsMail(gPokemonStorageSystemPtr->displayMonItemId))
                    gPokemonStorageSystemPtr->state = 5;
                else
                    SetPokeStorageCallback(Task_HidePartyPokemon);
            }
            break;
        case PSS_INPUT_CLOSE_BOX:
#if DEBUG
            if (gDebugPokeStorageActive != 0)
                break;
#endif
            SetPokeStorageCallback(Task_OnCloseBoxPressed);
            break;
        case PSS_INPUT_PRESSED_B:
#if DEBUG
            if (gDebugPokeStorageActive != 0)
                break;
#endif
            SetPokeStorageCallback(Task_OnBPressed);
            break;
        case PSS_INPUT_BOX_OPTIONS:
            PlaySE(SE_SELECT);
            SetPokeStorageCallback(Task_HandleBoxOptions);
            break;
        case PSS_INPUT_IN_MENU:
            PlaySE(SE_SELECT);
            SetPokeStorageCallback(Task_OnSelectedMon);
            break;
        case PSS_INPUT_SCROLL_RIGHT:
            PlaySE(SE_SELECT);
            gPokemonStorageSystemPtr->newCurrBoxId = gPokemonStorage.currentBox + 1;
            if (gPokemonStorageSystemPtr->newCurrBoxId > 13)
                gPokemonStorageSystemPtr->newCurrBoxId = 0;
            SetUpScrollToBox(gPokemonStorageSystemPtr->newCurrBoxId);
            gPokemonStorageSystemPtr->state = 2;
            break;
        case PSS_INPUT_SCROLL_LEFT:
            PlaySE(SE_SELECT);
            gPokemonStorageSystemPtr->newCurrBoxId = gPokemonStorage.currentBox - 1;
            if (gPokemonStorageSystemPtr->newCurrBoxId < 0)
                gPokemonStorageSystemPtr->newCurrBoxId = 13;
            SetUpScrollToBox(gPokemonStorageSystemPtr->newCurrBoxId);
            gPokemonStorageSystemPtr->state = 2;
            break;
        case PSS_INPUT_DEPOSIT:
            if (!IsRemovingLastPartyMon())
            {
                if (ItemIsMail(gPokemonStorageSystemPtr->displayMonItemId))
                {
                    gPokemonStorageSystemPtr->state = 5;
                }
                else
                {
                    PlaySE(SE_SELECT);
                    SetPokeStorageCallback(Task_DepositMenu);
                }
            }
            else
            {
                gPokemonStorageSystemPtr->state = 4;
            }
            break;
        case PSS_INPUT_MOVE_MON:
            if (IsRemovingLastPartyMon())
            {
                gPokemonStorageSystemPtr->state = 4;
            }
            else
            {
                PlaySE(SE_SELECT);
                SetPokeStorageCallback(Task_MoveMon);
            }
            break;
        case PSS_INPUT_SHIFT_MON:
            if (!CanShiftMon())
            {
                gPokemonStorageSystemPtr->state = 4;
            }
            else
            {
                PlaySE(SE_SELECT);
                SetPokeStorageCallback(Task_ShiftMon);
            }
            break;
        case PSS_INPUT_WITHDRAW:
            PlaySE(SE_SELECT);
            SetPokeStorageCallback(Task_WithdrawMon);
            break;
        case PSS_INPUT_PLACE_MON:
            PlaySE(SE_SELECT);
            SetPokeStorageCallback(Task_PlaceMon);
            break;
        }
        break;
    case 1:
        if (!UpdateCursorPos())
        {
            if (IsCursorOnCloseBox())
                StartFlashingCloseBoxButton();
            else
                StopFlashingCloseBoxButton();
            if (gPokemonStorageSystemPtr->setMosaic)
                StartDisplayMonMosaicEffect();
            gPokemonStorageSystemPtr->state = 0;
        }
        break;
    case 2:
        if (!ScrollToBox())
        {
            gPokemonStorage.currentBox = gPokemonStorageSystemPtr->newCurrBoxId;
            if (!gPokeStorageInPartyMenu && !IsMonBeingMoved())
            {
                RefreshDisplayMon();
                StartDisplayMonMosaicEffect();
            }
            gPokemonStorageSystemPtr->state = 0;
        }
        break;
    case 3:
        if (JOY_NEW(A_BUTTON | B_BUTTON | DPAD_ANY))
        {
            ClearBottomWindow();
            gPokemonStorageSystemPtr->state = 0;
        }
        break;
    case 4:
        PlaySE(SE_FAILURE);
        PrintMessage(PC_TEXT_LAST_POKE);
        gPokemonStorageSystemPtr->state = 6;
        break;
    case 5:
        PlaySE(SE_FAILURE);
        PrintMessage(PC_TEXT_PLEASE_REMOVE_MAIL);
        gPokemonStorageSystemPtr->state = 6;
        break;
    case 6:
        if (JOY_NEW(A_BUTTON | B_BUTTON | DPAD_ANY))
        {
            ClearBottomWindow();
            SetPokeStorageCallback(Task_PokeStorageMain);
        }
        break;
    }
}

static void Task_ShowPartyPokemon(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
        case 0:
            SetUpDoShowPartyMenu();
            gPokemonStorageSystemPtr->state++;
            break;
        case 1:
            if (!DoShowPartyMenu())
                SetPokeStorageCallback(Task_PokeStorageMain);
            break;
    }
}

static void Task_HidePartyPokemon(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
        case 0:
            PlaySE(SE_SELECT);
            SetUpHidePartyMenu();
            gPokemonStorageSystemPtr->state++;
            break;
        case 1:
            if (!HidePartyMenu())
            {
                SetCursorBoxPosition(GetSavedCursorPos());
                gPokemonStorageSystemPtr->state++;
            }
            break;
        case 2:
            if (!UpdateCursorPos())
            {
                if (gPokemonStorageSystemPtr->setMosaic)
                    StartDisplayMonMosaicEffect();
                SetPokeStorageCallback(Task_PokeStorageMain);
            }
            break;
    }
}

static void Task_OnSelectedMon(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        PrintMessage(PC_TEXT_IS_SELECTED);
        AddPokeStorageMenu();
        gPokemonStorageSystemPtr->state = 1;
        break;
    case 1:
        switch (HandlePokeStorageMenuInput())
        {
        case -1:
        case PSS_MENU_CANCEL:
            ClearBottomWindow();
            SetPokeStorageCallback(Task_PokeStorageMain);
            break;
        case PSS_MENU_MOVE:
            if (IsRemovingLastPartyMon())
            {
                gPokemonStorageSystemPtr->state = 2;
            }
            else
            {
                PlaySE(SE_SELECT);
                ClearBottomWindow();
                SetPokeStorageCallback(Task_MoveMon);
            }
            break;
        case PSS_MENU_PLACE:
            PlaySE(SE_SELECT);
            ClearBottomWindow();
            SetPokeStorageCallback(Task_PlaceMon);
            break;
        case PSS_MENU_SHIFT:
            if (!CanShiftMon())
            {
                gPokemonStorageSystemPtr->state = 2;
            }
            else
            {
                PlaySE(SE_SELECT);
                ClearBottomWindow();
                SetPokeStorageCallback(Task_ShiftMon);
            }
            break;
        case PSS_MENU_WITHDRAW:
            PlaySE(SE_SELECT);
            ClearBottomWindow();
            SetPokeStorageCallback(Task_WithdrawMon);
            break;
        case PSS_MENU_STORE:
            if (IsRemovingLastPartyMon())
            {
                gPokemonStorageSystemPtr->state = 2;
            }
            else if (ItemIsMail(gPokemonStorageSystemPtr->displayMonItemId))
            {
                gPokemonStorageSystemPtr->state = 3;
            }
            else
            {
                PlaySE(SE_SELECT);
                ClearBottomWindow();
                SetPokeStorageCallback(Task_DepositMenu);
            }
            break;
        case PSS_MENU_RELEASE:
            if (IsRemovingLastPartyMon())
            {
                gPokemonStorageSystemPtr->state = 2;
            }
            else if (gPokemonStorageSystemPtr->displayMonIsEgg)
            {
                gPokemonStorageSystemPtr->state = 4;
            }
            else if (ItemIsMail(gPokemonStorageSystemPtr->displayMonItemId))
            {
                gPokemonStorageSystemPtr->state = 3;
            }
            else
            {
                PlaySE(SE_SELECT);
                SetPokeStorageCallback(Task_ReleaseMon);
            }
            break;
        case PSS_MENU_SUMMARY:
            PlaySE(SE_SELECT);
            SetPokeStorageCallback(Task_ShowMonSummary);
            break;
        case PSS_MENU_MARK:
            PlaySE(SE_SELECT);
            SetPokeStorageCallback(Task_ShowMarkMenu);
            break;
#if DEBUG
        case PSS_MENU_DEBUG_SET:
            PlaySE(SE_SELECT);
            ClearBottomWindow();
            SetPokeStorageCallback(Debug_ReturnFromPokeStorage);
            break;
#endif
        }
        break;
    case 2:
        PlaySE(SE_FAILURE);
        PrintMessage(PC_TEXT_LAST_POKE);
        gPokemonStorageSystemPtr->state = 5;
        break;
    case 4:
        PlaySE(SE_FAILURE);
        PrintMessage(PC_TEXT_CANT_RELEASE_EGG);
        gPokemonStorageSystemPtr->state = 5;
        break;
    case 3:
        PlaySE(SE_FAILURE);
        PrintMessage(PC_TEXT_PLEASE_REMOVE_MAIL);
        gPokemonStorageSystemPtr->state = 5;
        break;
    case 5:
        if (JOY_NEW(A_BUTTON | B_BUTTON | DPAD_ANY))
        {
            ClearBottomWindow();
            SetPokeStorageCallback(Task_PokeStorageMain);
        }
        break;
    }
}

static void Task_MoveMon(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        InitMonPlaceChange(PSS_PLACE_CHANGE_GRAB);
        gPokemonStorageSystemPtr->state++;
        break;
    case 1:
        if (!DoMonPlaceChange())
        {
            if (gPokeStorageInPartyMenu)
                SetPokeStorageCallback(Task_HandleMovingMonFromParty);
            else
                SetPokeStorageCallback(Task_PokeStorageMain);
        }
        break;
    }
}

static void Task_PlaceMon(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        InitMonPlaceChange(PSS_PLACE_CHANGE_PLACE);
        gPokemonStorageSystemPtr->state++;
        break;
    case 1:
        if (!DoMonPlaceChange())
        {
            if (gPokeStorageInPartyMenu)
                SetPokeStorageCallback(Task_HandleMovingMonFromParty);
            else
                SetPokeStorageCallback(Task_PokeStorageMain);
        }
        break;
    }
}

static void Task_ShiftMon(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        InitMonPlaceChange(PSS_PLACE_CHANGE_SHIFT);
        gPokemonStorageSystemPtr->state++;
        break;
    case 1:
        if (!DoMonPlaceChange())
        {
            StartDisplayMonMosaicEffect();
            SetPokeStorageCallback(Task_PokeStorageMain);
        }
        break;
    }
}

static void Task_WithdrawMon(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        if (CalculatePlayerPartyCount() == 6)
        {
            PrintMessage(PC_TEXT_PARTY_FULL);
            gPokemonStorageSystemPtr->state = 1;
        }
        else
        {
            SaveCursorPos();
            InitMonPlaceChange(PSS_PLACE_CHANGE_GRAB);
            gPokemonStorageSystemPtr->state = 2;
        }
        break;
    case 1:
        if (JOY_NEW(A_BUTTON | B_BUTTON | DPAD_ANY))
        {
            ClearBottomWindow();
            SetPokeStorageCallback(Task_PokeStorageMain);
        }
        break;
    case 2:
        if (!DoMonPlaceChange())
        {
            SetUpDoShowPartyMenu();
            gPokemonStorageSystemPtr->state++;
        }
        break;
    case 3:
        if (!DoShowPartyMenu())
        {
            InitMonPlaceChange(PSS_PLACE_CHANGE_PLACE);
            gPokemonStorageSystemPtr->state++;
        }
        break;
    case 4:
        if (!DoMonPlaceChange())
        {
            UpdatePartySlotColors();
            gPokemonStorageSystemPtr->state++;
        }
        break;
    case 5:
        SetPokeStorageCallback(Task_HidePartyPokemon);
        break;
    }
}

static void Task_DepositMenu(void)
{
    u8 r4;

    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        PrintMessage(PC_TEXT_DEPOSIT_IN_WHICH_BOX);
        LoadChooseBoxMenuGfx(&gPokemonStorageSystemPtr->chooseBoxMenu, 0x0007, 0xdaca, 3);
        CreateChooseBoxMenuSprites(gPokeStorageDepositBoxId);
        gPokemonStorageSystemPtr->state++;
        break;
    case 1:
        r4 = HandleChooseBoxMenuInput();
        if (r4 == BOXID_NONE_CHOSEN);
        else if (r4 == BOXID_CANCELED)
        {
            ClearBottomWindow();
            DestroyChooseBoxMenuSprites();
            FreeChooseBoxMenu();
            SetPokeStorageCallback(Task_PokeStorageMain);
        }
        else
        {
            if (TryStorePartyMonInBox(r4))
            {
                ClearBottomWindow();
                DestroyChooseBoxMenuSprites();
                FreeChooseBoxMenu();
                gPokemonStorageSystemPtr->state = 2;
            }
            else
            {
                PrintMessage(PC_TEXT_BOX_IS_FULL);
                gPokemonStorageSystemPtr->state = 4;
            }
            gPokeStorageDepositBoxId = r4;
        }
        break;
    case 2:
        CompactPartySlots();
        CompactPartySprites();
        gPokemonStorageSystemPtr->state++;
        break;
    case 3:
        if (GetNumPartySpritesCompacting() == 0)
        {
            ResetSelectionAfterDeposit();
            StartDisplayMonMosaicEffect();
            UpdatePartySlotColors();
            SetPokeStorageCallback(Task_PokeStorageMain);
        }
        break;
    case 4:
        if (JOY_NEW(A_BUTTON | B_BUTTON))
        {
            PrintMessage(PC_TEXT_DEPOSIT_IN_WHICH_BOX);
            gPokemonStorageSystemPtr->state = 1;
        }
        break;
    }
}

static void Task_ReleaseMon(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        PrintMessage(PC_TEXT_RELEASE_POKE);
        ShowYesNoWindow(1);
        gPokemonStorageSystemPtr->state++;
        // fallthrough
    case 1:
        switch (Menu_ProcessInputNoWrap())
        {
            case -1:
            case  1:
                ClearBottomWindow();
                SetPokeStorageCallback(Task_PokeStorageMain);
                break;
            case  0:
                ClearBottomWindow();
                InitCanReleaseMonVars();
                InitReleaseMon();
                gPokemonStorageSystemPtr->state++;
                break;
        }
        break;
    case 2:
        RunCanReleaseMon();
        if (!TryHideReleaseMon())
        {
            while (1)
            {
                s8 r0 = RunCanReleaseMon();
                if (r0 == 1)
                {
                    gPokemonStorageSystemPtr->state++;
                    break;
                }
                if (r0 == 0)
                {
                    gPokemonStorageSystemPtr->state = 8;
                    break;
                }
            }
        }
        break;
    case 3:
        ReleaseMon();
        RefreshDisplayMonData();
        PrintMessage(PC_TEXT_WAS_RELEASED);
        gPokemonStorageSystemPtr->state++;
        break;
    case 4:
        if (JOY_NEW(A_BUTTON | B_BUTTON))
        {
            PrintMessage(PC_TEXT_BYE_BYE);
            gPokemonStorageSystemPtr->state++;
        }
        break;
    case 5:
        if (JOY_NEW(A_BUTTON | B_BUTTON))
        {
            ClearBottomWindow();
            if (gPokeStorageInPartyMenu)
            {
                CompactPartySlots();
                CompactPartySprites();
                gPokemonStorageSystemPtr->state++;
            }
            else
            {
                gPokemonStorageSystemPtr->state = 7;
            }
        }
        break;
    case 6:
        if (GetNumPartySpritesCompacting() == 0)
        {
            RefreshDisplayMon();
            StartDisplayMonMosaicEffect();
            UpdatePartySlotColors();
            gPokemonStorageSystemPtr->state++;
        }
        break;
    case 7:
        SetPokeStorageCallback(Task_PokeStorageMain);
        break;
    case 8:
        PrintMessage(PC_TEXT_WAS_RELEASED);
        gPokemonStorageSystemPtr->state++;
        break;
    case 9:
        if (JOY_NEW(A_BUTTON | B_BUTTON))
        {
            PrintMessage(PC_TEXT_SURPRISE);
            gPokemonStorageSystemPtr->state++;
        }
        break;
    case 10:
        if (JOY_NEW(A_BUTTON | B_BUTTON))
        {
            ClearBottomWindow();
            ReshowReleaseMon();
            gPokemonStorageSystemPtr->state++;
        }
        break;
    case 11:
        if (!ResetReleaseMonSpritePtr())
        {
            TrySetCursorFistAnim();
            PrintMessage(PC_TEXT_CAME_BACK);
            gPokemonStorageSystemPtr->state++;
        }
        break;
    case 12:
        if (JOY_NEW(A_BUTTON | B_BUTTON))
        {
            PrintMessage(PC_TEXT_WORRIED);
            gPokemonStorageSystemPtr->state++;
        }
        break;
    case 13:
        if (JOY_NEW(A_BUTTON | B_BUTTON))
        {
            ClearBottomWindow();
            SetPokeStorageCallback(Task_PokeStorageMain);
        }
        break;
    }
}

static void Task_ShowMonSummary(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        InitSummaryScreenData();
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        gPokemonStorageSystemPtr->state++;
        break;
    case 1:
        if (!UpdatePaletteFade())
        {
            gPokeStorageScreenToReshow = 0;
            gPokemonStorageSystemPtr->screenChangeType = 0;
            SetMainCallback2(CB2_ChangePokeStorageScreen);
        }
        break;
    }
}

static void Task_ShowMarkMenu(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        PrintMessage(PC_TEXT_MARK_POKE);
        gPokemonStorageSystemPtr->markMenu.markings = gPokemonStorageSystemPtr->displayMonMarkings;
        OpenMonMarkingsMenu(gPokemonStorageSystemPtr->displayMonMarkings, 0xb0, 0x10);
        gPokemonStorageSystemPtr->state++;
        break;
    case 1:
        if (!HandleMonMarkingsMenuInput())
        {
            FreeMonMarkingsMenu();
            ClearBottomWindow();
            SetPokeStorageMonMarkings(gPokemonStorageSystemPtr->markMenu.markings);
            RefreshDisplayMonData();
            SetPokeStorageCallback(Task_PokeStorageMain);
        }
        break;
    }
}

static void Task_HandleMovingMonFromParty(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        CompactPartySlots();
        CompactPartySprites();
        gPokemonStorageSystemPtr->state++;
        break;
    case 1:
        if (GetNumPartySpritesCompacting() == 0)
        {
            UpdatePartySlotColors();
            SetPokeStorageCallback(Task_PokeStorageMain);
        }
        break;
    }
}

static void Task_HandleBoxOptions(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        PrintMessage(PC_TEXT_WHAT_YOU_DO);
        AddPokeStorageMenu();
        gPokemonStorageSystemPtr->state++;
        break;
    case 1:
        switch (HandlePokeStorageMenuInput())
        {
        case -1:
        case PSS_MENU_CANCEL:
            AnimateBoxScrollArrows(TRUE);
            ClearBottomWindow();
            SetPokeStorageCallback(Task_PokeStorageMain);
            break;
        case PSS_MENU_NAME:
            PlaySE(SE_SELECT);
            SetPokeStorageCallback(Task_NameBox);
            break;
        case PSS_MENU_WALLPAPER:
            PlaySE(SE_SELECT);
            ClearBottomWindow();
            SetPokeStorageCallback(Task_HandleWallpapers);
            break;
        case PSS_MENU_JUMP:
            PlaySE(SE_SELECT);
            ClearBottomWindow();
            SetPokeStorageCallback(Task_JumpBox);
            break;
        }
        break;
    }
}

static void Task_HandleWallpapers(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        AddWallpaperSetsMenu();
        PrintMessage(PC_TEXT_PICK_A_THEME);
        gPokemonStorageSystemPtr->state++;
        break;
    case 1:
        gPokemonStorageSystemPtr->wallpaperSetId = HandlePokeStorageMenuInput();
        switch (gPokemonStorageSystemPtr->wallpaperSetId)
        {
        case -1:
            AnimateBoxScrollArrows(TRUE);
            ClearBottomWindow();
            SetPokeStorageCallback(Task_PokeStorageMain);
            break;
        case PSS_MENU_SCENERY_1 ... PSS_MENU_ETCETERA:
            PlaySE(SE_SELECT);
            gPokemonStorageSystemPtr->wallpaperSetId -= PSS_MENU_SCENERY_1;
            AddWallpapersMenu(gPokemonStorageSystemPtr->wallpaperSetId);
            PrintMessage(PC_TEXT_PICK_A_WALLPAPER);
            gPokemonStorageSystemPtr->state++;
            break;
        }
        break;
    case 2:
        gPokemonStorageSystemPtr->wallpaperId = HandlePokeStorageMenuInput();
        switch (gPokemonStorageSystemPtr->wallpaperId)
        {
        case -1:
            ClearBottomWindow();
            gPokemonStorageSystemPtr->state = 0;
            break;
        case -2:
            break;
        default:
            PlaySE(SE_SELECT);
            ClearBottomWindow();
            gPokemonStorageSystemPtr->wallpaperId -= PSS_MENU_FOREST;
            SetWallpaperForCurrentBox(gPokemonStorageSystemPtr->wallpaperId);
            gPokemonStorageSystemPtr->state++;
            break;
        }
        break;
    case 3:
        if (!DoWallpaperGfxChange())
        {
            AnimateBoxScrollArrows(TRUE);
            SetPokeStorageCallback(Task_PokeStorageMain);
        }
        break;
    }
}

static void Task_JumpBox(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        PrintMessage(PC_TEXT_JUMP_TO_WHICH_BOX);
        LoadChooseBoxMenuGfx(&gPokemonStorageSystemPtr->chooseBoxMenu, 0x0007, 0xdaca, 3);
        CreateChooseBoxMenuSprites(gPokemonStorage.currentBox);
        gPokemonStorageSystemPtr->state++;
        break;
    case 1:
        gPokemonStorageSystemPtr->newCurrBoxId = HandleChooseBoxMenuInput();
        switch (gPokemonStorageSystemPtr->newCurrBoxId)
        {
        case BOXID_NONE_CHOSEN:
            break;
        default:
            ClearBottomWindow();
            DestroyChooseBoxMenuSprites();
            FreeChooseBoxMenu();
            if (gPokemonStorageSystemPtr->newCurrBoxId == BOXID_CANCELED || gPokemonStorageSystemPtr->newCurrBoxId == gPokemonStorage.currentBox)
            {
                AnimateBoxScrollArrows(TRUE);
                SetPokeStorageCallback(Task_PokeStorageMain);
            }
            else
            {
                gPokemonStorageSystemPtr->state++;
            }
            break;
        }
        break;
    case 2:
        SetUpScrollToBox(gPokemonStorageSystemPtr->newCurrBoxId);
        gPokemonStorageSystemPtr->state++;
        break;
    case 3:
        if (!ScrollToBox())
        {
            gPokemonStorage.currentBox = gPokemonStorageSystemPtr->newCurrBoxId;
            SetPokeStorageCallback(Task_PokeStorageMain);
        }
        break;
    }
}

static void Task_NameBox(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        SaveMovingMon();
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        gPokemonStorageSystemPtr->state++;
        break;
    case 1:
        if (!UpdatePaletteFade())
        {
            gPokeStorageScreenToReshow = 1;
            gPokemonStorageSystemPtr->screenChangeType = 1;
            SetMainCallback2(CB2_ChangePokeStorageScreen);
        }
        break;
    }
}

static void Task_OnCloseBoxPressed(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        if (IsMonBeingMoved())
        {
            PlaySE(SE_FAILURE);
            PrintMessage(PC_TEXT_HOLDING_POKE);
            gPokemonStorageSystemPtr->state = 1;
        }
        else
        {
            PlaySE(SE_SELECT);
            PrintMessage(PC_TEXT_EXIT_BOX);
            ShowYesNoWindow(0);
            gPokemonStorageSystemPtr->state = 2;
        }
        break;
    case 1:
        if (JOY_NEW(A_BUTTON | B_BUTTON | DPAD_ANY))
        {
            ClearBottomWindow();
            SetPokeStorageCallback(Task_PokeStorageMain);
        }
        break;
    case 2:
        switch (Menu_ProcessInputNoWrap())
        {
        case 1:
        case -1:
            ClearBottomWindow();
            SetPokeStorageCallback(Task_PokeStorageMain);
            break;
        case 0:
            PlaySE(SE_PC_OFF);
            ClearBottomWindow();
            gPokemonStorageSystemPtr->state++;
            break;
        }
        break;
    case 3:
        gPokemonStorageSystemPtr->pcScreenEffect.tileTag = 0x000e;
        gPokemonStorageSystemPtr->pcScreenEffect.paletteTag = 0xdad0;
        gPokemonStorageSystemPtr->pcScreenEffect.spriteSpeed = 20;
        gPokemonStorageSystemPtr->pcScreenEffect.revealSpeed = 0;
        StartPCScreenCloseEffect(&gPokemonStorageSystemPtr->pcScreenEffect);
        gPokemonStorageSystemPtr->state++;
        break;
    case 4:
        if (UpdatePCScreenCloseEffect())
        {
            gPlayerPartyCount = CalculatePlayerPartyCount();
            SetMainCallback2(CB2_ExitPokeStorage);
        }
        break;
    }
}

static void Task_OnBPressed(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        if (IsMonBeingMoved())
        {
            PlaySE(SE_FAILURE);
            PrintMessage(PC_TEXT_HOLDING_POKE);
            gPokemonStorageSystemPtr->state = 1;
        }
        else
        {
            PlaySE(SE_SELECT);
            PrintMessage(PC_TEXT_CONTINUE_BOX);
            ShowYesNoWindow(0);
            gPokemonStorageSystemPtr->state = 2;
        }
        break;
    case 1:
        if (JOY_NEW(A_BUTTON | B_BUTTON | DPAD_ANY))
        {
            ClearBottomWindow();
            SetPokeStorageCallback(Task_PokeStorageMain);
        }
        break;
    case 2:
        switch (Menu_ProcessInputNoWrap())
        {
        case 0:
            ClearBottomWindow();
            SetPokeStorageCallback(Task_PokeStorageMain);
            break;
        case -1:
        case 1:
            PlaySE(SE_PC_OFF);
            ClearBottomWindow();
            gPokemonStorageSystemPtr->state++;
            break;
        }
        break;
    case 3:
        gPokemonStorageSystemPtr->pcScreenEffect.tileTag = 0x000e;
        gPokemonStorageSystemPtr->pcScreenEffect.paletteTag = 0xdad0;
        gPokemonStorageSystemPtr->pcScreenEffect.spriteSpeed = 20;
        gPokemonStorageSystemPtr->pcScreenEffect.revealSpeed = 0;
        StartPCScreenCloseEffect(&gPokemonStorageSystemPtr->pcScreenEffect);
        gPokemonStorageSystemPtr->state++;
        break;
    case 4:
        if (UpdatePCScreenCloseEffect())
        {
            gPlayerPartyCount = CalculatePlayerPartyCount();
            SetMainCallback2(CB2_ExitPokeStorage);
        }
        break;
    }
}

static void SetScrollingBackground(void)
{
    gPokemonStorageSystemPtr->scrollBackgroundTimer = 0;
    gPokemonStorageSystemPtr->bg3_Y = 0;
    gPokemonStorageSystemPtr->bg3_X = 0;
    REG_BG3CNT = BGCNT_PRIORITY(3) | BGCNT_CHARBASE(3) | BGCNT_SCREENBASE(30);
    DmaCopy16Defvars(3, gPokemonStorageScrollingBGTile, BG_SCREEN_ADDR(28), sizeof gPokemonStorageScrollingBGTile);
    LZ77UnCompVram(gPokemonStorageScrollingBGTilemap, BG_SCREEN_ADDR(30));
    LoadPalette(gPokemonStorageScrollingBGPalette, 0xd0, 0x10);
}

static void ScrollBackground(void)
{
    if (++gPokemonStorageSystemPtr->scrollBackgroundTimer >= 2)
    {
        gPokemonStorageSystemPtr->scrollBackgroundTimer = 0;
        gPokemonStorageSystemPtr->bg3_Y--;
        gPokemonStorageSystemPtr->bg3_X++;
    }
}

static void InitPalettesAndSprites(void)
{
    LZ77UnCompVram(gPSSMenuHeader_Gfx, BG_SCREEN_ADDR(10));
    LZ77UnCompWram(gPSSMenuHeader_Tilemap, gUnknown_02039760);
    UnkUtil_CpuAdd(BG_SCREEN_ADDR(15), 0, 0, gUnknown_02039760, 0, 0, 10, 20);
    LoadPalette(gPSSMenu1_Pal, 0x10, 0x20);
    LoadPalette(gPSSMenu2_Pal, 0x00, 0x20);
    LoadPalette(gUnknown_083B6D74, 0xB0, 0x20);
    LoadPalette(gUnknown_083B6D94, 0xC0, 0x20);
    LoadPalette(gUnknownPalette_81E6692 +  1, 0xF1, 0x02);
    LoadPalette(gUnknownPalette_81E6692 +  1, 0xF2, 0x02);
    LoadPalette(gUnknownPalette_81E6692 +  5, 0xF3, 0x02);
    LoadPalette(gUnknownPalette_81E6692 + 12, 0xF4, 0x04);
    LoadPalette(gUnknownPalette_81E6692 + 10, 0xF6, 0x04);
    LoadPalette(gUnknownPalette_81E6692 +  2, 0xFF, 0x02);
    LoadSpritePalette(&gWaveformSpritePalette);
    CreateDisplayMonSprite();
    CreateMarkingComboSprite();
    CreateWaveformSprites();
    RefreshDisplayMonData();
}

static void CreateMarkingComboSprite(void)
{
    gPokemonStorageSystemPtr->markingComboSprite = CreateMonMarkingComboSprite(0x000d, 0xdace, 0);
    gPokemonStorageSystemPtr->markingComboSprite->oam.priority = 0;
    gPokemonStorageSystemPtr->markingComboSprite->subpriority = 1;
    gPokemonStorageSystemPtr->markingComboSprite->x = 0x28;
    gPokemonStorageSystemPtr->markingComboSprite->y = 0x95;
    gPokemonStorageSystemPtr->markingComboTilesPtr = BG_CHAR_ADDR(4) + 32 * GetSpriteTileStartByTag(0x000d);
}

const struct SpriteTemplate gSpriteTemplate_83B6EFC;

static void CreateWaveformSprites(void)
{
    u16 i;
    struct SpriteSheet sheet = {WaveformTiles, 0x1c0, 0x0005};

    LoadSpriteSheet(&sheet);
    for (i = 0; i < 2; i++)
    {
        u8 spriteId = CreateSprite(&gSpriteTemplate_83B6EFC, i * 63 + 8, 9, 2);
        gPokemonStorageSystemPtr->waveformSprites[i] = gSprites + spriteId;
    }
}

static void RefreshDisplayMonData(void)
{
    LoadDisplayMonGfx(gPokemonStorageSystemPtr->displayMonSpecies, gPokemonStorageSystemPtr->displayMonPersonality);
    PrintDisplayMonInfo();
    UpdateWaveformAnimation();
}

static void StartDisplayMonMosaicEffect(void)
{
    RefreshDisplayMonData();
    if (gPokemonStorageSystemPtr->displayMonSprite)
    {
        gPokemonStorageSystemPtr->displayMonSprite->oam.mosaic = TRUE;
        gPokemonStorageSystemPtr->displayMonSprite->data[0] = 10;
        gPokemonStorageSystemPtr->displayMonSprite->data[1] = 1;
        gPokemonStorageSystemPtr->displayMonSprite->callback = SpriteCB_DisplayMonMosaic;
        REG_MOSAIC = (gPokemonStorageSystemPtr->displayMonSprite->data[0] << 12) | (gPokemonStorageSystemPtr->displayMonSprite->data[0] << 8);
    }
}

static void SpriteCB_DisplayMonMosaic(struct Sprite *sprite)
{
    sprite->data[0] -= sprite->data[1];
    if (sprite->data[0] < 0)
        sprite->data[0] = 0;
    REG_MOSAIC = (sprite->data[0] << 12) | (sprite->data[0] << 8);
    if (sprite->data[0] == 0)
    {
        sprite->oam.mosaic = FALSE;
        sprite->callback = SpriteCallbackDummy;
    }
}

// CreateDisplayMonSprite

const struct OamData gOamData_83B6EAC;

static void CreateDisplayMonSprite(void)
{
    u16 i;
    u16 startTile;
    u8 paletteNum;
    u8 spriteId;

    struct SpriteSheet spriteSheet = {
        .data = gPokemonStorageSystemPtr->tileBuffer,
        .size = 0x800,
        .tag = 0x0002
    };
    struct SpritePalette spritePalette = {
        .data = gPokemonStorageSystemPtr->displayMonPalBuffer,
        .tag = 0xDAC7
    };
    struct SpriteTemplate spriteTemplate = {
        .tileTag = 0x0002,
        .paletteTag = 0xDAC7,
        .oam = &gOamData_83B6EAC,
        .anims = gDummySpriteAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCallbackDummy
    };

    for (i = 0; i < 0x800; i++)
    {
        gPokemonStorageSystemPtr->tileBuffer[i] = 0;
    }
    for (i = 0; i < 0x10; i++)
    {
        gPokemonStorageSystemPtr->displayMonPalBuffer[i] = 0;
    }
    gPokemonStorageSystemPtr->displayMonSprite = NULL;
    do
    {
        startTile = LoadSpriteSheet(&spriteSheet);
        if (startTile == 0)
            break;

        paletteNum = LoadSpritePalette(&spritePalette);
        if (paletteNum == 0xFF)
            break;

        spriteId = CreateSprite(&spriteTemplate, 0x28, 0x30, 0);
        if (spriteId == MAX_SPRITES)
            break;

        gPokemonStorageSystemPtr->displayMonSprite = &gSprites[spriteId];
        gPokemonStorageSystemPtr->displayMonPalOffset = 0x100 + 16 * paletteNum;
        gPokemonStorageSystemPtr->displayMonTilePtr = (void *)(OBJ_VRAM0 + 32 * startTile);
    } while (0);
    if (gPokemonStorageSystemPtr->displayMonSprite == NULL)
    {
        FreeSpriteTilesByTag(0x0002);
        FreeSpritePaletteByTag(0xDAC7);
    }
}

static void LoadDisplayMonGfx(u16 species, u32 pid)
{
    if (gPokemonStorageSystemPtr->displayMonSprite)
    {
        if (species != SPECIES_NONE)
        {
            HandleLoadSpecialPokePic(gMonFrontPicTable + species, gMonFrontPicCoords[species].coords, 1, gPokemonStorageSystemPtr->displayMonPicBuffer, gPokemonStorageSystemPtr->tileBuffer, species, pid);
            LZ77UnCompWram(gPokemonStorageSystemPtr->displayMonPalette, gPokemonStorageSystemPtr->displayMonPalBuffer);
            CpuCopy32(gPokemonStorageSystemPtr->tileBuffer, gPokemonStorageSystemPtr->displayMonTilePtr, 0x800);
            LoadPalette(gPokemonStorageSystemPtr->displayMonPalBuffer, gPokemonStorageSystemPtr->displayMonPalOffset, 0x20);
            gPokemonStorageSystemPtr->displayMonSprite->invisible = FALSE;
        }
        else
        {
            gPokemonStorageSystemPtr->displayMonSprite->invisible = TRUE;
        }
    }
}

static void PrintDisplayMonInfo(void)
{
    if (gPokemonStorageSystemPtr->displayMonSpecies)
    {
        UpdateMonMarkingTiles(gPokemonStorageSystemPtr->displayMonMarkings, gPokemonStorageSystemPtr->markingComboTilesPtr);
        gPokemonStorageSystemPtr->markingComboSprite->invisible = FALSE;
    }
    else
    {
        gPokemonStorageSystemPtr->markingComboSprite->invisible = TRUE;
    }
    Menu_EraseWindowRect(0, 11, 9, 17);
    Menu_PrintText(gPokemonStorageSystemPtr->displayMonItemName, 1, 16);
    Menu_PrintText(gPokemonStorageSystemPtr->displayMonNameText, 1, 11);
    Menu_PrintText(gPokemonStorageSystemPtr->displayMonSpeciesName, 0, 13);
    Menu_PrintText(gPokemonStorageSystemPtr->displayMonGenderLvlText, 1, 15);
}

static void UpdateWaveformAnimation(void)
{
    u16 i;

    if (gPokemonStorageSystemPtr->displayMonSpecies)
    {
        UnkUtil_CpuAdd(BG_SCREEN_ADDR(15), 1, 0, gUnknown_02039760, 1, 0, 8, 2);
        for (i = 0; i < 2; i++)
            StartSpriteAnimIfDifferent(gPokemonStorageSystemPtr->waveformSprites[i], i * 2 + 1);
    }
    else
    {
        UnkUtil_CpuAdd(BG_SCREEN_ADDR(15), 1, 0, gUnknown_02039760, 10, 0, 8, 2);
        for (i = 0; i < 2; i++)
            StartSpriteAnim(gPokemonStorageSystemPtr->waveformSprites[i], i * 2);
    }
}

static void InitSupplementalTilemaps(void)
{
    REG_BG1CNT = BGCNT_PRIORITY(1) | BGCNT_SCREENBASE(15);
    LZ77UnCompVram(gPSSMenuMisc_Gfx, BG_SCREEN_ADDR(13));
    LZ77UnCompWram(gPSSMenuMisc_Tilemap, gPokemonStorageSystemPtr->partyMenuTilemapBuffer);
    LoadPalette(gPSSMenu3_Pal, 0x20, 0x20);
    LoadPalette(gPSSMenu4_Pal, 0x30, 0x20);
    DmaClear16(3, BG_SCREEN_ADDR(15), 0x800);
    SetPartySlotTilemaps();
    if (gPokeStorageInPartyMenu)
    {
        UpdateCloseBoxButtonTilemap(TRUE);
        CreatePartyMonsSprites(TRUE);
        UnkUtil_CpuAdd(BG_SCREEN_ADDR(15), 10, 0, gPokemonStorageSystemPtr->partyMenuTilemapBuffer, 0, 0, 12, 22);
    }
    else
    {
        UnkUtil_CpuAdd(BG_SCREEN_ADDR(15), 10, 0, gPokemonStorageSystemPtr->partyMenuTilemapBuffer, 0, 20, 12, 2);
        UpdateCloseBoxButtonTilemap(TRUE);
    }
    gPokemonStorageSystemPtr->closeBoxFlashing = 0;
}

static void SetUpShowPartyMenu(void)
{
    gPokemonStorageSystemPtr->partyMenuUnused1 = 20;
    gPokemonStorageSystemPtr->partyMenuY = 2;
    gPokemonStorageSystemPtr->partyMenuMoveTimer = 0;
    CreatePartyMonsSprites(FALSE);
}

static bool8 ShowPartyMenu(void)
{
    if (gPokemonStorageSystemPtr->partyMenuMoveTimer == 20)
        return FALSE;
    gPokemonStorageSystemPtr->partyMenuUnused1--;
    gPokemonStorageSystemPtr->partyMenuY++;
    UnkUtil_CpuAdd(BG_SCREEN_ADDR(15), 10, 0, gPokemonStorageSystemPtr->partyMenuTilemapBuffer, 0, gPokemonStorageSystemPtr->partyMenuUnused1, 12, gPokemonStorageSystemPtr->partyMenuY);
    MovePartySprites(8);
    if (++gPokemonStorageSystemPtr->partyMenuMoveTimer == 20)
    {
        gPokeStorageInPartyMenu = 1;
        return FALSE;
    }
    return TRUE;
}

static void SetUpHidePartyMenu(void)
{
    gPokemonStorageSystemPtr->partyMenuUnused1 = 0;
    gPokemonStorageSystemPtr->partyMenuY = 22;
    gPokemonStorageSystemPtr->partyMenuMoveTimer = 0;
}

static bool8 HidePartyMenu(void)
{
    if (gPokemonStorageSystemPtr->partyMenuMoveTimer == 20)
        return FALSE;
    gPokemonStorageSystemPtr->partyMenuUnused1++;
    gPokemonStorageSystemPtr->partyMenuY--;
    UnkUtil_CpuAdd(BG_SCREEN_ADDR(15), 10, 0, gPokemonStorageSystemPtr->partyMenuTilemapBuffer, 0, gPokemonStorageSystemPtr->partyMenuUnused1, 12, gPokemonStorageSystemPtr->partyMenuY);
    UnkUtil_DmaAdd(BG_SCREEN_ADDR(15), 10, gPokemonStorageSystemPtr->partyMenuY, 12, 1);
    MovePartySprites(-8);
    if (++gPokemonStorageSystemPtr->partyMenuMoveTimer == 20)
    {
        gPokeStorageInPartyMenu = 0;
        DestroyAllPartyMonIcons();
        CompactPartySlots();
        UnkUtil_CpuAdd(BG_SCREEN_ADDR(15), 21, 0, gPokemonStorageSystemPtr->partyMenuTilemapBuffer, 12, 0, 1, 2);
        return FALSE;
    }
    return TRUE;
}

static void UpdateCloseBoxButtonTilemap(bool8 flag)
{
    if (flag)
        UnkUtil_CpuAdd(BG_SCREEN_ADDR(15), 21, 0, gPokemonStorageSystemPtr->partyMenuTilemapBuffer, 12, 0, 9, 2);
    else
        UnkUtil_CpuAdd(BG_SCREEN_ADDR(15), 21, 0, gPokemonStorageSystemPtr->partyMenuTilemapBuffer, 12, 2, 9, 2);
}

static void StartFlashingCloseBoxButton(void)
{
    gPokemonStorageSystemPtr->closeBoxFlashing = 1;
    gPokemonStorageSystemPtr->closeBoxFlashTimer = 30;
    gPokemonStorageSystemPtr->closeBoxFlashState = 1;
}

static void StopFlashingCloseBoxButton(void)
{
    if (gPokemonStorageSystemPtr->closeBoxFlashing)
    {
        gPokemonStorageSystemPtr->closeBoxFlashing = 0;
        UpdateCloseBoxButtonTilemap(TRUE);
    }
}

static void UpdateCloseBoxButtonFlash(void)
{
    if (gPokemonStorageSystemPtr->closeBoxFlashing && ++gPokemonStorageSystemPtr->closeBoxFlashTimer > 30)
    {
        gPokemonStorageSystemPtr->closeBoxFlashTimer = 0;
        gPokemonStorageSystemPtr->closeBoxFlashState = gPokemonStorageSystemPtr->closeBoxFlashState ? FALSE : TRUE;
        UpdateCloseBoxButtonTilemap(gPokemonStorageSystemPtr->closeBoxFlashState);
    }
}

static void SetPartySlotTilemaps(void)
{
    int i;

    for (i = 1; i < PARTY_SIZE; i++)
    {
        u16 r1;
        bool32 r0 = GetMonData(gPlayerParty + i, MON_DATA_SPECIES);
        if (r0)
            r0 = TRUE;
        r1 = r0 ? 12 : 16;
        CopyTilemapRectFixedWidth(gPokemonStorageSystemPtr->partyMenuTilemapBuffer, 7, (i - 1) * 3 + 1, gPokemonStorageSystemPtr->partyMenuTilemapBuffer, r1, 4, 4, 3);
    }
}

static void UpdatePartySlotColors(void)
{
    SetPartySlotTilemaps();
    UnkUtil_CpuAdd(BG_SCREEN_ADDR(15), 10, 0, gPokemonStorageSystemPtr->partyMenuTilemapBuffer, 0, 0, 12, 22);
}

static void SetUpDoShowPartyMenu(void)
{
    gPokemonStorageSystemPtr->showPartyMenuState = 0;
    PlaySE(SE_WIN_OPEN);
    SetUpShowPartyMenu();
}

static bool8 DoShowPartyMenu(void)
{
    switch (gPokemonStorageSystemPtr->showPartyMenuState)
    {
    case 0:
        if (!ShowPartyMenu())
        {
            SetCursorInParty();
            gPokemonStorageSystemPtr->showPartyMenuState++;
        }
        break;
    case 1:
        if (!UpdateCursorPos())
        {
            if (gPokemonStorageSystemPtr->setMosaic)
                StartDisplayMonMosaicEffect();
            gPokemonStorageSystemPtr->showPartyMenuState++;
        }
        break;
    case 2:
        return FALSE;
    }
    return TRUE;
}

static const struct StorageMessage sMessages[] =
{
    {PCText_ExitBox, 0},
    {PCText_WhatYouDo, 0},
    {PCText_PickATheme, 0},
    {PCText_PickAWallpaper, 0},
    {PCText_IsSelected, 1},
    {PCText_JumpToWhichBox, 0},
    {PCText_DepositInWhichBox, 0},
    {PCText_WasDeposited, 1},
    {PCText_BoxIsFull, 0},
    {PCText_ReleasePoke, 0},
    {PCText_WasReleased, 4},
    {PCText_ByeBye, 6},
    {PCText_MarkPoke, 0},
    {PCText_LastPoke, 0},
    {PCText_PartyFull, 0},
    {PCText_HoldingPoke, 0},
    {PCText_WhichOneWillTake, 0},
    {PCText_CantReleaseEgg, 0},
    {PCText_ContinueBox, 0},
    {PCText_CameBack, 1},
    {PCText_Worried, 0},
    {PCText_Surprise, 0},
    {PCText_PleaseRemoveMail, 0}
};

static void PrintMessage(u8 id)
{
    u8 *ptr;

    Menu_DrawStdWindowFrame(10, 16, 29, 19);

    switch (sMessages[id].format)
    {
    case PC_TEXT_FMT_UNK_02:
        ptr = StringCopy(gPokemonStorageSystemPtr->messageText, sMessages[id].text);
        ptr = StringCopy(ptr, gPokemonStorageSystemPtr->displayMonName);
        break;
    case PC_TEXT_FMT_UNK_05:
        ptr = StringCopy(gPokemonStorageSystemPtr->messageText, sMessages[id].text);
        ptr = StringCopy(ptr, gPokemonStorageSystemPtr->releaseMonName);
        break;
    case PC_TEXT_FMT_MON_NAME:
        // {var} + " is selected."
        ptr = StringCopy(gPokemonStorageSystemPtr->messageText, gPokemonStorageSystemPtr->displayMonName);
        ptr = StringCopy(ptr, sMessages[id].text);
        break;
    case PC_TEXT_FMT_MON_NAME_2:
        // {var} + " was released."
        ptr = StringCopy(gPokemonStorageSystemPtr->messageText, gPokemonStorageSystemPtr->releaseMonName);
#if ENGLISH
        ptr = StringCopy(ptr, sMessages[id].text);
#elif GERMAN
        ptr = de_sub_8073174(gPokemonStorageSystemPtr->messageText, sMessages[id].text);
#endif
        break;
    case PC_TEXT_FMT_UNK_03:
        {
            const u8 *stringLength;
            const u8 *text;

            text = sMessages[id].text;
            stringLength = &text[StringLength(text)] + 1;

            ptr = StringCopy(gPokemonStorageSystemPtr->messageText, sMessages[id].text);
            ptr = StringCopy(ptr, gPokemonStorageSystemPtr->displayMonName);
            ptr = StringCopy(ptr, stringLength);
        }
        break;
    case PC_TEXT_FMT_MON_NAME_AFTER_EXCL_MARK:
        {
            const u8 *stringLength;
            const u8 *text;

            text = sMessages[id].text;
            stringLength = &text[StringLength(text)] - 1;

            ptr = StringCopy(gPokemonStorageSystemPtr->messageText, sMessages[id].text);
            ptr = StringCopy(ptr - 1, gPokemonStorageSystemPtr->releaseMonName);
            ptr = StringCopy(ptr, stringLength);
        }
        break;
    case PC_TEXT_FMT_NORMAL:
    default:
        ptr = StringCopy(gPokemonStorageSystemPtr->messageText, sMessages[id].text);
        break;
    }

    while (ptr < gPokemonStorageSystemPtr->unk_26a6)
    {
        ptr[0] = CHAR_SPACE;
        ptr++;
    }

    ptr[0] = EOS;
    Menu_PrintText(gPokemonStorageSystemPtr->messageText, 11, 17);
}

const struct OamData gOamData_83B6EAC = {
    .size = 3
};

const struct OamData gOamData_83B6EB4 = {
    .shape = ST_OAM_H_RECTANGLE
};

const union AnimCmd gSpriteAnim_83B6EBC[] = {
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END
};

const union AnimCmd gSpriteAnim_83B6EC4[] = {
    ANIMCMD_FRAME(2, 8),
    ANIMCMD_FRAME(4, 8),
    ANIMCMD_FRAME(6, 8),
    ANIMCMD_JUMP(0)
};

const union AnimCmd gSpriteAnim_83B6ED4[] = {
    ANIMCMD_FRAME(8, 5),
    ANIMCMD_END
};

const union AnimCmd gSpriteAnim_83B6EDC[] = {
    ANIMCMD_FRAME(10, 8),
    ANIMCMD_FRAME( 4, 8),
    ANIMCMD_FRAME(12, 8),
    ANIMCMD_JUMP(0)
};

const union AnimCmd *const gSpriteAnimTable_83B6EEC[] = {
    gSpriteAnim_83B6EBC,
    gSpriteAnim_83B6EC4,
    gSpriteAnim_83B6ED4,
    gSpriteAnim_83B6EDC
};

const struct SpriteTemplate gSpriteTemplate_83B6EFC = {
    0x0005,
    0xdacd,
    &gOamData_83B6EB4,
    gSpriteAnimTable_83B6EEC,
    NULL,
    gDummySpriteAffineAnimTable,
    SpriteCallbackDummy
};

static void ShowYesNoWindow(s8 a0)
{
    DisplayYesNoMenu(23, 10, 0);
    Menu_MoveCursor(a0);
}

static void ClearBottomWindow(void)
{
    Menu_DestroyCursor();
    Menu_EraseWindowRect(10, 16, 29, 19);
    Menu_EraseWindowRect(23, 10, 29, 15);
}

static void AddWallpaperSetsMenu(void)
{
    InitPokeStorageMenu();
    SetPokeStorageMenuText(PSS_MENU_SCENERY_1);
    SetPokeStorageMenuText(PSS_MENU_SCENERY_2);
    SetPokeStorageMenuText(PSS_MENU_SCENERY_3);
    SetPokeStorageMenuText(PSS_MENU_ETCETERA);
    AddPokeStorageMenu();
}

static void AddWallpapersMenu(u8 a0)
{
    InitPokeStorageMenu();
    switch (a0)
    {
    case 0:
        SetPokeStorageMenuText(PSS_MENU_FOREST);
        SetPokeStorageMenuText(PSS_MENU_CITY);
        SetPokeStorageMenuText(PSS_MENU_DESERT);
        SetPokeStorageMenuText(PSS_MENU_SAVANNA);
        break;
    case 1:
        SetPokeStorageMenuText(PSS_MENU_CRAG);
        SetPokeStorageMenuText(PSS_MENU_VOLCANO);
        SetPokeStorageMenuText(PSS_MENU_SNOW);
        SetPokeStorageMenuText(PSS_MENU_CAVE);
        break;
    case 2:
        SetPokeStorageMenuText(PSS_MENU_BEACH);
        SetPokeStorageMenuText(PSS_MENU_SEAFLOOR);
        SetPokeStorageMenuText(PSS_MENU_RIVER);
        SetPokeStorageMenuText(PSS_MENU_SKY);
        break;
    case 3:
        SetPokeStorageMenuText(PSS_MENU_POLKADOT);
        SetPokeStorageMenuText(PSS_MENU_POKECENTER);
        SetPokeStorageMenuText(PSS_MENU_MACHINE);
        SetPokeStorageMenuText(PSS_MENU_PLAIN);
        break;
    }
    AddPokeStorageMenu();
}
