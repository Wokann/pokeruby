#include "global.h"
#include "battle_anim.h"
#include "trig.h"


void sub_80CBAA4(struct Sprite* sprite);

// heal (healing sparkles on a Pokemon)
// Used in Present, if the move heals instead of damages.

const union AnimCmd gSpriteAnim_83D674C[] =
{
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(4, 4),
    ANIMCMD_FRAME(8, 4),
    ANIMCMD_FRAME(12, 4),
    ANIMCMD_END,
};

const union AnimCmd *const gSpriteAnimTable_83D6760[] =
{
    gSpriteAnim_83D674C,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83D6764 =
{
    .tileTag = ANIM_TAG_GREEN_SPARKLE,
    .paletteTag = ANIM_TAG_GREEN_SPARKLE,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = gSpriteAnimTable_83D6760,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80CBAA4,
};

void sub_80CBAA4(struct Sprite* sprite)
{
    if (sprite->data[0] == 0)
    {
        InitSpritePosToAnimTarget(sprite, FALSE);
        sprite->data[1] = gBattleAnimArgs[2];
    }

    sprite->data[0]++;
    sprite->y2 = sprite->data[1] * sprite->data[0];
    if (sprite->animEnded)
    {
        DestroyAnimSprite(sprite);
    }
}
