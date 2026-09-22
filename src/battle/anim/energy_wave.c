#include "global.h"
#include "constants/battle.h"
#include "battle_anim.h"
#include "rom_8077ABC.h"
#include "sprite.h"
#include "task.h"
#include "trig.h"

extern s16 gBattleAnimArgs[8];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;
extern const union AffineAnimCmd *const gGrowingRingAffineAnimTable[];

void AnimToTargetInSinWave(struct Sprite *sprite);
static void AnimHydroCannonCharge(struct Sprite *sprite);
static void AnimHydroCannonBeam(struct Sprite *sprite);
static void AnimToTargetInSinWave_Step(struct Sprite *);
static void AnimTask_RunSinAnimTimer(u8);
static void AnimHydroCannonCharge_Step(struct Sprite *);

// energy_wave (animates steady "waves" of energy)
// Used in Hydro Pump, Mud Shot, Signal Beam, Flamethrower, Psywave, and
// Hydro Cannon.

const union AnimCmd sAnim_WaterMudOrb[] =
{
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_FRAME(4, 1),
    ANIMCMD_FRAME(8, 1),
    ANIMCMD_FRAME(12, 1),
    ANIMCMD_JUMP(0),
};

const union AnimCmd *const gAnims_WaterMudOrb[] =
{
    sAnim_WaterMudOrb,
};

const struct SpriteTemplate gHydroPumpOrbSpriteTemplate =
{
    .tileTag = ANIM_TAG_WATER_ORB,
    .paletteTag = ANIM_TAG_WATER_ORB,
    .oam = &gOamData_AffineOff_ObjBlend_16x16,
    .anims = gAnims_WaterMudOrb,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimToTargetInSinWave,
};

const struct SpriteTemplate gMudShotOrbSpriteTemplate =
{
    .tileTag = ANIM_TAG_BROWN_ORB,
    .paletteTag = ANIM_TAG_BROWN_ORB,
    .oam = &gOamData_AffineOff_ObjBlend_16x16,
    .anims = gAnims_WaterMudOrb,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimToTargetInSinWave,
};

const struct SpriteTemplate gSignalBeamRedOrbSpriteTemplate =
{
    .tileTag = ANIM_TAG_GLOWY_RED_ORB,
    .paletteTag = ANIM_TAG_GLOWY_RED_ORB,
    .oam = &gOamData_AffineOff_ObjNormal_8x8,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimToTargetInSinWave,
};

const struct SpriteTemplate gSignalBeamGreenOrbSpriteTemplate =
{
    .tileTag = ANIM_TAG_GLOWY_GREEN_ORB,
    .paletteTag = ANIM_TAG_GLOWY_GREEN_ORB,
    .oam = &gOamData_AffineOff_ObjNormal_8x8,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimToTargetInSinWave,
};

static const union AnimCmd sAnim_FlamethrowerFlame[] =
{
    ANIMCMD_FRAME(16, 2),
    ANIMCMD_FRAME(32, 2),
    ANIMCMD_FRAME(48, 2),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd *const sAnims_FlamethrowerFlame[] =
{
    sAnim_FlamethrowerFlame,
};

const struct SpriteTemplate gFlamethrowerFlameSpriteTemplate =
{
    .tileTag = ANIM_TAG_SMALL_EMBER,
    .paletteTag = ANIM_TAG_SMALL_EMBER,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = sAnims_FlamethrowerFlame,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimToTargetInSinWave,
};

const struct SpriteTemplate gPsywaveRingSpriteTemplate =
{
    .tileTag = ANIM_TAG_BLUE_RING,
    .paletteTag = ANIM_TAG_BLUE_RING,
    .oam = &gOamData_837E034,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gGrowingRingAffineAnimTable,
    .callback = AnimToTargetInSinWave,
};

static const union AffineAnimCmd sAffineAnim_HydroCannonCharge[] =
{
    AFFINEANIMCMD_FRAME(0x3, 0x3, 10, 50),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 0, 10),
    AFFINEANIMCMD_FRAME(0xFFEC, 0xFFEC, -10, 20),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_HydroCannonBeam[] =
{
    AFFINEANIMCMD_FRAME(0x150, 0x150, 0, 0),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd *const sAffineAnims_HydroCannonCharge[] =
{
    sAffineAnim_HydroCannonCharge,
};

static const union AffineAnimCmd *const sAffineAnims_HydroCannonBeam[] =
{
    sAffineAnim_HydroCannonBeam,
};

const struct SpriteTemplate gHydroCannonChargeSpriteTemplate =
{
    .tileTag = ANIM_TAG_WATER_ORB,
    .paletteTag = ANIM_TAG_WATER_ORB,
    .oam = &gOamData_AffineDouble_ObjBlend_16x16,
    .anims = gAnims_WaterMudOrb,
    .images = NULL,
    .affineAnims = sAffineAnims_HydroCannonCharge,
    .callback = AnimHydroCannonCharge,
};

const struct SpriteTemplate gHydroCannonBeamSpriteTemplate =
{
    .tileTag = ANIM_TAG_WATER_ORB,
    .paletteTag = ANIM_TAG_WATER_ORB,
    .oam = &gOamData_AffineDouble_ObjBlend_16x16,
    .anims = gAnims_WaterMudOrb,
    .images = NULL,
    .affineAnims = sAffineAnims_HydroCannonBeam,
    .callback = AnimHydroCannonBeam,
};

void AnimToTargetInSinWave(struct Sprite *sprite)
{
    InitSpritePosToAnimAttacker(sprite, 1);

    sprite->data[0] = 30;
    sprite->data[1] = sprite->x;
    sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimTarget, 2);
    sprite->data[3] = sprite->y;
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimTarget, 3);

    InitAnimLinearTranslation(sprite);

    sprite->data[5] = 0xD200 / sprite->data[0];
    sprite->data[7] = gBattleAnimArgs[3];

    if (gBattleAnimArgs[7] > 127)
    {
        sprite->data[6] = (gBattleAnimArgs[7] - 127) << 8;
        sprite->data[7] = -sprite->data[7];
    }
    else
    {
        sprite->data[6] = gBattleAnimArgs[7] << 8;
    }

    sprite->callback = AnimToTargetInSinWave_Step;
    sprite->callback(sprite);
}

