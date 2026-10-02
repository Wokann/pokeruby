#include "global.h"
#include "constants/battle.h"
#include "battle.h"
#include "data.h"
#include "ewram.h"
#include "main.h"
#include "battle_anim.h"
#include "scanline_effect.h"
#include "task.h"
#include "trig.h"

extern u16 gBattleTypeFlags;
extern u16 gIntroSlideFlags;
extern u8 gBattleMonForms[];

static void BattleIntroSlide1(u8 taskId);
static void BattleIntroSlide2(u8 taskId);
static void BattleIntroSlide3(u8 taskId);
static void BattleIntroSlideLink(u8 taskId);

static const TaskFunc sBattleIntroSlideFuncs[] =
{
    [BATTLE_ENVIRONMENT_GRASS]      = BattleIntroSlide1,
    [BATTLE_ENVIRONMENT_LONG_GRASS] = BattleIntroSlide1,
    [BATTLE_ENVIRONMENT_SAND]       = BattleIntroSlide2,
    [BATTLE_ENVIRONMENT_UNDERWATER] = BattleIntroSlide2,
    [BATTLE_ENVIRONMENT_WATER]      = BattleIntroSlide2,
    [BATTLE_ENVIRONMENT_POND]       = BattleIntroSlide1,
    [BATTLE_ENVIRONMENT_MOUNTAIN]   = BattleIntroSlide1,
    [BATTLE_ENVIRONMENT_CAVE]       = BattleIntroSlide1,
    [BATTLE_ENVIRONMENT_BUILDING]   = BattleIntroSlide3,
    [BATTLE_ENVIRONMENT_PLAIN]      = BattleIntroSlide3,
};

#define tState data[0]
#define tEnvironment data[1]
#define tBgXOffset data[2]
#define tFramesUntilBg1Slide data[3]

void HandleIntroSlide(u8 environment)
{
    u8 taskId;

    if (gBattleTypeFlags & BATTLE_TYPE_LINK)
    {
        taskId = CreateTask(BattleIntroSlideLink, 0);
    }
    else if ((gBattleTypeFlags & BATTLE_TYPE_KYOGRE_GROUDON) && gGameVersion != VERSION_RUBY)
    {
        environment = BATTLE_ENVIRONMENT_UNDERWATER;
        taskId = CreateTask(BattleIntroSlide2, 0);
    }
    else
    {
        taskId = CreateTask(sBattleIntroSlideFuncs[environment], 0);
    }

    gTasks[taskId].tState = 0;
    gTasks[taskId].tEnvironment = environment;
    gTasks[taskId].tBgXOffset = 0;
    gTasks[taskId].tFramesUntilBg1Slide = 0;
    gTasks[taskId].data[4] = 0;
    gTasks[taskId].data[5] = 0;
    gTasks[taskId].data[6] = 0;
}

static void BattleIntroSlideEnd(u8 taskId)
{
    DestroyTask(taskId);
    gBattle_BG1_X = 0;
    gBattle_BG1_Y = 0;
    gBattle_BG2_X = 0;
    gBattle_BG2_Y = 0;
    REG_BLDCNT = 0;
    REG_BLDALPHA = 0;
    REG_BLDY = 0;
    REG_WININ = WININ_WIN0_BG_ALL | WININ_WIN0_OBJ | WININ_WIN0_CLR | WININ_WIN1_BG_ALL | WININ_WIN1_OBJ | WININ_WIN1_CLR;
    REG_WINOUT = WINOUT_WIN01_BG_ALL | WINOUT_WIN01_OBJ | WINOUT_WIN01_CLR | WINOUT_WINOBJ_BG_ALL | WINOUT_WINOBJ_OBJ | WINOUT_WINOBJ_CLR;
}

