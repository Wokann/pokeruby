
// Includes
#include "global.h"
#include "ewram.h"
#include "data.h"
#include "constants/moves.h"
#include "constants/species.h"
#include "palette.h"
#include "string_util.h"
#include "text.h"
#include "menu.h"
#include "item.h"
#include "pokemon_summary_screen.h"
#include "pokemon_storage_system.h"
#include "sound.h"
#include "constants/songs.h"

// Static type declarations

struct Wallpaper {
    const u8 *tiles;
    u32 tilesSize;
    const u8 *tilemap;
    const u16 *palettes;
};

// Static RAM declarations

EWRAM_DATA struct Pokemon sSavedMovingMon = {};
#if DEBUG
EWRAM_DATA u8 gDebugPokeStorageActive = 0;
EWRAM_DATA u32 gDebugPokeStorageReturnCallback = 0;
EWRAM_DATA u32 gDebugPokeStorageMon = 0;
#endif
EWRAM_DATA s8 sCursorArea = 0;
EWRAM_DATA s8 sCursorPosition = 0;
EWRAM_DATA bool8 sIsMonBeingMoved = FALSE;
EWRAM_DATA u8 sMovingMonOrigBoxId = 0;
EWRAM_DATA u8 sMovingMonOrigBoxPos = 0;
EWRAM_DATA u8 sAutoActionOn = 0;
EWRAM_DATA u8 sSavedCursorPosition = 0;
EWRAM_DATA struct TilemapTransferQueue *sTilemapTransferQueue = NULL;

// Static ROM declarations

s8 DetermineBoxScrollDirection(u8 boxId);
void LoadWallpaperGfx(u8 boxId, s8 direction);
void DrawWallpaper(u16 *buffer, const u16 *tilemap, s8 direction, u8 offset);
void TrimOldWallpaper(u16 *dest);
void InitBoxTitle(u8 boxId);
void CreateIncomingBoxTitle(u8 boxId, s8 direction);
void CycleBoxTitleSprites(void);
void SpriteCB_IncomingBoxTitle(struct Sprite *sprite);
void SpriteCB_OutgoingBoxTitle(struct Sprite *sprite);
void CycleBoxTitleColor(void);
s16 GetBoxTitleBaseX(u8 width);
void CreateBoxScrollArrows(void);
void StartBoxScrollArrowsSlide(s8 direction);
void StopBoxScrollArrowsSlide(void);
void DoCursorNewPosUpdate(void);
void SpriteCB_Arrow(struct Sprite *sprite);
bool8 MonPlaceChange_Grab(void);
bool8 MonPlaceChange_Place(void);
bool8 MonPlaceChange_Shift(void);
bool8 MonPlaceChange_CursorDown(void);
bool8 MonPlaceChange_CursorUp(void);
void MoveMon(void);
void PlaceMon(void);
void SetMovingMonData(u8 boxId, u8 position);
void SetPlacedMonData(u8 boxId, u8 position);
void PurgeMonOrBoxMon(u8 boxId, u8 position);
void SetShiftedMonData(u8 boxId, u8 position);
bool8 IsCursorOnBoxTitle(void);
void TryRefreshDisplayMon(void);
void ReshowDisplayMon(void);
void SetDisplayMonData(void *pokemon, u8 mode);
bool8 SetSelectionMenuTexts(void);
void CreateCursorSprites(void);
void ToggleCursorAutoAction(void);
s8 GetPokeStorageMenuItemTextId(u8 menuIndex);
void AddBoxOptionsMenu(void);
void RunTilemapCopy(struct TilemapTransfer *transfer);
void RunTilemapFill(struct TilemapTransfer *transfer);

#define PSS_CURSOR_DATA __attribute__((section(".rodata.pokemon_storage_cursor")))

#define PSS_GFXTAG_CURSOR        0
#define PSS_GFXTAG_CURSOR_SHADOW 1
#define PSS_PALTAG_CURSOR        0xDAC6
#define PSS_PALTAG_CURSOR_ALT    0xDAD1

u8 InBoxInput_Normal(void);
u8 HandleInput_InParty(void);
u8 HandleInput_OnBox(void);
u8 HandleInput_OnButtons(void);
void SpriteCB_CursorShadow(struct Sprite *sprite);

extern const u16 gHandCursorPalette[];
extern const u16 gHandCursorAltPalette[];
extern const u8 gHandCursorTiles[];
extern const u8 gHandCursorShadowTiles[];

extern const u8 PCText_Cancel2[];
extern const u8 PCText_Deposit[];
extern const u8 gPCText_Withdraw[];
extern const u8 gPCText_Move[];
extern const u8 PCText_Switch[];
extern const u8 gPCText_Place[];
extern const u8 gPCText_Summary[];
extern const u8 gPCText_Release[];
extern const u8 gPCText_Mark[];
extern const u8 gPCText_Jump[];
extern const u8 gPCText_Wallpaper[];
extern const u8 gPCText_Name[];
extern const u8 gPCText_Scenery1[];
extern const u8 gPCText_Scenery2[];
extern const u8 gPCText_Scenery3[];
extern const u8 gPCText_Etcetera[];
extern const u8 gPCText_Forest[];
extern const u8 gPCText_City[];
extern const u8 gPCText_Desert[];
extern const u8 gPCText_Savanna[];
extern const u8 gPCText_Crag[];
extern const u8 gPCText_Volcano[];
extern const u8 gPCText_Snow[];
extern const u8 gPCText_Cave[];
extern const u8 gPCText_Beach[];
extern const u8 gPCText_Seafloor[];
extern const u8 gPCText_River[];
extern const u8 gPCText_Sky[];
extern const u8 gPCText_PolkaDot[];
extern const u8 gPCText_Pokecenter[];
extern const u8 gPCText_Machine[];
extern const u8 PCText_Plain[];

const struct PokeStorageInputHandler gPokeStorageInputHandlers[] PSS_CURSOR_DATA =
{
    {InBoxInput_Normal, PSS_CURSOR_AREA_IN_BOX},
    {HandleInput_InParty, PSS_CURSOR_AREA_IN_PARTY},
    {HandleInput_OnBox, PSS_CURSOR_AREA_BOX_TITLE},
    {HandleInput_OnButtons, PSS_CURSOR_AREA_BUTTONS},
    {NULL, 0},
};

const struct SpriteSheet gPokeStorageCursorSpriteSheets[] PSS_CURSOR_DATA =
{
    {gHandCursorTiles, 0x800, PSS_GFXTAG_CURSOR},
    {gHandCursorShadowTiles, 0x80, PSS_GFXTAG_CURSOR_SHADOW},
    {NULL, 0, 0},
};

const struct SpritePalette gPokeStorageCursorSpritePalettes[] PSS_CURSOR_DATA =
{
    {gHandCursorPalette, PSS_PALTAG_CURSOR},
    {gHandCursorAltPalette, PSS_PALTAG_CURSOR_ALT},
    {NULL, 0},
};

const struct OamData gOamData_PokeStorageCursor PSS_CURSOR_DATA =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x32),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(32x32),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

const struct OamData gOamData_PokeStorageCursorShadow PSS_CURSOR_DATA =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(16x16),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(16x16),
    .tileNum = 0,
    .priority = 2,
    .paletteNum = 0,
    .affineParam = 0,
};

const union AnimCmd gAnim_PokeStorageCursorBouncing[] PSS_CURSOR_DATA =
{
    ANIMCMD_FRAME(0, 30),
    ANIMCMD_FRAME(16, 30),
    ANIMCMD_JUMP(0)
};

const union AnimCmd gAnim_PokeStorageCursorStill[] PSS_CURSOR_DATA =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END
};

const union AnimCmd gAnim_PokeStorageCursorOpen[] PSS_CURSOR_DATA =
{
    ANIMCMD_FRAME(32, 5),
    ANIMCMD_END
};

const union AnimCmd gAnim_PokeStorageCursorFist[] PSS_CURSOR_DATA =
{
    ANIMCMD_FRAME(48, 5),
    ANIMCMD_END
};

const union AnimCmd *const gAnims_PokeStorageCursor[] PSS_CURSOR_DATA =
{
    gAnim_PokeStorageCursorBouncing,
    gAnim_PokeStorageCursorStill,
    gAnim_PokeStorageCursorOpen,
    gAnim_PokeStorageCursorFist,
};

