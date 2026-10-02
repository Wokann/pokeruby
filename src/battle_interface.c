#include "global.h"
#include "battle.h"
#include "battle_interface.h"
#include "decompress.h"
#include "palette.h"
#include "pokedex.h"
#include "pokemon.h"
#include "battle_anim.h"
#include "safari_zone.h"
#include "constants/songs.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "ewram.h"
#include "graphics.h"
#include "pokemon_summary_screen.h"

struct BattleBar
{
    u8 healthboxSpriteId;
    u32 maxValue;
    u32 oldValue;
    u32 receivedValue;
    int currValue;
};

static void SpriteCB_HealthBar(struct Sprite *sprite);
static void SpriteCB_StatusSummaryBar_Enter(struct Sprite *sprite);
static void SpriteCB_StatusSummaryBalls_Enter(struct Sprite *sprite);

// Healthbox sprite data fields.
#define hMain_HealthBarSpriteId     data[5]
#define hMain_Battler               data[6]
#define hMain_Data7                 data[7]
#define hOther_HealthBoxSpriteId    data[5]
#define hBar_HealthBoxSpriteId      data[5]
#define hBar_Data6                  data[6]

const struct OamData sOamData_64x32 =
{
    .shape = 1,
    .size = 3,
    .priority = 1,
};

const struct SpriteTemplate sHealthboxPlayerSpriteTemplates[] =
{
    {
        .tileTag = 55039,
        .paletteTag = 55039,
        .oam = &sOamData_64x32,
        .anims = gDummySpriteAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCallbackDummy,
    },
    {
        .tileTag = 55040,
        .paletteTag = 55039,
        .oam = &sOamData_64x32,
        .anims = gDummySpriteAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCallbackDummy,
    },
};

const struct SpriteTemplate sHealthboxOpponentSpriteTemplates[] =
{
    {
        .tileTag = 55041,
        .paletteTag = 55039,
        .oam = &sOamData_64x32,
        .anims = gDummySpriteAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCallbackDummy,
    },
    {
        .tileTag = 55042,
        .paletteTag = 55039,
        .oam = &sOamData_64x32,
        .anims = gDummySpriteAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCallbackDummy,
    },
};

