#include "global.h"
#include "matsuda_debug_menu.h"
#include "battle.h"
#include "contest.h"
#include "contest_link.h"
#include "contest_link_util.h"
#include "data2.h"
#include "ewram.h"
#include "link.h"
#include "main.h"
#include "menu.h"
#include "overworld.h"
#include "palette.h"
#include "scanline_effect.h"
#include "sprite.h"
#include "start_menu.h"
#include "string_util.h"
#include "task.h"
#include "text.h"

extern u8 gContestDebugMode;
extern u8 gContestMonPartyIndex;
extern u16 gSpecialVar_ContestCategory;
extern u16 gSpecialVar_ContestRank;

extern u8 (*gMenuCallback)(void);

static bool8 MatsudaDebugMenu_ProcessInput(void);
static void Task_MatsudaDebugMenu_ShowContestResults(u8 taskId);
static void Task_MatsudaDebugMenu_LinkTest_CreatePlayerMon(u8);
static void Task_MatsudaDebugMenu_LinkTest_CommunicateRng(u8);
static void Task_MatsudaDebugMenu_LinkTest_CommunicateLeaderIds(u8);
static void Task_MatsudaDebugMenu_LinkTest_ProcessLeaderIds(u8);
static void Task_MatsudaDebugMenu_LinkTest_CommunicateTurnOrder(u8);
static void Task_MatsudaDebugMenu_LinkTest_Finish(u8);
static void Task_MatsudaDebugMenu_LinkTest_CheckCancel(u8 taskId);
static void Task_MatsudaDebugMenu_LinkTest_OpenLink(u8 taskId);
static void Task_MatsudaDebugMenu_LinkTest_WaitForPlayers(u8);
static void Task_MatsudaDebugMenu_LinkTest_AdvanceMasterState(u8);
static void Task_MatsudaDebugMenu_LinkTest_WaitForRemotePlayers(u8);
static void InitMatsudaDebugMenuContest(void);
static void PrintMatsudaDebugContestCategory(u8);
static void PrintMatsudaDebugContestantRound1Points(u8, u8);
static void StartMatsudaDebugContest(struct Sprite *, s8);

// unused
const u8 gText_MatsudaDebugMenuTest[] = DTR("テストです", "Test");
const u8 gMatsudaDebugMenuActionContestText[] = _("Contest");
const u8 gMatsudaDebugMenuActionContestResultsText[] = _("Contest results");
// unused
const u8 gMatsudaDebugMenuActionChangeParameterText[] = DTR("パラメータへんこう", "Change parameter");
// unused
const u8 gMatsudaDebugMenuActionChangeMoveText[] = DTR("わざ へんこう", "Change move"); // "Change move"
const u8 gMatsudaDebugMenuActionContestCommText[] = _("Contest (comm.)");
const u8 gMatsudaDebugMenuActionCommTestText[] = _("Init comm. data");
const u8 gMatsudaDebugMenuActionSetHighScoreText[] = _("Set highest score");
const u8 gMatsudaDebugMenuActionResetHighScoreText[] = _("Reset highest score");
const u8 gMatsudaDebugMenuActionSetAllArtMuseumText[] = _("Set all art museum items");

const struct MenuAction gMatsudaDebugMenuActions[] =
{
    {gMatsudaDebugMenuActionContestText, MatsudaDebugMenu_Contest},
    {gMatsudaDebugMenuActionContestResultsText, MatsudaDebugMenu_ContestResults},
    {gMatsudaDebugMenuActionContestCommText, MatsudaDebugMenu_ContestComm},
    {gMatsudaDebugMenuActionCommTestText, MatsudaDebugMenu_CommTest},
    {gMatsudaDebugMenuActionSetHighScoreText, MatsudaDebugMenu_SetHighScore},
    {gMatsudaDebugMenuActionResetHighScoreText, MatsudaDebugMenu_ResetHighScore},
    {gMatsudaDebugMenuActionSetAllArtMuseumText, MatsudaDebugMenu_SetArtMuseumItems},
};

u8 InitMatsudaDebugMenu(void)
{
    Menu_EraseScreen();
    Menu_DrawStdWindowFrame(0, 0, 17, 18);
    Menu_PrintItems(1, 1, 7, gMatsudaDebugMenuActions);
    InitMenu(0, 1, 1, 7, 0, 16);
    gMenuCallback = MatsudaDebugMenu_ProcessInput;
    return 0;
}

static bool8 MatsudaDebugMenu_ProcessInput(void)
{
    s8 choice = Menu_ProcessInput();

    switch (choice)
    {
    case -2:
        return FALSE;
    default:
        gMenuCallback = gMatsudaDebugMenuActions[choice].func;
        return FALSE;
    case -1:
        CloseMenu();
        return TRUE;
    }
}

u8 MatsudaDebugMenu_ContestResults(void)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    CreateTask(Task_MatsudaDebugMenu_ShowContestResults, 0xFF);
    return 1;
}

static void Task_MatsudaDebugMenu_ShowContestResults(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        DestroyTask(taskId);

        if (!(gIsLinkContest & 1))
            CalculateFinalScores();

        CB2_StartShowContestResults();
    }
}

u8 MatsudaDebugMenu_Contest(void)
{
    gIsLinkContest = 0;
    InitMatsudaDebugMenuContest();
    return 0;
}

u8 MatsudaDebugMenu_ContestComm(void)
{
    InitMatsudaDebugMenuContest();
    return 0;
}

