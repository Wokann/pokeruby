#include "global.h"
#include "math_util.h"
#include "random.h"
#include "graphics.h"
#include "ewram.h"
#include "field_fadetransition.h"
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
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "trig.h"
#include "scanline_effect.h"

asm(".include \"constants/gba_constants.inc\"");

#define HAS_SHROOMISH  (1 << 0)
#define HAS_TAILLOW    (1 << 1)
#define NO_DELAY       0xFFFF
#define ROULETTE_SPECIAL_RATE (1 << 7)
#define GET_MIN_BET_ID(value) (((value) & 1) + (((value) >> 7) * 2))

enum {
    SELECT_STATE_WAIT,
    SELECT_STATE_DRAW,
    SELECT_STATE_UPDATE,
    SELECT_STATE_ERASE = 0xFF,
};

#if DEBUG
void *gRouletteDebugState;
#endif

void CB2_Roulette(void);
void VBlankCB_Roulette(void);
void InitRouletteTableData(void);
void CB2_LoadRoulette(void);
void Task_SpinWheel(u8);
void Task_StartPlaying(u8);
void Task_AskKeepPlaying(u8);
void Task_ContinuePlaying(u8);
void Task_StopPlaying(u8);
void UpdateGridSelectionRect(u8);
void UpdateGridSelection(u8, u8);
void Task_StartHandleBetGridInput(u8);
void Task_SelectFirstEmptySquare(u8);
u8 CanMoveSelectionInDir(s16 *, u8);
void ProcessBetGridInput(u8);
void Task_PlaceBet(u8);
void Task_HandleBetGridInput(u8);
void Task_SlideGridOffscreen(u8);
u8 GetRandomForBallTravelDistance(u16, u16);
void Task_InitBallRoll(u8);
void Task_RollBall(u8);
void Task_RecordBallHit(u8);
void Task_SlideGridOnscreen(u8);
void Task_FlashBallOnWinningSquare(u8);
void Task_TryIncrementWins(u8);
void Task_PrintSpinResult(u8);
void Task_GivePayout(u8);
void Task_PrintPayout(u8);
void Task_EndTurn(u8);
void Task_TryPrintEndTurnMsg(u8);
void Task_ClearBoard(u8);
void ExitRoulette(u8);
void Task_ExitRoulette(u8);
void Task_WaitForNextTask(u8);
void StartTaskAfterDelayOrInput(u8, TaskFunc, u16, u16);
void ResetBallDataForNewSpin(u8);
void ResetHits(void);
u8 RecordHit(u8, u8);
bool8 IsHitInBetSelection(u8, u8);
void FlashSelectionOnWheel(u8);
void DrawGridBackground(u8);
u8 GetMultiplier(u8);
void UpdateWheelPosition(void);
void Task_ShowMinBetYesNo(u8);
void Task_FadeToRouletteGame(u8);
void Task_AcceptMinBet(u8);
void Task_DeclineMinBet(u8);
void Task_NotEnoughForMinBet(u8);
void Task_PrintMinBet(u8);
void Task_PrintRouletteEntryMsg(u8);

#define tMultiplier      data[2]
#define tSelectionId     data[4]
#define tWonBet          data[5]
#define tBallNum         data[6]
#define tTotalBallNum    data[8]
#define tConsecutiveWins data[11]
#define tWinningSquare   data[12]
#define tCoins           data[13]

const u16 sWheel_Pal[] = INCBIN_U16("graphics/roulette/wheel.gbapal");
const u8 sGrid_Tilemap[] = INCBIN_U8("graphics/roulette/grid.bin.lz");
const u8 sWheel_Tilemap[] = INCBIN_U8("graphics/roulette/wheel.bin.lz");

const struct GridSelection sGridSelections[NUM_GRID_SELECTIONS + 1] =
{
    [SELECTION_NONE] = {
        .spriteIdOffset = 0xFF,
        .baseMultiplier = 0,
        .column = 0,
        .row = 0,
        .x = 7,
        .y = 7,
        .var05 = 0,
        .tilemapOffset = 0,
        .flag = 0,
        .inSelectionFlags = 0,
        .flashFlags = 0,
    },
    [COL_WYNAUT] = {
        .spriteIdOffset = 12,
        .baseMultiplier = NUM_BOARD_POKES,
        .column = 1,
        .row = 0,
        .x = 17,
        .y = 7,
        .var05 = 0,
        .tilemapOffset = 0,
        .flag = F_WYNAUT_COL,
        .inSelectionFlags = F_WYNAUT_COL | F_ORANGE_WYNAUT | F_GREEN_WYNAUT | F_PURPLE_WYNAUT,
        .flashFlags = F_FLASH_COLUMN,
    },
    [COL_AZURILL] = {
        .spriteIdOffset = 13,
        .baseMultiplier = NUM_BOARD_POKES,
        .column = 2,
        .row = 0,
        .x = 20,
        .y = 7,
        .var05 = 0,
        .tilemapOffset = 0,
        .flag = F_AZURILL_COL,
        .inSelectionFlags = F_AZURILL_COL | F_ORANGE_AZURILL | F_GREEN_AZURILL | F_PURPLE_AZURILL,
        .flashFlags = F_FLASH_COLUMN,
    },
    [COL_SKITTY] = {
        .spriteIdOffset = 14,
        .baseMultiplier = NUM_BOARD_POKES,
        .column = 3,
        .row = 0,
        .x = 23,
        .y = 7,
        .var05 = 0,
        .tilemapOffset = 0,
        .flag = F_SKITTY_COL,
        .inSelectionFlags = F_SKITTY_COL | F_ORANGE_SKITTY | F_GREEN_SKITTY | F_PURPLE_SKITTY,
        .flashFlags = F_FLASH_COLUMN,
    },
    [COL_MAKUHITA] = {
        .spriteIdOffset = 15,
        .baseMultiplier = NUM_BOARD_POKES,
        .column = 4,
        .row = 0,
        .x = 26,
        .y = 7,
        .var05 = 0,
        .tilemapOffset = 0,
        .flag = F_MAKUHITA_COL,
        .inSelectionFlags = F_MAKUHITA_COL | F_ORANGE_MAKUHITA | F_GREEN_MAKUHITA | F_PURPLE_MAKUHITA,
        .flashFlags = F_FLASH_COLUMN,
    },
    [ROW_ORANGE] = {
        .spriteIdOffset = 16,
        .baseMultiplier = NUM_BOARD_COLORS,
        .column = 0,
        .row = 1,
        .x = 14,
        .y = 10,
        .var05 = 0,
        .tilemapOffset = 12,
        .flag = F_ORANGE_ROW,
        .inSelectionFlags = F_ORANGE_ROW | F_ORANGE_WYNAUT | F_ORANGE_AZURILL | F_ORANGE_SKITTY | F_ORANGE_MAKUHITA,
        .flashFlags = F_FLASH_COLOR_O_WYNAUT | F_FLASH_COLOR_O_AZURILL | F_FLASH_COLOR_O_SKITTY | F_FLASH_COLOR_O_MAKUHITA,
    },
    [SQU_ORANGE_WYNAUT] = {
        .spriteIdOffset = 0,
        .baseMultiplier = NUM_ROULETTE_SLOTS,
        .column = 1,
        .row = 1,
        .x = 17,
        .y = 10,
        .var05 = 3,
        .tilemapOffset = 3,
        .flag = F_ORANGE_WYNAUT,
        .inSelectionFlags = F_ORANGE_WYNAUT,
        .flashFlags = F_FLASH_ICON | F_FLASH_COLOR_O_WYNAUT,
    },
    [SQU_ORANGE_AZURILL] = {
        .spriteIdOffset = 9,
        .baseMultiplier = NUM_ROULETTE_SLOTS,
        .column = 2,
        .row = 1,
        .x = 20,
        .y = 10,
        .var05 = 3,
        .tilemapOffset = 3,
        .flag = F_ORANGE_AZURILL,
        .inSelectionFlags = F_ORANGE_AZURILL,
        .flashFlags = F_FLASH_ICON | F_FLASH_COLOR_O_AZURILL,
    },
    [SQU_ORANGE_SKITTY] = {
        .spriteIdOffset = 6,
        .baseMultiplier = NUM_ROULETTE_SLOTS,
        .column = 3,
        .row = 1,
        .x = 23,
        .y = 10,
        .var05 = 3,
        .tilemapOffset = 3,
        .flag = F_ORANGE_SKITTY,
        .inSelectionFlags = F_ORANGE_SKITTY,
        .flashFlags = F_FLASH_ICON | F_FLASH_COLOR_O_SKITTY,
    },
    [SQU_ORANGE_MAKUHITA] = {
        .spriteIdOffset = 3,
        .baseMultiplier = NUM_ROULETTE_SLOTS,
        .column = 4,
        .row = 1,
        .x = 26,
        .y = 10,
        .var05 = 3,
        .tilemapOffset = 3,
        .flag = F_ORANGE_MAKUHITA,
        .inSelectionFlags = F_ORANGE_MAKUHITA,
        .flashFlags = F_FLASH_ICON | F_FLASH_COLOR_O_MAKUHITA,
    },
    [ROW_GREEN] = {
        .spriteIdOffset = 17,
        .baseMultiplier = NUM_BOARD_COLORS,
        .column = 0,
        .row = 2,
        .x = 14,
        .y = 13,
        .var05 = 3,
        .tilemapOffset = 15,
        .flag = F_GREEN_ROW,
        .inSelectionFlags = F_GREEN_ROW | F_GREEN_WYNAUT | F_GREEN_AZURILL | F_GREEN_SKITTY | F_GREEN_MAKUHITA,
        .flashFlags = F_FLASH_COLOR_G_WYNAUT | F_FLASH_COLOR_G_AZURILL | F_FLASH_COLOR_G_SKITTY | F_FLASH_COLOR_G_MAKUHITA,
    },
    [SQU_GREEN_WYNAUT] = {
        .spriteIdOffset = 4,
        .baseMultiplier = NUM_ROULETTE_SLOTS,
        .column = 1,
        .row = 2,
        .x = 17,
        .y = 13,
        .var05 = 6,
        .tilemapOffset = 6,
        .flag = F_GREEN_WYNAUT,
        .inSelectionFlags = F_GREEN_WYNAUT,
        .flashFlags = F_FLASH_ICON | F_FLASH_COLOR_G_WYNAUT,
    },
    [SQU_GREEN_AZURILL] = {
        .spriteIdOffset = 1,
        .baseMultiplier = NUM_ROULETTE_SLOTS,
        .column = 2,
        .row = 2,
        .x = 20,
        .y = 13,
        .var05 = 6,
        .tilemapOffset = 6,
        .flag = F_GREEN_AZURILL,
        .inSelectionFlags = F_GREEN_AZURILL,
        .flashFlags = F_FLASH_ICON | F_FLASH_COLOR_G_AZURILL,
    },
    [SQU_GREEN_SKITTY] = {
        .spriteIdOffset = 10,
        .baseMultiplier = NUM_ROULETTE_SLOTS,
        .column = 3,
        .row = 2,
        .x = 23,
        .y = 13,
        .var05 = 6,
        .tilemapOffset = 6,
        .flag = F_GREEN_SKITTY,
        .inSelectionFlags = F_GREEN_SKITTY,
        .flashFlags = F_FLASH_ICON | F_FLASH_COLOR_G_SKITTY,
    },
    [SQU_GREEN_MAKUHITA] = {
        .spriteIdOffset = 7,
        .baseMultiplier = NUM_ROULETTE_SLOTS,
        .column = 4,
        .row = 2,
        .x = 26,
        .y = 13,
        .var05 = 6,
        .tilemapOffset = 6,
        .flag = F_GREEN_MAKUHITA,
        .inSelectionFlags = F_GREEN_MAKUHITA,
        .flashFlags = F_FLASH_ICON | F_FLASH_COLOR_G_MAKUHITA,
    },
    [ROW_PURPLE] = {
        .spriteIdOffset = 18,
        .baseMultiplier = NUM_BOARD_COLORS,
        .column = 0,
        .row = 3,
        .x = 14,
        .y = 16,
        .var05 = 6,
        .tilemapOffset = 18,
        .flag = F_PURPLE_ROW,
        .inSelectionFlags = F_PURPLE_ROW | F_PURPLE_WYNAUT | F_PURPLE_AZURILL | F_PURPLE_SKITTY | F_PURPLE_MAKUHITA,
        .flashFlags = F_FLASH_COLOR_P_WYNAUT | F_FLASH_COLOR_P_AZURILL | F_FLASH_COLOR_P_SKITTY | F_FLASH_COLOR_P_MAKUHITA,
    },
    [SQU_PURPLE_WYNAUT] = {
        .spriteIdOffset = 8,
        .baseMultiplier = NUM_ROULETTE_SLOTS,
        .column = 1,
        .row = 3,
        .x = 17,
        .y = 16,
        .var05 = 9,
        .tilemapOffset = 9,
        .flag = F_PURPLE_WYNAUT,
        .inSelectionFlags = F_PURPLE_WYNAUT,
        .flashFlags = F_FLASH_ICON | F_FLASH_COLOR_P_WYNAUT,
    },
    [SQU_PURPLE_AZURILL] = {
        .spriteIdOffset = 5,
        .baseMultiplier = NUM_ROULETTE_SLOTS,
        .column = 2,
        .row = 3,
        .x = 20,
        .y = 16,
        .var05 = 9,
        .tilemapOffset = 9,
        .flag = F_PURPLE_AZURILL,
        .inSelectionFlags = F_PURPLE_AZURILL,
        .flashFlags = F_FLASH_ICON | F_FLASH_COLOR_P_AZURILL,
    },
    [SQU_PURPLE_SKITTY] = {
        .spriteIdOffset = 2,
        .baseMultiplier = NUM_ROULETTE_SLOTS,
        .column = 3,
        .row = 3,
        .x = 23,
        .y = 16,
        .var05 = 9,
        .tilemapOffset = 9,
        .flag = F_PURPLE_SKITTY,
        .inSelectionFlags = F_PURPLE_SKITTY,
        .flashFlags = F_FLASH_ICON | F_FLASH_COLOR_P_SKITTY,
    },
    [SQU_PURPLE_MAKUHITA] = {
        .spriteIdOffset = 11,
        .baseMultiplier = NUM_ROULETTE_SLOTS,
        .column = 4,
        .row = 3,
        .x = 26,
        .y = 16,
        .var05 = 9,
        .tilemapOffset = 9,
        .flag = F_PURPLE_MAKUHITA,
        .inSelectionFlags = F_PURPLE_MAKUHITA,
        .flashFlags = F_FLASH_ICON | F_FLASH_COLOR_P_MAKUHITA,
    },
};

