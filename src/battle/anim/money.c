#include "global.h"
#include "battle_anim.h"
#include "trig.h"
#include "sound.h"


static void AnimCoinThrow(struct Sprite *sprite);
static void AnimFallingCoin(struct Sprite *sprite);
static void AnimFallingCoin_Step(struct Sprite *sprite);

// money
// Used by Pay Day.

const union AnimCmd gCoinAnimCmds[] =
{
    ANIMCMD_FRAME(8, 1),
    ANIMCMD_END,
};

const union AnimCmd *const gCoinAnimTable[] =
{
    gCoinAnimCmds,
};

const union AffineAnimCmd gFallingCoinAffineAnimCmds[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, 10, 1),
    AFFINEANIMCMD_JUMP(0),
};

const union AffineAnimCmd *const gFallingCoinAffineAnimTable[] =
{
    gFallingCoinAffineAnimCmds,
};

const struct SpriteTemplate gCoinThrowSpriteTemplate =
{
    .tileTag = ANIM_TAG_COIN,
    .paletteTag = ANIM_TAG_COIN,
    .oam = &gOamData_AffineNormal_ObjNormal_16x16,
    .anims = gCoinAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimCoinThrow,
};

const struct SpriteTemplate gFallingCoinSpriteTemplate =
{
    .tileTag = ANIM_TAG_COIN,
    .paletteTag = ANIM_TAG_COIN,
    .oam = &gOamData_AffineNormal_ObjNormal_16x16,
    .anims = gCoinAnimTable,
    .images = NULL,
    .affineAnims = gFallingCoinAffineAnimTable,
    .callback = AnimFallingCoin,
};

static void AnimCoinThrow(struct Sprite *sprite)
{
    s16 r6;
    s16 r7;
    u16 var;

    InitSpritePosToAnimAttacker(sprite, 1);
    r6 = GetBattlerSpriteCoord(gBattleAnimTarget, 2);
    r7 = GetBattlerSpriteCoord(gBattleAnimTarget, 3) + gBattleAnimArgs[3];
    if (GetBattlerSide(gBattleAnimAttacker) != 0)
        gBattleAnimArgs[2] = -gBattleAnimArgs[2];

    r6 += gBattleAnimArgs[2];
    var = ArcTan2Neg(r6 - sprite->x, r7 - sprite->y);
    var += 0xC000;
    TrySetSpriteRotScale(sprite, 0, 0x100, 0x100, var);
    sprite->data[0] = gBattleAnimArgs[4];
    sprite->data[2] = r6;
    sprite->data[4] = r7;
    sprite->callback = InitAnimLinearTranslationWithSpeedAndPos;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}

static void AnimFallingCoin(struct Sprite *sprite)
{
    sprite->data[2] = -16;
    sprite->y += 8;
    sprite->callback = AnimFallingCoin_Step;
}

static void AnimFallingCoin_Step(struct Sprite *sprite)
{
    sprite->data[0] += 0x80;
    sprite->x2 = sprite->data[0] >> 8;
    if (GetBattlerSide(gBattleAnimAttacker) == 0)
        sprite->x2 = -sprite->x2;

    sprite->y2 = Sin(sprite->data[1], sprite->data[2]);
    sprite->data[1] += 5;
    if (sprite->data[1] > 0x7E)
    {
        sprite->data[1] = 0;
        sprite->data[2] /= 2;
        if (++sprite->data[3] == 2)
            DestroyAnimSprite(sprite);
    }
}