u8 MatsudaDebugMenu_CommTest(void)
{
    u8 linkTaskId;
    u8 cancelTaskId;

    linkTaskId = CreateTask(Task_MatsudaDebugMenu_LinkTest_OpenLink, 0);
    SetTaskFuncWithFollowupFunc(linkTaskId, Task_MatsudaDebugMenu_LinkTest_OpenLink, Task_MatsudaDebugMenu_LinkTest_CreatePlayerMon);
    cancelTaskId = CreateTask(Task_MatsudaDebugMenu_LinkTest_CheckCancel, 1);
    gTasks[cancelTaskId].data[10] = linkTaskId;
    gTasks[linkTaskId].data[10] = cancelTaskId;
    return 1;
}

static void Task_MatsudaDebugMenu_LinkTest_CreatePlayerMon(u8 taskId)
{
    Contest_CreatePlayerMon(0);
    SetTaskFuncWithFollowupFunc(taskId, Task_LinkContest_CommunicateMonsRS, Task_MatsudaDebugMenu_LinkTest_CommunicateRng);
}

static void Task_MatsudaDebugMenu_LinkTest_CommunicateRng(u8 taskId)
{
    SetTaskFuncWithFollowupFunc(taskId, Task_LinkContest_CommunicateRngRS, Task_MatsudaDebugMenu_LinkTest_CommunicateLeaderIds);
}

static void Task_MatsudaDebugMenu_LinkTest_CommunicateLeaderIds(u8 taskId)
{
    SetTaskFuncWithFollowupFunc(taskId, Task_LinkContest_CommunicateLeaderIdsRS, Task_MatsudaDebugMenu_LinkTest_ProcessLeaderIds);
}

static void Task_MatsudaDebugMenu_LinkTest_WaitForDisconnect(u8 taskId)
{
    if (gReceivedRemoteLinkPlayers == FALSE)
    {
        DestroyTask(gTasks[taskId].data[10]);
        DestroyTask(taskId);
        CloseMenu();
    }
}

static void Task_MatsudaDebugMenu_LinkTest_CloseLink(u8 taskId)
{
    SetCloseLinkCallback();
    gTasks[taskId].func = Task_MatsudaDebugMenu_LinkTest_WaitForDisconnect;
}

static void Task_MatsudaDebugMenu_LinkTest_ProcessLeaderIds(u8 taskId)
{
    int i;
    u8 leaderIds[4];

    for (i = 0; i < 4; i++)
        leaderIds[i] = gTasks[taskId].data[5 + i];

    gContestLinkLeaderIndex = LinkContest_GetLeaderIndex(leaderIds);
    CalculateRound1Points((u8)gSpecialVar_ContestCategory);
    SortContestants(0);
    SetTaskFuncWithFollowupFunc(taskId, Task_LinkContest_CommunicateRound1Points, Task_MatsudaDebugMenu_LinkTest_CommunicateTurnOrder);
}

static void Task_MatsudaDebugMenu_LinkTest_CommunicateTurnOrder(u8 taskId)
{
    SetTaskFuncWithFollowupFunc(taskId, Task_LinkContest_CommunicateTurnOrder, Task_MatsudaDebugMenu_LinkTest_Finish);
}

static void Task_MatsudaDebugMenu_LinkTest_Finish(u8 taskId)
{
    DestroyTask(gTasks[taskId].data[10]);
    DestroyTask(taskId);
    CloseMenu();
}

static void Task_MatsudaDebugMenu_LinkTest_CheckCancel(u8 taskId)
{
    if (gMain.newKeys == B_BUTTON)
        gTasks[(u8)gTasks[taskId].data[10]].func = Task_MatsudaDebugMenu_LinkTest_CloseLink;
}

static void Task_MatsudaDebugMenu_LinkTest_OpenLink(u8 taskId)
{
    u8 i;

    OpenLink();

    for (i = 0; i < 4; i++)
        gBlockRecvBuffer[i][0] = 255;

    gTasks[taskId].data[0] = 0;
    gTasks[taskId].func = Task_MatsudaDebugMenu_LinkTest_WaitForPlayers;
}

static void Task_MatsudaDebugMenu_LinkTest_WaitForPlayers(u8 taskId)
{
    TaskFunc nextTaskFunc;

    if (gTasks[taskId].data[0] < 10)
        gTasks[taskId].data[0]++;
    else
    {
        if (GetLinkPlayerCount_2() > 3)
        {
            gTasks[taskId].data[0] = 0;

            if (IsLinkMaster())
            {
                nextTaskFunc = Task_MatsudaDebugMenu_LinkTest_AdvanceMasterState;
                gTasks[taskId].func = (TaskFunc)nextTaskFunc;
            }
            else
            {
                nextTaskFunc = Task_MatsudaDebugMenu_LinkTest_WaitForRemotePlayers;
                gTasks[taskId].func = (TaskFunc)nextTaskFunc;
            }
        }
    }
}

static void Task_MatsudaDebugMenu_LinkTest_AdvanceMasterState(u8 taskId)
{
    gTasks[taskId].data[0] = gTasks[taskId].data[0] + 1;
    if ((gTasks[taskId].data[0]) == 101)
    {
        CheckShouldAdvanceLinkState();
        gTasks[taskId].data[0] = 0;
        gTasks[taskId].func = Task_MatsudaDebugMenu_LinkTest_WaitForRemotePlayers;
    }
}

static void Task_MatsudaDebugMenu_LinkTest_WaitForRemotePlayers(u8 taskId)
{
    if (gReceivedRemoteLinkPlayers)
    {
        gContestPlayerMonIndex = GetMultiplayerId();
        if (GetLinkPlayerCount() == 4)
        {
            gIsLinkContest = 1;
            SwitchTaskToFollowupFunc(taskId);
        }
    }
}

