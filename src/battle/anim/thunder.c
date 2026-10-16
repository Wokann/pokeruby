#include "global.h"
#include "battle_anim.h"

static void AnimLightning(struct Sprite *sprite);
static void AnimLightning_Step(struct Sprite *);

// thunder (positions the lightning bolts)
// Used in Thunder, Thunder Punch, and Tri Attack.

static const union AnimCmd sAnim_Lightning[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_FRAME(16, 5),
    ANIMCMD_FRAME(32, 8),
    ANIMCMD_FRAME(48, 5),
    ANIMCMD_FRAME(64, 5),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_Lightning[] =
{
    sAnim_Lightning,
};

const struct SpriteTemplate gLightningSpriteTemplate =
{
    .tileTag = ANIM_TAG_LIGHTNING,
    .paletteTag = ANIM_TAG_LIGHTNING,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = sAnims_Lightning,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimLightning,
};

static void AnimLightning(struct Sprite *sprite)
{
    if (GetBattlerSide(gBattleAnimAttacker) != 0)
    {
        sprite->x -= gBattleAnimArgs[0];
    }
    else
    {
        sprite->x += gBattleAnimArgs[0];
    }

    sprite->y += gBattleAnimArgs[1];
    sprite->callback = AnimLightning_Step;
}

static void AnimLightning_Step(struct Sprite *sprite)
{
    if (sprite->animEnded)
    {
        DestroyAnimSprite(sprite);
    }
}
