#include "global.h"
#include "constants/battle.h"
#include "battle_anim.h"
#include "blend_palette.h"
#include "main.h"
#include "rom_8077ABC.h"
#include "task.h"
#include "trig.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimTarget;
extern u8 gBattleAnimAttacker;
extern u8 gBattlerSpriteIds[];

static void AnimWillOWispFire(struct Sprite *sprite);
static void AnimTask_MoveHeatWaveTargets_Step(u8 taskId);

static const union AnimCmd sAnim_WillOWispFire[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_FRAME(16, 5),
    ANIMCMD_FRAME(32, 5),
    ANIMCMD_FRAME(48, 5),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd *const sAnims_WillOWispFire[] =
{
    sAnim_WillOWispFire,
};

const struct SpriteTemplate gWillOWispFireSpriteTemplate =
{
    .tileTag = ANIM_TAG_WISP_FIRE,
    .paletteTag = ANIM_TAG_WISP_FIRE,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = sAnims_WillOWispFire,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimWillOWispFire,
};

// Directions for shaking up/down or left/right in AnimTask_ShakeTargetInPattern.
// Only the first 10 values are accessed.
static const s8 sShakeDirsPattern0[16] =
{
    -1, -1, 0, 1, 1, 0, 0, -1, -1, 1, 1, 0, 0, -1, 0, 1,
};

static const s8 sShakeDirsPattern1[16] =
{
    -1, 0, 1, 0, -1, 1, 0, -1, 0, 1, 0, -1, 0, 1, 0, 1,
};

static void AnimWillOWispFire(struct Sprite *sprite)
{
    if (!sprite->data[0])
    {
        sprite->data[1] = gBattleAnimArgs[0];
        sprite->data[0] += 1;
    }

    sprite->data[3] += 0xC0 * 2;
    sprite->data[4] += 0xA0;

    sprite->x2 = Sin(sprite->data[1], sprite->data[3] >> 8);
    sprite->y2 = Cos(sprite->data[1], sprite->data[4] >> 8);

    sprite->data[1] = (sprite->data[1] + 7) & 0xFF;

    if (gMain.inBattle)
    {
        if (sprite->data[1] < 64 || sprite->data[1] > 195)
            sprite->oam.priority = GetBattlerSpriteBGPriority(gBattleAnimTarget);
        else
            sprite->oam.priority = GetBattlerSpriteBGPriority(gBattleAnimTarget) + 1;
    }
    else
    {
        if (sprite->data[1] < 64 || sprite->data[1] > 195)
            sprite->subpriority = 0x1D;
        else
            sprite->subpriority = 0x1F;
    }

    if (++sprite->data[2] > 0x14)
        sprite->invisible ^= 1;

    if (sprite->data[2] == 0x1E)
        DestroyAnimSprite(sprite);
}

void AnimTask_MoveHeatWaveTargets(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    task->data[12] = GetBattlerSide(gBattleAnimAttacker) == B_SIDE_PLAYER ? 1 : -1;
    task->data[13] = IsAnimBankSpriteVisible(BATTLE_PARTNER(gBattleAnimTarget)) + 1;
    task->data[14] = GetAnimBattlerSpriteId(ANIM_BATTLER_TARGET);
    task->data[15] = GetAnimBattlerSpriteId(ANIM_BATTLER_DEF_PARTNER);

    task->func = AnimTask_MoveHeatWaveTargets_Step;
}

static void AnimTask_MoveHeatWaveTargets_Step(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->data[0])
    {
        case 0:
            task->data[10] += task->data[12] * 2;
            if (++task->data[1] >= 2)
            {
                task->data[1] = 0;
                task->data[2]++;
                if (task->data[2] & 1)
                    task->data[11] = 2;
                else
                    task->data[11] = -2;
            }

            for (task->data[3] = 0; task->data[3] < task->data[13]; task->data[3]++)
            {
                gSprites[task->data[task->data[3] + 14]].x2 = task->data[10] + task->data[11];
            }

            if (++task->data[9] == 16)
            {
                task->data[9] = 0;
                task->data[0]++;
            }
            break;
        case 1:
            if (++task->data[1] >= 5)
            {
                task->data[1] = 0;
                task->data[2]++;

                if (task->data[2] & 1)
                    task->data[11] = 2;
                else
                    task->data[11] = -2;
            }

            for (task->data[3] = 0; task->data[3] < task->data[13]; task->data[3]++)
            {
                gSprites[task->data[task->data[3] + 14]].x2 = task->data[10] + task->data[11];
            }

            if (++task->data[9] == 96)
            {
                task->data[9] = 0;
                task->data[0]++;
            }
            break;
        case 2:
            task->data[10] -= task->data[12] * 2;

            if (++task->data[1] >= 2)
            {
                task->data[1] = 0;
                task->data[2]++;

                if (task->data[2] & 1)
                    task->data[11] = 2;
                else
                    task->data[11] = -2;
            }

            for (task->data[3] = 0; task->data[3] < task->data[13]; task->data[3]++)
            {
                gSprites[task->data[task->data[3] + 14]].x2 = task->data[10] + task->data[11];
            }

            if (++task->data[9] == 16)
            {
                task->data[0]++;
            }
            break;
        case 3:
            for (task->data[3] = 0; task->data[3] < task->data[13]; task->data[3]++)
            {
                gSprites[task->data[task->data[3] + 14]].x2 = 0;
            }

            DestroyAnimVisualTask(taskId);
            break;
    }
}

