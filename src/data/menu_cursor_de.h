#ifndef GUARD_DATA_MENU_CURSOR_H
#define GUARD_DATA_MENU_CURSOR_H

const u8 OutlineCursorTiles_00[] = INCBIN_U8("graphics/interface/outline_cursor_00.4bpp");
const u8 OutlineCursorTiles_01[] = INCBIN_U8("graphics/interface/outline_cursor_01.4bpp");
const u8 OutlineCursorTiles_02[] = INCBIN_U8("graphics/interface/outline_cursor_02.4bpp");
const u8 OutlineCursorTiles_03[] = INCBIN_U8("graphics/interface/outline_cursor_03.4bpp");
const u8 OutlineCursorTiles_04[] = INCBIN_U8("graphics/interface/outline_cursor_04.4bpp");
const u8 OutlineCursorTiles_05[] = INCBIN_U8("graphics/interface/outline_cursor_05.4bpp");
const u8 OutlineCursorTiles_06[] = INCBIN_U8("graphics/interface/outline_cursor_06.4bpp");
const u8 OutlineCursorTiles_07[] = INCBIN_U8("graphics/interface/outline_cursor_07.4bpp");
const u8 OutlineCursorTiles_08[] = INCBIN_U8("graphics/interface/outline_cursor_08.4bpp");
const u8 OutlineCursorTiles_09[] = INCBIN_U8("graphics/interface/outline_cursor_09.4bpp");
const u8 OutlineCursorTiles_10[] = INCBIN_U8("graphics/interface/outline_cursor_10.4bpp");
const u8 OutlineCursorTiles_11[] = INCBIN_U8("graphics/interface/outline_cursor_11.4bpp");
const u8 OutlineCursorTiles_12[] = INCBIN_U8("graphics/interface/outline_cursor_12.4bpp");
const u8 OutlineCursorTiles_13[] = INCBIN_U8("graphics/interface/outline_cursor_13.4bpp");
const u8 OutlineCursorTiles_14[] = INCBIN_U8("graphics/interface/outline_cursor_14.4bpp");
const u8 OutlineCursorTiles_15[] = INCBIN_U8("graphics/interface/outline_cursor_15.4bpp");

const struct OamData gOamData_OutlineCursor = {
    .objMode = ST_OAM_OBJ_NORMAL,
    .shape = ST_OAM_V_RECTANGLE
};
const struct OamData gOamData_OutlineCursorWindow = {
    .objMode = ST_OAM_OBJ_WINDOW,
    .shape = ST_OAM_V_RECTANGLE
};
const struct OamData gOamData_BlendedOutlineCursor = {
    .objMode = ST_OAM_OBJ_NORMAL,
    .shape = ST_OAM_V_RECTANGLE
};

const union AnimCmd gSpriteAnim_OutlineCursor[] = {
    ANIMCMD_FRAME(0, 63),
    ANIMCMD_END
};

const union AnimCmd *const gSpriteAnimTable_OutlineCursor[] = {
    gSpriteAnim_OutlineCursor
};

const struct SpriteSheet gOutlineCursorSpriteSheets[] = {
    { OutlineCursorTiles_00, 0x380, 0xFFF0 },
    { OutlineCursorTiles_01, 0x380, 0xFFF0 },
    { OutlineCursorTiles_02, 0x380, 0xFFF0 },
    { OutlineCursorTiles_03, 0x380, 0xFFF0 },
    { OutlineCursorTiles_04, 0x380, 0xFFF0 },
    { OutlineCursorTiles_05, 0x380, 0xFFF0 },
    { OutlineCursorTiles_06, 0x380, 0xFFF0 },
    { OutlineCursorTiles_07, 0x380, 0xFFF0 },
    { OutlineCursorTiles_08, 0x380, 0xFFF0 },
    { OutlineCursorTiles_09, 0x380, 0xFFF0 },
    { OutlineCursorTiles_10, 0x380, 0xFFF0 },
    { OutlineCursorTiles_11, 0x380, 0xFFF0 },
    { OutlineCursorTiles_12, 0x380, 0xFFF0 },
    { OutlineCursorTiles_13, 0x380, 0xFFF0 },
    { OutlineCursorTiles_14, 0x380, 0xFFF0 },
    { OutlineCursorTiles_15, 0x380, 0xFFF0 }
};

