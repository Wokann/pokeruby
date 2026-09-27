#ifndef GUARD_MENU_CURSOR_H
#define GUARD_MENU_CURSOR_H

#include "sprite.h"

void InitOutlineCursorState(void);
u8 CreateOutlineCursor(u8 subpriority, u16 paletteTag, u8 tileIndex, u16 color, u8 width);
u8 CreateOutlineCursorWithPaletteNum(u8 subpriority, u8 paletteNum, u8 tileIndex, u8 width);
u8 CreateOutlineCursorWithPaletteColor(u8 subpriority, u16 color, u8 width);
void DestroyMenuCursor(void);
void SetOutlineCursorPosition(u8 x, u8 y);
void HideOutlineCursor(void);
void SetOutlineCursorWidth(u8 width);
void SetOutlineCursorCallback(void (*callback)(struct Sprite *));
void UpdateOutlineCursorPaletteByColor(u16 color);
void DestroyOutlineCursorWindowSprite(void);
void SetOutlineCursorWindowSubsprites(int index);
u8 CreateBlendedOutlineCursor(u8 subpriority, u16 paletteTag, u8 tileIndex, u16 color, u8 width);
void DestroyBlendedOutlineCursor(void);
void SetBlendedOutlineCursorPosition(u8 x, u8 y);
void HideBlendedOutlineCursor(void);
void SetBlendedOutlineCursorWidth(u8 widthIndex);

#endif // GUARD_MENU_CURSOR_H
