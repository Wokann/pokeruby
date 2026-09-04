#include "global.h"
#include "battle_anim.h"
#include "rom_8077ABC.h"
#include "trig.h"

extern s16 gBattleAnimArgs[8];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;
extern struct OamMatrix gOamMatrices[];

void sub_80D6294(struct Sprite *sprite);
static void AnimSparkElectricity(struct Sprite *sprite);

// shock (moves the little electricity lines)
// Used in Shock.

const union AnimCmd gSpriteAnim_83D9824[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_FRAME(16, 5),
    ANIMCMD_FRAME(32, 5),
    ANIMCMD_FRAME(48, 5),
    ANIMCMD_FRAME(64, 5),
    ANIMCMD_FRAME(80, 5),
    ANIMCMD_JUMP(0),
};

const union AnimCmd *const gSpriteAnimTable_83D9840[] =
{
    gSpriteAnim_83D9824,
};

const struct SpriteTemplate gSpriteTemplate_83D9844 =
{
    .tileTag = ANIM_TAG_SHOCK,
    .paletteTag = ANIM_TAG_SHOCK,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gSpriteAnimTable_83D9840,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80D6294,
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

void sub_80D6294(struct Sprite *sprite)
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
    sprite->callback = sub_8078114;
}

static void AnimSparkElectricity(struct Sprite *sprite)
{
    u8 battler;
    u32 matrixNum;
    s16 sineVal;

    switch (gBattleAnimArgs[4])
    {
    case ANIM_BATTLER_ATTACKER:
        battler = gBattleAnimAttacker;
        break;
    case ANIM_BATTLER_TARGET:
    default:
        battler = gBattleAnimTarget;
        break;
    case ANIM_BATTLER_ATK_PARTNER:
        if (!IsAnimBankSpriteVisible(gBattleAnimAttacker ^ 2))
        {
            battler = gBattleAnimAttacker;
        }
        else
        {
            battler = gBattleAnimAttacker ^ 2;
        }
        break;
    case ANIM_BATTLER_DEF_PARTNER:
        if (IsAnimBankSpriteVisible(gBattleAnimAttacker ^ 2))
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
        sprite->x = GetBattlerSpriteCoord(battler, BATTLER_COORD_X);
        sprite->y = GetBattlerSpriteCoord(battler, BATTLER_COORD_Y);
    }
    else
    {
        sprite->x = GetBattlerSpriteCoord(battler, BATTLER_COORD_X_2);
        sprite->y = GetBattlerSpriteCoord(battler, BATTLER_COORD_Y_PIC_OFFSET);
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
