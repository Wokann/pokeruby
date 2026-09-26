#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"
#include "constants/battle.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimTask_ThrashMoveMonHorizontal_Step(u8 taskId);
static void AnimTask_ThrashMoveMonVertical_Step(u8 taskId);

// thrashing (the movement of the Pokemon left/right repeatedly, with up/down movements below.)
// Used by Thrash.

const union AffineAnimCmd gThrashMoveMonAffineAnimCmds[] =
{
    AFFINEANIMCMD_FRAME(-10, 9, 0, 7),
    AFFINEANIMCMD_FRAME(20, -20, 0, 7),
    AFFINEANIMCMD_FRAME(-20, 20, 0, 7),
    AFFINEANIMCMD_FRAME(10, -9, 0, 7),
    AFFINEANIMCMD_LOOP(2),
    AFFINEANIMCMD_END,
};

// left/right movements
void AnimTask_ThrashMoveMonHorizontal(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    u8 spriteId = GetAnimBattlerSpriteId(ANIM_BATTLER_ATTACKER);
    task->data[0] = spriteId;
    task->data[1] = 0;
    PrepareAffineAnimInTaskData(task, spriteId, gThrashMoveMonAffineAnimCmds);
    task->func = AnimTask_ThrashMoveMonHorizontal_Step;
}

static void AnimTask_ThrashMoveMonHorizontal_Step(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    if (!RunAffineAnimFromTaskData(task))
        DestroyAnimVisualTask(taskId);
}

// up/down movements
void AnimTask_ThrashMoveMonVertical(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    task->data[0] = GetAnimBattlerSpriteId(ANIM_BATTLER_ATTACKER);
    task->data[1] = 0;
    task->data[2] = 4;
    task->data[3] = 7;
    task->data[4] = 3;
    task->data[5] = gSprites[task->data[0]].x;
    task->data[6] = gSprites[task->data[0]].y;
    task->data[7] = 0;
    task->data[8] = 0;
    task->data[9] = 2;
    if (GetBattlerSide(gBattleAnimAttacker) == B_SIDE_OPPONENT)
        task->data[2] *= -1;

    task->func = AnimTask_ThrashMoveMonVertical_Step;
}

static void AnimTask_ThrashMoveMonVertical_Step(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    if (++task->data[7] > 2)
    {
        task->data[7] = 0;
        task->data[8]++;
        if ((task->data[8] & 1) != 0)
        {
            gSprites[task->data[0]].y += task->data[9];
        }
        else
        {
            gSprites[task->data[0]].y -= task->data[9];
        }
    }
    switch (task->data[1])
    {
    case 0:
        gSprites[task->data[0]].x += task->data[2];
        if (--task->data[3] == 0)
        {
            task->data[3] = 14;
            task->data[1] = 1;
        }
        break;
    case 1:
        gSprites[task->data[0]].x -= task->data[2];
        if (--task->data[3] == 0)
        {
            task->data[3] = 7;
            task->data[1] = 2;
        }
        break;
    case 2:
        gSprites[task->data[0]].x += task->data[2];
        if (--task->data[3] == 0)
        {
            if (--task->data[4] != 0)
            {
                task->data[3] = 7;
                task->data[1] = 0;
            }
            else
            {
                if ((task->data[8] & 1) != 0)
                {
                    gSprites[task->data[0]].y -= task->data[9];
                }
                DestroyAnimVisualTask(taskId);
            }
        }
        break;
    }
}
