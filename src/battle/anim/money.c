#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"
#include "constants/battle.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

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
    s16 targetX;
    s16 targetY;
    u16 angle;

    InitSpritePosToAnimAttacker(sprite, TRUE);
    targetX = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_X_2);
    targetY = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_Y_PIC_OFFSET) + gBattleAnimArgs[3];
    if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
        gBattleAnimArgs[2] = -gBattleAnimArgs[2];

    targetX += gBattleAnimArgs[2];
    angle = ArcTan2Neg(targetX - sprite->x, targetY - sprite->y);
    angle += 0xC000;
    TrySetSpriteRotScale(sprite, FALSE, 0x100, 0x100, angle);
    sprite->data[0] = gBattleAnimArgs[4];
    sprite->data[2] = targetX;
    sprite->data[4] = targetY;
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
    if (GetBattlerSide(gBattleAnimAttacker) == B_SIDE_PLAYER)
        sprite->x2 = -sprite->x2;

    sprite->y2 = Sin(sprite->data[1], sprite->data[2]);
    sprite->data[1] += 5;
    if (sprite->data[1] > 126)
    {
        sprite->data[1] = 0;
        sprite->data[2] /= 2;
        if (++sprite->data[3] == 2)
            DestroyAnimSprite(sprite);
    }
}
