#include "global.h"
#include "battle_anim.h"
#include "contest.h"
#include "rom_8077ABC.h"
#include "trig.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimMegahornHorn(struct Sprite *sprite);
static void AnimLeechLifeNeedle(struct Sprite *sprite);
void sub_80DC9A0(struct Sprite *sprite);
void sub_80DCA70(struct Sprite *sprite);
void sub_80DCB38(struct Sprite *sprite);
void AnimTranslateStinger(struct Sprite *sprite);
void AnimMissileArc(struct Sprite *sprite);
static void AnimTailGlowOrb(struct Sprite *sprite);
static void sub_80DCA38(struct Sprite *sprite);
static void sub_80DCAEC(struct Sprite *sprite);
static void sub_80DCB5C(struct Sprite *sprite);
static void sub_80DCBB4(struct Sprite *sprite);
static void AnimMissileArcStep(struct Sprite *sprite);

static const union AffineAnimCmd sAffineAnim_MegahornHorn_0[] =
{
    AFFINEANIMCMD_FRAME(0x100, 0x100, 30, 0),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_MegahornHorn_1[] =
{
    AFFINEANIMCMD_FRAME(0x100, 0x100, -99, 0),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_MegahornHorn_2[] =
{
    AFFINEANIMCMD_FRAME(0x100, 0x100, 94, 0),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd *const sAffineAnims_MegahornHorn[] =
{
    sAffineAnim_MegahornHorn_0,
    sAffineAnim_MegahornHorn_1,
    sAffineAnim_MegahornHorn_2,
};

const struct SpriteTemplate gMegahornHornSpriteTemplate =
{
    .tileTag = ANIM_TAG_HORN_HIT_2,
    .paletteTag = ANIM_TAG_HORN_HIT_2,
    .oam = &gOamData_AffineDouble_ObjNormal_32x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = sAffineAnims_MegahornHorn,
    .callback = AnimMegahornHorn,
};

static const union AffineAnimCmd sAffineAnim_LeechLifeNeedle_0[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, -33, 1),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_LeechLifeNeedle_1[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, 96, 1),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_LeechLifeNeedle_2[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, -96, 1),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd *const sAffineAnims_LeechLifeNeedle[] =
{
    sAffineAnim_LeechLifeNeedle_0,
    sAffineAnim_LeechLifeNeedle_1,
    sAffineAnim_LeechLifeNeedle_2,
};

const struct SpriteTemplate gLeechLifeNeedleSpriteTemplate =
{
    .tileTag = ANIM_TAG_NEEDLE,
    .paletteTag = ANIM_TAG_NEEDLE,
    .oam = &gOamData_AffineNormal_ObjNormal_16x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = sAffineAnims_LeechLifeNeedle,
    .callback = AnimLeechLifeNeedle,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83DAB28 =
{
    .tileTag = ANIM_TAG_STRING_DOT,
    .paletteTag = ANIM_TAG_STRING_DOT,
    .oam = &gOamData_AffineOff_ObjNormal_8x8,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80DC9A0,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83DAB40 =
{
    .tileTag = ANIM_TAG_STRING,
    .paletteTag = ANIM_TAG_STRING,
    .oam = &gOamData_837DF5C,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80DCA70,
};

const union AffineAnimCmd gSpriteAffineAnim_83DAB58[] =
{
    AFFINEANIMCMD_FRAME(0x10, 0x10, 0, 0),
    AFFINEANIMCMD_FRAME(0x6, 0x6, 0, 1),
    AFFINEANIMCMD_JUMP(1),
};

const union AffineAnimCmd *const gSpriteAffineAnimTable_83DAB70[] =
{
    gSpriteAffineAnim_83DAB58,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83DAB74 =
{
    .tileTag = ANIM_TAG_WEB,
    .paletteTag = ANIM_TAG_WEB,
    .oam = &gOamData_AffineDouble_ObjBlend_64x64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gSpriteAffineAnimTable_83DAB70,
    .callback = sub_80DCB38,
};

const struct SpriteTemplate gLinearStingerSpriteTemplate =
{
    .tileTag = ANIM_TAG_NEEDLE,
    .paletteTag = ANIM_TAG_NEEDLE,
    .oam = &gOamData_AffineNormal_ObjNormal_16x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimTranslateStinger,
};

const struct SpriteTemplate gPinMissileSpriteTemplate =
{
    .tileTag = ANIM_TAG_NEEDLE,
    .paletteTag = ANIM_TAG_NEEDLE,
    .oam = &gOamData_AffineNormal_ObjNormal_16x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimMissileArc,
};

const struct SpriteTemplate gIcicleSpearSpriteTemplate =
{
    .tileTag = ANIM_TAG_ICICLE_SPEAR,
    .paletteTag = ANIM_TAG_ICICLE_SPEAR,
    .oam = &gOamData_AffineNormal_ObjNormal_32x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimMissileArc,
};

static const union AffineAnimCmd sAffineAnim_TailGlowOrb[] =
{
    AFFINEANIMCMD_FRAME(0x10, 0x10, 0, 0),
    AFFINEANIMCMD_FRAME(0x8, 0x8, 0, 18),
    AFFINEANIMCMD_LOOP(0),
    AFFINEANIMCMD_FRAME(0xFFFB, 0xFFFB, 0, 8),
    AFFINEANIMCMD_FRAME(0x5, 0x5, 0, 8),
    AFFINEANIMCMD_LOOP(5),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd *const sAffineAnims_TailGlowOrb[] =
{
    sAffineAnim_TailGlowOrb,
};

const struct SpriteTemplate gTailGlowOrbSpriteTemplate =
{
    .tileTag = ANIM_TAG_CIRCLE_OF_LIGHT,
    .paletteTag = ANIM_TAG_CIRCLE_OF_LIGHT,
    .oam = &gOamData_AffineNormal_ObjBlend_64x64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = sAffineAnims_TailGlowOrb,
    .callback = AnimTailGlowOrb,
};

// used in Move_MEGAHORN
static void AnimMegahornHorn(struct Sprite *sprite)
{
    if (IsContest())
    {
        StartSpriteAffineAnim(sprite, 2);
        gBattleAnimArgs[2] = -gBattleAnimArgs[2];
        gBattleAnimArgs[0] = -gBattleAnimArgs[0];
    }
    else if (!GetBattlerSide(gBattleAnimTarget))
    {
        StartSpriteAffineAnim(sprite, 1);
        gBattleAnimArgs[1] = -gBattleAnimArgs[1];
        gBattleAnimArgs[2] = -gBattleAnimArgs[2];
        gBattleAnimArgs[3] = -gBattleAnimArgs[3];
        gBattleAnimArgs[0] = -gBattleAnimArgs[0];
    }

    sprite->x = GetBattlerSpriteCoord2(gBattleAnimTarget, BATTLER_COORD_X_2) + gBattleAnimArgs[0];
    sprite->y = GetBattlerSpriteCoord2(gBattleAnimTarget, BATTLER_COORD_Y_PIC_OFFSET) + gBattleAnimArgs[1];
    sprite->data[0] = gBattleAnimArgs[4];

    sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimTarget, 2) + gBattleAnimArgs[2];
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimTarget, 3) + gBattleAnimArgs[3];

    sprite->callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}

// used in Move_LEECH_LIFE
static void AnimLeechLifeNeedle(struct Sprite *sprite)
{
    if (IsContest())
    {
        gBattleAnimArgs[0] = -gBattleAnimArgs[0];
        StartSpriteAffineAnim(sprite, 2);
    }
    else if (!GetBattlerSide(gBattleAnimTarget))
    {
        gBattleAnimArgs[1] = -gBattleAnimArgs[1];
        gBattleAnimArgs[0] = -gBattleAnimArgs[0];
    }

    sprite->x = GetBattlerSpriteCoord2(gBattleAnimTarget, BATTLER_COORD_X_2) + gBattleAnimArgs[0];
    sprite->y = GetBattlerSpriteCoord2(gBattleAnimTarget, BATTLER_COORD_Y_PIC_OFFSET) + gBattleAnimArgs[1];
    sprite->data[0] = gBattleAnimArgs[2];

    sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimTarget, 2);
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimTarget, 3);

    sprite->callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}

// used in 2 moves:
//         Move_STRING_SHOT, Move_SPIDER_WEB
void sub_80DC9A0(struct Sprite *sprite)
{
    if (IsContest())
        gBattleAnimArgs[2] /= 2;

    InitSpritePosToAnimAttacker(sprite, 1);
    sprite->data[0] = gBattleAnimArgs[2];
    sprite->data[1] = sprite->x;
    sprite->data[3] = sprite->y;

    if (!gBattleAnimArgs[4])
    {
        sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimTarget, 2);
        sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimTarget, 3);
    }
    else
    {
        SetAverageBattlerPositions(gBattleAnimTarget, 1, &sprite->data[2], &sprite->data[4]);
    }

    InitAnimLinearTranslationWithSpeed(sprite);
    sprite->data[5] = gBattleAnimArgs[3];
    sprite->callback = sub_80DCA38;
}

static void sub_80DCA38(struct Sprite *sprite)
{
    if (AnimTranslateLinear(sprite))
    {
        DestroyAnimSprite(sprite);
        return;
    }

    sprite->x2 += Sin(sprite->data[6], sprite->data[5]);
    sprite->data[6] = (sprite->data[6] + 13) & 0xFF;
}

// used in Move_STRING_SHOT
void sub_80DCA70(struct Sprite *sprite)
{
    SetAverageBattlerPositions(gBattleAnimTarget, 0, &sprite->x, &sprite->y);
    if (GetBattlerSide(gBattleAnimAttacker))
        sprite->x -= gBattleAnimArgs[0];
    else
        sprite->x += gBattleAnimArgs[0];

    sprite->y += gBattleAnimArgs[1];
    if (!GetBattlerSide(gBattleAnimTarget))
        sprite->y += 8;

    sprite->callback = sub_80DCAEC;
}

static void sub_80DCAEC(struct Sprite *sprite)
{
    if (++sprite->data[0] == 3)
    {
        sprite->data[0] = 0;
        sprite->invisible ^= 1;
    }

    if (++sprite->data[1] == 51)
    {
        DestroyAnimSprite(sprite);
    }
}

// used in Move_SPIDER_WEB
void sub_80DCB38(struct Sprite *sprite)
{
    REG_BLDCNT = 0x3F40;
    REG_BLDALPHA = 16;

    sprite->data[0] = 16;
    sprite->callback = sub_80DCB5C;
}

static void sub_80DCB5C(struct Sprite *sprite)
{
    if (sprite->data[2] < 20)
    {
        sprite->data[2]++;
    }
    else if (sprite->data[1]++ & 1)
    {
        sprite->data[0]--;
        REG_BLDALPHA = sprite->data[0] | ((16 - sprite->data[0]) << 8);

        if (sprite->data[0] == 0)
        {
            sprite->invisible = TRUE;
            sprite->callback = sub_80DCBB4;
        }
    }
}

static void sub_80DCBB4(struct Sprite *sprite)
{
    REG_BLDCNT = 0;
    REG_BLDALPHA = 0;
    DestroyAnimSprite(sprite);
}

// Translates a stinger sprite linearly to a destination location. The sprite is
// initially rotated so that it appears to be traveling in a straight line.
// Used by Move_POISON_STING, Move_TWINEEDLE, Move_SPIKE_CANNON
// arg 0: initial x pixel offset
// arg 1: initial y pixel offset
// arg 2: target x pixel offset
// arg 3: target y pixel offset
// arg 4: duration
void AnimTranslateStinger(struct Sprite *sprite)
{
    s16 lVarX, lVarY;
    u16 rot;

    if (IsContest())
    {
        gBattleAnimArgs[2] = -gBattleAnimArgs[2];
    }
    else
    {
        if (GetBattlerSide(gBattleAnimAttacker))
        {
            gBattleAnimArgs[2] = -gBattleAnimArgs[2];
            gBattleAnimArgs[1] = -gBattleAnimArgs[1];
            gBattleAnimArgs[3] = -gBattleAnimArgs[3];
        }
    }

    if (!IsContest())
    {
        if (GetBattlerSide(gBattleAnimAttacker) == GetBattlerSide(gBattleAnimTarget))
        {
            if (GetBattlerPosition(gBattleAnimTarget) == 0 || GetBattlerPosition(gBattleAnimTarget) == 1)
            {
                s16 temp1, temp2;

                temp1 = gBattleAnimArgs[2];
                gBattleAnimArgs[2] = -temp1;

                temp2 = gBattleAnimArgs[0];
                gBattleAnimArgs[0] = -temp2;
            }
        }
    }

    InitSpritePosToAnimAttacker(sprite, 1);

    lVarX = GetBattlerSpriteCoord(gBattleAnimTarget, 2) + gBattleAnimArgs[2];
    lVarY = GetBattlerSpriteCoord(gBattleAnimTarget, 3) + gBattleAnimArgs[3];
    rot = ArcTan2Neg(lVarX - sprite->x, lVarY - sprite->y);
    rot += 0xC000;
    TrySetSpriteRotScale(sprite, FALSE, 0x100, 0x100, rot);

    sprite->data[0] = gBattleAnimArgs[4];
    sprite->data[2] = lVarX;
    sprite->data[4] = lVarY;

    sprite->callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}

// used in 2 moves:
//         Move_PIN_MISSILE, Move_ICICLE_SPEAR
// Rotates sprite and moves it in an arc, so that it appears like a missle or arrow traveling.
// arg 0: initial x pixel offset
// arg 1: initial y pixel offset
// arg 2: target x pixel offset
// arg 3: target y pixel offset
// arg 4: duration
// arg 5: wave amplitude
void AnimMissileArc(struct Sprite *sprite)
{
    InitSpritePosToAnimAttacker(sprite, 1);

    if (GetBattlerSide(gBattleAnimAttacker))
        gBattleAnimArgs[2] = -gBattleAnimArgs[2];

    sprite->data[0] = gBattleAnimArgs[4];
    sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimTarget, 2) + gBattleAnimArgs[2];
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimTarget, 3) + gBattleAnimArgs[3];
    sprite->data[5] = gBattleAnimArgs[5];
    InitAnimArcTranslation(sprite);

    sprite->callback = AnimMissileArcStep;
    sprite->invisible = TRUE;
}

