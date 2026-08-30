#include "global.h"
#include "battle.h"
#include "battle_anim.h"

extern s16 gBattleAnimArgs[];

// fury_cutter (updates the direction and count of the fury cutter animation)
// Used in Fury Cutter.

void AnimTask_IsFuryCutterHitRight(u8 taskId)
{
    gBattleAnimArgs[ARG_RET_ID] = gAnimDisableStructPtr->furyCutterCounter & 1;
    DestroyAnimVisualTask(taskId);
}

void AnimTask_GetFuryCutterHitCount(u8 taskId)
{
    gBattleAnimArgs[ARG_RET_ID] = gAnimDisableStructPtr->furyCutterCounter;
    DestroyAnimVisualTask(taskId);
}
