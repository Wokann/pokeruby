
// Includes
#include "global.h"
#include "decompress.h"
#include "constants/songs.h"
#include "constants/species.h"
#include "data.h"
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
#include "mail.h"
#include "naming_screen.h"
#include "pokemon_storage_system.h"

// Static type declarations

enum {
    MSG_EXIT_BOX,
    MSG_WHAT_YOU_DO,
    MSG_PICK_A_THEME,
    MSG_PICK_A_WALLPAPER,
    MSG_IS_SELECTED,
    MSG_JUMP_TO_WHICH_BOX,
    MSG_DEPOSIT_IN_WHICH_BOX,
    MSG_WAS_DEPOSITED,
    MSG_BOX_IS_FULL,
    MSG_RELEASE_POKE,
    MSG_WAS_RELEASED,
    MSG_BYE_BYE,
    MSG_MARK_POKE,
    MSG_LAST_POKE,
    MSG_PARTY_FULL,
    MSG_HOLDING_POKE,
    MSG_WHICH_ONE_WILL_TAKE,
    MSG_CANT_RELEASE_EGG,
    MSG_CONTINUE_BOX,
    MSG_CAME_BACK,
    MSG_WORRIED,
    MSG_SURPRISE,
    MSG_PLEASE_REMOVE_MAIL
};

enum {
    MSG_VAR_NONE,
    MSG_VAR_MON_NAME_1,
    MSG_VAR_MON_NAME_2,
    MSG_VAR_MON_NAME_3,
    MSG_VAR_RELEASE_MON_1,
    MSG_VAR_RELEASE_MON_2,
    MSG_VAR_RELEASE_MON_3
};

struct StorageMessage {
    const u8 *text;
    u8 format;
};

// Static RAM declarations

static EWRAM_DATA u8 sInPartyMenu = 0;
static EWRAM_DATA u8 sCurrentBoxOption = 0;
static EWRAM_DATA u8 sDepositBoxId = 0;
static EWRAM_DATA u8 sWhichToReshow = 0;

// Static ROM declarations

void sub_8096884(void);
static void VBlankCB_PokeStorage(void);
static void CB2_PokeStorage(void);
void SetPSSCallback(void (*func)(void));
void sub_8096BF0(void);
void sub_8096C68(void);
void sub_8096C84(void);
void sub_8096FC8(void);
void sub_8097004(void);
void sub_8097078(void);
void sub_80972A8(void);
void sub_80972FC(void);
void c3_0808DC50(void);
void sub_8097390(void);
void sub_809746C(void);
void sub_8097594(void);
void sub_8097788(void);
void sub_80977E4(void);
void sub_8097858(void);
void sub_809789C(void);
void sub_8097974(void);
void sub_8097A64(void);
void sub_8097B44(void);
void sub_8097BA0(void);
void sub_8097CC0(void);
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

static const u16 sScrollingBg_Pal[] = INCBIN_U16("graphics/pokemon_storage/scrolling_bg.gbapal");

static const u16 sScrollingBg_Gfx[] = INCBIN_U16("graphics/pokemon_storage/scrolling_bg.4bpp");

static const u8 sScrollingBg_Tilemap[] = INCBIN_U8("graphics/pokemon_storage/scrolling_bg_map.bin.lz");

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

static const u16 sWaveform_Pal[] = INCBIN_U16("graphics/pokemon_storage/waveform.gbapal");

static const u8 sWaveform_Gfx[] = INCBIN_U8("graphics/pokemon_storage/waveform.4bpp");

const u16 gUnknown_083B6D74[] = INCBIN_U16("graphics/pokemon_storage/83B6D74.gbapal");

const u16 gUnknown_083B6D94[] = INCBIN_U16("graphics/pokemon_storage/83B6D94.gbapal");

struct PokemonStorageSystemData *const gPokemonStorageSystemPtr = &ePokemonStorageSystem;

u8 *const gUnknown_083B6DB8 = gTileBuffer;

static const struct SpritePalette sWaveformSpritePalette = {
    sWaveform_Pal, 0xdacd
};

// .text

void EnterPokeStorage(u8 boxOption)
{
    sCurrentBoxOption = boxOption;
    gPokemonStorageSystemPtr->boxOption = boxOption;
    SetMainCallback2(sub_8096884);
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
    sInPartyMenu = (gPokemonStorageSystemPtr->boxOption == 1 ? 1 : 0);
    sDepositBoxId = 0;
}

void sub_8096874(void)
{
    REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG_ALL_ON | DISPCNT_OBJ_ON;
}

extern u8 unk_2038790;

void sub_8096884(void)
{
    switch (gMain.state)
    {
    case 0:
        SetVBlankCallback(NULL);
        REG_DISPCNT = 0;
        ResetForPokeStorage();
#if DEBUG
        unk_2038790 = 0;
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
        sub_8099BF8(gPokemonStorage.currentBox);
        gPokemonStorageSystemPtr->markMenu.baseTileTag = 0x000a;
        gPokemonStorageSystemPtr->markMenu.basePaletteTag = 0xdacb;
        sub_80F727C(&gPokemonStorageSystemPtr->markMenu);
        sub_80F7404();
        gMain.state++;
        break;
    case 9:
        sub_8096874();
        SetPSSCallback(sub_8096BF0);
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
        gPokemonStorageSystemPtr->boxOption = sCurrentBoxOption;
        ResetForPokeStorage();
        if (sWhichToReshow == 1)
            LoadSavedMovingMon();
        if (sWhichToReshow == 0)
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
        sub_8099BF8(gPokemonStorage.currentBox);
        gPokemonStorageSystemPtr->markMenu.baseTileTag = 0x000a;
        gPokemonStorageSystemPtr->markMenu.basePaletteTag = 0xdacb;
        sub_80F727C(&gPokemonStorageSystemPtr->markMenu);
        sub_80F7404();
        gMain.state++;
        break;
    case 9:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        SetVBlankCallback(VBlankCB_PokeStorage);
        gMain.state++;
        break;
    case 10:
        sub_8096874();
        SetPSSCallback(sub_8096C68);
        SetMainCallback2(CB2_PokeStorage);
        gMain.state++;
        break;
    }
}

