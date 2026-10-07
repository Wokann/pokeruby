#include "global.h"
#include "math_util.h"
#include "random.h"
#include "graphics.h"
#include "ewram.h"
#include "field_screen_effect.h"
#include "constants/game_stat.h"
#include "m4a.h"
#include "main.h"
#include "menu.h"
#include "menu_cursor.h"
#include "menu_helpers.h"
#include "overworld.h"
#include "palette.h"
#include "pokemon.h"
#include "roulette.h"
#include "palette_util.h"
#include "rtc.h"
#include "constants/songs.h"
#include "sound.h"
#include "script.h"
#include "constants/species.h"
#include "sprite.h"
#include "strings2.h"
#include "strings.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "trig.h"
#include "scanline_effect.h"

asm(".include \"constants/gba_constants.inc\"");

#if DEBUG
void *gUnknown_Debug_03005FB8;
#endif

void sub_81150FC(void);
void sub_8115124(void);
void sub_8115238(void);
void sub_8115384(void);
void sub_8115634(u8);
void sub_81156BC(u8);
void sub_8115734(u8);
void sub_811577C(u8);
static void Task_StopPlaying(u8);
static void UpdateGridSelectionRect(u8);
void sub_8115928(u8, u8);
static void Task_StartHandleBetGridInput(u8);
static void Task_SelectFirstEmptySquare(u8);
static u8 CanMoveSelectionInDir(s16 *, u8);
void sub_8115B58(u8);
static void Task_PlaceBet(u8);
void sub_8115E14(u8);
static void Task_SlideGridOffscreen(u8);
static u8 GetRandomForBallTravelDistance(u16, u16);
static void Task_InitBallRoll(u8);
static void Task_RollBall(u8);
static void Task_RecordBallHit(u8);
static void Task_SlideGridOnscreen(u8);
static void Task_FlashBallOnWinningSquare(u8);
static void Task_TryIncrementWins(u8);
void sub_8116638(u8);
static void Task_GivePayout(u8);
void sub_811677C(u8);
static void Task_EndTurn(u8);
void sub_8116880(u8);
void dp01t_12_3_battle_menu(u8);
void sub_8116AB0(u8);
void sub_8116B40(u8);
static void Task_WaitForNextTask(u8);
static void StartTaskAfterDelayOrInput(u8, TaskFunc, u16, u16);
static void ResetBallDataForNewSpin(u8);
static void ResetHits(void);
static u8 RecordHit(u8, u8);
static u8 IsHitInBetSelection(u8, u8);
static void FlashSelectionOnWheel(u8);
static void DrawGridBackground(u8);
static u8 GetMultiplier(u8);
static void UpdateWheelPosition(void);
void sub_81174F8(u8);
static void Task_FadeToRouletteGame(u8);
void sub_811755C(u8);
void sub_81175C0(u8);
void sub_81175DC(u8);
void sub_8117630(u8);
void Task_Roulette_0(u8);

static const u16 sWheel_Pal[] = INCBIN_U16("graphics/roulette/wheel.gbapal");
static const u8 sGrid_Tilemap[] = INCBIN_U8("graphics/roulette/grid.bin.lz");
static const u8 sWheel_Tilemap[] = INCBIN_U8("graphics/roulette/wheel.bin.lz");

const struct GridSelection gUnknown_083F8C00[] = {
	{255, 0, 0, 0, 7, 7, 0, 0, 0, 0, 0, 0, 0},
	{12, 4, 1, 0, 17, 7, 0, 0, 0, 2, 0x10842, 0xe000, 0},
	{13, 4, 2, 0, 20, 7, 0, 0, 0, 4, 0x21084, 0xe000, 0},
	{14, 4, 3, 0, 23, 7, 0, 0, 0, 8, 0x42108, 0xe000, 0},
	{15, 4, 4, 0, 26, 7, 0, 0, 0, 16, 0x84210, 0xe000, 0},
	{16, 3, 0, 1, 14, 10, 0, 12, 0, 32, 0x3e0, 0x249, 0},
	{0, 12, 1, 1, 17, 10, 3, 3, 0, 64, 64, 0x2001, 0},
	{9, 12, 2, 1, 20, 10, 3, 3, 0, 128, 128, 0x2200, 0},
	{6, 12, 3, 1, 23, 10, 3, 3, 0, 0x100, 0x100, 0x2040, 0},
	{3, 12, 4, 1, 26, 10, 3, 3, 0, 0x200, 0x200, 0x2008, 0},
	{17, 3, 0, 2, 14, 13, 3, 15, 0, 0x400, 0x7c00, 0x492, 0},
	{4, 12, 1, 2, 17, 13, 6, 6, 0, 0x800, 0x800, 0x2010, 0},
	{1, 12, 2, 2, 20, 13, 6, 6, 0, 0x1000, 0x1000, 0x2002, 0},
	{10, 12, 3, 2, 23, 13, 6, 6, 0, 0x2000, 0x2000, 0x2400, 0},
	{7, 12, 4, 2, 26, 13, 6, 6, 0, 0x4000, 0x4000, 0x2080, 0},
	{18, 3, 0, 3, 14, 16, 6, 18, 0, 0x8000, 0xf8000, 0x924, 0},
	{8, 12, 1, 3, 17, 16, 9, 9, 0, 0x10000, 0x10000, 0x2100, 0},
	{5, 12, 2, 3, 20, 16, 9, 9, 0, 0x20000, 0x20000, 0x2020, 0},
	{2, 12, 3, 3, 23, 16, 9, 9, 0, 0x40000, 0x40000, 0x2004, 0},
	{11, 12, 4, 3, 26, 16, 9, 9, 0, 0x80000, 0x80000, 0x2800, 0},
};

const struct RouletteSlot gUnknown_083F8D90[] = {
    { 0,  1,  6, 64},
    { 1,  3, 12, 0x1000},
    { 2,  5, 18, 0x40000},
    { 3,  7,  9, 0x200},
    { 4,  9, 11, 0x800},
    { 5, 11, 17, 0x20000},
    { 6, 13,  8, 0x100},
    { 7, 15, 14, 0x4000},
    { 8, 17, 16, 0x10000},
    { 9, 19,  7, 128},
    {10, 21, 13, 0x2000},
    {11, 23, 19, 0x80000}
};

static const u8 sTableMinBets[] = {
    1, 3,
    1, 6,
};

const struct RouletteTable gUnknown_083F8DF4[] = {
    {
        .minBet = 1,
        .randDistanceHigh = 60,
        .randDistanceLow = 30,
        .wheelSpeed = 1,
        .wheelDelay = 1,
        .var08 = 45,
        .var0A = 30,
        .var0C = 1,
        .var10 = 75,
        .var12 = 27,
        .var14 = 24,
        .ballSpeed = 10,
        .baseTravelDist = 360,
        .var1C = -0.5f
    }, {
        .minBet = 3,
        .randDistanceHigh = 30,
        .randDistanceLow = 15,
        .wheelSpeed = 1,
        .wheelDelay = 0,
        .var08 = 75,
        .var0A = 60,
        .var0C = 2,
        .var10 = 0,
        .var12 = 54,
        .var14 = 48,
        .ballSpeed = 10,
        .baseTravelDist = 270,
        .var1C = -1.0f
    }
};

