#include "global.h"
#include "battle_gfx_sfx_util.h"
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

struct BattleBarInfo
{
    u8 healthboxSpriteId;
    u32 maxValue;
    u32 oldValue;
    u32 receivedValue;
    s32 currValue;
};

static void SpriteCB_HealthBar(struct Sprite *sprite);
static void SpriteCB_StatusSummaryBar_Enter(struct Sprite *sprite);
static void SpriteCB_StatusSummaryBalls_Enter(struct Sprite *sprite);

static const struct OamData sOamData_64x32 =
{
    .shape = 1,
    .size = 3,
    .priority = 1,
};

static const struct SpriteTemplate sHealthboxPlayerSpriteTemplates[] =
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

static const struct SpriteTemplate sHealthboxOpponentSpriteTemplates[] =
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

static const struct SpriteTemplate sHealthboxSafariSpriteTemplate =
{
    .tileTag = 55051,
    .paletteTag = 55039,
    .oam = &sOamData_64x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

static const struct OamData sOamData_Healthbar =
{
    .shape = 1,
    .size = 1,
    .priority = 1,
};

static const struct SpriteTemplate sHealthbarSpriteTemplates[] =
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

static const struct Subsprite sUnused_Subsprites_0[] =
{
    { .x = -16, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =  0, .size = 3 }, // size := 64x32
    { .x =  48, .y =  0, .shape =      ST_OAM_SQUARE, .priority = 1, .tileOffset = 32, .size = 2 }, // size := 32x32
    { .x = -16, .y = 32, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset = 48, .size = 1 }, // size := 32x8
    { .x =  16, .y = 32, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset = 52, .size = 1 }, // size := 32x8
    { .x =  48, .y = 32, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset = 56, .size = 1 }, // size := 32x8
};

static const struct Subsprite sUnused_Subsprites_2[] =
{
    { .x = -16, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset = 64, .size = 3 }, // size := 64x32
    { .x =  48, .y =  0, .shape =      ST_OAM_SQUARE, .priority = 1, .tileOffset = 96, .size = 2 }, // size := 32x32
    { .x = -16, .y = 32, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =112, .size = 1 }, // size := 32x8
    { .x =  16, .y = 32, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =116, .size = 1 }, // size := 32x8
    { .x =  48, .y = 32, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =120, .size = 1 }, // size := 32x8
};

static const struct Subsprite sUnused_Subsprites_1[] =
{
    { .x = -16, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =  0, .size = 3 }, // size := 64x32
    { .x =  48, .y =  0, .shape =      ST_OAM_SQUARE, .priority = 1, .tileOffset = 32, .size = 2 }, // size := 32x32
};

static const struct Subsprite sUnused_Subsprites_3[] =
{
    { .x = -16, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =  0, .size = 3 }, // size := 64x32
    { .x =  48, .y =  0, .shape =      ST_OAM_SQUARE, .priority = 1, .tileOffset = 32, .size = 2 }, // size := 32x32
};

static const struct Subsprite sHealthBar_Subsprites_Player[] =
{
    { .x = -16, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =  0, .size = 1 }, // size := 32x8
    { .x =  16, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =  4, .size = 1 }, // size := 32x8
};

static const struct Subsprite sHealthBar_Subsprites_Opponent[] =
{
    { .x = -16, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =  0, .size = 1 }, // size := 32x8
    { .x =  16, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =  4, .size = 1 }, // size := 32x8
    { .x = -32, .y =  0, .shape =      ST_OAM_SQUARE, .priority = 1, .tileOffset =  8, .size = 0 }, // size := 8x8
};

static const struct SubspriteTable UNUSED sUnused_SubspriteTable[] =
{
    { ARRAY_COUNT(sUnused_Subsprites_0), sUnused_Subsprites_0 },
    { ARRAY_COUNT(sUnused_Subsprites_1), sUnused_Subsprites_1 },
    { ARRAY_COUNT(sUnused_Subsprites_2), sUnused_Subsprites_2 },
    { ARRAY_COUNT(sUnused_Subsprites_3), sUnused_Subsprites_3 },
};

static const struct SubspriteTable sHealthBar_SubspriteTables[] =
{
    { ARRAY_COUNT(sHealthBar_Subsprites_Player), sHealthBar_Subsprites_Player },
    { ARRAY_COUNT(sHealthBar_Subsprites_Opponent), sHealthBar_Subsprites_Opponent },
};

static const struct Subsprite sStatusSummaryBar_Subsprites_Enter[] =
{
    { .x = -96, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =  0, .size = 1 }, // size := 32x8
    { .x = -64, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =  4, .size = 1 }, // size := 32x8
    { .x = -32, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =  8, .size = 1 }, // size := 32x8
    { .x =   0, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset = 12, .size = 1 }, // size := 32x8
};

static const struct Subsprite sStatusSummaryBar_Subsprites_Exit[] =
{
    { .x = -96, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =  0, .size = 1 }, // size := 32x8
    { .x = -64, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =  4, .size = 1 }, // size := 32x8
    { .x = -32, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =  8, .size = 1 }, // size := 32x8
    { .x =   0, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =  8, .size = 1 }, // size := 32x8
    { .x =  32, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset =  8, .size = 1 }, // size := 32x8
    { .x =  64, .y =  0, .shape = ST_OAM_H_RECTANGLE, .priority = 1, .tileOffset = 12, .size = 1 }, // size := 32x8
};

static const struct SubspriteTable sStatusSummaryBar_SubspriteTable_Enter[] =
{
    { ARRAY_COUNT(sStatusSummaryBar_Subsprites_Enter), sStatusSummaryBar_Subsprites_Enter },
};

static const struct SubspriteTable sStatusSummaryBar_SubspriteTable_Exit[] =
{
    { ARRAY_COUNT(sStatusSummaryBar_Subsprites_Exit), sStatusSummaryBar_Subsprites_Exit },
};

// unused dakuten/handakuten tiles
const u8 gUnusedDakuten[] = INCBIN_U8("graphics/unused/dakuten.4bpp");

const struct CompressedSpriteSheet gStatusSummaryBarSpriteSheets[] =
{
    { gBattleInterface_BallStatusBarGfx, 512, 0xd70c },
    { gBattleInterface_BallStatusBarGfx, 512, 0xd70d },
};

const struct SpritePalette gStatusSummaryBarSpritePals[] =
{
    { gBattleInterface_BallStatusBarPal, 0xd710 },
    { gBattleInterface_BallStatusBarPal, 0xd711 },
};

const struct SpritePalette gStatusSummaryBallsSpritePals[] =
{
    { gBattleInterface_BallDisplayPal, 0xd712 },
    { gBattleInterface_BallDisplayPal, 0xd713 },
};

const struct SpriteSheet gStatusSummaryBallsSpriteSheets[] =
{
    { Tiles_D129AC, 0x80, 0xd714 },
    { Tiles_D129AC, 0x80, 0xd715 },
};

static const struct OamData UNUSED sOamData_Unused64x32 =
{
    .shape = 1,
    .size = 3,
    .priority = 1,
};

static const struct OamData sOamData_StatusSummaryBalls =
{
    .shape = 0,
    .size = 0,
    .priority = 1,
};

const struct SpriteTemplate gStatusSummaryBarSpriteTemplates[] =
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

const struct SpriteTemplate gStatusSummaryBallsSpriteTemplates[] = {
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

u8 *const gUnknown_0820A804[2] =
{
    OBJ_VRAM0 + 32 * 74,
    OBJ_VRAM0 + 32 * 75,
};

u8 *const gUnknown_0820A80C[2] =
{
    OBJ_VRAM0 + 32 * 41,
    OBJ_VRAM0 + 32 * 42,
};

u8 *const gUnknown_0820A814[2] =
{
    OBJ_VRAM0 + 32 * 42,
    OBJ_VRAM0 + 32 * 43,
};

const u8 gUnknown_0820A81C[] = __("{COLOR DARK_GREY}{HIGHLIGHT RED}                        ");

u8 *const gUnknown_0820A83C[3] =
{
    OBJ_VRAM0 + 32 * 31,
    OBJ_VRAM0 + 32 * 88,
    OBJ_VRAM0 + 32 * 89,
};

u8 *const gUnknown_0820A848[3] =
{
    OBJ_VRAM0 + 32 * 22,
    OBJ_VRAM0 + 32 * 23,
    OBJ_VRAM0 + 32 * 48,
};

u8 *const gUnknown_0820A854[2] =
{
    OBJ_VRAM0 + 32 * 90,
    OBJ_VRAM0 + 32 * 91,
};

u8 *const gUnknown_0820A85C[2] =
{
    OBJ_VRAM0 + 32 * 49,
    OBJ_VRAM0 + 32 * 50,
};

static const u8 sEmptyWhiteText_GrayHighlight[] = _("{COLOR DARK_GREY}{HIGHLIGHT RED}              /");

u8 *const gUnknown_0820A87C[6] =
{
    OBJ_VRAM0 + 32 * 0,
    OBJ_VRAM0 + 32 * 1,
    OBJ_VRAM0 + 32 * 2,
    OBJ_VRAM0 + 32 * 3,
    OBJ_VRAM0 + 32 * 4,
    OBJ_VRAM0 + 32 * 5,
};

u8 *const gUnknown_0820A894[2] =
{
    OBJ_VRAM0 + 32 * 6,
    OBJ_VRAM0 + 32 * 7,
};

const u8 gUnknown_0820A89C[] = __("{COLOR DARK_GREY}{HIGHLIGHT TRANSPARENT}              ");
const u8 gUnknown_0820A8B0[] = _("{HIGHLIGHT RED}");

u8 *const gUnknown_0820A8B4[10] =
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

u8 *const gUnknown_0820A8DC[10] =
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

u8 *const gUnknown_0820A904[10] =
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

extern u16 gBattleTypeFlags;

extern const u8 gText_SafariBalls[];
extern const u8 gText_SafariBallLeft[];
extern const u8 BattleText_HighlightRed[];
extern const u8 gHealthboxElementsGfxTable[][32];

extern const u16 gBattleInterfaceStatusIcons_DynPal[];

#define ABS(n) ((n) >= 0 ? (n) : -(n))
// Used for computing copy destination addresses
#define MACRO1(n) ((n) - (n) / 8 * 8) + 64 * ((n) / 8)

static void SpriteCB_HealthBoxOther(struct Sprite *sprite);
static const u8 *GetHealthboxElementGfxPtr(u8 elementId);
static void UpdateHpTextInHealthboxInDoubles(u8 healthboxSpriteId, s16 value, u8 maxOrCurrent);
/*static*/ void UpdateStatusIconInHealthbox(u8);
static void SpriteCB_StatusSummaryBalls_OnSwitchout(struct Sprite *);
static void SpriteCB_StatusSummaryBalls_Exit(struct Sprite *sprite);
static void SpriteCB_StatusSummaryBar_Exit(struct Sprite *);
static void Task_HidePartyStatusSummary_DuringBattle(u8 taskId);
static void Task_HidePartyStatusSummary_BattleStart_1(u8 taskId);
static void Task_HidePartyStatusSummary_BattleStart_2(u8 taskId);
static u8 GetStatusIconForBattlerId(u8 statusElementId, u8 battler);
static s32 CalcNewBarValue(s32 maxValue, s32 oldValue, s32 receivedValue, s32 *currValue, u8 scale, u16 toAdd);
static u8 GetScaledExpFraction(s32 oldValue, s32 receivedValue, s32 maxValue, u8 scale);
static void MoveBattleBarGraphically(u8 battler, u8 whichBar);
static u8 CalcBarFilledPixels(s32 maxValue, s32 oldValue, s32 receivedValue, s32 *currValue, u8 *pixelsArray, u8 scale);
static void sub_8046128(struct BattleInterfaceStruct1 *barInfo, int *currValue, u16 *dest);

static s32 DummiedOutFunction(s16 unused1, s16 unused2, s32 unused3)
{
    return 9;
}

static void Debug_DrawNumber(s16 number, u16 *dest, bool8 unk)
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

static void UNUSED Debug_DrawNumberPair(s16 number1, s16 number2, u16 *dest)
{
    dest[4] = 0x1E;
    Debug_DrawNumber(number2, dest, 0);
    Debug_DrawNumber(number1, dest + 5, 1);
}

u8 CreateBattlerHealthboxSprites(u8 battler)
{
    int data6 = 0;
    u8 healthboxLeftSpriteId;
    u8 healthboxRightSpriteId;
    u8 healthbarSpriteId;
    struct Sprite *healthBarSpritePtr;

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
            data6 = 2;
        }
        //_080439F2

        gSprites[healthboxLeftSpriteId].oam.affineParam = healthboxRightSpriteId;
        gSprites[healthboxRightSpriteId].data[5] = healthboxLeftSpriteId;
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
            gSprites[healthboxRightSpriteId].data[5] = healthboxLeftSpriteId;
            gSprites[healthboxRightSpriteId].oam.tileNum += 32;
            gSprites[healthboxRightSpriteId].callback = SpriteCB_HealthBoxOther;
            data6 = 1;
        }
        //_08043ACC
        else
        {
            healthboxLeftSpriteId = CreateSprite(&sHealthboxOpponentSpriteTemplates[GetBattlerPosition(battler) / 2], 240, 160, 1);
            healthboxRightSpriteId = CreateSpriteAtEnd(&sHealthboxOpponentSpriteTemplates[GetBattlerPosition(battler) / 2], 240, 160, 1);

            gSprites[healthboxLeftSpriteId].oam.affineParam = healthboxRightSpriteId;
            gSprites[healthboxRightSpriteId].data[5] = healthboxLeftSpriteId;
            gSprites[healthboxRightSpriteId].oam.tileNum += 32;
            gSprites[healthboxRightSpriteId].callback = SpriteCB_HealthBoxOther;
            data6 = 2;
        }
        //_08043B4E
    }
    //_08043B50

    healthbarSpriteId = CreateSpriteAtEnd(&sHealthbarSpriteTemplates[gBattlerPositions[battler]], 140, 60, 0);
    healthBarSpritePtr = &gSprites[healthbarSpriteId];
    SetSubspriteTables(healthBarSpritePtr, &sHealthBar_SubspriteTables[GetBattlerSide(battler)]);
    healthBarSpritePtr->subspriteMode = 2;
    healthBarSpritePtr->oam.priority = 1;
    CpuCopy32(GetHealthboxElementGfxPtr(1), OBJ_VRAM0 + healthBarSpritePtr->oam.tileNum * 32, 64);

    gSprites[healthboxLeftSpriteId].data[5] = healthbarSpriteId;
    gSprites[healthboxLeftSpriteId].data[6] = battler;
    gSprites[healthboxLeftSpriteId].invisible = TRUE;
    gSprites[healthboxRightSpriteId].invisible = TRUE;
    healthBarSpritePtr->data[5] = healthboxLeftSpriteId;
    healthBarSpritePtr->data[6] = data6;
    healthBarSpritePtr->invisible = TRUE;

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
    gSprites[healthboxRightSpriteId].data[5] = healthboxLeftSpriteId;
    gSprites[healthboxRightSpriteId].callback = SpriteCB_HealthBoxOther;
    return healthboxLeftSpriteId;
}

