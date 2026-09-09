#include "global.h"
#include "battle.h"
#include "decompress.h"
#include "graphics.h"
#include "battle_anim.h"
#include "random.h"
#include "rom_8077ABC.h"
#include "sprite.h"
#include "trig.h"
#include "util.h"
#include "scanline_effect.h"
#include "palette.h"
#include "constants/battle_anim.h"

extern u8 gAnimVisualTaskCount;
extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;
extern u16 gBattlerPartyIndexes[];
extern const struct SpriteTemplate gWaterHitSplatSpriteTemplate;

extern const union AnimCmd *const gAnims_SmallBubblePair[];

void PrepareBattlerSpriteForRotScale(u8, u8);
void ResetSpriteRotScale(u8);
void SetBattlerSpriteYOffsetFromYScale(u8);
static void AnimWaterGunDroplet(struct Sprite *sprite);
void AnimSmallBubblePair(struct Sprite *sprite);
static void AnimTask_CreateSurfWave_Step1(u8 taskId);
static void AnimTask_SurfWaveScanlineEffect(u8 taskId);
void AnimSmallDriftingBubbles(struct Sprite *sprite);
void AnimSmallDriftingBubbles_Step(struct Sprite *);
static void AnimTask_WaterSpoutLaunch_Step(u8);
static u8 GetWaterSpoutPowerForAnim(void);
static void CreateWaterSpoutLaunchDroplets(struct Task *, u8);
static void AnimSmallWaterOrb(struct Sprite *sprite);
static void AnimTask_WaterSpoutRain_Step(u8);
static void CreateWaterSpoutRainDroplet(struct Task *, u8);
static void AnimWaterSpoutRain(struct Sprite *);
static void AnimWaterSpoutRainHit(struct Sprite *);
static void AnimTask_WaterSport_Step(u8);
static void CreateWaterSportDroplet(struct Task *);
static void AnimWaterSportDroplet(struct Sprite *);
static void AnimWaterSportDroplet_Step(struct Sprite *);
void sub_80D4BF0(struct Sprite *sprite);
void sub_80D4C18(struct Sprite *);
void sub_80D4CEC(struct Sprite *);
void sub_80D4C64(struct Sprite *sprite);
void sub_80D4D64(struct Sprite*, s32, s32);
void AnimTask_HorizontalShake(u8);
static void AnimSmallBubblePair_Step(struct Sprite *sprite);