static const struct RouletteFlashSettings sFlashData_Colors[] = {
	{
        .color = 0x8000,
        .paletteOffset = 5,
        .numColors = 1,
        .delay = 1,
        .unk6 = 255,
        .numFadeCycles = 8,
        .unk7_5 = 2,
        .colorDeltaDir = FALSE
    }, {
        .color = 0x8000,
        .paletteOffset = 10,
        .numColors = 1,
        .delay = 1,
        .unk6 = 255,
        .numFadeCycles = 8,
        .unk7_5 = 2,
        .colorDeltaDir = FALSE
    }, {
        .color = 0x8000,
        .paletteOffset = 21,
        .numColors = 1,
        .delay = 1,
        .unk6 = 255,
        .numFadeCycles = 8,
        .unk7_5 = 2,
        .colorDeltaDir = FALSE
    }, {
        .color = 0x8000,
        .paletteOffset = 85,
        .numColors = 1,
        .delay = 1,
        .unk6 = 255,
        .numFadeCycles = 8,
        .unk7_5 = 2,
        .colorDeltaDir = FALSE
    }, {
        .color = 0x8000,
        .paletteOffset = 90,
        .numColors = 1,
        .delay = 1,
        .unk6 = 255,
        .numFadeCycles = 8,
        .unk7_5 = 2,
        .colorDeltaDir = FALSE
    }, {
        .color = 0x8000,
        .paletteOffset = 101,
        .numColors = 1,
        .delay = 1,
        .unk6 = 255,
        .numFadeCycles = 8,
        .unk7_5 = 2,
        .colorDeltaDir = FALSE
    }, {
        .color = 0x8000,
        .paletteOffset = 117,
        .numColors = 1,
        .delay = 1,
        .unk6 = 255,
        .numFadeCycles = 8,
        .unk7_5 = 2,
        .colorDeltaDir = FALSE
    }, {
        .color = 0x8000,
        .paletteOffset = 122,
        .numColors = 1,
        .delay = 1,
        .unk6 = 255,
        .numFadeCycles = 8,
        .unk7_5 = 2,
        .colorDeltaDir = FALSE
    }, {
        .color = 0x8000,
        .paletteOffset = 133,
        .numColors = 1,
        .delay = 1,
        .unk6 = 255,
        .numFadeCycles = 8,
        .unk7_5 = 2,
        .colorDeltaDir = FALSE
    }, {
        .color = 0x8000,
        .paletteOffset = 149,
        .numColors = 1,
        .delay = 1,
        .unk6 = 255,
        .numFadeCycles = 8,
        .unk7_5 = 2,
        .colorDeltaDir = FALSE
    }, {
        .color = 0x8000,
        .paletteOffset = 154,
        .numColors = 1,
        .delay = 1,
        .unk6 = 255,
        .numFadeCycles = 8,
        .unk7_5 = 2,
        .colorDeltaDir = FALSE
    }, {
        .color = 0x8000,
        .paletteOffset = 165,
        .numColors = 1,
        .delay = 1,
        .unk6 = 255,
        .numFadeCycles = 8,
        .unk7_5 = 2,
        .colorDeltaDir = FALSE
    }, {
        .color = 0x77d6,
        .paletteOffset = 40,
        .numColors = 2,
        .delay = 10,
        .unk6 = 255,
        .numFadeCycles = 14,
        .unk7_5 = 2,
        .colorDeltaDir = FALSE
    }
};

static const struct RouletteFlashSettings sFlashData_PokeIcons[] = {
    {
        .color = 0x53ff,
        .paletteOffset = 0x101,
        .numColors = 5,
        .delay = 30,
        .unk6 = 255,
        .numFadeCycles = 14,
        .unk7_5 = 2,
        .colorDeltaDir = FALSE
    }, {
        .color = 0x7ffb,
        .paletteOffset = 0x106,
        .numColors = 5,
        .delay = 30,
        .unk6 = 255,
        .numFadeCycles = 14,
        .unk7_5 = 2,
        .colorDeltaDir = FALSE
    }, {
        .color = 0x7f7f,
        .paletteOffset = 0x10b,
        .numColors = 5,
        .delay = 30,
        .unk6 = 255,
        .numFadeCycles = 14,
        .unk7_5 = 2,
        .colorDeltaDir = FALSE
    }
};

static const struct YesNoFuncTable sYesNoTable_AcceptMinBet = {
    sub_811755C,
    sub_81175C0
};

static const struct YesNoFuncTable sYesNoTable_KeepPlaying = {
    sub_811577C,
    Task_StopPlaying
};

extern u8 gUnknown_02019000[];
extern u16 gSpecialVar_0x8004;
extern u16 gPlttBufferFaded[];
extern u16 gPlttBufferUnfaded[];
extern const u16 gUnknown_083F8ECE;
const extern u8 gUnknown_083F8ECA[];
extern const u32 gUnknown_083F8ED8[];
extern const u32 gUnknown_083F8EE8[];
extern const u8 gUnknown_083F8EF4[];
#if DEBUG
EWRAM_DATA u8 unk_203955C[4] = { 0 };
EWRAM_DATA u8 unk_2039560 = 0;
#endif


void sub_81150FC(void)
{
	RunTasks();
	AnimateSprites();
	BuildOamBuffer();
	if (eRoulette->flashUtil.enabled)
	   RouletteFlash_Run(&eRoulette->flashUtil);
}

void sub_8115124(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    UpdateWheelPosition();
    REG_BG1HOFS = 0x200 - eRoulette->gridX;
    if (eRoulette->shroomishShadowTimer)
        REG_BLDALPHA = eRoulette->shroomishShadowAlpha;
    if (eRoulette->updateGridHighlight != 0)
    {
        DmaCopy16(3, &gBGTilemapBuffers[1][224], (void *)(VRAM + 0x21c0), 0x340);
        eRoulette->updateGridHighlight = 0;
    }
    switch (eRoulette->selectionRectDrawState)
    {
    case 1:
        REG_BG0CNT = (32 * 248);
        DmaCopy16(3, &gBGTilemapBuffers[2][224], (void *)(VRAM + 0xf9c0), 0x340);
        eRoulette->selectionRectDrawState = 2;
    break;
    case 2:
        DmaCopy16(3, &gBGTilemapBuffers[2][224], (void *)(VRAM + 0xf9c0), 0x340);
    break;
    case 255:
        REG_BG0CNT = 0x1f08;
        DmaFill16(3, 0, (void *)(VRAM + 0xf9c0), 0x340);
        eRoulette->selectionRectDrawState = 0;
    case 0:
        break;
    }
}

void sub_8115238(void)
{
    u8 i;
    u32 temp;
    const struct RouletteTable *s0;
    u16 bgColors[] = {RGB(24, 4, 10), RGB(10, 19, 6), RGB(24, 4, 10)};
    memset(eRoulette, 0, sizeof(*eRoulette));
    eRoulette->tableId = (gSpecialVar_0x8004 & 1);
    if (gSpecialVar_0x8004 & 0x80)
        eRoulette->isSpecialRate = 1;
    s0 = gUnknown_083F8DF4;
    eRoulette->wheelSpeed   = s0[eRoulette->tableId].wheelSpeed;
    eRoulette->wheelDelay   = s0[eRoulette->tableId].wheelDelay;
    eRoulette->minBet = temp = sTableMinBets[eRoulette->tableId + eRoulette->isSpecialRate * 2];
    eRoulette->unk1 = 1;
    if (temp == 1)
        gPlttBufferUnfaded[0] = gPlttBufferUnfaded[81] = gPlttBufferFaded[0] = gPlttBufferFaded[81] = bgColors[0];
    else
        gPlttBufferUnfaded[0] = gPlttBufferUnfaded[81] = gPlttBufferFaded[0] = gPlttBufferFaded[81] = bgColors[1];
    RouletteFlash_Reset((&eRoulette->flashUtil));
    for (i = 0; i < 13; i++)
        RouletteFlash_Add((&eRoulette->flashUtil), i, &sFlashData_Colors[i]);
    for (i = 0; i < 6; i++)
    {
        switch (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG))
        {
        case SPECIES_SHROOMISH:
            eRoulette->partySpeciesFlags |= 1;
            break;
        case SPECIES_TAILLOW:
            eRoulette->partySpeciesFlags |= 2;
            break;
        }
    }
    RtcCalcLocalTime();
}

