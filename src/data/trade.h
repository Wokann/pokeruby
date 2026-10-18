static const u32 UNUSED sUnusedStructSizes[] =
{
    0x00000890,
    0x00003AC0,
    0x0000001C,
    0x00000530,
    0x00000024,
    0x00000064,
    0x000004D8
};

static const u16 UNUSED sTradeMovesBoxTilemap[] = INCBIN_U16("graphics/trade/moves_box_map.bin");
static const u16 sTradePartyBoxTilemap[] = INCBIN_U16("graphics/trade/party_box_map.bin");
static const u16 sTradeStripesBG2Tilemap[] = INCBIN_U16("graphics/trade/stripes_bg2_map.bin");
static const u16 sTradeStripesBG3Tilemap[] = INCBIN_U16("graphics/trade/stripes_bg3_map.bin");

static const struct OamData sOamData_MenuText = {
    .shape = ST_OAM_H_RECTANGLE,
    .size = 2,
    .priority = 1
};

static const union AnimCmd sAnim_MenuText_0[] = {
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END
};

static const union AnimCmd sAnim_MenuText_1[] = {
    ANIMCMD_FRAME(8, 5),
    ANIMCMD_END
};

static const union AnimCmd sAnim_MenuText_2[] = {
    ANIMCMD_FRAME(16, 5),
    ANIMCMD_END
};

static const union AnimCmd sAnim_MenuText_3[] = {
    ANIMCMD_FRAME(24, 5),
    ANIMCMD_END
};

static const union AnimCmd sAnim_MenuText_4[] = {
    ANIMCMD_FRAME(32, 5),
    ANIMCMD_END
};

