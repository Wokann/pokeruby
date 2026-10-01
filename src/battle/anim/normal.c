#include "global.h"
#include "battle.h"
#include "battle_anim_internal.h"
#include "blend_palette.h"
#include "decompress.h"
#include "ewram.h"
#include "palette.h"
#include "random.h"
#include "battle_anim.h"
#include "sound.h"
#include "task.h"
#include "trig.h"
#include "constants/battle.h"
#include "constants/rgb.h"
#include "constants/songs.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;
extern u8 gHealthboxSpriteIds[];
extern u8 gBattlersCount;
extern u8 gBattlerSpriteIds[];
extern u8 gBattleEnvironment;
extern u16 gBattlerPartyIndexes[];
extern u8 gBattlerTarget;
extern u8 gEffectBattler;
extern u8 gBattlerAttacker;
extern u8 gAnimVisualTaskCount;

extern const u8 gBattleAnimMaskImage_Curse[];
extern const u8 gBattleAnimMaskTilemap_Curse[];
extern const u8 gStatAnim_Increase_Tilemap[];
extern const u8 gStatAnim_Decrease_Tilemap[];
extern const u8 gStatAnim_Gfx[];
extern const u16 gStatAnim_Defense_Pal[];
extern const u16 gStatAnim_Attack_Pal[];
extern const u16 gStatAnim_Accuracy_Pal[];
extern const u16 gStatAnim_Speed_Pal[];
extern const u16 gStatAnim_Multiple_Pal[];
extern const u16 gStatAnim_Evasion_Pal[];
extern const u16 gStatAnim_SpAttack_Pal[];
extern const u16 gStatAnim_SpDefense_Pal[];

static void AnimConfusionDuck(struct Sprite *sprite);
static void AnimSimplePaletteBlend(struct Sprite *sprite);
static void AnimComplexPaletteBlend(struct Sprite *sprite);
static void AnimCirclingSparkle(struct Sprite *sprite);
static void AnimShakeMonOrBattlePlatforms(struct Sprite *sprite);
static void AnimHitSplatBasic(struct Sprite *sprite);
static void AnimHitSplatHandleInvert(struct Sprite *sprite);
static void AnimHitSplatRandom(struct Sprite *sprite);
static void AnimHitSplatOnMonEdge(struct Sprite *sprite);
static void AnimCrossImpact(struct Sprite *sprite);
static void AnimFlashingHitSplat(struct Sprite *sprite);
static void AnimHitSplatPersistent(struct Sprite *sprite);
static void AnimConfusionDuck_Step(struct Sprite *sprite);
u32 UnpackSelectedBattlePalettes(s16 selector);
static void AnimSimplePaletteBlend_Step(struct Sprite *sprite);
static void AnimComplexPaletteBlend_Step1(struct Sprite *sprite);
static void AnimComplexPaletteBlend_Step2(struct Sprite *sprite);
static void BlendColorCycle(u8, u8, u8);
static void AnimTask_BlendColorCycleLoop(u8 taskId);
static void BlendColorCycleByTag(u8, u8, u8);
static void AnimTask_BlendColorCycleByTagLoop(u8 taskId);
static void AnimTask_FlashAnimTagWithColor_Step1(u8 taskId);
static void AnimTask_FlashAnimTagWithColor_Step2(u8 taskId);
static void AnimShakeMonOrBattlePlatforms_UpdateCoordOffsetEnabled(void);
static void AnimShakeMonOrBattlePlatforms_Step(struct Sprite *sprite);
static void AnimTask_ShakeBattlePlatforms_Step(u8 taskId);
static void AnimFlashingHitSplat_Step(struct Sprite *sprite);
static void StartBlendAnimSpriteColor(u8 taskId, u32 selectedPalettes);
static void AnimTask_BlendSpriteColor_Step2(u8 taskId);
static void AnimTask_HardwarePaletteFade_Step(u8 taskId);
static void AnimTask_TraceMonBlended_Step(u8 taskId);
static void AnimMonTrace(struct Sprite *sprite);
static void AnimTask_DrawFallingWhiteLinesOnAttacker_Step(u8 taskId);
static void StatsChangeAnimation_Setup(u8 taskId);
static void StatsChangeAnimation_Step3(u8 taskId);
static void AnimTask_Flash_Step(u8 taskId);
static void SetPalettesToColor(u32, u16);
static void AnimTask_UpdateSlidingBg(u8 taskId);
static void UpdateMonScrollingBgMask(u8 taskId);
static void AnimTask_WaitAndRestoreVisibility(u8 taskId);

