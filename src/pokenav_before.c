#include "global.h"
#include "main.h"
#include "pokenav.h"
#include "ewram.h"
#include "battle.h"
#include "data2.h"
#include "de_string_util.h"
#include "decompress.h"
#include "event_data.h"
#include "constants/flags.h"
#include "landmark.h"
#include "link.h"
#include "menu.h"
#include "overworld.h"
#include "palette.h"
#include "region_map.h"
#include "constants/songs.h"
#include "string_util.h"
#include "strings.h"
#include "sound.h"
#include "task.h"
#include "text.h"
#include "trig.h"
#include "scanline_effect.h"
#include "pokemon_storage_system.h"
#include "pokemon_summary_screen.h"
#include "use_pokeblock.h"
#include "constants/game_stat.h"
#include "pokemon.h"

struct UnknownPokenav0
{
    /* 0x0000 */ u8 var0[0x0300];
    /* 0x0300 */ void (*var300)(void);
    /* 0x0304 */ u16 var304;
    /* 0x0306 */ u16 var306;
    /* 0x0308 */ u32 var308;
    /* 0x030C */ u8 fill030C[0x6a9f];
    /* 0x6dab */ u8 var6dab;
    /* 0x6dac */ u8 var6dac;
    /* 0x6dad */ u8 var6dad;
    /* 0x6dae */ u8 var6dae;
    /* 0x6daf */ u8 fill6daf[0x3];
    /* 0x6db2 */ u8 var6db2[5];
    /* 0x6db7 */ u8 fill6db7[0x25];
    /* 0x6ddc */ s8 var6ddc;
    /* 0x6ddd */ u8 fill6ddd[0x13];
    /* 0x6df0 */ s8 var6df0;
    /* 0x6df1 */ u8 fill6df1[0xB];
    /* 0x6dfc */ u8 var6dfc;
    /* 0x6dfd */ u8 fill6dfd[0x17];
    /* 0x6e14 */ u8 var6e14;
    /* 0x6e15 */ u8 var6e15;
    /* 0x6e16 */ u8 var6e16;
    /* 0x6e17 */ u8 var6e17;
    /* 0x6e18 */ u8 fill6e18[0x78];
    /* 0x6e90 */ u8 var6e90;
    /* 0x6e91 */ u8 fill6e91[0x4];
    /* 0x6e95 */ u8 var6e95;
    /* 0x6e96 */ u8 var6e96[0x814];
    /* 0x76AA */ u8 var76aa;
    /* 0x76AB */ u8 fill76AB[0x10BD];
    /* 0x8768 */ u32 var8768;
    /* 0x876C */ u8 fill876C[0x2];
    /* 0x876E */ u8 var876E;
    /* 0x876F */ u8 fill876F[0x5];
    /* 0x8774 */ s16 var8774;
    /* 0x8776 */ u8 fill8776[0x55];
    /* 0x87CB */ u8 var87CB;
    /* 0x87CC */ u8 fill87CC[0xC];
    /* 0x87D8 */ u8 var87D8;
    /* 0x87D9 */ u8 fill87D9[0x1];
    /* 0x87DA */ s16 var87DA;
    /* 0x87DC */ s16 var87DC;
    /* 0x87DE */ u8 fill87DE[0x4a];
    /* 0x8828 */ u8 playerPartyCount;
    /* 0x8829 */ u8 fill8829[0x07bf];
    /* 0x8FE8 */ s8 var8fe8;
    /* 0x8FE9 */ u8 fill8FE9[0x16];
    /* 0x8FFF */ u8 var8fff[5];
    /* 0x9004 */ u8 fill9004[0x340];
    /* 0x9344 */ u8 var9344;
    /* 0x9345 */ u8 fill9345[0x3b8b];
    /* 0xced0 */ u32 varCED0;
    /* 0xced4 */ u8 fillCED4[0x284];
    /* 0xD158 */ u16 varD158;
    /* 0xD15A */ u8 fillD15A[0x8];
    /* 0xD162 */ u8 varD162;
};

struct UnknownPokenav0_1 {
    u8 fill6dad[0x6dad];
    s8 var6dad;
    s8 var6dae;
};

struct PokenavRibbonIconGfx {
    u16 baseTile;
    u8 paletteNum;
};

static void (*sPokenavCallbackBeforeLinkWait)(void);

struct PokenavState *const gPokenavState = (struct PokenavState *)gSharedMem;

extern u16 gKeyRepeatStartDelay;

// rodata?
u8 *const gPokenavTileBuffer = gTileBuffer;

const u8 gPokenavMenuBackground_Pal[] = INCBIN_U8("graphics/pokenav/menu/background.gbapal");

const u8 gPokenavMenuBackground_Gfx[] = INCBIN_U8("graphics/pokenav/menu/background.4bpp");
const u8 gPokenavMenuBackground_Tilemap[] = INCBIN_U8("graphics/pokenav/menu/background.bin.lz");
const u16 gPokenavMenuPaletteGradientColors[] = INCBIN_U16("graphics/pokenav/menu/palette_gradient.gbapal");

const u8 gPokenavBg3Scroll_Gfx[] = INCBIN_U8("graphics/pokenav/menu/bg3_scroll.4bpp");
const u8 gPokenavBg3Scroll_Tilemap[] = INCBIN_U8("graphics/pokenav/menu/bg3_scroll.bin.lz");
const u16 gPokenavConditionMonInfoHeader_Pal[] = INCBIN_U16("graphics/pokenav/condition/mon_info_header.pal.bin");

const u16 gPokenavRibbonsMonInfoHeader_Pal[] = INCBIN_U16("graphics/pokenav/ribbons/mon_info.pal.bin");

const u16 gUnusedPokenav1_Pal[] = INCBIN_U16("graphics/pokenav/unused/palette1.bin");

const u8 gPokenavConditionSearchGraphTilemap[] = INCBIN_U8("graphics/pokenav/condition/search_graph.bin");
const u8 gPokenavPokeblockGraphTilemap[] = INCBIN_U8("graphics/pokenav/condition/pokeblock_graph.bin");
const u16 gConditionGraphData_Pal[] = INCBIN_U16("graphics/pokenav/condition/graph_data.gbapal");
const u16 gPokenavRibbonsList_Pal[] = INCBIN_U16("graphics/pokenav/ribbons/list_ui.gbapal");

const u16 gPokenavConditionSearch2_Pal[] = INCBIN_U16("graphics/pokenav/condition_search2.gbapal");

const u16 gPokenavList_Pal[] = INCBIN_U16("graphics/pokenav/list.gbapal");

const u16 gUnusedPokenav2_Pal[] = INCBIN_U16("graphics/pokenav/unused/palette2.bin");

const u8 gPokenavTrainerEyesList_Pal[] = INCBIN_U8("graphics/pokenav/trainers_eyes/list.gbapal");
const u8 gPokenavTrainerEyesMisc_Pal[] = INCBIN_U8("graphics/pokenav/trainers_eyes/misc_pal.bin");

const u8 gPokenavTrainerEyesMisc_Gfx[] = INCBIN_U8("graphics/pokenav/trainers_eyes/misc.4bpp.lz");

const u8 gPokenavTrainerEyesRematchIconTilemap[][4] =
{
    {0x80, 0x42, 0x82, 0x42},
};

const u8 gPokenavTrainerEyesNoRematchIconTilemap[][8] =
{
    {0x00, 0x40, 0x00, 0x40, 0x81, 0x42, 0x83, 0x42},
};

const u16 gPokenavRibbonsSummary_Pal[] = INCBIN_U16("graphics/pokenav/ribbons/summary.gbapal");
const u16 gUnusedPokenav3_Pal[] = INCBIN_U16("graphics/pokenav/unused/palette3.bin");
const u8 gPokenavRibbonIcons_Gfx[] = INCBIN_U8("graphics/pokenav/ribbons/icons.4bpp.lz");
const u8 gPokenavDeviceOutline_Pal[] = INCBIN_U8("graphics/pokenav/device_outline.gbapal");
const u8 gPokenavDeviceOutline_Gfx[] = INCBIN_U8("graphics/pokenav/device_outline.4bpp.lz");
const u8 gPokenavDeviceOutline_Tilemap[] = INCBIN_U8("graphics/pokenav/device_outline_map.bin.lz");

#include "data/region_map/city_map_tilemaps.h"

#ifdef ENGLISH
#include "data/text/ribbon_descriptions_en.h"
#include "data/text/gift_ribbon_descriptions_en.h"
#include "data/text/trainer_eye_descriptions_en.h"
#elif GERMAN
#include "data/text/ribbon_descriptions_de.h"
#include "data/text/gift_ribbon_descriptions_de.h"
#include "data/text/trainer_eye_descriptions_de.h"
#endif

const u8 *const gPokenavMainMenuDescriptions[] =
{
    PCText_CheckMap,
    PCText_CheckPoke,
    PCText_CheckTrainer,
    PCText_CheckRibbons,
    PCText_PutAwayNav,
    PCText_NoRibbonWin,
    PCText_NoTrainers,
};

const u8 *const gPokenavConditionMenuDescriptions[] =
{
    PCText_CheckParty,
    PCText_CheckPokeAll,
    PCText_ReturnToNav,
};

const u8 *const gPokenavConditionSearchDescriptions[] =
{
    PCText_FindCool,
    PCText_FindBeauty,
    PCText_FindCute,
    PCText_FindSmart,
    PCText_FindTough,
    PCText_ReturnToCondition,
};

#include "data/region_map/city_map_entries.h"

void PrintPokenavMonListRow(u16, u16);
void PrintTrainerEyesListRow(u16, u16);

void (*const gPokenavListRowPrinters[])(u16, u16) =
{
    [POKENAV_LIST_CONDITION_SEARCH] = PrintPokenavMonListRow,
    [POKENAV_LIST_RIBBONS] = PrintPokenavMonListRow,
    [POKENAV_LIST_TRAINERS_EYES] = PrintTrainerEyesListRow,
};

const u16 gPokenavSpinningIcon_Pal[] = INCBIN_U16("graphics/pokenav/nav_icon.gbapal");
const u8 gPokenavSpinningIcon_Gfx[] = INCBIN_U8("graphics/pokenav/nav_icon.4bpp.lz");
const u8 gPokenavConditionPokeball_Gfx[] = INCBIN_U8("graphics/pokenav/condition/pokeball.4bpp");
const u8 gPokenavConditionPokeballPlaceholder_Gfx[] = INCBIN_U8("graphics/pokenav/condition/pokeball_placeholder.4bpp");
const u16 gPokenavConditionSparkle_Pal[] = INCBIN_U16("graphics/pokenav/condition/sparkle.gbapal");
const u8 gPokenavConditionSparkle_Gfx[] = INCBIN_U8("graphics/pokenav/condition/sparkle.4bpp");
const u8 gPokenavListUpDownArrowGfx[] = INCBIN_U8("graphics/pokenav/list_arrows_up_down.4bpp");
const u8 gUnusedPokenav4_Pal[] =  INCBIN_U8("graphics/pokenav/unused/palette4.bin");
const u8 gUnusedPokenav_Gfx[] = INCBIN_U8("graphics/pokenav/unused.4bpp");

const u16 gPokenavRibbonIconPalettes[][16] =
{
    INCBIN_U16("graphics/pokenav/ribbons/icons1.gbapal"),
    INCBIN_U16("graphics/pokenav/ribbons/icons2.gbapal"),
    INCBIN_U16("graphics/pokenav/ribbons/icons3.gbapal"),
    INCBIN_U16("graphics/pokenav/ribbons/icons4.gbapal"),
    INCBIN_U16("graphics/pokenav/ribbons/icons5.gbapal"),
};

const u8 gPokenavRibbonIconsBigHalfGfx[] = INCBIN_U8("graphics/pokenav/ribbons/icons_big_half.4bpp.lz");
const u16 gPokenavBlueLightPalette[] = INCBIN_U16("graphics/pokenav/blue_light.gbapal");
const u8 gPokenavBlueLightTiles[] = INCBIN_U8("graphics/pokenav/blue_light.4bpp");
const u16 gPokenavTrainerEyesNamePalette[] = INCBIN_U16("graphics/pokenav/trainers_eyes/name_sprite.gbapal");

const u16 gPokenavMenuOptionBlendAlphas[] =
{
    BLDALPHA_BLEND(8, 4),
    BLDALPHA_BLEND(8, 4),
    BLDALPHA_BLEND(7, 5),
    BLDALPHA_BLEND(7, 5),
    BLDALPHA_BLEND(6, 6),
    BLDALPHA_BLEND(6, 6),
    BLDALPHA_BLEND(5, 8),
    BLDALPHA_BLEND(5, 8),
    BLDALPHA_BLEND(4, 10),
    BLDALPHA_BLEND(4, 10),
    BLDALPHA_BLEND(3, 12),
    BLDALPHA_BLEND(3, 12),
    BLDALPHA_BLEND(2, 14),
    BLDALPHA_BLEND(2, 14),
    BLDALPHA_BLEND(0, 16),
    BLDALPHA_BLEND(0, 16),
};