static void AnimToTargetInSinWave_Step(struct Sprite *sprite)
{
    if (AnimTranslateLinear(sprite))
    {
        DestroyAnimSprite(sprite);
    }

    sprite->y2 += Sin(sprite->data[6] >> 8, sprite->data[7]);
    if (((sprite->data[6] + sprite->data[5]) >> 8) > 127)
    {
        sprite->data[6] = 0;
        sprite->data[7] = -sprite->data[7];
    }
    else
    {
        sprite->data[6] += sprite->data[5];
    }
}

void AnimTask_StartSinAnimTimer(u8 taskId)
{
    gTasks[taskId].data[0] = gBattleAnimArgs[0];
    gBattleAnimArgs[7] = 0;
    gTasks[taskId].func = AnimTask_RunSinAnimTimer;
}

static void AnimTask_RunSinAnimTimer(u8 taskId)
{
    gBattleAnimArgs[7] = (gBattleAnimArgs[7] + 3) & 0xFF;
    if (--gTasks[taskId].data[0] == 0)
    {
        DestroyAnimVisualTask(taskId);
    }
}

static void AnimHydroCannonCharge(struct Sprite *sprite)
{
    u8 priority;

    sprite->x = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_X);
    sprite->y = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_Y);
    sprite->y2 = -10;

    priority = GetBattlerSpriteSubpriority(gBattleAnimAttacker);

    if (!IsContest())
    {
        if (GetBattlerSide(gBattleAnimAttacker) == B_SIDE_PLAYER)
        {
            sprite->x2 = 10;
            sprite->subpriority = priority + 2;
        }
        else
        {
            sprite->x2 = -10;
            sprite->subpriority = priority - 2;
        }
    }
    else
    {
        sprite->x2 = -10;
        sprite->subpriority = priority + 2;
    }

    sprite->callback = AnimHydroCannonCharge_Step;
}

static void AnimHydroCannonCharge_Step(struct Sprite *sprite)
{
    if (sprite->affineAnimEnded)
        DestroyAnimSprite(sprite);
}

static void AnimHydroCannonBeam(struct Sprite *sprite)
{
    bool8 respectMonPicOffsets;
    u8 coordType;

    if (GetBattlerSide(gBattleAnimAttacker) == GetBattlerSide(gBattleAnimTarget))
    {
        gBattleAnimArgs[0] *= -1;

        if (GetBattlerPosition(gBattleAnimAttacker) == B_POSITION_PLAYER_LEFT || GetBattlerPosition(gBattleAnimAttacker) == B_POSITION_OPPONENT_LEFT)
            gBattleAnimArgs[0] *= -1;
    }

    if ((gBattleAnimArgs[5] & 0xFF00) == 0)
        respectMonPicOffsets = TRUE;
    else
        respectMonPicOffsets = FALSE;

    if ((u8)gBattleAnimArgs[5] == 0)
        coordType = BATTLER_COORD_Y_PIC_OFFSET;
    else
        coordType = BATTLER_COORD_Y;

    InitSpritePosToAnimAttacker(sprite, respectMonPicOffsets);

    if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
        gBattleAnimArgs[2] = -gBattleAnimArgs[2];

    sprite->data[0] = gBattleAnimArgs[4];
    sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_X_2) + gBattleAnimArgs[2];
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimTarget, coordType) + gBattleAnimArgs[3];
    sprite->callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}
