#include "global.h"
#include "battle.h"
#include "battle_anim_internal.h"
#include "scanline_effect.h"
#include "random.h"
#include "battle_anim.h"
#include "task.h"
#include "trig.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;
extern u16 gBattle_BG1_X;
extern u16 gBattle_BG1_Y;
extern u16 gBattle_BG2_X;
extern u16 gBattle_BG2_Y;
extern u16 gBattle_BG3_X;
extern u16 gBattle_BG3_Y;
extern u16 gAnimMovePower;
extern u8 gBattlerSpriteIds[];

static void AnimBonemerangProjectile(struct Sprite *sprite);
static void AnimBonemerangProjectile_Step(struct Sprite *sprite);
static void AnimBonemerangProjectile_End(struct Sprite *sprite);
static void AnimBoneHitProjectile(struct Sprite *sprite);
static void AnimDirtScatter(struct Sprite *sprite);
static void AnimMudSportDirt(struct Sprite *sprite);
static void AnimMudSportDirtRising(struct Sprite *sprite);
static void AnimMudSportDirtFalling(struct Sprite *sprite);
static void AnimTask_DigBounceMovement(u8 taskId);
static void SetDigScanlineEffect(u8, s16, s16);
static void AnimTask_DigEndBounceMovementSetInvisible(u8 taskId);
static void AnimTask_DigSetVisibleUnderground(u8 taskId);
static void AnimTask_DigRiseUpFromHole(u8 taskId);
static void AnimDirtPlumeParticle(struct Sprite *sprite);
static void AnimDirtPlumeParticle_Step(struct Sprite *sprite);
static void AnimDigDirtMound(struct Sprite *sprite);
static void AnimTask_ShakePlatforms(u8 taskId);
static void AnimTask_ShakeBattlers(u8 taskId);
static void SetBattlersXOffsetForShake(struct Task *task);
static void WaitForFissureCompletion(u8 taskId);

static const union AffineAnimCmd sAffineAnim_Bonemerang[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, 15, 1),
    AFFINEANIMCMD_JUMP(0),
};

static const union AffineAnimCmd sAffineAnim_SpinningBone[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, 20, 1),
    AFFINEANIMCMD_JUMP(0),
};

static const union AffineAnimCmd *const sAffineAnims_Bonemerang[] =
{
    sAffineAnim_Bonemerang,
};

static const union AffineAnimCmd *const sAffineAnims_SpinningBone[] =
{
    sAffineAnim_SpinningBone,
};

const struct SpriteTemplate gBonemerangSpriteTemplate =
{
    .tileTag = ANIM_TAG_BONE,
    .paletteTag = ANIM_TAG_BONE,
    .oam = &gOamData_AffineNormal_ObjNormal_32x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = sAffineAnims_Bonemerang,
    .callback = AnimBonemerangProjectile,
};

const struct SpriteTemplate gSpinningBoneSpriteTemplate =
{
    .tileTag = ANIM_TAG_BONE,
    .paletteTag = ANIM_TAG_BONE,
    .oam = &gOamData_AffineNormal_ObjNormal_32x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = sAffineAnims_SpinningBone,
    .callback = AnimBoneHitProjectile,
};

const struct SpriteTemplate gSandAttackDirtSpriteTemplate =
{
    .tileTag = ANIM_TAG_MUD_SAND,
    .paletteTag = ANIM_TAG_MUD_SAND,
    .oam = &gOamData_AffineOff_ObjNormal_8x8,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimDirtScatter,
};

