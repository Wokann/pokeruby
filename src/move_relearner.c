#include "global.h"
#include "contest.h"
#include "contest_effect.h"
#include "data.h"
#include "ewram.h"
#include "field_screen_effect.h"
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
#include "constants/songs.h"

extern u16 gSpecialVar_0x8004;
extern u16 gSpecialVar_0x8005;
extern u8 gTileBuffer[];

extern const struct WindowTemplate gMenuTextWindowTemplate;
extern const struct WindowTemplate gMoveTutorMenuFramesWindowTemplate;
extern const u8 gTypeNames[][7];

static void Task_WaitForFadeOut(u8 taskId);
static void CB2_InitLearnMove(void);
static void CB2_MoveRelearnerMain(void);
static void DoMoveRelearnerMain(void);
static void DrawLearnMoveMenuWindow(void);
static void DrawBattleMoveInfoHeaders(bool8);
static u8 ChangeToContestMoveInfoWindow(void);
static void DrawContestMoveInfoHeaders(bool8);
static u8 ChangeToBattleMoveInfoWindow(void);
static void ResetMoveTutorMenu(void);
static void CreateUISprites(void);
static void CreateLearnableMovesList(void);
static void HandleMoveTutorMenuInput(void);
static void DrawMoveSelectionWindow(void);
static void DrawMoveInfoWindow(bool8, int);
static void RedrawMoveInfoWindow(void);
static void UpdateMoveTutorMenuCursorPosition(struct Sprite *);

struct MoveTutorMenu
{
    u8 state;
    u8 filler1;
    u8 unk2;
    u8 spriteIDs[20];
    u8 filler17;
    u8 cursorPos;
    u8 curMenuChoice;
    u8 numMenuChoices;
    u8 menuSelection;
    u8 previousCursorPos;
    bool8 redrawCursor;
    bool8 redrawMoveSelectionWindow;
    u16 movesToLearn[MAX_LEVEL_UP_MOVES];
    u8 filler48[10];
    u8 moveNames[6][25];
    u8 fillerE8[475];
    bool8 redrawMoveInfoWindow;
    bool8 showContestInfo;
    u8 partyMonIndex;
    u8 forgetMoveIndex;
};

static struct MoveTutorMenu *sMoveRelearnerStruct;

static const u16 sUI_Pal[] = INCBIN_U16("graphics/interface/ui_learn_move.gbapal");

static const u8 sUI_Tiles[] = INCBIN_U8("graphics/interface/ui_learn_move.4bpp");

const u8 gMoveTutorMenuWindowFrameDimensions[][4] =
{
    { 0,  0,  9, 13},
    {10,  0, 29,  7},
    { 2, 14, 27, 19},
    {10,  8, 29, 13},
};

struct MoveTutorMoveInfoHeaders
{
    const u8 *text;
    u8 left;
    u8 right;
    u8 index; // unused
};

const struct MoveTutorMoveInfoHeaders gMoveTutorMoveInfoHeaders[][4] =
{
    {
        {gText_MoveRelearnerBattleMoves,   1, 1, 0},
        {gText_MoveRelearnerPower,    1, 4, 1},
        {gText_MoveRelearnerAccuracy, 1, 9, 2},
        {NULL,               0, 0, 0},
    },
    {
        {gText_MoveRelearnerContestMovesTitle,  1, 1, 0},
        {gText_MoveRelearnerAppeal,   1, 4, 1},
        {gText_MoveRelearnerJam,      1, 9, 2},
        {NULL,               0, 0, 0},
    },
};

// XXX: What are these for?
const u32 unkDataFF00FFEF = 0xFF00FFEF;
const u8 *const gTileBuffer_ = gTileBuffer;

static const struct OamData sHeartSpriteOamData = {.shape = 0};
const struct OamData gOamData_8402D58 = {.shape = 2};
const struct OamData gOamData_8402D60 = {.shape = 1};

const union AnimCmd gSpriteAnim_8402D68[] =
{
    ANIMCMD_FRAME(2, 5),
    ANIMCMD_END,
};

const union AnimCmd gSpriteAnim_8402D70[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END,
};

const union AnimCmd *const gSpriteAnimTable_8402D78[] =
{
    gSpriteAnim_8402D68,
    gSpriteAnim_8402D70,
};

static const struct SpriteSheet sMoveRelearnerSpriteSheet = {sUI_Tiles, sizeof(sUI_Tiles), 5525};
static const struct SpritePalette sMoveRelearnerPalette = {sUI_Pal, 5526};