const struct OamData gPokenavMenuOptionOamData =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 1,
    .x = 0,
    .matrixNum = 0,
    .size = 2,
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sPokenavMenuOptionSpriteAnim0[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END,
};

static const union AnimCmd sPokenavMenuOptionSpriteAnim1[] =
{
    ANIMCMD_FRAME(8, 5),
    ANIMCMD_END,
};

static const union AnimCmd sPokenavMenuOptionSpriteAnim2[] =
{
    ANIMCMD_FRAME(16, 5),
    ANIMCMD_END,
};

static const union AnimCmd sPokenavMenuOptionSpriteAnim3[] =
{
    ANIMCMD_FRAME(24, 5),
    ANIMCMD_END,
};

static const union AnimCmd sPokenavMenuOptionSpriteAnim4[] =
{
    ANIMCMD_FRAME(32, 5),
    ANIMCMD_END,
};

static const union AnimCmd sPokenavMenuOptionSpriteAnim5[] =
{
    ANIMCMD_FRAME(40, 5),
    ANIMCMD_END,
};

static const union AnimCmd sPokenavMenuOptionSpriteAnim6[] =
{
    ANIMCMD_FRAME(48, 5),
    ANIMCMD_END,
};

static const union AnimCmd sPokenavMenuOptionSpriteAnim7[] =
{
    ANIMCMD_FRAME(56, 5),
    ANIMCMD_END,
};

static const union AnimCmd sPokenavMenuOptionSpriteAnim8[] =
{
    ANIMCMD_FRAME(64, 5),
    ANIMCMD_END,
};

static const union AnimCmd sPokenavMenuOptionSpriteAnim9[] =
{
    ANIMCMD_FRAME(72, 5),
    ANIMCMD_END,
};

static const union AnimCmd sPokenavMenuOptionSpriteAnim10[] =
{
    ANIMCMD_FRAME(80, 5),
    ANIMCMD_END,
};

static const union AnimCmd sPokenavMenuOptionSpriteAnim11[] =
{
    ANIMCMD_FRAME(88, 5),
    ANIMCMD_END,
};

static const union AnimCmd sPokenavMenuOptionSpriteAnim12[] =
{
    ANIMCMD_FRAME(96, 5),
    ANIMCMD_END,
};

static const union AnimCmd sPokenavMenuOptionSpriteAnim13[] =
{
    ANIMCMD_FRAME(104, 5),
    ANIMCMD_END,
};

static const union AnimCmd sPokenavMenuOptionSpriteAnim14[] =
{
    ANIMCMD_FRAME(112, 5),
    ANIMCMD_END,
};

static const union AnimCmd sPokenavMenuOptionSpriteAnim15[] =
{
    ANIMCMD_FRAME(120, 5),
    ANIMCMD_END,
};

static const union AnimCmd sPokenavMenuOptionSpriteAnim16[] =
{
    ANIMCMD_FRAME(128, 5),
    ANIMCMD_END,
};

static const union AnimCmd sPokenavMenuOptionSpriteAnim17[] =
{
    ANIMCMD_FRAME(136, 5),
    ANIMCMD_END,
};

static const union AnimCmd sPokenavMenuOptionSpriteAnim18[] =
{
    ANIMCMD_FRAME(144, 5),
    ANIMCMD_END,
};

static const union AnimCmd sPokenavMenuOptionSpriteAnim19[] =
{
    ANIMCMD_FRAME(152, 5),
    ANIMCMD_END,
};

static const union AnimCmd sPokenavMenuOptionSpriteAnim20[] =
{
    ANIMCMD_FRAME(160, 5),
    ANIMCMD_END,
};

static const union AnimCmd sPokenavMenuOptionSpriteAnim21[] =
{
    ANIMCMD_FRAME(168, 5),
    ANIMCMD_END,
};

static const union AnimCmd sPokenavMenuOptionSpriteAnim22[] =
{
    ANIMCMD_FRAME(176, 5),
    ANIMCMD_END,
};

static const union AnimCmd sPokenavMenuOptionSpriteAnim23[] =
{
    ANIMCMD_FRAME(184, 5),
    ANIMCMD_END,
};

const union AnimCmd *const gPokenavMenuOptionSpriteAnimTable[] =
{
    sPokenavMenuOptionSpriteAnim0,
    sPokenavMenuOptionSpriteAnim1,
    sPokenavMenuOptionSpriteAnim2,
    sPokenavMenuOptionSpriteAnim3,
    sPokenavMenuOptionSpriteAnim4,
    sPokenavMenuOptionSpriteAnim5,
    sPokenavMenuOptionSpriteAnim6,
    sPokenavMenuOptionSpriteAnim7,
    sPokenavMenuOptionSpriteAnim8,
    sPokenavMenuOptionSpriteAnim9,
    sPokenavMenuOptionSpriteAnim10,
    sPokenavMenuOptionSpriteAnim11,
    sPokenavMenuOptionSpriteAnim12,
    sPokenavMenuOptionSpriteAnim13,
    sPokenavMenuOptionSpriteAnim14,
    sPokenavMenuOptionSpriteAnim15,
    sPokenavMenuOptionSpriteAnim16,
    sPokenavMenuOptionSpriteAnim17,
    sPokenavMenuOptionSpriteAnim18,
    sPokenavMenuOptionSpriteAnim19,
    sPokenavMenuOptionSpriteAnim20,
    sPokenavMenuOptionSpriteAnim21,
    sPokenavMenuOptionSpriteAnim22,
    sPokenavMenuOptionSpriteAnim23,
};

const union AffineAnimCmd gPokenavMenuOptionZoomAffineAnim[] =
{
    AFFINEANIMCMD_FRAME(0x10, 0x10, 0, 16),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd *const gPokenavMenuOptionZoomAffineAnimTable[] =
{
    gPokenavMenuOptionZoomAffineAnim,
};

const struct SpriteTemplate gPokenavMenuOptionSpriteTemplate =
{
    .tileTag = 0,
    .paletteTag = 0,
    .oam = &gPokenavMenuOptionOamData,
    .anims = gPokenavMenuOptionSpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

// sizeof(gPokenavBlueLightTiles) ?
const struct SpriteSheet gSpriteSheet_PokenavBlueLight = {gPokenavBlueLightTiles, 0x100, 25};

const struct SpritePalette gSpritePalette_PokenavBlueLight = {gPokenavBlueLightPalette, 17};

const struct OamData gPokenavBlueLightOamData =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 1,
    .x = 0,
    .matrixNum = 0,
    .size = 2,
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

void SpriteCB_BlinkingBlueLight(struct Sprite *);

const struct SpriteTemplate gPokenavBlueLightSpriteTemplate =
{
    .tileTag = 25,
    .paletteTag = 17,
    .oam = &gPokenavBlueLightOamData,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_BlinkingBlueLight,
};

extern const u16 gPokenavMenuOptions3_Pal[];
extern const u16 gPokenavConditionHeader_Pal[];

const struct SpritePalette gPokenavMenuHeaderSpritePalettes[] =
{
    {gPokenavMenuOptions3_Pal, 2},
    {gPokenavConditionHeader_Pal, 3},
    {0x0, 0},
};

const struct OamData gPokenavLargeHeaderOamData =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = ST_OAM_H_RECTANGLE,
    .x = 0,
    .matrixNum = 0,
    .size = 3,
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

const union AnimCmd gPokenavLargeHeaderAnim0[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END,
};

const union AnimCmd gPokenavLargeHeaderAnim1[] =
{
    ANIMCMD_FRAME(32, 5),
    ANIMCMD_END,
};

const union AnimCmd gPokenavLargeHeaderAnim2[] =
{
    ANIMCMD_FRAME(64, 5),
    ANIMCMD_END,
};

const union AnimCmd *const gPokenavLargeHeaderAnimTable[] =
{
    gPokenavLargeHeaderAnim0,
    gPokenavLargeHeaderAnim1,
    gPokenavLargeHeaderAnim2,
};

void SpriteCB_SlideLeftHeaderIn(struct Sprite *);

const struct SpriteTemplate gPokenavLargeHeaderSpriteTemplate =
{
    .tileTag = 1,
    .paletteTag = 2,
    .oam = &gPokenavLargeHeaderOamData,
    .anims = gPokenavLargeHeaderAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_SlideLeftHeaderIn,
};

const struct SpriteTemplate gPokenavTrainersEyesHeaderSpriteTemplate =
{
    .tileTag = 1,
    .paletteTag = 3,
    .oam = &gPokenavLargeHeaderOamData,
    .anims = gPokenavLargeHeaderAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_SlideLeftHeaderIn,
};

const struct OamData gPokenavSmallHeaderOamData =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = ST_OAM_H_RECTANGLE,
    .x = 0,
    .matrixNum = 0,
    .size = 2,
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

const union AnimCmd gPokenavSmallHeaderAnim0[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END,
};

const union AnimCmd gPokenavSmallHeaderAnim1[] =
{
    ANIMCMD_FRAME(8, 5),
    ANIMCMD_END,
};

const union AnimCmd *const gPokenavSmallHeaderAnimTable[] =
{
    gPokenavSmallHeaderAnim0,
    gPokenavSmallHeaderAnim1,
};

const struct SpriteTemplate gPokenavSmallHeaderSpriteTemplate =
{
    .tileTag = 2,
    .paletteTag = 2,
    .oam = &gPokenavSmallHeaderOamData,
    .anims = gPokenavSmallHeaderAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_SlideLeftHeaderIn,
};

const struct SpriteTemplate gPokenavSmallHeaderAltSpriteTemplate =
{
    .tileTag = 2,
    .paletteTag = 3,
    .oam = &gPokenavSmallHeaderOamData,
    .anims = gPokenavSmallHeaderAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_SlideLeftHeaderIn,
};

const struct OamData gPokenavPortraitSpriteOamData =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = ST_OAM_SQUARE,
    .x = 0,
    .matrixNum = 0,
    .size = 3,
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

const struct SpriteSheet gPokenavPortraitSpriteSheet = {NULL, 0x800, 6};

void SpriteCB_UpdatePokenavPortraitSpriteX(struct Sprite *);

const struct SpriteTemplate gPokenavPortraitSpriteTemplate =
{
    .tileTag = 6,
    .paletteTag = 6,
    .oam = &gPokenavPortraitSpriteOamData,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_UpdatePokenavPortraitSpriteX,
};

const struct SpritePalette gPokenavPortraitSpritePalette = {NULL, 6};
extern const u8 gPokenavListRightArrowGfx[];

const struct SpriteSheet gPokenavListArrowSpriteSheets[] =
{
    {gPokenavListRightArrowGfx, 0x40, 9},
    {gPokenavListUpDownArrowGfx, 0x80, 10},
    {},
};

extern const u16 gPokenavListArrowPalette[];

const struct SpritePalette gPokenavListArrowSpritePalette = {gPokenavListArrowPalette, 9};

const struct OamData gPokenavListRightArrowOam =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = ST_OAM_V_RECTANGLE,
    .x = 0,
    .matrixNum = 0,
    .size = 0,
    .tileNum = 0,
    .priority = 2,
    .paletteNum = 0,
    .affineParam = 0,
};

void SpriteCB_UpdatePokenavListRightArrow(struct Sprite *);

const struct SpriteTemplate gPokenavListRightArrowSpriteTemplate =
{
    .tileTag = 9,
    .paletteTag = 9,
    .oam = &gPokenavListRightArrowOam,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_UpdatePokenavListRightArrow,
};

const union AnimCmd gPokenavListUpArrowAnim[] =
{
    ANIMCMD_FRAME(0, 3),
    ANIMCMD_END,
};

const union AnimCmd gPokenavListDownArrowAnim[] =
{
    ANIMCMD_FRAME(2, 3),
    ANIMCMD_END,
};

const union AnimCmd *const gPokenavListUpDownArrowAnims[] =
{
    gPokenavListUpArrowAnim,
    gPokenavListDownArrowAnim,
};

const struct OamData gPokenavListUpDownArrowOam =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = ST_OAM_H_RECTANGLE,
    .x = 0,
    .matrixNum = 0,
    .size = 0,
    .tileNum = 0,
    .priority = 2,
    .paletteNum = 0,
    .affineParam = 0,
};

void SpriteCB_UpdatePokenavListUpDownArrow(struct Sprite *);

const struct SpriteTemplate gPokenavListUpDownArrowSpriteTemplate =
{
    .tileTag = 10,
    .paletteTag = 9,
    .oam = &gPokenavListUpDownArrowOam,
    .anims = gPokenavListUpDownArrowAnims,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_UpdatePokenavListUpDownArrow,
};

