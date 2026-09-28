#ifndef GUARD_DECORATION_H
#define GUARD_DECORATION_H

#include "task.h"
#include "menu.h"
#include "menu_helpers.h"

enum DecorationPermission
{
    /*
     * The nomenclature here describes collision and placement permissions, in that order.
     */
    DECORPERM_SOLID_FLOOR,
    DECORPERM_PASS_FLOOR,
    DECORPERM_BEHIND_FLOOR,
    DECORPERM_NA_WALL,
    DECORPERM_SPRITE,
};

enum DecorationShape
{
    /*
     * Width-x-height
     */
    DECORSHAPE_1x1,
    DECORSHAPE_2x1,
    DECORSHAPE_3x1, // unused
    DECORSHAPE_4x2,
    DECORSHAPE_2x2,
    DECORSHAPE_1x2,
    DECORSHAPE_1x3, // unused
    DECORSHAPE_2x4,
    DECORSHAPE_3x3,
    DECORSHAPE_3x2,
};

enum DecorationCategory
{
    /*
     * In which category you can find the decoration in the PC.
     */
    /*0*/ DECORCAT_DESK,
    /*1*/ DECORCAT_CHAIR,
    /*2*/ DECORCAT_PLANT,
    /*3*/ DECORCAT_ORNAMENT,
    /*4*/ DECORCAT_MAT,
    /*5*/ DECORCAT_POSTER,
    /*6*/ DECORCAT_DOLL,
    /*7*/ DECORCAT_CUSHION,
    DECORCAT_COUNT,
};

struct Decoration
{
    /*0x00*/ u8 id;
    /*0x01*/ u8 name[16];
    /*0x11*/ u8 permission;
    /*0x12*/ u8 shape;
    /*0x13*/ u8 category;
    /*0x14*/ u16 price;
    /*0x18*/ const u8 *description;
    /*0x1c*/ const u16 *tiles;
};

struct DecorationPCContext
{
    /* 0x00 */ u8 *items;
    /* 0x04 */ u8 *pos;
    /* 0x08 */ u8 size;
    /* 0x09 */ u8 isPlayerRoom;
};

struct DecorationInventory
{
    u8 *items;
    u8 size;
};

struct DecorTilemap
{
    const u8 *tiles;
    const u8 *y;
    const u8 *x;
    u8 size;
};

struct PlaceDecorationGraphicsDataBuffer
{
    /*0x000; 0x02038900*/ const struct Decoration *decoration;
    /*0x004; 0x02038904*/ u16 tiles[0x40];
    /*0x084; 0x02038984*/ u8 image[0x800];
    /*0x884; 0x02039184*/ u16 palette[16];
};

struct DecorationMovementInfo
{
    u8 shape;
    u8 size;
    u8 cameraX;
    u8 cameraY;
};

struct DecorRearrangementDataBuffer
{
    u8 idx;
    u8 width;
    u8 height;
    u16 flagId;
};

extern const struct Decoration gDecorations[];
extern u8 *gCurDecorationItems;
extern u8 gCurDecorationIndex;
extern u8 gDecorationMarkerSpriteIds[];
extern u8 SecretBase_EventScript_SetDecoration[];
extern u8 SecretBase_EventScript_PutAwayDecoration[];
extern const struct DecorationInventory gDecorationInventories[];

void ExitTraderMenu(u8); // src/trader
extern void ReshowPlayerPC(u8); // src/player_pc
void DoSecretBaseDecorationMenu(u8 taskId);
void LoadDecorationMenuPalette(void);
void HandleDecorationActionsMenuInput(u8);
void PrintCurMainMenuDescription(void);
void DecorationMenuAction_Cancel(u8);
void InitDecorationCategoriesWindow(u8);
void HandleDecorationCategoriesMenuInput(u8);
void ExitDecorationCategoriesMenu(u8);
void ReturnToActionsMenuFromCategories(u8);
void ShowDecorationCategoriesWindow(u8);
void CopyDecorationCategoryName(u8 *, u8);
void ShowDecorationItemsWindow(u8);
void HandleDecorationItemsMenuInput(u8);
void UpdateDecorationItemsDisplay(u8, u8);
void ShowDecorationItemsListWindow(u8);
void ShowDecorationCategorySummaryWindow(u8);
#ifdef GERMAN
int PrintDecorationItemDescription(u8);
#else
void PrintDecorationItemDescription(u8);
#endif
void InitDecorationItemsMenuScrollAndCursor(u8);
bool8 IsSelectedDecorationUnused(void);
void ShowDecorationOnMap(u16, u16, u16);
void Task_PlaceDecoration(u8);
void SetUpDecorationShape(u8);
void AddDecorationIconObjectFromObjectEvent(struct PlaceDecorationGraphicsDataBuffer *, u8);
void SetUpPlacingDecorationPlayerAvatar(u8, struct PlaceDecorationGraphicsDataBuffer *);
void AttemptPlaceDecoration_(u8);
void PlaceDecorationPrompt(u8);
void PlaceDecoration_(u8);
void CancelDecoratingPrompt(u8);
void CancelDecorating_(u8);
void c1_overworld_prev_quest(u8);
void FieldCB_InitDecorationItemsWindow(void);
void ResetCursorMovement(void);
void ContinueDecorating(u8);
void CantPlaceDecorationPrompt(u8);
void ClearPlaceDecorationTileBuffers(struct PlaceDecorationGraphicsDataBuffer *);
void CopyPalette(u16 *, u16);
void SetDecorSelectionBoxTiles(struct PlaceDecorationGraphicsDataBuffer *);
void SetDecorSelectionMetatiles(struct PlaceDecorationGraphicsDataBuffer *);
void SetDecorSelectionBoxOamAttributes(u8);
void InitializeDecorationCursorSprite(struct Sprite *);
void UpdateDecorationCursorBlink(struct Sprite *);
bool8 HasDecorationsInUse(u8);
void Task_ContinuePuttingAwayDecorations(u8);
void ContinuePuttingAwayDecorations(u8);
void AttemptPutAwayDecoration_(u8);
void ContinuePuttingAwayDecorationsPrompt(u8);
void AttemptMarkDecorUnderCursorForRemoval(u8);
void ReturnDecorationPrompt(u8);
void StopPuttingAwayDecorations_(u8);
void Task_StopPuttingAwayDecorations(u8);
void StopPuttingAwayDecorationsPrompt(u8);
void FieldCB_StopPuttingAwayDecorations(void);
void InitializeCameraSprite1(struct Sprite *);
void LoadPlayerSpritePalette(void);
void FreePlayerSpritePalette(void);
void TossDecorationPrompt(u8);
void SetRecycledDecoration(u8);

void DecorationMenuAction_Decorate(u8);
void DecorationItemsMenuAction_AttemptPlace(u8);
void DecorationItemsMenuAction_Cancel(u8);
void DecorationMenuAction_PutAway(u8);
void DecorationMenuAction_Toss(u8);
void DecorationItemsMenuAction_AttemptToss(u8);
void DecorationItemsMenuAction_Trade(u8);

void ReturnToDecorationItemsMenu(u8);
void AttemptPlaceDecoration(u8);
void AttemptCancelPlaceDecoration(u8);
void PlaceDecoration(u8);
void CancelDecorating(u8);
void AttemptPutAwayDecoration(u8);
void AttemptCancelPutAwayDecoration(u8);
void PutAwayDecoration(u8);
void StopPuttingAwayDecorations(u8);
void TossDecoration(u8);
void DoPlayerRoomDecorationMenu(u8);

#endif // GUARD_DECORATION_H
