#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

void AnimPresentHealParticle(struct Sprite* sprite);

// heal (healing sparkles on a Pokemon)
// Used in Present, if the move heals instead of damages.

const union AnimCmd gPresentHealParticleAnimCmds[] =
{
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(4, 4),
    ANIMCMD_FRAME(8, 4),
    ANIMCMD_FRAME(12, 4),
    ANIMCMD_END,
};

const union AnimCmd *const gPresentHealParticleAnimTable[] =
{
    gPresentHealParticleAnimCmds,
};

const struct SpriteTemplate gPresentHealParticleSpriteTemplate =
{
    .tileTag = ANIM_TAG_GREEN_SPARKLE,
    .paletteTag = ANIM_TAG_GREEN_SPARKLE,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = gPresentHealParticleAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimPresentHealParticle,
};

void AnimPresentHealParticle(struct Sprite* sprite)
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
