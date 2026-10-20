#define GFXTAG_MENU_TEXT       200 // Used as a base tag in CB2_CreateTradeMenu and CB2_ReturnToTradeMenu
#define GFXTAG_CURSOR          300
#define GFXTAG_LINK_MON_GLOW   5550
#define GFXTAG_LINK_MON_SHADOW 5552
#define GFXTAG_CABLE_END       5554
#define GFXTAG_GBA_SCREEN      5556
#define GFXTAG_POKEBALL        5557

#define PALTAG_CURSOR    2345
#define PALTAG_MENU_TEXT 4925
#define PALTAG_LINK_MON  5551
#define PALTAG_GBA       5555
#define PALTAG_POKEBALL  5558

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
    .shape = SPRITE_SHAPE(32x16),
    .size = SPRITE_SIZE(32x16),
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
    {ewram_2010000.tileBuffers[ GFXTAG_PLAYER_NAME_L], 256, GFXTAG_MENU_TEXT + GFXTAG_PLAYER_NAME_L},
    {ewram_2010000.tileBuffers[ GFXTAG_PLAYER_NAME_M], 256, GFXTAG_MENU_TEXT + GFXTAG_PLAYER_NAME_M},
    {ewram_2010000.tileBuffers[ GFXTAG_PLAYER_NAME_R], 256, GFXTAG_MENU_TEXT + GFXTAG_PLAYER_NAME_R},
    {ewram_2010000.tileBuffers[ GFXTAG_PARTNER_NAME_L], 256, GFXTAG_MENU_TEXT + GFXTAG_PARTNER_NAME_L},
    {ewram_2010000.tileBuffers[ GFXTAG_PARTNER_NAME_M], 256, GFXTAG_MENU_TEXT + GFXTAG_PARTNER_NAME_M},
    {ewram_2010000.tileBuffers[ GFXTAG_PARTNER_NAME_R], 256, GFXTAG_MENU_TEXT + GFXTAG_PARTNER_NAME_R},
    {ewram_2010000.tileBuffers[ GFXTAG_CANCEL_L], 256, GFXTAG_MENU_TEXT + GFXTAG_CANCEL_L},
    {ewram_2010000.tileBuffers[ GFXTAG_CANCEL_R], 256, GFXTAG_MENU_TEXT + GFXTAG_CANCEL_R},
    {ewram_2010000.tileBuffers[ GFXTAG_CHOOSE_PKMN_L], 256, GFXTAG_MENU_TEXT + GFXTAG_CHOOSE_PKMN_L},
    {ewram_2010000.tileBuffers[ GFXTAG_CHOOSE_PKMN_M], 256, GFXTAG_MENU_TEXT + GFXTAG_CHOOSE_PKMN_M},
    {ewram_2010000.tileBuffers[GFXTAG_CHOOSE_PKMN_R], 256, GFXTAG_MENU_TEXT + GFXTAG_CHOOSE_PKMN_R},
    {ewram_2010000.tileBuffers[GFXTAG_CHOOSE_PKMN_EMPTY_1], 256, GFXTAG_MENU_TEXT + GFXTAG_CHOOSE_PKMN_EMPTY_1},
    {ewram_2010000.tileBuffers[GFXTAG_CHOOSE_PKMN_EMPTY_2], 256, GFXTAG_MENU_TEXT + GFXTAG_CHOOSE_PKMN_EMPTY_2},
};

static const struct SpritePalette sSpritePalette_MenuText = {
    .data = sMenuText_Pal,
    .tag = PALTAG_MENU_TEXT,
};

