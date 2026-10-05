#include "global.h"
#include "naming_screen.h"
#include "data.h"
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

#define INPUT_NONE 0
#define INPUT_A_BUTTON 5
#define INPUT_B_BUTTON 6
#define INPUT_SELECT 8
#define INPUT_START 9

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
 /*0x10*/ u8 swapBtnFrameSpriteId;
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

enum
{
    PAGE_UPPER,
    PAGE_LOWER,
    PAGE_OTHERS,
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
};

enum
{
    INPUT_STATE_DISABLED,
    INPUT_STATE_ENABLED,
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
static void sub_80B5DFC(void);
static void CreateSprites(void);
static void sub_80B5E3C(void);
static void NamingScreen_InitDisplayMode(void);
static void Task_DoNothing(u8);
static void sub_80B7558(void);
static void sub_80B753C(void);
static void sub_80B7680(void);
static void sub_80B75C4(void);
static void sub_80B7794(void);
static void sub_80B78A8(void);
static void DrawTextEntry(void);
static void CreateCursorSprite(void);
static void CreatePageSwapButtonSprites(void);
static void CreateBackOkSprites(void);
static void CreateTextEntrySprites(void);
static void CreateInputTargetIcon(void);
static void CreateInputHandlerTask(void);
static void CreateButtonFlashTask(void);
static void CreateNamingScreenTask(void);
static void Task_NamingScreen(u8 taskId);
static void SetInputState(u8);
static void SetCursorFlashing(bool8 flashing);
static bool8 HandleKeyboardEvent(void);
static bool8 IsCursorAnimFinished(void);
static void MoveCursorToOKButton(void);
static void StartPageSwapButtonAnim(void);
static void StartPageSwapAnim(void);
static void SetCursorInvisibility(bool8 invisible);
static void TryStartButtonFlash(u8 button, bool8 keepFlashing, bool8 interruptCurFlash);
static bool8 IsPageSwapAnimNotInProgress(void);
static void sub_80B7614(void);
static void GetCursorPos(s16 *, s16 *);
static void SetCursorPos(s16, s16);
static void sub_80B77F8(void);
static void SaveInputText(void);
static void DisplaySentToPCMessage(void);
static u8 GetKeyRoleAtCursorPos(void);
static bool8 SwapKeyboardPage(void);
static void DeleteTextCharacter(void);
static void sub_80B7090(void);
static u8 GetInputEvent(void);
static bool8 AddTextCharacter(void);
static void SquishCursor(void);
static void Task_HandlePageSwapAnim(u8);
static void SetPageSwapButtonGfx(u8 page, struct Sprite *text, struct Sprite *button);
static u8 GetTextEntryPosition(void);
static u8 GetCharAtKeyboardPos(s16 x, s16 y);
static bool8 sub_80B7104(void);
static bool8 sub_80B713C(void);
static void BufferCharacter(u8 ch);
static bool8 sub_80B7198(u8);
static bool8 sub_80B7264(u8);
static void sub_80B7370(u8, u8);
static void sub_80B73CC(u8, u8);
static bool8 sub_80B71E4(u8);
static void sub_80B7474(u8, u8);
static void sub_80B72A4(u8, u8);
static bool8 sub_80B720C(u8);
static void sub_80B7568(void);
static void sub_80B75B0(void);
static void sub_80B7698(u16 *, const u16 *);
static void sub_80B76E0();
static void nullsub_20(u8, u8);
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
        sub_80B5DFC();
        gMain.state++;
        break;
    case 7:
        CreateSprites();
        sub_80B5E3C();
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
    sNamingScreen->state = 0;
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
    Text_LoadWindowTemplate(&gWindowTemplate_81E6E88);
    InitMenuWindow(&gWindowTemplate_81E6E88);
}

static void NamingScreen_ResetObjects(void)
{
    ResetPaletteFade();
    ResetSpriteData();
    FreeAllSpritePalettes();
    ResetTasks();
}