const struct SpriteSheet gBlendedOutlineCursorSpriteSheets[] = {
    { OutlineCursorTiles_00, 0x380, 0xFFF1 },
    { OutlineCursorTiles_01, 0x380, 0xFFF1 },
    { OutlineCursorTiles_02, 0x380, 0xFFF1 },
    { OutlineCursorTiles_03, 0x380, 0xFFF1 },
    { OutlineCursorTiles_04, 0x380, 0xFFF1 },
    { OutlineCursorTiles_05, 0x380, 0xFFF1 },
    { OutlineCursorTiles_06, 0x380, 0xFFF1 },
    { OutlineCursorTiles_07, 0x380, 0xFFF1 },
    { OutlineCursorTiles_08, 0x380, 0xFFF1 },
    { OutlineCursorTiles_09, 0x380, 0xFFF1 },
    { OutlineCursorTiles_10, 0x380, 0xFFF1 },
    { OutlineCursorTiles_11, 0x380, 0xFFF1 },
    { OutlineCursorTiles_12, 0x380, 0xFFF1 },
    { OutlineCursorTiles_13, 0x380, 0xFFF1 },
    { OutlineCursorTiles_14, 0x380, 0xFFF1 },
    { OutlineCursorTiles_15, 0x380, 0xFFF1 }
};

const struct SpritePalette gOutlineCursorSpritePalette = {
    sMenuCursorPalette, 0xfff0
};
const struct SpritePalette gBlendedOutlineCursorSpritePalette = {
    sMenuCursorPalette, 0xfff1
};

const struct SpriteTemplate gOutlineCursorSpriteTemplates[] = {
    {
        0xfff0, 0xfff0, &gOamData_OutlineCursor, gSpriteAnimTable_OutlineCursor, NULL, gDummySpriteAffineAnimTable, SpriteCallbackDummy
    }, {
        0xfff0, 0xffff, &gOamData_OutlineCursor, gSpriteAnimTable_OutlineCursor, NULL, gDummySpriteAffineAnimTable, SpriteCallbackDummy
    }, {
        0xfff0, 0xffff, &gOamData_OutlineCursorWindow, gSpriteAnimTable_OutlineCursor, NULL, gDummySpriteAffineAnimTable, SpriteCallbackDummy
    }
};
const struct SpriteTemplate gBlendedOutlineCursorSpriteTemplates[] = {
    {
        0xfff1, 0xfff1, &gOamData_BlendedOutlineCursor, gSpriteAnimTable_OutlineCursor, NULL, gDummySpriteAffineAnimTable, SpriteCallbackDummy
    }, {
        0xfff1, 0xffff, &gOamData_BlendedOutlineCursor, gSpriteAnimTable_OutlineCursor, NULL, gDummySpriteAffineAnimTable, SpriteCallbackDummy
    }
};


const struct Subsprite sSubsprites_BlendedOutlineCursor_Width0Or1[] = {
    {  -1,  -1, ST_OAM_V_RECTANGLE, 1,   0, 0 },
    {   1,  -1, ST_OAM_V_RECTANGLE, 1,   8, 0 }
};

const struct Subsprite sSubsprites_BlendedOutlineCursor_Width2[] = {
    {  -1,  -1, ST_OAM_V_RECTANGLE, 1,   0, 0 },
    {   1,  -1, ST_OAM_V_RECTANGLE, 1,   4, 0 },
    {   9,  -1, ST_OAM_V_RECTANGLE, 1,   8, 0 }
};

const struct Subsprite sSubsprites_BlendedOutlineCursor_Width3[] = {
    {  -1,  -1, ST_OAM_V_RECTANGLE, 1,   0, 0 },
    {   1,  -1, ST_OAM_V_RECTANGLE, 1,   4, 0 },
    {   9,  -1, ST_OAM_V_RECTANGLE, 1,   4, 0 },
    {  17,  -1, ST_OAM_V_RECTANGLE, 1,   8, 0 }
};

const struct Subsprite sSubsprites_BlendedOutlineCursor_Width4[] = {
    {  -1,  -1, ST_OAM_V_RECTANGLE, 1,   0, 0 },
    {   1,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  25,  -1, ST_OAM_V_RECTANGLE, 1,   8, 0 }
};

const struct Subsprite sSubsprites_BlendedOutlineCursor_Width5[] = {
    {  -1,  -1, ST_OAM_V_RECTANGLE, 1,   0, 0 },
    {   1,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  33,  -1, ST_OAM_V_RECTANGLE, 1,   8, 0 }
};

const struct Subsprite sSubsprites_BlendedOutlineCursor_Width6[] = {
    {  -1,  -1, ST_OAM_V_RECTANGLE, 1,   0, 0 },
    {   1,  -1, ST_OAM_V_RECTANGLE, 1,   4, 0 },
    {   9,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  41,  -1, ST_OAM_V_RECTANGLE, 1,   8, 0 }
};

