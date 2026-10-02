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

#define BALLS_PER_ROUND 6

// "Board" refers to both the wheel and the bet-selection grid.
#define NUM_BOARD_COLORS 3
#define NUM_BOARD_POKES  4
#define NUM_ROULETTE_SLOTS (NUM_BOARD_COLORS * NUM_BOARD_POKES)

#define DEGREES_PER_SLOT (360 / NUM_ROULETTE_SLOTS)
#define SLOT_MIDPOINT (DEGREES_PER_SLOT / 2 - 1)

#define SELECTION_NONE      0
#define COL_WYNAUT          1
#define COL_AZURILL         2
#define COL_SKITTY          3
#define COL_MAKUHITA        4
#define ROW_ORANGE          (COL_MAKUHITA + 1)
#define SQU_ORANGE_WYNAUT   (ROW_ORANGE + COL_WYNAUT)
#define SQU_ORANGE_AZURILL  (ROW_ORANGE + COL_AZURILL)
#define SQU_ORANGE_SKITTY   (ROW_ORANGE + COL_SKITTY)
#define SQU_ORANGE_MAKUHITA (ROW_ORANGE + COL_MAKUHITA)
#define ROW_GREEN           (SQU_ORANGE_MAKUHITA + 1)
#define SQU_GREEN_WYNAUT    (ROW_GREEN + COL_WYNAUT)
#define SQU_GREEN_AZURILL   (ROW_GREEN + COL_AZURILL)
#define SQU_GREEN_SKITTY    (ROW_GREEN + COL_SKITTY)
#define SQU_GREEN_MAKUHITA  (ROW_GREEN + COL_MAKUHITA)
#define ROW_PURPLE          (SQU_GREEN_MAKUHITA + 1)
#define SQU_PURPLE_WYNAUT   (ROW_PURPLE + COL_WYNAUT)
#define SQU_PURPLE_AZURILL  (ROW_PURPLE + COL_AZURILL)
#define SQU_PURPLE_SKITTY   (ROW_PURPLE + COL_SKITTY)
#define SQU_PURPLE_MAKUHITA (ROW_PURPLE + COL_MAKUHITA)
#define NUM_GRID_SELECTIONS SQU_PURPLE_MAKUHITA

#define GET_COL(selectionId) ((selectionId) % (NUM_BOARD_POKES + 1))
#define GET_ROW(selectionId) ((selectionId) / (NUM_BOARD_POKES + 1) * (NUM_BOARD_POKES + 1))
#define GET_COL_IDX(selectionId) ((selectionId) - 1)
#define GET_ROW_IDX(selectionId) ((selectionId) / (NUM_BOARD_POKES + 1) - 1)

#define F_WYNAUT_COL      (1 << COL_WYNAUT)
#define F_AZURILL_COL     (1 << COL_AZURILL)
#define F_SKITTY_COL      (1 << COL_SKITTY)
#define F_MAKUHITA_COL    (1 << COL_MAKUHITA)
#define F_ORANGE_ROW      (1 << ROW_ORANGE)
#define F_ORANGE_WYNAUT   (1 << SQU_ORANGE_WYNAUT)
#define F_ORANGE_AZURILL  (1 << SQU_ORANGE_AZURILL)
#define F_ORANGE_SKITTY   (1 << SQU_ORANGE_SKITTY)
#define F_ORANGE_MAKUHITA (1 << SQU_ORANGE_MAKUHITA)
#define F_GREEN_ROW       (1 << ROW_GREEN)
#define F_GREEN_WYNAUT    (1 << SQU_GREEN_WYNAUT)
#define F_GREEN_AZURILL   (1 << SQU_GREEN_AZURILL)
#define F_GREEN_SKITTY    (1 << SQU_GREEN_SKITTY)
#define F_GREEN_MAKUHITA  (1 << SQU_GREEN_MAKUHITA)
#define F_PURPLE_ROW      (1 << ROW_PURPLE)
#define F_PURPLE_WYNAUT   (1 << SQU_PURPLE_WYNAUT)
#define F_PURPLE_AZURILL  (1 << SQU_PURPLE_AZURILL)
#define F_PURPLE_SKITTY   (1 << SQU_PURPLE_SKITTY)
#define F_PURPLE_MAKUHITA (1 << SQU_PURPLE_MAKUHITA)

#define F_FLASH_COLOR_O_WYNAUT   (1 << 0)
#define F_FLASH_COLOR_G_AZURILL  (1 << 1)
#define F_FLASH_COLOR_P_SKITTY   (1 << 2)
#define F_FLASH_COLOR_O_MAKUHITA (1 << 3)
#define F_FLASH_COLOR_G_WYNAUT   (1 << 4)
#define F_FLASH_COLOR_P_AZURILL  (1 << 5)
#define F_FLASH_COLOR_O_SKITTY   (1 << 6)
#define F_FLASH_COLOR_G_MAKUHITA (1 << 7)
#define F_FLASH_COLOR_P_WYNAUT   (1 << 8)
#define F_FLASH_COLOR_O_AZURILL  (1 << 9)
#define F_FLASH_COLOR_G_SKITTY   (1 << 10)
#define F_FLASH_COLOR_P_MAKUHITA (1 << 11)
#define F_FLASH_OUTER_EDGES      (1 << 12)
#define FLASH_ICON               (NUM_ROULETTE_SLOTS + 1)
#define FLASH_ICON_2             (FLASH_ICON + 1)
#define FLASH_ICON_3             (FLASH_ICON + 2)
#define F_FLASH_ICON             (1 << FLASH_ICON)
#define F_FLASH_COLUMN           (1 << FLASH_ICON | 1 << FLASH_ICON_2 | 1 << FLASH_ICON_3)

#define MAX_MULTIPLIER 12

#define PALTAG_SHADOW       1
#define PALTAG_BALL         2
#define PALTAG_BALL_COUNTER 3
#define PALTAG_CURSOR       4
#define PALTAG_INTERFACE    5
#define PALTAG_SHROOMISH    6
#define PALTAG_TAILLOW      7
#define PALTAG_GRID_ICONS   8
#define PALTAG_WYNAUT       9
#define PALTAG_AZURILL      10
#define PALTAG_SKITTY       11
#define PALTAG_MAKUHITA     12
#define PALTAG_UNUSED_1     13
#define PALTAG_UNUSED_2     14
#define PALTAG_UNUSED_3     15
#define PALTAG_UNUSED_4     16

#define GFXTAG_WHEEL_ICONS       0
#define GFXTAG_HEADERS           4
#define GFXTAG_GRID_ICONS        5
#define GFXTAG_WHEEL_CENTER      6
#define GFXTAG_CREDIT            7
#define GFXTAG_CREDIT_DIGIT      8
#define GFXTAG_MULTIPLIER        9
#define GFXTAG_BALL_COUNTER      10
#define GFXTAG_CURSOR            11
#define GFXTAG_BALL              12
#define GFXTAG_SHROOMISH_TAILLOW 13
#define GFXTAG_SHADOW            14

enum
{
    BALL_STATE_ROLLING,
    BALL_STATE_STUCK,
    BALL_STATE_LANDED = 0xFF,
};

enum
{
    SPR_WHEEL_BALL_1,
    SPR_WHEEL_BALL_2,
    SPR_WHEEL_BALL_3,
    SPR_WHEEL_BALL_4,
    SPR_WHEEL_BALL_5,
    SPR_WHEEL_BALL_6,
    SPR_WHEEL_CENTER,
    SPR_WHEEL_ICON_ORANGE_WYNAUT,
    SPR_WHEEL_ICON_GREEN_AZURILL,
    SPR_WHEEL_ICON_PURPLE_SKITTY,
    SPR_WHEEL_ICON_ORANGE_MAKUHITA,
    SPR_WHEEL_ICON_GREEN_WYNAUT,
    SPR_WHEEL_ICON_PURPLE_AZURILL,
    SPR_WHEEL_ICON_ORANGE_SKITTY,
    SPR_WHEEL_ICON_GREEN_MAKUHITA,
    SPR_WHEEL_ICON_PURPLE_WYNAUT,
    SPR_WHEEL_ICON_ORANGE_AZURILL,
    SPR_WHEEL_ICON_GREEN_SKITTY,
    SPR_WHEEL_ICON_PURPLE_MAKUHITA,
    SPR_UNUSED_19,
    SPR_CREDIT,
    SPR_CREDIT_DIG_1,
    SPR_CREDIT_DIG_10,
    SPR_CREDIT_DIG_100,
    SPR_CREDIT_DIG_1000,
    SPR_MULTIPLIER,
    SPR_BALL_COUNTER_1,
    SPR_BALL_COUNTER_2,
    SPR_BALL_COUNTER_3,
    SPR_GRID_ICON_ORANGE_WYNAUT,
    SPR_GRID_ICON_GREEN_AZURILL,
    SPR_GRID_ICON_PURPLE_SKITTY,
    SPR_GRID_ICON_ORANGE_MAKUHITA,
    SPR_GRID_ICON_GREEN_WYNAUT,
    SPR_GRID_ICON_PURPLE_AZURILL,
    SPR_GRID_ICON_ORANGE_SKITTY,
    SPR_GRID_ICON_GREEN_MAKUHITA,
    SPR_GRID_ICON_PURPLE_WYNAUT,
    SPR_GRID_ICON_ORANGE_AZURILL,
    SPR_GRID_ICON_GREEN_SKITTY,
    SPR_GRID_ICON_PURPLE_MAKUHITA,
    SPR_POKE_HEADER_1,
    SPR_POKE_HEADER_2,
    SPR_POKE_HEADER_3,
    SPR_POKE_HEADER_4,
    SPR_COLOR_HEADER_1,
    SPR_COLOR_HEADER_2,
    SPR_COLOR_HEADER_3,
    SPR_WIN_SLOT_CURSOR,
    SPR_GRID_BALL_1,
    SPR_GRID_BALL_2,
    SPR_GRID_BALL_3,
    SPR_GRID_BALL_4,
    SPR_GRID_BALL_5,
    SPR_GRID_BALL_6,
    SPR_CLEAR_MON,
    SPR_CLEAR_MON_SHADOW_1,
    SPR_CLEAR_MON_SHADOW_2,
    SPR_UNUSED_58,
    SPR_UNUSED_59,
    SPR_UNUSED_60,
    SPR_UNUSED_61,
    SPR_UNUSED_62,
    SPR_UNUSED_63,
};

#define SPR_WHEEL_BALLS   SPR_WHEEL_BALL_1
#define SPR_WHEEL_ICONS   SPR_WHEEL_ICON_ORANGE_WYNAUT
#define SPR_BALL_COUNTER  SPR_BALL_COUNTER_1
#define SPR_CREDIT_DIGITS SPR_CREDIT_DIG_1
#define SPR_GRID_ICONS    SPR_GRID_ICON_ORANGE_WYNAUT
#define SPR_POKE_HEADERS  SPR_POKE_HEADER_1
#define SPR_COLOR_HEADERS SPR_COLOR_HEADER_1
#define SPR_GRID_BALLS    SPR_GRID_BALL_1

struct Roulette /* ewram + 0x19000 */
{
    u8 unk0;
    u8 shroomishShadowTimer;
    u8 partySpeciesFlags;
    bool8 useTaillow:5;
    bool8 ballStuck:1;
    bool8 ballUnstuck:1;
    bool8 ballRolling:1;
    u8 tableId:2;
    u8 unused:5;
    bool8 isSpecialRate:1;
    u32 hitFlags;
    u8 hitSquares[6];
    u8 pokeHits[4];
    u8 colorHits[3];
    u8 minBet;
    u8 curBallNum:4;
    u8 unk1:4;
    u8 betSelection[6];
    u8 wheelDelayTimer;
    u8 wheelSpeed;
    u8 wheelDelay;
    s16 wheelAngle;
    s16 gridX;
    s16 selectionRectDrawState;
    s16 updateGridHighlight;
    struct OamMatrix wheelRotation;
    u16 shroomishShadowAlpha;
    struct Sprite *ball;
    u8 spriteIds[0x40];
    u8 curBallSpriteId;
    u8 ballState;
    u8 hitSlot;
    u8 stuckHitSlot;
    s16 ballTravelDist;
    s16 ballTravelDistFast;
    s16 ballTravelDistMed;
    s16 ballTravelDistSlow;
    float ballAngle;
    float ballAngleSpeed;
    float ballAngleAccel;
    float ballDistToCenter;
    float ballFallSpeed;
    float ballFallAccel;
    float varA0;
    u8 playTaskId;
    u8 spinTaskId;
    u8 filler_1[2];
    u16 taskWaitDelay;
    u16 taskWaitKey;
    TaskFunc nextTask;
    u8 filler_2[4];
    TaskFunc prevTask;
    struct RouletteFlashUtil flashUtil;
}; // size: 0x17C

