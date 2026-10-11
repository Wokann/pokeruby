#include "global.h"
#include "gba/m4a_internal.h"
#include "pokedex.h"
#include "battle.h"
#include "data.h"
#include "decompress.h"
#include "event_data.h"
#include "graphics.h"
#include "m4a.h"
#include "main.h"
#include "menu.h"
#include "menu_cursor.h"
#include "palette.h"
#include "pokedex_area_screen.h"
#include "pokedex_cry_screen.h"
#include "pokemon.h"
#include "random.h"
#include "overworld.h"
#include "constants/songs.h"
#include "sound.h"
#include "constants/species.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "trig.h"
#include "scanline_effect.h"
#include "ewram.h"

struct PokedexListItem
{
    u16 dexNum;
    u16 seen:1;
    u16 owned:1;
};

struct PokedexView
{
    struct PokedexListItem pokedexList[NATIONAL_DEX_COUNT + 1];
    u16 pokemonListCount;
    u16 selectedPokemon;
    u16 selectedPokemonBackup;
    u16 dexMode;
    u16 dexModeBackup;
    u16 dexOrder;
    u16 dexOrderBackup;
    u16 seenCount;
    u16 ownCount;
    u16 monSpriteIds[4];
    u16 selectedMonSpriteId;
    s16 pokeBallRotationStep;
    s16 pokeBallRotationBackup;
    u8 pokeBallRotation;
    u8 initialVOffset;
    u8 scrollTimer;
    u8 scrollDirection;
    s16 listVOffset;
    s16 listMovingVOffset;
    s16 scrollMonIncrement;
    s16 maxScrollTimer;
    u16 scrollSpeed;
    u16 unkArr1[4]; // Cleared, never read
    u8 filler[8];
    u8 currentPage;
    u8 currentPageBackup;
    u8 isSearchResults:1;
    u8 selectedScreen;
    u8 descriptionPageNum;
    u8 screenSwitchState;
    u8 menuIsOpen;
    u16 menuCursorPos;
    s16 menuY;     //Menu Y position (inverted because we use REG_BG0VOFS for this)
    u8 unkArr2[8]; // Cleared, never read
    u8 unkArr3[8]; // Cleared, never read
};

enum
{
    PAGE_SCREEN,
    AREA_SCREEN,
    CRY_SCREEN,
    SIZE_SCREEN
};

struct SearchOptionText
{
    const u8 *description;
    const u8 *title;
};

struct SearchOption
{
    const struct SearchOptionText *texts;
    u8 taskDataCursorPos;
    u8 taskDataScrollOffset;
    u16 numOptions;
};

struct SearchMenuTopBarItem
{
    const u8 *description;
    u8 highlightX;
    u8 highlightY;
    u8 highlightWidth;
};

struct SearchMenuItem
{
    const u8 *description;
    u8 titleBgX;
    u8 titleBgY;
    u8 titleBgWidth;
    u8 selectionBgX;
    u8 selectionBgY;
    u8 selectionBgWidth;
};

extern struct MusicPlayerInfo gMPlayInfo_BGM;
extern u8 gReservedSpritePaletteCount;

static EWRAM_DATA struct PokedexView *sPokedexView = NULL;
static EWRAM_DATA u16 sLastSelectedPokemon = 0;
static EWRAM_DATA u8 sPokeBallRotation = 0;
static EWRAM_DATA struct PokedexListItem *sPokedexListItem = NULL;

u8 gUnusedPokedexU8;
IntrCallback gPokedexVBlankCB;

static u8 LoadSearchMenu(void);
static void EraseAndPrintSearchTextBox(const u8 *str);

const u16 gPokedexSearchResults_Pal[] = INCBIN_U16("graphics/pokedex/search_results_bg.gbapal");
const u16 gPokedexBgNational_Pal[] = INCBIN_U16("graphics/pokedex/bg_national.gbapal");
const u8 gEmptySpacce_839F7FC[0xA4] = {0};
const u8 gPokedexCryScreen_Tilemap[] = INCBIN_U8("graphics/pokedex/cry_screen.bin.lz");
const u8 gPokedexSizeScreen_Tilemap[] = INCBIN_U8("graphics/pokedex/size_screen.bin.lz");
#if ENGLISH
static const u8 sCaughtBall_Gfx[] = INCBIN_U8("graphics/pokedex/caught_ball.4bpp.lz");
#elif GERMAN
extern const u8 sCaughtBall_Gfx[];
#endif