const struct OamData gPokenavConditionSelectionIconOam =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = ST_OAM_SQUARE,
    .x = 0,
    .matrixNum = 0,
    .size = 1,
    .tileNum = 0,
    .priority = 2,
    .paletteNum = 0,
    .affineParam = 0,
};

const union AnimCmd gPokenavConditionIconSelectedAnim[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END,
};

const union AnimCmd gPokenavConditionIconUnselectedAnim[] =
{
    ANIMCMD_FRAME(4, 5),
    ANIMCMD_END,
};

const union AnimCmd *const gPokenavConditionIconAnims[] =
{
    gPokenavConditionIconSelectedAnim,
    gPokenavConditionIconUnselectedAnim,
};

extern const u8 gPokenavConditionCancel_Gfx[];

const struct SpriteSheet gPokenavConditionSelectionIconSheets[] =
{
    {gPokenavConditionPokeball_Gfx, 0x100, 3},
    {gPokenavConditionPokeballPlaceholder_Gfx, 0x20, 4},
    {gPokenavConditionCancel_Gfx, 0x100, 5},
    {},
};

extern const u16 gPokenavConditionPokeball_Pal[];
extern const u16 gPokenavConditionCancel_Pal[];

const struct SpritePalette gPokenavConditionSelectionIconPalettes[] =
{
    {gPokenavConditionPokeball_Pal, 4},
    {gPokenavConditionCancel_Pal, 5},
    {NULL, 0},
};

void SpriteCB_ConditionPartyPokeball(struct Sprite *);

const struct SpriteTemplate gPokenavConditionSelectionIconTemplate =
{
    .tileTag = 3,
    .paletteTag = 4,
    .oam = &gPokenavConditionSelectionIconOam,
    .anims = gPokenavConditionIconAnims,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_ConditionPartyPokeball,
};

const u16 gPokenavConditionMonMarkingsPalette[] = INCBIN_U16("graphics/pokenav/condition/mon_markings.gbapal");

const u16 gPokenavRibbonGfxData[][2] =
{
    {0, 0},
    {1, 0},
    {2, 0},
    {3, 0},
    {4, 0},
    {1, 1},
    {2, 1},
    {3, 1},
    {4, 1},
    {1, 2},
    {2, 2},
    {3, 2},
    {4, 2},
    {1, 3},
    {2, 3},
    {3, 3},
    {4, 3},
    {1, 4},
    {2, 4},
    {3, 4},
    {4, 4},
    {5, 0},
    {6, 0},
    {7, 1},
    {8, 2},
    {9, 1},
    {9, 3},
    {9, 4},
    {10, 3},
    {10, 4},
    {11, 0},
    {11, 1},
};

const struct OamData gPokenavBigRibbonOam =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_NORMAL,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = ST_OAM_SQUARE,
    .x = 0,
    .matrixNum = 0,
    .size = 2,
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};

const union AffineAnimCmd gPokenavBigRibbonNormalAnim[] =
{
    AFFINEANIMCMD_FRAME(0x80, 0x80, 0, 0),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd gPokenavBigRibbonZoomInAnim[] =
{
    AFFINEANIMCMD_FRAME(0x80, 0x80, 0, 0),
    AFFINEANIMCMD_FRAME(0x20, 0x20, 0, 4),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd gPokenavBigRibbonZoomOutAnim[] =
{
    AFFINEANIMCMD_FRAME(0x100, 0x100, 0, 0),
    AFFINEANIMCMD_FRAME(0xFFE0, 0xFFE0, 0, 4),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd *const gPokenavBigRibbonAffineAnims[] =
{
    gPokenavBigRibbonNormalAnim,
    gPokenavBigRibbonZoomInAnim,
    gPokenavBigRibbonZoomOutAnim,
};

const struct SpriteTemplate gPokenavBigRibbonSpriteTemplate =
{
    .tileTag = 11,
    .paletteTag = 10,
    .oam = &gPokenavBigRibbonOam,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gPokenavBigRibbonAffineAnims,
    .callback = SpriteCallbackDummy,
};

const struct SpriteSheet gPokenavConditionSparkleSpriteSheet = {gPokenavConditionSparkle_Gfx, sizeof(gPokenavConditionSparkle_Gfx), 0x0017};
const struct SpritePalette gPokenavConditionSparkleSpritePalette = {gPokenavConditionSparkle_Pal, 0x000f};

const s16 gPokenavConditionSparkleCoords[][2] =
{
    { 0 , -35},
    { 20, -28},
    { 33, -10},
    { 33,  10},
    { 20,  28},
    { 0 ,  35},
    {-20,  28},
    {-33,  10},
    {-33, -10},
    {-20, -28},
};

const struct OamData gPokenavConditionSparkleOam =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = ST_OAM_SQUARE,
    .x = 0,
    .matrixNum = 0,
    .size = 1,
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};

const union AnimCmd gPokenavConditionSparkleAnim[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_FRAME(4, 5),
    ANIMCMD_FRAME(8, 5),
    ANIMCMD_FRAME(12, 5),
    ANIMCMD_FRAME(16, 5),
    ANIMCMD_FRAME(20, 5),
    ANIMCMD_FRAME(24, 5),
    ANIMCMD_END,
};

// sparkles around pokemon on condition page of pokenav
const union AnimCmd *const gPokenavConditionSparkleAnims[] =
{
    &gPokenavConditionSparkleAnim[0],
    &gPokenavConditionSparkleAnim[2],
    &gPokenavConditionSparkleAnim[4],
    &gPokenavConditionSparkleAnim[6],
    &gPokenavConditionSparkleAnim[8],
    &gPokenavConditionSparkleAnim[10],
    &gPokenavConditionSparkleAnim[12],
};

void SpriteCB_ConditionSparkle(struct Sprite *sprite);

const struct SpriteTemplate gPokenavConditionSparkleSpriteTemplate =
{
    .tileTag = 23,
    .paletteTag = 15,
    .oam = &gPokenavConditionSparkleOam,
    .anims = gPokenavConditionSparkleAnims,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_ConditionSparkle,
};

const struct SpritePalette gPokenavSpinningIconSpritePalette = {gPokenavSpinningIcon_Pal, 16};

const union AnimCmd gPokenavSpinningIconAnim[] =
{
    ANIMCMD_FRAME(0, 12),
    ANIMCMD_FRAME(16, 12),
    ANIMCMD_FRAME(32, 12),
    ANIMCMD_FRAME(48, 12),
    ANIMCMD_FRAME(64, 12),
    ANIMCMD_FRAME(80, 12),
    ANIMCMD_FRAME(96, 12),
    ANIMCMD_FRAME(112, 12),
    ANIMCMD_JUMP(0),
};

const union AnimCmd *const gPokenavSpinningIconAnims[] =
{
    gPokenavSpinningIconAnim,
};

const struct OamData gPokenavSpinningIconOam =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = ST_OAM_SQUARE,
    .x = 0,
    .matrixNum = 0,
    .size = 2,
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};

void SpriteCB_UpdatePokenavSpinningIconPosition(struct Sprite *);

const struct SpriteTemplate gPokenavSpinningIconSpriteTemplate =
{
    .tileTag = 24,
    .paletteTag = 16,
    .oam = &gPokenavSpinningIconOam,
    .anims = gPokenavSpinningIconAnims,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_UpdatePokenavSpinningIconPosition,
};

const struct SpritePalette gPokenavTrainerEyesNameSpritePalette = {gPokenavTrainerEyesNamePalette, 18};

const struct OamData gPokenavTrainerEyesNameOam =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = ST_OAM_H_RECTANGLE,
    .x = 0,
    .matrixNum = 0,
    .size = 2,
    .tileNum = 0,
    .priority = 3,
    .paletteNum = 0,
    .affineParam = 0,
};