const struct SpriteTemplate sHealthboxSafariSpriteTemplate =
{
    .tileTag = 55051,
    .paletteTag = 55039,
    .oam = &sOamData_64x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

const struct OamData sOamData_Healthbar =
{
    .shape = 1,
    .size = 1,
    .priority = 1,
};

const struct SpriteTemplate sHealthbarSpriteTemplates[] =
{
    {
        .tileTag = 55044,
        .paletteTag = 55044,
        .oam = &sOamData_Healthbar,
        .anims = gDummySpriteAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCB_HealthBar,
    },
    {
        .tileTag = 55045,
        .paletteTag = 55044,
        .oam = &sOamData_Healthbar,
        .anims = gDummySpriteAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCB_HealthBar,
    },
    {
        .tileTag = 55046,
        .paletteTag = 55044,
        .oam = &sOamData_Healthbar,
        .anims = gDummySpriteAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCB_HealthBar,
    },
    {
        .tileTag = 55047,
        .paletteTag = 55044,
        .oam = &sOamData_Healthbar,
        .anims = gDummySpriteAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCB_HealthBar,
    },
};

const struct Subsprite sUnused_Subsprites_0[] =
{
    { .x = -16, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =  0, .size = 3 }, // size := 64x32
    { .x =  48, .y =  0, .shape =      ST_OAM_SQUARE, .priority = 1, .tileOffset = 32, .size = 2 }, // size := 32x32
    { .x = -16, .y = 32, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset = 48, .size = 1 }, // size := 32x8
    { .x =  16, .y = 32, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset = 52, .size = 1 }, // size := 32x8
    { .x =  48, .y = 32, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset = 56, .size = 1 }, // size := 32x8
};

const struct Subsprite sUnused_Subsprites_2[] =
{
    { .x = -16, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset = 64, .size = 3 }, // size := 64x32
    { .x =  48, .y =  0, .shape =      ST_OAM_SQUARE, .priority = 1, .tileOffset = 96, .size = 2 }, // size := 32x32
    { .x = -16, .y = 32, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =112, .size = 1 }, // size := 32x8
    { .x =  16, .y = 32, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =116, .size = 1 }, // size := 32x8
    { .x =  48, .y = 32, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =120, .size = 1 }, // size := 32x8
};

const struct Subsprite sUnused_Subsprites_1[] =
{
    { .x = -16, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =  0, .size = 3 }, // size := 64x32
    { .x =  48, .y =  0, .shape =      ST_OAM_SQUARE, .priority = 1, .tileOffset = 32, .size = 2 }, // size := 32x32
};

const struct Subsprite sUnused_Subsprites_3[] =
{
    { .x = -16, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =  0, .size = 3 }, // size := 64x32
    { .x =  48, .y =  0, .shape =      ST_OAM_SQUARE, .priority = 1, .tileOffset = 32, .size = 2 }, // size := 32x32
};

const struct Subsprite sHealthBar_Subsprites_Player[] =
{
    { .x = -16, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =  0, .size = 1 }, // size := 32x8
    { .x =  16, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =  4, .size = 1 }, // size := 32x8
};

const struct Subsprite sHealthBar_Subsprites_Opponent[] =
{
    { .x = -16, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =  0, .size = 1 }, // size := 32x8
    { .x =  16, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =  4, .size = 1 }, // size := 32x8
    { .x = -32, .y =  0, .shape =      ST_OAM_SQUARE, .priority = 1, .tileOffset =  8, .size = 0 }, // size := 8x8
};

const struct SubspriteTable sUnused_SubspriteTable[] =
{
    { ARRAY_COUNT(sUnused_Subsprites_0), sUnused_Subsprites_0 },
    { ARRAY_COUNT(sUnused_Subsprites_1), sUnused_Subsprites_1 },
    { ARRAY_COUNT(sUnused_Subsprites_2), sUnused_Subsprites_2 },
    { ARRAY_COUNT(sUnused_Subsprites_3), sUnused_Subsprites_3 },
};

const struct SubspriteTable sHealthBar_SubspriteTables[] =
{
    { ARRAY_COUNT(sHealthBar_Subsprites_Player), sHealthBar_Subsprites_Player },
    { ARRAY_COUNT(sHealthBar_Subsprites_Opponent), sHealthBar_Subsprites_Opponent },
};

const struct Subsprite sStatusSummaryBar_Subsprites_Enter[] =
{
    { .x = -96, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =  0, .size = 1 }, // size := 32x8
    { .x = -64, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =  4, .size = 1 }, // size := 32x8
    { .x = -32, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =  8, .size = 1 }, // size := 32x8
    { .x =   0, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset = 12, .size = 1 }, // size := 32x8
};

const struct Subsprite sStatusSummaryBar_Subsprites_Exit[] =
{
    { .x = -96, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =  0, .size = 1 }, // size := 32x8
    { .x = -64, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =  4, .size = 1 }, // size := 32x8
    { .x = -32, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =  8, .size = 1 }, // size := 32x8
    { .x =   0, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =  8, .size = 1 }, // size := 32x8
    { .x =  32, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =  8, .size = 1 }, // size := 32x8
    { .x =  64, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset = 12, .size = 1 }, // size := 32x8
};

const struct SubspriteTable sStatusSummaryBar_SubspriteTable_Enter[] =
{
    { ARRAY_COUNT(sStatusSummaryBar_Subsprites_Enter), sStatusSummaryBar_Subsprites_Enter },
};

const struct SubspriteTable sStatusSummaryBar_SubspriteTable_Exit[] =
{
    { ARRAY_COUNT(sStatusSummaryBar_Subsprites_Exit), sStatusSummaryBar_Subsprites_Exit },
};

// unused dakuten/handakuten tiles
const u8 gUnusedDakuten[] = INCBIN_U8("graphics/unused/dakuten.4bpp");

const struct CompressedSpriteSheet sStatusSummaryBarSpriteSheets[] =
{
    { gBattleInterface_BallStatusBarGfx, 512, 0xd70c },
    { gBattleInterface_BallStatusBarGfx, 512, 0xd70d },
};

const struct SpritePalette sStatusSummaryBarSpritePals[] =
{
    { gBattleInterface_BallStatusBarPal, 0xd710 },
    { gBattleInterface_BallStatusBarPal, 0xd711 },
};

const struct SpritePalette sStatusSummaryBallsSpritePals[] =
{
    { gBattleInterface_BallDisplayPal, 0xd712 },
    { gBattleInterface_BallDisplayPal, 0xd713 },
};

const struct SpriteSheet sStatusSummaryBallsSpriteSheets[] =
{
    { gBattleInterface_BallDisplayGfx, 0x80, 0xd714 },
    { gBattleInterface_BallDisplayGfx, 0x80, 0xd715 },
};

const struct OamData sOamData_Unused64x32 =
{
    .shape = 1,
    .size = 3,
    .priority = 1,
};

const struct OamData sOamData_StatusSummaryBalls =
{
    .shape = 0,
    .size = 0,
    .priority = 1,
};

const struct SpriteTemplate sStatusSummaryBarSpriteTemplates[] =
{
    {
        .tileTag = 55052,
        .paletteTag = 55056,
        .oam = &sOamData_64x32,
        .anims = gDummySpriteAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCB_StatusSummaryBar_Enter,
    }, {
        .tileTag = 55053,
        .paletteTag = 55057,
        .oam = &sOamData_64x32,
        .anims = gDummySpriteAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCB_StatusSummaryBar_Enter,
    }
};

const struct SpriteTemplate sStatusSummaryBallsSpriteTemplates[] = {
    {
        .tileTag = 55060,
        .paletteTag = 55058,
        .oam = &sOamData_StatusSummaryBalls,
        .anims = gDummySpriteAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCB_StatusSummaryBalls_Enter,
    }, {
        .tileTag = 55061,
        .paletteTag = 55059,
        .oam = &sOamData_StatusSummaryBalls,
        .anims = gDummySpriteAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCB_StatusSummaryBalls_Enter,
    }
};

u8 *const sPlayerSingleHealthboxLevelTextVram[2] =
{
    OBJ_VRAM0 + 32 * 74,
    OBJ_VRAM0 + 32 * 75,
};

u8 *const sOpponentHealthboxLevelTextVram[2] =
{
    OBJ_VRAM0 + 32 * 41,
    OBJ_VRAM0 + 32 * 42,
};

u8 *const sPlayerDoubleHealthboxLevelTextVram[2] =
{
    OBJ_VRAM0 + 32 * 42,
    OBJ_VRAM0 + 32 * 43,
};

const u8 sEmptyDarkGrayText_RedHighlight[] = __("{COLOR DARK_GREY}{HIGHLIGHT RED}                        ");

u8 *const sPlayerHealthboxCurrentHpTextVram[3] =
{
    OBJ_VRAM0 + 32 * 31,
    OBJ_VRAM0 + 32 * 88,
    OBJ_VRAM0 + 32 * 89,
};

u8 *const sOpponentHealthboxCurrentHpTextVram[3] =
{
    OBJ_VRAM0 + 32 * 22,
    OBJ_VRAM0 + 32 * 23,
    OBJ_VRAM0 + 32 * 48,
};

u8 *const sPlayerHealthboxMaxHpTextVram[2] =
{
    OBJ_VRAM0 + 32 * 90,
    OBJ_VRAM0 + 32 * 91,
};

u8 *const sOpponentHealthboxMaxHpTextVram[2] =
{
    OBJ_VRAM0 + 32 * 49,
    OBJ_VRAM0 + 32 * 50,
};

const u8 sEmptyDarkGrayHpText_RedHighlight[] = _("{COLOR DARK_GREY}{HIGHLIGHT RED}              /");

u8 *const sDoubleHealthboxCurrentHpTextVram[6] =
{
    OBJ_VRAM0 + 32 * 0,
    OBJ_VRAM0 + 32 * 1,
    OBJ_VRAM0 + 32 * 2,
    OBJ_VRAM0 + 32 * 3,
    OBJ_VRAM0 + 32 * 4,
    OBJ_VRAM0 + 32 * 5,
};

u8 *const sDoubleHealthboxMaxHpTextVram[2] =
{
    OBJ_VRAM0 + 32 * 6,
    OBJ_VRAM0 + 32 * 7,
};

const u8 sEmptyDarkGrayText_TransparentHighlight[] = __("{COLOR DARK_GREY}{HIGHLIGHT TRANSPARENT}              ");
const u8 sHealthboxRedHighlightText[] = _("{HIGHLIGHT RED}");

u8 *const sPlayerSingleHealthboxNicknameTextVram[10] =
{
    OBJ_VRAM0 + 32 *  2,
    OBJ_VRAM0 + 32 *  3,
    OBJ_VRAM0 + 32 *  4,
    OBJ_VRAM0 + 32 *  5,
    OBJ_VRAM0 + 32 *  6,
    OBJ_VRAM0 + 32 *  7,
    OBJ_VRAM0 + 32 * 64,
    OBJ_VRAM0 + 32 * 65,
    OBJ_VRAM0 + 32 * 66,
    OBJ_VRAM0 + 32 * 67,
};

u8 *const sOpponentHealthboxNicknameTextVram[10] =
{
    OBJ_VRAM0 + 32 *  1,
    OBJ_VRAM0 + 32 *  2,
    OBJ_VRAM0 + 32 *  3,
    OBJ_VRAM0 + 32 *  4,
    OBJ_VRAM0 + 32 *  5,
    OBJ_VRAM0 + 32 *  6,
    OBJ_VRAM0 + 32 *  7,
    OBJ_VRAM0 + 32 * 32,
    OBJ_VRAM0 + 32 * 33,
    OBJ_VRAM0 + 32 * 34,
};

u8 *const sPlayerDoubleHealthboxNicknameTextVram[10] =
{
    OBJ_VRAM0 + 32 *  2,
    OBJ_VRAM0 + 32 *  3,
    OBJ_VRAM0 + 32 *  4,
    OBJ_VRAM0 + 32 *  5,
    OBJ_VRAM0 + 32 *  6,
    OBJ_VRAM0 + 32 *  7,
    OBJ_VRAM0 + 32 * 32,
    OBJ_VRAM0 + 32 * 33,
    OBJ_VRAM0 + 32 * 34,
    OBJ_VRAM0 + 32 * 35,
};

extern u8 gDisplayedStringBattle[];
extern u8 gBattlersCount;
extern u16 gBattlerPartyIndexes[];
extern u8 gBattlerPositions[];
extern u8 gHealthboxSpriteIds[];

extern u16 gBattleTypeFlags;

extern const u8 BattleText_SafariBalls[];
extern const u8 BattleText_SafariBallsLeft[];
extern const u8 BattleText_HighlightRed[];
extern const u8 gHealthboxElementsGfxTable[][32];

extern const u16 gBattleInterfaceStatusIcons_DynPal[];

#define ABS(n) ((n) >= 0 ? (n) : -(n))
// Used for computing copy destination addresses
#define MACRO1(n) ((n) - (n) / 8 * 8) + 64 * ((n) / 8)

static void SpriteCB_HealthBoxOther(struct Sprite *);
static const void *GetHealthboxElementGfxPtr(u8 elementId);
static void UpdateHpTextInHealthboxInDoubles(u8 healthboxSpriteId, s16 value, u8 maxOrCurrent);
/*static*/ void UpdateStatusIconInHealthbox(u8);
extern void SpriteCB_StatusSummaryBalls_OnSwitchout(struct Sprite *);
static void SpriteCB_StatusSummaryBalls_Exit(struct Sprite *);
static void SpriteCB_StatusSummaryBar_Exit(struct Sprite *);
static void Task_HidePartyStatusSummary_DuringBattle(u8 taskId);
static void Task_HidePartyStatusSummary_BattleStart_1(u8 taskId);
static void Task_HidePartyStatusSummary_BattleStart_2(u8 taskId);
static u8 GetStatusIconForBattlerId(u8 statusElementId, u8 battlerId);
static int CalcNewBarValue(s32 maxValue, s32 oldValue, int receivedValue, int *currValue, u8 scale, u16 toAdd);
static u8 GetScaledExpFraction(int oldValue, int receivedValue, int maxValue, u8 scale);
static void MoveBattleBarGraphically(u8 battler, u8 whichBar);
static u8 CalcBarFilledPixels(int maxValue, int oldValue, int receivedValue, int *currValue, u8 *pixelsArray, u8 scale);
static void UpdateHealthBarTiles(struct HealthBarInfo *barInfo, int *currValue, u16 *dest);

static int DummiedOutFunction(s16 unused1, s16 unused2, int unused3)
{
    return 9;
}

void Debug_DrawNumber(s16 number, u16 *dest, bool8 unk)
{
    s8 i, j;
    u8 buff[4];

    for (i = 0; i < 4; i++)
    {
        buff[i] = 0;
    }

    for (i = 3; ; i--)
    {
        if (number > 0)
        {
            buff[i] = number % 10;
            number /= 10;
        }
        else
        {
            for (; i > -1; i--)
            {
                buff[i] = 0xFF;
            }
            if (buff[3] == 0xFF)
                buff[3] = 0;
            break;
        }
    }

    if (!unk)
    {
        for (i = 0, j = 0; i < 4; i++)
        {
            if (buff[j] == 0xFF)
            {
                dest[j + 0x00] &= 0xFC00;
                dest[j + 0x00] |= 0x1E;
                dest[i + 0x20] &= 0xFC00;
                dest[i + 0x20] |= 0x1E;
            }
            else
            {
                dest[j + 0x00] &= 0xFC00;
                dest[j + 0x00] |= 0x14 + buff[j];
                dest[i + 0x20] &= 0xFC00;
                dest[i + 0x20] |= 0x34 + buff[i];
            }
            j++;
        }
    }
    else
    {
        for (i = 0; i < 4; i++)
        {
            if (buff[i] == 0xFF)
            {
                dest[i + 0x00] &= 0xFC00;
                dest[i + 0x00] |= 0x1E;
                dest[i + 0x20] &= 0xFC00;
                dest[i + 0x20] |= 0x1E;
            }
            else
            {
                dest[i + 0x00] &= 0xFC00;
                dest[i + 0x00] |= 0x14 + buff[i];
                dest[i + 0x20] &= 0xFC00;
                dest[i + 0x20] |= 0x34 + buff[i];
            }
        }
    }
}

void Debug_DrawNumberPair(s16 number1, s16 number2, u16 *dest)
{
    dest[4] = 0x1E;
    Debug_DrawNumber(number2, dest, FALSE);
    Debug_DrawNumber(number1, dest + 5, TRUE);
}

u8 CreateBattlerHealthboxSprites(u8 battler)
{
    int healthbarOffsetType = 0;
    u8 healthboxLeftSpriteId;
    u8 healthboxRightSpriteId;
    u8 healthBarSpriteId;
    struct Sprite *healthBarSprite;

    if (!IsDoubleBattle())
    {
        if (GetBattlerSide(battler) == 0)
        {
            healthboxLeftSpriteId = CreateSprite(&sHealthboxPlayerSpriteTemplates[0], 240, 160, 1);
            healthboxRightSpriteId = CreateSpriteAtEnd(&sHealthboxPlayerSpriteTemplates[0], 240, 160, 1);

            gSprites[healthboxLeftSpriteId].oam.shape = 0;
            gSprites[healthboxRightSpriteId].oam.shape = 0;
            gSprites[healthboxRightSpriteId].oam.tileNum += 64;
        }
        else
        {
            healthboxLeftSpriteId = CreateSprite(&sHealthboxOpponentSpriteTemplates[0], 240, 160, 1);
            healthboxRightSpriteId = CreateSpriteAtEnd(&sHealthboxOpponentSpriteTemplates[0], 240, 160, 1);

            gSprites[healthboxRightSpriteId].oam.tileNum += 32;
            healthbarOffsetType = 2;
        }
        //_080439F2

        gSprites[healthboxLeftSpriteId].oam.affineParam = healthboxRightSpriteId;
        gSprites[healthboxRightSpriteId].hOther_HealthBoxSpriteId = healthboxLeftSpriteId;
        gSprites[healthboxRightSpriteId].callback = SpriteCB_HealthBoxOther;
    }
    //_08043A28
    else
    {
        if (GetBattlerSide(battler) == 0)
        {
            healthboxLeftSpriteId = CreateSprite(&sHealthboxPlayerSpriteTemplates[GetBattlerPosition(battler) / 2], 240, 160, 1);
            healthboxRightSpriteId = CreateSpriteAtEnd(&sHealthboxPlayerSpriteTemplates[GetBattlerPosition(battler) / 2], 240, 160, 1);

            gSprites[healthboxLeftSpriteId].oam.affineParam = healthboxRightSpriteId;
            gSprites[healthboxRightSpriteId].hOther_HealthBoxSpriteId = healthboxLeftSpriteId;
            gSprites[healthboxRightSpriteId].oam.tileNum += 32;
            gSprites[healthboxRightSpriteId].callback = SpriteCB_HealthBoxOther;
            healthbarOffsetType = 1;
        }
        //_08043ACC
        else
        {
            healthboxLeftSpriteId = CreateSprite(&sHealthboxOpponentSpriteTemplates[GetBattlerPosition(battler) / 2], 240, 160, 1);
            healthboxRightSpriteId = CreateSpriteAtEnd(&sHealthboxOpponentSpriteTemplates[GetBattlerPosition(battler) / 2], 240, 160, 1);

            gSprites[healthboxLeftSpriteId].oam.affineParam = healthboxRightSpriteId;
            gSprites[healthboxRightSpriteId].hOther_HealthBoxSpriteId = healthboxLeftSpriteId;
            gSprites[healthboxRightSpriteId].oam.tileNum += 32;
            gSprites[healthboxRightSpriteId].callback = SpriteCB_HealthBoxOther;
            healthbarOffsetType = 2;
        }
        //_08043B4E
    }
    //_08043B50

    healthBarSpriteId = CreateSpriteAtEnd(&sHealthbarSpriteTemplates[gBattlerPositions[battler]], 140, 60, 0);
    healthBarSprite = &gSprites[healthBarSpriteId];
    SetSubspriteTables(healthBarSprite, &sHealthBar_SubspriteTables[GetBattlerSide(battler)]);
    healthBarSprite->subspriteMode = 2;
    healthBarSprite->oam.priority = 1;
    CpuCopy32(GetHealthboxElementGfxPtr(1), OBJ_VRAM0 + healthBarSprite->oam.tileNum * 32, 64);

    gSprites[healthboxLeftSpriteId].hMain_HealthBarSpriteId = healthBarSpriteId;
    gSprites[healthboxLeftSpriteId].hMain_Battler = battler;
    gSprites[healthboxLeftSpriteId].invisible = TRUE;
    gSprites[healthboxRightSpriteId].invisible = TRUE;
    healthBarSprite->hBar_HealthBoxSpriteId = healthboxLeftSpriteId;
    healthBarSprite->hBar_Data6 = healthbarOffsetType;
    healthBarSprite->invisible = TRUE;

    return healthboxLeftSpriteId;
}

u8 CreateSafariPlayerHealthboxSprites(void)
{
    u8 healthboxLeftSpriteId = CreateSprite(&sHealthboxSafariSpriteTemplate, 240, 160, 1);
    u8 healthboxRightSpriteId = CreateSpriteAtEnd(&sHealthboxSafariSpriteTemplate, 240, 160, 1);

    gSprites[healthboxLeftSpriteId].oam.shape = 0;
    gSprites[healthboxRightSpriteId].oam.shape = 0;
    gSprites[healthboxRightSpriteId].oam.tileNum += 0x40;
    gSprites[healthboxLeftSpriteId].oam.affineParam = healthboxRightSpriteId;
    gSprites[healthboxRightSpriteId].hOther_HealthBoxSpriteId = healthboxLeftSpriteId;
    gSprites[healthboxRightSpriteId].callback = SpriteCB_HealthBoxOther;
    return healthboxLeftSpriteId;
}

static const void *GetHealthboxElementGfxPtr(u8 elementId)
{
    return gHealthboxElementsGfxTable[elementId];
}

static void SpriteCB_HealthBar(struct Sprite *sprite)
{
    u8 healthboxSpriteId = sprite->hBar_HealthBoxSpriteId;

    switch (sprite->hBar_Data6)
    {
    case 0:
        sprite->x = gSprites[healthboxSpriteId].x + 16;
        sprite->y = gSprites[healthboxSpriteId].y;
        break;
    case 1:
        sprite->x = gSprites[healthboxSpriteId].x + 16;
        sprite->y = gSprites[healthboxSpriteId].y;
        break;
    default:
    case 2:
        sprite->x = gSprites[healthboxSpriteId].x + 8;
        sprite->y = gSprites[healthboxSpriteId].y;
        break;
    }
    sprite->x2 = gSprites[healthboxSpriteId].x2;
    sprite->y2 = gSprites[healthboxSpriteId].y2;
}

static void SpriteCB_HealthBoxOther(struct Sprite *sprite)
{
    u8 healthboxMainSpriteId = sprite->hOther_HealthBoxSpriteId;

    sprite->x = gSprites[healthboxMainSpriteId].x + 64;
    sprite->y = gSprites[healthboxMainSpriteId].y;
    sprite->x2 = gSprites[healthboxMainSpriteId].x2;
    sprite->y2 = gSprites[healthboxMainSpriteId].y2;
}

void SetBattleBarStruct(u8 battler, u8 healthboxSpriteId, u32 maxValue, u32 oldValue, u32 receivedValue)
{
    sBattleBars[battler].healthboxSpriteId = healthboxSpriteId;
    sBattleBars[battler].maxValue = maxValue;
    sBattleBars[battler].oldValue = oldValue;
    sBattleBars[battler].receivedValue = receivedValue;
    sBattleBars[battler].currValue = -0x8000;
}

void SetHealthboxSpriteInvisible(u8 healthboxSpriteId)
{
    gSprites[healthboxSpriteId].invisible = TRUE;
    gSprites[gSprites[healthboxSpriteId].hMain_HealthBarSpriteId].invisible = TRUE;
    gSprites[gSprites[healthboxSpriteId].oam.affineParam].invisible = TRUE;
}

void SetHealthboxSpriteVisible(u8 healthboxSpriteId)
{
    gSprites[healthboxSpriteId].invisible = FALSE;
    gSprites[gSprites[healthboxSpriteId].hMain_HealthBarSpriteId].invisible = FALSE;
    gSprites[gSprites[healthboxSpriteId].oam.affineParam].invisible = FALSE;
}

static void UpdateSpritePos(u8 spriteId, s16 x, s16 y)
{
    gSprites[spriteId].x = x;
    gSprites[spriteId].y = y;
}

void DestoryHealthboxSprite(u8 healthboxSpriteId)
{
    DestroySprite(&gSprites[gSprites[healthboxSpriteId].oam.affineParam]);
    DestroySprite(&gSprites[gSprites[healthboxSpriteId].hMain_HealthBarSpriteId]);
    DestroySprite(&gSprites[healthboxSpriteId]);
}

void DummyBattleInterfaceFunc(u8 healthboxSpriteId, bool8 isDoubleBattleBattlerOnly)
{
}

void UpdateOamPriorityInAllHealthboxes(u8 priority)
{
    s32 i;

    for (i = 0; i < gBattlersCount; i++)
    {
        u8 healthboxLeftSpriteId;
        u8 healthboxRightSpriteId;
        u8 healthBarSpriteId;

        healthboxLeftSpriteId = gHealthboxSpriteIds[i];
        healthboxRightSpriteId = gSprites[healthboxLeftSpriteId].oam.affineParam;
        healthBarSpriteId = gSprites[healthboxLeftSpriteId].hMain_HealthBarSpriteId;
        gSprites[healthboxLeftSpriteId].oam.priority = priority;
        gSprites[healthboxRightSpriteId].oam.priority = priority;
        gSprites[healthBarSpriteId].oam.priority = priority;
    }
}

void InitBattlerHealthboxCoords(u8 battler)
{
    s16 x = 0;
    s16 y = 0;

    if (!IsDoubleBattle())
    {
        if (GetBattlerSide(battler) != 0)
        {
            x = 44;
            y = 30;
        }
        else
        {
            x = 158;
            y = 88;
        }
    }
    else
    {
        switch (GetBattlerPosition(battler))
        {
        case 0:
            x = 159;
            y = 77;
            break;
        case 2:
            x = 171;
            y = 102;
            break;
        case 1:
            x = 44;
            y = 19;
            break;
        case 3:
            x = 32;
            y = 44;
            break;
        }
    }
    UpdateSpritePos(gHealthboxSpriteIds[battler], x, y);
}

#if ENGLISH
#define CHAR_LV_SEPARATOR CHAR_COLON
#elif GERMAN
#define CHAR_LV_SEPARATOR CHAR_PERIOD
#endif

/*static*/ void UpdateLvlInHealthbox(u8 healthboxSpriteId, u8 lvl)
{
    u8 str[30];
    u8 *const *levelTextVram;
    u8 *ptr;
    s32 i;
    s32 numTiles;

    // TODO: Make this a local variable
    memcpy(str, sEmptyDarkGrayText_RedHighlight, sizeof(str));
    if (!IsDoubleBattle())
    {
        if (GetBattlerSide(gSprites[healthboxSpriteId].hMain_Battler) == 0)
            levelTextVram = sPlayerSingleHealthboxLevelTextVram;
        else
            levelTextVram = sOpponentHealthboxLevelTextVram;
    }
    else
    {
        if (GetBattlerSide(gSprites[healthboxSpriteId].hMain_Battler) == 0)
            levelTextVram = sPlayerDoubleHealthboxLevelTextVram;
        else
            levelTextVram = sOpponentHealthboxLevelTextVram;
    }

    ptr = str + 6;
    if (lvl == 100)
    {
        ptr = ConvertIntToDecimalStringN(ptr, 100, 0, 3);
    }
    else
    {
        *(ptr++) = EXT_CTRL_CODE_BEGIN;
        *(ptr++) = EXT_CTRL_CODE_CLEAR;
        *(ptr++) = 1;
        *(ptr++) = EXT_CTRL_CODE_BEGIN;
        *(ptr++) = EXT_CTRL_CODE_MIN_LETTER_SPACING;
        *(ptr++) = 4;
        *(ptr++) = CHAR_LV_SEPARATOR;
        *(ptr++) = EXT_CTRL_CODE_BEGIN;
        *(ptr++) = EXT_CTRL_CODE_MIN_LETTER_SPACING;
        *(ptr++) = 0;
        ptr = ConvertIntToDecimalStringN(ptr, lvl, 0, 2);
    }

    *(ptr++) = EXT_CTRL_CODE_BEGIN;
    *(ptr++) = EXT_CTRL_CODE_CLEAR_TO;
    *(ptr++) = 15;
    *(ptr++) = EOS;
    RenderTextHandleBold(sBattleInterfaceGfxBuffer, str);

    numTiles = 2;
    for (i = 0; i < numTiles; i++)
        CpuCopy32(&sBattleInterfaceGfxBuffer[i * 64 + 32], levelTextVram[i] + gSprites[healthboxSpriteId].oam.tileNum * TILE_SIZE_4BPP, TILE_SIZE_4BPP);
}

void UpdateHpTextInHealthbox(u8 healthboxSpriteId, s16 value, u8 maxOrCurrent)
{
    u8 str[0x14];
    u8 *ptr;
    s32 battler;
    u8 *const *hpTextVram;
    s32 i;
    u8 numTiles;

    // TODO: make this a local variable
    memcpy(str, sEmptyDarkGrayHpText_RedHighlight, sizeof(str));
    battler = gSprites[healthboxSpriteId].hMain_Battler;

    if (IsDoubleBattle() == TRUE || GetBattlerSide(battler) == 1)
    {
        //_08044136
        UpdateHpTextInHealthboxInDoubles(healthboxSpriteId, value, maxOrCurrent);
    }
    else
    {
        //
        ptr = str + 6;
        if (maxOrCurrent == 0)
        {
            if (GetBattlerSide(gSprites[healthboxSpriteId].hMain_Battler) == 0)
                hpTextVram = sPlayerHealthboxCurrentHpTextVram;
            else
                hpTextVram = sOpponentHealthboxCurrentHpTextVram;
            numTiles = 3;
            ptr = AlignBoldTextInteger(ptr, value, 19, 1);
            *(ptr++) = CHAR_SLASH;
            *(ptr++) = EOS;
        }
        else
        {
            if (GetBattlerSide(gSprites[healthboxSpriteId].hMain_Battler) == 0)
                hpTextVram = sPlayerHealthboxMaxHpTextVram;
            else
                hpTextVram = sOpponentHealthboxMaxHpTextVram;
            numTiles = 2;
            ptr = AlignBoldTextInteger(ptr, value, 15, 1);
        }
        RenderTextHandleBold(sBattleInterfaceGfxBuffer, str);
        //asm(""::"r"(healthboxSpriteId));
        //_080441B6
        for (i = 0; i < numTiles; i++)  // _080440BC
        {
            CpuCopy32(
                &sBattleInterfaceGfxBuffer[i * 64 + 32],
                hpTextVram[i] + gSprites[healthboxSpriteId].oam.tileNum * 32,
                0x20
            );
        }
    }
}

static void UpdateHpTextInHealthboxInDoubles(u8 healthboxSpriteId, s16 value, u8 maxOrCurrent)
{
    u8 str[0x14];
    u8 *ptr;
    u8 *const *hpTextVram;
    int numTiles;
    int battler;
    int i;

    // TODO: make this a local variable
    memcpy(str, sEmptyDarkGrayText_TransparentHighlight, sizeof(str));
    battler = gSprites[healthboxSpriteId].hMain_Battler;
    if (gBattleSpriteInfo[battler].hpNumbersNoBars == 0)
        return;
    ptr = str + 6;
    if (maxOrCurrent == 0)
    {
        hpTextVram = sDoubleHealthboxCurrentHpTextVram;
        numTiles = 6;
        ptr = AlignBoldTextInteger(ptr, value, 0x2B, 1);
        *(ptr++) = CHAR_SLASH;
        *(ptr++) = EOS;
    }
    else
    {
        hpTextVram = sDoubleHealthboxMaxHpTextVram;
        numTiles = 2;
        AlignBoldTextInteger(ptr, value, 0xF, 1);
        if (GetBattlerSide(battler) == 0)
        {
            CpuCopy32(GetHealthboxElementGfxPtr(0x74), OBJ_VRAM0 + (gSprites[healthboxSpriteId].oam.tileNum + 0x34) * 32, 32);
        }
    }
    {
        int healthBarSpriteId = gSprites[healthboxSpriteId].hMain_HealthBarSpriteId;

        RenderTextHandleBold(sBattleInterfaceGfxBuffer, str);
        for (i = 0; i < numTiles; i++)
        {
            CpuCopy32((void *)(&sBattleInterfaceGfxBuffer[i * 64 + 32]), hpTextVram[i] + gSprites[healthBarSpriteId].oam.tileNum * 32, 32);
        }
    }
}

void PrintSafariMonInfo(u8 healthboxSpriteId, struct Pokemon *mon)
{
    u8 text[20];
    s32 j;
    u8 *objVram;
    u8 *barFontGfx;
    u8 i, numTiles, nature, healthBarSpriteId;

    // TODO: make this a local variable
    memcpy(text, sEmptyDarkGrayHpText_RedHighlight, sizeof(text));
    barFontGfx = &sBattleInterfaceGfxBuffer[0x520 + GetBattlerPosition(gSprites[healthboxSpriteId].hMain_Battler) * 0x180];
    numTiles = 5;
    nature = GetNature(mon);
    StringCopy(text + 6, gNatureNamePointers[nature]);
    RenderTextHandleBold(barFontGfx, text);

    for (j = 6, i = 0; i < numTiles; i++, j++)
    {
        u8 elementId;

        if ((text[j] >= 55 && text[j] <= 74) || (text[j] >= 135 && text[j] <= 154))
            elementId = 44;
        else if ((text[j] >= 75 && text[j] <= 79) || (text[j] >= 155 && text[j] <= 159))
            elementId = 45;
        else
            elementId = 43;

        CpuCopy32(GetHealthboxElementGfxPtr(elementId), &barFontGfx[i * 64], 0x20);
    }

    for (j = 1; j < numTiles + 1; j++)
    {
        objVram = (gSprites[healthboxSpriteId].oam.tileNum + MACRO1(j)) * TILE_SIZE_4BPP + (void *)OBJ_VRAM0;
        CpuCopy32(barFontGfx, objVram, 32);
        barFontGfx += 32;

        objVram = (8 + gSprites[healthboxSpriteId].oam.tileNum + MACRO1(j)) * TILE_SIZE_4BPP + (void *)OBJ_VRAM0;
        CpuCopy32(barFontGfx, objVram, 32);
        barFontGfx += 32;
    }

    healthBarSpriteId = gSprites[healthboxSpriteId].hMain_HealthBarSpriteId;
    ConvertIntToDecimalStringN(text + 6, gBattleStruct->safariCatchFactor, 1, 2);
    ConvertIntToDecimalStringN(text + 9, gBattleStruct->safariEscapeFactor, 1, 2);
    text[5] = CHAR_SPACE;
    text[8] = CHAR_SLASH;
    RenderTextHandleBold(sBattleInterfaceGfxBuffer, text);

    for (j = 0; j < 5; j++)
    {
        if (j < 2)
        {
            CpuCopy32(
                &sBattleInterfaceGfxBuffer[j * 64 + 32],
                 ((gSprites[healthBarSpriteId].oam.tileNum + 2 + j) * TILE_SIZE_4BPP) + (void*)(OBJ_VRAM0),
                32
            );
        }
        else
        {
            CpuCopy32(
                &sBattleInterfaceGfxBuffer[j * 64 + 32],
                (void*)(OBJ_VRAM0) + (gSprites[healthBarSpriteId].oam.tileNum + 8 + j - 2) * TILE_SIZE_4BPP, // Must be written as "8 + j - 2" to match
                32
            );
        }
    }
}

extern u8 gIsDebugMode;

void SwapHpBarsWithHpText(void)
{
    s32 i;
    u8 healthBarSpriteId;

    for (i = 0; i < gBattlersCount; i++)
    {
        if (gSprites[gHealthboxSpriteIds[i]].callback == SpriteCallbackDummy
#if DEBUG
         && (gIsDebugMode != 0 || GetBattlerSide(i) != 1)
#else
         && GetBattlerSide(i) != 1
#endif
         && (IsDoubleBattle() || GetBattlerSide(i) != 0))
        {
            u8 hpNumbersNoBars;

            gBattleSpriteInfo[i].hpNumbersNoBars ^= 1;
            hpNumbersNoBars = gBattleSpriteInfo[i].hpNumbersNoBars;
            if (GetBattlerSide(i) == 0)
            {

                if (!IsDoubleBattle())
                    continue;
                if (gBattleTypeFlags & BATTLE_TYPE_SAFARI)
                    continue;

                if (hpNumbersNoBars == 1)
                {
                    healthBarSpriteId = gSprites[gHealthboxSpriteIds[i]].hMain_HealthBarSpriteId;

                    CpuFill32(0, OBJ_VRAM0 + gSprites[healthBarSpriteId].oam.tileNum * 32, 0x100);
                    UpdateHpTextInHealthboxInDoubles(gHealthboxSpriteIds[i], GetMonData(&gPlayerParty[gBattlerPartyIndexes[i]], MON_DATA_HP), 0);
                    UpdateHpTextInHealthboxInDoubles(gHealthboxSpriteIds[i], GetMonData(&gPlayerParty[gBattlerPartyIndexes[i]], MON_DATA_MAX_HP), 1);
                }
                else
                {
                    UpdateStatusIconInHealthbox(gHealthboxSpriteIds[i]);
                    UpdateHealthboxAttribute(gHealthboxSpriteIds[i], &gPlayerParty[gBattlerPartyIndexes[i]], 5);
                    CpuCopy32(GetHealthboxElementGfxPtr(0x75), OBJ_VRAM0 + 0x680 + gSprites[gHealthboxSpriteIds[i]].oam.tileNum * 32, 32);
                }
            }
            else
            {
                if (hpNumbersNoBars == 1)
                {
                    if (gBattleTypeFlags & BATTLE_TYPE_SAFARI)
                    {
                        PrintSafariMonInfo(gHealthboxSpriteIds[i], &gEnemyParty[gBattlerPartyIndexes[i]]);
                    }
                    else
                    {
                        healthBarSpriteId = gSprites[gHealthboxSpriteIds[i]].hMain_HealthBarSpriteId;

                        CpuFill32(0, OBJ_VRAM0 + gSprites[healthBarSpriteId].oam.tileNum * 32, 0x100);
                        UpdateHpTextInHealthboxInDoubles(gHealthboxSpriteIds[i], GetMonData(&gEnemyParty[gBattlerPartyIndexes[i]], MON_DATA_HP), 0);
                        UpdateHpTextInHealthboxInDoubles(gHealthboxSpriteIds[i], GetMonData(&gEnemyParty[gBattlerPartyIndexes[i]], MON_DATA_MAX_HP), 1);
                    }
                }
                else
                {
                    UpdateStatusIconInHealthbox(gHealthboxSpriteIds[i]);
                    UpdateHealthboxAttribute(gHealthboxSpriteIds[i], &gEnemyParty[gBattlerPartyIndexes[i]], 5);
                    if (gBattleTypeFlags & BATTLE_TYPE_SAFARI)
                        UpdateHealthboxAttribute(gHealthboxSpriteIds[i], &gEnemyParty[gBattlerPartyIndexes[i]], 4);
                }
            }
            gSprites[gHealthboxSpriteIds[i]].hMain_Data7 ^= 1;
        }
    }
}

#define tBattler                data[0]
#define tSummaryBarSpriteId     data[1]
#define tBallIconSpriteId(n)    data[3 + n]
#define tIsBattleStart          data[10]
#define tBlend                  data[15]

u8 CreatePartyStatusSummarySprites(u8 battler, const struct HpAndStatus *partyInfo, u8 skipPlayer, u8 isBattleStart)
{
    u8 isOpponent;
    s16 x;
    s16 y;
    s16 barPos2X;
    s16 barData0;

    int i, j;
    u8 ballIconSpriteIds[6];
    s8 partyCount;
    u8 summaryBarSpriteId;
    u8 taskId;

    if (skipPlayer == 0 || GetBattlerPosition(battler) != 3)
    {
        if (GetBattlerSide(battler) == 0)
        {
            isOpponent = 0;
            x = 136;
            y = 96;
            barPos2X = 100;
            barData0 = -5;
        }
        else
        {
            isOpponent = 1;
            if (skipPlayer == 0 || !IsDoubleBattle())
            {
                x = 104;
                y = 40;
            }
            else
            {
                x = 104;
                y = 16;
            }
            barPos2X = -100;
            barData0 = 5;
        }
    }
    else
    {
        isOpponent = 1;
        x = 104;
        y = 40;
        barPos2X = -100;
        barData0 = 5;
    }
    //_08044884

    for (i = 0, partyCount = 0; i < 6; i++)  //_080448A0
    {
        if (partyInfo[i].hp != 0xFFFF)
            partyCount++;
    }

    LoadCompressedSpriteSheet(&sStatusSummaryBarSpriteSheets[isOpponent]);
    LoadSpriteSheet(&sStatusSummaryBallsSpriteSheets[isOpponent]);
    LoadSpritePalette(&sStatusSummaryBarSpritePals[isOpponent]);
    LoadSpritePalette(&sStatusSummaryBallsSpritePals[isOpponent]);

    summaryBarSpriteId = CreateSprite(&sStatusSummaryBarSpriteTemplates[isOpponent], x, y, 10);
    SetSubspriteTables(&gSprites[summaryBarSpriteId], sStatusSummaryBar_SubspriteTable_Enter);
    gSprites[summaryBarSpriteId].x2 = barPos2X;
    gSprites[summaryBarSpriteId].data[0] = barData0;
    if (isOpponent != 0)
    {
        gSprites[summaryBarSpriteId].x -= 96;
        gSprites[summaryBarSpriteId].oam.matrixNum = 8;
    }
    else
    {
        gSprites[summaryBarSpriteId].x += 0x60;
    }
    //_0804495A
    for (i = 0; i < 6; i++)  //_08044970
    {
        ballIconSpriteIds[i] = CreateSpriteAtEnd(&sStatusSummaryBallsSpriteTemplates[isOpponent], x, y - 4, 9);
        if (isBattleStart == 0)
        {
            gSprites[ballIconSpriteIds[i]].callback = SpriteCB_StatusSummaryBalls_OnSwitchout;
        }
        //_080449A0
        if (isOpponent == 0)
        {
            gSprites[ballIconSpriteIds[i]].x2 = 0;
            gSprites[ballIconSpriteIds[i]].y2 = 0;
        }
        //_080449BE
        gSprites[ballIconSpriteIds[i]].data[0] = summaryBarSpriteId;
        if (isOpponent == 0)
        {
            gSprites[ballIconSpriteIds[i]].x += 10 * i + 24;
            gSprites[ballIconSpriteIds[i]].data[1] = 7 * i + 10;
            gSprites[ballIconSpriteIds[i]].x2 = 120;
        }
        //_08044A18
        else
        {
            gSprites[ballIconSpriteIds[i]].x -= 10 * (5 - i) + 24;
            gSprites[ballIconSpriteIds[i]].data[1] = 7 * (6 - i) + 10;
            gSprites[ballIconSpriteIds[i]].x2 = -120;
        }
        //_08044A56
        gSprites[ballIconSpriteIds[i]].data[2] = isOpponent;
    }
    //_08044A76
    if (GetBattlerSide(battler) == 0)
    {
        for (i = 0; i < 6; i++)  //_08044A9A
        {
            if (gBattleTypeFlags & BATTLE_TYPE_MULTI) // && partyInfo[i] != 0xFFFF && partyInfo[i]
            {
                if (partyInfo[i].hp == 0xFFFF)
                {
                    //_08044AE6
                    gSprites[ballIconSpriteIds[i]].oam.tileNum++;
                    gSprites[ballIconSpriteIds[i]].data[7] = 1;
                    // to _08044B52
                }
                else if (partyInfo[i].hp == 0)
                {
                    gSprites[ballIconSpriteIds[i]].oam.tileNum += 3;
                    // to _08044B46
                }
                else if (partyInfo[i].status != 0)
                {
                    gSprites[ballIconSpriteIds[i]].oam.tileNum += 2;
                }
            }
            //_08044ADC
            else
            {
                if (i >= partyCount)
                {
                    //_08044AE6
                    gSprites[ballIconSpriteIds[i]].oam.tileNum++;
                    gSprites[ballIconSpriteIds[i]].data[7] = 1;
                    // to _08044B52
                }
                else if (partyInfo[i].hp == 0)
                {
                    //_08044B14
                    gSprites[ballIconSpriteIds[i]].oam.tileNum += 3;
                    // to _08044B46
                }
                else if (partyInfo[i].status != 0)
                {
                    gSprites[ballIconSpriteIds[i]].oam.tileNum += 2;
                }
            }
        }
    }
    //_08044B5E
    else
    {
        // Mismatch occurrs in this loop initialization
        j = 5;
        for (i = 0; i < 6; i++)
        {
            if (gBattleTypeFlags & BATTLE_TYPE_MULTI)
            {
                if (partyInfo[i].hp == 0xFFFF)
                {
                    gSprites[ballIconSpriteIds[j]].oam.tileNum++;
                    gSprites[ballIconSpriteIds[j]].data[7] = 1;
                }
                else if (partyInfo[i].hp == 0)
                {
                    gSprites[ballIconSpriteIds[j]].oam.tileNum += 3;
                }
                else if (partyInfo[i].status != 0)
                {
                    gSprites[ballIconSpriteIds[j]].oam.tileNum += 2;
                }
                j--;
            }
            else
            {
                if (i >= partyCount)
                {
                    gSprites[ballIconSpriteIds[j]].oam.tileNum++;
                    gSprites[ballIconSpriteIds[j]].data[7] = 1;
                }
                else if (partyInfo[i].hp == 0)
                {
                    gSprites[ballIconSpriteIds[j]].oam.tileNum += 3;
                }
                else if (partyInfo[i].status != 0)
                {
                    gSprites[ballIconSpriteIds[j]].oam.tileNum += 2;
                }
                j--;
            }
        }
    }
    //_08044C38
    taskId = CreateTask(TaskDummy, 5);
    gTasks[taskId].tBattler = battler;
    gTasks[taskId].tSummaryBarSpriteId = summaryBarSpriteId;
    for (i = 0; i < 6; i++)
        gTasks[taskId].tBallIconSpriteId(i) = ballIconSpriteIds[i];
    gTasks[taskId].tIsBattleStart = isBattleStart;
    PlaySE12WithPanning(SE_BALL_TRAY_ENTER, 0);
    return taskId;
}

void Task_HidePartyStatusSummary(u8 taskId)
{
    u8 ballIconSpriteIds[6];
    u8 isBattleStart;
    u8 summaryBarSpriteId;
    u8 battler;
    s32 i;

    isBattleStart = gTasks[taskId].tIsBattleStart;
    summaryBarSpriteId = gTasks[taskId].tSummaryBarSpriteId;
    battler = gTasks[taskId].tBattler;
    for (i = 0; i < 6; i++)
        ballIconSpriteIds[i] = gTasks[taskId].tBallIconSpriteId(i);

    REG_BLDCNT = 0x3F40;
    REG_BLDALPHA = 0x10;
    gTasks[taskId].tBlend = 16;
    for (i = 0; i < 6; i++)
        gSprites[ballIconSpriteIds[i]].oam.objMode = 1;
    gSprites[summaryBarSpriteId].oam.objMode = 1;
    if (isBattleStart != 0)
    {
        for (i = 0; i < 6; i++)
        {
            if (GetBattlerSide(battler) != 0)
            {
                gSprites[ballIconSpriteIds[5 - i]].data[1] = 7 * i;
                gSprites[ballIconSpriteIds[5 - i]].data[3] = 0;
                gSprites[ballIconSpriteIds[5 - i]].data[4] = 0;
                gSprites[ballIconSpriteIds[5 - i]].callback = SpriteCB_StatusSummaryBalls_Exit;
            }
            else
            {
                gSprites[ballIconSpriteIds[i]].data[1] = 7 * i;
                gSprites[ballIconSpriteIds[i]].data[3] = 0;
                gSprites[ballIconSpriteIds[i]].data[4] = 0;
                gSprites[ballIconSpriteIds[i]].callback = SpriteCB_StatusSummaryBalls_Exit;
            }
        }
        gSprites[summaryBarSpriteId].data[0] /= 2;
        gSprites[summaryBarSpriteId].data[1] = 0;
        gSprites[summaryBarSpriteId].callback = SpriteCB_StatusSummaryBar_Exit;
        SetSubspriteTables(&gSprites[summaryBarSpriteId], sStatusSummaryBar_SubspriteTable_Exit);
        gTasks[taskId].func = Task_HidePartyStatusSummary_BattleStart_1;
    }
    else
    {
        gTasks[taskId].func = Task_HidePartyStatusSummary_DuringBattle;
    }
}

static void Task_HidePartyStatusSummary_BattleStart_1(u8 taskId)
{
    u16 temp = gTasks[taskId].data[11]++;

    if ((temp & 1) == 0)
    {
        gTasks[taskId].tBlend--;
        if (gTasks[taskId].tBlend < 0)
            return;
        REG_BLDALPHA = (gTasks[taskId].tBlend) | ((16 - gTasks[taskId].tBlend) << 8);
    }
    if (gTasks[taskId].tBlend == 0)
        gTasks[taskId].func = Task_HidePartyStatusSummary_BattleStart_2;
}

static void Task_HidePartyStatusSummary_BattleStart_2(u8 taskId)
{
    u8 ballIconSpriteIds[6];
    s32 i;

    gTasks[taskId].tBlend--;
    if (gTasks[taskId].tBlend == -1)
    {
        u8 summaryBarSpriteId = gTasks[taskId].tSummaryBarSpriteId;

        for (i = 0; i < 6; i++)
            ballIconSpriteIds[i] = gTasks[taskId].tBallIconSpriteId(i);
        DestroySpriteAndFreeResources(&gSprites[summaryBarSpriteId]);
        DestroySpriteAndFreeResources(&gSprites[ballIconSpriteIds[0]]);
        for (i = 1; i < 6; i++)
            DestroySprite(&gSprites[ballIconSpriteIds[i]]);
    }
    else if (gTasks[taskId].tBlend == -3)
    {
        REG_BLDCNT = 0;
        REG_BLDALPHA = 0;
        DestroyTask(taskId);
    }
}

static void Task_HidePartyStatusSummary_DuringBattle(u8 taskId)
{
    u8 ballIconSpriteIds[6];
    s32 i;

    gTasks[taskId].tBlend--;
    // Same as above function except with this check.
    if (gTasks[taskId].tBlend >= 0)
    {
        REG_BLDALPHA = (gTasks[taskId].tBlend) | ((16 - gTasks[taskId].tBlend) << 8);
    }
    else if (gTasks[taskId].tBlend == -1)
    {
        u8 summaryBarSpriteId = gTasks[taskId].tSummaryBarSpriteId;

        for (i = 0; i < 6; i++)
            ballIconSpriteIds[i] = gTasks[taskId].tBallIconSpriteId(i);
        DestroySpriteAndFreeResources(&gSprites[summaryBarSpriteId]);
        DestroySpriteAndFreeResources(&gSprites[ballIconSpriteIds[0]]);
        for (i = 1; i < 6; i++)
            DestroySprite(&gSprites[ballIconSpriteIds[i]]);
    }
    else if (gTasks[taskId].tBlend == -3)
    {
        REG_BLDCNT = 0;
        REG_BLDALPHA = 0;
        DestroyTask(taskId);
    }
}

#undef tBattler
#undef tSummaryBarSpriteId
#undef tBallIconSpriteId
#undef tIsBattleStart
#undef tBlend

static void SpriteCB_StatusSummaryBar_Enter(struct Sprite *sprite)
{
    if (sprite->x2 != 0)
        sprite->x2 += sprite->data[0];
}

static void SpriteCB_StatusSummaryBar_Exit(struct Sprite *sprite)
{
    sprite->data[1] += 32;
    if (sprite->data[0] > 0)
        sprite->x2 += sprite->data[1] >> 4;
    else
        sprite->x2 -= sprite->data[1] >> 4;
    sprite->data[1] &= 0xF;
}

static void SpriteCB_StatusSummaryBalls_Enter(struct Sprite *sprite)
{
    u8 isOpponent;
    u16 movementOffset;
    s8 pan;

    if (sprite->data[1] > 0)
    {
        sprite->data[1]--;
        return;
    }
    isOpponent = sprite->data[2];
    movementOffset = sprite->data[3];
    movementOffset += 56;
    sprite->data[3] = movementOffset & 0xFFF0;
    if (isOpponent != 0)
    {
        sprite->x2 += movementOffset >> 4;
        if (sprite->x2 > 0)
            sprite->x2 = 0;
    }
    else
    {
        sprite->x2 -= movementOffset >> 4;
        if (sprite->x2 < 0)
            sprite->x2 = 0;
    }
    if (sprite->x2 == 0)
    {
        pan = 63;
        if (isOpponent != 0)
            pan = -64;
        if (sprite->data[7] != 0)
            PlaySE2WithPanning(SE_BALL_TRAY_EXIT, pan);
        else
            PlaySE1WithPanning(SE_BALL_TRAY_BALL, pan);
        sprite->callback = SpriteCallbackDummy;
    }
}

static void SpriteCB_StatusSummaryBalls_Exit(struct Sprite *sprite)
{
    u8 isOpponent;
    u16 movementOffset;

    if (sprite->data[1] > 0)
    {
        sprite->data[1]--;
        return;
    }
    isOpponent = sprite->data[2];
    movementOffset = sprite->data[3];
    movementOffset += 56;
    sprite->data[3] = movementOffset & 0xFFF0;
    if (isOpponent != 0)
        sprite->x2 += movementOffset >> 4;
    else
        sprite->x2 -= movementOffset >> 4;
    if (sprite->x2 + sprite->x > 248
     || sprite->x2 + sprite->x < -8)
    {
        sprite->invisible = TRUE;
        sprite->callback = SpriteCallbackDummy;
    }
}

void SpriteCB_StatusSummaryBalls_OnSwitchout(struct Sprite *sprite)
{
    u8 summaryBarSpriteId = sprite->data[0];

    sprite->x2 = gSprites[summaryBarSpriteId].x2;
    sprite->y2 = gSprites[summaryBarSpriteId].y2;
}

/*static*/ void UpdateNickInHealthbox(u8 healthboxSpriteId, struct Pokemon *mon)
{
    u8 nickname[POKEMON_NAME_LENGTH];
    u8 gender;
    u16 species;
    u8 language;
    u8 *ptr;
    s32 i;
    s32 numTiles;
    u8 *const *nicknameTextVram;

    StringCopy(gDisplayedStringBattle, sHealthboxRedHighlightText);
    GetMonData(mon, MON_DATA_NICKNAME, nickname);
    StringGet_Nickname(nickname);
    ptr = StringCopy(gDisplayedStringBattle + 3, nickname);
    ptr[0] = EXT_CTRL_CODE_BEGIN;
    ptr[1] = 3;
    ptr[2] = 2;
    ptr[3] = EXT_CTRL_CODE_BEGIN;
    ptr[4] = 1;
    ptr += 5;
    gender = GetMonGender(mon);
    species = GetMonData(mon, MON_DATA_SPECIES);
    language = GetMonData(mon, MON_DATA_LANGUAGE);
    if (ShouldHideGenderIconForLanguage(species, nickname, language))
        gender = 100;
    switch (gender)
    {
    default:
        ptr[0] = 0xB;
        ptr[1] = EOS;
        ptr += 1;
        break;
    case MON_MALE:
        ptr[0] = 0xB;
        ptr[1] = CHAR_MALE;
        ptr[2] = EOS;
        ptr += 2;
        break;
    case MON_FEMALE:
        ptr[0] = 0xA;
        ptr[1] = CHAR_FEMALE;
        ptr[2] = EOS;
        ptr += 2;
        break;
    }
    ptr[0] = EXT_CTRL_CODE_BEGIN;
    ptr[1] = 0x13;
    ptr[2] = 0x37;
    ptr[3] = EOS;
    ptr = &sBattleInterfaceGfxBuffer[0x520 + GetBattlerPosition(gSprites[healthboxSpriteId].hMain_Battler) * 0x180];
    RenderTextHandleBold(ptr, gDisplayedStringBattle);

    i = 0;
    numTiles = 7;
    if (GetMonData(mon, MON_DATA_LANGUAGE) == 1
     && GetMonData(mon, MON_DATA_IS_EGG) == 0)
    {
        u8 *p = gDisplayedStringBattle;

        while (*p != EOS)
        {
            if (*p == EXT_CTRL_CODE_BEGIN)
            {
                p += GetExtCtrlCodeLength(p[1]) + 1;
            }
            else
            {
                u8 elementId;

                if ((*p >= 0x37 && *p <= 0x4A) || (*p >= 0x87 && *p <= 0x9A))
                    elementId = 0x2C;
                else if ((*p >= 0x4B && *p <= 0x4F) || (*p >= 0x9B && *p <= 0x9F))
                    elementId = 0x2D;
                else
                    elementId = 0x2B;

                CpuCopy32(GetHealthboxElementGfxPtr(elementId), ptr + 0x40 * i, 32);
                i++;
                p++;
            }
        }
    }

    for (; i < numTiles; i++)
        CpuCopy32(GetHealthboxElementGfxPtr(0x2B), ptr + 64 * i, 32);

    if (GetBattlerSide(gSprites[healthboxSpriteId].hMain_Battler) == 0 && !IsDoubleBattle())
    {
        nicknameTextVram = (u8 *const *)sPlayerSingleHealthboxNicknameTextVram;
        for (i = 0; i < numTiles; i++)
        {
            u8 *dest = nicknameTextVram[i];

            dest += gSprites[healthboxSpriteId].oam.tileNum * 32;
            CpuCopy32(ptr, dest, 32);
            ptr += 32;

            dest += 0x100;
            CpuCopy32(ptr, dest, 32);
            ptr += 32;
        }
    }
    else
    {
        if (GetBattlerSide(gSprites[healthboxSpriteId].hMain_Battler) == 0)
            nicknameTextVram = (u8 *const *)sPlayerDoubleHealthboxNicknameTextVram;
        else
            nicknameTextVram = (u8 *const *)sOpponentHealthboxNicknameTextVram;
        for (i = 0; i < numTiles; i++)
        {
            u8 *dest = nicknameTextVram[i];

            dest += gSprites[healthboxSpriteId].oam.tileNum * 32;
            CpuCopy32(ptr, dest, 32);
            ptr += 32;

            dest += 0x100;
            CpuCopy32(ptr, dest, 32);
            ptr += 32;
        }
    }
}

static void TryAddPokeballIconToHealthbox(u8 healthboxSpriteId, u8 noStatus)
{
    u8 battler;

    if (gBattleTypeFlags & 0x200)
        return;
    if (gBattleTypeFlags & 8)
        return;

    battler = gSprites[healthboxSpriteId].hMain_Battler;
    if (GetBattlerSide(battler) != 0)
    {
        u16 species = GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_SPECIES);
        if (GetSetPokedexFlag(SpeciesToNationalPokedexNum(species), 1) != 0)
        {
            u8 healthBarSpriteId = gSprites[healthboxSpriteId].hMain_HealthBarSpriteId;

            if (noStatus != 0)
                CpuCopy32(GetHealthboxElementGfxPtr(0x46), OBJ_VRAM0 + (gSprites[healthBarSpriteId].oam.tileNum + 8) * 32, 32);
            else
                CpuFill32(0, OBJ_VRAM0 + (gSprites[healthBarSpriteId].oam.tileNum + 8) * 32, 32);
        }
    }
}

