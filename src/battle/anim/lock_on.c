#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"
#include "palette.h"
#include "constants/songs.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimLockOnTarget(struct Sprite *sprite);
static void AnimLockOnMoveTarget(struct Sprite *sprite);
static void AnimLockOnTarget_Step1(struct Sprite *sprite);
static void AnimLockOnTarget_Step2(struct Sprite *sprite);
static void AnimLockOnTarget_Step3(struct Sprite *sprite);
static void AnimLockOnTarget_Step4(struct Sprite *sprite);
static void AnimLockOnTarget_Step5(struct Sprite *sprite);
static void AnimLockOnTarget_Step6(struct Sprite *sprite);

// Used by Lock-On.

const struct SpriteTemplate gLockOnTargetSpriteTemplate =
{
    .tileTag = ANIM_TAG_LOCK_ON,
    .paletteTag = ANIM_TAG_LOCK_ON,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimLockOnTarget,
};

const struct SpriteTemplate gLockOnMoveTargetSpriteTemplate =
{
    .tileTag = ANIM_TAG_LOCK_ON,
    .paletteTag = ANIM_TAG_LOCK_ON,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimLockOnMoveTarget,
};

const s8 gInclineMonCoordTable[][2] =
{
    { 64,  64},
    {  0, -64},
    {-64,  64},
    { 32, -32},
};

static void AnimLockOnTarget(struct Sprite *sprite)
{
    sprite->x -= 32;
    sprite->y -= 32;
    sprite->data[0] = 20;
    sprite->callback = WaitAnimForDuration;
    StoreSpriteCallbackInData6(sprite, AnimLockOnTarget_Step1);
}

static void AnimLockOnTarget_Step1(struct Sprite *sprite)
{
    switch (sprite->data[5] & 1)
    {
    case 0:
        sprite->data[0] = 1;
        sprite->callback = WaitAnimForDuration;
        StoreSpriteCallbackInData6(sprite, AnimLockOnTarget_Step1);
        break;
    case 1:
        sprite->x += sprite->x2;
        sprite->y += sprite->y2;
        sprite->y2 = 0;
        sprite->x2 = 0;
        sprite->data[0] = 8;
        sprite->data[2] = sprite->x + gInclineMonCoordTable[sprite->data[5] >> 8][0];
        sprite->data[4] = sprite->y + gInclineMonCoordTable[sprite->data[5] >> 8][1];
        sprite->callback = StartAnimLinearTranslation;
        StoreSpriteCallbackInData6(sprite, AnimLockOnTarget_Step2);
        sprite->data[5] += 0x100;
        PlaySE12WithPanning(SE_M_LOCK_ON, BattleAnimAdjustPanning(SOUND_PAN_TARGET));
        break;
    }

    sprite->data[5] ^= 1;
}

static void AnimLockOnTarget_Step2(struct Sprite *sprite)
{
    if ((sprite->data[5] >> 8) == 4)
    {
        sprite->data[0] = 10;
        sprite->callback = WaitAnimForDuration;
        StoreSpriteCallbackInData6(sprite, AnimLockOnTarget_Step3);
    }
    else
    {
        sprite->callback = AnimLockOnTarget_Step1;
    }
}

static void AnimLockOnTarget_Step3(struct Sprite *sprite)
{
    s16 a;
    s16 b;
    if (sprite->oam.affineParam == 0)
    {
        sprite->data[0] = 3;
        sprite->data[1] = 0;
        sprite->data[2] = 0;
        sprite->callback = WaitAnimForDuration;
        StoreSpriteCallbackInData6(sprite, AnimLockOnTarget_Step4);
    }
    else
    {
        switch (sprite->oam.affineParam)
        {
        case 1:
            a = -8;
            b = -8;
            break;
        case 2:
            a = -8;
            b = 8;
            break;
        case 3:
            a = 8;
            b = -8;
            break;
        default:
            a = 8;
            b = 8;
            break;
        }

        sprite->x += sprite->x2;
        sprite->y += sprite->y2;
        sprite->y2 = 0;
        sprite->x2 = 0;
        sprite->data[0] = 6;
        sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_X_2) + a;
        sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_Y_PIC_OFFSET) + b;
        sprite->callback = StartAnimLinearTranslation;
        StoreSpriteCallbackInData6(sprite, AnimLockOnTarget_Step5);
    }
}

static void AnimLockOnTarget_Step4(struct Sprite *sprite)
{
    if (sprite->data[2] == 0)
    {
        if ((sprite->data[1] += 3) > 16)
            sprite->data[1] = 16;
    }
    else if ((sprite->data[1] -= 3) < 0)
    {
        sprite->data[1] = 0;
    }

    BlendPalettes(GetBattlePalettesMask(TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE), sprite->data[1], RGB(31, 31, 31));
    if (sprite->data[1] == 16)
    {
        int pal;
        sprite->data[2]++;
        pal = sprite->oam.paletteNum;
        LoadPalette(&gPlttBufferUnfaded[0x108 + pal * 16], pal * 16 | 0x101, 4);
        PlaySE12WithPanning(SE_M_LEER, BattleAnimAdjustPanning(SOUND_PAN_TARGET));
    }
    else if (sprite->data[1] == 0)
    {
        sprite->callback = AnimLockOnTarget_Step5;
    }
}

static void AnimLockOnTarget_Step5(struct Sprite *sprite)
{
    if ((u16)gBattleAnimArgs[7] == 0xFFFF)
    {
        sprite->data[1] = 0;
        sprite->data[0] = 0;
        sprite->callback = AnimLockOnTarget_Step6;
    }
}

static void AnimLockOnTarget_Step6(struct Sprite *sprite)
{
    if (sprite->data[0] % 3 == 0)
    {
        sprite->data[1]++;
        sprite->invisible ^= 1;
    }

    sprite->data[0]++;
    if (sprite->data[1] == 8)
        DestroyAnimSprite(sprite);
}

static void AnimLockOnMoveTarget(struct Sprite *sprite)
{
    sprite->oam.affineParam = gBattleAnimArgs[0];
    if ((s16)sprite->oam.affineParam == 1)
    {
        sprite->x -= 0x18;
        sprite->y -= 0x18;
    }
    else if ((s16)sprite->oam.affineParam == 2)
    {
        sprite->x -= 0x18;
        sprite->y += 0x18;
        sprite->oam.matrixNum = 16;
    }
    else if ((s16)sprite->oam.affineParam == 3)
    {
        sprite->x += 0x18;
        sprite->y -= 0x18;
        sprite->oam.matrixNum = 8;
    }
    else
    {
        sprite->x += 0x18;
        sprite->y += 0x18;
        sprite->oam.matrixNum = 24;
    }

    sprite->oam.tileNum = (sprite->oam.tileNum + 16);
    sprite->callback = AnimLockOnTarget;
    AnimLockOnTarget(sprite);
}