static const union AnimCmd sAnim_MenuText_5[] = {
    ANIMCMD_FRAME(40, 5),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_MenuText[] = {
    sAnim_MenuText_0,
    sAnim_MenuText_1,
    sAnim_MenuText_2,
    sAnim_MenuText_3,
    sAnim_MenuText_4,
    sAnim_MenuText_5
};

static const u16 sMenuText_Pal[] = INCBIN_U16("graphics/trade/text.gbapal");
const u16 UnrefTradeScreenTextPalette[] = INCBIN_U16("graphics/trade/text2.gbapal");

const struct SpriteSheet gUnknown_0820C07C[] = {
    {ewram_2010000.tileBuffers[ 0], 256, 200},
    {ewram_2010000.tileBuffers[ 1], 256, 201},
    {ewram_2010000.tileBuffers[ 2], 256, 202},
    {ewram_2010000.tileBuffers[ 3], 256, 203},
    {ewram_2010000.tileBuffers[ 4], 256, 204},
    {ewram_2010000.tileBuffers[ 5], 256, 205},
    {ewram_2010000.tileBuffers[ 6], 256, 206},
    {ewram_2010000.tileBuffers[ 7], 256, 207},
    {ewram_2010000.tileBuffers[ 8], 256, 208},
    {ewram_2010000.tileBuffers[ 9], 256, 209},
    {ewram_2010000.tileBuffers[10], 256, 210},
    {ewram_2010000.tileBuffers[11], 256, 211},
    {ewram_2010000.tileBuffers[12], 256, 212},
};

static const struct SpritePalette sSpritePalette_MenuText = {
    sMenuText_Pal, 4925
};

static const struct SpriteTemplate sSpriteTemplate_MenuText = {
    200,
    4925,
    &sOamData_MenuText,
    sAnims_MenuText,
    NULL,
    gDummySpriteAffineAnimTable,
    SpriteCallbackDummy
};

static const struct OamData sOamData_Cursor = {
    .shape = ST_OAM_H_RECTANGLE,
    .size = 3,
    .priority = 1
};

static const union AnimCmd sAnim_Cursor_Normal[] = {
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END
};

static const union AnimCmd sAnim_Cursor_OnCancel[] = {
    ANIMCMD_FRAME(32, 5),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_Cursor[] = {
    sAnim_Cursor_Normal,
    sAnim_Cursor_OnCancel
};

static const struct SpriteSheet sCursor_SpriteSheet = {
    gTradeCursor_Gfx, 0x800, 300
};

static const struct SpritePalette sCursor_SpritePalette = {
    gTradeCursor_Pal, 2345
};

static const struct SpriteTemplate sSpriteTemplate_Cursor = {
    300,
    2345,
    &sOamData_Cursor,
    sAnims_Cursor,
    NULL,
    gDummySpriteAffineAnimTable,
    SpriteCallbackDummy
};

static const u8 *const sActionTexts[] = {
    sText_Cancel,
    sText_ChooseAPkmn,
    sText_Summary,
    sText_Trade,
    sText_CancelTrade,
    TradeText_PressBToExit
};

// This is used to determine the next mon to select when the D-Pad is
// pressed in a given direction.
// Note that the mons are laid out like this.
// 0-5 are the player's party and 6-11 are the trading partner's party.
// 12 is the cancel button.
//  0  1  6  7
//  2  3  8  9
//  4  5 10 11
//          12

static const u8 sCursorMoveDestinations[][4][6] = {
    {
        { 4,  2, 12, 12,  0,  0},
        { 2,  4, 12, 12,  0,  0},
        { 7,  6,  1,  0,  0,  0},
        { 1,  6,  7,  0,  0,  0}
    }, {
        { 5,  3, 12, 12,  0,  0},
        { 3,  5, 12, 12,  0,  0},
        { 0,  7,  6,  1,  0,  0},
        { 6,  7,  0,  1,  0,  0}
    }, {
        { 0,  0,  0,  0,  0,  0},
        { 4,  0,  0,  0,  0,  0},
        { 9,  8,  7,  6,  0,  0},
        { 3,  1,  0,  0,  0,  0}
    }, {
        { 1,  1,  1,  1,  0,  0},
        { 5,  1,  1,  1,  0,  0},
        { 2,  9,  8,  7,  0,  0},
        { 8,  9,  6,  6,  0,  0}
    }, {
        { 2,  2,  2,  2,  0,  0},
        { 0,  0,  0,  0,  0,  0},
        {11, 10,  9,  8,  7,  6},
        { 5,  3,  1,  0,  0,  0}
    }, {
        { 3,  3,  3,  3,  0,  0},
        { 1,  1,  1,  1,  0,  0},
        { 4,  4,  4,  4,  0,  0},
        {10,  8,  6,  0,  0,  0}
    }, {
        {10,  8, 12,  0,  0,  0},
        { 8, 10, 12,  0,  0,  0},
        { 1,  0,  0,  0,  0,  0},
        { 7,  0,  1,  0,  0,  0}
    }, {
        {12,  0,  0,  0,  0,  0},
        { 9, 12,  0,  0,  0,  0},
        { 6,  0,  0,  0,  0,  0},
        { 0,  0,  0,  0,  0,  0}
    }, {
        { 6,  0,  0,  0,  0,  0},
        {10,  6,  0,  0,  0,  0},
        { 3,  2,  1,  0,  0,  0},
        { 9,  7,  0,  0,  0,  0}
    }, {
        { 7,  0,  0,  0,  0,  0},
        {11, 12,  0,  0,  0,  0},
        { 8,  0,  0,  0,  0,  0},
        { 2,  1,  0,  0,  0,  0}
    }, {
        { 8,  0,  0,  0,  0,  0},
        { 6,  0,  0,  0,  0,  0},
        { 5,  4,  3,  2,  1,  0},
        {11,  9,  7,  0,  0,  0}
    }, {
        { 9,  0,  0,  0,  0,  0},
        {12,  0,  0,  0,  0,  0},
        {10,  0,  0,  0,  0,  0},
        { 4,  2,  0,  0,  0,  0}
    }, {
        {11,  9,  7,  6,  0,  0},
        { 7,  6,  0,  0,  0,  0},
        {12,  0,  0,  0,  0,  0},
        {12,  0,  0,  0,  0,  0}
    }
};

static const u8 sTradeMonSpriteCoords[][2] = {
    // Your party
    {1, 5},
    {8, 5},
    {1, 10},
    {8, 10},
    {1, 15},
    {8, 15},

    // Friend's party
    {16, 5},
    {23, 5},
    {16, 10},
    {23, 10},
    {16, 15},
    {23, 15},

    {23, 18} // CANCEL
};

static const u8 sTradeMonLevelCoords[][2] = {
    // Your party
    {5, 4},
    {12, 4},
    {5, 9},
    {12, 9},
    {5, 14},
    {12, 14},
    // Friend's party
    {20, 4},
    {27, 4},
    {20, 9},
    {27, 9},
    {20, 14},
    {27, 14}
};

static const u8 sTradeMonBoxCoords[][2] = {
    // Your party
    {1, 3},
    {8, 3},
    {1, 8},
    {8, 8},
    {1, 13},
    {8, 13},
    // Friend's party
    {16, 3},
    {23, 3},
    {16, 8},
    {23, 8},
    {16, 13},
    {23, 13}
};

const u8 gTradeUnknownSpriteCoords[][2][2] = {
    {
        { 59, 10},
        {179, 10},
    },
    {
        { 59, 10},
        {179, 10}
    }
};

static const u8 *const sMessages[] = {
    sText_CommunicationStandby,
    sText_TheTradeHasBeenCanceled,
    sText_OnlyPkmnForBattle,
    TradeText_NonTradablePoke,
    sText_WaitingForYourFriend,
    sText_YourFriendWantsToTrade
};

const u8 gTradeMessageWindowRects[][4] = {
    {8, 7, 22, 12},
    {8, 7, 22, 12},
    {6, 7, 24, 12},
    {6, 7, 24, 12},
    {8, 7, 22, 12},
    {7, 7, 23, 12}
};

static const struct MenuAction2 sSelectTradeMonActions[] = {
    {sText_Summary2, Task_DrawSelectionSummary},
    {sText_Trade2, Task_DrawSelectionTrade}
};

const u8 gUnknown_0820C330[][2] = {
    { 0, 14},
    {15, 29}
};

const u8 gUnknown_0820C334[][2] = {
    { 3,  5},
    { 3,  7},
    {18,  5},
    {18,  7}
};

static const u8 sText_EmptyString[] = _("");
extern const u8 sText_NewLine[];
const u8 gText_MaleSymbol4[] = _("♂");
const u8 gText_FemaleSymbol4[] = _("♀");
const u8 gText_GenderlessSymbol[] = _("$");

u8 *const unref_data_820C344 = gTileBuffer;
const u8 unref_strings_820C348[][13] = {
    _("かいめの そうしん"),
    _("かいめの じゅしん"),
    _("ポケモンアイコンセット"),
    _("OBJテキストセット"),
    _("セルセット"),
    _("OBJテキストADD"),
    _("システムメッセージADD"),
    _("はいけいセット"),
};

static const u8 sText_ShedinjaJP[] = _("ヌケニン");
const u8 unref_string_0820C3B5[] = _("こうかんせいりつ     ");
const u8 unref_string_0820C3C3[] = _("だめだたらしいよ     ");
static const u8 sSelectedMonLevelGenderCoords[][2] = {
    { 4,  3},
    {19,  3},
    { 0,  0}
};

static const u16 sPokeball_Pal[] = INCBIN_U16("graphics/trade/pokeball.gbapal");
static const u32 sPokeball_Gfx[] = INCBIN_U32("graphics/trade/pokeball.4bpp");
const u16 gTradeGba2_Pal[][16] = {
    INCBIN_U16("graphics/trade/unknown.gbapal"),
    INCBIN_U16("graphics/trade/gba.gbapal"),
    INCBIN_U16("graphics/trade/shadow.gbapal"),
    {},
    INCBIN_U16("graphics/trade/misc.gbapal")
};

const u32 gTradeGba_Gfx[] = INCBIN_U32("graphics/trade/gba.4bpp");
const u32 gUnknown_0820CA98_2[] = INCBIN_U32("graphics/trade/shadow.4bpp");
static const u32 sPokeballSymbol_Gfx[] = INCBIN_U32("graphics/trade/pokeball_symbol.8bpp");
const u16 gTradePlatform_Tilemap[] = INCBIN_U16("graphics/trade/platform.bin");
static const u16 sGbaMapCable[] = INCBIN_U16("graphics/trade/gba_map_cable.bin");
static const u16 sCableCloseup_Map[] = INCBIN_U16("graphics/trade/crossing_highlight_cable.bin");
static const u16 sPokeballSymbol_Map[] = INCBIN_U16("graphics/trade/pokeball_symbol_map.bin");
const u16 gTradeUnusedPal1[] = INCBIN_U16("graphics/trade/unused1.gbapal");
static const u16 sGba_Pal[] = INCBIN_U16("graphics/trade/cable_end.gbapal");
const u16 gTradeUnusedPal2[] = INCBIN_U16("graphics/trade/unused2.gbapal");
const u16 nullpal_082120F8[16] = {};
static const u16 sLinkMon_Pal[] = INCBIN_U16("graphics/trade/link_mon.gbapal");
static const u32 sLinkMonGlow_Gfx[] = INCBIN_U32("graphics/trade/link_mon_glow.4bpp");
static const u32 sLinkMonShadow_Gfx[] = INCBIN_U32("graphics/trade/link_mon_shadow.4bpp");
static const u32 sCableEnd_Gfx[] = INCBIN_U32("graphics/trade/cable_end.4bpp");
static const u32 sGbaScreen_Gfx[] = INCBIN_U32("graphics/trade/gba_screen.4bpp");
static const u32 sGbaAffine_Gfx[] = INCBIN_U32("graphics/trade/gba_affine.8bpp");
static const u16 sGbaAffineMapCable[] = INCBIN_U16("graphics/trade/gba_affine_map_cable.bin");

static const struct OamData sOamData_Pokeball = {
    .affineMode = 1,
    .size = 1
};

static const union AnimCmd sAnim_Pokeball_SpinOnce[] = {
    ANIMCMD_FRAME( 0, 3),
    ANIMCMD_FRAME( 4, 3),
    ANIMCMD_FRAME( 8, 3),
    ANIMCMD_FRAME(12, 3),
    ANIMCMD_FRAME(16, 3),
    ANIMCMD_FRAME(20, 3),
    ANIMCMD_FRAME(24, 3),
    ANIMCMD_FRAME(28, 3),
    ANIMCMD_FRAME(32, 3),
    ANIMCMD_FRAME(36, 3),
    ANIMCMD_FRAME(40, 3),
    ANIMCMD_FRAME(44, 3),
    ANIMCMD_LOOP(1),
    ANIMCMD_FRAME( 0, 3),
    ANIMCMD_END
};

static const union AnimCmd sAnim_Pokeball_SpinTwice[] = {
    ANIMCMD_FRAME( 0, 3),
    ANIMCMD_FRAME( 4, 3),
    ANIMCMD_FRAME( 8, 3),
    ANIMCMD_FRAME(12, 3),
    ANIMCMD_FRAME(16, 3),
    ANIMCMD_FRAME(20, 3),
    ANIMCMD_FRAME(24, 3),
    ANIMCMD_FRAME(28, 3),
    ANIMCMD_FRAME(32, 3),
    ANIMCMD_FRAME(36, 3),
    ANIMCMD_FRAME(40, 3),
    ANIMCMD_FRAME(44, 3),
    ANIMCMD_LOOP(2),
    ANIMCMD_FRAME( 0, 3),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_Pokeball[] = {
    sAnim_Pokeball_SpinOnce,
    sAnim_Pokeball_SpinTwice
};

static const union AffineAnimCmd sAffineAnim_Pokeball_Normal[] = {
    AFFINEANIMCMD_FRAME(0, 0, 0, 1),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_Pokeball_Squish[] = {
    AFFINEANIMCMD_FRAME(-8, 0, 0, 20),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_Pokeball_Unsquish[] = {
    AFFINEANIMCMD_FRAME(0x60, 0x100, 0,  0),
    AFFINEANIMCMD_FRAME(   0,     0, 0,  5),
    AFFINEANIMCMD_FRAME(   8,     0, 0, 20),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd *const sAffineAnims_Pokeball[] = {
    sAffineAnim_Pokeball_Normal,
    sAffineAnim_Pokeball_Squish,
    sAffineAnim_Pokeball_Unsquish
};

static const struct SpriteSheet sPokeBallSpriteSheet = {
    (const u8 *)sPokeball_Gfx, 0x600, 5557
};

static const struct SpritePalette sPokeBallSpritePalette = {
    sPokeball_Pal, 5558
};

static const struct SpriteTemplate sSpriteTemplate_Pokeball = {
    5557,
    5558,
    &sOamData_Pokeball,
    sAnims_Pokeball,
    NULL,
    sAffineAnims_Pokeball,
    SpriteCB_BouncingPokeball
};

static const struct OamData sOamData_LinkMonGlow = {
    .affineMode = 1,
    .objMode = 1,
    .size = 2,
    .priority = 1
};

static const union AnimCmd sAnim_LinkMonGlow[] = {
    ANIMCMD_FRAME(0, 5, .hFlip = TRUE, .vFlip = TRUE),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_LinkMonGlow[] = {
    sAnim_LinkMonGlow
};

static const union AffineAnimCmd sAffineAnim_LinkMonGlow[] = {
    AFFINEANIMCMD_FRAME(-10, -10, 0, 5),
    AFFINEANIMCMD_FRAME(10, 10, 0, 5),
    AFFINEANIMCMD_JUMP(0)
};

static const union AffineAnimCmd *const sAffineAnims_LinkMonGlow[] = {
    sAffineAnim_LinkMonGlow
};

static const struct SpriteSheet sSpriteSheet_LinkMonGlow = {
    (const u8 *)sLinkMonGlow_Gfx, 0x200, 5550
};

static const struct SpritePalette sSpritePalette_LinkMon = {
    sLinkMon_Pal, 5551
};

static const struct SpritePalette sSpritePalette_Gba = {
    sGba_Pal, 5555
};

static const struct SpriteTemplate sSpriteTemplate_LinkMonGlow = {
    5550,
    5551,
    &sOamData_LinkMonGlow,
    sAnims_LinkMonGlow,
    NULL,
    sAffineAnims_LinkMonGlow,
    SpriteCB_LinkMonGlow
};

static const struct OamData sOamData_LinkMonShadow = {
    .shape = ST_OAM_V_RECTANGLE,
    .size = 2,
    .priority = 1
};

static const union AnimCmd sAnim_LinkMonShadow_Big[] = {
    ANIMCMD_FRAME(0, 5, .vFlip = TRUE, .hFlip = TRUE),
    ANIMCMD_END
};

static const union AnimCmd sAnim_LinkMonShadow_Small[] = {
    ANIMCMD_FRAME(8, 5, .vFlip = TRUE, .hFlip = TRUE),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_LinkMonShadow[] = {
    sAnim_LinkMonShadow_Big,
    sAnim_LinkMonShadow_Small
};

static const struct SpriteSheet sSpriteSheet_LinkMonShadow = {
    (const u8 *)sLinkMonShadow_Gfx, 0x300, 5552
};

static const struct SpriteTemplate sSpriteTemplate_LinkMonShadow = {
    5552,
    5551,
    &sOamData_LinkMonShadow,
    sAnims_LinkMonShadow,
    NULL,
    gDummySpriteAffineAnimTable,
    SpriteCB_LinkMonShadow
};

static const struct OamData sOamData_CableEnd = {
    .shape = ST_OAM_V_RECTANGLE,
    .size = 2,
    .priority = 1
};

static const union AnimCmd sAnim_CableEnd[] = {
    ANIMCMD_FRAME(0, 10),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_CableEnd[] = {
    sAnim_CableEnd
};

static const struct SpriteSheet sSpriteSheet_CableEnd = {
    (const u8 *)sCableEnd_Gfx, 0x100, 5554
};

static const struct SpriteTemplate sSpriteTemplate_CableEnd = {
    5554,
    5555,
    &sOamData_CableEnd,
    sAnims_CableEnd,
    NULL,
    gDummySpriteAffineAnimTable,
    SpriteCB_CableEndSending
};

static const struct OamData sOamData_GbaScreen = {
    .shape = ST_OAM_H_RECTANGLE,
    .size = 3,
    .priority = 1
};

static const union AnimCmd sAnim_GbaScreen_Long[] = {
    ANIMCMD_FRAME( 0, 2, .vFlip = TRUE, .hFlip = TRUE),
    ANIMCMD_FRAME(32, 2, .vFlip = TRUE, .hFlip = TRUE),
    ANIMCMD_FRAME(64, 2, .vFlip = TRUE, .hFlip = TRUE),
    ANIMCMD_FRAME(96, 2, .vFlip = TRUE, .hFlip = TRUE),
    ANIMCMD_FRAME(64, 2, .vFlip = TRUE, .hFlip = TRUE),
    ANIMCMD_FRAME(32, 2, .vFlip = TRUE, .hFlip = TRUE),
    ANIMCMD_FRAME( 0, 2, .vFlip = TRUE, .hFlip = TRUE),
    ANIMCMD_LOOP(8),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_GbaScreen_Long[] = {
    sAnim_GbaScreen_Long
};

static const struct SpriteSheet sSpriteSheet_GbaScreen = {
    (const u8 *)sGbaScreen_Gfx, 0x1000, 5556
};

static const struct SpriteTemplate sSpriteTemplate_GbaScreenFlash_Long = {
    5556,
    5555,
    &sOamData_GbaScreen,
    sAnims_GbaScreen_Long,
    NULL,
    gDummySpriteAffineAnimTable,
    SpriteCB_GbaScreen
};

static const u16 sLinkMonShadow_Pal[] = {
    RGB(18, 24, 31),
    RGB(18, 24, 31),
    RGB(18, 24, 31),
    RGB(31, 31, 31),
    RGB(31, 31, 31),
    RGB(31, 31, 31),
    RGB(18, 24, 31),
    RGB(18, 24, 31),
    RGB(18, 24, 31),
    RGB(31, 31, 31),
    RGB(31, 31, 31),
    RGB(31, 31, 31),
};

static const union AffineAnimCmd sAffineAnim_CrossingMonPic[] = {
    AFFINEANIMCMD_FRAME(-0x100, 0x100, 0, 0),
    AFFINEANIMCMD_JUMP(0)
};

static const union AffineAnimCmd *const sAffineAnims_CrossingMonPics[] = {
    sAffineAnim_CrossingMonPic
};

static const struct InGameTrade sIngameTrades[] = {
#if ENGLISH
    {
        _("MAKIT"), SPECIES_MAKUHITA,
        5, 5, 4, 4, 4, 4,
        TRUE, 49562,
        5, 5, 5, 5, 30,
        0x9C40,
        ITEM_X_ATTACK, -1,
        _("ELYSSA"), MALE, 10,
        SPECIES_SLAKOTH
    }, {
        _("SKITIT"), SPECIES_SKITTY,
        5, 4, 4, 5, 4, 4,
        FALSE, 2259,
        5, 5, 30, 5, 5,
        0x498A2E17,
        ITEM_GLITTER_MAIL, 0,
        _("DARRELL"), FEMALE, 10,
        SPECIES_PIKACHU
    }, {
        _("COROSO"),
        SPECIES_CORSOLA,
        4, 4, 5, 4, 4, 5,
        TRUE, 50183,
        5, 30, 5, 5, 5,
        0x4C970B7F,
        ITEM_TROPIC_MAIL, 1,
        _("LANE"), FEMALE, 10,
        SPECIES_BELLOSSOM
    }
#elif GERMAN
    {
        _("MAKIT"), SPECIES_MAKUHITA,
        5, 5, 4, 4, 4, 4,
        TRUE, 49562,
        5, 5, 5, 5, 30,
        0x9C40,
        ITEM_X_ATTACK, -1,
        _("MAIK"), MALE, 10,
        SPECIES_SLAKOTH
    }, {
        _("CONEC"), SPECIES_SKITTY,
        5, 4, 4, 5, 4, 4,
        FALSE, 2259,
        5, 5, 30, 5, 5,
        0x498A2E17,
        ITEM_GLITTER_MAIL, 0,
        _("MADINA"), FEMALE, 10,
        SPECIES_PIKACHU
    }, {
        _("CORASO"),
        SPECIES_CORSOLA,
        4, 4, 5, 4, 4, 5,
        TRUE, 50183,
        5, 30, 5, 5, 5,
        0x4C970B7F,
        ITEM_TROPIC_MAIL, 1,
        _("LIANA"), FEMALE, 10,
        SPECIES_BELLOSSOM
    }
#endif
};

static const u16 sIngameTradeMail[][10] = {
#if ENGLISH
    {
        EC_POKEMON(PIKACHU),
        EC_WORD_THANK_YOU,
        EC_WORD_EXCL,
        EC_WORD_MY,
        EC_POKEMON(SKITTY),
        EC_WORD_EATS,
        EC_WORD_A_LOT,
        EC_WORD_NOW,
        EC_WORD_EXCL,
        0
    }, {
        EC_WORD_I,
        EC_WORD_WANT,
        EC_WORD_TO,
        EC_WORD_SEE,
        EC_WORD_A,
        EC_MOVE2(PETAL_DANCE),
        EC_WORD_IT_S,
        EC_WORD_SO,
        EC_WORD_PRETTY,
        0
    }
#elif GERMAN
    {
        EC_POKEMON(PIKACHU),
        EC_WORD_THANK_YOU,
        EC_WORD_EXCL,
        EC_WORD_MY,
        EC_POKEMON(SKITTY),
        EC_WORD_EATS,
        0xFFFF,
        EC_WORD_A_LOT,
        EC_WORD_EXCL,
        0
    }, {
        EC_WORD_I,
        EC_WORD_WANT,
        EC_WORD_OF,
        EC_MOVE2(PETAL_DANCE),
        EC_WORD_WORKS,
        EC_WORD_THE,
        EC_WORD_IS,
        EC_WORD_SO,
        EC_WORD_PRETTY,
        0
    }
#endif
};

static const s8 sTradeBallVerticalVelocityTable[] = {
    0,  0,  1,  0,
    1,  0,  1,  1,
    1,  1,  2,  2,
    2,  2,  3,  3,
    3,  3,  4,  4,
    4,  4, -4, -4,
    -4, -3, -3, -3,
    -3, -2, -2, -2,
    -2, -1, -1, -1,
    -1,  0, -1,  0,
    -1,  0,  0,  0,
    0,  0,  1,  0,
    1,  0,  1,  1,
    1,  1,  2,  2,
    2,  2,  3,  3,
    3,  3,  4,  4,
    4,  4, -4, -3,
    -3, -2, -2, -1,
    -1, -1,  0, -1,
    0,  0,  0,  0,
    0,  0,  1,  0,
    1,  1,  1,  2,
    2,  3,  3,  4,
    -4, -3, -2, -1,
    -1, -1,  0,  0,
    0,  0,  1,  0,
    1,  1,  2,  3
};