const struct SpriteTemplate gSpriteTemplate_PokeStorageCursor PSS_CURSOR_DATA =
{
    .tileTag = PSS_GFXTAG_CURSOR,
    .paletteTag = PSS_PALTAG_CURSOR,
    .oam = &gOamData_PokeStorageCursor,
    .anims = gAnims_PokeStorageCursor,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

const struct SpriteTemplate gSpriteTemplate_PokeStorageCursorShadow PSS_CURSOR_DATA =
{
    .tileTag = PSS_GFXTAG_CURSOR_SHADOW,
    .paletteTag = PSS_PALTAG_CURSOR,
    .oam = &gOamData_PokeStorageCursorShadow,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_CursorShadow,
};

#if DEBUG
#if DEBUG_FIX
const u8 gText_DebugPokeStorageSet[] PSS_CURSOR_DATA = _("SET");
#else
const u8 gText_DebugPokeStorageSet[] PSS_CURSOR_DATA = _("セットする");
#endif
#endif

const u8 *const gPokeStorageMenuTexts[] PSS_CURSOR_DATA =
{
    PCText_Cancel2,
    PCText_Deposit,
    gPCText_Withdraw,
    gPCText_Move,
    PCText_Switch,
    gPCText_Place,
    gPCText_Summary,
    gPCText_Release,
    gPCText_Mark,
    gPCText_Jump,
    gPCText_Wallpaper,
    gPCText_Name,
    gPCText_Scenery1,
    gPCText_Scenery2,
    gPCText_Scenery3,
    gPCText_Etcetera,
    gPCText_Forest,
    gPCText_City,
    gPCText_Desert,
    gPCText_Savanna,
    gPCText_Crag,
    gPCText_Volcano,
    gPCText_Snow,
    gPCText_Cave,
    gPCText_Beach,
    gPCText_Seafloor,
    gPCText_River,
    gPCText_Sky,
    gPCText_PolkaDot,
    gPCText_Pokecenter,
    gPCText_Machine,
    PCText_Plain,
#if DEBUG
    gText_DebugPokeStorageSet,
#endif
};

// .rodata

const u16 sWallpaperBoxPalette_Forest[] = INCBIN_U16("graphics/pokemon_storage/box_bg1.gbapal");
const u16 sWallpaperFramePalette_Forest[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/forest/frame.gbapal");
const u16 sWallpaperBgPalette_Forest[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/forest/bg.gbapal");

const u8 sWallpaperTiles_Forest[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/forest/tiles.4bpp.lz");

const u8 sWallpaperTilemap_Forest[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/forest/tilemap.bin.lz");

const u16 sWallpaperBoxPalette_City[] = INCBIN_U16("graphics/pokemon_storage/box_bg1.gbapal");
const u16 sWallpaperFramePalette_City[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/city/frame.gbapal");
const u16 sWallpaperBgPalette_City[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/city/bg.gbapal");

const u8 sWallpaperTiles_City[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/city/tiles.4bpp.lz");

const u8 sWallpaperTilemap_City[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/city/tilemap.bin.lz");

const u16 sWallpaperBoxPalette_Desert[] = INCBIN_U16("graphics/pokemon_storage/box_bg1.gbapal");
const u16 sWallpaperFramePalette_Desert[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/desert/frame.gbapal");
const u16 sWallpaperBgPalette_Desert[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/desert/bg.gbapal");

const u8 sWallpaperTiles_Desert[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/desert/tiles.4bpp.lz");

const u8 sWallpaperTilemap_Desert[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/desert/tilemap.bin.lz");

const u16 sWallpaperBoxPalette_Savanna[] = INCBIN_U16("graphics/pokemon_storage/box_bg1.gbapal");
const u16 sWallpaperFramePalette_Savanna[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/savanna/frame.gbapal");
const u16 sWallpaperBgPalette_Savanna[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/savanna/bg.gbapal");

const u8 sWallpaperTiles_Savanna[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/savanna/tiles.4bpp.lz");

const u8 sWallpaperTilemap_Savanna[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/savanna/tilemap.bin.lz");

const u16 sWallpaperBoxPalette_Crag[] = INCBIN_U16("graphics/pokemon_storage/box_bg1.gbapal");
const u16 sWallpaperFramePalette_Crag[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/crag/frame.gbapal");
const u16 sWallpaperBgPalette_Crag[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/crag/bg.gbapal");

const u8 sWallpaperTiles_Crag[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/crag/tiles.4bpp.lz");

const u8 sWallpaperTilemap_Crag[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/crag/tilemap.bin.lz");

const u16 sWallpaperBoxPalette_Volcano[] = INCBIN_U16("graphics/pokemon_storage/box_bg1.gbapal");
const u16 sWallpaperFramePalette_Volcano[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/volcano/frame.gbapal");
const u16 sWallpaperBgPalette_Volcano[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/volcano/bg.gbapal");

const u8 sWallpaperTiles_Volcano[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/volcano/tiles.4bpp.lz");
const u32 sUnusedVolcanoTilemapPadding = 0; // needed to match but otherwise garbage

const u8 sWallpaperTilemap_Volcano[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/volcano/tilemap.bin.lz");

const u16 sWallpaperBoxPalette_Snow[] = INCBIN_U16("graphics/pokemon_storage/box_bg1.gbapal");
const u16 sWallpaperFramePalette_Snow[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/snow/frame.gbapal");
const u16 sWallpaperBgPalette_Snow[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/snow/bg.gbapal");

const u8 sWallpaperTiles_Snow[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/snow/tiles.4bpp.lz");

const u8 sWallpaperTilemap_Snow[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/snow/tilemap.bin.lz");

const u16 sWallpaperBoxPalette_Cave[] = INCBIN_U16("graphics/pokemon_storage/box_bg1.gbapal");
const u16 sWallpaperFramePalette_Cave[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/cave/frame.gbapal");
const u16 sWallpaperBgPalette_Cave[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/cave/bg.gbapal");

const u8 sWallpaperTiles_Cave[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/cave/tiles.4bpp.lz");

const u8 sWallpaperTilemap_Cave[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/cave/tilemap.bin.lz");

const u16 sWallpaperBoxPalette_Beach[] = INCBIN_U16("graphics/pokemon_storage/box_bg1.gbapal");
const u16 sWallpaperFramePalette_Beach[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/beach/frame.gbapal");
const u16 sWallpaperBgPalette_Beach[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/beach/bg.gbapal");

const u8 sWallpaperTiles_Beach[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/beach/tiles.4bpp.lz");

const u8 sWallpaperTilemap_Beach[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/beach/tilemap.bin.lz");

const u16 sWallpaperBoxPalette_Seafloor[] = INCBIN_U16("graphics/pokemon_storage/box_bg1.gbapal");
const u16 sWallpaperFramePalette_Seafloor[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/seafloor/frame.gbapal");
const u16 sWallpaperBgPalette_Seafloor[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/seafloor/bg.gbapal");

const u8 sWallpaperTiles_Seafloor[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/seafloor/tiles.4bpp.lz");

const u8 sWallpaperTilemap_Seafloor[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/seafloor/tilemap.bin.lz");

const u16 sWallpaperBoxPalette_River[] = INCBIN_U16("graphics/pokemon_storage/box_bg1.gbapal");
const u16 sWallpaperFramePalette_River[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/river/frame.gbapal");
const u16 sWallpaperBgPalette_River[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/river/bg.gbapal");

const u8 sWallpaperTiles_River[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/river/tiles.4bpp.lz");

const u8 sWallpaperTilemap_River[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/river/tilemap.bin.lz");

const u16 sWallpaperBoxPalette_Sky[] = INCBIN_U16("graphics/pokemon_storage/box_bg1.gbapal");
const u16 sWallpaperFramePalette_Sky[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/sky/frame.gbapal");
const u16 sWallpaperBgPalette_Sky[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/sky/bg.gbapal");

const u8 sWallpaperTiles_Sky[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/sky/tiles.4bpp.lz");

const u8 sWallpaperTilemap_Sky[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/sky/tilemap.bin.lz");

const u16 sWallpaperBoxPalette_Polkadot[] = INCBIN_U16("graphics/pokemon_storage/box_bg2.gbapal");
const u16 sWallpaperFramePalette_Polkadot[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/polkadot/frame.gbapal");
const u16 sWallpaperBgPalette_Polkadot[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/polkadot/bg.gbapal");

const u8 sWallpaperTiles_Polkadot[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/polkadot/tiles.4bpp.lz");

const u8 sWallpaperTilemap_Polkadot[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/polkadot/tilemap.bin.lz");

const u16 sWallpaperBoxPalette_Pokecenter[] = INCBIN_U16("graphics/pokemon_storage/box_bg2.gbapal");
const u16 sWallpaperFramePalette_Pokecenter[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/pokecenter/frame.gbapal");
const u16 sWallpaperBgPalette_Pokecenter[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/pokecenter/bg.gbapal");

const u8 sWallpaperTiles_Pokecenter[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/pokecenter/tiles.4bpp.lz");

const u8 sWallpaperTilemap_Pokecenter[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/pokecenter/tilemap.bin.lz");

const u16 sWallpaperBoxPalette_Machine[] = INCBIN_U16("graphics/pokemon_storage/box_bg3.gbapal");
const u16 sWallpaperFramePalette_Machine[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/machine/frame.gbapal");
const u16 sWallpaperBgPalette_Machine[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/machine/bg.gbapal");

const u8 sWallpaperTiles_Machine[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/machine/tiles.4bpp.lz");

const u8 sWallpaperTilemap_Machine[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/machine/tilemap.bin.lz");

const u16 sWallpaperBoxPalette_Plain[] = INCBIN_U16("graphics/pokemon_storage/box_bg4.gbapal");
const u16 sWallpaperFramePalette_Plain[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/plain/frame.gbapal");
const u16 sWallpaperBgPalette_Plain[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/plain/bg.gbapal");

const u8 sWallpaperTiles_Plain[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/plain/tiles.4bpp.lz");

const u8 sWallpaperTilemap_Plain[] = INCBIN_U8("graphics/pokemon_storage/wallpapers/plain/tilemap.bin.lz");

const u16 sUnusedWallpaperTilemap[] = INCBIN_U16("graphics/pokemon_storage/wallpapers/unused.bin");

const u16 sBoxTitleColors[][2] = {
    {0x1CE7, 0x7FFF},
    {0x1CE7, 0x7FFF},
    {0x1CE7, 0x7FFF},
    {0x1CE7, 0x7FFF},
    {0x1CE7, 0x7FFF},
    {0x1CE7, 0x7FFF},
    {0x1CE7, 0x7FFF},
    {0x1CE7, 0x7FFF},
    {0x1CE7, 0x7FFF},
    {0x1CE7, 0x7FFF},
    {0x1CE7, 0x7FFF},
    {0x1CE7, 0x7FFF},
    {0x1CE7, 0x7FFF},
    {0x1CE7, 0x7FFF},
    {0x1CE7, 0x7FFF},
    {0x1CE7, 0x7FFF}
};

const struct Wallpaper sWallpapers[] = {
    {sWallpaperTiles_Forest, 0x3D0, sWallpaperTilemap_Forest, sWallpaperBoxPalette_Forest}, // Forest
    {sWallpaperTiles_City, 0x208, sWallpaperTilemap_City, sWallpaperBoxPalette_City}, // City
    {sWallpaperTiles_Desert, 0x2EC, sWallpaperTilemap_Desert, sWallpaperBoxPalette_Desert}, // Desert
    {sWallpaperTiles_Savanna, 0x220, sWallpaperTilemap_Savanna, sWallpaperBoxPalette_Savanna}, // Savanna
    {sWallpaperTiles_Crag, 0x350, sWallpaperTilemap_Crag, sWallpaperBoxPalette_Crag}, // Crag
    {sWallpaperTiles_Volcano, 0x334, sWallpaperTilemap_Volcano, sWallpaperBoxPalette_Volcano}, // Volcano
    {sWallpaperTiles_Snow, 0x2B8, sWallpaperTilemap_Snow, sWallpaperBoxPalette_Snow}, // Snow
    {sWallpaperTiles_Cave, 0x344, sWallpaperTilemap_Cave, sWallpaperBoxPalette_Cave}, // Cave
    {sWallpaperTiles_Beach, 0x384, sWallpaperTilemap_Beach, sWallpaperBoxPalette_Beach}, // Beach
    {sWallpaperTiles_Seafloor, 0x2B4, sWallpaperTilemap_Seafloor, sWallpaperBoxPalette_Seafloor}, // Seafloor
    {sWallpaperTiles_River, 0x294, sWallpaperTilemap_River, sWallpaperBoxPalette_River}, // River
    {sWallpaperTiles_Sky, 0x298, sWallpaperTilemap_Sky, sWallpaperBoxPalette_Sky}, // Sky
    {sWallpaperTiles_Polkadot, 0x1FC, sWallpaperTilemap_Polkadot, sWallpaperBoxPalette_Polkadot}, // Polkadot
    {sWallpaperTiles_Pokecenter, 0x3A4, sWallpaperTilemap_Pokecenter, sWallpaperBoxPalette_Pokecenter}, // Pokecenter
    {sWallpaperTiles_Machine, 0x2F0, sWallpaperTilemap_Machine, sWallpaperBoxPalette_Machine}, // Machine
    {sWallpaperTiles_Plain, 0xFC, sWallpaperTilemap_Plain, sWallpaperBoxPalette_Plain} // Plain
};

const u16 sArrowPalette[] = INCBIN_U16("graphics/pokemon_storage/arrow.gbapal");
const u8 sArrowTiles[] = INCBIN_U8("graphics/pokemon_storage/arrow.4bpp");

const struct SpriteSheet sSpriteSheet_Arrow = {sArrowTiles, 0x80, 6};
const struct SpritePalette sSpritePalette_Arrow = {sArrowPalette, 0xdacf};

const struct OamData sOamData_BoxTitle = {
    .shape = ST_OAM_H_RECTANGLE,
    .size = 2,
    .priority = 2
};

const union AnimCmd sAnim_BoxTitleLeft[] = {
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END
};

const union AnimCmd sAnim_BoxTitleRight[] = {
    ANIMCMD_FRAME(8, 5),
    ANIMCMD_END
};

const union AnimCmd *const sAnims_BoxTitle[] = {
    sAnim_BoxTitleLeft,
    sAnim_BoxTitleRight
};

const struct SpriteTemplate sSpriteTemplate_BoxTitle = {
    3,
    0xdac8,
    &sOamData_BoxTitle,
    sAnims_BoxTitle,
    NULL,
    gDummySpriteAffineAnimTable,
    SpriteCallbackDummy
};

const struct OamData sOamData_Arrow = {
    .shape = ST_OAM_V_RECTANGLE,
    .priority = 2
};

const union AnimCmd sAnim_ArrowLeft[] = {
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END
};

const union AnimCmd sAnim_ArrowRight[] = {
    ANIMCMD_FRAME(2, 5),
    ANIMCMD_END
};

const union AnimCmd *const sAnims_Arrow[] = {
    sAnim_ArrowLeft,
    sAnim_ArrowRight
};

const struct SpriteTemplate sSpriteTemplate_Arrow = {
    6,
    0xdacf,
    &sOamData_Arrow,
    sAnims_Arrow,
    NULL,
    gDummySpriteAffineAnimTable,
    SpriteCB_Arrow
};

const u16 gHandCursorPalette[] = INCBIN_U16("graphics/pokemon_storage/hand_cursor.pal");
const u16 gHandCursorAltPalette[] = INCBIN_U16("graphics/pokemon_storage/hand_cursor_alt.pal");
const u8 gHandCursorTiles[] = INCBIN_U8("graphics/pokemon_storage/hand_cursor.4bpp");
const u8 gHandCursorShadowTiles[] = INCBIN_U8("graphics/pokemon_storage/hand_cursor_shadow.4bpp");

bool8 (*const sMonPlaceChangeFuncs[])(void) = {
    MonPlaceChange_Grab,
    MonPlaceChange_Place,
    MonPlaceChange_Shift
};


// .text

void InitPokeStorageBox(u8 boxId)
{
    gPokemonStorageSystemPtr->wallpaperOffset = FALSE;
    gPokemonStorageSystemPtr->bg2_X = 0;
    DmaFill32(3, 0, BG_SCREEN_ADDR(26), 0x1000);
    LoadWallpaperGfx(boxId, 0);
    InitBoxTitle(boxId);
    CreateBoxScrollArrows();
    InitBoxMonSprites(boxId);
    REG_BG2CNT = BGCNT_PRIORITY(2) | BGCNT_CHARBASE(2) | BGCNT_SCREENBASE(26) | BGCNT_TXT512x256;
}

void SetUpScrollToBox(u8 boxId)
{
    s8 direction = DetermineBoxScrollDirection(boxId);
    LoadWallpaperGfx(boxId, direction);
    gPokemonStorageSystemPtr->scrollSpeed = direction * 6;
    gPokemonStorageSystemPtr->scrollTimer = 0x20;
    gPokemonStorageSystemPtr->scrollToBoxIdUnused = boxId;
    gPokemonStorageSystemPtr->scrollUnused2 = direction > 0 ? 0 : 5;
    gPokemonStorageSystemPtr->scrollDirectionUnused = direction;
    gPokemonStorageSystemPtr->scrollUnused3 = direction > 0 ? 0x108 : 0x38;
    gPokemonStorageSystemPtr->scrollUnused4 = direction > 0 ? 0 : 5;
    gPokemonStorageSystemPtr->scrollUnused5 = direction;
    gPokemonStorageSystemPtr->scrollUnused6 = 0;
    gPokemonStorageSystemPtr->scrollUnused7 = 2;
    InitBoxMonIconScroll(boxId, direction);
    CreateIncomingBoxTitle(boxId, direction);
    StartBoxScrollArrowsSlide(direction);
}

bool8 ScrollToBox(void)
{
    bool8 retVal = UpdateBoxMonIconScroll();
    if (gPokemonStorageSystemPtr->scrollTimer != 0)
    {
        gPokemonStorageSystemPtr->bg2_X += gPokemonStorageSystemPtr->scrollSpeed;
        gPokemonStorageSystemPtr->bg2_X &= 0x1ff;
        if (--gPokemonStorageSystemPtr->scrollTimer == 0)
        {
            CycleBoxTitleSprites();
            StopBoxScrollArrowsSlide();
        }
        return TRUE;
    }
    return retVal;
}

s8 DetermineBoxScrollDirection(u8 boxId)
{
    u8 currentBox = StorageGetCurrentBox();
    u8 i;

    for (i = 0; currentBox != boxId; i++)
    {
        if (++currentBox >= TOTAL_BOXES_COUNT)
            currentBox = 0;
    }
    return i <= 6 ? 1 : -1;
}

void SetWallpaperForCurrentBox(u8 wallpaperId)
{
    u8 boxId = StorageGetCurrentBox();
    gPokemonStorage.wallpaper[boxId] = wallpaperId;
    gPokemonStorageSystemPtr->wallpaperChangeState = 0;
}

bool8 DoWallpaperGfxChange(void)
{
    switch (gPokemonStorageSystemPtr->wallpaperChangeState)
    {
        case 0:
            BeginNormalPaletteFade(gPokemonStorageSystemPtr->wallpaperPalBits, 1, 0, 16, FADE_COLOR_WHITE);
            gPokemonStorageSystemPtr->wallpaperChangeState++;
            break;
        case 1:
            if (!UpdatePaletteFade())
            {
                u8 curBox = StorageGetCurrentBox();
                LoadWallpaperGfx(curBox, 0);
                CycleBoxTitleColor();
                BeginNormalPaletteFade(gPokemonStorageSystemPtr->wallpaperPalBits, 1, 16, 0, FADE_COLOR_WHITE);
                gPokemonStorageSystemPtr->wallpaperChangeState++;
            }
            break;
        case 2:
            if (!UpdatePaletteFade())
                gPokemonStorageSystemPtr->wallpaperChangeState++;
            break;
        case 3:
            return FALSE;
    }
    return TRUE;
}

void LoadWallpaperGfx(u8 boxId, s8 direction)
{
    const struct Wallpaper *wallpaper;

    if (direction)
    {
        gPokemonStorageSystemPtr->wallpaperOffset = gPokemonStorageSystemPtr->wallpaperOffset ? FALSE : TRUE;
        TrimOldWallpaper(BG_SCREEN_ADDR(26));
    }
    wallpaper = sWallpapers + gPokemonStorage.wallpaper[boxId];
    LoadPalette(wallpaper->palettes, gPokemonStorageSystemPtr->wallpaperOffset * 0x30 + 0x40, 0x60);
    LZ77UnCompWram(wallpaper->tilemap, gPokemonStorageSystemPtr->wallpaperTilemap);
    DrawWallpaper(BG_SCREEN_ADDR(26), gPokemonStorageSystemPtr->wallpaperTilemap, direction, gPokemonStorageSystemPtr->wallpaperOffset);
    LZ77UnCompVram(wallpaper->tiles, BG_CHAR_ADDR(2) + (gPokemonStorageSystemPtr->wallpaperOffset << 13));
}

/*
    Copies the wallpaper rectangle tilemap to a buffer,
    depending on the wallpaper type.

    - buffer    : Where to copy the wallpaper to.
    - tilemap   : The wallpaper tilemap to copy.
    - direction : The direction of the wallpaper.
    - offset    : The offset of the wallpaper tilemap.

    Also depends on stored BG2 X variable in PSS.
*/
void DrawWallpaper(u16 *buffer, const u16 *tilemap, s8 direction, u8 offset)
{
    u16 *dest; // For either all of the wallpaper tilemap, or the first half of it.
    u16 *dest2; // The second half of wallpaper tilemap; used in case of two halves.
    u16 *fillDest; // For blanking; used when direction is set.

    u16 x, y; // Temps for copying the tilemap.

    // Temps for copying the tile and palette metadata of the wallpaper.
    u16 tileMeta;
    u16 tileOffset = offset * 256;
    u16 paletteMeta;
    u16 paletteOffset = (offset * 3 + 4) << 12;

    s16 rectWidth; // The width of the wallpaper, or it's first half.
    s16 rectWidth2; // Used in case of two halves.
    s16 rectX = ((gPokemonStorageSystemPtr->bg2_X / 8 + 10) + (direction * 24)) & 0x3F;

    s16 fillX; // For blanking; used when direction is set.

    /*
        Bull**** agbcc behavior discovered here
        while attempting to match this function:
        x + (y + z) is NOT the same as x + y + z.
        Even though it SHOULDN'T make a difference.

        Rather, it's the same as x + z + y. That's
        old codegen for you.
    */
    if (rectX < 13)
    {
        // Copy the wallpaper in full.
        rectWidth = 20;
        rectWidth2 = 0;
        dest = buffer + (rectX + 64);
        dest2 = NULL;
    }
    else if (rectX < 32)
    {
        // Split copying the wallpaper into two operations.
        rectWidth = 32 - rectX;
        rectWidth2 = 20 - rectWidth;
        dest = buffer + (rectX + 64);
        dest2 = buffer + 0x440;
    }
    else if (rectX < 45)
    {
        // Copy the wallpaper in full.
        rectWidth = 20;
        rectWidth2 = 0;
        dest = buffer + (rectX + 0x420);
        dest2 = NULL;
    }
    else
    {
        // Split copying the wallpaper into two operations.
        rectWidth = 64 - rectX;
        rectWidth2 = 20 - rectWidth;
        dest = buffer + (rectX + 0x420);
        dest2 = buffer + 0x40;
    }

    for (y = 0; y < 18; y++)
    {
        for(x = 0; x < rectWidth; x++)
        {
            // Get the tile and palette metadata for each
            // 2 bytes, and copy over into the buffer.
            tileMeta = (*tilemap & 0xfff) + tileOffset;
            paletteMeta =  (*tilemap & 0xf000) + paletteOffset;
            dest[x] = paletteMeta | tileMeta;
            tilemap++;
        }
        for(x = 0; x < rectWidth2; x++)
        {
            // Repeat the same process again in case we need
            // to copy halves.
            tileMeta = (*tilemap & 0xfff) + tileOffset;
            paletteMeta =  (*tilemap & 0xf000) + paletteOffset;
            dest2[x] = paletteMeta | tileMeta;
            tilemap++;
        }
        dest += 0x20;
        dest2 += 0x20;
    }

    if (direction != 0)
    {
        fillX = (direction > 0 ? rectX + 20 : rectX - 4) & 0x3F;

        if(fillX < 32)
            fillDest = buffer + (fillX + 0x40);
        else
            fillDest = buffer + (fillX + 0x420);

        for(x = 0; x < 4; x++)
        {
            for(y = 0; y < 18; y++)
            {
                *fillDest = 0; // Blank the buffer.
                fillDest += 0x20;
            }
            // Needs to be one statement, or else it won't match.
            // fillX = ++fillX & 0x3F; matches, but to have consistency
            // with TrimOldWallpaper, which requires var + 1
            // instead for a similar operation, we'll use that.
            fillX = (fillX + 1) & 0x3F;

            if (fillX < 32)
                fillDest = buffer + (fillX + 0x40);
            else
                fillDest = buffer + (fillX + 0x420);
        }
    }
}

/*
    Clears the lowest line of a wallpaper buffer.
    Depends on stored BG2 X variable in PSS.

    - dest   : Wallpaper buffer.
*/
void TrimOldWallpaper(u16 *dest)
{
    u16 x;
    s16 rectX = ((gPokemonStorageSystemPtr->bg2_X / 8) + 30) & 0x3F;

    if (rectX < 32)
        dest += rectX + 0x260;
    else
        dest += rectX + 0x640;

    for (x = 0; x < 44; x++)
    {
        *dest++ = 0;
        // Needs to be one statement in order to match.
        rectX = (rectX + 1) & 0x3F;
        if (rectX == 0)
            dest -= 0x420;
        if (rectX == 32)
            dest += 0x3e0;
    }
}

void DrawBoxTitleText(const u8 *tiles, const u8 *text)
{
    u8 *tileBuffer = gPokeStorageTileBuffer;
    DmaClear16(3, tileBuffer, 0x200);
    Text_InitWindow8004E3C(&gPSSBoxTitleWindowTemplate, tileBuffer, text);
    DmaCopy16(3, tileBuffer + 0x000, tiles + 0x000, 0x80);
    DmaCopy16(3, tileBuffer + 0x100, tiles + 0x080, 0x80);
    DmaCopy16(3, tileBuffer + 0x080, tiles + 0x100, 0x80);
    DmaCopy16(3, tileBuffer + 0x180, tiles + 0x180, 0x80);
}

const struct SpriteTemplate sSpriteTemplate_BoxTitle;

void InitBoxTitle(u8 boxId)
{
    u8 tagIndex;
    s16 baseX;
    u16 i;

    struct SpriteSheet spriteSheet = {gPokemonStorageSystemPtr->boxTitleTiles, 0x200, 3};
    struct SpritePalette palettes[] = {
        {gPokemonStorageSystemPtr->boxTitlePal, 0xdac8},
        {gPokemonStorageSystemPtr->boxTitlePal, 0xdac9},
        {}
    };

    u16 wallpaperId = gPokemonStorage.wallpaper[boxId];
    gPokemonStorageSystemPtr->boxTitlePal[14] = sBoxTitleColors[wallpaperId][0];
    gPokemonStorageSystemPtr->boxTitlePal[15] = sBoxTitleColors[wallpaperId][1];
    LoadSpritePalettes(palettes);
    gPokemonStorageSystemPtr->wallpaperPalBits = 0x3f0;
    tagIndex = IndexOfSpritePaletteTag(0xdac8);
    gPokemonStorageSystemPtr->boxTitlePalOffset = 0x10e + 16 * tagIndex;
    gPokemonStorageSystemPtr->wallpaperPalBits |= 0x10000 << tagIndex;
    tagIndex = IndexOfSpritePaletteTag(0xdac9);
    gPokemonStorageSystemPtr->boxTitleAltPalOffset = 0x10e + 16 * tagIndex;
    gPokemonStorageSystemPtr->wallpaperPalBits |= 0x10000 << tagIndex;
    DrawBoxTitleText(gPokemonStorageSystemPtr->boxTitleTiles, gPokemonStorage.boxNames[boxId]);
    LoadSpriteSheet(&spriteSheet);
    baseX = GetBoxTitleBaseX(GetStringWidthInMenuWindow(gPokemonStorage.boxNames[boxId]));
    for (i = 0; i < 2; i++)
    {
        u8 spriteId = CreateSprite(&sSpriteTemplate_BoxTitle, baseX + i * 32, 0x1c, 23);
        gPokemonStorageSystemPtr->curBoxTitleSprites[i] = gSprites + spriteId;
        StartSpriteAnim(gPokemonStorageSystemPtr->curBoxTitleSprites[i], i);
    }
    gPokemonStorageSystemPtr->boxTitleCycleId = 0;
}

// Sprite data for moving box-title text.
#define sSpeed         data[0]
#define sIncomingX     data[1]
#define sIncomingDelay data[2]
#define sOutgoingDelay data[1]
#define sOutgoingX     data[2]

void CreateIncomingBoxTitle(u8 boxId, s8 direction)
{
    u16 palOffset;
    s16 baseX;
    s16 incomingX;
    u16 i;
    struct SpriteSheet spriteSheet = {gPokemonStorageSystemPtr->boxTitleTiles, 0x200, 3};
    struct SpriteTemplate template = sSpriteTemplate_BoxTitle;

    gPokemonStorageSystemPtr->boxTitleCycleId = gPokemonStorageSystemPtr->boxTitleCycleId ? FALSE : TRUE;
    if (gPokemonStorageSystemPtr->boxTitleCycleId == 0)
    {
        spriteSheet.tag = 3;
        palOffset = gPokemonStorageSystemPtr->boxTitlePalOffset;
    }
    else
    {
        spriteSheet.tag = 4;
        palOffset = gPokemonStorageSystemPtr->boxTitlePalOffset;
        template.tileTag = 4;
        template.paletteTag = 0xdac9;
    }
    DrawBoxTitleText(gPokemonStorageSystemPtr->boxTitleTiles, gPokemonStorage.boxNames[boxId]);
    LoadSpriteSheet(&spriteSheet);
    LoadPalette(sBoxTitleColors[gPokemonStorage.wallpaper[boxId]], palOffset, 0x04);
    baseX = GetBoxTitleBaseX(GetStringWidthInMenuWindow(gPokemonStorage.boxNames[boxId]));
    incomingX = baseX + direction * 192;
    for (i = 0; i < 2; i++)
    {
        u8 spriteId = CreateSprite(&template, i * 32 + incomingX, 0x1c, 23);
        gPokemonStorageSystemPtr->nextBoxTitleSprites[i] = gSprites + spriteId;
        gPokemonStorageSystemPtr->nextBoxTitleSprites[i]->sSpeed = (-direction) * 6;
        gPokemonStorageSystemPtr->nextBoxTitleSprites[i]->sIncomingX = i * 32 + baseX;
        gPokemonStorageSystemPtr->nextBoxTitleSprites[i]->sIncomingDelay = 1;
        gPokemonStorageSystemPtr->nextBoxTitleSprites[i]->callback = SpriteCB_IncomingBoxTitle;
        StartSpriteAnim(gPokemonStorageSystemPtr->nextBoxTitleSprites[i], i);
        gPokemonStorageSystemPtr->curBoxTitleSprites[i]->sSpeed = (-direction) * 6;
        gPokemonStorageSystemPtr->curBoxTitleSprites[i]->sOutgoingDelay = 1;
        gPokemonStorageSystemPtr->curBoxTitleSprites[i]->callback = SpriteCB_OutgoingBoxTitle;
    }
}

void CycleBoxTitleSprites(void)
{
    if (gPokemonStorageSystemPtr->boxTitleCycleId == 0)
        FreeSpriteTilesByTag(4);
    else
        FreeSpriteTilesByTag(3);
    gPokemonStorageSystemPtr->curBoxTitleSprites[0] = gPokemonStorageSystemPtr->nextBoxTitleSprites[0];
    gPokemonStorageSystemPtr->curBoxTitleSprites[1] = gPokemonStorageSystemPtr->nextBoxTitleSprites[1];
}

void SpriteCB_IncomingBoxTitle(struct Sprite *sprite)
{
    if (sprite->sIncomingDelay)
        sprite->sIncomingDelay--;
    else if ((sprite->x += sprite->sSpeed) == sprite->sIncomingX)
        sprite->callback = SpriteCallbackDummy;
}

void SpriteCB_OutgoingBoxTitle(struct Sprite *sprite)
{
    if (sprite->sOutgoingDelay)
        sprite->sOutgoingDelay--;
    else
    {
        sprite->x += sprite->sSpeed;
        sprite->sOutgoingX = sprite->x + sprite->x2;
        if (sprite->sOutgoingX < 0x40 || sprite->sOutgoingX > 0x100)
            DestroySprite(sprite);
    }
}

#undef sSpeed
#undef sIncomingX
#undef sIncomingDelay
#undef sOutgoingDelay
#undef sOutgoingX

void CycleBoxTitleColor(void)
{
    u8 boxId = StorageGetCurrentBox();
    u8 wallpaperId = gPokemonStorage.wallpaper[boxId];
    if (gPokemonStorageSystemPtr->boxTitleCycleId == 0)
        CpuCopy16(sBoxTitleColors[wallpaperId], gPlttBufferUnfaded + gPokemonStorageSystemPtr->boxTitlePalOffset, 4);
    else
        CpuCopy16(sBoxTitleColors[wallpaperId], gPlttBufferUnfaded + gPokemonStorageSystemPtr->boxTitleAltPalOffset, 4);
}

s16 GetBoxTitleBaseX(u8 width)
{
    return 0xb0 - width / 2;
}

// Sprite data for box scroll arrows.
#define sState data[0]
#define sTimer data[1]
#define sSpeed data[3]

void CreateBoxScrollArrows(void)
{
    u16 i;

    LoadSpriteSheet(&sSpriteSheet_Arrow);
    LoadSpritePalette(&sSpritePalette_Arrow);
    for (i = 0; i < 2; i++)
    {
        u8 spriteId = CreateSprite(&sSpriteTemplate_Arrow, 0x5c + i * 0x88, 0x1c, 21);
        if (spriteId != MAX_SPRITES)
        {
            struct Sprite *sprite = gSprites + spriteId;
            StartSpriteAnim(sprite, i);
            sprite->sSpeed = (i == 0) ? -1 : 1;
            gPokemonStorageSystemPtr->arrowSprites[i] = sprite;
        }
    }
    if (IsCursorOnBoxTitle())
        AnimateBoxScrollArrows(TRUE);
}

void StartBoxScrollArrowsSlide(s8 direction)
{
    u16 i;

    for (i = 0; i < 2; i++)
    {
        gPokemonStorageSystemPtr->arrowSprites[i]->x2 = 0;
        gPokemonStorageSystemPtr->arrowSprites[i]->sState = 2;
    }
    if (direction < 0)
    {
        gPokemonStorageSystemPtr->arrowSprites[0]->sTimer = 29;
        gPokemonStorageSystemPtr->arrowSprites[1]->sTimer = 5;
        gPokemonStorageSystemPtr->arrowSprites[0]->data[2] = 0x48;
        gPokemonStorageSystemPtr->arrowSprites[1]->data[2] = 0x48;
    }
    else
    {
        gPokemonStorageSystemPtr->arrowSprites[0]->sTimer = 5;
        gPokemonStorageSystemPtr->arrowSprites[1]->sTimer = 29;
        gPokemonStorageSystemPtr->arrowSprites[0]->data[2] = 0xF8;
        gPokemonStorageSystemPtr->arrowSprites[1]->data[2] = 0xF8;
    }
    gPokemonStorageSystemPtr->arrowSprites[0]->data[7] = 0;
    gPokemonStorageSystemPtr->arrowSprites[1]->data[7] = 1;
}

void StopBoxScrollArrowsSlide(void)
{
    u16 i;

    for (i = 0; i < 2; i++)
    {
        gPokemonStorageSystemPtr->arrowSprites[i]->x = 0x88 * i + 0x5c;
        gPokemonStorageSystemPtr->arrowSprites[i]->x2 = 0;
        gPokemonStorageSystemPtr->arrowSprites[i]->invisible = FALSE;
    }
    AnimateBoxScrollArrows(TRUE);
}

void AnimateBoxScrollArrows(bool8 animate)
{
    u16 i;

    if (animate)
    {
        for (i = 0; i < 2; i++)
        {
            gPokemonStorageSystemPtr->arrowSprites[i]->sState = 1;
            gPokemonStorageSystemPtr->arrowSprites[i]->sTimer = 0;
            gPokemonStorageSystemPtr->arrowSprites[i]->data[2] = 0;
            gPokemonStorageSystemPtr->arrowSprites[i]->data[4] = 0;
        }
    }
    else
    {
        for (i = 0; i < 2; i++)
            gPokemonStorageSystemPtr->arrowSprites[i]->sState = 0;
    }
}

void SpriteCB_Arrow(struct Sprite *sprite)
{
    switch (sprite->sState)
    {
        case 0:
            sprite->x2 = 0;
            break;
        case 1:
            if (++sprite->sTimer > 3)
            {
                sprite->sTimer = 0;
                sprite->x2 += sprite->sSpeed;
                if (++sprite->data[2] > 5)
                {
                    sprite->data[2] = 0;
                    sprite->x2 = 0;
                }
            }
            break;
        case 2:
            sprite->sState = 3;
            break;
        case 3:
            sprite->x -= gPokemonStorageSystemPtr->scrollSpeed;
            if (sprite->x < 0x49 || sprite->x > 0xf7)
                sprite->invisible = TRUE;
            if (--sprite->sTimer == 0)
            {
                sprite->x = sprite->data[2];
                sprite->invisible = FALSE;
                sprite->sState = 4;
            }
            break;
        case 4:
            sprite->x -= gPokemonStorageSystemPtr->scrollSpeed;
            break;
    }
}

#undef sState
#undef sTimer
#undef sSpeed

struct Sprite *CreateChooseBoxArrows(u16 x, u16 y, u8 animId, u8 priority, u8 subpriority)
{
    u8 spriteId = CreateSprite(&sSpriteTemplate_Arrow, x, y, subpriority);
    if (spriteId == MAX_SPRITES)
        return NULL;
    animId %= 2;
    StartSpriteAnim(gSprites + spriteId, animId);
    gSprites[spriteId].oam.priority = priority;
    gSprites[spriteId].callback = SpriteCallbackDummy;
    return gSprites + spriteId;
}

void InitCursor(void)
{
    if (gPokemonStorageSystemPtr->boxOption != PSS_OPTION_DEPOSIT)
        sCursorArea = PSS_CURSOR_AREA_IN_BOX;
    else
        sCursorArea = PSS_CURSOR_AREA_IN_PARTY;
    sCursorPosition = 0;
    sIsMonBeingMoved = FALSE;
    sMovingMonOrigBoxId = 0;
    sMovingMonOrigBoxPos = 0;
    sAutoActionOn = 0;
    ClearSavedCursorPos();
    CreateCursorSprites();
    gPokemonStorageSystemPtr->cursorPrevHorizPos = 1;
    TryRefreshDisplayMon();
}

void InitCursorOnReopen(void)
{
    CreateCursorSprites();
    ReshowDisplayMon();
    gPokemonStorageSystemPtr->cursorPrevHorizPos = 1;
    if (sIsMonBeingMoved)
        CreateMovingMonIcon();
}

void GetCursorCoordsByPos(u8 cursorArea, u8 cursorPosition, u16 *x, u16 *y)
{
    switch (cursorArea)
    {
        case PSS_CURSOR_AREA_IN_BOX:
            *x = (cursorPosition % 6) * 24 + 100;
            *y = (cursorPosition / 6) * 24 +  32;
            break;
        case PSS_CURSOR_AREA_IN_PARTY:
            if (cursorPosition == 0)
            {
                *x = 0x68;
                *y = 0x34;
            }
            else if (cursorPosition == PARTY_SIZE)
            {
                *x = 0x98;
                *y = 0x84;
            }
            else
            {
                *x = 0x98;
                *y = (cursorPosition - 1) * 24 + 4;
            }
            break;
        case PSS_CURSOR_AREA_BOX_TITLE:
            *x = 0xa2;
            *y = 0x0c;
            break;
        case PSS_CURSOR_AREA_BUTTONS:
            *y = sIsMonBeingMoved ? 8 : 14;
            *x = cursorPosition * 0x58 + 0x78;
            break;
        case 4:
            *x = 0xa0;
            *y = 0x60;
            break;
    }
}

u16 GetSpeciesAtCursorPosition(void)
{
    switch (sCursorArea)
    {
        case PSS_CURSOR_AREA_IN_PARTY:
            return GetMonData(gPlayerParty + sCursorPosition, MON_DATA_SPECIES);
        case PSS_CURSOR_AREA_IN_BOX:
            return GetBoxMonData(gPokemonStorage.boxes[StorageGetCurrentBox()] + sCursorPosition, MON_DATA_SPECIES);
        default:
            return SPECIES_NONE;
    }
}

bool8 UpdateCursorPos(void)
{
    s16 tmp;
    if (gPokemonStorageSystemPtr->cursorMoveSteps == 0)
        return FALSE;
    if (--gPokemonStorageSystemPtr->cursorMoveSteps)
    {
        gPokemonStorageSystemPtr->cursorNewX += gPokemonStorageSystemPtr->cursorSpeedX;
        gPokemonStorageSystemPtr->cursorNewY += gPokemonStorageSystemPtr->cursorSpeedY;
        gPokemonStorageSystemPtr->cursorSprite->x = gPokemonStorageSystemPtr->cursorNewX >> 8;
        gPokemonStorageSystemPtr->cursorSprite->y = gPokemonStorageSystemPtr->cursorNewY >> 8;
        if (gPokemonStorageSystemPtr->cursorSprite->x > 0x100)
        {
            tmp = gPokemonStorageSystemPtr->cursorSprite->x - 0x100;
            gPokemonStorageSystemPtr->cursorSprite->x = tmp + 0x40;
        }
        if (gPokemonStorageSystemPtr->cursorSprite->x < 0x40)
        {
            tmp = 0x40 - gPokemonStorageSystemPtr->cursorSprite->x;
            gPokemonStorageSystemPtr->cursorSprite->x = 0x100 - tmp;
        }
        if (gPokemonStorageSystemPtr->cursorSprite->y > 0xb0)
        {
            tmp = gPokemonStorageSystemPtr->cursorSprite->y - 0xb0;
            gPokemonStorageSystemPtr->cursorSprite->y = tmp - 0x10;
        }
        if (gPokemonStorageSystemPtr->cursorSprite->y < -0x10)
        {
            tmp = -0x10 - gPokemonStorageSystemPtr->cursorSprite->y;
            gPokemonStorageSystemPtr->cursorSprite->y = 0xb0 - tmp;
        }
        if (gPokemonStorageSystemPtr->cursorFlipTimer && --gPokemonStorageSystemPtr->cursorFlipTimer == 0)
            gPokemonStorageSystemPtr->cursorSprite->vFlip = gPokemonStorageSystemPtr->cursorSprite->vFlip ? FALSE : TRUE;
    }
    else
    {
        gPokemonStorageSystemPtr->cursorSprite->x = gPokemonStorageSystemPtr->cursorTargetX;
        gPokemonStorageSystemPtr->cursorSprite->y = gPokemonStorageSystemPtr->cursorTargetY;
        DoCursorNewPosUpdate();
    }
    return TRUE;
}

void InitNewCursorPos(u8 newCursorArea, u8 newCursorPosition)
{
    u16 x;
    u16 y;

    GetCursorCoordsByPos(newCursorArea, newCursorPosition, &x, &y);
    gPokemonStorageSystemPtr->newCursorArea = newCursorArea;
    gPokemonStorageSystemPtr->newCursorPosition = newCursorPosition;
    gPokemonStorageSystemPtr->cursorTargetX = x;
    gPokemonStorageSystemPtr->cursorTargetY = y;
}

void InitCursorMove(void)
{
    int yDistance;
    int xDistance;

    if (gPokemonStorageSystemPtr->cursorVerticalWrap || gPokemonStorageSystemPtr->cursorHorizontalWrap)
        gPokemonStorageSystemPtr->cursorMoveSteps = 12;
    else
        gPokemonStorageSystemPtr->cursorMoveSteps = 6;
    if (gPokemonStorageSystemPtr->cursorFlipTimer)
        gPokemonStorageSystemPtr->cursorFlipTimer = gPokemonStorageSystemPtr->cursorMoveSteps >> 1;
    switch (gPokemonStorageSystemPtr->cursorVerticalWrap)
    {
        default:
            yDistance = gPokemonStorageSystemPtr->cursorTargetY - gPokemonStorageSystemPtr->cursorSprite->y;
            break;
        case -1:
            yDistance = gPokemonStorageSystemPtr->cursorTargetY - 0xc0 - gPokemonStorageSystemPtr->cursorSprite->y;
            break;
        case 1:
            yDistance = gPokemonStorageSystemPtr->cursorTargetY + 0xc0 - gPokemonStorageSystemPtr->cursorSprite->y;
            break;
    }
    switch (gPokemonStorageSystemPtr->cursorHorizontalWrap)
    {
        default:
            xDistance = gPokemonStorageSystemPtr->cursorTargetX - gPokemonStorageSystemPtr->cursorSprite->x;
            break;
        case -1:
            xDistance = gPokemonStorageSystemPtr->cursorTargetX - 0xc0 - gPokemonStorageSystemPtr->cursorSprite->x;
            break;
        case 1:
            xDistance = gPokemonStorageSystemPtr->cursorTargetX + 0xc0 - gPokemonStorageSystemPtr->cursorSprite->x;
            break;
    }
    yDistance <<= 8;
    xDistance <<= 8;
    gPokemonStorageSystemPtr->cursorSpeedX = xDistance / gPokemonStorageSystemPtr->cursorMoveSteps;
    gPokemonStorageSystemPtr->cursorSpeedY = yDistance / gPokemonStorageSystemPtr->cursorMoveSteps;
    gPokemonStorageSystemPtr->cursorNewX = gPokemonStorageSystemPtr->cursorSprite->x << 8;
    gPokemonStorageSystemPtr->cursorNewY = gPokemonStorageSystemPtr->cursorSprite->y << 8;
}

void SetCursorPosition(u8 newCursorArea, u8 newCursorPosition)
{
    InitNewCursorPos(newCursorArea, newCursorPosition);
    InitCursorMove();
    if (!sIsMonBeingMoved)
        StartSpriteAnim(gPokemonStorageSystemPtr->cursorSprite, PSS_CURSOR_ANIM_STILL);
    if (newCursorArea == PSS_CURSOR_AREA_IN_PARTY && sCursorArea != PSS_CURSOR_AREA_IN_PARTY)
    {
        gPokemonStorageSystemPtr->cursorPrevHorizPos = newCursorArea;
        gPokemonStorageSystemPtr->cursorShadowSprite->invisible = TRUE;
    }
    switch (newCursorArea)
    {
        case PSS_CURSOR_AREA_IN_BOX:
            break;
        case PSS_CURSOR_AREA_IN_PARTY ... PSS_CURSOR_AREA_BUTTONS:
            gPokemonStorageSystemPtr->cursorShadowSprite->invisible = TRUE;
            gPokemonStorageSystemPtr->cursorShadowSprite->oam.priority = 1;
            break;
    }
}

void DoCursorNewPosUpdate(void)
{
    sCursorArea = gPokemonStorageSystemPtr->newCursorArea;
    sCursorPosition = gPokemonStorageSystemPtr->newCursorPosition;
    if (!sIsMonBeingMoved)
        StartSpriteAnim(gPokemonStorageSystemPtr->cursorSprite, PSS_CURSOR_ANIM_BOUNCE);
    TryRefreshDisplayMon();
    switch (sCursorArea)
    {
        case PSS_CURSOR_AREA_BOX_TITLE:
            AnimateBoxScrollArrows(TRUE);
            break;
        case PSS_CURSOR_AREA_IN_PARTY:
            gPokemonStorageSystemPtr->cursorShadowSprite->subpriority = 12;
            break;
        case PSS_CURSOR_AREA_IN_BOX:
            gPokemonStorageSystemPtr->cursorShadowSprite->oam.priority = 2;
            gPokemonStorageSystemPtr->cursorShadowSprite->subpriority = 20;
            gPokemonStorageSystemPtr->cursorShadowSprite->invisible = FALSE;
            break;
    }
}

void SetCursorInParty(void)
{
    u8 partyCount;
    if (!sIsMonBeingMoved)
        partyCount = 0;
    else
    {
        partyCount = CalculatePlayerPartyCount();
        if (partyCount > PARTY_SIZE - 1)
            partyCount = PARTY_SIZE - 1;
    }
    if (gPokemonStorageSystemPtr->cursorSprite->vFlip)
        gPokemonStorageSystemPtr->cursorFlipTimer = 1;
    SetCursorPosition(PSS_CURSOR_AREA_IN_PARTY, partyCount);
}

void SetCursorBoxPosition(u8 cursorBoxPosition)
{
    SetCursorPosition(PSS_CURSOR_AREA_IN_BOX, cursorBoxPosition);
}

void ClearSavedCursorPos(void)
{
    sSavedCursorPosition = 0;
}

void SaveCursorPos(void)
{
    sSavedCursorPosition = sCursorPosition;
}

u8 GetSavedCursorPos(void)
{
    return sSavedCursorPosition;
}

void InitMonPlaceChange(u8 type)
{
    gPokemonStorageSystemPtr->monPlaceChangeFunc = sMonPlaceChangeFuncs[type];
    gPokemonStorageSystemPtr->monPlaceChangeState = 0;
}

bool8 DoMonPlaceChange(void)
{
    return gPokemonStorageSystemPtr->monPlaceChangeFunc();
}

bool8 MonPlaceChange_Grab(void)
{
    switch (gPokemonStorageSystemPtr->monPlaceChangeState)
    {
        case 0:
            if (sIsMonBeingMoved)
                return FALSE;
            StartSpriteAnim(gPokemonStorageSystemPtr->cursorSprite, PSS_CURSOR_ANIM_OPEN);
            gPokemonStorageSystemPtr->monPlaceChangeState++;
            break;
        case 1:
            if (!MonPlaceChange_CursorDown())
            {
                StartSpriteAnim(gPokemonStorageSystemPtr->cursorSprite, PSS_CURSOR_ANIM_FIST);
                MoveMon();
                gPokemonStorageSystemPtr->monPlaceChangeState++;
            }
            break;
        case 2:
            if (!MonPlaceChange_CursorUp())
                gPokemonStorageSystemPtr->monPlaceChangeState++;
            break;
        case 3:
            return FALSE;
    }
    return TRUE;
}

bool8 MonPlaceChange_Place(void)
{
    switch (gPokemonStorageSystemPtr->monPlaceChangeState)
    {
        case 0:
            if (!MonPlaceChange_CursorDown())
            {
                StartSpriteAnim(gPokemonStorageSystemPtr->cursorSprite, PSS_CURSOR_ANIM_OPEN);
                PlaceMon();
                gPokemonStorageSystemPtr->monPlaceChangeState++;
            }
            break;
        case 1:
            if (!MonPlaceChange_CursorUp())
            {
                StartSpriteAnim(gPokemonStorageSystemPtr->cursorSprite, PSS_CURSOR_ANIM_BOUNCE);
                gPokemonStorageSystemPtr->monPlaceChangeState++;
            }
            break;
        case 2:
            return FALSE;
    }
    return TRUE;
}

bool8 MonPlaceChange_Shift(void)
{
    switch (gPokemonStorageSystemPtr->monPlaceChangeState)
    {
        case 0:
            switch (sCursorArea)
            {
                case PSS_CURSOR_AREA_IN_PARTY:
                    gPokemonStorageSystemPtr->shiftBoxId = TOTAL_BOXES_COUNT;
                    break;
                case PSS_CURSOR_AREA_IN_BOX:
                    gPokemonStorageSystemPtr->shiftBoxId = StorageGetCurrentBox();
                    break;
                default:
                    return FALSE;
            }
            StartSpriteAnim(gPokemonStorageSystemPtr->cursorSprite, PSS_CURSOR_ANIM_OPEN);
            SaveMonSpriteAtPos(gPokemonStorageSystemPtr->shiftBoxId, sCursorPosition);
            gPokemonStorageSystemPtr->monPlaceChangeState++;
            break;
        case 1:
            if (!MoveShiftingMons())
            {
                StartSpriteAnim(gPokemonStorageSystemPtr->cursorSprite, PSS_CURSOR_ANIM_FIST);
                SetShiftedMonData(gPokemonStorageSystemPtr->shiftBoxId, sCursorPosition);
                gPokemonStorageSystemPtr->monPlaceChangeState++;
            }
            break;
        case 2:
            return FALSE;
    }
    return TRUE;
}

bool8 MonPlaceChange_CursorDown(void)
{
    switch (gPokemonStorageSystemPtr->cursorSprite->y2)
    {
        default:
            gPokemonStorageSystemPtr->cursorSprite->y2++;
            break;
        case 0:
            gPokemonStorageSystemPtr->cursorSprite->y2++;
            break;
        case 8:
            return FALSE;
    }
    return TRUE;
}

bool8 MonPlaceChange_CursorUp(void)
{
    switch (gPokemonStorageSystemPtr->cursorSprite->y2)
    {
        case 0:
            return FALSE;
        default:
            gPokemonStorageSystemPtr->cursorSprite->y2--;
            break;
    }
    return TRUE;
}

void MoveMon(void)
{
    switch (sCursorArea)
    {
        case PSS_CURSOR_AREA_IN_PARTY:
            SetMovingMonData(TOTAL_BOXES_COUNT, sCursorPosition);
            SetMovingMonSprite(PSS_MODE_PARTY, sCursorPosition);
            break;
        case PSS_CURSOR_AREA_IN_BOX:
            SetMovingMonData(StorageGetCurrentBox(), sCursorPosition);
            SetMovingMonSprite(PSS_MODE_BOX, sCursorPosition);
            break;
        default:
            return;
    }
    sIsMonBeingMoved = TRUE;
}

void PlaceMon(void)
{
    u8 boxId;
    switch (sCursorArea)
    {
        case PSS_CURSOR_AREA_IN_PARTY:
            SetPlacedMonData(TOTAL_BOXES_COUNT, sCursorPosition);
            SetPlacedMonSprite(TOTAL_BOXES_COUNT, sCursorPosition);
            break;
        case PSS_CURSOR_AREA_IN_BOX:
            boxId = StorageGetCurrentBox();
            SetPlacedMonData(boxId, sCursorPosition);
            SetPlacedMonSprite(boxId, sCursorPosition);
            break;
        default:
            return;
    }
    sIsMonBeingMoved = FALSE;
}

void RefreshDisplayMon(void)
{
    TryRefreshDisplayMon();
}

void SetMovingMonData(u8 boxId, u8 position)
{
    if (boxId == TOTAL_BOXES_COUNT)
        gPokemonStorageSystemPtr->movingMon = gPlayerParty[sCursorPosition];
    else
        BoxMonToMon(gPokemonStorage.boxes[boxId] + position, &gPokemonStorageSystemPtr->movingMon);
    PurgeMonOrBoxMon(boxId, position);
    sMovingMonOrigBoxId = boxId;
    sMovingMonOrigBoxPos = position;
}

void SetPlacedMonData(u8 boxId, u8 position)
{
    if (boxId == TOTAL_BOXES_COUNT)
    {
        gPlayerParty[position] = gPokemonStorageSystemPtr->movingMon;
    }
    else
    {
        BoxMonRestorePP(&gPokemonStorageSystemPtr->movingMon.box);
        gPokemonStorage.boxes[boxId][position] = gPokemonStorageSystemPtr->movingMon.box;
    }
}

void PurgeMonOrBoxMon(u8 boxId, u8 position)
{
    if (boxId == TOTAL_BOXES_COUNT)
        ZeroMonData(gPlayerParty + position);
    else
        ZeroBoxMonData(gPokemonStorage.boxes[boxId] + position);
}

void SetShiftedMonData(u8 boxId, u8 position)
{
    if (boxId == TOTAL_BOXES_COUNT)
        gPokemonStorageSystemPtr->tempMon = gPlayerParty[position];
    else
        BoxMonToMon(gPokemonStorage.boxes[boxId] + position, &gPokemonStorageSystemPtr->tempMon);
    SetPlacedMonData(boxId, position);
    gPokemonStorageSystemPtr->movingMon = gPokemonStorageSystemPtr->tempMon;
    SetDisplayMonData(&gPokemonStorageSystemPtr->movingMon, PSS_MODE_PARTY);
    sMovingMonOrigBoxId = boxId;
    sMovingMonOrigBoxPos = position;
}

bool8 TryStorePartyMonInBox(u8 boxId)
{
    s16 boxPosition = GetFirstFreeBoxSpot(boxId);
    if (boxPosition == -1)
        return FALSE;
    if (sIsMonBeingMoved)
    {
        SetPlacedMonData(boxId, boxPosition);
        DestroyMovingMonIcon();
        sIsMonBeingMoved = FALSE;
    }
    else
    {
        SetMovingMonData(TOTAL_BOXES_COUNT, sCursorPosition);
        SetPlacedMonData(boxId, boxPosition);
        DestroyPartyMonIcon(sCursorPosition);
    }
    if (boxId == StorageGetCurrentBox())
        CreateBoxMonIconAtPos(boxPosition);
    StartSpriteAnim(gPokemonStorageSystemPtr->cursorSprite, PSS_CURSOR_ANIM_STILL);
    return TRUE;
}

void ResetSelectionAfterDeposit(void)
{
    StartSpriteAnim(gPokemonStorageSystemPtr->cursorSprite, PSS_CURSOR_ANIM_BOUNCE);
    TryRefreshDisplayMon();
}

void InitReleaseMon(void)
{
    u8 mode;

    if (sIsMonBeingMoved)
        mode = PSS_MODE_MOVE;
    else if (sCursorArea == PSS_CURSOR_AREA_IN_PARTY)
        mode = PSS_MODE_PARTY;
    else
        mode = PSS_MODE_BOX;
    SetReleaseMon(mode, sCursorPosition);
    StringCopy(gPokemonStorageSystemPtr->releaseMonName, gPokemonStorageSystemPtr->displayMonName);
}

bool8 TryHideReleaseMon(void)
{
    if (!TryHideReleaseMonSprite())
    {
        StartSpriteAnim(gPokemonStorageSystemPtr->cursorSprite, PSS_CURSOR_ANIM_BOUNCE);
        return FALSE;
    }
    return TRUE;
}

void ReleaseMon(void)
{
    DestroyReleaseMonIcon();
    if (sIsMonBeingMoved)
        sIsMonBeingMoved = FALSE;
    else
    {
        u8 boxId;
        if (sCursorArea == PSS_CURSOR_AREA_IN_PARTY)
            boxId = TOTAL_BOXES_COUNT;
        else
            boxId = StorageGetCurrentBox();
        PurgeMonOrBoxMon(boxId, sCursorPosition);
    }
    TryRefreshDisplayMon();
}

void TrySetCursorFistAnim(void)
{
    if (sIsMonBeingMoved)
        StartSpriteAnim(gPokemonStorageSystemPtr->cursorSprite, PSS_CURSOR_ANIM_FIST);
}

void InitCanReleaseMonVars(void)
{
    u16 knownMoves;
    if (sIsMonBeingMoved)
    {
        gPokemonStorageSystemPtr->tempMon = gPokemonStorageSystemPtr->movingMon;
        gPokemonStorageSystemPtr->releaseBoxId = -1;
        gPokemonStorageSystemPtr->releaseBoxPos = -1;
    }
    else
    {
        if (sCursorArea == PSS_CURSOR_AREA_IN_PARTY)
        {
            gPokemonStorageSystemPtr->tempMon = gPlayerParty[sCursorPosition];
            gPokemonStorageSystemPtr->releaseBoxId = TOTAL_BOXES_COUNT;
        }
        else
        {
            BoxMonToMon(gPokemonStorage.boxes[gPokemonStorage.currentBox] + sCursorPosition, &gPokemonStorageSystemPtr->tempMon);
            gPokemonStorageSystemPtr->releaseBoxId = gPokemonStorage.currentBox;
        }
        gPokemonStorageSystemPtr->releaseBoxPos = sCursorPosition;
    }
    gPokemonStorageSystemPtr->unresolvedReleaseSurf = 0;
    gPokemonStorageSystemPtr->unresolvedReleaseDive = 0;
    gPokemonStorageSystemPtr->restrictedMoveList[0] = MOVE_SURF;
    gPokemonStorageSystemPtr->restrictedMoveList[1] = MOVE_DIVE;
    gPokemonStorageSystemPtr->restrictedMoveList[2] = MOVES_COUNT;
    knownMoves = GetMonData(&gPokemonStorageSystemPtr->tempMon, MON_DATA_KNOWN_MOVES, gPokemonStorageSystemPtr->restrictedMoveList);
    gPokemonStorageSystemPtr->unresolvedReleaseSurf = knownMoves & 1;
    gPokemonStorageSystemPtr->unresolvedReleaseDive = (knownMoves >> 1) & 1;
    if (gPokemonStorageSystemPtr->unresolvedReleaseSurf || gPokemonStorageSystemPtr->unresolvedReleaseDive)
        gPokemonStorageSystemPtr->releaseStatusResolved = 0;
    else
    {
        gPokemonStorageSystemPtr->releaseStatusResolved = 1;
        gPokemonStorageSystemPtr->canReleaseMon = 1;
    }
    gPokemonStorageSystemPtr->releaseCheckState = 0;
}

s8 RunCanReleaseMon(void)
{
    u16 i;
    u16 knownMoves;

    if (gPokemonStorageSystemPtr->releaseStatusResolved)
        return gPokemonStorageSystemPtr->canReleaseMon;
    switch (gPokemonStorageSystemPtr->releaseCheckState)
    {
        case 0:
            for (i = 0; i < PARTY_SIZE; i++)
            {
                if (gPokemonStorageSystemPtr->releaseBoxId != TOTAL_BOXES_COUNT || gPokemonStorageSystemPtr->releaseBoxPos != i)
                {
                    knownMoves = GetMonData(gPlayerParty + i, MON_DATA_KNOWN_MOVES, gPokemonStorageSystemPtr->restrictedMoveList);
                    if (knownMoves & 1)
                        gPokemonStorageSystemPtr->unresolvedReleaseSurf = 0;
                    if (knownMoves & 2)
                        gPokemonStorageSystemPtr->unresolvedReleaseDive = 0;
                }
            }
            if (gPokemonStorageSystemPtr->unresolvedReleaseSurf == 0 && gPokemonStorageSystemPtr->unresolvedReleaseDive == 0)
            {
                gPokemonStorageSystemPtr->releaseStatusResolved = 1;
                gPokemonStorageSystemPtr->canReleaseMon = 1;
            }
            else
            {
                gPokemonStorageSystemPtr->releaseCheckBoxId = 0;
                gPokemonStorageSystemPtr->releaseCheckBoxPos = 0;
                gPokemonStorageSystemPtr->releaseCheckState++;
            }
            break;
        case 1:
            for (i = 0; i < 5; i++)
            {
                knownMoves = GetBoxMonData(gPokemonStorage.boxes[gPokemonStorageSystemPtr->releaseCheckBoxId] + gPokemonStorageSystemPtr->releaseCheckBoxPos, MON_DATA_KNOWN_MOVES, gPokemonStorageSystemPtr->restrictedMoveList);
                if (knownMoves && (gPokemonStorageSystemPtr->releaseBoxId != gPokemonStorageSystemPtr->releaseCheckBoxId || gPokemonStorageSystemPtr->releaseBoxPos != gPokemonStorageSystemPtr->releaseCheckBoxPos))
                {
                    if (knownMoves & 1)
                        gPokemonStorageSystemPtr->unresolvedReleaseSurf = 0;
                    if (knownMoves & 2)
                        gPokemonStorageSystemPtr->unresolvedReleaseDive = 0;
                }
                if (++gPokemonStorageSystemPtr->releaseCheckBoxPos >= 30)
                {
                    gPokemonStorageSystemPtr->releaseCheckBoxPos = 0;
                    if (++gPokemonStorageSystemPtr->releaseCheckBoxId >= TOTAL_BOXES_COUNT)
                    {
                        gPokemonStorageSystemPtr->releaseStatusResolved = 1;
                        gPokemonStorageSystemPtr->canReleaseMon = 0;
                        break;
                    }
                }
            }
            if (gPokemonStorageSystemPtr->unresolvedReleaseSurf == 0 && gPokemonStorageSystemPtr->unresolvedReleaseDive == 0)
            {
                gPokemonStorageSystemPtr->releaseStatusResolved = 1;
                gPokemonStorageSystemPtr->canReleaseMon = 1;
            }
            break;
    }
    return -1;
}

void SaveMovingMon(void)
{
    if (sIsMonBeingMoved)
        sSavedMovingMon = gPokemonStorageSystemPtr->movingMon;
}

void LoadSavedMovingMon(void)
{
    if (sIsMonBeingMoved)
    {
        if (sMovingMonOrigBoxId == TOTAL_BOXES_COUNT)
            gPokemonStorageSystemPtr->movingMon = sSavedMovingMon;
        else
            gPokemonStorageSystemPtr->movingMon.box = sSavedMovingMon.box;
    }
}

void InitSummaryScreenData(void)
{
    if (sIsMonBeingMoved)
    {
        SaveMovingMon();
        gPokemonStorageSystemPtr->summaryMon.mon = &sSavedMovingMon;
        gPokemonStorageSystemPtr->summaryStartPos = 0;
        gPokemonStorageSystemPtr->summaryMaxPos = 0;
        gPokemonStorageSystemPtr->summaryScreenMode = 0;
    }
    else if (sCursorArea == PSS_CURSOR_AREA_IN_PARTY)
    {
        gPokemonStorageSystemPtr->summaryMon.mon = gPlayerParty;
        gPokemonStorageSystemPtr->summaryStartPos = sCursorPosition;
        gPokemonStorageSystemPtr->summaryMaxPos = CountPartyMons() - 1;
        gPokemonStorageSystemPtr->summaryScreenMode = 0;
    }
    else
    {
        gPokemonStorageSystemPtr->summaryMon.box = gPokemonStorage.boxes[gPokemonStorage.currentBox];
        gPokemonStorageSystemPtr->summaryStartPos = sCursorPosition;
        gPokemonStorageSystemPtr->summaryMaxPos = 30 - 1;
        gPokemonStorageSystemPtr->summaryScreenMode = 5;
    }
}

void SetSelectionAfterSummaryScreen(void)
{
    if (sIsMonBeingMoved)
        LoadSavedMovingMon();
    else
        sCursorPosition = sMonSummaryScreen->curMonIndex;
}

s16 CompactPartySlots(void)
{
    s16 retVal = -1;
    u16 i;
    u16 last;

    for (i = 0, last = 0; i < PARTY_SIZE; i++)
    {
        if ((u16)GetMonData(gPlayerParty + i, MON_DATA_SPECIES) != SPECIES_NONE)
        {
            if (i != last)
                gPlayerParty[last] = gPlayerParty[i];
            last++;
        }
        else if (retVal == -1)
            retVal = i;
    }
    for (; last < PARTY_SIZE; last++)
        ZeroMonData(gPlayerParty + last);
    return retVal;
}

void SetPokeStorageMonMarkings(u8 markings)
{
    gPokemonStorageSystemPtr->displayMonMarkings = markings;
    if (sIsMonBeingMoved)
        SetMonData(&gPokemonStorageSystemPtr->movingMon, MON_DATA_MARKINGS, &markings);
    else
    {
        if (sCursorArea == PSS_CURSOR_AREA_IN_PARTY)
            SetMonData(gPlayerParty + sCursorPosition, MON_DATA_MARKINGS, &markings);
        if (sCursorArea == PSS_CURSOR_AREA_IN_BOX)
            SetBoxMonData(gPokemonStorage.boxes[StorageGetCurrentBox()] + sCursorPosition, MON_DATA_MARKINGS, &markings);
    }
}

bool8 IsRemovingLastPartyMon(void)
{
    if (sCursorArea == PSS_CURSOR_AREA_IN_PARTY && !sIsMonBeingMoved && CountPartyAliveNonEggMonsExcept(sCursorPosition) == 0)
        return TRUE;
    return FALSE;
}

bool8 CanShiftMon(void)
{
    if (sIsMonBeingMoved)
    {
        if (sCursorArea == PSS_CURSOR_AREA_IN_PARTY && CountPartyAliveNonEggMonsExcept(sCursorPosition) == 0)
        {
            if (gPokemonStorageSystemPtr->displayMonIsEgg || GetMonData(&gPokemonStorageSystemPtr->movingMon, MON_DATA_HP) == 0)
                return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

bool8 IsMonBeingMoved(void)
{
    return sIsMonBeingMoved;
}

bool8 IsCursorOnBoxTitle(void)
{
    return sCursorArea == PSS_CURSOR_AREA_BOX_TITLE ? TRUE : FALSE;
}

bool8 IsCursorOnCloseBox(void)
{
    return (sCursorArea == PSS_CURSOR_AREA_BUTTONS && sCursorPosition == 1) ? TRUE : FALSE;
}

void TryRefreshDisplayMon(void)
{
    gPokemonStorageSystemPtr->setMosaic = sIsMonBeingMoved ? 0 : 1;
    if (!sIsMonBeingMoved)
    {
        switch (sCursorArea)
        {
            case PSS_CURSOR_AREA_IN_PARTY:
                if (sCursorPosition < PARTY_SIZE)
                {
                    SetDisplayMonData(gPlayerParty + sCursorPosition, PSS_MODE_PARTY);
                    break;
                }
                // fallthrough
            case PSS_CURSOR_AREA_BOX_TITLE:
            case PSS_CURSOR_AREA_BUTTONS:
                SetDisplayMonData(NULL, PSS_MODE_MOVE);
                break;
            case PSS_CURSOR_AREA_IN_BOX:
                SetDisplayMonData(gPokemonStorage.boxes[StorageGetCurrentBox()] + sCursorPosition, PSS_MODE_BOX);
                break;
        }
    }
}

void ReshowDisplayMon(void)
{
    if (sIsMonBeingMoved)
        SetDisplayMonData(&sSavedMovingMon.box, PSS_MODE_PARTY);
    else
        TryRefreshDisplayMon();
}

void SetDisplayMonData(void *pokemon, u8 mode)
{
    u8 *txtPtr;
    u16 gender = MON_MALE;
    gPokemonStorageSystemPtr->displayMonItemId = 0;
    if (mode == PSS_MODE_PARTY)
    {
        struct Pokemon *mon = (struct Pokemon *)pokemon;
        gPokemonStorageSystemPtr->displayMonSpecies = GetMonData(pokemon, MON_DATA_SPECIES_OR_EGG);
        if (gPokemonStorageSystemPtr->displayMonSpecies != SPECIES_NONE)
        {
            gPokemonStorageSystemPtr->displayMonIsEgg = GetMonData(mon, MON_DATA_IS_EGG);
            GetMonData(mon, MON_DATA_NICKNAME, gPokemonStorageSystemPtr->displayMonName);
            StringGet_Nickname(gPokemonStorageSystemPtr->displayMonName);
            gPokemonStorageSystemPtr->displayMonLevel = GetMonData(mon, MON_DATA_LEVEL);
            gPokemonStorageSystemPtr->displayMonMarkings = GetMonData(mon, MON_DATA_MARKINGS);
            gPokemonStorageSystemPtr->displayMonPersonality = GetMonData(mon, MON_DATA_PERSONALITY);
            gPokemonStorageSystemPtr->displayMonPalette = GetMonFrontSpritePal(mon);
            gender = GetMonGender(mon);
            gPokemonStorageSystemPtr->displayMonItemId = GetMonData(mon, MON_DATA_HELD_ITEM);
        }
    }
    else if (mode == PSS_MODE_BOX)
    {
        struct BoxPokemon *boxMon = (struct BoxPokemon *)pokemon;
        gPokemonStorageSystemPtr->displayMonSpecies = GetBoxMonData(pokemon, MON_DATA_SPECIES_OR_EGG);
        if (gPokemonStorageSystemPtr->displayMonSpecies != SPECIES_NONE)
        {
            u32 otId = GetBoxMonData(boxMon, MON_DATA_OT_ID);
            gPokemonStorageSystemPtr->displayMonIsEgg = GetBoxMonData(boxMon, MON_DATA_IS_EGG);
            GetBoxMonData(boxMon, MON_DATA_NICKNAME, gPokemonStorageSystemPtr->displayMonName);
            StringGet_Nickname(gPokemonStorageSystemPtr->displayMonName);
            gPokemonStorageSystemPtr->displayMonLevel = GetLevelFromBoxMonExp(boxMon);
            gPokemonStorageSystemPtr->displayMonMarkings = GetBoxMonData(boxMon, MON_DATA_MARKINGS);
            gPokemonStorageSystemPtr->displayMonPersonality = GetBoxMonData(boxMon, MON_DATA_PERSONALITY);
            gPokemonStorageSystemPtr->displayMonPalette = GetMonSpritePalFromSpeciesAndPersonality(gPokemonStorageSystemPtr->displayMonSpecies, otId, gPokemonStorageSystemPtr->displayMonPersonality);
            gender = GetGenderFromSpeciesAndPersonality(gPokemonStorageSystemPtr->displayMonSpecies, gPokemonStorageSystemPtr->displayMonPersonality);
            gPokemonStorageSystemPtr->displayMonItemId = GetBoxMonData(boxMon, MON_DATA_HELD_ITEM);
        }
    }
    else
    {
        gPokemonStorageSystemPtr->displayMonSpecies = SPECIES_NONE;
    }
    if (gPokemonStorageSystemPtr->displayMonSpecies == SPECIES_NONE)
    {
        gPokemonStorageSystemPtr->displayMonName[0] = EOS;
        gPokemonStorageSystemPtr->displayMonNameText[0] = EOS;
        gPokemonStorageSystemPtr->displayMonSpeciesName[0] = EOS;
        gPokemonStorageSystemPtr->displayMonGenderLvlText[0] = EOS;
        gPokemonStorageSystemPtr->displayMonItemName[0] = EOS;
    }
    else if (gPokemonStorageSystemPtr->displayMonIsEgg)
    {
        txtPtr = gPokemonStorageSystemPtr->displayMonNameText;
        txtPtr[0] = EXT_CTRL_CODE_BEGIN;
        txtPtr[1] = EXT_CTRL_CODE_COLOR_HIGHLIGHT_SHADOW;
        txtPtr[2] = TEXT_COLOR_WHITE2;
        txtPtr[3] = TEXT_COLOR_TRANSPARENT;
        txtPtr[4] = TEXT_COLOR_DARK_GREY;
        txtPtr = gPokemonStorageSystemPtr->displayMonNameText + 5;
        txtPtr = StringCopy(txtPtr, gPokemonStorageSystemPtr->displayMonName);
        gPokemonStorageSystemPtr->displayMonSpeciesName[0] = EOS;
        gPokemonStorageSystemPtr->displayMonGenderLvlText[0] = EOS;
        gPokemonStorageSystemPtr->displayMonItemName[0] = EOS;
    }
    else
    {
        if (gPokemonStorageSystemPtr->displayMonSpecies == SPECIES_NIDORAN_M || gPokemonStorageSystemPtr->displayMonSpecies == SPECIES_NIDORAN_F)
            gender = MON_GENDERLESS;
        txtPtr = gPokemonStorageSystemPtr->displayMonNameText;
        txtPtr[0] = EXT_CTRL_CODE_BEGIN;
        txtPtr[1] = EXT_CTRL_CODE_COLOR_HIGHLIGHT_SHADOW;
        txtPtr[2] = TEXT_COLOR_WHITE2;
        txtPtr[3] = TEXT_COLOR_TRANSPARENT;
        txtPtr[4] = TEXT_COLOR_DARK_GREY;
        txtPtr = gPokemonStorageSystemPtr->displayMonNameText + 5;
        txtPtr = StringCopy(txtPtr, gPokemonStorageSystemPtr->displayMonName);
        txtPtr = gPokemonStorageSystemPtr->displayMonSpeciesName;
        txtPtr[0] = EXT_CTRL_CODE_BEGIN;
        txtPtr[1] = EXT_CTRL_CODE_COLOR_HIGHLIGHT_SHADOW;
        txtPtr[2] = TEXT_COLOR_WHITE2;
        txtPtr[3] = TEXT_COLOR_TRANSPARENT;
        txtPtr[4] = TEXT_COLOR_DARK_GREY;
        txtPtr[5] = EXT_CTRL_CODE_BEGIN;
        txtPtr[6] = EXT_CTRL_CODE_CLEAR_TO;
        txtPtr[7] = 7;
        txtPtr[8] = CHAR_SLASH;
        txtPtr = gPokemonStorageSystemPtr->displayMonSpeciesName + 9;
        txtPtr = StringCopy(txtPtr, gSpeciesNames[gPokemonStorageSystemPtr->displayMonSpecies]);
        txtPtr[0] = EXT_CTRL_CODE_BEGIN;
        txtPtr[1] = EXT_CTRL_CODE_CLEAR_TO;
        txtPtr[2] = 0x50;
        txtPtr[3] = EOS;
        txtPtr = gPokemonStorageSystemPtr->displayMonGenderLvlText;
        txtPtr[0] = EXT_CTRL_CODE_BEGIN;
        txtPtr[1] = EXT_CTRL_CODE_COLOR_HIGHLIGHT_SHADOW;
        txtPtr[2] = TEXT_COLOR_WHITE2;
        txtPtr[3] = TEXT_COLOR_TRANSPARENT;
        txtPtr[4] = TEXT_COLOR_DARK_GREY;
        txtPtr[5] = EXT_CTRL_CODE_BEGIN;
        txtPtr[6] = EXT_CTRL_CODE_CLEAR_TO;
        txtPtr[7] = 8;
        txtPtr = gPokemonStorageSystemPtr->displayMonGenderLvlText + 8;
        txtPtr[0] = CHAR_LV;
        txtPtr = gPokemonStorageSystemPtr->displayMonGenderLvlText + 9;
        txtPtr = AlignInt1InMenuWindow(txtPtr, gPokemonStorageSystemPtr->displayMonLevel, 0x22, STR_CONV_MODE_RIGHT_ALIGN);
        txtPtr[0] = EXT_CTRL_CODE_BEGIN;
        txtPtr[1] = EXT_CTRL_CODE_CLEAR;
        txtPtr[2] = 8;
        txtPtr += 3;
        switch (gender)
        {
            case MON_MALE:
                txtPtr[0] = EXT_CTRL_CODE_BEGIN;
                txtPtr[1] = EXT_CTRL_CODE_COLOR_HIGHLIGHT_SHADOW;
                txtPtr[2] = TEXT_COLOR_BLUE;
                txtPtr[3] = TEXT_COLOR_TRANSPARENT;
                txtPtr[4] = TEXT_COLOR_YELLOW;
                txtPtr[5] = CHAR_MALE;
                txtPtr += 6;
                break;
            case MON_FEMALE:
                txtPtr[0] = EXT_CTRL_CODE_BEGIN;
                txtPtr[1] = EXT_CTRL_CODE_COLOR_HIGHLIGHT_SHADOW;
                txtPtr[2] = TEXT_COLOR_CYAN;
                txtPtr[3] = TEXT_COLOR_TRANSPARENT;
                txtPtr[4] = TEXT_COLOR_MAGENTA;
                txtPtr[5] = CHAR_FEMALE;
                txtPtr += 6;
                break;
        }
        txtPtr[0] = EOS;
        txtPtr = gPokemonStorageSystemPtr->displayMonItemName;
        if (gPokemonStorageSystemPtr->displayMonItemId)
        {
            txtPtr[0] = EXT_CTRL_CODE_BEGIN;
            txtPtr[1] = EXT_CTRL_CODE_COLOR_HIGHLIGHT_SHADOW;
            txtPtr[2] = TEXT_COLOR_WHITE2;
            txtPtr[3] = TEXT_COLOR_TRANSPARENT;
            txtPtr[4] = TEXT_COLOR_DARK_GREY;
            txtPtr = gPokemonStorageSystemPtr->displayMonItemName + 5;
            txtPtr[0] = EXT_CTRL_CODE_BEGIN;
            txtPtr[1] = EXT_CTRL_CODE_FONT;
            txtPtr[2] = 0x04;
            txtPtr = gPokemonStorageSystemPtr->displayMonItemName + 8;
            txtPtr = StringCopy(txtPtr, GetItemName(gPokemonStorageSystemPtr->displayMonItemId));
            txtPtr[0] = EXT_CTRL_CODE_BEGIN;
            txtPtr[1] = EXT_CTRL_CODE_RESET_FONT;
            txtPtr += 2;
        }
        txtPtr[0] = EOS;
    }
}

u8 InBoxInput_Normal(void)
{
    u8 retVal;
    s8 cursorArea = sCursorArea;
    s8 cursorPosition = sCursorPosition;

    do
    {
        gPokemonStorageSystemPtr->cursorVerticalWrap = 0;
        gPokemonStorageSystemPtr->cursorHorizontalWrap = 0;
        gPokemonStorageSystemPtr->cursorFlipTimer = 0;

        if (JOY_REPT(DPAD_UP))
        {
            retVal = PSS_INPUT_MOVE_CURSOR;
            if (sCursorPosition >= 6)
            {
                cursorPosition -= 6;
            }
            else
            {
                cursorArea = PSS_CURSOR_AREA_BOX_TITLE;
                cursorPosition = 0;
            }
            break;
        }
        if (JOY_REPT(DPAD_DOWN))
        {
            retVal = PSS_INPUT_MOVE_CURSOR;
            cursorPosition += 6;
            if (cursorPosition >= 30)
            {
                cursorArea = PSS_CURSOR_AREA_BUTTONS;
                cursorPosition -= 30;
                cursorPosition /= 3;
                gPokemonStorageSystemPtr->cursorVerticalWrap = 1;
                gPokemonStorageSystemPtr->cursorFlipTimer = 1;
            }
            break;
        }
        if (JOY_REPT(DPAD_LEFT))
        {
            retVal = PSS_INPUT_MOVE_CURSOR;
            if (sCursorPosition % 6)
                cursorPosition--;
            else
            {
                gPokemonStorageSystemPtr->cursorHorizontalWrap = -1;
                cursorPosition += 5;
            }
            break;
        }
        if (JOY_REPT(DPAD_RIGHT))
        {
            retVal = PSS_INPUT_MOVE_CURSOR;
            if ((sCursorPosition + 1) % 6)
                cursorPosition++;
            else
            {
                gPokemonStorageSystemPtr->cursorHorizontalWrap = 1;
                cursorPosition -= 5;
            }
            break;
        }
        if (JOY_NEW(START_BUTTON))
        {
            retVal = PSS_INPUT_MOVE_CURSOR;
            cursorArea = PSS_CURSOR_AREA_BOX_TITLE;
            cursorPosition = 0;
            break;
        }
        if ((JOY_NEW(A_BUTTON)) && SetSelectionMenuTexts())
        {
            if (sAutoActionOn == 0)
                return PSS_INPUT_IN_MENU;
            switch (GetPokeStorageMenuItemTextId(0))
            {
            case PSS_MENU_STORE:
                return PSS_INPUT_DEPOSIT;
            case PSS_MENU_WITHDRAW:
                return PSS_INPUT_WITHDRAW;
            case PSS_MENU_MOVE:
                return PSS_INPUT_MOVE_MON;
            case PSS_MENU_SHIFT:
                return PSS_INPUT_SHIFT_MON;
            case PSS_MENU_PLACE:
                return PSS_INPUT_PLACE_MON;
            }
        }
        if (JOY_NEW(B_BUTTON))
            return PSS_INPUT_PRESSED_B;
        if (gSaveBlock2.optionsButtonMode == OPTIONS_BUTTON_MODE_LR)
        {
            if (JOY_HELD(L_BUTTON))
                return PSS_INPUT_SCROLL_LEFT;
            if (JOY_HELD(R_BUTTON))
                return PSS_INPUT_SCROLL_RIGHT;
        }
        if (JOY_NEW(SELECT_BUTTON))
        {
            ToggleCursorAutoAction();
            return PSS_INPUT_NONE;
        }
        retVal = PSS_INPUT_NONE;
    } while (0);
    if (retVal != PSS_INPUT_NONE)
        SetCursorPosition(cursorArea, cursorPosition);
    return retVal;
}

u8 HandleInput_InParty(void)
{
    u8 retVal;
    bool8 gotoBox;
    s8 cursorArea;
    s8 cursorPosition;

    do
    {
        cursorArea = sCursorArea;
        cursorPosition = sCursorPosition;
        gPokemonStorageSystemPtr->cursorHorizontalWrap = 0;
        gPokemonStorageSystemPtr->cursorVerticalWrap = 0;
        gPokemonStorageSystemPtr->cursorFlipTimer = 0;
        gotoBox = FALSE;
        retVal = PSS_INPUT_NONE;

        if (JOY_REPT(DPAD_UP))
        {
            if (--cursorPosition < 0)
                cursorPosition = 6;
            if (cursorPosition != sCursorPosition)
                retVal = PSS_INPUT_MOVE_CURSOR;
            break;
        }

        if (JOY_REPT(DPAD_DOWN))
        {
            if (++cursorPosition > 6)
                cursorPosition = 0;
            if (cursorPosition != sCursorPosition)
                retVal = PSS_INPUT_MOVE_CURSOR;
            break;
        }

        if (JOY_REPT(DPAD_LEFT) && sCursorPosition != 0)
        {
            retVal = PSS_INPUT_MOVE_CURSOR;
            gPokemonStorageSystemPtr->cursorPrevHorizPos = sCursorPosition;
            cursorPosition = 0;
            break;
        }

        if (JOY_REPT(DPAD_RIGHT))
        {
            if (sCursorPosition == 0)
            {
                retVal = PSS_INPUT_MOVE_CURSOR;
                cursorPosition = gPokemonStorageSystemPtr->cursorPrevHorizPos;
            }
            else
            {
                retVal = PSS_INPUT_HIDE_PARTY;
                cursorArea = PSS_CURSOR_AREA_IN_BOX;
                cursorPosition = 0;
            }
            break;
        }

        if (JOY_NEW(A_BUTTON))
        {
            if (sCursorPosition == 6)
            {
                if (gPokemonStorageSystemPtr->boxOption == PSS_OPTION_DEPOSIT)
                    return PSS_INPUT_CLOSE_BOX;

                gotoBox = TRUE;
            }
            else if (SetSelectionMenuTexts())
            {
                if (!sAutoActionOn)
                    return PSS_INPUT_IN_MENU;

                switch (GetPokeStorageMenuItemTextId(0))
                {
                case PSS_MENU_STORE:
                    return PSS_INPUT_DEPOSIT;
                case PSS_MENU_WITHDRAW:
                    return PSS_INPUT_WITHDRAW;
                case PSS_MENU_MOVE:
                    return PSS_INPUT_MOVE_MON;
                case PSS_MENU_SHIFT:
                    return PSS_INPUT_SHIFT_MON;
                case PSS_MENU_PLACE:
                    return PSS_INPUT_PLACE_MON;
                }
            }
        }

        if (JOY_NEW(B_BUTTON))
        {
            if (gPokemonStorageSystemPtr->boxOption == PSS_OPTION_DEPOSIT)
                return PSS_INPUT_PRESSED_B;

            gotoBox = TRUE;
        }

        if (gotoBox)
        {
            retVal = PSS_INPUT_HIDE_PARTY;
            cursorArea = PSS_CURSOR_AREA_IN_BOX;
            cursorPosition = 0;
            break;
        }

        if (JOY_NEW(SELECT_BUTTON))
        {
            ToggleCursorAutoAction();
            return PSS_INPUT_NONE;
        }
    } while (0);

    if (retVal != PSS_INPUT_NONE)
    {
        SetCursorPosition(cursorArea, cursorPosition);
    }

    return retVal;
}

u8 HandleInput_OnBox(void)
{
    u8 retVal;
    s8 cursorArea;
    s8 cursorPosition;

    do
    {
        gPokemonStorageSystemPtr->cursorHorizontalWrap = 0;
        gPokemonStorageSystemPtr->cursorVerticalWrap = 0;
        gPokemonStorageSystemPtr->cursorFlipTimer = 0;

        if (JOY_REPT(DPAD_UP))
        {
            retVal = PSS_INPUT_MOVE_CURSOR;
            cursorArea = PSS_CURSOR_AREA_BUTTONS;
            cursorPosition = 0;
            gPokemonStorageSystemPtr->cursorFlipTimer = 1;
            break;
        }
        else if (JOY_REPT(DPAD_DOWN))
        {
            retVal = PSS_INPUT_MOVE_CURSOR;
            cursorArea = PSS_CURSOR_AREA_IN_BOX;
            cursorPosition = 2;
            break;
        }

        if (JOY_HELD(DPAD_LEFT))
            return PSS_INPUT_SCROLL_LEFT;
        if (JOY_HELD(DPAD_RIGHT))
            return PSS_INPUT_SCROLL_RIGHT;

        if (gSaveBlock2.optionsButtonMode == OPTIONS_BUTTON_MODE_LR)
        {
            if (JOY_HELD(L_BUTTON))
                return PSS_INPUT_SCROLL_LEFT;
            if (JOY_HELD(R_BUTTON))
                return PSS_INPUT_SCROLL_RIGHT;
        }

        if (JOY_NEW(A_BUTTON))
        {
            AnimateBoxScrollArrows(FALSE);
            AddBoxOptionsMenu();
            return PSS_INPUT_BOX_OPTIONS;
        }

        if (JOY_NEW(B_BUTTON))
            return PSS_INPUT_PRESSED_B;

        if (JOY_NEW(SELECT_BUTTON))
        {
            ToggleCursorAutoAction();
            return PSS_INPUT_NONE;
        }

        retVal = PSS_INPUT_NONE;
    } while (0);

    if (retVal != PSS_INPUT_NONE)
    {
        if (cursorArea != PSS_CURSOR_AREA_BOX_TITLE)
            AnimateBoxScrollArrows(FALSE);
        SetCursorPosition(cursorArea, cursorPosition);
    }

    return retVal;
}

u8 HandleInput_OnButtons(void)
{
    u8 retVal;
    s8 cursorArea;
    s8 cursorPosition;

    do
    {
        cursorArea = sCursorArea;
        cursorPosition = sCursorPosition;
        gPokemonStorageSystemPtr->cursorHorizontalWrap = 0;
        gPokemonStorageSystemPtr->cursorVerticalWrap = 0;
        gPokemonStorageSystemPtr->cursorFlipTimer = 0;

        if (JOY_REPT(DPAD_UP))
        {
            retVal = PSS_INPUT_MOVE_CURSOR;
            cursorArea = PSS_CURSOR_AREA_IN_BOX;
            gPokemonStorageSystemPtr->cursorVerticalWrap = -1;
            if (sCursorPosition == 0)
                cursorPosition = 24;
            else
                cursorPosition = 29;
            gPokemonStorageSystemPtr->cursorFlipTimer = 1;
            break;
        }

        if (JOY_REPT(DPAD_DOWN | START_BUTTON))
        {
            retVal = PSS_INPUT_MOVE_CURSOR;
            cursorArea = PSS_CURSOR_AREA_BOX_TITLE;
            cursorPosition = 0;
            gPokemonStorageSystemPtr->cursorFlipTimer = 1;
            break;
        }

        if (JOY_REPT(DPAD_LEFT))
        {
            retVal = PSS_INPUT_MOVE_CURSOR;
            if (--cursorPosition < 0)
            {
                gPokemonStorageSystemPtr->cursorHorizontalWrap = -1;
                cursorPosition = 1;
            }
            break;
        }

        if (JOY_REPT(DPAD_RIGHT))
        {
            retVal = PSS_INPUT_MOVE_CURSOR;
            if (++cursorPosition > 1)
            {
                gPokemonStorageSystemPtr->cursorHorizontalWrap = 1;
                cursorPosition = 0;
            }
            break;
        }

        if (JOY_NEW(A_BUTTON))
            return (cursorPosition == 0) ? PSS_INPUT_SHOW_PARTY : PSS_INPUT_CLOSE_BOX;
        if (JOY_NEW(B_BUTTON))
            return PSS_INPUT_PRESSED_B;

        if (JOY_NEW(SELECT_BUTTON))
        {
            ToggleCursorAutoAction();
            return PSS_INPUT_NONE;
        }

        retVal = PSS_INPUT_NONE;
    } while (0);

    if (retVal != PSS_INPUT_NONE)
    {
        SetCursorPosition(cursorArea, cursorPosition);
    }

    return retVal;
}

u8 HandlePokeStorageInput(void)
{
    u16 i = 0;
    while (gPokeStorageInputHandlers[i].func != NULL)
    {
        if (gPokeStorageInputHandlers[i].cursorArea == sCursorArea)
            return gPokeStorageInputHandlers[i].func();
        i++;
    }

    return PSS_INPUT_NONE;
}

void AddBoxOptionsMenu(void)
{
    InitPokeStorageMenu();
    SetPokeStorageMenuText(PSS_MENU_JUMP);
    SetPokeStorageMenuText(PSS_MENU_WALLPAPER);
    SetPokeStorageMenuText(PSS_MENU_NAME);
    SetPokeStorageMenuText(PSS_MENU_CANCEL);
}

#if DEBUG
u8 Debug_SetSelectionMenuTexts(void)
{
    if (!GetSpeciesAtCursorPosition())
        return FALSE;

    InitPokeStorageMenu();
    SetPokeStorageMenuText(PSS_MENU_DEBUG_SET);
    SetPokeStorageMenuText(PSS_MENU_SUMMARY);
    return TRUE;
}
#endif

bool8 SetSelectionMenuTexts(void)
{
    u16 species;

#if DEBUG
    if (gDebugPokeStorageActive)
        return Debug_SetSelectionMenuTexts();
#endif

    InitPokeStorageMenu();
    species = GetSpeciesAtCursorPosition();

    switch (gPokemonStorageSystemPtr->boxOption)
    {
    case PSS_OPTION_DEPOSIT:
        if (species)
            SetPokeStorageMenuText(PSS_MENU_STORE);
        else
            return FALSE;
        break;
    case PSS_OPTION_WITHDRAW:
        if (species)
            SetPokeStorageMenuText(PSS_MENU_WITHDRAW);
        else
            return FALSE;
        break;
    case PSS_OPTION_MOVE_MONS:
        if (sIsMonBeingMoved)
        {
            if (species)
                SetPokeStorageMenuText(PSS_MENU_SHIFT);
            else
                SetPokeStorageMenuText(PSS_MENU_PLACE);
        }
        else
        {
            if (species)
                SetPokeStorageMenuText(PSS_MENU_MOVE);
            else
                return FALSE;
        }
        break;
    default:
        return FALSE;
    }

    SetPokeStorageMenuText(PSS_MENU_SUMMARY);
    if (gPokemonStorageSystemPtr->boxOption == PSS_OPTION_MOVE_MONS)
    {
        if (sCursorArea == PSS_CURSOR_AREA_IN_BOX)
            SetPokeStorageMenuText(PSS_MENU_WITHDRAW);
        else
            SetPokeStorageMenuText(PSS_MENU_STORE);
    }

    SetPokeStorageMenuText(PSS_MENU_MARK);
    SetPokeStorageMenuText(PSS_MENU_RELEASE);
    SetPokeStorageMenuText(PSS_MENU_CANCEL);
    return TRUE;
}

void SpriteCB_CursorShadow(struct Sprite *sprite)
{
    sprite->x = gPokemonStorageSystemPtr->cursorSprite->x;
    sprite->y = gPokemonStorageSystemPtr->cursorSprite->y + 20;
}

void CopyCursorMonToDebugMon(struct Pokemon *mon)
{
    if (sCursorArea == PSS_CURSOR_AREA_IN_PARTY)
    {
        mon->box = gPlayerParty[sCursorPosition].box;
    }
    else
    {
        u8 boxId = StorageGetCurrentBox();
        mon->box = gPokemonStorage.boxes[boxId][sCursorPosition];
    }
}

void CreateCursorSprites(void)
{
    u16 x, y;
    u8 spriteId;
    int priority, subpriority;
    struct SpriteSheet spriteSheets[3];
    struct SpritePalette spritePalettes[3];

    memcpy(spriteSheets, gPokeStorageCursorSpriteSheets, 24);
    memcpy(spritePalettes, gPokeStorageCursorSpritePalettes, 24);
    LoadSpriteSheets(spriteSheets);
    LoadSpritePalettes(spritePalettes);
    gPokemonStorageSystemPtr->cursorPalNums[0] = IndexOfSpritePaletteTag(0xDAC6);
    gPokemonStorageSystemPtr->cursorPalNums[1] = IndexOfSpritePaletteTag(0xDAD1);

    GetCursorCoordsByPos(sCursorArea, sCursorPosition, &x, &y);
    spriteId = CreateSprite(&gSpriteTemplate_PokeStorageCursor, x, y, 6);
    if (spriteId != MAX_SPRITES)
    {
        gPokemonStorageSystemPtr->cursorSprite = &gSprites[spriteId];
        gPokemonStorageSystemPtr->cursorSprite->oam.paletteNum = gPokemonStorageSystemPtr->cursorPalNums[sAutoActionOn];
        if (sIsMonBeingMoved)
            StartSpriteAnim(gPokemonStorageSystemPtr->cursorSprite, PSS_CURSOR_ANIM_FIST);
    }
    else
    {
        gPokemonStorageSystemPtr->cursorSprite = NULL;
    }

    if (sCursorArea == PSS_CURSOR_AREA_IN_PARTY)
    {
        subpriority = 12;
        priority = 1;
    }
    else
    {
        subpriority = 20;
        priority = 2;
    }

    spriteId = CreateSprite(&gSpriteTemplate_PokeStorageCursorShadow, 0, 0, subpriority);
    if (spriteId != MAX_SPRITES)
    {
        gPokemonStorageSystemPtr->cursorShadowSprite = &gSprites[spriteId];
        gPokemonStorageSystemPtr->cursorShadowSprite->oam.priority = priority;
        if (sCursorArea != PSS_CURSOR_AREA_IN_BOX)
            gPokemonStorageSystemPtr->cursorShadowSprite->invisible = TRUE;
    }
    else
    {
        gPokemonStorageSystemPtr->cursorShadowSprite = NULL;
    }
}

void ToggleCursorAutoAction(void)
{
    sAutoActionOn = !sAutoActionOn;
    gPokemonStorageSystemPtr->cursorSprite->oam.paletteNum = gPokemonStorageSystemPtr->cursorPalNums[sAutoActionOn];
}

void InitPokeStorageMenu(void)
{
    gPokemonStorageSystemPtr->menuItemsCount = 0;
    gPokemonStorageSystemPtr->menuWidth = 0;
}

void SetPokeStorageMenuText(u8 menuItemId)
{
    u8 width;
    u8 menuItemWidth;

    if (gPokemonStorageSystemPtr->menuItemsCount < 7)
    {
        struct PokeStorageMenuItem *storageText = &gPokemonStorageSystemPtr->menuItems[gPokemonStorageSystemPtr->menuItemsCount];
        storageText->text = gPokeStorageMenuTexts[menuItemId];
        storageText->menuItemId = menuItemId;
        width = GetStringWidthInMenuWindow(storageText->text);
        menuItemWidth = (width + 7) / 8;
        if (menuItemWidth > gPokemonStorageSystemPtr->menuWidth)
            gPokemonStorageSystemPtr->menuWidth = menuItemWidth;

        gPokemonStorageSystemPtr->menuItemsCount++;
    }
}

s8 GetPokeStorageMenuItemTextId(u8 menuIndex)
{
    if (menuIndex >= gPokemonStorageSystemPtr->menuItemsCount)
        return -1;
    else
        return gPokemonStorageSystemPtr->menuItems[menuIndex].menuItemId;
}

void AddPokeStorageMenu(void)
{
    gPokemonStorageSystemPtr->menuTop = 14 - gPokemonStorageSystemPtr->menuItemsCount * 2;
    gPokemonStorageSystemPtr->menuLeft = 28 - gPokemonStorageSystemPtr->menuWidth;
    Menu_DrawStdWindowFrame(gPokemonStorageSystemPtr->menuLeft, gPokemonStorageSystemPtr->menuTop, 29, 15);
    Menu_PrintItems(
        gPokemonStorageSystemPtr->menuLeft + 1,
        gPokemonStorageSystemPtr->menuTop + 1,
        gPokemonStorageSystemPtr->menuItemsCount,
        gPokemonStorageSystemPtr->menuItems);
    InitMenu(
        0,
        gPokemonStorageSystemPtr->menuLeft + 1,
        gPokemonStorageSystemPtr->menuTop + 1,
        gPokemonStorageSystemPtr->menuItemsCount,
        0,
        gPokemonStorageSystemPtr->menuWidth);
}

s16 HandlePokeStorageMenuInput(void)
{
    int textId = -2;

    do
    {
        if (!(JOY_NEW(A_BUTTON)))
        {
            if (JOY_NEW(B_BUTTON))
            {
                PlaySE(SE_SELECT);
                textId++;
            }

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
        }
        else
        {
            textId = Menu_GetCursorPos();
        }

        if (textId != -2)
        {
            Menu_DestroyCursor();
            Menu_EraseWindowRect(
                gPokemonStorageSystemPtr->menuLeft,
                gPokemonStorageSystemPtr->menuTop,
                29,
                15);
        }
    } while (0);

    if (textId >= 0)
    {
        textId = gPokemonStorageSystemPtr->menuItems[textId].menuItemId;
    }

    return textId;
}

void InitTilemapTransferQueue(struct TilemapTransferQueue *queue, struct TilemapTransfer *transfers, u8 capacity)
{
    sTilemapTransferQueue = queue;
    queue->transfers = transfers;
    queue->capacity = capacity;
    queue->transferCount = 0;
}

void RunTilemapTransferQueue(void)
{
    u16 i;
    struct TilemapTransfer *transfer;

    if (sTilemapTransferQueue->transferCount)
    {
        for (i = 0; i < sTilemapTransferQueue->transferCount; i++)
        {
            transfer = &sTilemapTransferQueue->transfers[i];
            transfer->execute(transfer);
        }

        sTilemapTransferQueue->transferCount = 0;
    }
}

bool8 QueueTilemapCopy(u8 *destination, u16 destLeft, u16 destTop, const u8 *source, u16 sourceLeft, u16 sourceTop, u16 width, u16 height)
{
    struct TilemapTransfer *transfer;

    if (sTilemapTransferQueue->transferCount >= sTilemapTransferQueue->capacity)
        return FALSE;

    transfer = &sTilemapTransferQueue->transfers[sTilemapTransferQueue->transferCount++];
    transfer->rowSize = width * 2;
    transfer->destination = destination + (destTop * 64) + (destLeft * 2);
    transfer->source = source + (sourceTop * 64) + (sourceLeft * 2);
    transfer->height = height;
    transfer->execute = RunTilemapCopy;
    return TRUE;
}

void RunTilemapCopy(struct TilemapTransfer *transfer)
{
    u32 i;
    const u8 *source = transfer->source;
    u8 *destination = transfer->destination;
    int height = transfer->height;
    int rowSize = transfer->rowSize;
    for (i = 0; i < height; i++)
    {
        CpuCopy16(source, destination, rowSize);
        destination += 64;
        source += 64;
    }
}

void CopyTilemapRectFixedWidth(u8 *dest, u16 dLeft, u16 dTop, const u8 *src, u16 sLeft, u16 sTop, u16 width, u16 height)
{
    const u8 *from = src + (sTop * 64) + (sLeft * 2);
    u8 *to = dest + (dTop * 64) + (dLeft * 2);
    u8 *end = dest + (dTop + height) * 64;
    int width2 = width * 2;
    while (to < end)
    {
        CpuCopy16(from, to, width2);
        to += 64;
        from += 64;
    }
}

bool8 QueueTilemapFill(void *destination, u16 destLeft, u16 destTop, u16 width, u16 height)
{
    struct TilemapTransfer *transfer;

    if (sTilemapTransferQueue->transferCount >= sTilemapTransferQueue->capacity)
        return FALSE;

    transfer = &sTilemapTransferQueue->transfers[sTilemapTransferQueue->transferCount++];
    transfer->rowSize = width * 2;
    transfer->destination = destination + ((destTop * 32) + destLeft) * 2;
    transfer->height = height;
    transfer->execute = RunTilemapFill;
    return TRUE;
}

void RunTilemapFill(struct TilemapTransfer *transfer)
{
    u16 i;

    for (i = 0; i < transfer->height; i++)
    {
        Dma3FillLarge_(0, transfer->destination, transfer->rowSize, 16);
        transfer->destination += 64;
    }
}
