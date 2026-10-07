#ifndef GUARD_DECORATION_H
#define GUARD_DECORATION_H

#include "task.h"
#include "menu.h"
#include "menu_helpers.h"

struct PlaceDecorationGraphicsDataBuffer;

enum DecorPerm {
    /*
     * The nomenclature here describes collision and placement permissions, in that order.
     */
    DECORPERM_SOLID_FLOOR,
    DECORPERM_PASS_FLOOR,
    DECORPERM_BEHIND_FLOOR,
    DECORPERM_NA_WALL,
    DECORPERM_SPRITE
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


struct UnkStruct_083EC900
{
    u8 shape;
    u8 size;
    u8 cameraX;
    u8 cameraY;
};


extern const struct Decoration gDecorations[];
extern u8 *gCurDecorationItems;
extern u8 gUnknown_081A2F7B[];
extern u8 gUnknown_081A2F8A[];

extern u8 gUnknown_020388F3;
extern u8 gCurDecorationIndex;

extern const u16 gUnknown_083EC654[3];
extern const struct DecorationInventory gDecorationInventories[];
extern u8 gUnknown_020388F7[];
extern const struct YesNoFuncTable gUnknown_083EC634[];
extern const u8 gUnknown_083EC65A[];

extern void sub_8109DAC(u8); // src/trader
extern void ReshowPlayerPC(u8); // src/player_pc
void DoSecretBaseDecorationMenu(u8);

void DoSecretBaseDecorationMenu(u8 taskId);
void sub_80FE2B4(void);
void Task_DecorationPCProcessMenuInput(u8);
void sub_80FE394(void);
void gpu_pal_decompress_alloc_tag_and_upload(u8);
void sub_80FE5AC(u8);
void sub_80FE604(u8);
void sub_80FE728(u8);
void sub_80FE758(u8);
void sub_80FE7A8(u8);
void CopyDecorationCategoryName(u8 *dest, u8 category);
void SetDecoration(void);
void PutAwayDecorationIteration(void);
void GetObjectEventLocalIdByFlag(void);
void sub_80FE948(u8);
void sub_80FEABC(u8, u8);
void sub_80FEC94(u8);
void sub_80FECB8(u8);
#ifdef GERMAN
int sub_80FECE0(u8);
#else
void sub_80FECE0(u8);
#endif
void sub_80FEF50(u8);
bool8 IsSelectedDecorInThePC(void);
void ShowDecorationOnMap(u16 mapX, u16 mapY, u16 decIdx);
void sub_80FF6AC(u8);
void AddDecorationIconObjectFromObjectEvent(struct PlaceDecorationGraphicsDataBuffer *, u8);
void SetUpPlacingDecorationPlayerAvatar(u8, struct PlaceDecorationGraphicsDataBuffer *);
void sub_8100038(u8);
void sub_81000A0(u8);
void sub_8100248(u8);
void c1_overworld_prev_quest(u8);
void sub_8100364(void);
void sub_810065C(u8);
void sub_81006D0(struct PlaceDecorationGraphicsDataBuffer *);
void sub_810070C(u16 *, u16);
void sub_8100930(u8);
bool8 sub_8100D38(u8);
void sub_8100E70(u8);
void sub_8100EEC(u8);
void sub_8101024(u8);
void sub_8101518(u8);
void sub_810156C(u8);
void sub_8101678(void);
void sub_8101824(u8);
void sub_8109A30(u8);

void sub_80FF160(u8);
void sub_80FF5BC(u8);
void sub_80FF058(u8);
void sub_8100A0C(u8);
void sub_8101700(u8);
void sub_81017A0(u8);
void sub_8109D04(u8);

void sub_80FED3C(u8);
void sub_80FFB08(u8);
void sub_81000C4(u8);
void sub_810026C(u8);
void sub_8100FB4(u8);
void sub_8101590(u8);
void sub_8101848(u8);
void DoPlayerPCDecoration(u8);

#endif // GUARD_DECORATION_H