const struct SpriteTemplate gSpriteTemplate_8402D90 =
{
    .tileTag = 5525,
    .paletteTag = 5526,
    .oam = &gOamData_8402D58,
    .anims = gSpriteAnimTable_8402D78,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = UpdateMoveTutorMenuCursorPosition,
};

const union AnimCmd gSpriteAnim_8402DA8[] =
{
    ANIMCMD_FRAME(4, 5),
    ANIMCMD_END,
};

const union AnimCmd gSpriteAnim_8402DB0[] =
{
    ANIMCMD_FRAME(6, 5),
    ANIMCMD_END,
};

const union AnimCmd *const gSpriteAnimTable_8402DB8[] =
{
    gSpriteAnim_8402DA8,
    gSpriteAnim_8402DB0,
};

const struct SpriteTemplate gSpriteTemplate_8402DC0 =
{
    .tileTag = 5525,
    .paletteTag = 5526,
    .oam = &gOamData_8402D60,
    .anims = gSpriteAnimTable_8402DB8,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = UpdateMoveTutorMenuCursorPosition,
};

static const union AnimCmd sHeartSprite_AppealEmptyFrame[] =
{
    ANIMCMD_FRAME(8, 5),
    ANIMCMD_END,
};

static const union AnimCmd sHeartSprite_AppealFullFrame[] =
{
    ANIMCMD_FRAME(9, 5),
    ANIMCMD_END,
};

static const union AnimCmd sHeartSprite_JamEmptyFrame[] =
{
    ANIMCMD_FRAME(10, 5),
    ANIMCMD_END,
};

static const union AnimCmd sHeartSprite_JamFullFrame[] =
{
    ANIMCMD_FRAME(11, 5),
    ANIMCMD_END,
};

static const union AnimCmd *const sHeartSpriteAnimationCommands[] =
{
    sHeartSprite_AppealEmptyFrame,
    sHeartSprite_AppealFullFrame,
    sHeartSprite_JamEmptyFrame,
    sHeartSprite_JamFullFrame,
};

static const struct SpriteTemplate sConstestMoveHeartSprite =
{
    .tileTag = 5525,
    .paletteTag = 5526,
    .oam = &sHeartSpriteOamData,
    .anims = sHeartSpriteAnimationCommands,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = UpdateMoveTutorMenuCursorPosition,
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
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
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
    sMoveRelearnerStruct = eMoveTutorMenu;
    ResetMoveTutorMenu();
    sMoveRelearnerStruct->partyMonIndex = gSpecialVar_0x8004;
    CreateLearnableMovesList();
    SetVBlankCallback(VBlankCB_MoveRelearner);

    Text_LoadWindowTemplate(&gMoveTutorMenuFramesWindowTemplate);
    InitMenuWindow(&gMoveTutorMenuFramesWindowTemplate);
    Menu_EraseScreen();

    Text_LoadWindowTemplate(&gMenuTextWindowTemplate);
    InitMenuWindow(&gMenuTextWindowTemplate);
    Menu_EraseScreen();

    REG_BG0VOFS = 0;
    REG_BG0VOFS = 0;
    REG_BG1HOFS = 0;
    REG_BG1HOFS = 0;

    LoadSpriteSheet(&sMoveRelearnerSpriteSheet);
    LoadSpritePalette(&sMoveRelearnerPalette);
    CreateUISprites();
    FillPalette(0, 0, 2);
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
    SetMainCallback2(CB2_MoveRelearnerMain);
}