/*static*/ void UpdateStatusIconInHealthbox(u8 healthboxSpriteId)
{
    s32 status;
    s32 paletteOffset;
    u8 battler;
    u8 healthBarSpriteId;
    s16 tileNumOffset;
    const u8 *statusGfxPtr;
    u8 statusPaletteId;
    s32 i;

    battler = gSprites[healthboxSpriteId].hMain_Battler;
    healthBarSpriteId = gSprites[healthboxSpriteId].hMain_HealthBarSpriteId;
    if (GetBattlerSide(battler) == 0)
    {
        status = GetMonData(&gPlayerParty[gBattlerPartyIndexes[battler]], MON_DATA_STATUS);
        if (!IsDoubleBattle())
            tileNumOffset = 0x1A;
        else
            tileNumOffset = 0x12;
    }
    else
    {
        status = GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_STATUS);
        tileNumOffset = 0x11;
    }
    if (status & 7)
    {
        statusGfxPtr = GetHealthboxElementGfxPtr(GetStatusIconForBattlerId(0x1B, battler));
        statusPaletteId = 2;
    }
    else if (status & 0x88)
    {
        statusGfxPtr = GetHealthboxElementGfxPtr(GetStatusIconForBattlerId(0x15, battler));
        statusPaletteId = 0;
    }
    else if (status & 0x10)
    {
        statusGfxPtr = GetHealthboxElementGfxPtr(GetStatusIconForBattlerId(0x21, battler));
        statusPaletteId = 4;
    }
    else if (status & 0x20)
    {
        statusGfxPtr = GetHealthboxElementGfxPtr(GetStatusIconForBattlerId(0x1E, battler));
        statusPaletteId = 3;
    }
    else if (status & 0x40)
    {
        statusGfxPtr = GetHealthboxElementGfxPtr(GetStatusIconForBattlerId(0x18, battler));
        statusPaletteId = 1;
    }
    else
    {
        statusGfxPtr = GetHealthboxElementGfxPtr(0x27);

        for (i = 0; i < 3; i++)
            CpuCopy32(statusGfxPtr, OBJ_VRAM0 + (gSprites[healthboxSpriteId].oam.tileNum + tileNumOffset + i) * 32, 32);

        if (!gBattleSpriteInfo[battler].hpNumbersNoBars)
            CpuCopy32(GetHealthboxElementGfxPtr(1), OBJ_VRAM0 + gSprites[healthBarSpriteId].oam.tileNum * 32, 64);

        TryAddPokeballIconToHealthbox(healthboxSpriteId, 1);
        return;
    }

    paletteOffset = gSprites[healthboxSpriteId].oam.paletteNum * 16;
    paletteOffset += battler + 12;
    // I don't like writing the array index like this, but I can't get it to match otherwise.
    FillPalette(statusPaletteId[gBattleInterfaceStatusIcons_DynPal], paletteOffset + 0x100, 2);
    CpuCopy16(gPlttBufferUnfaded + 0x100 + paletteOffset, (void *)(OBJ_PLTT + paletteOffset * 2), 2);
    CpuCopy32(statusGfxPtr, OBJ_VRAM0 + (gSprites[healthboxSpriteId].oam.tileNum + tileNumOffset) * 32, 96);
    if (IsDoubleBattle() == TRUE || GetBattlerSide(battler) == TRUE)
    {
        if (!gBattleSpriteInfo[battler].hpNumbersNoBars)
        {
            CpuCopy32(GetHealthboxElementGfxPtr(0), OBJ_VRAM0 + gSprites[healthBarSpriteId].oam.tileNum * 32, 32);
            CpuCopy32(GetHealthboxElementGfxPtr(0x41), OBJ_VRAM0 + (gSprites[healthBarSpriteId].oam.tileNum + 1) * 32, 32);
        }
    }
    TryAddPokeballIconToHealthbox(healthboxSpriteId, 0);
}