static void InitMatsudaDebugMenuDisplay(void)
{
    REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP;
    REG_DISPCNT |= DISPCNT_OBJ_ON | DISPCNT_BG0_ON;
    Text_LoadWindowTemplate(&gMenuTextWindowLoadTemplate);
    Text_InitWindowWithTemplate(&gMenuWindow, &gMenuTextWindowLoadTemplate);
    REG_MOSAIC = 0;
    REG_BLDCNT = 0;
    REG_BLDALPHA = 0;
    REG_BLDY = 0;
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;
    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    gBattle_BG1_X = 0;
    gBattle_BG1_Y = 0;
    gBattle_BG2_X = 0;
    gBattle_BG2_Y = 0;
    gBattle_BG3_X = 0;
    gBattle_BG3_Y = 0;
}

static void LoadMatsudaDebugMenuGraphics(void)
{
    u8 paletteText[] = _("{HIGHLIGHT WHITE2} ");
    DmaFill32Large(3, 0, (void *)VRAM, VRAM_SIZE, 0x1000);
    RenderTextHandleBold((void *)VRAM, paletteText);
    LoadFontDefaultPalette(&gMenuTextWindowLoadTemplate);
}

//const u8 gMatsudaDebugMenu_UnknownByteArray[] = {0xFC, 0x02, 0x0F, 0x00, 0xFF};
const u8 gMatsudaDebugCursorGfx[] = INCBIN_U8("graphics/unknown/ball.4bpp");
const u16 gMatsudaDebugCursorPal[] = INCBIN_U16("graphics/unknown/ball.gbapal");

const u8 gMatsudaDebugContestantNameCoords[][2] =
    {
        {2,  0},
        {17, 0},
        {2,  2},
        {17, 2},
    };

const u8 gMatsudaDebugContestStatLabelCoords[][2] =
    {
        {0,   6},
        {15,  6},
        {0,   8},
        {15,  8},
        {0,  10},
        {15, 10},
    };

const u8 gMatsudaDebugContestStatValueCoords[] =
    {
        // TODO: 2d array?
        7,   6,
        22,  6,
        7,   8,
        22,  8,
        7,  10,
        22, 10,
    };

const u8 gMatsudaDebugContestMoveCoords[][2] =
    {
        {2,  14},
        {17, 14},
        {2,  16},
        {17, 16},
    };

const u8 gMatsudaDebugCursorXPositions[] = {0x08, 0x80, 0x08, 0x80, 0x10, 0x10, 0x30, 0xA8, 0x30, 0xA8, 0x30, 0xA8, 0x08, 0x80, 0x08, 0x80, 0x10, 0x88};
const u8 gMatsudaDebugCursorYPositions[] = {0x08, 0x18, 0x28, 0x38, 0x48, 0x58, 0x78, 0x88, 0x98};

const struct SpriteSheet gMatsudaDebugCursorSpriteSheet[] = {gMatsudaDebugCursorGfx, 32, 0x4B0};
const struct SpritePalette gMatsudaDebugCursorSpritePalette[] = {gMatsudaDebugCursorPal, 0x4B0};

const struct OamData gOamData_MatsudaDebugCursor =
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

const struct SpriteTemplate gMatsudaDebugCursorSpriteTemplate =
    {
        .tileTag = 1200,
        .paletteTag = 1200,
        .oam = &gOamData_MatsudaDebugCursor,
        .anims = gDummySpriteAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCB_MatsudaDebugCursor,
    };

static void (* const sMatsudaDebugCursorActions[][2])(struct Sprite *, s8) =
    {
        {SelectMatsudaDebugContestant0, SelectMatsudaDebugContestant1},
        {SelectMatsudaDebugContestant2, SelectMatsudaDebugContestant3},
        {AdjustMatsudaDebugContestRank, AdjustMatsudaDebugContestRank},
        {AdjustMatsudaDebugContestantCool, AdjustMatsudaDebugContestantCute},
        {AdjustMatsudaDebugContestantBeauty, AdjustMatsudaDebugContestantSmart},
        {AdjustMatsudaDebugContestantTough, AdjustMatsudaDebugContestantSheen},
        {AdjustMatsudaDebugContestantMove1, AdjustMatsudaDebugContestantMove2},
        {AdjustMatsudaDebugContestantMove3, AdjustMatsudaDebugContestantMove4},
        {AdjustMatsudaDebugContestCategory, ReturnToFieldWithMatsudaDebugContestScores},
    };

static const u8 sMatsudaDebugMenuSelectedContestantTextPrefix[] = _("{COLOR CYAN}{HIGHLIGHT RED}");
static const u8 sMatsudaDebugMenuStartText[] = DTR("かいし", "START"); // because StartText is only called by StringCopy, this part appears to be unused.
static const u8 sMatsudaDebugMenuBackText[] = DTR("もどる", "BACK");

#ifdef GERMAN
static const u8 sMatsudaDebugMenuBattlePointsText[] = _("{COLOR MAGENTA}BP,");
#else
static const u8 sMatsudaDebugMenuBattlePointsText[] = _("{COLOR MAGENTA}BP.");
#endif

static const u8 sMatsudaDebugMenuCoolText[] = DTR("かっこよさ", "COOL");
static const u8 sMatsudaDebugMenuCuteText[] = DTR("かわいさ", "CUTE");
static const u8 sMatsudaDebugMenuBeautyText[] = DTR("うつくしさ", "BEAUTY");
static const u8 sMatsudaDebugMenuSmartText[] = DTR("かしこさ", "SMART");
static const u8 sMatsudaDebugMenuToughText[] = DTR("たくましさ", "TOUGH");
static const u8 sMatsudaDebugMenuSheenText[] = DTR("けづや", "SHEEN");

