#include "global.h"
#include "battle_anim.h"
#include "rom_8077ABC.h"
#include "sprite.h"
#include "trig.h"

extern s16 gBattleAnimArgs[8];
extern u8 gBattleAnimTarget;

static void AnimDevil(struct Sprite *sprite);

const union AnimCmd gDevilAnimCmds1[] =
{
    ANIMCMD_FRAME(0, 3),
    ANIMCMD_JUMP(0),
};

const union AnimCmd gDevilAnimCmds2[] =
{
    ANIMCMD_FRAME(16, 3),
    ANIMCMD_JUMP(0),
};

const union AnimCmd *const gDevilAnimTable[] =
{
    gDevilAnimCmds1,
    gDevilAnimCmds2,
};

const struct SpriteTemplate gDevilSpriteTemplate =
{
    .tileTag = ANIM_TAG_DEVIL,
    .paletteTag = ANIM_TAG_DEVIL,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gDevilAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimDevil,
};

static void AnimDevil(struct Sprite *sprite)
{
    if (sprite->data[3] == 0)
    {
        sprite->x += gBattleAnimArgs[0];
        sprite->y += gBattleAnimArgs[1];
        StartSpriteAnim(sprite, 0);
        sprite->subpriority = GetBattlerSpriteSubpriority(gBattleAnimTarget) - 1;
        sprite->data[2] = 1;
    }
    sprite->data[0] += sprite->data[2];
    sprite->data[1] = (sprite->data[0] * 4) % 256;
    if (sprite->data[1] < 0)
        sprite->data[1] = 0;
    sprite->x2 = Cos(sprite->data[1], 30 - sprite->data[0] / 4);
    sprite->y2 = Sin(sprite->data[1], 10 - sprite->data[0] / 8);
    if (sprite->data[1] > 128 && sprite->data[2] > 0)
        sprite->data[2] = -1;
    if (sprite->data[1] == 0 && sprite->data[2] < 0)
        sprite->data[2] = 1;
    sprite->data[3]++;
    if (sprite->data[3] < 10 || sprite->data[3] > 0x50)
        sprite->invisible = sprite->data[0] % 2;
    else
        sprite->invisible = FALSE;
    if (sprite->data[3] > 0x5A)
        DestroyAnimSprite(sprite);
}
