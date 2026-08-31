#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimSpeedDust(struct Sprite *sprite);
static void AnimTask_AttackerStretchAndDisappear_Step(u8 taskId);
static void AnimTask_ExtremeSpeedImpact_Step(u8 taskId);
static void AnimTask_ExtremeSpeedMonReappear_Step(u8 taskId);
static void AnimTask_SpeedDust_Step(u8 taskId);

// espeed (the Pokemon's width decreases as the sprite becomes vertically compressed)
// Used in Extremespeed.

const union AffineAnimCmd gStretchAttackerAffineAnimCmds[] =
{
    AFFINEANIMCMD_FRAME(96, -13, 0, 8),
    AFFINEANIMCMD_END,
};

const union AnimCmd gSpeedDustAnimCmds[] =
{
    ANIMCMD_FRAME(0, 3),
    ANIMCMD_FRAME(4, 3),
    ANIMCMD_FRAME(8, 3),
    ANIMCMD_FRAME(4, 3),
    ANIMCMD_FRAME(0, 3),
    ANIMCMD_END,
};

const union AnimCmd *const gSpeedDustAnimTable[] =
{
    gSpeedDustAnimCmds,
};

const struct SpriteTemplate gSpeedDustSpriteTemplate =
{
    .tileTag = ANIM_TAG_SPEED_DUST,
    .paletteTag = ANIM_TAG_SPEED_DUST,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = gSpeedDustAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimSpeedDust,
};

const s8 gSpeedDustPosTable[][2] =
{
    {30, 28},
    {-20, 24},
    {16, 26},
    {-10, 28},
};

// apply espeed
void AnimTask_AttackerStretchAndDisappear(u8 taskId)
{
    struct Task* task = &gTasks[taskId];
    u8 spriteId = GetAnimBattlerSpriteId(ANIM_BATTLER_ATTACKER);
    task->data[0] = spriteId;
    PrepareAffineAnimInTaskData(task, spriteId, gStretchAttackerAffineAnimCmds);
    task->func = AnimTask_AttackerStretchAndDisappear_Step;
}

static void AnimTask_AttackerStretchAndDisappear_Step(u8 taskId)
{
    struct Task* task = &gTasks[taskId];
    if (RunAffineAnimFromTaskData(task) == 0)
    {
        gSprites[task->data[0]].y2 = 0;
        gSprites[task->data[0]].invisible = TRUE;
        DestroyAnimVisualTask(taskId);
    }
}

// espeed hit effect
void AnimTask_ExtremeSpeedImpact(u8 taskId)
{
    struct Task* task = &gTasks[taskId];
    task->data[0] = 0;
    task->data[1] = 0;
    task->data[2] = 0;
    task->data[3] = 0;
    task->data[12] = 3;
    if (GetBattlerSide(gBattleAnimTarget) == 0)
    {
        task->data[13] = 0xFFFF;
        task->data[14] = 8;
    }
    else
    {
        task->data[13] = 1;
        task->data[14] = -8;
    }

    task->data[15] = GetAnimBattlerSpriteId(ANIM_BATTLER_TARGET);
    task->func = AnimTask_ExtremeSpeedImpact_Step;
}

static void AnimTask_ExtremeSpeedImpact_Step(u8 taskId)
{
    struct Task* task = &gTasks[taskId];

    switch (task->data[0])
    {
    case 0:
        gSprites[task->data[15]].x2 += task->data[14];
        task->data[1] = 0;
        task->data[2] = 0;
        task->data[3] = 0;
        task->data[0]++;
        break;
    case 1:
        if (++task->data[1] > 1)
        {
            task->data[1] = 0;
            task->data[2]++;
            if (task->data[2] & 1)
                gSprites[task->data[15]].x2 += 6;
            else
                gSprites[task->data[15]].x2 -= 6;

            if (++task->data[3] > 4)
            {
                if (task->data[2] & 1)
                    gSprites[task->data[15]].x2 -= 6;

                task->data[0]++;
            }
        }
        break;
    case 2:
        if (--task->data[12] != 0)
            task->data[0] = 0;
        else
            task->data[0]++;
        break;
    case 3:
        gSprites[task->data[15]].x2 += task->data[13];
        if (gSprites[task->data[15]].x2 == 0)
            DestroyAnimVisualTask(taskId);
        break;
    }
}

