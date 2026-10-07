#include "global.h"
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
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "trig.h"
#include "scanline_effect.h"

asm(".include \"constants/gba_constants.inc\"");

static u8 CreateWheelIconSprite(const struct SpriteTemplate *template, u8 r1, u16 *angle);
static void UNUSED DestroyGridSprites(void);
static void SpriteCB_WheelIcon(struct Sprite *sprite);
static u8 GetMultiplierAnimId(u8 selectionId);
static void SpriteCB_WheelCenter(struct Sprite *sprite);
static s16 UpdateBallRelativeWheelAngle(struct Sprite *sprite);
static u8 UpdateSlotBelowBall(struct Sprite *sprite);
static s16 GetBallDistanceToSlotMidpoint(struct Sprite *sprite);
static void UpdateBallPos(struct Sprite *sprite);
static void SpriteCB_BallLandInSlot(struct Sprite *sprite);
static void SpriteCB_UnstickBall_ShroomishBallFall(struct Sprite *sprite);
static void SpriteCB_UnstickBall_Shroomish(struct Sprite *sprite);
static void SpriteCB_UnstickBall_TaillowDrop(struct Sprite *sprite);
static void SpriteCB_UnstickBall_TaillowPickUp(struct Sprite *sprite);
static void SpriteCB_UnstickBall_Taillow(struct Sprite *sprite);
static void SpriteCB_UnstickBall(struct Sprite *sprite);
static void SpriteCB_RollBall_TryLandAdjacent(struct Sprite *sprite);
static void SpriteCB_RollBall_TryLand(struct Sprite *sprite);
static void SpriteCB_RollBall_Medium(struct Sprite *sprite);
static void SpriteCB_RollBall_Fast(struct Sprite *sprite);
static void CreateShroomishSprite(struct Sprite *ball);
static void CreateTaillowSprite(struct Sprite *ball);
static void SetBallStuck(struct Sprite *sprite);
static void SpriteCB_ShroomishExit(struct Sprite *sprite);
static void SpriteCB_ShroomishShakeScreen(struct Sprite *sprite);
static void SpriteCB_ShroomishFall(struct Sprite *sprite);
static void SpriteCB_Shroomish(struct Sprite *sprite);
static void SpriteCB_TaillowShadow_Flash(struct Sprite *sprite);
static void SpriteCB_Taillow_FlyAway(struct Sprite *sprite);
static void SpriteCB_Taillow_PickUpBall(struct Sprite *sprite);
static void SpriteCB_Taillow_FlyIn(struct Sprite *sprite);
static void SpriteCB_TaillowShadow_FlyIn(struct Sprite *sprite);
static void SpriteCB_Taillow(struct Sprite *sprite);
void sub_8118F8C(struct Sprite *sprite);

#if DEBUG
extern u8 unk_2039560;
#endif //DEBUG

#ifdef ENGLISH
ALIGNED(4) const u16 RouletteSpritePalette_01[] = INCBIN_U16("graphics/roulette/shadow.gbapal");
const u16 RouletteSpritePalette_02[] = INCBIN_U16("graphics/roulette/ball.gbapal");
const u16 RouletteSpritePalette_03[] = INCBIN_U16("graphics/roulette/ball_counter.gbapal");
const u16 RouletteSpritePalette_04[] = INCBIN_U16("graphics/roulette/cursor.gbapal");
const u16 RouletteSpritePalette_05[] = INCBIN_U16("graphics/roulette/credit.gbapal");
const u16 RouletteSpritePalette_06[] = INCBIN_U16("graphics/roulette/shroomish.gbapal");
const u16 RouletteSpritePalette_07[] = INCBIN_U16("graphics/roulette/tailow.gbapal");
const u16 RouletteSpritePalette_08[] = INCBIN_U16("graphics/roulette/grid_icons.gbapal");
const u16 RouletteSpritePalette_09[] = INCBIN_U16("graphics/roulette/wynaut.gbapal");
const u16 RouletteSpritePalette_10[] = INCBIN_U16("graphics/roulette/azurill.gbapal");
const u16 RouletteSpritePalette_11[] = INCBIN_U16("graphics/roulette/skitty.gbapal");
const u16 RouletteSpritePalette_12[] = INCBIN_U16("graphics/roulette/makuhita.gbapal");
const u16 RouletteSpritePalette_13[] = INCBIN_U16("graphics/roulette/unused_1.gbapal");
const u16 RouletteSpritePalette_14[] = INCBIN_U16("graphics/roulette/unused_2.gbapal");
const u16 RouletteSpritePalette_15[] = INCBIN_U16("graphics/roulette/unused_3.gbapal");
const u16 RouletteSpritePalette_16[] = INCBIN_U16("graphics/roulette/unused_4.gbapal");
#else
extern const u16 RouletteSpritePalette_01[];
extern const u16 RouletteSpritePalette_02[];
extern const u16 RouletteSpritePalette_03[];
extern const u16 RouletteSpritePalette_04[];
extern const u16 RouletteSpritePalette_05[];
extern const u16 RouletteSpritePalette_06[];
extern const u16 RouletteSpritePalette_07[];
extern const u16 RouletteSpritePalette_08[];
extern const u16 RouletteSpritePalette_09[];
extern const u16 RouletteSpritePalette_10[];
extern const u16 RouletteSpritePalette_11[];
extern const u16 RouletteSpritePalette_12[];
extern const u16 RouletteSpritePalette_13[];
extern const u16 RouletteSpritePalette_14[];
extern const u16 RouletteSpritePalette_15[];
extern const u16 RouletteSpritePalette_16[];
#endif //ENGLISH

static const u8 sBall_Gfx[] = INCBIN_U8("graphics/roulette/ball.4bpp.lz");
static const u8 sBallCounter_Gfx[] = INCBIN_U8("graphics/roulette/ball_counter.4bpp.lz");
static const u8 sShroomishTaillow_Gfx[] = INCBIN_U8("graphics/roulette/roulette_tilt.4bpp.lz");
static const u8 sGridIcons_Gfx[] = INCBIN_U8("graphics/roulette/grid_icons.4bpp.lz");
static const u8 sWheelIcons_Gfx[] = INCBIN_U8("graphics/roulette/wheel_icons.4bpp.lz");
static const u8 sShadow_Gfx[] = INCBIN_U8("graphics/roulette/shadow.4bpp.lz");
static const u8 sCursor_Gfx[] = INCBIN_U8("graphics/roulette/cursor.4bpp.lz");

