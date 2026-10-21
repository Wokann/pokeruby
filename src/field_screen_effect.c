#include "global.h"
#include "constants/rgb.h"
#include "field_screen_effect.h"
#include "event_data.h"
#include "field_camera.h"
#include "io_reg.h"
#include "menu.h"
#include "palette.h"
#include "overworld.h"
#include "script.h"
#include "task.h"
#include "text.h"
#include "scanline_effect.h"

static void Task_EnableScriptAfterMusicFade(u8 taskId);

// data[0] is used universally by tasks in this file as a state for switches
#define tState       data[0]

static const u16 sFlashLevelToRadius[] = { 200, 72, 56, 40, 24, 0 };

const s32 gMaxFlashLevel = 4;

static const struct ScanlineEffectParams sFlashEffectParams =
{
    .dmaDest = (void *)REG_ADDR_WIN0H,
    .dmaControl = SCANLINE_EFFECT_DMACNT_16BIT,
    .initState = 1
};

static void SetFlashScanlineEffectWindowBoundary(u16 *dest, u32 y, s32 left, s32 right)
{
    if (y <= DISPLAY_HEIGHT)
    {
        if (left < 0)
            left = 0;
        if (left > 255)
            left = 255;
        if (right < 0)
            right = 0;
        if (right > 255)
            right = 255;
        dest[y] = WIN_RANGE(left, right);
    }
}

static void SetFlashScanlineEffectWindowBoundaries(u16 *dest, s32 centerX, s32 centerY, s32 radius)
{
    s32 r = radius;
    s32 v2 = radius;
    s32 v3 = 0;
    while (r >= v3)
    {
        SetFlashScanlineEffectWindowBoundary(dest, centerY - v3, centerX - r, centerX + r);
        SetFlashScanlineEffectWindowBoundary(dest, centerY + v3, centerX - r, centerX + r);
        SetFlashScanlineEffectWindowBoundary(dest, centerY - r, centerX - v3, centerX + v3);
        SetFlashScanlineEffectWindowBoundary(dest, centerY + r, centerX - v3, centerX + v3);
        v2 -= (v3 * 2) - 1;
        v3++;
        if (v2 < 0)
        {
            v2 += 2 * (r - 1);
            r--;
        }
    }
}

#define tFlashCenterX        data[1]
#define tFlashCenterY        data[2]
#define tCurFlashRadius      data[3]
#define tDestFlashRadius     data[4]
#define tFlashRadiusDelta    data[5]
#define tClearScanlineEffect data[6]

static void UpdateFlashLevelEffect(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    switch (tState)
    {
    case 0:
        SetFlashScanlineEffectWindowBoundaries(gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer], tFlashCenterX, tFlashCenterY, tCurFlashRadius);
        tState = 1;
        break;
    case 1:
        SetFlashScanlineEffectWindowBoundaries(gScanlineEffectRegBuffers[gScanlineEffect.srcBuffer], tFlashCenterX, tFlashCenterY, tCurFlashRadius);
        tState = 0;
        tCurFlashRadius += tFlashRadiusDelta;
        if (tCurFlashRadius > tDestFlashRadius)
        {
            if (tClearScanlineEffect == TRUE)
            {
                ScanlineEffect_Stop();
                tState = 2;
            }
            else
            {
                DestroyTask(taskId);
            }
        }
        break;
    case 2:
        ScanlineEffect_Clear();
        DestroyTask(taskId);
        break;
    }
}

static void Task_WaitForFlashUpdate(u8 taskId)
{
    if (!FuncIsActiveTask(UpdateFlashLevelEffect))
    {
        ScriptContext_Enable();
        DestroyTask(taskId);
    }
}

static void StartWaitForFlashUpdate(void)
{
    if (!FuncIsActiveTask(Task_WaitForFlashUpdate))
        CreateTask(Task_WaitForFlashUpdate, 80);
}

