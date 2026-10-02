#include "global.h"
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

enum
{
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

enum
{
    SELECTION_DAYS = 1,
    SELECTION_HOURS,
    SELECTION_MINS,
    SELECTION_SECS,
    SELECTION_CONFIRM,
    SELECTION_NONE,
};

enum
{
    ARROW_DOWN,
    ARROW_UP,
    ARROW_RIGHT,
};

struct ResetRtcInputMap
{
    /*0x0*/ u8 dataIndex;
    /*0x2*/ u16 minVal;
    /*0x4*/ u16 maxVal;
    /*0x6*/ u8 left;
    /*0x7*/ u8 right;
    /*0x8*/ u8 unk;
};

extern u16 gSaveFileStatus;

void CB2_ResetRtcScreen(void);
static void VBlankCB(void);
void Task_ResetRtcScreen(u8);
void Task_ResetRtc_SetFinished(u8);
void Task_ResetRtc_Exit(u8);
void Task_ResetRtc_HandleInput(u8);
void Task_ResetRtc_Init(u8);
void Task_ShowResetRtcPrompt(u8);

static const struct ResetRtcInputMap sInputMap[] =
{
    {
        .dataIndex = DATAIDX_DAYS,
        .minVal = 1,
        .maxVal = 9999,
        .left = 0,
        .right = 2,
        .unk = 0,
    },
    {
        .dataIndex = DATAIDX_HOURS,
        .minVal = 0,
        .maxVal = 23,
        .left = 1,
        .right = 3,
        .unk = 0,
    },
    {
        .dataIndex = DATAIDX_MINS,
        .minVal = 0,
        .maxVal = 59,
        .left = 2,
        .right = 4,
        .unk = 0,
    },
    {
        .dataIndex = DATAIDX_SECS,
        .minVal = 0,
        .maxVal = 59,
        .left = 3,
        .right = 5,
        .unk = 0,
    },
    {
        .dataIndex = DATAIDX_CONFIRM,
        .minVal = 0,
        .maxVal = 0,
        .left = 4,
        .right = 0,
        .unk = 6,
    },
};

const struct OamData gOamData_ResetRtcArrow =
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

const u8 gResetRtcArrowDown_Gfx[] = INCBIN_U8("graphics/reset_rtc_screen/arrow_down.4bpp");
const u8 gResetRtcArrowRight_Gfx[] = INCBIN_U8("graphics/reset_rtc_screen/arrow_right.4bpp");

const u16 gResetRtcArrow_Pal[] = INCBIN_U16("graphics/reset_rtc_screen/arrow.gbapal");

const struct SpriteFrameImage gResetRtcArrowPicTable[] =
{
    { gResetRtcArrowDown_Gfx, 0x20 },
    { gResetRtcArrowRight_Gfx, 0x20 },
};

const struct SpritePalette gSpritePalette_ResetRtcArrow =
{
    .data = gResetRtcArrow_Pal,
    .tag = PALTAG_ARROW,
};

const union AnimCmd gAnim_ResetRtcArrow_Down[] =
{
    ANIMCMD_FRAME(0, 30),
    ANIMCMD_JUMP(0),
};

const union AnimCmd gAnim_ResetRtcArrow_Up[] =
{
    ANIMCMD_FRAME(0, 30, .vFlip = TRUE),
    ANIMCMD_JUMP(0),
};

const union AnimCmd gAnim_ResetRtcArrow_Right[] =
{
    ANIMCMD_FRAME(1, 30),
    ANIMCMD_JUMP(0),
};

const union AnimCmd *const gAnims_ResetRtcArrow[] =
{
    gAnim_ResetRtcArrow_Down,
    gAnim_ResetRtcArrow_Up,
    gAnim_ResetRtcArrow_Right,
};

const struct SpriteTemplate gSpriteTemplate_ResetRtcArrow =
{
    .tileTag = TAG_NONE,
    .paletteTag = PALTAG_ARROW,
    .oam = &gOamData_ResetRtcArrow,
    .anims = gAnims_ResetRtcArrow,
    .images = gResetRtcArrowPicTable,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

const u8 gText_ResetRtcColon[] = _(" : ");

#define sTaskId data[0]
#define sState  data[1]

void SpriteCB_Cursor_UpOrRight(struct Sprite *sprite)
{
    int selection = gTasks[sprite->sTaskId].tSelection;
    if (selection != sprite->sState)
    {
        sprite->sState = selection;
        switch (selection)
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

void SpriteCB_Cursor_Down(struct Sprite *sprite)
{
    int selection = gTasks[sprite->sTaskId].tSelection;
    if (selection != sprite->sState)
    {
        sprite->sState = selection;
        switch (selection)
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

void CreateCursor(u8 taskId)
{
    int spriteId;

    LoadSpritePalette(&gSpritePalette_ResetRtcArrow);

    spriteId = CreateSpriteAtEnd(&gSpriteTemplate_ResetRtcArrow, 53, 68, 0);
    gSprites[spriteId].callback = SpriteCB_Cursor_UpOrRight;
    gSprites[spriteId].sTaskId = taskId;
    gSprites[spriteId].sState = -1;

    spriteId = CreateSpriteAtEnd(&gSpriteTemplate_ResetRtcArrow, 53, 68, 0);
    gSprites[spriteId].callback = SpriteCB_Cursor_Down;
    gSprites[spriteId].sTaskId = taskId;
    gSprites[spriteId].sState = -1;
}

#undef sTaskId
#undef sState

void FreeCursorPalette(void)
{
    FreeSpritePaletteByTag(gSpritePalette_ResetRtcArrow.tag);
}

void HideChooseTimeWindow(void)
{
    Menu_EraseWindowRect(3, 8, 25, 11);
}

void PrintTime(u8 x, u8 y, u16 days, u8 hours, u8 minutes, u8 seconds)
{
    u8 *dest = gStringVar4;
    days %= 10000;
    hours %= 24;
    minutes %= 60;
    seconds %= 60;
    AlignInt2InMenuWindow(gStringVar1, days, 24, 1);
    dest = StringCopy(dest, gStringVar1);
    dest = StringCopy(dest, gOtherText_Day);
    AlignInt2InMenuWindow(gStringVar1, hours, 18, 1);
    dest = StringCopy(dest, gStringVar1);
    dest = StringCopy(dest, gText_ResetRtcColon);
    dest = ConvertIntToDecimalStringN(dest, minutes, STR_CONV_MODE_LEADING_ZEROS, 2);
    dest = StringCopy(dest, gText_ResetRtcColon);
    ConvertIntToDecimalStringN(dest, seconds, STR_CONV_MODE_LEADING_ZEROS, 2);
    Menu_PrintText(gStringVar4, x, y);
}

void ShowChooseTimeWindow(u16 days, u8 hours, u8 minutes, u8 seconds)
{
    Menu_DrawStdWindowFrame(3, 8, 25, 11);
    Menu_PrintText(gOtherText_OK, 20, 9);
    PrintTime(4, 9, days, hours, minutes, seconds);
}

bool32 MoveTimeUpDown(s16 *val, int minVal, int maxVal, u16 keys)
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

void Task_ResetRtc_SetFinished(u8 taskId)
{
    gTasks[taskId].tFinished = TRUE;
}

void Task_ResetRtc_Exit(u8 taskId)
{
    HideChooseTimeWindow();
    FreeCursorPalette();
    gTasks[taskId].func = Task_ResetRtc_SetFinished;
}

void Task_ResetRtc_HandleInput(u8 taskId)
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
    else if (MoveTimeUpDown(&data[selectionInfo->dataIndex], selectionInfo->minVal, selectionInfo->maxVal, JOY_REPT(DPAD_UP | DPAD_DOWN)))
    {
        PlaySE(SE_SELECT);
        PrintTime(4, 9, tDays, tHours, tMinutes, tSeconds);
    }
}

void Task_ResetRtc_Init(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    tFinished = FALSE;
    tDays = gLocalTime.days;
    tHours = gLocalTime.hours;
    tMinutes = gLocalTime.minutes;
    tSeconds = gLocalTime.seconds;
    ShowChooseTimeWindow(tDays, tHours, tMinutes, tSeconds);
    CreateCursor(taskId);
    tSelection = SELECTION_HOURS;
    gTasks[taskId].func = Task_ResetRtc_HandleInput;
}

#undef tFinished
#undef tSetTime
#undef tSelection
#undef tDays
#undef tHours
#undef tMinutes
#undef tSeconds
#undef tConfirm

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
    REG_DISPCNT = DISPCNT_OBJ_ON | DISPCNT_BG0_ON;
    SetVBlankCallback(VBlankCB);
    SetMainCallback2(CB2_ResetRtcScreen);
    CreateTask(Task_ResetRtcScreen, 80);
}

void CB2_ResetRtcScreen(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void VBlankCB(void)
{
    ProcessSpriteCopyRequests();
    LoadOam();
    TransferPlttBuffer();
}

void ShowMessage(const u8 *text)
{
    Menu_DisplayDialogueFrame();
    Menu_PrintText(text, 2, 15);
}

#define tPromptState data[0]

void Task_ShowResetRtcPrompt(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    switch (tPromptState)
    {
    case 0:
        Menu_EraseScreen();
        Menu_DrawStdWindowFrame(0, 0, 20, 10);
        Menu_PrintText(gText_PresentTime, 1, 1);
        PrintTime(
            1,
            3,
            gLocalTime.days,
            gLocalTime.hours,
            gLocalTime.minutes,
            gLocalTime.seconds);
        Menu_PrintText(gText_PreviousTime, 1, 5);
        PrintTime(
            1,
            7,
            gSaveBlock2.lastBerryTreeUpdate.days,
            gSaveBlock2.lastBerryTreeUpdate.hours,
            gSaveBlock2.lastBerryTreeUpdate.minutes,
            gSaveBlock2.lastBerryTreeUpdate.seconds);
        ShowMessage(gText_ResetRTCConfirmCancel);
        tPromptState++;
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

// Task states for Task_ResetRtcScreen.
enum
{
    MAINSTATE_FADE_IN,
    MAINSTATE_CHECK_SAVE,
    MAINSTATE_START_SET_TIME,
    MAINSTATE_WAIT_SET_TIME,
    MAINSTATE_SAVE,
    MAINSTATE_WAIT_EXIT,
    MAINSTATE_EXIT,
};

#undef tPromptState

#define tState           data[0]
#define tSubTaskId       data[1]
#define tSubTaskFinished gTasks[tSubTaskId].data[0]
#define tSubTaskSetTime  gTasks[tSubTaskId].data[1]

void Task_ResetRtcScreen(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    switch (tState)
    {
    case MAINSTATE_FADE_IN:
        BeginNormalPaletteFade(PALETTES_ALL, 1, 16, 0, FADE_COLOR_WHITE);
        tState = MAINSTATE_CHECK_SAVE;
        break;
    case MAINSTATE_CHECK_SAVE:
        if (!gPaletteFade.active)
        {
            if (gSaveFileStatus == SAVE_STATUS_EMPTY || gSaveFileStatus == SAVE_STATUS_CORRUPT)
            {
                ShowMessage(gText_NoSaveFileCantSetTime);
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
            ShowMessage(gText_PleaseResetTime);
            gLocalTime = gSaveBlock2.lastBerryTreeUpdate;
            tSubTaskId = CreateTask(Task_ResetRtc_Init, 80);
            tState = MAINSTATE_WAIT_SET_TIME;
        }
        break;
    case MAINSTATE_WAIT_SET_TIME:
        if (tSubTaskFinished)
        {
            if (!tSubTaskSetTime)
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
                ShowMessage(gText_ClockHasBeenReset);
                tState = MAINSTATE_SAVE;
            }
        }
        break;
    case MAINSTATE_SAVE:
        if (TrySavingData(SAVE_NORMAL) == SAVE_STATUS_OK)
        {
            ShowMessage(gText_SaveCompleted);
            PlaySE(SE_DING_DONG);
        }
        else
        {
            ShowMessage(gSystemText_SaveFailed);
            PlaySE(SE_BOO);
        }
        tState = MAINSTATE_WAIT_EXIT;
        // fall through
    case MAINSTATE_WAIT_EXIT:
        if (JOY_NEW(A_BUTTON))
        {
            BeginNormalPaletteFade(PALETTES_ALL, 1, 0, 16, FADE_COLOR_WHITE);
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
#undef tSubTaskFinished
#undef tSubTaskSetTime

#if DEBUG
void Debug_StartResetRtcScreen(void)
{
    SetMainCallback2(CB2_InitResetRtcScreen);
}

#define tState           data[0]
#define tSubTaskId       data[1]
#define tSubTaskFinished gTasks[tSubTaskId].data[0]
#define tSubTaskSetTime  gTasks[tSubTaskId].data[1]

void Task_DebugSetRtcTime(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    switch (tState)
    {
    case 0:
        tSubTaskId = CreateTask(Task_ResetRtc_Init, 80);
        tState++;
        break;
    case 1:
        if (tSubTaskFinished != FALSE)
        {
            if (tSubTaskSetTime == TRUE)
                RtcCalcLocalTimeOffset(gLocalTime.days, gLocalTime.hours, gLocalTime.minutes, gLocalTime.seconds);
            DestroyTask(tSubTaskId);
            Menu_EraseScreen();
            UnlockPlayerFieldControls();
            DestroyTask(taskId);
        }
        break;
    }
}

#undef tState
#undef tSubTaskId
#undef tSubTaskFinished
#undef tSubTaskSetTime

void Debug_StartSetRtcTime(void)
{
    RtcCalcLocalTime();
    CreateTask(Task_DebugSetRtcTime, 80);
    LockPlayerFieldControls();
}

void Debug_StartSetRtcTimeFromLastBerryUpdate(void)
{
    gLocalTime = gSaveBlock2.lastBerryTreeUpdate;
    CreateTask(Task_DebugSetRtcTime, 80);
    LockPlayerFieldControls();
}

static const u8 sDebugText_Days[] = DTR("にっすう", "days");
static const u8 sDebugText_Time[] = DTR("じかん", "time");
static const u8 sDebugText_GameTime[] = DTR("ゲームない　じかん", "game time");
static const u8 sDebugText_RTCTime[] = DTR("RTC　じかん", "RTC time");

#define tState      data[0]
#define tInputDelay data[1]

void Task_DebugDisplayRtcInfo(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    u8 *text = gStringVar4;

    switch (tState)
    {
    case 0:
        tState++;
        break;
    case 1:
        Menu_DrawStdWindowFrame(0, 9, 29, 19);
        ConvertIntToHexStringN(text + 80, RtcGetErrorStatus(), 2, 4);
        Menu_PrintText(text + 80, 2, 10);
        Menu_PrintText(sDebugText_Days, 12, 12);
        Menu_PrintText(sDebugText_Time, 20, 12);
        Menu_PrintText(sDebugText_RTCTime, 1, 14);
        Menu_PrintText(sDebugText_GameTime, 1, 16);
        tState++;
        break;
    case 2:
        RtcCalcLocalTime();
        FormatHexRtcTime(text);
        FormatRtcDayCount(text + 20);
        FormatDecimalTime(text + 40, gLocalTime.hours, gLocalTime.minutes, gLocalTime.seconds);
        ConvertIntToDecimalStringN(text + 60, gLocalTime.days, 1, 4);
        if (gSaveBlock2.playTimeVBlanks == 0)
        {
            Menu_PrintText(text, 20, 14);
            Menu_PrintText(text + 20, 12, 14);
            Menu_PrintText(text + 40, 20, 16);
            Menu_PrintText(text + 60, 12, 16);
        }
        if (tInputDelay > 19)  // Did you mean < 19?
        {
            tInputDelay++;
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

#undef tState
#undef tInputDelay
#endif
