#include "global.h"
#include "battle.h"
#include "battle_anim.h"
#include "task.h"
#include "trig.h"

static void AnimEmberFlare(struct Sprite *sprite);
static void AnimBurnFlame(struct Sprite *sprite);
static void AnimFireRing(struct Sprite *sprite);
static void AnimFireCross(struct Sprite *sprite);
static void AnimFireSpiralOutward(struct Sprite *sprite);
static void AnimEruptionLaunchRock(struct Sprite *sprite);
static void AnimEruptionFallingRock(struct Sprite *sprite);
static void AnimFireRing_Step1(struct Sprite *);
static void UpdateFireRingCircleOffset(struct Sprite *);
static void AnimFireRing_Step2(struct Sprite *);
static void AnimFireRing_Step3(struct Sprite *);
static void AnimFireSpiralOutward_Step1(struct Sprite *);
static void AnimFireSpiralOutward_Step2(struct Sprite *);
static void AnimTask_EruptionLaunchRocks_Step(u8 taskId);
static void CreateEruptionLaunchRocks(u8 spriteId, u8 taskId, u8 activeSpritesIdx);
static u16 GetEruptionLaunchRockInitialYPos(u8 spriteId);
static void InitEruptionLaunchRockCoordData(struct Sprite *sprite, s16 speedX, s16 speedY);
static void UpdateEruptionLaunchRockPos(struct Sprite *);
static void AnimEruptionFallingRock_Step(struct Sprite *);

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

static const union AnimCmd sAnim_FireBlastCross[] =
{
    ANIMCMD_FRAME(32, 6),
    ANIMCMD_FRAME(48, 6),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd *const sAnims_FireBlastCross[] =
{
    sAnim_FireBlastCross,
};

static const union AffineAnimCmd sAffineAnim_Unused_0[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, 0, 1),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_Unused_1[] =
{
    AFFINEANIMCMD_FRAME(0xA0, 0xA0, 0, 0),
    AFFINEANIMCMD_END,
};

// Unused
static const union AffineAnimCmd *const sAffineAnims_Unused[] =
{
    sAffineAnim_Unused_0,
    sAffineAnim_Unused_1,
};

const struct SpriteTemplate gFireBlastCrossSpriteTemplate =
{
    .tileTag = ANIM_TAG_SMALL_EMBER,
    .paletteTag = ANIM_TAG_SMALL_EMBER,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = sAnims_FireBlastCross,
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

static const s16 sEruptionLaunchRockSpeeds[][2] =
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
static void AnimEmberFlare(struct Sprite *sprite)
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
static void AnimFireRing(struct Sprite *sprite)
{
    InitSpritePosToAnimAttacker(sprite, 1);

    sprite->data[7] = gBattleAnimArgs[2];
    sprite->data[0] = 0;

    sprite->callback = AnimFireRing_Step1;
}

static void AnimFireRing_Step1(struct Sprite *sprite)
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

        sprite->callback = AnimFireRing_Step2;
    }
}

static void AnimFireRing_Step2(struct Sprite *sprite)
{
    if (AnimTranslateLinear(sprite))
    {
        sprite->data[0] = 0;

        sprite->x = GetBattlerSpriteCoord(gBattleAnimTarget, 2);
        sprite->y = GetBattlerSpriteCoord(gBattleAnimTarget, 3);
        sprite->y2 = 0;
        sprite->x2 = 0;

        sprite->callback = AnimFireRing_Step3;
        sprite->callback(sprite);
    }
    else
    {
        sprite->x2 += Sin(sprite->data[7], 28);
        sprite->y2 += Cos(sprite->data[7], 28);

        sprite->data[7] = (sprite->data[7] + 20) & 0xFF;
    }
}

