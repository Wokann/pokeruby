#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"
#include "constants/battle.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

extern u8 gBattlerSpriteIds[];

static void AnimBowMon(struct Sprite *sprite);
static void AnimTipMon(struct Sprite *sprite);
static void AnimBowMon_Step1(struct Sprite *sprite);
static void AnimBowMon_Step1_Callback(struct Sprite *sprite);
static void AnimBowMon_Step2(struct Sprite *sprite);
static void AnimBowMon_Step3(struct Sprite *sprite);
static void AnimBowMon_Step3_Callback(struct Sprite *sprite);
static void AnimBowMon_Step4(struct Sprite *sprite);
static void AnimTipMon_Step(struct Sprite *sprite);
static void AnimTask_SkullBashPositionSet(u8 taskId);
static void AnimTask_SkullBashPositionReset(u8 taskId);

// Bowing and tipping battler effects, plus Skull Bash positioning.

const struct SpriteTemplate gBowMonSpriteTemplate =
{
    .tileTag = 0,
    .paletteTag = 0,
    .oam = &gDummyOamData,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimBowMon,
};

// Unused: tips the battler without first moving it back.
static const struct SpriteTemplate sTipMonSpriteTemplate =
{
    .tileTag = 0,
    .paletteTag = 0,
    .oam = &gDummyOamData,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimTipMon,
};

static void AnimBowMon(struct Sprite *sprite)
{
    sprite->invisible = TRUE;
    sprite->data[0] = 0;
    switch (gBattleAnimArgs[0])
    {
    case 0:
        sprite->callback = AnimBowMon_Step1;
        break;
    case 1:
        sprite->callback = AnimBowMon_Step2;
        break;
    case 2:
        sprite->callback = AnimBowMon_Step3;
        break;
    default:
        sprite->callback = AnimBowMon_Step4;
        break;
    }
}

static void AnimBowMon_Step1(struct Sprite *sprite)
{
    sprite->data[0] = 6;
    sprite->data[1] = (GetBattlerSide(gBattleAnimAttacker)) ? 2 : -2;
    sprite->data[2] = 0;
    sprite->data[3] = gBattlerSpriteIds[gBattleAnimAttacker];
    StoreSpriteCallbackInData6(sprite, AnimBowMon_Step1_Callback);
    sprite->callback = TranslateMonBGUntil;
}

static void AnimBowMon_Step1_Callback(struct Sprite *sprite)
{
    if (sprite->data[0] == 0)
    {
        sprite->data[3] = gBattlerSpriteIds[gBattleAnimAttacker];
        PrepareBattlerSpriteForRotScale(sprite->data[3], ST_OAM_OBJ_NORMAL);
        sprite->data[4] = (sprite->data[6] = GetBattlerSide(gBattleAnimAttacker)) ? 0x300 : 0xFFFFFD00;
        sprite->data[5] = 0;
    }

    sprite->data[5] += sprite->data[4];
    SetSpriteRotScale(sprite->data[3], 0x100, 0x100, sprite->data[5]);
    SetBattlerSpriteYOffsetFromRotation(sprite->data[3]);
    if (++sprite->data[0] > 3)
    {
        sprite->data[0] = 0;
        sprite->callback = AnimBowMon_Step4;
    }
}

static void AnimBowMon_Step2(struct Sprite *sprite)
{
    sprite->data[0] = 4;
    sprite->data[1] = (GetBattlerSide(gBattleAnimAttacker)) ? -3 : 3;
    sprite->data[2] = 0;
    sprite->data[3] = gBattlerSpriteIds[gBattleAnimAttacker];
    StoreSpriteCallbackInData6(sprite, AnimBowMon_Step4);
    sprite->callback = TranslateMonBGUntil;
}

static void AnimBowMon_Step3(struct Sprite *sprite)
{
    if (++sprite->data[0] > 8)
    {
        sprite->data[0] = 0;
        sprite->callback = AnimBowMon_Step3_Callback;
    }
}

static void AnimBowMon_Step3_Callback(struct Sprite *sprite)
{
    if (sprite->data[0] == 0)
    {
        sprite->data[3] = gBattlerSpriteIds[gBattleAnimAttacker];
        sprite->data[6] = GetBattlerSide(gBattleAnimAttacker);
        if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
        {
            sprite->data[4] = 0xFC00;
            sprite->data[5] = 0xC00;
        }
        else
        {
            sprite->data[4] = 0x400;
            sprite->data[5] = 0xF400;
        }
    }

    sprite->data[5] += sprite->data[4];
    SetSpriteRotScale(sprite->data[3], 0x100, 0x100, sprite->data[5]);
    SetBattlerSpriteYOffsetFromRotation(sprite->data[3]);
    if (++sprite->data[0] > 2)
    {
        ResetSpriteRotScale(sprite->data[3]);
        sprite->callback = AnimBowMon_Step4;
    }
}

static void AnimBowMon_Step4(struct Sprite *sprite)
{
    DestroyAnimSprite(sprite);
}

static void AnimTipMon(struct Sprite *sprite)
{
    sprite->data[0] = 0;
    sprite->callback = AnimTipMon_Step;
}

