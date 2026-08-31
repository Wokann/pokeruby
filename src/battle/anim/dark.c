#include "global.h"
#include "battle_anim.h"
#include "blend_palette.h"
#include "decompress.h"
#include "ewram.h"
#include "palette.h"
#include "rom_8077ABC.h"
#include "scanline_effect.h"
#include "sound.h"
#include "sprite.h"
#include "trig.h"
#include "constants/battle.h"

extern s16 gBattleAnimArgs[];
extern u8 gBattleAnimAttacker;
extern u8 gBattleAnimTarget;
extern u8 gBattlerSpriteIds[];
extern u16 gBattle_BG1_X;
extern u16 gBattle_BG1_Y;
extern u16 gBattle_BG2_X;
extern u16 gBattle_BG2_Y;
extern u16 gBattle_WIN0H;
extern u16 gBattle_WIN0V;
extern u16 gBattlerPartyIndexes[];
extern u8 gAnimMoveTurn;

extern const u8 gUnknown_08D1D574[];
extern const u8 gUnknown_08D1D410[];
extern const u16 gUnknown_08D1D54C[];

void sub_80DFE14(struct Sprite *sprite);
void sub_80DFF1C(struct Sprite *sprite);
static void AnimTearDrop(struct Sprite *sprite);
void AnimClawSlash(struct Sprite *sprite);
static void sub_80DFE90(struct Sprite *sprite);
static void AnimTask_AttackerFadeToInvisible_Step(u8 taskId);
static void AnimTask_AttackerFadeFromInvisible_Step(u8 taskId);
static void sub_80DFF58(struct Sprite *sprite);
static void sub_80DFF98(struct Sprite *sprite);
static void AnimTearDrop_Step(struct Sprite *sprite);
static void AnimTask_MoveAttackerMementoShadow_Step(u8 taskId);
static void AnimTask_MoveTargetMementoShadow_Step(u8 taskId);
static void SetAllBattlersSpritePriority(u8 priority);
static void DoMementoShadowEffect(struct Task *task);
static void AnimTask_MetallicShine_Step(u8 taskId);

const struct SpriteTemplate gSpriteTemplate_83DB118 =
{
    .tileTag = ANIM_TAG_TIED_BAG,
    .paletteTag = ANIM_TAG_TIED_BAG,
    .oam = &gOamData_AffineOff_ObjNormal_16x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80DFE14,
};

