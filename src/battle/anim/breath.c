#include "global.h"
#include "battle_anim.h"
#include "trig.h"
#include "sound.h"

static void AnimBreathPuff(struct Sprite *sprite);

const union AnimCmd gBreathPuffAnimCmds1[] =
{
    ANIMCMD_FRAME(0, 4, .hFlip = TRUE),
    ANIMCMD_FRAME(4, 40, .hFlip = TRUE),
    ANIMCMD_FRAME(8, 4, .hFlip = TRUE),
    ANIMCMD_FRAME(12, 4, .hFlip = TRUE),
    ANIMCMD_END,
};

const union AnimCmd gBreathPuffAnimCmds2[] =
{
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(4, 40),
    ANIMCMD_FRAME(8, 4),
    ANIMCMD_FRAME(12, 4),
    ANIMCMD_END,
};

const union AnimCmd *const gBreathPuffAnimTable[] =
{
    gBreathPuffAnimCmds1,
    gBreathPuffAnimCmds2,
};

const struct SpriteTemplate gBreathPuffSpriteTemplate =
{
    .tileTag = ANIM_TAG_BREATH,
    .paletteTag = ANIM_TAG_BREATH,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = gBreathPuffAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimBreathPuff,
};

// Animates a little puff of the mon's breath.
// Used by MOVE_SWAGGER and MOVE_BULK_UP
// No args.
static void AnimBreathPuff(struct Sprite *sprite)
{
    if (GetBattlerSide(gBattleAnimAttacker) == 0)
    {
        StartSpriteAnim(sprite, 0);
        sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, 2) + 32;
        sprite->data[1] = 0x40;
    }
    else
    {
        StartSpriteAnim(sprite, 1);
        sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, 2) - 32;
        sprite->data[1] = -0x40;
    }

    sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, 3);
    sprite->data[0] = 0x34;
    sprite->data[2] = 0;
    sprite->data[3] = 0;
    sprite->data[4] = 0;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
    sprite->callback = TranslateSpriteLinearFixedPoint;
}