static const union AnimCmd sAnim_MudSlapMud[] =
{
    ANIMCMD_FRAME(1, 1),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_MudSlapMud[] =
{
    sAnim_MudSlapMud,
};

const struct SpriteTemplate gMudSlapMudSpriteTemplate =
{
    .tileTag = ANIM_TAG_MUD_SAND,
    .paletteTag = ANIM_TAG_MUD_SAND,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = sAnims_MudSlapMud,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimDirtScatter,
};

const struct SpriteTemplate gMudsportMudSpriteTemplate =
{
    .tileTag = ANIM_TAG_MUD_SAND,
    .paletteTag = ANIM_TAG_MUD_SAND,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimMudSportDirt,
};

const struct SpriteTemplate gDirtPlumeSpriteTemplate =
{
    .tileTag = ANIM_TAG_MUD_SAND,
    .paletteTag = ANIM_TAG_MUD_SAND,
    .oam = &gOamData_AffineOff_ObjNormal_8x8,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimDirtPlumeParticle,
};

const struct SpriteTemplate gDirtMoundSpriteTemplate =
{
    .tileTag = ANIM_TAG_DIRT_MOUND,
    .paletteTag = ANIM_TAG_DIRT_MOUND,
    .oam = &gOamData_AffineOff_ObjNormal_32x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimDigDirtMound,
};

// Moves a bone projectile towards the target mon, which moves like
// a boomerang. After hitting the target mon, it comes back to the user.
static void AnimBonemerangProjectile(struct Sprite *sprite)
{
    sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, 2);
    sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, 3);
    sprite->data[0] = 20;
    sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimTarget, 2);
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimTarget, 3);
    sprite->data[5] = -40;
    InitAnimArcTranslation(sprite);
    sprite->callback = AnimBonemerangProjectile_Step;
}

static void AnimBonemerangProjectile_Step(struct Sprite *sprite)
{
    if (TranslateAnimHorizontalArc(sprite))
    {
        sprite->x += sprite->x2;
        sprite->y += sprite->y2;
        sprite->y2 = 0;
        sprite->x2 = 0;
        sprite->data[0] = 20;
        sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimAttacker, 2);
        sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimAttacker, 3);
        sprite->data[5] = 40;
        InitAnimArcTranslation(sprite);
        sprite->callback = AnimBonemerangProjectile_End;
    }
}

static void AnimBonemerangProjectile_End(struct Sprite *sprite)
{
    if (TranslateAnimHorizontalArc(sprite))
        DestroyAnimSprite(sprite);
}

// Moves a bone projectile towards the target mon, starting right next to
// the target mon.
// arg 0: initial x pixel offset
// arg 1: initial y pixel offset
// arg 2: target x pixel offset
// arg 3: target y pixel offset
// arg 4: duration
static void AnimBoneHitProjectile(struct Sprite *sprite)
{
    CMD_ARGS(initialX, initialY, targetX, targetY, duration);

    InitSpritePosToAnimTarget(sprite, TRUE);
    if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
        cmd->targetX = -cmd->targetX;
    
    sprite->data[0] = cmd->duration;
    sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimTarget, 2) + cmd->targetX;
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimTarget, 3) + cmd->targetY;
    sprite->callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}

// Moves a small dirt projectile towards the target mon.
// arg 0: initial x pixel offset
// arg 1: initial y pixel offset
// arg 2: duration
// arg 3: target x pixel offset
// arg 4: target y pixel offset
static void AnimDirtScatter(struct Sprite *sprite)
{
    CMD_ARGS(initialX, initialY, duration, targetX, targetY);
    u8 targetXPos, targetYPos;
    s16 xOffset, yOffset;

    InitSpritePosToAnimAttacker(sprite, TRUE);

    targetXPos = GetBattlerSpriteCoord2(gBattleAnimTarget, BATTLER_COORD_X_2);
    targetYPos = GetBattlerSpriteCoord2(gBattleAnimTarget, BATTLER_COORD_Y_PIC_OFFSET);

    xOffset = Random() & 0x1F;
    yOffset = Random() & 0x1F;
    if (xOffset > 16)
        xOffset = 16 - xOffset;
    if (yOffset > 16)
        yOffset = 16 - yOffset;

    sprite->data[0] = cmd->duration;
    sprite->data[2] = targetXPos + xOffset;
    sprite->data[4] = targetYPos + yOffset;
    sprite->callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(sprite, DestroySpriteAndMatrix);
}

// Moves a particle of dirt in the Mud Sport animation.
// The dirt can either be rising upward, or falling down.
// arg 0: 0 = dirt is rising into the air, 1 = dirt is falling down
// arg 1: initial x pixel offset
// arg 2: initial y pixel offset
static void AnimMudSportDirt(struct Sprite *sprite)
{
    CMD_ARGS(type, x, y);

    sprite->oam.tileNum++;
    if (cmd->type == FALSE)
    {
        sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, 2) + cmd->x;
        sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, 3) + cmd->y;
        sprite->data[0] = cmd->x > 0 ? TRUE : -1;
        sprite->callback = AnimMudSportDirtRising;
    }
    else
    {
        sprite->x = cmd->x;
        sprite->y = cmd->y;
        sprite->y2 = -cmd->y;
        sprite->callback = AnimMudSportDirtFalling;
    }
}

