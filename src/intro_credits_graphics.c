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
#include "sprite.h"

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
static const u8 sHouses_Gfx[] = INCBIN_U8("graphics/intro/scene_2/houses.4bpp.lz"); // only used in credits, coupled with intro because bicycle sequence
static const u16 sHouseSilhouette_Pal[] = INCBIN_U16("graphics/intro/scene_2/house_silhouette.gbapal");
static const u8 sHouses_Tilemap[] = INCBIN_U8("graphics/intro/scene_2/houses_map.bin.lz");
static const u8 sHouseSilhouette_Gfx[] = INCBIN_U8("graphics/intro/scene_2/house_silhouette.4bpp.lz");
static const u16 sBrendanCredits_Pal[] = INCBIN_U16("graphics/intro/scene_2/brendan_credits.gbapal");
static const u8 sBrendanCredits_Gfx[] = INCBIN_U8("graphics/intro/scene_2/brendan_credits.4bpp.lz");
static const u16 sMayCredits_Pal[] = INCBIN_U16("graphics/intro/scene_2/may_credits.gbapal");
static const u16 sUnused[0xF0] = {0};
static const u8 sMayCredits_Gfx[] = INCBIN_U8("graphics/intro/scene_2/may_credits.4bpp.lz");
static const u8 sBicycle_Gfx[] = INCBIN_U8("graphics/intro/scene_2/bicycle.4bpp.lz");
static const u16 sLatios_Pal[] = INCBIN_U16("graphics/intro/scene_2/latios.gbapal");
static const u8 sLatios_Gfx[] = INCBIN_U8("graphics/intro/scene_2/latios.4bpp.lz");
static const u16 sLatias_Pal[] = INCBIN_U16("graphics/intro/scene_2/latias.gbapal");
static const u8 sLatias_Gfx[] = INCBIN_U8("graphics/intro/scene_2/latias.4bpp.lz");

static void SpriteCB_MovingScenery(struct Sprite *sprite);
static void SpriteCB_Player(struct Sprite *sprite);
static void SpriteCB_Bicycle(struct Sprite *sprite);
void nullsub_83(struct Sprite *sprite);
static void Task_BicycleBgAnimation(u8 taskId);

static const struct SpriteTemplate sSpriteTemplate_MovingScenery = {
    2000, 0xFFFF, &gDummyOamData, gDummySpriteAnimTable, NULL, gDummySpriteAffineAnimTable, SpriteCB_MovingScenery
};

static const struct CompressedSpriteSheet sSpriteSheet_Clouds[] = {
    { sClouds_Gfx, 0x400, 2000 },
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
    { sTreesSmall_Gfx, 0x400, 2000 },
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
    { sHouseSilhouette_Gfx, 0x400, 2000 },
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
    1002, 1002, &sOamData_Player, sAnims_Player, NULL, gDummySpriteAffineAnimTable, SpriteCB_Player
};

static const struct SpriteTemplate sSpriteTemplate_May = {
    1003, 1003, &sOamData_Player, sAnims_Player, NULL, gDummySpriteAffineAnimTable, SpriteCB_Player
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
    1001, 1002, &sOamData_Bicycle, sAnims_Bicycle, NULL, gDummySpriteAffineAnimTable, SpriteCB_Bicycle
};

static const struct SpriteTemplate sSpriteTemplate_MayBicycle = {
    1001, 1003, &sOamData_Bicycle, sAnims_Bicycle, NULL, gDummySpriteAffineAnimTable, SpriteCB_Bicycle
};

const struct OamData gOamData_8416D5C = {
    .y = 160, .shape = ST_OAM_SQUARE, .size = 3, .priority = 1
};

const union AnimCmd gSpriteAnim_8416D64[] = {
    ANIMCMD_FRAME(  0, 16),
    ANIMCMD_END
};

const union AnimCmd gSpriteAnim_8416D6C[] = {
    ANIMCMD_FRAME( 64, 16),
    ANIMCMD_END
};

const union AnimCmd *const gSpriteAnimTable_8416D74[] = {
    gSpriteAnim_8416D64,
    gSpriteAnim_8416D6C
};

const struct SpriteTemplate gSpriteTemplate_8416D7C = {
    1004, 1004, &gOamData_8416D5C, gSpriteAnimTable_8416D74, NULL, gDummySpriteAffineAnimTable, nullsub_83
};

const struct SpriteTemplate gSpriteTemplate_8416D94 = {
    1005, 1005, &gOamData_8416D5C, gSpriteAnimTable_8416D74, NULL, gDummySpriteAffineAnimTable, nullsub_83
};

const struct CompressedSpriteSheet gSpriteSheet_CreditsBrendan[] = {
    { sBrendanCredits_Gfx, 0x3800, 1002 },
    {}
};
const struct CompressedSpriteSheet gSpriteSheet_CreditsMay[] = {
    { sMayCredits_Gfx, 0x3800, 1003 },
    {}
};
const struct CompressedSpriteSheet gSpriteSheet_IntroBicycle[] = {
    { sBicycle_Gfx, 0x1000, 1001 },
    {}
};
const struct CompressedSpriteSheet gIntro2LatiosSpriteSheet[] = {
    { sLatios_Gfx, 0x1000, 1004 },
    {}
};
const struct CompressedSpriteSheet gIntro2LatiasSpriteSheet[] = {
    { sLatias_Gfx, 0x1000, 1005 },
    {}
};

