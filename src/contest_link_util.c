#include "global.h"
#include "constants/rgb.h"
#include "contest_link_util.h"
#include "contest_util.h"
#include "battle.h"
#include "constants/songs.h"
#include "contest.h"
#include "contest_link.h"
#include "data.h"
#include "decompress.h"
#include "event_data.h"
#include "ewram.h"
#include "field_effect.h"
#include "field_specials.h"
#include "graphics.h"
#include "link.h"
#include "main.h"
#include "menu.h"
#include "overworld.h"
#include "palette.h"
#include "pokedex.h"
#include "pokemon_icon.h"
#include "pokemon_storage_system.h"
#include "random.h"
#include "scanline_effect.h"
#include "script.h"
#include "sound.h"
#include "string_util.h"
#include "strings.h"
#include "text.h"
#include "trig.h"
#include "tv.h"
#include "util.h"
#define ABS(x) ((x) < 0 ? -(x) : (x))

#define GET_CONTEST_WINNER_ID(var) {           \
    for ((var) = 0; (var) < 4; (var)++)     \
    {                                       \
        if (gContestFinalStandings[i] == 0) \
            break;                          \
    }                                       \
}

struct ContestResultsInternal {
    u8 slidingTextBoxSpriteId;
    u8 linkTextBoxSpriteId;
    u8 showResultsTaskId;
    u8 highlightWinnerTaskId;
    u8 slidingTextBoxState;
    u8 numStandingsPrinted;
    u8 winnerMonSlidingState;
    u8 confettiCount;
    u8 winnerMonSpriteId;
    u8 destroyConfetti;
    u8 pointsFlashing;
    s16 barLength[4];
    u8 numBarsUpdating;
};

struct ContestMonResults {
    s32 relativePreliminaryPoints;
    s32 relativeRound2Points;
    u32 barLengthPreliminary;
    u32 barLengthRound2;
    u8 lostPoints;
    u8 numStars;
    u8 numHearts;
};

#define eContestLink80C2020Struct2018000 (*(struct ContestResultsInternal *)(gSharedMem + 0x18000))
#define eContestLink80C2020Struct2018018 ((struct ContestMonResults *)(gSharedMem + 0x18018))
#define eContestLink80C2020Struct2018068 (gSharedMem + 0x18068)

static void CB2_ShowContestResults(void);
static void VBlankCB_ShowContestResults(void);
static void Task_ShowContestResults(u8 taskId);
static void Task_WaitForLinkPartnersBeforeResults(u8 taskId);
static void Task_CommunicateMonIdxsForResults(u8 taskId);
static void Task_WaitForLinkPartnerMonIdxs(u8 taskId);
static void Task_AnnouncePreliminaryResults(u8 taskId);
static void Task_ShowPreliminaryResults(u8 taskId);
static void Task_AnnounceRound2Results(u8 taskId);
static void Task_ShowRound2Results(u8 taskId);
static void Task_AnnounceWinner(u8 taskId);
static void Task_ShowWinnerMonBanner(u8 taskId);
static void Task_SetSeenWinnerMon(u8 taskId);
static void Task_TryDisconnectLinkPartners(u8 taskId);
static void Task_WaitForLinkPartnersDisconnect(u8 taskId);
static void Task_TrySetContestInterviewData(u8 taskId);
static void Task_EndShowContestResults(u8 taskId);
static void Task_SlideContestResultsBg(u8 taskId);
static void Task_FlashStarsAndHearts(u8 taskId);
static void LoadAllContestMonIcons(u8 srcOffset, bool8 useDmaNow);
static void LoadAllContestMonIconPalettes(void);
static void DrawResultsTextWindow(const u8 * text, u8 spriteId);
static void CreateResultsTextWindowSprites(void);
u16 sub_80C34AC(const u8 * string);
static void StartTextBoxSlideIn(s16 x, u16 y, u16 slideOutTimer, u16 slideIncrement);
static void StartTextBoxSlideOut(u16 slideIncrement);
static void SpriteCB_TextBoxSlideIn(struct Sprite *sprite);
static void SpriteCB_EndTextBoxSlideIn(struct Sprite *sprite);
static void SpriteCB_TextBoxSlideOut(struct Sprite *sprite);
static void ShowLinkResultsTextBox(const u8 *text);
static void HideLinkResultsTextBox(void);
static void LoadContestResultsTitleBarTilemaps(void);
static u8 GetNumPreliminaryPoints(u8 monIndex, u8 capPoints);
static s8 GetNumRound2Points(u8 monIndex, u8 capPoints);
static void Task_DrawFinalStandingNumber(u8 taskId);
void Task_HighlightWinnersBox(u8 taskId);
static void Task_StartHighlightWinnersBox(u8 taskId);
static void SpriteCB_WinnerMonSlideIn(struct Sprite *sprite);
static void SpriteCB_WinnerMonSlideOut(struct Sprite *sprite);
static void Task_CreateConfetti(u8 taskId);
static void SpriteCB_Confetti(struct Sprite *sprite);
static void BounceMonIconInBox(u8 monIndex, u8 numFrames);
static void Task_BounceMonIconInBox(u8 taskId);
static void CalculateContestantsResultData(void);
static void UpdateContestResultBars(u8 isRound2, u8 numUpdates);
void Task_UpdateContestResultBar(u8 taskId /*r12*/);
static void Task_StartCommunication(u8 taskId);
static void Task_StartCommunicateRngRS(u8 taskId);
static void Task_StartCommunicateLeaderIdsRS(u8 taskId);
static void Task_StartCommunicateCategoryRS(u8 taskId);
static void Task_LinkContest_SetUpContestRS(u8 taskId);
static void Task_LinkContest_CalculateTurnOrderRS(u8 taskId);
static void Task_LinkContest_Disconnect(u8 taskId);
static void Task_LinkContest_WaitDisconnect(u8 taskId);

const u16 gUnknown_083D1624[] = INCBIN_U16("graphics/unknown/unknown_3D1624/0.4bpp");
const u16 gUnknown_083D1644[] = INCBIN_U16("graphics/unknown/unknown_3D1624/1.4bpp");
const u16 gUnknown_083D1664[] = INCBIN_U16("graphics/unknown/unknown_3D1624/2.4bpp");
const u16 gUnknown_083D1684[] = INCBIN_U16("graphics/unknown/unknown_3D1624/3.4bpp");
const u16 gUnknown_083D16A4[] = INCBIN_U16("graphics/unknown/unknown_3D1624/4.4bpp");
const u16 gUnknown_083D16C4[] = INCBIN_U16("graphics/unknown/unknown_3D1624/5.4bpp");
const u16 gUnknown_083D16E4[] = INCBIN_U16("graphics/unknown/unknown_3D1624/6.4bpp");
const u16 gUnknown_083D1704[] = INCBIN_U16("graphics/unknown/unknown_3D1624/7.4bpp");
static const u16 sMiscBlank_Pal[] = INCBIN_U16("graphics/interface/blank.gbapal");

static const struct OamData sOamData_ResultsTextWindow = {
    .shape = ST_OAM_H_RECTANGLE,
    .size = 3,
    .priority = 3,
    .paletteNum = 2
};

static const struct SpriteTemplate sSpriteTemplate_ResultsTextWindow = {
    0xbc1,
    0xbc1,
    &sOamData_ResultsTextWindow,
    gDummySpriteAnimTable,
    NULL,
    gDummySpriteAffineAnimTable,
    SpriteCallbackDummy
};

static const struct SpriteSheet sSpriteSheets_ResultsTextWindow[] = {
    {gMiscBlank_Gfx, 0x400, 0xbc1},
    {gMiscBlank_Gfx, 0x400, 0xbc2},
    {gMiscBlank_Gfx, 0x400, 0xbc3},
    {gMiscBlank_Gfx, 0x400, 0xbc4},
    {gMiscBlank_Gfx, 0x400, 0xbc5},
    {gMiscBlank_Gfx, 0x400, 0xbc6},
    {gMiscBlank_Gfx, 0x400, 0xbc7},
    {gMiscBlank_Gfx, 0x400, 0xbc8},
};

static const struct SpritePalette sSpritePalette_ResultsTextWindow = {
    sMiscBlank_Pal, 0xbc1
};

static const struct OamData sOamData_Confetti = {};

static const struct SpriteTemplate sSpriteTemplate_Confetti = {
    0xbc9,
    0xbc9,
    &sOamData_Confetti,
    gDummySpriteAnimTable,
    NULL,
    gDummySpriteAffineAnimTable,
    SpriteCB_Confetti
};

static const struct CompressedSpriteSheet sSpriteSheet_Confetti = {gConfetti_Gfx, 0x220, 0xbc9};

static const struct CompressedSpritePalette sSpritePalette_Confetti = {gConfetti_Pal, 0xbc9};

const u8 gUnknown_083D17DC[] = _("{COLOR RED}");
const u8 gUnknown_083D17E0[] = _("/");
const u8 gUnknown_083D17E2[] = _("{SIZE 3}{COLOR_HIGHLIGHT_SHADOW WHITE2 DARK_GRAY LIGHT_BLUE}");

static void InitContestResultsDisplay(void)
{
    REG_DISPCNT = DISPCNT_OBJ_1D_MAP;
    Text_LoadWindowTemplate(&gWindowTemplate_81E6FA0);
    Text_InitWindowWithTemplate(&gMenuWindow, &gWindowTemplate_81E6FA0);
    REG_BG0CNT = BGCNT_WRAP | BGCNT_SCREENBASE(30);
    REG_BG1CNT = BGCNT_PRIORITY(3) | BGCNT_SCREENBASE(24);
    REG_BG2CNT = BGCNT_PRIORITY(3) | BGCNT_SCREENBASE(28);
    REG_BG3CNT = BGCNT_WRAP | BGCNT_PRIORITY(3) | BGCNT_SCREENBASE(26);
    REG_MOSAIC = 0;
    REG_WININ = 0x3f3f;
    REG_WINOUT = 0x3f2e;
    REG_WIN0H = 0;
    REG_WIN0V = 0;
    REG_WIN1H = 0;
    REG_WIN1V = 0;
    REG_BLDCNT = 0;
    REG_BLDALPHA = 0;
    REG_BLDY = 0;
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;
    REG_BG1HOFS = 0;
    REG_BG1VOFS = 0;
    REG_BG2HOFS = 0;
    REG_BG2VOFS = 0;
    REG_BG3HOFS = 0;
    REG_BG3VOFS = 0;
    REG_DISPCNT |= DISPCNT_BG_ALL_ON | DISPCNT_OBJ_ON | DISPCNT_WIN0_ON | DISPCNT_WIN1_ON;
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

static void LoadContestResultsBgGfx(void)
{
    int i;
    int j;
    s8 numStars;
    s8 round2Points;
    u16 tile1;
    u16 tile2;

    DmaFill32Large(3, 0, VRAM, VRAM_SIZE, 0x1000);
    LZDecompressVram(gContestResults_Gfx, BG_SCREEN_ADDR(0));
    LZDecompressVram(gContestResults_Bg_Tilemap, BG_SCREEN_ADDR(26));
    LZDecompressVram(gContestResults_Interface_Tilemap, BG_SCREEN_ADDR(28));
    LZDecompressVram(gContestResults_WinnerBanner_Tilemap, BG_SCREEN_ADDR(30));
    LoadContestResultsTitleBarTilemaps();
    LoadCompressedPalette(gContestResults_Pal, 0, 0x200);
    LoadFontDefaultPalette(&gWindowTemplate_81E6FA0);
    for (i = 0; i < 4; i++)
    {
        numStars = GetNumPreliminaryPoints(i, 1);
        round2Points = GetNumRound2Points(i, 1);
        for (j = 0; j < 10; j++)
        {
            tile1 = 0x60b2;
            if (j < numStars)
                tile1 = 0x60b4;
            if (j < ABS(round2Points))
            {
                tile2 = 0x60a4;
                if (round2Points < 0)
                    tile2 = 0x60a6;
            }
            else
                tile2 = 0x60a2;
            ((u16 *)BG_VRAM)[i * 0x60 + j + 0x60b3] = tile1;
            ((u16 *)BG_VRAM)[i * 0x60 + j + 0x60d3] = tile2;
        }
    }
}

static void LoadContestMonName(u8 monIndex)
{
    u8 *str;

    if (monIndex == gContestPlayerMonIndex)
        str = StringCopy(gDisplayedStringBattle, gUnknown_083D17DC);
    else
        str = gDisplayedStringBattle;
    str[0] = EXT_CTRL_CODE_BEGIN;
    str[1] = 0x06;
    str[2] = 0x04;
    str += 3;
    str = StringCopy(str, gContestMons[monIndex].nickname);
    str[0] = EXT_CTRL_CODE_BEGIN;
    str[1] = 0x13;
    str[2] = 0x32;
    str += 3;
    str = StringCopy(str, gUnknown_083D17E0);
    if (gLinkContestFlags & 1)
        StringCopy(str, gLinkPlayers[monIndex].name);
    else
        StringCopy(str, gContestMons[monIndex].trainerName);
    Text_InitWindowAndPrintText(&gMenuWindow, gDisplayedStringBattle, monIndex * 36 + 770, 7, monIndex * 3 + 4);
}

static void LoadAllContestMonNames(void)
{
    int i;

    for (i = 0; i < 4; i++)
        LoadContestMonName(i);
}

void CB2_StartShowContestResults(void)
{
    gPaletteFade.bufferTransferDisabled = TRUE;
    SetVBlankCallback(NULL);
    InitContestResultsDisplay();
    ScanlineEffect_Clear();
    ResetPaletteFade();
    ResetSpriteData();
    ResetTasks();
    FreeAllSpritePalettes();
    LoadContestResultsBgGfx();
    LoadAllContestMonIconPalettes();
    LoadAllContestMonIcons(0, TRUE);
    LoadAllContestMonNames();
    eContestLink80C2020Struct2018000 = (struct ContestResultsInternal){};
    memset(eContestLink80C2020Struct2018018, 0, 4 * sizeof(struct ContestMonResults));
    CreateResultsTextWindowSprites();
    BeginNormalPaletteFade(0xffffffff, 0, 16, 0, 0);
    gPaletteFade.bufferTransferDisabled = FALSE;
    eContestLink80C2020Struct2018000.showResultsTaskId = CreateTask(Task_ShowContestResults, 5);
    SetMainCallback2(CB2_ShowContestResults);
    gBattle_WIN1H = 0xf0;
    gBattle_WIN1V = 0x80a0;
    CreateTask(Task_SlideContestResultsBg, 20);
    CalculateContestantsResultData();
    PlayBGM(MUS_CONTEST_RESULTS);
    SetVBlankCallback(VBlankCB_ShowContestResults);
}

static void CB2_ShowContestResults(void)
{
    AnimateSprites();
    BuildOamBuffer();
    RunTasks();
    UpdatePaletteFade();
}

static void VBlankCB_ShowContestResults(void)
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
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    ScanlineEffect_InitHBlankDmaTransfer();
}