static const u8 *const sMatsudaDebugMenuContestStatTexts[] =
    {
        sMatsudaDebugMenuCoolText,
        sMatsudaDebugMenuCuteText,
        sMatsudaDebugMenuBeautyText,
        sMatsudaDebugMenuSmartText,
        sMatsudaDebugMenuToughText,
        sMatsudaDebugMenuSheenText,
    };

static const u8 sMatsudaDebugMenuCoolContestText[] = DTR("かっこよさコンテスト", "COOL CONTEST");
static const u8 sMatsudaDebugMenuBeautyContestText[] = DTR("うつくしさコンテスト", "BEAUTY CONTEST");
static const u8 sMatsudaDebugMenuCuteContestText[] = DTR("かわいさコンテスト　", "CUTE CONTEST");
static const u8 sMatsudaDebugMenuSmartContestText[] = DTR("かしこさコンテスト　", "SMART CONTEST");
static const u8 sMatsudaDebugMenuToughContestText[] = DTR("たくましさコンテスト", "TOUGH CONTEST");

static const u8 *const sMatsudaDebugMenuContestCategoryTexts[] =
    {
        sMatsudaDebugMenuCoolContestText,
        sMatsudaDebugMenuBeautyContestText,
        sMatsudaDebugMenuCuteContestText,
        sMatsudaDebugMenuSmartContestText,
        sMatsudaDebugMenuToughContestText,
    };

static const u8 sMatsudaDebugMenuNormalRankText[] = DTR("ハジツゲ/ノーマル", "FALLARBOR/NORMAL"); // Fallarbor / Normal
static const u8 sMatsudaDebugMenuSuperRankText[] = DTR("シダケ/スーパー　", "VERDANTURF/SUPER"); // Verdanturf / Super
static const u8 sMatsudaDebugMenuHyperRankText[] = DTR("カイナ/ハイパー　", "SLATEPORT/HYPER"); // Slateport / Hyper
static const u8 sMatsudaDebugMenuMasterRankText[] = DTR("ミナモ/マスター　", "LILYCOVE/MASTER"); // Lilycove / Master

static const u8 *const sMatsudaDebugMenuContestRankTexts[] =
    {
        sMatsudaDebugMenuNormalRankText,
        sMatsudaDebugMenuSuperRankText,
        sMatsudaDebugMenuHyperRankText,
        sMatsudaDebugMenuMasterRankText,
    };

static const struct OamData sOamData_MatsudaDebugTextSprite =
    {
        .y = 0,
        .affineMode = 0,
        .objMode = 0,
        .mosaic = 0,
        .bpp = 0,
        .shape = 1,
        .x = 0,
        .matrixNum = 0,
        .size = 1,
        .tileNum = 1023,
        .priority = 0,
        .paletteNum = 15,
        .affineParam = 0,
    };

static void CB2_MatsudaDebugMenu(void)
{
    AnimateSprites();
    BuildOamBuffer();
    RunTasks();
    UpdatePaletteFade();
    if (gMain.newKeys == SELECT_BUTTON)
        SetMainCallback2(CB2_ReturnToFieldWithOpenMenu);
}

static void VBlankCB_MatsudaDebugMenu(void)
{
    REG_BG0HOFS = gBattle_BG0_X;
    REG_BG0VOFS = gBattle_BG0_Y;
    REG_BG1HOFS = gBattle_BG1_X;
    REG_BG1VOFS = gBattle_BG1_Y;
    REG_BG2HOFS = gBattle_BG2_X;
    REG_BG2VOFS = gBattle_BG2_Y;
    REG_BG3HOFS = gBattle_BG3_X;
    REG_BG3VOFS = gBattle_BG3_Y;
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    ScanlineEffect_InitHBlankDmaTransfer();
}

static void InitMatsudaDebugMenuContest(void)
{
    u8 i;
    u8 zero;
    u8 cursorSpriteId;

    gPaletteFade.bufferTransferDisabled = 1;
    SetVBlankCallback(0);
    InitMatsudaDebugMenuDisplay();
    ScanlineEffect_Clear();
    ResetPaletteFade();
    ResetSpriteData();
    ResetTasks();
    FreeAllSpritePalettes();
    LoadMatsudaDebugMenuGraphics();
    SetVBlankCallback(VBlankCB_MatsudaDebugMenu);
    SetMainCallback2(CB2_MatsudaDebugMenu);
    gPaletteFade.bufferTransferDisabled = 0;
    gContestMonPartyIndex = 0;

    if (!(gContestMons[0].nickname[0]))
        Contest_InitAllPokemon(0, 0);

    Contest_CreatePlayerMon(gContestMonPartyIndex);

    for (i = 0; i < 6; i++)
    {
        Text_InitWindowAndPrintText(&gMenuWindow, sMatsudaDebugMenuContestStatTexts[i],
            (0xA * i + 0x2A), gMatsudaDebugContestStatLabelCoords[i][0],
            gMatsudaDebugContestStatLabelCoords[i][1]);
    }

    gSpecialVar_ContestCategory = gSpecialVar_ContestRank = 0;
    zero = 0; // it's possible this was some assignment that matsuda used to quickly edit and test things without changing whats passed to the later functions.
    PrintMatsudaDebugContestCategory(zero);
    PrintMatsudaDebugContestRank(gSpecialVar_ContestRank);
    Text_InitWindowAndPrintText(&gMenuWindow, sMatsudaDebugMenuBackText, 0xD6, 0x12, 0x12);
    Text_InitWindowAndPrintText(&gMenuWindow, sMatsudaDebugMenuBattlePointsText, 0xDC, zero, 0xC);
    LoadSpriteSheet(gMatsudaDebugCursorSpriteSheet);
    LoadSpritePalette(gMatsudaDebugCursorSpritePalette);
    PrintMatsudaDebugContestantNames(3);
    PrintMatsudaDebugContestantData(3);
    PrintMatsudaDebugContestantRound1Points(3, zero);
    cursorSpriteId = CreateSprite(&gMatsudaDebugCursorSpriteTemplate, gMatsudaDebugCursorXPositions[3], gMatsudaDebugCursorYPositions[1], 5);
    gSprites[cursorSpriteId].data[0] = 1;
    gSprites[cursorSpriteId].data[1] = 1;
    gSprites[cursorSpriteId].data[2] = 3;
    gSprites[cursorSpriteId].data[3] = zero; // only this assignment of zero is necessary. other replacements of 0 with zero do not change the asm, compiler will treat it the same.
}