static void BattleIntroSlide1(u8 taskId)
{
    s32 scanline;

    gBattle_BG1_X += 6;

    switch (gTasks[taskId].tState)
    {
    case 0:
        if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        {
            gTasks[taskId].tBgXOffset = 16;
            gTasks[taskId].tState++;
        }
        else
        {
            gTasks[taskId].tBgXOffset = 1;
            gTasks[taskId].tState++;
        }
        break;
    case 1:
        gTasks[taskId].tBgXOffset--;
        if (gTasks[taskId].tBgXOffset == 0)
        {
            gTasks[taskId].tState++;
            REG_WININ = WININ_WIN0_BG_ALL | WININ_WIN0_OBJ | WININ_WIN0_CLR;
        }
        break;
    case 2:
        // Open up the window
        gBattle_WIN0V -= WIN_RANGE(1, 0);  // decrement min Y
        gBattle_WIN0V += WIN_RANGE(0, 1);  // increment max Y
        if ((gBattle_WIN0V & 0xFF00) == 0x3000)
        {
            gTasks[taskId].tState++;
            gTasks[taskId].tBgXOffset = DISPLAY_WIDTH;
            gTasks[taskId].tFramesUntilBg1Slide = 32;
            gIntroSlideFlags &= ~1;
        }
        break;
    case 3:
        if (gTasks[taskId].tFramesUntilBg1Slide != 0)
        {
            gTasks[taskId].tFramesUntilBg1Slide--;
        }
        else
        {
            if (gTasks[taskId].tEnvironment == BATTLE_ENVIRONMENT_LONG_GRASS)
            {
                if (gBattle_BG1_Y != (u16)(-80))
                    gBattle_BG1_Y -= 2;
            }
            else
            {
                if (gBattle_BG1_Y != (u16)(-56))
                    gBattle_BG1_Y -= 1;
            }
        }

        if ((gBattle_WIN0V & 0xFF00) != 0)
            gBattle_WIN0V -= 0x3FC;

        if (gTasks[taskId].tBgXOffset != 0)
            gTasks[taskId].tBgXOffset -= 2;

        // Slide in the top half of the BG from the left
        for (scanline = 0; scanline < DISPLAY_HEIGHT / 2; scanline++)
            gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][scanline] = gTasks[taskId].tBgXOffset;
        // Slide in the bottom half of the BG from the right
        for (; scanline < DISPLAY_HEIGHT; scanline++)
            gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][scanline] = -gTasks[taskId].tBgXOffset;

        if (gTasks[taskId].tBgXOffset == 0)
        {
            gScanlineEffect.state = 3;
            gTasks[taskId].tState++;
            CpuFill32(0, BG_SCREEN_ADDR(28), 0x800);
            REG_BG1CNT = BGCNT_PRIORITY(0) | BGCNT_CHARBASE(0) | BGCNT_16COLOR | BGCNT_SCREENBASE(28) | BGCNT_TXT256x512;
            REG_BG2CNT = BGCNT_PRIORITY(0) | BGCNT_CHARBASE(0) | BGCNT_16COLOR | BGCNT_SCREENBASE(30) | BGCNT_TXT512x256;
        }
        break;
    case 4:
        BattleIntroSlideEnd(taskId);
        break;
    }
}

#define tBlendAlpha data[4]
#define tBlendTimer data[5]
#define tWaterWavePhase data[6]