static u8 GetStatusIconForBattlerId(u8 statusElementId, u8 battlerId)
{
    u8 ret = statusElementId;

    switch (statusElementId)
    {
    case 21:
        if (battlerId == 0)
            ret = 21;
        else if (battlerId == 1)
            ret = 71;
        else if (battlerId == 2)
            ret = 86;
        else
            ret = 101;
        break;
    case 24:
        if (battlerId == 0)
            ret = 24;
        else if (battlerId == 1)
            ret = 74;
        else if (battlerId == 2)
            ret = 89;
        else
            ret = 104;
        break;
    case 27:
        if (battlerId == 0)
            ret = 27;
        else if (battlerId == 1)
            ret = 77;
        else if (battlerId == 2)
            ret = 92;
        else
            ret = 107;
        break;
    case 30:
        if (battlerId == 0)
            ret = 30;
        else if (battlerId == 1)
            ret = 80;
        else if (battlerId == 2)
            ret = 95;
        else
            ret = 110;
        break;
    case 33:
        if (battlerId == 0)
            ret = 33;
        else if (battlerId == 1)
            ret = 83;
        else if (battlerId == 2)
            ret = 98;
        else
            ret = 113;
        break;
    }
    return ret;
}

/*static*/ void UpdateSafariBallsTextOnHealthbox(u8 healthboxSpriteId)
{
    u8 *barFontGfx;
    u8 numTiles;
    u8 i;
    s32 tileIndex;
    u8 *objVram;

    barFontGfx = &sBattleInterfaceGfxBuffer[0x520 + GetBattlerPosition(gSprites[healthboxSpriteId].hMain_Battler) * 0x180];
    numTiles = 7;
    RenderTextHandleBold(barFontGfx, BattleText_SafariBalls);
    for (i = 0; i < numTiles; i++)
        CpuCopy32(GetHealthboxElementGfxPtr(0x2B), barFontGfx + i * 64, 32);
    for (tileIndex = 3; tileIndex < 3 + numTiles; tileIndex++)
    {
        objVram = OBJ_VRAM0 + (gSprites[healthboxSpriteId].oam.tileNum + MACRO1(tileIndex)) * 32;
        CpuCopy32(barFontGfx, objVram, 32);
        barFontGfx += 32;

        objVram = OBJ_VRAM0 + (8 + gSprites[healthboxSpriteId].oam.tileNum + MACRO1(tileIndex)) * 32;
        CpuCopy32(barFontGfx, objVram, 32);
        barFontGfx += 32;
    }

}