static const union AnimCmd sAnim_ConfusionDuck_0[] =
{
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_FRAME(4, 8),
    ANIMCMD_FRAME(0, 8, .hFlip = TRUE),
    ANIMCMD_FRAME(8, 8),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sAnim_ConfusionDuck_1[] =
{
    ANIMCMD_FRAME(0, 8, .hFlip = TRUE),
    ANIMCMD_FRAME(4, 8),
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_FRAME(8, 8),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd *const sAnims_ConfusionDuck[] =
{
    sAnim_ConfusionDuck_0,
    sAnim_ConfusionDuck_1,
};

const struct SpriteTemplate gConfusionDuckSpriteTemplate =
{
    .tileTag = ANIM_TAG_DUCK,
    .paletteTag = ANIM_TAG_DUCK,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = sAnims_ConfusionDuck,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimConfusionDuck,
};

const struct SpriteTemplate gSimplePaletteBlendSpriteTemplate =
{
    .tileTag = 0,
    .paletteTag = 0,
    .oam = &gDummyOamData,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimSimplePaletteBlend,
};

const struct SpriteTemplate gComplexPaletteBlendSpriteTemplate =
{
    .tileTag = 0,
    .paletteTag = 0,
    .oam = &gDummyOamData,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimComplexPaletteBlend,
};

static const union AnimCmd sAnim_CirclingSparkle[] =
{
    ANIMCMD_FRAME(0, 3),
    ANIMCMD_FRAME(16, 3),
    ANIMCMD_FRAME(32, 3),
    ANIMCMD_FRAME(48, 3),
    ANIMCMD_FRAME(64, 3),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd *const sAnims_CirclingSparkle[] =
{
    sAnim_CirclingSparkle,
};

static const struct SpriteTemplate sCirclingSparkleSpriteTemplate =
{
    .tileTag = ANIM_TAG_SPARKLE_4,
    .paletteTag = ANIM_TAG_SPARKLE_4,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = sAnims_CirclingSparkle,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimCirclingSparkle,
};

const struct SpriteTemplate gShakeMonOrPlatformSpriteTemplate =
{
    .tileTag = 0,
    .paletteTag = 0,
    .oam = &gDummyOamData,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimShakeMonOrBattlePlatforms,
};

static const union AffineAnimCmd sAffineAnim_HitSplat_0[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, 0, 8),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_HitSplat_1[] =
{
    AFFINEANIMCMD_FRAME(0xD8, 0xD8, 0, 0),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 0, 8),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_HitSplat_2[] =
{
    AFFINEANIMCMD_FRAME(0xB0, 0xB0, 0, 0),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 0, 8),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_HitSplat_3[] =
{
    AFFINEANIMCMD_FRAME(0x80, 0x80, 0, 0),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 0, 8),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd *const sAffineAnims_HitSplat[] =
{
    sAffineAnim_HitSplat_0,
    sAffineAnim_HitSplat_1,
    sAffineAnim_HitSplat_2,
    sAffineAnim_HitSplat_3,
};

const struct SpriteTemplate gBasicHitSplatSpriteTemplate =
{
    .tileTag = ANIM_TAG_IMPACT,
    .paletteTag = ANIM_TAG_IMPACT,
    .oam = &gOamData_AffineNormal_ObjBlend_32x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = sAffineAnims_HitSplat,
    .callback = AnimHitSplatBasic,
};

const struct SpriteTemplate gHandleInvertHitSplatSpriteTemplate =
{
    .tileTag = ANIM_TAG_IMPACT,
    .paletteTag = ANIM_TAG_IMPACT,
    .oam = &gOamData_AffineNormal_ObjBlend_32x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = sAffineAnims_HitSplat,
    .callback = AnimHitSplatHandleInvert,
};

const struct SpriteTemplate gWaterHitSplatSpriteTemplate =
{
    .tileTag = ANIM_TAG_WATER_IMPACT,
    .paletteTag = ANIM_TAG_WATER_IMPACT,
    .oam = &gOamData_AffineNormal_ObjBlend_32x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = sAffineAnims_HitSplat,
    .callback = AnimHitSplatBasic,
};

const struct SpriteTemplate gRandomPosHitSplatSpriteTemplate =
{
    .tileTag = ANIM_TAG_IMPACT,
    .paletteTag = ANIM_TAG_IMPACT,
    .oam = &gOamData_AffineNormal_ObjBlend_32x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = sAffineAnims_HitSplat,
    .callback = AnimHitSplatRandom,
};

const struct SpriteTemplate gMonEdgeHitSplatSpriteTemplate =
{
    .tileTag = ANIM_TAG_IMPACT,
    .paletteTag = ANIM_TAG_IMPACT,
    .oam = &gOamData_AffineNormal_ObjBlend_32x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = sAffineAnims_HitSplat,
    .callback = AnimHitSplatOnMonEdge,
};

const struct SpriteTemplate gCrossImpactSpriteTemplate =
{
    .tileTag = ANIM_TAG_CROSS_IMPACT,
    .paletteTag = ANIM_TAG_CROSS_IMPACT,
    .oam = &gOamData_AffineOff_ObjBlend_32x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimCrossImpact,
};

const struct SpriteTemplate gFlashingHitSplatSpriteTemplate =
{
    .tileTag = ANIM_TAG_IMPACT,
    .paletteTag = ANIM_TAG_IMPACT,
    .oam = &gOamData_AffineNormal_ObjNormal_32x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = sAffineAnims_HitSplat,
    .callback = AnimFlashingHitSplat,
};

const struct SpriteTemplate gPersistHitSplatSpriteTemplate =
{
    .tileTag = ANIM_TAG_IMPACT,
    .paletteTag = ANIM_TAG_IMPACT,
    .oam = &gOamData_AffineNormal_ObjBlend_32x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = sAffineAnims_HitSplat,
    .callback = AnimHitSplatPersistent,
};

static const u16 sCurseLinesPalette = RGB(31, 31, 31);

// Moves a spinning duck around the mon's head.
// arg 0: initial x pixel offset
// arg 1: initial y pixel offset
// arg 2: initial wave offset
// arg 3: wave period (higher means faster wave)
// arg 4: duration
static void AnimConfusionDuck(struct Sprite *sprite)
{
    CMD_ARGS(x, y, waveOffset, wavePeriod, duration);

    sprite->x += cmd->x;
    sprite->y += cmd->y;
    sprite->data[0] = cmd->waveOffset;
    if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
    {
        sprite->data[1] = -cmd->wavePeriod;
        sprite->data[4] = 1;
    }
    else
    {
        sprite->data[1] = cmd->wavePeriod;
        sprite->data[4] = 0;
        StartSpriteAnim(sprite, 1);
    }

    sprite->data[3] = cmd->duration;
    sprite->callback = AnimConfusionDuck_Step;
    sprite->callback(sprite);
}

static void AnimConfusionDuck_Step(struct Sprite *sprite)
{
    sprite->x2 = Cos(sprite->data[0], 30);
    sprite->y2 = Sin(sprite->data[0], 10);

    if ((u16)sprite->data[0] < 128)
        sprite->oam.priority = 1;
    else
        sprite->oam.priority = 3;

    sprite->data[0] = (sprite->data[0] + sprite->data[1]) & 0xFF;
    if (++sprite->data[2] == sprite->data[3])
        DestroyAnimSprite(sprite);
}

// Performs a simple color blend on a specified sprite.
// arg 0: palette selector
// arg 1: delay
// arg 2: start blend amount
// arg 3: end blend amount
// arg 4: blend color
static void AnimSimplePaletteBlend(struct Sprite *sprite)
{
    CMD_ARGS(selector, delay, initialBlendY, targetBlendY, color);
    u32 selectedPalettes = UnpackSelectedBattlePalettes(cmd->selector);

    BeginNormalPaletteFade(selectedPalettes, cmd->delay, cmd->initialBlendY, cmd->targetBlendY, cmd->color);
    sprite->invisible = TRUE;
    sprite->callback = AnimSimplePaletteBlend_Step;
}

// Unpacks a bitfield and returns a bitmask of its selected palettes.
// Bits 0-6 of the selector parameter result in the following palettes being selected:
//   0: battle background palettes (BG palettes 1, 2, and 3)
//   1: gBattleAnimAttacker OBJ palette
//   2: gBattleAnimTarget OBJ palette
//   3: gBattleAnimAttacker partner OBJ palette
//   4: gBattleAnimTarget partner OBJ palette
//   5: BG palette 4
//   6: BG palette 5
u32 UnpackSelectedBattlePalettes(s16 selector)
{
    bool8 battleBackground = selector & 1;
    bool8 attacker = (selector >> 1) & 1;
    bool8 target = (selector >> 2) & 1;
    bool8 attackerPartner = (selector >> 3) & 1;
    bool8 targetPartner = (selector >> 4) & 1;
    bool8 anim1 = (selector >> 5) & 1;
    bool8 anim2 = (selector >> 6) & 1;

    return GetBattlePalettesMask(battleBackground, attacker, target, attackerPartner, targetPartner, anim1, anim2);
}

static void AnimSimplePaletteBlend_Step(struct Sprite *sprite)
{
    if (!gPaletteFade.active)
        DestroyAnimSprite(sprite);
}

#define sTimer           data[0]
#define sDelay           data[1]
#define sNumBlends       data[2]
#define sColor1          data[3]
#define sBlendY1         data[4]
#define sColor2          data[5]
#define sBlendY2         data[6]
#define sPaletteSelector data[7]

static void AnimComplexPaletteBlend(struct Sprite *sprite)
{
    CMD_ARGS(selector, delay, numBlends, color1, blendY1, color2, blendY2);
    u32 selectedPalettes;

    sprite->sTimer = cmd->delay;
    sprite->sDelay = cmd->delay;
    sprite->sNumBlends = cmd->numBlends;
    sprite->sColor1 = cmd->color1;
    sprite->sBlendY1 = cmd->blendY1;
    sprite->sColor2 = cmd->color2;
    sprite->sBlendY2 = cmd->blendY2;
    sprite->sPaletteSelector = cmd->selector;

    selectedPalettes = UnpackSelectedBattlePalettes(sprite->sPaletteSelector);
    BlendPalettes(selectedPalettes, cmd->blendY1, cmd->color1);
    sprite->invisible = TRUE;
    sprite->callback = AnimComplexPaletteBlend_Step1;
}

static void AnimComplexPaletteBlend_Step1(struct Sprite *sprite)
{
    u32 selectedPalettes;

    if (sprite->sTimer > 0)
    {
        sprite->sTimer--;
        return;
    }

    if (gPaletteFade.active)
        return;

    if (sprite->sNumBlends == 0)
    {
        sprite->callback = AnimComplexPaletteBlend_Step2;
        return;
    }

    selectedPalettes = UnpackSelectedBattlePalettes(sprite->sPaletteSelector);
    if (sprite->sDelay & 0x100)
        BlendPalettes(selectedPalettes, sprite->sBlendY1, sprite->sColor1);
    else
        BlendPalettes(selectedPalettes, sprite->sBlendY2, sprite->sColor2);

    sprite->sDelay ^= 0x100;
    sprite->sTimer = sprite->sDelay & 0xFF;
    sprite->sNumBlends--;
}

static void AnimComplexPaletteBlend_Step2(struct Sprite *sprite)
{
    u32 selectedPalettes;

    if (!gPaletteFade.active)
    {
        selectedPalettes = UnpackSelectedBattlePalettes(sprite->sPaletteSelector);
        BlendPalettes(selectedPalettes, 0, 0);
        DestroyAnimSprite(sprite);
    }
}

#undef sTimer
#undef sDelay
#undef sNumBlends
#undef sColor1
#undef sBlendY1
#undef sColor2
#undef sBlendY2
#undef sPaletteSelector

static void AnimCirclingSparkle(struct Sprite *sprite)
{
    CMD_ARGS(x, y);

    sprite->x += cmd->x;
    sprite->y += cmd->y;
    sprite->data[0] = 0;
    sprite->data[1] = 10;
    sprite->data[2] = 8;
    sprite->data[3] = 40;
    sprite->data[4] = 112;
    sprite->data[5] = 0;
    StoreSpriteCallbackInData6(sprite, DestroySpriteAndMatrix);
    sprite->callback = TranslateSpriteInGrowingCircle;
    sprite->callback(sprite);
}

#define tPalSelector   data[0]
#define tPalTag        data[0]
#define tDelay         data[1]
#define tNumBlends     data[2]
#define tInitialBlendY data[3]
#define tTargetBlendY  data[4]
#define tBlendColor    data[5]
#define tRestoreBlend  data[8]

void AnimTask_BlendColorCycle(u8 taskId)
{
    CMD_ARGS(selector, delay, numBlends, initialBlendY, targetBlendY, color);

    gTasks[taskId].tPalSelector = cmd->selector;
    gTasks[taskId].tDelay = cmd->delay;
    gTasks[taskId].tNumBlends = cmd->numBlends;
    gTasks[taskId].tInitialBlendY = cmd->initialBlendY;
    gTasks[taskId].tTargetBlendY = cmd->targetBlendY;
    gTasks[taskId].tBlendColor = cmd->color;
    gTasks[taskId].tRestoreBlend = FALSE;
    BlendColorCycle(taskId, 0, gTasks[taskId].tTargetBlendY);
    gTasks[taskId].func = AnimTask_BlendColorCycleLoop;
}

static void BlendColorCycle(u8 taskId, u8 startBlendAmount, u8 targetBlendAmount)
{
    u32 selectedPalettes = UnpackSelectedBattlePalettes(gTasks[taskId].tPalSelector);
    BeginNormalPaletteFade(
        selectedPalettes,
        gTasks[taskId].tDelay,
        startBlendAmount,
        targetBlendAmount,
        gTasks[taskId].tBlendColor);

    gTasks[taskId].tNumBlends--;
    gTasks[taskId].tRestoreBlend ^= TRUE;
}

static void AnimTask_BlendColorCycleLoop(u8 taskId)
{
    u8 startBlendAmount, targetBlendAmount;
    if (!gPaletteFade.active)
    {
        if (gTasks[taskId].tNumBlends > 0)
        {
            if (gTasks[taskId].tRestoreBlend == FALSE)
            {
                startBlendAmount = gTasks[taskId].tInitialBlendY;
                targetBlendAmount = gTasks[taskId].tTargetBlendY;
            }
            else
            {
                startBlendAmount = gTasks[taskId].tTargetBlendY;
                targetBlendAmount = gTasks[taskId].tInitialBlendY;
            }

            if (gTasks[taskId].tNumBlends == 1)
                targetBlendAmount = 0;

            BlendColorCycle(taskId, startBlendAmount, targetBlendAmount);
        }
        else
        {
            DestroyAnimVisualTask(taskId);
        }
    }
}

void AnimTask_BlendColorCycleByTag(u8 taskId)
{
    CMD_ARGS(tag, delay, numBlends, initialBlendY, targetBlendY, color);

    gTasks[taskId].tPalTag = cmd->tag;
    gTasks[taskId].tDelay = cmd->delay;
    gTasks[taskId].tNumBlends = cmd->numBlends;
    gTasks[taskId].tInitialBlendY = cmd->initialBlendY;
    gTasks[taskId].tTargetBlendY = cmd->targetBlendY;
    gTasks[taskId].tBlendColor = cmd->color;
    gTasks[taskId].tRestoreBlend = FALSE;
    BlendColorCycleByTag(taskId, 0, gTasks[taskId].tTargetBlendY);
    gTasks[taskId].func = AnimTask_BlendColorCycleByTagLoop;
}

static void BlendColorCycleByTag(u8 taskId, u8 initialBlendAmount, u8 targetBlendAmount)
{
    u8 paletteIndex = IndexOfSpritePaletteTag(gTasks[taskId].tPalTag);
    BeginNormalPaletteFade(
        1 << (paletteIndex + 16),
        gTasks[taskId].tDelay,
        initialBlendAmount,
        targetBlendAmount,
        gTasks[taskId].tBlendColor);

    gTasks[taskId].tNumBlends--;
    gTasks[taskId].tRestoreBlend ^= TRUE;
}

static void AnimTask_BlendColorCycleByTagLoop(u8 taskId)
{
    u8 initialBlendAmount, targetBlendAmount;
    if (!gPaletteFade.active)
    {
        if (gTasks[taskId].tNumBlends > 0)
        {
            if (gTasks[taskId].tRestoreBlend == FALSE)
            {
                initialBlendAmount = gTasks[taskId].tInitialBlendY;
                targetBlendAmount = gTasks[taskId].tTargetBlendY;
            }
            else
            {
                initialBlendAmount = gTasks[taskId].tTargetBlendY;
                targetBlendAmount = gTasks[taskId].tInitialBlendY;
            }

            if (gTasks[taskId].tNumBlends == 1)
                targetBlendAmount = 0;

            BlendColorCycleByTag(taskId, initialBlendAmount, targetBlendAmount);
        }
        else
        {
            DestroyAnimVisualTask(taskId);
        }
    }
}

#undef tPalSelector
#undef tPalTag
#undef tDelay
#undef tNumBlends
#undef tInitialBlendY
#undef tTargetBlendY
#undef tBlendColor
#undef tRestoreBlend

#define tTimer     data[0]
#define tDelay     data[1]
#define tNumBlends data[2]
#define tColor1    data[3]
#define tBlendY1   data[4]
#define tColor2    data[5]
#define tBlendY2   data[6]
#define tAnimTag   data[7]

void AnimTask_FlashAnimTagWithColor(u8 taskId)
{
    CMD_ARGS(tag, delay, numBlends, color1, blendY1, color2, blendY2);
    u8 paletteIndex;

    gTasks[taskId].tTimer = cmd->delay;
    gTasks[taskId].tDelay = cmd->delay;
    gTasks[taskId].tNumBlends = cmd->numBlends;
    gTasks[taskId].tColor1 = cmd->color1;
    gTasks[taskId].tBlendY1 = cmd->blendY1;
    gTasks[taskId].tColor2 = cmd->color2;
    gTasks[taskId].tBlendY2 = cmd->blendY2;
    gTasks[taskId].tAnimTag = cmd->tag;

    paletteIndex = IndexOfSpritePaletteTag(cmd->tag);
    BeginNormalPaletteFade(
        1 << (paletteIndex + 16),
        0,
        cmd->blendY1,
        cmd->blendY1,
        cmd->color1);

    gTasks[taskId].func = AnimTask_FlashAnimTagWithColor_Step1;
}

static void AnimTask_FlashAnimTagWithColor_Step1(u8 taskId)
{
    u32 selectedPalettes;

    if (gTasks[taskId].tTimer > 0)
    {
        gTasks[taskId].tTimer--;
        return;
    }

    if (gPaletteFade.active)
        return;

    if (gTasks[taskId].tNumBlends == 0)
    {
        gTasks[taskId].func = AnimTask_FlashAnimTagWithColor_Step2;
        return;
    }

    selectedPalettes = 1 << (IndexOfSpritePaletteTag(gTasks[taskId].tAnimTag) + 16);
    if (gTasks[taskId].tDelay & 0x100)
    {
        BeginNormalPaletteFade(
            selectedPalettes,
            0,
            gTasks[taskId].tBlendY1,
            gTasks[taskId].tBlendY1,
            gTasks[taskId].tColor1);
    }
    else
    {
        BeginNormalPaletteFade(
            selectedPalettes,
            0,
            gTasks[taskId].tBlendY2,
            gTasks[taskId].tBlendY2,
            gTasks[taskId].tColor2);
    }

    gTasks[taskId].tDelay ^= 0x100;
    gTasks[taskId].tTimer = gTasks[taskId].tDelay & 0xFF;
    gTasks[taskId].tNumBlends--;
}

static void AnimTask_FlashAnimTagWithColor_Step2(u8 taskId)
{
    u32 selectedPalettes;

    if (!gPaletteFade.active)
    {
        selectedPalettes = 1 << (IndexOfSpritePaletteTag(gTasks[taskId].tAnimTag) + 16);
        BeginNormalPaletteFade(selectedPalettes, 0, 0, 0, RGB_BLACK);
        DestroyAnimVisualTask(taskId);
    }
}

#undef tTimer
#undef tDelay
#undef tNumBlends
#undef tColor1
#undef tBlendY1
#undef tColor2
#undef tBlendY2
#undef tAnimTag

void AnimTask_InvertScreenColor(u8 taskId)
{
    CMD_ARGS(flagsScenery, flagsAttacker, flagsTarget);
    u32 selectedPalettes = 0;
    u8 attackerBattler = gBattleAnimAttacker;
    u8 targetBattler = gBattleAnimTarget;

    if (cmd->flagsScenery & (1 << 8))
        selectedPalettes = GetBattlePalettesMask(TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE);

    if (cmd->flagsAttacker & (1 << 8))
        selectedPalettes |= (0x10000 << attackerBattler);

    if (cmd->flagsTarget & (1 << 8))
        selectedPalettes |= (0x10000 << targetBattler);

    InvertPlttBuffer(selectedPalettes);
    DestroyAnimVisualTask(taskId);
}

#define tTimer         data[0]
#define tLength        data[1]
#define tFlagsScenery  data[2]
#define tFlagsAttacker data[3]
#define tFlagsTarget   data[4]
#define tColorR        data[5]
#define tColorG        data[6]
#define tColorB        data[7]

void AnimTask_TintPalettes(u8 taskId)
{
    CMD_ARGS(flagsScenery, flagsAttacker, flagsTarget, duration, r, g, b);
    u8 attackerBattler;
    u8 targetBattler;
    u8 paletteIndex;
    u32 selectedPalettes = 0;

    if (gTasks[taskId].tTimer == 0)
    {
        gTasks[taskId].tFlagsScenery = cmd->flagsScenery;
        gTasks[taskId].tFlagsAttacker = cmd->flagsAttacker;
        gTasks[taskId].tFlagsTarget = cmd->flagsTarget;
        gTasks[taskId].tLength = cmd->duration;
        gTasks[taskId].tColorR = cmd->r;
        gTasks[taskId].tColorG = cmd->g;
        gTasks[taskId].tColorB = cmd->b;
    }

    gTasks[taskId].tTimer++;
    attackerBattler = gBattleAnimAttacker;
    targetBattler = gBattleAnimTarget;

    if (gTasks[taskId].tFlagsScenery & (1 << 8))
        selectedPalettes = PALETTES_BG;

    if (gTasks[taskId].tFlagsScenery & 1)
    {
        paletteIndex = IndexOfSpritePaletteTag(gSprites[gHealthboxSpriteIds[attackerBattler]].template->paletteTag);
        selectedPalettes |= ((1 << paletteIndex) << 16);
    }
    
    if (gTasks[taskId].tFlagsAttacker & (1 << 8))
        selectedPalettes |= ((1 << attackerBattler) << 16);

    if (gTasks[taskId].tFlagsTarget & (1 << 8))
        selectedPalettes |= ((1 << targetBattler) << 16);

    TintPlttBuffer(selectedPalettes, gTasks[taskId].tColorR, gTasks[taskId].tColorG, gTasks[taskId].tColorB);
    if (gTasks[taskId].tTimer == gTasks[taskId].tLength)
    {
        UnfadePlttBuffer(selectedPalettes);
        DestroyAnimVisualTask(taskId);
    }
}

#undef tTimer
#undef tLength
#undef tFlagsScenery
#undef tFlagsAttacker
#undef tFlagsTarget
#undef tColorR
#undef tColorG
#undef tColorB

enum { SHAKE_BG_X, SHAKE_BG_Y, SHAKE_MON_X, SHAKE_MON_Y };
enum { SHAKE_MON_ATTACKER, SHAKE_MON_TARGET, SHAKE_MON_BOTH };

#define sShakeVelocity data[0]
#define sShakeTimer    data[1]
#define sShakeDuration data[2]
#define sTimer         data[3]
#define sOriginalValue data[4]
#define sType          data[5]
#define sShakePtrLo    data[6]
#define sShakePtrHi    data[7]

static void AnimShakeMonOrBattlePlatforms(struct Sprite *sprite)
{
    CMD_ARGS(velocity, shakeTimer, shakeDuration, type, battlerSelector);
    u16 shakeType;

    sprite->invisible = TRUE;
    sprite->sShakeVelocity = -cmd->velocity;
    sprite->sShakeTimer = cmd->shakeTimer;
    sprite->sShakeDuration = cmd->shakeTimer;
    sprite->sTimer = cmd->shakeDuration;

    switch (cmd->type)
    {
    case SHAKE_BG_X:
        StoreSpriteCallbackInData6(sprite, (void *)&gBattle_BG3_X);
        break;
    case SHAKE_BG_Y:
        StoreSpriteCallbackInData6(sprite, (void *)&gBattle_BG3_Y);
        break;
    case SHAKE_MON_X:
        StoreSpriteCallbackInData6(sprite, (void *)&gSpriteCoordOffsetX);
        break;
    default:
        StoreSpriteCallbackInData6(sprite, (void *)&gSpriteCoordOffsetY);
        break;
    }

    #if MODERN
    sprite->sOriginalValue = *(u16 *)(sprite->sShakePtrLo | (sprite->sShakePtrHi << 16));
    #else
    sprite->sOriginalValue = *(u32 *)(sprite->sShakePtrLo | (sprite->sShakePtrHi << 16));
    #endif
    sprite->sType = cmd->type;
    shakeType = sprite->sType - SHAKE_MON_X;
    if (shakeType < 2)
        AnimShakeMonOrBattlePlatforms_UpdateCoordOffsetEnabled();

    sprite->callback = AnimShakeMonOrBattlePlatforms_Step;
}

static void AnimShakeMonOrBattlePlatforms_Step(struct Sprite *sprite)
{
    u8 i;
    u16 shakeType;

    if (sprite->sTimer > 0)
    {
        sprite->sTimer--;
        if (sprite->sShakeTimer > 0)
        {
            sprite->sShakeTimer--;
        }
        else
        {
            sprite->sShakeTimer = sprite->sShakeDuration;
            #if MODERN
            *(u16 *)(sprite->sShakePtrLo | (sprite->sShakePtrHi << 16)) += sprite->sShakeVelocity;
            #else
            *(u32 *)(sprite->sShakePtrLo | (sprite->sShakePtrHi << 16)) += sprite->sShakeVelocity;
            #endif
            sprite->sShakeVelocity = -sprite->sShakeVelocity;
        }
    }
    else
    {
        #if MODERN
        *(u16 *)(sprite->sShakePtrLo | (sprite->sShakePtrHi << 16)) = sprite->sOriginalValue;
        #else
        *(u32 *)(sprite->sShakePtrLo | (sprite->sShakePtrHi << 16)) = sprite->sOriginalValue;
        #endif
        shakeType = sprite->sType - SHAKE_MON_X;
        if (shakeType < 2)
        {
            for (i = 0; i < gBattlersCount; i++)
                gSprites[gBattlerSpriteIds[i]].coordOffsetEnabled = FALSE;
        }

        DestroyAnimSprite(sprite);
    }
}

static void AnimShakeMonOrBattlePlatforms_UpdateCoordOffsetEnabled(void)
{
    CMD_ARGS(velocity, shakeDuration, duration, type, battlerSelector);

    gSprites[gBattlerSpriteIds[gBattleAnimAttacker]].coordOffsetEnabled = FALSE;
    gSprites[gBattlerSpriteIds[gBattleAnimTarget]].coordOffsetEnabled = FALSE;

    if (cmd->battlerSelector == SHAKE_MON_BOTH)
    {
        gSprites[gBattlerSpriteIds[gBattleAnimAttacker]].coordOffsetEnabled = TRUE;
        gSprites[gBattlerSpriteIds[gBattleAnimTarget]].coordOffsetEnabled = TRUE;
    }
    else
    {
        if (cmd->battlerSelector == SHAKE_MON_ATTACKER)
            gSprites[gBattlerSpriteIds[gBattleAnimAttacker]].coordOffsetEnabled = TRUE;
        else
            gSprites[gBattlerSpriteIds[gBattleAnimTarget]].coordOffsetEnabled = TRUE;
    }
}

#undef sShakeVelocity
#undef sShakeTimer
#undef sShakeDuration
#undef sTimer
#undef sOriginalValue
#undef sType
#undef sShakePtrLo
#undef sShakePtrHi

#define tXOffset    data[0]
#define tYOffset    data[1]
#define tNumShakes  data[2]
#define tTimer      data[3]
#define tShakeDelay data[8]

void AnimTask_ShakeBattlePlatforms(u8 taskId)
{
    CMD_ARGS(xOffset, yOffset, shakes, delay);

    gTasks[taskId].tXOffset = cmd->xOffset;
    gTasks[taskId].tYOffset = cmd->yOffset;
    gTasks[taskId].tNumShakes = cmd->shakes;
    gTasks[taskId].tTimer = cmd->delay;
    gTasks[taskId].tShakeDelay = cmd->delay;
    gBattle_BG3_X = cmd->xOffset;
    gBattle_BG3_Y = cmd->yOffset;
    gTasks[taskId].func = AnimTask_ShakeBattlePlatforms_Step;
    gTasks[taskId].func(taskId);
}

static void AnimTask_ShakeBattlePlatforms_Step(u8 taskId)
{
    if (gTasks[taskId].tTimer == 0)
    {
        if (gBattle_BG3_X == gTasks[taskId].tXOffset)
            gBattle_BG3_X = -gTasks[taskId].tXOffset;
        else
            gBattle_BG3_X = gTasks[taskId].tXOffset;

        if (gBattle_BG3_Y == -gTasks[taskId].tYOffset)
            gBattle_BG3_Y = 0;
        else
            gBattle_BG3_Y = -gTasks[taskId].tYOffset;

        gTasks[taskId].tTimer = gTasks[taskId].tShakeDelay;
        if (--gTasks[taskId].tNumShakes == 0)
        {
            gBattle_BG3_X = 0;
            gBattle_BG3_Y = 0;
            DestroyAnimVisualTask(taskId);
        }
    }
    else
    {
        gTasks[taskId].tTimer--;
    }
}

#undef tXOffset
#undef tYOffset
#undef tNumShakes
#undef tTimer
#undef tShakeDelay

static void AnimHitSplatBasic(struct Sprite *sprite)
{
    CMD_ARGS(x, y, relativeTo, animation);

    StartSpriteAffineAnim(sprite, cmd->animation);
    if (cmd->relativeTo == ANIM_ATTACKER)
        InitSpritePosToAnimAttacker(sprite, TRUE);
    else
        InitSpritePosToAnimTarget(sprite, TRUE);

    sprite->callback = RunStoredCallbackWhenAffineAnimEnds;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}

static void AnimHitSplatPersistent(struct Sprite *sprite)
{
    CMD_ARGS(x, y, relativeTo, animation, duration);

    StartSpriteAffineAnim(sprite, cmd->animation);
    if (cmd->relativeTo == ANIM_ATTACKER)
        InitSpritePosToAnimAttacker(sprite, TRUE);
    else
        InitSpritePosToAnimTarget(sprite, TRUE);

    sprite->data[0] = cmd->duration;
    sprite->callback = RunStoredCallbackWhenAffineAnimEnds;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSpriteAfterTimer);
}

static void AnimHitSplatHandleInvert(struct Sprite *sprite)
{
    CMD_ARGS(x, y, relativeTo, animation);

    if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER && !IsContest())
        cmd->y = -cmd->y;
    
    AnimHitSplatBasic(sprite);
}

static void AnimHitSplatRandom(struct Sprite *sprite)
{
    CMD_ARGS(relativeTo, animation);

    if (cmd->animation == -1)
        cmd->animation = Random() & 3;

    StartSpriteAffineAnim(sprite, cmd->animation);
    if (cmd->relativeTo == ANIM_ATTACKER)
        InitSpritePosToAnimAttacker(sprite, FALSE);
    else
        InitSpritePosToAnimTarget(sprite, FALSE);

    sprite->x2 += (Random() % 48) - 24;
    sprite->y2 += (Random() % 24) - 12;

    StoreSpriteCallbackInData6(sprite, DestroySpriteAndMatrix);
    sprite->callback = RunStoredCallbackWhenAffineAnimEnds;
}

static void AnimHitSplatOnMonEdge(struct Sprite *sprite)
{
    CMD_ARGS(relativeTo, x, y, animation);

    sprite->data[0] = GetAnimBattlerSpriteId(cmd->relativeTo);
    sprite->x = gSprites[sprite->data[0]].x + gSprites[sprite->data[0]].x2;
    sprite->y = gSprites[sprite->data[0]].y + gSprites[sprite->data[0]].y2;
    sprite->x2 = cmd->x;
    sprite->y2 = cmd->y;
    StartSpriteAffineAnim(sprite, cmd->animation);
    StoreSpriteCallbackInData6(sprite, DestroySpriteAndMatrix);
    sprite->callback = RunStoredCallbackWhenAffineAnimEnds;
}

static void AnimCrossImpact(struct Sprite *sprite)
{
    CMD_ARGS(x, y, relativeTo, duration);

    if (cmd->relativeTo == ANIM_ATTACKER)
        InitSpritePosToAnimAttacker(sprite, TRUE);
    else
        InitSpritePosToAnimTarget(sprite, TRUE);

    sprite->data[0] = cmd->duration;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
    sprite->callback = WaitAnimForDuration;
}

static void AnimFlashingHitSplat(struct Sprite *sprite)
{
    CMD_ARGS(x, y, relativeTo, animation);

    StartSpriteAffineAnim(sprite, cmd->animation);
    if (cmd->relativeTo == ANIM_ATTACKER)
        InitSpritePosToAnimAttacker(sprite, TRUE);
    else
        InitSpritePosToAnimTarget(sprite, TRUE);

    sprite->callback = AnimFlashingHitSplat_Step;
}

static void AnimFlashingHitSplat_Step(struct Sprite *sprite)
{
    sprite->invisible ^= TRUE;
    if (sprite->data[0]++ > 12)
        DestroyAnimSprite(sprite);
}

void AnimTask_BlendBattleAnimPal(u8 taskId)
{
    CMD_ARGS(selector);
    u32 selectedPalettes = UnpackSelectedBattlePalettes(cmd->selector);

    selectedPalettes |= GetBattleMonSpritePalettesMask(
        (cmd->selector >> 7)  & 1,
        (cmd->selector >> 8)  & 1,
        (cmd->selector >> 9)  & 1,
        (cmd->selector >> 10) & 1);

    StartBlendAnimSpriteColor(taskId, selectedPalettes);
}

void AnimTask_BlendBattleAnimPalExclude(u8 taskId)
{
    CMD_ARGS(selector);
    u8 battler;
    u32 selectedPalettes;
    u8 animBattlers[2];

    animBattlers[1] = 0xFF;
    selectedPalettes = UnpackSelectedBattlePalettes(F_PAL_BG);
    switch (cmd->selector)
    {
    case 2:
        selectedPalettes = 0;
        // fall through
    case ANIM_ATTACKER:
        animBattlers[0] = gBattleAnimAttacker;
        break;
    case 3:
        selectedPalettes = 0;
        // fall through
    case ANIM_TARGET:
        animBattlers[0] = gBattleAnimTarget;
        break;
    case 4:
        animBattlers[0] = gBattleAnimAttacker;
        animBattlers[1] = gBattleAnimTarget;
        break;
    case 5:
        animBattlers[0] = 0xFF;
        break;
    case 6:
        selectedPalettes = 0;
        animBattlers[0] = BATTLE_PARTNER(gBattleAnimAttacker);
        break;
    case 7:
        selectedPalettes = 0;
        animBattlers[0] = BATTLE_PARTNER(gBattleAnimTarget);
        break;
    }

    for (battler = 0; battler < MAX_BATTLERS_COUNT; battler++)
    {
        if (battler != animBattlers[0] && battler != animBattlers[1] && IsBattlerSpriteVisible(battler))
        {
            u8 paletteIndex = GetSpritePalIdxByBattler(battler);
            selectedPalettes |= (0x10000 << paletteIndex);
        }
    }

    StartBlendAnimSpriteColor(taskId, selectedPalettes);
}

void AnimTask_SetCamouflageBlend(u8 taskId)
{
    CMD_ARGS(selector, unused1, unused2, unused3, color);
    u32 selectedPalettes = UnpackSelectedBattlePalettes(cmd->selector);

    switch (gBattleEnvironment)
    {
    case BATTLE_ENVIRONMENT_GRASS:
        cmd->color = RGB(12, 24, 2);
        break;
    case BATTLE_ENVIRONMENT_LONG_GRASS:
        cmd->color = RGB(0, 15, 2);
        break;
    case BATTLE_ENVIRONMENT_SAND:
        cmd->color = RGB(30, 24, 11);
        break;
    case BATTLE_ENVIRONMENT_UNDERWATER:
        cmd->color = RGB(0, 0, 18);
        break;
    case BATTLE_ENVIRONMENT_WATER:
        cmd->color = RGB(11, 22, 31);
        break;
    case BATTLE_ENVIRONMENT_POND:
        cmd->color = RGB(11, 22, 31);
        break;
    case BATTLE_ENVIRONMENT_MOUNTAIN:
        cmd->color = RGB(22, 16, 10);
        break;
    case BATTLE_ENVIRONMENT_CAVE:
        cmd->color = RGB(14, 9, 3);
        break;
    case BATTLE_ENVIRONMENT_BUILDING:
        cmd->color = RGB_WHITE;
        break;
    case BATTLE_ENVIRONMENT_PLAIN:
        cmd->color = RGB_WHITE;
        break;
    }

    StartBlendAnimSpriteColor(taskId, selectedPalettes);
}

// Used to add a color mask to a Sprite.
// arg 0: spriteId
// arg 1: ??? unknown
// arg 2: ??? unknown
// arg 3: coefficient
// arg 4: color code
void AnimTask_BlendParticle(u8 taskId)
{
    CMD_ARGS(tag);
    u8 paletteIndex = IndexOfSpritePaletteTag(cmd->tag);

    StartBlendAnimSpriteColor(taskId, 1 << (paletteIndex + 16));
}

static void StartBlendAnimSpriteColor(u8 taskId, u32 selectedPalettes)
{
    CMD_ARGS(selector, delay, initialBlendY, targetBlendY, color);

    gTasks[taskId].data[0] = selectedPalettes;
    gTasks[taskId].data[1] = selectedPalettes >> 16;
    gTasks[taskId].data[2] = cmd->delay;
    gTasks[taskId].data[3] = cmd->initialBlendY;
    gTasks[taskId].data[4] = cmd->targetBlendY;
    gTasks[taskId].data[5] = cmd->color;
    gTasks[taskId].data[10] = cmd->initialBlendY;
    gTasks[taskId].func = AnimTask_BlendSpriteColor_Step2;
    gTasks[taskId].func(taskId);
}

static void AnimTask_BlendSpriteColor_Step2(u8 taskId)
{
    u32 selectedPalettes;
    u16 singlePaletteOffset = 0;

    if (gTasks[taskId].data[9] == gTasks[taskId].data[2])
    {
        gTasks[taskId].data[9] = 0;
        selectedPalettes = gTasks[taskId].data[0] | (gTasks[taskId].data[1] << 16);
        while (selectedPalettes != 0)
        {
            if (selectedPalettes & 1)
                BlendPalette(singlePaletteOffset, 16, gTasks[taskId].data[10], gTasks[taskId].data[5]);
            singlePaletteOffset += 16;
            selectedPalettes >>= 1;
        }

        if (gTasks[taskId].data[10] < gTasks[taskId].data[4])
            gTasks[taskId].data[10]++;
        else if (gTasks[taskId].data[10] > gTasks[taskId].data[4])
            gTasks[taskId].data[10]--;
        else
            DestroyAnimVisualTask(taskId);
    }
    else
    {
        gTasks[taskId].data[9]++;
    }
}

void AnimTask_HardwarePaletteFade(u8 taskId)
{
    CMD_ARGS(selectedPalettes, delay, initialBlendY, targetBlendY, color);

    BeginHardwarePaletteFade(
        cmd->selectedPalettes,
        cmd->delay,
        cmd->initialBlendY,
        cmd->targetBlendY,
        cmd->color);

    gTasks[taskId].func = AnimTask_HardwarePaletteFade_Step;
}

static void AnimTask_HardwarePaletteFade_Step(u8 taskId)
{
    if (!gPaletteFade.active)
        DestroyAnimVisualTask(taskId);
}

void AnimTask_TraceMonBlended(u8 taskId)
{
    CMD_ARGS(battler, delay, duration, numTraces);
    struct Task *task = &gTasks[taskId];

    task->data[0] = cmd->battler;
    task->data[1] = FALSE;
    task->data[2] = cmd->delay;
    task->data[3] = cmd->duration;
    task->data[4] = cmd->numTraces;
    task->data[5] = FALSE;
    task->func = AnimTask_TraceMonBlended_Step;
}

static void AnimTask_TraceMonBlended_Step(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    if (task->data[4])
    {
        if (task->data[1])
        {
            task->data[1]--;
        }
        else
        {
            task->data[6] = CloneBattlerSpriteWithBlend(task->data[0]);
            if (task->data[6] >= 0)
            {
                gSprites[task->data[6]].oam.priority = task->data[0] ? 1 : 2;
                gSprites[task->data[6]].data[0] = task->data[3];
                gSprites[task->data[6]].data[1] = taskId;
                gSprites[task->data[6]].data[2] = 5;
                gSprites[task->data[6]].callback = AnimMonTrace;
                task->data[5]++;
            }

            task->data[4]--;
            task->data[1] = task->data[2];
        }
    }
    else if (task->data[5] == 0)
    {
        DestroyAnimVisualTask(taskId);
    }
}

static void AnimMonTrace(struct Sprite *sprite)
{
    if (sprite->data[0])
    {
        sprite->data[0]--;
    }
    else
    {
        gTasks[sprite->data[1]].data[sprite->data[2]]--;
        DestroySpriteWithActiveSheet(sprite);
    }
}

void AnimTask_DrawFallingWhiteLinesOnAttacker(u8 taskId)
{
    u16 species;
    int spriteId, newSpriteId;
    u16 loweredPartnerPriority;
    struct BattleAnimBgData animBg;

    loweredPartnerPriority = FALSE;
    gBattle_WIN0H = 0;
    gBattle_WIN0V = 0;
    REG_WININ = WININ_WIN0_BG_ALL | WININ_WIN0_OBJ | WININ_WIN0_CLR | WININ_WIN1_BG_ALL | WININ_WIN1_OBJ | WININ_WIN1_CLR;
    REG_WINOUT = WINOUT_WIN01_BG0 | WINOUT_WIN01_BG2 | WINOUT_WIN01_BG3 | WINOUT_WIN01_OBJ | WINOUT_WIN01_CLR | WINOUT_WINOBJ_BG_ALL | WINOUT_WINOBJ_OBJ | WINOUT_WINOBJ_CLR;
    REG_DISPCNT |= DISPCNT_OBJWIN_ON;
    REG_BLDCNT = BLDCNT_TGT1_BG1 | BLDCNT_TGT2_ALL | BLDCNT_EFFECT_BLEND;
    REG_BLDALPHA = BLDALPHA_BLEND(8, 12);
    REG_BG1CNT_BITFIELD.priority = 0;
    REG_BG1CNT_BITFIELD.screenSize = 0;
    REG_BG1CNT_BITFIELD.areaOverflowMode = 1;

    if (!IsContest())
        REG_BG1CNT_BITFIELD.charBaseBlock = 1;

    if (IsDoubleBattle() && !IsContest())
    {
        if (GetBattlerPosition(gBattleAnimAttacker) == B_POSITION_OPPONENT_RIGHT
         || GetBattlerPosition(gBattleAnimAttacker) == B_POSITION_PLAYER_LEFT)
        {
            if (IsBattlerSpriteVisible(BATTLE_PARTNER(gBattleAnimAttacker)) == TRUE)
            {
                gSprites[gBattlerSpriteIds[BATTLE_PARTNER(gBattleAnimAttacker)]].oam.priority -= 1;
                REG_BG1CNT_BITFIELD.priority = 1;
                loweredPartnerPriority = TRUE;
            }
        }
    }

    if (IsContest())
    {
        species = gContestResources__moveAnim.species;
    }
    else
    {
        if (GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
            species = GetMonData(&gEnemyParty[gBattlerPartyIndexes[gBattleAnimAttacker]], MON_DATA_SPECIES);
        else
            species = GetMonData(&gPlayerParty[gBattlerPartyIndexes[gBattleAnimAttacker]], MON_DATA_SPECIES);
    }

    spriteId = GetAnimBattlerSpriteId(ANIM_ATTACKER);
    newSpriteId = CreateInvisibleSpriteCopy(gBattleAnimAttacker, spriteId, species);
    GetBattleAnimBg1Data(&animBg);
    DmaFill32Defvars(3, 0, animBg.bgTilemap, 0x1000);
    LZDecompressVram(&gBattleAnimMaskTilemap_Curse, animBg.bgTilemap);
    if (IsContest())
        RelocateBattleBgPal(animBg.paletteId, (u16 *)animBg.bgTilemap, 0, FALSE);
    
    LZDecompressVram(&gBattleAnimMaskImage_Curse, animBg.bgTiles);
    LoadPalette(&sCurseLinesPalette, animBg.paletteId * 16 + 1, 2);

    gBattle_BG1_X = -gSprites[spriteId].x + 32;
    gBattle_BG1_Y = -gSprites[spriteId].y + 32;
    gTasks[taskId].data[0] = newSpriteId;
    gTasks[taskId].data[6] = loweredPartnerPriority;
    gTasks[taskId].func = AnimTask_DrawFallingWhiteLinesOnAttacker_Step;
}

static void AnimTask_DrawFallingWhiteLinesOnAttacker_Step(u8 taskId)
{
    struct BattleAnimBgData animBg;
    struct Sprite *sprite;

    gTasks[taskId].data[10] += 4;
    gBattle_BG1_Y -= 4;
    if (gTasks[taskId].data[10] == 64)
    {
        gTasks[taskId].data[10] = 0;
        gBattle_BG1_Y += 64;
        if (++gTasks[taskId].data[11] == 4)
        {
            ResetBattleAnimBg(FALSE);
            gBattle_WIN0H = 0;
            gBattle_WIN0V = 0;
            REG_WININ = WININ_WIN0_BG_ALL | WININ_WIN0_OBJ | WININ_WIN0_CLR | WININ_WIN1_BG_ALL | WININ_WIN1_OBJ | WININ_WIN1_CLR;
            REG_WINOUT = WINOUT_WIN01_BG_ALL | WINOUT_WIN01_OBJ | WINOUT_WIN01_CLR | WINOUT_WINOBJ_BG_ALL | WINOUT_WINOBJ_OBJ | WINOUT_WINOBJ_CLR;

            if (!IsContest())
                REG_BG1CNT_BITFIELD.charBaseBlock = 0;

            REG_DISPCNT ^= DISPCNT_OBJWIN_ON;
            REG_BLDCNT = 0;
            REG_BLDALPHA = 0;
            sprite = &gSprites[GetAnimBattlerSpriteId(ANIM_ATTACKER)]; // unused
            sprite = &gSprites[gTasks[taskId].data[0]];
            DestroySprite(sprite);

            GetBattleAnimBg1Data(&animBg);
            DmaFill32Defvars(3, 0, animBg.bgTilemap, 0x800);
            if (gTasks[taskId].data[6] == TRUE)
                gSprites[gBattlerSpriteIds[BATTLE_PARTNER(gBattleAnimAttacker)]].oam.priority++;

            REG_BG1CNT_BITFIELD.areaOverflowMode = 0;
            do {} while(0); // needed to match. perhaps part of a debug macro
            gBattle_BG1_Y = 0;
            DestroyAnimVisualTask(taskId);
        }
    }
}

#define aDecrease         taskData[0]
#define aAnimStatId       taskData[1]
#define aIsTarget         taskData[2]
#define aMultipleBattlers taskData[3]
#define aSharply          taskData[4]

#define tAnimSpriteId1    data[0]
#define tVelocity         data[1]
#define tMultipleBattlers data[2]
#define tAnimSpriteId2    data[3]
#define tTargetBlend      data[4]
#define tWaitTime         data[5]
#define tHidBattler2      data[6]
#define tBattler2SpriteId data[7]
#define tWaitTimer        data[10]
#define tFadeTimer        data[11]
#define tBlend            data[12]
#define tState            data[15]

void InitStatsChangeAnimation(u8 taskId)
{
    u8 i;

    for (i = 0; i < 8; i++)
        gTasks[taskId].data[i] = gBattleAnimArgs[i];

    gTasks[taskId].func = StatsChangeAnimation_Setup;
}

static void StatsChangeAnimation_Setup(u8 taskId)
{
    int i;
    u8 battler1, battler2;
    u16 species;
    u8 animSpriteId1, animSpriteId2;
    u16 hidBattler2;
    struct BattleAnimBgData animBg;
    s16 taskData[8];

    animSpriteId2 = 0;
    hidBattler2 = FALSE;

    for (i = 0; i < 8; i++)
    {
        taskData[i] = gTasks[taskId].data[i]; 
        gTasks[taskId].data[i] = 0;
    }

    if (aIsTarget == FALSE)
        battler1 = gBattleAnimAttacker;
    else
        battler1 = gBattleAnimTarget;

    battler2 = BATTLE_PARTNER(battler1);
    if (IsContest() || (aMultipleBattlers && !IsBattlerSpriteVisible(battler2)))
        aMultipleBattlers = FALSE;

    gBattle_WIN0H = 0;
    gBattle_WIN0V = 0;
    REG_WININ = WININ_WIN0_BG_ALL | WININ_WIN0_OBJ | WININ_WIN0_CLR | WININ_WIN1_BG_ALL | WININ_WIN1_OBJ | WININ_WIN1_CLR;
    REG_WINOUT = WINOUT_WIN01_BG0 | WINOUT_WIN01_BG2 | WINOUT_WIN01_BG3 | WINOUT_WIN01_OBJ | WINOUT_WIN01_CLR | WINOUT_WINOBJ_BG_ALL | WINOUT_WINOBJ_OBJ | WINOUT_WINOBJ_CLR;
    REG_DISPCNT |= DISPCNT_OBJWIN_ON;
    REG_BLDCNT = BLDCNT_TGT1_BG1 | BLDCNT_TGT2_ALL | BLDCNT_EFFECT_BLEND;
    REG_BLDALPHA = BLDALPHA_BLEND(0, 16);
    REG_BG1CNT_BITFIELD.priority = 0;
    REG_BG1CNT_BITFIELD.screenSize = 0;
    REG_BG1CNT_BITFIELD.areaOverflowMode = 1;
    if (!IsContest())
        REG_BG1CNT_BITFIELD.charBaseBlock = 1;

    if (IsDoubleBattle() && aMultipleBattlers == FALSE)
    {
        if (GetBattlerPosition(battler1) == B_POSITION_OPPONENT_RIGHT
         || GetBattlerPosition(battler1) == B_POSITION_PLAYER_LEFT)
        {
            if (IsBattlerSpriteVisible(battler2) == TRUE)
            {
                gSprites[gBattlerSpriteIds[battler2]].oam.priority -= 1;
                REG_BG1CNT_BITFIELD.priority = 1;
                hidBattler2 = TRUE;
            }
        }
    }

    if (IsContest())
    {
        species = gContestResources__moveAnim.species;
    }
    else
    {
        if (GetBattlerSide(battler1) != B_SIDE_PLAYER)
            species = GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler1]], MON_DATA_SPECIES);
        else
            species = GetMonData(&gPlayerParty[gBattlerPartyIndexes[battler1]], MON_DATA_SPECIES);
    }

    animSpriteId1 = CreateInvisibleSpriteCopy(battler1, gBattlerSpriteIds[battler1], species);
    if (aMultipleBattlers)
        animSpriteId2 = CreateInvisibleSpriteCopy(battler2, gBattlerSpriteIds[battler2], species);
    
    GetBattleAnimBg1Data(&animBg);
    if (aDecrease == FALSE)
        LZDecompressVram(&gStatAnim_Increase_Tilemap, animBg.bgTilemap);
    else
        LZDecompressVram(&gStatAnim_Decrease_Tilemap, animBg.bgTilemap);

    if (IsContest())
        RelocateBattleBgPal(animBg.paletteId, (u16 *)animBg.bgTilemap, 0, FALSE);

    LZDecompressVram(&gStatAnim_Gfx, animBg.bgTiles);

    if (aAnimStatId == STAT_ANIM_PAL_ATK)
        LoadCompressedPalette(gStatAnim_Attack_Pal, animBg.paletteId << 4, 32);
    else if (aAnimStatId == STAT_ANIM_PAL_DEF)
        LoadCompressedPalette(gStatAnim_Defense_Pal, animBg.paletteId << 4, 32);
    else if (aAnimStatId == STAT_ANIM_PAL_ACC)
        LoadCompressedPalette(gStatAnim_Accuracy_Pal, animBg.paletteId << 4, 32);
    else if (aAnimStatId == STAT_ANIM_PAL_SPEED)
        LoadCompressedPalette(gStatAnim_Speed_Pal, animBg.paletteId << 4, 32);
    else if (aAnimStatId == STAT_ANIM_PAL_EVASION)
        LoadCompressedPalette(gStatAnim_Evasion_Pal, animBg.paletteId << 4, 32);
    else if (aAnimStatId == STAT_ANIM_PAL_SPATK)
        LoadCompressedPalette(gStatAnim_SpAttack_Pal, animBg.paletteId << 4, 32);
    else if (aAnimStatId == STAT_ANIM_PAL_SPDEF)
        LoadCompressedPalette(gStatAnim_SpDefense_Pal, animBg.paletteId << 4, 32);
    else
        LoadCompressedPalette(gStatAnim_Multiple_Pal, animBg.paletteId << 4, 32);

    gBattle_BG1_X = 0;
    gBattle_BG1_Y = 0;

    if (aDecrease == TRUE)
    {
        gBattle_BG1_X = 64;
        gTasks[taskId].tVelocity = -3;
    }
    else
    {
        gTasks[taskId].tVelocity = 3;
    }

    if (aSharply == FALSE)
    {
        gTasks[taskId].tTargetBlend = 10;
        gTasks[taskId].tWaitTime = 20;
    }
    else
    {
        gTasks[taskId].tTargetBlend = 13;
        gTasks[taskId].tWaitTime = 30;
    }

    gTasks[taskId].tAnimSpriteId1 = animSpriteId1;
    gTasks[taskId].tMultipleBattlers = aMultipleBattlers;
    gTasks[taskId].tAnimSpriteId2 = animSpriteId2;
    gTasks[taskId].tHidBattler2 = hidBattler2;
    gTasks[taskId].tBattler2SpriteId = gBattlerSpriteIds[battler2];
    gTasks[taskId].func = StatsChangeAnimation_Step3;

    if (aDecrease == FALSE)
        PlaySE12WithPanning(SE_M_STAT_INCREASE, BattleAnimAdjustPanning2(SOUND_PAN_ATTACKER));
    else
        PlaySE12WithPanning(SE_M_STAT_DECREASE, BattleAnimAdjustPanning2(SOUND_PAN_ATTACKER));
}

