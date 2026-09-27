#include "global.h"
#include "clear_save_data_menu.h"
#include "main.h"
#include "menu.h"
#include "palette.h"
#include "save.h"
#include "constants/songs.h"
#include "sound.h"
#include "sprite.h"
#include "strings2.h"
#include "task.h"

static void VBlankCB_ClearSaveDataScreen(void);
static void Task_DoClearSaveDataScreenYesNo(u8);
static void Task_ClearSaveDataScreenYesNoChoice(u8);
static void Task_ClearSaveData(u8);
static void CB2_ClearSaveDataScreen(void);
static void VBlankCB_SetupClearSaveDataScreen(void);
static bool8 SetupClearSaveDataScreen(void);
static void CB2_FadeAndDoReset(void);

void CB2_InitClearSaveDataScreen(void)
{
    if (SetupClearSaveDataScreen())
    {
        CreateTask(Task_DoClearSaveDataScreenYesNo, 0);
    }
}

static void VBlankCB_ClearSaveDataScreen(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void Task_DoClearSaveDataScreenYesNo(u8 taskId)
{
    ResetSpriteData();

    REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG3_ON | DISPCNT_OBJ_ON;

    SetVBlankCallback(VBlankCB_ClearSaveDataScreen);
    Menu_DrawStdWindowFrame(2, 14, 27, 19);
    Menu_PrintText(gText_ClearAllSaveData, 3, 15);

    Menu_DrawStdWindowFrame(2, 1, 8, 6);
    Menu_PrintItems(3, 2, 2, gMenuYesNoItems);
    InitMenu(0, 3, 2, 2, 1, 5);

    gTasks[taskId].func = Task_ClearSaveDataScreenYesNoChoice;
}

static void Task_ClearSaveDataScreenYesNoChoice(u8 taskId)
{
    switch (Menu_ProcessInputNoWrap_())
    {
    case 0:
        PlaySE(SE_SELECT);
        sub_8071F40(gText_ClearingData);
        gTasks[taskId].func = Task_ClearSaveData;
        break;
    case -1:
    case 1:
        PlaySE(SE_SELECT);
        DestroyTask(taskId);
        SetMainCallback2(CB2_FadeAndDoReset);
        break;
    }
    AnimateSprites();
    BuildOamBuffer();
}

static void Task_ClearSaveData(u8 taskId)
{
    ClearSaveData();
    DestroyTask(taskId);
    SetMainCallback2(CB2_FadeAndDoReset);
}

static void CB2_ClearSaveDataScreen(void)
{
    RunTasks();
    UpdatePaletteFade();
}

static void VBlankCB_SetupClearSaveDataScreen(void)
{
    TransferPlttBuffer();
}

static bool8 SetupClearSaveDataScreen(void)
{
    u16 i;
    u16 ime;

    switch (gMain.state)
    {
    case 0:
    default:
        SetVBlankCallback(NULL);

        REG_DISPCNT = 0;
        REG_BG0HOFS = 0;
        REG_BG0VOFS = 0;
        REG_BG3HOFS = 0;
        REG_BG3VOFS = 0;
        REG_WIN0H = 0;
        REG_WIN0V = 0;
        REG_WININ = 0;
        REG_WINOUT = 0;
        REG_BLDCNT = 0;
        REG_BLDALPHA = 0;
        REG_BLDY = 0;

        DmaFill16(3, 0, (void *)VRAM, VRAM_SIZE);
        DmaFill32(3, 0, (void *)OAM, OAM_SIZE);
        DmaFill16(3, 0, (void *)(PLTT + 2), PLTT_SIZE - 2);

        ResetPaletteFade();

        gPlttBufferUnfaded[0] = 0x7fff;
        gPlttBufferFaded[0] = 0x7fff;
        gPlttBufferUnfaded[1] = 0x3945;
        gPlttBufferFaded[1] = 0x3945;

        for (i = 0; i < 0x10; i++)
            ((u16 *)(VRAM + 0x20))[i] = 0x1111;

        for (i = 0; i < 0x500; i++)
            ((u16 *)(VRAM + 0x3800))[i] = 0x0001;

        ResetTasks();
        ResetSpriteData();

        Text_LoadWindowTemplate(&gWindowTemplate_81E6C3C);
        InitMenuWindow(&gMenuTextWindowTemplate);
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, FADE_COLOR_WHITE);

        ime = REG_IME;
        REG_IME = 0;
        REG_IE |= INTR_FLAG_VBLANK;
        REG_IME = ime;
        REG_DISPSTAT |= DISPSTAT_VBLANK_INTR;

        SetVBlankCallback(VBlankCB_SetupClearSaveDataScreen);

        REG_BG3CNT = BGCNT_PRIORITY(3) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(7) | BGCNT_16COLOR | BGCNT_TXT256x256;
        REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_BG0_ON | DISPCNT_BG3_ON;
        gMain.state = 1;
        return FALSE;
    case 1:
        UpdatePaletteFade();
        if (gPaletteFade.active)
            return FALSE;
        SetMainCallback2(CB2_ClearSaveDataScreen);
        return TRUE;
    }
}

static void CB2_FadeAndDoReset(void)
{
    switch (gMain.state)
    {
    case 0:
    default:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, FADE_COLOR_WHITE);
        gMain.state = 1;
        break;
    case 1:
        UpdatePaletteFade();
        if (gPaletteFade.active)
            return;
        DoSoftReset();
        break;
    }
}
