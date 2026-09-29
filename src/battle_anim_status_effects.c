#include "global.h"
#include "battle.h"
#include "battle_anim.h"
#include "blend_palette.h"
#include "decompress.h"
#include "palette.h"
#include "sprite.h"
#include "task.h"
#include "trig.h"
#include "ewram.h"
#include "constants/rgb.h"

extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;
extern s16 gBattleAnimArgs[];
extern u8 gBattlerSpriteIds[];
extern const struct CompressedSpriteSheet gBattleAnimPicTable[];
extern const struct CompressedSpritePalette gBattleAnimPaletteTable[];
extern const u8 *const gBattleAnims_StatusConditions[];
extern const struct OamData gOamData_AffineOff_ObjBlend_64x64;
extern const struct OamData gOamData_AffineOff_ObjNormal_8x8;

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
static void AnimTask_FrozenIceCube_Step1(u8);
static void AnimTask_FrozenIceCube_Step2(u8);
static void AnimTask_FrozenIceCube_Step3(u8);
static void AnimTask_FrozenIceCube_Step4(u8);
static void Task_DoStatusAnimation(u8 taskId);

u8 Task_FlashingCircleImpacts(u8 battler, bool8 red)
{
    u8 battlerSpriteId = gBattlerSpriteIds[battler];
    u8 taskId = CreateTask(Task_UpdateFlashingCircleImpacts, 10);
    u8 spriteId;
    u8 i;

    LoadCompressedSpriteSheet(&gBattleAnimPicTable[GET_TRUE_SPRITE_INDEX(ANIM_TAG_CIRCLE_IMPACT)]);
    LoadCompressedSpritePalette(&gBattleAnimPaletteTable[GET_TRUE_SPRITE_INDEX(ANIM_TAG_CIRCLE_IMPACT)]);
    gTasks[taskId].data[0] = battler;
    if (red)
    {
        gTasks[taskId].data[1] = RGB_RED;
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
        gTasks[taskId].data[1] = RGB_BLUE;
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
    gSprites[spriteId].data[7] = TRUE;
    return taskId;
}

static void Task_UpdateFlashingCircleImpacts(u8 taskId)
{
    if (gTasks[taskId].data[2] == 2)
    {
        gTasks[taskId].data[2] = 0;
        BlendPalette(OBJ_PLTT_ID(gTasks[taskId].data[0]), 16, gTasks[taskId].data[4], gTasks[taskId].data[1]);
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
    s16 x = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_X_2) - 32;
    s16 y = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_Y_PIC_OFFSET) - 36;
    u8 spriteId;

    if (IsContest())
        x -= 6;
    REG_BLDCNT = BLDCNT_EFFECT_BLEND | BLDCNT_TGT2_ALL;
    REG_BLDALPHA = BLDALPHA_BLEND(0, 16);
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

        REG_BLDALPHA = BLDALPHA_BLEND(var, 16 - var);
    }
}