static void BattleIntroSlide2(u8 taskId)
{
    s32 scanline;

    switch (gTasks[taskId].tEnvironment)
    {
    case BATTLE_ENVIRONMENT_SAND:
    case BATTLE_ENVIRONMENT_WATER:
        gBattle_BG1_X += 8;
        break;
    case BATTLE_ENVIRONMENT_UNDERWATER:
        gBattle_BG1_X += 6;
        break;
    }

    if (gTasks[taskId].tEnvironment == BATTLE_ENVIRONMENT_WATER)
    {
        gBattle_BG1_Y = Cos2(gTasks[taskId].tWaterWavePhase) / 512 - 8;
        if (gTasks[taskId].tWaterWavePhase < 180)
            gTasks[taskId].tWaterWavePhase += 4;
        else
            gTasks[taskId].tWaterWavePhase += 6;
        if (gTasks[taskId].tWaterWavePhase == 360)
            gTasks[taskId].tWaterWavePhase = 0;
    }

    switch (gTasks[taskId].tState)
    {
    case 0:
        gTasks[taskId].tBlendAlpha = 16;
        if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        {
            gTasks[taskId].tBgXOffset = 16;
            gTasks[taskId].tState++;
        }
        else
        {
            gTasks[taskId].tBgXOffset = 1;
            gTasks[taskId].tState++;
        }
        break;
    case 1:
        gTasks[taskId].tBgXOffset--;
        if (gTasks[taskId].tBgXOffset == 0)
        {
            gTasks[taskId].tState++;
            REG_WININ = WININ_WIN0_BG_ALL | WININ_WIN0_OBJ | WININ_WIN0_CLR;
        }
        break;
    case 2:
        // Open up window
        gBattle_WIN0V -= WIN_RANGE(1, 0);  // decrement min Y
        gBattle_WIN0V += WIN_RANGE(0, 1);  // increment max Y
        if ((gBattle_WIN0V & 0xFF00) == 0x3000)
        {
            gTasks[taskId].tState++;
            gTasks[taskId].tBgXOffset = DISPLAY_WIDTH;
            gTasks[taskId].tFramesUntilBg1Slide = 32;
            gTasks[taskId].tBlendTimer = 1;
            gIntroSlideFlags &= ~1;
        }
        break;
    case 3:
        if (gTasks[taskId].tFramesUntilBg1Slide != 0)
        {
            gTasks[taskId].tFramesUntilBg1Slide--;
            if (gTasks[taskId].tFramesUntilBg1Slide == 0)
            {
                REG_BLDCNT = BLDCNT_TGT1_BG1 | BLDCNT_EFFECT_BLEND | BLDCNT_TGT2_BG3 | BLDCNT_TGT2_OBJ;
                REG_BLDALPHA = BLDALPHA_BLEND(15, 0);
                REG_BLDY = 0;
            }
        }
        else
        {
            if ((gTasks[taskId].tBlendAlpha & 0x1F) && --gTasks[taskId].tBlendTimer == 0)
            {
                gTasks[taskId].tBlendAlpha += 0xFF;
                gTasks[taskId].tBlendTimer = 4;
            }
        }

        if ((gBattle_WIN0V & 0xFF00) != 0)
            gBattle_WIN0V -= 0x3FC;

        if (gTasks[taskId].tBgXOffset != 0)
            gTasks[taskId].tBgXOffset -= 2;

        // Slide in the top half of the BG from the left
        for (scanline = 0; scanline < DISPLAY_HEIGHT / 2; scanline++)
            gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][scanline] = gTasks[taskId].tBgXOffset;
        // Slide in the bottom half of the BG from the right
        for (; scanline < DISPLAY_HEIGHT; scanline++)
            gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][scanline] = -gTasks[taskId].tBgXOffset;

        if (gTasks[taskId].tBgXOffset == 0)
        {
            gScanlineEffect.state = 3;
            gTasks[taskId].tState++;
            CpuFill32(0, BG_SCREEN_ADDR(28), 0x800);
            REG_BG1CNT = BGCNT_PRIORITY(0) | BGCNT_CHARBASE(0) | BGCNT_16COLOR | BGCNT_SCREENBASE(28) | BGCNT_TXT256x512;
            REG_BG2CNT = BGCNT_PRIORITY(0) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(30) | BGCNT_TXT512x256;
        }
        break;
    case 4:
        BattleIntroSlideEnd(taskId);
        break;
    }

    if (gTasks[taskId].tState != 4)
        REG_BLDALPHA = gTasks[taskId].tBlendAlpha;
}

