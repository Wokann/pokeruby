#ifndef GUARD_DECORATION_H
#define GUARD_DECORATION_H

#include "task.h"
#include "menu.h"
#include "menu_helpers.h"

enum DecorPerm {
    /*
     * The nomenclature here describes collision and placement permissions, in that order.
     */
    DECORPERM_SOLID_FLOOR,
    DECORPERM_PASS_FLOOR,
    DECORPERM_BEHIND_FLOOR,
    DECORPERM_NA_WALL,
    DECORPERM_SOLID_MAT
};

enum DecorShape {
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
    DECORSHAPE_3x2
};

enum DecoCat {
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
    /*7*/ DECORCAT_CUSHION
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

struct DecoPCPointers
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

struct UnkStruct_803EC860
{
    const u8 *tiles;
    const u8 *y;
    const u8 *x;
    u8 size;
};

extern const struct UnkStruct_803EC860 gUnknown_083EC860[];

struct UnkStruct_02038900
{
    /*0x000; 0x02038900*/ const struct Decoration *decoration;
    /*0x004; 0x02038904*/ u16 tiles[0x40];
    /*0x084; 0x02038984*/ u8 image[0x800];
    /*0x884; 0x02039184*/ u16 palette[16];
};

struct UnkStruct_083EC900
{
    u8 shape;
    u8 size;
    u8 x;
    u8 y;
};

struct UnkStruct_020391B4
{
    u8 decorId;
    u8 width;
    u8 height;
    u16 flagId;
};

extern const struct UnkStruct_083EC900 gUnknown_083EC900[];
extern const struct SpritePalette gUnknown_083EC954;
extern const struct SpritePalette gUnknown_083ECA5C;
extern const struct SpritePalette gUnknown_083ECA64;
extern const struct SpriteTemplate gSpriteTemplate_83EC93C;
extern const struct SpriteTemplate gSpriteTemplate_83ECA88;

extern const struct Decoration gDecorations[];
extern struct UnkStruct_02038900 gUnknown_02038900;
extern u8 *gUnknown_020388D0;
extern u8 SecretBase_EventScript_SetDecoration[];
extern u8 SecretBase_EventScript_PutAwayDecoration[];
extern struct OamData gUnknown_020391AC;

extern u8 gUnknown_020388D4;
extern u8 gUnknown_020388D5;
extern u8 gUnknown_020388F2;
extern u8 gUnknown_020388F3;
extern u8 gUnknown_020388F4;
extern u8 gUnknown_020388F5;
extern u8 gUnknown_020388F6;
extern u8 gUnknown_020388D6[16];
extern u8 gUnknown_020388E6[12];
extern u16 gUnknown_020391A4;
extern u16 gUnknown_020391A6;
extern u8 gUnknown_020391A8;
extern u8 gUnknown_020391A9;
extern u8 gUnknown_020391AA;
extern u8 gUnknown_02039234;
extern const struct YesNoFuncTable gUnknown_083EC96C[];
extern struct UnkStruct_020391B4 gUnknown_020391B4[16];

extern const u16 gUnknown_083EC654[3];
extern const struct DecorationInventory gDecorationInventories[];
extern const u8 *const gUnknown_083EC5E4[];
extern const u8 *const gUnknown_083EC624[];
extern u8 gUnknown_020388F7[];
extern const struct MenuAction2 gUnknown_083EC604[];
extern const struct YesNoFuncTable gUnknown_083EC634[];
extern const u8 gUnknown_083EC65A[];
extern const u8 gUnknown_083EC97C[];
extern const u8 gUnknown_083EC984[];
extern const struct YesNoFuncTable gUnknown_083EC95C;
extern const struct YesNoFuncTable gUnknown_083EC964;
extern const struct YesNoFuncTable gUnknown_083EC9CC;
extern const struct YesNoFuncTable gUnknown_083EC9D4;
extern const struct YesNoFuncTable gUnknown_083ECAA0;

void ExitTraderMenu(u8); // src/trader
extern void ReshowPlayerPC(u8); // src/player_pc
void DoSecretBaseDecorationMenu(u8);

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
void AddDecorationIconObjectFromObjectEvent(struct UnkStruct_02038900 *, u8);
void SetUpPlacingDecorationPlayerAvatar(u8, struct UnkStruct_02038900 *);
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
void sub_81006D0(struct UnkStruct_02038900 *);
void sub_810070C(u16 *, u16);
void sub_8100874(struct UnkStruct_02038900 *);
void sub_81008BC(struct UnkStruct_02038900 *);
void sub_8100930(u8);
void sub_81009A8(struct Sprite *);
void sub_81009C0(struct Sprite *);
bool8 sub_8100D38(u8);
void sub_8100E70(u8);
void sub_8100EEC(u8);
void sub_8101024(u8);
void sub_81010F0(u8);
void sub_8101460(u8);
void sub_8101518(u8);
void sub_81015B0(u8);
void sub_81015E0(u8);
void sub_810156C(u8);
void sub_8101678(void);
void sub_8101698(struct Sprite *);
void sub_81016C8(void);
void sub_81016F4(void);
void sub_8101824(u8);
void SetRecycledDecoration(u8);

void DecorationMenuAction_Decorate(u8);
void DecorationItemsMenuAction_AttemptPlace(u8);
void DecorationItemsMenuAction_Cancel(u8);
void sub_8100A0C(u8);
void sub_8101700(u8);
void sub_81017A0(u8);
void DecorationItemsMenuAction_Trade(u8);

void ReturnToDecorationItemsMenu(u8);
void AttemptPlaceDecoration(u8);
void AttemptCancelPlaceDecoration(u8);
void PlaceDecoration(u8);
void CancelDecorating(u8);
void sub_8100F88(u8);
void sub_8100FB4(u8);
void sub_810153C(u8);
void sub_8101590(u8);
void sub_8101848(u8);
void DoPlayerRoomDecorationMenu(u8);

#endif // GUARD_DECORATION_H
