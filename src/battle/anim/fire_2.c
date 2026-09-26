#include "global.h"
#include "battle.h"
#include "battle_anim.h"
#include "heated_rock.h"
#include "rom_8077ABC.h"
#include "task.h"
#include "trig.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

void AnimEmberFlare(struct Sprite *sprite);
static void AnimBurnFlame(struct Sprite *sprite);
void AnimFireRing(struct Sprite *sprite);
void AnimFireCross(struct Sprite *sprite);
void AnimFireSpiralOutward(struct Sprite *sprite);
static void AnimEruptionLaunchRock(struct Sprite *sprite);
static void AnimEruptionFallingRock(struct Sprite *sprite);
static void AnimFireRingStep1(struct Sprite *);
static void UpdateFireRingCircleOffset(struct Sprite *);
static void AnimFireRingStep2(struct Sprite *);
static void AnimFireRingStep3(struct Sprite *);
static void AnimFireSpiralOutward_Step1(struct Sprite *);
static void AnimFireSpiralOutward_Step2(struct Sprite *);
static void AnimTask_EruptionLaunchRocks_Step(u8 taskId);
static void CreateEruptionLaunchRocks(u8 spriteId, u8 taskId, u8 activeSpritesIdx);
static void UpdateEruptionLaunchRockPos(struct Sprite *sprite);
static void AnimEruptionFallingRock_Step(struct Sprite *sprite);

static const union AnimCmd sAnim_BasicFire[] =
{
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(16, 4),
    ANIMCMD_FRAME(32, 4),
    ANIMCMD_FRAME(48, 4),
    ANIMCMD_FRAME(64, 4),
    ANIMCMD_JUMP(0),
};

const union AnimCmd *const gAnims_BasicFire[] =
{
    sAnim_BasicFire,
};

const struct SpriteTemplate gEmberSpriteTemplate =
{
    .tileTag = ANIM_TAG_SMALL_EMBER,
    .paletteTag = ANIM_TAG_SMALL_EMBER,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = TranslateAnimSpriteToTargetMonLocation,
};

const struct SpriteTemplate gEmberFlareSpriteTemplate =
{
    .tileTag = ANIM_TAG_SMALL_EMBER,
    .paletteTag = ANIM_TAG_SMALL_EMBER,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gAnims_BasicFire,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimEmberFlare,
};

const struct SpriteTemplate gBurnFlameSpriteTemplate =
{
    .tileTag = ANIM_TAG_SMALL_EMBER,
    .paletteTag = ANIM_TAG_SMALL_EMBER,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gAnims_BasicFire,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimBurnFlame,
};

const struct SpriteTemplate gFireBlastRingSpriteTemplate =
{
    .tileTag = ANIM_TAG_SMALL_EMBER,
    .paletteTag = ANIM_TAG_SMALL_EMBER,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gAnims_BasicFire,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimFireRing,
};

const union AnimCmd gSpriteAnim_83D9644[] =
{
    ANIMCMD_FRAME(32, 6),
    ANIMCMD_FRAME(48, 6),
    ANIMCMD_JUMP(0),
};

const union AnimCmd *const gSpriteAnimTable_83D9650[] =
{
    gSpriteAnim_83D9644,
};

