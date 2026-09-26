#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimSuperFang(struct Sprite *sprite);

// Used by Super Fang.

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
