#include "global.h"
#include "script.h"
#include "script_menu.h"
#include "event_data.h"
#include "field_effect.h"
#include "menu.h"
#include "palette.h"
#include "constants/songs.h"
#include "sound.h"
#include "sprite.h"
#include "strings.h"
#include "task.h"
#include "data/script_menu.h"

extern u8 gText_WhichPCShouldBeAccessed[];

extern u16 gSpecialVar_Result;

static void Task_HandleMultichoiceInput(u8);
static void Task_HandleYesNoInput(u8);
static void Task_HandleMultichoiceGridInput(u8);
static u16 GetStringWidthInTilesForScriptMenu(const u8 *str);
static void DrawMultichoiceMenu(u8, u8, u8, const struct MenuAction *list, u8, u8);
static void DrawMultichoiceMenuNoWindow(u8, u8, u8, const struct MenuAction *list, u8);
static void StartScriptMenuTask(u8, u8, u8, u8, u8, u8);
static void CreatePCMultichoice(void);
static bool8 IsPicboxClosed(void);

bool8 ScriptMenu_Multichoice(u8 left, u8 top, u8 multichoiceId, bool8 ignoreBPress)
{
    if (FuncIsActiveTask(Task_HandleMultichoiceInput) == TRUE)
    {
        return FALSE;
    }
    else
    {
        gSpecialVar_Result = 0xFF;
        DrawMultichoiceMenu(left, top, sMultichoiceLists[multichoiceId].count, sMultichoiceLists[multichoiceId].list, ignoreBPress, 0);
        return TRUE;
    }
}

bool8 ScriptMenu_MultichoiceWithDefault(u8 left, u8 top, u8 multichoiceId, bool8 ignoreBPress, u8 defaultChoice)
{
    if (FuncIsActiveTask(Task_HandleMultichoiceInput) == TRUE)
    {
        return FALSE;
    }
    else
    {
        gSpecialVar_Result = 0xFF;
        DrawMultichoiceMenu(left, top, sMultichoiceLists[multichoiceId].count, sMultichoiceLists[multichoiceId].list, ignoreBPress, defaultChoice);
        return TRUE;
    }
}

static u16 GetStringWidthInTilesForScriptMenu(const u8 *str)
{
    // each tile on screen is 8x8, so it needs the number of tiles and not pixels, hence the division by 8.
    return (Text_GetStringWidthFromWindowTemplate((struct WindowTemplate *)&gMenuTextWindowTemplate, str) + 7) / 8;
}

static void DrawMultichoiceMenu(u8 left, u8 top, u8 count, const struct MenuAction *list, u8 ignoreBPress, u8 cursorPos)
{
    u16 width = GetStringWidthInTilesForScriptMenu(list[0].text);
    u16 newWidth;
    u8 i;
    u8 right;
    u8 bottom;

    for (i = 1; i < count; i++)
    {
        newWidth = GetStringWidthInTilesForScriptMenu(list[i].text);
        if (width < newWidth)
            width = newWidth;
    }

    right = width;
    right = (right + left) + 1;

    if (right > 29)
    {
        left = left + (29 - right);
        right = 29;
    }

    bottom = top + (2 * count + 1);

    Menu_DrawStdWindowFrame(left, top, right, bottom);
    Menu_PrintItems(left + 1, top + 1, count, list);
    InitMenu(0, left + 1, top + 1, count, cursorPos, right - left - 1);
    StartScriptMenuTask(left, top, right, bottom, ignoreBPress, count);
}

#define tLeft         data[0]
#define tTop          data[1]
#define tRight        data[2]
#define tBottom       data[3]
#define tIgnoreBPress data[4]
#define tDoWrap       data[5]
#define tYesNoInputDelay data[2]

