#include "global.h"
#include "gba/m4a_internal.h"
#include "intro.h"
#include "data2.h"
#include "decompress.h"
#include "hall_of_fame.h"
#include "intro_credits_graphics.h"
#include "libgncmultiboot.h"
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

const u16 gIntro2GrassPalette[] = INCBIN_U16("graphics/intro/intro2_grass.gbapal");
const u16 gIntro2GrassAfternoonPalette[] = INCBIN_U16("graphics/intro/intro2_grass_afternoon.gbapal");
const u16 gIntro2GrassNightPalette[] = INCBIN_U16("graphics/intro/intro2_grass_night.gbapal");
const u8 gIntro2GrassTiles[] = INCBIN_U8("graphics/intro/intro2_grass.4bpp.lz");
const u8 gIntro2GrassTilemap[] = INCBIN_U8("graphics/intro/intro2_grass_map.bin.lz");
const u16 gIntro2CloudsBgPalette[] = INCBIN_U16("graphics/intro/8412818.gbapal");
const u16 gIntro2CloudsBgAfternoonPalette[] = INCBIN_U16("graphics/intro/8412878.gbapal");
const u8 gIntro2CloudsBgTiles[] = INCBIN_U8("graphics/intro/intro2_bgclouds.4bpp.lz");
const u8 gIntro2CloudsBgTilemap[] = INCBIN_U8("graphics/intro/intro2_bgclouds_map.bin.lz");
const u16 gIntro2CloudsPalette[] = INCBIN_U16("graphics/intro/intro2_bgclouds.gbapal");
const u16 gIntro2CloudsAfternoonPalette[] = INCBIN_U16("graphics/intro/intro2_bgclouds_afternoon.gbapal");
const u8 gIntro2CloudsTiles[] = INCBIN_U8("graphics/intro/intro2_bgclouds2.4bpp.lz");
const u16 gIntro2TreesBgPalette[] = INCBIN_U16("graphics/intro/intro2_bgtrees2.gbapal");
const u16 gIntro2TreesAfternoonPalette[] = INCBIN_U16("graphics/intro/intro2_bgtrees2_afternoon.gbapal");
const u8 gIntro2TreesTiles[] = INCBIN_U8("graphics/intro/intro2_bgtrees.4bpp.lz");
const u8 gIntro2TreesTilemap[] = INCBIN_U8("graphics/intro/intro2_bgtrees_map.bin.lz");
const u16 gIntro2TreesSmallPalette[] = INCBIN_U16("graphics/intro/intro2_bgtrees.gbapal");
const u8 gIntro2TreeTiles[] = INCBIN_U8("graphics/intro/intro2_bgtreessmall.4bpp.lz");
const u16 gIntro2NightBgLayerPalette[] = INCBIN_U16("graphics/intro/8413E38.gbapal");
const u8 gIntro2NightBgTiles[] = INCBIN_U8("graphics/intro/intro2_bgnight.4bpp.lz"); // only used in credits, coupled with intro because bicycle sequence
const u16 gIntro2NightBgPalette[] = INCBIN_U16("graphics/intro/intro2_bgnight.gbapal");
const u8 gIntro2NightBgTilemap[] = INCBIN_U8("graphics/intro/intro2_bgnight_map.bin.lz");
const u8 gIntro2NightTiles[] = INCBIN_U8("graphics/intro/intro2_night.4bpp.lz");
const u16 gIntro2BrendanPalette[] = INCBIN_U16("graphics/intro/intro2_brendan.gbapal");
const u8 gIntro2BrendanTiles[] = INCBIN_U8("graphics/intro/intro2_brendan.4bpp.lz");
const u16 gIntro2MayPalette[] = INCBIN_U16("graphics/intro/intro2_may.gbapal");
const u16 gIntro2UnusedPaletteData[0xF0] = {0};
const u8 gIntro2MayTiles[] = INCBIN_U8("graphics/intro/intro2_may.4bpp.lz");
const u8 gIntro2BicycleTiles[] = INCBIN_U8("graphics/intro/intro2_bicycle.4bpp.lz");
const u16 gIntro2LatiosPalette[] = INCBIN_U16("graphics/intro/intro2_latios.gbapal");
const u8 gIntro2LatiosTiles[] = INCBIN_U8("graphics/intro/intro2_latios.4bpp.lz");
const u16 gIntro2LatiasPalette[] = INCBIN_U16("graphics/intro/intro2_latias.gbapal");
const u8 gIntro2LatiasTiles[] = INCBIN_U8("graphics/intro/intro2_latias.4bpp.lz");