const struct RouletteSlot sRouletteSlots[] =
{
    {
        .id1 = 0,
        .id2 = 1,
        .gridSquare = SQU_ORANGE_WYNAUT,
        .flag = F_ORANGE_WYNAUT,
    },
    {
        .id1 = 1,
        .id2 = 3,
        .gridSquare = SQU_GREEN_AZURILL,
        .flag = F_GREEN_AZURILL,
    },
    {
        .id1 = 2,
        .id2 = 5,
        .gridSquare = SQU_PURPLE_SKITTY,
        .flag = F_PURPLE_SKITTY,
    },
    {
        .id1 = 3,
        .id2 = 7,
        .gridSquare = SQU_ORANGE_MAKUHITA,
        .flag = F_ORANGE_MAKUHITA,
    },
    {
        .id1 = 4,
        .id2 = 9,
        .gridSquare = SQU_GREEN_WYNAUT,
        .flag = F_GREEN_WYNAUT,
    },
    {
        .id1 = 5,
        .id2 = 11,
        .gridSquare = SQU_PURPLE_AZURILL,
        .flag = F_PURPLE_AZURILL,
    },
    {
        .id1 = 6,
        .id2 = 13,
        .gridSquare = SQU_ORANGE_SKITTY,
        .flag = F_ORANGE_SKITTY,
    },
    {
        .id1 = 7,
        .id2 = 15,
        .gridSquare = SQU_GREEN_MAKUHITA,
        .flag = F_GREEN_MAKUHITA,
    },
    {
        .id1 = 8,
        .id2 = 17,
        .gridSquare = SQU_PURPLE_WYNAUT,
        .flag = F_PURPLE_WYNAUT,
    },
    {
        .id1 = 9,
        .id2 = 19,
        .gridSquare = SQU_ORANGE_AZURILL,
        .flag = F_ORANGE_AZURILL,
    },
    {
        .id1 = 10,
        .id2 = 21,
        .gridSquare = SQU_GREEN_SKITTY,
        .flag = F_GREEN_SKITTY,
    },
    {
        .id1 = 11,
        .id2 = 23,
        .gridSquare = SQU_PURPLE_MAKUHITA,
        .flag = F_PURPLE_MAKUHITA,
    },
};

static const u8 sTableMinBets[] = {1, 3, 1, 6};

const struct RouletteTable sRouletteTables[] = {
    {
        .minBet = 1,
        .randDistanceHigh = 60,
        .randDistanceLow = 30,
        .wheelSpeed = 1,
        .wheelDelay = 1,
        .shroomish = {
            .startAngle = 45,
            .dropAngle = 30,
            .fallSlowdown = 1,
        },
        .taillow = {
            .baseDropDelay = 75,
            .rightStartAngle = 27,
            .leftStartAngle = 24,
        },
        .ballSpeed = 10,
        .baseTravelDist = 360,
        .var1C = -0.5f
    }, {
        .minBet = 3,
        .randDistanceHigh = 30,
        .randDistanceLow = 15,
        .wheelSpeed = 1,
        .wheelDelay = 0,
        .shroomish = {
            .startAngle = 75,
            .dropAngle = 60,
            .fallSlowdown = 2,
        },
        .taillow = {
            .baseDropDelay = 0,
            .rightStartAngle = 54,
            .leftStartAngle = 48,
        },
        .ballSpeed = 10,
        .baseTravelDist = 270,
        .var1C = -1.0f
    }
};