static const union AnimCmd sAnim_WaterBubble[] =
{
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_WaterGunDroplet[] =
{
    ANIMCMD_FRAME(4, 1),
    ANIMCMD_END,
};

const union AnimCmd *const gAnims_WaterBubble[] =
{
    sAnim_WaterBubble,
};

static const union AnimCmd *const sAnims_WaterGunDroplet[] =
{
    sAnim_WaterGunDroplet,
};

const struct SpriteTemplate gWaterGunProjectileSpriteTemplate =
{
    .tileTag = ANIM_TAG_SMALL_BUBBLES,
    .paletteTag = ANIM_TAG_SMALL_BUBBLES,
    .oam = &gOamData_AffineOff_ObjBlend_16x16,
    .anims = gAnims_WaterBubble,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimThrowProjectile,
};

const struct SpriteTemplate gWaterGunDropletSpriteTemplate =
{
    .tileTag = ANIM_TAG_SMALL_BUBBLES,
    .paletteTag = ANIM_TAG_SMALL_BUBBLES,
    .oam = &gOamData_AffineDouble_ObjBlend_16x16,
    .anims = sAnims_WaterGunDroplet,
    .images = NULL,
    .affineAnims = gAffineAnims_Droplet,
    .callback = AnimWaterGunDroplet,
};

const struct SpriteTemplate gSmallBubblePairSpriteTemplate =
{
    .tileTag = ANIM_TAG_ICE_CRYSTALS,
    .paletteTag = ANIM_TAG_ICE_CRYSTALS,
    .oam = &gOamData_AffineOff_ObjNormal_8x8,
    .anims = gAnims_SmallBubblePair,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimSmallBubblePair,
};

const struct SpriteTemplate gSmallDriftingBubblesSpriteTemplate =
{
    .tileTag = ANIM_TAG_SMALL_BUBBLES,
    .paletteTag = ANIM_TAG_SMALL_BUBBLES,
    .oam = &gOamData_AffineOff_ObjNormal_8x8,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimSmallDriftingBubbles,
};

// Used by Water Spout / Water Sport
const struct SpriteTemplate gSmallWaterOrbSpriteTemplate =
{
    .tileTag = ANIM_TAG_GLOWY_BLUE_ORB,
    .paletteTag = ANIM_TAG_GLOWY_BLUE_ORB,
    .oam = &gOamData_AffineOff_ObjNormal_8x8,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimSmallWaterOrb,
};

const union AnimCmd gSpriteAnim_83D9390[] =
{
    ANIMCMD_FRAME(8, 1),
    ANIMCMD_END,
};

const union AnimCmd gSpriteAnim_83D9398[] =
{
    ANIMCMD_FRAME(9, 1),
    ANIMCMD_END,
};

const union AnimCmd gSpriteAnim_83D93A0[] =
{
    ANIMCMD_FRAME(4, 1),
    ANIMCMD_END,
};

const union AnimCmd *const gSpriteAnimTable_83D93A8[] =
{
    gSpriteAnim_83D9390,
    gSpriteAnim_83D9398,
};

const union AnimCmd *const gSpriteAnimTable_83D93B0[] =
{
    gSpriteAnim_83D93A0,
};

const union AffineAnimCmd gSpriteAffineAnim_83D93B4[] =
{
    AFFINEANIMCMD_FRAME(0x100, 0x100, 0, 0),
    AFFINEANIMCMD_FRAME(0xFFF6, 0xFFF6, 0, 15),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd gSpriteAffineAnim_83D93CC[] =
{
    AFFINEANIMCMD_FRAME(0xE0, 0xE0, 0, 0),
    AFFINEANIMCMD_FRAME(0xFFF8, 0xFFF8, 0, 15),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd gSpriteAffineAnim_83D93E4[] =
{
    AFFINEANIMCMD_FRAME(0x150, 0x150, 0, 0),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 0, 15),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd *const gSpriteAffineAnimTable_83D93FC[] =
{
    gSpriteAffineAnim_83D93B4,
    gSpriteAffineAnim_83D93CC,
};

const union AffineAnimCmd *const gSpriteAffineAnimTable_83D9404[] =
{
    gSpriteAffineAnim_83D93E4,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83D9408 =
{
    .tileTag = ANIM_TAG_SMALL_BUBBLES,
    .paletteTag = ANIM_TAG_SMALL_BUBBLES,
    .oam = &gOamData_AffineOff_ObjNormal_8x8,
    .anims = gSpriteAnimTable_83D93A8,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80D4BF0,
};

const struct SpriteTemplate gSpriteTemplate_83D9420 =
{
    .tileTag = ANIM_TAG_SMALL_BUBBLES,
    .paletteTag = ANIM_TAG_SMALL_BUBBLES,
    .oam = &gOamData_837DF84,
    .anims = gSpriteAnimTable_83D93A8,
    .images = NULL,
    .affineAnims = gSpriteAffineAnimTable_83D93FC,
    .callback = sub_80D4C64,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83D9438 =
{
    .tileTag = ANIM_TAG_SMALL_BUBBLES,
    .paletteTag = ANIM_TAG_SMALL_BUBBLES,
    .oam = &gOamData_AffineNormal_ObjNormal_16x16,
    .anims = gSpriteAnimTable_83D93B0,
    .images = NULL,
    .affineAnims = gSpriteAffineAnimTable_83D9404,
    .callback = sub_807A9BC,
};

static void AnimWaterGunDroplet(struct Sprite *sprite)
{
    InitSpritePosToAnimTarget(sprite, TRUE);

    sprite->data[0] = gBattleAnimArgs[4];
    sprite->data[2] = sprite->x + gBattleAnimArgs[2];
    sprite->data[4] = sprite->y + gBattleAnimArgs[4];

    sprite->callback = StartAnimLinearTranslation;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}

void AnimSmallBubblePair(struct Sprite *sprite)
{
    if (gBattleAnimArgs[3] != ANIM_BATTLER_ATTACKER)
    {
        InitSpritePosToAnimTarget(sprite, TRUE);
    }
    else
    {
        InitSpritePosToAnimAttacker(sprite, 1);
    }

    sprite->data[7] = gBattleAnimArgs[2];
    sprite->callback = AnimSmallBubblePair_Step;
}

static void AnimSmallBubblePair_Step(struct Sprite *sprite)
{
    sprite->data[0] = (sprite->data[0] + 11) & 0xFF;
    sprite->x2 = Sin(sprite->data[0], 4);

    sprite->data[1] += 48;
    sprite->y2 = -(sprite->data[1] >> 8);

    if (--sprite->data[7] == -1)
    {
        DestroyAnimSprite(sprite);
    }
}

void AnimTask_CreateSurfWave(u8 taskId)
{
    struct BattleAnimBgData animBg;
    u8 taskId2;
    u16 *x = &gBattle_BG1_X;
    u16 *y = &gBattle_BG1_Y;
    vu8 cpuDelay; // explanation below

    REG_BLDCNT = BLDCNT_TGT1_BG1 | BLDCNT_EFFECT_BLEND | BLDCNT_TGT2_ALL;
    REG_BLDALPHA = BLDALPHA_BLEND(0, 16);
    REG_BG1CNT_BITFIELD.priority = 1;
    REG_BG1CNT_BITFIELD.screenSize = 1;
    GetBattleAnimBg1Data(&animBg);

    // This is gone in FireRed and Emerald.
    Dma3FillLarge32_(0, animBg.bgTiles, 0x2000); // !
    /*
        Many games use wasteful NOPs; some of which are
        even moreso than regular ones. This is so that
        hardware operations can finish.

        This is just an example. Also, this is apparently
        not a macro, as making it a macro results in a
        NONMATCHING.
    */
    cpuDelay = 0; // stall the CPU
    cpuDelay = 0; // stall the CPU
    Dma3FillLarge32_(0, animBg.bgTilemap, 0x1000); // !

    if (!IsContest())
    {
        REG_BG1CNT_BITFIELD.charBaseBlock = 1;
        if (GetBattlerSide(gBattleAnimAttacker) == B_SIDE_OPPONENT)
            LZDecompressVram(&gBattleAnimBgTilemap_SurfOpponent, animBg.bgTilemap);
        else
            LZDecompressVram(&gBattleAnimBgTilemap_SurfPlayer, animBg.bgTilemap);
    }
    else
    {
        LZDecompressVram(&gBattleAnimBgTilemap_SurfContest, animBg.bgTilemap);
        sub_80763FC(animBg.paletteId, (u16 *)animBg.bgTilemap, 0, 1);
    }
    LZDecompressVram(&gBattleAnimBgImage_Surf, animBg.bgTiles);
    if (gBattleAnimArgs[0] == ANIM_SURF_PAL_SURF)
        LoadCompressedPalette(&gBattleAnimBgPalette_Surf, BG_PLTT_ID(animBg.paletteId), PLTT_SIZE_4BPP);
    else
        LoadCompressedPalette(&gBattleAnimBackgroundImageMuddyWater_Pal, BG_PLTT_ID(animBg.paletteId), PLTT_SIZE_4BPP);
    taskId2 = CreateTask(AnimTask_SurfWaveScanlineEffect, gTasks[taskId].priority + 1);
    gTasks[taskId].data[15] = taskId2;
    gTasks[taskId2].data[0] = 0;
    gTasks[taskId2].data[1] = 0x1000;
    gTasks[taskId2].data[2] = 0x1000;
    if (IsContest())
    {
        *x = -80;
        *y = -48;
        gTasks[taskId].data[0] = 2;
        gTasks[taskId].data[1] = 1;
        gTasks[taskId2].data[3] = 0;
    }
    else if (GetBattlerSide(gBattleAnimAttacker) == B_SIDE_OPPONENT)
    {
        *x = -224;
        *y = 256;
        gTasks[taskId].data[0] = 2;
        gTasks[taskId].data[1] = -1;
        gTasks[taskId2].data[3] = 1;
    }
    else
    {
        *x = 0;
        *y = -48;
        gTasks[taskId].data[0] = -2;
        gTasks[taskId].data[1] = 1;
        gTasks[taskId2].data[3] = 0;
    }
    REG_BG1HOFS = *x;
    REG_BG1VOFS = *y;
    if(gTasks[taskId2].data[3] == 0)
    {
        gTasks[taskId2].data[4] = 48;
        gTasks[taskId2].data[5] = 112;
    }
    else
    {
        gTasks[taskId2].data[4] = 0;
        gTasks[taskId2].data[5] = 0;
    }
    gTasks[taskId].data[6] = 1;
    gTasks[taskId].func = AnimTask_CreateSurfWave_Step1;
}

static void AnimTask_CreateSurfWave_Step1(u8 taskId)
{

    vu8 cpuDelay; // yet again
    struct BattleAnimBgData animBg;
    u8 i;
    u16 rgbBuffer;
    u16 *x = &gBattle_BG1_X;
    u16 *y = &gBattle_BG1_Y;

    *x += gTasks[taskId].data[0];
    *y += gTasks[taskId].data[1];
    GetBattleAnimBg1Data(&animBg);
    gTasks[taskId].data[2] += gTasks[taskId].data[1];
    if (++gTasks[taskId].data[5] == 4)
    {
        rgbBuffer = gPlttBufferFaded[animBg.paletteId * 16 + 7];
        for (i = 6; i != 0; i--)
        {
            gPlttBufferFaded[animBg.paletteId * 16 + 1 + i] = gPlttBufferFaded[animBg.paletteId * 16 + 1 + i - 1];
        }
        gPlttBufferFaded[animBg.paletteId * 16 + 1] = rgbBuffer;
        gTasks[taskId].data[5] = 0;
    }
    if (++gTasks[taskId].data[6] > 1)
    {
        // there is some weird math going on here
        gTasks[taskId].data[6] = 0;
        if (++gTasks[taskId].data[3] <= 13)
        {
            gTasks[gTasks[taskId].data[15]].data[1] = gTasks[taskId].data[3] | ((16 - gTasks[taskId].data[3]) << 8);
            gTasks[taskId].data[4]++;
        }
        if (gTasks[taskId].data[3] > 54)
        {
            gTasks[taskId].data[4]--;
            gTasks[gTasks[taskId].data[15]].data[1] = gTasks[taskId].data[4] | ((16 - gTasks[taskId].data[4]) << 8);
        }
    }
    if (!(gTasks[gTasks[taskId].data[15]].data[1] & 0x1F))
    {
        Dma3FillLarge32_(0, animBg.bgTiles, 0x2000); // !
        cpuDelay = 0; // stall the CPU
        cpuDelay = 0; // stall the CPU
        Dma3FillLarge32_(0, animBg.bgTilemap, 0x1000); // !
        if (!IsContest())
            REG_BG1CNT_BITFIELD.charBaseBlock = 0;
        *x = 0;
        *y = 0;

        REG_BLDCNT = 0;
        REG_BLDALPHA = BLDALPHA_BLEND(0, 0);

        gTasks[gTasks[taskId].data[15]].data[15] = 0xffff;
        DestroyAnimVisualTask(taskId);
    }
}

static void AnimTask_SurfWaveScanlineEffect(u8 taskId)
{
    s16 i;
    struct ScanlineEffectParams params;
    struct Task *task = &gTasks[taskId];

    switch (task->data[0])
    {
        case 0:
            for (i = 0; i < task->data[4]; i++)
            {
                /* variable initialization isn't literal to ASM */
                gScanlineEffectRegBuffers[0][i] = gScanlineEffectRegBuffers[1][i] = task->data[2];
            }
            for (i = task->data[4]; i < task->data[5]; i++)
            {
                gScanlineEffectRegBuffers[0][i] = gScanlineEffectRegBuffers[1][i] = task->data[1];
            }
            for (i = task->data[5]; i < 160; i++)
            {
                gScanlineEffectRegBuffers[0][i] = gScanlineEffectRegBuffers[1][i] = task->data[2];
            }
            if (task->data[4] == 0)
            {
                gScanlineEffectRegBuffers[0][i] = gScanlineEffectRegBuffers[1][i] = task->data[1];
            }
            else
            {
                gScanlineEffectRegBuffers[0][i] = gScanlineEffectRegBuffers[1][i] = task->data[2];
            }
            params.dmaDest = (vu16 *)REG_ADDR_BLDALPHA;
            params.dmaControl = SCANLINE_EFFECT_DMACNT_16BIT;
            params.initState = 1;
            params.unused9 = 0;
            ScanlineEffect_SetParams(params);
            task->data[0]++;
            break;
        case 1:
            if (task->data[3] == 0)
            {
                if (--task->data[4] <= 0)
                {
                    task->data[4] = 0;
                    task->data[0]++;
                }
            }
            else if (++task->data[5] > 111)
            {
                task->data[0]++;
            }
            for (i = 0; i < task->data[4]; i++)
            {
                gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][i] = task->data[2];
            }
            for (i = task->data[4]; i < task->data[5]; i++)
            {
                gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][i] = task->data[1];
            }
            for (i = task->data[5]; i < 160; i++)
            {
                gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][i] = task->data[2];
            }
            break;
        case 2:
            for (i = 0; i < task->data[4]; i++)
            {
                gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][i] = task->data[2];
            }
            for (i = task->data[4]; i < task->data[5]; i++)
            {
                gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][i] = task->data[1];
            }
            for (i = task->data[5]; i < 160; i++)
            {
                gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][i] = task->data[2];
            }
            if (task->data[15] == -1)
            {
                ScanlineEffect_Stop();
                DestroyTask(taskId);
            }
            break;
    }
}

