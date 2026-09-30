#include "global.h"
#include "international_string_util.h"
#include "string_util.h"
#include "text.h"
#include "main.h"
#include "constants/species.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/easy_chat.h"
#include "link.h"
#include "strings2.h"
#include "graphics.h"
#include "palette.h"
#include "task.h"
#include "menu.h"
#include "text_window.h"
#include "pokemon_icon.h"
#include "cable_club.h"
#include "party_menu.h"
#include "constants/songs.h"
#include "sound.h"
#include "data2.h"
#include "pokemon_summary_screen.h"
#include "overworld.h"
#include "battle_anim.h"
#include "daycare.h"
#include "event_data.h"
#include "strings.h"
#include "load_save.h"
#include "save.h"
#include "script.h"
#include "field_fadetransition.h"
#include "decompress.h"
#include "mail.h"
#include "evolution_scene.h"
#include "pokeball.h"
#include "pokedex.h"
#include "field_effect.h"
#include "util.h"
#include "battle_interface.h"
#include "trade.h"
#include "ewram.h"

#ifndef NONMATCHING
asm(".include \"constants/gba_constants.inc\"");
#endif

#ifdef ENGLISH
#define CopyTradeTilemapToBufferAndRequestUpload CopyTradeTilemapToBuffer
asm(".set CopyTradeTilemapToBufferAndRequestUpload, CopyTradeTilemapToBuffer");
#endif

#define Trade_SendData(ptr) (SendBlock(BitmaskAllOtherLinkPlayers(), ptr->linkData, 20))

#define GFXTAG_MENU_TEXT 200
#define GFXTAG_CURSOR 300
#define GFXTAG_LINK_MON_GLOW 5550
#define GFXTAG_LINK_MON_SHADOW 5552
#define GFXTAG_CABLE_END 5554
#define GFXTAG_GBA_SCREEN 5556
#define GFXTAG_POKEBALL 5557
#define PALTAG_CURSOR 2345
#define PALTAG_MENU_TEXT 4925
#define PALTAG_LINK_MON 5551
#define PALTAG_GBA 5555
#define PALTAG_POKEBALL 5558

enum {
    CURSOR_ANIM_NORMAL,
    CURSOR_ANIM_ON_CANCEL,
};

enum {
    POKEBALL_ANIM_SPIN_ONCE,
    POKEBALL_ANIM_SPIN_TWICE,
};

enum {
    POKEBALL_AFFINE_ANIM_NORMAL,
    POKEBALL_AFFINE_ANIM_SQUISH,
    POKEBALL_AFFINE_ANIM_UNSQUISH,
};

enum {
    ANIM_LINKMON_BIG,
    ANIM_LINKMON_SMALL,
};

struct InGameTrade {
    /*0x00*/ u8 nickname[11];
    /*0x0C*/ u16 species;
    /*0x0E*/ u8 ivs[6];
    /*0x14*/ bool8 abilityNum;
    /*0x18*/ u32 otId;
    /*0x1C*/ u8 conditions[5];
    /*0x24*/ u32 personality;
    /*0x28*/ u16 heldItem;
    /*0x2A*/ u8 mailNum;
    /*0x2B*/ u8 otName[11];
    /*0x36*/ u8 otGender;
    /*0x37*/ u8 sheen;
    /*0x38*/ u16 requestedSpecies;
};

struct UnkStructC {
    /*0x00*/ u16 words[9];
    /*0x10*/ u8 string[8];
    /*0x1A*/ u8 otId[4];
    /*0x1E*/ u16 species;
    /*0x20*/ u16 heldItem;
};

struct TradeTilemapBuffer {
    /*0x00*/ u8 filler_00[8];
    /*0x08*/ void *vramDest;
    /*0x0c*/ u8 filler_0c[4];
    /*0x10*/ u8 needsUpload;
    /*0x12*/ u16 tilemap[0x400];
};

struct TradeQueuedAction {
    /*0x00*/ u8 active;
    /*0x02*/ u16 delay;
    /*0x04*/ u8 actionId;
};

struct TradeMenuState {
    /*0x0000*/ u8 bg2hofs;
    /*0x0001*/ u8 bg3hofs;
    /*0x0004*/ struct Window window;
    /*0x0034*/ u8 partySpriteIds[2][6];
    /*0x0040*/ u8 cursorSpriteId;
    /*0x0041*/ u8 cursorPosition;
    /*0x0042*/ u8 partyCounts[2];
    /*0x0044*/ u8 optionsActive[13];
    /*0x0051*/ u8 isLiveMon[2][6];
    /*0x005d*/ u8 isEgg[2][6];
    /*0x0069*/ u8 hpBarLevels[2][6];
    /*0x0075*/ u8 bufferPartyState;
    /*0x0076*/ u8 filler_0076[4];
    /*0x007a*/ u8 textWindowBaseTileNum;
    /*0x007b*/ u8 callbackId;
    /*0x007c*/ u8 unk_007c;
    /*0x007d*/ u8 unk_007d;
    /*0x007e*/ u16 bottomTextTileStart;
    /*0x0080*/ u8 drawSelectedMonState[2];
    /*0x0082*/ u8 selectedMonIdx[2];
    /*0x0084*/ u8 playerSelectStatus;
    /*0x0085*/ u8 partnerSelectStatus;
    /*0x0086*/ u8 playerConfirmStatus;
    /*0x0087*/ u8 partnerConfirmStatus;
    /*0x0088*/ u8 filler_0088[2];
    /*0x008a*/ u8 partnerCursorPosition;
    /*0x008c*/ u16 linkData[20];
    /*0x00b4*/ u8 timer;
    /*0x00b5*/ u8 giftRibbons[GIFT_RIBBONS_COUNT];
    /*0x00c0*/ u8 filler_00c0[8];
    /*0x00c8*/ struct TradeTilemapBuffer tilemapBuffer;
    /*0x08dc*/ struct TradeQueuedAction queuedActions[4];
    /*0x08fc*/ u8 filler_08fc[0x704];
};

struct TradeAnimState *gTradeAnim;

static u8 gUnknown_03000508[8];

struct UnkStructF {
    u8 filler_0000[9];
    u8 unk_0009;
    u8 filler_000a[0xff6];
};

struct TradeEwramStruct {
    /*0x00000*/ u8 filler_00000[0x7000];
    /*0x07000*/ struct TradeMenuState tradeMenu;
    /*0x08000*/ struct UnkStructF unk_08000;
    /*0x09000*/ u8 filler_09000[0x4000];
    /*0x0d000*/ u8 tileBuffers[13][256];
    /*0x0dd00*/ u8 filler_0dd00[0x1300];
    /*0x0f000*/ struct TradeAnimState tradeAnim;
};

static void CB2_CreateTradeMenu(void);
static void DrawTextWindowAndBufferTiles(const struct WindowTemplate *, u8 *, const u8 *, u8);
static void DrawBottomRowText(const u8 *, u8 *, u8);
static void nullsub_5(u8, u8);
static void InitTradeMenuActionQueue(void);
static void InitTradeTilemapBuffer(struct TradeTilemapBuffer *, void *);
static void VBlankCB_TradeMenu(void);
static void PrintTradeMessage(u8);
static bool8 BufferTradeParties(void);
static void SaveTradeGiftRibbons(void);
static bool8 LoadUISpriteGfx(void);
static void ComputePartyTradeableFlags(u8);
static void PrintPartyNicknames(u8);
static void SetActiveMenuOptions(void);
static void LoadTradeBgGfx(u8);
static void ComputePartyHPBarLevels(u8);
static void SetTradePartyHPBarSprites(void);
static void CB1_UpdateLink(void);
static void CB2_TradeMenu(void);
static void UploadTradeTilemapIfNeeded(struct TradeTilemapBuffer *);
void CB2_LinkTrade(void);
static void RunTradeMenuCallback(void);
static void DoQueuedActions(void);
static void DrawSelectedMonScreen(u8);
static void PrintPartyLevelsAndGenders(u8);
static void MarkTradeTilemapForUpload(struct TradeTilemapBuffer *);
static void Task_DrawSelectionSummary(u8);
static void Task_DrawSelectionTrade(u8);
static void SetSelectedMon(u8);
static void QueueAction(u16, u8);
static void PrintTradePartnerPartyNicknames(void);
static u8 CountOtherUsableMons(const u8 *, u8, u8);
static void RedrawPartyWindow(u8);
u8 GetMonNicknameWidth(u8 *, u8, u8);
static void CopyTradeTilemapToBufferAndRequestUpload(struct TradeTilemapBuffer *, u8, u8, const u16 *, u8, u8, u16);
static void CopyTradeTilemapToBuffer(struct TradeTilemapBuffer *, u8, u8, const u16 *, u8, u8, u16);
void BufferMovesString(u8 *, u8, u8);
void PrintLevelAndGender(u8, u8, u8, u8, u8, u8);
static void SpriteCB_BouncingPokeballDepartEnd(struct Sprite *);
static bool8 DoTradeAnim_Cable(void);
static void CB2_WaitTradeComplete(void);
static u8 TradeGetMultiplayerId(void);
static void CB2_ReturnToTradeMenuAfterLinkTrade(void);
static void Task_InGameTrade(u8);
static void GetInGameTradeMail(struct Mail *, const struct InGameTrade *);
static void CB2_SaveAndEndTrade(void);
static void SpriteCB_BouncingPokeball(struct Sprite *);
static void SpriteCB_BouncingPokeballDepart(struct Sprite *);
static void SetTradeSequenceBgGpuRegs(u8);
static void SpriteCB_LinkMonGlow(struct Sprite *);
static void SpriteCB_LinkMonShadow(struct Sprite *);
static void SpriteCB_CableEndSending(struct Sprite *);
static void SpriteCB_GbaScreen(struct Sprite *);
static void SpriteCB_BouncingPokeballArrive(struct Sprite *);
static void CheckPartnersMonForRibbons(void);
static void CB2_InGameTrade(void);
static void BufferInGameTradeMonName(void);
static void LoadTradeSequenceSpriteSheetsAndPalettes(void);
static void BufferTradeSceneStrings(void);
void CB2_UpdateLinkTrade(void);

EWRAM_DATA u8 *sMenuTextTileBuffers[13] = {0};
EWRAM_DATA struct Mail gTradeMail[PARTY_SIZE] = {0};
EWRAM_DATA u8 gSelectedTradeMonPositions[2] = {0};

struct TradeMenuState *sTradeMenu;

const u32 unref_data_820ABD4[] =
{
    0x00000890,
    0x00003AC0,
    0x0000001C,
    0x00000530,
    0x00000024,
    0x00000064,
    0x000004D8
};

const u16 gTradeMovesBoxTilemap[] = INCBIN_U16("graphics/trade/moves_box_map.bin");
const u16 gTradePartyBoxTilemap[] = INCBIN_U16("graphics/trade/party_box_map.bin");
const u16 gTradeStripesBG2Tilemap[] = INCBIN_U16("graphics/trade/stripes_bg2_map.bin");
const u16 gTradeStripesBG3Tilemap[] = INCBIN_U16("graphics/trade/stripes_bg3_map.bin");

const struct OamData sOamData_MenuText = {
    .shape = ST_OAM_H_RECTANGLE,
    .size = 2,
    .priority = 1
};

const union AnimCmd sAnim_MenuText_0[] = {
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END
};

const union AnimCmd sAnim_MenuText_1[] = {
    ANIMCMD_FRAME(8, 5),
    ANIMCMD_END
};

const union AnimCmd sAnim_MenuText_2[] = {
    ANIMCMD_FRAME(16, 5),
    ANIMCMD_END
};

const union AnimCmd sAnim_MenuText_3[] = {
    ANIMCMD_FRAME(24, 5),
    ANIMCMD_END
};

const union AnimCmd sAnim_MenuText_4[] = {
    ANIMCMD_FRAME(32, 5),
    ANIMCMD_END
};

const union AnimCmd sAnim_MenuText_5[] = {
    ANIMCMD_FRAME(40, 5),
    ANIMCMD_END
};

const union AnimCmd *const sAnims_MenuText[] = {
    sAnim_MenuText_0,
    sAnim_MenuText_1,
    sAnim_MenuText_2,
    sAnim_MenuText_3,
    sAnim_MenuText_4,
    sAnim_MenuText_5
};

const u16 sMenuText_Pal[] = INCBIN_U16("graphics/trade/text1.gbapal");
const u16 sUnusedMenuTextPalette[] = INCBIN_U16("graphics/trade/text2.gbapal");

const struct SpriteSheet sMenuTextSpriteSheets[] = {
    {ewram_2010000.tileBuffers[ 0], 256, GFXTAG_MENU_TEXT +  0},
    {ewram_2010000.tileBuffers[ 1], 256, GFXTAG_MENU_TEXT +  1},
    {ewram_2010000.tileBuffers[ 2], 256, GFXTAG_MENU_TEXT +  2},
    {ewram_2010000.tileBuffers[ 3], 256, GFXTAG_MENU_TEXT +  3},
    {ewram_2010000.tileBuffers[ 4], 256, GFXTAG_MENU_TEXT +  4},
    {ewram_2010000.tileBuffers[ 5], 256, GFXTAG_MENU_TEXT +  5},
    {ewram_2010000.tileBuffers[ 6], 256, GFXTAG_MENU_TEXT +  6},
    {ewram_2010000.tileBuffers[ 7], 256, GFXTAG_MENU_TEXT +  7},
    {ewram_2010000.tileBuffers[ 8], 256, GFXTAG_MENU_TEXT +  8},
    {ewram_2010000.tileBuffers[ 9], 256, GFXTAG_MENU_TEXT +  9},
    {ewram_2010000.tileBuffers[10], 256, GFXTAG_MENU_TEXT + 10},
    {ewram_2010000.tileBuffers[11], 256, GFXTAG_MENU_TEXT + 11},
    {ewram_2010000.tileBuffers[12], 256, GFXTAG_MENU_TEXT + 12},
};

const struct SpritePalette sSpritePalette_MenuText = {
    sMenuText_Pal, PALTAG_MENU_TEXT
};

const struct SpriteTemplate sSpriteTemplate_MenuText = {
    GFXTAG_MENU_TEXT,
    PALTAG_MENU_TEXT,
    &sOamData_MenuText,
    sAnims_MenuText,
    NULL,
    gDummySpriteAffineAnimTable,
    SpriteCallbackDummy
};

const struct OamData sOamData_Cursor = {
    .shape = ST_OAM_H_RECTANGLE,
    .size = 3,
    .priority = 1
};

const union AnimCmd sAnim_Cursor_Normal[] = {
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END
};

const union AnimCmd sAnim_Cursor_OnCancel[] = {
    ANIMCMD_FRAME(32, 5),
    ANIMCMD_END
};

const union AnimCmd *const sAnims_Cursor[] = {
    sAnim_Cursor_Normal,
    sAnim_Cursor_OnCancel
};

const struct SpriteSheet sCursorSpriteSheet = {
    gTradeCursor_Gfx, 0x800, GFXTAG_CURSOR
};

const struct SpritePalette sCursorSpritePalette = {
    gTradeCursor_Pal, PALTAG_CURSOR
};

const struct SpriteTemplate sSpriteTemplate_Cursor = {
    GFXTAG_CURSOR,
    PALTAG_CURSOR,
    &sOamData_Cursor,
    sAnims_Cursor,
    NULL,
    gDummySpriteAffineAnimTable,
    SpriteCallbackDummy
};