static void sub_80B5DFC(void)
{
    sub_80B7558();
    sub_80B753C();
    sub_80B7680();
    sub_80B75C4();
    sub_80B7794();
    sub_80B78A8();
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

static void sub_80B5E3C(void)
{
    CreateInputHandlerTask();
    CreateButtonFlashTask();
    CreateNamingScreenTask();
}

//--------------------------------------------------
// Naming screen main
//--------------------------------------------------

static bool8 MainState_FadeIn(struct Task *task);
static bool8 MainState_WaitFadeIn(struct Task *);
static bool8 MainState_HandleInput(struct Task *);
static bool8 MainState_MoveToOKButton(struct Task *);
static bool8 MainState_StartPageSwap(struct Task *);
static bool8 MainState_WaitPageSwap(struct Task *);
static bool8 MainState_PressedOKButton(struct Task *task);
static bool8 MainState_WaitSentToPCMessage(struct Task *task);
static bool8 MainState_FadeOut(struct Task *task);
static bool8 MainState_Exit(struct Task *task);

static bool8 (*const sMainStateFuncs[])(struct Task *) =
{
    MainState_FadeIn,
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

static bool8 MainState_FadeIn(struct Task *task)
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
        SetCursorFlashing(1);
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
    SetCursorInvisibility(1);
    TryStartButtonFlash(0, 0, 1);
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
        sNamingScreen->currentPage %= 3;
        sub_80B7614();
        sub_80B77F8();
        SetInputState(INPUT_STATE_ENABLED);
        GetCursorPos(&cursorX, &cursorY);
#if ENGLISH
        if (sNamingScreen->currentPage == PAGE_OTHERS && (cursorX == 6 || cursorX == 7))
            cursorX = 5;
#elif GERMAN
        if (sNamingScreen->currentPage == PAGE_OTHERS && (cursorX == 7 || cursorX == 8))
            cursorX = 6;
#endif
        SetCursorPos(cursorX, cursorY);
        SetCursorInvisibility(0);
    }
    return FALSE;
}

static bool8 MainState_PressedOKButton(struct Task *task)
{
    SaveInputText();
    SetInputState(INPUT_STATE_DISABLED);
    SetCursorFlashing(0);
    TryStartButtonFlash(3, 0, 1);
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


static bool8 KeyboardKeyHandler_Character(u8 input);
static bool8 KeyboardKeyHandler_Page(u8 input);
static bool8 KeyboardKeyHandler_Backspace(u8 input);
static bool8 KeyboardKeyHandler_OK(u8 input);

static bool8 (*const sKeyboardKeyHandlers[])(u8) =
{
    KeyboardKeyHandler_Character,
    KeyboardKeyHandler_Page,
    KeyboardKeyHandler_Backspace,
    KeyboardKeyHandler_OK,
};

static bool8 HandleKeyboardEvent(void)
{
    u8 input = GetInputEvent();
    u8 keyRole = GetKeyRoleAtCursorPos();

    if (input == INPUT_SELECT)
        return SwapKeyboardPage();
    else if (input == INPUT_B_BUTTON)
    {
        DeleteTextCharacter();
        return FALSE;
    }
    else if (input == 7)
    {
        sub_80B7090();
        return FALSE;
    }
    return sKeyboardKeyHandlers[keyRole](input);
}

static bool8 KeyboardKeyHandler_Character(u8 input)
{
    TryStartButtonFlash(3, 0, 0);
    if (input == INPUT_A_BUTTON)
    {
        u8 textFull = AddTextCharacter();

        SquishCursor();
        if (textFull)
        {
            SetInputState(INPUT_STATE_DISABLED);
            sNamingScreen->state = STATE_MOVE_TO_OK_BUTTON;
        }
    }
    return FALSE;
}

static bool8 KeyboardKeyHandler_Page(u8 input)
{
    TryStartButtonFlash(0, 1, 0);
    if (input == INPUT_A_BUTTON)
        return SwapKeyboardPage();
    else
        return FALSE;
}

static bool8 KeyboardKeyHandler_Backspace(u8 input)
{
    TryStartButtonFlash(1, 1, 0);
    if (input == INPUT_A_BUTTON)
        DeleteTextCharacter();
    return FALSE;
}

static bool8 KeyboardKeyHandler_OK(u8 input)
{
    TryStartButtonFlash(2, 1, 0);
    if (input == INPUT_A_BUTTON)
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
    BUTTON_PAGE,
    BUTTON_BACK,
    BUTTON_OK,
};

#define tState data[0]
#define tKeyboardEvent data[1]
#define tButtonId data[2]

static void Input_Disabled(struct Task *task);
static void Input_Enabled(struct Task *task);

static void (*const sInputFuncs[])(struct Task *) =
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
    sInputFuncs[gTasks[taskId].tState](&gTasks[taskId]);
}

static void Input_Disabled(struct Task *task)
{
    task->tKeyboardEvent = 0;
}

static void Input_Enabled(struct Task *task)
{
    task->tKeyboardEvent = 0;
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
    0,   //none
    0,   //up
    0,   //down
    -1,  //left
    1    //right
};

