#include "global.h"
#include "battle_anim.h"
#include "trig.h"
#include "sound.h"
#include "battle.h"
#include "math_util.h"

extern u16 gBattleTypeFlags;

static void AnimSonicBoomProjectile(struct Sprite *sprite);
static void AnimAirWaveProjectile(struct Sprite *sprite);

const struct SpriteTemplate gSonicBoomSpriteTemplate =
{
    .tileTag = ANIM_TAG_AIR_WAVE,
    .paletteTag = ANIM_TAG_AIR_WAVE,
    .oam = &gOamData_AffineDouble_ObjBlend_32x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimSonicBoomProjectile,
};

const struct SpriteTemplate gAirWaveProjectileSpriteTemplate =
{
    .tileTag = ANIM_TAG_AIR_WAVE,
    .paletteTag = ANIM_TAG_AIR_WAVE,
    .oam = &gOamData_AffineOff_ObjBlend_32x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimAirWaveProjectile,
};

const union AffineAnimCmd gGrowingRingAffineAnimCmds[] =
{
    AFFINEANIMCMD_FRAME(0x20, 0x20, 0, 0),
    AFFINEANIMCMD_FRAME(0x7, 0x7, 0, -56),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd gWaterPulseRingAffineAnimCmds[] =
{
    AFFINEANIMCMD_FRAME(0x5, 0x5, 0, 10),
    AFFINEANIMCMD_FRAME(0xFFF6, 0xFFF6, 0, 10),
    AFFINEANIMCMD_FRAME(0xA, 0xA, 0, 10),
    AFFINEANIMCMD_FRAME(0xFFF6, 0xFFF6, 0, 10),
    AFFINEANIMCMD_FRAME(0xA, 0xA, 0, 10),
    AFFINEANIMCMD_FRAME(0xFFF6, 0xFFF6, 0, 10),
    AFFINEANIMCMD_FRAME(0xA, 0xA, 0, 10),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd *const gGrowingRingAffineAnimTable[] =
{
    gGrowingRingAffineAnimCmds,
};

const union AffineAnimCmd *const gWaterPulseRingAffineAnimTable[] =
{
    gWaterPulseRingAffineAnimCmds,
};

const struct SpriteTemplate gSupersonicRingSpriteTemplate =
{
    .tileTag = ANIM_TAG_GOLD_RING,
    .paletteTag = ANIM_TAG_GOLD_RING,
    .oam = &gOamData_AffineDouble_ObjNormal_16x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gGrowingRingAffineAnimTable,
    .callback = TranslateAnimSpriteToTargetMonLocation,
};

const struct SpriteTemplate gScreechRingSpriteTemplate =
{
    .tileTag = ANIM_TAG_PURPLE_RING,
    .paletteTag = ANIM_TAG_PURPLE_RING,
    .oam = &gOamData_AffineDouble_ObjNormal_16x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gGrowingRingAffineAnimTable,
    .callback = TranslateAnimSpriteToTargetMonLocation,
};

const struct SpriteTemplate gMetalSoundSpriteTemplate =
{
    .tileTag = ANIM_TAG_METAL_SOUND_WAVES,
    .paletteTag = ANIM_TAG_METAL_SOUND_WAVES,
    .oam = &gOamData_AffineDouble_ObjNormal_32x64,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gGrowingRingAffineAnimTable,
    .callback = TranslateAnimSpriteToTargetMonLocation,
};

const struct SpriteTemplate gWaterPulseRingSpriteTemplate =
{
    .tileTag = ANIM_TAG_BLUE_RING_2,
    .paletteTag = ANIM_TAG_BLUE_RING_2,
    .oam = &gOamData_AffineDouble_ObjNormal_16x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gWaterPulseRingAffineAnimTable,
    .callback = AnimWaterPulseRing,
};

// Moves a projectile towards the target mon. The sprite is rotated to be pointing
// in the same direction it's moving.
// arg 0: initial x pixel offset
// arg 1: initial y pixel offset
// arg 2: target x pixel offset
// arg 3: target y pixel offset
// arg 4: duration
static void AnimSonicBoomProjectile(struct Sprite *sprite)
{
    s16 targetXPos;
    s16 targetYPos;
    u16 rotation;

    if (IsContest())
    {
        gBattleAnimArgs[2] = -gBattleAnimArgs[2];
    }
    else if (GetBattlerSide(gBattleAnimAttacker) != 0)
    {
        gBattleAnimArgs[2] = -gBattleAnimArgs[2];
        gBattleAnimArgs[1] = -gBattleAnimArgs[1];
        gBattleAnimArgs[3] = -gBattleAnimArgs[3];
    }

    InitSpritePosToAnimAttacker(sprite, 1);
    targetXPos = GetBattlerSpriteCoord(gBattleAnimTarget, 2) + gBattleAnimArgs[2];
    targetYPos = GetBattlerSpriteCoord(gBattleAnimTarget, 3) + gBattleAnimArgs[3];
    rotation = ArcTan2Neg(targetXPos - sprite->x, targetYPos - sprite->y);
    rotation += 0xF000;
    if (IsContest())
        rotation -= 0x6000;

    TrySetSpriteRotScale(sprite, 0, 0x100, 0x100, rotation);
    sprite->data[0] = gBattleAnimArgs[4];
    sprite->data[2] = targetXPos;
    sprite->data[4] = targetYPos;
    sprite->callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}

static void AnimAirWaveProjectile_Step2(struct Sprite *sprite)
{
    if (sprite->data[0]-- <= 0)
    {
        gTasks[sprite->data[7]].data[1]--;
        DestroySprite(sprite);
    }
}

static void AnimAirWaveProjectile_Step1(struct Sprite *sprite)
{
    struct Task *task = &gTasks[sprite->data[7]];
    if (sprite->data[0] > task->data[5])
    {
        sprite->data[5] += sprite->data[3];
        sprite->data[6] += sprite->data[4];
    }
    else
    {
        sprite->data[5] -= sprite->data[3];
        sprite->data[6] -= sprite->data[4];
    }

    sprite->data[1] += sprite->data[5];
    sprite->data[2] += sprite->data[6];
    if (1 & task->data[7])
        sprite->x2 = ((u16)sprite->data[1] >> 8) * -1;
    else
        sprite->x2 = (u16)sprite->data[1] >> 8;

    if (1 & task->data[8])
        sprite->y2 = ((u16)sprite->data[2] / 256u) * -1;
    else
        sprite->y2 = (u16)sprite->data[2] / 256u;

    if (sprite->data[0]-- <= 0)
    {
        sprite->data[0] = 30;
        sprite->callback = AnimAirWaveProjectile_Step2;
    }
}

static void AnimAirWaveProjectile(struct Sprite *sprite)
{
    s16 a;
    s16 b;
    s16 c;

    struct Task *task = &gTasks[sprite->data[7]];
    sprite->data[1] += (-2 & task->data[7]);
    sprite->data[2] += (-2 & task->data[8]);
    if (1 & task->data[7])
        sprite->x2 = ((u16)sprite->data[1] >> 8) * -1;
    else
        sprite->x2 = (u16)sprite->data[1] >> 8;

    if (1 & task->data[8])
        sprite->y2 = ((u16)sprite->data[2] / 256u) * -1;
    else
        sprite->y2 = (u16)sprite->data[2] / 256u;

    if (sprite->data[0]-- <= 0)
    {
        sprite->data[0] = 8;
        task->data[5] = 4;
        a = MathUtil_Inv16(0x1000);
        sprite->x += sprite->x2;
        sprite->y += sprite->y2;
        sprite->y2 = 0;
        sprite->x2 = 0;
        if (task->data[11] >= sprite->x)
            b = (task->data[11] - sprite->x) << 8;
        else
            b = (sprite->x - task->data[11]) << 8;

        if (task->data[12] >= sprite->y)
            c = (task->data[12] - sprite->y) << 8;
        else
            c = (sprite->y - task->data[12]) << 8;

        sprite->data[2] = 0;
        sprite->data[1] = 0;
        sprite->data[6] = 0;
        sprite->data[5] = 0;
        sprite->data[3] = MathUtil_Mul16(MathUtil_Mul16(b, a), MathUtil_Inv16(0x1C0));
        sprite->data[4] = MathUtil_Mul16(MathUtil_Mul16(c, a), MathUtil_Inv16(0x1C0));
        sprite->callback = AnimAirWaveProjectile_Step1;
    } 
}

static void AirCutterProjectileStep2(u8 taskId)
{
    if (gTasks[taskId].data[1] == 0)
        DestroyAnimVisualTask(taskId);
}

static void AirCutterProjectileStep1(u8 taskId)
{
    if (gTasks[taskId].data[0]-- <= 0)
    {
        u8 spriteId;
        struct Sprite *sprite;
        spriteId = CreateSprite(&gAirWaveProjectileSpriteTemplate, gTasks[taskId].data[9], gTasks[taskId].data[10], gTasks[taskId].data[2] - gTasks[taskId].data[1]);
        sprite = &gSprites[spriteId];
        switch (gTasks[taskId].data[4])
        {
        case 1:
            sprite->oam.matrixNum |= 24;
            break;
        case 2:
            sprite->oam.matrixNum = 8;
            break;
        }

        sprite->data[0] = gTasks[taskId].data[5] - gTasks[taskId].data[6];
        sprite->data[7] = taskId;
        gTasks[taskId].data[gTasks[taskId].data[1] + 13] = spriteId;
        gTasks[taskId].data[0] = gTasks[taskId].data[3];
        gTasks[taskId].data[1]++;
        PlaySE12WithPanning(0x9A, BattleAnimAdjustPanning(-0x3F));
        if (gTasks[taskId].data[1] > 2)
            gTasks[taskId].func = AirCutterProjectileStep2;
    }
}

void AnimTask_AirCutterProjectile(u8 taskId)
{
    s16 attackerY = 0;
    s16 attackerX = 0;
    s16 targetX = 0;
    s16 targetY = 0;
    s16 r4;

    if (IsContest())
    {
        gTasks[taskId].data[4] = 2;
        gBattleAnimArgs[0] = -gBattleAnimArgs[0];
        if (gBattleAnimArgs[2] & 1)
            gBattleAnimArgs[2] &= ~1;
        else
            gBattleAnimArgs[2] |= 1;
    }
    else
    {
        if ((gBattlerPositions[gBattleAnimTarget] & 1) == 0)
        {
            gTasks[taskId].data[4] = 1;
            gBattleAnimArgs[0] = -gBattleAnimArgs[0];
            gBattleAnimArgs[1] = -gBattleAnimArgs[1];
            if (gBattleAnimArgs[2] & 1)
                gBattleAnimArgs[2] &= ~1;
            else
                gBattleAnimArgs[2] |= 1;
        }
    }
    attackerX = gTasks[taskId].data[9] = GetBattlerSpriteCoord(gBattleAnimAttacker, 0);
    attackerY = gTasks[taskId].data[10] = GetBattlerSpriteCoord(gBattleAnimAttacker, 1);
    if ((gBattleTypeFlags & BATTLE_TYPE_DOUBLE)
        && IsBattlerSpriteVisible(gBattleAnimTarget ^ 2))
    {
        SetAverageBattlerPositions(gBattleAnimTarget, 0, &targetX, &targetY);
    }
    else
    {
        targetX = GetBattlerSpriteCoord(gBattleAnimTarget, 0);
        targetY = GetBattlerSpriteCoord(gBattleAnimTarget, 1);
    }

    targetX = gTasks[taskId].data[11] = targetX + gBattleAnimArgs[0];
    targetY = gTasks[taskId].data[12] = targetY + gBattleAnimArgs[1];
    if (targetX >= attackerX)
        r4 = targetX - attackerX;
    else
        r4 = attackerX - targetX;

    gTasks[taskId].data[5] = MathUtil_Mul16(r4, MathUtil_Inv16(gBattleAnimArgs[2] & ~1));
    gTasks[taskId].data[6] = MathUtil_Mul16(gTasks[taskId].data[5], 0x80);
    gTasks[taskId].data[7] = gBattleAnimArgs[2];
    if (targetY >= attackerY)
    {
        r4 = targetY - attackerY;
        gTasks[taskId].data[8] = MathUtil_Mul16(r4, MathUtil_Inv16(gTasks[taskId].data[5])) & ~1;
    }
    else
    {
        r4 = attackerY - targetY;
        gTasks[taskId].data[8] = MathUtil_Mul16(r4, MathUtil_Inv16(gTasks[taskId].data[5])) | 1;
    }

    gTasks[taskId].data[3] = gBattleAnimArgs[3];
    if (gBattleAnimArgs[4] & 0x80)
    {
        gBattleAnimArgs[4] ^= 0x80;
        if (gBattleAnimArgs[4] >= 64)
        {
            u16 var = GetBattlerSpriteSubpriority(gBattleAnimTarget) + (gBattleAnimArgs[4] - 64);
            gTasks[taskId].data[2] = var;
        }
        else
        {
            u16 var = GetBattlerSpriteSubpriority(gBattleAnimTarget) - gBattleAnimArgs[4];
            gTasks[taskId].data[2] = var;
        }
    }
    else
    {
        if (gBattleAnimArgs[4] >= 64)
        {
            u16 var = GetBattlerSpriteSubpriority(gBattleAnimTarget) + (gBattleAnimArgs[4] - 64);
            gTasks[taskId].data[2] = var;
        }
        else
        {
            u16 var = GetBattlerSpriteSubpriority(gBattleAnimTarget) - gBattleAnimArgs[4];
            gTasks[taskId].data[2] = var;
        }
    }

    if (gTasks[taskId].data[2] < 3)
        gTasks[taskId].data[2] = 3;

    gTasks[taskId].func = AirCutterProjectileStep1;
}
