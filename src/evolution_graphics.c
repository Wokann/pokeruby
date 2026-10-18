#include "global.h"
#include "constants/rgb.h"
#include "evolution_graphics.h"
#include "sprite.h"
#include "trig.h"
#include "random.h"
#include "decompress.h"
#include "task.h"
#include "sound.h"
#include "constants/songs.h"
#include "palette.h"
// this file's functions
static void SpriteCB_Sparkle_Dummy(struct Sprite* sprite);
static void Task_Sparkles_SpiralUpward_Init(u8 taskId);
static void Task_Sparkles_SpiralUpward(u8 taskId);
static void Task_Sparkles_SpiralUpward_End(u8 taskId);
static void Task_Sparkles_ArcDown_Init(u8 taskId);
static void Task_Sparkles_ArcDown(u8 taskId);
static void Task_Sparkles_ArcDown_End(u8 taskId);
static void Task_Sparkles_CircleInward_Init(u8 taskId);
static void Task_Sparkles_CircleInward(u8 taskId);
static void Task_Sparkles_CircleInward_End(u8 taskId);
static void Task_Sparkles_SprayAndFlash_Init(u8 taskId);
static void Task_Sparkles_SprayAndFlash(u8 taskId);
static void Task_Sparkles_SprayAndFlashTrade_Init(u8 taskId);
static void Task_Sparkles_SprayAndFlashTrade(u8 taskId);
static void Task_Sparkles_SprayAndFlash_End(u8 taskId);

static void Task_CycleEvolutionMonSprite_Init(u8 taskId);
static void Task_CycleEvolutionMonSprite_TryEnd(u8 taskId);
static void EndOnPreEvoMon(u8 taskId);
static void EndOnPostEvoMon(u8 taskId);
static void Task_CycleEvolutionMonSprite_UpdateSize(u8 taskId);

// const data
static const u16 sEvoSparkle_Pal[] = INCBIN_U16("graphics/misc/evo_sparkle.gbapal");
static const u8 sEvoSparkle_Gfx[] = INCBIN_U8("graphics/misc/evo_sparkle.4bpp.lz");

static const struct CompressedSpriteSheet sEvoSparkleSpriteSheets[] =
{
    {sEvoSparkle_Gfx, 0x20, 1001},
    {NULL, 0, 0}
};
static const struct SpritePalette sEvoSparkleSpritePals[] =
{
    {sEvoSparkle_Pal, 1001},
    {NULL, 0}
};

