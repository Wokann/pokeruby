#include "global.h"
#include "naming_screen.h"
#include "data2.h"
#include "graphics.h"
#include "field_effect.h"
#include "event_object_movement.h"
#include "field_player_avatar.h"
#include "main.h"
#include "menu.h"
#include "palette.h"
#include "pokemon_icon.h"
#include "constants/songs.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "strings2.h"
#include "task.h"
#include "text.h"
#include "trig.h"
#include "util.h"
#include "ewram.h"

#ifdef ENGLISH
#define COLUMN_COUNT 9
#elif GERMAN
#define COLUMN_COUNT 10
#endif

enum
{
    NAMING_SCREEN_PLAYER_NAME_LENGTH = 7,
    BOX_NAME_LENGTH = 8,
};

enum
{
    INPUT_NONE,
    INPUT_DPAD_UP,
    INPUT_DPAD_DOWN,
    INPUT_DPAD_LEFT,
    INPUT_DPAD_RIGHT,
    INPUT_A_BUTTON,
    INPUT_B_BUTTON,
    INPUT_LR_BUTTON,
    INPUT_SELECT,
    INPUT_START,
};

enum
{
    GFXTAG_BACK_BUTTON,
    GFXTAG_OK_BUTTON,
    GFXTAG_PAGE_SWAP_FRAME,
    GFXTAG_PAGE_SWAP_BUTTON,
    GFXTAG_PAGE_SWAP_LOWER,
    GFXTAG_PAGE_SWAP_UPPER,
    GFXTAG_PAGE_SWAP_OTHERS,
    GFXTAG_CURSOR,
    GFXTAG_CURSOR_SQUISHED,
    GFXTAG_CURSOR_FILLED,
    GFXTAG_INPUT_ARROW,
    GFXTAG_UNDERSCORE,
};

enum
{
    PALTAG_MENU,
    PALTAG_PAGE_SWAP_UPPER,
    PALTAG_PAGE_SWAP_LOWER,
    PALTAG_PAGE_SWAP_OTHERS,
    PALTAG_PAGE_SWAP,
    PALTAG_CURSOR,
    PALTAG_BUTTON,
};

enum
{
    BUTTON_PAGE,
    BUTTON_BACK,
    BUTTON_OK,
    BUTTON_COUNT,
};

enum
{
    KBPAGE_LETTERS_UPPER,
    KBPAGE_LETTERS_LOWER,
    KBPAGE_SYMBOLS,
    KBPAGE_COUNT,
};

enum
{
    STATE_FADE_IN,
    STATE_WAIT_FADE_IN,
    STATE_HANDLE_INPUT,
    STATE_MOVE_TO_OK_BUTTON,
    STATE_START_PAGE_SWAP,
    STATE_WAIT_PAGE_SWAP,
    STATE_PRESSED_OK,
    STATE_WAIT_SENT_TO_PC_MESSAGE,
    STATE_FADE_OUT,
    STATE_EXIT,
};

enum
{
    INPUT_STATE_DISABLED,
    INPUT_STATE_ENABLED,
};

struct NamingScreenTemplate
{
    u8 copyExistingString;
    u8 maxChars;
    u8 iconFunction;
    u8 addGenderIcon;
    u8 initialPage;
    u8 filler[3];
    const u8 *title;
};

struct NamingScreenData
{
 /*0x00*/ u8 state;
 /*0x01*/ u8 templateNum;
 /*0x02*/ u16 nameLeftOffset;
 /*0x04*/ u16 bg1vOffset;
 /*0x06*/ u16 bg2vOffset;
 /*0x08*/ u16 bg1Priority;
 /*0x0A*/ u16 bg2Priority;
 /*0x0C*/ u8 bgToReveal;
 /*0x0D*/ u8 bgToHide;
 /*0x0E*/ u8 currentPage;
 /*0x0F*/ u8 cursorSpriteId;
 /*0x10*/ u8 pageIndicatorSpriteId;
 /*0x11*/ u8 textBuffer[0x10];
 /*0x21*/ u8 filler21[0x13];
 /*0x34*/ const struct NamingScreenTemplate *template;
 /*0x38*/ u8 *destBuffer;
 /*0x3C*/ u16 keyRepeatStartDelayCopy;
 /*0x3E*/ u16 speciesOrPlayerGender;
 /*0x40*/ u16 monGender;
 /*0x42*/ u32 monPersonality;
 /*0x46*/ MainCallback returnCallback;
};

static const u8 sPCIconOff_Gfx[] = INCBIN_U8("graphics/naming_screen/pc_icon_off.4bpp");
static const u8 sPCIconOn_Gfx[] = INCBIN_U8("graphics/naming_screen/pc_icon_on.4bpp");

static struct NamingScreenData *const sNamingScreen = &namingScreenData;

static const struct SubspriteTable sSubspriteTable_PageSwapFrame[];
static const struct SubspriteTable sSubspriteTable_PageSwapText[];
static const struct SubspriteTable sSubspriteTable_Button[];
static const struct SubspriteTable sSubspriteTable_PCIcon[];
static const struct SpriteTemplate sSpriteTemplate_PageSwapFrame;
static const struct SpriteTemplate sSpriteTemplate_PageSwapButton;
static const struct SpriteTemplate sSpriteTemplate_PageSwapText;
static const struct SpriteTemplate sSpriteTemplate_BackButton;
static const struct SpriteTemplate sSpriteTemplate_OkButton;
static const struct SpriteTemplate sSpriteTemplate_Cursor;
static const struct SpriteTemplate sSpriteTemplate_InputArrow;
static const struct SpriteTemplate sSpriteTemplate_Underscore;
static const struct SpriteTemplate sSpriteTemplate_PCIcon;
static const struct SpriteSheet sSpriteSheets[];
static const struct SpritePalette sSpritePalettes[];
static const u16 sKeyboardLower_Tilemap[];
static const u16 sKeyboardUpper_Tilemap[];
static const u16 sKeyboardSymbols_Tilemap[];

static void CB2_LoadNamingScreen(void);
static void CB2_NamingScreen(void);
static void StoreNamingScreenParameters(u8, u8 *, u16, u16, u32, MainCallback);
static void NamingScreen_TurnOffScreen(void);
static void NamingScreen_Init(void);
static void NamingScreen_ClearVram(void);
static void NamingScreen_ClearOam(void);
static void NamingScreen_SetUpVideoRegs(void);
static void NamingScreen_SetUpWindow(void);
static void NamingScreen_ResetObjects(void);
static void LoadGfx(void);
static void CreateSprites(void);
static void CreateNamingScreenTasks(void);
static void NamingScreen_InitDisplayMode(void);
static void Task_DoNothing(u8);
static void LoadNamingScreenGfx(void);
static void LoadSpriteSheetsAndPalettes(void);
static void LoadNamingScreenBackground(void);
static void LoadKeyboardPages(void);
static void InitializeKeyboardWindows(void);
static void InitializeTextEntryWindow(void);
static void DrawTextEntry(void);
static void CreateCursorSprite(void);
static void CreatePageSwapButtonSprites(void);
static void CreateBackOkSprites(void);
static void CreateTextEntrySprites(void);
static void CreateInputTargetIcon(void);
static void CreateInputHandlerTask(void);
static void CreateButtonFlashTask(void);
static void CreateNamingScreenTask(void);
static void Task_NamingScreen(u8);
static void SetInputState(u8);
static void SetCursorFlashing(bool8);
static bool8 HandleKeyboardEvent(void);
static bool8 IsCursorAnimFinished(void);
static void MoveCursorToOKButton(void);
static void StartPageSwapButtonAnim(void);
static void StartPageSwapAnim(void);
static void SetCursorInvisibility(bool8);
static void TryStartButtonFlash(u8, bool8, bool8);
static bool8 IsPageSwapAnimNotInProgress(void);
static void LoadKeyboardPageOnDeck(void);
static void GetCursorPos(s16 *, s16 *);
static void SetCursorPos(s16, s16);
static void InitializeKeyboardWindowOnDeck(void);
static void SaveInputText(void);
static void DisplaySentToPCMessage(void);
static u8 GetKeyRoleAtCursorPos(void);
static u8 SwapKeyboardPage(void);
static void DeleteTextCharacter(void);
static void CycleLastTextCharacter(void);
static u8 GetInputEvent(void);
static bool8 AddTextCharacter(void);
static void SquishCursor(void);
static void Task_HandlePageSwapAnim(u8);
static void SetPageSwapButtonGfx(u8, struct Sprite *, struct Sprite *);
static u8 GetTextEntryPosition(void);
static u8 GetCharAtKeyboardPos(s16, s16);
static bool8 TryApplyPrimaryCharacterModifier(void);
static bool8 TryApplySecondaryCharacterModifier(void);
static void BufferCharacter(u8);
static bool8 HasAlternateCharacterForm(u8);
static bool8 HasSecondaryCharacterModifier(u8);
static void ApplySecondaryCharacterModifier(u8, u8);
static void ApplyAlternateCharacterForm(u8, u8);
static bool8 HasSecondAlternateCharacterForm(u8);
static void ApplySecondAlternateCharacterForm(u8, u8);
static void ApplyPrimaryCharacterModifier(u8, u8);
static bool8 HasPrimaryCharacterModifier(u8);
static void LoadNamingScreenTiles(void);
static void LoadNamingScreenPalette(void);
static void CopyKeyboardTilemap(u16 *, const u16 *);
static void CopyBackgroundTilemap(u16 *, const u16 *);
static void NamingScreen_Dummy(u8, u8);
static void PrintKeyboardCharacters(u8);

void DoNamingScreen(u8 templateNum, u8 *destBuffer, u16 speciesOrPlayerGender, u16 monGender, u32 monPersonality, MainCallback returnCallback)
{
    StoreNamingScreenParameters(templateNum, destBuffer, speciesOrPlayerGender, monGender, monPersonality, returnCallback);
    SetMainCallback2(CB2_LoadNamingScreen);
}