static const struct SpritePalette sSpritePalettes[] = {
    {RouletteSpritePalette_01, 1 },
    {RouletteSpritePalette_02, 2 },
    {RouletteSpritePalette_03, 3 },
    {RouletteSpritePalette_04, 4 },
    {RouletteSpritePalette_05, 5 },
    {RouletteSpritePalette_06, 6 },
    {RouletteSpritePalette_07, 7 },
    {RouletteSpritePalette_08, 8 },
    {RouletteSpritePalette_09, 9 },
    {RouletteSpritePalette_10, 10},
    {RouletteSpritePalette_11, 11},
    {RouletteSpritePalette_12, 12},
    {RouletteSpritePalette_13, 13},
    {RouletteSpritePalette_14, 14},
    {RouletteSpritePalette_15, 15},
    {RouletteSpritePalette_16, 16},
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

const union AnimCmd gSpriteAnim_83F9ED0[] = {
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END
};

const union AnimCmd *const gSpriteAnimTable_83F9ED8[] = {
    gSpriteAnim_83F9ED0
};

static const union AffineAnimCmd sAffineAnim_Unused2[] = {
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd *const sAffineAnims_Unused2[] = {
    sAffineAnim_Unused2
};

static const struct CompressedSpriteSheet sSpriteSheet_WheelIcons = {
    sWheelIcons_Gfx,
    0xc00,
    0
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
    4
};

static const struct CompressedSpriteSheet sSpriteSheet_GridIcons = {
    sGridIcons_Gfx,
    0x400,
    5
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
    {4, 8, &sOam_GridHeader, sAnim_WynautHeader, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare},
    {4, 8, &sOam_GridHeader, sAnim_AzurillHeader, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare},
    {4, 8, &sOam_GridHeader, sAnim_SkittyHeader, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare},
    {4, 8, &sOam_GridHeader, sAnim_MakuhitaHeader, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare}
};

static const struct SpriteTemplate sSpriteTemplates_ColorHeaders[] = {
    {4, 8, &sOam_GridHeader, sAnim_OrangeHeader, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare},
    {4, 8, &sOam_GridHeader, sAnim_GreenHeader, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare},
    {4, 8, &sOam_GridHeader, sAnim_PurpleHeader, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare}
};

static const struct SpriteTemplate sSpriteTemplates_GridIcons[] ={
    {5, 8, &sOam_GridIcon, sAnim_GridIcon_Wynaut, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare},
    {5, 8, &sOam_GridIcon, sAnim_GridIcon_Azurill, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare},
    {5, 8, &sOam_GridIcon, sAnim_GridIcon_Skitty, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare},
    {5, 8, &sOam_GridIcon, sAnim_GridIcon_Makuhita, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare}
};

static const struct SpriteTemplate sSpriteTemplates_WheelIcons[] = {
    {0, 9, &sOam_WheelIcon, sAnim_WheelIcon_OrangeWynaut, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {0, 10, &sOam_WheelIcon, sAnim_WheelIcon_GreenAzurill, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {0, 11, &sOam_WheelIcon, sAnim_WheelIcon_PurpleSkitty, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {0, 12, &sOam_WheelIcon, sAnim_WheelIcon_OrangeMakuhita, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {0, 9, &sOam_WheelIcon, sAnim_WheelIcon_GreenWynaut, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {0, 10, &sOam_WheelIcon, sAnim_WheelIcon_PurpleAzurill, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {0, 11, &sOam_WheelIcon, sAnim_WheelIcon_OrangeSkitty, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {0, 12, &sOam_WheelIcon, sAnim_WheelIcon_GreenMakuhita, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {0, 9, &sOam_WheelIcon, sAnim_WheelIcon_PurpleWynaut, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {0, 10, &sOam_WheelIcon, sAnim_WheelIcon_OrangeAzurill, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {0, 11, &sOam_WheelIcon, sAnim_WheelIcon_GreenSkitty, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {0, 12, &sOam_WheelIcon, sAnim_WheelIcon_PurpleMakuhita, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon}
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
    {gRouletteCredit_Gfx, 0x400, 7},
    {gRouletteNumbers_Gfx, 0x280, 8},
    {gRouletteMultiplier_Gfx, 0x500, 9},
    {sBallCounter_Gfx, 0x140, 10},
    {sCursor_Gfx, 0x200, 11},
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

static const struct SpriteTemplate sSpriteTemplate_Credit = {7, 5, &sOam_Credit, gDummySpriteAnimTable, NULL, gDummySpriteAffineAnimTable, SpriteCallbackDummy};

static const struct SpriteTemplate sSpriteTemplate_CreditDigit = {8, 5, &sOam_CreditDigit, sAnims_CreditDigit, NULL, gDummySpriteAffineAnimTable, SpriteCallbackDummy};

static const struct SpriteTemplate sSpriteTemplate_Multiplier = {9, 5, &sOam_Multiplier, sAnims_Multiplier, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare};

static const struct SpriteTemplate sSpriteTemplate_BallCounter = {10, 3, &sOam_BallCounter, sAnims_BallCounter, NULL, gDummySpriteAffineAnimTable, SpriteCallbackDummy};

static const struct SpriteTemplate sSpriteTemplate_Cursor = {11, 5, &sOam_GridHeader, gDummySpriteAnimTable, NULL, gDummySpriteAffineAnimTable, SpriteCallbackDummy};

static const struct OamData sOam_Ball = {
    .shape = ST_OAM_SQUARE,
    .size = 1,
    .priority = 2
};

const struct SpriteFrameImage gSpriteImageTable_83FA330[] = {
    {(void *)ewram17E00 + 0, 128},
    {(void *)ewram17E00 + 128, 128},
    {(void *)ewram17E00 + 0x100, 128},
    {(void *)ewram17E00 + 0x180, 128}
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

static const struct SpriteTemplate sSpriteTemplate_Ball = {0xffff, 2, &sOam_Ball, sAnims_Ball, gSpriteImageTable_83FA330, gDummySpriteAffineAnimTable, SpriteCallbackDummy};

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
    6
};

static const struct SpriteTemplate sSpriteTemplate_WheelCenter = {6, 2, &sOam_WheelCenter, gDummySpriteAnimTable, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelCenter};

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

const struct SpriteFrameImage gSpriteImageTable_83FA45C[] = {
    {(void *)ewram17000 + 0, 0x200},
    {(void *)ewram17000 + 0x200, 0x200},
    {(void *)ewram17000 + 0x400, 0x200},
    {(void *)ewram17000 + 0x600, 0x200},
    {(void *)ewram17000 + 0x800, 0x200},
    {(void *)ewram17000 + 0xa00, 0x200},
    {(void *)ewram17000 + 0xc00, 0x200}
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

static const struct SpriteTemplate sSpriteTemplate_Shroomish = {0xffff, 6, &sOam_Shroomish, sAnims_Shroomish, gSpriteImageTable_83FA45C, gDummySpriteAffineAnimTable, SpriteCallbackDummy};

static const struct SpriteTemplate sSpriteTemplate_Taillow = {0xffff, 7, &sOam_Taillow, sAnims_Taillow, gSpriteImageTable_83FA45C, gDummySpriteAffineAnimTable, SpriteCB_Taillow};

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

const struct SpriteFrameImage gSpriteImageTable_83FA554[] = {
    {(void *)ewram18000 + 0, 128}
};

const struct SpriteFrameImage gSpriteImageTable_83FA55C[] = {
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
    {0xffff, 1, &sOam_ShroomishBallShadow, gSpriteAnimTable_83F9ED8, gSpriteImageTable_83FA554, gDummySpriteAffineAnimTable, SpriteCallbackDummy},
    {0xffff, 1, &sOam_ShroomishShadow, gSpriteAnimTable_83F9ED8, gSpriteImageTable_83FA55C, gDummySpriteAffineAnimTable, SpriteCB_Shroomish}
};

static const struct SpriteTemplate sSpriteTemplate_TaillowShadow = {0xffff, 1, &sOam_TaillowShadow, gSpriteAnimTable_83F9ED8, gSpriteImageTable_83FA55C, sAffineAnims_TaillowShadow, SpriteCB_Taillow};

void sub_8117838(u8 free)
{
    if (!free)
    {
        FreeAllSpritePalettes();
        LoadSpritePalettes(sSpritePalettes);
        LZ77UnCompWram(sShroomishTaillow_Gfx, ewram17000);
        LZ77UnCompWram(sBall_Gfx, ewram17E00);
        LZ77UnCompWram(sShadow_Gfx, ewram18000);
    }
    else
    {
        FreeAllSpritePalettes();
    }
}

static u8 CreateWheelIconSprite(const struct SpriteTemplate *template, u8 r1, u16 *angle)
{
    u16 temp;
    u8 spriteId = CreateSprite(template, 116, 80, template->oam->y);
    gSprites[spriteId].data[0]            = *angle;
    gSprites[spriteId].data[1]            = r1;
    gSprites[spriteId].coordOffsetEnabled = TRUE;
    gSprites[spriteId].animPaused         = TRUE;
    gSprites[spriteId].affineAnimPaused   = TRUE;
    temp = *angle;
    *angle += 30;
    if (*angle >= 360)
        *angle = temp - 330;
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
    for (i = 0; i < 3; i++)
    {
        u8 y = i * 24;
        for (j = 0; j < 4; j++)
        {
            spriteId = eRoulette->spriteIds[29 + i * 4 + j] =
                CreateSprite(&sSpriteTemplates_GridIcons[j], j * 24 + 148, y + 92, 30);
            gSprites[spriteId].animPaused = TRUE;
            y += 24;
            if (y > 71)
                y = 0;
        }
    }
    for (i = 0; i < 4; i++)
    {
        spriteId = eRoulette->spriteIds[41 + i] =
            CreateSprite(&sSpriteTemplates_PokeHeaders[i], (s16)(i * 24 + 148), 70, 30);
        gSprites[spriteId].animPaused = TRUE;
    }
    for (i = 0; i < 3; i++)
    {
        spriteId = eRoulette->spriteIds[45 + i] =
            CreateSprite(&sSpriteTemplates_ColorHeaders[i], 126, (s16)(i * 24 + 92), 30);
        gSprites[spriteId].animPaused = TRUE;
    }
}

static void UNUSED DestroyGridSprites(void) //destroy all sprites at 29
{
    u8 i;
    for (i = 0; i < 12; i++)
        DestroySprite(&gSprites[eRoulette->spriteIds[29 + i]]);
}

void ShowHideGridIcons(u8 hideAll, u8 hideSquare)
{
    u8 i;
    switch (hideAll)
    {
    case 1:
        for (i = 0; i < 19; i++)
            gSprites[eRoulette->spriteIds[29 + i]].invisible = TRUE;
        break;
    case 0:
        for (i = 0; i < 12; i++)
        {
            if (!(eRoulette->hitFlags & gUnknown_083F8D90[i].flag))
                gSprites[eRoulette->spriteIds[29 + i]].invisible = FALSE;
            else if(!(gUnknown_083F8D90[i].gridSquare == hideSquare))
                gSprites[eRoulette->spriteIds[29 + i]].invisible = TRUE;
            else
                gSprites[eRoulette->spriteIds[29 + i]].invisible = FALSE;
        }
        for (; i < 19; i++)
            gSprites[eRoulette->spriteIds[29 + i]].invisible = FALSE;
        break;
    }
}

void CreateGridBallSprites(void)
{
    u8 i;
    for (i = 0; i < 6; i++)
    {
        eRoulette->spriteIds[49 + i] = CreateSprite(&sSpriteTemplate_Ball, 116, 20, 10);
        gSprites[eRoulette->spriteIds[49 + i]].invisible    = TRUE;
        gSprites[eRoulette->spriteIds[49 + i]].data[0]      = 1;
        gSprites[eRoulette->spriteIds[49 + i]].callback     = SpriteCB_GridSquare;
        gSprites[eRoulette->spriteIds[49 + i]].oam.priority = 1;
        StartSpriteAnim(&gSprites[eRoulette->spriteIds[49 + i]], 8);
    }
}

void ShowHideGridBalls(u8 hideAll, u8 hideBallId)
{
    u8 i = 0;
    if (hideAll)
    {
        for ( ; i < 6; i++)
            gSprites[eRoulette->spriteIds[49 + i]].invisible = TRUE;
    }
    else
    {
        for ( ; i < 6; i++)
        {
            if (!(eRoulette->hitSquares[i]) || (i == hideBallId))
            {
                gSprites[eRoulette->spriteIds[49 + i]].invisible = TRUE;
            }
            else
            {
                gSprites[eRoulette->spriteIds[49 + i]].invisible = FALSE;
                gSprites[eRoulette->spriteIds[49 + i]].x         = (gUnknown_083F8C00[eRoulette->hitSquares[i]].x + 1) * 8 + 4;
                gSprites[eRoulette->spriteIds[49 + i]].y         = (gUnknown_083F8C00[eRoulette->hitSquares[i]].y + 1) * 8 + 3;
            }
        }
    }
}

void ShowHideWinSlotCursor(u8 selectionId)
{
    if (!selectionId)
    {
        gSprites[eRoulette->spriteIds[48]].invisible = TRUE;
    }
    else
    {
        gSprites[eRoulette->spriteIds[48]].invisible = FALSE;
        gSprites[eRoulette->spriteIds[48]].x    = (gUnknown_083F8C00[selectionId].x + 2) * 8;
        gSprites[eRoulette->spriteIds[48]].y    = (gUnknown_083F8C00[selectionId].y + 2) * 8;
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
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 4; j++)
        {
            u8 spriteId;
            spriteId =
                eRoulette->spriteIds[7 + i * 4 + j] = CreateWheelIconSprite(&sSpriteTemplates_WheelIcons[i * 4 + j], 40, &angle);
            gSprites[spriteId].animPaused       = TRUE;
            gSprites[spriteId].affineAnimPaused = TRUE;
        }
    }
}

static void SpriteCB_WheelIcon(struct Sprite *sprite)
{
    s16 cos;
    s16 sin;
    u32 matrixNum;
    s16 angle = eRoulette->wheelAngle + sprite->data[0];
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
    eRoulette->spriteIds[20] = CreateSprite(&sSpriteTemplate_Credit, 208, 16, 4);
    gSprites[eRoulette->spriteIds[20]].animPaused = TRUE;
    for (i = 0; i < 4; i++)
    {
        eRoulette->spriteIds[21 + i] = CreateSprite(&sSpriteTemplate_CreditDigit, (s16)(i * 8 + 196), 24, 0);
        gSprites[eRoulette->spriteIds[21 + i]].invisible = TRUE;
        gSprites[eRoulette->spriteIds[21 + i]].animPaused = TRUE;
    }
    eRoulette->spriteIds[25] = CreateSprite(&sSpriteTemplate_Multiplier, 120, 68, 4);
    gSprites[eRoulette->spriteIds[25]].animPaused = TRUE;
    for (i = 0; i < 3; i++)
    {
        eRoulette->spriteIds[26 + i] = CreateSprite(&sSpriteTemplate_BallCounter, (s16)(i * 16 + 192), 36, 4);
        gSprites[eRoulette->spriteIds[26 + i]].invisible = TRUE;
        gSprites[eRoulette->spriteIds[26 + i]].animPaused = TRUE;
    }
    eRoulette->spriteIds[48] = CreateSprite(&sSpriteTemplate_Cursor, 152, 96, 9);
    gSprites[eRoulette->spriteIds[48]].oam.priority = 1;
    gSprites[eRoulette->spriteIds[48]].animPaused = TRUE;
    gSprites[eRoulette->spriteIds[48]].invisible = TRUE;
}

void SetCreditDigits(u16 num)
{
    u8 i;
    u16 d = 1000;
    bool8 printZero = FALSE;
    for (i = 0; i < 4; i++)
    {
        u8 digit = num / d;
        gSprites[eRoulette->spriteIds[21 + i]].invisible = TRUE;
        if (digit > 0 || printZero || i == 3)
        {
            gSprites[eRoulette->spriteIds[21 + i]].invisible = FALSE;
            gSprites[eRoulette->spriteIds[21 + i]].oam.tileNum =
                gSprites[eRoulette->spriteIds[21 + i]].sheetTileStart
                + (*gSprites[eRoulette->spriteIds[21 + i]].anims + digit)->type;
            printZero = TRUE;
        }
        num = num % d;
        d = d / 10;
    }
}

static u8 GetMultiplierAnimId(u8 selectionId)
{
    u8 animIds[] = {0, 1, 2, 3, 4};
    if (selectionId >= 20)
        selectionId = 0;
    switch (gUnknown_083F8C00[selectionId].baseMultiplier)
    {
    case 3:
        selectionId = selectionId / 5 - 1;
        if (eRoulette->colorHits[selectionId] < 4)
            return animIds[((vu8 *)eRoulette->colorHits)[selectionId] + 1]; // couldn't recreate redundant loads
        break;
    case 4:
        selectionId--;
        if (eRoulette->pokeHits[selectionId] < 3)
            return animIds[((vu8 *)eRoulette->pokeHits)[selectionId] + 2];
        break;
    case 12:
        if (!(eRoulette->hitFlags & gUnknown_083F8C00[selectionId].flag))
            return animIds[4];
        break;
    }
    return 0;
}

void SetMultiplierSprite(u8 selectionId)
{
    struct Sprite *sprite = &gSprites[eRoulette->spriteIds[25]];
    sprite->animCmdIndex  = GetMultiplierAnimId(selectionId);
    sprite->oam.tileNum   =
        sprite->sheetTileStart
        + (*sprite->anims + sprite->animCmdIndex)->type;
}

void SetBallCounterNumLeft(u8 numBalls)
{
    u8 i;
    u8 t = 0;
    if (eRoulette->minBet == 1)
        t = 2;
    switch (numBalls)
    {
    case 6:
        for (i = 0; i < 3; i++)
        {
            gSprites[eRoulette->spriteIds[26 + i]].invisible = FALSE;
            gSprites[eRoulette->spriteIds[26 + i]].oam.tileNum =
                gSprites[eRoulette->spriteIds[26 + i]].sheetTileStart
                + (*gSprites[eRoulette->spriteIds[26 + i]].anims)->type;
        }
        break;
    case 5:
        gSprites[eRoulette->spriteIds[28]].oam.tileNum =
            gSprites[eRoulette->spriteIds[28]].sheetTileStart
            + (*gSprites[eRoulette->spriteIds[28]].anims + t + 1)->type;
        break;
    case 4:
        gSprites[eRoulette->spriteIds[28]].oam.tileNum =
            gSprites[eRoulette->spriteIds[28]].sheetTileStart
            + (*gSprites[eRoulette->spriteIds[28]].anims + t + 2)->type;
        break;
    case 3:
        gSprites[eRoulette->spriteIds[27]].oam.tileNum =
            gSprites[eRoulette->spriteIds[27]].sheetTileStart
            + (*gSprites[eRoulette->spriteIds[27]].anims + t + 1)->type;
        break;
    case 2:
        gSprites[eRoulette->spriteIds[27]].oam.tileNum =
            gSprites[eRoulette->spriteIds[27]].sheetTileStart
            + (*gSprites[eRoulette->spriteIds[27]].anims + t + 2)->type;
        break;
    case 1:
        gSprites[eRoulette->spriteIds[26]].oam.tileNum =
            gSprites[eRoulette->spriteIds[26]].sheetTileStart
            + (*gSprites[eRoulette->spriteIds[26]].anims + t + 1)->type;
        break;
    case 0:
    default:
        for (i = 0; i < 3; i++)
        {
            gSprites[eRoulette->spriteIds[26 + i]].oam.tileNum =
                gSprites[eRoulette->spriteIds[26 + i]].sheetTileStart
                + (*gSprites[eRoulette->spriteIds[26 + i]].anims + t + 2)->type;
        }
    }
}

void SpriteCB_GridSquare(struct Sprite *sprite)
{
    sprite->x2 = eRoulette->gridX;
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
    gSprites[spriteId].data[0]            = eRoulette->wheelAngle;
    gSprites[spriteId].data[1]            = 0;
    gSprites[spriteId].animPaused         = TRUE;
    gSprites[spriteId].affineAnimPaused   = TRUE;
    gSprites[spriteId].coordOffsetEnabled = TRUE;
}

static void SpriteCB_WheelCenter(struct Sprite *sprite)
{
    u32 matrixNum = sprite->oam.matrixNum;
    struct OamMatrix *matrix = gOamMatrices;
    matrix[matrixNum].d = eRoulette->wheelRotation.a;
    matrix[matrixNum].a = eRoulette->wheelRotation.a;
    matrix[matrixNum].b = eRoulette->wheelRotation.b;
    matrix[matrixNum].c = eRoulette->wheelRotation.c;
}

void CreateWheelBallSprites(void)
{
    u8 i;
    for (i = 0; i < 6; i++)
    {
        u8 spriteid =
            eRoulette->spriteIds[i] = CreateSprite(&sSpriteTemplate_Ball, 116, 80, 57 - i);
        if (spriteid != 64)
        {
            gSprites[eRoulette->spriteIds[i]].invisible          = TRUE;
            gSprites[eRoulette->spriteIds[i]].coordOffsetEnabled = TRUE;
        }
    }
}

void HideWheelBalls(void)
{
    u8 spriteId = eRoulette->spriteIds[0];
    u8 i;
    for (i = 0; i < 6; i++)
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

static s16 UpdateBallRelativeWheelAngle(struct Sprite *sprite)
{
    if (eRoulette->wheelAngle > sprite->data[3])
    {
        sprite->data[6] = 360 - eRoulette->wheelAngle + sprite->data[3];
        if (sprite->data[6] >= 360)
            sprite->data[6] -= 360;
    }
    else
        sprite->data[6] = sprite->data[3] - eRoulette->wheelAngle;
    return sprite->data[6];
}

static u8 UpdateSlotBelowBall(struct Sprite *sprite)
{
    eRoulette->hitSlot = (u8)(((float)(s16)UpdateBallRelativeWheelAngle(sprite)) / 30.0f);
    return eRoulette->hitSlot;
}

static s16 GetBallDistanceToSlotMidpoint(struct Sprite *sprite)
{
    s16 angleIntoSlot = UpdateBallRelativeWheelAngle(sprite) % 30;
    u16 distanceToMidpoint;
    if (angleIntoSlot == 14)
    {
        distanceToMidpoint = 0;
        return sprite->data[2] = distanceToMidpoint;
    }
    else if (angleIntoSlot > 13)
    {
        distanceToMidpoint = (u16)(43 - angleIntoSlot);
        return sprite->data[2] = distanceToMidpoint;
    }
    else
    {
        distanceToMidpoint = (u16)(14 - angleIntoSlot);
        return sprite->data[2] = distanceToMidpoint;
    }
}

static void UpdateBallPos(struct Sprite *sprite)
{
    s16 sin, cos;
    eRoulette->ballAngleSpeed += eRoulette->ballAngleAccel;
    eRoulette->ballAngle += eRoulette->ballAngleSpeed;
    if (eRoulette->ballAngle >= 360)
        eRoulette->ballAngle -= 360.0f;
    else if (eRoulette->ballAngle < 0.0f)
        eRoulette->ballAngle += 360.0f;
    sprite->data[3] = eRoulette->ballAngle;
    eRoulette->ballFallSpeed += eRoulette->ballFallAccel;
    eRoulette->ballDistToCenter += eRoulette->ballFallSpeed;
    sprite->data[4] = eRoulette->ballDistToCenter;
    sin = Sin2(sprite->data[3]);
    cos = Cos2(sprite->data[3]);
    sprite->x2 =  sin * sprite->data[4] >> 12;
    sprite->y2 = -cos * sprite->data[4] >> 12;
    if (IsSEPlaying())
    {
        m4aMPlayPanpotControl(&gMPlayInfo_SE1, 0xffff, sprite->x2);
        m4aMPlayPanpotControl(&gMPlayInfo_SE2, 0xffff, sprite->x2);
    }
}

static void SpriteCB_BallLandInSlot(struct Sprite *sprite)
{
    s16 sin, cos;
    sprite->data[3] = eRoulette->wheelAngle + sprite->data[6];
    if (sprite->data[3] >= 360)
        sprite->data[3] -= 360;
    sin = Sin2(sprite->data[3]);
    cos = Cos2(sprite->data[3]);
    sprite->x2 =  sin * sprite->data[4] >> 12;
    sprite->y2 = -cos * sprite->data[4] >> 12;
    sprite->y2 += gSpriteCoordOffsetY;
}

static void SpriteCB_UnstickBall_ShroomishBallFall(struct Sprite *sprite)
{
    UpdateBallPos(sprite);
    sprite->data[2]++;
    if ((u16)(sprite->data[4] + 132) > 212)
        sprite->invisible = TRUE;
    else
        sprite->invisible = FALSE;
    if (!(sprite->data[2] < 30))
    {
        if (!sprite->data[0])
        {
            if (eRoulette->ballDistToCenter <= eRoulette->varA0 - 2.0f)
            {
                eRoulette->ballState = 255;
                eRoulette->ballRolling = 0;
                StartSpriteAnim(sprite, sprite->animCmdIndex + 3);
                UpdateSlotBelowBall(sprite);
                sprite->data[4] = 30;
                UpdateBallRelativeWheelAngle(sprite);
                sprite->data[6] = (sprite->data[6] / 30) * 30 + 15;
                sprite->callback = SpriteCB_BallLandInSlot;
                m4aSongNumStartOrChange(SE_BRIDGE_WALK);
                eRoulette->ballFallAccel = eRoulette->ballFallSpeed = 0.0f;
                eRoulette->ballAngleSpeed = -1.0f;
            }
        }
        else
        {
            if (eRoulette->ballDistToCenter >= eRoulette->varA0 - 2.0f)
            {
                eRoulette->ballState = 255;
                eRoulette->ballRolling = 0;
                StartSpriteAnim(sprite, sprite->animCmdIndex + 3);
                UpdateSlotBelowBall(sprite);
                sprite->data[4] = 30;
                UpdateBallRelativeWheelAngle(sprite);
                sprite->data[6] = (sprite->data[6] / 30) * 30 + 15;
                sprite->callback = SpriteCB_BallLandInSlot;
                m4aSongNumStartOrChange(SE_BRIDGE_WALK);
                eRoulette->ballFallAccel = eRoulette->ballFallSpeed = 0.0f;
                eRoulette->ballAngleSpeed = -1.0f;
            }
        }
    }
}

static void SpriteCB_UnstickBall_Shroomish(struct Sprite *sprite)
{
    float slotOffset, ballFallDist, ballFallSpeed;
    const struct RouletteTable *p;
    UpdateBallPos(sprite);
    switch (sprite->data[3])
    {
    case 0:
        if (sprite->data[0] != 1)
        {
            slotOffset = ((float)sprite->data[7]);
            p = gUnknown_083F8DF4;
            ballFallDist = (slotOffset * ((float)(s32)p[eRoulette->tableId].randDistanceHigh) + (float)((s32)p[eRoulette->tableId].randDistanceLow - 1));
            ballFallSpeed = (slotOffset / ((float)(s32)p[eRoulette->tableId].var0C));
        }
        else
        {
            return;
        }
        break;
    case 180:
        if (sprite->data[0] != 0)
        {
            slotOffset = ((float)sprite->data[7]);
            p = gUnknown_083F8DF4;
            ballFallDist = (slotOffset * ((float)(s32)p[eRoulette->tableId].randDistanceHigh) + (float)((s32)p[eRoulette->tableId].randDistanceLow - 1));
            ballFallSpeed = -(slotOffset / ((float)(s32)p[eRoulette->tableId].var0C));
        }
        else
        {
            return;
        }
        break;
        default: return;
    }
    eRoulette->varA0 = eRoulette->ballDistToCenter;
    eRoulette->ballFallSpeed = ballFallSpeed;
    eRoulette->ballFallAccel = -((ballFallSpeed + ballFallSpeed) / ballFallDist + (2.0f / (ballFallDist * ballFallDist)));
    eRoulette->ballAngleSpeed = 0.0f;
    sprite->animPaused    = FALSE;
    sprite->animNum       = 0;
    sprite->animBeginning = TRUE;
    sprite->animEnded     = FALSE;
    sprite->callback      = SpriteCB_UnstickBall_ShroomishBallFall;
    sprite->data[2]       = 0;
}

static void SpriteCB_UnstickBall_TaillowDrop(struct Sprite *sprite)
{
    sprite->y2 = (s32)(((float)sprite->data[2]) * 0.05f * ((float)sprite->data[2])) - 45;
    sprite->data[2]++;
    if (sprite->data[2] > 29 && sprite->y2 >= 0)
    {
        eRoulette->ballState   = 255;
        eRoulette->ballRolling = FALSE;
        StartSpriteAnim(sprite, sprite->animCmdIndex + 3);
        UpdateSlotBelowBall(sprite);
        sprite->data[4] = 30;
        UpdateBallRelativeWheelAngle(sprite);
        sprite->data[6] = (sprite->data[6] / 30) * 30 + 15;
        sprite->callback  = SpriteCB_BallLandInSlot;
        m4aSongNumStartOrChange(SE_BRIDGE_WALK);
        eRoulette->ballUnstuck = TRUE;
    }
}

static void SpriteCB_UnstickBall_TaillowPickUp(struct Sprite *sprite)
{
    if (sprite->data[2]++ < 45)
    {
        sprite->y2--;
        if (sprite->data[2] == 45)
        {
            if (gSprites[eRoulette->spriteIds[55]].animCmdIndex == 1)
                sprite->y2++;
        }
    }
    else
    {
        if (sprite->data[2] < sprite->data[7])
        {
            if (gSprites[eRoulette->spriteIds[55]].animDelayCounter == 0)
            {
                if (gSprites[eRoulette->spriteIds[55]].animCmdIndex == 1)
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

static void SpriteCB_UnstickBall_Taillow(struct Sprite *sprite)
{
    UpdateBallPos(sprite);
    switch (sprite->data[3])
    {
    case 90:
        if (sprite->data[0] != 1)
        {
            sprite->callback  = SpriteCB_UnstickBall_TaillowPickUp;
            sprite->data[2] = 0;
        }
        break;
    case 270:
        if (sprite->data[0] != 0)
        {
            sprite->callback  = SpriteCB_UnstickBall_TaillowPickUp;
            sprite->data[2] = 0;
        }
        break;
    }
}

static void SpriteCB_UnstickBall(struct Sprite *sprite)
{
    UpdateBallPos(sprite);
    switch (eRoulette->useTaillow)
    {
    default:
    case 0:
        CreateShroomishSprite(sprite);
        sprite->callback = SpriteCB_UnstickBall_Shroomish;
        break;
    case 1:
        CreateTaillowSprite(sprite);
        sprite->callback = SpriteCB_UnstickBall_Taillow;
        break;
    }
}

static void SpriteCB_RollBall_TryLandAdjacent(struct Sprite *sprite)
{
    UpdateBallPos(sprite);
    if (sprite->data[2]-- == 16)
        eRoulette->ballFallSpeed *= -1.0f;
    if (sprite->data[2] == 0)
    {
        if (!sprite->data[0])
        {
            eRoulette->ballState   = 255;
            eRoulette->ballRolling = 0;
            StartSpriteAnim(sprite, sprite->animCmdIndex + 3);
            UpdateSlotBelowBall(sprite);
            sprite->data[4] = 30;
            UpdateBallRelativeWheelAngle(sprite);
            sprite->data[6] = (sprite->data[6] / 30) * 30 + 15;
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

void debug_sub_812E698(struct Sprite *sprite)
{
    UpdateBallPos(sprite);
    sprite->data[2] = 0;
    UpdateSlotBelowBall(sprite);
    m4aSongNumStart(SE_BALL_BOUNCE_1);
    if (Random() & 1)
    {
        eRoulette->ballAngleSpeed = 0;
        eRoulette->stuckHitSlot = (eRoulette->hitSlot + 1) % 12;
    }
    else
    {
        eRoulette->ballAngleSpeed = ({float f = gUnknown_083F8DF4[eRoulette->tableId].var1C; f * 2;});
        eRoulette->stuckHitSlot = eRoulette->hitSlot;
    }
    sprite->data[0] = 1;
    sprite->data[2] = gUnknown_083F8DF4[eRoulette->tableId].randDistanceLow;
    eRoulette->ballFallSpeed = 0.085f;
    sprite->callback = SpriteCB_RollBall_TryLandAdjacent;
    sprite->data[1] = 5;
}

#endif

static void SpriteCB_RollBall_TryLand(struct Sprite *sprite)
{
    UpdateBallPos(sprite);
    sprite->data[2] = 0;
    UpdateSlotBelowBall(sprite);
    if (!(gUnknown_083F8D90[eRoulette->hitSlot].flag & eRoulette->hitFlags))
    {
        eRoulette->ballState   = 255;
        eRoulette->ballRolling = 0;
        StartSpriteAnim(sprite, sprite->animCmdIndex + 3);
        UpdateSlotBelowBall(sprite);
        sprite->data[4] = 30;
        UpdateBallRelativeWheelAngle(sprite);
        sprite->data[6] = (sprite->data[6] / 30) * 30 + 15;
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
            u32 o;
            eRoulette->ballAngleSpeed = 0.0f;
            o = (eRoulette->hitSlot + 1) % 12;
            slotId = o;
            eRoulette->stuckHitSlot = o;
        }
        else
        {
            eRoulette->ballAngleSpeed = ({
                float temp = gUnknown_083F8DF4[eRoulette->tableId].var1C;
                temp * 2;
            }); // couldn't replicate loads
            slotId = (eRoulette->hitSlot + 11) % 12;
            eRoulette->stuckHitSlot = eRoulette->hitSlot;
        }
        if (gUnknown_083F8D90[slotId].flag & eRoulette->hitFlags)
        {
            sprite->data[0] = 1;
            sprite->data[2] = gUnknown_083F8DF4[eRoulette->tableId].randDistanceLow;
        }
        else
        {
            sprite->data[0] = gUnknown_083F8D90[slotId].flag & eRoulette->hitFlags;
            if (eRoulette->tableId)
            {
                sprite->data[2] = gUnknown_083F8DF4[eRoulette->tableId].randDistanceHigh;
            }
            else
            {
                sprite->data[2] = gUnknown_083F8DF4[eRoulette->tableId].randDistanceLow;
                if (fallRight)
                {
                    eRoulette->ballAngleSpeed = 0.5f;
                }
                else
                {
                    eRoulette->ballAngleSpeed = -1.5f;
                }
            }
        }
        eRoulette->ballFallSpeed = 0.085f;
        sprite->callback = SpriteCB_RollBall_TryLandAdjacent;
        sprite->data[1] = 5;
    }
}

void sub_8118F8C(struct Sprite *sprite)
{
    UpdateBallPos(sprite);
    if (!(eRoulette->ballAngleSpeed > 0.5f))
    {
        UpdateSlotBelowBall(sprite);
        if (!GetBallDistanceToSlotMidpoint(sprite))
        {
            const struct RouletteTable *p;
            eRoulette->ballAngleAccel = 0.0f;
            p = gUnknown_083F8DF4;
            eRoulette->ballAngleSpeed -= ((float)(p[eRoulette->tableId].wheelSpeed))
                / ((float)(s16)(p[eRoulette->tableId].wheelDelay + 1));
            sprite->data[1] = 4;
#if DEBUG
            if (unk_2039560 != 0)
                sprite->callback = debug_sub_812E698;
            else
#endif
                sprite->callback = SpriteCB_RollBall_TryLand;
        }
        else
        {
            if (eRoulette->ballAngleAccel != 0.0f)
            {
                if (eRoulette->ballAngleSpeed < 0.0f)
                {
                    eRoulette->ballAngleAccel = 0.0f;
                    eRoulette->ballAngleSpeed = 0.0f;
                    eRoulette->ballFallSpeed /= 1.2;
                }
            }
        }
    }
}

static void SpriteCB_RollBall_Medium(struct Sprite *sprite)
{
    UpdateBallPos(sprite);
    if (!(eRoulette->ballDistToCenter > 40.f))
    {
        eRoulette->ballFallSpeed = - (        4.0f / (float)((u16)eRoulette->ballTravelDistSlow));
        eRoulette->ballAngleAccel = - (eRoulette->ballAngleSpeed / (float)((u16)eRoulette->ballTravelDistSlow));
        sprite->animNum       = 2;
        sprite->animBeginning = TRUE;
        sprite->animEnded     = FALSE;
        sprite->data[1]     = 3;
        sprite->callback      = sub_8118F8C;
    }
}

static void SpriteCB_RollBall_Fast(struct Sprite *sprite)
{
    UpdateBallPos(sprite);
    if (!(eRoulette->ballDistToCenter > 60.0f))
    {
        m4aSongNumStartOrChange(SE_ROULETTE_BALL2);
        eRoulette->ballFallSpeed = - (                20.0f / (float)((u16)eRoulette->ballTravelDistMed));
        eRoulette->ballAngleAccel =   ((1.0f - eRoulette->ballAngleSpeed) / (float)((u16)eRoulette->ballTravelDistMed));
        sprite->animNum       = 1;
        sprite->animBeginning = TRUE;
        sprite->animEnded     = FALSE;
        sprite->data[1]     = 2;
        sprite->callback      = SpriteCB_RollBall_Medium;
    }
}

void SpriteCB_RollBall_Start(struct Sprite *sprite)
{
    sprite->data[1] = 1;
    sprite->data[2] = 0;
    UpdateBallPos(sprite);
    sprite->invisible = FALSE;
    sprite->callback  = SpriteCB_RollBall_Fast;
}

static void CreateShroomishSprite(struct Sprite *ball)
{
    u16 t;
    u8 i;
    s16 coords[2][2] = {
        {116, 44},
        {116, 112}
    };
    struct Roulette *roulette;
    t = ball->data[7] - 2;
    roulette = eRoulette;
    roulette->spriteIds[55] = CreateSprite(&sSpriteTemplate_Shroomish, 36, -12, 50);
    roulette->spriteIds[56] = CreateSprite(sSpriteTemplate_ShroomishShadow, coords[ball->data[0]][0], coords[ball->data[0]][1], 59);
    roulette->spriteIds[57] = CreateSprite(&sSpriteTemplate_ShroomishShadow[1], 36, 140, 51);
    gSprites[eRoulette->spriteIds[57]].oam.objMode = 1;
    for (i = 0; i < 3; i++)
    {
        gSprites[eRoulette->spriteIds[55 + i]].coordOffsetEnabled = FALSE;
        gSprites[eRoulette->spriteIds[55 + i]].invisible          = TRUE;
        gSprites[eRoulette->spriteIds[55 + i]].animPaused         = TRUE;
        gSprites[eRoulette->spriteIds[55 + i]].affineAnimPaused   = TRUE;
        gSprites[eRoulette->spriteIds[55 + i]].data[4]          = eRoulette->spriteIds[55];
        gSprites[eRoulette->spriteIds[55 + i]].data[5]          = eRoulette->spriteIds[56];
        gSprites[eRoulette->spriteIds[55 + i]].data[6]          = eRoulette->spriteIds[57];
        gSprites[eRoulette->spriteIds[55 + i]].data[2]          = t;
        gSprites[eRoulette->spriteIds[55 + i]].data[3]          = (ball->data[7] * gUnknown_083F8DF4[eRoulette->tableId].randDistanceHigh) +
                                                                (gUnknown_083F8DF4[eRoulette->tableId].randDistanceLow + 0xffff);
    }
    gSprites[eRoulette->spriteIds[56]].coordOffsetEnabled = TRUE;
    eRoulette->ball = ball;
}

static void CreateTaillowSprite(struct Sprite *ball)
{
    u8 i = 0;
    s16 t;
    s16 coords[2][2] = {
        {256, 84},
        {-16, 84}
    };
    const struct RouletteTable *p;
    t = ball->data[7] - 2;
    eRoulette->spriteIds[55] = CreateSprite(&sSpriteTemplate_Taillow, coords[ball->data[0]][0], coords[ball->data[0]][1], 50);
    StartSpriteAnim(&gSprites[eRoulette->spriteIds[55]], ball->data[0]);
    eRoulette->spriteIds[56] = CreateSprite(&sSpriteTemplate_TaillowShadow, coords[ball->data[0]][0], coords[ball->data[0]][1], 51);
    gSprites[eRoulette->spriteIds[56]].affineAnimPaused = TRUE;
    gSprites[eRoulette->spriteIds[56]].animPaused       = TRUE;
    ball->data[7] = (t * (p = gUnknown_083F8DF4)[eRoulette->tableId].randDistanceHigh) + (p[eRoulette->tableId].var10 + 45);
    for (; i < 2; i++)
    {
        gSprites[eRoulette->spriteIds[55 + i]].data[4] = eRoulette->spriteIds[55];
        gSprites[eRoulette->spriteIds[55 + i]].data[5] = eRoulette->spriteIds[56];
        gSprites[eRoulette->spriteIds[55 + i]].data[6] = eRoulette->spriteIds[56];
        gSprites[eRoulette->spriteIds[55 + i]].data[2] = t;
        gSprites[eRoulette->spriteIds[55 + i]].data[3] = ball->data[7] - 45;
    }
    eRoulette->ball = ball;
}

static void SetBallStuck(struct Sprite *sprite)
{
    u8 slotId;
    u16 angle;
    u8 numCandidates = 0;
    u8 maxSlotToCheck = 5;
    u8 betSlotId = 0;
    u8 i = 0;
    u8 slotsToSkip;
    u8 slotCandidates[10] = {};
    u16 rand = Random();

    eRoulette->ballState = 1;
    eRoulette->ballStuck = TRUE;
    eRoulette->ballUnstuck = FALSE;
    eRoulette->hitSlot = 0xFF;
    eRoulette->ballAngle = sprite->data[3];
    eRoulette->ballFallSpeed = 0.0f;
    eRoulette->ballAngleSpeed = gUnknown_083F8DF4[eRoulette->tableId].var1C;

    angle = (eRoulette->tableId * 30 + 33) + (0x1 - eRoulette->useTaillow) * 15;
    for (i = 0; i < 4; i++)
    {
        if (angle < sprite->data[3] && sprite->data[3] <= angle + 90)
        {
            sprite->data[0] = i / 2;
            eRoulette->useTaillow = i % 2;
            break;
        }
        if (i == 3)
        {
            sprite->data[0] = 1;
            eRoulette->useTaillow = 1;
            break;
        }
        angle += 90;
    }

    if (eRoulette->useTaillow)
    {
        if (sprite->data[0])
            PlayCry_Normal(SPECIES_TAILLOW, -63);
        else
            PlayCry_Normal(SPECIES_TAILLOW, 63);
    }
    else
    {
        PlayCry_Normal(SPECIES_SHROOMISH, -63);
    }

    slotsToSkip = 2;
    slotId = (eRoulette->stuckHitSlot + 2) % 12;

    if (eRoulette->useTaillow == 1 && eRoulette->tableId == 1)
        maxSlotToCheck += 6;
    else
        maxSlotToCheck += slotsToSkip;

    for (i = slotsToSkip; i < maxSlotToCheck; i++)
    {
        if (!(eRoulette->hitFlags & gUnknown_083F8D90[slotId].flag))
        {
            slotCandidates[numCandidates++] = i;
            if (betSlotId == 0 && (gUnknown_083F8D90[slotId].flag & gUnknown_083F8C00[eRoulette->betSelection[eRoulette->curBallNum]].inSelectionFlags))
                betSlotId = i;
        }
        slotId = (slotId + 1) % 0xC;
    }

    if ((eRoulette->useTaillow + 1) & eRoulette->partySpeciesFlags)
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

static void SpriteCB_ShroomishExit(struct Sprite *sprite)
{
    if (sprite->data[1]++ >= sprite->data[3])
    {
        sprite->x -= 2;
        if (sprite->x < -16)
        {
            if (!eRoulette->ballUnstuck)
                eRoulette->ballUnstuck = TRUE;
            DestroySprite(sprite);
            eRoulette->shroomishShadowTimer = 0;
            eRoulette->shroomishShadowAlpha = sShroomishShadowAlphas[0];
        }
    }
}

static void SpriteCB_ShroomishShakeScreen(struct Sprite *sprite)
{
    s16 screenShakeOffsets[3][4] = {
        {-1, 0, 1, 0},
        {-2, 0, 2, 0},
        {-3, 0, 3, 0}
    };
    s32 p, screenShakeIdx;

    if (sprite->data[1]++ < sprite->data[3])
    {
        if (sprite->data[1] & 1)
        {
            gSpriteCoordOffsetY = screenShakeOffsets[sprite->data[2] / 2][sprite->data[7]];
            p = screenShakeIdx = sprite->data[7] + 1;
            if (screenShakeIdx < 0)
                p += 3;
            sprite->data[7] = screenShakeIdx - ((p >> 2) * 4);
        }
        sprite->invisible ^= 1;
    }
    else
    {
        gSpriteCoordOffsetY = 0;
        gSprites[eRoulette->spriteIds[55]].animPaused = FALSE;
        DestroySprite(sprite);
    }
}

static void SpriteCB_ShroomishFall(struct Sprite *sprite)
{
    float timer;
    sprite->data[1]++;
    timer = sprite->data[1];
    sprite->y2 = timer * 0.039f * timer;
    eRoulette->shroomishShadowAlpha = sShroomishShadowAlphas[(eRoulette->shroomishShadowTimer - 1) / 2];
    if (eRoulette->shroomishShadowTimer < 19)
        eRoulette->shroomishShadowTimer++;
    if (sprite->data[1] > 60)
    {
        sprite->data[1] = 0;
        sprite->callback = SpriteCB_ShroomishExit;
        gSprites[sprite->data[6]].callback  = SpriteCB_ShroomishExit;
        gSprites[sprite->data[6]].data[1] = -2;
        gSprites[sprite->data[5]].invisible = FALSE;
        gSprites[sprite->data[5]].callback  = SpriteCB_ShroomishShakeScreen;
        m4aSongNumStart(SE_M_STRENGTH);
    }
}

static void SpriteCB_Shroomish(struct Sprite *sprite)
{
    if (sprite->data[7] == 0)
    {
        register u32 t asm("r2");
        u32 z ;
        if (eRoulette->ball->data[0] == 0)
        {
            t = eRoulette->ball->data[3];
            z = gUnknown_083F8DF4[eRoulette->tableId].var08;
        }
        else
        {
            t = eRoulette->ball->data[3];
            z = gUnknown_083F8DF4[eRoulette->tableId].var08;
            z += 180;
        }
        if (t == z)
        {
            sprite->invisible = FALSE;
            sprite->data[7]++;
            m4aSongNumStart(SE_FALL);
            eRoulette->shroomishShadowTimer = 1;
            eRoulette->shroomishShadowAlpha = sShroomishShadowAlphas[0];
        }
    }
    else
    {
        u32 t, z;
        eRoulette->shroomishShadowAlpha = sShroomishShadowAlphas[(eRoulette->shroomishShadowTimer - 1) / 2];
        if (eRoulette->shroomishShadowTimer < 19)
            eRoulette->shroomishShadowTimer++;
        if (eRoulette->ball->data[0] == 0)
        {
            t = eRoulette->ball->data[3];
            z = gUnknown_083F8DF4[eRoulette->tableId].var0A;
        }
        else
        {
            t = eRoulette->ball->data[3];
            z = gUnknown_083F8DF4[eRoulette->tableId].var0A ;
            z += 180;
        }
        if (t == z)
        {
            gSprites[sprite->data[4]].callback  = SpriteCB_ShroomishFall;
            gSprites[sprite->data[4]].invisible = FALSE;
            sprite->callback  = &SpriteCallbackDummy;
            sprite->data[7] = 0;
        }
    }
}

static void SpriteCB_TaillowShadow_Flash(struct Sprite *sprite)
{
    sprite->invisible ^= 1;
}

static void SpriteCB_Taillow_FlyAway(struct Sprite *sprite)
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
        FreeOamMatrix(gSprites[eRoulette->spriteIds[56]].oam.matrixNum);
        DestroySprite(&gSprites[eRoulette->spriteIds[56]]);
    }
}

static void SpriteCB_Taillow_PickUpBall(struct Sprite *sprite)
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
            StartSpriteAnim(sprite, eRoulette->ball->data[0] + 4);
            sprite->callback = SpriteCB_Taillow_FlyAway;
            gSprites[sprite->data[6]].affineAnimPaused = FALSE;
        }
    }
}

static void SpriteCB_Taillow_FlyIn(struct Sprite *sprite)
{
    s8 xMoveOffsets[2] = {-1, 1}; //sign
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
        sprite->x += xMoveOffsets[eRoulette->ball->data[0]] * 2;
        if (IsSEPlaying())
        {
            s8 pan =  -((116 - sprite->x) / 2);
            m4aMPlayPanpotControl(&gMPlayInfo_SE1, 0xffff, pan);
            m4aMPlayPanpotControl(&gMPlayInfo_SE2, 0xffff, pan);
        }
    }
    else
    {
        if (sprite->data[1] >= 0)
        {
            sprite->x += xMoveOffsets[eRoulette->ball->data[0]] * yMoveOffsets[7 - sprite->data[1]][0];
            sprite->y += yMoveOffsets[7 - sprite->data[1]][1];
        }
        else
        {
            m4aSongNumStartOrChange(SE_TAILLOW_WING_FLAP);
            if (eRoulette->ball->data[0] == 0)
                PlayCry_Normal(SPECIES_TAILLOW, 63);
            else
                PlayCry_Normal(SPECIES_TAILLOW, -63);
            StartSpriteAnim(sprite, eRoulette->ball->data[0] + 2);
            sprite->data[1] = 45;
            sprite->callback = SpriteCB_Taillow_PickUpBall;
        }
    }
}

static void SpriteCB_TaillowShadow_FlyIn(struct Sprite *sprite)
{
    s8 moveDir[2] = {-1, 1}; //sign
    if (sprite->data[1]-- >= 0)
    {
        sprite->x += moveDir[eRoulette->ball->data[0]] * 2;
        gSprites[sprite->data[6]].invisible ^= 1;
    }
    else
    {
        sprite->callback = SpriteCB_TaillowShadow_Flash;
    }
}

static void SpriteCB_Taillow(struct Sprite *sprite)
{
    if (eRoulette->ball->data[0] == 0)
    {
        if (eRoulette->ball->data[3] == gUnknown_083F8DF4[eRoulette->tableId].var12 + 90)
        {
            gSprites[sprite->data[6]].data[1] = 52;
            gSprites[sprite->data[4]].data[1] = 52;
        }
        else
        {
            return;
        }
    }
    else
    {
        if (eRoulette->ball->data[3] == gUnknown_083F8DF4[eRoulette->tableId].var14 + 270)
        {
            gSprites[sprite->data[6]].data[1] = 46;
            gSprites[sprite->data[4]].data[1] = 46;
        }
        else
        {
            return;
        }
    }
    gSprites[sprite->data[6]].callback = SpriteCB_TaillowShadow_FlyIn;
    gSprites[sprite->data[4]].callback = SpriteCB_Taillow_FlyIn;
    m4aSongNumStart(SE_FALL);
}