#include "data/pokemon/pokedex_orders.h"
static const struct OamData sOamData_ScrollBar =
{
    .y = 160,
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
static const struct OamData sOamData_ScrollArrow =
{
    .y = 160,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 1,
    .x = 0,
    .matrixNum = 0,
    .size = 0,
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};
static const struct OamData sOamData_InterfaceText =
{
    .y = 160,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 1,
    .x = 0,
    .matrixNum = 0,
    .size = 3,
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};
static const struct OamData sOamData_RotatingPokeBall =
{
    .y = 160,
    .affineMode = 0,
    .objMode = 2,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 2,
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};
static const struct OamData sOamData_SeenOwnText =
{
    .y = 160,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 1,
    .x = 0,
    .matrixNum = 0,
    .size = 3,
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};
static const struct OamData sOamData_Dex8x16 =
{
    .y = 160,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 2,
    .x = 0,
    .matrixNum = 0,
    .size = 0,
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};
static const union AnimCmd sSpriteAnim_ScrollBar[] =
{
    ANIMCMD_FRAME(3, 30),
    ANIMCMD_END,
};
static const union AnimCmd sSpriteAnim_ScrollArrow[] =
{
    ANIMCMD_FRAME(1, 30),
    ANIMCMD_END,
};
static const union AnimCmd sSpriteAnim_RotatingPokeBall[] =
{
    ANIMCMD_FRAME(16, 30),
    ANIMCMD_END,
};
static const union AnimCmd sSpriteAnim_StartButton[] =
{
    ANIMCMD_FRAME(32, 30),
    ANIMCMD_END,
};
static const union AnimCmd sSpriteAnim_SearchText[] =
{
    ANIMCMD_FRAME(64, 30),
    ANIMCMD_END,
};
static const union AnimCmd sSpriteAnim_SelectButton[] =
{
    ANIMCMD_FRAME(96, 30),
    ANIMCMD_END,
};
static const union AnimCmd sSpriteAnim_MenuText[] =
{
    ANIMCMD_FRAME(128, 30),
    ANIMCMD_END,
};
static const union AnimCmd sSpriteAnim_SeenText[] =
{
    ANIMCMD_FRAME(160, 30),
    ANIMCMD_END,
};
static const union AnimCmd sSpriteAnim_OwnText[] =
{
    ANIMCMD_FRAME(192, 30),
    ANIMCMD_END,
};
static const union AnimCmd gSpriteAnim_83A047C[] =
{
    ANIMCMD_FRAME(224, 30),
    ANIMCMD_END,
};
static const union AnimCmd gSpriteAnim_83A0484[] =
{
    ANIMCMD_FRAME(226, 30),
    ANIMCMD_END,
};
static const union AnimCmd gSpriteAnim_83A048C[] =
{
    ANIMCMD_FRAME(228, 30),
    ANIMCMD_END,
};
static const union AnimCmd gSpriteAnim_83A0494[] =
{
    ANIMCMD_FRAME(230, 30),
    ANIMCMD_END,
};
static const union AnimCmd gSpriteAnim_83A049C[] =
{
    ANIMCMD_FRAME(232, 30),
    ANIMCMD_END,
};
static const union AnimCmd gSpriteAnim_83A04A4[] =
{
    ANIMCMD_FRAME(234, 30),
    ANIMCMD_END,
};
static const union AnimCmd gSpriteAnim_83A04AC[] =
{
    ANIMCMD_FRAME(236, 30),
    ANIMCMD_END,
};
static const union AnimCmd gSpriteAnim_83A04B4[] =
{
    ANIMCMD_FRAME(238, 30),
    ANIMCMD_END,
};
static const union AnimCmd gSpriteAnim_83A04BC[] =
{
    ANIMCMD_FRAME(240, 30),
    ANIMCMD_END,
};
static const union AnimCmd gSpriteAnim_83A04C4[] =
{
    ANIMCMD_FRAME(242, 30),
    ANIMCMD_END,
};
static const union AnimCmd sSpriteAnim_DexListStartMenuCursor[] =
{
    ANIMCMD_FRAME(4, 30),
    ANIMCMD_END,
};
static const union AnimCmd *const sSpriteAnimTable_ScrollBar[] =
{
    sSpriteAnim_ScrollBar,
};
static const union AnimCmd *const sSpriteAnimTable_ScrollArrow[] =
{
    sSpriteAnim_ScrollArrow,
};
static const union AnimCmd *const sSpriteAnimTable_RotatingPokeBall[] =
{
    sSpriteAnim_RotatingPokeBall,
};
static const union AnimCmd *const sSpriteAnimTable_InterfaceText[] =
{
    sSpriteAnim_StartButton,
    sSpriteAnim_SearchText,
    sSpriteAnim_SelectButton,
    sSpriteAnim_MenuText,
};
static const union AnimCmd *const sSpriteAnimTable_SeenOwnText[] =
{
    sSpriteAnim_SeenText,
    sSpriteAnim_OwnText,
};
static const union AnimCmd *const gSpriteAnimTable_83A04F8[] =
{
    gSpriteAnim_83A047C,
    gSpriteAnim_83A0484,
    gSpriteAnim_83A048C,
    gSpriteAnim_83A0494,
    gSpriteAnim_83A049C,
    gSpriteAnim_83A04A4,
    gSpriteAnim_83A04AC,
    gSpriteAnim_83A04B4,
    gSpriteAnim_83A04BC,
    gSpriteAnim_83A04C4,
};
static const union AnimCmd *const sSpriteAnimTable_DexListStartMenuCursor[] =
{
    sSpriteAnim_DexListStartMenuCursor,
};
static void SpriteCB_Scrollbar(struct Sprite *);
static const struct SpriteTemplate sScrollBarSpriteTemplate =
{
    .tileTag = 4096,
    .paletteTag = 4096,
    .oam = &sOamData_ScrollBar,
    .anims = sSpriteAnimTable_ScrollBar,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_Scrollbar,
};
static void SpriteCB_ScrollArrow(struct Sprite *);
static const struct SpriteTemplate sScrollArrowSpriteTemplate =
{
    .tileTag = 4096,
    .paletteTag = 4096,
    .oam = &sOamData_ScrollArrow,
    .anims = sSpriteAnimTable_ScrollArrow,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_ScrollArrow,
};
static void SpriteCB_DexListInterfaceText(struct Sprite *);
static const struct SpriteTemplate sInterfaceTextSpriteTemplate =
{
    .tileTag = 4096,
    .paletteTag = 4096,
    .oam = &sOamData_InterfaceText,
    .anims = sSpriteAnimTable_InterfaceText,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_DexListInterfaceText,
};
static void SpriteCB_RotatingPokeBall(struct Sprite *);
static const struct SpriteTemplate sRotatingPokeBallSpriteTemplate =
{
    .tileTag = 4096,
    .paletteTag = 4096,
    .oam = &sOamData_RotatingPokeBall,
    .anims = sSpriteAnimTable_RotatingPokeBall,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_RotatingPokeBall,
};
static void SpriteCB_SeenOwnInfo(struct Sprite *);
static const struct SpriteTemplate sSeenOwnTextSpriteTemplate =
{
    .tileTag = 4096,
    .paletteTag = 4096,
    .oam = &sOamData_SeenOwnText,
    .anims = sSpriteAnimTable_SeenOwnText,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_SeenOwnInfo,
};
static const struct SpriteTemplate gSpriteTemplate_83A059C =
{
    .tileTag = 4096,
    .paletteTag = 4096,
    .oam = &sOamData_Dex8x16,
    .anims = gSpriteAnimTable_83A04F8,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_SeenOwnInfo,
};
static void SpriteCB_DexListStartMenuCursor(struct Sprite *);
static const struct SpriteTemplate sDexListStartMenuCursorSpriteTemplate =
{
    .tileTag = 4096,
    .paletteTag = 4096,
    .oam = &sOamData_Dex8x16,
    .anims = sSpriteAnimTable_DexListStartMenuCursor,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_DexListStartMenuCursor,
};
static const struct CompressedSpriteSheet sInterfaceSpriteSheet[] =
{
    {gPokedexInterface_Gfx, 0x1F00, 0x1000},
    {NULL, 0, 0},
};
static const struct SpritePalette sInterfaceSpritePalette[] =
{
    {gPokedexBgHoenn_Pal, 0x1000},
    {NULL, 0},
};
static const u8 sScrollMonIncrements[] = {2, 4, 8, 16, 32};
static const u8 sScrollTimers[] = {16, 8, 4, 2, 1};
const u8 gEmptySpacce_83A05F6[] = {0, 0};  // Padding, maybe?
static const u8 sExpandedPlaceholder_PokedexDescription[] = _("");
#if ENGLISH
#include "data/pokemon/pokedex_text.h"
#include "data/pokemon/pokedex_entries.h"
#elif GERMAN
#include "data/pokemon/pokedex_text_de.h"
#include "data/pokemon/pokedex_entries_de.h"
#endif
static const u16 sSizeScreenSilhouette_Pal[16] = {0};
#include "data/pokemon_graphics/footprint_table.h"
static const u8 sText_ClearTo0[] = _("{CLEAR_TO 0}");
const u8 gUnknown_083B555C[] = INCBIN_U8("graphics/unknown/unknown_3B555C.bin");
static const struct OamData sOamData_MonOrTrainerPic =
{
    .y = 0,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 3,
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static void *const sMonOrTrainerPicDecompressDestPtrs[] =
{
    ePokedexGraphicsBuffer + 0x0000,
    ePokedexGraphicsBuffer + 0x4000,
    ePokedexGraphicsBuffer + 0x8000,
    ePokedexGraphicsBuffer + 0xC000,
};

static const struct SpriteFrameImage sSpriteFrameImages_MonOrTrainerPic_0[] =
{
    {ePokedexGraphicsBuffer + 0x0000, 0x800},
    {ePokedexGraphicsBuffer + 0x0800, 0x800},
    {ePokedexGraphicsBuffer + 0x1000, 0x800},
    {ePokedexGraphicsBuffer + 0x1800, 0x800},
    {ePokedexGraphicsBuffer + 0x2000, 0x800},
    {ePokedexGraphicsBuffer + 0x2800, 0x800},
    {ePokedexGraphicsBuffer + 0x3000, 0x800},
    {ePokedexGraphicsBuffer + 0x3800, 0x800},
    {ePokedexGraphicsBuffer + 0x4000, 0x800},
    {ePokedexGraphicsBuffer + 0x4800, 0x800},
    {ePokedexGraphicsBuffer + 0x5000, 0x800},
    {ePokedexGraphicsBuffer + 0x5800, 0x800},
    {ePokedexGraphicsBuffer + 0x6000, 0x800},
    {ePokedexGraphicsBuffer + 0x6800, 0x800},
    {ePokedexGraphicsBuffer + 0x7000, 0x800},
    {ePokedexGraphicsBuffer + 0x7800, 0x800},
};

static const struct SpriteFrameImage sSpriteFrameImages_MonOrTrainerPic_1[] =
{
    {ePokedexGraphicsBuffer + 0x4000, 0x800},
    {ePokedexGraphicsBuffer + 0x4800, 0x800},
    {ePokedexGraphicsBuffer + 0x5000, 0x800},
    {ePokedexGraphicsBuffer + 0x5800, 0x800},
    {ePokedexGraphicsBuffer + 0x6000, 0x800},
    {ePokedexGraphicsBuffer + 0x6800, 0x800},
    {ePokedexGraphicsBuffer + 0x7000, 0x800},
    {ePokedexGraphicsBuffer + 0x7800, 0x800},
    {ePokedexGraphicsBuffer + 0x8000, 0x800},
    {ePokedexGraphicsBuffer + 0x8800, 0x800},
    {ePokedexGraphicsBuffer + 0x9000, 0x800},
    {ePokedexGraphicsBuffer + 0x9800, 0x800},
    {ePokedexGraphicsBuffer + 0xA000, 0x800},
    {ePokedexGraphicsBuffer + 0xA800, 0x800},
    {ePokedexGraphicsBuffer + 0xB000, 0x800},
    {ePokedexGraphicsBuffer + 0xB800, 0x800},
};

static const struct SpriteFrameImage sSpriteFrameImages_MonOrTrainerPic_2[] =
{
    {ePokedexGraphicsBuffer + 0x8000, 0x800},
    {ePokedexGraphicsBuffer + 0x8800, 0x800},
    {ePokedexGraphicsBuffer + 0x9000, 0x800},
    {ePokedexGraphicsBuffer + 0x9800, 0x800},
    {ePokedexGraphicsBuffer + 0xA000, 0x800},
    {ePokedexGraphicsBuffer + 0xA800, 0x800},
    {ePokedexGraphicsBuffer + 0xB000, 0x800},
    {ePokedexGraphicsBuffer + 0xB800, 0x800},
    {ePokedexGraphicsBuffer + 0xC000, 0x800},
    {ePokedexGraphicsBuffer + 0xC800, 0x800},
    {ePokedexGraphicsBuffer + 0xD000, 0x800},
    {ePokedexGraphicsBuffer + 0xD800, 0x800},
    {ePokedexGraphicsBuffer + 0xE000, 0x800},
    {ePokedexGraphicsBuffer + 0xE800, 0x800},
    {ePokedexGraphicsBuffer + 0xF000, 0x800},
    {ePokedexGraphicsBuffer + 0xF800, 0x800},
};

static const struct SpriteFrameImage sSpriteFrameImages_MonOrTrainerPic_3[] =
{
    {ePokedexGraphicsBuffer + 0xC000, 0x800},
    {ePokedexGraphicsBuffer + 0xC800, 0x800},
    {ePokedexGraphicsBuffer + 0xD000, 0x800},
    {ePokedexGraphicsBuffer + 0xD800, 0x800},
    {ePokedexGraphicsBuffer + 0xE000, 0x800},
    {ePokedexGraphicsBuffer + 0xE800, 0x800},
    {ePokedexGraphicsBuffer + 0xF000, 0x800},
    {ePokedexGraphicsBuffer + 0xF800, 0x800},
    // UB: may overlap ePokedexView1-4
    {ePokedexGraphicsBuffer + 0x10000, 0x800},
    {ePokedexGraphicsBuffer + 0x10800, 0x800},
    {ePokedexGraphicsBuffer + 0x11000, 0x800},
    {ePokedexGraphicsBuffer + 0x11800, 0x800},
    {ePokedexGraphicsBuffer + 0x12000, 0x800},
    {ePokedexGraphicsBuffer + 0x12800, 0x800},
    {ePokedexGraphicsBuffer + 0x13000, 0x800},
    {ePokedexGraphicsBuffer + 0x13800, 0x800},
};

static const struct SpriteFrameImage *const sSpriteFrameImages_MonOrTrainerPic[] =
{
    sSpriteFrameImages_MonOrTrainerPic_0,
    sSpriteFrameImages_MonOrTrainerPic_1,
    sSpriteFrameImages_MonOrTrainerPic_2,
    sSpriteFrameImages_MonOrTrainerPic_3,
};

static void _SpriteCB_Dummy(struct Sprite *);

static const struct SpriteTemplate sMonOrTrainerPicSpriteTemplate =
{
    .tileTag = 0xFFFF,
    .paletteTag = 0,
    .oam = &sOamData_MonOrTrainerPic,
    .anims = NULL,
    .images = sSpriteFrameImages_MonOrTrainerPic_0,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = _SpriteCB_Dummy,
};

// First character in range followed by number of characters in range for upper and lowercase
static const u8 sLetterSearchRanges[][4] =
{
    {0,      0, 0,      0},
    {CHAR_A, 3, CHAR_a, 3},
    {CHAR_D, 3, CHAR_d, 3},
    {CHAR_G, 3, CHAR_g, 3},
    {CHAR_J, 3, CHAR_j, 3},
    {CHAR_M, 3, CHAR_m, 3},
    {CHAR_P, 3, CHAR_p, 3},
    {CHAR_S, 3, CHAR_s, 3},
    {CHAR_V, 3, CHAR_v, 3},
    {CHAR_Y, 2, CHAR_y, 2},
};
static const struct SearchMenuTopBarItem sSearchMenuTopBarItems[] =
{
    {gText_SearchForPkmnBasedOnParameters, 0,  0, 5},
    {gText_SwitchPokedexListings,     6,  0, 5},
    {gText_ReturnToPokedex,  12,  0, 5},
};
static const struct SearchMenuItem sSearchMenuItems[] =
{
    {gText_ListByFirstLetter,           0,  2,  5,  5,  2, 12},
    {gText_ListByBodyColor,         0,  4,  5,  5,  4, 12},
    {gText_ListByType,          0,  6,  5,  5,  6,  6},
    {gText_ListByType,          0,  6,  5, 11,  6,  6},
    {gText_SelectPokedexListingMode,       0,  8,  5,  5,  8, 12},
    {gText_SelectPokedexMode,       0, 10,  5,  5, 10, 12},
    {gText_ExecuteSearchSwitch, 0, 12,  5,  0,  0,  0},
};
static const u8 sSearchMovementMap_SearchNatDex[][4] =
{
    {0xFF, 0xFF, 0xFF,    1},
    {0xFF, 0xFF,    0,    2},
    {0xFF,    3,    1,    4},
    {   2, 0xFF,    1,    4},
    {0xFF, 0xFF,    2,    5},
    {0xFF, 0xFF,    4,    6},
    {0xFF, 0xFF,    5, 0xFF},
};
static const u8 sSearchMovementMap_ShiftNatDex[][4] =
{
    {0xFF, 0xFF, 0xFF, 0xFF},
    {0xFF, 0xFF, 0xFF, 0xFF},
    {0xFF, 0xFF, 0xFF, 0xFF},
    {0xFF, 0xFF, 0xFF, 0xFF},
    {0xFF, 0xFF, 0xFF,    5},
    {0xFF, 0xFF,    4,    6},
    {0xFF, 0xFF,    5, 0xFF},
};
static const u8 sSearchMovementMap_SearchHoennDex[][4] =
{
    {0xFF, 0xFF, 0xFF,    1},
    {0xFF, 0xFF,    0,    2},
    {0xFF,    3,    1,    4},
    {   2, 0xFF,    1,    4},
    {0xFF, 0xFF,    2,    6},
    {0xFF, 0xFF, 0xFF, 0xFF},
    {0xFF, 0xFF,    4, 0xFF},
};
static const u8 sSearchMovementMap_ShiftHoennDex[][4] =
{
    {0xFF, 0xFF, 0xFF, 0xFF},
    {0xFF, 0xFF, 0xFF, 0xFF},
    {0xFF, 0xFF, 0xFF, 0xFF},
    {0xFF, 0xFF, 0xFF, 0xFF},
    {0xFF, 0xFF, 0xFF,    6},
    {0xFF, 0xFF, 0xFF, 0xFF},
    {0xFF, 0xFF,    4, 0xFF},
};
static const struct SearchOptionText sDexModeOptions[] =
{
    {gText_DexHoennDescription,    gText_DexHoennTitle},
    {gText_DexNatDescription, gText_DexNatTitle},
    {NULL, NULL},
};
static const struct SearchOptionText sDexOrderOptions[] =
{
    {gText_DexSortNumericalDescription,          gText_DexSortNumericalTitle},
    {gText_DexSortAtoZDescription,            gText_DexSortAtoZTitle},
    {gText_DexSortHeaviestDescription, gText_DexSortHeaviestTitle},
    {gText_DexSortLightestDescription, gText_DexSortLightestTitle},
    {gText_DexSortTallestDescription,  gText_DexSortTallestTitle},
    {gText_DexSortSmallestDescription,  gText_DexSortSmallestTitle},
    {NULL, NULL},
};
static const struct SearchOptionText sDexSearchNameOptions[] =
{
    {gText_DexEmptyString, gText_DexSearchDontSpecify},
    {gText_DexEmptyString, gText_DexSearchAlphaABC},
    {gText_DexEmptyString, gText_DexSearchAlphaDEF},
    {gText_DexEmptyString, gText_DexSearchAlphaGHI},
    {gText_DexEmptyString, gText_DexSearchAlphaJKL},
    {gText_DexEmptyString, gText_DexSearchAlphaMNO},
    {gText_DexEmptyString, gText_DexSearchAlphaPQR},
    {gText_DexEmptyString, gText_DexSearchAlphaSTU},
    {gText_DexEmptyString, gText_DexSearchAlphaVWX},
    {gText_DexEmptyString, gText_DexSearchAlphaYZ},
    {NULL, NULL},
};
static const struct SearchOptionText sDexSearchColorOptions[] =
{
    {gText_DexEmptyString, gText_DexSearchDontSpecify},
    {gText_DexEmptyString, gText_DexSearchColorRed},
    {gText_DexEmptyString, gText_DexSearchColorBlue},
    {gText_DexEmptyString, gText_DexSearchColorYellow},
    {gText_DexEmptyString, gText_DexSearchColorGreen},
    {gText_DexEmptyString, gText_DexSearchColorBlack},
    {gText_DexEmptyString, gText_DexSearchColorBrown},
    {gText_DexEmptyString, gText_DexSearchColorPurple},
    {gText_DexEmptyString, gText_DexSearchColorGray},
    {gText_DexEmptyString, gText_DexSearchColorWhite},
    {gText_DexEmptyString, gText_DexSearchColorPink},
    {NULL, NULL},
};
static const struct SearchOptionText sDexSearchTypeOptions[] =
{
    {gText_DexEmptyString, gText_DexSearchTypeNone},
    {gText_DexEmptyString, gTypeNames[TYPE_NORMAL]},
    {gText_DexEmptyString, gTypeNames[TYPE_FIGHTING]},
    {gText_DexEmptyString, gTypeNames[TYPE_FLYING]},
    {gText_DexEmptyString, gTypeNames[TYPE_POISON]},
    {gText_DexEmptyString, gTypeNames[TYPE_GROUND]},
    {gText_DexEmptyString, gTypeNames[TYPE_ROCK]},
    {gText_DexEmptyString, gTypeNames[TYPE_BUG]},
    {gText_DexEmptyString, gTypeNames[TYPE_GHOST]},
    {gText_DexEmptyString, gTypeNames[TYPE_STEEL]},
    {gText_DexEmptyString, gTypeNames[TYPE_FIRE]},
    {gText_DexEmptyString, gTypeNames[TYPE_WATER]},
    {gText_DexEmptyString, gTypeNames[TYPE_GRASS]},
    {gText_DexEmptyString, gTypeNames[TYPE_ELECTRIC]},
    {gText_DexEmptyString, gTypeNames[TYPE_PSYCHIC]},
    {gText_DexEmptyString, gTypeNames[TYPE_ICE]},
    {gText_DexEmptyString, gTypeNames[TYPE_DRAGON]},
    {gText_DexEmptyString, gTypeNames[TYPE_DARK]},
    {NULL, NULL},
};
static const u8 sPokedexModes[] = {0, 1};
static const u8 sOrderOptions[] = {0, 1, 2, 3, 4, 5};
static const u8 sDexSearchTypeIds[] = {0xFF, 0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 11, 12, 13, 14, 15, 16, 17};
static const struct SearchOption sSearchOptions[] =
{
    { sDexSearchNameOptions,  6,  7, 10},
    { sDexSearchColorOptions,  8,  9, 11},
    { sDexSearchTypeOptions, 10, 11, 18},
    { sDexSearchTypeOptions, 12, 13, 18},
    { sDexOrderOptions,  4,  5,  6},
    { sDexModeOptions,  2,  3,  2},
};
static const u8 gUnknown_083B5AAC[] = _("{STR_VAR_1}{CLEAR_TO 43}");
static const u8 gUnknown_083B5AB2[] = _("{STR_VAR_1}{CLEAR_TO 96}");

static void CB2_Pokedex(void);
static void Task_OpenPokedexMainPage(u8 taskId);
static void Task_HandlePokedexInput(u8 taskId);
static void Task_WaitForScroll(u8 taskId);
static void Task_HandlePokedexStartMenuInput(u8 taskId);
static void Task_OpenInfoScreenAfterMonMovement(u8 taskId);
static void Task_WaitForExitInfoScreen(u8 taskId);
static void Task_WaitForExitSearch(u8 taskId);
static void Task_ClosePokedex(u8 taskId);
static void Task_OpenSearchResults(u8 taskId);
static void Task_HandleSearchResultsInput(u8 taskId);
static void Task_WaitForSearchResultsScroll(u8 taskId);
static void Task_HandleSearchResultsStartMenuInput(u8 taskId);
static void Task_OpenSearchResultsInfoScreenAfterMonMovement(u8 taskId);
static void Task_WaitForExitSearchResultsInfoScreen(u8 taskId);
static void Task_ReturnToPokedexFromSearchResults(u8 taskId);
static void Task_ClosePokedexFromSearchResultsStartMenu(u8 taskId);
static bool8 LoadPokedexListPage(u8 page);
static void LoadPokedexBgPalette(void);
static void CreatePokedexList(u8 dexMode, u8 order);
static void CreateMonDexNum(u16 entryNum, u8 left, u8 top, u16 screenBase);
static void CreateCaughtBall(bool16 owned, u8 x, u8 y, u16 screenBase);
static u8 CreateMonName(u16 num, u8 left, u8 top);
static void ClearMonListEntry(u8 x, u8 y, u16 screenBase);
static void CreateMonSpritesAtPos(u16 selectedMon, u16 b);
static bool8 UpdateDexListScroll(u8 direction, u8 monMoveIncrement, u8 scrollTimerMax);
static u16 TryDoPokedexScroll(u16 selectedMon, u16 b);
static void UpdateSelectedMonSpriteId(void);
static bool8 TryDoInfoScreenScroll(void);
static u8 ClearMonSprites(void);
static u16 GetPokemonSpriteToDisplay(u16 species);
static u32 CreatePokedexMonSprite(u16 num, s16 x, s16 y);
static void CreateInterfaceSprites(u8 page);
static void SpriteCB_MoveMonForInfoScreen(struct Sprite *sprite);
static void SpriteCB_PokedexListMonSprite(struct Sprite *sprite);
static u8 LoadInfoScreen(struct PokedexListItem *, u8);
static bool8 IsInfoScreenScrolling(u8 taskId);
static u8 StartInfoScreenScroll(struct PokedexListItem *, u8);
static void Task_LoadInfoScreen(u8 taskId);
static void Task_HandleInfoScreenInput(u8 taskId);
static void Task_LoadInfoScreenWaitForFade(u8 taskId);
static void Task_ExitInfoScreen(u8 taskId);
static void Task_LoadAreaScreen(u8 taskId);
static void Task_WaitForAreaScreenInput(u8 taskId);
static void Task_SwitchScreensFromAreaScreen(u8 taskId);
static void Task_LoadCryScreen(u8 taskId);
static void Task_HandleCryScreenInput(u8 taskId);
static void Task_SwitchScreensFromCryScreen(u8 taskId);
static void LoadPlayArrowPalette(u8 cryPlaying);
static void Task_LoadSizeScreen(u8 taskId);
static void Task_HandleSizeScreenInput(u8 taskId);
static void Task_SwitchScreensFromSizeScreen(u8 taskId);
static void LoadScreenSelectBarMain(u16 screenBase);
static void LoadScreenSelectBarSubmenu(u16 screenBase);
static void HighlightScreenSelectBarItem(u8 selectedScreen, u16 b);
static void HighlightSubmenuScreenSelectBarItem(u8 a, u16 b);
static void Task_DisplayCaughtMonDexPage(u8);
static void Task_HandleCaughtMonPageInput(u8);
static void Task_ExitCaughtMonPage(u8);
static void SpriteCB_SlideCaughtMonToCenter(struct Sprite *);
static void sub_8090C68(void);
static void ResetOtherVideoRegisters(u16);
static void PrintEntryScreenDexNum(u16 order, u8, u8);
static u8 PrintEntryScreenSpeciesName(u16 num, u8, u8);
static u8 PrintCryScreenSpeciesName(u16 num, u8, u8, u8);
static void UnusedPrintMonName(const u8 *name, u8, u8);
static void PrintMonHeight(u16 height, u8 i, u8 i1);
static void PrintMonWeight(u16 weight, u8 i, u8 i1);
static void RS_DrawFootprint(u16 offset, u16 tileNum);
static u16 GetNextPosition(u8 direction, u16 position, u16 min, u16 max);
static void Task_LoadSearchMenu(u8 taskId);
static void Task_SwitchToSearchMenuTopBar(u8 taskId);
static void Task_HandleSearchTopBarInput(u8 taskId);
static void Task_SwitchToSearchMenu(u8 taskId);
static void Task_HandleSearchMenuInput(u8 taskId);
static void Task_StartPokedexSearch(u8 taskId);
static void Task_WaitAndCompleteSearch(u8 taskId);
static void Task_SearchCompleteWaitForInput(u8 taskId);
static void Task_SelectSearchMenuItem(u8 taskId);
static void Task_HandleSearchParameterInput(u8 taskId);
static void Task_ExitSearch(u8 taskId);
static void Task_ExitSearchWaitForFade(u8 taskId);
static void HighlightSelectedSearchTopBarItem(u8 topBarItem);
static void HighlightSelectedSearchMenuItem(u8 topBarItem, u8 menuItem);
static void PrintSelectedSearchParameters(u8 taskId);
static void DrawOrEraseSearchParameterBox(bool8 erase);
static void PrintSearchParameterText(u8 taskId);
static u8 GetSearchModeSelection(u8 taskId, u8 option);
static void SetDefaultSearchModeAndOrder(u8 taskId);
static void CreateSearchParameterScrollArrows(u8 taskId);


void ResetPokedex(void)
{
    u16 i;

    sLastSelectedPokemon = 0;
    sPokeBallRotation = 0x40;
    gUnusedPokedexU8 = 0;
    gSaveBlock2.pokedex.mode = 0;
    gSaveBlock2.pokedex.order = 0;
    gSaveBlock2.pokedex.nationalMagic = 0;
    gSaveBlock2.pokedex.unknown2 = 0;
    gSaveBlock2.pokedex.unownPersonality = 0;
    gSaveBlock2.pokedex.spindaPersonality = 0;
    gSaveBlock2.pokedex.unknown3 = 0;
    DisableNationalPokedex();
    for (i = 0; i < DEX_FLAGS_NO; i++)
    {
        gSaveBlock2.pokedex.owned[i] = 0;
        gSaveBlock2.pokedex.seen[i] = 0;
        gSaveBlock1.dexSeen2[i] = 0;
        gSaveBlock1.dexSeen3[i] = 0;
    }
}

void ResetPokedexScrollPositions(void)
{
    sLastSelectedPokemon = 0;
    sPokeBallRotation = 0x40;
}

static void VBlankCB_Pokedex(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void ResetPokedexView(struct PokedexView *pokedexView)
{
    u16 i;

    for (i = 0; i < NATIONAL_DEX_COUNT; i++)
    {
        pokedexView->pokedexList[i].dexNum |= 0xFFFF;
        pokedexView->pokedexList[i].seen = 0;
        pokedexView->pokedexList[i].owned = 0;
    }
    pokedexView->pokedexList[NATIONAL_DEX_COUNT].dexNum = 0;
    pokedexView->pokedexList[NATIONAL_DEX_COUNT].seen = 0;
    pokedexView->pokedexList[NATIONAL_DEX_COUNT].owned = 0;
    pokedexView->pokemonListCount = 0;
    pokedexView->selectedPokemon = 0;
    pokedexView->selectedPokemonBackup = 0;
    pokedexView->dexMode = 0;
    pokedexView->dexModeBackup = 0;
    pokedexView->dexOrder = 0;
    pokedexView->dexOrderBackup = 0;
    pokedexView->seenCount = 0;
    pokedexView->ownCount = 0;
    for (i = 0; i < 4; i++)
        pokedexView->monSpriteIds[i] |= 0xFFFF;
    pokedexView->pokeBallRotationStep = 0;
    pokedexView->pokeBallRotationBackup = 0;
    pokedexView->pokeBallRotation = 0;
    pokedexView->initialVOffset = 0;
    pokedexView->scrollTimer = 0;
    pokedexView->scrollDirection = 0;
    pokedexView->listVOffset = 0;
    pokedexView->listMovingVOffset = 0;
    pokedexView->scrollMonIncrement = 0;
    pokedexView->maxScrollTimer = 0;
    pokedexView->scrollSpeed = 0;
    for (i = 0; i < 4; i++)
        pokedexView->unkArr1[i] = 0;
    pokedexView->currentPage = 0;
    pokedexView->currentPageBackup = 0;
    pokedexView->isSearchResults = FALSE;
    pokedexView->selectedScreen = 0;
    pokedexView->screenSwitchState = 0;
    pokedexView->menuIsOpen = 0;
    pokedexView->menuCursorPos = 0;
    pokedexView->menuY = 0;
    for (i = 0; i < 8; i++)
        pokedexView->unkArr2[i] = 0;
    for (i = 0; i < 8; i++)
        pokedexView->unkArr3[i] = 0;
}

void CB2_OpenPokedex(void)
{
    switch (gMain.state)
    {
    case 0:
    default:
        SetVBlankCallback(NULL);
        ResetOtherVideoRegisters(0);
        DmaFill16Large(3, 0, (void *)(VRAM + 0x0), VRAM_SIZE, 0x1000);
        DmaClear32(3, OAM, OAM_SIZE);
        DmaClear16(3, PLTT, PLTT_SIZE);
        gMain.state = 1;
        break;
    case 1:
        ScanlineEffect_Stop();
        ResetTasks();
        ResetSpriteData();
        ResetPaletteFade();
        FreeAllSpritePalettes();
        gReservedSpritePaletteCount = 8;
        Text_LoadWindowTemplate(&gWindowTemplate_81E7048);
        InitMenuWindow(&gWindowTemplate_81E7048);
        gMain.state++;
        break;
    case 2:
        switch (Random() & 3)
        {
        case 0:
        default:
            sPokedexView = ePokedexView1;
            break;
        case 1:
            sPokedexView = ePokedexView2;
            break;
        case 2:
            sPokedexView = ePokedexView3;
            break;
        case 3:
            sPokedexView = ePokedexView4;
            break;
        }
        ResetPokedexView(sPokedexView);
        CreateTask(Task_OpenPokedexMainPage, 0);
        sPokedexView->dexMode = gSaveBlock2.pokedex.mode;
        if (!IsNationalPokedexEnabled())
            sPokedexView->dexMode = DEX_MODE_HOENN;
        sPokedexView->dexOrder = gSaveBlock2.pokedex.order;
        sPokedexView->selectedPokemon = sLastSelectedPokemon;
        sPokedexView->pokeBallRotation = sPokeBallRotation;
        sPokedexView->selectedScreen = PAGE_SCREEN;
        sPokedexView->descriptionPageNum = 0;
        if (!IsNationalPokedexEnabled())
        {
            sPokedexView->seenCount = GetHoennPokedexCount(0);
            sPokedexView->ownCount = GetHoennPokedexCount(1);
        }
        else
        {
            sPokedexView->seenCount = GetNationalPokedexCount(0);
            sPokedexView->ownCount = GetNationalPokedexCount(1);
        }
        sPokedexView->initialVOffset = 8;
        gMain.state++;
        break;
    case 3:
        {
            u16 savedIme;

            savedIme = REG_IME;
            REG_IME = 0;
            REG_IE |= INTR_FLAG_VBLANK;
            REG_IME = savedIme;
            REG_DISPSTAT |= DISPSTAT_VBLANK_INTR;
            SetVBlankCallback(VBlankCB_Pokedex);
            SetMainCallback2(CB2_Pokedex);
            CreatePokedexList(sPokedexView->dexMode, sPokedexView->dexOrder);
            m4aMPlayVolumeControl(&gMPlayInfo_BGM, 0xFFFF, 0x80);
        }
        break;
    }
}

UNUSED
u8 unref_sub_808C540(void (*func)(u8))
{
    u16 savedIme;
    u8 taskId;

    SetVBlankCallback(NULL);
    ResetOtherVideoRegisters(0x200);
    ScanlineEffect_Stop();
    ResetTasks();
    ResetPaletteFade();
    savedIme = REG_IME;
    REG_IME = 0;
    REG_IE |= INTR_FLAG_VBLANK;
    REG_IME = savedIme;
    REG_DISPSTAT |= DISPSTAT_VBLANK_INTR;
    SetVBlankCallback(VBlankCB_Pokedex);
    SetMainCallback2(CB2_Pokedex);
    if (!sPokedexView->currentPageBackup)
        taskId = CreateTask(Task_WaitForExitInfoScreen, 0);
    else
        taskId = CreateTask(Task_WaitForExitSearchResultsInfoScreen, 0);
    gTasks[taskId].data[0] = CreateTask(func, 0);
    return gTasks[taskId].data[0];
}

static void CB2_Pokedex(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void Task_OpenPokedexMainPage(u8 taskId)
{
    sPokedexView->isSearchResults = FALSE;
    if (LoadPokedexListPage(0))
        gTasks[taskId].func = Task_HandlePokedexInput;
}

//Hide menu and process input on main screen
static void Task_HandlePokedexInput(u8 taskId)
{
    REG_BG0VOFS = sPokedexView->menuY;

    //If menu is open, slide it down, off screen
    if (sPokedexView->menuY)
        sPokedexView->menuY -= 8;
    else
    {
        if (JOY_NEW(A_BUTTON) && sPokedexView->pokedexList[sPokedexView->selectedPokemon].seen)
        {
            UpdateSelectedMonSpriteId();
            BeginNormalPaletteFade(
              ~(1 << (gSprites[sPokedexView->selectedMonSpriteId].oam.paletteNum + 16)),
              0, 0, 16, RGB(0, 0, 0));
            gSprites[sPokedexView->selectedMonSpriteId].callback = SpriteCB_MoveMonForInfoScreen;
            gTasks[taskId].func = Task_OpenInfoScreenAfterMonMovement;
            PlaySE(SE_PIN);
        }
        else if (JOY_NEW(START_BUTTON))
        {
            //Open menu
            sPokedexView->menuY = 0;
            sPokedexView->menuIsOpen = 1;
            sPokedexView->menuCursorPos = 0;
            gTasks[taskId].func = Task_HandlePokedexStartMenuInput;
            PlaySE(SE_SELECT);
        }
        else if (JOY_NEW(SELECT_BUTTON))
        {
            PlaySE(SE_SELECT);
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            gTasks[taskId].data[0] = LoadSearchMenu();
            sPokedexView->screenSwitchState = 0;
            sPokedexView->pokeBallRotationBackup = sPokedexView->pokeBallRotation;
            sPokedexView->selectedPokemonBackup = sPokedexView->selectedPokemon;
            sPokedexView->dexModeBackup = sPokedexView->dexMode;
            sPokedexView->dexOrderBackup = sPokedexView->dexOrder;
            gTasks[taskId].func = Task_WaitForExitSearch;
            PlaySE(SE_PC_LOGIN);
        }
        else if (JOY_NEW(B_BUTTON))
        {
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            gTasks[taskId].func = Task_ClosePokedex;
            PlaySE(SE_PC_OFF);
        }
        else
        {
            //Handle D-pad
            sPokedexView->selectedPokemon = TryDoPokedexScroll(sPokedexView->selectedPokemon, 0xE);
            if (sPokedexView->scrollTimer)
                gTasks[taskId].func = Task_WaitForScroll;
        }
    }
}

static void Task_WaitForScroll(u8 taskId)
{
    if (UpdateDexListScroll(sPokedexView->scrollDirection,
            sPokedexView->scrollMonIncrement,
            sPokedexView->maxScrollTimer))
        gTasks[taskId].func = Task_HandlePokedexInput;
}

//Bring up menu and process menu input
static void Task_HandlePokedexStartMenuInput(u8 taskId)
{
    REG_BG0VOFS = sPokedexView->menuY;

    //If menu is not open, slide it up, on screen
    if (sPokedexView->menuY != 80)
        sPokedexView->menuY += 8;
    else
    {
        if (JOY_NEW(A_BUTTON))
        {
            switch (sPokedexView->menuCursorPos)
            {
            case 0: //BACK TO LIST
            default:
                gMain.newKeys |= START_BUTTON;  //Exit menu
                break;
            case 1: //LIST TOP
                sPokedexView->selectedPokemon = 0;
                sPokedexView->pokeBallRotation = 0x40;
                ClearMonSprites();
                CreateMonSpritesAtPos(sPokedexView->selectedPokemon, 0xE);
                gMain.newKeys |= START_BUTTON;  //Exit menu
                break;
            case 2: //LIST BOTTOM
                sPokedexView->selectedPokemon = sPokedexView->pokemonListCount - 1;
                sPokedexView->pokeBallRotation = sPokedexView->pokemonListCount * 16 + 0x30;
                ClearMonSprites();
                CreateMonSpritesAtPos(sPokedexView->selectedPokemon, 0xE);
                gMain.newKeys |= START_BUTTON;  //Exit menu
                break;
            case 3: //CLOSE POKEDEX
                BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
                gTasks[taskId].func = Task_ClosePokedex;
                PlaySE(SE_PC_OFF);
                break;
            }
        }

        //Exit menu when Start or B is pressed
        if (JOY_NEW(START_BUTTON | B_BUTTON))
        {
            sPokedexView->menuIsOpen = 0;
            gTasks[taskId].func = Task_HandlePokedexInput;
            PlaySE(SE_SELECT);
        }
        else if (JOY_REPEAT(DPAD_UP) && sPokedexView->menuCursorPos != 0)
        {
            sPokedexView->menuCursorPos--;
            PlaySE(SE_SELECT);
        }
        else if (JOY_REPEAT(DPAD_DOWN) && sPokedexView->menuCursorPos <= 2)
        {
            sPokedexView->menuCursorPos++;
            PlaySE(SE_SELECT);
        }
    }
}

static void Task_OpenInfoScreenAfterMonMovement(u8 taskId)
{
    if (gSprites[sPokedexView->selectedMonSpriteId].x == 48
     && gSprites[sPokedexView->selectedMonSpriteId].y == 56)
    {
        sPokedexView->currentPageBackup = sPokedexView->currentPage;
        gTasks[taskId].data[0] = LoadInfoScreen(&sPokedexView->pokedexList[sPokedexView->selectedPokemon], sPokedexView->selectedMonSpriteId);
        gTasks[taskId].func = Task_WaitForExitInfoScreen;
    }
}

static void Task_WaitForExitInfoScreen(u8 taskId)
{
    if (gTasks[gTasks[taskId].data[0]].isActive)
    {
        if (sPokedexView->currentPage == 1 && !IsInfoScreenScrolling(gTasks[taskId].data[0]) && TryDoInfoScreenScroll())
            StartInfoScreenScroll(&sPokedexView->pokedexList[sPokedexView->selectedPokemon], gTasks[taskId].data[0]);
    }
    else
    {
        sLastSelectedPokemon = sPokedexView->selectedPokemon;
        sPokeBallRotation = sPokedexView->pokeBallRotation;
        gTasks[taskId].func = Task_OpenPokedexMainPage;
    }
}

static void Task_WaitForExitSearch(u8 taskId)
{
    bool8 isActive = gTasks[gTasks[taskId].data[0]].isActive;

    if (!isActive)
    {
        if (sPokedexView->screenSwitchState != 0)
        {
            sPokedexView->selectedPokemon = isActive;
            sPokedexView->pokeBallRotation = 0x40;
            gTasks[taskId].func = Task_OpenSearchResults;
        }
        else
        {
            sPokedexView->pokeBallRotation = sPokedexView->pokeBallRotationBackup;
            sPokedexView->selectedPokemon = sPokedexView->selectedPokemonBackup;
            sPokedexView->dexMode = sPokedexView->dexModeBackup;
            if (!IsNationalPokedexEnabled())
                sPokedexView->dexMode = DEX_MODE_HOENN;
            sPokedexView->dexOrder = sPokedexView->dexOrderBackup;
            gTasks[taskId].func = Task_OpenPokedexMainPage;
        }
    }
}

static void Task_ClosePokedex(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        gSaveBlock2.pokedex.mode = sPokedexView->dexMode;
        if (!IsNationalPokedexEnabled())
            gSaveBlock2.pokedex.mode = 0;
        gSaveBlock2.pokedex.order = sPokedexView->dexOrder;
        DestroyTask(taskId);
        SetMainCallback2(CB2_ReturnToFieldWithOpenMenu);
        m4aMPlayVolumeControl(&gMPlayInfo_BGM, 0xFFFF, 0x100);
    }
}

static void Task_OpenSearchResults(u8 taskId)
{
    sPokedexView->isSearchResults = TRUE;
    if (LoadPokedexListPage(3))
        gTasks[taskId].func = Task_HandleSearchResultsInput;
}

static void Task_HandleSearchResultsInput(u8 taskId)
{
    REG_BG0VOFS = sPokedexView->menuY;

    if (sPokedexView->menuY)
    {
        sPokedexView->menuY -= 8;
    }
    else
    {
        if (JOY_NEW(A_BUTTON) && sPokedexView->pokedexList[sPokedexView->selectedPokemon].seen)
        {
            u32 excludedPalettes;

            UpdateSelectedMonSpriteId();
            excludedPalettes = (1 << (gSprites[sPokedexView->selectedMonSpriteId].oam.paletteNum + 16));
            gSprites[sPokedexView->selectedMonSpriteId].callback = SpriteCB_MoveMonForInfoScreen;
            BeginNormalPaletteFade(~excludedPalettes, 0, 0, 16, RGB(0, 0, 0));
            gTasks[taskId].func = Task_OpenSearchResultsInfoScreenAfterMonMovement;
            PlaySE(SE_PIN);
        }
        else if (JOY_NEW(START_BUTTON))
        {
            sPokedexView->menuY = 0;
            sPokedexView->menuIsOpen = 1;
            sPokedexView->menuCursorPos = 0;
            gTasks[taskId].func = Task_HandleSearchResultsStartMenuInput;
            PlaySE(SE_SELECT);
        }
        else if (JOY_NEW(SELECT_BUTTON))
        {
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            gTasks[taskId].data[0] = LoadSearchMenu();
            sPokedexView->screenSwitchState = 0;
            gTasks[taskId].func = Task_WaitForExitSearch;
            PlaySE(SE_PC_LOGIN);
        }
        else if (JOY_NEW(B_BUTTON))
        {
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            gTasks[taskId].func = Task_ReturnToPokedexFromSearchResults;
            PlaySE(SE_PC_OFF);
        }
        else
        {
            //Handle D-pad
            sPokedexView->selectedPokemon = TryDoPokedexScroll(sPokedexView->selectedPokemon, 0xE);
            if (sPokedexView->scrollTimer)
                gTasks[taskId].func = Task_WaitForSearchResultsScroll;
        }
    }
}

static void Task_WaitForSearchResultsScroll(u8 taskId)
{
    if (UpdateDexListScroll(sPokedexView->scrollDirection,
            sPokedexView->scrollMonIncrement,
            sPokedexView->maxScrollTimer))
        gTasks[taskId].func = Task_HandleSearchResultsInput;
}

static void Task_HandleSearchResultsStartMenuInput(u8 taskId)
{
    REG_BG0VOFS = sPokedexView->menuY;

    if (sPokedexView->menuY != 96)
    {
        sPokedexView->menuY += 8;
    }
    else
    {
        if (JOY_NEW(A_BUTTON))
        {
            switch (sPokedexView->menuCursorPos)
            {
            case 0: //BACK TO LIST
            default:
                gMain.newKeys |= START_BUTTON;
                break;
            case 1: //LIST TOP
                sPokedexView->selectedPokemon = 0;
                sPokedexView->pokeBallRotation = 0x40;
                ClearMonSprites();
                CreateMonSpritesAtPos(sPokedexView->selectedPokemon, 0xE);
                gMain.newKeys |= START_BUTTON;
                break;
            case 2: //LIST BOTTOM
                sPokedexView->selectedPokemon = sPokedexView->pokemonListCount - 1;
                sPokedexView->pokeBallRotation = sPokedexView->pokemonListCount * 16 + 0x30;
                ClearMonSprites();
                CreateMonSpritesAtPos(sPokedexView->selectedPokemon, 0xE);
                gMain.newKeys |= START_BUTTON;
                break;
            case 3: //BACK TO POKEDEX
                BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
                gTasks[taskId].func = Task_ReturnToPokedexFromSearchResults;
                PlaySE(SE_TRUCK_DOOR);
                break;
            case 4: //CLOSE POKEDEX
                BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
                gTasks[taskId].func = Task_ClosePokedexFromSearchResultsStartMenu;
                PlaySE(SE_PC_OFF);
                break;
            }
        }

        //Exit menu when Start or B is pressed
        if (JOY_NEW(START_BUTTON | B_BUTTON))
        {
            sPokedexView->menuIsOpen = 0;
            gTasks[taskId].func = Task_HandleSearchResultsInput;
            PlaySE(SE_SELECT);
        }
        else if (JOY_REPEAT(DPAD_UP) && sPokedexView->menuCursorPos)
        {
            sPokedexView->menuCursorPos--;
            PlaySE(SE_SELECT);
        }
        else if (JOY_REPEAT(DPAD_DOWN) && sPokedexView->menuCursorPos <= 3)
        {
            sPokedexView->menuCursorPos++;
            PlaySE(SE_SELECT);
        }
    }
}

static void Task_OpenSearchResultsInfoScreenAfterMonMovement(u8 taskId)
{
    if (gSprites[sPokedexView->selectedMonSpriteId].x == 48
     && gSprites[sPokedexView->selectedMonSpriteId].y == 56)
    {
        sPokedexView->currentPageBackup = sPokedexView->currentPage;
        gTasks[taskId].data[0] = LoadInfoScreen(&sPokedexView->pokedexList[sPokedexView->selectedPokemon], sPokedexView->selectedMonSpriteId);
        gTasks[taskId].func = Task_WaitForExitSearchResultsInfoScreen;
    }
}

static void Task_WaitForExitSearchResultsInfoScreen(u8 taskId)
{
    if (gTasks[gTasks[taskId].data[0]].isActive)
    {
        if (sPokedexView->currentPage == 1 && !IsInfoScreenScrolling(gTasks[taskId].data[0]) && TryDoInfoScreenScroll())
            StartInfoScreenScroll(&sPokedexView->pokedexList[sPokedexView->selectedPokemon], gTasks[taskId].data[0]);
    }
    else
    {
        gTasks[taskId].func = Task_OpenSearchResults;
    }
}

static void Task_ReturnToPokedexFromSearchResults(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        sPokedexView->pokeBallRotation = sPokedexView->pokeBallRotationBackup;
        sPokedexView->selectedPokemon = sPokedexView->selectedPokemonBackup;
        sPokedexView->dexMode = sPokedexView->dexModeBackup;
        if (!IsNationalPokedexEnabled())
            sPokedexView->dexMode = DEX_MODE_HOENN;
        sPokedexView->dexOrder = sPokedexView->dexOrderBackup;
        gTasks[taskId].func = Task_OpenPokedexMainPage;
    }
}

static void Task_ClosePokedexFromSearchResultsStartMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        sPokedexView->pokeBallRotation = sPokedexView->pokeBallRotationBackup;
        sPokedexView->selectedPokemon = sPokedexView->selectedPokemonBackup;
        sPokedexView->dexMode = sPokedexView->dexModeBackup;
        if (!IsNationalPokedexEnabled())
            sPokedexView->dexMode = DEX_MODE_HOENN;
        sPokedexView->dexOrder = sPokedexView->dexOrderBackup;
        gTasks[taskId].func = Task_ClosePokedex;
    }
}