static void CB2_LoadNamingScreen(void)
{
    switch (gMain.state)
    {
    case 0:
        NamingScreen_TurnOffScreen();
        NamingScreen_Init();
        gMain.state++;
        break;
    case 1:
        NamingScreen_ClearVram();
        gMain.state++;
        break;
    case 2:
        NamingScreen_ClearOam();
        gMain.state++;
        break;
    case 3:
        NamingScreen_SetUpVideoRegs();
        gMain.state++;
        break;
    case 4:
        NamingScreen_SetUpWindow();
        gMain.state++;
        break;
    case 5:
        NamingScreen_ResetObjects();
        gMain.state++;
        break;
    case 6:
        LoadGfx();
        gMain.state++;
        break;
    case 7:
        CreateSprites();
        CreateNamingScreenTasks();
        NamingScreen_InitDisplayMode();
        SetMainCallback2(CB2_NamingScreen);
        break;
    }
}

static void CB2_NamingScreen(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void VBlankCB_NamingScreen(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    REG_BG1VOFS = sNamingScreen->bg1vOffset;
    REG_BG2VOFS = sNamingScreen->bg2vOffset;
    REG_BG1CNT &= 0xFFFC;
    REG_BG1CNT |= sNamingScreen->bg1Priority;
    REG_BG2CNT &= 0xFFFC;
    REG_BG2CNT |= sNamingScreen->bg2Priority;
}

static void StoreNamingScreenParameters(u8 templateNum, u8 *destBuffer, u16 speciesOrPlayerGender, u16 monGender, u32 monPersonality, MainCallback returnCallback)
{
    struct Task *task;

    //Create a task that does nothing, and use it as a temporary space to store parameters
    task = &gTasks[CreateTask(Task_DoNothing, 0xFF)];
    task->data[0] = templateNum;
    task->data[1] = speciesOrPlayerGender;
    task->data[2] = monGender;
    task->data[3] = monPersonality >> 16;
    task->data[4] = monPersonality;
    StoreWordInTwoHalfwords(&task->data[5], (u32)destBuffer);
    StoreWordInTwoHalfwords(&task->data[7], (u32)returnCallback);
}

static void GetNamingScreenParameters(void)
{
    u8 taskId;
    struct Task *task;

    taskId = FindTaskIdByFunc(Task_DoNothing);
    task = &gTasks[taskId];
    sNamingScreen->templateNum = task->data[0];
    sNamingScreen->speciesOrPlayerGender = task->data[1];
    sNamingScreen->monGender = task->data[2];
    sNamingScreen->monPersonality = (task->data[3] << 16) | (u16)task->data[4];
    LoadWordFromTwoHalfwords(&task->data[5], (u32 *)&sNamingScreen->destBuffer);
    LoadWordFromTwoHalfwords(&task->data[7], (u32 *)&sNamingScreen->returnCallback);
    DestroyTask(taskId);
}

static void Task_DoNothing(u8 taskId)
{
}

static void NamingScreen_TurnOffScreen(void)
{
    SetVBlankCallback(NULL);
    SetHBlankCallback(NULL);
    REG_DISPCNT = 0;
}

static void NamingScreen_InitDisplayMode(void)
{
    u16 savedIme;

    SetVBlankCallback(VBlankCB_NamingScreen);
    savedIme = REG_IME;
    REG_IME = 0;
    REG_IE |= INTR_FLAG_VBLANK;
    REG_IME = savedIme;
    REG_DISPSTAT |= DISPSTAT_VBLANK_INTR;
    REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG_ALL_ON | DISPCNT_OBJ_ON;
}

static void NamingScreen_ClearVram(void)
{
    DmaFill16Large(3, 0, (void *)VRAM, 0x10000, 0x1000);
}

static void NamingScreen_ClearOam(void)
{
    DmaClear16(3, (void *)OAM, 0x400);
}

static void NamingScreen_SetUpVideoRegs(void)
{
    REG_BG0CNT = 0;
    REG_BG1CNT = 0;
    REG_BG2CNT = 0;
    REG_BG3CNT = 0;
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;
    REG_BG1HOFS = 0;
    REG_BG1VOFS = 0;
    REG_BG2HOFS = 0;
    REG_BG2VOFS = 0;
    REG_BG3HOFS = 0;
    REG_BG3VOFS = 0;
    REG_BG0CNT = BGCNT_PRIORITY(0) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(31) | BGCNT_16COLOR | BGCNT_TXT256x256;
    REG_BG1CNT = BGCNT_PRIORITY(1) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(28) | BGCNT_16COLOR | BGCNT_TXT256x256;
    REG_BG2CNT = BGCNT_PRIORITY(2) | BGCNT_CHARBASE(2) | BGCNT_SCREENBASE(29) | BGCNT_16COLOR | BGCNT_TXT256x256;
    REG_BG3CNT = BGCNT_PRIORITY(3) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(30) | BGCNT_16COLOR | BGCNT_TXT256x256;
    REG_BLDCNT = 0x640;
    REG_BLDALPHA = 0x80C;
}

static const struct NamingScreenTemplate *const sNamingScreenTemplates[];  //forward declaration

static void NamingScreen_Init(void)
{
    GetNamingScreenParameters();
    sNamingScreen->state = STATE_FADE_IN;
    sNamingScreen->bg1vOffset = 0;
    sNamingScreen->bg2vOffset = 0;
    sNamingScreen->bg1Priority = BGCNT_PRIORITY(1);
    sNamingScreen->bg2Priority = BGCNT_PRIORITY(2);
    sNamingScreen->bgToReveal = 0;
    sNamingScreen->bgToHide = 1;
    sNamingScreen->template = sNamingScreenTemplates[sNamingScreen->templateNum];
    sNamingScreen->currentPage = sNamingScreen->template->initialPage;
    sNamingScreen->nameLeftOffset = 14 - sNamingScreen->template->maxChars / 2;
    sNamingScreen->keyRepeatStartDelayCopy = gKeyRepeatStartDelay;
    memset(sNamingScreen->textBuffer, 0xFF, sizeof(sNamingScreen->textBuffer));
    if (sNamingScreen->template->copyExistingString)
        StringCopy(sNamingScreen->textBuffer, sNamingScreen->destBuffer);
    gKeyRepeatStartDelay = 16;
}

static void NamingScreen_SetUpWindow(void)
{
    Text_LoadWindowTemplate(&gNamingScreenWindowTemplate);
    InitMenuWindow(&gNamingScreenWindowTemplate);
}

static void NamingScreen_ResetObjects(void)
{
    ResetPaletteFade();
    ResetSpriteData();
    FreeAllSpritePalettes();
    ResetTasks();
}

static void LoadGfx(void)
{
    LoadNamingScreenGfx();
    LoadSpriteSheetsAndPalettes();
    LoadNamingScreenBackground();
    LoadKeyboardPages();
    InitializeKeyboardWindows();
    InitializeTextEntryWindow();
    DrawTextEntry();
}

static void CreateSprites(void)
{
    CreateCursorSprite();
    CreatePageSwapButtonSprites();
    CreateBackOkSprites();
    CreateTextEntrySprites();
    CreateInputTargetIcon();
}

static void CreateNamingScreenTasks(void)
{
    CreateInputHandlerTask();
    CreateButtonFlashTask();
    CreateNamingScreenTask();
}

//--------------------------------------------------
// Naming screen main
//--------------------------------------------------

static bool8 MainState_BeginFadeIn(struct Task *);
static bool8 MainState_WaitFadeIn(struct Task *);
static bool8 MainState_HandleInput(struct Task *);
static bool8 MainState_MoveToOKButton(struct Task *);
static bool8 MainState_StartPageSwap(struct Task *);
static bool8 MainState_WaitPageSwap(struct Task *);
static bool8 MainState_PressedOKButton(struct Task *);
static bool8 MainState_WaitSentToPCMessage(struct Task *);
static bool8 MainState_FadeOut(struct Task *);
static bool8 MainState_Exit(struct Task *);

static bool8 (*const sMainStateFuncs[])(struct Task *) =
{
    MainState_BeginFadeIn,
    MainState_WaitFadeIn,
    MainState_HandleInput,
    MainState_MoveToOKButton,
    MainState_StartPageSwap,
    MainState_WaitPageSwap,
    MainState_PressedOKButton,
    MainState_WaitSentToPCMessage,
    MainState_FadeOut,
    MainState_Exit,
};

static void CreateNamingScreenTask(void)
{
    u8 taskId;

    taskId = CreateTask(Task_NamingScreen, 2);
    Task_NamingScreen(taskId);
}

static void Task_NamingScreen(u8 taskId)
{
    while (sMainStateFuncs[sNamingScreen->state](&gTasks[taskId]) != 0)
        ;
}

static bool8 MainState_BeginFadeIn(struct Task *task)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
    sNamingScreen->state++;
    return 0;
}

static bool8 MainState_WaitFadeIn(struct Task *task)
{
    if (!gPaletteFade.active)
    {
        SetInputState(INPUT_STATE_ENABLED);
        SetCursorFlashing(TRUE);
        sNamingScreen->state++;
    }
    return FALSE;
}

static bool8 MainState_HandleInput(struct Task *task)
{
    return HandleKeyboardEvent();
}

static bool8 MainState_MoveToOKButton(struct Task *task)
{
    if (IsCursorAnimFinished())
    {
        SetInputState(INPUT_STATE_ENABLED);
        MoveCursorToOKButton();
        sNamingScreen->state = STATE_HANDLE_INPUT;
    }
    return FALSE;
}

static bool8 MainState_StartPageSwap(struct Task *task)
{
    SetInputState(INPUT_STATE_DISABLED);
    StartPageSwapButtonAnim();
    StartPageSwapAnim();
    SetCursorInvisibility(TRUE);
    TryStartButtonFlash(BUTTON_PAGE, FALSE, TRUE);
    PlaySE(SE_WIN_OPEN);
    sNamingScreen->state = STATE_WAIT_PAGE_SWAP;
    return FALSE;
}

