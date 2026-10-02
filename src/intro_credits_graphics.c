#include "global.h"
#include "gba/m4a_internal.h"
#include "intro.h"
#include "data.h"
#include "decompress.h"
#include "hall_of_fame.h"
#include "intro_credits_graphics.h"
#include "libgcnmultiboot.h"
#include "link.h"
#include "m4a.h"
#include "main.h"
#include "new_game.h"
#include "palette.h"
#include "random.h"
#include "save.h"
#include "constants/songs.h"
#include "sound.h"
#include "constants/species.h"
#include "task.h"
#include "title_screen.h"
#include "trig.h"
#include "scanline_effect.h"

// define register constants for the inline asm
asm(".include \"constants/gba_constants.inc\"\n");

struct IntroCreditsSpriteMetadata
{
    u8 animNum:4;
    u8 shape:2;
    u8 size:2;
    u8 x;
    u8 y;
    u8 subpriority;
    u16 xOff;
};

#define TAG_BICYCLE        1001
#define TAG_BRENDAN        1002
#define TAG_MAY            1003
#define TAG_LATIOS         1004
#define TAG_LATIAS         1005
#define TAG_MOVING_SCENERY 2000

static const u16 sGrass_Pal[] = INCBIN_U16("graphics/intro/scene_2/grass.gbapal");
static const u16 sGrassSunset_Pal[] = INCBIN_U16("graphics/intro/scene_2/grass_sunset.gbapal");
static const u16 sGrassNight_Pal[] = INCBIN_U16("graphics/intro/scene_2/grass_night.gbapal");
static const u8 sGrass_Gfx[] = INCBIN_U8("graphics/intro/scene_2/grass.4bpp.lz");
static const u8 sGrass_Tilemap[] = INCBIN_U8("graphics/intro/scene_2/grass_map.bin.lz");
static const u16 sCloudsBg_Pal[] = INCBIN_U16("graphics/intro/scene_2/clouds_bg.gbapal");
static const u16 sCloudsBgSunset_Pal[] = INCBIN_U16("graphics/intro/scene_2/clouds_bg_sunset.gbapal");
static const u8 sCloudsBg_Gfx[] = INCBIN_U8("graphics/intro/scene_2/clouds_bg.4bpp.lz");
static const u8 sCloudsBg_Tilemap[] = INCBIN_U8("graphics/intro/scene_2/clouds_bg_map.bin.lz");
static const u16 sClouds_Pal[] = INCBIN_U16("graphics/intro/scene_2/clouds.gbapal");
static const u16 sCloudsSunset_Pal[] = INCBIN_U16("graphics/intro/scene_2/clouds_sunset.gbapal");
static const u8 sClouds_Gfx[] = INCBIN_U8("graphics/intro/scene_2/clouds.4bpp.lz");
static const u16 sTrees_Pal[] = INCBIN_U16("graphics/intro/scene_2/trees.gbapal");
static const u16 sTreesSunset_Pal[] = INCBIN_U16("graphics/intro/scene_2/trees_sunset.gbapal");
static const u8 sTrees_Gfx[] = INCBIN_U8("graphics/intro/scene_2/trees.4bpp.lz");
static const u8 sTrees_Tilemap[] = INCBIN_U8("graphics/intro/scene_2/trees_map.bin.lz");
static const u16 sTreesSmall_Pal[] = INCBIN_U16("graphics/intro/scene_2/trees_small.gbapal");
static const u8 sTreesSmall_Gfx[] = INCBIN_U8("graphics/intro/scene_2/trees_small.4bpp.lz");
static const u16 sHouses_Pal[] = INCBIN_U16("graphics/intro/scene_2/houses.gbapal");
static const u8 sHouses_Gfx[] = INCBIN_U8("graphics/intro/scene_2/houses.4bpp.lz"); // Only used by the credits.
static const u16 sHouseSilhouette_Pal[] = INCBIN_U16("graphics/intro/scene_2/house_silhouette.gbapal");
static const u8 sHouses_Tilemap[] = INCBIN_U8("graphics/intro/scene_2/houses_map.bin.lz");
static const u8 sHouseSilhouette_Gfx[] = INCBIN_U8("graphics/intro/scene_2/house_silhouette.4bpp.lz");
static const u16 sBrendan_Pal[] = INCBIN_U16("graphics/intro/scene_2/brendan.gbapal");
static const u8 sBrendan_Gfx[] = INCBIN_U8("graphics/intro/scene_2/brendan.4bpp.lz");
static const u16 sMay_Pal[] = INCBIN_U16("graphics/intro/scene_2/may.gbapal");
static const u16 sUnused[0xF0] = {0};
static const u8 sMay_Gfx[] = INCBIN_U8("graphics/intro/scene_2/may.4bpp.lz");
static const u8 sBicycle_Gfx[] = INCBIN_U8("graphics/intro/scene_2/bicycle.4bpp.lz");
static const u16 sLatios_Pal[] = INCBIN_U16("graphics/intro/scene_2/latios.gbapal");
static const u8 sLatios_Gfx[] = INCBIN_U8("graphics/intro/scene_2/latios.4bpp.lz");
static const u16 sLatias_Pal[] = INCBIN_U16("graphics/intro/scene_2/latias.gbapal");
static const u8 sLatias_Gfx[] = INCBIN_U8("graphics/intro/scene_2/latias.4bpp.lz");