void AnimSmallDriftingBubbles(struct Sprite *sprite)
{
    s16 randData;
    s16 randData2;

    sprite->oam.tileNum += 8;
    InitSpritePosToAnimTarget(sprite, TRUE);
    randData = (Random() & 0xFF) | 256;
    randData2 = (Random() & 0x1FF);
    if (randData2 > 255)
        randData2 = 256 - randData2;
    sprite->data[1] = randData;
    sprite->data[2] = randData2;
    sprite->callback = AnimSmallDriftingBubbles_Step;
}

void AnimSmallDriftingBubbles_Step(struct Sprite *sprite)
{
    sprite->data[3] += sprite->data[1];
    sprite->data[4] += sprite->data[2];
    if (sprite->data[1] & 1)
        sprite->x2 = -(sprite->data[3] >> 8);
    else
        sprite->x2 = sprite->data[3] >> 8;
    sprite->y2 = sprite->data[4] >> 8;
    if (++sprite->data[0] == 21)
        DestroyAnimSprite(sprite);
}

void AnimTask_WaterSpoutLaunch(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    task->data[15] = GetAnimBattlerSpriteId(ANIM_BATTLER_ATTACKER);
    task->data[5] = gSprites[task->data[15]].y;
    task->data[1] = GetWaterSpoutPowerForAnim();
    PrepareBattlerSpriteForRotScale(task->data[15], ST_OAM_OBJ_NORMAL);
    task->func = AnimTask_WaterSpoutLaunch_Step;
}

