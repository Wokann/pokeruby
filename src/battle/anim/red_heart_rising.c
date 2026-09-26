#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimRedHeartRising(struct Sprite *sprite);
static void AnimRedHeartRising_Step(struct Sprite *sprite);

// Red hearts rising from the bottom of the screen for Attract.

const struct SpriteTemplate gRedHeartRisingSpriteTemplate =
{
    .tileTag = ANIM_TAG_RED_HEART,
    .paletteTag = ANIM_TAG_RED_HEART,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimRedHeartRising,
};

static void AnimRedHeartRising(struct Sprite *sprite)
{
    sprite->x = gBattleAnimArgs[0];
    sprite->y = DISPLAY_HEIGHT;
    sprite->data[0] = gBattleAnimArgs[2];
    sprite->data[1] = gBattleAnimArgs[1];
    sprite->callback = WaitAnimForDuration;
    StoreSpriteCallbackInData6(sprite, AnimRedHeartRising_Step);
}

static void AnimRedHeartRising_Step(struct Sprite *sprite)
{
    s16 y;
    sprite->data[2] += sprite->data[1];
    sprite->y2 = -((u16)sprite->data[2] >> 8);
    sprite->x2 = Sin(sprite->data[3], 4);
    sprite->data[3] = (sprite->data[3] + 3) & 0xFF;
    y = sprite->y + sprite->y2;
    if (y <= 72)
    {
        sprite->invisible = sprite->data[3] % 2;
        if (y <= 64)
            DestroyAnimSprite(sprite);
    }
}