const union AffineAnimCmd gSpriteAffineAnim_83DB130[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, 0, 1),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd gSpriteAffineAnim_83DB140[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, 32, 1),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd gSpriteAffineAnim_83DB150[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, 64, 1),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd gSpriteAffineAnim_83DB160[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, 96, 1),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd gSpriteAffineAnim_83DB170[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, -128, 1),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd gSpriteAffineAnim_83DB180[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, -96, 1),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd gSpriteAffineAnim_83DB190[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, -64, 1),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd gSpriteAffineAnim_83DB1A0[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, -32, 1),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd *const gSpriteAffineAnimTable_83DB1B0[] =
{
    gSpriteAffineAnim_83DB130,
    gSpriteAffineAnim_83DB140,
    gSpriteAffineAnim_83DB150,
    gSpriteAffineAnim_83DB160,
    gSpriteAffineAnim_83DB170,
    gSpriteAffineAnim_83DB180,
    gSpriteAffineAnim_83DB190,
    gSpriteAffineAnim_83DB1A0,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83DB1D0 =
{
    .tileTag = ANIM_TAG_SHARP_TEETH,
    .paletteTag = ANIM_TAG_SHARP_TEETH,
    .oam = &gOamData_837E0BC,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gSpriteAffineAnimTable_83DB1B0,
    .callback = sub_80DFF1C,
};

const struct SpriteTemplate gBattleAnimSpriteTemplate_83DB1E8 =
{
    .tileTag = ANIM_TAG_CLAMP,
    .paletteTag = ANIM_TAG_CLAMP,
    .oam = &gOamData_837E0BC,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gSpriteAffineAnimTable_83DB1B0,
    .callback = sub_80DFF1C,
};

static const union AffineAnimCmd sAffineAnim_TearDrop_0[] =
{
    AFFINEANIMCMD_FRAME(0xC0, 0xC0, 80, 0),
    AFFINEANIMCMD_FRAME(0x0, 0x0, -2, 8),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_TearDrop_1[] =
{
    AFFINEANIMCMD_FRAME(0xC0, 0xC0, -80, 0),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 2, 8),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd *const sAffineAnims_TearDrop[] =
{
    sAffineAnim_TearDrop_0,
    sAffineAnim_TearDrop_1,
};

const struct SpriteTemplate gTearDropSpriteTemplate =
{
    .tileTag = ANIM_TAG_SMALL_BUBBLES,
    .paletteTag = ANIM_TAG_SMALL_BUBBLES,
    .oam = &gOamData_AffineNormal_ObjNormal_16x16,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = sAffineAnims_TearDrop,
    .callback = AnimTearDrop,
};

static const union AnimCmd sAnim_ClawSlash_0[] =
{
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(16, 4),
    ANIMCMD_FRAME(32, 4),
    ANIMCMD_FRAME(48, 4),
    ANIMCMD_FRAME(64, 4),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_ClawSlash_1[] =
{
    ANIMCMD_FRAME(0, 4, .hFlip = TRUE),
    ANIMCMD_FRAME(16, 4, .hFlip = TRUE),
    ANIMCMD_FRAME(32, 4, .hFlip = TRUE),
    ANIMCMD_FRAME(48, 4, .hFlip = TRUE),
    ANIMCMD_FRAME(64, 4, .hFlip = TRUE),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_ClawSlash[] =
{
    sAnim_ClawSlash_0,
    sAnim_ClawSlash_1,
};

const struct SpriteTemplate gClawSlashSpriteTemplate =
{
    .tileTag = ANIM_TAG_CLAW_SLASH,
    .paletteTag = ANIM_TAG_CLAW_SLASH,
    .oam = &gOamData_AffineOff_ObjNormal_32x32,
    .anims = sAnims_ClawSlash,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = AnimClawSlash,
};

void AnimTask_AttackerFadeToInvisible(u8 taskId)
{
    int battler;
    gTasks[taskId].data[0] = gBattleAnimArgs[0];
    battler = gBattleAnimAttacker;
    gTasks[taskId].data[1] = 16;
    REG_BLDALPHA = BLDALPHA_BLEND(16, 0);
    if (GetBattlerSpriteBGPriorityRank(battler) == 1)
        REG_BLDCNT = BLDCNT_TGT2_ALL | BLDCNT_EFFECT_BLEND | BLDCNT_TGT1_BG1;
    else
        REG_BLDCNT = BLDCNT_TGT2_ALL | BLDCNT_EFFECT_BLEND | BLDCNT_TGT1_BG2;
    gTasks[taskId].func = AnimTask_AttackerFadeToInvisible_Step;
}

static void AnimTask_AttackerFadeToInvisible_Step(u8 taskId)
{
    u8 blendB = gTasks[taskId].data[1] >> 8;
    u8 blendA = gTasks[taskId].data[1];
    if (gTasks[taskId].data[2] == (u8)gTasks[taskId].data[0])
    {
        blendB++;
        blendA--;
        gTasks[taskId].data[1] = BLDALPHA_BLEND(blendA, blendB);
        REG_BLDALPHA = BLDALPHA_BLEND(blendA, blendB);
        gTasks[taskId].data[2] = 0;
        if (blendB == 16)
        {
            gSprites[gBattlerSpriteIds[gBattleAnimAttacker]].invisible = TRUE;
            DestroyAnimVisualTask(taskId);
        }
    }
    else
        gTasks[taskId].data[2]++;
}

void AnimTask_AttackerFadeFromInvisible(u8 taskId)
{
    gTasks[taskId].data[0] = gBattleAnimArgs[0];
    gTasks[taskId].data[1] = BLDALPHA_BLEND(0, 16);
    gTasks[taskId].func = AnimTask_AttackerFadeFromInvisible_Step;
    REG_BLDALPHA = BLDALPHA_BLEND(0, 16);
}

static void AnimTask_AttackerFadeFromInvisible_Step(u8 taskId)
{
    u8 blendB = gTasks[taskId].data[1] >> 8;
    u8 blendA = gTasks[taskId].data[1];
    if (gTasks[taskId].data[2] == (u8)gTasks[taskId].data[0])
    {
        blendB--;
        blendA++;
        gTasks[taskId].data[1] = BLDALPHA_BLEND(blendA, blendB);
        REG_BLDALPHA = BLDALPHA_BLEND(blendA, blendB);
        gTasks[taskId].data[2] = 0;
        if (blendB == 0)
        {
            REG_BLDCNT = 0;
            REG_BLDALPHA = 0;
            DestroyAnimVisualTask(taskId);
        }
    }
    else
        gTasks[taskId].data[2]++;
}

// unlike the above is only used in Feint Attack

void sub_80DFDC0(u8 taskId)
{
    REG_BLDALPHA = 0x1000;
    if (GetBattlerSpriteBGPriorityRank(gBattleAnimAttacker) == 1)
        REG_BLDCNT = 0x3F42;
    else
        REG_BLDCNT = 0x3F44;
    DestroyAnimVisualTask(taskId);
}

// unused sprite template's callback

void sub_80DFE14(struct Sprite *sprite)
{
    sprite->data[1] = GetBattlerSpriteCoord(gBattleAnimTarget, 2);
    sprite->data[2] = GetBattlerSpriteCoord(gBattleAnimAttacker, 2);
    sprite->data[3] = GetBattlerSpriteCoord(gBattleAnimTarget, 3);
    sprite->data[4] = GetBattlerSpriteCoord(gBattleAnimAttacker, 3);
    sprite->data[0] = 0x7E;
    InitSpriteDataForLinearTranslation(sprite);
    sprite->data[3] = -sprite->data[1];
    sprite->data[4] = -sprite->data[2];
    sprite->data[6] = 0xFFD8;
    sprite->callback = sub_80DFE90;
    sub_80DFE90(sprite);
}

static void sub_80DFE90(struct Sprite *sprite)
{
    sprite->data[3] += sprite->data[1];
    sprite->data[4] += sprite->data[2];
    sprite->x2 = sprite->data[3] >> 8;
    sprite->y2 = sprite->data[4] >> 8;
    if (sprite->data[7] == 0)
    {
        sprite->data[3] += sprite->data[1];
        sprite->data[4] += sprite->data[2];
        sprite->x2 = sprite->data[3] >> 8;
        sprite->y2 = sprite->data[4] >> 8;
        sprite->data[0]--;
    }
    sprite->y2 += Sin(sprite->data[5], sprite->data[6]);
    sprite->data[5] = (sprite->data[5] + 3) & 0xFF;
    if (sprite->data[5] > 0x7F)
    {
        sprite->data[5] = 0;
        sprite->data[6] += 20;
        sprite->data[7]++;
    }
    if (--sprite->data[0] == 0)
        DestroyAnimSprite(sprite);
}

void sub_80DFF1C(struct Sprite *sprite)
{
    sprite->x += gBattleAnimArgs[0];
    sprite->y += gBattleAnimArgs[1];
    StartSpriteAffineAnim(sprite, gBattleAnimArgs[2]);

    sprite->data[0] = gBattleAnimArgs[3];
    sprite->data[1] = gBattleAnimArgs[4];
    sprite->data[2] = gBattleAnimArgs[5];
    sprite->callback = sub_80DFF58;
}

static void sub_80DFF58(struct Sprite *sprite)
{
    sprite->data[4] += sprite->data[0];
    sprite->data[5] += sprite->data[1];
    sprite->x2 = sprite->data[4] >> 8;
    sprite->y2 = sprite->data[5] >> 8;

    if (++sprite->data[3] == sprite->data[2])
        sprite->callback = sub_80DFF98;
}

static void sub_80DFF98(struct Sprite *sprite)
{
    sprite->data[4] -= sprite->data[0];
    sprite->data[5] -= sprite->data[1];
    sprite->x2 = sprite->data[4] >> 8;
    sprite->y2 = sprite->data[5] >> 8;

    if (--sprite->data[3] == 0)
        DestroySpriteAndMatrix(sprite);
}

static void AnimTearDrop(struct Sprite *sprite)
{
    u8 battler;
    s8 xOffset;

    if (gBattleAnimArgs[0] == 0)
        battler = gBattleAnimAttacker;
    else
        battler = gBattleAnimTarget;

    xOffset = 20;
    sprite->oam.tileNum += 4;

    switch (gBattleAnimArgs[1])
    {
    case 0:
        sprite->x = GetBattlerSpriteCoordAttr(battler, BATTLER_COORD_ATTR_RIGHT) - 8;
        sprite->y = GetBattlerSpriteCoordAttr(battler, BATTLER_COORD_ATTR_TOP) + 8;
        break;
    case 1:
        sprite->x = GetBattlerSpriteCoordAttr(battler, BATTLER_COORD_ATTR_RIGHT) - 14;
        sprite->y = GetBattlerSpriteCoordAttr(battler, BATTLER_COORD_ATTR_TOP) + 16;
        break;
    case 2:
        sprite->x = GetBattlerSpriteCoordAttr(battler, BATTLER_COORD_ATTR_LEFT) + 8;
        sprite->y = GetBattlerSpriteCoordAttr(battler, BATTLER_COORD_ATTR_TOP) + 8;
        StartSpriteAffineAnim(sprite, 1);
        xOffset = -20;
        break;
    case 3:
        sprite->x = GetBattlerSpriteCoordAttr(battler, BATTLER_COORD_ATTR_LEFT) + 14;
        sprite->y = GetBattlerSpriteCoordAttr(battler, BATTLER_COORD_ATTR_TOP) + 16;
        StartSpriteAffineAnim(sprite, 1);
        xOffset = -20;
        break;
    }

    sprite->data[0] = 32;
    sprite->data[2] = sprite->x + xOffset;
    sprite->data[4] = sprite->y + 12;
    sprite->data[5] = -12;

    InitAnimArcTranslation(sprite);
    sprite->callback = AnimTearDrop_Step;
}

static void AnimTearDrop_Step(struct Sprite *sprite)
{
    if (TranslateAnimArc(sprite))
        DestroySpriteAndMatrix(sprite);
}

void AnimTask_MoveAttackerMementoShadow(u8 taskId)
{
    struct ScanlineEffectParams scanlineParams;
    struct BattleAnimBgData animBg;
    u16 i;
    u8 pos;
    int var0;
    struct Task *task = &gTasks[taskId];

    task->data[7] = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_Y) + 31;
    task->data[6] = GetBattlerSpriteCoordAttr(gBattleAnimAttacker, BATTLER_COORD_ATTR_TOP) - 7;
    task->data[5] = task->data[7];
    task->data[4] = task->data[6];
    task->data[13] = (task->data[7] - task->data[6]) << 8;

    pos = GetBattlerSpriteCoord(gBattleAnimAttacker, BATTLER_COORD_X);
    task->data[14] = pos - 32;
    task->data[15] = pos + 32;

    if (GetBattlerSide(gBattleAnimAttacker) == B_SIDE_PLAYER)
        task->data[8] = -12;
    else
        task->data[8] = -64;

    task->data[3] = GetBattlerSpriteBGPriorityRank(gBattleAnimAttacker);
    if (task->data[3] == 1)
    {
        GetBattleAnimBg1Data(&animBg);
        task->data[10] = gBattle_BG1_Y;
        REG_BLDCNT = 0x3F42;
        FillPalette(0, animBg.paletteId << 4, 32);
        scanlineParams.dmaDest = &REG_BG1VOFS;
        var0 = 2;

        if (!IsContest())
            gBattle_BG2_X += 240;
    }
    else
    {
        task->data[10] = gBattle_BG2_Y;
        REG_BLDCNT = 0x3F44;
        FillPalette(0, 144, 32);
        scanlineParams.dmaDest = &REG_BG2VOFS;
        var0 = 4;

        if (!IsContest())
            gBattle_BG1_X += 240;
    }

    scanlineParams.dmaControl = 0xA2600001;
    scanlineParams.initState = 1;
    scanlineParams.unused9 = 0;
    task->data[11] = 0;
    task->data[12] = 16;
    task->data[0] = 0;
    task->data[1] = 0;
    task->data[2] = 0;

    SetAllBattlersSpritePriority(3);

    for (i = 0; i < 112; i++)
    {
        gScanlineEffectRegBuffers[0][i] = task->data[10];
        gScanlineEffectRegBuffers[1][i] = task->data[10];
    }

    ScanlineEffect_SetParams(scanlineParams);

    REG_WINOUT = 0x3F00 | (var0 ^ 0x3F);
    REG_WININ = 0x3F3F;
    gBattle_WIN0H = (task->data[14] << 8) | task->data[15];
    gBattle_WIN0V = 160;

    task->func = AnimTask_MoveAttackerMementoShadow_Step;
}

static void AnimTask_MoveAttackerMementoShadow_Step(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->data[0])
    {
    case 0:
        if (++task->data[1] > 1)
        {
            task->data[1] = 0;
            if (++task->data[2] & 1)
            {
                if (task->data[11] != 12)
                    task->data[11]++;
            }
            else
            {
                if (task->data[12] != 8)
                    task->data[12]--;
            }

            REG_BLDALPHA = (task->data[12] << 8) | task->data[11];

            if (task->data[11] == 12 && task->data[12] == 8)
                task->data[0]++;
        }
        break;
    case 1:
        task->data[4] -= 8;
        DoMementoShadowEffect(task);

        if (task->data[4] < task->data[8])
            task->data[0]++;
        break;
    case 2:
        task->data[4] -= 8;
        DoMementoShadowEffect(task);
        task->data[14] += 4;
        task->data[15] -= 4;

        if (task->data[14] >= task->data[15])
            task->data[14] = task->data[15];

        gBattle_WIN0H = (task->data[14] << 8) | task->data[15];

        if (task->data[14] == task->data[15])
            task->data[0]++;
        break;
    case 3:
        gScanlineEffect.state = 3;
        task->data[0]++;
        break;
    case 4:
        DestroyAnimVisualTask(taskId);
        break;
    }
}

void AnimTask_MoveTargetMementoShadow(u8 taskId)
{
    struct BattleAnimBgData animBg;
    struct ScanlineEffectParams scanlineParams;
    u8 pos;
    u16 i;
    struct Task *task = &gTasks[taskId];

    switch (task->data[0])
    {
    case 0:
        if (IsContest() == TRUE)
        {
            gBattle_WIN0H = 0;
            gBattle_WIN0V = 0;
            REG_WININ = 0x3F3F;
            REG_WINOUT = 0x3F3F;
            DestroyAnimVisualTask(taskId);
        }
        else
        {
            task->data[3] = GetBattlerSpriteBGPriorityRank(gBattleAnimTarget);
            if (task->data[3] == 1)
            {
                REG_BLDCNT = 0x3F42;
                gBattle_BG2_X += 240;
            }
            else
            {
                REG_BLDCNT = 0x3F44;
                gBattle_BG1_X += 240;
            }

            task->data[0]++;
        }
        break;
    case 1:
        if (task->data[3] == 1)
        {
            GetBattleAnimBg1Data(&animBg);
            task->data[10] = gBattle_BG1_Y;
            FillPalette(0, animBg.paletteId << 4, 32);
        }
        else
        {
            task->data[10] = gBattle_BG2_Y;
            FillPalette(0, 144, 32);
        }

        SetAllBattlersSpritePriority(3);
        task->data[0]++;
        break;
    case 2:
        task->data[7] = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_Y) + 31;
        task->data[6] = GetBattlerSpriteCoordAttr(gBattleAnimTarget, BATTLER_COORD_ATTR_TOP) - 7;
        task->data[13] = (task->data[7] - task->data[6]) << 8;
        pos = GetBattlerSpriteCoord(gBattleAnimTarget, BATTLER_COORD_X);
        task->data[14] = pos - 4;
        task->data[15] = pos + 4;

        if (GetBattlerSide(gBattleAnimTarget) == B_SIDE_PLAYER)
            task->data[8] = -12;
        else
            task->data[8] = -64;

        task->data[4] = task->data[8];
        task->data[5] = task->data[8];
        task->data[11] = 12;
        task->data[12] = 8;
        task->data[0]++;
        break;
    case 3:
        if (task->data[3] == 1)
            scanlineParams.dmaDest = &REG_BG1VOFS;
        else
            scanlineParams.dmaDest = &REG_BG2VOFS;

        for (i = 0; i < 112; i++)
        {
            gScanlineEffectRegBuffers[0][i] = task->data[10] + (159 - i);
            gScanlineEffectRegBuffers[1][i] = task->data[10] + (159 - i);
        }

        scanlineParams.dmaControl = 0xA2600001;
        scanlineParams.initState = 1;
        scanlineParams.unused9 = 0;
        ScanlineEffect_SetParams(scanlineParams);
        task->data[0]++;
        break;
    case 4:
        if (task->data[3] == 1)
            REG_WINOUT = 0x3F3D;
        else
            REG_WINOUT = 0x3F3B;

        REG_WININ = 0x3F3F;
        gBattle_WIN0H = (task->data[14] << 8) | task->data[15];
        gBattle_WIN0V = 160;

        task->data[0] = 0;
        task->data[1] = 0;
        task->data[2] = 0;
        REG_BLDALPHA = 0x80C;
        task->func = AnimTask_MoveTargetMementoShadow_Step;
        break;
    }
}

static void AnimTask_MoveTargetMementoShadow_Step(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->data[0])
    {
    case 0:
        task->data[5] += 8;
        if (task->data[5] >= task->data[7])
            task->data[5] = task->data[7];

        DoMementoShadowEffect(task);

        if (task->data[5] == task->data[7])
            task->data[0]++;
        break;
    case 1:
        if (task->data[15] - task->data[14] < 64)
        {
            task->data[14] -= 4;
            task->data[15] += 4;
        }
        else
        {
            task->data[1] = 1;
        }

        gBattle_WIN0H = (task->data[14] << 8) | task->data[15];
        task->data[4] += 8;

        if (task->data[4] >= task->data[6])
            task->data[4] = task->data[6];

        DoMementoShadowEffect(task);

        if (task->data[4] == task->data[6] && task->data[1] != 0)
        {
            task->data[1] = 0;
            task->data[0]++;
        }
        break;
    case 2:
        if (++task->data[1] > 1)
        {
            task->data[1] = 0;
            if ((++task->data[2] & 1) != 0)
            {
                if (task->data[11] != 0)
                    task->data[11]--;
            }
            else
            {
                if (task->data[12] < 16)
                    task->data[12]++;
            }

            REG_BLDALPHA = (task->data[12] << 8) | task->data[11];

            if (task->data[11] == 0 && task->data[12] == 16)
                task->data[0]++;
        }
        break;
    case 3:
        gScanlineEffect.state = 3;
        task->data[0]++;
        break;
    case 4:
        gBattle_WIN0H = 0;
        gBattle_WIN0V = 0;
        REG_WININ = 0x3F3F;
        REG_WINOUT = 0x3F3F;
        DestroyAnimVisualTask(taskId);
        break;
    }
}