static void AnimTask_FrozenIceCube_Step2(u8 taskId)
{
    u8 paletteIndex = IndexOfSpritePaletteTag(ANIM_TAG_ICE_CUBE);

    if (gTasks[taskId].data[1]++ > 13)
    {
        gTasks[taskId].data[2]++;
        if (gTasks[taskId].data[2] == 3)
        {
            u16 temp;

            temp = gPlttBufferFaded[OBJ_PLTT_ID(paletteIndex) + 13];
            gPlttBufferFaded[OBJ_PLTT_ID(paletteIndex) + 13] = gPlttBufferFaded[OBJ_PLTT_ID(paletteIndex) + 14];
            gPlttBufferFaded[OBJ_PLTT_ID(paletteIndex) + 14] = gPlttBufferFaded[OBJ_PLTT_ID(paletteIndex) + 15];
            gPlttBufferFaded[OBJ_PLTT_ID(paletteIndex) + 15] = temp;

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

        REG_BLDALPHA = BLDALPHA_BLEND(var, 16 - var);
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
    s16 goesDown;
    s16 animStatId;
    s16 sharply = FALSE;

    // Ruby serializes these IDs from the zero-based statStages index.
    switch (ewram17840.unk0)
    {
    case STAT_ANIM_PLUS1 + STAT_STAGE_ATK - 1:     goesDown = FALSE;  animStatId = STAT_ANIM_PAL_ATK;      break;
    case STAT_ANIM_PLUS1 + STAT_STAGE_DEF - 1:     goesDown = FALSE;  animStatId = STAT_ANIM_PAL_DEF;      break;
    case STAT_ANIM_PLUS1 + STAT_STAGE_SPEED - 1:   goesDown = FALSE;  animStatId = STAT_ANIM_PAL_SPEED;    break;
    case STAT_ANIM_PLUS1 + STAT_STAGE_SPATK - 1:   goesDown = FALSE;  animStatId = STAT_ANIM_PAL_SPATK;    break;
    case STAT_ANIM_PLUS1 + STAT_STAGE_SPDEF - 1:   goesDown = FALSE;  animStatId = STAT_ANIM_PAL_SPDEF;    break;
    case STAT_ANIM_PLUS1 + STAT_STAGE_ACC - 1:     goesDown = FALSE;  animStatId = STAT_ANIM_PAL_ACC;      break;
    case STAT_ANIM_PLUS1 + STAT_STAGE_EVASION - 1: goesDown = FALSE;  animStatId = STAT_ANIM_PAL_EVASION;  break;

    case STAT_ANIM_MINUS1 + STAT_STAGE_ATK - 1:     goesDown = TRUE;  animStatId = STAT_ANIM_PAL_ATK;      break;
    case STAT_ANIM_MINUS1 + STAT_STAGE_DEF - 1:     goesDown = TRUE;  animStatId = STAT_ANIM_PAL_DEF;      break;
    case STAT_ANIM_MINUS1 + STAT_STAGE_SPEED - 1:   goesDown = TRUE;  animStatId = STAT_ANIM_PAL_SPEED;    break;
    case STAT_ANIM_MINUS1 + STAT_STAGE_SPATK - 1:   goesDown = TRUE;  animStatId = STAT_ANIM_PAL_SPATK;    break;
    case STAT_ANIM_MINUS1 + STAT_STAGE_SPDEF - 1:   goesDown = TRUE;  animStatId = STAT_ANIM_PAL_SPDEF;    break;
    case STAT_ANIM_MINUS1 + STAT_STAGE_ACC - 1:     goesDown = TRUE;  animStatId = STAT_ANIM_PAL_ACC;      break;
    case STAT_ANIM_MINUS1 + STAT_STAGE_EVASION - 1: goesDown = TRUE;  animStatId = STAT_ANIM_PAL_EVASION;  break;

    case STAT_ANIM_PLUS2 + STAT_STAGE_ATK - 1:     goesDown = FALSE;  animStatId = STAT_ANIM_PAL_ATK;      sharply = TRUE;  break;
    case STAT_ANIM_PLUS2 + STAT_STAGE_DEF - 1:     goesDown = FALSE;  animStatId = STAT_ANIM_PAL_DEF;      sharply = TRUE;  break;
    case STAT_ANIM_PLUS2 + STAT_STAGE_SPEED - 1:   goesDown = FALSE;  animStatId = STAT_ANIM_PAL_SPEED;    sharply = TRUE;  break;
    case STAT_ANIM_PLUS2 + STAT_STAGE_SPATK - 1:   goesDown = FALSE;  animStatId = STAT_ANIM_PAL_SPATK;    sharply = TRUE;  break;
    case STAT_ANIM_PLUS2 + STAT_STAGE_SPDEF - 1:   goesDown = FALSE;  animStatId = STAT_ANIM_PAL_SPDEF;    sharply = TRUE;  break;
    case STAT_ANIM_PLUS2 + STAT_STAGE_ACC - 1:     goesDown = FALSE;  animStatId = STAT_ANIM_PAL_ACC;      sharply = TRUE;  break;
    case STAT_ANIM_PLUS2 + STAT_STAGE_EVASION - 1: goesDown = FALSE;  animStatId = STAT_ANIM_PAL_EVASION;  sharply = TRUE;  break;

    case STAT_ANIM_MINUS2 + STAT_STAGE_ATK - 1:     goesDown = TRUE;  animStatId = STAT_ANIM_PAL_ATK;      sharply = TRUE;  break;
    case STAT_ANIM_MINUS2 + STAT_STAGE_DEF - 1:     goesDown = TRUE;  animStatId = STAT_ANIM_PAL_DEF;      sharply = TRUE;  break;
    case STAT_ANIM_MINUS2 + STAT_STAGE_SPEED - 1:   goesDown = TRUE;  animStatId = STAT_ANIM_PAL_SPEED;    sharply = TRUE;  break;
    case STAT_ANIM_MINUS2 + STAT_STAGE_SPATK - 1:   goesDown = TRUE;  animStatId = STAT_ANIM_PAL_SPATK;    sharply = TRUE;  break;
    case STAT_ANIM_MINUS2 + STAT_STAGE_SPDEF - 1:   goesDown = TRUE;  animStatId = STAT_ANIM_PAL_SPDEF;    sharply = TRUE;  break;
    case STAT_ANIM_MINUS2 + STAT_STAGE_ACC - 1:     goesDown = TRUE;  animStatId = STAT_ANIM_PAL_ACC;      sharply = TRUE;  break;
    case STAT_ANIM_MINUS2 + STAT_STAGE_EVASION - 1: goesDown = TRUE;  animStatId = STAT_ANIM_PAL_EVASION;  sharply = TRUE;  break;

    case STAT_ANIM_MULTIPLE_PLUS1:  goesDown = FALSE;  animStatId = STAT_ANIM_PAL_MULTIPLE;  sharply = FALSE;  break;
    case STAT_ANIM_MULTIPLE_PLUS2:  goesDown = FALSE;  animStatId = STAT_ANIM_PAL_MULTIPLE;  sharply = TRUE;   break;
    case STAT_ANIM_MULTIPLE_MINUS1: goesDown = TRUE;   animStatId = STAT_ANIM_PAL_MULTIPLE;  sharply = FALSE;  break;
    case STAT_ANIM_MULTIPLE_MINUS2: goesDown = TRUE;   animStatId = STAT_ANIM_PAL_MULTIPLE;  sharply = TRUE;   break;

    default:
        DestroyAnimVisualTask(taskId);
        return;
    }

    gBattleAnimArgs[0] = goesDown;
    gBattleAnimArgs[1] = animStatId;
    gBattleAnimArgs[2] = FALSE;
    gBattleAnimArgs[3] = FALSE;
    gBattleAnimArgs[4] = sharply;
    gTasks[taskId].func = InitStatsChangeAnimation;
    InitStatsChangeAnimation(taskId);
}

void LaunchStatusAnimation(u8 battler, u8 statusAnimId)
{
    u8 taskId;

    gBattleAnimAttacker = battler;
    gBattleAnimTarget = battler;
    LaunchBattleAnimation(gBattleAnims_StatusConditions, statusAnimId, FALSE);
    taskId = CreateTask(Task_DoStatusAnimation, 10);
    gTasks[taskId].data[0] = battler;
}

static void Task_DoStatusAnimation(u8 taskId)
{
    gAnimScriptCallback();
    if (!gAnimScriptActive)
    {
        gBattleHealthBoxInfo[gTasks[taskId].data[0]].statusAnimActive = FALSE;
        DestroyTask(taskId);
    }
}