static const u8 *GetHealthboxElementGfxPtr(u8 elementId)
{
    return gHealthboxElementsGfxTable[elementId];
}

static void SpriteCB_HealthBar(struct Sprite *sprite)
{
    u8 healthboxSpriteId = sprite->data[5];

    switch (sprite->data[6])
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
    u8 healthboxMainSpriteId = sprite->data[5];

    sprite->x = gSprites[healthboxMainSpriteId].x + 64;
    sprite->y = gSprites[healthboxMainSpriteId].y;
    sprite->x2 = gSprites[healthboxMainSpriteId].x2;
    sprite->y2 = gSprites[healthboxMainSpriteId].y2;
}

void SetBattleBarStruct(u8 battler, u8 healthboxSpriteId, u32 maxVal, u32 oldVal, u32 receivedValue)
{
    eBattleBars[battler].healthboxSpriteId = healthboxSpriteId;
    eBattleBars[battler].maxValue = maxVal;
    eBattleBars[battler].oldValue = oldVal;
    eBattleBars[battler].receivedValue = receivedValue;
    eBattleBars[battler].currValue = -0x8000;
}

void SetHealthboxSpriteInvisible(u8 healthboxSpriteId)
{
    gSprites[healthboxSpriteId].invisible = TRUE;
    gSprites[gSprites[healthboxSpriteId].data[5]].invisible = TRUE;
    gSprites[gSprites[healthboxSpriteId].oam.affineParam].invisible = TRUE;
}