static bool8 LoadPokedexListPage(u8 page)
{
    switch (gMain.state)
    {
    case 0:
    default:
        if (gPaletteFade.active)
            return 0;
        SetVBlankCallback(NULL);
        sPokedexView->currentPage = page;
        ResetOtherVideoRegisters(0);
        REG_BG2VOFS = sPokedexView->initialVOffset;
        LZ77UnCompVram(gPokedexMenu_Gfx, (void *)(VRAM));
        LZ77UnCompVram(gPokedexList_Tilemap, (void *)(VRAM + 0x6800));
        LZ77UnCompVram(gPokedexListUnderlay_Tilemap, (void *)(VRAM + 0x7800));
        DmaClear16(3, VRAM + 0x6000, 0x500);
        if (page == 0)
            LZ77UnCompVram(gPokedexStartMenuMain_Tilemap, (void *)(VRAM + 0x6500));
        else
            LZ77UnCompVram(gPokedexStartMenuSearchResults_Tilemap, (void *)(VRAM + 0x6500));
        ResetPaletteFade();
        if (page == 0)
            sPokedexView->isSearchResults = FALSE;
        else
            sPokedexView->isSearchResults = TRUE;
        LoadPokedexBgPalette();
        gMain.state = 1;
        break;
    case 1:
        ResetSpriteData();
        FreeAllSpritePalettes();
        gReservedSpritePaletteCount = 8;
        LoadCompressedObjectPic(&sInterfaceSpriteSheet[0]);
        LoadSpritePalettes(sInterfaceSpritePalette);
        CreateInterfaceSprites(page);
        gMain.state++;
        break;
    case 2:
        Text_LoadWindowTemplate(&gWindowTemplate_81E7048);
        InitMenuWindow(&gWindowTemplate_81E7048);
        LZ77UnCompVram(sCaughtBall_Gfx, (void *)(VRAM + 0xFF80));
        gMain.state++;
        break;
    case 3:
        if (page == 0)
            CreatePokedexList(sPokedexView->dexMode, sPokedexView->dexOrder);
        CreateMonSpritesAtPos(sPokedexView->selectedPokemon, 0xE);
        sPokedexView->menuIsOpen = 0;
        sPokedexView->menuY = 0;
        gMain.state++;
        break;
    case 4:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        SetVBlankCallback(VBlankCB_Pokedex);
        gMain.state++;
        break;
    case 5:
        REG_WININ = 0x3F3F;
        REG_WINOUT = 0x1D3F;
        REG_WIN0H = 0;
        REG_WIN0V = 0;
        REG_WIN1H = 0;
        REG_WIN1V = 0;
        REG_BLDCNT = 0;
        REG_BLDALPHA = 0;
        REG_BLDY = 0;
        REG_BG3CNT = BGCNT_PRIORITY(3) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(15) | BGCNT_16COLOR | BGCNT_TXT256x256;
        REG_BG1CNT = BGCNT_PRIORITY(1) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(13) | BGCNT_16COLOR | BGCNT_TXT256x256;
        REG_BG0CNT = BGCNT_PRIORITY(0) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(12) | BGCNT_16COLOR | BGCNT_TXT256x256;
        REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG_ALL_ON | DISPCNT_OBJ_ON | DISPCNT_OBJWIN_ON;
        gMain.state++;
        break;
    case 6:
        if (!gPaletteFade.active)
        {
            gMain.state = 0;
            return TRUE;
        }
        break;
    }
    return FALSE;
}

static void LoadPokedexBgPalette(void)
{
    if (sPokedexView->isSearchResults)
        LoadPalette(gPokedexSearchResults_Pal + 1, 1, sizeof(gPokedexSearchResults_Pal) - sizeof(u16));
    else if (!IsNationalPokedexEnabled())
        LoadPalette(gPokedexBgHoenn_Pal + 1, 1, 0xBE);
    else
        LoadPalette(gPokedexBgNational_Pal + 1, 1, sizeof(gPokedexBgNational_Pal) - sizeof(u16));
}

static void CreatePokedexList(u8 dexMode, u8 order)
{
    s16 i, r5, r10;
    u16 vars[3]; //I have no idea why three regular variables are stored in an array, but whatever.

    sPokedexView->pokemonListCount = 0;

    switch (dexMode)
    {
    default:
    case DEX_MODE_HOENN:
        vars[0] = HOENN_DEX_COUNT;
        vars[1] = 1;
        break;
    case DEX_MODE_NATIONAL:
        if (IsNationalPokedexEnabled())
        {
            vars[0] = NATIONAL_DEX_COUNT;
            vars[1] = 0;
        }
        else
        {
            vars[0] = HOENN_DEX_COUNT;
            vars[1] = 1;
        }
        break;
    }

    switch (order)
    {
    case 0:
        if (vars[1])
        {
            for (i = 0; i < vars[0]; i++)
            {
                vars[2] = HoennToNationalOrder(i + 1);
                sPokedexView->pokedexList[i].dexNum = vars[2];
                sPokedexView->pokedexList[i].seen = GetSetPokedexFlag(vars[2], 0);
                sPokedexView->pokedexList[i].owned = GetSetPokedexFlag(vars[2], 1);
                if (sPokedexView->pokedexList[i].seen)
                    sPokedexView->pokemonListCount = i + 1;
            }
        }
        else
        {
            for (i = 0, r5= 0, r10 = 0; i < vars[0]; i++)
            {
                vars[2] = i + 1;
                if (GetSetPokedexFlag(vars[2], 0))
                    r10 = 1;
                if (r10)
                {
                    sPokedexView->pokedexList[r5].dexNum = vars[2];
                    sPokedexView->pokedexList[r5].seen = GetSetPokedexFlag(vars[2], 0);
                    sPokedexView->pokedexList[r5].owned = GetSetPokedexFlag(vars[2], 1);
                    if (sPokedexView->pokedexList[r5].seen)
                        sPokedexView->pokemonListCount = r5 + 1;
                    r5++;
                }
            }
        }
        break;
    case 1:
        for (i = 0; i < POKEMON_SLOTS_NUMBER - 1; i++)
        {
            vars[2] = gPokedexOrder_Alphabetical[i];

            if (NationalToHoennOrder(vars[2]) <= vars[0] && GetSetPokedexFlag(vars[2], 0))
            {
                sPokedexView->pokedexList[sPokedexView->pokemonListCount].dexNum = vars[2];
                sPokedexView->pokedexList[sPokedexView->pokemonListCount].seen = 1;
                sPokedexView->pokedexList[sPokedexView->pokemonListCount].owned = GetSetPokedexFlag(vars[2], 1);
                sPokedexView->pokemonListCount++;
            }
        }
        break;
    case 2:
        for (i = NATIONAL_DEX_COUNT - 1; i >= 0; i--)
        {
            vars[2] = gPokedexOrder_Weight[i];

            if (NationalToHoennOrder(vars[2]) <= vars[0] && GetSetPokedexFlag(vars[2], 1))
            {
                sPokedexView->pokedexList[sPokedexView->pokemonListCount].dexNum = vars[2];
                sPokedexView->pokedexList[sPokedexView->pokemonListCount].seen = 1;
                sPokedexView->pokedexList[sPokedexView->pokemonListCount].owned = 1;
                sPokedexView->pokemonListCount++;
            }
        }
        break;
    case 3:
        for (i = 0; i < NATIONAL_DEX_COUNT; i++)
        {
            vars[2] = gPokedexOrder_Weight[i];

            if (NationalToHoennOrder(vars[2]) <= vars[0] && GetSetPokedexFlag(vars[2], 1))
            {
                sPokedexView->pokedexList[sPokedexView->pokemonListCount].dexNum = vars[2];
                sPokedexView->pokedexList[sPokedexView->pokemonListCount].seen = 1;
                sPokedexView->pokedexList[sPokedexView->pokemonListCount].owned = 1;
                sPokedexView->pokemonListCount++;
            }
        }
        break;
    case 4:
        for (i = NATIONAL_DEX_COUNT - 1; i >=0; i--)
        {
            vars[2] = gPokedexOrder_Height[i];

            if (NationalToHoennOrder(vars[2]) <= vars[0] && GetSetPokedexFlag(vars[2], 1))
            {
                sPokedexView->pokedexList[sPokedexView->pokemonListCount].dexNum = vars[2];
                sPokedexView->pokedexList[sPokedexView->pokemonListCount].seen = 1;
                sPokedexView->pokedexList[sPokedexView->pokemonListCount].owned = 1;
                sPokedexView->pokemonListCount++;
            }
        }
        break;
    case 5:
        for (i = 0; i < NATIONAL_DEX_COUNT; i++)
        {
            vars[2] = gPokedexOrder_Height[i];

            if (NationalToHoennOrder(vars[2]) <= vars[0] && GetSetPokedexFlag(vars[2], 1))
            {
                sPokedexView->pokedexList[sPokedexView->pokemonListCount].dexNum = vars[2];
                sPokedexView->pokedexList[sPokedexView->pokemonListCount].seen = 1;
                sPokedexView->pokedexList[sPokedexView->pokemonListCount].owned = 1;
                sPokedexView->pokemonListCount++;
            }
        }
        break;
    }

    for (i = sPokedexView->pokemonListCount; i < NATIONAL_DEX_COUNT; i++)
    {
        sPokedexView->pokedexList[i].dexNum |= 0xFFFF;
        sPokedexView->pokedexList[i].seen = 0;
        sPokedexView->pokedexList[i].owned = 0;
    }
}

static void CreateMonListEntry(u8 position, u16 b, u16 screenBase)
{
    s16 entryNum;
    u16 i;
    u16 vOffset;

    switch (position)
    {
    case 0:
    default:
        entryNum = b - 5;
        for (i = 0; i <= 10; i++)
        {
            if (entryNum < 0 || entryNum >= NATIONAL_DEX_COUNT || sPokedexView->pokedexList[entryNum].dexNum == 0xFFFF)
            {
                ClearMonListEntry(0x11, i * 2, screenBase);
            }
            else
            {
                ClearMonListEntry(0x11, i * 2, screenBase);
                if (sPokedexView->pokedexList[entryNum].seen)
                {
                    CreateMonDexNum(entryNum, 0x12, i * 2, screenBase);
                    CreateCaughtBall(sPokedexView->pokedexList[entryNum].owned, 0x11, i * 2, screenBase);
                    CreateMonName(sPokedexView->pokedexList[entryNum].dexNum, 0x17, i * 2);
                }
                else
                {
                    CreateMonDexNum(entryNum, 0x12, i * 2, screenBase);
                    CreateCaughtBall(0, 0x11, i * 2, screenBase);
                    CreateMonName(0, 0x17, i * 2);
                }
            }
            entryNum++;
        }
        break;
    case 1:
        entryNum = b - 5;
        if (entryNum < 0 || entryNum >= NATIONAL_DEX_COUNT || sPokedexView->pokedexList[entryNum].dexNum == 0xFFFF)
        {
            ClearMonListEntry(0x11, sPokedexView->listVOffset * 2, screenBase);
        }
        else
        {
            ClearMonListEntry(0x11, sPokedexView->listVOffset * 2, screenBase);
            if (sPokedexView->pokedexList[entryNum].seen)
            {
                CreateMonDexNum(entryNum, 0x12, sPokedexView->listVOffset * 2, screenBase);
                CreateCaughtBall(sPokedexView->pokedexList[entryNum].owned, 0x11, sPokedexView->listVOffset * 2,
                    screenBase);
                CreateMonName(sPokedexView->pokedexList[entryNum].dexNum, 0x17, sPokedexView->listVOffset * 2);
            }
            else
            {
                CreateMonDexNum(entryNum, 0x12, sPokedexView->listVOffset * 2, screenBase);
                CreateCaughtBall(0, 0x11, sPokedexView->listVOffset * 2, screenBase);
                CreateMonName(0, 0x17, sPokedexView->listVOffset * 2);
            }
        }
        break;
    case 2:
        entryNum = b + 5;
        vOffset = sPokedexView->listVOffset + 10;
        if (vOffset > 15)
            vOffset -= 16;
        if (entryNum < 0 || entryNum >= NATIONAL_DEX_COUNT || sPokedexView->pokedexList[entryNum].dexNum == 0xFFFF)
            ClearMonListEntry(0x11, vOffset * 2, screenBase);
        else
        {
            ClearMonListEntry(0x11, vOffset * 2, screenBase);
            if (sPokedexView->pokedexList[entryNum].seen)
            {
                CreateMonDexNum(entryNum, 0x12, vOffset * 2, screenBase);
                CreateCaughtBall(sPokedexView->pokedexList[entryNum].owned, 0x11, vOffset * 2, screenBase);
                CreateMonName(sPokedexView->pokedexList[entryNum].dexNum, 0x17, vOffset * 2);
            }
            else
            {
                CreateMonDexNum(entryNum, 0x12, vOffset * 2, screenBase);
                CreateCaughtBall(0, 0x11, vOffset * 2, screenBase);
                CreateMonName(0, 0x17, vOffset * 2);
            }
        }
        break;
    }
}

static void CreateMonDexNum(u16 entryNum, u8 left, u8 top, u16 screenBase)
{
    u8 text[4];
    u16 unk[2];
    u16 dexNum = sPokedexView->pokedexList[entryNum].dexNum;

    if (sPokedexView->dexMode == DEX_MODE_HOENN)
        dexNum = NationalToHoennOrder(dexNum);
    unk[0] = 0x3FC;
    unk[1] = 0x3FD;
    text[0] = CHAR_0 + dexNum / 100;
    text[1] = CHAR_0 + (dexNum % 100) / 10;
    text[2] = CHAR_0 + (dexNum % 100) % 10;
    text[3] = EOS;
    *(u16 *)(BG_VRAM + screenBase * 0x800 + top * 0x40 + left * 2) = unk[0];
    *(u16 *)(BG_VRAM + screenBase * 0x800 + (top + 1) * 0x40 + left * 2) = unk[1];
    Menu_PrintText(text, left - 15, top);
}

