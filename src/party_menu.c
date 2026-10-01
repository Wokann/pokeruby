#include "global.h"
#include "constants/items.h"
#include "constants/item_effects.h"
#include "constants/moves.h"
#include "constants/songs.h"
#include "constants/species.h"
#include "party_menu.h"
#include "player_pc.h"
#include "battle.h"
#include "battle_interface.h"
#include "battle_party_menu.h"
#include "choose_party.h"
#include "data2.h"
#include "decompress.h"
#include "event_data.h"
#include "evolution_scene.h"
#include "item.h"
#include "item_use.h"
#include "item_menu.h"
#include "link.h"
#include "mail.h"
#include "main.h"
#include "menu.h"
#include "menu_helpers.h"
#include "palette.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "pokemon_item_effect.h"
#include "pokemon_menu.h"
#include "pokemon_summary_screen.h"
#include "battle_anim.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "scanline_effect.h"
#include "util.h"
#include "script_party_menu.h"
#include "ewram.h"

struct Coords8
{
    u8 x;
    u8 y;
};

struct PartyMenuWindowCoords
{
    u8 left;
    u8 top;
    u8 right;
    u8 bottom;
};

struct PartyMonTextSettingsStruct
{
    u8 xOffset;
    u8 yOffset;
    const u16 *oamSettings;
};

struct PartyMenuHandlersStruct
{
    /*0x0*/TaskFunc menuHandler;
    /*0x4*/bool8 (*menuSetup)(void);
    /*0x8*/u8 initialPromptTextId; // element in sPartyMenuPromptTexts
};

static void PartyMenuNoOpMonUpdate(u8 monIndex, struct Pokemon *pokemon);
static void TryPrintPartyMenuMonNickname(u8 monIndex, struct Pokemon *pokemon);
static void PartyMenuTryPrintHP(u8 monIndex, struct Pokemon *pokemon);
static void Task_AnimatePartyMenuLeftColumnSwitchIn(u8 taskId);
static void Task_AnimatePartyMenuRightColumnSwitchIn(u8 taskId);
static void FinishPartyMenuMonSwitch(u8 taskId);
static void Task_PartyMenuPrintRun(u8 taskId);
static void Task_FinishPartyMenuMoveLearning(u8 taskId);
static void ClearPartyMenuMonTextAndIcon(u8 monIndex);
static void Task_AnimatePartyMenuLeftColumnSwitchOut(u8 taskId);
static void Task_AnimatePartyMenuRightColumnSwitchOut(u8 taskId);
static void CB2_InitPartyMenu(void);
static void ReDrawPartyMonBackgrounds(void);
static void DrawPartyMenuLargeMonBackground(s16 x, u16 y, u8 isEmpty, u8 paletteNum);
static void DrawPartyMenuSmallMonBackground(s16 x, u16 y, u8 paletteNum);
static void SetPartyMenuCursorSpriteId(u8 taskId, u8 spriteId);
static void ChangeDoubleBattlePartyMenuSelection(u8 spriteId, u8 menuIndex, s8 directionPressed);
static void ChangeDefaultPartyMenuSelection(u8 spriteId, u8 menuIndex, s8 directionPressed);
void ChangeLinkDoubleBattlePartyMenuSelection(u8 spriteId, u8 menuIndex, s8 directionPressed);
static void UpdatePartyMonIconFrameAndBounce(struct Sprite *sprite);
static void UpdatePartyMonIconFrame(struct Sprite *sprite);
static void SetPartyMonIconSelected(u8 taskId, u8 monIndex, bool8 selected);
static u8 GetPartyMenuCursorSpriteId(u8 taskId);
static void SpriteCB_PartyMonIconSlide(struct Sprite *sprite);
static u8 GetMonIconSpriteId(u8 taskId, u8 monIndex);
void SpriteCB_UpdateHeldItemIconPosition(struct Sprite *sprite);
static void ItemUseMoveMenu_HandleMoveSelection(u8 taskId);
static void ItemUseMoveMenu_HandleCancel(u8 taskId);
static bool8 SetupDefaultPartyMenu(void);
/*static*/ void InitPartyMenuGpuRegs(void);
/*static*/ void VBlankCB_PartyMenu(void);
/*static*/ bool8 LoadPartyMenuGraphics(u8 a);
static void DrawPartyMenuMonBackgroundWithStatus(const u8 *coords, u8 monIndex, u8 paletteNum, bool8 selected);
static void DrawBattleTowerOkButton(u8 paletteNum);
static void DrawPartyMenuCancelButton(u8 paletteNum);

void (*gPokemonItemUseCallback)(u8 taskId, u16 itemId, TaskFunc taskFunc);

extern u16 Random();

EWRAM_DATA u8 gPartyMenuActionResult = 0;
EWRAM_DATA u8 gSelectedMonPartyId = 0;
EWRAM_DATA u8 gPartyMenuMessage_IsPrinting = 0;
EWRAM_DATA u16 gPartyMenuPreviousHeldItem = 0;
EWRAM_DATA u8 gPartyMenuType = 0;

const u16 sTmHmMoves[] =
{
    MOVE_FOCUS_PUNCH,
    MOVE_DRAGON_CLAW,
    MOVE_WATER_PULSE,
    MOVE_CALM_MIND,
    MOVE_ROAR,
    MOVE_TOXIC,
    MOVE_HAIL,
    MOVE_BULK_UP,
    MOVE_BULLET_SEED,
    MOVE_HIDDEN_POWER,
    MOVE_SUNNY_DAY,
    MOVE_TAUNT,
    MOVE_ICE_BEAM,
    MOVE_BLIZZARD,
    MOVE_HYPER_BEAM,
    MOVE_LIGHT_SCREEN,
    MOVE_PROTECT,
    MOVE_RAIN_DANCE,
    MOVE_GIGA_DRAIN,
    MOVE_SAFEGUARD,
    MOVE_FRUSTRATION,
    MOVE_SOLAR_BEAM,
    MOVE_IRON_TAIL,
    MOVE_THUNDERBOLT,
    MOVE_THUNDER,
    MOVE_EARTHQUAKE,
    MOVE_RETURN,
    MOVE_DIG,
    MOVE_PSYCHIC,
    MOVE_SHADOW_BALL,
    MOVE_BRICK_BREAK,
    MOVE_DOUBLE_TEAM,
    MOVE_REFLECT,
    MOVE_SHOCK_WAVE,
    MOVE_FLAMETHROWER,
    MOVE_SLUDGE_BOMB,
    MOVE_SANDSTORM,
    MOVE_FIRE_BLAST,
    MOVE_ROCK_TOMB,
    MOVE_AERIAL_ACE,
    MOVE_TORMENT,
    MOVE_FACADE,
    MOVE_SECRET_POWER,
    MOVE_REST,
    MOVE_ATTRACT,
    MOVE_THIEF,
    MOVE_STEEL_WING,
    MOVE_SKILL_SWAP,
    MOVE_SNATCH,
    MOVE_OVERHEAT,
    MOVE_CUT,
    MOVE_FLY,
    MOVE_SURF,
    MOVE_STRENGTH,
    MOVE_FLASH,
    MOVE_ROCK_SMASH,
    MOVE_WATERFALL,
    MOVE_DIVE,
};

//FIXME

static u8 *const sPartyMenuTileBuffer = gTileBuffer;

static const u8 sHeldItemIconGfx[] = INCBIN_U8("graphics/interface/hold_icons.4bpp");
static const u16 sHeldItemIconPal[] = INCBIN_U16("graphics/interface/hold_icons.gbapal");

static const struct SpriteSheet sHeldItemIconSpriteSheet = {
    sHeldItemIconGfx,
    sizeof sHeldItemIconGfx,
    0xd750
};

static const struct SpritePalette sHeldItemIconSpritePalette = {
    sHeldItemIconPal,
    0xd750
};

