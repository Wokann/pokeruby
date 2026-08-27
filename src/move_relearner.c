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
#include "constants/songs.h"

extern u16 gSpecialVar_0x8004;
extern u16 gSpecialVar_0x8005;
extern u8 gTileBuffer[];

extern const struct WindowTemplate gMenuTextWindowTemplate;
extern const struct WindowTemplate gMoveRelearnerMenuFramesWindowTemplate;
extern const u8 *const gContestEffectStrings[];
extern const u8 *const gMoveDescriptions[];
extern const u8 gTypeNames[][7];
extern const u8 *const gContestCategoryNames[];

#ifdef GERMAN
extern const u8 deuOtherText_ForgotAndLearned[];
#endif

static void InitMoveRelearnerMenuWaitFade(u8);
static void CB2_InitMoveRelearnerMenu(void);
static void CB2_MoveRelearnerMenu(void);
static void MoveRelearnerMain(void);
static void DrawLearnMoveMenuWindow(void);
static void DrawBattleMoveInfoHeaders(bool8);
static u8 ChangeToContestMoveInfoWindow(void);
static void DrawContestMoveInfoHeaders(bool8);
static u8 ChangeToBattleMoveInfoWindow(void);
static void ResetMoveRelearnerMenu(void);
static void InitMoveRelearnerMenuSprites(void);
static void InitMoveRelearnerMenuStrings(void);
static void HandleMoveRelearnerMenuInput(void);
static void DrawMoveSelectionWindow(void);
static void DrawMoveInfoWindow(bool8, int);
static void RedrawMoveInfoWindow(void);
static void UpdateMoveRelearnerMenuCursorPosition(struct Sprite *);

struct MoveRelearnerMenu
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

const u16 gMoveRelearnerMenuArrows_Pal[] = INCBIN_U16("graphics/move_relearner/arrows.gbapal");

const u8 gMoveRelearnerMenuArrows_Gfx[] = INCBIN_U8("graphics/move_relearner/arrows.4bpp");

const u8 gMoveRelearnerMenuWindowFrameDimensions[][4] =
{
    { 0,  0,  9, 13},
    {10,  0, 29,  7},
    { 2, 14, 27, 19},
    {10,  8, 29, 13},
};

struct MoveRelearnerMoveInfoHeaders
{
    const u8 *text;
    u8 left;
    u8 right;
    u8 index; // unused
};

const struct MoveRelearnerMoveInfoHeaders gMoveRelearnerMoveInfoHeaders[][4] =
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

// XXX: What are these for?
const u32 unkDataFF00FFEF = 0xFF00FFEF;
const u8 *const gTileBuffer_ = gTileBuffer;

const struct OamData gOamData_8402D50 = {.shape = 0};
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

const struct SpriteSheet gMoveRelearnerMenuArrowsSpriteSheet = {gMoveRelearnerMenuArrows_Gfx, sizeof(gMoveRelearnerMenuArrows_Gfx), 5525};
const struct SpritePalette gMoveRelearnerMenuArrowsPalette = {gMoveRelearnerMenuArrows_Pal, 5526};

const struct SpriteTemplate gSpriteTemplate_8402D90 =
{
    .tileTag = 5525,
    .paletteTag = 5526,
    .oam = &gOamData_8402D58,
    .anims = gSpriteAnimTable_8402D78,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = UpdateMoveRelearnerMenuCursorPosition,
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
    .callback = UpdateMoveRelearnerMenuCursorPosition,
};

const union AnimCmd gSpriteAnim_8402DD8[] =
{
    ANIMCMD_FRAME(8, 5),
    ANIMCMD_END,
};

const union AnimCmd gSpriteAnim_8402DE0[] =
{
    ANIMCMD_FRAME(9, 5),
    ANIMCMD_END,
};

const union AnimCmd gSpriteAnim_8402DE8[] =
{
    ANIMCMD_FRAME(10, 5),
    ANIMCMD_END,
};

