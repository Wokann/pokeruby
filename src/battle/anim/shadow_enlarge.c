#include "global.h"
#include "battle_anim.h"
#include "trig.h"
#include "sound.h"

static void AnimTask_GrowAndGrayscale_Step(u8 taskId);

// Scales up the target mon sprite, and sets the palette to grayscale.
// Used in MOVE_DISABLE.
// No args.

void AnimTask_GrowAndGrayscale(u8 taskId)
{
    u8 spriteId = GetAnimBattlerSpriteId(1);
    PrepareBattlerSpriteForRotScale(spriteId, 1);
    SetSpriteRotScale(spriteId, 0xD0, 0xD0, 0);
    SetGrayscaleOrOriginalPalette(gSprites[spriteId].oam.paletteNum + 16, 0);
    gTasks[taskId].data[0] = 0x50;
    gTasks[taskId].func = AnimTask_GrowAndGrayscale_Step;
}

static void AnimTask_GrowAndGrayscale_Step(u8 taskId)
{
    if (--gTasks[taskId].data[0] == -1)
    {
        u8 spriteId = GetAnimBattlerSpriteId(1);
        ResetSpriteRotScale(spriteId);
        SetGrayscaleOrOriginalPalette(gSprites[spriteId].oam.paletteNum + 16, 1);
        DestroyAnimVisualTask(taskId);
    }
}