static void DoMementoShadowEffect(struct Task *task)
{
    int var0, var1;
    s16 var2;
    s16 i;
    int var4;

    var2 = task->data[5] - task->data[4];
    if (var2 != 0)
    {
        var0 = task->data[13] / var2;
        var1 = task->data[6] << 8;

        for (i = 0; i < task->data[4]; i++)
        {
            gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][i] = task->data[10] - (i - 159);
        }

        for (i = task->data[4]; i <= task->data[5]; i++)
        {
            if (i >= 0)
            {
                s16 var3 = (var1 >> 8) - i;
                gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][i] = var3 + task->data[10];
            }

            var1 += var0;
        }

        var4 = task->data[10] - (i - 159);
        for (i = i; i < task->data[7]; i++)
        {
            if (i >= 0)
            {
                gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][i] = var4;
                var4--;
            }
        }
    }
    else
    {
        var4 = task->data[10] + 159;
        for (i = 0; i < 112; i++)
        {
            gScanlineEffectRegBuffers[0][i] = var4;
            gScanlineEffectRegBuffers[1][i] = var4;
            var4--;
        }
    }
}

static void SetAllBattlersSpritePriority(u8 priority)
{
    u16 i;

    for (i = 0; i < MAX_BATTLERS_COUNT; i++)
    {
        u8 spriteId = GetAnimBattlerSpriteId(i);
        if (spriteId != 0xFF)
            gSprites[spriteId].oam.priority = priority;
    }
}