void SetHealthboxSpriteVisible(u8 healthboxSpriteId)
{
    gSprites[healthboxSpriteId].invisible = FALSE;
    gSprites[gSprites[healthboxSpriteId].data[5]].invisible = FALSE;
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
    DestroySprite(&gSprites[gSprites[healthboxSpriteId].data[5]]);
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
        u8 healthbarSpriteId;

        healthboxLeftSpriteId = gHealthboxSpriteIds[i];
        healthboxRightSpriteId = gSprites[healthboxLeftSpriteId].oam.affineParam;
        healthbarSpriteId = gSprites[healthboxLeftSpriteId].data[5];
        gSprites[healthboxLeftSpriteId].oam.priority = priority;
        gSprites[healthboxRightSpriteId].oam.priority = priority;
        gSprites[healthbarSpriteId].oam.priority = priority;
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
    u8 text[30];
    u8 *const *r7;
    u8 *ptr;
    s32 i;
    s32 two;

    // TODO: Make this a local variable
    memcpy(text, gUnknown_0820A81C, sizeof(text));
    if (!IsDoubleBattle())
    {
        if (GetBattlerSide(gSprites[healthboxSpriteId].data[6]) == 0)
            r7 = gUnknown_0820A804;
        else
            r7 = gUnknown_0820A80C;
    }
    else
    {
        if (GetBattlerSide(gSprites[healthboxSpriteId].data[6]) == 0)
            r7 = gUnknown_0820A814;
        else
            r7 = gUnknown_0820A80C;
    }

    ptr = text + 6;
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
    RenderTextHandleBold(eBattleInterfaceGfxBuffer, text);

    two = 2;
    for (i = 0; i < two; i++)
        CpuCopy32(&eBattleInterfaceGfxBuffer[i * 64 + 32], r7[i] + gSprites[healthboxSpriteId].oam.tileNum * TILE_SIZE_4BPP, TILE_SIZE_4BPP);
}

void UpdateHpTextInHealthbox(u8 healthboxSpriteId, s16 value, u8 maxOrCurrent)
{
    u8 text[0x14];
    u8 *ptr;
    s32 battler;
    u8 *const *r4;
    s32 i;
    u8 r8;

    // TODO: make this a local variable
    memcpy(text, sEmptyWhiteText_GrayHighlight, sizeof(text));
    battler = gSprites[healthboxSpriteId].data[6];

    if (IsDoubleBattle() == TRUE || GetBattlerSide(battler) == 1)
    {
        //_08044136
        UpdateHpTextInHealthboxInDoubles(healthboxSpriteId, value, maxOrCurrent);
    }
    else
    {
        //
        ptr = text + 6;
        if (maxOrCurrent == 0)
        {
            if (GetBattlerSide(gSprites[healthboxSpriteId].data[6]) == 0)
                r4 = gUnknown_0820A83C;
            else
                r4 = gUnknown_0820A848;
            r8 = 3;
            ptr = sub_8003504(ptr, value, 19, 1);
            *(ptr++) = CHAR_SLASH;
            *(ptr++) = EOS;
        }
        else
        {
            if (GetBattlerSide(gSprites[healthboxSpriteId].data[6]) == 0)
                r4 = gUnknown_0820A854;
            else
                r4 = gUnknown_0820A85C;
            r8 = 2;
            ptr = sub_8003504(ptr, value, 15, 1);
        }
        RenderTextHandleBold(eBattleInterfaceGfxBuffer, text);
        //asm(""::"r"(a));
        //_080441B6
        for (i = 0; i < r8; i++)  // _080440BC
        {
            CpuCopy32(
                &eBattleInterfaceGfxBuffer[i * 64 + 32],
                r4[i] + gSprites[healthboxSpriteId].oam.tileNum * 32,
                0x20
            );
        }
    }
}

static void UpdateHpTextInHealthboxInDoubles(u8 healthboxSpriteId, s16 value, u8 maxOrCurrent)
{
    u8 text[0x14];
    u8 *ptr;
    u8 *const *r7;
    int r10;
    int r4;
    int i;

    // TODO: make this a local variable
    memcpy(text, gUnknown_0820A89C, sizeof(text));
    r4 = gSprites[healthboxSpriteId].data[6];
    if ((gBattleSpriteInfo[r4].hpNumbersNoBars) == 0)
        return;
    ptr = text + 6;
    if (maxOrCurrent == 0)
    {
        r7 = gUnknown_0820A87C;
        r10 = 6;
        ptr = sub_8003504(ptr, value, 0x2B, 1);
        *(ptr++) = CHAR_SLASH;
        *(ptr++) = EOS;
    }
    else
    {
        r7 = gUnknown_0820A894;
        r10 = 2;
        sub_8003504(ptr, value, 0xF, 1);
        if (GetBattlerSide(r4) == 0)
        {
            CpuCopy32(GetHealthboxElementGfxPtr(0x74), OBJ_VRAM0 + (gSprites[healthboxSpriteId].oam.tileNum + 0x34) * 32, 32);
        }
    }
    r4 = gSprites[healthboxSpriteId].data[5];
    RenderTextHandleBold(eBattleInterfaceGfxBuffer, text);
    for (i = 0; i < r10; i++)
    {
        CpuCopy32((void *)(&eBattleInterfaceGfxBuffer[i * 64 + 32]), r7[i] + gSprites[r4].oam.tileNum * 32, 32);
    }
}