void sub_8115384(void)
{
    u32 temp_IME;
    u8 taskId;

#if DEBUG
    gUnknown_Debug_03005FB8 = eRoulette;
#endif

    switch (gMain.state)
    {
    case 0:
        SetVBlankCallback(NULL);
        ScanlineEffect_Stop();
        SetVBlankHBlankCallbacksToNull();
        ResetVramOamAndBgCntRegs();
        REG_BG2CNT   = BGCNT_PRIORITY(2) | BGCNT_CHARBASE(1) | BGCNT_256COLOR | BGCNT_SCREENBASE(6) | BGCNT_TXT512x256;
        REG_BG1CNT   = BGCNT_PRIORITY(1) | BGCNT_CHARBASE(0) | BGCNT_16COLOR | BGCNT_SCREENBASE(4) | BGCNT_TXT512x256;
        REG_BLDCNT   = BLDCNT_EFFECT_NONE | BLDCNT_TGT2_BG2 | BLDCNT_TGT2_BD;
        REG_BLDALPHA = BLDALPHA_BLEND(10, 6);
        LZ77UnCompVram(gRouletteMenu_Gfx, (void *)VRAM);
        LZ77UnCompVram(gRouletteWheel_Gfx, (void *)(VRAM + 0x4000));
        gMain.state++;
        break;
    case 1:
        ResetPaletteFade();
        ResetSpriteData();
        ResetTasks();
        gMain.state++;
        break;
    case 2:
        Text_LoadWindowTemplate(&gWindowTemplate_81E6C3C);
        InitMenuWindow(&gMenuTextWindowTemplate);
        LoadPalette(sWheel_Pal, 0, 0x1c0);
        gMain.state++;
        break;
    case 3:
        sub_8115238();
        ClearBGTilemapBuffers();
        LZ77UnCompWram(sGrid_Tilemap, (void *)(ewram18800));
        LZ77UnCompVram(sWheel_Tilemap, (void *)(VRAM + 0x3000));
        gMain.state++;
        break;
    case 4:
        sub_8117838(0);
        CreateWheelBallSprites();
        CreateWheelCenterSprite();
        CreateInterfaceSprites();
        CreateGridSprites();
        CreateGridBallSprites();
        CreateWheelIconSprites();
        gMain.state++;
        break;
    case 5:
        AnimateSprites();
        BuildOamBuffer();
        SetCreditDigits(gSaveBlock1.coins);
        SetBallCounterNumLeft(6);
        SetMultiplierSprite(0);
        DrawGridBackground(0);
        Menu_DrawStdWindowFrame(0, 14, 29, 19);
        Menu_PrintText(Roulette_Text_ControlsInstruction, 1, 15);
        gSpriteCoordOffsetX = -60;
        gSpriteCoordOffsetY = 0;
        gMain.state++;
        break;
    case 6:
        REG_DISPCNT = DISPCNT_MODE_1 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG1_ON | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
        gMain.state++;
        break;
    case 7:
        temp_IME      = REG_IME;
        REG_IME       = 0;
        REG_IE       |= INTR_FLAG_VBLANK;
        REG_IME       = temp_IME;
        REG_DISPSTAT |= 8;
        SetVBlankCallback(sub_8115124);
        BeginHardwarePaletteFade(255, 0, 16, 0 , 1);
        taskId = eRoulette->playTaskId = CreateTask(sub_81156BC, 0);
        gTasks[taskId].data[6] = 6;
        gTasks[taskId].data[13] = gSaveBlock1.coins;
        eRoulette->spinTaskId = CreateTask(sub_8115634, 1);
        SetMainCallback2(sub_81150FC);
        break;
    }
}

void sub_8115634(u8 taskId)
{
    s16 sin;
    s16 cos;
    s32 cos32;

    if (eRoulette->wheelDelayTimer++ == eRoulette->wheelDelay)
    {
        eRoulette->wheelDelayTimer = 0;
        if ((eRoulette->wheelAngle -= eRoulette->wheelSpeed) < 0)
            eRoulette->wheelAngle = 360 - eRoulette->wheelSpeed;
    }
    sin = Sin2(eRoulette->wheelAngle);
    cos = Cos2(eRoulette->wheelAngle);
    sin = sin / 16;
    cos32 = cos / 16;
    eRoulette->wheelRotation.d =  cos32;
    eRoulette->wheelRotation.a =  cos32;
    eRoulette->wheelRotation.b =  sin;
    eRoulette->wheelRotation.c = -sin;
#if DEBUG
    if (unk_203955C[0] != 0 && JOY_NEW(START_BUTTON))
    {
        gTasks[eRoulette->playTaskId].func = Task_StopPlaying;
        m4aMPlayStop(&gMPlayInfo_SE1);
        m4aMPlayStop(&gMPlayInfo_SE2);
        m4aMPlayStop(&gMPlayInfo_SE3);
    }
#endif
}

void sub_81156BC(u8 taskId)
{
    if (UpdatePaletteFade() == 0)
    {
        REG_BLDCNT   |= BLDCNT_TGT2_BG2 | BLDCNT_TGT2_BD;
        REG_BLDALPHA  = BLDALPHA_BLEND(8, 8);
        gTasks[taskId].data[6] = 0;
        ResetBallDataForNewSpin(taskId);
        ResetHits();
        HideWheelBalls();
        DrawGridBackground(0);
        SetBallCounterNumLeft(6);
        StartTaskAfterDelayOrInput(taskId, Task_SelectFirstEmptySquare, 0xffff, 3);
    }
}

void sub_8115734(u8 taskId)
{
    DisplayYesNoMenu(20, 8, 1);
    sub_814AAF8(RGB(30, 12, 11));
    Menu_DrawStdWindowFrame(0, 14, 29, 19);
    Menu_PrintText(Roulette_Text_KeepPlaying, 1, 15);
    DoYesNoFuncWithChoice(taskId, &sYesNoTable_KeepPlaying);
}

void sub_811577C(u8 taskId)
{
    Menu_EraseWindowRect(20, 8, 26, 13);
    gTasks[taskId].func = Task_SelectFirstEmptySquare;
}

static void Task_StopPlaying(u8 taskId)
{
    DestroyTask(eRoulette->spinTaskId);
    sub_8116AB0(taskId);
}

static void UpdateGridSelectionRect(u8 selectionId)
{
    u8 temp0, temp1;
    switch (selectionId)
    {
    case 0:
        FillTilemapRect(&gBGTilemapBuffers[2][0], 0, 14, 7, 16, 13);
        break;
    case 1:
    case 2:
    case 3:
    case 4:
        temp0 = (selectionId * 3 + 14);
        FillTilemapRect(&gBGTilemapBuffers[2][0], 0, 14, 7, 16, 13);
        SetTilemapRect(&gBGTilemapBuffers[2][0], ewram18800 + 0x119, temp0, 7, 3, 13);
        break;
    case 5:
    case 10:
    case 15:
        temp1 = ((selectionId - 1) / 5 * 3 + 10);
        FillTilemapRect(&gBGTilemapBuffers[2][0], 0, 14, 7, 16, 13);
        SetTilemapRect(&gBGTilemapBuffers[2][0], ewram18800 + 0x140, 14, temp1, 16, 3);
        break;
    default:
        temp0 = ((selectionId % 5) * 3 + 14);
        temp1 = ((selectionId - 1) / 5 * 3 + 7);
        FillTilemapRect(&gBGTilemapBuffers[2][0], 0, 14, 7, 16, 13);
        SetTilemapRect(&gBGTilemapBuffers[2][0], ewram18800 + 0x110, temp0, temp1, 3, 3);
        break;
    }
}

void sub_8115928(u8 taskId, u8 r1)
{
    if (!r1)
        SetMultiplierSprite(gTasks[taskId].data[4]);
    else
        SetMultiplierSprite(eRoulette->betSelection[eRoulette->curBallNum]);
    UpdateGridSelectionRect(gTasks[taskId].data[4]);
}

static void Task_StartHandleBetGridInput(u8 taskId)
{
    eRoulette->selectionRectDrawState = 1;
    UpdateGridSelectionRect(gTasks[taskId].data[4]);
    eRoulette->wheelDelay = 2;
    eRoulette->wheelDelayTimer = 0;
    gTasks[taskId].func = sub_8115E14;
}

static void Task_SelectFirstEmptySquare(u8 taskId)
{
    s16 i;

    if (eRoulette->hitFlags & 0x20)
    {
        for (i = 11; (i < 14); i++)
            if ((eRoulette->hitFlags & gUnknown_083F8C00[i].flag) == 0)
                break;
    }
    else
    {
        for (i = 6; (i < 10); i++)
            if ((eRoulette->hitFlags & gUnknown_083F8C00[i].flag) == 0)
                break;
    }
    gTasks[taskId].data[4] = i;
    ResetBallDataForNewSpin(taskId);
    DrawGridBackground(gTasks[taskId].data[4]);
    SetMultiplierSprite(gTasks[taskId].data[4]);
    FlashSelectionOnWheel(gTasks[taskId].data[4]);
    gTasks[taskId].data[1] = 0;
    gTasks[taskId].func = Task_StartHandleBetGridInput;
}