static void AnimMudSportDirtRising(struct Sprite *sprite)
{
    if (++sprite->data[1] > 1)
    {
        sprite->data[1] = 0;
        sprite->x += sprite->data[0];
    }

    sprite->y -= 4;
    if (sprite->y < -4)
        DestroyAnimSprite(sprite);
}

static void AnimMudSportDirtFalling(struct Sprite *sprite)
{
    switch (sprite->data[0])
    {
    case 0:
        sprite->y2 += 4;
        if (sprite->y2 >= 0)
        {
            sprite->y2 = 0;
            sprite->data[0]++;
        }
        break;
    case 1:
        if (++sprite->data[1] > 0)
        {
            sprite->data[1] = 0;
            sprite->invisible ^= 1;
            if (++sprite->data[2] == 10)
                DestroyAnimSprite(sprite);
        }
        break;
    }
}

void AnimTask_DigDownMovement(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    CMD_ARGS(mode);

    if (cmd->mode == FALSE)
        task->func = AnimTask_DigBounceMovement;
    else
        task->func = AnimTask_DigEndBounceMovementSetInvisible;

    task->func(taskId);
}

static void AnimTask_DigBounceMovement(u8 taskId)
{
    u8 battlerY;
    struct Task *task = &gTasks[taskId];

    switch (task->data[0])
    {
    case 0:
        task->data[10] = GetAnimBattlerSpriteId(ANIM_BATTLER_ATTACKER);
        task->data[11] = GetBattlerSpriteBGPriorityRank(gBattleAnimAttacker);
        if (task->data[11] == 1)
        {
            task->data[12] = gBattle_BG1_X;
            task->data[13] = gBattle_BG1_Y;
        }
        else
        {
            task->data[12] = gBattle_BG2_X;
            task->data[13] = gBattle_BG2_Y;
        }

        battlerY = GetBattlerYCoordWithElevation(gBattleAnimAttacker);
        task->data[14] = battlerY - 32;
        task->data[15] = battlerY + 32;
        if (task->data[14] < 0)
            task->data[14] = 0;

        gSprites[task->data[10]].invisible = TRUE;
        task->data[0]++;
        break;
    case 1:
        SetDigScanlineEffect(task->data[11], task->data[14], task->data[15]);
        task->data[0]++;
        break;
    case 2:
        task->data[2] = (task->data[2] + 6) & 0x7F;
        if (++task->data[4] > 2)
        {
            task->data[4] = 0;
            task->data[3]++;
        }

        task->data[5] = task->data[3] + (gSineTable[task->data[2]] >> 4);
        if (task->data[11] == 1)
            gBattle_BG1_Y = task->data[13] - task->data[5];
        else
            gBattle_BG2_Y = task->data[13] - task->data[5];

        if (task->data[5] > 63)
        {
            task->data[5] = 120 - task->data[14];
            if (task->data[11] == 1)
                gBattle_BG1_Y = task->data[13] - task->data[5];
            else
                gBattle_BG2_Y = task->data[13] - task->data[5];

            gSprites[task->data[10]].x2 = DISPLAY_WIDTH + 32 - gSprites[task->data[10]].x;
            task->data[0]++;
        }
        break;
    case 3:
        gScanlineEffect.state = 3;
        task->data[0]++;
        break;
    case 4:
        DestroyAnimVisualTask(taskId);
        gSprites[task->data[10]].invisible = TRUE;
        break;
    }
}

static void AnimTask_DigEndBounceMovementSetInvisible(u8 taskId)
{
    u8 spriteId = GetAnimBattlerSpriteId(ANIM_BATTLER_ATTACKER);
    gSprites[spriteId].invisible = TRUE;
    gSprites[spriteId].x2 = 0;
    gSprites[spriteId].y2 = 0;

    if (GetBattlerSpriteBGPriorityRank(gBattleAnimAttacker) == 1)
        gBattle_BG1_Y = 0;
    else
        gBattle_BG2_Y = 0;

    DestroyAnimVisualTask(taskId);
}