const struct SpriteTemplate gPokenavTrainerEyesNameSpriteTemplate =
{
    .tileTag = 26,
    .paletteTag = 18,
    .oam = &gPokenavTrainerEyesNameOam,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

const u8 gConditionToLineLength[] =
{   
    4,   5,  6,  7,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 13, 
    13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 17, 
    17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 
    19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 
    21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 
    23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 
    24, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 
    26, 26, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 
    27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 28, 
    28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 
    29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 
    30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 
    31, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 
    32, 32, 32, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 33, 33, 33, 
    33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 
    34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 35,
};

const struct ScanlineEffectParams gConditionGraphScanlineParams =
{
    (void *)REG_ADDR_WIN0H,
    ((DMA_ENABLE | DMA_START_HBLANK | DMA_32BIT | DMA_REPEAT | DMA_DEST_RELOAD) << 16) | 1,
    1,
    0,
};

const u16 gRibbonSummaryMonDataFields[] =
{
    MON_DATA_CHAMPION_RIBBON,
    MON_DATA_COOL_RIBBON,
    MON_DATA_BEAUTY_RIBBON,
    MON_DATA_CUTE_RIBBON,
    MON_DATA_SMART_RIBBON,
    MON_DATA_TOUGH_RIBBON,
    MON_DATA_WINNING_RIBBON,
    MON_DATA_VICTORY_RIBBON,
    MON_DATA_ARTIST_RIBBON,
    MON_DATA_EFFORT_RIBBON,
    MON_DATA_MARINE_RIBBON,
    MON_DATA_LAND_RIBBON,
    MON_DATA_SKY_RIBBON,
    MON_DATA_COUNTRY_RIBBON,
    MON_DATA_NATIONAL_RIBBON,
    MON_DATA_EARTH_RIBBON,
    MON_DATA_WORLD_RIBBON,
};

void CB2_InitPokeNav(void)
{
    switch (gMain.state)
    {
    default:
        gMain.state = 0;
    case 0:
        gPokenavState->isLinkActive = IsOverworldLinkActive();
        if (!gPokenavState->isLinkActive)
        {
            gPokenavState->isPokenavInitialized = 0;
            gMain.state++;
            SetMainCallback2(&CB2_ContinueInitPokeNav);
        }
        break;
    case 1:
        SetVBlankCallback(NULL);
        break;
    case 2:
        ResetPaletteFade();
        break;
    case 3:
        ResetSpriteData();
        break;
    case 4:
        FreeAllSpritePalettes();
        break;
    case 5:
        ResetTasks();
        break;
    case 6:
        ScanlineEffect_Stop();
        break;
    case 7:
        InitPokenavMenuOptionGfx();
        gMain.state++;
    case 8:
        if (LoadPokenavMenuOptionGfxStep())
            return;
        break;
    case 9:
        InitPokenavMenuHeaderGfx();
        gMain.state++;
        // fall through
    case 10:
        if (LoadPokenavMenuHeaderGfxStep())
            return;
        break;
    case 11:
        InitRibbonIconGfx();
        gMain.state++;
    case 12:
        if (LoadRibbonIconGfxStep())
            return;
        break;
    case 13:
        InitPokenavState();
        SetPokenavCallback(&InitPokenavMainMenu);
        break;
    case 14:
        gPokenavState->isPokenavInitialized = 1;
        PlaySE(SE_POKENAV_ON);
        SetMainCallback2(&CB2_Pokenav);
        SetVBlankCallback(&VBlankCB_Pokenav);
        StopRegionMapLinkLandmarks();
        break;
    }

    gMain.state++;
}

void CB2_ContinueInitPokeNav(void)
{
    while (!gPokenavState->isPokenavInitialized)
        CB2_InitPokeNav();
}

void InitPokenavState(void)
{
    u16 i;

    gKeyRepeatStartDelay = 0x14;
    gPokenavState->playerPartyCount = CalculatePlayerPartyCount();
    gPokenavState->mainMenuCursorPos = 0;
    gPokenavState->conditionGraph.needsDraw = 0;
    gPokenavState->portraitSprite = 0;
    gPokenavState->trainerEyesPortraitSprite = 0;

    for (i = 0; i < 5; ++i)
    {
        gPokenavState->conditionGraph.conditions[3][i] = 0;
        gPokenavState->conditionGraph.savedPositions[3][i].x = 0x9B;
        gPokenavState->conditionGraph.savedPositions[3][i].y = 0x5B;
    }

    gPokenavState->regionMap.needUpdateVideoRegs = 0;
    InitPokenavMenuOptions();
}

void InitPokenavMenuOptions(void)
{
    gPokenavState->mainMenuItemIds[0] = 1;
    gPokenavState->mainMenuItemIds[1] = 2;
    gPokenavState->mainMenuItemIds[2] = 3;

    if (FlagGet(FLAG_SYS_RIBBON_GET))
        gPokenavState->mainMenuItemIds[3] = 4;
    else
        gPokenavState->mainMenuItemIds[3] = 0;

    gPokenavState->mainMenuItemIds[4] = 5;
}

void VBlankCB_Pokenav(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    UpdateMenuOptionBlendRegisters();
}

void VBlankCB_PokenavRegionMap(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    UpdateRegionMapVideoRegs();
    UpdateRegionMapCityBanner();
}

void VBlankCB_PokenavConditionGraph(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    ConditionGraph_Draw();
    ScanlineEffect_InitHBlankDmaTransfer();
}

void VBlankCB_PokenavTrainerEyes(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    UpdateTrainerEyesBackgroundPulse();
}

void VBlankCB_PokenavOamOnly(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
}

void CB2_Pokenav(void)
{
    gPokenavState->callback();
    AnimateSprites();
    BuildOamBuffer();
    RunTasks();
    UpdatePaletteFade();
    UpdateRegionMapLinkLandmarks();
}

void SetPokenavCallback(void (*callback)(void))
{
    gPokenavState->callback = callback;
    gPokenavState->callbackStep = 0;
}

void InitPokenavMainMenu(void)
{
    switch (gPokenavState->callbackStep)
    {
    case 0:
        SetVBlankCallback(NULL);
        REG_DISPCNT = 0;
        gPokenavState->menuCursorPos = gPokenavState->mainMenuCursorPos;
        gPokenavState->menuItemCount = 5;
        gPokenavState->callbackStep++;
        break;
    case 1:
        InitPokenavSpinningIconSprite();
        gPokenavState->callbackStep++;
        // fall through
    case 2:
        if (!LoadPokenavSpinningIconSpriteStep())
			gPokenavState->callbackStep++;
        break;
    case 3:
        CreateRematchBlueLightSprite();
        gPokenavState->callbackStep++;
        break;
    case 4:
        InitPokenavBackground();
        gPokenavState->callbackStep++;
        // fall through
    case 5:
        if (!LoadPokenavBackgroundStep())
		{
			ResetPokenavSetupStep();
			gPokenavState->callbackStep++;
		}
        break;
    case 6:
        InitPokenavMenuGfx(0);
        gPokenavState->callbackStep++;
        // fall through
    case 7:
        if (!LoadPokenavMenuGfxStep(0))
			gPokenavState->callbackStep++;
        break;
    case 8:
        InitPokenavMenuOptionSprites(0);
        gPokenavState->callbackStep++;
		// fall through
    case 9:
        if (!LoadPokenavMenuOptionSpritesStep(0))
			gPokenavState->callbackStep++;
        break;
    case 10:
        SetVBlankCallback(&VBlankCB_Pokenav);
        gPokenavState->callbackStep++;
        break;
    case 11:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        gPokenavState->callbackStep++;
        break;
    case 12:
        SetPokenavDisplayForScreen(0);
        gPokenavState->callbackStep++;
        break;
    case 13:
        if (!gPaletteFade.active)
			gPokenavState->callbackStep++;
        break;
    case 14:
        BeginPokenavLeftHeaderLoad(0);
        gPokenavState->callbackStep++;
        // fall through
    case 15:
        if (!LoadPokenavLeftHeaderStep(0))
			gPokenavState->callbackStep++;
        break;
    case 16:
        StartMenuOptionSpritesSlideIn();
        gPokenavState->callbackStep++;
        break;
    case 17:
        if (!UpdateMenuOptionEntryHighlight())
        {
            PrintPokenavMenuDescription(0, 0);
            SetPokenavCallback(&HandlePokenavMainMenuInput);
#if DEBUG
			if (gLinkOpen == TRUE)
				InitDebugHexPrinter((void *)(VRAM + 0x7DE0), 0, (void *)(VRAM + 0xF800), 4);
#endif
        }
        break;
    }
}

void RestorePokenavMainMenu(void)
{
    switch (gPokenavState->callbackStep)
    {
    case 0:
        if (!SlideMenuHeaderDown())
        {
            SetVBlankCallback(&VBlankCB_PokenavOamOnly);
            SetPokenavDisplayForMenu();
            gPokenavState->menuCursorPos = gPokenavState->mainMenuCursorPos;
            gPokenavState->menuItemCount = 5;
            ResetPokenavSetupStep();
            gPokenavState->callbackStep++;
        }
        break;
    case 1:
        InitPokenavMenuGfx(0);
        gPokenavState->callbackStep++;
        // fall through
    case 2:
        if (!LoadPokenavMenuGfxStep(0))
			gPokenavState->callbackStep++;
        break;
    case 3:
        InitPokenavMenuOptionSprites(0);
        gPokenavState->callbackStep++;
        // fall through
    case 4:
        if (!LoadPokenavMenuOptionSpritesStep(0))
			gPokenavState->callbackStep++;
        break;
    case 5:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavState->callbackStep++;
        break;
    case 6:
        BeginNormalPaletteFade(gPokenavState->paletteFadeMask, -1, 16, 0, RGB(0, 0, 0));
        SetVBlankCallback(&VBlankCB_Pokenav);
        gPokenavState->callbackStep++;
        break;
    case 7:
        SetPokenavDisplayForScreen(0);
        gPokenavState->callbackStep++;
        break;
    case 8:
        if (!gPaletteFade.active)
			gPokenavState->callbackStep++;
        break;
    case 9:
        CreateRematchBlueLightSprite();
        gPokenavState->callbackStep++;
        break;
    case 10:
        BeginPokenavLeftHeaderLoad(0);
        gPokenavState->callbackStep++;
        // fall through
    case 11:
        if (!LoadPokenavLeftHeaderStep(0))
			gPokenavState->callbackStep++;
        break;
    case 12:
        StartMenuOptionSpritesSlideIn();
        gPokenavState->callbackStep++;
        break;
    case 13:
        if (!UpdateMenuOptionEntryHighlight())
        {
            PrintPokenavMenuDescription(0, gPokenavState->menuCursorPos);
			SetPokenavCallback(&HandlePokenavMainMenuInput);
#if DEBUG
			if (gLinkOpen == TRUE)
				InitDebugHexPrinter((void *)(VRAM + 0x7DE0), 0, (void *)(VRAM + 0xF800), 4);
#endif
		}
		break;
    }
}

void ClosePokenavConditionMenu(void)
{
    switch (gPokenavState->callbackStep)
    {
    case 0:
        StartPokenavLeftHeaderSlideOut(1);
        StartPokenavMenuTransition(0, gPokenavState->mainMenuCursorPos, 0);
        gPokenavState->callbackStep++;
        break;
    case 1:
        if (!UpdatePokenavMenuTransition())
			SetPokenavCallback(&HandlePokenavMainMenuInput);
		break;
    }
}

void HandlePokenavMainMenuInput(void)
{
    u8 cursorMoved;

    switch (gPokenavState->callbackStep)
    {
    case 0:
        if (UpdateMainMenuCursorPos() != 0)
        {
            PlaySE(SE_SELECT);
            PrintPokenavMenuDescription(0, gPokenavState->menuCursorPos);
            PausePokenavCallbackForLink();
        }
        else
        {
            if (JOY_NEW(A_BUTTON))
            {
                gPokenavState->mainMenuCursorPos = gPokenavState->menuCursorPos;
                switch (gPokenavState->mainMenuItemIds[gPokenavState->mainMenuCursorPos] - 1)
                {
                case 0:
                    PlaySE(SE_SELECT);
                    SetPokenavCallback(&OpenPokenavRegionMap);
                    break;
                case 1:
                    PlaySE(SE_SELECT);
                    SetPokenavCallback(&OpenPokenavConditionMenu);
                    break;
                case 4:
                    gPokenavState->callbackStep = 1;
                    break;
                case 3:
                    gPokenavState->callbackStep = 2;
                    break;
                case 2:
                    gPokenavState->callbackStep = 6;
                    break;
                }
            }
            else if (JOY_NEW(B_BUTTON))
            {
                gPokenavState->callbackStep = 1;
            }
        }
        break;
    case 1:
        ResetMenuOptionBlendOnPokenavExit();
        SetPokenavCallback(&ExitPokenav);
        break;
    case 2:
        InitAnyMonHasRibbonScan();
        gPokenavState->callbackStep++;
        // fall through
    case 3:
        if (!AnyMonHasRibbonScanStep())
			gPokenavState->callbackStep++;
        break;
    case 4:
        if (!Overworld_IsRecvQueueAtMax())
        {
			if (gPokenavState->hasAnyRibbons != 0)
			{
				PlaySE(SE_SELECT);
				SetPokenavCallback(&OpenRibbonsMonList);
			}
			else
			{
				PlaySE(0x20);
				PrintPokenavMenuDescription(0, 5);
				gPokenavState->callbackStep = 0xFF;
			}
		}
		break;
    case 6:
        BuildTrainerEyesList();
        if (gPokenavState->trainerEyesCount != 0)
        {
            PlaySE(SE_SELECT);
            SetPokenavCallback(&OpenTrainerEyes);
        }
        else
        {
            PlaySE(0x20);
            PrintPokenavMenuDescription(0, 6);
            gPokenavState->callbackStep = 0xFF;
        }
        break;
    case 0xFF:
        if ((cursorMoved = UpdateMainMenuCursorPos()) != 0)
        {
            PlaySE(SE_SELECT);
            PrintPokenavMenuDescription(0, gPokenavState->menuCursorPos);
            gPokenavState->callbackStep = 0;
            PausePokenavCallbackForLink();
        }
        else if (JOY_NEW(A_BUTTON | B_BUTTON))
		{
			PrintPokenavMenuDescription(0, gPokenavState->menuCursorPos);
			gPokenavState->callbackStep = cursorMoved;
		}
        break;
    }
}

void OpenPokenavRegionMap(void)
{
    u32 toolbarId;

    switch (gPokenavState->callbackStep)
    {
    case 0:
        StartMenuOptionSpritesSlideOut();
        StartPokenavLeftHeaderSlideOut(0);
        gPokenavState->callbackStep++;
        break;
    case 1:
        if (!UpdateMenuOptionSpritesSlideOut())
        {
            if (gSaveBlock2.regionMapZoom)
                toolbarId = 0x7;
            else
				toolbarId = 0x8;
            DrawPokenavBottomToolbar(toolbarId);
            gPokenavState->callbackStep++;
        }
        break;
    case 2:
        if (!SlideMenuHeaderUp())
        {
            BeginNormalPaletteFade(gPokenavState->paletteFadeMask, -1, 0, 16, RGB(0, 0, 0));
            gPokenavState->callbackStep++;
        }
        break;
    case 3:
        if (!gPaletteFade.active)
        {
			SetVBlankCallback(NULL);
			SetPokenavDisplayForTransition();
			gPokenavState->callbackStep++;
		}
        break;
    case 4:
        DestroyRematchBlueLightSprite();
        gPokenavState->callbackStep++;
        break;
    case 5:
        StopPokenavBg3Scroll();
        gPokenavState->callbackStep++;
        break;
    case 6:
        InitPokenavRegionMapGfx();
        gPokenavState->callbackStep++;
        // fall through
    case 7:
        if (!LoadPokenavRegionMapGfxStep())
			gPokenavState->callbackStep++;
        break;
    case 8:
        BeginPokenavLeftHeaderLoad(0x4);
        gPokenavState->callbackStep++;
        // fall through
    case 9:
        if (!LoadPokenavLeftHeaderStep(0x4))
			gPokenavState->callbackStep++;
        break;
    case 0xA:
        CreatePokenavRegionMapIcons();
        SetVBlankCallback(&VBlankCB_PokenavRegionMap);
        gPokenavState->callbackStep++;
        break;
    case 0xB:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavState->callbackStep++;
        break;
    case 0xC:
        BeginNormalPaletteFade(gPokenavState->paletteFadeMask, -1, 16, 0, RGB(0, 0, 0));
        gPokenavState->callbackStep++;
        break;
    case 0xD:
        SetPokenavDisplayForScreen(0x1);
#if DEBUG
		if (gLinkOpen == TRUE)
			InitDebugHexPrinter((void *)(VRAM + 0x75E0), 0, (void *)(VRAM + 0xF800), 4);
#endif
        gPokenavState->callbackStep++;
        break;
    case 0xE:
        if (!gPaletteFade.active)
			SetPokenavCallback(&HandleRegionMapInput);
		break;
    }
}

void HandleRegionMapInput(void)
{
    switch (gPokenavState->callbackStep)
    {
    case 0:
        switch (DoRegionMapInputCallback())
        {
        case 1:
            PausePokenavCallbackForLink();
            break;
        case 3:
            UpdateMapSecInfoWindow();
            break;
        case 4:
            PlaySE(SE_SELECT);
            gPokenavState->callbackStep = 1;
            break;
        case 5:
            PlaySE(SE_SELECT);
            gPokenavState->callbackStep = 4;
            break;
        }
        break;
    case 1:
        if (!gPokenavState->regionMap.zoomed)
        {
            SetRegionMapDataForZoom();
            gPokenavState->callbackStep++;
        }
        else if (!UpdateRegionMapBgYForZoom(1))
        {
			SetRegionMapDataForZoom();
			gPokenavState->callbackStep++;
		}
        break;
    case 2:
        if (!UpdateRegionMapZoom())
        {
			if (!gPokenavState->regionMap.zoomed)
			{
				UpdateRegionMapBottomToolbar();
				gPokenavState->callbackStep = 0;
			}
			else
			{
				gPokenavState->callbackStep++;
			}
		}
        break;
    case 3:
        if (!UpdateRegionMapBgYForZoom(0))
        {
			UpdateRegionMapBottomToolbar();
			gPokenavState->callbackStep = 0;
		}
        break;
    case 4:
        BeginNormalPaletteFade(gPokenavState->paletteFadeMask, -1, 0, 16, RGB(0, 0, 0));
        gPokenavState->callbackStep++;
        break;
    case 5:
        if (!gPaletteFade.active)
        {
			FreePokenavRegionMapIcons();
			StartPokenavLeftHeaderSlideOut(0x4);
			gSaveBlock2.regionMapZoom = (gPokenavState->regionMap.zoomed == 1) ? 1 : 0;
			SetPokenavCallback(&RestorePokenavMainMenu);
		}
		break;
    }
#if DEBUG
	if (gLinkOpen == TRUE)
		PrintDebugHex(gLink.recvQueue.count, 1, 1, 2, 4);
#endif
}

void OpenPokenavConditionMenu(void)
{
    switch (gPokenavState->callbackStep)
    {
    case 0:
        StartPokenavLeftHeaderSlideOut(0);
        StartPokenavMenuTransition(1, 0, 1);
        gPokenavState->callbackStep++;
        break;
    case 1:
        if (!UpdatePokenavMenuTransition())
			SetPokenavCallback(&HandleConditionMenuInput);
		break;
    }
}

void HandleConditionMenuInput(void)
{
    switch (gPokenavState->callbackStep)
    {
    case 0:
        if (UpdateSubmenuCursorPos())
        {
            PlaySE(SE_SELECT);
            PrintPokenavMenuDescription(1, gPokenavState->menuCursorPos);
            PausePokenavCallbackForLink();
        }
        else if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            gPokenavState->conditionMenuCursorPos = gPokenavState->menuCursorPos;
            switch (gPokenavState->conditionMenuCursorPos)
            {
            case 0:
                SetPokenavCallback(&OpenConditionGraphFromParty);
                break;
            case 1:
                SetPokenavCallback(&OpenPokenavConditionSearchMenu);
                break;
            case 2:
                SetPokenavCallback(&ClosePokenavConditionMenu);
                break;
            }
        }
        else if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            gPokenavState->conditionMenuCursorPos = 0x2;
            gPokenavState->menuCursorPos = 0x2;
            gPokenavState->callbackStep++;
        }
        break;
    case 1:
        if (!AreMenuOptionSpriteOffsetsMoving())
			SetPokenavCallback(&ClosePokenavConditionMenu);
		break;
    }
}