static const struct OamData sOamData_HeldItemIcon =
{
    .y = 0,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 0,
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sSpriteAnim_HeldItem[] = {
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_Mail[] = {
    ANIMCMD_FRAME(1, 1),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_HeldItemIcon[] = {
    sSpriteAnim_HeldItem,
    sSpriteAnim_Mail,
};

static const struct SpriteTemplate sSpriteTemplate_HeldItemIcon = {
    55120,
    55120,
    &sOamData_HeldItemIcon,
    sSpriteAnimTable_HeldItemIcon,
    NULL,
    gDummySpriteAffineAnimTable,
    SpriteCB_UpdateHeldItemIconPosition
};

// Texts that can be displayed in the bottom of the party menu.
static const u8 *const sPartyMenuPromptTexts[] = {
    OtherText_ChoosePoke,
    OtherText_MovePokeTo,
    OtherText_TeachWhat,
    OtherText_UseWhat,
    OtherText_GiveWhat,
    OtherText_DoWhat,
    OtherText_NothingToCut,
    OtherText_CantSurf,
    OtherText_AlreadySurfing,
    OtherText_CantUseThatHere,
    OtherText_RestoreWhatMove,
    OtherText_BoostPP,
    gOtherText_CancelWithTerminator,
    OtherText_DoWhatWithItem,
    OtherText_NoPokeForBattle,
    OtherText_ChoosePoke2,
    OtherText_NotEnoughHP,
    OtherText_ThreePokeNeeded,
    OtherText_PokeCantBeSame,
    OtherText_NoIdenticalHoldItems,
    OtherText_TeachWhichPoke,
};

static const struct Coords8 sPartyMenuMonIconCoords[8][6] = {
    {{16, 40}, {104, 18}, {104, 42}, {104, 66}, {104, 90}, {104, 114}}, // PARTY_MENU_TYPE_STANDARD
    {{16, 24}, { 16, 80}, {104, 18}, {104, 50}, {104, 82}, {104, 114}}, // PARTY_MENU_TYPE_BATTLE
    {{16, 24}, { 16, 80}, {104, 26}, {104, 50}, {104, 82}, {104, 106}}, // PARTY_MENU_TYPE_CONTEST
    {{16, 24}, {104, 26}, {104, 50}, { 16, 80}, {104, 82}, {104, 106}}, // PARTY_MENU_TYPE_IN_GAME_TRADE
    {{ 5,  4}, { 16,  1}, { 16,  4}, { 16,  7}, { 16, 10}, { 16,  13}}, // PARTY_MENU_TYPE_BATTLE_TOWER
    {{ 5,  2}, {  5,  9}, { 16,  1}, { 16,  5}, { 16,  9}, { 16,  13}}, // PARTY_MENU_TYPE_LINK_MULTI_BATTLE
    {{ 5,  2}, {  5,  9}, { 16,  2}, { 16,  5}, { 16,  9}, { 16,  12}}, // PARTY_MENU_TYPE_DAYCARE
    {{ 5,  2}, { 16,  2}, { 16,  5}, {  5,  9}, { 16,  9}, { 16,  12}}, // PARTY_MENU_TYPE_MOVE_RELEARNER
};

static const struct Coords8 sPartyMenuMonTextTileCoords[12][6] = {
    {{6,  5}, {17,  2}, {17,  5}, {17,  8}, {17, 11}, {17, 14}},
    {{6,  3}, { 6, 10}, {17,  2}, {17,  6}, {17, 10}, {17, 14}},
    {{6,  3}, { 6, 10}, {17,  3}, {17,  6}, {17, 10}, {17, 13}},
    {{6,  3}, {17,  3}, {17,  6}, { 6, 10}, {17, 10}, {17, 13}},
    {{3,  7}, {22,  2}, {22,  5}, {22,  8}, {22, 11}, {22, 14}},
    {{3,  5}, { 3, 12}, {22,  2}, {22,  6}, {22, 10}, {22, 14}},
    {{3,  5}, { 3, 12}, {22,  3}, {22,  6}, {22, 10}, {22, 13}},
    {{3,  5}, {22,  3}, {22,  6}, { 3, 12}, {22, 10}, {22, 13}},
    {{7,  7}, {26,  2}, {26,  5}, {26,  8}, {26, 11}, {26, 14}},
    {{7,  5}, { 7, 12}, {26,  2}, {26,  6}, {26, 10}, {26, 14}},
    {{7,  5}, { 7, 12}, {26,  3}, {26,  6}, {26, 10}, {26, 13}},
    {{7,  5}, {26,  3}, {26,  6}, { 7, 12}, {26, 10}, {26, 13}},
};

static u16 *const sPartyMenuHpBarTilemapPtrs[4][6] = {
    {(u16*)(BG_VRAM + 0xF1C8), (u16*)(BG_VRAM + 0xF0AE), (u16*)(BG_VRAM + 0xF16E), (u16*)(BG_VRAM + 0xF22E), (u16*)(BG_VRAM + 0xF2EE), (u16*)(BG_VRAM + 0xF3AE)}, // PARTY_MENU_STANDARD
    {(u16*)(BG_VRAM + 0xF148), (u16*)(BG_VRAM + 0xF308), (u16*)(BG_VRAM + 0xF0AE), (u16*)(BG_VRAM + 0xF1AE), (u16*)(BG_VRAM + 0xF2AE), (u16*)(BG_VRAM + 0xF3AE)}, // PARTY_MENU_LAYOUT_DOUBLE_BATTLE
    {(u16*)(BG_VRAM + 0xF148), (u16*)(BG_VRAM + 0xF308), (u16*)(BG_VRAM + 0xF0EE), (u16*)(BG_VRAM + 0xF1AE), (u16*)(BG_VRAM + 0xF2AE), (u16*)(BG_VRAM + 0xF36E)}, // PARTY_MENU_LAYOUT_LINK_DOUBLE_BATTLE
    {(u16*)(BG_VRAM + 0xF148), (u16*)(BG_VRAM + 0xF0EE), (u16*)(BG_VRAM + 0xF1AE), (u16*)(BG_VRAM + 0xF308), (u16*)(BG_VRAM + 0xF2AE), (u16*)(BG_VRAM + 0xF36E)}, // PARTY_MENU_LAYOUT_MULTI_BATTLE
};

static const struct Coords8 sPartyMenuCursorCoords[3][8] = {
    {{8,  44}, {92,  22}, {92,  46}, {92,  70}, {92,  94}, {92, 118}, {196, 136}, {196, 152}}, // PARTY_MENU_LAYOUT_STANDARD
    {{8,  28}, { 8,  84}, {92,  22}, {92,  54}, {92,  86}, {92, 118}, {196, 136}, {196, 152}}, // PARTY_MENU_LAYOUT_DOUBLE_BATTLE
    {{8,  28}, { 8,  84}, {92,  30}, {92,  54}, {92,  86}, {92, 110}, {196, 136}, {196, 152}}, // PARTY_MENU_LAYOUT_LINK_DOUBLE_BATTLE
};

static u16 *const sPartyMenuDescriptorStatusTilemapPtrs[2][PARTY_SIZE] = {
    {(u16*)(BG_VRAM + 0xF1C6), (u16*)(BG_VRAM + 0xF06C), (u16*)(BG_VRAM + 0xF12C), (u16*)(BG_VRAM + 0xF1EC), (u16*)(BG_VRAM + 0xF2AC), (u16*)(BG_VRAM + 0xF36C)},
    {(u16*)(BG_VRAM + 0xF148), (u16*)(BG_VRAM + 0xF308), (u16*)(BG_VRAM + 0xF0AE), (u16*)(BG_VRAM + 0xF1AE), (u16*)(BG_VRAM + 0xF2AE), (u16*)(BG_VRAM + 0xF3AE)},
};

static const struct PartyMenuWindowCoords sPartyMenuMonTextWindowCoords[2][6] = {
    {{2, 4, 10, 9}, {16, 1, 29,  3}, {16, 4, 29, 6}, {16, 7, 29, 9}, {16, 10, 29, 12}, {16, 13, 29, 15}},
    {{2, 2, 10, 7}, { 2, 9, 10, 14}, {16, 1, 29, 3}, {16, 5, 29, 7}, {16,  9, 29, 11}, {16, 13, 29, 15}},
};

static const struct PartyMenuWindowCoords sPartyMenuDescriptorStatusWindowCoords[2][6] = {
    {{2, 7, 10,  9}, {21, 1, 29,  3}, {21, 4, 29,  6}, {21, 7, 29, 9}, {21, 10, 29, 12}, {21, 13, 29, 15}},
    {{2, 2, 10,  7}, { 2, 9, 10, 14}, {16, 1, 29,  3}, {16, 5, 29, 7}, {16,  9, 29, 11}, {16, 13, 29, 15}},
};

// This is actually a 2x6x2 array, but the code reads it as a flat array.
static const u8 sPartyMenuMonBackgroundCoords[] = {
    0, 3,   11, 1,   11, 4,    11, 7,    11, 10,    11, 13,
    0, 1,    0, 8,   11, 1,    11, 5,    11,  9,    11, 13, // Double battle
};

// This is actually a 2x6x2 array, but the code reads it as a flat array.
//FIXME: DrawLinkMultiBattlePartyMonBackgrounds() accesses this data via
// sPartyMenuMonBackgroundCoords (directly above this). This means these
// two arrays might be a struct, rather than separate arrays.
static const u8 sLinkMultiBattlePartyMonBackgroundCoords[] = {
    0, 1,    0, 8,   11, 2,    11, 5,    11, 9,    11, 12,
    0, 1,    0, 8,   11, 2,    11, 5,    11, 9,    11, 12, // Double battle
};

static const u8 sPartyMenuSmallMonBackgroundTilemap[] = {
    0x24,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x25,0x27,
    0x34,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x37,
    0x34,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x37,
    0x34,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x35,0x37,
    0x44,0x45,0x45,0x45,0x45,0x45,0x45,0x45,0x45,0x45,0x47,
    0x44,0x45,0x45,0x45,0x45,0x45,0x45,0x45,0x45,0x45,0x47,
    0x54,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x57,
};

static const u8 sPartyMenuLargeMonBackgroundTilemap[] = {
    0x50,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x51,0x53,
    0x60,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x61,0x63,
    0x70,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x71,0x73,
};

static const u8 sPartyMenuLargeEmptyMonBackgroundTilemap[] = {
    0x20,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x21,0x23,
    0x30,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x33,
    0x40,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x43,
};

static const u8 sUnusedPartyMenuData[] = {
    0x0C,0x0D,0x0E,0x0F,0x00,0x01,0x02,0x03,0x04,0x05,0x0F,
    0x10,0x11,0x12,0x13,0x14,0x15,0x0F,0x06,0x05,0x01,0x07,
    0x08,0x09,0x0F,0x16,0x15,0x11,0x17,0x18,0x19,0x0F,0x09,
    0x0A,0x0B,0x05,0x0C,0x0F,0x0F,0x19,0x1A,0x1B,0x15,0x1C,
    0x0F,0x0F,0x0D,0x0B,0x05,0x0C,0x0F,0x0F,0x0F,0x1D,0x1B,
    0x15,0x1C,0x0F,0x0F,0x0F,0x06,0x05,0x0B,0x05,0x0C,0x0F,
    0x0F,0x16,0x15,0x1B,0x15,0x1C,0x0F,0x0F,0x0F,0x0F,0x20,
    0x0C,0x09,0x0F,0x0F,0x0F,0x1F,0x30,0x1C,0x19,0x0F,0x0F,
    0x0F,0x25,0x0F,0x22,0x24,0x0F,0x0F,0x0F,0x35,0x41,0x32,
    0x34,0x0F,0x0F,0x0F,0x26,0x0F,0x23,0x0E,0x0F,0x0F,0x0F,
    0x36,0x43,0x33,0x1E,0x0F,0x0F,0x27,0x28,0x29,0x03,0x2A,
    0x0F,0x0F,0x37,0x38,0x39,0x13,0x3A,0x0F,0x0F,0x27,0x28,
    0x29,0x03,0x04,0x05,0x0F,0x37,0x38,0x39,0x13,0x14,0x15,
    0x0F,0x2B,0x2C,0x02,0x28,0x29,0x2D,0x21,0x3B,0x3C,0x12,
    0x38,0x39,0x3D,0x31,0x2B,0x2C,0x02,0x28,0x29,0x08,0x09,
    0x3B,0x3C,0x12,0x38,0x39,0x18,0x19,0x2E,0x2F,0x2B,0x2C,
    0x02,0x27,0x2D,0x3E,0x3F,0x3B,0x3C,0x12,0x45,0x3D,
};

static const TaskFunc sItemUseMoveMenuHandlers[] = {
    ItemUseMoveMenu_HandleMoveSelection,
    ItemUseMoveMenu_HandleCancel,
};

#define PartyMonOAMSettings(x, y, palette, shape, size, priority, tileOffset) \
    ((shape)   << 14) | (y),                                                  \
    ((size)    << 14) | (x),                                                  \
    ((palette) << 12) | ((priority) << 10) | (tileOffset)

static const u16 sPartyMonOamSettings_LeftColumn[] = {
    PartyMonOAMSettings(24,  0, 15, ST_OAM_H_RECTANGLE, 1, 1,  0),
    PartyMonOAMSettings(56,  0, 15, ST_OAM_H_RECTANGLE, 1, 1,  4),
    PartyMonOAMSettings(24,  8, 15, ST_OAM_H_RECTANGLE, 1, 1,  8),
    PartyMonOAMSettings(56,  8, 15, ST_OAM_H_RECTANGLE, 1, 1, 12),
    PartyMonOAMSettings(32, 16, 15, ST_OAM_H_RECTANGLE, 1, 1, 16),
    PartyMonOAMSettings(37, 32, 15, ST_OAM_H_RECTANGLE, 1, 1, 24),
    PartyMonOAMSettings(69, 32, 15, ST_OAM_H_RECTANGLE, 1, 1, 28),
    0xFFFF,
};

static const u16 sPartyMonOamSettings_RightColumn[] = {
    PartyMonOAMSettings( 24,  0, 15, ST_OAM_H_RECTANGLE, 1, 1,  0),
    PartyMonOAMSettings( 56,  0, 15, ST_OAM_H_RECTANGLE, 1, 1,  4),
    PartyMonOAMSettings( 24,  8, 15, ST_OAM_H_RECTANGLE, 1, 1,  8),
    PartyMonOAMSettings( 56,  8, 15, ST_OAM_H_RECTANGLE, 1, 1, 12),
    PartyMonOAMSettings( 32, 16, 15, ST_OAM_H_RECTANGLE, 1, 1, 16),
    PartyMonOAMSettings(101, 16, 15, ST_OAM_H_RECTANGLE, 1, 1, 24),
    PartyMonOAMSettings(133, 16, 15, ST_OAM_H_RECTANGLE, 1, 1, 28),
    0xFFFF,
};

// Controls where and how the mons' text appears in the party menu screen (nickname, HP, and level).
static const struct PartyMonTextSettingsStruct sPartyMonTextSettings[4][6] =
{
    { // PARTY_MENU_LAYOUT_STANDARD
        { 1,  4, sPartyMonOamSettings_LeftColumn},
        {12,  1, sPartyMonOamSettings_RightColumn},
        {12,  4, sPartyMonOamSettings_RightColumn},
        {12,  7, sPartyMonOamSettings_RightColumn},
        {12, 10, sPartyMonOamSettings_RightColumn},
        {12, 13, sPartyMonOamSettings_RightColumn},
    },
    { // PARTY_MENU_LAYOUT_DOUBLE_BATTLE
        { 1,  2, sPartyMonOamSettings_LeftColumn},
        { 1,  9, sPartyMonOamSettings_LeftColumn},
        {12,  1, sPartyMonOamSettings_RightColumn},
        {12,  5, sPartyMonOamSettings_RightColumn},
        {12,  9, sPartyMonOamSettings_RightColumn},
        {12, 13, sPartyMonOamSettings_RightColumn},
    },
    { // PARTY_MENU_LAYOUT_LINK_DOUBLE_BATTLE
        { 1,  2, sPartyMonOamSettings_LeftColumn},
        { 1,  9, sPartyMonOamSettings_LeftColumn},
        {12,  2, sPartyMonOamSettings_RightColumn},
        {12,  5, sPartyMonOamSettings_RightColumn},
        {12,  9, sPartyMonOamSettings_RightColumn},
        {12, 12, sPartyMonOamSettings_RightColumn},
    },
    { // PARTY_MENU_LAYOUT_MULTI_BATTLE
        { 1,  2, sPartyMonOamSettings_LeftColumn},
        {12,  2, sPartyMonOamSettings_RightColumn},
        {12,  5, sPartyMonOamSettings_RightColumn},
        { 1,  9, sPartyMonOamSettings_LeftColumn},
        {12,  9, sPartyMonOamSettings_RightColumn},
        {12, 12, sPartyMonOamSettings_RightColumn},
    },
};

static const struct PartyMenuHandlersStruct sPartyMenuHandlers[] =
{
    {HandleDefaultPartyMenu,         SetupDefaultPartyMenu,            0}, // PARTY_MENU_TYPE_STANDARD
    {HandleBattlePartyMenu,          SetUpBattlePartyMenu,             0}, // PARTY_MENU_TYPE_BATTLE
    {HandleSelectPartyMenu,          SetupContestPartyMenu,            0}, // PARTY_MENU_TYPE_CONTEST
    {HandleSelectPartyMenu,          SetupDefaultPartyMenu,            0}, // PARTY_MENU_TYPE_IN_GAME_TRADE
    {HandleBattleTowerPartyMenu,     SetupBattleTowerPartyMenu,        0}, // PARTY_MENU_TYPE_BATTLE_TOWER
    {HandleLinkMultiBattlePartyMenu, SetupLinkMultiBattlePartyMenu, 0xFF}, // PARTY_MENU_TYPE_LINK_MULTI_BATTLE
    {HandleDaycarePartyMenu,         SetupDefaultPartyMenu,         0x0F}, // PARTY_MENU_TYPE_DAYCARE
    {HandleMoveRelearnerPartyMenu,   SetupMoveRelearnerPartyMenu,      0}, // PARTY_MENU_TYPE_MOVE_RELEARNER
};

extern u16 gBattleTypeFlags;
extern u8 gLastFieldPokeMenuOpened;
extern u8 gPlayerPartyCount;
extern s32 gBattleMoveDamage;
extern u16 gMoveToLearn;

// The original consumer indexes this byte table as u16 values; keep that access width for a matching build.
extern u16 gPartyMenuDescriptorStatusTilemap[];
extern struct Window gWindowTemplate_Contest_MoveDescription;

extern const u8 gPartyMenuMisc_Gfx[];
extern const u8 gPartyMenuMisc_Tilemap[];
extern const u8 gPartyMenuMisc_Pal[];
extern const u8 gFontDefaultPalette[];
extern const u8 gPartyMenuHpBar_Gfx[];
extern const u8 gPartyMenuOrderText_Gfx[];
extern const u8 gStatusGfx_Icons[];
extern const u8 gStatusPal_Icons[];

#if ENGLISH
#define WINDOW_LEFT (3)
#define WINDOW_RIGHT (26)
#elif GERMAN
#define WINDOW_LEFT (0)
#define WINDOW_RIGHT (29)
#endif

void CB2_PartyMenuMain(void)
{
    const struct PartyMonTextSettingsStruct *textSettings;
    s32 i;

    AnimateSprites();
    BuildOamBuffer();

    textSettings = sPartyMonTextSettings[gPartyMenuType];
    for (i = 0; i < PARTY_SIZE; i++)
    {
        // Draw mon name, level, and hp sprites
        DrawPartyMenuMonText(
            textSettings->xOffset * 8,
            textSettings->yOffset * 8,
            textSettings->oamSettings,
            0,
            (i << 5) | 0x200);
        textSettings++;
    }

    RunTasks();
    UpdatePaletteFade();

#if DEBUG
    if (gLinkOpen == TRUE)
        PrintDebugHex(gLink.recvQueue.count, 1, 1, 2, 2);
#endif
}

void VBlankCB_PartyMenu(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    ReDrawPartyMonBackgrounds();
}

void SetPartyMenuSettings(u8 menuType, u8 battleTypeFlags, TaskFunc menuHandlerFunc, u8 textId)
{
    if (battleTypeFlags != 0xFF)
    {
        gBattleTypeFlags = battleTypeFlags;
    }

    ePartyMenu2.menuType = menuType;
    ePartyMenu2.menuHandler = menuHandlerFunc;
    ePartyMenu2.promptTextId = textId;
}

void DoOpenPartyMenu(u8 menuType, u8 battleFlags, TaskFunc menuHandlerFunc, u8 textId)
{
    SetPartyMenuSettings(menuType, battleFlags, menuHandlerFunc, textId);
    SetMainCallback2(CB2_InitPartyMenu);
}

void OpenPartyMenu(u8 menuType, u8 battleFlags)
{
    DoOpenPartyMenu(menuType, battleFlags, sPartyMenuHandlers[menuType].menuHandler, sPartyMenuHandlers[menuType].initialPromptTextId);
}

// This is a Task which is repeatedly called until it eventually returns TRUE when finished.
bool8 SetupDefaultPartyMenu(void)
{
    switch (ePartyMenu2.pmSetupState)
    {
    case 0:
        if (ePartyMenu2.pmMonIndex < gPlayerPartyCount)
        {
            TryCreatePartyMenuMonIcon(ePartyMenu2.menuHandlerTaskId,
                ePartyMenu2.pmMonIndex, &gPlayerParty[ePartyMenu2.pmMonIndex]);
            ePartyMenu2.pmMonIndex++;
        }
        else
        {
            ePartyMenu2.pmMonIndex = 0;
            ePartyMenu2.pmSetupState++;
        }
        break;
    case 1:
        LoadHeldItemIconGraphics();
        ePartyMenu2.pmSetupState++;
        break;
    case 2:
        CreatePartyMenuHeldItemIcons(ePartyMenu2.menuHandlerTaskId);
        ePartyMenu2.pmSetupState++;
        break;
    case 3:
        if (CreatePartyMenuCursor(ePartyMenu2.menuHandlerTaskId, ePartyMenu2.pmMonIndex) != 1)
        {
            ePartyMenu2.pmMonIndex++;
        }
        else
        {
            ePartyMenu2.pmMonIndex = 0;
            ePartyMenu2.pmSetupState++;
        }
        break;
    case 4:
        PartyMenuPrintMonsLevelOrStatus();
        ePartyMenu2.pmSetupState++;
        break;
    case 5:
        PrintPartyMenuMonNicknames();
        ePartyMenu2.pmSetupState++;
        break;
    case 6:
        PartyMenuTryPrintMonsHP();
        ePartyMenu2.pmSetupState++;
        break;
    case 7:
        PartyMenuNoOpSetupStep();
        ePartyMenu2.pmSetupState++;
        break;
    case 8:
        PartyMenuDrawHPBars();
        ePartyMenu2.pmSetupState++;
        break;
    case 9:
        if (DrawPartyMonBackground(ePartyMenu2.pmMonIndex) == 1)
        {
            ePartyMenu2.pmMonIndex = 0;
            ePartyMenu2.pmSetupState = 0;
            return TRUE;
        }
        else
        {
            ePartyMenu2.pmMonIndex++;
            break;
        }
    }

    return FALSE;
}

bool8 InitPartyMenu(void)
{
    switch (gMain.state)
    {
    case 0:
        SetVBlankCallback(NULL);
        DmaFill16Large(3, 0, (void *)(VRAM + 0x0), VRAM_SIZE, 0x1000);
        DmaClear32(3, OAM, OAM_SIZE);
        DmaClear16(3, PLTT, PLTT_SIZE);
        gPaletteFade.bufferTransferDisabled = 1;
        gMain.state++;
        break;
    case 1:
        ScanlineEffect_Stop();
        gMain.state++;
        break;
    case 2:
        InitPartyMenuGpuRegs();
        ePartyMenu2.pmSetupState = 0;
        ePartyMenu2.pmMonIndex = 0;
        ePartyMenu2.pmUnusedData2 = 0;
        gMain.state++;
        break;
    case 3:
        ResetSpriteData();
        gMain.state++;
        break;
    case 4:
        if (ePartyMenu2.menuType != PARTY_MENU_TYPE_BATTLE &&
            ePartyMenu2.menuType != PARTY_MENU_TYPE_LINK_MULTI_BATTLE)
            ResetTasks();
        gMain.state++;
        break;
    case 5:
        FreeAllSpritePalettes();
        gMain.state++;
        break;
    case 6:
        ePartyMenu2.menuHandlerTaskId = CreateTask(ePartyMenu2.menuHandler, 0);
        gMain.state++;
        break;
    case 7:
        Text_LoadWindowTemplate(&gPartyMenuWindowTemplate);
        gMain.state++;
        break;
    case 8:
        Text_InitWindowWithTemplate(&gWindowTemplate_Contest_MoveDescription, &gPartyMenuWindowTemplate);
        MultistepInitWindowTileData(&gWindowTemplate_Contest_MoveDescription, 1);
        gMain.state++;
        break;
    case 9:
        if (MultistepLoadFont())
        {
            ePartyMenu2.pmSetupState = 1;
            gMain.state++;
        }
        break;
    case 10:
        if (LoadPartyMenuGraphics(ePartyMenu2.pmSetupState) == TRUE)
        {
            ePartyMenu2.pmSetupState = 0;
            gMain.state++;
        }
        else
        {
            ePartyMenu2.pmSetupState++;
        }
        break;
    case 11:
        LoadMonIconPalettes();
        gMain.state++;
        break;
    case 12:
        if (sPartyMenuHandlers[ePartyMenu2.menuType].menuSetup() == TRUE)
            gMain.state++;
        break;
    case 13:
        MultistepInitMenuWindowBegin(&gPartyMenuMenuWindowTemplate);
        gMain.state++;
        break;
    case 14:
        if (MultistepInitMenuWindowContinue())
            gMain.state++;
        break;
    case 15:
        PrintPartyMenuPromptText(ePartyMenu2.promptTextId, 0);
        gMain.state++;
        break;
    case 16:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        gPaletteFade.bufferTransferDisabled = 0;
        gMain.state++;
        break;
    case 17:
        SetVBlankCallback(VBlankCB_PartyMenu);
#if DEBUG
        if (gLinkOpen == TRUE)
            InitDebugHexPrinter((void *)(VRAM + 0xE5E0), 0x8000, (void *)(VRAM + 0x7800), 2);
#endif
        return TRUE;
    }

    return FALSE;
}

void CB2_InitPartyMenu(void)
{
    while (InitPartyMenu() != TRUE)
    {
        if (MenuHelpers_IsLinkActive() == TRUE)
            return;
    }

    if (ePartyMenu2.menuType != PARTY_MENU_TYPE_LINK_MULTI_BATTLE)
        ChangePartyMenuSelection(ePartyMenu2.menuHandlerTaskId, 0);

    SetMainCallback2(CB2_PartyMenuMain);
}

void InitPartyMenuGpuRegs(void)
{
    SetHBlankCallback(NULL);
    REG_DISPCNT = 8000;
    REG_BG0CNT = 0x1E05;
    REG_BG1CNT = 0x703;
    REG_BG2CNT = 0xF08;
    REG_BG3CNT = 0x602;
    REG_BLDCNT = 0;
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;
    REG_BG1HOFS = 0;
    REG_BG1VOFS = 0;
    REG_BG2HOFS = 0;
    REG_BG2VOFS = 0;
    REG_BG3HOFS = 0;
    REG_BG3VOFS = 0;
    REG_BG3VOFS = -1;
}

bool8 IsLinkDoubleBattle()
{
    if ((gBattleTypeFlags & BATTLE_TYPE_LINK_DOUBLE) == BATTLE_TYPE_LINK_DOUBLE)
        return TRUE;
    else
        return FALSE;
}

// Draws the blue rectangular regions surrounding each of the party mons.
void ReDrawPartyMonBackgrounds(void)
{
    if (ePartyMenu2.backgroundRedrawState)
    {
        DmaCopy16Defvars(3, gBGTilemapBuffers[2], (void *)(BG_VRAM + 0x3000), 0x800);

        if (ePartyMenu2.backgroundRedrawState == 2)
            ePartyMenu2.backgroundRedrawState = 0;
    }
}

bool8 DrawPartyMonBackground(u8 monIndex)
{
    const u8 *arr;

    if (!IsDoubleBattle())
        gPartyMenuType = PARTY_MENU_LAYOUT_STANDARD;
    else if (IsLinkDoubleBattle() == TRUE)
        gPartyMenuType = PARTY_MENU_LAYOUT_LINK_DOUBLE_BATTLE;
    else
        gPartyMenuType = PARTY_MENU_LAYOUT_DOUBLE_BATTLE;

    arr = &sPartyMenuMonBackgroundCoords[gPartyMenuType * 12];

    switch (monIndex)
    {
    case 0:
        memset(&gBGTilemapBuffers[2], 0, 0x800);
        break;
    case 1:
        DrawPartyMenuSmallMonBackground(arr[0], arr[1], 3);
        DrawPartyMenuMonBackgroundWithStatus(&arr[0], 0, 3, FALSE);
        break;
    case 2:
        if (!IsDoubleBattle())
        {
            if (gPlayerPartyCount > 1)
            {
                DrawPartyMenuLargeMonBackground(arr[2], arr[3], FALSE, 3);
                DrawPartyMenuMonBackgroundWithStatus(&arr[2], 1, 3, FALSE);
            }
            else
            {
                DrawPartyMenuLargeMonBackground(arr[2], arr[3], TRUE, 3);
            }
        }
        else if (IsLinkDoubleBattle() == TRUE)
        {
            DrawPartyMenuSmallMonBackground(arr[2], arr[3], 4);
            DrawPartyMenuMonBackgroundWithStatus(&arr[2], 1, 4, FALSE);
        }
        else
        {
            DrawPartyMenuSmallMonBackground(arr[2], arr[3], 3);
            DrawPartyMenuMonBackgroundWithStatus(&arr[2], 1, 3, FALSE);
        }
        break;
    case 3:
        if (!IsDoubleBattle())
        {
            if (gPlayerPartyCount > 2)
            {
                DrawPartyMenuLargeMonBackground(arr[4], arr[5], FALSE, 3);
                DrawPartyMenuMonBackgroundWithStatus(&arr[4], 2, 3, FALSE);
            }
            else
            {
                DrawPartyMenuLargeMonBackground(arr[4], arr[5], TRUE, 3);
            }
        }
        else if (IsLinkDoubleBattle() == TRUE)
        {
            if (GetMonData(&gPlayerParty[2], MON_DATA_SPECIES))
            {
                DrawPartyMenuLargeMonBackground(arr[4], arr[5], FALSE, 3);
                DrawPartyMenuMonBackgroundWithStatus(&arr[4], 2, 3, FALSE);
            }
            else
            {
                DrawPartyMenuLargeMonBackground(arr[4], arr[5], TRUE, 3);
            }
        }
        else if (gPlayerPartyCount > 2)
        {
            DrawPartyMenuLargeMonBackground(arr[4], arr[5], FALSE, 3);
            DrawPartyMenuMonBackgroundWithStatus(&arr[4], 2, 3, FALSE);
        }
        else
        {
            DrawPartyMenuLargeMonBackground(arr[4], arr[5], TRUE, 3);
        }
        break;
    case 4:
        if (!IsDoubleBattle())
        {
            if (gPlayerPartyCount > 3)
            {
                DrawPartyMenuLargeMonBackground(arr[6], arr[7], FALSE, 3);
                DrawPartyMenuMonBackgroundWithStatus(&arr[6], 3, 3, FALSE);
            }
            else
            {
                DrawPartyMenuLargeMonBackground(arr[6], arr[7], TRUE, 3);
            }
        }
        else if (IsLinkDoubleBattle() == TRUE)
        {
            if (GetMonData(&gPlayerParty[3], MON_DATA_SPECIES))
            {
                DrawPartyMenuLargeMonBackground(arr[6], arr[7], FALSE, 3);
                DrawPartyMenuMonBackgroundWithStatus(&arr[6], 3, 3, FALSE);
            }
            else
            {
                DrawPartyMenuLargeMonBackground(arr[6], arr[7], TRUE, 3);
            }
        }
        else if (gPlayerPartyCount > 3)
        {
            DrawPartyMenuLargeMonBackground(arr[6], arr[7], FALSE, 3);
            DrawPartyMenuMonBackgroundWithStatus(&arr[6], 3, 3, FALSE);
        }
        else
        {
            DrawPartyMenuLargeMonBackground(arr[6], arr[7], TRUE, 3);
        }
        break;
    case 5:
        if (!IsDoubleBattle())
        {
            if (gPlayerPartyCount > 4)
            {
                DrawPartyMenuLargeMonBackground(arr[8], arr[9], FALSE, 3);
                DrawPartyMenuMonBackgroundWithStatus(&arr[8], 4, 3, FALSE);
            }
            else
            {
                DrawPartyMenuLargeMonBackground(arr[8], arr[9], TRUE, 3);
            }
        }
        else if (IsLinkDoubleBattle() == TRUE)
        {
            if (GetMonData(&gPlayerParty[4], MON_DATA_SPECIES))
            {
                DrawPartyMenuLargeMonBackground(arr[8], arr[9], FALSE, 4);
                DrawPartyMenuMonBackgroundWithStatus(&arr[8], 4, 4, FALSE);
            }
            else
            {
                DrawPartyMenuLargeMonBackground(arr[8], arr[9], TRUE, 4);
            }
        }
        else if (gPlayerPartyCount > 4)
        {
            DrawPartyMenuLargeMonBackground(arr[8], arr[9], FALSE, 3);
            DrawPartyMenuMonBackgroundWithStatus(&arr[8], 4, 3, FALSE);
        }
        else
        {
            DrawPartyMenuLargeMonBackground(arr[8], arr[9], TRUE, 3);
        }
        break;
    case 6:
        if (!IsDoubleBattle())
        {
            if (gPlayerPartyCount > 5)
            {
                DrawPartyMenuLargeMonBackground(arr[10], arr[11], FALSE, 3);
                DrawPartyMenuMonBackgroundWithStatus(&arr[10], 5, 3, FALSE);
            }
            else
            {
                DrawPartyMenuLargeMonBackground(arr[10], arr[11], TRUE, 3);
            }
        }
        else if (IsLinkDoubleBattle() == TRUE)
        {
            if (GetMonData(&gPlayerParty[5], MON_DATA_SPECIES))
            {
                DrawPartyMenuLargeMonBackground(arr[10], arr[11], FALSE, 4);
                DrawPartyMenuMonBackgroundWithStatus(&arr[10], 5, 4, FALSE);
            }
            else
            {
                DrawPartyMenuLargeMonBackground(arr[10], arr[11], TRUE, 4);
            }
        }
        else if (gPlayerPartyCount > 5)
        {
            DrawPartyMenuLargeMonBackground(arr[10], arr[11], FALSE, 3);
            DrawPartyMenuMonBackgroundWithStatus(&arr[10], 5, 3, FALSE);
        }
        else
        {
            DrawPartyMenuLargeMonBackground(arr[10], arr[11], TRUE, 3);
        }
        break;
    case 7:
        if (ePartyMenu2.menuType == PARTY_MENU_TYPE_BATTLE_TOWER)
            DrawBattleTowerOkButton(1);
        DrawPartyMenuCancelButton(1);
        break;
    case 8:
        ePartyMenu2.backgroundRedrawState = 2;
        return TRUE;
    }

    return FALSE;
}

void DrawLinkMultiBattlePartyMonBackgrounds(void)
{
    const u8 * r4;
    struct Pokemon * pokemon;
    memset(&gBGTilemapBuffers[2], 0, 0x800);
    gPartyMenuType = 3;

    r4 = ({const u8 * r0 = sPartyMenuMonBackgroundCoords; r0 + 36;});
    pokemon = &gPlayerParty[1];
    DrawPartyMenuSmallMonBackground(r4[0], r4[1], 3);

    if (GetMonData(pokemon, MON_DATA_SPECIES))
        DrawPartyMenuLargeMonBackground(r4[4], r4[5], FALSE, 3);
    else
        DrawPartyMenuLargeMonBackground(r4[4], r4[5], TRUE, 3);

    if (GetMonData(pokemon + 1, MON_DATA_SPECIES))
        DrawPartyMenuLargeMonBackground(r4[6], r4[7], FALSE, 3);
    else
        DrawPartyMenuLargeMonBackground(r4[6], r4[7], TRUE, 3);

    ePartyMenu2.backgroundRedrawState = 2;
}

void DrawPartyMenuSmallMonBackground(s16 x, u16 y, u8 paletteNum)
{
    u8 i;
    u16 tilemapOffset = y * 32;

    for (i = 0; i <= 6; i++)
    {
        u8 j = 0;

        if (x <= 0x1F)
        {
            for (; j <= 10 && x + j <= 0x1F; j++)
            {
                if (x + j >= 0)
                    gBGTilemapBuffers[2][tilemapOffset + (i * 32) + (x + j)] = (paletteNum << 12) | sPartyMenuSmallMonBackgroundTilemap[i * 11 + j];
            }
        }
    }
}

void ClearPartyMenuSmallMonBackground(s16 x, u16 y)
{
    u8 i;
    u16 tilemapOffset = y * 32;

    for (i = 0; i <= 6; i++)
    {
        u8 j = 0;

        if (x <= 0x1F)
        {
            for (; j <= 10 && x + j <= 0x1F; j++)
            {
                if (x + j >= 0)
                    gBGTilemapBuffers[2][tilemapOffset + (i * 32) + (x + j)] = 0;
            }
        }
    }
}

void DrawPartyMenuLargeMonBackground(s16 x, u16 y, u8 isEmpty, u8 paletteNum)
{
    u8 i;
    const u8 *arr;
    u16 tilemapOffset;

    if (isEmpty == FALSE)
        arr = sPartyMenuLargeMonBackgroundTilemap;
    else
        arr = sPartyMenuLargeEmptyMonBackgroundTilemap;

    tilemapOffset = y * 32;

    for (i = 0; i < 3; i++)
    {
        u8 j = 0;

        if (x <= 0x1F)
        {
            while (j <= 0x12 && x + j <= 0x1F)
            {
                if (x + j >= 0)
                {
                    gBGTilemapBuffers[2][tilemapOffset + (i * 32) + (x + j)] = (paletteNum << 12) | arr[i * 19 + j];
                }

                j++;
            }
        }
    }
}

void ClearPartyMenuLargeMonBackground(s16 x, u16 y)
{
    u8 i;
    u16 tilemapOffset = y * 32;

    for (i = 0; i < 3; i++)
    {
        u8 j = 0;

        if (x <= 0x1F)
        {
            for (; j <= 0x12 && x + j <= 0x1F; j++)
            {
                if (x + j >= 0)
                {
                    gBGTilemapBuffers[2][tilemapOffset + (i * 32) + (x + j)] = 0;
                }
            }
        }
    }
}

void DrawBattleTowerOkButton(u8 paletteNum)
{
    u8 i;
    u16 arr[12] =
    {
        0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
        0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    };
    u16 *vramPtr = (u16 *)(BG_VRAM + 0x3C30);

    for (i = 0; i < PARTY_SIZE; i++)
    {
        vramPtr[i] = arr[i] + (paletteNum << 12);
        vramPtr[i + 0x20] = arr[i + PARTY_SIZE] + (paletteNum << 12);
    }
}

void DrawPartyMenuCancelButton(u8 paletteNum)
{
    u8 i;
    u16 arr[12] =
    {
        0x2A, 0x0B, 0x0C, 0x0D, 0x0E, 0x2F,
        0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    };
    u16 *vramPtr = (u16 *)(BG_VRAM + 0x3CB0);

    for (i = 0; i < PARTY_SIZE; i++)
    {
        vramPtr[i] = arr[i] + (paletteNum << 12);
        vramPtr[i + 0x20] = arr[i + PARTY_SIZE] + (paletteNum << 12);
    }
}

// Draws the descriptor text in the mon's party bubble.
// Descriptor texts are things like "ABLE", "NOT ABLE", "LEARNED", etc.
void DrawMonDescriptorStatus(u8 monIndex, u8 descriptorOffset)
{
    u8 i;
    u32 offset;
    u16 *vramPtr = sPartyMenuDescriptorStatusTilemapPtrs[IsDoubleBattle()][monIndex];
    int paletteNum = 0;

    for (i = 0; i < 7; i++)
    {
        offset = i + (descriptorOffset / 7) * 32;
        vramPtr[i] = paletteNum | (gPartyMenuDescriptorStatusTilemap[offset] + 0x10C);
        vramPtr[i + 0x20] = paletteNum | (gPartyMenuDescriptorStatusTilemap[offset + 0x20] + 0x10C);
    }
}

void DrawAllPartyMonDescriptorStatuses(u8 descriptorOffset)
{
    u8 i;

    for (i = 0; i < gPlayerPartyCount; i++)
        DrawMonDescriptorStatus(i, descriptorOffset);
}

// This is ultimately unreferenced, since it's caller is unreferenced.
void DrawPartyMonGenderDescriptorStatuses(void)
{
    u8 i;

    for (i = 0; i < gPlayerPartyCount; i++)
    {
        if (!GetMonData(&gPlayerParty[i], MON_DATA_IS_EGG))
        {
            switch (GetMonGender(&gPlayerParty[i]))
            {
            case MON_MALE:
                DrawMonDescriptorStatus(i, 0x54);
                break;
            case MON_FEMALE:
                DrawMonDescriptorStatus(i, 0x62);
                break;
            default:
                DrawMonDescriptorStatus(i, 0x46);
                break;
            }
        }
        else
        {
            DrawMonDescriptorStatus(i, 0x46);
        }
    }
}

u8 CreatePartyMenuCursor(u8 taskId, u8 unused)
{
    u8 spriteId = CreateInvisibleSpriteWithCallback(SpriteCallbackDummy);
    SetPartyMenuCursorSpriteId(taskId, spriteId);
    return 1;
}

u16 HandleDefaultPartyMenuInput(u8 taskId)
{
    s8 menuDirectionPressed = 0x0;

    switch (gMain.newAndRepeatedKeys)
    {
    case DPAD_UP:
        menuDirectionPressed = -1;
        break;
    case DPAD_DOWN:
        menuDirectionPressed = 1;
        break;
    case DPAD_LEFT:
        menuDirectionPressed = -2;
        break;
    case DPAD_RIGHT:
        menuDirectionPressed = 2;
        break;
    }

    if (menuDirectionPressed == 0)
    {
        // Check L/R input
        u8 var1 = GetLRKeysPressedAndHeld();
        switch (var1)
        {
        case 1:
            menuDirectionPressed = -1;
            break;
        case 2:
            menuDirectionPressed = 1;
            break;
        }
    }

    if (menuDirectionPressed != 0)
    {
        ChangePartyMenuSelection(taskId, menuDirectionPressed);
        return gMain.newAndRepeatedKeys;
    }
    else if (JOY_NEW(A_BUTTON) && gSprites[GetPartyMenuCursorSpriteId(taskId)].data[0] == 7)
    {
        // Selected "CANCEL"
        return B_BUTTON;
    }
    else
    {
        return JOY_NEW(A_BUTTON | B_BUTTON);
    }
}

u16 HandleBattleTowerPartyMenuInput(u8 taskId)
{
    u8 menuDirectionPressed = 0x0;

    switch (gMain.newAndRepeatedKeys)
    {
    case DPAD_UP:
        menuDirectionPressed = 0xFF;
        break;
    case DPAD_DOWN:
        menuDirectionPressed = 0x1;
        break;
    case DPAD_LEFT:
        menuDirectionPressed = 0xFE;
        break;
    case DPAD_RIGHT:
        menuDirectionPressed = 0x2;
        break;
    }

    if (menuDirectionPressed == 0)
    {
        switch (GetLRKeysPressedAndHeld())
        {
        case 1:
            menuDirectionPressed = 0xFF;
            break;
        case 2:
            menuDirectionPressed = 0x1;
            break;
        }
    }

    if (JOY_NEW(START_BUTTON))
    {
        SelectBattleTowerOKButton(taskId);
        return START_BUTTON;
    }
    else
    {
        s8 signedMenuDirection = menuDirectionPressed;
        if (signedMenuDirection)
        {
            ChangeBattleTowerPartyMenuSelection(taskId, signedMenuDirection);
            return gMain.newAndRepeatedKeys;
        }
        else
        {
            if (JOY_NEW(A_BUTTON))
            {
                if (gSprites[GetPartyMenuCursorSpriteId(taskId)].data[0] == 7)
                    return B_BUTTON;
            }
        }
    }

    return JOY_NEW(A_BUTTON | B_BUTTON);
}

void DrawPartyMenuMonBackgroundAt(const u8 *coords, u8 paletteNum)
{
    if (coords[0])
        DrawPartyMenuLargeMonBackground(coords[0], coords[1], FALSE, paletteNum);
    else
        DrawPartyMenuSmallMonBackground(coords[0], coords[1], paletteNum);
}

void DrawPartyMenuMonBackgroundWithStatus(const u8 *coords, u8 monIndex, u8 paletteNum, bool8 selected)
{
    if (GetMonData(&gPlayerParty[monIndex], MON_DATA_SPECIES) && GetMonData(&gPlayerParty[monIndex], MON_DATA_HP) == 0)
        paletteNum = PARTY_SIZE - 1;

    if (selected == TRUE)
        paletteNum += 4;

    DrawPartyMenuMonBackgroundAt(coords, paletteNum);
}

void ChangePartyMenuSelection(u8 taskId, s8 directionPressed)
{
    bool8 isLinkDoubleBattle;
    u8 spriteId = GetPartyMenuCursorSpriteId(taskId);
    u8 menuIndex = gSprites[spriteId].data[0];

    SetPartyMonIconSelected(taskId, menuIndex, FALSE);

    isLinkDoubleBattle = IsLinkDoubleBattle();
    if (isLinkDoubleBattle == 1)
    {
        if (menuIndex == 0 || menuIndex == 2 || menuIndex == 3)
            DrawPartyMenuMonBackgroundWithStatus(&sLinkMultiBattlePartyMonBackgroundCoords[menuIndex * 2], menuIndex, 3, FALSE);
        if (menuIndex == 1 || menuIndex == 4 || menuIndex == 5)
            DrawPartyMenuMonBackgroundWithStatus(&sLinkMultiBattlePartyMonBackgroundCoords[menuIndex * 2], menuIndex, 4, FALSE);
        if (menuIndex == 7)
            DrawPartyMenuCancelButton(1);

        ChangeLinkDoubleBattlePartyMenuSelection(spriteId, menuIndex, directionPressed);

        if (gSprites[spriteId].data[0] == 0 || gSprites[spriteId].data[0] == 2 || gSprites[spriteId].data[0] == 3)
            DrawPartyMenuMonBackgroundWithStatus(&sLinkMultiBattlePartyMonBackgroundCoords[gSprites[spriteId].data[0] * 2], gSprites[spriteId].data[0], 3, TRUE);
        if (gSprites[spriteId].data[0] == 1 || gSprites[spriteId].data[0] == 4 || gSprites[spriteId].data[0] == 5)
                DrawPartyMenuMonBackgroundWithStatus(&sLinkMultiBattlePartyMonBackgroundCoords[gSprites[spriteId].data[0] * 2], gSprites[spriteId].data[0], 4, TRUE);
        if (gSprites[spriteId].data[0] == 7)
            DrawPartyMenuCancelButton(2);

        ePartyMenu2.backgroundRedrawState = 2;

        gSprites[spriteId].x = sPartyMenuCursorCoords[PARTY_MENU_LAYOUT_LINK_DOUBLE_BATTLE][gSprites[spriteId].data[0]].x;
        gSprites[spriteId].y = sPartyMenuCursorCoords[PARTY_MENU_LAYOUT_LINK_DOUBLE_BATTLE][gSprites[spriteId].data[0]].y;
    }
    else
    {
        u8 isDoubleBattle = IsDoubleBattle();

        if (menuIndex < PARTY_SIZE)
            DrawPartyMenuMonBackgroundWithStatus(&sPartyMenuMonBackgroundCoords[isDoubleBattle * 12 + menuIndex * 2], menuIndex, 3, FALSE);
        else
            DrawPartyMenuCancelButton(1);

        if (!isDoubleBattle)
            ChangeDefaultPartyMenuSelection(spriteId, menuIndex, directionPressed);
        else
            ChangeDoubleBattlePartyMenuSelection(spriteId, menuIndex, directionPressed);

        if (gSprites[spriteId].data[0] < PARTY_SIZE)
            DrawPartyMenuMonBackgroundWithStatus(&sPartyMenuMonBackgroundCoords[isDoubleBattle * 12 + gSprites[spriteId].data[0] * 2], gSprites[spriteId].data[0], 3, TRUE);
        else
            DrawPartyMenuCancelButton(2);

        ePartyMenu2.backgroundRedrawState = 2;

        gSprites[spriteId].x = sPartyMenuCursorCoords[isDoubleBattle][gSprites[spriteId].data[0]].x;
        gSprites[spriteId].y = sPartyMenuCursorCoords[isDoubleBattle][gSprites[spriteId].data[0]].y;
    }

    SetPartyMonIconSelected(taskId, gSprites[spriteId].data[0], TRUE);

    if (menuIndex != gSprites[spriteId].data[0])
        PlaySE(SE_SELECT);
}

void ChangeDefaultPartyMenuSelection(u8 spriteId, u8 menuIndex, s8 directionPressed)
{
    u8 nextIndex;
    s8 menuMovement = directionPressed + 2;

    switch (menuMovement)
    {
    case 2: // no movement
        gSprites[spriteId].data[1] = 0;
        break;
    case 1: // moving up
        if (menuIndex == 0)
        {
            gSprites[spriteId].data[0] = 7;
        }
        else if (menuIndex == 7)
        {
            gSprites[spriteId].data[0] = gPlayerPartyCount - 1;
        }
        else
        {
            s8 diff = directionPressed;
            gSprites[spriteId].data[0] += diff;
        }
        gSprites[spriteId].data[1] = 0;
        break;
    case 3: // moving down
        if (menuIndex == gPlayerPartyCount - 1)
        {
            gSprites[spriteId].data[0] = 7;
        }
        else if (menuIndex == 7)
        {
            gSprites[spriteId].data[0] = 0;
        }
        else
        {
            s8 diff = directionPressed;
            gSprites[spriteId].data[0] += diff;
        }
        gSprites[spriteId].data[1] = 0;
        break;
    case 4: // moving right
        if (gPlayerPartyCount > 1 && menuIndex == 0)
        {
            if (gSprites[spriteId].data[1] == 0)
                gSprites[spriteId].data[1] = 1;
            gSprites[spriteId].data[0] = gSprites[spriteId].data[1];
        }
        break;
    case 0: // moving left
        // Only move the selection to the left side if one of the mons in the right-hand column are currently selected
        nextIndex = menuIndex - 1;
        if (nextIndex <= 4)
        {
            gSprites[spriteId].data[0] = 0;
            gSprites[spriteId].data[1] = menuIndex;
        }
        break;
    }
}

void ChangeDoubleBattlePartyMenuSelection(u8 spriteId, u8 menuIndex, s8 directionPressed)
{
    u8 var1;
    s8 menuMovement = directionPressed + 2;

    switch(menuMovement)
    {
    case 2: // no movement
        gSprites[spriteId].data[1] = 0;
        break;
    case 3: // moving down
        if (menuIndex == 7)
        {
            gSprites[spriteId].data[0] = 0;
        }
        else if (menuIndex == gPlayerPartyCount - 1)
        {
            gSprites[spriteId].data[0] = 7;
        }
        else
        {
            s8 diff = directionPressed;
            gSprites[spriteId].data[0] += diff;
        }
        gSprites[spriteId].data[1] = 0;
        break;
    case 1: // moving up
        if (menuIndex == 0)
        {
            gSprites[spriteId].data[0] = 7;
        }
        else if (menuIndex == 7)
        {
            gSprites[spriteId].data[0] = gPlayerPartyCount - 1;
        }
        else
        {
            s8 diff = directionPressed;
            gSprites[spriteId].data[0] += diff;
        }
        gSprites[spriteId].data[1] = 0;
        break;
    case 4: // moving right
        if (menuIndex == 0)
        {
            if (gPlayerPartyCount > 2)
            {
                u16 var1 = gSprites[spriteId].data[1] - 2;
                if (var1 > 1)
                    gSprites[spriteId].data[0] = 2;
                else
                    gSprites[spriteId].data[0] = gSprites[spriteId].data[1];
            }
        }
        else if (menuIndex == 1)
        {
            if (gPlayerPartyCount > 4)
            {
                u16 var1 = gSprites[spriteId].data[1] - 4;
                if (var1 <= 1)
                    gSprites[spriteId].data[0] = gSprites[spriteId].data[1];
                else
                    gSprites[spriteId].data[0] = 4;
            }
        }
        break;
    case 0: // moving left
        var1 = menuIndex - 2;
        if (var1 <= 1)
        {
            gSprites[spriteId].data[0] = 0;
            gSprites[spriteId].data[1] = menuIndex;
        }
        else
        {
            u8 var2 = menuIndex - 4;
            if (var2 <= 1)
            {
                gSprites[spriteId].data[0] = 1;
                gSprites[spriteId].data[1] = menuIndex;
            }
        }
        break;
    }
}

void ChangeLinkDoubleBattlePartyMenuSelection(u8 spriteId, u8 menuIndex, s8 directionPressed)
{
    switch (directionPressed)
    {
    case 0: // no movement
        gSprites[spriteId].data[1] = 0;
        break;
    case 1: // moving down
        if (menuIndex == 7) {
            gSprites[spriteId].data[0] = 0;
        }
        else
        {
            while (1)
            {
                if (menuIndex == PARTY_SIZE - 1)
                {
                    gSprites[spriteId].data[0] = 7;
                    break;
                }
                menuIndex++;
                if (GetMonData(&gPlayerParty[menuIndex], MON_DATA_SPECIES) != SPECIES_NONE)
                {
                    gSprites[spriteId].data[0] = menuIndex;
                    break;
                }
            }
        }
        gSprites[spriteId].data[1] = 0;
        break;
    case -1: // moving up
        while (1)
        {
            if (menuIndex == 0)
            {
                gSprites[spriteId].data[0] = 7;
                break;
            }
            menuIndex--;
            if (menuIndex != PARTY_SIZE && GetMonData(&gPlayerParty[menuIndex], MON_DATA_SPECIES) != SPECIES_NONE)
            {
                gSprites[spriteId].data[0] = menuIndex;
                break;
            }
        }

        gSprites[spriteId].data[1] = 0;
        break;
    case 2: // moving right
        if (menuIndex == 0) {
            if (gSprites[spriteId].data[1] < 2 || gSprites[spriteId].data[1] > 3) {
                if (GetMonData(&gPlayerParty[2], MON_DATA_SPECIES) != SPECIES_NONE) {
                    gSprites[spriteId].data[0] = 2;
                } else if (GetMonData(&gPlayerParty[3], MON_DATA_SPECIES) != SPECIES_NONE) {
                    gSprites[spriteId].data[0] = 3;
                }
            } else {
                gSprites[spriteId].data[0] = gSprites[spriteId].data[1];
            }
        } else if (menuIndex == 1) {
            if (gSprites[spriteId].data[1] >= 4 && gSprites[spriteId].data[1] <= 5) {
                gSprites[spriteId].data[0] = gSprites[spriteId].data[1];
            } else {
                if (GetMonData(&gPlayerParty[4], MON_DATA_SPECIES) != SPECIES_NONE) {
                    gSprites[spriteId].data[0] = 4;
                } else if (GetMonData(&gPlayerParty[5], MON_DATA_SPECIES) != SPECIES_NONE) {
                    gSprites[spriteId].data[0] = 5;
                }
            }
        }
        break;
    case -2: // moving left
        if (menuIndex > 1 && menuIndex < 4) {
            gSprites[spriteId].data[0] = 0;
            gSprites[spriteId].data[1] = menuIndex;
        } else {
            if (menuIndex > 3 && menuIndex < 6) {
                gSprites[spriteId].data[0] = 1;
                gSprites[spriteId].data[1] = menuIndex;
            }
        }
        break;
    }
}

void ChangeBattleTowerPartyMenuSelection(u8 taskId, s8 directionPressed)
{
    u16 newMenuIndex;
    u8 newMenuIndex2;
    u8 newMenuIndex3;
    s8 menuMovement;
    u8 spriteId = GetPartyMenuCursorSpriteId(taskId);
    u8 menuIndex = gSprites[spriteId].data[0];

    SetPartyMonIconSelected(taskId, menuIndex, FALSE);

    if (menuIndex < PARTY_SIZE)
        DrawPartyMenuMonBackgroundWithStatus(&sPartyMenuMonBackgroundCoords[menuIndex * 2], menuIndex, 3, FALSE);
    else if (menuIndex == PARTY_SIZE)
        DrawBattleTowerOkButton(1);
    else
        DrawPartyMenuCancelButton(1);

    menuMovement = directionPressed + 2;
    switch (menuMovement)
    {
    case 2: // no movement
        gSprites[spriteId].data[1] = 0;
        break;
    case 1: // moving up
        if (menuIndex == 0)
            gSprites[spriteId].data[0] = 7;
        else if (menuIndex == PARTY_SIZE)
            gSprites[spriteId].data[0] = gPlayerPartyCount - 1;
        else
            gSprites[spriteId].data[0] += directionPressed;
        gSprites[spriteId].data[1] = 0;
        break;
    case 3: // moving down
        if (menuIndex == gPlayerPartyCount - 1)
            gSprites[spriteId].data[0] = 6;
        else if (menuIndex == 7)
            gSprites[spriteId].data[0] = 0;
        else
            gSprites[spriteId].data[0] += directionPressed;
        gSprites[spriteId].data[1] = 0;
        break;
    case 4: // moving right
        if (gPlayerPartyCount > 1 && menuIndex == 0)
        {
            if (gSprites[spriteId].data[1] == 0)
                gSprites[spriteId].data[1] = 1;
            gSprites[spriteId].data[0] = gSprites[spriteId].data[1];
        }
        break;
    case 0: // moving left
        newMenuIndex3 = menuIndex - 1;
        if (newMenuIndex3 <= 4)
        {
            gSprites[spriteId].data[0] = 0;
            gSprites[spriteId].data[1] = menuIndex;
        }
        break;
    }

    gSprites[spriteId].x = sPartyMenuCursorCoords[PARTY_MENU_LAYOUT_STANDARD][gSprites[spriteId].data[0]].x;
    gSprites[spriteId].y = sPartyMenuCursorCoords[PARTY_MENU_LAYOUT_STANDARD][gSprites[spriteId].data[0]].y;


    newMenuIndex = gSprites[spriteId].data[0];
    if (gSprites[spriteId].data[0] < PARTY_SIZE)
        DrawPartyMenuMonBackgroundWithStatus(&sPartyMenuMonBackgroundCoords[gSprites[spriteId].data[0] * 2], newMenuIndex, 3, TRUE);
    else if (gSprites[spriteId].data[0] == PARTY_SIZE)
        DrawBattleTowerOkButton(2);
    else
        DrawPartyMenuCancelButton(2);

    ePartyMenu2.backgroundRedrawState = 2;

    newMenuIndex2 = gSprites[spriteId].data[0];
    SetPartyMonIconSelected(taskId, newMenuIndex2, TRUE);

    if (menuIndex != gSprites[spriteId].data[0])
        PlaySE(SE_SELECT);
}

// Selects the "OK" button in the Battle Tower party menu.
void SelectBattleTowerOKButton(u8 taskId)
{
    u8 spriteId = GetPartyMenuCursorSpriteId(taskId);

    u8 menuIndex = gSprites[spriteId].data[0];
    if (menuIndex != 6)
    {
        SetPartyMonIconSelected(taskId, menuIndex, FALSE);

        if (menuIndex < PARTY_SIZE)
            DrawPartyMenuMonBackgroundWithStatus(&sPartyMenuMonBackgroundCoords[menuIndex * 2], menuIndex, 3, FALSE);
        else
            DrawPartyMenuCancelButton(1);

        gSprites[spriteId].data[1] = 0;
        gSprites[spriteId].data[0] = 6;
        gSprites[spriteId].x = sPartyMenuCursorCoords[PARTY_MENU_LAYOUT_STANDARD][6].x;
        gSprites[spriteId].y = sPartyMenuCursorCoords[PARTY_MENU_LAYOUT_STANDARD][6].y;

        DrawBattleTowerOkButton(2);

        ePartyMenu2.backgroundRedrawState = 2;
        PlaySE(SE_SELECT);
    }
}

void UpdatePartyMenuCursorColumnSelection(u8 spriteId)
{
    u8 menuIndex1 = gSprites[spriteId].data[0];
    u8 menuIndex2 = gSprites[spriteId].data[1];

    if (!IsDoubleBattle())
    {
        if (menuIndex1 < 1)
        {
            if (menuIndex2 < 1)
                menuIndex2 = 1;
        }
        else
        {
            if (menuIndex2 >= 1)
                menuIndex2 = 0;
        }
    }
    else
    {
        if (menuIndex1 < 2)
        {
            if (menuIndex2 < 2)
                menuIndex2 = 2;
        }
        else
        {
            if (menuIndex2 >= 2)
                menuIndex2 = 0;
        }
    }

    gSprites[spriteId].data[1] = menuIndex2;
}

void SetPartyMenuCursorSelection(u8 taskId, u8 selection)
{
    u8 spriteId = GetPartyMenuCursorSpriteId(taskId);

    gSprites[spriteId].data[0] = selection;
    UpdatePartyMenuCursorColumnSelection(spriteId);
}

void CopyPartyMenuCursorSprite(u8 taskId, u8 spriteId)
{
    u8 cursorSpriteId = GetPartyMenuCursorSpriteId(taskId);

    gSprites[spriteId].x = gSprites[cursorSpriteId].x;
    gSprites[spriteId].y = gSprites[cursorSpriteId].y;
    gSprites[spriteId].data[0] = gSprites[cursorSpriteId].data[0];
}

u8 GetPartyMenuCursorSpriteId(u8 taskId)
{
    return gTasks[taskId].data[3] >> 8;
}

void SetPartyMenuCursorSpriteId(u8 taskId, u8 spriteId)
{
    u8 taskData = gTasks[taskId].data[3];
    gTasks[taskId].data[3] = taskData | (spriteId << 8);
}

u8 GetCursorSelectionMonId(u8 taskId)
{
    u8 spriteId = GetPartyMenuCursorSpriteId(taskId);
    return gSprites[spriteId].data[0];
}

void SetupDefaultPartyMenuSwitchPokemon(u8 taskId)
{
    gTasks[taskId].func = TaskDummy;
    ePartyMenu.partyMenuTaskId = taskId;

    CreateTask(HandlePartyMenuSwitchPokemonInput, 0);
    ePartyMenu.firstSelectedMonSpriteId = CreateInvisibleSpriteWithCallback(SpriteCallbackDummy);

    CopyPartyMenuCursorSprite(taskId, ePartyMenu.firstSelectedMonSpriteId);
    ePartyMenu.currentSelectionSpriteId = GetPartyMenuCursorSpriteId(taskId);

    PrintPartyMenuPromptText(ePartyMenu2.pmTwoMonActionPromptTextId, 0);

    DrawPartyMenuMonBackgroundWithStatus(&sPartyMenuMonBackgroundCoords[gSprites[ePartyMenu.firstSelectedMonSpriteId].data[0] * 2], gSprites[ePartyMenu.firstSelectedMonSpriteId].data[0], 6, FALSE);
    ePartyMenu2.backgroundRedrawState = 2;
}

// Handles changing the the current mon selection when choosing mons to swap places.
void ChangePartyMenuSwitchPokemonSelection(u8 taskId, s16 menuDirectionPressed)
{
    struct Sprite *firstSelectedMonSprite = &gSprites[ePartyMenu.firstSelectedMonSpriteId];
    struct Sprite *currentSelectionSprite = &gSprites[ePartyMenu.currentSelectionSpriteId];

    ChangePartyMenuSelection(taskId, menuDirectionPressed);

    if (firstSelectedMonSprite->data[0] != currentSelectionSprite->data[0])
        DrawPartyMenuMonBackgroundWithStatus(&sPartyMenuMonBackgroundCoords[firstSelectedMonSprite->data[0] * 2], firstSelectedMonSprite->data[0], 6, TRUE);
    else
        DrawPartyMenuMonBackgroundWithStatus(&sPartyMenuMonBackgroundCoords[firstSelectedMonSprite->data[0] * 2], firstSelectedMonSprite->data[0], 6, FALSE);
}

void HandlePartyMenuSwitchPokemonInput(u8 taskId)
{
    switch (gMain.newAndRepeatedKeys)
    {
    case DPAD_UP:
        ChangePartyMenuSwitchPokemonSelection(ePartyMenu.partyMenuTaskId, -1);
        break;
    case DPAD_DOWN:
        ChangePartyMenuSwitchPokemonSelection(ePartyMenu.partyMenuTaskId, 1);
        break;
    case DPAD_LEFT:
        ChangePartyMenuSwitchPokemonSelection(ePartyMenu.partyMenuTaskId, -2);
        break;
    case DPAD_RIGHT:
        ChangePartyMenuSwitchPokemonSelection(ePartyMenu.partyMenuTaskId, 2);
        break;
    }

    if (JOY_NEW(A_BUTTON))
    {
        PlaySE(5);
        gTasks[taskId].func = ePartyMenu.twoMonActionCallback;
        gTasks[taskId].func(taskId);
    }
    else if (gMain.newKeys == B_BUTTON)
    {
        PlaySE(5);
        CancelTwoMonAction(taskId);
    }
}

void FinishTwoMonAction(u8 taskId)
{
    DestroySprite(&gSprites[ePartyMenu.firstSelectedMonSpriteId]);
    PrintPartyMenuPromptText(0, 0);

    ePartyMenu2.backgroundRedrawState = 2;
    SwitchTaskToFollowupFunc(ePartyMenu.partyMenuTaskId);
    DestroyTask(taskId);
}

void FinishTwoMonActionAfterSwitch(u8 taskId)
{
    DrawPartyMenuMonBackgroundWithStatus(&sPartyMenuMonBackgroundCoords[gSprites[ePartyMenu.currentSelectionSpriteId].data[0] * 2], gSprites[ePartyMenu.currentSelectionSpriteId].data[0], 3, TRUE);
    DrawPartyMenuMonBackgroundWithStatus(&sPartyMenuMonBackgroundCoords[gSprites[ePartyMenu.firstSelectedMonSpriteId].data[0] * 2], gSprites[ePartyMenu.firstSelectedMonSpriteId].data[0], 3, FALSE);
    FinishTwoMonAction(taskId);
}

void ResetTwoMonActionSelection()
{
    u8 firstMonIndex = gSprites[ePartyMenu.firstSelectedMonSpriteId].data[0];
    u8 secondMonIndex = gSprites[ePartyMenu.currentSelectionSpriteId].data[0];

    if (firstMonIndex <= 5)
        DrawPartyMenuMonBackgroundWithStatus(&sPartyMenuMonBackgroundCoords[firstMonIndex * 2], firstMonIndex, 3, FALSE);
    if (secondMonIndex <= 5)
        DrawPartyMenuMonBackgroundWithStatus(&sPartyMenuMonBackgroundCoords[secondMonIndex * 2], secondMonIndex, 3, TRUE);
}

void CancelTwoMonAction(u8 taskId)
{
    ResetTwoMonActionSelection();
    FinishTwoMonAction(taskId);
}

void Task_SwitchSelectedPartyMons(u8 taskId)
{
    u8 firstMonIndex = gSprites[ePartyMenu.firstSelectedMonSpriteId].data[0];
    u8 secondMonIndex = gSprites[ePartyMenu.currentSelectionSpriteId].data[0];

    if (firstMonIndex == secondMonIndex || firstMonIndex == 7 || secondMonIndex == 7)
    {
        CancelTwoMonAction(taskId);
    }
    else
    {
        u8 var1;

        ClearPartyMenuMonTextAndIcon(firstMonIndex);
        ClearPartyMenuMonTextAndIcon(secondMonIndex);

        if (firstMonIndex > secondMonIndex)
        {
            ePartyMenu.lowerMonIndex = secondMonIndex;
            ePartyMenu.higherMonIndex = firstMonIndex;
        }
        else
        {
            ePartyMenu.lowerMonIndex = firstMonIndex;
            ePartyMenu.higherMonIndex = secondMonIndex;
        }

        ePartyMenu.lowerMonIconSpriteId = GetMonIconSpriteId(ePartyMenu.partyMenuTaskId, ePartyMenu.lowerMonIndex);
        ePartyMenu.higherMonIconSpriteId = GetMonIconSpriteId(ePartyMenu.partyMenuTaskId, ePartyMenu.higherMonIndex);

        var1 = ePartyMenu.lowerMonIndex;
        if (!var1)
        {
            gSprites[ePartyMenu.lowerMonIconSpriteId].data[0] = -8;
            gSprites[ePartyMenu.lowerMonIconSpriteId].data[2] = -0xA8;
            ePartyMenu.lowerMonBackgroundX = var1;
            ePartyMenu.higherMonBackgroundX = 11;

            gTasks[taskId].func = Task_AnimatePartyMenuLeftColumnSwitchOut;
            ePartyMenu2.backgroundRedrawState = 1;
        }
        else
        {
            gSprites[ePartyMenu.lowerMonIconSpriteId].data[0] = 8;
            gSprites[ePartyMenu.lowerMonIconSpriteId].data[2] = 0xA8;
            ePartyMenu.lowerMonBackgroundX = 11;
            ePartyMenu.higherMonBackgroundX = 11;

            gTasks[taskId].func = Task_AnimatePartyMenuRightColumnSwitchOut;
            ePartyMenu2.backgroundRedrawState = 1;
        }

        gSprites[ePartyMenu.lowerMonIconSpriteId].callback = SpriteCB_PartyMonIconSlide;

        gSprites[ePartyMenu.higherMonIconSpriteId].data[0] = 8;
        gSprites[ePartyMenu.higherMonIconSpriteId].data[2] = 0xA8;
        gSprites[ePartyMenu.higherMonIconSpriteId].callback = SpriteCB_PartyMonIconSlide;

        gSprites[ePartyMenu.lowerMonIconSpriteId].callback(&gSprites[ePartyMenu.lowerMonIconSpriteId]);
        gSprites[ePartyMenu.higherMonIconSpriteId].callback(&gSprites[ePartyMenu.higherMonIconSpriteId]);
    }
}

void SwapValues_s16(s16 *a, s16 *b)
{
    s16 temp = *a;
    *a = *b;
    *b = temp;
}

void SwapPartyMenuMonIconPositions(void)
{
    SwapValues_s16(&gSprites[ePartyMenu.lowerMonIconSpriteId].x, &gSprites[ePartyMenu.higherMonIconSpriteId].x);
    SwapValues_s16(&gSprites[ePartyMenu.lowerMonIconSpriteId].y, &gSprites[ePartyMenu.higherMonIconSpriteId].y);
    SwapValues_s16(&gSprites[ePartyMenu.lowerMonIconSpriteId].x2, &gSprites[ePartyMenu.higherMonIconSpriteId].x2);
    SwapValues_s16(&gSprites[ePartyMenu.lowerMonIconSpriteId].data[0], &gSprites[ePartyMenu.higherMonIconSpriteId].data[0]);

    gSprites[ePartyMenu.lowerMonIconSpriteId].callback = SpriteCB_PartyMonIconSlide;
    gSprites[ePartyMenu.higherMonIconSpriteId].callback = SpriteCB_PartyMonIconSlide;
}

void AnimatePartyMenuLeftColumnSwitchBackgrounds(u8 taskId, bool8 restore)
{
    u8 higherMonY = ((ePartyMenu.higherMonIndex - 1) * 3) + 1;

    ClearPartyMenuSmallMonBackground(ePartyMenu.lowerMonBackgroundX, 3);
    ClearPartyMenuLargeMonBackground(ePartyMenu.higherMonBackgroundX, higherMonY);

    if (restore == FALSE)
    {
        ePartyMenu.lowerMonBackgroundX--;
        ePartyMenu.higherMonBackgroundX++;
    }
    else
    {
        ePartyMenu.lowerMonBackgroundX++;
        ePartyMenu.higherMonBackgroundX--;
    }

    DrawPartyMenuSmallMonBackground(ePartyMenu.lowerMonBackgroundX, 3, 10);
    DrawPartyMenuLargeMonBackground(ePartyMenu.higherMonBackgroundX, higherMonY, FALSE, 10);
}

void Task_AnimatePartyMenuLeftColumnSwitchOut(u8 taskId)
{
    AnimatePartyMenuLeftColumnSwitchBackgrounds(taskId, FALSE);

    if (ePartyMenu.lowerMonBackgroundX < -13 && ePartyMenu.higherMonBackgroundX > 32)
    {
        SwapPartyMenuMonIconPositions();
        gTasks[taskId].func = Task_AnimatePartyMenuLeftColumnSwitchIn;
    }
}

void Task_AnimatePartyMenuLeftColumnSwitchIn(u8 taskId)
{
    AnimatePartyMenuLeftColumnSwitchBackgrounds(taskId, TRUE);

    if (ePartyMenu.lowerMonBackgroundX == 0 && ePartyMenu.higherMonBackgroundX == 11)
        gTasks[taskId].func = FinishPartyMenuMonSwitch;
}

void AnimatePartyMenuRightColumnSwitchBackgrounds(u8 taskId, bool8 restore)
{
    u8 lowerMonY = ((ePartyMenu.lowerMonIndex - 1) * 3) + 1;
    u8 higherMonY = ((ePartyMenu.higherMonIndex - 1) * 3) + 1;

    ClearPartyMenuLargeMonBackground(ePartyMenu.lowerMonBackgroundX, lowerMonY);
    ClearPartyMenuLargeMonBackground(ePartyMenu.higherMonBackgroundX, higherMonY);

    if (restore == FALSE)
    {
        ePartyMenu.lowerMonBackgroundX++;
        ePartyMenu.higherMonBackgroundX++;
    }
    else
    {
        ePartyMenu.lowerMonBackgroundX--;
        ePartyMenu.higherMonBackgroundX--;
    }

    DrawPartyMenuLargeMonBackground(ePartyMenu.lowerMonBackgroundX, lowerMonY, FALSE, 10);
    DrawPartyMenuLargeMonBackground(ePartyMenu.higherMonBackgroundX, higherMonY, FALSE, 10);
}

void Task_AnimatePartyMenuRightColumnSwitchOut(u8 taskId)
{
    AnimatePartyMenuRightColumnSwitchBackgrounds(taskId, FALSE);

    if (ePartyMenu.lowerMonBackgroundX > 32 && ePartyMenu.higherMonBackgroundX > 32)
    {
        SwapPartyMenuMonIconPositions();
        gTasks[taskId].func = Task_AnimatePartyMenuRightColumnSwitchIn;
    }
}

void Task_AnimatePartyMenuRightColumnSwitchIn(u8 taskId)
{
    AnimatePartyMenuRightColumnSwitchBackgrounds(taskId, TRUE);

    if (ePartyMenu.lowerMonBackgroundX == 11 && ePartyMenu.higherMonBackgroundX == 11)
        gTasks[taskId].func = FinishPartyMenuMonSwitch;
}

void FinishPartyMenuMonSwitch(u8 taskId)
{
    u8 spriteId;

    SetMonIconSpriteId(ePartyMenu.partyMenuTaskId, ePartyMenu.lowerMonIndex, ePartyMenu.higherMonIconSpriteId);
    SetMonIconSpriteId(ePartyMenu.partyMenuTaskId, ePartyMenu.higherMonIndex, ePartyMenu.lowerMonIconSpriteId);

    gSprites[ePartyMenu.lowerMonIconSpriteId].x = sPartyMenuMonIconCoords[IsDoubleBattle()][ePartyMenu.higherMonIndex].x;
    gSprites[ePartyMenu.lowerMonIconSpriteId].y = sPartyMenuMonIconCoords[IsDoubleBattle()][ePartyMenu.higherMonIndex].y;
    gSprites[ePartyMenu.lowerMonIconSpriteId].x2 = 0;
    gSprites[ePartyMenu.lowerMonIconSpriteId].y2 = 0;
    gSprites[ePartyMenu.lowerMonIconSpriteId].callback = UpdatePartyMonIconFrame;

    gSprites[ePartyMenu.higherMonIconSpriteId].x = sPartyMenuMonIconCoords[IsDoubleBattle()][ePartyMenu.lowerMonIndex].x;
    gSprites[ePartyMenu.higherMonIconSpriteId].y = sPartyMenuMonIconCoords[IsDoubleBattle()][ePartyMenu.lowerMonIndex].y;
    gSprites[ePartyMenu.higherMonIconSpriteId].x2 = 0;
    gSprites[ePartyMenu.higherMonIconSpriteId].y2 = 0;
    gSprites[ePartyMenu.higherMonIconSpriteId].callback = UpdatePartyMonIconFrame;

    spriteId = GetMonIconSpriteId(ePartyMenu.partyMenuTaskId, gSprites[ePartyMenu.currentSelectionSpriteId].data[0]);
    gSprites[spriteId].callback = UpdatePartyMonIconFrameAndBounce;

    SwapPokemon(&gPlayerParty[ePartyMenu.lowerMonIndex], &gPlayerParty[ePartyMenu.higherMonIndex]);

    PartyMenuPrintMonLevelOrStatus(ePartyMenu.lowerMonIndex, &gPlayerParty[ePartyMenu.lowerMonIndex]);
    TryPrintPartyMenuMonNickname(ePartyMenu.lowerMonIndex, &gPlayerParty[ePartyMenu.lowerMonIndex]);
    PartyMenuTryPrintHP(ePartyMenu.lowerMonIndex, &gPlayerParty[ePartyMenu.lowerMonIndex]);
    PartyMenuNoOpMonUpdate(ePartyMenu.lowerMonIndex, &gPlayerParty[ePartyMenu.lowerMonIndex]);

    PartyMenuPrintMonLevelOrStatus(ePartyMenu.higherMonIndex, &gPlayerParty[ePartyMenu.higherMonIndex]);
    TryPrintPartyMenuMonNickname(ePartyMenu.higherMonIndex, &gPlayerParty[ePartyMenu.higherMonIndex]);
    PartyMenuTryPrintHP(ePartyMenu.higherMonIndex, &gPlayerParty[ePartyMenu.higherMonIndex]);
    PartyMenuNoOpMonUpdate(ePartyMenu.higherMonIndex, &gPlayerParty[ePartyMenu.higherMonIndex]);

    PartyMenuDrawHPBars();
    FinishTwoMonActionAfterSwitch(taskId);
}

void SpriteCB_PartyMonIconSlide(struct Sprite *sprite)
{
    UpdateMonIconFrame(sprite);

    if (sprite->x2 == sprite->data[2])
    {
        sprite->data[0] *= -1;
        sprite->data[2] = 0;
        sprite->callback = UpdatePartyMonIconFrame;
    }
    else
    {
        sprite->x2 += sprite->data[0];
    }
}

void UpdateLinkMultiBattlePartnerPartyBackgrounds(u8 taskId, u16 species1, u16 species2)
{
    const u8 *coords = sLinkMultiBattlePartyMonBackgroundCoords;

    ClearPartyMenuSmallMonBackground(coords[2] + gTasks[taskId].data[0], coords[3]);
    ClearPartyMenuLargeMonBackground(coords[8] + gTasks[taskId].data[0], coords[9]);
    ClearPartyMenuLargeMonBackground(coords[10] + gTasks[taskId].data[0], coords[11]);

    gTasks[taskId].data[0]--;

    DrawPartyMenuSmallMonBackground(coords[2] + gTasks[taskId].data[0], coords[3], 4);

    if (species1)
        DrawPartyMenuLargeMonBackground(coords[8] + gTasks[taskId].data[0], coords[9], FALSE, 4);
    else
        DrawPartyMenuLargeMonBackground(coords[8] + gTasks[taskId].data[0], coords[9], TRUE, 4);

    if (species2)
        DrawPartyMenuLargeMonBackground(coords[10] + gTasks[taskId].data[0], coords[11], FALSE, 4);
    else
        DrawPartyMenuLargeMonBackground(coords[10] + gTasks[taskId].data[0], coords[11], TRUE, 4);
}

void StartLinkMultiBattlePartnerMonIconSlide(u8 taskId, u16 species, u8 partnerMonIndex)
{
    if (species)
    {
        u8 monIndex = partnerMonIndex + 3;
        u8 spriteId = GetMonIconSpriteId(taskId, monIndex);

        gSprites[spriteId].data[0] = -8;
        gSprites[spriteId].data[2] = gTasks[taskId].data[0] * -8;
        gSprites[spriteId].callback = SpriteCB_PartyMonIconSlide;
    }
}

void OffsetLinkMultiBattlePartnerMonIcon(u8 taskId, u8 monIndex)
{
    gSprites[GetMonIconSpriteId(taskId, monIndex)].x += 0xF0;
}

void PrintPartyMenuPromptText(u8 textId, u8 b)
{
    if (textId != 0xFF)
    {
        switch (b)
        {
        case 0:
            Menu_DrawStdWindowFrame(0, 16, 23, 19);
            break;
        case 1:
            Menu_DrawStdWindowFrame(0, 16, 19, 19);
            break;
        case 2:
            Menu_DrawStdWindowFrame(0, 16, 22, 19);
            break;
        case 3:
            Menu_DrawStdWindowFrame(0, 16, 18, 19);
            break;
        }

        Menu_PrintText(sPartyMenuPromptTexts[textId], 1, 17);
    }
}

void PartyMenuEraseMsgBoxAndFrame(void)
{
    Menu_EraseWindowRect(0, 16, 29, 19);
}

void ClearPartyMenuMonTextAndIcon(u8 monIndex)
{
    u8 left = sPartyMenuMonTextWindowCoords[IsDoubleBattle()][monIndex].left;
    u8 top = sPartyMenuMonTextWindowCoords[IsDoubleBattle()][monIndex].top;
    u8 right = sPartyMenuMonTextWindowCoords[IsDoubleBattle()][monIndex].right;
    u8 bottom = sPartyMenuMonTextWindowCoords[IsDoubleBattle()][monIndex].bottom;

    Text_EraseWindowRect(&gWindowTemplate_Contest_MoveDescription, left, top, right, bottom);

    CpuFastFill(0, OBJ_VRAM1 + monIndex * 0x400, 0x400);
}

void ClearPartyMonDescriptorStatus(u8 monIndex)
{
    u8 left = sPartyMenuDescriptorStatusWindowCoords[IsDoubleBattle()][monIndex].left;
    u8 top = sPartyMenuDescriptorStatusWindowCoords[IsDoubleBattle()][monIndex].top;
    u8 right = sPartyMenuDescriptorStatusWindowCoords[IsDoubleBattle()][monIndex].right;
    u8 bottom = sPartyMenuDescriptorStatusWindowCoords[IsDoubleBattle()][monIndex].bottom;

    Text_EraseWindowRect(&gWindowTemplate_Contest_MoveDescription, left, top, right, bottom);

    CpuFastFill(0, OBJ_VRAM1 + 0x300 + monIndex * 0x400, 0x100);
}

bool8 LoadPartyMenuGraphics(u8 a)
{
    u16 palette = 0x7FFF;
    bool8 retVal = FALSE;

    if (a < 2)
        LZDecompressVram(gPartyMenuMisc_Gfx, (void *)BG_VRAM);

    if (a == 2 || a == 0)
        LZDecompressVram(gPartyMenuMisc_Tilemap, (void *)(BG_VRAM + 0x3800));

    if (a == 3 || a == 0)
        LoadCompressedPalette(gPartyMenuMisc_Pal, 0, 0x160);

    if (a == 4 || a == 0)
    {
        LoadPalette(&palette, 0, 2);
        LoadPalette(gFontDefaultPalette, 0xF0, 0x20);
    }

    if (a == 5 || a == 0)
        LZDecompressVram(gPartyMenuHpBar_Gfx, (void *)(BG_VRAM + 0x6000));

    if (a == 6 || a == 0)
        LZDecompressVram(gPartyMenuOrderText_Gfx, (void *)(BG_VRAM + 0x6180));

    if (a == 7 || a == 0)
        LZDecompressVram(gStatusGfx_Icons, (void *)(BG_VRAM + 0x7180));

    if (a == 8 || a == 0)
    {
        LoadCompressedPalette(gStatusPal_Icons, 0xB0, 0x20);
        retVal = TRUE;
    }

    return retVal;
}

void SetMonIconAnimByHP(u8 spriteId, u16 currentHP, u16 maxHP)
{
    u8 hpBarLevel = GetHPBarLevel(currentHP, maxHP);
    u8 animNum = 0;

    if (currentHP != maxHP)
    {
        if (hpBarLevel == 3)
            animNum = 1;
        else if (hpBarLevel == 2)
            animNum = 2;
        else if (hpBarLevel == 1)
            animNum = 3;
        else
            animNum = 4;
    }

    SetPartyHPBarSprite(&gSprites[spriteId], animNum);
}

void SetMonIconAnim(u8 spriteId, struct Pokemon *pokemon)
{
    u16 currentHP = GetMonData(pokemon, MON_DATA_HP);
    u16 maxHP = GetMonData(pokemon, MON_DATA_MAX_HP);

    SetMonIconAnimByHP(spriteId, currentHP, maxHP);
}

void CreatePartyMenuMonIcon(u8 taskId, u8 monIndex, u8 menuType, struct Pokemon *pokemon)
{
    u8 x = sPartyMenuMonIconCoords[menuType][monIndex].x;
    u8 y = sPartyMenuMonIconCoords[menuType][monIndex].y;

    u16 species2 = GetMonData(pokemon, MON_DATA_SPECIES2);
    u32 personality = GetMonData(pokemon, MON_DATA_PERSONALITY);

    u8 spriteId = CreateMonIcon(species2, SpriteCB_MonIcon, x, y, 5, personality);
    SetMonIconSpriteId(taskId, monIndex, spriteId);
    SetMonIconAnim(spriteId, pokemon);
}

void TryCreatePartyMenuMonIcon(u8 taskId, u8 monIndex, struct Pokemon *pokemon)
{
    if (GetMonData(pokemon, MON_DATA_SPECIES))
    {
        if (IsLinkDoubleBattle() == TRUE)
            CreatePartyMenuMonIcon(taskId, monIndex, PARTY_MENU_LAYOUT_LINK_DOUBLE_BATTLE, pokemon);
        else
            CreatePartyMenuMonIcon(taskId, monIndex, IsDoubleBattle(), pokemon);
    }
}

void CreateAllPartyMenuMonIcons(u8 taskId)
{
    u8 i;

    for (i = 0; i < gPlayerPartyCount; i++)
        TryCreatePartyMenuMonIcon(taskId, i, &gPlayerParty[i]);
}

void CreateMonIcon_LinkMultiBattle(u8 taskId, u8 monIndex, u8 menuType, struct MultiBattlePokemonTx *pokemon)
{
    u8 x = sPartyMenuMonIconCoords[menuType][monIndex].x;
    u8 y = sPartyMenuMonIconCoords[menuType][monIndex].y;

    u8 spriteId = CreateMonIcon(pokemon->species, SpriteCB_MonIcon, x, y, 5, pokemon->personality);
    SetMonIconSpriteId(taskId, monIndex, spriteId);
    SetMonIconAnimByHP(spriteId, pokemon->hp, pokemon->maxhp);
}

void UpdatePartyMonIconFrameAndBounce(struct Sprite *sprite)
{
    u8 var1 = UpdateMonIconFrame(sprite);

    if (var1)
    {
        if (var1 & 1)
            sprite->y2 = -3;
        else
            sprite->y2 = 1;
    }
}

void UpdatePartyMonIconFrame(struct Sprite *sprite)
{
    UpdateMonIconFrame(sprite);
}

void SetPartyMonIconSelected(u8 taskId, u8 monIndex, bool8 selected)
{
    u8 spriteId;

    if (monIndex < PARTY_SIZE)
    {
        spriteId = GetMonIconSpriteId(taskId, monIndex);
        gSprites[spriteId].y2 = 0;
        gSprites[spriteId].data[0] = 0;

        if (selected == FALSE)
            gSprites[spriteId].callback = UpdatePartyMonIconFrame;
        else
            gSprites[spriteId].callback = UpdatePartyMonIconFrameAndBounce;
    }
}

void LoadHeldItemIconGraphics(void)
{
    LoadSpriteSheet(&sHeldItemIconSpriteSheet);
    LoadSpritePalette(&sHeldItemIconSpritePalette);
}

void SpriteCB_HeldItem(struct Sprite *sprite)
{
    u8 data7 = sprite->data[7];

    if (gSprites[data7].invisible)
    {
        sprite->invisible = TRUE;
    }
    else
    {
        sprite->invisible = FALSE;
        sprite->x = gSprites[data7].x + gSprites[data7].x2;
        sprite->y = gSprites[data7].y + gSprites[data7].y2;
    }
}

void CreateHeldItemIcon(u8 a, u8 b)
{
    u8 subPriority;
    u8 spriteId;

    subPriority = gSprites[a].subpriority;
    spriteId = CreateSprite(&sSpriteTemplate_HeldItemIcon, 0xFA, 0xAA, subPriority - 1);

    gSprites[spriteId].x2 = 4;
    gSprites[spriteId].y2 = 10;
    gSprites[spriteId].callback = SpriteCB_HeldItem;
    gSprites[spriteId].data[7] = a;

    StartSpriteAnim(&gSprites[spriteId], b);
    gSprites[spriteId].callback(&gSprites[spriteId]);
}

void CreateHeldItemIcons(u8 *a, u8 *b, u8 c)
{
    u16 i;
    u16 heldItem;

    switch (c)
    {
    case 0:
        for (i = 0; i < a[0]; i++)
        {
            heldItem = GetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM);
            if (heldItem)
                CreateHeldItemIcon(b[i], ItemIsMail(heldItem));
        }
        break;
    case 1:
        for (i = 0; i < a[1]; i++)
        {
            heldItem = GetMonData(&gEnemyParty[i], MON_DATA_HELD_ITEM);
            if (heldItem)
                CreateHeldItemIcon(b[i + 6], ItemIsMail(heldItem));
        }
        break;
    }
}

void CreatePartyMenuHeldItemIcons(u8 taskId)
{
    u8 i;
    u8 monIconSpriteId;
    u8 heldItemSpriteId;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES))
        {
            monIconSpriteId = GetMonIconSpriteId(taskId, i);
            heldItemSpriteId = CreateSprite(&sSpriteTemplate_HeldItemIcon, 0xFA, 0xAA, 4);

            gSprites[heldItemSpriteId].x2 = 4;
            gSprites[heldItemSpriteId].y2 = 10;
            gSprites[heldItemSpriteId].data[7] = monIconSpriteId;
            gSprites[monIconSpriteId].data[7] = heldItemSpriteId;

            SetHeldItemIconVisibility(taskId, i);
            gSprites[heldItemSpriteId].callback(&gSprites[heldItemSpriteId]);
        }
    }
}