static bool8 MainState_WaitPageSwap(struct Task *task)
{
    s16 cursorX;
    s16 cursorY;

    if (IsPageSwapAnimNotInProgress())
    {
        sNamingScreen->state = STATE_HANDLE_INPUT;
        sNamingScreen->currentPage++;
        sNamingScreen->currentPage %= KBPAGE_COUNT;
        LoadKeyboardPageOnDeck();
        InitializeKeyboardWindowOnDeck();
        SetInputState(INPUT_STATE_ENABLED);
        GetCursorPos(&cursorX, &cursorY);
#if ENGLISH
        if (sNamingScreen->currentPage == KBPAGE_SYMBOLS && (cursorX == 6 || cursorX == 7))
            cursorX = 5;
#elif GERMAN
        if (sNamingScreen->currentPage == KBPAGE_SYMBOLS && (cursorX == 7 || cursorX == 8))
            cursorX = 6;
#endif
        SetCursorPos(cursorX, cursorY);
        SetCursorInvisibility(FALSE);
    }
    return FALSE;
}

static bool8 MainState_PressedOKButton(struct Task *task)
{
    SaveInputText();
    SetInputState(INPUT_STATE_DISABLED);
    SetCursorFlashing(FALSE);
    TryStartButtonFlash(BUTTON_COUNT, FALSE, TRUE);
    gKeyRepeatStartDelay = sNamingScreen->keyRepeatStartDelayCopy;
    if (sNamingScreen->templateNum == NAMING_SCREEN_CAUGHT_MON
     && CalculatePlayerPartyCount() >= 6)
    {
        DisplaySentToPCMessage();
        sNamingScreen->state = STATE_WAIT_SENT_TO_PC_MESSAGE;
        return FALSE;
    }
    else
    {
        sNamingScreen->state = STATE_FADE_OUT;
        return TRUE;  //Exit the naming screen
    }
}

static bool8 MainState_WaitSentToPCMessage(struct Task *task)
{
    if (Menu_UpdateWindowText())
        sNamingScreen->state++;
    return FALSE;
}

static bool8 MainState_FadeOut(struct Task *task)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    sNamingScreen->state++;
    return FALSE;
}

static bool8 MainState_Exit(struct Task *task)
{
    if (!gPaletteFade.active)
        SetMainCallback2(sNamingScreen->returnCallback);
    return FALSE;
}

//--------------------------------------------------
// Keyboard handling
//--------------------------------------------------

enum
{
    KEY_ROLE_CHAR,
    KEY_ROLE_PAGE,
    KEY_ROLE_BACKSPACE,
    KEY_ROLE_OK,
};

#define CHAR_KEYBOARD_PRIMARY_MODIFIER EOS
#define CHAR_KEYBOARD_SECONDARY_MODIFIER 0xFE


static bool8 KeyboardKeyHandler_Character(u8);
static bool8 KeyboardKeyHandler_Page(u8);
static bool8 KeyboardKeyHandler_Backspace(u8);
static bool8 KeyboardKeyHandler_OK(u8);

static bool8 (*const sKeyboardKeyHandlers[])(u8) =
{
    KeyboardKeyHandler_Character,
    KeyboardKeyHandler_Page,
    KeyboardKeyHandler_Backspace,
    KeyboardKeyHandler_OK,
};

static bool8 HandleKeyboardEvent(void)
{
    u8 event = GetInputEvent();
    u8 keyRole = GetKeyRoleAtCursorPos();

    if (event == INPUT_SELECT)
        return SwapKeyboardPage();
    else if (event == INPUT_B_BUTTON)
    {
        DeleteTextCharacter();
        return FALSE;
    }
    else if (event == INPUT_LR_BUTTON)
    {
        CycleLastTextCharacter();
        return FALSE;
    }
    return sKeyboardKeyHandlers[keyRole](event);
}

static bool8 KeyboardKeyHandler_Character(u8 event)
{
    TryStartButtonFlash(BUTTON_COUNT, FALSE, FALSE);
    if (event == INPUT_A_BUTTON)
    {
        bool8 textFull = AddTextCharacter();

        SquishCursor();
        if (textFull)
        {
            SetInputState(INPUT_STATE_DISABLED);
            sNamingScreen->state = STATE_MOVE_TO_OK_BUTTON;
        }
    }
    return FALSE;
}

static bool8 KeyboardKeyHandler_Page(u8 event)
{
    TryStartButtonFlash(BUTTON_PAGE, TRUE, FALSE);
    if (event == INPUT_A_BUTTON)
        return SwapKeyboardPage();
    else
        return FALSE;
}

static bool8 KeyboardKeyHandler_Backspace(u8 event)
{
    TryStartButtonFlash(BUTTON_BACK, TRUE, FALSE);
    if (event == INPUT_A_BUTTON)
        DeleteTextCharacter();
    return FALSE;
}

static bool8 KeyboardKeyHandler_OK(u8 event)
{
    TryStartButtonFlash(BUTTON_OK, TRUE, FALSE);
    if (event == INPUT_A_BUTTON)
    {
        PlaySE(SE_SELECT);
        sNamingScreen->state = STATE_PRESSED_OK;
        return TRUE;
    }
    else
        return FALSE;
}

static bool8 SwapKeyboardPage(void)
{
    sNamingScreen->state = STATE_START_PAGE_SWAP;
    return TRUE;
}

//--------------------------------------------------
// Input handling
//--------------------------------------------------

enum
{
    FNKEY_CASE,
    FNKEY_BACK,
    FNKEY_OK,
};

#define tState data[0]
#define tKeyboardEvent data[1]
#define tKbFunctionKey data[2]

static void Input_Disabled(struct Task *);
static void Input_Enabled(struct Task *);

static void (*const sInputStateFuncs[])(struct Task *) =
{
    Input_Disabled,
    Input_Enabled,
};

static void Task_HandleInput(u8);
static void HandleDpadMovement(struct Task *);

static void CreateInputHandlerTask(void)
{
    CreateTask(Task_HandleInput, 1);
}

static u8 GetInputEvent(void)
{
    u8 taskId = FindTaskIdByFunc(Task_HandleInput);

    return gTasks[taskId].tKeyboardEvent;
}

static void SetInputState(u8 state)
{
    u8 taskId = FindTaskIdByFunc(Task_HandleInput);

    gTasks[taskId].tState = state;
}

static void Task_HandleInput(u8 taskId)
{
    sInputStateFuncs[gTasks[taskId].tState](&gTasks[taskId]);
}

static void Input_Disabled(struct Task *task)
{
    task->tKeyboardEvent = INPUT_NONE;
}

static void Input_Enabled(struct Task *task)
{
    task->tKeyboardEvent = INPUT_NONE;
    if (JOY_NEW(A_BUTTON))
    {
        task->tKeyboardEvent = INPUT_A_BUTTON;
        return;
    }
    if (JOY_NEW(B_BUTTON))
    {
        task->tKeyboardEvent = INPUT_B_BUTTON;
        return;
    }
    if (JOY_NEW(SELECT_BUTTON))
    {
        task->tKeyboardEvent = INPUT_SELECT;
        return;
    }
    if (JOY_NEW(START_BUTTON))
    {
        task->tKeyboardEvent = INPUT_START;
        MoveCursorToOKButton();
        return;
    }
    HandleDpadMovement(task);
}

static const s16 sDpadDeltaX[] =
{
    [INPUT_NONE]       = 0,
    [INPUT_DPAD_UP]    = 0,
    [INPUT_DPAD_DOWN]  = 0,
    [INPUT_DPAD_LEFT]  = -1,
    [INPUT_DPAD_RIGHT] = 1,
};

static const s16 sDpadDeltaY[] =
{
    [INPUT_NONE]       = 0,
    [INPUT_DPAD_UP]    = -1,
    [INPUT_DPAD_DOWN]  = 1,
    [INPUT_DPAD_LEFT]  = 0,
    [INPUT_DPAD_RIGHT] = 0,
};

static const s16 sKeyRowToButtonRow[] = {0, 1, 1, 2};
static const s16 sButtonRowToKeyRow[] = {0, 0, 3, 0};

static void HandleDpadMovement(struct Task *task)
{
    s16 cursorX;
    s16 cursorY;
    u16 dpadDir;
    s16 prevCursorX;

    GetCursorPos(&cursorX, &cursorY);
    dpadDir = INPUT_NONE;
    if (JOY_REPT(DPAD_UP))
        dpadDir = INPUT_DPAD_UP;
    if (JOY_REPT(DPAD_DOWN))
        dpadDir = INPUT_DPAD_DOWN;
    if (JOY_REPT(DPAD_LEFT))
        dpadDir = INPUT_DPAD_LEFT;
    if (JOY_REPT(DPAD_RIGHT))
        dpadDir = INPUT_DPAD_RIGHT;

    //Get new cursor position
    prevCursorX = cursorX;
    cursorX += sDpadDeltaX[dpadDir];
    cursorY += sDpadDeltaY[dpadDir];

    //Wrap cursor position in the X direction
    if (cursorX < 0)
        cursorX = COLUMN_COUNT - 1;
    if (cursorX > COLUMN_COUNT - 1)
        cursorX = 0;

    //Handle cursor movement in X direction
    if (sDpadDeltaX[dpadDir] != 0)
    {
        //The "others" page only has 5 columns
#if ENGLISH
        if (sNamingScreen->currentPage == KBPAGE_SYMBOLS && (cursorX == 6 || cursorX == 7))
#elif GERMAN
        if (sNamingScreen->currentPage == KBPAGE_SYMBOLS && (cursorX == 6 || cursorX == 7 || cursorX == 8))
#endif
        {
            if (sDpadDeltaX[dpadDir] > 0)
                cursorX = COLUMN_COUNT - 1;
            else
                cursorX = 5;
        }

        if (cursorX == COLUMN_COUNT - 1)
        {
            //We are now on the last column
            task->tKbFunctionKey = cursorY;
            cursorY = sKeyRowToButtonRow[cursorY];
        }
        else if (prevCursorX == COLUMN_COUNT - 1)
        {
            if (cursorY == 1)
                cursorY = task->tKbFunctionKey;
            else
                cursorY = sButtonRowToKeyRow[cursorY];
        }
    }

    if (cursorX == COLUMN_COUNT - 1)
    {
        //There are only 3 keys on the last column, unlike the others,
        //so wrap Y accordingly
        if (cursorY < 0)
            cursorY = 2;
        if (cursorY > 2)
            cursorY = 0;
        if (cursorY == 0)
            task->tKbFunctionKey = FNKEY_BACK;
        else if (cursorY == 2)
            task->tKbFunctionKey = FNKEY_OK;
    }
    else
    {
        if (cursorY < 0)
            cursorY = 3;
        if (cursorY > 3)
            cursorY = 0;
    }
    SetCursorPos(cursorX, cursorY);
}