const struct RouletteFlashSettings sFlashData_Colors[] = {
	{
        .color = FLASHUTIL_USE_EXISTING_COLOR,
        .paletteOffset = 5,
        .numColors = 1,
        .delay = 1,
        .unk6 = 255,
        .numFadeCycles = 8,
        .unk7_5 = 2,
        .colorDeltaDir = FALSE
    }, {
        .color = FLASHUTIL_USE_EXISTING_COLOR,
        .paletteOffset = 10,
        .numColors = 1,
        .delay = 1,
        .unk6 = 255,
        .numFadeCycles = 8,
        .unk7_5 = 2,
        .colorDeltaDir = FALSE
    }, {
        .color = FLASHUTIL_USE_EXISTING_COLOR,
        .paletteOffset = 21,
        .numColors = 1,
        .delay = 1,
        .unk6 = 255,
        .numFadeCycles = 8,
        .unk7_5 = 2,
        .colorDeltaDir = FALSE
    }, {
        .color = FLASHUTIL_USE_EXISTING_COLOR,
        .paletteOffset = 85,
        .numColors = 1,
        .delay = 1,
        .unk6 = 255,
        .numFadeCycles = 8,
        .unk7_5 = 2,
        .colorDeltaDir = FALSE
    }, {
        .color = FLASHUTIL_USE_EXISTING_COLOR,
        .paletteOffset = 90,
        .numColors = 1,
        .delay = 1,
        .unk6 = 255,
        .numFadeCycles = 8,
        .unk7_5 = 2,
        .colorDeltaDir = FALSE
    }, {
        .color = FLASHUTIL_USE_EXISTING_COLOR,
        .paletteOffset = 101,
        .numColors = 1,
        .delay = 1,
        .unk6 = 255,
        .numFadeCycles = 8,
        .unk7_5 = 2,
        .colorDeltaDir = FALSE
    }, {
        .color = FLASHUTIL_USE_EXISTING_COLOR,
        .paletteOffset = 117,
        .numColors = 1,
        .delay = 1,
        .unk6 = 255,
        .numFadeCycles = 8,
        .unk7_5 = 2,
        .colorDeltaDir = FALSE
    }, {
        .color = FLASHUTIL_USE_EXISTING_COLOR,
        .paletteOffset = 122,
        .numColors = 1,
        .delay = 1,
        .unk6 = 255,
        .numFadeCycles = 8,
        .unk7_5 = 2,
        .colorDeltaDir = FALSE
    }, {
        .color = FLASHUTIL_USE_EXISTING_COLOR,
        .paletteOffset = 133,
        .numColors = 1,
        .delay = 1,
        .unk6 = 255,
        .numFadeCycles = 8,
        .unk7_5 = 2,
        .colorDeltaDir = FALSE
    }, {
        .color = FLASHUTIL_USE_EXISTING_COLOR,
        .paletteOffset = 149,
        .numColors = 1,
        .delay = 1,
        .unk6 = 255,
        .numFadeCycles = 8,
        .unk7_5 = 2,
        .colorDeltaDir = FALSE
    }, {
        .color = FLASHUTIL_USE_EXISTING_COLOR,
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

const struct RouletteFlashSettings sFlashData_PokeIcons[] = {
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

const struct YesNoFuncTable sYesNoTable_AcceptMinBet = {
    Task_AcceptMinBet,
    Task_DeclineMinBet
};

const struct YesNoFuncTable sYesNoTable_KeepPlaying = {
    Task_ContinuePlaying,
    Task_StopPlaying
};

extern u16 gSpecialVar_0x8004;
extern u16 gPlttBufferFaded[];
extern u16 gPlttBufferUnfaded[];
extern const u8 Roulette_Text_ControlsInstruction[];
extern const u8 Roulette_Text_KeepPlaying[];
extern const u8 Roulette_Text_Jackpot[];
extern const u8 Roulette_Text_ItsAHit[];
extern const u8 Roulette_Text_NothingDoing[];
extern const u8 Roulette_Text_YouveWonXCoins[];
extern const u8 Roulette_Text_BoardWillBeCleared[];
extern const u8 Roulette_Text_CoinCaseIsFull[];
extern const u8 Roulette_Text_NoCoinsLeft[];
extern const u8 Roulette_Text_PlayMinimumWagerIsX[];
extern const u8 Roulette_Text_SpecialRateTable[];
extern const u8 Roulette_Text_NotEnoughCoins[];
#if DEBUG
EWRAM_DATA u8 gRouletteDebugActive[4] = { 0 };
EWRAM_DATA u8 gRouletteDebugForceBallStuck = 0;
#endif


void CB2_Roulette(void)
{
	RunTasks();
	AnimateSprites();
	BuildOamBuffer();
	if (sRoulette->flashUtil.enabled)
	   RouletteFlash_Run(&sRoulette->flashUtil);
}

void VBlankCB_Roulette(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    UpdateWheelPosition();
    REG_BG1HOFS = 0x200 - sRoulette->gridX;
    if (sRoulette->shroomishShadowTimer)
        REG_BLDALPHA = sRoulette->shroomishShadowAlpha;
    if (sRoulette->updateGridHighlight != 0)
    {
        DmaCopy16(3, &gBGTilemapBuffers[1][224], (void *)(VRAM + 0x21c0), 0x340);
        sRoulette->updateGridHighlight = 0;
    }
    switch (sRoulette->selectionRectDrawState)
    {
    case SELECT_STATE_DRAW:
        REG_BG0CNT = (32 * 248);
        DmaCopy16(3, &gBGTilemapBuffers[2][224], (void *)(VRAM + 0xf9c0), 0x340);
        sRoulette->selectionRectDrawState = SELECT_STATE_UPDATE;
    break;
    case SELECT_STATE_UPDATE:
        DmaCopy16(3, &gBGTilemapBuffers[2][224], (void *)(VRAM + 0xf9c0), 0x340);
    break;
    case SELECT_STATE_ERASE:
        REG_BG0CNT = 0x1f08;
        DmaFill16(3, 0, (void *)(VRAM + 0xf9c0), 0x340);
        sRoulette->selectionRectDrawState = SELECT_STATE_WAIT;
    case SELECT_STATE_WAIT:
        break;
    }
}

void InitRouletteTableData(void)
{
    u8 i;
    u16 bgColors[] = {RGB(24, 4, 10), RGB(10, 19, 6), RGB(24, 4, 10)};
    memset(sRoulette, 0, sizeof(*sRoulette));
    sRoulette->tableId = (gSpecialVar_0x8004 & 1);
    if (gSpecialVar_0x8004 & ROULETTE_SPECIAL_RATE)
        sRoulette->isSpecialRate = TRUE;
    sRoulette->wheelSpeed = sRouletteTables[sRoulette->tableId].wheelSpeed;
    sRoulette->wheelDelay = sRouletteTables[sRoulette->tableId].wheelDelay;
    sRoulette->minBet = sTableMinBets[sRoulette->tableId + sRoulette->isSpecialRate * 2];
    sRoulette->unk1 = 1;
    if (sRoulette->minBet == 1)
        gPlttBufferUnfaded[0] = gPlttBufferUnfaded[81] = gPlttBufferFaded[0] = gPlttBufferFaded[81] = bgColors[0];
    else
        gPlttBufferUnfaded[0] = gPlttBufferUnfaded[81] = gPlttBufferFaded[0] = gPlttBufferFaded[81] = bgColors[1];
    RouletteFlash_Reset(&sRoulette->flashUtil);
    for (i = 0; i < NUM_ROULETTE_SLOTS + 1; i++)
        RouletteFlash_Add(&sRoulette->flashUtil, i, &sFlashData_Colors[i]);
    for (i = 0; i < PARTY_SIZE; i++)
    {
        switch (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES2))
        {
        case SPECIES_SHROOMISH:
            sRoulette->partySpeciesFlags |= HAS_SHROOMISH;
            break;
        case SPECIES_TAILLOW:
            sRoulette->partySpeciesFlags |= HAS_TAILLOW;
            break;
        }
    }
    RtcCalcLocalTime();
}

void CB2_LoadRoulette(void)
{
    u32 temp_IME;
    u8 taskId;

#if DEBUG
    gRouletteDebugState = sRoulette;
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
        Text_LoadWindowTemplate(&gMenuTextWindowLoadTemplate);
        InitMenuWindow(&gMenuTextWindowTemplate);
        LoadPalette(sWheel_Pal, 0, 0x1c0);
        gMain.state++;
        break;
    case 3:
        InitRouletteTableData();
        ClearBGTilemapBuffers();
        LZ77UnCompWram(sGrid_Tilemap, (void *)(sRouletteGridTilemap));
        LZ77UnCompVram(sWheel_Tilemap, (void *)(VRAM + 0x3000));
        gMain.state++;
        break;
    case 4:
        LoadOrFreeMiscSpritePalettesAndSheets(0);
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
        SetVBlankCallback(VBlankCB_Roulette);
        BeginHardwarePaletteFade(255, 0, 16, 0 , 1);
        taskId = sRoulette->playTaskId = CreateTask(Task_StartPlaying, 0);
        gTasks[taskId].tBallNum = BALLS_PER_ROUND;
        gTasks[taskId].tCoins = gSaveBlock1.coins;
        sRoulette->spinTaskId = CreateTask(Task_SpinWheel, 1);
        SetMainCallback2(CB2_Roulette);
        break;
    }
}

void Task_SpinWheel(u8 taskId)
{
    s16 sin;
    s16 cos;
    s32 cos32;

    if (sRoulette->wheelDelayTimer++ == sRoulette->wheelDelay)
    {
        sRoulette->wheelDelayTimer = 0;
        if ((sRoulette->wheelAngle -= sRoulette->wheelSpeed) < 0)
            sRoulette->wheelAngle = 360 - sRoulette->wheelSpeed;
    }
    sin = Sin2(sRoulette->wheelAngle);
    cos = Cos2(sRoulette->wheelAngle);
    sin = sin / 16;
    cos32 = cos / 16;
    sRoulette->wheelRotation.d =  cos32;
    sRoulette->wheelRotation.a =  cos32;
    sRoulette->wheelRotation.b =  sin;
    sRoulette->wheelRotation.c = -sin;
#if DEBUG
    if (gRouletteDebugActive[0] != 0 && JOY_NEW(START_BUTTON))
    {
        gTasks[sRoulette->playTaskId].func = Task_StopPlaying;
        m4aMPlayStop(&gMPlayInfo_SE1);
        m4aMPlayStop(&gMPlayInfo_SE2);
        m4aMPlayStop(&gMPlayInfo_SE3);
    }
#endif
}

void Task_StartPlaying(u8 taskId)
{
    if (UpdatePaletteFade() == 0)
    {
        REG_BLDCNT   |= BLDCNT_TGT2_BG2 | BLDCNT_TGT2_BD;
        REG_BLDALPHA  = BLDALPHA_BLEND(8, 8);
        gTasks[taskId].tBallNum = 0;
        ResetBallDataForNewSpin(taskId);
        ResetHits();
        HideWheelBalls();
        DrawGridBackground(SELECTION_NONE);
        SetBallCounterNumLeft(BALLS_PER_ROUND);
        StartTaskAfterDelayOrInput(taskId, Task_SelectFirstEmptySquare, NO_DELAY, A_BUTTON | B_BUTTON);
    }
}

void Task_AskKeepPlaying(u8 taskId)
{
    DisplayYesNoMenu(20, 8, 1);
    UpdateOutlineCursorPaletteByColor(RGB(30, 12, 11));
    Menu_DrawStdWindowFrame(0, 14, 29, 19);
    Menu_PrintText(Roulette_Text_KeepPlaying, 1, 15);
    DoYesNoFuncWithChoice(taskId, &sYesNoTable_KeepPlaying);
}

void Task_ContinuePlaying(u8 taskId)
{
    Menu_EraseWindowRect(20, 8, 26, 13);
    gTasks[taskId].func = Task_SelectFirstEmptySquare;
}

void Task_StopPlaying(u8 taskId)
{
    DestroyTask(sRoulette->spinTaskId);
    ExitRoulette(taskId);
}

void UpdateGridSelectionRect(u8 selectionId)
{
    u8 temp0, temp1;
    switch (selectionId)
    {
    case SELECTION_NONE:
        FillTilemapRect(&gBGTilemapBuffers[2][0], 0, 14, 7, 16, 13);
        break;
    case COL_WYNAUT:
    case COL_AZURILL:
    case COL_SKITTY:
    case COL_MAKUHITA:
        temp0 = (selectionId * 3 + 14);
        FillTilemapRect(&gBGTilemapBuffers[2][0], 0, 14, 7, 16, 13);
        SetTilemapRect(&gBGTilemapBuffers[2][0], sRouletteGridTilemap + 0x119, temp0, 7, 3, 13);
        break;
    case ROW_ORANGE:
    case ROW_GREEN:
    case ROW_PURPLE:
        temp1 = ((selectionId - 1) / 5 * 3 + 10);
        FillTilemapRect(&gBGTilemapBuffers[2][0], 0, 14, 7, 16, 13);
        SetTilemapRect(&gBGTilemapBuffers[2][0], sRouletteGridTilemap + 0x140, 14, temp1, 16, 3);
        break;
    default:
        temp0 = (GET_COL(selectionId) * 3 + 14);
        temp1 = ((selectionId - 1) / 5 * 3 + 7);
        FillTilemapRect(&gBGTilemapBuffers[2][0], 0, 14, 7, 16, 13);
        SetTilemapRect(&gBGTilemapBuffers[2][0], sRouletteGridTilemap + 0x110, temp0, temp1, 3, 3);
        break;
    }
}

void UpdateGridSelection(u8 taskId, bool8 useCurrentBet)
{
    if (!useCurrentBet)
        SetMultiplierSprite(gTasks[taskId].tSelectionId);
    else
        SetMultiplierSprite(sRoulette->betSelection[sRoulette->curBallNum]);
    UpdateGridSelectionRect(gTasks[taskId].tSelectionId);
}

void Task_StartHandleBetGridInput(u8 taskId)
{
    sRoulette->selectionRectDrawState = SELECT_STATE_DRAW;
    UpdateGridSelectionRect(gTasks[taskId].tSelectionId);
    sRoulette->wheelDelay = 2;
    sRoulette->wheelDelayTimer = 0;
    gTasks[taskId].func = Task_HandleBetGridInput;
}

void Task_SelectFirstEmptySquare(u8 taskId)
{
    s16 i;

    if (sRoulette->hitFlags & F_ORANGE_ROW)
    {
        for (i = SQU_GREEN_WYNAUT; i < SQU_GREEN_MAKUHITA; i++)
            if ((sRoulette->hitFlags & sGridSelections[i].flag) == 0)
                break;
    }
    else
    {
        for (i = SQU_ORANGE_WYNAUT; i < ROW_GREEN; i++)
            if ((sRoulette->hitFlags & sGridSelections[i].flag) == 0)
                break;
    }
    gTasks[taskId].tSelectionId = i;
    ResetBallDataForNewSpin(taskId);
    DrawGridBackground(gTasks[taskId].tSelectionId);
    SetMultiplierSprite(gTasks[taskId].tSelectionId);
    FlashSelectionOnWheel(gTasks[taskId].tSelectionId);
    gTasks[taskId].data[1] = 0;
    gTasks[taskId].func = Task_StartHandleBetGridInput;
}

u8 CanMoveSelectionInDir(s16 *selectionId, u8 dir)
{
    s8 minSelectionId = 0;
    s8 maxSelectionId = 0;
    s8 moveOffsets[4] = {-5, 5, -1, 1};
    s8 originalSelection;
    originalSelection = (u8)*selectionId;
    switch (dir)
    {
    case 0:
    case 1:
        minSelectionId = GET_COL(*selectionId);
        maxSelectionId = minSelectionId + ROW_PURPLE;
        if (minSelectionId == SELECTION_NONE)
            minSelectionId = ROW_ORANGE;
        break;
    case 2:
    case 3:
        minSelectionId = GET_ROW(*selectionId);
        maxSelectionId = minSelectionId + COL_MAKUHITA;
        if (minSelectionId == SELECTION_NONE)
            minSelectionId = COL_WYNAUT;
        break;
    }
    *selectionId += moveOffsets[dir];
    if (*selectionId < minSelectionId)
        *selectionId = maxSelectionId;
    if (*selectionId > maxSelectionId)
        *selectionId = minSelectionId;
    if (*selectionId != originalSelection)
        return TRUE;
    return FALSE;
}

void ProcessBetGridInput(u8 taskId)
{
    u8 headerOffset = 0;
    bool8 dirPressed = FALSE;
    if (JOY_NEW(DPAD_UP))
    {
        dirPressed = TRUE;
        if (!CanMoveSelectionInDir(&gTasks[taskId].tSelectionId, 0))
            return;
    }
    if (JOY_NEW(DPAD_DOWN))
    {
        dirPressed = TRUE;
        if (!CanMoveSelectionInDir(&gTasks[taskId].tSelectionId, 1))
            return;
    }
    if (JOY_NEW(DPAD_LEFT))
    {
        dirPressed = TRUE;
        if (!CanMoveSelectionInDir(&gTasks[taskId].tSelectionId, 2))
            return;
    }
    if (JOY_NEW(DPAD_RIGHT))
    {
        dirPressed = TRUE;
        if (!CanMoveSelectionInDir(&gTasks[taskId].tSelectionId, 3))
            return;
    }
    if (dirPressed)
    {
        u8 i;
        DrawGridBackground(gTasks[taskId].tSelectionId);
        UpdateGridSelection(taskId, FALSE);
        gTasks[taskId].data[1] = headerOffset;
        PlaySE(SE_SELECT);
        RouletteFlash_Stop(&sRoulette->flashUtil, NO_DELAY);
        sRoulette->flashUtil.palettes[FLASH_ICON_3].available = FALSE;
        sRoulette->flashUtil.palettes[FLASH_ICON_2].available = FALSE;
        sRoulette->flashUtil.palettes[FLASH_ICON].available = FALSE;
        FlashSelectionOnWheel(gTasks[taskId].tSelectionId);
        for (i = 0; i < 4; i++)
        {
            gSprites[sRoulette->spriteIds[i + 41]].oam.tileNum =
            gSprites[sRoulette->spriteIds[i + 41]].sheetTileStart
            + (*gSprites[sRoulette->spriteIds[i + 41]].anims)->type;
        }
        if ((u16)(gTasks[taskId].tSelectionId - 1) < COL_MAKUHITA && !(sRoulette->hitFlags & sGridSelections[gTasks[taskId].tSelectionId].flag))
        {
            headerOffset = gTasks[taskId].tSelectionId - 1;
            gSprites[sRoulette->spriteIds[headerOffset + 41]].oam.tileNum =
            gSprites[sRoulette->spriteIds[headerOffset + 41]].sheetTileStart
            + (*gSprites[sRoulette->spriteIds[headerOffset + 41]].anims + 1)->type;
        }
    }
}

void Task_StartSpin(u8 taskId)
{
    sRoulette->selectionRectDrawState = SELECT_STATE_ERASE;
    if (sRoulette->minBet == 1)
        sRoulette->wheelDelay = 1;
    else
        sRoulette->wheelDelay = 0;
    sRoulette->wheelDelayTimer = 0;
    gTasks[taskId].data[1] = 32;
    gTasks[taskId].func = Task_SlideGridOffscreen;
}

void Task_PlaceBet(u8 taskId)
{
    sRoulette->betSelection[sRoulette->curBallNum] = gTasks[taskId].tSelectionId;
    gTasks[taskId].tMultiplier = GetMultiplier(sRoulette->betSelection[sRoulette->curBallNum]);
    SetMultiplierSprite(sRoulette->betSelection[sRoulette->curBallNum]);
    if ((gTasks[taskId].tCoins -= sRoulette->minBet) < 0)
        gTasks[taskId].tCoins = 0;
    SetCreditDigits(gTasks[taskId].tCoins);
    gTasks[taskId].func = Task_StartSpin;
}

void Task_HandleBetGridInput(u8 taskId)
{
    ProcessBetGridInput(taskId);
    switch (gTasks[taskId].data[1])
    {
    case 0:
        UpdateGridSelectionRect(gTasks[taskId].tSelectionId);
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
    if (sRoulette->hitFlags & sGridSelections[gTasks[taskId].tSelectionId].flag)
            PlaySE(SE_BOO);
        else
        {
            m4aSongNumStart(SE_SHOP);
            gTasks[taskId].func = Task_PlaceBet;
        }
    }
}

void Task_SlideGridOffscreen(u8 taskId)
{
    if (gTasks[taskId].data[1]-- > 0)
    {
        if (gTasks[taskId].data[1] > 2)
            gSpriteCoordOffsetX += 2;
        if ((sRoulette->gridX += 4) == 104)
            gSprites[sRoulette->spriteIds[25]].callback = &SpriteCallbackDummy;
    }
    else
    {
        ShowHideGridIcons(TRUE, -1);
        ShowHideGridBalls(TRUE, -1);
        gTasks[taskId].func = Task_InitBallRoll;
        gTasks[taskId].data[1] = 0;
    }
}

u8 GetRandomForBallTravelDistance(u16 ballNum, u16 rand)
{
    u8 hourOffset;
    switch (sRoulette->partySpeciesFlags)
    {
    case HAS_SHROOMISH:
    case HAS_TAILLOW:
        // one of the two is in party
        hourOffset = gLocalTime.hours - 4;
        if (!(hourOffset > 5))
        {
            if (!(ballNum < BALLS_PER_ROUND * 2 || (rand & 1)))
                return 1;
            else
            {
                const struct RouletteTable *tables = sRouletteTables;
                return tables[sRoulette->tableId].randDistanceLow / 2;
            }
        }
        else if (!(rand & 3))
        {
            const struct RouletteTable *tables = sRouletteTables;
            return tables[sRoulette->tableId].randDistanceLow / 2;
        }
        else
        {
            const struct RouletteTable *tables = sRouletteTables;
            return tables[sRoulette->tableId].randDistanceLow;
        }
        break;
    case HAS_SHROOMISH | HAS_TAILLOW:
        // both are in party
        hourOffset = gLocalTime.hours - 4;
        if (!(hourOffset > 6))
        {
            if (ballNum < BALLS_PER_ROUND || (rand & 1))
            {
                const struct RouletteTable *tables = sRouletteTables;
                return tables[sRoulette->tableId].randDistanceLow / 2;
            }
            else
            {
                return 1;
            }
        }
        else if ((rand & 1) && ballNum > BALLS_PER_ROUND)
        {
            const struct RouletteTable *tables = sRouletteTables;
            return tables[sRoulette->tableId].randDistanceLow / 4;
        }
        else
        {
            const struct RouletteTable *tables = sRouletteTables;
            return tables[sRoulette->tableId].randDistanceLow / 2;
        }
        break;
    case 0:
    default:
        // neither is in party
        hourOffset = gLocalTime.hours - 4;
        if (!(hourOffset > 5))
        {
            if (!(rand & 3))
            {
                return 1;
            }
            else
            {
                const struct RouletteTable *tables = sRouletteTables;
                return tables[sRoulette->tableId].randDistanceLow / 2;
            }
        }
        else if (!(rand & 3))
        {
            if (ballNum > BALLS_PER_ROUND * 2)
            {
                const struct RouletteTable *tables = sRouletteTables;
                return tables[sRoulette->tableId].randDistanceLow / 2;
            }
            else
            {
                const struct RouletteTable *tables = sRouletteTables;
                return tables[sRoulette->tableId].randDistanceLow;
            }
        }
        else if (rand & (1 << 15))
        {
            if (ballNum > BALLS_PER_ROUND * 2)
            {
                const struct RouletteTable *tables = sRouletteTables;
                return tables[sRoulette->tableId].randDistanceLow;
            }
            else
            {
                const struct RouletteTable *tables = sRouletteTables;
                return tables[sRoulette->tableId].randDistanceHigh;
            }
        }
        else
        {
            const struct RouletteTable *tables = sRouletteTables;
            return tables[sRoulette->tableId].randDistanceHigh * 2;
        }
    }
}

void Task_InitBallRoll(u8 taskId)
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
    sRoulette->curBallSpriteId = gTasks[taskId].tBallNum;
    sRoulette->stuckHitSlot = BALL_STATE_ROLLING;
    sRoulette->hitSlot = BALL_STATE_ROLLING;
    sRoulette->ballState = BALL_STATE_ROLLING;
    randTravelMod = GetRandomForBallTravelDistance(gTasks[taskId].tTotalBallNum, rand);
    randTravelDist = (rand % randTravelMod) - (randTravelMod / 2);
    if (gLocalTime.hours < 13)
        startAngleId = 0;
    else
        startAngleId = 1;
    if (randmod < 80)
        startAngleId *= 2;
    else
        startAngleId = (1 - startAngleId) * 2;
    sRoulette->ballTravelDist = travelDist = sRouletteTables[sRoulette->tableId].baseTravelDist + randTravelDist;
    travelDist = S16TOPOSFLOAT(travelDist) / 5.0f;
    sRoulette->ballTravelDistFast = travelDist * 3;
    sRoulette->ballTravelDistMed = travelDist;
    sRoulette->ballTravelDistSlow = travelDist;
    sRoulette->ballAngle = S16TOPOSFLOAT(startAngles[(rand & 1) + startAngleId]);
    sRoulette->ballAngleSpeed = S16TOPOSFLOAT(sRouletteTables[sRoulette->tableId].ballSpeed);
    sRoulette->ballAngleAccel = ((sRoulette->ballAngleSpeed * 0.5f) - sRoulette->ballAngleSpeed) / S16TOPOSFLOAT(travelDist * 3);
    sRoulette->ballDistToCenter = 68.0f;
    sRoulette->ballFallAccel = 0.0f;
    sRoulette->ballFallSpeed = -(8.0f / S16TOPOSFLOAT(travelDist * 3));
    sRoulette->varA0 = 36.0f;
    gTasks[taskId].func = Task_RollBall;
}

