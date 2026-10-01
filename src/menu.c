#include "global.h"
#include "menu.h"
#include "main.h"
#include "event_object_lock.h"
#include "menu_cursor.h"
#include "script.h"
#include "constants/songs.h"
#include "sound.h"
#include "strings.h"
#include "text.h"
#include "text_window.h"
#include "string_util.h"

struct Menu
{
    u8 left;
    u8 top;
    s8 cursorPos;
    s8 minCursorPos;
    s8 maxCursorPos;
    u8 width;
    u8 height;
    u8 destroyCursorOnExit;
    u8 columnXCoords[8];
};

static void MultistepInitMenuWindowInternal(const struct WindowTemplate *, u16);
static void InitMenuWindowInternal(const struct WindowTemplate *, u16);
static bool8 IsGridCursorMovementClamped(void);
static u8 InitMenuActionGrid(u8 left, u8 top, u8 menuItemCount, u8 initialCursorPos, u8 columnCount, u8 cursorWidth, u32 destroyCursorOnExit);
static u8 DrawMenuActionGridWindow(u8, u8, u8, const struct MenuAction[], u8);
static void PrintMenuActionGrid(u8, u8, u8, const struct MenuAction[], u8);
static void CreateMenuOutlineCursor(u8, u8);
static void RedrawMenuCursor(u8, u8);

static struct Menu gMenu;

EWRAM_DATA struct Window gMenuWindow = {0};
EWRAM_DATA u8 gFiller_202E908[0x90] = {0};
EWRAM_DATA struct Window *gMenuWindowPtr = NULL;
EWRAM_DATA u8 gMenuMultistepInitState = 0;
EWRAM_DATA u16 gMenuTextTileOffset = 0;
EWRAM_DATA u16 gMenuTextWindowTileOffset = 0;
EWRAM_DATA u16 gMenuTextWindowContentTileOffset = 0;
EWRAM_DATA u16 gMenuMessageBoxContentTileOffset = 0;

const struct MenuAction gMenuYesNoItems[] =
{
    { OtherText_Yes, NULL },
    { OtherText_No, NULL },
};

void CloseMenu(void)
{
    PlaySE(SE_SELECT);
    Menu_EraseScreen();
    ScriptUnfreezeObjectEvents();
    UnlockPlayerFieldControls();
    Menu_DestroyCursor();
}

void AppendToList(u8 *list, u8 *pindex, u32 value)
{
    list[*pindex] = value;
    (*pindex)++;
}

void InitMenuWindow(const struct WindowTemplate *winTemplate)
{
    InitMenuWindowInternal(winTemplate, 1);
}

void MultistepInitMenuWindowBegin(const struct WindowTemplate *winTemplate)
{
    MultistepInitMenuWindowInternal(winTemplate, 1);
}

static void MultistepInitMenuWindowInternal(const struct WindowTemplate *winTemplate, u16 tileOffset)
{
    gMenuMultistepInitState = 0;
    gMenuTextTileOffset = tileOffset;
    gMenuWindowPtr = &gMenuWindow;
    Text_InitWindowWithTemplate(&gMenuWindow, winTemplate);
}

bool32 MultistepInitMenuWindowContinue(void)
{
    switch (gMenuMultistepInitState)
    {
    case 0:
        gMenuMultistepInitState++;
        return 0;
    case 1:
        gMenuTextWindowTileOffset = MultistepInitWindowTileData(gMenuWindowPtr, gMenuTextTileOffset);
        goto next;
    case 2:
        if (!MultistepLoadFont())
            goto fail;
        goto next;
    case 3:
        gMenuTextWindowContentTileOffset = TextWindow_SetBaseTileNum(gMenuTextWindowTileOffset);
    next:
        gMenuMultistepInitState++;
        return 0;
    case 4:
        TextWindow_LoadStdFrameGraphics(gMenuWindowPtr);
        gMenuMessageBoxContentTileOffset = TextWindow_SetDlgFrameBaseTileNum(gMenuTextWindowContentTileOffset);
        return 1;
    default:
    fail:
        return 0;
    }
}