static void Task_ShowContestResults(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        if (gLinkContestFlags & 1)
        {
            ShowLinkResultsTextBox(gText_CommunicationStandby);
            gTasks[taskId].func = Task_WaitForLinkPartnersBeforeResults;
        }
        else
        {
            gTasks[taskId].func = Task_AnnouncePreliminaryResults;
        }
    }
}

static void Task_WaitForLinkPartnersBeforeResults(u8 taskId)
{
    if (gReceivedRemoteLinkPlayers && GetLinkPlayerCount() == MAX_LINK_PLAYERS)
    {
        CreateTask(Task_CommunicateMonIdxsForResults, 0);
        gTasks[taskId].func = TaskDummy;
    }
}

static void Task_CommunicateMonIdxsForResults(u8 taskId)
{
    SetTaskFuncWithFollowupFunc(taskId, Task_LinkContest_CommunicateMonIdxs, Task_WaitForLinkPartnerMonIdxs);
}

static void Task_WaitForLinkPartnerMonIdxs(u8 taskId)
{
    if (IsLinkTaskFinished())
    {
        DestroyTask(taskId);
        gTasks[eContestLink80C2020Struct2018000.showResultsTaskId].func = Task_AnnouncePreliminaryResults;
        HideLinkResultsTextBox();
    }
}

static void Task_AnnouncePreliminaryResults(u8 taskId)
{
    if (gTasks[taskId].data[0] == 0)
    {
        CreateTask(Task_FlashStarsAndHearts, 20);
        DrawResultsTextWindow(gText_AnnouncingResults, eContestLink80C2020Struct2018000.slidingTextBoxSpriteId);
        StartTextBoxSlideIn(sub_80C34AC(gText_AnnouncingResults), 0x90, 0x78, 0x440);
        gTasks[taskId].data[0]++;
    }
    else if (gTasks[taskId].data[0] == 1)
    {
        if (eContestLink80C2020Struct2018000.slidingTextBoxState == 0)
        {
            gTasks[taskId].data[1] = 0;
            gTasks[taskId].data[0]++;
        }
    }
    else if (gTasks[taskId].data[0] == 2)
    {
        if (++gTasks[taskId].data[1] == 0x15)
        {
            gTasks[taskId].data[1] = 0;
            gTasks[taskId].data[0]++;
        }
    }
    else if (gTasks[taskId].data[0] == 3)
    {
        DrawResultsTextWindow(gText_PreliminaryResults, eContestLink80C2020Struct2018000.slidingTextBoxSpriteId);
        StartTextBoxSlideIn(sub_80C34AC(gText_PreliminaryResults), 0x90, 0xffff, 0x440);
        gTasks[taskId].data[0]++;
    }
    else if (gTasks[taskId].data[0] == 4)
    {
        if (eContestLink80C2020Struct2018000.slidingTextBoxState == 2)
        {
            gTasks[taskId].data[0] = 0;
            gTasks[taskId].func = Task_ShowPreliminaryResults;
        }
    }
}

static void Task_ShowPreliminaryResults(u8 taskId)
{
    switch (gTasks[taskId].data[0])
    {
        case 0:
            if (eContestLink80C2020Struct2018000.pointsFlashing == 0)
            {
                UpdateContestResultBars(0, gTasks[taskId].data[2]++);
                if (eContestLink80C2020Struct2018000.numBarsUpdating == 0)
                {
                    gTasks[taskId].data[0] = 2;
                }
                else
                {
                    gTasks[taskId].data[0]++;
                }
            }
            break;
        case 1:
            if (eContestLink80C2020Struct2018000.numBarsUpdating == 0)
            {
                gTasks[taskId].data[0] = 0;
            }
            break;
        case 2:
            StartTextBoxSlideOut(0x440);
            gTasks[taskId].data[0] = 0;
            gTasks[taskId].data[2] = 0;
            gTasks[taskId].func = Task_AnnounceRound2Results;
            break;
    }
}

static void Task_AnnounceRound2Results(u8 taskId)
{
    if (eContestLink80C2020Struct2018000.slidingTextBoxState == 0)
    {
        if (++gTasks[taskId].data[1] == 21)
        {
            gTasks[taskId].data[1] = 0;
            DrawResultsTextWindow(gText_Round2Results, eContestLink80C2020Struct2018000.slidingTextBoxSpriteId);
            StartTextBoxSlideIn(sub_80C34AC(gText_Round2Results), 0x90, 0xffff, 0x440);
        }
    }
    else if (eContestLink80C2020Struct2018000.slidingTextBoxState == 2)
    {
        gTasks[taskId].func = Task_ShowRound2Results;
    }
}

static void Task_ShowRound2Results(u8 taskId)
{
    switch (gTasks[taskId].data[0])
    {
        case 0:
            if (eContestLink80C2020Struct2018000.pointsFlashing == 0)
            {
                UpdateContestResultBars(1, gTasks[taskId].data[2]++);
                if (eContestLink80C2020Struct2018000.numBarsUpdating == 0)
                {
                    gTasks[taskId].data[0] = 2;
                }
                else
                {
                    gTasks[taskId].data[0]++;
                }
            }
            break;
        case 1:
            if (eContestLink80C2020Struct2018000.numBarsUpdating == 0)
            {
                gTasks[taskId].data[0] = 0;
            }
            break;
        case 2:
            StartTextBoxSlideOut(0x440);
            gTasks[taskId].data[0] = 0;
            gTasks[taskId].func = Task_AnnounceWinner;
            break;
    }
}

static void Task_AnnounceWinner(u8 taskId)
{
    int i;
    u8 newTaskId;
    u8 winnerTextBuffer[100];

    switch (gTasks[taskId].data[0])
    {
        case 0:
            if (eContestLink80C2020Struct2018000.slidingTextBoxState == 0)
                gTasks[taskId].data[0]++;
            break;
        case 1:
            if (++gTasks[taskId].data[1] == 31)
            {
                gTasks[taskId].data[1] = 0;
                gTasks[taskId].data[0]++;
            }
            break;
        case 2:
            for (i = 0; i < 4; i++)
            {
                newTaskId = CreateTask(Task_DrawFinalStandingNumber, 10);
                gTasks[newTaskId].data[0] = gContestFinalStandings[i];
                gTasks[newTaskId].data[1] = i;
            }
            gTasks[taskId].data[0]++;
            break;
        case 3:
            if (eContestLink80C2020Struct2018000.numStandingsPrinted == 4)
            {
                if (++gTasks[taskId].data[1] == 31)
                {
                    gTasks[taskId].data[1] = 0;
                    CreateTask(Task_StartHighlightWinnersBox, 10);
                    gTasks[taskId].data[0]++;
                    GET_CONTEST_WINNER_ID(i);
                    BounceMonIconInBox(i, 14);
                }
            }
            break;
        case 4:
            if (++gTasks[taskId].data[1] == 21)
            {
                gTasks[taskId].data[1] = 0;
                GET_CONTEST_WINNER_ID(i);
                if (gLinkContestFlags & 1)
                {
                    StringCopy(gStringVar1, gLinkPlayers[i].name);
                }
                else
                {
                    StringCopy(gStringVar1, gContestMons[i].trainerName);
                }
                StringCopy(gStringVar2, gContestMons[i].nickname);
                StringExpandPlaceholders(winnerTextBuffer, gText_ContestantsMonWon);
                DrawResultsTextWindow(winnerTextBuffer, eContestLink80C2020Struct2018000.slidingTextBoxSpriteId);
                StartTextBoxSlideIn(sub_80C34AC(winnerTextBuffer), 0x90, 0xffff, 0x440);
                gTasks[taskId].data[0]++;
            }
            break;
        case 5:
            gTasks[taskId].data[0] = 0;
            gTasks[taskId].func = Task_ShowWinnerMonBanner;
            break;
    }
}

static void Task_ShowWinnerMonBanner(u8 taskId)
{
    int i;
    u8 spriteId;
    u16 species;
    u32 personality;
    u32 otId;
    const struct CompressedSpritePalette *pokePal;

    switch (gTasks[taskId].data[0])
    {
        case 0:
            gBattle_WIN0H = 0xf0;
            gBattle_WIN0V = 0x5050;
            GET_CONTEST_WINNER_ID(i);
            species = gContestMons[i].species;
            personality = gContestMons[i].personality;
            otId = gContestMons[i].otId;
            HandleLoadSpecialPokePic(gMonFrontPicTable + species, gMonFrontPicCoords[species].size, gMonFrontPicCoords[species].y_offset, (void *)gSharedMem, gMonSpriteGfx_Sprite_ptr[1], species, personality);
            pokePal = GetMonSpritePalStructFromOtIdPersonality(species, otId, personality);
            LoadCompressedSpritePalette(pokePal);
            SetMultiuseSpriteTemplateToPokemon(species, 1);
            gMultiuseSpriteTemplate.paletteTag = pokePal->tag;
            spriteId = CreateSprite(&gMultiuseSpriteTemplate, 0x110, 0x50, 10);
            gSprites[spriteId].data[1] = species;
            gSprites[spriteId].oam.priority = 0;
            gSprites[spriteId].callback = SpriteCB_WinnerMonSlideIn;
            eContestLink80C2020Struct2018000.winnerMonSpriteId = spriteId;
            LoadCompressedSpriteSheet(&sSpriteSheet_Confetti);
            LoadCompressedSpritePalette(&sSpritePalette_Confetti);
            CreateTask(Task_CreateConfetti, 10);
            gTasks[taskId].data[0]++;
            break;
        case 1:
            if (++gTasks[taskId].data[3] == 1)
            {
                u8 counter;
                gTasks[taskId].data[3] = 0;
                gTasks[taskId].data[2] += 2;
                if (gTasks[taskId].data[2] > 0x20)
                    gTasks[taskId].data[2] = 0x20;
                counter = gTasks[taskId].data[2];
                gBattle_WIN0V = ((0x50 - counter) << 8) | (0x50 + counter);
                if (counter == 0x20)
                {
                    gTasks[taskId].data[0]++;
                }
            }
            break;
        case 2:
            if (eContestLink80C2020Struct2018000.winnerMonSlidingState == 1)
            {
                gTasks[taskId].data[0]++;
            }
            break;
        case 3:
            if (++gTasks[taskId].data[1] == 121)
            {
                gTasks[taskId].data[1] = 0;
                gSprites[eContestLink80C2020Struct2018000.winnerMonSpriteId].callback = SpriteCB_WinnerMonSlideOut;
                gTasks[taskId].data[0]++;
            }
            break;
        case 4:
            if (eContestLink80C2020Struct2018000.winnerMonSlidingState == 2)
            {
                u8 top = (gBattle_WIN0V >> 8);
                top += 2;
                if (top > 0x50)
                    top = 0x50;
                gBattle_WIN0V = (top << 8) | (0xa0 - top);
                if (top == 0x50)
                {
                    gTasks[taskId].data[0]++;
                }
            }
            break;
        case 5:
            if (eContestLink80C2020Struct2018000.winnerMonSlidingState == 2)
            {
                eContestLink80C2020Struct2018000.destroyConfetti = 1;
                gTasks[taskId].data[0] = 0;
                gTasks[taskId].func = Task_SetSeenWinnerMon;
            }
            break;
    }
}

static void Task_SetSeenWinnerMon(u8 taskId)
{
    int i;

    if (JOY_NEW(A_BUTTON))
    {
        if (!(gLinkContestFlags & 1))
        {
            for (i = 0; i < 4; i++)
            {
                GetSetPokedexFlag(SpeciesToNationalPokedexNum(gContestMons[i].species), FLAG_SET_SEEN);
            }
        }
        gTasks[taskId].func = Task_TryDisconnectLinkPartners;
    }
}

static void Task_TryDisconnectLinkPartners(u8 taskId)
{
    if (gLinkContestFlags & 1)
    {
        ShowLinkResultsTextBox(gText_CommunicationStandby);
        SetCloseLinkCallback();
        gTasks[taskId].func = Task_WaitForLinkPartnersDisconnect;
    }
    else
    {
        gTasks[taskId].func = Task_TrySetContestInterviewData;
    }
}

static void Task_WaitForLinkPartnersDisconnect(u8 taskId)
{
    if (gReceivedRemoteLinkPlayers == 0)
    {
        gLinkContestFlags = 0;
        HideLinkResultsTextBox();
        gTasks[taskId].func = Task_TrySetContestInterviewData;
    }
}

static void Task_TrySetContestInterviewData(u8 taskId)
{
    BravoTrainerPokemonProfile_BeforeInterview2(gContestFinalStandings[gContestPlayerMonIndex]);
    TryGainNewFanFromCounter(2);
    SaveContestWinner(gSpecialVar_ContestRank);
    SaveContestWinner(0xFE);
    gCurContestWinnerIsForArtist = TRUE;
    gCurContestWinnerSaveIdx = GetContestWinnerSaveIdx(0xfe, 0);
    BeginHardwarePaletteFade(0xff, 0, 0, 16, 0);
    gTasks[taskId].func = Task_EndShowContestResults;
}

static void Task_EndShowContestResults(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        if (gTasks[taskId].data[1] == 0)
        {
            DestroyTask(eContestLink80C2020Struct2018000.highlightWinnerTaskId);
            BlendPalettes(0x0000ffff, 16, 0);
            gTasks[taskId].data[1]++;
        }
        else if (gTasks[taskId].data[1] == 1)
        {
            BlendPalettes(0xffff0000, 16, 0);
            gTasks[taskId].data[1]++;
        }
        else
        {
            REG_BLDCNT = 0;
            REG_BLDY = 0;
            DestroyTask(taskId);
            SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
        }
    }
}

