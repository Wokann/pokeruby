#include "global.h"
#include "battle_anim.h"
#include "sprite.h"

extern s16 gBattleAnimArgs[8];

static void AnimFurySwipes(struct Sprite *sprite);

static const union AnimCmd sAnim_FurySwipes[] =
{
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(16, 4),
    ANIMCMD_FRAME(32, 4),
    ANIMCMD_FRAME(48, 4),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_FurySwipes_Flipped[] =
{
    ANIMCMD_FRAME(0, 4, .hFlip = TRUE),
    ANIMCMD_FRAME(16, 4, .hFlip = TRUE),
    ANIMCMD_FRAME(32, 4, .hFlip = TRUE),
    ANIMCMD_FRAME(48, 4, .hFlip = TRUE),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_FurySwipes[] =
{
    sAnim_FurySwipes,
    sAnim_FurySwipes_Flipped,
};

const struct SpriteTemplate gFurySwipesSpriteTemplate =
{
    .tileTag = ANIM_TAG_SWIPE,
    .paletteTag = ANIM_TAG_SWIPE,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = sAnims_FurySwipes,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimFurySwipes,
};

static void AnimFurySwipes(struct Sprite *sprite)
{
    if (sprite->data[0] == 0)
    {
        sprite->x += gBattleAnimArgs[0];
        sprite->y += gBattleAnimArgs[1];
        StartSpriteAnim(sprite, gBattleAnimArgs[2]);
        sprite->data[0]++;
    }
    else if (sprite->animEnded)
    {
        DestroyAnimSprite(sprite);
    }
}