const union AffineAnimCmd gSpriteAffineAnim_83D9654[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, 0, 1),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd gSpriteAffineAnim_83D9664[] =
{
    AFFINEANIMCMD_FRAME(0xA0, 0xA0, 0, 0),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd *const gSpriteAffineAnimTable_83D9674[] =
{
    gSpriteAffineAnim_83D9654,
    gSpriteAffineAnim_83D9664,
};

const struct SpriteTemplate gFireBlastCrossSpriteTemplate =
{
    .tileTag = ANIM_TAG_SMALL_EMBER,
    .paletteTag = ANIM_TAG_SMALL_EMBER,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gSpriteAnimTable_83D9650,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimFireCross,
};

const struct SpriteTemplate gFireSpiralOutwardSpriteTemplate =
{
    .tileTag = ANIM_TAG_SMALL_EMBER,
    .paletteTag = ANIM_TAG_SMALL_EMBER,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gAnims_BasicFire,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimFireSpiralOutward,
};

const struct SpriteTemplate gWeatherBallFireDownSpriteTemplate =
{
    .tileTag = ANIM_TAG_SMALL_EMBER,
    .paletteTag = ANIM_TAG_SMALL_EMBER,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gAnims_BasicFire,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimWeatherBallDown,
};


const struct SpriteTemplate gEruptionLaunchRockSpriteTemplate =
{
    .tileTag = ANIM_TAG_WARM_ROCK,
    .paletteTag = ANIM_TAG_WARM_ROCK,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimEruptionLaunchRock,
};

const s16 sEruptionLaunchRockSpeeds[][2] =
{
    {-2, -5},
    {-1, -1},
    { 3, -6},
    { 4, -2},
    { 2, -8},
    {-5, -5},
    { 4, -7},
};

const struct SpriteTemplate gEruptionFallingRockSpriteTemplate =
{
    .tileTag = ANIM_TAG_WARM_ROCK,
    .paletteTag = ANIM_TAG_WARM_ROCK,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimEruptionFallingRock,
};

// Animates the secondary effect of MOVE_EMBER, where the flames grow and slide
// horizontally a bit.
// arg 0: initial x pixel offset
// arg 1: initial y pixel offset
// arg 2: target x pixel offset
// arg 3: target y pixel offset
// arg 4: duration
// arg 5: ? (todo: something related to which mon the pixel offsets are based on)
// arg 6: ? (todo: something related to which mon the pixel offsets are based on)
void AnimEmberFlare(struct Sprite *sprite)
{
    if (GetBattlerSide(gBattleAnimAttacker) == GetBattlerSide(gBattleAnimTarget)
        && (gBattleAnimAttacker == GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT)
            || gBattleAnimAttacker == GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT)))
            gBattleAnimArgs[2] = -gBattleAnimArgs[2];

    sprite->callback = AnimTravelDiagonally;
    sprite->callback(sprite);
}

static void AnimBurnFlame(struct Sprite *sprite)
{
    gBattleAnimArgs[0] = -gBattleAnimArgs[0];
    gBattleAnimArgs[2] = -gBattleAnimArgs[2];

    sprite->callback = AnimTravelDiagonally;
}

// Animates the a fire sprite in the first-half of the MOVE_FIRE_BLAST
// animation.  The fire sprite first moves in a circle around the mon,
// and then it is translated towards the target mon, while still rotating.
// Lastly, it moves in a circle around the target mon.
// arg 0: initial x pixel offset
// arg 1: initial y pixel offset
// arg 2: initial wave offset
void AnimFireRing(struct Sprite *sprite)
{
    InitSpritePosToAnimAttacker(sprite, 1);

    sprite->data[7] = gBattleAnimArgs[2];
    sprite->data[0] = 0;

    sprite->callback = AnimFireRingStep1;
}

static void AnimFireRingStep1(struct Sprite *sprite)
{
    UpdateFireRingCircleOffset(sprite);

    if (++sprite->data[0] == 0x12)
    {
        sprite->data[0] = 0x19;
        sprite->data[1] = sprite->x;
        sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimTarget, 2);
        sprite->data[3] = sprite->y;
        sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimTarget, 3);

        InitAnimLinearTranslation(sprite);

        sprite->callback = AnimFireRingStep2;
    }
}

static void AnimFireRingStep2(struct Sprite *sprite)
{
    if (AnimTranslateLinear(sprite))
    {
        sprite->data[0] = 0;

        sprite->x = GetBattlerSpriteCoord(gBattleAnimTarget, 2);
        sprite->y = GetBattlerSpriteCoord(gBattleAnimTarget, 3);
        sprite->y2 = 0;
        sprite->x2 = 0;

        sprite->callback = AnimFireRingStep3;
        sprite->callback(sprite);
    }
    else
    {
        sprite->x2 += Sin(sprite->data[7], 28);
        sprite->y2 += Cos(sprite->data[7], 28);

        sprite->data[7] = (sprite->data[7] + 20) & 0xFF;
    }
}

static void AnimFireRingStep3(struct Sprite *sprite)
{
    UpdateFireRingCircleOffset(sprite);

    if (++sprite->data[0] == 0x1F)
        DestroyAnimSprite(sprite);
}

