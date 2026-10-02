#include "global.h"
#include "contest.h"
#include "data2.h"
#include "ewram.h"
#include "field_fadetransition.h"
#include "main.h"
#include "menu.h"
#include "menu_cursor.h"
#include "move_relearner.h"
#include "overworld.h"
#include "palette.h"
#include "pokemon.h"
#include "pokemon_summary_screen.h"
#include "script.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "strings.h"
#include "strings2.h"
#include "task.h"
#include "trig.h"
#include "constants/rgb.h"
#include "constants/songs.h"

#define MENU_STATE_FADE_TO_BLACK                 0
#define MENU_STATE_WAIT_FOR_FADE                  1
#define MENU_STATE_UNREACHABLE                    2
#define MENU_STATE_SETUP_BATTLE_MODE              3
#define MENU_STATE_IDLE_BATTLE_MODE               4
#define MENU_STATE_SETUP_CONTEST_MODE             5
#define MENU_STATE_IDLE_CONTEST_MODE              6
#define MENU_STATE_PRINT_TEACH_MOVE_PROMPT        8
#define MENU_STATE_TEACH_MOVE_CONFIRM             9
#define MENU_STATE_PRINT_GIVE_UP_PROMPT           12
#define MENU_STATE_GIVE_UP_CONFIRM                13
#define MENU_STATE_FADE_AND_RETURN                14
#define MENU_STATE_RETURN_TO_FIELD                15
#define MENU_STATE_PRINT_TRYING_TO_LEARN_PROMPT   16
#define MENU_STATE_WAIT_FOR_TRYING_TO_LEARN       17
#define MENU_STATE_CONFIRM_DELETE_OLD_MOVE        18
#define MENU_STATE_PRINT_WHICH_MOVE_PROMPT        19
#define MENU_STATE_SHOW_MOVE_SUMMARY_SCREEN       20
#define MENU_STATE_UNUSED_21                      21
#define MENU_STATE_UNUSED_22                      22
#define MENU_STATE_PRINT_STOP_TEACHING            24
#define MENU_STATE_WAIT_FOR_STOP_TEACHING         25
#define MENU_STATE_CONFIRM_STOP_TEACHING          26
#define MENU_STATE_CHOOSE_SETUP_STATE             27
#define MENU_STATE_FADE_FROM_SUMMARY_SCREEN       28
#define MENU_STATE_TRY_OVERWRITE_MOVE             29
#define MENU_STATE_DOUBLE_FANFARE_FORGOT_MOVE     30
#define MENU_STATE_PRINT_TEXT_THEN_FANFARE        31
#define MENU_STATE_WAIT_FOR_FANFARE               32
#define MENU_STATE_WAIT_FOR_A_BUTTON              33

#define GFXTAG_UI 5525
#define PALTAG_UI 5526

#define MOVE_RELEARNER_HEART_COUNT 8
#define MOVE_RELEARNER_UI_SPRITE_COUNT 20

enum
{
    MOVE_RELEARNER_SPRITE_LEFT_ARROW,
    MOVE_RELEARNER_SPRITE_RIGHT_ARROW,
    MOVE_RELEARNER_SPRITE_UP_ARROW,
    MOVE_RELEARNER_SPRITE_DOWN_ARROW,
    MOVE_RELEARNER_SPRITE_APPEAL_HEARTS,
    MOVE_RELEARNER_SPRITE_JAM_HEARTS = MOVE_RELEARNER_SPRITE_APPEAL_HEARTS + MOVE_RELEARNER_HEART_COUNT,
};

enum
{
    APPEAL_HEART_EMPTY,
    APPEAL_HEART_FULL,
    JAM_HEART_EMPTY,
    JAM_HEART_FULL,
};

enum
{
    MOVE_RELEARNER_UI_SPRITE_STATIC,
    MOVE_RELEARNER_UI_SPRITE_HORIZONTAL_ARROW,
    MOVE_RELEARNER_UI_SPRITE_VERTICAL_ARROW,
};

enum
{
    MOVE_RELEARNER_HORIZONTAL_ARROW_LEFT,
    MOVE_RELEARNER_HORIZONTAL_ARROW_RIGHT,
};

enum
{
    MOVE_RELEARNER_VERTICAL_ARROW_DOWN,
    MOVE_RELEARNER_VERTICAL_ARROW_UP,
};

extern u16 gSpecialVar_0x8004;
extern u16 gSpecialVar_0x8005;
extern u8 gTileBuffer[];

extern const struct WindowTemplate gMenuTextWindowTemplate;
extern const struct WindowTemplate gMoveRelearnerMenuFramesWindowTemplate;
extern const u8 *const gContestEffectStrings[];
extern const u8 gTypeNames[NUMBER_OF_MON_TYPES][TYPE_NAME_LENGTH + 1];
extern const u8 *const gContestCategoryNames[];

#ifdef GERMAN
extern const u8 deuOtherText_ForgotAndLearned[];
#endif

static void Task_WaitForFadeOut(u8);
static void CB2_InitLearnMove(void);
static void CB2_MoveRelearnerMain(void);
static void DoMoveRelearnerMain(void);
static void DrawLearnMoveMenuWindow(void);
static void DrawBattleMoveInfoHeaders(bool8);
static u8 ChangeToContestMoveInfoWindow(void);
static void DrawContestMoveInfoHeaders(bool8);
static u8 ChangeToBattleMoveInfoWindow(void);
static void ResetMoveRelearnerMenuState(void);
static void CreateMoveRelearnerUiSprites(void);
static void CreateLearnableMovesList(void);
static void HandleInput(void);
static void DrawMoveSelectionWindow(void);
static void DrawMoveInfoWindow(bool8, int);
static void RedrawMoveInfoWindow(void);
static void SpriteCB_MoveRelearnerUiSprite(struct Sprite *);

struct MoveRelearnerMenu
{
    u8 state;
    u8 filler1;
    u8 unused2;
    u8 spriteIds[MOVE_RELEARNER_UI_SPRITE_COUNT];
    u8 filler17;
    u8 cursorRow;
    u8 scrollOffset;
    u8 numMenuChoices;
    u8 selectedMoveIndex;
    u8 previousCursorRow;
    bool8 redrawCursor;
    bool8 redrawMoveSelectionWindow;
    u16 movesToLearn[MAX_RELEARNER_MOVES];
    u8 filler48[10];
    u8 moveNames[6][25];
    u8 fillerE8[475];
    bool8 redrawMoveInfoWindow;
    bool8 showContestInfo;
    u8 partyMonIndex;
    u8 forgetMoveIndex;
};

