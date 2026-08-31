#include "global.h"
#include "constants/battle.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "sound.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimTask_LeafBlade_Step(u8 taskId);
static s16 LeafBladeGetPosFactor(struct Sprite *sprite);
static void AnimTask_LeafBlade_Step2(struct Task *task, u8 taskId);
static void AnimTask_LeafBlade_Step2_Callback(struct Sprite *sprite);

// flying_path (guides a sprite along a specific path.)
// Used by Leaf Blade.

const union AnimCmd gLeafBladeAnimCmds1[] =
{
    ANIMCMD_FRAME(28, 1),
    ANIMCMD_END,
};

const union AnimCmd gLeafBladeAnimCmds2[] =
{
    ANIMCMD_FRAME(32, 1),
    ANIMCMD_END,
};

const union AnimCmd gLeafBladeAnimCmds3[] =
{
    ANIMCMD_FRAME(20, 1),
    ANIMCMD_END,
};

const union AnimCmd gLeafBladeAnimCmds4[] =
{
    ANIMCMD_FRAME(28, 1, .hFlip = TRUE),
    ANIMCMD_END,
};

const union AnimCmd gLeafBladeAnimCmds5[] =
{
    ANIMCMD_FRAME(16, 1),
    ANIMCMD_END,
};

const union AnimCmd gLeafBladeAnimCmds6[] =
{
    ANIMCMD_FRAME(16, 1, .hFlip = TRUE),
    ANIMCMD_END,
};

const union AnimCmd gLeafBladeAnimCmds7[] =
{
    ANIMCMD_FRAME(28, 1),
    ANIMCMD_END,
};

const union AnimCmd *const gLeafBladeAnimTable[] =
{
    gLeafBladeAnimCmds1,
    gLeafBladeAnimCmds2,
    gLeafBladeAnimCmds3,
    gLeafBladeAnimCmds4,
    gLeafBladeAnimCmds5,
    gLeafBladeAnimCmds6,
    gLeafBladeAnimCmds7,
};

const struct SpriteTemplate gLeafBladeSpriteTemplate =
{
    .tileTag = ANIM_TAG_LEAF,
    .paletteTag = ANIM_TAG_LEAF,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = gLeafBladeAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

void AnimTask_LeafBlade(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    task->data[4] = GetBattlerSpriteSubpriority(gBattleAnimTarget) - 1;
    task->data[6] = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_X_2);
    task->data[7] = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_Y_PIC_OFFSET);
    task->data[10] = GetBattlerSpriteCoordAttr(gBattleAnimTarget, BATTLER_COORD_ATTR_WIDTH);
    task->data[11] = GetBattlerSpriteCoordAttr(gBattleAnimTarget, BATTLER_COORD_ATTR_HEIGHT);
    task->data[5] = (GetBattlerSide(gBattleAnimTarget) == B_SIDE_OPPONENT) ? 1 : -1;
    task->data[9] = 56 - (task->data[5] * 64);
    task->data[8] = task->data[7] - task->data[9] + task->data[6];
    task->data[2] = CreateSprite(&gLeafBladeSpriteTemplate, task->data[8], task->data[9], task->data[4]);
    if (task->data[2] == MAX_SPRITES)
        DestroyAnimVisualTask(taskId);

    gSprites[task->data[2]].data[0] = 10;
    gSprites[task->data[2]].data[1] = task->data[8];
    gSprites[task->data[2]].data[2] = task->data[6] - (task->data[10] / 2 + 10) * task->data[5];
    gSprites[task->data[2]].data[3] = task->data[9];
    gSprites[task->data[2]].data[4] = task->data[7] + (task->data[11] / 2 + 10) * task->data[5];
    gSprites[task->data[2]].data[5] = LeafBladeGetPosFactor(&gSprites[task->data[2]]);
    InitAnimArcTranslation(&gSprites[task->data[2]]);
    task->func = AnimTask_LeafBlade_Step;
}