static void InitMenuWindowInternal(const struct WindowTemplate *winTemplate, u16 tileOffset)
{
    gMenuWindowPtr = &gMenuWindow;
    Text_InitWindowWithTemplate(&gMenuWindow, winTemplate);
    gMenuTextTileOffset = tileOffset;
    gMenuTextWindowTileOffset = InitWindowTileData(gMenuWindowPtr, gMenuTextTileOffset);
    gMenuTextWindowContentTileOffset = TextWindow_SetBaseTileNum(gMenuTextWindowTileOffset);
    TextWindow_LoadStdFrameGraphics(gMenuWindowPtr);
    gMenuMessageBoxContentTileOffset = TextWindow_SetDlgFrameBaseTileNum(gMenuTextWindowContentTileOffset);
}

void InitMenuWindowWithCustomTileOffsets(struct WindowTemplate *winTemplate, u16 tileOffset)
{
    gMenuWindowPtr = &gMenuWindow;
    Text_InitWindowWithTemplate(&gMenuWindow, winTemplate);
    gMenuTextWindowTileOffset = tileOffset;
    gMenuTextWindowContentTileOffset = TextWindow_SetBaseTileNum(gMenuTextWindowTileOffset);
    TextWindow_LoadStdFrameGraphics(gMenuWindowPtr);
    gMenuTextTileOffset = TextWindow_SetDlgFrameBaseTileNum(gMenuTextWindowContentTileOffset);
    gMenuMessageBoxContentTileOffset = InitWindowTileData(gMenuWindowPtr, gMenuTextTileOffset);
}

void Menu_LoadStdFrameGraphicsOverrideStyle(u8 frameType)
{
    TextWindow_LoadStdFrameGraphicsOverrideStyle(gMenuWindowPtr, frameType);
}

void Menu_LoadStdFrameGraphics(void)
{
    TextWindow_LoadStdFrameGraphics(gMenuWindowPtr);
}

void BasicInitMenuWindow(const struct WindowTemplate *winTemplate)
{
    Text_InitWindowWithTemplate(gMenuWindowPtr, winTemplate);
    gMenuWindowPtr->tileDataStartOffset = gMenuTextTileOffset;
}

// Prints 
void Menu_PrintText(const u8 *str, u8 left, u8 top)
{
    Text_InitWindowAndPrintText(gMenuWindowPtr, str, gMenuTextTileOffset, left, top);
}

void Menu_EraseWindowRect(u8 left, u8 top, u8 right, u8 bottom)
{
    Text_EraseWindowRect(gMenuWindowPtr, left, top, right, bottom);
}

void Menu_BlankWindowRect(u8 left, u8 top, u8 right, u8 bottom)
{
    Text_BlankWindowRect(gMenuWindowPtr, left, top, right, bottom);
}

void Menu_EraseScreen(void)
{
    Menu_EraseWindowRect(0, 0, 29, 19);
}

void Menu_DrawStdWindowFrame(u8 left, u8 top, u8 right, u8 bottom)
{
    TextWindow_DrawStdFrame(gMenuWindowPtr, left, top, right, bottom);
}

void Menu_PrintTextWithStdFrame(const u8 *str)
{
    Menu_DrawStdWindowFrame(2, 14, 28, 19);
    Menu_PrintText(str, 3, 15);
}

void Menu_PrintChar(u8 c, u8 left, u8 top)
{
    Text_InitWindowAndPrintChar(gMenuWindowPtr, c, gMenuTextTileOffset, left, top);
}

u16 Menu_GetWindowTilemapEntry(u8 x, u8 y)
{
    return Text_GetWindowTilemapEntry(gMenuWindowPtr, x, y);
}

void Menu_FillWindowBorder(u16 tilemapEntry, u8 left, u8 top, u8 right, u8 bottom)
{
    Text_FillWindowBorder(gMenuWindowPtr, tilemapEntry, left, top, right, bottom);
}