#if DEBUG

void debug_sub_80A4300(void)
{
    sCurrentBoxOption = 0;
    gPokemonStorageSystemPtr->boxOption = 0;
    sub_8096884();
    if (gMain.callback2 == CB2_PokeStorage)
        unk_2038790 = 1;
}

extern void (*unk_2038794)(void);
extern struct Pokemon * unk_2038798;

extern void sub_809CB94(struct Pokemon *);

void debug_sub_80A433C(struct Pokemon * a, void (*b)(void))
{
    unk_2038794 = b;
    unk_2038798 = a;
    SetMainCallback2(debug_sub_80A4300);
}

void debug_sub_80A435C(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        sub_809CB94(unk_2038798);
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        gPokemonStorageSystemPtr->state++;
        break;
    case 1:
        if (!UpdatePaletteFade())
            unk_2038794();
        break;
    }
}

#endif

static void VBlankCB_PokeStorage(void)
{
    REG_BG2HOFS = gPokemonStorageSystemPtr->bg2_X;
    REG_BG3HOFS = gPokemonStorageSystemPtr->unk_000a;
    REG_BG3VOFS = gPokemonStorageSystemPtr->unk_0008;
    LoadOam();
    ProcessSpriteCopyRequests();
    UnkUtil_Run();
    TransferPlttBuffer();
}

static void CB2_PokeStorage(void)
{
    gPokemonStorageSystemPtr->unk_0000();
    ScrollBackground();
    UpdateCloseBoxButtonFlash();
    AnimateSprites();
    BuildOamBuffer();
}

void sub_8096B5C(void)
{
    switch (gPokemonStorageSystemPtr->screenChangeType)
    {
    case 0:
        ShowPokemonSummaryScreen(gPokemonStorageSystemPtr->summaryMon.mon, gPokemonStorageSystemPtr->summaryStartPos, gPokemonStorageSystemPtr->summaryMaxPos, CB2_ReturnToPokeStorage, gPokemonStorageSystemPtr->summaryScreenMode);
        break;
    case 1:
        DoNamingScreen(1, gPokemonStorage.boxNames[gPokemonStorage.currentBox], 0, 0, 0, CB2_ReturnToPokeStorage);
        break;
    }
}

void SetPSSCallback(void (*func)(void))
{
    gPokemonStorageSystemPtr->unk_0000 = func;
    gPokemonStorageSystemPtr->state = 0;
}

void sub_8096BF0(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        BlendPalettes(0xFFFFFFFF, 16, RGB(0, 0, 0));
        gPokemonStorageSystemPtr->state++;
        break;
    case 1:
        PlaySE(SE_PC_LOGIN);
        gPokemonStorageSystemPtr->unk_000c.tileTag = 14;
        gPokemonStorageSystemPtr->unk_000c.paletteTag = 0xdad0;
        gPokemonStorageSystemPtr->unk_000c.unk04 = 0;
        gPokemonStorageSystemPtr->unk_000c.unk06 = 0;
        sub_80C5CD4(&gPokemonStorageSystemPtr->unk_000c);
        BlendPalettes(0xFFFFFFFF, 0, RGB(0, 0, 0));
        gPokemonStorageSystemPtr->state++;
        break;
    case 2:
        if (sub_80C5DCC())
            SetPSSCallback(sub_8096C84);
        break;
    }
}

void sub_8096C68(void)
{
    if (!UpdatePaletteFade())
        SetPSSCallback(sub_8096C84);
}