void OpenPokenavConditionSearchMenu(void)
{
    switch (gPokenavState->callbackStep)
    {
    case 0:
        StartPokenavMenuTransition(2, 0, 5);
        gPokenavState->callbackStep++;
        break;
    case 1:
        if (!UpdatePokenavMenuTransition())
			SetPokenavCallback(&HandleConditionSearchMenuInput);
		break;
    }
}

void ReturnToConditionMenu(void)
{
    switch (gPokenavState->callbackStep)
    {
    case 0:
        StartPokenavLeftHeaderSlideOut(5);
        StartPokenavMenuTransition(1, gPokenavState->conditionMenuCursorPos, 0xC);
        gPokenavState->callbackStep++;
        break;
    case 1:
        if (!UpdatePokenavMenuTransition())
        {
			SetPokenavCallback(&HandleConditionMenuInput);
			PrintPokenavMenuDescription(1, gPokenavState->menuCursorPos);
		}
		break;
    }
}

void RestorePokenavConditionMenu(void)
{
    switch (gPokenavState->callbackStep)
    {
    case 0:
        if (!SlideMenuHeaderDown())
        {
			SetVBlankCallback(&VBlankCB_PokenavOamOnly);
			SetPokenavDisplayForMenu();
			gPokenavState->menuCursorPos = gPokenavState->conditionMenuCursorPos;
			gPokenavState->menuItemCount = 3;
			ResetPokenavSetupStep();
			gPokenavState->callbackStep++;
		}
        break;
    case 1:
        InitPokenavMenuGfx(1);
        gPokenavState->callbackStep++;
    case 2:
        if (!LoadPokenavMenuGfxStep(1))
			gPokenavState->callbackStep++;
        break;
    case 3:
        InitPokenavMenuOptionSprites(1);
        gPokenavState->callbackStep++;
    case 4:
        if (!LoadPokenavMenuOptionSpritesStep(1))
			gPokenavState->callbackStep++;
        break;
    case 5:
        if (Overworld_IsRecvQueueAtMax()) return;
        gPokenavState->callbackStep++;
        break;
    case 6:
        BeginNormalPaletteFade(gPokenavState->paletteFadeMask, -1, 16, 0, RGB(0, 0, 0));
        SetVBlankCallback(&VBlankCB_Pokenav);
        gPokenavState->callbackStep++;
        break;
    case 7:
        SetPokenavDisplayForScreen(0);
        gPokenavState->callbackStep++;
        break;
    case 8:
        CreateRematchBlueLightSprite();
        gPokenavState->callbackStep++;
        break;
    case 9:
        if (!gPaletteFade.active)
			gPokenavState->callbackStep++;
        break;
    case 10:
        BeginPokenavLeftHeaderLoad(1);
        gPokenavState->callbackStep++;
		// fall through
    case 11:
        if (!LoadPokenavLeftHeaderStep(1))
			gPokenavState->callbackStep++;
        break;
    case 12:
        StartMenuOptionSpritesSlideIn();
        gPokenavState->callbackStep++;
        break;
    case 13:
        if (!UpdateMenuOptionEntryHighlight())
        {
			PrintPokenavMenuDescription(1, gPokenavState->menuCursorPos);
			SetPokenavCallback(&HandleConditionMenuInput);
#if DEBUG
			if (gLinkOpen == TRUE)
				InitDebugHexPrinter((void *)(VRAM + 0x7DE0), 0, (void *)(VRAM + 0xF800), 4);
#endif
		}
		break;
    }
}

void HandleConditionSearchMenuInput(void)
{
    switch (gPokenavState->callbackStep)
    {
    case 0:
        if (UpdateSubmenuCursorPos())
        {
            PlaySE(SE_SELECT);
            PrintPokenavMenuDescription(2, gPokenavState->menuCursorPos);
            PausePokenavCallbackForLink();
        }
        else if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            switch ((s8)gPokenavState->menuCursorPos)
            {
            case 0:
                gPokenavState->conditionSearchMonDataId = 22;
                break;
            case 1:
                gPokenavState->conditionSearchMonDataId = 23;
                break;
            case 2:
                gPokenavState->conditionSearchMonDataId = 24;
                break;
            case 3:
                gPokenavState->conditionSearchMonDataId = 33;
                break;
            case 4:
                gPokenavState->conditionSearchMonDataId = 47;
                break;
            case 5:
                SetPokenavCallback(&ReturnToConditionMenu);
                return;
            }

            gPokenavState->conditionSearchCursorPos = gPokenavState->menuCursorPos;
            gPokenavState->isConditionGraphSearchMode = 1;
            SetPokenavCallback(&OpenConditionSearchResults);
        }
        else if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            gPokenavState->menuCursorPos = 0x5;
            gPokenavState->callbackStep++;
        }
        break;
    case 1:
        if (!AreMenuOptionSpriteOffsetsMoving())
			SetPokenavCallback(&ReturnToConditionMenu);
        break;
    }
}

void ReturnToConditionSearchMenu(void)
{
    switch (gPokenavState->callbackStep)
    {
    case 0:
        BeginNormalPaletteFade(gPokenavState->paletteFadeMask, -1, 0, 16, RGB(0, 0, 0));
        gPokenavState->callbackStep++;
        break;
    case 1:
        if (!gPaletteFade.active)
			gPokenavState->callbackStep++;
        break;
    case 2:
        if (!SlideMenuHeaderDown())
        {
			SetVBlankCallback(&VBlankCB_PokenavOamOnly);
			gPokenavState->callbackStep++;
		}
        break;
    case 3:
        SetPokenavDisplayForMenu();
        DestroyPokenavListArrowSprites();
        gPokenavState->callbackStep++;
        break;
    case 4:
        DestroyPokenavLeftHeaderSprites(0x1);
        gPokenavState->callbackStep++;
        break;
    case 5:
        DestroyPokenavLeftHeaderSprites(0x5);
        gPokenavState->callbackStep++;
        break;
    case 6:
        gPokenavState->menuCursorPos = gPokenavState->conditionSearchCursorPos;
        gPokenavState->menuItemCount = 0x6;
        ResetPokenavSetupStep();
        gPokenavState->callbackStep++;
        break;
    case 7:
        InitPokenavMenuGfx(0x2);
        gPokenavState->callbackStep++;
        // fall through
    case 8:
        if (!LoadPokenavMenuGfxStep(0x2))
			gPokenavState->callbackStep++;
        break;
    case 9:
        InitPokenavMenuOptionSprites(2);
        gPokenavState->callbackStep++;
        // fall through
    case 10:
        if (!LoadPokenavMenuOptionSpritesStep(2))
			gPokenavState->callbackStep++;
        break;
    case 11:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavState->callbackStep++;
        break;
    case 12:
        BeginNormalPaletteFade(gPokenavState->paletteFadeMask, -1, 16, 0, RGB(0, 0, 0));
        SetVBlankCallback(&VBlankCB_Pokenav);
        gPokenavState->callbackStep++;
        break;
    case 13:
        SetPokenavDisplayForScreen(0);
        gPokenavState->callbackStep++;
        break;
    case 14:
        CreateRematchBlueLightSprite();
        gPokenavState->callbackStep++;
        break;
    case 15:
        if (!gPaletteFade.active)
			gPokenavState->callbackStep++;
        break;
    case 16:
        BeginPokenavLeftHeaderLoad(0x1);
        gPokenavState->callbackStep++;
        // fall through
    case 17:
        if (!LoadPokenavLeftHeaderStep(0x1))
        {
			gPokenavState->loadState = 0;
			gPokenavState->callbackStep++;
		}
        break;
    case 18:
        BeginPokenavLeftHeaderLoad(0x5);
        gPokenavState->callbackStep++;
        // fall through
    case 19:
        if (!LoadPokenavLeftHeaderStep(0x5))
        {
			StartMenuOptionSpritesSlideIn();
			gPokenavState->callbackStep++;
		}
        break;
    case 20:
        if (!UpdateMenuOptionEntryHighlight())
		{
			PrintPokenavMenuDescription(2, gPokenavState->menuCursorPos);
			SetPokenavCallback(&HandleConditionSearchMenuInput);
#if DEBUG
			if (gLinkOpen == TRUE)
				InitDebugHexPrinter((void *)(VRAM + 0x7DE0), 0, (void *)(VRAM + 0xF800), 4);
#endif
		}
		break;
    }
}

void OpenConditionSearchResults(void)
{
    switch (gPokenavState->callbackStep)
    {
    case 0:
        StartMenuOptionSpritesSlideOut();
        StartPokenavLeftHeaderSlideOut(0x1);
        StartPokenavLeftHeaderSlideOut(0x5);
        gPokenavState->callbackStep++;
        break;
    case 1:
        if (!UpdateMenuOptionSpritesSlideOut())
        {
			DrawPokenavBottomToolbar(0);
			gPokenavState->callbackStep++;
		}
        break;
    case 2:
        if (!SlideMenuHeaderUp())
        {
			BeginNormalPaletteFade(gPokenavState->paletteFadeMask, -1, 0, 16, RGB(0, 0, 0));
			gPokenavState->callbackStep++;
		}
        break;
    case 3:
        if (!gPaletteFade.active)
        {
			SetVBlankCallback(NULL);
			SetPokenavDisplayForTransition();
			StopPokenavBg3Scroll();
			gPokenavState->callbackStep++;
		}
        break;
    case 4:
        DestroyRematchBlueLightSprite();
        gPokenavState->callbackStep++;
        break;
    case 5:
        InitConditionSearchResults();
        gPokenavState->callbackStep++;
        // fall through
    case 6:
        if (!BuildConditionSearchResultsStep())
            gPokenavState->callbackStep += 2;
        else
			gPokenavState->callbackStep += 1;
        break;
    case 7:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavState->callbackStep--;
        break;
    case 8:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavState->callbackStep++;
        break;
    case 9:
        InitPokenavListScreen(POKENAV_LIST_CONDITION_SEARCH);
        gPokenavState->callbackStep++;
        // fall through
    case 10:
        if (!LoadPokenavListScreenStep())
			gPokenavState->callbackStep++;
        break;
    case 11:
        CreatePokenavListArrowSprites(0);
        gPokenavState->callbackStep++;
        break;
    case 12:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavState->callbackStep++;
        break;
    case 13:
        BeginNormalPaletteFade(gPokenavState->paletteFadeMask, -1, 16, 0, RGB(0, 0, 0));
        SetVBlankCallback(&VBlankCB_Pokenav);
        gPokenavState->callbackStep++;
        break;
    case 14:
        SetPokenavDisplayForScreen(0x4);
        gPokenavState->callbackStep++;
        break;
    case 15:
        if (!gPaletteFade.active)
        {
			gPokenavState->loadState = 0;
			gPokenavState->callbackStep++;
		}
        break;
    case 16:
        BeginPokenavLeftHeaderLoad(0x1);
        gPokenavState->callbackStep++;
        // fall through
    case 17:
        if (!LoadPokenavLeftHeaderStep(0x1))
        {
			gPokenavState->loadState = 0;
			gPokenavState->callbackStep++;
		}
        break;
    case 18:
        BeginPokenavLeftHeaderLoad(gPokenavState->conditionSearchCursorPos + 7);
        gPokenavState->callbackStep++;
        // fall through
    case 19:
        if (!LoadPokenavLeftHeaderStep(gPokenavState->conditionSearchCursorPos + 7))
		{
			SetPokenavCallback(&HandleConditionSearchInput);
#if DEBUG
			if (gLinkOpen == TRUE)
				InitDebugHexPrinter((void *)(VRAM + 0x7DE0), 0, (void *)(VRAM + 0xF800), 4);
#endif
		}
        break;
    }
}