const union AnimCmd gSpriteAnim_8402DF0[] =
{
    ANIMCMD_FRAME(11, 5),
    ANIMCMD_END,
};

const union AnimCmd *const gSpriteAnimTable_8402DF8[] =
{
    gSpriteAnim_8402DD8,
    gSpriteAnim_8402DE0,
    gSpriteAnim_8402DE8,
    gSpriteAnim_8402DF0,
};

const struct SpriteTemplate gSpriteTemplate_8402E08 =
{
    .tileTag = 5525,
    .paletteTag = 5526,
    .oam = &gOamData_8402D50,
    .anims = gSpriteAnimTable_8402DF8,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = UpdateMoveRelearnerMenuCursorPosition,
};

const u8 gString_AkitoMori[] = _("あきと");  // programmer Akito Mori?

static void VBlankCB_MoveRelearnerMenu(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

void TeachMoveRelearnerMove(void)
{
    LockPlayerFieldControls();
    CreateTask(InitMoveRelearnerMenuWaitFade, 10);
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
}

static void InitMoveRelearnerMenuWaitFade(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(CB2_InitMoveRelearnerMenu);
        gFieldCallback = sub_8080990;
        DestroyTask(taskId);
    }
}

static void CB2_InitMoveRelearnerMenu(void)
{
    REG_DISPCNT = 0;
    ResetSpriteData();
    FreeAllSpritePalettes();
    ResetTasks();
    sMoveRelearnerMenu = eMoveRelearnerMenu;
    ResetMoveRelearnerMenu();
    sMoveRelearnerMenu->partyMonIndex = gSpecialVar_0x8004;
    InitMoveRelearnerMenuStrings();
    SetVBlankCallback(VBlankCB_MoveRelearnerMenu);

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

    LoadSpriteSheet(&gMoveRelearnerMenuArrowsSpriteSheet);
    LoadSpritePalette(&gMoveRelearnerMenuArrowsPalette);
    InitMoveRelearnerMenuSprites();
    FillPalette(0, 0, 2);
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
    SetMainCallback2(CB2_MoveRelearnerMenu);
}

void CB2_ReturnToMoveRelearnerMenu(void)
{
    ResetSpriteData();
    FreeAllSpritePalettes();
    ResetTasks();
    sMoveRelearnerMenu = eMoveRelearnerMenu;
    InitMoveRelearnerMenuStrings();
    sMoveRelearnerMenu->forgetMoveIndex = gSpecialVar_0x8005;
    SetVBlankCallback(VBlankCB_MoveRelearnerMenu);

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

    LoadSpriteSheet(&gMoveRelearnerMenuArrowsSpriteSheet);
    LoadSpritePalette(&gMoveRelearnerMenuArrowsPalette);
    InitMoveRelearnerMenuSprites();
    FillPalette(0, 0, 2);
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
    SetMainCallback2(CB2_MoveRelearnerMenu);
}