static void UpdateFireRingCircleOffset(struct Sprite *sprite)
{
    sprite->x2 = Sin(sprite->data[7], 28);
    sprite->y2 = Cos(sprite->data[7], 28);

    sprite->data[7] = (sprite->data[7] + 20) & 0xFF;
}

// arg 0: initial x pixel offset
// arg 1: initial y pixel offset
// arg 2: duration
// arg 3: x delta
// arg 4: y delta 
void AnimFireCross(struct Sprite *sprite)
{
    sprite->x += gBattleAnimArgs[0];
    sprite->y += gBattleAnimArgs[1];

    sprite->data[0] = gBattleAnimArgs[2];
    sprite->data[1] = gBattleAnimArgs[3];
    sprite->data[2] = gBattleAnimArgs[4];

    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);

    sprite->callback = TranslateSpriteLinear;
}

void AnimFireSpiralOutward(struct Sprite *sprite)
{
    InitSpritePosToAnimAttacker(sprite, 1);

    sprite->data[1] = gBattleAnimArgs[2];
    sprite->data[0] = gBattleAnimArgs[3];

    sprite->invisible = TRUE;
    sprite->callback = WaitAnimForDuration;

    StoreSpriteCallbackInData6(sprite, AnimFireSpiralOutward_Step1);
}

static void AnimFireSpiralOutward_Step1(struct Sprite *sprite)
{
    sprite->invisible = FALSE;

    sprite->data[0] = sprite->data[1];
    sprite->data[1] = 0;

    sprite->callback = AnimFireSpiralOutward_Step2;
    AnimFireSpiralOutward_Step2(sprite);
}

static void AnimFireSpiralOutward_Step2(struct Sprite *sprite)
{
    sprite->x2 = Sin(sprite->data[1], sprite->data[2] >> 8);
    sprite->y2 = Cos(sprite->data[1], sprite->data[2] >> 8);

    sprite->data[1] = (sprite->data[1] + 10) & 0xFF;
    sprite->data[2] += 0xD0;

    if (--sprite->data[0] == -1)
        DestroyAnimSprite(sprite);
}

#define IDX_ACTIVE_SPRITES 6

#define tState            data[0]
#define tTimer1           data[1]
#define tTimer2           data[2]
#define tTimer3           data[3]
#define tAttackerY        data[4]
#define tAttackerSide     data[5]
#define tActiveSprites    data[IDX_ACTIVE_SPRITES]
#define tAttackerSpriteId data[15]

#define sSpeedDelay       data[0]
#define sLaunchStage      data[1]
#define sX                data[2]
#define sY                data[3]
#define sSpeedX           data[4]
#define sSpeedY           data[5]
#define sTaskId           data[6]
#define sActiveSpritesIdx data[7]

void AnimTask_EruptionLaunchRocks(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    task->tAttackerSpriteId = GetAnimBattlerSpriteId(ANIM_BATTLER_ATTACKER);

    task->tState = 0;
    task->tTimer1 = 0;
    task->tTimer2 = 0;
    task->tTimer3 = 0;
    task->tAttackerY = gSprites[task->tAttackerSpriteId].y;
    task->tAttackerSide = GetBattlerSide(gBattleAnimAttacker);
    task->tActiveSprites = 0;

    PrepareBattlerSpriteForRotScale(task->tAttackerSpriteId, ST_OAM_OBJ_NORMAL);

    task->func = AnimTask_EruptionLaunchRocks_Step;
}

