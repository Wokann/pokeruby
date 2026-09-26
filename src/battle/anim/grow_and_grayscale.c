#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimTask_GrowAndGrayscale_Step(u8 taskId);

// Scales up the target mon sprite, and sets the palette to grayscale.
// Used in Disable.
// No args.

void AnimTask_GrowAndGrayscale(u8 taskId)
{
    u8 spriteId = GetAnimBattlerSpriteId(ANIM_BATTLER_TARGET);
    PrepareBattlerSpriteForRotScale(spriteId, ST_OAM_OBJ_BLEND);
    SetSpriteRotScale(spriteId, 0xD0, 0xD0, 0);
    SetGrayscaleOrOriginalPalette(gSprites[spriteId].oam.paletteNum + 16, FALSE);
    gTasks[taskId].data[0] = 80;
    gTasks[taskId].func = AnimTask_GrowAndGrayscale_Step;
}

static void AnimTask_GrowAndGrayscale_Step(u8 taskId)
{
    if (--gTasks[taskId].data[0] == -1)
    {
        u8 spriteId = GetAnimBattlerSpriteId(ANIM_BATTLER_TARGET);
        ResetSpriteRotScale(spriteId);
        SetGrayscaleOrOriginalPalette(gSprites[spriteId].oam.paletteNum + 16, TRUE);
        DestroyAnimVisualTask(taskId);
    }
}