static void CB2_MoveRelearnerMenu(void)
{
    MoveRelearnerMain();
    if (sMoveRelearnerMenu->redrawCursor)
    {
        sMoveRelearnerMenu->redrawCursor = FALSE;
        MenuCursor_SetPos814AD7C(0x58, (sMoveRelearnerMenu->cursorPos * 2 + 1) * 8);
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

static void PrintMainMoveRelearnerMenuText(const u8 *str)
{
    StringExpandPlaceholders(gStringVar4, str);
    MenuPrintMessage(gStringVar4, 3, 15);
}

static void MoveRelearnerMain(void)
{
    switch (sMoveRelearnerMenu->state)
    {
    case 0:
        sMoveRelearnerMenu->state++;
        DrawLearnMoveMenuWindow();
        DrawBattleMoveInfoHeaders(FALSE);
        DrawMoveSelectionWindow();
        gSprites[1].x = 0x48;
        sMoveRelearnerMenu->redrawMoveInfoWindow = TRUE;
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        REG_DISPCNT = DISPCNT_OBJ_ON | DISPCNT_BG0_ON | DISPCNT_BG1_ON | DISPCNT_OBJ_1D_MAP;
        break;
    case 1:
        if (!gPaletteFade.active)
            sMoveRelearnerMenu->state = 4;
        break;
    case 2:
        sMoveRelearnerMenu->state++;
        break;
    case 3:
        DrawBattleMoveInfoHeaders(FALSE);
        DrawMoveSelectionWindow();
        sMoveRelearnerMenu->redrawMoveInfoWindow = TRUE;
        sMoveRelearnerMenu->state++;
        gSprites[1].x = 0x48;
        break;
    case 4:
        if (!ChangeToContestMoveInfoWindow())
            HandleMoveRelearnerMenuInput();
        return;
    case 5:
        DrawContestMoveInfoHeaders(FALSE);
        DrawMoveSelectionWindow();
        sMoveRelearnerMenu->redrawMoveInfoWindow = TRUE;
        gSprites[1].x = 0x48;
        sMoveRelearnerMenu->state++;
        break;
    case 6:
        if (!ChangeToBattleMoveInfoWindow())
            HandleMoveRelearnerMenuInput();
        break;
    case 8:
        if (Menu_UpdateWindowText())
        {
            DisplayYesNoMenu(21, 7, 1);
            sMoveRelearnerMenu->state++;
        }
        break;
    case 9:
        {
            s8 selection = Menu_ProcessInputNoWrap_();
            if (selection == 0)
            {
                RedrawMoveInfoWindow();
                if (GiveMoveToMon(&gPlayerParty[sMoveRelearnerMenu->partyMonIndex], sMoveRelearnerMenu->movesToLearn[sMoveRelearnerMenu->menuSelection]) != 0xFFFF)
                {
                    PrintMainMoveRelearnerMenuText(gOtherText_PokeLearnedMove);
                    gSpecialVar_0x8004 = 1;
                    sMoveRelearnerMenu->state = 31;
                }
                else
                {
                    sMoveRelearnerMenu->state = 16;
                }
            }
            else if (selection == -1 || selection == 1)
            {
                RedrawMoveInfoWindow();
                if (sMoveRelearnerMenu->showContestInfo == FALSE)
                    sMoveRelearnerMenu->state = 3;
                if (sMoveRelearnerMenu->showContestInfo == TRUE)
                    sMoveRelearnerMenu->state = 5;
            }
        }
        break;
    case 12:
        if (Menu_UpdateWindowText())
        {
            DisplayYesNoMenu(21, 7, 1);
            sMoveRelearnerMenu->state++;
        }
        break;
    case 13:
        {
            s8 selection = Menu_ProcessInputNoWrap_();

            if (selection == 0)
            {
                RedrawMoveInfoWindow();
                gSpecialVar_0x8004 = selection;
                sMoveRelearnerMenu->state = 14;
            }
            else if (selection == -1 || selection == 1)
            {
                RedrawMoveInfoWindow();
                if (sMoveRelearnerMenu->showContestInfo == FALSE)
                    sMoveRelearnerMenu->state = 3;
                if (sMoveRelearnerMenu->showContestInfo == TRUE)
                    sMoveRelearnerMenu->state = 5;
            }
        }
        break;
    case 16:
        PrintMainMoveRelearnerMenuText(gOtherText_DeleteOlderMove);
        sMoveRelearnerMenu->state++;
        break;
    case 17:
        if (Menu_UpdateWindowText())
        {
            DisplayYesNoMenu(21, 7, 1);
            sMoveRelearnerMenu->state = 18;
        }
        break;
    case 18:
        {
            s8 var = Menu_ProcessInputNoWrap_();

            if (var == 0)
            {
                RedrawMoveInfoWindow();
                PrintMainMoveRelearnerMenuText(gOtherText_WhichMoveToForget);
                sMoveRelearnerMenu->state = 19;
            }
            else if (var == -1 || var == 1)
            {
                RedrawMoveInfoWindow();
                sMoveRelearnerMenu->state = 24;
            }
        }
        break;
    case 24:
        PrintMainMoveRelearnerMenuText(gOtherText_StopLearningMove);
        sMoveRelearnerMenu->state++;
        break;
    case 25:
        if (Menu_UpdateWindowText())
        {
            sMoveRelearnerMenu->state = 26;
            DisplayYesNoMenu(21, 7, 1);
        }
        break;
    case 26:
        {
            s8 var = Menu_ProcessInputNoWrap_();

            if (var == 0)
            {
                RedrawMoveInfoWindow();
                sMoveRelearnerMenu->state = 27;
            }
            else if (var == -1 || var == 1)
            {
                RedrawMoveInfoWindow();

                // What's the point? It gets set to 16, anyway.
                if (sMoveRelearnerMenu->showContestInfo == FALSE)
                    sMoveRelearnerMenu->state = 3;
                if (sMoveRelearnerMenu->showContestInfo == TRUE)
                    sMoveRelearnerMenu->state = 5;
                sMoveRelearnerMenu->state = 16;
            }
        }
        break;
    case 27:
        if (Menu_UpdateWindowText())
        {
            if (sMoveRelearnerMenu->showContestInfo == FALSE)
                sMoveRelearnerMenu->state = 3;
            if (sMoveRelearnerMenu->showContestInfo == TRUE)
                sMoveRelearnerMenu->state = 5;
        }
        break;
    case 19:
        if (Menu_UpdateWindowText())
        {
            sMoveRelearnerMenu->state = 20;
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        }
        break;
    case 20:
        if (!gPaletteFade.active)
        {
            ShowSelectMovePokemonSummaryScreen(gPlayerParty, sMoveRelearnerMenu->partyMonIndex, gPlayerPartyCount - 1, CB2_ReturnToMoveRelearnerMenu, sMoveRelearnerMenu->movesToLearn[sMoveRelearnerMenu->menuSelection]);
            sMoveRelearnerMenu->state = 28;
        }
        break;
    case 21:
        if (Menu_UpdateWindowText())
            sMoveRelearnerMenu->state = 14;
        break;
    case 22:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        break;
    case 14:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        sMoveRelearnerMenu->state++;
        break;
    case 15:
        if (!gPaletteFade.active)
            SetMainCallback2(CB2_ReturnToField);
        break;
    case 28:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        sMoveRelearnerMenu->state++;
        DrawLearnMoveMenuWindow();
        DrawMoveSelectionWindow();
        if (sMoveRelearnerMenu->showContestInfo == FALSE)
            DrawBattleMoveInfoHeaders(TRUE);
        if (sMoveRelearnerMenu->showContestInfo == TRUE)
        {
            gSprites[1].x = 0x48;
            DrawContestMoveInfoHeaders(TRUE);
        }
        DrawMoveInfoWindow(sMoveRelearnerMenu->showContestInfo, 1);
        break;
    case 29:
        if (!gPaletteFade.active)
        {
            if (sMoveRelearnerMenu->forgetMoveIndex == 4)
            {
                sMoveRelearnerMenu->state = 24;
            }
            else
            {
                u16 moveId = GetMonData(&gPlayerParty[sMoveRelearnerMenu->partyMonIndex], MON_DATA_MOVE1 + sMoveRelearnerMenu->forgetMoveIndex);

                StringCopy(gStringVar3, gMoveNames[moveId]);
                RemoveMonPPBonus(&gPlayerParty[sMoveRelearnerMenu->partyMonIndex], sMoveRelearnerMenu->forgetMoveIndex);
                SetMonMoveSlot(&gPlayerParty[sMoveRelearnerMenu->partyMonIndex], sMoveRelearnerMenu->movesToLearn[sMoveRelearnerMenu->menuSelection], sMoveRelearnerMenu->forgetMoveIndex);
                StringCopy(gStringVar2, gMoveNames[sMoveRelearnerMenu->movesToLearn[sMoveRelearnerMenu->menuSelection]]);
                PrintMainMoveRelearnerMenuText(gOtherText_ForgotMove123);
                sMoveRelearnerMenu->state = 30;
                gSpecialVar_0x8004 = 1;
            }
        }
        break;
    case 30:
        if (Menu_UpdateWindowText())
        {
#ifdef GERMAN
            PrintMainMoveRelearnerMenuText(deuOtherText_ForgotAndLearned);
#else
            PrintMainMoveRelearnerMenuText(gOtherText_ForgotOrDidNotLearnMove);
#endif
            sMoveRelearnerMenu->state = 31;
            PlayFanfare(MUS_LEVEL_UP);
        }
        break;
    case 31:
        if (Menu_UpdateWindowText())
        {
            PlayFanfare(MUS_LEVEL_UP);
            sMoveRelearnerMenu->state = 32;
        }
        break;
    case 32:
        if (IsFanfareTaskInactive())
            sMoveRelearnerMenu->state = 33;
        break;
    case 33:
        if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            sMoveRelearnerMenu->state = 14;
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
          gMoveRelearnerMenuWindowFrameDimensions[i][0],
          gMoveRelearnerMenuWindowFrameDimensions[i][1],
          gMoveRelearnerMenuWindowFrameDimensions[i][2],
          gMoveRelearnerMenuWindowFrameDimensions[i][3]);
    }
    BasicInitMenuWindow(&gMenuTextWindowTemplate);
}

static void DrawBattleMoveInfoHeaders(bool8 noTeachMoveText)
{
    s32 i;

    gSprites[sMoveRelearnerMenu->spriteIDs[0]].invisible = FALSE;
    gSprites[sMoveRelearnerMenu->spriteIDs[1]].invisible = FALSE;

    for (i = 0; i < 16; i++)
        gSprites[sMoveRelearnerMenu->spriteIDs[i + 4]].invisible = TRUE;

    for (i = 0; gMoveRelearnerMoveInfoHeaders[0][i].text != NULL; i++)
    {
        AlignStringInMenuWindow(gTileBuffer, gMoveRelearnerMoveInfoHeaders[0][i].text, 64, 2);
        Menu_PrintText(gTileBuffer, gMoveRelearnerMoveInfoHeaders[0][i].left, gMoveRelearnerMoveInfoHeaders[0][i].right);
    }

    if (!noTeachMoveText)
        sub_8072AB0(gOtherText_TeachWhichMove, 24, 120, 192, 32, 1);
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
        sMoveRelearnerMenu->state = 5;
        sMoveRelearnerMenu->showContestInfo = TRUE;
    }

    return result;
}