static void PrintSafariMonInfo(u8 healthboxSpriteId, struct Pokemon *mon)
{
    u8 text[20];
    s32 j;
    u8* spriteTileNum;
    u8 *barFontGfx;
    u8 i, var, nature, healthBarSpriteId;

    // TODO: make this a local variable
    memcpy(text, sEmptyWhiteText_GrayHighlight, sizeof(text));
    barFontGfx = &eBattleInterfaceGfxBuffer[0x520 + GetBattlerPosition(gSprites[healthboxSpriteId].data[6]) * 0x180];
    var = 5;
    nature = GetNature(mon);
    StringCopy(text + 6, gNatureNamePointers[nature]);
    RenderTextHandleBold(barFontGfx, text);

    for (j = 6, i = 0; i < var; i++, j++)
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

    for (j = 1; j < var + 1; j++)
    {
        spriteTileNum = (gSprites[healthboxSpriteId].oam.tileNum + MACRO1(j)) * TILE_SIZE_4BPP + (void *)OBJ_VRAM0;
        CpuCopy32(barFontGfx, spriteTileNum, 32);
        barFontGfx += 32;

        spriteTileNum = (8 + gSprites[healthboxSpriteId].oam.tileNum + MACRO1(j)) * TILE_SIZE_4BPP + (void *)OBJ_VRAM0;
        CpuCopy32(barFontGfx, spriteTileNum, 32);
        barFontGfx += 32;
    }

    healthBarSpriteId = gSprites[healthboxSpriteId].data[5];
    ConvertIntToDecimalStringN(text + 6, gBattleStruct->safariCatchFactor, 1, 2);
    ConvertIntToDecimalStringN(text + 9, gBattleStruct->safariEscapeFactor, 1, 2);
    text[5] = CHAR_SPACE;
    text[8] = CHAR_SLASH;
    RenderTextHandleBold(eBattleInterfaceGfxBuffer, text);

    for (j = 0; j < 5; j++)
    {
        if (j < 2)
        {
            CpuCopy32(
                &eBattleInterfaceGfxBuffer[j * 64 + 32],
                 ((gSprites[healthBarSpriteId].oam.tileNum + 2 + j) * TILE_SIZE_4BPP) + (void*)(OBJ_VRAM0),
                32
            );
        }
        else
        {
            CpuCopy32(
                &eBattleInterfaceGfxBuffer[j * 64 + 32],
                (void*)(OBJ_VRAM0) + (gSprites[healthBarSpriteId].oam.tileNum + 8 + j - 2) * TILE_SIZE_4BPP, // Must be written as "8 + j - 2" to match
                32
            );
        }
    }
}

extern u8 gUnknown_020297ED;