static u8 CanMoveSelectionInDir(s16 *selectionId, u8 dir)
{
    s8 temp1 = 0;
    s8 temp = 0;
    s8 moveOffsets[4] = {-5, 5, -1, 1};
    s8 originalSelection;
    originalSelection = (u8)*selectionId;
    switch (dir)
    {
    case 0:
    case 1:
        temp1 = (*selectionId % 5);
        temp = temp1 + 15;
        if (temp1 == 0)
            temp1 = 5;
        break;
    case 2:
    case 3:
        temp1 = (s16)(*selectionId / 5) * 5;
        temp = temp1 + 4;
        if (temp1 == 0)
            temp1 = 1;
        break;
    }
    *selectionId += moveOffsets[dir];
    if (*selectionId < temp1)
        *selectionId = temp;
    if (*selectionId > temp)
        *selectionId = temp1;
    if (*selectionId != originalSelection)
        return TRUE;
    return FALSE;
}

void sub_8115B58(u8 taskId)
{
    u8 headerOffset = 0;
    bool8 dirPressed = FALSE;
    if (JOY_NEW(DPAD_UP))
    {
        dirPressed = TRUE;
        if (!CanMoveSelectionInDir(&gTasks[taskId].data[4], 0))
            return;
    }
    if (JOY_NEW(DPAD_DOWN))
    {
        dirPressed = TRUE;
        if (!CanMoveSelectionInDir(&gTasks[taskId].data[4], 1))
            return;
    }
    if (JOY_NEW(DPAD_LEFT))
    {
        dirPressed = TRUE;
        if (!CanMoveSelectionInDir(&gTasks[taskId].data[4], 2))
            return;
    }
    if (JOY_NEW(DPAD_RIGHT))
    {
        dirPressed = TRUE;
        if (!CanMoveSelectionInDir(&gTasks[taskId].data[4], 3))
            return;
    }
    if (dirPressed)
    {
        u8 i;
        DrawGridBackground(gTasks[taskId].data[4]);
        sub_8115928(taskId, FALSE);
        gTasks[taskId].data[1] = headerOffset;
        PlaySE(SE_SELECT);
        RouletteFlash_Stop((&eRoulette->flashUtil), 0xffff);
        eRoulette->flashUtil.palettes[15].available = 0;
        eRoulette->flashUtil.palettes[14].available = 0;
        eRoulette->flashUtil.palettes[13].available = 0;
        FlashSelectionOnWheel(gTasks[taskId].data[4]);
        for (i = 0; i < 4; i++)
        {
            gSprites[eRoulette->spriteIds[i + 41]].oam.tileNum =
            gSprites[eRoulette->spriteIds[i + 41]].sheetTileStart
            + (*gSprites[eRoulette->spriteIds[i + 41]].anims)->type;
        }
        if ((u16)(gTasks[taskId].data[4] - 1) < 4 && !(eRoulette->hitFlags & gUnknown_083F8C00[gTasks[taskId].data[4]].flag) )
        {
            headerOffset = gTasks[taskId].data[4] - 1;
            gSprites[eRoulette->spriteIds[headerOffset + 41]].oam.tileNum =
            gSprites[eRoulette->spriteIds[headerOffset + 41]].sheetTileStart
            + (*gSprites[eRoulette->spriteIds[headerOffset + 41]].anims + 1)->type;
        }
    }
}

void sub_8115D58(u8 taskId)
{
    eRoulette->selectionRectDrawState = 255;
    if (eRoulette->minBet == 1)
        eRoulette->wheelDelay = 1;
    else
        eRoulette->wheelDelay = 0;
    eRoulette->wheelDelayTimer = 0;
    gTasks[taskId].data[1] = 32;
    gTasks[taskId].func = Task_SlideGridOffscreen;
}

static void Task_PlaceBet(u8 taskId)
{
    eRoulette->betSelection[eRoulette->curBallNum] = gTasks[taskId].data[4];
    gTasks[taskId].data[2] = GetMultiplier(eRoulette->betSelection[eRoulette->curBallNum]);
    SetMultiplierSprite(eRoulette->betSelection[eRoulette->curBallNum]);
    if ((gTasks[taskId].data[13] -= eRoulette->minBet) < 0)
        gTasks[taskId].data[13] = 0;
    SetCreditDigits(gTasks[taskId].data[13]);
    gTasks[taskId].func = sub_8115D58;
}

void sub_8115E14(u8 taskId)
{
    sub_8115B58(taskId);
    switch (gTasks[taskId].data[1])
    {
    case 0:
        UpdateGridSelectionRect(gTasks[taskId].data[4]);
        gTasks[taskId].data[1]+= 1;
    break;
    case 30:
        UpdateGridSelectionRect(0);
        gTasks[taskId].data[1]+= 1;
    break;
    case 59:
        gTasks[taskId].data[1] = 0;
    break;
    default:
        gTasks[taskId].data[1]++;
    }
    if (JOY_NEW(A_BUTTON))
    {
        if ((eRoulette->hitFlags & gUnknown_083F8C00[gTasks[taskId].data[4]].flag))
            PlaySE(SE_BOO);
        else
        {
            m4aSongNumStart(SE_SHOP);
            gTasks[taskId].func = Task_PlaceBet;
        }
    }
}

static void Task_SlideGridOffscreen(u8 taskId)
{
    if (gTasks[taskId].data[1]-- > 0)
    {
        if (gTasks[taskId].data[1] > 2)
            gSpriteCoordOffsetX += 2;
        if ((eRoulette->gridX += 4) == 104)
            gSprites[eRoulette->spriteIds[25]].callback = &SpriteCallbackDummy;
    }
    else
    {
        ShowHideGridIcons(1, 255);
        ShowHideGridBalls(1, 255);
        gTasks[taskId].func = Task_InitBallRoll;
        gTasks[taskId].data[1] = 0;
    }
}

static u8 GetRandomForBallTravelDistance(u16 ballNum, u16 rand)
{
    u8 timeh;
    switch (eRoulette->partySpeciesFlags)
    {
    case 1: // SHROOMISH
    case 2: // TAILLOW
        // one of the two is in party
        timeh = gLocalTime.hours - 4;
        if (!(timeh > 5))
        {
            if (!(ballNum < 12 || (rand & 1)))
                return 1;
            else
            {
                const struct RouletteTable *p = gUnknown_083F8DF4;
                return p[eRoulette->tableId].randDistanceLow / 2;
            }
        }
        else if (!(rand & 3))
        {
            const struct RouletteTable *p = gUnknown_083F8DF4;
            return p[eRoulette->tableId].randDistanceLow / 2;
        }
        else
        {
            const struct RouletteTable *p = gUnknown_083F8DF4;
            return p[eRoulette->tableId].randDistanceLow;
        }
        break;
    case 3:
        // both are in party
        timeh = gLocalTime.hours - 4;
        if (!(timeh > 6))
        {
            if (ballNum < 6 || (rand & 1))
            {
                const struct RouletteTable *p = gUnknown_083F8DF4;
                return p[eRoulette->tableId].randDistanceLow / 2;
            }
            else
            {
                return 1;
            }
        }
        else if ((rand & 1) && !(ballNum < 7))
        {
            const struct RouletteTable *p = gUnknown_083F8DF4;
            return p[eRoulette->tableId].randDistanceLow / 4;
        }
        else
        {
            const struct RouletteTable *p = gUnknown_083F8DF4;
            return p[eRoulette->tableId].randDistanceLow / 2;
        }
        break;
    case 0:
    default:
        // neither is in party
        timeh = gLocalTime.hours - 4;
        if (!(timeh > 5))
        {
            if (!(rand & 3))
            {
                return 1;
            }
            else
            {
                const struct RouletteTable *p = gUnknown_083F8DF4;
                return p[eRoulette->tableId].randDistanceLow / 2;
            }
        }
        else if (!(rand & 3))
        {
            if (!(ballNum < 13))
            {
                const struct RouletteTable *p = gUnknown_083F8DF4;
                return p[eRoulette->tableId].randDistanceLow / 2;
            }
            else
            {
                const struct RouletteTable *p = gUnknown_083F8DF4;
                return p[eRoulette->tableId].randDistanceLow;
            }
        }
        else if (rand & (128 << 8))
        {
            if (!(ballNum < 13))
            {
                const struct RouletteTable *p = gUnknown_083F8DF4;
                return p[eRoulette->tableId].randDistanceLow;
            }
            else
            {
                const struct RouletteTable *p = gUnknown_083F8DF4;
                return p[eRoulette->tableId].randDistanceHigh;
            }
        }
        else
        {
            const struct RouletteTable *p = gUnknown_083F8DF4;
            return p[eRoulette->tableId].randDistanceHigh * 2;
        }
    }
}