/*static*/ void UpdateLeftNoOfBallsTextOnHealthbox(u8 healthboxSpriteId)
{
    u8 *ptr;
    u8 battlerPosition;
    s32 numTiles;
    s32 i;

    ptr = StringCopy(gDisplayedStringBattle, BattleText_SafariBallsLeft);
    ptr = AlignBoldTextInteger(ptr, gNumSafariBalls, 10, 1);
    StringAppend(ptr, BattleText_HighlightRed);
    battlerPosition = GetBattlerPosition(gSprites[healthboxSpriteId].hMain_Battler);
    ptr = &sBattleInterfaceGfxBuffer[0x520 + battlerPosition * 0x180];
    numTiles = 5;
    RenderTextHandleBold(ptr, gDisplayedStringBattle);
    ptr = &sBattleInterfaceGfxBuffer[0x520 + battlerPosition * 0x180 + 32];
    for (i = 6; i < 6 + numTiles; i++)
    {
        CpuCopy32(ptr, OBJ_VRAM0 + (gSprites[healthboxSpriteId].oam.tileNum + 0x18 + MACRO1(i)) * 32, 32);
        ptr += 64;
    }
}

void UpdateHealthboxAttribute(u8 healthboxSpriteId, struct Pokemon *mon, u8 elementId)
{
    u8 battler;
    u32 maxHp;
    u32 currHp;

    battler = gSprites[healthboxSpriteId].hMain_Battler;
    if (GetBattlerSide(battler) == 0)
    {
        if (elementId == HEALTHBOX_LEVEL || elementId == HEALTHBOX_ALL)
            UpdateLvlInHealthbox(healthboxSpriteId, GetMonData(mon, MON_DATA_LEVEL));
        if (elementId == HEALTHBOX_CURRENT_HP || elementId == HEALTHBOX_ALL)
            UpdateHpTextInHealthbox(healthboxSpriteId, GetMonData(mon, MON_DATA_HP), HP_CURRENT);
        if (elementId == HEALTHBOX_MAX_HP || elementId == HEALTHBOX_ALL)
            UpdateHpTextInHealthbox(healthboxSpriteId, GetMonData(mon, MON_DATA_MAX_HP), HP_MAX);
        if (elementId == HEALTHBOX_HEALTH_BAR || elementId == HEALTHBOX_ALL)
        {
            LoadBattleBarGfx(0);
            maxHp = GetMonData(mon, MON_DATA_MAX_HP);
            currHp = GetMonData(mon, MON_DATA_HP);
            SetBattleBarStruct(battler, healthboxSpriteId, maxHp, currHp, 0);
            MoveBattleBar(battler, healthboxSpriteId, HEALTH_BAR, 0);
        }
        if (!IsDoubleBattle() && (elementId == HEALTHBOX_EXP_BAR || elementId == HEALTHBOX_ALL))
        {
            u16 species;
            u8 level;
            u32 exp;
            u32 currExpBarValue;
            u32 maxExpBarValue;
            u32 currLevelExp;

            LoadBattleBarGfx(3);
            species = GetMonData(mon, MON_DATA_SPECIES);
            level = GetMonData(mon, MON_DATA_LEVEL);
            exp = GetMonData(mon, MON_DATA_EXP);
            currLevelExp = gExperienceTables[gBaseStats[species].growthRate][level];
            currExpBarValue = exp - currLevelExp;
            maxExpBarValue = gExperienceTables[gBaseStats[species].growthRate][level + 1] - currLevelExp;
            SetBattleBarStruct(battler, healthboxSpriteId, maxExpBarValue, currExpBarValue, 0);
            MoveBattleBar(battler, healthboxSpriteId, EXP_BAR, 0);
        }
        if (elementId == HEALTHBOX_NICK || elementId == HEALTHBOX_ALL)
            UpdateNickInHealthbox(healthboxSpriteId, mon);
        if (elementId == HEALTHBOX_STATUS_ICON || elementId == HEALTHBOX_ALL)
            UpdateStatusIconInHealthbox(healthboxSpriteId);
        if (elementId == HEALTHBOX_SAFARI_ALL_TEXT)
            UpdateSafariBallsTextOnHealthbox(healthboxSpriteId);
        if (elementId == HEALTHBOX_SAFARI_ALL_TEXT || elementId == HEALTHBOX_SAFARI_BALLS_TEXT)
            UpdateLeftNoOfBallsTextOnHealthbox(healthboxSpriteId);
    }
    else
    {
        if (elementId == HEALTHBOX_LEVEL || elementId == HEALTHBOX_ALL)
            UpdateLvlInHealthbox(healthboxSpriteId, GetMonData(mon, MON_DATA_LEVEL));
#if DEBUG
        if (gIsDebugMode == 1)
        {
            if (elementId == HEALTHBOX_CURRENT_HP || elementId == HEALTHBOX_ALL)
                UpdateHpTextInHealthbox(healthboxSpriteId, GetMonData(mon, MON_DATA_HP), HP_CURRENT);
            if (elementId == HEALTHBOX_MAX_HP || elementId == HEALTHBOX_ALL)
                UpdateHpTextInHealthbox(healthboxSpriteId, GetMonData(mon, MON_DATA_MAX_HP), HP_MAX);
        }
#endif
        if (elementId == HEALTHBOX_HEALTH_BAR || elementId == HEALTHBOX_ALL)
        {
            LoadBattleBarGfx(0);
            maxHp = GetMonData(mon, MON_DATA_MAX_HP);
            currHp = GetMonData(mon, MON_DATA_HP);
            SetBattleBarStruct(battler, healthboxSpriteId, maxHp, currHp, 0);
            MoveBattleBar(battler, healthboxSpriteId, HEALTH_BAR, 0);
        }
        if (elementId == HEALTHBOX_NICK || elementId == HEALTHBOX_ALL)
            UpdateNickInHealthbox(healthboxSpriteId, mon);
        if (elementId == HEALTHBOX_STATUS_ICON || elementId == HEALTHBOX_ALL)
            UpdateStatusIconInHealthbox(healthboxSpriteId);
    }
}
s32 MoveBattleBar(u8 battler, u8 healthboxSpriteId, u8 whichBar, u8 unused2)
{
    s32 currentBarValue;

    if (whichBar == 0)
    {
        currentBarValue = CalcNewBarValue(sBattleBars[battler].maxValue, sBattleBars[battler].oldValue, sBattleBars[battler].receivedValue, &sBattleBars[battler].currValue, 6, 1);
    }
    else
    {
        u16 expFraction;
        s32 receivedValue;

        expFraction = GetScaledExpFraction(sBattleBars[battler].oldValue, sBattleBars[battler].receivedValue, sBattleBars[battler].maxValue, 8);
        if (expFraction == 0)
            expFraction = 1;
        receivedValue = sBattleBars[battler].receivedValue;
        expFraction = ABS(receivedValue / expFraction);
        currentBarValue = CalcNewBarValue(sBattleBars[battler].maxValue, sBattleBars[battler].oldValue, receivedValue, &sBattleBars[battler].currValue, 8, expFraction);
    }
    if (whichBar == 1 || (whichBar == 0 && (!gBattleSpriteInfo[battler].hpNumbersNoBars)))
        MoveBattleBarGraphically(battler, whichBar);
    if (currentBarValue == -1)
        sBattleBars[battler].currValue = 0;
    return currentBarValue;
}