static void AnimFireRing_Step3(struct Sprite *sprite)
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
static void AnimFireCross(struct Sprite *sprite)
{
    sprite->x += gBattleAnimArgs[0];
    sprite->y += gBattleAnimArgs[1];

    sprite->data[0] = gBattleAnimArgs[2];
    sprite->data[1] = gBattleAnimArgs[3];
    sprite->data[2] = gBattleAnimArgs[4];

    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);

    sprite->callback = TranslateSpriteLinear;
}

static void AnimFireSpiralOutward(struct Sprite *sprite)
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

// Animates first stage of Eruption where the attacker squishes and launches rocks away from themself
void AnimTask_EruptionLaunchRocks(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    task->data[15] = GetAnimBattlerSpriteId(0);

    task->data[0] = 0;
    task->data[1] = 0;
    task->data[2] = 0;
    task->data[3] = 0;
    task->data[4] = gSprites[task->data[15]].y;
    task->data[5] = GetBattlerSide(gBattleAnimAttacker);
    task->data[6] = 0;

    PrepareBattlerSpriteForRotScale(task->data[15], 0);

    task->func = AnimTask_EruptionLaunchRocks_Step;
}

static void AnimTask_EruptionLaunchRocks_Step(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->data[0])
    {
    case 0:
        PrepareEruptAnimTaskData(task, task->data[15], 0x100, 0x100, 0xE0, 0x200, 0x20);

        task->data[0]++;
    case 1:
        if (++task->data[1] > 1)
        {
            task->data[1] = 0;

            if (++task->data[2] & 0x1)
                gSprites[task->data[15]].x2 = 3;
            else
                gSprites[task->data[15]].x2 = -3;
        }

        if (task->data[5])
        {
            if (++task->data[3] > 4)
            {
                task->data[3] = 0;
                gSprites[task->data[15]].y++;
            }
        }

        if(!UpdateEruptAnimTask(task))
        {
            SetBattlerSpriteYOffsetFromYScale(task->data[15]);
            gSprites[task->data[15]].x2 = 0;

            task->data[1] = 0;
            task->data[2] = 0;
            task->data[3] = 0;
            task->data[0]++;
        }
        break;
    case 2:
        if (++task->data[1] > 4)
        {
            if (task->data[5])
                PrepareEruptAnimTaskData(task, task->data[15], 0xE0, 0x200, 0x180, 0xF0, 0x6);
            else
                PrepareEruptAnimTaskData(task, task->data[15], 0xE0, 0x200, 0x180, 0xC0, 0x6);

            task->data[1] = 0;
            task->data[0]++;
        }
        break;
    case 3:
        if (!UpdateEruptAnimTask(task))
        {
            CreateEruptionLaunchRocks(task->data[15], taskId, 6);

            task->data[0]++;
        }
        break;
    case 4:
        if (++task->data[1] > 1)
        {
            task->data[1] = 0;

            if (++task->data[2] & 1)
                gSprites[task->data[15]].y2 += 3;
            else
                gSprites[task->data[15]].y2 -= 3;
        }

        if (++task->data[3] > 0x18)
        {
            if (task->data[5])
                PrepareEruptAnimTaskData(task, task->data[15], 0x180, 0xF0, 0x100, 0x100, 0x8);
            else
                PrepareEruptAnimTaskData(task, task->data[15], 0x180, 0xC0, 0x100, 0x100, 0x8);

            if (task->data[2] & 1)
                gSprites[task->data[15]].y2 -= 3;

            task->data[1] = 0;
            task->data[2] = 0;
            task->data[3] = 0;
            task->data[0]++;
        }
        break;
    case 5:
        if (task->data[5])
            gSprites[task->data[15]].y--;

        if (!UpdateEruptAnimTask(task))
        {
            gSprites[task->data[15]].y = task->data[4];
            ResetSpriteRotScale(task->data[15]);

            task->data[2] = 0;
            task->data[0]++;
        }
        break;
    case 6:
        if (!task->data[6])
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

    if(!GetBattlerSide(gBattleAnimAttacker))
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

        if (spriteId != 0x40)
        {
            gSprites[spriteId].oam.tileNum += j * 4 + 0x40;

            if (++j >= 5)
                j = 0;

            InitEruptionLaunchRockCoordData(&gSprites[spriteId], sEruptionLaunchRockSpeeds[i][0] * sign, sEruptionLaunchRockSpeeds[i][1]);
            gSprites[spriteId].data[6] = taskId;
            gSprites[spriteId].data[7] = activeSpritesIdx;

            gTasks[taskId].data[activeSpritesIdx]++;
        }
    }
}