void Task_RollBall(u8 taskId)
{
    u8 ballSpriteId;
    sRoulette->ballRolling = TRUE;
    ballSpriteId = sRoulette->spriteIds[sRoulette->curBallSpriteId];
    sRoulette->ball = &gSprites[ballSpriteId];
    sRoulette->ball->callback = SpriteCB_RollBall_Start;
    gTasks[taskId].tBallNum++;
    gTasks[taskId].tTotalBallNum++;
    SetBallCounterNumLeft(BALLS_PER_ROUND - gTasks[taskId].tBallNum);
    m4aSongNumStart(SE_ROULETTE_BALL);
    gTasks[taskId].func = Task_RecordBallHit;
}

void Task_RecordBallHit(u8 taskId)
{
    if (sRoulette->ballState != BALL_STATE_ROLLING)
    {
        if (sRoulette->ballStuck)
        {
            if (sRoulette->ballUnstuck)
            {
                sRoulette->ballUnstuck = FALSE;
                sRoulette->ballStuck = FALSE;
            }
        }
        else
        {
            if (!gTasks[taskId].data[1])
            {
                bool8 won = IsHitInBetSelection(RecordHit(taskId, sRoulette->hitSlot), sRoulette->betSelection[sRoulette->curBallNum]);
                gTasks[taskId].tWonBet = won;
                if (won == TRUE)
                    RouletteFlash_Enable(&sRoulette->flashUtil, F_FLASH_OUTER_EDGES);
            }
            if (gTasks[taskId].data[1] < 61)
            {
                if (JOY_NEW(A_BUTTON))
                    gTasks[taskId].data[1] = 60;
                gTasks[taskId].data[1]++;
            }
            else
            {
                DrawGridBackground(sRoulette->betSelection[sRoulette->curBallNum]);
                ShowHideGridIcons(FALSE, (u8)gTasks[taskId].tWinningSquare);
                ShowHideGridBalls(FALSE, (u8)gTasks[taskId].tBallNum - 1);
                gTasks[taskId].data[1] = 32;
                gTasks[taskId].func = Task_SlideGridOnscreen;
            }
        }
    }
}