void SwapHpBarsWithHpText(void)
{
    s32 i;
    u8 healthBarSpriteId;

    for (i = 0; i < gBattlersCount; i++)
    {
        if (gSprites[gHealthboxSpriteIds[i]].callback == SpriteCallbackDummy
#if DEBUG
         && (gUnknown_020297ED != 0 || GetBattlerSide(i) != 1)
#else
         && GetBattlerSide(i) != 1
#endif
         && (IsDoubleBattle() || GetBattlerSide(i) != 0))
        {
            bool8 noBars;

            gBattleSpriteInfo[i].hpNumbersNoBars ^= 1;
            noBars = gBattleSpriteInfo[i].hpNumbersNoBars;
            if (GetBattlerSide(i) == 0)
            {

                if (!IsDoubleBattle())
                    continue;
                if (gBattleTypeFlags & BATTLE_TYPE_SAFARI)
                    continue;

                if (noBars == 1)
                {
                    healthBarSpriteId = gSprites[gHealthboxSpriteIds[i]].data[5];

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
                if (noBars == 1)
                {
                    if (gBattleTypeFlags & BATTLE_TYPE_SAFARI)
                    {
                        PrintSafariMonInfo(gHealthboxSpriteIds[i], &gEnemyParty[gBattlerPartyIndexes[i]]);
                    }
                    else
                    {
                        healthBarSpriteId = gSprites[gHealthboxSpriteIds[i]].data[5];

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
            gSprites[gHealthboxSpriteIds[i]].data[7] ^= 1;
        }
    }
}

u8 CreatePartyStatusSummarySprites(u8 battler, const struct HpAndStatus *partyInfo, bool8 skipPlayer, bool8 isBattleStart)
{
    bool8 isOpponent;
    s16 bar_X;
    s16 bar_Y;
    s16 bar_pos2_X;
    s16 bar_data0;

    s32 i, j;
    u8 ballIconSpritesIds[6];
    s8 sp14;
    u8 summaryBarSpriteId;
    u8 taskId;

    if (skipPlayer == 0 || GetBattlerPosition(battler) != 3)
    {
        if (GetBattlerSide(battler) == 0)
        {
            isOpponent = 0;
            bar_X = 136;
            bar_Y = 96;
            bar_pos2_X = 100;
            bar_data0 = -5;
        }
        else
        {
            isOpponent = 1;
            if (skipPlayer == 0 || !IsDoubleBattle())
            {
                bar_X = 104;
                bar_Y = 40;
            }
            else
            {
                bar_X = 104;
                bar_Y = 16;
            }
            bar_pos2_X = -100;
            bar_data0 = 5;
        }
    }
    else
    {
        isOpponent = 1;
        bar_X = 104;
        bar_Y = 40;
        bar_pos2_X = -100;
        bar_data0 = 5;
    }
    //_08044884

    for (i = 0, sp14 = 0; i < 6; i++)  //_080448A0
    {
        if (partyInfo[i].hp != 0xFFFF)
            sp14++;
    }

    LoadCompressedSpriteSheet(&gStatusSummaryBarSpriteSheets[isOpponent]);
    LoadSpriteSheet(&gStatusSummaryBallsSpriteSheets[isOpponent]);
    LoadSpritePalette(&gStatusSummaryBarSpritePals[isOpponent]);
    LoadSpritePalette(&gStatusSummaryBallsSpritePals[isOpponent]);

    summaryBarSpriteId = CreateSprite(&gStatusSummaryBarSpriteTemplates[isOpponent], bar_X, bar_Y, 10);
    SetSubspriteTables(&gSprites[summaryBarSpriteId], sStatusSummaryBar_SubspriteTable_Enter);
    gSprites[summaryBarSpriteId].x2 = bar_pos2_X;
    gSprites[summaryBarSpriteId].data[0] = bar_data0;
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
        ballIconSpritesIds[i] = CreateSpriteAtEnd(&gStatusSummaryBallsSpriteTemplates[isOpponent], bar_X, bar_Y - 4, 9);
        if (isBattleStart == 0)
        {
            gSprites[ballIconSpritesIds[i]].callback = SpriteCB_StatusSummaryBalls_OnSwitchout;
        }
        //_080449A0
        if (isOpponent == 0)
        {
            gSprites[ballIconSpritesIds[i]].x2 = 0;
            gSprites[ballIconSpritesIds[i]].y2 = 0;
        }
        //_080449BE
        gSprites[ballIconSpritesIds[i]].data[0] = summaryBarSpriteId;
        if (isOpponent == 0)
        {
            gSprites[ballIconSpritesIds[i]].x += 10 * i + 24;
            gSprites[ballIconSpritesIds[i]].data[1] = 7 * i + 10;
            gSprites[ballIconSpritesIds[i]].x2 = 120;
        }
        //_08044A18
        else
        {
            gSprites[ballIconSpritesIds[i]].x -= 10 * (5 - i) + 24;
            gSprites[ballIconSpritesIds[i]].data[1] = 7 * (6 - i) + 10;
            gSprites[ballIconSpritesIds[i]].x2 = -120;
        }
        //_08044A56
        gSprites[ballIconSpritesIds[i]].data[2] = isOpponent;
    }
    //_08044A76
    if (GetBattlerSide(battler) == 0)
    {
        for (i = 0; i < 6; i++)  //_08044A9A
        {
            if (gBattleTypeFlags & BATTLE_TYPE_MULTI) // && b[i] != 0xFFFF && b[i]
            {
                if (partyInfo[i].hp == 0xFFFF)
                {
                    //_08044AE6
                    gSprites[ballIconSpritesIds[i]].oam.tileNum++;
                    gSprites[ballIconSpritesIds[i]].data[7] = 1;
                    // to _08044B52
                }
                else if (partyInfo[i].hp == 0)
                {
                    gSprites[ballIconSpritesIds[i]].oam.tileNum += 3;
                    // to _08044B46
                }
                else if (partyInfo[i].status != 0)
                {
                    gSprites[ballIconSpritesIds[i]].oam.tileNum += 2;
                }
            }
            //_08044ADC
            else
            {
                if (i >= sp14)
                {
                    //_08044AE6
                    gSprites[ballIconSpritesIds[i]].oam.tileNum++;
                    gSprites[ballIconSpritesIds[i]].data[7] = 1;
                    // to _08044B52
                }
                else if (partyInfo[i].hp == 0)
                {
                    //_08044B14
                    gSprites[ballIconSpritesIds[i]].oam.tileNum += 3;
                    // to _08044B46
                }
                else if (partyInfo[i].status != 0)
                {
                    gSprites[ballIconSpritesIds[i]].oam.tileNum += 2;
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
                    gSprites[ballIconSpritesIds[j]].oam.tileNum++;
                    gSprites[ballIconSpritesIds[j]].data[7] = 1;
                }
                else if (partyInfo[i].hp == 0)
                {
                    gSprites[ballIconSpritesIds[j]].oam.tileNum += 3;
                }
                else if (partyInfo[i].status != 0)
                {
                    gSprites[ballIconSpritesIds[j]].oam.tileNum += 2;
                }
                j--;
            }
            else
            {
                if (i >= sp14)
                {
                    gSprites[ballIconSpritesIds[j]].oam.tileNum++;
                    gSprites[ballIconSpritesIds[j]].data[7] = 1;
                }
                else if (partyInfo[i].hp == 0)
                {
                    gSprites[ballIconSpritesIds[j]].oam.tileNum += 3;
                }
                else if (partyInfo[i].status != 0)
                {
                    gSprites[ballIconSpritesIds[j]].oam.tileNum += 2;
                }
                j--;
            }
        }
    }
    //_08044C38
    taskId = CreateTask(TaskDummy, 5);
    gTasks[taskId].data[0] = battler;
    gTasks[taskId].data[1] = summaryBarSpriteId;
    for (i = 0; i < 6; i++)
        gTasks[taskId].data[3 + i] = ballIconSpritesIds[i];
    gTasks[taskId].data[10] = isBattleStart;
    PlaySE12WithPanning(SE_BALL_TRAY_ENTER, 0);
    return taskId;
}

void Task_HidePartyStatusSummary(u8 taskId)
{
    u8 ballIconSpriteIds[6];
    bool8 isBattleStart;
    u8 summaryBarSpriteId;
    u8 battler;
    s32 i;

    isBattleStart = gTasks[taskId].data[10];
    summaryBarSpriteId = gTasks[taskId].data[1];
    battler = gTasks[taskId].data[0];
    for (i = 0; i < 6; i++)
        ballIconSpriteIds[i] = gTasks[taskId].data[3 + i];

    REG_BLDCNT = 0x3F40;
    REG_BLDALPHA = 0x10;
    gTasks[taskId].data[15] = 16;
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
        gTasks[taskId].data[15]--;
        if (gTasks[taskId].data[15] < 0)
            return;
        REG_BLDALPHA = (gTasks[taskId].data[15]) | ((16 - gTasks[taskId].data[15]) << 8);
    }
    if (gTasks[taskId].data[15] == 0)
        gTasks[taskId].func = Task_HidePartyStatusSummary_BattleStart_2;
}

static void Task_HidePartyStatusSummary_BattleStart_2(u8 taskId)
{
    u8 ballIconSpriteIds[6];
    s32 i;

    gTasks[taskId].data[15]--;
    if (gTasks[taskId].data[15] == -1)
    {
        u8 summaryBarSpriteId = gTasks[taskId].data[1];

        for (i = 0; i < 6; i++)
            ballIconSpriteIds[i] = gTasks[taskId].data[3 + i];
        DestroySpriteAndFreeResources(&gSprites[summaryBarSpriteId]);
        DestroySpriteAndFreeResources(&gSprites[ballIconSpriteIds[0]]);
        for (i = 1; i < 6; i++)
            DestroySprite(&gSprites[ballIconSpriteIds[i]]);
    }
    else if (gTasks[taskId].data[15] == -3)
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

    gTasks[taskId].data[15]--;
    // Same as above function except with this check.
    if (gTasks[taskId].data[15] >= 0)
    {
        REG_BLDALPHA = (gTasks[taskId].data[15]) | ((16 - gTasks[taskId].data[15]) << 8);
    }
    else if (gTasks[taskId].data[15] == -1)
    {
        u8 summaryBarSpriteId = gTasks[taskId].data[1];

        for (i = 0; i < 6; i++)
            ballIconSpriteIds[i] = gTasks[taskId].data[3 + i];
        DestroySpriteAndFreeResources(&gSprites[summaryBarSpriteId]);
        DestroySpriteAndFreeResources(&gSprites[ballIconSpriteIds[0]]);
        for (i = 1; i < 6; i++)
            DestroySprite(&gSprites[ballIconSpriteIds[i]]);
    }
    else if (gTasks[taskId].data[15] == -3)
    {
        REG_BLDCNT = 0;
        REG_BLDALPHA = 0;
        DestroyTask(taskId);
    }
}

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
    u8 var1;
    u16 var2;
    s8 pan;

    if (sprite->data[1] > 0)
    {
        sprite->data[1]--;
        return;
    }
    var1 = sprite->data[2];
    var2 = sprite->data[3];
    var2 += 56;
    sprite->data[3] = var2 & 0xFFF0;
    if (var1 != 0)
    {
        sprite->x2 += var2 >> 4;
        if (sprite->x2 > 0)
            sprite->x2 = 0;
    }
    else
    {
        sprite->x2 -= var2 >> 4;
        if (sprite->x2 < 0)
            sprite->x2 = 0;
    }
    if (sprite->x2 == 0)
    {
        pan = 63;
        if (var1 != 0)
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
    u8 var1;
    u16 var2;

    if (sprite->data[1] > 0)
    {
        sprite->data[1]--;
        return;
    }
    var1 = sprite->data[2];
    var2 = sprite->data[3];
    var2 += 56;
    sprite->data[3] = var2 & 0xFFF0;
    if (var1 != 0)
        sprite->x2 += var2 >> 4;
    else
        sprite->x2 -= var2 >> 4;
    if (sprite->x2 + sprite->x > 248
     || sprite->x2 + sprite->x < -8)
    {
        sprite->invisible = TRUE;
        sprite->callback = SpriteCallbackDummy;
    }
}

static void SpriteCB_StatusSummaryBalls_OnSwitchout(struct Sprite *sprite)
{
    u8 barSpriteId = sprite->data[0];

    sprite->x2 = gSprites[barSpriteId].x2;
    sprite->y2 = gSprites[barSpriteId].y2;
}

/*static*/ void UpdateNickInHealthbox(u8 healthboxSpriteId, struct Pokemon *mon)
{
    u8 nickname[POKEMON_NAME_LENGTH];
    u8 gender;
    u16 species;
    u8 language;
    u8 *ptr;
    s32 i;
    s32 _7;
    u8 *const *r1;

    StringCopy(gDisplayedStringBattle, gUnknown_0820A8B0);
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
    ptr = &eBattleInterfaceGfxBuffer[0x520 + GetBattlerPosition(gSprites[healthboxSpriteId].data[6]) * 0x180];
    RenderTextHandleBold(ptr, gDisplayedStringBattle);

    i = 0;
    _7 = 7;
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
                u8 r0;

                if ((*p >= 0x37 && *p <= 0x4A) || (*p >= 0x87 && *p <= 0x9A))
                    r0 = 0x2C;
                else if ((*p >= 0x4B && *p <= 0x4F) || (*p >= 0x9B && *p <= 0x9F))
                    r0 = 0x2D;
                else
                    r0 = 0x2B;

                CpuCopy32(GetHealthboxElementGfxPtr(r0), ptr + 0x40 * i, 32);
                i++;
                p++;
            }
        }
    }

    for (; i < _7; i++)
        CpuCopy32(GetHealthboxElementGfxPtr(0x2B), ptr + 64 * i, 32);

    if (GetBattlerSide(gSprites[healthboxSpriteId].data[6]) == 0 && !IsDoubleBattle())
    {
        r1 = (u8 *const *)gUnknown_0820A8B4;
        for (i = 0; i < _7; i++)
        {
            u8 *r4 = r1[i];

            r4 += gSprites[healthboxSpriteId].oam.tileNum * 32;
            CpuCopy32(ptr, r4, 32);
            ptr += 32;

            r4 += 0x100;
            CpuCopy32(ptr, r4, 32);
            ptr += 32;
        }
    }
    else
    {
        if (GetBattlerSide(gSprites[healthboxSpriteId].data[6]) == 0)
            r1 = (u8 *const *)gUnknown_0820A904;
        else
            r1 = (u8 *const *)gUnknown_0820A8DC;
        for (i = 0; i < _7; i++)
        {
            u8 *r4 = r1[i];

            r4 += gSprites[healthboxSpriteId].oam.tileNum * 32;
            CpuCopy32(ptr, r4, 32);
            ptr += 32;

            r4 += 0x100;
            CpuCopy32(ptr, r4, 32);
            ptr += 32;
        }
    }
}

