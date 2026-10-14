#include "global.h"
#include "contest.h"
#include "battle.h"
#include "battle_anim.h"
#include "constants/event_objects.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/songs.h"
#include "constants/species.h"
#include "contest_ai.h"
#include "contest_effect.h"
#include "contest_link.h"
#include "data.h"
#include "decompress.h"
#include "ewram.h"
#include "graphics.h"
#include "link.h"
#include "m4a.h"
#include "main.h"
#include "menu.h"
#include "menu_cursor.h"
#include "overworld.h"
#include "palette.h"
#include "random.h"
#include "scanline_effect.h"
#include "script.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "tv.h"
#include "util.h"

#include "contest_internal.h"

static void TaskDummy1(u8 taskId);
static void SetupContestGpuRegs(void);
static void InitContestWindows(void);
static void LoadContestPalettes(void);
static void InitContestResources(void);
static void Task_StartContestWaitFade(u8 taskId);
static void Task_TryStartLinkContest(u8 taskId);
static void Task_CommunicateMonIdxs(u8 taskId);
static void Task_EndCommunicateMonIdxs(u8 taskId);
static void Task_ReadyStartLinkContest(u8 taskId);
static bool8 SetupContestGraphics(u8 *stateVar);
static void Task_WaitToRaiseCurtainAtStart(u8 taskId);
static void Task_RaiseCurtainAtStart(u8 taskId);
static void CB2_ContestMain(void);
static void VBlankCB_Contest(void);
static void Task_DisplayAppealNumberText(u8 taskId);
static void Task_TryShowMoveSelectScreen(u8 taskId);
static void Task_ShowMoveSelectScreen(u8 taskId);
static void Task_HandleMoveSelectInput(u8 taskId);
static void DrawMoveSelectArrow(s8 moveIndex);
static void EraseMoveSelectArrow(s8 moveIndex);
static void Task_SelectedMove(u8 taskId);
static void Task_EndCommunicateMoveSelections(u8 taskId);
static void Task_HideMoveSelectScreen(u8 taskId);
static void Task_HideApplauseMeterForAppealStart(u8 taskId);
static void Task_WaitHideApplauseMeterForAppealStart(u8 taskId);
static void Task_AppealSetup(u8 taskId);
static void Task_DoAppeals(u8 taskId);
static void SpriteCB_MonSlideIn(struct Sprite *sprite);
static void SpriteCB_MonSlideOut(struct Sprite *sprite);
static void Task_FinishRoundOfAppeals(u8 taskId);
static void Task_ReadyUpdateHeartSliders(u8 taskId);
static void Task_UpdateHeartSliders(u8 taskId);
static void Task_WaitForHeartSliders(u8 taskId);
static void Task_RestorePlttBufferUnfaded(u8 taskId);
static void Task_WaitPrintRoundResult(u8 taskId);
static void Task_PrintRoundResultText(u8 taskId);
static void Task_ReUpdateHeartSliders(u8 taskId);
static void Task_WaitForHeartSlidersAgain(u8 taskId);
static void Task_DropCurtainAtRoundEnd(u8 taskId);
static void Task_TryStartNextRoundOfAppeals(u8 taskId);
static void Task_StartNewRoundOfAppeals(u8 taskId);
static void Task_EndAppeals(u8 taskId);
static void Task_WaitForOutOfTimeMsg(u8 taskId);
static void Task_DropCurtainAtAppealsEnd(u8 taskId);
static void Task_TryCommunicateFinalStandings(u8 taskId);
static void Task_CommunicateFinalStandings(u8 taskId);
static void Task_EndCommunicateFinalStandings(u8 taskId);
static void Task_ContestReturnToField(u8 taskId);
static void FieldCB_ContestReturnToField(void);

extern u8 gUnknown_020297ED;

extern struct Window gWindowTemplate_Contest_MoveDescription;
u32 gContestRngValue;

extern const u8 gUnknown_083CA340[][4];
extern const u8 gText_AppealNumWhichMoveWillBePlayed[];
extern const u8 gText_AppealNumButItCantParticipate[];
extern const u8 gText_MonAppealedWithMove[];
extern const u8 gText_MonWasWatchingOthers[];
extern const u8 gText_JudgeLookedAtMonExpectantly[];
extern const u8 gText_AppealComboWentOverWell[];
extern const u8 gText_AppealComboWentOverVeryWell[];
extern const u8 gText_AppealComboWentOverExcellently[];
extern const u8 gText_MonWasTooNervousToMove[];
extern const u8 gText_AllOutOfAppealTime[];
extern const u8 *const gContestStandOutStrings[];
extern const u8 gText_RepeatedAppeal[];
extern const u8 gText_MonsXWentOverGreat[];
extern const u8 gText_MonsXDidntGoOverWell[];
extern const u8 gText_MonsXGotTheCrowdGoing[];
extern const u8 gText_MonCantAppealNextTurn[];
extern const u8 gText_CrowdContinuesToWatchMon[];
extern const u8 gText_MonsMoveIsIgnored[];
extern const u8 *const gContestCategoryStrings[];
extern const u8 *const sInvalidContestMoveNames[];
extern const u8 gUnknownText_UnknownFormatting2[];
extern const u8 gUnknownText_UnknownFormatting3[];

EWRAM_DATA u8 gUnknown_0203856C = 0;
EWRAM_DATA struct ContestPokemon gContestMons[4] = {0};
EWRAM_DATA s16 gContestMonRound1Points[4] = {0};
EWRAM_DATA s16 gContestMonTotalPoints[4] = {0};
EWRAM_DATA s16 gContestMonAppealPointTotals[4] = {0};
EWRAM_DATA s16 gContestMonRound2Points[4] = {0};
EWRAM_DATA u8 gContestFinalStandings[4] = {0};  // What "place" each participant came in.
EWRAM_DATA u8 gContestMonPartyIndex = 0;
EWRAM_DATA u8 gContestPlayerMonIndex = 0;
EWRAM_DATA u8 gContestantTurnOrder[4] = {0};
EWRAM_DATA u8 gLinkContestFlags = 0;
EWRAM_DATA u8 gContestLinkLeaderIndex = 0;
EWRAM_DATA u16 gSpecialVar_ContestCategory = 0;
EWRAM_DATA u16 gSpecialVar_ContestRank = 0;

static void TaskDummy1(u8 taskId)
{
}

void ResetLinkContestBoolean(void)
{
    gLinkContestFlags = FALSE;
}

static void SetupContestGpuRegs(void)
{
    u16 savedIme;

    REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP;
    savedIme = REG_IME;
    REG_IME = 0;
    REG_IE |= INTR_FLAG_VBLANK;
    REG_IME = savedIme;
    REG_DISPSTAT = DISPSTAT_VBLANK_INTR;
    REG_BG0CNT = 0x9800;
    REG_BG1CNT = 0x9E09;
    REG_BG2CNT = 0x9C00;
    REG_BG3CNT = 0x3A03;
    REG_BLDCNT = 0;
    REG_BLDALPHA = 0;
    REG_BLDY = 0;
    REG_WININ = 0x3F3F;
    REG_WINOUT = 0x3F3F;
    REG_DISPCNT |= 0x7F00;

    gBattle_BG0_X = 0;
    gBattle_BG0_Y = 0;
    gBattle_BG1_X = 0;
    gBattle_BG1_Y = 0;
    gBattle_BG2_X = 0;
    gBattle_BG2_Y = 0;
    gBattle_BG3_X = 0;
    gBattle_BG3_Y = 0;
    gBattle_WIN0H = 0;
    gBattle_WIN0V = 0;
    gBattle_WIN1H = 0;
    gBattle_WIN1V = 0;
}

void LoadContestBgAfterMoveAnim(void)
{
    s32 i;

    LZDecompressVram(gContestInterfaceGfx, (void *)VRAM);
    LZDecompressVram(gContestAudienceGfx, (void *)(VRAM + 0x2000));
    LZDecompressVram(gContestAudienceTilemap, (void *)(VRAM + 0xD000));
    LoadCompressedPalette(gContestInterfaceAudiencePalette, 0, 0x200);
    LoadContestPalettes();
    for (i = 0; i < 4; i++)
    {
        u32 contestantWindowId = 5 + i;

        LoadPalette(eContestTempSave.cachedWindowPalettes[contestantWindowId], 16 * (5 + gContestantTurnOrder[i]), 16 * sizeof(u16));
    }
}

static void InitContestWindows(void)
{
    Text_LoadWindowTemplate(&gWindowTemplate_81E6FD8);
    Text_InitWindowWithTemplate(&gWindowTemplate_Contest_MoveDescription, &gWindowTemplate_81E6FD8);
    Text_InitWindowWithTemplate(&gMenuWindow, &gWindowTemplate_81E6FF4);
}