static void AnimEruptionLaunchRock(struct Sprite *sprite)
{
    UpdateEruptionLaunchRockPos(sprite);

    if (sprite->invisible)
    {
        gTasks[sprite->data[6]].data[sprite->data[7]]--;
        DestroySprite(sprite);
    }
}

static u16 GetEruptionLaunchRockInitialYPos(u8 spriteId)
{
    u16 y = gSprites[spriteId].y + gSprites[spriteId].y2 + gSprites[spriteId].centerToCornerVecY;

    if (GetBattlerSide(gBattleAnimAttacker) == 0)
    {
        y = ((y << 16) + 0x4A0000) >> 16;
    }
    else
    {
        y = ((y << 16) + 0x2C0000) >> 16;
    }

    return y;
}

static void InitEruptionLaunchRockCoordData(struct Sprite *sprite, s16 speedX, s16 speedY)
{
    sprite->data[0] = 0;
    sprite->data[1] = 0;
    sprite->data[2] = (u16)sprite->x * 8;
    sprite->data[3] = (u16)sprite->y * 8;
    sprite->data[4] = speedX * 8;
    sprite->data[5] = speedY * 8;
}

static void UpdateEruptionLaunchRockPos(struct Sprite *sprite)
{
    int extraLaunchSpeed;
    if (++sprite->data[0] > 2)
    {
        sprite->data[0] = 0;
        ++sprite->data[1];
        extraLaunchSpeed = (u16)sprite->data[1] * (u16)sprite->data[1];
        sprite->data[3] += extraLaunchSpeed;
    }

    sprite->data[2] += sprite->data[4];
    sprite->x = sprite->data[2] >> 3;
    sprite->data[3] += sprite->data[5];
    sprite->y = sprite->data[3] >> 3;

    if (sprite->x < -8 || sprite->x > 0xf8 || sprite->y < -8 || sprite->y > 120)
        sprite->invisible = TRUE;
}

static void AnimEruptionFallingRock(struct Sprite *sprite)
{
    sprite->x = gBattleAnimArgs[0];
    sprite->y = gBattleAnimArgs[1];

    sprite->data[0] = 0;
    sprite->data[1] = 0;
    sprite->data[2] = 0;
    sprite->data[6] = gBattleAnimArgs[2];
    sprite->data[7] = gBattleAnimArgs[3];

    sprite->oam.tileNum += gBattleAnimArgs[4] * 16;
    sprite->callback = AnimEruptionFallingRock_Step;
}

static void AnimEruptionFallingRock_Step(struct Sprite *sprite)
{
    switch (sprite->data[0])
    {
    case 0:
        if (sprite->data[6] != 0)
        {
            sprite->data[6]--;
            return;
        }

        sprite->data[0]++;
        // fall through
    case 1:
        sprite->y += 8;
        if (sprite->y >= sprite->data[7])
        {
            sprite->y = sprite->data[7];
            sprite->data[0]++;
        }
        break;
    case 2:
        if (++sprite->data[1] > 1)
        {
            sprite->data[1] = 0;
            if ((++sprite->data[2] & 1) != 0)
            {
                sprite->y2 = -3;
            }
            else
            {
                sprite->y2 = 3;
            }
        }

        if (++sprite->data[3] > 16)
        {
            DestroyAnimSprite(sprite);
        }
        break;
    }
}