void Task_SlideGridOnscreen(u8 taskId)
{
    if (gTasks[taskId].data[1]-- > 0)
    {
        if (gTasks[taskId].data[1] > 2)
            gSpriteCoordOffsetX -= 2;
        if ((sRoulette->gridX -= 4) == 104)
            gSprites[sRoulette->spriteIds[25]].callback = SpriteCB_GridSquare;
    }
    else
    {
        ShowHideWinSlotCursor(gTasks[taskId].tWinningSquare);
        if (gTasks[taskId].tWonBet == TRUE)
            gTasks[taskId].data[1] = 121;
        else
            gTasks[taskId].data[1] = 61;
        gTasks[taskId].func = Task_FlashBallOnWinningSquare;
    }
}

void Task_FlashBallOnWinningSquare(u8 taskId)
{
    if (gTasks[taskId].data[1]-- > 1)
    {
        switch (gTasks[taskId].data[1] % 16)
        {
        case 8:
            ShowHideGridIcons(FALSE, -1);
            ShowHideGridBalls(FALSE, -1);
            break;
        case 0:
            ShowHideGridIcons(FALSE, (u8)gTasks[taskId].tWinningSquare);
            ShowHideGridBalls(FALSE, (u8)gTasks[taskId].tBallNum - 1);
            break;
        }
    }
    else
    {
        StartTaskAfterDelayOrInput(taskId, Task_PrintSpinResult, 30, 0);
    }
}