#undef tState
#undef tKeyboardEvent
#undef tKbFunctionKey

//--------------------------------------------------
//
//--------------------------------------------------

#define tButtonId     data[0]
#define tKeepFlashing data[1]
#define tAllowFlash   data[2]
#define tColor        data[3]
#define tColorIncr    data[4]
#define tColorDelay   data[5]

static void Task_UpdateButtonFlash(u8);
static u16 GetButtonPalOffset(u8);
static void RestoreButtonColor(u8);
static void StartButtonFlash(struct Task *, u8, bool8);

static void CreateButtonFlashTask(void)
{
    u8 taskId;

    taskId = CreateTask(Task_UpdateButtonFlash, 3);
    gTasks[taskId].tButtonId = BUTTON_COUNT;
}

static void TryStartButtonFlash(u8 button, bool8 keepFlashing, bool8 interruptCurFlash)
{
    struct Task *task = &gTasks[FindTaskIdByFunc(Task_UpdateButtonFlash)];

    if (button == task->tButtonId && !interruptCurFlash)
    {
        task->tKeepFlashing = keepFlashing;
        task->tAllowFlash = TRUE;
        return;
    }
    if (button == BUTTON_COUNT && !task->tKeepFlashing && !interruptCurFlash)
        return;
    if (task->tButtonId != BUTTON_COUNT)
        RestoreButtonColor(task->tButtonId);
    StartButtonFlash(task, button, keepFlashing);
}

static void Task_UpdateButtonFlash(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    if (task->tButtonId == BUTTON_COUNT || !task->tAllowFlash)
        return;
    MultiplyInvertedPaletteRGBComponents(GetButtonPalOffset(task->tButtonId), task->tColor, task->tColor, task->tColor);
    if (task->tColorDelay != 0)
    {
        task->tColorDelay--;
        if (task->tColorDelay != 0)
            return;
    }
    task->tColorDelay = 2;
    task->tColor += task->tColorIncr;
    if (task->tColor == 16)
        task->tColorIncr = -task->tColorIncr;
    else if (task->tColor == 0)
    {
        task->tAllowFlash = task->tKeepFlashing;
        task->tColorIncr = -task->tColorIncr;
    }
}

static u16 GetButtonPalOffset(u8 button)
{
    const u16 palOffsets[] =
    {
        [BUTTON_PAGE] = OBJ_PLTT_ID(IndexOfSpritePaletteTag(PALTAG_PAGE_SWAP)) + 14,
        [BUTTON_BACK] = OBJ_PLTT_ID(IndexOfSpritePaletteTag(PALTAG_BUTTON)) + 12,
        [BUTTON_OK] = OBJ_PLTT_ID(IndexOfSpritePaletteTag(PALTAG_BUTTON)) + 14,
    };

    return palOffsets[button];
}

static void RestoreButtonColor(u8 button)
{
    u16 index = GetButtonPalOffset(button);

    gPlttBufferFaded[index] = gPlttBufferUnfaded[index];
}

static void StartButtonFlash(struct Task *task, u8 button, bool8 keepFlashing)
{
    task->tButtonId = button;
    task->tKeepFlashing = keepFlashing;
    task->tAllowFlash = TRUE;
    task->tColor = 15;
    task->tColorIncr = 1;
    task->tColorDelay = 0;
}

#undef tButtonId
#undef tKeepFlashing
#undef tAllowFlash
#undef tColor
#undef tColorIncr
#undef tColorDelay

//--------------------------------------------------
// Page Swap
//--------------------------------------------------

#define tState data[0]
#define tFrameCount data[1]

static bool8 PageSwapAnimState_Init(struct Task *);
static bool8 PageSwapAnimState_1(struct Task *);
static bool8 PageSwapAnimState_2(struct Task *);
static bool8 PageSwapAnimState_Done(struct Task *);

static bool8 (*const sPageSwapAnimStateFuncs[])(struct Task *) =
{
    PageSwapAnimState_Init,
    PageSwapAnimState_1,
    PageSwapAnimState_2,
    PageSwapAnimState_Done,
};

static void StartPageSwapAnim(void)
{
    u8 taskId;

    taskId = CreateTask(Task_HandlePageSwapAnim, 0);
    Task_HandlePageSwapAnim(taskId);
}

static bool8 IsPageSwapAnimNotInProgress(void)
{
    if (FindTaskIdByFunc(Task_HandlePageSwapAnim) == 0xFF)
        return TRUE;
    else
        return FALSE;
}

static void Task_HandlePageSwapAnim(u8 taskId)
{
    while (sPageSwapAnimStateFuncs[gTasks[taskId].tState](&gTasks[taskId]) != 0)
        ;
}

static bool8 PageSwapAnimState_Init(struct Task *task)
{
    sNamingScreen->bg1vOffset = 0;
    sNamingScreen->bg2vOffset = 0;
    task->tState++;
    return 0;
}

static bool8 PageSwapAnimState_1(struct Task *task)
{
    u16 *const vOffsets[] =
    {
        &sNamingScreen->bg2vOffset,
        &sNamingScreen->bg1vOffset
    };

    task->tFrameCount += 4;
    *vOffsets[sNamingScreen->bgToReveal] = Sin(task->tFrameCount, 40);
    *vOffsets[sNamingScreen->bgToHide] = Sin((task->tFrameCount + 128) & 0xFF, 40);
    if (task->tFrameCount >= 64)
    {
        u8 temp = sNamingScreen->bg1Priority;
        sNamingScreen->bg1Priority = sNamingScreen->bg2Priority;
        sNamingScreen->bg2Priority = temp;
        task->tState++;
    }
    return 0;
}

static bool8 PageSwapAnimState_2(struct Task *task)
{
    u16 *const vOffsets[] = {&sNamingScreen->bg2vOffset, &sNamingScreen->bg1vOffset};

    task->tFrameCount += 4;
    *vOffsets[sNamingScreen->bgToReveal] = Sin(task->tFrameCount, 40);
    *vOffsets[sNamingScreen->bgToHide] = Sin((task->tFrameCount + 128) & 0xFF, 40);
    if (task->tFrameCount >= 128)
    {
        u8 temp = sNamingScreen->bgToReveal;

        sNamingScreen->bgToReveal = sNamingScreen->bgToHide;
        sNamingScreen->bgToHide = temp;
        task->tState++;
    }
    return 0;
}

static bool8 PageSwapAnimState_Done(struct Task *task)
{
    DestroyTask(FindTaskIdByFunc(Task_HandlePageSwapAnim));
    return 0;
}

#undef tState
#undef tFrameCount

//--------------------------------------------------
// Cursor
//--------------------------------------------------

#define sX          data[0]
#define sY          data[1]
#define sPrevX      data[2]
#define sPrevY      data[3]
#define sInvisible  data[4] & 0x00FF
#define sFlashing   data[4] & 0xFF00
#define sColor      data[5]
#define sColorIncr  data[6]
#define sColorDelay data[7]

static void CreateCursorSprite(void)
{
    sNamingScreen->cursorSpriteId = CreateSprite(&sSpriteTemplate_Cursor, 0, 0, 0);
    gSprites[sNamingScreen->cursorSpriteId].oam.priority = 1;
    gSprites[sNamingScreen->cursorSpriteId].oam.objMode = 1;
    gSprites[sNamingScreen->cursorSpriteId].data[6] = 1;
    gSprites[sNamingScreen->cursorSpriteId].data[6] = 2;
    SetCursorPos(0, 0);
}

static const u8 sKeyboardColumnTextOffsets[][COLUMN_COUNT] = {
#if ENGLISH
    {1,  3,  5,  8, 10, 12, 14, 17, 19},  //Upper page
    {1,  3,  5,  8, 10, 12, 14, 17, 19},  //Lower page
    {1,  4,  7, 10, 13, 16, 16, 16, 19},  //Others page
#elif GERMAN
    {2, 3, 4, 5,  9,  10, 11, 12, 16, 19},  //Upper page
    {2, 3, 4, 5,  9,  10, 11, 12, 16, 19},  //Lower page
    {1, 4, 7, 10, 13, 16, 16, 16, 16, 19},  //Others page
#endif
};

static u8 CursorColToKeyboardCol(s16 x)
{
    return sKeyboardColumnTextOffsets[sNamingScreen->currentPage][x];
}

static void SetCursorPos(s16 x, s16 y)
{
    struct Sprite *cursorSprite = &gSprites[sNamingScreen->cursorSpriteId];

    cursorSprite->x = CursorColToKeyboardCol(x) * 8 + 27;
    cursorSprite->y = y * 16 + 80;
    cursorSprite->data[2] = cursorSprite->data[0];
    cursorSprite->data[3] = cursorSprite->data[1];
    cursorSprite->data[0] = x;
    cursorSprite->data[1] = y;
}

static void GetCursorPos(s16 *x, s16 *y)
{
    struct Sprite *cursorSprite = &gSprites[sNamingScreen->cursorSpriteId];

    *x = cursorSprite->data[0];
    *y = cursorSprite->data[1];
}

static void MoveCursorToOKButton(void)
{
    SetCursorPos(COLUMN_COUNT - 1, 2);
}

