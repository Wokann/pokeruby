#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "constants/battle.h"
#include "sound.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

void AnimSlidingHit(struct Sprite *sprite);
void AnimWhipHit(struct Sprite *sprite);

// whip (does a hitting animation that uses a rotating sprite to invoke the sprite getting hit.)
// Used by Slam and Vine Whip.

static const union AnimCmd sAnim_Whip[] =
{
    ANIMCMD_FRAME(64, 3),
    ANIMCMD_FRAME(80, 3),
    ANIMCMD_FRAME(96, 3),
    ANIMCMD_FRAME(112, 6),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_Whip_Flipped[] =
{
    ANIMCMD_FRAME(64, 3, .hFlip = TRUE),
    ANIMCMD_FRAME(80, 3, .hFlip = TRUE),
    ANIMCMD_FRAME(96, 3, .hFlip = TRUE),
    ANIMCMD_FRAME(112, 6, .hFlip = TRUE),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_Whip[] =
{
    sAnim_Whip,
    sAnim_Whip_Flipped,
};

const struct SpriteTemplate gSlamHitSpriteTemplate =
{
    .tileTag = ANIM_TAG_SLAM_HIT,
    .paletteTag = ANIM_TAG_SLAM_HIT,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = sAnims_Whip,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimWhipHit,
};

const struct SpriteTemplate gVineWhipSpriteTemplate =
{
    .tileTag = ANIM_TAG_WHIP_HIT,
    .paletteTag = ANIM_TAG_WHIP_HIT,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = sAnims_Whip,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimWhipHit,
};

static const union AnimCmd sAnim_SlidingHit[] =
{
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(16, 4),
    ANIMCMD_FRAME(32, 4),
    ANIMCMD_FRAME(48, 4),
    ANIMCMD_FRAME(64, 5),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_SlidingHit[] =
{
    sAnim_SlidingHit,
};

static const struct SpriteTemplate sSlidingHit1SpriteTemplate =
{
    .tileTag = ANIM_TAG_HIT,
    .paletteTag = ANIM_TAG_HIT,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = sAnims_SlidingHit,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimSlidingHit,
};

static const struct SpriteTemplate sSlidingHit2SpriteTemplate =
{
    .tileTag = ANIM_TAG_HIT_2,
    .paletteTag = ANIM_TAG_HIT_2,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = sAnims_SlidingHit,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimSlidingHit,
};

static void AnimWhipHit_WaitEnd(struct Sprite *sprite)
{
    if (sprite->animEnded)
        DestroyAnimSprite(sprite);
}

void AnimSlidingHit(struct Sprite *sprite)
{
    if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
    {
        sprite->x -= gBattleAnimArgs[0];
        sprite->y += gBattleAnimArgs[1];
    }
    else
    {
        sprite->x += gBattleAnimArgs[0];
        sprite->y += gBattleAnimArgs[1];
    }

    sprite->callback = RunStoredCallbackWhenAnimEnds;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}

void AnimWhipHit(struct Sprite *sprite)
{
    if (GetBattlerSide(gBattleAnimAttacker) == B_SIDE_PLAYER)
        StartSpriteAnim(sprite, 1);

    sprite->callback = AnimWhipHit_WaitEnd;
    SetAnimSpriteInitialXOffset(sprite, gBattleAnimArgs[0]);
    sprite->y += gBattleAnimArgs[1];
}