static void Task_InitBallRoll(u8 taskId)
{
    u8 randTravelMod;
    s8 randTravelDist;
    s8 startAngleId;
    u16 travelDist = 0;
    u16 rand;
    u16 randmod;
    u16 startAngles[] = {0, 180, 90, 270};
    rand = Random();
    randmod = rand % 100;
    eRoulette->curBallSpriteId = gTasks[taskId].data[6];
    eRoulette->stuckHitSlot = travelDist;
    eRoulette->hitSlot = travelDist;
    eRoulette->ballState = travelDist;
    randTravelMod = GetRandomForBallTravelDistance(gTasks[taskId].data[8], rand);
    randTravelDist = (rand % randTravelMod) - (randTravelMod / 2);
    if (gLocalTime.hours < 13)
        startAngleId = 0;
    else
        startAngleId = 1;
    if (randmod < 80)
        startAngleId *= 2;
    else
        startAngleId = (1 - startAngleId) * 2;
    eRoulette->ballTravelDist = travelDist = gUnknown_083F8DF4[eRoulette->tableId].baseTravelDist + randTravelDist;
    //
    travelDist = S16TOPOSFLOAT(travelDist) / 5.0f;
    eRoulette->ballTravelDistFast = travelDist * 3;
    eRoulette->ballTravelDistMed = travelDist;
    eRoulette->ballTravelDistSlow = travelDist;
    //
    eRoulette->ballAngle = S16TOPOSFLOAT(startAngles[(rand & 1) + startAngleId]);
    eRoulette->ballAngleSpeed = S16TOPOSFLOAT(gUnknown_083F8DF4[eRoulette->tableId].ballSpeed);
    eRoulette->ballAngleAccel = ((eRoulette->ballAngleSpeed * 0.5f) - eRoulette->ballAngleSpeed) / S16TOPOSFLOAT(travelDist * 3);
    eRoulette->ballDistToCenter = 68.0f;
    eRoulette->ballFallAccel = 0.0f;
    eRoulette->ballFallSpeed = -(8.0f / S16TOPOSFLOAT(travelDist * 3));
    eRoulette->varA0 = 36.0f;
    gTasks[taskId].func = Task_RollBall;
}

static void Task_RollBall(u8 taskId)
{
    u8 index;
    eRoulette->ballRolling = 1;
    index = eRoulette->spriteIds[eRoulette->curBallSpriteId];
    eRoulette->ball = &gSprites[index];
    eRoulette->ball->callback = SpriteCB_RollBall_Start;
    gTasks[taskId].data[6]++;
    gTasks[taskId].data[8]++;
    SetBallCounterNumLeft(6 - gTasks[taskId].data[6]);
    m4aSongNumStart(SE_ROULETTE_BALL);
    gTasks[taskId].func = Task_RecordBallHit;
}

static void Task_RecordBallHit(u8 taskId)
{
    if (eRoulette->ballState)
    {
        if (eRoulette->ballStuck)
        {
            if (eRoulette->ballUnstuck)
            {
                eRoulette->ballUnstuck = FALSE;
                eRoulette->ballStuck = FALSE;
            }
        }
        else
        {
            if (!gTasks[taskId].data[1])
            {
                u8 won = IsHitInBetSelection(RecordHit(taskId, eRoulette->hitSlot), eRoulette->betSelection[eRoulette->curBallNum]);
                gTasks[taskId].data[5] = won;
                if (won == 1)
                    RouletteFlash_Enable((&eRoulette->flashUtil), 0x1000);
            }
            if (gTasks[taskId].data[1] < 61)
            {
                if (JOY_NEW(A_BUTTON))
                    gTasks[taskId].data[1] = 60;
                gTasks[taskId].data[1]++;
            }
            else
            {
                DrawGridBackground(eRoulette->betSelection[eRoulette->curBallNum]);
                ShowHideGridIcons(0, (u8)gTasks[taskId].data[12]);
                ShowHideGridBalls(0, (u8)gTasks[taskId].data[6] - 1);
                gTasks[taskId].data[1] = 32;
                gTasks[taskId].func = Task_SlideGridOnscreen;
            }
        }
    }
}

static void Task_SlideGridOnscreen(u8 taskId)
{
    if (gTasks[taskId].data[1]-- > 0)
    {
        if (gTasks[taskId].data[1] > 2)
            gSpriteCoordOffsetX -= 2;
        if ((eRoulette->gridX -= 4) == 104)
            gSprites[eRoulette->spriteIds[25]].callback = SpriteCB_GridSquare;
    }
    else
    {
        ShowHideWinSlotCursor(gTasks[taskId].data[12]);
        if (gTasks[taskId].data[5] == 1)
            gTasks[taskId].data[1] = 121;
        else
            gTasks[taskId].data[1] = 61;
        gTasks[taskId].func = Task_FlashBallOnWinningSquare;
    }
}

static void Task_FlashBallOnWinningSquare(u8 taskId)
{
    if (gTasks[taskId].data[1]-- > 1)
    {
        switch (gTasks[taskId].data[1] % 16)
        {
        case 8:
            ShowHideGridIcons(0, 255);
            ShowHideGridBalls(0, 255);
            break;
        case 0:
            ShowHideGridIcons(0, (u8)gTasks[taskId].data[12]);
            ShowHideGridBalls(0, (u8)gTasks[taskId].data[6] - 1);
            break;
        }
    }
    else
    {
        StartTaskAfterDelayOrInput(taskId, sub_8116638, 30, 0);
    }
}

static void Task_TryIncrementWins(u8 taskId)
{
    switch (gTasks[taskId].data[5])
    {
    case 1:
    case 2:
        if (IsFanfareTaskInactive())
        {
            u32 wins = GetGameStat(GAME_STAT_CONSECUTIVE_ROULETTE_WINS);
            if (wins < ++gTasks[taskId].data[11])
                SetGameStat(GAME_STAT_CONSECUTIVE_ROULETTE_WINS, gTasks[taskId].data[11]);
            StartTaskAfterDelayOrInput(taskId, sub_811677C, 0xffff, 3);
        }
        break;
    case 0:
    default:
        if (!IsSEPlaying())
        {
            gTasks[taskId].data[11] = FALSE;
            StartTaskAfterDelayOrInput(taskId, Task_EndTurn, 0xffff, 3);
        }
    }
}

void sub_8116638(u8 taskId)
{
    switch (gTasks[taskId].data[5])
    {
    case 1:
    case 2:
        if (gTasks[taskId].data[2] == 12)
        {
            PlayFanfare(MUS_SLOTS_JACKPOT);
            Menu_DrawStdWindowFrame(0, 14, 29, 19);
            Menu_PrintText(Roulette_Text_Jackpot, 1, 15);
        }
        else
        {
            PlayFanfare(MUS_SLOTS_WIN);
            Menu_DrawStdWindowFrame(0, 14, 29, 19);
            Menu_PrintText(Roulette_Text_ItsAHit, 1, 15);
        }
        break;
    case 0:
    default:
        m4aSongNumStart(SE_FAILURE);
        Menu_DrawStdWindowFrame(0, 14, 29, 19);
        Menu_PrintText(Roulette_Text_NothingDoing, 1, 15);
    }
    gTasks[taskId].data[1] = 0;
    gTasks[taskId].func = Task_TryIncrementWins;
}

static void Task_GivePayout(u8 taskId)
{
    s32 r0 = gTasks[taskId].data[7];
    switch (r0)
    {
    case 0:
        gTasks[taskId].data[13]++;
        m4aSongNumStart(SE_PIN);
        SetCreditDigits(gTasks[taskId].data[13]);
        if (gTasks[taskId].data[13] > 0x270e)
        {
            gTasks[taskId].data[1] = r0;
        }
        else
        {
            gTasks[taskId].data[1]--;
            gTasks[taskId].data[7]++;
        }
        break;
    case 3:
        m4aSongNumStop(SE_PIN);
        gTasks[taskId].data[7] = 0;
        break;
    default:
        gTasks[taskId].data[7]++;
    }
    if (gTasks[taskId].data[1] == 0)
        StartTaskAfterDelayOrInput(taskId, Task_EndTurn, 0xffff, 3);
}

