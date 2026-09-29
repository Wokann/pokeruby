#include "global.h"
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
#include "roulette_util.h"
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

static const u8 sBall_Gfx[] = INCBIN_U8("graphics/roulette/ball.4bpp.lz");
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
        LZ77UnCompWram(sShroomishTaillow_Gfx, ewram17000);
        LZ77UnCompWram(sBall_Gfx, ewram17E00);
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
            spriteId = eRoulette->spriteIds[(i * NUM_BOARD_POKES) + SPR_GRID_ICONS + j] =
                CreateSprite(&sSpriteTemplates_GridIcons[j], j * 24 + 148, y + 92, 30);
            gSprites[spriteId].animPaused = TRUE;
            y += 24;
            if (y > 71)
                y = 0;
        }
    }
    for (i = 0; i < NUM_BOARD_POKES; i++)
    {
        spriteId = eRoulette->spriteIds[SPR_POKE_HEADERS + i] =
            CreateSprite(&sSpriteTemplates_PokeHeaders[i], (s16)(i * 24 + 148), 70, 30);
        gSprites[spriteId].animPaused = TRUE;
    }
    for (i = 0; i < NUM_BOARD_COLORS; i++)
    {
        spriteId = eRoulette->spriteIds[SPR_COLOR_HEADERS + i] =
            CreateSprite(&sSpriteTemplates_ColorHeaders[i], 126, (s16)(i * 24 + 92), 30);
        gSprites[spriteId].animPaused = TRUE;
    }
}

void DestroyGridSprites(void)
{
    u8 i;
    for (i = 0; i < NUM_ROULETTE_SLOTS; i++)
        DestroySprite(&gSprites[eRoulette->spriteIds[SPR_GRID_ICONS + i]]);
}

void ShowHideGridIcons(bool8 hideAll, u8 hideSquare)
{
    u8 i;
    switch (hideAll)
    {
    case TRUE:
        for (i = 0; i < NUM_GRID_SELECTIONS; i++)
            gSprites[eRoulette->spriteIds[SPR_GRID_ICONS + i]].invisible = TRUE;
        break;
    case FALSE:
        for (i = 0; i < NUM_ROULETTE_SLOTS; i++)
        {
            if (!(eRoulette->hitFlags & sRouletteSlots[i].flag))
                gSprites[eRoulette->spriteIds[SPR_GRID_ICONS + i]].invisible = FALSE;
            else if (sRouletteSlots[i].gridSquare != hideSquare)
                gSprites[eRoulette->spriteIds[SPR_GRID_ICONS + i]].invisible = TRUE;
            else
                gSprites[eRoulette->spriteIds[SPR_GRID_ICONS + i]].invisible = FALSE;
        }
        for (; i < NUM_GRID_SELECTIONS; i++)
            gSprites[eRoulette->spriteIds[SPR_GRID_ICONS + i]].invisible = FALSE;
        break;
    }
}

void CreateGridBallSprites(void)
{
    u8 i;
    for (i = 0; i < BALLS_PER_ROUND; i++)
    {
        eRoulette->spriteIds[SPR_GRID_BALLS + i] = CreateSprite(&sSpriteTemplate_Ball, 116, 20, 10);
        gSprites[eRoulette->spriteIds[SPR_GRID_BALLS + i]].invisible    = TRUE;
        gSprites[eRoulette->spriteIds[SPR_GRID_BALLS + i]].data[0]      = 1;
        gSprites[eRoulette->spriteIds[SPR_GRID_BALLS + i]].callback     = SpriteCB_GridSquare;
        gSprites[eRoulette->spriteIds[SPR_GRID_BALLS + i]].oam.priority = 1;
        StartSpriteAnim(&gSprites[eRoulette->spriteIds[SPR_GRID_BALLS + i]], 8);
    }
}