void CreatePartyMenuHeldItemIcon(u8 taskId, u8 monIndex, u16 item)
{
    u8 monIconSpriteId;
    u8 heldItemSpriteId;
    struct Sprite * r1;
    struct Sprite * heldItemSprite;

    monIconSpriteId = GetMonIconSpriteId(taskId, monIndex);
    heldItemSpriteId = CreateSprite(&sSpriteTemplate_HeldItemIcon, 0xFA, 0xAA, 4);

    r1 = gSprites;
    heldItemSprite = r1 + heldItemSpriteId;
    heldItemSprite->x2 = 4;
    heldItemSprite->y2 = 10;
    heldItemSprite->data[7] = monIconSpriteId;
    gSprites[monIconSpriteId].data[7] = heldItemSpriteId;

    if (item == ITEM_NONE)
    {
        heldItemSprite->invisible = TRUE;
    }
    else if (ItemIsMail(item))
    {
        StartSpriteAnim(heldItemSprite, 1);
        heldItemSprite->invisible = FALSE;
    }
    else
    {
        StartSpriteAnim(heldItemSprite, 0);
        heldItemSprite->invisible = FALSE;
    }

    gSprites[heldItemSpriteId].callback(&gSprites[heldItemSpriteId]);
}

void SpriteCB_UpdateHeldItemIconPosition(struct Sprite *sprite)
{
    u8 spriteId = sprite->data[7];

    sprite->x = gSprites[spriteId].x + gSprites[spriteId].x2;
    sprite->y = gSprites[spriteId].y;
}

