#include "global.h"
#include "battle_anim.h"
#include "trig.h"
#include "sound.h"

extern u8 gBattlerSpriteIds[];

static void AnimTask_Withdraw_Step(u8 taskId);

// withdraw (where a Pokemon leans inward to mimic the effect of withdrawing into a shell.)
// Used in Withdraw.

void AnimTask_Withdraw(u8 taskId)
{
    PrepareBattlerSpriteForRotScale(gBattlerSpriteIds[gBattleAnimAttacker], 0);
    gTasks[taskId].func = AnimTask_Withdraw_Step;
}

static void AnimTask_Withdraw_Step(u8 taskId)
{
    u8 spriteId = gBattlerSpriteIds[gBattleAnimAttacker];
    s16 rotation;
    if (GetBattlerSide(gBattleAnimAttacker) == 0)
    {
        rotation = -gTasks[taskId].data[0];
    }
    else
    {
        rotation = gTasks[taskId].data[0];
    }

    SetSpriteRotScale(spriteId, 0x100, 0x100, rotation);
    if (gTasks[taskId].data[1] == 0)
    {
        gTasks[taskId].data[0] += 0xB0;
        gSprites[spriteId].y2++;
    }
    else if (gTasks[taskId].data[1] == 1)
    {
        if (++gTasks[taskId].data[3] == 0x1E)
            gTasks[taskId].data[1] = 2;

        return;
    }
    else
    {
        gTasks[taskId].data[0] -= 0xB0;
        gSprites[spriteId].y2--;
    }

    SetBattlerSpriteYOffsetFromRotation(spriteId);
    if (gTasks[taskId].data[0] == 0xF20 || gTasks[taskId].data[0] == 0)
    {
        if (gTasks[taskId].data[1] == 2)
        {
            ResetSpriteRotScale(spriteId);
            DestroyAnimVisualTask(taskId);
        }
        else
        {
            gTasks[taskId].data[1]++;
        }
    }
}
