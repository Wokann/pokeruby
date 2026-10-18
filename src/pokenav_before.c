#include "global.h"
#include "constants/rgb.h"
#include "main.h"
#include "pokenav.h"
#include "ewram.h"
#include "battle.h"
#include "data.h"
#include "de_rom_8040FE0.h"
#include "decompress.h"
#include "event_data.h"
#include "graphics.h"
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

static void (*gUnknown_03000744)(void);

struct UnkPokenavStruct *const gPokenavStructPtr = (struct UnkPokenavStruct *)gSharedMem;

// rodata?
const u8 *const gUnknown_083DFEC8 = gTileBuffer;

const u8 gPokenavBgDotsPal[] = INCBIN_U8("graphics/pokenav/bg_dots.gbapal");

const u8 gPokenavBgDotsTiles[] = INCBIN_U8("graphics/pokenav/bg_dots.4bpp");
const u8 gPokenavBgDotsTilemap[] = INCBIN_U8("graphics/pokenav/bg_dots.bin.lz");
const u16 gUnknown_083E003C[] = INCBIN_U16("graphics/pokenav/83E003C.gbapal");

const u8 gUnknown_083E005C[] = INCBIN_U8("graphics/pokenav/83E005C.4bpp");
const u8 gUnknown_083E007C[] = INCBIN_U8("graphics/pokenav/83E007C.bin.lz");
const u16 gUnknown_083E0124[] = INCBIN_U16("graphics/pokenav/83E0124_pal.bin");

const u16 gUnknown_083E0144[] = INCBIN_U16("graphics/pokenav/83E0144_pal.bin");

const u16 gUnknown_083E0164[] = INCBIN_U16("graphics/pokenav/83E0164.bin");//?

const u8 gUnknown_083E01AC[] = INCBIN_U8("graphics/pokenav/83E01AC.bin");
const u8 gUnknown_083E01F4[] = INCBIN_U8("graphics/pokenav/83E01F4.bin");
const u8 gConditionGraphData_Pal[] = INCBIN_U8("graphics/pokenav/condition/graph_data.gbapal");
const u16 gUnknown_083E0274[] = INCBIN_U16("graphics/pokenav/83E0274.gbapal");

const u16 gPokenavConditionSearch2_Pal[] = INCBIN_U16("graphics/pokenav/condition_search2.gbapal");

const u16 gUnknown_083E02B4[] = INCBIN_U16("graphics/pokenav/83E02B4.gbapal");

const u16 gUnknown_083E02D4[] =  INCBIN_U16("graphics/pokenav/83E02D4.bin");//?

const u8 gUnknown_083E0314[] = INCBIN_U8("graphics/pokenav/83E0314.gbapal");
const u8 gUnknown_083E0334[] = INCBIN_U8("graphics/pokenav/trainereyes_misc_pal.bin");

const u8 gUnknown_083E0354[] = INCBIN_U8("graphics/pokenav/trainereyes_misc.4bpp.lz");

const u8 gUnknown_083E039C[][4] = 
{
    {0x80, 0x42, 0x82, 0x42},
};

const u8 gUnknown_083E03A0[][8] = 
{
    {0x00, 0x40, 0x00, 0x40, 0x81, 0x42, 0x83, 0x42},
};

const u16 gUnknown_083E03A8[] = INCBIN_U16("graphics/pokenav/83E03A8.gbapal");
const u16 gUnknown_083E03C8[] = INCBIN_U16("graphics/pokenav/83E03C8.bin");
const u8 gRibbonIconsSmall_Gfx[] = INCBIN_U8("graphics/pokenav/ribbons/icons.4bpp.lz");
const u8 gPokenavDeviceBgPal[] = INCBIN_U8("graphics/pokenav/device_outline.gbapal");
const u8 gPokenavDeviceBgTiles[] = INCBIN_U8("graphics/pokenav/device_outline.4bpp.lz");
const u8 gPokenavDeviceBgTilemap[] = INCBIN_U8("graphics/pokenav/device_outline_map.bin.lz");

#include "data/region_map/city_map_tilemaps.h"

#ifdef ENGLISH
#include "data/text/ribbon_descriptions.h"
#include "data/text/gift_ribbon_descriptions.h"
#include "data/text/trainer_eye_descriptions_en.h"
#elif GERMAN
#include "data/text/ribbon_descriptions_de.h"
#include "data/text/gift_ribbon_descriptions_de.h"
#include "data/text/trainer_eye_descriptions_de.h"
#endif

const u8 *const gUnknown_083E31B0[] = 
{
    gText_CheckMapOfHoenn,
    gText_CheckPokemonInDetail,
    PCText_CheckTrainer,
    gText_CheckObtainedRibbons,
    gText_PutAwayPokenav,
    gText_NoRibbonWinners,
    gText_NoTrainersRegistered,
};

const u8 *const gUnknown_083E31CC[] = 
{
    gText_CheckPartyPokemonInDetail,
    gText_CheckAllPokemonInDetail,
    gText_ReturnToPokenavMenu,
};

const u8 *const gUnknown_083E31D8[] = 
{
    gText_FindCoolPokemon,
    gText_FindBeautifulPokemon,
    gText_FindCutePokemon,
    gText_FindSmartPokemon,
    gText_FindToughPokemon,
    gText_ReturnToConditionMenu,
};

#include "data/region_map/city_map_entries.h"

void sub_80F0A24(u16, u16);
void sub_80F0A74(u16, u16);

void (*const gUnknown_083E3270[])(u16, u16) = 
{
    sub_80F0A24,
    sub_80F0A24,
    sub_80F0A74,
};

static const u16 sSpinningPokenav_Pal[] = INCBIN_U16("graphics/pokenav/nav_icon.gbapal");
const u8 gSpinningPokenav_Gfx[] = INCBIN_U8("graphics/pokenav/nav_icon.4bpp.lz");
static const u8 sConditionPokeball_Gfx[] = INCBIN_U8("graphics/pokenav/condition/pokeball.4bpp");
static const u8 sConditionPokeballPlaceholder_Gfx[] = INCBIN_U8("graphics/pokenav/condition/pokeball_placeholder.4bpp");
static const u16 sPokenavSparkle_Pal[] = INCBIN_U16("graphics/pokenav/condition/sparkle.gbapal");
static const u8 sPokenavSparkle_Gfx[] = INCBIN_U8("graphics/pokenav/condition/sparkle.4bpp");
static const u8 sListArrow_Gfx[] = INCBIN_U8("graphics/pokenav/list_arrows.4bpp");
const u8 gUnknown_083E3BC0[] =  INCBIN_U8("graphics/pokenav/83E3BC0.bin");
const u8 gTiles_083E3BE0[] = INCBIN_U8("graphics/unknown/unknown_3E3BE0.4bpp");

const u16 gUnknown_083E3C60[][16] = 
{
    INCBIN_U16("graphics/pokenav/ribbons/icons1.gbapal"),
    INCBIN_U16("graphics/pokenav/ribbons/icons2.gbapal"),
    INCBIN_U16("graphics/pokenav/ribbons/icons3.gbapal"),
    INCBIN_U16("graphics/pokenav/ribbons/icons4.gbapal"),
    INCBIN_U16("graphics/pokenav/ribbons/icons5.gbapal"),
};

const u8 gRibbonIconsBig_Gfx[] = INCBIN_U8("graphics/pokenav/ribbons/icons_big.4bpp.lz");
const u16 PokenavBlueLightPalette[] = INCBIN_U16("graphics/pokenav/blue_light.gbapal");
const u8 PokenavBlueLightTiles[] = INCBIN_U8("graphics/pokenav/blue_light.4bpp");
const u16 Palette_3E42D8[] = INCBIN_U16("graphics/pokenav/83E42D8.gbapal");

const u16 gUnknown_083E42F8[] = 
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

const struct OamData gOamData_83E4318 = 
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