static void AnimTask_EruptionLaunchRocks_Step(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->tState)
    {
    case 0:
        PrepareEruptAnimTaskData(task, task->tAttackerSpriteId, 0x100, 0x100, 0xE0, 0x200, 32);

        task->tState++;
    case 1:
        if (++task->tTimer1 > 1)
        {
            task->tTimer1 = 0;

            if (++task->tTimer2 & 1)
                gSprites[task->tAttackerSpriteId].x2 = 3;
            else
                gSprites[task->tAttackerSpriteId].x2 = -3;
        }

        if (task->tAttackerSide != B_SIDE_PLAYER)
        {
            if (++task->tTimer3 > 4)
            {
                task->tTimer3 = 0;
                gSprites[task->tAttackerSpriteId].y++;
            }
        }

        if (!UpdateEruptAnimTask(task))
        {
            SetBattlerSpriteYOffsetFromYScale(task->tAttackerSpriteId);
            gSprites[task->tAttackerSpriteId].x2 = 0;

            task->tTimer1 = 0;
            task->tTimer2 = 0;
            task->tTimer3 = 0;
            task->tState++;
        }
        break;
    case 2:
        if (++task->tTimer1 > 4)
        {
            if (task->tAttackerSide != B_SIDE_PLAYER)
                PrepareEruptAnimTaskData(task, task->tAttackerSpriteId, 0xE0, 0x200, 0x180, 0xF0, 6);
            else
                PrepareEruptAnimTaskData(task, task->tAttackerSpriteId, 0xE0, 0x200, 0x180, 0xC0, 6);

            task->tTimer1 = 0;
            task->tState++;
        }
        break;
    case 3:
        if (!UpdateEruptAnimTask(task))
        {
            CreateEruptionLaunchRocks(task->tAttackerSpriteId, taskId, IDX_ACTIVE_SPRITES);

            task->tState++;
        }
        break;
    case 4:
        if (++task->tTimer1 > 1)
        {
            task->tTimer1 = 0;

            if (++task->tTimer2 & 1)
                gSprites[task->tAttackerSpriteId].y2 += 3;
            else
                gSprites[task->tAttackerSpriteId].y2 -= 3;
        }

        if (++task->tTimer3 > 24)
        {
            if (task->tAttackerSide != B_SIDE_PLAYER)
                PrepareEruptAnimTaskData(task, task->tAttackerSpriteId, 0x180, 0xF0, 0x100, 0x100, 8);
            else
                PrepareEruptAnimTaskData(task, task->tAttackerSpriteId, 0x180, 0xC0, 0x100, 0x100, 8);

            if (task->tTimer2 & 1)
                gSprites[task->tAttackerSpriteId].y2 -= 3;

            task->tTimer1 = 0;
            task->tTimer2 = 0;
            task->tTimer3 = 0;
            task->tState++;
        }
        break;
    case 5:
        if (task->tAttackerSide != B_SIDE_PLAYER)
            gSprites[task->tAttackerSpriteId].y--;

        if (!UpdateEruptAnimTask(task))
        {
            gSprites[task->tAttackerSpriteId].y = task->tAttackerY;
            ResetSpriteRotScale(task->tAttackerSpriteId);

            task->tTimer2 = 0;
            task->tState++;
        }
        break;
    case 6:
        if (task->tActiveSprites == 0)
            DestroyAnimVisualTask(taskId);

        break;
    default:
        break;
    }
}

static void CreateEruptionLaunchRocks(u8 spriteId, u8 taskId, u8 activeSpritesIdx)
{
    u16 i, j;
    s8 sign;

    u16 y = GetEruptionLaunchRockInitialYPos(spriteId);
    u16 x = gSprites[spriteId].x;

    if (GetBattlerSide(gBattleAnimAttacker) == B_SIDE_PLAYER)
    {
        x -= 0xC;
        sign = 1;
    }
    else
    {
        x += 0x10;
        sign = -1;
    }

    for (i = 0, j = 0; i <= 6; i++)
    {
        u8 spriteId = CreateSprite(&gEruptionLaunchRockSpriteTemplate, x, y, 2);

        if (spriteId != MAX_SPRITES)
        {
            gSprites[spriteId].oam.tileNum += j * 4 + 0x40;

            if (++j >= 5)
                j = 0;

            InitEruptionLaunchRockCoordData(&gSprites[spriteId], sEruptionLaunchRockSpeeds[i][0] * sign, sEruptionLaunchRockSpeeds[i][1]);
            gSprites[spriteId].sTaskId = taskId;
            gSprites[spriteId].sActiveSpritesIdx = activeSpritesIdx;

            gTasks[taskId].data[activeSpritesIdx]++;
        }
    }
}

