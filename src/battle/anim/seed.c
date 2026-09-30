#include "global.h"
#include "trig.h"
#include "battle_anim.h"
#include "battle_anim_internal.h"
#include "constants/battle.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimLeechSeed(struct Sprite *sprite);
static void AnimLeechSeed_Step(struct Sprite *sprite);
static void AnimLeechSeedSprouts(struct Sprite *sprite);

const union AnimCmd gLeechSeedAnimCmds1[] =
{
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END,
};

const union AnimCmd gLeechSeedAnimCmds2[] =
{
    ANIMCMD_FRAME(4, 7),
    ANIMCMD_FRAME(8, 7),
    ANIMCMD_JUMP(0),
};

const union AnimCmd *const gLeechSeedAnimTable[] =
{
    gLeechSeedAnimCmds1,
    gLeechSeedAnimCmds2,
};

const struct SpriteTemplate gLeechSeedSpriteTemplate =
{
    .tileTag = ANIM_TAG_SEED,
    .paletteTag = ANIM_TAG_SEED,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = gLeechSeedAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimLeechSeed,
};

// seed (sprouts a sapling from a seed.)
// Used by Leech Seed.
// arg 0: initial x pixel offset
// arg 1: initial y pixel offset
// arg 2: target x pixel offset
// arg 3: target y pixel offset
// arg 4: duration
// arg 5: wave amplitude
static void AnimLeechSeed(struct Sprite *sprite)
{
    CMD_ARGS(initialX, initialY, targetX, targetY, duration, waveAmplitude);

    InitSpritePosToAnimAttacker(sprite, TRUE);
    if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
    {
        cmd->targetX = -cmd->targetX;
    }

    sprite->data[0] = cmd->duration;
    sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_X) + cmd->targetX;
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_Y) + cmd->targetY;
    sprite->data[5] = cmd->waveAmplitude;
    InitAnimArcTranslation(sprite);
    sprite->callback = AnimLeechSeed_Step;
}

static void AnimLeechSeed_Step(struct Sprite *sprite)
{
    if (TranslateAnimHorizontalArc(sprite))
    {
        sprite->invisible = TRUE;
        sprite->data[0] = 10;
        sprite->callback = WaitAnimForDuration;
        StoreSpriteCallbackInData6(sprite, AnimLeechSeedSprouts);
    }
}

static void AnimLeechSeedSprouts(struct Sprite *sprite)
{
    sprite->invisible = FALSE;
    StartSpriteAnim(sprite, 1);
    sprite->data[0] = 60;
    sprite->callback = WaitAnimForDuration;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}
