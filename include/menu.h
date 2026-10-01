#ifndef GUARD_MENU_H
#define GUARD_MENU_H

#define MENU_NOTHING_CHOSEN -2
#define MENU_B_PRESSED       -1

#include "task.h"
#include "text.h"

typedef bool8 (*MenuFunc)();

struct MenuAction
{
   const u8 *text;
   MenuFunc func;
};

struct MenuAction2
{
    const u8 *text;
    TaskFunc func;
};

extern const struct MenuAction gMenuYesNoItems[];

extern struct Window gMenuWindow;
extern struct Window *gMenuWindowPtr;
extern u16 gMenuTextTileOffset;
extern u16 gMenuTextWindowContentTileOffset;
extern u16 gMenuMessageBoxContentTileOffset;

void CloseMenu(void);
void AppendToList(u8 *list, u8 *pindex, u32 newEntry);
void InitMenuWindow(const struct WindowTemplate *);
void MultistepInitMenuWindowBegin(const struct WindowTemplate *);
bool32 MultistepInitMenuWindowContinue(void);
void InitMenuWindowWithCustomTileOffsets(struct WindowTemplate *, u16);
void Menu_LoadStdFrameGraphicsOverrideStyle(u8);
void Menu_LoadStdFrameGraphics(void);
void BasicInitMenuWindow(const struct WindowTemplate *);
void Menu_PrintText(const u8 *, u8, u8);
void Menu_EraseWindowRect(u8, u8, u8, u8);
void Menu_BlankWindowRect(u8, u8, u8, u8);
void Menu_EraseScreen(void);
void Menu_DrawStdWindowFrame(u8, u8, u8, u8);
void Menu_PrintTextWithStdFrame(const u8 *str);
void Menu_PrintChar(u8 c, u8 left, u8 top);
u16 Menu_GetWindowTilemapEntry(u8, u8);
void Menu_FillWindowBorder(u16, u8, u8, u8, u8);
void Menu_DisplayDialogueFrame(void);
void MenuPrintMessage(const u8 *, u8, u8);
void MenuPrintMessageDefaultCoords(const u8 *);
void Menu_SetText(const u8 *);
u8 Menu_UpdateWindowText(void);
u8 Menu_PrintWindowSimple(void);
void Menu_ClearWindowText(void);
u8 Menu_MoveCursor(s8);
u8 Menu_MoveCursorNoWrapAround(s8);
u8 Menu_GetCursorPos(void);
s8 Menu_ProcessInput(void);
s8 Menu_ProcessInputNoWrap(void);
void DrawMenuActionGrid(u8 left, u8 top, u8 menuItemCount, u8 initialCursorPos, const struct MenuAction menuItems[], u8 columnCount, u32 destroyCursorOnExit);
s8 Menu_ProcessInputGridLayout(void);
u8 Menu_GetColumnXCoord(u8);
void Menu_PrintItems(u8, u8, u8, const void *);
void Menu_PrintItemsReordered(u8 left, u8 top, u8 menuItemCount, const void * menuItems, const u8 *order);
void InitYesNoMenu(u8 left, u8 top, u8 cursorWidth);
void DisplayYesNoMenu(u8 left, u8 top, u32 destroyCursorOnExit);
s8 Menu_ProcessInputNoWrap_(void);
u8 Menu_PrintTextPixelCoords(const u8 *text, u8 left, u16 top, u8 clipLeft);
u8 Menu_PrintTextWithNewlineIndent(const u8 *text, u8 left, u16 top, u8 width, u32 clipLeft);
u8 Menu_PrintTextWithNewlineIndentToBuffer(u8 *dest, u8 *src, u8 left, u16 top, u8 width, u32 clipLeft);
int Menu_PrintTextWithNewlineIndentAndClear(const u8 *str, u8 left, u16 top, u8 width, u8 height, u32 clipLeft);
void MenuPrint_RightAligned(const u8 *, u8, u8);

// Prints src aligned to right edge of widthRefStr in window defined by left and top
void MenuPrint_AlignedToRightOfReferenceString(const u8 *src, u8 left, u8 top, const u8 *widthRefStr);
void MenuPrint_Centered(const u8 *, u8, u8, u16);
u8 *AlignInt1InMenuWindow(u8 *, s32, u8, u8);
u8 *AlignInt2InMenuWindow(u8 *, s32, u8, u8);
u8 *AlignStringInMenuWindow(u8 *, const u8 *, u8, u8);
u8 GetStringWidthInMenuWindow(const u8 *s);
u8 Menu_GetTextWindowPaletteNum(void);
void Menu_GetTextColors(u8 *foreground, u8 *background, u8 *shadow);
u32 Menu_UpdateWindowTextOverrideLineLength(u8);
struct Window *Menu_GetWindow(void);
u8 InitMenu(u8, u8, u8, u8, u8, u8);
void Menu_HideCursor(void);
void Menu_SetCursorWidth(u8 width);
void Menu_SetCursorWidthInTiles(u8 widthInTiles);
void Menu_DestroyCursor(void);
u8 MoveMenuCursor3(s8 delta);

#if GERMAN
u8 *GermanStringFormatName(u8 *name, const u8 *format);
#endif

#endif // GUARD_MENU_H