static void DrawContestMoveInfoHeaders(bool8 noTeachMoveText)
{
    s32 i;

    gSprites[sMoveRelearnerMenu->spriteIDs[0]].invisible = FALSE;
    gSprites[sMoveRelearnerMenu->spriteIDs[1]].invisible = FALSE;

    for (i = 0; i < 16; i++)
        gSprites[sMoveRelearnerMenu->spriteIDs[i + 4]].invisible = FALSE;

    for (i = 0; gMoveRelearnerMoveInfoHeaders[0][i].text != NULL; i++)
    {
        AlignStringInMenuWindow(gTileBuffer, gMoveRelearnerMoveInfoHeaders[1][i].text, 64, 2);
        Menu_PrintText(gTileBuffer, gMoveRelearnerMoveInfoHeaders[1][i].left, gMoveRelearnerMoveInfoHeaders[1][i].right);
        if (i != 0)
        {
            Menu_EraseWindowRect(
              gMoveRelearnerMoveInfoHeaders[1][i].left,
              gMoveRelearnerMoveInfoHeaders[1][i].right + 2,
              gMoveRelearnerMoveInfoHeaders[1][i].left + 7,
              gMoveRelearnerMoveInfoHeaders[1][i].right + 3);
        }
    }

    if (!noTeachMoveText)
        sub_8072AB0(gOtherText_TeachWhichMove, 24, 120, 192, 32, 1);
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
        sMoveRelearnerMenu->state = 3;
        sMoveRelearnerMenu->showContestInfo = FALSE;
    }

    return result;
}