static void SetCursorInvisibility(bool8 invisible)
{
    gSprites[sNamingScreen->cursorSpriteId].data[4] &= -256;
    gSprites[sNamingScreen->cursorSpriteId].data[4] |= invisible;
    StartSpriteAnim(&gSprites[sNamingScreen->cursorSpriteId], 0);
}

static void SetCursorFlashing(bool8 flashing)
{
    gSprites[sNamingScreen->cursorSpriteId].data[4] &= 0xFF;
    gSprites[sNamingScreen->cursorSpriteId].data[4] |= flashing << 8;
}

static void SquishCursor(void)
{
    StartSpriteAnim(&gSprites[sNamingScreen->cursorSpriteId], 1);
}

static bool8 IsCursorAnimFinished(void)
{
    return gSprites[sNamingScreen->cursorSpriteId].animEnded;
}

static u8 GetKeyRoleAtCursorPos(void)
{
    const u8 keyRoles[] = {KEY_ROLE_PAGE, KEY_ROLE_BACKSPACE, KEY_ROLE_OK};
    s16 cursorX;
    s16 cursorY;

    GetCursorPos(&cursorX, &cursorY);
    if (cursorX < COLUMN_COUNT - 1)
        return KEY_ROLE_CHAR;
    else
        return keyRoles[cursorY];
}

static void SpriteCB_Cursor(struct Sprite *sprite)
{
    if (sprite->animEnded)
        StartSpriteAnim(sprite, 0);
    sprite->invisible = sprite->sInvisible;
    if (sprite->sX == COLUMN_COUNT - 1)
        sprite->invisible = TRUE;
    if (sprite->invisible || !(sprite->sFlashing)
     || sprite->sX != sprite->sPrevX || sprite->sY != sprite->sPrevY)
    {
        sprite->sColor = 0;
        sprite->sColorIncr = 1;
        sprite->sColorDelay = 2;
    }
    sprite->sColorDelay--;
    if (sprite->sColorDelay == 0)
    {
        sprite->sColor += sprite->sColorIncr;
        if (sprite->sColor == 16 || sprite->sColor == 0)
            sprite->sColorIncr = -sprite->sColorIncr;
        sprite->sColorDelay = 2;
    }
    if (sprite->sFlashing)
    {
        s8 greenBlueIntensity = sprite->sColor;
        s8 redIntensity = sprite->sColor >> 1;
        u16 index = OBJ_PLTT_ID(IndexOfSpritePaletteTag(PALTAG_CURSOR)) + 1;

        MultiplyInvertedPaletteRGBComponents(index, redIntensity, greenBlueIntensity, greenBlueIntensity);
    }
}

#undef sX
#undef sY
#undef sPrevX
#undef sPrevY
#undef sInvisible
#undef sFlashing
#undef sColor
#undef sColorIncr
#undef sColorDelay

static void CreatePageSwapButtonSprites(void)
{
    u8 frameSpriteId;
    u8 textSpriteId;
    u8 buttonSpriteId;

    frameSpriteId = CreateSprite(&sSpriteTemplate_PageSwapFrame, 0xCC, 0x50, 0);
    sNamingScreen->pageIndicatorSpriteId = frameSpriteId;
    SetSubspriteTables(&gSprites[frameSpriteId], sSubspriteTable_PageSwapFrame);

    textSpriteId = CreateSprite(&sSpriteTemplate_PageSwapText, 0xCC, 0x4C, 1);
    gSprites[frameSpriteId].data[6] = textSpriteId;
    SetSubspriteTables(&gSprites[textSpriteId], sSubspriteTable_PageSwapText);

    buttonSpriteId = CreateSprite(&sSpriteTemplate_PageSwapButton, 0xCC, 0x4B, 2);
    gSprites[buttonSpriteId].oam.priority = 1;
    gSprites[frameSpriteId].data[7] = buttonSpriteId;
}

enum
{
    PAGE_SWAP_SPRITE_INIT,
    PAGE_SWAP_SPRITE_IDLE,
    PAGE_SWAP_SPRITE_SLIDE_OFF,
    PAGE_SWAP_SPRITE_SLIDE_ON,
};

static void StartPageSwapButtonAnim(void)
{
    struct Sprite *sprite = &gSprites[sNamingScreen->pageIndicatorSpriteId];

    sprite->data[0] = PAGE_SWAP_SPRITE_SLIDE_OFF;
    sprite->data[1] = sNamingScreen->currentPage;
}

#define sState          data[0]
#define sCurrentPage    data[1]
#define sTextSpriteId   data[6]
#define sButtonSpriteId data[7]

static u8 PageSwapSprite_Init(struct Sprite *);
static u8 PageSwapSprite_Idle(struct Sprite *);
static u8 PageSwapSprite_SlideOff(struct Sprite *);
static u8 PageSwapSprite_SlideOn(struct Sprite *);

static u8 (*const sPageSwapSpriteFuncs[])(struct Sprite *) =
{
    PageSwapSprite_Init,
    PageSwapSprite_Idle,
    PageSwapSprite_SlideOff,
    PageSwapSprite_SlideOn,
};

static void SpriteCB_PageSwap(struct Sprite *sprite)
{
    while (sPageSwapSpriteFuncs[sprite->sState](sprite) != 0)
        ;
}

static u8 PageSwapSprite_Init(struct Sprite *sprite)
{
    struct Sprite *textSprite = &gSprites[sprite->sTextSpriteId];
    struct Sprite *buttonSprite = &gSprites[sprite->sButtonSpriteId];

    SetPageSwapButtonGfx(sNamingScreen->currentPage, textSprite, buttonSprite);
    sprite->sState++;
    return FALSE;
}

static u8 PageSwapSprite_Idle(struct Sprite *sprite)
{
    return FALSE;
}

static u8 PageSwapSprite_SlideOff(struct Sprite *sprite)
{
    struct Sprite *textSprite = &gSprites[sprite->sTextSpriteId];
    struct Sprite *buttonSprite = &gSprites[sprite->sButtonSpriteId];

    textSprite->y2++;
    if (textSprite->y2 > 7)
    {
        sprite->sState++;
        textSprite->y2 = -4;
        textSprite->invisible = TRUE;
        SetPageSwapButtonGfx(((u8)sprite->sCurrentPage + 1) % KBPAGE_COUNT, textSprite, buttonSprite);
    }
    return FALSE;
}

static u8 PageSwapSprite_SlideOn(struct Sprite *sprite)
{
    struct Sprite *textSprite = &gSprites[sprite->sTextSpriteId];

    textSprite->invisible = FALSE;
    textSprite->y2++;
    if (textSprite->y2 >= 0)
    {
        textSprite->y2 = 0;
        sprite->sState = PAGE_SWAP_SPRITE_IDLE;
    }
    return FALSE;
}

#undef sState
#undef sCurrentPage
#undef sTextSpriteId
#undef sButtonSpriteId

static const u16 sPageSwapPalTags[] =
{
    PALTAG_PAGE_SWAP_UPPER,
    PALTAG_PAGE_SWAP_OTHERS,
    PALTAG_PAGE_SWAP_LOWER,
};

static const u16 sPageSwapGfxTags[] =
{
    GFXTAG_PAGE_SWAP_LOWER,
    GFXTAG_PAGE_SWAP_OTHERS,
    GFXTAG_PAGE_SWAP_UPPER,
};

static void SetPageSwapButtonGfx(u8 page, struct Sprite *textSprite, struct Sprite *buttonSprite)
{
    buttonSprite->oam.paletteNum = IndexOfSpritePaletteTag(sPageSwapPalTags[page]);
    textSprite->sheetTileStart = GetSpriteTileStartByTag(sPageSwapGfxTags[page]);
    textSprite->subspriteTableNum = page;
}

//

static void CreateBackOkSprites(void)
{
    u8 spriteId;

    spriteId = CreateSprite(&sSpriteTemplate_BackButton, 0xCC, 0x6C, 0);
    SetSubspriteTables(&gSprites[spriteId], sSubspriteTable_Button);

    spriteId = CreateSprite(&sSpriteTemplate_OkButton, 0xCC, 0x84, 0);
    SetSubspriteTables(&gSprites[spriteId], sSubspriteTable_Button);
}

static void CreateTextEntrySprites(void)
{
    u8 spriteId;
    s16 left;
    u8 i;

    left = (sNamingScreen->nameLeftOffset - 1) * 8 + 4;
    spriteId = CreateSprite(&sSpriteTemplate_InputArrow, left, 0x28, 0);
    gSprites[spriteId].oam.priority = 3;
    left = sNamingScreen->nameLeftOffset * 8 + 4;
    for (i = 0; i < sNamingScreen->template->maxChars; i++, left += 8)
    {
        spriteId = CreateSprite(&sSpriteTemplate_Underscore, left, 0x2C, 0);
        gSprites[spriteId].oam.priority = 3;
        gSprites[spriteId].data[0] = i;
    }
}

static void SpriteCB_InputArrow(struct Sprite *sprite)
{
    const s16 arr[] = {0, -4, -2, -1};

    if (sprite->data[0] == 0 || --sprite->data[0] == 0)
    {
        sprite->data[0] = 8;
        sprite->data[1] = (sprite->data[1] + 1) & 3;
    }
    sprite->x2 = arr[sprite->data[1]];
}

static void SpriteCB_Underscore(struct Sprite *sprite)
{
    const s16 arr[] = {2, 3, 2, 1};
    u8 caretPosition;

    caretPosition = GetTextEntryPosition();
    if (caretPosition != (u8)sprite->data[0])
    {
        sprite->y2 = 0;
        sprite->data[1] = 0;
        sprite->data[2] = 0;
    }
    else
    {
        sprite->y2 = arr[sprite->data[1]];
        sprite->data[2]++;
        if (sprite->data[2] > 8)
        {
            sprite->data[1] = (sprite->data[1] + 1) & 3;
            sprite->data[2] = 0;
        }
    }
}

//

static void NamingScreen_NoIcon(void);
static void NamingScreen_CreatePlayerIcon(void);
static void NamingScreen_CreatePCIcon(void);
static void NamingScreen_CreateMonIcon(void);