static struct MoveRelearnerMenu *sMoveRelearnerMenu;

static const u16 sMoveRelearnerUiPalette[] = INCBIN_U16("graphics/move_relearner/arrows.gbapal");

static const u8 sMoveRelearnerUiGfx[] = INCBIN_U8("graphics/move_relearner/arrows.4bpp");

static const u8 sMoveRelearnerWindowFrameDimensions[][4] =
{
    { 0,  0,  9, 13},
    {10,  0, 29,  7},
    { 2, 14, 27, 19},
    {10,  8, 29, 13},
};

struct MoveRelearnerMoveInfoHeader
{
    const u8 *text;
    u8 left;
    u8 right;
    u8 index; // unused
};

static const struct MoveRelearnerMoveInfoHeader sMoveRelearnerMoveInfoHeaders[][4] =
{
    {
        {OtherText_Battle,   1, 1, 0},
        {OtherText_Power,    1, 4, 1},
        {OtherText_Accuracy, 1, 9, 2},
        {NULL,               0, 0, 0},
    },
    {
        {OtherText_Contest,  1, 1, 0},
        {OtherText_Appeal,   1, 4, 1},
        {OtherText_Jam,      1, 9, 2},
        {NULL,               0, 0, 0},
    },
};

const u32 gMoveRelearnerUnusedData = 0xFF00FFEF;
const u8 *const gMoveRelearnerUnusedTileBuffer = gTileBuffer;

static const struct OamData sHeartSpriteOamData = {.shape = 0};
static const struct OamData sHorizontalArrowSpriteOamData = {.shape = 2};
static const struct OamData sVerticalArrowSpriteOamData = {.shape = 1};