static void TryAddPokeballIconToHealthbox(u8 healthboxSpriteId, bool8 noStatus)
{
    u8 r4;

    if (gBattleTypeFlags & 0x200)
        return;
    if (gBattleTypeFlags & 8)
        return;

    r4 = gSprites[healthboxSpriteId].data[6];
    if (GetBattlerSide(r4) != 0)
    {
        u16 species = GetMonData(&gEnemyParty[gBattlerPartyIndexes[r4]], MON_DATA_SPECIES);
        if (GetSetPokedexFlag(SpeciesToNationalPokedexNum(species), 1) != 0)
        {
            r4 = gSprites[healthboxSpriteId].data[5];
            if (noStatus != 0)
                CpuCopy32(GetHealthboxElementGfxPtr(0x46), OBJ_VRAM0 + (gSprites[r4].oam.tileNum + 8) * 32, 32);
            else
                CpuFill32(0, OBJ_VRAM0 + (gSprites[r4].oam.tileNum + 8) * 32, 32);
        }
    }
}

/*static*/ void UpdateStatusIconInHealthbox(u8 healthboxSpriteId)
{
    s32 status;
    s32 pltAdder;
    u8 battler;
    u8 healthBarSpriteId;
    s16 tileNumAdder;
    const u8 *statusGfxPtr;
    u8 statusPalId;
    s32 i;

    battler = gSprites[healthboxSpriteId].data[6];
    healthBarSpriteId = gSprites[healthboxSpriteId].data[5];
    if (GetBattlerSide(battler) == 0)
    {
        status = GetMonData(&gPlayerParty[gBattlerPartyIndexes[battler]], MON_DATA_STATUS);
        if (!IsDoubleBattle())
            tileNumAdder = 0x1A;
        else
            tileNumAdder = 0x12;
    }
    else
    {
        status = GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_STATUS);
        tileNumAdder = 0x11;
    }
    if (status & 7)
    {
        statusGfxPtr = GetHealthboxElementGfxPtr(GetStatusIconForBattlerId(0x1B, battler));
        statusPalId = 2;
    }
    else if (status & 0x88)
    {
        statusGfxPtr = GetHealthboxElementGfxPtr(GetStatusIconForBattlerId(0x15, battler));
        statusPalId = 0;
    }
    else if (status & 0x10)
    {
        statusGfxPtr = GetHealthboxElementGfxPtr(GetStatusIconForBattlerId(0x21, battler));
        statusPalId = 4;
    }
    else if (status & 0x20)
    {
        statusGfxPtr = GetHealthboxElementGfxPtr(GetStatusIconForBattlerId(0x1E, battler));
        statusPalId = 3;
    }
    else if (status & 0x40)
    {
        statusGfxPtr = GetHealthboxElementGfxPtr(GetStatusIconForBattlerId(0x18, battler));
        statusPalId = 1;
    }
    else
    {
        statusGfxPtr = GetHealthboxElementGfxPtr(0x27);

        for (i = 0; i < 3; i++)
            CpuCopy32(statusGfxPtr, OBJ_VRAM0 + (gSprites[healthboxSpriteId].oam.tileNum + tileNumAdder + i) * 32, 32);

        if (!gBattleSpriteInfo[battler].hpNumbersNoBars)
            CpuCopy32(GetHealthboxElementGfxPtr(1), OBJ_VRAM0 + gSprites[healthBarSpriteId].oam.tileNum * 32, 64);

        TryAddPokeballIconToHealthbox(healthboxSpriteId, 1);
        return;
    }

    pltAdder = gSprites[healthboxSpriteId].oam.paletteNum * 16;
    pltAdder += battler + 12;
    // I don't like writing the array index like this, but I can't get it to match otherwise.
    FillPalette(statusPalId[gBattleInterfaceStatusIcons_DynPal], pltAdder + 0x100, 2);
    CpuCopy16(gPlttBufferUnfaded + 0x100 + pltAdder, (void *)(OBJ_PLTT + pltAdder * 2), 2);
    CpuCopy32(statusGfxPtr, OBJ_VRAM0 + (gSprites[healthboxSpriteId].oam.tileNum + tileNumAdder) * 32, 96);
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

