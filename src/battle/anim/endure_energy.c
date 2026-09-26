#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimEndureEnergy(struct Sprite *sprite);
static void AnimEndureEnergy_Step(struct Sprite *sprite);

// Used in Endure.

const union AnimCmd gEndureEnergyAnimCmds[] =
{
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(8, 12),
    ANIMCMD_FRAME(16, 4),
    ANIMCMD_FRAME(24, 4),
    ANIMCMD_END,
};

const union AnimCmd *const gEndureEnergyAnimTable[] =
{
    gEndureEnergyAnimCmds,
};

const struct SpriteTemplate gEndureEnergySpriteTemplate =
{
    .tileTag = ANIM_TAG_FOCUS_ENERGY,
    .paletteTag = ANIM_TAG_FOCUS_ENERGY,
    .oam = &gOamData_AffineOff_ObjNormal_16x32,
    .anims = gEndureEnergyAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimEndureEnergy,
};

static void AnimEndureEnergy(struct Sprite *sprite)
{
    if (gBattleAnimArgs[0] == 0)
    {
        sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_X) + gBattleAnimArgs[1];
        sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_Y) + gBattleAnimArgs[2];
    }
    else
    {
        sprite->x = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_X) + gBattleAnimArgs[1];
        sprite->y = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_Y) + gBattleAnimArgs[2];
    }

    sprite->data[0] = 0;
    sprite->data[1] = gBattleAnimArgs[3];
    sprite->callback = AnimEndureEnergy_Step;
}

static void AnimEndureEnergy_Step(struct Sprite *sprite)
{
    if (++sprite->data[0] > sprite->data[1])
    {
        sprite->data[0] = 0;
        sprite->y--;
    }

    sprite->y -= sprite->data[0];
    if (sprite->animEnded)
        DestroyAnimSprite(sprite);
}