static void Task_SlideContestResultsBg(u8 taskId)
{
    gBattle_BG3_X += 2;
    gBattle_BG3_Y++;
    if (gBattle_BG3_X > 0xff)
        gBattle_BG3_X -= 0xff;
    if (gBattle_BG3_Y > 0xff)
        gBattle_BG3_Y -= 0xff;
}

static void Task_FlashStarsAndHearts(u8 taskId)
{
    if (++gTasks[taskId].data[0] == 2)
    {
        gTasks[taskId].data[0] = 0;
        if (gTasks[taskId].data[2] == 0)
            gTasks[taskId].data[1]++;
        else
            gTasks[taskId].data[1]--;
        if (gTasks[taskId].data[1] == 16)
            gTasks[taskId].data[2] = 1;
        else if (gTasks[taskId].data[1] == 0)
            gTasks[taskId].data[2] = 0;
        BlendPalette(0x6b, 0x01, gTasks[taskId].data[1], RGB(30, 22, 11));
        BlendPalette(0x68, 0x01, gTasks[taskId].data[1], RGB(31, 31, 31));
        BlendPalette(0x6e, 0x01, gTasks[taskId].data[1], RGB(30, 29, 29));
    }
    if (gTasks[taskId].data[1] == 0)
        eContestLink80C2020Struct2018000.pointsFlashing = 0;
    else
        eContestLink80C2020Struct2018000.pointsFlashing = 1;
}

static void LoadContestMonIcon(u16 species, u8 monIndex, u8 srcOffset, bool8 useDmaNow, u32 personality)
{
    int i;
    int j;
    u16 tile;
    u16 offset;
    u16 var0;
    u16 var1;

    if (useDmaNow)
    {
        DmaCopy32Defvars(3, GetMonIconPtr(species, personality) + (srcOffset << 9) + 0x80, BG_CHAR_ADDR(1) + (monIndex << 9), 0x180);
        var0 = ((monIndex + 10) << 12);
        var1 = (monIndex * 16 + 0x200);
        tile = var1 | var0;
        offset = monIndex * 0x60 + 0x83;
        for (i = 0; i < 3; i++)
        {
            for (j = 0; j < 4; j++)
            {
                ((u16 *)BG_CHAR_ADDR(3))[(i << 5) + j + offset] = tile;
                tile++;
            }
        }
    }
    else
    {
        RequestSpriteCopy(GetMonIconPtr(species, personality) + (srcOffset << 9) + 0x80, BG_CHAR_ADDR(1) + (monIndex << 9), 0x180);
    }
}

static void LoadAllContestMonIcons(u8 srcOffset, bool8 useDmaNow)
{
    int i;

    for (i = 0; i < 4; i++)
    {
        LoadContestMonIcon(gContestMons[i].species, i, srcOffset, useDmaNow, gContestMons[i].personality);
    }
}

static void LoadAllContestMonIconPalettes(void)
{
    int i;
    register u16 species asm("r0");

    for (i = 0; i < 4; i++)
    {
        species = GetIconSpecies(gContestMons[i].species, 0);
        LoadPalette(gMonIconPalettes[gMonIconPaletteIndices[species]], 0xa0 + 0x10 * i, 0x20);
    }
}

#ifdef NONMATCHING
static void DrawResultsTextWindow(const u8 *text, u8 spriteId)
{
    int i, j;
    u8 width;
    u8 * displayedStringBattle;
    void * dst;
    u8 * d1;
    u8 * d2;
    void *d3;
    void *d4;
    void *d5;
    void *d6;
    int tileWidth;
    u16 sp00[4];
    struct Sprite *sprite = &gSprites[spriteId];
    sp00[0] = gSprites[spriteId].oam.tileNum;
    sp00[1] = gSprites[sprite->data[0]].oam.tileNum;
    sp00[2] = gSprites[sprite->data[1]].oam.tileNum;
    sp00[3] = gSprites[sprite->data[2]].oam.tileNum;

    for (i = 0; i < 4; i++)
    {
        DmaClear32(3, (void *)VRAM + 0x10000 + 32 * sp00[i], 0x400);
    }

    width = Text_GetStringWidthFromWindowTemplate(&gWindowTemplate_81E7278, text);
    displayedStringBattle = gDisplayedStringBattle;
    displayedStringBattle = StringCopy(displayedStringBattle, gUnknown_083D17E2);
    if ((~width + 1) & 7)
    {
        displayedStringBattle[0] = EXT_CTRL_CODE_BEGIN;
        displayedStringBattle[1] = 0x11;
        displayedStringBattle[2] = ((~width + 1) & 7) / 2;
        displayedStringBattle += 3;
    }

    width += -8 & (width + 7);
    displayedStringBattle = StringCopy(displayedStringBattle, text);

    displayedStringBattle[0] = EXT_CTRL_CODE_BEGIN;
    displayedStringBattle[1] = 0x13;
    displayedStringBattle[2] = width;
    displayedStringBattle[3] = EOS;

    RenderTextHandleBold(eContestLink80C2020Struct2018068, gDisplayedStringBattle);

    CpuCopy32(&gUnknown_083D1624[0x0], (void *)(VRAM + 0x10000) + 32 * sp00[0], 32);
    CpuCopy32(&gUnknown_083D1624[0x40], (void *)(VRAM + 0x10000) + 32 * sp00[0] + 0x100, 32);
    CpuCopy32(&gUnknown_083D1624[0x40], (void *)(VRAM + 0x10000) + 32 * sp00[0] + 0x200, 32);
    CpuCopy32(&gUnknown_083D1624[0x20], (void *)(VRAM + 0x10000) + 32 * sp00[0] + 0x300, 32);

    tileWidth = width / 8;
    j = 0;
    if (j <= tileWidth)
    {
        d2 = eContestLink80C2020Struct2018068 + 0x20;
        d1 = eContestLink80C2020Struct2018068;
        d3 = (void *)VRAM + 0x0FD20;
        d4 = (void *)VRAM + 0x0FE20;
        d5 = (void *)VRAM + 0x0FF20;
        d6 = (void *)VRAM + 0x10020;
        while (j <= tileWidth)
        {
            if (j < 7)
                dst = 32 * sp00[0] + d6;
            else if (j < 15)
                dst = 32 * sp00[1] + d5;
            else if (j < 23)
                dst = 32 * sp00[2] + d4;
            else
                dst = 32 * sp00[3] + d3;

            if (j == tileWidth)
                break;

            CpuCopy32(gUnknown_083D16E4, dst, 32);
            CpuCopy32(gUnknown_083D16E4 + 0x10, dst + 0x300, 32);
            CpuCopy32(j * 0x40 + d2, dst + 0x100, 32);
            CpuCopy32(j * 0x40 + d1, dst + 0x200, 32);

            d3 += 0x20;
            d4 += 0x20;
            d5 += 0x20;
            d6 += 0x20;
            j++;
        }
    }

    CpuCopy32(gUnknown_083D1644, dst, 32);
    CpuCopy32(gUnknown_083D1644 + 0x40, dst + 0x100, 32);
    CpuCopy32(gUnknown_083D1644 + 0x40, dst + 0x200, 32);
    CpuCopy32(gUnknown_083D1644 + 0x20, dst + 0x300, 32);
}
#else
asm(".include \"constants/gba_constants.inc\"");
asm(".include \"asm/macros.inc\"");
NAKED
static void DrawResultsTextWindow(const u8 * text, u8 spriteId)
{
    asm_unified("\tpush {r4-r7,lr}\n"
                "\tmov r7, r10\n"
                "\tmov r6, r9\n"
                "\tmov r5, r8\n"
                "\tpush {r5-r7}\n"
                "\tsub sp, 0x1C\n"
                "\tmov r9, r0\n"
                "\tlsls r1, 24\n"
                "\tlsrs r1, 24\n"
                "\tlsls r2, r1, 4\n"
                "\tadds r2, r1\n"
                "\tlsls r2, 2\n"
                "\tldr r3, _080C32C0 @ =gSprites\n"
                "\tadds r2, r3\n"
                "\tmov r1, sp\n"
                "\tldrh r0, [r2, 0x4]\n"
                "\tlsls r0, 22\n"
                "\tlsrs r0, 22\n"
                "\tstrh r0, [r1]\n"
                "\tmov r4, sp\n"
                "\tmovs r0, 0x2E\n"
                "\tldrsh r1, [r2, r0]\n"
                "\tlsls r0, r1, 4\n"
                "\tadds r0, r1\n"
                "\tlsls r0, 2\n"
                "\tadds r0, r3\n"
                "\tldrh r0, [r0, 0x4]\n"
                "\tlsls r0, 22\n"
                "\tlsrs r0, 22\n"
                "\tstrh r0, [r4, 0x2]\n"
                "\tmovs r0, 0x30\n"
                "\tldrsh r1, [r2, r0]\n"
                "\tlsls r0, r1, 4\n"
                "\tadds r0, r1\n"
                "\tlsls r0, 2\n"
                "\tadds r0, r3\n"
                "\tldrh r0, [r0, 0x4]\n"
                "\tlsls r0, 22\n"
                "\tlsrs r0, 22\n"
                "\tstrh r0, [r4, 0x4]\n"
                "\tmovs r0, 0x32\n"
                "\tldrsh r1, [r2, r0]\n"
                "\tlsls r0, r1, 4\n"
                "\tadds r0, r1\n"
                "\tlsls r0, 2\n"
                "\tadds r0, r3\n"
                "\tldrh r0, [r0, 0x4]\n"
                "\tlsls r0, 22\n"
                "\tlsrs r0, 22\n"
                "\tstrh r0, [r4, 0x6]\n"
                "\tldr r1, _080C32C4 @ =gWindowTemplate_81E7278\n"
                "\tmov r8, r1\n"
                "\tldr r7, _080C32C8 @ =0x06010000\n"
                "\tldr r2, _080C32CC @ =0x040000d4\n"
                "\tldr r6, _080C32D0 @ =0x85000100\n"
                "\tmov r1, sp\n"
                "\tmovs r5, 0\n"
                "\tadd r3, sp, 0x8\n"
                "\tmovs r4, 0x3\n"
                "_080C31CE:\n"
                "\tldrh r0, [r1]\n"
                "\tlsls r0, 5\n"
                "\tadds r0, r7\n"
                "\tstr r5, [sp, 0x8]\n"
                "\tstr r3, [r2]\n"
                "\tstr r0, [r2, 0x4]\n"
                "\tstr r6, [r2, 0x8]\n"
                "\tldr r0, [r2, 0x8]\n"
                "\tadds r1, 0x2\n"
                "\tsubs r4, 0x1\n"
                "\tcmp r4, 0\n"
                "\tbge _080C31CE\n"
                "\tmov r0, r8\n"
                "\tmov r1, r9\n"
                "\tbl Text_GetStringWidthFromWindowTemplate\n"
                "\tlsls r0, 24\n"
                "\tlsrs r5, r0, 24\n"
                "\tldr r2, _080C32D4 @ =gDisplayedStringBattle\n"
                "\tldr r1, _080C32D8 @ =gUnknown_083D17E2\n"
                "\tadds r0, r2, 0\n"
                "\tbl StringCopy\n"
                "\tadds r2, r0, 0\n"
                "\tmvns r0, r5\n"
                "\tadds r1, r0, 0x1\n"
                "\tmovs r0, 0x7\n"
                "\tands r1, r0\n"
                "\tcmp r1, 0\n"
                "\tbeq _080C3218\n"
                "\tmovs r0, 0xFC\n"
                "\tstrb r0, [r2]\n"
                "\tmovs r0, 0x11\n"
                "\tstrb r0, [r2, 0x1]\n"
                "\tlsrs r0, r1, 1\n"
                "\tstrb r0, [r2, 0x2]\n"
                "\tadds r2, 0x3\n"
                "_080C3218:\n"
                "\tadds r6, r5, 0x7\n"
                "\tmovs r1, 0x8\n"
                "\tnegs r1, r1\n"
                "\tadds r0, r1, 0\n"
                "\tands r6, r0\n"
                "\tlsls r6, 24\n"
                "\tlsrs r5, r6, 24\n"
                "\tadds r0, r2, 0\n"
                "\tmov r1, r9\n"
                "\tbl StringCopy\n"
                "\tadds r2, r0, 0\n"
                "\tmovs r0, 0xFC\n"
                "\tstrb r0, [r2]\n"
                "\tmovs r0, 0x13\n"
                "\tstrb r0, [r2, 0x1]\n"
                "\tstrb r5, [r2, 0x2]\n"
                "\tmovs r0, 0xFF\n"
                "\tstrb r0, [r2, 0x3]\n"
                "\tldr r0, _080C32DC @ =gSharedMem + 0x18068\n"
                "\tmov r10, r0\n"
                "\tldr r1, _080C32D4 @ =gDisplayedStringBattle\n"
                "\tbl RenderTextHandleBold\n"
                "\tmov r0, sp\n"
                "\tldrh r4, [r0]\n"
                "\tlsls r4, 5\n"
                "\tldr r1, _080C32C8 @ =0x06010000\n"
                "\tadds r7, r4, r1\n"
                "\tldr r0, _080C32E0 @ =gUnknown_083D1624\n"
                "\tmov r9, r0\n"
                "\tldr r1, _080C32E4 @ =REG_BG0CNT\n"
                "\tmov r8, r1\n"
                "\tadds r1, r7, 0\n"
                "\tmov r2, r8\n"
                "\tbl CpuSet\n"
                "\tmov r5, r9\n"
                "\tadds r5, 0x80\n"
                "\tldr r0, _080C32E8 @ =0x06010100\n"
                "\tadds r1, r4, r0\n"
                "\tadds r0, r5, 0\n"
                "\tmov r2, r8\n"
                "\tbl CpuSet\n"
                "\tldr r0, _080C32EC @ =0x06010200\n"
                "\tadds r1, r4, r0\n"
                "\tadds r0, r5, 0\n"
                "\tmov r2, r8\n"
                "\tbl CpuSet\n"
                "\tmov r0, r9\n"
                "\tadds r0, 0x40\n"
                "\tldr r1, _080C32F0 @ =0x06010300\n"
                "\tadds r4, r1\n"
                "\tadds r1, r4, 0\n"
                "\tmov r2, r8\n"
                "\tbl CpuSet\n"
                "\tlsrs r5, r6, 27\n"
                "\tmovs r4, 0\n"
                "\tcmp r4, r5\n"
                "\tbgt _080C3382\n"
                "\tmov r6, sp\n"
                "\tmov r0, r10\n"
                "\tadds r0, 0x20\n"
                "\tstr r0, [sp, 0xC]\n"
                "\tmov r1, r10\n"
                "\tstr r1, [sp, 0x10]\n"
                "\tldr r0, _080C32F4 @ =0x0600fd20\n"
                "\tstr r0, [sp, 0x14]\n"
                "\tldr r1, _080C32F8 @ =0x0600fe20\n"
                "\tstr r1, [sp, 0x18]\n"
                "\tldr r0, _080C32FC @ =0x0600ff20\n"
                "\tmov r10, r0\n"
                "\tldr r1, _080C3300 @ =0x06010020\n"
                "\tmov r9, r1\n"
                "_080C32B2:\n"
                "\tcmp r4, 0x6\n"
                "\tbgt _080C3304\n"
                "\tldrh r0, [r6]\n"
                "\tlsls r0, 5\n"
                "\tmov r1, r9\n"
                "\tb _080C3322\n"
                "\t.align 2, 0\n"
                "_080C32C0: .4byte gSprites\n"
                "_080C32C4: .4byte gWindowTemplate_81E7278\n"
                "_080C32C8: .4byte 0x06010000\n"
                "_080C32CC: .4byte 0x040000d4\n"
                "_080C32D0: .4byte 0x85000100\n"
                "_080C32D4: .4byte gDisplayedStringBattle\n"
                "_080C32D8: .4byte gUnknown_083D17E2\n"
                "_080C32DC: .4byte gSharedMem + 0x18068\n"
                "_080C32E0: .4byte gUnknown_083D1624\n"
                "_080C32E4: .4byte REG_BG0CNT\n"
                "_080C32E8: .4byte 0x06010100\n"
                "_080C32EC: .4byte 0x06010200\n"
                "_080C32F0: .4byte 0x06010300\n"
                "_080C32F4: .4byte 0x0600fd20\n"
                "_080C32F8: .4byte 0x0600fe20\n"
                "_080C32FC: .4byte 0x0600ff20\n"
                "_080C3300: .4byte 0x06010020\n"
                "_080C3304:\n"
                "\tcmp r4, 0xE\n"
                "\tbgt _080C3310\n"
                "\tldrh r0, [r6, 0x2]\n"
                "\tlsls r0, 5\n"
                "\tmov r1, r10\n"
                "\tb _080C3322\n"
                "_080C3310:\n"
                "\tcmp r4, 0x16\n"
                "\tbgt _080C331C\n"
                "\tldrh r0, [r6, 0x4]\n"
                "\tlsls r0, 5\n"
                "\tldr r1, [sp, 0x18]\n"
                "\tb _080C3322\n"
                "_080C331C:\n"
                "\tldrh r0, [r6, 0x6]\n"
                "\tlsls r0, 5\n"
                "\tldr r1, [sp, 0x14]\n"
                "_080C3322:\n"
                "\tadds r7, r0, r1\n"
                "\tcmp r4, r5\n"
                "\tbeq _080C3382\n"
                "\tldr r0, _080C33D0 @ =gUnknown_083D16E4\n"
                "\tadds r1, r7, 0\n"
                "\tmov r2, r8\n"
                "\tbl CpuSet\n"
                "\tmovs r0, 0xC0\n"
                "\tlsls r0, 2\n"
                "\tadds r1, r7, r0\n"
                "\tldr r0, _080C33D0 @ =gUnknown_083D16E4\n"
                "\tadds r0, 0x20\n"
                "\tmov r2, r8\n"
                "\tbl CpuSet\n"
                "\tmovs r0, 0x80\n"
                "\tlsls r0, 1\n"
                "\tadds r1, r7, r0\n"
                "\tldr r0, [sp, 0x10]\n"
                "\tmov r2, r8\n"
                "\tbl CpuSet\n"
                "\tmovs r0, 0x80\n"
                "\tlsls r0, 2\n"
                "\tadds r1, r7, r0\n"
                "\tldr r0, [sp, 0xC]\n"
                "\tmov r2, r8\n"
                "\tbl CpuSet\n"
                "\tldr r1, [sp, 0xC]\n"
                "\tadds r1, 0x40\n"
                "\tstr r1, [sp, 0xC]\n"
                "\tldr r0, [sp, 0x10]\n"
                "\tadds r0, 0x40\n"
                "\tstr r0, [sp, 0x10]\n"
                "\tldr r1, [sp, 0x14]\n"
                "\tadds r1, 0x20\n"
                "\tstr r1, [sp, 0x14]\n"
                "\tldr r0, [sp, 0x18]\n"
                "\tadds r0, 0x20\n"
                "\tstr r0, [sp, 0x18]\n"
                "\tmovs r1, 0x20\n"
                "\tadd r10, r1\n"
                "\tadd r9, r1\n"
                "\tadds r4, 0x1\n"
                "\tcmp r4, r5\n"
                "\tble _080C32B2\n"
                "_080C3382:\n"
                "\tldr r4, _080C33D4 @ =gUnknown_083D1644\n"
                "\tldr r5, _080C33D8 @ =REG_BG0CNT\n"
                "\tadds r0, r4, 0\n"
                "\tadds r1, r7, 0\n"
                "\tadds r2, r5, 0\n"
                "\tbl CpuSet\n"
                "\tadds r6, r4, 0\n"
                "\tadds r6, 0x80\n"
                "\tmovs r0, 0x80\n"
                "\tlsls r0, 1\n"
                "\tadds r1, r7, r0\n"
                "\tadds r0, r6, 0\n"
                "\tadds r2, r5, 0\n"
                "\tbl CpuSet\n"
                "\tmovs r0, 0x80\n"
                "\tlsls r0, 2\n"
                "\tadds r1, r7, r0\n"
                "\tadds r0, r6, 0\n"
                "\tadds r2, r5, 0\n"
                "\tbl CpuSet\n"
                "\tadds r4, 0x40\n"
                "\tmovs r0, 0xC0\n"
                "\tlsls r0, 2\n"
                "\tadds r1, r7, r0\n"
                "\tadds r0, r4, 0\n"
                "\tadds r2, r5, 0\n"
                "\tbl CpuSet\n"
                "\tadd sp, 0x1C\n"
                "\tpop {r3-r5}\n"
                "\tmov r8, r3\n"
                "\tmov r9, r4\n"
                "\tmov r10, r5\n"
                "\tpop {r4-r7}\n"
                "\tpop {r0}\n"
                "\tbx r0\n"
                "\t.align 2, 0\n"
                "_080C33D0: .4byte gUnknown_083D16E4\n"
                "_080C33D4: .4byte gUnknown_083D1644\n"
                "_080C33D8: .4byte REG_BG0CNT");
}
#endif //NONMATCHING

