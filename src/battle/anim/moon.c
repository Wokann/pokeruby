#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimMoon(struct Sprite *sprite);
static void AnimMoon_Step(struct Sprite *sprite);

// moon (shows a moon image.)
// Used in Moonlight.

const struct SpriteTemplate gMoonSpriteTemplate =
{
    .tileTag = ANIM_TAG_MOON,
    .paletteTag = ANIM_TAG_MOON,
    .oam = &gOamData_AffineOff_ObjBlend_64x64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimMoon,
};

static void AnimMoon(struct Sprite *sprite)
{
    if (IsContest())
    {
        sprite->x = 48;
        sprite->y = 40;
    }
    else
    {
        sprite->x = gBattleAnimArgs[0];
        sprite->y = gBattleAnimArgs[1];
    }

    sprite->oam.shape = SPRITE_SHAPE(64x64);
    sprite->oam.size = SPRITE_SIZE(64x64);
    sprite->data[0] = 0;
    sprite->callback = AnimMoon_Step;
}

static void AnimMoon_Step(struct Sprite *sprite)
{
    if (sprite->data[0])
        DestroyAnimSprite(sprite);
}