const u8 *const sActionTexts[] = {
    TradeText_Cancel,
    TradeText_ChoosePoke,
    TradeText_Summary1,
    TradeText_Trade1,
    TradeText_CancelTradePrompt,
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

const u8 gTradeNextSelectedMonTable[][4][6] = {
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

const u8 gTradeMonSpriteCoords[][2] = {
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

const u8 gTradeLevelDisplayCoords[][2] = {
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

const u8 gTradeMonBoxCoords[][2] = {
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

const u8 gTradeMenuNameSpriteCoords[][2][2] = {
    {
        { 59, 10},
        {179, 10},
    },
    {
        { 59, 10},
        {179, 10}
    }
};

const u8 *const sTradeMessages[] = {
    TradeText_LinkStandby,
    TradeText_TradeCancelled,
    TradeText_OnlyPoke,
    TradeText_NonTradablePoke,
    TradeText_WaitingForFriend,
    TradeText_WantToTrade
};

const u8 gTradeMessageWindowRects[][4] = {
    {8, 7, 22, 12},
    {8, 7, 22, 12},
    {6, 7, 24, 12},
    {6, 7, 24, 12},
    {8, 7, 22, 12},
    {7, 7, 23, 12}
};

const struct MenuAction2 sSelectTradeMonActions[] = {
    {TradeText_Summary2, Task_DrawSelectionSummary},
    {TradeText_Trade2, Task_DrawSelectionTrade}
};

const u8 sTradePartyWindowColumnBounds[][2] = {
    { 0, 14},
    {15, 29}
};

const u8 sSelectedMonTextCoords[][2] = {
    { 3,  5},
    { 3,  7},
    {18,  5},
    {18,  7}
};

const u8 sText_EmptyString[] = _("");
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

const u8 sText_ShedinjaJP[] = _("ヌケニン");
const u8 unref_string_0820C3B5[] = _("こうかんせいりつ     ");
const u8 unref_string_0820C3C3[] = _("だめだたらしいよ     ");
const u8 sSelectedMonLevelGenderCoords[][2] = {
    { 4,  3},
    {19,  3},
    { 0,  0}
};

const u16 sPokeball_Pal[] = INCBIN_U16("graphics/trade/ball.gbapal");
const u32 sPokeball_Gfx[] = INCBIN_U32("graphics/trade/ball.4bpp");
const u16 gTradeGba2_Pal[][16] = {
    INCBIN_U16("graphics/trade/unknown.gbapal"),
    INCBIN_U16("graphics/trade/gba.gbapal"),
    INCBIN_U16("graphics/trade/shadow.gbapal"),
    {},
    INCBIN_U16("graphics/trade/misc.gbapal")
};

const u32 gTradeGba_Gfx[] = INCBIN_U32("graphics/trade/gba.4bpp");
const u32 gUnknown_0820CA98_2[] = INCBIN_U32("graphics/trade/shadow.4bpp");
const u32 sPokeballSymbol_Gfx[] = INCBIN_U32("graphics/trade/pokeball_symbol.8bpp");
const u16 gTradePlatform_Tilemap[] = INCBIN_U16("graphics/trade/shadow_map.bin");
const u16 sGbaMapCable[] = INCBIN_U16("graphics/trade/gba_map.bin");
const u16 sCableCloseup_Map[] = INCBIN_U16("graphics/trade/cable_closeup_map.bin");
const u16 sPokeballSymbol_Map[] = INCBIN_U16("graphics/trade/pokeball_symbol_map.bin");
const u16 unused_08212098[] = INCBIN_U16("graphics/unused/unknown/8212098.gbapal");
const u16 sCableEnd_Pal[] = INCBIN_U16("graphics/trade/cable_end.gbapal");
const u16 unused_082120D8[] = INCBIN_U16("graphics/unused/unknown/82120D8.gbapal");
const u16 nullpal_082120F8[16] = {};
const u16 sLinkMon_Pal[] = INCBIN_U16("graphics/trade/glow.gbapal");
const u32 sLinkMonGlow_Gfx[] = INCBIN_U32("graphics/trade/glow1.4bpp");
const u32 sLinkMonShadow_Gfx[] = INCBIN_U32("graphics/trade/glow2.4bpp");
const u32 sCableEnd_Gfx[] = INCBIN_U32("graphics/trade/cable_end.4bpp");
const u32 sGbaScreen_Gfx[] = INCBIN_U32("graphics/trade/gba_screen.4bpp");
const u32 sGbaAffine_Gfx[] = INCBIN_U32("graphics/trade/gba_affine.8bpp");
const u16 sGbaAffineMapCable[] = INCBIN_U16("graphics/trade/gba_affine_map.bin");

const struct OamData sOamData_Pokeball = {
    .affineMode = 1,
    .size = 1
};

const union AnimCmd sAnim_Pokeball_SpinOnce[] = {
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

const union AnimCmd sAnim_Pokeball_SpinTwice[] = {
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

const union AnimCmd *const sAnims_Pokeball[] = {
    sAnim_Pokeball_SpinOnce,
    sAnim_Pokeball_SpinTwice
};

const union AffineAnimCmd sAffineAnim_Pokeball_Normal[] = {
    AFFINEANIMCMD_FRAME(0, 0, 0, 1),
    AFFINEANIMCMD_END
};

const union AffineAnimCmd sAffineAnim_Pokeball_Squish[] = {
    AFFINEANIMCMD_FRAME(-8, 0, 0, 20),
    AFFINEANIMCMD_END
};

const union AffineAnimCmd sAffineAnim_Pokeball_Unsquish[] = {
    AFFINEANIMCMD_FRAME(0x60, 0x100, 0,  0),
    AFFINEANIMCMD_FRAME(   0,     0, 0,  5),
    AFFINEANIMCMD_FRAME(   8,     0, 0, 20),
    AFFINEANIMCMD_END
};

const union AffineAnimCmd *const sAffineAnims_Pokeball[] = {
    sAffineAnim_Pokeball_Normal,
    sAffineAnim_Pokeball_Squish,
    sAffineAnim_Pokeball_Unsquish
};

const struct SpriteSheet sPokeBallSpriteSheet = {
    (const u8 *)sPokeball_Gfx, 0x600, GFXTAG_POKEBALL
};

const struct SpritePalette sPokeBallSpritePalette = {
    sPokeball_Pal, PALTAG_POKEBALL
};

const struct SpriteTemplate sSpriteTemplate_Pokeball = {
    GFXTAG_POKEBALL,
    PALTAG_POKEBALL,
    &sOamData_Pokeball,
    sAnims_Pokeball,
    NULL,
    sAffineAnims_Pokeball,
    SpriteCB_BouncingPokeball
};

const struct OamData sOamData_LinkMonGlow = {
    .affineMode = 1,
    .objMode = 1,
    .size = 2,
    .priority = 1
};

const union AnimCmd sAnim_LinkMonGlow[] = {
    ANIMCMD_FRAME(0, 5, .hFlip = TRUE, .vFlip = TRUE),
    ANIMCMD_END
};

const union AnimCmd *const sAnims_LinkMonGlow[] = {
    sAnim_LinkMonGlow
};

const union AffineAnimCmd sAffineAnim_LinkMonGlow[] = {
    AFFINEANIMCMD_FRAME(-10, -10, 0, 5),
    AFFINEANIMCMD_FRAME(10, 10, 0, 5),
    AFFINEANIMCMD_JUMP(0)
};

const union AffineAnimCmd *const sAffineAnims_LinkMonGlow[] = {
    sAffineAnim_LinkMonGlow
};

const struct SpriteSheet sSpriteSheet_LinkMonGlow = {
    (const u8 *)sLinkMonGlow_Gfx, 0x200, GFXTAG_LINK_MON_GLOW
};

const struct SpritePalette sSpritePalette_LinkMon = {
    sLinkMon_Pal, PALTAG_LINK_MON
};

const struct SpritePalette sSpritePalette_Gba = {
    sCableEnd_Pal, PALTAG_GBA
};

const struct SpriteTemplate sSpriteTemplate_LinkMonGlow = {
    GFXTAG_LINK_MON_GLOW,
    PALTAG_LINK_MON,
    &sOamData_LinkMonGlow,
    sAnims_LinkMonGlow,
    NULL,
    sAffineAnims_LinkMonGlow,
    SpriteCB_LinkMonGlow
};

const struct OamData sOamData_LinkMonShadow = {
    .shape = ST_OAM_V_RECTANGLE,
    .size = 2,
    .priority = 1
};

const union AnimCmd sAnim_LinkMonShadow_Big[] = {
    ANIMCMD_FRAME(0, 5, .vFlip = TRUE, .hFlip = TRUE),
    ANIMCMD_END
};

const union AnimCmd sAnim_LinkMonShadow_Small[] = {
    ANIMCMD_FRAME(8, 5, .vFlip = TRUE, .hFlip = TRUE),
    ANIMCMD_END
};

const union AnimCmd *const sAnims_LinkMonShadow[] = {
    sAnim_LinkMonShadow_Big,
    sAnim_LinkMonShadow_Small
};

const struct SpriteSheet sSpriteSheet_LinkMonShadow = {
    (const u8 *)sLinkMonShadow_Gfx, 0x300, GFXTAG_LINK_MON_SHADOW
};

const struct SpriteTemplate sSpriteTemplate_LinkMonShadow = {
    GFXTAG_LINK_MON_SHADOW,
    PALTAG_LINK_MON,
    &sOamData_LinkMonShadow,
    sAnims_LinkMonShadow,
    NULL,
    gDummySpriteAffineAnimTable,
    SpriteCB_LinkMonShadow
};

const struct OamData sOamData_CableEnd = {
    .shape = ST_OAM_V_RECTANGLE,
    .size = 2,
    .priority = 1
};

const union AnimCmd sAnim_CableEnd[] = {
    ANIMCMD_FRAME(0, 10),
    ANIMCMD_END
};

const union AnimCmd *const sAnims_CableEnd[] = {
    sAnim_CableEnd
};

const struct SpriteSheet sSpriteSheet_CableEnd = {
    (const u8 *)sCableEnd_Gfx, 0x100, GFXTAG_CABLE_END
};

const struct SpriteTemplate sSpriteTemplate_CableEnd = {
    GFXTAG_CABLE_END,
    PALTAG_GBA,
    &sOamData_CableEnd,
    sAnims_CableEnd,
    NULL,
    gDummySpriteAffineAnimTable,
    SpriteCB_CableEndSending
};

const struct OamData sOamData_GbaScreen = {
    .shape = ST_OAM_H_RECTANGLE,
    .size = 3,
    .priority = 1
};

const union AnimCmd sAnim_GbaScreen_Long[] = {
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

const union AnimCmd *const sAnims_GbaScreen_Long[] = {
    sAnim_GbaScreen_Long
};

const struct SpriteSheet sSpriteSheet_GbaScreen = {
    (const u8 *)sGbaScreen_Gfx, 0x1000, GFXTAG_GBA_SCREEN
};

const struct SpriteTemplate sSpriteTemplate_GbaScreenFlash_Long = {
    GFXTAG_GBA_SCREEN,
    PALTAG_GBA,
    &sOamData_GbaScreen,
    sAnims_GbaScreen_Long,
    NULL,
    gDummySpriteAffineAnimTable,
    SpriteCB_GbaScreen
};

const u16 sLinkMonShadow_Pal[] = {
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

const union AffineAnimCmd sAffineAnim_CrossingMonPic[] = {
    AFFINEANIMCMD_FRAME(-0x100, 0x100, 0, 0),
    AFFINEANIMCMD_JUMP(0)
};

const union AffineAnimCmd *const sAffineAnims_CrossingMonPics[] = {
    sAffineAnim_CrossingMonPic
};

const struct InGameTrade sIngameTrades[] = {
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

const u16 sIngameTradeMail[][10] = {
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

const s8 sTradeBallVerticalVelocityTable[] = {
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

// .text

void CB2_StartCreateTradeMenu(void)
{
    SetMainCallback2(CB2_CreateTradeMenu);
}

static void DrawTradeMenuText(void)
{
    u8 playerId;
    DrawTextWindowAndBufferTiles(&gTradeMenuTextWindowTemplate, sMenuTextTileBuffers[0], gSaveBlock2.playerName, 0xC);
    playerId = GetMultiplayerId();
    DrawTextWindowAndBufferTiles(&gTradeMenuTextWindowTemplate, sMenuTextTileBuffers[3], gLinkPlayers[playerId ^ 1].name, 0xC);
    DrawTextWindowAndBufferTiles(&gTradeMenuTextWindowTemplate, sMenuTextTileBuffers[6], sActionTexts[0], 0x8);
    DrawBottomRowText(sActionTexts[1], sMenuTextTileBuffers[8], 0x14);
    nullsub_5(3, 0);
}

static void CreateTradeMenuPlayerNameSprites(void)
{
    struct SpriteTemplate spriteTemplate;
    int i;
    u8 playerId;
    u16 nameLength;
    int nameLengthLayout;

    nameLength = StringLength(gSaveBlock2.playerName);
    nameLengthLayout = nameLength <= 5 ? 0 : 1;
    for (i = 0; i < 3; i ++)
    {
        spriteTemplate = sSpriteTemplate_MenuText;
        spriteTemplate.tileTag += i;
        CreateSprite(&spriteTemplate, gTradeMenuNameSpriteCoords[nameLengthLayout][0][0] + 32 * i, gTradeMenuNameSpriteCoords[nameLengthLayout][0][1], 1);
    }

    playerId = GetMultiplayerId();
    nameLength = StringLength(gLinkPlayers[playerId ^ 1].name);
    nameLengthLayout = nameLength <= 5 ? 0 : 1;
    for (i = 0; i < 3; i ++)
    {
        spriteTemplate = sSpriteTemplate_MenuText;
        spriteTemplate.tileTag += i + 3;
        CreateSprite(&spriteTemplate, gTradeMenuNameSpriteCoords[nameLengthLayout][1][0] + 32 * i, gTradeMenuNameSpriteCoords[nameLengthLayout][1][1], 1);
    }
    nullsub_5(5, 0);
}

static void CreateTradeMenuActionTextSprites(void)
{
    struct SpriteTemplate spriteTemplate;
    int i;

    for (i = 0; i < 2; i ++)
    {
        spriteTemplate = sSpriteTemplate_MenuText;
        spriteTemplate.tileTag += i + 6;
        CreateSprite(&spriteTemplate, 0xd6 + 32 * i, 0x98, 1);
    }

    for (i = 0; i < 5; i ++)
    {
        spriteTemplate = sSpriteTemplate_MenuText;
        spriteTemplate.tileTag += i + 8;
        CreateSprite(&spriteTemplate, 0x18 + 32 * i, 0x96, 1);
    }
}

static void CB2_CreateTradeMenu(void)
{
    int i;

    switch (gMain.state)
    {
        case  0:
            sTradeMenu = &ewram_2010000.tradeMenu;
            InitTradeMenuActionQueue();
            ResetSpriteData();
            FreeAllSpritePalettes();
            ResetTasks();
            InitTradeTilemapBuffer(&sTradeMenu->tilemapBuffer, BG_SCREEN_ADDR(5));
            SetVBlankCallback(VBlankCB_TradeMenu);
            InitMenuWindow(&gMenuTextWindowTemplate);
            Text_LoadWindowTemplate(&gBattleMessageWindowTemplate);
            Text_InitWindowWithTemplate(&sTradeMenu->window, &gBattleMessageWindowTemplate);
            sTradeMenu->textWindowBaseTileNum = TextWindow_SetBaseTileNum(20);
            TextWindow_LoadStdFrameGraphics(&sTradeMenu->window);
            Menu_EraseScreen();
            LoadMonIconPalettes();
            sTradeMenu->bufferPartyState = 0;
            sTradeMenu->callbackId = 0;
            sTradeMenu->unk_007c = 0;
            sTradeMenu->drawSelectedMonState[0] = 0;
            sTradeMenu->drawSelectedMonState[1] = 0;
            sTradeMenu->playerConfirmStatus = 0;
            sTradeMenu->partnerConfirmStatus = 0;
            sTradeMenu->timer = 0;
            gUnknown_03000508[0] = 0;
            gMain.state ++;
            PrintTradeMessage(0);
            CpuFill16(0, ewram_2010000.tileBuffers, sizeof(ewram_2010000.tileBuffers));
            for (i = 0; i < 13; i ++)
                sMenuTextTileBuffers[i] = ewram_2010000.tileBuffers[i];
            break;
        case  1:
            gLinkType = LINKTYPE_TRADE_CONNECTING;
            OpenLink();
            for (i = 0; i < PARTY_SIZE; i ++)
                CreateMon(&gEnemyParty[i], 0, 0, 0x20, FALSE, 0, FALSE, 0);
            gMain.state ++;
            sTradeMenu->timer = 0;
            CreateTask(Task_WaitForLinkPlayerConnection, 1);
            break;
        case  2:
            sTradeMenu->timer ++;
            if (sTradeMenu->timer > 11)
            {
                sTradeMenu->timer = 0;
                gMain.state ++;
            }
            break;
        case  3:
            if (GetLinkPlayerCount_2() >= GetSavedPlayerCount())
            {
                if (IsLinkMaster())
                {
                    if (++sTradeMenu->timer > 30)
                    {
                        CheckShouldAdvanceLinkState();
                        gMain.state ++;
                    }
                }
                else
                    gMain.state ++;
            }
            break;
        case  4:
            if (gReceivedRemoteLinkPlayers == 1 && IsLinkPlayerDataExchangeComplete() == TRUE)
            {
                CalculatePlayerPartyCount();
                gMain.state ++;
            }
            break;
        case  5:
            if (BufferTradeParties())
            {
                SaveTradeGiftRibbons();
                gMain.state ++;
            }
            break;
        case  6:
            CalculateEnemyPartyCount();
            Text_FillWindowRectDefPalette(&sTradeMenu->window, 0, 0, 0, 29, 19);
            REG_DISPCNT = 0;
            sTradeMenu->partyCounts[0] = gPlayerPartyCount;
            sTradeMenu->partyCounts[1] = gEnemyPartyCount;
            for (i = 0; i < sTradeMenu->partyCounts[0]; i ++)
                sTradeMenu->partySpriteIds[0][i] = CreateMonIcon(GetMonData(&gPlayerParty[i], MON_DATA_SPECIES2), SpriteCB_MonIcon, gTradeMonSpriteCoords[i][0] * 8 + 14, gTradeMonSpriteCoords[i][1] * 8 - 12, TRUE, GetMonData(&gPlayerParty[i], MON_DATA_PERSONALITY));
            for (i = 0; i < sTradeMenu->partyCounts[1]; i ++)
                sTradeMenu->partySpriteIds[1][i] = CreateMonIcon(GetMonData(&gEnemyParty[i], MON_DATA_SPECIES2, NULL), SpriteCB_MonIcon, gTradeMonSpriteCoords[6 + i][0] * 8 + 14, gTradeMonSpriteCoords[6 + i][1] * 8 - 12, TRUE, GetMonData(&gEnemyParty[i], MON_DATA_PERSONALITY));
            nullsub_5(2, 0);
            gMain.state ++;
            break;
        case  7:
            LoadHeldItemIconGraphics();
            CreateHeldItemIcons(&sTradeMenu->partyCounts[0], sTradeMenu->partySpriteIds[0], 0);
            gMain.state ++;
            break;
        case  8:
            CreateHeldItemIcons(&sTradeMenu->partyCounts[0], sTradeMenu->partySpriteIds[0], 1);
            gMain.state ++;
            break;
        case  9:
            DrawTradeMenuText();
            gMain.state ++;
            sTradeMenu->timer = 0;
            break;
        case 10:
            nullsub_5(4, 0);
            if (LoadUISpriteGfx())
                gMain.state ++;
            break;
        case 11:
            CreateTradeMenuPlayerNameSprites();
            gMain.state ++;
            break;
        case 12:
            CreateTradeMenuActionTextSprites();
            sTradeMenu->cursorSpriteId = CreateSprite(&sSpriteTemplate_Cursor, gTradeMonSpriteCoords[0][0] * 8 + 32, gTradeMonSpriteCoords[0][1] * 8, 2);
            sTradeMenu->cursorPosition = 0;
            gMain.state ++;
            nullsub_5(6, 0);
            break;
        case 13:
            ComputePartyTradeableFlags(0);
            PrintPartyNicknames(0);
            sTradeMenu->bg2hofs = 0;
            sTradeMenu->bg3hofs = 0;
            SetActiveMenuOptions();
            gMain.state ++;
            nullsub_5(7, 0);
            PlayBGM(MUS_SCHOOL);
            break;
        case 14:
            ComputePartyTradeableFlags(1);
            PrintPartyNicknames(1);
            gMain.state ++;
            // fallthrough
        case 15:
            LoadTradeBgGfx(0);
            gMain.state ++;
            break;
        case 16:
            LoadTradeBgGfx(1);
            gMain.state ++;
            break;
        case 17:
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
            gMain.state ++;
            break;
        case 18:
            REG_DISPCNT = DISPCNT_OBJ_1D_MAP | DISPCNT_BG_ALL_ON | DISPCNT_OBJ_ON;
            gMain.state ++;
            break;
        case 19:
            ComputePartyHPBarLevels(0);
            gMain.state ++;
            break;
        case 20:
            ComputePartyHPBarLevels(1);
            SetTradePartyHPBarSprites();
            gMain.state ++;
            break;
        case 21:
            if (!gPaletteFade.active)
            {
                gMain.callback1 = CB1_UpdateLink;
                SetMainCallback2(CB2_TradeMenu);
                gUnknown_03000508[0] = 0;
            }
            break;
    }
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void CB2_ReturnToTradeMenu(void)
{
    int i;
    struct UnkStructF *unkStructF;

    switch (gMain.state)
    {
        case  0:
            sTradeMenu = &ewram_2010000.tradeMenu;
            ResetSpriteData();
            FreeAllSpritePalettes();
            ResetTasks();
            InitTradeTilemapBuffer(&sTradeMenu->tilemapBuffer, BG_SCREEN_ADDR(5));
            SetVBlankCallback(VBlankCB_TradeMenu);
            InitMenuWindow(&gMenuTextWindowTemplate);
            Text_LoadWindowTemplate(&gBattleMessageWindowTemplate);
            Text_InitWindowWithTemplate(&sTradeMenu->window, &gBattleMessageWindowTemplate);
            sTradeMenu->textWindowBaseTileNum = TextWindow_SetBaseTileNum(20);
            TextWindow_LoadStdFrameGraphics(&sTradeMenu->window);
            Menu_EraseScreen();
            LoadMonIconPalettes();
            sTradeMenu->bufferPartyState = 0;
            sTradeMenu->callbackId = 0;
            sTradeMenu->unk_007c = 0;
            sTradeMenu->drawSelectedMonState[0] = 0;
            sTradeMenu->drawSelectedMonState[1] = 0;
            sTradeMenu->playerConfirmStatus = 0;
            sTradeMenu->partnerConfirmStatus = 0;
            sTradeMenu->timer = 0;
            gUnknown_03000508[0] = 0;
            gMain.state ++;
            for (i = 0; i < 13; i ++)
                sMenuTextTileBuffers[i] = ewram_2010000.tileBuffers[i];
            break;
        case  1:
            gMain.state ++;
            sTradeMenu->timer = 0;
            break;
        case  2:
            gMain.state ++;
            break;
        case  3:
            gMain.state ++;
            break;
        case  4:
            CalculatePlayerPartyCount();
            gMain.state ++;
            break;
        case  5:
            gMain.state ++;
            break;
        case  6:
            CalculateEnemyPartyCount();
            REG_DISPCNT = 0;
            sTradeMenu->partyCounts[0] = gPlayerPartyCount;
            sTradeMenu->partyCounts[1] = gEnemyPartyCount;
            PrintPartyNicknames(0);
            PrintPartyNicknames(1);
            for (i = 0; i < sTradeMenu->partyCounts[0]; i ++)
                sTradeMenu->partySpriteIds[0][i] = CreateMonIcon(GetMonData(&gPlayerParty[i], MON_DATA_SPECIES2, NULL), SpriteCB_MonIcon, gTradeMonSpriteCoords[i][0] * 8 + 14, gTradeMonSpriteCoords[i][1] * 8 - 12, TRUE, GetMonData(&gPlayerParty[i], MON_DATA_PERSONALITY));
            for (i = 0; i < sTradeMenu->partyCounts[1]; i ++)
                sTradeMenu->partySpriteIds[1][i] = CreateMonIcon(GetMonData(&gEnemyParty[i], MON_DATA_SPECIES2, NULL), SpriteCB_MonIcon, gTradeMonSpriteCoords[6 + i][0] * 8 + 14, gTradeMonSpriteCoords[6 + i][1] * 8 - 12, TRUE, GetMonData(&gEnemyParty[i], MON_DATA_PERSONALITY));
            nullsub_5(2, 0);
            gMain.state ++;
            break;
        case  7:
            LoadHeldItemIconGraphics();
            CreateHeldItemIcons(&sTradeMenu->partyCounts[0], sTradeMenu->partySpriteIds[0], 0);
            gMain.state ++;
            break;
        case  8:
            CreateHeldItemIcons(&sTradeMenu->partyCounts[0], sTradeMenu->partySpriteIds[0], 1);
            gMain.state ++;
            break;
        case  9:
            DrawTradeMenuText();
            gMain.state ++;
            sTradeMenu->timer = 0;
            break;
        case 10:
            nullsub_5(4, 0);
            if (LoadUISpriteGfx())
            {
                gMain.state ++;
            }
            break;
        case 11:
            CreateTradeMenuPlayerNameSprites();
            gMain.state ++;
            break;
        case 12:
            CreateTradeMenuActionTextSprites();
            unkStructF = &ewram_2010000.unk_08000;
            if (sTradeMenu->cursorPosition < 6)
                sTradeMenu->cursorPosition = unkStructF->unk_0009;
            else
                sTradeMenu->cursorPosition = unkStructF->unk_0009 + 6;
            sTradeMenu->cursorSpriteId = CreateSprite(&sSpriteTemplate_Cursor, gTradeMonSpriteCoords[sTradeMenu->cursorPosition][0] * 8 + 32, gTradeMonSpriteCoords[sTradeMenu->cursorPosition][1] * 8, 2);
            gMain.state = 15;
            nullsub_5(6, 0);
            break;
        case 15:
            LoadTradeBgGfx(0);
            gMain.state ++;
            break;
        case 16:
            LoadTradeBgGfx(1);
            sTradeMenu->bg2hofs = 0;
            sTradeMenu->bg3hofs = 0;
            SetActiveMenuOptions();
            nullsub_5(7, 0);
            gMain.state ++;
            break;
        case 17:
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
            gMain.state ++;
            break;
        case 18:
            REG_DISPCNT = DISPCNT_OBJ_1D_MAP | DISPCNT_BG_ALL_ON | DISPCNT_OBJ_ON;
            gMain.state ++;
            break;
        case 19:
            gMain.state ++;
            break;
        case 20:
            SetTradePartyHPBarSprites();
            gMain.state ++;
            break;
        case 21:
            if (!gPaletteFade.active)
            {
                SetMainCallback2(CB2_TradeMenu);
                gUnknown_03000508[0] = 0;
            }
            break;
    }
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void VBlankCB_TradeMenu(void)
{
    UploadTradeTilemapIfNeeded(&sTradeMenu->tilemapBuffer);
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

/*static*/ void CB_FadeToStartTrade(void)
{
    if (++sTradeMenu->timer >= 16)
    {
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        sTradeMenu->callbackId = 10;
    }
}

/*static*/ void CB_WaitToStartTrade(void)
{
    if (!gPaletteFade.active)
    {
        gSelectedTradeMonPositions[0] = sTradeMenu->cursorPosition;
        gSelectedTradeMonPositions[1] = sTradeMenu->partnerCursorPosition;
        SetCloseLinkCallback();
        sTradeMenu->callbackId = 13;
    }
}

/*static*/ void CB_StartLinkTrade(void)
{
    if (!gReceivedRemoteLinkPlayers)
    {
        gMain.callback1 = NULL;
        SetMainCallback2(CB2_LinkTrade);
    }
}

static void CB2_TradeMenu(void)
{
    RunTradeMenuCallback();
    DoQueuedActions();
    DrawSelectedMonScreen(0);
    DrawSelectedMonScreen(1);
    REG_BG2HOFS = sTradeMenu->bg2hofs++;
    REG_BG3HOFS = sTradeMenu->bg3hofs--;
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

void LoadTradeBgGfx(u8 state)
{
    int i;

    switch (state)
    {
        case 0:
            for (i = 0; i < 48; i ++)
            {
                gPlttBufferUnfaded[i] = *(gTradeMenu_Pal + i);
                gPlttBufferFaded[i] = *(gTradeMenu_Pal + i);
            }
            DmaCopyLarge16(3, gTradeMenu_Gfx, (void *)BG_VRAM, 0x1280, 0x1000);
            for (i = 0; i < 0x400; i ++)
                sTradeMenu->tilemapBuffer.tilemap[i] = gTradeMenu_Tilemap[i];
            DmaCopy16Defvars(3, gTradeStripesBG2Tilemap, BG_SCREEN_ADDR(6), 0x800);
            break;
        case 1:
            DmaCopy16Defvars(3, gTradeStripesBG3Tilemap, BG_SCREEN_ADDR(7), 0x800);
            PrintPartyLevelsAndGenders(0);
            PrintPartyLevelsAndGenders(1);
            MarkTradeTilemapForUpload(&sTradeMenu->tilemapBuffer);
            REG_BG0CNT &= ~0x03; // BGCNT_PRIORITY(0)
            REG_BG1CNT = BGCNT_PRIORITY(1) | BGCNT_SCREENBASE(5);
            REG_BG2CNT = BGCNT_PRIORITY(2) | BGCNT_SCREENBASE(6);
            REG_BG3CNT = BGCNT_PRIORITY(3) | BGCNT_SCREENBASE(7);
            REG_BG0HOFS = 0;
            REG_BG1HOFS = 0;
            REG_BG2HOFS = 0;
            REG_BG3HOFS = 0;
            REG_BG0VOFS = 0;
            REG_BG1VOFS = 0;
            REG_BG2VOFS = 0;
            REG_BG3VOFS = 0;
            break;
    }
}

void SetActiveMenuOptions(void)
{
    int i;
    for (i = 0; i < PARTY_SIZE; i ++)
    {
        if (i < sTradeMenu->partyCounts[0])
        {
            gSprites[sTradeMenu->partySpriteIds[0][i]].invisible = FALSE;
            sTradeMenu->optionsActive[i] = TRUE;
        }
        else
        {
            sTradeMenu->optionsActive[i] = FALSE;
        }
        if (i < sTradeMenu->partyCounts[1])
        {
            gSprites[sTradeMenu->partySpriteIds[1][i]].invisible = FALSE;
            sTradeMenu->optionsActive[i + 6] = TRUE;
        }
        else
        {
            sTradeMenu->optionsActive[i + 6] = FALSE;
        }
    }
    sTradeMenu->optionsActive[12] = TRUE;
}

static void nullsub_5(u8 a0, u8 a1) {}

// why not just use memcpy?
static void Trade_Memcpy(void *dataDest, void *dataSrc, u32 size)
{
    u8 *dest = dataDest;
    u8 *src = dataSrc;
    int i;
    for (i = 0; i < size; i ++) dest[i] = src[i];
}

static bool8 BufferTradeParties(void)
{
    u8 mpId = GetMultiplayerId();
    int i;
    u16 species;
    u8 nickname[11];
    struct Pokemon *pokemon;

    SetLinkDebugValues(sTradeMenu->bufferPartyState / 100, sTradeMenu->bufferPartyState % 100);
    switch (sTradeMenu->bufferPartyState)
    {
        case  0:
            Trade_Memcpy(gBlockSendBuffer, &gPlayerParty[0], 2 * sizeof(struct Pokemon));
            sTradeMenu->bufferPartyState ++;
            break;
        case  1:
            if (IsLinkTaskFinished())
            {
                if (GetBlockReceivedStatus() == 0)
                {
                    sTradeMenu->bufferPartyState ++;
                }
                else
                {
                    ResetBlockReceivedFlags();
                    sTradeMenu->bufferPartyState ++;
                }
            }
            break;
        case  2:
            if (mpId == 0)
            {
                SendBlockRequest(BLOCK_REQ_SIZE_200);
            }
            sTradeMenu->bufferPartyState ++;
            break;
        case  3:
            if (GetBlockReceivedStatus() == 3)
            {
                Trade_Memcpy(&gEnemyParty[0], gBlockRecvBuffer[mpId ^ 1], 2 * sizeof(struct Pokemon));
                ResetBlockReceivedFlags();
                sTradeMenu->bufferPartyState ++;
            }
            break;
        case  4:
            Trade_Memcpy(gBlockSendBuffer, &gPlayerParty[2], 2 * sizeof(struct Pokemon));
            sTradeMenu->bufferPartyState ++;
            break;
        case  5:
            if (mpId == 0)
            {
                SendBlockRequest(BLOCK_REQ_SIZE_200);
            }
            sTradeMenu->bufferPartyState ++;
            break;
        case  6:
            if (GetBlockReceivedStatus() == 3)
            {
                Trade_Memcpy(&gEnemyParty[2], gBlockRecvBuffer[mpId ^ 1], 2 * sizeof(struct Pokemon));
                ResetBlockReceivedFlags();
                sTradeMenu->bufferPartyState ++;
            }
            break;
        case  7:
            Trade_Memcpy(gBlockSendBuffer, &gPlayerParty[4], 2 * sizeof(struct Pokemon));
            sTradeMenu->bufferPartyState ++;
            break;
        case  8:
            if (mpId == 0)
            {
                SendBlockRequest(BLOCK_REQ_SIZE_200);
            }
            sTradeMenu->bufferPartyState ++;
            break;
        case  9:
            if (GetBlockReceivedStatus() == 3)
            {
                Trade_Memcpy(&gEnemyParty[4], gBlockRecvBuffer[mpId ^ 1], 2 * sizeof(struct Pokemon));
                ResetBlockReceivedFlags();
                sTradeMenu->bufferPartyState ++;
            }
            break;
        case 10:
            Trade_Memcpy(gBlockSendBuffer, &gSaveBlock1.mail[0], PARTY_SIZE * sizeof(struct Mail) + 4);
            sTradeMenu->bufferPartyState ++;
            break;
        case 11:
            if (mpId == 0)
            {
                SendBlockRequest(BLOCK_REQ_SIZE_220);
            }
            sTradeMenu->bufferPartyState ++;
            break;
        case 12:
            if (GetBlockReceivedStatus() == 3)
            {
                Trade_Memcpy(&gTradeMail[0], gBlockRecvBuffer[mpId ^ 1], PARTY_SIZE * sizeof(struct Mail));
                ResetBlockReceivedFlags();
                sTradeMenu->bufferPartyState ++;
            }
            break;
        case 13:
            Trade_Memcpy(gBlockSendBuffer, gSaveBlock1.giftRibbons, GIFT_RIBBONS_COUNT);
            sTradeMenu->bufferPartyState ++;
            break;
        case 14:
            if (mpId == 0)
            {
                SendBlockRequest(BLOCK_REQ_SIZE_40);
            }
            sTradeMenu->bufferPartyState ++;
            break;
        case 15:
            if (GetBlockReceivedStatus() == 3)
            {
                Trade_Memcpy(sTradeMenu->giftRibbons, gBlockRecvBuffer[mpId ^ 1], GIFT_RIBBONS_COUNT);
                ResetBlockReceivedFlags();
                sTradeMenu->bufferPartyState ++;
            }
            break;
        case 16:
            pokemon = gEnemyParty;
            for (i = 0; i < PARTY_SIZE; i ++)
            {
                if ((species = GetMonData(pokemon, MON_DATA_SPECIES)) != SPECIES_NONE && species == SPECIES_SHEDINJA && GetMonData(pokemon, MON_DATA_LANGUAGE) != LANGUAGE_JAPANESE)
                {
                    GetMonData(pokemon, MON_DATA_NICKNAME, nickname);
                    if (!StringCompareWithoutExtCtrlCodes(nickname, sText_ShedinjaJP))
                        SetMonData(pokemon, MON_DATA_NICKNAME, gSpeciesNames[SPECIES_SHEDINJA]);
                }
                pokemon ++;
            }
            return TRUE;
    }
    return FALSE;
}

static void PrintIsThisTradeOkay(void)
{
    u8 message[28];
    StringCopy(message, gTradeText_TradeOkayPrompt);
    DrawBottomRowText(message, BG_CHAR_ADDR(4) + sTradeMenu->bottomTextTileStart * 32, 20);
}

static void Leader_ReadLinkBuffer(u8 mpId, u8 status)
{
    if (status & 1)
    {
        switch (gBlockRecvBuffer[mpId][0])
        {
            case LINKCMD_REQUEST_CANCEL:
                sTradeMenu->playerSelectStatus = 2;
                break;
            case LINKCMD_READY_TO_TRADE:
                sTradeMenu->playerSelectStatus = 1;
                break;
            case LINKCMD_INIT_BLOCK:
                sTradeMenu->playerConfirmStatus = 1;
                break;
            case LINKCMD_READY_CANCEL_TRADE:
                sTradeMenu->playerConfirmStatus = 2;
                break;
        }
        ResetBlockReceivedFlag(0);
    }
    if (status & 2)
    {
        switch (gBlockRecvBuffer[1][0])
        {
            case LINKCMD_REQUEST_CANCEL:
                sTradeMenu->partnerSelectStatus = 2;
                break;
            case LINKCMD_READY_TO_TRADE:
                sTradeMenu->partnerCursorPosition = gBlockRecvBuffer[1][1] + PARTY_SIZE;
                sTradeMenu->partnerSelectStatus = 1;
                break;
            case LINKCMD_INIT_BLOCK:
                sTradeMenu->partnerConfirmStatus = 1;
                break;
            case LINKCMD_READY_CANCEL_TRADE:
                sTradeMenu->partnerConfirmStatus = 2;
                break;
        }
        ResetBlockReceivedFlag(1);
    }
}

static void Follower_ReadLinkBuffer(u8 mpId, u8 status)
{
    if (status & 1)
    {
        switch (gBlockRecvBuffer[0][0])
        {
            case LINKCMD_BOTH_CANCEL_TRADE:
                BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
                PrintTradeMessage(4);
                sTradeMenu->callbackId = 11;
                break;
            case LINKCMD_PARTNER_CANCEL_TRADE:
                PrintTradeMessage(5);
                sTradeMenu->callbackId = 8;
                break;
            case LINKCMD_SET_MONS_TO_TRADE:
                sTradeMenu->partnerCursorPosition = gBlockRecvBuffer[0][1] + PARTY_SIZE;
                SetSelectedMon(sTradeMenu->cursorPosition);
                SetSelectedMon(sTradeMenu->partnerCursorPosition);
                sTradeMenu->callbackId = 7;
                break;
            case LINKCMD_START_TRADE:
                BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
                sTradeMenu->callbackId = 10;
                break;
            case LINKCMD_PLAYER_CANCEL_TRADE:
                PrintTradeMessage(1);
                sTradeMenu->callbackId = 8;
                break;
        }
        ResetBlockReceivedFlag(0);
    }
    if (status & 2)
    {
        ResetBlockReceivedFlag(1);
    }
}

static void Leader_HandleCommunication(void)
{
    if (sTradeMenu->playerSelectStatus && sTradeMenu->partnerSelectStatus)
    {
        if (sTradeMenu->playerSelectStatus == 1 && sTradeMenu->partnerSelectStatus == 1)
        {
            sTradeMenu->callbackId = 6;
            sTradeMenu->linkData[0] = LINKCMD_SET_MONS_TO_TRADE;
            sTradeMenu->linkData[1] = sTradeMenu->cursorPosition;
            QueueAction(5, 0);
            sTradeMenu->playerSelectStatus = sTradeMenu->partnerSelectStatus = 0;
        }
        else if (sTradeMenu->playerSelectStatus == 1 && sTradeMenu->partnerSelectStatus == 2)
        {
            PrintTradeMessage(1);
            sTradeMenu->linkData[0] = LINKCMD_PARTNER_CANCEL_TRADE;
            sTradeMenu->linkData[1] = 0;
            QueueAction(5, 0);
            sTradeMenu->playerConfirmStatus = sTradeMenu->partnerConfirmStatus = 0;
            sTradeMenu->playerSelectStatus = sTradeMenu->partnerSelectStatus = 0;
            sTradeMenu->callbackId = 8;
        }
        else if (sTradeMenu->playerSelectStatus == 2 && sTradeMenu->partnerSelectStatus == 1)
        {
            PrintTradeMessage(5);
            sTradeMenu->linkData[0] = LINKCMD_PLAYER_CANCEL_TRADE;
            sTradeMenu->linkData[1] = 0;
            QueueAction(5, 0);
            sTradeMenu->playerConfirmStatus = sTradeMenu->partnerConfirmStatus = 0;
            sTradeMenu->playerSelectStatus = sTradeMenu->partnerSelectStatus = 0;
            sTradeMenu->callbackId = 8;
        }
        else if (sTradeMenu->playerSelectStatus == 2 && sTradeMenu->partnerSelectStatus == 2)
        {
            sTradeMenu->linkData[0] = LINKCMD_BOTH_CANCEL_TRADE;
            sTradeMenu->linkData[1] = 0;
            QueueAction(5, 0);
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            sTradeMenu->playerSelectStatus = sTradeMenu->partnerSelectStatus = 0;
            sTradeMenu->callbackId = 11;
        }
    }
    if (sTradeMenu->playerConfirmStatus && sTradeMenu->partnerConfirmStatus)
    {
        if (sTradeMenu->playerConfirmStatus == 1 && sTradeMenu->partnerConfirmStatus == 1)
        {
            sTradeMenu->linkData[0] = LINKCMD_START_TRADE;
            sTradeMenu->linkData[1] = 0;
            QueueAction(5, 0);
            sTradeMenu->playerConfirmStatus = 0;
            sTradeMenu->partnerConfirmStatus = 0;
            sTradeMenu->callbackId = 9;
        }
        if (sTradeMenu->playerConfirmStatus == 2 || sTradeMenu->partnerConfirmStatus == 2)
        {
            PrintTradeMessage(1);
            sTradeMenu->linkData[0] = LINKCMD_PLAYER_CANCEL_TRADE;
            sTradeMenu->linkData[1] = 0;
            QueueAction(5, 0);
            sTradeMenu->playerConfirmStatus = 0;
            sTradeMenu->partnerConfirmStatus = 0;
            sTradeMenu->callbackId = 8;
        }
    }
}

static void CB1_UpdateLink(void)
{
    u8 mpId = GetMultiplayerId();
    u8 status;
    if ((status = GetBlockReceivedStatus()))
    {
        if (mpId == 0)
            Leader_ReadLinkBuffer(mpId, status);
        else
            Follower_ReadLinkBuffer(mpId, status);
        ResetBlockReceivedFlags();
    }
    if (mpId == 0)
        Leader_HandleCommunication();
}

static u8 GetNewCursorPosition(u8 oldPosition, u8 direction)
{
    int i;
    u8 newPosition = 0;
    for (i = 0; i < PARTY_SIZE; i ++)
    {
        if (sTradeMenu->optionsActive[gTradeNextSelectedMonTable[oldPosition][direction][i]] == TRUE)
        {
            newPosition = gTradeNextSelectedMonTable[oldPosition][direction][i];
            break;
        }
    }
    return newPosition;
}

static void TradeMenuMoveCursor(u8 *cursorPosition, u8 direction)
{
    u8 newPosition = GetNewCursorPosition(*cursorPosition, direction);
    if (newPosition == 12) // CANCEL
    {
        StartSpriteAnim(&gSprites[sTradeMenu->cursorSpriteId], CURSOR_ANIM_ON_CANCEL);
        gSprites[sTradeMenu->cursorSpriteId].x = 0xe0;
        gSprites[sTradeMenu->cursorSpriteId].y = 0xa0;
    }
    else
    {
        StartSpriteAnim(&gSprites[sTradeMenu->cursorSpriteId], CURSOR_ANIM_NORMAL);
        gSprites[sTradeMenu->cursorSpriteId].x = gTradeMonSpriteCoords[newPosition][0] * 8 + 32;
        gSprites[sTradeMenu->cursorSpriteId].y = gTradeMonSpriteCoords[newPosition][1] * 8;
    }
    if (*cursorPosition != newPosition)
    {
        PlaySE(SE_SELECT);
    }
    *cursorPosition = newPosition;
}

static void SetReadyToTrade(void)
{
    PrintTradeMessage(0);
    sTradeMenu->callbackId = 5;
    if (GetMultiplayerId() == 1)
    {
        sTradeMenu->linkData[0] = LINKCMD_READY_TO_TRADE;
        sTradeMenu->linkData[1] = sTradeMenu->cursorPosition;
        Trade_SendData(sTradeMenu);
    }
    else
    {
        sTradeMenu->playerSelectStatus = 1;
    }
}

static void CB_ProcessMenuInput(void)
{
    int i;
    if (JOY_REPT(DPAD_UP))
    {
        TradeMenuMoveCursor(&sTradeMenu->cursorPosition, 0);
    }
    else if (JOY_REPT(DPAD_DOWN))
    {
        TradeMenuMoveCursor(&sTradeMenu->cursorPosition, 1);
    }
    else if (JOY_REPT(DPAD_LEFT))
    {
        TradeMenuMoveCursor(&sTradeMenu->cursorPosition, 2);
    }
    else if (JOY_REPT(DPAD_RIGHT))
    {
        TradeMenuMoveCursor(&sTradeMenu->cursorPosition, 3);
    }
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (sTradeMenu->cursorPosition < PARTY_SIZE)
        {
            TextWindow_DrawStdFrame(&sTradeMenu->window, 18, 14, 28, 19);
            Menu_PrintItems(19, 15, 2, (const struct MenuAction *)sSelectTradeMonActions);
            InitMenu(0, 19, 15, 2, 0, 9);
            sTradeMenu->callbackId = 1;
        }
        else if (sTradeMenu->cursorPosition < 2 * PARTY_SIZE)
        {
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            sTradeMenu->callbackId = 2;
        }
        else if (sTradeMenu->cursorPosition == 2 * PARTY_SIZE)
        {
            TextWindow_DrawStdFrame(&sTradeMenu->window, 24, 14, 29, 19);
            InitYesNoMenu(24, 14, 4);
            sTradeMenu->callbackId = 4;
            DrawBottomRowText(sActionTexts[4], BG_CHAR_ADDR(4) + 32 * sTradeMenu->bottomTextTileStart, 20);
        }
    }
    if (JOY_NEW(R_BUTTON))
    {
        for (i = 0; i < 10; i ++)
        {
            sTradeMenu->linkData[i] = i;
        }
        Trade_SendData(sTradeMenu);
    }
}

static void RedrawChooseAPokemonWindow(void)
{
    Menu_DestroyCursor();
    PrintTradePartnerPartyNicknames();
    sTradeMenu->callbackId = 0;
    gSprites[sTradeMenu->cursorSpriteId].invisible = FALSE;
    DrawBottomRowText(sActionTexts[1], BG_CHAR_ADDR(4) + 32 * sTradeMenu->bottomTextTileStart, 20);
}

static void CB_ProcessSelectedMonInput(void)
{
    if (JOY_REPT(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        sTradeMenu->unk_007c = Menu_MoveCursor(-1);
    }
    else if (JOY_REPT(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        sTradeMenu->unk_007c = Menu_MoveCursor(+1);
    }
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (Menu_GetCursorPos() == 0)
        {
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            sTradeMenu->callbackId = 2;
        }
        else if (CountOtherUsableMons(sTradeMenu->isLiveMon[0], sTradeMenu->partyCounts[0], sTradeMenu->cursorPosition) == 0)
        {
            QueueAction(3, 2);
            sTradeMenu->callbackId = 8;
        }
        else
        {
            SetReadyToTrade();
            gSprites[sTradeMenu->cursorSpriteId].invisible = TRUE;
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        RedrawChooseAPokemonWindow();
    }
}

static void CB_ChooseMonAfterButtonPress(void)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        RedrawChooseAPokemonWindow();
    }
}

static void CB_ShowTradeMonSummaryScreen(void)
{
    if (!gPaletteFade.active)
    {
        if (sTradeMenu->cursorPosition < PARTY_SIZE)
        {
            ShowPokemonSummaryScreen(gPlayerParty, sTradeMenu->cursorPosition, sTradeMenu->partyCounts[0] - 1, CB2_ReturnToTradeMenu, PSS_MODE_NO_MOVE_ORDER_EDIT);
        }
        else
        {
            ShowPokemonSummaryScreen(gEnemyParty, sTradeMenu->cursorPosition - 6, sTradeMenu->partyCounts[1] - 1, CB2_ReturnToTradeMenu, PSS_MODE_NO_MOVE_ORDER_EDIT);
        }
    }
}

static u8 CountOtherUsableMons(const u8 *usableMons, u8 partyCount, u8 selectedMonIndex)
{
    u8 usableMonCount = 0;
    int i;
    for (i = 0; i < partyCount; i ++)
    {
        if (selectedMonIndex != i)
        {
            usableMonCount += usableMons[i];
        }
    }
    return usableMonCount;
}

static void CheckMonsBeforeTrade(void)
{
    u8 usableMons[12];
    int i;
    for (i = 0; i < sTradeMenu->partyCounts[0]; i ++)
    {
        usableMons[i] = sTradeMenu->isLiveMon[0][i];
    }
    if (CountOtherUsableMons(usableMons, sTradeMenu->partyCounts[0], sTradeMenu->cursorPosition) == 0)
    {
        QueueAction(3, 2);
        sTradeMenu->linkData[0] = LINKCMD_READY_CANCEL_TRADE;
        QueueAction(0xb4, 0);
    }
    else
    {
        QueueAction(3, 1);
        sTradeMenu->linkData[0] = LINKCMD_INIT_BLOCK;
        if (IsLinkTaskFinished())
        {
            Trade_SendData(sTradeMenu);
        }
    }
}

static void CB_ProcessConfirmTradeInput(void)
{
    if (JOY_REPT(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        sTradeMenu->unk_007c = Menu_MoveCursor(-1);
    }
    else if (JOY_REPT(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        sTradeMenu->unk_007c = Menu_MoveCursor(+1);
    }
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (Menu_GetCursorPos() == 0)
        {
            CheckMonsBeforeTrade();
        }
        else
        {
            QueueAction(3, 1);
            if (IsLinkTaskFinished())
            {
                sTradeMenu->linkData[0] = LINKCMD_READY_CANCEL_TRADE;
                Trade_SendData(sTradeMenu);
            }
        }
        sTradeMenu->callbackId = 100;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        QueueAction(3, 1);
        if (Menu_GetCursorPos() == 0)
        {
            sTradeMenu->unk_007c = Menu_MoveCursor(+1);
        }
        sTradeMenu->linkData[0] = LINKCMD_READY_CANCEL_TRADE;
        Trade_SendData(sTradeMenu);
        sTradeMenu->callbackId = 100;
    }
}

static void CB_ProcessCancelTradeInput(void)
{
    if (JOY_REPT(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        sTradeMenu->unk_007c = Menu_MoveCursor(-1);
    }
    else if (JOY_REPT(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        sTradeMenu->unk_007c = Menu_MoveCursor(+1);
    }
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (Menu_GetCursorPos() == 0)
        {
            PrintTradeMessage(4);
            sTradeMenu->linkData[0] = LINKCMD_REQUEST_CANCEL;
            sTradeMenu->linkData[1] = 0;
            QueueAction(5, 0);
            gSprites[sTradeMenu->cursorSpriteId].invisible = TRUE;
            sTradeMenu->callbackId = 100;
        }
        else
        {
            RedrawChooseAPokemonWindow();
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        RedrawChooseAPokemonWindow();
    }
}

static void CB_SetSelectedMons(void)
{
    if (GetMultiplayerId() == 0)
    {
        SetSelectedMon(sTradeMenu->cursorPosition);
        SetSelectedMon(sTradeMenu->partnerCursorPosition);
    }
    sTradeMenu->callbackId = 7;
}

static void CB_PrintIsThisTradeOkay(void)
{
    if (sTradeMenu->drawSelectedMonState[0] == 5 && sTradeMenu->drawSelectedMonState[1] == 5)
    {
        PrintIsThisTradeOkay();
        sTradeMenu->callbackId = 14;
    }
}

static void CB_InitConfirmTradePrompt(void)
{
    sTradeMenu->timer++;
    if (sTradeMenu->timer > 120)
    {
        TextWindow_DrawStdFrame(&sTradeMenu->window, 24, 14, 29, 19);
        InitYesNoMenu(24, 14, 4);
        sTradeMenu->timer = 0;
        sTradeMenu->callbackId = 3;
    }
}

static void CB_HandleTradeCanceled(void)
{
    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(SE_SELECT);
        RedrawPartyWindow(0);
        RedrawPartyWindow(1);
        sTradeMenu->callbackId = 0;
        gSprites[sTradeMenu->cursorSpriteId].invisible = FALSE;
    }
}

static void CB_InitExitCanceledTrade(void)
{
    if (!gPaletteFade.active)
    {
        SetCloseLinkCallback();
        sTradeMenu->callbackId = 12;
    }
}

static void CB_ExitCanceledTrade(void)
{
    if (gReceivedRemoteLinkPlayers == 0)
    {
        SetMainCallback2(CB2_ReturnToFieldFromMultiplayer);
    }
}

static void RunTradeMenuCallback(void)
{
    switch (sTradeMenu->callbackId)
    {
        case  0:
            CB_ProcessMenuInput();
            break;
        case  1:
            CB_ProcessSelectedMonInput();
            break;
        case  2:
            CB_ShowTradeMonSummaryScreen();
            break;
        case  3:
            CB_ProcessConfirmTradeInput();
            break;
        case  4:
            CB_ProcessCancelTradeInput();
            break;
        case  6:
            CB_SetSelectedMons();
            break;
        case  7:
            CB_PrintIsThisTradeOkay();
            break;
        case  8:
            CB_HandleTradeCanceled();
            break;
        case  9:
            CB_FadeToStartTrade();
            break;
        case 10:
            CB_WaitToStartTrade();
            break;
        case 11:
            CB_InitExitCanceledTrade();
            break;
        case 12:
            CB_ExitCanceledTrade();
            break;
        case 13:
            CB_StartLinkTrade();
            break;
        case 14:
            CB_InitConfirmTradePrompt();
            break;
        case 15:
            CB_ChooseMonAfterButtonPress();
            break;
    }
}

static void SetSelectedMon(u8 cursorPosition)
{
    u8 whichParty = cursorPosition / PARTY_SIZE;
    if (sTradeMenu->drawSelectedMonState[whichParty] == 0)
    {
        sTradeMenu->drawSelectedMonState[whichParty] = 1;
        sTradeMenu->selectedMonIdx[whichParty] = cursorPosition;
    }
}

// TODO: Figure out what the f**k is going on here
#ifdef NONMATCHING
static void DrawSelectedMonScreen(u8 a0)
{
    u8 i;
    s8 stringLength;
    u8 whichParty = 1;
    u8 whichPokemon;
    u8 string1[40];
    u8 string2[56];
    u8 temp0 = sTradeMenu->selectedMonIdx[a0];
    if (temp0 < PARTY_SIZE)
        whichParty = 0;
    whichPokemon = temp0 % PARTY_SIZE;

    switch (sTradeMenu->drawSelectedMonState[a0])
    {
        case 1:
            for (i = 0; i < sTradeMenu->partyCounts[a0]; i ++)
            {
                gSprites[sTradeMenu->partySpriteIds[whichParty][i]].invisible = TRUE;
            }
            gSprites[sTradeMenu->partySpriteIds[whichParty][whichPokemon]].invisible = FALSE;
            gSprites[sTradeMenu->partySpriteIds[whichParty][whichPokemon]].data[0] = 20;
            gSprites[sTradeMenu->partySpriteIds[whichParty][whichPokemon]].data[2] = (gTradeMonSpriteCoords[6 * whichParty + whichPokemon][0] + gTradeMonSpriteCoords[6 * whichParty + whichPokemon + 1][0]) / 2 * 8 + 14;
            gSprites[sTradeMenu->partySpriteIds[whichParty][whichPokemon]].data[4] = gTradeMonSpriteCoords[6 * whichParty + whichPokemon][1] * 8 - 12;
            StoreSpriteCallbackInData6(&gSprites[sTradeMenu->partySpriteIds[whichParty][whichPokemon]], SpriteCB_MonIcon);
            sTradeMenu->drawSelectedMonState[a0] ++;
            Trade_MoveSelectedMonToTarget(&gSprites[sTradeMenu->partySpriteIds[whichParty][whichPokemon]]);
            Menu_DestroyCursor();
            Text_FillWindowRectDefPalette(&sTradeMenu->window, 0, sTradePartyWindowColumnBounds[whichParty][0], 0, sTradePartyWindowColumnBounds[whichParty][1], 19);
            CopyTradeTilemapToBufferAndRequestUpload(&sTradeMenu->tilemapBuffer, 15 * a0, 0, gTradePartyBoxTilemap, 15, 17, 0);
            if (whichParty == 0)
            {
                PrintTradePartnerPartyNicknames();
            }
            break;
        case 2:
            if (gSprites[sTradeMenu->partySpriteIds[whichParty][whichPokemon]].callback == SpriteCB_MonIcon)
            {
                sTradeMenu->drawSelectedMonState[a0] = 3;
            }
            break;
        case 3:
            CopyTradeTilemapToBufferAndRequestUpload(&sTradeMenu->tilemapBuffer, 15 * whichParty, 0, gTradePartyBoxTilemap, 15, 17, 0);
            gSprites[sTradeMenu->partySpriteIds[whichParty][whichPokemon]].x = (gTradeMonSpriteCoords[6 * whichParty + whichPokemon ][0] + gTradeMonSpriteCoords[6 * whichParty + whichPokemon + 1][0]) / 2 * 8 + 14;
            gSprites[sTradeMenu->partySpriteIds[whichParty][whichPokemon]].y = gTradeMonSpriteCoords[6 * whichParty + whichPokemon ][1] * 8 - 12;
            gSprites[sTradeMenu->partySpriteIds[whichParty][whichPokemon]].x2 = 0;
            gSprites[sTradeMenu->partySpriteIds[whichParty][whichPokemon]].y2 = 0;
            stringLength = GetMonNicknameWidth(string1 + 6, whichParty, whichPokemon);
            string1[0] = 0xFC;
            string1[1] = 0x06;
            string1[2] = 0x04;
            string1[3] = 0xFC;
            string1[4] = 0x11;
            string1[5] = (64 - stringLength) / 2;
            Text_InitWindowAndPrintText(&sTradeMenu->window, string1, sTradeMenu->textWindowBaseTileNum + whichParty * 6 * 32, sSelectedMonTextCoords[whichParty][0], sSelectedMonTextCoords[whichParty][1]);
            BufferMovesString(string2, whichParty, whichPokemon);
            Text_InitWindowAndPrintText(&sTradeMenu->window, gOtherText_Terminator2, sTradeMenu->textWindowBaseTileNum + whichParty * 6 * 32 + 32, sSelectedMonTextCoords[whichParty + 1][0], sSelectedMonTextCoords[whichParty + 1][1] + 1);
            Text_InitWindowAndPrintText(&sTradeMenu->window, string2, sTradeMenu->textWindowBaseTileNum + whichParty * 6 * 32 + 38, sSelectedMonTextCoords[whichParty + 1][0], sSelectedMonTextCoords[whichParty + 1][1] + 1);
            sTradeMenu->drawSelectedMonState[a0] ++;
            break;
        case 4:
            DrawBottomRowText(sActionTexts[5], BG_CHAR_ADDR(4) + 32 * sTradeMenu->bottomTextTileStart, 20);
            PrintLevelAndGender(a0, whichPokemon, sSelectedMonLevelGenderCoords[a0][0] + 4, sSelectedMonLevelGenderCoords[a0][1] + 1, sSelectedMonLevelGenderCoords[a0][0], sSelectedMonLevelGenderCoords[a0][1]);
            sTradeMenu->drawSelectedMonState[a0] ++;
            break;
    }
}
#else
NAKED
static void DrawSelectedMonScreen(u8 a0)
{
    asm_unified("\tpush {r4-r7,lr}\n"
                    "\tmov r7, r10\n"
                    "\tmov r6, r9\n"
                    "\tmov r5, r8\n"
                    "\tpush {r5-r7}\n"
                    "\tsub sp, 0x74\n"
                    "\tlsls r0, 24\n"
                    "\tlsrs r0, 24\n"
                    "\tmov r10, r0\n"
                    "\tldr r0, _08049F24 @ =sTradeMenu\n"
                    "\tldr r5, [r0]\n"
                    "\tadds r0, r5, 0\n"
                    "\tadds r0, 0x82\n"
                    "\tadd r0, r10\n"
                    "\tldrb r0, [r0]\n"
                    "\tmovs r1, 0x1\n"
                    "\tstr r1, [sp, 0x6C]\n"
                    "\tcmp r0, 0x5\n"
                    "\tbhi _08049EFE\n"
                    "\tmovs r2, 0\n"
                    "\tstr r2, [sp, 0x6C]\n"
                    "_08049EFE:\n"
                    "\tmovs r1, 0x6\n"
                    "\tbl __umodsi3\n"
                    "\tlsls r0, 24\n"
                    "\tlsrs r0, 24\n"
                    "\tmov r8, r0\n"
                    "\tadds r0, r5, 0\n"
                    "\tadds r0, 0x80\n"
                    "\tmov r1, r10\n"
                    "\tadds r3, r0, r1\n"
                    "\tldrb r0, [r3]\n"
                    "\tcmp r0, 0x2\n"
                    "\tbne _08049F1A\n"
                    "\tb _0804A0AC\n"
                    "_08049F1A:\n"
                    "\tcmp r0, 0x2\n"
                    "\tbgt _08049F28\n"
                    "\tcmp r0, 0x1\n"
                    "\tbeq _08049F36\n"
                    "\tb _0804A294\n"
                    "\t.align 2, 0\n"
                    "_08049F24: .4byte sTradeMenu\n"
                    "_08049F28:\n"
                    "\tcmp r0, 0x3\n"
                    "\tbne _08049F2E\n"
                    "\tb _0804A0E4\n"
                    "_08049F2E:\n"
                    "\tcmp r0, 0x4\n"
                    "\tbne _08049F34\n"
                    "\tb _0804A244\n"
                    "_08049F34:\n"
                    "\tb _0804A294\n"
                    "_08049F36:\n"
                    "\tmovs r4, 0\n"
                    "\tadds r0, r5, 0\n"
                    "\tadds r0, 0x42\n"
                    "\tadd r0, r10\n"
                    "\tldr r7, _0804A094 @ =gSprites\n"
                    "\tldr r2, [sp, 0x6C]\n"
                    "\tlsls r2, 1\n"
                    "\tmov r9, r2\n"
                    "\tldr r3, _0804A098 @ =gTradeMonSpriteCoords\n"
                    "\tmov r12, r3\n"
                    "\tmov r5, r10\n"
                    "\tlsls r5, 4\n"
                    "\tstr r5, [sp, 0x70]\n"
                    "\tldrb r0, [r0]\n"
                    "\tcmp r4, r0\n"
                    "\tbcs _08049F8E\n"
                    "\tadds r6, r7, 0\n"
                    "\tldr r2, _0804A09C @ =sTradeMenu\n"
                    "\tldr r0, [sp, 0x6C]\n"
                    "\tadd r0, r9\n"
                    "\tlsls r3, r0, 1\n"
                    "\tmovs r5, 0x4\n"
                    "_08049F62:\n"
                    "\tldr r0, [r2]\n"
                    "\tadds r1, r4, r3\n"
                    "\tadds r0, 0x34\n"
                    "\tadds r0, r1\n"
                    "\tldrb r1, [r0]\n"
                    "\tlsls r0, r1, 4\n"
                    "\tadds r0, r1\n"
                    "\tlsls r0, 2\n"
                    "\tadds r0, r6\n"
                    "\tadds r0, 0x3E\n"
                    "\tldrb r1, [r0]\n"
                    "\torrs r1, r5\n"
                    "\tstrb r1, [r0]\n"
                    "\tadds r0, r4, 0x1\n"
                    "\tlsls r0, 24\n"
                    "\tlsrs r4, r0, 24\n"
                    "\tldr r0, [r2]\n"
                    "\tadds r0, 0x42\n"
                    "\tadd r0, r10\n"
                    "\tldrb r0, [r0]\n"
                    "\tcmp r4, r0\n"
                    "\tbcc _08049F62\n"
                    "_08049F8E:\n"
                    "\tldr r1, _0804A09C @ =sTradeMenu\n"
                    "\tldr r0, [r1]\n"
                    "\tldr r5, [sp, 0x6C]\n"
                    "\tadd r5, r9\n"
                    "\tlsls r3, r5, 1\n"
                    "\tmov r2, r8\n"
                    "\tadds r6, r2, r3\n"
                    "\tadds r0, 0x34\n"
                    "\tadds r0, r6\n"
                    "\tldrb r1, [r0]\n"
                    "\tlsls r0, r1, 4\n"
                    "\tadds r0, r1\n"
                    "\tlsls r0, 2\n"
                    "\tadds r0, r7\n"
                    "\tadds r0, 0x3E\n"
                    "\tldrb r2, [r0]\n"
                    "\tmovs r1, 0x5\n"
                    "\tnegs r1, r1\n"
                    "\tands r1, r2\n"
                    "\tstrb r1, [r0]\n"
                    "\tldr r0, _0804A09C @ =sTradeMenu\n"
                    "\tldr r4, [r0]\n"
                    "\tadds r4, 0x34\n"
                    "\tadds r4, r6\n"
                    "\tldrb r1, [r4]\n"
                    "\tlsls r0, r1, 4\n"
                    "\tadds r0, r1\n"
                    "\tlsls r0, 2\n"
                    "\tadds r0, r7\n"
                    "\tmovs r1, 0x14\n"
                    "\tstrh r1, [r0, 0x2E]\n"
                    "\tldrb r0, [r4]\n"
                    "\tlsls r2, r0, 4\n"
                    "\tadds r2, r0\n"
                    "\tlsls r2, 2\n"
                    "\tadds r2, r7\n"
                    "\tlsls r5, 2\n"
                    "\tmov r1, r12\n"
                    "\tadds r0, r5, r1\n"
                    "\tldrb r0, [r0]\n"
                    "\tadds r3, 0x1\n"
                    "\tlsls r3, 1\n"
                    "\tadd r3, r12\n"
                    "\tldrb r1, [r3]\n"
                    "\tadds r0, r1\n"
                    "\tasrs r0, 1\n"
                    "\tlsls r0, 3\n"
                    "\tadds r0, 0xE\n"
                    "\tstrh r0, [r2, 0x32]\n"
                    "\tldrb r0, [r4]\n"
                    "\tlsls r1, r0, 4\n"
                    "\tadds r1, r0\n"
                    "\tlsls r1, 2\n"
                    "\tadds r1, r7\n"
                    "\tmov r0, r12\n"
                    "\tadds r0, 0x1\n"
                    "\tadds r5, r0\n"
                    "\tldrb r0, [r5]\n"
                    "\tlsls r0, 3\n"
                    "\tsubs r0, 0xC\n"
                    "\tstrh r0, [r1, 0x36]\n"
                    "\tldrb r1, [r4]\n"
                    "\tlsls r0, r1, 4\n"
                    "\tadds r0, r1\n"
                    "\tlsls r0, 2\n"
                    "\tadds r0, r7\n"
                    "\tldr r1, _0804A0A0 @ =SpriteCB_MonIcon\n"
                    "\tbl StoreSpriteCallbackInData6\n"
                    "\tldr r2, _0804A09C @ =sTradeMenu\n"
                    "\tldr r1, [r2]\n"
                    "\tadds r1, 0x80\n"
                    "\tadd r1, r10\n"
                    "\tldrb r0, [r1]\n"
                    "\tadds r0, 0x1\n"
                    "\tstrb r0, [r1]\n"
                    "\tldr r0, [r2]\n"
                    "\tadds r0, 0x34\n"
                    "\tadds r0, r6\n"
                    "\tldrb r1, [r0]\n"
                    "\tlsls r0, r1, 4\n"
                    "\tadds r0, r1\n"
                    "\tlsls r0, 2\n"
                    "\tadds r0, r7\n"
                    "\tbl Trade_MoveSelectedMonToTarget\n"
                    "\tbl Menu_DestroyCursor\n"
                    "\tldr r3, _0804A09C @ =sTradeMenu\n"
                    "\tldr r0, [r3]\n"
                    "\tadds r0, 0x4\n"
                    "\tldr r1, _0804A0A4 @ =sTradePartyWindowColumnBounds\n"
                    "\tmov r5, r9\n"
                    "\tadds r2, r5, r1\n"
                    "\tldrb r2, [r2]\n"
                    "\tadds r1, 0x1\n"
                    "\tadd r1, r9\n"
                    "\tldrb r1, [r1]\n"
                    "\tstr r1, [sp]\n"
                    "\tmovs r1, 0x13\n"
                    "\tstr r1, [sp, 0x4]\n"
                    "\tmovs r1, 0\n"
                    "\tmovs r3, 0\n"
                    "\tbl Text_FillWindowRectDefPalette\n"
                    "\tldr r1, _0804A09C @ =sTradeMenu\n"
                    "\tldr r0, [r1]\n"
                    "\tadds r0, 0xC8\n"
                    "\tldr r2, [sp, 0x70]\n"
                    "\tmov r3, r10\n"
                    "\tsubs r1, r2, r3\n"
                    "\tlsls r1, 24\n"
                    "\tlsrs r1, 24\n"
                    "\tldr r3, _0804A0A8 @ =gTradePartyBoxTilemap\n"
                    "\tmovs r2, 0xF\n"
                    "\tstr r2, [sp]\n"
                    "\tmovs r2, 0x11\n"
                    "\tstr r2, [sp, 0x4]\n"
                    "\tmovs r5, 0\n"
                    "\tstr r5, [sp, 0x8]\n"
                    "\tmovs r2, 0\n"
                    "\tbl CopyTradeTilemapToBufferAndRequestUpload\n"
                    "\tldr r0, [sp, 0x6C]\n"
                    "\tcmp r0, 0\n"
                    "\tbeq _0804A08C\n"
                    "\tb _0804A294\n"
                    "_0804A08C:\n"
                    "\tbl PrintTradePartnerPartyNicknames\n"
                    "\tb _0804A294\n"
                    "\t.align 2, 0\n"
                    "_0804A094: .4byte gSprites\n"
                    "_0804A098: .4byte gTradeMonSpriteCoords\n"
                    "_0804A09C: .4byte sTradeMenu\n"
                    "_0804A0A0: .4byte SpriteCB_MonIcon\n"
                    "_0804A0A4: .4byte sTradePartyWindowColumnBounds\n"
                    "_0804A0A8: .4byte gTradePartyBoxTilemap\n"
                    "_0804A0AC:\n"
                    "\tldr r2, _0804A0DC @ =gSprites\n"
                    "\tldr r1, [sp, 0x6C]\n"
                    "\tlsls r0, r1, 1\n"
                    "\tadds r0, r1\n"
                    "\tlsls r0, 1\n"
                    "\tadd r0, r8\n"
                    "\tadds r1, r5, 0\n"
                    "\tadds r1, 0x34\n"
                    "\tadds r1, r0\n"
                    "\tldrb r1, [r1]\n"
                    "\tlsls r0, r1, 4\n"
                    "\tadds r0, r1\n"
                    "\tlsls r0, 2\n"
                    "\tadds r2, 0x1C\n"
                    "\tadds r0, r2\n"
                    "\tldr r1, [r0]\n"
                    "\tldr r0, _0804A0E0 @ =SpriteCB_MonIcon\n"
                    "\tcmp r1, r0\n"
                    "\tbeq _0804A0D4\n"
                    "\tb _0804A294\n"
                    "_0804A0D4:\n"
                    "\tmovs r0, 0x3\n"
                    "\tstrb r0, [r3]\n"
                    "\tb _0804A294\n"
                    "\t.align 2, 0\n"
                    "_0804A0DC: .4byte gSprites\n"
                    "_0804A0E0: .4byte SpriteCB_MonIcon\n"
                    "_0804A0E4:\n"
                    "\tadds r0, r5, 0\n"
                    "\tadds r0, 0xC8\n"
                    "\tldr r2, [sp, 0x6C]\n"
                    "\tlsls r1, r2, 4\n"
                    "\tsubs r1, r2\n"
                    "\tlsls r1, 24\n"
                    "\tlsrs r1, 24\n"
                    "\tldr r3, _0804A228 @ =gTradeMovesBoxTilemap\n"
                    "\tmovs r2, 0xF\n"
                    "\tstr r2, [sp]\n"
                    "\tmovs r7, 0x11\n"
                    "\tstr r7, [sp, 0x4]\n"
                    "\tmovs r5, 0\n"
                    "\tstr r5, [sp, 0x8]\n"
                    "\tmovs r2, 0\n"
                    "\tbl CopyTradeTilemapToBufferAndRequestUpload\n"
                    "\tldr r0, _0804A22C @ =sTradeMenu\n"
                    "\tldr r3, [r0]\n"
                    "\tldr r1, [sp, 0x6C]\n"
                    "\tlsls r6, r1, 1\n"
                    "\tadds r1, r6\n"
                    "\tmov r9, r1\n"
                    "\tlsls r1, 1\n"
                    "\tmov r2, r8\n"
                    "\tadds r0, r2, r1\n"
                    "\tadds r3, 0x34\n"
                    "\tadds r3, r0\n"
                    "\tldrb r0, [r3]\n"
                    "\tlsls r2, r0, 4\n"
                    "\tadds r2, r0\n"
                    "\tlsls r2, 2\n"
                    "\tldr r5, _0804A230 @ =gSprites\n"
                    "\tadds r2, r5\n"
                    "\tldr r4, _0804A234 @ =gTradeMonSpriteCoords\n"
                    "\tmov r0, r9\n"
                    "\tlsls r5, r0, 2\n"
                    "\tadds r0, r5, r4\n"
                    "\tldrb r0, [r0]\n"
                    "\tadds r1, 0x1\n"
                    "\tlsls r1, 1\n"
                    "\tadds r1, r4\n"
                    "\tldrb r1, [r1]\n"
                    "\tadds r0, r1\n"
                    "\tasrs r0, 1\n"
                    "\tlsls r0, 3\n"
                    "\tadds r0, 0xE\n"
                    "\tstrh r0, [r2, 0x20]\n"
                    "\tldrb r0, [r3]\n"
                    "\tlsls r1, r0, 4\n"
                    "\tadds r1, r0\n"
                    "\tlsls r1, 2\n"
                    "\tldr r2, _0804A230 @ =gSprites\n"
                    "\tadds r1, r2\n"
                    "\tadds r4, 0x1\n"
                    "\tadds r5, r4\n"
                    "\tldrb r0, [r5]\n"
                    "\tlsls r0, 3\n"
                    "\tsubs r0, 0xC\n"
                    "\tstrh r0, [r1, 0x22]\n"
                    "\tldrb r1, [r3]\n"
                    "\tlsls r0, r1, 4\n"
                    "\tadds r0, r1\n"
                    "\tlsls r0, 2\n"
                    "\tadds r0, r2\n"
                    "\tmovs r5, 0\n"
                    "\tstrh r5, [r0, 0x24]\n"
                    "\tldrb r1, [r3]\n"
                    "\tlsls r0, r1, 4\n"
                    "\tadds r0, r1\n"
                    "\tlsls r0, 2\n"
                    "\tadds r0, r2\n"
                    "\tstrh r5, [r0, 0x26]\n"
                    "\tmov r0, sp\n"
                    "\tadds r0, 0x12\n"
                    "\tldr r1, [sp, 0x6C]\n"
                    "\tmov r2, r8\n"
                    "\tbl GetMonNicknameWidth\n"
                    "\tadd r1, sp, 0xC\n"
                    "\tmovs r3, 0xFC\n"
                    "\tstrb r3, [r1]\n"
                    "\tadds r2, r1, 0\n"
                    "\tmovs r1, 0x6\n"
                    "\tstrb r1, [r2, 0x1]\n"
                    "\tmovs r1, 0x4\n"
                    "\tstrb r1, [r2, 0x2]\n"
                    "\tadds r1, r2, 0\n"
                    "\tstrb r3, [r1, 0x3]\n"
                    "\tstrb r7, [r1, 0x4]\n"
                    "\tlsls r0, 24\n"
                    "\tasrs r0, 24\n"
                    "\tmovs r1, 0x40\n"
                    "\tsubs r1, r0\n"
                    "\tlsrs r0, r1, 31\n"
                    "\tadds r1, r0\n"
                    "\tasrs r1, 1\n"
                    "\tstrb r1, [r2, 0x5]\n"
                    "\tldr r0, _0804A22C @ =sTradeMenu\n"
                    "\tldr r1, [r0]\n"
                    "\tadds r0, r1, 0x4\n"
                    "\tadds r1, 0x7A\n"
                    "\tldrb r2, [r1]\n"
                    "\tmov r1, r9\n"
                    "\tlsls r1, 6\n"
                    "\tmov r9, r1\n"
                    "\tadd r2, r9\n"
                    "\tldr r4, _0804A238 @ =sSelectedMonTextCoords\n"
                    "\tldr r3, [sp, 0x6C]\n"
                    "\tlsls r1, r3, 2\n"
                    "\tadds r3, r1, r4\n"
                    "\tldrb r3, [r3]\n"
                    "\tldr r5, _0804A23C @ =sSelectedMonTextCoords + 0x1\n"
                    "\tadds r1, r5\n"
                    "\tldrb r1, [r1]\n"
                    "\tstr r1, [sp]\n"
                    "\tadd r1, sp, 0xC\n"
                    "\tbl Text_InitWindowAndPrintText\n"
                    "\tadd r7, sp, 0x34\n"
                    "\tadds r0, r7, 0\n"
                    "\tldr r1, [sp, 0x6C]\n"
                    "\tmov r2, r8\n"
                    "\tbl BufferMovesString\n"
                    "\tldr r0, _0804A22C @ =sTradeMenu\n"
                    "\tldr r2, [r0]\n"
                    "\tadds r0, r2, 0x4\n"
                    "\tldr r1, _0804A240 @ =gOtherText_Terminator2\n"
                    "\tadds r2, 0x7A\n"
                    "\tldrb r2, [r2]\n"
                    "\tadd r2, r9\n"
                    "\tadds r2, 0x20\n"
                    "\tadds r6, 0x1\n"
                    "\tlsls r6, 1\n"
                    "\tadds r4, r6, r4\n"
                    "\tldrb r5, [r4]\n"
                    "\tldr r3, _0804A23C @ =sSelectedMonTextCoords + 0x1\n"
                    "\tadds r6, r3\n"
                    "\tldrb r4, [r6]\n"
                    "\tadds r4, 0x1\n"
                    "\tlsls r4, 24\n"
                    "\tlsrs r4, 24\n"
                    "\tstr r4, [sp]\n"
                    "\tadds r3, r5, 0\n"
                    "\tbl Text_InitWindowAndPrintText\n"
                    "\tldr r0, _0804A22C @ =sTradeMenu\n"
                    "\tldr r1, [r0]\n"
                    "\tadds r0, r1, 0x4\n"
                    "\tadds r1, 0x7A\n"
                    "\tldrb r2, [r1]\n"
                    "\tadd r2, r9\n"
                    "\tadds r2, 0x26\n"
                    "\tstr r4, [sp]\n"
                    "\tadds r1, r7, 0\n"
                    "\tadds r3, r5, 0\n"
                    "\tbl Text_InitWindowAndPrintText\n"
                    "\tldr r2, _0804A22C @ =sTradeMenu\n"
                    "\tldr r1, [r2]\n"
                    "\tb _0804A28A\n"
                    "\t.align 2, 0\n"
                    "_0804A228: .4byte gTradeMovesBoxTilemap\n"
                    "_0804A22C: .4byte sTradeMenu\n"
                    "_0804A230: .4byte gSprites\n"
                    "_0804A234: .4byte gTradeMonSpriteCoords\n"
                    "_0804A238: .4byte sSelectedMonTextCoords\n"
                    "_0804A23C: .4byte sSelectedMonTextCoords + 0x1\n"
                    "_0804A240: .4byte gOtherText_Terminator2\n"
                    "_0804A244:\n"
                    "\tldr r0, _0804A2A4 @ =sActionTexts\n"
                    "\tldr r0, [r0, 0x14]\n"
                    "\tadds r1, r5, 0\n"
                    "\tadds r1, 0x7E\n"
                    "\tldrh r1, [r1]\n"
                    "\tlsls r1, 5\n"
                    "\tldr r3, _0804A2A8 @ =0x06010000\n"
                    "\tadds r1, r3\n"
                    "\tmovs r2, 0x14\n"
                    "\tbl DrawBottomRowText\n"
                    "\tldr r0, _0804A2AC @ =sSelectedMonLevelGenderCoords\n"
                    "\tmov r5, r10\n"
                    "\tlsls r1, r5, 1\n"
                    "\tadds r4, r1, r0\n"
                    "\tldrb r2, [r4]\n"
                    "\tadds r2, 0x4\n"
                    "\tlsls r2, 24\n"
                    "\tlsrs r2, 24\n"
                    "\tadds r0, 0x1\n"
                    "\tadds r1, r0\n"
                    "\tldrb r3, [r1]\n"
                    "\tadds r3, 0x1\n"
                    "\tlsls r3, 24\n"
                    "\tlsrs r3, 24\n"
                    "\tldrb r0, [r4]\n"
                    "\tstr r0, [sp]\n"
                    "\tldrb r0, [r1]\n"
                    "\tstr r0, [sp, 0x4]\n"
                    "\tmov r0, r10\n"
                    "\tmov r1, r8\n"
                    "\tbl PrintLevelAndGender\n"
                    "\tldr r0, _0804A2B0 @ =sTradeMenu\n"
                    "\tldr r1, [r0]\n"
                    "_0804A28A:\n"
                    "\tadds r1, 0x80\n"
                    "\tadd r1, r10\n"
                    "\tldrb r0, [r1]\n"
                    "\tadds r0, 0x1\n"
                    "\tstrb r0, [r1]\n"
                    "_0804A294:\n"
                    "\tadd sp, 0x74\n"
                    "\tpop {r3-r5}\n"
                    "\tmov r8, r3\n"
                    "\tmov r9, r4\n"
                    "\tmov r10, r5\n"
                    "\tpop {r4-r7}\n"
                    "\tpop {r0}\n"
                    "\tbx r0\n"
                    "\t.align 2, 0\n"
                    "_0804A2A4: .4byte sActionTexts\n"
                    "_0804A2A8: .4byte 0x06010000\n"
                    "_0804A2AC: .4byte sSelectedMonLevelGenderCoords\n"
                    "_0804A2B0: .4byte sTradeMenu");
}
#endif

u8 GetMonNicknameWidth(u8 *str, u8 whichParty, u8 partyIdx)
{
    u8 nickname[11];
    if (whichParty == 0)
    {
        GetMonData(&gPlayerParty[partyIdx], MON_DATA_NICKNAME, nickname);
        StringCopy_Nickname(str, nickname);
        GetMonGender(&gPlayerParty[partyIdx]);
        GetMonData(&gPlayerParty[partyIdx], MON_DATA_LEVEL);
    }
    else
    {
        GetMonData(&gEnemyParty[partyIdx], MON_DATA_NICKNAME, nickname);
        StringCopy_Nickname(str, nickname);
        GetMonGender(&gEnemyParty[partyIdx]);
        GetMonData(&gEnemyParty[partyIdx], MON_DATA_LEVEL);
    }
    return Text_GetStringWidthFromWindowTemplate(&gTradeNicknameWindowTemplate, str);
}

void BufferMovesString(u8 *str, u8 whichParty, u8 partyIdx)
{
    u16 i;
    u16 moves[4];
    if (sTradeMenu->isEgg[whichParty][partyIdx] == 0)
    {
        for (i = 0; i < 4; i ++)
        {
            if (whichParty == 0)
                moves[i] = GetMonData(&gPlayerParty[partyIdx], MON_DATA_MOVE1 + i, NULL);
            else
                moves[i] = GetMonData(&gEnemyParty[partyIdx], MON_DATA_MOVE1 + i, NULL);
        }
        StringCopy(str, sText_EmptyString);
        for (i = 0; i < 4; i ++)
        {
            if (moves[i] != 0)
            {
                StringAppend(str, gMoveNames[moves[i]]);
            }
            StringAppend(str, gOtherText_ControlAndMiscText);
        }
    }
    else
    {
        StringCopy(str, sText_EmptyString);
        StringAppend(str, gOtherText_FourQuestions);
    }
}

#ifdef NONMATCHING
static void PrintPartyNicknames(u8 whichParty)
{
    u8 i;
    u8 nickname[22];
    u8 string[40];
    struct Pokemon *pokemon;

    string[0] = 0xFC;
    string[1] = 0x06;
    string[2] = 0x04;
    string[3] = 0xFC;
    string[4] = 0x11;
    string[5] = 0x00;

    for (i = 0; i < sTradeMenu->partyCounts[whichParty]; i ++)
    {
        pokemon = whichParty == 0 ? &gPlayerParty[i] : &gEnemyParty[i];
        GetMonData(pokemon, MON_DATA_NICKNAME, nickname);
        StringCopy_Nickname(string + 6, nickname);
        GetMonGender(pokemon);
        string[5] = (50 - Text_GetStringWidthFromWindowTemplate(&gTradeNicknameWindowTemplate, string + 6)) / 2;
        Text_InitWindowAndPrintText(&sTradeMenu->window, string, sTradeMenu->textWindowBaseTileNum + 22 * 6 * whichParty + 22 * i, gTradeMonSpriteCoords[i + 6 * whichParty][0], gTradeMonSpriteCoords[i + 6 * whichParty][1]);
    }
}
#else
NAKED
static void PrintPartyNicknames(u8 whichParty)
{
    asm_unified("\tpush {r4-r7,lr}\n"
                    "\tmov r7, r10\n"
                    "\tmov r6, r9\n"
                    "\tmov r5, r8\n"
                    "\tpush {r5-r7}\n"
                    "\tsub sp, 0x44\n"
                    "\tlsls r0, 24\n"
                    "\tlsrs r5, r0, 24\n"
                    "\tadd r3, sp, 0x1C\n"
                    "\tmovs r2, 0\n"
                    "\tmovs r1, 0xFC\n"
                    "\tstrb r1, [r3]\n"
                    "\tmovs r0, 0x6\n"
                    "\tstrb r0, [r3, 0x1]\n"
                    "\tmovs r0, 0x4\n"
                    "\tstrb r0, [r3, 0x2]\n"
                    "\tstrb r1, [r3, 0x3]\n"
                    "\tmovs r0, 0x11\n"
                    "\tstrb r0, [r3, 0x4]\n"
                    "\tstrb r2, [r3, 0x5]\n"
                    "\tmovs r6, 0\n"
                    "\tldr r1, _0804A470 @ =sTradeMenu\n"
                    "\tldr r0, [r1]\n"
                    "\tadds r0, 0x42\n"
                    "\tadds r0, r5\n"
                    "\tldrb r0, [r0]\n"
                    "\tcmp r6, r0\n"
                    "\tbcs _0804A504\n"
                    "\tmov r8, r3\n"
                    "\tadds r7, r1, 0\n"
                    "\tldr r0, _0804A474 @ =gTradeMonSpriteCoords\n"
                    "\tmov r9, r0\n"
                    "\tmovs r4, 0x1\n"
                    "\tadd r4, r9\n"
                    "\tmov r10, r4\n"
                    "_0804A462:\n"
                    "\tcmp r5, 0\n"
                    "\tbne _0804A47C\n"
                    "\tmovs r0, 0x64\n"
                    "\tadds r4, r6, 0\n"
                    "\tmuls r4, r0\n"
                    "\tldr r0, _0804A478 @ =gPlayerParty\n"
                    "\tb _0804A484\n"
                    "\t.align 2, 0\n"
                    "_0804A470: .4byte sTradeMenu\n"
                    "_0804A474: .4byte gTradeMonSpriteCoords\n"
                    "_0804A478: .4byte gPlayerParty\n"
                    "_0804A47C:\n"
                    "\tmovs r0, 0x64\n"
                    "\tadds r4, r6, 0\n"
                    "\tmuls r4, r0\n"
                    "\tldr r0, _0804A514 @ =gEnemyParty\n"
                    "_0804A484:\n"
                    "\tadds r4, r0\n"
                    "\tadds r0, r4, 0\n"
                    "\tmovs r1, 0x2\n"
                    "\tadd r2, sp, 0x4\n"
                    "\tbl GetMonData\n"
                    "\tmov r0, sp\n"
                    "\tadds r0, 0x22\n"
                    "\tadd r1, sp, 0x4\n"
                    "\tbl StringCopy_Nickname\n"
                    "\tadds r0, r4, 0\n"
                    "\tbl GetMonGender\n"
                    "\tmov r1, sp\n"
                    "\tadds r1, 0x22\n"
                    "\tldr r0, _0804A518 @ =gTradeNicknameWindowTemplate\n"
                    "\tbl Text_GetStringWidthFromWindowTemplate\n"
                    "\tlsls r0, 24\n"
                    "\tlsrs r0, 24\n"
                    "\tmovs r1, 0x32\n"
                    "\tsubs r1, r0\n"
                    "\tlsrs r0, r1, 31\n"
                    "\tadds r1, r0\n"
                    "\tasrs r1, 1\n"
                    "\tmov r0, r8\n"
                    "\tstrb r1, [r0, 0x5]\n"
                    "\tldr r1, [r7]\n"
                    "\tadds r0, r1, 0x4\n"
                    "\tadds r1, 0x7A\n"
                    "\tlsls r2, r5, 5\n"
                    "\tadds r2, r5\n"
                    "\tlsls r2, 2\n"
                    "\tldrb r1, [r1]\n"
                    "\tadds r2, r1\n"
                    "\tmovs r1, 0x16\n"
                    "\tmuls r1, r6\n"
                    "\tadds r2, r1\n"
                    "\tlsls r2, 16\n"
                    "\tlsrs r2, 16\n"
                    "\tlsls r1, r5, 1\n"
                    "\tadds r1, r5\n"
                    "\tlsls r1, 1\n"
                    "\tadds r1, r6, r1\n"
                    "\tlsls r1, 1\n"
                    "\tmov r4, r9\n"
                    "\tadds r3, r1, r4\n"
                    "\tldrb r3, [r3]\n"
                    "\tadd r1, r10\n"
                    "\tldrb r1, [r1]\n"
                    "\tstr r1, [sp]\n"
                    "\tmov r1, r8\n"
                    "\tbl Text_InitWindowAndPrintText\n"
                    "\tadds r0, r6, 0x1\n"
                    "\tlsls r0, 24\n"
                    "\tlsrs r6, r0, 24\n"
                    "\tldr r0, [r7]\n"
                    "\tadds r0, 0x42\n"
                    "\tadds r0, r5\n"
                    "\tldrb r0, [r0]\n"
                    "\tcmp r6, r0\n"
                    "\tbcc _0804A462\n"
                    "_0804A504:\n"
                    "\tadd sp, 0x44\n"
                    "\tpop {r3-r5}\n"
                    "\tmov r8, r3\n"
                    "\tmov r9, r4\n"
                    "\tmov r10, r5\n"
                    "\tpop {r4-r7}\n"
                    "\tpop {r0}\n"
                    "\tbx r0\n"
                    "\t.align 2, 0\n"
                    "_0804A514: .4byte gEnemyParty\n"
                    "_0804A518: .4byte gTradeNicknameWindowTemplate");
}
#endif

void PrintLevelAndGender(u8 whichParty, u8 monIdx, u8 x, u8 y, u8 width, u8 height)
{
    u8 nickname[24];
    u8 level;
    u8 gender;
    u8 levelTens;
    CopyTradeTilemapToBuffer(&sTradeMenu->tilemapBuffer, width, height, gTradeMonBoxTilemap, 6, 3, 0);
    if (whichParty == 0)
    {
        level = GetMonData(&gPlayerParty[monIdx], MON_DATA_LEVEL, NULL);
        gender = GetMonGender(&gPlayerParty[monIdx]);
        GetMonData(&gPlayerParty[monIdx], MON_DATA_NICKNAME, nickname);
    }
    else
    {
        level = GetMonData(&gEnemyParty[monIdx], MON_DATA_LEVEL, NULL);
        gender = GetMonGender(&gEnemyParty[monIdx]);
        GetMonData(&gEnemyParty[monIdx], MON_DATA_NICKNAME, nickname);
    }
    if (sTradeMenu->isEgg[whichParty][monIdx] == 0)
    {
        levelTens = level / 10;
        if (levelTens != 0)
        {
            sTradeMenu->tilemapBuffer.tilemap[x + 32 * y] = levelTens + 0x60;
        }
        sTradeMenu->tilemapBuffer.tilemap[x + 32 * y + 1] = (u8)(level % 10) + 0x70;
        if (gender == MON_MALE)
        {
            if (!NameHasGenderSymbol(nickname, MON_MALE))
            {
                sTradeMenu->tilemapBuffer.tilemap[x + 32 * y - 31] += 1;
            }
        }
        else if (gender == MON_FEMALE)
        {
            if (!NameHasGenderSymbol(nickname, MON_FEMALE))
            {
                sTradeMenu->tilemapBuffer.tilemap[x + 32 * y - 31] += 2;
            }
        }
    }
    else
    {
        sTradeMenu->tilemapBuffer.tilemap[x + 32 * y - 32] = sTradeMenu->tilemapBuffer.tilemap[x + 32 * y - 33];
        sTradeMenu->tilemapBuffer.tilemap[x + 32 * y - 31] = sTradeMenu->tilemapBuffer.tilemap[x + 32 * y - 36] | 0x400;
    }
#ifdef GERMAN
    sTradeMenu->tilemapBuffer.needsUpload = TRUE;
#endif
}

static void PrintPartyLevelsAndGenders(u8 whichParty)
{
    int i;
    for (i = 0; i < sTradeMenu->partyCounts[whichParty]; i++)
    {
        int partySlot = i + whichParty * PARTY_SIZE;
        PrintLevelAndGender(whichParty, i, gTradeLevelDisplayCoords[partySlot][0], gTradeLevelDisplayCoords[partySlot][1], gTradeMonBoxCoords[partySlot][0], gTradeMonBoxCoords[partySlot][1]);
    }
}

static void ShowTradePartyMonIcons(u8 whichParty)
{
    int i;
    for (i = 0; i < sTradeMenu->partyCounts[whichParty]; i ++)
    {
        gSprites[sTradeMenu->partySpriteIds[whichParty][i]].invisible = FALSE;
        gSprites[sTradeMenu->partySpriteIds[whichParty][i]].x = gTradeMonSpriteCoords[PARTY_SIZE * whichParty + i][0] * 8 + 14;
        gSprites[sTradeMenu->partySpriteIds[whichParty][i]].y = gTradeMonSpriteCoords[PARTY_SIZE * whichParty + i][1] * 8 - 12;
        gSprites[sTradeMenu->partySpriteIds[whichParty][i]].x2 = 0;
        gSprites[sTradeMenu->partySpriteIds[whichParty][i]].y2 = 0;
    }
}

static void PrintTradePartnerPartyNicknames(void)
{
    Text_FillWindowRectDefPalette(&sTradeMenu->window, 0, sTradePartyWindowColumnBounds[1][0], 0, sTradePartyWindowColumnBounds[1][1], 19);
    PrintPartyNicknames(1);
}

static void RedrawPartyWindow(u8 whichParty)
{
    if (whichParty == 0)
    {
        Text_FillWindowRectDefPalette(&sTradeMenu->window, 0, sTradePartyWindowColumnBounds[0][0], 0, sTradePartyWindowColumnBounds[0][1], 19);
        CopyTradeTilemapToBufferAndRequestUpload(&sTradeMenu->tilemapBuffer, 0, 0, gTradePartyBoxTilemap, 15, 17, 0);
        PrintPartyLevelsAndGenders(0);
        PrintPartyNicknames(0);
        ShowTradePartyMonIcons(0);
        MarkTradeTilemapForUpload(&sTradeMenu->tilemapBuffer);
    }
    else
    {
        Menu_DestroyCursor();
        Text_FillWindowRectDefPalette(&sTradeMenu->window, 0, sTradePartyWindowColumnBounds[1][0], 0, sTradePartyWindowColumnBounds[1][1], 19);
        CopyTradeTilemapToBufferAndRequestUpload(&sTradeMenu->tilemapBuffer, 15, 0, gTradePartyBoxTilemap, 15, 17, 0);
        PrintPartyLevelsAndGenders(1);
        PrintPartyNicknames(1);
        ShowTradePartyMonIcons(1);
        MarkTradeTilemapForUpload(&sTradeMenu->tilemapBuffer);
    }
    DrawBottomRowText(sActionTexts[1], BG_CHAR_ADDR(4) + 32 * sTradeMenu->bottomTextTileStart, 20);
    sTradeMenu->drawSelectedMonState[whichParty] = 0;
}

static void MarkTradeTilemapForUpload(struct TradeTilemapBuffer *tilemapBuffer)
{
    tilemapBuffer->needsUpload = TRUE;
}

static void UploadTradeTilemapIfNeeded(struct TradeTilemapBuffer *tilemapBuffer)
{
    if (tilemapBuffer->needsUpload)
    {
        CpuCopy16(tilemapBuffer->tilemap, tilemapBuffer->vramDest, sizeof(tilemapBuffer->tilemap));
        tilemapBuffer->needsUpload = FALSE;
    }
}

static void InitTradeTilemapBuffer(struct TradeTilemapBuffer *tilemapBuffer, void *vramDest)
{
    tilemapBuffer->needsUpload = FALSE;
    tilemapBuffer->vramDest = vramDest;
}

static void CopyTradeTilemapToBuffer(struct TradeTilemapBuffer *tilemapBuffer, u8 left, u8 top, const u16 *tilemap, u8 width, u8 height, u16 paletteOffset) {
    int y, x;

    for (y = 0; y < height; y++)
    {

        for (x = 0; x < width; x++)
        {
            tilemapBuffer->tilemap[(top * 32 + left) + y * 32 + x] = tilemap[width * y + x] | paletteOffset;
        }
    }

#if ENGLISH
    tilemapBuffer->needsUpload = TRUE;
#endif
}

#if GERMAN
static void CopyTradeTilemapToBufferAndRequestUpload(struct TradeTilemapBuffer *tilemapBuffer, u8 left, u8 top, const u16 *tilemap, u8 width, u8 height, u16 paletteOffset) {
    CopyTradeTilemapToBuffer(tilemapBuffer, left, top, tilemap, width, height, paletteOffset);

    tilemapBuffer->needsUpload = TRUE;
}
#endif

static void Task_DrawSelectionSummary(u8 unused)
{
    Menu_EraseScreen();
}

static void Task_DrawSelectionTrade(u8 unused)
{
    Menu_EraseScreen();
}

static void PrintTradeMessage(u8 messageId)
{
    TextWindow_DrawStdFrame(&sTradeMenu->window, gTradeMessageWindowRects[messageId][0], gTradeMessageWindowRects[messageId][1], gTradeMessageWindowRects[messageId][2], gTradeMessageWindowRects[messageId][3]);
    Text_InitWindowAndPrintText(&sTradeMenu->window, sTradeMessages[messageId], 0x180 + sTradeMenu->textWindowBaseTileNum, gTradeMessageWindowRects[messageId][0] + 1, gTradeMessageWindowRects[messageId][1] + 1);
}

static void InitTradeMenuActionQueue(void)
{
    int i;
    for (i = 0; i < 4; i ++)
    {
        sTradeMenu->queuedActions[i].active = 0;
        sTradeMenu->queuedActions[i].delay = 0;
        sTradeMenu->queuedActions[i].actionId |= 0xff;
    }
}

static void QueueAction(u16 delay, u8 actionId)
{
    int i;
    for (i = 0; i < 4; i ++)
    {
        if (sTradeMenu->queuedActions[i].active == 0)
        {
            sTradeMenu->queuedActions[i].delay = delay;
            sTradeMenu->queuedActions[i].actionId = actionId;
            sTradeMenu->queuedActions[i].active = 1;
            break;
        }
    }
}

static void DoQueuedActions(void)
{
    int i;
    for (i = 0; i < 4; i ++)
    {
        if (sTradeMenu->queuedActions[i].active)
        {
            if (sTradeMenu->queuedActions[i].delay != 0)
            {
                sTradeMenu->queuedActions[i].delay --;
                continue;
            }
            switch (sTradeMenu->queuedActions[i].actionId)
            {
                case 0:
                    Trade_SendData(sTradeMenu);
                    break;
                case 1:
                    PrintTradeMessage(0);
                    break;
                case 2:
                    PrintTradeMessage(2);
                    break;
                case 3:
                    PrintTradeMessage(3);
                    break;
                case 4:
                    PrintTradeMessage(3);
                    break;
                case 5:
                    PrintTradeMessage(3);
                    break;
            }
            sTradeMenu->queuedActions[i].active = 0;
        }
    }
}

static bool8 LoadUISpriteGfx(void)
{
    switch (sTradeMenu->timer)
    {
        case  8:
            sTradeMenu->bottomTextTileStart = LoadSpriteSheet(&sMenuTextSpriteSheets[sTradeMenu->timer]);
            sTradeMenu->timer ++;
            return FALSE;
        case 13:
            LoadSpritePalette(&sSpritePalette_MenuText);
            sTradeMenu->timer ++;
            return FALSE;
        case 14:
            LoadSpritePalette(&sCursorSpritePalette);
            sTradeMenu->timer ++;
            return FALSE;
        case 15:
            LoadSpriteSheet(&sCursorSpriteSheet);
            sTradeMenu->timer ++;
            // fallthrough
        case 16:
            sTradeMenu->timer = 0;
            return TRUE;
        default:
            LoadSpriteSheet(&sMenuTextSpriteSheets[sTradeMenu->timer]);
            sTradeMenu->timer ++;
            return FALSE;
    }
}

static void DrawBottomRowText(const u8 *str, u8 *dest, u8 unused)
{
    DrawTextWindowAndBufferTiles(&gTradeMenuTextWindowTemplate, dest, str, unused);
}

// replacing [whichParty] with [0]/[1] depending on case compiles better code
static void ComputePartyTradeableFlags(u8 whichParty)
{
    struct Pokemon *pokemon;
    int i;
    
    switch (whichParty)
    {
        case 0:
            for (i = 0; i < sTradeMenu->partyCounts[whichParty]; i++)
            {
                pokemon = &gPlayerParty[i];
                if (GetMonData(pokemon, MON_DATA_IS_EGG) == TRUE)
                {
                    sTradeMenu->isLiveMon[whichParty][i] = 0;
                    sTradeMenu->isEgg[whichParty][i] = 1;
                }
                else if (GetMonData(pokemon, MON_DATA_HP) == 0)
                {
                    sTradeMenu->isLiveMon[whichParty][i] = 0;
                    sTradeMenu->isEgg[whichParty][i] = 0;
                }
                else
                {
                    sTradeMenu->isLiveMon[whichParty][i] = 1;
                    sTradeMenu->isEgg[whichParty][i] = 0;
                }
            }
            break;
        case 1:
            for (i = 0; i < sTradeMenu->partyCounts[whichParty]; i++)
            {
                pokemon = &gEnemyParty[i];
                if (GetMonData(pokemon, MON_DATA_IS_EGG) == TRUE)
                {
                    sTradeMenu->isLiveMon[whichParty][i] = 0;
                    sTradeMenu->isEgg[whichParty][i] = 1;
                }
                else if (GetMonData(pokemon, MON_DATA_HP) == 0)
                {
                    sTradeMenu->isLiveMon[whichParty][i] = 0;
                    sTradeMenu->isEgg[whichParty][i] = 0;
                }
                else
                {
                    sTradeMenu->isLiveMon[whichParty][i] = 1;
                    sTradeMenu->isEgg[whichParty][i] = 0;
                }
            }
            break;
    }
}

static void ComputePartyHPBarLevels(u8 whichParty)
{
    u16 i;
    u16 curHp;
    u16 maxHp;
    switch (whichParty)
    {
        case 0:
            for (i = 0; i < sTradeMenu->partyCounts[0]; i ++)
            {
                curHp = GetMonData(&gPlayerParty[i], MON_DATA_HP);
                maxHp = GetMonData(&gPlayerParty[i], MON_DATA_MAX_HP);
                sTradeMenu->hpBarLevels[0][i] = GetHPBarLevel(curHp, maxHp);
            }
            break;
        case 1:
            for (i = 0; i < sTradeMenu->partyCounts[1]; i ++)
            {
                curHp = GetMonData(&gEnemyParty[i], MON_DATA_HP);
                maxHp = GetMonData(&gEnemyParty[i], MON_DATA_MAX_HP);
                sTradeMenu->hpBarLevels[1][i] = GetHPBarLevel(curHp, maxHp);
            }
            break;
    }
}

static void SetTradePartyHPBarSprites(void)
{
    int i, j;
    for (i = 0; i < 2; i ++)
    {
        for (j = 0; j < sTradeMenu->partyCounts[i]; j ++)
        {
            SetPartyHPBarSprite(&gSprites[sTradeMenu->partySpriteIds[i][j]], 4 - sTradeMenu->hpBarLevels[i][j]);
        }
    }
}

static void SaveTradeGiftRibbons(void)
{
    int i;
    for (i = 0; i < 11; i ++)
        if (gSaveBlock1.giftRibbons[i] == 0 && sTradeMenu->giftRibbons[i] != 0)
            gSaveBlock1.giftRibbons[i] = sTradeMenu->giftRibbons[i];
}

static void DrawTextWindowAndBufferTiles(const struct WindowTemplate *windowTemplate, u8 *dest, const u8 *str, u8 size)
{
    u8 i;
    u8 *tileBuffer;
    size = (size + 3) / 4;
    tileBuffer = gTileBuffer;
    CpuFill16(0, tileBuffer, size * 0x80);
    CpuFill16(0, tileBuffer + windowTemplate->width * 0x20, size * 0x80);
    Text_InitWindow8004E3C(windowTemplate, tileBuffer, str);
    for (i = 0; i < size; i ++)
    {
        CpuCopy16(&tileBuffer[32 * (i * 4)], &dest[32 * (i * 8)], 0x80);
        CpuCopy16(&tileBuffer[32 * (i * 4 + windowTemplate->width)], &dest[32 * (i * 8 + 4)], 0x80);
    }
}

static void SpriteCB_LinkMonGlow(struct Sprite *sprite)
{
    if (++ sprite->data[0] == 10)
    {
        PlaySE(SE_BALL);
        sprite->data[0] = 0;
    }
}

static void SpriteCB_LinkMonShadow(struct Sprite *sprite)
{
    if (sprite->data[1] == 0)
    {
        if (++ sprite->data[0] == 12)
            sprite->data[0] = 0;
        LoadPalette(&sLinkMonShadow_Pal[sprite->data[0]], 16 * (sprite->oam.paletteNum + 16) + 4, 2);
    }
}

static void SpriteCB_CableEndSending(struct Sprite *sprite)
{
    sprite->data[0] ++;
    sprite->y2 ++;
    if (sprite->data[0] == 10)
        DestroySprite(sprite);
}

static void SpriteCB_CableEndReceiving(struct Sprite *sprite)
{
    sprite->data[0] ++;
    sprite->y2 --;
    if (sprite->data[0] == 10)
        DestroySprite(sprite);
}

static void SpriteCB_GbaScreen(struct Sprite *sprite)
{
    if (++ sprite->data[0] == 15)
    {
        PlaySE(SE_M_MINIMIZE);
        sprite->data[0] = 0;
    }
}

static void SetTradeBGAffine(void)
{
    struct BgAffineDstData dest;
    DoBgAffineSet(&dest, gTradeAnim->texX << 8, gTradeAnim->texY << 8, gTradeAnim->scrX, gTradeAnim->scrY, gTradeAnim->sXY, gTradeAnim->sXY, gTradeAnim->alpha);
    REG_BG2PA = dest.pa;
    REG_BG2PB = dest.pb;
    REG_BG2PC = dest.pc;
    REG_BG2PD = dest.pd;
    REG_BG2X = dest.dx;
    REG_BG2Y = dest.dy;
}

static void SetTradeGpuRegs(void)
{
    u16 dispcnt;

    REG_BG1VOFS = gTradeAnim->bg1vofs;
    REG_BG1HOFS = gTradeAnim->bg1hofs;

    /*
        A u16 cast allows for REG_DISPCNT storage to be swapped.
        This is required for the function to match.

        You can see this less obfuscated in FireRed and Emerald,
        since they use gflib's GPU manager for this instead.
    */
    dispcnt = (*(u16 *)REG_ADDR_DISPCNT);
    if ((dispcnt & 7) == DISPCNT_MODE_0)
    {
        REG_BG2VOFS = gTradeAnim->bg2vofs;
        REG_BG2HOFS = gTradeAnim->bg2hofs;
    }
    else
    {
        SetTradeBGAffine();
    }
}

static void VBlankCB_TradeAnim(void)
{
    SetTradeGpuRegs();
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void ClearLinkTimeoutTimer(void)
{
    gTradeAnim->linkTimeoutTimer = 0;
    gTradeAnim->linkTimeoutZero1 = 0;
    gTradeAnim->linkTimeoutZero2 = 0;
}

void CheckForLinkTimeout(void)
{
    if (gTradeAnim->linkTimeoutZero1 == gTradeAnim->linkTimeoutZero2)
    {
        gTradeAnim->linkTimeoutTimer ++;
    }
    else
    {
        gTradeAnim->linkTimeoutTimer = 0;
    }
    if (gTradeAnim->linkTimeoutTimer > 0xb4)
    {
        gTradeAnim->linkTimeoutTimer = 0;
        gTradeAnim->linkTimeoutZero2 = 0;
        gTradeAnim->linkTimeoutZero1 = 0;
    }
    gTradeAnim->linkTimeoutZero2 = gTradeAnim->linkTimeoutZero1;
}

static u8 TradeGetMultiplayerId(void)
{
    if (gReceivedRemoteLinkPlayers)
        return GetMultiplayerId();
    return 0;
}

static void LoadTradeMonPic(u8 whichParty, u8 state)
{
    u8 battlePosition;
    struct Pokemon *mon;
    u16 species;
    u32 personality;

    battlePosition = 0;
    mon = NULL;
    if (whichParty == 0)
    {
        mon = &gPlayerParty[gSelectedTradeMonPositions[0]];
        battlePosition = 1;
    }
    if (whichParty == 1)
    {
        mon = &gEnemyParty[gSelectedTradeMonPositions[1] % 6];
        battlePosition = 3;
    }
    switch (state)
    {
        case 0:
            species = GetMonData(mon, MON_DATA_SPECIES2);
            personality = GetMonData(mon, MON_DATA_PERSONALITY);
            HandleLoadSpecialPokePic(&gMonFrontPicTable[species], gMonFrontPicCoords[species].coords, gMonFrontPicCoords[species].y_offset, gSharedMem, gMonSpriteGfxBufferPtrs[whichParty * 2 + 1], species, personality);
            LoadCompressedSpritePalette(GetMonSpritePalStruct(mon));
            gTradeAnim->monSpecies[whichParty] = species;
            break;
        case 1:
            SetMultiuseSpriteTemplateToPokemon(GetMonSpritePalStruct(mon)->tag, battlePosition);
            gTradeAnim->monSpriteIds[whichParty] = CreateSprite(&gCreatingSpriteTemplate, 0x78, 0x3c, 0x6);
            gSprites[gTradeAnim->monSpriteIds[whichParty]].invisible = TRUE;
            gSprites[gTradeAnim->monSpriteIds[whichParty]].callback = SpriteCallbackDummy;
            break;
    }
}

// non-shifting nonmatch
// out of order statements + register swaps?
// r4 is loaded with 0 way before it's used (and it should be r2)
#ifdef NONMATCHING
void CB2_LinkTrade(void)
// Link trade init
{
    switch (gMain.state)
    {
        case 0:
            REG_DISPCNT = 0;
            ResetTasks();
            CloseLink();
            gTradeAnim = &ewram_2010000.tradeAnim;
            ResetSpriteData();
            FreeAllSpritePalettes();
            SetVBlankCallback(VBlankCB_TradeAnim);
            ClearLinkTimeoutTimer();
            Text_LoadWindowTemplate(&gBattleMessageWindowTemplate);
            Text_InitWindowWithTemplate(&gTradeAnim->window, &gBattleMessageWindowTemplate);
            gTradeAnim->textWindowBaseTileNum = TextWindow_SetBaseTileNum(2);
            //
            // start of nonmatching here?
            TextWindow_LoadStdFrameGraphics(&gTradeAnim->window);
            Menu_EraseScreen();
            gLinkType = LINKTYPE_TRADE_DISCONNECTED;
            gMain.state++;
            LZDecompressVram(gBattleTextboxTiles, (void *)VRAM);
            CpuCopy16(gBattleTextboxTilemap, gSharedMem, 0x1000);
            DmaCopy16Defvars(3, gSharedMem, BG_SCREEN_ADDR(5), 0x500);
            LoadCompressedPalette(gBattleTextboxPalette, 0, 32);
            gTradeAnim->neverRead_00B6 = 0; // nonmatching code writes to this address with r4, as opposed to r2 in matching code. r4 is also loaded with zero way before
            gTradeAnim->state = 0;
            gTradeAnim->isLinkTrade = TRUE;
            gTradeAnim->texX = 0x40;
            gTradeAnim->texY = 0x40;
            gTradeAnim->neverRead_0108 = 0;
            gTradeAnim->neverRead_010A = 0;
            gTradeAnim->scrX = 0x78;
            gTradeAnim->scrY = 0x50;
            gTradeAnim->sXY = 0x100;
            gTradeAnim->alpha = 0;
            break;
        case 1:
            OpenLink();
            gMain.state ++;
            gTradeAnim->timer = 0;
            break;
        case 2:
            if (++ gTradeAnim->timer > 60)
            {
                gTradeAnim->timer = 0;
                gMain.state ++;
            }
            break;
        case 3:
            if (IsLinkMaster())
            {
                if (GetLinkPlayerCount_2() >= GetSavedPlayerCount() && ++ gTradeAnim->timer > 30)
                {
                    CheckShouldAdvanceLinkState();
                    gMain.state ++;
                }
            }
            else
            {
                gMain.state ++;
            }
            break;
        case 4:
            CheckForLinkTimeout();
            if (gReceivedRemoteLinkPlayers == TRUE && IsLinkPlayerDataExchangeComplete() == TRUE) gMain.state ++;
            break;
        case 5:
            gTradeAnim->playerFinishStatus = 0;
            gTradeAnim->partnerFinishStatus = 0;
            gTradeAnim->scheduleLinkTransfer = 0;
            LoadTradeMonPic(0, 0);
            gMain.state ++;
            break;
        case 6:
            LoadTradeMonPic(0, 1);
            gMain.state ++;
            break;
        case 7:
            LoadTradeMonPic(1, 0);
            gMain.state ++;
            break;
        case 8:
            LoadTradeMonPic(1, 1);
            gMain.state ++;
            break;
        case 9:
            LoadTradeSequenceSpriteSheetsAndPalettes();
            LoadSpriteSheet(&sPokeBallSpriteSheet);
            LoadSpritePalette(&sPokeBallSpritePalette);
            REG_BG1CNT = BGCNT_PRIORITY(2) | BGCNT_SCREENBASE(5);
            gMain.state ++;
            break;
        case 10:
            gMain.state ++;
            // fallthrough
        case 11:
            SetTradeSequenceBgGpuRegs(5);
            SetTradeSequenceBgGpuRegs(0);
            BufferTradeSceneStrings();
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
            gMain.state ++;
            break;
        case 12:
            if (!gPaletteFade.active)
            {
                SetMainCallback2(CB2_UpdateLinkTrade);
            }
            break;
    }
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}
#else
NAKED void CB2_LinkTrade(void)
{
    asm_unified("\tpush {r4-r6,lr}\n"
                    "\tsub sp, 0x4\n"
                    "\tldr r1, _0804B43C @ =gMain\n"
                    "\tldr r2, _0804B440 @ =0x0000043c\n"
                    "\tadds r0, r1, r2\n"
                    "\tldrb r0, [r0]\n"
                    "\tadds r2, r1, 0\n"
                    "\tcmp r0, 0xC\n"
                    "\tbls _0804B430\n"
                    "\tb _0804B76E_break\n"
                    "_0804B430:\n"
                    "\tlsls r0, 2\n"
                    "\tldr r1, _0804B444 @ =_0804B448\n"
                    "\tadds r0, r1\n"
                    "\tldr r0, [r0]\n"
                    "\tmov pc, r0\n"
                    "\t.align 2, 0\n"
                    "_0804B43C: .4byte gMain\n"
                    "_0804B440: .4byte 0x0000043c\n"
                    "_0804B444: .4byte _0804B448\n"
                    "\t.align 2, 0\n"
                    "_0804B448:\n"
                    "\t.4byte _0804B47C_case00\n"
                    "\t.4byte _0804B5AC_case01\n"
                    "\t.4byte _0804B5D4_case02\n"
                    "\t.4byte _0804B5FC_case03\n"
                    "\t.4byte _0804B648_case04\n"
                    "\t.4byte _0804B678_case05\n"
                    "\t.4byte _0804B6A8_case06\n"
                    "\t.4byte _0804B6B2_case07\n"
                    "\t.4byte _0804B6CC_case08\n"
                    "\t.4byte _0804B6E4_case09\n"
                    "\t.4byte _0804B71C_case10\n"
                    "\t.4byte _0804B726_case11\n"
                    "\t.4byte _0804B75C_case12\n"
                    "_0804B47C_case00:\n"
                    "\tmovs r1, 0x80\n"
                    "\tlsls r1, 19\n"
                    "\tmovs r0, 0\n"
                    "\tstrh r0, [r1]\n"
                    "\tbl ResetTasks\n"
                    "\tbl CloseLink\n"
                    "\tldr r6, _0804B570 @ =gTradeAnim\n"
                    "\tldr r5, _0804B574 @ =gSharedMem + 0x1F000\n"
                    "\tstr r5, [r6]\n"
                    "\tbl ResetSpriteData\n"
                    "\tbl FreeAllSpritePalettes\n"
                    "\tldr r0, _0804B578 @ =VBlankCB_TradeAnim\n"
                    "\tbl SetVBlankCallback\n"
                    "\tbl ClearLinkTimeoutTimer\n"
                    "\tldr r4, _0804B57C @ =gBattleMessageWindowTemplate\n"
                    "\tadds r0, r4, 0\n"
                    "\tbl Text_LoadWindowTemplate\n"
                    "\tldr r0, [r6]\n"
                    "\tadds r0, 0x4\n"
                    "\tadds r1, r4, 0\n"
                    "\tbl Text_InitWindowWithTemplate\n"
                    "\tmovs r0, 0x2\n"
                    "\tbl TextWindow_SetBaseTileNum\n"
                    "\tldr r1, [r6]\n"
                    "\tadds r1, 0x34\n"
                    "\tstrb r0, [r1]\n" // start of nonmatching
                    "\tldr r0, [r6]\n"
                    "\tadds r0, 0x4\n"
                    "\tbl TextWindow_LoadStdFrameGraphics\n"
                    "\tbl Menu_EraseScreen\n"
                    "\tldr r1, _0804B580 @ =gLinkType\n"
                    "\tldr r4, _0804B584 @ =0x00001144\n"
                    "\tadds r0, r4, 0\n"
                    "\tstrh r0, [r1]\n"
                    "\tldr r1, _0804B588 @ =gMain\n"
                    "\tldr r0, _0804B58C @ =0x0000043c\n"
                    "\tadds r1, r0\n"
                    "\tldrb r0, [r1]\n"
                    "\tadds r0, 0x1\n"
                    "\tstrb r0, [r1]\n"
                    "\tldr r0, _0804B590 @ =gBattleTextboxTiles\n"
                    "\tmovs r1, 0xC0\n"
                    "\tlsls r1, 19\n"
                    "\tbl LZDecompressVram\n"
                    "\tldr r0, _0804B594 @ =gBattleTextboxTilemap\n"
                    "\tldr r1, _0804B598 @ =0xfffe1000\n"
                    "\tadds r5, r1\n"
                    "\tmovs r2, 0x80\n"
                    "\tlsls r2, 4\n"
                    "\tadds r1, r5, 0\n"
                    "\tbl CpuSet\n"
                    "\tldr r1, _0804B59C @ =0x06002800\n"
                    "\tldr r0, _0804B5A0 @ =0x040000d4\n"
                    "\tstr r5, [r0]\n"
                    "\tstr r1, [r0, 0x4]\n"
                    "\tldr r1, _0804B5A4 @ =0x80000280\n"
                    "\tstr r1, [r0, 0x8]\n"
                    "\tldr r0, [r0, 0x8]\n"
                    "\tldr r0, _0804B5A8 @ =gBattleTextboxPalette\n"
                    "\tmovs r1, 0\n"
                    "\tmovs r2, 0x20\n"
                    "\tbl LoadCompressedPalette\n"
                    "\tldr r1, [r6]\n"
                    "\tadds r0, r1, 0\n"
                    "\tadds r0, 0xB6\n"
                    "\tmovs r2, 0\n"
                    "\tstrh r2, [r0]\n"
                    "\tadds r0, 0xE\n"
                    "\tstrh r2, [r0]\n"
                    "\tmovs r4, 0x8F\n"
                    "\tlsls r4, 1\n"
                    "\tadds r1, r4\n"
                    "\tmovs r0, 0x1\n"
                    "\tstrb r0, [r1]\n"
                    "\tldr r3, [r6]\n"
                    "\tmovs r1, 0x82\n"
                    "\tlsls r1, 1\n"
                    "\tadds r0, r3, r1\n"
                    "\tmovs r1, 0x40\n"
                    "\tstrh r1, [r0]\n"
                    "\tsubs r4, 0x18\n"
                    "\tadds r0, r3, r4\n"
                    "\tstrh r1, [r0]\n"
                    "\tadds r1, 0xC8\n"
                    "\tadds r0, r3, r1\n"
                    "\tstrh r2, [r0]\n"
                    "\tadds r4, 0x4\n"
                    "\tadds r0, r3, r4\n"
                    "\tstrh r2, [r0]\n"
                    "\tmovs r0, 0x86\n"
                    "\tlsls r0, 1\n"
                    "\tadds r1, r3, r0\n"
                    "\tmovs r0, 0x78\n"
                    "\tstrh r0, [r1]\n"
                    "\tadds r4, 0x4\n"
                    "\tadds r1, r3, r4\n"
                    "\tmovs r0, 0x50\n"
                    "\tstrh r0, [r1]\n"
                    "\tadds r0, 0xC8\n"
                    "\tadds r1, r3, r0\n"
                    "\tsubs r0, 0x18\n"
                    "\tstrh r0, [r1]\n"
                    "\tmovs r1, 0x8E\n"
                    "\tlsls r1, 1\n"
                    "\tadds r0, r3, r1\n"
                    "\tstrh r2, [r0]\n"
                    "\tb _0804B76E_break\n"
                    "\t.align 2, 0\n"
                    "_0804B570: .4byte gTradeAnim\n"
                    "_0804B574: .4byte gSharedMem + 0x1F000\n"
                    "_0804B578: .4byte VBlankCB_TradeAnim\n"
                    "_0804B57C: .4byte gBattleMessageWindowTemplate\n"
                    "_0804B580: .4byte gLinkType\n"
                    "_0804B584: .4byte 0x00001144\n"
                    "_0804B588: .4byte gMain\n"
                    "_0804B58C: .4byte 0x0000043c\n"
                    "_0804B590: .4byte gBattleTextboxTiles\n"
                    "_0804B594: .4byte gBattleTextboxTilemap\n"
                    "_0804B598: .4byte 0xfffe1000\n"
                    "_0804B59C: .4byte 0x06002800\n"
                    "_0804B5A0: .4byte 0x040000d4\n"
                    "_0804B5A4: .4byte 0x80000280\n"
                    "_0804B5A8: .4byte gBattleTextboxPalette\n"
                    "_0804B5AC_case01:\n"
                    "\tbl OpenLink\n"
                    "\tldr r1, _0804B5C8 @ =gMain\n"
                    "\tldr r2, _0804B5CC @ =0x0000043c\n"
                    "\tadds r1, r2\n"
                    "\tldrb r0, [r1]\n"
                    "\tadds r0, 0x1\n"
                    "\tmovs r2, 0\n"
                    "\tstrb r0, [r1]\n"
                    "\tldr r0, _0804B5D0 @ =gTradeAnim\n"
                    "\tldr r0, [r0]\n"
                    "\tadds r0, 0xC0\n"
                    "\tstr r2, [r0]\n"
                    "\tb _0804B76E_break\n"
                    "\t.align 2, 0\n"
                    "_0804B5C8: .4byte gMain\n"
                    "_0804B5CC: .4byte 0x0000043c\n"
                    "_0804B5D0: .4byte gTradeAnim\n"
                    "_0804B5D4_case02:\n"
                    "\tldr r0, _0804B5F4 @ =gTradeAnim\n"
                    "\tldr r0, [r0]\n"
                    "\tadds r1, r0, 0\n"
                    "\tadds r1, 0xC0\n"
                    "\tldr r0, [r1]\n"
                    "\tadds r0, 0x1\n"
                    "\tstr r0, [r1]\n"
                    "\tcmp r0, 0x3C\n"
                    "\tbhi _0804B5E8\n"
                    "\tb _0804B76E_break\n"
                    "_0804B5E8:\n"
                    "\tmovs r0, 0\n"
                    "\tstr r0, [r1]\n"
                    "\tldr r4, _0804B5F8 @ =0x0000043c\n"
                    "\tadds r1, r2, r4\n"
                    "\tb _0804B74C\n"
                    "\t.align 2, 0\n"
                    "_0804B5F4: .4byte gTradeAnim\n"
                    "_0804B5F8: .4byte 0x0000043c\n"
                    "_0804B5FC_case03:\n"
                    "\tbl IsLinkMaster\n"
                    "\tlsls r0, 24\n"
                    "\tcmp r0, 0\n"
                    "\tbne _0804B608\n"
                    "\tb _0804B746\n"
                    "_0804B608:\n"
                    "\tbl GetLinkPlayerCount_2\n"
                    "\tadds r4, r0, 0\n"
                    "\tbl GetSavedPlayerCount\n"
                    "\tlsls r4, 24\n"
                    "\tlsls r0, 24\n"
                    "\tcmp r4, r0\n"
                    "\tbcs _0804B61C\n"
                    "\tb _0804B76E_break\n"
                    "_0804B61C:\n"
                    "\tldr r0, _0804B63C @ =gTradeAnim\n"
                    "\tldr r1, [r0]\n"
                    "\tadds r1, 0xC0\n"
                    "\tldr r0, [r1]\n"
                    "\tadds r0, 0x1\n"
                    "\tstr r0, [r1]\n"
                    "\tcmp r0, 0x1E\n"
                    "\tbhi _0804B62E\n"
                    "\tb _0804B76E_break\n"
                    "_0804B62E:\n"
                    "\tbl CheckShouldAdvanceLinkState\n"
                    "\tldr r1, _0804B640 @ =gMain\n"
                    "\tldr r0, _0804B644 @ =0x0000043c\n"
                    "\tadds r1, r0\n"
                    "\tb _0804B74C\n"
                    "\t.align 2, 0\n"
                    "_0804B63C: .4byte gTradeAnim\n"
                    "_0804B640: .4byte gMain\n"
                    "_0804B644: .4byte 0x0000043c\n"
                    "_0804B648_case04:\n"
                    "\tbl CheckForLinkTimeout\n"
                    "\tldr r0, _0804B66C @ =gReceivedRemoteLinkPlayers\n"
                    "\tldrb r0, [r0]\n"
                    "\tcmp r0, 0x1\n"
                    "\tbeq _0804B656\n"
                    "\tb _0804B76E_break\n"
                    "_0804B656:\n"
                    "\tbl IsLinkPlayerDataExchangeComplete\n"
                    "\tlsls r0, 24\n"
                    "\tlsrs r0, 24\n"
                    "\tcmp r0, 0x1\n"
                    "\tbeq _0804B664\n"
                    "\tb _0804B76E_break\n"
                    "_0804B664:\n"
                    "\tldr r1, _0804B670 @ =gMain\n"
                    "\tldr r4, _0804B674 @ =0x0000043c\n"
                    "\tadds r1, r4\n"
                    "\tb _0804B74C\n"
                    "\t.align 2, 0\n"
                    "_0804B66C: .4byte gReceivedRemoteLinkPlayers\n"
                    "_0804B670: .4byte gMain\n"
                    "_0804B674: .4byte 0x0000043c\n"
                    "_0804B678_case05:\n"
                    "\tldr r2, _0804B69C @ =gTradeAnim\n"
                    "\tldr r0, [r2]\n"
                    "\tadds r0, 0x9C\n"
                    "\tmovs r1, 0\n"
                    "\tstrb r1, [r0]\n"
                    "\tldr r0, [r2]\n"
                    "\tadds r0, 0x9D\n"
                    "\tstrb r1, [r0]\n"
                    "\tldr r0, [r2]\n"
                    "\tadds r0, 0xBD\n"
                    "\tstrb r1, [r0]\n"
                    "\tmovs r0, 0\n"
                    "\tbl LoadTradeMonPic\n"
                    "\tldr r1, _0804B6A0 @ =gMain\n"
                    "\tldr r0, _0804B6A4 @ =0x0000043c\n"
                    "\tadds r1, r0\n"
                    "\tb _0804B74C\n"
                    "\t.align 2, 0\n"
                    "_0804B69C: .4byte gTradeAnim\n"
                    "_0804B6A0: .4byte gMain\n"
                    "_0804B6A4: .4byte 0x0000043c\n"
                    "_0804B6A8_case06:\n"
                    "\tmovs r0, 0\n"
                    "\tmovs r1, 0x1\n"
                    "\tbl LoadTradeMonPic\n"
                    "\tb _0804B746\n"
                    "_0804B6B2_case07:\n"
                    "\tmovs r0, 0x1\n"
                    "\tmovs r1, 0\n"
                    "\tbl LoadTradeMonPic\n"
                    "\tldr r1, _0804B6C4 @ =gMain\n"
                    "\tldr r4, _0804B6C8 @ =0x0000043c\n"
                    "\tadds r1, r4\n"
                    "\tb _0804B74C\n"
                    "\t.align 2, 0\n"
                    "_0804B6C4: .4byte gMain\n"
                    "_0804B6C8: .4byte 0x0000043c\n"
                    "_0804B6CC_case08:\n"
                    "\tmovs r0, 0x1\n"
                    "\tmovs r1, 0x1\n"
                    "\tbl LoadTradeMonPic\n"
                    "\tldr r1, _0804B6DC @ =gMain\n"
                    "\tldr r0, _0804B6E0 @ =0x0000043c\n"
                    "\tadds r1, r0\n"
                    "\tb _0804B74C\n"
                    "\t.align 2, 0\n"
                    "_0804B6DC: .4byte gMain\n"
                    "_0804B6E0: .4byte 0x0000043c\n"
                    "_0804B6E4_case09:\n"
                    "\tbl LoadTradeSequenceSpriteSheetsAndPalettes\n"
                    "\tldr r0, _0804B704 @ =sPokeBallSpriteSheet\n"
                    "\tbl LoadSpriteSheet\n"
                    "\tldr r0, _0804B708 @ =sPokeBallSpritePalette\n"
                    "\tbl LoadSpritePalette\n"
                    "\tldr r1, _0804B70C @ =REG_BG1CNT\n"
                    "\tldr r2, _0804B710 @ =0x00000502\n"
                    "\tadds r0, r2, 0\n"
                    "\tstrh r0, [r1]\n"
                    "\tldr r1, _0804B714 @ =gMain\n"
                    "\tldr r4, _0804B718 @ =0x0000043c\n"
                    "\tadds r1, r4\n"
                    "\tb _0804B74C\n"
                    "\t.align 2, 0\n"
                    "_0804B704: .4byte sPokeBallSpriteSheet\n"
                    "_0804B708: .4byte sPokeBallSpritePalette\n"
                    "_0804B70C: .4byte REG_BG1CNT\n"
                    "_0804B710: .4byte 0x00000502\n"
                    "_0804B714: .4byte gMain\n"
                    "_0804B718: .4byte 0x0000043c\n"
                    "_0804B71C_case10:\n"
                    "\tldr r0, _0804B754 @ =0x0000043c\n"
                    "\tadds r1, r2, r0\n"
                    "\tldrb r0, [r1]\n"
                    "\tadds r0, 0x1\n"
                    "\tstrb r0, [r1]\n"
                    "_0804B726_case11:\n"
                    "\tmovs r0, 0x5\n"
                    "\tbl SetTradeSequenceBgGpuRegs\n"
                    "\tmovs r0, 0\n"
                    "\tbl SetTradeSequenceBgGpuRegs\n"
                    "\tbl BufferTradeSceneStrings\n"
                    "\tmovs r0, 0x1\n"
                    "\tnegs r0, r0\n"
                    "\tmovs r1, 0\n"
                    "\tstr r1, [sp]\n"
                    "\tmovs r2, 0x10\n"
                    "\tmovs r3, 0\n"
                    "\tbl BeginNormalPaletteFade\n"
                    "_0804B746:\n"
                    "\tldr r1, _0804B758 @ =gMain\n"
                    "\tldr r2, _0804B754 @ =0x0000043c\n"
                    "\tadds r1, r2\n"
                    "_0804B74C:\n"
                    "\tldrb r0, [r1]\n"
                    "\tadds r0, 0x1\n"
                    "\tstrb r0, [r1]\n"
                    "\tb _0804B76E_break\n"
                    "\t.align 2, 0\n"
                    "_0804B754: .4byte 0x0000043c\n"
                    "_0804B758: .4byte gMain\n"
                    "_0804B75C_case12:\n"
                    "\tldr r0, _0804B788 @ =gPaletteFade\n"
                    "\tldrb r1, [r0, 0x7]\n"
                    "\tmovs r0, 0x80\n"
                    "\tands r0, r1\n"
                    "\tcmp r0, 0\n"
                    "\tbne _0804B76E_break\n"
                    "\tldr r0, _0804B78C @ =CB2_UpdateLinkTrade\n"
                    "\tbl SetMainCallback2\n"
                    "_0804B76E_break:\n"
                    "\tbl RunTasks\n"
                    "\tbl AnimateSprites\n"
                    "\tbl BuildOamBuffer\n"
                    "\tbl UpdatePaletteFade\n"
                    "\tadd sp, 0x4\n"
                    "\tpop {r4-r6}\n"
                    "\tpop {r0}\n"
                    "\tbx r0\n"
                    "\t.align 2, 0\n"
                    "_0804B788: .4byte gPaletteFade\n"
                    "_0804B78C: .4byte CB2_UpdateLinkTrade");
}
#endif

// In-game trade init
void CB2_InitInGameTrade(void)
{
    u8 otName[11];
    switch (gMain.state)
    {
        case 0:
            gSelectedTradeMonPositions[0] = gSpecialVar_0x8005;
            gSelectedTradeMonPositions[1] = 6;
            StringCopy(gLinkPlayers[0].name, gSaveBlock2.playerName);
            GetMonData(&gEnemyParty[0], MON_DATA_OT_NAME, otName);
            StringCopy(gLinkPlayers[1].name, otName);
            REG_DISPCNT = 0;
            ResetTasks();
            gTradeAnim = &ewram_2010000.tradeAnim;
            ResetSpriteData();
            FreeAllSpritePalettes();
            SetVBlankCallback(VBlankCB_TradeAnim);
            ClearLinkTimeoutTimer();
            Text_LoadWindowTemplate(&gTradeAnimationWindowTemplate);
            Text_InitWindowWithTemplate(&gTradeAnim->window, &gTradeAnimationWindowTemplate);
            gTradeAnim->textWindowBaseTileNum = TextWindow_SetBaseTileNum(2);
            TextWindow_LoadStdFrameGraphics(&gTradeAnim->window);
            Menu_EraseScreen();
            gLinkType = LINKTYPE_TRADE_DISCONNECTED;
            gTradeAnim->isLinkTrade = FALSE;
            gTradeAnim->neverRead_00B6 = 0;
            gTradeAnim->state = 0;
            gTradeAnim->texX = 0x40;
            gTradeAnim->texY = 0x40;
            gTradeAnim->neverRead_0108 = 0;
            gTradeAnim->neverRead_010A = 0;
            gTradeAnim->scrX = 0x78;
            gTradeAnim->scrY = 0x50;
            gTradeAnim->sXY = 0x100;
            gTradeAnim->alpha = 0;
            gTradeAnim->timer = 0;
            gMain.state = 5;
            break;
        case 5:
            LoadTradeMonPic(0, 0);
            gMain.state ++;
            break;
        case 6:
            LoadTradeMonPic(0, 1);
            gMain.state ++;
            break;
        case 7:
            LoadTradeMonPic(1, 0);
            gMain.state ++;
            break;
        case 8:
            LoadTradeMonPic(1, 1);
            gMain.state ++;
            break;
        case 9:
            LoadTradeSequenceSpriteSheetsAndPalettes();
            LoadSpriteSheet(&sPokeBallSpriteSheet);
            LoadSpritePalette(&sPokeBallSpritePalette);
            REG_BG1CNT = BGCNT_PRIORITY(2) | BGCNT_SCREENBASE(5);
            gMain.state ++;
            break;
        case 10:
            gMain.state ++;
            // fallthrough
        case 11:
            SetTradeSequenceBgGpuRegs(5);
            SetTradeSequenceBgGpuRegs(0);
            BufferTradeSceneStrings();
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
            gMain.state ++;
            break;
        case 12:
            if (!gPaletteFade.active)
            {
                SetMainCallback2(CB2_InGameTrade);
            }
            break;
    }
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void UpdatePokedexForReceivedMon(u8 partyIdx)
{
    struct Pokemon *mon = &gPlayerParty[partyIdx];
    if (!GetMonData(mon, MON_DATA_IS_EGG))
    {
        u16 species = SpeciesToNationalPokedexNum(GetMonData(mon, MON_DATA_SPECIES, NULL));
        GetSetPokedexFlag(species, 2);
        GetSetPokedexFlag(species, 3);
    }
}

static void TryEnableNationalDexFromLinkPartner(void)
{
    u8 mpId = GetMultiplayerId();
    if (gLinkPlayers[mpId ^ 1].lp_field_2 == 0x8000)
        EnableNationalPokedex();
}

static void TradeMons(u8 playerPartyIdx, u8 partnerPartyIdx)
{
    u8 friendship;
    struct Pokemon *playerMon = &gPlayerParty[playerPartyIdx];
    u16 playerMail = GetMonData(playerMon, MON_DATA_MAIL);

    struct Pokemon *partnerMon = &gEnemyParty[partnerPartyIdx];
    u16 partnerMail = GetMonData(partnerMon, MON_DATA_MAIL);

    if (playerMail != MAIL_NONE)
        ClearMail(&gSaveBlock1.mail[playerMail]);

    // This is where the actual trade happens!!
    gTradeAnim->tempMon = *playerMon;
    *playerMon = *partnerMon;
    *partnerMon = gTradeAnim->tempMon;

    friendship = 70;
    if (!GetMonData(playerMon, MON_DATA_IS_EGG))
        SetMonData(playerMon, MON_DATA_FRIENDSHIP, &friendship);

    if (partnerMail != MAIL_NONE)
        GiveMailToMon(playerMon, &gTradeMail[partnerMail]);

    UpdatePokedexForReceivedMon(playerPartyIdx);
    if (gReceivedRemoteLinkPlayers)
        TryEnableNationalDexFromLinkPartner();
}

static void HandleLinkDataSend(void)
{
    switch (gTradeAnim->scheduleLinkTransfer)
    {
        case 1:
            if (IsLinkTaskFinished())
            {
                Trade_SendData(gTradeAnim);
                gTradeAnim->scheduleLinkTransfer ++;
            }
            // fallthrough
        case 2:
            gTradeAnim->scheduleLinkTransfer = 0;
            break;
    }
}

static void CB2_InGameTrade(void)
{
    DoTradeAnim_Cable();
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void SetTradeSequenceBgGpuRegs(u8 state)
{
    int i;
    u16 *buffer;
    switch (state)
    {
        case 0:
            LoadPalette(gTradeGba2_Pal, 0x10, 0xa0);
            DmaCopyLarge16(3, gTradeGba_Gfx, BG_CHAR_ADDR(1), 0x1300, 0x1000);
            DmaCopy16Defvars(3, gTradePlatform_Tilemap, BG_SCREEN_ADDR(18), 0x1000);
            gTradeAnim->bg2vofs = 0;
            gTradeAnim->bg2hofs = 0xb4;
            REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG1_ON | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
            REG_BG2CNT = BGCNT_PRIORITY(2) | BGCNT_CHARBASE(1) | BGCNT_SCREENBASE(18) | BGCNT_TXT512x256;
            break;
        case 1:
            gTradeAnim->bg1hofs = 0;
            gTradeAnim->bg1vofs = 0x15c;
            REG_BG1VOFS = 0x15c;
            REG_BG1CNT = BGCNT_PRIORITY(2) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(5) | BGCNT_TXT256x512;

            DmaCopy16Defvars(3, sGbaMapCable, BG_SCREEN_ADDR(5), 0x1000);
            DmaCopyLarge16(3, gTradeGba_Gfx, BG_CHAR_ADDR(0), 0x1300, 0x1000);
            REG_DISPCNT = DISPCNT_MODE_1 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG1_ON | DISPCNT_OBJ_ON;
            break;
        case 2:
            gTradeAnim->bg1vofs = 0;
            gTradeAnim->bg1hofs = 0;
            REG_DISPCNT = DISPCNT_MODE_1 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG1_ON | DISPCNT_OBJ_ON;
            DmaCopy16Defvars(3, sCableCloseup_Map, BG_SCREEN_ADDR(5), 0x800);
            break;
        case 3:
            REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG1_ON | DISPCNT_OBJ_ON;
            gTradeAnim->texX = 0x40;
            gTradeAnim->texY = 0x40;
            gTradeAnim->scrX = 0x78;
            gTradeAnim->scrY = -0x46;
            gTradeAnim->alpha = 0;
            DmaCopyLarge16(3, sPokeballSymbol_Gfx, BG_CHAR_ADDR(1), 0x1a00, 0x1000);
            DmaCopy16Defvars(3, sPokeballSymbol_Map, BG_SCREEN_ADDR(18), 0x100);
            break;
        case 4:
            REG_DISPCNT = DISPCNT_MODE_1 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
            REG_BG2CNT = BGCNT_PRIORITY(3) | BGCNT_CHARBASE(1) | BGCNT_256COLOR | BGCNT_SCREENBASE(18);
            gTradeAnim->texX = 0x40;
            gTradeAnim->texY = 0x5c;
            gTradeAnim->sXY = 0x20;
            gTradeAnim->gbaScale = 0x400;
            gTradeAnim->alpha = 0;
            DmaCopyLarge16(3, sGbaAffine_Gfx, BG_CHAR_ADDR(1), 0x2040, 0x1000);
            DmaCopy16Defvars(3, sGbaAffineMapCable, BG_SCREEN_ADDR(18), 0x100);
            break;
        case 5:
            gTradeAnim->bg1vofs = 0;
            gTradeAnim->bg1hofs = 0;
            REG_BG1CNT = BGCNT_PRIORITY(2) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(5);
            LZDecompressVram(gBattleTextboxTiles, BG_CHAR_ADDR(0));
            CpuCopy16(gBattleTextboxTilemap, buffer = (u16 *)gSharedMem, 0x1000);
            LoadCompressedPalette(gBattleTextboxPalette, 0x70, 0x20);
            FillPalette(0, 0, 2);
            for (i = 0; i < 0x280; i ++)
                buffer[i] |= 0x7000;
            DmaCopy16Defvars(3, gSharedMem, BG_SCREEN_ADDR(5), 0x500);
            Menu_EraseWindowRect(2, 15, 27, 18);
            break;
        case 6:
            REG_DISPCNT = DISPCNT_MODE_1 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
            REG_BG2CNT = BGCNT_PRIORITY(3) | BGCNT_CHARBASE(1) | BGCNT_256COLOR | BGCNT_SCREENBASE(18);
            gTradeAnim->texX = 0x40;
            gTradeAnim->texY = 0x5c;
            gTradeAnim->sXY = 0x100;
            gTradeAnim->gbaScale = 0x80;
            gTradeAnim->scrX = 0x78;
            gTradeAnim->scrY = 0x50;
            gTradeAnim->alpha = 0;
            DmaCopyLarge16(3, sGbaAffine_Gfx, BG_CHAR_ADDR(1), 0x2040, 0x1000);
            DmaCopy16Defvars(3, sGbaAffineMapCable, BG_SCREEN_ADDR(18), 0x100);
            break;
        case 7:
            gTradeAnim->bg2vofs = 0;
            gTradeAnim->bg2hofs = 0;
            REG_BG2CNT = BGCNT_PRIORITY(2) | BGCNT_CHARBASE(1) | BGCNT_SCREENBASE(18) | BGCNT_TXT512x256;
            LoadPalette(gTradeGba2_Pal, 0x10, 0xa0);
            DmaCopyLarge16(3, gTradeGba_Gfx, BG_CHAR_ADDR(1), 0x1300, 0x1000);
            DmaCopy16Defvars(3, gTradePlatform_Tilemap, BG_SCREEN_ADDR(18), 0x1000);
            break;
    }
}

static void UpdateTradeBGAffinePan(u8 direction)
{
    if (direction == 0)
    {
        if (gTradeAnim->bg1vofs < 0x10a)
        {
            gTradeAnim->scrY ++;
            gTradeAnim->alpha += 64;
        }
    }
    else
    {
        if (gTradeAnim->scrY > -0x40)
        {
            gTradeAnim->scrY --;
            gTradeAnim->alpha += 64;
        }
    }
}

static void LoadTradeSequenceSpriteSheetsAndPalettes(void)
{
    LoadSpriteSheet(&sSpriteSheet_LinkMonGlow);
    LoadSpriteSheet(&sSpriteSheet_LinkMonShadow);
    LoadSpriteSheet(&sSpriteSheet_CableEnd);
    LoadSpriteSheet(&sSpriteSheet_GbaScreen);
    LoadSpritePalette(&sSpritePalette_LinkMon);
    LoadSpritePalette(&sSpritePalette_Gba);
}

static void BufferTradeSceneStrings(void)
{
    /*Sets the variable strings printed on the
     *actual trading screen. For use in strings
     *like "[Pokemon] will be sent to [Trainer]."
     */
    u8 mpId;
    u8 string[20];
    const struct InGameTrade *ingameTrade;
    if (gTradeAnim->isLinkTrade)
    {
        mpId = GetMultiplayerId();
        StringCopy(gStringVar1, gLinkPlayers[mpId ^ 1].name);
        GetMonData(&gEnemyParty[gSelectedTradeMonPositions[1] % 6], MON_DATA_NICKNAME, string);
        StringCopy_Nickname(gStringVar3, string);
        GetMonData(&gPlayerParty[gSelectedTradeMonPositions[0]], MON_DATA_NICKNAME, string);
        StringCopy_Nickname(gStringVar2, string);
    }
    else
    {
        ingameTrade = &sIngameTrades[gSpecialVar_0x8004];
        StringCopy(gStringVar1, ingameTrade->otName);
        StringCopy_Nickname(gStringVar3, ingameTrade->nickname);
        GetMonData(&gPlayerParty[gSpecialVar_0x8005], MON_DATA_NICKNAME, string);
        StringCopy_Nickname(gStringVar2, string);
    }
}

static bool8 DoTradeAnim_Cable(void)
{
    u16 evoTarget;

    switch (gTradeAnim->state)
    {
        case 0:
            gSprites[gTradeAnim->monSpriteIds[0]].invisible = FALSE;
            gSprites[gTradeAnim->monSpriteIds[0]].x2 = -0xb4;
            gSprites[gTradeAnim->monSpriteIds[0]].y2 = gMonFrontPicCoords[gTradeAnim->monSpecies[0]].y_offset;
            gTradeAnim->state ++;
            gTradeAnim->cachedMapMusic = GetCurrentMapMusic();
            PlayBGM(MUS_EVOLUTION);
            break;
        case 1:
            if (gTradeAnim->bg2hofs > 0)
            {
                gSprites[gTradeAnim->monSpriteIds[0]].x2 += 3;
                gTradeAnim->bg2hofs -= 3;
            }
            else
            {
                gSprites[gTradeAnim->monSpriteIds[0]].x2 = 0;
                gTradeAnim->bg2hofs = 0;
                gTradeAnim->state = 10;
            }
            break;

        case 10:
            StringExpandPlaceholders(gStringVar4, gTradeText_WillBeSent);
            Text_InitWindowAndPrintText(&gTradeAnim->window, gStringVar4, gTradeAnim->textWindowBaseTileNum, 2, 15);
            gTradeAnim->state = 11;
            gTradeAnim->timer = 0;
            break;
        case 11:
            if (++gTradeAnim->timer == 80)
            {
                gTradeAnim->releasePokeballSpriteId = CreateTradePokeballSprite(gTradeAnim->monSpriteIds[0], gSprites[gTradeAnim->monSpriteIds[0]].oam.paletteNum, 0x78, 0x20, 0x2, 0x1, 0x14, 0xfffff);
                gTradeAnim->state ++;
                Text_EraseWindowRect(&gTradeAnim->window, 0, 0, 29, 19);
                StringExpandPlaceholders(gStringVar4, gTradeText_ByeBye);
                Text_InitWindowAndPrintText(&gTradeAnim->window, gStringVar4, gTradeAnim->textWindowBaseTileNum, 2, 15);
            }
            break;
        case 12:
            if (gSprites[gTradeAnim->releasePokeballSpriteId].callback == SpriteCallbackDummy && Text_UpdateWindow(&gTradeAnim->window) == TRUE)
            {
                gTradeAnim->bouncingPokeballSpriteId = CreateSprite(&sSpriteTemplate_Pokeball, 0x78, 0x20, 0);
                gSprites[gTradeAnim->bouncingPokeballSpriteId].callback = SpriteCB_BouncingPokeballDepart;
                DestroySprite(&gSprites[gTradeAnim->releasePokeballSpriteId]);
                gTradeAnim->state ++;
            }
            break;
        case 13:
            // The game waits here for the sprite to finish its animation sequence.
            break;
        case 14:
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            gTradeAnim->state = 20;
            break;

        case 20:
            if (!gPaletteFade.active)
            {
                SetTradeSequenceBgGpuRegs(4);
                gTradeAnim->state ++;
            }
            break;
        case 21:
            BeginNormalPaletteFade(0xFFFFFFFF, -1, 16, 0, RGB(0, 0, 0));
            gTradeAnim->state ++;
            break;
        case 22:
            if (!gPaletteFade.active)
            {
                gTradeAnim->state = 23;
            }
            break;
        case 23:
            if (gTradeAnim->gbaScale > 0x100)
            {
                gTradeAnim->gbaScale -= 0x34;
            }
            else
            {
                SetTradeSequenceBgGpuRegs(1);
                gTradeAnim->gbaScale = 0x80;
                gTradeAnim->state ++;
                gTradeAnim->timer = 0;
            }
            gTradeAnim->sXY = 0x8000 / gTradeAnim->gbaScale;
            break;
        case 24:
            if (++ gTradeAnim->timer > 20)
            {
                SetTradeSequenceBgGpuRegs(3);
                SetTradeBGAffine();
                gTradeAnim->connectionSpriteId2 = CreateSprite(&sSpriteTemplate_GbaScreenFlash_Long, 0x78, 0x50, 0);
                gTradeAnim->state ++;
            }
            break;
        case 25:
            if (gSprites[gTradeAnim->connectionSpriteId2].animEnded)
            {
                DestroySprite(&gSprites[gTradeAnim->connectionSpriteId2]);
                REG_BLDCNT = 0x640;
                REG_BLDALPHA = 0x40C;
                gTradeAnim->state ++;
            }
            break;
        case 26:
            if (-- gTradeAnim->bg1vofs == 0x13C)
            {
                gTradeAnim->state ++;
            }
            if (gTradeAnim->bg1vofs == 0x148)
            {
                gTradeAnim->cableEndSpriteId = CreateSprite(&sSpriteTemplate_CableEnd, 0x80, 0x41, 0);
            }
            break;
        case 27:
            gTradeAnim->connectionSpriteId1 = CreateSprite(&sSpriteTemplate_LinkMonGlow, 0x80, 0x50, 3);
            gTradeAnim->connectionSpriteId2 = CreateSprite(&sSpriteTemplate_LinkMonShadow, 0x80, 0x50, 0);
            StartSpriteAnim(&gSprites[gTradeAnim->connectionSpriteId2], ANIM_LINKMON_SMALL);
            gTradeAnim->state ++;
            break;
        case 28:
            if ((gTradeAnim->bg1vofs -= 2) == 0xA6)
            {
                gTradeAnim->state = 200;
            }
            UpdateTradeBGAffinePan(0);
            REG_DISPCNT = DISPCNT_MODE_1 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG1_ON | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
            break;
        case 200:
            gSprites[gTradeAnim->connectionSpriteId1].y -= 2;
            gSprites[gTradeAnim->connectionSpriteId2].y -= 2;
            UpdateTradeBGAffinePan(0);
            if (gSprites[gTradeAnim->connectionSpriteId1].y < -8)
            {
                gTradeAnim->state = 29;
            }
            break;
        case 29:
            BeginNormalPaletteFade(0xFFFFFFFF, -1, 0, 16, RGB(0, 0, 0));
            gTradeAnim->state = 30;
            break;
        case 30:
            if (!gPaletteFade.active)
            {
                DestroySprite(&gSprites[gTradeAnim->connectionSpriteId1]);
                DestroySprite(&gSprites[gTradeAnim->connectionSpriteId2]);
                SetTradeSequenceBgGpuRegs(2);
                gTradeAnim->state ++;
            }
            break;
        case 31:
            BeginNormalPaletteFade(0xFFFFFFFF, -1, 16, 0, RGB(0, 0, 0));
            gTradeAnim->connectionSpriteId1 = CreateSprite(&sSpriteTemplate_LinkMonShadow, 0x6f, 0xaa, 0);
            gTradeAnim->connectionSpriteId2 = CreateSprite(&sSpriteTemplate_LinkMonShadow, 0x81, -0xa, 0);
            gTradeAnim->state ++;
            break;
        case 32:
            if (!gPaletteFade.active)
            {
                PlaySE(SE_WARP_OUT);
                gTradeAnim->state ++;
            }
            gSprites[gTradeAnim->connectionSpriteId1].y2 -= 3;
            gSprites[gTradeAnim->connectionSpriteId2].y2 += 3;
            break;
        case 33:
            gSprites[gTradeAnim->connectionSpriteId1].y2 -= 3;
            gSprites[gTradeAnim->connectionSpriteId2].y2 += 3;
            if (gSprites[gTradeAnim->connectionSpriteId1].y2 <= -0x5a)
            {
                gSprites[gTradeAnim->connectionSpriteId1].data[1] = 1;
                gSprites[gTradeAnim->connectionSpriteId2].data[1] = 1;
                gTradeAnim->state ++;
            }
            break;
        case 34:
            BlendPalettes(0x1, 16, FADE_COLOR_WHITE);
            gTradeAnim->state ++;
            break;
        case 35:
            BlendPalettes(0x1,  0, FADE_COLOR_WHITE);
            gTradeAnim->state ++;
            break;
        case 36:
            BlendPalettes(0x1, 16, FADE_COLOR_WHITE);
            gTradeAnim->state ++;
            break;
        case 37:
            if (!IsPokeSpriteNotFlipped(gTradeAnim->monSpecies[0]))
            {
                gSprites[gTradeAnim->monSpriteIds[0]].affineAnims = sAffineAnims_CrossingMonPics;
                gSprites[gTradeAnim->monSpriteIds[0]].oam.affineMode = 3;
                CalcCenterToCornerVec(&gSprites[gTradeAnim->monSpriteIds[0]], 0, 3, 3);
                StartSpriteAffineAnim(&gSprites[gTradeAnim->monSpriteIds[0]], 0);
            }
            else
            {
                StartSpriteAffineAnim(&gSprites[gTradeAnim->monSpriteIds[0]], 0);
            }
            StartSpriteAffineAnim(&gSprites[gTradeAnim->monSpriteIds[1]], 0);
            gSprites[gTradeAnim->monSpriteIds[0]].x = 0x3c;
            gSprites[gTradeAnim->monSpriteIds[1]].x = 0xb4;
            gSprites[gTradeAnim->monSpriteIds[0]].y = 0xc0;
            gSprites[gTradeAnim->monSpriteIds[1]].y = -0x20;
            gSprites[gTradeAnim->monSpriteIds[0]].invisible = FALSE;
            gSprites[gTradeAnim->monSpriteIds[1]].invisible = FALSE;
            gTradeAnim->state ++;
            break;
        case 38:
            gSprites[gTradeAnim->monSpriteIds[0]].y2 -= 3;
            gSprites[gTradeAnim->monSpriteIds[1]].y2 += 3;
            if (-0xa0 > gSprites[gTradeAnim->monSpriteIds[0]].y2 && gSprites[gTradeAnim->monSpriteIds[0]].y2 >= -0xa3)
            {
                PlaySE(SE_WARP_IN);
            }
            if (gSprites[gTradeAnim->monSpriteIds[0]].y2 < -0xde)
            {
                gSprites[gTradeAnim->connectionSpriteId1].data[1] = 0;
                gSprites[gTradeAnim->connectionSpriteId2].data[1] = 0;
                gTradeAnim->state ++;
                gSprites[gTradeAnim->monSpriteIds[0]].invisible = TRUE;
                gSprites[gTradeAnim->monSpriteIds[1]].invisible = TRUE;
                BlendPalettes(0x1, 0, FADE_COLOR_WHITE);
            }
            break;
        case 39:
            gSprites[gTradeAnim->connectionSpriteId1].y2 -= 3;
            gSprites[gTradeAnim->connectionSpriteId2].y2 += 3;
            if (gSprites[gTradeAnim->connectionSpriteId1].y2 <= -0xde)
            {
                BeginNormalPaletteFade(0xFFFFFFFF, -1, 0, 16, RGB(0, 0, 0));
                gTradeAnim->state ++;
                DestroySprite(&gSprites[gTradeAnim->connectionSpriteId1]);
                DestroySprite(&gSprites[gTradeAnim->connectionSpriteId2]);
            }
            break;
        case 40:
            if (!gPaletteFade.active)
            {
                gTradeAnim->state ++;
                SetTradeSequenceBgGpuRegs(1);
                gTradeAnim->bg1vofs = 0xa6;
                gTradeAnim->connectionSpriteId1 = CreateSprite(&sSpriteTemplate_LinkMonGlow, 0x80, -0x14, 3);
                gTradeAnim->connectionSpriteId2 = CreateSprite(&sSpriteTemplate_LinkMonShadow, 0x80, -0x14, 0);
                StartSpriteAnim(&gSprites[gTradeAnim->connectionSpriteId2], ANIM_LINKMON_SMALL);
            }
            break;
        case 41:
            BeginNormalPaletteFade(0xFFFFFFFF, -1, 16, 0, RGB(0, 0, 0));
            gTradeAnim->state ++;
            break;
        case 42:
            REG_DISPCNT = DISPCNT_MODE_1 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG1_ON | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
            UpdateTradeBGAffinePan(1);
            if (!gPaletteFade.active)
            {
                gTradeAnim->state ++;
            }
            break;
        case 43:
            UpdateTradeBGAffinePan(1);
            gSprites[gTradeAnim->connectionSpriteId1].y2 += 3;
            gSprites[gTradeAnim->connectionSpriteId2].y2 += 3;
            if (gSprites[gTradeAnim->connectionSpriteId1].y2 + gSprites[gTradeAnim->connectionSpriteId1].y == 64)
            {
                gTradeAnim->state ++;
            }
            break;
        case 44:
            UpdateTradeBGAffinePan(1);
            if ((gTradeAnim->bg1vofs += 2) > 0x13c)
            {
                gTradeAnim->bg1vofs = 0x13c;
                gTradeAnim->state ++;
            }
            break;
        case 45:
            DestroySprite(&gSprites[gTradeAnim->connectionSpriteId1]);
            DestroySprite(&gSprites[gTradeAnim->connectionSpriteId2]);
            gTradeAnim->state ++;
            gTradeAnim->timer = 0;
            break;
        case 46:
            if (++ gTradeAnim->timer == 10)
            {
                gTradeAnim->state ++;
            }
            break;
        case 47:
            if (++ gTradeAnim->bg1vofs > 0x15c)
            {
                gTradeAnim->bg1vofs = 0x15c;
                gTradeAnim->state ++;
            }
            if (gTradeAnim->bg1vofs == 0x148)
                gTradeAnim->cableEndSpriteId = CreateSprite(&sSpriteTemplate_CableEnd, 0x80, 0x41, 0);
            gSprites[gTradeAnim->cableEndSpriteId].callback = SpriteCB_CableEndReceiving;
            break;
        case 48:
            gTradeAnim->connectionSpriteId2 = CreateSprite(&sSpriteTemplate_GbaScreenFlash_Long, 0x78, 0x50, 0);
            gTradeAnim->state = 50;
            break;

        case 50:
            if (gSprites[gTradeAnim->connectionSpriteId2].animEnded)
            {
                DestroySprite(&gSprites[gTradeAnim->connectionSpriteId2]);
                SetTradeSequenceBgGpuRegs(6);
                gTradeAnim->state ++;
                PlaySE(SE_M_SAND_ATTACK);
            }
            break;
        case 51:
            if (gTradeAnim->gbaScale < 0x400)
            {
                gTradeAnim->gbaScale += 0x34;
            }
            else
            {
                gTradeAnim->gbaScale = 0x400;
                gTradeAnim->state ++;
            }
            gTradeAnim->sXY = 0x8000 / gTradeAnim->gbaScale;
            break;
        case 52:
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            gTradeAnim->state = 60;
            break;

        case 60:
            if (!gPaletteFade.active)
            {
                SetTradeSequenceBgGpuRegs(5);
                SetTradeSequenceBgGpuRegs(7);
                gTradeAnim->state ++;
            }
            break;
        case 61:
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
            gTradeAnim->state ++;
            break;
        case 62:
            REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
            if (!gPaletteFade.active)
            {
                gTradeAnim->state ++;
            }
            break;
        case 63:
            gTradeAnim->bouncingPokeballSpriteId = CreateSprite(&sSpriteTemplate_Pokeball, 0x78, -0x8, 0);
            gSprites[gTradeAnim->bouncingPokeballSpriteId].data[3] = 0x4a;
            gSprites[gTradeAnim->bouncingPokeballSpriteId].callback = SpriteCB_BouncingPokeballArrive;
            StartSpriteAnim(&gSprites[gTradeAnim->bouncingPokeballSpriteId], POKEBALL_ANIM_SPIN_TWICE);
            StartSpriteAffineAnim(&gSprites[gTradeAnim->bouncingPokeballSpriteId], POKEBALL_AFFINE_ANIM_UNSQUISH);
            BlendPalettes(1 << (16 + gSprites[gTradeAnim->bouncingPokeballSpriteId].oam.paletteNum), 16, FADE_COLOR_WHITE);
            gTradeAnim->state ++;
            gTradeAnim->timer = 0;
            break;
        case 64:
            BeginNormalPaletteFade(1 << (16 + gSprites[gTradeAnim->bouncingPokeballSpriteId].oam.paletteNum), 1, 16, 0, FADE_COLOR_WHITE);
            gTradeAnim->state ++;
            break;
        case 65:
            if (gSprites[gTradeAnim->bouncingPokeballSpriteId].callback == SpriteCallbackDummy)
            {
                gTradeAnim->state ++;
            }
            break;
        case 66:
            gSprites[gTradeAnim->monSpriteIds[1]].x = 0x78;
            gSprites[gTradeAnim->monSpriteIds[1]].y = gMonFrontPicCoords[gTradeAnim->monSpecies[1]].y_offset + 60;
            gSprites[gTradeAnim->monSpriteIds[1]].x2 = 0;
            gSprites[gTradeAnim->monSpriteIds[1]].y2 = 0;
            CreatePokeballSprite(gTradeAnim->monSpriteIds[1], gSprites[gTradeAnim->monSpriteIds[1]].oam.paletteNum, 0x78, 0x54, 2, 1, 0x14, 0xfffff);
            FreeSpriteOamMatrix(&gSprites[gTradeAnim->bouncingPokeballSpriteId]);
            DestroySprite(&gSprites[gTradeAnim->bouncingPokeballSpriteId]);
            gTradeAnim->state ++;
            break;
        case 67:
            REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG1_ON | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
            Text_EraseWindowRect(&gTradeAnim->window, 0, 0, 29, 19);
            StringExpandPlaceholders(gStringVar4, gTradeText_SentOverPoke);
            Text_InitWindowAndPrintText(&gTradeAnim->window, gStringVar4, gTradeAnim->textWindowBaseTileNum, 2, 15);
            gTradeAnim->state ++;
            gTradeAnim->timer = 0;
            break;
        case 68:
            if (++ gTradeAnim->timer == 4)
            {
                PlayFanfare(MUS_EVOLVED);
            }
            if (gTradeAnim->timer == 0xf0)
            {
                gTradeAnim->state ++;
                Text_EraseWindowRect(&gTradeAnim->window, 0, 0, 29, 19);
                StringExpandPlaceholders(gStringVar4, gTradeText_TakeGoodCare);
                Text_InitWindowAndPrintText(&gTradeAnim->window, gStringVar4, gTradeAnim->textWindowBaseTileNum, 2, 15);
                gTradeAnim->timer = 0;
            }
            break;
        case 69: // OneHand
            if (++ gTradeAnim->timer == 60)
            {
                gTradeAnim->state ++;
            }
            break;
        case 70:
            CheckPartnersMonForRibbons();
            gTradeAnim->state ++;
            break;
        case 71:
            if (gTradeAnim->isLinkTrade)
            {
                return TRUE;
            }
            else if (JOY_NEW(A_BUTTON))
            {
                gTradeAnim->state ++;
            }
            break;
        case 72: // Only if in-game trade
            TradeMons(gSpecialVar_0x8005, 0);
            gCB2_AfterEvolution = CB2_InGameTrade;
            evoTarget = GetEvolutionTargetSpecies(&gPlayerParty[gSelectedTradeMonPositions[0]], TRUE, ITEM_NONE);
            if (evoTarget != SPECIES_NONE)
                TradeEvolutionScene(&gPlayerParty[gSelectedTradeMonPositions[0]], evoTarget, gTradeAnim->monSpriteIds[1], gSelectedTradeMonPositions[0]);
            gTradeAnim->state ++;
            break;
        case 73:
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            gTradeAnim->state ++;
            break;
        case 74:
            if (!gPaletteFade.active)
            {
                PlayBGM(gTradeAnim->cachedMapMusic);
                SetMainCallback2(CB2_ReturnToField);
                BufferInGameTradeMonName();
            }
            break;
    }
    return FALSE;
}

static void CB2_TryLinkTradeEvolution(void)
{
    u16 evoTarget;
    switch (gMain.state)
    {
        case 0:
            gMain.state = 4;
            gSoftResetDisabled = TRUE;
            break;
        case 4:
            gCB2_AfterEvolution = CB2_SaveAndEndTrade;
            evoTarget = GetEvolutionTargetSpecies(&gPlayerParty[gSelectedTradeMonPositions[0]], TRUE, ITEM_NONE);
            if (evoTarget != SPECIES_NONE)
                TradeEvolutionScene(&gPlayerParty[gSelectedTradeMonPositions[0]], evoTarget, gTradeAnim->monSpriteIds[1], gSelectedTradeMonPositions[0]);
            else
                SetMainCallback2(CB2_SaveAndEndTrade);
            gSelectedTradeMonPositions[0] = 255;
            break;
    }
    if (!HasLinkErrorOccurred())
        RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void HandleLinkDataReceive(void)
{
    u8 blockReceivedStatus;
    TradeGetMultiplayerId();
    blockReceivedStatus = GetBlockReceivedStatus();
    if (blockReceivedStatus & 0x01)
    {
        if (gBlockRecvBuffer[0][0] == 0xdcba)
        {
            SetMainCallback2(CB2_TryLinkTradeEvolution);
        }
        if (gBlockRecvBuffer[0][0] == 0xabcd)
        {
            gTradeAnim->playerFinishStatus = 1;
        }
        ResetBlockReceivedFlag(0);
    }
    if (blockReceivedStatus & 0x02)
    {
        if (gBlockRecvBuffer[1][0] == 0xabcd)
        {
            gTradeAnim->partnerFinishStatus = 1;
        }
        ResetBlockReceivedFlag(1);
    }
}

static void SpriteCB_BouncingPokeball(struct Sprite *sprite)
{
    sprite->y += sprite->data[0] / 10;
    sprite->data[5] += sprite->data[1];
    sprite->x = sprite->data[5] / 10;
    if (sprite->y > 0x4c)
    {
        sprite->y = 0x4c;
        sprite->data[0] = -(sprite->data[0] * sprite->data[2]) / 100;
        sprite->data[3] ++;
    }
    if (sprite->x == 0x78)
        sprite->data[1] = 0;
    sprite->data[0] += sprite->data[4];
    if (sprite->data[3] == 4)
    {
        sprite->data[7] = 1;
        sprite->callback = SpriteCallbackDummy;
    }
}

static void SpriteCB_BouncingPokeballDepart(struct Sprite *sprite)
{
    sprite->y2 += sTradeBallVerticalVelocityTable[sprite->data[0]];
    if (sprite->data[0] == 22)
        PlaySE(SE_BALL_BOUNCE_1);
    if (++ sprite->data[0] == 44)
    {
        PlaySE(SE_M_MEGA_KICK);
        sprite->callback = SpriteCB_BouncingPokeballDepartEnd;
        sprite->data[0] = 0;
        BeginNormalPaletteFade(1 << (16 + sprite->oam.paletteNum), -1, 0, 16, FADE_COLOR_WHITE);
    }
}

static void SpriteCB_BouncingPokeballDepartEnd(struct Sprite *sprite)
{
    if (sprite->data[1] == 20)
        StartSpriteAffineAnim(sprite, 1);
    if (++ sprite->data[1] > 20)
    {
        sprite->y2 -= sTradeBallVerticalVelocityTable[sprite->data[0]];
        if (++ sprite->data[0] == 23)
        {
            DestroySprite(sprite);
            gTradeAnim->state = 14; // Resume the master trade animation
        }
    }
}

static void SpriteCB_BouncingPokeballArrive(struct Sprite *sprite)
{
    if (sprite->data[2] == 0)
    {
        if ((sprite->y += 4) > sprite->data[3])
        {
            sprite->data[2] ++;
            sprite->data[0] = 0x16;
            PlaySE(SE_BALL_BOUNCE_1);
        }
    }
    else
    {
        if (sprite->data[0] == 0x42)
            PlaySE(SE_BALL_BOUNCE_2);
        if (sprite->data[0] == 0x5c)
            PlaySE(SE_BALL_BOUNCE_3);
        if (sprite->data[0] == 0x6b)
            PlaySE(SE_BALL_BOUNCE_4);
        sprite->y2 += sTradeBallVerticalVelocityTable[sprite->data[0]];
        if (++sprite->data[0] == 0x6c)
            sprite->callback = SpriteCallbackDummy;
    }
}

u16 GetInGameTradeSpeciesInfo(void)
{
    const struct InGameTrade *inGameTrade = &sIngameTrades[gSpecialVar_0x8004];
    StringCopy(gStringVar1, gSpeciesNames[inGameTrade->requestedSpecies]);
    StringCopy(gStringVar2, gSpeciesNames[inGameTrade->species]);
    return inGameTrade->requestedSpecies;
}

static void BufferInGameTradeMonName(void)
{
    u8 nickname[32];
    const struct InGameTrade *inGameTrade = &sIngameTrades[gSpecialVar_0x8004];
    GetMonData(&gPlayerParty[gSpecialVar_0x8005], MON_DATA_NICKNAME, nickname);
    StringCopy_Nickname(gStringVar1, nickname);
    StringCopy(gStringVar2, gSpeciesNames[inGameTrade->species]);
}

static void CreateInGameTradePokemonInternal(u8 whichPlayerMon, u8 whichInGameTrade)
{
    const struct InGameTrade *inGameTrade = &sIngameTrades[whichInGameTrade];
    u8 level = GetMonData(&gPlayerParty[whichPlayerMon], MON_DATA_LEVEL);

    struct Mail mail;
    u8 metLocation = 0xFE;
    u8 mailNum;
    struct Pokemon *mon = &gEnemyParty[0];

    CreateMon(mon, inGameTrade->species, level, 32, TRUE, inGameTrade->personality, TRUE, inGameTrade->otId);

    SetMonData(mon, MON_DATA_HP_IV, &inGameTrade->ivs[0]);
    SetMonData(mon, MON_DATA_ATK_IV, &inGameTrade->ivs[1]);
    SetMonData(mon, MON_DATA_DEF_IV, &inGameTrade->ivs[2]);
    SetMonData(mon, MON_DATA_SPEED_IV, &inGameTrade->ivs[3]);
    SetMonData(mon, MON_DATA_SPATK_IV, &inGameTrade->ivs[4]);
    SetMonData(mon, MON_DATA_SPDEF_IV, &inGameTrade->ivs[5]);
    SetMonData(mon, MON_DATA_NICKNAME, inGameTrade->nickname);
    SetMonData(mon, MON_DATA_OT_NAME, inGameTrade->otName);
    SetMonData(mon, MON_DATA_OT_GENDER, &inGameTrade->otGender);
    SetMonData(mon, MON_DATA_ALT_ABILITY, &inGameTrade->abilityNum);
    SetMonData(mon, MON_DATA_BEAUTY, &inGameTrade->conditions[1]);
    SetMonData(mon, MON_DATA_CUTE, &inGameTrade->conditions[2]);
    SetMonData(mon, MON_DATA_COOL, &inGameTrade->conditions[0]);
    SetMonData(mon, MON_DATA_SMART, &inGameTrade->conditions[3]);
    SetMonData(mon, MON_DATA_TOUGH, &inGameTrade->conditions[4]);
    SetMonData(mon, MON_DATA_SHEEN, &inGameTrade->sheen);
    SetMonData(mon, MON_DATA_MET_LOCATION, &metLocation);

    mailNum = FALSE;
    if (inGameTrade->heldItem != ITEM_NONE)
    {
        if (ItemIsMail(inGameTrade->heldItem))
        {
            GetInGameTradeMail(&mail, inGameTrade);
            gTradeMail[0] = mail;
            SetMonData(mon, MON_DATA_MAIL, &mailNum);
            SetMonData(mon, MON_DATA_HELD_ITEM, &inGameTrade->heldItem);
        }
        else
        {
            SetMonData(mon, MON_DATA_HELD_ITEM, &inGameTrade->heldItem);
        }
    }
    CalculateMonStats(&gEnemyParty[0]);
}

static void GetInGameTradeMail(struct Mail *mail, const struct InGameTrade *trade) {
    s32 i;

    for (i = 0; i < 9; i++)
    {
        mail->words[i] = sIngameTradeMail[trade->mailNum][i];
    }

    StringCopy(mail->playerName, trade->otName);

#if GERMAN
    PadNameString(mail->playerName, CHAR_SPACE);
#endif

    mail->trainerId[0] = trade->otId >> 24;
    mail->trainerId[1] = trade->otId >> 16;
    mail->trainerId[2] = trade->otId >> 8;
    mail->trainerId[3] = trade->otId;
    mail->species = trade->species;
    mail->itemId = trade->heldItem;
}

u16 GetTradeSpecies(void)
{
    if (GetMonData(&gPlayerParty[gSpecialVar_0x8005], MON_DATA_IS_EGG))
        return SPECIES_NONE;
    return GetMonData(&gPlayerParty[gSpecialVar_0x8005], MON_DATA_SPECIES);
}

void CreateInGameTradePokemon(void)
{
    CreateInGameTradePokemonInternal(gSpecialVar_0x8005, gSpecialVar_0x8004);
}

void CB2_UpdateLinkTrade(void)
{
    if (DoTradeAnim_Cable() == TRUE)
    {
        DestroySprite(&gSprites[gTradeAnim->monSpriteIds[0]]);
        FreeSpriteOamMatrix(&gSprites[gTradeAnim->monSpriteIds[1]]);
        TradeMons(gSelectedTradeMonPositions[0], gSelectedTradeMonPositions[1] % 6);
        gTradeAnim->linkData[0] = 0xabcd;
        gTradeAnim->scheduleLinkTransfer = 1;
        SetMainCallback2(CB2_WaitTradeComplete);
    }
    HandleLinkDataSend();
    HandleLinkDataReceive();
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void CB2_WaitTradeComplete(void)
{
    u8 mpId = TradeGetMultiplayerId();
    HandleLinkDataReceive();
    if (mpId == 0 && gTradeAnim->playerFinishStatus == 1 && gTradeAnim->partnerFinishStatus == 1)
    {
        gTradeAnim->linkData[0] = 0xdcba;
        Trade_SendData(gTradeAnim);
        gTradeAnim->playerFinishStatus = 2;
        gTradeAnim->partnerFinishStatus = 2;
    }
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void CB2_SaveAndEndTrade(void)
{
    switch (gMain.state)
    {
        case 0:
            gTradeAnim = &ewram_2010000.tradeAnim;
            gMain.state ++;
            Text_EraseWindowRect(&gTradeAnim->window, 0, 0, 29, 19);
            StringExpandPlaceholders(gStringVar4, gOtherText_LinkStandby2);
            Text_InitWindowAndPrintText(&gTradeAnim->window, gStringVar4, gTradeAnim->textWindowBaseTileNum, 2, 15);
            break;
        case 1:
            SetLinkStandbyCallback();
            gMain.state = 100;
            gTradeAnim->timer = 0;
            break;
        case 100:
            if (++ gTradeAnim->timer > 180)
            {
                gMain.state = 101;
                gTradeAnim->timer = 0;
            }
            if (IsLinkTaskFinished())
            {
                gMain.state = 2;
            }
            break;
        case 101:
            if (IsLinkTaskFinished())
            {
                gMain.state = 2;
            }
            break;
        case 2:
            gMain.state = 50;
            Text_EraseWindowRect(&gTradeAnim->window, 0, 0, 29, 19);
            Text_InitWindowAndPrintText(&gTradeAnim->window, gSystemText_Saving, gTradeAnim->textWindowBaseTileNum, 2, 15);
            break;
        case 50:
            SetContinueGameWarpStatusToDynamicWarp();
            IncrementGameStat(GAME_STAT_POKEMON_TRADES);
            LinkFullSave_Init();
            gMain.state ++;
            gTradeAnim->timer = 0;
            break;
        case 51:
            if (++ gTradeAnim->timer == 5)
            {
                gMain.state ++;
            }
            break;
        case 52:
            if (LinkFullSave_WriteSector())
            {
                ClearContinueGameWarpStatus2();
                gMain.state = 4;
            }
            else
            {
                gTradeAnim->timer = 0;
                gMain.state = 51;
            }
            break;
        case 4:
            LinkFullSave_ReplaceLastSector();
            gMain.state = 40;
            gTradeAnim->timer = 0;
            break;
        case 40:
            if (++ gTradeAnim->timer > 50)
            {
                gTradeAnim->timer = 0;
                gMain.state = 41;
            }
            break;
        case 41:
            SetLinkStandbyCallback();
            gMain.state = 42;
            break;
        case 42:
            if (IsLinkTaskFinished())
            {
                LinkFullSave_SetLastSectorSignature();
                gSoftResetDisabled = FALSE;
                gMain.state = 5;
            }
            break;
        case 5:
            if (++ gTradeAnim->timer > 60)
            {
                gMain.state ++;
                SetLinkStandbyCallback();
            }
            break;
        case 6:
            if (IsLinkTaskFinished())
            {
                BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
                gMain.state ++;
            }
            break;
        case 7:
            if (!gPaletteFade.active)
            {
                FadeOutBGM(3);
                gMain.state ++;
            }
            break;
        case 8:
            if (IsBGMStopped() == TRUE)
            {
                SetCloseLinkCallback();
                gMain.state ++;
            }
            break;
        case 9:
            if (!gReceivedRemoteLinkPlayers)
            {
                SetMainCallback2(CB2_ReturnToTradeMenuAfterLinkTrade);
            }
            break;
    }
    if (!HasLinkErrorOccurred())
    {
        RunTasks();
    }
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void CB2_ReturnToTradeMenuAfterLinkTrade(void)
{
    if (!gPaletteFade.active)
        SetMainCallback2(CB2_StartCreateTradeMenu);
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

void DoInGameTradeScene(void)
{
    LockPlayerFieldControls();
    CreateTask(Task_InGameTrade, 10);
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
}

static void Task_InGameTrade(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(CB2_InitInGameTrade);
        gFieldCallback = FieldCB_ContinueScriptHandleMusic;
        DestroyTask(taskId);
    }
}

static void CheckPartnersMonForRibbons(void)
{
    u8 i;
    u8 numRibbons = 0;
    for (i = 0; i < 12; i ++)
    {
        numRibbons += GetMonData(&gEnemyParty[gSelectedTradeMonPositions[1] % 6], MON_DATA_CHAMPION_RIBBON + i);
    }
    if (numRibbons != 0)
        FlagSet(FLAG_SYS_RIBBON_GET);
}

void LoadTradeAnimGfx(void)
{
    LZDecompressVram(gBattleTextboxTiles, (void *)VRAM);
    CpuCopy16(gBattleTextboxTilemap, gSharedMem, 0x1000);
    DmaCopy16Defvars(3, gSharedMem, BG_SCREEN_ADDR(5), 0x500);
    LoadCompressedPalette(gBattleTextboxPalette, 0, 32);
    REG_BG1CNT = BGCNT_PRIORITY(2) | BGCNT_SCREENBASE(5);
}