static void StatsChangeAnimation_Step3(u8 taskId)
{
    gBattle_BG1_Y += gTasks[taskId].tVelocity;

    switch (gTasks[taskId].tState)
    {
    case 0:
        if (gTasks[taskId].tFadeTimer++ > 0)
        {
            gTasks[taskId].tFadeTimer = 0;
            gTasks[taskId].tBlend++;
            REG_BLDALPHA = BLDALPHA_BLEND(gTasks[taskId].tBlend, 16 - gTasks[taskId].tBlend);
            if (gTasks[taskId].tBlend == gTasks[taskId].tTargetBlend)
                gTasks[taskId].tState++;
        }
        break;
    case 1:
        if (++gTasks[taskId].tWaitTimer == gTasks[taskId].tWaitTime)
            gTasks[taskId].tState++;
        break;
    case 2:
        if (gTasks[taskId].tFadeTimer++ > 0)
        {
            gTasks[taskId].tFadeTimer = 0;
            gTasks[taskId].tBlend--;
            REG_BLDALPHA = BLDALPHA_BLEND(gTasks[taskId].tBlend, 16 - gTasks[taskId].tBlend);
            if (gTasks[taskId].tBlend == 0)
            {
                ResetBattleAnimBg(FALSE);
                gBattle_WIN0H = 0;
                gBattle_WIN0V = 0;
                REG_WININ = WININ_WIN0_BG_ALL | WININ_WIN0_OBJ | WININ_WIN0_CLR | WININ_WIN1_BG_ALL | WININ_WIN1_OBJ | WININ_WIN1_CLR;
                REG_WINOUT = WINOUT_WIN01_BG_ALL | WINOUT_WIN01_OBJ | WINOUT_WIN01_CLR | WINOUT_WINOBJ_BG_ALL | WINOUT_WINOBJ_OBJ | WINOUT_WINOBJ_CLR;

                if (!IsContest())
                    REG_BG1CNT_BITFIELD.charBaseBlock = 0;

                REG_DISPCNT ^= DISPCNT_OBJWIN_ON;
                REG_BLDCNT = 0;
                REG_BLDALPHA = 0;
                DestroySprite(&gSprites[gTasks[taskId].tAnimSpriteId1]);
                if (gTasks[taskId].tMultipleBattlers)
                    DestroySprite(&gSprites[gTasks[taskId].tAnimSpriteId2]);

                if (gTasks[taskId].tHidBattler2 == TRUE)
                    gSprites[gTasks[taskId].tBattler2SpriteId].oam.priority++;

                REG_BG1CNT_BITFIELD.areaOverflowMode = 0;
                DestroyAnimVisualTask(taskId);
            }
        }
        break;
    }
}