static u8 GetStatusIconForBattlerId(u8 statusElementId, u8 battler)
{
    u8 ret = statusElementId;

    switch (statusElementId)
    {
    case 21:
        if (battler == 0)
            ret = 21;
        else if (battler == 1)
            ret = 71;
        else if (battler == 2)
            ret = 86;
        else
            ret = 101;
        break;
    case 24:
        if (battler == 0)
            ret = 24;
        else if (battler == 1)
            ret = 74;
        else if (battler == 2)
            ret = 89;
        else
            ret = 104;
        break;
    case 27:
        if (battler == 0)
            ret = 27;
        else if (battler == 1)
            ret = 77;
        else if (battler == 2)
            ret = 92;
        else
            ret = 107;
        break;
    case 30:
        if (battler == 0)
            ret = 30;
        else if (battler == 1)
            ret = 80;
        else if (battler == 2)
            ret = 95;
        else
            ret = 110;
        break;
    case 33:
        if (battler == 0)
            ret = 33;
        else if (battler == 1)
            ret = 83;
        else if (battler == 2)
            ret = 98;
        else
            ret = 113;
        break;
    }
    return ret;
}

/*static*/ void UpdateSafariBallsTextOnHealthbox(u8 healthboxSpriteId)
{
    u8 *r6;
    u8 r8;
    u8 i;
    s32 r7;
    u8 *addr;

    r6 = &eBattleInterfaceGfxBuffer[0x520 + GetBattlerPosition(gSprites[healthboxSpriteId].data[6]) * 0x180];
    r8 = 7;
    RenderTextHandleBold(r6, gText_SafariBalls);
    for (i = 0; i < r8; i++)
        CpuCopy32(GetHealthboxElementGfxPtr(0x2B), r6 + i * 64, 32);
    for (r7 = 3; r7 < 3 + r8; r7++)
    {
        addr = OBJ_VRAM0 + (gSprites[healthboxSpriteId].oam.tileNum + MACRO1(r7)) * 32;
        CpuCopy32(r6, addr, 32);
        r6 += 32;

        addr = OBJ_VRAM0 + (8 + gSprites[healthboxSpriteId].oam.tileNum + MACRO1(r7)) * 32;
        CpuCopy32(r6, addr, 32);
        r6 += 32;
    }

}

/*static*/ void UpdateLeftNoOfBallsTextOnHealthbox(u8 healthboxSpriteId)
{
    u8 *r7;
    u8 status;
    s32 r6;
    s32 i;

    r7 = StringCopy(gDisplayedStringBattle, gText_SafariBallLeft);
    r7 = sub_8003504(r7, gNumSafariBalls, 10, 1);
    StringAppend(r7, BattleText_HighlightRed);
    status = GetBattlerPosition(gSprites[healthboxSpriteId].data[6]);
    r7 = &eBattleInterfaceGfxBuffer[0x520 + status * 0x180];
    r6 = 5;
    RenderTextHandleBold(r7, gDisplayedStringBattle);
    r7 = &eBattleInterfaceGfxBuffer[0x520 + status * 0x180 + 32];
    for (i = 6; i < 6 + r6; i++)
    {
        CpuCopy32(r7, OBJ_VRAM0 + (gSprites[healthboxSpriteId].oam.tileNum + 0x18 + MACRO1(i)) * 32, 32);
        r7 += 64;
    }
}

void UpdateHealthboxAttribute(u8 healthboxSpriteId, struct Pokemon *mon, u8 elementId)
{
    u8 battler;
    u32 maxHp;
    u32 currHp;

    battler = gSprites[healthboxSpriteId].data[6];
    if (GetBattlerSide(battler) == 0)
    {
        if (elementId == 3 || elementId == 0)
            UpdateLvlInHealthbox(healthboxSpriteId, GetMonData(mon, MON_DATA_LEVEL));
        if (elementId == 1 || elementId == 0)
            UpdateHpTextInHealthbox(healthboxSpriteId, GetMonData(mon, MON_DATA_HP), 0);
        if (elementId == 2 || elementId == 0)
            UpdateHpTextInHealthbox(healthboxSpriteId, GetMonData(mon, MON_DATA_MAX_HP), 1);
        if (elementId == 5 || elementId == 0)
        {
            LoadBattleBarGfx(0);
            maxHp = GetMonData(mon, MON_DATA_MAX_HP);
            currHp = GetMonData(mon, MON_DATA_HP);
            SetBattleBarStruct(battler, healthboxSpriteId, maxHp, currHp, 0);
            MoveBattleBar(battler, healthboxSpriteId, 0, 0);
        }
        if (!IsDoubleBattle() && (elementId == 6 || elementId == 0))
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
            currLevelExp = gExperienceTables[gSpeciesInfo[species].growthRate][level];
            currExpBarValue = exp - currLevelExp;
            maxExpBarValue = gExperienceTables[gSpeciesInfo[species].growthRate][level + 1] - currLevelExp;
            SetBattleBarStruct(battler, healthboxSpriteId, maxExpBarValue, currExpBarValue, 0);
            MoveBattleBar(battler, healthboxSpriteId, 1, 0);
        }
        if (elementId == 4 || elementId == 0)
            UpdateNickInHealthbox(healthboxSpriteId, mon);
        if (elementId == 9 || elementId == 0)
            UpdateStatusIconInHealthbox(healthboxSpriteId);
        if (elementId == 10)
            UpdateSafariBallsTextOnHealthbox(healthboxSpriteId);
        if (elementId == 10 || elementId == 11)
            UpdateLeftNoOfBallsTextOnHealthbox(healthboxSpriteId);
    }
    else
    {
        if (elementId == 3 || elementId == 0)
            UpdateLvlInHealthbox(healthboxSpriteId, GetMonData(mon, MON_DATA_LEVEL));
#if DEBUG
        if (gUnknown_020297ED == 1)
        {
            if (elementId == 1 || elementId == 0)
                UpdateHpTextInHealthbox(healthboxSpriteId, GetMonData(mon, MON_DATA_HP), 0);
            if (elementId == 2 || elementId == 0)
                UpdateHpTextInHealthbox(healthboxSpriteId, GetMonData(mon, MON_DATA_MAX_HP), 1);
        }
#endif
        if (elementId == 5 || elementId == 0)
        {
            LoadBattleBarGfx(0);
            maxHp = GetMonData(mon, MON_DATA_MAX_HP);
            currHp = GetMonData(mon, MON_DATA_HP);
            SetBattleBarStruct(battler, healthboxSpriteId, maxHp, currHp, 0);
            MoveBattleBar(battler, healthboxSpriteId, 0, 0);
        }
        if (elementId == 4 || elementId == 0)
            UpdateNickInHealthbox(healthboxSpriteId, mon);
        if (elementId == 9 || elementId == 0)
            UpdateStatusIconInHealthbox(healthboxSpriteId);
    }
}