const struct SpritePalette gSpritePalettes_Credits[] = {
    {sBrendanCredits_Pal, 1002},
    {sMayCredits_Pal,     1003},
    {sLatios_Pal,  1004},
    {sLatias_Pal,  1005},
    {}
};

const struct CompressedSpriteSheet gSpriteSheet_CreditsRivalBrendan[] = {
    { sBrendanCredits_Gfx, 0x2000, 1002},
    {}
};

const struct CompressedSpriteSheet gSpriteSheet_CreditsRivalMay[] = {
    { sMayCredits_Gfx, 0x2000, 1003},
    {}
};


EWRAM_DATA u16 gIntroCredits_MovingSceneryVBase = 0;
EWRAM_DATA s16 gIntroCredits_MovingSceneryVOffset = 0;
EWRAM_DATA s16 gIntroCredits_MovingSceneryState = 0;


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
    gIntroCredits_MovingSceneryState = 0;
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
    case 0:
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
    case 1:
        LoadPalette(&sGrassSunset_Pal, 240, 32);
        LZ77UnCompVram(&sCloudsBg_Gfx, (void *)(VRAM));
        LZ77UnCompVram(&sCloudsBg_Tilemap, (void *)(VRAM + 0x3000));
        LoadPalette(&sCloudsBgSunset_Pal, 0, 96);
        LoadCompressedSpriteSheet(sSpriteSheet_Clouds);
        LZ77UnCompVram(&sClouds_Gfx, (void *)(VRAM + 0x10000));
        LoadPalette(&sCloudsSunset_Pal, 256, 32);
        CreateCloudSprites();
        break;
    case 2:
    case 3:
        LoadPalette(&sGrassSunset_Pal, 240, 32);
        LZ77UnCompVram(&sTrees_Gfx, (void *)(VRAM));
        LZ77UnCompVram(&sTrees_Tilemap, (void *)(VRAM + 0x3000));
        LoadPalette(&sTreesSunset_Pal, 0, 32);
        LoadCompressedSpriteSheet(sSpriteSheet_TreesSmall);
        LoadPalette(&sTreesSunset_Pal, 256, 32);
        CreateTreeSprites();
        break;
    case 4:
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
    gIntroCredits_MovingSceneryState = 0;
}

void SetCreditsSceneBgCnt(u8 scene)
{
    REG_BG3CNT = 0x603;
    REG_BG2CNT = 0x702;
    REG_BG1CNT = 0xF05;
    REG_DISPCNT = 0x1F40;
}

u8 CreateBicycleBgAnimationTask(u8 mode, u16 bg1Speed, u16 bg2Speed, u16 bg3Speed)
{
    u8 taskId = CreateTask(&Task_BicycleBgAnimation, 0);

    gTasks[taskId].data[0] = mode;
    gTasks[taskId].data[1] = bg1Speed;
    gTasks[taskId].data[2] = 0;
    gTasks[taskId].data[3] = 0;
    gTasks[taskId].data[4] = bg2Speed;
    gTasks[taskId].data[5] = 0;
    gTasks[taskId].data[6] = 0;
    gTasks[taskId].data[7] = bg3Speed;
    gTasks[taskId].data[8] = 8;
    gTasks[taskId].data[9] = 0;
    Task_BicycleBgAnimation(taskId);
    return taskId;
}

static void Task_BicycleBgAnimation(u8 taskId)
{
    s16 bg1Speed;
    s16 bg2Speed;
    s16 bg3Speed;
    s32 offset;

    bg1Speed = gTasks[taskId].data[1];
    if (bg1Speed != 0)
    {
        offset = (gTasks[taskId].data[2] << 16) + (u16)gTasks[taskId].data[3];
        offset -= 16 * (u16)bg1Speed;
        gTasks[taskId].data[2] = offset >> 16;
        gTasks[taskId].data[3] = offset;
        REG_BG1HOFS = gTasks[taskId].data[2];
        REG_BG1VOFS = gIntroCredits_MovingSceneryVBase + gIntroCredits_MovingSceneryVOffset;
    }

    bg2Speed = gTasks[taskId].data[4];
    if (bg2Speed != 0)
    {
        offset = (gTasks[taskId].data[5] << 16) + (u16)gTasks[taskId].data[6];
        offset -= 16 * (u16)bg2Speed;
        gTasks[taskId].data[5] = offset >> 16;
        gTasks[taskId].data[6] = offset;
        REG_BG2HOFS = gTasks[taskId].data[5];
        if (gTasks[taskId].data[0] != 0)
            REG_BG2VOFS = gIntroCredits_MovingSceneryVBase + gIntroCredits_MovingSceneryVOffset;
        else
            REG_BG2VOFS = gIntroCredits_MovingSceneryVBase;
    }

    bg3Speed = gTasks[taskId].data[7];
    if (bg3Speed != 0)
    {
        offset = (gTasks[taskId].data[8] << 16) + (u16)gTasks[taskId].data[9];
        offset -= 16 * (u16)bg3Speed;
        gTasks[taskId].data[8] = offset >> 16;
        gTasks[taskId].data[9] = offset;
        REG_BG3HOFS = gTasks[taskId].data[8];
        REG_BG3VOFS = gIntroCredits_MovingSceneryVBase;
    }
}

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