static void AnimTask_WaterSpoutLaunch_Step(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->data[0])
    {
        case 0:
            PrepareEruptAnimTaskData(task, task->data[15], 0x100, 0x100, 224, 0x200, 32);
            task->data[0]++;
        case 1:
            if (++task->data[3] > 1)
            {
                task->data[3] = 0;
                if (++task->data[4] & 1)
                {
                    gSprites[task->data[15]].x2 = 3;
                    gSprites[task->data[15]].y++;
                }
                else
                {
                    gSprites[task->data[15]].x2 = -3;
                }
            }
            if (UpdateEruptAnimTask(task) == 0)
            {
                SetBattlerSpriteYOffsetFromYScale(task->data[15]);
                gSprites[task->data[15]].x2 = 0;
                task->data[3] = 0;
                task->data[4] = 0;
                task->data[0]++;
            }
            break;
        case 2:
            if (++task->data[3] > 4)
            {
                PrepareEruptAnimTaskData(task, task->data[15], 224, 0x200, 384, 224, 8);
                task->data[3] = 0;
                task->data[0]++;
            }
            break;
        case 3:
            if (UpdateEruptAnimTask(task) == 0)
            {
                task->data[3] = 0;
                task->data[4] = 0;
                task->data[0]++;
            }
            break;
        case 4:
            CreateWaterSpoutLaunchDroplets(task, taskId);
            task->data[0]++;
        case 5:
            if (++task->data[3] > 1)
            {
                task->data[3] = 0;
                if (++task->data[4] & 1)
                    gSprites[task->data[15]].y2 += 2;
                else
                    gSprites[task->data[15]].y2 -= 2;
                if (task->data[4] == 10)
                {
                    PrepareEruptAnimTaskData(task, task->data[15], 384, 224, 0x100, 0x100, 8);
                    task->data[3] = 0;
                    task->data[4] = 0;
                    task->data[0]++;
                }
            }
            break;
        case 6:
            gSprites[task->data[15]].y--;
            if (UpdateEruptAnimTask(task) == 0)
            {
                ResetSpriteRotScale(task->data[15]);
                gSprites[task->data[15]].y = task->data[5];
                task->data[4] = 0;
                task->data[0]++;
            }
            break;
        case 7:
            if (task->data[2] == 0)
                DestroyAnimVisualTask(taskId);
            break;
    }
}