static void CreateCaughtBall(bool16 owned, u8 x, u8 y, u16 screenBase)
{
    u16 unk[2];

    if (owned)
    {
        unk[0] = 0x3FE;
        unk[1] = 0x3FF;
    }
    else
    {
        unk[0] = 0;
        unk[1] = 0;
    }
    *(u16 *)(BG_VRAM + screenBase * 0x800 + y * 0x40 + x * 2) = unk[0];
    *(u16 *)(BG_VRAM + screenBase * 0x800 + (y + 1) * 0x40 + x * 2) = unk[1];
}

static u8 CreateMonName(u16 num, u8 left, u8 top)
{
    u8 text[POKEMON_NAME_LENGTH + (MODERN ? 1 : 0)];
    u8 i;

    for (i = 0; i < 10; i++)
        text[i] = CHAR_SPACE;
    text[i] = EOS;

    num = NationalPokedexNumToSpecies(num);
    switch (num)
    {
    default:
        for (i = 0; gSpeciesNames[num][i] != EOS && i < POKEMON_NAME_LENGTH; i++)
            text[i] = gSpeciesNames[num][i];
        break;
    case 0:
        for (i = 0; i < POKEMON_NAME_LENGTH; i++)
            text[i] = CHAR_HYPHEN;
        break;
    }
    Menu_PrintTextPixelCoords(text, (left - 0x11) * 8 + 0xFC, top * 8, 0);
    return i;
}

static void ClearMonListEntry(u8 x, u8 y, u16 screenBase)
{
    u8 i;

    for (i = 0; i < 12; i++)
    {
        *(u16 *)(BG_VRAM + screenBase * 0x800 + y * 64 + (x + i) * 2) = 0;
        *(u16 *)(BG_VRAM + screenBase * 0x800 + (y + 1) * 64 + (x + i) * 2) = 0;
    }
}

static void CreateMonSpritesAtPos(u16 selectedMon, u16 b)
{
    u8 i;
    u16 dexNum;
    u8 spriteId;

    for (i = 0; i < 4; i++)
        sPokedexView->monSpriteIds[i] = 0xFFFF;
    sPokedexView->selectedMonSpriteId = 0xFFFF;
    CreateMonListEntry(0, selectedMon, b);
    REG_BG2VOFS = sPokedexView->initialVOffset;

    dexNum = GetPokemonSpriteToDisplay(selectedMon - 1);
    if (dexNum != 0xFFFF)
    {
        spriteId = CreatePokedexMonSprite(dexNum, 0x60, 0x50);
        gSprites[spriteId].callback = SpriteCB_PokedexListMonSprite;
        gSprites[spriteId].data[5] = -32;
    }

    dexNum = GetPokemonSpriteToDisplay(selectedMon);
    if (dexNum != 0xFFFF)
    {
        spriteId = CreatePokedexMonSprite(dexNum, 0x60, 0x50);
        gSprites[spriteId].callback = SpriteCB_PokedexListMonSprite;
        gSprites[spriteId].data[5] = 0;
    }

    dexNum = GetPokemonSpriteToDisplay(selectedMon + 1);
    if (dexNum != 0xFFFF)
    {
        spriteId = CreatePokedexMonSprite(dexNum, 0x60, 0x50);
        gSprites[spriteId].callback = SpriteCB_PokedexListMonSprite;
        gSprites[spriteId].data[5] = 32;
    }

    sPokedexView->listVOffset = 0;
    sPokedexView->listMovingVOffset = 0;
}

static bool8 UpdateDexListScroll(u8 direction, u8 monMoveIncrement, u8 scrollTimerMax)
{
    u16 i;
    u8 step;

    if (sPokedexView->scrollTimer)
    {
        sPokedexView->scrollTimer--;
        switch (direction)
        {
        case 1:
            for (i = 0; i < 4; i++)
            {
                if (sPokedexView->monSpriteIds[i] != 0xFFFF)
                    gSprites[sPokedexView->monSpriteIds[i]].data[5] += monMoveIncrement;
            }
            step = 16 * (scrollTimerMax - sPokedexView->scrollTimer) / scrollTimerMax;
            REG_BG2VOFS = sPokedexView->initialVOffset + sPokedexView->listMovingVOffset * 16 - step;
            sPokedexView->pokeBallRotation -= sPokedexView->pokeBallRotationStep;
            break;
        case 2:
            for (i = 0; i < 4; i++)
            {
                if (sPokedexView->monSpriteIds[i] != 0xFFFF)
                    gSprites[sPokedexView->monSpriteIds[i]].data[5] -= monMoveIncrement;
            }
            step = 16 * (scrollTimerMax - sPokedexView->scrollTimer) / scrollTimerMax;
            REG_BG2VOFS = sPokedexView->initialVOffset + sPokedexView->listMovingVOffset * 16 + step;
            sPokedexView->pokeBallRotation += sPokedexView->pokeBallRotationStep;
            break;
        }
        return FALSE;
    }
    else
    {
        REG_BG2VOFS = sPokedexView->initialVOffset + sPokedexView->listVOffset * 16;
        return TRUE;
    }
}

static void CreateScrollingPokemonSprite(u8 direction, u16 selectedMon)
{
    u16 dexNum;
    u8 spriteId;

    sPokedexView->listMovingVOffset = sPokedexView->listVOffset;
    switch (direction)
    {
    case 1:
        dexNum = GetPokemonSpriteToDisplay(selectedMon - 1);
        if (dexNum != 0xFFFF)
        {
            spriteId = CreatePokedexMonSprite(dexNum, 0x60, 0x50);
            gSprites[spriteId].callback = SpriteCB_PokedexListMonSprite;
            gSprites[spriteId].data[5] = -64;
        }
        if (sPokedexView->listVOffset > 0)
            sPokedexView->listVOffset--;
        else
            sPokedexView->listVOffset = 15;
        break;
    case 2:
        dexNum = GetPokemonSpriteToDisplay(selectedMon + 1);
        if (dexNum != 0xFFFF)
        {
            spriteId = CreatePokedexMonSprite(dexNum, 0x60, 0x50);
            gSprites[spriteId].callback = SpriteCB_PokedexListMonSprite;
            gSprites[spriteId].data[5] = 0x40;
        }
        if (sPokedexView->listVOffset <= 0xE)
            sPokedexView->listVOffset++;
        else
            sPokedexView->listVOffset = 0;
        break;
    }
}

// Ugly, ugly, ugly. I couldn't get it to match otherwise.
static u16 TryDoPokedexScroll(u16 selectedMon, u16 b)
{
    u8 scrollTimer;
    u8 scrollMonIncrement;
    u8 i;
    u16 startingPos;
    u8 scrollDir = 0;

    if (!(JOY_HELD(DPAD_UP) && (selectedMon > 0)))
    {
        //_0808E4B6
        if (!(JOY_HELD(DPAD_DOWN) && (selectedMon < sPokedexView->pokemonListCount - 1)))
        //_0808E4CE
        {
            if (JOY_NEW(DPAD_LEFT) && (selectedMon > 0))
            {
                startingPos = selectedMon;
                //_0808E4E0
                for (i = 0; i < 7; i++)
                    selectedMon = GetNextPosition(1, selectedMon, 0, sPokedexView->pokemonListCount - 1);
                sPokedexView->pokeBallRotation += 16 * (selectedMon - startingPos);
                ClearMonSprites();
                CreateMonSpritesAtPos(selectedMon, 0xE);
                PlaySE(0x6D);
            }
            //_0808E53C
            else if (JOY_NEW(DPAD_RIGHT) && (selectedMon < sPokedexView->pokemonListCount - 1))
            {
                startingPos = selectedMon;
                for (i = 0; i < 7; i++)
                    selectedMon = GetNextPosition(0, selectedMon, 0, sPokedexView->pokemonListCount - 1);
                sPokedexView->pokeBallRotation += (selectedMon - startingPos) * 16;
                ClearMonSprites();
                CreateMonSpritesAtPos(selectedMon, 0xE);
                PlaySE(0x6D);
            }
          _0808E5A2:
            if (scrollDir == 0)
            {
                sPokedexView->scrollSpeed = 0;
                return selectedMon;
            }
        }
        else
        {
            // to _0808E5C4
            scrollDir = 2;
            selectedMon = GetNextPosition(0, selectedMon, 0, sPokedexView->pokemonListCount - 1);
            CreateScrollingPokemonSprite(2, selectedMon);
            //goto _0808E60E
            CreateMonListEntry(2, selectedMon, b);
            PlaySE(0x6C);
            goto _0808E5A2;
        }
    }
    else
    {
        //to _0808E5E4
        scrollDir = 1;
        selectedMon = GetNextPosition(1, selectedMon, 0, sPokedexView->pokemonListCount - 1);
        CreateScrollingPokemonSprite(1, selectedMon);
        //_0808E60E
        CreateMonListEntry(1, selectedMon, b);
        PlaySE(0x6C);
        goto _0808E5A2;
    }
    //_0808E628
    scrollMonIncrement = sScrollMonIncrements[sPokedexView->scrollSpeed / 4];
    scrollTimer = sScrollTimers[sPokedexView->scrollSpeed / 4];
    sPokedexView->scrollTimer = scrollTimer;
    sPokedexView->maxScrollTimer = scrollTimer;
    sPokedexView->scrollMonIncrement = scrollMonIncrement;
    sPokedexView->scrollDirection = scrollDir;
    sPokedexView->pokeBallRotationStep = scrollMonIncrement / 2;
    UpdateDexListScroll(sPokedexView->scrollDirection,
        sPokedexView->scrollMonIncrement,
        sPokedexView->maxScrollTimer);
    if (sPokedexView->scrollSpeed <= 0xB)
        sPokedexView->scrollSpeed++;
    return selectedMon;
}

static void UpdateSelectedMonSpriteId(void)
{
    u16 i;

    for (i = 0; i < 4; i++)
    {
        u16 spriteId = sPokedexView->monSpriteIds[i];

        if (gSprites[spriteId].x2 == 0 && gSprites[spriteId].y2 == 0 && spriteId != 0xFFFF)
            sPokedexView->selectedMonSpriteId = spriteId;
    }
}

static bool8 TryDoInfoScreenScroll(void)
{
    u16 nextPokemon;
    u16 selectedPokemon = sPokedexView->selectedPokemon;

    if (JOY_NEW(DPAD_UP) && selectedPokemon)
    {
        nextPokemon = selectedPokemon;
        while (nextPokemon != 0)
        {
            nextPokemon = GetNextPosition(1, nextPokemon, 0, sPokedexView->pokemonListCount - 1);

            if (sPokedexView->pokedexList[nextPokemon].seen)
            {
                selectedPokemon = nextPokemon;
                break;
            }
        }

        if (sPokedexView->selectedPokemon == selectedPokemon)
            return 0;
        else
        {
            sPokedexView->selectedPokemon = selectedPokemon;
            sPokedexView->pokeBallRotation -= 16;
            return 1;
        }
    }
    else if (JOY_NEW(DPAD_DOWN) && selectedPokemon < sPokedexView->pokemonListCount - 1)
    {
        nextPokemon = selectedPokemon;
        while (nextPokemon < sPokedexView->pokemonListCount - 1)
        {
            nextPokemon = GetNextPosition(0, nextPokemon, 0, sPokedexView->pokemonListCount - 1);

            if (sPokedexView->pokedexList[nextPokemon].seen)
            {
                selectedPokemon = nextPokemon;
                break;
            }
        }

        if (sPokedexView->selectedPokemon == selectedPokemon)
            return 0;
        else
        {
            sPokedexView->selectedPokemon = selectedPokemon;
            sPokedexView->pokeBallRotation += 16;
            return 1;
        }
    }
    return 0;
}

static u8 ClearMonSprites(void)
{
    u16 i;

    for (i = 0; i < 4; i++)
    {
        if (sPokedexView->monSpriteIds[i] != 0xFFFF)
        {
            DestroySprite(&gSprites[sPokedexView->monSpriteIds[i]]);
            sPokedexView->monSpriteIds[i] |= 0xFFFF;
        }
    }
    return 0;
}

static u16 GetPokemonSpriteToDisplay(u16 species)
{
    if (species >= NATIONAL_DEX_COUNT || sPokedexView->pokedexList[species].dexNum == 0xFFFF)
        return 0xFFFF;
    else if (sPokedexView->pokedexList[species].seen)
        return sPokedexView->pokedexList[species].dexNum;
    else
        return 0;
}

static u32 CreatePokedexMonSprite(u16 num, s16 x, s16 y)
{
    u8 i;

    for (i = 0; i < 4; i++)
    {
        if (sPokedexView->monSpriteIds[i] == 0xFFFF)
        {
            u8 spriteId = CreateMonSpriteFromNationalDexNumber(num, x, y, i);

            gSprites[spriteId].oam.affineMode = ST_OAM_AFFINE_NORMAL;
            gSprites[spriteId].oam.priority = 3;
            gSprites[spriteId].data[0] = 0;
            gSprites[spriteId].data[1] = i;
            gSprites[spriteId].data[2] = NationalPokedexNumToSpecies(num);
            sPokedexView->monSpriteIds[i] = spriteId;
            return spriteId;
        }
    }
    return 0xFFFF;
}

static void CreateInterfaceSprites(u8 page)
{
    u8 spriteId;
    u16 digitNum;

    spriteId = CreateSprite(&sScrollArrowSpriteTemplate, 184, 4, 0);
    gSprites[spriteId].data[1] = 0;

    spriteId = CreateSprite(&sScrollArrowSpriteTemplate, 184, 156, 0);
    gSprites[spriteId].data[1] = 1;
    gSprites[spriteId].vFlip = TRUE;

    CreateSprite(&sScrollBarSpriteTemplate, 234, 20, 0);
    CreateSprite(&sInterfaceTextSpriteTemplate, 16, 138, 0);

    spriteId = CreateSprite(&sInterfaceTextSpriteTemplate, 48, 138, 0);
    StartSpriteAnim(&gSprites[spriteId], 3);

    spriteId = CreateSprite(&sInterfaceTextSpriteTemplate, 16, 158, 0);
    StartSpriteAnim(&gSprites[spriteId], 2);
    gSprites[spriteId].data[2] = 0x80;

    spriteId = CreateSprite(&sInterfaceTextSpriteTemplate, 48, 158, 0);
    StartSpriteAnim(&gSprites[spriteId], 1);

    spriteId = CreateSprite(&sRotatingPokeBallSpriteTemplate, 0, 80, 2);
    gSprites[spriteId].oam.affineMode = 1;
    gSprites[spriteId].oam.matrixNum = 30;
    gSprites[spriteId].data[0] = 0x1E;
    gSprites[spriteId].data[1] = 0;

    spriteId = CreateSprite(&sRotatingPokeBallSpriteTemplate, 0, 80, 2);
    gSprites[spriteId].oam.affineMode = 1;
    gSprites[spriteId].oam.matrixNum = 31;
    gSprites[spriteId].data[0] = 0x1F;
    gSprites[spriteId].data[1] = 0x80;

    if (page == 0)
    {
        u32 drawNextDigit;

        CreateSprite(&sSeenOwnTextSpriteTemplate, 32, 40, 1);

        spriteId = CreateSprite(&sSeenOwnTextSpriteTemplate, 32, 72, 1);
        StartSpriteAnim(&gSprites[spriteId], 1);
        drawNextDigit = 0;

        spriteId = CreateSprite(&gSpriteTemplate_83A059C, 28, 48, 1);
        digitNum = sPokedexView->seenCount / 100;
        StartSpriteAnim(&gSprites[spriteId], digitNum);
        if (digitNum != 0)
            drawNextDigit = 1;
        else
            gSprites[spriteId].invisible = TRUE;

        spriteId = CreateSprite(&gSpriteTemplate_83A059C, 34, 48, 1);
        digitNum = (sPokedexView->seenCount % 100) / 10;
        if (digitNum != 0 || drawNextDigit != 0)
            StartSpriteAnim(&gSprites[spriteId], digitNum);
        else
            gSprites[spriteId].invisible = TRUE;

        spriteId = CreateSprite(&gSpriteTemplate_83A059C, 40, 48, 1);
        digitNum = (sPokedexView->seenCount % 100) % 10;
        StartSpriteAnim(&gSprites[spriteId], digitNum);
        drawNextDigit = 0;

        spriteId = CreateSprite(&gSpriteTemplate_83A059C, 28, 80, 1);
        digitNum = sPokedexView->ownCount / 100;
        StartSpriteAnim(&gSprites[spriteId], digitNum);
        if (digitNum != 0)
            drawNextDigit = 1;
        else
            gSprites[spriteId].invisible = TRUE;

        spriteId = CreateSprite(&gSpriteTemplate_83A059C, 34, 80, 1);
        digitNum = (sPokedexView->ownCount % 100) / 10;
        if (digitNum != 0 || drawNextDigit != 0)
            StartSpriteAnim(&gSprites[spriteId], digitNum);
        else
            gSprites[spriteId].invisible = TRUE;

        spriteId = CreateSprite(&gSpriteTemplate_83A059C, 40, 80, 1);
        digitNum = (sPokedexView->ownCount % 100) % 10;
        StartSpriteAnim(&gSprites[spriteId], digitNum);

        spriteId = CreateSprite(&sDexListStartMenuCursorSpriteTemplate, 140, 96, 1);
        gSprites[spriteId].invisible = TRUE;
    }
    else
    {
        spriteId = CreateSprite(&sDexListStartMenuCursorSpriteTemplate, 140, 80, 1);
        gSprites[spriteId].invisible = TRUE;
    }
}

static void SpriteCB_EndMoveMonForInfoScreen(struct Sprite *sprite)
{
}

static void SpriteCB_SeenOwnInfo(struct Sprite *sprite)
{
    if (sPokedexView->currentPage != 0)
        DestroySprite(sprite);
}

//Move Pokemon into position for description page
static void SpriteCB_MoveMonForInfoScreen(struct Sprite *sprite)
{
    sprite->oam.priority = 0;
    sprite->oam.affineMode = 0;
    sprite->x2 = 0;
    sprite->y2 = 0;
    if (sprite->x != 48 || sprite->y != 56)
    {
        if (sprite->x > 48)
            sprite->x--;
        if (sprite->x < 48)
            sprite->x++;

        if (sprite->y > 56)
            sprite->y--;
        if (sprite->y < 56)
            sprite->y++;
    }
    else
    {
        sprite->callback = SpriteCB_EndMoveMonForInfoScreen;
    }
}

static void SpriteCB_PokedexListMonSprite(struct Sprite *sprite)
{
    u8 monId = sprite->data[1];

    if (sPokedexView->currentPage != 0 && sPokedexView->currentPage != 3)
    {
        DestroySprite(sprite);
        sPokedexView->monSpriteIds[monId] = 0xFFFF;
    }
    else
    {
        u32 var;

        sprite->y2 = gSineTable[(u8)sprite->data[5]] * 76 / 256;
        var = 0x10000 / gSineTable[sprite->data[5] + 0x40];
        if (var > 0xFFFF)
            var = 0xFFFF;
        SetOamMatrix(sprite->data[1] + 1, 0x100, 0, 0, var);
        sprite->oam.matrixNum = monId + 1;

        if (sprite->data[5] > -64 && sprite->data[5] < 64)
        {
            sprite->invisible = FALSE;
            sprite->data[0] = 1;
        }
        else
        {
            sprite->invisible = TRUE;
        }

        if ((sprite->data[5] <= -64 || sprite->data[5] >= 64) && sprite->data[0] != 0)
        {
            DestroySprite(sprite);
            sPokedexView->monSpriteIds[monId] = 0xFFFF;
        }
    }
}

static void SpriteCB_Scrollbar(struct Sprite *sprite)
{
    if (sPokedexView->currentPage != 0 && sPokedexView->currentPage != 3)
        DestroySprite(sprite);
    else
        sprite->y2 = sPokedexView->selectedPokemon * 120 / (sPokedexView->pokemonListCount - 1);
}

static void SpriteCB_ScrollArrow(struct Sprite *sprite)
{
    if (sPokedexView->currentPage != 0 && sPokedexView->currentPage != 3)
    {
        DestroySprite(sprite);
    }
    else
    {
        u8 r0;

        if (sprite->data[1] != 0)
        {
            if (sPokedexView->selectedPokemon == sPokedexView->pokemonListCount - 1)
                sprite->invisible = TRUE;
            else
                sprite->invisible = FALSE;
            r0 = sprite->data[2];
        }
        else
        {
            if (sPokedexView->selectedPokemon == 0)
                sprite->invisible = TRUE;
            else
                sprite->invisible = FALSE;
            r0 = sprite->data[2] - 128;
        }
        sprite->y2 = gSineTable[r0] / 64;
        sprite->data[2] = sprite->data[2] + 8;
        if (sPokedexView->menuIsOpen == 0 && sPokedexView->menuY == 0 && sprite->invisible == 0)
            sprite->invisible = FALSE;
        else
            sprite->invisible = TRUE;
    }
}

static void SpriteCB_DexListInterfaceText(struct Sprite *sprite)
{
    if (sPokedexView->currentPage != 0 && sPokedexView->currentPage != 3)
        DestroySprite(sprite);
}

static void SpriteCB_RotatingPokeBall(struct Sprite *sprite)
{
    if (sPokedexView->currentPage != 0 && sPokedexView->currentPage != 3)
    {
        DestroySprite(sprite);
    }
    else
    {
        u8 val;
        s16 r3;
        s16 r0;

        val = sPokedexView->pokeBallRotation + sprite->data[1];
        r3 = gSineTable[val];
        r0 = gSineTable[val + 0x40];
        SetOamMatrix(sprite->data[0], r0, r3, -r3, r0);

        val = sPokedexView->pokeBallRotation + (sprite->data[1] + 0x40);
        r3 = gSineTable[val];
        r0 = gSineTable[val + 0x40];
        sprite->x2 = r0 * 40 / 256;
        sprite->y2 = r3 * 40 / 256;
    }
}

static void SpriteCB_DexListStartMenuCursor(struct Sprite *sprite)
{
    if (sPokedexView->currentPage != 0 && sPokedexView->currentPage != 3)
    {
        DestroySprite(sprite);
    }
    else
    {
        u16 r1 = sPokedexView->currentPage == 0 ? 80 : 96;

        if (sPokedexView->menuIsOpen != 0 && sPokedexView->menuY == r1)
        {
            sprite->invisible = FALSE;
            sprite->y2 = sPokedexView->menuCursorPos * 16;
            sprite->x2 = gSineTable[(u8)sprite->data[2]] / 64;
            sprite->data[2] += 8;
        }
        else
        {
            sprite->invisible = TRUE;
        }
    }
}