const struct Subsprite sSubsprites_BlendedOutlineCursor_Width7[] = {
    {  -1,  -1, ST_OAM_V_RECTANGLE, 1,   0, 0 },
    {   1,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  17,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  49,  -1, ST_OAM_V_RECTANGLE, 1,   8, 0 }
};

const struct Subsprite sSubsprites_BlendedOutlineCursor_Width8[] = {
    {  -1,  -1, ST_OAM_V_RECTANGLE, 1,   0, 0 },
    {   1,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  25,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  57,  -1, ST_OAM_V_RECTANGLE, 1,   8, 0 }
};

const struct Subsprite sSubsprites_BlendedOutlineCursor_Width9[] = {
    {  -1,  -1, ST_OAM_V_RECTANGLE, 1,   0, 0 },
    {   1,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  33,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  65,  -1, ST_OAM_V_RECTANGLE, 1,   8, 0 }
};

const struct Subsprite sSubsprites_BlendedOutlineCursor_Width10[] = {
    {  -1,  -1, ST_OAM_V_RECTANGLE, 1,   0, 0 },
    {   1,  -1, ST_OAM_V_RECTANGLE, 1,   4, 0 },
    {   9,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  41,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  73,  -1, ST_OAM_V_RECTANGLE, 1,   8, 0 }
};

const struct Subsprite sSubsprites_BlendedOutlineCursor_Width11[] = {
    {  -1,  -1, ST_OAM_V_RECTANGLE, 1,   0, 0 },
    {   1,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  33,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  49,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  81,  -1, ST_OAM_V_RECTANGLE, 1,   8, 0 }
};

const struct Subsprite sSubsprites_BlendedOutlineCursor_Width12[] = {
    {  -1,  -1, ST_OAM_V_RECTANGLE, 1,   0, 0 },
    {   1,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  33,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  57,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  89,  -1, ST_OAM_V_RECTANGLE, 1,   8, 0 }
};

const struct Subsprite sSubsprites_BlendedOutlineCursor_Width13[] = {
    {  -1,  -1, ST_OAM_V_RECTANGLE, 1,   0, 0 },
    {   1,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  33,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  65,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  97,  -1, ST_OAM_V_RECTANGLE, 1,   8, 0 }
};