#undef aDecrease
#undef aAnimStatId
#undef aIsTarget
#undef aMultipleBattlers
#undef aSharply
#undef tAnimSpriteId1
#undef tVelocity
#undef tMultipleBattlers
#undef tAnimSpriteId2
#undef tTargetBlend
#undef tWaitTime
#undef tHidBattler2
#undef tBattler2SpriteId
#undef tWaitTimer
#undef tFadeTimer
#undef tBlend
#undef tState

void AnimTask_Flash(u8 taskId)
{
    u32 selectedPalettes = GetBattleMonSpritePalettesMask(TRUE, TRUE, TRUE, TRUE);

    SetPalettesToColor(selectedPalettes, RGB_BLACK);
    gTasks[taskId].data[14] = selectedPalettes >> 16;

    selectedPalettes = GetBattlePalettesMask(TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE) & 0xFFFF;
    SetPalettesToColor(selectedPalettes, RGB_WHITEALPHA);
    gTasks[taskId].data[15] = selectedPalettes;

    gTasks[taskId].data[0] = 0;
    gTasks[taskId].data[1] = 0;
    gTasks[taskId].func = AnimTask_Flash_Step;
}

static void AnimTask_Flash_Step(u8 taskId)
{
    u16 i;
    struct Task *task = &gTasks[taskId];

    switch (task->data[0])
    {
    case 0:
        if (++task->data[1] > 6)
        {
            task->data[1] = 0;
            task->data[2] = 16;
            task->data[0]++;
        }
        break;
    case 1:
        if (++task->data[1] > 1)
        {
            task->data[1] = 0;
            task->data[2]--;

            for (i = 0; i < 16; i++)
            {
                if ((task->data[15] >> i) & 1)
                {
                    u16 paletteOffset = BG_PLTT_ID(i);
                    BlendPalette(paletteOffset, 16, task->data[2], RGB_WHITEALPHA);
                }

                if ((task->data[14] >> i) & 1)
                {
                    u16 paletteOffset = OBJ_PLTT_ID(i);
                    BlendPalette(paletteOffset, 16, task->data[2], RGB_BLACK);
                }
            }

            if (task->data[2] == 0)
                task->data[0]++;
        }
        break;
    case 2:
        DestroyAnimVisualTask(taskId);
        break;
    }
}