static u8 LoadInfoScreen(struct PokedexListItem *item, u8 monSpriteId)
{
    u8 taskId;

    sPokedexListItem = item;
    taskId = CreateTask(Task_LoadInfoScreen, 0);
    gTasks[taskId].data[0] = 0;
    gTasks[taskId].data[1] = 1;
    gTasks[taskId].data[2] = 0;
    gTasks[taskId].data[3] = 0;
    gTasks[taskId].data[4] = monSpriteId;
    return taskId;
}

static bool8 IsInfoScreenScrolling(u8 taskId)
{
    if (gTasks[taskId].data[0] == 0 && gTasks[taskId].func == Task_HandleInfoScreenInput)
        return FALSE;
    else
        return TRUE;
}

static u8 StartInfoScreenScroll(struct PokedexListItem *item, u8 taskId)
{
    sPokedexListItem = item;
    gTasks[taskId].data[0] = 1;
    gTasks[taskId].data[1] = 0;
    gTasks[taskId].data[2] = 0;
    gTasks[taskId].data[3] = 0;
    return taskId;
}

#if ENGLISH
#define CATEGORY_LEFT (11)
#elif GERMAN
#define CATEGORY_LEFT (16)
#endif

static void Task_LoadInfoScreen(u8 taskId)
{
    switch (gMain.state)
    {
    case 0:
    default:
        if (!gPaletteFade.active)
        {
            u16 r2;

            sPokedexView->currentPage = 1;
            sPokedexView->descriptionPageNum = 0;
            gPokedexVBlankCB = gMain.vblankCallback;
            SetVBlankCallback(NULL);
            r2 = 0;
            if (gTasks[taskId].data[1] != 0)
                r2 += 0x1000;
            if (gTasks[taskId].data[2] != 0)
                r2 |= 0x200;
            ResetOtherVideoRegisters(r2);
            gMain.state = 1;
        }
        break;
    case 1:
        LZ77UnCompVram(gPokedexMenu_Gfx, (void *)VRAM);
        LZ77UnCompVram(gPokedexInfoScreen_Tilemap, (void *)(VRAM + 0x7800));
        DrawFootprint(sPokedexListItem->dexNum, 2, 0x3FC);
        gMain.state++;
        break;
    case 2:
        LoadScreenSelectBarMain(0xD);
        HighlightScreenSelectBarItem(sPokedexView->selectedScreen, 0xD);
        LoadPokedexBgPalette();
        gMain.state++;
        break;
    case 3:
        Text_LoadWindowTemplate(&gWindowTemplate_81E7064);
        InitMenuWindow(&gWindowTemplate_81E7064);
        gMain.state++;
        break;
    case 4:
        if (sPokedexView->dexMode == DEX_MODE_HOENN)
            PrintEntryScreenDexNum(NationalToHoennOrder(sPokedexListItem->dexNum), 0xD, 3);
        else
            PrintEntryScreenDexNum(sPokedexListItem->dexNum, 0xD, 3);
        PrintEntryScreenSpeciesName(sPokedexListItem->dexNum, 0x10, 3);
        Menu_PrintText(gText_5MarksPokemon, CATEGORY_LEFT, 5);
        Menu_PrintText(gText_UnkHeight, 16, 7);
        Menu_PrintText(gText_UnkWeight, 16, 9);
        if (sPokedexListItem->owned)
        {
            UnusedPrintMonName(gPokedexEntries[sPokedexListItem->dexNum].categoryName, CATEGORY_LEFT, 5);
            PrintMonHeight(gPokedexEntries[sPokedexListItem->dexNum].height, 16, 7);
            PrintMonWeight(gPokedexEntries[sPokedexListItem->dexNum].weight, 16, 9);
            Menu_PrintText(gPokedexEntries[sPokedexListItem->dexNum].descriptionPage1, 2, 13);
            RS_DrawFootprint(14, 0x3FC);
        }
        else
        {
            Menu_PrintText(sExpandedPlaceholder_PokedexDescription, 2, 13);
            LoadPalette(gPlttBufferUnfaded + 1, 0x31, 0x1E);
        }
        gMain.state++;
        break;
    case 5:
        if (gTasks[taskId].data[1] == 0)
        {
            gTasks[taskId].data[4] = (u16)CreateMonSpriteFromNationalDexNumber(sPokedexListItem->dexNum, 0x30, 0x38, 0);
            gSprites[gTasks[taskId].data[4]].oam.priority = 0;
        }
        gMain.state++;
        break;
    case 6:
        {
            u32 excludedPalettes = 0;

            if (gTasks[taskId].data[2] != 0)
                excludedPalettes = 0x14;
            if (gTasks[taskId].data[1] != 0)
                excludedPalettes |= (1 << (gSprites[gTasks[taskId].data[4]].oam.paletteNum + 16));
            BeginNormalPaletteFade(~excludedPalettes, 0, 16, 0, RGB(0, 0, 0));
            SetVBlankCallback(gPokedexVBlankCB);
            gMain.state++;
        }
        break;
    case 7:
        REG_BLDCNT = 0;
        REG_BLDALPHA = 0;
        REG_BLDY = 0;
        REG_BG3CNT = BGCNT_PRIORITY(3) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(15) | BGCNT_16COLOR | BGCNT_TXT256x256;
        REG_BG1CNT = BGCNT_PRIORITY(0) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(13) | BGCNT_16COLOR | BGCNT_TXT256x256;
        REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG1_ON | DISPCNT_BG2_ON | DISPCNT_BG3_ON | DISPCNT_OBJ_ON;
        gMain.state++;
        break;
    case 8:
        if (!gPaletteFade.active)
        {
            gMain.state++;
            if (gTasks[taskId].data[3] == 0)
            {
                StopCryAndClearCrySongs();
                PlayCry_NormalNoDucking(NationalPokedexNumToSpecies(sPokedexListItem->dexNum), 0, 0x7D, 0xA);
            }
            else
            {
                gMain.state++;
            }
        }
        break;
    case 9:
        if (!IsCryPlayingOrClearCrySongs())
            gMain.state++;
        break;
    case 10:
        gTasks[taskId].data[0] = 0;
        gTasks[taskId].data[1] = 0;
        gTasks[taskId].data[2] = 1;
        gTasks[taskId].data[3] = 1;
        gTasks[taskId].func = Task_HandleInfoScreenInput;
        gMain.state = 0;
        break;
    }
}

static void Task_HandleInfoScreenInput(u8 taskId)
{
    if (gTasks[taskId].data[0] != 0)
    {
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        gTasks[taskId].func = Task_LoadInfoScreenWaitForFade;
        PlaySE(SE_DEX_SCROLL);
        return;
    }
    if (JOY_NEW(B_BUTTON))
    {
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        gTasks[taskId].func = Task_ExitInfoScreen;
        PlaySE(SE_PC_OFF);
        return;
    }
    if (JOY_NEW(A_BUTTON))
    {
        switch (sPokedexView->selectedScreen)
        {
        case PAGE_SCREEN:
            sub_8090C68();
            break;
        case AREA_SCREEN:
            BeginNormalPaletteFade(0xFFFFFFEB, 0, 0, 16, RGB(0, 0, 0));
            gTasks[taskId].func = Task_LoadAreaScreen;
            PlaySE(SE_PIN);
            break;
        case CRY_SCREEN:
            BeginNormalPaletteFade(0xFFFFFFEB, 0, 0, 16, RGB(0, 0, 0));
            gTasks[taskId].func = Task_LoadCryScreen;
            PlaySE(SE_PIN);
            break;
        case SIZE_SCREEN:
            if (!sPokedexListItem->owned)
            {
                PlaySE(SE_FAILURE);
            }
            else
            {
                BeginNormalPaletteFade(0xFFFFFFEB, 0, 0, 16, RGB(0, 0, 0));
                gTasks[taskId].func = Task_LoadSizeScreen;
                PlaySE(SE_PIN);
            }
            break;
        }
        return;
    }
    if ((JOY_NEW(DPAD_LEFT)
     || (JOY_NEW(L_BUTTON) && gSaveBlock2.optionsButtonMode == OPTIONS_BUTTON_MODE_LR))
     && sPokedexView->selectedScreen > 0)
    {
        sPokedexView->selectedScreen--;
        HighlightScreenSelectBarItem(sPokedexView->selectedScreen, 0xD);
        PlaySE(SE_DEX_PAGE);
        return;
    }
    if ((JOY_NEW(DPAD_RIGHT)
     || (JOY_NEW(R_BUTTON) && gSaveBlock2.optionsButtonMode == OPTIONS_BUTTON_MODE_LR))
     && sPokedexView->selectedScreen < 3)
    {
        sPokedexView->selectedScreen++;
        HighlightScreenSelectBarItem(sPokedexView->selectedScreen, 0xD);
        PlaySE(SE_DEX_PAGE);
        return;
    }
}

static void Task_LoadInfoScreenWaitForFade(u8 taskId)
{
    if (!gPaletteFade.active)
        gTasks[taskId].func = Task_LoadInfoScreen;
}

static void Task_ExitInfoScreen(u8 taskId)
{
    if (!gPaletteFade.active)
        DestroyTask(taskId);
}

static void Task_LoadAreaScreen(u8 taskId)
{
    switch (gMain.state)
    {
    case 0:
    default:
        if (!gPaletteFade.active)
        {
            sPokedexView->currentPage = 5;
            gPokedexVBlankCB = gMain.vblankCallback;
            SetVBlankCallback(NULL);
            ResetOtherVideoRegisters(0x200);
            sPokedexView->selectedScreen = AREA_SCREEN;
            gMain.state = 1;
        }
        break;
    case 1:
        LoadScreenSelectBarSubmenu(0xD);
        HighlightSubmenuScreenSelectBarItem(1, 0xD);
        LoadPokedexBgPalette();
        REG_BG1CNT = BGCNT_PRIORITY(0) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(13) | BGCNT_16COLOR | BGCNT_TXT256x256;
        gMain.state++;
        break;
    case 2:
        ShowPokedexAreaScreen(NationalPokedexNumToSpecies(sPokedexListItem->dexNum), &sPokedexView->screenSwitchState);
        SetVBlankCallback(gPokedexVBlankCB);
        sPokedexView->screenSwitchState = 0;
        gMain.state = 0;
        gTasks[taskId].func = Task_WaitForAreaScreenInput;
        break;
    }
}

static void Task_WaitForAreaScreenInput(u8 taskId)
{
    if (sPokedexView->screenSwitchState != 0)
        gTasks[taskId].func = Task_SwitchScreensFromAreaScreen;
}

static void Task_SwitchScreensFromAreaScreen(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        switch (sPokedexView->screenSwitchState)
        {
        case 1:
        default:
            gTasks[taskId].func = Task_LoadInfoScreen;
            break;
        case 2:
            gTasks[taskId].func = Task_LoadCryScreen;
            break;
        }
    }
}

static void Task_LoadCryScreen(u8 taskId)
{
    switch (gMain.state)
    {
    case 0:
    default:
        if (!gPaletteFade.active)
        {
            m4aMPlayStop(&gMPlayInfo_BGM);
            sPokedexView->currentPage = 6;
            gPokedexVBlankCB = gMain.vblankCallback;
            SetVBlankCallback(NULL);
            ResetOtherVideoRegisters(0x200);
            sPokedexView->selectedScreen = CRY_SCREEN;
            gMain.state = 1;
        }
        break;
    case 1:
        LZ77UnCompVram(gPokedexMenu_Gfx, (void *)VRAM);
        LZ77UnCompVram(gPokedexCryScreen_Tilemap, (void *)(VRAM + 0x7000));
        gMain.state++;
        break;
    case 2:
        LoadScreenSelectBarSubmenu(0xD);
        HighlightSubmenuScreenSelectBarItem(2, 0xD);
        LoadPokedexBgPalette();
        DmaClear16(3, (void *)(VRAM + 0xF800), 0x500);
        gMain.state++;
        break;
    case 3:
        Text_LoadWindowTemplate(&gWindowTemplate_81E702C);
        InitMenuWindow(&gWindowTemplate_81E702C);
        ResetPaletteFade();
        gMain.state++;
        break;
    case 4:
        Menu_PrintText(gText_CryOf, 10, 4);
        PrintCryScreenSpeciesName(sPokedexListItem->dexNum, 10, 6, 2);
        gMain.state++;
        break;
    case 5:
        gTasks[taskId].data[4] =
            CreateMonSpriteFromNationalDexNumber(sPokedexListItem->dexNum, 0x30, 0x38, 0);
        gSprites[gTasks[taskId].data[4]].oam.priority = 0;
        gDexCryScreenState = 0;
        gMain.state++;
        break;
    case 6:
        {
            struct CryScreenWindow waveformWindow;

            waveformWindow.charBase = 0x4020;
            waveformWindow.screenBase = 0x1F;
            waveformWindow.paletteNo = 8;
            waveformWindow.yPos = 0x1E;
            waveformWindow.xPos = 0xC;
            if (LoadCryWaveformWindow(&waveformWindow, 0) != 0)
            {
                gMain.state++;
                gDexCryScreenState = 0;
            }
        }
        break;
    case 7:
        {
            struct CryScreenWindow cryMeter;

            cryMeter.charBase = 0x3000;
            cryMeter.screenBase = 0xE;
            cryMeter.paletteNo = 9;
            cryMeter.xPos = 0x12;
            cryMeter.yPos = 3;
            if (ShowPokedexCryScreen(&cryMeter, 1) != 0)
                gMain.state++;
        }
        break;
    case 8:
        BeginNormalPaletteFade(0xFFFFFFEB, 0, 16, 0, RGB(0, 0, 0));
        SetVBlankCallback(gPokedexVBlankCB);
        gMain.state++;
        break;
    case 9:
        REG_BLDCNT = 0;
        REG_BLDALPHA = 0;
        REG_BLDY = 0;
        REG_BG2CNT = BGCNT_PRIORITY(2) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(14) | BGCNT_16COLOR | BGCNT_TXT256x256;
        REG_BG0CNT = BGCNT_PRIORITY(3) | BGCNT_CHARBASE(1) | BGCNT_SCREENBASE(31) | BGCNT_16COLOR | BGCNT_TXT256x256;
        REG_BG1CNT = BGCNT_PRIORITY(0) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(13) | BGCNT_16COLOR | BGCNT_TXT256x256;
        REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG_ALL_ON | DISPCNT_OBJ_ON;
        gMain.state++;
        break;
    case 10:
        sPokedexView->screenSwitchState = 0;
        gMain.state = 0;
        gTasks[taskId].func = Task_HandleCryScreenInput;
        break;
    }
}

static void Task_HandleCryScreenInput(u8 taskId)
{
    UpdateCryWaveformWindow(0);

    if (IsCryPlaying())
        LoadPlayArrowPalette(1);
    else
        LoadPlayArrowPalette(0);

    if (JOY_NEW(A_BUTTON))
    {
        LoadPlayArrowPalette(1);
        CryScreenPlayButton(NationalPokedexNumToSpecies(sPokedexListItem->dexNum));
        return;
    }
    else if (!gPaletteFade.active)
    {
        if (JOY_NEW(B_BUTTON))
        {
            BeginNormalPaletteFade(0xFFFFFFEB, 0, 0, 16, RGB(0, 0, 0));
            m4aMPlayContinue(&gMPlayInfo_BGM);
            sPokedexView->screenSwitchState = 1;
            gTasks[taskId].func = Task_SwitchScreensFromCryScreen;
            PlaySE(SE_PC_OFF);
            return;
        }
        if (JOY_NEW(DPAD_LEFT)
         || (JOY_NEW(L_BUTTON) && gSaveBlock2.optionsButtonMode == OPTIONS_BUTTON_MODE_LR))
        {
            BeginNormalPaletteFade(0xFFFFFFEB, 0, 0, 16, RGB(0, 0, 0));
            m4aMPlayContinue(&gMPlayInfo_BGM);
            sPokedexView->screenSwitchState = 2;
            gTasks[taskId].func = Task_SwitchScreensFromCryScreen;
            PlaySE(SE_DEX_PAGE);
            return;
        }
        if (JOY_NEW(DPAD_RIGHT)
         || (JOY_NEW(R_BUTTON) && gSaveBlock2.optionsButtonMode == OPTIONS_BUTTON_MODE_LR))
        {
            if (!sPokedexListItem->owned)
            {
                PlaySE(SE_FAILURE);
            }
            else
            {
                BeginNormalPaletteFade(0xFFFFFFEB, 0, 0, 16, RGB(0, 0, 0));
                m4aMPlayContinue(&gMPlayInfo_BGM);
                sPokedexView->screenSwitchState = 3;
                gTasks[taskId].func = Task_SwitchScreensFromCryScreen;
                PlaySE(SE_DEX_PAGE);
            }
            return;
        }
    }
}

static void Task_SwitchScreensFromCryScreen(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        DestroyCryMeterNeedleSprite();
        switch (sPokedexView->screenSwitchState)
        {
        default:
        case 1:
            gTasks[taskId].func = Task_LoadInfoScreen;
            break;
        case 2:
            gTasks[taskId].func = Task_LoadAreaScreen;
            break;
        case 3:
            gTasks[taskId].func = Task_LoadSizeScreen;
            break;
        }
    }
}

static void LoadPlayArrowPalette(u8 cryPlaying)
{
    u16 color;

    if (cryPlaying)
        color = RGB(18, 28, 0);
    else
        color = RGB(15, 21, 0);
    LoadPalette(&color, 0x5D, 2);
}

static void Task_LoadSizeScreen(u8 taskId)
{
    u8 spriteId;

    switch (gMain.state)
    {
    default:
    case 0:
        if (!gPaletteFade.active)
        {
            sPokedexView->currentPage = 7;
            gPokedexVBlankCB = gMain.vblankCallback;
            SetVBlankCallback(NULL);
            ResetOtherVideoRegisters(0x200);
            sPokedexView->selectedScreen = SIZE_SCREEN;
            gMain.state = 1;
        }
        break;
    case 1:
        LZ77UnCompVram(gPokedexMenu_Gfx, (void *)VRAM);
        LZ77UnCompVram(gPokedexSizeScreen_Tilemap, (void *)(VRAM + 0x7000));
        gMain.state++;
        break;
    case 2:
        LoadScreenSelectBarSubmenu(0xD);
        HighlightSubmenuScreenSelectBarItem(3, 0xD);
        LoadPokedexBgPalette();
        gMain.state++;
        break;
    case 3:
        {
            // This only needs to be 25 chars long (31 in German).
            u8 string[40];  //I hope this is the correct size

            Text_LoadWindowTemplate(&gWindowTemplate_81E702C);
            InitMenuWindow(&gWindowTemplate_81E702C);
            string[0] = EOS;
            StringAppend(string, gText_SizeComparedTo);
            StringAppend(string, gSaveBlock2.playerName);
            MenuPrint_Centered(string, 3, 15, 0xC0);
            gMain.state++;
        }
        break;
    case 4:
        ResetPaletteFade();
        gMain.state++;
        break;
    case 5:
        spriteId = CreateSizeScreenTrainerPic(gSaveBlock2.playerGender, 152, 56, 0);
        gSprites[spriteId].oam.affineMode = 1;
        gSprites[spriteId].oam.matrixNum = 1;
        gSprites[spriteId].oam.priority = 0;
        gSprites[spriteId].y2 = gPokedexEntries[sPokedexListItem->dexNum].trainerOffset;
        SetOamMatrix(1, gPokedexEntries[sPokedexListItem->dexNum].trainerScale, 0, 0, gPokedexEntries[sPokedexListItem->dexNum].trainerScale);
        LoadPalette(sSizeScreenSilhouette_Pal, (gSprites[spriteId].oam.paletteNum + 16) * 16, sizeof(sSizeScreenSilhouette_Pal));
        gMain.state++;
        break;
    case 6:
        spriteId = CreateMonSpriteFromNationalDexNumber(sPokedexListItem->dexNum, 88, 56, 1);
        gSprites[spriteId].oam.affineMode = 1;
        gSprites[spriteId].oam.matrixNum = 2;
        gSprites[spriteId].oam.priority = 0;
        gSprites[spriteId].y2 = gPokedexEntries[sPokedexListItem->dexNum].pokemonOffset;
        SetOamMatrix(2, gPokedexEntries[sPokedexListItem->dexNum].pokemonScale, 0, 0, gPokedexEntries[sPokedexListItem->dexNum].pokemonScale);
        LoadPalette(sSizeScreenSilhouette_Pal, (gSprites[spriteId].oam.paletteNum + 16) * 16, sizeof(sSizeScreenSilhouette_Pal));
        gMain.state++;
        break;
    case 7:
        BeginNormalPaletteFade(0xFFFFFFEB, 0, 16, 0, RGB(0, 0, 0));
        SetVBlankCallback(gPokedexVBlankCB);
        gMain.state++;
        break;
    case 8:
        REG_BLDCNT = 0;
        REG_BLDALPHA = 0;
        REG_BLDY = 0;
        REG_BG2CNT = BGCNT_PRIORITY(3) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(14) | BGCNT_16COLOR | BGCNT_TXT256x256;
        REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG1_ON | DISPCNT_BG2_ON | DISPCNT_BG3_ON | DISPCNT_OBJ_ON;
        gMain.state++;
        break;
    case 9:
        if (!gPaletteFade.active)
        {
            sPokedexView->screenSwitchState = 0;
            gMain.state = 0;
            gTasks[taskId].func = Task_HandleSizeScreenInput;
        }
        break;
    }
}

static void Task_HandleSizeScreenInput(u8 taskId)
{
    if (JOY_NEW(B_BUTTON))
    {
        BeginNormalPaletteFade(0xFFFFFFEB, 0, 0, 16, RGB(0, 0, 0));
        sPokedexView->screenSwitchState = 1;
        gTasks[taskId].func = Task_SwitchScreensFromSizeScreen;
        PlaySE(SE_PC_OFF);
    }
    else if (JOY_NEW(DPAD_LEFT)
     || (JOY_NEW(L_BUTTON) && gSaveBlock2.optionsButtonMode == OPTIONS_BUTTON_MODE_LR))
    {
        BeginNormalPaletteFade(0xFFFFFFEB, 0, 0, 16, RGB(0, 0, 0));
        sPokedexView->screenSwitchState = 2;
        gTasks[taskId].func = Task_SwitchScreensFromSizeScreen;
        PlaySE(SE_DEX_PAGE);
    }
}

static void Task_SwitchScreensFromSizeScreen(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        switch (sPokedexView->screenSwitchState)
        {
        default:
        case 1:
            gTasks[taskId].func = Task_LoadInfoScreen;
            break;
        case 2:
            gTasks[taskId].func = Task_LoadCryScreen;
            break;
        }
    }
}

static void LoadScreenSelectBarMain(u16 screenBase)
{
    LZ77UnCompVram(gPokedexScreenSelectBarMain_Tilemap, (void *)(VRAM + screenBase * 0x800));
    DmaClear16(3, (void *)(VRAM + screenBase * 0x800 + 0xC0), 0x440);
}

static void LoadScreenSelectBarSubmenu(u16 screenBase)
{
    LZ77UnCompVram(gPokedexScreenSelectBarSubmenu_Tilemap, (void *)(VRAM + screenBase * 0x800));
    DmaClear16(3, (void *)(VRAM + screenBase * 0x800 + 0xC0), 0x440);
}