// Soft casting of gTasks[taskId].data to u8* but big-endian.
u8 GetMonIconSpriteId(u8 taskId, u8 monIndex)
{
    switch (monIndex)
    {
    case 1:
        return gTasks[taskId].data[0];
        break;
    case 2:
        return gTasks[taskId].data[1] >> 8;
        break;
    case 3:
        return gTasks[taskId].data[1];
        break;
    case 4:
        return gTasks[taskId].data[2] >> 8;
        break;
    case 5:
        return gTasks[taskId].data[2];
        break;
    case 0:
    default:
        return gTasks[taskId].data[0] >> 8;
        break;
    }
}

void SetMonIconSpriteId(u8 taskId, u8 monIndex, u8 spriteId)
{
    switch (monIndex)
    {
    case 0:
        gTasks[taskId].data[0] = (u8)gTasks[taskId].data[0] | (spriteId << 8);
        break;
    case 1:
        gTasks[taskId].data[0] = (gTasks[taskId].data[0] & ~0xFF) | spriteId;
        break;
    case 2:
        gTasks[taskId].data[1] = (u8)gTasks[taskId].data[1] | (spriteId << 8);
        break;
    case 3:
        gTasks[taskId].data[1] = (gTasks[taskId].data[1] & ~0xFF) | spriteId;
        break;
    case 4:
        gTasks[taskId].data[2] = (u8)gTasks[taskId].data[2] | (spriteId << 8);
        break;
    case 5:
        gTasks[taskId].data[2] = (gTasks[taskId].data[2] & ~0xFF) | spriteId;
        break;
    }
}

