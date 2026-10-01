#ifndef GUARD_MENU_HELPERS_H
#define GUARD_MENU_HELPERS_H

#include "task.h"

struct YesNoFuncTable
{
    void (*yesFunc)(u8);
    void (*noFunc)(u8);
};

// Scroll indicator arrow IDs
enum
{
    TOP_ARROW,
    BOTTOM_ARROW,
    LEFT_ARROW, // Bag arrows
    RIGHT_ARROW
};

enum
{
    VISIBLE,
    INVISIBLE
};

void ClearBGTilemapBuffers(void);
void DisplayItemMessageOnField(u8, const u8 *, TaskFunc, u16);
void DoYesNoFuncWithChoice(u8, const struct YesNoFuncTable *);
u8 GetLRKeysPressed(void);
u8 GetLRKeysPressedAndHeld(void);
bool8 IsHoldingItemAllowed(u16);
bool8 IsWritingMailAllowed(u16);
bool8 MenuHelpers_IsLinkActive(void);
void ResetVramOamAndBgCntRegs(void);
void SetVBlankHBlankCallbacksToNull(void);
void ResetScrollIndicatorState(void);
void InitDecorationMarkerSpriteIds(u8 *, u8);
void CreateDecorationMarkerSprite(u8, u8 *, s16, s16);
void DestroyDecorationMarkerSprite(u8 *);
void DestroyDecorationMarkerSprites(u8 *, u8);
void CreateScrollIndicatorArrow(u8, u16, u16);
void SetScrollIndicatorArrowInvisibility(u8, bool8);
void DestroyScrollIndicatorArrow(u8);
void LoadScrollIndicatorPalette(void);
void FreeScrollIndicatorPalette(void);
void StopScrollIndicatorArrow(u8);
void StartScrollIndicatorArrow(u8);
void PauseScrollIndicatorArrow(u8);
void SetScrollIndicatorArrowPriority(u8, u8);
#if DEBUG
void PrintTriangleCursorWithPalette(u8, u8, u8);
#endif // DEBUG

#endif // GUARD_MENU_HELPERS_H