static void ResetMoveRelearnerMenu(void)
{
    s32 i;

    sMoveRelearnerMenu->state = 0;
    sMoveRelearnerMenu->unk2 = 0;
    sMoveRelearnerMenu->curMenuChoice = 0;
    sMoveRelearnerMenu->cursorPos = 0;
    sMoveRelearnerMenu->previousCursorPos = 0;
    sMoveRelearnerMenu->numMenuChoices = 0;
    sMoveRelearnerMenu->menuSelection = 0;
    sMoveRelearnerMenu->redrawCursor = FALSE;
    sMoveRelearnerMenu->redrawMoveSelectionWindow = 0;
    sMoveRelearnerMenu->redrawMoveInfoWindow = FALSE;
    sMoveRelearnerMenu->showContestInfo = FALSE;
    for (i = 0; i < MAX_RELEARNER_MOVES; i++)
        sMoveRelearnerMenu->movesToLearn[i] = 0;
}

static void UpdateMoveRelearnerMenuCursorPosition(struct Sprite *sprite)
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

static void InitMoveRelearnerMenuSprites(void)
{
    s32 i;

    sMoveRelearnerMenu->spriteIDs[0] = CreateSprite(&gSpriteTemplate_8402D90, 8, 16, 0);
    gSprites[sMoveRelearnerMenu->spriteIDs[0]].data[0] = 1;
    gSprites[sMoveRelearnerMenu->spriteIDs[0]].data[2] = -1;

    sMoveRelearnerMenu->spriteIDs[1] = CreateSprite(&gSpriteTemplate_8402D90, 72, 16, 0);
    StartSpriteAnim(&gSprites[sMoveRelearnerMenu->spriteIDs[1]], 1);
    gSprites[sMoveRelearnerMenu->spriteIDs[1]].data[0] = 1;
    gSprites[sMoveRelearnerMenu->spriteIDs[1]].data[2] = 1;

    sMoveRelearnerMenu->spriteIDs[2] = CreateSprite(&gSpriteTemplate_8402DC0, 160, 4, 0);
    StartSpriteAnim(&gSprites[sMoveRelearnerMenu->spriteIDs[2]], 1);
    gSprites[sMoveRelearnerMenu->spriteIDs[2]].data[0] = 2;
    gSprites[sMoveRelearnerMenu->spriteIDs[2]].data[2] = -1;

    sMoveRelearnerMenu->spriteIDs[3] = CreateSprite(&gSpriteTemplate_8402DC0, 160, 60, 0);
    gSprites[sMoveRelearnerMenu->spriteIDs[3]].data[0] = 2;
    gSprites[sMoveRelearnerMenu->spriteIDs[3]].data[2] = 1;

    for (i = 0; i < 8; i++)
    {
        sMoveRelearnerMenu->spriteIDs[i + 4] = CreateSprite(&gSpriteTemplate_8402E08, (i - (i / 4) * 4) * 8 + 0x1C, (i / 4) * 8 + 0x34, 0);
        StartSpriteAnim(&gSprites[sMoveRelearnerMenu->spriteIDs[i + 4]], 2);
    }

    for (i = 0; i < 8; i++)
    {
        sMoveRelearnerMenu->spriteIDs[i + 12] = CreateSprite(&gSpriteTemplate_8402E08, (i - (i / 4) * 4) * 8 + 0x1C, (i / 4) * 8 + 0x5C, 0);
        StartSpriteAnim(&gSprites[sMoveRelearnerMenu->spriteIDs[i + 12]], 2);
    }

    for (i = 0; i < 20; i++)
        gSprites[sMoveRelearnerMenu->spriteIDs[i]].invisible = TRUE;

    CreateBlendedOutlineCursor(16, 0xFFFF, 12, 0x2D9F, 18);
}