void Menu_DisplayDialogueFrame(void)
{
    TextWindow_DisplayDialogueFrame(gMenuWindowPtr);
}

void MenuPrintMessage(const u8 *str, u8 left, u8 top)
{
    Contest_StartTextPrinter(gMenuWindowPtr, str, gMenuTextTileOffset, left, top);
}

void MenuPrintMessageDefaultCoords(const u8 *str)
{
    Contest_StartTextPrinter(gMenuWindowPtr, str, gMenuTextTileOffset, 2, 15);
}

void Menu_SetText(const u8 *str)
{
    Text_SetWindowText(gMenuWindowPtr, str);
}

u8 Menu_UpdateWindowText(void)
{
    return Text_UpdateWindow(gMenuWindowPtr);
}

u8 Menu_PrintWindowSimple(void)
{
    return Text_PrintWindowSimple(gMenuWindowPtr);
}

void Menu_ClearWindowText(void)
{
    Text_ClearWindow(gMenuWindowPtr);
}

u8 Menu_MoveCursor(s8 delta)
{
    s32 newPos = gMenu.cursorPos + delta;

    if (newPos < gMenu.minCursorPos)
        gMenu.cursorPos = gMenu.maxCursorPos;
    else if (newPos > gMenu.maxCursorPos)
        gMenu.cursorPos = gMenu.minCursorPos;
    else
        gMenu.cursorPos += delta;

    RedrawMenuCursor(gMenu.left, 2 * gMenu.cursorPos + gMenu.top);
    return gMenu.cursorPos;
}

u8 Menu_MoveCursorNoWrapAround(s8 delta)
{
    s32 newPos = gMenu.cursorPos + delta;

    if (newPos < gMenu.minCursorPos)
        gMenu.cursorPos = gMenu.minCursorPos;
    else if (newPos > gMenu.maxCursorPos)
        gMenu.cursorPos = gMenu.maxCursorPos;
    else
        gMenu.cursorPos += delta;

    RedrawMenuCursor(gMenu.left, 2 * gMenu.cursorPos + gMenu.top);
    return gMenu.cursorPos;
}

u8 Menu_GetCursorPos(void)
{
    return gMenu.cursorPos;
}

s8 Menu_ProcessInput(void)
{
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (gMenu.destroyCursorOnExit)
            Menu_DestroyCursor();
        return gMenu.cursorPos;
    }

    if (JOY_NEW(B_BUTTON))
    {
        if (gMenu.destroyCursorOnExit)
            Menu_DestroyCursor();
        return -1;
    }

    if (JOY_NEW(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        Menu_MoveCursor(-1);
        return -2;
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        Menu_MoveCursor(1);
        return -2;
    }

    return -2;
}

s8 Menu_ProcessInputNoWrap(void)
{
    u8 oldPos = gMenu.cursorPos;

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (gMenu.destroyCursorOnExit)
            Menu_DestroyCursor();
        return gMenu.cursorPos;
    }

    if (JOY_NEW(B_BUTTON))
    {
        if (gMenu.destroyCursorOnExit)
            Menu_DestroyCursor();
        return -1;
    }

    if (JOY_NEW(DPAD_UP))
    {
        if (oldPos != Menu_MoveCursorNoWrapAround(-1))
            PlaySE(SE_SELECT);
        return -2;
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        if (oldPos != Menu_MoveCursorNoWrapAround(1))
            PlaySE(SE_SELECT);
        return -2;
    }

    Menu_MoveCursorNoWrapAround(0);
    return -2;
}

u8 MoveMenuCursor3(s8 delta)
{
    u8 menuHeight = (gMenu.maxCursorPos + 1) >> 1;
    s32 newPos = gMenu.cursorPos + delta;

    if (newPos < gMenu.minCursorPos)
        gMenu.cursorPos = gMenu.maxCursorPos;
    else if (newPos > gMenu.maxCursorPos)
        gMenu.cursorPos = gMenu.minCursorPos;
    else
        gMenu.cursorPos += delta;

    RedrawMenuCursor(
        6 * (gMenu.cursorPos / menuHeight) + gMenu.left,
        2 * (gMenu.cursorPos % menuHeight) + gMenu.top);

    return gMenu.cursorPos;
}

