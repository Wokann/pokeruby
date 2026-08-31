#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimGrantingStars(struct Sprite* sprite);
static void AnimSparklingStars(struct Sprite* sprite);

// glitter (the sparkling effect seen on Pokemon, usually after healing or a beneficial effect.)
// Used by Heal Bell, Cosmic Power, and Aromatherapy.

const union AnimCmd gGrantingStarsAnimCmds[] =
{
    ANIMCMD_FRAME(0, 7),
    ANIMCMD_FRAME(16, 7),
    ANIMCMD_FRAME(32, 7),
    ANIMCMD_FRAME(48, 7),
    ANIMCMD_FRAME(64, 7),
    ANIMCMD_FRAME(80, 7),
    ANIMCMD_FRAME(96, 7),
    ANIMCMD_FRAME(112, 7),
    ANIMCMD_JUMP(0),
};

const union AnimCmd *const gGrantingStarsAnimTable[] =
{
    gGrantingStarsAnimCmds,
};

const struct SpriteTemplate gGrantingStarsSpriteTemplate =
{
    .tileTag = ANIM_TAG_SPARKLE_2,
    .paletteTag = ANIM_TAG_SPARKLE_2,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gGrantingStarsAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimGrantingStars,
};

const struct SpriteTemplate gSparklingStarsSpriteTemplate =
{
    .tileTag = ANIM_TAG_SPARKLE_2,
    .paletteTag = ANIM_TAG_SPARKLE_2,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gGrantingStarsAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimSparklingStars,
};

static void AnimGrantingStars(struct Sprite* sprite)
{
    if (!gBattleAnimArgs[2])
        SetSpriteCoordsToAnimAttackerCoords(sprite);

    SetAnimSpriteInitialXOffset(sprite, gBattleAnimArgs[0]);
    sprite->y += gBattleAnimArgs[1];
    sprite->data[0] = gBattleAnimArgs[5];
    sprite->data[1] = gBattleAnimArgs[3];
    sprite->data[2] = gBattleAnimArgs[4];
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
    sprite->callback = TranslateSpriteLinearFixedPoint;
}

static void AnimSparklingStars(struct Sprite* sprite)
{
    u8 bank;
    if (!gBattleAnimArgs[2])
        bank = gBattleAnimAttacker;
    else
        bank = gBattleAnimTarget;

    if (IsDoubleBattle() && IsAnimBankSpriteVisible(bank ^ 2))
    {
        SetAverageBattlerPositions(bank, gBattleAnimArgs[6], &sprite->x, &sprite->y);
        SetAnimSpriteInitialXOffset(sprite, gBattleAnimArgs[0]);
        sprite->y += gBattleAnimArgs[1];
    }
    else
    {
        if (!gBattleAnimArgs[6])
        {
            sprite->x = GetBattlerSpriteCoord(bank, 0);
            sprite->y = GetBattlerSpriteCoord(bank, 1) + gBattleAnimArgs[1];
        }
        else
        {
            sprite->x = GetBattlerSpriteCoord(bank, 2);
            sprite->y = GetBattlerSpriteCoord(bank, 3) + gBattleAnimArgs[1];
        }

        SetAnimSpriteInitialXOffset(sprite, gBattleAnimArgs[0]);
    }

    sprite->data[0] = gBattleAnimArgs[5];
    sprite->data[1] = gBattleAnimArgs[3];
    sprite->data[2] = gBattleAnimArgs[4];
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
    sprite->callback = TranslateSpriteLinearFixedPoint;
}
