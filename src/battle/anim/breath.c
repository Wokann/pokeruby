#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"
#include "constants/battle.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimBreathPuff(struct Sprite *sprite);

// Animates a little puff of the mon's breath.
// Used by Swagger and Bulk Up.
// No args.

const union AnimCmd gBreathPuffAnimCmds1[] =
{
    ANIMCMD_FRAME(0, 4, .hFlip = TRUE),
    ANIMCMD_FRAME(4, 40, .hFlip = TRUE),
    ANIMCMD_FRAME(8, 4, .hFlip = TRUE),
    ANIMCMD_FRAME(12, 4, .hFlip = TRUE),
    ANIMCMD_END,
};

const union AnimCmd gBreathPuffAnimCmds2[] =
{
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(4, 40),
    ANIMCMD_FRAME(8, 4),
    ANIMCMD_FRAME(12, 4),
    ANIMCMD_END,
};

const union AnimCmd *const gBreathPuffAnimTable[] =
{
    gBreathPuffAnimCmds1,
    gBreathPuffAnimCmds2,
};

const struct SpriteTemplate gBreathPuffSpriteTemplate =
{
    .tileTag = ANIM_TAG_BREATH,
    .paletteTag = ANIM_TAG_BREATH,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = gBreathPuffAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimBreathPuff,
};

static void AnimBreathPuff(struct Sprite *sprite)
{
    if (GetBattlerSide(gBattleAnimAttacker) == B_SIDE_PLAYER)
    {
        StartSpriteAnim(sprite, 0);
        sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_X_2) + 32;
        sprite->data[1] = 64;
    }
    else
    {
        StartSpriteAnim(sprite, 1);
        sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_X_2) - 32;
        sprite->data[1] = -64;
    }

    sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_Y_PIC_OFFSET);
    sprite->data[0] = 52;
    sprite->data[2] = 0;
    sprite->data[3] = 0;
    sprite->data[4] = 0;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
    sprite->callback = TranslateSpriteLinearFixedPoint;
}