static u8 GetWaterSpoutPowerForAnim(void)
{
    u8 i;
    u16 hp;
    u16 maxhp;
    u16 partyIndex;
    struct Pokemon *slot;

    if (GetBattlerSide(gBattleAnimAttacker) == B_SIDE_PLAYER)
    {
        partyIndex = gBattlerPartyIndexes[gBattleAnimAttacker];
        slot =  &gPlayerParty[partyIndex];
        maxhp = GetMonData(slot, MON_DATA_MAX_HP);
        hp = GetMonData(slot, MON_DATA_HP);
        maxhp /= 4;
    }
    else
    {
        partyIndex = gBattlerPartyIndexes[gBattleAnimAttacker];
        slot =  &gEnemyParty[partyIndex];
        maxhp = GetMonData(slot, MON_DATA_MAX_HP);
        hp = GetMonData(slot, MON_DATA_HP);
        maxhp /= 4;
    }
    for (i = 0; i < 3; i++)
    {
        if (hp < maxhp * (i + 1))
            return i;
    }
    return 3;
}

static void CreateWaterSpoutLaunchDroplets(struct Task *task, u8 taskId)
{
    s16 i;
    s16 attackerCoordX = GetBattlerSpriteCoord(gBattleAnimAttacker, 2);
    s16 attackerCoordY = GetBattlerSpriteCoord(gBattleAnimAttacker, 3);
    s16 trigIndex = 172;
    u8 subpriority = GetBattlerSpriteSubpriority(gBattleAnimAttacker) - 1;
    s16 increment = 4 - task->data[1];
    u8 spriteId;

    if (increment <= 0)
        increment = 1;
    for (i = 0; i < 20; i += increment)
    {
        spriteId = CreateSprite(&gSmallWaterOrbSpriteTemplate, attackerCoordX, attackerCoordY, subpriority);
        if (spriteId != MAX_SPRITES)
        {
            gSprites[spriteId].data[1] = i;
            gSprites[spriteId].data[2] = attackerCoordX * 16;
            gSprites[spriteId].data[3] = attackerCoordY * 16;
            gSprites[spriteId].data[4] = Cos(trigIndex, 64);
            gSprites[spriteId].data[5] = Sin(trigIndex, 64);
            gSprites[spriteId].data[6] = taskId;
            gSprites[spriteId].data[7] = 2;
            if (task->data[2] & 1)
                AnimSmallWaterOrb(&gSprites[spriteId]);
            task->data[2]++;
        }
        trigIndex = (trigIndex + increment * 2);
        trigIndex &= 0xFF;
    }
}