static void SetPalettesToColor(u32 selectedPalettes, u16 color)
{
    u16 i;

    for (i = 0; i < 32; i++)
    {
        if (selectedPalettes & 1)
        {
            u16 curOffset = i * 16;
            u16 paletteOffset = curOffset;
            while (curOffset < paletteOffset + 16)
            {
                gPlttBufferFaded[curOffset] = color;
                curOffset++;
            }
        }

        selectedPalettes >>= 1;
    }
}

void AnimTask_BlendNonAttackerPalettes(u8 taskId)
{
    u32 battler;
    int j;
    u32 selectedPalettes = 0;

    for (battler = 0; battler < MAX_BATTLERS_COUNT; battler++)
    {
        if (gBattleAnimAttacker != battler)
            selectedPalettes |= 1 << (battler + 16);
    }

    for (j = 5; j != 0; j--)
        gBattleAnimArgs[j] = gBattleAnimArgs[j - 1];

    StartBlendAnimSpriteColor(taskId, selectedPalettes);
}

void AnimTask_StartSlidingBg(u8 taskId)
{
    CMD_ARGS(xSpeed, ySpeed, reverseForOpponent, completionValue);
    u8 newTaskId;

    UpdateAnimBg3ScreenSize(FALSE);
    newTaskId = CreateTask(AnimTask_UpdateSlidingBg, 5);
    if (cmd->reverseForOpponent && GetBattlerSide(gBattleAnimAttacker) != B_SIDE_PLAYER)
    {
        cmd->xSpeed = -cmd->xSpeed;
        cmd->ySpeed = -cmd->ySpeed;
    }

    gTasks[newTaskId].data[1] = cmd->xSpeed;
    gTasks[newTaskId].data[2] = cmd->ySpeed;
    gTasks[newTaskId].data[3] = cmd->completionValue;
    gTasks[newTaskId].data[0]++;
    DestroyAnimVisualTask(taskId);
}