void PrintMatsudaDebugContestantNames(u8 contestantId)
{
    u8 i;

    Text_FillWindowRectDefPalette(&gMenuWindow, 0, 0, 0, 0x1E, 3);
    StringCopy(gSharedMem, sMatsudaDebugMenuSelectedContestantTextPrefix);
    StringAppend(gSharedMem, gContestMons[contestantId].trainerName);

    for (i = 0; i < 4; i++)
    {
        if (contestantId == i)
            Text_InitWindowAndPrintText(&gMenuWindow, gSharedMem, 10 * i + 2, gMatsudaDebugContestantNameCoords[i][0], gMatsudaDebugContestantNameCoords[i][1]);
        else
            Text_InitWindowAndPrintText(&gMenuWindow, gContestMons[i].trainerName, 10 * i + 2, gMatsudaDebugContestantNameCoords[i][0], gMatsudaDebugContestantNameCoords[i][1]);
    }
}

static void PrintMatsudaDebugContestantCool(u8 contestantId)
{
    ConvertIntToDecimalStringN(gSharedMem, gContestMons[contestantId].cool, STR_CONV_MODE_RIGHT_ALIGN, 3);
    Text_InitWindowAndPrintText(&gMenuWindow, gSharedMem, 0x66, gMatsudaDebugContestStatValueCoords[0], gMatsudaDebugContestStatValueCoords[1]);
}

static void PrintMatsudaDebugContestantCute(u8 contestantId)
{
    ConvertIntToDecimalStringN(gSharedMem, gContestMons[contestantId].cute, STR_CONV_MODE_RIGHT_ALIGN, 3);
    Text_InitWindowAndPrintText(&gMenuWindow, gSharedMem, 0x6C, gMatsudaDebugContestStatValueCoords[2], gMatsudaDebugContestStatValueCoords[3]);
}

static void PrintMatsudaDebugContestantBeauty(u8 contestantId)
{
    ConvertIntToDecimalStringN(gSharedMem, gContestMons[contestantId].beauty, STR_CONV_MODE_RIGHT_ALIGN, 3);
    Text_InitWindowAndPrintText(&gMenuWindow, gSharedMem, 0x72, gMatsudaDebugContestStatValueCoords[4], gMatsudaDebugContestStatValueCoords[5]);
}

static void PrintMatsudaDebugContestantSmart(u8 contestantId)
{
    ConvertIntToDecimalStringN(gSharedMem, gContestMons[contestantId].smart, STR_CONV_MODE_RIGHT_ALIGN, 3);
    Text_InitWindowAndPrintText(&gMenuWindow, gSharedMem, 0x78, gMatsudaDebugContestStatValueCoords[6], gMatsudaDebugContestStatValueCoords[7]);
}

static void PrintMatsudaDebugContestantTough(u8 contestantId)
{
    ConvertIntToDecimalStringN(gSharedMem, gContestMons[contestantId].tough, STR_CONV_MODE_RIGHT_ALIGN, 3);
    Text_InitWindowAndPrintText(&gMenuWindow, gSharedMem, 0x7E, gMatsudaDebugContestStatValueCoords[8], gMatsudaDebugContestStatValueCoords[9]);
}

static void PrintMatsudaDebugContestantSheen(u8 contestantId)
{
    ConvertIntToDecimalStringN(gSharedMem, gContestMons[contestantId].sheen, STR_CONV_MODE_RIGHT_ALIGN, 3);
    Text_InitWindowAndPrintText(&gMenuWindow, gSharedMem, 0x84, gMatsudaDebugContestStatValueCoords[10], gMatsudaDebugContestStatValueCoords[11]);
}

static void PrintMatsudaDebugContestantMove(u8 contestantId, u8 moveSlot)
{
    Text_FillWindowRectDefPalette(&gMenuWindow, 0, gMatsudaDebugContestMoveCoords[moveSlot][0], gMatsudaDebugContestMoveCoords[moveSlot][1], gMatsudaDebugContestMoveCoords[moveSlot][0] + 7, gMatsudaDebugContestMoveCoords[moveSlot][1] + 1);
    Text_InitWindowAndPrintText(&gMenuWindow, gMoveNames[gContestMons[contestantId].moves[moveSlot]], 0x8A + moveSlot * 14, gMatsudaDebugContestMoveCoords[moveSlot][0], gMatsudaDebugContestMoveCoords[moveSlot][1]);
    ConvertIntToDecimalStringN(gStringVar1, gContestMons[contestantId].moves[moveSlot], STR_CONV_MODE_LEADING_ZEROS, 3);
    Text_InitWindowAndPrintText(&gMenuWindow, gStringVar1, 0xFA + moveSlot * 6, gMatsudaDebugContestMoveCoords[moveSlot][0] + 7, gMatsudaDebugContestMoveCoords[moveSlot][1]);
}

