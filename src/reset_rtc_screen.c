#include "global.h"
#include "constants/rgb.h"
#include "reset_rtc_screen.h"
#include "event_data.h"
#include "main.h"
#include "menu.h"
#include "palette.h"
#include "rtc.h"
#include "save.h"
#include "script.h"
#include "sprite.h"
#include "constants/songs.h"
#include "sound.h"
#include "string_util.h"
#include "strings.h"
#include "strings2.h"
#include "task.h"
#include "text.h"
#include "scanline_effect.h"

#define PALTAG_ARROW 0x1000

// Task data for the Task_ResetRtc_* series of tasks, when setting the time on the clock
// Data from these tasks is also used by the cursors and the main task (Task_ResetRtcScreen)
enum {
    DATAIDX_DAYS = 3,
    DATAIDX_HOURS,
    DATAIDX_MINS,
    DATAIDX_SECS,
    DATAIDX_CONFIRM,
};
#define tFinished  data[0]
#define tSetTime   data[1]
#define tSelection data[2]
#define tDays      data[DATAIDX_DAYS]
#define tHours     data[DATAIDX_HOURS]
#define tMinutes   data[DATAIDX_MINS]
#define tSeconds   data[DATAIDX_SECS]
#define tConfirm   data[DATAIDX_CONFIRM]

enum {
    SELECTION_DAYS = 1,
    SELECTION_HOURS,
    SELECTION_MINS,
    SELECTION_SECS,
    SELECTION_CONFIRM,
    SELECTION_NONE
};

struct ResetRtcInputMap
{
    /*0x0*/ u8 dataIndex;
    /*0x2*/ u16 minVal;
    /*0x4*/ u16 maxVal;
    /*0x6*/ u8 left;
    /*0x7*/ u8 right;
    /*0x8*/ u8 unk; // never read
};

static void CB2_ResetRtcScreen(void);
static void VBlankCB_ResetRtcScreen(void);
static void Task_ResetRtcScreen(u8 taskId);

static const struct ResetRtcInputMap sInputMap[] =
{
    [SELECTION_DAYS - 1] = {
        .dataIndex = DATAIDX_DAYS,
        .minVal = 1,
        .maxVal = 9999,
        .left = 0,
        .right = SELECTION_HOURS,
        .unk = 0,
    },
    [SELECTION_HOURS - 1] = {
        .dataIndex = DATAIDX_HOURS,
        .minVal = 0,
        .maxVal = HOURS_PER_DAY - 1,
        .left = SELECTION_DAYS,
        .right = SELECTION_MINS,
        .unk = 0,
    },
    [SELECTION_MINS - 1] = {
        .dataIndex = DATAIDX_MINS,
        .minVal = 0,
        .maxVal = MINUTES_PER_HOUR - 1,
        .left = SELECTION_HOURS,
        .right = SELECTION_SECS,
        .unk = 0,
    },
    [SELECTION_SECS - 1] = {
        .dataIndex = DATAIDX_SECS,
        .minVal = 0,
        .maxVal = SECONDS_PER_MINUTE - 1,
        .left = SELECTION_MINS,
        .right = SELECTION_CONFIRM,
        .unk = 0,
    },
    [SELECTION_CONFIRM - 1] = {
        .dataIndex = DATAIDX_CONFIRM,
        .minVal = 0,
        .maxVal = 0,
        .left = SELECTION_SECS,
        .right = 0,
        .unk = 6,
    },
};

static const struct OamData sOamData_Arrow =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(8x8),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(8x8),
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};

static const u8 sArrowDown_Gfx[] = INCBIN_U8("graphics/reset_rtc_screen/arrow_down.4bpp");
static const u8 sArrowRight_Gfx[] = INCBIN_U8("graphics/reset_rtc_screen/arrow_right.4bpp");

static const u16 sArrow_Pal[] = INCBIN_U16("graphics/reset_rtc_screen/arrow.gbapal");