static void (*const sIconFunctions[])(void) =
{
    NamingScreen_NoIcon,
    NamingScreen_CreatePlayerIcon,
    NamingScreen_CreatePCIcon,
    NamingScreen_CreateMonIcon,
};

static void CreateInputTargetIcon(void)
{
    sIconFunctions[sNamingScreen->template->iconFunction]();
}

static void NamingScreen_NoIcon(void)
{
}

static void NamingScreen_CreatePlayerIcon(void)
{
    u8 rivalGfxId;
    u8 spriteId;

    rivalGfxId = GetRivalAvatarGraphicsIdByStateIdAndGender(PLAYER_AVATAR_STATE_NORMAL, sNamingScreen->speciesOrPlayerGender);
    spriteId = CreateObjectGraphicsSprite(rivalGfxId, SpriteCallbackDummy, 0x38, 0x18, 0);
    gSprites[spriteId].oam.priority = 3;
    StartSpriteAnim(&gSprites[spriteId], 4);
}

static void NamingScreen_CreatePCIcon(void)
{
    u8 spriteId;

    spriteId = CreateSprite(&sSpriteTemplate_PCIcon, 0x34, 0x18, 0);
    SetSubspriteTables(&gSprites[spriteId], sSubspriteTable_PCIcon);
    gSprites[spriteId].oam.priority = 3;
}

static void NamingScreen_CreateMonIcon(void)
{
    u8 spriteId;

    LoadMonIconPalettes();
    spriteId = CreateMonIcon(sNamingScreen->speciesOrPlayerGender, SpriteCallbackDummy, 0x34, 0x18, 0, sNamingScreen->monPersonality);
    gSprites[spriteId].oam.priority = 3;
}

static u8 GetTextEntryPosition(void)
{
    u8 i;

    for (i = 0; i < sNamingScreen->template->maxChars; i++)
    {
        if (sNamingScreen->textBuffer[i] == EOS)
            return i;
    }
    return sNamingScreen->template->maxChars - 1;
}

static u8 GetPreviousTextCaretPosition(void)
{
    s8 i;

    for (i = sNamingScreen->template->maxChars - 1; i > 0; i--)
    {
        if (sNamingScreen->textBuffer[i] != EOS)
            return i;
    }
    return 0;
}

static void DeleteTextCharacter(void)
{
    u8 index;
    u8 keyRole;

    index = GetPreviousTextCaretPosition();
    sNamingScreen->textBuffer[index] = CHAR_SPACE;
    DrawTextEntry();
    sNamingScreen->textBuffer[index] = EOS;
    keyRole = GetKeyRoleAtCursorPos();
    if (keyRole == KEY_ROLE_CHAR || keyRole == KEY_ROLE_BACKSPACE)
        TryStartButtonFlash(BUTTON_BACK, FALSE, TRUE);
    PlaySE(SE_BALL);
}

static bool8 AddTextCharacter(void)
{
    s16 x;
    s16 y;
    u8 ch;
    bool8 textChanged;

    GetCursorPos(&x, &y);
    x = CursorColToKeyboardCol(x);
    ch = GetCharAtKeyboardPos(x, y);
    textChanged = TRUE;
    if (ch == CHAR_KEYBOARD_PRIMARY_MODIFIER)
        textChanged = TryApplyPrimaryCharacterModifier();
    else if (ch == CHAR_KEYBOARD_SECONDARY_MODIFIER)
        textChanged = TryApplySecondaryCharacterModifier();
    else
        BufferCharacter(ch);
    DrawTextEntry();
    PlaySE(SE_SELECT);
    if (textChanged)
    {
        if (GetPreviousTextCaretPosition() == sNamingScreen->template->maxChars - 1)
            return TRUE;
    }
    return FALSE;
}

static void CycleLastTextCharacter(void)
{
    u8 textIndex;
    u8 character;

    textIndex = GetPreviousTextCaretPosition();
    character = sNamingScreen->textBuffer[textIndex];
    if (HasAlternateCharacterForm(character))
    {
        if (HasSecondaryCharacterModifier(character))
            ApplySecondaryCharacterModifier(character, textIndex);
        else
            ApplyAlternateCharacterForm(character, textIndex);
    }
    else
    {
        if (HasSecondAlternateCharacterForm(character))
            ApplySecondAlternateCharacterForm(character, textIndex);
        else
            ApplyPrimaryCharacterModifier(character, textIndex);
    }
    DrawTextEntry();
    PlaySE(SE_SELECT);
}

static bool8 TryApplyPrimaryCharacterModifier(void)
{
    u8 textIndex;
    u8 character;

    textIndex = GetPreviousTextCaretPosition();
    character = sNamingScreen->textBuffer[textIndex];
    if (HasPrimaryCharacterModifier(character))
    {
        ApplyPrimaryCharacterModifier(character, textIndex);
        return TRUE;
    }
    return FALSE;
}

static bool8 TryApplySecondaryCharacterModifier(void)
{
    u8 textIndex;
    u8 character;

    textIndex = GetPreviousTextCaretPosition();
    character = sNamingScreen->textBuffer[textIndex];
    if (HasSecondaryCharacterModifier(character))
    {
        ApplySecondaryCharacterModifier(character, textIndex);
        return TRUE;
    }
    return FALSE;
}

static void BufferCharacter(u8 ch)
{
    u8 index = GetTextEntryPosition();

    sNamingScreen->textBuffer[index] = ch;
}

static bool8 HasAlternateCharacterForm(u8 character)
{
    if ((character >= 55 && character <= 74)
     || (character >= 135 && character <= 139)
     || (character >= 140 && character <= 144)
     || (character >= 145 && character <= 149)
     || (character >= 150 && character <= 154))
        return TRUE;
    else
        return FALSE;
}

static bool8 HasSecondAlternateCharacterForm(u8 character)
{
    if ((character >= 75 && character <= 79)
     || (character >= 155 && character <= 159))
        return TRUE;
    else
        return FALSE;
}

static bool8 HasPrimaryCharacterModifier(u8 character)
{
    if ((character >= 6 && character <= 20)
     || (character >= 26 && character <= 30)
     || (character >= 75 && character <= 79)
     || (character >= 86 && character <= 100)
     || (character >= 106 && character <= 110)
     || (character >= 155 && character <= 159))
        return TRUE;
    else
        return FALSE;
}

static bool8 HasSecondaryCharacterModifier(u8 character)
{
    if ((character >= 26 && character <= 30)
     || (character >= 70 && character <= 74)
     || (character >= 106 && character <= 110)
     || (character >= 150 && character <= 154))
        return TRUE;
    else
        return FALSE;
}

static void ApplyPrimaryCharacterModifier(u8 character, u8 textIndex)
{
    u8 modifiedCharacter = character;

    if (character >= 6 && character <= 10)
        modifiedCharacter = character + 0x31;
    else if (character >= 11 && character <= 15)
        modifiedCharacter = character + 0x31;
    else if (character >= 16 && character <= 20)
        modifiedCharacter = character + 0x31;
    else if (character >= 26 && character <= 30)
        modifiedCharacter = character + 0x2C;
    else if (character >= 75 && character <= 79)
        modifiedCharacter = character + 0xFB;
    else if (character >= 86 && character <= 90)
        modifiedCharacter = character + 0x31;
    else if (character >= 91 && character <= 95)
        modifiedCharacter = character + 0x31;
    else if (character >= 96 && character <= 100)
        modifiedCharacter = character + 0x31;
    else if (character >= 106 && character <= 110)
        modifiedCharacter = character + 0x2C;
    else if (character >= 155 && character <= 159)
        modifiedCharacter = character + 0xFB;
    sNamingScreen->textBuffer[textIndex] = modifiedCharacter;
}

static void ApplySecondaryCharacterModifier(u8 character, u8 textIndex)
{
    u8 modifiedCharacter = character;

    if (character >= 26 && character <= 30)
        modifiedCharacter = character + 0x31;
    else if (character >= 70 && character <= 74)
        modifiedCharacter = character + 5;
    else if (character >= 106 && character <= 110)
        modifiedCharacter = character + 0x31;
    else if (character >= 150 && character <= 154)
        modifiedCharacter = character + 5;
    sNamingScreen->textBuffer[textIndex] = modifiedCharacter;
}

static void ApplyAlternateCharacterForm(u8 character, u8 textIndex)
{
    u8 modifiedCharacter = character;

    if (character >= 55 && character <= 59)
        modifiedCharacter = character + 0xCF;
    else if (character >= 60 && character <= 64)
        modifiedCharacter = character + 0xCF;
    else if (character >= 65 && character <= 69)
        modifiedCharacter = character + 0xCF;
    else if (character >= 70 && character <= 74)
        modifiedCharacter = character + 0xD4;
    else if (character >= 135 && character <= 139)
        modifiedCharacter = character + 0xCF;
    else if (character >= 140 && character <= 144)
        modifiedCharacter = character + 0xCF;
    else if (character >= 145 && character <= 149)
        modifiedCharacter = character + 0xCF;
    else if (character >= 150 && character <= 154)
        modifiedCharacter = character + 0xD4;
    sNamingScreen->textBuffer[textIndex] = modifiedCharacter;
}

static void ApplySecondAlternateCharacterForm(u8 character, u8 textIndex)
{
    u8 modifiedCharacter = character;

    if (character >= 75 && character <= 79)
        modifiedCharacter = character + 0xCF;
    else if (character >= 155 && character <= 159)
        modifiedCharacter = character + 0xCF;
    sNamingScreen->textBuffer[textIndex] = modifiedCharacter;
}

static void SaveInputText(void)
{
    u8 i;

    for (i = 0; i < sNamingScreen->template->maxChars; i++)
    {
        if (sNamingScreen->textBuffer[i] != CHAR_SPACE && sNamingScreen->textBuffer[i] != EOS)
        {
            StringCopyN(sNamingScreen->destBuffer, sNamingScreen->textBuffer, sNamingScreen->template->maxChars + 1);
            break;
        }
    }
}