#ifdef NONMATCHING
static void HighlightScreenSelectBarItem(u8 selectedScreen, u16 b)
{
    u8 i;   //r1
    u8 j;   //r3
    u32 r6;
    register u8 r7;

    for (i = 0; i < 4; i++)
    {
        r7 = i * 5 + 1;
        r6 = 0x4000;

        if (i == selectedScreen)
            r6 = 0x2000;

        for (j = 0; j < 5; j++)
        {
            u32 r0 = b * 0x800 + (r7 + j) * 2;
            u8 *ptr;

            ptr = (void *)VRAM;
            *(u16 *)(ptr + r0) = (*(u16 *)(ptr + r0) & 0xFFF) | r6;
            ptr = (void *)VRAM + 0x40;
            *(u16 *)(ptr + r0) = (*(u16 *)(ptr + r0) & 0xFFF) | r6;
        }
    }
    r6 = 0x4000;
    for (j = 0; j < 5; j++)
    {
        u32 r0 = b * 0x800 + j * 2;
        u8 *ptr;

        ptr = (void *)VRAM + 0x32;
        *(u16 *)(ptr + r0) = (*(u16 *)(ptr + r0) & 0xFFF) | r6;
        ptr = (void *)VRAM + 0x72;
        *(u16 *)(ptr + r0) = (*(u16 *)(ptr + r0) & 0xFFF) | r6;
    }
}
#else
NAKED
static void HighlightScreenSelectBarItem(u8 selectedScreen, u16 b)
{
    asm(".syntax unified\n\
    push {r4-r7,lr}\n\
    mov r7, r10\n\
    mov r6, r9\n\
    mov r5, r8\n\
    push {r5-r7}\n\
    lsls r0, 24\n\
    lsrs r0, 24\n\
    mov r10, r0\n\
    lsls r1, 16\n\
    lsrs r1, 16\n\
    mov r9, r1\n\
    movs r1, 0\n\
_0809059C:\n\
    lsls r0, r1, 2\n\
    adds r0, r1\n\
    adds r0, 0x1\n\
    lsls r0, 24\n\
    lsrs r7, r0, 24\n\
    movs r6, 0x80\n\
    lsls r6, 7\n\
    cmp r1, r10\n\
    bne _080905B2\n\
    movs r6, 0x80\n\
    lsls r6, 6\n\
_080905B2:\n\
    movs r3, 0\n\
    mov r0, r9\n\
    lsls r0, 11\n\
    mov r12, r0\n\
    adds r1, 0x1\n\
    mov r8, r1\n\
    mov r5, r12\n\
    ldr r4, _08090634 @ =0x00000fff\n\
_080905C2:\n\
    adds r0, r7, r3\n\
    lsls r0, 1\n\
    adds r0, r5, r0\n\
    movs r2, 0xC0\n\
    lsls r2, 19\n\
    adds r1, r0, r2\n\
    ldrh r2, [r1]\n\
    ands r2, r4\n\
    orrs r2, r6\n\
    strh r2, [r1]\n\
    ldr r1, _08090638 @ =0x06000040\n\
    adds r0, r1\n\
    ldrh r2, [r0]\n\
    ands r2, r4\n\
    orrs r2, r6\n\
    strh r2, [r0]\n\
    adds r0, r3, 0x1\n\
    lsls r0, 24\n\
    lsrs r3, r0, 24\n\
    cmp r3, 0x4\n\
    bls _080905C2\n\
    mov r2, r8\n\
    lsls r0, r2, 24\n\
    lsrs r1, r0, 24\n\
    cmp r1, 0x3\n\
    bls _0809059C\n\
    movs r6, 0x80\n\
    lsls r6, 7\n\
    movs r3, 0\n\
    mov r5, r12\n\
    ldr r4, _08090634 @ =0x00000fff\n\
_08090600:\n\
    lsls r0, r3, 1\n\
    adds r0, r5, r0\n\
    ldr r2, _0809063C @ =0x06000032\n\
    adds r1, r0, r2\n\
    ldrh r2, [r1]\n\
    ands r2, r4\n\
    orrs r2, r6\n\
    strh r2, [r1]\n\
    ldr r1, _08090640 @ =0x06000072\n\
    adds r0, r1\n\
    ldrh r2, [r0]\n\
    ands r2, r4\n\
    orrs r2, r6\n\
    strh r2, [r0]\n\
    adds r0, r3, 0x1\n\
    lsls r0, 24\n\
    lsrs r3, r0, 24\n\
    cmp r3, 0x4\n\
    bls _08090600\n\
    pop {r3-r5}\n\
    mov r8, r3\n\
    mov r9, r4\n\
    mov r10, r5\n\
    pop {r4-r7}\n\
    pop {r0}\n\
    bx r0\n\
    .align 2, 0\n\
_08090634: .4byte 0x00000fff\n\
_08090638: .4byte 0x06000040\n\
_0809063C: .4byte 0x06000032\n\
_08090640: .4byte 0x06000072\n\
    .syntax divided\n");
}
#endif

//Nope, can't get this one to match, either.
#ifdef NONMATCHING
static void HighlightSubmenuScreenSelectBarItem(u8 a, u16 b)
{
    u8 i;
    u8 j;

    for (i = 0; i < 4; i++)
    {
        u8 r8 = i * 5 + 1;
        u32 r5;

        if (i == a || i == 0)
            r5 = 0x2000;
        else if (a != 0)
            r5 = 0x4000;

        for (j = 0; j < 5; j++)
        {
            u16 (*vramData)[0x400];

            vramData = (u16 (*)[])VRAM;
            vramData[b][r8 + j] = (vramData[b][r8 + j] & 0xFFF) | r5;
            vramData = (u16 (*)[])(VRAM + 0x40);
            vramData[b][r8 + j] = (vramData[b][r8 + j] & 0xFFF) | r5;
        }
    }

    for (j = 0; j < 5; j++)
    {
        u16 (*vramData)[0x400];

        vramData = (u16 (*)[])(VRAM + 0x32);
        vramData[b][j] = (vramData[b][j] & 0xFFF) | 0x4000;
        vramData = (u16 (*)[])(VRAM + 0x72);
        vramData[b][j] = (vramData[b][j] & 0xFFF) | 0x4000;
    }
}
#else
NAKED
static void HighlightSubmenuScreenSelectBarItem(u8 a, u16 b)
{
    asm(".syntax unified\n\
    push {r4-r7,lr}\n\
    mov r7, r10\n\
    mov r6, r9\n\
    mov r5, r8\n\
    push {r5-r7}\n\
    lsls r0, 24\n\
    lsrs r0, 24\n\
    mov r10, r0\n\
    lsls r1, 16\n\
    lsrs r1, 16\n\
    mov r9, r1\n\
    movs r1, 0\n\
_0809065C:\n\
    lsls r0, r1, 2\n\
    adds r0, r1\n\
    adds r0, 0x1\n\
    lsls r0, 24\n\
    lsrs r0, 24\n\
    mov r8, r0\n\
    cmp r1, r10\n\
    beq _08090670\n\
    cmp r1, 0\n\
    bne _08090676\n\
_08090670:\n\
    movs r5, 0x80\n\
    lsls r5, 6\n\
    b _0809067A\n\
_08090676:\n\
    movs r5, 0x80\n\
    lsls r5, 7\n\
_0809067A:\n\
    movs r3, 0\n\
    mov r0, r9\n\
    lsls r7, r0, 11\n\
    adds r1, 0x1\n\
    mov r12, r1\n\
    adds r6, r7, 0\n\
    ldr r4, _080906FC @ =0x00000fff\n\
_08090688:\n\
    mov r1, r8\n\
    adds r0, r1, r3\n\
    lsls r0, 1\n\
    adds r0, r6, r0\n\
    movs r2, 0xC0\n\
    lsls r2, 19\n\
    adds r1, r0, r2\n\
    ldrh r2, [r1]\n\
    ands r2, r4\n\
    orrs r2, r5\n\
    strh r2, [r1]\n\
    ldr r1, _08090700 @ =0x06000040\n\
    adds r0, r1\n\
    ldrh r2, [r0]\n\
    ands r2, r4\n\
    orrs r2, r5\n\
    strh r2, [r0]\n\
    adds r0, r3, 0x1\n\
    lsls r0, 24\n\
    lsrs r3, r0, 24\n\
    cmp r3, 0x4\n\
    bls _08090688\n\
    mov r2, r12\n\
    lsls r0, r2, 24\n\
    lsrs r1, r0, 24\n\
    cmp r1, 0x3\n\
    bls _0809065C\n\
    movs r5, 0x80\n\
    lsls r5, 7\n\
    movs r3, 0\n\
    adds r6, r7, 0\n\
    ldr r4, _080906FC @ =0x00000fff\n\
_080906C8:\n\
    lsls r0, r3, 1\n\
    adds r0, r6, r0\n\
    ldr r2, _08090704 @ =0x06000032\n\
    adds r1, r0, r2\n\
    ldrh r2, [r1]\n\
    ands r2, r4\n\
    orrs r2, r5\n\
    strh r2, [r1]\n\
    ldr r1, _08090708 @ =0x06000072\n\
    adds r0, r1\n\
    ldrh r2, [r0]\n\
    ands r2, r4\n\
    orrs r2, r5\n\
    strh r2, [r0]\n\
    adds r0, r3, 0x1\n\
    lsls r0, 24\n\
    lsrs r3, r0, 24\n\
    cmp r3, 0x4\n\
    bls _080906C8\n\
    pop {r3-r5}\n\
    mov r8, r3\n\
    mov r9, r4\n\
    mov r10, r5\n\
    pop {r4-r7}\n\
    pop {r0}\n\
    bx r0\n\
    .align 2, 0\n\
_080906FC: .4byte 0x00000fff\n\
_08090700: .4byte 0x06000040\n\
_08090704: .4byte 0x06000032\n\
_08090708: .4byte 0x06000072\n\
    .syntax divided\n");
}
#endif

u8 DisplayCaughtMonDexPage(u16 dexNum, u32 otId, u32 personality)
{
    u8 taskId = CreateTask(Task_DisplayCaughtMonDexPage, 0);

    gTasks[taskId].data[0] = 0;
    gTasks[taskId].data[1] = dexNum;
    gTasks[taskId].data[12] = otId;
    gTasks[taskId].data[13] = otId >> 16;
    gTasks[taskId].data[14] = personality;
    gTasks[taskId].data[15] = personality >> 16;
    return taskId;
}

static void Task_DisplayCaughtMonDexPage(u8 taskId)
{
    u8 spriteId;
    u16 dexNum = gTasks[taskId].data[1];
    u16 i;

    switch (gTasks[taskId].data[0])
    {
    default:
    case 0:
        if (!gPaletteFade.active)
        {
            gPokedexVBlankCB = gMain.vblankCallback;
            SetVBlankCallback(NULL);
            ResetOtherVideoRegisters(0x100);
            gTasks[taskId].data[0] = 1;
        }
        break;
    case 1:
        LZ77UnCompVram(gPokedexMenu_Gfx, (void *)(VRAM + 0x4000));
        LZ77UnCompVram(gPokedexInfoScreen_Tilemap, (void *)(VRAM + 0x7800));
        for (i = 0; i < 0x280; i++)
        {
            u16 temp = *(u16 *)(BG_VRAM + 0x7800 + 2 * i);
            temp += 0x2000;
            *(u16 *)(BG_VRAM + 0x7800 + 2 * i) = temp;
        }
        DrawFootprint(gTasks[taskId].data[1], 2, 0x3FC);
        ResetPaletteFade();
        LoadPalette(gPokedexBgHoenn_Pal + 1, 0x21, 0x9E);
        gTasks[taskId].data[0]++;
        break;
    case 2:
        Text_LoadWindowTemplate(&gWindowTemplate_81E7064);
        InitMenuWindow(&gWindowTemplate_81E7064);
        DmaClear16(3, (void *)(VRAM + 0xC000), 0x200);
        gTasks[taskId].data[0]++;
        break;
    case 3:
        MenuPrint_Centered(gText_PokedexRegistration, 2, 0, 0xD0);
        if (!IsNationalPokedexEnabled())
            PrintEntryScreenDexNum(NationalToHoennOrder(dexNum), 13, 3);
        else
            PrintEntryScreenDexNum(dexNum, 13, 3);
        PrintEntryScreenSpeciesName(dexNum, 16, 3);
        Menu_PrintText(gText_5MarksPokemon, CATEGORY_LEFT, 5);
        Menu_PrintText(gText_UnkHeight, 16, 7);
        Menu_PrintText(gText_UnkWeight, 16, 9);
        UnusedPrintMonName(gPokedexEntries[dexNum].categoryName, CATEGORY_LEFT, 5);
        PrintMonHeight(gPokedexEntries[dexNum].height, 16, 7);
        PrintMonWeight(gPokedexEntries[dexNum].weight, 16, 9);
        Menu_PrintText(gPokedexEntries[dexNum].descriptionPage1, 2, 13);
        RS_DrawFootprint(14, 0x3FC);
        gTasks[taskId].data[0]++;
        break;
    case 4:
        spriteId = CreateMonSpriteFromNationalDexNumber(dexNum, 0x30, 0x38, 0);
        gSprites[spriteId].oam.priority = 0;
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        SetVBlankCallback(gPokedexVBlankCB);
        gTasks[taskId].data[3] = spriteId;
        gTasks[taskId].data[0]++;
        break;
    case 5:
        REG_BLDCNT = 0;
        REG_BLDALPHA = 0;
        REG_BLDY = 0;
        REG_BG3CNT = BGCNT_PRIORITY(3) | BGCNT_CHARBASE(1) | BGCNT_SCREENBASE(15) | BGCNT_16COLOR | BGCNT_TXT256x256;
        REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG2_ON | DISPCNT_BG3_ON | DISPCNT_OBJ_ON;
        gTasks[taskId].data[0]++;
        break;
    case 6:
        if (!gPaletteFade.active)
        {
            PlayCry_Normal(NationalPokedexNumToSpecies(dexNum), 0);
            gTasks[taskId].data[2] = 0;
            gTasks[taskId].data[4] = 0;
            gTasks[taskId].func = Task_HandleCaughtMonPageInput;
        }
        break;
    }
}

static void Task_HandleCaughtMonPageInput(u8 taskId)
{
    if (JOY_NEW(B_BUTTON))
    {
        BeginNormalPaletteFade(0xFFFC, 0, 0, 16, RGB(0, 0, 0));
        gSprites[gTasks[taskId].data[3]].callback = SpriteCB_SlideCaughtMonToCenter;
        gTasks[taskId].func = Task_ExitCaughtMonPage;
        return;
    }
    else if (JOY_NEW(A_BUTTON))
    {
        if (gTasks[taskId].data[4] == 0)
        {
            u16 r4 = gTasks[taskId].data[1];

            Menu_EraseWindowRect(2, 13, 27, 19);
            Menu_PrintText(gPokedexEntries[r4].descriptionPage2, 2, 13);
            (*(u16 *)(BG_VRAM + 0x7ACA))++;
            (*(u16 *)(BG_VRAM + 0x7B0A))++;
            gTasks[taskId].data[4] = 1;
            PlaySE(SE_PIN);
        }
        else
        {
            BeginNormalPaletteFade(0xFFFC, 0, 0, 16, RGB(0, 0, 0));
            gSprites[gTasks[taskId].data[3]].callback = SpriteCB_SlideCaughtMonToCenter;
            gTasks[taskId].func = Task_ExitCaughtMonPage;
            return;
        }
    }
    gTasks[taskId].data[2]++;
    if (gTasks[taskId].data[2] & 0x10)
        LoadPalette(gPokedexBgHoenn_Pal + 1, 0x51, 14);
    else
        LoadPalette(gPokedexMenu2_Pal + 1, 0x51, 14);
}

static void Task_ExitCaughtMonPage(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        u16 species;
        u32 otId;
        u32 personality;
        u8 paletteNum;
        const u8 *lzPaletteData;

        REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG3_ON | DISPCNT_OBJ_ON;
        CpuCopy16(gBattleTextboxTilemap, (void *)(VRAM + 0xC000), 0x1000);
        ApplyPlayerChosenFrameToBattleMenu();
        species = NationalPokedexNumToSpecies(gTasks[taskId].data[1]);
        otId = ((u16)gTasks[taskId].data[13] << 16) | (u16)gTasks[taskId].data[12];
        personality = ((u16)gTasks[taskId].data[15] << 16) | (u16)gTasks[taskId].data[14];
        paletteNum = gSprites[gTasks[taskId].data[3]].oam.paletteNum;
        lzPaletteData = GetMonSpritePalFromSpeciesAndPersonality(species, otId, personality);
        LoadCompressedPalette(lzPaletteData, 0x100 | paletteNum * 16, 32);
        DestroyTask(taskId);
    }
}

static void SpriteCB_SlideCaughtMonToCenter(struct Sprite *sprite)
{
    if (sprite->x < 0x78)
        sprite->x += 2;
    if (sprite->x > 0x78)
        sprite->x -= 2;

    if (sprite->y < 0x50)
        sprite->y += 1;
    if (sprite->y > 0x50)
        sprite->y -= 1;
}

static void sub_8090C68(void)
{
    if (sPokedexListItem->owned)
    {
        if (sPokedexView->descriptionPageNum == 0)
        {
            Menu_EraseWindowRect(2, 13, 27, 19);
            Menu_PrintText(gPokedexEntries[sPokedexListItem->dexNum].descriptionPage2, 2, 13);
            sPokedexView->descriptionPageNum = 1;
            (*(u16 *)(BG_VRAM + 0x7ACA))++;
            (*(u16 *)(BG_VRAM + 0x7B0A))++;
            PlaySE(SE_PIN);
        }
        else
        {
            Menu_EraseWindowRect(2, 13, 27, 19);
            Menu_PrintText(gPokedexEntries[sPokedexListItem->dexNum].descriptionPage1, 2, 13);
            sPokedexView->descriptionPageNum = 0;
            (*(u16 *)(BG_VRAM + 0x7ACA))--;
            (*(u16 *)(BG_VRAM + 0x7B0A))--;
            PlaySE(SE_PIN);
        }
    }
}

const u8 *GetPokedexCategoryName(u16 dexNum)
{
    return gPokedexEntries[dexNum].categoryName;
}

u16 GetPokedexHeightWeight(u16 dexNum, u8 data)
{
    switch (data)
    {
    case 0:  // height
        return gPokedexEntries[dexNum].height;
    case 1:  // weight
        return gPokedexEntries[dexNum].weight;
    default:
        return 1;
    }
}

s8 GetSetPokedexFlag(u16 nationalDexNo, u8 caseID)
{
    u8 index;
    u8 bit;
    u8 mask;
    s8 retVal;

    nationalDexNo--;
    index = nationalDexNo / 8;
    bit = nationalDexNo % 8;
    mask = 1 << bit;
    retVal = 0;
    switch (caseID)
    {
    case FLAG_GET_SEEN:
        if (gSaveBlock2.pokedex.seen[index] & mask)
        {
            if ((gSaveBlock2.pokedex.seen[index] & mask) == (gSaveBlock1.dexSeen2[index] & mask)
             && (gSaveBlock2.pokedex.seen[index] & mask) == (gSaveBlock1.dexSeen3[index] & mask))
                retVal = 1;
            else
            {
                gSaveBlock2.pokedex.seen[index] &= ~mask;
                gSaveBlock1.dexSeen2[index] &= ~mask;
                gSaveBlock1.dexSeen3[index] &= ~mask;
                retVal = 0;
            }
        }
        break;
    case FLAG_GET_CAUGHT:
        if (gSaveBlock2.pokedex.owned[index] & mask)
        {
            if ((gSaveBlock2.pokedex.owned[index] & mask) == (gSaveBlock2.pokedex.seen[index] & mask)
             && (gSaveBlock2.pokedex.owned[index] & mask) == (gSaveBlock1.dexSeen2[index] & mask)
             && (gSaveBlock2.pokedex.owned[index] & mask) == (gSaveBlock1.dexSeen3[index] & mask))
                retVal = 1;
            else
            {
                gSaveBlock2.pokedex.owned[index] &= ~mask;
                gSaveBlock2.pokedex.seen[index] &= ~mask;
                gSaveBlock1.dexSeen2[index] &= ~mask;
                gSaveBlock1.dexSeen3[index] &= ~mask;
                retVal = 0;
            }
        }
        break;
    case FLAG_SET_SEEN:
        gSaveBlock2.pokedex.seen[index] |= mask;
        gSaveBlock1.dexSeen2[index] |= mask;
        gSaveBlock1.dexSeen3[index] |= mask;
        break;
    case FLAG_SET_CAUGHT:
        gSaveBlock2.pokedex.owned[index] |= mask;
        break;
    }
    return retVal;
}

u16 GetNationalPokedexCount(u8 caseID)
{
    u16 count = 0;
    u16 i;

    for (i = 0; i < NATIONAL_DEX_COUNT; i++)
    {
        switch (caseID)
        {
        case FLAG_GET_SEEN:
            if (GetSetPokedexFlag(i + 1, FLAG_GET_SEEN))
                count++;
            break;
        case FLAG_GET_CAUGHT:
            if (GetSetPokedexFlag(i + 1, FLAG_GET_CAUGHT))
                count++;
            break;
        }
    }
    return count;
}

u16 GetHoennPokedexCount(u8 caseID)
{
    u16 count = 0;
    u16 i;

    for (i = 0; i < HOENN_DEX_COUNT; i++)
    {
        switch (caseID)
        {
        case FLAG_GET_SEEN:
            if (GetSetPokedexFlag(HoennToNationalOrder(i + 1), FLAG_GET_SEEN))
                count++;
            break;
        case FLAG_GET_CAUGHT:
            if (GetSetPokedexFlag(HoennToNationalOrder(i + 1), FLAG_GET_CAUGHT))
                count++;
            break;
        }
    }
    return count;
}

bool8 HasAllHoennMons(void)
{
    u16 i;

    for (i = 0; i < 200; i++)
    {
        if (!GetSetPokedexFlag(HoennToNationalOrder(i + 1), FLAG_GET_CAUGHT))
            return FALSE;
    }
    return TRUE;
}

bool16 HasAllMons(void)
{
    u16 i;

    // BUG: This function indexes pokemon checks by 0, but adds
    // 1 before passing to GetSetPokedexFlag. Normally, this is
    // fine, because GetSetPokedexFlag subtracts by 1 to get the
    // array index value, but since the array is 0 indexed
    // starting with Bulbasaur, values passed actually means that
    // dex entries 152 (Chikorita) and 252 (Treecko) are skipped.
    // Because an earlier Hoenn Dex check prevented Treecko from
    // being skippable, it means that Chikorita is not required
    // to obtain the National Diploma. This was fixed in Emerald.
    for (i = 0; i < 150; i++)
    {
        if (GetSetPokedexFlag(i + 1, 1) == 0)
            return FALSE;
    }
    for (i = 152; i < 250; i++)
    {
        if (GetSetPokedexFlag(i + 1, 1) == 0)
            return FALSE;
    }
    for (i = 252; i < 384; i++)
    {
        if (GetSetPokedexFlag(i + 1, 1) == 0)
            return FALSE;
    }
    return TRUE;
}