static u8 MoveMenuCursorGridLayout(s8 delta)
{
    // Don't move cursor if it is clamped
    if (gMenu.cursorPos + delta <= gMenu.maxCursorPos)
    {
        if (IsGridCursorMovementClamped() == TRUE)
            return gMenu.cursorPos;
    }
    else
    {
        return gMenu.cursorPos;
    }

    gMenu.cursorPos += delta;

    if ((gMenu.maxCursorPos + 1) / gMenu.width == 0)
        RedrawMenuCursor(
            gMenu.left + gMenu.columnXCoords[gMenu.cursorPos % gMenu.width],
            2 * ((gMenu.cursorPos / gMenu.width) % gMenu.height) + gMenu.top);
    else
        RedrawMenuCursor(
            gMenu.left + gMenu.columnXCoords[gMenu.cursorPos % gMenu.width],
            2 * (gMenu.cursorPos / gMenu.width) + gMenu.top);

    return gMenu.cursorPos;
}

// Returns TRUE if the cursor is on an edge and cannot move in the pressed direction
static bool8 IsGridCursorMovementClamped(void)
{
    if (JOY_NEW(DPAD_UP))
    {
        // is cursor on first row?
        if (gMenu.cursorPos < gMenu.width)
            return TRUE;
    }
    if (JOY_NEW(DPAD_DOWN))
    {
        // is cursor on last row?
        if (gMenu.cursorPos >= gMenu.maxCursorPos + 1 - gMenu.width)
            return TRUE;
    }
    if (JOY_NEW(DPAD_LEFT))
    {
        // is cursor on first column?
        if ((gMenu.cursorPos - gMenu.cursorPos % gMenu.width) % gMenu.width == 1 // always false
         || gMenu.cursorPos == 0  // redundant
         || gMenu.cursorPos % gMenu.width == 0)
            return TRUE;
    }
    if (JOY_NEW(DPAD_RIGHT))
    {
        // is cursor on last column?
        if (gMenu.cursorPos % gMenu.width == gMenu.width - 1)
            return TRUE;
    }

    return FALSE;
}

static u8 InitMenuActionGrid(u8 left, u8 top, u8 menuItemCount, u8 initialCursorPos, u8 columnCount, u8 cursorWidth, u32 destroyCursorOnExit)
{
    u8 destroyCursor;

    gMenu.width = columnCount;
    gMenu.height = menuItemCount / columnCount;
    InitMenu(0, left, top, menuItemCount, initialCursorPos, cursorWidth);
    destroyCursor = 0;
    if (destroyCursorOnExit)
        destroyCursor = -1;
    gMenu.destroyCursorOnExit = destroyCursor;
    return initialCursorPos;
}

static u8 DrawMenuActionGridWindow(u8 left, u8 top, u8 menuItemCount, const struct MenuAction menuItems[], u8 columnCount)
{
    u8 i;
    u8 maxWidth;
    s32 height;

    for (i = 0; i < 7; i++)
        gMenu.columnXCoords[i] = 0;

    maxWidth = 0;
    for (i = 0; i < menuItemCount; i++)
    {
        u8 width = (GetStringWidthInMenuWindow(menuItems[i].text) + 7) / 8;

        if (width > maxWidth)
            maxWidth = width;
    }

    for (i = 1; i <= columnCount; i++)
        gMenu.columnXCoords[i] = maxWidth;

    for (i = 1; i <= columnCount; i++)
        gMenu.columnXCoords[i] += 1 + gMenu.columnXCoords[i - 1];

    gMenu.columnXCoords[columnCount]--;

    if (!((menuItemCount / 2) < columnCount || (menuItemCount % 2 != 0))
     || columnCount == 1
     || columnCount == menuItemCount)
    {
        height = 2 * (menuItemCount / columnCount) + 1;
    }
    else
    {
        height = 2 * ((menuItemCount / columnCount) + 1) + 1;
    }

    {
        // TODO: Make this code less hideous but still match the original asm.
        u8 right;
        u8 bottom;
        u32 totalWidth;
        register s32 val asm("r1");

        val = (s8)top + height;
        val = val << 24;
        asm("" ::: "r3");
        bottom = val >> 24;

        totalWidth = (gMenu.columnXCoords[columnCount] + 1);
        right = left + totalWidth;

        Menu_DrawStdWindowFrame(left, top, right, bottom);
    }

    return maxWidth;
}