static void InitMoveRelearnerMenuStrings(void)
{
    s32 i;
    u8 nickname[POKEMON_NAME_LENGTH + 1];

    sMoveRelearnerMenu->numMenuChoices = GetMoveRelearnerMoves(&gPlayerParty[sMoveRelearnerMenu->partyMonIndex], sMoveRelearnerMenu->movesToLearn);
    for (i = 0; i < sMoveRelearnerMenu->numMenuChoices; i++)
        StringCopy(sMoveRelearnerMenu->moveNames[i], gMoveNames[sMoveRelearnerMenu->movesToLearn[i]]);
    GetMonData(&gPlayerParty[sMoveRelearnerMenu->partyMonIndex], MON_DATA_NICKNAME, nickname);
    StringCopy10(gStringVar1, nickname);
    StringCopy(sMoveRelearnerMenu->moveNames[sMoveRelearnerMenu->numMenuChoices], gOtherText_Exit);
    sMoveRelearnerMenu->numMenuChoices++;
}

static void MoveCursorPos(s8 delta)
{
    sMoveRelearnerMenu->previousCursorPos = sMoveRelearnerMenu->cursorPos;
    sMoveRelearnerMenu->cursorPos += delta;
    sMoveRelearnerMenu->redrawCursor = TRUE;
}