void AnimTask_InitMementoShadow(u8 taskId)
{
    u8 toBG2 = GetBattlerSpriteBGPriorityRank(gBattleAnimAttacker) ^ 1 ? 1 : 0;
    MoveBattlerSpriteToBG(gBattleAnimAttacker, toBG2);
    gSprites[gBattlerSpriteIds[gBattleAnimAttacker]].invisible = FALSE;

    if (IsAnimBankSpriteVisible(BATTLE_PARTNER(gBattleAnimAttacker)))
    {
        MoveBattlerSpriteToBG(BATTLE_PARTNER(gBattleAnimAttacker), toBG2 ^ 1);
        gSprites[gBattlerSpriteIds[BATTLE_PARTNER(gBattleAnimAttacker)]].invisible = FALSE;
    }

    DestroyAnimVisualTask(taskId);
}

void AnimTask_MementoHandleBg(u8 taskId)
{
    u8 toBG2 = GetBattlerSpriteBGPriorityRank(gBattleAnimAttacker) ^ 1 ? 1 : 0;
    ResetBattleAnimBg(toBG2);

    if (IsAnimBankSpriteVisible(BATTLE_PARTNER(gBattleAnimAttacker)))
        ResetBattleAnimBg(toBG2 ^ 1);

    DestroyAnimVisualTask(taskId);
}

