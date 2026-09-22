#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimAngel(struct Sprite* sprite);

// angel (a little angel descends from somewhere towards a position)
// Used in Sweet Kiss.

const union AnimCmd gAngelSpriteAnimCmds[] =
{
    ANIMCMD_FRAME(0, 24),
    ANIMCMD_END,
};

const union AnimCmd *const gAngelSpriteAnimTable[] =
{
    gAngelSpriteAnimCmds,
};

const struct SpriteTemplate gAngelSpriteTemplate =
{
    .tileTag = ANIM_TAG_ANGEL,
    .paletteTag = ANIM_TAG_ANGEL,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gAngelSpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimAngel,
};

static void AnimAngel(struct Sprite* sprite)
{
    s16 angle;
    if (sprite->data[0] == 0)
    {
        sprite->x += gBattleAnimArgs[0];
        sprite->y += gBattleAnimArgs[1];
    }

    sprite->data[0]++;
    angle = (sprite->data[0] * 10) & 0xFF;
    sprite->x2 = Sin(angle, 0x50) >> 8;
    if (sprite->data[0] <= 0x4F)
        sprite->y2 = (sprite->data[0] / 2) + (Cos(angle, 0x50) >> 8);

    if (sprite->data[0] > 0x5A)
    {
        sprite->data[2]++;
        sprite->x2 -= sprite->data[2] / 2;
    }

    if (sprite->data[0] > 0x64)
        DestroyAnimSprite(sprite);
}