u16 GetMonHeldItemIconSpriteId(u8 taskId, u8 monIndex)
{
    u8 spriteId = GetMonIconSpriteId(taskId, monIndex);
    u8 retVal = gSprites[spriteId].data[7];
    return retVal;
}

// #ifdef NONMATCHING (for grep)
void SetHeldItemIconVisibility(u8 taskId, u8 monIndex)
{
    u8 spriteId;
    u16 heldItem;

    spriteId = GetMonHeldItemIconSpriteId(taskId, monIndex);
    if (!GetMonData(&gPlayerParty[monIndex], MON_DATA_HELD_ITEM))
    {
        gSprites[spriteId].invisible = TRUE;
    }
    else
    {
        register struct Sprite *sprite asm("r4");
// sprite2 is required to mimic a failed optimization where r0 would have been loaded at the end of the if statement
        register struct Sprite *sprite2 asm("r0");
        u8 animNum;
        heldItem = GetMonData(&gPlayerParty[monIndex], MON_DATA_HELD_ITEM);
        if (ItemIsMail(heldItem))
        {
            sprite = &gSprites[spriteId];
            sprite2 = sprite; // hack
            animNum = 1;
        }
        else
        {
            sprite = &gSprites[spriteId];
            sprite2 = sprite; // hack
            animNum = 0;
        }
        StartSpriteAnim(sprite2, animNum);
        sprite->invisible = FALSE;
    }
}

void PartyMenuDoPrintMonNickname(u8 monIndex, int b, const u8 *nameBuffer)
{
    CpuFastFill(0, sPartyMenuTileBuffer, 0x400);
    Text_InitWindow8004E3C((struct WindowTemplate *)&gPartyMenuMonDataWindowTemplate, sPartyMenuTileBuffer, nameBuffer);
    CpuFastCopy(sPartyMenuTileBuffer, OBJ_VRAM1 + (monIndex * 0x400), 0x200);
}

void PrintPartyMenuMonNickname(u8 monIndex, u8 b, struct Pokemon *pokemon)
{
    u8 nameBuffer[12];
    GetMonNickname(pokemon, nameBuffer);
    PartyMenuDoPrintMonNickname(monIndex, b, nameBuffer);
}

void TryPrintPartyMenuMonNickname(u8 monIndex, struct Pokemon *pokemon)
{
    if (GetMonData(pokemon, MON_DATA_SPECIES))
    {
        if (IsLinkDoubleBattle() == TRUE)
            PrintPartyMenuMonNickname(monIndex, PARTY_MENU_LAYOUT_LINK_DOUBLE_BATTLE, pokemon);
        else
            PrintPartyMenuMonNickname(monIndex, IsDoubleBattle(), pokemon);
    }
}

void PrintPartyMenuMonNicknames(void)
{
    u8 i;

    for (i = 0; i < PARTY_SIZE; i++)
        TryPrintPartyMenuMonNickname(i, &gPlayerParty[i]);
}

u8 *GetMonNickname(struct Pokemon *pokemon, u8 *stringBuffer)
{
    GetMonData(pokemon, MON_DATA_NICKNAME, stringBuffer);
    return StringGet_Nickname(stringBuffer);
}

void PartyMenuPutStatusTilemap(u8 monIndex, u8 menuLayout, u8 status)
{
    u8 i;
    u8 x = sPartyMenuMonTextTileCoords[menuLayout][monIndex].x - 1;
    u8 y = sPartyMenuMonTextTileCoords[menuLayout][monIndex].y + 1;
    u16 *vramPtr = (u16*)(VRAM + 0xF000) + (x + y * 32);
    u8 var1 = status * 4;

    for (i = 0; i < 4; i++)
        vramPtr[i] = (0x18C + var1 + i) | -0x5000;
}

static void PartyMenuClearLevelStatusTilemap(u8 monIndex)
{
    u8 menuLayout;
    u8 x;
    u8 y;
    u16 *vramPtr;
    u8 i;

    if (IsLinkDoubleBattle() == TRUE)
        menuLayout = PARTY_MENU_LAYOUT_LINK_DOUBLE_BATTLE;
    else
        menuLayout = IsDoubleBattle();

    x = sPartyMenuMonTextTileCoords[menuLayout][monIndex].x - 1;
    y = sPartyMenuMonTextTileCoords[menuLayout][monIndex].y + 1;

    vramPtr = (u16*)(VRAM + 0xF000) + (x + y * 32);
    for (i = 0; i < 4; i++)
        vramPtr[i] = 0;
}

static void PartyMenuWriteTilemap(u8 a, u8 x, u8 y)
{
    u16 *vramPtr = (u16*)(VRAM + 0xF000) + (x + y * 32);
    *vramPtr = a + 0x10C;
}

void PartyMenuDoPrintLevel(u8 monIndex, u8 menuLayout, u8 level)
{
    u8 *stringVar;
    u8 x = sPartyMenuMonTextTileCoords[menuLayout][monIndex].x;
    u8 y = sPartyMenuMonTextTileCoords[menuLayout][monIndex].y;

    PartyMenuWriteTilemap(0x40, x - 1, y + 1);

    stringVar = gStringVar1;
    stringVar[0] = 0xFC;
    stringVar[1] = 0x12;
    stringVar[2] = 0x8;

    ConvertIntToDecimalStringSimple(&stringVar[3], level);

    CpuFastFill(0, sPartyMenuTileBuffer + 0x100, 0x80);
    Text_InitWindow8004E3C((struct WindowTemplate *)&gPartyMenuMonDataWindowTemplate, sPartyMenuTileBuffer, gStringVar1);
    CpuFastCopy(sPartyMenuTileBuffer + 0x100, OBJ_VRAM1 + 0x200 + (monIndex * 0x400), 0x80);
}

void PartyMenuPrintLevel(u8 monIndex, u8 menuLayout, struct Pokemon *pokemon)
{
    u16 level = GetMonData(pokemon, MON_DATA_LEVEL);
    PartyMenuDoPrintLevel(monIndex, menuLayout, level);
}

void PartyMenuPrintMonLevelOrStatus(u8 monIndex, struct Pokemon *pokemon)
{
    if (GetMonData(pokemon, MON_DATA_SPECIES) && !GetMonData(pokemon, MON_DATA_IS_EGG))
    {
        u8 statusAndPkrs = GetMonStatusAndPokerus(pokemon);
        u8 menuLayout;

        if (IsLinkDoubleBattle() == TRUE)
            menuLayout = PARTY_MENU_LAYOUT_LINK_DOUBLE_BATTLE;
        else
            menuLayout = IsDoubleBattle();

        if (statusAndPkrs && statusAndPkrs != STATUS_PRIMARY_POKERUS)
            PartyMenuPutStatusTilemap(monIndex, menuLayout, statusAndPkrs - 1);
        else
            PartyMenuPrintLevel(monIndex, menuLayout, pokemon);

        PartyMenuPrintGenderIcon(monIndex, menuLayout, pokemon);
    }
}

void PartyMenuPrintMonsLevelOrStatus(void)
{
    u8 i;

    for (i = 0; i < PARTY_SIZE; i++)
        PartyMenuPrintMonLevelOrStatus(i, &gPlayerParty[i]);
}

void PartyMenuDoPrintGenderIcon(u16 species, u8 gender, u8 menuLayout, u8 monIndex, u8 *nickname)
{
    if (!ShouldHideGenderIcon(species, nickname))
    {
        u8 x = sPartyMenuMonTextTileCoords[menuLayout][monIndex].x + 3;
        u8 y = sPartyMenuMonTextTileCoords[menuLayout][monIndex].y + 1;

        switch (gender)
        {
        case MON_MALE:
            PartyMenuWriteTilemap(0x42, x, y);
            break;
        case MON_FEMALE:
            PartyMenuWriteTilemap(0x44, x, y);
            break;
        }
    }
}

