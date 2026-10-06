#include "global.h"
#include "battle_anim.h"
#include "trig.h"
#include "sound.h"

static void AnimEyeSparkle(struct Sprite *sprite);
static void AnimEyeSparkle_Step(struct Sprite *sprite);

// perceive (shows a sparkle in a set of eyes, usually for heightened perception.)
// Used in Glare, Tickle, and Scary Face.

const union AnimCmd gEyeSparkleAnimCmds[] =
{
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(4, 4),
    ANIMCMD_FRAME(8, 4),
    ANIMCMD_FRAME(4, 4),
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_END,
};

const union AnimCmd *const gEyeSparkleAnimTable[] =
{
    gEyeSparkleAnimCmds,
};

const struct SpriteTemplate gEyeSparkleSpriteTemplate =
{
    .tileTag = ANIM_TAG_EYE_SPARKLE,
    .paletteTag = ANIM_TAG_EYE_SPARKLE,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = gEyeSparkleAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimEyeSparkle,
};

static void AnimEyeSparkle_Step(struct Sprite *sprite)
{
    if (sprite->animEnded)
        DestroyAnimSprite(sprite);
}

static void AnimEyeSparkle(struct Sprite *sprite)
{
    InitSpritePosToAnimAttacker(sprite, 1);
    sprite->callback = AnimEyeSparkle_Step;
}