static const union AnimCmd sSpriteAnim_83E4320[] = 
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_83E4328[] = 
{
    ANIMCMD_FRAME(8, 5),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_83E4330[] = 
{
    ANIMCMD_FRAME(16, 5),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_83E4338[] = 
{
    ANIMCMD_FRAME(24, 5),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_83E4340[] = 
{
    ANIMCMD_FRAME(32, 5),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_83E4348[] = 
{
    ANIMCMD_FRAME(40, 5),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_83E4350[] = 
{
    ANIMCMD_FRAME(48, 5),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_83E4358[] = 
{
    ANIMCMD_FRAME(56, 5),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_83E4360[] = 
{
    ANIMCMD_FRAME(64, 5),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_83E4368[] = 
{
    ANIMCMD_FRAME(72, 5),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_83E4370[] = 
{
    ANIMCMD_FRAME(80, 5),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_83E4378[] = 
{
    ANIMCMD_FRAME(88, 5),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_83E4380[] = 
{
    ANIMCMD_FRAME(96, 5),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_83E4388[] = 
{
    ANIMCMD_FRAME(104, 5),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_83E4390[] = 
{
    ANIMCMD_FRAME(112, 5),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_83E4398[] = 
{
    ANIMCMD_FRAME(120, 5),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_83E43A0[] = 
{
    ANIMCMD_FRAME(128, 5),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_83E43A8[] = 
{
    ANIMCMD_FRAME(136, 5),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_83E43B0[] = 
{
    ANIMCMD_FRAME(144, 5),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_83E43B8[] = 
{
    ANIMCMD_FRAME(152, 5),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_83E43C0[] = 
{
    ANIMCMD_FRAME(160, 5),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_83E43C8[] = 
{
    ANIMCMD_FRAME(168, 5),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_83E43D0[] = 
{
    ANIMCMD_FRAME(176, 5),
    ANIMCMD_END,
};

static const union AnimCmd sSpriteAnim_83E43D8[] = 
{
    ANIMCMD_FRAME(184, 5),
    ANIMCMD_END,
};

const union AnimCmd *const gSpriteAnimTable_83E43E0[] = 
{
    sSpriteAnim_83E4320,
    sSpriteAnim_83E4328,
    sSpriteAnim_83E4330,
    sSpriteAnim_83E4338,
    sSpriteAnim_83E4340,
    sSpriteAnim_83E4348,
    sSpriteAnim_83E4350,
    sSpriteAnim_83E4358,
    sSpriteAnim_83E4360,
    sSpriteAnim_83E4368,
    sSpriteAnim_83E4370,
    sSpriteAnim_83E4378,
    sSpriteAnim_83E4380,
    sSpriteAnim_83E4388,
    sSpriteAnim_83E4390,
    sSpriteAnim_83E4398,
    sSpriteAnim_83E43A0,
    sSpriteAnim_83E43A8,
    sSpriteAnim_83E43B0,
    sSpriteAnim_83E43B8,
    sSpriteAnim_83E43C0,
    sSpriteAnim_83E43C8,
    sSpriteAnim_83E43D0,
    sSpriteAnim_83E43D8,
};

const union AffineAnimCmd gSpriteAffineAnim_83E4440[] = 
{
    AFFINEANIMCMD_FRAME(0x10, 0x10, 0, 16),
    AFFINEANIMCMD_END,
};

const union AffineAnimCmd *const gSpriteAffineAnimTable_83E4450[] = 
{
    gSpriteAffineAnim_83E4440,
};

const struct SpriteTemplate gSpriteTemplate_83E4454 = 
{
    .tileTag = 0,
    .paletteTag = 0,
    .oam = &gOamData_83E4318,
    .anims = gSpriteAnimTable_83E43E0,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

// sizeof(PokenavBlueLightTiles) ?
const struct SpriteSheet gSpriteSheet_PokenavBlueLight = {PokenavBlueLightTiles, 0x100, 25};

const struct SpritePalette gSpritePalette_PokenavBlueLight = {PokenavBlueLightPalette, 17};

const struct OamData gOamData_83E447C = 
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

const struct SpriteTemplate gSpriteTemplate_83E4484 = 
{
    .tileTag = 25,
    .paletteTag = 17,
    .oam = &gOamData_83E447C,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_BlinkingBlueLight,
};

extern const u16 gPokenavMenuOptions3_Pal[];
extern const u16 gPokenavCondition5_Pal[];

const struct SpritePalette gUnknown_083E449C[] = 
{
    {gPokenavMenuOptions3_Pal, 2},
    {gPokenavCondition5_Pal, 3},
    {0x0, 0},
};

static const struct OamData sOamData_LeftHeader =
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

const union AnimCmd gSpriteAnim_83E44BC[] = 
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END,
};

const union AnimCmd gSpriteAnim_83E44C4[] = 
{
    ANIMCMD_FRAME(32, 5),
    ANIMCMD_END,
};

const union AnimCmd gSpriteAnim_83E44CC[] = 
{
    ANIMCMD_FRAME(64, 5),
    ANIMCMD_END,
};

const union AnimCmd *const gSpriteAnimTable_83E44D4[] = 
{
    gSpriteAnim_83E44BC,
    gSpriteAnim_83E44C4,
    gSpriteAnim_83E44CC,
};

void sub_80F2BBC(struct Sprite *);

const struct SpriteTemplate gSpriteTemplate_83E44E0 = 
{
    .tileTag = 1,
    .paletteTag = 2,
    .oam = &sOamData_LeftHeader,
    .anims = gSpriteAnimTable_83E44D4,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80F2BBC,
};

const struct SpriteTemplate gSpriteTemplate_83E44F8 = 
{
    .tileTag = 1,
    .paletteTag = 3,
    .oam = &sOamData_LeftHeader,
    .anims = gSpriteAnimTable_83E44D4,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80F2BBC,
};

static const struct OamData sOamData_SubmenuLeftHeader =
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

const union AnimCmd gSpriteAnim_83E4518[] = 
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END,
};

const union AnimCmd gSpriteAnim_83E4520[] = 
{
    ANIMCMD_FRAME(8, 5),
    ANIMCMD_END,
};

const union AnimCmd *const gSpriteAnimTable_83E4528[] = 
{
    gSpriteAnim_83E4518,
    gSpriteAnim_83E4520,
};

const struct SpriteTemplate gSpriteTemplate_83E4530 = 
{
    .tileTag = 2,
    .paletteTag = 2,
    .oam = &sOamData_SubmenuLeftHeader,
    .anims = gSpriteAnimTable_83E4528,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80F2BBC,
};

const struct SpriteTemplate gSpriteTemplate_83E4548 = 
{
    .tileTag = 2,
    .paletteTag = 3,
    .oam = &sOamData_SubmenuLeftHeader,
    .anims = gSpriteAnimTable_83E4528,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80F2BBC,
};

static const struct OamData sOam_ConditionMonPic =
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

const struct SpriteSheet gUnknown_083E4568 = {NULL, 0x800, 6};

void MonPicGfxSpriteCallback(struct Sprite *);

const struct SpriteTemplate gSpriteTemplate_83E4570 = 
{
    .tileTag = 6,
    .paletteTag = 6,
    .oam = &sOam_ConditionMonPic,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = MonPicGfxSpriteCallback,
};

const struct SpritePalette gUnknown_083E4588 = {NULL, 6};
extern const u8 gPokenavArrow_Gfx[];

const struct SpriteSheet gUnknown_083E4590[] = 
{
    {gPokenavArrow_Gfx, 0x40, 9},
    {sListArrow_Gfx, 0x80, 10},
    {},
};

extern const u16 gPokenavArrowsPalette[];

const struct SpritePalette gUnknown_083E45A8 = {gPokenavArrowsPalette, 9};

const struct OamData gOamData_83E45B0 = 
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

void SpriteCB_RightArrow(struct Sprite *);

const struct SpriteTemplate gSpriteTemplate_83E45B8 = 
{
    .tileTag = 9,
    .paletteTag = 9,
    .oam = &gOamData_83E45B0,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_RightArrow,
};

const union AnimCmd gSpriteAnim_83E45D0[] = 
{
    ANIMCMD_FRAME(0, 3),
    ANIMCMD_END,
};

const union AnimCmd gSpriteAnim_83E45D8[] = 
{
    ANIMCMD_FRAME(2, 3),
    ANIMCMD_END,
};

const union AnimCmd *const gSpriteAnimTable_83E45E0[] = 
{
    gSpriteAnim_83E45D0,
    gSpriteAnim_83E45D8,
};

const struct OamData gOamData_83E45E8 = 
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

void sub_80F31AC(struct Sprite *);

const struct SpriteTemplate gSpriteTemplate_83E45F0 = 
{
    .tileTag = 10,
    .paletteTag = 9,
    .oam = &gOamData_83E45E8,
    .anims = gSpriteAnimTable_83E45E0,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80F31AC,
};

static const struct OamData sOam_ConditionSelectionIcon =
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

static const union AnimCmd sAnim_ConditionSelectionIcon_Selected[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_ConditionSelectionIcon_Unselected[] =
{
    ANIMCMD_FRAME(4, 5),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_ConditionSelectionIcon[] =
{
    sAnim_ConditionSelectionIcon_Selected,
    sAnim_ConditionSelectionIcon_Unselected,
};

const struct SpriteSheet gUnknown_083E4628[] = 
{
    {sConditionPokeball_Gfx, 0x100, 3},
    {sConditionPokeballPlaceholder_Gfx, 0x20, 4},
    {gPokenavConditionCancel_Gfx, 0x100, 5},
    {},
};

extern const u16 gPokenavConditionPokeball_Pal[];
extern const u16 gPokenavCondition4_Pal[];

const struct SpritePalette gUnknown_083E4648[] = 
{
    {gPokenavConditionPokeball_Pal, 4},
    {gPokenavCondition4_Pal, 5},
    {NULL, 0},
};

void SpriteCB_PartyPokeball(struct Sprite *);

const struct SpriteTemplate gSpriteTemplate_83E4660 = 
{
    .tileTag = 3,
    .paletteTag = 4,
    .oam = &sOam_ConditionSelectionIcon,
    .anims = sAnims_ConditionSelectionIcon,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_PartyPokeball,
};

const u16 gMonMarkings_Pal[] = INCBIN_U16("graphics/pokenav/condition/mon_markings.gbapal");

const u16 gRibbonGfxData[][2] =
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

static const struct OamData sOamData_RibbonIconBig =
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

static const union AffineAnimCmd sAffineAnim_RibbonIconBig_Normal[] =
{
    AFFINEANIMCMD_FRAME(0x80, 0x80, 0, 0),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_RibbonIconBig_ZoomIn[] =
{
    AFFINEANIMCMD_FRAME(0x80, 0x80, 0, 0),
    AFFINEANIMCMD_FRAME(0x20, 0x20, 0, 4),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_RibbonIconBig_ZoomOut[] =
{
    AFFINEANIMCMD_FRAME(0x100, 0x100, 0, 0),
    AFFINEANIMCMD_FRAME(0xFFE0, 0xFFE0, 0, 4),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd *const sAffineAnims_RibbonIconBig[] =
{
    sAffineAnim_RibbonIconBig_Normal,
    sAffineAnim_RibbonIconBig_ZoomIn,
    sAffineAnim_RibbonIconBig_ZoomOut,
};

const struct SpriteTemplate gSpriteTemplate_RibbonIconBig =
{
    .tileTag = 11,
    .paletteTag = 10,
    .oam = &sOamData_RibbonIconBig,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = sAffineAnims_RibbonIconBig,
    .callback = SpriteCallbackDummy,
};

const struct SpriteSheet gUnknown_083E4784 = {sPokenavSparkle_Gfx, sizeof(sPokenavSparkle_Gfx), 0x0017};
const struct SpritePalette gUnknown_083E478C = {sPokenavSparkle_Pal, 0x000f};

const s16 gConditionSparkleCoords[][2] =
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

static const struct OamData sOam_ConditionSparkle =
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

static const union AnimCmd sAnim_ConditionSparkle[] =
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
static const union AnimCmd *const sAnims_ConditionSparkle[] =
{
    &sAnim_ConditionSparkle[0],
    &sAnim_ConditionSparkle[2],
    &sAnim_ConditionSparkle[4],
    &sAnim_ConditionSparkle[6],
    &sAnim_ConditionSparkle[8],
    &sAnim_ConditionSparkle[10],
    &sAnim_ConditionSparkle[12],
};

void SpriteCB_ConditionSparkle(struct Sprite *sprite);

const struct SpriteTemplate gSpriteTemplate_ConditionSparkle =
{
    .tileTag = 23,
    .paletteTag = 15,
    .oam = &sOam_ConditionSparkle,
    .anims = sAnims_ConditionSparkle,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_ConditionSparkle,
};

const struct SpritePalette gUnknown_083E4818 = {sSpinningPokenav_Pal, 16};

static const union AnimCmd sSpinningPokenavAnims[] =
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

static const union AnimCmd *const sSpinningPokenavAnimTable[] =
{
    sSpinningPokenavAnims,
};

static const struct OamData sSpinningPokenavSpriteOam =
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

void sub_80F4138(struct Sprite *);

const struct SpriteTemplate gSpinningPokenavSpriteTemplate =
{
    .tileTag = 24,
    .paletteTag = 16,
    .oam = &sSpinningPokenavSpriteOam,
    .anims = sSpinningPokenavAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_80F4138,
};

const struct SpritePalette gUnknown_083E4868 = {Palette_3E42D8, 18};

const struct OamData gOamData_83E4870 = 
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

const struct SpriteTemplate gSpriteTemplate_83E4878 = 
{
    .tileTag = 26,
    .paletteTag = 18,
    .oam = &gOamData_83E4870,
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

const struct ScanlineEffectParams gConditionGraphScanline =
{
    (void *)REG_ADDR_WIN0H,
    ((DMA_ENABLE | DMA_START_HBLANK | DMA_32BIT | DMA_REPEAT | DMA_DEST_RELOAD) << 16) | 1,
    1,
    0,
};

const u16 gUnknown_083E499C[] = 
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

void sub_80EBA5C()
{
    switch (gMain.state)
    {
    default:
        gMain.state = 0;
    case 0:
        gPokenavStructPtr->unk6DAC = IsOverworldLinkActive();
        if (!gPokenavStructPtr->unk6DAC)
        {
            gPokenavStructPtr->unk6DAB = 0;
            gMain.state++;
            SetMainCallback2(&sub_80EBBE8);
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
        sub_80F1A90();
        gMain.state++;
    case 8:
        if (sub_80F1AC4())
            return;
        break;
    case 9:
        sub_80F2688();
        gMain.state++;
        // fall through
    case 10:
        if (sub_80F26BC())
            return;
        break;
    case 11:
        sub_80F36F0();
        gMain.state++;
    case 12:
        if (sub_80F3724())
            return;
        break;
    case 13:
        sub_80EBC10();
        sub_80EBDBC(&sub_80EBDD8);
        break;
    case 14:
        gPokenavStructPtr->unk6DAB = 1;
        PlaySE(SE_POKENAV_ON);
        SetMainCallback2(&sub_80EBD90);
        SetVBlankCallback(&sub_80EBD18);
        sub_80F1A74();
        break;
    }

    gMain.state++;
}

void sub_80EBBE8()
{
    while (!gPokenavStructPtr->unk6DAB)
        sub_80EBA5C();
}

void sub_80EBC10()
{
    u16 i;

    gKeyRepeatStartDelay = 0x14;
    gPokenavStructPtr->unk8828 = CalculatePlayerPartyCount();
    gPokenavStructPtr->unk6DDC = 0;
    gPokenavStructPtr->needsDraw = 0;
    gPokenavStructPtr->unk8768 = 0;
    gPokenavStructPtr->unkCED0 = 0;

    for (i = 0; i < 5; ++i)
    {
        gPokenavStructPtr->conditions[3][i] = 0;
        gPokenavStructPtr->savedPositions[3][i].x = 0x9B;
        gPokenavStructPtr->savedPositions[3][i].y = 0x5B;
    }

    gPokenavStructPtr->regionMap.needUpdateVideoRegs = 0;
    sub_80EBCA8();
}

void sub_80EBCA8()
{
    gPokenavStructPtr->unk6DB2[0] = 1;
    gPokenavStructPtr->unk6DB2[1] = 2;
    gPokenavStructPtr->unk6DB2[2] = 3;

    if (FlagGet(FLAG_SYS_RIBBON_GET))
        gPokenavStructPtr->unk6DB2[3] = 4;
    else
        gPokenavStructPtr->unk6DB2[3] = 0;

    gPokenavStructPtr->unk6DB2[4] = 5;
}

void sub_80EBD18()
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    sub_80F1FF0();
}

void sub_80EBD30()
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    UpdateRegionMapVideoRegs();
    sub_80EFE7C();
}

void sub_80EBD4C()
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    ConditionGraph_Draw();
    ScanlineEffect_InitHBlankDmaTransfer();
}

void sub_80EBD68()
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    sub_80F6F64();
}

void sub_80EBD80()
{
    LoadOam();
    ProcessSpriteCopyRequests();
}

void sub_80EBD90()
{
    gPokenavStructPtr->unk300();
    AnimateSprites();
    BuildOamBuffer();
    RunTasks();
    UpdatePaletteFade();
    sub_80F19FC();
}

void sub_80EBDBC(void (*func)(void))
{
    gPokenavStructPtr->unk300 = func;
    gPokenavStructPtr->unk304 = 0;
}

void sub_80EBDD8()
{
    switch (gPokenavStructPtr->unk304)
    {
    case 0:
        SetVBlankCallback(NULL);
        REG_DISPCNT = 0;
        gPokenavStructPtr->cursorPos = gPokenavStructPtr->unk6DDC;
        gPokenavStructPtr->unk6DAE = 5;
        gPokenavStructPtr->unk304++;
        break;
    case 1:
        sub_80F3FF0();
        gPokenavStructPtr->unk304++;
        // fall through
    case 2:
        if (!sub_80F4024())
			gPokenavStructPtr->unk304++;
        break;
    case 3:
        sub_80F2598();
        gPokenavStructPtr->unk304++;
        break;
    case 4:
        sub_80EEE20();
        gPokenavStructPtr->unk304++;
        // fall through
    case 5:
        if (!sub_80EEE54())
		{
			sub_80EEE08();
			gPokenavStructPtr->unk304++;
		}
        break;
    case 6:
        sub_80EF248(0);
        gPokenavStructPtr->unk304++;
        // fall through
    case 7:
        if (!sub_80EF284(0))
			gPokenavStructPtr->unk304++;
        break;
    case 8:
        sub_80F1B8C(0);
        gPokenavStructPtr->unk304++;
		// fall through
    case 9:
        if (!sub_80F1BC8(0))
			gPokenavStructPtr->unk304++;
        break;
    case 10:
        SetVBlankCallback(&sub_80EBD18);
        gPokenavStructPtr->unk304++;
        break;
    case 11:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        gPokenavStructPtr->unk304++;
        break;
    case 12:
        sub_80EED2C(0);
        gPokenavStructPtr->unk304++;
        break;
    case 13:
        if (!gPaletteFade.active)
			gPokenavStructPtr->unk304++;
        break;
    case 14:
        sub_80F2C80(0);
        gPokenavStructPtr->unk304++;
        // fall through
    case 15:
        if (!sub_80F2CBC(0))
			gPokenavStructPtr->unk304++;
        break;
    case 16:
        sub_80F1DF0();
        gPokenavStructPtr->unk304++;
        break;
    case 17:
        if (!sub_80F1E50())
        {
            sub_80EF428(0, 0);
            sub_80EBDBC(&sub_80EC268);
#if DEBUG
			if (gLinkOpen == TRUE)
				debug_sub_8008218((void *)(VRAM + 0x7DE0), 0, (void *)(VRAM + 0xF800), 4);
#endif
        }
        break;
    }
}

void sub_80EC00C()
{
    switch (gPokenavStructPtr->unk304)
    {
    case 0:
        if (!sub_80EEF78())
        {
            SetVBlankCallback(&sub_80EBD80);
            sub_80EED1C();
            gPokenavStructPtr->cursorPos = gPokenavStructPtr->unk6DDC;
            gPokenavStructPtr->unk6DAE = 5;
            sub_80EEE08();
            gPokenavStructPtr->unk304++;
        }
        break;
    case 1:
        sub_80EF248(0);
        gPokenavStructPtr->unk304++;
        // fall through
    case 2:
        if (!sub_80EF284(0))
			gPokenavStructPtr->unk304++;
        break;
    case 3:
        sub_80F1B8C(0);
        gPokenavStructPtr->unk304++;
        // fall through
    case 4:
        if (!sub_80F1BC8(0))
			gPokenavStructPtr->unk304++;
        break;
    case 5:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavStructPtr->unk304++;
        break;
    case 6:
        BeginNormalPaletteFade(gPokenavStructPtr->unk308, -1, 16, 0, RGB(0, 0, 0));
        SetVBlankCallback(&sub_80EBD18);
        gPokenavStructPtr->unk304++;
        break;
    case 7:
        sub_80EED2C(0);
        gPokenavStructPtr->unk304++;
        break;
    case 8:
        if (!gPaletteFade.active)
			gPokenavStructPtr->unk304++;
        break;
    case 9:
        sub_80F2598();
        gPokenavStructPtr->unk304++;
        break;
    case 10:
        sub_80F2C80(0);
        gPokenavStructPtr->unk304++;
        // fall through
    case 11:
        if (!sub_80F2CBC(0))
			gPokenavStructPtr->unk304++;
        break;
    case 12:
        sub_80F1DF0();
        gPokenavStructPtr->unk304++;
        break;
    case 13:
        if (!sub_80F1E50())
        {
            sub_80EF428(0, gPokenavStructPtr->cursorPos);
			sub_80EBDBC(&sub_80EC268);
#if DEBUG
			if (gLinkOpen == TRUE)
				debug_sub_8008218((void *)(VRAM + 0x7DE0), 0, (void *)(VRAM + 0xF800), 4);
#endif
		}
		break;
    }
}

void sub_80EC210()
{
    switch (gPokenavStructPtr->unk304)
    {
    case 0:
        sub_80F2D04(1);
        sub_80EE9C0(0, gPokenavStructPtr->unk6DDC, 0);
        gPokenavStructPtr->unk304++;
        break;
    case 1:
        if (!sub_80EEA0C())
			sub_80EBDBC(&sub_80EC268);
		break;
    }
}

void sub_80EC268()
{
    u8 var1;

    switch (gPokenavStructPtr->unk304)
    {
    case 0:
        if (sub_80EEC10() != 0)
        {
            PlaySE(SE_SELECT);
            sub_80EF428(0, gPokenavStructPtr->cursorPos);
            sub_80EED9C();
        }
        else
        {
            if (JOY_NEW(A_BUTTON))
            {
                gPokenavStructPtr->unk6DDC = gPokenavStructPtr->cursorPos;
                switch (gPokenavStructPtr->unk6DB2[gPokenavStructPtr->unk6DDC] - 1)
                {
                case 0:
                    PlaySE(SE_SELECT);
                    sub_80EBDBC(&sub_80EC4A0);
                    break;
                case 1:
                    PlaySE(SE_SELECT);
                    sub_80EBDBC(&sub_80EC81C);
                    break;
                case 4:
                    gPokenavStructPtr->unk304 = 1;
                    break;
                case 3:
                    gPokenavStructPtr->unk304 = 2;
                    break;
                case 2:
                    gPokenavStructPtr->unk304 = 6;
                    break;
                }
            }
            else if (JOY_NEW(B_BUTTON))
            {
                gPokenavStructPtr->unk304 = 1;
            }
        }
        break;
    case 1:
        sub_80F208C();
        sub_80EBDBC(&sub_80EE96C);
        break;
    case 2:
        sub_80F6208();
        gPokenavStructPtr->unk304++;
        // fall through
    case 3:
        if (!sub_80F6250())
			gPokenavStructPtr->unk304++;
        break;
    case 4:
        if (!Overworld_IsRecvQueueAtMax())
        {
			if (gPokenavStructPtr->unk8FE8 != 0)
			{
				PlaySE(SE_SELECT);
				sub_80EBDBC(&sub_80EDB88);
			}
			else
			{
				PlaySE(0x20);
				sub_80EF428(0, 5);
				gPokenavStructPtr->unk304 = 0xFF;
			}
		}
		break;
    case 6:
        sub_80F6C20();
        if (gPokenavStructPtr->unkD158 != 0)
        {
            PlaySE(SE_SELECT);
            sub_80EBDBC(&sub_80EE3D8);
        }
        else
        {
            PlaySE(0x20);
            sub_80EF428(0, 6);
            gPokenavStructPtr->unk304 = 0xFF;
        }
        break;
    case 0xFF:
        if ((var1 = sub_80EEC10()) != 0)
        {
            PlaySE(SE_SELECT);
            sub_80EF428(0, gPokenavStructPtr->cursorPos);
            gPokenavStructPtr->unk304 = 0;
            sub_80EED9C();
        }
        else if (JOY_NEW(A_BUTTON | B_BUTTON))
		{
			sub_80EF428(0, gPokenavStructPtr->cursorPos);
			gPokenavStructPtr->unk304 = var1;
		}
        break;
    }
}

void sub_80EC4A0()
{
    u32 var1;

    switch (gPokenavStructPtr->unk304)
    {
    case 0:
        sub_80F1E84();
        sub_80F2D04(0);
        gPokenavStructPtr->unk304++;
        break;
    case 1:
        if (!sub_80F1F10())
        {
            if (gSaveBlock2.regionMapZoom)
                var1 = 0x7;
            else
				var1 = 0x8;
            sub_80EEFBC(var1);
            gPokenavStructPtr->unk304++;
        }
        break;
    case 2:
        if (!sub_80EEF34())
        {
            BeginNormalPaletteFade(gPokenavStructPtr->unk308, -1, 0, 16, RGB(0, 0, 0));
            gPokenavStructPtr->unk304++;
        }
        break;
    case 3:
        if (!gPaletteFade.active)
        {
			SetVBlankCallback(NULL);
			sub_80EED0C();
			gPokenavStructPtr->unk304++;
		}
        break;
    case 4:
        DestroyRematchBlueLightSprite();
        gPokenavStructPtr->unk304++;
        break;
    case 5:
        DestroyMovingDotsBgTask();
        gPokenavStructPtr->unk304++;
        break;
    case 6:
        sub_80EF840();
        gPokenavStructPtr->unk304++;
        // fall through
    case 7:
        if (!sub_80EF874())
			gPokenavStructPtr->unk304++;
        break;
    case 8:
        sub_80F2C80(0x4);
        gPokenavStructPtr->unk304++;
        // fall through
    case 9:
        if (!sub_80F2CBC(0x4))
			gPokenavStructPtr->unk304++;
        break;
    case 0xA:
        sub_80F2DD8();
        SetVBlankCallback(&sub_80EBD30);
        gPokenavStructPtr->unk304++;
        break;
    case 0xB:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavStructPtr->unk304++;
        break;
    case 0xC:
        BeginNormalPaletteFade(gPokenavStructPtr->unk308, -1, 16, 0, RGB(0, 0, 0));
        gPokenavStructPtr->unk304++;
        break;
    case 0xD:
        sub_80EED2C(0x1);
#if DEBUG
		if (gLinkOpen == TRUE)
			debug_sub_8008218((void *)(VRAM + 0x75E0), 0, (void *)(VRAM + 0xF800), 4);
#endif
        gPokenavStructPtr->unk304++;
        break;
    case 0xE:
        if (!gPaletteFade.active)
			sub_80EBDBC(&sub_80EC67C);
		break;
    }
}

void sub_80EC67C()
{
    switch (gPokenavStructPtr->unk304)
    {
    case 0:
        switch (DoRegionMapInputCallback())
        {
        case 1:
            sub_80EED9C();
            break;
        case 3:
            sub_80EF9F8();
            break;
        case 4:
            PlaySE(SE_SELECT);
            gPokenavStructPtr->unk304 = 1;
            break;
        case 5:
            PlaySE(SE_SELECT);
            gPokenavStructPtr->unk304 = 4;
            break;
        }
        break;
    case 1:
        if (!gPokenavStructPtr->regionMap.zoomed)
        {
            SetRegionMapDataForZoom();
            gPokenavStructPtr->unk304++;
        }
        else if (!sub_80EFBDC(1))
        {
			SetRegionMapDataForZoom();
			gPokenavStructPtr->unk304++;
		}
        break;
    case 2:
        if (!UpdateRegionMapZoom())
        {
			if (!gPokenavStructPtr->regionMap.zoomed)
			{
				sub_80EFBB0();
				gPokenavStructPtr->unk304 = 0;
			}
			else
			{
				gPokenavStructPtr->unk304++;
			}
		}
        break;
    case 3:
        if (!sub_80EFBDC(0))
        {
			sub_80EFBB0();
			gPokenavStructPtr->unk304 = 0;
		}
        break;
    case 4:
        BeginNormalPaletteFade(gPokenavStructPtr->unk308, -1, 0, 16, RGB(0, 0, 0));
        gPokenavStructPtr->unk304++;
        break;
    case 5:
        if (!gPaletteFade.active)
        {
			sub_80F2DF4();
			sub_80F2D04(0x4);
			gSaveBlock2.regionMapZoom = (gPokenavStructPtr->regionMap.zoomed == 1) ? 1 : 0;
			sub_80EBDBC(&sub_80EC00C);
		}
		break;
    }
#if DEBUG
	if (gLinkOpen == TRUE)
		debug_sub_8008264(gLink.recvQueue.count, 1, 1, 2, 4);
#endif
}

void sub_80EC81C()
{
    switch (gPokenavStructPtr->unk304)
    {
    case 0:
        sub_80F2D04(0);
        sub_80EE9C0(1, 0, 1);
        gPokenavStructPtr->unk304++;
        break;
    case 1:
        if (!sub_80EEA0C())
			sub_80EBDBC(&sub_80EC86C);
		break;
    }
}

void sub_80EC86C()
{
    switch (gPokenavStructPtr->unk304)
    {
    case 0:
        if (sub_80EEC90())
        {
            PlaySE(SE_SELECT);
            sub_80EF428(1, gPokenavStructPtr->cursorPos);
            sub_80EED9C();
        }
        else if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            gPokenavStructPtr->unk6DF0 = gPokenavStructPtr->cursorPos;
            switch (gPokenavStructPtr->unk6DF0)
            {
            case 0:
                sub_80EBDBC(&sub_80ED620);
                break;
            case 1:
                sub_80EBDBC(&sub_80EC960);
                break;
            case 2:
                sub_80EBDBC(&sub_80EC210);
                break;
            }
        }
        else if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            gPokenavStructPtr->unk6DF0 = 0x2;
            gPokenavStructPtr->cursorPos = 0x2;
            gPokenavStructPtr->unk304++;
        }
        break;
    case 1:
        if (!sub_80F1E6C())
			sub_80EBDBC(&sub_80EC210);
		break;
    }
}

void sub_80EC960()
{
    switch (gPokenavStructPtr->unk304)
    {
    case 0:
        sub_80EE9C0(2, 0, 5);
        gPokenavStructPtr->unk304++;
        break;
    case 1:
        if (!sub_80EEA0C())
			sub_80EBDBC(&sub_80ECC08);
		break;
    }
}

void sub_80EC9A8()
{
    switch (gPokenavStructPtr->unk304)
    {
    case 0:
        sub_80F2D04(5);
        sub_80EE9C0(1, gPokenavStructPtr->unk6DF0, 0xC);
        gPokenavStructPtr->unk304++;
        break;
    case 1:
        if (!sub_80EEA0C())
        {
			sub_80EBDBC(&sub_80EC86C);
			sub_80EF428(1, gPokenavStructPtr->cursorPos);
		}
		break;
    }
}

void sub_80ECA10()
{
    switch (gPokenavStructPtr->unk304)
    {
    case 0:
        if (!sub_80EEF78())
        {
			SetVBlankCallback(&sub_80EBD80);
			sub_80EED1C();
			gPokenavStructPtr->cursorPos = gPokenavStructPtr->unk6DF0;
			gPokenavStructPtr->unk6DAE = 3;
			sub_80EEE08();
			gPokenavStructPtr->unk304++;
		}
        break;
    case 1:
        sub_80EF248(1);
        gPokenavStructPtr->unk304++;
    case 2:
        if (!sub_80EF284(1))
			gPokenavStructPtr->unk304++;
        break;
    case 3:
        sub_80F1B8C(1);
        gPokenavStructPtr->unk304++;
    case 4:
        if (!sub_80F1BC8(1))
			gPokenavStructPtr->unk304++;
        break;
    case 5:
        if (Overworld_IsRecvQueueAtMax()) return;
        gPokenavStructPtr->unk304++;
        break;
    case 6:
        BeginNormalPaletteFade(gPokenavStructPtr->unk308, -1, 16, 0, RGB(0, 0, 0));
        SetVBlankCallback(&sub_80EBD18);
        gPokenavStructPtr->unk304++;
        break;
    case 7:
        sub_80EED2C(0);
        gPokenavStructPtr->unk304++;
        break;
    case 8:
        sub_80F2598();
        gPokenavStructPtr->unk304++;
        break;
    case 9:
        if (!gPaletteFade.active)
			gPokenavStructPtr->unk304++;
        break;
    case 10:
        sub_80F2C80(1);
        gPokenavStructPtr->unk304++;
		// fall through
    case 11:
        if (!sub_80F2CBC(1))
			gPokenavStructPtr->unk304++;
        break;
    case 12:
        sub_80F1DF0();
        gPokenavStructPtr->unk304++;
        break;
    case 13:
        if (!sub_80F1E50())
        {
			sub_80EF428(1, gPokenavStructPtr->cursorPos);
			sub_80EBDBC(&sub_80EC86C);
#if DEBUG
			if (gLinkOpen == TRUE)
				debug_sub_8008218((void *)(VRAM + 0x7DE0), 0, (void *)(VRAM + 0xF800), 4);
#endif
		}
		break;
    }
}

void sub_80ECC08()
{
    switch (gPokenavStructPtr->unk304)
    {
    case 0:
        if (sub_80EEC90())
        {
            PlaySE(SE_SELECT);
            sub_80EF428(2, gPokenavStructPtr->cursorPos);
            sub_80EED9C();
        }
        else if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            switch ((s8)gPokenavStructPtr->cursorPos)
            {
            case 0:
                gPokenavStructPtr->conditionDataId = 22;
                break;
            case 1:
                gPokenavStructPtr->conditionDataId = 23;
                break;
            case 2:
                gPokenavStructPtr->conditionDataId = 24;
                break;
            case 3:
                gPokenavStructPtr->conditionDataId = 33;
                break;
            case 4:
                gPokenavStructPtr->conditionDataId = 47;
                break;
            case 5:
                sub_80EBDBC(&sub_80EC9A8);
                return;
            }

            gPokenavStructPtr->unk6DFC = gPokenavStructPtr->cursorPos;
            gPokenavStructPtr->inSearchMode = 1;
            sub_80EBDBC(&sub_80ED01C);
        }
        else if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            gPokenavStructPtr->cursorPos = 0x5;
            gPokenavStructPtr->unk304++;
        }
        break;
    case 1:
        if (!sub_80F1E6C())
			sub_80EBDBC(&sub_80EC9A8);
        break;
    }
}

void sub_80ECD80()
{
    switch (gPokenavStructPtr->unk304)
    {
    case 0:
        BeginNormalPaletteFade(gPokenavStructPtr->unk308, -1, 0, 16, RGB(0, 0, 0));
        gPokenavStructPtr->unk304++;
        break;
    case 1:
        if (!gPaletteFade.active)
			gPokenavStructPtr->unk304++;
        break;
    case 2:
        if (!sub_80EEF78())
        {
			SetVBlankCallback(&sub_80EBD80);
			gPokenavStructPtr->unk304++;
		}
        break;
    case 3:
        sub_80EED1C();
        sub_80F3130();
        gPokenavStructPtr->unk304++;
        break;
    case 4:
        sub_80F2D6C(0x1);
        gPokenavStructPtr->unk304++;
        break;
    case 5:
        sub_80F2D6C(0x5);
        gPokenavStructPtr->unk304++;
        break;
    case 6:
        gPokenavStructPtr->cursorPos = gPokenavStructPtr->unk6DFC;
        gPokenavStructPtr->unk6DAE = 0x6;
        sub_80EEE08();
        gPokenavStructPtr->unk304++;
        break;
    case 7:
        sub_80EF248(0x2);
        gPokenavStructPtr->unk304++;
        // fall through
    case 8:
        if (!sub_80EF284(0x2))
			gPokenavStructPtr->unk304++;
        break;
    case 9:
        sub_80F1B8C(2);
        gPokenavStructPtr->unk304++;
        // fall through
    case 10:
        if (!sub_80F1BC8(2))
			gPokenavStructPtr->unk304++;
        break;
    case 11:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavStructPtr->unk304++;
        break;
    case 12:
        BeginNormalPaletteFade(gPokenavStructPtr->unk308, -1, 16, 0, RGB(0, 0, 0));
        SetVBlankCallback(&sub_80EBD18);
        gPokenavStructPtr->unk304++;
        break;
    case 13:
        sub_80EED2C(0);
        gPokenavStructPtr->unk304++;
        break;
    case 14:
        sub_80F2598();
        gPokenavStructPtr->unk304++;
        break;
    case 15:
        if (!gPaletteFade.active)
			gPokenavStructPtr->unk304++;
        break;
    case 16:
        sub_80F2C80(0x1);
        gPokenavStructPtr->unk304++;
        // fall through
    case 17:
        if (!sub_80F2CBC(0x1))
        {
			gPokenavStructPtr->unk306 = 0;
			gPokenavStructPtr->unk304++;
		}
        break;
    case 18:
        sub_80F2C80(0x5);
        gPokenavStructPtr->unk304++;
        // fall through
    case 19:
        if (!sub_80F2CBC(0x5))
        {
			sub_80F1DF0();
			gPokenavStructPtr->unk304++;
		}
        break;
    case 20:
        if (!sub_80F1E50())
		{
			sub_80EF428(2, gPokenavStructPtr->cursorPos);
			sub_80EBDBC(&sub_80ECC08);
#if DEBUG
			if (gLinkOpen == TRUE)
				debug_sub_8008218((void *)(VRAM + 0x7DE0), 0, (void *)(VRAM + 0xF800), 4);
#endif
		}
		break;
    }
}

void sub_80ED01C()
{
    switch (gPokenavStructPtr->unk304)
    {
    case 0:
        sub_80F1E84();
        sub_80F2D04(0x1);
        sub_80F2D04(0x5);
        gPokenavStructPtr->unk304++;
        break;
    case 1:
        if (!sub_80F1F10())
        {
			sub_80EEFBC(0);
			gPokenavStructPtr->unk304++;
		}
        break;
    case 2:
        if (!sub_80EEF34())
        {
			BeginNormalPaletteFade(gPokenavStructPtr->unk308, -1, 0, 16, RGB(0, 0, 0));
			gPokenavStructPtr->unk304++;
		}
        break;
    case 3:
        if (!gPaletteFade.active)
        {
			SetVBlankCallback(NULL);
			sub_80EED0C();
			DestroyMovingDotsBgTask();
			gPokenavStructPtr->unk304++;
		}
        break;
    case 4:
        DestroyRematchBlueLightSprite();
        gPokenavStructPtr->unk304++;
        break;
    case 5:
        sub_80F4D44();
        gPokenavStructPtr->unk304++;
        // fall through
    case 6:
        if (!sub_80F4D88())
            gPokenavStructPtr->unk304 += 2;
        else
			gPokenavStructPtr->unk304 += 1;
        break;
    case 7:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavStructPtr->unk304--;
        break;
    case 8:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavStructPtr->unk304++;
        break;
    case 9:
        sub_80F0264(0);
        gPokenavStructPtr->unk304++;
        // fall through
    case 10:
        if (!sub_80F02A0())
			gPokenavStructPtr->unk304++;
        break;
    case 11:
        sub_80F3008(0);
        gPokenavStructPtr->unk304++;
        break;
    case 12:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavStructPtr->unk304++;
        break;
    case 13:
        BeginNormalPaletteFade(gPokenavStructPtr->unk308, -1, 16, 0, RGB(0, 0, 0));
        SetVBlankCallback(&sub_80EBD18);
        gPokenavStructPtr->unk304++;
        break;
    case 14:
        sub_80EED2C(0x4);
        gPokenavStructPtr->unk304++;
        break;
    case 15:
        if (!gPaletteFade.active)
        {
			gPokenavStructPtr->unk306 = 0;
			gPokenavStructPtr->unk304++;
		}
        break;
    case 16:
        sub_80F2C80(0x1);
        gPokenavStructPtr->unk304++;
        // fall through
    case 17:
        if (!sub_80F2CBC(0x1))
        {
			gPokenavStructPtr->unk306 = 0;
			gPokenavStructPtr->unk304++;
		}
        break;
    case 18:
        sub_80F2C80(gPokenavStructPtr->unk6DFC + 7);
        gPokenavStructPtr->unk304++;
        // fall through
    case 19:
        if (!sub_80F2CBC(gPokenavStructPtr->unk6DFC + 7))
		{
			sub_80EBDBC(&sub_80ED31C);
#if DEBUG
			if (gLinkOpen == TRUE)
				debug_sub_8008218((void *)(VRAM + 0x7DE0), 0, (void *)(VRAM + 0xF800), 4);
#endif
		}
        break;
    }
}

void sub_80ED31C()
{
    switch (gPokenavStructPtr->unk304)
    {
    case 0:
        switch (sub_80F5DD4())
        {
        case 1:
            PlaySE(SE_SELECT);
            ShowMapNamePopUpWindow();
            return;
        case 2:
            PlaySE(SE_SELECT);
            gPokenavStructPtr->unk304++;
            return;
        default:
            if (JOY_NEW(A_BUTTON))
            {
                PlaySE(SE_SELECT);
                sub_80EBDBC(&sub_80ED4D8);
				return;
            }
            else if (JOY_NEW(B_BUTTON))
            {
                PlaySE(SE_SELECT);
                sub_80EBDBC(&sub_80ECD80);
				return;
            }
            break;
        }
        break;
    case 1:
        if (!sub_80F0718())
        {
			ShowMapNamePopUpWindow();
			sub_80F3264();
			gPokenavStructPtr->unk304++;
		}
        break;
    case 2:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavStructPtr->unk304 = 0;
        break;
    }
#if DEBUG
	if (gLinkOpen == TRUE)
		debug_sub_8008264(gLink.recvQueue.count, 1, 8, 2, 4);
#endif
}

void sub_80ED3D0()
{
    switch (gPokenavStructPtr->unk304)
    {
    case 0:
        SetVBlankCallback(NULL);
        gPokenavStructPtr->unk304++;
        break;
    case 1:
        sub_80EED0C();
        sub_80F6134();
        gPokenavStructPtr->unk304++;
        break;
    case 2:
        sub_80EEFBC(0);
        gPokenavStructPtr->unk304++;
        break;
    case 3:
        sub_80F0264(0);
        gPokenavStructPtr->unk304++;
        // fall through
    case 4:
        if (!sub_80F02A0())
			gPokenavStructPtr->unk304++;
        break;
    case 5:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavStructPtr->unk304++;
        break;
    case 6:
        sub_80F3008(0);
        BeginNormalPaletteFade(gPokenavStructPtr->unk308, -1, 16, 0, RGB(0, 0, 0));
        SetVBlankCallback(&sub_80EBD18);
        gPokenavStructPtr->unk304++;
        break;
    case 7:
        sub_80EED2C(0x4);
        gPokenavStructPtr->unk304++;
#if DEBUG
		if (gLinkOpen == TRUE)
			debug_sub_8008218((void *)(VRAM + 0x7DE0), 0, (void *)(VRAM + 0xF800), 4);
#endif
        break;
    case 8:
        if (!gPaletteFade.active)
			sub_80EBDBC(&sub_80ED31C);
		break;
    }
}

void sub_80ED4D8()
{
    switch (gPokenavStructPtr->unk304)
    {
    case 0:
        BeginNormalPaletteFade(gPokenavStructPtr->unk308, -1, 0, 16, RGB(0, 0, 0));
        gPokenavStructPtr->unk304++;
        break;
    case 1:
        if (!gPaletteFade.active)
        {
			SetVBlankCallback(NULL);
			sub_80EED0C();
			sub_80F3130();
			gPokenavStructPtr->unk304++;
		}
        break;
    case 2:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavStructPtr->unk304++;
        break;
    case 3:
        sub_80F4CF0();
        gPokenavStructPtr->unk304++;
        break;
    case 4:
        sub_80EFF34();
        gPokenavStructPtr->unk304++;
        // fall through
    case 5:
        if (!sub_80EFF68())
			gPokenavStructPtr->unk304++;
        break;
    case 6:
        sub_80F35B4();
        sub_80EEFBC(0x2);
        BeginNormalPaletteFade(gPokenavStructPtr->unk308, -1, 16, 0, RGB(0, 0, 0));
        SetVBlankCallback(sub_80EBD4C);
        gPokenavStructPtr->unk304++;
        break;
    case 7:
        sub_80EED2C(0x2);
        gPokenavStructPtr->unk304++;
#if DEBUG
		if (gLinkOpen == TRUE)
			debug_sub_8008218((void *)(VRAM + 0x7DE0), 0, (void *)(VRAM + 0xF000), 4);
#endif
        break;
    case 8:
        if (!gPaletteFade.active)
			sub_80EBDBC(&sub_80ED858);
        break;
    }
}

void sub_80ED620()
{
    switch (gPokenavStructPtr->unk304)
    {
    case 0:
        sub_80F1E84();
        sub_80F2D04(0x1);
        gPokenavStructPtr->unk304++;
        break;
    case 1:
        if (!sub_80F1F10())
        {
			sub_80EEFBC(0x1);
			gPokenavStructPtr->unk304++;
		}
        break;
    case 2:
        if (!sub_80EEF34())
        {
            BeginNormalPaletteFade(gPokenavStructPtr->unk308, -1, 0x0, 16, RGB(0, 0, 0));
            gPokenavStructPtr->unk304++;
        }
        break;
    case 3:
        if (!gPaletteFade.active)
        {
			SetVBlankCallback(NULL);
			sub_80EED0C();
			DestroyMovingDotsBgTask();
			gPokenavStructPtr->inSearchMode = 0;
			gPokenavStructPtr->unk304++;
		}
        break;
    case 4:
        DestroyRematchBlueLightSprite();
        gPokenavStructPtr->unk304++;
        break;
    case 5:
        gPokenavStructPtr->unkD162 = 0x2;
        sub_80F4BD0();
        gPokenavStructPtr->unk304++;
        break;
    case 6:
        sub_80EFF34();
        gPokenavStructPtr->unk304++;
        // fall through
    case 7:
        if (!sub_80EFF68())
			gPokenavStructPtr->unk304++;
        break;
    case 8:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavStructPtr->unk304++;
        break;
    case 9:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavStructPtr->unk304++;
        break;
    case 10:
        sub_80F33A8();
        BeginNormalPaletteFade(gPokenavStructPtr->unk308, -1, 16, 0, RGB(0, 0, 0));
        SetVBlankCallback(&sub_80EBD4C);
        gPokenavStructPtr->unk304++;
        break;
    case 11:
        sub_80EED2C(0x2);
        gPokenavStructPtr->unk304++;
        break;
    case 12:
        if (!gPaletteFade.active)
			gPokenavStructPtr->unk304++;
        break;
    case 13:
        sub_80F2C80(0x1);
        gPokenavStructPtr->unk304++;
    case 14:
        if (!sub_80F2CBC(0x1))
			gPokenavStructPtr->unk304++;
        break;
    case 15:
        sub_80F2C80(0x6);
        gPokenavStructPtr->unk304++;
        // fall through
    case 16:
        if (!sub_80F2CBC(0x6))
		{
			sub_80EBDBC(&sub_80ED858);
#if DEBUG
			if (gLinkOpen == TRUE)
				debug_sub_8008218((void *)(VRAM + 0x7DE0), 0, (void *)(VRAM + 0xF000), 4);
#endif
		}
        break;
    }
}

void DoConditionGraphEnterTransition(void);
void ToggleGraphData(bool8);
void sub_80F3668(void);
void sub_80F3698(void);
void sub_80F2F48(void);
void sub_80F3614(void);
void sub_80F357C(void);
void DoConditionGraphExitTransition(void);

void sub_80ED858(void)
{
    switch (gPokenavStructPtr->unk304)
    {
    case 0:
        DoConditionGraphEnterTransition();
        ConditionGraph_InitResetScanline();
        gPokenavStructPtr->unk304 = 1;
        break;
    case 1:
        if (!ConditionGraph_ResetScanline())
            gPokenavStructPtr->unk304 = 2;
        break;
    case 2:
        ToggleGraphData(1);
        gPokenavStructPtr->unk304 = 3;
        break;
    case 3:
        if (!ConditionMenu_UpdateMonEnter())
        {
            sub_80F3C94();
            CreateConditionSparkleSprites();
            gPokenavStructPtr->unk304 = 4;
        }
        break;
    case 4:
        if (JOY_HELD(DPAD_UP))
        {
            if (gPokenavStructPtr->unk87CB && (
                gPokenavStructPtr->inSearchMode == 0
                || gPokenavStructPtr->unk87DC != 0
            ))
            {
                PlaySE(SE_SELECT);
                sub_80F5060(1);
                DestroyConditionSparkleSprites();
                gPokenavStructPtr->unk304 = 5;
                break;
            }
        }
        if (JOY_HELD(DPAD_DOWN))
        {
            if (gPokenavStructPtr->unk87CB && (
                gPokenavStructPtr->inSearchMode == 0
                || gPokenavStructPtr->unk87DC < gPokenavStructPtr->unk8774
            ))
            {
                PlaySE(SE_SELECT);
                sub_80F5060(0);
                DestroyConditionSparkleSprites();
                gPokenavStructPtr->unk304 = 5;
                break;
            }
        }
        if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            DoConditionGraphExitTransition();
            DestroyConditionSparkleSprites();
            gPokenavStructPtr->unk304 = 9;
            break;
        }
        if (JOY_NEW(A_BUTTON))
        {
            if (gPokenavStructPtr->inSearchMode == 0)
            {
                if (gPokenavStructPtr->unk87DC == gPokenavStructPtr->unk87DA - 1)
                {
                    PlaySE(SE_SELECT);
                    gPokenavStructPtr->unk304 = 9;
                }
            }
            else
            {
                if (gPokenavStructPtr->unk6DAC == 0)
                {
                    PlaySE(SE_SELECT);
                    gPokenavStructPtr->unk304 = 7;
                }
            }
            break;
        }
        break;
    case 5:
        if (!gpu_sync_bg_show())
        {
            CreateConditionSparkleSprites();
            gPokenavStructPtr->unk304 = 6;
        }
        break;
    case 6:
        if (!Overworld_IsRecvQueueAtMax())
            gPokenavStructPtr->unk304 = 4;
        break;
    case 7:
        sub_80EEFBC(3);
        sub_80F3668();
        gPokenavStructPtr->unk304 = 8;
        break;
    case 8:
        if (!HandleMonMarkingsMenuInput())
        {
            sub_80EEFBC(2);
            sub_80F3698();
            gPokenavStructPtr->unk304 = 4;
        }
        break;
    case 9:
        if (!ConditionMenu_UpdateMonExit())
        {
            ToggleGraphData(0);
            sub_80F2F48();
            BeginNormalPaletteFade(gPokenavStructPtr->unk308, -1, 0, 16, RGB_BLACK);
            gPokenavStructPtr->unk304 = 11;
        }
        break;
    case 10:
        break;
    case 11:
        if (!gPaletteFade.active)
        {
            FreeConditionSparkles();
            sub_80F5BDC();
            if (!gPokenavStructPtr->inSearchMode)
            {
                sub_80F357C();
                sub_80F2D6C(1);
                sub_80F2D6C(6);
                sub_80EBDBC(sub_80ECA10);
            }
            else
            {
                sub_80F3614();
                sub_80EBDBC(sub_80ED3D0);
            }
        }
        break;
    }

#if DEBUG
    if (gLinkOpen == TRUE)
        debug_sub_8008264(gLink.recvQueue.count, 9, 6, 2, 4);
#endif
}

void sub_80EDB88()
{
    switch (gPokenavStructPtr->unk304)
    {
    case 0:
        sub_80F1E84();
        sub_80F2D04(0);
        gPokenavStructPtr->unk304++;
        break;
    case 1:
        if (!sub_80F1F10())
        {
			sub_80EEFBC(0x4);
			gPokenavStructPtr->unk304++;
		}
        break;
    case 2:
        if (!sub_80EEF34())
        {
            BeginNormalPaletteFade(gPokenavStructPtr->unk308, -1, 0, 16, RGB(0, 0, 0));
            gPokenavStructPtr->unk304++;
        }
        break;
    case 3:
        if (!gPaletteFade.active)
        {
			SetVBlankCallback(NULL);
			sub_80EED0C();
			DestroyMovingDotsBgTask();
			gPokenavStructPtr->unk304++;
		}
        break;
    case 4:
        DestroyRematchBlueLightSprite();
        gPokenavStructPtr->unk304++;
        break;
    case 5:
        sub_80F638C();
        gPokenavStructPtr->unk304++;
    case 6:
        if (!sub_80F63D0())
            gPokenavStructPtr->unk304 += 2;
        else
			gPokenavStructPtr->unk304 += 1;
        break;
    case 7:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavStructPtr->unk304--;
        break;
    case 8:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavStructPtr->unk304++;
        break;
    case 9:
        sub_80F0264(0x1);
        gPokenavStructPtr->unk304++;
    case 10:
        if (!sub_80F02A0())
			gPokenavStructPtr->unk304++;
        break;
    case 11:
        sub_80F2C80(0x2);
        gPokenavStructPtr->unk304++;
    case 12:
        if (!sub_80F2CBC(0x2))
			gPokenavStructPtr->unk304++;
        break;
    case 13:
        sub_80F3008(0x1);
        gPokenavStructPtr->unk304++;
        break;
    case 14:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavStructPtr->unk304++;
        break;
    case 15:
        BeginNormalPaletteFade(gPokenavStructPtr->unk308, -1, 16, 0, RGB(0, 0, 0));
        SetVBlankCallback(&sub_80EBD18);
        gPokenavStructPtr->unk304++;
        break;
    case 16:
        sub_80EED2C(0x4);
        gPokenavStructPtr->unk304++;
#if DEBUG
		if (gLinkOpen == TRUE)
			debug_sub_8008218((void *)(VRAM + 0x7DE0), 0, (void *)(VRAM + 0xF800), 4);
#endif
        break;
    case 17:
        if (!gPaletteFade.active)
			sub_80EBDBC(&sub_80EDDBC);
        break;
    }
}

void sub_80EDDBC()
{
    switch (gPokenavStructPtr->unk304)
    {
    case 0:
        switch (sub_80F5DD4())
        {
        case 1:
            PlaySE(SE_SELECT);
            ShowMapNamePopUpWindow();
            return;
        case 2:
            PlaySE(SE_SELECT);
            gPokenavStructPtr->unk304++;
            return;
        default:
            if (JOY_NEW(A_BUTTON))
            {
                PlaySE(SE_SELECT);
                sub_80EBDBC(&sub_80EDEE4);
				return;
            }
            else if (JOY_NEW(B_BUTTON))
            {
                PlaySE(SE_SELECT);
                sub_80EBDBC(&sub_80EDE70);
				return;
            }
            break;
        }
        break;
    case 1:
        if (!sub_80F0718())
        {
			ShowMapNamePopUpWindow();
			sub_80F3264();
			gPokenavStructPtr->unk304++;
		}
        break;
    case 2:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavStructPtr->unk304 = 0;
        break;
    }
#if DEBUG
	if (gLinkOpen == TRUE)
		debug_sub_8008264(gLink.recvQueue.count, 1, 8, 2, 4);
#endif
}

void sub_80EDE70()
{
    switch (gPokenavStructPtr->unk304)
    {
    case 0:
        BeginNormalPaletteFade(gPokenavStructPtr->unk308, -1, 0, 16, RGB(0, 0, 0));
        gPokenavStructPtr->unk304++;
        break;
    case 1:
        if (!gPaletteFade.active)
        {
			sub_80F3130();
			sub_80F2D6C(0x2);
			gPokenavStructPtr->unk304++;
		}
        break;
    case 2:
        sub_80EBDBC(&sub_80EC00C);
        break;
    }

}

void sub_80EDEE4()
{
    switch (gPokenavStructPtr->unk304)
    {
    case 0:
        BeginNormalPaletteFade(gPokenavStructPtr->unk308, -1, 0, 16, RGB(0, 0, 0));
        gPokenavStructPtr->unk304++;
        break;
    case 1:
        if (!gPaletteFade.active)
        {
			SetVBlankCallback(NULL);
			sub_80EED0C();
			sub_80F3130();
			gPokenavStructPtr->unk304++;
		}
        break;
    case 2:
        sub_80F66E0();
        sub_80EEE08();
        gPokenavStructPtr->unk304++;
        break;
    case 3:
        if (!sub_80F1080())
			gPokenavStructPtr->unk304++;
        break;
    case 4:
        sub_80EEFBC(0x5);
        sub_80F38B8();
        gPokenavStructPtr->unk304++;
    case 5:
        if (!sub_80F38EC())
			gPokenavStructPtr->unk304++;
        break;
    case 6:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavStructPtr->unk304++;
        break;
    case 7:
        BeginNormalPaletteFade(gPokenavStructPtr->unk308, -1, 16, 0, RGB(0, 0, 0));
        SetVBlankCallback(&sub_80EBD18);
        gPokenavStructPtr->unk304++;
        break;
    case 8:
        sub_80EED2C(0x3);
        gPokenavStructPtr->unk304++;
#if DEBUG
		if (gLinkOpen == TRUE)
			debug_sub_8008218((void *)(VRAM + 0x7DE0), 0, (void *)(VRAM + 0xF000), 4);
#endif
        break;
    case 9:
        if (!gPaletteFade.active)
			gPokenavStructPtr->unk304++;
        break;
    case 10:
        if (!MoveConditionMonOnscreen())
			sub_80EBDBC(&sub_80EE06C);
        break;
    }
}

void sub_80EE06C()
{
    switch (gPokenavStructPtr->unk304)
    {
    case 0:
        sub_80F15A8();
        gPokenavStructPtr->unk304++;
        break;
    case 1:
        if (JOY_REPEAT(0x40) && (gPokenavStructPtr->unk87DC))
        {
			PlaySE(SE_SELECT);
			sub_80F6A4C(-1);
			gPokenavStructPtr->unk304++;
        }
		else if (JOY_REPEAT(0x80) && gPokenavStructPtr->unk87DC < gPokenavStructPtr->unk8774)
		{
            PlaySE(SE_SELECT);
            sub_80F6A4C(1);
            gPokenavStructPtr->unk304++;
        }
        else if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            sub_80EEFBC(0xB);
            gPokenavStructPtr->unk304 = 0x4;
        }
        else if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            sub_80EBDBC(&sub_80EE294);
        }
        break;
    case 2:
        if (!sub_80F6AF0())
			gPokenavStructPtr->unk304++;
        break;
    case 3:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavStructPtr->unk304 = 0;
        break;
    case 4:
        sub_80F3B00();
        gPokenavStructPtr->unk304++;
        break;
    case 5:
        if (!sub_80F3B58())
		{
			sub_80F1494();
			gPokenavStructPtr->unk304++;
			sub_80EED9C();
		}
        break;
    case 6:
        switch (sub_80F68E8())
        {
        case 1:
            PlaySE(SE_SELECT);
            sub_80F3B94();
            gPokenavStructPtr->unk304 = 0x7;
            return;
        default:
        case 0:
            if (JOY_NEW(B_BUTTON))
            {
                PlaySE(SE_SELECT);
                sub_80F3B94();
                gPokenavStructPtr->unk304 = 0x8;
            }
            break;
        }
        break;
    case 7:
        if (!sub_80F3BD4())
			gPokenavStructPtr->unk304 = 0x4;
        break;
    case 8:
        if (!sub_80F3BD4())
		{
			sub_80EEFBC(0x5);
			gPokenavStructPtr->unk304 = 0;
		}
        break;
    }
#if DEBUG
	if (gLinkOpen == TRUE)
		debug_sub_8008264(gLink.recvQueue.count, 1, 4, 2, 4);
#endif
}

void sub_80EE294()
{
    switch (gPokenavStructPtr->unk304)
    {
    case 0:
        BeginNormalPaletteFade(gPokenavStructPtr->unk308, -1, 0, 16, RGB(0, 0, 0));
        gPokenavStructPtr->unk304++;
        break;
    case 1:
        if (!gPaletteFade.active)
        {
			SetVBlankCallback(NULL);
			sub_80EED0C();
			sub_80F3C2C();
			sub_80EEE08();
			gPokenavStructPtr->unk304++;
		}
        break;
    case 2:
        sub_80F6134();
        sub_80F0264(0x1);
        gPokenavStructPtr->unk304++;
    case 3:
        if (!sub_80F02A0())
			gPokenavStructPtr->unk304++;
        break;
    case 4:
        sub_80EEFBC(0x4);
        sub_80F3008(0x1);
        SetVBlankCallback(&sub_80EBD18);
        gPokenavStructPtr->unk304++;
        break;
    case 5:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavStructPtr->unk304++;
        break;
    case 6:
        BeginNormalPaletteFade(gPokenavStructPtr->unk308, -1, 16, 0, RGB(0, 0, 0));
        gPokenavStructPtr->unk304++;
        break;
    case 7:
        sub_80EED2C(0x4);
        gPokenavStructPtr->unk304++;
#if DEBUG
		if (gLinkOpen == TRUE)
			debug_sub_8008218((void *)(VRAM + 0x7DE0), 0, (void *)(VRAM + 0xF800), 4);
#endif
        break;
    case 8:
        if (!gPaletteFade.active)
			sub_80EBDBC(&sub_80EDDBC);
        break;
    }
}

void sub_80EE3D8()
{
    switch (gPokenavStructPtr->unk304)
    {
    case 0:
        sub_80F1E84();
        sub_80F2D04(0);
        gPokenavStructPtr->unk304++;
        break;
    case 1:
        if (!sub_80F1F10())
        {
			sub_80EEFBC(0x9);
			gPokenavStructPtr->unk304++;
		}
        break;
    case 2:
        if (!sub_80EEF34())
        {
            BeginNormalPaletteFade(gPokenavStructPtr->unk308, -1, 0, 16, RGB(0, 0, 0));
            gPokenavStructPtr->unk304++;
        }
        break;
    case 3:
        if (!gPaletteFade.active)
        {
			SetVBlankCallback(NULL);
			sub_80EED0C();
			DestroyMovingDotsBgTask();
			sub_80EEE08();
			gPokenavStructPtr->unk304++;
		}
        break;
    case 4:
        DestroyRematchBlueLightSprite();
        gPokenavStructPtr->unk304++;
        break;
    case 5:
        sub_80F0264(0x2);
        gPokenavStructPtr->unk304++;
    case 6:
        if (!sub_80F02A0())
			gPokenavStructPtr->unk304++;
        break;
    case 7:
        sub_80F2C80(0x3);
        gPokenavStructPtr->unk304++;
    case 8:
        if (!sub_80F2CBC(0x3))
			gPokenavStructPtr->unk304++;
        break;
    case 9:
        sub_80F3008(0x2);
        gPokenavStructPtr->unk304++;
        break;
    case 10:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavStructPtr->unk304++;
        break;
    case 11:
        sub_80F6F10();
        BeginNormalPaletteFade(gPokenavStructPtr->unk308, -1, 16, 0, RGB(0, 0, 0));
        SetVBlankCallback(&sub_80EBD68);
        gPokenavStructPtr->unk304++;
        break;
    case 12:
        sub_80EED2C(0x5);
        gPokenavStructPtr->unk304++;
#if DEBUG
		if (gLinkOpen == TRUE)
			debug_sub_8008218((void *)(VRAM + 0x7DE0), 0, (void *)(VRAM + 0xF800), 4);
#endif
        break;
    case 13:
        if (!gPaletteFade.active)
			sub_80EBDBC(&sub_80EE58C);
        break;
    }
}

void sub_80EE58C()
{
    switch (gPokenavStructPtr->unk304)
    {
    case 0:
        switch (sub_80F5DD4())
        {
        case 1:
            PlaySE(SE_SELECT);
            sub_80F0FFC(gPokenavStructPtr->unk876E);
            return;
        case 2:
            PlaySE(SE_SELECT);
            gPokenavStructPtr->unk304++;
            return;
        default:
            if (JOY_NEW(A_BUTTON))
            {
                PlaySE(SE_SELECT);
                sub_80EBDBC(&sub_80EE658);
				return;
            }
            else if (JOY_NEW(B_BUTTON))
            {
                PlaySE(SE_SELECT);
                sub_80EBDBC(&sub_80EE8F4);
				return;
            }
            break;
        }
        break;
    case 1:
        if (!sub_80F0718())
        {
			sub_80F0FFC(gPokenavStructPtr->unk876E);
			sub_80F3264();
			gPokenavStructPtr->unk304++;
		}
        break;
    case 2:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavStructPtr->unk304 = 0;
        break;
    }
#if DEBUG
	if (gLinkOpen == TRUE)
		debug_sub_8008264(gLink.recvQueue.count, 1, 3, 2, 4);
#endif
}

void sub_80EE658()
{
    switch (gPokenavStructPtr->unk304)
    {
    case 0:
        sub_80F3294(0x1);
        sub_80EEFBC(0xA);
        sub_80F0B24();
        gPokenavStructPtr->unk304++;
        break;
    case 1:
        if (!sub_80F0B44())
			gPokenavStructPtr->unk304++;
        break;
    case 2:
        sub_80F0F64();
        gPokenavStructPtr->unk304++;
        break;
    case 3:
        if (!sub_80F0FEC())
        {
			sub_80F6FB8(0x1);
			gPokenavStructPtr->unk304++;
		}
        break;
    case 4:
        sub_80F6DB8();
        gPokenavStructPtr->unk304++;
        break;
    case 5:
        sub_80F700C((u8 *)(gSharedMem + 0x8788), *(u16 *)(gSharedMem + 0x8788 - 0x1A));
        sub_80F42C4((u8 *)(gSharedMem + 0x8788));
        gPokenavStructPtr->unk304++;
        break;
    case 6:
        LoadTrainerEyesDescriptionLines();
        gPokenavStructPtr->unk304++;
    case 7:
        if (!sub_80F6E9C() && !sub_80F0D5C())
			gPokenavStructPtr->unk304++;
        break;
    case 8:
        if (JOY_HELD(0x40) && gPokenavStructPtr->unk87DC)
        {
			PlaySE(SE_SELECT);
			sub_80F708C(-1);
			gPokenavStructPtr->unk304 = 0x10;
        }
        else if (JOY_HELD(0x80) && gPokenavStructPtr->unk87DC < gPokenavStructPtr->unk8774)
        {
			PlaySE(SE_SELECT);
			sub_80F708C(1);
			gPokenavStructPtr->unk304 = 0x10;
        }
		else if (JOY_NEW(B_BUTTON))
		{
            PlaySE(SE_SELECT);
            sub_80F4394();
            sub_80F0EC0();
            gPokenavStructPtr->unk304++;
        }
        break;
    case 9:
        if (!sub_80F6ED4() && !sub_80F0EF4())
        {
			sub_80F6FB8(0);
			sub_80F2FB0();
			gPokenavStructPtr->unk304++;
		}
        break;
    case 10:
        sub_80F6134();
        gPokenavStructPtr->unk304++;
        break;
    case 11:
        sub_80F0FA0();
        gPokenavStructPtr->unk304++;
        break;
    case 12:
        if (!sub_80F0FEC())
			gPokenavStructPtr->unk304++;
        break;
    case 13:
        sub_80F0C28();
        gPokenavStructPtr->unk304++;
        break;
    case 14:
        if (!sub_80F0C48())
			gPokenavStructPtr->unk304++;
        break;
    case 15:
        sub_80EEFBC(0x9);
        sub_80F3294(0);
        sub_80EBDBC(&sub_80EE58C);
        sub_80EED9C();
        break;
    case 16:
        if (!sub_80F70FC())
			gPokenavStructPtr->unk304++;
        break;
    case 17:
        if (!Overworld_IsRecvQueueAtMax())
			gPokenavStructPtr->unk304 = 0x8;
        break;
    }
}

void sub_80EE8F4()
{
    switch (gPokenavStructPtr->unk304)
    {
    case 0:
        BeginNormalPaletteFade(gPokenavStructPtr->unk308, -1, 0, 16, RGB(0, 0, 0));
        gPokenavStructPtr->unk304++;
        break;
    case 1:
        if (!gPaletteFade.active)
        {
			sub_80F3130();
			sub_80F2D6C(0x2);
			sub_80F6FFC();
			gPokenavStructPtr->unk304++;
		}
        break;
    case 2:
        sub_80EBDBC(&sub_80EC00C);
        break;
    }
}

void sub_80EE96C()
{
    if (!gPokenavStructPtr->unk304)
    {
        PlaySE(0x6F);
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        gPokenavStructPtr->unk304++;
    }
    else
    {
        if (!gPaletteFade.active)
        {
			sub_80F5BDC();
			CB2_ReturnToFieldWithOpenMenu();
		}
    }
}

void sub_80EE9C0(u8 param1, u8 param2, u8 param3)
{
    sub_80F1E84();
    gPokenavStructPtr->unk6E14 = param1;
    gPokenavStructPtr->unk6E15 = param2;
    gPokenavStructPtr->unk6E16 = 0;
    gPokenavStructPtr->unk6E17 = param3;
}

bool8 sub_80EF4F8(void);
void sub_80EF490(u8);

bool8 sub_80EEA0C(void)
{
    switch (gPokenavStructPtr->unk6E16)
    {
    case 0:
        if (!sub_80F1F10())
        {
            if (gPokenavStructPtr->unk6E17 != 12)
                gPokenavStructPtr->unk6E16 = 1;
            else
                gPokenavStructPtr->unk6E16 = 3;
        }
        break;
    case 1:
        sub_80F2C80(gPokenavStructPtr->unk6E17);
        gPokenavStructPtr->unk6E16++;
        // fallthrough
    case 2:
        if (!sub_80F2CBC(gPokenavStructPtr->unk6E17))
            gPokenavStructPtr->unk6E16++;
        break;
    case 3:
        gPokenavStructPtr->cursorPos = gPokenavStructPtr->unk6E15;
        switch (gPokenavStructPtr->unk6E14)
        {
        case 0:
            gPokenavStructPtr->unk6DAE = 5;
            break;
        case 1:
            gPokenavStructPtr->unk6DAE = 3;
            break;
        case 2:
            gPokenavStructPtr->unk6DAE = 6;
            break;
        }
        sub_80F1B8C(gPokenavStructPtr->unk6E14);
        gPokenavStructPtr->unk6E16++;
        // fallthrough
    case 4:
        if (!sub_80F1BC8(gPokenavStructPtr->unk6E14))
            gPokenavStructPtr->unk6E16++;
        break;
    case 5:
        if (!Overworld_IsRecvQueueAtMax())
            gPokenavStructPtr->unk6E16++;
        break;
    case 6:
        sub_80F1DF0();
        sub_80EF490(gPokenavStructPtr->unk6E14);
        gPokenavStructPtr->unk6E16++;
        break;
    case 7:
        if (!sub_80F1E50())
        {
            sub_80EF428(gPokenavStructPtr->unk6E14, gPokenavStructPtr->cursorPos);
            gPokenavStructPtr->unk6E16++;
        }
        break;
    case 8:
        if (!sub_80EF4F8())
        {
            gPokenavStructPtr->unk6E16++;
            return FALSE;
        }
        break;
    default:
        return FALSE;
    }

    return TRUE;
}

// var6dad and var6dae must be s8 in this func
bool8 sub_80EEC10()
{
    if (JOY_NEW(DPAD_UP))
    {
        do
        {
            if (--gPokenavStructPtr->cursorPos < 0)
                gPokenavStructPtr->cursorPos = gPokenavStructPtr->unk6DAE - 1;
        } while (!gPokenavStructPtr->unk6DB2[gPokenavStructPtr->cursorPos]);
        return TRUE;
    }
    if (JOY_NEW(DPAD_DOWN))
    {
        do
        {
            if (++gPokenavStructPtr->cursorPos >= gPokenavStructPtr->unk6DAE)
                gPokenavStructPtr->cursorPos = 0;
        } while (!gPokenavStructPtr->unk6DB2[gPokenavStructPtr->cursorPos]);
        return TRUE;
    }
#if DEBUG
	if (gLinkOpen == TRUE)
		debug_sub_8008264(gLink.recvQueue.count, 6, 10, 2, 4);
#endif
    return FALSE;
}

bool8 sub_80EEC90()
{
    if (JOY_NEW(DPAD_UP))
    {
        if (--gPokenavStructPtr->cursorPos < 0)
            gPokenavStructPtr->cursorPos = gPokenavStructPtr->unk6DAE - 1;
        return TRUE;
    }
    if (JOY_NEW(DPAD_DOWN))
    {
        if (++gPokenavStructPtr->cursorPos >= gPokenavStructPtr->unk6DAE)
            gPokenavStructPtr->cursorPos = 0;
        return TRUE;
    }
#if DEBUG
	if (gLinkOpen == TRUE)
		debug_sub_8008264(gLink.recvQueue.count, 6, 10, 2, 4);
#endif
    return FALSE;
}

void sub_80EED0C()
{
    REG_DISPCNT = 512;
}

void sub_80EED1C()
{
    REG_DISPCNT = 0x92 << 5;
}

void sub_80EED2C(u8 param1)
{
    switch (param1) {
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

void sub_80EED9C(void)
{
    gUnknown_03000744 = gPokenavStructPtr->unk300;
    gPokenavStructPtr->unk300 = &sub_80EEDC4;
    gPokenavStructPtr->unk300();
}

void sub_80EEDC4(void)
{
    if (Overworld_IsRecvQueueAtMax() != 0x1)
        gPokenavStructPtr->unk300 = gUnknown_03000744;
}