static void CB2_InitLearnMoveReturnFromSelectMove(void)
{
    ResetSpriteData();
    FreeAllSpritePalettes();
    ResetTasks();
    sMoveRelearnerStruct = eMoveTutorMenu;
    CreateLearnableMovesList();
    sMoveRelearnerStruct->forgetMoveIndex = gSpecialVar_0x8005;
    SetVBlankCallback(VBlankCB_MoveRelearner);

    Text_LoadWindowTemplate(&gMoveTutorMenuFramesWindowTemplate);
    InitMenuWindow(&gMoveTutorMenuFramesWindowTemplate);
    Menu_EraseScreen();

    Text_LoadWindowTemplate(&gMenuTextWindowTemplate);
    InitMenuWindow(&gMenuTextWindowTemplate);
    Menu_EraseScreen();

    REG_DISPCNT = DISPCNT_OBJ_ON | DISPCNT_BG0_ON | DISPCNT_BG1_ON | DISPCNT_OBJ_1D_MAP;
    REG_BG0VOFS = 0;
    REG_BG0HOFS = 0;
    REG_BG1HOFS = 0;
    REG_BG1HOFS = 0;

    LoadSpriteSheet(&sMoveRelearnerSpriteSheet);
    LoadSpritePalette(&sMoveRelearnerPalette);
    CreateUISprites();
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
    if (sMoveRelearnerStruct->redrawCursor)
    {
        sMoveRelearnerStruct->redrawCursor = FALSE;
        MenuCursor_SetPos814AD7C(0x58, (sMoveRelearnerStruct->cursorPos * 2 + 1) * 8);
    }
    if (sMoveRelearnerStruct->redrawMoveSelectionWindow)
    {
        sMoveRelearnerStruct->redrawMoveSelectionWindow = 0;
        DrawMoveSelectionWindow();
    }
    if (sMoveRelearnerStruct->redrawMoveInfoWindow)
    {
        DrawMoveInfoWindow(sMoveRelearnerStruct->showContestInfo, 1);
        sMoveRelearnerStruct->redrawMoveInfoWindow = FALSE;
    }
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void PrintMessageWithPlaceholders(const u8 *src)
{
    StringExpandPlaceholders(gStringVar4, src);
    MenuPrintMessage(gStringVar4, 3, 15);
}

static void DoMoveRelearnerMain(void)
{
    switch (sMoveRelearnerStruct->state)
    {
    case 0:
        sMoveRelearnerStruct->state++;
        DrawLearnMoveMenuWindow();
        DrawBattleMoveInfoHeaders(FALSE);
        DrawMoveSelectionWindow();
        gSprites[1].x = 0x48;
        sMoveRelearnerStruct->redrawMoveInfoWindow = TRUE;
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        REG_DISPCNT = DISPCNT_OBJ_ON | DISPCNT_BG0_ON | DISPCNT_BG1_ON | DISPCNT_OBJ_1D_MAP;
        break;
    case 1:
        if (!gPaletteFade.active)
            sMoveRelearnerStruct->state = 4;
        break;
    case 2:
        sMoveRelearnerStruct->state++;
        break;
    case 3:
        DrawBattleMoveInfoHeaders(FALSE);
        DrawMoveSelectionWindow();
        sMoveRelearnerStruct->redrawMoveInfoWindow = TRUE;
        sMoveRelearnerStruct->state++;
        gSprites[1].x = 0x48;
        break;
    case 4:
        if (!ChangeToContestMoveInfoWindow())
            HandleMoveTutorMenuInput();
        return;
    case 5:
        DrawContestMoveInfoHeaders(FALSE);
        DrawMoveSelectionWindow();
        sMoveRelearnerStruct->redrawMoveInfoWindow = TRUE;
        gSprites[1].x = 0x48;
        sMoveRelearnerStruct->state++;
        break;
    case 6:
        if (!ChangeToBattleMoveInfoWindow())
            HandleMoveTutorMenuInput();
        break;
    case 8:
        if (Menu_UpdateWindowText())
        {
            DisplayYesNoMenu(21, 7, 1);
            sMoveRelearnerStruct->state++;
        }
        break;
    case 9:
        {
            s8 selection = Menu_ProcessInputNoWrap_();
            if (selection == 0)
            {
                RedrawMoveInfoWindow();
                if (GiveMoveToMon(&gPlayerParty[sMoveRelearnerStruct->partyMonIndex], sMoveRelearnerStruct->movesToLearn[sMoveRelearnerStruct->menuSelection]) != 0xFFFF)
                {
                    PrintMessageWithPlaceholders(gText_MoveRelearnerPkmnLearnedMove);
                    gSpecialVar_0x8004 = 1;
                    sMoveRelearnerStruct->state = 31;
                }
                else
                {
                    sMoveRelearnerStruct->state = 16;
                }
            }
            else if (selection == -1 || selection == 1)
            {
                RedrawMoveInfoWindow();
                if (sMoveRelearnerStruct->showContestInfo == FALSE)
                    sMoveRelearnerStruct->state = 3;
                if (sMoveRelearnerStruct->showContestInfo == TRUE)
                    sMoveRelearnerStruct->state = 5;
            }
        }
        break;
    case 12:
        if (Menu_UpdateWindowText())
        {
            DisplayYesNoMenu(21, 7, 1);
            sMoveRelearnerStruct->state++;
        }
        break;
    case 13:
        {
            s8 selection = Menu_ProcessInputNoWrap_();

            if (selection == 0)
            {
                RedrawMoveInfoWindow();
                gSpecialVar_0x8004 = selection;
                sMoveRelearnerStruct->state = 14;
            }
            else if (selection == -1 || selection == 1)
            {
                RedrawMoveInfoWindow();
                if (sMoveRelearnerStruct->showContestInfo == FALSE)
                    sMoveRelearnerStruct->state = 3;
                if (sMoveRelearnerStruct->showContestInfo == TRUE)
                    sMoveRelearnerStruct->state = 5;
            }
        }
        break;
    case 16:
        PrintMessageWithPlaceholders(gText_MoveRelearnerPkmnTryingToLearnMove);
        sMoveRelearnerStruct->state++;
        break;
    case 17:
        if (Menu_UpdateWindowText())
        {
            DisplayYesNoMenu(21, 7, 1);
            sMoveRelearnerStruct->state = 18;
        }
        break;
    case 18:
        {
            s8 selection = Menu_ProcessInputNoWrap_();

            if (selection == 0)
            {
                RedrawMoveInfoWindow();
                PrintMessageWithPlaceholders(gText_MoveRelearnerWhichMoveToForget);
                sMoveRelearnerStruct->state = 19;
            }
            else if (selection == -1 || selection == 1)
            {
                RedrawMoveInfoWindow();
                sMoveRelearnerStruct->state = 24;
            }
        }
        break;
    case 24:
        PrintMessageWithPlaceholders(gText_MoveRelearnerStopTryingToTeachMove);
        sMoveRelearnerStruct->state++;
        break;
    case 25:
        if (Menu_UpdateWindowText())
        {
            sMoveRelearnerStruct->state = 26;
            DisplayYesNoMenu(21, 7, 1);
        }
        break;
    case 26:
        {
            s8 selection = Menu_ProcessInputNoWrap_();

            if (selection == 0)
            {
                RedrawMoveInfoWindow();
                sMoveRelearnerStruct->state = 27;
            }
            else if (selection == -1 || selection == 1)
            {
                RedrawMoveInfoWindow();

                // What's the point? It gets set to 16, anyway.
                if (sMoveRelearnerStruct->showContestInfo == FALSE)
                    sMoveRelearnerStruct->state = 3;
                if (sMoveRelearnerStruct->showContestInfo == TRUE)
                    sMoveRelearnerStruct->state = 5;
                sMoveRelearnerStruct->state = 16;
            }
        }
        break;
    case 27:
        if (Menu_UpdateWindowText())
        {
            if (sMoveRelearnerStruct->showContestInfo == FALSE)
                sMoveRelearnerStruct->state = 3;
            if (sMoveRelearnerStruct->showContestInfo == TRUE)
                sMoveRelearnerStruct->state = 5;
        }
        break;
    case 19:
        if (Menu_UpdateWindowText())
        {
            sMoveRelearnerStruct->state = 20;
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        }
        break;
    case 20:
        if (!gPaletteFade.active)
        {
            ShowSelectMovePokemonSummaryScreen(gPlayerParty, sMoveRelearnerStruct->partyMonIndex, gPlayerPartyCount - 1, CB2_InitLearnMoveReturnFromSelectMove, sMoveRelearnerStruct->movesToLearn[sMoveRelearnerStruct->menuSelection]);
            sMoveRelearnerStruct->state = 28;
        }
        break;
    case 21:
        if (Menu_UpdateWindowText())
            sMoveRelearnerStruct->state = 14;
        break;
    case 22:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        break;
    case 14:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        sMoveRelearnerStruct->state++;
        break;
    case 15:
        if (!gPaletteFade.active)
            SetMainCallback2(CB2_ReturnToField);
        break;
    case 28:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        sMoveRelearnerStruct->state++;
        DrawLearnMoveMenuWindow();
        DrawMoveSelectionWindow();
        if (sMoveRelearnerStruct->showContestInfo == FALSE)
            DrawBattleMoveInfoHeaders(TRUE);
        if (sMoveRelearnerStruct->showContestInfo == TRUE)
        {
            gSprites[1].x = 0x48;
            DrawContestMoveInfoHeaders(TRUE);
        }
        DrawMoveInfoWindow(sMoveRelearnerStruct->showContestInfo, 1);
        break;
    case 29:
        if (!gPaletteFade.active)
        {
            if (sMoveRelearnerStruct->forgetMoveIndex == 4)
            {
                sMoveRelearnerStruct->state = 24;
            }
            else
            {
                u16 moveId = GetMonData(&gPlayerParty[sMoveRelearnerStruct->partyMonIndex], MON_DATA_MOVE1 + sMoveRelearnerStruct->forgetMoveIndex);

                StringCopy(gStringVar3, gMoveNames[moveId]);
                RemoveMonPPBonus(&gPlayerParty[sMoveRelearnerStruct->partyMonIndex], sMoveRelearnerStruct->forgetMoveIndex);
                SetMonMoveSlot(&gPlayerParty[sMoveRelearnerStruct->partyMonIndex], sMoveRelearnerStruct->movesToLearn[sMoveRelearnerStruct->menuSelection], sMoveRelearnerStruct->forgetMoveIndex);
                StringCopy(gStringVar2, gMoveNames[sMoveRelearnerStruct->movesToLearn[sMoveRelearnerStruct->menuSelection]]);
                PrintMessageWithPlaceholders(gText_MoveRelearnerAndPoof);
                sMoveRelearnerStruct->state = 30;
                gSpecialVar_0x8004 = 1;
            }
        }
        break;
    case 30:
        if (Menu_UpdateWindowText())
        {
            PrintMessageWithPlaceholders(gText_MoveRelearnerPkmnForgotMoveAndLearnedNew);
            sMoveRelearnerStruct->state = 31;
            PlayFanfare(MUS_LEVEL_UP);
        }
        break;
    case 31:
        if (Menu_UpdateWindowText())
        {
            PlayFanfare(MUS_LEVEL_UP);
            sMoveRelearnerStruct->state = 32;
        }
        break;
    case 32:
        if (IsFanfareTaskInactive())
            sMoveRelearnerStruct->state = 33;
        break;
    case 33:
        if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            sMoveRelearnerStruct->state = 14;
        }
        break;
    }
}