void AnimClawSlash(struct Sprite *sprite)
{
    sprite->x += gBattleAnimArgs[0];
    sprite->y += gBattleAnimArgs[1];
    StartSpriteAnim(sprite, gBattleAnimArgs[2]);
    sprite->callback = RunStoredCallbackWhenAnimEnds;
    StoreSpriteCallbackInData6(sprite, DestroyAnimSprite);
}

void AnimTask_MetallicShine(u8 taskId)
{
    u16 species;
    u8 spriteId;
    u8 newSpriteId;
    u16 paletteNum;
    struct BattleAnimBgData animBg;
    int priorityChanged = FALSE;

    gBattle_WIN0H = priorityChanged;
    gBattle_WIN0V = priorityChanged;
    REG_WININ = 0x3F3F;
    REG_WINOUT = 0x3F3D;
    REG_DISPCNT |= DISPCNT_OBJWIN_ON;
    REG_BLDCNT = 0x3F42;
    REG_BLDALPHA = 0xC08;
    REG_BG1CNT_BITFIELD.priority = 0;
    REG_BG1CNT_BITFIELD.screenSize = 0;
    if (!IsContest())
        REG_BG1CNT_BITFIELD.charBaseBlock = 1;

    if (IsDoubleBattle() && !IsContest())
    {
        if (GetBattlerPosition(gBattleAnimAttacker) == B_POSITION_OPPONENT_RIGHT
         || GetBattlerPosition(gBattleAnimAttacker) == B_POSITION_PLAYER_LEFT)
        {
            if (IsAnimBankSpriteVisible(gBattleAnimAttacker ^ 2) == TRUE)
            {
                gSprites[gBattlerSpriteIds[gBattleAnimAttacker ^ 2]].oam.priority--;
                REG_BG1CNT_BITFIELD.priority = 1;
                priorityChanged = TRUE;
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

    spriteId = GetAnimBattlerSpriteId(ANIM_BATTLER_ATTACKER);
    newSpriteId = CreateInvisibleSpriteCopy(gBattleAnimAttacker, spriteId, species);

    GetBattleAnimBg1Data(&animBg);
    DmaClear32(3, animBg.bgTilemap, 0x1000);
    LZDecompressVram(&gUnknown_08D1D574, animBg.bgTilemap);
    LZDecompressVram(&gUnknown_08D1D410, animBg.bgTiles);
    LoadCompressedPalette(&gUnknown_08D1D54C, animBg.paletteId << 4, 32);

    gBattle_BG1_X = -gSprites[spriteId].x + 96;
    gBattle_BG1_Y = -gSprites[spriteId].y + 32;
    paletteNum = 16 + gSprites[spriteId].oam.paletteNum;

    if (gBattleAnimArgs[1]  == 0)
        SetGrayscaleOrOriginalPalette(paletteNum, FALSE);
    else
        BlendPalette(paletteNum * 16, 16, 11, gBattleAnimArgs[2]);

    gTasks[taskId].data[0] = newSpriteId;
    gTasks[taskId].data[1] = gBattleAnimArgs[0];
    gTasks[taskId].data[2] = gBattleAnimArgs[1];
    gTasks[taskId].data[3] = gBattleAnimArgs[2];
    gTasks[taskId].data[6] = priorityChanged;
    gTasks[taskId].func = AnimTask_MetallicShine_Step;
}

static void AnimTask_MetallicShine_Step(u8 taskId)
{
    struct BattleAnimBgData animBg;
    u16 paletteNum;
    u8 spriteId;
    u8 taskIdCopy = taskId;

    gTasks[taskIdCopy].data[10] += 4;
    gBattle_BG1_X -= 4;

    if (gTasks[taskIdCopy].data[10] == 128)
    {
        gTasks[taskIdCopy].data[10] = 0;
        gBattle_BG1_X += 128;

        if (++gTasks[taskIdCopy].data[11] == 2)
        {
            ResetBattleAnimBg(0);
            gBattle_WIN0H = 0;
            gBattle_WIN0V = 0;
            REG_WININ = 0x3F3F;
            REG_WINOUT = 0x3F3F;

            if (!IsContest())
                REG_BG1CNT_BITFIELD.charBaseBlock = 0;

            REG_DISPCNT ^= DISPCNT_OBJWIN_ON;
            REG_BLDCNT = 0;
            REG_BLDALPHA = 0;

            spriteId = GetAnimBattlerSpriteId(0);
            paletteNum = 16 + gSprites[spriteId].oam.paletteNum;
            if (gTasks[taskIdCopy].data[1] == 0)
                SetGrayscaleOrOriginalPalette(paletteNum, TRUE);

            DestroySprite(&gSprites[gTasks[taskIdCopy].data[0]]);
            GetBattleAnimBg1Data(&animBg);
            DmaClear32(3, animBg.bgTilemap, 0x800);

            if (gTasks[taskIdCopy].data[6] == 1)
            {
                gSprites[gBattlerSpriteIds[gBattleAnimAttacker ^ 2]].oam.priority++;
            }
            
            DestroyAnimVisualTask(taskIdCopy);
        }
    }
}

void AnimTask_SetGrayscaleOrOriginalPal(u8 taskId)
{
    u8 spriteId;
    u8 battler;
    bool8 calcSpriteId = FALSE;
    u8 position = B_POSITION_PLAYER_LEFT;

    switch (gBattleAnimArgs[0])
    {
    case ANIM_BATTLER_ATTACKER:
    case ANIM_BATTLER_TARGET:
    case ANIM_BATTLER_ATK_PARTNER:
    case ANIM_BATTLER_DEF_PARTNER:
        spriteId = GetAnimBattlerSpriteId(gBattleAnimArgs[0]);
        break;
    case ANIM_PLAYER_LEFT:
        position = B_POSITION_PLAYER_LEFT;
        calcSpriteId = TRUE;
        break;
    case ANIM_PLAYER_RIGHT:
        position = B_POSITION_PLAYER_RIGHT;
        calcSpriteId = TRUE;
        break;
    case ANIM_OPPONENT_LEFT:
        position = B_POSITION_OPPONENT_LEFT;
        calcSpriteId = TRUE;
        break;
    case ANIM_OPPONENT_RIGHT:
        position = B_POSITION_OPPONENT_RIGHT;
        calcSpriteId = TRUE;
        break;
    default:
        spriteId = 0xFF;
        break;
    }

    if (calcSpriteId)
    {
        battler = GetBattlerAtPosition(position);
        if (IsAnimBankSpriteVisible(battler))
            spriteId = gBattlerSpriteIds[battler];
        else
            spriteId = 0xFF;
    }

    if (spriteId != 0xFF)
        SetGrayscaleOrOriginalPalette(gSprites[spriteId].oam.paletteNum + 16, gBattleAnimArgs[1]);

    DestroyAnimVisualTask(taskId);
}

void sub_80E0EE8(u8 taskId)
{
    if (gAnimMoveTurn < 2)
        gBattleAnimArgs[7] = 0;

    if (gAnimMoveTurn == 2)
        gBattleAnimArgs[7] = 1;

    DestroyAnimVisualTask(taskId);
}