static void DisplaySentToPCMessage(void)
{
    StringCopy(gStringVar1, sNamingScreen->destBuffer);
    StringExpandPlaceholders(gStringVar4, gOtherText_SentToPC);
    BasicInitMenuWindow(&gNamingScreenWindowTemplate);
    Menu_DisplayDialogueFrame();
    MenuPrintMessageDefaultCoords(gStringVar4);
}

static void LoadSpriteSheetsAndPalettes(void)
{
    LoadSpriteSheets(sSpriteSheets);
    LoadSpritePalettes(sSpritePalettes);
}

static void LoadNamingScreenGfx(void)
{
    LoadNamingScreenTiles();
    LoadNamingScreenPalette();
}

static void LoadNamingScreenTiles(void)
{
    DmaCopy16Defvars(3, gNamingScreenMenu_Gfx, (void *)(VRAM + gMenuMessageBoxContentTileOffset * 32), 0x800);
    DmaCopy16Defvars(3, gNamingScreenMenu_Gfx, (void *)(VRAM + 0x8000 + gMenuMessageBoxContentTileOffset * 32), 0x800);
}

static void LoadNamingScreenPalette(void)
{
    LoadPalette(gNamingScreenMenu_Pal, 0, 0x80);
}

static void LoadKeyboardPageLower(u16 *);
static void LoadKeyboardPageUpper(u16 *);
static void LoadKeyboardPageSymbols(u16 *);

static void (*const sLoadKeyboardPageFuncs[][2])(u16 *) =
{
    {LoadKeyboardPageUpper, LoadKeyboardPageLower},
    {LoadKeyboardPageLower, LoadKeyboardPageSymbols},
    {LoadKeyboardPageSymbols, LoadKeyboardPageUpper},
};

static void LoadKeyboardPages(void)
{
    u16 *const arr[] =
    {
        (u16 *)(VRAM + 0xE000),
        (u16 *)(VRAM + 0xE800),
    };

    sLoadKeyboardPageFuncs[sNamingScreen->currentPage][0](arr[sNamingScreen->bgToReveal]);
    sLoadKeyboardPageFuncs[sNamingScreen->currentPage][1](arr[sNamingScreen->bgToHide]);
}

static void LoadKeyboardPageOnDeck(void)
{
    u16 *const arr[] =
    {
        (u16 *)(VRAM + 0xE000),
        (u16 *)(VRAM + 0xE800),
    };

    sLoadKeyboardPageFuncs[sNamingScreen->currentPage][1](arr[sNamingScreen->bgToHide]);
}

static void LoadKeyboardPageLower(u16 *vramBuffer)
{
    CopyKeyboardTilemap(vramBuffer, sKeyboardLower_Tilemap);
}

static void LoadKeyboardPageUpper(u16 *vramBuffer)
{
    CopyKeyboardTilemap(vramBuffer, sKeyboardUpper_Tilemap);
}

static void LoadKeyboardPageSymbols(u16 *vramBuffer)
{
    CopyKeyboardTilemap(vramBuffer, sKeyboardSymbols_Tilemap);
}

static void LoadNamingScreenBackground(void)
{
    CopyBackgroundTilemap((u16 *)(VRAM + 0xF000), (const u16 *)gNamingScreenBackground_Tilemap);
}

static void CopyKeyboardTilemap(u16 *vramBuffer, const u16 *src)
{
    s16 i;
    s16 j;

    for (i = 0; i < 20; i++)
    {
        for (j = 0; j < 30; j++, src++)
        {
            vramBuffer[i * 32 + j] = *src + gMenuMessageBoxContentTileOffset;
        }
    }
}

static void CopyBackgroundTilemap(u16 *vramBuffer, const u16 *src)
{
    s16 i;
    s16 j;

    for (i = 0; i < 20; i++)
    {
        for (j = 0; j < 30; j++, src++)
        {
            vramBuffer[i * 32 + j] = *src + gMenuMessageBoxContentTileOffset;
        }
        src += 2;
    }
}

static void DrawCurrentKeyboardPage(void)
{
    NamingScreen_Dummy(sNamingScreen->currentPage, sNamingScreen->bgToReveal);
}

static void DrawKeyboardPageOnDeck(void)
{
    NamingScreen_Dummy((sNamingScreen->currentPage + 1) % 3, sNamingScreen->bgToHide);
}

static void NamingScreen_Dummy(u8 page, u8 bg)
{
}

static void PrintKeyboardPageUpper(void);
static void PrintKeyboardPageLower(void);
static void PrintKeyboardPageSymbols(void);

static void (*const sPrintKeyboardPageFuncs[][2])(void) =
{
    PrintKeyboardPageLower,
    PrintKeyboardPageUpper,
    PrintKeyboardPageUpper,
    PrintKeyboardPageSymbols,
    PrintKeyboardPageSymbols,
    PrintKeyboardPageLower,
};

static const struct WindowTemplate *const sKeyboardWindowTemplates[][2][2] =
{
    {
        {&gNamingScreenKeyboardLowerWindowTemplateBg1, &gNamingScreenKeyboardLowerWindowTemplateBg2},
        {&gNamingScreenKeyboardUpperWindowTemplateBg1, &gNamingScreenKeyboardUpperWindowTemplateBg2},
    },
    {
        {&gNamingScreenKeyboardUpperWindowTemplateBg1, &gNamingScreenKeyboardUpperWindowTemplateBg2},
        {&gNamingScreenKeyboardSymbolsWindowTemplateBg1, &gNamingScreenKeyboardSymbolsWindowTemplateBg2},
    },
    {
        {&gNamingScreenKeyboardSymbolsWindowTemplateBg1, &gNamingScreenKeyboardSymbolsWindowTemplateBg2},
        {&gNamingScreenKeyboardLowerWindowTemplateBg1, &gNamingScreenKeyboardLowerWindowTemplateBg2},
    },
};

static void NamingScreen_NoSpeciesName(void);
static void NamingScreen_BufferSpeciesName(void);

static void (*const sSpeciesNameFuncs[])(void) =
{
    NamingScreen_NoSpeciesName,
    NamingScreen_NoSpeciesName,
    NamingScreen_BufferSpeciesName,
    NamingScreen_BufferSpeciesName,
};

static void NamingScreen_NoGenderIcon(void);
static void NamingScreen_DrawGenderIcon(void);

static void (*const sDrawGenderIconFuncs[])(void) =
{
    NamingScreen_NoGenderIcon,
    NamingScreen_DrawGenderIcon,
};

static const u8 sKeyboardChars[][4][20];  //forward declaration

static u8 GetCharAtKeyboardPos(s16 x, s16 y)
{
    return sKeyboardChars[sNamingScreen->currentPage][y][x];
}

static void InitializeKeyboardWindows(void)
{
    BasicInitMenuWindow(sKeyboardWindowTemplates[sNamingScreen->currentPage][0][sNamingScreen->bgToReveal]);
    sPrintKeyboardPageFuncs[sNamingScreen->currentPage][0]();
    BasicInitMenuWindow(sKeyboardWindowTemplates[sNamingScreen->currentPage][1][sNamingScreen->bgToHide]);
    sPrintKeyboardPageFuncs[sNamingScreen->currentPage][1]();
    DrawCurrentKeyboardPage();
    DrawKeyboardPageOnDeck();
}

static void InitializeKeyboardWindowOnDeck(void)
{
    BasicInitMenuWindow(sKeyboardWindowTemplates[sNamingScreen->currentPage][1][sNamingScreen->bgToHide]);
    sPrintKeyboardPageFuncs[sNamingScreen->currentPage][1]();
    DrawKeyboardPageOnDeck();
}

static void PrintKeyboardPageUpper(void)
{
    PrintKeyboardCharacters(1);
}

static void PrintKeyboardPageLower(void)
{
    PrintKeyboardCharacters(0);
}

static void PrintKeyboardPageSymbols(void)
{
    PrintKeyboardCharacters(2);
}

static void PrintKeyboardCharacters(u8 page)  //print letters on page
{
    s16 i;
    s16 y;

    for (i = 0, y = 9; i < 4; i++, y += 2)
        Menu_PrintText(sKeyboardChars[page][i], 3, y);
}

static void InitializeTextEntryWindow(void)
{
    BasicInitMenuWindow(&gNamingScreenTextEntryWindowTemplate);
    sSpeciesNameFuncs[sNamingScreen->templateNum]();
    sDrawGenderIconFuncs[sNamingScreen->template->addGenderIcon]();
    Menu_PrintText(sNamingScreen->template->title, 9, 2);
}

static void NamingScreen_NoSpeciesName(void)
{
}

static void NamingScreen_BufferSpeciesName(void)
{
    StringCopy(gStringVar1, gSpeciesNames[(s16)sNamingScreen->speciesOrPlayerGender]);
}

static void NamingScreen_NoGenderIcon(void)
{
}

static void NamingScreen_DrawGenderIcon(void)
{
    u8 genderSymbol[2] = _("♂");

    if ((s16)sNamingScreen->monGender != MON_GENDERLESS)
    {
        if ((s16)sNamingScreen->monGender == MON_FEMALE)
            genderSymbol[0] = 0xB6;  //female symbol
        Menu_PrintText(genderSymbol, 0x14, 4);
    }
}

static void DrawTextEntry(void)
{
    u8 *string = gStringVar1;

    string[0] = 0xFC;
    string[1] = 0x14;
    string[2] = 8;
    string[3] = 0xFC;
    string[4] = 0x11;
    string[5] = 1;
    string += 6;
    StringCopy(string, sNamingScreen->textBuffer);
    BasicInitMenuWindow(&gNamingScreenTextEntryWindowTemplate);
    Menu_PrintText(gStringVar1, sNamingScreen->nameLeftOffset, 4);
}

//--------------------------------------------------
// Forward-declared variables
//--------------------------------------------------

enum
{
    ICON_NONE,
    ICON_PLAYER,
    ICON_PC,
    ICON_MON,
};