static void AnimTask_LeafBlade_Step(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    struct Sprite *sprite = &gSprites[task->data[2]];
    int a = task->data[0];
    switch (a)
    {
    case 4:
        AnimTask_LeafBlade_Step2(task, taskId);
        if (TranslateAnimArc(sprite) == 0)
        {
            break;
        }
        else
        {
            task->data[15] = 5;
            task->data[0] = 0xFF;
        }
        break;
    case 8:
        AnimTask_LeafBlade_Step2(task, taskId);
        if (TranslateAnimArc(sprite) == 0)
        {
            break;
        }
        else
        {
            task->data[15] = 9;
            task->data[0] = 0xFF;
        }
        break;
    case 0:
        AnimTask_LeafBlade_Step2(task, taskId);
        if (TranslateAnimArc(sprite) == 0)
            break;

        task->data[15] = 1;
        task->data[0] = 0xFF;
        break;
    case 1:
        sprite->x += sprite->x2;
        sprite->y += sprite->y2;
        sprite->x2 = 0;
        sprite->y2 = 0;
        sprite->data[0] = 10;
        sprite->data[1] = sprite->x;
        sprite->data[2] = task->data[6];
        sprite->data[3] = sprite->y;
        sprite->data[4] = task->data[7];
        sprite->data[5] = LeafBladeGetPosFactor(sprite);
        task->data[4] += 2;
        task->data[3] = a;
        sprite->subpriority = task->data[4];
        StartSpriteAnim(sprite, task->data[3]);
        InitAnimArcTranslation(sprite);
        task->data[0]++;
        break;
    case 2:
        AnimTask_LeafBlade_Step2(task, taskId);
        if (TranslateAnimArc(sprite) == 0)
            break;

        task->data[15] = 3;
        task->data[0] = 0xFF;
        break;
    case 3:
        sprite->x += sprite->x2;
        sprite->y += sprite->y2;
        sprite->x2 = 0;
        sprite->y2 = 0;
        sprite->data[0] = 10;
        sprite->data[1] = sprite->x;
        sprite->data[2] = task->data[6] - ((task->data[10] / 2) + 10) * task->data[5];
        sprite->data[3] = sprite->y;
        sprite->data[4] = task->data[7] - ((task->data[11] / 2) + 10) * task->data[5];
        sprite->data[5] = LeafBladeGetPosFactor(sprite);
        task->data[3] = 2;
        sprite->subpriority = task->data[4];
        StartSpriteAnim(sprite, task->data[3]);
        InitAnimArcTranslation(sprite);
        task->data[0]++;
        break;
    case 5:
        sprite->x += sprite->x2;
        sprite->y += sprite->y2;
        sprite->x2 = 0;
        sprite->y2 = 0;
        sprite->data[0] = 10;
        sprite->data[1] = sprite->x;
        sprite->data[2] = task->data[6] + ((task->data[10] / 2) + 10) * task->data[5];
        sprite->data[3] = sprite->y;
        sprite->data[4] = task->data[7] + ((task->data[11] / 2) + 10) * task->data[5];
        sprite->data[5] = LeafBladeGetPosFactor(sprite);
        task->data[4] -= 2;
        task->data[3] = 3;
        sprite->subpriority = task->data[4];
        StartSpriteAnim(sprite, task->data[3]);
        InitAnimArcTranslation(sprite);
        task->data[0]++;
        break;
    case 6:
        AnimTask_LeafBlade_Step2(task, taskId);
        if (TranslateAnimArc(sprite) == 0)
            break;

        task->data[15] = 7;
        task->data[0] = 0xFF;
        break;
    case 7:
        sprite->x += sprite->x2;
        sprite->y += sprite->y2;
        sprite->x2 = 0;
        sprite->y2 = 0;
        sprite->data[0] = 10;
        sprite->data[1] = sprite->x;
        sprite->data[2] = task->data[6];
        sprite->data[3] = sprite->y;
        sprite->data[4] = task->data[7];
        sprite->data[5] = LeafBladeGetPosFactor(sprite);
        task->data[4] += 2;
        task->data[3] = 4;
        sprite->subpriority = task->data[4];
        StartSpriteAnim(sprite, task->data[3]);
        InitAnimArcTranslation(sprite);
        task->data[0]++;
        break;
    case 9:
        sprite->x += sprite->x2;
        sprite->y += sprite->y2;
        sprite->x2 = 0;
        sprite->y2 = 0;
        sprite->data[0] = 10;
        sprite->data[1] = sprite->x;
        sprite->data[2] = task->data[6] - ((task->data[10] / 2) + 10) * task->data[5];
        sprite->data[3] = sprite->y;
        sprite->data[4] = task->data[7] + ((task->data[11] / 2) + 10) * task->data[5];
        sprite->data[5] = LeafBladeGetPosFactor(sprite);
        task->data[3] = 5;
        sprite->subpriority = task->data[4];
        StartSpriteAnim(sprite, task->data[3]);
        InitAnimArcTranslation(sprite);
        task->data[0]++;
        break;
    case 10:
        AnimTask_LeafBlade_Step2(task, taskId);
        if (TranslateAnimArc(sprite) == 0)
        {
            break;
        }
        else
        {
            task->data[15] = 11;
            task->data[0] = 0xFF;
        }
        break;
    case 11:
    {
        sprite->x += sprite->x2;
        sprite->y += sprite->y2;
        sprite->x2 = 0;
        sprite->y2 = 0;
        sprite->data[0] = 10;
        sprite->data[1] = sprite->x;
        sprite->data[2] = task->data[8];
        sprite->data[3] = sprite->y;
        sprite->data[4] = task->data[9];
        sprite->data[5] = LeafBladeGetPosFactor(sprite);
        task->data[4] -= 2;
        task->data[3] = 6;
        sprite->subpriority = task->data[4];
        StartSpriteAnim(sprite, task->data[3]);
        InitAnimArcTranslation(sprite);
        task->data[0]++;
        break;
    }
    case 12:
        AnimTask_LeafBlade_Step2(task, taskId);
        if (TranslateAnimArc(sprite) != 0)
        {
            DestroySprite(sprite);
            task->data[0]++;
        }
        break;
    case 13:
        if (task->data[12] == 0)
        {
            DestroyAnimVisualTask(taskId);
        }
        break;
    case 0xFF:
        task->data[1]++;
        if (task->data[1] > 5)
        {
            task->data[1] = 0;
            task->data[0] = task->data[15];
        }
        break;
    }
}