static void SpriteCB_MovingScenery(struct Sprite *sprite)
{
    if (gIntroCredits_MovingSceneryState)
    {
        DestroySprite(sprite);
    }
    else
    {
        s32 x = ((sprite->x << 16) | (u16)sprite->data[2]) + (u16)sprite->data[1];
        sprite->x = x >> 16;
        sprite->data[2] = x;
        if (sprite->x > 255) sprite->x = 0xFFE0;
        if (sprite->data[0])
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

    for(i = 0; i < numSprites; i++)
    {
        u8 sprite = CreateSprite(&sSpriteTemplate_MovingScenery, metadata[i].x, metadata[i].y, metadata[i].subpriority);
        CalcCenterToCornerVec(&gSprites[sprite], metadata[i].shape, metadata[i].size, 0);
        gSprites[sprite].oam.priority = 3;
        gSprites[sprite].oam.shape = metadata[i].shape;
        gSprites[sprite].oam.size = metadata[i].size;
        gSprites[sprite].oam.paletteNum = 0;
        gSprites[sprite].anims = anims;
        StartSpriteAnim(&gSprites[sprite], metadata[i].animNum);
        gSprites[sprite].data[0] = hasVerticalMove;
        gSprites[sprite].data[1] = metadata[i].xOff;
        gSprites[sprite].data[2] = 0;
    }
}

static void CreateCloudSprites(void)
{
    CreateMovingScenerySprites(0, sSpriteMetadata_Clouds, sAnims_Clouds, 9);
}

static void CreateTreeSprites(void)
{
    CreateMovingScenerySprites(1, sSpriteMetadata_Trees, sAnims_Trees, 12);
}

static void CreateHouseSprites(void)
{
    CreateMovingScenerySprites(1, sSpriteMetadata_HouseSilhouette, sAnims_HouseSilhouette, 6);
}

static void SpriteCB_Player(struct Sprite *sprite)
{
}

static void SpriteCB_Bicycle(struct Sprite* sprite)
{
    sprite->invisible = gSprites[sprite->data[0]].invisible;
    sprite->x = gSprites[sprite->data[0]].x;
    sprite->y = gSprites[sprite->data[0]].y + 8;
    sprite->x2 = gSprites[sprite->data[0]].x2;
    sprite->y2 = gSprites[sprite->data[0]].y2;
}



u8 CreateIntroBrendanSprite(s16 x, s16 y)
{
    u8 playerSpriteId = CreateSprite(&sSpriteTemplate_Brendan, x, y, 0);
    u8 bicycleSpriteId = CreateSprite(&sSpriteTemplate_BrendanBicycle, x, y + 8, 1);
    gSprites[bicycleSpriteId].data[0] = playerSpriteId;
    return playerSpriteId;
}

u8 CreateIntroMaySprite(s16 x, s16 y)
{
    u8 playerSpriteId = CreateSprite(&sSpriteTemplate_May, x, y, 0);
    u8 bicycleSpriteId = CreateSprite(&sSpriteTemplate_MayBicycle, x, y + 8, 1);
    gSprites[bicycleSpriteId].data[0] = playerSpriteId;
    return playerSpriteId;
}

void nullsub_83(struct Sprite *sprite)
{
}

void sub_81493C4(struct Sprite* sprite)
{
    sprite->invisible = gSprites[sprite->data[0]].invisible;
    sprite->y = gSprites[sprite->data[0]].y;
    sprite->x2 = gSprites[sprite->data[0]].x2;
    sprite->y2 = gSprites[sprite->data[0]].y2;
}

u8 intro_create_latios_sprite(s16 x, s16 y)
{
    u8 leftSpriteId = CreateSprite(&gSpriteTemplate_8416D7C, x - 32, y, 2);
    u8 rightSpriteId = CreateSprite(&gSpriteTemplate_8416D7C, x + 32, y, 2);
    gSprites[rightSpriteId].data[0] = leftSpriteId;
    StartSpriteAnim(&gSprites[rightSpriteId], 1);
    gSprites[rightSpriteId].callback = &sub_81493C4;
    return leftSpriteId;
}

u8 intro_create_latias_sprite(s16 x, s16 y)
{
    u8 leftSpriteId = CreateSprite(&gSpriteTemplate_8416D94, x - 32, y, 2);
    u8 rightSpriteId = CreateSprite(&gSpriteTemplate_8416D94, x + 32, y, 2);
    gSprites[rightSpriteId].data[0] = leftSpriteId;
    StartSpriteAnim(&gSprites[rightSpriteId], 1);
    gSprites[rightSpriteId].callback = &sub_81493C4;
    return leftSpriteId;
}
