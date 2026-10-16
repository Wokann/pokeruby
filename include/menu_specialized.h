#ifndef GUARD_MENU_SPECIALIZED_H
#define GUARD_MENU_SPECIALIZED_H

#include "global.h"

struct ConditionGraph
{
    /*0x000*/ u8 conditions[4][5];
    /*0x014*/ struct UCoords16 savedPositions[4][5];
    /*0x064*/ struct UCoords16 newPositions[10][5];
    /*0x12C*/ struct UCoords16 curPositions[5];
    /*0x140*/ u16 scanlineRight[66][2];
    /*0x248*/ u16 scanlineLeft[66][2];
    /*0x350*/ u16 bottom;
    /*0x352*/ u16 updateCounter;
    /*0x354*/ bool8 needsDraw;
    /*0x355*/ u8 scanlineResetState;
};

// Condition graph
void ConditionGraph_InitWindow(void);
void ConditionGraph_InitResetScanline(void);
bool8 ConditionGraph_ResetScanline(void);
void ConditionGraph_Draw(void);
bool8 ConditionGraph_TryUpdate(void);
void ConditionGraph_CalcPositions(u8 *conditions, struct UCoords16 *positions);
void ConditionGraph_SetNewPositions(struct UCoords16 *old, struct UCoords16 *new);

// Condition menu
bool8 ConditionMenu_UpdateMonEnter(void);
bool8 ConditionMenu_UpdateMonExit(void);
bool8 MoveConditionMonOnscreen(void);
bool8 MoveConditionMonOffscreen(void);
u32 GetBoxOrPartyMonData(u16 boxId, u16 monId, int request, u8 *dst);

// Condition sparkles
void CreateConditionSparkleSprites(void);
void DestroyConditionSparkleSprites(void);
void FreeConditionSparkles(void);

#endif // GUARD_MENU_SPECIALIZED_H