static void BattleIntroSlide3(u8 taskId)
{
    s32 scanline;

    gBattle_BG1_X += 8;

    switch (gTasks[taskId].tState)
    {
    case 0:
        REG_BLDCNT = BLDCNT_TGT1_BG1 | BLDCNT_EFFECT_BLEND | BLDCNT_TGT2_BG3 | BLDCNT_TGT2_OBJ;
        REG_BLDALPHA = BLDALPHA_BLEND(8, 8);
        REG_BLDY = 0;
        gTasks[taskId].tBlendAlpha = BLDALPHA_BLEND(8, 8);
        if (gBattleTypeFlags & BATTLE_TYPE_LINK)
        {
            gTasks[taskId].tBgXOffset = 16;
            gTasks[taskId].tState++;
        }
        else
        {
            gTasks[taskId].tBgXOffset = 1;
            gTasks[taskId].tState++;
        }
        break;
    case 1:
        gTasks[taskId].tBgXOffset--;
        if (gTasks[taskId].tBgXOffset == 0)
        {
            gTasks[taskId].tState++;
            REG_WININ = WININ_WIN0_BG_ALL | WININ_WIN0_OBJ | WININ_WIN0_CLR;
        }
        break;
    case 2:
        // Open up window
        gBattle_WIN0V -= WIN_RANGE(1, 0);  // decrement min Y
        gBattle_WIN0V += WIN_RANGE(0, 1);  // increment max Y
        if ((gBattle_WIN0V & 0xFF00) == 0x3000)
        {
            gTasks[taskId].tState++;
            gTasks[taskId].tBgXOffset = DISPLAY_WIDTH;
            gTasks[taskId].tFramesUntilBg1Slide = 32;
            gTasks[taskId].tBlendTimer = 1;
            gIntroSlideFlags &= ~1;
        }
        break;
    case 3:
        if (gTasks[taskId].tFramesUntilBg1Slide != 0)
        {
            gTasks[taskId].tFramesUntilBg1Slide--;
        }
        else
        {
            if ((gTasks[taskId].tBlendAlpha & 0xF) && --gTasks[taskId].tBlendTimer == 0)
            {
                gTasks[taskId].tBlendAlpha += 0xFF;
                gTasks[taskId].tBlendTimer = 6;
            }
        }

        if ((gBattle_WIN0V & 0xFF00) != 0)
            gBattle_WIN0V -= 0x3FC;

        if (gTasks[taskId].tBgXOffset != 0)
            gTasks[taskId].tBgXOffset -= 2;

        // Slide in the top half of the BG from the left
        for (scanline = 0; scanline < DISPLAY_HEIGHT / 2; scanline++)
            gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][scanline] = gTasks[taskId].tBgXOffset;
        // Slide in the bottom half of the BG from the right
        for (; scanline < DISPLAY_HEIGHT; scanline++)
            gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][scanline] = -gTasks[taskId].tBgXOffset;

        if (gTasks[taskId].tBgXOffset == 0)
        {
            gScanlineEffect.state = 3;
            gTasks[taskId].tState++;
            CpuFill32(0, BG_SCREEN_ADDR(28), 0x800);
            REG_BG1CNT = BGCNT_PRIORITY(0) | BGCNT_CHARBASE(0) | BGCNT_16COLOR | BGCNT_SCREENBASE(28) | BGCNT_TXT256x512;
            REG_BG2CNT = BGCNT_PRIORITY(0) | BGCNT_CHARBASE(0) | BGCNT_16COLOR | BGCNT_SCREENBASE(30) | BGCNT_TXT512x256;
        }
        break;
    case 4:
        BattleIntroSlideEnd(taskId);
        break;
    }

    if (gTasks[taskId].tState != 4)
        REG_BLDALPHA = gTasks[taskId].tBlendAlpha;
}

#undef tBlendAlpha
#undef tBlendTimer
#undef tWaterWavePhase

// Used for link battles.
#define tTilemapCleared data[4]