void sub_8096C84(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        switch (HandleInput())
        {
        case 1:
            PlaySE(SE_SELECT);
            gPokemonStorageSystemPtr->state = 1;
            break;
        case 5:
            if (gPokemonStorageSystemPtr->boxOption != 2)
            {
                PrintMessage(MSG_WHICH_ONE_WILL_TAKE);
                gPokemonStorageSystemPtr->state = 3;
            }
            else
            {
                ClearSavedCursorPos();
                SetPSSCallback(sub_8096FC8);
            }
            break;
        case 6:
            if (gPokemonStorageSystemPtr->boxOption == 2)
            {
                if (IsMonBeingMoved() && ItemIsMail(gPokemonStorageSystemPtr->displayMonItemId))
                    gPokemonStorageSystemPtr->state = 5;
                else
                    SetPSSCallback(sub_8097004);
            }
            break;
        case 4:
#if DEBUG
            if (unk_2038790 != 0)
                break;
#endif
            SetPSSCallback(sub_8097BA0);
            break;
        case 16:
#if DEBUG
            if (unk_2038790 != 0)
                break;
#endif
            SetPSSCallback(sub_8097CC0);
            break;
        case 7:
            PlaySE(SE_SELECT);
            SetPSSCallback(sub_809789C);
            break;
        case 8:
            PlaySE(SE_SELECT);
            SetPSSCallback(sub_8097078);
            break;
        case 9:
            PlaySE(SE_SELECT);
            gPokemonStorageSystemPtr->newCurrBoxId = gPokemonStorage.currentBox + 1;
            if (gPokemonStorageSystemPtr->newCurrBoxId > 13)
                gPokemonStorageSystemPtr->newCurrBoxId = 0;
            SetUpScrollToBox(gPokemonStorageSystemPtr->newCurrBoxId);
            gPokemonStorageSystemPtr->state = 2;
            break;
        case 10:
            PlaySE(SE_SELECT);
            gPokemonStorageSystemPtr->newCurrBoxId = gPokemonStorage.currentBox - 1;
            if (gPokemonStorageSystemPtr->newCurrBoxId < 0)
                gPokemonStorageSystemPtr->newCurrBoxId = 13;
            SetUpScrollToBox(gPokemonStorageSystemPtr->newCurrBoxId);
            gPokemonStorageSystemPtr->state = 2;
            break;
        case 11:
            if (!IsRemovingLastPartyMon())
            {
                if (ItemIsMail(gPokemonStorageSystemPtr->displayMonItemId))
                {
                    gPokemonStorageSystemPtr->state = 5;
                }
                else
                {
                    PlaySE(SE_SELECT);
                    SetPSSCallback(sub_809746C);
                }
            }
            else
            {
                gPokemonStorageSystemPtr->state = 4;
            }
            break;
        case 13:
            if (IsRemovingLastPartyMon())
            {
                gPokemonStorageSystemPtr->state = 4;
            }
            else
            {
                PlaySE(SE_SELECT);
                SetPSSCallback(sub_80972A8);
            }
            break;
        case 14:
            if (!CanShiftMon())
            {
                gPokemonStorageSystemPtr->state = 4;
            }
            else
            {
                PlaySE(SE_SELECT);
                SetPSSCallback(c3_0808DC50);
            }
            break;
        case 12:
            PlaySE(SE_SELECT);
            SetPSSCallback(sub_8097390);
            break;
        case 15:
            PlaySE(SE_SELECT);
            SetPSSCallback(sub_80972FC);
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
            if (!sInPartyMenu && !IsMonBeingMoved())
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
        PrintMessage(MSG_LAST_POKE);
        gPokemonStorageSystemPtr->state = 6;
        break;
    case 5:
        PlaySE(SE_FAILURE);
        PrintMessage(MSG_PLEASE_REMOVE_MAIL);
        gPokemonStorageSystemPtr->state = 6;
        break;
    case 6:
        if (JOY_NEW(A_BUTTON | B_BUTTON | DPAD_ANY))
        {
            ClearBottomWindow();
            SetPSSCallback(sub_8096C84);
        }
        break;
    }
}

void sub_8096FC8(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
        case 0:
            SetUpDoShowPartyMenu();
            gPokemonStorageSystemPtr->state++;
            break;
        case 1:
            if (!DoShowPartyMenu())
                SetPSSCallback(sub_8096C84);
            break;
    }
}

void sub_8097004(void)
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
                SetPSSCallback(sub_8096C84);
            }
            break;
    }
}

void sub_8097078(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        PrintMessage(MSG_IS_SELECTED);
        AddMenu();
        gPokemonStorageSystemPtr->state = 1;
        break;
    case 1:
        switch (HandleMenuInput())
        {
        case -1:
        case  0:
            ClearBottomWindow();
            SetPSSCallback(sub_8096C84);
            break;
        case 3:
            if (IsRemovingLastPartyMon())
            {
                gPokemonStorageSystemPtr->state = 2;
            }
            else
            {
                PlaySE(SE_SELECT);
                ClearBottomWindow();
                SetPSSCallback(sub_80972A8);
            }
            break;
        case 5:
            PlaySE(SE_SELECT);
            ClearBottomWindow();
            SetPSSCallback(sub_80972FC);
            break;
        case 4:
            if (!CanShiftMon())
            {
                gPokemonStorageSystemPtr->state = 2;
            }
            else
            {
                PlaySE(SE_SELECT);
                ClearBottomWindow();
                SetPSSCallback(c3_0808DC50);
            }
            break;
        case 2:
            PlaySE(SE_SELECT);
            ClearBottomWindow();
            SetPSSCallback(sub_8097390);
            break;
        case 1:
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
                SetPSSCallback(sub_809746C);
            }
            break;
        case 7:
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
                SetPSSCallback(sub_8097594);
            }
            break;
        case 6:
            PlaySE(SE_SELECT);
            SetPSSCallback(sub_8097788);
            break;
        case 8:
            PlaySE(SE_SELECT);
            SetPSSCallback(sub_80977E4);
            break;
#if DEBUG
        case 32:
            PlaySE(SE_SELECT);
            ClearBottomWindow();
            SetPSSCallback(debug_sub_80A435C);
            break;
#endif
        }
        break;
    case 2:
        PlaySE(SE_FAILURE);
        PrintMessage(MSG_LAST_POKE);
        gPokemonStorageSystemPtr->state = 5;
        break;
    case 4:
        PlaySE(SE_FAILURE);
        PrintMessage(MSG_CANT_RELEASE_EGG);
        gPokemonStorageSystemPtr->state = 5;
        break;
    case 3:
        PlaySE(SE_FAILURE);
        PrintMessage(MSG_PLEASE_REMOVE_MAIL);
        gPokemonStorageSystemPtr->state = 5;
        break;
    case 5:
        if (JOY_NEW(A_BUTTON | B_BUTTON | DPAD_ANY))
        {
            ClearBottomWindow();
            SetPSSCallback(sub_8096C84);
        }
        break;
    }
}

void sub_80972A8(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        InitMonPlaceChange(0);
        gPokemonStorageSystemPtr->state++;
        break;
    case 1:
        if (!DoMonPlaceChange())
        {
            if (sInPartyMenu)
                SetPSSCallback(sub_8097858);
            else
                SetPSSCallback(sub_8096C84);
        }
        break;
    }
}

void sub_80972FC(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        InitMonPlaceChange(1);
        gPokemonStorageSystemPtr->state++;
        break;
    case 1:
        if (!DoMonPlaceChange())
        {
            if (sInPartyMenu)
                SetPSSCallback(sub_8097858);
            else
                SetPSSCallback(sub_8096C84);
        }
        break;
    }
}