static void PrintMenuActionGrid(u8 left, u8 top, u8 menuItemCount, const struct MenuAction menuItems[], u8 columnCount)
{
    u8 i;
    u8 maxWidth;

    for (i = 0; i < 7; i++)
        gMenu.columnXCoords[i] = 0;

    maxWidth = 0;
    for (i = 0; i < menuItemCount; i++)
    {
        u8 width = (GetStringWidthInMenuWindow(menuItems[i].text) + 7) / 8;

        if (width > maxWidth)
            maxWidth = width;
    }

    for (i = 1; i <= columnCount; i++)
        gMenu.columnXCoords[i] = maxWidth;

    for (i = 1; i <= columnCount; i++)
        gMenu.columnXCoords[i] += 1 + gMenu.columnXCoords[i - 1];

    gMenu.columnXCoords[columnCount]--;

    for (i = 0; i < columnCount; i++)
    {
        u8 row = 0;
        u8 j;
        for (j = 0; i + j < menuItemCount; j += columnCount, row++)
            Menu_PrintText(menuItems[i + j].text, left + gMenu.columnXCoords[i % columnCount], top + 2 * row);
    }
}

void DrawMenuActionGrid(u8 left, u8 top, u8 menuItemCount, u8 initialCursorPos, const struct MenuAction menuItems[], u8 columnCount, u32 destroyCursorOnExit)
{
    u8 maxWidth = DrawMenuActionGridWindow(left, top, menuItemCount, menuItems, columnCount);

    InitMenuActionGrid(left + 1, top + 1, menuItemCount, initialCursorPos, columnCount, maxWidth, destroyCursorOnExit);
    PrintMenuActionGrid(left + 1, top + 1, menuItemCount, menuItems, columnCount);
}