static void StartScriptMenuTask(u8 left, u8 top, u8 right, u8 bottom, u8 ignoreBPress, u8 count)
{
    u8 taskId = CreateTask(Task_HandleMultichoiceInput, 80);

    gTasks[taskId].tLeft = left;
    gTasks[taskId].tTop = top;
    gTasks[taskId].tRight = right;
    gTasks[taskId].tBottom = bottom;
    gTasks[taskId].tIgnoreBPress = ignoreBPress;

    if (count > 3)
        gTasks[taskId].tDoWrap = TRUE;
    else
        gTasks[taskId].tDoWrap = FALSE;
}

static void Task_HandleMultichoiceInput(u8 taskId)
{
    s8 selection;

    if (!gPaletteFade.active)
    {
        if (!gTasks[taskId].tDoWrap)
            selection = Menu_ProcessInputNoWrap();
        else
            selection = Menu_ProcessInput();

        if (selection != -2)
        {
            if (selection == -1)
            {
                if (gTasks[taskId].tIgnoreBPress)
                    return;
                PlaySE(SE_SELECT);
                gSpecialVar_Result = MULTI_B_PRESSED;
            }
            else
            {
                gSpecialVar_Result = selection;
            }
            Menu_DestroyCursor();
            Menu_EraseWindowRect(gTasks[taskId].tLeft, gTasks[taskId].tTop, gTasks[taskId].tRight, gTasks[taskId].tBottom);
            DestroyTask(taskId);
            ScriptContext_Enable();
        }
    }
}

bool8 ScriptMenu_MultichoiceNoWindow(u8 left, u8 top, u8 multichoiceId, bool8 ignoreBPress)
{
    if (FuncIsActiveTask(Task_HandleMultichoiceInput) == TRUE)
    {
        return FALSE;
    }
    else
    {
        gSpecialVar_Result = 0xFF;
        DrawMultichoiceMenuNoWindow(left, top, sMultichoiceLists[multichoiceId].count, sMultichoiceLists[multichoiceId].list, ignoreBPress);
        return TRUE;
    }
}

static void DrawMultichoiceMenuNoWindow(u8 left, u8 top, u8 count, const struct MenuAction *list, u8 ignoreBPress)
{
    u16 width = GetStringWidthInTilesForScriptMenu(list[0].text);
    u16 newWidth;
    u8 i;
    u8 right;
    u8 bottom;

    for (i = 1; i < count; i++)
    {
        newWidth = GetStringWidthInTilesForScriptMenu(list[i].text);
        if (width < newWidth)
            width = newWidth;
    }

    right = width;
    right = (right + left) + 2;
    bottom = top + (2 * count + 1);

    Menu_PrintItems(left, top, count, list);
    InitMenu(0, left, top, count, 0, right - left - 1);
    StartScriptMenuTask(left, top, right, bottom, ignoreBPress, count);
}

bool8 ScriptMenu_YesNo(u8 left, u8 top)
{
    u8 taskId;

    if (FuncIsActiveTask(Task_HandleYesNoInput) == TRUE)
    {
        return FALSE;
    }
    else
    {
        gSpecialVar_Result = 0xFF;
        DisplayYesNoMenu(left, top, 1);
        taskId = CreateTask(Task_HandleYesNoInput, 0x50);
        gTasks[taskId].tLeft = left;
        gTasks[taskId].tTop = top;
        return TRUE;
    }
}

// unused
bool8 ScriptMenu_IsResultReady(void)
{
    if (gSpecialVar_Result == 0xFF)
        return FALSE;
    else
        return TRUE;
}

static void Task_HandleYesNoInput(u8 taskId)
{
    u8 left, top;

    if (gTasks[taskId].tYesNoInputDelay < 5)
    {
        gTasks[taskId].tYesNoInputDelay++;
        return;
    }

    switch (Menu_ProcessInputNoWrap())
    {
    case -2:
        return;
    case -1:
    case 1:
        PlaySE(SE_SELECT);
        gSpecialVar_Result = 0;
        break;
    case 0:
        gSpecialVar_Result = 1;
        break;
    }

    left = gTasks[taskId].tLeft;
    top = gTasks[taskId].tTop;

    Menu_EraseWindowRect(left, top, left + 6, top + 5);
    DestroyTask(taskId);
    ScriptContext_Enable();
}