static void DrawLearnMoveMenuWindow(void)
{
    u32 i;

    BasicInitMenuWindow(&gMoveTutorMenuFramesWindowTemplate);
    for (i = 0; i < 4; i++)
    {
        Menu_DrawStdWindowFrame(
          gMoveTutorMenuWindowFrameDimensions[i][0],
          gMoveTutorMenuWindowFrameDimensions[i][1],
          gMoveTutorMenuWindowFrameDimensions[i][2],
          gMoveTutorMenuWindowFrameDimensions[i][3]);
    }
    BasicInitMenuWindow(&gMenuTextWindowTemplate);
}

static void DrawBattleMoveInfoHeaders(bool8 noTeachMoveText)
{
    s32 i;

    gSprites[sMoveRelearnerStruct->spriteIDs[0]].invisible = FALSE;
    gSprites[sMoveRelearnerStruct->spriteIDs[1]].invisible = FALSE;

    for (i = 0; i < 16; i++)
        gSprites[sMoveRelearnerStruct->spriteIDs[i + 4]].invisible = TRUE;

    for (i = 0; gMoveTutorMoveInfoHeaders[0][i].text != NULL; i++)
    {
        AlignStringInMenuWindow(gTileBuffer, gMoveTutorMoveInfoHeaders[0][i].text, 64, 2);
        Menu_PrintText(gTileBuffer, gMoveTutorMoveInfoHeaders[0][i].left, gMoveTutorMoveInfoHeaders[0][i].right);
    }

    if (!noTeachMoveText)
        sub_8072AB0(gText_TeachWhichMoveToPkmn, 24, 120, 192, 32, 1);
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
        sMoveRelearnerStruct->state = 5;
        sMoveRelearnerStruct->showContestInfo = TRUE;
    }

    return result;
}

