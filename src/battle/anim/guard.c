#include "global.h"
#include "battle.h"
#include "battle_anim.h"

static void AnimGuardRing(struct Sprite *sprite);

// guard (moves guard rings upwards)
// Used in Safeguard.

const union AffineAnimCmd gGuardRingAffineAnimCmds1[] =
{
    AFFINEANIMCMD_FRAME(0x100, 0x100, 0, 0),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd gGuardRingAffineAnimCmds2[] =
{
    AFFINEANIMCMD_FRAME(0x200, 0x100, 0, 0),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd *const gGuardRingAffineAnimTable[] =
{
    gGuardRingAffineAnimCmds1,
    gGuardRingAffineAnimCmds2,
};

const struct SpriteTemplate gGuardRingSpriteTemplate =
{
    .tileTag = ANIM_TAG_GUARD_RING,
    .paletteTag = ANIM_TAG_GUARD_RING,
    .oam = &gOamData_AffineDouble_ObjBlend_64x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gGuardRingAffineAnimTable,
    .callback = AnimGuardRing,
};

static void AnimGuardRing(struct Sprite *sprite)
{
    if ((gBattleTypeFlags & BATTLE_TYPE_DOUBLE) && IsBattlerSpriteVisible(gBattleAnimAttacker ^ 2))
    {
        SetAverageBattlerPositions(gBattleAnimAttacker, 0, &sprite->x, &sprite->y);
        sprite->y += 40;

        StartSpriteAffineAnim(sprite, 1);
    }
    else
    {
        sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, 0);
        sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, 1) + 40;
    }

    sprite->data[0] = 13;
    sprite->data[2] = sprite->x;
    sprite->data[4] = sprite->y - 72;

    sprite->callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}