void HandleConditionSearchInput(void)
{
    switch (gPokenavState->callbackStep)
    {
    case 0:
        switch (HandlePokenavListDpadInput())
        {
        case 1:
            PlaySE(SE_SELECT);
            PrintPokenavListSelectionInfo();
            return;
        case 2:
            PlaySE(SE_SELECT);
            gPokenavState->callbackStep++;
            return;
        default:
            if (JOY_NEW(A_BUTTON))
            {
                PlaySE(SE_SELECT);
                SetPokenavCallback(&OpenConditionGraphFromSearchResults);
				return;
            }
            else if (JOY_NEW(B_BUTTON))
            {
                PlaySE(SE_SELECT);
                SetPokenavCallback(&ReturnToConditionSearchMenu);
				return;
            }
            break;
        }
        break;
    case 1:
        if (!UpdatePokenavListScroll())
        {
			PrintPokenavListSelectionInfo();
			RefreshPokenavListArrowVisibility();
			gPokenavState->callbackStep++;
		}
        break;
    case 2:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavState->callbackStep = 0;
        break;
    }
#if DEBUG
	if (gLinkOpen == TRUE)
		PrintDebugHex(gLink.recvQueue.count, 1, 8, 2, 4);
#endif
}

void OpenConditionSearchListFromGraph(void)
{
    switch (gPokenavState->callbackStep)
    {
    case 0:
        SetVBlankCallback(NULL);
        gPokenavState->callbackStep++;
        break;
    case 1:
        SetPokenavDisplayForTransition();
        RestorePokenavListSelection();
        gPokenavState->callbackStep++;
        break;
    case 2:
        DrawPokenavBottomToolbar(0);
        gPokenavState->callbackStep++;
        break;
    case 3:
        InitPokenavListScreen(POKENAV_LIST_CONDITION_SEARCH);
        gPokenavState->callbackStep++;
        // fall through
    case 4:
        if (!LoadPokenavListScreenStep())
			gPokenavState->callbackStep++;
        break;
    case 5:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavState->callbackStep++;
        break;
    case 6:
        CreatePokenavListArrowSprites(0);
        BeginNormalPaletteFade(gPokenavState->paletteFadeMask, -1, 16, 0, RGB(0, 0, 0));
        SetVBlankCallback(&VBlankCB_Pokenav);
        gPokenavState->callbackStep++;
        break;
    case 7:
        SetPokenavDisplayForScreen(0x4);
        gPokenavState->callbackStep++;
#if DEBUG
		if (gLinkOpen == TRUE)
			InitDebugHexPrinter((void *)(VRAM + 0x7DE0), 0, (void *)(VRAM + 0xF800), 4);
#endif
        break;
    case 8:
        if (!gPaletteFade.active)
			SetPokenavCallback(&HandleConditionSearchInput);
		break;
    }
}

void OpenConditionGraphFromSearchResults(void)
{
    switch (gPokenavState->callbackStep)
    {
    case 0:
        BeginNormalPaletteFade(gPokenavState->paletteFadeMask, -1, 0, 16, RGB(0, 0, 0));
        gPokenavState->callbackStep++;
        break;
    case 1:
        if (!gPaletteFade.active)
        {
			SetVBlankCallback(NULL);
			SetPokenavDisplayForTransition();
			DestroyPokenavListArrowSprites();
			gPokenavState->callbackStep++;
		}
        break;
    case 2:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavState->callbackStep++;
        break;
    case 3:
        PrepareConditionGraphFromSearchResult();
        gPokenavState->callbackStep++;
        break;
    case 4:
        InitConditionGraphScreen();
        gPokenavState->callbackStep++;
        // fall through
    case 5:
        if (!LoadConditionGraphScreenStep())
			gPokenavState->callbackStep++;
        break;
    case 6:
        CreateConditionMonMarkingsSprite();
        DrawPokenavBottomToolbar(0x2);
        BeginNormalPaletteFade(gPokenavState->paletteFadeMask, -1, 16, 0, RGB(0, 0, 0));
        SetVBlankCallback(VBlankCB_PokenavConditionGraph);
        gPokenavState->callbackStep++;
        break;
    case 7:
        SetPokenavDisplayForScreen(0x2);
        gPokenavState->callbackStep++;
#if DEBUG
		if (gLinkOpen == TRUE)
			InitDebugHexPrinter((void *)(VRAM + 0x7DE0), 0, (void *)(VRAM + 0xF000), 4);
#endif
        break;
    case 8:
        if (!gPaletteFade.active)
			SetPokenavCallback(&HandleConditionGraphInput);
        break;
    }
}

void OpenConditionGraphFromParty(void)
{
    switch (gPokenavState->callbackStep)
    {
    case 0:
        StartMenuOptionSpritesSlideOut();
        StartPokenavLeftHeaderSlideOut(0x1);
        gPokenavState->callbackStep++;
        break;
    case 1:
        if (!UpdateMenuOptionSpritesSlideOut())
        {
			DrawPokenavBottomToolbar(0x1);
			gPokenavState->callbackStep++;
		}
        break;
    case 2:
        if (!SlideMenuHeaderUp())
        {
            BeginNormalPaletteFade(gPokenavState->paletteFadeMask, -1, 0x0, 16, RGB(0, 0, 0));
            gPokenavState->callbackStep++;
        }
        break;
    case 3:
        if (!gPaletteFade.active)
        {
			SetVBlankCallback(NULL);
			SetPokenavDisplayForTransition();
			StopPokenavBg3Scroll();
			gPokenavState->isConditionGraphSearchMode = 0;
			gPokenavState->callbackStep++;
		}
        break;
    case 4:
        DestroyRematchBlueLightSprite();
        gPokenavState->callbackStep++;
        break;
    case 5:
        gPokenavState->unkD162 = 0x2;
        BuildPartyConditionGraphMonList();
        gPokenavState->callbackStep++;
        break;
    case 6:
        InitConditionGraphScreen();
        gPokenavState->callbackStep++;
        // fall through
    case 7:
        if (!LoadConditionGraphScreenStep())
			gPokenavState->callbackStep++;
        break;
    case 8:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavState->callbackStep++;
        break;
    case 9:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavState->callbackStep++;
        break;
    case 10:
        CreateConditionPartyPokeballIndicators();
        BeginNormalPaletteFade(gPokenavState->paletteFadeMask, -1, 16, 0, RGB(0, 0, 0));
        SetVBlankCallback(&VBlankCB_PokenavConditionGraph);
        gPokenavState->callbackStep++;
        break;
    case 11:
        SetPokenavDisplayForScreen(0x2);
        gPokenavState->callbackStep++;
        break;
    case 12:
        if (!gPaletteFade.active)
			gPokenavState->callbackStep++;
        break;
    case 13:
        BeginPokenavLeftHeaderLoad(0x1);
        gPokenavState->callbackStep++;
    case 14:
        if (!LoadPokenavLeftHeaderStep(0x1))
			gPokenavState->callbackStep++;
        break;
    case 15:
        BeginPokenavLeftHeaderLoad(0x6);
        gPokenavState->callbackStep++;
        // fall through
    case 16:
        if (!LoadPokenavLeftHeaderStep(0x6))
		{
			SetPokenavCallback(&HandleConditionGraphInput);
#if DEBUG
			if (gLinkOpen == TRUE)
				InitDebugHexPrinter((void *)(VRAM + 0x7DE0), 0, (void *)(VRAM + 0xF000), 4);
#endif
		}
        break;
    }
}

void StartConditionGraphMonEnter(void);
void SetConditionGraphBg2Visible(bool8 visible);
bool8 UpdateConditionGraphMonEnter(void);
void OpenConditionMonMarkingsMenu(void);
bool8 HandleMonMarkingsMenuInput(void);
void SaveAndCloseConditionMonMarkingsMenu(void);
bool8 UpdateConditionGraphMonExit(void);
void DestroyPokenavPortraitSprite(void);
void FreeConditionSparkles(void);
void DestroyConditionMonMarkingsSprite(void);
void DestroyConditionPartyPokeballIndicators(void);
void StartConditionGraphMonExit(void);

void HandleConditionGraphInput(void)
{
    switch (gPokenavState->callbackStep)
    {
    case 0:
        StartConditionGraphMonEnter();
        ConditionGraph_InitResetScanline();
        gPokenavState->callbackStep = 1;
        break;
    case 1:
        if (!ConditionGraph_ResetScanline())
            gPokenavState->callbackStep = 2;
        break;
    case 2:
        SetConditionGraphBg2Visible(1);
        gPokenavState->callbackStep = 3;
        break;
    case 3:
        if (!UpdateConditionGraphMonEnter())
        {
            LoadConditionSparkles();
            CreateConditionSparkleSprites();
            gPokenavState->callbackStep = 4;
        }
        break;
    case 4:
        if (JOY_HELD(DPAD_UP))
        {
            if (gPokenavState->canScrollConditionGraph && (
                gPokenavState->isConditionGraphSearchMode == 0
                || gPokenavState->detailSelectionIndex != 0
            ))
            {
                PlaySE(SE_SELECT);
                BeginConditionGraphMonScroll(1);
                DestroyConditionSparkleSprites();
                gPokenavState->callbackStep = 5;
                break;
            }
        }
        if (JOY_HELD(DPAD_DOWN))
        {
            if (gPokenavState->canScrollConditionGraph && (
                gPokenavState->isConditionGraphSearchMode == 0
                || gPokenavState->detailSelectionIndex < gPokenavState->lastListIndex
            ))
            {
                PlaySE(SE_SELECT);
                BeginConditionGraphMonScroll(0);
                DestroyConditionSparkleSprites();
                gPokenavState->callbackStep = 5;
                break;
            }
        }
        if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            StartConditionGraphMonExit();
            DestroyConditionSparkleSprites();
            gPokenavState->callbackStep = 9;
            break;
        }
        if (JOY_NEW(A_BUTTON))
        {
            if (gPokenavState->isConditionGraphSearchMode == 0)
            {
                if (gPokenavState->detailSelectionIndex == gPokenavState->conditionGraphMonCount - 1)
                {
                    PlaySE(SE_SELECT);
                    gPokenavState->callbackStep = 9;
                }
            }
            else
            {
                if (gPokenavState->isLinkActive == 0)
                {
                    PlaySE(SE_SELECT);
                    gPokenavState->callbackStep = 7;
                }
            }
            break;
        }
        break;
    case 5:
        if (!UpdateConditionGraphMonScroll())
        {
            CreateConditionSparkleSprites();
            gPokenavState->callbackStep = 6;
        }
        break;
    case 6:
        if (!Overworld_IsRecvQueueAtMax())
            gPokenavState->callbackStep = 4;
        break;
    case 7:
        DrawPokenavBottomToolbar(3);
        OpenConditionMonMarkingsMenu();
        gPokenavState->callbackStep = 8;
        break;
    case 8:
        if (!HandleMonMarkingsMenuInput())
        {
            DrawPokenavBottomToolbar(2);
            SaveAndCloseConditionMonMarkingsMenu();
            gPokenavState->callbackStep = 4;
        }
        break;
    case 9:
        if (!UpdateConditionGraphMonExit())
        {
            SetConditionGraphBg2Visible(0);
            DestroyPokenavPortraitSprite();
            BeginNormalPaletteFade(gPokenavState->paletteFadeMask, -1, 0, 16, RGB_BLACK);
            gPokenavState->callbackStep = 11;
        }
        break;
    case 10:
        break;
    case 11:
        if (!gPaletteFade.active)
        {
            FreeConditionSparkles();
            StopConditionGraphScanlineEffect();
            if (!gPokenavState->isConditionGraphSearchMode)
            {
                DestroyConditionPartyPokeballIndicators();
                DestroyPokenavLeftHeaderSprites(1);
                DestroyPokenavLeftHeaderSprites(6);
                SetPokenavCallback(RestorePokenavConditionMenu);
            }
            else
            {
                DestroyConditionMonMarkingsSprite();
                SetPokenavCallback(OpenConditionSearchListFromGraph);
            }
        }
        break;
    }