static void AnimSmallWaterOrb(struct Sprite *sprite)
{
    switch (sprite->data[0])
    {
        case 0:
            sprite->data[4] += (sprite->data[1] % 6) * 3;
            sprite->data[5] += (sprite->data[1] % 3) * 3;
            sprite->data[0]++;
        case 1:
            sprite->data[2] += sprite->data[4];
            sprite->data[3] += sprite->data[5];
            sprite->x = sprite->data[2] >> 4;
            sprite->y = sprite->data[3] >> 4;
            if (sprite->x < -8 || sprite->x > 248 || sprite->y < -8 || sprite->y > 120)
            {
                gTasks[sprite->data[6]].data[sprite->data[7]]--;
                DestroySprite(sprite);
            }
            break;
    }
}

void AnimTask_WaterSpoutRain(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    task->data[1] = GetWaterSpoutPowerForAnim();
    if (GetBattlerSide(gBattleAnimAttacker) == B_SIDE_PLAYER)
    {
        task->data[4] = 136;
        task->data[6] = 40;
    }
    else
    {
        task->data[4] = 16;
        task->data[6] = 80;
    }
    task->data[5] = 98;
    task->data[7] = task->data[4] + 49;
    task->data[12] = task->data[1] * 5 + 5;
    task->func = AnimTask_WaterSpoutRain_Step;
}

static void AnimTask_WaterSpoutRain_Step(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    u8 taskId2;

    switch (task->data[0])
    {
        case 0:
            if (++task->data[2] > 2)
            {
                task->data[2] = 0;
                CreateWaterSpoutRainDroplet(task, taskId);
            }
            if (task->data[10] != 0 && task->data[13] == 0)
            {
                gBattleAnimArgs[0] = ANIM_BATTLER_TARGET;
                gBattleAnimArgs[1] = 0;
                gBattleAnimArgs[2] = 12;
                taskId2 = CreateTask(AnimTask_HorizontalShake, 80);
                if (taskId2 != 0xFF)
                {
                    gTasks[taskId2].func(taskId2);
                    gAnimVisualTaskCount++;
                }
                gBattleAnimArgs[0] = ANIM_BATTLER_DEF_PARTNER;
                taskId2 = CreateTask(AnimTask_HorizontalShake, 80);
                if (taskId2 != 0xFF)
                {
                    gTasks[taskId2].func(taskId2);
                    gAnimVisualTaskCount++;
                }
                task->data[13] = 1;
            }
            if (task->data[11] >= task->data[12])
                task->data[0]++;
            break;
        case 1:
            if (task->data[9] == 0)
                DestroyAnimVisualTask(taskId);
            break;
    }
}

static void CreateWaterSpoutRainDroplet(struct Task *task, u8 taskId)
{
    u16 yPosArg = ((gSineTable[task->data[8]] + 3) >> 4) + task->data[6];
    u8 spriteId = CreateSprite(&gSmallWaterOrbSpriteTemplate, task->data[7], 0, 0);

    if (spriteId != MAX_SPRITES)
    {
        gSprites[spriteId].callback = AnimWaterSpoutRain;
        gSprites[spriteId].data[5] = yPosArg;
        gSprites[spriteId].data[6] = taskId;
        gSprites[spriteId].data[7] = 9;
        task->data[9]++;
    }
    task->data[11]++;
    task->data[8] = (task->data[8] + 39) & 0xFF;
    task->data[7] = ((task->data[7] * 0x41c64e6d + 0x3039) % task->data[5]) + task->data[4];
}