static const struct SpriteFrameImage sPicTable_Arrow[] =
{
    obj_frame_tiles(sArrowDown_Gfx),
    obj_frame_tiles(sArrowRight_Gfx),
};

static const struct SpritePalette sSpritePalette_Arrow =
{
    .data = sArrow_Pal,
    .tag = PALTAG_ARROW,
};

static const union AnimCmd sAnim_Arrow_Down[] =
{
    ANIMCMD_FRAME(0, 30),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sAnim_Arrow_Up[] =
{
    ANIMCMD_FRAME(0, 30, .vFlip = TRUE),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sAnim_Arrow_Right[] =
{
    ANIMCMD_FRAME(1, 30),
    ANIMCMD_JUMP(0),
};

enum {
    ARROW_DOWN,
    ARROW_UP,
    ARROW_RIGHT,
};

static const union AnimCmd *const sAnims_Arrow[] =
{
    [ARROW_DOWN]  = sAnim_Arrow_Down,
    [ARROW_UP]    = sAnim_Arrow_Up,
    [ARROW_RIGHT] = sAnim_Arrow_Right,
};

static const struct SpriteTemplate sSpriteTemplate_Arrow =
{
    .tileTag = TAG_NONE,
    .paletteTag = PALTAG_ARROW,
    .oam = &sOamData_Arrow,
    .anims = sAnims_Arrow,
    .images = sPicTable_Arrow,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

const u8 gText_Colon3[] = _(" : ");

#define sTaskId data[0]
#define sState  data[1]

static void SpriteCB_Cursor_UpOrRight(struct Sprite *sprite)
{
    int state = gTasks[sprite->sTaskId].tSelection;
    if (state != sprite->sState)
    {
        sprite->sState = state;
        switch (state)
        {
        case SELECTION_DAYS:
            sprite->invisible = FALSE;
            sprite->animNum = ARROW_UP;
            sprite->animDelayCounter = 0;
            sprite->x = 53;
            sprite->y = 68;
            break;
        case SELECTION_HOURS:
            sprite->invisible = FALSE;
            sprite->animNum = ARROW_UP;
            sprite->animDelayCounter = 0;
            sprite->x = 86;
            sprite->y = 68;
            break;
        case SELECTION_MINS:
            sprite->invisible = FALSE;
            sprite->animNum = ARROW_UP;
            sprite->animDelayCounter = 0;
            sprite->x = 107;
            sprite->y = 68;
            break;
        case SELECTION_SECS:
            sprite->invisible = FALSE;
            sprite->animNum = ARROW_UP;
            sprite->animDelayCounter = 0;
            sprite->x = 128;
            sprite->y = 68;
            break;
        case SELECTION_CONFIRM:
            sprite->invisible = FALSE;
            sprite->animNum = ARROW_RIGHT;
            sprite->animDelayCounter = 0;
            sprite->x = 155;
            sprite->y = 80;
            break;
        case SELECTION_NONE:
            DestroySprite(sprite);
            break;
        }
    }
}

static void SpriteCB_Cursor_Down(struct Sprite *sprite)
{
    int state = gTasks[sprite->sTaskId].tSelection;
    if (state != sprite->sState)
    {
        sprite->sState = state;
        switch (state)
        {
        case SELECTION_DAYS:
            sprite->invisible = FALSE;
            sprite->animNum = ARROW_DOWN;
            sprite->animDelayCounter = 0;
            sprite->x = 53;
            sprite->y = 92;
            break;
        case SELECTION_HOURS:
            sprite->invisible = FALSE;
            sprite->animNum = ARROW_DOWN;
            sprite->animDelayCounter = 0;
            sprite->x = 86;
            sprite->y = 92;
            break;
        case SELECTION_MINS:
            sprite->invisible = FALSE;
            sprite->animNum = ARROW_DOWN;
            sprite->animDelayCounter = 0;
            sprite->x = 107;
            sprite->y = 92;
            break;
        case SELECTION_SECS:
            sprite->invisible = FALSE;
            sprite->animNum = ARROW_DOWN;
            sprite->animDelayCounter = 0;
            sprite->x = 128;
            sprite->y = 92;
            break;
        case SELECTION_CONFIRM:
            sprite->invisible = TRUE;
            break;
        case SELECTION_NONE:
            DestroySprite(sprite);
            break;
        }
    }
}

static void ResetRtcScreen_CreateCursor(u8 taskId)
{
    u32 spriteId;

    LoadSpritePalette(&sSpritePalette_Arrow);

    spriteId = CreateSpriteAtEnd(&sSpriteTemplate_Arrow, 53, 68, 0);
    gSprites[spriteId].callback = SpriteCB_Cursor_UpOrRight;
    gSprites[spriteId].sTaskId = taskId;
    gSprites[spriteId].sState = -1;

    spriteId = CreateSpriteAtEnd(&sSpriteTemplate_Arrow, 53, 68, 0);
    gSprites[spriteId].callback = SpriteCB_Cursor_Down;
    gSprites[spriteId].sTaskId = taskId;
    gSprites[spriteId].sState = -1;
}

static void ResetRtcScreen_FreeCursorPalette(void)
{
    FreeSpritePaletteByTag(sSpritePalette_Arrow.tag);
}

static void ResetRtcScreen_HideChooseTimeWindow(void)
{
    Menu_EraseWindowRect(3, 8, 25, 11);
}

static void ResetRtcScreen_PrintTime(u8 x, u8 y, u16 days, u8 hours, u8 minutes, u8 seconds)
{
    u8 *dest = gStringVar4;
    days %= 10000;
    hours %= HOURS_PER_DAY;
    minutes %= MINUTES_PER_HOUR;
    seconds %= SECONDS_PER_MINUTE;
    AlignInt2InMenuWindow(gStringVar1, days, 24, 1);
    dest = StringCopy(dest, gStringVar1);
    dest = StringCopy(dest, gText_Day);
    AlignInt2InMenuWindow(gStringVar1, hours, 18, 1);
    dest = StringCopy(dest, gStringVar1);
    dest = StringCopy(dest, gText_Colon3);
    dest = ConvertIntToDecimalStringN(dest, minutes, STR_CONV_MODE_LEADING_ZEROS, 2);
    dest = StringCopy(dest, gText_Colon3);
    ConvertIntToDecimalStringN(dest, seconds, STR_CONV_MODE_LEADING_ZEROS, 2);
    Menu_PrintText(gStringVar4, x, y);
}

static void ResetRtcScreen_ShowChooseTimeWindow(u16 days, u8 hours, u8 minutes, u8 seconds)
{
    Menu_DrawStdWindowFrame(3, 8, 25, 11);
    Menu_PrintText(gOtherText_OK, 20, 9);
    ResetRtcScreen_PrintTime(4, 9, days, hours, minutes, seconds);
}

static bool32 ResetRtcScreen_MoveTimeUpDown(s16 *val, int minVal, int maxVal, u16 keys)
{
    if (keys & DPAD_DOWN)
    {
        (*val)--;
        if (*val < minVal)
            *val = maxVal;
    }
    else if (keys & DPAD_UP)
    {
        (*val)++;
        if (*val > maxVal)
            *val = minVal;
    }
    else if (keys & DPAD_LEFT)
    {
        *val -= 10;
        if (*val < minVal)
            *val = maxVal;
    }
    else if (keys & DPAD_RIGHT)
    {
        *val += 10;
        if (*val > maxVal)
            *val = minVal;
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

static void Task_ResetRtc_SetFinished(u8 taskId)
{
    gTasks[taskId].tFinished = TRUE;
}

static void Task_ResetRtc_Exit(u8 taskId)
{
    ResetRtcScreen_HideChooseTimeWindow();
    ResetRtcScreen_FreeCursorPalette();
    gTasks[taskId].func = Task_ResetRtc_SetFinished;
}

static void Task_ResetRtc_HandleInput(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    u8 selection = tSelection;
    const struct ResetRtcInputMap *selectionInfo = &sInputMap[selection - 1];

    if (JOY_NEW(B_BUTTON))
    {
        gTasks[taskId].func = Task_ResetRtc_Exit;
        tSetTime = FALSE;
        tSelection = SELECTION_NONE;
        PlaySE(SE_SELECT);
        return;
    }

    if (JOY_NEW(DPAD_RIGHT))
    {
        if (selectionInfo->right)
        {
            tSelection = selectionInfo->right;
            PlaySE(SE_SELECT);
            return;
        }
    }

    if (JOY_NEW(DPAD_LEFT))
    {
        if (selectionInfo->left)
        {
            tSelection = selectionInfo->left;
            PlaySE(SE_SELECT);
            return;
        }
    }

    if (selection == SELECTION_CONFIRM)
    {
        if (JOY_NEW(A_BUTTON))
        {
            gLocalTime.days = tDays;
            gLocalTime.hours = tHours;
            gLocalTime.minutes = tMinutes;
            gLocalTime.seconds = tSeconds;
            PlaySE(SE_SELECT);
            gTasks[taskId].func = Task_ResetRtc_Exit;
            tSetTime = TRUE;
            tSelection = SELECTION_NONE;
        }
    }
    else if (ResetRtcScreen_MoveTimeUpDown(&data[selectionInfo->dataIndex], selectionInfo->minVal, selectionInfo->maxVal, JOY_REPEAT(DPAD_UP | DPAD_DOWN)))
    {
        PlaySE(SE_SELECT);
        ResetRtcScreen_PrintTime(4, 9, tDays, tHours, tMinutes, tSeconds);
    }
}

static void Task_ResetRtc_Init(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    tFinished = FALSE;
    tDays = gLocalTime.days;
    tHours = gLocalTime.hours;
    tMinutes = gLocalTime.minutes;
    tSeconds = gLocalTime.seconds;
    ResetRtcScreen_ShowChooseTimeWindow(tDays, tHours, tMinutes, tSeconds);
    ResetRtcScreen_CreateCursor(taskId);
    tSelection = SELECTION_HOURS;
    gTasks[taskId].func = Task_ResetRtc_HandleInput;
}

void CB2_InitResetRtcScreen(void)
{
    REG_DISPCNT = 0;
    SetVBlankCallback(NULL);
    DmaClear16(3, PLTT, PLTT_SIZE);
    DmaFill16Large(3, 0, (u8 *)VRAM, VRAM_SIZE, 0x1000);
    ResetOamRange(0, 128);
    LoadOam();
    ScanlineEffect_Stop();
    ScanlineEffect_Clear();
    ResetSpriteData();
    ResetTasks();
    ResetPaletteFade();
    Text_LoadWindowTemplate(&gMenuTextWindowTemplate);
    InitMenuWindow(&gMenuTextWindowTemplate);
    REG_DISPCNT = DISPCNT_BG0_ON | DISPCNT_OBJ_ON;
    SetVBlankCallback(VBlankCB_ResetRtcScreen);
    SetMainCallback2(CB2_ResetRtcScreen);
    CreateTask(Task_ResetRtcScreen, 80);
}

static void CB2_ResetRtcScreen(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void VBlankCB_ResetRtcScreen(void)
{
    ProcessSpriteCopyRequests();
    LoadOam();
    TransferPlttBuffer();
}

static void ResetRtcScreen_ShowMessage(const u8 *str)
{
    Menu_DisplayDialogueFrame();
    Menu_PrintText(str, 2, 15);
}

#define tState data[0]

static void Task_ShowResetRtcPrompt(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    switch (tState)
    {
    case 0:
        Menu_EraseScreen();
        Menu_DrawStdWindowFrame(0, 0, 20, 10);
        Menu_PrintText(gText_PresentTime, 1, 1);
        ResetRtcScreen_PrintTime(
            1,
            3,
            gLocalTime.days,
            gLocalTime.hours,
            gLocalTime.minutes,
            gLocalTime.seconds);
        Menu_PrintText(gText_PreviousTime, 1, 5);
        ResetRtcScreen_PrintTime(
            1,
            7,
            gSaveBlock2.lastBerryTreeUpdate.days,
            gSaveBlock2.lastBerryTreeUpdate.hours,
            gSaveBlock2.lastBerryTreeUpdate.minutes,
            gSaveBlock2.lastBerryTreeUpdate.seconds);
        ResetRtcScreen_ShowMessage(gText_ResetRTCConfirmCancel);
        tState++;
    case 1:
        if (JOY_NEW(B_BUTTON))
        {
            DestroyTask(taskId);
            DoSoftReset();
        }
        else if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            DestroyTask(taskId);
        }
        break;
    }
}

#undef tState

enum {
    MAINSTATE_FADE_IN,
    MAINSTATE_CHECK_SAVE,
    MAINSTATE_START_SET_TIME,
    MAINSTATE_WAIT_SET_TIME,
    MAINSTATE_SAVE,
    MAINSTATE_WAIT_EXIT,
    MAINSTATE_EXIT,
};

#define tState data[0]
#define tSubTaskId data[1]

static void Task_ResetRtcScreen(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    switch (tState)
    {
    case MAINSTATE_FADE_IN:
        BeginNormalPaletteFade(PALETTES_ALL, 1, 16, 0, RGB_WHITEALPHA);
        tState = MAINSTATE_CHECK_SAVE;
        break;
    case MAINSTATE_CHECK_SAVE:
        if (!gPaletteFade.active)
        {
            if (gSaveFileStatus == SAVE_STATUS_EMPTY || gSaveFileStatus == SAVE_STATUS_CORRUPT)
            {
                ResetRtcScreen_ShowMessage(gText_NoSaveFileCantSetTime);
                tState = MAINSTATE_WAIT_EXIT;
            }
            else
            {
                RtcCalcLocalTime();
                tSubTaskId = CreateTask(Task_ShowResetRtcPrompt, 80);
                tState = MAINSTATE_START_SET_TIME;
            }
        }
        break;
    case MAINSTATE_START_SET_TIME:
        if (gTasks[tSubTaskId].isActive != TRUE)
        {
            Menu_EraseScreen();
            ResetRtcScreen_ShowMessage(gText_PleaseResetTime);
            gLocalTime = gSaveBlock2.lastBerryTreeUpdate;
            tSubTaskId = CreateTask(Task_ResetRtc_Init, 80);
            tState = MAINSTATE_WAIT_SET_TIME;
        }
        break;
    case MAINSTATE_WAIT_SET_TIME:
        if (gTasks[tSubTaskId].tFinished)
        {
            if (!gTasks[tSubTaskId].tSetTime)
            {
                DestroyTask(tSubTaskId);
                tState = MAINSTATE_START_SET_TIME;
            }
            else
            {
                DestroyTask(tSubTaskId);
                RtcReset();
                RtcCalcLocalTimeOffset(
                    gLocalTime.days,
                    gLocalTime.hours,
                    gLocalTime.minutes,
                    gLocalTime.seconds);
                gSaveBlock2.lastBerryTreeUpdate = gLocalTime;
                VarSet(VAR_DAYS, gLocalTime.days);
                DisableResetRTC();
                ResetRtcScreen_ShowMessage(gText_ClockHasBeenReset);
                tState = MAINSTATE_SAVE;
            }
        }
        break;
    case MAINSTATE_SAVE:
        if (TrySavingData(SAVE_NORMAL) == SAVE_STATUS_OK)
        {
            ResetRtcScreen_ShowMessage(gText_SaveCompleted);
            PlaySE(SE_DING_DONG);
        }
        else
        {
            ResetRtcScreen_ShowMessage(gText_SaveFailed);
            PlaySE(SE_BOO);
        }
        tState = MAINSTATE_WAIT_EXIT;
        // fall through
    case MAINSTATE_WAIT_EXIT:
        if (JOY_NEW(A_BUTTON))
        {
            BeginNormalPaletteFade(PALETTES_ALL, 1, 0, 16, RGB_WHITEALPHA);
            tState = MAINSTATE_EXIT;
        }
        else
        {
            break;
        }
        // fall through
    case MAINSTATE_EXIT:
        if (!gPaletteFade.active)
        {
            DestroyTask(taskId);
            DoSoftReset();
        }
    }
}

#undef tState
#undef tSubTaskId

#if DEBUG
void debug_sub_806F8F8(void)
{
    SetMainCallback2(CB2_InitResetRtcScreen);
}

void debug_sub_806F908(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    switch (data[0])
    {
    case 0:
        data[1] = CreateTask(Task_ResetRtc_Init, 80);
        data[0]++;
        break;
    case 1:
        if (gTasks[data[1]].tFinished != FALSE)
        {
            if (gTasks[data[1]].tSetTime == TRUE)
                RtcCalcLocalTimeOffset(gLocalTime.days, gLocalTime.hours, gLocalTime.minutes, gLocalTime.seconds);
            DestroyTask(data[1]);
            Menu_EraseScreen();
            UnlockPlayerFieldControls();
            DestroyTask(taskId);
        }
        break;
    }
}

void debug_sub_806F99C(void)
{
    RtcCalcLocalTime();
    CreateTask(debug_sub_806F908, 80);
    LockPlayerFieldControls();
}

void debug_sub_806F9B8(void)
{
    gLocalTime = gSaveBlock2.lastBerryTreeUpdate;
    CreateTask(debug_sub_806F908, 80);
    LockPlayerFieldControls();
}

static const u8 sDebugText_Days[] = DTR("にっすう", "days");
static const u8 sDebugText_Time[] = DTR("じかん", "time");
static const u8 sDebugText_GameTime[] = DTR("ゲームない　じかん", "game time");
static const u8 sDebugText_RTCTime[] = DTR("RTC　じかん", "RTC time");

void debug_sub_806F9E4(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    u8 *str = gStringVar4;

    switch (data[0])
    {
    case 0:
        data[0]++;
        break;
    case 1:
        Menu_DrawStdWindowFrame(0, 9, 29, 19);
        ConvertIntToHexStringN(str + 80, RtcGetErrorStatus(), STR_CONV_MODE_LEADING_ZEROS, 4);
        Menu_PrintText(str + 80, 2, 10);
        Menu_PrintText(sDebugText_Days, 12, 12);
        Menu_PrintText(sDebugText_Time, 20, 12);
        Menu_PrintText(sDebugText_RTCTime, 1, 14);
        Menu_PrintText(sDebugText_GameTime, 1, 16);
        data[0]++;
        break;
    case 2:
        RtcCalcLocalTime();
        FormatHexRtcTime(str);
        debug_sub_8009894(str + 20);
        FormatDecimalTime(str + 40, gLocalTime.hours, gLocalTime.minutes, gLocalTime.seconds);
        ConvertIntToDecimalStringN(str + 60, gLocalTime.days, STR_CONV_MODE_RIGHT_ALIGN, 4);
        if (gSaveBlock2.playTimeVBlanks == 0)
        {
            Menu_PrintText(str, 20, 14);
            Menu_PrintText(str + 20, 12, 14);
            Menu_PrintText(str + 40, 20, 16);
            Menu_PrintText(str + 60, 12, 16);
        }
        if (data[1] > 19)  // Did you mean < 19?
        {
            data[1]++;
        }
        else
        {
            if (JOY_NEW(A_BUTTON))
            {
                Menu_EraseScreen();
                DestroyTask(taskId);
                UnlockPlayerFieldControls();
            }
        }
        break;
    }
}
#endif