static void LoadContestPalettes(void)
{
    u8 i;

    LoadFontDefaultPalette(&gWindowTemplate_81E6FD8);
    FillPalette(0, 0, 2);
    for (i = 10; i < 14; i++)
        LoadPalette(gPlttBufferUnfaded + 241, 240 + i, 2);
    FillPalette(0x7E3F, 0xF3, 2);
}

static void InitContestResources(void)
{
    s32 i;

    memset(&eContest, 0, sizeof(eContest));
    for (i = 0; i < 4; i++)
        eContest.unk19206[i] = 0xFF;

    for (i = 0; i < 4; i++)
        memset(&eContestantStatus[i], 0, sizeof(eContestantStatus[i]));
    for (i = 0; i < 4; i++)
    {
        eContestantStatus[i].ranking = 0;
        eContestantStatus[i].effectStringId = CONTEST_STRING_NONE;
        eContestantStatus[i].effectStringId2 = CONTEST_STRING_NONE;
    }

    memset(&eContestAppealResults, 0, sizeof(eContestAppealResults));
    memset(eContestAI, 0, sizeof(struct ContestAIInfo));
    memset(&eContestExcitement, 0, sizeof(eContestExcitement));
    memset(eContestGfxState, 0, 4 * sizeof(*eContestGfxState));
    if (!(gLinkContestFlags & 1))
        SortContestants(0);
    for (i = 0; i < 4; i++)
    {
        eContestantStatus[i].nextTurnOrder = 0xFF;
        eContest.prevTurnOrder[i] = gContestantTurnOrder[i];
    }
    ApplyNextTurnOrder();
}

void CB2_StartContest(void)
{
    switch (gMain.state)
    {
    case 0:
        SetVBlankCallback(NULL);
        InitContestWindows();
        SetupContestGpuRegs();
        ScanlineEffect_Clear();
        ResetPaletteFade();
        gPaletteFade.bufferTransferDisabled = TRUE;
        DmaClearLarge32(3, (void *)VRAM, VRAM_SIZE, 0x1000);
        ResetSpriteData();
        ResetTasks();
        FreeAllSpritePalettes();
        gReservedSpritePaletteCount = 4;
        //eContestTempSave.unk18000 = 0;
        eContestDebugMode = 0;
        ClearBattleMonForms();
        InitContestResources();
        gMain.state++;
        break;
    case 1:
        LoadContestPalettes();
        gMain.state++;
        break;
    case 2:
        if (SetupContestGraphics(&eContest.contestSetupState) == 0)
            break;
        eContest.contestSetupState = 0;
        gMain.state++;
        break;
    case 3:
        SetBgForCurtainDrop();
        gBattle_BG1_X = 0;
        gBattle_BG1_Y = 0;
        BeginFastPaletteFade(2);
        gPaletteFade.bufferTransferDisabled = FALSE;
        SetVBlankCallback(VBlankCB_Contest);
        eContest.mainTaskId = CreateTask(Task_StartContestWaitFade, 10);
        SetMainCallback2(CB2_ContestMain);
        break;
    }
}

static void Task_StartContestWaitFade(u8 taskId)
{
    if (!gPaletteFade.active)
        gTasks[taskId].func = Task_TryStartLinkContest;
}

static void Task_TryStartLinkContest(u8 taskId)
{
    if (gLinkContestFlags & 1)
    {
        if (!gPaletteFade.active)
        {
            gPaletteFade.bufferTransferDisabled = FALSE;
            ContestPrintLinkStandby();
            CreateTask(Task_CommunicateMonIdxs, 0);
            gTasks[taskId].func = TaskDummy1;
        }
    }
    else
    {
        gTasks[taskId].func = Task_WaitToRaiseCurtainAtStart;
    }
}

static void Task_CommunicateMonIdxs(u8 taskId)
{
    SetTaskFuncWithFollowupFunc(taskId, Task_LinkContest_CommunicateMonIdxs, Task_EndCommunicateMonIdxs);
}

static void Task_EndCommunicateMonIdxs(u8 taskId)
{
    u16 one = 1;

    gTasks[taskId].data[0] = one;
    gTasks[taskId].func = Task_ReadyStartLinkContest;
}

static void Task_ReadyStartLinkContest(u8 taskId)
{
    if (--gTasks[taskId].data[0] > 0)
        return;
    GetMultiplayerId(); // unused return value
    DestroyTask(taskId);
    gTasks[eContest.mainTaskId].func = Task_WaitToRaiseCurtainAtStart;
    gRngValue = gContestRngValue;
}

static bool8 SetupContestGraphics(u8 *stateVar)
{
    u16 tempPalette1[16];
    u16 tempPalette2[16];

    switch (*stateVar)
    {
    case 0:
        gPaletteFade.bufferTransferDisabled = TRUE;
        DmaClearLarge32(3, (void *)VRAM, VRAM_SIZE, 0x1000);
        break;
    case 1:
        LZDecompressVram(gContestInterfaceGfx, (void *)VRAM);
        break;
    case 2:
        LZDecompressVram(gContestAudienceGfx, (void *)(VRAM + 0x2000));
        DmaCopyLarge32(3, (void *)(VRAM + 0x2000), eUnzippedContestAudience_Gfx, 0x2000, 0x1000);
        break;
    case 3:
        LZDecompressVram(gContestAudienceTilemap, (void *)(VRAM + 0xD000));
        break;
    case 4:
        LZDecompressVram(gContestInterfaceTilemap, (void *)(VRAM + 0xE000));
        DmaCopy32Defvars(3, (void *)(VRAM + 0xE000), eContestTempSave.savedJunk, 0x800);
        break;
    case 5:
        LoadCompressedPalette(gContestInterfaceAudiencePalette, 0, 0x200);
        CpuCopy32(gPlttBufferUnfaded + 128, tempPalette1, 16 * sizeof(u16));
        CpuCopy32(gPlttBufferUnfaded + (5 + gContestPlayerMonIndex) * 16, tempPalette2, 16 * sizeof(u16));
        CpuCopy32(tempPalette2, gPlttBufferUnfaded + 128, 16 * sizeof(u16));
        CpuCopy32(tempPalette1, gPlttBufferUnfaded + (5 + gContestPlayerMonIndex) * 16, 16 * sizeof(u16));
        DmaCopy32Defvars(3, gPlttBufferUnfaded, eContestTempSave.cachedWindowPalettes, 0x200);
        LoadContestPalettes();
        break;
    case 6:
        DrawContestantWindows();
        FillContestantWindowBgs();
        SwapMoveDescAndContestTilemaps();
        eContest.judgeSpeechBubbleSpriteId = CreateJudgeSpeechBubbleSprite();
        CreateSliderHeartSprites();
        CreateNextTurnSprites();
        CreateApplauseMeterSprite();
        CreateJudgeAttentionEyeTask();
        CreateUnusedBlendTask();
        gBattlerPositions[0] = 0;
        gBattlerPositions[1] = 1;
        gBattlerPositions[2] = 3;
        gBattlerPositions[3] = 2;
        gBattleTypeFlags = 0;
        gBattlerAttacker = 2;
        gBattlerTarget = 3;
        gBattlerSpriteIds[gBattlerAttacker] = CreateJudgeSprite();
        CreateInvisibleBattleTargetSprite();
        break;
    default:
        *stateVar = 0;
        return 1;
    }

    (*stateVar)++;
    return 0;
}

static void Task_WaitToRaiseCurtainAtStart(u8 taskId)
{
    gPaletteFade.bufferTransferDisabled = FALSE;
    if (!gPaletteFade.active)
    {
        gTasks[taskId].data[0] = 0;
        gTasks[taskId].data[1] = 0;
        gTasks[taskId].func = Task_RaiseCurtainAtStart;
    }
}

static void Task_RaiseCurtainAtStart(u8 taskId)
{
    switch (gTasks[taskId].data[0])
    {
    case 0:
        if (gTasks[taskId].data[1]++ <= 60)
            break;
        gTasks[taskId].data[1] = 0;
        PlaySE12WithPanning(SE_CONTEST_CURTAIN_RISE, 0);
        gTasks[taskId].data[0]++;
        break;
    case 1:
        if ((s16)(gBattle_BG1_Y += 7) <= 160)
            break;
        gTasks[taskId].data[0]++;
        break;
    case 2:
        UpdateContestantBoxOrder();
        gTasks[taskId].data[0]++;
        break;
    case 3:
        REG_BG0CNT_BITFIELD.priority = 0;
        REG_BG2CNT_BITFIELD.priority = 0;
        SlideApplauseMeterIn();
        gTasks[taskId].data[0]++;
        break;
    case 4:
    default:
        if (eContest.applauseMeterIsMoving)
            break;
        gTasks[taskId].data[0] = 0;
        gTasks[taskId].data[1] = 0;
        gTasks[taskId].func = Task_DisplayAppealNumberText;
        break;
    }
}