static void AnimWaterSpoutRain(struct Sprite *sprite)
{
    if (sprite->data[0] == 0)
    {
        sprite->y += 8;
        if (sprite->y >= sprite->data[5])
        {
            gTasks[sprite->data[6]].data[10] = 1;
            sprite->data[1] = CreateSprite(&gWaterHitSplatSpriteTemplate, sprite->x, sprite->y, 1);
            if (sprite->data[1] != MAX_SPRITES)
            {
                StartSpriteAffineAnim(&gSprites[sprite->data[1]], 3);
                gSprites[sprite->data[1]].data[6] = sprite->data[6];
                gSprites[sprite->data[1]].data[7] = sprite->data[7];
                gSprites[sprite->data[1]].callback = AnimWaterSpoutRainHit;
            }
            DestroySprite(sprite);
        }
    }
}

static void AnimWaterSpoutRainHit(struct Sprite *sprite)
{
    if (++sprite->data[1] > 1)
    {
        sprite->data[1] = 0;
        sprite->invisible ^= 1;
        if (++sprite->data[2] == 12)
        {
            gTasks[sprite->data[6]].data[sprite->data[7]]--;
            FreeOamMatrix(sprite->oam.matrixNum);
            DestroySprite(sprite);
        }
    }
}

void AnimTask_WaterSport(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    task->data[3] = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_X_2);
    task->data[4] = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_Y_PIC_OFFSET);
    task->data[7] = (GetBattlerSide(gBattleAnimAttacker) == B_SIDE_PLAYER) ? 1 : -1;
    if (IsContest())
        task->data[7] *= -1;
    task->data[5] = task->data[3] + task->data[7] * 8;
    task->data[6] = task->data[4] - task->data[7] * 8;
    task->data[9] = -32;
    task->data[1] = 0;
    task->data[0] = 0;
    task->func = AnimTask_WaterSport_Step;
}

static void AnimTask_WaterSport_Step(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->data[0])
    {
        case 0:
            CreateWaterSportDroplet(task);
            if (task->data[10] != 0)
                task->data[0]++;
            break;
        case 1:
            CreateWaterSportDroplet(task);
            if (++task->data[1] > 16)
            {
                task->data[1] = 0;
                task->data[0]++;
            }
            break;
        case 2:
            CreateWaterSportDroplet(task);
            task->data[5] += task->data[7] * 6;
            if (!(task->data[5] >= -16 && task->data[5] <= 256))
            {
                if (++task->data[12] > 2)
                {
                    task->data[13] = 1;
                    task->data[0] = 6;
                    task->data[1] = 0;
                }
                else
                {
                    task->data[1] = 0;
                    task->data[0]++;
                }
            }
            break;
        case 3:
            CreateWaterSportDroplet(task);
            task->data[6] -= task->data[7] * 2;
            if (++task->data[1] > 7)
                task->data[0]++;
            break;
        case 4:
            CreateWaterSportDroplet(task);
            task->data[5] -= task->data[7] * 6;
            if (!(task->data[5] >= -16 && task->data[5] <= 256))
            {
                task->data[12]++;
                task->data[1] = 0;
                task->data[0]++;
            }
            break;
        case 5:
            CreateWaterSportDroplet(task);
            task->data[6] -= task->data[7] * 2;
            if (++task->data[1] > 7)
                task->data[0] = 2;
            break;
        case 6:
            if (task->data[8] == 0)
                task->data[0]++;
            break;
        default:
            DestroyAnimVisualTask(taskId);
            break;
    }
}

static void CreateWaterSportDroplet(struct Task *task)
{
    u8 spriteId;

    if (++task->data[2] > 1)
    {
        task->data[2] = 0;
        spriteId = CreateSprite(&gSmallWaterOrbSpriteTemplate, task->data[3], task->data[4], 10);
        if (spriteId != MAX_SPRITES)
        {
            gSprites[spriteId].data[0] = 16;
            gSprites[spriteId].data[2] = task->data[5];
            gSprites[spriteId].data[4] = task->data[6];
            gSprites[spriteId].data[5] = task->data[9];
            InitAnimArcTranslation(&gSprites[spriteId]);
            gSprites[spriteId].callback = AnimWaterSportDroplet;
            task->data[8]++;
        }
    }
}

static void AnimWaterSportDroplet(struct Sprite *sprite)
{
    if (TranslateAnimArc(sprite))
    {
        sprite->x += sprite->x2;
        sprite->y += sprite->y2;
        sprite->data[0] = 6;
        sprite->data[2] = (Random() & 0x1F) - 16 + sprite->x;
        sprite->data[4] = (Random() & 0x1F) - 16 + sprite->y;
        sprite->data[5] = ~(Random() & 7);
        InitAnimArcTranslation(sprite);
        sprite->callback = AnimWaterSportDroplet_Step;
    }
}