static void CreateResultsTextWindowSprites(void)
{
    int i;
    struct SpriteTemplate template;
    u8 spriteIds[8];

    template = sSpriteTemplate_ResultsTextWindow;
    for (i = 0; i <8; i++)
        LoadSpriteSheet(&sSpriteSheets_ResultsTextWindow[i]);

    LoadSpritePalette(&sSpritePalette_ResultsTextWindow);
    for (i = 0; i < 8; i++)
    {
        spriteIds[i] = CreateSprite(&template, 272, 144, 10);
        template.tileTag++;
    }

    gSprites[spriteIds[0]].data[0] = spriteIds[1];
    gSprites[spriteIds[0]].data[1] = spriteIds[2];
    gSprites[spriteIds[0]].data[2] = spriteIds[3];

    gSprites[spriteIds[4]].data[0] = spriteIds[5];
    gSprites[spriteIds[4]].data[1] = spriteIds[6];
    gSprites[spriteIds[4]].data[2] = spriteIds[7];

    eContestLink80C2020Struct2018000.slidingTextBoxSpriteId = spriteIds[0];
    eContestLink80C2020Struct2018000.slidingTextBoxState = 0;
    eContestLink80C2020Struct2018000.linkTextBoxSpriteId = spriteIds[4];
    HideLinkResultsTextBox();
}

u16 sub_80C34AC(const u8 * string)
{
    u8 width = (StringLength(string) * 6);
    return 0x70 - (width / 2);
}

static void StartTextBoxSlideIn(s16 x, u16 y, u16 slideOutTimer, u16 slideIncrement)
{
    struct Sprite *sprite = &gSprites[eContestLink80C2020Struct2018000.slidingTextBoxSpriteId];
    sprite->x = 272;
    sprite->y = y;
    sprite->x2 = 0;
    sprite->y2 = 0;
    sprite->data[4] = x + 32;
    sprite->data[5] = slideOutTimer;
    sprite->data[6] = slideIncrement;
    sprite->data[7] = 0;
    sprite->callback = SpriteCB_TextBoxSlideIn;
    eContestLink80C2020Struct2018000.slidingTextBoxState = 1;
}

static void StartTextBoxSlideOut(u16 slideIncrement)
{
    struct Sprite *sprite = &gSprites[eContestLink80C2020Struct2018000.slidingTextBoxSpriteId];
    sprite->x += sprite->x2;
    sprite->y += sprite->y2;
    sprite->y2 = 0;
    sprite->x2 = 0;
    sprite->data[6] = slideIncrement;
    sprite->data[7] = 0;
    sprite->callback = SpriteCB_TextBoxSlideOut;
    eContestLink80C2020Struct2018000.slidingTextBoxState = 3;
}

static void EndTextBoxSlideOut(struct Sprite *sprite)
{
    sprite->x = 272;
    sprite->y = 144;
    sprite->y2 = 0;
    sprite->x2 = 0;
    sprite->callback = SpriteCallbackDummy;
    eContestLink80C2020Struct2018000.slidingTextBoxState = 0;
}


static void SpriteCB_TextBoxSlideIn(struct Sprite *sprite)
{
    int i;
    s16 delta;

    delta = (u16)sprite->data[7] + (u16)sprite->data[6];
    sprite->x -= delta >> 8;
    sprite->data[7] = (sprite->data[6] + sprite->data[7]) & 0xFF;
    if (sprite->x < sprite->data[4])
        sprite->x = sprite->data[4];

    for (i = 0; i < 3; i++)
    {
        struct Sprite *sprite2 = &gSprites[sprite->data[i]];
        sprite2->x = sprite->x + sprite->x2 + (i + 1) * 64;
    }

    if (sprite->x == sprite->data[4])
        sprite->callback = SpriteCB_EndTextBoxSlideIn;
}

static void SpriteCB_EndTextBoxSlideIn(struct Sprite *sprite)
{
    eContestLink80C2020Struct2018000.slidingTextBoxState = 2;
    if ((u16)sprite->data[5] != 0xFFFF)
    {
        if (--sprite->data[5] == -1)
            StartTextBoxSlideOut(sprite->data[6]);
    }
}

static void SpriteCB_TextBoxSlideOut(struct Sprite *sprite)
{
    int i;
    s16 delta;

    delta = (u16)sprite->data[7] + (u16)sprite->data[6];
    sprite->x -= delta >> 8;
    sprite->data[7] = (sprite->data[6] + sprite->data[7]) & 0xFF;
    for (i = 0; i < 3; i++)
    {
        struct Sprite *sprite2 = &gSprites[sprite->data[i]];
        sprite2->x = sprite->x + sprite->x2 + (i + 1) * 64;
    }

    if (sprite->x + sprite->x2 < -224)
        EndTextBoxSlideOut(sprite);
}

static void ShowLinkResultsTextBox(const u8 *text)
{
    int i;
    u16 x;
    struct Sprite *sprite;

    DrawResultsTextWindow(text, eContestLink80C2020Struct2018000.linkTextBoxSpriteId);
    x = sub_80C34AC(text);
    sprite = &gSprites[eContestLink80C2020Struct2018000.linkTextBoxSpriteId];
    sprite->x = x + 32;
    sprite->y = 80;
    sprite->invisible = FALSE;
    for (i = 0; i < 3; i++)
    {
        gSprites[sprite->data[i]].x = sprite->x + sprite->x2 + (i + 1) * 64;
        gSprites[sprite->data[i]].y = sprite->y;
        gSprites[sprite->data[i]].invisible = FALSE;
    }

    gBattle_WIN0H = 0x00F0;
    gBattle_WIN0V = ((sprite->y - 16) << 8) | (sprite->y + 16);
    REG_WININ = WININ_WIN1_BG_ALL | WININ_WIN1_OBJ | WININ_WIN1_CLR | WININ_WIN0_BG1 | WININ_WIN0_BG2 | WININ_WIN0_BG3 | WININ_WIN0_OBJ | WININ_WIN0_CLR;
}

static void HideLinkResultsTextBox(void)
{
    int i;
    struct Sprite *sprite;

    sprite = &gSprites[eContestLink80C2020Struct2018000.linkTextBoxSpriteId];
    sprite->invisible = TRUE;
    for (i = 0; i < 3; i++)
        gSprites[sprite->data[i]].invisible = TRUE;

    gBattle_WIN0H = 0;
    gBattle_WIN0V = 0;
    REG_WIN0H = gBattle_WIN0H;
    REG_WIN0V = gBattle_WIN0V;
    REG_WININ = WININ_WIN0_BG_ALL | WININ_WIN0_OBJ | WININ_WIN0_CLR  | WININ_WIN1_BG_ALL | WININ_WIN1_OBJ | WININ_WIN1_CLR;
}

