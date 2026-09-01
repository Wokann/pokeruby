#include "global.h"
#include "battle_anim.h"
#include "random.h"
#include "sprite.h"
#include "task.h"

extern s16 gBattleAnimArgs[8];

static void AnimRainDrop(struct Sprite *sprite);
static void AnimRainDrop_Step(struct Sprite *sprite);

// rain (spawns and animates raindrops)
// Used in Rain Dance and general rain animation.

static const u8 sUnusedWater_Gfx[] = INCBIN_U8("graphics/unknown/unknown_3D7D8C.4bpp");
static const u8 sUnusedWater[] = INCBIN_U8("graphics/unknown/unknown_3D810C.bin");

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
    .tileTag = ANIM_TAG_RAIN_DROPS,
    .paletteTag = ANIM_TAG_RAIN_DROPS,
    .oam = &gOamData_AffineOff_ObjNormal_16x32,
    .anims = sAnims_RainDrop,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimRainDrop,
};

#define tRaindropSpawnTimer    data[0]
#define tRaindropUnused        data[1]
#define tRaindropSpawnInterval data[2]
#define tRaindropSpawnDuration data[3]

void AnimTask_CreateRaindrops(u8 taskId)
{
    if (gTasks[taskId].tRaindropSpawnTimer == 0)
    {
        gTasks[taskId].tRaindropUnused = gBattleAnimArgs[0];
        gTasks[taskId].tRaindropSpawnInterval = gBattleAnimArgs[1];
        gTasks[taskId].tRaindropSpawnDuration = gBattleAnimArgs[2];
    }

    gTasks[taskId].tRaindropSpawnTimer++;

    if (gTasks[taskId].tRaindropSpawnTimer % gTasks[taskId].tRaindropSpawnInterval == 1)
    {
        u8 x = Random() % DISPLAY_WIDTH;
        u8 y = Random() % (DISPLAY_HEIGHT / 2);
        CreateSprite(&gRainDropSpriteTemplate, x, y, 4);
    }

    if (gTasks[taskId].tRaindropSpawnTimer == gTasks[taskId].tRaindropSpawnDuration)
    {
        DestroyAnimVisualTask(taskId);
    }
}

#undef tRaindropSpawnTimer
#undef tRaindropUnused
#undef tRaindropSpawnInterval
#undef tRaindropSpawnDuration

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