void PartyMenuPrintGenderIcon(u8 monIndex, u8 menuLayout, struct Pokemon *pokemon)
{
    u16 species2;
    u8 gender;

    GetMonNickname(pokemon, gStringVar1);
    species2 = GetMonData(pokemon, MON_DATA_SPECIES2);
    gender = GetMonGender(pokemon);
    PartyMenuDoPrintGenderIcon(species2, gender, menuLayout, monIndex, gStringVar1);
}

void PartyMenuDoPrintHP(u8 monIndex, u8 b, u16 currentHP, u16 maxHP)
{
    register u8 *stringVar1 asm("r2") = gStringVar1;
    register u8 *textPtr asm("r2") = AlignInt1InMenuWindow(stringVar1, currentHP, 15, 1);
    textPtr[0] = CHAR_SLASH;

    AlignInt1InMenuWindow(++textPtr, maxHP, 35, 1);
    CpuFastFill(0, sPartyMenuTileBuffer + 0x100, 0x100);
    Text_InitWindow8004E3C((struct WindowTemplate *)&gPartyMenuMonDataWindowTemplate, sPartyMenuTileBuffer, gStringVar1);
    CpuFastCopy(sPartyMenuTileBuffer + 0x100, OBJ_VRAM1 + 0x300 + (monIndex * 0x400), 0x100);
}

void PartyMenuPrintHP(u8 monIndex, u8 b, struct Pokemon *pokemon)
{
    u16 currentHP = GetMonData(pokemon, MON_DATA_HP);
    u16 maxHP = GetMonData(pokemon, MON_DATA_MAX_HP);

    PartyMenuDoPrintHP(monIndex, b, currentHP, maxHP);
}

void PartyMenuTryPrintHP(u8 monIndex, struct Pokemon *pokemon)
{
    if (GetMonData(pokemon, MON_DATA_SPECIES) && !GetMonData(pokemon, MON_DATA_IS_EGG))
    {
        if (IsLinkDoubleBattle() == TRUE)
            PartyMenuPrintHP(monIndex, PARTY_MENU_LAYOUT_LINK_DOUBLE_BATTLE, pokemon);
        else
            PartyMenuPrintHP(monIndex, IsDoubleBattle(), pokemon);
    }
}

void PartyMenuTryPrintMonsHP(void)
{
    u8 i;

    for (i = 0; i < PARTY_SIZE; i++)
        PartyMenuTryPrintHP(i, &gPlayerParty[i]);
}

void PartyMenuUnusedCallback1(void) { }
void PartyMenuUnusedCallback2(void) { }
void PartyMenuNoOpMonUpdate(u8 monIndex, struct Pokemon *pokemon) { }
void PartyMenuNoOpSetupStep(void) { }

void PartyMenuDoDrawHPBar(u8 monIndex, u8 menuLayout, u16 currentHP, u16 maxHP)
{
    u8 hpBarLevel;
    u16 *vramPtr;
    int var = -32768;
    struct HealthBarInfo battleInterface;
    battleInterface.maxValue = maxHP;
    battleInterface.oldValue = currentHP;
    battleInterface.receivedValue = 0;

    hpBarLevel = GetHPBarLevel(currentHP, maxHP);
    if (hpBarLevel > 2)
        battleInterface.unkC_0 = 4;
    if (hpBarLevel == 2)
        battleInterface.unkC_0 = 5;
    if (hpBarLevel < 2)
        battleInterface.unkC_0 = 6;

    battleInterface.unk10 = 0x100;

    vramPtr = sPartyMenuHpBarTilemapPtrs[menuLayout][monIndex];
    UpdateHealthBar(&battleInterface, &var, vramPtr, 0);

    vramPtr -= 2;

    vramPtr[0] = 0x3109;
    vramPtr[1] = 0x310A;
    vramPtr[8] = 0x310B;
}

void PartyMenuDrawHPBar(u8 monIndex, u8 menuLayout, struct Pokemon *pokemon)
{
    u16 currentHP = GetMonData(pokemon, MON_DATA_HP);
    u16 maxHP = GetMonData(pokemon, MON_DATA_MAX_HP);

    PartyMenuDoDrawHPBar(monIndex, menuLayout, currentHP, maxHP);
}

void PartyMenuTryDrawHPBar(u8 monIndex, struct Pokemon *pokemon)
{
    if (GetMonData(pokemon, MON_DATA_SPECIES) && !GetMonData(pokemon, MON_DATA_IS_EGG))
    {
        if (IsLinkDoubleBattle() == TRUE)
            PartyMenuDrawHPBar(monIndex, PARTY_MENU_LAYOUT_LINK_DOUBLE_BATTLE, pokemon);
        else
            PartyMenuDrawHPBar(monIndex, IsDoubleBattle(), pokemon);
    }
}

void PartyMenuDrawHPBars(void)
{
    u8 i;

    for (i = 0; i < PARTY_SIZE; i++)
        PartyMenuTryDrawHPBar(i, &gPlayerParty[i]);
}

void SwapPokemon(struct Pokemon *a, struct Pokemon *b)
{
    struct Pokemon temp = *a;
    *a = *b;
    *b = temp;
}

void SetPartyPopupMenuOffsets(u8 menuIndex, u8 *left, u8 *top, const struct PartyPopupMenu *menu)
{
    u8 bottomOffset = (2 * menu[menuIndex].numChoices) + 2;
    u8 rightOffset = menu[menuIndex].width + 1;

    *left = 30 - rightOffset;
    *top = 20 - bottomOffset;
}

void ShowPartyPopupMenu(u8 menuIndex, const struct PartyPopupMenu *menu, const struct MenuAction2 *menuActions, u8 cursorPos)
{
    u8 left;
    u8 top;

    SetPartyPopupMenuOffsets(menuIndex, &left, &top, menu);
    DrawPartyPopupMenu(left, top, menu[menuIndex].width, menu[menuIndex].numChoices, menuActions, menu[menuIndex].items);

    InitMenu(0, left + 1, top + 1, menu[menuIndex].numChoices, cursorPos, menu[menuIndex].width - 1);
}

void ClosePartyPopupMenu(u8 index, const struct PartyPopupMenu *menu)
{
    u8 left;
    u8 top;

    SetPartyPopupMenuOffsets(index, &left, &top, menu);

    Menu_EraseWindowRect(left, top, left + menu[index].width, menu[index].numChoices * 2 + top + 1);
    Menu_DestroyCursor();
}

TaskFunc PartyMenuGetPopupMenuFunc(u8 menuIndex, const struct PartyPopupMenu *menus, const struct MenuAction2 *menuActions, u8 itemIndex)
{
    u8 action = menus[menuIndex].items[itemIndex];
    return menuActions[action].func;
}

u8 DisplayPartyMenuMessage(const u8 *message, u8 noClearAfter)
{
    u8 taskId;

    gPartyMenuMessage_IsPrinting = 1;

    Menu_DrawStdWindowFrame(WINDOW_LEFT, 14, WINDOW_RIGHT, 19);
    MenuPrintMessage(message, WINDOW_LEFT + 1, 15);

    taskId = CreateTask(Task_PartyMenuPrintRun, 1);
    gTasks[taskId].data[0] = noClearAfter;

    return taskId;
}

static void Task_PartyMenuPrintRun(u8 taskId)
{
    if (Menu_UpdateWindowText())
    {
        gPartyMenuMessage_IsPrinting = 0;
        if (gTasks[taskId].data[0] == 0)
            Menu_EraseWindowRect(WINDOW_LEFT, 14, WINDOW_RIGHT, 19);
        DestroyTask(taskId);
    }
}

void SetPartyMenuItemUseContext(u8 taskId, u16 itemOrMoveId, TaskFunc followupCallback)
{
    gPartyMenu.followupCallback = followupCallback;
    gPartyMenu.menuTaskId = taskId;
    gPartyMenu.selectedMonIndex = GetCursorSelectionMonId(taskId);
    gPartyMenu.itemOrMoveId = itemOrMoveId;
    gPartyMenu.selectedMon = &gPlayerParty[gPartyMenu.selectedMonIndex];
}

bool8 PartyMenuUpdateMonHeldItem(struct Pokemon *pkmn, u16 item)
{
    u8 itemData[2];

    if (ItemIsMail(item) == TRUE)
    {
        if (GiveMailToMonByItemId(pkmn, item) == MAIL_NONE)
            return TRUE;
        gPartyMenuActionResult = 2;
    }
    else
    {
        gPartyMenuActionResult = 1;
    }

    itemData[0] = item;
    itemData[1] = item >> 8;
    SetMonData(pkmn, MON_DATA_HELD_ITEM, itemData);
    return FALSE;
}

void PartyMenuTryGiveMonHeldItem(u8 taskId, u16 newItem, TaskFunc c)
{
    u16 currentItem;

    gTasks[taskId].func = TaskDummy;
    SetPartyMenuItemUseContext(taskId, newItem, c);
    currentItem = GetMonData(gPartyMenu.selectedMon, MON_DATA_HELD_ITEM);
    gPartyMenuActionResult = 0;
    gPartyMenuPreviousHeldItem = 0;
    if (currentItem != 0)
    {
        if (ItemIsMail(currentItem) == TRUE)
        {
            DisplayPartyMenuMessage(gOtherText_MailMustBeRemoved, 1);
            CreateTask(Task_UpdatePartyMenuHeldItemIconAndFollowup, 5);
        }
        else
        {
            GetMonNickname(gPartyMenu.selectedMon, gStringVar1);
            CopyItemName(currentItem, gStringVar2);
            StringExpandPlaceholders(gStringVar4, gOtherText_AlreadyHolding);
            DisplayPartyMenuMessage(gStringVar4, 1);
            CreateTask(Task_ConfirmGiveHeldItem, 5);
            if (ItemIsMail(newItem) == TRUE)
                gPartyMenuPreviousHeldItem = currentItem;
        }
    }
    else
    {
        PartyMenuUpdateMonHeldItem(gPartyMenu.selectedMon, newItem);
        RemoveBagItem(newItem, 1);
        if (ItemIsMail(newItem))
        {
            gTasks[taskId].func = c;
        }
        else
        {
            DisplayGiveHeldItemMessage(gPartyMenu.selectedMonIndex, newItem, 1);
            CreateTask(Task_UpdatePartyMenuHeldItemIconAndFollowup, 5);
        }
    }
}

void Task_UpdatePartyMenuHeldItemIconAndFollowup(u8 taskId)
{
    if (gPartyMenuMessage_IsPrinting == 0)
    {
        SetHeldItemIconVisibility(gPartyMenu.menuTaskId, gPartyMenu.selectedMonIndex);
        gTasks[gPartyMenu.menuTaskId].func = gPartyMenu.followupCallback;
        DestroyTask(taskId);
    }
}

void Task_HandleHeldItemReplacement(u8 taskId)
{
    s8 selection = Menu_ProcessInputNoWrap_();

    if (selection == 0)
    {
        u16 currentItem;

        Menu_EraseWindowRect(23, 8, 29, 13);
        currentItem = GetMonData(gPartyMenu.selectedMon, MON_DATA_HELD_ITEM);
        RemoveBagItem(gPartyMenu.itemOrMoveId, 1);
        if (AddBagItem(currentItem, 1) == TRUE)
        {
            PartyMenuUpdateMonHeldItem(gPartyMenu.selectedMon, gPartyMenu.itemOrMoveId);
            if (ItemIsMail(gPartyMenu.itemOrMoveId))
            {
                DisplayTakeHeldItemMessage(gPartyMenu.selectedMonIndex, currentItem, 1);
            }
            else
            {
                CopyItemName(gPartyMenu.itemOrMoveId, gStringVar1);
                StringExpandPlaceholders(gStringVar4, gOtherText_TakenAndReplaced);
                DisplayPartyMenuMessage(gStringVar4, 1);
            }
        }
        else
        {
            DisplayPartyMenuMessage(gOtherText_BagFullCannotRemoveItem, 0);
            AddBagItem(gPartyMenu.itemOrMoveId, 1);
        }
    }
    else
    {
        if (selection != 1 && selection != -1)
            return;
        if (selection == -1)
            PlaySE(SE_SELECT);
        Menu_EraseWindowRect(23, 8, 29, 13);
    }
    gTasks[taskId].func = Task_UpdatePartyMenuHeldItemIconAndFollowup;
}

void Task_ConfirmGiveHeldItem(u8 taskId)
{
    if (gPartyMenuMessage_IsPrinting == 0)
    {
        DisplayYesNoMenu(23, 8, 1);
        gTasks[taskId].func = Task_HandleHeldItemReplacement;
    }
}

void DisplayGiveHeldItemMessage(u8 a, u16 b, u8 c)
{
    GetMonNickname(&gPlayerParty[a], gStringVar1);
    CopyItemName(b, gStringVar2);
    StringExpandPlaceholders(gStringVar4, gOtherText_WasGivenToHold);
    DisplayPartyMenuMessage(gStringVar4, c);
}

void PartyMenuTryGiveMonMail(u8 taskId, TaskFunc func)
{
    u32 var1;
    u16 currentItem;
    struct Mail *mail;

    gTasks[taskId].func = TaskDummy;
    SetPartyMenuItemUseContext(taskId, 0, func);
    currentItem = GetMonData(gPartyMenu.selectedMon, MON_DATA_HELD_ITEM);
    gPartyMenuActionResult = 0;
    var1 = gPlayerPCItemPageInfo.cursorPos + 6;
    mail = &gSaveBlock1.mail[var1 + gPlayerPCItemPageInfo.itemsAbove];

    if (currentItem != 0)
    {
        DisplayPartyMenuMessage(gOtherText_PokeHoldingItemCantMail, 1);
        CreateTask(Task_UpdatePartyMenuHeldItemIconAndFollowup, 5);
    }
    else
    {
        GiveMailToMon(gPartyMenu.selectedMon, mail);
        ClearMail(mail);
        DisplayPartyMenuMessage(gOtherText_MailTransferredMailbox, 1);
        CreateTask(Task_UpdatePartyMenuHeldItemIconAndFollowup, 5);
    }
}

void TryTakePartyMonHeldItem(u8 taskId, TaskFunc func)
{
    u16 currentItem;

    gTasks[taskId].func = TaskDummy;
    SetPartyMenuItemUseContext(taskId, 0, func);
    currentItem = GetMonData(gPartyMenu.selectedMon, MON_DATA_HELD_ITEM);
    if (currentItem == 0)
    {
        StringExpandPlaceholders(gStringVar4, gOtherText_NotHoldingAnything);
        DisplayPartyMenuMessage(gStringVar4, 0);
        CreateTask(Task_UpdatePartyMenuHeldItemIconAndFollowup, 5);
    }
    else
    {
        u8 itemData[2];

        itemData[0] = 0;
        itemData[1] = 0;
        if (AddBagItem(currentItem, 1) == TRUE)
        {
            if (ItemIsMail(currentItem) == TRUE)
                TakeMailFromMon(gPartyMenu.selectedMon);
            DisplayTakeHeldItemMessage(gPartyMenu.selectedMonIndex, currentItem, 0);
            SetMonData(gPartyMenu.selectedMon, MON_DATA_HELD_ITEM, itemData);
        }
        else
        {
            DisplayPartyMenuMessage(gOtherText_BagFullCannotRemoveItem, 0);
        }
        CreateTask(Task_UpdatePartyMenuHeldItemIconAndFollowup, 5);
    }
}

void DisplayTakeHeldItemMessage(u8 a, u16 b, u8 c)
{
    GetMonNickname(&gPlayerParty[a], gStringVar1);
    CopyItemName(b, gStringVar2);
    StringExpandPlaceholders(gStringVar4, gOtherText_ReceivedTheThingFrom);
    DisplayPartyMenuMessage(gStringVar4, c);
}

void DoTakeMail(u8 taskId, TaskFunc func)
{
    gTasks[taskId].func = TaskDummy;
    SetPartyMenuItemUseContext(taskId, 0, func);
    gPartyMenu.itemOrMoveId = GetMonData(gPartyMenu.selectedMon, MON_DATA_HELD_ITEM);
    DisplayPartyMenuMessage(gOtherText_SendRemovedMailPrompt, 1);
    CreateTask(Task_ConfirmTakeHeldMail, 5);
}

void Task_LoseMailMessage(u8 taskId)
{
    s8 selection = Menu_ProcessInputNoWrap_();

    if (selection == 0)
    {
        if (AddBagItem(gPartyMenu.itemOrMoveId, 1) == TRUE)
        {
            TakeMailFromMon(gPartyMenu.selectedMon);
            DisplayPartyMenuMessage(gOtherText_MailTaken, 0);
        }
        else
        {
            DisplayPartyMenuMessage(gOtherText_BagFullCannotRemoveItem, 0);
        }
        Menu_EraseWindowRect(23, 8, 29, 13);
        gTasks[taskId].func = Task_UpdatePartyMenuHeldItemIconAndFollowup;
    }
    else
    {
        if (selection != 1 && selection != -1)
            return;
        if (selection == -1)
            PlaySE(SE_SELECT);
        Menu_EraseWindowRect(23, 8, 29, 13);
        Menu_EraseWindowRect(0, 14, 29, 19);
        gTasks[taskId].func = Task_UpdatePartyMenuHeldItemIconAndFollowup;
    }
}

void Task_ConfirmLoseMailMessage(u8 taskId)
{
    if (gPartyMenuMessage_IsPrinting == 0)
    {
        DisplayYesNoMenu(23, 8, 1);
        gTasks[taskId].func = Task_LoseMailMessage;
    }
}

void Task_TakeHeldMail(u8 taskId)
{
    s8 selection = Menu_ProcessInputNoWrap_();

    if (selection == 0)
    {
        Menu_EraseWindowRect(23, 8, 29, 13);
        if (TakeMailFromMonAndSave(gPartyMenu.selectedMon) != MAIL_NONE)
            DisplayPartyMenuMessage(gOtherText_MailWasSent, 0);
        else
            DisplayPartyMenuMessage(gOtherText_MailboxIsFull, 0);
        gTasks[taskId].func = Task_UpdatePartyMenuHeldItemIconAndFollowup;
    }
    else
    {
        if (selection != 1 && selection != -1)
            return;
        if (selection == -1)
            PlaySE(SE_SELECT);
        Menu_EraseWindowRect(23, 8, 29, 13);
        DisplayPartyMenuMessage(gOtherText_MailRemovedMessageLost, 1);
        gTasks[taskId].func = Task_ConfirmLoseMailMessage;
    }
}

void Task_ConfirmTakeHeldMail(u8 taskId)
{
    if (gPartyMenuMessage_IsPrinting == 0)
    {
        DisplayYesNoMenu(23, 8, 1);
        gTasks[taskId].func = Task_TakeHeldMail;
    }
}

u16 ItemIdToBattleMoveId(u16 item)
{
    u16 machineNumber = item - ITEM_TM01_FOCUS_PUNCH;
    return sTmHmMoves[machineNumber];
}

bool8 MonKnowsMove(struct Pokemon *pkmn, u16 move)
{
    u8 i;

    for (i = 0; i < 4; i++)
    {
        if (GetMonData(pkmn, MON_DATA_MOVE1 + i) == move)
            return TRUE;
    }
    return FALSE;
}

void TeachMonTMMove(u8 taskId, u16 move, TaskFunc func)
{
    PlaySE(SE_SELECT);
    gTasks[taskId].func = TaskDummy;
    SetPartyMenuItemUseContext(taskId, move, func);
    CreateTask(Task_TeamMonTMMove, 5);
}

#if DEBUG
extern u8 gIsDebugMode;
#endif

void Task_TeamMonTMMove(u8 taskId)
{
    GetMonNickname(gPartyMenu.selectedMon, gStringVar1);
    gPartyMenu.moveToLearn = ItemIdToBattleMoveId(gPartyMenu.itemOrMoveId);
    StringCopy(gStringVar2, gMoveNames[gPartyMenu.moveToLearn]);
    ePartyMenu2.pmMoveLearningFromLevelUp = FALSE;
    if (MonKnowsMove(gPartyMenu.selectedMon, gPartyMenu.moveToLearn))
    {
        StringExpandPlaceholders(gStringVar4, gOtherText_AlreadyKnows);
        DisplayPartyMenuMessage(gStringVar4, 1);
        gTasks[taskId].func = Task_UpdatePartyMenuHeldItemIconAndFollowup;
    }
    else
    {
        if (
#if DEBUG
         !gIsDebugMode &&
#endif
         !CanMonLearnTMHM(gPartyMenu.selectedMon, gPartyMenu.itemOrMoveId - 33))
        {
            StringExpandPlaceholders(gStringVar4, gOtherText_NotCompatible);
            DisplayPartyMenuMessage(gStringVar4, 1);
            gTasks[taskId].func = Task_UpdatePartyMenuHeldItemIconAndFollowup;
        }
        else
        {
            if (GiveMoveToMon(gPartyMenu.selectedMon, gPartyMenu.moveToLearn) != MON_HAS_MAX_MOVES)
            {
                Task_TeamMonTMMove2(taskId);
            }
            else
            {
                StringExpandPlaceholders(gStringVar4, gOtherText_WantsToLearn);
                DisplayPartyMenuMessage(gStringVar4, 1);
                gTasks[taskId].func = Task_ShowForgetMovePrompt;
            }
        }
    }
}

void Task_TeamMonTMMove2(u8 taskId)
{
    StringCopy(gStringVar2, gMoveNames[gPartyMenu.moveToLearn]);
    StringExpandPlaceholders(gStringVar4, gOtherText_LearnedMove);
    DisplayPartyMenuMessage(gStringVar4, 1);
    AdjustFriendship(gPartyMenu.selectedMon, FRIENDSHIP_EVENT_LEARN_TMHM);
    if (ePartyMenu2.pmMoveLearningFromLevelUp == FALSE && gPartyMenu.itemOrMoveId < ITEM_HM01_CUT)
        RemoveBagItem(gPartyMenu.itemOrMoveId, 1);
    gTasks[taskId].func = Task_TeamMonTMMove3;
}

void Task_TeamMonTMMove3(u8 taskId)
{
    if (gPartyMenuMessage_IsPrinting == 0)
    {
        PlayFanfare(MUS_LEVEL_UP);
        gTasks[taskId].func = Task_TeamMonTMMove4;
    }
}

