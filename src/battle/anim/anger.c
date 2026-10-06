#include "global.h"
#include "battle_anim.h"
#include "trig.h"
#include "sound.h"

static void AnimAngerMark(struct Sprite *sprite);

const union AffineAnimCmd gAngerMarkAffineAnimCmds[] =
{
    AFFINEANIMCMD_FRAME(0xB, 0xB, 0, 8),
    AFFINEANIMCMD_FRAME(0xFFF5, 0xFFF5, 0, 8),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd *const gAngerMarkAffineAnimTable[] =
{
    gAngerMarkAffineAnimCmds,
};

const struct SpriteTemplate gAngerMarkSpriteTemplate =
{
    .tileTag = ANIM_TAG_ANGER,
    .paletteTag = ANIM_TAG_ANGER,
    .oam = &gOamData_AffineNormal_ObjNormal_16x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gAngerMarkAffineAnimTable,
    .callback = AnimAngerMark,
};

// Animates an "angry" mark above a mon's head.
// arg 0: target mon (0 = attacker, 1 = target)
// arg 1: x pixel offset
// arg 2: y pixel offset
static void AnimAngerMark(struct Sprite *sprite)
{
    u8 battler;
    if (gBattleAnimArgs[0] == 0)
        battler = gBattleAnimAttacker;
    else
        battler = gBattleAnimTarget;

    if (GetBattlerSide(battler) == 1)
    {
        gBattleAnimArgs[1] *= -1;
    }

    sprite->x = GetBattlerSpriteCoord(battler, 2) + gBattleAnimArgs[1];
    sprite->y = GetBattlerSpriteCoord(battler, 3) + gBattleAnimArgs[2];
    if (sprite->y <= 7)
        sprite->y = 8;

    StoreSpriteCallbackInData6(sprite, DestroySpriteAndMatrix);
    sprite->callback = RunStoredCallbackWhenAffineAnimEnds;
}