#ifdef ENGLISH
#ifdef NONMATCHING
static inline s32 de_sub_80C39A8(s32 a0)
{
    s32 result = 0;
    if (gLinkContestFlags & 0x1)
    {
        sub_809D104((void *)(VRAM + 0xE000), a0, 1, gUnknown_08E964B8, 9, 2, 8, 2);
        result = 8;
    }
    else if (gSpecialVar_ContestRank == 0)
    {
        sub_809D104((void *)(VRAM + 0xE000), a0, 1, gUnknown_08E964B8, 0, 0, 9, 2);
        result = 9;
    }
    else if (gSpecialVar_ContestRank == 1)
    {
        sub_809D104((void *)(VRAM + 0xE000), a0, 1, gUnknown_08E964B8, 9, 0, 8, 2);
        result = 8;
    }
    else if (gSpecialVar_ContestRank == 2)
    {
        sub_809D104((void *)(VRAM + 0xE000), a0, 1, gUnknown_08E964B8, 17, 0, 8, 2);
        result = 8;
    }
    else
    {
        sub_809D104((void *)(VRAM + 0xE000), a0, 1, gUnknown_08E964B8, 0, 2, 9, 2);
        result = 9;
    }
    return result;
}

static inline s32 de_sub_80C3A84(s32 a0, s32 * a1)
{
    s32 result;
    if (gSpecialVar_ContestCategory == 0)
    {
        *a1 = 0;
        sub_809D104((void *)(VRAM + 0xE000), a0, 1, gUnknown_08E964B8, 17, 2, 10, 2);
        result = 10;
    }
    else if (gSpecialVar_ContestCategory == 1)
    {
        *a1 = 1;
        sub_809D104((void *)(VRAM + 0xE000), a0, 1, gUnknown_08E964B8, 0, 4, 11, 2);
        result = 11;
    }
    else if (gSpecialVar_ContestCategory == 2)
    {
        *a1 = 2;
        sub_809D104((void *)(VRAM + 0xE000), a0, 1, gUnknown_08E964B8, 11, 4, 10, 2);
        result = 10;
    }
    else if (gSpecialVar_ContestCategory == 3)
    {
        *a1 = 3;
        sub_809D104((void *)(VRAM + 0xE000), a0, 1, gUnknown_08E964B8, 21, 4, 10, 2);
        result = 10;
    }
    else
    {
        *a1 = 4;
        sub_809D104((void *)(VRAM + 0xE000), a0, 1, gUnknown_08E964B8, 0, 6, 10, 2);
        result = 10;
    }
    return result;
}

static void LoadContestResultsTitleBarTilemaps(void)
{
    s32 palette;
    s32 i;
    de_sub_80C3A84(de_sub_80C39A8(5) + 5, &palette);
    for (i = 0; i < 0x80; i++)
    {
        ((vu16 *)(VRAM + 0xE000))[i] &= 0xFFF;
        ((vu16 *)(VRAM + 0xE000))[i] |= palette << 12;;
    }
}
#else
NAKED
static void LoadContestResultsTitleBarTilemaps(void)
{
    asm_unified("\tpush {r4-r6,lr}\n"
                "\tsub sp, 0x10\n"
                "\tmovs r5, 0x1\n"
                "\tmovs r4, 0\n"
                "\tldr r0, _080C3808 @ =gLinkContestFlags\n"
                "\tldrb r0, [r0]\n"
                "\tadds r1, r5, 0\n"
                "\tands r1, r0\n"
                "\tcmp r1, 0\n"
                "\tbeq _080C3814\n"
                "\tldr r0, _080C380C @ =0x0600e000\n"
                "\tldr r3, _080C3810 @ =gUnknown_08E964B8\n"
                "\tmovs r1, 0x9\n"
                "\tstr r1, [sp]\n"
                "\tmovs r2, 0x2\n"
                "\tstr r2, [sp, 0x4]\n"
                "\tb _080C386A\n"
                "\t.align 2, 0\n"
                "_080C3808: .4byte gLinkContestFlags\n"
                "_080C380C: .4byte 0x0600e000\n"
                "_080C3810: .4byte gUnknown_08E964B8\n"
                "_080C3814:\n"
                "\tldr r0, _080C3830 @ =gSpecialVar_ContestRank\n"
                "\tldrh r2, [r0]\n"
                "\tcmp r2, 0\n"
                "\tbne _080C383C\n"
                "\tmovs r4, 0x1\n"
                "\tldr r0, _080C3834 @ =0x0600e000\n"
                "\tldr r3, _080C3838 @ =gUnknown_08E964B8\n"
                "\tstr r2, [sp]\n"
                "\tstr r2, [sp, 0x4]\n"
                "\tmovs r1, 0x9\n"
                "\tstr r1, [sp, 0x8]\n"
                "\tmovs r1, 0x2\n"
                "\tstr r1, [sp, 0xC]\n"
                "\tb _080C3870\n"
                "\t.align 2, 0\n"
                "_080C3830: .4byte gSpecialVar_ContestRank\n"
                "_080C3834: .4byte 0x0600e000\n"
                "_080C3838: .4byte gUnknown_08E964B8\n"
                "_080C383C:\n"
                "\tcmp r2, 0x1\n"
                "\tbne _080C385C\n"
                "\tldr r0, _080C3854 @ =0x0600e000\n"
                "\tldr r3, _080C3858 @ =gUnknown_08E964B8\n"
                "\tmovs r1, 0x9\n"
                "\tstr r1, [sp]\n"
                "\tstr r4, [sp, 0x4]\n"
                "\tmovs r1, 0x8\n"
                "\tstr r1, [sp, 0x8]\n"
                "\tmovs r1, 0x2\n"
                "\tstr r1, [sp, 0xC]\n"
                "\tb _080C3870\n"
                "\t.align 2, 0\n"
                "_080C3854: .4byte 0x0600e000\n"
                "_080C3858: .4byte gUnknown_08E964B8\n"
                "_080C385C:\n"
                "\tcmp r2, 0x2\n"
                "\tbne _080C3884\n"
                "\tldr r0, _080C387C @ =0x0600e000\n"
                "\tldr r3, _080C3880 @ =gUnknown_08E964B8\n"
                "\tmovs r1, 0x11\n"
                "\tstr r1, [sp]\n"
                "\tstr r4, [sp, 0x4]\n"
                "_080C386A:\n"
                "\tmovs r1, 0x8\n"
                "\tstr r1, [sp, 0x8]\n"
                "\tstr r2, [sp, 0xC]\n"
                "_080C3870:\n"
                "\tmovs r1, 0x5\n"
                "\tmovs r2, 0x1\n"
                "\tbl sub_809D104\n"
                "\tb _080C389E\n"
                "\t.align 2, 0\n"
                "_080C387C: .4byte 0x0600e000\n"
                "_080C3880: .4byte gUnknown_08E964B8\n"
                "_080C3884:\n"
                "\tmovs r4, 0x1\n"
                "\tldr r0, _080C38C0 @ =0x0600e000\n"
                "\tldr r3, _080C38C4 @ =gUnknown_08E964B8\n"
                "\tstr r1, [sp]\n"
                "\tmovs r2, 0x2\n"
                "\tstr r2, [sp, 0x4]\n"
                "\tmovs r1, 0x9\n"
                "\tstr r1, [sp, 0x8]\n"
                "\tstr r2, [sp, 0xC]\n"
                "\tmovs r1, 0x5\n"
                "\tmovs r2, 0x1\n"
                "\tbl sub_809D104\n"
                "_080C389E:\n"
                "\tadds r4, 0xD\n"
                "\tldr r0, _080C38C8 @ =gSpecialVar_ContestCategory\n"
                "\tldrh r0, [r0]\n"
                "\tcmp r0, 0\n"
                "\tbne _080C38CC\n"
                "\tmovs r6, 0\n"
                "\tldr r0, _080C38C0 @ =0x0600e000\n"
                "\tldr r3, _080C38C4 @ =gUnknown_08E964B8\n"
                "\tmovs r1, 0x11\n"
                "\tstr r1, [sp]\n"
                "\tmovs r2, 0x2\n"
                "\tstr r2, [sp, 0x4]\n"
                "\tmovs r1, 0xA\n"
                "\tstr r1, [sp, 0x8]\n"
                "\tstr r2, [sp, 0xC]\n"
                "\tb _080C392A\n"
                "\t.align 2, 0\n"
                "_080C38C0: .4byte 0x0600e000\n"
                "_080C38C4: .4byte gUnknown_08E964B8\n"
                "_080C38C8: .4byte gSpecialVar_ContestCategory\n"
                "_080C38CC:\n"
                "\tcmp r0, 0x1\n"
                "\tbne _080C38EC\n"
                "\tmovs r6, 0x1\n"
                "\tldr r0, _080C38E4 @ =0x0600e000\n"
                "\tldr r3, _080C38E8 @ =gUnknown_08E964B8\n"
                "\tmovs r1, 0\n"
                "\tstr r1, [sp]\n"
                "\tmovs r1, 0x4\n"
                "\tstr r1, [sp, 0x4]\n"
                "\tmovs r1, 0xB\n"
                "\tb _080C3924\n"
                "\t.align 2, 0\n"
                "_080C38E4: .4byte 0x0600e000\n"
                "_080C38E8: .4byte gUnknown_08E964B8\n"
                "_080C38EC:\n"
                "\tcmp r0, 0x2\n"
                "\tbne _080C3910\n"
                "\tmovs r6, 0x2\n"
                "\tldr r0, _080C3908 @ =0x0600e000\n"
                "\tldr r3, _080C390C @ =gUnknown_08E964B8\n"
                "\tmovs r1, 0xB\n"
                "\tstr r1, [sp]\n"
                "\tmovs r1, 0x4\n"
                "\tstr r1, [sp, 0x4]\n"
                "\tmovs r1, 0xA\n"
                "\tstr r1, [sp, 0x8]\n"
                "\tstr r6, [sp, 0xC]\n"
                "\tb _080C392A\n"
                "\t.align 2, 0\n"
                "_080C3908: .4byte 0x0600e000\n"
                "_080C390C: .4byte gUnknown_08E964B8\n"
                "_080C3910:\n"
                "\tcmp r0, 0x3\n"
                "\tbne _080C393C\n"
                "\tmovs r6, 0x3\n"
                "\tldr r0, _080C3934 @ =0x0600e000\n"
                "\tldr r3, _080C3938 @ =gUnknown_08E964B8\n"
                "\tmovs r1, 0x15\n"
                "\tstr r1, [sp]\n"
                "\tmovs r1, 0x4\n"
                "\tstr r1, [sp, 0x4]\n"
                "\tmovs r1, 0xA\n"
                "_080C3924:\n"
                "\tstr r1, [sp, 0x8]\n"
                "\tmovs r1, 0x2\n"
                "\tstr r1, [sp, 0xC]\n"
                "_080C392A:\n"
                "\tadds r1, r4, 0\n"
                "\tadds r2, r5, 0\n"
                "\tbl sub_809D104\n"
                "\tb _080C395A\n"
                "\t.align 2, 0\n"
                "_080C3934: .4byte 0x0600e000\n"
                "_080C3938: .4byte gUnknown_08E964B8\n"
                "_080C393C:\n"
                "\tmovs r6, 0x4\n"
                "\tldr r0, _080C3984 @ =0x0600e000\n"
                "\tldr r3, _080C3988 @ =gUnknown_08E964B8\n"
                "\tmovs r1, 0\n"
                "\tstr r1, [sp]\n"
                "\tmovs r1, 0x6\n"
                "\tstr r1, [sp, 0x4]\n"
                "\tmovs r1, 0xA\n"
                "\tstr r1, [sp, 0x8]\n"
                "\tmovs r1, 0x2\n"
                "\tstr r1, [sp, 0xC]\n"
                "\tadds r1, r4, 0\n"
                "\tadds r2, r5, 0\n"
                "\tbl sub_809D104\n"
                "_080C395A:\n"
                "\tldr r5, _080C398C @ =0x00000fff\n"
                "\tlsls r4, r6, 12\n"
                "\tldr r2, _080C3984 @ =0x0600e000\n"
                "\tmovs r3, 0x7F\n"
                "_080C3962:\n"
                "\tldrh r1, [r2]\n"
                "\tadds r0, r5, 0\n"
                "\tands r0, r1\n"
                "\tstrh r0, [r2]\n"
                "\tldrh r1, [r2]\n"
                "\tadds r0, r4, 0\n"
                "\torrs r0, r1\n"
                "\tstrh r0, [r2]\n"
                "\tadds r2, 0x2\n"
                "\tsubs r3, 0x1\n"
                "\tcmp r3, 0\n"
                "\tbge _080C3962\n"
                "\tadd sp, 0x10\n"
                "\tpop {r4-r6}\n"
                "\tpop {r0}\n"
                "\tbx r0\n"
                "\t.align 2, 0\n"
                "_080C3984: .4byte 0x0600e000\n"
                "_080C3988: .4byte gUnknown_08E964B8\n"
                "_080C398C: .4byte 0x00000fff");
}
#endif // NONMATCHING

#elif defined(GERMAN)
s16 de_sub_80C39A8(s32 a0)
{
    s16 result;
    if (gLinkContestFlags & 1)
    {
        sub_809D104((void *)(VRAM + 0xE000), a0, 0, gUnknown_08E964B8, 11, 3, 8, 3);
        result = 8;
    }
    else if (gSpecialVar_ContestRank == 0)
    {
        sub_809D104((void *)(VRAM + 0xE000), a0, 0, gUnknown_08E964B8, 0, 0, 11, 3);
        result = 11;
    }
    else if (gSpecialVar_ContestRank == 1)
    {
        sub_809D104((void *)(VRAM + 0xE000), a0, 0, gUnknown_08E964B8, 11, 0, 10, 3);
        result = 10;
    }
    else if (gSpecialVar_ContestRank == 2)
    {
        sub_809D104((void *)(VRAM + 0xE000), a0, 0, gUnknown_08E964B8, 21, 0, 10, 3);
        result = 10;
    }
    else
    {
        sub_809D104((void *)(VRAM + 0xE000), a0, 0, gUnknown_08E964B8, 0, 3, 11, 3);
        result = 11;
    }
    return result;
}