static void AnimTask_UpdateSlidingBg(u8 taskId)
{
    gTasks[taskId].data[10] += gTasks[taskId].data[1];
    gTasks[taskId].data[11] += gTasks[taskId].data[2];
    gBattle_BG3_X += gTasks[taskId].data[10] >> 8;
    gBattle_BG3_Y += gTasks[taskId].data[11] >> 8;
    gTasks[taskId].data[10] &= 0xFF;
    gTasks[taskId].data[11] &= 0xFF;

    if (gBattleAnimArgs[ARG_RET_ID] == gTasks[taskId].data[3])
    {
        gBattle_BG3_X = 0;
        gBattle_BG3_Y = 0;
        UpdateAnimBg3ScreenSize(TRUE);
        DestroyTask(taskId);
    }
}

void AnimTask_GetAttackerSide(u8 taskId)
{
    gBattleAnimArgs[ARG_RET_ID] = GetBattlerSide(gBattleAnimAttacker);
    DestroyAnimVisualTask(taskId);
}

void AnimTask_GetTargetSide(u8 taskId)
{
    gBattleAnimArgs[ARG_RET_ID] = GetBattlerSide(gBattleAnimTarget);
    DestroyAnimVisualTask(taskId);
}

void AnimTask_GetTargetIsAttackerPartner(u8 taskId)
{
    gBattleAnimArgs[ARG_RET_ID] = BATTLE_PARTNER(gBattleAnimAttacker) == gBattleAnimTarget;
    DestroyAnimVisualTask(taskId);
}