void SpriteCB_MovingScenery(struct Sprite *sprite);
void SpriteCB_Player(struct Sprite *sprite);
void SpriteCB_Bicycle(struct Sprite *sprite);
void SpriteCB_LatiLeftHalf(struct Sprite *sprite);

const struct SpriteTemplate gSpriteTemplate_MovingScenery = {
    2000, 0xFFFF, &gDummyOamData, gDummySpriteAnimTable, NULL, gDummySpriteAffineAnimTable, SpriteCB_MovingScenery
};

const struct CompressedSpriteSheet gSpriteSheet_Clouds[] = {
    { gIntro2CloudsTiles, 0x400, 2000 },
    {}
};

const union AnimCmd gAnim_CloudLargest[] = {
    ANIMCMD_FRAME( 0, 30),
    ANIMCMD_END
};

const union AnimCmd gAnim_CloudLarge[] = {
    ANIMCMD_FRAME(16, 30),
    ANIMCMD_END
};

const union AnimCmd gAnim_CloudSmall[] = {
    ANIMCMD_FRAME(20, 30),
    ANIMCMD_END
};

const union AnimCmd gAnim_CloudSmallest[] = {
    ANIMCMD_FRAME(22, 30),
    ANIMCMD_END
};

const union AnimCmd *const gAnims_Clouds[] = {
    gAnim_CloudLargest,
    gAnim_CloudLarge,
    gAnim_CloudSmall,
    gAnim_CloudSmallest
};

