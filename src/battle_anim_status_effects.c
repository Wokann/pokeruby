#include "global.h"
#include "battle.h"
#include "battle_anim.h"
#include "util.h"
#include "decompress.h"
#include "palette.h"
#include "sprite.h"
#include "task.h"
#include "trig.h"
#include "ewram.h"

extern const u8 *const gBattleAnims_StatusConditions[];

static const struct Subsprite sFrozenIceCubeSubsprites[] =
{
    {.x = -16, .y = -16, .shape = ST_OAM_SQUARE,      .size = 3, .tileOffset =   0, .priority = 2},
    {.x = -16, .y =  48, .shape = ST_OAM_H_RECTANGLE, .size = 3, .tileOffset =  64, .priority = 2},
    {.x =  48, .y = -16, .shape = ST_OAM_V_RECTANGLE, .size = 3, .tileOffset =  96, .priority = 2},
    {.x =  48, .y =  48, .shape = ST_OAM_SQUARE,      .size = 2, .tileOffset = 128, .priority = 2},
};

static const struct SubspriteTable sFrozenIceCubeSubspriteTable[] =
{
    {ARRAY_COUNT(sFrozenIceCubeSubsprites), sFrozenIceCubeSubsprites},
};

static const struct SpriteTemplate sFrozenIceCubeSpriteTemplate =
{
    .tileTag = ANIM_TAG_ICE_CUBE,
    .paletteTag = ANIM_TAG_ICE_CUBE,
    .oam = &gOamData_AffineOff_ObjBlend_64x64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

static void AnimFlashingCircleImpact(struct Sprite *sprite);
static const struct SpriteTemplate sFlashingCircleImpactSpriteTemplate =
{
    .tileTag = ANIM_TAG_CIRCLE_IMPACT,
    .paletteTag = ANIM_TAG_CIRCLE_IMPACT,
    .oam = &gOamData_AffineOff_ObjNormal_8x8,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimFlashingCircleImpact,
};

static void Task_UpdateFlashingCircleImpacts(u8 taskId);
static void AnimFlashingCircleImpact_Step(struct Sprite *sprite);
static void AnimTask_FrozenIceCube_Step1(u8 taskId);
static void AnimTask_FrozenIceCube_Step2(u8 taskId);
static void AnimTask_FrozenIceCube_Step3(u8 taskId);
static void AnimTask_FrozenIceCube_Step4(u8 taskId);
static void Task_DoStatusAnimation(u8 taskId);

static u8 UNUSED Task_FlashingCircleImpacts(u8 battler, bool8 red)
{
    u8 battlerSpriteId = gBattlerSpriteIds[battler];
    u8 taskId = CreateTask(Task_UpdateFlashingCircleImpacts, 10);
    u8 spriteId;
    u8 i;

    LoadCompressedSpriteSheet(&gBattleAnimPicTable[136]);
    LoadCompressedSpritePalette(&gBattleAnimPaletteTable[136]);
    gTasks[taskId].data[0] = battler;
    if (red != 0)
    {
        gTasks[taskId].data[1] = 0x1F;
        for (i = 0; i < 10; i++)
        {
            spriteId = CreateSprite(&sFlashingCircleImpactSpriteTemplate, gSprites[battlerSpriteId].x, gSprites[battlerSpriteId].y + 32, 0);
            gSprites[spriteId].data[0] = i * 51;
            gSprites[spriteId].data[1] = -256;
            gSprites[spriteId].invisible = TRUE;
            if (i > 4)
                gSprites[spriteId].data[6] = 21;
        }
    }
    else
    {
        gTasks[taskId].data[1] = 0x7C00;
        for (i = 0; i < 10; i++)
        {
            spriteId = CreateSprite(&sFlashingCircleImpactSpriteTemplate, gSprites[battlerSpriteId].x, gSprites[battlerSpriteId].y - 32, 0);
            gSprites[spriteId].data[0] = i * 51;
            gSprites[spriteId].data[1] = 256;
            gSprites[spriteId].invisible = TRUE;
            if (i > 4)
                gSprites[spriteId].data[6] = 21;
        }
    }
    gSprites[spriteId].data[7] = 1;
    return taskId;
}

static void Task_UpdateFlashingCircleImpacts(u8 taskId)
{
    if (gTasks[taskId].data[2] == 2)
    {
        gTasks[taskId].data[2] = 0;
        BlendPalette(0x100 + gTasks[taskId].data[0] * 16, 16, gTasks[taskId].data[4], gTasks[taskId].data[1]);
        if (gTasks[taskId].data[5] == 0)
        {
            gTasks[taskId].data[4]++;
            if (gTasks[taskId].data[4] > 8)
                gTasks[taskId].data[5] ^= 1;
        }
        else
        {
            u16 var = gTasks[taskId].data[4];

            gTasks[taskId].data[4]--;
            if (gTasks[taskId].data[4] < 0)
            {
                gTasks[taskId].data[4] = var;
                gTasks[taskId].data[5] ^= 1;
                gTasks[taskId].data[3]++;
                if (gTasks[taskId].data[3] == 2)
                    DestroyTask(taskId);
            }
        }
    }
    else
    {
        gTasks[taskId].data[2]++;
    }
}

static void AnimFlashingCircleImpact(struct Sprite *sprite)
{
    if (sprite->data[6] == 0)
    {
        sprite->invisible = FALSE;
        sprite->callback = AnimFlashingCircleImpact_Step;
        AnimFlashingCircleImpact_Step(sprite);
    }
    else
    {
        sprite->data[6] --;
    }
}

static void AnimFlashingCircleImpact_Step(struct Sprite *sprite)
{
    sprite->x2 = Cos(sprite->data[0], 32);
    sprite->y2 = Sin(sprite->data[0], 8);
    if (sprite->data[0] < 128)
        sprite->subpriority = 29;
    else
        sprite->subpriority = 31;
    sprite->data[0] = (sprite->data[0] + 8) & 0xFF;
    sprite->data[5] += sprite->data[1];
    sprite->y2 += sprite->data[5] >> 8;
    sprite->data[2]++;
    if (sprite->data[2] == 52)
    {
        if (sprite->data[7] != 0)
            DestroySpriteAndFreeResources(sprite);
        else
            DestroySprite(sprite);
    }
}

void AnimTask_FrozenIceCube(u8 taskId)
{
    s16 x = GetBattlerSpriteCoord(gBattleAnimTarget, 2) - 32;
    s16 y = GetBattlerSpriteCoord(gBattleAnimTarget, 3) - 36;
    u8 spriteId;

    if (IsContest())
        x -= 6;
    REG_BLDCNT = 0x3F40;
    REG_BLDALPHA = 0x1000;
    spriteId = CreateSprite(&sFrozenIceCubeSpriteTemplate, x, y, 4);
    SetSubspriteTables(&gSprites[spriteId], sFrozenIceCubeSubspriteTable);
    gTasks[taskId].data[15] = spriteId;
    gTasks[taskId].func = AnimTask_FrozenIceCube_Step1;
}

static void AnimTask_FrozenIceCube_Step1(u8 taskId)
{
    gTasks[taskId].data[1]++;
    if (gTasks[taskId].data[1] == 10)
    {
        gTasks[taskId].func = AnimTask_FrozenIceCube_Step2;
        gTasks[taskId].data[1] = 0;
    }
    else
    {
        u8 var = gTasks[taskId].data[1];

        REG_BLDALPHA = ((16 - var) << 8) | var;
    }
}

static void AnimTask_FrozenIceCube_Step2(u8 taskId)
{
    u8 palIndex = IndexOfSpritePaletteTag(0x271A);

    if (gTasks[taskId].data[1]++ > 13)
    {
        gTasks[taskId].data[2]++;
        if (gTasks[taskId].data[2] == 3)
        {
            u16 temp;

            temp = gPlttBufferFaded[0x100 + palIndex * 16 + 13];
            gPlttBufferFaded[0x100 + palIndex * 16 + 13] = gPlttBufferFaded[0x100 + palIndex * 16 + 14];
            gPlttBufferFaded[0x100 + palIndex * 16 + 14] = gPlttBufferFaded[0x100 + palIndex * 16 + 15];
            gPlttBufferFaded[0x100 + palIndex * 16 + 15] = temp;

            gTasks[taskId].data[2] = 0;
            gTasks[taskId].data[3]++;
            if (gTasks[taskId].data[3] == 3)
            {
                gTasks[taskId].data[3] = 0;
                gTasks[taskId].data[1] = 0;
                gTasks[taskId].data[4]++;
                if (gTasks[taskId].data[4] == 2)
                {
                    gTasks[taskId].data[1] = 9;
                    gTasks[taskId].func = AnimTask_FrozenIceCube_Step3;
                }
            }
        }
    }
}

static void AnimTask_FrozenIceCube_Step3(u8 taskId)
{
    gTasks[taskId].data[1]--;
    if (gTasks[taskId].data[1] == -1)
    {
        gTasks[taskId].func = AnimTask_FrozenIceCube_Step4;
        gTasks[taskId].data[1] = 0;
    }
    else
    {
        u8 var = gTasks[taskId].data[1];

        REG_BLDALPHA = ((16 - var) << 8) | var;
    }
}

static void AnimTask_FrozenIceCube_Step4(u8 taskId)
{
    gTasks[taskId].data[1]++;
    if (gTasks[taskId].data[1] == 37)
    {
        u8 spriteId = gTasks[taskId].data[15];

        FreeSpriteOamMatrix(&gSprites[spriteId]);
        DestroySprite(&gSprites[spriteId]);
    }
    else if (gTasks[taskId].data[1] == 39)
    {
        REG_BLDCNT = 0;
        REG_BLDALPHA = 0;
        DestroyAnimVisualTask(taskId);
    }
}

void AnimTask_StatsChange(u8 taskId)
{
    bool16 goesDown;
    s16 animStatId;
    bool16 sharply = 0;

    switch (gBattleAnimationInfo->animArg)
    {
    case 15:  goesDown = 0;  animStatId = 0;  break;
    case 16:  goesDown = 0;  animStatId = 1;  break;
    case 17:  goesDown = 0;  animStatId = 3;  break;
    case 18:  goesDown = 0;  animStatId = 5;  break;
    case 19:  goesDown = 0;  animStatId = 6;  break;
    case 20:  goesDown = 0;  animStatId = 2;  break;
    case 21:  goesDown = 0;  animStatId = 4;  break;
    case 22:  goesDown = 1;  animStatId = 0;  break;
    case 23:  goesDown = 1;  animStatId = 1;  break;
    case 24:  goesDown = 1;  animStatId = 3;  break;
    case 25:  goesDown = 1;  animStatId = 5;  break;
    case 26:  goesDown = 1;  animStatId = 6;  break;
    case 27:  goesDown = 1;  animStatId = 2;  break;
    case 28:  goesDown = 1;  animStatId = 4;  break;
    case 39:  goesDown = 0;  animStatId = 0;     sharply = 1;  break;
    case 40:  goesDown = 0;  animStatId = 1;     sharply = 1;  break;
    case 41:  goesDown = 0;  animStatId = 3;     sharply = 1;  break;
    case 42:  goesDown = 0;  animStatId = 5;     sharply = 1;  break;
    case 43:  goesDown = 0;  animStatId = 6;     sharply = 1;  break;
    case 44:  goesDown = 0;  animStatId = 2;     sharply = 1;  break;
    case 45:  goesDown = 0;  animStatId = 4;     sharply = 1;  break;
    case 46:  goesDown = 1;  animStatId = 0;     sharply = 1;  break;
    case 47:  goesDown = 1;  animStatId = 1;     sharply = 1;  break;
    case 48:  goesDown = 1;  animStatId = 3;     sharply = 1;  break;
    case 49:  goesDown = 1;  animStatId = 5;     sharply = 1;  break;
    case 50:  goesDown = 1;  animStatId = 6;     sharply = 1;  break;
    case 51:  goesDown = 1;  animStatId = 2;     sharply = 1;  break;
    case 52:  goesDown = 1;  animStatId = 4;     sharply = 1;  break;
    case 55:  goesDown = 0;  animStatId = 0xFF;  sharply = 0;  break;
    case 56:  goesDown = 0;  animStatId = 0xFF;  sharply = 1;  break;
    case 57:  goesDown = 1;  animStatId = 0xFF;  sharply = 0;  break;
    case 58:  goesDown = 1;  animStatId = 0xFF;  sharply = 1;  break;

    default:
        DestroyAnimVisualTask(taskId);
        return;
    }

    gBattleAnimArgs[0] = goesDown;
    gBattleAnimArgs[1] = animStatId;
    gBattleAnimArgs[2] = 0;
    gBattleAnimArgs[3] = 0;
    gBattleAnimArgs[4] = sharply;
    gTasks[taskId].func = InitStatsChangeAnimation;
    InitStatsChangeAnimation(taskId);
}

void LaunchStatusAnimation(u8 battler, u8 statusAnimId)
{
    u8 taskId;

    gBattleAnimAttacker = battler;
    gBattleAnimTarget = battler;
    LaunchBattleAnimation(gBattleAnims_StatusConditions, statusAnimId, 0);
    taskId = CreateTask(Task_DoStatusAnimation, 10);
    gTasks[taskId].data[0] = battler;
}

static void Task_DoStatusAnimation(u8 taskId)
{
    gAnimScriptCallback();
    if (!gAnimScriptActive)
    {
        gBattleHealthBoxInfo[gTasks[taskId].data[0]].statusAnimActive = 0;
        DestroyTask(taskId);
    }
}
