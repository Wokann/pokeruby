#include "global.h"
#include "field_message_box.h"
#include "menu.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "text_window.h"

static EWRAM_DATA struct Window sFieldMessageBoxWindow = {0};

static u8 sFieldMessageBoxMode;

static void Task_DrawFieldMessage(u8 taskId);
static void CreateTask_DrawFieldMessage(void);
static void DestroyTask_DrawFieldMessage(void);
static void ExpandStringAndStartDrawFieldMessage(const u8 *message);
static void StartDrawFieldMessage(void);

void InitFieldMessageBox(void)
{
    sFieldMessageBoxMode = FIELD_MESSAGE_BOX_HIDDEN;
    TextWindow_SetDlgFrameBaseTileNum(gMenuTextWindowContentTileOffset);
    Text_InitWindowWithTemplate(&sFieldMessageBoxWindow, &gMenuTextWindowTemplate);
}

static void Task_DrawFieldMessage(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    switch (task->data[0])
    {
    case 0:
        TextWindow_LoadDialogueFrameTiles(&sFieldMessageBoxWindow);
        task->data[0]++;
        break;
    case 1:
        TextWindow_DrawDialogueFrame(&sFieldMessageBoxWindow);
        task->data[0]++;
        break;
    case 2:
        switch (sFieldMessageBoxMode)
        {
        case FIELD_MESSAGE_BOX_NORMAL:
            if (!Text_UpdateWindow(&sFieldMessageBoxWindow))
                return;
            break;
        case FIELD_MESSAGE_BOX_AUTO_SCROLL:
            if (!Text_UpdateWindowAutoscroll(&sFieldMessageBoxWindow))
                return;
            break;
        }
        sFieldMessageBoxMode = FIELD_MESSAGE_BOX_HIDDEN;
        DestroyTask(taskId);
    }
}

static void CreateTask_DrawFieldMessage(void)
{
    CreateTask(Task_DrawFieldMessage, 80);
}

static void DestroyTask_DrawFieldMessage(void)
{
    u8 taskId = FindTaskIdByFunc(Task_DrawFieldMessage);
    if (taskId != 0xFF)
        DestroyTask(taskId);
}

bool8 ShowFieldMessage(const u8 *message)
{
    if (sFieldMessageBoxMode != FIELD_MESSAGE_BOX_HIDDEN)
    {
        return FALSE;
    }
    else
    {
        ExpandStringAndStartDrawFieldMessage(message);
        sFieldMessageBoxMode = FIELD_MESSAGE_BOX_NORMAL;
        return TRUE;
    }
}

bool8 ShowFieldAutoScrollMessage(const u8 *message)
{
    if (sFieldMessageBoxMode != FIELD_MESSAGE_BOX_HIDDEN)
    {
        return FALSE;
    }
    else
    {
        sFieldMessageBoxMode = FIELD_MESSAGE_BOX_AUTO_SCROLL;
        ExpandStringAndStartDrawFieldMessage(message);
        return TRUE;
    }
}

bool8 ForceShowFieldAutoScrollMessage(const u8 *message)
{
    sFieldMessageBoxMode = FIELD_MESSAGE_BOX_AUTO_SCROLL;
    ExpandStringAndStartDrawFieldMessage(message);
    return TRUE;
}

bool8 ShowFieldMessageFromBuffer(const u8 *unused)
{
    if (sFieldMessageBoxMode != FIELD_MESSAGE_BOX_HIDDEN)
    {
        return FALSE;
    }
    else
    {
        sFieldMessageBoxMode = FIELD_MESSAGE_BOX_NORMAL;
        StartDrawFieldMessage();
        return TRUE;
    }
}

static void ExpandStringAndStartDrawFieldMessage(const u8 *message)
{
    StringExpandPlaceholders(gStringVar4, message);
    Contest_StartTextPrinter(&sFieldMessageBoxWindow, gStringVar4, gMenuTextTileOffset, 2, 15);
    CreateTask_DrawFieldMessage();
}

static void StartDrawFieldMessage(void)
{
    Contest_StartTextPrinter(&sFieldMessageBoxWindow, gStringVar4, gMenuTextTileOffset, 2, 15);
    CreateTask_DrawFieldMessage();
}

void HideFieldMessageBox(void)
{
    DestroyTask_DrawFieldMessage();
    TextWindow_EraseDialogueFrame(&sFieldMessageBoxWindow);
    sFieldMessageBoxMode = FIELD_MESSAGE_BOX_HIDDEN;
}

u8 GetFieldMessageBoxMode(void)
{
    return sFieldMessageBoxMode;
}

bool8 IsFieldMessageBoxHidden(void)
{
    if (sFieldMessageBoxMode == FIELD_MESSAGE_BOX_HIDDEN)
        return TRUE;
    else
        return FALSE;
}

void ReplaceFieldMessageWithFrame(void)
{
    DestroyTask_DrawFieldMessage();
    TextWindow_DrawDialogueFrame(&sFieldMessageBoxWindow);
    sFieldMessageBoxMode = FIELD_MESSAGE_BOX_HIDDEN;
}