static void PrintMatsudaDebugContestCategory(u8 contestCategory)
{
    Text_InitWindowAndPrintText(&gMenuWindow, sMatsudaDebugMenuContestCategoryTexts[contestCategory], 0xC2, 3, 0x12);
}

void PrintMatsudaDebugContestRank(u8 contestRank)
{
    Text_InitWindowAndPrintText(&gMenuWindow, sMatsudaDebugMenuContestRankTexts[contestRank], 0xE8, 3, 4);
}

static void PrintMatsudaDebugContestantRound1Points(u8 contestantId, u8 contestCategory)
{
    u16 round1Points = CalculateContestantRound1Points(contestantId, contestCategory);

    ConvertIntToDecimalStringN(gSharedMem, round1Points, STR_CONV_MODE_RIGHT_ALIGN, 3);
    Text_InitWindowAndPrintText(&gMenuWindow, gSharedMem, 0xE2, 3, 0xC);
}

void PrintMatsudaDebugContestantData(u8 contestantId)
{
    u8 i;

    PrintMatsudaDebugContestantCool(contestantId);
    PrintMatsudaDebugContestantCute(contestantId);
    PrintMatsudaDebugContestantBeauty(contestantId);
    PrintMatsudaDebugContestantSmart(contestantId);
    PrintMatsudaDebugContestantTough(contestantId);
    PrintMatsudaDebugContestantSheen(contestantId);

    for (i = 0; i < 4; i++)
        PrintMatsudaDebugContestantMove(contestantId, i);
}

void SetDebugMonForContest(void)
{
    SetMonData(&gPlayerParty[0], MON_DATA_COOL, &gContestMons[gContestPlayerMonIndex].cool);
    SetMonData(&gPlayerParty[0], MON_DATA_CUTE, &gContestMons[gContestPlayerMonIndex].cute);
    SetMonData(&gPlayerParty[0], MON_DATA_BEAUTY, &gContestMons[gContestPlayerMonIndex].beauty);
    SetMonData(&gPlayerParty[0], MON_DATA_SMART, &gContestMons[gContestPlayerMonIndex].smart);
    SetMonData(&gPlayerParty[0], MON_DATA_TOUGH, &gContestMons[gContestPlayerMonIndex].tough);
    SetMonData(&gPlayerParty[0], MON_DATA_SHEEN, &gContestMons[gContestPlayerMonIndex].sheen);
    SetMonData(&gPlayerParty[0], MON_DATA_MOVE1, &gContestMons[gContestPlayerMonIndex].moves[0]);
    SetMonData(&gPlayerParty[0], MON_DATA_MOVE2, &gContestMons[gContestPlayerMonIndex].moves[1]);
    SetMonData(&gPlayerParty[0], MON_DATA_MOVE3, &gContestMons[gContestPlayerMonIndex].moves[2]);
    SetMonData(&gPlayerParty[0], MON_DATA_MOVE4, &gContestMons[gContestPlayerMonIndex].moves[3]);
}

void SpriteCB_MatsudaDebugCursor(struct Sprite *sprite)
{
    switch (gMain.newAndRepeatedKeys)
    {
    case DPAD_RIGHT:
    case DPAD_LEFT:
        sprite->data[0] ^= 1;
        break;
    case DPAD_UP:
        if (sprite->data[1] == 0)
            sprite->data[1] = 8;
        else
            sprite->data[1]--;
        break;
    case DPAD_DOWN:
        if (sprite->data[1] == 8)
            sprite->data[1] = 0;
        else
            sprite->data[1]++;
        break;
    case A_BUTTON:
        sMatsudaDebugCursorActions[sprite->data[1]][sprite->data[0]](sprite, 1);
        PrintMatsudaDebugContestantRound1Points(sprite->data[2], sprite->data[3]);
        break;
    case B_BUTTON:
        sMatsudaDebugCursorActions[sprite->data[1]][sprite->data[0]](sprite, -1);
        PrintMatsudaDebugContestantRound1Points(sprite->data[2], sprite->data[3]);
        break;
    case R_BUTTON:
        sMatsudaDebugCursorActions[sprite->data[1]][sprite->data[0]](sprite, 10);
        PrintMatsudaDebugContestantRound1Points(sprite->data[2], sprite->data[3]);
        break;
    case L_BUTTON:
        sMatsudaDebugCursorActions[sprite->data[1]][sprite->data[0]](sprite, -10);
        PrintMatsudaDebugContestantRound1Points(sprite->data[2], sprite->data[3]);
        break;
    case START_BUTTON:
        StartMatsudaDebugContest(sprite, 1);
        break;
    }
    sprite->x = gMatsudaDebugCursorXPositions[sprite->data[0] + sprite->data[1] * 2];
    sprite->y = gMatsudaDebugCursorYPositions[sprite->data[1]];
}

static void SelectMatsudaDebugContestant(struct Sprite *sprite, s8 delta, u8 contestantId)
{
    if (delta == 1)
    {
        sprite->data[2] = contestantId;
        PrintMatsudaDebugContestantNames(contestantId);
        PrintMatsudaDebugContestantData(sprite->data[2]);
    }
}

void SelectMatsudaDebugContestant0(struct Sprite *sprite, s8 delta)
{
    SelectMatsudaDebugContestant(sprite, delta, 0);
}

void SelectMatsudaDebugContestant1(struct Sprite *sprite, s8 delta)
{
    SelectMatsudaDebugContestant(sprite, delta, 1);
}

void SelectMatsudaDebugContestant2(struct Sprite *sprite, s8 delta)
{
    SelectMatsudaDebugContestant(sprite, delta, 2);
}