void Task_TryIncrementWins(u8 taskId)
{
    switch (gTasks[taskId].tWonBet)
    {
    case TRUE:
    case 2:
        if (IsFanfareTaskInactive())
        {
            u32 wins = GetGameStat(GAME_STAT_CONSECUTIVE_ROULETTE_WINS);
            if (wins < ++gTasks[taskId].tConsecutiveWins)
                SetGameStat(GAME_STAT_CONSECUTIVE_ROULETTE_WINS, gTasks[taskId].tConsecutiveWins);
            StartTaskAfterDelayOrInput(taskId, Task_PrintPayout, NO_DELAY, A_BUTTON | B_BUTTON);
        }
        break;
    case FALSE:
    default:
        if (!IsSEPlaying())
        {
            gTasks[taskId].tConsecutiveWins = FALSE;
            StartTaskAfterDelayOrInput(taskId, Task_EndTurn, NO_DELAY, A_BUTTON | B_BUTTON);
        }
    }
}

void Task_PrintSpinResult(u8 taskId)
{
    switch (gTasks[taskId].tWonBet)
    {
    case TRUE:
    case 2:
        if (gTasks[taskId].tMultiplier == MAX_MULTIPLIER)
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
    case FALSE:
    default:
        m4aSongNumStart(SE_FAILURE);
        Menu_DrawStdWindowFrame(0, 14, 29, 19);
        Menu_PrintText(Roulette_Text_NothingDoing, 1, 15);
    }
    gTasks[taskId].data[1] = 0;
    gTasks[taskId].func = Task_TryIncrementWins;
}

#define tPayout data[1]

