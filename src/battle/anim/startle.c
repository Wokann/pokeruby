#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

// startle (the pokemon sprite shrivels upward and restores after a brief time.)
// Used in Fake Out, Trick, and Astonish.

static const union AffineAnimCmd sAffineAnims_StretchBattlerUp[] =
{
    AFFINEANIMCMD_FRAME(0x000A, 0xFFF3, 0x00, 0x0A),
    AFFINEANIMCMD_FRAME(0xFFF6, 0x000D, 0x00, 0x0A),
    AFFINEANIMCMD_END,
};

// opponent
void AnimTask_StretchTargetUp(u8 taskId)
{
    u8 spriteId = GetAnimBattlerSpriteId(ANIM_BATTLER_TARGET);
    if (++gTasks[taskId].data[0] == 1)
    {
        PrepareAffineAnimInTaskData(&gTasks[taskId], GetAnimBattlerSpriteId(ANIM_BATTLER_TARGET), sAffineAnims_StretchBattlerUp);
        gSprites[spriteId].x2 = 4;
    }
    else
    {
        gSprites[spriteId].x2 = -gSprites[spriteId].x2;
        if (RunAffineAnimFromTaskData(&gTasks[taskId]) == 0)
        {
            gSprites[spriteId].x2 = 0;
            gSprites[spriteId].y2 = 0;
            DestroyAnimVisualTask(taskId);
        }
    }
}

// player
void AnimTask_StretchAttackerUp(u8 taskId)
{
    u8 spriteId = GetAnimBattlerSpriteId(ANIM_BATTLER_ATTACKER);
    if (++gTasks[taskId].data[0] == 1)
    {
        PrepareAffineAnimInTaskData(&gTasks[taskId], GetAnimBattlerSpriteId(ANIM_BATTLER_ATTACKER), sAffineAnims_StretchBattlerUp);
        gSprites[spriteId].x2 = 4;
    }
    else
    {
        gSprites[spriteId].x2 = -gSprites[spriteId].x2;
        if (RunAffineAnimFromTaskData(&gTasks[taskId]) == 0)
        {
            gSprites[spriteId].x2 = 0;
            gSprites[spriteId].y2 = 0;
            DestroyAnimVisualTask(taskId);
        }
    }
}
