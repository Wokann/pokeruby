#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimMagentaHeart(struct Sprite *sprite);

// A magenta heart floating upward in a wave pattern.
// Used in Charm, Covet, and infatuation.

const struct SpriteTemplate gMagentaHeartSpriteTemplate =
{
    .tileTag = ANIM_TAG_MAGENTA_HEART,
    .paletteTag = ANIM_TAG_MAGENTA_HEART,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimMagentaHeart,
};

static void AnimMagentaHeart(struct Sprite *sprite)
{
    if (++sprite->data[0] == 1)
        InitSpritePosToAnimAttacker(sprite, FALSE);

    sprite->x2 = Sin(sprite->data[1], 8);
    sprite->y2 = sprite->data[2] >> 8;
    sprite->data[1] = (sprite->data[1] + 7) & 0xFF;
    sprite->data[2] -= 0x80;
    if (sprite->data[0] == 60)
        DestroyAnimSprite(sprite);
}