static s16 LeafBladeGetPosFactor(struct Sprite *sprite)
{
    s16 var = 8;
    if (sprite->data[4] < sprite->y)
        var = -var;

    return var;
}

static void AnimTask_LeafBlade_Step2(struct Task *task, u8 taskId)
{
    task->data[14]++;
    if (task->data[14] > 0)
    {
        u8 spriteId;
        s16 spriteX;
        s16 spriteY;
        task->data[14] = 0;
        spriteX = gSprites[task->data[2]].x + gSprites[task->data[2]].x2;
        spriteY = gSprites[task->data[2]].y + gSprites[task->data[2]].y2;
        spriteId = CreateSprite(&gLeafBladeSpriteTemplate, spriteX, spriteY, task->data[4]);
        if (spriteId != MAX_SPRITES)
        {
            gSprites[spriteId].data[6] = taskId;
            gSprites[spriteId].data[7] = 12;
            gTasks[taskId].data[12]++;
            gSprites[spriteId].data[0] = task->data[13] & 1;
            gTasks[taskId].data[13]++;
            StartSpriteAnim(&gSprites[spriteId], task->data[3]);
            gSprites[spriteId].subpriority = task->data[4];
            gSprites[spriteId].callback = AnimTask_LeafBlade_Step2_Callback;
        }
    }
}

static void AnimTask_LeafBlade_Step2_Callback(struct Sprite *sprite)
{
    sprite->data[0]++;
    if (sprite->data[0] > 1)
    {
        sprite->data[0] = 0;
        sprite->invisible ^= 1;
        sprite->data[1]++;
        if (sprite->data[1] > 8)
        {
            gTasks[sprite->data[6]].data[sprite->data[7]]--;
            DestroySprite(sprite);
        }
    }
}
