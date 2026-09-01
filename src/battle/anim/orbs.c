#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"
#include "random.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

static void AnimPowerAbsorptionOrb(struct Sprite *sprite);
static void AnimSolarBeamBigOrb(struct Sprite *sprite);
static void AnimSolarBeamSmallOrb(struct Sprite *sprite);
void AnimAbsorptionOrb(struct Sprite* sprite);
void sub_80CAA14(struct Sprite* sprite);
static void AnimSolarBeamSmallOrb_Step(struct Sprite *sprite);
static void AnimAbsorptionOrb_Step(struct Sprite* sprite);
static void sub_80CAACC(struct Sprite* sprite);

const union AnimCmd gSolarBeamBigOrbAnimCmds1[] =
{
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END,
};

const union AnimCmd gSolarBeamBigOrbAnimCmds2[] =
{
    ANIMCMD_FRAME(1, 1),
    ANIMCMD_END,
};

const union AnimCmd gSolarBeamBigOrbAnimCmds3[] =
{
    ANIMCMD_FRAME(2, 1),
    ANIMCMD_END,
};

const union AnimCmd gSolarBeamBigOrbAnimCmds4[] =
{
    ANIMCMD_FRAME(3, 1),
    ANIMCMD_END,
};

const union AnimCmd gSolarBeamBigOrbAnimCmds5[] =
{
    ANIMCMD_FRAME(4, 1),
    ANIMCMD_END,
};

const union AnimCmd gSolarBeamBigOrbAnimCmds6[] =
{
    ANIMCMD_FRAME(5, 1),
    ANIMCMD_END,
};

const union AnimCmd gSolarBeamBigOrbAnimCmds7[] =
{
    ANIMCMD_FRAME(6, 1),
    ANIMCMD_END,
};

const union AnimCmd gSolarBeamSmallOrbAnimCms[] =
{
    ANIMCMD_FRAME(7, 1),
    ANIMCMD_END,
};

const union AnimCmd gPowerAbsorptionOrbAnimCmds[] =
{
    ANIMCMD_FRAME(8, 1),
    ANIMCMD_END,
};

const union AnimCmd *const gSolarBeamBigOrbAnimTable[] =
{
    gSolarBeamBigOrbAnimCmds1,
    gSolarBeamBigOrbAnimCmds2,
    gSolarBeamBigOrbAnimCmds3,
    gSolarBeamBigOrbAnimCmds4,
    gSolarBeamBigOrbAnimCmds5,
    gSolarBeamBigOrbAnimCmds6,
    gSolarBeamBigOrbAnimCmds7,
};

const union AnimCmd *const gSolarBeamSmallOrbAnimTable[] =
{
    gSolarBeamSmallOrbAnimCms,
};

const union AnimCmd *const gPowerAbsorptionOrbAnimTable[] =
{
    gPowerAbsorptionOrbAnimCmds,
};

const union AffineAnimCmd gPowerAbsorptionOrbAffineAnimCmds[] = {
    AFFINEANIMCMD_FRAME(-5, -5, 0, 1),
    AFFINEANIMCMD_JUMP(0),
};

const union AffineAnimCmd *const gPowerAbsorptionOrbAffineAnimTable[] = {
    gPowerAbsorptionOrbAffineAnimCmds,
};

const struct SpriteTemplate gPowerAbsorptionOrbSpriteTemplate =
{
    .tileTag = ANIM_TAG_ORBS,
    .paletteTag = ANIM_TAG_ORBS,
    .oam = &gOamData_AffineNormal_ObjBlend_16x16,
    .anims = gPowerAbsorptionOrbAnimTable,
    .images = NULL,
    .affineAnims = gPowerAbsorptionOrbAffineAnimTable,
    .callback = AnimPowerAbsorptionOrb,
};

const struct SpriteTemplate gSolarBeamBigOrbSpriteTemplate =
{
    .tileTag = ANIM_TAG_ORBS,
    .paletteTag = ANIM_TAG_ORBS,
    .oam = &gOamData_AffineOff_ObjNormal_8x8,
    .anims = gSolarBeamBigOrbAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimSolarBeamBigOrb,
};

const struct SpriteTemplate gSolarBeamSmallOrbSpriteTemplate =
{
    .tileTag = ANIM_TAG_ORBS,
    .paletteTag = ANIM_TAG_ORBS,
    .oam = &gOamData_AffineOff_ObjNormal_8x8,
    .anims = gSolarBeamSmallOrbAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimSolarBeamSmallOrb,
};

const union AffineAnimCmd gSpriteAffineAnim_83D6334[] = {
    AFFINEANIMCMD_FRAME(320, 320, 0, 0),
    AFFINEANIMCMD_FRAME(-14, -14, 0, 1),
    AFFINEANIMCMD_JUMP(1),
};

