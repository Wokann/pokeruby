#ifndef GUARD_ROULETTE_H
#define GUARD_ROULETTE_H

#include "task.h"
#include "palette_util.h"

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

void PlayRoulette(void);

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

#endif
