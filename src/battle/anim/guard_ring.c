#include "global.h"
#include "battle.h"
#include "battle_anim.h"
#include "rom_8077ABC.h"

extern s16 gBattleAnimArgs[8];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;
extern u16 gBattleTypeFlags;

static void AnimGuardRing(struct Sprite *sprite);

// Guard rings move upward during Safeguard.

const union AffineAnimCmd gGuardRingAffineAnimCmds1[] =
{
    AFFINEANIMCMD_FRAME(0x100, 0x100, 0, 0),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd gGuardRingAffineAnimCmds2[] =
{
    AFFINEANIMCMD_FRAME(0x200, 0x100, 0, 0),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd *const gGuardRingAffineAnimTable[] =
{
    gGuardRingAffineAnimCmds1,
    gGuardRingAffineAnimCmds2,
};

const struct SpriteTemplate gGuardRingSpriteTemplate =
{
    .tileTag = ANIM_TAG_GUARD_RING,
    .paletteTag = ANIM_TAG_GUARD_RING,
    .oam = &gOamData_AffineDouble_ObjBlend_64x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gGuardRingAffineAnimTable,
    .callback = AnimGuardRing,
};

static void AnimGuardRing(struct Sprite *sprite)
{
    if ((gBattleTypeFlags & BATTLE_TYPE_DOUBLE) && IsAnimBankSpriteVisible(BATTLE_PARTNER(gBattleAnimAttacker)))
    {
        SetAverageBattlerPositions(gBattleAnimAttacker, FALSE, &sprite->x, &sprite->y);
        sprite->y += 40;

        StartSpriteAffineAnim(sprite, 1);
    }
    else
    {
        sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_X);
        sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_Y) + 40;
    }

    sprite->data[0] = 13;
    sprite->data[2] = sprite->x;
    sprite->data[4] = sprite->y - 72;

    sprite->callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}