s32 MoveBattleBar(u8 battler, u8 healthboxSpriteId, u8 whichBar, u8 unused)
{
    s32 currentBarValue;

    if (whichBar == 0)
    {
        currentBarValue = CalcNewBarValue(eBattleBars[battler].maxValue, eBattleBars[battler].oldValue, eBattleBars[battler].receivedValue, &eBattleBars[battler].currValue, 6, 1);
    }
    else
    {
        u16 expFraction;
        s32 receivedValue;

        expFraction = GetScaledExpFraction(eBattleBars[battler].oldValue, eBattleBars[battler].receivedValue, eBattleBars[battler].maxValue, 8);
        if (expFraction == 0)
            expFraction = 1;
        receivedValue = eBattleBars[battler].receivedValue;
        expFraction = ABS(receivedValue / expFraction);
        currentBarValue = CalcNewBarValue(eBattleBars[battler].maxValue, eBattleBars[battler].oldValue, receivedValue, &eBattleBars[battler].currValue, 8, expFraction);
    }
    if (whichBar == 1 || (whichBar == 0 && (!gBattleSpriteInfo[battler].hpNumbersNoBars)))
        MoveBattleBarGraphically(battler, whichBar);
    if (currentBarValue == -1)
        eBattleBars[battler].currValue = 0;
    return currentBarValue;
}

static void MoveBattleBarGraphically(u8 battler, u8 whichBar)
{
    u8 array[8];
    u8 r0;
    u8 barElementId;
    u8 i;

    switch (whichBar)
    {
    case 0:
        r0 = CalcBarFilledPixels(eBattleBars[battler].maxValue,
            eBattleBars[battler].oldValue,
            eBattleBars[battler].receivedValue,
            &eBattleBars[battler].currValue,
            array,
            6);
        barElementId = 3;
        if (r0 <= 0x18)
        {
            barElementId = 0x38;
            if (r0 > 9)
                barElementId = 0x2F;
        }
        for (i = 0; i < 6; i++)
        {
            u8 healthbarSpriteId = gSprites[eBattleBars[battler].healthboxSpriteId].data[5];
            if (i < 2)
                CpuCopy32(GetHealthboxElementGfxPtr(barElementId) + array[i] * 32, OBJ_VRAM0 + (gSprites[healthbarSpriteId].oam.tileNum + 2 + i) * 32, 32);
            else
                CpuCopy32(GetHealthboxElementGfxPtr(barElementId) + array[i] * 32, OBJ_VRAM0 + 64 + (i + gSprites[healthbarSpriteId].oam.tileNum) * 32, 32);
        }
        break;
    case 1:
        CalcBarFilledPixels(eBattleBars[battler].maxValue,
            eBattleBars[battler].oldValue,
            eBattleBars[battler].receivedValue,
            &eBattleBars[battler].currValue,
            array,
            8);
        r0 = GetMonData(&gPlayerParty[gBattlerPartyIndexes[battler]], MON_DATA_LEVEL);
        if (r0 == 100)
        {
            for (i = 0; i < 8; i++)
                array[i] = 0;
        }
        for (i = 0; i < 8; i++)
        {
            if (i < 4)
                CpuCopy32(GetHealthboxElementGfxPtr(0xC) + array[i] * 32, OBJ_VRAM0 + (gSprites[eBattleBars[battler].healthboxSpriteId].oam.tileNum + 0x24 + i) * 32, 32);
            else
                CpuCopy32(GetHealthboxElementGfxPtr(0xC) + array[i] * 32, OBJ_VRAM0 + 0xB80 + (i + gSprites[eBattleBars[battler].healthboxSpriteId].oam.tileNum) * 32, 32);
        }
        break;
    }
}

static s32 CalcNewBarValue(s32 maxValue, s32 oldValue, s32 receivedValue, s32 *currValue, u8 scale, u16 toAdd)
{
    u8 totalPixels = scale << 3;
    s32 r6;
    s32 ret;

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
        s32 var = *currValue >> 8;

        r6 = *currValue;
        if (oldValue == var && (r6 & 0xFF) == 0)
            return -1;
    }
    else
    {
        r6 = *currValue;
        if (oldValue == r6)
            return -1;
    }
    //_08045FC4
    if (maxValue < totalPixels)
    {
        s32 toAdd = (maxValue << 8) / totalPixels;

        if (receivedValue < 0)
        {
            *currValue = r6 + toAdd;
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
            *currValue = r6 - toAdd;
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

static u8 CalcBarFilledPixels(s32 maxValue, s32 oldValue, s32 receivedValue, s32 *currValue, u8 *pixelsArray, u8 scale)
{
    s32 newValue = oldValue - receivedValue;
    u8 r3;
    u8 i;
    u8 pixels;

    if (newValue < 0)
        newValue = 0;
    else if (newValue > maxValue)
        newValue = maxValue;
    r3 = scale << 3;
    for (i = 0; i < scale; i++)
        pixelsArray[i] = 0;
    if (maxValue < r3)
        pixels = *currValue * r3 / maxValue >> 8;
    else
        pixels = *currValue * r3 / maxValue;
    r3 = pixels;
    if (r3 == 0 && newValue > 0)
    {
        pixelsArray[0] = 1;
        r3 = 1;
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
    return r3;
}

// These two functions seem as if they were made for testing the health bar.
s16 sub_80460C8(struct BattleInterfaceStruct1 *barInfo, int *currValue, u16 *dest, int unused)
{
    u16 r7;
    s16 r1;

    r7 = CalcNewBarValue(barInfo->maxValue, barInfo->oldValue, barInfo->receivedValue, currValue, 6, 1);
    sub_8046128(barInfo, currValue, dest);
    if (barInfo->maxValue < 0x30)
        r1 = *currValue >> 8;
    else
        r1 = *currValue;
    DummiedOutFunction(barInfo->maxValue, r1, unused);
    return r7;
}

static void sub_8046128(struct BattleInterfaceStruct1 *barInfo, int *currValue, u16 *dest)
{
    u8 pixels[6];
    u16 src[6];
    u8 i;

    CalcBarFilledPixels(barInfo->maxValue, barInfo->oldValue, barInfo->receivedValue, currValue, (u8 *)pixels, 6);
    for (i = 0; i < 6; i++)
        src[i] = (barInfo->unkC_0 << 12) | (barInfo->unk10 + pixels[i]);
    CpuCopy16(src, dest, sizeof(src));
}

static u8 GetScaledExpFraction(s32 oldValue, s32 receivedValue, s32 maxValue, u8 scale)
{
    u8 totalPixels = scale * 8;
    s32 newVal = oldValue - receivedValue;
    s8 oldToMax;
    s8 newToMax;
    s32 result;

    if (newVal < 0)
        newVal = 0;
    else if (newVal > maxValue)
        newVal = maxValue;

    oldToMax = oldValue * totalPixels / maxValue;
    newToMax = newVal * totalPixels / maxValue;
    result = oldToMax - newToMax;
    return ABS(result);
}

u8 GetScaledHPFraction(s16 hp, s16 maxhp, u8 scale)
{
    u8 result = hp * scale / maxhp;

    if (result == 0 && hp > 0)
        return 1;
    return result;
}

u8 GetHPBarLevel(s16 hp, s16 maxhp)
{
    int result;

    if (hp == maxhp)
        result = 4;
    else
    {
        u8 fraction = GetScaledHPFraction(hp, maxhp, 48);
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
