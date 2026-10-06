#include "global.h"
#include "battle_anim.h"
#include "trig.h"
#include "sound.h"

static void AnimThoughtBubble(struct Sprite *sprite);
static void AnimThoughtBubble_Step(struct Sprite *sprite);

// thought (thought bubble)
// Used in Metronome and Taunt.

const union AnimCmd gMetronomeThroughtBubbleAnimCmds1[] =
{
    ANIMCMD_FRAME(0, 2, .hFlip = TRUE),
    ANIMCMD_FRAME(16, 2, .hFlip = TRUE),
    ANIMCMD_FRAME(32, 2, .hFlip = TRUE),
    ANIMCMD_FRAME(48, 2, .hFlip = TRUE),
    ANIMCMD_END,
};

const union AnimCmd gMetronomeThroughtBubbleAnimCmds3[] =
{
    ANIMCMD_FRAME(48, 2, .hFlip = TRUE),
    ANIMCMD_FRAME(32, 2, .hFlip = TRUE),
    ANIMCMD_FRAME(16, 2, .hFlip = TRUE),
    ANIMCMD_FRAME(0, 2, .hFlip = TRUE),
    ANIMCMD_END,
};

const union AnimCmd gMetronomeThroughtBubbleAnimCmds2[] =
{
    ANIMCMD_FRAME(0, 2),
    ANIMCMD_FRAME(16, 2),
    ANIMCMD_FRAME(32, 2),
    ANIMCMD_FRAME(48, 2),
    ANIMCMD_END,
};

const union AnimCmd gMetronomeThroughtBubbleAnimCmds4[] =
{
    ANIMCMD_FRAME(48, 2),
    ANIMCMD_FRAME(32, 2),
    ANIMCMD_FRAME(16, 2),
    ANIMCMD_FRAME(0, 2),
    ANIMCMD_END,
};

const union AnimCmd *const gMetronomeThroughtBubbleAnimTable[] =
{
    gMetronomeThroughtBubbleAnimCmds1,
    gMetronomeThroughtBubbleAnimCmds2,
    gMetronomeThroughtBubbleAnimCmds3,
    gMetronomeThroughtBubbleAnimCmds4,
};

const struct SpriteTemplate gThoughtBubbleSpriteTemplate =
{
    .tileTag = ANIM_TAG_THOUGHT_BUBBLE,
    .paletteTag = ANIM_TAG_THOUGHT_BUBBLE,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gMetronomeThroughtBubbleAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimThoughtBubble,
};

void SetSpriteNextToMonHead(u8 battler, struct Sprite *sprite)
{
    if (GetBattlerSide(battler) == 0)
        sprite->x = GetBattlerSpriteCoordAttr(battler, 5) + 8;
    else
        sprite->x = GetBattlerSpriteCoordAttr(battler, 4) - 8;

    sprite->y = GetBattlerSpriteCoord(battler, 3) - (s16)GetBattlerSpriteCoordAttr(battler, 0) / 4;
}

static void AnimThoughtBubble(struct Sprite *sprite)
{
    u8 animNum;
    u8 battler;
    if (gBattleAnimArgs[0] == 0)
        battler = gBattleAnimAttacker;
    else
        battler = gBattleAnimTarget;

    SetSpriteNextToMonHead(battler, sprite);
    animNum = (GetBattlerSide(battler) == 0) ? 0 : 1;
    sprite->data[0] = gBattleAnimArgs[1];
    sprite->data[1] = animNum + 2;
    StartSpriteAnim(sprite, animNum);
    StoreSpriteCallbackInData6(sprite, AnimThoughtBubble_Step);
    sprite->callback = RunStoredCallbackWhenAnimEnds;
}

static void AnimThoughtBubble_Step(struct Sprite *sprite)
{
    if (--sprite->data[0] == 0)
    {
        StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
        StartSpriteAnim(sprite, sprite->data[1]);
        sprite->callback = RunStoredCallbackWhenAnimEnds;
    }
}