void Task_GivePayout(u8 taskId)
{
    s32 payoutState = gTasks[taskId].data[7];
    switch (payoutState)
    {
    case 0:
        gTasks[taskId].tCoins++;
        m4aSongNumStart(SE_PIN);
        SetCreditDigits(gTasks[taskId].tCoins);
        if (gTasks[taskId].tCoins > 0x270e)
        {
            gTasks[taskId].tPayout = payoutState;
        }
        else
        {
            gTasks[taskId].tPayout--;
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
    if (gTasks[taskId].tPayout == 0)
        StartTaskAfterDelayOrInput(taskId, Task_EndTurn, NO_DELAY, A_BUTTON | B_BUTTON);
}

void Task_PrintPayout(u8 taskId)
{
    ConvertIntToDecimalStringN((u8 *)&gStringVar1, (sRoulette->minBet * gTasks[taskId].tMultiplier), STR_CONV_MODE_LEFT_ALIGN, 2);
    StringExpandPlaceholders((u8 *)&gStringVar4, Roulette_Text_YouveWonXCoins);
    Menu_DrawStdWindowFrame(0, 14, 29, 19);
    Menu_PrintText((u8 *)&gStringVar4, 1, 15);
    gTasks[taskId].tPayout = (sRoulette->minBet * gTasks[taskId].tMultiplier);
    gTasks[taskId].data[7] = 0;
    gTasks[taskId].func = Task_GivePayout;
}

#undef tPayout

void Task_EndTurn(u8 taskId)
{
    RouletteFlash_Stop(&sRoulette->flashUtil, NO_DELAY);
    sRoulette->flashUtil.palettes[FLASH_ICON_3].available = FALSE;
    sRoulette->flashUtil.palettes[FLASH_ICON_2].available = FALSE;
    sRoulette->flashUtil.palettes[FLASH_ICON].available = FALSE;
    gSprites[sRoulette->spriteIds[7 + sGridSelections[gTasks[taskId].tWinningSquare].spriteIdOffset]].invisible = TRUE;
    gTasks[taskId].func = Task_TryPrintEndTurnMsg;
}

void Task_TryPrintEndTurnMsg(u8 taskId)
{
    u8 i = 0;
    gTasks[taskId].tSelectionId = i;
    sRoulette->betSelection[sRoulette->curBallNum] = SELECTION_NONE;
    DrawGridBackground(SELECTION_NONE);
    gSprites[sRoulette->spriteIds[48]].invisible = TRUE;
    for (i = 0; i < 4; i++)
    {
        gSprites[sRoulette->spriteIds[i + 41]].oam.tileNum =
        gSprites[sRoulette->spriteIds[i + 41]].sheetTileStart
        + (*gSprites[sRoulette->spriteIds[i + 41]].anims)->type;
    }
    if (!(gTasks[taskId].tCoins < sRoulette->minBet))
    {
        if (gTasks[taskId].tBallNum == BALLS_PER_ROUND)
        {
            Menu_DrawStdWindowFrame(0, 14, 29, 19);
            Menu_PrintText(Roulette_Text_BoardWillBeCleared, 1, 15);
            StartTaskAfterDelayOrInput(taskId, &Task_ClearBoard, NO_DELAY, A_BUTTON | B_BUTTON);
        }
        else if (gTasks[taskId].tCoins == 0x270f)
        {
            Menu_DrawStdWindowFrame(0, 14, 29, 19);
            Menu_PrintText(Roulette_Text_CoinCaseIsFull, 1, 15);
            StartTaskAfterDelayOrInput(taskId, Task_AskKeepPlaying, NO_DELAY, A_BUTTON | B_BUTTON);
        }
        else
        {
            gTasks[taskId].func = Task_AskKeepPlaying;
        }
    }
    else
    {
        Menu_DrawStdWindowFrame(0, 14, 29, 19);
        Menu_PrintText(Roulette_Text_NoCoinsLeft, 1, 15);
        StartTaskAfterDelayOrInput(taskId, Task_StopPlaying, 60, A_BUTTON | B_BUTTON);
    }
}

void Task_ClearBoard(u8 taskId)
{
    u8 i = 0;
    gTasks[taskId].tBallNum = 0;
    ResetBallDataForNewSpin(taskId);
    ResetHits();
    HideWheelBalls();
    DrawGridBackground(SELECTION_NONE);
    SetBallCounterNumLeft(BALLS_PER_ROUND);
    for (i = 0; i < NUM_ROULETTE_SLOTS; i++)
        gSprites[sRoulette->spriteIds[i + 7]].invisible = FALSE;
    if (gTasks[taskId].tCoins == 0x270f)
    {
        Menu_DrawStdWindowFrame(0, 14, 29, 19);
        Menu_PrintText(Roulette_Text_CoinCaseIsFull, 1, 15);
        StartTaskAfterDelayOrInput(taskId, Task_AskKeepPlaying, NO_DELAY, A_BUTTON | B_BUTTON);
    }
    else
    {
        gTasks[taskId].func = Task_AskKeepPlaying;
    }
}

void ExitRoulette(u8 taskId)
{
    RouletteFlash_Stop(&sRoulette->flashUtil, NO_DELAY);
    RouletteFlash_Reset(&sRoulette->flashUtil);
    gSaveBlock1.coins = gTasks[taskId].tCoins;
    if (gSaveBlock1.coins < sRoulette->minBet)
        gSpecialVar_0x8004 = TRUE;
    else
        gSpecialVar_0x8004 = FALSE;
    BeginHardwarePaletteFade(255, 0, 0, 16, 0);
    gTasks[taskId].func = Task_ExitRoulette;
}

void Task_ExitRoulette(u8 taskId) // end roulette ?
{
    if (UpdatePaletteFade() == 0)
    {
        SetVBlankCallback(NULL);
        memset(sRoulette, 0, sizeof(*sRoulette));
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
        gRouletteDebugActive[0] = 0;
#endif
    }
}

void Task_WaitForNextTask(u8 taskId)
{
    if (sRoulette->taskWaitDelay == 0 || JOY_NEW(sRoulette->taskWaitKey))
    {
        gTasks[taskId].func = sRoulette->nextTask;
        if (sRoulette->taskWaitKey > 0)
            PlaySE(SE_SELECT);
        sRoulette->nextTask = NULL;
        sRoulette->taskWaitKey = 0;
        sRoulette->taskWaitDelay = 0;
    }
    if (sRoulette->taskWaitDelay != NO_DELAY)
        sRoulette->taskWaitDelay--;
}

void StartTaskAfterDelayOrInput(u8 taskId, TaskFunc task, u16 delay, u16 key)
{
    sRoulette->prevTask = gTasks[taskId].func;
    if (task == NULL)
        task = sRoulette->prevTask;
    sRoulette->nextTask = task;
    sRoulette->taskWaitDelay = delay;
    if (delay == NO_DELAY && key == 0)
        sRoulette->taskWaitKey = NO_DELAY;
    else
        sRoulette->taskWaitKey = key;
    gTasks[taskId].func = Task_WaitForNextTask;
}

void ResetBallDataForNewSpin(u8 taskId)
{
    u8 i = 0;
    sRoulette->unk0 = i;
    sRoulette->useTaillow = 0;
    sRoulette->ballStuck = 0;
    sRoulette->ballUnstuck = 0;
    sRoulette->ballRolling = 0;
    for (i = 0; i < BALLS_PER_ROUND; i++)
        sRoulette->betSelection[i] = SELECTION_NONE;
    sRoulette->curBallNum = SELECTION_NONE;
    gTasks[taskId].data[1] = 0;
}

void ResetHits(void)
{
    u8 i;
    sRoulette->hitFlags = 0;
    for (i = 0; i < BALLS_PER_ROUND; i++)
        sRoulette->hitSquares[i] = SELECTION_NONE;
    for (i = 0; i < NUM_BOARD_POKES; i++)
        sRoulette->pokeHits[i] = 0;
    for (i = 0; i < NUM_BOARD_COLORS; i++)
        sRoulette->colorHits[i] = 0;
    ShowHideGridBalls(TRUE, -1);
}

u8 RecordHit(u8 taskId, u8 slotId)
{
    u32 columnFlags[NUM_BOARD_POKES] = {
        F_WYNAUT_COL | F_ORANGE_WYNAUT | F_GREEN_WYNAUT | F_PURPLE_WYNAUT,
        F_AZURILL_COL | F_ORANGE_AZURILL | F_GREEN_AZURILL | F_PURPLE_AZURILL,
        F_SKITTY_COL | F_ORANGE_SKITTY | F_GREEN_SKITTY | F_PURPLE_SKITTY,
        F_MAKUHITA_COL | F_ORANGE_MAKUHITA | F_GREEN_MAKUHITA | F_PURPLE_MAKUHITA,
    };
    u32 rowFlags[NUM_BOARD_COLORS] = {
        F_ORANGE_ROW | F_ORANGE_WYNAUT | F_ORANGE_AZURILL | F_ORANGE_SKITTY | F_ORANGE_MAKUHITA,
        F_GREEN_ROW | F_GREEN_WYNAUT | F_GREEN_AZURILL | F_GREEN_SKITTY | F_GREEN_MAKUHITA,
        F_PURPLE_ROW | F_PURPLE_WYNAUT | F_PURPLE_AZURILL | F_PURPLE_SKITTY | F_PURPLE_MAKUHITA,
    };
    if (slotId >= NUM_ROULETTE_SLOTS)
    {
        return 0;
    }
    else
    {
        u8 i;
        u8 j;
        sRoulette->hitSquares[gTasks[taskId].tBallNum - 1] = sRouletteSlots[slotId].gridSquare;
        gTasks[taskId].tWinningSquare = sRouletteSlots[slotId].gridSquare;
        sRoulette->hitFlags |= sRouletteSlots[slotId].flag;
        for (i = 0; i < NUM_BOARD_POKES; i++)
        {
            if (sRouletteSlots[slotId].flag & columnFlags[i])
                sRoulette->pokeHits[i]++;
            if (sRoulette->pokeHits[i] >= NUM_BOARD_COLORS)
                sRoulette->hitFlags |= columnFlags[i];
        }
        for (j = 0; j < NUM_BOARD_COLORS; j++)
        {
            if (sRouletteSlots[slotId].flag & rowFlags[j])
                sRoulette->colorHits[j]++;
            if (sRoulette->colorHits[j] >= NUM_BOARD_POKES)
                sRoulette->hitFlags |= rowFlags[j];
        }
        return sRouletteSlots[slotId].gridSquare;
    }
}

bool8 IsHitInBetSelection(u8 gridSquare, u8 betSelection)
{
    u8 hit = gridSquare;
    if (--gridSquare < NUM_GRID_SELECTIONS)
    {
        switch (betSelection)
        {
        case SELECTION_NONE:
            return 3;
            break;
        case COL_WYNAUT ... COL_MAKUHITA:
            if (hit == betSelection + ROW_ORANGE || hit == betSelection + ROW_GREEN || hit == betSelection + ROW_PURPLE)
                return TRUE;
            break;
        case ROW_ORANGE:
        case ROW_GREEN:
        case ROW_PURPLE:
            if (!(hit < betSelection + COL_WYNAUT) && !(hit > betSelection + COL_MAKUHITA))
                return TRUE;
            break;
        default:
            if (hit == betSelection)
                return TRUE;
        }
    }
    return FALSE;
}

void FlashSelectionOnWheel(u8 selectionId)
{
    u16 flashFlags = 0;
    u8 numSelected;
    u16 palOffset;
    u8 i;
    switch (selectionId)
    {
    case ROW_ORANGE:
    case ROW_GREEN:
    case ROW_PURPLE:
        for (i = selectionId + 1; i < selectionId + 5; i++)
            if (!(sRoulette->hitFlags & sGridSelections[i].flag))
                flashFlags |= sGridSelections[i].flashFlags;
        RouletteFlash_Enable(&sRoulette->flashUtil, flashFlags &= ~F_FLASH_ICON);
        break;
    default:
    {
        struct RouletteFlashSettings iconFlash[NUM_BOARD_COLORS];
        memcpy(iconFlash, sFlashData_PokeIcons, sizeof iconFlash);
        if (selectionId >= COL_WYNAUT && selectionId <= COL_MAKUHITA)
            numSelected = NUM_BOARD_COLORS;
        else
            numSelected = 1;
        palOffset = GET_ROW_IDX(selectionId);
        switch (GET_COL(selectionId))
        {
            case COL_WYNAUT:
                palOffset = gSprites[sRoulette->spriteIds[SPR_WHEEL_ICON_ORANGE_WYNAUT]].oam.paletteNum * 16;
                break;
            case COL_AZURILL:
                palOffset = gSprites[sRoulette->spriteIds[SPR_WHEEL_ICON_GREEN_AZURILL]].oam.paletteNum * 16;
                break;
            case COL_SKITTY:
                palOffset = gSprites[sRoulette->spriteIds[SPR_WHEEL_ICON_PURPLE_SKITTY]].oam.paletteNum * 16;
                break;
            case COL_MAKUHITA:
                palOffset = gSprites[sRoulette->spriteIds[SPR_WHEEL_ICON_ORANGE_MAKUHITA]].oam.paletteNum * 16;
                break;
        }
        if (numSelected == 1)
        {
            if (!(sRoulette->hitFlags & sGridSelections[selectionId].flag))
            {
                iconFlash[GET_ROW_IDX(selectionId)].paletteOffset += palOffset;
                RouletteFlash_Add(&sRoulette->flashUtil, FLASH_ICON, &iconFlash[GET_ROW_IDX(selectionId)]);
            }
            else
                return;
        }
        else
        {
            for (i = 0; i < NUM_BOARD_COLORS; i++)
            {
                u8 columnSlotId = i * ROW_ORANGE + selectionId + ROW_ORANGE;
                if (!(sRoulette->hitFlags & sGridSelections[columnSlotId].flag))
                {
                    iconFlash[GET_ROW_IDX(columnSlotId)].paletteOffset += palOffset;
                    RouletteFlash_Add(&sRoulette->flashUtil, i + FLASH_ICON, &iconFlash[GET_ROW_IDX(columnSlotId)]);
                    if (numSelected == NUM_BOARD_COLORS)
                        flashFlags = sGridSelections[columnSlotId].flashFlags;
                    numSelected--;
                }
            }
            if (numSelected != 2)
                flashFlags = 0;
        }
        RouletteFlash_Enable(&sRoulette->flashUtil, flashFlags |= sGridSelections[selectionId].flashFlags);
        break;
    }
    }
}

void DrawGridBackground(u8 selectionId)
{
    vu8 i;
    vu8 j;
    vu16 x;
    vu16 y;
    vu8 tilemapOffset;
    u8 selectionIds[5];
    u8 numSquares;
    sRoulette->updateGridHighlight = TRUE;
    ShowHideGridIcons(FALSE, SELECTION_NONE);
    SetTilemapRect(gBGTilemapBuffers[1], (u16 *)sRouletteGridTilemap, 14, 7, 16, 13);
    switch (selectionId)
    {
    case SELECTION_NONE:
        return;
    case COL_WYNAUT ... COL_MAKUHITA:
        numSquares = NUM_BOARD_COLORS + 1;
        for (i = 0; i < numSquares; i++)
            selectionIds[i] = i * ROW_ORANGE + selectionId;
        break;
    case ROW_ORANGE:
    case ROW_GREEN:
    case ROW_PURPLE:
        numSquares = NUM_BOARD_POKES + 1;
        for (i = 0; i < numSquares; i++)
            selectionIds[i] = i + selectionId;
        break;
    default:
        numSquares = 1;
        selectionIds[0] = selectionId;
    }
    for (i = 0; i < numSquares; i++)
    {
        tilemapOffset = sGridSelections[selectionIds[i]].tilemapOffset;
        x = sGridSelections[selectionIds[i]].x;
        for (j = 0; j < 3; j++)
        {
            y = (sGridSelections[selectionIds[i]].y + j) * 32;
            gBGTilemapBuffers[1][x + y + 0] = sRouletteGridTilemap[0xD0 + (tilemapOffset + j) * 3 + 0];
            gBGTilemapBuffers[1][x + y + 1] = sRouletteGridTilemap[0xD0 + (tilemapOffset + j) * 3 + 1];
            gBGTilemapBuffers[1][x + y + 2] = sRouletteGridTilemap[0xD0 + (tilemapOffset + j) * 3 + 2];
        }
    }
}

u8 GetMultiplier(u8 selectionId)
{
    u8 multipliers[] = {0, 3, 4, 6, MAX_MULTIPLIER};
    if (selectionId > NUM_GRID_SELECTIONS)
        selectionId = SELECTION_NONE;
    switch (sGridSelections[selectionId].baseMultiplier)
    {
    case NUM_BOARD_COLORS:
        selectionId = GET_ROW_IDX(selectionId);
        if (sRoulette->colorHits[selectionId] >= NUM_BOARD_POKES)
            return 0;
        return multipliers[sRoulette->colorHits[selectionId] + 1];
    case NUM_BOARD_POKES:
        selectionId = GET_COL_IDX(selectionId);
        if (sRoulette->pokeHits[selectionId] >= NUM_BOARD_COLORS)
            return 0;
        return multipliers[sRoulette->pokeHits[selectionId] + 2];
    case NUM_ROULETTE_SLOTS:
        if (sRoulette->hitFlags & sGridSelections[selectionId].flag)
            return 0;
        return multipliers[4];
    }
    return 0;
}

void UpdateWheelPosition(void)
{
    s32 bg2x;
    s32 bg2y;
    REG_BG2PA = sRoulette->wheelRotation.a;
    REG_BG2PB = sRoulette->wheelRotation.b;
    REG_BG2PC = sRoulette->wheelRotation.c;
    REG_BG2PD = sRoulette->wheelRotation.d;
    bg2x = 0x7400 - sRoulette->wheelRotation.a * (gSpriteCoordOffsetX + 116)
                - sRoulette->wheelRotation.b * (gSpriteCoordOffsetY + 80);
    bg2y = 0x5400 - sRoulette->wheelRotation.c * (gSpriteCoordOffsetX + 116)
                - sRoulette->wheelRotation.d * (gSpriteCoordOffsetY + 80);
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
    s32 result = 0x10000;
    return result / y;
}

void Task_ShowMinBetYesNo(u8 taskId)
{
    DisplayYesNoMenu(20, 8, 1);
    UpdateOutlineCursorPaletteByColor(RGB(30, 12, 11));
    DoYesNoFuncWithChoice(taskId, &sYesNoTable_AcceptMinBet);
}

void Task_FadeToRouletteGame(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetVBlankCallback(NULL);
        SetMainCallback2(CB2_LoadRoulette);
        DestroyTask(taskId);
    }
}

void Task_AcceptMinBet(u8 taskId)
{
    Menu_EraseWindowRect(20, 8, 26, 13);
    Menu_EraseScreen();
    BeginNormalPaletteFade(0xffffffff, 0, 0, 16, RGB(0, 0, 0));
    gPaletteFade.delayCounter = gPaletteFade.multipurpose2;
    UpdatePaletteFade();
    gTasks[taskId].func = Task_FadeToRouletteGame;
}

void Task_DeclineMinBet(u8 taskId)
{
#if DEBUG
    gRouletteDebugActive[0] = 0;
#endif
    Menu_EraseScreen();
    UnlockPlayerFieldControls();
    DestroyTask(taskId);
}

void Task_NotEnoughForMinBet(u8 taskId)
{
    gTasks[taskId].data[0]++;
    if (!JOY_NEW(A_BUTTON | B_BUTTON) && gTasks[taskId].data[0] < 61)
        return;
    gSpecialVar_0x8004 = 1;
    Menu_EraseScreen();
    UnlockPlayerFieldControls();
    DestroyTask(taskId);
}

void Task_PrintMinBet(u8 taskId)
{
    u32 minBet = sTableMinBets[GET_MIN_BET_ID(gSpecialVar_0x8004)];
    ConvertIntToDecimalStringN(gStringVar1, minBet, 2, 1);
    StringExpandPlaceholders(gStringVar4, Roulette_Text_PlayMinimumWagerIsX);
    Menu_DrawStdWindowFrame(0, 14, 29, 19);
    Menu_PrintText(gStringVar4, 1, 15);
    gTasks[taskId].func = Task_ShowMinBetYesNo;
}

void Task_PrintRouletteEntryMsg(u8 taskId)
{
    s32 minBet;
    ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].tCoins, 1, 4);
    StringExpandPlaceholders(gStringVar4, gOtherText_Coins);
    Menu_DrawStdWindowFrame(0, 0, 9, 3);
    MenuPrint_RightAligned(gStringVar4, 9, 1);
    minBet = sTableMinBets[GET_MIN_BET_ID(gSpecialVar_0x8004)];
    ConvertIntToDecimalStringN(gStringVar1, minBet, 2, 1);
    if (gTasks[taskId].tCoins >= minBet)
    {
        if ((gSpecialVar_0x8004 & ROULETTE_SPECIAL_RATE) && (gSpecialVar_0x8004 & 1))
        {
            Menu_DrawStdWindowFrame(0, 14, 29, 19);
            Menu_PrintText(Roulette_Text_SpecialRateTable, 1, 15);
            StartTaskAfterDelayOrInput(taskId, Task_PrintMinBet, NO_DELAY, A_BUTTON | B_BUTTON);
        }
        else
        {
            StringExpandPlaceholders(gStringVar4, Roulette_Text_PlayMinimumWagerIsX);
            Menu_DrawStdWindowFrame(0, 14, 29, 19);
            Menu_PrintText(gStringVar4, 1, 15);
            gTasks[taskId].func = Task_ShowMinBetYesNo;
        }
    }
    else
    {
        StringExpandPlaceholders(gStringVar4, Roulette_Text_NotEnoughCoins);
        Menu_DrawStdWindowFrame(0, 14, 29, 19);
        Menu_PrintText(gStringVar4, 1, 15);
        gTasks[taskId].func = Task_NotEnoughForMinBet;
        gTasks[taskId].tCoins = 0;
        gTasks[taskId].data[0] = 0;
    }
}

