#include "global.h"
#include "battle_anim.h"
#include "trig.h"
#include "sound.h"


static void sub_80D0428(u8 taskId);

// shadow_enlarge (the magnifying-like shadow over the Pokemon effect)
// Used in Disable.

void sub_80D03C4(u8 taskId)
{
    u8 spriteId = GetAnimBattlerSpriteId(1);
    PrepareBattlerSpriteForRotScale(spriteId, 1);
    SetSpriteRotScale(spriteId, 0xD0, 0xD0, 0);
    SetGrayscaleOrOriginalPalette(gSprites[spriteId].oam.paletteNum + 16, 0);
    gTasks[taskId].data[0] = 0x50;
    gTasks[taskId].func = sub_80D0428;
}

void sub_80D0428(u8 taskId)
{
    if (--gTasks[taskId].data[0] == -1)
    {
        u8 spriteId = GetAnimBattlerSpriteId(1);
        ResetSpriteRotScale(spriteId);
        SetGrayscaleOrOriginalPalette(gSprites[spriteId].oam.paletteNum + 16, 1);
        DestroyAnimVisualTask(taskId);
    }
}