// espeed poke flicker in
void AnimTask_ExtremeSpeedMonReappear(u8 taskId)
{
    struct Task* task = &gTasks[taskId];
    task->data[0] = 0;
    task->data[1] = 0;
    task->data[2] = 0;
    task->data[3] = 0;
    task->data[4] = 1;
    task->data[13] = 14;
    task->data[14] = 2;
    task->data[15] = GetAnimBattlerSpriteId(ANIM_BATTLER_ATTACKER);
    task->func = AnimTask_ExtremeSpeedMonReappear_Step;
}

static void AnimTask_ExtremeSpeedMonReappear_Step(u8 taskId)
{
    struct Task* task = &gTasks[taskId];
    if (task->data[0] == 0 && ++task->data[1] > task->data[4])
    {
        task->data[1] = 0;
        if (++task->data[2] & 1)
            gSprites[task->data[15]].invisible = FALSE;
        else
            gSprites[task->data[15]].invisible = TRUE;

        if (++task->data[3] >= task->data[13])
        {
            if (++task->data[4] < task->data[14])
            {
                task->data[1] = 0;
                task->data[2] = 0;
                task->data[3] = 0;
            }
            else
            {
                gSprites[task->data[15]].invisible = FALSE;
                DestroyAnimVisualTask(taskId);
            }
        }
    }
}

// espeed smoke flicker
void AnimTask_SpeedDust(u8 taskId)
{
    struct Task* task = &gTasks[taskId];
    task->data[0] = 0;
    task->data[1] = 4;
    task->data[2] = 0;
    task->data[3] = 0;
    task->data[4] = 0;
    task->data[5] = 0;
    task->data[6] = 0;
    task->data[7] = 0;
    task->data[8] = 0;
    task->data[13] = 0;
    task->data[14] = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_X);
    task->data[15] = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_Y);
    task->func = AnimTask_SpeedDust_Step;
}

static void AnimTask_SpeedDust_Step(u8 taskId)
{
    struct Task* task = &gTasks[taskId];
    switch (task->data[8])
    {
    case 0:
        if (++task->data[4] > 1)
        {
            task->data[4] = 0;
            task->data[5] = (task->data[5] + 1) & 1;
            if (++task->data[6] > 20)
            {
                if (task->data[7] == 0)
                {
                    task->data[6] = 0;
                    task->data[8] = 1;
                }
                else
                    task->data[8] = 2;
            }
        }
        break;
    case 1:
        task->data[5] = 0;
        if (++task->data[4] > 20)
        {
            task->data[7] = 1;
            task->data[8] = 0;
        }
        break;
    case 2:
        task->data[5] = 1;
        break;
    }

    switch (task->data[0])
    {
    case 0:
        if (++task->data[1] > 4)
        {
            u8 spriteId;
            task->data[1] = 0;
            spriteId = CreateSprite(&gSpeedDustSpriteTemplate, task->data[14], task->data[15], 0);
            if (spriteId != MAX_SPRITES)
            {
                gSprites[spriteId].data[0] = taskId;
                gSprites[spriteId].data[1] = 13;
                gSprites[spriteId].x2 = gSpeedDustPosTable[task->data[2]][0];
                gSprites[spriteId].y2 = gSpeedDustPosTable[task->data[2]][1];
                task->data[13]++;
                if (++task->data[2] > 3)
                {
                    task->data[2] = 0;
                    if (++task->data[3] > 5)
                        task->data[0]++;
                }
            }
        }
        break;
    case 1:
        if (task->data[13] == 0)
            DestroyAnimVisualTask(taskId);
        break;
    }
}

static void AnimSpeedDust(struct Sprite *sprite)
{
    sprite->invisible = gTasks[sprite->data[0]].data[5];
    if (sprite->animEnded)
    {
        gTasks[sprite->data[0]].data[sprite->data[1]]--;
        DestroySprite(sprite);
    }
}
