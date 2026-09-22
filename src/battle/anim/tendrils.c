#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimConstrictBinding(struct Sprite *sprite);
static void AnimConstrictBinding_Step1(struct Sprite *sprite);
static void AnimConstrictBinding_Step2(struct Sprite *sprite);

// tendrils
// Used by Constrict.

static const union AnimCmd sAnim_ConstrictBinding[] =
{
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(32, 4),
    ANIMCMD_FRAME(64, 4),
    ANIMCMD_FRAME(96, 4),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_ConstrictBinding_Flipped[] =
{
    ANIMCMD_FRAME(0,  4, .hFlip = TRUE),
    ANIMCMD_FRAME(32, 4, .hFlip = TRUE),
    ANIMCMD_FRAME(64, 4, .hFlip = TRUE),
    ANIMCMD_FRAME(96, 4, .hFlip = TRUE),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_ConstrictBinding[] =
{
    sAnim_ConstrictBinding,
    sAnim_ConstrictBinding_Flipped,
};

static const union AffineAnimCmd sAffineAnim_ConstrictBinding[] = {
    AFFINEANIMCMD_FRAME(256, 256, 0, 0),
    AFFINEANIMCMD_FRAME(-11, 0, 0, 6),
    AFFINEANIMCMD_FRAME(11, 0, 0, 6),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_ConstrictBinding_Flipped[] = {
    AFFINEANIMCMD_FRAME(-256, 256, 0, 0),
    AFFINEANIMCMD_FRAME(11, 0, 0, 6),
    AFFINEANIMCMD_FRAME(-11, 0, 0, 6),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd *const sAffineAnims_ConstrictBinding[] = {
    sAffineAnim_ConstrictBinding,
    sAffineAnim_ConstrictBinding_Flipped,
};

const struct SpriteTemplate gConstrictBindingSpriteTemplate =
{
    .tileTag = ANIM_TAG_TENDRILS,
    .paletteTag = ANIM_TAG_TENDRILS,
    .oam = &gOamData_AffineNormal_ObjNormal_64x32,
    .anims = sAnims_ConstrictBinding,
    .images = NULL,
    .affineAnims = sAffineAnims_ConstrictBinding,
    .callback = AnimConstrictBinding,
};

static void AnimConstrictBinding(struct Sprite *sprite)
{
    InitSpritePosToAnimTarget(sprite, FALSE);
    sprite->affineAnimPaused = 1;
    StartSpriteAffineAnim(sprite, gBattleAnimArgs[2]);
    sprite->data[6] = gBattleAnimArgs[2];
    sprite->data[7] = gBattleAnimArgs[3];
    sprite->callback = AnimConstrictBinding_Step1;
}

static void AnimConstrictBinding_Step1(struct Sprite *sprite)
{
    if ((u16)gBattleAnimArgs[7] == 0xFFFF)
    {
        sprite->affineAnimPaused = 0;
        GetAnimBattlerSpriteId(1);
        sprite->data[0] = 0x100;
        sprite->callback = AnimConstrictBinding_Step2;
    }
}

static void AnimConstrictBinding_Step2(struct Sprite *sprite)
{
    GetAnimBattlerSpriteId(1);
    if (!sprite->data[2])
    {
        sprite->data[0] += 11;
    }
    else
    {
        sprite->data[0] -= 11;
    }
    sprite->data[1]++;
    if (sprite->data[1] == 6)
    {
        sprite->data[1] = 0;
        sprite->data[2] ^= 1;
    }

    if (sprite->affineAnimEnded)
    {
        sprite->data[7]--;
        if (sprite->data[7] > 0)
        {
            StartSpriteAffineAnim(sprite, sprite->data[6]);
        }
        else
        {
            DestroyAnimSprite(sprite);
        }
    }
}
