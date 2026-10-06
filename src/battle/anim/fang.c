#include "global.h"
#include "battle_anim.h"
#include "trig.h"
#include "sound.h"

static void AnimSuperFang(struct Sprite *sprite);

// fang
// Used by Super Fang (and probably Hyper Fang, but the actual callbacks are not in this file.)
// (Look into this one later.)

const union AnimCmd gSuperFangAnimCmds[] =
{
    ANIMCMD_FRAME(0, 2),
    ANIMCMD_FRAME(16, 2),
    ANIMCMD_FRAME(32, 2),
    ANIMCMD_FRAME(48, 2),
    ANIMCMD_END,
};

const union AnimCmd *const gSuperFangAnimTable[] =
{
    gSuperFangAnimCmds,
};

const struct SpriteTemplate gSuperFangSpriteTemplate =
{
    .tileTag = ANIM_TAG_FANG_ATTACK,
    .paletteTag = ANIM_TAG_FANG_ATTACK,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gSuperFangAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimSuperFang,
};

static void AnimSuperFang(struct Sprite *sprite)
{
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
    sprite->callback = RunStoredCallbackWhenAnimEnds;
}