static void HandleMoveRelearnerMenuInput(void)
{
    if (JOY_REPT(DPAD_UP))
    {
        if (sMoveRelearnerMenu->menuSelection != 0)
        {
            PlaySE(SE_SELECT);
            sMoveRelearnerMenu->menuSelection--;
            sMoveRelearnerMenu->redrawMoveInfoWindow = TRUE;
            if (sMoveRelearnerMenu->cursorPos != 0)
            {
                MoveCursorPos(-1);
            }
            else if (sMoveRelearnerMenu->curMenuChoice != 0)
            {
                sMoveRelearnerMenu->curMenuChoice--;
                sMoveRelearnerMenu->redrawMoveSelectionWindow++;
            }
        }
    }
    else if (JOY_REPT(DPAD_DOWN))
    {
        if (sMoveRelearnerMenu->menuSelection < sMoveRelearnerMenu->numMenuChoices - 1)
        {
            PlaySE(SE_SELECT);
            sMoveRelearnerMenu->menuSelection++;
            sMoveRelearnerMenu->redrawMoveInfoWindow = TRUE;
            if (sMoveRelearnerMenu->cursorPos != 2)
            {
                MoveCursorPos(1);
            }
            else if (sMoveRelearnerMenu->curMenuChoice != sMoveRelearnerMenu->numMenuChoices - 3)
            {
                sMoveRelearnerMenu->curMenuChoice++;
                sMoveRelearnerMenu->redrawMoveSelectionWindow++;
            }
        }
    }
    else if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (sMoveRelearnerMenu->menuSelection != sMoveRelearnerMenu->numMenuChoices - 1)
        {
            sMoveRelearnerMenu->state = 8;
            StringCopy(gStringVar2, sMoveRelearnerMenu->moveNames[sMoveRelearnerMenu->menuSelection]);
            StringExpandPlaceholders(gStringVar4, gOtherText_TeachSpecificMove);
            MenuPrintMessage(gStringVar4, 3, 15);
        }
        else
        {
            StringExpandPlaceholders(gStringVar4, gOtherText_GiveUpTeachingMove);
            MenuPrintMessage(gStringVar4, 3, 15);
            sMoveRelearnerMenu->state = 12;
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        sMoveRelearnerMenu->state = 12;
        StringExpandPlaceholders(gStringVar4, gOtherText_GiveUpTeachingMove);
        MenuPrintMessage(gStringVar4, 3, 15);
    }
    if (sMoveRelearnerMenu->numMenuChoices > 3)
    {
        gSprites[2].invisible = FALSE;
        gSprites[3].invisible = FALSE;
        if (sMoveRelearnerMenu->curMenuChoice == 0)
            gSprites[2].invisible = TRUE;
        else if (sMoveRelearnerMenu->curMenuChoice == sMoveRelearnerMenu->numMenuChoices - 3)
            gSprites[3].invisible = TRUE;
    }
}

