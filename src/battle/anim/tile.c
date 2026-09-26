#include "global.h"
#include "battle_anim.h"
#include "battle_interface.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "sound.h"
#include "constants/battle.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;
extern u8 gBattlersCount;
extern u8 gHealthboxSpriteIds[];

static void AnimConversion(struct Sprite *sprite);
static void AnimConversion2(struct Sprite *sprite);
static void AnimConversion2_Step(struct Sprite *sprite);

// Conversion effects and the Octazooka ball and smoke sprites.

const struct SpriteTemplate gOctazookaBallSpriteTemplate =
{
    .tileTag = ANIM_TAG_BLACK_BALL,
    .paletteTag = ANIM_TAG_BLACK_BALL,
    .oam = &gOamData_AffineOff_ObjNormal_8x8,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = TranslateAnimSpriteToTargetMonLocation,
};

const union AnimCmd gOctazookaAnimCmds[] =
{
    ANIMCMD_FRAME(0, 3),
    ANIMCMD_FRAME(16, 3),
    ANIMCMD_FRAME(32, 3),
    ANIMCMD_FRAME(48, 3),
    ANIMCMD_FRAME(64, 3),
    ANIMCMD_END,
};

const union AnimCmd *const gOctazookaAnimTable[] =
{
    gOctazookaAnimCmds,
};

const struct SpriteTemplate gOctazookaSmokeSpriteTemplate =
{
    .tileTag = ANIM_TAG_GRAY_SMOKE,
    .paletteTag = ANIM_TAG_GRAY_SMOKE,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gOctazookaAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimSpriteOnMonPos,
};

const union AnimCmd gConversionAnimCmds[] =
{
    ANIMCMD_FRAME(3, 5),
    ANIMCMD_FRAME(2, 5),
    ANIMCMD_FRAME(1, 5),
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END,
};

const union AnimCmd *const gConversionAnimTable[] =
{
    gConversionAnimCmds,
};

const union AffineAnimCmd gConversionAffineAnimCmds[] =
{
    AFFINEANIMCMD_FRAME(0x200, 0x200, 0, 0),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd *const gConversionAffineAnimTable[] =
{
    gConversionAffineAnimCmds,
};

const struct SpriteTemplate gConversionSpriteTemplate =
{
    .tileTag = ANIM_TAG_CONVERSION,
    .paletteTag = ANIM_TAG_CONVERSION,
    .oam = &gOamData_AffineDouble_ObjBlend_8x8,
    .anims = gConversionAnimTable,
    .images = NULL,
    .affineAnims = gConversionAffineAnimTable,
    .callback = AnimConversion,
};

const union AnimCmd gConversion2AnimCmds[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_FRAME(1, 5),
    ANIMCMD_FRAME(2, 5),
    ANIMCMD_FRAME(3, 5),
    ANIMCMD_END,
};

const union AnimCmd *const gConversion2AnimTable[] =
{
    gConversion2AnimCmds,
};

const struct SpriteTemplate gConversion2SpriteTemplate =
{
    .tileTag = ANIM_TAG_CONVERSION,
    .paletteTag = ANIM_TAG_CONVERSION,
    .oam = &gOamData_AffineDouble_ObjBlend_8x8,
    .anims = gConversion2AnimTable,
    .images = NULL,
    .affineAnims = gConversionAffineAnimTable,
    .callback = AnimConversion2,
};

static void AnimConversion(struct Sprite *sprite)
{
    if (sprite->data[0] == 0)
    {
        sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_X) + gBattleAnimArgs[0];
        sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_Y) + gBattleAnimArgs[1];
        if (IsContest())
            sprite->y += 10;
        sprite->data[0]++;
    }

    if ((u16)gBattleAnimArgs[7] == 0xFFFF)
        DestroyAnimSprite(sprite);
}

void AnimTask_ConversionAlphaBlend(u8 taskId)
{
    if (gTasks[taskId].data[2] == 1)
    {
        gBattleAnimArgs[7] = 0xFFFF;
        gTasks[taskId].data[2]++;
    }
    else if (gTasks[taskId].data[2] == 2)
    {
        DestroyAnimVisualTask(taskId);
    }
    else
    {
        if (++gTasks[taskId].data[0] == 4)
        {
            gTasks[taskId].data[0] = 0;
            gTasks[taskId].data[1]++;
            REG_BLDALPHA = (gTasks[taskId].data[1] << 8) | (16 - gTasks[taskId].data[1]);
            if (gTasks[taskId].data[1] == 16)
                gTasks[taskId].data[2]++;
        }
    }
}

static void AnimConversion2(struct Sprite *sprite)
{
    InitSpritePosToAnimTarget(sprite, FALSE);
    sprite->animPaused = 1;
    sprite->data[0] = gBattleAnimArgs[2];
    sprite->callback = AnimConversion2_Step;
}

static void AnimConversion2_Step(struct Sprite *sprite)
{
    if (sprite->data[0])
    {
        sprite->data[0]--;
    }
    else
    {
        sprite->animPaused = 0;
        sprite->data[0] = 30;
        sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_X_2);
        sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_Y_PIC_OFFSET);
        sprite->callback = StartAnimLinearTranslation;
        StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
    }
}

void AnimTask_Conversion2AlphaBlend(u8 taskId)
{
    if (++gTasks[taskId].data[0] == 4)
    {
        gTasks[taskId].data[0] = 0;
        gTasks[taskId].data[1]++;
        REG_BLDALPHA = gTasks[taskId].data[1] | ((16 - gTasks[taskId].data[1]) << 8);
        if (gTasks[taskId].data[1] == 16)
            DestroyAnimVisualTask(taskId);
    }
}

static void UNUSED AnimTask_HideBattlersHealthbox(u8 taskId)
{
    u8 i;
    for (i = 0; i < gBattlersCount; i++)
    {
        if (gBattleAnimArgs[0] == TRUE && GetBattlerSide(i) == B_SIDE_PLAYER)
            SetHealthboxSpriteInvisible(gHealthboxSpriteIds[i]);

        if (gBattleAnimArgs[1] == TRUE && GetBattlerSide(i) == B_SIDE_OPPONENT)
            SetHealthboxSpriteInvisible(gHealthboxSpriteIds[i]);
    }

    DestroyAnimVisualTask(taskId);
}

static void UNUSED AnimTask_ShowBattlersHealthbox(u8 taskId)
{
    u8 i;
    for (i = 0; i < gBattlersCount; i++)
    {
        SetHealthboxSpriteVisible(gHealthboxSpriteIds[i]);
    }

    DestroyAnimVisualTask(taskId);
}
