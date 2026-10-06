#include "global.h"
#include "battle_anim.h"
#include "random.h"
#include "sprite.h"
#include "task.h"

static void AnimRainDrop(struct Sprite *sprite);
static void AnimRainDrop_Step(struct Sprite *sprite);

// rain (spawns and animates raindrops)
// Used in Rain Dance and general rain animation.

static const u8 sUnusedWater_Gfx[] = INCBIN_U8("graphics/battle_anims/unused/water_gfx.4bpp");
static const u8 sUnusedWater[] = INCBIN_U8("graphics/battle_anims/unused/water.bin");

static const union AnimCmd sAnim_RainDrop[] =
{
    ANIMCMD_FRAME(0, 2),
    ANIMCMD_FRAME(8, 2),
    ANIMCMD_FRAME(16, 2),
    ANIMCMD_FRAME(24, 6),
    ANIMCMD_FRAME(32, 2),
    ANIMCMD_FRAME(40, 2),
    ANIMCMD_FRAME(48, 2),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_RainDrop[] =
{
    sAnim_RainDrop,
};

const struct SpriteTemplate gRainDropSpriteTemplate =
{
    .tileTag = 10115,
    .paletteTag = 10115,
    .oam = &gOamData_AffineOff_ObjNormal_16x32,
    .anims = sAnims_RainDrop,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimRainDrop,
};

void AnimTask_CreateRaindrops(u8 taskId)
{
    if (gTasks[taskId].data[0] == 0)
    {
        gTasks[taskId].data[1] = gBattleAnimArgs[0];
        gTasks[taskId].data[2] = gBattleAnimArgs[1];
        gTasks[taskId].data[3] = gBattleAnimArgs[2];
    }

    gTasks[taskId].data[0]++;

    if (gTasks[taskId].data[0] % gTasks[taskId].data[2] == 1)
    {
        u8 x = Random() % 240;
        u8 y = Random() % 80;
        CreateSprite(&gRainDropSpriteTemplate, x, y, 4);
    }

    if (gTasks[taskId].data[0] == gTasks[taskId].data[3])
    {
        DestroyAnimVisualTask(taskId);
    }
}

static void AnimRainDrop(struct Sprite *sprite)
{
    sprite->callback = AnimRainDrop_Step;
}

static void AnimRainDrop_Step(struct Sprite *sprite)
{
    if (++sprite->data[0] <= 13)
    {
        sprite->x2++;
        sprite->y2 += 4;
    }

    if (sprite->animEnded)
    {
        DestroySprite(sprite);
    }
}