static void ResetOtherVideoRegisters(u16 regBits)
{
    if (!(regBits & 0x100))
    {
        REG_DISPCNT &= 0xFEFF;
        REG_BG0CNT = 0;
        REG_BG0HOFS = 0;
        REG_BG0VOFS = 0;
    }
    if (!(regBits & 0x200))
    {
        REG_DISPCNT &= 0xFDFF;
        REG_BG1CNT = 0;
        REG_BG1HOFS = 0;
        REG_BG1VOFS = 0;
    }
    if (!(regBits & 0x400))
    {
        REG_DISPCNT &= 0xFBFF;
        REG_BG2CNT = 0;
        REG_BG2HOFS = 0;
        REG_BG2VOFS = 0;
    }
    if (!(regBits & 0x800))
    {
        REG_DISPCNT &= 0xF7FF;
        REG_BG3CNT = 0;
        REG_BG3HOFS = 0;
        REG_BG3VOFS = 0;
    }
    if (!(regBits & 0x1000))
    {
        REG_DISPCNT &= 0xEFFF;
        ResetSpriteData();
        FreeAllSpritePalettes();
        gReservedSpritePaletteCount = 8;
    }
}

static void PrintEntryScreenDexNum(u16 order, u8 left, u8 top)
{
    u8 str[4];

    str[0] = CHAR_0 + order / 100;
    str[1] = CHAR_0 + (order % 100) / 10;
    str[2] = CHAR_0 + (order % 100) % 10;
    str[3] = EOS;
    Menu_PrintText(str, left, top);
}

static u8 PrintEntryScreenSpeciesName(u16 num, u8 left, u8 top)
{
    u8 str[11];
    u8 i;

    for (i = 0; i < 11; i++)
        str[i] = EOS;
    num = NationalPokedexNumToSpecies(num);
    if (num)
    {
        for (i = 0; gSpeciesNames[num][i] != EOS && i < 10; i++)
            str[i] = gSpeciesNames[num][i];
    }
    else
    {
        for (i = 0; i < 10; i++)
            str[i] = 0xAE;
    }
    Menu_PrintText(str, left, top);
    return i;
}

static u8 PrintCryScreenSpeciesName(u16 num, u8 left, u8 top, u8 d)
{
    u8 str[40];
    u8 *end;
    u8 i;

    end = StringCopy(str, sText_ClearTo0);
    str[2] = d;
    num = NationalPokedexNumToSpecies(num);
    switch (num)
    {
    default:
        for (i = 0; gSpeciesNames[num][i] != EOS && i < 10; i++)
            end[i] = gSpeciesNames[num][i];
        break;
    case 0:
        for (i = 0; i < 10; i++)
            end[i] = 0xAE;
        break;
    }
    end[i] = EOS;
    Menu_PrintText(str, left, top);
    return i;
}

static void UnusedPrintMonName(const u8 *name, u8 left, u8 top)
{
    u8 str[32];
    u8 i;
#if ENGLISH
    u8 j;
#endif

    for (i = 0; name[i] != EOS && i < 11; i++)
        str[i] = name[i];
#if ENGLISH
    for (j = 0; gText_5MarksPokemon[j] == CHAR_QUESTION_MARK || gText_5MarksPokemon[j] == CHAR_SPACE; j++)
        ;
    j--;
    while (gText_5MarksPokemon[j] != EOS)
        str[i++] = gText_5MarksPokemon[j++];
#endif
    str[i] = EOS;
    MenuPrint_AlignedToRightOfReferenceString(str, left, top, gText_5MarksPokemon);
}

#if ENGLISH
static void UNUSED PrintDecimalNum(u16 num, u8 left, u8 top)
{
    u8 str[6];
    bool8 outputted = FALSE;
    u8 result;

    result = num / 1000;
    if (result == 0)
    {
        str[0] = CHAR_SPACE;
        outputted = FALSE;
    }
    else
    {
        str[0] = CHAR_0 + result;
        outputted = TRUE;
    }

    result = (num % 1000) / 100;
    if (result == 0 && !outputted)
    {
        str[1] = CHAR_SPACE;
        outputted = FALSE;
    }
    else
    {
        str[1] = CHAR_0 + result;
        outputted = TRUE;
    }

    str[2] = CHAR_0 + ((num % 1000) % 100) / 10;
    str[3] = CHAR_PERIOD;
    str[4] = CHAR_0 + ((num % 1000) % 100) % 10;
    str[5] = EOS;
    Menu_PrintText(str, left, top);
}
#elif GERMAN
static void UNUSED PrintDecimalNum(u16 num, u8 left, u8 top) {
    u8 buffer[8];
    int offset;
    u8 result;

    u8 r6 = 0;
    offset = 0;


    buffer[r6++] = 0xFC;
    buffer[r6++] = 0x13;
    r6++;

    result = (num / 1000);
    if (result == 0)
    {
        offset = 6;
    }
    else
    {
        buffer[r6++] = result + CHAR_0;
    }


    result = (num % 1000) / 100;

    if (result == 0 && offset != 0)
    {
        offset += 6;
    }
    else
    {
        buffer[r6++] = result + CHAR_0;
    }

    buffer[r6++] = (((num % 1000) % 100) / 10) + CHAR_0;
    buffer[r6++] = CHAR_COMMA;
    buffer[r6++] = (((num % 1000) % 100) % 10) + CHAR_0;

    buffer[r6++] = EOS;
    buffer[2] = offset;
    Menu_PrintText(buffer, left, top);
}
#endif

#ifdef UNITS_IMPERIAL
#define CHAR_PRIME (0xB4)
#define CHAR_DOUBLE_PRIME (0xB2)
static void PrintMonHeight(u16 height, u8 left, u8 top)
{
    u8 buffer[16];
    u32 inches, feet;
    u8 i = 0;

    inches = (height * 10000) / 254;
    if (inches % 10 >= 5)
        inches += 10;
    feet = inches / 120;
    inches = (inches - (feet * 120)) / 10;

    buffer[i++] = EXT_CTRL_CODE_BEGIN;
    buffer[i++] = 0x13;
    if (feet / 10 == 0)
    {
        buffer[i++] = 18;
        buffer[i++] = feet + CHAR_0;
    }
    else
    {
        buffer[i++] = 12;
        buffer[i++] = feet / 10 + CHAR_0;
        buffer[i++] = (feet % 10) + CHAR_0;
    }
    buffer[i++] = CHAR_PRIME;
    buffer[i++] = (inches / 10) + CHAR_0;
    buffer[i++] = (inches % 10) + CHAR_0;
    buffer[i++] = CHAR_DOUBLE_PRIME;
    buffer[i++] = EOS;
    Menu_PrintText(buffer, left, top);
}
#else
static void PrintMonHeight(u16 height, u8 left, u8 top)
{
    PrintDecimalNum(height, left, top);
}
#endif

#ifdef UNITS_IMPERIAL
static void PrintMonWeight(u16 weight, u8 left, u8 top)
{
    u8 buffer[16];
    u32 lbs;
    u8 i = 0;
    bool8 output;

    lbs = (weight * 100000) / 4536;
    if (lbs % 10 >= 5)
        lbs += 10;
    output = FALSE;

    buffer[i] = (lbs / 100000) + CHAR_0;
    if (buffer[i] == CHAR_0 && output == FALSE)
    {
        buffer[i++] = CHAR_SPACE;
        buffer[i++] = CHAR_SPACE;
    }
    else
    {
        output = TRUE;
        i++;
    }

    lbs = (lbs % 100000);
    buffer[i] = (lbs / 10000) + CHAR_0;
    if (buffer[i] == CHAR_0 && output == FALSE)
    {
        buffer[i++] = CHAR_SPACE;
        buffer[i++] = CHAR_SPACE;
    }
    else
    {
        output = TRUE;
        i++;
    }

    lbs = (lbs % 10000);
    buffer[i] = (lbs / 1000) + CHAR_0;
    if (buffer[i] == CHAR_0 && output == FALSE)
    {
        buffer[i++] = CHAR_SPACE;
        buffer[i++] = CHAR_SPACE;
    }
    else
    {
        output = TRUE;
        i++;
    }
    lbs = (lbs % 1000);
    buffer[i++] = (lbs / 100) + CHAR_0;
    lbs = (lbs % 100);
    buffer[i++] = CHAR_PERIOD;
    buffer[i++] = (lbs / 10) + CHAR_0;
    buffer[i++] = CHAR_SPACE;
    buffer[i++] = CHAR_l;
    buffer[i++] = CHAR_b;
    buffer[i++] = CHAR_s;
    buffer[i++] = CHAR_PERIOD;
    buffer[i++] = EOS;
    Menu_PrintText(buffer, left, top);
}
#else
static void PrintMonWeight(u16 weight, u8 left, u8 top)
{
    PrintDecimalNum(weight, left, top);
}
#endif

void DrawFootprint(u16 dexNum, u16 charBase, u16 tileNum)
{
    u8 footprint4bpp[0x80];
    u16 i, j, tileIdx;
    const u8 *footprintGfx;
    u8 footprint1bpp, tile;

    footprintGfx = (const u8*)gMonFootprintTable[NationalPokedexNumToSpecies(dexNum)];
    tileIdx = 0;
    for (i = 0; i < 32; i++)
    {
        footprint1bpp = footprintGfx[i];
        for (j = 0; j < 4; j++)
        {
            tile = 0;
            if (footprint1bpp & (1 << (j * 2)))
                tile |= 0x01;
            if (footprint1bpp & (2 << (j * 2)))
                tile |= 0x10;

            footprint4bpp[tileIdx] = tile;
            tileIdx++;
        }
    }
    CpuCopy16((void*)footprint4bpp, (void *)(BG_VRAM + charBase * 0x4000 + tileNum * 0x20), 0x80);
}

static void RS_DrawFootprint(u16 offset, u16 tileNum)
{
    *(u16 *)(BG_VRAM + offset * 0x800 + 0x232) = 0xF000 + tileNum + 0;
    *(u16 *)(BG_VRAM + offset * 0x800 + 0x234) = 0xF000 + tileNum + 1;
    *(u16 *)(BG_VRAM + offset * 0x800+ 0x272) = 0xF000 + tileNum + 2;
    *(u16 *)(BG_VRAM + offset * 0x800 + 0x274) = 0xF000 + tileNum + 3;
}

static u16 GetNextPosition(u8 direction, u16 position, u16 min, u16 max)
{
    switch (direction)
    {
    case 1:
        if (position > min)
            position--;
        break;
    case 0:
        if (position < max)
            position++;
        break;
    case 3:
        if (position > min)
            position--;
        else
            position = max;
        break;
    case 2:
        if (position < max)
            position++;
        else
            position = min;
        break;
    }
    return position;
}

static void _SpriteCB_Dummy(struct Sprite *sprite)
{
}

static void SetSpriteTemplateForPokemonPic(u16 paletteTag, u8 imagesSet)
{
    gMultiuseSpriteTemplate = sMonOrTrainerPicSpriteTemplate;
    gMultiuseSpriteTemplate.paletteTag = paletteTag;
    gMultiuseSpriteTemplate.images = sSpriteFrameImages_MonOrTrainerPic[imagesSet];
    gMultiuseSpriteTemplate.anims = gAnims_MonPic;
}

static void SetSpriteTemplateForSizeComparisonTrainerPic(u16 paletteTag, u8 imagesSet)
{
    gMultiuseSpriteTemplate = sMonOrTrainerPicSpriteTemplate;
    gMultiuseSpriteTemplate.paletteTag = paletteTag;
    gMultiuseSpriteTemplate.images = sSpriteFrameImages_MonOrTrainerPic[imagesSet];
    gMultiuseSpriteTemplate.anims = gTrainerFrontAnimsPtrTable[0];
}

u16 CreateMonSpriteFromNationalDexNumber(u16 nationalNum, s16 x, s16 y, u16 paletteSlot)
{
    u8 spriteId;

    nationalNum = NationalPokedexNumToSpecies(nationalNum);
    switch (nationalNum)
    {
    default:
        DecompressPicFromTable_2(
          &gMonFrontPicTable[nationalNum],
          gMonFrontPicCoords[nationalNum].size,
          gMonFrontPicCoords[nationalNum].y_offset,
          (void *)EWRAM,
          sMonOrTrainerPicDecompressDestPtrs[paletteSlot],
          nationalNum);
        break;
    case SPECIES_SPINDA:
        LoadSpecialPokePic(
          &gMonFrontPicTable[nationalNum],
          gMonFrontPicCoords[nationalNum].size,
          gMonFrontPicCoords[nationalNum].y_offset,
          (void *)EWRAM,
          sMonOrTrainerPicDecompressDestPtrs[paletteSlot],
          nationalNum,
          gSaveBlock2.pokedex.spindaPersonality,
          1);
        break;
    case SPECIES_UNOWN:
        LoadSpecialPokePic(
          &gMonFrontPicTable[nationalNum],
          gMonFrontPicCoords[nationalNum].size,
          gMonFrontPicCoords[nationalNum].y_offset,
          (void *)EWRAM,
          sMonOrTrainerPicDecompressDestPtrs[paletteSlot],
          nationalNum,
          gSaveBlock2.pokedex.unownPersonality,
          1);
        break;
    }
    LoadCompressedPalette(gMonPaletteTable[nationalNum].data, 0x100 + paletteSlot * 16, 32);
    SetSpriteTemplateForPokemonPic(paletteSlot, paletteSlot);
    spriteId = CreateSprite(&gMultiuseSpriteTemplate, x, y, 0);
    gSprites[spriteId].oam.paletteNum = paletteSlot;
    return spriteId;
}

u16 CreateSizeScreenTrainerPic(u16 gender, s16 x, s16 y, u16 paletteSlot)
{
    u8 spriteId;

    DecompressPicFromTable_2(
      &gTrainerFrontPicTable[gender],
      gTrainerFrontPicCoords[gender].size,
      gTrainerFrontPicCoords[gender].y_offset,
      (void *)EWRAM,
      sMonOrTrainerPicDecompressDestPtrs[0],
      gender);
    SetSpriteTemplateForSizeComparisonTrainerPic(gender, 0);
    spriteId = CreateSprite(&gMultiuseSpriteTemplate, x, y, 0);
    gSprites[spriteId].oam.paletteNum = paletteSlot;
    return spriteId;
}

static int DoPokedexSearch(u8 dexMode, u8 order, u8 abcGroup, u8 bodyColor, u8 type1, u8 type2)
{
    u16 species;
    u16 i;
    u16 resultsCount;
    u8 types[2];

    CreatePokedexList(dexMode, order);

    for (i = 0, resultsCount = 0; i < NATIONAL_DEX_COUNT; i++)
    {
        if (sPokedexView->pokedexList[i].seen)
        {
            sPokedexView->pokedexList[resultsCount] = sPokedexView->pokedexList[i];
            resultsCount++;
        }
    }
    sPokedexView->pokemonListCount = resultsCount;

    // Search by name
    if (abcGroup != 0xFF)
    {
        for (i = 0, resultsCount = 0; i < sPokedexView->pokemonListCount; i++)
        {
            u8 firstLetter;

            species = NationalPokedexNumToSpecies(sPokedexView->pokedexList[i].dexNum);
            firstLetter = gSpeciesNames[species][0];
            if ((firstLetter >= sLetterSearchRanges[abcGroup][0] && firstLetter < sLetterSearchRanges[abcGroup][0] + sLetterSearchRanges[abcGroup][1])
             || (firstLetter >= sLetterSearchRanges[abcGroup][2] && firstLetter < sLetterSearchRanges[abcGroup][2] + sLetterSearchRanges[abcGroup][3]))
            {
                sPokedexView->pokedexList[resultsCount] = sPokedexView->pokedexList[i];
                resultsCount++;
            }
        }
        sPokedexView->pokemonListCount = resultsCount;
    }

    // Search by body color
    if (bodyColor != 0xFF)
    {
        for (i = 0, resultsCount = 0; i < sPokedexView->pokemonListCount; i++)
        {
            species = NationalPokedexNumToSpecies(sPokedexView->pokedexList[i].dexNum);

            if (bodyColor == gSpeciesInfo[species].bodyColor)
            {
                sPokedexView->pokedexList[resultsCount] = sPokedexView->pokedexList[i];
                resultsCount++;
            }
        }
        sPokedexView->pokemonListCount = resultsCount;
    }

    // Search by type
    if (type1 != 0xFF || type2 != 0xFF)
    {
        if (type1 == 0xFF)
        {
            type1 = type2;
            type2 = 0xFF;
        }

        if (type2 == 0xFF)
        {
            for (i = 0, resultsCount = 0; i < sPokedexView->pokemonListCount; i++)
            {
                if (sPokedexView->pokedexList[i].owned)
                {
                    species = NationalPokedexNumToSpecies(sPokedexView->pokedexList[i].dexNum);

                    types[0] = gSpeciesInfo[species].type1;
                    types[1] = gSpeciesInfo[species].type2;
                    if (types[0] == type1 || types[1] == type1)
                    {
                        sPokedexView->pokedexList[resultsCount] = sPokedexView->pokedexList[i];
                        resultsCount++;
                    }
                }
            }
        }
        else
        {
            for (i = 0, resultsCount = 0; i < sPokedexView->pokemonListCount; i++)
            {
                if (sPokedexView->pokedexList[i].owned)
                {
                    species = NationalPokedexNumToSpecies(sPokedexView->pokedexList[i].dexNum);

                    types[0] = gSpeciesInfo[species].type1;
                    types[1] = gSpeciesInfo[species].type2;
                    if ((types[0] == type1 && types[1] == type2) || (types[0] == type2 && types[1] == type1))
                    {
                        sPokedexView->pokedexList[resultsCount] = sPokedexView->pokedexList[i];
                        resultsCount++;
                    }
                }
            }
        }
        sPokedexView->pokemonListCount = resultsCount;
    }

    if (sPokedexView->pokemonListCount != 0)
    {
        for (i = sPokedexView->pokemonListCount; i < NATIONAL_DEX_COUNT; i++)
        {
            sPokedexView->pokedexList[i].dexNum = 0xFFFF;
            sPokedexView->pokedexList[i].seen = FALSE;
            sPokedexView->pokedexList[i].owned = FALSE;

        }
    }

    return resultsCount;
}

#if ENGLISH
#define SUB_8091E20_WIDTH (208)
#elif GERMAN
#define SUB_8091E20_WIDTH (216)
#endif

static void EraseAndPrintSearchTextBox(const u8 *str)
{
    sub_8072AB0(str, 9, 120, SUB_8091E20_WIDTH, 32, 1);
}

static u8 LoadSearchMenu(void)
{
    return CreateTask(Task_LoadSearchMenu, 0);
}

static void Task_LoadSearchMenu(u8 taskId)
{
    u16 i;

    switch (gMain.state)
    {
    default:
    case 0:
        if (!gPaletteFade.active)
        {
            sPokedexView->currentPage = 2;
            ResetOtherVideoRegisters(0);
            LZ77UnCompVram(gPokedexSearchMenu_Gfx, (void *)VRAM);
            LZ77UnCompVram(gPokedexMenuSearch_Tilemap, (void *)(VRAM + 0x7800));
            LoadPalette(gPokedexSearchMenu_Pal + 1, 1, 0x7E);
            if (!IsNationalPokedexEnabled())
            {
                for (i = 0; i < 17; i++)
                {
                    ((u16 *)(VRAM + 0x7A80))[i] = ((u16 *)(VRAM + 0x7B00))[i];
                    ((u16 *)(VRAM + 0x7AC0))[i] = ((u16 *)(VRAM + 0x7B40))[i];
                    ((u16 *)(VRAM + 0x7B00))[i] = 1;
                    ((u16 *)(VRAM + 0x7B40))[i] = 1;
                }
            }
            gMain.state = 1;
        }
        break;
    case 1:
        Text_LoadWindowTemplate(&gWindowTemplate_81E7064);
        InitMenuWindow(&gWindowTemplate_81E7064);
        LoadCompressedObjectPic(&sInterfaceSpriteSheet[0]);
        LoadSpritePalettes(sInterfaceSpritePalette);
        CreateSearchParameterScrollArrows(taskId);
        for (i = 0; i < 16; i++)
            gTasks[taskId].data[i] = 0;
        SetDefaultSearchModeAndOrder(taskId);
        HighlightSelectedSearchTopBarItem(0);
        PrintSelectedSearchParameters(taskId);
        gMain.state++;
        break;
    case 2:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        gMain.state++;
        break;
    case 3:
        REG_BG3CNT = 0x0F03;
        REG_DISPCNT = 0x1C40;
        gMain.state++;
        break;
    case 4:
        if (!gPaletteFade.active)
        {
            gTasks[taskId].func = Task_SwitchToSearchMenuTopBar;
            gMain.state = 0;
        }
        break;
    }
}

static void Task_SwitchToSearchMenuTopBar(u8 taskId)
{
    HighlightSelectedSearchTopBarItem(gTasks[taskId].data[0]);
    PrintSelectedSearchParameters(taskId);
    gTasks[taskId].func = Task_HandleSearchTopBarInput;
}

static void Task_HandleSearchTopBarInput(u8 taskId)
{
    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_PC_OFF);
        gTasks[taskId].func = Task_ExitSearch;
        return;
    }
    if (JOY_NEW(A_BUTTON))
    {
        switch (gTasks[taskId].data[0])
        {
        case 0:
            PlaySE(SE_PIN);
            gTasks[taskId].data[1] = 0;
            gTasks[taskId].func = Task_SwitchToSearchMenu;
            break;
        case 1:
            PlaySE(SE_PIN);
            gTasks[taskId].data[1] = 4;
            gTasks[taskId].func = Task_SwitchToSearchMenu;
            break;
        case 2:
            PlaySE(SE_PC_OFF);
            gTasks[taskId].func = Task_ExitSearch;
            break;
        }
        return;
    }
    if (JOY_NEW(DPAD_LEFT) && gTasks[taskId].data[0] > 0)
    {
        PlaySE(SE_DEX_PAGE);
        gTasks[taskId].data[0]--;
        HighlightSelectedSearchTopBarItem(gTasks[taskId].data[0]);
    }
    if (JOY_NEW(DPAD_RIGHT) && gTasks[taskId].data[0] < 2)
    {
        PlaySE(SE_DEX_PAGE);
        gTasks[taskId].data[0]++;
        HighlightSelectedSearchTopBarItem(gTasks[taskId].data[0]);
    }
}

static void Task_SwitchToSearchMenu(u8 taskId)
{
    HighlightSelectedSearchMenuItem(gTasks[taskId].data[0], gTasks[taskId].data[1]);
    PrintSelectedSearchParameters(taskId);
    gTasks[taskId].func = Task_HandleSearchMenuInput;
}