static void SpriteCB_MovingScenery(struct Sprite *sprite);
static void SpriteCB_Player(struct Sprite *sprite);
static void SpriteCB_Bicycle(struct Sprite *sprite);
static void SpriteCB_LatiLeftHalf(struct Sprite *sprite);
static void Task_BicycleBgAnimation(u8 taskId);

static const struct SpriteTemplate sSpriteTemplate_MovingScenery = {
    TAG_MOVING_SCENERY, TAG_NONE, &gDummyOamData, gDummySpriteAnimTable, NULL, gDummySpriteAffineAnimTable, SpriteCB_MovingScenery
};

static const struct CompressedSpriteSheet sSpriteSheet_Clouds[] = {
    { sClouds_Gfx, 0x400, TAG_MOVING_SCENERY },
    {}
};

static const union AnimCmd sAnim_Cloud_Largest[] = {
    ANIMCMD_FRAME( 0, 30),
    ANIMCMD_END
};

static const union AnimCmd sAnim_Cloud_Large[] = {
    ANIMCMD_FRAME(16, 30),
    ANIMCMD_END
};

static const union AnimCmd sAnim_Cloud_Small[] = {
    ANIMCMD_FRAME(20, 30),
    ANIMCMD_END
};

static const union AnimCmd sAnim_Cloud_Smallest[] = {
    ANIMCMD_FRAME(22, 30),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_Clouds[] = {
    sAnim_Cloud_Largest,
    sAnim_Cloud_Large,
    sAnim_Cloud_Small,
    sAnim_Cloud_Smallest
};

static const struct IntroCreditsSpriteMetadata sSpriteMetadata_Clouds[] = {
    {  0, ST_OAM_SQUARE,      2,   72, 32, 100, 0xc00 },
    {  0, ST_OAM_SQUARE,      2,  158, 32, 100, 0xc00 },
    {  1, ST_OAM_SQUARE,      1,  192, 40, 101, 0x800 },
    {  1, ST_OAM_SQUARE,      1,   56, 40, 101, 0x800 },
    {  2, ST_OAM_H_RECTANGLE, 0,  100, 44, 102, 0x400 },
    {  2, ST_OAM_H_RECTANGLE, 0,  152, 44, 102, 0x400 },
    {  3, ST_OAM_H_RECTANGLE, 0,    8, 46, 103, 0x100 },
    {  3, ST_OAM_H_RECTANGLE, 0,   56, 46, 103, 0x100 },
    {  3, ST_OAM_H_RECTANGLE, 0,  240, 46, 103, 0x100 },
};

static const struct CompressedSpriteSheet sSpriteSheet_TreesSmall[] = {
    { sTreesSmall_Gfx, 0x400, TAG_MOVING_SCENERY },
    {}
};

static const union AnimCmd sAnim_Trees_0[] = {
    ANIMCMD_FRAME( 0, 30),
    ANIMCMD_END
};

static const union AnimCmd sAnim_Trees_1[] = {
    ANIMCMD_FRAME(16, 30),
    ANIMCMD_END
};

static const union AnimCmd sAnim_Trees_2[] = {
    ANIMCMD_FRAME(24, 30),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_Trees[] = {
    sAnim_Trees_0,
    sAnim_Trees_1,
    sAnim_Trees_2
};

static const struct IntroCreditsSpriteMetadata sSpriteMetadata_Trees[] = {
    {  0, ST_OAM_SQUARE,      2,   16, 88, 100, 0x2000 },
    {  0, ST_OAM_SQUARE,      2,   80, 88, 100, 0x2000 },
    {  0, ST_OAM_SQUARE,      2,  144, 88, 100, 0x2000 },
    {  0, ST_OAM_SQUARE,      2,  208, 88, 100, 0x2000 },
    {  1, ST_OAM_V_RECTANGLE, 2,   40, 88, 101, 0x1000 },
    {  1, ST_OAM_V_RECTANGLE, 2,  104, 88, 101, 0x1000 },
    {  1, ST_OAM_V_RECTANGLE, 2,  168, 88, 101, 0x1000 },
    {  1, ST_OAM_V_RECTANGLE, 2,  232, 88, 101, 0x1000 },
    {  2, ST_OAM_V_RECTANGLE, 2,   56, 88, 102, 0x800  },
    {  2, ST_OAM_V_RECTANGLE, 2,  120, 88, 102, 0x800  },
    {  2, ST_OAM_V_RECTANGLE, 2,  184, 88, 102, 0x800  },
    {  2, ST_OAM_V_RECTANGLE, 2,  248, 88, 102, 0x800  },
};

static const struct CompressedSpriteSheet sSpriteSheet_HouseSilhouette[] = {
    { sHouseSilhouette_Gfx, 0x400, TAG_MOVING_SCENERY },
    {}
};

static const union AnimCmd sAnim_HouseSilhouette[] = {
    ANIMCMD_FRAME(0, 30),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_HouseSilhouette[] = {
    sAnim_HouseSilhouette
};

static const struct IntroCreditsSpriteMetadata sSpriteMetadata_HouseSilhouette[] = {
    { 0, ST_OAM_SQUARE, 2,   24, 88, 100, 0x1000 },
    { 0, ST_OAM_SQUARE, 2,   64, 88, 100, 0x1000 },
    { 0, ST_OAM_SQUARE, 2,  104, 88, 100, 0x1000 },
    { 0, ST_OAM_SQUARE, 2,  144, 88, 100, 0x1000 },
    { 0, ST_OAM_SQUARE, 2,  184, 88, 100, 0x1000 },
    { 0, ST_OAM_SQUARE, 2,  224, 88, 100, 0x1000 },
};

static const struct OamData sOamData_Player = {
    .y = 160, .shape = ST_OAM_SQUARE, .size = 3, .priority = 1
};

static const union AnimCmd sAnim_Player[] = {
    ANIMCMD_FRAME(  0, 8),
    ANIMCMD_FRAME( 64, 8),
    ANIMCMD_FRAME(128, 8),
    ANIMCMD_FRAME(192, 8),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd *const sAnims_Player[] = {
    sAnim_Player
};

static const struct SpriteTemplate sSpriteTemplate_Brendan = {
    TAG_BRENDAN, TAG_BRENDAN, &sOamData_Player, sAnims_Player, NULL, gDummySpriteAffineAnimTable, SpriteCB_Player
};

static const struct SpriteTemplate sSpriteTemplate_May = {
    TAG_MAY, TAG_MAY, &sOamData_Player, sAnims_Player, NULL, gDummySpriteAffineAnimTable, SpriteCB_Player
};

static const struct OamData sOamData_Bicycle = {
    .y = 160, .shape = ST_OAM_H_RECTANGLE, .size = 3, .priority = 1
};

static const union AnimCmd sAnim_Bicycle[] = {
    ANIMCMD_FRAME(  0, 8),
    ANIMCMD_FRAME( 32, 8),
    ANIMCMD_FRAME( 64, 8),
    ANIMCMD_FRAME( 96, 8),
    ANIMCMD_JUMP(0)
};

static const union AnimCmd *const sAnims_Bicycle[] = {
    sAnim_Bicycle
};

static const struct SpriteTemplate sSpriteTemplate_BrendanBicycle = {
    TAG_BICYCLE, TAG_BRENDAN, &sOamData_Bicycle, sAnims_Bicycle, NULL, gDummySpriteAffineAnimTable, SpriteCB_Bicycle
};

static const struct SpriteTemplate sSpriteTemplate_MayBicycle = {
    TAG_BICYCLE, TAG_MAY, &sOamData_Bicycle, sAnims_Bicycle, NULL, gDummySpriteAffineAnimTable, SpriteCB_Bicycle
};

static const struct OamData sOamData_Lati = {
    .y = 160, .shape = ST_OAM_SQUARE, .size = 3, .priority = 1
};

static const union AnimCmd sAnim_LatiLeftHalf[] = {
    ANIMCMD_FRAME(  0, 16),
    ANIMCMD_END
};

static const union AnimCmd sAnim_LatiRightHalf[] = {
    ANIMCMD_FRAME( 64, 16),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_Lati[] = {
    sAnim_LatiLeftHalf,
    sAnim_LatiRightHalf
};

static const struct SpriteTemplate sSpriteTemplate_Latios = {
    TAG_LATIOS, TAG_LATIOS, &sOamData_Lati, sAnims_Lati, NULL, gDummySpriteAffineAnimTable, SpriteCB_LatiLeftHalf
};

static const struct SpriteTemplate sSpriteTemplate_Latias = {
    TAG_LATIAS, TAG_LATIAS, &sOamData_Lati, sAnims_Lati, NULL, gDummySpriteAffineAnimTable, SpriteCB_LatiLeftHalf
};

const struct CompressedSpriteSheet gSpriteSheet_IntroBrendan[] = {
    { sBrendan_Gfx, 0x3800, TAG_BRENDAN },
    {}
};
const struct CompressedSpriteSheet gSpriteSheet_IntroMay[] = {
    { sMay_Gfx, 0x3800, TAG_MAY },
    {}
};
const struct CompressedSpriteSheet gSpriteSheet_IntroBicycle[] = {
    { sBicycle_Gfx, 0x1000, TAG_BICYCLE },
    {}
};
const struct CompressedSpriteSheet gSpriteSheet_IntroLatios[] = {
    { sLatios_Gfx, 0x1000, TAG_LATIOS },
    {}
};
const struct CompressedSpriteSheet gSpriteSheet_IntroLatias[] = {
    { sLatias_Gfx, 0x1000, TAG_LATIAS },
    {}
};

const struct SpritePalette gSpritePalettes_IntroPlayerLati[] = {
    { sBrendan_Pal, TAG_BRENDAN },
    { sMay_Pal,     TAG_MAY },
    { sLatios_Pal,  TAG_LATIOS },
    { sLatias_Pal,  TAG_LATIAS },
    {}
};

const struct CompressedSpriteSheet gSpriteSheet_CreditsRivalBrendan[] = {
    { sBrendan_Gfx, 0x2000, TAG_BRENDAN },
    {}
};

const struct CompressedSpriteSheet gSpriteSheet_CreditsRivalMay[] = {
    { sMay_Gfx, 0x2000, TAG_MAY },
    {}
};


EWRAM_DATA u16 gIntroCredits_MovingSceneryVBase = 0;
EWRAM_DATA s16 gIntroCredits_MovingSceneryVOffset = 0;
EWRAM_DATA s16 gIntroCredits_MovingSceneryState = INTROCRED_SCENERY_NORMAL;

extern u8 gReservedSpritePaletteCount;

static void CreateCloudSprites(void);
static void CreateTreeSprites(void);

void LoadIntroPart2Graphics(u8 scenery)
{
    LZ77UnCompVram(&sGrass_Gfx, (void *)(VRAM + 0x4000));
    LZ77UnCompVram(&sGrass_Tilemap, (void *)(VRAM + 0x7800));
    LoadPalette(&sGrass_Pal, 240, 32);
    switch (scenery)
    {
    case 0:
    default:
        LZ77UnCompVram(&sCloudsBg_Gfx, (void *)(VRAM));
        LZ77UnCompVram(&sCloudsBg_Tilemap, (void *)(VRAM + 0x3000));
        LoadPalette(&sCloudsBg_Pal, 0, 96);
        LoadCompressedSpriteSheet(sSpriteSheet_Clouds);
        LoadPalette(&sClouds_Pal, 256, 32);
        CreateCloudSprites();
        break;
    case 1:
        LZ77UnCompVram(&sTrees_Gfx, (void *)(VRAM));
        LZ77UnCompVram(&sTrees_Tilemap, (void *)(VRAM + 0x3000));
        LoadPalette(&sTrees_Pal, 0, 32);
        LoadCompressedSpriteSheet(sSpriteSheet_TreesSmall);
        LoadPalette(&sTreesSmall_Pal, 256, 32);
        CreateTreeSprites();
        break;
    }
    gIntroCredits_MovingSceneryState = INTROCRED_SCENERY_NORMAL;
    gReservedSpritePaletteCount = 8;
}

void SetIntroPart2BgCnt(u8 scenery)
{
    if (scenery == 1)
    {
        REG_BG3CNT = 0x603;
        REG_BG2CNT = 0x702;
        REG_BG1CNT = 0xF05;
        REG_DISPCNT = 0x1E40;
    }
    else
    {
        REG_BG3CNT = 0x603;
        REG_BG2CNT = 0x702;
        REG_BG1CNT = 0xF05;
        REG_DISPCNT = 0x1E40;
    }
}

static void CreateHouseSprites(void);

void LoadCreditsSceneGraphics(u8 scene)
{
    LZ77UnCompVram(&sGrass_Gfx, (void *)(VRAM + 0x4000));
    LZ77UnCompVram(&sGrass_Tilemap, (void *)(VRAM + 0x7800));
    switch (scene)
    {
    case SCENE_OCEAN_MORNING:
    default:
        LoadPalette(&sGrass_Pal, 240, 32);
        LZ77UnCompVram(&sCloudsBg_Gfx, (void *)(VRAM));
        LZ77UnCompVram(&sCloudsBg_Tilemap, (void *)(VRAM + 0x3000));
        LoadPalette(&sCloudsBg_Pal, 0, 96);
        LoadCompressedSpriteSheet(sSpriteSheet_Clouds);
        LZ77UnCompVram(&sClouds_Gfx, (void *)(VRAM + 0x10000));
        LoadPalette(&sClouds_Pal, 256, 32);
        CreateCloudSprites();
        break;
    case SCENE_OCEAN_SUNSET:
        LoadPalette(&sGrassSunset_Pal, 240, 32);
        LZ77UnCompVram(&sCloudsBg_Gfx, (void *)(VRAM));
        LZ77UnCompVram(&sCloudsBg_Tilemap, (void *)(VRAM + 0x3000));
        LoadPalette(&sCloudsBgSunset_Pal, 0, 96);
        LoadCompressedSpriteSheet(sSpriteSheet_Clouds);
        LZ77UnCompVram(&sClouds_Gfx, (void *)(VRAM + 0x10000));
        LoadPalette(&sCloudsSunset_Pal, 256, 32);
        CreateCloudSprites();
        break;
    case SCENE_FOREST_RIVAL_ARRIVE:
    case SCENE_FOREST_CATCH_RIVAL:
        LoadPalette(&sGrassSunset_Pal, 240, 32);
        LZ77UnCompVram(&sTrees_Gfx, (void *)(VRAM));
        LZ77UnCompVram(&sTrees_Tilemap, (void *)(VRAM + 0x3000));
        LoadPalette(&sTreesSunset_Pal, 0, 32);
        LoadCompressedSpriteSheet(sSpriteSheet_TreesSmall);
        LoadPalette(&sTreesSunset_Pal, 256, 32);
        CreateTreeSprites();
        break;
    case SCENE_CITY_NIGHT:
        LoadPalette(&sGrassNight_Pal, 240, 32);
        LZ77UnCompVram(&sHouses_Gfx, (void *)(VRAM));
        LZ77UnCompVram(&sHouses_Tilemap, (void *)(VRAM + 0x3000));
        LoadPalette(&sHouses_Pal, 0, 64);
        LoadCompressedSpriteSheet(sSpriteSheet_HouseSilhouette);
        LoadPalette(&sHouseSilhouette_Pal, 256, 32);
        CreateHouseSprites();
        break;
    }
    gReservedSpritePaletteCount = 8;
    gIntroCredits_MovingSceneryState = INTROCRED_SCENERY_NORMAL;
}

void SetCreditsSceneBgCnt(u8 scene)
{
    REG_BG3CNT = 0x603;
    REG_BG2CNT = 0x702;
    REG_BG1CNT = 0xF05;
    REG_DISPCNT = 0x1F40;
}

#define tMode     data[0]
#define tBg1Speed data[1]
#define tBg1PosHi data[2]
#define tBg1PosLo data[3]
#define tBg2Speed data[4]
#define tBg2PosHi data[5]
#define tBg2PosLo data[6]
#define tBg3Speed data[7]
#define tBg3PosHi data[8]
#define tBg3PosLo data[9]

u8 CreateBicycleBgAnimationTask(u8 mode, u16 bg1Speed, u16 bg2Speed, u16 bg3Speed)
{
    u8 taskId = CreateTask(&Task_BicycleBgAnimation, 0);

    gTasks[taskId].tMode = mode;
    gTasks[taskId].tBg1Speed = bg1Speed;
    gTasks[taskId].tBg1PosHi = 0;
    gTasks[taskId].tBg1PosLo = 0;
    gTasks[taskId].tBg2Speed = bg2Speed;
    gTasks[taskId].tBg2PosHi = 0;
    gTasks[taskId].tBg2PosLo = 0;
    gTasks[taskId].tBg3Speed = bg3Speed;
    gTasks[taskId].tBg3PosHi = 8;
    gTasks[taskId].tBg3PosLo = 0;
    Task_BicycleBgAnimation(taskId);
    return taskId;
}

static void Task_BicycleBgAnimation(u8 taskId)
{
    s16 bg1Speed;
    s16 bg2Speed;
    s16 bg3Speed;
    s32 offset;

    bg1Speed = gTasks[taskId].tBg1Speed;
    if (bg1Speed != 0)
    {
        offset = (gTasks[taskId].tBg1PosHi << 16) + (u16)gTasks[taskId].tBg1PosLo;
        offset -= 16 * (u16)bg1Speed;
        gTasks[taskId].tBg1PosHi = offset >> 16;
        gTasks[taskId].tBg1PosLo = offset;
        REG_BG1HOFS = gTasks[taskId].tBg1PosHi;
        REG_BG1VOFS = gIntroCredits_MovingSceneryVBase + gIntroCredits_MovingSceneryVOffset;
    }

    bg2Speed = gTasks[taskId].tBg2Speed;
    if (bg2Speed != 0)
    {
        offset = (gTasks[taskId].tBg2PosHi << 16) + (u16)gTasks[taskId].tBg2PosLo;
        offset -= 16 * (u16)bg2Speed;
        gTasks[taskId].tBg2PosHi = offset >> 16;
        gTasks[taskId].tBg2PosLo = offset;
        REG_BG2HOFS = gTasks[taskId].tBg2PosHi;
        if (gTasks[taskId].tMode != 0)
            REG_BG2VOFS = gIntroCredits_MovingSceneryVBase + gIntroCredits_MovingSceneryVOffset;
        else
            REG_BG2VOFS = gIntroCredits_MovingSceneryVBase;
    }

    bg3Speed = gTasks[taskId].tBg3Speed;
    if (bg3Speed != 0)
    {
        offset = (gTasks[taskId].tBg3PosHi << 16) + (u16)gTasks[taskId].tBg3PosLo;
        offset -= 16 * (u16)bg3Speed;
        gTasks[taskId].tBg3PosHi = offset >> 16;
        gTasks[taskId].tBg3PosLo = offset;
        REG_BG3HOFS = gTasks[taskId].tBg3PosHi;
        REG_BG3VOFS = gIntroCredits_MovingSceneryVBase;
    }
}

#undef tMode
#undef tBg1Speed
#undef tBg1PosHi
#undef tBg1PosLo
#undef tBg2Speed
#undef tBg2PosHi
#undef tBg2PosLo
#undef tBg3Speed
#undef tBg3PosHi
#undef tBg3PosLo

void CycleSceneryPalette(u8 mode)
{
    u16 x;
    u16 y;
    switch (mode)
    {
        case 0:
        default:
            /* stuff */
            if (gMain.vblankCounter1 & 3 || gPaletteFade.active)
                break;
            if (gMain.vblankCounter1 & 4)
            {
                x = gPlttBufferUnfaded[9];
                y = gPlttBufferUnfaded[10];
            }
            else
            {
                x = gPlttBufferUnfaded[10];
                y = gPlttBufferUnfaded[9];
            }
            LoadPalette(&x, 9, 2);
            LoadPalette(&y, 10, 2);
            break;
        case 2:
            if (gMain.vblankCounter1 & 3 || gPaletteFade.active)
                break;
            if (gMain.vblankCounter1 & 4)
            {
                x = 0x3D27;
                y = 0x295;
            }
            else
            {
                x = 0x31C;
                y = 0x3D27;
            }
            LoadPalette(&x, 12, 2);
            LoadPalette(&y, 13, 2);
            break;
        case 1:
            break;
    }
}

#define tHasVerticalMove data[0]
#define tXOffset         data[1]
#define tXPos            data[2]

static void SpriteCB_MovingScenery(struct Sprite *sprite)
{
    if (gIntroCredits_MovingSceneryState != INTROCRED_SCENERY_NORMAL)
    {
        DestroySprite(sprite);
    }
    else
    {
        s32 x = ((sprite->x << 16) | (u16)sprite->tXPos) + (u16)sprite->tXOffset;

        sprite->x = x >> 16;
        sprite->tXPos = x;
        if (sprite->x > 255)
            sprite->x = 0xFFE0;
        if (sprite->tHasVerticalMove)
        {
            sprite->y2 = -(gIntroCredits_MovingSceneryVBase + gIntroCredits_MovingSceneryVOffset);
        }
        else
        {
            sprite->y2 = -gIntroCredits_MovingSceneryVBase;
        }
    }
}

static void CreateMovingScenerySprites(bool8 hasVerticalMove, const struct IntroCreditsSpriteMetadata *metadata, const union AnimCmd *const *anims, u8 numSprites)
{
    u8 i;

    for (i = 0; i < numSprites; i++)
    {
        u8 spriteId = CreateSprite(&sSpriteTemplate_MovingScenery, metadata[i].x, metadata[i].y, metadata[i].subpriority);

        CalcCenterToCornerVec(&gSprites[spriteId], metadata[i].shape, metadata[i].size, 0);
        gSprites[spriteId].oam.priority = 3;
        gSprites[spriteId].oam.shape = metadata[i].shape;
        gSprites[spriteId].oam.size = metadata[i].size;
        gSprites[spriteId].oam.paletteNum = 0;
        gSprites[spriteId].anims = anims;
        StartSpriteAnim(&gSprites[spriteId], metadata[i].animNum);
        gSprites[spriteId].tHasVerticalMove = hasVerticalMove;
        gSprites[spriteId].tXOffset = metadata[i].xOff;
        gSprites[spriteId].tXPos = 0;
    }
}

static void CreateCloudSprites(void)
{
    CreateMovingScenerySprites(FALSE, sSpriteMetadata_Clouds, sAnims_Clouds, 9);
}

static void CreateTreeSprites(void)
{
    CreateMovingScenerySprites(TRUE, sSpriteMetadata_Trees, sAnims_Trees, 12);
}

static void CreateHouseSprites(void)
{
    CreateMovingScenerySprites(TRUE, sSpriteMetadata_HouseSilhouette, sAnims_HouseSilhouette, 6);
}

#undef tHasVerticalMove
#undef tXOffset
#undef tXPos

static void SpriteCB_Player(struct Sprite *sprite)
{
}

#define sPlayerSpriteId data[0]

static void SpriteCB_Bicycle(struct Sprite *sprite)
{
    sprite->invisible = gSprites[sprite->sPlayerSpriteId].invisible;
    sprite->x = gSprites[sprite->sPlayerSpriteId].x;
    sprite->y = gSprites[sprite->sPlayerSpriteId].y + 8;
    sprite->x2 = gSprites[sprite->sPlayerSpriteId].x2;
    sprite->y2 = gSprites[sprite->sPlayerSpriteId].y2;
}

u8 CreateIntroBrendanSprite(s16 x, s16 y)
{
    u8 playerSpriteId = CreateSprite(&sSpriteTemplate_Brendan, x, y, 0);
    u8 bicycleSpriteId = CreateSprite(&sSpriteTemplate_BrendanBicycle, x, y + 8, 1);

    gSprites[bicycleSpriteId].sPlayerSpriteId = playerSpriteId;
    return playerSpriteId;
}

u8 CreateIntroMaySprite(s16 x, s16 y)
{
    u8 playerSpriteId = CreateSprite(&sSpriteTemplate_May, x, y, 0);
    u8 bicycleSpriteId = CreateSprite(&sSpriteTemplate_MayBicycle, x, y + 8, 1);

    gSprites[bicycleSpriteId].sPlayerSpriteId = playerSpriteId;
    return playerSpriteId;
}

#undef sPlayerSpriteId

static void SpriteCB_LatiLeftHalf(struct Sprite *sprite)
{
}

#define sLeftSpriteId data[0]

static void SpriteCB_LatiRightHalf(struct Sprite *sprite)
{
    sprite->invisible = gSprites[sprite->sLeftSpriteId].invisible;
    sprite->y = gSprites[sprite->sLeftSpriteId].y;
    sprite->x2 = gSprites[sprite->sLeftSpriteId].x2;
    sprite->y2 = gSprites[sprite->sLeftSpriteId].y2;
}

u8 CreateIntroLatiosSprite(s16 x, s16 y)
{
    u8 leftSpriteId = CreateSprite(&sSpriteTemplate_Latios, x - 32, y, 2);
    u8 rightSpriteId = CreateSprite(&sSpriteTemplate_Latios, x + 32, y, 2);

    gSprites[rightSpriteId].sLeftSpriteId = leftSpriteId;
    StartSpriteAnim(&gSprites[rightSpriteId], 1);
    gSprites[rightSpriteId].callback = SpriteCB_LatiRightHalf;
    return leftSpriteId;
}

u8 CreateIntroLatiasSprite(s16 x, s16 y)
{
    u8 leftSpriteId = CreateSprite(&sSpriteTemplate_Latias, x - 32, y, 2);
    u8 rightSpriteId = CreateSprite(&sSpriteTemplate_Latias, x + 32, y, 2);

    gSprites[rightSpriteId].sLeftSpriteId = leftSpriteId;
    StartSpriteAnim(&gSprites[rightSpriteId], 1);
    gSprites[rightSpriteId].callback = SpriteCB_LatiRightHalf;
    return leftSpriteId;
}

#undef sLeftSpriteId