static const union AnimCmd sAnim_HorizontalArrowLeft[] =
{
    ANIMCMD_FRAME(2, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_HorizontalArrowRight[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_HorizontalArrow[] =
{
    sAnim_HorizontalArrowLeft,
    sAnim_HorizontalArrowRight,
};

static const struct SpriteSheet sMoveRelearnerUiSpriteSheet = {sMoveRelearnerUiGfx, sizeof(sMoveRelearnerUiGfx), GFXTAG_UI};
static const struct SpritePalette sMoveRelearnerUiSpritePalette = {sMoveRelearnerUiPalette, PALTAG_UI};

static const struct SpriteTemplate sHorizontalArrowSpriteTemplate =
{
    .tileTag = GFXTAG_UI,
    .paletteTag = PALTAG_UI,
    .oam = &sHorizontalArrowSpriteOamData,
    .anims = sAnims_HorizontalArrow,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_MoveRelearnerUiSprite,
};

static const union AnimCmd sAnim_VerticalArrowDown[] =
{
    ANIMCMD_FRAME(4, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_VerticalArrowUp[] =
{
    ANIMCMD_FRAME(6, 5),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_VerticalArrow[] =
{
    sAnim_VerticalArrowDown,
    sAnim_VerticalArrowUp,
};

static const struct SpriteTemplate sVerticalArrowSpriteTemplate =
{
    .tileTag = GFXTAG_UI,
    .paletteTag = PALTAG_UI,
    .oam = &sVerticalArrowSpriteOamData,
    .anims = sAnims_VerticalArrow,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_MoveRelearnerUiSprite,
};

static const union AnimCmd sAnim_HeartAppealEmpty[] =
{
    ANIMCMD_FRAME(8, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_HeartAppealFull[] =
{
    ANIMCMD_FRAME(9, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_HeartJamEmpty[] =
{
    ANIMCMD_FRAME(10, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_HeartJamFull[] =
{
    ANIMCMD_FRAME(11, 5),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_Heart[] =
{
    sAnim_HeartAppealEmpty,
    sAnim_HeartAppealFull,
    sAnim_HeartJamEmpty,
    sAnim_HeartJamFull,
};

static const struct SpriteTemplate sHeartSpriteTemplate =
{
    .tileTag = GFXTAG_UI,
    .paletteTag = PALTAG_UI,
    .oam = &sHeartSpriteOamData,
    .anims = sAnims_Heart,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_MoveRelearnerUiSprite,
};

const u8 gString_AkitoMori[] = _("あきと");  // programmer Akito Mori?

static void VBlankCB_MoveRelearner(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

void TeachMoveRelearnerMove(void)
{
    LockPlayerFieldControls();
    CreateTask(Task_WaitForFadeOut, 10);
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
}

static void Task_WaitForFadeOut(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(CB2_InitLearnMove);
        gFieldCallback = FieldCB_ContinueScriptHandleMusic;
        DestroyTask(taskId);
    }
}

static void CB2_InitLearnMove(void)
{
    REG_DISPCNT = 0;
    ResetSpriteData();
    FreeAllSpritePalettes();
    ResetTasks();
    sMoveRelearnerMenu = sMoveRelearnerMenuSharedMem;
    ResetMoveRelearnerMenuState();
    sMoveRelearnerMenu->partyMonIndex = gSpecialVar_0x8004;
    CreateLearnableMovesList();
    SetVBlankCallback(VBlankCB_MoveRelearner);

    Text_LoadWindowTemplate(&gMoveRelearnerMenuFramesWindowTemplate);
    InitMenuWindow(&gMoveRelearnerMenuFramesWindowTemplate);
    Menu_EraseScreen();

    Text_LoadWindowTemplate(&gMenuTextWindowTemplate);
    InitMenuWindow(&gMenuTextWindowTemplate);
    Menu_EraseScreen();

    REG_BG0VOFS = 0;
    REG_BG0VOFS = 0;
    REG_BG1HOFS = 0;
    REG_BG1HOFS = 0;

    LoadSpriteSheet(&sMoveRelearnerUiSpriteSheet);
    LoadSpritePalette(&sMoveRelearnerUiSpritePalette);
    CreateMoveRelearnerUiSprites();
    FillPalette(0, 0, 2);
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
    SetMainCallback2(CB2_MoveRelearnerMain);
}

void CB2_InitLearnMoveReturnFromSelectMove(void)
{
    ResetSpriteData();
    FreeAllSpritePalettes();
    ResetTasks();
    sMoveRelearnerMenu = sMoveRelearnerMenuSharedMem;
    CreateLearnableMovesList();
    sMoveRelearnerMenu->forgetMoveIndex = gSpecialVar_0x8005;
    SetVBlankCallback(VBlankCB_MoveRelearner);

    Text_LoadWindowTemplate(&gMoveRelearnerMenuFramesWindowTemplate);
    InitMenuWindow(&gMoveRelearnerMenuFramesWindowTemplate);
    Menu_EraseScreen();

    Text_LoadWindowTemplate(&gMenuTextWindowTemplate);
    InitMenuWindow(&gMenuTextWindowTemplate);
    Menu_EraseScreen();

    REG_DISPCNT = DISPCNT_OBJ_ON | DISPCNT_BG0_ON | DISPCNT_BG1_ON | DISPCNT_OBJ_1D_MAP;
    REG_BG0VOFS = 0;
    REG_BG0HOFS = 0;
    REG_BG1HOFS = 0;
    REG_BG1HOFS = 0;

    LoadSpriteSheet(&sMoveRelearnerUiSpriteSheet);
    LoadSpritePalette(&sMoveRelearnerUiSpritePalette);
    CreateMoveRelearnerUiSprites();
    FillPalette(0, 0, 2);
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
    SetMainCallback2(CB2_MoveRelearnerMain);
}

static void CB2_MoveRelearnerMain(void)
{
    DoMoveRelearnerMain();
    if (sMoveRelearnerMenu->redrawCursor)
    {
        sMoveRelearnerMenu->redrawCursor = FALSE;
        SetBlendedOutlineCursorPosition(0x58, (sMoveRelearnerMenu->cursorRow * 2 + 1) * 8);
    }
    if (sMoveRelearnerMenu->redrawMoveSelectionWindow)
    {
        sMoveRelearnerMenu->redrawMoveSelectionWindow = 0;
        DrawMoveSelectionWindow();
    }
    if (sMoveRelearnerMenu->redrawMoveInfoWindow)
    {
        DrawMoveInfoWindow(sMoveRelearnerMenu->showContestInfo, 1);
        sMoveRelearnerMenu->redrawMoveInfoWindow = FALSE;
    }
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void PrintMessageWithPlaceholders(const u8 *str)
{
    StringExpandPlaceholders(gStringVar4, str);
    MenuPrintMessage(gStringVar4, 3, 15);
}

static void DoMoveRelearnerMain(void)
{
    switch (sMoveRelearnerMenu->state)
    {
    case MENU_STATE_FADE_TO_BLACK:
        sMoveRelearnerMenu->state++;
        DrawLearnMoveMenuWindow();
        DrawBattleMoveInfoHeaders(FALSE);
        DrawMoveSelectionWindow();
        gSprites[MOVE_RELEARNER_SPRITE_RIGHT_ARROW].x = 0x48;
        sMoveRelearnerMenu->redrawMoveInfoWindow = TRUE;
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        REG_DISPCNT = DISPCNT_OBJ_ON | DISPCNT_BG0_ON | DISPCNT_BG1_ON | DISPCNT_OBJ_1D_MAP;
        break;
    case MENU_STATE_WAIT_FOR_FADE:
        if (!gPaletteFade.active)
            sMoveRelearnerMenu->state = MENU_STATE_IDLE_BATTLE_MODE;
        break;
    case MENU_STATE_UNREACHABLE:
        sMoveRelearnerMenu->state++;
        break;
    case MENU_STATE_SETUP_BATTLE_MODE:
        DrawBattleMoveInfoHeaders(FALSE);
        DrawMoveSelectionWindow();
        sMoveRelearnerMenu->redrawMoveInfoWindow = TRUE;
        sMoveRelearnerMenu->state++;
        gSprites[MOVE_RELEARNER_SPRITE_RIGHT_ARROW].x = 0x48;
        break;
    case MENU_STATE_IDLE_BATTLE_MODE:
        if (!ChangeToContestMoveInfoWindow())
            HandleInput();
        return;
    case MENU_STATE_SETUP_CONTEST_MODE:
        DrawContestMoveInfoHeaders(FALSE);
        DrawMoveSelectionWindow();
        sMoveRelearnerMenu->redrawMoveInfoWindow = TRUE;
        gSprites[MOVE_RELEARNER_SPRITE_RIGHT_ARROW].x = 0x48;
        sMoveRelearnerMenu->state++;
        break;
    case MENU_STATE_IDLE_CONTEST_MODE:
        if (!ChangeToBattleMoveInfoWindow())
            HandleInput();
        break;
    case MENU_STATE_PRINT_TEACH_MOVE_PROMPT:
        if (Menu_UpdateWindowText())
        {
            DisplayYesNoMenu(21, 7, 1);
            sMoveRelearnerMenu->state++;
        }
        break;
    case MENU_STATE_TEACH_MOVE_CONFIRM:
        {
            s8 selection = Menu_ProcessInputNoWrap_();
            if (selection == 0)
            {
                RedrawMoveInfoWindow();
                if (GiveMoveToMon(&gPlayerParty[sMoveRelearnerMenu->partyMonIndex], sMoveRelearnerMenu->movesToLearn[sMoveRelearnerMenu->selectedMoveIndex]) != MON_HAS_MAX_MOVES)
                {
                    PrintMessageWithPlaceholders(gOtherText_PokeLearnedMove);
                    gSpecialVar_0x8004 = TRUE;
                    sMoveRelearnerMenu->state = MENU_STATE_PRINT_TEXT_THEN_FANFARE;
                }
                else
                {
                    sMoveRelearnerMenu->state = MENU_STATE_PRINT_TRYING_TO_LEARN_PROMPT;
                }
            }
            else if (selection == -1 || selection == 1)
            {
                RedrawMoveInfoWindow();
                if (sMoveRelearnerMenu->showContestInfo == FALSE)
                    sMoveRelearnerMenu->state = MENU_STATE_SETUP_BATTLE_MODE;
                if (sMoveRelearnerMenu->showContestInfo == TRUE)
                    sMoveRelearnerMenu->state = MENU_STATE_SETUP_CONTEST_MODE;
            }
        }
        break;
    case MENU_STATE_PRINT_GIVE_UP_PROMPT:
        if (Menu_UpdateWindowText())
        {
            DisplayYesNoMenu(21, 7, 1);
            sMoveRelearnerMenu->state++;
        }
        break;
    case MENU_STATE_GIVE_UP_CONFIRM:
        {
            s8 selection = Menu_ProcessInputNoWrap_();

            if (selection == 0)
            {
                RedrawMoveInfoWindow();
                gSpecialVar_0x8004 = FALSE;
                sMoveRelearnerMenu->state = MENU_STATE_FADE_AND_RETURN;
            }
            else if (selection == -1 || selection == 1)
            {
                RedrawMoveInfoWindow();
                if (sMoveRelearnerMenu->showContestInfo == FALSE)
                    sMoveRelearnerMenu->state = MENU_STATE_SETUP_BATTLE_MODE;
                if (sMoveRelearnerMenu->showContestInfo == TRUE)
                    sMoveRelearnerMenu->state = MENU_STATE_SETUP_CONTEST_MODE;
            }
        }
        break;
    case MENU_STATE_PRINT_TRYING_TO_LEARN_PROMPT:
        PrintMessageWithPlaceholders(gOtherText_DeleteOlderMove);
        sMoveRelearnerMenu->state++;
        break;
    case MENU_STATE_WAIT_FOR_TRYING_TO_LEARN:
        if (Menu_UpdateWindowText())
        {
            DisplayYesNoMenu(21, 7, 1);
            sMoveRelearnerMenu->state = MENU_STATE_CONFIRM_DELETE_OLD_MOVE;
        }
        break;
    case MENU_STATE_CONFIRM_DELETE_OLD_MOVE:
        {
            s8 selection = Menu_ProcessInputNoWrap_();

            if (selection == 0)
            {
                RedrawMoveInfoWindow();
                PrintMessageWithPlaceholders(gOtherText_WhichMoveToForget);
                sMoveRelearnerMenu->state = MENU_STATE_PRINT_WHICH_MOVE_PROMPT;
            }
            else if (selection == -1 || selection == 1)
            {
                RedrawMoveInfoWindow();
                sMoveRelearnerMenu->state = MENU_STATE_PRINT_STOP_TEACHING;
            }
        }
        break;
    case MENU_STATE_PRINT_STOP_TEACHING:
        PrintMessageWithPlaceholders(gOtherText_StopLearningMove);
        sMoveRelearnerMenu->state++;
        break;
    case MENU_STATE_WAIT_FOR_STOP_TEACHING:
        if (Menu_UpdateWindowText())
        {
            sMoveRelearnerMenu->state = MENU_STATE_CONFIRM_STOP_TEACHING;
            DisplayYesNoMenu(21, 7, 1);
        }
        break;
    case MENU_STATE_CONFIRM_STOP_TEACHING:
        {
            s8 selection = Menu_ProcessInputNoWrap_();

            if (selection == 0)
            {
                RedrawMoveInfoWindow();
                sMoveRelearnerMenu->state = MENU_STATE_CHOOSE_SETUP_STATE;
            }
            else if (selection == -1 || selection == 1)
            {
                RedrawMoveInfoWindow();

                // This setup-state selection is overwritten immediately below.
                if (sMoveRelearnerMenu->showContestInfo == FALSE)
                    sMoveRelearnerMenu->state = MENU_STATE_SETUP_BATTLE_MODE;
                if (sMoveRelearnerMenu->showContestInfo == TRUE)
                    sMoveRelearnerMenu->state = MENU_STATE_SETUP_CONTEST_MODE;
                sMoveRelearnerMenu->state = MENU_STATE_PRINT_TRYING_TO_LEARN_PROMPT;
            }
        }
        break;
    case MENU_STATE_CHOOSE_SETUP_STATE:
        if (Menu_UpdateWindowText())
        {
            if (sMoveRelearnerMenu->showContestInfo == FALSE)
                sMoveRelearnerMenu->state = MENU_STATE_SETUP_BATTLE_MODE;
            if (sMoveRelearnerMenu->showContestInfo == TRUE)
                sMoveRelearnerMenu->state = MENU_STATE_SETUP_CONTEST_MODE;
        }
        break;
    case MENU_STATE_PRINT_WHICH_MOVE_PROMPT:
        if (Menu_UpdateWindowText())
        {
            sMoveRelearnerMenu->state = MENU_STATE_SHOW_MOVE_SUMMARY_SCREEN;
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        }
        break;
    case MENU_STATE_SHOW_MOVE_SUMMARY_SCREEN:
        if (!gPaletteFade.active)
        {
            ShowSelectMovePokemonSummaryScreen(gPlayerParty, sMoveRelearnerMenu->partyMonIndex, gPlayerPartyCount - 1, CB2_InitLearnMoveReturnFromSelectMove, sMoveRelearnerMenu->movesToLearn[sMoveRelearnerMenu->selectedMoveIndex]);
            sMoveRelearnerMenu->state = MENU_STATE_FADE_FROM_SUMMARY_SCREEN;
        }
        break;
    case MENU_STATE_UNUSED_21:
        if (Menu_UpdateWindowText())
            sMoveRelearnerMenu->state = MENU_STATE_FADE_AND_RETURN;
        break;
    case MENU_STATE_UNUSED_22:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        break;
    case MENU_STATE_FADE_AND_RETURN:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        sMoveRelearnerMenu->state++;
        break;
    case MENU_STATE_RETURN_TO_FIELD:
        if (!gPaletteFade.active)
            SetMainCallback2(CB2_ReturnToField);
        break;
    case MENU_STATE_FADE_FROM_SUMMARY_SCREEN:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        sMoveRelearnerMenu->state++;
        DrawLearnMoveMenuWindow();
        DrawMoveSelectionWindow();
        if (sMoveRelearnerMenu->showContestInfo == FALSE)
            DrawBattleMoveInfoHeaders(TRUE);
        if (sMoveRelearnerMenu->showContestInfo == TRUE)
        {
            gSprites[MOVE_RELEARNER_SPRITE_RIGHT_ARROW].x = 0x48;
            DrawContestMoveInfoHeaders(TRUE);
        }
        DrawMoveInfoWindow(sMoveRelearnerMenu->showContestInfo, 1);
        break;
    case MENU_STATE_TRY_OVERWRITE_MOVE:
        if (!gPaletteFade.active)
        {
            if (sMoveRelearnerMenu->forgetMoveIndex == MAX_MON_MOVES)
            {
                sMoveRelearnerMenu->state = MENU_STATE_PRINT_STOP_TEACHING;
            }
            else
            {
                u16 moveId = GetMonData(&gPlayerParty[sMoveRelearnerMenu->partyMonIndex], MON_DATA_MOVE1 + sMoveRelearnerMenu->forgetMoveIndex);

                StringCopy(gStringVar3, gMoveNames[moveId]);
                RemoveMonPPBonus(&gPlayerParty[sMoveRelearnerMenu->partyMonIndex], sMoveRelearnerMenu->forgetMoveIndex);
                SetMonMoveSlot(&gPlayerParty[sMoveRelearnerMenu->partyMonIndex], sMoveRelearnerMenu->movesToLearn[sMoveRelearnerMenu->selectedMoveIndex], sMoveRelearnerMenu->forgetMoveIndex);
                StringCopy(gStringVar2, gMoveNames[sMoveRelearnerMenu->movesToLearn[sMoveRelearnerMenu->selectedMoveIndex]]);
                PrintMessageWithPlaceholders(gOtherText_ForgotMove123);
                sMoveRelearnerMenu->state = MENU_STATE_DOUBLE_FANFARE_FORGOT_MOVE;
                gSpecialVar_0x8004 = TRUE;
            }
        }
        break;
    case MENU_STATE_DOUBLE_FANFARE_FORGOT_MOVE:
        if (Menu_UpdateWindowText())
        {
#ifdef GERMAN
            PrintMessageWithPlaceholders(deuOtherText_ForgotAndLearned);
#else
            PrintMessageWithPlaceholders(gOtherText_ForgotOrDidNotLearnMove);
#endif
            sMoveRelearnerMenu->state = MENU_STATE_PRINT_TEXT_THEN_FANFARE;
            PlayFanfare(MUS_LEVEL_UP);
        }
        break;
    case MENU_STATE_PRINT_TEXT_THEN_FANFARE:
        if (Menu_UpdateWindowText())
        {
            PlayFanfare(MUS_LEVEL_UP);
            sMoveRelearnerMenu->state = MENU_STATE_WAIT_FOR_FANFARE;
        }
        break;
    case MENU_STATE_WAIT_FOR_FANFARE:
        if (IsFanfareTaskInactive())
            sMoveRelearnerMenu->state = MENU_STATE_WAIT_FOR_A_BUTTON;
        break;
    case MENU_STATE_WAIT_FOR_A_BUTTON:
        if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            sMoveRelearnerMenu->state = MENU_STATE_FADE_AND_RETURN;
        }
        break;
    }
}

static void DrawLearnMoveMenuWindow(void)
{
    u32 i;

    BasicInitMenuWindow(&gMoveRelearnerMenuFramesWindowTemplate);
    for (i = 0; i < 4; i++)
    {
        Menu_DrawStdWindowFrame(
          sMoveRelearnerWindowFrameDimensions[i][0],
          sMoveRelearnerWindowFrameDimensions[i][1],
          sMoveRelearnerWindowFrameDimensions[i][2],
          sMoveRelearnerWindowFrameDimensions[i][3]);
    }
    BasicInitMenuWindow(&gMenuTextWindowTemplate);
}

static void DrawBattleMoveInfoHeaders(bool8 noTeachMoveText)
{
    s32 i;

    gSprites[sMoveRelearnerMenu->spriteIds[MOVE_RELEARNER_SPRITE_LEFT_ARROW]].invisible = FALSE;
    gSprites[sMoveRelearnerMenu->spriteIds[MOVE_RELEARNER_SPRITE_RIGHT_ARROW]].invisible = FALSE;

    for (i = 0; i < MOVE_RELEARNER_HEART_COUNT * 2; i++)
        gSprites[sMoveRelearnerMenu->spriteIds[i + MOVE_RELEARNER_SPRITE_APPEAL_HEARTS]].invisible = TRUE;

    for (i = 0; sMoveRelearnerMoveInfoHeaders[0][i].text != NULL; i++)
    {
        AlignStringInMenuWindow(gTileBuffer, sMoveRelearnerMoveInfoHeaders[0][i].text, 64, 2);
        Menu_PrintText(gTileBuffer, sMoveRelearnerMoveInfoHeaders[0][i].left, sMoveRelearnerMoveInfoHeaders[0][i].right);
    }

    if (!noTeachMoveText)
        Menu_PrintTextWithNewlineIndentAndClear(gOtherText_TeachWhichMove, 24, 120, 192, 32, 1);
}

static u8 ChangeToContestMoveInfoWindow(void)
{
    u32 result = JOY_NEW(DPAD_LEFT) || JOY_NEW(DPAD_RIGHT);

    if (gSaveBlock2.optionsButtonMode == OPTIONS_BUTTON_MODE_LR
     && (JOY_NEW(L_BUTTON) || JOY_NEW(R_BUTTON)))
        result++;

    if (result != 0)
    {
        PlaySE(SE_SELECT);
        sMoveRelearnerMenu->state = MENU_STATE_SETUP_CONTEST_MODE;
        sMoveRelearnerMenu->showContestInfo = TRUE;
    }

    return result;
}

static void DrawContestMoveInfoHeaders(bool8 noTeachMoveText)
{
    s32 i;

    gSprites[sMoveRelearnerMenu->spriteIds[MOVE_RELEARNER_SPRITE_LEFT_ARROW]].invisible = FALSE;
    gSprites[sMoveRelearnerMenu->spriteIds[MOVE_RELEARNER_SPRITE_RIGHT_ARROW]].invisible = FALSE;

    for (i = 0; i < MOVE_RELEARNER_HEART_COUNT * 2; i++)
        gSprites[sMoveRelearnerMenu->spriteIds[i + MOVE_RELEARNER_SPRITE_APPEAL_HEARTS]].invisible = FALSE;

    for (i = 0; sMoveRelearnerMoveInfoHeaders[0][i].text != NULL; i++)
    {
        AlignStringInMenuWindow(gTileBuffer, sMoveRelearnerMoveInfoHeaders[1][i].text, 64, 2);
        Menu_PrintText(gTileBuffer, sMoveRelearnerMoveInfoHeaders[1][i].left, sMoveRelearnerMoveInfoHeaders[1][i].right);
        if (i != 0)
        {
            Menu_EraseWindowRect(
              sMoveRelearnerMoveInfoHeaders[1][i].left,
              sMoveRelearnerMoveInfoHeaders[1][i].right + 2,
              sMoveRelearnerMoveInfoHeaders[1][i].left + 7,
              sMoveRelearnerMoveInfoHeaders[1][i].right + 3);
        }
    }

    if (!noTeachMoveText)
        Menu_PrintTextWithNewlineIndentAndClear(gOtherText_TeachWhichMove, 24, 120, 192, 32, 1);
}

static u8 ChangeToBattleMoveInfoWindow(void)
{
    u32 result = JOY_NEW(DPAD_LEFT) || JOY_NEW(DPAD_RIGHT);

    if (gSaveBlock2.optionsButtonMode == OPTIONS_BUTTON_MODE_LR
     && (JOY_NEW(L_BUTTON) || JOY_NEW(R_BUTTON)))
        result++;

    if (result != 0)
    {
        PlaySE(SE_SELECT);
        sMoveRelearnerMenu->state = MENU_STATE_SETUP_BATTLE_MODE;
        sMoveRelearnerMenu->showContestInfo = FALSE;
    }

    return result;
}

static void ResetMoveRelearnerMenuState(void)
{
    s32 i;

    sMoveRelearnerMenu->state = MENU_STATE_FADE_TO_BLACK;
    sMoveRelearnerMenu->unused2 = 0;
    sMoveRelearnerMenu->scrollOffset = 0;
    sMoveRelearnerMenu->cursorRow = 0;
    sMoveRelearnerMenu->previousCursorRow = 0;
    sMoveRelearnerMenu->numMenuChoices = 0;
    sMoveRelearnerMenu->selectedMoveIndex = 0;
    sMoveRelearnerMenu->redrawCursor = FALSE;
    sMoveRelearnerMenu->redrawMoveSelectionWindow = 0;
    sMoveRelearnerMenu->redrawMoveInfoWindow = FALSE;
    sMoveRelearnerMenu->showContestInfo = FALSE;
    for (i = 0; i < MAX_RELEARNER_MOVES; i++)
        sMoveRelearnerMenu->movesToLearn[i] = 0;
}

#define sUiSpriteMovementType      data[0]
#define sUiSpriteSineIndex         data[1]
#define sUiSpriteMovementDirection data[2]

static void SpriteCB_MoveRelearnerUiSprite(struct Sprite *sprite)
{
    s16 sineIndex = (sprite->sUiSpriteSineIndex * 10) & 0xFF;

    switch (sprite->sUiSpriteMovementType)
    {
    case MOVE_RELEARNER_UI_SPRITE_STATIC:
        break;
    case MOVE_RELEARNER_UI_SPRITE_HORIZONTAL_ARROW:
        sprite->x2 = Sin(sineIndex, 3) * sprite->sUiSpriteMovementDirection;
        break;
    case MOVE_RELEARNER_UI_SPRITE_VERTICAL_ARROW:
        sprite->y2 = Sin(sineIndex, 1) * sprite->sUiSpriteMovementDirection;
        break;
    }
    sprite->sUiSpriteSineIndex++;
}

static void CreateMoveRelearnerUiSprites(void)
{
    s32 i;

    sMoveRelearnerMenu->spriteIds[MOVE_RELEARNER_SPRITE_LEFT_ARROW] = CreateSprite(&sHorizontalArrowSpriteTemplate, 8, 16, 0);
    gSprites[sMoveRelearnerMenu->spriteIds[MOVE_RELEARNER_SPRITE_LEFT_ARROW]].sUiSpriteMovementType = MOVE_RELEARNER_UI_SPRITE_HORIZONTAL_ARROW;
    gSprites[sMoveRelearnerMenu->spriteIds[MOVE_RELEARNER_SPRITE_LEFT_ARROW]].sUiSpriteMovementDirection = -1;

    sMoveRelearnerMenu->spriteIds[MOVE_RELEARNER_SPRITE_RIGHT_ARROW] = CreateSprite(&sHorizontalArrowSpriteTemplate, 72, 16, 0);
    StartSpriteAnim(&gSprites[sMoveRelearnerMenu->spriteIds[MOVE_RELEARNER_SPRITE_RIGHT_ARROW]], MOVE_RELEARNER_HORIZONTAL_ARROW_RIGHT);
    gSprites[sMoveRelearnerMenu->spriteIds[MOVE_RELEARNER_SPRITE_RIGHT_ARROW]].sUiSpriteMovementType = MOVE_RELEARNER_UI_SPRITE_HORIZONTAL_ARROW;
    gSprites[sMoveRelearnerMenu->spriteIds[MOVE_RELEARNER_SPRITE_RIGHT_ARROW]].sUiSpriteMovementDirection = 1;

    sMoveRelearnerMenu->spriteIds[MOVE_RELEARNER_SPRITE_UP_ARROW] = CreateSprite(&sVerticalArrowSpriteTemplate, 160, 4, 0);
    StartSpriteAnim(&gSprites[sMoveRelearnerMenu->spriteIds[MOVE_RELEARNER_SPRITE_UP_ARROW]], MOVE_RELEARNER_VERTICAL_ARROW_UP);
    gSprites[sMoveRelearnerMenu->spriteIds[MOVE_RELEARNER_SPRITE_UP_ARROW]].sUiSpriteMovementType = MOVE_RELEARNER_UI_SPRITE_VERTICAL_ARROW;
    gSprites[sMoveRelearnerMenu->spriteIds[MOVE_RELEARNER_SPRITE_UP_ARROW]].sUiSpriteMovementDirection = -1;

    sMoveRelearnerMenu->spriteIds[MOVE_RELEARNER_SPRITE_DOWN_ARROW] = CreateSprite(&sVerticalArrowSpriteTemplate, 160, 60, 0);
    gSprites[sMoveRelearnerMenu->spriteIds[MOVE_RELEARNER_SPRITE_DOWN_ARROW]].sUiSpriteMovementType = MOVE_RELEARNER_UI_SPRITE_VERTICAL_ARROW;
    gSprites[sMoveRelearnerMenu->spriteIds[MOVE_RELEARNER_SPRITE_DOWN_ARROW]].sUiSpriteMovementDirection = 1;

    for (i = 0; i < MOVE_RELEARNER_HEART_COUNT; i++)
    {
        sMoveRelearnerMenu->spriteIds[i + MOVE_RELEARNER_SPRITE_APPEAL_HEARTS] = CreateSprite(&sHeartSpriteTemplate, (i - (i / 4) * 4) * 8 + 0x1C, (i / 4) * 8 + 0x34, 0);
        StartSpriteAnim(&gSprites[sMoveRelearnerMenu->spriteIds[i + MOVE_RELEARNER_SPRITE_APPEAL_HEARTS]], JAM_HEART_EMPTY);
    }

    for (i = 0; i < MOVE_RELEARNER_HEART_COUNT; i++)
    {
        sMoveRelearnerMenu->spriteIds[i + MOVE_RELEARNER_SPRITE_JAM_HEARTS] = CreateSprite(&sHeartSpriteTemplate, (i - (i / 4) * 4) * 8 + 0x1C, (i / 4) * 8 + 0x5C, 0);
        StartSpriteAnim(&gSprites[sMoveRelearnerMenu->spriteIds[i + MOVE_RELEARNER_SPRITE_JAM_HEARTS]], JAM_HEART_EMPTY);
    }

    for (i = 0; i < MOVE_RELEARNER_UI_SPRITE_COUNT; i++)
        gSprites[sMoveRelearnerMenu->spriteIds[i]].invisible = TRUE;

    CreateBlendedOutlineCursor(16, 0xFFFF, 12, 0x2D9F, 18);
}

#undef sUiSpriteMovementType
#undef sUiSpriteSineIndex
#undef sUiSpriteMovementDirection

static void CreateLearnableMovesList(void)
{
    s32 i;
    u8 nickname[POKEMON_NAME_LENGTH + 1];

    sMoveRelearnerMenu->numMenuChoices = GetMoveRelearnerMoves(&gPlayerParty[sMoveRelearnerMenu->partyMonIndex], sMoveRelearnerMenu->movesToLearn);
    for (i = 0; i < sMoveRelearnerMenu->numMenuChoices; i++)
        StringCopy(sMoveRelearnerMenu->moveNames[i], gMoveNames[sMoveRelearnerMenu->movesToLearn[i]]);
    GetMonData(&gPlayerParty[sMoveRelearnerMenu->partyMonIndex], MON_DATA_NICKNAME, nickname);
    StringCopy_Nickname(gStringVar1, nickname);
    StringCopy(sMoveRelearnerMenu->moveNames[sMoveRelearnerMenu->numMenuChoices], gText_Exit);
    sMoveRelearnerMenu->numMenuChoices++;
}

static void MoveCursorRow(s8 delta)
{
    sMoveRelearnerMenu->previousCursorRow = sMoveRelearnerMenu->cursorRow;
    sMoveRelearnerMenu->cursorRow += delta;
    sMoveRelearnerMenu->redrawCursor = TRUE;
}

static void HandleInput(void)
{
    if (JOY_REPT(DPAD_UP))
    {
        if (sMoveRelearnerMenu->selectedMoveIndex != 0)
        {
            PlaySE(SE_SELECT);
            sMoveRelearnerMenu->selectedMoveIndex--;
            sMoveRelearnerMenu->redrawMoveInfoWindow = TRUE;
            if (sMoveRelearnerMenu->cursorRow != 0)
            {
                MoveCursorRow(-1);
            }
            else if (sMoveRelearnerMenu->scrollOffset != 0)
            {
                sMoveRelearnerMenu->scrollOffset--;
                sMoveRelearnerMenu->redrawMoveSelectionWindow++;
            }
        }
    }
    else if (JOY_REPT(DPAD_DOWN))
    {
        if (sMoveRelearnerMenu->selectedMoveIndex < sMoveRelearnerMenu->numMenuChoices - 1)
        {
            PlaySE(SE_SELECT);
            sMoveRelearnerMenu->selectedMoveIndex++;
            sMoveRelearnerMenu->redrawMoveInfoWindow = TRUE;
            if (sMoveRelearnerMenu->cursorRow != 2)
            {
                MoveCursorRow(1);
            }
            else if (sMoveRelearnerMenu->scrollOffset != sMoveRelearnerMenu->numMenuChoices - 3)
            {
                sMoveRelearnerMenu->scrollOffset++;
                sMoveRelearnerMenu->redrawMoveSelectionWindow++;
            }
        }
    }
    else if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (sMoveRelearnerMenu->selectedMoveIndex != sMoveRelearnerMenu->numMenuChoices - 1)
        {
            sMoveRelearnerMenu->state = MENU_STATE_PRINT_TEACH_MOVE_PROMPT;
            StringCopy(gStringVar2, sMoveRelearnerMenu->moveNames[sMoveRelearnerMenu->selectedMoveIndex]);
            StringExpandPlaceholders(gStringVar4, gOtherText_TeachSpecificMove);
            MenuPrintMessage(gStringVar4, 3, 15);
        }
        else
        {
            StringExpandPlaceholders(gStringVar4, gOtherText_GiveUpTeachingMove);
            MenuPrintMessage(gStringVar4, 3, 15);
            sMoveRelearnerMenu->state = MENU_STATE_PRINT_GIVE_UP_PROMPT;
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        sMoveRelearnerMenu->state = MENU_STATE_PRINT_GIVE_UP_PROMPT;
        StringExpandPlaceholders(gStringVar4, gOtherText_GiveUpTeachingMove);
        MenuPrintMessage(gStringVar4, 3, 15);
    }
    if (sMoveRelearnerMenu->numMenuChoices > 3)
    {
        gSprites[MOVE_RELEARNER_SPRITE_UP_ARROW].invisible = FALSE;
        gSprites[MOVE_RELEARNER_SPRITE_DOWN_ARROW].invisible = FALSE;
        if (sMoveRelearnerMenu->scrollOffset == 0)
            gSprites[MOVE_RELEARNER_SPRITE_UP_ARROW].invisible = TRUE;
        else if (sMoveRelearnerMenu->scrollOffset == sMoveRelearnerMenu->numMenuChoices - 3)
            gSprites[MOVE_RELEARNER_SPRITE_DOWN_ARROW].invisible = TRUE;
    }
}

static void DrawMoveSelectionWindow(void)
{
    u8 listIndex = sMoveRelearnerMenu->scrollOffset;
    u8 *str = gTileBuffer;
    s32 i;

    for (i = 0; i < 3; i++)
    {
        if (listIndex >= sMoveRelearnerMenu->numMenuChoices)
        {
            str = AlignStringInMenuWindow(str, gText_EmptyString, 0x90, 0);
        }
        else if (listIndex == sMoveRelearnerMenu->numMenuChoices - 1)
        {
            str = AlignStringInMenuWindow(str, gText_Exit, 0x90, 0);
        }
        else
        {
            u16 moveId = sMoveRelearnerMenu->movesToLearn[listIndex];

            if (sMoveRelearnerMenu->showContestInfo)
                str = AlignStringInMenuWindow(str, gContestCategoryNames[gContestMoves[moveId].contestCategory], 0x27, 0);
            else
                str = AlignStringInMenuWindow(str, gTypeNames[gBattleMoves[moveId].type], 0x27, 0);

            str = AlignStringInMenuWindow(str, sMoveRelearnerMenu->moveNames[listIndex], 0x72, 0);

            str[0] = CHAR_P;
            str[1] = CHAR_P;
            str[2] = CHAR_SLASH;
            str += 3;

            str = AlignInt1InMenuWindow(str, gBattleMoves[moveId].pp, 0x90, 0);
        }
        *str++ = CHAR_NEWLINE;
        listIndex++;
    }
    *str = EOS;
    Menu_PrintText(gTileBuffer, 11, 1);
    MoveCursorRow(0);
}

static const u8 sMoveInfoTextCoords[7][3] =
{
    {11,  1,  1},
    { 3,  6,  2},
    {24,  1,  3},
    { 3, 11,  4},
    { 5,  4,  5},
    { 3,  6,  6},
    { 3, 11,  7},
};

static const u8 sBattleMoveInfoCoordIds[] = {0, 1, 2, 3};
static const u8 sContestMoveInfoCoordIds[] = {4, 5, 6};

void PrintMoveInfo(u16 moveId, const u8 *moveInfoCoords)
{
    u8 str[0x34];
    u8 numHearts;
    u8 i;

    StringCopy(str, gExpandedPlaceholder_Empty);
    switch (moveInfoCoords[2])
    {
    case 1:
        break;
    case 2:
        if (gBattleMoves[moveId].power < 2)
            AlignStringInMenuWindow(str, gOtherText_ThreeDashes2, 32, 2);
        else
            AlignInt1InMenuWindow(str, gBattleMoves[moveId].power, 32, 2);
        Menu_PrintText(str, moveInfoCoords[0], moveInfoCoords[1]);
        break;
    case 4:
        if (gBattleMoves[moveId].accuracy == 0)
            AlignStringInMenuWindow(str, gOtherText_ThreeDashes2, 32, 2);
        else
            AlignInt1InMenuWindow(str, gBattleMoves[moveId].accuracy, 32, 2);
        Menu_PrintText(str, moveInfoCoords[0], moveInfoCoords[1]);
        break;
    case 6:
        Menu_EraseWindowRect(moveInfoCoords[0], moveInfoCoords[1], moveInfoCoords[0], moveInfoCoords[1] + 1);
        numHearts = gContestEffects[gContestMoves[moveId].effect].appeal / 10;
        if (numHearts == 255)
            numHearts = 0;
        for (i = 0; i < MOVE_RELEARNER_HEART_COUNT; i++)
        {
            if (i < numHearts)
                StartSpriteAnim(&gSprites[sMoveRelearnerMenu->spriteIds[i + MOVE_RELEARNER_SPRITE_APPEAL_HEARTS]], APPEAL_HEART_FULL);
            else
                StartSpriteAnim(&gSprites[sMoveRelearnerMenu->spriteIds[i + MOVE_RELEARNER_SPRITE_APPEAL_HEARTS]], APPEAL_HEART_EMPTY);
        }
        break;
    case 7:
        Menu_EraseWindowRect(moveInfoCoords[0], moveInfoCoords[1], moveInfoCoords[0], moveInfoCoords[1] + 1);
        numHearts = gContestEffects[gContestMoves[moveId].effect].jam / 10;
        if (numHearts == 255)
            numHearts = 0;
        for (i = 0; i < MOVE_RELEARNER_HEART_COUNT; i++)
        {
            if (i < numHearts)
                StartSpriteAnim(&gSprites[sMoveRelearnerMenu->spriteIds[i + MOVE_RELEARNER_SPRITE_JAM_HEARTS]], JAM_HEART_FULL);
            else
                StartSpriteAnim(&gSprites[sMoveRelearnerMenu->spriteIds[i + MOVE_RELEARNER_SPRITE_JAM_HEARTS]], JAM_HEART_EMPTY);
        }
        break;
    }
}

static void DrawMoveInfoWindow(bool8 contestInfo, int unused)
{
    u16 i;

    if (sMoveRelearnerMenu->selectedMoveIndex != sMoveRelearnerMenu->numMenuChoices - 1)
    {
        u16 moveId = sMoveRelearnerMenu->movesToLearn[sMoveRelearnerMenu->selectedMoveIndex];

        if (contestInfo)
        {
            for (i = 0; i < MOVE_RELEARNER_HEART_COUNT * 2; i++)
                gSprites[sMoveRelearnerMenu->spriteIds[i + MOVE_RELEARNER_SPRITE_APPEAL_HEARTS]].invisible = FALSE;
            for (i = 0; i < 3; i++)
                PrintMoveInfo(moveId, sMoveInfoTextCoords[sContestMoveInfoCoordIds[i]]);
            Menu_PrintTextWithNewlineIndentAndClear(gContestEffectStrings[gContestMoves[moveId].effect], 0x58, 0x48, 0x90, 32, 1);
        }
        else
        {
            u8 numDescriptionLines;

            for (i = 0; i < 4; i++)
                PrintMoveInfo(moveId, sMoveInfoTextCoords[sBattleMoveInfoCoordIds[i]]);
            numDescriptionLines = Menu_PrintTextWithNewlineIndent(gMoveDescriptionPointers[moveId - 1], 0x58, 0x48, 0x90, 1);
            if (numDescriptionLines < 2)
            {
                u8 blankWindowTop = numDescriptionLines * 2 + 9;

                Menu_BlankWindowRect(11, blankWindowTop, 28, 12);
            }
        }
    }
    else
    {
        if (contestInfo)
        {
            Menu_EraseWindowRect(sMoveInfoTextCoords[5][0], sMoveInfoTextCoords[5][1], sMoveInfoTextCoords[5][0], sMoveInfoTextCoords[5][1] + 1);
            Menu_EraseWindowRect(sMoveInfoTextCoords[6][0], sMoveInfoTextCoords[6][1], sMoveInfoTextCoords[6][0], sMoveInfoTextCoords[6][1] + 1);
            for (i = 0; i < MOVE_RELEARNER_HEART_COUNT * 2; i++)
                gSprites[sMoveRelearnerMenu->spriteIds[i + MOVE_RELEARNER_SPRITE_APPEAL_HEARTS]].invisible = TRUE;
        }
        else
        {
            Menu_EraseWindowRect(sMoveInfoTextCoords[1][0], sMoveInfoTextCoords[1][1], sMoveInfoTextCoords[1][0] + 3, sMoveInfoTextCoords[1][1] + 1);
            Menu_EraseWindowRect(sMoveInfoTextCoords[3][0], sMoveInfoTextCoords[3][1], sMoveInfoTextCoords[3][0] + 3, sMoveInfoTextCoords[3][1] + 1);
        }
        Menu_EraseWindowRect(11, 9, 28, 12);
    }
}

static void RedrawMoveInfoWindow(void)
{
    Menu_EraseWindowRect(21, 7, 27, 12);
    DrawMoveInfoWindow(sMoveRelearnerMenu->showContestInfo, 0);
}