static void BattleIntroSlideLink(u8 taskId)
{
    s32 scanline;

    if (gTasks[taskId].tState > 1 && gTasks[taskId].tTilemapCleared == FALSE)
    {
        u16 bg1XOverflow = gBattle_BG1_X & 0x8000;

        if (bg1XOverflow || gBattle_BG1_X < 80)
        {
            gBattle_BG1_X += 3;
            gBattle_BG2_X -= 3;
        }
        else
        {
            CpuFill32(0, BG_SCREEN_ADDR(28), 0x800);
            CpuFill32(0, BG_SCREEN_ADDR(30), 0x800);
            gTasks[taskId].tTilemapCleared = TRUE;
        }
    }

    switch (gTasks[taskId].tState)
    {
    case 0:
        gTasks[taskId].tBgXOffset = 16;
        gTasks[taskId].tState++;
        break;
    case 1:
        gTasks[taskId].tBgXOffset--;
        if (gTasks[taskId].tBgXOffset == 0)
        {
            gTasks[taskId].tState++;
            gSprites[gBattleStruct->linkBattleVsSpriteId_V].oam.objMode = ST_OAM_OBJ_WINDOW;
            gSprites[gBattleStruct->linkBattleVsSpriteId_V].callback = SpriteCB_VsLetterInit;
            gSprites[gBattleStruct->linkBattleVsSpriteId_S].oam.objMode = ST_OAM_OBJ_WINDOW;
            gSprites[gBattleStruct->linkBattleVsSpriteId_S].callback = SpriteCB_VsLetterInit;
            REG_WININ = WININ_WIN0_BG_ALL | WININ_WIN0_OBJ | WININ_WIN0_CLR;
            REG_WINOUT = WINOUT_WINOBJ_BG_ALL | WINOUT_WINOBJ_OBJ | WINOUT_WINOBJ_CLR | WINOUT_WIN01_BG1 | WINOUT_WIN01_BG2;
        }
        break;
    case 2:
        // Open up window
        gBattle_WIN0V -= WIN_RANGE(1, 0);  // decrement min Y
        gBattle_WIN0V += WIN_RANGE(0, 1);  // increment max Y
        if ((gBattle_WIN0V & 0xFF00) == 0x3000)
        {
            gTasks[taskId].tState++;
            gTasks[taskId].tBgXOffset = DISPLAY_WIDTH;
            gTasks[taskId].tFramesUntilBg1Slide = 32;
            gIntroSlideFlags &= ~1;
        }
        break;
    case 3:
        if ((gBattle_WIN0V & 0xFF00) != 0)
            gBattle_WIN0V -= 0x3FC;

        if (gTasks[taskId].tBgXOffset != 0)
            gTasks[taskId].tBgXOffset -= 2;

        // Slide in the top half of the BG from the left
        for (scanline = 0; scanline < DISPLAY_HEIGHT / 2; scanline++)
            gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][scanline] = gTasks[taskId].tBgXOffset;
        // Slide in the bottom half of the BG from the right
        for (; scanline < DISPLAY_HEIGHT; scanline++)
            gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer][scanline] = -gTasks[taskId].tBgXOffset;

        if (gTasks[taskId].tBgXOffset == 0)
        {
            gScanlineEffect.state = 3;
            gTasks[taskId].tState++;
            REG_BG1CNT = BGCNT_PRIORITY(0) | BGCNT_CHARBASE(0) | BGCNT_16COLOR | BGCNT_SCREENBASE(28) | BGCNT_TXT256x512;
            REG_BG2CNT = BGCNT_PRIORITY(0) | BGCNT_CHARBASE(0) | BGCNT_16COLOR | BGCNT_SCREENBASE(30) | BGCNT_TXT512x256;
        }
        break;
    case 4:
        BattleIntroSlideEnd(taskId);
        break;
    }
}

#undef tTilemapCleared

void DrawBattlerOnBgDefault(u8 x, u8 y, u8 battlerPosition, u8 paletteId)
{
    DrawBattlerOnBg(x, y, battlerPosition, paletteId, 0x4000, 30, 1);
}

void DrawBattlerOnBg(u8 x, u8 y, u8 battlerPosition, u8 paletteId, u16 tilesOffset, u8 screenBaseBlock, u8 charBaseBlock)
{
    u32 tileOffset;
    s32 tileX;
    s32 tileY;
    u8 battler = GetBattlerAtPosition(battlerPosition);

    DmaCopy16(3, gMonSpriteGfxBufferPtrs[battlerPosition] + gBattleMonForms[battler] * 0x800, (void *)(VRAM + tilesOffset), 0x800);
    tileOffset = tilesOffset / 32 - charBaseBlock * 512;
    for (tileY = y; tileY < y + 8; tileY++)
    {
        for (tileX = x; tileX < x + 8; tileX++)
        {
            *(u16 *)(VRAM + tileX * 2 + screenBaseBlock * 0x800 + tileY * 64) = tileOffset | (paletteId << 12);
            tileOffset++;
        }
    }
}

void DrawBattlerOnBgDMA(u8 x, u8 y, u8 battlerPosition, u8 form, u8 paletteId, u16 tilesOffset, u8 screenBaseBlock, u8 charBaseBlock)
{
    u32 tileOffset;
    s32 tileX;
    s32 tileY;

    DmaCopy16(3, gMonSpriteGfxBufferPtrs[battlerPosition] + form * 0x800, (void *)(VRAM + tilesOffset), 0x800);
    tileOffset = tilesOffset / 32 - charBaseBlock * 512;
    for (tileY = y; tileY < y + 8; tileY++)
    {
        for (tileX = x; tileX < x + 8; tileX++)
        {
            *(u16 *)(VRAM + tileX * 2 + screenBaseBlock * 0x800 + tileY * 64) = tileOffset | (paletteId << 12);
            tileOffset++;
        }
    }
}