void AnimTask_DigUpMovement(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    CMD_ARGS(mode);

    if (cmd->mode == FALSE)
        task->func = AnimTask_DigSetVisibleUnderground;
    else
        task->func = AnimTask_DigRiseUpFromHole;

    task->func(taskId);
}

static void AnimTask_DigSetVisibleUnderground(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->data[0])
    {
    case 0:
        task->data[10] = GetAnimBattlerSpriteId(ANIM_BATTLER_ATTACKER);
        gSprites[task->data[10]].invisible = FALSE;
        gSprites[task->data[10]].x2 = 0;
        gSprites[task->data[10]].y2 = DISPLAY_HEIGHT - gSprites[task->data[10]].y;
        task->data[0]++;
        break;
    case 1:
        DestroyAnimVisualTask(taskId);
    }
}

static void AnimTask_DigRiseUpFromHole(u8 taskId)
{
    u8 battlerY;
    struct Task *task = &gTasks[taskId];

    switch (task->data[0])
    {
    case 0:
        task->data[10] = GetAnimBattlerSpriteId(ANIM_BATTLER_ATTACKER);
        task->data[11] = GetBattlerSpriteBGPriorityRank(gBattleAnimAttacker);
        if (task->data[11] == 1)
            task->data[12] = gBattle_BG1_X;
        else
            task->data[12] = gBattle_BG2_X;

        battlerY = GetBattlerYCoordWithElevation(gBattleAnimAttacker);
        task->data[14] = battlerY - 32;
        task->data[15] = battlerY + 32;
        task->data[0]++;
        break;
    case 1:
        SetDigScanlineEffect(task->data[11], 0, task->data[15]);
        task->data[0]++;
        break;
    case 2:
        gSprites[task->data[10]].y2 = 96;
        task->data[0]++;
        break;
    case 3:
        gSprites[task->data[10]].y2 -= 8;
        if (gSprites[task->data[10]].y2 == 0)
        {
            gScanlineEffect.state = 3;
            task->data[0]++;
        }
        break;
    case 4:
        DestroyAnimVisualTask(taskId);
        break;
    }
}

static void SetDigScanlineEffect(u8 useBG1, s16 y, s16 endY)
{
    s16 bgX;
    struct ScanlineEffectParams scanlineParams;

    if (useBG1 == TRUE)
    {
        bgX = gBattle_BG1_X;
        scanlineParams.dmaDest = &REG_BG1HOFS;
    }
    else
    {
        bgX = gBattle_BG2_X;
        scanlineParams.dmaDest = &REG_BG2HOFS;
    }

    if (y < 0)
        y = 0;

    while (y < endY)
    {
        gScanlineEffectRegBuffers[0][y] = bgX;
        gScanlineEffectRegBuffers[1][y] = bgX;
        y++;
    }

    while (y < DISPLAY_HEIGHT)
    {
        gScanlineEffectRegBuffers[0][y] = bgX + DISPLAY_WIDTH;
        gScanlineEffectRegBuffers[1][y] = bgX + DISPLAY_WIDTH;
        y++;
    }

    scanlineParams.dmaControl = SCANLINE_EFFECT_DMACNT_16BIT;
    scanlineParams.initState = 1;
    scanlineParams.unused9 = 0;
    ScanlineEffect_SetParams(scanlineParams);
}

// Moves a particle of dirt in a plume of dirt. Used in Fissure and Dig.
// arg 0: which mon (0 = attacker, 1 = target)
// arg 1: which side of mon (0 = left, 1 = right)
// arg 2: target x offset
// arg 3: target y offset
// arg 4: wave amplitude
// arg 5: duration
static void AnimDirtPlumeParticle(struct Sprite *sprite)
{
    CMD_ARGS(battlerSelector, side, x, y, amplitude, duration);
    s8 battler;
    s16 xOffset;

    if (cmd->battlerSelector == ANIM_BATTLER_ATTACKER)
        battler = gBattleAnimAttacker;
    else
        battler = gBattleAnimTarget;

    xOffset = 24;
    if (cmd->side == TRUE)
    {
        xOffset *= -1;
        cmd->x *= -1;
    }

    sprite->x = GetBattlerSpriteCoord(battler, 2) + xOffset;
    sprite->y = GetBattlerYCoordWithElevation(battler) + 30;
    sprite->data[0] = cmd->duration;
    sprite->data[2] = sprite->x + cmd->x;
    sprite->data[4] = sprite->y + cmd->y;
    sprite->data[5] = cmd->amplitude;
    InitAnimArcTranslation(sprite);
    sprite->callback = AnimDirtPlumeParticle_Step;
}

