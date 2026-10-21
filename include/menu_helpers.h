#ifndef GUARD_MENU_HELPERS_H
#define GUARD_MENU_HELPERS_H

#include "task.h"

#define MENU_L_PRESSED 1
#define MENU_R_PRESSED 2

struct YesNoFuncTable
{
    TaskFunc yesFunc;
    TaskFunc noFunc;
};

enum
{
    VISIBLE,
    INVISIBLE
};

void ClearBGTilemapBuffers(void);
void DoYesNoFuncWithChoice(u8 taskId, const struct YesNoFuncTable *funcTable);
void sub_80F914C(u8, const struct YesNoFuncTable *);
u8 GetLRKeysPressed(void);
u8 GetLRKeysPressedAndHeld(void);
bool8 IsHoldingItemAllowed(u16 itemId);
bool8 IsWritingMailAllowed(u16 itemId);
bool8 MenuHelpers_IsLinkActive(void);
void ResetVramOamAndBgCntRegs(void);
void SetVBlankHBlankCallbacksToNull(void);
void ClearVerticalScrollIndicatorPalettes(void);
void sub_80F9480(u8 *, u8);
void sub_80F94A4(u8, u8 *, s16, s16);
void sub_80F94F8(u8 *);
void sub_80F9520(u8 *, u8);
void CreateVerticalScrollIndicators(u8, u16, u16);
void SetVerticalScrollIndicators(u8, bool8);
void DestroyVerticalScrollIndicator(u8);
void LoadScrollIndicatorPalette(void);
void BuyMenuFreeMemory(void);
void StopVerticalScrollIndicators(u8);
void StartVerticalScrollIndicators(u8);
void sub_80F98DC(u8);
void PauseVerticalScrollIndicator(u8);
void SetVerticalScrollIndicatorPriority(u8, u8);
#if DEBUG
void PrintTriangleCursorWithPalette(u8, u8, u8);
#endif // DEBUG

#endif // GUARD_MENU_HELPERS_H