static void DrawMoveSelectionWindow(void)
{
    u8 menuChoice = sMoveRelearnerMenu->curMenuChoice;
    u8 *str = gTileBuffer;
    s32 i;

    for (i = 0; i < 3; i++)
    {
        if (menuChoice >= sMoveRelearnerMenu->numMenuChoices)
        {
            str = AlignStringInMenuWindow(str, gEmptyString_81E72B0, 0x90, 0);
        }
        else if (menuChoice == sMoveRelearnerMenu->numMenuChoices - 1)
        {
            str = AlignStringInMenuWindow(str, gOtherText_Exit, 0x90, 0);
        }
        else
        {
            u16 moveId = sMoveRelearnerMenu->movesToLearn[menuChoice];

            if (sMoveRelearnerMenu->showContestInfo)
                str = AlignStringInMenuWindow(str, gContestCategoryNames[gContestMoves[moveId].contestCategory], 0x27, 0);
            else
                str = AlignStringInMenuWindow(str, gTypeNames[gBattleMoves[moveId].type], 0x27, 0);

            str = AlignStringInMenuWindow(str, sMoveRelearnerMenu->moveNames[menuChoice], 0x72, 0);

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
        for (i = 0; i < 8; i++)
        {
            if (i < numHearts)
                StartSpriteAnim(&gSprites[sMoveRelearnerMenu->spriteIDs[i + 4]], 1);
            else
                StartSpriteAnim(&gSprites[sMoveRelearnerMenu->spriteIDs[i + 4]], 0);
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
                StartSpriteAnim(&gSprites[sMoveRelearnerMenu->spriteIDs[i + 12]], 3);
            else
                StartSpriteAnim(&gSprites[sMoveRelearnerMenu->spriteIDs[i + 12]], 2);
        }
        break;
    }
}

static void DrawMoveInfoWindow(bool8 contestInfo, int unused)
{
    u16 i;

    if (sMoveRelearnerMenu->menuSelection != sMoveRelearnerMenu->numMenuChoices - 1)
    {
        u16 moveId = sMoveRelearnerMenu->movesToLearn[sMoveRelearnerMenu->menuSelection];

        if (contestInfo)
        {
            for (i = 0; i < 16; i++)
                gSprites[sMoveRelearnerMenu->spriteIDs[i + 4]].invisible = FALSE;
            for (i = 0; i < 3; i++)
                PrintMoveInfo(moveId, sMoveInfoTextCoords[sContestMoveInfoCoordIds[i]]);
            sub_8072AB0(gContestEffectStrings[gContestMoves[moveId].effect], 0x58, 0x48, 0x90, 32, 1);
        }
        else
        {
            u8 var;

            for (i = 0; i < 4; i++)
                PrintMoveInfo(moveId, sMoveInfoTextCoords[sBattleMoveInfoCoordIds[i]]);
            var = sub_8072A18(gMoveDescriptions[moveId - 1], 0x58, 0x48, 0x90, 1);
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
                gSprites[sMoveRelearnerMenu->spriteIDs[i + 4]].invisible = TRUE;
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
