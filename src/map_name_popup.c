#include "global.h"
#include "map_name_popup.h"
#include "event_data.h"
#include "menu.h"
#include "region_map.h"
#include "task.h"

EWRAM_DATA static u8 sPopupTaskId = 0;

static void Task_MapNamePopUpWindow(u8 taskId);
static void ShowMapNamePopUpWindow(void);

static bool8 UNUSED StartMenu_ShowMapNamePopup(void)
{
    CloseMenu();
    ShowMapNamePopup();
    return TRUE;
}

enum
{
    STATE_SLIDE_IN,
    STATE_WAIT,
    STATE_SLIDE_OUT,
    STATE_UNUSED,
    STATE_END,
};

#define POPUP_OFFSCREEN_Y 32
#define POPUP_SLIDE_SPEED 2

#define tState         data[0]
#define tOnscreenTimer data[1]
#define tYOffset       data[2]
#define tIncomingPopUp data[3]

void ShowMapNamePopup(void)
{
    if (FlagGet(FLAG_HIDE_MAP_NAME_POPUP) != TRUE)
    {
        if (!FuncIsActiveTask(Task_MapNamePopUpWindow))
        {
            sPopupTaskId = CreateTask(Task_MapNamePopUpWindow, 90);
            REG_BG0VOFS = POPUP_OFFSCREEN_Y;
            ShowMapNamePopUpWindow();
            gTasks[sPopupTaskId].tState = STATE_SLIDE_IN;
            gTasks[sPopupTaskId].tYOffset = POPUP_OFFSCREEN_Y;
        }
        else
        {
            if (gTasks[sPopupTaskId].tState != STATE_SLIDE_OUT)
                gTasks[sPopupTaskId].tState = STATE_SLIDE_OUT;
            gTasks[sPopupTaskId].tIncomingPopUp = TRUE;
        }
    }
}

static void Task_MapNamePopUpWindow(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->tState)
    {
    case STATE_SLIDE_IN:
        task->tYOffset -= POPUP_SLIDE_SPEED;
        if (task->tYOffset <= 0 )
        {
            task->tState = STATE_WAIT;
            gTasks[sPopupTaskId].tOnscreenTimer = 0;
        }
        break;
    case STATE_WAIT:
        task->tOnscreenTimer++;
        if (task->tOnscreenTimer > 120 )
        {
            task->tOnscreenTimer = 0;
            task->tState = STATE_SLIDE_OUT;
        }
        break;
    case STATE_SLIDE_OUT:
        task->tYOffset += POPUP_SLIDE_SPEED;
        if (task->tYOffset > POPUP_OFFSCREEN_Y - 1)
        {
            if (task->tIncomingPopUp)
            {
                ShowMapNamePopUpWindow();
                task->tState = STATE_SLIDE_IN;
                task->tIncomingPopUp = FALSE;
            }
            else
            {
                task->tState = STATE_END;
                return;
            }
        }
        break;
    case STATE_END:
        HideMapNamePopUpWindow();
        return;
    }

    REG_BG0VOFS = task->tYOffset;
}

void HideMapNamePopUpWindow(void)
{
    if (FuncIsActiveTask(Task_MapNamePopUpWindow))
    {
        Menu_LoadStdFrameGraphics();
        Menu_EraseWindowRect(0, 0, 13, 3);
        REG_BG0VOFS = 0;
        DestroyTask(sPopupTaskId);
    }
}

static void ShowMapNamePopUpWindow(void)
{
    u8 mapName[20];

    Menu_LoadStdFrameGraphicsOverrideStyle(0);
    GetMapName(mapName, gMapHeader.regionMapSectionId, 0);
    Menu_DrawStdWindowFrame(0, 0, 13, 3);
    MenuPrint_Centered(mapName, 1, 1, 0x60);
}
