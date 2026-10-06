#include "global.h"
#include "battle_anim.h"
#include "trig.h"
#include "sound.h"


static void AnimRazorWindTornado(struct Sprite *sprite);

// cyclone (creates a circling motion like a cyclone, usually a wind sprite.)
// Used in Razor Wind.

const union AffineAnimCmd gRazorWindTornadoAffineAnimCmds[] =
{
    AFFINEANIMCMD_FRAME(0x10, 0x100, 0, 0),
    AFFINEANIMCMD_FRAME(0x4, 0x0, 0, 40),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd *const gRazorWindTornadoAffineAnimTable[] =
{
    gRazorWindTornadoAffineAnimCmds,
};

const struct SpriteTemplate gRazorWindTornadoSpriteTemplate =
{
    .tileTag = ANIM_TAG_GUST,
    .paletteTag = ANIM_TAG_GUST,
    .oam = &gOamData_AffineNormal_ObjNormal_32x64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gRazorWindTornadoAffineAnimTable,
    .callback = AnimRazorWindTornado,
};

static void AnimRazorWindTornado(struct Sprite *sprite)
{
    InitSpritePosToAnimAttacker(sprite, 0);
    if (GetBattlerSide(gBattleAnimAttacker) == 0)
        sprite->y += 16;

    sprite->data[0] = gBattleAnimArgs[4];
    sprite->data[1] = gBattleAnimArgs[2];
    sprite->data[2] = gBattleAnimArgs[5];
    sprite->data[3] = gBattleAnimArgs[6];
    sprite->data[4] = gBattleAnimArgs[3];
    sprite->callback = TranslateSpriteInCircle;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
    sprite->callback(sprite);
}