bool8 ScriptMenu_MultichoiceGrid(u8 left, u8 top, u8 multichoiceId, bool8 ignoreBPress, u8 columnCount)
{
    u8 bottom = 0;

    if (FuncIsActiveTask(Task_HandleMultichoiceGridInput) == TRUE)
    {
        return FALSE;
    }
    else
    {
        u8 taskId;
        u8 width;

        gSpecialVar_Result = 0xFF;

        DrawMenuActionGrid(left, top, sMultichoiceLists[multichoiceId].count, 0, sMultichoiceLists[multichoiceId].list, columnCount, 0);

        taskId = CreateTask(Task_HandleMultichoiceGridInput, 80);

        if (!((sMultichoiceLists[multichoiceId].count >> 1) < columnCount || (sMultichoiceLists[multichoiceId].count & 1))
         || columnCount == 1 || sMultichoiceLists[multichoiceId].count == columnCount)
        {
            bottom = (2 * (sMultichoiceLists[multichoiceId].count / columnCount)) + 1 + top;
        }
        else
        {
            bottom = (2 * (sMultichoiceLists[multichoiceId].count / columnCount)) + 3 + top;
        }

        width = Menu_GetColumnXCoord(columnCount);
        gTasks[taskId].tLeft = left;
        gTasks[taskId].tTop = top;
        gTasks[taskId].tRight = width + left + 2;
        gTasks[taskId].tBottom = bottom;
        gTasks[taskId].tIgnoreBPress = ignoreBPress;
        return TRUE;
    }
}

static void Task_HandleMultichoiceGridInput(u8 taskId)
{
    s8 selection = Menu_ProcessInputGridLayout();

    if (selection != -2)
    {
        if (selection == -1)
        {
            if (gTasks[taskId].tIgnoreBPress)
                return;
            PlaySE(SE_SELECT);
            gSpecialVar_Result = MULTI_B_PRESSED;
        }
        else
        {
            gSpecialVar_Result = selection;
        }
        Menu_DestroyCursor();
        Menu_EraseWindowRect(gTasks[taskId].tLeft, gTasks[taskId].tTop, gTasks[taskId].tRight, gTasks[taskId].tBottom);
        DestroyTask(taskId);
        ScriptContext_Enable();
    }
}

#undef tLeft
#undef tTop
#undef tRight
#undef tBottom
#undef tIgnoreBPress
#undef tDoWrap
#undef tYesNoInputDelay

bool8 ScriptMenu_CreatePCMultichoice(void)
{
    if (FuncIsActiveTask(Task_HandleMultichoiceInput) == TRUE)
    {
        return FALSE;
    }
    else
    {
        gSpecialVar_Result = 0xFF;
        CreatePCMultichoice();
        return TRUE;
    }
}