static void MoveBattleBarGraphically(u8 battler, u8 whichBar)
{
    u8 pixelsArray[8];
    u8 barElementId;
    u8 i;

    switch (whichBar)
    {
    case 0:
        {
            u8 filledPixelsCount = CalcBarFilledPixels(sBattleBars[battler].maxValue,
                sBattleBars[battler].oldValue,
                sBattleBars[battler].receivedValue,
                &sBattleBars[battler].currValue,
                pixelsArray,
                6);

            barElementId = 3;
            if (filledPixelsCount <= 0x18)
            {
                barElementId = 0x38;
                if (filledPixelsCount > 9)
                    barElementId = 0x2F;
            }
            for (i = 0; i < 6; i++)
            {
                u8 healthBarSpriteId = gSprites[sBattleBars[battler].healthboxSpriteId].hMain_HealthBarSpriteId;
                if (i < 2)
                    CpuCopy32(GetHealthboxElementGfxPtr(barElementId) + pixelsArray[i] * 32, OBJ_VRAM0 + (gSprites[healthBarSpriteId].oam.tileNum + 2 + i) * 32, 32);
                else
                    CpuCopy32(GetHealthboxElementGfxPtr(barElementId) + pixelsArray[i] * 32, OBJ_VRAM0 + 64 + (i + gSprites[healthBarSpriteId].oam.tileNum) * 32, 32);
            }
        }
        break;
    case 1:
        {
            u8 level;

            CalcBarFilledPixels(sBattleBars[battler].maxValue,
                sBattleBars[battler].oldValue,
                sBattleBars[battler].receivedValue,
                &sBattleBars[battler].currValue,
                pixelsArray,
                8);
            level = GetMonData(&gPlayerParty[gBattlerPartyIndexes[battler]], MON_DATA_LEVEL);
            if (level == 100)
            {
                for (i = 0; i < 8; i++)
                    pixelsArray[i] = 0;
            }
            for (i = 0; i < 8; i++)
            {
                if (i < 4)
                    CpuCopy32(GetHealthboxElementGfxPtr(0xC) + pixelsArray[i] * 32, OBJ_VRAM0 + (gSprites[sBattleBars[battler].healthboxSpriteId].oam.tileNum + 0x24 + i) * 32, 32);
                else
                    CpuCopy32(GetHealthboxElementGfxPtr(0xC) + pixelsArray[i] * 32, OBJ_VRAM0 + 0xB80 + (i + gSprites[sBattleBars[battler].healthboxSpriteId].oam.tileNum) * 32, 32);
            }
        }
        break;
    }
}