static void AnimEruptionLaunchRock(struct Sprite *sprite)
{
    UpdateEruptionLaunchRockPos(sprite);

    if (sprite->invisible)
    {
        gTasks[sprite->sTaskId].data[sprite->sActiveSpritesIdx]--;
        DestroySprite(sprite);
    }
}

u16 GetEruptionLaunchRockInitialYPos(u8 spriteId)
{
    u16 y = gSprites[spriteId].y + gSprites[spriteId].y2 + gSprites[spriteId].centerToCornerVecY;

    if (GetBattlerSide(gBattleAnimAttacker) == B_SIDE_PLAYER)
    {
        y = ((y << 16) + 0x4A0000) >> 16;
    }
    else
    {
        y = ((y << 16) + 0x2C0000) >> 16;
    }

    return y;
}

void InitEruptionLaunchRockCoordData(struct Sprite *sprite, s16 speedX, s16 speedY)
{
    sprite->sSpeedDelay = 0;
    sprite->sLaunchStage = 0;
    sprite->sX = (u16)sprite->x * 8;
    sprite->sY = (u16)sprite->y * 8;
    sprite->sSpeedX = speedX * 8;
    sprite->sSpeedY = speedY * 8;
}

static void UpdateEruptionLaunchRockPos(struct Sprite *sprite)
{
    int extraLaunchSpeed;
    if (++sprite->sSpeedDelay > 2)
    {
        sprite->sSpeedDelay = 0;
        ++sprite->sLaunchStage;
        extraLaunchSpeed = (u16)sprite->sLaunchStage * (u16)sprite->sLaunchStage;
        sprite->sY += extraLaunchSpeed;
    }

    sprite->sX += sprite->sSpeedX;
    sprite->x = sprite->sX >> 3;
    sprite->sY += sprite->sSpeedY;
    sprite->y = sprite->sY >> 3;

    if (sprite->x < -8 || sprite->x > DISPLAY_WIDTH + 8 || sprite->y < -8 || sprite->y > 120)
        sprite->invisible = TRUE;
}

#undef sSpeedDelay
#undef sLaunchStage
#undef sX
#undef sY
#undef sSpeedX
#undef sSpeedY
#undef sTaskId
#undef sActiveSpritesIdx

#define sState       data[0]
#define sBounceTimer data[1]
#define sBounceDir   data[2]
#define sEndTimer    data[3]
#define sFallDelay   data[6]
#define sTargetY     data[7]

static void AnimEruptionFallingRock(struct Sprite *sprite)
{
    sprite->x = gBattleAnimArgs[0];
    sprite->y = gBattleAnimArgs[1];

    sprite->sState = 0;
    sprite->sBounceTimer = 0;
    sprite->sBounceDir = 0;
    sprite->sFallDelay = gBattleAnimArgs[2];
    sprite->sTargetY = gBattleAnimArgs[3];

    sprite->oam.tileNum += gBattleAnimArgs[4] * 16;
    sprite->callback = AnimEruptionFallingRock_Step;
}

static void AnimEruptionFallingRock_Step(struct Sprite *sprite)
{
    switch (sprite->sState)
    {
    case 0:
        if (sprite->sFallDelay != 0)
        {
            sprite->sFallDelay--;
            return;
        }

        sprite->sState++;
        // fall through
    case 1:
        sprite->y += 8;
        if (sprite->y >= sprite->sTargetY)
        {
            sprite->y = sprite->sTargetY;
            sprite->sState++;
        }
        break;
    case 2:
        if (++sprite->sBounceTimer > 1)
        {
            sprite->sBounceTimer = 0;
            if ((++sprite->sBounceDir & 1) != 0)
            {
                sprite->y2 = -3;
            }
            else
            {
                sprite->y2 = 3;
            }
        }

        if (++sprite->sEndTimer > 16)
        {
            DestroyAnimSprite(sprite);
        }
        break;
    }
}

#undef IDX_ACTIVE_SPRITES
#undef tState
#undef tTimer1
#undef tTimer2
#undef tTimer3
#undef tAttackerY
#undef tAttackerSide
#undef tActiveSprites
#undef tAttackerSpriteId
#undef sState
#undef sBounceTimer
#undef sBounceDir
#undef sEndTimer
#undef sFallDelay
#undef sTargetY