static void DrawContestMoveInfoHeaders(bool8 noTeachMoveText)
{
    s32 i;

    gSprites[sMoveRelearnerStruct->spriteIDs[0]].invisible = FALSE;
    gSprites[sMoveRelearnerStruct->spriteIDs[1]].invisible = FALSE;

    for (i = 0; i < 16; i++)
        gSprites[sMoveRelearnerStruct->spriteIDs[i + 4]].invisible = FALSE;

    for (i = 0; gMoveTutorMoveInfoHeaders[0][i].text != NULL; i++)
    {
        AlignStringInMenuWindow(gTileBuffer, gMoveTutorMoveInfoHeaders[1][i].text, 64, 2);
        Menu_PrintText(gTileBuffer, gMoveTutorMoveInfoHeaders[1][i].left, gMoveTutorMoveInfoHeaders[1][i].right);
        if (i != 0)
        {
            Menu_EraseWindowRect(
              gMoveTutorMoveInfoHeaders[1][i].left,
              gMoveTutorMoveInfoHeaders[1][i].right + 2,
              gMoveTutorMoveInfoHeaders[1][i].left + 7,
              gMoveTutorMoveInfoHeaders[1][i].right + 3);
        }
    }

    if (!noTeachMoveText)
        sub_8072AB0(gText_TeachWhichMoveToPkmn, 24, 120, 192, 32, 1);
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
        sMoveRelearnerStruct->state = 3;
        sMoveRelearnerStruct->showContestInfo = FALSE;
    }

    return result;
}