#if DEBUG
    if (gLinkOpen == TRUE)
        PrintDebugHex(gLink.recvQueue.count, 9, 6, 2, 4);
#endif
}

void OpenRibbonsMonList(void)
{
    switch (gPokenavState->callbackStep)
    {
    case 0:
        StartMenuOptionSpritesSlideOut();
        StartPokenavLeftHeaderSlideOut(0);
        gPokenavState->callbackStep++;
        break;
    case 1:
        if (!UpdateMenuOptionSpritesSlideOut())
        {
			DrawPokenavBottomToolbar(0x4);
			gPokenavState->callbackStep++;
		}
        break;
    case 2:
        if (!SlideMenuHeaderUp())
        {
            BeginNormalPaletteFade(gPokenavState->paletteFadeMask, -1, 0, 16, RGB(0, 0, 0));
            gPokenavState->callbackStep++;
        }
        break;
    case 3:
        if (!gPaletteFade.active)
        {
			SetVBlankCallback(NULL);
			SetPokenavDisplayForTransition();
			StopPokenavBg3Scroll();
			gPokenavState->callbackStep++;
		}
        break;
    case 4:
        DestroyRematchBlueLightSprite();
        gPokenavState->callbackStep++;
        break;
    case 5:
        InitRibbonsMonList();
        gPokenavState->callbackStep++;
    case 6:
        if (!BuildRibbonsMonListStep())
            gPokenavState->callbackStep += 2;
        else
			gPokenavState->callbackStep += 1;
        break;
    case 7:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavState->callbackStep--;
        break;
    case 8:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavState->callbackStep++;
        break;
    case 9:
        InitPokenavListScreen(POKENAV_LIST_RIBBONS);
        gPokenavState->callbackStep++;
    case 10:
        if (!LoadPokenavListScreenStep())
			gPokenavState->callbackStep++;
        break;
    case 11:
        BeginPokenavLeftHeaderLoad(0x2);
        gPokenavState->callbackStep++;
    case 12:
        if (!LoadPokenavLeftHeaderStep(0x2))
			gPokenavState->callbackStep++;
        break;
    case 13:
        CreatePokenavListArrowSprites(0x1);
        gPokenavState->callbackStep++;
        break;
    case 14:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavState->callbackStep++;
        break;
    case 15:
        BeginNormalPaletteFade(gPokenavState->paletteFadeMask, -1, 16, 0, RGB(0, 0, 0));
        SetVBlankCallback(&VBlankCB_Pokenav);
        gPokenavState->callbackStep++;
        break;
    case 16:
        SetPokenavDisplayForScreen(0x4);
        gPokenavState->callbackStep++;
#if DEBUG
		if (gLinkOpen == TRUE)
			InitDebugHexPrinter((void *)(VRAM + 0x7DE0), 0, (void *)(VRAM + 0xF800), 4);
#endif
        break;
    case 17:
        if (!gPaletteFade.active)
			SetPokenavCallback(&HandleRibbonsMonListInput);
        break;
    }
}

void HandleRibbonsMonListInput(void)
{
    switch (gPokenavState->callbackStep)
    {
    case 0:
        switch (HandlePokenavListDpadInput())
        {
        case 1:
            PlaySE(SE_SELECT);
            PrintPokenavListSelectionInfo();
            return;
        case 2:
            PlaySE(SE_SELECT);
            gPokenavState->callbackStep++;
            return;
        default:
            if (JOY_NEW(A_BUTTON))
            {
                PlaySE(SE_SELECT);
                SetPokenavCallback(&OpenRibbonsSummaryMenu);
				return;
            }
            else if (JOY_NEW(B_BUTTON))
            {
                PlaySE(SE_SELECT);
                SetPokenavCallback(&RibbonsMonMenu_ReturnToMainMenu);
				return;
            }
            break;
        }
        break;
    case 1:
        if (!UpdatePokenavListScroll())
        {
			PrintPokenavListSelectionInfo();
			RefreshPokenavListArrowVisibility();
			gPokenavState->callbackStep++;
		}
        break;
    case 2:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavState->callbackStep = 0;
        break;
    }
#if DEBUG
	if (gLinkOpen == TRUE)
		PrintDebugHex(gLink.recvQueue.count, 1, 8, 2, 4);
#endif
}

void RibbonsMonMenu_ReturnToMainMenu(void)
{
    switch (gPokenavState->callbackStep)
    {
    case 0:
        BeginNormalPaletteFade(gPokenavState->paletteFadeMask, -1, 0, 16, RGB(0, 0, 0));
        gPokenavState->callbackStep++;
        break;
    case 1:
        if (!gPaletteFade.active)
        {
			DestroyPokenavListArrowSprites();
			DestroyPokenavLeftHeaderSprites(0x2);
			gPokenavState->callbackStep++;
		}
        break;
    case 2:
        SetPokenavCallback(&RestorePokenavMainMenu);
        break;
    }

}

void OpenRibbonsSummaryMenu(void)
{
    switch (gPokenavState->callbackStep)
    {
    case 0:
        BeginNormalPaletteFade(gPokenavState->paletteFadeMask, -1, 0, 16, RGB(0, 0, 0));
        gPokenavState->callbackStep++;
        break;
    case 1:
        if (!gPaletteFade.active)
        {
			SetVBlankCallback(NULL);
			SetPokenavDisplayForTransition();
			DestroyPokenavListArrowSprites();
			gPokenavState->callbackStep++;
		}
        break;
    case 2:
        InitRibbonsSummaryMonData();
        ResetPokenavSetupStep();
        gPokenavState->callbackStep++;
        break;
    case 3:
        if (!LoadRibbonsSummaryScreenStep())
			gPokenavState->callbackStep++;
        break;
    case 4:
        DrawPokenavBottomToolbar(0x5);
        InitRibbonsSummarySpriteGfx();
        gPokenavState->callbackStep++;
    case 5:
        if (!LoadRibbonsSummarySpriteGfxStep())
			gPokenavState->callbackStep++;
        break;
    case 6:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavState->callbackStep++;
        break;
    case 7:
        BeginNormalPaletteFade(gPokenavState->paletteFadeMask, -1, 16, 0, RGB(0, 0, 0));
        SetVBlankCallback(&VBlankCB_Pokenav);
        gPokenavState->callbackStep++;
        break;
    case 8:
        SetPokenavDisplayForScreen(0x3);
        gPokenavState->callbackStep++;
#if DEBUG
		if (gLinkOpen == TRUE)
			InitDebugHexPrinter((void *)(VRAM + 0x7DE0), 0, (void *)(VRAM + 0xF000), 4);
#endif
        break;
    case 9:
        if (!gPaletteFade.active)
			gPokenavState->callbackStep++;
        break;
    case 10:
        if (!SlidePokenavMonInfoHeaderIn())
			SetPokenavCallback(&RibbonsSummaryHandleInput);
        break;
    }
}

void RibbonsSummaryHandleInput(void)
{
    switch (gPokenavState->callbackStep)
    {
    case 0:
        PrintRibbonsSummaryCount();
        gPokenavState->callbackStep++;
        break;
    case 1:
        if (JOY_REPT(0x40) && (gPokenavState->detailSelectionIndex))
        {
			PlaySE(SE_SELECT);
            BeginRibbonsSummaryMonSwitch(-1);
			gPokenavState->callbackStep++;
        }
		else if (JOY_REPT(0x80) && gPokenavState->detailSelectionIndex < gPokenavState->lastListIndex)
		{
            PlaySE(SE_SELECT);
            BeginRibbonsSummaryMonSwitch(1);
            gPokenavState->callbackStep++;
        }
        else if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            DrawPokenavBottomToolbar(0xB);
            gPokenavState->callbackStep = 0x4;
        }
        else if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            SetPokenavCallback(&OpenRibbonsMonListFromRibbonsSummary);
        }
        break;
    case 2:
        if (!UpdateRibbonsSummaryMonSwitch())
			gPokenavState->callbackStep++;
        break;
    case 3:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavState->callbackStep = 0;
        break;
    case 4:
        CreateAndZoomInSelectedRibbon();
        gPokenavState->callbackStep++;
        break;
    case 5:
        if (!IsRibbonZoomInAnimating())
		{
			PrintRibbonsSummaryDescription();
			gPokenavState->callbackStep++;
			PausePokenavCallbackForLink();
		}
        break;
    case 6:
        switch (HandleRibbonsSummaryCursorInput())
        {
        case 1:
            PlaySE(SE_SELECT);
            ZoomOutSelectedRibbon();
            gPokenavState->callbackStep = 0x7;
            return;
        default:
        case 0:
            if (JOY_NEW(B_BUTTON))
            {
                PlaySE(SE_SELECT);
                ZoomOutSelectedRibbon();
                gPokenavState->callbackStep = 0x8;
            }
            break;
        }
        break;
    case 7:
        if (!UpdateRibbonZoomOutAndDestroySprite())
			gPokenavState->callbackStep = 0x4;
        break;
    case 8:
        if (!UpdateRibbonZoomOutAndDestroySprite())
		{
			DrawPokenavBottomToolbar(0x5);
			gPokenavState->callbackStep = 0;
		}
        break;
    }
#if DEBUG
	if (gLinkOpen == TRUE)
		PrintDebugHex(gLink.recvQueue.count, 1, 4, 2, 4);
#endif
}

void OpenRibbonsMonListFromRibbonsSummary(void)
{
    switch (gPokenavState->callbackStep)
    {
    case 0:
        BeginNormalPaletteFade(gPokenavState->paletteFadeMask, -1, 0, 16, RGB(0, 0, 0));
        gPokenavState->callbackStep++;
        break;
    case 1:
        if (!gPaletteFade.active)
        {
			SetVBlankCallback(NULL);
			SetPokenavDisplayForTransition();
			FreeRibbonsSummarySprites();
			ResetPokenavSetupStep();
			gPokenavState->callbackStep++;
		}
        break;
    case 2:
        RestorePokenavListSelection();
        InitPokenavListScreen(POKENAV_LIST_RIBBONS);
        gPokenavState->callbackStep++;
    case 3:
        if (!LoadPokenavListScreenStep())
			gPokenavState->callbackStep++;
        break;
    case 4:
        DrawPokenavBottomToolbar(0x4);
        CreatePokenavListArrowSprites(0x1);
        SetVBlankCallback(&VBlankCB_Pokenav);
        gPokenavState->callbackStep++;
        break;
    case 5:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavState->callbackStep++;
        break;
    case 6:
        BeginNormalPaletteFade(gPokenavState->paletteFadeMask, -1, 16, 0, RGB(0, 0, 0));
        gPokenavState->callbackStep++;
        break;
    case 7:
        SetPokenavDisplayForScreen(0x4);
        gPokenavState->callbackStep++;
#if DEBUG
		if (gLinkOpen == TRUE)
			InitDebugHexPrinter((void *)(VRAM + 0x7DE0), 0, (void *)(VRAM + 0xF800), 4);
#endif
        break;
    case 8:
        if (!gPaletteFade.active)
			SetPokenavCallback(&HandleRibbonsMonListInput);
        break;
    }
}