static void AnimMissileArcStep(struct Sprite *sprite)
{
    sprite->invisible = FALSE;

    if (TranslateAnimArc(sprite))
    {
        DestroyAnimSprite(sprite);
    }
    else
    {
        s16 tempData[8];
        u16 *data = sprite->data;
        u16 x1 = sprite->x;
        s16 x2 = sprite->x2;
        u16 y1 = sprite->y;
        s16 y2 = sprite->y2;
        int i;

        for (i = 0; i < 8; i++)
            tempData[i] = data[i];

        x2 += x1;
        y2 += y1;

        if (!TranslateAnimArc(sprite))
        {
            u16 rotation = ArcTan2Neg(sprite->x + sprite->x2 - x2,
                                  sprite->y + sprite->y2 - y2);
            rotation += 0xC000;
            TrySetSpriteRotScale(sprite, FALSE, 0x100, 0x100, rotation);

            for (i = 0; i < 8; i++)
                data[i] = tempData[i];
        }
    }
}

static void AnimTailGlowOrb(struct Sprite *sprite)
{
    if (gBattleAnimArgs[0] == ANIM_BATTLER_ATTACKER)
    {
        sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_X_2);
        sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_Y_PIC_OFFSET) + 18;
    }
    else
    {
        sprite->x = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_X_2);
        sprite->y = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_Y_PIC_OFFSET) + 18;
    }

    StoreSpriteCallbackInData6(sprite, DestroySpriteAndMatrix);
    sprite->callback = RunStoredCallbackWhenAffineAnimEnds;
}