static u8 StartUpdateFlashLevelEffect(s32 centerX, s32 centerY, s32 initialFlashRadius, s32 destFlashRadius, s32 clearScanlineEffect, u8 delta)
{
    u8 taskId = CreateTask(UpdateFlashLevelEffect, 80);
    s16 *data = gTasks[taskId].data;

    tCurFlashRadius = initialFlashRadius;
    tDestFlashRadius = destFlashRadius;
    tFlashCenterX = centerX;
    tFlashCenterY = centerY;
    tClearScanlineEffect = clearScanlineEffect;

    if (initialFlashRadius < destFlashRadius)
        tFlashRadiusDelta = delta;
    else
        tFlashRadiusDelta = -delta;

    return taskId;
}

#undef tCurFlashRadius
#undef tDestFlashRadius
#undef tFlashRadiusDelta
#undef tClearScanlineEffect

void AnimateFlash(u8 newFlashLevel)
{
    u8 curFlashLevel = GetFlashLevel();
    bool8 fullBrightness = FALSE;
    if (!newFlashLevel)
        fullBrightness = TRUE;
    StartUpdateFlashLevelEffect(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, sFlashLevelToRadius[curFlashLevel], sFlashLevelToRadius[newFlashLevel], fullBrightness, 1);
    StartWaitForFlashUpdate();
    LockPlayerFieldControls();
}

void WriteFlashScanlineEffectBuffer(u8 flashLevel)
{
    if (flashLevel)
    {
        SetFlashScanlineEffectWindowBoundaries(&gScanlineEffectRegBuffers[0][0], DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, sFlashLevelToRadius[flashLevel]);
        CpuFastCopy(&gScanlineEffectRegBuffers[0], &gScanlineEffectRegBuffers[1], sizeof(gScanlineEffectRegBuffers[0]));
    }
}

static void LoadOrbEffectPalette(bool8 blueOrb)
{
    int i;
    u16 color[1];

    if (!blueOrb)
        color[0] = RGB_RED;
    else
        color[0] = RGB_BLUE;

    for (i = 0; i < 16; i++)
    {
        LoadPalette(color, BG_PLTT_ID(15) + i, PLTT_SIZEOF(1));
    }
}

static bool8 UpdateOrbEffectBlend(u16 shakeDir)
{
    u8 lo = REG_BLDALPHA & 0xFF;
    u8 hi = REG_BLDALPHA >> 8;

    if (shakeDir)
    {
        if (lo)
        {
            lo--;
        }
    }
    else
    {
        if (hi < 16)
        {
            hi++;
        }
    }

    REG_BLDALPHA = BLDALPHA_BLEND(lo, hi);

    if (lo == 0 && hi == 16)
    {
        return TRUE;
    }

    return FALSE;
}

#define tBlueOrb     data[1]
#define tCenterX     data[2]
#define tCenterY     data[3]
#define tShakeDelay  data[4]
#define tShakeDir    data[5]
#define tDispCnt     data[6]
#define tBldCnt      data[7]
#define tBldAlpha    data[8]
#define tWinIn       data[9]
#define tWinOut      data[10]