void SelectMatsudaDebugContestant3(struct Sprite *sprite, s8 delta)
{
    SelectMatsudaDebugContestant(sprite, delta, 3);
}

static u8 AdjustMatsudaDebugContestStat(u32 unused, u8 value, s8 delta) // first param is unused.
{
    s16 adjustedValue = value + delta;

    if (adjustedValue > 255)
        adjustedValue = 255;
    else if (adjustedValue < 0)
        adjustedValue = 0;

    return adjustedValue;
}

void AdjustMatsudaDebugContestantCool(struct Sprite *sprite, s8 delta)
{
    u8 unusedContestantId = sprite->data[2];

    gContestMons[sprite->data[2]].cool = AdjustMatsudaDebugContestStat(unusedContestantId, gContestMons[sprite->data[2]].cool, delta);
    PrintMatsudaDebugContestantCool(sprite->data[2]);
}

void AdjustMatsudaDebugContestantCute(struct Sprite *sprite, s8 delta)
{
    u8 unusedContestantId = sprite->data[2];

    gContestMons[sprite->data[2]].cute = AdjustMatsudaDebugContestStat(unusedContestantId, gContestMons[sprite->data[2]].cute, delta);
    PrintMatsudaDebugContestantCute(sprite->data[2]);
}

void AdjustMatsudaDebugContestantBeauty(struct Sprite *sprite, s8 delta)
{
    u8 unusedContestantId = sprite->data[2];

    gContestMons[sprite->data[2]].beauty = AdjustMatsudaDebugContestStat(unusedContestantId, gContestMons[sprite->data[2]].beauty, delta);
    PrintMatsudaDebugContestantBeauty(sprite->data[2]);
}

void AdjustMatsudaDebugContestantSmart(struct Sprite *sprite, s8 delta)
{
    u8 unusedContestantId = sprite->data[2];

    gContestMons[sprite->data[2]].smart = AdjustMatsudaDebugContestStat(unusedContestantId, gContestMons[sprite->data[2]].smart, delta);
    PrintMatsudaDebugContestantSmart(sprite->data[2]);
}

void AdjustMatsudaDebugContestantTough(struct Sprite *sprite, s8 delta)
{
    u8 unusedContestantId = sprite->data[2];

    gContestMons[sprite->data[2]].tough = AdjustMatsudaDebugContestStat(unusedContestantId, gContestMons[sprite->data[2]].tough, delta);
    PrintMatsudaDebugContestantTough(sprite->data[2]);
}

void AdjustMatsudaDebugContestantSheen(struct Sprite *sprite, s8 delta)
{
    u8 unusedContestantId = sprite->data[2];

    gContestMons[sprite->data[2]].sheen = AdjustMatsudaDebugContestStat(unusedContestantId, gContestMons[sprite->data[2]].sheen, delta);
    PrintMatsudaDebugContestantSheen(sprite->data[2]);
}

// a similar function is at 0x80AA908, however, it apparently returns the wrong type (u8 vs u16).
static u16 AdjustMatsudaDebugContestMove(u32 unused, u16 move, s8 delta) // first param is unused.
{
    s16 adjustedMove = move + delta;

    if (adjustedMove > 354)
        adjustedMove = 1;
    else if (adjustedMove < 0)
        adjustedMove = 354;

    return adjustedMove;
}

void AdjustMatsudaDebugContestantMove1(struct Sprite *sprite, s8 delta)
{
    u8 unusedContestantId = sprite->data[2];

    gContestMons[sprite->data[2]].moves[0] = AdjustMatsudaDebugContestMove(unusedContestantId, gContestMons[sprite->data[2]].moves[0], delta);
    PrintMatsudaDebugContestantMove(sprite->data[2], 0);
}

void AdjustMatsudaDebugContestantMove2(struct Sprite *sprite, s8 delta)
{
    u8 unusedContestantId = sprite->data[2];

    gContestMons[sprite->data[2]].moves[1] = AdjustMatsudaDebugContestMove(unusedContestantId, gContestMons[sprite->data[2]].moves[1], delta);
    PrintMatsudaDebugContestantMove(sprite->data[2], 1);
}

void AdjustMatsudaDebugContestantMove3(struct Sprite *sprite, s8 delta)
{
    u8 unusedContestantId = sprite->data[2];

    gContestMons[sprite->data[2]].moves[2] = AdjustMatsudaDebugContestMove(unusedContestantId, gContestMons[sprite->data[2]].moves[2], delta);
    PrintMatsudaDebugContestantMove(sprite->data[2], 2);
}

void AdjustMatsudaDebugContestantMove4(struct Sprite *sprite, s8 delta)
{
    u8 unusedContestantId = sprite->data[2];

    gContestMons[sprite->data[2]].moves[3] = AdjustMatsudaDebugContestMove(unusedContestantId, gContestMons[sprite->data[2]].moves[3], delta);
    PrintMatsudaDebugContestantMove(sprite->data[2], 3);
}

void AdjustMatsudaDebugContestCategory(struct Sprite *sprite, s8 delta)
{
    s8 categoryDelta = (delta > 0) ? 1 : -1;
    s8 contestCategory = sprite->data[3] + categoryDelta;

    if (contestCategory < 0)
        contestCategory = 0;
    else if (contestCategory > 4)
        contestCategory = 4;
    PrintMatsudaDebugContestCategory(contestCategory);
    sprite->data[3] = contestCategory;
    gSpecialVar_ContestCategory = sprite->data[3];
    Contest_InitAllPokemon(sprite->data[3], gSpecialVar_ContestRank);
    PrintMatsudaDebugContestantNames(sprite->data[2]);
    PrintMatsudaDebugContestantData(sprite->data[2]);
}