void c3_0808DC50(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        InitMonPlaceChange(2);
        gPokemonStorageSystemPtr->state++;
        break;
    case 1:
        if (!DoMonPlaceChange())
        {
            StartDisplayMonMosaicEffect();
            SetPSSCallback(sub_8096C84);
        }
        break;
    }
}

void sub_8097390(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        if (CalculatePlayerPartyCount() == 6)
        {
            PrintMessage(MSG_PARTY_FULL);
            gPokemonStorageSystemPtr->state = 1;
        }
        else
        {
            SaveCursorPos();
            InitMonPlaceChange(0);
            gPokemonStorageSystemPtr->state = 2;
        }
        break;
    case 1:
        if (JOY_NEW(A_BUTTON | B_BUTTON | DPAD_ANY))
        {
            ClearBottomWindow();
            SetPSSCallback(sub_8096C84);
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
            InitMonPlaceChange(1);
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
        SetPSSCallback(sub_8097004);
        break;
    }
}

void sub_809746C(void)
{
    u8 r4;

    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        PrintMessage(MSG_DEPOSIT_IN_WHICH_BOX);
        LoadChooseBoxMenuGfx(&gPokemonStorageSystemPtr->chooseBoxMenu, 0x0007, 0xdaca, 3);
        CreateChooseBoxMenuSprites(sDepositBoxId);
        gPokemonStorageSystemPtr->state++;
        break;
    case 1:
        r4 = HandleChooseBoxMenuInput();
        if (r4 == 200);
        else if (r4 == 201)
        {
            ClearBottomWindow();
            DestroyChooseBoxMenuSprites();
            FreeChooseBoxMenu();
            SetPSSCallback(sub_8096C84);
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
                PrintMessage(MSG_BOX_IS_FULL);
                gPokemonStorageSystemPtr->state = 4;
            }
            sDepositBoxId = r4;
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
            SetPSSCallback(sub_8096C84);
        }
        break;
    case 4:
        if (JOY_NEW(A_BUTTON | B_BUTTON))
        {
            PrintMessage(MSG_DEPOSIT_IN_WHICH_BOX);
            gPokemonStorageSystemPtr->state = 1;
        }
        break;
    }
}

void sub_8097594(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        PrintMessage(MSG_RELEASE_POKE);
        ShowYesNoWindow(1);
        gPokemonStorageSystemPtr->state++;
        // fallthrough
    case 1:
        switch (Menu_ProcessInputNoWrap())
        {
            case -1:
            case  1:
                ClearBottomWindow();
                SetPSSCallback(sub_8096C84);
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
        PrintMessage(MSG_WAS_RELEASED);
        gPokemonStorageSystemPtr->state++;
        break;
    case 4:
        if (JOY_NEW(A_BUTTON | B_BUTTON))
        {
            PrintMessage(MSG_BYE_BYE);
            gPokemonStorageSystemPtr->state++;
        }
        break;
    case 5:
        if (JOY_NEW(A_BUTTON | B_BUTTON))
        {
            ClearBottomWindow();
            if (sInPartyMenu)
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
        SetPSSCallback(sub_8096C84);
        break;
    case 8:
        PrintMessage(MSG_WAS_RELEASED);
        gPokemonStorageSystemPtr->state++;
        break;
    case 9:
        if (JOY_NEW(A_BUTTON | B_BUTTON))
        {
            PrintMessage(MSG_SURPRISE);
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
            PrintMessage(MSG_CAME_BACK);
            gPokemonStorageSystemPtr->state++;
        }
        break;
    case 12:
        if (JOY_NEW(A_BUTTON | B_BUTTON))
        {
            PrintMessage(MSG_WORRIED);
            gPokemonStorageSystemPtr->state++;
        }
        break;
    case 13:
        if (JOY_NEW(A_BUTTON | B_BUTTON))
        {
            ClearBottomWindow();
            SetPSSCallback(sub_8096C84);
        }
        break;
    }
}

void sub_8097788(void)
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
            sWhichToReshow = 0;
            gPokemonStorageSystemPtr->screenChangeType = 0;
            SetMainCallback2(sub_8096B5C);
        }
        break;
    }
}

void sub_80977E4(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        PrintMessage(MSG_MARK_POKE);
        gPokemonStorageSystemPtr->markMenu.markings = gPokemonStorageSystemPtr->displayMonMarkings;
        sub_80F7418(gPokemonStorageSystemPtr->displayMonMarkings, 0xb0, 0x10);
        gPokemonStorageSystemPtr->state++;
        break;
    case 1:
        if (!sub_80F7500())
        {
            sub_80F7470();
            ClearBottomWindow();
            sub_809BDD8(gPokemonStorageSystemPtr->markMenu.markings);
            RefreshDisplayMonData();
            SetPSSCallback(sub_8096C84);
        }
        break;
    }
}

void sub_8097858(void)
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
            SetPSSCallback(sub_8096C84);
        }
        break;
    }
}

void sub_809789C(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        PrintMessage(MSG_WHAT_YOU_DO);
        AddMenu();
        gPokemonStorageSystemPtr->state++;
        break;
    case 1:
        switch (HandleMenuInput())
        {
        case -1:
        case  0:
            AnimateBoxScrollArrows(TRUE);
            ClearBottomWindow();
            SetPSSCallback(sub_8096C84);
            break;
        case 11:
            PlaySE(SE_SELECT);
            SetPSSCallback(sub_8097B44);
            break;
        case 10:
            PlaySE(SE_SELECT);
            ClearBottomWindow();
            SetPSSCallback(sub_8097974);
            break;
        case 9:
            PlaySE(SE_SELECT);
            ClearBottomWindow();
            SetPSSCallback(sub_8097A64);
            break;
        }
        break;
    }
}