static void AnimWaterSportDroplet_Step(struct Sprite *sprite)
{
    u16 i;

    if (TranslateAnimArc(sprite))
    {
        for (i = 0; i < NUM_TASKS; i++)
        {
            if (gTasks[i].func == AnimTask_WaterSport_Step)
            {
                gTasks[i].data[10] = 1;
                gTasks[i].data[8]--;
                DestroySprite(sprite);
            }
        }
    }
}

void sub_80D4BF0(struct Sprite *sprite)
{
    sprite->x = gBattleAnimArgs[0];
    sprite->y = gBattleAnimArgs[1];
    sprite->data[0] = gBattleAnimArgs[2];
    sprite->data[1] = gBattleAnimArgs[3];
    sprite->data[2] = gBattleAnimArgs[4];
    sprite->data[3] = gBattleAnimArgs[5];
    sprite->callback = sub_80D4C18;
}

void sub_80D4C18(struct Sprite *sprite)
{
    sprite->data[4] -= sprite->data[0];
    sprite->y2 = sprite->data[4] / 10;
    sprite->data[5] = (sprite->data[5] + sprite->data[1]) & 0xFF;
    sprite->x2 = Sin(sprite->data[5], sprite->data[2]);
    if (--sprite->data[3] == 0)
        DestroyAnimSprite(sprite);
}

void sub_80D4C64(struct Sprite *sprite)
{
    sprite->data[3] += sprite->data[1];
    sprite->data[4] += sprite->data[2];
    sprite->x2 = sprite->data[3] >> 7;
    sprite->y2 = sprite->data[4] >> 7;
    if (--sprite->data[0] == 0)
    {
        FreeSpriteOamMatrix(sprite);
        DestroySprite(sprite);
    }
}

void sub_80D4CA4(struct Sprite *sprite)
{
    InitSpritePosToAnimAttacker(sprite, TRUE);
    sprite->data[1] = GetBattlerSpriteCoord(gBattleAnimTarget, 2);
    sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimTarget, 3);
    sprite->data[3] = gBattleAnimArgs[2];
    sprite->data[4] = gBattleAnimArgs[3];
    sprite->callback = sub_80D4CEC;
}

void sub_80D4CEC(struct Sprite *sprite)
{
    int xDiff = sprite->data[1] - sprite->x;
    int yDiff = sprite->data[2] - sprite->y;

    sprite->x2 = (sprite->data[0] * xDiff) / sprite->data[3];
    sprite->y2 = (sprite->data[0] * yDiff) / sprite->data[3];
    if (++sprite->data[5] == sprite->data[4])
    {
        sprite->data[5] = 0;
        sub_80D4D64(sprite, xDiff, yDiff);
    }
    if (sprite->data[3] == sprite->data[0])
        DestroyAnimSprite(sprite);
    sprite->data[0]++;
}

void sub_80D4D64(struct Sprite *sprite, s32 xDiff, s32 yDiff)
{
    s16 i;
    u8 spriteId;

    s16 combinedX;
    s16 combinedY;
    s16 something;
    s16 randomSomethingX;
    s16 randomSomethingY;

    something = sprite->data[0] / 2;
    // regalloc acts strange here...
    combinedX = sprite->x + sprite->x2;
    combinedY = sprite->y + sprite->y2;

    // ...then goes back to normal right here.
    // Nothing but this appears to reproduce the behavior.
    if (xDiff) // yDiff works too, but not sprite.
    {
        u8 unk = -unk; // this can be any sort of negation
    }

    randomSomethingY = yDiff + (Random() % 10) - 5;
    randomSomethingX = -xDiff + (Random() % 10) - 5;

    for (i = 0; i <= 0; i++)
    {
        spriteId = CreateSprite(&gSpriteTemplate_83D9420, combinedX, combinedY + something, 130);
        gSprites[spriteId].data[0] = 20;
        gSprites[spriteId].data[1] = randomSomethingY;
        gSprites[spriteId].subpriority = GetBattlerSpriteSubpriority(gBattleAnimAttacker) - 1;
        if (randomSomethingX < 0)
            gSprites[spriteId].data[2] = -randomSomethingX;
        else
            gSprites[spriteId].data[2] = randomSomethingX;
    }
    for (i = 0; i <= 0; i++)
    {
        spriteId = CreateSprite(&gSpriteTemplate_83D9420, combinedX, combinedY - something, 130);
        gSprites[spriteId].data[0] = 20;
        gSprites[spriteId].data[1] = randomSomethingY;
        gSprites[spriteId].subpriority = GetBattlerSpriteSubpriority(gBattleAnimAttacker) - 1;
        if (randomSomethingX > 0)
            gSprites[spriteId].data[2] = -randomSomethingX;
        else
            gSprites[spriteId].data[2] = randomSomethingX;
    }
}