s16 de_sub_80C3A84(s32 a0, s32 * a1)
{
    s16 result;
    if (gSpecialVar_ContestCategory == 0)
    {
        *a1 = 0;
        sub_809D104((void *)(VRAM + 0xE000), a0, 0, gUnknown_08E964B8, 19, 3, 7, 3);
        result = 7;
    }
    else if (gSpecialVar_ContestCategory == 1)
    {
        *a1 = 1;
        sub_809D104((void *)(VRAM + 0xE000), a0, 0, gUnknown_08E964B8, 0, 6, 7, 3);
        result = 7;
    }
    else if (gSpecialVar_ContestCategory == 2)
    {
        *a1 = 2;
        sub_809D104((void *)(VRAM + 0xE000), a0, 0, gUnknown_08E964B8, 7, 6, 4, 3);
        result = 4;
    }
    else if (gSpecialVar_ContestCategory == 3)
    {
        *a1 = 3;
        sub_809D104((void *)(VRAM + 0xE000), a0, 0, gUnknown_08E964B8, 11, 6, 6, 3);
        result = 6;
    }
    else
    {
        *a1 = 4;
        sub_809D104((void *)(VRAM + 0xE000), a0, 0, gUnknown_08E964B8, 17, 6, 5, 3);
        result = 5;
    }
    return result;
}

static void LoadContestResultsTitleBarTilemaps(void)
{
    s32 palette;
    s32 i;
    de_sub_80C3A84(de_sub_80C39A8(6) + 6, &palette);
    for (i = 0; i < 0x80; i++)
    {
        ((vu16 *)(VRAM + 0xE000))[i] &= 0xFFF;
        ((vu16 *)(VRAM + 0xE000))[i] |= palette << 12;;
    }
}
#endif

// fakematching?
static u8 GetNumPreliminaryPoints(u8 monIndex, u8 capPoints)
{
    u32 condition;
    u32 numStars;

    condition = gContestMonRound1Points[monIndex] << 16;
    numStars = condition / 0x3F;
    if (numStars & 0xFFFF)
        numStars += 0x10000;

    numStars >>= 16;
    if (numStars == 0 && condition)
        numStars = 1;

    if (capPoints && numStars > 10)
        numStars = 10;

    return numStars;
}

static s8 GetNumRound2Points(u8 monIndex, u8 capPoints)
{
    u32 r4;
    u32 numHearts;
    s16 results;
    s8 points;

    results = gContestMonRound2Points[monIndex];
    if (results < 0)
        r4 = -results << 16;
    else
        r4 =  results << 16;
    numHearts = r4 / 80;
    if (numHearts & 0xFFFF)
        numHearts += 0x10000;

    numHearts >>= 16;
    if (numHearts == 0 && r4 != 0)
        numHearts = 1;

    if (capPoints != 0 && numHearts > 10)
        numHearts = 10;

    if (gContestMonRound2Points[monIndex] < 0)
        points = -numHearts;
    else
        points =  numHearts;

    return points;
}

static void Task_DrawFinalStandingNumber(u8 taskId)
{
    u16 firstTileNum;

    if (gTasks[taskId].data[10] == 0)
    {
        gTasks[taskId].data[11] = (3 - gTasks[taskId].data[0]) * 40;
        gTasks[taskId].data[10]++;
    }
    else if (gTasks[taskId].data[10] == 1)
    {
        if (--gTasks[taskId].data[11] == -1)
        {
            firstTileNum = gTasks[taskId].data[0] * 2 + 0x5043;
            *(vu16 *)((VRAM + 0xE142) + gTasks[taskId].data[1] * 192) = firstTileNum + 0x00;
            *(vu16 *)((VRAM + 0xE144) + gTasks[taskId].data[1] * 192) = firstTileNum + 0x01;
            *(vu16 *)((VRAM + 0xE182) + gTasks[taskId].data[1] * 192) = firstTileNum + 0x10;
            *(vu16 *)((VRAM + 0xE184) + gTasks[taskId].data[1] * 192) = firstTileNum + 0x11;
            eContestLink80C2020Struct2018000.numStandingsPrinted++;
            DestroyTask(taskId);
            PlaySE(SE_CONTEST_PLACE);
        }
    }
}

#ifdef NONMATCHING
static void Task_StartHighlightWinnersBox(u8 taskId)
{
    int i, j, k;

    for (i = 0; i < 4 && gContestFinalStandings[i] != 0; i++)
        ;

    for (j = 0; j < 3; j++)
    {
        for (k = 0; k < 30; k++)
        {
            ((u16 *)((VRAM + 0xE100) + 2 * (96 * i + 32 * j)))[k] &= 0x0FFF;
            ((u16 *)((VRAM + 0xE100) + 2 * (96 * i + 32 * j)))[k] |= 0x9000;
        }
    }
    gTasks[taskId].data[10] = i;
    gTasks[taskId].data[12] = 1;
    gTasks[taskId].func = Task_HighlightWinnersBox;
    eContestLink80C2020Struct2018000.highlightWinnerTaskId = taskId;
}
#else
NAKED
static void Task_StartHighlightWinnersBox(u8 taskId)
{
    asm_unified("\tpush {r4-r7,lr}\n"
                "\tmov r7, r10\n"
                "\tmov r6, r9\n"
                "\tmov r5, r8\n"
                "\tpush {r5-r7}\n"
                "\tlsls r0, 24\n"
                "\tlsrs r0, 24\n"
                "\tmov r12, r0\n"
                "\tmovs r5, 0\n"
                "\tldr r1, _080C3BC0 @ =gContestFinalStandings\n"
                "\tldrb r0, [r1]\n"
                "\tldr r2, _080C3BC4 @ =gTasks\n"
                "\tmov r10, r2\n"
                "\tcmp r0, 0\n"
                "\tbeq _080C3B5C\n"
                "_080C3B4E:\n"
                "\tadds r5, 0x1\n"
                "\tcmp r5, 0x3\n"
                "\tbgt _080C3B5C\n"
                "\tadds r0, r5, r1\n"
                "\tldrb r0, [r0]\n"
                "\tcmp r0, 0\n"
                "\tbne _080C3B4E\n"
                "_080C3B5C:\n"
                "\tmovs r1, 0\n"
                "\tlsls r0, r5, 1\n"
                "\tmov r2, r12\n"
                "\tlsls r2, 2\n"
                "\tmov r9, r2\n"
                "\tadds r0, r5\n"
                "\tlsls r0, 5\n"
                "\tmov r8, r0\n"
                "\tldr r7, _080C3BC8 @ =0x00000fff\n"
                "\tmovs r0, 0x90\n"
                "\tlsls r0, 8\n"
                "\tadds r6, r0, 0\n"
                "_080C3B74:\n"
                "\tlsls r0, r1, 5\n"
                "\tadds r4, r1, 0x1\n"
                "\tadd r0, r8\n"
                "\t@ the next two instructions are swapped\n"
                "\tmovs r3, 0x1D\n"
                "\tlsls r0, 1\n"
                "\tldr r1, _080C3BCC @ =0x0600e100\n"
                "\tadds r2, r0, r1\n"
                "_080C3B82:\n"
                "\tldrh r1, [r2]\n"
                "\tadds r0, r7, 0\n"
                "\tands r0, r1\n"
                "\torrs r0, r6\n"
                "\tstrh r0, [r2]\n"
                "\tadds r2, 0x2\n"
                "\tsubs r3, 0x1\n"
                "\tcmp r3, 0\n"
                "\tbge _080C3B82\n"
                "\tadds r1, r4, 0\n"
                "\tcmp r1, 0x2\n"
                "\tble _080C3B74\n"
                "\tmov r0, r9\n"
                "\tadd r0, r12\n"
                "\tlsls r0, 3\n"
                "\tadd r0, r10\n"
                "\tstrh r5, [r0, 0x1C]\n"
                "\tmovs r1, 0x1\n"
                "\tstrh r1, [r0, 0x20]\n"
                "\tldr r2, _080C3BD0 @ =Task_HighlightWinnersBox\n"
                "\tstr r2, [r0]\n"
                "\tmov r1, r12\n"
                "\tldr r0, _080C3BD4 @ =gSharedMem + 0x18000\n"
                "\tstrb r1, [r0, 0x3]\n"
                "\tpop {r3-r5}\n"
                "\tmov r8, r3\n"
                "\tmov r9, r4\n"
                "\tmov r10, r5\n"
                "\tpop {r4-r7}\n"
                "\tpop {r0}\n"
                "\tbx r0\n"
                "\t.align 2, 0\n"
                "_080C3BC0: .4byte gContestFinalStandings\n"
                "_080C3BC4: .4byte gTasks\n"
                "_080C3BC8: .4byte 0x00000fff\n"
                "_080C3BCC: .4byte 0x0600e100\n"
                "_080C3BD0: .4byte Task_HighlightWinnersBox\n"
                "_080C3BD4: .4byte gSharedMem + 0x18000");
}
#endif //NONMATCHING

void Task_HighlightWinnersBox(u8 taskId)
{
    if (++gTasks[taskId].data[11] == 1)
    {
        gTasks[taskId].data[11] = 0;
        BlendPalette(0x91, 1, gTasks[taskId].data[12], RGB(13, 28, 27));
        if (gTasks[taskId].data[13] == 0)
        {
            if (++gTasks[taskId].data[12] == 16)
                gTasks[taskId].data[13] = 1;
        }
        else
        {
            if (--gTasks[taskId].data[12] == 0)
                gTasks[taskId].data[13] = 0;
        }
    }
}

static void SpriteCB_WinnerMonSlideIn(struct Sprite *sprite)
{
    if (sprite->data[0] < 10)
    {
        if (++sprite->data[0] == 10)
        {
            PlayCry_Normal(sprite->data[1], 0);
            sprite->data[1] = 0;
        }
    }
    else
    {
        s16 delta = (u16)sprite->data[1] + 0x600;
        sprite->x -= delta >> 8;
        sprite->data[1] = (sprite->data[1] + 0x600) & 0xFF;
        if (sprite->x < 120)
            sprite->x = 120;

        if (sprite->x == 120)
        {
            sprite->callback = SpriteCallbackDummy;
            sprite->data[1] = 0;
            eContestLink80C2020Struct2018000.winnerMonSlidingState = 1;
        }
    }
}

static void SpriteCB_WinnerMonSlideOut(struct Sprite *sprite)
{
    s16 delta = (u16)sprite->data[1] + 0x600;
    sprite->x -= delta >> 8;
    sprite->data[1] = (sprite->data[1] + 0x600) & 0xFF;
    if (sprite->x < -32)
    {
        sprite->callback = SpriteCallbackDummy;
        sprite->invisible = TRUE;
        eContestLink80C2020Struct2018000.winnerMonSlidingState = 2;
    }
}

static void Task_CreateConfetti(u8 taskId)
{
    if (++gTasks[taskId].data[0] == 5)
    {
        gTasks[taskId].data[0] = 0;
        if (eContestLink80C2020Struct2018000.confettiCount < 40)
        {
            u8 spriteId = CreateSprite(&sSpriteTemplate_Confetti, (Random() % 240) - 20, 44, 5);
            gSprites[spriteId].data[0] = Random() % 512;
            gSprites[spriteId].data[1] = (Random() % 24) + 16;
            gSprites[spriteId].data[2] = (Random() % 256) + 48;
            gSprites[spriteId].oam.tileNum += Random() % 17;
            eContestLink80C2020Struct2018000.confettiCount++;
        }
    }

    if (eContestLink80C2020Struct2018000.destroyConfetti)
        DestroyTask(taskId);
}

static void SpriteCB_Confetti(struct Sprite *sprite)
{
    register s16 delta asm("r1");

    sprite->data[3] += sprite->data[0];
    sprite->x2 = Sin(sprite->data[3] >> 8, sprite->data[1]);
    delta = sprite->data[4] + sprite->data[2];
    sprite->x += delta >> 8;
    delta = delta & 0xFF;
    sprite->data[4] = delta;
    sprite->y++;
    if (eContestLink80C2020Struct2018000.destroyConfetti)
        sprite->invisible = TRUE;

    if (sprite->x > 248 || sprite->y > 116)
    {
        DestroySprite(sprite);
        eContestLink80C2020Struct2018000.confettiCount--;
    }
}

static void BounceMonIconInBox(u8 monIndex, u8 numFrames)
{
    u8 taskId = CreateTask(Task_BounceMonIconInBox, 8);
    gTasks[taskId].data[0] = monIndex;
    gTasks[taskId].data[1] = numFrames;
    gTasks[taskId].data[2] = gContestMons[monIndex].species;
}

static void Task_BounceMonIconInBox(u8 taskId)
{
    u8 monIndex = gTasks[taskId].data[0];
    if (gTasks[taskId].data[10]++ == gTasks[taskId].data[1])
    {
        gTasks[taskId].data[10] = 0;
        LoadContestMonIcon(gTasks[taskId].data[2], monIndex, gTasks[taskId].data[11], FALSE, gContestMons[monIndex].personality);
        gTasks[taskId].data[11] ^= 1;
    }
}

static void CalculateContestantsResultData(void)
{
    s32 i;
    s16 highestPoints = gContestMonTotalPoints[0];
    s32 relativePoints;
    u32 barLength;
    s8 round2Points;

    for (i = 1; i < 4; i++)
    {
        if (highestPoints < gContestMonTotalPoints[i])
            highestPoints = gContestMonTotalPoints[i];
    }

    if (highestPoints < 0)
    {
        highestPoints = gContestMonTotalPoints[0];

        for (i = 1; i < 4; i++)
        {
            if (highestPoints > gContestMonTotalPoints[i])
                highestPoints = gContestMonTotalPoints[i];
        }
    }

    for (i = 0; i < 4; i++)
    {
        relativePoints = 1000 * gContestMonRound1Points[i] / ABS(highestPoints);
        if ((relativePoints % 10) >= 5)
            relativePoints += 10;
        eContestLink80C2020Struct2018018[i].relativePreliminaryPoints = relativePoints / 10;

        relativePoints = 1000 * ABS(gContestMonRound2Points[i]) / ABS(highestPoints);
        if ((relativePoints % 10) >= 5)
            relativePoints += 10;
        eContestLink80C2020Struct2018018[i].relativeRound2Points = relativePoints / 10;

        if (gContestMonRound2Points[i] < 0)
            eContestLink80C2020Struct2018018[i].lostPoints = 1;

        barLength = 22528 * eContestLink80C2020Struct2018018[i].relativePreliminaryPoints / 100;
        if ((barLength % 256) >= 128)
            barLength += 256;
        eContestLink80C2020Struct2018018[i].barLengthPreliminary = barLength / 256;

        barLength = eContestLink80C2020Struct2018018[i].relativeRound2Points * 22528 / 100;
        if ((barLength % 256) >= 128)
            barLength += 256;
        eContestLink80C2020Struct2018018[i].barLengthRound2 = barLength / 256;

        eContestLink80C2020Struct2018018[i].numStars = GetNumPreliminaryPoints(i, 1);
        round2Points = GetNumRound2Points(i, 1);
        eContestLink80C2020Struct2018018[i].numHearts = ABS(round2Points);

        if (gContestFinalStandings[i])
        {
            s16 barLengthPreliminary = eContestLink80C2020Struct2018018[i].barLengthPreliminary;
            s16 barLengthRound2 = eContestLink80C2020Struct2018018[i].barLengthRound2;
            if (eContestLink80C2020Struct2018018[i].lostPoints)
                barLengthRound2 = -barLengthRound2;
            if (barLengthPreliminary + barLengthRound2 == 88)
            {
                if (barLengthRound2 > 0)
                    eContestLink80C2020Struct2018018[i].barLengthRound2--;
                else if (barLengthPreliminary > 0)
                    eContestLink80C2020Struct2018018[i].barLengthPreliminary--;
            }
        }
    }
}

