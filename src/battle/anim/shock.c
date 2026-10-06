#include "global.h"
#include "battle_anim.h"
#include "trig.h"

extern struct OamMatrix gOamMatrices[];

static void AnimUnusedCirclingShock(struct Sprite *sprite);
static void AnimSparkElectricity(struct Sprite *sprite);

static const union AnimCmd sAnim_UnusedCirclingShock[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_FRAME(16, 5),
    ANIMCMD_FRAME(32, 5),
    ANIMCMD_FRAME(48, 5),
    ANIMCMD_FRAME(64, 5),
    ANIMCMD_FRAME(80, 5),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd *const sAnims_UnusedCirclingShock[] =
{
    sAnim_UnusedCirclingShock,
};

// Unused
static const struct SpriteTemplate sUnusedCirclingShockSpriteTemplate =
{
    .tileTag = ANIM_TAG_SHOCK,
    .paletteTag = ANIM_TAG_SHOCK,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = sAnims_UnusedCirclingShock,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimUnusedCirclingShock,
};

const struct SpriteTemplate gSparkElectricitySpriteTemplate =
{
    .tileTag = ANIM_TAG_SPARK_2,
    .paletteTag = ANIM_TAG_SPARK_2,
    .oam = &gOamData_AffineNormal_ObjNormal_16x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimSparkElectricity,
};

const struct SpriteTemplate gZapCannonBallSpriteTemplate =
{
    .tileTag = ANIM_TAG_BLACK_BALL_2,
    .paletteTag = ANIM_TAG_BLACK_BALL_2,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = TranslateAnimSpriteToTargetMonLocation,
};

static void AnimUnusedCirclingShock(struct Sprite *sprite)
{
    sprite->x = GetBattlerSpriteCoord(gBattleAnimTarget, 2);
    sprite->y = GetBattlerSpriteCoord(gBattleAnimTarget, 3);

    if (GetBattlerSide(gBattleAnimAttacker) != 0)
    {
        sprite->x -= gBattleAnimArgs[0];
        sprite->y -= gBattleAnimArgs[1];
    }
    else
    {
        sprite->x += gBattleAnimArgs[0];
        sprite->y += gBattleAnimArgs[1];
    }

    sprite->data[0] = 0;
    sprite->data[1] = gBattleAnimArgs[2];
    sprite->data[2] = gBattleAnimArgs[3];
    sprite->data[3] = gBattleAnimArgs[4];

    StoreSpriteCallbackInData6(sprite, DestroySpriteAndMatrix);
    sprite->callback = TranslateSpriteInCircle;
}

static void AnimSparkElectricity(struct Sprite *sprite)
{
    u8 battler;
    u32 matrixNum;
    s16 sineVal;

    switch (gBattleAnimArgs[4])
    {
    case 0:
        battler = gBattleAnimAttacker;
        break;
    case 1:
    default:
        battler = gBattleAnimTarget;
        break;
    case 2:
        if (!IsBattlerSpriteVisible(gBattleAnimAttacker ^ 2))
        {
            battler = gBattleAnimAttacker;
        }
        else
        {
            battler = gBattleAnimAttacker ^ 2;
        }
        break;
    case 3:
        if (IsBattlerSpriteVisible(gBattleAnimAttacker ^ 2))
        {
            battler = gBattleAnimTarget ^ 2;
        }
        else
        {
            battler = gBattleAnimTarget;
        }
        break;
    }

    if (gBattleAnimArgs[5] == 0)
    {
        sprite->x = GetBattlerSpriteCoord(battler, 0);
        sprite->y = GetBattlerSpriteCoord(battler, 1);
    }
    else
    {
        sprite->x = GetBattlerSpriteCoord(battler, 2);
        sprite->y = GetBattlerSpriteCoord(battler, 3);
    }

    sprite->x2 = (gSineTable[gBattleAnimArgs[0]] * gBattleAnimArgs[1]) >> 8;
    sprite->y2 = (gSineTable[gBattleAnimArgs[0] + 64] * gBattleAnimArgs[1]) >> 8;

    if (gBattleAnimArgs[6] & 1)
    {
        sprite->oam.priority = GetBattlerSpriteBGPriority(battler) + 1;
    }

    matrixNum = sprite->oam.matrixNum;
    sineVal = gSineTable[gBattleAnimArgs[2]];

    gOamMatrices[matrixNum].a = gOamMatrices[matrixNum].d =  gSineTable[gBattleAnimArgs[2] + 64];
    gOamMatrices[matrixNum].b =  sineVal;
    gOamMatrices[matrixNum].c = -sineVal;

    sprite->data[0] = gBattleAnimArgs[3];
    sprite->callback = DestroyAnimSpriteAfterTimer;
}