struct Shroomish
{
    u16 startAngle;
    u16 dropAngle;
    u16 fallSlowdown;
};

struct Taillow
{
    u16 baseDropDelay;
    u16 rightStartAngle;
    u16 leftStartAngle;
};

struct RouletteTable
{
    u8 minBet;
    u8 randDistanceHigh;
    u8 randDistanceLow;
    u8 wheelSpeed;
    u8 wheelDelay;
    struct Shroomish shroomish;
    struct Taillow taillow;
    u16 ballSpeed;
    u16 baseTravelDist;
    float var1C;
};

struct GridSelection
{
    u8 spriteIdOffset;
    u8 baseMultiplier:4;
    u8 column:4;
    u8 row;
    u8 x;
    u8 y;
    u8 var05;
    u8 tilemapOffset;
    u32 flag;
    u32 inSelectionFlags;
    u16 flashFlags;
};

struct RouletteSlot
{
    u8 id1;
    u8 id2;
    u8 gridSquare;
    u32 flag;
};

extern const struct GridSelection sGridSelections[];
extern const struct RouletteSlot sRouletteSlots[];
extern const struct RouletteTable sRouletteTables[];

void LoadOrFreeMiscSpritePalettesAndSheets(bool8 free);
u8 CreateWheelIconSprite(const struct SpriteTemplate *template, u8 radius, u16 *angle);
void CreateGridSprites(void);
void DestroyGridSprites(void);
void ShowHideGridIcons(bool8 hideAll, u8 hideSquare);
void CreateGridBallSprites(void);
void ShowHideGridBalls(bool8 hideAll, u8 hideBallId);
void ShowHideWinSlotCursor(u8 selectionId);
void CreateWheelIconSprites(void);
void SpriteCB_WheelIcon(struct Sprite *);
void CreateInterfaceSprites(void);
void SetCreditDigits(u16 num);
u8 GetMultiplierAnimId(u8 selectionId);
void SetMultiplierSprite(u8 selectionId);
void SetBallCounterNumLeft(u8 numBalls);
void SpriteCB_GridSquare(struct Sprite *);
void CreateWheelCenterSprite(void);
void SpriteCB_WheelCenter(struct Sprite *);
void CreateWheelBallSprites(void);
void HideWheelBalls(void);
void SpriteCB_RollBall_Start(struct Sprite *);

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

#define MAX_COIN_DIGITS 4

// Sprite data for the roulette ball.
#define sStuckOnWheelLeft data[0]
#define sState            data[1]
#define sSlotMidpointDist data[2]
#define sBallAngle        data[3]
#define sBallDistToCenter data[4]
#define sBallWheelAngle   data[6]

// Sprite data for Shroomish, Taillow, and their shadows.
#define sMonSpriteId        data[4]
#define sBallShadowSpriteId data[5]
#define sMonShadowSpriteId  data[6]

s16 UpdateBallRelativeWheelAngle(struct Sprite *sprite);
u8 UpdateSlotBelowBall(struct Sprite *sprite);
s16 GetBallDistanceToSlotMidpoint(struct Sprite *sprite);
void UpdateBallPos(struct Sprite *sprite);
void SpriteCB_BallLandInSlot(struct Sprite *sprite);
void SpriteCB_UnstickBall_ShroomishBallFall(struct Sprite *sprite);
void SpriteCB_UnstickBall_Shroomish(struct Sprite *sprite);
void SpriteCB_UnstickBall_TaillowDrop(struct Sprite *sprite);
void SpriteCB_UnstickBall_TaillowPickUp(struct Sprite *sprite);
void SpriteCB_UnstickBall_Taillow(struct Sprite *sprite);
void SpriteCB_UnstickBall(struct Sprite *sprite);
void SpriteCB_RollBall_TryLandAdjacent(struct Sprite *sprite);
void SpriteCB_RollBall_TryLand(struct Sprite *sprite);
void SpriteCB_RollBall_Slow(struct Sprite *sprite);
void SpriteCB_RollBall_Medium(struct Sprite *sprite);
void SpriteCB_RollBall_Fast(struct Sprite *sprite);
void CreateShroomishSprite(struct Sprite *ball);
void CreateTaillowSprite(struct Sprite *ball);
void SetBallStuck(struct Sprite *sprite);
void SpriteCB_ShroomishExit(struct Sprite *sprite);
void SpriteCB_ShroomishShakeScreen(struct Sprite *sprite);
void SpriteCB_ShroomishFall(struct Sprite *sprite);
void SpriteCB_Shroomish(struct Sprite *sprite);
void SpriteCB_TaillowShadow_Flash(struct Sprite *sprite);
void SpriteCB_Taillow_FlyAway(struct Sprite *sprite);
void SpriteCB_Taillow_PickUpBall(struct Sprite *sprite);
void SpriteCB_Taillow_FlyIn(struct Sprite *sprite);
void SpriteCB_TaillowShadow_FlyIn(struct Sprite *sprite);
void SpriteCB_Taillow(struct Sprite *sprite);

#if DEBUG
extern u8 gRouletteDebugForceBallStuck;
#endif //DEBUG

#ifdef ENGLISH
ALIGNED(4) const u16 gRouletteShadow_Pal[] = INCBIN_U16("graphics/roulette/shadow.gbapal");
const u16 gRouletteBall_Pal[] = INCBIN_U16("graphics/roulette/ball.gbapal");
const u16 gRouletteBallCounter_Pal[] = INCBIN_U16("graphics/roulette/ball_counter.gbapal");
const u16 gRouletteCursor_Pal[] = INCBIN_U16("graphics/roulette/cursor.gbapal");
const u16 gRouletteCredit_Pal[] = INCBIN_U16("graphics/roulette/credit.gbapal");
const u16 gRouletteShroomish_Pal[] = INCBIN_U16("graphics/roulette/shroomish.gbapal");
const u16 gRouletteTaillow_Pal[] = INCBIN_U16("graphics/roulette/tailow.gbapal");
const u16 gRouletteGridIcons_Pal[] = INCBIN_U16("graphics/roulette/grid_icons.gbapal");
const u16 gRouletteWynaut_Pal[] = INCBIN_U16("graphics/roulette/wynaut.gbapal");
const u16 gRouletteAzurill_Pal[] = INCBIN_U16("graphics/roulette/azurill.gbapal");
const u16 gRouletteSkitty_Pal[] = INCBIN_U16("graphics/roulette/skitty.gbapal");
const u16 gRouletteMakuhita_Pal[] = INCBIN_U16("graphics/roulette/makuhita.gbapal");
const u16 gRouletteUnused1_Pal[] = INCBIN_U16("graphics/roulette/unused_1.gbapal");
const u16 gRouletteUnused2_Pal[] = INCBIN_U16("graphics/roulette/unused_2.gbapal");
const u16 gRouletteUnused3_Pal[] = INCBIN_U16("graphics/roulette/unused_3.gbapal");
const u16 gRouletteUnused4_Pal[] = INCBIN_U16("graphics/roulette/unused_4.gbapal");
#else
extern const u16 gRouletteShadow_Pal[];
extern const u16 gRouletteBall_Pal[];
extern const u16 gRouletteBallCounter_Pal[];
extern const u16 gRouletteCursor_Pal[];
extern const u16 gRouletteCredit_Pal[];
extern const u16 gRouletteShroomish_Pal[];
extern const u16 gRouletteTaillow_Pal[];
extern const u16 gRouletteGridIcons_Pal[];
extern const u16 gRouletteWynaut_Pal[];
extern const u16 gRouletteAzurill_Pal[];
extern const u16 gRouletteSkitty_Pal[];
extern const u16 gRouletteMakuhita_Pal[];
extern const u16 gRouletteUnused1_Pal[];
extern const u16 gRouletteUnused2_Pal[];
extern const u16 gRouletteUnused3_Pal[];
extern const u16 gRouletteUnused4_Pal[];
#endif //ENGLISH

ALIGNED(4) static const u8 sBall_Gfx[] = INCBIN_U8("graphics/roulette/ball.4bpp.lz");
const u8 gRouletteBallCounter_Gfx[] = INCBIN_U8("graphics/roulette/ball_counter.4bpp.lz");
static const u8 sShroomishTaillow_Gfx[] = INCBIN_U8("graphics/roulette/roulette_tilt.4bpp.lz");
const u8 gRouletteGridIcons_Gfx[] = INCBIN_U8("graphics/roulette/grid_icons.4bpp.lz");
const u8 gRouletteWheelIcons_Gfx[] = INCBIN_U8("graphics/roulette/wheel_icons.4bpp.lz");
static const u8 sShadow_Gfx[] = INCBIN_U8("graphics/roulette/shadow.4bpp.lz");
const u8 gRouletteCursor_Gfx[] = INCBIN_U8("graphics/roulette/cursor.4bpp.lz");

static const struct SpritePalette sSpritePalettes[] = {
    {gRouletteShadow_Pal, PALTAG_SHADOW},
    {gRouletteBall_Pal, PALTAG_BALL},
    {gRouletteBallCounter_Pal, PALTAG_BALL_COUNTER},
    {gRouletteCursor_Pal, PALTAG_CURSOR},
    {gRouletteCredit_Pal, PALTAG_INTERFACE},
    {gRouletteShroomish_Pal, PALTAG_SHROOMISH},
    {gRouletteTaillow_Pal, PALTAG_TAILLOW},
    {gRouletteGridIcons_Pal, PALTAG_GRID_ICONS},
    {gRouletteWynaut_Pal, PALTAG_WYNAUT},
    {gRouletteAzurill_Pal, PALTAG_AZURILL},
    {gRouletteSkitty_Pal, PALTAG_SKITTY},
    {gRouletteMakuhita_Pal, PALTAG_MAKUHITA},
    {gRouletteUnused1_Pal, PALTAG_UNUSED_1},
    {gRouletteUnused2_Pal, PALTAG_UNUSED_2},
    {gRouletteUnused3_Pal, PALTAG_UNUSED_3},
    {gRouletteUnused4_Pal, PALTAG_UNUSED_4},
    {}
};

static const struct OamData sOam_GridHeader = {
    .shape = ST_OAM_SQUARE,
    .size = 2,
    .priority = 1
};

static const struct OamData sOam_GridIcon = {
    .shape = ST_OAM_SQUARE,
    .size = 1,
    .priority = 1
};

static const struct OamData sOam_WheelIcon = {
    .y = 60,
    .affineMode = ST_OAM_AFFINE_DOUBLE,
    .shape = ST_OAM_V_RECTANGLE,
    .size = 2,
    .priority = 2
};

static const union AnimCmd sAffineAnim_Unused1[] = {
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END
};

static const union AnimCmd *const sAffineAnims_Unused1[] = {
    sAffineAnim_Unused1
};