static void Task_OrbEffect(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    switch (tState)
    {
    case 0:
        tDispCnt = REG_DISPCNT;
        tBldCnt = REG_BLDCNT;
        tBldAlpha = REG_BLDALPHA;
        tWinIn = REG_WININ;
        tWinOut = REG_WINOUT;
        REG_DISPCNT &= ~DISPCNT_WIN1_ON;
        REG_BLDCNT |= gOrbEffectBackgroundLayerFlags[0];
        REG_BLDALPHA = BLDALPHA_BLEND(12, 7);
        REG_WININ = WININ_WIN0_BG_ALL | WININ_WIN0_OBJ | WININ_WIN0_CLR;
        REG_WINOUT = WINOUT_WIN01_BG1 | WINOUT_WIN01_BG2 | WINOUT_WIN01_BG3 | WINOUT_WIN01_OBJ;
        SetFlashScanlineEffectWindowBoundaries(&gScanlineEffectRegBuffers[0][0], tCenterX, tCenterY, 1);
        CpuFastCopy(&gScanlineEffectRegBuffers[0], &gScanlineEffectRegBuffers[1], sizeof(gScanlineEffectRegBuffers[0]));
        ScanlineEffect_SetParams(sFlashEffectParams);
        tState = 1;
        break;
    case 1:
        Menu_BlankWindowRect(0, 0, DISPLAY_TILE_WIDTH - 1, DISPLAY_TILE_HEIGHT - 1);
        LoadOrbEffectPalette(tBlueOrb);
        StartUpdateFlashLevelEffect(tCenterX, tCenterY, 1, 160, TRUE, 2);
        tState = 2;
        break;
    case 2:
        if (!FuncIsActiveTask(UpdateFlashLevelEffect))
        {
            ScriptContext_Enable();
            tState = 3;
        }
        break;
    case 3:
        InstallCameraPanAheadCallback();
        SetCameraPanningCallback(NULL);
        tShakeDir = 0;
        tShakeDelay = 4;
        tState = 4;
        break;
    case 4:
        tShakeDelay--;
        if (!tShakeDelay)
        {
            s32 panning;
            tShakeDelay = 4;
            tShakeDir ^= 1;
            if (tShakeDir)
                panning = 4;
            else
                panning = -4;
            SetCameraPanning(0, panning);
        }
        break;
    case 6:
        InstallCameraPanAheadCallback();
        tShakeDelay = 8;
        tState = 7;
        break;
    case 7:
        tShakeDelay--;
        if (!tShakeDelay)
        {
            tShakeDelay = 8;
            tShakeDir ^= 1;
            if (UpdateOrbEffectBlend(tShakeDir) == TRUE)
                tState = 5;
        }
        break;
    case 5:
        Menu_EraseWindowRect(0, 0, DISPLAY_TILE_WIDTH - 1, DISPLAY_TILE_HEIGHT - 1);
        LoadFontDefaultPalette(&gMenuTextWindowTemplate);
        REG_WIN0H = WIN_RANGE(0, 255);
        REG_DISPCNT = tDispCnt;
        REG_BLDCNT = tBldCnt;
        REG_BLDALPHA = tBldAlpha;
        REG_WININ = tWinIn;
        REG_WINOUT = tWinOut;
        ScriptContext_Enable();
        DestroyTask(taskId);
        break;
    }
}

void DoOrbEffect(void)
{
    u8 taskId = CreateTask(Task_OrbEffect, 80);
    s16 *data = gTasks[taskId].data;

    if (gSpecialVar_Result == 0)
    {
        tBlueOrb = FALSE;
        tCenterX = 104;
    }
    else if (gSpecialVar_Result == 1)
    {
        tBlueOrb = TRUE;
        tCenterX = 104;
    }
    else if (gSpecialVar_Result == 2)
    {
        tBlueOrb = FALSE;
        tCenterX = DISPLAY_WIDTH / 2;
    }
    else
    {
        tBlueOrb = TRUE;
        tCenterX = DISPLAY_WIDTH / 2;
    }

    tCenterY = DISPLAY_HEIGHT / 2;
}

void FadeOutOrbEffect(void)
{
    u8 taskId = FindTaskIdByFunc(Task_OrbEffect);
    gTasks[taskId].tState = 6;
}

#undef tBlueOrb
#undef tCenterX
#undef tCenterY
#undef tShakeDelay
#undef tShakeDir
#undef tDispCnt
#undef tBldCnt
#undef tBldAlpha
#undef tWinIn
#undef tWinOut

void Script_FadeOutMapMusic(void)
{
    Overworld_FadeOutMapMusic();
    CreateTask(Task_EnableScriptAfterMusicFade, 80);
}

static void Task_EnableScriptAfterMusicFade(u8 taskId)
{
    if (BGMusicStopped() == TRUE)
    {
        DestroyTask(taskId);
        ScriptContext_Enable();
    }
}