static int CalcNewBarValue(s32 maxValue, s32 oldValue, int receivedValue, int *currValue, u8 scale, u16 toAdd)
{
    u8 totalPixels = scale << 3;
    int currentValue;
    int ret;

    if (*currValue == -32768)
    {
        if (maxValue < totalPixels)
            *currValue = oldValue << 8;
        else
            *currValue = oldValue;
    }
    //_08045F8A
    oldValue -= receivedValue;
    if (oldValue < 0)
        oldValue = 0;
    else if (oldValue > maxValue)
        oldValue = maxValue;
    if (maxValue < totalPixels)
    {
        int currentInt = *currValue >> 8;

        currentValue = *currValue;
        if (oldValue == currentInt && (currentValue & 0xFF) == 0)
            return -1;
    }
    else
    {
        currentValue = *currValue;
        if (oldValue == currentValue)
            return -1;
    }
    //_08045FC4
    if (maxValue < totalPixels)
    {
        int increment = (maxValue << 8) / totalPixels;

        if (receivedValue < 0)
        {
            *currValue = currentValue + increment;
            ret = *currValue >> 8;
            if (ret >= oldValue)
            {
                *currValue = oldValue << 8;
                ret = oldValue;
            }
        }
        //_08045FE2
        else
        {
            *currValue = currentValue - increment;
            ret = *currValue >> 8;
            if ((*currValue & 0xFF) > 0)
                ret++;
            if (ret <= oldValue)
            {
                *currValue = oldValue << 8;
                ret = oldValue;
            }
        }
    }
    else
    {
        //_08045FFE
        if (receivedValue < 0)
        {
            *currValue += toAdd;
            if (*currValue > oldValue)
                *currValue = oldValue;
            ret = *currValue;
        }
        //_08046010
        else
        {
            *currValue -= toAdd;
            if (*currValue < oldValue)
                *currValue = oldValue;
            ret = *currValue;
        }
    }
    return ret;
}