static const union AffineAnimCmd sAffineAnim_Unused2[] = {
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd *const sAffineAnims_Unused2[] = {
    sAffineAnim_Unused2
};

static const struct CompressedSpriteSheet sSpriteSheet_WheelIcons = {
    gRouletteWheelIcons_Gfx,
    0xc00,
    GFXTAG_WHEEL_ICONS
};

static const union AnimCmd sAnim_WheelIcons[] = {
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_FRAME(32, 0),
    ANIMCMD_FRAME(64, 0),
    ANIMCMD_FRAME(72, 0),
    ANIMCMD_FRAME(8, 0),
    ANIMCMD_FRAME(40, 0),
    ANIMCMD_FRAME(48, 0),
    ANIMCMD_FRAME(80, 0),
    ANIMCMD_FRAME(16, 0),
    ANIMCMD_FRAME(24, 0),
    ANIMCMD_FRAME(56, 0),
    ANIMCMD_FRAME(88, 0),
    ANIMCMD_END
};

static const union AnimCmd *const sAnim_WheelIcon_OrangeWynaut[] = {
    sAnim_WheelIcons +  0
};

static const union AnimCmd *const sAnim_WheelIcon_GreenAzurill[] = {
    sAnim_WheelIcons +  1
};

static const union AnimCmd *const sAnim_WheelIcon_PurpleSkitty[] = {
    sAnim_WheelIcons +  2
};

static const union AnimCmd *const sAnim_WheelIcon_OrangeMakuhita[] = {
    sAnim_WheelIcons +  3
};

static const union AnimCmd *const sAnim_WheelIcon_GreenWynaut[] = {
    sAnim_WheelIcons +  4
};

static const union AnimCmd *const sAnim_WheelIcon_PurpleAzurill[] = {
    sAnim_WheelIcons +  5
};

static const union AnimCmd *const sAnim_WheelIcon_OrangeSkitty[] = {
    sAnim_WheelIcons +  6
};

static const union AnimCmd *const sAnim_WheelIcon_GreenMakuhita[] = {
    sAnim_WheelIcons +  7
};

static const union AnimCmd *const sAnim_WheelIcon_PurpleWynaut[] = {
    sAnim_WheelIcons +  8
};

static const union AnimCmd *const sAnim_WheelIcon_OrangeAzurill[] = {
    sAnim_WheelIcons +  9
};

static const union AnimCmd *const sAnim_WheelIcon_GreenSkitty[] = {
    sAnim_WheelIcons + 10
};

static const union AnimCmd *const sAnim_WheelIcon_PurpleMakuhita[] = {
    sAnim_WheelIcons + 11
};

static const struct CompressedSpriteSheet sSpriteSheet_Headers = {
    gRouletteHeaders_Gfx,
    0x1600,
    GFXTAG_HEADERS
};

static const struct CompressedSpriteSheet sSpriteSheet_GridIcons = {
    gRouletteGridIcons_Gfx,
    0x400,
    GFXTAG_GRID_ICONS
};

static const union AnimCmd sAnim_Headers[] = {
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_FRAME(16, 0),
    ANIMCMD_FRAME(32, 0),
    ANIMCMD_FRAME(48, 0),
    ANIMCMD_FRAME(64, 0),
    ANIMCMD_FRAME(80, 0),
    ANIMCMD_FRAME(96, 0),
    ANIMCMD_FRAME(112, 0),
    ANIMCMD_FRAME(128, 0),
    ANIMCMD_FRAME(144, 0),
    ANIMCMD_FRAME(160, 0),
    ANIMCMD_END
};

static const union AnimCmd sAnim_GridIcons[] = {
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_FRAME(4, 0),
    ANIMCMD_FRAME(8, 0),
    ANIMCMD_FRAME(12, 0),
    ANIMCMD_END
};

static const union AnimCmd *const sAnim_WynautHeader[] = {
    sAnim_Headers +  0
};

static const union AnimCmd *const sAnim_AzurillHeader[] = {
    sAnim_Headers +  2
};

static const union AnimCmd *const sAnim_SkittyHeader[] = {
    sAnim_Headers +  4
};

static const union AnimCmd *const sAnim_MakuhitaHeader[] = {
    sAnim_Headers +  6
};

static const union AnimCmd *const sAnim_OrangeHeader[] = {
    sAnim_Headers +  8
};

static const union AnimCmd *const sAnim_GreenHeader[] = {
    sAnim_Headers +  9
};

static const union AnimCmd *const sAnim_PurpleHeader[] = {
    sAnim_Headers + 10
};

static const union AnimCmd *const sAnim_GridIcon_Wynaut[] = {
    sAnim_GridIcons + 0
};

static const union AnimCmd *const sAnim_GridIcon_Azurill[] = {
    sAnim_GridIcons + 1
};

static const union AnimCmd *const sAnim_GridIcon_Skitty[] = {
    sAnim_GridIcons + 2
};

static const union AnimCmd *const sAnim_GridIcon_Makuhita[] = {
    sAnim_GridIcons + 3
};

static const struct SpriteTemplate sSpriteTemplates_PokeHeaders[] = {
    {GFXTAG_HEADERS, PALTAG_GRID_ICONS, &sOam_GridHeader, sAnim_WynautHeader, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare},
    {GFXTAG_HEADERS, PALTAG_GRID_ICONS, &sOam_GridHeader, sAnim_AzurillHeader, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare},
    {GFXTAG_HEADERS, PALTAG_GRID_ICONS, &sOam_GridHeader, sAnim_SkittyHeader, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare},
    {GFXTAG_HEADERS, PALTAG_GRID_ICONS, &sOam_GridHeader, sAnim_MakuhitaHeader, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare}
};

static const struct SpriteTemplate sSpriteTemplates_ColorHeaders[] = {
    {GFXTAG_HEADERS, PALTAG_GRID_ICONS, &sOam_GridHeader, sAnim_OrangeHeader, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare},
    {GFXTAG_HEADERS, PALTAG_GRID_ICONS, &sOam_GridHeader, sAnim_GreenHeader, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare},
    {GFXTAG_HEADERS, PALTAG_GRID_ICONS, &sOam_GridHeader, sAnim_PurpleHeader, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare}
};

static const struct SpriteTemplate sSpriteTemplates_GridIcons[] ={
    {GFXTAG_GRID_ICONS, PALTAG_GRID_ICONS, &sOam_GridIcon, sAnim_GridIcon_Wynaut, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare},
    {GFXTAG_GRID_ICONS, PALTAG_GRID_ICONS, &sOam_GridIcon, sAnim_GridIcon_Azurill, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare},
    {GFXTAG_GRID_ICONS, PALTAG_GRID_ICONS, &sOam_GridIcon, sAnim_GridIcon_Skitty, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare},
    {GFXTAG_GRID_ICONS, PALTAG_GRID_ICONS, &sOam_GridIcon, sAnim_GridIcon_Makuhita, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare}
};

static const struct SpriteTemplate sSpriteTemplates_WheelIcons[] = {
    {GFXTAG_WHEEL_ICONS, PALTAG_WYNAUT, &sOam_WheelIcon, sAnim_WheelIcon_OrangeWynaut, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {GFXTAG_WHEEL_ICONS, PALTAG_AZURILL, &sOam_WheelIcon, sAnim_WheelIcon_GreenAzurill, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {GFXTAG_WHEEL_ICONS, PALTAG_SKITTY, &sOam_WheelIcon, sAnim_WheelIcon_PurpleSkitty, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {GFXTAG_WHEEL_ICONS, PALTAG_MAKUHITA, &sOam_WheelIcon, sAnim_WheelIcon_OrangeMakuhita, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {GFXTAG_WHEEL_ICONS, PALTAG_WYNAUT, &sOam_WheelIcon, sAnim_WheelIcon_GreenWynaut, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {GFXTAG_WHEEL_ICONS, PALTAG_AZURILL, &sOam_WheelIcon, sAnim_WheelIcon_PurpleAzurill, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {GFXTAG_WHEEL_ICONS, PALTAG_SKITTY, &sOam_WheelIcon, sAnim_WheelIcon_OrangeSkitty, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {GFXTAG_WHEEL_ICONS, PALTAG_MAKUHITA, &sOam_WheelIcon, sAnim_WheelIcon_GreenMakuhita, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {GFXTAG_WHEEL_ICONS, PALTAG_WYNAUT, &sOam_WheelIcon, sAnim_WheelIcon_PurpleWynaut, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {GFXTAG_WHEEL_ICONS, PALTAG_AZURILL, &sOam_WheelIcon, sAnim_WheelIcon_OrangeAzurill, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {GFXTAG_WHEEL_ICONS, PALTAG_SKITTY, &sOam_WheelIcon, sAnim_WheelIcon_GreenSkitty, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {GFXTAG_WHEEL_ICONS, PALTAG_MAKUHITA, &sOam_WheelIcon, sAnim_WheelIcon_PurpleMakuhita, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon}
};

static const struct OamData sOam_Credit = {
    .shape = ST_OAM_H_RECTANGLE,
    .size = 3,
    .priority = 1
};

static const struct OamData sOam_CreditDigit = {
    .shape = ST_OAM_V_RECTANGLE,
    .size = 0,
    .priority = 1
};

static const struct OamData sOam_Multiplier = {
    .shape = ST_OAM_H_RECTANGLE,
    .size = 2,
    .priority = 1
};

static const struct OamData sOam_BallCounter = {
    .shape = ST_OAM_H_RECTANGLE,
    .size = 0,
    .priority = 1
};

static const struct SpriteSheet sSpriteSheets_Interface[] = {
    {gRouletteCredit_Gfx, 0x400, GFXTAG_CREDIT},
    {gRouletteNumbers_Gfx, 0x280, GFXTAG_CREDIT_DIGIT},
    {gRouletteMultiplier_Gfx, 0x500, GFXTAG_MULTIPLIER},
    {gRouletteBallCounter_Gfx, 0x140, GFXTAG_BALL_COUNTER},
    {gRouletteCursor_Gfx, 0x200, GFXTAG_CURSOR},
    {}
};

static const union AnimCmd sAnim_CreditDigit[] = {
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_FRAME(2, 0),
    ANIMCMD_FRAME(4, 0),
    ANIMCMD_FRAME(6, 0),
    ANIMCMD_FRAME(8, 0),
    ANIMCMD_FRAME(10, 0),
    ANIMCMD_FRAME(12, 0),
    ANIMCMD_FRAME(14, 0),
    ANIMCMD_FRAME(16, 0),
    ANIMCMD_FRAME(18, 0),
    // ANIMCMD_END
};

static const union AnimCmd *const sAnims_CreditDigit[] = {
    sAnim_CreditDigit
};

static const union AnimCmd sAnim_Multiplier[] = {
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_FRAME(8, 0),
    ANIMCMD_FRAME(16, 0),
    ANIMCMD_FRAME(24, 0),
    ANIMCMD_FRAME(32, 0),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_Multiplier[] = {
    sAnim_Multiplier
};

static const union AnimCmd sAnim_BallCounter[] = {
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_FRAME(2, 0),
    ANIMCMD_FRAME(4, 0),
    ANIMCMD_FRAME(6, 0),
    ANIMCMD_FRAME(8, 0),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_BallCounter[] = {
    sAnim_BallCounter
};

static const struct SpriteTemplate sSpriteTemplate_Credit = {GFXTAG_CREDIT, PALTAG_INTERFACE, &sOam_Credit, gDummySpriteAnimTable, NULL, gDummySpriteAffineAnimTable, SpriteCallbackDummy};

static const struct SpriteTemplate sSpriteTemplate_CreditDigit = {GFXTAG_CREDIT_DIGIT, PALTAG_INTERFACE, &sOam_CreditDigit, sAnims_CreditDigit, NULL, gDummySpriteAffineAnimTable, SpriteCallbackDummy};

static const struct SpriteTemplate sSpriteTemplate_Multiplier = {GFXTAG_MULTIPLIER, PALTAG_INTERFACE, &sOam_Multiplier, sAnims_Multiplier, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare};

static const struct SpriteTemplate sSpriteTemplate_BallCounter = {GFXTAG_BALL_COUNTER, PALTAG_BALL_COUNTER, &sOam_BallCounter, sAnims_BallCounter, NULL, gDummySpriteAffineAnimTable, SpriteCallbackDummy};

static const struct SpriteTemplate sSpriteTemplate_Cursor = {GFXTAG_CURSOR, PALTAG_INTERFACE, &sOam_GridHeader, gDummySpriteAnimTable, NULL, gDummySpriteAffineAnimTable, SpriteCallbackDummy};

static const struct OamData sOam_Ball = {
    .shape = ST_OAM_SQUARE,
    .size = 1,
    .priority = 2
};

static const struct SpriteFrameImage sBallImages[] = {
    {sRouletteBallGfxBuffer + 0, 128},
    {sRouletteBallGfxBuffer + 128, 128},
    {sRouletteBallGfxBuffer + 0x100, 128},
    {sRouletteBallGfxBuffer + 0x180, 128}
};

static const union AnimCmd sAnim_Ball_RollFast[] = {
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_FRAME(1, 5),
    ANIMCMD_FRAME(2, 5),
    ANIMCMD_FRAME(1, 5),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_Ball_RollMedium[] = {
    ANIMCMD_FRAME(0, 10),
    ANIMCMD_FRAME(1, 10),
    ANIMCMD_FRAME(2, 10),
    ANIMCMD_FRAME(1, 10),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_Ball_RollSlow[] = {
    ANIMCMD_FRAME(0, 15),
    ANIMCMD_FRAME(1, 15),
    ANIMCMD_FRAME(2, 15),
    ANIMCMD_FRAME(1, 15),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_Ball_StopOnFrame1[] = {
    ANIMCMD_FRAME(1, 2),
    ANIMCMD_FRAME(2, 5),
    ANIMCMD_FRAME(1, 5),
    ANIMCMD_FRAME(3, 5),
    ANIMCMD_END
};

static const union AnimCmd sAnim_Ball_StopOnFrame3[] = {
    ANIMCMD_FRAME(1, 2),
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_FRAME(2, 4),
    ANIMCMD_FRAME(3, 4),
    ANIMCMD_END
};

static const union AnimCmd sAnim_Ball_StopOnFrame4[] = {
    ANIMCMD_FRAME(0, 2),
    ANIMCMD_FRAME(1, 5),
    ANIMCMD_FRAME(2, 5),
    ANIMCMD_FRAME(3, 5),
    ANIMCMD_END
};

static const union AnimCmd sAnim_Ball_Still[] = {
    ANIMCMD_FRAME(3, 0),
    ANIMCMD_END
};

static const union AnimCmd sAnim_Ball_StopOnFrame2[] = {
    ANIMCMD_FRAME(2, 2),
    ANIMCMD_FRAME(1, 5),
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_FRAME(3, 5),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_Ball[] = {
    sAnim_Ball_RollFast,
    sAnim_Ball_RollMedium,
    sAnim_Ball_RollSlow,
    sAnim_Ball_StopOnFrame1,
    sAnim_Ball_StopOnFrame2,
    sAnim_Ball_StopOnFrame3,
    sAnim_Ball_StopOnFrame4,
    sAnim_Ball_StopOnFrame4,
    sAnim_Ball_Still
};

static const struct SpriteTemplate sSpriteTemplate_Ball = {0xffff, PALTAG_BALL, &sOam_Ball, sAnims_Ball, sBallImages, gDummySpriteAffineAnimTable, SpriteCallbackDummy};

static const struct OamData sOam_WheelCenter = {
    .y = 81,
    .affineMode = ST_OAM_AFFINE_DOUBLE,
    .shape = ST_OAM_SQUARE,
    .size = 3,
    .priority = 2
};

static const struct CompressedSpriteSheet sSpriteSheet_WheelCenter = {
    gRouletteCenter_Gfx,
    0x800,
    GFXTAG_WHEEL_CENTER
};

static const struct SpriteTemplate sSpriteTemplate_WheelCenter = {GFXTAG_WHEEL_CENTER, PALTAG_BALL, &sOam_WheelCenter, gDummySpriteAnimTable, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelCenter};

static const struct OamData sOam_Shroomish = {
    .shape = ST_OAM_SQUARE,
    .size = 2,
    .priority = 2
};

static const struct OamData sOam_Taillow = {
    .shape = ST_OAM_SQUARE,
    .size = 2,
    .priority = 2
};

static const struct SpriteFrameImage sShroomishTaillowImages[] = {
    {sRouletteShroomishTaillowGfxBuffer + 0, 0x200},
    {sRouletteShroomishTaillowGfxBuffer + 0x200, 0x200},
    {sRouletteShroomishTaillowGfxBuffer + 0x400, 0x200},
    {sRouletteShroomishTaillowGfxBuffer + 0x600, 0x200},
    {sRouletteShroomishTaillowGfxBuffer + 0x800, 0x200},
    {sRouletteShroomishTaillowGfxBuffer + 0xa00, 0x200},
    {sRouletteShroomishTaillowGfxBuffer + 0xc00, 0x200}
};

static const union AnimCmd sAnim_Shroomish[] = {
    ANIMCMD_FRAME(0, 6),
    ANIMCMD_FRAME(1, 6),
    // jump here
    ANIMCMD_FRAME(2, 6),
    ANIMCMD_FRAME(3, 6),
    ANIMCMD_FRAME(2, 6),
    ANIMCMD_FRAME(4, 6),
    ANIMCMD_JUMP(2)
};

static const union AnimCmd sAnim_Taillow_WingDown_Left[] = {
    ANIMCMD_FRAME(5, 10),
    ANIMCMD_END
};

static const union AnimCmd sAnim_Taillow_WingDown_Right[] = {
    ANIMCMD_FRAME(5, 10, .hFlip = TRUE),
    ANIMCMD_END
};

static const union AnimCmd sAnim_Taillow_FlapSlow_Left[] = {
    ANIMCMD_FRAME(5, 20),
    ANIMCMD_FRAME(6, 20),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_Taillow_FlapSlow_Right[] = {
    ANIMCMD_FRAME(5, 20, .hFlip = TRUE),
    ANIMCMD_FRAME(6, 20, .hFlip = TRUE),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_Taillow_FlapFast_Left[] = {
    ANIMCMD_FRAME(5, 10),
    ANIMCMD_FRAME(6, 10),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd sAnim_Taillow_FlapFast_Right[] = {
    ANIMCMD_FRAME(5, 10, .hFlip = TRUE),
    ANIMCMD_FRAME(6, 10, .hFlip = TRUE),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd *const sAnims_Shroomish[] = {
    sAnim_Shroomish
};

static const union AnimCmd *const sAnims_Taillow[] = {
    sAnim_Taillow_WingDown_Left,
    sAnim_Taillow_WingDown_Right,
    sAnim_Taillow_FlapSlow_Left,
    sAnim_Taillow_FlapSlow_Right,
    sAnim_Taillow_FlapFast_Left,
    sAnim_Taillow_FlapFast_Right
};

static const struct SpriteTemplate sSpriteTemplate_Shroomish = {0xffff, PALTAG_SHROOMISH, &sOam_Shroomish, sAnims_Shroomish, sShroomishTaillowImages, gDummySpriteAffineAnimTable, SpriteCallbackDummy};

static const struct SpriteTemplate sSpriteTemplate_Taillow = {0xffff, PALTAG_TAILLOW, &sOam_Taillow, sAnims_Taillow, sShroomishTaillowImages, gDummySpriteAffineAnimTable, SpriteCB_Taillow};

static const struct OamData sOam_ShroomishBallShadow = {
    .affineMode = ST_OAM_AFFINE_OFF,
    .shape = ST_OAM_SQUARE,
    .size = 1,
    .priority = 2
};

static const struct OamData sOam_ShroomishShadow = {
    .affineMode = ST_OAM_AFFINE_OFF,
    .shape = ST_OAM_H_RECTANGLE,
    .size = 2,
    .priority = 2
};

static const struct OamData sOam_TaillowShadow = {
    .affineMode = ST_OAM_AFFINE_NORMAL,
    .shape = ST_OAM_H_RECTANGLE,
    .size = 2,
    .priority = 2
};

static const struct SpriteFrameImage sShroomishBallShadowImages[] = {
    {(void *)ewram18000 + 0, 128}
};

static const struct SpriteFrameImage sUnstickMonShadowImages[] = {
    {(void *)ewram18000 + 128, 0x100}
};

static const union AffineAnimCmd sAffineAnim_Unused3[] = {
    AFFINEANIMCMD_FRAME(128, 128, 0,  0),
    AFFINEANIMCMD_FRAME(  2,   2, 0, 60),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_TaillowShadow[] = {
    AFFINEANIMCMD_FRAME(256, 256, 0,  0),
    AFFINEANIMCMD_FRAME( -2,   0, 0, 15),
    AFFINEANIMCMD_FRAME( -1,  -2, 0, 15),
    AFFINEANIMCMD_FRAME( -1,  -5, 0, 24),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd *const sAffineAnims_Unused3[] = {
    sAffineAnim_Unused3
};

static const union AffineAnimCmd *const sAffineAnims_TaillowShadow[] = {
    sAffineAnim_TaillowShadow
};

static const union AffineAnimCmd sAffineAnim_Unused4[] = {
    AFFINEANIMCMD_FRAME(256, 256, 0, 0),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd *const sAffineAnims_Unused4[] = {
    sAffineAnim_Unused4
};

static const struct SpriteTemplate sSpriteTemplate_ShroomishShadow[] = {
    {0xffff, PALTAG_SHADOW, &sOam_ShroomishBallShadow, sAffineAnims_Unused1, sShroomishBallShadowImages, gDummySpriteAffineAnimTable, SpriteCallbackDummy},
    {0xffff, PALTAG_SHADOW, &sOam_ShroomishShadow, sAffineAnims_Unused1, sUnstickMonShadowImages, gDummySpriteAffineAnimTable, SpriteCB_Shroomish}
};

static const struct SpriteTemplate sSpriteTemplate_TaillowShadow = {0xffff, PALTAG_SHADOW, &sOam_TaillowShadow, sAffineAnims_Unused1, sUnstickMonShadowImages, sAffineAnims_TaillowShadow, SpriteCB_Taillow};

void LoadOrFreeMiscSpritePalettesAndSheets(bool8 free)
{
    if (!free)
    {
        FreeAllSpritePalettes();
        LoadSpritePalettes(sSpritePalettes);
        LZ77UnCompWram(sShroomishTaillow_Gfx, sRouletteShroomishTaillowGfxBuffer);
        LZ77UnCompWram(sBall_Gfx, sRouletteBallGfxBuffer);
        LZ77UnCompWram(sShadow_Gfx, ewram18000);
    }
    else
    {
        FreeAllSpritePalettes();
    }
}

u8 CreateWheelIconSprite(const struct SpriteTemplate *template, u8 radius, u16 *angle)
{
    u16 temp;
    u8 spriteId = CreateSprite(template, 116, 80, template->oam->y);
    gSprites[spriteId].data[0]            = *angle;
    gSprites[spriteId].data[1]            = radius;
    gSprites[spriteId].coordOffsetEnabled = TRUE;
    gSprites[spriteId].animPaused         = TRUE;
    gSprites[spriteId].affineAnimPaused   = TRUE;
    temp = *angle;
    *angle += DEGREES_PER_SLOT;
    if (*angle >= 360)
        *angle = temp - (360 - DEGREES_PER_SLOT);
    return spriteId;
}

void CreateGridSprites(void)
{
    u8 i, j;
    u8 spriteId;
    struct SpriteSheet s;
    LZ77UnCompWram(sSpriteSheet_Headers.data, gSharedMem);
    s.data = gSharedMem;
    s.size = sSpriteSheet_Headers.size;
    s.tag  = sSpriteSheet_Headers.tag;
    LoadSpriteSheet(&s);
    LZ77UnCompWram(sSpriteSheet_GridIcons.data, gSharedMem);
    s.data = gSharedMem;
    s.size = sSpriteSheet_GridIcons.size;
    s.tag  = sSpriteSheet_GridIcons.tag;
    LoadSpriteSheet(&s);
    for (i = 0; i < NUM_BOARD_COLORS; i++)
    {
        u8 y = i * 24;
        for (j = 0; j < NUM_BOARD_POKES; j++)
        {
            spriteId = sRoulette->spriteIds[(i * NUM_BOARD_POKES) + SPR_GRID_ICONS + j] =
                CreateSprite(&sSpriteTemplates_GridIcons[j], j * 24 + 148, y + 92, 30);
            gSprites[spriteId].animPaused = TRUE;
            y += 24;
            if (y > 71)
                y = 0;
        }
    }
    for (i = 0; i < NUM_BOARD_POKES; i++)
    {
        spriteId = sRoulette->spriteIds[SPR_POKE_HEADERS + i] =
            CreateSprite(&sSpriteTemplates_PokeHeaders[i], (s16)(i * 24 + 148), 70, 30);
        gSprites[spriteId].animPaused = TRUE;
    }
    for (i = 0; i < NUM_BOARD_COLORS; i++)
    {
        spriteId = sRoulette->spriteIds[SPR_COLOR_HEADERS + i] =
            CreateSprite(&sSpriteTemplates_ColorHeaders[i], 126, (s16)(i * 24 + 92), 30);
        gSprites[spriteId].animPaused = TRUE;
    }
}

void DestroyGridSprites(void)
{
    u8 i;
    for (i = 0; i < NUM_ROULETTE_SLOTS; i++)
        DestroySprite(&gSprites[sRoulette->spriteIds[SPR_GRID_ICONS + i]]);
}

void ShowHideGridIcons(bool8 hideAll, u8 hideSquare)
{
    u8 i;
    switch (hideAll)
    {
    case TRUE:
        for (i = 0; i < NUM_GRID_SELECTIONS; i++)
            gSprites[sRoulette->spriteIds[SPR_GRID_ICONS + i]].invisible = TRUE;
        break;
    case FALSE:
        for (i = 0; i < NUM_ROULETTE_SLOTS; i++)
        {
            if (!(sRoulette->hitFlags & sRouletteSlots[i].flag))
                gSprites[sRoulette->spriteIds[SPR_GRID_ICONS + i]].invisible = FALSE;
            else if (sRouletteSlots[i].gridSquare != hideSquare)
                gSprites[sRoulette->spriteIds[SPR_GRID_ICONS + i]].invisible = TRUE;
            else
                gSprites[sRoulette->spriteIds[SPR_GRID_ICONS + i]].invisible = FALSE;
        }
        for (; i < NUM_GRID_SELECTIONS; i++)
            gSprites[sRoulette->spriteIds[SPR_GRID_ICONS + i]].invisible = FALSE;
        break;
    }
}

void CreateGridBallSprites(void)
{
    u8 i;
    for (i = 0; i < BALLS_PER_ROUND; i++)
    {
        sRoulette->spriteIds[SPR_GRID_BALLS + i] = CreateSprite(&sSpriteTemplate_Ball, 116, 20, 10);
        gSprites[sRoulette->spriteIds[SPR_GRID_BALLS + i]].invisible    = TRUE;
        gSprites[sRoulette->spriteIds[SPR_GRID_BALLS + i]].data[0]      = 1;
        gSprites[sRoulette->spriteIds[SPR_GRID_BALLS + i]].callback     = SpriteCB_GridSquare;
        gSprites[sRoulette->spriteIds[SPR_GRID_BALLS + i]].oam.priority = 1;
        StartSpriteAnim(&gSprites[sRoulette->spriteIds[SPR_GRID_BALLS + i]], 8);
    }
}

void ShowHideGridBalls(bool8 hideAll, u8 hideBallId)
{
    u8 i = 0;
    if (hideAll)
    {
        for (; i < BALLS_PER_ROUND; i++)
            gSprites[sRoulette->spriteIds[SPR_GRID_BALLS + i]].invisible = TRUE;
    }
    else
    {
        for (; i < BALLS_PER_ROUND; i++)
        {
            if (!sRoulette->hitSquares[i] || i == hideBallId)
            {
                gSprites[sRoulette->spriteIds[SPR_GRID_BALLS + i]].invisible = TRUE;
            }
            else
            {
                gSprites[sRoulette->spriteIds[SPR_GRID_BALLS + i]].invisible = FALSE;
                gSprites[sRoulette->spriteIds[SPR_GRID_BALLS + i]].x = (sGridSelections[sRoulette->hitSquares[i]].x + 1) * 8 + 4;
                gSprites[sRoulette->spriteIds[SPR_GRID_BALLS + i]].y = (sGridSelections[sRoulette->hitSquares[i]].y + 1) * 8 + 3;
            }
        }
    }
}

void ShowHideWinSlotCursor(u8 selectionId)
{
    if (selectionId == SELECTION_NONE)
    {
        gSprites[sRoulette->spriteIds[SPR_WIN_SLOT_CURSOR]].invisible = TRUE;
    }
    else
    {
        gSprites[sRoulette->spriteIds[SPR_WIN_SLOT_CURSOR]].invisible = FALSE;
        gSprites[sRoulette->spriteIds[SPR_WIN_SLOT_CURSOR]].x = (sGridSelections[selectionId].x + 2) * 8;
        gSprites[sRoulette->spriteIds[SPR_WIN_SLOT_CURSOR]].y = (sGridSelections[selectionId].y + 2) * 8;
    }
}

void CreateWheelIconSprites(void)
{
    u8 i, j;
    u16 angle;
    struct SpriteSheet s;
    LZ77UnCompWram(sSpriteSheet_WheelIcons.data, gSharedMem);
    s.data = gSharedMem;
    s.size = sSpriteSheet_WheelIcons.size;
    s.tag  = sSpriteSheet_WheelIcons.tag;
    LoadSpriteSheet(&s);
    angle = 15;
    for (i = 0; i < NUM_BOARD_COLORS; i++)
    {
        for (j = 0; j < NUM_BOARD_POKES; j++)
        {
            u8 spriteId;
            spriteId = sRoulette->spriteIds[(i * NUM_BOARD_POKES) + SPR_WHEEL_ICONS + j] =
                CreateWheelIconSprite(&sSpriteTemplates_WheelIcons[i * NUM_BOARD_POKES + j], 40, &angle);
            gSprites[spriteId].animPaused       = TRUE;
            gSprites[spriteId].affineAnimPaused = TRUE;
        }
    }
}

void SpriteCB_WheelIcon(struct Sprite *sprite)
{
    s16 cos;
    s16 sin;
    u32 matrixNum;
    s16 angle = sRoulette->wheelAngle + sprite->data[0];
    if (angle >= 360)
        angle -= 360;
    sin = Sin2(angle);
    cos = Cos2(angle);
    sprite->x2 =  sin * sprite->data[1] >> 12;
    sprite->y2 = -cos * sprite->data[1] >> 12;
    matrixNum = sprite->oam.matrixNum;
    sin /= 16;
    gOamMatrices[matrixNum].d = cos /= 16;
    gOamMatrices[matrixNum].a = cos;
    gOamMatrices[matrixNum].b = sin;
    gOamMatrices[matrixNum].c = -sin;
}

void CreateInterfaceSprites(void)
{
    u8 i;
    for (i = 0; i < 5; i++)
    {
        struct SpriteSheet s;
        LZ77UnCompWram(sSpriteSheets_Interface[i].data, gSharedMem);
        s.data = gSharedMem;
        s.size = sSpriteSheets_Interface[i].size;
        s.tag  = sSpriteSheets_Interface[i].tag;
        LoadSpriteSheet(&s);
    }
    sRoulette->spriteIds[SPR_CREDIT] = CreateSprite(&sSpriteTemplate_Credit, 208, 16, 4);
    gSprites[sRoulette->spriteIds[SPR_CREDIT]].animPaused = TRUE;
    for (i = 0; i < MAX_COIN_DIGITS; i++)
    {
        sRoulette->spriteIds[SPR_CREDIT_DIGITS + i] = CreateSprite(&sSpriteTemplate_CreditDigit, (s16)(i * 8 + 196), 24, 0);
        gSprites[sRoulette->spriteIds[SPR_CREDIT_DIGITS + i]].invisible = TRUE;
        gSprites[sRoulette->spriteIds[SPR_CREDIT_DIGITS + i]].animPaused = TRUE;
    }
    sRoulette->spriteIds[SPR_MULTIPLIER] = CreateSprite(&sSpriteTemplate_Multiplier, 120, 68, 4);
    gSprites[sRoulette->spriteIds[SPR_MULTIPLIER]].animPaused = TRUE;
    for (i = 0; i < BALLS_PER_ROUND / 2; i++)
    {
        sRoulette->spriteIds[SPR_BALL_COUNTER + i] = CreateSprite(&sSpriteTemplate_BallCounter, (s16)(i * 16 + 192), 36, 4);
        gSprites[sRoulette->spriteIds[SPR_BALL_COUNTER + i]].invisible = TRUE;
        gSprites[sRoulette->spriteIds[SPR_BALL_COUNTER + i]].animPaused = TRUE;
    }
    sRoulette->spriteIds[SPR_WIN_SLOT_CURSOR] = CreateSprite(&sSpriteTemplate_Cursor, 152, 96, 9);
    gSprites[sRoulette->spriteIds[SPR_WIN_SLOT_CURSOR]].oam.priority = 1;
    gSprites[sRoulette->spriteIds[SPR_WIN_SLOT_CURSOR]].animPaused = TRUE;
    gSprites[sRoulette->spriteIds[SPR_WIN_SLOT_CURSOR]].invisible = TRUE;
}

void SetCreditDigits(u16 num)
{
    u8 i;
    u16 d = 1000;
    bool8 printZero = FALSE;
    for (i = 0; i < MAX_COIN_DIGITS; i++)
    {
        u8 digit = num / d;
        gSprites[sRoulette->spriteIds[SPR_CREDIT_DIGITS + i]].invisible = TRUE;
        if (digit > 0 || printZero || i == MAX_COIN_DIGITS - 1)
        {
            gSprites[sRoulette->spriteIds[SPR_CREDIT_DIGITS + i]].invisible = FALSE;
            gSprites[sRoulette->spriteIds[SPR_CREDIT_DIGITS + i]].oam.tileNum =
                gSprites[sRoulette->spriteIds[SPR_CREDIT_DIGITS + i]].sheetTileStart
                + (*gSprites[sRoulette->spriteIds[SPR_CREDIT_DIGITS + i]].anims + digit)->type;
            printZero = TRUE;
        }
        num = num % d;
        d = d / 10;
    }
}

u8 GetMultiplierAnimId(u8 selectionId)
{
    u8 animIds[] = {0, 1, 2, 3, 4};
    if (selectionId > NUM_GRID_SELECTIONS)
        selectionId = SELECTION_NONE;
    switch (sGridSelections[selectionId].baseMultiplier)
    {
    case NUM_BOARD_COLORS:
        selectionId = GET_ROW_IDX(selectionId);
        if (sRoulette->colorHits[selectionId] < NUM_BOARD_POKES)
            return animIds[((vu8 *)sRoulette->colorHits)[selectionId] + 1]; // couldn't recreate redundant loads
        break;
    case NUM_BOARD_POKES:
        selectionId = GET_COL_IDX(selectionId);
        if (sRoulette->pokeHits[selectionId] < NUM_BOARD_COLORS)
            return animIds[((vu8 *)sRoulette->pokeHits)[selectionId] + 2];
        break;
    case NUM_ROULETTE_SLOTS:
        if (!(sRoulette->hitFlags & sGridSelections[selectionId].flag))
            return animIds[4];
        break;
    }
    return 0;
}

void SetMultiplierSprite(u8 selectionId)
{
    struct Sprite *sprite = &gSprites[sRoulette->spriteIds[SPR_MULTIPLIER]];
    sprite->animCmdIndex = GetMultiplierAnimId(selectionId);
    sprite->oam.tileNum =
        sprite->sheetTileStart
        + (*sprite->anims + sprite->animCmdIndex)->type;
}

void SetBallCounterNumLeft(u8 numBalls)
{
    u8 i;
    u8 animOffset = 0;
    if (sRoulette->minBet == 1)
        animOffset = 2;
    switch (numBalls)
    {
    case BALLS_PER_ROUND:
        for (i = 0; i < BALLS_PER_ROUND / 2; i++)
        {
            gSprites[sRoulette->spriteIds[SPR_BALL_COUNTER + i]].invisible = FALSE;
            gSprites[sRoulette->spriteIds[SPR_BALL_COUNTER + i]].oam.tileNum =
                gSprites[sRoulette->spriteIds[SPR_BALL_COUNTER + i]].sheetTileStart
                + (*gSprites[sRoulette->spriteIds[SPR_BALL_COUNTER + i]].anims)->type;
        }
        break;
    case 5:
        gSprites[sRoulette->spriteIds[SPR_BALL_COUNTER_3]].oam.tileNum =
            gSprites[sRoulette->spriteIds[SPR_BALL_COUNTER_3]].sheetTileStart
            + (*gSprites[sRoulette->spriteIds[SPR_BALL_COUNTER_3]].anims + animOffset + 1)->type;
        break;
    case 4:
        gSprites[sRoulette->spriteIds[SPR_BALL_COUNTER_3]].oam.tileNum =
            gSprites[sRoulette->spriteIds[SPR_BALL_COUNTER_3]].sheetTileStart
            + (*gSprites[sRoulette->spriteIds[SPR_BALL_COUNTER_3]].anims + animOffset + 2)->type;
        break;
    case 3:
        gSprites[sRoulette->spriteIds[SPR_BALL_COUNTER_2]].oam.tileNum =
            gSprites[sRoulette->spriteIds[SPR_BALL_COUNTER_2]].sheetTileStart
            + (*gSprites[sRoulette->spriteIds[SPR_BALL_COUNTER_2]].anims + animOffset + 1)->type;
        break;
    case 2:
        gSprites[sRoulette->spriteIds[SPR_BALL_COUNTER_2]].oam.tileNum =
            gSprites[sRoulette->spriteIds[SPR_BALL_COUNTER_2]].sheetTileStart
            + (*gSprites[sRoulette->spriteIds[SPR_BALL_COUNTER_2]].anims + animOffset + 2)->type;
        break;
    case 1:
        gSprites[sRoulette->spriteIds[SPR_BALL_COUNTER_1]].oam.tileNum =
            gSprites[sRoulette->spriteIds[SPR_BALL_COUNTER_1]].sheetTileStart
            + (*gSprites[sRoulette->spriteIds[SPR_BALL_COUNTER_1]].anims + animOffset + 1)->type;
        break;
    case 0:
    default:
        for (i = 0; i < BALLS_PER_ROUND / 2; i++)
        {
            gSprites[sRoulette->spriteIds[SPR_BALL_COUNTER + i]].oam.tileNum =
                gSprites[sRoulette->spriteIds[SPR_BALL_COUNTER + i]].sheetTileStart
                + (*gSprites[sRoulette->spriteIds[SPR_BALL_COUNTER + i]].anims + animOffset + 2)->type;
        }
    }
}

void SpriteCB_GridSquare(struct Sprite *sprite)
{
    sprite->x2 = sRoulette->gridX;
}

void CreateWheelCenterSprite(void)
{
    u8 spriteId;
    struct SpriteSheet s;
    LZ77UnCompWram(sSpriteSheet_WheelCenter.data, gSharedMem);
    s.data = gSharedMem;
    s.size = sSpriteSheet_WheelCenter.size;
    s.tag  = sSpriteSheet_WheelCenter.tag;
    LoadSpriteSheet(&s);
    spriteId = CreateSprite(&sSpriteTemplate_WheelCenter, 116, 80, 81);
    gSprites[spriteId].data[0]            = sRoulette->wheelAngle;
    gSprites[spriteId].data[1]            = 0;
    gSprites[spriteId].animPaused         = TRUE;
    gSprites[spriteId].affineAnimPaused   = TRUE;
    gSprites[spriteId].coordOffsetEnabled = TRUE;
}

void SpriteCB_WheelCenter(struct Sprite *sprite)
{
    u32 matrixNum = sprite->oam.matrixNum;
    struct OamMatrix *matrix = gOamMatrices;
    matrix[matrixNum].d = sRoulette->wheelRotation.a;
    matrix[matrixNum].a = sRoulette->wheelRotation.a;
    matrix[matrixNum].b = sRoulette->wheelRotation.b;
    matrix[matrixNum].c = sRoulette->wheelRotation.c;
}

void CreateWheelBallSprites(void)
{
    u8 i;
    for (i = 0; i < BALLS_PER_ROUND; i++)
    {
        u8 spriteId =
            sRoulette->spriteIds[i] = CreateSprite(&sSpriteTemplate_Ball, 116, 80, 57 - i);
        if (spriteId != MAX_SPRITES)
        {
            gSprites[sRoulette->spriteIds[i]].invisible          = TRUE;
            gSprites[sRoulette->spriteIds[i]].coordOffsetEnabled = TRUE;
        }
    }
}

void HideWheelBalls(void)
{
    u8 spriteId = sRoulette->spriteIds[SPR_WHEEL_BALLS];
    u8 i;
    for (i = 0; i < BALLS_PER_ROUND; i++)
    {
        u8 j;
        gSprites[spriteId].invisible = TRUE;
        gSprites[spriteId].callback = &SpriteCallbackDummy;
        StartSpriteAnim(&gSprites[spriteId], 0);
        for (j = 0; j < 8; j++)
            gSprites[spriteId].data[j] = 0;
        spriteId++;
    }
}

s16 UpdateBallRelativeWheelAngle(struct Sprite *sprite)
{
    if (sRoulette->wheelAngle > sprite->sBallAngle)
    {
        sprite->sBallWheelAngle = 360 - sRoulette->wheelAngle + sprite->sBallAngle;
        if (sprite->sBallWheelAngle >= 360)
            sprite->sBallWheelAngle -= 360;
    }
    else
        sprite->sBallWheelAngle = sprite->sBallAngle - sRoulette->wheelAngle;
    return sprite->sBallWheelAngle;
}

u8 UpdateSlotBelowBall(struct Sprite *sprite)
{
    sRoulette->hitSlot = (u8)(((float)(s16)UpdateBallRelativeWheelAngle(sprite)) / DEGREES_PER_SLOT);
    return sRoulette->hitSlot;
}

s16 GetBallDistanceToSlotMidpoint(struct Sprite *sprite)
{
    s16 angleIntoSlot = UpdateBallRelativeWheelAngle(sprite) % DEGREES_PER_SLOT;
    u16 distanceToMidpoint;
    if (angleIntoSlot == SLOT_MIDPOINT)
    {
        distanceToMidpoint = 0;
        return sprite->sSlotMidpointDist = distanceToMidpoint;
    }
    else if (angleIntoSlot > SLOT_MIDPOINT - 1)
    {
        distanceToMidpoint = (u16)(DEGREES_PER_SLOT - 1 + SLOT_MIDPOINT - angleIntoSlot);
        return sprite->sSlotMidpointDist = distanceToMidpoint;
    }
    else
    {
        distanceToMidpoint = (u16)(SLOT_MIDPOINT - angleIntoSlot);
        return sprite->sSlotMidpointDist = distanceToMidpoint;
    }
}

void UpdateBallPos(struct Sprite *sprite)
{
    s16 sin, cos;
    sRoulette->ballAngleSpeed += sRoulette->ballAngleAccel;
    sRoulette->ballAngle += sRoulette->ballAngleSpeed;
    if (sRoulette->ballAngle >= 360)
        sRoulette->ballAngle -= 360.0f;
    else if (sRoulette->ballAngle < 0.0f)
        sRoulette->ballAngle += 360.0f;
    sprite->sBallAngle = sRoulette->ballAngle;
    sRoulette->ballFallSpeed += sRoulette->ballFallAccel;
    sRoulette->ballDistToCenter += sRoulette->ballFallSpeed;
    sprite->sBallDistToCenter = sRoulette->ballDistToCenter;
    sin = Sin2(sprite->sBallAngle);
    cos = Cos2(sprite->sBallAngle);
    sprite->x2 =  sin * sprite->sBallDistToCenter >> 12;
    sprite->y2 = -cos * sprite->sBallDistToCenter >> 12;
    if (IsSEPlaying())
    {
        m4aMPlayPanpotControl(&gMPlayInfo_SE1, 0xffff, sprite->x2);
        m4aMPlayPanpotControl(&gMPlayInfo_SE2, 0xffff, sprite->x2);
    }
}

void SpriteCB_BallLandInSlot(struct Sprite *sprite)
{
    s16 sin, cos;
    sprite->sBallAngle = sRoulette->wheelAngle + sprite->sBallWheelAngle;
    if (sprite->sBallAngle >= 360)
        sprite->sBallAngle -= 360;
    sin = Sin2(sprite->sBallAngle);
    cos = Cos2(sprite->sBallAngle);
    sprite->x2 =  sin * sprite->sBallDistToCenter >> 12;
    sprite->y2 = -cos * sprite->sBallDistToCenter >> 12;
    sprite->y2 += gSpriteCoordOffsetY;
}

void SpriteCB_UnstickBall_ShroomishBallFall(struct Sprite *sprite)
{
    UpdateBallPos(sprite);
    sprite->data[2]++;
    if ((u16)(sprite->sBallDistToCenter + 132) > 212)
        sprite->invisible = TRUE;
    else
        sprite->invisible = FALSE;
    if (!(sprite->data[2] < DEGREES_PER_SLOT))
    {
        if (!sprite->sStuckOnWheelLeft)
        {
            if (sRoulette->ballDistToCenter <= sRoulette->varA0 - 2.0f)
            {
                sRoulette->ballState = BALL_STATE_LANDED;
                sRoulette->ballRolling = FALSE;
                StartSpriteAnim(sprite, sprite->animCmdIndex + 3);
                UpdateSlotBelowBall(sprite);
                sprite->sBallDistToCenter = DEGREES_PER_SLOT;
                UpdateBallRelativeWheelAngle(sprite);
                sprite->sBallWheelAngle = (sprite->sBallWheelAngle / DEGREES_PER_SLOT) * DEGREES_PER_SLOT + 15;
                sprite->callback = SpriteCB_BallLandInSlot;
                m4aSongNumStartOrChange(SE_BRIDGE_WALK);
                sRoulette->ballFallAccel = sRoulette->ballFallSpeed = 0.0f;
                sRoulette->ballAngleSpeed = -1.0f;
            }
        }
        else
        {
            if (sRoulette->ballDistToCenter >= sRoulette->varA0 - 2.0f)
            {
                sRoulette->ballState = BALL_STATE_LANDED;
                sRoulette->ballRolling = FALSE;
                StartSpriteAnim(sprite, sprite->animCmdIndex + 3);
                UpdateSlotBelowBall(sprite);
                sprite->sBallDistToCenter = DEGREES_PER_SLOT;
                UpdateBallRelativeWheelAngle(sprite);
                sprite->sBallWheelAngle = (sprite->sBallWheelAngle / DEGREES_PER_SLOT) * DEGREES_PER_SLOT + 15;
                sprite->callback = SpriteCB_BallLandInSlot;
                m4aSongNumStartOrChange(SE_BRIDGE_WALK);
                sRoulette->ballFallAccel = sRoulette->ballFallSpeed = 0.0f;
                sRoulette->ballAngleSpeed = -1.0f;
            }
        }
    }
}

void SpriteCB_UnstickBall_Shroomish(struct Sprite *sprite)
{
    float slotOffset, ballFallDist, ballFallSpeed;
    const struct RouletteTable *tables;
    UpdateBallPos(sprite);
    switch (sprite->sBallAngle)
    {
    case 0:
        if (sprite->sStuckOnWheelLeft != TRUE)
        {
            slotOffset = ((float)sprite->data[7]);
            tables = sRouletteTables;
            ballFallDist = (slotOffset * ((float)(s32)tables[sRoulette->tableId].randDistanceHigh) + (float)((s32)tables[sRoulette->tableId].randDistanceLow - 1));
            ballFallSpeed = (slotOffset / ((float)(s32)tables[sRoulette->tableId].shroomish.fallSlowdown));
        }
        else
        {
            return;
        }
        break;
    case 180:
        if (sprite->sStuckOnWheelLeft != FALSE)
        {
            slotOffset = ((float)sprite->data[7]);
            tables = sRouletteTables;
            ballFallDist = (slotOffset * ((float)(s32)tables[sRoulette->tableId].randDistanceHigh) + (float)((s32)tables[sRoulette->tableId].randDistanceLow - 1));
            ballFallSpeed = -(slotOffset / ((float)(s32)tables[sRoulette->tableId].shroomish.fallSlowdown));
        }
        else
        {
            return;
        }
        break;
        default: return;
    }
    sRoulette->varA0 = sRoulette->ballDistToCenter;
    sRoulette->ballFallSpeed = ballFallSpeed;
    sRoulette->ballFallAccel = -((ballFallSpeed + ballFallSpeed) / ballFallDist + (2.0f / (ballFallDist * ballFallDist)));
    sRoulette->ballAngleSpeed = 0.0f;
    sprite->animPaused    = FALSE;
    sprite->animNum       = 0;
    sprite->animBeginning = TRUE;
    sprite->animEnded     = FALSE;
    sprite->callback      = SpriteCB_UnstickBall_ShroomishBallFall;
    sprite->data[2]       = 0;
}

void SpriteCB_UnstickBall_TaillowDrop(struct Sprite *sprite)
{
    sprite->y2 = (s32)(((float)sprite->data[2]) * 0.05f * ((float)sprite->data[2])) - 45;
    sprite->data[2]++;
    if (sprite->data[2] >= DEGREES_PER_SLOT && sprite->y2 >= 0)
    {
        sRoulette->ballState   = BALL_STATE_LANDED;
        sRoulette->ballRolling = FALSE;
        StartSpriteAnim(sprite, sprite->animCmdIndex + 3);
        UpdateSlotBelowBall(sprite);
        sprite->sBallDistToCenter = DEGREES_PER_SLOT;
        UpdateBallRelativeWheelAngle(sprite);
        sprite->sBallWheelAngle = (sprite->sBallWheelAngle / DEGREES_PER_SLOT) * DEGREES_PER_SLOT + 15;
        sprite->callback  = SpriteCB_BallLandInSlot;
        m4aSongNumStartOrChange(SE_BRIDGE_WALK);
        sRoulette->ballUnstuck = TRUE;
    }
}

void SpriteCB_UnstickBall_TaillowPickUp(struct Sprite *sprite)
{
    if (sprite->data[2]++ < 45)
    {
        sprite->y2--;
        if (sprite->data[2] == 45)
        {
            if (gSprites[sRoulette->spriteIds[SPR_CLEAR_MON]].animCmdIndex == 1)
                sprite->y2++;
        }
    }
    else
    {
        if (sprite->data[2] < sprite->data[7])
        {
            if (gSprites[sRoulette->spriteIds[SPR_CLEAR_MON]].animDelayCounter == 0)
            {
                if (gSprites[sRoulette->spriteIds[SPR_CLEAR_MON]].animCmdIndex == 1)
                    sprite->y2++;
                else
                    sprite->y2--;
            }
        }
        else
        {
            sprite->animPaused    = FALSE;
            sprite->animNum       = 1;
            sprite->animBeginning = TRUE;
            sprite->animEnded     = FALSE;
            sprite->data[2]       = 0;
            sprite->callback      = SpriteCB_UnstickBall_TaillowDrop;
            m4aSongNumStart(SE_BALL_THROW);
        }
    }
}

void SpriteCB_UnstickBall_Taillow(struct Sprite *sprite)
{
    UpdateBallPos(sprite);
    switch (sprite->sBallAngle)
    {
    case 90:
        if (sprite->sStuckOnWheelLeft != TRUE)
        {
            sprite->callback  = SpriteCB_UnstickBall_TaillowPickUp;
            sprite->data[2] = 0;
        }
        break;
    case 270:
        if (sprite->sStuckOnWheelLeft != FALSE)
        {
            sprite->callback  = SpriteCB_UnstickBall_TaillowPickUp;
            sprite->data[2] = 0;
        }
        break;
    }
}

void SpriteCB_UnstickBall(struct Sprite *sprite)
{
    UpdateBallPos(sprite);
    switch (sRoulette->useTaillow)
    {
    default:
    case FALSE:
        CreateShroomishSprite(sprite);
        sprite->callback = SpriteCB_UnstickBall_Shroomish;
        break;
    case TRUE:
        CreateTaillowSprite(sprite);
        sprite->callback = SpriteCB_UnstickBall_Taillow;
        break;
    }
}

#define sStillStuck data[0]

void SpriteCB_RollBall_TryLandAdjacent(struct Sprite *sprite)
{
    UpdateBallPos(sprite);
    if (sprite->data[2]-- == 16)
        sRoulette->ballFallSpeed *= -1.0f;
    if (sprite->data[2] == 0)
    {
        if (!sprite->sStillStuck)
        {
            sRoulette->ballState   = BALL_STATE_LANDED;
            sRoulette->ballRolling = FALSE;
            StartSpriteAnim(sprite, sprite->animCmdIndex + 3);
            UpdateSlotBelowBall(sprite);
            sprite->sBallDistToCenter = DEGREES_PER_SLOT;
            UpdateBallRelativeWheelAngle(sprite);
            sprite->sBallWheelAngle = (sprite->sBallWheelAngle / DEGREES_PER_SLOT) * DEGREES_PER_SLOT + 15;
            sprite->callback  = SpriteCB_BallLandInSlot;
            m4aSongNumStartOrChange(SE_BRIDGE_WALK);
        }
        else
        {
            sprite->animPaused = TRUE;
            m4aSongNumStart(SE_BALL_BOUNCE_1);
            SetBallStuck(sprite);
        }
    }
}

#if DEBUG

void SpriteCB_DebugForceBallStuck(struct Sprite *sprite)
{
    UpdateBallPos(sprite);
    sprite->data[2] = 0;
    UpdateSlotBelowBall(sprite);
    m4aSongNumStart(SE_BALL_BOUNCE_1);
    if (Random() & 1)
    {
        sRoulette->ballAngleSpeed = 0;
        sRoulette->stuckHitSlot = (sRoulette->hitSlot + 1) % NUM_ROULETTE_SLOTS;
    }
    else
    {
        sRoulette->ballAngleSpeed = ({float f = sRouletteTables[sRoulette->tableId].var1C; f * 2;});
        sRoulette->stuckHitSlot = sRoulette->hitSlot;
    }
    sprite->sStillStuck = TRUE;
    sprite->data[2] = sRouletteTables[sRoulette->tableId].randDistanceLow;
    sRoulette->ballFallSpeed = 0.085f;
    sprite->callback = SpriteCB_RollBall_TryLandAdjacent;
    sprite->sState = 5;
}

#endif

void SpriteCB_RollBall_TryLand(struct Sprite *sprite)
{
    UpdateBallPos(sprite);
    sprite->data[2] = 0;
    UpdateSlotBelowBall(sprite);
    if (!(sRouletteSlots[sRoulette->hitSlot].flag & sRoulette->hitFlags))
    {
        sRoulette->ballState   = BALL_STATE_LANDED;
        sRoulette->ballRolling = FALSE;
        StartSpriteAnim(sprite, sprite->animCmdIndex + 3);
        UpdateSlotBelowBall(sprite);
        sprite->sBallDistToCenter = DEGREES_PER_SLOT;
        UpdateBallRelativeWheelAngle(sprite);
        sprite->sBallWheelAngle = (sprite->sBallWheelAngle / DEGREES_PER_SLOT) * DEGREES_PER_SLOT + 15;
        sprite->callback  = SpriteCB_BallLandInSlot;
        m4aSongNumStartOrChange(SE_BRIDGE_WALK);
    }
    else
    {
        u8 slotId;
        u32 fallRight;
        m4aSongNumStart(SE_BALL_BOUNCE_1);
        if ((fallRight = (Random() & 1)))
        {
            u32 nextSlotId;
            sRoulette->ballAngleSpeed = 0.0f;
            nextSlotId = (sRoulette->hitSlot + 1) % NUM_ROULETTE_SLOTS;
            slotId = nextSlotId;
            sRoulette->stuckHitSlot = nextSlotId;
        }
        else
        {
            sRoulette->ballAngleSpeed = ({
                float f = sRouletteTables[sRoulette->tableId].var1C;
                f * 2;
            }); // couldn't replicate loads
            slotId = (sRoulette->hitSlot + NUM_ROULETTE_SLOTS - 1) % NUM_ROULETTE_SLOTS;
            sRoulette->stuckHitSlot = sRoulette->hitSlot;
        }
        if (sRouletteSlots[slotId].flag & sRoulette->hitFlags)
        {
            sprite->sStillStuck = TRUE;
            sprite->data[2] = sRouletteTables[sRoulette->tableId].randDistanceLow;
        }
        else
        {
            sprite->sStillStuck = sRouletteSlots[slotId].flag & sRoulette->hitFlags;
            if (sRoulette->tableId)
            {
                sprite->data[2] = sRouletteTables[sRoulette->tableId].randDistanceHigh;
            }
            else
            {
                sprite->data[2] = sRouletteTables[sRoulette->tableId].randDistanceLow;
                if (fallRight)
                {
                    sRoulette->ballAngleSpeed = 0.5f;
                }
                else
                {
                    sRoulette->ballAngleSpeed = -1.5f;
                }
            }
        }
        sRoulette->ballFallSpeed = 0.085f;
                sprite->callback = SpriteCB_RollBall_TryLandAdjacent;
        sprite->sState = 5;
    }
}

#undef sStillStuck

void SpriteCB_RollBall_Slow(struct Sprite *sprite)
{
    UpdateBallPos(sprite);
    if (!(sRoulette->ballAngleSpeed > 0.5f))
    {
        UpdateSlotBelowBall(sprite);
        if (!GetBallDistanceToSlotMidpoint(sprite))
        {
            const struct RouletteTable *tables;
            sRoulette->ballAngleAccel = 0.0f;
            tables = sRouletteTables;
            sRoulette->ballAngleSpeed -= ((float)(tables[sRoulette->tableId].wheelSpeed))
                / ((float)(s16)(tables[sRoulette->tableId].wheelDelay + 1));
            sprite->sState = 4;
#if DEBUG
            if (gRouletteDebugForceBallStuck != 0)
                sprite->callback = SpriteCB_DebugForceBallStuck;
            else
#endif
                sprite->callback = SpriteCB_RollBall_TryLand;
        }
        else
        {
            if (sRoulette->ballAngleAccel != 0.0f)
            {
                if (sRoulette->ballAngleSpeed < 0.0f)
                {
                    sRoulette->ballAngleAccel = 0.0f;
                    sRoulette->ballAngleSpeed = 0.0f;
                    sRoulette->ballFallSpeed /= 1.2;
                }
            }
        }
    }
}

void SpriteCB_RollBall_Medium(struct Sprite *sprite)
{
    UpdateBallPos(sprite);
    if (!(sRoulette->ballDistToCenter > 40.f))
    {
        sRoulette->ballFallSpeed = - (        4.0f / (float)((u16)sRoulette->ballTravelDistSlow));
        sRoulette->ballAngleAccel = - (sRoulette->ballAngleSpeed / (float)((u16)sRoulette->ballTravelDistSlow));
        sprite->animNum       = 2;
        sprite->animBeginning = TRUE;
        sprite->animEnded     = FALSE;
        sprite->sState      = 3;
        sprite->callback      = SpriteCB_RollBall_Slow;
    }
}

void SpriteCB_RollBall_Fast(struct Sprite *sprite)
{
    UpdateBallPos(sprite);
    if (!(sRoulette->ballDistToCenter > 60.0f))
    {
        m4aSongNumStartOrChange(SE_ROULETTE_BALL2);
        sRoulette->ballFallSpeed = - (                20.0f / (float)((u16)sRoulette->ballTravelDistMed));
        sRoulette->ballAngleAccel =   ((1.0f - sRoulette->ballAngleSpeed) / (float)((u16)sRoulette->ballTravelDistMed));
        sprite->animNum       = 1;
        sprite->animBeginning = TRUE;
        sprite->animEnded     = FALSE;
        sprite->sState      = 2;
        sprite->callback      = SpriteCB_RollBall_Medium;
    }
}

void SpriteCB_RollBall_Start(struct Sprite *sprite)
{
    sprite->sState = 1;
    sprite->data[2] = 0;
    UpdateBallPos(sprite);
    sprite->invisible = FALSE;
    sprite->callback  = SpriteCB_RollBall_Fast;
}

void CreateShroomishSprite(struct Sprite *ball)
{
    u16 slotOffset;
    u8 i;
    s16 coords[2][2] = {
        {116, 44},
        {116, 112}
    };
    struct Roulette *roulette;
    slotOffset = ball->data[7] - 2;
    roulette = sRoulette;
    roulette->spriteIds[SPR_CLEAR_MON] = CreateSprite(&sSpriteTemplate_Shroomish, 36, -12, 50);
    roulette->spriteIds[SPR_CLEAR_MON_SHADOW_1] = CreateSprite(sSpriteTemplate_ShroomishShadow, coords[ball->sStuckOnWheelLeft][0], coords[ball->sStuckOnWheelLeft][1], 59);
    roulette->spriteIds[SPR_CLEAR_MON_SHADOW_2] = CreateSprite(&sSpriteTemplate_ShroomishShadow[1], 36, 140, 51);
    gSprites[sRoulette->spriteIds[SPR_CLEAR_MON_SHADOW_2]].oam.objMode = 1;
    for (i = 0; i < 3; i++)
    {
        gSprites[sRoulette->spriteIds[SPR_CLEAR_MON + i]].coordOffsetEnabled = FALSE;
        gSprites[sRoulette->spriteIds[SPR_CLEAR_MON + i]].invisible          = TRUE;
        gSprites[sRoulette->spriteIds[SPR_CLEAR_MON + i]].animPaused         = TRUE;
        gSprites[sRoulette->spriteIds[SPR_CLEAR_MON + i]].affineAnimPaused   = TRUE;
        gSprites[sRoulette->spriteIds[SPR_CLEAR_MON + i]].sMonSpriteId = sRoulette->spriteIds[SPR_CLEAR_MON];
        gSprites[sRoulette->spriteIds[SPR_CLEAR_MON + i]].sBallShadowSpriteId = sRoulette->spriteIds[SPR_CLEAR_MON_SHADOW_1];
        gSprites[sRoulette->spriteIds[SPR_CLEAR_MON + i]].sMonShadowSpriteId = sRoulette->spriteIds[SPR_CLEAR_MON_SHADOW_2];
        gSprites[sRoulette->spriteIds[SPR_CLEAR_MON + i]].data[2] = slotOffset;
        gSprites[sRoulette->spriteIds[SPR_CLEAR_MON + i]].data[3] = (ball->data[7] * sRouletteTables[sRoulette->tableId].randDistanceHigh) +
                                                                (sRouletteTables[sRoulette->tableId].randDistanceLow + 0xffff);
    }
    gSprites[sRoulette->spriteIds[SPR_CLEAR_MON_SHADOW_1]].coordOffsetEnabled = TRUE;
    sRoulette->ball = ball;
}

void CreateTaillowSprite(struct Sprite *ball)
{
    u8 i = 0;
    s16 slotOffset;
    s16 coords[2][2] = {
        {256, 84},
        {-16, 84}
    };
    const struct RouletteTable *tables;
    slotOffset = ball->data[7] - 2;
    sRoulette->spriteIds[SPR_CLEAR_MON] = CreateSprite(&sSpriteTemplate_Taillow, coords[ball->sStuckOnWheelLeft][0], coords[ball->sStuckOnWheelLeft][1], 50);
    StartSpriteAnim(&gSprites[sRoulette->spriteIds[SPR_CLEAR_MON]], ball->sStuckOnWheelLeft);
    sRoulette->spriteIds[SPR_CLEAR_MON_SHADOW_1] = CreateSprite(&sSpriteTemplate_TaillowShadow, coords[ball->sStuckOnWheelLeft][0], coords[ball->sStuckOnWheelLeft][1], 51);
    gSprites[sRoulette->spriteIds[SPR_CLEAR_MON_SHADOW_1]].affineAnimPaused = TRUE;
    gSprites[sRoulette->spriteIds[SPR_CLEAR_MON_SHADOW_1]].animPaused       = TRUE;
    ball->data[7] = (slotOffset * (tables = sRouletteTables)[sRoulette->tableId].randDistanceHigh) + (tables[sRoulette->tableId].taillow.baseDropDelay + 45);
    for (; i < 2; i++)
    {
        gSprites[sRoulette->spriteIds[SPR_CLEAR_MON + i]].sMonSpriteId = sRoulette->spriteIds[SPR_CLEAR_MON];
        gSprites[sRoulette->spriteIds[SPR_CLEAR_MON + i]].sBallShadowSpriteId = sRoulette->spriteIds[SPR_CLEAR_MON_SHADOW_1];
        gSprites[sRoulette->spriteIds[SPR_CLEAR_MON + i]].sMonShadowSpriteId = sRoulette->spriteIds[SPR_CLEAR_MON_SHADOW_1];
        gSprites[sRoulette->spriteIds[SPR_CLEAR_MON + i]].data[2] = slotOffset;
        gSprites[sRoulette->spriteIds[SPR_CLEAR_MON + i]].data[3] = ball->data[7] - 45;
    }
    sRoulette->ball = ball;
}

void SetBallStuck(struct Sprite *sprite)
{
    u8 slotId;
    u16 angle;
    u8 numCandidates = 0;
    u8 maxSlotToCheck = 5;
    u8 betSlotId = 0;
    u8 i = 0;
    u8 slotsToSkip;
    u8 slotCandidates[NUM_ROULETTE_SLOTS - 2] = {};
    u16 rand = Random();

    sRoulette->ballState = BALL_STATE_STUCK;
    sRoulette->ballStuck = TRUE;
    sRoulette->ballUnstuck = FALSE;
    sRoulette->hitSlot = 0xFF;
    sRoulette->ballAngle = sprite->sBallAngle;
    sRoulette->ballFallSpeed = 0.0f;
    sRoulette->ballAngleSpeed = sRouletteTables[sRoulette->tableId].var1C;

    angle = (sRoulette->tableId * DEGREES_PER_SLOT + 33) + (1 - sRoulette->useTaillow) * 15;
    for (i = 0; i < 4; i++)
    {
        if (angle < sprite->sBallAngle && sprite->sBallAngle <= angle + 90)
        {
            sprite->sStuckOnWheelLeft = i / 2;
            sRoulette->useTaillow = i % 2;
            break;
        }
        if (i == 3)
        {
            sprite->sStuckOnWheelLeft = TRUE;
            sRoulette->useTaillow = TRUE;
            break;
        }
        angle += 90;
    }

    if (sRoulette->useTaillow)
    {
        if (sprite->sStuckOnWheelLeft)
            PlayCry_Normal(SPECIES_TAILLOW, -63);
        else
            PlayCry_Normal(SPECIES_TAILLOW, 63);
    }
    else
    {
        PlayCry_Normal(SPECIES_SHROOMISH, -63);
    }

    slotsToSkip = 2;
    slotId = (sRoulette->stuckHitSlot + 2) % NUM_ROULETTE_SLOTS;

    if (sRoulette->useTaillow == TRUE && sRoulette->tableId == 1)
        maxSlotToCheck += BALLS_PER_ROUND;
    else
        maxSlotToCheck += slotsToSkip;

    for (i = slotsToSkip; i < maxSlotToCheck; i++)
    {
        if (!(sRoulette->hitFlags & sRouletteSlots[slotId].flag))
        {
            slotCandidates[numCandidates++] = i;
            if (betSlotId == 0 && (sRouletteSlots[slotId].flag & sGridSelections[sRoulette->betSelection[sRoulette->curBallNum]].inSelectionFlags))
                betSlotId = i;
        }
        slotId = (slotId + 1) % NUM_ROULETTE_SLOTS;
    }

    if ((sRoulette->useTaillow + 1) & sRoulette->partySpeciesFlags)
    {
        if (betSlotId && (rand & 0xFF) < 0xc0)
            sprite->data[7] = betSlotId;
        else
            sprite->data[7] = slotCandidates[rand % numCandidates];
    }
    else
    {
        sprite->data[7] = slotCandidates[rand % numCandidates];
    }

    sprite->callback = SpriteCB_UnstickBall;
}

static const u16 sShroomishShadowAlphas[] = {
    BLDALPHA_BLEND( 7, 9),
    BLDALPHA_BLEND( 8, 8),
    BLDALPHA_BLEND( 9, 7),
    BLDALPHA_BLEND(10, 6),
    BLDALPHA_BLEND(11, 5),
    BLDALPHA_BLEND(12, 4),
    BLDALPHA_BLEND(13, 3),
    BLDALPHA_BLEND(14, 2),
    BLDALPHA_BLEND(15, 1),
    BLDALPHA_BLEND(16, 0),
};

void SpriteCB_ShroomishExit(struct Sprite *sprite)
{
    if (sprite->data[1]++ >= sprite->data[3])
    {
        sprite->x -= 2;
        if (sprite->x < -16)
        {
            if (!sRoulette->ballUnstuck)
                sRoulette->ballUnstuck = TRUE;
            DestroySprite(sprite);
            sRoulette->shroomishShadowTimer = 0;
            sRoulette->shroomishShadowAlpha = sShroomishShadowAlphas[0];
        }
    }
}

void SpriteCB_ShroomishShakeScreen(struct Sprite *sprite)
{
    s16 screenShakeOffsets[3][4] = {
        {-1, 0, 1, 0},
        {-2, 0, 2, 0},
        {-3, 0, 3, 0}
    };
    s32 normalizedIndex;
    s32 screenShakeIdx;

    if (sprite->data[1]++ < sprite->data[3])
    {
        if (sprite->data[1] & 1)
        {
            gSpriteCoordOffsetY = screenShakeOffsets[sprite->data[2] / 2][sprite->data[7]];
            normalizedIndex = screenShakeIdx = sprite->data[7] + 1;
            if (screenShakeIdx < 0)
                normalizedIndex += 3;
            sprite->data[7] = screenShakeIdx - ((normalizedIndex >> 2) * 4);
        }
        sprite->invisible ^= 1;
    }
    else
    {
        gSpriteCoordOffsetY = 0;
        gSprites[sRoulette->spriteIds[SPR_CLEAR_MON]].animPaused = FALSE;
        DestroySprite(sprite);
    }
}

void SpriteCB_ShroomishFall(struct Sprite *sprite)
{
    float timer;
    sprite->data[1]++;
    timer = sprite->data[1];
    sprite->y2 = timer * 0.039f * timer;
    sRoulette->shroomishShadowAlpha = sShroomishShadowAlphas[(sRoulette->shroomishShadowTimer - 1) / 2];
    if (sRoulette->shroomishShadowTimer < 19)
        sRoulette->shroomishShadowTimer++;
    if (sprite->data[1] > 60)
    {
        sprite->data[1] = 0;
        sprite->callback = SpriteCB_ShroomishExit;
        gSprites[sprite->sMonShadowSpriteId].callback  = SpriteCB_ShroomishExit;
        gSprites[sprite->sMonShadowSpriteId].data[1] = -2;
        gSprites[sprite->sBallShadowSpriteId].invisible = FALSE;
        gSprites[sprite->sBallShadowSpriteId].callback  = SpriteCB_ShroomishShakeScreen;
        m4aSongNumStart(SE_M_STRENGTH);
    }
}

void SpriteCB_Shroomish(struct Sprite *sprite)
{
    if (sprite->data[7] == 0)
    {
        register u32 ballAngle asm("r2");
        u32 startAngle;
        if (sRoulette->ball->sStuckOnWheelLeft == FALSE)
        {
            ballAngle = sRoulette->ball->sBallAngle;
            startAngle = sRouletteTables[sRoulette->tableId].shroomish.startAngle;
        }
        else
        {
            ballAngle = sRoulette->ball->sBallAngle;
            startAngle = sRouletteTables[sRoulette->tableId].shroomish.startAngle;
            startAngle += 180;
        }
        if (ballAngle == startAngle)
        {
            sprite->invisible = FALSE;
            sprite->data[7]++;
            m4aSongNumStart(SE_FALL);
            sRoulette->shroomishShadowTimer = 1;
            sRoulette->shroomishShadowAlpha = sShroomishShadowAlphas[0];
        }
    }
    else
    {
        u32 ballAngle, dropAngle;
        sRoulette->shroomishShadowAlpha = sShroomishShadowAlphas[(sRoulette->shroomishShadowTimer - 1) / 2];
        if (sRoulette->shroomishShadowTimer < 19)
            sRoulette->shroomishShadowTimer++;
        if (sRoulette->ball->sStuckOnWheelLeft == FALSE)
        {
            ballAngle = sRoulette->ball->sBallAngle;
            dropAngle = sRouletteTables[sRoulette->tableId].shroomish.dropAngle;
        }
        else
        {
            ballAngle = sRoulette->ball->sBallAngle;
            dropAngle = sRouletteTables[sRoulette->tableId].shroomish.dropAngle;
            dropAngle += 180;
        }
        if (ballAngle == dropAngle)
        {
            gSprites[sprite->sMonSpriteId].callback  = SpriteCB_ShroomishFall;
            gSprites[sprite->sMonSpriteId].invisible = FALSE;
            sprite->callback  = &SpriteCallbackDummy;
            sprite->data[7] = 0;
        }
    }
}

void SpriteCB_TaillowShadow_Flash(struct Sprite *sprite)
{
    sprite->invisible ^= 1;
}

void SpriteCB_Taillow_FlyAway(struct Sprite *sprite)
{
    if (sprite->y > -16)
    {
        sprite->y--;
    }
    else
    {
        sprite->callback   = &SpriteCallbackDummy;
        sprite->invisible  = TRUE;
        sprite->animPaused = TRUE;
        m4aSongNumStop(SE_TAILLOW_WING_FLAP);
        DestroySprite(sprite);
        FreeOamMatrix(gSprites[sRoulette->spriteIds[SPR_CLEAR_MON_SHADOW_1]].oam.matrixNum);
        DestroySprite(&gSprites[sRoulette->spriteIds[SPR_CLEAR_MON_SHADOW_1]]);
    }
}

void SpriteCB_Taillow_PickUpBall(struct Sprite *sprite)
{
    if (sprite->data[1] >= 0)
    {
        sprite->data[1]--;
        sprite->y--;
        if (sprite->data[1] == 0 && sprite->animCmdIndex == 1)
            sprite->y2++;
    }
    else
    {
        if (sprite->data[3] >= 0)
        {
            sprite->data[3]--;
            if (sprite->animDelayCounter == 0)
            {
                if (sprite->animCmdIndex == 1)
                    sprite->y2++;
                else
                    sprite->y2--;
            }
        }
        else
        {
            m4aSongNumStart(SE_FALL);
            StartSpriteAnim(sprite, sRoulette->ball->sStuckOnWheelLeft + 4);
            sprite->callback = SpriteCB_Taillow_FlyAway;
            gSprites[sprite->sMonShadowSpriteId].affineAnimPaused = FALSE;
        }
    }
}

void SpriteCB_Taillow_FlyIn(struct Sprite *sprite)
{
    s8 xMoveOffsets[2] = {-1, 1};
    s8 yMoveOffsets[8][2] = {
        {2,  0},
        {2,  0},
        {2, -1},
        {2, -1},
        {2, -1},
        {2, -1},
        {2, -2},
        {2, -2}
    };
    if (sprite->data[1]-- > 7)
    {
        sprite->x += xMoveOffsets[sRoulette->ball->sStuckOnWheelLeft] * 2;
        if (IsSEPlaying())
        {
            s8 pan = -((116 - sprite->x) / 2);
            m4aMPlayPanpotControl(&gMPlayInfo_SE1, 0xffff, pan);
            m4aMPlayPanpotControl(&gMPlayInfo_SE2, 0xffff, pan);
        }
    }
    else
    {
        if (sprite->data[1] >= 0)
        {
            sprite->x += xMoveOffsets[sRoulette->ball->sStuckOnWheelLeft] * yMoveOffsets[7 - sprite->data[1]][0];
            sprite->y += yMoveOffsets[7 - sprite->data[1]][1];
        }
        else
        {
            m4aSongNumStartOrChange(SE_TAILLOW_WING_FLAP);
            if (sRoulette->ball->sStuckOnWheelLeft == FALSE)
                PlayCry_Normal(SPECIES_TAILLOW, 63);
            else
                PlayCry_Normal(SPECIES_TAILLOW, -63);
            StartSpriteAnim(sprite, sRoulette->ball->sStuckOnWheelLeft + 2);
            sprite->data[1] = 45;
            sprite->callback = SpriteCB_Taillow_PickUpBall;
        }
    }
}

void SpriteCB_TaillowShadow_FlyIn(struct Sprite *sprite)
{
    s8 moveDir[2] = {-1, 1};
    if (sprite->data[1]-- >= 0)
    {
        sprite->x += moveDir[sRoulette->ball->sStuckOnWheelLeft] * 2;
        gSprites[sprite->sMonShadowSpriteId].invisible ^= 1;
    }
    else
    {
        sprite->callback = SpriteCB_TaillowShadow_Flash;
    }
}

void SpriteCB_Taillow(struct Sprite *sprite)
{
    if (sRoulette->ball->sStuckOnWheelLeft == FALSE)
    {
        if (sRoulette->ball->sBallAngle == sRouletteTables[sRoulette->tableId].taillow.rightStartAngle + 90)
        {
            gSprites[sprite->sMonShadowSpriteId].data[1] = 52;
            gSprites[sprite->sMonSpriteId].data[1] = 52;
        }
        else
        {
            return;
        }
    }
    else
    {
        if (sRoulette->ball->sBallAngle == sRouletteTables[sRoulette->tableId].taillow.leftStartAngle + 270)
        {
            gSprites[sprite->sMonShadowSpriteId].data[1] = 46;
            gSprites[sprite->sMonSpriteId].data[1] = 46;
        }
        else
        {
            return;
        }
    }
    gSprites[sprite->sMonShadowSpriteId].callback = SpriteCB_TaillowShadow_FlyIn;
    gSprites[sprite->sMonSpriteId].callback = SpriteCB_Taillow_FlyIn;
    m4aSongNumStart(SE_FALL);
}