// Used to add a color mask to the battle interface / HUD in Heat Wave.
// arg 0: opacity
// arg 1: color code
void AnimTask_BlendBackground(u8 taskId)
{
    struct BattleAnimBgData animBg;
    GetBattleAnimBg1Data(&animBg);
    BlendPalette(animBg.paletteId << 4, 16, gBattleAnimArgs[0], gBattleAnimArgs[1]); // u16 palOffset, u16 numEntries, u8 coeff, u16 blendColor
    DestroyAnimVisualTask(taskId);
}

#define tShakeNum    data[0]
#define tMaxShakes   data[1]
#define tShakeOffset data[2] // Never read; gBattleAnimArgs[1] is used directly.
#define tVertical    data[3]
#define tPatternId   data[4]

void AnimTask_ShakeTargetInPattern(u8 taskId)
{
    s8 dir;
    u8 spriteId;

    if (gTasks[taskId].tShakeNum == 0)
    {
        gTasks[taskId].tMaxShakes = gBattleAnimArgs[0];
        gTasks[taskId].tShakeOffset = gBattleAnimArgs[1];
        gTasks[taskId].tVertical = gBattleAnimArgs[2];
        gTasks[taskId].tPatternId = gBattleAnimArgs[3];
    }
    gTasks[taskId].tShakeNum++;

    spriteId = gBattlerSpriteIds[gBattleAnimTarget];

    if (gTasks[taskId].tPatternId == 0)
        dir = sShakeDirsPattern0[gTasks[taskId].tShakeNum % 10];
    else
        dir = sShakeDirsPattern1[gTasks[taskId].tShakeNum % 10];

    if (gTasks[taskId].tVertical == TRUE)
        gSprites[spriteId].y2 = gBattleAnimArgs[1] * dir < 0 ? -(gBattleAnimArgs[1] * dir) : gBattleAnimArgs[1] * dir;
    else
        gSprites[spriteId].x2 = gBattleAnimArgs[1] * dir;

    if (gTasks[taskId].tShakeNum == gTasks[taskId].tMaxShakes)
    {
        gSprites[spriteId].x2 = 0;
        gSprites[spriteId].y2 = 0;
        DestroyAnimVisualTask(taskId);
    }
}

#undef tShakeNum
#undef tMaxShakes
#undef tShakeOffset
#undef tVertical
#undef tPatternId