const union AffineAnimCmd *const gSpriteAffineAnimTable_83D634C[] = {
    gSpriteAffineAnim_83D6334,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83D6350 =
{
    .tileTag = ANIM_TAG_GRAY_ORB,
    .paletteTag = ANIM_TAG_GRAY_ORB,
    .oam = &gOamData_837DFE4,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gSpriteAffineAnimTable_83D634C,
    .callback = AnimPowerAbsorptionOrb,
};

const union AffineAnimCmd gAbsorptionOrbAffineAnimCmds[] = {
    AFFINEANIMCMD_FRAME(-5, -5, 0, 1),
    AFFINEANIMCMD_JUMP(0),
};

const union AffineAnimCmd *const gAbsorptionOrbAffineAnimTable[] = {
    gAbsorptionOrbAffineAnimCmds,
};

const struct SpriteTemplate gAbsorptionOrbSpriteTemplate =
{
    .tileTag = ANIM_TAG_ORBS,
    .paletteTag = ANIM_TAG_ORBS,
    .oam = &gOamData_AffineNormal_ObjBlend_16x16,
    .anims = gPowerAbsorptionOrbAnimTable,
    .images = NULL,
    .affineAnims = gAbsorptionOrbAffineAnimTable,
    .callback = AnimAbsorptionOrb,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83D6394 =
{
    .tileTag = ANIM_TAG_ORBS,
    .paletteTag = ANIM_TAG_ORBS,
    .oam = &gOamData_AffineOff_ObjNormal_8x8,
    .anims = gSolarBeamBigOrbAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80CAA14,
};


// orbs
// Used by Solar Beam, Absorb, Hyper Beam, and Leech Seed.

static void AnimPowerAbsorptionOrb(struct Sprite *sprite)
{
    InitSpritePosToAnimAttacker(sprite, 1);
    sprite->data[0] = gBattleAnimArgs[2];
    sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimAttacker, 2);
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimAttacker, 3);
    sprite->callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(sprite, DestroySpriteAndMatrix);
}

static void AnimSolarBeamBigOrb(struct Sprite *sprite)
{
    InitSpritePosToAnimAttacker(sprite, 1);
    StartSpriteAnim(sprite, gBattleAnimArgs[3]);
    sprite->data[0] = gBattleAnimArgs[2];
    sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimTarget, 2);
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimTarget, 3);
    sprite->callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}

static void AnimSolarBeamSmallOrb(struct Sprite *sprite)
{
    InitSpritePosToAnimAttacker(sprite, 1);
    sprite->data[0] = gBattleAnimArgs[2];
    sprite->data[1] = sprite->x;
    sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimTarget, 2);
    sprite->data[3] = sprite->y;
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimTarget, 3);
    InitAnimLinearTranslation(sprite);
    sprite->data[5] = gBattleAnimArgs[3];
    sprite->callback = AnimSolarBeamSmallOrb_Step;
    AnimSolarBeamSmallOrb_Step(sprite);
}

static void AnimSolarBeamSmallOrb_Step(struct Sprite *sprite)
{
    if (AnimTranslateLinear(sprite))
    {
        DestroySprite(sprite);
    }
    else
    {
        if (sprite->data[5] > 0x7F)
        {
            sprite->subpriority = GetBattlerSpriteSubpriority(gBattleAnimTarget) + 1;
        }
        else
        {
            sprite->subpriority = GetBattlerSpriteSubpriority(gBattleAnimTarget) + 6;
        }
        sprite->x2 += Sin(sprite->data[5], 5);
        sprite->y2 += Cos(sprite->data[5], 14);
        sprite->data[5] = (sprite->data[5] + 15) & 0xFF;
    }
}

void AnimTask_CreateSmallSolarBeamOrbs(u8 taskId)
{
    gTasks[taskId].data[0]--;
    if (gTasks[taskId].data[0] == -1)
    {
        gTasks[taskId].data[1]++;
        gTasks[taskId].data[0] = 6;
        gBattleAnimArgs[0] = 15;
        gBattleAnimArgs[1] = 0;
        gBattleAnimArgs[2] = 80;
        gBattleAnimArgs[3] = 0;
        CreateSpriteAndAnimate(&gSolarBeamSmallOrbSpriteTemplate, 0, 0, GetBattlerSpriteSubpriority(gBattleAnimTarget) + 1);
    }

    if (gTasks[taskId].data[1] == 15)
        DestroyAnimVisualTask(taskId);
}

void AnimAbsorptionOrb(struct Sprite* sprite)
{
    sub_8078764(sprite, TRUE);
    sprite->data[0] = gBattleAnimArgs[3];
    sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimAttacker, 2);
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimAttacker, 3);
    sprite->data[5] = gBattleAnimArgs[2];
    InitAnimArcTranslation(sprite);
    sprite->callback = AnimAbsorptionOrb_Step;
}

static void AnimAbsorptionOrb_Step(struct Sprite* sprite)
{
    if (TranslateAnimArc(sprite))
        DestroyAnimSprite(sprite);
}

void sub_80CAA14(struct Sprite* sprite)
{
    u16 a = Random();
    u16 b;
    
    StartSpriteAnim(sprite, a & 7);
    sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, 2);
    sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, 3);
    if (GetBattlerSide(gBattleAnimAttacker))
    {
        sprite->x -= 20;
    }
    else
    {
        sprite->x += 20;
    }

    b = Random();
    sprite->data[0] = (b & 31) + 64;
    sprite->data[1] = sprite->x;
    sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimTarget, 2);
    sprite->data[3] = sprite->y;
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimTarget, 3);
    sub_8078D60(sprite);
    sprite->data[5] = Random() & 0xFF;
    sprite->data[6] = sprite->subpriority;
    sprite->callback = sub_80CAACC;
    sub_80CAACC(sprite);
}

static void sub_80CAACC(struct Sprite* sprite)
{
    if (sub_8078CE8(sprite))
    {
        DestroyAnimSprite(sprite);
    }
    else
    {
        sprite->y2 += Cos(sprite->data[5], 12);
        if (sprite->data[5] <= 0x7E)
        {
            sprite->subpriority = sprite->data[6];
        }
        else
        {
            sprite->subpriority = sprite->data[6] + 1;
        }

        sprite->data[5] = (sprite->data[5] + 24) & 0xFF;
    }
}
