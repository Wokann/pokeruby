#ifndef GUARD_MENU_CURSOR_H
#define GUARD_MENU_CURSOR_H

#include "sprite.h"

void sub_814A590(void);
u8 MenuCursor_Create814A5C0(u8 subpriority, u16 paletteTag, u8 a3, u16 a4, u8 rowWidth);
u8 sub_814A758(u8 subpriority, u8 palNum, u8 a3, u8 rowWidth);
u8 unref_sub_814A7AC(u8 subpriority, u16 a2, u8 rowWidth);
void DestroyMenuCursor(void);
void MenuCursor_SetPos814A880(u8 x, u8 y);
void sub_814A904(void);
void sub_814A958(u8 rowWidth);
void sub_814AABC(void (*callback)(struct Sprite *));
void sub_814AAF8(u16 a1);
void sub_814AB84(void);
void unref_sub_814ABE4(int a1);
u8 CreateBlendedOutlineCursor(u8 subpriority, u16 paletteTag, u8 a3, u16 a4, u8 a5);
void MenuCursor_Destroy814AD44(void);
void MenuCursor_SetPos814AD7C(u8 x, u8 y);
void sub_814ADC8(void);
void sub_814ADF4(u8 a1);

#endif // GUARD_MENU_CURSOR_H