static const struct OamData sOamData_EvoSparkle =
{
    .y = 160,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 0,
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sSpriteAnim_EvoSparkle[] =
{
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_EvoSparkle[] =
{
    sSpriteAnim_EvoSparkle,
};

static const struct SpriteTemplate sEvoSparkleSpriteTemplate =
{
    .tileTag = 1001,
    .paletteTag = 1001,
    .oam = &sOamData_EvoSparkle,
    .anims = sSpriteAnimTable_EvoSparkle,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_Sparkle_Dummy
};

static const s16 sEvoSparkleMatrices[] =
{
    0x3C0, 0x380, 0x340, 0x300, 0x2C0, 0x280, 0x240, 0x200, 0x1C0,
    0x180, 0x140, 0x100, -4, 0x10, -3, 0x30, -2, 0x50,
    -1, 0x70, 0x1, 0x70, 0x2, 0x50, 0x3, 0x30, 0x4, 0x10
};

// code

static void SpriteCB_Sparkle_Dummy(struct Sprite* sprite)
{

}

static void SetEvoSparklesMatrices(void)
{
    u16 i;
    for (i = 0; i < 12; i++)
    {
        SetOamMatrix(20 + i, sEvoSparkleMatrices[i], 0, 0, sEvoSparkleMatrices[i]);
    }
}

static void SpriteCB_Sparkle_SpiralUpward(struct Sprite* sprite)
{
    if (sprite->y > 8)
    {
        u8 matrixNum;

        sprite->y = 88 - (sprite->data[7] * sprite->data[7]) / 80;
        sprite->y2 = Sin((u8)(sprite->data[6]), sprite->data[5]) / 4;
        sprite->x2 = Cos((u8)(sprite->data[6]), sprite->data[5]);
        sprite->data[6] += 4;
        if (sprite->data[7] & 1)
            sprite->data[5]--;
        sprite->data[7]++;
        if (sprite->y2 > 0)
            sprite->subpriority = 1;
        else
            sprite->subpriority = 20;
        matrixNum = sprite->data[5] / 4 + 20;
        if (matrixNum > 31)
            matrixNum = 31;
        sprite->oam.matrixNum = matrixNum;
    }
    else
        DestroySprite(sprite);
}

static void CreateSparkle_SpiralUpward(u8 trigIdx)
{
    u8 spriteId = CreateSprite(&sEvoSparkleSpriteTemplate, 120, 88, 0);
    if (spriteId != MAX_SPRITES)
    {
        gSprites[spriteId].data[5] = 48;
        gSprites[spriteId].data[6] = trigIdx;
        gSprites[spriteId].data[7] = 0;
        gSprites[spriteId].oam.affineMode = 1;
        gSprites[spriteId].oam.matrixNum = 31;
        gSprites[spriteId].callback = SpriteCB_Sparkle_SpiralUpward;
    }
}

static void SpriteCB_Sparkle_ArcDown(struct Sprite* sprite)
{
    if (sprite->y < 88)
    {
        sprite->y = 8 + (sprite->data[7] * sprite->data[7]) / 5;
        sprite->y2 = Sin((u8)(sprite->data[6]), sprite->data[5]) / 4;
        sprite->x2 = Cos((u8)(sprite->data[6]), sprite->data[5]);
        sprite->data[5] = 8 + Sin((u8)(sprite->data[7] * 4), 40);
        sprite->data[7]++;
    }
    else
        DestroySprite(sprite);
}

static void CreateSparkle_ArcDown(u8 trigIdx)
{
    u8 spriteId = CreateSprite(&sEvoSparkleSpriteTemplate, 120, 8, 0);
    if (spriteId != MAX_SPRITES)
    {
        gSprites[spriteId].data[5] = 8;
        gSprites[spriteId].data[6] = trigIdx;
        gSprites[spriteId].data[7] = 0;
        gSprites[spriteId].oam.affineMode = 1;
        gSprites[spriteId].oam.matrixNum = 25;
        gSprites[spriteId].subpriority = 1;
        gSprites[spriteId].callback = SpriteCB_Sparkle_ArcDown;
    }
}

static void SpriteCB_Sparkle_CircleInward(struct Sprite* sprite)
{
    if (sprite->data[5] > 8)
    {
        sprite->y2 = Sin((u8)(sprite->data[6]), sprite->data[5]);
        sprite->x2 = Cos((u8)(sprite->data[6]), sprite->data[5]);
        sprite->data[5] -= sprite->data[3];
        sprite->data[6] += 4;
    }
    else
        DestroySprite(sprite);
}

static void CreateSparkle_CircleInward(u8 trigIdx, u8 speed)
{
    u8 spriteId = CreateSprite(&sEvoSparkleSpriteTemplate, 120, 56, 0);
    if (spriteId != MAX_SPRITES)
    {
        gSprites[spriteId].data[3] = speed;
        gSprites[spriteId].data[5] = 120;
        gSprites[spriteId].data[6] = trigIdx;
        gSprites[spriteId].data[7] = 0;
        gSprites[spriteId].oam.affineMode = 1;
        gSprites[spriteId].oam.matrixNum = 31;
        gSprites[spriteId].subpriority = 1;
        gSprites[spriteId].callback = SpriteCB_Sparkle_CircleInward;
    }
}

static void SpriteCB_Sparkle_Spray(struct Sprite* sprite)
{
    if (!(sprite->data[7] & 3))
        sprite->y++;
    if (sprite->data[6] < 128)
    {
        u8 matrixNum;

        sprite->y2 = -Sin((u8)(sprite->data[6]), sprite->data[5]);
        sprite->x = 120 + (sprite->data[3] * sprite->data[7]) / 3;
        sprite->data[6]++;
        matrixNum = 31 - (sprite->data[6] * 12 / 128);
        if (sprite->data[6] > 64)
            sprite->subpriority = 1;
        else
        {
            sprite->invisible = FALSE;
            sprite->subpriority = 20;
            if (sprite->data[6] > 112 && sprite->data[6] & 1)
                sprite->invisible = TRUE;
        }
        if (matrixNum < 20)
            matrixNum = 20;
        sprite->oam.matrixNum = matrixNum;
        sprite->data[7]++;
    }
    else
        DestroySprite(sprite);
}

static void CreateSparkle_Spray(u8 id)
{
    u8 spriteId = CreateSprite(&sEvoSparkleSpriteTemplate, 120, 56, 0);
    if (spriteId != MAX_SPRITES)
    {
        gSprites[spriteId].data[3] = 3 - (Random() % 7);
        gSprites[spriteId].data[5] = 48 + (Random() & 0x3F);
        gSprites[spriteId].data[7] = 0;
        gSprites[spriteId].oam.affineMode = 1;
        gSprites[spriteId].oam.matrixNum = 31;
        gSprites[spriteId].subpriority = 20;
        gSprites[spriteId].callback = SpriteCB_Sparkle_Spray;
    }
}

void LoadEvoSparkleSpriteAndPal(void)
{
    LoadCompressedSpriteSheet(&sEvoSparkleSpriteSheets[0]);
    LoadSpritePalettes(sEvoSparkleSpritePals);
}

#define tTimer   data[15]

u8 EvolutionSparkles_SpiralUpward(u16 palNum)
{
    u8 taskId = CreateTask(Task_Sparkles_SpiralUpward_Init, 0);
    gTasks[taskId].data[1] = palNum;
    return taskId;
}

static void Task_Sparkles_SpiralUpward_Init(u8 taskId)
{
    SetEvoSparklesMatrices();
    gTasks[taskId].tTimer = 0;
    BeginNormalPaletteFade(3 << gTasks[taskId].data[1], 10, 0, 16, RGB(31, 31, 31));
    gTasks[taskId].func = Task_Sparkles_SpiralUpward;
    PlaySE(SE_M_MEGA_KICK);
}

static void Task_Sparkles_SpiralUpward(u8 taskId)
{
    if (gTasks[taskId].tTimer < 64)
    {
        if (!(gTasks[taskId].tTimer & 7))
        {
            u8 i;
            for (i = 0; i < 4; i++)
                CreateSparkle_SpiralUpward((0x78 & gTasks[taskId].tTimer) * 2 + i * 64);
        }
        gTasks[taskId].tTimer++;
    }
    else
    {
        gTasks[taskId].tTimer = 96;
        gTasks[taskId].func = Task_Sparkles_SpiralUpward_End;
    }
}

static void Task_Sparkles_SpiralUpward_End(u8 taskId)
{
    if (gTasks[taskId].tTimer != 0)
        gTasks[taskId].tTimer--;
    else
        DestroyTask(taskId);
}

u8 EvolutionSparkles_ArcDown(void)
{
    return CreateTask(Task_Sparkles_ArcDown_Init, 0);
}

static void Task_Sparkles_ArcDown_Init(u8 taskId)
{
    SetEvoSparklesMatrices();
    gTasks[taskId].tTimer = 0;
    gTasks[taskId].func = Task_Sparkles_ArcDown;
    PlaySE(SE_M_BUBBLE_BEAM2);
}

static void Task_Sparkles_ArcDown(u8 taskId)
{
    if (gTasks[taskId].tTimer < 96)
    {
        if (gTasks[taskId].tTimer < 6)
        {
            u8 i;
            for (i = 0; i < 9; i++)
                CreateSparkle_ArcDown(i * 16);
        }
        gTasks[taskId].tTimer++;
    }
    else
        gTasks[taskId].func = Task_Sparkles_ArcDown_End;
}

static void Task_Sparkles_ArcDown_End(u8 taskId)
{
    DestroyTask(taskId);
}

u8 EvolutionSparkles_CircleInward(void)
{
    return CreateTask(Task_Sparkles_CircleInward_Init, 0);
}

static void Task_Sparkles_CircleInward_Init(u8 taskId)
{
    SetEvoSparklesMatrices();
    gTasks[taskId].tTimer = 0;
    gTasks[taskId].func = Task_Sparkles_CircleInward;
    PlaySE(SE_SHINY);
}

static void Task_Sparkles_CircleInward(u8 taskId)
{
    if (gTasks[taskId].tTimer < 48)
    {
        if (gTasks[taskId].tTimer == 0)
        {
            u8 i;
            for (i = 0; i < 16; i++)
                CreateSparkle_CircleInward(i * 16, 4);
        }
        if (gTasks[taskId].tTimer == 32)
        {
            u8 i;
            for (i = 0; i < 16; i++)
                CreateSparkle_CircleInward(i * 16, 8);
        }
        gTasks[taskId].tTimer++;
    }
    else
        gTasks[taskId].func = Task_Sparkles_CircleInward_End;
}

static void Task_Sparkles_CircleInward_End(u8 taskId)
{
    DestroyTask(taskId);
}

u8 EvolutionSparkles_SprayAndFlash(u16 species)
{
    u8 taskId = CreateTask(Task_Sparkles_SprayAndFlash_Init, 0);
    gTasks[taskId].data[2] = species;
    return taskId;
}

static void Task_Sparkles_SprayAndFlash_Init(u8 taskId)
{
    SetEvoSparklesMatrices();
    gTasks[taskId].tTimer = 0;
    CpuSet(&gPlttBufferFaded[0x20], &gPlttBufferUnfaded[0x20], 0x30);
    BeginNormalPaletteFade(0xFFF9001C, 0, 0, 16, RGB(31, 31, 31));
    gTasks[taskId].func = Task_Sparkles_SprayAndFlash;
    PlaySE(SE_M_PETAL_DANCE);
}

static void Task_Sparkles_SprayAndFlash(u8 taskId)
{
    if (gTasks[taskId].tTimer < 128)
    {
        u8 i;
        switch (gTasks[taskId].tTimer)
        {
        default:
            if (gTasks[taskId].tTimer < 50)
                CreateSparkle_Spray(Random() & 7);
            break;
        case 0:
            for (i = 0; i < 8; i++)
                CreateSparkle_Spray(i);
            break;
        case 32:
            BeginNormalPaletteFade(0xFFFF001C, 16, 16, 0, RGB(31, 31, 31));
            break;
        }
        gTasks[taskId].tTimer++;
    }
    else
        gTasks[taskId].func = Task_Sparkles_SprayAndFlash_End;
}

static void Task_Sparkles_SprayAndFlash_End(u8 taskId)
{
    if (!gPaletteFade.active)
        DestroyTask(taskId);
}

u8 EvolutionSparkles_SprayAndFlash_Trade(u16 species)
{
    u8 taskId = CreateTask(Task_Sparkles_SprayAndFlashTrade_Init, 0);
    gTasks[taskId].data[2] = species;
    return taskId;
}

static void Task_Sparkles_SprayAndFlashTrade_Init(u8 taskId)
{
    SetEvoSparklesMatrices();
    gTasks[taskId].tTimer = 0;
    CpuSet(&gPlttBufferFaded[0x20], &gPlttBufferUnfaded[0x20], 0x30);
    BeginNormalPaletteFade(0xFFF90001, 0, 0, 16, RGB(31, 31, 31));
    gTasks[taskId].func = Task_Sparkles_SprayAndFlashTrade;
    PlaySE(SE_M_PETAL_DANCE);
}

static void Task_Sparkles_SprayAndFlashTrade(u8 taskId)
{
    if (gTasks[taskId].tTimer < 128)
    {
        u8 i;
        switch (gTasks[taskId].tTimer)
        {
        default:
            if (gTasks[taskId].tTimer < 50)
                CreateSparkle_Spray(Random() & 7);
            break;
        case 0:
            for (i = 0; i < 8; i++)
                CreateSparkle_Spray(i);
            break;
        case 32:
            BeginNormalPaletteFade(0xFFFF0001, 16, 16, 0, RGB(31, 31, 31));
            break;
        }
        gTasks[taskId].tTimer++;
    }
    else
        gTasks[taskId].func = Task_Sparkles_SprayAndFlash_End;
}

#undef tTimer

static void SpriteCB_EvolutionMonSprite(struct Sprite* sprite)
{

}

#define tPreEvoSpriteId     data[1]
#define tPostEvoSpriteId    data[2]
#define tEvoStopped         data[8]

u8 CycleEvolutionMonSprite(u8 preEvoSpriteId, u8 postEvoSpriteId)
{
    u16 i;
    u16 monPalette[16];
    u8 taskId;
    s32 toDiv;

    for (i = 0; i < 16; i++)
        monPalette[i] = 0x7FFF;

    taskId = CreateTask(Task_CycleEvolutionMonSprite_Init, 0);
    gTasks[taskId].tPreEvoSpriteId = preEvoSpriteId;
    gTasks[taskId].tPostEvoSpriteId = postEvoSpriteId;
    gTasks[taskId].data[3] = 256;
    gTasks[taskId].data[4] = 16;

    toDiv = 65536;
    SetOamMatrix(30, 256, 0, 0, 256);
    SetOamMatrix(31, toDiv / gTasks[taskId].data[4], 0, 0, toDiv / gTasks[taskId].data[4]);

    gSprites[preEvoSpriteId].callback = SpriteCB_EvolutionMonSprite;
    gSprites[preEvoSpriteId].oam.affineMode = 1;
    gSprites[preEvoSpriteId].oam.matrixNum = 30;
    gSprites[preEvoSpriteId].invisible = FALSE;
    CpuSet(monPalette, &gPlttBufferFaded[0x100 + (gSprites[preEvoSpriteId].oam.paletteNum * 16)], 16);

    gSprites[postEvoSpriteId].callback = SpriteCB_EvolutionMonSprite;
    gSprites[postEvoSpriteId].oam.affineMode = 1;
    gSprites[postEvoSpriteId].oam.matrixNum = 31;
    gSprites[postEvoSpriteId].invisible = FALSE;
    CpuSet(monPalette, &gPlttBufferFaded[0x100 + (gSprites[postEvoSpriteId].oam.paletteNum * 16)], 16);

    gTasks[taskId].tEvoStopped = FALSE;
    return taskId;
}

static void Task_CycleEvolutionMonSprite_Init(u8 taskId)
{
    gTasks[taskId].data[5] = 0;
    gTasks[taskId].data[6] = 8;
    gTasks[taskId].func = Task_CycleEvolutionMonSprite_TryEnd;
}

static void Task_CycleEvolutionMonSprite_TryEnd(u8 taskId)
{
    if (gTasks[taskId].tEvoStopped)
        EndOnPreEvoMon(taskId);
    else if (gTasks[taskId].data[6] == 128)
        EndOnPostEvoMon(taskId);
    else
    {
        gTasks[taskId].data[6] += 2;
        gTasks[taskId].data[5] ^= 1;
        gTasks[taskId].func = Task_CycleEvolutionMonSprite_UpdateSize;
    }
}

static void Task_CycleEvolutionMonSprite_UpdateSize(u8 taskId)
{
    if (gTasks[taskId].tEvoStopped)
        gTasks[taskId].func = EndOnPreEvoMon;
    else
    {
        u16 oamMatrixArg;
        u8 numSpritesFinished = 0;
        if (gTasks[taskId].data[5] == 0)
        {
            if (gTasks[taskId].data[3] < 256 - gTasks[taskId].data[6])
                gTasks[taskId].data[3] += gTasks[taskId].data[6];
            else
            {
                gTasks[taskId].data[3] = 256;
                numSpritesFinished++;
            }
            if (gTasks[taskId].data[4] > 16 + gTasks[taskId].data[6])
                gTasks[taskId].data[4]  -= gTasks[taskId].data[6];
            else
            {
                gTasks[taskId].data[4] = 16;
                numSpritesFinished++;
            }
        }
        else
        {
            if (gTasks[taskId].data[4] < 256 - gTasks[taskId].data[6])
                gTasks[taskId].data[4] += gTasks[taskId].data[6];
            else
            {
                gTasks[taskId].data[4] = 256;
                numSpritesFinished++;
            }
            if (gTasks[taskId].data[3] > 16 + gTasks[taskId].data[6])
                gTasks[taskId].data[3]  -= gTasks[taskId].data[6];
            else
            {
                gTasks[taskId].data[3] = 16;
                numSpritesFinished++;
            }
        }
        oamMatrixArg = 65536 / gTasks[taskId].data[3];
        SetOamMatrix(30, oamMatrixArg, 0, 0, oamMatrixArg);

        oamMatrixArg = 65536 / gTasks[taskId].data[4];
        SetOamMatrix(31, oamMatrixArg, 0, 0, oamMatrixArg);
        if (numSpritesFinished == 2)
            gTasks[taskId].func = Task_CycleEvolutionMonSprite_TryEnd;
    }
}

static void EndOnPostEvoMon(u8 taskId)
{
    gSprites[gTasks[taskId].tPreEvoSpriteId].oam.affineMode = 0;
    gSprites[gTasks[taskId].tPreEvoSpriteId].oam.matrixNum = 0;
    gSprites[gTasks[taskId].tPreEvoSpriteId].invisible = TRUE;

    gSprites[gTasks[taskId].tPostEvoSpriteId].oam.affineMode = 0;
    gSprites[gTasks[taskId].tPostEvoSpriteId].oam.matrixNum = 0;
    gSprites[gTasks[taskId].tPostEvoSpriteId].invisible = FALSE;

    DestroyTask(taskId);
}

static void EndOnPreEvoMon(u8 taskId)
{
    gSprites[gTasks[taskId].tPreEvoSpriteId].oam.affineMode = 0;
    gSprites[gTasks[taskId].tPreEvoSpriteId].oam.matrixNum = 0;
    gSprites[gTasks[taskId].tPreEvoSpriteId].invisible = FALSE;

    gSprites[gTasks[taskId].tPostEvoSpriteId].oam.affineMode = 0;
    gSprites[gTasks[taskId].tPostEvoSpriteId].oam.matrixNum = 0;
    gSprites[gTasks[taskId].tPostEvoSpriteId].invisible = TRUE;

    DestroyTask(taskId);
}
