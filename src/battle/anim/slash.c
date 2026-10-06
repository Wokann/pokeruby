#include "global.h"
#include "battle_anim.h"
#include "trig.h"
#include "sound.h"

static void AnimSlashSlice(struct Sprite *sprite);
static void AnimFalseSwipeSlice(struct Sprite *sprite);
static void AnimFalseSwipePositionedSlice(struct Sprite *sprite);
static void AnimFalseSwipeSlice_Step1(struct Sprite *sprite);
static void AnimFalseSwipeSlice_Step2(struct Sprite *sprite);
static void AnimFalseSwipeSlice_Step3(struct Sprite *sprite);

// slash (a cutting animation)
// Used in Slash and False Swipe.

const union AnimCmd gSlashSliceAnimCmds1[] =
{
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(16, 4),
    ANIMCMD_FRAME(32, 4),
    ANIMCMD_FRAME(48, 4),
    ANIMCMD_END,
};

const union AnimCmd gSlashSliceAnimCmds2[] =
{
    ANIMCMD_FRAME(48, 4),
    ANIMCMD_END,
};

const union AnimCmd *const gSlashSliceAnimTable[] =
{
    gSlashSliceAnimCmds1,
    gSlashSliceAnimCmds2,
};

const struct SpriteTemplate gSlashSliceSpriteTemplate =
{
    .tileTag = ANIM_TAG_SLASH,
    .paletteTag = ANIM_TAG_SLASH,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gSlashSliceAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimSlashSlice,
};

const struct SpriteTemplate gFalseSwipeSliceSpriteTemplate =
{
    .tileTag = ANIM_TAG_SLASH_2,
    .paletteTag = ANIM_TAG_SLASH_2,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gSlashSliceAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimFalseSwipeSlice,
};

const struct SpriteTemplate gFalseSwipePositionedSliceSpriteTemplate =
{
    .tileTag = ANIM_TAG_SLASH_2,
    .paletteTag = ANIM_TAG_SLASH_2,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gSlashSliceAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimFalseSwipePositionedSlice,
};

static void AnimSlashSlice(struct Sprite *sprite)
{
    if (gBattleAnimArgs[0] == 0)
    {
        sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, 2) + gBattleAnimArgs[1];
        sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, 3) + gBattleAnimArgs[2];
    }
    else
    {
        sprite->x = GetBattlerSpriteCoord(gBattleAnimTarget, 2) + gBattleAnimArgs[1];
        sprite->y = GetBattlerSpriteCoord(gBattleAnimTarget, 3) + gBattleAnimArgs[2];
    }

    sprite->data[0] = 0;
    sprite->data[1] = 0;
    StoreSpriteCallbackInData6(sprite, AnimFalseSwipeSlice_Step3);
    sprite->callback = RunStoredCallbackWhenAnimEnds;
}

static void AnimFalseSwipeSlice(struct Sprite *sprite)
{
    sprite->x = GetBattlerSpriteCoord(gBattleAnimTarget, 2) + 0xFFD0;
    sprite->y = GetBattlerSpriteCoord(gBattleAnimTarget, 3);
    StoreSpriteCallbackInData6(sprite, AnimFalseSwipeSlice_Step1);
    sprite->callback = RunStoredCallbackWhenAnimEnds;
}

static void AnimFalseSwipePositionedSlice(struct Sprite *sprite)
{
    sprite->x = sprite->x = GetBattlerSpriteCoord(gBattleAnimTarget, 2) + 0xFFD0 + gBattleAnimArgs[0];
    sprite->y = GetBattlerSpriteCoord(gBattleAnimTarget, 3);
    StartSpriteAnim(sprite, 1);
    sprite->data[0] = 0;
    sprite->data[1] = 0;
    sprite->callback = AnimFalseSwipeSlice_Step3;
}

static void AnimFalseSwipeSlice_Step1(struct Sprite *sprite)
{
    if (++sprite->data[0] > 8)
    {
        sprite->data[0] = 12;
        sprite->data[1] = 8;
        sprite->data[2] = 0;
        StoreSpriteCallbackInData6(sprite, AnimFalseSwipeSlice_Step2);
        sprite->callback = TranslateSpriteLinear;
    }
}

static void AnimFalseSwipeSlice_Step2(struct Sprite *sprite)
{
    sprite->data[0] = 0;
    sprite->data[1] = 0;
    sprite->callback = AnimFalseSwipeSlice_Step3;
}

static void AnimFalseSwipeSlice_Step3(struct Sprite *sprite)
{
    if (++sprite->data[0] > 1)
    {
        sprite->data[0] = 0;
        sprite->invisible = !sprite->invisible;
        if (++sprite->data[1] > 8)
            DestroyAnimSprite(sprite);
    }
}