#ifdef NONMATCHING
static void UpdateContestResultBars(u8 isRound2, u8 numUpdates)
{
    int i;
    u8 taskId;
    u8 numIncreasing, numDecreasing;

    numIncreasing = 0;
    numDecreasing = 0;
    if (!isRound2)
    {
        u32 target;
        for (i = 0; i < 4; i++)
        {
            u8 numStars = eContestLink80C2020Struct2018018[i].numStars;
            if (numUpdates < numStars)
            {
                int x = numStars + 19;
                x += 32 * (i * 3 + 5);
                x -= numUpdates;
                x--;
                *(vu16 *)((VRAM + 0xC000) + 2 * x) = 0x60B3;
                taskId = CreateTask(Task_UpdateContestResultBar, 10);
                target = ((eContestLink80C2020Struct2018018[i].barLengthPreliminary << 16) / eContestLink80C2020Struct2018018[i].numStars) * (numUpdates + 1);
                if ((target % 0x10000) >= 0x8000)
                    target += 0x10000;

                gTasks[taskId].data[0] = i;
                gTasks[taskId].data[1] = target >> 16;
                eContestLink80C2020Struct2018000.numBarsUpdating++;
                numIncreasing++;
            }
        }
    }
    else
    {
        u32 target;
        for (i = 0; i < 4; i++)
        {
            int tile;
            s8 numHearts = eContestLink80C2020Struct2018018[i].numHearts;
            tile = eContestLink80C2020Struct2018018[i].lostPoints ? 0x60A5 : 0x60A3;
            if (numUpdates < numHearts)
            {
                int x = numHearts + 19;
                x += 32 * (i * 3 + 6);
                x -= numUpdates;
                x--;
                *(vu16 *)((VRAM + 0xC000) + 2 * x) = tile;
                taskId = CreateTask(Task_UpdateContestResultBar, 10);
                target = ((eContestLink80C2020Struct2018018[i].barLengthRound2 << 16) / eContestLink80C2020Struct2018018[i].numHearts) * (numUpdates + 1);
                if ((target % 0x10000) >= 0x8000)
                    target += 0x10000;

                gTasks[taskId].data[0] = i;
                if (eContestLink80C2020Struct2018018[i].lostPoints)
                {
                    gTasks[taskId].data[2] = 1;
                    numDecreasing++;
                }
                else
                {
                    numIncreasing++;
                }

                if (eContestLink80C2020Struct2018018[i].lostPoints)
                    gTasks[taskId].data[1] =  -(target >> 16) + eContestLink80C2020Struct2018018[i].barLengthPreliminary;
                else
                    gTasks[taskId].data[1] = (target >> 16) + eContestLink80C2020Struct2018018[i].barLengthPreliminary;

                eContestLink80C2020Struct2018000.numBarsUpdating++;
            }
        }
    }

    if (numDecreasing)
        PlaySE(SE_BOO);

    if (numIncreasing)
        PlaySE(SE_PIN);
}
#else
// Assorted register differences
NAKED
static void UpdateContestResultBars(u8 isRound2, u8 numUpdates)
{
    asm_unified("\tpush {r4-r7,lr}\n"
                "\tmov r7, r10\n"
                "\tmov r6, r9\n"
                "\tmov r5, r8\n"
                "\tpush {r5-r7}\n"
                "\tsub sp, 0x8\n"
                "\tlsls r0, 24\n"
                "\tlsls r1, 24\n"
                "\tlsrs r7, r1, 24\n"
                "\tmovs r1, 0\n"
                "\tmov r10, r1\n"
                "\tmovs r2, 0\n"
                "\tstr r2, [sp]\n"
                "\tcmp r0, 0\n"
                "\tbne _080C4198\n"
                "\tmov r8, r2\n"
                "\tldr r0, _080C417C @ =gSharedMem + 0x18018\n"
                "\tsubs r1, 0x18\n"
                "\tadds r1, r0\n"
                "\tmov r9, r1\n"
                "\tadds r4, r0, 0\n"
                "\tadds r4, 0x8\n"
                "\tmovs r6, 0xA0\n"
                "_080C4102:\n"
                "\tldrb r0, [r4, 0x9]\n"
                "\tcmp r7, r0\n"
                "\tbcs _080C416A\n"
                "\tadds r0, 0x13\n"
                "\tadds r0, r6, r0\n"
                "\tsubs r0, r7\n"
                "\tlsls r0, 1\n"
                "\tldr r2, _080C4180 @ =0x0600bffe\n"
                "\tadds r0, r2\n"
                "\tldr r2, _080C4184 @ =0x000060b3\n"
                "\tadds r1, r2, 0\n"
                "\tstrh r1, [r0]\n"
                "\tldr r0, _080C4188 @ =Task_UpdateContestResultBar\n"
                "\tmovs r1, 0xA\n"
                "\tbl CreateTask\n"
                "\tlsls r0, 24\n"
                "\tlsrs r5, r0, 24\n"
                "\tldr r0, [r4]\n"
                "\tlsls r0, 16\n"
                "\tldrb r1, [r4, 0x9]\n"
                "\tbl __udivsi3\n"
                "\tadds r1, r7, 0x1\n"
                "\tadds r3, r0, 0\n"
                "\tmuls r3, r1\n"
                "\tldr r0, _080C418C @ =0x0000ffff\n"
                "\tands r0, r3\n"
                "\tldr r1, _080C4190 @ =0x00007fff\n"
                "\tcmp r0, r1\n"
                "\tbls _080C4146\n"
                "\tmovs r0, 0x80\n"
                "\tlsls r0, 9\n"
                "\tadds r3, r0\n"
                "_080C4146:\n"
                "\tldr r1, _080C4194 @ =gTasks\n"
                "\tlsls r0, r5, 2\n"
                "\tadds r0, r5\n"
                "\tlsls r0, 3\n"
                "\tadds r0, r1\n"
                "\tmov r1, r8\n"
                "\tstrh r1, [r0, 0x8]\n"
                "\tlsrs r1, r3, 16\n"
                "\tstrh r1, [r0, 0xA]\n"
                "\tmov r2, r9\n"
                "\tldrb r0, [r2, 0x14]\n"
                "\tadds r0, 0x1\n"
                "\tstrb r0, [r2, 0x14]\n"
                "\tmov r0, r10\n"
                "\tadds r0, 0x1\n"
                "\tlsls r0, 24\n"
                "\tlsrs r0, 24\n"
                "\tmov r10, r0\n"
                "_080C416A:\n"
                "\tadds r4, 0x14\n"
                "\tadds r6, 0x60\n"
                "\tmovs r0, 0x1\n"
                "\tadd r8, r0\n"
                "\tmov r1, r8\n"
                "\tcmp r1, 0x3\n"
                "\tble _080C4102\n"
                "\tb _080C4292\n"
                "\t.align 2, 0\n"
                "_080C417C: .4byte gSharedMem + 0x18018\n"
                "_080C4180: .4byte 0x0600bffe\n"
                "_080C4184: .4byte 0x000060b3\n"
                "_080C4188: .4byte Task_UpdateContestResultBar\n"
                "_080C418C: .4byte 0x0000ffff\n"
                "_080C4190: .4byte 0x00007fff\n"
                "_080C4194: .4byte gTasks\n"
                "_080C4198:\n"
                "\tmovs r2, 0\n"
                "\tmov r8, r2\n"
                "\tldr r0, _080C4220 @ =gSharedMem + 0x18018\n"
                "\tmov r12, r0\n"
                "\tmov r9, r2\n"
                "\tmovs r1, 0xC0\n"
                "\tstr r1, [sp, 0x4]\n"
                "_080C41A6:\n"
                "\tmov r6, r9\n"
                "\tadd r6, r12\n"
                "\tldrb r1, [r6, 0x12]\n"
                "\tldrb r0, [r6, 0x10]\n"
                "\tldr r2, _080C4224 @ =0x000060a3\n"
                "\tcmp r0, 0\n"
                "\tbeq _080C41B6\n"
                "\tadds r2, 0x2\n"
                "_080C41B6:\n"
                "\tlsls r0, r1, 24\n"
                "\tasrs r0, 24\n"
                "\tcmp r7, r0\n"
                "\tbge _080C427E\n"
                "\tadds r0, 0x13\n"
                "\tldr r1, [sp, 0x4]\n"
                "\tadds r0, r1, r0\n"
                "\tsubs r0, r7\n"
                "\tlsls r0, 1\n"
                "\tldr r1, _080C4228 @ =0x0600bffe\n"
                "\tadds r0, r1\n"
                "\tstrh r2, [r0]\n"
                "\tldr r0, _080C422C @ =Task_UpdateContestResultBar\n"
                "\tmovs r1, 0xA\n"
                "\tbl CreateTask\n"
                "\tlsls r0, 24\n"
                "\tlsrs r5, r0, 24\n"
                "\tldr r0, [r6, 0xC]\n"
                "\tlsls r0, 16\n"
                "\tldrb r1, [r6, 0x12]\n"
                "\tbl __udivsi3\n"
                "\tadds r1, r7, 0x1\n"
                "\tadds r3, r0, 0\n"
                "\tmuls r3, r1\n"
                "\tldr r0, _080C4230 @ =0x0000ffff\n"
                "\tands r0, r3\n"
                "\tldr r1, _080C4234 @ =0x00007fff\n"
                "\tcmp r0, r1\n"
                "\tbls _080C41FA\n"
                "\tmovs r2, 0x80\n"
                "\tlsls r2, 9\n"
                "\tadds r3, r2\n"
                "_080C41FA:\n"
                "\tldr r1, _080C4238 @ =gTasks\n"
                "\tlsls r2, r5, 2\n"
                "\tadds r0, r2, r5\n"
                "\tlsls r0, 3\n"
                "\tadds r4, r0, r1\n"
                "\tmov r0, r8\n"
                "\tstrh r0, [r4, 0x8]\n"
                "\tldrb r0, [r6, 0x10]\n"
                "\tadds r6, r1, 0\n"
                "\tcmp r0, 0\n"
                "\tbeq _080C423C\n"
                "\tmovs r0, 0x1\n"
                "\tstrh r0, [r4, 0xC]\n"
                "\tldr r0, [sp]\n"
                "\tadds r0, 0x1\n"
                "\tlsls r0, 24\n"
                "\tlsrs r0, 24\n"
                "\tstr r0, [sp]\n"
                "\tb _080C4246\n"
                "\t.align 2, 0\n"
                "_080C4220: .4byte gSharedMem + 0x18018\n"
                "_080C4224: .4byte 0x000060a3\n"
                "_080C4228: .4byte 0x0600bffe\n"
                "_080C422C: .4byte Task_UpdateContestResultBar\n"
                "_080C4230: .4byte 0x0000ffff\n"
                "_080C4234: .4byte 0x00007fff\n"
                "_080C4238: .4byte gTasks\n"
                "_080C423C:\n"
                "\tmov r0, r10\n"
                "\tadds r0, 0x1\n"
                "\tlsls r0, 24\n"
                "\tlsrs r0, 24\n"
                "\tmov r10, r0\n"
                "_080C4246:\n"
                "\tldr r0, _080C4264 @ =gSharedMem + 0x18018\n"
                "\tmov r1, r9\n"
                "\tadds r4, r1, r0\n"
                "\tldrb r1, [r4, 0x10]\n"
                "\tmov r12, r0\n"
                "\tcmp r1, 0\n"
                "\tbeq _080C4268\n"
                "\tadds r0, r2, r5\n"
                "\tlsls r0, 3\n"
                "\tadds r0, r6\n"
                "\tlsrs r2, r3, 16\n"
                "\tldr r1, [r4, 0x8]\n"
                "\tsubs r1, r2\n"
                "\tb _080C4274\n"
                "\t.align 2, 0\n"
                "_080C4264: .4byte gSharedMem + 0x18018\n"
                "_080C4268:\n"
                "\tadds r0, r2, r5\n"
                "\tlsls r0, 3\n"
                "\tadds r0, r6\n"
                "\tlsrs r2, r3, 16\n"
                "\tldr r1, [r4, 0x8]\n"
                "\tadds r1, r2\n"
                "_080C4274:\n"
                "\tstrh r1, [r0, 0xA]\n"
                "\tldr r1, _080C42BC @ =gSharedMem + 0x18000\n"
                "\tldrb r0, [r1, 0x14]\n"
                "\tadds r0, 0x1\n"
                "\tstrb r0, [r1, 0x14]\n"
                "_080C427E:\n"
                "\tmovs r2, 0x14\n"
                "\tadd r9, r2\n"
                "\tldr r0, [sp, 0x4]\n"
                "\tadds r0, 0x60\n"
                "\tstr r0, [sp, 0x4]\n"
                "\tmovs r1, 0x1\n"
                "\tadd r8, r1\n"
                "\tmov r2, r8\n"
                "\tcmp r2, 0x3\n"
                "\tble _080C41A6\n"
                "_080C4292:\n"
                "\tldr r0, [sp]\n"
                "\tcmp r0, 0\n"
                "\tbeq _080C429E\n"
                "\tmovs r0, 0x16\n"
                "\tbl PlaySE\n"
                "_080C429E:\n"
                "\tmov r1, r10\n"
                "\tcmp r1, 0\n"
                "\tbeq _080C42AA\n"
                "\tmovs r0, 0x15\n"
                "\tbl PlaySE\n"
                "_080C42AA:\n"
                "\tadd sp, 0x8\n"
                "\tpop {r3-r5}\n"
                "\tmov r8, r3\n"
                "\tmov r9, r4\n"
                "\tmov r10, r5\n"
                "\tpop {r4-r7}\n"
                "\tpop {r0}\n"
                "\tbx r0\n"
                "\t.align 2, 0\n"
                "_080C42BC: .4byte gSharedMem + 0x18000");
}
#endif //NONMATCHING