const struct Subsprite sSubsprites_BlendedOutlineCursor_Width14[] = {
    {  -1,  -1, ST_OAM_V_RECTANGLE, 1,   0, 0 },
    {   1,  -1, ST_OAM_V_RECTANGLE, 1,   4, 0 },
    {   9,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  41,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  73,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    { 105,  -1, ST_OAM_V_RECTANGLE, 1,   8, 0 }
};

const struct Subsprite sSubsprites_BlendedOutlineCursor_Width15[] = {
    {  -1,  -1, ST_OAM_V_RECTANGLE, 1,   0, 0 },
    {   1,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  33,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  65,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  81,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    { 113,  -1, ST_OAM_V_RECTANGLE, 1,   8, 0 }
};

const struct Subsprite sSubsprites_BlendedOutlineCursor_Width16[] = {
    {  -1,  -1, ST_OAM_V_RECTANGLE, 1,   0, 0 },
    {   1,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  33,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  65,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  89,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    { 121,  -1, ST_OAM_V_RECTANGLE, 1,   8, 0 }
};

const struct Subsprite sSubsprites_BlendedOutlineCursor_Width17[] = {
    {  -1,  -1, ST_OAM_V_RECTANGLE, 1,   0, 0 },
    {   1,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  33,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  65,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  97,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    { 129,  -1, ST_OAM_V_RECTANGLE, 1,   8, 0 }
};

const struct Subsprite sSubsprites_BlendedOutlineCursor_Width18[] = {
    {  -1,  -1, ST_OAM_V_RECTANGLE, 1,   0, 0 },
    {   1,  -1, ST_OAM_V_RECTANGLE, 1,   4, 0 },
    {   9,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  41,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  73,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    { 105,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    { 137,  -1, ST_OAM_V_RECTANGLE, 1,   8, 0 }
};

const struct Subsprite sSubsprites_OutlineCursorWindow[] = {
    {  -1,  -1, ST_OAM_V_RECTANGLE, 0,   0, 0 },
    {   7,  -1, ST_OAM_SQUARE, 2,  12, 0 },
    {  35,  -1, ST_OAM_V_RECTANGLE, 0,   8, 0 }
};

const struct Subsprite *const gOutlineCursorWindowSubspritePointers[] = {
    sSubsprites_OutlineCursorWindow
};

const struct SubspriteTable gDynamicOutlineCursorSubspriteTables[] = {
    {1, sMenuCursorSubsprites},
    {1, sMenuCursorSubsprites},
    {2, sMenuCursorSubsprites},
    {3, sMenuCursorSubsprites},
    {4, sMenuCursorSubsprites},
    {5, sMenuCursorSubsprites},
    {6, sMenuCursorSubsprites},
    {7, sMenuCursorSubsprites},
    {8, sMenuCursorSubsprites},
    {9, sMenuCursorSubsprites},
    {10, sMenuCursorSubsprites},
    {11, sMenuCursorSubsprites},
    {12, sMenuCursorSubsprites},
    {13, sMenuCursorSubsprites},
    {14, sMenuCursorSubsprites},
    {15, sMenuCursorSubsprites},
    {16, sMenuCursorSubsprites},
    {17, sMenuCursorSubsprites},
    {18, sMenuCursorSubsprites},
    {19, sMenuCursorSubsprites},
    {20, sMenuCursorSubsprites},
    {21, sMenuCursorSubsprites},
    {22, sMenuCursorSubsprites},
    {23, sMenuCursorSubsprites},
    {24, sMenuCursorSubsprites},
    {25, sMenuCursorSubsprites},
    {26, sMenuCursorSubsprites},
    {27, sMenuCursorSubsprites},
    {28, sMenuCursorSubsprites},
    {29, sMenuCursorSubsprites},
    {30, sMenuCursorSubsprites},
    {31, sMenuCursorSubsprites}
};

const struct SubspriteTable gBlendedOutlineCursorSubspriteTables[] = {
    {2, sSubsprites_BlendedOutlineCursor_Width0Or1},
    {2, sSubsprites_BlendedOutlineCursor_Width0Or1},
    {3, sSubsprites_BlendedOutlineCursor_Width2},
    {4, sSubsprites_BlendedOutlineCursor_Width3},
    {3, sSubsprites_BlendedOutlineCursor_Width4},
    {3, sSubsprites_BlendedOutlineCursor_Width5},
    {4, sSubsprites_BlendedOutlineCursor_Width6},
    {4, sSubsprites_BlendedOutlineCursor_Width7},
    {4, sSubsprites_BlendedOutlineCursor_Width8},
    {4, sSubsprites_BlendedOutlineCursor_Width9},
    {5, sSubsprites_BlendedOutlineCursor_Width10},
    {5, sSubsprites_BlendedOutlineCursor_Width11},
    {5, sSubsprites_BlendedOutlineCursor_Width12},
    {5, sSubsprites_BlendedOutlineCursor_Width13},
    {6, sSubsprites_BlendedOutlineCursor_Width14},
    {6, sSubsprites_BlendedOutlineCursor_Width15},
    {6, sSubsprites_BlendedOutlineCursor_Width16},
    {6, sSubsprites_BlendedOutlineCursor_Width17},
    {7, sSubsprites_BlendedOutlineCursor_Width18}
};

const struct SubspriteTable gOutlineCursorWindowSubspriteTable[] = {
    {3, sMenuCursorSubsprites}
};

const struct Subsprite gUnusedOutlineCursorSubsprite_Left = {
    .y = -1, .shape = ST_OAM_V_RECTANGLE, .size = 1
};

const struct Subsprite gUnusedOutlineCursorSubsprite_Right = {
    .y = -1, .shape = ST_OAM_V_RECTANGLE, .size = 1, .tileOffset = 8
};

const struct Subsprite gUnusedOutlineCursorSubsprite_BodyLarge = {
    .y = -1, .shape = ST_OAM_SQUARE, .size = 2, .tileOffset = 12
};

const struct Subsprite gUnusedOutlineCursorSubsprite_BodySmall = {
    .y = -1, .shape = ST_OAM_V_RECTANGLE, .size = 1, .tileOffset = 4
};

const struct Subsprite gOutlineCursorSubsprite_Left = {
    .y = -1, .shape = ST_OAM_V_RECTANGLE, .size = 1
};

const struct Subsprite gOutlineCursorSubsprite_BodyLarge = {
    .y = -1, .shape = ST_OAM_SQUARE, .size = 2, .tileOffset = 12
};

const struct Subsprite gOutlineCursorSubsprite_BodySmall = {
    .y = -1, .shape = ST_OAM_V_RECTANGLE, .size = 1, .tileOffset = 4
};

const struct Subsprite gOutlineCursorSubsprite_Right = {
    .y = -1, .shape = ST_OAM_V_RECTANGLE, .size = 1, .tileOffset = 8
};

#endif //GUARD_DATA_MENU_CURSOR_H