void sub_811677C(u8 taskId)
{
    ConvertIntToDecimalStringN((u8 *)&gStringVar1, (eRoulette->minBet * gTasks[taskId].data[2]), STR_CONV_MODE_LEFT_ALIGN, 2);
    StringExpandPlaceholders((u8 *)&gStringVar4, Roulette_Text_YouveWonXCoins);
    Menu_DrawStdWindowFrame(0, 14, 29, 19);
    Menu_PrintText((u8 *)&gStringVar4, 1, 15);
    gTasks[taskId].data[1] = (eRoulette->minBet * gTasks[taskId].data[2]);
    gTasks[taskId].data[7] = 0;
    gTasks[taskId].func = Task_GivePayout;
}

static void Task_EndTurn(u8 taskId)
{
    RouletteFlash_Stop((&eRoulette->flashUtil), 0xffff);
    eRoulette->flashUtil.palettes[15].available = 0;
    eRoulette->flashUtil.palettes[14].available = 0;
    eRoulette->flashUtil.palettes[13].available = 0;
    gSprites[eRoulette->spriteIds[7 + gUnknown_083F8C00[gTasks[taskId].data[12]].spriteIdOffset]].invisible = TRUE;
    gTasks[taskId].func = sub_8116880;
}

void sub_8116880(u8 taskId)
{
    u8 i = 0;
    gTasks[taskId].data[4] = i;
    eRoulette->betSelection[eRoulette->curBallNum] = 0;
    DrawGridBackground(0);
    gSprites[eRoulette->spriteIds[48]].invisible = TRUE;
    for (i = 0; i < 4; i++)
    {
        gSprites[eRoulette->spriteIds[i + 41]].oam.tileNum =
        gSprites[eRoulette->spriteIds[i + 41]].sheetTileStart
        + (*gSprites[eRoulette->spriteIds[i + 41]].anims)->type;
    }
    if (!(gTasks[taskId].data[13] < eRoulette->minBet))
    {
        if (gTasks[taskId].data[6] == 6)
        {
            Menu_DrawStdWindowFrame(0, 14, 29, 19);
            Menu_PrintText(Roulette_Text_BoardWillBeCleared, 1, 15);
            StartTaskAfterDelayOrInput(taskId, &dp01t_12_3_battle_menu, 0xffff, 3);
        }
        else if (gTasks[taskId].data[13] == 0x270f)
        {
            Menu_DrawStdWindowFrame(0, 14, 29, 19);
            Menu_PrintText(Roulette_Text_CoinCaseIsFull, 1, 15);
            StartTaskAfterDelayOrInput(taskId, sub_8115734, 0xffff, 3);
        }
        else
        {
            gTasks[taskId].func = sub_8115734;
        }
    }
    else
    {
        Menu_DrawStdWindowFrame(0, 14, 29, 19);
        Menu_PrintText(Roulette_Text_NoCoinsLeft, 1, 15);
        StartTaskAfterDelayOrInput(taskId, Task_StopPlaying, 60, 3);
    }
}

void dp01t_12_3_battle_menu(u8 taskId)
{
    u8 i = 0;
    gTasks[taskId].data[6] = 0;
    ResetBallDataForNewSpin(taskId);
    ResetHits();
    HideWheelBalls();
    DrawGridBackground(0);
    SetBallCounterNumLeft(6);
    for (i = 0; i < 12; i++)
        gSprites[eRoulette->spriteIds[i + 7]].invisible = FALSE;
    if (gTasks[taskId].data[13] == 0x270f)
    {
        Menu_DrawStdWindowFrame(0, 14, 29, 19);
        Menu_PrintText(Roulette_Text_CoinCaseIsFull, 1, 15);
        StartTaskAfterDelayOrInput(taskId, sub_8115734, 0xffff, 3);
    }
    else
    {
        gTasks[taskId].func = sub_8115734;
    }
}

void sub_8116AB0(u8 taskId)
{
    RouletteFlash_Stop((&eRoulette->flashUtil), 0xffff);
    RouletteFlash_Reset((&eRoulette->flashUtil));
    gSaveBlock1.coins = gTasks[taskId].data[13];
    if (gSaveBlock1.coins < eRoulette->minBet)
        gSpecialVar_0x8004 = TRUE;
    else
        gSpecialVar_0x8004 = FALSE;
    BeginHardwarePaletteFade(255, 0, 0, 16, 0);
    gTasks[taskId].func = sub_8116B40;
}

void sub_8116B40(u8 taskId) // end roulette ?
{
    if (UpdatePaletteFade() == 0)
    {
        SetVBlankCallback(NULL);
        memset(eRoulette, 0, 0x17c);
        gSpriteCoordOffsetX = gSpriteCoordOffsetY = 0;
        ResetVramOamAndBgCntRegs();
        FreeAllSpritePalettes();
        ResetPaletteFade();
        ResetSpriteData();
        ClearBGTilemapBuffers();
        REG_BLDCNT = 0;
        REG_BLDALPHA = 0;
        REG_BLDY = 0;
        gFieldCallback = FieldCB_ContinueScriptHandleMusic;
        SetMainCallback2(&CB2_ReturnToField);
        DestroyTask(taskId);
#if DEBUG
        unk_203955C[0] = 0;
#endif
    }
}

static void Task_WaitForNextTask(u8 taskId)
{
    if (eRoulette->taskWaitDelay == 0 || JOY_NEW(eRoulette->taskWaitKey))
    {
        gTasks[taskId].func = eRoulette->nextTask;
        if (eRoulette->taskWaitKey > 0)
            PlaySE(SE_SELECT);
        eRoulette->nextTask = NULL;
        eRoulette->taskWaitKey = 0;
        eRoulette->taskWaitDelay = 0;
    }
    if (eRoulette->taskWaitDelay != 0xffff)
        eRoulette->taskWaitDelay--;
}

static void StartTaskAfterDelayOrInput(u8 taskId, TaskFunc task, u16 delay, u16 key)
{
    eRoulette->prevTask = gTasks[taskId].func;
    if (task == NULL)
        task = eRoulette->prevTask;
    eRoulette->nextTask = task;
    eRoulette->taskWaitDelay = delay;
    if (delay == 0xffff && key == 0)
        eRoulette->taskWaitKey = 0xffff;
    else
        eRoulette->taskWaitKey = key;
    gTasks[taskId].func = Task_WaitForNextTask;
}

static void ResetBallDataForNewSpin(u8 taskId)
{
    u8 i = 0;
    eRoulette->unk0 = i;
    eRoulette->useTaillow = 0;
    eRoulette->ballStuck = 0;
    eRoulette->ballUnstuck = 0;
    eRoulette->ballRolling = 0;
    for (i = 0; i < 6; i++)
        eRoulette->betSelection[i] = 0;
    eRoulette->curBallNum = 0;
    gTasks[taskId].data[1] = 0;
}

static void ResetHits(void)
{
    u8 i;
    eRoulette->hitFlags = 0;
    for (i = 0; i < 6; i++)
        eRoulette->hitSquares[i] = 0;
    for (i = 0; i < 4; i++)
        eRoulette->pokeHits[i] = 0;
    for (i = 0; i < 3; i++)
        eRoulette->colorHits[i] = 0;
    ShowHideGridBalls(1, 255);
}

static u8 RecordHit(u8 taskId, u8 slotId)
{
    u32 columnFlags[] = {67650, 135300, 270600, 541200};
    u32 rowFlags[] = {0x3e0, 0x7c00, 0xf8000};
    if (slotId > 11)
    {
        return 0;
    }
    else
    {
        u8 i;
        u8 j;
        eRoulette->hitSquares[gTasks[taskId].data[6] - 1] = gUnknown_083F8D90[slotId].gridSquare;
        gTasks[taskId].data[12] = gUnknown_083F8D90[slotId].gridSquare;
        eRoulette->hitFlags |= gUnknown_083F8D90[slotId].flag;
        for (i = 0; i < 4; i++)
        {
            if (gUnknown_083F8D90[slotId].flag & columnFlags[i])
                eRoulette->pokeHits[i]++;
            if (eRoulette->pokeHits[i] > 2)
                eRoulette->hitFlags |= columnFlags[i];
        }
        for (j = 0; j < 3; j++)
        {
            if (gUnknown_083F8D90[slotId].flag & rowFlags[j])
                eRoulette->colorHits[j]++;
            if (eRoulette->colorHits[j] > 3)
                eRoulette->hitFlags |= rowFlags[j];
        }
        return gUnknown_083F8D90[slotId].gridSquare;
    }
}

