#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimSporeParticle(struct Sprite* sprite);
void sub_80CAD54(struct Sprite* sprite);
void sub_80CAE20(struct Sprite* sprite);
static void AnimSporeParticle_Step(struct Sprite* sprite);
static void sub_80CADA8(struct Sprite* sprite);
static void sub_80CAE74(struct Sprite* sprite);

const union AnimCmd gSporeParticleAnimCmds1[] =
{
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END,
};

const union AnimCmd gSporeParticleAnimCmds2[] =
{
    ANIMCMD_FRAME(4, 7),
    ANIMCMD_END,
};

const union AnimCmd *const gSporeParticleAnimTable[] =
{
    gSporeParticleAnimCmds1,
    gSporeParticleAnimCmds2,
};

const struct SpriteTemplate gSporeParticleSpriteTemplate =
{
    .tileTag = ANIM_TAG_SPORE,
    .paletteTag = ANIM_TAG_SPORE,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = gSporeParticleAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimSporeParticle,
};

const union AnimCmd gPetalDanceBigFlowerAnimCmds[] =
{
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END,
};

const union AnimCmd gPetalDanceSmallFlowerAnimCmds[] =
{
    ANIMCMD_FRAME(4, 1),
    ANIMCMD_END,
};

const union AnimCmd *const gPetalDanceBigFlowerAnimTable[] =
{
    gPetalDanceBigFlowerAnimCmds,
};

const union AnimCmd *const gPetalDanceSmallFlowerAnimTable[] =
{
    gPetalDanceSmallFlowerAnimCmds,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83D6428 =
{
    .tileTag = ANIM_TAG_FLOWER,
    .paletteTag = ANIM_TAG_FLOWER,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = gPetalDanceBigFlowerAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80CAD54,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83D6440 =
{
    .tileTag = ANIM_TAG_FLOWER,
    .paletteTag = ANIM_TAG_FLOWER,
    .oam = &gOamData_AffineOff_ObjNormal_8x8,
    .anims = gPetalDanceSmallFlowerAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80CAE20,
};

// Moves a spore particle in a halo around the target mon.
// The sprite's priority is updated to give the effect of going
// behind the mon's sprite.
static void AnimSporeParticle(struct Sprite* sprite)
{
    sub_8078764(sprite, TRUE);
    StartSpriteAnim(sprite, gBattleAnimArgs[4]);
    if (gBattleAnimArgs[4] == 1)
    {
        sprite->oam.objMode = 1;
    }

    sprite->data[0] = gBattleAnimArgs[3];
    sprite->data[1] = gBattleAnimArgs[2];
    sprite->callback = AnimSporeParticle_Step;
    AnimSporeParticle_Step(sprite);
}

static void AnimSporeParticle_Step(struct Sprite* sprite)
{
    u8 var1;
    
    sprite->x2 = Sin(sprite->data[1], 32);
    sprite->y2 = Cos(sprite->data[1], -3) + ((sprite->data[2] += 24) >> 8);
    if ((u16)(sprite->data[1] - 0x40) < 0x80)
    {
        sprite->oam.priority = (GetBattlerSpriteBGPriority(gBattleAnimTarget) & 3);
    }
    else
    {
        var1 = GetBattlerSpriteBGPriority(gBattleAnimTarget) + 1;
        if (var1 > 3)
        {
            var1 = 3;
        }

        sprite->oam.priority = var1;
    }

    sprite->data[1] = (sprite->data[1] + 2) & 0xFF;
    sprite->data[0]--;
    if (sprite->data[0] == -1)
        DestroyAnimSprite(sprite);
}

void AnimTask_SporeDoubleBattle(u8 taskId)
{
    if (IsContest() || !IsDoubleBattle())
    {
        DestroyAnimVisualTask(taskId);
    }
    else
    {
        if (GetBattlerSpriteBGPriorityRank(gBattleAnimTarget) == 1)
        {
            REG_BG2CNT_BITFIELD.priority = 3;
        }
        else
        {
            REG_BG1CNT_BITFIELD.priority = 1;
        }

        DestroyAnimVisualTask(taskId);
    }
}

void sub_80CAD54(struct Sprite* sprite)
{
    InitSpritePosToAnimAttacker(sprite, 0);
    sprite->data[0] = gBattleAnimArgs[3];
    sprite->data[1] = sprite->x;
    sprite->data[2] = sprite->x;
    sprite->data[3] = sprite->y;
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimAttacker, 3) + gBattleAnimArgs[2];
    InitAnimLinearTranslation(sprite);
    sprite->data[5] = 0x40;
    sprite->callback = sub_80CADA8;
    sub_80CADA8(sprite);
}

static void sub_80CADA8(struct Sprite* sprite)
{
    if (!AnimTranslateLinear(sprite))
    {
        sprite->x2 += Sin(sprite->data[5], 32);
        sprite->y2 += Cos(sprite->data[5], -5);
        if ((u16)(sprite->data[5] - 0x40) < 0x80)
        {
            sprite->subpriority = GetBattlerSpriteSubpriority(gBattleAnimAttacker) - 1;
        }
        else
        {
            sprite->subpriority = GetBattlerSpriteSubpriority(gBattleAnimAttacker) + 1;
        }

        sprite->data[5] = (sprite->data[5] + 5) & 0xFF;
    }
    else
    {
        DestroyAnimSprite(sprite);
    }
}

void sub_80CAE20(struct Sprite* sprite)
{
    InitSpritePosToAnimAttacker(sprite, 1);
    sprite->data[0] = gBattleAnimArgs[3];
    sprite->data[1] = sprite->x;
    sprite->data[2] = sprite->x;
    sprite->data[3] = sprite->y;
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimAttacker, 3) + gBattleAnimArgs[2];
    InitAnimLinearTranslation(sprite);
    sprite->data[5] = 0x40;
    sprite->callback = sub_80CAE74;
    sub_80CAE74(sprite);
}

static void sub_80CAE74(struct Sprite* sprite)
{
    if (!AnimTranslateLinear(sprite))
    {
        sprite->x2 += Sin(sprite->data[5], 8);
        if ((u16)(sprite->data[5] - 0x3B) < 5 || (u16)(sprite->data[5] - 0xBB) < 5)
        {
            sprite->oam.matrixNum ^= 8;
        }

        sprite->data[5] = (sprite->data[5] + 5) & 0xFF;
    }
    else
    {
       DestroyAnimSprite(sprite);
    }
}