s8 Menu_ProcessInputGridLayout(void)
{
    if (JOY_NEW(A_BUTTON))
    {
        if (gMenu.destroyCursorOnExit)
            Menu_DestroyCursor();
        PlaySE(SE_SELECT);
        return Menu_GetCursorPos();
    }

    if (JOY_NEW(B_BUTTON))
    {
        if (gMenu.destroyCursorOnExit)
            Menu_DestroyCursor();
        return -1;
    }

    if (JOY_NEW(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        // Move up one whole row
        MoveMenuCursorGridLayout(-gMenu.width);
        return -2;
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        // Move down one whole column
        MoveMenuCursorGridLayout(gMenu.width);
        return -2;
    }
    else if (JOY_NEW(DPAD_LEFT))
    {
        PlaySE(SE_SELECT);
        MoveMenuCursorGridLayout(-1);
        return -2;
    }
    else if (JOY_NEW(DPAD_RIGHT))
    {
        PlaySE(SE_SELECT);
        MoveMenuCursorGridLayout(1);
        return -2;
    }

    return -2;
}

u8 Menu_GetColumnXCoord(u8 column)
{
    return gMenu.columnXCoords[column];
}

void Menu_PrintItems(u8 left, u8 top, u8 menuItemCount, const void * menuItems)
{
    u8 i;

    for (i = 0; i < menuItemCount; i++)
        Menu_PrintText(((const struct MenuAction *)menuItems)[i].text, left, top + 2 * i);
}

void Menu_PrintItemsReordered(u8 left, u8 top, u8 menuItemCount, const void * menuItems, const u8 *order)
{
    u8 i;

    for (i = 0; i < menuItemCount; i++)
        Menu_PrintText(((const struct MenuAction *)menuItems)[order[i]].text, left, top + 2 * i);
}

void InitYesNoMenu(u8 left, u8 top, u8 cursorWidth)
{
    Menu_PrintItems(left + 1, top + 1, 2, gMenuYesNoItems);
    InitMenu(0, left + 1, top + 1, 2, 0, cursorWidth);
}

void DisplayYesNoMenu(u8 left, u8 top, u32 destroyCursorOnExit)
{
    Menu_DrawStdWindowFrame(left, top, left + 6, top + 5);
    InitYesNoMenu(left, top, 5);
    gMenu.destroyCursorOnExit = destroyCursorOnExit ? -1 : 0;
}

s8 Menu_ProcessInputNoWrap_(void)
{
    return Menu_ProcessInputNoWrap();
}

u8 Menu_PrintTextPixelCoords(const u8 *text, u8 left, u16 top, u8 clipLeft)
{
    return Text_InitWindow8004D04(gMenuWindowPtr, text, gMenuTextTileOffset, left, top, clipLeft);
}

u8 Menu_PrintTextWithNewlineIndent(const u8 *text, u8 left, u16 top, u8 width, u32 clipLeft)
{
    return Text_PrintWithNewlineIndent(gMenuWindowPtr, 0, text, gMenuTextTileOffset, left, top, width, clipLeft);
}

u8 Menu_PrintTextWithNewlineIndentToBuffer(u8 *dest, u8 *src, u8 left, u16 top, u8 width, u32 clipLeft)
{
    return Text_PrintWithNewlineIndent(gMenuWindowPtr, dest, src, gMenuTextTileOffset, left, top, width, clipLeft);
}

int Menu_PrintTextWithNewlineIndentAndClear(const u8 *str, u8 left, u16 top, u8 width, u8 height, u32 clipLeft)
{
    u8 newlineCount = Text_PrintWithNewlineIndent(gMenuWindowPtr, NULL, str, gMenuTextTileOffset, left, top, width, clipLeft);

#if ENGLISH
    left /= 8;
    top /= 8;
    width = (width + 7) / 8;
    height = (height + 7) / 8;

    if (newlineCount < height)
        Menu_BlankWindowRect(left, top + 2 * newlineCount, left + width - 1, height + top - 1);
#elif GERMAN
    // This looks suspiciously macro-like
    u8 widthInTiles;
    if ((left & 7) == 0)
        widthInTiles = (width + 7) / 8 - 1;
    else
        widthInTiles = (width + (left & 7) - 1) / 8;
    width = widthInTiles;
    left /= 8;
    height = (height + 7) / 8;
    top /= 8;
    if (newlineCount < height)
        Menu_BlankWindowRect(left, top + 2 * newlineCount, left + width, height + top - 1);
#endif
}

void MenuPrint_RightAligned(const u8 *str, u8 right, u8 top)
{
    Text_InitWindow_RightAligned(gMenuWindowPtr, str, gMenuTextTileOffset, right, top);
}

void MenuPrint_AlignedToRightOfReferenceString(const u8 *src, u8 left, u8 top, const u8 *widthRefStr)
{
    u8 buffer[64];
    u8 width = GetStringWidth(gMenuWindowPtr, widthRefStr);
    AlignString(gMenuWindowPtr, buffer, src, width, 1);
    Text_InitWindowAndPrintText(gMenuWindowPtr, buffer, gMenuTextTileOffset, left, top);
}

void MenuPrint_Centered(const u8 *text, u8 left, u8 top, u16 width)
{
    Text_InitWindow_Centered(gMenuWindowPtr, text, gMenuTextTileOffset, left, top, width);
}

u8 *AlignInt1InMenuWindow(u8 *dest, s32 value, u8 alignAmount, u8 alignType)
{
    return AlignInt1(gMenuWindowPtr, dest, value, alignAmount, alignType);
}

u8 *AlignInt2InMenuWindow(u8 *dest, s32 value, u8 alignAmount, u8 alignType)
{
    return AlignInt2(gMenuWindowPtr, dest, value, alignAmount, alignType);
}

u8 *AlignStringInMenuWindow(u8 *dest, const u8 *src, u8 alignAmount, u8 alignType)
{
    return AlignString(gMenuWindowPtr, dest, src, alignAmount, alignType);
}

u8 GetStringWidthInMenuWindow(const u8 *str)
{
    return GetStringWidth(gMenuWindowPtr, str);
}

u8 Menu_GetTextWindowPaletteNum(void)
{
    return Text_GetWindowPaletteNum(gMenuWindowPtr);
}

void Menu_GetTextColors(u8 *foreground, u8 *background, u8 *shadow)
{
    Text_GetTextColors(gMenuWindowPtr, foreground, background, shadow);
}

u32 Menu_UpdateWindowTextOverrideLineLength(u8 lineLength)
{
    return Text_UpdateWindowOverrideLineLength(gMenuWindowPtr, lineLength);
}

struct Window *Menu_GetWindow(void)
{
    return gMenuWindowPtr;
}

static void CreateMenuOutlineCursor(u8 subpriority, u8 width)
{
    CreateOutlineCursor(subpriority, 0xFFFF, 12, 11679, 8 * width);
}

u8 InitMenu(u8 cursorSubpriority, u8 left, u8 top, u8 numChoices, u8 cursorPos, u8 cursorWidth)
{
    s32 pos;

    if (cursorWidth)
        CreateMenuOutlineCursor(cursorSubpriority, cursorWidth);

    gMenu.left = left - 1;
    gMenu.top = top;
    gMenu.minCursorPos = 0;
    gMenu.maxCursorPos = numChoices - 1;
    gMenu.destroyCursorOnExit = 0;

    pos = cursorPos;

    if (pos < 0 || pos > gMenu.maxCursorPos)
        pos = 0;

    gMenu.cursorPos = pos;
    Menu_MoveCursor(0);

    return pos;
}

static void RedrawMenuCursor(u8 x, u8 y)
{
    SetOutlineCursorPosition((x + 1) * 8, 8 * y);
}

void Menu_HideCursor()
{
    HideOutlineCursor();
}

void Menu_SetCursorWidth(u8 width)
{
    SetOutlineCursorWidth(width);
}

void Menu_SetCursorWidthInTiles(u8 widthInTiles)
{
    Menu_SetCursorWidth(8 * widthInTiles);
}

void Menu_DestroyCursor(void)
{
    DestroyMenuCursor();
}

#if GERMAN
void GermanStringReplaceNamePlaceholder(u8 *buffer, u8 *name) {
    u8 * ptr, *ptr2, *ptr3;

    ptr2 = buffer;
    ptr = &gStringVar1[1 + StringLengthN(gStringVar1, 256)];
    ptr3 = ptr;

    for (;;)
    {
        if (*ptr2 == EOS)
            break;

        if (*ptr2 == 0xFD)
        {

            *ptr3 = EOS;
            ptr2 += 2;

            StringAppend(ptr, name);
            StringAppend(ptr, ptr2);

            buffer[0] = EOS;
            StringAppend(buffer, ptr);
            break;
        }

        *ptr3 = *ptr2;
        ptr2 += 1;
        ptr3 += 1;
    }
}

u8 *GermanStringFormatName(u8 *name, const u8 *format) {
    u32 offset;
    u8 *ptr;

    offset = StringLengthN(gStringVar2, 0x100);
    ptr = &gStringVar2[1 + offset];

    StringCopy(ptr, format);

    GermanStringReplaceNamePlaceholder(ptr, name);

    return StringCopy(name, ptr);
}
#endif