static u8 IsHitInBetSelection(u8 gridSquare, u8 betSelection)
{
    u8 hit = gridSquare;
    if (--gridSquare < 19)
    {
        switch (betSelection)
        {
        case 0:
            return 3;
            break;
        case 1 ... 4:
            if (hit == betSelection + 5 || hit == betSelection + 10 || hit == betSelection + 15)
                return 1;
            break;
        case 5:
        case 10:
        case 15:
            if (!(hit < betSelection + 1) && !(hit > betSelection + 4))
                return 1;
            break;
        default:
            if (hit == betSelection)
                return 1;
        }
    }
    return 0;
}

static void FlashSelectionOnWheel(u8 selectionId)
{

    u16 flashFlags = 0;
    u8 numSelected;
    u16 palOffset;
    u8 i;
    switch (selectionId)
    {
    case 5:
    case 10:
    case 15:
        for (i = (selectionId + 1); i < (selectionId + 5); i++)
            if (!(eRoulette->hitFlags & gUnknown_083F8C00[i].flag))
                flashFlags |= gUnknown_083F8C00[i].flashFlags;
        RouletteFlash_Enable(&eRoulette->flashUtil, flashFlags &= 0xdfff);
        break;
    default:
    {
        struct RouletteFlashSettings iconFlash[3];
        memcpy(iconFlash, sFlashData_PokeIcons, sizeof iconFlash);
        if (selectionId > 0 && selectionId < 5)
            numSelected = 3;
        else
            numSelected = 1;
        palOffset = selectionId / 5 - 1;
        switch (selectionId % 5)
        {
            case 1:
                palOffset = gSprites[eRoulette->spriteIds[7 + 0]].oam.paletteNum * 16;
                break;
            case 2:
                palOffset = gSprites[eRoulette->spriteIds[7 + 1]].oam.paletteNum * 16;
                break;
            case 3:
                palOffset = gSprites[eRoulette->spriteIds[7 + 2]].oam.paletteNum * 16;
                break;
            case 4:
                palOffset = gSprites[eRoulette->spriteIds[7 + 3]].oam.paletteNum * 16;
                break;
        }
        if (numSelected == 1)
        {
            if (!(eRoulette->hitFlags & gUnknown_083F8C00[selectionId].flag))
            {
                iconFlash[selectionId / 5 - 1].paletteOffset += palOffset;
                RouletteFlash_Add(&eRoulette->flashUtil, 13, &iconFlash[selectionId / 5 - 1]);
            }
            else
                return;
        }
        else
        {
            for (i = 0; i < 3; i++)
            {
                u8 columnSlotId = i * 5 + selectionId + 5;
                if (!(eRoulette->hitFlags & gUnknown_083F8C00[columnSlotId].flag))
                {
                    iconFlash[columnSlotId / 5 - 1].paletteOffset += palOffset;
                    RouletteFlash_Add(&eRoulette->flashUtil, i + 13, &iconFlash[columnSlotId / 5 - 1]);
                    if (numSelected == 3)
                        flashFlags = gUnknown_083F8C00[columnSlotId].flashFlags;
                    numSelected--;
                }
            }
            if (numSelected != 2)
                flashFlags = 0;
        }
        RouletteFlash_Enable(&eRoulette->flashUtil, flashFlags |= gUnknown_083F8C00[selectionId].flashFlags);
        break;
    }
    }
}

static void DrawGridBackground(u8 selectionId)
{
    vu8 i;
    vu8 j;
    vu16 x;
    vu16 y;
    vu8 tilemapOffset;
    u8 selectionIds[5];
    u8 numSquares;
    eRoulette->updateGridHighlight = 1;
    ShowHideGridIcons(0, 0);
    SetTilemapRect(gBGTilemapBuffers[1], (u16 *)ewram18800, 14, 7, 16, 13);
    switch (selectionId)
    {
    case 0:
        return;
    case 1 ... 4:
        numSquares = 4;
        for (i = 0; i < numSquares; i++)
            selectionIds[i] = i * 5 + selectionId;
        break;
    case 5:
    case 10:
    case 15:
        numSquares = 5;
        for (i = 0; i < numSquares; i++)
            selectionIds[i] = i + selectionId;
        break;
    default:
        numSquares = 1;
        selectionIds[0] = selectionId;
    }
    for (i = 0; i < numSquares; i++)
    {
        tilemapOffset = gUnknown_083F8C00[selectionIds[i]].tilemapOffset;
        x = gUnknown_083F8C00[selectionIds[i]].x;
        for (j = 0; j < 3; j++)
        {
            y = (gUnknown_083F8C00[selectionIds[i]].y + j) * 32;
            gBGTilemapBuffers[1][x + y + 0] = ewram18800[0xD0 + (tilemapOffset + j) * 3 + 0];
            gBGTilemapBuffers[1][x + y + 1] = ewram18800[0xD0 + (tilemapOffset + j) * 3 + 1];
            gBGTilemapBuffers[1][x + y + 2] = ewram18800[0xD0 + (tilemapOffset + j) * 3 + 2];
        }
    }
}

static u8 GetMultiplier(u8 selectionId)
{
    u8 multipliers[] = {0, 3, 4, 6, 12};
    if (selectionId > 19)
        selectionId = 0;
    switch (gUnknown_083F8C00[selectionId].baseMultiplier)
    {
    case 3:
        selectionId = selectionId / 5 - 1;
        if (eRoulette->colorHits[selectionId] > 3)
            return 0;
        return multipliers[eRoulette->colorHits[selectionId] + 1];
    case 4:
        selectionId--;
        if (eRoulette->pokeHits[selectionId] > 2)
            return 0;
        return multipliers[eRoulette->pokeHits[selectionId] + 2];
    case 12:
        if (eRoulette->hitFlags & gUnknown_083F8C00[selectionId].flag)
            return 0;
        return multipliers[4];
    }
    return 0;
}

static void UpdateWheelPosition(void)
{
    s32 bg2x;
    s32 bg2y;
    REG_BG2PA = eRoulette->wheelRotation.a;
    REG_BG2PB = eRoulette->wheelRotation.b;
    REG_BG2PC = eRoulette->wheelRotation.c;
    REG_BG2PD = eRoulette->wheelRotation.d;
    bg2x = 0x7400 - eRoulette->wheelRotation.a * (gSpriteCoordOffsetX + 116)
                - eRoulette->wheelRotation.b * (gSpriteCoordOffsetY + 80);
    bg2y = 0x5400 - eRoulette->wheelRotation.c * (gSpriteCoordOffsetX + 116)
                - eRoulette->wheelRotation.d * (gSpriteCoordOffsetY + 80);
    REG_BG2X_L = bg2x;
    REG_BG2X_H = (bg2x & 0xfff0000) >> 16;
    REG_BG2Y_L = bg2y;
    REG_BG2Y_H = (bg2y & 0xfff0000) >> 16;
}

s16 MathUtil_Mul16(s16 x, s16 y)
{
    s32 result = x * y;
    result /= 0x100;
    return result;
}

s16 MathUtil_Inv16(s16 y)
{
    s32 x = 0x10000;
    return x / y;
}

void sub_81174F8(u8 taskId)
{
    DisplayYesNoMenu(20, 8, 1);
    sub_814AAF8(RGB(30, 12, 11));
    DoYesNoFuncWithChoice(taskId, &sYesNoTable_AcceptMinBet);
}

static void Task_FadeToRouletteGame(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetVBlankCallback(NULL);
        SetMainCallback2(sub_8115384);
        DestroyTask(taskId);
    }
}

void sub_811755C(u8 taskId)
{
    Menu_EraseWindowRect(20, 8, 26, 13);
    Menu_EraseScreen();
    BeginNormalPaletteFade(0xffffffff, 0, 0, 16, RGB(0, 0, 0));
    gPaletteFade.delayCounter = gPaletteFade.multipurpose2;
    UpdatePaletteFade();
    gTasks[taskId].func = Task_FadeToRouletteGame;
}