void Task_TeamMonTMMove4(u8 taskId)
{
    if (IsFanfareTaskInactive())
    {
        if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
        {
            SetHeldItemIconVisibility(gPartyMenu.menuTaskId, gPartyMenu.selectedMonIndex);
            if (ePartyMenu2.pmMoveLearningFromLevelUp == TRUE)
            {
                TeachMonMoveInPartyMenu(taskId);
            }
            else
            {
                gTasks[gPartyMenu.menuTaskId].func = gPartyMenu.followupCallback;
                DestroyTask(taskId);
            }
        }
    }
}

void Task_FinishMoveLearningMessage(u8 taskId)
{
    if (gPartyMenuMessage_IsPrinting == 0)
    {
        SetHeldItemIconVisibility(gPartyMenu.menuTaskId, gPartyMenu.selectedMonIndex);
        if (ePartyMenu2.pmMoveLearningFromLevelUp == TRUE)
        {
            TeachMonMoveInPartyMenu(taskId);
        }
        else
        {
            gTasks[gPartyMenu.menuTaskId].func = gPartyMenu.followupCallback;
            DestroyTask(taskId);
        }
    }
}

void Task_ShowForgetMovePrompt(u8 taskId)
{
    if (gPartyMenuMessage_IsPrinting == 0)
    {
        DisplayYesNoMenu(23, 8, 1);
        gTasks[taskId].func = Task_HandleForgetMovePrompt;
    }
}

void Task_HandleForgetMovePrompt(u8 taskId)
{
    s8 selection = Menu_ProcessInputNoWrap_();

    if (selection == 0)
    {
        Menu_EraseWindowRect(23, 8, 29, 13);
        DisplayPartyMenuMessage(gOtherText_WhichMoveToForget2, 1);
        gTasks[taskId].func = Task_FadeToMoveSelectionSummary;
    }
    else
    {
        if (selection != 1 && selection != -1)
            return;
        if (selection == -1)
            PlaySE(SE_SELECT);
        Task_ShowStopTryingMoveMessage(taskId);
    }
}

void Task_OpenMoveSelectionSummary(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        ShowSelectMovePokemonSummaryScreen(gPlayerParty, gPartyMenu.selectedMonIndex, gPlayerPartyCount - 1, CB2_ReturnToPartyMenuForMoveLearning, gPartyMenu.moveToLearn);
        DestroyTask(taskId);
    }
}

void Task_FadeToMoveSelectionSummary(u8 taskId)
{
    if (gPartyMenuMessage_IsPrinting == 0)
    {
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        gTasks[taskId].func = Task_OpenMoveSelectionSummary;
    }
}

void TaughtMove(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        u8 moveIndex;
        u16 r4;

        gTasks[taskId].func = TaskDummy;
        SetPartyMenuItemUseContext(taskId, gSpecialVar_ItemId, Task_ExitPartyMenuToBag);
        moveIndex = GetMoveSlotToReplace();
        r4 = GetMonData(gPartyMenu.selectedMon, MON_DATA_MOVE1 + moveIndex);
        GetMonNickname(gPartyMenu.selectedMon, gStringVar1);
        StringCopy(gStringVar2, gMoveNames[r4]);
        StringExpandPlaceholders(gStringVar4, gOtherText_ForgetMove123_2);
        DisplayPartyMenuMessage(gStringVar4, 1);
        CreateTask(TMMoveUpdateMoveSlot, 5);
    }
}

void TMMoveUpdateMoveSlot(u8 taskId)
{
    if (gPartyMenuMessage_IsPrinting == 0)
    {
        RemoveMonPPBonus(gPartyMenu.selectedMon, GetMoveSlotToReplace());
        SetMonMoveSlot(gPartyMenu.selectedMon, gPartyMenu.moveToLearn, GetMoveSlotToReplace());
        Task_TeamMonTMMove2(taskId);
    }
}

void Task_WaitToShowStopTryingMoveMessage(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        gTasks[taskId].func = TaskDummy;
        SetPartyMenuItemUseContext(taskId, gSpecialVar_ItemId, Task_ExitPartyMenuToBag);
        StringCopy(gStringVar2, gMoveNames[gPartyMenu.moveToLearn]);
        StringExpandPlaceholders(gStringVar4, gOtherText_StopTryingTo);
        DisplayPartyMenuMessage(gStringVar4, 1);
        CreateTask(Task_ShowStopTryingMovePrompt, 5);
    }
}

void Task_ShowStopTryingMoveMessage(u8 taskId)
{
    Menu_EraseWindowRect(23, 8, 29, 13);
    StringCopy(gStringVar2, gMoveNames[gPartyMenu.moveToLearn]);
    StringExpandPlaceholders(gStringVar4, gOtherText_StopTryingTo);
    DisplayPartyMenuMessage(gStringVar4, 1);
    gTasks[taskId].func = Task_ShowStopTryingMovePrompt;
}

void Task_ShowStopTryingMovePrompt(u8 taskId)
{
    if (gPartyMenuMessage_IsPrinting == 0)
    {
        DisplayYesNoMenu(23, 8, 1);
        gTasks[taskId].func = Task_HandleStopTryingMovePrompt;
    }
}

void Task_HandleStopTryingMovePrompt(u8 taskId)
{
    s8 selection = Menu_ProcessInputNoWrap_();

    if (selection == 0)
    {
        Menu_EraseWindowRect(23, 8, 29, 13);
        GetMonNickname(gPartyMenu.selectedMon, gStringVar1);
        StringCopy(gStringVar2, gMoveNames[gPartyMenu.moveToLearn]);
        StringExpandPlaceholders(gStringVar4, gOtherText_DidNotLearnMove2);
        DisplayPartyMenuMessage(gStringVar4, 1);
        gTasks[taskId].func = Task_FinishMoveLearningMessage;
    }
    else
    {
        if (selection != 1 && selection != -1)
            return;
        if (selection == -1)
            PlaySE(SE_SELECT);
        Menu_EraseWindowRect(23, 8, 29, 13);
        GetMonNickname(gPartyMenu.selectedMon, gStringVar1);
        StringCopy(gStringVar2, gMoveNames[gPartyMenu.moveToLearn]);
        StringExpandPlaceholders(gStringVar4, gOtherText_WantsToLearn);
        DisplayPartyMenuMessage(gStringVar4, 1);
        gTasks[taskId].func = Task_ShowForgetMovePrompt;
    }
}

bool8 IsMoveHm(u16 move)
{
    u8 i;

    for (i = 0; i < NUM_HIDDEN_MACHINES; i++)
    {
        if (sTmHmMoves[NUM_TECHNICAL_MACHINES + i] == move)
            return TRUE;
    }

    return FALSE;
}

s16 UpdatePartyMenuHealthBar(u8 taskId, struct HealthBarInfo *b, s8 c)
{
    s16 *taskData;
    u8 hpBarLevel;
    void *vramPtr;

    taskData = gTasks[taskId].data;

    b->maxValue = taskData[10];
    b->oldValue = taskData[11];
    b->receivedValue = taskData[12] * c;
    b->unk10 = 0x100;
    hpBarLevel = GetHPBarLevel(ePartyMenu2.pmPartyMenuHpBarValue, b->maxValue);
    if (hpBarLevel > 2)
        b->unkC_0 = 4;
    if (hpBarLevel == 2)
        b->unkC_0 = 5;
    if (hpBarLevel < 2)
        b->unkC_0 = 6;
    vramPtr = sPartyMenuHpBarTilemapPtrs[IsDoubleBattle()][gPartyMenu.selectedMonIndex];
    return UpdateHealthBar(b, (int *)&gPartyMenu.hpBarAnimationState, vramPtr, 0);
}

void Task_AnimatePartyMenuHpRestore(u8 taskId)
{
    struct HealthBarInfo sp0;
    u16 sp14;

    ePartyMenu2.pmPartyMenuHpBarValue = UpdatePartyMenuHealthBar(taskId, &sp0, -1);
    if (ePartyMenu2.pmPartyMenuHpBarValue == -1)
    {
        gPartyMenu.hpBarAnimationState = 0;
        if (-sp0.receivedValue + sp0.oldValue > sp0.maxValue)
            sp14 = sp0.maxValue - sp0.oldValue;
        else
            sp14 = -sp0.receivedValue;
        gBattleMoveDamage = -sp14;
        GetMonNickname(gPartyMenu.selectedMon, gStringVar1);
        ConvertIntToDecimalStringN(gStringVar2, sp14, 0, 3);
        if (gTasks[taskId].data[14] == 0)
            StringExpandPlaceholders(gStringVar4, gOtherText_HPRestoredBy);
        else
            StringExpandPlaceholders(gStringVar4, gOtherText_RegainedHealth);
        SetMonIconAnim(GetMonIconSpriteId(gPartyMenu.menuTaskId, gPartyMenu.selectedMonIndex), gPartyMenu.selectedMon);
        DrawPartyMenuMonBackgroundAt(&sPartyMenuMonBackgroundCoords[IsDoubleBattle() * 12 + gPartyMenu.selectedMonIndex * 2], 7);
        ePartyMenu2.backgroundRedrawState = 2;
        DisplayPartyMenuMessage(gStringVar4, 1);
        sp14 += sp0.oldValue;
        SetMonData(gPartyMenu.selectedMon, MON_DATA_HP, &sp14);
        RemoveBagItem(gPartyMenu.itemOrMoveId, 1);
        HandleBattleLowHpMusicChange();
        gTasks[taskId].func = Task_WaitForPartyMenuMessageThenFollowup;
    }
    else
    {
        PartyMenuDoPrintHP(gPartyMenu.selectedMonIndex, IsDoubleBattle(), ePartyMenu2.pmPartyMenuHpBarValue, sp0.maxValue);
    }
}

void Task_SoftboiledReduceUserHp(u8 taskId)
{
    struct HealthBarInfo sp0;

    ePartyMenu2.pmPartyMenuHpBarValue = UpdatePartyMenuHealthBar(taskId, &sp0, 1);
    if (ePartyMenu2.pmPartyMenuHpBarValue == -1)
    {
        PlaySE(SE_USE_ITEM);
        gPartyMenu.hpBarAnimationState = 0;
        gTasks[taskId].data[11] -= gTasks[taskId].data[12];
        SetMonData(gPartyMenu.selectedMon, MON_DATA_HP, &gTasks[taskId].data[11]);
        SetMonIconAnim(GetMonIconSpriteId(gPartyMenu.menuTaskId, ePartyMenu.firstSelectedMonSpriteId), gPartyMenu.selectedMon);
        gPartyMenu.selectedMonIndex = gSprites[ePartyMenu.currentSelectionSpriteId].data[0];
        gPartyMenu.selectedMon = &gPlayerParty[gPartyMenu.selectedMonIndex];
        gTasks[taskId].data[10] = GetMonData(gPartyMenu.selectedMon, MON_DATA_MAX_HP);
        gTasks[taskId].data[11] = GetMonData(gPartyMenu.selectedMon, MON_DATA_HP);
        gPartyMenu.hpBarAnimationState = -32768;
        gPartyMenu.afterHpAnimationCallback = gPartyMenu.followupCallback;
        gTasks[taskId].func = Task_AnimatePartyMenuHpRestore;
        ePartyMenu2.pmPartyMenuHpBarValue = gTasks[taskId].data[11];
    }
    else
    {
        PartyMenuDoPrintHP(gPartyMenu.selectedMonIndex, IsDoubleBattle(), ePartyMenu2.pmPartyMenuHpBarValue, sp0.maxValue);
    }
}

void Task_WaitForPartyMenuMessage(u8 taskId)
{
    if (gPartyMenuMessage_IsPrinting == 0)
    {
        gTasks[gPartyMenu.menuTaskId].func = gPartyMenu.followupCallback;
        DestroyTask(taskId);
    }
}

void Task_WaitForPartyMenuMessageThenFollowup(u8 taskId)
{
    if (gPartyMenuMessage_IsPrinting == 0)
    {
        gTasks[gPartyMenu.menuTaskId].func = gPartyMenu.afterHpAnimationCallback;
        DestroyTask(taskId);
    }
}

bool8 IsHPRecoveryItem(u16 item)
{
    const u8 *itemEffect;

    if (item == ITEM_ENIGMA_BERRY)
        itemEffect = gSaveBlock1.enigmaBerry.itemEffect;
    else
        itemEffect = gItemEffectTable[item - ITEM_POTION];

    if (itemEffect[4] & 4)
        return TRUE;
    else
        return FALSE;
}

void PartyMenuUpdateLevelOrStatus(struct Pokemon *pkmn, u8 monIndex)
{
    PartyMenuClearLevelStatusTilemap(monIndex);
    PartyMenuPrintMonLevelOrStatus(monIndex, pkmn);
}

void GetMedicineItemEffectMessage(u16 item)
{
    switch (GetItemEffectType(item))
    {
    case 3:
        StringExpandPlaceholders(gStringVar4, gOtherText_CuredPoisoning);
        break;
    case 4:
        StringExpandPlaceholders(gStringVar4, gOtherText_WokeUp);
        break;
    case 5:
        StringExpandPlaceholders(gStringVar4, gOtherText_BurnHealed);
        break;
    case 6:
        StringExpandPlaceholders(gStringVar4, gOtherText_ThawedOut);
        break;
    case 7:
        StringExpandPlaceholders(gStringVar4, gOtherText_CuredParalysis);
        break;
    case 8:
        StringExpandPlaceholders(gStringVar4, gOtherText_SnapConfusion);
        break;
    case 9:
        StringExpandPlaceholders(gStringVar4, gOtherText_GotOverLove);
        break;
    case 11:
        StringExpandPlaceholders(gStringVar4, gOtherText_BecameHealthy);
        break;
    case 13:
        StringCopy(gStringVar2, gOtherText_Hp2);
        StringExpandPlaceholders(gStringVar4, gOtherText_WasRaised);
        break;
    case 12:
        StringCopy(gStringVar2, gOtherText_Attack);
        StringExpandPlaceholders(gStringVar4, gOtherText_WasRaised);
        break;
    case 17:
        StringCopy(gStringVar2, gOtherText_Defense);
        StringExpandPlaceholders(gStringVar4, gOtherText_WasRaised);
        break;
    case 16:
        StringCopy(gStringVar2, gOtherText_Speed);
        StringExpandPlaceholders(gStringVar4, gOtherText_WasRaised);
        break;
    case 14:
        StringCopy(gStringVar2, gOtherText_SpAtk2);
        StringExpandPlaceholders(gStringVar4, gOtherText_WasRaised);
        break;
    case 15:
        StringCopy(gStringVar2, gOtherText_SpDef2);
        StringExpandPlaceholders(gStringVar4, gOtherText_WasRaised);
        break;
    case 19:
    case 20:
        StringExpandPlaceholders(gStringVar4, gOtherText_PPIncreased);
        break;
    case 21:
        StringExpandPlaceholders(gStringVar4, gOtherText_PPRestored);
        break;
    default:
        StringExpandPlaceholders(gStringVar4, gOtherText_WontHaveAnyEffect);
        break;
    }
}

bool8 IsMedicineIneffective(struct Pokemon *pkmn, u16 item)
{
    if (GetItemEffectType(item) == 13
     && GetMonData(pkmn, MON_DATA_SPECIES) == SPECIES_SHEDINJA)
        return TRUE;
    else
        return FALSE;
}

bool8 ExecuteTableBasedItemEffectForPartyMon(u8 monIndex, u16 item, u8 moveIndex)
{
    if (gMain.inBattle)
        return ExecuteTableBasedItemEffect(&gPlayerParty[monIndex], item, GetPartyIdFromBattleSlot(monIndex), moveIndex);
    else
        return ExecuteTableBasedItemEffect(&gPlayerParty[monIndex], item, monIndex, moveIndex);
}

void UseMedicine(u8 taskId, u16 item, TaskFunc func)
{
    u8 r7;
    bool8 r9 = FALSE;
    bool8 r0;

    gTasks[taskId].func = TaskDummy;
    r7 = CreateTask(TaskDummy, 5);
    SetPartyMenuItemUseContext(taskId, item, func);
    if (!IsMedicineIneffective(gPartyMenu.selectedMon, item))
    {
        r9 = IsHPRecoveryItem(item);
        if (r9 == TRUE)
        {
            gTasks[r7].data[10] = GetMonData(gPartyMenu.selectedMon, MON_DATA_MAX_HP);
            gTasks[r7].data[11] = GetMonData(gPartyMenu.selectedMon, MON_DATA_HP);
            if (gTasks[r7].data[10] == gTasks[r7].data[11])
            {
                r9 = FALSE;
                gTasks[r7].data[10] = 0;
                gTasks[r7].data[11] = 0;
            }
        }
        r0 = ExecuteTableBasedItemEffectForPartyMon(gPartyMenu.selectedMonIndex, item, 0);
    }
    else
    {
        r0 = TRUE;
    }

    if (r0)
    {
        gPartyMenuActionResult = 0;
        PlaySE(SE_SELECT);
        DisplayPartyMenuMessage(gOtherText_WontHaveAnyEffect, 1);
        gTasks[r7].func = Task_WaitForPartyMenuMessage;
    }
    else
    {
        u8 statusAndPkrs;

        gPartyMenuActionResult = 1;
        if (!IsBlueYellowRedFlute(item))
            PlaySE(SE_USE_ITEM);
        else
            PlaySE(SE_GLASS_FLUTE);
        statusAndPkrs = GetMonStatusAndPokerus(gPartyMenu.selectedMon);
        if (statusAndPkrs == STATUS_PRIMARY_POKERUS || statusAndPkrs == STATUS_PRIMARY_NONE)
            PartyMenuUpdateLevelOrStatus(gPartyMenu.selectedMon, gPartyMenu.selectedMonIndex);
        if (r9 == TRUE)
        {
            gTasks[r7].data[12] = GetMonData(gPartyMenu.selectedMon, MON_DATA_HP) - gTasks[r7].data[11];
            gPartyMenu.hpBarAnimationState = -32768;
            if (gTasks[r7].data[11] == 0)
                gTasks[r7].data[14] = 1;
            else
                gTasks[r7].data[14] = 0;
            gPartyMenu.afterHpAnimationCallback = gPartyMenu.followupCallback;
            gTasks[r7].func = Task_AnimatePartyMenuHpRestore;
            ePartyMenu2.pmPartyMenuHpBarValue = gTasks[r7].data[11];
        }
        else
        {
            GetMonNickname(gPartyMenu.selectedMon, gStringVar1);
            if (!IsBlueYellowRedFlute(item))
                RemoveBagItem(item, 1);
            GetMedicineItemEffectMessage(item);
            TryPrintPartyMenuMonNickname(gPartyMenu.selectedMonIndex, gPartyMenu.selectedMon);
            DisplayPartyMenuMessage(gStringVar4, 1);
            gTasks[r7].func = Task_WaitForPartyMenuMessage;
        }
    }
}

bool8 IsBlueYellowRedFlute(u16 item)
{
    if (item == ITEM_BLUE_FLUTE
     || item == ITEM_RED_FLUTE
     || item == ITEM_YELLOW_FLUTE)
        return TRUE;
    else
        return FALSE;
}

void DoSacredAshItemEffect(u8 taskId, u16 item, TaskFunc func)
{
    gPartyMenu.followupCallback = func;
    gPartyMenu.menuTaskId = taskId;
    gPartyMenu.itemOrMoveId = item;
    gPartyMenu.selectedMonIndex = 0;
    gPartyMenu.afterHpAnimationCallback = FinishSacredAshPartyMonRestore;
    ePartyMenu2.pmSacredAshRestoreInProgress = FALSE;
    ePartyMenu2.pmSacredAshHadEffect = FALSE;
    TryUseSacredAshOnNextPartyMon(taskId);
}

void TryUseSacredAshOnNextPartyMon(u8 taskId)
{
    u8 taskId2;

    gTasks[taskId].func = TaskDummy;
    if (GetMonData(&gPlayerParty[gPartyMenu.selectedMonIndex], MON_DATA_SPECIES) == SPECIES_NONE)
    {
        gTasks[taskId].func = FinishSacredAshPartyMonRestore;
    }
    else
    {
        s16 *taskData;

        taskId2 = CreateTask(TaskDummy, 5);
        taskData = gTasks[taskId2].data;
        gPartyMenu.selectedMon = &gPlayerParty[gPartyMenu.selectedMonIndex];
        taskData[10] = GetMonData(gPartyMenu.selectedMon, MON_DATA_MAX_HP);
        taskData[11] = GetMonData(gPartyMenu.selectedMon, MON_DATA_HP);
        if (ExecuteTableBasedItemEffectForPartyMon(gPartyMenu.selectedMonIndex, gPartyMenu.itemOrMoveId, 0))
        {
            DestroyTask(taskId2);
            gTasks[taskId].func = FinishSacredAshPartyMonRestore;
        }
        else
        {
            gPartyMenuActionResult = 1;
            Menu_EraseWindowRect(WINDOW_LEFT, 14, WINDOW_RIGHT, 19);
            PlaySE(SE_USE_ITEM);
            PartyMenuUpdateLevelOrStatus(gPartyMenu.selectedMon, gPartyMenu.selectedMonIndex);
            DrawPartyMenuMonBackgroundAt(&sPartyMenuMonBackgroundCoords[IsDoubleBattle() * 12 + gPartyMenu.selectedMonIndex * 2], 9);
            ePartyMenu2.backgroundRedrawState = 2;
            taskData[12] = GetMonData(gPartyMenu.selectedMon, MON_DATA_HP) - taskData[11];
            taskData[14] = 1;
            ePartyMenu2.pmSacredAshRestoreInProgress = TRUE;
            ePartyMenu2.pmSacredAshHadEffect = TRUE;
            ePartyMenu2.pmPartyMenuHpBarValue = taskData[11];
            gPartyMenu.hpBarAnimationState = -32768;
            gTasks[taskId2].func = Task_AnimatePartyMenuHpRestore;
        }
    }
}