static void ResetMoveTutorMenu(void)
{
    s32 i;

    sMoveRelearnerStruct->state = 0;
    sMoveRelearnerStruct->unk2 = 0;
    sMoveRelearnerStruct->curMenuChoice = 0;
    sMoveRelearnerStruct->cursorPos = 0;
    sMoveRelearnerStruct->previousCursorPos = 0;
    sMoveRelearnerStruct->numMenuChoices = 0;
    sMoveRelearnerStruct->menuSelection = 0;
    sMoveRelearnerStruct->redrawCursor = FALSE;
    sMoveRelearnerStruct->redrawMoveSelectionWindow = 0;
    sMoveRelearnerStruct->redrawMoveInfoWindow = FALSE;
    sMoveRelearnerStruct->showContestInfo = FALSE;
    for (i = 0; i < MAX_LEVEL_UP_MOVES; i++)
        sMoveRelearnerStruct->movesToLearn[i] = 0;
}

static void UpdateMoveTutorMenuCursorPosition(struct Sprite *sprite)
{
    s16 var = (sprite->data[1] * 10) & 0xFF;

    switch (sprite->data[0])
    {
    case 0:
        break;
    case 1:
        sprite->x2 = Sin(var, 3) * sprite->data[2];
        break;
    case 2:
        sprite->y2 = Sin(var, 1) * sprite->data[2];
        break;
    }
    sprite->data[1]++;
}

static void CreateUISprites(void)
{
    s32 i;

    sMoveRelearnerStruct->spriteIDs[0] = CreateSprite(&gSpriteTemplate_8402D90, 8, 16, 0);
    gSprites[sMoveRelearnerStruct->spriteIDs[0]].data[0] = 1;
    gSprites[sMoveRelearnerStruct->spriteIDs[0]].data[2] = -1;

    sMoveRelearnerStruct->spriteIDs[1] = CreateSprite(&gSpriteTemplate_8402D90, 72, 16, 0);
    StartSpriteAnim(&gSprites[sMoveRelearnerStruct->spriteIDs[1]], 1);
    gSprites[sMoveRelearnerStruct->spriteIDs[1]].data[0] = 1;
    gSprites[sMoveRelearnerStruct->spriteIDs[1]].data[2] = 1;

    sMoveRelearnerStruct->spriteIDs[2] = CreateSprite(&gSpriteTemplate_8402DC0, 160, 4, 0);
    StartSpriteAnim(&gSprites[sMoveRelearnerStruct->spriteIDs[2]], 1);
    gSprites[sMoveRelearnerStruct->spriteIDs[2]].data[0] = 2;
    gSprites[sMoveRelearnerStruct->spriteIDs[2]].data[2] = -1;

    sMoveRelearnerStruct->spriteIDs[3] = CreateSprite(&gSpriteTemplate_8402DC0, 160, 60, 0);
    gSprites[sMoveRelearnerStruct->spriteIDs[3]].data[0] = 2;
    gSprites[sMoveRelearnerStruct->spriteIDs[3]].data[2] = 1;

    for (i = 0; i < 8; i++)
    {
        sMoveRelearnerStruct->spriteIDs[i + 4] = CreateSprite(&sConstestMoveHeartSprite, (i - (i / 4) * 4) * 8 + 0x1C, (i / 4) * 8 + 0x34, 0);
        StartSpriteAnim(&gSprites[sMoveRelearnerStruct->spriteIDs[i + 4]], 2);
    }

    for (i = 0; i < 8; i++)
    {
        sMoveRelearnerStruct->spriteIDs[i + 12] = CreateSprite(&sConstestMoveHeartSprite, (i - (i / 4) * 4) * 8 + 0x1C, (i / 4) * 8 + 0x5C, 0);
        StartSpriteAnim(&gSprites[sMoveRelearnerStruct->spriteIDs[i + 12]], 2);
    }

    for (i = 0; i < 20; i++)
        gSprites[sMoveRelearnerStruct->spriteIDs[i]].invisible = TRUE;

    CreateBlendedOutlineCursor(16, 0xFFFF, 12, 0x2D9F, 18);
}

static void CreateLearnableMovesList(void)
{
    s32 i;
    u8 nickname[POKEMON_NAME_LENGTH + 1];

    sMoveRelearnerStruct->numMenuChoices = GetMoveRelearnerMoves(&gPlayerParty[sMoveRelearnerStruct->partyMonIndex], sMoveRelearnerStruct->movesToLearn);
    for (i = 0; i < sMoveRelearnerStruct->numMenuChoices; i++)
        StringCopy(sMoveRelearnerStruct->moveNames[i], gMoveNames[sMoveRelearnerStruct->movesToLearn[i]]);
    GetMonData(&gPlayerParty[sMoveRelearnerStruct->partyMonIndex], MON_DATA_NICKNAME, nickname);
    StringCopy_Nickname(gStringVar1, nickname);
    StringCopy(sMoveRelearnerStruct->moveNames[sMoveRelearnerStruct->numMenuChoices], gText_Exit);
    sMoveRelearnerStruct->numMenuChoices++;
}