static void CB2_ContestMain(void)
{
#if DEBUG
    if (gUnknown_020297ED == 1 && gMain.newKeys == SELECT_BUTTON)
	ContestDebugTogglePointTotal();
#endif
    AnimateSprites();
    RunTasks();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void VBlankCB_Contest(void)
{
    REG_BG0HOFS = gBattle_BG0_X;
    REG_BG0VOFS = gBattle_BG0_Y;
    REG_BG1HOFS = gBattle_BG1_X;
    REG_BG1VOFS = gBattle_BG1_Y;
    REG_BG2HOFS = gBattle_BG2_X;
    REG_BG2VOFS = gBattle_BG2_Y;
    REG_BG3HOFS = gBattle_BG3_X;
    REG_BG3VOFS = gBattle_BG3_Y;
    REG_WIN0H = gBattle_WIN0H;
    REG_WIN0V = gBattle_WIN0V;
    REG_WIN1H = gBattle_WIN1H;
    REG_WIN1V = gBattle_WIN1V;
    TransferPlttBuffer();
    LoadOam();
    ProcessSpriteCopyRequests();
    ScanlineEffect_InitHBlankDmaTransfer();
}

static void Task_DisplayAppealNumberText(u8 taskId)
{
    gBattle_BG0_Y = 0;
    gBattle_BG2_Y = 0;
    ContestDebugDoPrint();
    DmaCopy32Defvars(3, gPlttBufferUnfaded, eContestTempSave.cachedPlttBufferUnfaded, 0x400);
    if (!Contest_IsMonsTurnDisabled(gContestPlayerMonIndex))
        StringCopy(gDisplayedStringBattle, gText_AppealNumWhichMoveWillBePlayed);
    else
        StringCopy(gDisplayedStringBattle, gText_AppealNumButItCantParticipate);
    InsertStringDigit(gDisplayedStringBattle, eContest.appealNumber + 1);
    ContestClearGeneralTextWindow();
    StringExpandPlaceholders(gStringVar4, gDisplayedStringBattle);
    Contest_StartTextPrinter(&gMenuWindow, gStringVar4, 776, 1, 15);
    gTasks[taskId].func = sub_80ABC3C;
}

void sub_80ABC3C(u8 taskId)
{
    if (Contest_RunTextPrinter(&gMenuWindow) == 1)
        gTasks[taskId].func = Task_TryShowMoveSelectScreen;
}

static void Task_TryShowMoveSelectScreen(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || (gMain.newKeys == B_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (!Contest_IsMonsTurnDisabled(gContestPlayerMonIndex))
        {
            SetBottomSliderHeartsInvisibility(TRUE);
            gTasks[taskId].func = Task_ShowMoveSelectScreen;
        }
        else
        {
            gTasks[taskId].func = Task_SelectedMove;
        }
    }
}

// Print the move list
static void Task_ShowMoveSelectScreen(u8 taskId)
{
    u8 i;
    u8 moveName[32];

    gBattle_BG0_Y = 0xA0;
    gBattle_BG2_Y = 0xA0;
    Text_FillWindowRectDefPalette(
      &gWindowTemplate_Contest_MoveDescription,
      0,
      gUnknown_083CA340[0][0],
      gUnknown_083CA340[0][1],
      gUnknown_083CA340[0][2],
      gUnknown_083CA340[0][3]);

    for (i = 0; i < 4; i++)
    {
        u16 move = gContestMons[gContestPlayerMonIndex].moves[i];
        u8 *moveNameBuffer = moveName;

        if (eContestantStatus[gContestPlayerMonIndex].prevMove != MOVE_NONE
         &&
            IsContestantAllowedToCombo(gContestPlayerMonIndex)
         && AreMovesContestCombo(eContestantStatus[gContestPlayerMonIndex].prevMove, move) != 0
         && eContestantStatus[gContestPlayerMonIndex].hasJudgesAttention)
        {
            moveNameBuffer = StringCopy(moveName, gUnknownText_UnknownFormatting2);
        }
        else if (move != 0
         && eContestantStatus[gContestPlayerMonIndex].prevMove == move
         && gContestMoves[move].effect != CONTEST_EFFECT_REPETITION_NOT_BORING)
        {
            // Gray the text because it is a repeated move
            moveNameBuffer = StringCopy(moveName, gUnknownText_UnknownFormatting3);
        }
        moveNameBuffer = StringCopy(moveNameBuffer, gMoveNames[move]);

        Text_InitWindow8002E4C(
          &gWindowTemplate_Contest_MoveDescription,
          moveName,
          776 + i * 20,
          gUnknown_083CA340[i][0] * 8 + 4,
          gUnknown_083CA340[i][1] * 8,
          1);
        Text_PrintWindow8002F44(&gWindowTemplate_Contest_MoveDescription);
    }

    MenuCursor_Create814A5C0(0, 0xFFFF, 12, 0x2D9F, 72);
    DrawMoveSelectArrow(eContest.playerMoveChoice);
    PrintContestMoveDescription(gContestMons[gContestPlayerMonIndex].moves[eContest.playerMoveChoice]);
    gTasks[taskId].func = Task_HandleMoveSelectInput;
}

void debug_sub_80B9EBC(u8);

// Handle move selection input
static void Task_HandleMoveSelectInput(u8 taskId)
{
    u8 numMoves = 0;
    s32 i;

    for (i = 0; i < 4; i++)
    {
        if (gContestMons[gContestPlayerMonIndex].moves[i] != MOVE_NONE)
            numMoves++;
    }

    if (JOY_NEW(A_BUTTON))
    {
        DestroyMenuCursor();
        PlaySE(SE_SELECT);
        gTasks[taskId].func = Task_SelectedMove;
    }
    else
    {
        switch (gMain.newAndRepeatedKeys)
        {
        case B_BUTTON:
            sub_814A904();
            PlaySE(SE_SELECT);
            SetBottomSliderHeartsInvisibility(FALSE);
            Text_FillWindowRectDefPalette(
              &gWindowTemplate_Contest_MoveDescription,
              0,
              gUnknown_083CA340[0][0],
              gUnknown_083CA340[0][1],
              gUnknown_083CA340[0][2],
              gUnknown_083CA340[0][3]);
            if (!Contest_IsMonsTurnDisabled(gContestPlayerMonIndex))
                StringCopy(gDisplayedStringBattle, gText_AppealNumWhichMoveWillBePlayed);
            else
                StringCopy(gDisplayedStringBattle, gText_AppealNumButItCantParticipate);
            InsertStringDigit(gDisplayedStringBattle, eContest.appealNumber + 1);
            ContestClearGeneralTextWindow();
            StringExpandPlaceholders(gStringVar4, gDisplayedStringBattle);
            Text_InitWindowAndPrintText(&gMenuWindow, gStringVar4, 776, 1, 15);
            gBattle_BG0_Y = 0;
            gBattle_BG2_Y = 0;
            gTasks[taskId].func = Task_TryShowMoveSelectScreen;
            break;
        case DPAD_LEFT:
        case DPAD_RIGHT:
            break;
        case DPAD_UP:
            EraseMoveSelectArrow(eContest.playerMoveChoice);
            if (eContest.playerMoveChoice == 0)
                eContest.playerMoveChoice = numMoves - 1;
            else
                eContest.playerMoveChoice--;
            DrawMoveSelectArrow(eContest.playerMoveChoice);
            Contest_ClearMoveDescriptionBox();
            PrintContestMoveDescription(gContestMons[gContestPlayerMonIndex].moves[eContest.playerMoveChoice]);
            if (numMoves > 1)
                PlaySE(SE_SELECT);
            break;
        case DPAD_DOWN:
            EraseMoveSelectArrow(eContest.playerMoveChoice);
            if (eContest.playerMoveChoice == numMoves - 1)
                eContest.playerMoveChoice = 0;
            else
                eContest.playerMoveChoice++;
            DrawMoveSelectArrow(eContest.playerMoveChoice);
            Contest_ClearMoveDescriptionBox();
            PrintContestMoveDescription(gContestMons[gContestPlayerMonIndex].moves[eContest.playerMoveChoice]);
            if (numMoves > 1)
                PlaySE(SE_SELECT);
            break;
#if DEBUG
	case START_BUTTON:
	    if (gUnknown_020297ED == 1 && !(gLinkContestFlags & 1))
	    {
		gTasks[taskId].data[0] = 0;
		gTasks[taskId].data[1] = gContestMons[gContestPlayerMonIndex].moves[0];
		gTasks[taskId].func = debug_sub_80B9EBC;
	    }
	    break;
#endif
        }
    }
}

#if DEBUG

void debug_sub_80BA054(u8);

void debug_sub_80B9EBC(u8 taskId)
{
    u8 text[100];

    switch (gTasks[taskId].data[0])
    {
    case 0:
	Text_FillWindowRectDefPalette(
	  &gWindowTemplate_Contest_MoveDescription,
	  0,
	  gUnknown_083CA340[0][0],
	  gUnknown_083CA340[0][1],
	  gUnknown_083CA340[0][2],
	  gUnknown_083CA340[0][3]);
	Text_InitWindowAndPrintText(
	  &gWindowTemplate_Contest_MoveDescription,
	  gMoveNames[gTasks[taskId].data[1]],
	  776,
	  gUnknown_083CA340[0][0],
	  gUnknown_083CA340[0][1]);
	ConvertIntToDecimalStringN(text, gTasks[taskId].data[1], 2, 3);
	Text_InitWindowAndPrintText(
	  &gWindowTemplate_Contest_MoveDescription,
	  text,
	  796,
	  gUnknown_083CA340[1][0],
	  gUnknown_083CA340[1][1]);
        Contest_ClearMoveDescriptionBox();
	PrintContestMoveDescription(gTasks[taskId].data[1]);
	gTasks[taskId].data[0]++;
	break;
    case 1:
	switch (gMain.newAndRepeatedKeys)
	{
	case 0x20:
	    gTasks[taskId].data[1] -= 1;
	    if (gTasks[taskId].data[1] < 1)
		gTasks[taskId].data[1] = 354;
	    gTasks[taskId].data[0]--;
	    break;
	case 0x200:
	    gTasks[taskId].data[1] -= 10;
	    if (gTasks[taskId].data[1] < 1)
		gTasks[taskId].data[1] = 354;
	    gTasks[taskId].data[0]--;
	    break;
	case 0x10:
	    gTasks[taskId].data[1] += 1;
	    if (gTasks[taskId].data[1] > 354)
		gTasks[taskId].data[1] = 1;
	    gTasks[taskId].data[0]--;
	    break;
	case 0x100:
	    gTasks[taskId].data[1] += 10;
	    if (gTasks[taskId].data[1] > 354)
		gTasks[taskId].data[1] = 1;
	    gTasks[taskId].data[0]--;
	    break;
	case 4:
	case 8:
	    gBattle_BG0_Y = 0;
	    gBattle_BG2_Y = 0;
	    SlideApplauseMeterOut();
	    gTasks[taskId].data[0] = 0;
	    gTasks[taskId].func = debug_sub_80BA054;
	    break;
	case 2:
	    gBattle_BG0_Y = DISPLAY_HEIGHT;
	    gBattle_BG2_Y = DISPLAY_HEIGHT;
	    gContestMons[gContestPlayerMonIndex].moves[0] = gTasks[taskId].data[1];
	    gTasks[taskId].data[0] = 0;
	    gTasks[taskId].data[1] = 0;
	    gTasks[taskId].data[2] = 0;
	    gTasks[taskId].data[3] = 0;
	    gTasks[taskId].func = Task_ShowMoveSelectScreen;
	    break;
	}
	break;
    }
}

void debug_sub_80BA054(u8 taskId)
{
    s32 i;
    u8 r6;

    switch (gTasks[taskId].data[0])
    {
    case 0:
	for (i = 0; i < 4; i++)
	    gBattleMonForms[i] = 0;
	memset(&gContestResources__moveAnim, 0, sizeof(gContestResources__moveAnim));
	SetMoveAnimAttackerData(gContestPlayerMonIndex);
	r6 = CreateContestantSprite(
	  gContestMons[gContestPlayerMonIndex].species, 
	  gContestMons[gContestPlayerMonIndex].otId, 
	  gContestMons[gContestPlayerMonIndex].personality);
	gSprites[r6].x2 = 120;
	gSprites[r6].callback = SpriteCB_MonSlideIn;
	gTasks[taskId].data[2] = r6;
	gBattlerSpriteIds[gBattlerAttacker] = r6;
	gTasks[taskId].data[3] = 0;
	gTasks[taskId].data[0]++;
	eContest.moveAnimTurnCount = 0;
	break;
    case 1:
	r6 = gTasks[taskId].data[2];
	if (gSprites[r6].callback == SpriteCallbackDummy)
	{
	    eContestantStatus[gContestPlayerMonIndex].currMove = gTasks[taskId].data[1];
	    SetMoveSpecificAnimData(gContestPlayerMonIndex);
	    SetMoveAnimAttackerData(gContestPlayerMonIndex);
	    SetMoveTargetPosition(gTasks[taskId].data[1]);
	    DoMoveAnim(gTasks[taskId].data[1]);
	    gTasks[taskId].data[3] = 0;
	    gTasks[taskId].data[0]++;
	}
	break;
    case 2:
	gAnimScriptCallback();
	if (!gAnimScriptActive)
	{
	    ClearMoveAnimData(gContestPlayerMonIndex);
	    if (eContest.moveAnimTurnCount != 0)
	    {
		gTasks[taskId].data[10] = 0;
		gTasks[taskId].data[0] = 20;
	    }
	    else
	    {
		gTasks[taskId].data[0]++;
	    }
	}
	break;
    case 3:
	gTasks[taskId].data[3]++;
	if (gTasks[taskId].data[3] == 21)
	{
	    r6 = gTasks[taskId].data[2];
	    gSprites[r6].callback = SpriteCB_MonSlideOut;
	    SlideApplauseMeterIn();
	    gTasks[taskId].data[3] = 0;
	    gTasks[taskId].data[0]++;
	}
	break;
    case 4:
	r6 = gTasks[taskId].data[2];
	if (gSprites[r6].invisible)
	{
	    FreeSpriteOamMatrix(&gSprites[r6]);
	    DestroySprite(&gSprites[r6]);
	    gTasks[taskId].data[0] = 0;
	    gTasks[taskId].func = debug_sub_80B9EBC;
	    gBattle_BG0_Y = DISPLAY_HEIGHT;
	    gBattle_BG2_Y = DISPLAY_HEIGHT;
	}
	break;
    case 20:
	if (gTasks[taskId].data[10]++ > 30)
	{
	    gTasks[taskId].data[10] = 0;
	    gTasks[taskId].data[0] = 1;
	}
	break;
    }
}

#endif

static void DrawMoveSelectArrow(s8 moveIndex)
{
    MenuCursor_SetPos814A880(4, 88 + moveIndex * 16);
}

static void EraseMoveSelectArrow(s8 moveIndex)
{
}

static void Task_SelectedMove(u8 taskId)
{
    if (gLinkContestFlags & 1)
    {
        u16 move = GetChosenMove(gContestPlayerMonIndex);
        u8 taskId2;

        eContestantStatus[gContestPlayerMonIndex].currMove = move;
        taskId2 = CreateTask(Task_LinkContest_CommunicateMoveSelections, 0);
        SetTaskFuncWithFollowupFunc(taskId2, Task_LinkContest_CommunicateMoveSelections, Task_EndCommunicateMoveSelections);
        gTasks[taskId].func = TaskDummy1;
        ContestPrintLinkStandby();
        SetBottomSliderHeartsInvisibility(FALSE);
    }
    else
    {
        GetAllChosenMoves();
        gTasks[taskId].func = Task_HideMoveSelectScreen;
    }
}

static void Task_EndCommunicateMoveSelections(u8 taskId)
{
    DestroyTask(taskId);
    gTasks[eContest.mainTaskId].func = Task_HideMoveSelectScreen;
}

static void Task_HideMoveSelectScreen(u8 taskId)
{
    ContestClearGeneralTextWindow();
    gBattle_BG0_Y = 0;
    gBattle_BG2_Y = 0;
    SetBottomSliderHeartsInvisibility(FALSE);
    DmaCopy32Defvars(3, gPlttBufferFaded, eContestTempSave.cachedPlttBufferFaded, 0x400);
    LoadPalette(eContestTempSave.cachedPlttBufferUnfaded, 0, 0x400);
    gTasks[taskId].data[0] = 0;
    gTasks[taskId].data[1] = 0;
    gTasks[taskId].func = Task_HideApplauseMeterForAppealStart;
}

static void Task_HideApplauseMeterForAppealStart(u8 taskId)
{
    if (++gTasks[taskId].data[0] > 2)
    {
        gTasks[taskId].data[0] = 0;
        if (++gTasks[taskId].data[1] == 2)
        {
            SlideApplauseMeterOut();
            AnimateSliderHearts(1);
            gTasks[taskId].func = Task_WaitHideApplauseMeterForAppealStart;
        }
    }
}

static void Task_WaitHideApplauseMeterForAppealStart(u8 taskId)
{
    if (!eContest.applauseMeterIsMoving && !eContest.sliderHeartsAnimating)
        gTasks[taskId].func = Task_AppealSetup;
}

static void Task_AppealSetup(u8 taskId)
{
    if (++gTasks[taskId].data[0] > 19)
    {
        eContest.turnNumber = 0;
        eContest.unusedRng = gRngValue;
        gTasks[taskId].data[0] = 0;
        gTasks[taskId].func = Task_DoAppeals;
    }
}

static void Task_DoAppeals(u8 taskId)
{
    u8 spriteId;
    s32 i;
    u8 contestant = eContest.currentContestant;
    s8 r4;

    switch (gTasks[taskId].data[0])
    {
    case 0:
        ContestDebugDoPrint();
        for (i = 0; eContest.turnNumber != eContestAppealResults.turnOrder[i]; i++)
            ;
        eContest.currentContestant = i;
        contestant = eContest.currentContestant;
        if (gLinkContestFlags & 1)
        {
            u8 taskId2;

            eContest.waitForLink = 1;
            if (IsPlayerLinkLeader())
                CalculateAppealMoveImpact(eContest.currentContestant);
            taskId2 = CreateTask(Task_LinkContest_CommunicateAppealsState, 0);
            SetTaskFuncWithFollowupFunc(
                taskId2, Task_LinkContest_CommunicateAppealsState, Task_EndWaitForLink);
            ContestPrintLinkStandby();
            gTasks[taskId].data[0] = 1;
        }
        else
        {
            CalculateAppealMoveImpact(eContest.currentContestant);
            gTasks[taskId].data[0] = 2;
        }
        return;
    case 1:
        if (!eContest.waitForLink)
            gTasks[taskId].data[0] = 2;
        return;
    case 2:
        if (eContestantStatus[contestant].numTurnsSkipped != 0
         || eContestantStatus[contestant].noMoreTurns)
        {
            gTasks[taskId].data[0] = 31;
        }
        else
        {
            ContestClearGeneralTextWindow();
            Unused_EndBlinkingState(contestant, 0);
            gTasks[taskId].data[10] = 0;
            gTasks[taskId].data[0] = 3;
        }
        return;
    case 3:
        for (i = 0; i < 4; i++)
            gBattleMonForms[i] = 0;
        memset(&gContestResources__moveAnim, 0, sizeof(gContestResources__moveAnim));
        SetMoveAnimAttackerData(eContest.currentContestant);
        spriteId = CreateContestantSprite(
          gContestMons[eContest.currentContestant].species,
          gContestMons[eContest.currentContestant].otId,
          gContestMons[eContest.currentContestant].personality);
        gSprites[spriteId].x2 = 120;
        gSprites[spriteId].callback = SpriteCB_MonSlideIn;
        gTasks[taskId].data[2] = spriteId;
        gBattlerSpriteIds[gBattlerAttacker] = spriteId;
        BlinkContestantBox(CreateContestantBoxBlinkSprites(eContest.currentContestant), FALSE);
        gTasks[taskId].data[0] = 4;
        return;
    case 4:
        spriteId = gTasks[taskId].data[2];
        if (gSprites[spriteId].callback == SpriteCallbackDummy)
        {
            if (!eContestGfxState[contestant].boxBlinking)
                gTasks[taskId].data[0] = 5;
        }
        return;
    case 5:
        if (eContestantStatus[contestant].nervous)
        {
            gTasks[taskId].data[0] = 33;
        }
        else
        {
            ContestClearGeneralTextWindow();
            StringCopy(gStringVar1, gContestMons[contestant].nickname);
            if (eContestantStatus[contestant].currMove < MOVES_COUNT)
                StringCopy(gStringVar2, gMoveNames[eContestantStatus[contestant].currMove]);
            else
                StringCopy(gStringVar2,
                    sInvalidContestMoveNames[eContestantStatus[contestant].moveCategory]);
            StringExpandPlaceholders(gStringVar4, gText_MonAppealedWithMove);
            Contest_StartTextPrinter(&gMenuWindow, gStringVar4, 776, 1, 15);
            gTasks[taskId].data[0] = 6;
        }
        return;
    case 6:
        if (Contest_RunTextPrinter(&gMenuWindow) != 0)
        {
            eContest.moveAnimTurnCount = 0;
            gTasks[taskId].data[0] = 7;
        }
        return;
    case 7:
        {
            u16 move = SanitizeMove(eContestantStatus[eContest.currentContestant].currMove);

            SetMoveSpecificAnimData(eContest.currentContestant);
            SetMoveAnimAttackerData(eContest.currentContestant);
            SetMoveTargetPosition(move);
            DoMoveAnim(move);
            gTasks[taskId].data[0] = 8;
        }
        return;
    case 8:
        gAnimScriptCallback();
        if (!gAnimScriptActive)
        {
            ClearMoveAnimData(contestant);
            if (eContest.moveAnimTurnCount != 0)
            {
                gTasks[taskId].data[10] = 0;
                gTasks[taskId].data[0] = 9;
            }
            else
            {
                if (!eContestantStatus[contestant].hasJudgesAttention)
                    StopFlashJudgeAttentionEye(contestant);
                DrawUnnervedSymbols();
                gTasks[taskId].data[0] = 23;
            }
        }
        return;
    case 9:
        if (gTasks[taskId].data[10]++ > 30)
        {
            gTasks[taskId].data[10] = 0;
            gTasks[taskId].data[0] = 7;
        }
        return;
    case 23:
        gTasks[taskId].data[1] = 0;
        if (eContestantStatus[contestant].effectStringId != CONTEST_STRING_NONE)
        {
            PrintAppealMoveResultText(contestant, eContestantStatus[contestant].effectStringId);
            eContestantStatus[contestant].effectStringId = CONTEST_STRING_NONE;
            gTasks[taskId].data[0] = 24;
        }
        else
        {
            if (eContestantStatus[contestant].effectStringId2 != CONTEST_STRING_NONE)
            {
                for (i = 0; i < 4; i++)
                {
                    if (i != contestant && eContestantStatus[i].effectStringId != CONTEST_STRING_NONE)
                        break;
                }
                if (i == 4)
                {
                    PrintAppealMoveResultText(
                        contestant, eContestantStatus[contestant].effectStringId2);
                    eContestantStatus[contestant].effectStringId2 = CONTEST_STRING_NONE;
                    gTasks[taskId].data[0] = 24;
                }
                else
                {
                    gTasks[taskId].data[0] = 48;
                }
            }
            else
            {
                gTasks[taskId].data[0] = 48;
            }
        }
        return;
    case 24:
        if (Contest_RunTextPrinter(&gMenuWindow) != 0)
            gTasks[taskId].data[0] = 23;
        return;
    case 48:
        if (eContestantStatus[contestant].turnOrderModAction == 1)
        {
            DoJudgeSpeechBubble(5);
        }
        else if (eContestantStatus[contestant].turnOrderModAction == 2)
        {
            DoJudgeSpeechBubble(6);
        }
        else if (eContestantStatus[contestant].turnOrderModAction == 3)
        {
            DoJudgeSpeechBubble(7);
        }
        else
        {
            gTasks[taskId].data[0] = 47;
            return;
        }
        gTasks[taskId].data[0] = 49;
        return;
    case 49:
        if (!eContest.waitForJudgeSpeechBubble)
            gTasks[taskId].data[0] = 47;
        return;
    case 47:
        ShowHideNextTurnGfx(TRUE);
        gTasks[taskId].data[0] = 12;
        return;
    case 12:
        UpdateAppealHearts(0, eContestantStatus[contestant].appeal, contestant);
        gTasks[taskId].data[0] = 13;
        return;
    case 13:
        if (!eContestGfxState[eContest.currentContestant].updatingAppealHearts)
            gTasks[taskId].data[0] = 35;
        return;
    case 35:
        if (eContestantStatus[contestant].conditionMod == 1)
            DoJudgeSpeechBubble(8);
        gTasks[taskId].data[0] = 36;
        return;
    case 36:
        if (!eContest.waitForJudgeSpeechBubble)
            gTasks[taskId].data[0] = 37;
        return;
    case 37:
        if (UpdateConditionStars(contestant, 1))
        {
            gTasks[taskId].data[10] = 0;
            gTasks[taskId].data[0] = 38;
        }
        else
        {
            gTasks[taskId].data[0] = 50;
        }
        return;
    case 38:
        if (++gTasks[taskId].data[10] > 20)
        {
            gTasks[taskId].data[10] = 0;
            gTasks[taskId].data[0] = 50;
        }
        return;
    case 50:
        if (DrawStatusSymbol(contestant))
            PlaySE(SE_CONTEST_ICON_CHANGE);
        gTasks[taskId].data[0] = 25;
        return;
    case 25:
        gTasks[taskId].data[1] = 0;
        gTasks[taskId].data[0] = 26;
        return;
    case 26:
        {
            s32 j = 0;

            r4 = 0;
            for (i = gTasks[taskId].data[1]; i < 4; i++)
            {
                r4 = 0;
                for (j = 0; j < 4; j++)
                {
                    if (j != contestant && gContestantTurnOrder[j] == i
                     && eContestantStatus[j].effectStringId != CONTEST_STRING_NONE)
                    {
                        r4 = 1;
                        break;
                    }
                }
                if (r4 != 0)
                    break;
            }
            if (r4)
            {
                gTasks[taskId].data[1] = gContestantTurnOrder[j];
                PrintAppealMoveResultText(j, eContestantStatus[j].effectStringId);
                eContestantStatus[j].effectStringId = CONTEST_STRING_NONE;
                gTasks[taskId].data[0] = 27;
            }
            else
            {
                gTasks[taskId].data[1] = 0;
                gTasks[taskId].data[10] = 0;
                gTasks[taskId].data[0] = 51;
                DrawStatusSymbols();
            }
        }
        return;
    case 27:
        if (Contest_RunTextPrinter(&gMenuWindow) != 0)
            gTasks[taskId].data[0] = 28;
        return;
    case 28:
        for (i = 0; gTasks[taskId].data[1] != gContestantTurnOrder[i]; i++)
            ;
        UpdateAppealHearts(
            eContestantStatus[i].appeal + eContestantStatus[i].jam, -eContestantStatus[i].jam, i);
        gTasks[taskId].data[0] = 29;
        return;
    case 29:
        for (i = 0; gTasks[taskId].data[1] != gContestantTurnOrder[i]; i++)
            ;
        if (!eContestGfxState[i].updatingAppealHearts)
            gTasks[taskId].data[0] = 39;
        return;
    case 39:
        for (i = 0; gTasks[taskId].data[1] != gContestantTurnOrder[i]; i++)
            ;
        if (UpdateConditionStars(i, 1))
        {
            gTasks[taskId].data[10] = 0;
            gTasks[taskId].data[0] = 40;
        }
        else
        {
            gTasks[taskId].data[0] = 30;
        }
        return;
    case 40:
        if (++gTasks[taskId].data[10] > 20)
        {
            gTasks[taskId].data[10] = 0;
            gTasks[taskId].data[0] = 30;
        }
        return;
    case 30:
        for (i = 0; i < 4; i++)
        {
            if (gContestantTurnOrder[i] == gTasks[taskId].data[1])
                break;
        }
        if (DrawStatusSymbol(i))
            PlaySE(SE_CONTEST_ICON_CHANGE);
        else
            PlaySE(SE_CONTEST_ICON_CLEAR);
        if (eContestantStatus[i].judgesAttentionWasRemoved)
        {
            StopFlashJudgeAttentionEye(i);
            eContestantStatus[i].judgesAttentionWasRemoved = 0;
        }
        gTasks[taskId].data[1]++;
        gTasks[taskId].data[0] = 26;
        return;
    case 51:
        if (gTasks[taskId].data[10]++ > 9)
        {
            gTasks[taskId].data[10] = 0;
            if (eContestantStatus[contestant].numTurnsSkipped != 0
             || eContestantStatus[contestant].turnSkipped)
            {
                ContestClearGeneralTextWindow();
                StringCopy(gStringVar1, gContestMons[contestant].nickname);
                StringExpandPlaceholders(gStringVar4, gText_MonCantAppealNextTurn);
                Contest_StartTextPrinter(&gMenuWindow, gStringVar4, 776, 1, 15);
            }
            gTasks[taskId].data[0] = 52;
        }
        return;
    case 52:
        if (Contest_RunTextPrinter(&gMenuWindow) != 0)
        {
            if (!eContestantStatus[contestant].usedComboMove)
                gTasks[taskId].data[0] = 17;
            else
                gTasks[taskId].data[0] = 14;
        }
        return;
    case 14:
        r4 = eContestantStatus[contestant].completedCombo;
        if (eContestantStatus[contestant].completedCombo != 0)
        {
            ContestClearGeneralTextWindow();
            if (r4 == 1)
                Contest_StartTextPrinter(&gMenuWindow, gText_AppealComboWentOverWell, 776, 1, 15);
            else if (r4 == 2)
                Contest_StartTextPrinter(&gMenuWindow, gText_AppealComboWentOverVeryWell, 776, 1, 15);
            else
                Contest_StartTextPrinter(&gMenuWindow, gText_AppealComboWentOverExcellently, 776, 1, 15);
            DoJudgeSpeechBubble(3);
            gTasks[taskId].data[10] = 0;
            gTasks[taskId].data[0] = 45;
        }
        else
        {
            ContestClearGeneralTextWindow();
            StringCopy(gStringVar1, gContestMons[contestant].nickname);
            StringExpandPlaceholders(gStringVar4, gText_JudgeLookedAtMonExpectantly);
            Contest_StartTextPrinter(&gMenuWindow, gStringVar4, 776, 1, 15);
            DoJudgeSpeechBubble(2);
            gTasks[taskId].data[10] = 0;
            gTasks[taskId].data[0] = 45;
        }
        return;
    case 45:
        if (!eContest.waitForJudgeSpeechBubble)
        {
            StartStopFlashJudgeAttentionEye(eContest.currentContestant);
            gTasks[taskId].data[0] = 15;
        }
        return;
    case 15:
        if (Contest_RunTextPrinter(&gMenuWindow) != 0)
        {
            if (++gTasks[taskId].data[10] > 50)
            {
                if (!eContestantStatus[contestant].hasJudgesAttention)
                {
                    UpdateAppealHearts(
                        eContestantStatus[contestant].appeal, eContestantStatus[contestant].comboAppealBonus,
                        contestant);
                    eContestantStatus[contestant].appeal += eContestantStatus[contestant].comboAppealBonus;
                }
                gTasks[taskId].data[0] = 16;
            }
        }
        return;
    case 16:
        if (!eContestGfxState[contestant].updatingAppealHearts)
        {
            gTasks[taskId].data[10] = 0;
            gTasks[taskId].data[0] = 17;
        }
        return;
    case 17:
        if (eContestantStatus[contestant].repeatedMove)
        {
            ContestClearGeneralTextWindow();
            StringCopy(gStringVar1, gContestMons[contestant].nickname);
            StringExpandPlaceholders(gStringVar4, gText_RepeatedAppeal);
            Contest_StartTextPrinter(&gMenuWindow, gStringVar4, 776, 1, 15);
            gTasks[taskId].data[10] = 0;
            DoJudgeSpeechBubble(0);
            gTasks[taskId].data[0] = 46;
        }
        else
        {
            gTasks[taskId].data[0] = 41;
        }
        return;
    case 46:
        if (!eContest.waitForJudgeSpeechBubble)
            gTasks[taskId].data[0] = 19;
        return;
    case 19:
        if (Contest_RunTextPrinter(&gMenuWindow) != 0)
        {
            UpdateAppealHearts(eContestantStatus[contestant].appeal, -eContestantStatus[contestant].repeatJam,
                contestant);
            eContestantStatus[contestant].appeal -= eContestantStatus[contestant].repeatJam;
            gTasks[taskId].data[0] = 18;
        }
        return;
    case 18:
        ContestDebugDoPrint();
        if (!eContestGfxState[contestant].updatingAppealHearts)
        {
            gTasks[taskId].data[10] = 0;
            ContestClearGeneralTextWindow();
            gTasks[taskId].data[0] = 41;
        }
        return;
    case 41:
        if (eContestExcitement.frozen &&
            contestant != eContestExcitement.freezer)
        {
            gTasks[taskId].data[0] = 57;
        }
        else
        {
            r4 = eContestExcitement.moveExcitement;
            if (eContestantStatus[contestant].overrideCategoryExcitementMod)
            {
                r4 = 1;
                StringCopy(gStringVar3, gMoveNames[eContestantStatus[contestant].currMove]);
            }
            else
            {
                StringCopy(gStringVar3, gContestCategoryStrings[gContestMoves[eContestantStatus[contestant].currMove].contestCategory]);
            }
            if (r4 > 0)
            {
                if (eContestantStatus[contestant].repeatedMove)
                    r4 = 0;
            }
            ContestClearGeneralTextWindow();
            StringCopy(gStringVar1, gContestMons[contestant].nickname);
            eContest.applauseLevel += r4;
            if (eContest.applauseLevel < 0)
                eContest.applauseLevel = 0;
            if (r4 == 0)
            {
                gTasks[taskId].data[0] = 55;
            }
            else
            {
                if (r4 < 0)
                    StringExpandPlaceholders(gStringVar4, gText_MonsXDidntGoOverWell);
                else if (r4 > 0 && eContest.applauseLevel <= 4)
                    StringExpandPlaceholders(gStringVar4, gText_MonsXWentOverGreat);
                else
                    StringExpandPlaceholders(gStringVar4, gText_MonsXGotTheCrowdGoing);
                Contest_StartTextPrinter(&gMenuWindow, gStringVar4, 776, 1, 15);
                gTasks[taskId].data[10] = 0;
                gTasks[taskId].data[11] = 0;
                if (r4 < 0)
                    gTasks[taskId].data[0] = 53;
                else
                    gTasks[taskId].data[0] = 54;
            }
        }
        return;
    case 53:
        switch (gTasks[taskId].data[10])
        {
        case 0:
            BlendAudienceBackground(-1, 1);
            PlayFanfare(MUS_TOO_BAD);
            gTasks[taskId].data[10]++;
            break;
        case 1:
            if (!eContest.waitForAudienceBlend && Contest_RunTextPrinter(&gMenuWindow) != 0)
            {
                ShowAndUpdateApplauseMeter(-1);
                gTasks[taskId].data[10]++;
            }
            break;
        case 2:
            if (!eContest.isShowingApplauseMeter)
            {
                if (gTasks[taskId].data[11]++ > 29)
                {
                    gTasks[taskId].data[11] = 0;
                    BlendAudienceBackground(-1, -1);
                    gTasks[taskId].data[10]++;
                }
            }
            break;
        case 3:
            if (!gPaletteFade.active)
            {
                gTasks[taskId].data[10] = 0;
                gTasks[taskId].data[11] = 0;
                gTasks[taskId].data[0] = 43;
            }
            break;
        }
        return;
    case 54:
        switch (gTasks[taskId].data[10])
        {
        case 0:
            if (Contest_RunTextPrinter(&gMenuWindow) != 0)
            {
                BlendAudienceBackground(1, 1);
                gTasks[taskId].data[10]++;
            }
            break;
        case 1:
            if (!eContest.waitForAudienceBlend)
            {
                AnimateAudience();
                PlaySE(SE_M_ENCORE2);
                ShowAndUpdateApplauseMeter(1);
                gTasks[taskId].data[10]++;
            }
            break;
        case 2:
            if (!eContest.isShowingApplauseMeter)
            {
                if (gTasks[taskId].data[11]++ > 29)
                {
                    gTasks[taskId].data[11] = 0;
                    UpdateAppealHearts(
                        eContestantStatus[contestant].appeal, eContestExcitement.excitementAppealBonus,
                        contestant);
                    eContestantStatus[contestant].appeal += eContestExcitement.excitementAppealBonus;
                    gTasks[taskId].data[10]++;
                }
            }
            break;
        case 3:
            if (!eContestGfxState[contestant].updatingAppealHearts)
            {
                if (!eContest.animatingAudience)
                {
                    BlendAudienceBackground(1, -1);
                    gTasks[taskId].data[10]++;
                }
            }
            break;
        case 4:
            if (!gPaletteFade.active)
            {
                gTasks[taskId].data[10] = 0;
                gTasks[taskId].data[11] = 0;
                gTasks[taskId].data[0] = 43;
            }
            break;
        }
        return;
    case 43:
        if (!eContestGfxState[contestant].updatingAppealHearts)
        {
            ContestClearGeneralTextWindow();
            gTasks[taskId].data[0] = 55;
        }
        return;
    case 57:
        ContestClearGeneralTextWindow();
        StringCopy(gStringVar3, gContestMons[eContestExcitement.freezer].nickname);
        StringCopy(gStringVar1, gContestMons[contestant].nickname);
        StringCopy(gStringVar2, gMoveNames[eContestantStatus[contestant].currMove]);
        StringExpandPlaceholders(gStringVar4, gText_CrowdContinuesToWatchMon);
        Contest_StartTextPrinter(&gMenuWindow, gStringVar4, 776, 1, 15);
        gTasks[taskId].data[0] = 58;
        return;
    case 58:
        if (Contest_RunTextPrinter(&gMenuWindow) != 0)
        {
            ContestClearGeneralTextWindow();
            StringExpandPlaceholders(gStringVar4, gText_MonsMoveIsIgnored);
            Contest_StartTextPrinter(&gMenuWindow, gStringVar4, 776, 1, 15);
            gTasks[taskId].data[0] = 59;
        }
        return;
    case 59:
        if (Contest_RunTextPrinter(&gMenuWindow) != 0)
        {
            ContestClearGeneralTextWindow();
            gTasks[taskId].data[0] = 55;
        }
        return;
    case 33:
        if (eContestantStatus[contestant].hasJudgesAttention)
            eContestantStatus[contestant].hasJudgesAttention = 0;
        StartStopFlashJudgeAttentionEye(contestant);
        StringCopy(gStringVar1, gContestMons[contestant].nickname);
        StringCopy(gStringVar2, gMoveNames[eContestantStatus[contestant].currMove]);
        StringExpandPlaceholders(gStringVar4, gText_MonWasTooNervousToMove);
        Contest_StartTextPrinter(&gMenuWindow, gStringVar4, 776, 1, 15);
        gTasks[taskId].data[0] = 34;
        return;
    case 34:
        if (Contest_RunTextPrinter(&gMenuWindow) != 0)
            gTasks[taskId].data[0] = 55;
        return;
    case 55:
        SlideApplauseMeterOut();
        gTasks[taskId].data[0] = 56;
        return;
    case 56:
        if (!eContest.applauseMeterIsMoving)
        {
            if (eContest.applauseLevel > 4)
            {
                eContest.applauseLevel = 0;
                UpdateApplauseMeter();
            }
            gTasks[taskId].data[0] = 10;
        }
        return;
    case 10:
        spriteId = gTasks[taskId].data[2];
        gSprites[spriteId].callback = SpriteCB_MonSlideOut;
        gTasks[taskId].data[0] = 11;
        return;
    case 11:
        spriteId = gTasks[taskId].data[2];
        if (gSprites[spriteId].invisible)
        {
            FreeSpriteOamMatrix(&gSprites[spriteId]);
            DestroySprite(&gSprites[spriteId]);
            gTasks[taskId].data[0] = 20;
        }
        return;
    case 20:
        for (i = 0; i < 4; i++)
            Unused_EndBlinkingState(i, 1);
        gTasks[taskId].data[10] = 0;
        gTasks[taskId].data[0] = 21;
        return;
    case 31:
        ContestClearGeneralTextWindow();
        StringCopy(gStringVar1, gContestMons[contestant].nickname);
        StringExpandPlaceholders(gStringVar4, gText_MonWasWatchingOthers);
        Contest_StartTextPrinter(&gMenuWindow, gStringVar4, 776, 1, 15);
        gTasks[taskId].data[0] = 32;
        return;
    case 32:
        if (Contest_RunTextPrinter(&gMenuWindow) != 0)
            gTasks[taskId].data[0] = 21;
        return;
    case 21:
        if (++gTasks[taskId].data[10] > 29)
        {
            gTasks[taskId].data[10] = 0;
            gTasks[taskId].data[0] = 22;
        }
        return;
    case 22:
        if (++eContest.turnNumber == 4)
        {
            gTasks[taskId].data[0] = 0;
            gTasks[taskId].data[1] = 0;
            gTasks[taskId].data[2] = 0;
            gTasks[taskId].func = Task_FinishRoundOfAppeals;
        }
        else
        {
            gTasks[taskId].data[0] = 0;
        }
        nullsub_18(0);
        return;
    }
}

void Task_EndWaitForLink(u8 taskId)
{
    eContest.waitForLink = 0;
    DestroyTask(taskId);
}

static void SpriteCB_MonSlideIn(struct Sprite *sprite)
{
    if (sprite->x2 != 0)
    {
        sprite->x2 -= 2;
    }
    else
    {
        if (++sprite->data[0] == 31)
        {
            sprite->data[0] = 0;
            sprite->callback = SpriteCallbackDummy;
        }
    }
}

static void SpriteCB_MonSlideOut(struct Sprite *sprite)
{
    sprite->x2 -= 6;
    if (sprite->x + sprite->x2 < -32)
    {
        sprite->callback = SpriteCallbackDummy;
        sprite->invisible = TRUE;
    }
}

static void Task_FinishRoundOfAppeals(u8 taskId)
{
    switch (gTasks[taskId].data[0])
    {
    case 0:
        if (gLinkContestFlags & 1)
        {
            u8 taskId2;

            eContest.waitForLink = 1;
            if (IsPlayerLinkLeader())
            {
                RankContestants();
                SetAttentionLevels();
            }
            taskId2 = CreateTask(Task_LinkContest_CommunicateAppealsState, 0);
            SetTaskFuncWithFollowupFunc(
                taskId2, Task_LinkContest_CommunicateAppealsState, Task_EndWaitForLink);
            ContestPrintLinkStandby();
            gTasks[taskId].data[0] = 1;
        }
        else
        {
            RankContestants();
            SetAttentionLevels();
            gTasks[taskId].data[0] = 2;
        }
        break;
    case 1:
        if (!eContest.waitForLink)
            gTasks[taskId].data[0] = 2;
        break;
    case 2:
        gTasks[taskId].data[0] = 0;
        gTasks[taskId].func = Task_ReadyUpdateHeartSliders;
        break;
    }
}

static void Task_ReadyUpdateHeartSliders(u8 taskId)
{
    ShowHideNextTurnGfx(FALSE);
    gTasks[taskId].data[0] = 0;
    gTasks[taskId].data[1] = 0;
    gTasks[taskId].func = Task_UpdateHeartSliders;
}

static void Task_UpdateHeartSliders(u8 taskId)
{
    switch (gTasks[taskId].data[0])
    {
    case 0:
        if (++gTasks[taskId].data[1] > 20)
        {
            AnimateSliderHearts(2);
            gTasks[taskId].data[1] = 0;
            gTasks[taskId].data[0]++;
        }
        break;
    case 1:
        if (!eContest.sliderHeartsAnimating)
        {
            if (++gTasks[taskId].data[1] > 20)
            {
                gTasks[taskId].data[1] = 0;
                gTasks[taskId].data[0]++;
            }
        }
        break;
    case 2:
        UpdateHeartSliders();
        gTasks[taskId].data[0] = 0;
        gTasks[taskId].data[1] = 0;
        gTasks[taskId].func = Task_WaitForHeartSliders;
        break;
    }
}

static void Task_WaitForHeartSliders(u8 taskId)
{
    if (SlidersDoneUpdating())
        gTasks[taskId].func = Task_RestorePlttBufferUnfaded;
}

static void Task_RestorePlttBufferUnfaded(u8 taskId)
{
    DmaCopy32Defvars(3, eContestTempSave.cachedPlttBufferUnfaded, gPlttBufferUnfaded, 0x400);
    gTasks[taskId].data[0] = 0;
    gTasks[taskId].data[1] = 2;
    gTasks[taskId].func = Task_WaitPrintRoundResult;
}

static void Task_WaitPrintRoundResult(u8 taskId)
{
    if (++gTasks[taskId].data[0] > 2)
    {
        gTasks[taskId].data[0] = 0;
        if (--gTasks[taskId].data[1] == 0)
            gTasks[taskId].func = Task_PrintRoundResultText;
    }
}

static void Task_PrintRoundResultText(u8 taskId)
{
    u8 attention = eContestantStatus[gContestPlayerMonIndex].attentionLevel;

    ContestClearGeneralTextWindow();
    StringCopy(gStringVar1, gContestMons[gContestPlayerMonIndex].nickname);
    StringExpandPlaceholders(gStringVar4, gContestStandOutStrings[attention]);
    Contest_StartTextPrinter(&gMenuWindow, gStringVar4, 776, 1, 15);
    gTasks[taskId].func = Task_WaitPrintRoundResultText;
}

void Task_WaitPrintRoundResultText(u8 taskId)
{
    if (Contest_RunTextPrinter(&gMenuWindow) == 1)
    {
        gTasks[taskId].data[0] = 0;
        gTasks[taskId].func = Task_ReUpdateHeartSliders;
        ContestDebugDoPrint();
    }
}

static void Task_ReUpdateHeartSliders(u8 taskId)
{
    if (gTasks[taskId].data[0]++ > 29)
    {
        gTasks[taskId].data[0] = 0;
        UpdateHeartSliders();
        gTasks[taskId].func = Task_WaitForHeartSlidersAgain;
    }
}

static void Task_WaitForHeartSlidersAgain(u8 taskId)
{
    if (SlidersDoneUpdating())
    {
        gTasks[taskId].data[0] = 0;
        gTasks[taskId].func = Task_DropCurtainAtRoundEnd;
    }
}

static void Task_DropCurtainAtRoundEnd(u8 taskId)
{
    SetBgForCurtainDrop();
    gTasks[taskId].func = Task_StartDropCurtainAtRoundEnd;
}

void Task_UpdateContestantBoxOrder(u8 taskId)
{
    UpdateContestantBoxOrder();
    gTasks[taskId].func = Task_TryStartNextRoundOfAppeals;
}

static void Task_TryStartNextRoundOfAppeals(u8 taskId)
{
    REG_BG0CNT_BITFIELD.priority = 0;
    REG_BG2CNT_BITFIELD.priority = 0;
    if (++eContest.appealNumber == 5)
    {
        gTasks[taskId].func = Task_EndAppeals;
    }
    else
    {
        SlideApplauseMeterIn();
        gTasks[taskId].func = Task_StartNewRoundOfAppeals;
    }
}

static void Task_StartNewRoundOfAppeals(u8 taskId)
{
    if (!eContest.applauseMeterIsMoving)
        gTasks[taskId].func = Task_DisplayAppealNumberText;
}

static void Task_EndAppeals(u8 taskId)
{
    s32 i;

    gBattle_BG0_Y = 0;
    gBattle_BG2_Y = 0;
    for (i = 0; i < 4; i++)
        gContestMonAppealPointTotals[i] = eContestantStatus[i].pointTotal;
    CalculateFinalScores();
    ContestClearGeneralTextWindow();
    BravoTrainerPokemonProfile_BeforeInterview1(eContestantStatus[gContestPlayerMonIndex].prevMove);
    gContestRngValue = gRngValue;
    StringExpandPlaceholders(gStringVar4, gText_AllOutOfAppealTime);
    Contest_StartTextPrinter(&gMenuWindow, gStringVar4, 776, 1, 15);
    gTasks[taskId].data[2] = 0;
    gTasks[taskId].func = Task_WaitForOutOfTimeMsg;
}

static void Task_WaitForOutOfTimeMsg(u8 taskId)
{
    if (Contest_RunTextPrinter(&gMenuWindow) == 1)
    {
        SetBgForCurtainDrop();
        gBattle_BG1_X = 0;
        gBattle_BG1_Y = 160;
        PlaySE12WithPanning(SE_CONTEST_CURTAIN_FALL, 0);
        gTasks[taskId].data[0] = 0;
        gTasks[taskId].func = Task_DropCurtainAtAppealsEnd;
    }
}

static void Task_DropCurtainAtAppealsEnd(u8 taskId)
{
    if ((s16)(gBattle_BG1_Y -= 7) < 0)
        gBattle_BG1_Y = 0;
    if (gBattle_BG1_Y == 0)  // Why cast?
    {
        gTasks[taskId].func = Task_TryCommunicateFinalStandings;
        gTasks[taskId].data[0] = 0;
    }
}

static void Task_TryCommunicateFinalStandings(u8 taskId)
{
    if (gTasks[taskId].data[0]++ >= 50)
    {
        gTasks[taskId].data[0] = 0;
        if (gLinkContestFlags & 1)
        {
            gTasks[taskId].func = Task_CommunicateFinalStandings;
        }
        else
        {
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            gTasks[taskId].func = Task_ContestReturnToField;
        }
    }
}

static void Task_CommunicateFinalStandings(u8 taskId)
{
    u8 taskId2 = CreateTask(Task_LinkContest_CommunicateFinalStandings, 0);

    SetTaskFuncWithFollowupFunc(
        taskId2, Task_LinkContest_CommunicateFinalStandings, Task_EndCommunicateFinalStandings);
    gTasks[taskId].func = TaskDummy1;
    ContestPrintLinkStandby();
    SetBottomSliderHeartsInvisibility(FALSE);
}

static void Task_EndCommunicateFinalStandings(u8 taskId)
{
    DestroyTask(taskId);
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gTasks[eContest.mainTaskId].func = Task_ContestReturnToField;
}

static void Task_ContestReturnToField(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        DestroyTask(taskId);
        gFieldCallback = FieldCB_ContestReturnToField;
        SetMainCallback2(CB2_ReturnToField);
    }
}

static void FieldCB_ContestReturnToField(void)
{
    // Why disable and re-enable again?
    UnlockPlayerFieldControls();
    ScriptContext_Enable();
}

// Expands a one-digit number in the string
void InsertStringDigit(u8 *str, s32 n)
{
    while (1)
    {
        if (*str == EOS)
            break;
        // Find the EXT_CTRL_CODE_BEGIN, 0 sequence, and place the number there
        if (*str == EXT_CTRL_CODE_BEGIN && *(str + 1) == 0)
        {
            u8 *end;

            end = ConvertIntToDecimalStringN(str, n, STR_CONV_MODE_LEFT_ALIGN, 1);
            *end = EOS;  // ConvertIntToDecimalStringN already does this.
            StringAppend(end, str + 2);  // Shift the rest of the string over.
            break;
        }
        str++;
    }
}

// File boundary
