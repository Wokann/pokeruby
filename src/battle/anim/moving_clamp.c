#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;
extern const union AffineAnimCmd *const gAffineAnims_Bite[];

static void AnimMovingClamp(struct Sprite *sprite);
static void AnimMovingClamp_Step(struct Sprite *sprite);
static void AnimMovingClamp_End(struct Sprite *sprite);

// Unused

static const struct SpriteTemplate sMovingClampSpriteTemplate =
{
    .tileTag = ANIM_TAG_CLAMP,
    .paletteTag = ANIM_TAG_CLAMP,
    .oam = &gOamData_AffineNormal_ObjBlend_64x64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gAffineAnims_Bite,
    .callback = AnimMovingClamp,
};

static const union AnimCmd sAnim_SmallExplosion[] =
{
    ANIMCMD_FRAME(0, 9),
    ANIMCMD_FRAME(16, 3),
    ANIMCMD_FRAME(32, 3),
    ANIMCMD_FRAME(48, 3),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_SmallExplosion[] =
{
    sAnim_SmallExplosion,
};

static const union AffineAnimCmd sAffineAnim_SmallExplosion[] =
{
    AFFINEANIMCMD_FRAME(0x50, 0x50, 0, 0),
    AFFINEANIMCMD_FRAME(0x9, 0x9, 0, 18),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd *const sAffineAnims_SmallExplosion[] =
{
    sAffineAnim_SmallExplosion,
};

// Unused
static const struct SpriteTemplate sSmallExplosionSpriteTemplate =
{
    .tileTag = ANIM_TAG_EXPLOSION_6,
    .paletteTag = ANIM_TAG_EXPLOSION_6,
    .oam = &gOamData_AffineNormal_ObjNormal_32x32,
    .anims = sAnims_SmallExplosion,
    .images = NULL,
    .affineAnims = sAffineAnims_SmallExplosion,
    .callback = AnimSpriteOnMonPos,
};

static void AnimMovingClamp(struct Sprite *sprite)
{
    InitSpritePosToAnimAttacker(sprite, TRUE);
    sprite->data[0] = gBattleAnimArgs[2];
    sprite->data[1] = gBattleAnimArgs[3];
    sprite->data[5] = gBattleAnimArgs[4];
    sprite->callback = WaitAnimForDuration;
    StoreSpriteCallbackInData6(sprite, AnimMovingClamp_Step);
}

static void AnimMovingClamp_Step(struct Sprite *sprite)
{
    sprite->data[0] = sprite->data[1];
    sprite->data[2] = sprite->x;
    sprite->data[4] = sprite->y + 15;
    sprite->callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(sprite, AnimMovingClamp_End);
}

static void AnimMovingClamp_End(struct Sprite *sprite)
{
    if (sprite->data[5] == 0)
        DestroyAnimSprite(sprite);
    else
        sprite->data[5]--;
}
