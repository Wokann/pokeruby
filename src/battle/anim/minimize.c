#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimTask_Minimize_Step(u8 taskId);
static void CreateMinimizeSprite(struct Task *task, u8 taskId);
static void ClonedMinimizeSprite_Step(struct Sprite *sprite);

// Shrinks and grows the attacking mon several times. Also creates transparent versions of the
// mon's sprite while it is shrinking.
// No args.

void AnimTask_Minimize(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    u8 spriteId = GetAnimBattlerSpriteId(ANIM_BATTLER_ATTACKER);
    task->data[0] = spriteId;
    PrepareBattlerSpriteForRotScale(spriteId, ST_OAM_OBJ_NORMAL);
    task->data[1] = 0;
    task->data[2] = 0;
    task->data[3] = 0;
    task->data[4] = 0x100;
    task->data[5] = 0;
    task->data[6] = 0;
    task->data[7] = GetBattlerSpriteSubpriority(gBattleAnimAttacker);
    task->func = AnimTask_Minimize_Step;
}

static void AnimTask_Minimize_Step(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    switch (task->data[1])
    {
    case 0:
        if (task->data[2] == 0 || task->data[2] == 3 || task->data[2] == 6)
            CreateMinimizeSprite(task, taskId);
        task->data[2]++;
        task->data[4] += 0x28;
        SetSpriteRotScale(task->data[0], task->data[4], task->data[4], 0);
        SetBattlerSpriteYOffsetFromYScale(task->data[0]);
        if (task->data[2] == 32)
        {
            task->data[5]++;
            task->data[1]++;
        }
        break;
    case 1:
        if (task->data[6] == 0)
        {
            if (task->data[5] == 3)
            {
                task->data[2] = 0;
                task->data[1] = 3;
            }
            else
            {
                task->data[2] = 0;
                task->data[3] = 0;
                task->data[4] = 0x100;
                SetSpriteRotScale(task->data[0], task->data[4], task->data[4], 0);
                SetBattlerSpriteYOffsetFromYScale(task->data[0]);
                task->data[1] = 2;
            }
        }
        break;
    case 2:
        task->data[1] = 0;
        break;
    case 3:
        if (++task->data[2] > 32)
        {
            task->data[2] = 0;
            task->data[1]++;
        }
        break;
    case 4:
        task->data[2] += 2;
        task->data[4] -= 0x50;
        SetSpriteRotScale(task->data[0], task->data[4], task->data[4], 0);
        SetBattlerSpriteYOffsetFromYScale(task->data[0]);
        if (task->data[2] == 32)
        {
            task->data[2] = 0;
            task->data[1]++;
        }
        break;
    case 5:
        ResetSpriteRotScale(task->data[0]);
        gSprites[task->data[15]].y2 = 0;
        DestroyAnimVisualTask(taskId);
        break;
    }
}

static void CreateMinimizeSprite(struct Task *task, u8 taskId)
{
    s16 matrixNum; // u16 in Emerald
    s16 spriteId = CloneBattlerSpriteWithBlend(ANIM_BATTLER_ATTACKER);

    if (spriteId >= 0)
    {
        matrixNum = AllocOamMatrix();
        if (matrixNum == 0xFF)
        {
            DestroySpriteWithActiveSheet(&gSprites[spriteId]);
            return;
        }
        else
        {
            gSprites[spriteId].oam.objMode = ST_OAM_OBJ_BLEND;
            gSprites[spriteId].oam.affineMode = ST_OAM_AFFINE_DOUBLE;
            gSprites[spriteId].affineAnimPaused = TRUE;
            gSprites[spriteId].oam.matrixNum = matrixNum;
            gSprites[spriteId].subpriority = task->data[7] - task->data[3];
            task->data[3]++;
            task->data[6]++;
            gSprites[spriteId].data[0] = 16;
            gSprites[spriteId].data[1] = taskId;
            gSprites[spriteId].data[2] = 6;
            gSprites[spriteId].callback = ClonedMinimizeSprite_Step;
            SetSpriteRotScale(spriteId, task->data[4], task->data[4], 0);
            gSprites[spriteId].oam.affineMode = ST_OAM_AFFINE_NORMAL;
            CalcCenterToCornerVec(&gSprites[spriteId], gSprites[spriteId].oam.shape, gSprites[spriteId].oam.size, gSprites[spriteId].oam.affineMode);
        }
    }
}

static void ClonedMinimizeSprite_Step(struct Sprite *sprite)
{
    if (--sprite->data[0] == 0)
    {
        gTasks[sprite->data[1]].data[sprite->data[2]]--;
        FreeOamMatrix(sprite->oam.matrixNum);
        DestroySpriteWithActiveSheet(sprite);
    }
}
