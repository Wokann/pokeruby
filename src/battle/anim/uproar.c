#include "global.h"
#include "battle_anim.h"
#include "sprite.h"
#include "task.h"


static void sub_80D2D3C(u8);

const union AffineAnimCmd gSpriteAffineAnim_83D7CA8[] =
{
    AFFINEANIMCMD_FRAME(-12, 8, 0, 4),
    AFFINEANIMCMD_FRAME(20, -20, 0, 4),
    AFFINEANIMCMD_FRAME(-8, 12, 0, 4),
    AFFINEANIMCMD_END,
};

void sub_80D2CF8(u8 taskId)
{
    u8 spriteId = GetAnimBattlerSpriteId(gBattleAnimArgs[0]);

    PrepareAffineAnimInTaskData(&gTasks[taskId], spriteId, gSpriteAffineAnim_83D7CA8);
    gTasks[taskId].func = sub_80D2D3C;
}

static void sub_80D2D3C(u8 taskId)
{
    if (!RunAffineAnimFromTaskData(&gTasks[taskId]))
        DestroyAnimVisualTask(taskId);
}