static const struct NamingScreenTemplate sPlayerNamingScreenTemplate =
{
    .copyExistingString = FALSE,
    .maxChars = NAMING_SCREEN_PLAYER_NAME_LENGTH,
    .iconFunction = ICON_PLAYER,
    .addGenderIcon = FALSE,
    .initialPage = KBPAGE_LETTERS_UPPER,
    .title = OtherText_YourName,
};

static const struct NamingScreenTemplate sPCBoxNamingTemplate =
{
    .copyExistingString = FALSE,
    .maxChars = BOX_NAME_LENGTH,
    .iconFunction = ICON_PC,
    .addGenderIcon = FALSE,
    .initialPage = KBPAGE_LETTERS_UPPER,
    .title = OtherText_BoxName,
};

static const struct NamingScreenTemplate sMonNamingScreenTemplate =
{
    .copyExistingString = FALSE,
    .maxChars = POKEMON_NAME_LENGTH,
    .iconFunction = ICON_MON,
    .addGenderIcon = TRUE,
    .initialPage = KBPAGE_LETTERS_UPPER,
    .title = OtherText_PokeName,
};

static const struct NamingScreenTemplate *const sNamingScreenTemplates[] =
{
    [NAMING_SCREEN_PLAYER] = &sPlayerNamingScreenTemplate,
    [NAMING_SCREEN_BOX] = &sPCBoxNamingTemplate,
    [NAMING_SCREEN_CAUGHT_MON] = &sMonNamingScreenTemplate,
    [NAMING_SCREEN_NICKNAME] = &sMonNamingScreenTemplate,
};

static const u8 sKeyboardChars[][4][20] =
{
#if ENGLISH
    {
        _(" A B C  D E F    . "),
        _(" G H I  J K L    , "),
        _(" M N O  P Q R S    "),
        _(" T U V  W X Y Z    "),
    },
    {
        _(" a b c  d e f    . "),
        _(" g h i  j k l    , "),
        _(" m n o  p q r s    "),
        _(" t u v  w x y z    "),
    },
#elif GERMAN
    {
        _("  ABCD   EFGH   .  "),
        _("  IJKL   MNOP   ,  "),
        _("  QRST   UVWX      "),
        _("  YZ     ÄÖÜ       "),
    },
    {
        _("  abcd   efgh   .  "),
        _("  ijkl   mnop   ,  "),
        _("  qrst   uvwx      "),
        _("  yz     äöü       "),
    },
#endif
    {
        _(" 0  1  2  3  4     "),
        _(" 5  6  7  8  9     "),
        _(" !  ?  ♂  ♀  /  -  "),
        _(" …  “  ”  ‘  '     "),
    },
};

static const struct OamData sOam_8x8 =
{
    .y = 0,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 0,
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};

static const struct OamData sOam_16x16 =
{
    .y = 0,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 1,
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};

static const struct OamData sOam_32x16 =
{
    .y = 0,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 1,
    .x = 0,
    .matrixNum = 0,
    .size = 2,
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};


static const struct Subsprite sSubsprites_PageSwapFrame[] = {
    { -20, -16, ST_OAM_H_RECTANGLE, 1,   0, 1 },
    {  12, -16, ST_OAM_SQUARE, 0,   4, 1 },
    { -20,  -8, ST_OAM_H_RECTANGLE, 1,   5, 1 },
    {  12,  -8, ST_OAM_SQUARE, 0,   9, 1 },
    { -20,   0, ST_OAM_H_RECTANGLE, 1,  10, 1 },
    {  12,   0, ST_OAM_SQUARE, 0,  14, 1 },
    { -20,   8, ST_OAM_H_RECTANGLE, 1,  15, 1 },
    {  12,   8, ST_OAM_SQUARE, 0,  19, 1 }
};

static const struct Subsprite sSubsprites_Unused[] = {
    { -12,  -4, ST_OAM_SQUARE, 0,   0, 1 },
    {   4,  -4, ST_OAM_SQUARE, 0,   1, 1 }
};

static const struct Subsprite sSubsprites_PageSwapText[] = {
    { -12,  -4, ST_OAM_H_RECTANGLE, 0,   0, 1 },
    {   4,  -4, ST_OAM_SQUARE, 0,   2, 1 }
};

static const struct Subsprite sSubsprites_Button[] = {
    { -20, -12, ST_OAM_H_RECTANGLE, 1,   0, 1 },
    {  12, -12, ST_OAM_SQUARE, 0,   4, 1 },
    { -20,  -4, ST_OAM_H_RECTANGLE, 1,   5, 1 },
    {  12,  -4, ST_OAM_SQUARE, 0,   9, 1 },
    { -20,   4, ST_OAM_H_RECTANGLE, 1,  10, 1 },
    {  12,   4, ST_OAM_SQUARE, 0,  14, 1 }
};

static const struct Subsprite sSubsprites_PCIcon[] = {
    {  -8, -12, ST_OAM_H_RECTANGLE, 0,   0, 3 },
    {  -8,  -4, ST_OAM_H_RECTANGLE, 0,   2, 3 },
    {  -8,   4, ST_OAM_H_RECTANGLE, 0,   4, 3 }
};

static const struct SubspriteTable sSubspriteTable_PageSwapFrame[] = {
    { 8, sSubsprites_PageSwapFrame }
};

static const struct SubspriteTable sSubspriteTable_PageSwapText[] = {
    { 2, sSubsprites_PageSwapText },
    { 2, sSubsprites_PageSwapText },
    { 2, sSubsprites_PageSwapText }
};

static const struct SubspriteTable sSubspriteTable_Button[] = {
    { 6, sSubsprites_Button }
};

static const struct SubspriteTable sSubspriteTable_PCIcon[] = {
    { 3, sSubsprites_PCIcon }
};

static const struct SpriteFrameImage sImageTable_PCIcon[] = {
    { sPCIconOff_Gfx, 0xc0 },
    { sPCIconOn_Gfx, 0xc0 }
};

static const union AnimCmd sAnim_Loop[] = {
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_CursorSquish[] = {
    ANIMCMD_FRAME(4, 8),
    ANIMCMD_FRAME(8, 8),
    ANIMCMD_END
};

static const union AnimCmd sAnim_PCIcon[] = {
    ANIMCMD_FRAME(0, 2),
    ANIMCMD_FRAME(1, 2),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd *const sAnims_Loop[] = {
    sAnim_Loop
};

static const union AnimCmd *const sAnims_Cursor[] = {
    sAnim_Loop,
    sAnim_CursorSquish
};

static const union AnimCmd *const sAnims_PCIcon[] = {
    sAnim_PCIcon
};

static const struct SpriteTemplate sSpriteTemplate_PageSwapFrame = {
    2, 4, &sOam_8x8, sAnims_Loop, NULL, gDummySpriteAffineAnimTable, SpriteCB_PageSwap
};

static const struct SpriteTemplate sSpriteTemplate_PageSwapButton = {
    3, 1, &sOam_32x16, sAnims_Loop, NULL, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_PageSwapText = {
    4, 4, &sOam_8x8, sAnims_Loop, NULL, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_BackButton = {
    0, 6, &sOam_8x8, sAnims_Loop, NULL, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_OkButton = {
    1, 6, &sOam_8x8, sAnims_Loop, NULL, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteTemplate sSpriteTemplate_Cursor = {
    7, 5, &sOam_16x16, sAnims_Cursor, NULL, gDummySpriteAffineAnimTable, SpriteCB_Cursor
};

static const struct SpriteTemplate sSpriteTemplate_InputArrow = {
    10, 3, &sOam_8x8, sAnims_Loop, NULL, gDummySpriteAffineAnimTable, SpriteCB_InputArrow
};

static const struct SpriteTemplate sSpriteTemplate_Underscore = {
    11, 3, &sOam_8x8, sAnims_Loop, NULL, gDummySpriteAffineAnimTable, SpriteCB_Underscore
};

static const struct SpriteTemplate sSpriteTemplate_PCIcon = {
    0xFFFF, 0, &sOam_8x8, sAnims_PCIcon, sImageTable_PCIcon, gDummySpriteAffineAnimTable, SpriteCallbackDummy
};

static const struct SpriteSheet sSpriteSheets[] = {
    { gNamingScreenBackButton_Gfx, 0x1E0, 0 },
    { gNamingScreenOKButton_Gfx, 0x1E0, 1 },
    { gNamingScreenPageSwapFrame_Gfx, 0x280, 2 },
    { gNamingScreenPageSwapButton_Gfx, 0x100, 3 },
    { gNamingScreenPageSwapLower_Gfx, 0x60, 4 },
    { gNamingScreenPageSwapUpper_Gfx, 0x60, 5 },
    { gNamingScreenPageSwapOthers_Gfx, 0x60, 6 },
    { gNamingScreenCursor_Gfx, 0x80, 7 },
    { gNamingScreenCursorSquished_Gfx, 0x80, 8 },
    { gNamingScreenCursorFilled_Gfx, 0x80, 9 },
    { gNamingScreenInputArrow_Gfx, 0x20, 10 },
    { gNamingScreenUnderscore_Gfx, 0x20, 11 },
    {}
};

static const struct SpritePalette sSpritePalettes[] = {
    {gNamingScreenMenu_Pal[0], 0},
    {gNamingScreenMenu_Pal[1], 1},
    {gNamingScreenMenu_Pal[2], 2},
    {gNamingScreenMenu_Pal[3], 3},
    {gNamingScreenMenu_Pal[4], 4},
    {gNamingScreenMenu_Pal[5], 5},
    {gNamingScreenMenu_Pal[4], 6},
    {}
};

static const u16 sKeyboardLower_Tilemap[] = INCBIN_U16("graphics/naming_screen/keyboard_lower.bin");

static const u16 sKeyboardUpper_Tilemap[] = INCBIN_U16("graphics/naming_screen/keyboard_upper.bin");

static const u16 sKeyboardSymbols_Tilemap[] = INCBIN_U16("graphics/naming_screen/keyboard_symbols.bin");