void sub_8097974(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        AddWallpaperSetsMenu();
        PrintMessage(MSG_PICK_A_THEME);
        gPokemonStorageSystemPtr->state++;
        break;
    case 1:
        gPokemonStorageSystemPtr->wallpaperSetId = HandleMenuInput();
        switch (gPokemonStorageSystemPtr->wallpaperSetId)
        {
        case -1:
            AnimateBoxScrollArrows(TRUE);
            ClearBottomWindow();
            SetPSSCallback(sub_8096C84);
            break;
        case 12 ... 15:
            PlaySE(SE_SELECT);
            gPokemonStorageSystemPtr->wallpaperSetId -= 12;
            AddWallpapersMenu(gPokemonStorageSystemPtr->wallpaperSetId);
            PrintMessage(MSG_PICK_A_WALLPAPER);
            gPokemonStorageSystemPtr->state++;
            break;
        }
        break;
    case 2:
        gPokemonStorageSystemPtr->wallpaperId = HandleMenuInput();
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
            gPokemonStorageSystemPtr->wallpaperId -= 16;
            SetWallpaperForCurrentBox(gPokemonStorageSystemPtr->wallpaperId);
            gPokemonStorageSystemPtr->state++;
            break;
        }
        break;
    case 3:
        if (!DoWallpaperGfxChange())
        {
            AnimateBoxScrollArrows(TRUE);
            SetPSSCallback(sub_8096C84);
        }
        break;
    }
}

void sub_8097A64(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        PrintMessage(MSG_JUMP_TO_WHICH_BOX);
        LoadChooseBoxMenuGfx(&gPokemonStorageSystemPtr->chooseBoxMenu, 0x0007, 0xdaca, 3);
        CreateChooseBoxMenuSprites(gPokemonStorage.currentBox);
        gPokemonStorageSystemPtr->state++;
        break;
    case 1:
        gPokemonStorageSystemPtr->newCurrBoxId = HandleChooseBoxMenuInput();
        switch (gPokemonStorageSystemPtr->newCurrBoxId)
        {
        case 200:
            break;
        default:
            ClearBottomWindow();
            DestroyChooseBoxMenuSprites();
            FreeChooseBoxMenu();
            if (gPokemonStorageSystemPtr->newCurrBoxId == 201 || gPokemonStorageSystemPtr->newCurrBoxId == gPokemonStorage.currentBox)
            {
                AnimateBoxScrollArrows(TRUE);
                SetPSSCallback(sub_8096C84);
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
            SetPSSCallback(sub_8096C84);
        }
        break;
    }
}

void sub_8097B44(void)
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
            sWhichToReshow = 1;
            gPokemonStorageSystemPtr->screenChangeType = 1;
            SetMainCallback2(sub_8096B5C);
        }
        break;
    }
}

void sub_8097BA0(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        if (IsMonBeingMoved())
        {
            PlaySE(SE_FAILURE);
            PrintMessage(MSG_HOLDING_POKE);
            gPokemonStorageSystemPtr->state = 1;
        }
        else
        {
            PlaySE(SE_SELECT);
            PrintMessage(MSG_EXIT_BOX);
            ShowYesNoWindow(0);
            gPokemonStorageSystemPtr->state = 2;
        }
        break;
    case 1:
        if (JOY_NEW(A_BUTTON | B_BUTTON | DPAD_ANY))
        {
            ClearBottomWindow();
            SetPSSCallback(sub_8096C84);
        }
        break;
    case 2:
        switch (Menu_ProcessInputNoWrap())
        {
        case 1:
        case -1:
            ClearBottomWindow();
            SetPSSCallback(sub_8096C84);
            break;
        case 0:
            PlaySE(SE_PC_OFF);
            ClearBottomWindow();
            gPokemonStorageSystemPtr->state++;
            break;
        }
        break;
    case 3:
        gPokemonStorageSystemPtr->unk_000c.tileTag = 0x000e;
        gPokemonStorageSystemPtr->unk_000c.paletteTag = 0xdad0;
        gPokemonStorageSystemPtr->unk_000c.unk04 = 20;
        gPokemonStorageSystemPtr->unk_000c.unk06 = 0;
        sub_80C5E38(&gPokemonStorageSystemPtr->unk_000c);
        gPokemonStorageSystemPtr->state++;
        break;
    case 4:
        if (sub_80C5F98())
        {
            gPlayerPartyCount = CalculatePlayerPartyCount();
            SetMainCallback2(CB2_ExitPokeStorage);
        }
        break;
    }
}

