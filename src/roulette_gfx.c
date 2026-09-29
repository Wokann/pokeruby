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

s16 UpdateBallRelativeWheelAngle(struct Sprite *);
u8 UpdateSlotBelowBall(struct Sprite *);
s16 GetBallDistanceToSlotMidpoint(struct Sprite *);
void UpdateBallPos(struct Sprite *);
void SpriteCB_BallLandInSlot(struct Sprite *);
void SpriteCB_UnstickBall_ShroomishBallFall(struct Sprite *);
void SpriteCB_UnstickBall_Shroomish(struct Sprite *);
void SpriteCB_UnstickBall_TaillowDrop(struct Sprite *);
void SpriteCB_UnstickBall_TaillowPickUp(struct Sprite *);
void SpriteCB_UnstickBall_Taillow(struct Sprite *);
void SpriteCB_UnstickBall(struct Sprite *);
void SpriteCB_RollBall_TryLandAdjacent(struct Sprite *);
void SpriteCB_RollBall_TryLand(struct Sprite *);
void SpriteCB_RollBall_Slow(struct Sprite *);
void SpriteCB_RollBall_Medium(struct Sprite *);
void SpriteCB_RollBall_Fast(struct Sprite *);
void CreateShroomishSprite(struct Sprite *);
void CreateTaillowSprite(struct Sprite *);
void SetBallStuck(struct Sprite *);
void SpriteCB_ShroomishExit(struct Sprite *);
void SpriteCB_ShroomishShakeScreen(struct Sprite *);
void SpriteCB_ShroomishFall(struct Sprite *);
void SpriteCB_Shroomish(struct Sprite *);
void SpriteCB_TaillowShadow_Flash(struct Sprite *);
void SpriteCB_Taillow_FlyAway(struct Sprite *);
void SpriteCB_Taillow_PickUpBall(struct Sprite *);
void SpriteCB_Taillow_FlyIn(struct Sprite *);
void SpriteCB_TaillowShadow_FlyIn(struct Sprite *);
void SpriteCB_Taillow(struct Sprite *);

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
const u16 RouletteSpritePalette_08[] = INCBIN_U16("graphics/roulette/poke_icons.gbapal");
const u16 RouletteSpritePalette_09[] = INCBIN_U16("graphics/roulette/wynaut.gbapal");
const u16 RouletteSpritePalette_10[] = INCBIN_U16("graphics/roulette/azurill.gbapal");
const u16 RouletteSpritePalette_11[] = INCBIN_U16("graphics/roulette/skitty.gbapal");
const u16 RouletteSpritePalette_12[] = INCBIN_U16("graphics/roulette/makuhita.gbapal");
const u16 RouletteSpritePalette_13[] = INCBIN_U16("graphics/roulette/83F907C.gbapal");
const u16 RouletteSpritePalette_14[] = INCBIN_U16("graphics/roulette/83F909C.gbapal");
const u16 RouletteSpritePalette_15[] = INCBIN_U16("graphics/roulette/83F90BC.gbapal");
const u16 RouletteSpritePalette_16[] = INCBIN_U16("graphics/roulette/83F90DC.gbapal");
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

const u8 sBall_Gfx[] = INCBIN_U8("graphics/roulette/ball.4bpp.lz");
const u8 RouletteBallCounterTiles[] = INCBIN_U8("graphics/roulette/ball_counter.4bpp.lz");
const u8 sShroomishTaillow_Gfx[] = INCBIN_U8("graphics/roulette/roulette_tilt.4bpp.lz");
const u8 RoulettePokeIconsTiles[] = INCBIN_U8("graphics/roulette/poke_icons.4bpp.lz");
const u8 RoulettePokeIcons2Tiles[] = INCBIN_U8("graphics/roulette/poke_icons2.4bpp.lz");
const u8 sShadow_Gfx[] = INCBIN_U8("graphics/roulette/shadow.4bpp.lz");
const u8 RouletteCursorTiles[] = INCBIN_U8("graphics/roulette/cursor.4bpp.lz");