static void MoveCursorPos(s8 delta)
{
    sMoveRelearnerStruct->previousCursorPos = sMoveRelearnerStruct->cursorPos;
    sMoveRelearnerStruct->cursorPos += delta;
    sMoveRelearnerStruct->redrawCursor = TRUE;
}

static void HandleMoveTutorMenuInput(void)
{
    if (JOY_REPEAT(DPAD_UP))
    {
        if (sMoveRelearnerStruct->menuSelection != 0)
        {
            PlaySE(SE_SELECT);
            sMoveRelearnerStruct->menuSelection--;
            sMoveRelearnerStruct->redrawMoveInfoWindow = TRUE;
            if (sMoveRelearnerStruct->cursorPos != 0)
            {
                MoveCursorPos(-1);
            }
            else if (sMoveRelearnerStruct->curMenuChoice != 0)
            {
                sMoveRelearnerStruct->curMenuChoice--;
                sMoveRelearnerStruct->redrawMoveSelectionWindow++;
            }
        }
    }
    else if (JOY_REPEAT(DPAD_DOWN))
    {
        if (sMoveRelearnerStruct->menuSelection < sMoveRelearnerStruct->numMenuChoices - 1)
        {
            PlaySE(SE_SELECT);
            sMoveRelearnerStruct->menuSelection++;
            sMoveRelearnerStruct->redrawMoveInfoWindow = TRUE;
            if (sMoveRelearnerStruct->cursorPos != 2)
            {
                MoveCursorPos(1);
            }
            else if (sMoveRelearnerStruct->curMenuChoice != sMoveRelearnerStruct->numMenuChoices - 3)
            {
                sMoveRelearnerStruct->curMenuChoice++;
                sMoveRelearnerStruct->redrawMoveSelectionWindow++;
            }
        }
    }
    else if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (sMoveRelearnerStruct->menuSelection != sMoveRelearnerStruct->numMenuChoices - 1)
        {
            sMoveRelearnerStruct->state = 8;
            StringCopy(gStringVar2, sMoveRelearnerStruct->moveNames[sMoveRelearnerStruct->menuSelection]);
            StringExpandPlaceholders(gStringVar4, gText_MoveRelearnerTeachMoveConfirm);
            MenuPrintMessage(gStringVar4, 3, 15);
        }
        else
        {
            StringExpandPlaceholders(gStringVar4, gText_MoveRelearnerGiveUp);
            MenuPrintMessage(gStringVar4, 3, 15);
            sMoveRelearnerStruct->state = 12;
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        sMoveRelearnerStruct->state = 12;
        StringExpandPlaceholders(gStringVar4, gText_MoveRelearnerGiveUp);
        MenuPrintMessage(gStringVar4, 3, 15);
    }
    if (sMoveRelearnerStruct->numMenuChoices > 3)
    {
        gSprites[2].invisible = FALSE;
        gSprites[3].invisible = FALSE;
        if (sMoveRelearnerStruct->curMenuChoice == 0)
            gSprites[2].invisible = TRUE;
        else if (sMoveRelearnerStruct->curMenuChoice == sMoveRelearnerStruct->numMenuChoices - 3)
            gSprites[3].invisible = TRUE;
    }
}