void ShowHideGridBalls(bool8 hideAll, u8 hideBallId)
{
    u8 i = 0;
    if (hideAll)
    {
        for (; i < BALLS_PER_ROUND; i++)
            gSprites[eRoulette->spriteIds[SPR_GRID_BALLS + i]].invisible = TRUE;
    }
    else
    {
        for (; i < BALLS_PER_ROUND; i++)
        {
            if (!eRoulette->hitSquares[i] || i == hideBallId)
            {
                gSprites[eRoulette->spriteIds[SPR_GRID_BALLS + i]].invisible = TRUE;
            }
            else
            {
                gSprites[eRoulette->spriteIds[SPR_GRID_BALLS + i]].invisible = FALSE;
                gSprites[eRoulette->spriteIds[SPR_GRID_BALLS + i]].x = (sGridSelections[eRoulette->hitSquares[i]].x + 1) * 8 + 4;
                gSprites[eRoulette->spriteIds[SPR_GRID_BALLS + i]].y = (sGridSelections[eRoulette->hitSquares[i]].y + 1) * 8 + 3;
            }
        }
    }
}

void ShowHideWinSlotCursor(u8 selectionId)
{
    if (selectionId == SELECTION_NONE)
    {
        gSprites[eRoulette->spriteIds[SPR_WIN_SLOT_CURSOR]].invisible = TRUE;
    }
    else
    {
        gSprites[eRoulette->spriteIds[SPR_WIN_SLOT_CURSOR]].invisible = FALSE;
        gSprites[eRoulette->spriteIds[SPR_WIN_SLOT_CURSOR]].x = (sGridSelections[selectionId].x + 2) * 8;
        gSprites[eRoulette->spriteIds[SPR_WIN_SLOT_CURSOR]].y = (sGridSelections[selectionId].y + 2) * 8;
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
            spriteId = eRoulette->spriteIds[(i * NUM_BOARD_POKES) + SPR_WHEEL_ICONS + j] =
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
    eRoulette->spriteIds[SPR_CREDIT] = CreateSprite(&sSpriteTemplate_Credit, 208, 16, 4);
    gSprites[eRoulette->spriteIds[SPR_CREDIT]].animPaused = TRUE;
    for (i = 0; i < MAX_COIN_DIGITS; i++)
    {
        eRoulette->spriteIds[SPR_CREDIT_DIGITS + i] = CreateSprite(&sSpriteTemplate_CreditDigit, (s16)(i * 8 + 196), 24, 0);
        gSprites[eRoulette->spriteIds[SPR_CREDIT_DIGITS + i]].invisible = TRUE;
        gSprites[eRoulette->spriteIds[SPR_CREDIT_DIGITS + i]].animPaused = TRUE;
    }
    eRoulette->spriteIds[SPR_MULTIPLIER] = CreateSprite(&sSpriteTemplate_Multiplier, 120, 68, 4);
    gSprites[eRoulette->spriteIds[SPR_MULTIPLIER]].animPaused = TRUE;
    for (i = 0; i < BALLS_PER_ROUND / 2; i++)
    {
        eRoulette->spriteIds[SPR_BALL_COUNTER + i] = CreateSprite(&sSpriteTemplate_BallCounter, (s16)(i * 16 + 192), 36, 4);
        gSprites[eRoulette->spriteIds[SPR_BALL_COUNTER + i]].invisible = TRUE;
        gSprites[eRoulette->spriteIds[SPR_BALL_COUNTER + i]].animPaused = TRUE;
    }
    eRoulette->spriteIds[SPR_WIN_SLOT_CURSOR] = CreateSprite(&sSpriteTemplate_Cursor, 152, 96, 9);
    gSprites[eRoulette->spriteIds[SPR_WIN_SLOT_CURSOR]].oam.priority = 1;
    gSprites[eRoulette->spriteIds[SPR_WIN_SLOT_CURSOR]].animPaused = TRUE;
    gSprites[eRoulette->spriteIds[SPR_WIN_SLOT_CURSOR]].invisible = TRUE;
}

void SetCreditDigits(u16 num)
{
    u8 i;
    u16 d = 1000;
    bool8 printZero = FALSE;
    for (i = 0; i < MAX_COIN_DIGITS; i++)
    {
        u8 digit = num / d;
        gSprites[eRoulette->spriteIds[SPR_CREDIT_DIGITS + i]].invisible = TRUE;
        if (digit > 0 || printZero || i == MAX_COIN_DIGITS - 1)
        {
            gSprites[eRoulette->spriteIds[SPR_CREDIT_DIGITS + i]].invisible = FALSE;
            gSprites[eRoulette->spriteIds[SPR_CREDIT_DIGITS + i]].oam.tileNum =
                gSprites[eRoulette->spriteIds[SPR_CREDIT_DIGITS + i]].sheetTileStart
                + (*gSprites[eRoulette->spriteIds[SPR_CREDIT_DIGITS + i]].anims + digit)->type;
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
        if (eRoulette->colorHits[selectionId] < NUM_BOARD_POKES)
            return animIds[((vu8 *)eRoulette->colorHits)[selectionId] + 1]; // couldn't recreate redundant loads
        break;
    case NUM_BOARD_POKES:
        selectionId = GET_COL_IDX(selectionId);
        if (eRoulette->pokeHits[selectionId] < NUM_BOARD_COLORS)
            return animIds[((vu8 *)eRoulette->pokeHits)[selectionId] + 2];
        break;
    case NUM_ROULETTE_SLOTS:
        if (!(eRoulette->hitFlags & sGridSelections[selectionId].flag))
            return animIds[4];
        break;
    }
    return 0;
}

void SetMultiplierSprite(u8 selectionId)
{
    struct Sprite *sprite = &gSprites[eRoulette->spriteIds[SPR_MULTIPLIER]];
    sprite->animCmdIndex = GetMultiplierAnimId(selectionId);
    sprite->oam.tileNum =
        sprite->sheetTileStart
        + (*sprite->anims + sprite->animCmdIndex)->type;
}

void SetBallCounterNumLeft(u8 numBalls)
{
    u8 i;
    u8 animOffset = 0;
    if (eRoulette->minBet == 1)
        animOffset = 2;
    switch (numBalls)
    {
    case BALLS_PER_ROUND:
        for (i = 0; i < BALLS_PER_ROUND / 2; i++)
        {
            gSprites[eRoulette->spriteIds[SPR_BALL_COUNTER + i]].invisible = FALSE;
            gSprites[eRoulette->spriteIds[SPR_BALL_COUNTER + i]].oam.tileNum =
                gSprites[eRoulette->spriteIds[SPR_BALL_COUNTER + i]].sheetTileStart
                + (*gSprites[eRoulette->spriteIds[SPR_BALL_COUNTER + i]].anims)->type;
        }
        break;
    case 5:
        gSprites[eRoulette->spriteIds[SPR_BALL_COUNTER_3]].oam.tileNum =
            gSprites[eRoulette->spriteIds[SPR_BALL_COUNTER_3]].sheetTileStart
            + (*gSprites[eRoulette->spriteIds[SPR_BALL_COUNTER_3]].anims + animOffset + 1)->type;
        break;
    case 4:
        gSprites[eRoulette->spriteIds[SPR_BALL_COUNTER_3]].oam.tileNum =
            gSprites[eRoulette->spriteIds[SPR_BALL_COUNTER_3]].sheetTileStart
            + (*gSprites[eRoulette->spriteIds[SPR_BALL_COUNTER_3]].anims + animOffset + 2)->type;
        break;
    case 3:
        gSprites[eRoulette->spriteIds[SPR_BALL_COUNTER_2]].oam.tileNum =
            gSprites[eRoulette->spriteIds[SPR_BALL_COUNTER_2]].sheetTileStart
            + (*gSprites[eRoulette->spriteIds[SPR_BALL_COUNTER_2]].anims + animOffset + 1)->type;
        break;
    case 2:
        gSprites[eRoulette->spriteIds[SPR_BALL_COUNTER_2]].oam.tileNum =
            gSprites[eRoulette->spriteIds[SPR_BALL_COUNTER_2]].sheetTileStart
            + (*gSprites[eRoulette->spriteIds[SPR_BALL_COUNTER_2]].anims + animOffset + 2)->type;
        break;
    case 1:
        gSprites[eRoulette->spriteIds[SPR_BALL_COUNTER_1]].oam.tileNum =
            gSprites[eRoulette->spriteIds[SPR_BALL_COUNTER_1]].sheetTileStart
            + (*gSprites[eRoulette->spriteIds[SPR_BALL_COUNTER_1]].anims + animOffset + 1)->type;
        break;
    case 0:
    default:
        for (i = 0; i < BALLS_PER_ROUND / 2; i++)
        {
            gSprites[eRoulette->spriteIds[SPR_BALL_COUNTER + i]].oam.tileNum =
                gSprites[eRoulette->spriteIds[SPR_BALL_COUNTER + i]].sheetTileStart
                + (*gSprites[eRoulette->spriteIds[SPR_BALL_COUNTER + i]].anims + animOffset + 2)->type;
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

void SpriteCB_WheelCenter(struct Sprite *sprite)
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
    for (i = 0; i < BALLS_PER_ROUND; i++)
    {
        u8 spriteId =
            eRoulette->spriteIds[i] = CreateSprite(&sSpriteTemplate_Ball, 116, 80, 57 - i);
        if (spriteId != MAX_SPRITES)
        {
            gSprites[eRoulette->spriteIds[i]].invisible          = TRUE;
            gSprites[eRoulette->spriteIds[i]].coordOffsetEnabled = TRUE;
        }
    }
}

void HideWheelBalls(void)
{
    u8 spriteId = eRoulette->spriteIds[SPR_WHEEL_BALLS];
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
    if (eRoulette->wheelAngle > sprite->sBallAngle)
    {
        sprite->sBallWheelAngle = 360 - eRoulette->wheelAngle + sprite->sBallAngle;
        if (sprite->sBallWheelAngle >= 360)
            sprite->sBallWheelAngle -= 360;
    }
    else
        sprite->sBallWheelAngle = sprite->sBallAngle - eRoulette->wheelAngle;
    return sprite->sBallWheelAngle;
}

u8 UpdateSlotBelowBall(struct Sprite *sprite)
{
    eRoulette->hitSlot = (u8)(((float)(s16)UpdateBallRelativeWheelAngle(sprite)) / DEGREES_PER_SLOT);
    return eRoulette->hitSlot;
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
    eRoulette->ballAngleSpeed += eRoulette->ballAngleAccel;
    eRoulette->ballAngle += eRoulette->ballAngleSpeed;
    if (eRoulette->ballAngle >= 360)
        eRoulette->ballAngle -= 360.0f;
    else if (eRoulette->ballAngle < 0.0f)
        eRoulette->ballAngle += 360.0f;
    sprite->sBallAngle = eRoulette->ballAngle;
    eRoulette->ballFallSpeed += eRoulette->ballFallAccel;
    eRoulette->ballDistToCenter += eRoulette->ballFallSpeed;
    sprite->sBallDistToCenter = eRoulette->ballDistToCenter;
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
    sprite->sBallAngle = eRoulette->wheelAngle + sprite->sBallWheelAngle;
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
            if (eRoulette->ballDistToCenter <= eRoulette->varA0 - 2.0f)
            {
                eRoulette->ballState = BALL_STATE_LANDED;
                eRoulette->ballRolling = FALSE;
                StartSpriteAnim(sprite, sprite->animCmdIndex + 3);
                UpdateSlotBelowBall(sprite);
                sprite->sBallDistToCenter = DEGREES_PER_SLOT;
                UpdateBallRelativeWheelAngle(sprite);
                sprite->sBallWheelAngle = (sprite->sBallWheelAngle / DEGREES_PER_SLOT) * DEGREES_PER_SLOT + 15;
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
                eRoulette->ballState = BALL_STATE_LANDED;
                eRoulette->ballRolling = FALSE;
                StartSpriteAnim(sprite, sprite->animCmdIndex + 3);
                UpdateSlotBelowBall(sprite);
                sprite->sBallDistToCenter = DEGREES_PER_SLOT;
                UpdateBallRelativeWheelAngle(sprite);
                sprite->sBallWheelAngle = (sprite->sBallWheelAngle / DEGREES_PER_SLOT) * DEGREES_PER_SLOT + 15;
                sprite->callback = SpriteCB_BallLandInSlot;
                m4aSongNumStartOrChange(SE_BRIDGE_WALK);
                eRoulette->ballFallAccel = eRoulette->ballFallSpeed = 0.0f;
                eRoulette->ballAngleSpeed = -1.0f;
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
            ballFallDist = (slotOffset * ((float)(s32)tables[eRoulette->tableId].randDistanceHigh) + (float)((s32)tables[eRoulette->tableId].randDistanceLow - 1));
            ballFallSpeed = (slotOffset / ((float)(s32)tables[eRoulette->tableId].shroomish.fallSlowdown));
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
            ballFallDist = (slotOffset * ((float)(s32)tables[eRoulette->tableId].randDistanceHigh) + (float)((s32)tables[eRoulette->tableId].randDistanceLow - 1));
            ballFallSpeed = -(slotOffset / ((float)(s32)tables[eRoulette->tableId].shroomish.fallSlowdown));
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

void SpriteCB_UnstickBall_TaillowDrop(struct Sprite *sprite)
{
    sprite->y2 = (s32)(((float)sprite->data[2]) * 0.05f * ((float)sprite->data[2])) - 45;
    sprite->data[2]++;
    if (sprite->data[2] >= DEGREES_PER_SLOT && sprite->y2 >= 0)
    {
        eRoulette->ballState   = BALL_STATE_LANDED;
        eRoulette->ballRolling = FALSE;
        StartSpriteAnim(sprite, sprite->animCmdIndex + 3);
        UpdateSlotBelowBall(sprite);
        sprite->sBallDistToCenter = DEGREES_PER_SLOT;
        UpdateBallRelativeWheelAngle(sprite);
        sprite->sBallWheelAngle = (sprite->sBallWheelAngle / DEGREES_PER_SLOT) * DEGREES_PER_SLOT + 15;
        sprite->callback  = SpriteCB_BallLandInSlot;
        m4aSongNumStartOrChange(SE_BRIDGE_WALK);
        eRoulette->ballUnstuck = TRUE;
    }
}

void SpriteCB_UnstickBall_TaillowPickUp(struct Sprite *sprite)
{
    if (sprite->data[2]++ < 45)
    {
        sprite->y2--;
        if (sprite->data[2] == 45)
        {
            if (gSprites[eRoulette->spriteIds[SPR_CLEAR_MON]].animCmdIndex == 1)
                sprite->y2++;
        }
    }
    else
    {
        if (sprite->data[2] < sprite->data[7])
        {
            if (gSprites[eRoulette->spriteIds[SPR_CLEAR_MON]].animDelayCounter == 0)
            {
                if (gSprites[eRoulette->spriteIds[SPR_CLEAR_MON]].animCmdIndex == 1)
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
    switch (eRoulette->useTaillow)
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
        eRoulette->ballFallSpeed *= -1.0f;
    if (sprite->data[2] == 0)
    {
        if (!sprite->sStillStuck)
        {
            eRoulette->ballState   = BALL_STATE_LANDED;
            eRoulette->ballRolling = FALSE;
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
        eRoulette->ballAngleSpeed = 0;
        eRoulette->stuckHitSlot = (eRoulette->hitSlot + 1) % NUM_ROULETTE_SLOTS;
    }
    else
    {
        eRoulette->ballAngleSpeed = ({float f = sRouletteTables[eRoulette->tableId].var1C; f * 2;});
        eRoulette->stuckHitSlot = eRoulette->hitSlot;
    }
    sprite->sStillStuck = TRUE;
    sprite->data[2] = sRouletteTables[eRoulette->tableId].randDistanceLow;
    eRoulette->ballFallSpeed = 0.085f;
    sprite->callback = SpriteCB_RollBall_TryLandAdjacent;
    sprite->sState = 5;
}

#endif

void SpriteCB_RollBall_TryLand(struct Sprite *sprite)
{
    UpdateBallPos(sprite);
    sprite->data[2] = 0;
    UpdateSlotBelowBall(sprite);
    if (!(sRouletteSlots[eRoulette->hitSlot].flag & eRoulette->hitFlags))
    {
        eRoulette->ballState   = BALL_STATE_LANDED;
        eRoulette->ballRolling = FALSE;
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
            eRoulette->ballAngleSpeed = 0.0f;
            nextSlotId = (eRoulette->hitSlot + 1) % NUM_ROULETTE_SLOTS;
            slotId = nextSlotId;
            eRoulette->stuckHitSlot = nextSlotId;
        }
        else
        {
            eRoulette->ballAngleSpeed = ({
                float f = sRouletteTables[eRoulette->tableId].var1C;
                f * 2;
            }); // couldn't replicate loads
            slotId = (eRoulette->hitSlot + NUM_ROULETTE_SLOTS - 1) % NUM_ROULETTE_SLOTS;
            eRoulette->stuckHitSlot = eRoulette->hitSlot;
        }
        if (sRouletteSlots[slotId].flag & eRoulette->hitFlags)
        {
            sprite->sStillStuck = TRUE;
            sprite->data[2] = sRouletteTables[eRoulette->tableId].randDistanceLow;
        }
        else
        {
            sprite->sStillStuck = sRouletteSlots[slotId].flag & eRoulette->hitFlags;
            if (eRoulette->tableId)
            {
                sprite->data[2] = sRouletteTables[eRoulette->tableId].randDistanceHigh;
            }
            else
            {
                sprite->data[2] = sRouletteTables[eRoulette->tableId].randDistanceLow;
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
        sprite->sState = 5;
    }
}

#undef sStillStuck

void SpriteCB_RollBall_Slow(struct Sprite *sprite)
{
    UpdateBallPos(sprite);
    if (!(eRoulette->ballAngleSpeed > 0.5f))
    {
        UpdateSlotBelowBall(sprite);
        if (!GetBallDistanceToSlotMidpoint(sprite))
        {
            const struct RouletteTable *tables;
            eRoulette->ballAngleAccel = 0.0f;
            tables = sRouletteTables;
            eRoulette->ballAngleSpeed -= ((float)(tables[eRoulette->tableId].wheelSpeed))
                / ((float)(s16)(tables[eRoulette->tableId].wheelDelay + 1));
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

void SpriteCB_RollBall_Medium(struct Sprite *sprite)
{
    UpdateBallPos(sprite);
    if (!(eRoulette->ballDistToCenter > 40.f))
    {
        eRoulette->ballFallSpeed = - (        4.0f / (float)((u16)eRoulette->ballTravelDistSlow));
        eRoulette->ballAngleAccel = - (eRoulette->ballAngleSpeed / (float)((u16)eRoulette->ballTravelDistSlow));
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
    if (!(eRoulette->ballDistToCenter > 60.0f))
    {
        m4aSongNumStartOrChange(SE_ROULETTE_BALL2);
        eRoulette->ballFallSpeed = - (                20.0f / (float)((u16)eRoulette->ballTravelDistMed));
        eRoulette->ballAngleAccel =   ((1.0f - eRoulette->ballAngleSpeed) / (float)((u16)eRoulette->ballTravelDistMed));
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
    roulette = eRoulette;
    roulette->spriteIds[SPR_CLEAR_MON] = CreateSprite(&sSpriteTemplate_Shroomish, 36, -12, 50);
    roulette->spriteIds[SPR_CLEAR_MON_SHADOW_1] = CreateSprite(sSpriteTemplate_ShroomishShadow, coords[ball->sStuckOnWheelLeft][0], coords[ball->sStuckOnWheelLeft][1], 59);
    roulette->spriteIds[SPR_CLEAR_MON_SHADOW_2] = CreateSprite(&sSpriteTemplate_ShroomishShadow[1], 36, 140, 51);
    gSprites[eRoulette->spriteIds[SPR_CLEAR_MON_SHADOW_2]].oam.objMode = 1;
    for (i = 0; i < 3; i++)
    {
        gSprites[eRoulette->spriteIds[SPR_CLEAR_MON + i]].coordOffsetEnabled = FALSE;
        gSprites[eRoulette->spriteIds[SPR_CLEAR_MON + i]].invisible          = TRUE;
        gSprites[eRoulette->spriteIds[SPR_CLEAR_MON + i]].animPaused         = TRUE;
        gSprites[eRoulette->spriteIds[SPR_CLEAR_MON + i]].affineAnimPaused   = TRUE;
        gSprites[eRoulette->spriteIds[SPR_CLEAR_MON + i]].sMonSpriteId = eRoulette->spriteIds[SPR_CLEAR_MON];
        gSprites[eRoulette->spriteIds[SPR_CLEAR_MON + i]].sBallShadowSpriteId = eRoulette->spriteIds[SPR_CLEAR_MON_SHADOW_1];
        gSprites[eRoulette->spriteIds[SPR_CLEAR_MON + i]].sMonShadowSpriteId = eRoulette->spriteIds[SPR_CLEAR_MON_SHADOW_2];
        gSprites[eRoulette->spriteIds[SPR_CLEAR_MON + i]].data[2] = slotOffset;
        gSprites[eRoulette->spriteIds[SPR_CLEAR_MON + i]].data[3] = (ball->data[7] * sRouletteTables[eRoulette->tableId].randDistanceHigh) +
                                                                (sRouletteTables[eRoulette->tableId].randDistanceLow + 0xffff);
    }
    gSprites[eRoulette->spriteIds[SPR_CLEAR_MON_SHADOW_1]].coordOffsetEnabled = TRUE;
    eRoulette->ball = ball;
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
    eRoulette->spriteIds[SPR_CLEAR_MON] = CreateSprite(&sSpriteTemplate_Taillow, coords[ball->sStuckOnWheelLeft][0], coords[ball->sStuckOnWheelLeft][1], 50);
    StartSpriteAnim(&gSprites[eRoulette->spriteIds[SPR_CLEAR_MON]], ball->sStuckOnWheelLeft);
    eRoulette->spriteIds[SPR_CLEAR_MON_SHADOW_1] = CreateSprite(&sSpriteTemplate_TaillowShadow, coords[ball->sStuckOnWheelLeft][0], coords[ball->sStuckOnWheelLeft][1], 51);
    gSprites[eRoulette->spriteIds[SPR_CLEAR_MON_SHADOW_1]].affineAnimPaused = TRUE;
    gSprites[eRoulette->spriteIds[SPR_CLEAR_MON_SHADOW_1]].animPaused       = TRUE;
    ball->data[7] = (slotOffset * (tables = sRouletteTables)[eRoulette->tableId].randDistanceHigh) + (tables[eRoulette->tableId].taillow.baseDropDelay + 45);
    for (; i < 2; i++)
    {
        gSprites[eRoulette->spriteIds[SPR_CLEAR_MON + i]].sMonSpriteId = eRoulette->spriteIds[SPR_CLEAR_MON];
        gSprites[eRoulette->spriteIds[SPR_CLEAR_MON + i]].sBallShadowSpriteId = eRoulette->spriteIds[SPR_CLEAR_MON_SHADOW_1];
        gSprites[eRoulette->spriteIds[SPR_CLEAR_MON + i]].sMonShadowSpriteId = eRoulette->spriteIds[SPR_CLEAR_MON_SHADOW_1];
        gSprites[eRoulette->spriteIds[SPR_CLEAR_MON + i]].data[2] = slotOffset;
        gSprites[eRoulette->spriteIds[SPR_CLEAR_MON + i]].data[3] = ball->data[7] - 45;
    }
    eRoulette->ball = ball;
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

    eRoulette->ballState = BALL_STATE_STUCK;
    eRoulette->ballStuck = TRUE;
    eRoulette->ballUnstuck = FALSE;
    eRoulette->hitSlot = 0xFF;
    eRoulette->ballAngle = sprite->sBallAngle;
    eRoulette->ballFallSpeed = 0.0f;
    eRoulette->ballAngleSpeed = sRouletteTables[eRoulette->tableId].var1C;

    angle = (eRoulette->tableId * DEGREES_PER_SLOT + 33) + (1 - eRoulette->useTaillow) * 15;
    for (i = 0; i < 4; i++)
    {
        if (angle < sprite->sBallAngle && sprite->sBallAngle <= angle + 90)
        {
            sprite->sStuckOnWheelLeft = i / 2;
            eRoulette->useTaillow = i % 2;
            break;
        }
        if (i == 3)
        {
            sprite->sStuckOnWheelLeft = TRUE;
            eRoulette->useTaillow = TRUE;
            break;
        }
        angle += 90;
    }

    if (eRoulette->useTaillow)
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
    slotId = (eRoulette->stuckHitSlot + 2) % NUM_ROULETTE_SLOTS;

    if (eRoulette->useTaillow == TRUE && eRoulette->tableId == 1)
        maxSlotToCheck += BALLS_PER_ROUND;
    else
        maxSlotToCheck += slotsToSkip;

    for (i = slotsToSkip; i < maxSlotToCheck; i++)
    {
        if (!(eRoulette->hitFlags & sRouletteSlots[slotId].flag))
        {
            slotCandidates[numCandidates++] = i;
            if (betSlotId == 0 && (sRouletteSlots[slotId].flag & sGridSelections[eRoulette->betSelection[eRoulette->curBallNum]].inSelectionFlags))
                betSlotId = i;
        }
        slotId = (slotId + 1) % NUM_ROULETTE_SLOTS;
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

void SpriteCB_ShroomishExit(struct Sprite *sprite)
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
        gSprites[eRoulette->spriteIds[SPR_CLEAR_MON]].animPaused = FALSE;
        DestroySprite(sprite);
    }
}

void SpriteCB_ShroomishFall(struct Sprite *sprite)
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
        if (eRoulette->ball->sStuckOnWheelLeft == FALSE)
        {
            ballAngle = eRoulette->ball->sBallAngle;
            startAngle = sRouletteTables[eRoulette->tableId].shroomish.startAngle;
        }
        else
        {
            ballAngle = eRoulette->ball->sBallAngle;
            startAngle = sRouletteTables[eRoulette->tableId].shroomish.startAngle;
            startAngle += 180;
        }
        if (ballAngle == startAngle)
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
        u32 ballAngle, dropAngle;
        eRoulette->shroomishShadowAlpha = sShroomishShadowAlphas[(eRoulette->shroomishShadowTimer - 1) / 2];
        if (eRoulette->shroomishShadowTimer < 19)
            eRoulette->shroomishShadowTimer++;
        if (eRoulette->ball->sStuckOnWheelLeft == FALSE)
        {
            ballAngle = eRoulette->ball->sBallAngle;
            dropAngle = sRouletteTables[eRoulette->tableId].shroomish.dropAngle;
        }
        else
        {
            ballAngle = eRoulette->ball->sBallAngle;
            dropAngle = sRouletteTables[eRoulette->tableId].shroomish.dropAngle;
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
        FreeOamMatrix(gSprites[eRoulette->spriteIds[SPR_CLEAR_MON_SHADOW_1]].oam.matrixNum);
        DestroySprite(&gSprites[eRoulette->spriteIds[SPR_CLEAR_MON_SHADOW_1]]);
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
            StartSpriteAnim(sprite, eRoulette->ball->sStuckOnWheelLeft + 4);
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
        sprite->x += xMoveOffsets[eRoulette->ball->sStuckOnWheelLeft] * 2;
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
            sprite->x += xMoveOffsets[eRoulette->ball->sStuckOnWheelLeft] * yMoveOffsets[7 - sprite->data[1]][0];
            sprite->y += yMoveOffsets[7 - sprite->data[1]][1];
        }
        else
        {
            m4aSongNumStartOrChange(SE_TAILLOW_WING_FLAP);
            if (eRoulette->ball->sStuckOnWheelLeft == FALSE)
                PlayCry_Normal(SPECIES_TAILLOW, 63);
            else
                PlayCry_Normal(SPECIES_TAILLOW, -63);
            StartSpriteAnim(sprite, eRoulette->ball->sStuckOnWheelLeft + 2);
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
        sprite->x += moveDir[eRoulette->ball->sStuckOnWheelLeft] * 2;
        gSprites[sprite->sMonShadowSpriteId].invisible ^= 1;
    }
    else
    {
        sprite->callback = SpriteCB_TaillowShadow_Flash;
    }
}

void SpriteCB_Taillow(struct Sprite *sprite)
{
    if (eRoulette->ball->sStuckOnWheelLeft == FALSE)
    {
        if (eRoulette->ball->sBallAngle == sRouletteTables[eRoulette->tableId].taillow.rightStartAngle + 90)
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
        if (eRoulette->ball->sBallAngle == sRouletteTables[eRoulette->tableId].taillow.leftStartAngle + 270)
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