static const struct SpriteTemplate sSpriteTemplate_MenuText = {
    .tileTag = GFXTAG_MENU_TEXT,
    .paletteTag = PALTAG_MENU_TEXT,
    .oam = &sOamData_MenuText,
    .anims = sAnims_MenuText,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

static const struct OamData sOamData_Cursor = {
    .shape = SPRITE_SHAPE(64x32),
    .size = SPRITE_SIZE(64x32),
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

enum {
    CURSOR_ANIM_NORMAL,
    CURSOR_ANIM_ON_CANCEL,
};

static const union AnimCmd *const sAnims_Cursor[] = {
    [CURSOR_ANIM_NORMAL] = sAnim_Cursor_Normal,
    [CURSOR_ANIM_ON_CANCEL] = sAnim_Cursor_OnCancel
};

static const struct SpriteSheet sCursor_SpriteSheet = {
    .data = gTradeCursor_Gfx,
    .size = 0x800,
    .tag = GFXTAG_CURSOR,
};

static const struct SpritePalette sCursor_SpritePalette = {
    .data = gTradeCursor_Pal,
    .tag = PALTAG_CURSOR,
};

static const struct SpriteTemplate sSpriteTemplate_Cursor = {
    .tileTag = GFXTAG_CURSOR,
    .paletteTag = PALTAG_CURSOR,
    .oam = &sOamData_Cursor,
    .anims = sAnims_Cursor,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

static const u8 *const sActionTexts[] = {
    [TEXT_CANCEL] = sText_Cancel,
    [TEXT_CHOOSE_MON] = sText_ChooseAPkmn,
    [TEXT_SUMMARY] = sText_Summary,
    [TEXT_TRADE] = sText_Trade,
    [TEXT_CANCEL_TRADE] = sText_CancelTrade,
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

static const u8 sCursorMoveDestinations[PARTY_SIZE * 2 + 1][4][PARTY_SIZE] = {
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

#define COL0_X 1
#define COL1_X 8
#define COL2_X 16
#define COL3_X 23
#define ROW0_Y 5
#define ROW1_Y 10
#define ROW2_Y 15
#define ROW3_Y 18

static const u8 sTradeMonSpriteCoords[PARTY_SIZE * 2 + 1][2] = {
    // Your party
    {COL0_X, ROW0_Y},
    {COL1_X, ROW0_Y},
    {COL0_X, ROW1_Y},
    {COL1_X, ROW1_Y},
    {COL0_X, ROW2_Y},
    {COL1_X, ROW2_Y},

    // Friend's party
    {COL2_X, ROW0_Y},
    {COL3_X, ROW0_Y},
    {COL2_X, ROW1_Y},
    {COL3_X, ROW1_Y},
    {COL2_X, ROW2_Y},
    {COL3_X, ROW2_Y},

    {COL3_X, ROW3_Y} // CANCEL
};

static const u8 sTradeMonLevelCoords[PARTY_SIZE * 2][2] = {
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

static const u8 sTradeMonBoxCoords[PARTY_SIZE * 2][2] = {
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
    [MSG_STANDBY] = sText_CommunicationStandby,
    [MSG_CANCELED] = sText_TheTradeHasBeenCanceled,
    [MSG_ONLY_MON1] = sText_OnlyPkmnForBattle,
    TradeText_NonTradablePoke,
    [MSG_WAITING_FOR_FRIEND] = sText_WaitingForYourFriend,
    [MSG_FRIEND_WANTS_TO_TRADE] = sText_YourFriendWantsToTrade
};

const u8 gTradeMessageWindowRects[][4] = {
    [MSG_STANDBY] = {8, 7, 22, 12},
    [MSG_CANCELED] = {8, 7, 22, 12},
    [MSG_ONLY_MON1] = {6, 7, 24, 12},
    {6, 7, 24, 12},
    [MSG_WAITING_FOR_FRIEND] = {8, 7, 22, 12},
    [MSG_FRIEND_WANTS_TO_TRADE] = {7, 7, 23, 12}
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
    .affineMode = ST_OAM_AFFINE_NORMAL,
    .size = SPRITE_SIZE(16x16)
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
    AFFINEANIMCMD_FRAME(Q_8_8(0.375), Q_8_8(1), 0,  0),
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
    .data = (const u8 *)sPokeball_Gfx,
    .size = sizeof(sPokeball_Gfx),
    .tag = GFXTAG_POKEBALL,
};

static const struct SpritePalette sPokeBallSpritePalette = {
    .data = sPokeball_Pal,
    .tag = PALTAG_POKEBALL,
};

static const struct SpriteTemplate sSpriteTemplate_Pokeball = {
    .tileTag = GFXTAG_POKEBALL,
    .paletteTag = PALTAG_POKEBALL,
    .oam = &sOamData_Pokeball,
    .anims = sAnims_Pokeball,
    .images = NULL,
    .affineAnims = sAffineAnims_Pokeball,
    .callback = SpriteCB_BouncingPokeball,
};

static const struct OamData sOamData_LinkMonGlow = {
    .affineMode = ST_OAM_AFFINE_NORMAL,
    .objMode = ST_OAM_OBJ_BLEND,
    .size = SPRITE_SIZE(32x32),
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
    .data = (const u8 *)sLinkMonGlow_Gfx,
    .size = sizeof(sLinkMonGlow_Gfx),
    .tag = GFXTAG_LINK_MON_GLOW,
};

static const struct SpritePalette sSpritePalette_LinkMon = {
    .data = sLinkMon_Pal,
    .tag = PALTAG_LINK_MON,
};

static const struct SpritePalette sSpritePalette_Gba = {
    .data = sGba_Pal,
    .tag = PALTAG_GBA,
};

static const struct SpriteTemplate sSpriteTemplate_LinkMonGlow = {
    .tileTag = GFXTAG_LINK_MON_GLOW,
    .paletteTag = PALTAG_LINK_MON,
    .oam = &sOamData_LinkMonGlow,
    .anims = sAnims_LinkMonGlow,
    .images = NULL,
    .affineAnims = sAffineAnims_LinkMonGlow,
    .callback = SpriteCB_LinkMonGlow,
};

static const struct OamData sOamData_LinkMonShadow = {
    .shape = SPRITE_SHAPE(16x32),
    .size = SPRITE_SIZE(16x32),
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

enum {
    ANIM_LINKMON_BIG,
    ANIM_LINKMON_SMALL,
};

static const union AnimCmd *const sAnims_LinkMonShadow[] = {
    [ANIM_LINKMON_BIG] = sAnim_LinkMonShadow_Big,
    [ANIM_LINKMON_SMALL] = sAnim_LinkMonShadow_Small
};

static const struct SpriteSheet sSpriteSheet_LinkMonShadow = {
    .data = (const u8 *)sLinkMonShadow_Gfx,
    .size = 0x300,
    .tag = GFXTAG_LINK_MON_SHADOW,
};

static const struct SpriteTemplate sSpriteTemplate_LinkMonShadow = {
    .tileTag = GFXTAG_LINK_MON_SHADOW,
    .paletteTag = PALTAG_LINK_MON,
    .oam = &sOamData_LinkMonShadow,
    .anims = sAnims_LinkMonShadow,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_LinkMonShadow,
};

static const struct OamData sOamData_CableEnd = {
    .shape = SPRITE_SHAPE(16x32),
    .size = SPRITE_SIZE(16x32),
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
    .data = (const u8 *)sCableEnd_Gfx,
    .size = 0x100,
    .tag = GFXTAG_CABLE_END,
};

static const struct SpriteTemplate sSpriteTemplate_CableEnd = {
    .tileTag = GFXTAG_CABLE_END,
    .paletteTag = PALTAG_GBA,
    .oam = &sOamData_CableEnd,
    .anims = sAnims_CableEnd,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_CableEndSending,
};

static const struct OamData sOamData_GbaScreen = {
    .shape = SPRITE_SHAPE(64x32),
    .size = SPRITE_SIZE(64x32),
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
    .data = (const u8 *)sGbaScreen_Gfx,
    .size = sizeof(sGbaScreen_Gfx),
    .tag = GFXTAG_GBA_SCREEN,
};

static const struct SpriteTemplate sSpriteTemplate_GbaScreenFlash_Long = {
    .tileTag = GFXTAG_GBA_SCREEN,
    .paletteTag = PALTAG_GBA,
    .oam = &sOamData_GbaScreen,
    .anims = sAnims_GbaScreen_Long,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_GbaScreen,
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
    AFFINEANIMCMD_FRAME(Q_8_8(-1), Q_8_8(1), 0, 0),
    AFFINEANIMCMD_JUMP(0)
};

static const union AffineAnimCmd *const sAffineAnims_CrossingMonPics[] = {
    sAffineAnim_CrossingMonPic
};

static const struct InGameTrade sIngameTrades[] = {
#if ENGLISH
    {
        .nickname = _("MAKIT"),
        .species = SPECIES_MAKUHITA,
        .ivs = {5, 5, 4, 4, 4, 4},
        .abilityNum = TRUE,
        .otId = 49562,
        .conditions = {5, 5, 5, 5, 30},
        .personality = 0x9C40,
        .heldItem = ITEM_X_ATTACK,
        .mailNum = -1,
        .otName = _("ELYSSA"),
        .otGender = MALE,
        .sheen = 10,
        .requestedSpecies = SPECIES_SLAKOTH,
    }, {
        .nickname = _("SKITIT"),
        .species = SPECIES_SKITTY,
        .ivs = {5, 4, 4, 5, 4, 4},
        .abilityNum = FALSE,
        .otId = 2259,
        .conditions = {5, 5, 30, 5, 5},
        .personality = 0x498A2E17,
        .heldItem = ITEM_GLITTER_MAIL,
        .mailNum = 0,
        .otName = _("DARRELL"),
        .otGender = FEMALE,
        .sheen = 10,
        .requestedSpecies = SPECIES_PIKACHU,
    }, {
        .nickname = _("COROSO"),
        .species = SPECIES_CORSOLA,
        .ivs = {4, 4, 5, 4, 4, 5},
        .abilityNum = TRUE,
        .otId = 50183,
        .conditions = {5, 30, 5, 5, 5},
        .personality = 0x4C970B7F,
        .heldItem = ITEM_TROPIC_MAIL,
        .mailNum = 1,
        .otName = _("LANE"),
        .otGender = FEMALE,
        .sheen = 10,
        .requestedSpecies = SPECIES_BELLOSSOM,
    }
#elif GERMAN
    {
        .nickname = _("MAKIT"),
        .species = SPECIES_MAKUHITA,
        .ivs = {5, 5, 4, 4, 4, 4},
        .abilityNum = TRUE,
        .otId = 49562,
        .conditions = {5, 5, 5, 5, 30},
        .personality = 0x9C40,
        .heldItem = ITEM_X_ATTACK,
        .mailNum = -1,
        .otName = _("MAIK"),
        .otGender = MALE,
        .sheen = 10,
        .requestedSpecies = SPECIES_SLAKOTH,
    }, {
        .nickname = _("CONEC"),
        .species = SPECIES_SKITTY,
        .ivs = {5, 4, 4, 5, 4, 4},
        .abilityNum = FALSE,
        .otId = 2259,
        .conditions = {5, 5, 30, 5, 5},
        .personality = 0x498A2E17,
        .heldItem = ITEM_GLITTER_MAIL,
        .mailNum = 0,
        .otName = _("MADINA"),
        .otGender = FEMALE,
        .sheen = 10,
        .requestedSpecies = SPECIES_PIKACHU,
    }, {
        .nickname = _("CORASO"),
        .species = SPECIES_CORSOLA,
        .ivs = {4, 4, 5, 4, 4, 5},
        .abilityNum = TRUE,
        .otId = 50183,
        .conditions = {5, 30, 5, 5, 5},
        .personality = 0x4C970B7F,
        .heldItem = ITEM_TROPIC_MAIL,
        .mailNum = 1,
        .otName = _("LIANA"),
        .otGender = FEMALE,
        .sheen = 10,
        .requestedSpecies = SPECIES_BELLOSSOM,
    }
#endif
};

static const u16 sIngameTradeMail[][MAIL_WORDS_COUNT + 1] = {
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
        EC_EMPTY_WORD,
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