static void CreatePCMultichoice(void)
{
    u8 width;
    u8 numChoices;

#if ENGLISH
    u16 playersPCWidth = GetStringWidthInTilesForScriptMenu(gPCText_PlayersPC);
    if (playersPCWidth > GetStringWidthInTilesForScriptMenu(gPCText_SomeonesPC))
        width = playersPCWidth;
    else
        width = 8;
#elif GERMAN
    s32 pcOptionWidths[4];
    s32 optionCount = 0;
    s32 optionIndex;

    if (FlagGet(FLAG_SYS_PC_LANETTE))
        pcOptionWidths[optionCount++] = GetStringWidthInTilesForScriptMenu(gPCText_LanettesPC);
    else
        pcOptionWidths[optionCount++] = GetStringWidthInTilesForScriptMenu(gPCText_SomeonesPC);
    pcOptionWidths[optionCount++] = GetStringWidthInTilesForScriptMenu(gPCText_PlayersPC);
    pcOptionWidths[optionCount++] = GetStringWidthInTilesForScriptMenu(gPCText_LogOff);
    if (FlagGet(FLAG_SYS_GAME_CLEAR))
        pcOptionWidths[optionCount++] = GetStringWidthInTilesForScriptMenu(gPCText_HallOfFame);

    width = 0;
    for (optionIndex = 0; optionIndex < optionCount; optionIndex++)
    {
        if (width < pcOptionWidths[optionIndex])
            width = pcOptionWidths[optionIndex];
    }
#endif

    if (FlagGet(FLAG_SYS_GAME_CLEAR)) // player has cleared game?
    {
        numChoices = 4;
        Menu_DrawStdWindowFrame(0, 0, width + 2, 9);
        Menu_PrintText(gPCText_HallOfFame, 1, 5);
        Menu_PrintText(gPCText_LogOff, 1, 7);
    }
    else
    {
        numChoices = 3;
        Menu_DrawStdWindowFrame(0, 0, width + 2, 7);
        Menu_PrintText(gPCText_LogOff, 1, 5);
    }

    if (FlagGet(FLAG_SYS_PC_LANETTE)) // player met lanette?
        Menu_PrintText(gPCText_LanettesPC, 1, 1);
    else
        Menu_PrintText(gPCText_SomeonesPC, 1, 1);

    Menu_PrintText(gPCText_PlayersPC, 1, 3);
    InitMenu(0, 1, 1, numChoices, 0, width + 1);
    StartScriptMenuTask(0, 0, width + 2, 2 * numChoices + 1, 0, numChoices);
}

void ScriptMenu_DisplayPCStartupPrompt(void)
{
    Menu_DisplayDialogueFrame();
    Menu_PrintText(gText_WhichPCShouldBeAccessed, 2, 15);
}

#define tState       data[0]
#define tMonSpecies  data[1]
#define tMonSpriteId data[2]
#define tWindowX     data[3]
#define tWindowY     data[4]

static void Task_PokemonPicWindow(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->tState)
    {
    case 0:
        task->tState++;
        break;
    case 1:
        break;
    case 2:
        FreeResourcesAndDestroySprite(&gSprites[task->tMonSpriteId]);
        task->tState++;
        break;
    case 3:
        Menu_EraseWindowRect(task->tWindowX, task->tWindowY, task->tWindowX + 9, task->tWindowY + 10);
        DestroyTask(taskId);
        break;
    }
}

bool8 ScriptMenu_ShowPokemonPic(u16 species, u8 x, u8 y)
{
    u8 taskId;
    u8 spriteId;

    if (FindTaskIdByFunc(Task_PokemonPicWindow) != 0xFF)
    {
        return FALSE;
    }
    else
    {
        Menu_DrawStdWindowFrame(x, y, x + 9, y + 10);
        taskId = CreateTask(Task_PokemonPicWindow, 0x50);
        gTasks[taskId].tState = 0;
        gTasks[taskId].tMonSpecies = species;
        spriteId = CreateMonSprite_PicBox(species, x * 8 + 40, y * 8 + 40, 0);
        gTasks[taskId].tMonSpriteId = spriteId;
        gTasks[taskId].tWindowX = x;
        gTasks[taskId].tWindowY = y;
        gSprites[spriteId].callback = SpriteCallbackDummy;
        gSprites[spriteId].oam.priority = 0;
        return TRUE;
    }
}  

bool8 (*ScriptMenu_HidePokemonPic(void))(void)
{
    u8 taskId = FindTaskIdByFunc(Task_PokemonPicWindow);

    if (taskId == 0xFF)
        return NULL;
    gTasks[taskId].tState++;
    return IsPicboxClosed;
}

static bool8 IsPicboxClosed(void)
{
    if (FindTaskIdByFunc(Task_PokemonPicWindow) == 0xFF)
        return TRUE;
    else
        return FALSE;
}

#undef tState      
#undef tMonSpecies 
#undef tMonSpriteId
#undef tWindowX    
#undef tWindowY  