const struct IntroCreditsSpriteMetadata gSpriteMetadata_Clouds[] = {
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

const struct CompressedSpriteSheet gSpriteSheet_TreesSmall[] = {
    { gIntro2TreeTiles, 0x400, 2000 },
    {}
};

const union AnimCmd gAnim_Trees0[] = {
    ANIMCMD_FRAME( 0, 30),
    ANIMCMD_END
};

const union AnimCmd gAnim_Trees1[] = {
    ANIMCMD_FRAME(16, 30),
    ANIMCMD_END
};

const union AnimCmd gAnim_Trees2[] = {
    ANIMCMD_FRAME(24, 30),
    ANIMCMD_END
};

const union AnimCmd *const gAnims_Trees[] = {
    gAnim_Trees0,
    gAnim_Trees1,
    gAnim_Trees2
};

const struct IntroCreditsSpriteMetadata gSpriteMetadata_Trees[] = {
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

const struct CompressedSpriteSheet gSpriteSheet_HouseSilhouette[] = {
    { gIntro2NightTiles, 0x400, 2000 },
    {}
};

const union AnimCmd gAnim_HouseSilhouette[] = {
    ANIMCMD_FRAME(0, 30),
    ANIMCMD_END
};

const union AnimCmd *const gAnims_HouseSilhouette[] = {
    gAnim_HouseSilhouette
};

const struct IntroCreditsSpriteMetadata gSpriteMetadata_HouseSilhouette[] = {
    { 0, ST_OAM_SQUARE, 2,   24, 88, 100, 0x1000 },
    { 0, ST_OAM_SQUARE, 2,   64, 88, 100, 0x1000 },
    { 0, ST_OAM_SQUARE, 2,  104, 88, 100, 0x1000 },
    { 0, ST_OAM_SQUARE, 2,  144, 88, 100, 0x1000 },
    { 0, ST_OAM_SQUARE, 2,  184, 88, 100, 0x1000 },
    { 0, ST_OAM_SQUARE, 2,  224, 88, 100, 0x1000 },
};

const struct OamData gOamData_Player = {
    .y = 160, .shape = ST_OAM_SQUARE, .size = 3, .priority = 1
};

const union AnimCmd gAnim_Player[] = {
    ANIMCMD_FRAME(  0, 8),
    ANIMCMD_FRAME( 64, 8),
    ANIMCMD_FRAME(128, 8),
    ANIMCMD_FRAME(192, 8),
    ANIMCMD_JUMP(0)
};

const union AnimCmd *const gAnims_Player[] = {
    gAnim_Player
};

const struct SpriteTemplate gSpriteTemplate_Brendan = {
    1002, 1002, &gOamData_Player, gAnims_Player, NULL, gDummySpriteAffineAnimTable, SpriteCB_Player
};

const struct SpriteTemplate gSpriteTemplate_May = {
    1003, 1003, &gOamData_Player, gAnims_Player, NULL, gDummySpriteAffineAnimTable, SpriteCB_Player
};

const struct OamData gOamData_Bicycle = {
    .y = 160, .shape = ST_OAM_H_RECTANGLE, .size = 3, .priority = 1
};

const union AnimCmd gAnim_Bicycle[] = {
    ANIMCMD_FRAME(  0, 8),
    ANIMCMD_FRAME( 32, 8),
    ANIMCMD_FRAME( 64, 8),
    ANIMCMD_FRAME( 96, 8),
    ANIMCMD_JUMP(0)
};

const union AnimCmd *const gAnims_Bicycle[] = {
    gAnim_Bicycle
};

const struct SpriteTemplate gSpriteTemplate_BrendanBicycle = {
    1001, 1002, &gOamData_Bicycle, gAnims_Bicycle, NULL, gDummySpriteAffineAnimTable, SpriteCB_Bicycle
};

const struct SpriteTemplate gSpriteTemplate_MayBicycle = {
    1001, 1003, &gOamData_Bicycle, gAnims_Bicycle, NULL, gDummySpriteAffineAnimTable, SpriteCB_Bicycle
};

const struct OamData gOamData_LatiosLatias = {
    .y = 160, .shape = ST_OAM_SQUARE, .size = 3, .priority = 1
};

const union AnimCmd gAnim_LatiLeftHalf[] = {
    ANIMCMD_FRAME(  0, 16),
    ANIMCMD_END
};

const union AnimCmd gAnim_LatiRightHalf[] = {
    ANIMCMD_FRAME( 64, 16),
    ANIMCMD_END
};

const union AnimCmd *const gAnims_Lati[] = {
    gAnim_LatiLeftHalf,
    gAnim_LatiRightHalf
};

const struct SpriteTemplate gSpriteTemplate_Latios = {
    1004, 1004, &gOamData_LatiosLatias, gAnims_Lati, NULL, gDummySpriteAffineAnimTable, SpriteCB_LatiLeftHalf
};

const struct SpriteTemplate gSpriteTemplate_Latias = {
    1005, 1005, &gOamData_LatiosLatias, gAnims_Lati, NULL, gDummySpriteAffineAnimTable, SpriteCB_LatiLeftHalf
};

const struct CompressedSpriteSheet gIntro2BrendanSpriteSheet[] = {
    { gIntro2BrendanTiles, 0x3800, 1002 },
    {}
};
const struct CompressedSpriteSheet gIntro2MaySpriteSheet[] = {
    { gIntro2MayTiles, 0x3800, 1003 },
    {}
};
const struct CompressedSpriteSheet gIntro2BicycleSpriteSheet[] = {
    { gIntro2BicycleTiles, 0x1000, 1001 },
    {}
};
const struct CompressedSpriteSheet gIntro2LatiosSpriteSheet[] = {
    { gIntro2LatiosTiles, 0x1000, 1004 },
    {}
};
const struct CompressedSpriteSheet gIntro2LatiasSpriteSheet[] = {
    { gIntro2LatiasTiles, 0x1000, 1005 },
    {}
};

const struct SpritePalette gIntro2SpritePalettes[] = {
    {gIntro2BrendanPalette, 1002},
    {gIntro2MayPalette,     1003},
    {gIntro2LatiosPalette,  1004},
    {gIntro2LatiasPalette,  1005},
    {}
};

const struct CompressedSpriteSheet gSpriteSheet_CreditsRivalBrendan[] = {
    { gIntro2BrendanTiles, 0x2000, 1002},
    {}
};

const struct CompressedSpriteSheet gSpriteSheet_CreditsRivalMay[] = {
    { gIntro2MayTiles, 0x2000, 1003},
    {}
};


EWRAM_DATA u16 gIntroCredits_MovingSceneryVBase = 0;
EWRAM_DATA s16 gIntroCredits_MovingSceneryVOffset = 0;
EWRAM_DATA s16 gIntroCredits_MovingSceneryState = 0;

extern u8 gReservedSpritePaletteCount;

void CreateIntroCloudSprites();
void CreateTreeSprites();

void LoadIntroPart2Graphics(u8 scenery)
{
    LZ77UnCompVram(&gIntro2GrassTiles, (void *)(VRAM + 0x4000));
    LZ77UnCompVram(&gIntro2GrassTilemap, (void *)(VRAM + 0x7800));
    LoadPalette(&gIntro2GrassPalette, 240, 32);
    switch (scenery)
    {
    case 0:
    default:
        LZ77UnCompVram(&gIntro2CloudsBgTiles, (void *)(VRAM));
        LZ77UnCompVram(&gIntro2CloudsBgTilemap, (void *)(VRAM + 0x3000));
        LoadPalette(&gIntro2CloudsBgPalette, 0, 96);
        LoadCompressedSpriteSheet(gSpriteSheet_Clouds);
        LoadPalette(&gIntro2CloudsPalette, 256, 32);
        CreateIntroCloudSprites();
        break;
    case 1:
        LZ77UnCompVram(&gIntro2TreesTiles, (void *)(VRAM));
        LZ77UnCompVram(&gIntro2TreesTilemap, (void *)(VRAM + 0x3000));
        LoadPalette(&gIntro2TreesBgPalette, 0, 32);
        LoadCompressedSpriteSheet(gSpriteSheet_TreesSmall);
        LoadPalette(&gIntro2TreesSmallPalette, 256, 32);
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

void CreateHouseSprites();

void LoadCreditsSceneGraphics(u8 scene)
{
    LZ77UnCompVram(&gIntro2GrassTiles, (void *)(VRAM + 0x4000));
    LZ77UnCompVram(&gIntro2GrassTilemap, (void *)(VRAM + 0x7800));
    switch (scene)
    {
    case 0:
    default:
        LoadPalette(&gIntro2GrassPalette, 240, 32);
        LZ77UnCompVram(&gIntro2CloudsBgTiles, (void *)(VRAM));
        LZ77UnCompVram(&gIntro2CloudsBgTilemap, (void *)(VRAM + 0x3000));
        LoadPalette(&gIntro2CloudsBgPalette, 0, 96);
        LoadCompressedSpriteSheet(gSpriteSheet_Clouds);
        LZ77UnCompVram(&gIntro2CloudsTiles, (void *)(VRAM + 0x10000));
        LoadPalette(&gIntro2CloudsPalette, 256, 32);
        CreateIntroCloudSprites();
        break;
    case 1:
        LoadPalette(&gIntro2GrassAfternoonPalette, 240, 32);
        LZ77UnCompVram(&gIntro2CloudsBgTiles, (void *)(VRAM));
        LZ77UnCompVram(&gIntro2CloudsBgTilemap, (void *)(VRAM + 0x3000));
        LoadPalette(&gIntro2CloudsBgAfternoonPalette, 0, 96);
        LoadCompressedSpriteSheet(gSpriteSheet_Clouds);
        LZ77UnCompVram(&gIntro2CloudsTiles, (void *)(VRAM + 0x10000));
        LoadPalette(&gIntro2CloudsAfternoonPalette, 256, 32);
        CreateIntroCloudSprites();
        break;
    case 2:
    case 3:
        LoadPalette(&gIntro2GrassAfternoonPalette, 240, 32);
        LZ77UnCompVram(&gIntro2TreesTiles, (void *)(VRAM));
        LZ77UnCompVram(&gIntro2TreesTilemap, (void *)(VRAM + 0x3000));
        LoadPalette(&gIntro2TreesAfternoonPalette, 0, 32);
        LoadCompressedSpriteSheet(gSpriteSheet_TreesSmall);
        LoadPalette(&gIntro2TreesAfternoonPalette, 256, 32);
        CreateTreeSprites();
        break;
    case 4:
        LoadPalette(&gIntro2GrassNightPalette, 240, 32);
        LZ77UnCompVram(&gIntro2NightBgTiles, (void *)(VRAM));
        LZ77UnCompVram(&gIntro2NightBgTilemap, (void *)(VRAM + 0x3000));
        LoadPalette(&gIntro2NightBgLayerPalette, 0, 64);
        LoadCompressedSpriteSheet(gSpriteSheet_HouseSilhouette);
        LoadPalette(&gIntro2NightBgPalette, 256, 32);
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

void Task_BicycleBgAnimation(u8 taskId)
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

void SpriteCB_MovingScenery(struct Sprite *sprite)
{
    if (gIntroCredits_MovingSceneryState)
    {
        DestroySprite(sprite);
    }
    else
    {
        s32 var = ((sprite->x << 16) | (u16)sprite->data[2]) + (u16)sprite->data[1];
        sprite->x = var >> 16;
        sprite->data[2] = var;
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

void CreateMovingScenerySprites(bool8 hasVerticalMove, const struct IntroCreditsSpriteMetadata *metadata, const union AnimCmd *const *anims, u8 numSprites)
{
    u8 i;

    for(i = 0; i < numSprites; i++)
    {
        u8 sprite = CreateSprite(&gSpriteTemplate_MovingScenery, metadata[i].x, metadata[i].y, metadata[i].subpriority);
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

void CreateIntroCloudSprites()
{
    CreateMovingScenerySprites(0, gSpriteMetadata_Clouds, gAnims_Clouds, 9);
}

void CreateTreeSprites()
{
    CreateMovingScenerySprites(1, gSpriteMetadata_Trees, gAnims_Trees, 12);
}

void CreateHouseSprites()
{
    CreateMovingScenerySprites(1, gSpriteMetadata_HouseSilhouette, gAnims_HouseSilhouette, 6);
}

void SpriteCB_Player(struct Sprite *sprite)
{
}

void SpriteCB_Bicycle(struct Sprite* sprite)
{
    sprite->invisible = gSprites[sprite->data[0]].invisible;
    sprite->x = gSprites[sprite->data[0]].x;
    sprite->y = gSprites[sprite->data[0]].y + 8;
    sprite->x2 = gSprites[sprite->data[0]].x2;
    sprite->y2 = gSprites[sprite->data[0]].y2;
}



u8 CreateIntroBrendanSprite(s16 x, s16 y)
{
    u8 playerSprite = CreateSprite(&gSpriteTemplate_Brendan, x, y, 0);
    u8 bicycleSprite = CreateSprite(&gSpriteTemplate_BrendanBicycle, x, y + 8, 1);
    gSprites[bicycleSprite].data[0] = playerSprite;
    return playerSprite;
}

u8 CreateIntroMaySprite(s16 x, s16 y)
{
    u8 playerSprite = CreateSprite(&gSpriteTemplate_May, x, y, 0);
    u8 bicycleSprite = CreateSprite(&gSpriteTemplate_MayBicycle, x, y + 8, 1);
    gSprites[bicycleSprite].data[0] = playerSprite;
    return playerSprite;
}

void SpriteCB_LatiLeftHalf(struct Sprite *sprite)
{
}

void SpriteCB_LatiRightHalf(struct Sprite* sprite)
{
    sprite->invisible = gSprites[sprite->data[0]].invisible;
    sprite->y = gSprites[sprite->data[0]].y;
    sprite->x2 = gSprites[sprite->data[0]].x2;
    sprite->y2 = gSprites[sprite->data[0]].y2;
}

u8 CreateIntroLatiosSprite(s16 x, s16 y)
{
    u8 leftHalf = CreateSprite(&gSpriteTemplate_Latios, x - 32, y, 2);
    u8 rightHalf = CreateSprite(&gSpriteTemplate_Latios, x + 32, y, 2);
    gSprites[rightHalf].data[0] = leftHalf;
    StartSpriteAnim(&gSprites[rightHalf], 1);
    gSprites[rightHalf].callback = &SpriteCB_LatiRightHalf;
    return leftHalf;
}

u8 CreateIntroLatiasSprite(s16 x, s16 y)
{
    u8 leftHalf = CreateSprite(&gSpriteTemplate_Latias, x - 32, y, 2);
    u8 rightHalf = CreateSprite(&gSpriteTemplate_Latias, x + 32, y, 2);
    gSprites[rightHalf].data[0] = leftHalf;
    StartSpriteAnim(&gSprites[rightHalf], 1);
    gSprites[rightHalf].callback = &SpriteCB_LatiRightHalf;
    return leftHalf;
}
