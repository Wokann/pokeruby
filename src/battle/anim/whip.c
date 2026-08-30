#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

void sub_80CC82C(struct Sprite* sprite);
void AnimWhipHit(struct Sprite* sprite);

// whip (does a hitting animation that uses a rotating sprite to invoke the sprite getting hit.)
// Used by Slam and Vine Whip.

const union AnimCmd gWhipAnimCmds[] =
{
    ANIMCMD_FRAME(64, 3),
    ANIMCMD_FRAME(80, 3),
    ANIMCMD_FRAME(96, 3),
    ANIMCMD_FRAME(112, 6),
    ANIMCMD_END,
};

const union AnimCmd gWhipAnimCmds_Flipped[] =
{
    ANIMCMD_FRAME(64, 3, .hFlip = TRUE),
    ANIMCMD_FRAME(80, 3, .hFlip = TRUE),
    ANIMCMD_FRAME(96, 3, .hFlip = TRUE),
    ANIMCMD_FRAME(112, 6, .hFlip = TRUE),
    ANIMCMD_END,
};

const union AnimCmd *const gWhipAnimTable[] =
{
    gWhipAnimCmds,
    gWhipAnimCmds_Flipped,
};

const struct SpriteTemplate gSlamHitSpriteTemplate =
{
    .tileTag = ANIM_TAG_SLAM_HIT,
    .paletteTag = ANIM_TAG_SLAM_HIT,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gWhipAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimWhipHit,
};

const struct SpriteTemplate gVineWhipSpriteTemplate =
{
    .tileTag = ANIM_TAG_WHIP_HIT,
    .paletteTag = ANIM_TAG_WHIP_HIT,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gWhipAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimWhipHit,
};

const union AnimCmd gSpriteAnim_83D6A0C[] =
{
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(16, 4),
    ANIMCMD_FRAME(32, 4),
    ANIMCMD_FRAME(48, 4),
    ANIMCMD_FRAME(64, 5),
    ANIMCMD_END,
};

const union AnimCmd *const gSpriteAnimTable_83D6A24[] =
{
    gSpriteAnim_83D6A0C,
};

const struct SpriteTemplate gSpriteTemplate_83D6A28 =
{
    .tileTag = ANIM_TAG_UNUSED_HIT,
    .paletteTag = ANIM_TAG_UNUSED_HIT,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gSpriteAnimTable_83D6A24,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80CC82C,
};

const struct SpriteTemplate gSpriteTemplate_83D6A40 =
{
    .tileTag = ANIM_TAG_UNUSED_HIT_2,
    .paletteTag = ANIM_TAG_UNUSED_HIT_2,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gSpriteAnimTable_83D6A24,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80CC82C,
};

static void AnimWhipHit_WaitEnd(struct Sprite* sprite)
{
    if (sprite->animEnded)
        DestroyAnimSprite(sprite);
}

void sub_80CC82C(struct Sprite* sprite)
{
    if (GetBattlerSide(gBattleAnimAttacker) != 0)
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

void AnimWhipHit(struct Sprite* sprite)
{
    if (GetBattlerSide(gBattleAnimAttacker) == 0)
        StartSpriteAnim(sprite, 1);

    sprite->callback = AnimWhipHit_WaitEnd;
    SetAnimSpriteInitialXOffset(sprite, gBattleAnimArgs[0]);
    sprite->y += gBattleAnimArgs[1];
}