static void Task_HandleSearchMenuInput(u8 taskId)
{
    const u8 (*movementMap)[4];

    if (gTasks[taskId].data[0] != 0)
    {
        if (!IsNationalPokedexEnabled())
            movementMap = sSearchMovementMap_ShiftHoennDex;
        else
            movementMap = sSearchMovementMap_ShiftNatDex;
    }
    else
    {
        if (!IsNationalPokedexEnabled())
            movementMap = sSearchMovementMap_SearchHoennDex;
        else
            movementMap = sSearchMovementMap_SearchNatDex;
    }

    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_BALL);
        SetDefaultSearchModeAndOrder(taskId);
        gTasks[taskId].func = Task_SwitchToSearchMenuTopBar;
        return;
    }
    if (JOY_NEW(A_BUTTON))
    {
        if (gTasks[taskId].data[1] == 6)
        {
            if (gTasks[taskId].data[0] != 0)
            {
                sPokeBallRotation = 0x40;
                sPokedexView->pokeBallRotationBackup = 0x40;
                sLastSelectedPokemon = 0;
                sPokedexView->selectedPokemonBackup = 0;
                gSaveBlock2.pokedex.mode = GetSearchModeSelection(taskId, 5);
                if (!IsNationalPokedexEnabled())
                    gSaveBlock2.pokedex.mode = 0;
                sPokedexView->dexModeBackup = gSaveBlock2.pokedex.mode;
                gSaveBlock2.pokedex.order = GetSearchModeSelection(taskId, 4);
                sPokedexView->dexOrderBackup = gSaveBlock2.pokedex.order;
                PlaySE(SE_PC_OFF);
                gTasks[taskId].func = Task_ExitSearch;
            }
            else
            {
                EraseAndPrintSearchTextBox(gText_SearchingPleaseWait);
                gTasks[taskId].func = Task_StartPokedexSearch;
                PlaySE(SE_DEX_SEARCH);
            }
        }
        else
        {
            PlaySE(SE_PIN);
            gTasks[taskId].func = Task_SelectSearchMenuItem;
        }
        return;
    }

    if (JOY_NEW(DPAD_LEFT) && movementMap[gTasks[taskId].data[1]][0] != 0xFF)
    {
        PlaySE(SE_SELECT);
        gTasks[taskId].data[1] = movementMap[gTasks[taskId].data[1]][0];
        HighlightSelectedSearchMenuItem(gTasks[taskId].data[0], gTasks[taskId].data[1]);
    }
    if (JOY_NEW(DPAD_RIGHT) && movementMap[gTasks[taskId].data[1]][1] != 0xFF)
    {
        PlaySE(SE_SELECT);
        gTasks[taskId].data[1] = movementMap[gTasks[taskId].data[1]][1];
        HighlightSelectedSearchMenuItem(gTasks[taskId].data[0], gTasks[taskId].data[1]);
    }
    if (JOY_NEW(DPAD_UP) && movementMap[gTasks[taskId].data[1]][2] != 0xFF)
    {
        PlaySE(SE_SELECT);
        gTasks[taskId].data[1] = movementMap[gTasks[taskId].data[1]][2];
        HighlightSelectedSearchMenuItem(gTasks[taskId].data[0], gTasks[taskId].data[1]);
    }
    if (JOY_NEW(DPAD_DOWN) && movementMap[gTasks[taskId].data[1]][3] != 0xFF)
    {
        PlaySE(SE_SELECT);
        gTasks[taskId].data[1] = movementMap[gTasks[taskId].data[1]][3];
        HighlightSelectedSearchMenuItem(gTasks[taskId].data[0], gTasks[taskId].data[1]);
    }
}

static void Task_StartPokedexSearch(u8 taskId)
{
    u8 dexMode = GetSearchModeSelection(taskId, 5);
    u8 order = GetSearchModeSelection(taskId, 4);
    u8 abcGroup = GetSearchModeSelection(taskId, 0);
    u8 bodyColor = GetSearchModeSelection(taskId, 1);
    u8 type1 = GetSearchModeSelection(taskId, 2);
    u8 type2 = GetSearchModeSelection(taskId, 3);

    DoPokedexSearch(dexMode, order, abcGroup, bodyColor, type1, type2);
    gTasks[taskId].func = Task_WaitAndCompleteSearch;
}

static void Task_WaitAndCompleteSearch(u8 taskId)
{
    if (!IsSEPlaying())
    {
        if (sPokedexView->pokemonListCount != 0)
        {
            PlaySE(SE_SUCCESS);
            EraseAndPrintSearchTextBox(gText_SearchCompleted);
        }
        else
        {
            PlaySE(SE_FAILURE);
            EraseAndPrintSearchTextBox(gText_NoMatchingPkmnWereFound);
        }
        gTasks[taskId].func = Task_SearchCompleteWaitForInput;
    }
}

static void Task_SearchCompleteWaitForInput(u8 taskId)
{
    if (JOY_NEW(A_BUTTON))
    {
        if (sPokedexView->pokemonListCount != 0)
        {
            sPokedexView->screenSwitchState = 1;
            sPokedexView->dexMode = GetSearchModeSelection(taskId, 5);
            sPokedexView->dexOrder = GetSearchModeSelection(taskId, 4);
            gTasks[taskId].func = Task_ExitSearch;
            PlaySE(SE_PC_OFF);
        }
        else
        {
            gTasks[taskId].func = Task_SwitchToSearchMenu;
            PlaySE(SE_BALL);
        }
    }
}

static void PrintSelectorArrow(u16 y, int unused)
{
    MenuCursor_SetPos814AD7C(0x90, (y * 2 + 1) * 8);
}

static void Task_SelectSearchMenuItem(u8 taskId)
{
    u8 menuItem;
    u16 *cursorPos;
    u16 *scrollOffset;

    DrawOrEraseSearchParameterBox(0);
    menuItem = gTasks[taskId].data[1];
    cursorPos = (u16 *)&gTasks[taskId].data[sSearchOptions[menuItem].taskDataCursorPos];
    scrollOffset = (u16 *)&gTasks[taskId].data[sSearchOptions[menuItem].taskDataScrollOffset];
    gTasks[taskId].data[14] = *cursorPos;
    gTasks[taskId].data[15] = *scrollOffset;
    PrintSearchParameterText(taskId);
    CreateBlendedOutlineCursor(16, 0xFFFF, 12, 0x2D9F, 11);
    PrintSelectorArrow(*cursorPos, 1);
    gTasks[taskId].func = Task_HandleSearchParameterInput;
}

static void Task_HandleSearchParameterInput(u8 taskId)
{
    u8 menuItem;
    const struct SearchOptionText *texts;
    u16 *cursorPos;
    u16 *scrollOffset;
    u16 maxOption;
    bool8 moved;

    menuItem = gTasks[taskId].data[1];
    texts = sSearchOptions[menuItem].texts;
    cursorPos = (u16 *)&gTasks[taskId].data[sSearchOptions[menuItem].taskDataCursorPos];
    scrollOffset = (u16 *)&gTasks[taskId].data[sSearchOptions[menuItem].taskDataScrollOffset];
    maxOption = sSearchOptions[menuItem].numOptions - 1;
    if (JOY_NEW(A_BUTTON))
    {
        sub_814ADC8();
        PlaySE(SE_PIN);
        Menu_EraseWindowRect(18, 1, 28, 12);
        DrawOrEraseSearchParameterBox(1);
        gTasks[taskId].func = Task_SwitchToSearchMenu;
        return;
    }
    if (JOY_NEW(B_BUTTON))
    {
        sub_814ADC8();
        PlaySE(SE_BALL);
        Menu_EraseWindowRect(18, 1, 28, 12);
        DrawOrEraseSearchParameterBox(1);
        *cursorPos = gTasks[taskId].data[14];
        *scrollOffset = gTasks[taskId].data[15];
        gTasks[taskId].func = Task_SwitchToSearchMenu;
        return;
    }
    moved = FALSE;
    if (JOY_REPEAT(DPAD_UP))
    {
        if (*cursorPos != 0)
        {
            PrintSelectorArrow(*cursorPos, 0);
            (*cursorPos)--;
            PrintSelectorArrow(*cursorPos, 1);
            moved = TRUE;
        }
        else if (*scrollOffset != 0)
        {
            (*scrollOffset)--;
            PrintSearchParameterText(taskId);
            PrintSelectorArrow(*cursorPos, 1);
            moved = TRUE;
        }
        if (moved)
        {
            PlaySE(SE_SELECT);
            EraseAndPrintSearchTextBox(texts[*cursorPos + *scrollOffset].description);
        }
        return;
    }
    if (JOY_REPEAT(DPAD_DOWN))
    {
        if (*cursorPos < 5 && *cursorPos < maxOption)
        {
            PrintSelectorArrow(*cursorPos, 0);
            (*cursorPos)++;
            PrintSelectorArrow(*cursorPos, 1);
            moved = TRUE;
        }
        else if (maxOption > 5 && *scrollOffset < maxOption - 5)
        {
            (*scrollOffset)++;
            PrintSearchParameterText(taskId);
            PrintSelectorArrow(5, 1);
            moved = TRUE;
        }
        if (moved)
        {
            PlaySE(SE_SELECT);
            EraseAndPrintSearchTextBox(texts[*cursorPos + *scrollOffset].description);
        }
        return;
    }
}

static void Task_ExitSearch(u8 taskId)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = Task_ExitSearchWaitForFade;
}

static void Task_ExitSearchWaitForFade(u8 taskId)
{
    if (!gPaletteFade.active)
        DestroyTask(taskId);
}

#define VRAM_ADDR(a, b, c) *(u16 *)(BG_VRAM + (15 * 0x800) + (c) * 64 + ((b) + (a))*2)
void SetSearchRectHighlight(u8 flags, u8 x, u8 y, u8 width)
{
    u16 i, temp;

    for (i = 0; i < width; i++)
    {
        temp = VRAM_ADDR(i, x, y);
        temp &= 0xFFF;
        temp |= flags << 12;

        VRAM_ADDR(i, x, y) = temp;

        temp = VRAM_ADDR(i, x, y + 1);
        temp &= 0xFFF;
        temp |= flags << 12;
        VRAM_ADDR(i, x, y + 1) = temp;
    }
}

static void DrawSearchMenuItemBgHighlight(u8 searchBg, bool8 unselected, bool8 disabled)
{
    u8 highlightFlags = (unselected & 1) | ((disabled & 1) << 1);

    switch (searchBg)
    {
    case 0:
    case 1:
    case 2:
        SetSearchRectHighlight(highlightFlags,
            sSearchMenuTopBarItems[searchBg].highlightX,
            sSearchMenuTopBarItems[searchBg].highlightY,
            sSearchMenuTopBarItems[searchBg].highlightWidth);
        break;
    case 3:
    case 4:
    case 7:
    case 8:
        SetSearchRectHighlight(highlightFlags,
            sSearchMenuItems[searchBg - 3].titleBgX,
            sSearchMenuItems[searchBg - 3].titleBgY,
            sSearchMenuItems[searchBg - 3].titleBgWidth);
        // fall through
    case 5:
    case 6:
        SetSearchRectHighlight(highlightFlags,
            sSearchMenuItems[searchBg - 3].selectionBgX,
            sSearchMenuItems[searchBg - 3].selectionBgY,
            sSearchMenuItems[searchBg - 3].selectionBgWidth);
        break;
    case 10:
        SetSearchRectHighlight(
            highlightFlags, sSearchMenuItems[2].titleBgX, sSearchMenuItems[2].titleBgY, sSearchMenuItems[2].titleBgWidth);
        break;
    case 9:
        if (!IsNationalPokedexEnabled())
            SetSearchRectHighlight(highlightFlags,
                sSearchMenuItems[searchBg - 3].titleBgX,
                sSearchMenuItems[searchBg - 3].titleBgY - 2,
                sSearchMenuItems[searchBg - 3].titleBgWidth);
        else
            SetSearchRectHighlight(highlightFlags,
                sSearchMenuItems[searchBg - 3].titleBgX,
                sSearchMenuItems[searchBg - 3].titleBgY,
                sSearchMenuItems[searchBg - 3].titleBgWidth);
        break;
    }
}

static void SetInitialSearchMenuBgHighlights(u8 topBarItem)
{
    switch (topBarItem)
    {
    case 0:
        DrawSearchMenuItemBgHighlight(0, 0, 0);
        DrawSearchMenuItemBgHighlight(1, 1, 0);
        DrawSearchMenuItemBgHighlight(2, 1, 0);
        DrawSearchMenuItemBgHighlight(3, 1, 0);
        DrawSearchMenuItemBgHighlight(4, 1, 0);
        DrawSearchMenuItemBgHighlight(10, 1, 0);
        DrawSearchMenuItemBgHighlight(5, 1, 0);
        DrawSearchMenuItemBgHighlight(6, 1, 0);
        DrawSearchMenuItemBgHighlight(7, 1, 0);
        DrawSearchMenuItemBgHighlight(8, 1, 0);
        DrawSearchMenuItemBgHighlight(9, 1, 0);
        break;
    case 1:
        DrawSearchMenuItemBgHighlight(0, 1, 0);
        DrawSearchMenuItemBgHighlight(1, 0, 0);
        DrawSearchMenuItemBgHighlight(2, 1, 0);
        DrawSearchMenuItemBgHighlight(3, 1, 1);
        DrawSearchMenuItemBgHighlight(4, 1, 1);
        DrawSearchMenuItemBgHighlight(10, 1, 1);
        DrawSearchMenuItemBgHighlight(5, 1, 1);
        DrawSearchMenuItemBgHighlight(6, 1, 1);
        DrawSearchMenuItemBgHighlight(7, 1, 0);
        DrawSearchMenuItemBgHighlight(8, 1, 0);
        DrawSearchMenuItemBgHighlight(9, 1, 0);
        break;
    case 2:
        DrawSearchMenuItemBgHighlight(0, 1, 0);
        DrawSearchMenuItemBgHighlight(1, 1, 0);
        DrawSearchMenuItemBgHighlight(2, 0, 0);
        DrawSearchMenuItemBgHighlight(3, 1, 1);
        DrawSearchMenuItemBgHighlight(4, 1, 1);
        DrawSearchMenuItemBgHighlight(10, 1, 1);
        DrawSearchMenuItemBgHighlight(5, 1, 1);
        DrawSearchMenuItemBgHighlight(6, 1, 1);
        DrawSearchMenuItemBgHighlight(7, 1, 1);
        DrawSearchMenuItemBgHighlight(8, 1, 1);
        DrawSearchMenuItemBgHighlight(9, 1, 1);
        break;
    }
}

static void HighlightSelectedSearchTopBarItem(u8 topBarItem)
{
    SetInitialSearchMenuBgHighlights(topBarItem);
    EraseAndPrintSearchTextBox(sSearchMenuTopBarItems[topBarItem].description);
}

static void HighlightSelectedSearchMenuItem(u8 topBarItem, u8 menuItem)
{
    SetInitialSearchMenuBgHighlights(topBarItem);
    switch (menuItem)
    {
    case 0:
        DrawSearchMenuItemBgHighlight(3, 0, 0);
        break;
    case 1:
        DrawSearchMenuItemBgHighlight(4, 0, 0);
        break;
    case 2:
        DrawSearchMenuItemBgHighlight(10, 0, 0);
        DrawSearchMenuItemBgHighlight(5, 0, 0);
        break;
    case 3:
        DrawSearchMenuItemBgHighlight(10, 0, 0);
        DrawSearchMenuItemBgHighlight(6, 0, 0);
        break;
    case 4:
        DrawSearchMenuItemBgHighlight(7, 0, 0);
        break;
    case 5:
        DrawSearchMenuItemBgHighlight(8, 0, 0);
        break;
    case 6:
        DrawSearchMenuItemBgHighlight(9, 0, 0);
        break;
    }
    EraseAndPrintSearchTextBox(sSearchMenuItems[menuItem].description);
}

static void PrintSelectedSearchParameters(u8 taskId)
{
    u16 searchParamId;

    searchParamId = gTasks[taskId].data[6] + gTasks[taskId].data[7];
    StringCopy(gStringVar1, sDexSearchNameOptions[searchParamId].title);
    Menu_PrintTextPixelCoords(gUnknown_083B5AB2, 45, 16, 1);

    searchParamId = gTasks[taskId].data[8] + gTasks[taskId].data[9];
    StringCopy(gStringVar1, sDexSearchColorOptions[searchParamId].title);
    Menu_PrintTextPixelCoords(gUnknown_083B5AB2, 45, 32, 1);

    searchParamId = gTasks[taskId].data[10] + gTasks[taskId].data[11];
    StringCopy(gStringVar1, sDexSearchTypeOptions[searchParamId].title);
    Menu_PrintTextPixelCoords(gUnknown_083B5AAC, 45, 48, 1);

    searchParamId = gTasks[taskId].data[12] + gTasks[taskId].data[13];
    StringCopy(gStringVar1, sDexSearchTypeOptions[searchParamId].title);
    Menu_PrintTextPixelCoords(gUnknown_083B5AAC, 93, 48, 1);

    searchParamId = gTasks[taskId].data[4] + gTasks[taskId].data[5];
    StringCopy(gStringVar1, sDexOrderOptions[searchParamId].title);
    Menu_PrintTextPixelCoords(gUnknown_083B5AB2, 45, 64, 1);

    if (IsNationalPokedexEnabled())
    {
        searchParamId = gTasks[taskId].data[2] + gTasks[taskId].data[3];
        StringCopy(gStringVar1, sDexModeOptions[searchParamId].title);
        Menu_PrintTextPixelCoords(gUnknown_083B5AB2, 45, 80, 1);
    }
}

static void DrawOrEraseSearchParameterBox(bool8 erase)
{
    u16 i;
    u16 j;

    if (erase == 0)
    {
        *((u16 *)(VRAM + 0x7800 + 0x22)) = 0xC0B;
        for (i = 0x12; i < 0x1D; i++)
            *((u16 *)(VRAM + 0x7800 + i * 2)) = 0x80D;
        *((u16 *)(VRAM + 0x7800 + 0x3A)) = 0x80B;
        for (j = 1; j < 13; j++)
        {
            *((u16 *)(VRAM + 0x7800 + 0x22 + j * 64)) = 0x40A;
            for (i = 0x12; i < 0x1D; i++)
                *((u16 *)(VRAM + 0x7800 + j * 64 + i * 2)) = 2;
            *((u16 *)(VRAM + 0x7800 + 0x3A + j * 64)) = 0xA;
        }
        *((u16 *)(VRAM + 0x7800 + 0x362)) = 0x40B;
        for (i = 0x12; i < 0x1D; i++)
            *((u16 *)(VRAM + 0x7800 + 0x340 + i * 2)) = 0xD;
        *((u16 *)(VRAM + 0x7800 + 0x37A)) = 0xB;
    }
    else
    {
        for (j = 0; j < 14; j++)
        {
            for (i = 0x11; i < 0x1E; i++)
            {
                *((u16 *)(VRAM + 0x7800 + j * 64 + i * 2)) = 0x4F;
            }
        }
    }
}

// Prints the currently viewable search parameter titles in the right-hand text box
// and the currently selected search parameter description in the bottom text box
static void PrintSearchParameterText(u8 taskId)
{
    const struct SearchOptionText *texts = sSearchOptions[gTasks[taskId].data[1]].texts;
    const u16 *cursorPos = &gTasks[taskId].data[sSearchOptions[gTasks[taskId].data[1]].taskDataCursorPos];
    const u16 *scrollOffset = &gTasks[taskId].data[sSearchOptions[gTasks[taskId].data[1]].taskDataScrollOffset];
    u16 i;
    u16 j;

    Menu_EraseWindowRect(18, 1, 28, 12);
    for (i = 0, j = *scrollOffset; i < 6; i++, j++)
    {
        if (texts[j].title == NULL)
            break;
        Menu_PrintText(texts[j].title, 18, i * 2 + 1);
    }
    EraseAndPrintSearchTextBox(texts[*cursorPos + *scrollOffset].description);
}

static u8 GetSearchModeSelection(u8 taskId, u8 option)
{
    const u16 *cursorPos = &gTasks[taskId].data[sSearchOptions[option].taskDataCursorPos];
    const u16 *scrollOffset = &gTasks[taskId].data[sSearchOptions[option].taskDataScrollOffset];
    u16 id = *cursorPos + *scrollOffset;

    switch (option)
    {
    default:
        return 0;
    case 5:
        return sPokedexModes[id];
    case 4:
        return sOrderOptions[id];
    case 0:
        if (id == 0)
            return 0xFF;
        else
            return id;
    case 1:
        if (id == 0)
            return 0xFF;
        else
            return id - 1;
    case 2:
    case 3:
        return sDexSearchTypeIds[id];
    }
}

static void SetDefaultSearchModeAndOrder(u8 taskId)
{
    u16 selected;

    switch (sPokedexView->dexModeBackup)
    {
    default:
    case 0:
        selected = 0;
        break;
    case 1:
        selected = 1;
        break;
    }
    gTasks[taskId].data[2] = selected;

    switch (sPokedexView->dexOrderBackup)
    {
    default:
    case 0:
        selected = 0;
        break;
    case 1:
        selected = 1;
        break;
    case 2:
        selected = 2;
        break;
    case 3:
        selected = 3;
        break;
    case 4:
        selected = 4;
        break;
    case 5:
        selected = 5;
        break;
    }
    gTasks[taskId].data[4] = selected;
}

static bool8 SearchParamCantScrollUp(u8 taskId)
{
    u8 val1 = gTasks[taskId].data[1];
    const u16 *ptr = (const u16 *)&gTasks[taskId].data[sSearchOptions[val1].taskDataScrollOffset];
    u16 val2 = sSearchOptions[val1].numOptions - 1;

    if (val2 > 5 && *ptr != 0)
        return FALSE;
    else
        return TRUE;
}

static bool8 SearchParamCantScrollDown(u8 taskId)
{
    u8 val1 = gTasks[taskId].data[1];
    const u16 *ptr = (const u16 *)&gTasks[taskId].data[sSearchOptions[val1].taskDataScrollOffset];
    u16 val2 = sSearchOptions[val1].numOptions - 1;

    if (val2 > 5 && *ptr < val2 - 5)
        return FALSE;
    else
        return TRUE;
}

static void SpriteCB_SearchParameterScrollArrow(struct Sprite *sprite)
{
    if (gTasks[sprite->data[0]].func == Task_HandleSearchParameterInput)
    {
        u8 val;

        if (sprite->data[1] != 0)
        {
            if (SearchParamCantScrollDown(sprite->data[0]))
                sprite->invisible = TRUE;
            else
                sprite->invisible = FALSE;
        }
        else
        {
            if (SearchParamCantScrollUp(sprite->data[0]))
                sprite->invisible = TRUE;
            else
                sprite->invisible = FALSE;
        }
        val = sprite->data[2] + sprite->data[1] * 128;
        sprite->y2 = gSineTable[val] / 128;
        sprite->data[2] += 8;
    }
    else
    {
        sprite->invisible = TRUE;
    }
}

static void CreateSearchParameterScrollArrows(u8 taskId)
{
    u8 spriteId;

    spriteId = CreateSprite(&sScrollArrowSpriteTemplate, 184, 4, 0);
    gSprites[spriteId].data[0] = taskId;
    gSprites[spriteId].data[1] = 0;
    gSprites[spriteId].callback = SpriteCB_SearchParameterScrollArrow;

    spriteId = CreateSprite(&sScrollArrowSpriteTemplate, 184, 108, 0);
    gSprites[spriteId].data[0] = taskId;
    gSprites[spriteId].data[1] = 1;
    gSprites[spriteId].vFlip = TRUE;
    gSprites[spriteId].callback = SpriteCB_SearchParameterScrollArrow;
}