// For hiding or subsequently revealing all other battlers.
void AnimTask_SetAllNonAttackersInvisiblity(u8 taskId)
{
    CMD_ARGS(invisible);
    u16 battler;

    for (battler = 0; battler < MAX_BATTLERS_COUNT; battler++)
    {
        if (battler != gBattleAnimAttacker && IsBattlerSpriteVisible(battler))
            gSprites[gBattlerSpriteIds[battler]].invisible = cmd->invisible;
    }

    DestroyAnimVisualTask(taskId);
}

void StartMonScrollingBgMask(u8 taskId, int unused, u16 scrollSpeed, u8 battler, u8 includePartner, u8 numFadeSteps, u8 fadeStepDelay, u8 duration, const u8 *gfx, const u8 *tilemap, const u16 *palette)
{
    u16 species;
    u8 spriteId, spriteId2;
    struct BattleAnimBgData animBg;
    u8 battler2;

    spriteId2 = 0;
    battler2 = BATTLE_PARTNER(battler);

    if (IsContest() || (includePartner && !IsBattlerSpriteVisible(battler2)))
        includePartner = FALSE;

    gBattle_WIN0H = 0;
    gBattle_WIN0V = 0;
    REG_WININ = WININ_WIN0_BG_ALL | WININ_WIN0_OBJ | WININ_WIN0_CLR | WININ_WIN1_BG_ALL | WININ_WIN1_OBJ | WININ_WIN1_CLR;
    REG_WINOUT = WINOUT_WIN01_BG0 | WINOUT_WIN01_BG2 | WINOUT_WIN01_BG3 | WINOUT_WIN01_OBJ | WINOUT_WIN01_CLR | WINOUT_WINOBJ_BG_ALL | WINOUT_WINOBJ_OBJ | WINOUT_WINOBJ_CLR;
    REG_DISPCNT |= DISPCNT_OBJWIN_ON;
    REG_BLDCNT = BLDCNT_TGT1_BG1 | BLDCNT_TGT2_ALL | BLDCNT_EFFECT_BLEND;
    REG_BLDALPHA = BLDALPHA_BLEND(0, 16);
    REG_BG1CNT_BITFIELD.priority = 0;
    REG_BG1CNT_BITFIELD.screenSize = 0;
    REG_BG1CNT_BITFIELD.areaOverflowMode = 1;
    if (!IsContest())
        REG_BG1CNT_BITFIELD.charBaseBlock = 1;

    if (IsContest())
    {
        species = gContestResources__moveAnim.species;
    }
    else
    {
        if (GetBattlerSide(battler) != B_SIDE_PLAYER)
            species = GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_SPECIES);
        else
            species = GetMonData(&gPlayerParty[gBattlerPartyIndexes[battler]], MON_DATA_SPECIES);
    }

    spriteId = CreateInvisibleSpriteCopy(battler, gBattlerSpriteIds[battler], species);
    if (includePartner)
        spriteId2 = CreateInvisibleSpriteCopy(battler2, gBattlerSpriteIds[battler2], species);

    GetBattleAnimBg1Data(&animBg);
    LZDecompressVram(tilemap, animBg.bgTilemap);
    if (IsContest())
        RelocateBattleBgPal(animBg.paletteId, (u16 *)animBg.bgTilemap, 0, FALSE);

    LZDecompressVram(gfx, animBg.bgTiles);
    LoadCompressedPalette(palette, animBg.paletteId << 4, 32);
    gBattle_BG1_X = 0;
    gBattle_BG1_Y = 0;
    gTasks[taskId].data[1] = scrollSpeed;
    gTasks[taskId].data[4] = numFadeSteps;
    gTasks[taskId].data[5] = duration;
    gTasks[taskId].data[6] = fadeStepDelay;
    gTasks[taskId].data[0] = spriteId;
    gTasks[taskId].data[2] = includePartner;
    gTasks[taskId].data[3] = spriteId2;
    gTasks[taskId].func = UpdateMonScrollingBgMask;
}