static void DrawMoveSelectionWindow(void)
{
    u8 menuChoice = sMoveRelearnerStruct->curMenuChoice;
    u8 *str = gTileBuffer;
    s32 i;

    for (i = 0; i < 3; i++)
    {
        if (menuChoice >= sMoveRelearnerStruct->numMenuChoices)
        {
            str = AlignStringInMenuWindow(str, gEmptyString_81E72B0, 0x90, 0);
        }
        else if (menuChoice == sMoveRelearnerStruct->numMenuChoices - 1)
        {
            str = AlignStringInMenuWindow(str, gText_Exit, 0x90, 0);
        }
        else
        {
            u16 moveId = sMoveRelearnerStruct->movesToLearn[menuChoice];

            if (sMoveRelearnerStruct->showContestInfo)
                str = AlignStringInMenuWindow(str, gContestMoveTypeTextPointers[gContestMoves[moveId].contestCategory], 0x27, 0);
            else
                str = AlignStringInMenuWindow(str, gTypeNames[gBattleMoves[moveId].type], 0x27, 0);

            str = AlignStringInMenuWindow(str, sMoveRelearnerStruct->moveNames[menuChoice], 0x72, 0);

            str[0] = CHAR_P;
            str[1] = CHAR_P;
            str[2] = CHAR_SLASH;
            str += 3;

            str = AlignInt1InMenuWindow(str, gBattleMoves[moveId].pp, 0x90, 0);
        }
        *str++ = CHAR_NEWLINE;
        menuChoice++;
    }
    *str = EOS;
    Menu_PrintText(gTileBuffer, 11, 1);
    MoveCursorPos(0);
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

    StringCopy(str, gText_ExpandedPlaceholder_Empty);
    switch (moveInfoCoords[2])
    {
    case 1:
        break;
    case 2:
        if (gBattleMoves[moveId].power < 2)
            AlignStringInMenuWindow(str, gText_ThreeDashes, 32, 2);
        else
            AlignInt1InMenuWindow(str, gBattleMoves[moveId].power, 32, 2);
        Menu_PrintText(str, moveInfoCoords[0], moveInfoCoords[1]);
        break;
    case 4:
        if (gBattleMoves[moveId].accuracy == 0)
            AlignStringInMenuWindow(str, gText_ThreeDashes, 32, 2);
        else
            AlignInt1InMenuWindow(str, gBattleMoves[moveId].accuracy, 32, 2);
        Menu_PrintText(str, moveInfoCoords[0], moveInfoCoords[1]);
        break;
    case 6:
        Menu_EraseWindowRect(moveInfoCoords[0], moveInfoCoords[1], moveInfoCoords[0], moveInfoCoords[1] + 1);
        numHearts = gContestEffects[gContestMoves[moveId].effect].appeal / 10;
        if (numHearts == 255)
            numHearts = 0;
        for (i = 0; i < 8; i++)
        {
            if (i < numHearts)
                StartSpriteAnim(&gSprites[sMoveRelearnerStruct->spriteIDs[i + 4]], 1);
            else
                StartSpriteAnim(&gSprites[sMoveRelearnerStruct->spriteIDs[i + 4]], 0);
        }
        break;
    case 7:
        Menu_EraseWindowRect(moveInfoCoords[0], moveInfoCoords[1], moveInfoCoords[0], moveInfoCoords[1] + 1);
        numHearts = gContestEffects[gContestMoves[moveId].effect].jam / 10;
        if (numHearts == 255)
            numHearts = 0;
        for (i = 0; i < 8; i++)
        {
            if (i < numHearts)
                StartSpriteAnim(&gSprites[sMoveRelearnerStruct->spriteIDs[i + 12]], 3);
            else
                StartSpriteAnim(&gSprites[sMoveRelearnerStruct->spriteIDs[i + 12]], 2);
        }
        break;
    }
}

static void DrawMoveInfoWindow(bool8 contestInfo, int unused)
{
    u16 i;

    if (sMoveRelearnerStruct->menuSelection != sMoveRelearnerStruct->numMenuChoices - 1)
    {
        u16 moveId = sMoveRelearnerStruct->movesToLearn[sMoveRelearnerStruct->menuSelection];

        if (contestInfo)
        {
            for (i = 0; i < 16; i++)
                gSprites[sMoveRelearnerStruct->spriteIDs[i + 4]].invisible = FALSE;
            for (i = 0; i < 3; i++)
                PrintMoveInfo(moveId, sMoveInfoTextCoords[sContestMoveInfoCoordIds[i]]);
            sub_8072AB0(gContestEffectDescriptionPointers[gContestMoves[moveId].effect], 0x58, 0x48, 0x90, 32, 1);
        }
        else
        {
            u8 var;

            for (i = 0; i < 4; i++)
                PrintMoveInfo(moveId, sMoveInfoTextCoords[sBattleMoveInfoCoordIds[i]]);
            var = sub_8072A18(gMoveDescriptionPointers[moveId - 1], 0x58, 0x48, 0x90, 1);
            if (var < 2)
            {
                u8 r1 = var * 2 + 9;

                Menu_BlankWindowRect(11, r1, 28, 12);
            }
        }
    }
    else
    {
        if (contestInfo)
        {
            Menu_EraseWindowRect(sMoveInfoTextCoords[5][0], sMoveInfoTextCoords[5][1], sMoveInfoTextCoords[5][0], sMoveInfoTextCoords[5][1] + 1);
            Menu_EraseWindowRect(sMoveInfoTextCoords[6][0], sMoveInfoTextCoords[6][1], sMoveInfoTextCoords[6][0], sMoveInfoTextCoords[6][1] + 1);
            for (i = 0; i < 16; i++)
                gSprites[sMoveRelearnerStruct->spriteIDs[i + 4]].invisible = TRUE;
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
    DrawMoveInfoWindow(sMoveRelearnerStruct->showContestInfo, 0);
}