static const s16 sDpadDeltaY[] =
{
    0,   //none
    -1,  //up
    1,   //down
    0,   //left
    0    //right
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
    dpadDir = 0;
    if (JOY_REPT(DPAD_UP))
        dpadDir = 1;
    if (JOY_REPT(DPAD_DOWN))
        dpadDir = 2;
    if (JOY_REPT(DPAD_LEFT))
        dpadDir = 3;
    if (JOY_REPT(DPAD_RIGHT))
        dpadDir = 4;

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
        if (sNamingScreen->currentPage == PAGE_OTHERS && (cursorX == 6 || cursorX == 7))
#elif GERMAN
        if (sNamingScreen->currentPage == PAGE_OTHERS && (cursorX == 6 || cursorX == 7 || cursorX == 8))
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
            task->tButtonId = cursorY;
            cursorY = sKeyRowToButtonRow[cursorY];
        }
        else if (prevCursorX == COLUMN_COUNT - 1)
        {
            if (cursorY == 1)
                cursorY = task->tButtonId;
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
            task->tButtonId = BUTTON_BACK;
        else if (cursorY == 2)
            task->tButtonId = BUTTON_OK;
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
#undef tButtonId

//--------------------------------------------------
//
//--------------------------------------------------

static void Task_UpdateButtonFlash(u8 taskId);
static u16 GetButtonPalOffset(u8 button);
static void RestoreButtonColor(u8 button);
static void StartButtonFlash(struct Task *task, u8 button, bool8 keepFlashing);

static void CreateButtonFlashTask(void)
{
    u8 taskId;

    taskId = CreateTask(Task_UpdateButtonFlash, 3);
    gTasks[taskId].data[0] = 3;
}

static void TryStartButtonFlash(u8 button, bool8 keepFlashing, bool8 interruptCurFlash)
{
    struct Task *task = &gTasks[FindTaskIdByFunc(Task_UpdateButtonFlash)];

    if (button == task->data[0] && interruptCurFlash == 0)
    {
        task->data[1] = keepFlashing;
        task->data[2] = 1;
        return;
    }
    if (button == 3 && task->data[1] == 0 && interruptCurFlash == 0)
        return;
    if (task->data[0] != 3)
        RestoreButtonColor(task->data[0]);
    StartButtonFlash(task, button, keepFlashing);
}

static void Task_UpdateButtonFlash(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    if (task->data[0] == 3 || task->data[2] == 0)
        return;
    MultiplyInvertedPaletteRGBComponents(GetButtonPalOffset(task->data[0]), task->data[3], task->data[3], task->data[3]);
    if (task->data[5] != 0)
    {
        task->data[5]--;
        if (task->data[5] != 0)
            return;
    }
    task->data[5] = 2;
    task->data[3] += task->data[4];
    if (task->data[3] == 16)
        task->data[4] = -task->data[4];
    else if (task->data[3] == 0)
    {
        task->data[2] = task->data[1];
        task->data[4] = -task->data[4];
    }
}

static u16 GetButtonPalOffset(u8 button)
{
    const u16 palOffsets[] =
    {
        IndexOfSpritePaletteTag(4) * 16 + 0x10E,
        IndexOfSpritePaletteTag(6) * 16 + 0x10C,
        IndexOfSpritePaletteTag(6) * 16 + 0x10E,
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
    task->data[0] = button;
    task->data[1] = keepFlashing;
    task->data[2] = 1;
    task->data[3] = 15;
    task->data[4] = 1;
    task->data[5] = 0;
}

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
    u16 *const arr[] =
    {
        &sNamingScreen->bg2vOffset,
        &sNamingScreen->bg1vOffset
    };

    task->tFrameCount += 4;
    *arr[sNamingScreen->bgToReveal] = Sin(task->tFrameCount, 40);
    *arr[sNamingScreen->bgToHide] = Sin((task->tFrameCount + 128) & 0xFF, 40);
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
    u16 *const arr[] = {&sNamingScreen->bg2vOffset, &sNamingScreen->bg1vOffset};

    task->tFrameCount += 4;
    *arr[sNamingScreen->bgToReveal] = Sin(task->tFrameCount, 40);
    *arr[sNamingScreen->bgToHide] = Sin((task->tFrameCount + 128) & 0xFF, 40);
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

static void CreateCursorSprite(void)
{
    sNamingScreen->cursorSpriteId = CreateSprite(&sSpriteTemplate_Cursor, 0, 0, 0);
    gSprites[sNamingScreen->cursorSpriteId].oam.priority = 1;
    gSprites[sNamingScreen->cursorSpriteId].oam.objMode = 1;
    gSprites[sNamingScreen->cursorSpriteId].data[6] = 1;
    gSprites[sNamingScreen->cursorSpriteId].data[6] = 2;
    SetCursorPos(0, 0);
}

static const u8 sKeyboardSymbolPositions[][COLUMN_COUNT] = {
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
    return sKeyboardSymbolPositions[sNamingScreen->currentPage][x];
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
    sprite->invisible = (sprite->data[4] & 0xFF);
    if (sprite->data[0] == COLUMN_COUNT - 1)
        sprite->invisible = TRUE;
    if (sprite->invisible || (sprite->data[4] & 0xFF00) == 0
     || sprite->data[0] != sprite->data[2] || sprite->data[1] != sprite->data[3])
    {
        sprite->data[5] = 0;
        sprite->data[6] = 1;
        sprite->data[7] = 2;
    }
    sprite->data[7]--;
    if (sprite->data[7] == 0)
    {
        sprite->data[5] += sprite->data[6];
        if (sprite->data[5] == 16 || sprite->data[5] == 0)
            sprite->data[6] = -sprite->data[6];
        sprite->data[7] = 2;
    }
    if ((sprite->data[4] & 0xFF00) != 0)
    {
        s8 gb = sprite->data[5];
        s8 r = sprite->data[5] >> 1;
        u16 index = IndexOfSpritePaletteTag(5) * 16 + 0x0101;

        MultiplyInvertedPaletteRGBComponents(index, r, gb, gb);
    }
}

static void CreatePageSwapButtonSprites(void)
{
    u8 frameSpriteId;
    u8 textSpriteId;
    u8 buttonSpriteId;

    frameSpriteId = CreateSprite(&sSpriteTemplate_PageSwapFrame, 0xCC, 0x50, 0);
    sNamingScreen->swapBtnFrameSpriteId = frameSpriteId;
    SetSubspriteTables(&gSprites[frameSpriteId], sSubspriteTable_PageSwapFrame);

    textSpriteId = CreateSprite(&sSpriteTemplate_PageSwapText, 0xCC, 0x4C, 1);
    gSprites[frameSpriteId].data[6] = textSpriteId;
    SetSubspriteTables(&gSprites[textSpriteId], sSubspriteTable_PageSwapText);

    buttonSpriteId = CreateSprite(&sSpriteTemplate_PageSwapButton, 0xCC, 0x4B, 2);
    gSprites[buttonSpriteId].oam.priority = 1;
    gSprites[frameSpriteId].data[7] = buttonSpriteId;
}

static void StartPageSwapButtonAnim(void)
{
    struct Sprite *sprite = &gSprites[sNamingScreen->swapBtnFrameSpriteId];

    sprite->data[0] = 2;
    sprite->data[1] = sNamingScreen->currentPage;
}

static bool8 PageSwapSprite_Init(struct Sprite *sprite);
static bool8 PageSwapSprite_Idle(struct Sprite *sprite);
static bool8 PageSwapSprite_SlideOff(struct Sprite *sprite);
static bool8 PageSwapSprite_SlideOn(struct Sprite *sprite);

static u8 (*const sPageSwapSpriteFuncs[])(struct Sprite *) =
{
    PageSwapSprite_Init,
    PageSwapSprite_Idle,
    PageSwapSprite_SlideOff,
    PageSwapSprite_SlideOn,
};

static void SpriteCB_PageSwap(struct Sprite *sprite)
{
    while (sPageSwapSpriteFuncs[sprite->data[0]](sprite) != 0)
        ;
}

static bool8 PageSwapSprite_Init(struct Sprite *sprite)
{
    struct Sprite *text = &gSprites[sprite->data[6]];
    struct Sprite *button = &gSprites[sprite->data[7]];

    SetPageSwapButtonGfx(sNamingScreen->currentPage, text, button);
    sprite->data[0]++;
    return 0;
}

static bool8 PageSwapSprite_Idle(struct Sprite *sprite)
{
    return 0;
}

static bool8 PageSwapSprite_SlideOff(struct Sprite *sprite)
{
    struct Sprite *text = &gSprites[sprite->data[6]];
    struct Sprite *button = &gSprites[sprite->data[7]];

    text->y2++;
    if (text->y2 > 7)
    {
        sprite->data[0]++;
        text->y2 = -4;
        text->invisible = TRUE;
        SetPageSwapButtonGfx(((u8)sprite->data[1] + 1) % 3, text, button);
    }
    return 0;
}

static bool8 PageSwapSprite_SlideOn(struct Sprite *sprite)
{
    struct Sprite *text = &gSprites[sprite->data[6]];

    text->invisible = FALSE;
    text->y2++;
    if (text->y2 >= 0)
    {
        text->y2 = 0;
        sprite->data[0] = 1;
    }
    return 0;
}

static const u16 sPageSwapPalTags[] = {1, 3, 2};
static const u16 sPageSwapGfxTags[] = {4, 6, 5};

static void SetPageSwapButtonGfx(u8 page, struct Sprite *text, struct Sprite *button)
{
    button->oam.paletteNum = IndexOfSpritePaletteTag(sPageSwapPalTags[page]);
    text->sheetTileStart = GetSpriteTileStartByTag(sPageSwapGfxTags[page]);
    text->subspriteTableNum = page;
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
    s16 xPos;
    u8 i;

    xPos = (sNamingScreen->nameLeftOffset - 1) * 8 + 4;
    spriteId = CreateSprite(&sSpriteTemplate_InputArrow, xPos, 0x28, 0);
    gSprites[spriteId].oam.priority = 3;
    xPos = sNamingScreen->nameLeftOffset * 8 + 4;
    for (i = 0; i < sNamingScreen->template->maxChars; i++, xPos += 8)
    {
        spriteId = CreateSprite(&sSpriteTemplate_Underscore, xPos, 0x2C, 0);
        gSprites[spriteId].oam.priority = 3;
        gSprites[spriteId].data[0] = i;
    }
}

static void SpriteCB_InputArrow(struct Sprite *sprite)
{
    const s16 x[] = {0, -4, -2, -1};

    if (sprite->data[0] == 0 || --sprite->data[0] == 0)
    {
        sprite->data[0] = 8;
        sprite->data[1] = (sprite->data[1] + 1) & 3;
    }
    sprite->x2 = x[sprite->data[1]];
}

static void SpriteCB_Underscore(struct Sprite *sprite)
{
    const s16 y[] = {2, 3, 2, 1};
    u8 pos;

    pos = GetTextEntryPosition();
    if (pos != (u8)sprite->data[0])
    {
        sprite->y2 = 0;
        sprite->data[1] = 0;
        sprite->data[2] = 0;
    }
    else
    {
        sprite->y2 = y[sprite->data[1]];
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
    sNamingScreen->textBuffer[index] = 0;
    DrawTextEntry();
    sNamingScreen->textBuffer[index] = EOS;
    keyRole = GetKeyRoleAtCursorPos();
    if (keyRole == 0 || keyRole == 2)
        TryStartButtonFlash(1, 0, 1);
    PlaySE(SE_BALL);
}

static bool8 AddTextCharacter(void)
{
    s16 x;
    s16 y;
    u8 ch;
    bool8 r4;

    GetCursorPos(&x, &y);
    x = CursorColToKeyboardCol(x);
    ch = GetCharAtKeyboardPos(x, y);
    r4 = 1;
    if (ch == 0xFF)
        r4 = sub_80B7104();
    else if (ch == 0xFE)
        r4 = sub_80B713C();
    else
        BufferCharacter(ch);
    DrawTextEntry();
    PlaySE(SE_SELECT);
    if (r4)
    {
        if (GetPreviousTextCaretPosition() == sNamingScreen->template->maxChars - 1)
            return TRUE;
    }
    return FALSE;
}

static void sub_80B7090(void) // DoInput?
{
    u8 r5;
    u8 r4;

    r5 = GetPreviousTextCaretPosition();
    r4 = sNamingScreen->textBuffer[r5];
    if (sub_80B7198(r4))
    {
        if (sub_80B7264(r4))
            sub_80B7370(r4, r5);
        else
            sub_80B73CC(r4, r5);
    }
    else
    {
        if (sub_80B71E4(r4))
            sub_80B7474(r4, r5);
        else
            sub_80B72A4(r4, r5);
    }
    DrawTextEntry();
    PlaySE(SE_SELECT);
}

static bool8 sub_80B7104(void)
{
    u8 r5;
    u8 r4;

    r5 = GetPreviousTextCaretPosition();
    r4 = sNamingScreen->textBuffer[r5];
    if (sub_80B720C(r4))
    {
        sub_80B72A4(r4, r5);
        return TRUE;
    }
    return FALSE;
}

static bool8 sub_80B713C(void)
{
    u8 r5;
    u8 r4;

    r5 = GetPreviousTextCaretPosition();
    r4 = sNamingScreen->textBuffer[r5];
    if (sub_80B7264(r4))
    {
        sub_80B7370(r4, r5);
        return TRUE;
    }
    return FALSE;
}

static void BufferCharacter(u8 ch)
{
    u8 index = GetTextEntryPosition();

    sNamingScreen->textBuffer[index] = ch;
}

static bool8 sub_80B7198(u8 a)
{
    if ((a >= 55 && a <= 74)
     || (a >= 135 && a <= 139)
     || (a >= 140 && a <= 144)
     || (a >= 145 && a <= 149)
     || (a >= 150 && a <= 154))
        return TRUE;
    else
        return FALSE;
}

static bool8 sub_80B71E4(u8 a)
{
    if ((a >= 75 && a <= 79)
     || (a >= 155 && a <= 159))
        return TRUE;
    else
        return FALSE;
}

static bool8 sub_80B720C(u8 a)
{
    if ((a >= 6 && a <= 20)
     || (a >= 26 && a <= 30)
     || (a >= 75 && a <= 79)
     || (a >= 86 && a <= 100)
     || (a >= 106 && a <= 110)
     || (a >= 155 && a <= 159))
        return TRUE;
    else
        return FALSE;
}

static bool8 sub_80B7264(u8 a)
{
    if ((a >= 26 && a <= 30)
     || (a >= 70 && a <= 74)
     || (a >= 106 && a <= 110)
     || (a >= 150 && a <= 154))
        return TRUE;
    else
        return FALSE;
}

static void sub_80B72A4(u8 a, u8 b)
{
    u8 chr = a;

    if (a >= 6 && a <= 10)
        chr = a + 0x31;
    else if (a >= 11 && a <= 15)
        chr = a + 0x31;
    else if (a >= 16 && a <= 20)
        chr = a + 0x31;
    else if (a >= 26 && a <= 30)
        chr = a + 0x2C;
    else if (a >= 75 && a <= 79)
        chr = a + 0xFB;
    else if (a >= 86 && a <= 90)
        chr = a + 0x31;
    else if (a >= 91 && a <= 95)
        chr = a + 0x31;
    else if (a >= 96 && a <= 100)
        chr = a + 0x31;
    else if (a >= 106 && a <= 110)
        chr = a + 0x2C;
    else if (a >= 155 && a <= 159)
        chr = a + 0xFB;
    sNamingScreen->textBuffer[b] = chr;
}

static void sub_80B7370(u8 a, u8 b)
{
    u8 chr = a;

    if (a >= 26 && a <= 30)
        chr = a + 0x31;
    else if (a >= 70 && a <= 74)
        chr = a + 5;
    else if (a >= 106 && a <= 110)
        chr = a + 0x31;
    else if (a >= 150 && a <= 154)
        chr = a + 5;
    sNamingScreen->textBuffer[b] = chr;
}

static void sub_80B73CC(u8 a, u8 b)
{
    u8 chr = a;

    if (a >= 55 && a <= 59)
        chr = a + 0xCF;
    else if (a >= 60 && a <= 64)
        chr = a + 0xCF;
    else if (a >= 65 && a <= 69)
        chr = a + 0xCF;
    else if (a >= 70 && a <= 74)
        chr = a + 0xD4;
    else if (a >= 135 && a <= 139)
        chr = a + 0xCF;
    else if (a >= 140 && a <= 144)
        chr = a + 0xCF;
    else if (a >= 145 && a <= 149)
        chr = a + 0xCF;
    else if (a >= 150 && a <= 154)
        chr = a + 0xD4;
    sNamingScreen->textBuffer[b] = chr;
}

static void sub_80B7474(u8 a, u8 b)
{
    u8 chr = a;

    if (a >= 75 && a <= 79)
        chr = a + 0xCF;
    else if (a >= 155 && a <= 159)
        chr = a + 0xCF;
    sNamingScreen->textBuffer[b] = chr;
}

static void SaveInputText(void)
{
    u8 i;

    for (i = 0; i < sNamingScreen->template->maxChars; i++)
    {
        if (sNamingScreen->textBuffer[i] != 0 && sNamingScreen->textBuffer[i] != 0xFF)
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
    BasicInitMenuWindow(&gWindowTemplate_81E6E88);
    Menu_DisplayDialogueFrame();
    MenuPrintMessageDefaultCoords(gStringVar4);
}

static void sub_80B753C(void)
{
    LoadSpriteSheets(sSpriteSheets);
    LoadSpritePalettes(sSpritePalettes);
}

static void sub_80B7558(void)
{
    sub_80B7568();
    sub_80B75B0();
}

static void sub_80B7568(void)
{
    DmaCopy16Defvars(3, gNamingScreenMenu_Gfx, (void *)(VRAM + gMenuMessageBoxContentTileOffset * 32), 0x800);
    DmaCopy16Defvars(3, gNamingScreenMenu_Gfx, (void *)(VRAM + 0x8000 + gMenuMessageBoxContentTileOffset * 32), 0x800);
}

static void sub_80B75B0(void)
{
    LoadPalette(gNamingScreenMenu_Pal, 0, 0x80);
}

static void sub_80B7650(u16 *);
static void sub_80B7660(u16 *);
static void sub_80B7670(u16 *);

static void (*const gUnknown_083CE2F0[][2])(u16 *) =
{
    {sub_80B7660, sub_80B7650},
    {sub_80B7650, sub_80B7670},
    {sub_80B7670, sub_80B7660},
};

static void sub_80B75C4(void)
{
    u16 *const arr[] =
    {
        (u16 *)(VRAM + 0xE000),
        (u16 *)(VRAM + 0xE800),
    };

    gUnknown_083CE2F0[sNamingScreen->currentPage][0](arr[sNamingScreen->bgToReveal]);
    gUnknown_083CE2F0[sNamingScreen->currentPage][1](arr[sNamingScreen->bgToHide]);
}

static void sub_80B7614(void)
{
    u16 *const arr[] =
    {
        (u16 *)(VRAM + 0xE000),
        (u16 *)(VRAM + 0xE800),
    };

    gUnknown_083CE2F0[sNamingScreen->currentPage][1](arr[sNamingScreen->bgToHide]);
}

static void sub_80B7650(u16 *vramBuffer)
{
    sub_80B7698(vramBuffer, gNamingScreenKeyboardLower_Tilemap);
}

static void sub_80B7660(u16 *vramBuffer)
{
    sub_80B7698(vramBuffer, gNamingScreenKeyboardUpper_Tilemap);
}

static void sub_80B7670(u16 *vramBuffer)
{
    sub_80B7698(vramBuffer, gNamingScreenKeyboardSymbols_Tilemap);
}

static void sub_80B7680(void)
{
    sub_80B76E0(VRAM + 0xF000, (const u16 *)gNamingScreenBackground_Tilemap);
}

static void sub_80B7698(u16 *vramBuffer, const u16 *src)
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

static void sub_80B76E0(u16 *vramBuffer, const u16 *src)
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

static void sub_80B772C(void)
{
    nullsub_20(sNamingScreen->currentPage, sNamingScreen->bgToReveal);
}

static void sub_80B7740(void)
{
    nullsub_20((sNamingScreen->currentPage + 1) % 3, sNamingScreen->bgToHide);
}

static void nullsub_20(u8 a, u8 b)
{
}

static void sub_80B7838(void);
static void sub_80B7844(void);
static void sub_80B7850(void);

static void (*const gUnknown_083CE310[][2])(void) =
{
    sub_80B7844,
    sub_80B7838,
    sub_80B7838,
    sub_80B7850,
    sub_80B7850,
    sub_80B7844,
};

static const struct WindowTemplate *const gUnknown_083CE328[][2][2] =
{
    {
        {&gWindowTemplate_81E6EDC, &gWindowTemplate_81E6EF8},
        {&gWindowTemplate_81E6EA4, &gWindowTemplate_81E6EC0},
    },
    {
        {&gWindowTemplate_81E6EA4, &gWindowTemplate_81E6EC0},
        {&gWindowTemplate_81E6F14, &gWindowTemplate_81E6F30},
    },
    {
        {&gWindowTemplate_81E6F14, &gWindowTemplate_81E6F30},
        {&gWindowTemplate_81E6EDC, &gWindowTemplate_81E6EF8},
    },
};

static void nullsub_61(void);
static void sub_80B78F8(void);

static void (*const gUnknown_083CE358[])(void) =
{
    nullsub_61,
    nullsub_61,
    sub_80B78F8,
    sub_80B78F8,
};

static void DummyGenderIcon(void);
static void DrawGenderIcon(void);

static void (*const sDrawGenderIconFuncs[])(void) =
{
    DummyGenderIcon,
    DrawGenderIcon,
};

static const u8 sKeyboardCharacters[][4][20];  //forward declaration

static u8 GetCharAtKeyboardPos(s16 x, s16 y)
{
    return sKeyboardCharacters[sNamingScreen->currentPage][y][x];
}

static void sub_80B7794(void)
{
    BasicInitMenuWindow(gUnknown_083CE328[sNamingScreen->currentPage][0][sNamingScreen->bgToReveal]);
    gUnknown_083CE310[sNamingScreen->currentPage][0]();
    BasicInitMenuWindow(gUnknown_083CE328[sNamingScreen->currentPage][1][sNamingScreen->bgToHide]);
    gUnknown_083CE310[sNamingScreen->currentPage][1]();
    sub_80B772C();
    sub_80B7740();
}

static void sub_80B77F8(void)
{
    BasicInitMenuWindow(gUnknown_083CE328[sNamingScreen->currentPage][1][sNamingScreen->bgToHide]);
    gUnknown_083CE310[sNamingScreen->currentPage][1]();
    sub_80B7740();
}

static void sub_80B7838(void)
{
    PrintKeyboardCharacters(1);
}

static void sub_80B7844(void)
{
    PrintKeyboardCharacters(0);
}

static void sub_80B7850(void)
{
    PrintKeyboardCharacters(2);
}

static void PrintKeyboardCharacters(u8 page)  //print letters on page
{
    s16 i;
    s16 r5;

    for (i = 0, r5 = 9; i < 4; i++, r5 += 2)
        Menu_PrintText(sKeyboardCharacters[page][i], 3, r5);
}

static void sub_80B78A8(void)
{
    BasicInitMenuWindow(&gWindowTemplate_81E6F4C);
    gUnknown_083CE358[sNamingScreen->templateNum]();
    sDrawGenderIconFuncs[sNamingScreen->template->addGenderIcon]();
    Menu_PrintText(sNamingScreen->template->title, 9, 2);
}

static void nullsub_61(void)
{
}

static void sub_80B78F8(void)
{
    StringCopy(gStringVar1, gSpeciesNames[(s16)sNamingScreen->speciesOrPlayerGender]);
}

static void DummyGenderIcon(void)
{
}

static void DrawGenderIcon(void)
{
    u8 text[2] = _("♂");

    if ((s16)sNamingScreen->monGender != MON_GENDERLESS)
    {
        if ((s16)sNamingScreen->monGender == MON_FEMALE)
            text[0] = 0xB6;  //female symbol
        Menu_PrintText(text, 0x14, 4);
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
    BasicInitMenuWindow(&gWindowTemplate_81E6F4C);
    Menu_PrintText(gStringVar1, sNamingScreen->nameLeftOffset, 4);
}

//--------------------------------------------------
// Forward-declared variables
//--------------------------------------------------

static const struct NamingScreenTemplate sPlayerNamingScreenTemplate =
{
    .copyExistingString = 0,
    .maxChars = 7,
    .iconFunction = 1,
    .addGenderIcon = 0,
    .initialPage = 0,
    .title = OtherText_YourName,
};

static const struct NamingScreenTemplate sPCBoxNamingTemplate =
{
    .copyExistingString = 0,
    .maxChars = 8,
    .iconFunction = 2,
    .addGenderIcon = 0,
    .initialPage = 0,
    .title = OtherText_BoxName,
};

static const struct NamingScreenTemplate sMonNamingScreenTemplate =
{
    .copyExistingString = 0,
    .maxChars = 10,
    .iconFunction = 3,
    .addGenderIcon = 1,
    .initialPage = 0,
    .title = OtherText_PokeName,
};

static const struct NamingScreenTemplate *const sNamingScreenTemplates[] =
{
    &sPlayerNamingScreenTemplate,
    &sPCBoxNamingTemplate,
    &sMonNamingScreenTemplate,
    &sMonNamingScreenTemplate,
};

static const u8 sKeyboardCharacters[][4][20] =
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

const struct Subsprite gSubspriteTable_83CE4F0[] = {
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

const u16 gNamingScreenKeyboardLower_Tilemap[] = INCBIN_U16("graphics/naming_screen/keyboard_lower.bin");

const u16 gNamingScreenKeyboardUpper_Tilemap[] = INCBIN_U16("graphics/naming_screen/keyboard_upper.bin");

const u16 gNamingScreenKeyboardSymbols_Tilemap[] = INCBIN_U16("graphics/naming_screen/keyboard_symbols.bin");

