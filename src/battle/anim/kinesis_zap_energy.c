#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"
#include "constants/battle.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimKinesisZapEnergy(struct Sprite *sprite);

// Animates a "zap of energy" used in KINESIS.
// arg 0: x pixel offset
// arg 1: y pixel offset
// arg 2: vertical flip

const union AnimCmd gKinesisZapEnergyAnimCmds[] =
{
    ANIMCMD_FRAME(0, 3, .hFlip = TRUE),
    ANIMCMD_FRAME(8, 3, .hFlip = TRUE),
    ANIMCMD_FRAME(16, 3, .hFlip = TRUE),
    ANIMCMD_FRAME(24, 3, .hFlip = TRUE),
    ANIMCMD_FRAME(32, 3, .hFlip = TRUE),
    ANIMCMD_FRAME(40, 3, .hFlip = TRUE),
    ANIMCMD_FRAME(48, 3, .hFlip = TRUE),
    ANIMCMD_LOOP(1),
    ANIMCMD_END,
};

const union AnimCmd *const gKinesisZapEnergyAnimTable[] =
{
    gKinesisZapEnergyAnimCmds,
};

const struct SpriteTemplate gKinesisZapEnergySpriteTemplate =
{
    .tileTag = ANIM_TAG_ALERT,
    .paletteTag = ANIM_TAG_ALERT,
    .oam = &gOamData_AffineOff_ObjNormal_32x16,
    .anims = gKinesisZapEnergyAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimKinesisZapEnergy,
};

static void AnimKinesisZapEnergy(struct Sprite *sprite)
{
    SetSpriteCoordsToAnimAttackerCoords(sprite);
    SetAnimSpriteInitialXOffset(sprite, gBattleAnimArgs[0]);
    sprite->y += gBattleAnimArgs[1];
    if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
    {
        sprite->hFlip = TRUE;
        if (gBattleAnimArgs[2] != 0)
            sprite->vFlip = TRUE;
    }
    else
    {
        if (gBattleAnimArgs[2] != 0)
            sprite->vFlip = TRUE;
    }

    sprite->callback = RunStoredCallbackWhenAnimEnds;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}