static void AnimDirtPlumeParticle_Step(struct Sprite *sprite)
{
    if (TranslateAnimHorizontalArc(sprite))
        DestroyAnimSprite(sprite);
}

// Displays the dirt mound seen in the move Dig for set duration.
// The dirt mound image is too large for a single sprite, so two
// sprites are lined up next to each other.
// arg 0: which mon (0 = attacker, 1 = target)
// arg 1: oam tile num (0 = left half of image, 1 = right half of image)
// arg 2: duration
static void AnimDigDirtMound(struct Sprite *sprite)
{
    CMD_ARGS(battlerSelector, tileOffset, duration);
    s8 battler;

    if (cmd->battlerSelector == ANIM_BATTLER_ATTACKER)
        battler = gBattleAnimAttacker;
    else
        battler = gBattleAnimTarget;

    sprite->x = GetBattlerSpriteCoord(battler, 0) - 16 + (cmd->tileOffset * 32);
    sprite->y = GetBattlerYCoordWithElevation(battler) + 32;
    sprite->oam.tileNum += cmd->tileOffset * 8;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
    sprite->data[0] = cmd->duration;
    sprite->callback = WaitAnimForDuration;
}

#define tState               data[0]
#define tDelay               data[1]
#define tTimer               data[2]
#define tMaxTime             data[3]
#define tbattlerSpriteIds(i) data[9 + (i)]
#define tNumBattlers         data[13]
#define tInitialX            data[13]
#define tHorizOffset         data[14]
#define tInitHorizOffset     data[15]

// Shakes battler(s) or the battle platforms back and forth horizontally.
// arg0: battler (0-3), all battlers (MAX_BATTLERS_COUNT), or platforms
//       (MAX_BATTLERS_COUNT + 1)
// arg1: shake intensity (0 uses move power)
// arg2: shake duration
void AnimTask_HorizontalShake(u8 taskId)
{
    CMD_ARGS(battler, intensity, duration);
    u16 i;
    struct Task *task = &gTasks[taskId];

    if (cmd->intensity)
        task->tHorizOffset = task->tInitHorizOffset = cmd->intensity + 3;
    else
        task->tHorizOffset = task->tInitHorizOffset = (gAnimMovePower / 10) + 3;

    task->tMaxTime = cmd->duration;
    switch (cmd->battler)
    {
    case MAX_BATTLERS_COUNT + 1:
        task->tInitialX = gBattle_BG3_X;
        task->func = AnimTask_ShakePlatforms;
        break;
    case MAX_BATTLERS_COUNT:
        task->tNumBattlers = 0;
        for (i = 0; i < MAX_BATTLERS_COUNT; i++)
        {
            if (IsBattlerSpriteVisible(i))
            {
                task->tbattlerSpriteIds(task->tNumBattlers) = gBattlerSpriteIds[i];
                task->tNumBattlers++;
            }
        }
        task->func = AnimTask_ShakeBattlers;
        break;
    default:
        task->tbattlerSpriteIds(0) = GetAnimBattlerSpriteId(cmd->battler);
        if (task->tbattlerSpriteIds(0) == 0xFF)
        {
            DestroyAnimVisualTask(taskId);
        }
        else
        {
            task->tNumBattlers = 1;
            task->func = AnimTask_ShakeBattlers;
        }

        break;
    }
}