void Task_UpdateContestResultBar(u8 taskId /*r12*/)
{
    bool32 minMaxReached = FALSE;
    bool32 targetReached = FALSE;
    u8 monId = gTasks[taskId].data[0];
    s16 target = gTasks[taskId].data[1];
    s16 decreasing = gTasks[taskId].data[2];
    s32 i;

    if (decreasing != 0)
    {
        if (eContestLink80C2020Struct2018000.barLength[monId] <= 0)
            minMaxReached = TRUE;
    }
    else
    {
        if (eContestLink80C2020Struct2018000.barLength[monId] >= 88)
            minMaxReached = TRUE;
    }
    if (eContestLink80C2020Struct2018000.barLength[monId] == target)
        targetReached = TRUE;

    if (!targetReached)
    {
        if (minMaxReached)
        {
            eContestLink80C2020Struct2018000.barLength[monId] = target;
        }
        else if (decreasing != 0)
        {
            eContestLink80C2020Struct2018000.barLength[monId]--;
        }
        else
        {
            eContestLink80C2020Struct2018000.barLength[monId]++;
        }
    }
    if (!minMaxReached)
    {
        if (!targetReached)
        {
            for (i = 0; i < 11; i++)
            {
                u8 tileOffset;
                u16 tileNum;
                if (eContestLink80C2020Struct2018000.barLength[monId] >= 8 * (i + 1))
                {
                    tileOffset = 8;
                }
                else if (eContestLink80C2020Struct2018000.barLength[monId] >= 8 * i)
                {
                    tileOffset = eContestLink80C2020Struct2018000.barLength[monId] % 8;
                }
                else
                {
                    tileOffset = 0;
                }
                if (tileOffset < 4)
                    tileNum = 0x504C + tileOffset;
                else
                    tileNum = 0x5057 + tileOffset;
                *(vu16 *)((VRAM + 0xE18E) + 2 * (96 * monId + i)) = tileNum;
            }
        }
    }
    if (targetReached)
    {
        eContestLink80C2020Struct2018000.numBarsUpdating--;
        DestroyTask(taskId);
    }
}

void TryEnterContestMon(void)
{
    u8 eligibility = GetContestEntryEligibility(&gPlayerParty[gContestMonPartyIndex]);
    if (eligibility != 0)
    {
        SetContestants(gSpecialVar_ContestCategory, gSpecialVar_ContestRank);
        CalculateRound1Points(gSpecialVar_ContestCategory);
    }
    gSpecialVar_Result = eligibility;
}

u16 HasMonWonThisContestBefore(void)
{
    u16 hasRankRibbon = 0;
    struct Pokemon *mon = &gPlayerParty[gContestMonPartyIndex];
    switch (gSpecialVar_ContestCategory)
    {
    case CONTEST_CATEGORY_COOL:
        if (GetMonData(mon, MON_DATA_COOL_RIBBON) > gSpecialVar_ContestRank)
            hasRankRibbon = 1;
        break;
    case CONTEST_CATEGORY_BEAUTY:
        if (GetMonData(mon, MON_DATA_BEAUTY_RIBBON) > gSpecialVar_ContestRank)
            hasRankRibbon = 1;
        break;
    case CONTEST_CATEGORY_CUTE:
        if (GetMonData(mon, MON_DATA_CUTE_RIBBON) > gSpecialVar_ContestRank)
            hasRankRibbon = 1;
        break;
    case CONTEST_CATEGORY_SMART:
        if (GetMonData(mon, MON_DATA_SMART_RIBBON) > gSpecialVar_ContestRank)
            hasRankRibbon = 1;
        break;
    case CONTEST_CATEGORY_TOUGH:
        if (GetMonData(mon, MON_DATA_TOUGH_RIBBON) > gSpecialVar_ContestRank)
            hasRankRibbon = 1;
        break;
    }

    return hasRankRibbon;
}


void GiveMonContestRibbon(void)
{
    u8 ribbonData;

    if (gContestFinalStandings[gContestPlayerMonIndex] != 0)
        return;

    switch (gSpecialVar_ContestCategory)
    {
    case CONTEST_CATEGORY_COOL:
        ribbonData = GetMonData(&gPlayerParty[gContestMonPartyIndex], MON_DATA_COOL_RIBBON);
        if (ribbonData <= gSpecialVar_ContestRank && ribbonData < 4)
        {
            ribbonData++;
            SetMonData(&gPlayerParty[gContestMonPartyIndex], MON_DATA_COOL_RIBBON, &ribbonData);
        }
        break;
    case CONTEST_CATEGORY_BEAUTY:
        ribbonData = GetMonData(&gPlayerParty[gContestMonPartyIndex], MON_DATA_BEAUTY_RIBBON);
        if (ribbonData <= gSpecialVar_ContestRank && ribbonData < 4)
        {
            ribbonData++;
            SetMonData(&gPlayerParty[gContestMonPartyIndex], MON_DATA_BEAUTY_RIBBON, &ribbonData);
        }
        break;
    case CONTEST_CATEGORY_CUTE:
        ribbonData = GetMonData(&gPlayerParty[gContestMonPartyIndex], MON_DATA_CUTE_RIBBON);
        if (ribbonData <= gSpecialVar_ContestRank && ribbonData < 4)
        {
            ribbonData++;
            SetMonData(&gPlayerParty[gContestMonPartyIndex], MON_DATA_CUTE_RIBBON, &ribbonData);
        }
        break;
    case CONTEST_CATEGORY_SMART:
        ribbonData = GetMonData(&gPlayerParty[gContestMonPartyIndex], MON_DATA_SMART_RIBBON);
        if (ribbonData <= gSpecialVar_ContestRank && ribbonData < 4)
        {
            ribbonData++;
            SetMonData(&gPlayerParty[gContestMonPartyIndex], MON_DATA_SMART_RIBBON, &ribbonData);
        }
        break;
    case CONTEST_CATEGORY_TOUGH:
        ribbonData = GetMonData(&gPlayerParty[gContestMonPartyIndex], MON_DATA_TOUGH_RIBBON);
        if (ribbonData <= gSpecialVar_ContestRank && ribbonData < 4)
        {
            ribbonData++;
            SetMonData(&gPlayerParty[gContestMonPartyIndex], MON_DATA_TOUGH_RIBBON, &ribbonData);
        }
        break;
    }
}

void Contest_CopyAndConvertTrainerName_Intl(u8 * dest, const u8 * src)
{
    StringCopy(dest, src);
    if (dest[0] == EXT_CTRL_CODE_BEGIN && dest[1] == 0x15)
        ConvertInternationalString(dest, LANGUAGE_JAPANESE);
}

void Contest_CopyAndConvertNicknameI_Intl(u8 * dest, u8 idx)
{
    StringCopy(dest, gContestMons[idx].nickname);
    if (gLinkContestFlags & 1)
    {
        if (gLinkPlayers[idx].language == LANGUAGE_JAPANESE)
        {
            ConvertInternationalString(dest, GetMonNicknameLanguage(dest));
        }
    }
}

void BufferContestantTrainerName(void)
{
    if (gLinkContestFlags & 1)
    {
        Contest_CopyAndConvertTrainerName_Intl(gStringVar1, gLinkPlayers[gSpecialVar_0x8006].name);
    }
    else
    {
        Contest_CopyAndConvertTrainerName_Intl(gStringVar1, gContestMons[gSpecialVar_0x8006].trainerName);
    }
}

void BufferContestantMonNickname(void)
{
    Contest_CopyAndConvertNicknameI_Intl(gStringVar3, gSpecialVar_0x8006);
}

void GetContestMonConditionRanking(void)
{
    u8 i;
    u8 rank;

    for (i = 0, rank = 0; i < 4; i++)
    {
        if (gContestMonRound1Points[gSpecialVar_0x8006] < gContestMonRound1Points[i])
            rank++;
    }

    gSpecialVar_0x8004 = rank;
}

void GetContestMonCondition(void)
{
    gSpecialVar_0x8004 = gContestMonRound1Points[gSpecialVar_0x8006];
}

void GetContestWinnerId(void)
{
    u8 i;

    for (i = 0; i < 4 && gContestFinalStandings[i] != 0; i++)
        ;

    gSpecialVar_0x8005 = i;
}

void BufferContestWinnerTrainerName(void)
{
    u8 i;

    for (i = 0; i < 4 && gContestFinalStandings[i] != 0; i++)
        ;

    if (gLinkContestFlags & 1)
    {
        Contest_CopyAndConvertTrainerName_Intl(gStringVar3, gLinkPlayers[i].name);
    }
    else
    {
        Contest_CopyAndConvertTrainerName_Intl(gStringVar3, gContestMons[i].trainerName);
    }
}

void BufferContestWinnerMonName(void)
{
    u8 i;

    for (i = 0; i < 4 && gContestFinalStandings[i] != 0; i++)
        ;

    Contest_CopyAndConvertNicknameI_Intl(gStringVar1, i);
}

void CB2_SetStartContestCallback(void)
{
    SetMainCallback2(CB2_StartContest);
}

static void Task_StartContest(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        DestroyTask(taskId);
        SetMainCallback2(CB2_SetStartContestCallback);
    }
}

void StartContest(void)
{
    LockPlayerFieldControls();
    CreateTask(Task_StartContest, 10);
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB_BLACK);
}

void BufferContestantMonSpecies(void)
{
    gSpecialVar_0x8004 = gContestMons[gSpecialVar_0x8006].species;
}

static void Task_StartShowContestResults(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        DestroyTask(taskId);
        SetMainCallback2(CB2_StartShowContestResults);
    }
}

void ShowContestResults(void)
{
    LockPlayerFieldControls();
    CreateTask(Task_StartShowContestResults, 10);
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB_BLACK);
}

void GetContestPlayerId(void)
{
    gSpecialVar_0x8004 = gContestPlayerMonIndex;
}

void ContestLinkTransfer(u8 category)
{
    u8 newTaskId;
    LockPlayerFieldControls();
    newTaskId = CreateTask(Task_LinkContest_Init, 0);
    SetTaskFuncWithFollowupFunc(newTaskId, Task_LinkContest_Init, Task_StartCommunication);
    gTasks[newTaskId].data[9] = category;
}

static void Task_StartCommunication(u8 taskId)
{
    CreateContestMonFromParty(gContestMonPartyIndex);
    SetTaskFuncWithFollowupFunc(taskId, Task_LinkContest_CommunicateMonsRS, Task_StartCommunicateRngRS);
}

static void Task_StartCommunicateRngRS(u8 taskId)
{
    SetTaskFuncWithFollowupFunc(
        taskId, Task_LinkContest_CommunicateRngRS, Task_StartCommunicateLeaderIdsRS);
}

static void Task_StartCommunicateLeaderIdsRS(u8 taskId)
{
    SetTaskFuncWithFollowupFunc(
        taskId, Task_LinkContest_CommunicateLeaderIdsRS, Task_StartCommunicateCategoryRS);
}

static void Task_StartCommunicateCategoryRS(u8 taskId)
{
    SetTaskFuncWithFollowupFunc(
        taskId, Task_LinkContest_CommunicateCategoryRS, Task_LinkContest_SetUpContestRS);
}

static void Task_LinkContest_SetUpContestRS(u8 taskId)
{
    u8 i;
    u8 categories[4];
    u8 leaderIds[4];

    for (i = 0; i < 4; i++)
        categories[i] = gTasks[taskId].data[i + 1];

    for (i = 0; i < 4; i++)
    {
        if (categories[0] != categories[i])
            break;
    }

    if (i == 4)
        gSpecialVar_0x8004 = 0;
    else
        gSpecialVar_0x8004 = 1;

    for (i = 0; i < 4; i++)
        leaderIds[i] = gTasks[taskId].data[i + 5];

    gContestLinkLeaderIndex = LinkContest_GetLeaderIndex(leaderIds);
    CalculateRound1Points(gSpecialVar_ContestCategory);
    SetTaskFuncWithFollowupFunc(
        taskId, Task_LinkContest_CommunicateRound1Points, Task_LinkContest_CalculateTurnOrderRS);
}

static void Task_LinkContest_CalculateTurnOrderRS(u8 taskId)
{
    SortContestants(0);
    SetTaskFuncWithFollowupFunc(
        taskId, Task_LinkContest_CommunicateTurnOrder, Task_LinkContest_FinalizeConnection);
}

u8 LinkContest_GetLeaderIndex(u8 * ids)
{
    s32 i;
    u8 leaderIdx = 0;

    for (i = 1; i < 4; i++)
    {
        if (ids[leaderIdx] < ids[i])
            leaderIdx = i;
    }

    return leaderIdx;
}

void Task_LinkContest_FinalizeConnection(u8 taskId)
{
    if (gSpecialVar_0x8004 == 1)
    {
        if (IsLinkTaskFinished())
            gTasks[taskId].func = Task_LinkContest_Disconnect;
    }
    else
    {
        DestroyTask(taskId);
        UnlockPlayerFieldControls();
        ScriptContext_Enable();
    }
}

static void Task_LinkContest_Disconnect(u8 taskId)
{
    SetCloseLinkCallback();
    gTasks[taskId].func = Task_LinkContest_WaitDisconnect;
}

static void Task_LinkContest_WaitDisconnect(u8 taskId)
{
    if (!gReceivedRemoteLinkPlayers)
    {
        DestroyTask(taskId);
        UnlockPlayerFieldControls();
        ScriptContext_Enable();
    }
}