void OpenTrainerEyes(void)
{
    switch (gPokenavState->callbackStep)
    {
    case 0:
        StartMenuOptionSpritesSlideOut();
        StartPokenavLeftHeaderSlideOut(0);
        gPokenavState->callbackStep++;
        break;
    case 1:
        if (!UpdateMenuOptionSpritesSlideOut())
        {
			DrawPokenavBottomToolbar(0x9);
			gPokenavState->callbackStep++;
		}
        break;
    case 2:
        if (!SlideMenuHeaderUp())
        {
            BeginNormalPaletteFade(gPokenavState->paletteFadeMask, -1, 0, 16, RGB(0, 0, 0));
            gPokenavState->callbackStep++;
        }
        break;
    case 3:
        if (!gPaletteFade.active)
        {
			SetVBlankCallback(NULL);
			SetPokenavDisplayForTransition();
			StopPokenavBg3Scroll();
			ResetPokenavSetupStep();
			gPokenavState->callbackStep++;
		}
        break;
    case 4:
        DestroyRematchBlueLightSprite();
        gPokenavState->callbackStep++;
        break;
    case 5:
        InitPokenavListScreen(POKENAV_LIST_TRAINERS_EYES);
        gPokenavState->callbackStep++;
    case 6:
        if (!LoadPokenavListScreenStep())
			gPokenavState->callbackStep++;
        break;
    case 7:
        BeginPokenavLeftHeaderLoad(0x3);
        gPokenavState->callbackStep++;
    case 8:
        if (!LoadPokenavLeftHeaderStep(0x3))
			gPokenavState->callbackStep++;
        break;
    case 9:
        CreatePokenavListArrowSprites(0x2);
        gPokenavState->callbackStep++;
        break;
    case 10:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavState->callbackStep++;
        break;
    case 11:
        InitTrainerEyesBackgroundPulse();
        BeginNormalPaletteFade(gPokenavState->paletteFadeMask, -1, 16, 0, RGB(0, 0, 0));
        SetVBlankCallback(&VBlankCB_PokenavTrainerEyes);
        gPokenavState->callbackStep++;
        break;
    case 12:
        SetPokenavDisplayForScreen(0x5);
        gPokenavState->callbackStep++;
#if DEBUG
		if (gLinkOpen == TRUE)
			InitDebugHexPrinter((void *)(VRAM + 0x7DE0), 0, (void *)(VRAM + 0xF800), 4);
#endif
        break;
    case 13:
        if (!gPaletteFade.active)
			SetPokenavCallback(&HandleTrainerEyesInput);
        break;
    }
}

void HandleTrainerEyesInput(void)
{
    switch (gPokenavState->callbackStep)
    {
    case 0:
        switch (HandlePokenavListDpadInput())
        {
        case 1:
            PlaySE(SE_SELECT);
            PrintTrainerEyesLocation(gPokenavState->listSelectedIndex);
            return;
        case 2:
            PlaySE(SE_SELECT);
            gPokenavState->callbackStep++;
            return;
        default:
            if (JOY_NEW(A_BUTTON))
            {
                PlaySE(SE_SELECT);
                SetPokenavCallback(&ShowTrainerEyesTrainerInfo);
				return;
            }
            else if (JOY_NEW(B_BUTTON))
            {
                PlaySE(SE_SELECT);
                SetPokenavCallback(&TrainerEyes_ReturnToMainMenu);
				return;
            }
            break;
        }
        break;
    case 1:
        if (!UpdatePokenavListScroll())
        {
			PrintTrainerEyesLocation(gPokenavState->listSelectedIndex);
			RefreshPokenavListArrowVisibility();
			gPokenavState->callbackStep++;
		}
        break;
    case 2:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavState->callbackStep = 0;
        break;
    }
#if DEBUG
	if (gLinkOpen == TRUE)
		PrintDebugHex(gLink.recvQueue.count, 1, 3, 2, 4);
#endif
}

void ShowTrainerEyesTrainerInfo(void)
{
    switch (gPokenavState->callbackStep)
    {
    case 0:
        TogglePokenavListArrows(0x1);
        DrawPokenavBottomToolbar(0xA);
        InitTrainerEyesListErase();
        gPokenavState->callbackStep++;
        break;
    case 1:
        if (!EraseTrainerEyesListStep())
			gPokenavState->callbackStep++;
        break;
    case 2:
        BeginTrainerEyesDetailEnterScroll();
        gPokenavState->callbackStep++;
        break;
    case 3:
        if (!UpdateTrainerEyesDetailScroll())
        {
			SetTrainerEyesDetailWindow(0x1);
			gPokenavState->callbackStep++;
		}
        break;
    case 4:
        PrepareTrainerEyesDetailPortrait();
        gPokenavState->callbackStep++;
        break;
    case 5:
        BufferTrainerEyesListRowText((u8 *)(gSharedMem + 0x8788), *(u16 *)(gSharedMem + 0x8788 - 0x1A));
        CreateTrainerEyesNameSprites((u8 *)(gSharedMem + 0x8788));
        gPokenavState->callbackStep++;
        break;
    case 6:
        LoadTrainerEyesDescriptionLines();
        gPokenavState->callbackStep++;
    case 7:
        if (!SlideTrainerEyesPortraitIn() && !PrintTrainerEyesDescriptionStep())
			gPokenavState->callbackStep++;
        break;
    case 8:
        if (JOY_HELD(0x40) && gPokenavState->detailSelectionIndex)
        {
			PlaySE(SE_SELECT);
			ChangeTrainerEyesDetailSelection(-1);
			gPokenavState->callbackStep = 0x10;
        }
        else if (JOY_HELD(0x80) && gPokenavState->detailSelectionIndex < gPokenavState->lastListIndex)
        {
			PlaySE(SE_SELECT);
			ChangeTrainerEyesDetailSelection(1);
			gPokenavState->callbackStep = 0x10;
        }
		else if (JOY_NEW(B_BUTTON))
		{
            PlaySE(SE_SELECT);
            DestroyTrainerEyesNameSprites();
            InitTrainerEyesDescriptionErase();
            gPokenavState->callbackStep++;
        }
        break;
    case 9:
        if (!SlideTrainerEyesPortraitOut() && !EraseTrainerEyesDescriptionStep())
        {
			SetTrainerEyesDetailWindow(0);
			DestroyTrainerEyesPortrait();
			gPokenavState->callbackStep++;
		}
        break;
    case 10:
        RestorePokenavListSelection();
        gPokenavState->callbackStep++;
        break;
    case 11:
        BeginTrainerEyesDetailExitScroll();
        gPokenavState->callbackStep++;
        break;
    case 12:
        if (!UpdateTrainerEyesDetailScroll())
			gPokenavState->callbackStep++;
        break;
    case 13:
        InitTrainerEyesListRestore();
        gPokenavState->callbackStep++;
        break;
    case 14:
        if (!RestoreTrainerEyesListStep())
			gPokenavState->callbackStep++;
        break;
    case 15:
        DrawPokenavBottomToolbar(0x9);
        TogglePokenavListArrows(0);
        SetPokenavCallback(&HandleTrainerEyesInput);
        PausePokenavCallbackForLink();
        break;
    case 16:
        if (!UpdateTrainerEyesDetailSelection())
			gPokenavState->callbackStep++;
        break;
    case 17:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavState->callbackStep = 0x8;
        break;
    }
}

void TrainerEyes_ReturnToMainMenu(void)
{
    switch (gPokenavState->callbackStep)
    {
    case 0:
        BeginNormalPaletteFade(gPokenavState->paletteFadeMask, -1, 0, 16, RGB(0, 0, 0));
        gPokenavState->callbackStep++;
        break;
    case 1:
        if (!gPaletteFade.active)
        {
			DestroyPokenavListArrowSprites();
			DestroyPokenavLeftHeaderSprites(0x2);
			StopTrainerEyesBackgroundPulse();
			gPokenavState->callbackStep++;
		}
        break;
    case 2:
        SetPokenavCallback(&RestorePokenavMainMenu);
        break;
    }
}

void ExitPokenav(void)
{
    if (!gPokenavState->callbackStep)
    {
        PlaySE(0x6F);
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        gPokenavState->callbackStep++;
    }
    else
    {
        if (!gPaletteFade.active)
        {
			StopConditionGraphScanlineEffect();
			CB2_ReturnToFieldWithOpenMenu();
		}
    }
}

void StartPokenavMenuTransition(u8 menuType, u8 cursorPos, u8 gfxId)
{
    StartMenuOptionSpritesSlideOut();
    gPokenavState->transitionMenuType = menuType;
    gPokenavState->transitionCursorPos = cursorPos;
    gPokenavState->transitionStep = 0;
    gPokenavState->transitionGfxId = gfxId;
}

bool8 UpdatePokenavPaletteTransition(void);
void StartPokenavPaletteTransition(u8 menuType);

bool8 UpdatePokenavMenuTransition(void)
{
    switch (gPokenavState->transitionStep)
    {
    case 0:
        if (!UpdateMenuOptionSpritesSlideOut())
        {
            if (gPokenavState->transitionGfxId != 12)
                gPokenavState->transitionStep = 1;
            else
                gPokenavState->transitionStep = 3;
        }
        break;
    case 1:
        BeginPokenavLeftHeaderLoad(gPokenavState->transitionGfxId);
        gPokenavState->transitionStep++;
        // fallthrough
    case 2:
        if (!LoadPokenavLeftHeaderStep(gPokenavState->transitionGfxId))
            gPokenavState->transitionStep++;
        break;
    case 3:
        gPokenavState->menuCursorPos = gPokenavState->transitionCursorPos;
        switch (gPokenavState->transitionMenuType)
        {
        case 0:
            gPokenavState->menuItemCount = 5;
            break;
        case 1:
            gPokenavState->menuItemCount = 3;
            break;
        case 2:
            gPokenavState->menuItemCount = 6;
            break;
        }
        InitPokenavMenuOptionSprites(gPokenavState->transitionMenuType);
        gPokenavState->transitionStep++;
        // fallthrough
    case 4:
        if (!LoadPokenavMenuOptionSpritesStep(gPokenavState->transitionMenuType))
            gPokenavState->transitionStep++;
        break;
    case 5:
        if (!Overworld_IsRecvQueueAtMax())
            gPokenavState->transitionStep++;
        break;
    case 6:
        StartMenuOptionSpritesSlideIn();
        StartPokenavPaletteTransition(gPokenavState->transitionMenuType);
        gPokenavState->transitionStep++;
        break;
    case 7:
        if (!UpdateMenuOptionEntryHighlight())
        {
            PrintPokenavMenuDescription(gPokenavState->transitionMenuType, gPokenavState->menuCursorPos);
            gPokenavState->transitionStep++;
        }
        break;
    case 8:
        if (!UpdatePokenavPaletteTransition())
        {
            gPokenavState->transitionStep++;
            return FALSE;
        }
        break;
    default:
        return FALSE;
    }

    return TRUE;
}

// Keep menuCursorPos and menuItemCount signed for upward wraparound.
bool8 UpdateMainMenuCursorPos(void)
{
    if (JOY_NEW(DPAD_UP))
    {
        do
        {
            if (--gPokenavState->menuCursorPos < 0)
                gPokenavState->menuCursorPos = gPokenavState->menuItemCount - 1;
        } while (!gPokenavState->mainMenuItemIds[gPokenavState->menuCursorPos]);
        return TRUE;
    }
    if (JOY_NEW(DPAD_DOWN))
    {
        do
        {
            if (++gPokenavState->menuCursorPos >= gPokenavState->menuItemCount)
                gPokenavState->menuCursorPos = 0;
        } while (!gPokenavState->mainMenuItemIds[gPokenavState->menuCursorPos]);
        return TRUE;
    }
#if DEBUG
	if (gLinkOpen == TRUE)
		PrintDebugHex(gLink.recvQueue.count, 6, 10, 2, 4);
#endif
    return FALSE;
}

bool8 UpdateSubmenuCursorPos(void)
{
    if (JOY_NEW(DPAD_UP))
    {
        if (--gPokenavState->menuCursorPos < 0)
            gPokenavState->menuCursorPos = gPokenavState->menuItemCount - 1;
        return TRUE;
    }
    if (JOY_NEW(DPAD_DOWN))
    {
        if (++gPokenavState->menuCursorPos >= gPokenavState->menuItemCount)
            gPokenavState->menuCursorPos = 0;
        return TRUE;
    }
#if DEBUG
	if (gLinkOpen == TRUE)
		PrintDebugHex(gLink.recvQueue.count, 6, 10, 2, 4);
#endif
    return FALSE;
}

void SetPokenavDisplayForTransition(void)
{
    REG_DISPCNT = 512;
}

void SetPokenavDisplayForMenu(void)
{
    REG_DISPCNT = 0x92 << 5;
}

void SetPokenavDisplayForScreen(u8 screenId)
{
    switch (screenId) {
    case 0:
        REG_DISPCNT = 0xFA << 5;
        break;
    case 1:
        REG_DISPCNT = 0x1741;
        break;
    case 3:
    case 4:
        REG_DISPCNT = 0xFA << 5;
        break;
    case 2:
        REG_DISPCNT = 0x7b40;
        break;
    case 5:
        REG_DISPCNT = 0xFD << 6;
        break;
    }
}

void PausePokenavCallbackForLink(void)
{
    sPokenavCallbackBeforeLinkWait = gPokenavState->callback;
    gPokenavState->callback = &WaitForPokenavLinkQueue;
    gPokenavState->callback();
}

void WaitForPokenavLinkQueue(void)
{
    if (Overworld_IsRecvQueueAtMax() != 0x1)
        gPokenavState->callback = sPokenavCallbackBeforeLinkWait;
}