static u8 CalcBarFilledPixels(int maxValue, int oldValue, int receivedValue, int *currValue, u8 *pixelsArray, u8 scale)
{
    s32 newValue = oldValue - receivedValue;
    u8 pixelCount;
    u8 i;
    u8 pixels;

    if (newValue < 0)
        newValue = 0;
    else if (newValue > maxValue)
        newValue = maxValue;
    pixelCount = scale << 3;
    for (i = 0; i < scale; i++)
        pixelsArray[i] = 0;
    if (maxValue < pixelCount)
        pixels = *currValue * pixelCount / maxValue >> 8;
    else
        pixels = *currValue * pixelCount / maxValue;
    pixelCount = pixels;
    if (pixelCount == 0 && newValue > 0)
    {
        pixelsArray[0] = 1;
        pixelCount = 1;
    }
    else
    {
        for (i = 0; i < scale; i++)
        {
            if (pixels >= 8)
            {
                pixelsArray[i] = 8;
            }
            else
            {
                pixelsArray[i] = pixels;
                break;
            }
            pixels -= 8;
        }
    }
    return pixelCount;
}

// These two functions seem as if they were made for testing the health bar.
s16 UpdateHealthBar(struct HealthBarInfo *barInfo, int *currValue, u16 *dest, int unused)
{
    u16 ret;
    s16 value;

    ret = CalcNewBarValue(barInfo->maxValue, barInfo->oldValue, barInfo->receivedValue, currValue, 6, 1);
    UpdateHealthBarTiles(barInfo, currValue, dest);
    if (barInfo->maxValue < 0x30)
        value = *currValue >> 8;
    else
        value = *currValue;
    DummiedOutFunction(barInfo->maxValue, value, unused);
    return ret;
}

static void UpdateHealthBarTiles(struct HealthBarInfo *barInfo, int *currValue, u16 *dest)
{
    u8 pixels[6];
    u16 tiles[6];
    u8 i;

    CalcBarFilledPixels(barInfo->maxValue, barInfo->oldValue, barInfo->receivedValue, currValue, pixels, 6);
    for (i = 0; i < 6; i++)
        tiles[i] = (barInfo->paletteNum << 12) | (barInfo->baseTileNum + pixels[i]);
    CpuCopy16(tiles, dest, sizeof(tiles));
}

static u8 GetScaledExpFraction(int oldValue, int receivedValue, int maxValue, u8 scale)
{
    u8 totalPixels = scale * 8;
    int newValue = oldValue - receivedValue;
    s8 oldToMax;
    s8 newToMax;
    s32 result;

    if (newValue < 0)
        newValue = 0;
    else if (newValue > maxValue)
        newValue = maxValue;

    oldToMax = oldValue * totalPixels / maxValue;
    newToMax = newValue * totalPixels / maxValue;
    result = oldToMax - newToMax;
    return ABS(result);
}

u8 GetScaledHPFraction(s16 hp, s16 maxHp, u8 scale)
{
    u8 result = hp * scale / maxHp;

    if (result == 0 && hp > 0)
        return 1;
    return result;
}

u8 GetHPBarLevel(s16 hp, s16 maxHp)
{
    int result;

    if (hp == maxHp)
        result = 4;
    else
    {
        u8 fraction = GetScaledHPFraction(hp, maxHp, 48);
        if (fraction >= 25)
            result = 3;
        else if (fraction >= 10)
            result = 2;
        else if (fraction > 0)
            result = 1;
        else
            result = 0;
    }
    return result;
}
