#include "global.h"
#include "battle_anim.h"
#include "trig.h"
#include "sound.h"


void sub_80CC82C(struct Sprite* sprite);
void sub_80CC884(struct Sprite* sprite);

// whip (does a hitting animation that uses a rotating sprite to invoke the sprite getting hit.)
// Used by Slam and Vine Whip.

const union AnimCmd gSpriteAnim_83D69AC[] =
{
    ANIMCMD_FRAME(64, 3),
    ANIMCMD_FRAME(80, 3),
    ANIMCMD_FRAME(96, 3),
    ANIMCMD_FRAME(112, 6),
    ANIMCMD_END,
};

const union AnimCmd gSpriteAnim_83D69C0[] =
{
    ANIMCMD_FRAME(64, 3, .hFlip = TRUE),
    ANIMCMD_FRAME(80, 3, .hFlip = TRUE),
    ANIMCMD_FRAME(96, 3, .hFlip = TRUE),
    ANIMCMD_FRAME(112, 6, .hFlip = TRUE),
    ANIMCMD_END,
};

const union AnimCmd *const gSpriteAnimTable_83D69D4[] =
{
    gSpriteAnim_83D69AC,
    gSpriteAnim_83D69C0,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83D69DC =
{
    .tileTag = 10056,
    .paletteTag = 10056,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gSpriteAnimTable_83D69D4,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80CC884,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83D69F4 =
{
    .tileTag = ANIM_TAG_WHIP_HIT,
    .paletteTag = ANIM_TAG_WHIP_HIT,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gSpriteAnimTable_83D69D4,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80CC884,
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
    .tileTag = ANIM_TAG_HIT,
    .paletteTag = ANIM_TAG_HIT,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gSpriteAnimTable_83D6A24,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80CC82C,
};

const struct SpriteTemplate gSpriteTemplate_83D6A40 =
{
    .tileTag = ANIM_TAG_HIT_2,
    .paletteTag = ANIM_TAG_HIT_2,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gSpriteAnimTable_83D6A24,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80CC82C,
};

static void sub_80CC810(struct Sprite* sprite)
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

void sub_80CC884(struct Sprite* sprite)
{
    if (GetBattlerSide(gBattleAnimAttacker) == 0)
        StartSpriteAnim(sprite, 1);

    sprite->callback = sub_80CC810;
    SetAnimSpriteInitialXOffset(sprite, gBattleAnimArgs[0]);
    sprite->y += gBattleAnimArgs[1];
}