void AdjustMatsudaDebugContestRank(struct Sprite *sprite, s8 delta)
{
    if (delta > 0)
        gSpecialVar_ContestRank++;
    else if (gSpecialVar_ContestRank != 0)
        gSpecialVar_ContestRank--;
    if (gSpecialVar_ContestRank > 3)
        gSpecialVar_ContestRank = 3;
    PrintMatsudaDebugContestRank(gSpecialVar_ContestRank);
    Contest_InitAllPokemon(gSpecialVar_ContestCategory, gSpecialVar_ContestRank);
    PrintMatsudaDebugContestantNames(sprite->data[2]);
    PrintMatsudaDebugContestantData(sprite->data[2]);
}

void CB2_StartMatsudaDebugContest(void)
{
    UpdatePaletteFade();
    if (!gPaletteFade.active)
    {
        SetDebugMonForContest();
        if (!(gIsLinkContest & 1))
            CalculateRound1Points(gSharedMem[0]);
        SetMainCallback2(CB2_StartContest);
    }
}

void StartMatsudaDebugContest(struct Sprite *sprite, s8 delta)
{
    if (delta == 1)
    {
        gSharedMem[0] = sprite->data[3];
        SetMainCallback2(CB2_StartMatsudaDebugContest);
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    }
}

void ReturnToFieldWithMatsudaDebugContestScores(struct Sprite *sprite, s8 delta)
{
    if (delta == 1)
    {
        int i;

        SetDebugMonForContest();
        for (i = 0; i < 4; i++)
            gContestMonRound1Points[i] = CalculateContestantRound1Points(i, gSpecialVar_ContestCategory);
        SetMainCallback2(CB2_ReturnToFieldWithOpenMenu);
    }
}

void PrintMatsudaDebugTextSprite(u8 *text, u8 x, u8 y, u8 spriteSlot)
{
    u16 textLength;
    u8 oamIndex;
    s32 i;
    u8 glyphRenderBuffer[32];
    u8 glyphCopyBuffer[32];
    u8 glyphText[2];

    DmaClear32(3, (void *)(VRAM + 0x18000 - (spriteSlot + 1) * 256), 0x100);

    textLength = StringLength(text);
    if (textLength > 8)
        textLength = 8;

    for (i = 0; i < textLength; i++)
    {
        glyphText[0] = text[i];
        glyphText[1] = EOS;
        RenderTextHandleBold(glyphRenderBuffer, glyphText);
        DmaCopy32Defvars(3, glyphCopyBuffer, (void *)(VRAM + 0x18000 - (spriteSlot + 1) * 256 + i * 32), sizeof(glyphRenderBuffer));
    }

    oamIndex = 124 - spriteSlot * 2;

    gMain.oamBuffer[oamIndex] = sOamData_MatsudaDebugTextSprite;
    gMain.oamBuffer[oamIndex].tileNum = 1024 - (spriteSlot + 1) * 8;
    gMain.oamBuffer[oamIndex].x = x;
    gMain.oamBuffer[oamIndex].y = y;

    gMain.oamBuffer[oamIndex + 1] = sOamData_MatsudaDebugTextSprite;
    gMain.oamBuffer[oamIndex + 1].tileNum = gMain.oamBuffer[oamIndex].tileNum + 4;
    gMain.oamBuffer[oamIndex + 1].x = x + 32;
    gMain.oamBuffer[oamIndex + 1].y = y;
}

void PrintMatsudaDebugNumberSprite(s32 number, u8 x, u8 y, u8 spriteSlot)
{
    u8 text[12];
    u8 offset = 0;

    if (number < 0)
    {
        text[0] = 0xAE;
        offset = 1;
    }
    ConvertIntToDecimalStringN(text + offset, (number < 0) ? -number : number, 0, 8);
    PrintMatsudaDebugTextSprite(text, x, y, spriteSlot);
}

void PrepareContestArtistDebugResult(void)
{
    s32 i;

    gContestDebugMode = 1;
    gContestPlayerMonIndex = 3;
    Contest_CreatePlayerMon(0);

    for (i = 3; i > -1; i--)
        gContestFinalStandings[i] = 3 - i;

    for (i = 0; i < 3; i++)
    {
        gContestMonRound1Points[i] = 0;
        gContestMonAppealPointTotals[i] = 0;
        gContestMonTotalPoints[i] = 0;
        gContestMons[i] = gContestMons[3];
    }

    gContestMonRound1Points[3] = 0x12C;
    gContestMonAppealPointTotals[3] = 0x190;
    gContestMonTotalPoints[3] = 0x190;
    SaveContestWinner(CONTEST_SAVE_FOR_ARTIST);
}

u8 MatsudaDebugMenu_SetHighScore(void)
{
    PrepareContestArtistDebugResult();
    CloseMenu();
    return 1;
}

u8 MatsudaDebugMenu_ResetHighScore(void)
{
    s32 i;

    gContestDebugMode = 0;
    for (i = 0; i < 4; i++)
    {
        gContestMonRound1Points[i] = 0;
        gContestMonAppealPointTotals[i] = 0;
        gContestMonTotalPoints[i] = 0;
    }
    CloseMenu();
    return 1;
}

u8 MatsudaDebugMenu_SetArtMuseumItems(void)
{
    s32 i;

    gContestPlayerMonIndex = 3;
    Contest_CreatePlayerMon(0);
    for (i = 3; i > -1; i--)
        gContestFinalStandings[i] = 3 - i;
    for (gSpecialVar_ContestCategory = 0; gSpecialVar_ContestCategory < 5; gSpecialVar_ContestCategory++)
        SaveContestWinner(0xFF);
    CloseMenu();
    return 1;
}