#if DEBUG

#if (ENGLISH && REVISION == 0)
    static const u8 sText_RouletteDebugSetCoins[] = _("コインの かず STARTで きめて\nAーp1　Bーm1　R:Lー×10");
#else
    static const u8 sText_RouletteDebugSetCoins[] = _("Set　COIN　and　Press　START\nAーp1　Bーm1　R:Lー×10");
#endif

void Task_RouletteDebugSetCoins(u8 taskId)
{
#if (ENGLISH && REVISION == 0)
    u8 coinText[] = {0xFD, 0x02, 0xFF};
#endif

    if (JOY_NEW(A_BUTTON))
    {
        gTasks[taskId].tCoins++;
        if (gTasks[taskId].tCoins == 10000)
            gTasks[taskId].tCoins = 0;
        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].tCoins, 1, 4);
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
        gTasks[taskId].tCoins--;
        if (gTasks[taskId].tCoins == -1)
            gTasks[taskId].tCoins = 9999;
        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].tCoins, 1, 4);
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
        gTasks[taskId].tCoins += 10;
        if (gTasks[taskId].tCoins > 9999)
            gTasks[taskId].tCoins -= 9999;
        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].tCoins, 1, 4);
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
        gTasks[taskId].tCoins -= 10;
        if (gTasks[taskId].tCoins < 0)
            gTasks[taskId].tCoins += 9999;
        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].tCoins, 1, 4);
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
        gSaveBlock1.coins = gTasks[taskId].tCoins;
        gTasks[taskId].func = Task_PrintRouletteEntryMsg;
        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].tCoins, 1, 4);
#if (ENGLISH && REVISION == 0)
        StringExpandPlaceholders(gStringVar4, coinText);
        Menu_PrintText(gStringVar4, 2, 1);
#else
        StringExpandPlaceholders(gStringVar4, gOtherText_Coins);
        MenuPrint_RightAligned(gStringVar4, 9, 1);
#endif
        gRouletteDebugForceBallStuck = 0;
    }
    else if (JOY_NEW(SELECT_BUTTON))
    {
        gSaveBlock1.coins = gTasks[taskId].tCoins;
        gTasks[taskId].func = Task_PrintRouletteEntryMsg;
        ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].tCoins, 1, 4);
#if (ENGLISH && REVISION == 0)
        StringExpandPlaceholders(gStringVar4, coinText);
        Menu_PrintText(gStringVar4, 2, 1);
#else
        StringExpandPlaceholders(gStringVar4, gOtherText_Coins);
        MenuPrint_RightAligned(gStringVar4, 9, 1);
#endif
        gRouletteDebugForceBallStuck = 1;
    }
}

void Task_RouletteDebugInit(u8 taskId)
{
    gTasks[taskId].tCoins = gSaveBlock1.coins;
    if (Random() & 1)
        gSpecialVar_0x8004 |= 128;
    ConvertIntToDecimalStringN(gStringVar1, gTasks[taskId].tCoins, 1, 4);
    StringExpandPlaceholders(gStringVar4, gOtherText_Coins);
    Menu_DrawStdWindowFrame(0, 0, 9, 3);
#if (ENGLISH && REVISION == 0)
    Menu_PrintText(gStringVar4, 2, 1);
#else
    MenuPrint_RightAligned(gStringVar4, 9, 1);
#endif
    Menu_DrawStdWindowFrame(0, 14, 29, 19);
    Menu_PrintText(sText_RouletteDebugSetCoins, 1, 15);
    gTasks[taskId].func = Task_RouletteDebugSetCoins;
}

#endif

void PlayRoulette(void)
{
    u8 taskId;

    LockPlayerFieldControls();
#if DEBUG
    gRouletteDebugForceBallStuck = 0;
    if (gRouletteDebugActive[0] != 0)
    {
	    CreateTask(Task_RouletteDebugInit, 0);
	    return;
    }
#endif
    taskId = CreateTask(Task_PrintRouletteEntryMsg, 0);
    gTasks[taskId].tCoins = gSaveBlock1.coins;
}
