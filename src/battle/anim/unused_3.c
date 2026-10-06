#include "global.h"
#include "battle_anim.h"
#include "trig.h"
#include "sound.h"

static void AnimBubbleBurst(struct Sprite *sprite);
static void AnimBubbleBurst_Step(struct Sprite *sprite);

// unused_3 (seems to be some sort of popping effect with a growing diamond shape)

static const union AnimCmd sAnim_BubbleBurst[] =
{
    ANIMCMD_FRAME(0, 10),
    ANIMCMD_FRAME(4, 10),
    ANIMCMD_FRAME(8, 10),
    ANIMCMD_FRAME(12, 10),
    ANIMCMD_FRAME(16, 26),
    ANIMCMD_FRAME(16, 5),
    ANIMCMD_FRAME(20, 5),
    ANIMCMD_FRAME(24, 15),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_BubbleBurst_Flipped[] =
{
    ANIMCMD_FRAME(0, 10, .hFlip = TRUE),
    ANIMCMD_FRAME(4, 10, .hFlip = TRUE),
    ANIMCMD_FRAME(8, 10, .hFlip = TRUE),
    ANIMCMD_FRAME(12, 10, .hFlip = TRUE),
    ANIMCMD_FRAME(16, 26, .hFlip = TRUE),
    ANIMCMD_FRAME(16, 5, .hFlip = TRUE),
    ANIMCMD_FRAME(20, 5, .hFlip = TRUE),
    ANIMCMD_FRAME(24, 15, .hFlip = TRUE),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_BubbleBurst[] =
{
    sAnim_BubbleBurst,
    sAnim_BubbleBurst_Flipped,
};

static const struct SpriteTemplate sBubbleBurstSpriteTemplate =
{
    .tileTag = ANIM_TAG_BUBBLE_BURST,
    .paletteTag = ANIM_TAG_BUBBLE_BURST,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = sAnims_BubbleBurst,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimBubbleBurst,
};

static void AnimBubbleBurst(struct Sprite *sprite)
{
    SetSpriteCoordsToAnimAttackerCoords(sprite);
    if (GetBattlerSide(gBattleAnimAttacker) == 0)
    {
        sprite->x += gBattleAnimArgs[0];
        sprite->y += gBattleAnimArgs[1];
    }
    else
    {
        sprite->x -= gBattleAnimArgs[0];
        sprite->y += gBattleAnimArgs[1];
        StartSpriteAnim(sprite, 1);
    }

    sprite->callback = AnimBubbleBurst_Step;
}

static void AnimBubbleBurst_Step(struct Sprite *sprite)
{
    if (++sprite->data[0] > 30)
    {
        sprite->y2 = (30 - sprite->data[0]) / 3;
        sprite->x2 = Sin(sprite->data[1] * 4, 3);
        sprite->data[1]++;
    }

    if (sprite->animEnded)
        DestroyAnimSprite(sprite);
}