void sub_81175C0(u8 taskId)
{
#if DEBUG
    unk_203955C[0] = 0;
#endif
    Menu_EraseScreen();
    UnlockPlayerFieldControls();
    DestroyTask(taskId);
}

void sub_81175DC(u8 taskId)
{
    gTasks[taskId].data[0]++;
    if (!JOY_NEW(A_BUTTON | B_BUTTON) && gTasks[taskId].data[0] < 61)
        return;
    gSpecialVar_0x8004 = 1;
    Menu_EraseScreen();
    UnlockPlayerFieldControls();
    DestroyTask(taskId);
}

void sub_8117630(u8 taskId)
{
    u32 minBet = sTableMinBets[(gSpecialVar_0x8004 & 1) + (gSpecialVar_0x8004 >> 7 << 1)];
    ConvertIntToDecimalStringN(gStringVar1, minBet, 2, 1);
    StringExpandPlaceholders(gStringVar4, Roulette_Text_PlayMinimumWagerIsX);
    Menu_DrawStdWindowFrame(0, 14, 29, 19);
    Menu_PrintText(gStringVar4, 1, 15);
    gTasks[taskId].func = sub_81174F8;
}

void Task_Roulette_0(u8 taskId)
{
    s32 minBet;
    ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].data[13], 1, 4);
    StringExpandPlaceholders(gStringVar4, gOtherText_Coins);
    Menu_DrawStdWindowFrame(0, 0, 9, 3);
    MenuPrint_RightAligned(gStringVar4, 9, 1);
    minBet = sTableMinBets[(gSpecialVar_0x8004 & 1) + (gSpecialVar_0x8004 >> 7 << 1)];
    ConvertIntToDecimalStringN(gStringVar1, minBet, 2, 1);
    if (gTasks[taskId].data[13] >= minBet)
    {
        if ((gSpecialVar_0x8004 & 0x80) && (gSpecialVar_0x8004 & 1))
        {
            Menu_DrawStdWindowFrame(0, 14, 29, 19);
            Menu_PrintText(Roulette_Text_SpecialRateTable, 1, 15);
            StartTaskAfterDelayOrInput(taskId , sub_8117630, 0xffff, 3);
        }
        else
        {
            StringExpandPlaceholders(gStringVar4, Roulette_Text_PlayMinimumWagerIsX);
            Menu_DrawStdWindowFrame(0, 14, 29, 19);
            Menu_PrintText(gStringVar4, 1, 15);
            gTasks[taskId].func = sub_81174F8;
        }
    }
    else
    {
        StringExpandPlaceholders(gStringVar4, Roulette_Text_NotEnoughCoins);
        Menu_DrawStdWindowFrame(0, 14, 29, 19);
        Menu_PrintText(gStringVar4, 1, 15);
        gTasks[taskId].func = sub_81175DC;
        gTasks[taskId].data[13] = 0;
        gTasks[taskId].data[0] = 0;
    }
}

#if DEBUG

#if (ENGLISH && REVISION == 0)
    static const u8 gUnknown_Debug_0842510D[] = _("コインの かず STARTで きめて\nAーp1　Bーm1　R:Lー×10");
#else
    static const u8 gUnknown_Debug_0842510D[] = _("Set　COIN　and　Press　START\nAーp1　Bーm1　R:Lー×10");
#endif

void debug_sub_812CDE4(u8 taskId)
{
#if (ENGLISH && REVISION == 0)
    u8 coinText[] = {0xFD, 0x02, 0xFF};
#endif

    if (JOY_NEW(A_BUTTON))
    {
        gTasks[taskId].data[13]++;
        if (gTasks[taskId].data[13] == 10000)
            gTasks[taskId].data[13] = 0;
        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].data[13], 1, 4);
#if (ENGLISH && REVISION == 0)
        StringExpandPlaceholders(gStringVar4, coinText);
        Menu_PrintText(gStringVar4, 2, 1);
#else
        StringExpandPlaceholders(gStringVar4, gOtherText_Coins);
        MenuPrint_RightAligned(gStringVar4, 9, 1);
#endif
    }
    else if (JOY_NEW(B_BUTTON))
    {
        gTasks[taskId].data[13]--;
        if (gTasks[taskId].data[13] == -1)
            gTasks[taskId].data[13] = 9999;
        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].data[13], 1, 4);
#if (ENGLISH && REVISION == 0)
        StringExpandPlaceholders(gStringVar4, coinText);
        Menu_PrintText(gStringVar4, 2, 1);
#else
        StringExpandPlaceholders(gStringVar4, gOtherText_Coins);
        MenuPrint_RightAligned(gStringVar4, 9, 1);
#endif
    }
    else if (JOY_NEW(R_BUTTON))
    {
        gTasks[taskId].data[13] += 10;
        if (gTasks[taskId].data[13] > 9999)
            gTasks[taskId].data[13] -= 9999;
        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].data[13], 1, 4);
#if (ENGLISH && REVISION == 0)
        StringExpandPlaceholders(gStringVar4, coinText);
        Menu_PrintText(gStringVar4, 2, 1);
#else
        StringExpandPlaceholders(gStringVar4, gOtherText_Coins);
        MenuPrint_RightAligned(gStringVar4, 9, 1);
#endif
    }
    else if (JOY_NEW(L_BUTTON))
    {
        gTasks[taskId].data[13] -= 10;
        if (gTasks[taskId].data[13] < 0)
            gTasks[taskId].data[13] += 9999;
        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].data[13], 1, 4);
#if (ENGLISH && REVISION == 0)
        StringExpandPlaceholders(gStringVar4, coinText);
        Menu_PrintText(gStringVar4, 2, 1);
#else
        StringExpandPlaceholders(gStringVar4, gOtherText_Coins);
        MenuPrint_RightAligned(gStringVar4, 9, 1);
#endif
    }
    else if (JOY_NEW(START_BUTTON))
    {
        gSaveBlock1.coins = gTasks[taskId].data[13];
        gTasks[taskId].func = Task_Roulette_0;
        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].data[13], 1, 4);
#if (ENGLISH && REVISION == 0)
        StringExpandPlaceholders(gStringVar4, coinText);
        Menu_PrintText(gStringVar4, 2, 1);
#else
        StringExpandPlaceholders(gStringVar4, gOtherText_Coins);
        MenuPrint_RightAligned(gStringVar4, 9, 1);
#endif
        unk_2039560 = 0;
    }
    else if (JOY_NEW(SELECT_BUTTON))
    {
        gSaveBlock1.coins = gTasks[taskId].data[13];
        gTasks[taskId].func = Task_Roulette_0;
        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].data[13], 1, 4);
#if (ENGLISH && REVISION == 0)
        StringExpandPlaceholders(gStringVar4, coinText);
        Menu_PrintText(gStringVar4, 2, 1);
#else
        StringExpandPlaceholders(gStringVar4, gOtherText_Coins);
        MenuPrint_RightAligned(gStringVar4, 9, 1);
#endif
        unk_2039560 = 1;
    }
}

void debug_sub_812CFE8(u8 taskId)
{
    gTasks[taskId].data[13] = gSaveBlock1.coins;
    if (Random() & 1)
        gSpecialVar_0x8004 |= 128;
    ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].data[13], 1, 4);
    StringExpandPlaceholders(gStringVar4, gOtherText_Coins);
    Menu_DrawStdWindowFrame(0, 0, 9, 3);
#if (ENGLISH && REVISION == 0)
    Menu_PrintText(gStringVar4, 2, 1);
#else
    MenuPrint_RightAligned(gStringVar4, 9, 1);
#endif
    Menu_DrawStdWindowFrame(0, 14, 29, 19);
    Menu_PrintText(gUnknown_Debug_0842510D, 1, 15);
    gTasks[taskId].func = debug_sub_812CDE4;
}

#endif

void PlayRoulette(void)
{
    u8 taskid;

    LockPlayerFieldControls();
#if DEBUG
    unk_2039560 = 0;
    if (unk_203955C[0] != 0)
    {
	    CreateTask(debug_sub_812CFE8, 0);
	    return;
    }
#endif
    taskid = CreateTask(Task_Roulette_0, 0);
    gTasks[taskid].data[13] = gSaveBlock1.coins;
}