void FinishSacredAshPartyMonRestore(u8 taskId)
{
    if (ePartyMenu2.pmSacredAshRestoreInProgress == TRUE)
    {
        AddBagItem(gPartyMenu.itemOrMoveId, 1);
        if (GetMonData(&gPlayerParty[gPartyMenu.selectedMonIndex], MON_DATA_SPECIES) != 0)
        {
            DrawPartyMenuMonBackgroundAt(&sPartyMenuMonBackgroundCoords[IsDoubleBattle() * 12 + gPartyMenu.selectedMonIndex * 2], 3);
            ePartyMenu2.backgroundRedrawState = 2;
        }
        ePartyMenu2.pmSacredAshRestoreInProgress = FALSE;
    }
    gPartyMenu.selectedMonIndex++;
    if (gPartyMenu.selectedMonIndex == PARTY_SIZE)
    {
        gPartyMenuActionResult = 0;
        if (ePartyMenu2.pmSacredAshHadEffect == FALSE)
        {
            gTasks[taskId].func = TaskDummy;
            DisplayPartyMenuMessage(gOtherText_WontHaveAnyEffect, 1);
            CreateTask(Task_WaitForPartyMenuMessage, 8);
        }
        else
        {
            RemoveBagItem(gPartyMenu.itemOrMoveId, 1);
            gTasks[taskId].func = gPartyMenu.followupCallback;
        }
        gLastFieldPokeMenuOpened = 0;
    }
    else
    {
        TryUseSacredAshOnNextPartyMon(taskId);
    }
}

void CreateItemUseMoveMenu(u8 partyMonIndex)
{
    u8 r6;
    u8 i;

    r6 = 0;
    Menu_DrawStdWindowFrame(19, 10, 29, 19);
    for (i = 0; i < 4; i++)
    {
        u16 move = GetMonData(&gPlayerParty[partyMonIndex], MON_DATA_MOVE1 + i);

        Menu_PrintText(gMoveNames[move], 20, i * 2 + 11);
        if (move != 0)
            r6++;
    }
    InitMenu(0, 20, 11, r6, 0, 9);
}

void Task_HandleItemUseMoveMenuInput(u8 taskId)
{
    if (JOY_NEW(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        Menu_MoveCursor(-1);
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        Menu_MoveCursor(1);
    }
    else if (JOY_NEW(A_BUTTON))
    {
        sItemUseMoveMenuHandlers[0](taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        sItemUseMoveMenuHandlers[1](taskId);
    }
}

void DoPPRecoveryItemEffect(u8 taskId, u16 item, TaskFunc c)
{
    const u8 *itemEffect;
    u8 taskId2;

    if (item == ITEM_ENIGMA_BERRY)
        itemEffect = gSaveBlock1.enigmaBerry.itemEffect;
    else
        itemEffect = gItemEffectTable[item - ITEM_POTION];
    gTasks[taskId].func = TaskDummy;
    taskId2 = CreateTask(TaskDummy, 5);
    SetPartyMenuItemUseContext(taskId, item, c);
    if (!(itemEffect[4] & 0x10))
    {
        gTasks[taskId2].data[11] = 0;
        DoRecoverPP(taskId2);
    }
    else
    {
        PlaySE(SE_SELECT);
        PrintPartyMenuPromptText(10, 3);
        CreateItemUseMoveMenu(gPartyMenu.selectedMonIndex);
        gTasks[taskId2].func = Task_HandleItemUseMoveMenuInput;
        gMain.newKeys = 0;
    }
}

void ItemUseMoveMenu_HandleMoveSelection(u8 taskId)
{
    Menu_DestroyCursor();
    Menu_EraseWindowRect(19, 10, 29, 19);
    PartyMenuEraseMsgBoxAndFrame();
    gTasks[taskId].data[11] = Menu_GetCursorPos();
    DoRecoverPP(taskId);
}

void ItemUseMoveMenu_HandleCancel(u8 taskId)
{
    Menu_DestroyCursor();
    Menu_EraseWindowRect(19, 10, 29, 19);
    if (gMain.inBattle)
        gTasks[gPartyMenu.menuTaskId].func = HandleBattlePartyMenu;
    else
        gTasks[gPartyMenu.menuTaskId].func = Task_HandlePartyMenuItemUseInput;
    PrintPartyMenuPromptText(3, 0);
    DestroyTask(taskId);
}

void DoRecoverPP(u8 taskId)
{
    u16 r5 = 0;

    if (ExecuteTableBasedItemEffectForPartyMon(gPartyMenu.selectedMonIndex, gPartyMenu.itemOrMoveId, gTasks[taskId].data[11]))
    {
        gPartyMenuActionResult = 0;
        PlaySE(SE_SELECT);
        DisplayPartyMenuMessage(gOtherText_WontHaveAnyEffect, 1);
    }
    else
    {
        gPartyMenuActionResult = 1;
        PlaySE(SE_USE_ITEM);
        RemoveBagItem(gPartyMenu.itemOrMoveId, 1);
        r5 = GetMonData(gPartyMenu.selectedMon, MON_DATA_MOVE1 + gTasks[taskId].data[11]);
        StringCopy(gStringVar1, gMoveNames[r5]);
        GetMedicineItemEffectMessage(gPartyMenu.itemOrMoveId);
        DisplayPartyMenuMessage(gStringVar4, 1);
    }
    gTasks[taskId].func = Task_WaitForPartyMenuMessage;
}

void DoPPUpItemEffect(u8 taskId, u16 b, TaskFunc c)
{
    gTasks[taskId].func = TaskDummy;
    SetPartyMenuItemUseContext(taskId, b, c);
    PlaySE(SE_SELECT);
    PrintPartyMenuPromptText(11, 3);
    CreateItemUseMoveMenu(gPartyMenu.selectedMonIndex);
    CreateTask(Task_HandleItemUseMoveMenuInput, 5);
    gMain.newKeys = 0;
}

static const u8 *const sStatNames[] =
{
    gOtherText_HP,
    gOtherText_Attack,
    gOtherText_Defense,
    gOtherText_SpAtk,
    gOtherText_SpDef,
    gOtherText_Speed,
};

static const u8 sStatDataTypes[] =
{
    MON_DATA_MAX_HP,
    MON_DATA_ATK,
    MON_DATA_DEF,
    MON_DATA_SPATK,
    MON_DATA_SPDEF,
    MON_DATA_SPEED,
};

#if DEBUG

const u8 gDebugEnigmaBerryItemEffect[] = {0x40, 0, 0, 0, 0x44, 0, 0xFF, 0, 0, 0};

const u8 gDebugText_ExecutionCountAndTime[] = _(
    "{STR_VAR_1}かい　じっこう\n"
    "{STR_VAR_2}たんいじかん　しょうひ");

#endif

void DoRareCandyItemEffect(u8 taskId, u16 item, TaskFunc c)
{
    u8 i;
    bool8 noEffect;

    gTasks[taskId].func = TaskDummy;
    SetPartyMenuItemUseContext(taskId, item, c);

    if (GetMonData(gPartyMenu.selectedMon, MON_DATA_LEVEL) != 100)
    {
        for (i = 0; i < NUM_STATS; i++)
            ePartyMenu2.pmStatGrowths[i] = GetMonData(gPartyMenu.selectedMon, sStatDataTypes[i]);
        noEffect = ExecuteTableBasedItemEffectForPartyMon(gPartyMenu.selectedMonIndex, item, 0);
    }
    else
        noEffect = TRUE;

    if (noEffect)
    {
        gPartyMenuActionResult = 0;
        PlaySE(SE_SELECT);
        DisplayPartyMenuMessage(gOtherText_WontHaveAnyEffect, 1);
        CreateTask(Task_WaitForPartyMenuMessage, 5);
    }
    else
    {
        u8 level;

        gPartyMenuActionResult = 1;
        PlayFanfareByFanfareNum(0);
        RedrawPokemonInfoInMenu(gPartyMenu.selectedMonIndex, gPartyMenu.selectedMon);
        RemoveBagItem(item, 1);
        GetMonNickname(gPartyMenu.selectedMon, gStringVar1);
        level = GetMonData(gPartyMenu.selectedMon, MON_DATA_LEVEL);
        ConvertIntToDecimalStringN(gStringVar2, level, 0, 3);
        StringExpandPlaceholders(gStringVar4, gOtherText_ElevatedTo);
        DisplayPartyMenuMessage(gStringVar4, 1);
        CreateTask(Task_RareCandy1, 5);
    }
}

void Task_RareCandy1(u8 taskId)
{
    if (WaitFanfare(0) && gPartyMenuMessage_IsPrinting == 0)
    {
        if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            PrintStatGrowthsInLevelUpWindow(taskId);
            gTasks[taskId].func = Task_RareCandy2;
        }
    }
}

void Task_RareCandy2(u8 taskId)
{
    if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        PrintNewStatsInLevelUpWindow(taskId);
        gTasks[taskId].func = Task_RareCandy3;
    }
}

void PrintStatGrowthsInLevelUpWindow(u8 taskId)
{
    u8 i;

    Menu_DrawStdWindowFrame(11, 0, 29, 7);

    for (i = 0; i < NUM_STATS; i++)
    {
        u8 x;
        u8 y;
        u32 stat;
#if GERMAN
        u8 *ptr;
#endif

        stat = GetMonData(gPartyMenu.selectedMon, sStatDataTypes[i]);

        ePartyMenu2.pmStatGrowths[i + NUM_STATS] = stat;
        ePartyMenu2.pmStatGrowths[i] = stat - ePartyMenu2.pmStatGrowths[i];

        x = (i / 3) * 9 + 11;
        y = ((i % 3) << 1) + 1;

#if GERMAN
        ptr = StringCopy(gStringVar1, sStatNames[i]);
        *ptr++ = EXT_CTRL_CODE_BEGIN;
        *ptr++ = 0x13;
        *ptr++ = 0x2E;
        ptr = StringCopy(ptr, gOtherText_TallPlusAndRightArrow);
        *ptr++ = EXT_CTRL_CODE_BEGIN;
        *ptr++ = 0x13;
        *ptr++ = 0x34;
        ConvertIntToDecimalStringN(ptr, ePartyMenu2.pmStatGrowths[i], 1, 2);
        Menu_PrintText(gStringVar1, x + 1, y);
#else
        Menu_PrintTextPixelCoords(sStatNames[i], (x + 1) * 8, y * 8, 1);
        if (i == 2)
            Menu_PrintTextPixelCoords(gOtherText_TallPlusAndRightArrow, (x + 6) * 8 + 6, y * 8, 0);
        else
            Menu_PrintTextPixelCoords(gOtherText_TallPlusAndRightArrow, (x + 6) * 8 + 6, y * 8, 1);
        gStringVar1[0] = EXT_CTRL_CODE_BEGIN;
        gStringVar1[1] = 0x14;
        gStringVar1[2] = 0x06;
        ConvertIntToDecimalStringN(gStringVar1 + 3, ePartyMenu2.pmStatGrowths[i], 1, 2);
        Menu_PrintTextPixelCoords(gStringVar1, (x + 6) * 8 + 12, y * 8, 0);
#endif
    }
}

void PrintNewStatsInLevelUpWindow(u8 taskId)
{
    u8 i;

    for (i = 0; i < NUM_STATS; i++)
    {
        u8 x;
        u8 y;
        u32 stat;
        u32 newStatIndex;

        stat = GetMonData(gPartyMenu.selectedMon, sStatDataTypes[i]);
        newStatIndex = i + 6;
        ePartyMenu2.pmStatGrowths[newStatIndex] = stat;

        x = ((i / 3) * 9) + 11;
        y = ((i % 3) << 1) + 1;

        gStringVar1[0] = EXT_CTRL_CODE_BEGIN;
        gStringVar1[1] = 0x14;
        gStringVar1[2] = 0x06;

        ConvertIntToDecimalStringN(gStringVar1 + 3, ePartyMenu2.pmStatGrowths[newStatIndex], 1, 3);
        Menu_PrintTextPixelCoords(gStringVar1, (x + 6) * 8 + 6, y * 8, 0);
    }
}

void RedrawPokemonInfoInMenu(u8 monIndex, struct Pokemon *pokemon)
{
    u8 statusAndPkrs = GetMonStatusAndPokerus(pokemon);
    u8 icon;

    if (statusAndPkrs == STATUS_PRIMARY_NONE || statusAndPkrs == STATUS_PRIMARY_POKERUS)
        PartyMenuUpdateLevelOrStatus(pokemon, monIndex);

    PartyMenuDoPrintHP(monIndex, IsDoubleBattle(), GetMonData(pokemon, MON_DATA_HP), GetMonData(pokemon, MON_DATA_MAX_HP));
    PartyMenuTryDrawHPBar(monIndex, pokemon);

    icon = GetMonIconSpriteId(gPartyMenu.menuTaskId, monIndex);
    SetMonIconAnim(icon, pokemon);

    DrawPartyMenuMonBackgroundAt(&sPartyMenuMonBackgroundCoords[IsDoubleBattle() * 12 + monIndex * 2], 7);
    ePartyMenu2.backgroundRedrawState = 2;
}

void Task_RareCandy3(u8 taskId)
{
    if (WaitFanfare(0))
    {
        if (JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
        {
            u16 learnedMove;
            u16 evolutionSpecies;

            Menu_EraseWindowRect(11, 0, 29, 7);

            learnedMove = MonTryLearningNewMove(gPartyMenu.selectedMon, TRUE);
            ePartyMenu2.pmMoveLearningFromLevelUp = TRUE;

            switch (learnedMove)
            {
            case 0:
                // No move is learned.
                evolutionSpecies = GetEvolutionTargetSpecies(gPartyMenu.selectedMon, EVO_MODE_NORMAL, 0);
                if (evolutionSpecies != 0)
                {
                    gCB2_AfterEvolution = CB2_BagMenuFromStartMenu;
                    BeginEvolutionScene(gPartyMenu.selectedMon, evolutionSpecies, TRUE, gPartyMenu.selectedMonIndex);
                    DestroyTask(taskId);
                }
                else
                {
                    Task_FinishPartyMenuMoveLearning(taskId);
                }
                break;
            case MON_HAS_MAX_MOVES:
                // Mon already knows 4 moves.
                GetMonNickname(gPartyMenu.selectedMon, gStringVar1);
                StringCopy(gStringVar2, gMoveNames[gMoveToLearn]);

                StringExpandPlaceholders(gStringVar4, gOtherText_WantsToLearn);
                DisplayPartyMenuMessage(gStringVar4, 1);

                gPartyMenu.moveToLearn = gMoveToLearn;
                gTasks[taskId].func = Task_ShowForgetMovePrompt;
                break;
            case MON_ALREADY_KNOWS_MOVE:
                // Move was already known by the mon.
                gTasks[taskId].func = TeachMonMoveInPartyMenu;
                break;
            default:
                // Mon automatically learned a move because it knew less than four moves.
                GetMonNickname(gPartyMenu.selectedMon, gStringVar1);
                StringCopy(gStringVar2, gMoveNames[learnedMove]);

                StringExpandPlaceholders(gStringVar4, gOtherText_LearnedMove);
                DisplayPartyMenuMessage(gStringVar4, 1);

                gTasks[taskId].func = Task_TeamMonTMMove3;
                break;
            }
        }
    }
}

void TeachMonMoveInPartyMenu(u8 taskId)
{
    u16 learnedMove;
    u16 evolutionSpecies;

    learnedMove = MonTryLearningNewMove(gPartyMenu.selectedMon, FALSE);
    switch (learnedMove)
    {
    case 0:
        // No move is learned.
        evolutionSpecies = GetEvolutionTargetSpecies(gPartyMenu.selectedMon, EVO_MODE_NORMAL, 0);
        if (evolutionSpecies != 0)
        {
            gCB2_AfterEvolution = CB2_BagMenuFromStartMenu;
            BeginEvolutionScene(gPartyMenu.selectedMon, evolutionSpecies, TRUE, gPartyMenu.selectedMonIndex);
            DestroyTask(taskId);
        }
        else
        {
            Task_FinishPartyMenuMoveLearning(taskId);
        }
        break;
    case MON_HAS_MAX_MOVES:
        // Mon already knows 4 moves.
        GetMonNickname(gPartyMenu.selectedMon, gStringVar1);
        StringCopy(gStringVar2, gMoveNames[gMoveToLearn]);

        StringExpandPlaceholders(gStringVar4, gOtherText_WantsToLearn);
        DisplayPartyMenuMessage(gStringVar4, 1);

        gPartyMenu.moveToLearn = gMoveToLearn;
        gTasks[taskId].func = Task_ShowForgetMovePrompt;
        break;
    case MON_ALREADY_KNOWS_MOVE:
        // Move was already known by the mon. Go on the the next move to be learned.
        TeachMonMoveInPartyMenu(taskId);
        break;
    default:
        // Mon automatically learned a move because it knew less than four moves.
        GetMonNickname(gPartyMenu.selectedMon, gStringVar1);
        StringCopy(gStringVar2, gMoveNames[learnedMove]);

        StringExpandPlaceholders(gStringVar4, gOtherText_LearnedMove);
        DisplayPartyMenuMessage(gStringVar4, 1);

        gTasks[taskId].func = Task_TeamMonTMMove3;
        break;
    }
}

static void Task_FinishPartyMenuMoveLearning(u8 taskId)
{
    gTasks[gPartyMenu.menuTaskId].func = gPartyMenu.followupCallback;
    DestroyTask(taskId);
}

void DoEvolutionStoneItemEffect(u8 taskId, u16 evolutionStoneItem, TaskFunc c)
{
    PlaySE(SE_SELECT);

    gTasks[taskId].func = TaskDummy;
    SetPartyMenuItemUseContext(taskId, evolutionStoneItem, c);

    gCB2_AfterEvolution = CB2_BagMenuFromStartMenu;

    if (ExecuteTableBasedItemEffectForPartyMon(gPartyMenu.selectedMonIndex, evolutionStoneItem, 0))
    {
        gPartyMenuActionResult = 0;
        DisplayPartyMenuMessage(gOtherText_WontHaveAnyEffect, 1);

        CreateTask(Task_WaitForPartyMenuMessage, 5);
    }
    else
    {
        RemoveBagItem(evolutionStoneItem, 1);
    }
}

u8 GetItemEffectType(u16 item)
{
    const u8 *itemEffect;
    u32 statusCure;

#ifdef UBFIX
    // Enforces that the item ID is within the bounds of gItemEffectTable.
    // In the retail ROM, the otherwise situation will never arise.
    // Nevertheless, GameFreak added this sanity check in FireRed.
    if (!ITEM_HAS_EFFECT(item))
        return ITEM_EFFECT_NONE;
#endif

    // Read the item's effect properties.
    if (item == ITEM_ENIGMA_BERRY)
        itemEffect = gSaveBlock1.enigmaBerry.itemEffect;
    else
        itemEffect = gItemEffectTable[item - ITEM_POTION];

    if ((itemEffect[0] & (ITEM0_DIRE_HIT | ITEM0_X_ATTACK)) || itemEffect[1] || itemEffect[2] || (itemEffect[3] & ITEM3_GUARD_SPEC))
        return ITEM_EFFECT_X_ITEM;
    else if (itemEffect[0] & ITEM0_SACRED_ASH)
        return ITEM_EFFECT_SACRED_ASH;
    else if (itemEffect[3] & ITEM3_LEVEL_UP)
        return ITEM_EFFECT_RAISE_LEVEL;

    statusCure = itemEffect[3] & ITEM3_STATUS_ALL;
    if (statusCure || (itemEffect[0] >> 7))
    {
        if (statusCure == ITEM3_SLEEP)
            return ITEM_EFFECT_CURE_SLEEP;
        else if (statusCure == ITEM3_POISON)
            return ITEM_EFFECT_CURE_POISON;
        else if (statusCure == ITEM3_BURN)
            return ITEM_EFFECT_CURE_BURN;
        else if (statusCure == ITEM3_FREEZE)
            return ITEM_EFFECT_CURE_FREEZE;
        else if (statusCure == ITEM3_PARALYSIS)
            return ITEM_EFFECT_CURE_PARALYSIS;
        else if (statusCure == ITEM3_CONFUSION)
            return ITEM_EFFECT_CURE_CONFUSION;
        else if (itemEffect[0] >> 7 && !statusCure)
            return ITEM_EFFECT_CURE_INFATUATION;
        else
            return ITEM_EFFECT_CURE_ALL_STATUS;
    }

    if (itemEffect[4] & (ITEM4_REVIVE | ITEM4_HEAL_HP))
        return ITEM_EFFECT_HEAL_HP;
    else if (itemEffect[4] & ITEM4_EV_ATK)
        return ITEM_EFFECT_ATK_EV;
    else if (itemEffect[4] & ITEM4_EV_HP)
        return ITEM_EFFECT_HP_EV;
    else if (itemEffect[5] & ITEM5_EV_SPATK)
        return ITEM_EFFECT_SPATK_EV;
    else if (itemEffect[5] & ITEM5_EV_SPDEF)
        return ITEM_EFFECT_SPDEF_EV;
    else if (itemEffect[5] & ITEM5_EV_SPEED)
        return ITEM_EFFECT_SPEED_EV;
    else if (itemEffect[5] & ITEM5_EV_DEF)
        return ITEM_EFFECT_DEF_EV;
    else if (itemEffect[4] & ITEM4_EVO_STONE)
        return ITEM_EFFECT_EVO_STONE;
    else if (itemEffect[4] & ITEM4_PP_UP)
        return ITEM_EFFECT_PP_UP;
    else if (itemEffect[5] & ITEM5_PP_MAX)
        return ITEM_EFFECT_PP_MAX;
    else if (itemEffect[4] & (ITEM4_HEAL_PP | ITEM4_HEAL_PP_ONE))
        return ITEM_EFFECT_HEAL_PP;
    else
        return ITEM_EFFECT_NONE;
}

// Maybe this goes in start_menu.c
#if !DEBUG
void EnablePlayerSystemFlags(void)
{
    FlagSet(FLAG_SYS_POKEDEX_GET);
    FlagSet(FLAG_SYS_POKEMON_GET);
    FlagSet(FLAG_SYS_POKENAV_GET);
}
#endif