void sub_8097CC0(void)
{
    switch (gPokemonStorageSystemPtr->state)
    {
    case 0:
        if (IsMonBeingMoved())
        {
            PlaySE(SE_FAILURE);
            PrintMessage(MSG_HOLDING_POKE);
            gPokemonStorageSystemPtr->state = 1;
        }
        else
        {
            PlaySE(SE_SELECT);
            PrintMessage(MSG_CONTINUE_BOX);
            ShowYesNoWindow(0);
            gPokemonStorageSystemPtr->state = 2;
        }
        break;
    case 1:
        if (JOY_NEW(A_BUTTON | B_BUTTON | DPAD_ANY))
        {
            ClearBottomWindow();
            SetPSSCallback(sub_8096C84);
        }
        break;
    case 2:
        switch (Menu_ProcessInputNoWrap())
        {
        case 0:
            ClearBottomWindow();
            SetPSSCallback(sub_8096C84);
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
        gPokemonStorageSystemPtr->unk_000c.tileTag = 0x000e;
        gPokemonStorageSystemPtr->unk_000c.paletteTag = 0xdad0;
        gPokemonStorageSystemPtr->unk_000c.unk04 = 20;
        gPokemonStorageSystemPtr->unk_000c.unk06 = 0;
        sub_80C5E38(&gPokemonStorageSystemPtr->unk_000c);
        gPokemonStorageSystemPtr->state++;
        break;
    case 4:
        if (sub_80C5F98())
        {
            gPlayerPartyCount = CalculatePlayerPartyCount();
            SetMainCallback2(CB2_ExitPokeStorage);
        }
        break;
    }
}

static void SetScrollingBackground(void)
{
    gPokemonStorageSystemPtr->unk_0007 = 0;
    gPokemonStorageSystemPtr->unk_0008 = 0;
    gPokemonStorageSystemPtr->unk_000a = 0;
    REG_BG3CNT = BGCNT_PRIORITY(3) | BGCNT_CHARBASE(3) | BGCNT_SCREENBASE(30);
    DmaCopy16Defvars(3, sScrollingBg_Gfx, BG_SCREEN_ADDR(28), sizeof sScrollingBg_Gfx);
    LZ77UnCompVram(sScrollingBg_Tilemap, BG_SCREEN_ADDR(30));
    LoadPalette(sScrollingBg_Pal, 0xd0, 0x10);
}

static void ScrollBackground(void)
{
    if (++gPokemonStorageSystemPtr->unk_0007 >= 2)
    {
        gPokemonStorageSystemPtr->unk_0007 = 0;
        gPokemonStorageSystemPtr->unk_0008--;
        gPokemonStorageSystemPtr->unk_000a++;
    }
}

static void InitPalettesAndSprites(void)
{
    LZ77UnCompVram(gPSSMenuHeader_Gfx, BG_SCREEN_ADDR(10));
    LZ77UnCompWram(gPSSMenuHeader_Tilemap, gUnknown_02039760);
    sub_809D034(BG_SCREEN_ADDR(15), 0, 0, gUnknown_02039760, 0, 0, 10, 20);
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
    LoadSpritePalette(&sWaveformSpritePalette);
    CreateDisplayMonSprite();
    CreateMarkingComboSprite();
    CreateWaveformSprites();
    RefreshDisplayMonData();
}

static void CreateMarkingComboSprite(void)
{
    gPokemonStorageSystemPtr->markingComboSprite = sub_80F7940(0x000d, 0xdace, 0);
    gPokemonStorageSystemPtr->markingComboSprite->oam.priority = 0;
    gPokemonStorageSystemPtr->markingComboSprite->subpriority = 1;
    gPokemonStorageSystemPtr->markingComboSprite->x = 0x28;
    gPokemonStorageSystemPtr->markingComboSprite->y = 0x95;
    gPokemonStorageSystemPtr->markingComboTilesPtr = BG_CHAR_ADDR(4) + 32 * GetSpriteTileStartByTag(0x000d);
}

static const struct SpriteTemplate sSpriteTemplate_Waveform;

static void CreateWaveformSprites(void)
{
    u16 i;
    struct SpriteSheet sheet = {sWaveform_Gfx, 0x1c0, 0x0005};

    LoadSpriteSheet(&sheet);
    for (i = 0; i < 2; i++)
    {
        u8 spriteId = CreateSprite(&sSpriteTemplate_Waveform, i * 63 + 8, 9, 2);
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

// sub_80980D4

static const struct OamData sOamData_DisplayMon;

static void CreateDisplayMonSprite(void)
{
    u16 i;
    u16 tileStart;
    u8 palSlot;
    u8 spriteId;

    struct SpriteSheet sheet = {
        .data = gPokemonStorageSystemPtr->tileBuffer,
        .size = 0x800,
        .tag = 0x0002
    };
    struct SpritePalette palette = {
        .data = gPokemonStorageSystemPtr->displayMonPalBuffer,
        .tag = 0xDAC7
    };
    struct SpriteTemplate template = {
        .tileTag = 0x0002,
        .paletteTag = 0xDAC7,
        .oam = &sOamData_DisplayMon,
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
        tileStart = LoadSpriteSheet(&sheet);
        if (tileStart == 0)
            break;

        palSlot = LoadSpritePalette(&palette);
        if (palSlot == 0xFF)
            break;

        spriteId = CreateSprite(&template, 0x28, 0x30, 0);
        if (spriteId == MAX_SPRITES)
            break;

        gPokemonStorageSystemPtr->displayMonSprite = &gSprites[spriteId];
        gPokemonStorageSystemPtr->displayMonPalOffset = 0x100 + 16 * palSlot;
        gPokemonStorageSystemPtr->displayMonTilePtr = (void *)(OBJ_VRAM0 + 32 * tileStart);
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
            HandleLoadSpecialPokePic(gMonFrontPicTable + species, gMonFrontPicCoords[species].size, 1, gPokemonStorageSystemPtr->unk_4784, gPokemonStorageSystemPtr->tileBuffer, species, pid);
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
        sub_80F7A10(gPokemonStorageSystemPtr->displayMonMarkings, gPokemonStorageSystemPtr->markingComboTilesPtr);
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
        sub_809D034(BG_SCREEN_ADDR(15), 1, 0, gUnknown_02039760, 1, 0, 8, 2);
        for (i = 0; i < 2; i++)
            StartSpriteAnimIfDifferent(gPokemonStorageSystemPtr->waveformSprites[i], i * 2 + 1);
    }
    else
    {
        sub_809D034(BG_SCREEN_ADDR(15), 1, 0, gUnknown_02039760, 10, 0, 8, 2);
        for (i = 0; i < 2; i++)
            StartSpriteAnim(gPokemonStorageSystemPtr->waveformSprites[i], i * 2);
    }
}

static void InitSupplementalTilemaps(void)
{
    REG_BG1CNT = BGCNT_PRIORITY(1) | BGCNT_SCREENBASE(15);
    LZ77UnCompVram(gPSSMenuMisc_Gfx, BG_SCREEN_ADDR(13));
    LZ77UnCompWram(gPSSMenuMisc_Tilemap, gPokemonStorageSystemPtr->unk_00a8);
    LoadPalette(gPSSMenu3_Pal, 0x20, 0x20);
    LoadPalette(gPSSMenu4_Pal, 0x30, 0x20);
    DmaClear16(3, BG_SCREEN_ADDR(15), 0x800);
    SetPartySlotTilemaps();
    if (sInPartyMenu)
    {
        UpdateCloseBoxButtonTilemap(TRUE);
        CreatePartyMonsSprites(TRUE);
        sub_809D034(BG_SCREEN_ADDR(15), 10, 0, gPokemonStorageSystemPtr->unk_00a8, 0, 0, 12, 22);
    }
    else
    {
        sub_809D034(BG_SCREEN_ADDR(15), 10, 0, gPokemonStorageSystemPtr->unk_00a8, 0, 20, 12, 2);
        UpdateCloseBoxButtonTilemap(TRUE);
    }
    gPokemonStorageSystemPtr->closeBoxFlashing = 0;
}

static void SetUpShowPartyMenu(void)
{
    gPokemonStorageSystemPtr->unk_08a8 = 20;
    gPokemonStorageSystemPtr->partyMenuY = 2;
    gPokemonStorageSystemPtr->partyMenuMoveTimer = 0;
    CreatePartyMonsSprites(FALSE);
}

static bool8 ShowPartyMenu(void)
{
    if (gPokemonStorageSystemPtr->partyMenuMoveTimer == 20)
        return FALSE;
    gPokemonStorageSystemPtr->unk_08a8--;
    gPokemonStorageSystemPtr->partyMenuY++;
    sub_809D034(BG_SCREEN_ADDR(15), 10, 0, gPokemonStorageSystemPtr->unk_00a8, 0, gPokemonStorageSystemPtr->unk_08a8, 12, gPokemonStorageSystemPtr->partyMenuY);
    MovePartySprites(8);
    if (++gPokemonStorageSystemPtr->partyMenuMoveTimer == 20)
    {
        sInPartyMenu = 1;
        return FALSE;
    }
    return TRUE;
}

static void SetUpHidePartyMenu(void)
{
    gPokemonStorageSystemPtr->unk_08a8 = 0;
    gPokemonStorageSystemPtr->partyMenuY = 22;
    gPokemonStorageSystemPtr->partyMenuMoveTimer = 0;
}

static bool8 HidePartyMenu(void)
{
    if (gPokemonStorageSystemPtr->partyMenuMoveTimer == 20)
        return FALSE;
    gPokemonStorageSystemPtr->unk_08a8++;
    gPokemonStorageSystemPtr->partyMenuY--;
    sub_809D034(BG_SCREEN_ADDR(15), 10, 0, gPokemonStorageSystemPtr->unk_00a8, 0, gPokemonStorageSystemPtr->unk_08a8, 12, gPokemonStorageSystemPtr->partyMenuY);
    UnkUtil_DmaAdd(BG_SCREEN_ADDR(15), 10, gPokemonStorageSystemPtr->partyMenuY, 12, 1);
    MovePartySprites(-8);
    if (++gPokemonStorageSystemPtr->partyMenuMoveTimer == 20)
    {
        sInPartyMenu = 0;
        DestroyAllPartyMonIcons();
        CompactPartySlots();
        sub_809D034(BG_SCREEN_ADDR(15), 21, 0, gPokemonStorageSystemPtr->unk_00a8, 12, 0, 1, 2);
        return FALSE;
    }
    return TRUE;
}

static void UpdateCloseBoxButtonTilemap(bool8 flag)
{
    if (flag)
        sub_809D034(BG_SCREEN_ADDR(15), 21, 0, gPokemonStorageSystemPtr->unk_00a8, 12, 0, 9, 2);
    else
        sub_809D034(BG_SCREEN_ADDR(15), 21, 0, gPokemonStorageSystemPtr->unk_00a8, 12, 2, 9, 2);
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
        sub_809D104(gPokemonStorageSystemPtr->unk_00a8, 7, (i - 1) * 3 + 1, gPokemonStorageSystemPtr->unk_00a8, r1, 4, 4, 3);
    }
}

static void UpdatePartySlotColors(void)
{
    SetPartySlotTilemaps();
    sub_809D034(BG_SCREEN_ADDR(15), 10, 0, gPokemonStorageSystemPtr->unk_00a8, 0, 0, 12, 22);
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
    {gText_ExitFromBox, 0},
    {gText_WhatDoYouWantToDo, 0},
    {gText_PleasePickATheme, 0},
    {gText_PickTheWallpaper, 0},
    {gText_PkmnIsSelected, 1},
    {gText_JumpToWhichBox, 0},
    {gText_DepositInWhichBox, 0},
    {gText_PkmnWasDeposited, 1},
    {gText_BoxIsFull2, 0},
    {gText_ReleaseThisPokemon, 0},
    {gText_PkmnWasReleased, 4},
    {gText_ByeByePkmn, 6},
    {gText_MarkYourPkmn, 0},
    {gText_ThatsYourLastPkmn, 0},
    {gText_YourPartysFull, 0},
    {gText_YoureHoldingAPkmn, 0},
    {gText_WhichOneWillYouTake, 0},
    {gText_YouCantReleaseAnEgg, 0},
    {gText_ContinueBoxOperations, 0},
    {gText_PkmnCameBack, 1},
    {gText_WasItWorriedAboutYou, 0},
    {gText_FourEllipsesExclamation, 0},
    {gText_PleaseRemoveTheMail, 0}
};

static void PrintMessage(u8 id)
{
    u8 *txtPtr;

    Menu_DrawStdWindowFrame(10, 16, 29, 19);

    switch (sMessages[id].format)
    {
    case MSG_VAR_MON_NAME_2:
        txtPtr = StringCopy(gPokemonStorageSystemPtr->messageText, sMessages[id].text);
        txtPtr = StringCopy(txtPtr, gPokemonStorageSystemPtr->displayMonName);
        break;
    case MSG_VAR_RELEASE_MON_2:
        txtPtr = StringCopy(gPokemonStorageSystemPtr->messageText, sMessages[id].text);
        txtPtr = StringCopy(txtPtr, gPokemonStorageSystemPtr->releaseMonName);
        break;
    case MSG_VAR_MON_NAME_1:
        // {var} + " is selected."
        txtPtr = StringCopy(gPokemonStorageSystemPtr->messageText, gPokemonStorageSystemPtr->displayMonName);
        txtPtr = StringCopy(txtPtr, sMessages[id].text);
        break;
    case MSG_VAR_RELEASE_MON_1:
        // {var} + " was released."
        txtPtr = StringCopy(gPokemonStorageSystemPtr->messageText, gPokemonStorageSystemPtr->releaseMonName);
#if ENGLISH
        txtPtr = StringCopy(txtPtr, sMessages[id].text);
#elif GERMAN
        txtPtr = de_sub_8073174(gPokemonStorageSystemPtr->messageText, sMessages[id].text);
#endif
        break;
    case MSG_VAR_MON_NAME_3:
        {
            const u8 *stringLength;
            const u8 *text;

            text = sMessages[id].text;
            stringLength = &text[StringLength(text)] + 1;

            txtPtr = StringCopy(gPokemonStorageSystemPtr->messageText, sMessages[id].text);
            txtPtr = StringCopy(txtPtr, gPokemonStorageSystemPtr->displayMonName);
            txtPtr = StringCopy(txtPtr, stringLength);
        }
        break;
    case MSG_VAR_RELEASE_MON_3:
        {
            const u8 *stringLength;
            const u8 *text;

            text = sMessages[id].text;
            stringLength = &text[StringLength(text)] - 1;

            txtPtr = StringCopy(gPokemonStorageSystemPtr->messageText, sMessages[id].text);
            txtPtr = StringCopy(txtPtr - 1, gPokemonStorageSystemPtr->releaseMonName);
            txtPtr = StringCopy(txtPtr, stringLength);
        }
        break;
    case MSG_VAR_NONE:
    default:
        txtPtr = StringCopy(gPokemonStorageSystemPtr->messageText, sMessages[id].text);
        break;
    }

    while (txtPtr < gPokemonStorageSystemPtr->unk_26a6)
    {
        txtPtr[0] = CHAR_SPACE;
        txtPtr++;
    }

    txtPtr[0] = EOS;
    Menu_PrintText(gPokemonStorageSystemPtr->messageText, 11, 17);
}

static const struct OamData sOamData_DisplayMon = {
    .size = 3
};

static const struct OamData sOamData_Waveform = {
    .shape = ST_OAM_H_RECTANGLE
};

static const union AnimCmd sAnim_Waveform_LeftOff[] = {
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END
};

static const union AnimCmd sAnim_Waveform_LeftOn[] = {
    ANIMCMD_FRAME(2, 8),
    ANIMCMD_FRAME(4, 8),
    ANIMCMD_FRAME(6, 8),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_Waveform_RightOff[] = {
    ANIMCMD_FRAME(8, 5),
    ANIMCMD_END
};

static const union AnimCmd sAnim_Waveform_RightOn[] = {
    ANIMCMD_FRAME(10, 8),
    ANIMCMD_FRAME( 4, 8),
    ANIMCMD_FRAME(12, 8),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd *const sAnims_Waveform[] = {
    sAnim_Waveform_LeftOff,
    sAnim_Waveform_LeftOn,
    sAnim_Waveform_RightOff,
    sAnim_Waveform_RightOn
};

static const struct SpriteTemplate sSpriteTemplate_Waveform = {
    0x0005,
    0xdacd,
    &sOamData_Waveform,
    sAnims_Waveform,
    NULL,
    gDummySpriteAffineAnimTable,
    SpriteCallbackDummy
};

static void ShowYesNoWindow(s8 cursorPos)
{
    DisplayYesNoMenu(23, 10, 0);
    Menu_MoveCursor(cursorPos);
}

static void ClearBottomWindow(void)
{
    Menu_DestroyCursor();
    Menu_EraseWindowRect(10, 16, 29, 19);
    Menu_EraseWindowRect(23, 10, 29, 15);
}

static void AddWallpaperSetsMenu(void)
{
    sub_809CDCC();
    SetMenuText(12);
    SetMenuText(13);
    SetMenuText(14);
    SetMenuText(15);
    AddMenu();
}

static void AddWallpapersMenu(u8 wallpaperSet)
{
    sub_809CDCC();
    switch (wallpaperSet)
    {
    case 0:
        SetMenuText(16);
        SetMenuText(17);
        SetMenuText(18);
        SetMenuText(19);
        break;
    case 1:
        SetMenuText(20);
        SetMenuText(21);
        SetMenuText(22);
        SetMenuText(23);
        break;
    case 2:
        SetMenuText(24);
        SetMenuText(25);
        SetMenuText(26);
        SetMenuText(27);
        break;
    case 3:
        SetMenuText(28);
        SetMenuText(29);
        SetMenuText(30);
        SetMenuText(31);
        break;
    }
    AddMenu();
}
