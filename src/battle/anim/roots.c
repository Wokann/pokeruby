#include "global.h"
#include "rom_8077ABC.h"
#include "trig.h"
#include "battle_anim.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;

struct FrenzyPlantRootData
{
    s16 startX;
    s16 startY;
    s16 targetX;
    s16 targetY;
};

BSS_DATA u32 filler_03000724;
BSS_DATA struct FrenzyPlantRootData sFrenzyPlantRootData;
BSS_DATA u16 gUnusedOverheatData[6];
BSS_DATA u32 filler_0300073c;

static void AnimIngrainRoot(struct Sprite* sprite);
static void AnimFrenzyPlantRoot(struct Sprite *sprite);
static void AnimRootFlickerOut(struct Sprite* sprite);

// roots
// Used by Ingrain and Frenzy Plant.

const union AnimCmd gIngrainRootAnimCmds1[] =
{
    ANIMCMD_FRAME(0, 7),
    ANIMCMD_FRAME(16, 7),
    ANIMCMD_FRAME(32, 7),
    ANIMCMD_FRAME(48, 7),
    ANIMCMD_END,
};

const union AnimCmd gIngrainRootAnimCmds2[] =
{
    ANIMCMD_FRAME(0, 7, .hFlip = TRUE),
    ANIMCMD_FRAME(16, 7, .hFlip = TRUE),
    ANIMCMD_FRAME(32, 7, .hFlip = TRUE),
    ANIMCMD_FRAME(48, 7, .hFlip = TRUE),
    ANIMCMD_END,
};

const union AnimCmd gIngrainRootAnimCmds3[] =
{
    ANIMCMD_FRAME(0, 7),
    ANIMCMD_FRAME(16, 7),
    ANIMCMD_FRAME(32, 7),
    ANIMCMD_END,
};

const union AnimCmd gIngrainRootAnimCmds4[] =
{
    ANIMCMD_FRAME(0, 7, .hFlip = TRUE),
    ANIMCMD_FRAME(16, 7, .hFlip = TRUE),
    ANIMCMD_FRAME(32, 7, .hFlip = TRUE),
    ANIMCMD_END,
};

const union AnimCmd *const gIngrainRootAnimTable[] =
{
    gIngrainRootAnimCmds1,
    gIngrainRootAnimCmds2,
    gIngrainRootAnimCmds3,
    gIngrainRootAnimCmds4,
};

const struct SpriteTemplate gIngrainRootSpriteTemplate =
{
    .tileTag = ANIM_TAG_ROOTS,
    .paletteTag = ANIM_TAG_ROOTS,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gIngrainRootAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimIngrainRoot,
};

const struct SpriteTemplate gFrenzyPlantRootSpriteTemplate =
{
    .tileTag = ANIM_TAG_ROOTS,
    .paletteTag = ANIM_TAG_ROOTS,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = gIngrainRootAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimFrenzyPlantRoot,
};

static void AnimIngrainRoot(struct Sprite* sprite)
{
    if (!sprite->data[0])
    {
        sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, 2);
        sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, 1);
        sprite->x2 = gBattleAnimArgs[0];
        sprite->y2 = gBattleAnimArgs[1];
        sprite->subpriority = gBattleAnimArgs[2] + 30;
        StartSpriteAnim(sprite, gBattleAnimArgs[3]);
        sprite->data[2] = gBattleAnimArgs[4];
        sprite->data[0]++;
        if ((sprite->y + sprite->y2) > 120)
        {
            sprite->y += -120 + (sprite->y2 + sprite->y);
        }
    }
    sprite->callback = AnimRootFlickerOut;
}

static void AnimFrenzyPlantRoot(struct Sprite *sprite)
{
    s16 attackerX = GetBattlerSpriteCoord(gBattleAnimAttacker, 2);
    s16 attackerY = GetBattlerSpriteCoord(gBattleAnimAttacker, 3);
    s16 targetX = GetBattlerSpriteCoord(gBattleAnimTarget, 2);
    s16 targetY = GetBattlerSpriteCoord(gBattleAnimTarget, 3);

    targetX -= attackerX;
    targetY -= attackerY;
    sprite->x = attackerX + targetX * gBattleAnimArgs[0] / 100;
    sprite->y = attackerY + targetY * gBattleAnimArgs[0] / 100;
    sprite->x2 = gBattleAnimArgs[1];
    sprite->y2 = gBattleAnimArgs[2];
    sprite->subpriority = gBattleAnimArgs[3] + 30;
    StartSpriteAnim(sprite, gBattleAnimArgs[4]);
    sprite->data[2] = gBattleAnimArgs[5];
    sprite->callback = AnimRootFlickerOut;
    sFrenzyPlantRootData.startX = sprite->x;
    sFrenzyPlantRootData.startY = sprite->y;
    sFrenzyPlantRootData.targetX = targetX;
    sFrenzyPlantRootData.targetY = targetY;
}

static void AnimRootFlickerOut(struct Sprite* sprite)
{
    if (++sprite->data[0] > (sprite->data[2] - 10))
        sprite->invisible = sprite->data[0] % 2;

    if (sprite->data[0] > sprite->data[2])
        DestroyAnimSprite(sprite);
}