static void AnimTask_ShakePlatforms(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->tState)
    {
    case 0:
        if (++task->tDelay > 1)
        {
            task->tDelay = 0;
            if ((task->tTimer & 1) == 0)
                gBattle_BG3_X = task->tInitialX + task->tInitHorizOffset;
            else
                gBattle_BG3_X = task->tInitialX - task->tInitHorizOffset;

            if (++task->tTimer == task->tMaxTime)
            {
                task->tTimer = 0;
                task->tHorizOffset--;
                task->tState++;
            }
        }
        break;
    case 1:
        if (++task->tDelay > 1)
        {
            task->tDelay = 0;
            if ((task->tTimer & 1) == 0)
                gBattle_BG3_X = task->tInitialX + task->tHorizOffset;
            else
                gBattle_BG3_X = task->tInitialX - task->tHorizOffset;

            if (++task->tTimer == 4)
            {
                task->tTimer = 0;
                if (--task->tHorizOffset == 0)
                    task->tState++;
            }
        }
        break;
    case 2:
        gBattle_BG3_X = task->tInitialX;
        DestroyAnimVisualTask(taskId);
        break;
    }
}

static void AnimTask_ShakeBattlers(u8 taskId)
{
    u16 i;
    struct Task *task = &gTasks[taskId];

    switch (task->tState)
    {
    case 0:
        if (++task->tDelay > 1)
        {
            task->tDelay = 0;
            SetBattlersXOffsetForShake(task);
            if (++task->tTimer == task->tMaxTime)
            {
                task->tTimer = 0;
                task->tHorizOffset--;
                task->tState++;
            }
        }
        break;
    case 1:
        if (++task->tDelay > 1)
        {
            task->tDelay = 0;
            SetBattlersXOffsetForShake(task);
            if (++task->tTimer == 4)
            {
                task->tTimer = 0;
                if (--task->tHorizOffset == 0)
                    task->tState++;
            }
        }
        break;
    case 2:
        for (i = 0; i < task->tNumBattlers; i++)
            gSprites[task->tbattlerSpriteIds(i)].x2 = 0;

        DestroyAnimVisualTask(taskId);
        break;
    }
}

static void SetBattlersXOffsetForShake(struct Task *task)
{
    u16 i;
    u16 xOffset;

    if ((task->tTimer & 1) == 0)
        xOffset = (task->tHorizOffset / 2) + (task->tHorizOffset & 1);
    else
        xOffset = -(task->tHorizOffset / 2);

    for (i = 0; i < task->tNumBattlers; i++)
    {
        gSprites[task->tbattlerSpriteIds(i)].x2 = xOffset;
    }
}

void AnimTask_IsPowerOver99(u8 taskId)
{
    gBattleAnimArgs[ARG_MAGNITUDE_POWER_RESULT] = gAnimMovePower > 99;
    DestroyAnimVisualTask(taskId);
}

#undef tState
#undef tDelay
#undef tTimer
#undef tMaxTime
#undef tbattlerSpriteIds
#undef tNumBattlers
#undef tInitialX
#undef tHorizOffset
#undef tInitHorizOffset

void AnimTask_PositionFissureBgOnBattler(u8 taskId)
{
    CMD_ARGS(battlerSelector, taskPriority, completionValue);
    struct Task *task;
    u8 battler = (cmd->battlerSelector & ANIM_BATTLER_TARGET) ? gBattleAnimTarget : gBattleAnimAttacker;

    if (cmd->battlerSelector > ANIM_BATTLER_TARGET)
        battler = BATTLE_PARTNER(battler);

    task = &gTasks[CreateTask(WaitForFissureCompletion, cmd->taskPriority)];
    task->data[1] = (32 - GetBattlerSpriteCoord(battler, 2)) & 0x1FF;
    task->data[2] = (64 - GetBattlerSpriteCoord(battler, 3)) & 0xFF;
    gBattle_BG3_X = task->data[1];
    gBattle_BG3_Y = task->data[2];
    task->data[3] = cmd->completionValue;
    DestroyAnimVisualTask(taskId);
}

static void WaitForFissureCompletion(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    if (gBattleAnimArgs[ARG_RET_ID] == task->data[3])
    {
        gBattle_BG3_X = 0;
        gBattle_BG3_Y = 0;
        DestroyTask(taskId);
    }
    else
    {
        gBattle_BG3_X = task->data[1];
        gBattle_BG3_Y = task->data[2];
    }
}