static void UpdateMonScrollingBgMask(u8 taskId)
{
    gTasks[taskId].data[13] += gTasks[taskId].data[1] < 0 ? -gTasks[taskId].data[1] : gTasks[taskId].data[1];
    if (gTasks[taskId].data[1] < 0)
        gBattle_BG1_Y -= gTasks[taskId].data[13] >> 8;
    else
        gBattle_BG1_Y += gTasks[taskId].data[13] >> 8;

    gTasks[taskId].data[13] &= 0xFF;
    switch (gTasks[taskId].data[15])
    {
    case 0:
        if (gTasks[taskId].data[11]++ >= gTasks[taskId].data[6])
        {
            gTasks[taskId].data[11] = 0;
            gTasks[taskId].data[12]++;
            REG_BLDALPHA = BLDALPHA_BLEND(gTasks[taskId].data[12], 16 - gTasks[taskId].data[12]);
            if (gTasks[taskId].data[12] == gTasks[taskId].data[4])
                gTasks[taskId].data[15]++;
        }
        break;
    case 1:
        if (++gTasks[taskId].data[10] == gTasks[taskId].data[5])
            gTasks[taskId].data[15]++;
        break;
    case 2:
        if (gTasks[taskId].data[11]++ >= gTasks[taskId].data[6])
        {
            gTasks[taskId].data[11] = 0;
            gTasks[taskId].data[12]--;
            REG_BLDALPHA = BLDALPHA_BLEND(gTasks[taskId].data[12], 16 - gTasks[taskId].data[12]);
            if (gTasks[taskId].data[12] == 0)
            {
                ResetBattleAnimBg(FALSE);
                gBattle_WIN0H = 0;
                gBattle_WIN0V = 0;
                REG_WININ = WININ_WIN0_BG_ALL | WININ_WIN0_OBJ | WININ_WIN0_CLR | WININ_WIN1_BG_ALL | WININ_WIN1_OBJ | WININ_WIN1_CLR;
                REG_WINOUT = WINOUT_WIN01_BG_ALL | WINOUT_WIN01_OBJ | WINOUT_WIN01_CLR | WINOUT_WINOBJ_BG_ALL | WINOUT_WINOBJ_OBJ | WINOUT_WINOBJ_CLR;

                if (!IsContest())
                    REG_BG1CNT_BITFIELD.charBaseBlock = 0;

                REG_DISPCNT ^= DISPCNT_OBJWIN_ON;
                REG_BLDCNT = 0;
                REG_BLDALPHA = 0;
                DestroySprite(&gSprites[gTasks[taskId].data[0]]);
                if (gTasks[taskId].data[2])
                    DestroySprite(&gSprites[gTasks[taskId].data[3]]);

                REG_BG1CNT_BITFIELD.areaOverflowMode = 0;
                DestroyAnimVisualTask(taskId);
            }
        }
        break;
    }
}

void AnimTask_GetBattleEnvironment(u8 taskId)
{
    gBattleAnimArgs[0] = gBattleEnvironment;
    DestroyAnimVisualTask(taskId);
}

void AnimTask_CopyPalUnfadedToBackup(u8 taskId)
{
    CMD_ARGS(paletteType, backupSlot);
    u32 selectedPalettes;
    u8 *dest;
    int offset;
    int i = 0;

    if (cmd->paletteType == 0)
    {
        selectedPalettes = GetBattlePalettesMask(TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE);
        while ((selectedPalettes & 1) == 0)
        {
            selectedPalettes >>= 1;
            i++;
        }
    }
    else if (cmd->paletteType == 1)
    {
        i = gBattleAnimAttacker + 16;
    }
    else if (cmd->paletteType == 2)
    {
        i = gBattleAnimTarget + 16;
    }

    offset = cmd->backupSlot * 32;
    dest = IsContest() ? &((u8 *)eBattleAnimPalBackup_Contest)[offset] : &((u8 *)eBattleAnimPalBackup_Battle)[offset];
    // This doesn't match when u16* is used.
    memcpy(dest, &((u8 *)gPlttBufferUnfaded)[i * 32], 32);
    DestroyAnimVisualTask(taskId);
}


void AnimTask_CopyPalUnfadedFromBackup(u8 taskId)
{
    CMD_ARGS(paletteType, backupSlot);
    u32 selectedPalettes;
    u8 *src;
    u8 *dest;
    int offset;
    int i = 0;

    if (cmd->paletteType == 0)
    {
        selectedPalettes = GetBattlePalettesMask(TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE);
        while ((selectedPalettes & 1) == 0)
        {
            selectedPalettes >>= 1;
            i++;
        }
    }
    else if (cmd->paletteType == 1)
    {
        i = gBattleAnimAttacker + 16;
    }
    else if (cmd->paletteType == 2)
    {
        i = gBattleAnimTarget + 16;
    }

    dest = &((u8 *)gPlttBufferUnfaded)[i * 32];
    offset = cmd->backupSlot * 32;
    src = IsContest() ? &((u8 *)eBattleAnimPalBackup_Contest)[offset] : &((u8 *)eBattleAnimPalBackup_Battle)[offset];
    // This doesn't match when u16* is used.
    memcpy(dest, src, 32);
    DestroyAnimVisualTask(taskId);
}

void AnimTask_CopyPalFadedToUnfaded(u8 taskId)
{
    CMD_ARGS(paletteType);
    u32 selectedPalettes;
    int i = 0;

    if (cmd->paletteType == 0)
    {
        selectedPalettes = GetBattlePalettesMask(TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE);
        while ((selectedPalettes & 1) == 0)
        {
            selectedPalettes >>= 1;
            i++;
        }
    }
    else if (cmd->paletteType == 1)
    {
        i = gBattleAnimAttacker + 16;
    }
    else if (cmd->paletteType == 2)
    {
        i = gBattleAnimTarget + 16;
    }

    memcpy(&gPlttBufferUnfaded[i * 16], &gPlttBufferFaded[i * 16], 32);
    DestroyAnimVisualTask(taskId);
}

void AnimTask_IsContest(u8 taskId)
{
    if (IsContest())
        gBattleAnimArgs[ARG_RET_ID] = TRUE;
    else
        gBattleAnimArgs[ARG_RET_ID] = FALSE;

    DestroyAnimVisualTask(taskId);
}

void AnimTask_SetAnimAttackerAndTargetForEffectTgt(u8 taskId)
{
    gBattleAnimAttacker = gBattlerTarget;
    gBattleAnimTarget = gEffectBattler;
    DestroyAnimVisualTask(taskId);
}

void AnimTask_IsTargetSameSide(u8 taskId)
{
    if (GetBattlerSide(gBattleAnimAttacker) == GetBattlerSide(gBattleAnimTarget))
        gBattleAnimArgs[ARG_RET_ID] = TRUE;
    else
        gBattleAnimArgs[ARG_RET_ID] = FALSE;

    DestroyAnimVisualTask(taskId);
}

void AnimTask_SetAnimTargetToBattlerTarget(u8 taskId)
{
    gBattleAnimTarget = gBattlerTarget;
    DestroyAnimVisualTask(taskId);
}

void AnimTask_SetAnimAttackerAndTargetForEffectAtk(u8 taskId)
{
    gBattleAnimAttacker = gBattlerAttacker;
    gBattleAnimTarget = gEffectBattler;
    DestroyAnimVisualTask(taskId);
}

void AnimTask_SetAttackerInvisibleWaitForSignal(u8 taskId)
{
    if (IsContest())
    {
        DestroyAnimVisualTask(taskId);
    }
    else
    {
        gTasks[taskId].data[0] = gBattleSpriteInfo[gBattleAnimAttacker].invisible;
        gBattleSpriteInfo[gBattleAnimAttacker].invisible = TRUE;
        gTasks[taskId].func = AnimTask_WaitAndRestoreVisibility;
        gAnimVisualTaskCount--;
    }
}

static void AnimTask_WaitAndRestoreVisibility(u8 taskId)
{
    if (gBattleAnimArgs[ARG_RET_ID] == 0x1000)
    {
        gBattleSpriteInfo[gBattleAnimAttacker].invisible = gTasks[taskId].data[0] & 1;
        DestroyTask(taskId);
    }
}