const struct SpritePalette sSpritePalettes[] = {
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

const struct OamData gOamData_83F9EB8 = {
    .shape = ST_OAM_SQUARE,
    .size = 2,
    .priority = 1
};

const struct OamData gOamData_83F9EC0 = {
    .shape = ST_OAM_SQUARE,
    .size = 1,
    .priority = 1
};

const struct OamData gOamData_83F9EC8 = {
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

const union AffineAnimCmd gSpriteAffineAnim_83F9EDC[] = {
    AFFINEANIMCMD_END
};

const union AffineAnimCmd *const gSpriteAffineAnimTable_83F9EE4[] = {
    gSpriteAffineAnim_83F9EDC
};

const struct CompressedSpriteSheet sSpriteSheet_WheelIcons = {
    RoulettePokeIcons2Tiles,
    0xc00,
    0
};

const union AnimCmd gSpriteAnim_83F9EF0[] = {
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

const union AnimCmd *const gSpriteAnimTable_83F9F24[] = {
    gSpriteAnim_83F9EF0 +  0
};

const union AnimCmd *const gSpriteAnimTable_83F9F28[] = {
    gSpriteAnim_83F9EF0 +  1
};

const union AnimCmd *const gSpriteAnimTable_83F9F2C[] = {
    gSpriteAnim_83F9EF0 +  2
};

const union AnimCmd *const gSpriteAnimTable_83F9F30[] = {
    gSpriteAnim_83F9EF0 +  3
};

const union AnimCmd *const gSpriteAnimTable_83F9F34[] = {
    gSpriteAnim_83F9EF0 +  4
};

const union AnimCmd *const gSpriteAnimTable_83F9F38[] = {
    gSpriteAnim_83F9EF0 +  5
};

const union AnimCmd *const gSpriteAnimTable_83F9F3C[] = {
    gSpriteAnim_83F9EF0 +  6
};

const union AnimCmd *const gSpriteAnimTable_83F9F40[] = {
    gSpriteAnim_83F9EF0 +  7
};

const union AnimCmd *const gSpriteAnimTable_83F9F44[] = {
    gSpriteAnim_83F9EF0 +  8
};

const union AnimCmd *const gSpriteAnimTable_83F9F48[] = {
    gSpriteAnim_83F9EF0 +  9
};

const union AnimCmd *const gSpriteAnimTable_83F9F4C[] = {
    gSpriteAnim_83F9EF0 + 10
};

const union AnimCmd *const gSpriteAnimTable_83F9F50[] = {
    gSpriteAnim_83F9EF0 + 11
};

const struct CompressedSpriteSheet sSpriteSheet_Headers = {
    gRouletteHeadersTiles,
    0x1600,
    4
};

const struct CompressedSpriteSheet sSpriteSheet_GridIcons = {
    RoulettePokeIconsTiles,
    0x400,
    5
};

const union AnimCmd gSpriteAnim_83F9F64[] = {
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

const union AnimCmd gSpriteAnim_83F9F94[] = {
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_FRAME(4, 0),
    ANIMCMD_FRAME(8, 0),
    ANIMCMD_FRAME(12, 0),
    ANIMCMD_END
};

const union AnimCmd *const gSpriteAnimTable_83F9FA8[] = {
    gSpriteAnim_83F9F64 +  0
};

const union AnimCmd *const gSpriteAnimTable_83F9FAC[] = {
    gSpriteAnim_83F9F64 +  2
};

const union AnimCmd *const gSpriteAnimTable_83F9FB0[] = {
    gSpriteAnim_83F9F64 +  4
};

const union AnimCmd *const gSpriteAnimTable_83F9FB4[] = {
    gSpriteAnim_83F9F64 +  6
};

const union AnimCmd *const gSpriteAnimTable_83F9FB8[] = {
    gSpriteAnim_83F9F64 +  8
};

const union AnimCmd *const gSpriteAnimTable_83F9FBC[] = {
    gSpriteAnim_83F9F64 +  9
};

const union AnimCmd *const gSpriteAnimTable_83F9FC0[] = {
    gSpriteAnim_83F9F64 + 10
};

const union AnimCmd *const gSpriteAnimTable_83F9FC4[] = {
    gSpriteAnim_83F9F94 + 0
};

const union AnimCmd *const gSpriteAnimTable_83F9FC8[] = {
    gSpriteAnim_83F9F94 + 1
};

const union AnimCmd *const gSpriteAnimTable_83F9FCC[] = {
    gSpriteAnim_83F9F94 + 2
};

const union AnimCmd *const gSpriteAnimTable_83F9FD0[] = {
    gSpriteAnim_83F9F94 + 3
};

const struct SpriteTemplate gSpriteTemplate_83F9FD4[] = {
    {4, 8, &gOamData_83F9EB8, gSpriteAnimTable_83F9FA8, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare},
    {4, 8, &gOamData_83F9EB8, gSpriteAnimTable_83F9FAC, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare},
    {4, 8, &gOamData_83F9EB8, gSpriteAnimTable_83F9FB0, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare},
    {4, 8, &gOamData_83F9EB8, gSpriteAnimTable_83F9FB4, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare}
};

const struct SpriteTemplate gSpriteTemplate_83FA034[] = {
    {4, 8, &gOamData_83F9EB8, gSpriteAnimTable_83F9FB8, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare},
    {4, 8, &gOamData_83F9EB8, gSpriteAnimTable_83F9FBC, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare},
    {4, 8, &gOamData_83F9EB8, gSpriteAnimTable_83F9FC0, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare}
};

const struct SpriteTemplate gSpriteTemplate_83FA07C[] ={
    {5, 8, &gOamData_83F9EC0, gSpriteAnimTable_83F9FC4, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare},
    {5, 8, &gOamData_83F9EC0, gSpriteAnimTable_83F9FC8, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare},
    {5, 8, &gOamData_83F9EC0, gSpriteAnimTable_83F9FCC, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare},
    {5, 8, &gOamData_83F9EC0, gSpriteAnimTable_83F9FD0, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare}
};

const struct SpriteTemplate gSpriteTemplate_83FA0DC[] = {
    {0, 9, &gOamData_83F9EC8, gSpriteAnimTable_83F9F24, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {0, 10, &gOamData_83F9EC8, gSpriteAnimTable_83F9F28, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {0, 11, &gOamData_83F9EC8, gSpriteAnimTable_83F9F2C, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {0, 12, &gOamData_83F9EC8, gSpriteAnimTable_83F9F30, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {0, 9, &gOamData_83F9EC8, gSpriteAnimTable_83F9F34, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {0, 10, &gOamData_83F9EC8, gSpriteAnimTable_83F9F38, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {0, 11, &gOamData_83F9EC8, gSpriteAnimTable_83F9F3C, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {0, 12, &gOamData_83F9EC8, gSpriteAnimTable_83F9F40, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {0, 9, &gOamData_83F9EC8, gSpriteAnimTable_83F9F44, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {0, 10, &gOamData_83F9EC8, gSpriteAnimTable_83F9F48, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {0, 11, &gOamData_83F9EC8, gSpriteAnimTable_83F9F4C, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon},
    {0, 12, &gOamData_83F9EC8, gSpriteAnimTable_83F9F50, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelIcon}
};

const struct OamData gOamData_83FA1FC = {
    .shape = ST_OAM_H_RECTANGLE,
    .size = 3,
    .priority = 1
};

const struct OamData gOamData_83FA204 = {
    .shape = ST_OAM_V_RECTANGLE,
    .size = 0,
    .priority = 1
};

const struct OamData gOamData_83FA20C = {
    .shape = ST_OAM_H_RECTANGLE,
    .size = 2,
    .priority = 1
};

const struct OamData gOamData_83FA214 = {
    .shape = ST_OAM_H_RECTANGLE,
    .size = 0,
    .priority = 1
};

const struct SpriteSheet sSpriteSheets_Interface[] = {
    {gRouletteCreditTiles, 0x400, 7},
    {gRouletteNumbersTiles, 0x280, 8},
    {gRouletteMultiplierTiles, 0x500, 9},
    {RouletteBallCounterTiles, 0x140, 10},
    {RouletteCursorTiles, 0x200, 11},
    {}
};

const union AnimCmd gSpriteAnim_83FA24C[] = {
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

const union AnimCmd *const gSpriteAnimTable_83FA274[] = {
    gSpriteAnim_83FA24C
};

const union AnimCmd gSpriteAnim_83FA278[] = {
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_FRAME(8, 0),
    ANIMCMD_FRAME(16, 0),
    ANIMCMD_FRAME(24, 0),
    ANIMCMD_FRAME(32, 0),
    ANIMCMD_END
};

const union AnimCmd *const gSpriteAnimTable_83FA290[] = {
    gSpriteAnim_83FA278
};

const union AnimCmd gSpriteAnim_83FA294[] = {
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_FRAME(2, 0),
    ANIMCMD_FRAME(4, 0),
    ANIMCMD_FRAME(6, 0),
    ANIMCMD_FRAME(8, 0),
    ANIMCMD_END
};

const union AnimCmd *const gSpriteAnimTable_83FA2AC[] = {
    gSpriteAnim_83FA294
};

const struct SpriteTemplate gSpriteTemplate_83FA2B0 = {7, 5, &gOamData_83FA1FC, gDummySpriteAnimTable, NULL, gDummySpriteAffineAnimTable, SpriteCallbackDummy};

const struct SpriteTemplate gSpriteTemplate_83FA2C8 = {8, 5, &gOamData_83FA204, gSpriteAnimTable_83FA274, NULL, gDummySpriteAffineAnimTable, SpriteCallbackDummy};

const struct SpriteTemplate gSpriteTemplate_83FA2E0 = {9, 5, &gOamData_83FA20C, gSpriteAnimTable_83FA290, NULL, gDummySpriteAffineAnimTable, SpriteCB_GridSquare};

const struct SpriteTemplate gSpriteTemplate_83FA2F8 = {10, 3, &gOamData_83FA214, gSpriteAnimTable_83FA2AC, NULL, gDummySpriteAffineAnimTable, SpriteCallbackDummy};

const struct SpriteTemplate gSpriteTemplate_83FA310 = {11, 5, &gOamData_83F9EB8, gDummySpriteAnimTable, NULL, gDummySpriteAffineAnimTable, SpriteCallbackDummy};

const struct OamData gOamData_83FA328 = {
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

const union AnimCmd gSpriteAnim_83FA350[] = {
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_FRAME(1, 5),
    ANIMCMD_FRAME(2, 5),
    ANIMCMD_FRAME(1, 5),
    ANIMCMD_JUMP(0)
};

const union AnimCmd gSpriteAnim_83FA364[] = {
    ANIMCMD_FRAME(0, 10),
    ANIMCMD_FRAME(1, 10),
    ANIMCMD_FRAME(2, 10),
    ANIMCMD_FRAME(1, 10),
    ANIMCMD_JUMP(0)
};

const union AnimCmd gSpriteAnim_83FA378[] = {
    ANIMCMD_FRAME(0, 15),
    ANIMCMD_FRAME(1, 15),
    ANIMCMD_FRAME(2, 15),
    ANIMCMD_FRAME(1, 15),
    ANIMCMD_JUMP(0)
};

const union AnimCmd gSpriteAnim_83FA38C[] = {
    ANIMCMD_FRAME(1, 2),
    ANIMCMD_FRAME(2, 5),
    ANIMCMD_FRAME(1, 5),
    ANIMCMD_FRAME(3, 5),
    ANIMCMD_END
};

const union AnimCmd gSpriteAnim_83FA3A0[] = {
    ANIMCMD_FRAME(1, 2),
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(1, 4),
    ANIMCMD_FRAME(2, 4),
    ANIMCMD_FRAME(3, 4),
    ANIMCMD_END
};

const union AnimCmd gSpriteAnim_83FA3B8[] = {
    ANIMCMD_FRAME(0, 2),
    ANIMCMD_FRAME(1, 5),
    ANIMCMD_FRAME(2, 5),
    ANIMCMD_FRAME(3, 5),
    ANIMCMD_END
};

const union AnimCmd gSpriteAnim_83FA3CC[] = {
    ANIMCMD_FRAME(3, 0),
    ANIMCMD_END
};

const union AnimCmd gSpriteAnim_83FA3D4[] = {
    ANIMCMD_FRAME(2, 2),
    ANIMCMD_FRAME(1, 5),
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_FRAME(3, 5),
    ANIMCMD_END
};

const union AnimCmd *const gSpriteAnimTable_83FA3E8[] = {
    gSpriteAnim_83FA350,
    gSpriteAnim_83FA364,
    gSpriteAnim_83FA378,
    gSpriteAnim_83FA38C,
    gSpriteAnim_83FA3D4,
    gSpriteAnim_83FA3A0,
    gSpriteAnim_83FA3B8,
    gSpriteAnim_83FA3B8,
    gSpriteAnim_83FA3CC
};

const struct SpriteTemplate gSpriteTemplate_83FA40C = {0xffff, 2, &gOamData_83FA328, gSpriteAnimTable_83FA3E8, gSpriteImageTable_83FA330, gDummySpriteAffineAnimTable, SpriteCallbackDummy};

const struct OamData gOamData_83FA424 = {
    .y = 81,
    .affineMode = ST_OAM_AFFINE_DOUBLE,
    .shape = ST_OAM_SQUARE,
    .size = 3,
    .priority = 2
};

const struct CompressedSpriteSheet sSpriteSheet_WheelCenter = {
    gRouletteCenter_Gfx,
    0x800,
    6
};

const struct SpriteTemplate gSpriteTemplate_83FA434 = {6, 2, &gOamData_83FA424, gDummySpriteAnimTable, NULL, gDummySpriteAffineAnimTable, SpriteCB_WheelCenter};

const struct OamData gOamData_83FA44C = {
    .shape = ST_OAM_SQUARE,
    .size = 2,
    .priority = 2
};

const struct OamData gOamData_83FA454 = {
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

const union AnimCmd gSpriteAnim_83FA494[] = {
    ANIMCMD_FRAME(0, 6),
    ANIMCMD_FRAME(1, 6),
    // jump here
    ANIMCMD_FRAME(2, 6),
    ANIMCMD_FRAME(3, 6),
    ANIMCMD_FRAME(2, 6),
    ANIMCMD_FRAME(4, 6),
    ANIMCMD_JUMP(2)
};

const union AnimCmd gSpriteAnim_83FA4B0[] = {
    ANIMCMD_FRAME(5, 10),
    ANIMCMD_END
};

const union AnimCmd gSpriteAnim_83FA4B8[] = {
    ANIMCMD_FRAME(5, 10, .hFlip = TRUE),
    ANIMCMD_END
};

const union AnimCmd gSpriteAnim_83FA4C0[] = {
    ANIMCMD_FRAME(5, 20),
    ANIMCMD_FRAME(6, 20),
    ANIMCMD_JUMP(0)
};

const union AnimCmd gSpriteAnim_83FA4CC[] = {
    ANIMCMD_FRAME(5, 20, .hFlip = TRUE),
    ANIMCMD_FRAME(6, 20, .hFlip = TRUE),
    ANIMCMD_JUMP(0)
};

const union AnimCmd gSpriteAnim_83FA4D8[] = {
    ANIMCMD_FRAME(5, 10),
    ANIMCMD_FRAME(6, 10),
    ANIMCMD_JUMP(0)
};

const union AnimCmd gSpriteAnim_83FA4E4[] = {
    ANIMCMD_FRAME(5, 10, .hFlip = TRUE),
    ANIMCMD_FRAME(6, 10, .hFlip = TRUE),
    ANIMCMD_JUMP(0)
};

const union AnimCmd *const gSpriteAnimTable_83FA4F0[] = {
    gSpriteAnim_83FA494
};

const union AnimCmd *const gSpriteAnimTable_83FA4F4[] = {
    gSpriteAnim_83FA4B0,
    gSpriteAnim_83FA4B8,
    gSpriteAnim_83FA4C0,
    gSpriteAnim_83FA4CC,
    gSpriteAnim_83FA4D8,
    gSpriteAnim_83FA4E4
};

const struct SpriteTemplate gSpriteTemplate_83FA50C = {0xffff, 6, &gOamData_83FA44C, gSpriteAnimTable_83FA4F0, gSpriteImageTable_83FA45C, gDummySpriteAffineAnimTable, SpriteCallbackDummy};

const struct SpriteTemplate gSpriteTemplate_83FA524 = {0xffff, 7, &gOamData_83FA454, gSpriteAnimTable_83FA4F4, gSpriteImageTable_83FA45C, gDummySpriteAffineAnimTable, SpriteCB_Taillow};

const struct OamData gOamData_83FA53C = {
    .affineMode = ST_OAM_AFFINE_OFF,
    .shape = ST_OAM_SQUARE,
    .size = 1,
    .priority = 2
};

const struct OamData gOamData_83FA544 = {
    .affineMode = ST_OAM_AFFINE_OFF,
    .shape = ST_OAM_H_RECTANGLE,
    .size = 2,
    .priority = 2
};

const struct OamData gOamData_83FA54C = {
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

const union AffineAnimCmd gSpriteAffineAnim_83FA564[] = {
    AFFINEANIMCMD_FRAME(128, 128, 0,  0),
    AFFINEANIMCMD_FRAME(  2,   2, 0, 60),
    AFFINEANIMCMD_END
};

const union AffineAnimCmd gSpriteAffineAnim_83FA57C[] = {
    AFFINEANIMCMD_FRAME(256, 256, 0,  0),
    AFFINEANIMCMD_FRAME( -2,   0, 0, 15),
    AFFINEANIMCMD_FRAME( -1,  -2, 0, 15),
    AFFINEANIMCMD_FRAME( -1,  -5, 0, 24),
    AFFINEANIMCMD_END
};

const union AffineAnimCmd *const gSpriteAffineAnimTable_83FA5A4[] = {
    gSpriteAffineAnim_83FA564
};

const union AffineAnimCmd *const gSpriteAffineAnimTable_83FA5A8[] = {
    gSpriteAffineAnim_83FA57C
};

const union AffineAnimCmd gSpriteAffineAnim_83FA5AC[] = {
    AFFINEANIMCMD_FRAME(256, 256, 0, 0),
    AFFINEANIMCMD_END
};

const union AffineAnimCmd *const gSpriteAffineAnimTable_83FA5BC[] = {
    gSpriteAffineAnim_83FA5AC
};

const struct SpriteTemplate gSpriteTemplate_83FA5C0[] = {
    {0xffff, 1, &gOamData_83FA53C, gSpriteAnimTable_83F9ED8, gSpriteImageTable_83FA554, gDummySpriteAffineAnimTable, SpriteCallbackDummy},
    {0xffff, 1, &gOamData_83FA544, gSpriteAnimTable_83F9ED8, gSpriteImageTable_83FA55C, gDummySpriteAffineAnimTable, SpriteCB_Shroomish}
};

const struct SpriteTemplate gSpriteTemplate_83FA5F0 = {0xffff, 1, &gOamData_83FA54C, gSpriteAnimTable_83F9ED8, gSpriteImageTable_83FA55C, gSpriteAffineAnimTable_83FA5A8, SpriteCB_Taillow};

void LoadOrFreeMiscSpritePalettesAndSheets(u8 r0)
{
    if (!r0)
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

u8 CreateWheelIconSprite(const struct SpriteTemplate *r0, u8 r1, u16 *r2)
{
    u16 temp;
    u8 spriteid = CreateSprite(r0, 116, 80, r0->oam->y);
    gSprites[spriteid].data[0]            = *r2;
    gSprites[spriteid].data[1]            = r1;
    gSprites[spriteid].coordOffsetEnabled = TRUE;
    gSprites[spriteid].animPaused         = TRUE;
    gSprites[spriteid].affineAnimPaused   = TRUE;
    temp = *r2;
    *r2 += 30;
    if (*r2 >= 360)
        *r2 = temp - 330;
    return spriteid;
}

void CreateGridSprites(void)
{
    u8 i, j;
    u8 spriteid;
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
        u8 o = i * 24;
        for (j = 0; j < 4; j++)
        {
            spriteid = eRoulette->spriteIds[29 + i * 4 + j] =
                CreateSprite(&gSpriteTemplate_83FA07C[j], j * 24 + 148, o + 92, 30);
            gSprites[spriteid].animPaused = TRUE;
            o += 24;
            if (o > 71)
                o = 0;
        }
    }
    for (i = 0; i < 4; i++)
    {
        spriteid = eRoulette->spriteIds[41 + i] =
            CreateSprite(&gSpriteTemplate_83F9FD4[i], (s16)(i * 24 + 148), 70, 30);
        gSprites[spriteid].animPaused = TRUE;
    }
    for (i = 0; i < 3; i++)
    {
        spriteid = eRoulette->spriteIds[45 + i] =
            CreateSprite(&gSpriteTemplate_83FA034[i], 126, (s16)(i * 24 + 92), 30);
        gSprites[spriteid].animPaused = TRUE;
    }
}

void DestroyGridSprites(void) //destroy all sprites at 29
{
    u8 i;
    for (i = 0; i < 12; i++)
        DestroySprite(&gSprites[eRoulette->spriteIds[29 + i]]);
}

void ShowHideGridIcons(u8 r0, u8 r1)
{
    u8 i;
    switch (r0)
    {
    case 1:
        for (i = 0; i < 19; i++)
            gSprites[eRoulette->spriteIds[29 + i]].invisible = TRUE;
        break;
    case 0:
        for (i = 0; i < 12; i++)
        {
            if (!(eRoulette->hitFlags & sRouletteSlots[i].flag))
                gSprites[eRoulette->spriteIds[29 + i]].invisible = FALSE;
            else if(!(sRouletteSlots[i].gridSquare == r1))
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
        eRoulette->spriteIds[49 + i] = CreateSprite(&gSpriteTemplate_83FA40C, 116, 20, 10);
        gSprites[eRoulette->spriteIds[49 + i]].invisible    = TRUE;
        gSprites[eRoulette->spriteIds[49 + i]].data[0]      = 1;
        gSprites[eRoulette->spriteIds[49 + i]].callback     = SpriteCB_GridSquare;
        gSprites[eRoulette->spriteIds[49 + i]].oam.priority = 1;
        StartSpriteAnim(&gSprites[eRoulette->spriteIds[49 + i]], 8);
    }
}

void ShowHideGridBalls(u8 r0, u8 r1)
{
    u8 i = 0;
    if (r0)
    {
        for ( ; i < 6; i++)
            gSprites[eRoulette->spriteIds[49 + i]].invisible = TRUE;
    }
    else
    {
        for ( ; i < 6; i++)
        {
            if (!(eRoulette->hitSquares[i]) || (i == r1))
            {
                gSprites[eRoulette->spriteIds[49 + i]].invisible = TRUE;
            }
            else
            {
                gSprites[eRoulette->spriteIds[49 + i]].invisible = FALSE;
                gSprites[eRoulette->spriteIds[49 + i]].x         = (sGridSelections[eRoulette->hitSquares[i]].x + 1) * 8 + 4;
                gSprites[eRoulette->spriteIds[49 + i]].y         = (sGridSelections[eRoulette->hitSquares[i]].y + 1) * 8 + 3;
            }
        }
    }
}

void ShowHideWinSlotCursor(u8 r0)
{
    if (!r0)
    {
        gSprites[eRoulette->spriteIds[48]].invisible = TRUE;
    }
    else
    {
        gSprites[eRoulette->spriteIds[48]].invisible = FALSE;
        gSprites[eRoulette->spriteIds[48]].x    = (sGridSelections[r0].x + 2) * 8;
        gSprites[eRoulette->spriteIds[48]].y    = (sGridSelections[r0].y + 2) * 8;
    }
}

void CreateWheelIconSprites(void)
{
    u8 i, j;
    u16 k;
    struct SpriteSheet s;
    LZ77UnCompWram(sSpriteSheet_WheelIcons.data, gSharedMem);
    s.data = gSharedMem;
    s.size = sSpriteSheet_WheelIcons.size;
    s.tag  = sSpriteSheet_WheelIcons.tag;
    LoadSpriteSheet(&s);
    k = 15;
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 4; j++)
        {
            u8 spriteid;
            spriteid =
                eRoulette->spriteIds[7 + i * 4 + j] = CreateWheelIconSprite(&gSpriteTemplate_83FA0DC[i * 4 + j], 40, &k);
            gSprites[spriteid].animPaused       = TRUE;
            gSprites[spriteid].affineAnimPaused = TRUE;
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
    eRoulette->spriteIds[20] = CreateSprite(&gSpriteTemplate_83FA2B0, 208, 16, 4);
    gSprites[eRoulette->spriteIds[20]].animPaused = TRUE;
    for (i = 0; i < 4; i++)
    {
        eRoulette->spriteIds[21 + i] = CreateSprite(&gSpriteTemplate_83FA2C8, (s16)(i * 8 + 196), 24, 0);
        gSprites[eRoulette->spriteIds[21 + i]].invisible = TRUE;
        gSprites[eRoulette->spriteIds[21 + i]].animPaused = TRUE;
    }
    eRoulette->spriteIds[25] = CreateSprite(&gSpriteTemplate_83FA2E0, 120, 68, 4);
    gSprites[eRoulette->spriteIds[25]].animPaused = TRUE;
    for (i = 0; i < 3; i++)
    {
        eRoulette->spriteIds[26 + i] = CreateSprite(&gSpriteTemplate_83FA2F8, (s16)(i * 16 + 192), 36, 4);
        gSprites[eRoulette->spriteIds[26 + i]].invisible = TRUE;
        gSprites[eRoulette->spriteIds[26 + i]].animPaused = TRUE;
    }
    eRoulette->spriteIds[48] = CreateSprite(&gSpriteTemplate_83FA310, 152, 96, 9);
    gSprites[eRoulette->spriteIds[48]].oam.priority = 1;
    gSprites[eRoulette->spriteIds[48]].animPaused = TRUE;
    gSprites[eRoulette->spriteIds[48]].invisible = TRUE;
}

void SetCreditDigits(u16 r0)
{
    u8 i;
    u16 d = 1000;
    bool8 v = FALSE;
    for (i = 0; i < 4; i++)
    {
        u8 t = r0 / d;
        gSprites[eRoulette->spriteIds[21 + i]].invisible = TRUE;
        if (t > 0 || v || i == 3)
        {
            gSprites[eRoulette->spriteIds[21 + i]].invisible = FALSE;
            gSprites[eRoulette->spriteIds[21 + i]].oam.tileNum =
                gSprites[eRoulette->spriteIds[21 + i]].sheetTileStart
                + (*gSprites[eRoulette->spriteIds[21 + i]].anims + t)->type;
            v = TRUE;
        }
        r0 = r0 % d;
        d = d / 10;
    }
}

u8 GetMultiplierAnimId(u8 r0)
{
    u8 t[] = {0, 1, 2, 3, 4};
    if (r0 >= 20)
        r0 = 0;
    switch (sGridSelections[r0].baseMultiplier)
    {
    case 3:
        r0 = r0 / 5 - 1;
        if (eRoulette->colorHits[r0] < 4)
            return t[((vu8 *)eRoulette->colorHits)[r0] + 1]; // couldn't recreate redundant loads
        break;
    case 4:
        r0--;
        if (eRoulette->pokeHits[r0] < 3)
            return t[((vu8 *)eRoulette->pokeHits)[r0] + 2];
        break;
    case 12:
        if (!(eRoulette->hitFlags & sGridSelections[r0].flag))
            return t[4];
        break;
    }
    return 0;
}

void SetMultiplierSprite(u8 r0)
{
    struct Sprite *s = &gSprites[eRoulette->spriteIds[25]];
    s->animCmdIndex  = GetMultiplierAnimId(r0);
    s->oam.tileNum   =
        s->sheetTileStart
        + (*s->anims + s->animCmdIndex)->type;
}

void SetBallCounterNumLeft(u8 r0)
{
    u8 i;
    u8 t = 0;
    if (eRoulette->minBet == 1)
        t = 2;
    switch (r0)
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
    u8 spriteid;
    struct SpriteSheet s;
    LZ77UnCompWram(sSpriteSheet_WheelCenter.data, gSharedMem);
    s.data = gSharedMem;
    s.size = sSpriteSheet_WheelCenter.size;
    s.tag  = sSpriteSheet_WheelCenter.tag;
    LoadSpriteSheet(&s);
    spriteid = CreateSprite(&gSpriteTemplate_83FA434, 116, 80, 81);
    gSprites[spriteid].data[0]            = eRoulette->wheelAngle;
    gSprites[spriteid].data[1]            = 0;
    gSprites[spriteid].animPaused         = TRUE;
    gSprites[spriteid].affineAnimPaused   = TRUE;
    gSprites[spriteid].coordOffsetEnabled = TRUE;
}

void SpriteCB_WheelCenter(struct Sprite *sprite)
{
    u32 t = sprite->oam.matrixNum;
    struct OamMatrix *m = gOamMatrices;
    m[t].d = eRoulette->wheelRotation.a;
    m[t].a = eRoulette->wheelRotation.a;
    m[t].b = eRoulette->wheelRotation.b;
    m[t].c = eRoulette->wheelRotation.c;
}

void CreateWheelBallSprites(void)
{
    u8 i;
    for (i = 0; i < 6; i++)
    {
        u8 spriteid =
            eRoulette->spriteIds[i] = CreateSprite(&gSpriteTemplate_83FA40C, 116, 80, 57 - i);
        if (spriteid != 64)
        {
            gSprites[eRoulette->spriteIds[i]].invisible          = TRUE;
            gSprites[eRoulette->spriteIds[i]].coordOffsetEnabled = TRUE;
        }
    }
}

void HideWheelBalls(void)
{
    u8 t = eRoulette->spriteIds[0];
    u8 i;
    for (i = 0; i < 6; i++)
    {
        u8 j;
        gSprites[t].invisible = TRUE;
        gSprites[t].callback = &SpriteCallbackDummy;
        StartSpriteAnim(&gSprites[t], 0);
        for (j = 0; j < 8; j++)
            gSprites[t].data[j] = 0;
        t++;
    }
}

s16 UpdateBallRelativeWheelAngle(struct Sprite *sprite)
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

u8 UpdateSlotBelowBall(struct Sprite *sprite)
{
    eRoulette->hitSlot = (u8)(((float)(s16)UpdateBallRelativeWheelAngle(sprite)) / 30.0f);
    return eRoulette->hitSlot;
}

s16 GetBallDistanceToSlotMidpoint(struct Sprite *sprite)
{
    s16 t = UpdateBallRelativeWheelAngle(sprite) % 30;
    u16 z;
    if (t == 14)
    {
        z = 0;
        return sprite->data[2] = z;
    }
    else if (t > 13)
    {
        z = (u16)(43 - t);
        return sprite->data[2] = z;
    }
    else
    {
        z = (u16)(14 - t);
        return sprite->data[2] = z;
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

void SpriteCB_BallLandInSlot(struct Sprite *sprite)
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

void SpriteCB_UnstickBall_ShroomishBallFall(struct Sprite *sprite)
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

void SpriteCB_UnstickBall_Shroomish(struct Sprite *sprite)
{
    float f0, f1, f2;
    const struct RouletteTable *p;
    UpdateBallPos(sprite);
    switch (sprite->data[3])
    {
    case 0:
        if (sprite->data[0] != 1)
        {
            f0 = ((float)sprite->data[7]);
            p = sRouletteTables;
            f1 = (f0 * ((float)(s32)p[eRoulette->tableId].randDistanceHigh) + (float)((s32)p[eRoulette->tableId].randDistanceLow - 1));
            f2 = (f0 / ((float)(s32)p[eRoulette->tableId].shroomish.fallSlowdown));
        }
        else
        {
            return;
        }
        break;
    case 180:
        if (sprite->data[0] != 0)
        {
            f0 = ((float)sprite->data[7]);
            p = sRouletteTables;
            f1 = (f0 * ((float)(s32)p[eRoulette->tableId].randDistanceHigh) + (float)((s32)p[eRoulette->tableId].randDistanceLow - 1));
            f2 = -(f0 / ((float)(s32)p[eRoulette->tableId].shroomish.fallSlowdown));
        }
        else
        {
            return;
        }
        break;
        default: return;
    }
    eRoulette->varA0 = eRoulette->ballDistToCenter;
    eRoulette->ballFallSpeed = f2;
    eRoulette->ballFallAccel = -((f2 + f2) / f1 + (2.0f / (f1 * f1)));
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

void SpriteCB_UnstickBall_TaillowPickUp(struct Sprite *sprite)
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

void SpriteCB_UnstickBall_Taillow(struct Sprite *sprite)
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

void SpriteCB_UnstickBall(struct Sprite *sprite)
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

void SpriteCB_RollBall_TryLandAdjacent(struct Sprite *sprite)
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
        eRoulette->ballAngleSpeed = ({float f = sRouletteTables[eRoulette->tableId].var1C; f * 2;});
        eRoulette->stuckHitSlot = eRoulette->hitSlot;
    }
    sprite->data[0] = 1;
    sprite->data[2] = sRouletteTables[eRoulette->tableId].randDistanceLow;
    eRoulette->ballFallSpeed = 0.085f;
    sprite->callback = SpriteCB_RollBall_TryLandAdjacent;
    sprite->data[1] = 5;
}

#endif

void SpriteCB_RollBall_TryLand(struct Sprite *sprite)
{
    UpdateBallPos(sprite);
    sprite->data[2] = 0;
    UpdateSlotBelowBall(sprite);
    if (!(sRouletteSlots[eRoulette->hitSlot].flag & eRoulette->hitFlags))
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
        u8 t;
        u32 z;
        m4aSongNumStart(SE_BALL_BOUNCE_1);
        if ((z = (Random() & 1)))
        {
            u32 o;
            eRoulette->ballAngleSpeed = 0.0f;
            o = (eRoulette->hitSlot + 1) % 12;
            t = o;
            eRoulette->stuckHitSlot = o;
        }
        else
        {
            eRoulette->ballAngleSpeed = ({
                float f = sRouletteTables[eRoulette->tableId].var1C;
                f * 2;
            }); // couldn't replicate loads
            t = (eRoulette->hitSlot + 11) % 12;
            eRoulette->stuckHitSlot = eRoulette->hitSlot;
        }
        if (sRouletteSlots[t].flag & eRoulette->hitFlags)
        {
            sprite->data[0] = 1;
            sprite->data[2] = sRouletteTables[eRoulette->tableId].randDistanceLow;
        }
        else
        {
            sprite->data[0] = sRouletteSlots[t].flag & eRoulette->hitFlags;
            if (eRoulette->tableId)
            {
                sprite->data[2] = sRouletteTables[eRoulette->tableId].randDistanceHigh;
            }
            else
            {
                sprite->data[2] = sRouletteTables[eRoulette->tableId].randDistanceLow;
                if (z)
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

void SpriteCB_RollBall_Slow(struct Sprite *sprite)
{
    UpdateBallPos(sprite);
    if (!(eRoulette->ballAngleSpeed > 0.5f))
    {
        UpdateSlotBelowBall(sprite);
        if (!GetBallDistanceToSlotMidpoint(sprite))
        {
            const struct RouletteTable *p;
            eRoulette->ballAngleAccel = 0.0f;
            p = sRouletteTables;
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
        sprite->data[1]     = 3;
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

void CreateShroomishSprite(struct Sprite *sprite)
{
    u16 t;
    u8 i;
    s16 s[2][2] = {
        {116, 44},
        {116, 112}
    };
    struct Roulette *p;
    t = sprite->data[7] - 2;
    p = eRoulette;
    p->spriteIds[55] = CreateSprite(&gSpriteTemplate_83FA50C, 36, -12, 50);
    p->spriteIds[56] = CreateSprite(gSpriteTemplate_83FA5C0, s[sprite->data[0]][0], s[sprite->data[0]][1], 59);
    p->spriteIds[57] = CreateSprite(&gSpriteTemplate_83FA5C0[1], 36, 140, 51);
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
        gSprites[eRoulette->spriteIds[55 + i]].data[3]          = (sprite->data[7] * sRouletteTables[eRoulette->tableId].randDistanceHigh) +
                                                                (sRouletteTables[eRoulette->tableId].randDistanceLow + 0xffff);
    }
    gSprites[eRoulette->spriteIds[56]].coordOffsetEnabled = TRUE;
    eRoulette->ball = sprite;
}

void CreateTaillowSprite(struct Sprite *sprite)
{
    u8 i = 0;
    s16 t;
    s16 s[2][2] = {
        {256, 84},
        {-16, 84}
    };
    const struct RouletteTable *p;
    t = sprite->data[7] - 2;
    eRoulette->spriteIds[55] = CreateSprite(&gSpriteTemplate_83FA524, s[sprite->data[0]][0], s[sprite->data[0]][1], 50);
    StartSpriteAnim(&gSprites[eRoulette->spriteIds[55]], sprite->data[0]);
    eRoulette->spriteIds[56] = CreateSprite(&gSpriteTemplate_83FA5F0, s[sprite->data[0]][0], s[sprite->data[0]][1], 51);
    gSprites[eRoulette->spriteIds[56]].affineAnimPaused = TRUE;
    gSprites[eRoulette->spriteIds[56]].animPaused       = TRUE;
    sprite->data[7] = (t * (p = sRouletteTables)[eRoulette->tableId].randDistanceHigh) + (p[eRoulette->tableId].taillow.baseDropDelay + 45);
    for (; i < 2; i++)
    {
        gSprites[eRoulette->spriteIds[55 + i]].data[4] = eRoulette->spriteIds[55];
        gSprites[eRoulette->spriteIds[55 + i]].data[5] = eRoulette->spriteIds[56];
        gSprites[eRoulette->spriteIds[55 + i]].data[6] = eRoulette->spriteIds[56];
        gSprites[eRoulette->spriteIds[55 + i]].data[2] = t;
        gSprites[eRoulette->spriteIds[55 + i]].data[3] = sprite->data[7] - 45;
    }
    eRoulette->ball = sprite;
}

void SetBallStuck(struct Sprite *sprite)
{
    u8 z;
    u16 o;
    u8 h = 0;
    u8 j = 5;
    u8 p = 0;
    u8 i = 0;
    u8 val;
    u8 s[10] = {};
    u16 rand = Random();

    eRoulette->ballState = 1;
    eRoulette->ballStuck = TRUE;
    eRoulette->ballUnstuck = FALSE;
    eRoulette->hitSlot = 0xFF;
    eRoulette->ballAngle = sprite->data[3];
    eRoulette->ballFallSpeed = 0.0f;
    eRoulette->ballAngleSpeed = sRouletteTables[eRoulette->tableId].var1C;

    o = (eRoulette->tableId * 30 + 33) + (0x1 - eRoulette->useTaillow) * 15;
    for (i = 0; i < 4; i++)
    {
        if (o < sprite->data[3] && sprite->data[3] <= o + 90)
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
        o += 90;
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

    val = 2;
    z = (eRoulette->stuckHitSlot + 2) % 12;

    if (eRoulette->useTaillow == 1 && eRoulette->tableId == 1)
        j += 6;
    else
        j += val;

    for (i = val; i < j; i++)
    {
        if (!(eRoulette->hitFlags & sRouletteSlots[z].flag))
        {
            s[h++] = i;
            if (p == 0 && (sRouletteSlots[z].flag & sGridSelections[eRoulette->betSelection[eRoulette->curBallNum]].inSelectionFlags))
                p = i;
        }
        z = (z + 1) % 0xC;
    }

    if ((eRoulette->useTaillow + 1) & eRoulette->partySpeciesFlags)
    {
        if (p && (rand & 0xFF) < 0xc0)
            sprite->data[7] = p;
        else
            sprite->data[7] = s[rand % h];
    }
    else
    {
        sprite->data[7] = s[rand % h];
    }

    sprite->callback = SpriteCB_UnstickBall;
}

const u16 gUnknown_083FA61E[] = {
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
            eRoulette->shroomishShadowAlpha = gUnknown_083FA61E[0];
        }
    }
}

void SpriteCB_ShroomishShakeScreen(struct Sprite *sprite)
{
    s16 t[3][4] = {
        {-1, 0, 1, 0},
        {-2, 0, 2, 0},
        {-3, 0, 3, 0}
    };
    s32 p, z;

    if (sprite->data[1]++ < sprite->data[3])
    {
        if (sprite->data[1] & 1)
        {
            gSpriteCoordOffsetY = t[sprite->data[2] / 2][sprite->data[7]];
            p = z = sprite->data[7] + 1;
            if (z < 0)
                p += 3;
            sprite->data[7] = z - ((p >> 2) * 4);
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

void SpriteCB_ShroomishFall(struct Sprite *sprite)
{
    float t;
    sprite->data[1]++;
    t = sprite->data[1];
    sprite->y2 = t * 0.039f * t;
    eRoulette->shroomishShadowAlpha = gUnknown_083FA61E[(eRoulette->shroomishShadowTimer - 1) / 2];
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

void SpriteCB_Shroomish(struct Sprite *sprite)
{
    if (sprite->data[7] == 0)
    {
        register u32 t asm("r2");
        u32 z ;
        if (eRoulette->ball->data[0] == 0)
        {
            t = eRoulette->ball->data[3];
            z = sRouletteTables[eRoulette->tableId].shroomish.startAngle;
        }
        else
        {
            t = eRoulette->ball->data[3];
            z = sRouletteTables[eRoulette->tableId].shroomish.startAngle;
            z += 180;
        }
        if (t == z)
        {
            sprite->invisible = FALSE;
            sprite->data[7]++;
            m4aSongNumStart(SE_FALL);
            eRoulette->shroomishShadowTimer = 1;
            eRoulette->shroomishShadowAlpha = gUnknown_083FA61E[0];
        }
    }
    else
    {
        u32 t, z;
        eRoulette->shroomishShadowAlpha = gUnknown_083FA61E[(eRoulette->shroomishShadowTimer - 1) / 2];
        if (eRoulette->shroomishShadowTimer < 19)
            eRoulette->shroomishShadowTimer++;
        if (eRoulette->ball->data[0] == 0)
        {
            t = eRoulette->ball->data[3];
            z = sRouletteTables[eRoulette->tableId].shroomish.dropAngle;
        }
        else
        {
            t = eRoulette->ball->data[3];
            z = sRouletteTables[eRoulette->tableId].shroomish.dropAngle;
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
        FreeOamMatrix(gSprites[eRoulette->spriteIds[56]].oam.matrixNum);
        DestroySprite(&gSprites[eRoulette->spriteIds[56]]);
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
            StartSpriteAnim(sprite, eRoulette->ball->data[0] + 4);
            sprite->callback = SpriteCB_Taillow_FlyAway;
            gSprites[sprite->data[6]].affineAnimPaused = FALSE;
        }
    }
}

void SpriteCB_Taillow_FlyIn(struct Sprite *sprite)
{
    s8 t[2] = {-1, 1}; //sign
    s8 z[8][2] = {
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
        sprite->x += t[eRoulette->ball->data[0]] * 2;
        if (IsSEPlaying())
        {
            s8 u =  -((116 - sprite->x) / 2);
            m4aMPlayPanpotControl(&gMPlayInfo_SE1, 0xffff, u);
            m4aMPlayPanpotControl(&gMPlayInfo_SE2, 0xffff, u);
        }
    }
    else
    {
        if (sprite->data[1] >= 0)
        {
            sprite->x += t[eRoulette->ball->data[0]] * z[7 - sprite->data[1]][0];
            sprite->y += z[7 - sprite->data[1]][1];
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

void SpriteCB_TaillowShadow_FlyIn(struct Sprite *sprite)
{
    s8 t[2] = {-1, 1}; //sign
    if (sprite->data[1]-- >= 0)
    {
        sprite->x += t[eRoulette->ball->data[0]] * 2;
        gSprites[sprite->data[6]].invisible ^= 1;
    }
    else
    {
        sprite->callback = SpriteCB_TaillowShadow_Flash;
    }
}

void SpriteCB_Taillow(struct Sprite *sprite)
{
    if (eRoulette->ball->data[0] == 0)
    {
        if (eRoulette->ball->data[3] == sRouletteTables[eRoulette->tableId].taillow.rightStartAngle + 90)
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
        if (eRoulette->ball->data[3] == sRouletteTables[eRoulette->tableId].taillow.leftStartAngle + 270)
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
