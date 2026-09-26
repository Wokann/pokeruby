#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"
#include "palette.h"
#include "blend_palette.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimTask_DoubleTeam_Step(u8 taskId);
static void AnimDoubleTeam(struct Sprite *sprite);

// Used by Double Team.

void AnimTask_DoubleTeam(u8 taskId)
{
    u16 i;
    int obj;
    u16 r3;
    u16 r4;
    struct Task* task = &gTasks[taskId];
    task->data[0] = GetAnimBattlerSpriteId(0);
    task->data[1] = AllocSpritePalette(ANIM_TAG_BENT_SPOON);
    r3 = (task->data[1] * 16) + 0x100;
    r4 = (gSprites[task->data[0]].oam.paletteNum + 16) << 4;
    for (i = 1; i < 16; i++)
    {
        gPlttBufferUnfaded[r3 + i] = gPlttBufferUnfaded[r4 + i];
    }

    BlendPalette(r3, 16, 11, RGB_BLACK);
    task->data[3] = 0;
    i = 0;
    while (i <= 1 && (obj = CloneBattlerSpriteWithBlend(0)) >= 0)
    {
        gSprites[obj].oam.paletteNum = task->data[1];
        gSprites[obj].data[0] = 0;
        gSprites[obj].data[1] = i << 7;
        gSprites[obj].data[2] = taskId;
        gSprites[obj].callback = AnimDoubleTeam;
        task->data[3]++;
        i++;
    }

    task->func = AnimTask_DoubleTeam_Step;
    if (GetBattlerSpriteBGPriorityRank(gBattleAnimAttacker) == 1)
    {
        REG_DISPCNT &= 0xFDFF;
    }
    else
    {
        REG_DISPCNT &= 0xFBFF;
    }
}

static void AnimTask_DoubleTeam_Step(u8 taskId)
{
    struct Task* task = &gTasks[taskId];
    if (!task->data[3])
    {
        if (GetBattlerSpriteBGPriorityRank(gBattleAnimAttacker) == 1)
            REG_DISPCNT |= 0x200;
        else
            REG_DISPCNT |= 0x400;

        FreeSpritePaletteByTag(ANIM_TAG_BENT_SPOON);
        DestroyAnimVisualTask(taskId);
    }
}

static void AnimDoubleTeam(struct Sprite *sprite)
{
    if (++sprite->data[3] > 1)
    {
        sprite->data[3] = 0;
        sprite->data[0]++;
    }

    if (sprite->data[0] > 0x40)
    {
        gTasks[sprite->data[2]].data[3]--;
        DestroySpriteWithActiveSheet(sprite);
    }
    else
    {
        sprite->data[4] = gSineTable[sprite->data[0]] / 6;
        sprite->data[5] = gSineTable[sprite->data[0]] / 13;
        sprite->data[1] = (sprite->data[1] + sprite->data[5]) & 0xFF;
        sprite->x2 = Sin(sprite->data[1], sprite->data[4]);
    }
}