static void AnimTipMon_Step(struct Sprite *sprite)
{
    switch (sprite->data[0])
    {
    case 0:
        sprite->data[1] = 0;
        sprite->data[2] = gBattlerSpriteIds[gBattleAnimAttacker];
        sprite->data[3] = GetBattlerSide(gBattleAnimAttacker);
        sprite->data[4] = (sprite->data[3] != B_SIDE_PLAYER) ? 0x200 : -0x200;
        sprite->data[5] = 0;
        PrepareBattlerSpriteForRotScale(sprite->data[2], ST_OAM_OBJ_NORMAL);
        sprite->data[0]++;
    case 1:
        sprite->data[5] += sprite->data[4];
        SetSpriteRotScale(sprite->data[2], 0x100, 0x100, sprite->data[5]);
        SetBattlerSpriteYOffsetFromRotation(sprite->data[2]);
        if (++sprite->data[1] > 3)
        {
            sprite->data[1] = 0;
            sprite->data[4] *= -1;
            sprite->data[0]++;
        }
        break;
    case 2:
        sprite->data[5] += sprite->data[4];
        SetSpriteRotScale(sprite->data[2], 0x100, 0x100, sprite->data[5]);
        SetBattlerSpriteYOffsetFromRotation(sprite->data[2]);
        if (++sprite->data[1] > 3)
        {
            ResetSpriteRotScale(sprite->data[2]);
            DestroyAnimSprite(sprite);
        }
        break;
    }
}

void AnimTask_SkullBashPosition(u8 taskId)
{
    u8 side;

    gTasks[taskId].data[0] = gBattlerSpriteIds[gBattleAnimAttacker];
    side = GetBattlerSide(gBattleAnimAttacker);
    gTasks[taskId].data[1] = side;
    gTasks[taskId].data[2] = 0;
    switch (gBattleAnimArgs[0])
    {
    default:
        DestroyAnimVisualTask(taskId);
        break;
    case 0:
        gTasks[taskId].data[2] = 0;
        gTasks[taskId].data[3] = 8;
        gTasks[taskId].data[4] = 0;
        gTasks[taskId].data[5] = 3;
        if (side == B_SIDE_PLAYER)
            gTasks[taskId].data[5] *= -1;

        gTasks[taskId].func = AnimTask_SkullBashPositionSet;
        break;
    case 1:
        gTasks[taskId].data[3] = 8;
        gTasks[taskId].data[4] = 0x600;
        gTasks[taskId].data[5] = 0xC0;
        if (side == B_SIDE_PLAYER)
        {
            gTasks[taskId].data[4] = -gTasks[taskId].data[4];
            gTasks[taskId].data[5] = -gTasks[taskId].data[5];
        }

        gTasks[taskId].func = AnimTask_SkullBashPositionReset;
        break;
    }
}

static void AnimTask_SkullBashPositionSet(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    switch (task->data[2])
    {
    case 0:
        if (task->data[3])
        {
            task->data[4] += task->data[5];
            gSprites[task->data[0]].x2 = task->data[4];
            task->data[3]--;
        }
        else
        {
            task->data[3] = 8;
            task->data[4] = 0;
            task->data[5] = (task->data[1] == B_SIDE_PLAYER) ? -0xC0 : 0xC0;
            PrepareBattlerSpriteForRotScale(task->data[0], ST_OAM_OBJ_NORMAL);
            task->data[2]++;
        }
        break;
    case 1:
        if (task->data[3])
        {
            task->data[4] += task->data[5];
            SetSpriteRotScale(task->data[0], 0x100, 0x100, task->data[4]);
            SetBattlerSpriteYOffsetFromRotation(task->data[0]);
            task->data[3]--;
        }
        else
        {
            task->data[3] = 8;
            task->data[4] = gSprites[task->data[0]].x2;
            task->data[5] = (task->data[1] == B_SIDE_PLAYER) ? 0x2 : -0x2;
            task->data[6] = 1;
            task->data[2]++;
        }
        break;
    case 2:
        if (task->data[3])
        {
            if (task->data[6])
            {
                task->data[6]--;
            }
            else
            {
                if (task->data[3] & 1)
                    gSprites[task->data[0]].x2 = task->data[4] + task->data[5];
                else
                    gSprites[task->data[0]].x2 = task->data[4] - task->data[5];

                task->data[6] = 1;
                task->data[3]--;
            }
        }
        else
        {
            gSprites[task->data[0]].x2 = task->data[4];
            task->data[3] = 12;
            task->data[2]++;
        }
        break;
    case 3:
        if (task->data[3])
        {
            task->data[3]--;
        }
        else
        {
            task->data[3] = 3;
            task->data[4] = gSprites[task->data[0]].x2;
            task->data[5] = (task->data[1] == B_SIDE_PLAYER) ? 8 : -8;
            task->data[2]++;
        }
        break;
    case 4:
        if (task->data[3])
        {
            task->data[4] += task->data[5];
            gSprites[task->data[0]].x2 = task->data[4];
            task->data[3]--;
        }
        else
        {
            DestroyAnimVisualTask(taskId);
        }
        break;
    }
}

static void AnimTask_SkullBashPositionReset(u8 taskId)
{
    struct Task* task = &gTasks[taskId];
    if (task->data[3])
    {
        task->data[4] -= task->data[5];
        SetSpriteRotScale(task->data[0], 0x100, 0x100, task->data[4]);
        SetBattlerSpriteYOffsetFromRotation(task->data[0]);
        task->data[3]--;
    }
    else
    {
        ResetSpriteRotScale(task->data[0]);
        DestroyAnimVisualTask(taskId);
    }
}
