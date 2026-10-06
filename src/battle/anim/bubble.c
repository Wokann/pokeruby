#include "global.h"
#include "battle_anim.h"
#include "trig.h"
#include "util.h"

static void AnimWaterBubbleProjectile(struct Sprite *sprite);
static void AnimWaterBubbleProjectile_Step1(struct Sprite *sprite);
static void AnimWaterBubbleProjectile_Step2(struct Sprite *sprite);
static void AnimWaterBubbleProjectile_Step3(struct Sprite *sprite);

// bubble (individual bubble that floats around)
// Used in Bubble and Bubblebeam

static const union AffineAnimCmd sAffineAnim_WaterBubbleProjectile[] =
{
    AFFINEANIMCMD_FRAME(0xFFFB, 0xFFFB, 0, 10),
    AFFINEANIMCMD_FRAME(0x5, 0x5, 0, 10),
    AFFINEANIMCMD_JUMP(0),
};

static const union AffineAnimCmd *const sAffineAnims_WaterBubbleProjectile[] =
{
    sAffineAnim_WaterBubbleProjectile,
};

static const union AnimCmd sAnim_WaterBubbleProjectile[] =
{
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_FRAME(4, 5),
    ANIMCMD_FRAME(8, 5),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_WaterBubbleProjectile[] =
{
    sAnim_WaterBubbleProjectile,
};

const struct SpriteTemplate gWaterBubbleProjectileSpriteTemplate =
{
    .tileTag = ANIM_TAG_BUBBLE,
    .paletteTag = ANIM_TAG_BUBBLE,
    .oam = &gOamData_AffineNormal_ObjBlend_16x16,
    .anims = sAnims_WaterBubbleProjectile,
    .images = NULL,
    .affineAnims = sAffineAnims_WaterBubbleProjectile,
    .callback = AnimWaterBubbleProjectile,
};

static void AnimWaterBubbleProjectile(struct Sprite *sprite)
{
    u8 newSpriteId;

    if (GetBattlerSide(gBattleAnimAttacker) != 0)
    {
        sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, 2) - gBattleAnimArgs[0];
        sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, 3) + gBattleAnimArgs[1];
    }
    else
    {
        sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, 2) + gBattleAnimArgs[0];
        sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, 3) + gBattleAnimArgs[1];
    }

    sprite->animPaused = 1;

    if (GetBattlerSide(gBattleAnimAttacker) != 0)
    {
        gBattleAnimArgs[2] = -gBattleAnimArgs[2];
    }

    sprite->data[0] = gBattleAnimArgs[6];
    sprite->data[1] = sprite->x;
    sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimTarget, 2);
    sprite->data[3] = sprite->y;
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimTarget, 3);

    InitAnimLinearTranslation(sprite);

    newSpriteId = CreateInvisibleSpriteWithCallback(SpriteCallbackDummy);
    sprite->data[5] = newSpriteId;

    sprite->x -= Sin((u8)gBattleAnimArgs[4], gBattleAnimArgs[2]);
    sprite->y -= Cos((u8)gBattleAnimArgs[4], gBattleAnimArgs[3]);

    gSprites[newSpriteId].data[0] = gBattleAnimArgs[2];
    gSprites[newSpriteId].data[1] = gBattleAnimArgs[3];
    gSprites[newSpriteId].data[2] = gBattleAnimArgs[5];
    gSprites[newSpriteId].data[3] = (u8)gBattleAnimArgs[4] * 256;
    gSprites[newSpriteId].data[4] = gBattleAnimArgs[6];

    sprite->callback = AnimWaterBubbleProjectile_Step1;
    sprite->callback(sprite);
}

static void AnimWaterBubbleProjectile_Step1(struct Sprite *sprite)
{
    u8 otherSpriteId = sprite->data[5];

    u8 timer = gSprites[otherSpriteId].data[4];
    u16 trigIndex = gSprites[otherSpriteId].data[3];

    sprite->data[0] = 1;
    AnimTranslateLinear(sprite);

    sprite->x2 += Sin(trigIndex / 256, gSprites[otherSpriteId].data[0]);
    sprite->y2 += Cos(trigIndex / 256, gSprites[otherSpriteId].data[1]);

    gSprites[otherSpriteId].data[3] = gSprites[otherSpriteId].data[2] + trigIndex;

    if (--timer != 0)
    {
        gSprites[otherSpriteId].data[4] = timer;
    }
    else
    {
        sprite->callback = AnimWaterBubbleProjectile_Step2;
        DestroySprite(&gSprites[otherSpriteId]);
    }
}

static void AnimWaterBubbleProjectile_Step2(struct Sprite *sprite)
{
    sprite->animPaused = 0;
    sprite->callback = RunStoredCallbackWhenAnimEnds;
    StoreSpriteCallbackInData6(sprite, AnimWaterBubbleProjectile_Step3);
}

static void AnimWaterBubbleProjectile_Step3(struct Sprite *sprite)
{
    sprite->data[0] = 10;
    sprite->callback = WaitAnimForDuration;
    StoreSpriteCallbackInData6(sprite, DestroySpriteAndMatrix);
}
