#include "global.h"
#include "constants/rgb.h"
#include "graphics.h"
#include "gba/m4a_internal.h"
#include "intro.h"
#include "data.h"
#include "decompress.h"
#include "hall_of_fame.h"
#include "intro_credits_graphics.h"
#include "libgcnmultiboot.h"
#include "link.h"
#include "m4a.h"
#include "main.h"
#include "new_game.h"
#include "palette.h"
#include "random.h"
#include "save.h"
#include "constants/songs.h"
#include "sound.h"
#include "constants/species.h"
#include "task.h"
#include "title_screen.h"
#include "trig.h"
#include "scanline_effect.h"
#include "ewram.h"
extern const u16 gIntroCopyright_Tilemap[];
extern void *const gUnknown_0840B5A0[];

static EWRAM_DATA u16 sIntroCharacterGender = 0;
static EWRAM_DATA u16 gUnknown_0203931A = 0;

u32 gIntroFrameCounter;
struct GcmbStruct gMultibootProgramStruct;

//--------------------------------------------------
// Graphics Data
//--------------------------------------------------

static const u16 sIntroDrops_Pal[] = INCBIN_U16("graphics/intro/scene_1/drops.gbapal");
static const u16 sIntroLogo_Pal[] = INCBIN_U16("graphics/intro/scene_1/logo.gbapal");
#if ENGLISH
static const u8 gIntroDropsLogo_Gfx[] = INCBIN_U8("graphics/intro/scene_1/drops_logo.4bpp.lz");
#elif GERMAN
extern const u8 gIntroDropsLogo_Gfx[];
#endif
static const u16 sIntro1Bg_Pal[][16] =
{
    INCBIN_U16("graphics/intro/scene_1/bg_pal1.gbapal"),
    INCBIN_U16("graphics/intro/scene_1/bg_pal2.gbapal"),
    INCBIN_U16("graphics/intro/scene_1/bg_pal3.gbapal"),
    INCBIN_U16("graphics/intro/scene_1/bg_pal4.gbapal"),
    INCBIN_U16("graphics/intro/scene_1/bg_pal5.gbapal"),
    INCBIN_U16("graphics/intro/scene_1/bg_pal6.gbapal"),
    INCBIN_U16("graphics/intro/scene_1/bg_pal7.gbapal"),
    INCBIN_U16("graphics/intro/scene_1/bg_pal8.gbapal"),
    INCBIN_U16("graphics/intro/scene_1/bg_pal9.gbapal"),
    INCBIN_U16("graphics/intro/scene_1/bg_pal10.gbapal"),
    INCBIN_U16("graphics/intro/scene_1/bg_pal11.gbapal"),
    INCBIN_U16("graphics/intro/scene_1/bg_pal12.gbapal"),
    INCBIN_U16("graphics/intro/scene_1/bg_pal13.gbapal"),
    INCBIN_U16("graphics/intro/scene_1/bg_pal14.gbapal"),
    INCBIN_U16("graphics/intro/scene_1/bg_pal15.gbapal"),
    INCBIN_U16("graphics/intro/scene_1/bg_pal16.gbapal"),
};
static const u8 sIntro1Bg0_Tilemap[] = INCBIN_U8("graphics/intro/scene_1/bg0_map.bin.lz");
static const u8 sIntro1Bg1_Tilemap[] = INCBIN_U8("graphics/intro/scene_1/bg1_map.bin.lz");
static const u8 sIntro1Bg2_Tilemap[] = INCBIN_U8("graphics/intro/scene_1/bg2_map.bin.lz");
static const u8 sIntro1Bg3_Tilemap[] = INCBIN_U8("graphics/intro/scene_1/bg3_map.bin.lz");
static const u8 sIntro1Bg_Gfx[] = INCBIN_U8("graphics/intro/scene_1/bg.4bpp.lz");
static const u16 sIntroPokeball_Pal[] = INCBIN_U16("graphics/intro/scene_3/pokeball.gbapal");
static const u8 sIntroPokeball_Tilemap[] = INCBIN_U8("graphics/intro/scene_3/pokeball_map.bin.lz");
static const u8 sIntroPokeball_Gfx[] = INCBIN_U8("graphics/intro/scene_3/pokeball.8bpp.lz");
static const u16 sIntroStreaks_Pal[] = INCBIN_U16("graphics/intro/scene_3/streaks.gbapal");
static const u8 sIntroStreaks_Gfx[] = INCBIN_U8("graphics/intro/scene_3/streaks.4bpp.lz");
static const u8 sIntroStreaks_Tilemap[] = INCBIN_U8("graphics/intro/scene_3/streaks_map.bin.lz");
static const u16 gIntro3Misc1Palette[] = INCBIN_U16("graphics/intro/intro3_misc1.gbapal");
static const u16 gIntro3Misc2Palette[] = INCBIN_U16("graphics/intro/intro3_misc2.gbapal");
static const u8 gIntro3MiscTiles[] = INCBIN_U8("graphics/intro/intro3_misc.4bpp.lz");
static const u16 gIntro1EonPalette[] = INCBIN_U16("graphics/intro/scene_1/lati.gbapal");
static const u8 sIntroLati_Gfx[] = INCBIN_U8("graphics/intro/scene_1/lati.4bpp.lz");
static const struct OamData sOamData_WaterDrop =
{
    .y = 160,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 2,
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};
static const union AnimCmd sAnim_WaterDrop_UpperHalf[] =
{
    ANIMCMD_FRAME(16, 8),
    ANIMCMD_END,
};
static const union AnimCmd sAnim_WaterDrop_LowerHalf[] =
{
    ANIMCMD_FRAME(24, 8),
    ANIMCMD_END,
};
static const union AnimCmd sAnim_WaterDrop_Reflection[] =
{
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_END,
};
static const union AnimCmd sAnim_WaterDrop_Ripple[] =
{
    ANIMCMD_FRAME(48, 8),
    ANIMCMD_END,
};
static const union AnimCmd *const sAnims_WaterDrop[] =
{
    sAnim_WaterDrop_UpperHalf,
    sAnim_WaterDrop_LowerHalf,
    sAnim_WaterDrop_Reflection,
    sAnim_WaterDrop_Ripple,
};
static void SpriteCB_WaterDrop(struct Sprite *sprite);
static const struct SpriteTemplate sSpriteTemplate_WaterDrop =
{
    .tileTag = 2000,
    .paletteTag = 2000,
    .oam = &sOamData_WaterDrop,
    .anims = sAnims_WaterDrop,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_WaterDrop,
};
static const union AnimCmd sAnim_PlayerBicycle_Fast[] =
{
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(64, 4),
    ANIMCMD_FRAME(128, 4),
    ANIMCMD_FRAME(192, 4),
    ANIMCMD_JUMP(0),
};
static const union AnimCmd sAnim_PlayerBicycle_Slow[] =
{
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_FRAME(64, 8),
    ANIMCMD_FRAME(128, 8),
    ANIMCMD_FRAME(192, 8),
    ANIMCMD_JUMP(0),
};
static const union AnimCmd sAnim_PlayerBicycle_LookBack[] =
{
    ANIMCMD_FRAME(256, 4),
    ANIMCMD_FRAME(0x140, 4),
    ANIMCMD_FRAME(0x180, 4),
    ANIMCMD_END,
};
static const union AnimCmd sAnim_PlayerBicycle_LookForward[] =
{
    ANIMCMD_FRAME(0x180, 16),
    ANIMCMD_FRAME(0x140, 16),
    ANIMCMD_FRAME(256, 16),
    ANIMCMD_END,
};
static const union AnimCmd *const sAnims_PlayerBicycle[] =
{
    sAnim_PlayerBicycle_Fast,
    sAnim_PlayerBicycle_Slow,
    sAnim_PlayerBicycle_LookBack,
    sAnim_PlayerBicycle_LookForward,
};
static const struct OamData sOamData_GameFreakLetter =
{
    .y = 160,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 1,
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};
static const struct OamData sOamData_PresentsLetter =
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
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};
static const struct OamData sOamData_GameFreakLogo =
{
    .y = 160,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 2,
    .x = 0,
    .matrixNum = 0,
    .size = 3,
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};
static const union AnimCmd sAnim_GameFreakLetter_G[] =
{
    ANIMCMD_FRAME(80, 8),
    ANIMCMD_END,
};
static const union AnimCmd sAnim_GameFreakLetter_A[] =
{
    ANIMCMD_FRAME(84, 8),
    ANIMCMD_END,
};
static const union AnimCmd sAnim_GameFreakLetter_M[] =
{
    ANIMCMD_FRAME(88, 8),
    ANIMCMD_END,
};
static const union AnimCmd sAnim_GameFreakLetter_E[] =
{
    ANIMCMD_FRAME(92, 8),
    ANIMCMD_END,
};
static const union AnimCmd sAnim_GameFreakLetter_F[] =
{
    ANIMCMD_FRAME(96, 8),
    ANIMCMD_END,
};
static const union AnimCmd sAnim_GameFreakLetter_R[] =
{
    ANIMCMD_FRAME(100, 8),
    ANIMCMD_END,
};
static const union AnimCmd sAnim_GameFreakLetter_K[] =
{
    ANIMCMD_FRAME(104, 8),
    ANIMCMD_END,
};
static const union AnimCmd sAnim_PresentsLetter_P[] =
{
    ANIMCMD_FRAME(112, 8),
    ANIMCMD_END,
};
static const union AnimCmd sAnim_PresentsLetter_R[] =
{
    ANIMCMD_FRAME(113, 8),
    ANIMCMD_END,
};
static const union AnimCmd sAnim_PresentsLetter_E[] =
{
    ANIMCMD_FRAME(114, 8),
    ANIMCMD_END,
};
static const union AnimCmd sAnim_PresentsLetter_S[] =
{
    ANIMCMD_FRAME(115, 8),
    ANIMCMD_END,
};
static const union AnimCmd sAnim_PresentsLetter_N[] =
{
    ANIMCMD_FRAME(116, 8),
    ANIMCMD_END,
};
static const union AnimCmd sAnim_PresentsLetter_T[] =
{
    ANIMCMD_FRAME(117, 8),
    ANIMCMD_END,
};
static const union AnimCmd sAnim_GameFreakLogo[] =
{
    ANIMCMD_FRAME(128, 8),
    ANIMCMD_END,
};
#if GERMAN
static const union AnimCmd gSpriteAnim_8416DA4[] =
{
    ANIMCMD_FRAME(118, 8),
    ANIMCMD_END,
};
#endif
static const union AnimCmd *const sAnims_GameFreakLetter[] =
{
    sAnim_GameFreakLetter_G,
    sAnim_GameFreakLetter_A,
    sAnim_GameFreakLetter_M,
    sAnim_GameFreakLetter_E,
    sAnim_GameFreakLetter_F,
    sAnim_GameFreakLetter_R,
    sAnim_GameFreakLetter_K,
};
static const union AnimCmd *const sAnims_PresentsLetter[] =
{
    sAnim_PresentsLetter_P,
    sAnim_PresentsLetter_R,
    sAnim_PresentsLetter_E,
    sAnim_PresentsLetter_S,
    sAnim_PresentsLetter_N,
    sAnim_PresentsLetter_T,
#if GERMAN
    gSpriteAnim_8416DA4,
#endif
};
static const union AnimCmd *const sAnims_GameFreakLogo[] =
{
    sAnim_GameFreakLogo,
};
static const s16 sGameFreakLetterData[][2] =
{
    {0, -72},
    {1, -56},
    {2, -40},
    {3, -24},
    {4, 8},
    {5, 24},
    {3, 40},
    {1, 56},
    {6, 72},
};
static const s16 sPresentsLetterData[][2] =
{
    {0, -28},
    {1, -20},
    {2, -12},
    {3, -4},
    {2, 4},
    {4, 12},
    {5, 20},
    {3, 28},
};
static void sub_813D908(struct Sprite *sprite);
static const struct SpriteTemplate sSpriteTemplate_GameFreakLetter =
{
    .tileTag = 2000,
    .paletteTag = 2001,
    .oam = &sOamData_GameFreakLetter,
    .anims = sAnims_GameFreakLetter,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_813D908,
};
static const struct SpriteTemplate sSpriteTemplate_PresentsLetter =
{
    .tileTag = 2000,
    .paletteTag = 2001,
    .oam = &sOamData_PresentsLetter,
    .anims = sAnims_PresentsLetter,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_813D908,
};
static const struct SpriteTemplate sSpriteTemplate_GameFreakLogo =
{
    .tileTag = 2000,
    .paletteTag = 2001,
    .oam = &sOamData_GameFreakLogo,
    .anims = sAnims_GameFreakLogo,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_813D908,
};
static const struct OamData gOamData_840AFDC =
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
static const union AnimCmd gSpriteAnim_840AFE4[] =
{
    ANIMCMD_FRAME(0, 10),
    ANIMCMD_JUMP(0),
};
static const union AnimCmd *const gSpriteAnimTable_840AFEC[] =
{
    gSpriteAnim_840AFE4,
};
static void sub_813DA64(struct Sprite *sprite);
static const struct SpriteTemplate gSpriteTemplate_840AFF0 =
{
    .tileTag = 2002,
    .paletteTag = 2002,
    .oam = &gOamData_840AFDC,
    .anims = gSpriteAnimTable_840AFEC,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_813DA64,
};
static const struct CompressedSpriteSheet sSpriteSheet_WaterDropsAndLogo[] =
{
    {gIntroDropsLogo_Gfx, 0x1400, 2000},
    {NULL},
};
const struct CompressedSpriteSheet gUnknown_0840B018[] =
{
    {sIntroLati_Gfx, 0x400, 2002},
    {NULL},
};
static const struct SpritePalette sSpritePalettes_Intro1[] =
{
    {sIntroDrops_Pal, 2000},
    {sIntroLogo_Pal, 2001},
    {gIntro1EonPalette, 2002},
    {NULL},
};
static const union AnimCmd gUnknown_0840B048[] =
{
    ANIMCMD_FRAME(3, 0),
    ANIMCMD_END,
};
static const union AnimCmd gUnknown_0840B050[] =
{
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_END,
};
static const union AnimCmd gUnknown_0840B058[] =
{
    ANIMCMD_FRAME(1, 8),
    ANIMCMD_FRAME(2, 8),
    ANIMCMD_END,
};
static const union AnimCmd *const gUnknown_0840B064[] =
{
    gUnknown_0840B048,
    gUnknown_0840B050,
    gUnknown_0840B058,
};
static const struct OamData gOamData_840B070 =
{
    .y = 160,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 1,
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};
static const union AnimCmd gSpriteAnim_840B078[] =
{
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_END,
};
static const union AnimCmd *const gSpriteAnimTable_840B080[] =
{
    gSpriteAnim_840B078,
};
static void sub_813E30C(struct Sprite *sprite);
static const struct SpriteTemplate gSpriteTemplate_840B084 =
{
    .tileTag = 2002,
    .paletteTag = 2002,
    .oam = &gOamData_840B070,
    .anims = gSpriteAnimTable_840B080,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_813E30C,
};
static const struct OamData gOamData_840B09C =
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
static const union AnimCmd gSpriteAnim_840B0A4[] =
{
    ANIMCMD_FRAME(1, 8),
    ANIMCMD_END,
};
static const union AnimCmd *const gSpriteAnimTable_840B0AC[] =
{
    gSpriteAnim_840B0A4,
};
static void sub_813E4B8(struct Sprite *sprite);
static const struct SpriteTemplate gSpriteTemplate_840B0B0 =
{
    .tileTag = 2003,
    .paletteTag = 2003,
    .oam = &gOamData_840B09C,
    .anims = gSpriteAnimTable_840B0AC,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_813E4B8,
};
static const struct OamData gOamData_840B0C8 =
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
static const union AnimCmd gSpriteAnim_840B0D0[] =
{
    ANIMCMD_FRAME(14, 8),
    ANIMCMD_END,
};
static const union AnimCmd *const gSpriteAnimTable_840B0D8[] =
{
    gSpriteAnim_840B0D0,
};
static void sub_813E5E0(struct Sprite *sprite);
static const struct SpriteTemplate gSpriteTemplate_840B0DC =
{
    .tileTag = 2003,
    .paletteTag = 2004,
    .oam = &gOamData_840B0C8,
    .anims = gSpriteAnimTable_840B0D8,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_813E5E0,
};
static void sub_813E6C0(struct Sprite *sprite);
static const struct SpriteTemplate gSpriteTemplate_840B0F4 =
{
    .tileTag = 2003,
    .paletteTag = 2004,
    .oam = &gOamData_840B0C8,
    .anims = gSpriteAnimTable_840B0D8,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_813E6C0,
};
static const struct OamData gOamData_840B10C =
{
    .y = 160,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 1,
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};
static const union AnimCmd gSpriteAnim_840B114[] =
{
    ANIMCMD_FRAME(6, 8),
    ANIMCMD_FRAME(6, 8, .hFlip = TRUE),
    ANIMCMD_JUMP(0),
};
static const union AnimCmd *const gSpriteAnimTable_840B120[] =
{
    gSpriteAnim_840B114,
};
static void sub_813E804(struct Sprite *sprite);
static const struct SpriteTemplate gSpriteTemplate_840B124 =
{
    .tileTag = 2003,
    .paletteTag = 2004,
    .oam = &gOamData_840B10C,
    .anims = gSpriteAnimTable_840B120,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_813E804,
};
static const struct OamData gOamData_840B13C =
{
    .y = 160,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 1,
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};
static const union AnimCmd gSpriteAnim_840B144[] =
{
    ANIMCMD_FRAME(10, 8),
    ANIMCMD_END,
};
static const union AnimCmd *const gSpriteAnimTable_840B14C[] =
{
    gSpriteAnim_840B144,
};
static void sub_813E980(struct Sprite *sprite);
static const struct SpriteTemplate gSpriteTemplate_840B150 =
{
    .tileTag = 2003,
    .paletteTag = 2004,
    .oam = &gOamData_840B13C,
    .anims = gSpriteAnimTable_840B14C,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_813E980,
};
static const u8 gUnknown_0840B168[] = {0xE6, 0xEB, 0xE4, 0xEA, 0xE5, 0xE9, 0xE7, 0xE8};
static void sub_813EA60(struct Sprite *sprite);
static const struct SpriteTemplate gSpriteTemplate_840B170 =
{
    .tileTag = 2003,
    .paletteTag = 2004,
    .oam = &gOamData_840B13C,
    .anims = gSpriteAnimTable_840B14C,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_813EA60,
};
static const u16 gUnknown_0840B188[] = {0x200, 0x1C0, 0x180, 0x140, 0x100, 0xE0, 0xC0, 0xA0, 0x80, 0x80};
static const struct OamData gOamData_840B19C =
{
    .y = 160,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 1,
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};
static const union AnimCmd gSpriteAnim_840B1A4[] =
{
    ANIMCMD_FRAME(2, 8),
    ANIMCMD_END,
};
static const union AnimCmd *const gSpriteAnimTable_840B1AC[] =
{
    gSpriteAnim_840B1A4,
};
static void sub_813EBBC(struct Sprite *sprite);
static const struct SpriteTemplate gSpriteTemplate_840B1B0 =
{
    .tileTag = 2003,
    .paletteTag = 2004,
    .oam = &gOamData_840B19C,
    .anims = gSpriteAnimTable_840B1AC,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_813EBBC,
};
static void sub_813EC90(struct Sprite *sprite);
static const struct SpriteTemplate gSpriteTemplate_840B1C8 =
{
    .tileTag = 2003,
    .paletteTag = 2004,
    .oam = &gOamData_840B19C,
    .anims = gSpriteAnimTable_840B1AC,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_813EC90,
};
static const struct OamData gOamData_840B1E0 =
{
    .y = 160,
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
static const union AnimCmd gSpriteAnim_840B1E8[] =
{
    ANIMCMD_FRAME(16, 8),
    ANIMCMD_END,
};
static const union AnimCmd *const gSpriteAnimTable_840B1F0[] =
{
    gSpriteAnim_840B1E8,
};
static void sub_813EDFC(struct Sprite *sprite);
static const struct SpriteTemplate gSpriteTemplate_840B1F4 =
{
    .tileTag = 2003,
    .paletteTag = 2003,
    .oam = &gOamData_840B1E0,
    .anims = gSpriteAnimTable_840B1F0,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = sub_813EDFC,
};
const struct CompressedSpriteSheet gIntro3PokeballGfx_Table[] =
{
    {gBallGfx_Poke, 0x100, 2002},
    {NULL},
};
const struct CompressedSpriteSheet gIntro3MiscGfx_Table[] =
{
    {gIntro3MiscTiles, 0xa00, 2003},
    {NULL},
};
const struct CompressedSpritePalette gInterfacePokeballPal_Table[] =
{
    {gBallPal_Poke, 2002},
    {NULL},
};
const struct SpritePalette gIntro3MiscPal_Table[] =
{
    {gIntro3Misc1Palette, 2003},
    {gIntro3Misc2Palette, 2004},
    {NULL},
};

// Game Freak probably used the raw address here.
// Treating this like a u8 * causes the compiler
// to remove it at link time.
u8 (*const ewram0arr)[32] = (u8 (*)[32])gSharedMem;

static void MainCB2_EndIntro(void);
static void Task_Scene1_Load(u8 taskId);
static void Task_Scene1_FadeIn(u8 taskId);
static void Task_Scene1_WaterDrops(u8 taskId);
static void Task_Scene1_PanUp(u8 taskId);
static void Task_Scene1_End(u8 taskId);
static void Task_Scene2_Load(u8 taskId);
static void Task_Scene2_CreateSprites(u8 taskId);
static void Task_Scene2_BikeRide(u8 taskId);
static void Task_Scene2_End(u8 taskId);
static void Task_Scene3_Load(u8 taskId);
static void Task_Scene3_SpinPokeball(u8 taskId);
static void Task_IntroWaitToSetupPart3DoubleFight(u8);
static void Task_IntroLoadPart3Streaks(u8);
static void task_intro_14(u8);
static void Task_Scene3_NarrowWindow(u8 taskId);
static void Task_Scene3_EndNarrowWindow(u8 taskId);
static void task_intro_17(u8);
static void Task_IntroPokemonBattle(u8);
static void Task_EndIntroMovie(u8 taskId);
static void task_intro_20(u8);
static void IntroResetGpuRegs(void);
static void sub_813CCE8(u8);
static u16 sub_813CE88(u16, s16, s16, u16, u8);
static u8 sub_813CFA8(u16, u16, u16, u16);
static void sub_813D084(u8);
static void SpriteCB_WaterDrop_Slide(struct Sprite *sprite);
static void SpriteCB_WaterDrop_ReachLeafEnd(struct Sprite *sprite);
static void SpriteCB_WaterDrop_DangleFromLeaf(struct Sprite *sprite);
static void SpriteCB_WaterDrop_Fall(struct Sprite *sprite);
static u8 CreateWaterDrop(s16 x, s16 y, u16 c, u16 d, u16 e, u8 fallImmediately);
static void SpriteCB_PlayerOnBicycle(struct Sprite *sprite);
static void sub_813D880(struct Sprite *);
static u8 CreateGameFreakLogoSprites(s16 x, s16 y, u8 c);
static void sub_813DB9C(struct Sprite *);
static void sub_813DE70(struct Sprite *);
static void sub_813E10C(struct Sprite *);
static void sub_813E210(struct Sprite *);
static void sub_813E580(u16, u16);
static void sub_813E7C0(u8);
static void sub_813E930(u8);
static void InitIntroTorchicAttackAnim(u8);
static void InitIntroMudkipAttackAnim(u8);

static void VBlankCB_Intro(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void MainCB2_Intro(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
    if (gMain.newKeys && !gPaletteFade.active)
        SetMainCallback2(MainCB2_EndIntro);
    else if (gIntroFrameCounter != -1)
        gIntroFrameCounter++;
}

static void MainCB2_EndIntro(void)
{
    if (!UpdatePaletteFade())
        SetMainCallback2(CB2_InitTitleScreen);
}

static void LoadCopyrightGraphics(u16 tilesetAddress, u16 tilemapAddress, u16 paletteOffset)
{
    LZ77UnCompVram(gIntroCopyright_Gfx, (void *)(VRAM + tilesetAddress));
    LoadPalette(gIntroCopyright_Pal, paletteOffset, 0x20);
    CpuCopy16(gIntroCopyright_Tilemap, (void *)(VRAM + tilemapAddress), 0x500);
}

static void SerialCB_CopyrightScreen(void)
{
    GameCubeMultiBoot_HandleSerialInterrupt(&gMultibootProgramStruct);
}

static u8 SetUpCopyrightScreen(void)
{
    u16 ime;

    switch (gMain.state)
    {
    case 0:
        SetVBlankCallback(NULL);
        REG_BLDCNT = 0;
        REG_BLDALPHA = 0;
        REG_BLDY = 0;
        *(u16 *)PLTT = 0x7FFF;
        REG_DISPCNT = 0;
        REG_BG0HOFS = 0;
        REG_BG0VOFS = 0;
        DmaFill16(3, 0, (void *)VRAM, VRAM_SIZE);
        DmaFill32(3, 0, (void *)OAM, OAM_SIZE);
        DmaFill16(3, 0, (void *)(PLTT + 2), PLTT_SIZE - 2);
        ResetPaletteFade();
        LoadCopyrightGraphics(0, 0x3800, 0);
        ScanlineEffect_Stop();
        ResetTasks();
        ResetSpriteData();
        FreeAllSpritePalettes();
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB_WHITEALPHA);
        REG_BG0CNT = BGCNT_PRIORITY(0)
                   | BGCNT_CHARBASE(0)
                   | BGCNT_SCREENBASE(7)
                   | BGCNT_16COLOR
                   | BGCNT_TXT256x256;
        ime = REG_IME;
        REG_IME = 0;
        REG_IE |= INTR_FLAG_VBLANK;
        REG_IME = ime;
        REG_DISPSTAT |= DISPSTAT_VBLANK_INTR;
        SetVBlankCallback(VBlankCB_Intro);
        REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON;
        SetSerialCallback(SerialCB_CopyrightScreen);
        GameCubeMultiBoot_Init(&gMultibootProgramStruct);
    default:
        UpdatePaletteFade();
        gMain.state++;
        GameCubeMultiBoot_Main(&gMultibootProgramStruct);
        break;
    case 140:
        GameCubeMultiBoot_Main(&gMultibootProgramStruct);
        if (gMultibootProgramStruct.gcmb_field_2 != 1)
        {
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            gMain.state++;
        }
        break;
    case 141:
        if (UpdatePaletteFade())
            break;
        CreateTask(Task_Scene1_Load, 0);
        SetMainCallback2(MainCB2_Intro);
        if (gMultibootProgramStruct.gcmb_field_2)
        {
            GameCubeMultiBoot_ExecuteProgram(&gMultibootProgramStruct);
        }
        else
        {
            GameCubeMultiBoot_Quit();
            SetSerialCallback(SerialCB);
        }
        return 0;
    }

    return 1;
}

void CB2_InitCopyrightScreenAfterBootup(void)
{
    if (!SetUpCopyrightScreen())
    {
        ResetMenuAndMonGlobals();
        Save_ResetSaveCounters();
        LoadGameSave(SAVE_NORMAL);
        if (gSaveFileStatus == 0 || gSaveFileStatus == 2)
            Sav2_ClearSetDefault();
        SetPokemonCryStereo(gSaveBlock2.optionsSound);
    }
}

void CB2_InitCopyrightScreenAfterTitleScreen(void)
{
    SetUpCopyrightScreen();
}

static void Task_Scene1_Load(u8 taskId)
{
    SetVBlankCallback(NULL);
    sIntroCharacterGender = Random() & 1;
    IntroResetGpuRegs();
    REG_BG3VOFS = 0;
    REG_BG2VOFS = 0x50;
    REG_BG1VOFS = 0x18;
    REG_BG0VOFS = 0x28;
    LZ77UnCompVram(sIntro1Bg_Gfx, (void *)VRAM);
    LZ77UnCompVram(sIntro1Bg0_Tilemap, (void *)(VRAM + 0x8000));
    DmaClear16(3, VRAM + 0x8800, 0x800);
    LZ77UnCompVram(sIntro1Bg1_Tilemap, (void *)(VRAM + 0x9000));
    DmaClear16(3, VRAM + 0x9800, 0x800);
    LZ77UnCompVram(sIntro1Bg2_Tilemap, (void *)(VRAM + 0xA000));
    DmaClear16(3, VRAM + 0xA800, 0x800);
    LZ77UnCompVram(sIntro1Bg3_Tilemap, (void *)(VRAM + 0xB000));
    DmaClear16(3, VRAM + 0xB800, 0x800);
    LoadPalette(sIntro1Bg_Pal, 0, sizeof(sIntro1Bg_Pal));
    REG_BG3CNT = BGCNT_PRIORITY(3) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(22) | BGCNT_16COLOR | BGCNT_TXT256x512;
    REG_BG2CNT = BGCNT_PRIORITY(2) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(20) | BGCNT_16COLOR | BGCNT_TXT256x512;
    REG_BG1CNT = BGCNT_PRIORITY(1) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(18) | BGCNT_16COLOR | BGCNT_TXT256x512;
    REG_BG0CNT = BGCNT_PRIORITY(0) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(16) | BGCNT_16COLOR | BGCNT_TXT256x512;
    LoadCompressedSpriteSheet(&sSpriteSheet_WaterDropsAndLogo[0]);
    LoadCompressedSpriteSheet(&gUnknown_0840B018[0]);
    LoadSpritePalettes(sSpritePalettes_Intro1);
    CpuCopy16(gPlttBufferUnfaded + 0x100, gPlttBufferUnfaded + 0x1F0, 0x20);
    CpuCopy16(gPlttBufferUnfaded + 0x100, gPlttBufferUnfaded + 0x1E1, 0x1E);
    CpuCopy16(gPlttBufferUnfaded + 0x100, gPlttBufferUnfaded + 0x1D2, 0x1C);
    CpuCopy16(gPlttBufferUnfaded + 0x100, gPlttBufferUnfaded + 0x1C3, 0x1A);
    CpuCopy16(gPlttBufferUnfaded + 0x100, gPlttBufferUnfaded + 0x1B4, 0x18);
    CpuCopy16(gPlttBufferUnfaded + 0x100, gPlttBufferUnfaded + 0x1A5, 0x16);
    CpuCopy16(gPlttBufferUnfaded + 0x100, gPlttBufferUnfaded + 0x196, 0x14);
    gTasks[taskId].data[0] = CreateWaterDrop(236, -14, 0x200, 1, 0x78, FALSE);
    gTasks[taskId].func = Task_Scene1_FadeIn;
}

static void Task_Scene1_FadeIn(u8 taskId)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
    SetVBlankCallback(VBlankCB_Intro);
    REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG_ALL_ON | DISPCNT_OBJ_ON;
    gTasks[taskId].func = Task_Scene1_WaterDrops;
    gIntroFrameCounter = 0;
    m4aSongNumStart(0x19E);
    ResetSerial();
}

static void Task_Scene1_WaterDrops(u8 taskId)
{
    //start moving rock
    if (gIntroFrameCounter == 76)
        gSprites[gTasks[taskId].data[0]].data[0] = 1;

    //drop rock
    if (gIntroFrameCounter == 251)
        gSprites[gTasks[taskId].data[0]].data[0] = 2;

    if (gIntroFrameCounter == 368)
        CreateWaterDrop(48, 0, 0x400, 5, 0x70, TRUE);
    if (gIntroFrameCounter == 384)
        CreateWaterDrop(200, 60, 0x400, 9, 0x80, TRUE);

    if (gIntroFrameCounter == 560)
        CreateGameFreakLogoSprites(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, CreateTask(sub_813CCE8, 0));

    if (gIntroFrameCounter > 739)
    {
        gTasks[taskId].data[1] = 0x50;
        gTasks[taskId].data[2] = 0;
        gTasks[taskId].data[3] = 0x18;
        gTasks[taskId].data[4] = 0;
        gTasks[taskId].data[5] = 0x28;
        gTasks[taskId].data[6] = 0;
        gTasks[taskId].func = Task_Scene1_PanUp;
    }
}

static void Task_Scene1_PanUp(u8 taskId)
{
    if (gIntroFrameCounter < 904)
    {
        s32 offset;

        //slide backgrounds downward
        offset = (gTasks[taskId].data[1] << 16) + (u16)gTasks[taskId].data[2] - 0xC000;
        gTasks[taskId].data[1] = offset >> 16;
        gTasks[taskId].data[2] = offset;
        REG_BG2VOFS = gTasks[taskId].data[1];
        offset = (gTasks[taskId].data[3] << 16) + (u16)gTasks[taskId].data[4] - 0x10000;
        gTasks[taskId].data[3] = offset >> 16;
        gTasks[taskId].data[4] = offset;
        REG_BG1VOFS = gTasks[taskId].data[3];
        offset = (gTasks[taskId].data[5] << 16) + (u16)gTasks[taskId].data[6] - 0x18000;
        gTasks[taskId].data[5] = offset >> 16;
        gTasks[taskId].data[6] = offset;
        REG_BG0VOFS = gTasks[taskId].data[5];

        //show Lati@s sprite
        if (gIntroFrameCounter == 880)
        {
            u8 spriteId = CreateSprite(&gSpriteTemplate_840AFF0, 200, 160, 10);

            gSprites[spriteId].invisible = TRUE;
        }
    }
    else
    {
        //fade to white
        if (gIntroFrameCounter > 1007)
        {
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB_WHITEALPHA);
            gTasks[taskId].func = Task_Scene1_End;
        }
    }
}

static void Task_Scene1_End(u8 taskId)
{
    if (gIntroFrameCounter > 1026)
        gTasks[taskId].func = Task_Scene2_Load;
}

static void Task_Scene2_Load(u8 taskId)
{
    IntroResetGpuRegs();
    SetVBlankCallback(NULL);
    ResetSpriteData();
    FreeAllSpritePalettes();
    gIntroCredits_MovingSceneryVBase = 0;
    gIntroCredits_MovingSceneryVOffset = 0;
#ifdef SAPPHIRE
    LoadIntroPart2Graphics(0);
#else
    LoadIntroPart2Graphics(1);
#endif
    gTasks[taskId].func = Task_Scene2_CreateSprites;
}

static void Task_Scene2_CreateSprites(u8 taskId)
{
    u8 spriteId;

    if (sIntroCharacterGender == 0)
        LoadCompressedSpriteSheet(gSpriteSheet_CreditsBrendan);
    else
        LoadCompressedSpriteSheet(gSpriteSheet_CreditsMay);
    LoadCompressedSpriteSheet(gSpriteSheet_IntroBicycle);
#ifdef SAPPHIRE
    LoadCompressedSpriteSheet(gIntro2LatiasSpriteSheet);
#else
    LoadCompressedSpriteSheet(gIntro2LatiosSpriteSheet);
#endif
    LoadSpritePalettes(gSpritePalettes_Credits);
    if (sIntroCharacterGender == 0)
        spriteId = CreateIntroBrendanSprite(0x110, 100);
    else
        spriteId = CreateIntroMaySprite(0x110, 100);
    gSprites[spriteId].callback = SpriteCB_PlayerOnBicycle;
    gSprites[spriteId].anims = sAnims_PlayerBicycle;
    gTasks[taskId].data[1] = spriteId;
#ifdef SAPPHIRE
    spriteId = intro_create_latias_sprite(-0x40, 0x3C);
#else
    spriteId = intro_create_latios_sprite(-0x40, 0x3C);
#endif
    gSprites[spriteId].callback = sub_813D880;
    gTasks[taskId].data[2] = spriteId;
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB_WHITEALPHA);
    SetVBlankCallback(VBlankCB_Intro);
#ifdef SAPPHIRE
    gTasks[taskId].data[0] = CreateBicycleBgAnimationTask(0, 0x4000, 0x40, 0x10);
    SetIntroPart2BgCnt(0);
#else
    gTasks[taskId].data[0] = CreateBicycleBgAnimationTask(1, 0x4000, 0x400, 0x10);
    SetIntroPart2BgCnt(1);
#endif
    gTasks[taskId].func = Task_Scene2_BikeRide;
}

static void Task_Scene2_BikeRide(u8 taskId)
{
    s16 a;
    u16 offset;

    if (gIntroFrameCounter > 1823)
    {
        BeginNormalPaletteFade(0xFFFFFFFF, 16, 0, 16, RGB_WHITEALPHA);
        gTasks[taskId].func = Task_Scene2_End;
    }
    if (gIntroFrameCounter == 1109)
        gSprites[gTasks[taskId].data[1]].data[0] = 1;
    if (gIntroFrameCounter == 1214)
        gSprites[gTasks[taskId].data[1]].data[0] = 0;
    if (gIntroFrameCounter == 1394)
        gSprites[gTasks[taskId].data[2]].data[0] = 1;
    if (gIntroFrameCounter == 1398)
        gSprites[gTasks[taskId].data[1]].data[0] = 2;
    if (gIntroFrameCounter == 1586)
        gSprites[gTasks[taskId].data[1]].data[0] = 3;
    if (gIntroFrameCounter == 1727)
        gSprites[gTasks[taskId].data[1]].data[0] = 4;

    //TODO: Clean this up
    a = (((u16)gTasks[taskId].data[3] << 16) >> 18) & 0x7F;
    offset = Sin(a, 48);
    gIntroCredits_MovingSceneryVOffset = offset;
    if (gTasks[taskId].data[3] < 512)
        gTasks[taskId].data[3]++;
#ifdef SAPPHIRE
    CycleSceneryPalette(0);
#else
    CycleSceneryPalette(1);
#endif
}

static void Task_Scene2_End(u8 taskId)
{
    if (gIntroFrameCounter > 2068)
    {
        DestroyTask(gTasks[taskId].data[0]);
        gTasks[taskId].func = Task_Scene3_Load;
    }
}

static void Task_Scene3_Load(u8 taskId)
{
    IntroResetGpuRegs();
    LZ77UnCompVram(sIntroPokeball_Gfx, (void *)VRAM);
    LZ77UnCompVram(sIntroPokeball_Tilemap, (void *)(VRAM + 0x4000));
    LoadPalette(sIntroPokeball_Pal, 0, 0x200);
    gTasks[taskId].data[0] = 0;
    gTasks[taskId].data[1] = 0;
    gTasks[taskId].data[2] = 0;
    gTasks[taskId].data[3] = 0;
    PanFadeAndZoomScreen(0x78, 0x50, 0, 0);
    ResetSpriteData();
    FreeAllSpritePalettes();
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB_WHITEALPHA);
    REG_BG2CNT = BGCNT_PRIORITY(3) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(8) | BGCNT_256COLOR | BGCNT_AFF256x256;
    REG_DISPCNT = DISPCNT_MODE_1 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
    gTasks[taskId].func = Task_Scene3_SpinPokeball;
    gIntroFrameCounter = 0;
    m4aSongNumStart(0x1BA);
}

static void Task_Scene3_SpinPokeball(u8 taskId)
{
    gTasks[taskId].data[0] += 0x400;
    if (gTasks[taskId].data[1] <= 0x6BF)
    {
        gTasks[taskId].data[1] += gTasks[taskId].data[2];
        gTasks[taskId].data[2]++;
    }
    else
    {
        gTasks[taskId].func = Task_IntroWaitToSetupPart3DoubleFight;
    }
    PanFadeAndZoomScreen(0x78, 0x50, 0x10000 / gTasks[taskId].data[1], gTasks[taskId].data[0]);
    if (gIntroFrameCounter == 44)
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB_WHITEALPHA);
}

static void Task_IntroWaitToSetupPart3DoubleFight(u8 taskId)
{
    if (gIntroFrameCounter > 59)
        gTasks[taskId].func = Task_IntroLoadPart3Streaks;
}

static void Task_IntroLoadPart3Streaks(u8 taskId)
{
    u16 i;

    IntroResetGpuRegs();
    for (i = 0; i < 32; i++)
    {
        ewram0arr[0][i] = 0;
        ewram0arr[1][i] = 17;
        ewram0arr[2][i] = 34;
    }
    DmaCopy16Defvars(3, ewram0arr, (void *)(VRAM + 0x0), 0x60);
    for (i = 0; i < 0x280; i++)
        ((u16 *)(VRAM + 0x3000))[i] = 0xF001;
    for (i = 0; i < 0x80; i++)
        ((u16 *)(VRAM + 0x3800))[i] = 0xF002;
    for (i = 0; i < 0x180; i++)
        ((u16 *)(VRAM + 0x3900))[i] = 0xF000;
    for (i = 0; i < 0x80; i++)
        ((u16 *)(VRAM + 0x3C00))[i] = 0xF002;
    gPlttBufferUnfaded[0xF0] = RGB_WHITE;
    gPlttBufferFaded[0xF0] = RGB_WHITE;
    sub_813D084(1);
    gPlttBufferUnfaded[0xF2] = RGB_BLACK;
    gPlttBufferFaded[0xF2] = RGB_BLACK;
    LZ77UnCompVram(sIntroStreaks_Gfx, (void *)(VRAM + 0x4000));
    LZ77UnCompVram(sIntroStreaks_Tilemap, (void *)(VRAM + 0x7000));
    LoadPalette(sIntroStreaks_Pal, 0, 0x20);
    ResetSpriteData();
    FreeAllSpritePalettes();
    gReservedSpritePaletteCount = 8;
    LoadCompressedSpriteSheet(&gIntro3PokeballGfx_Table[0]);
    LoadCompressedSpriteSheet(&gIntro3MiscGfx_Table[0]);
    LoadCompressedSpritePalette(&gInterfacePokeballPal_Table[0]);
    LoadSpritePalettes(gIntro3MiscPal_Table);
    gTasks[taskId].func = task_intro_14;
}

static void task_intro_14(u8 taskId)
{
    REG_WIN0H = 0xF0;
    REG_WIN0V = 0xA0;
    REG_WININ = 0x1C;
    REG_WINOUT = 0x1D;
    REG_BG3CNT = BGCNT_PRIORITY(3)
               | BGCNT_CHARBASE(0)
               | BGCNT_SCREENBASE(6)
               | BGCNT_16COLOR
               | BGCNT_TXT256x256;
    REG_BG0CNT = BGCNT_PRIORITY(0)
               | BGCNT_CHARBASE(0)
               | BGCNT_SCREENBASE(7)
               | BGCNT_16COLOR
               | BGCNT_TXT256x256;
    REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG3_ON | DISPCNT_OBJ_ON | DISPCNT_WIN0_ON;
    gTasks[taskId].data[15] = CreateTask(task_intro_20, 0);
    gTasks[gTasks[taskId].data[15]].data[0] = 0;
    gTasks[taskId].data[0] = 0;
    gTasks[taskId].func = Task_Scene3_NarrowWindow;
}

static void Task_Scene3_NarrowWindow(u8 taskId)
{
    u16 foo = gTasks[taskId].data[0];

    if (gTasks[taskId].data[0] != 32)
    {
        u32 bar; //needed to match for some reason

        gTasks[taskId].data[0] += 4;
        REG_WIN0V = (gTasks[taskId].data[0] * 256) - (bar = foo - 0x9C);
    }
    else
    {
        REG_WIN0V = 0x2080;
        gTasks[taskId].func = Task_Scene3_EndNarrowWindow;
    }
}

static void Task_Scene3_EndNarrowWindow(u8 taskId)
{
    gTasks[taskId].func = task_intro_17;
}

static void task_intro_17(u8 taskId)
{
    gUnknown_0203931A = 0;
    gTasks[taskId].func = Task_IntroPokemonBattle;
}

static void Task_IntroPokemonBattle(u8 taskId)
{
    u8 spriteId;

    if (gIntroFrameCounter == 80)
    {
        spriteId = sub_813CE88(SPECIES_SHARPEDO, 0xF0, 0xA0, 5, 1);
        gSprites[spriteId].callback = sub_813DB9C;
        gSprites[spriteId].data[1] = 1;
        gSprites[spriteId].data[2] = 0;
    }
    if (gIntroFrameCounter == 152)
    {
        spriteId = sub_813CE88(SPECIES_DUSKULL, 0, 0xA0, 4, 1);
        gSprites[spriteId].callback = sub_813DB9C;
        gSprites[spriteId].data[1] = 2;
        gSprites[spriteId].data[2] = 1;
    }
    if (gIntroFrameCounter == 219)
    {
        sub_813D084(0);
        spriteId = sub_813CFA8(sIntroCharacterGender, 0x110, 0x60, 6);
        gSprites[spriteId].callback = sub_813DE70;
        gTasks[taskId].data[1] = spriteId;
    }
    if (gIntroFrameCounter == 304)
    {
        gTasks[gTasks[taskId].data[15]].data[0] = 4;
        gSprites[gTasks[taskId].data[1]].data[0] = 2;
    }
    if (gIntroFrameCounter == 384)
    {
        gTasks[gTasks[taskId].data[15]].data[0] = 0;
        gSprites[gTasks[taskId].data[1]].data[0] = 4;
    }
    if (gIntroFrameCounter == 400)
    {
        BeginNormalPaletteFade(0xFF0000, 0, 16, 0, RGB(31, 23, 31));
    }
    if (gIntroFrameCounter == 432)
    {
        gSprites[gTasks[taskId].data[1]].data[0] = 5;
    }
    if (gIntroFrameCounter == 462)
    {
        gSprites[gTasks[taskId].data[1]].data[0] = 6;
        gTasks[gTasks[taskId].data[15]].data[0] = 2;
    }
    if (gIntroFrameCounter == 463)
    {
        sub_813D084(1);
        spriteId = sub_813CE88(SPECIES_SHARPEDO, 0xD0, 8, 5, 1);
        gSprites[spriteId].callback = sub_813E10C;
        gTasks[taskId].data[2] = spriteId;
        sub_813E7C0(spriteId);
    }
    if (gIntroFrameCounter == 539)
    {
        spriteId = sub_813CE88(SPECIES_DUSKULL, 0xF8, 0x10, 4, 1);
        gSprites[spriteId].callback = sub_813E10C;
        gTasks[taskId].data[3] = spriteId;
        sub_813E930(spriteId);
    }
    if (gIntroFrameCounter == 623)
    {
        gSprites[gTasks[taskId].data[2]].data[0] = 2;
        gSprites[gTasks[taskId].data[3]].data[0] = 2;
        gTasks[gTasks[taskId].data[15]].data[0] = 3;
    }
    if (gIntroFrameCounter == 624)
    {
        sub_813D084(0);
        spriteId = sub_813CE88(SPECIES_MUDKIP, 0x20, 0x98, 0, 0);
        gSprites[spriteId].callback = sub_813E210;
        gTasks[taskId].data[4] = spriteId;
        InitIntroMudkipAttackAnim(spriteId);
    }
    if (gIntroFrameCounter == 700)
    {
        spriteId = sub_813CE88(SPECIES_TORCHIC, -8, 0x90, 1, 0);
        gSprites[spriteId].callback = sub_813E210;
        gTasks[taskId].data[5] = spriteId;
        InitIntroTorchicAttackAnim(spriteId);
    }
    if (gIntroFrameCounter == 776)
    {
        gUnknown_0203931A = 1;
        gSprites[gTasks[taskId].data[4]].data[0] = 2;
        gSprites[gTasks[taskId].data[5]].data[0] = 2;
        gTasks[gTasks[taskId].data[15]].data[0] = 0;
    }
    if (gIntroFrameCounter == 781)
    {
        sub_813D084(2);
        gSprites[gTasks[taskId].data[2]].data[0] = 3;
        gSprites[gTasks[taskId].data[3]].data[0] = 3;
        gSprites[gTasks[taskId].data[4]].data[0] = 3;
        gSprites[gTasks[taskId].data[5]].data[0] = 3;
        spriteId = CreateSprite(&gSpriteTemplate_840B1F4, 0x78, 0x50, 15);
        gSprites[spriteId].invisible = TRUE;
    }
    if (gIntroFrameCounter == 800)
        PlaySE(SE_INTRO_BLAST);
    if (gIntroFrameCounter == 850)
        BeginNormalPaletteFade(0xFFFFFFFF, 4, 0, 16, RGB_WHITEALPHA);
    if (gIntroFrameCounter == 946)
        gTasks[taskId].func = Task_EndIntroMovie;
}

static void Task_EndIntroMovie(u8 taskId)
{
    DestroyTask(taskId);
    SetMainCallback2(MainCB2_EndIntro);
}

static void task_intro_20(u8 taskId)
{
#define BG2_FLAGS (BGCNT_PRIORITY(3) | BGCNT_CHARBASE(1) | BGCNT_SCREENBASE(14) | BGCNT_16COLOR | BGCNT_TXT256x256)
#define DISPCNT_FLAGS (DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG2_ON | DISPCNT_BG3_ON | DISPCNT_OBJ_ON | DISPCNT_WIN0_ON)

    gTasks[taskId].data[15]++;
    switch (gTasks[taskId].data[0])
    {
    case 0:
        REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG3_ON | DISPCNT_OBJ_ON | DISPCNT_WIN0_ON;
        REG_BG2CNT = 0;
        gTasks[taskId].data[0] = 0xFF;
        break;
    case 2:
        BeginNormalPaletteFade(0x1, 0, 16, 0, RGB_WHITEALPHA);
        REG_BG2CNT = BG2_FLAGS;
        REG_DISPCNT = DISPCNT_FLAGS;
        gTasks[taskId].data[1] = 0;
        gTasks[taskId].data[2] = 0;
        gTasks[taskId].data[0] = 20;
        //fall through
    case 20:
        REG_BG2VOFS = gTasks[taskId].data[1];
        REG_BG2HOFS = gTasks[taskId].data[2];
        gTasks[taskId].data[1] += 6;
        gTasks[taskId].data[2] -= 8;
        break;
    case 3:
        BeginNormalPaletteFade(0x1, 0, 16, 0, RGB_WHITEALPHA);
        REG_BG2CNT = BG2_FLAGS;
        REG_DISPCNT = DISPCNT_FLAGS;
        gTasks[taskId].data[1] = 0;
        gTasks[taskId].data[2] = 0;
        gTasks[taskId].data[0] = 0x1E;
        //fall through
    case 0x1E:
        REG_BG2VOFS = gTasks[taskId].data[1];
        REG_BG2HOFS = gTasks[taskId].data[2];
        gTasks[taskId].data[1] -= 6;
        gTasks[taskId].data[2] += 8;
        break;
    case 4:
        BeginNormalPaletteFade(0x1, 5, 0, 16, RGB(23, 31, 13));
        REG_BG2CNT = BG2_FLAGS;
        REG_DISPCNT = DISPCNT_FLAGS;
        gTasks[taskId].data[1] = 0;
        gTasks[taskId].data[2] = 0;
        gTasks[taskId].data[3] = 8;
        gTasks[taskId].data[0] = 0x28;
        //fall through
    case 0x28:
        REG_BG2VOFS = gTasks[taskId].data[1];
        REG_BG2HOFS = gTasks[taskId].data[2];
        gTasks[taskId].data[1] -= gTasks[taskId].data[3];
        gTasks[taskId].data[2] += gTasks[taskId].data[3];
        if (!(gTasks[taskId].data[15] & 7) && gTasks[taskId].data[3] != 0)
            gTasks[taskId].data[3]--;
        break;
    case 0xFF:  //needed to prevent jump table optimization
        break;
    }

#undef BG2_FLAGS
#undef DISPCNT_FLAGS
}

static void IntroResetGpuRegs(void)
{
    REG_DISPCNT = 0;
    REG_BG3HOFS = 0;
    REG_BG3VOFS = 0;
    REG_BG2HOFS = 0;
    REG_BG2VOFS = 0;
    REG_BG1HOFS = 0;
    REG_BG1VOFS = 0;
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;
    REG_BLDCNT = 0;
    REG_BLDALPHA = 0;
    REG_BLDY = 0;
}

static void sub_813CCE8(u8 taskId)
{
    switch (gTasks[taskId].data[0])
    {
    default:
    case 0:
        REG_BLDCNT = 0x3F50;
        REG_BLDALPHA = 0x1000;
        REG_BLDY = 0;
        gTasks[taskId].data[1] = 0x40;
        gTasks[taskId].data[0] = 1;
        break;
    case 1:
        if (gTasks[taskId].data[1] != 0)
        {
            u8 tmp;

            gTasks[taskId].data[1]--;
            tmp = gTasks[taskId].data[1] / 2;
            REG_BLDALPHA = gTitleScreenAlphaBlend[tmp];
        }
        else
        {
            REG_BLDALPHA = gTitleScreenAlphaBlend[0];
            gTasks[taskId].data[1] = 0x80;
            gTasks[taskId].data[0]++;
        }
        break;
    case 2:
        if (gTasks[taskId].data[1] != 0)
        {
            //tail merge at _0813CE0E
            gTasks[taskId].data[1]--;
        }
        else
        {
            gTasks[taskId].data[1] = 0;  //redundant?
            gTasks[taskId].data[0]++;
        }
        break;
    case 3:
        if (gTasks[taskId].data[1] <= 0x3D)
        {
            u8 tmp;

            gTasks[taskId].data[1]++;
            tmp = gTasks[taskId].data[1] / 2;
            REG_BLDALPHA = gTitleScreenAlphaBlend[tmp];
        }
        else
        {
            //_0813CDE0
            REG_BLDALPHA = gTitleScreenAlphaBlend[0x1F];
            gTasks[taskId].data[1] = 0x10;
            gTasks[taskId].data[0]++;
        }
        break;
    case 4:
        if (gTasks[taskId].data[1] != 0)
        {
            gTasks[taskId].data[1]--;
        }
        else
        {
            REG_BLDCNT = 0;
            REG_BLDALPHA = 0;
            REG_BLDY = 0;
            DestroyTask(taskId);
        }
        break;
    }
}

void PanFadeAndZoomScreen(u16 screenX, u16 screenY, u16 zoom, u16 alpha)
{
    struct BgAffineSrcData src;
    struct BgAffineDstData dest;

    src.texX = 0x8000;
    src.texY = 0x8000;
    src.scrX = screenX;
    src.scrY = screenY;
    src.sx = zoom;
    src.sy = zoom;
    src.alpha = alpha;
    BgAffineSet(&src, &dest, 1);
    REG_BG2PA = dest.pa;
    REG_BG2PB = dest.pb;
    REG_BG2PC = dest.pc;
    REG_BG2PD = dest.pd;
    REG_BG2X = dest.dx;
    REG_BG2Y = dest.dy;
}

static u16 sub_813CE88(u16 species, s16 x, s16 y, u16 d, u8 front)
{
    const u8 *lzPaletteData;
    u8 spriteId;

    if (front)
        LoadSpecialPokePic(&gMonFrontPicTable[species], gMonFrontPicCoords[species].size, gMonFrontPicCoords[species].y_offset, (void *)EWRAM, gUnknown_0840B5A0[d], species, 0, 1);
    else
        LoadSpecialPokePic(&gMonBackPicTable[species], gMonBackPicCoords[species].size, gMonBackPicCoords[species].y_offset, (void *)EWRAM, gUnknown_0840B5A0[d], species, 0, 0);
    lzPaletteData = GetMonSpritePalFromSpeciesAndPersonality(species, 0, 0xFFFF);
    LoadCompressedPalette(lzPaletteData, 0x100 + d * 0x10, 0x20);
    sub_8143648(d, d);
    spriteId = CreateSprite(&gMultiuseSpriteTemplate, x, y, (d + 1) * 4);
    gSprites[spriteId].oam.paletteNum = d;
    gSprites[spriteId].oam.priority = 1;
    return spriteId;
}

static u8 sub_813CFA8(u16 a, u16 b, u16 c, u16 d)
{
    u8 spriteId;

    DecompressPicFromTable_2(&gTrainerBackPicTable[a], gTrainerBackPicCoords[a].size, gTrainerBackPicCoords[a].y_offset, (void *)EWRAM, gUnknown_0840B5A0[d], a);
    LoadCompressedPalette(gTrainerBackPicPaletteTable[a].data, 0x100 + d * 0x10, 0x20);
    sub_8143680(d, d);
    gMultiuseSpriteTemplate.anims = gUnknown_0840B064;
    spriteId = CreateSprite(&gMultiuseSpriteTemplate, b, c, 1);
    gSprites[spriteId].oam.paletteNum = d;
    gSprites[spriteId].oam.priority = 1;
    return spriteId;
}

static void sub_813D084(u8 a)
{
    u16 color;

    switch (a)
    {
    default:
    case 0:
        color = RGB(22, 31, 15);
        break;
    case 1:
        color = RGB(31, 14, 12);
        break;
    case 2:
        color = RGB(12, 12, 20);
        break;
    }
    gPlttBufferUnfaded[241] = color;
    gPlttBufferFaded[241] = color;
}

static void SpriteCB_WaterDrop_Ripple(struct Sprite *sprite)
{
    u8 palNum;

    if (sprite->data[2] >= 192)
    {
        if (sprite->data[3] != 0)
        {
            sprite->data[3]--;
        }
        else
        {
            sprite->invisible = FALSE;
            SetOamMatrix(sprite->data[1], sprite->data[2], 0, 0, sprite->data[2]);
            sprite->data[2] = (sprite->data[2] * 95) / 100;
            palNum = (sprite->data[2] - 192) / 128 + 9;
            if (palNum > 15)
                palNum = 15;
            sprite->oam.paletteNum = palNum;
        }
    }
    else
    {
        DestroySprite(sprite);
    }
}

static void SpriteCB_WaterDropHalf(struct Sprite *sprite)
{
    if (gSprites[sprite->data[7]].data[7] != 0)
    {
        sprite->invisible = TRUE;
        sprite->x += sprite->x2;
        sprite->y += sprite->y2;
        StartSpriteAnim(sprite, 3);
        sprite->data[2] = 1024;
        sprite->data[3] = 8 * (sprite->data[1] & 3);
        sprite->callback = SpriteCB_WaterDrop_Ripple;
        sprite->oam.shape = 1;
        sprite->oam.size = 3;
        CalcCenterToCornerVec(sprite, 1, 3, 2);
    }
    else
    {
        sprite->x2 = gSprites[sprite->data[7]].x2;
        sprite->y2 = gSprites[sprite->data[7]].y2;
        sprite->x = gSprites[sprite->data[7]].x;
        sprite->y = gSprites[sprite->data[7]].y;
    }
}

static void SpriteCB_WaterDrop(struct Sprite *sprite)
{
    if (sprite->data[0] != 0)
        sprite->callback = SpriteCB_WaterDrop_Slide;
}

static void SpriteCB_WaterDrop_Slide(struct Sprite *sprite)
{
    if (sprite->x <= 116)
    {
        sprite->y += sprite->y2;
        sprite->y2 = 0;
        sprite->x += 4;
        sprite->x2 = -4;
        sprite->data[4] = 128;
        sprite->callback = SpriteCB_WaterDrop_ReachLeafEnd;
    }
    else
    {
        u16 data2;
        u16 data3;
        u16 data4;
        s16 sin1;
        s16 sin2;
        s16 sin3;
        s16 sin4;
        s16 var1;
        s16 var2;
        s16 var3;
        s16 var4;
        s16 temp;

        data4 = sprite->data[4];
        sin1 = gSineTable[(u8)data4];
        sin2 = gSineTable[(u8)(data4 + 64)];
        sprite->data[4] += 2;
        sprite->y2 = sin1 / 32;
        sprite->x--;
        if (sprite->x & 1)
            sprite->y++;
        temp = -sin2 / 16;
        data2 = sprite->data[2];
        data3 = sprite->data[3];
        sin3 = gSineTable[(u8)(temp - 16)];
        sin4 = gSineTable[(u8)(temp + 48)];
        var1 = sin4 * data2 / 256;
        var2 = -sin3 * data3 / 256;
        var3 = sin3 * data2 / 256;
        var4 = sin4 * data3 / 256;
        SetOamMatrix(sprite->data[1], data2, 0, 0, data3);
        SetOamMatrix(sprite->data[1] + 1, var1, var3, var2, var4);
        SetOamMatrix(sprite->data[1] + 2, var1, var3, var2 * 2, var4 * 2);
    }
}

static void SpriteCB_WaterDrop_ReachLeafEnd(struct Sprite *sprite)
{
    SetOamMatrix(sprite->data[1], sprite->data[6] + 64, 0, 0, sprite->data[6] + 64);
    SetOamMatrix(sprite->data[1] + 1, sprite->data[6] + 64, 0, 0, sprite->data[6] + 64);
    SetOamMatrix(sprite->data[1] + 2, sprite->data[6] + 64, 0, 0, sprite->data[6] + 64);
    if (sprite->data[4] != 64)
    {
        u16 sinIdx;

        sprite->data[4] -= 8;
        sinIdx = sprite->data[4];
        sprite->x2 = gSineTable[(u8)(sinIdx + 64)] / 64;
        sprite->y2 = gSineTable[(u8)sinIdx] / 64;
    }
    else
    {
        sprite->data[4] = 0;
        sprite->callback = SpriteCB_WaterDrop_DangleFromLeaf;
    }
}

static void SpriteCB_WaterDrop_DangleFromLeaf(struct Sprite *sprite)
{
    if (sprite->data[0] != 2)
    {
        s16 r2;

        sprite->data[4] += 8;
        r2 = gSineTable[(u8)sprite->data[4]] / 16 + 64;
        sprite->x2 = gSineTable[(u8)(r2 + 64)] / 64;
        sprite->y2 = gSineTable[(u8)r2] / 64;
    }
    else
    {
        sprite->callback = SpriteCB_WaterDrop_Fall;
    }
}

static void SpriteCB_WaterDrop_Fall(struct Sprite *sprite)
{
    if (sprite->y < sprite->data[5])
    {
        sprite->y += 4;
    }
    else
    {
        sprite->data[7] = 1;
        sprite->invisible = TRUE;
        sprite->x += sprite->x2;
        sprite->y += sprite->y2;
        StartSpriteAnim(sprite, 3);
        sprite->data[2] = 1024;
        sprite->data[3] = 8 * (sprite->data[1] & 3);
        sprite->callback = SpriteCB_WaterDrop_Ripple;
        sprite->oam.shape = 1;
        sprite->oam.size = 3;
        CalcCenterToCornerVec(sprite, 1, 3, 2);
    }
}

//Duplicate function
static void SpriteCB_WaterDropShort(struct Sprite *sprite)
{
    if (sprite->y < sprite->data[5])
    {
        sprite->y += 4;
    }
    else
    {
        sprite->data[7] = 1;
        sprite->invisible = TRUE;
        sprite->x += sprite->x2;
        sprite->y += sprite->y2;
        StartSpriteAnim(sprite, 3);
        sprite->data[2] = 1024;
        sprite->data[3] = 8 * (sprite->data[1] & 3);
        sprite->callback = SpriteCB_WaterDrop_Ripple;
        sprite->oam.shape = 1;
        sprite->oam.size = 3;
        CalcCenterToCornerVec(sprite, 1, 3, 2);
    }
}

static u8 CreateWaterDrop(s16 x, s16 y, u16 c, u16 d, u16 e, u8 fallImmediately)
{
    u8 spriteId;
    u8 oldSpriteId;

    spriteId = CreateSprite(&sSpriteTemplate_WaterDrop, x, y, 0);
    gSprites[spriteId].data[0] = 0;
    gSprites[spriteId].data[7] = 0;
    gSprites[spriteId].data[1] = d;
    gSprites[spriteId].data[2] = c;
    gSprites[spriteId].data[3] = c;
    gSprites[spriteId].data[5] = e;
    gSprites[spriteId].data[6] = c;
    gSprites[spriteId].oam.affineMode = 3;
    gSprites[spriteId].oam.matrixNum = d;
    CalcCenterToCornerVec(&gSprites[spriteId], 0, 2, 2);
    StartSpriteAnim(&gSprites[spriteId], 2);
    if (!fallImmediately)
        gSprites[spriteId].callback = SpriteCB_WaterDrop;
    else
        gSprites[spriteId].callback = SpriteCB_WaterDropShort;
    oldSpriteId = spriteId;

    spriteId = CreateSprite(&sSpriteTemplate_WaterDrop, x, y, 0);
    gSprites[spriteId].data[7] = oldSpriteId;
    gSprites[spriteId].data[1] = d + 1;
    gSprites[spriteId].oam.affineMode = 3;
    gSprites[spriteId].oam.matrixNum = d + 1;
    CalcCenterToCornerVec(&gSprites[spriteId], 0, 2, 2);
    gSprites[spriteId].callback = SpriteCB_WaterDropHalf;

    spriteId = CreateSprite(&sSpriteTemplate_WaterDrop, x, y, 0);
    gSprites[spriteId].data[7] = oldSpriteId;
    gSprites[spriteId].data[1] = d + 2;
    StartSpriteAnim(&gSprites[spriteId], 1);
    gSprites[spriteId].oam.affineMode = 3;
    gSprites[spriteId].oam.matrixNum = d + 2;
    CalcCenterToCornerVec(&gSprites[spriteId], 0, 2, 2);
    gSprites[spriteId].callback = SpriteCB_WaterDropHalf;

    SetOamMatrix(d, c + 32, 0, 0, c + 32);
    SetOamMatrix(d + 1, c + 32, 0, 0, c + 32);
    SetOamMatrix(d + 2, c + 32, 0, 0, 2 * (c + 32));

    return oldSpriteId;
}

static void SpriteCB_PlayerOnBicycle(struct Sprite *sprite)
{
    switch (sprite->data[0])
    {
    case 0:
        StartSpriteAnimIfDifferent(sprite, 0);
        sprite->x--;
        break;
    case 1:
        StartSpriteAnimIfDifferent(sprite, 0);
        if (gIntroFrameCounter & 7)
            return;
        sprite->x++;
        break;
    case 2:
        StartSpriteAnimIfDifferent(sprite, 2);
        if (sprite->x <= 120 || (gIntroFrameCounter & 7))
            sprite->x++;
        break;
    case 3:
        StartSpriteAnimIfDifferent(sprite, 3);
        break;
    case 4:
        StartSpriteAnimIfDifferent(sprite, 0);
        if (sprite->x > -32)
            sprite->x -= 2;
        break;
    }
    if (gIntroFrameCounter & 7)
        return;
    if (sprite->y2 != 0)
    {
        sprite->y2 = 0;
    }
    else
    {
        switch (Random() & 3)
        {
        case 0:
            sprite->y2 = -1;
            break;
        case 1:
            sprite->y2 = 1;
            break;
        case 2:
        case 3:
            sprite->y2 = 0;
            break;
        }
    }
}

static void sub_813D880(struct Sprite *sprite)
{
    switch (sprite->data[0])
    {
    case 0:
        break;
    case 1:
        if (sprite->x2 + sprite->x < 304)
            sprite->x2 += 8;
        else
            sprite->data[0] = 2;
        break;
    case 2:
        if (sprite->x2 + sprite->x > 120)
            sprite->x2 -= 1;
        else
            sprite->data[0] = 3;
        break;
    case 3:
        if (sprite->x2 > 0)
            sprite->x2 -= 2;
        break;
    }
    sprite->y2 = Sin((u8)sprite->data[1], 8) - gIntroCredits_MovingSceneryVOffset;
    sprite->data[1] += 4;
}

static void sub_813D908(struct Sprite *sprite)
{
    if (gTasks[sprite->data[0]].data[0] == 0)
    {
        sprite->invisible = TRUE;
    }
    else if (gTasks[sprite->data[0]].data[0] != 4)
    {
        sprite->invisible = FALSE;
    }
    else
    {
        DestroySprite(sprite);
    }
}

static u8 CreateGameFreakLogoSprites(s16 x, s16 y, u8 c)
{
    u8 spriteId;
    u16 i;

    for (i = 0; i < 9; i++)
    {
        spriteId = CreateSprite(&sSpriteTemplate_GameFreakLetter, sGameFreakLetterData[i][1] + x, y - 4, 0);
        gSprites[spriteId].data[0] = c;
        StartSpriteAnim(&gSprites[spriteId], sGameFreakLetterData[i][0]);
    }
    for (i = 0; i < 8; i++)
    {
        spriteId = CreateSprite(&sSpriteTemplate_PresentsLetter, sPresentsLetterData[i][1] + x, y + 12, 0);
        gSprites[spriteId].data[0] = c;
        StartSpriteAnim(&gSprites[spriteId], sPresentsLetterData[i][0]);
    }
    spriteId = CreateSprite(&sSpriteTemplate_GameFreakLogo, 120, y - 4, 0);
    gSprites[spriteId].data[0] = c;

    return spriteId;
}

static void sub_813DA64(struct Sprite *sprite)
{
    sprite->data[7]++;

    switch (sprite->data[0])
    {
    case 0:
    default:
        sprite->oam.affineMode = 3;
        sprite->oam.matrixNum = 1;
        CalcCenterToCornerVec(sprite, 1, 3, 3);
        sprite->invisible = FALSE;
        sprite->data[0] = 1;
        sprite->data[1] = 128;
        sprite->data[2] = -24;
        sprite->data[3] = 0;
        break;
    case 1:
    {
        s16 sin;
        s16 cos;
        s16 a;
        s16 b;
        s16 c;
        s16 d;

        if (sprite->data[3] < 0x50)
        {
            sprite->y2 = -Sin((u8)sprite->data[3], 0x78);
            sprite->x2 = -Sin((u8)sprite->data[3], 0x8C);
            if (sprite->data[3] > 64)
                sprite->oam.priority = 3;
        }
        sin = gSineTable[(u8)sprite->data[2]];
        cos = gSineTable[(u8)(sprite->data[2] + 64)];
        d =  cos * sprite->data[1] / 256;
        c = -sin * sprite->data[1] / 256;
        b =  sin * sprite->data[1] / 256;
        a  = cos * sprite->data[1] / 256;

        SetOamMatrix(1, a, b, c, d);

        if (sprite->data[1] < 0x100)
            sprite->data[1] += 8;
        else
            sprite->data[1] += 32;
        if (sprite->data[2] < 0x18)
            sprite->data[2] += 1;
        if (sprite->data[3] < 64)
            sprite->data[3] += 2;
        else if (!(sprite->data[7] & 3))
            sprite->data[3] += 1;
        break;
    }
    }
}

static void sub_813DB9C(struct Sprite *sprite)
{
    switch (sprite->data[0])
    {
    case 0:
    default:
        if (sprite->data[2] != 0)
            sprite->hFlip = TRUE;
        else
            sprite->hFlip = FALSE;
        sprite->data[0] = 1;
        //fall through
    case 1:
        if (sprite->y > 96)
        {
            sprite->y -= 4;
            if (sprite->data[2] != 0)
                sprite->x += 2;
            else
                sprite->x -= 2;
        }
        else
        {
            sprite->data[0]++;
            sprite->data[3] = 8;
        }
        break;
    case 2:
        if (sprite->data[3] != 0)
        {
            sprite->data[3]--;
        }
        else
        {
            sprite->data[0]++;
            sprite->data[3] = 0;  //redundant?
        }
        break;
    case 3:
        sprite->oam.affineMode = 3;
        sprite->oam.matrixNum = sprite->data[1];
        CalcCenterToCornerVec(sprite, 0, 3, 3);
        if (sprite->data[2] != 0)
            SetOamMatrix(sprite->data[1], -256, 0, 0, 256);
        else
            SetOamMatrix(sprite->data[1], 256, 0, 0, 256);
        sprite->data[0]++;
        sprite->data[4] = 0;
        break;
    case 4:
        sprite->data[4]++;
        if (sprite->y + sprite->y2 > -32
         && sprite->x + sprite->x2 > -64)
        {
            u16 r2;

            sprite->y2 = -(sprite->data[4] * sprite->data[4]) / 8;
            if (sprite->data[2] != 0)
                sprite->x2 += sprite->data[4];
            else
                sprite->x2 -= sprite->data[4];
            if (sprite->data[3] < 128)
                sprite->data[3] += 8;
            r2 = 256 - sprite->data[3];
            if (sprite->data[2] != 0)
                SetOamMatrix(sprite->data[1], -r2, 0, 0, r2);
            else
                SetOamMatrix(sprite->data[1], r2, 0, 0, r2);
        }
        else
        {
            DestroySprite(sprite);
        }
    }
}

static void sub_813DD58(struct Sprite *sprite)
{
    switch (sprite->data[0])
    {
    case 0:
    default:
        sprite->invisible = FALSE;
        sprite->oam.affineMode = 1;
        sprite->oam.matrixNum = sprite->data[1];
        sprite->data[3] = 2048;
        sprite->data[0] = 1;
        //fall through
    case 1:
        if (sprite->data[3] > 256)
        {
            sprite->data[3] -= 128;
            if (sprite->data[2] != 0)
                SetOamMatrix(sprite->data[1], -sprite->data[3], 0, 0, sprite->data[3]);
            else
                SetOamMatrix(sprite->data[1], sprite->data[3], 0, 0, sprite->data[3]);
        }
        else
        {
            if (sprite->data[2] != 0)
                SetOamMatrix(sprite->data[1], -256, 0, 0, 256);
            else
                SetOamMatrix(sprite->data[1], 256, 0, 0, 256);
            sprite->data[0]++;
        }
        break;
    case 2:
        break;
    case 3:
        sprite->data[4]++;
        sprite->y2 = sprite->data[4] * sprite->data[4] / 32;
        if (sprite->data[2] != 0)
            sprite->x2 = sprite->data[4] / 4;
        else
            sprite->x2 = -(sprite->data[4] / 4);
        break;
    }
}

static void sub_813DE70(struct Sprite *sprite)
{
    switch (sprite->data[0])
    {
    case 0:
    default:
        if (sprite->x > 40)
        {
            sprite->x -= 4;
        }
        else
        {
            StartSpriteAnim(sprite, 1);
            sprite->data[6] = CreateSprite(&gSpriteTemplate_840B084, 16, 104, 100);
            sprite->data[7] = CreateSprite(&gSpriteTemplate_840B084, 12, 106, 101);
            sprite->data[0] = 1;
        }
        break;
    case 1:
        break;
    case 2:
        StartSpriteAnim(sprite, 2);
        gSprites[sprite->data[6]].data[0] = 1;
        gSprites[sprite->data[7]].data[0] = 2;
        sprite->data[0]++;
        break;
    case 3:
        if (sprite->y > 160)
        {
            sprite->invisible = TRUE;
            sprite->data[0] = 1;
        }
        else
        {
            sprite->y += 2;
            sprite->x--;
        }
        break;
    case 4:
    {
        s16 r4, r5;

        r5 = gSprites[sprite->data[6]].x + gSprites[sprite->data[6]].x2;
        r4 = gSprites[sprite->data[6]].y + gSprites[sprite->data[6]].y2;
        DestroySprite(&gSprites[sprite->data[6]]);
        sprite->data[6] = sub_813CE88(SPECIES_TORCHIC, r5, r4, 2, 1);
        gSprites[sprite->data[6]].callback = sub_813DD58;
        gSprites[sprite->data[6]].invisible = TRUE;
        gSprites[sprite->data[6]].data[1] = 1;
        gSprites[sprite->data[6]].data[2] = 1;
        sub_813E580(r5, r4);

        r5 = gSprites[sprite->data[7]].x + gSprites[sprite->data[7]].x2;
        r4 = gSprites[sprite->data[7]].y + gSprites[sprite->data[7]].y2;
        DestroySprite(&gSprites[sprite->data[7]]);
        sprite->data[7] = sub_813CE88(SPECIES_MUDKIP, r5, r4, 3, 1);
        gSprites[sprite->data[7]].callback = sub_813DD58;
        gSprites[sprite->data[7]].invisible = TRUE;
        gSprites[sprite->data[7]].data[1] = 2;
        gSprites[sprite->data[7]].data[2] = 0;
        sub_813E580(r5, r4);

        BeginNormalPaletteFade(0xFF0000, 0, 16, 16, RGB(31, 23, 31));
        sprite->data[0] = 1;
        break;
    }
    case 5:
        gSprites[sprite->data[6]].data[0] = 3;
        gSprites[sprite->data[7]].data[0] = 3;
        break;
    case 6:
        DestroySprite(&gSprites[sprite->data[6]]);
        DestroySprite(&gSprites[sprite->data[7]]);
        DestroySprite(sprite);
        break;
    }
}

static void sub_813E10C(struct Sprite *sprite)
{
    switch (sprite->data[0])
    {
    case 0:
    default:
        if (sprite->x2 > -56)
        {
            sprite->x2 -= 8;
            sprite->y2 += 6;
        }
        else
        {
            sprite->data[6] = sprite->x;
            sprite->data[7] = sprite->y;
            sprite->x += sprite->x2;
            sprite->y += sprite->y2;
            sprite->x2 = 0;
            sprite->y2 = 0;
            sprite->data[0] = 1;
            sprite->data[1] = 0;
        }
        break;
    case 1:
        if (!(sprite->data[1] & 1))
        {
            if (sprite->data[1] & 2)
            {
                sprite->x2 = -1;
                sprite->y2 = 1;
            }
            else
            {
                sprite->x2 = 0;
                sprite->y2 = 0;
            }
        }
        sprite->data[1]++;
        break;
    case 2:
        sprite->invisible = TRUE;
        sprite->x = sprite->data[6];
        sprite->y = sprite->data[7];
        sprite->x2 = 0;
        sprite->y2 = 0;
        break;
    case 3:
        sprite->invisible = FALSE;
        sprite->data[1]++;
        //fall through
    case 4:
        if (sprite->x2 > -56)
        {
            sprite->x2 -= 4;
            sprite->y2 += 3;
        }
        else
        {
            sprite->x += sprite->x2;
            sprite->y += sprite->y2;
            sprite->x2 = 0;
            sprite->y2 = 0;
            sprite->data[0] = 1;
        }
        break;
    }
}

static void sub_813E210(struct Sprite *sprite)
{
    switch (sprite->data[0])
    {
    case 0:
    default:
        if (sprite->x2 < 56)
        {
            sprite->x2 += 8;
            sprite->y2 -= 6;
        }
        else
        {
            sprite->data[6] = sprite->x;
            sprite->data[7] = sprite->y;
            sprite->x += sprite->x2;
            sprite->y += sprite->y2;
            sprite->x2 = 0;
            sprite->y2 = 0;
            sprite->data[0] = 1;
            sprite->data[1] = 0;
        }
        break;
    case 1:
        if (!(sprite->data[1] & 1))
        {
            if (sprite->data[1] & 2)
            {
                sprite->x2 = 1;
                sprite->y2 = -1;
            }
            else
            {
                sprite->x2 = 0;
                sprite->y2 = 0;
            }
        }
        sprite->data[1]++;
        break;
    case 2:
        sprite->invisible = TRUE;
        sprite->x = sprite->data[6];
        sprite->y = sprite->data[7];
        sprite->x2 = 0;
        sprite->y2 = 0;
        break;
    case 3:
        sprite->invisible = FALSE;
        sprite->data[1]++;
        //fall through
    case 4:
        if (sprite->x2 < 56)
        {
            sprite->x2 += 4;
            sprite->y2 -= 3;
        }
        else
        {
            sprite->x += sprite->x2;
            sprite->y += sprite->y2;
            sprite->x2 = 0;
            sprite->y2 = 0;
            sprite->data[0] = 1;
        }
        break;
    }
}

static void sub_813E30C(struct Sprite *sprite)
{
    u16 r4, r1;

    sprite->data[7]++;
    switch (sprite->data[0])
    {
    case 0:
    default:
        break;
    case 1:
        sprite->oam.affineMode = 1;
        sprite->oam.matrixNum = 1;
        sprite->data[0] = 10;
        sprite->data[4] = 36;
        //fall through
    case 10:
        if (sprite->x <= 144)
        {
            sprite->x += 4;
            sprite->y -= 1;
            sprite->y2 = -Sin((u8)sprite->data[2], 24);
            sprite->data[2] += 4;
        }
        sprite->data[3] -= sprite->data[4];
        if ((sprite->data[7] & 1) && sprite->data[4] != 0)
            sprite->data[4]--;
        r4 = gSineTable[(u8)sprite->data[3]];
        r1 = gSineTable[(u8)(sprite->data[3] + 64)];
        SetOamMatrix(1, r1, r4, -r4, r1);
        break;
    case 2:
        sprite->oam.affineMode = 1;
        sprite->oam.matrixNum = 2;
        sprite->data[0] = 20;
        sprite->data[4] = 36;
        //fall through
    case 20:
        if (sprite->x <= 96)
        {
            sprite->x += 3;
            sprite->y -= 1;
            sprite->y2 = -Sin((u8)sprite->data[2], 24);
            sprite->data[2] += 4;
        }
        sprite->data[3] -= sprite->data[4];
        if ((sprite->data[7] & 1) && sprite->data[4] != 0)
            sprite->data[4]--;
        r4 = gSineTable[(u8)sprite->data[3]];
        r1 = gSineTable[(u8)(sprite->data[3] + 64)];
        SetOamMatrix(2, r1, r4, -r4, r1);
        break;
    }
}

static void sub_813E4B8(struct Sprite *sprite)
{
    u16 r4;
    u16 r2;
    u16 r1;

    sprite->data[7]++;
    if (sprite->data[7] & 1)
        sprite->invisible = FALSE;
    else
        sprite->invisible = TRUE;
    if (sprite->data[2] >= 64)
    {
        DestroySprite(sprite);
        return;
    }
    sprite->data[2] += 2;
    r4 = Sin((u8)sprite->data[2], 40);
    sprite->x2 = Cos((u8)(sprite->data[0] * 32), r4);
    sprite->y2 = Sin((u8)(sprite->data[0] * 32), r4);
    if (sprite->data[0] == 0)
    {
        sprite->data[3] -= sprite->data[1];
        if ((sprite->data[7] & 1) && sprite->data[1] != 0)
            sprite->data[1]--;
        r2 = gSineTable[(u8)sprite->data[3]];
        r1 = gSineTable[(u8)(sprite->data[3] + 64)];
        SetOamMatrix(16, r1, r2, -r2, r1);
    }
}

static void sub_813E580(u16 x, u16 y)
{
    u8 i;
    u8 spriteId;

    for (i = 0; i < 8; i++)
    {
        spriteId = CreateSprite(&gSpriteTemplate_840B0B0, x, y, 0);
        gSprites[spriteId].oam.affineMode = 1;
        gSprites[spriteId].oam.matrixNum = 16;
        gSprites[spriteId].data[0] = i;
        gSprites[spriteId].data[1] = 32;
    }
}

static void sub_813E5E0(struct Sprite *sprite)
{
    if (gUnknown_0203931A != 0)
    {
        DestroySprite(sprite);
    }
    else
    {
        sprite->invisible = gSprites[sprite->data[0]].invisible;
        if (sprite->data[7] < 12)
            sprite->data[7]++;
        sprite->data[6] += 4;
        sprite->x = sprite->data[4] + gSineTable[(u8)(sprite->data[3] + 64)] * sprite->data[6] / 256;
        //This useless '+ 0' is needed to make the asm match
        sprite->y = sprite->data[5] + gSineTable[(u8)(sprite->data[3] + 0)] * sprite->data[6] / 256;
        sprite->y2 = gSineTable[(u8)(sprite->data[1] + 0)] * sprite->data[7] / 256;
        sprite->data[1] += 16;
        if (sprite->y > sprite->data[2])
            DestroySprite(sprite);
    }
}

static void sub_813E6C0(struct Sprite *sprite)
{
    u8 spriteId;
    u8 i;
    s16 var1;
    s16 var2;

    if (gUnknown_0203931A != 0)
    {
        DestroySprite(sprite);
    }
    else
    {
        sprite->data[7]++;
        sprite->invisible = TRUE;
        if (gSprites[sprite->data[0]].data[0] == 1 && !(sprite->data[7] & 3))
        {
            var1 = sprite->data[1] + gSprites[sprite->data[0]].x;
            var2 = sprite->data[2] + gSprites[sprite->data[0]].y;
            for (i = 0; i < 3; i++)
            {
                u8 r3 = gSprites[sprite->data[0]].subpriority - 1;
                //Make redundant copies of these variables to get the asm to match
                s16 _var1 = var1;
                s16 _var2 = var2;

                spriteId = CreateSprite(&gSpriteTemplate_840B0DC, _var1, _var2, r3);
                if (spriteId != 64)
                {
                    gSprites[spriteId].data[0] = sprite->data[0];
                    gSprites[spriteId].data[1] = (((sprite->data[7] >> 2) & 7) << 5) + i * 85;
                    gSprites[spriteId].data[2] = sprite->data[3];
                    gSprites[spriteId].data[3] = 104;
                    gSprites[spriteId].data[4] = var1;
                    gSprites[spriteId].data[5] = var2;
                    gSprites[spriteId].data[6] = 0;
                }
            }
        }
    }
}

static void sub_813E7C0(u8 a)
{
    u8 spriteId;

    spriteId = CreateSprite(&gSpriteTemplate_840B0F4, 0, 0, 0);
    if (spriteId != 64)
    {
        gSprites[spriteId].data[0] = a;
        gSprites[spriteId].data[1] = -12;
        gSprites[spriteId].data[2] = 0;
        gSprites[spriteId].data[3] = 136;
    }
}

static void sub_813E804(struct Sprite *sprite)
{
    if (gUnknown_0203931A != 0)
    {
        DestroySprite(sprite);
    }
    else
    {
        sprite->invisible = gSprites[sprite->data[0]].invisible;
        sprite->data[7]++;
        if (sprite->data[3] < 40)
            sprite->data[3] += 2;
        //This useless '+ 0' is needed to make the asm match
        sprite->x = gSprites[sprite->data[0]].x + gSprites[sprite->data[0]].x2 + gSineTable[(u8)(sprite->data[1] + 64)] * sprite->data[3] / 256;
        sprite->y = gSprites[sprite->data[0]].y + gSprites[sprite->data[0]].y2 + gSineTable[(u8)(sprite->data[1] + 0)] * sprite->data[3] / 512;
        sprite->data[1] += 2;
        sprite->y2 = gSineTable[(u8)(sprite->data[2] + 0)] / 32;
        sprite->data[2] += 8;
        if ((sprite->data[1] & 0xFF) < 128)
            sprite->subpriority = gSprites[sprite->data[0]].subpriority - 1;
        else
            sprite->subpriority = gSprites[sprite->data[0]].subpriority + 1;
    }
}

static void sub_813E930(u8 a)
{
    u8 i;
    u8 spriteId;

    for (i = 0; i < 8; i++)
    {
        spriteId = CreateSprite(&gSpriteTemplate_840B124, gSprites[a].x, gSprites[a].y, 0);
        if (spriteId != 64)
        {
            gSprites[spriteId].data[0] = a;
            gSprites[spriteId].data[1] = i * 32;
        }
    }
}

static void sub_813E980(struct Sprite *sprite)
{
    if (gUnknown_0203931A != 0)
    {
        DestroySprite(sprite);
    }
    else
    {
        u8 r0;
        u16 matrixNum;

        sprite->invisible = gSprites[sprite->data[0]].invisible;
        sprite->data[7]++;
        sprite->data[6] += 8;
        sprite->x = sprite->data[4] + gSineTable[(u8)(sprite->data[3] + 64)] * sprite->data[6] / 256;
        sprite->y = sprite->data[5] + gSineTable[(u8)(sprite->data[3] + 0)] * sprite->data[6] / 256;
        r0 = sprite->data[6] / 16;
        if (r0 > 9)
            r0 = 9;
        matrixNum = (r0 + 18) & 31;
        sprite->oam.matrixNum = matrixNum;
        if (sprite->data[6] > 160)
            DestroySprite(sprite);
    }
}

static void sub_813EA60(struct Sprite *sprite)
{
    bool32 r6;
    s16 r1, r2;
    u8 spriteId;

    if (gUnknown_0203931A != 0)
    {
        DestroySprite(sprite);
    }
    else
    {
        sprite->data[7]++;
        sprite->invisible = TRUE;
        if (gSprites[sprite->data[0]].data[0] == 1)
        {
            r6 = (sprite->data[7] & 1);
            if (!r6)
            {
                r1 = sprite->data[1] + gSprites[sprite->data[0]].x;
                r2 = sprite->data[2] + gSprites[sprite->data[0]].y;
                spriteId = CreateSprite(&gSpriteTemplate_840B150, r1, r2, gSprites[sprite->data[0]].subpriority + 1);
                if (spriteId != 64)
                {
                    gSprites[spriteId].oam.affineMode = 3;
                    gSprites[spriteId].oam.matrixNum = 18;
                    CalcCenterToCornerVec(&gSprites[spriteId], 0, 1, 3);
                    gSprites[spriteId].data[0] = sprite->data[0];
                    gSprites[spriteId].data[3] = gUnknown_0840B168[(sprite->data[7] >> 1) & 7];
                    gSprites[spriteId].data[4] = r1;
                    gSprites[spriteId].data[5] = r2;
                    gSprites[spriteId].data[6] = r6;
                }
            }
        }
    }
}

static void InitIntroTorchicAttackAnim(u8 a)
{
    u8 spriteId;
    u8 i;

    spriteId = CreateSprite(&gSpriteTemplate_840B170, 0, 0, 0);
    if (spriteId != 64)
    {
        gSprites[spriteId].data[0] = a;
        gSprites[spriteId].data[1] = 0;
        gSprites[spriteId].data[2] = 8;
        gSprites[spriteId].data[3] = 24;
    }
    for (i = 0; i < 10; i++)
    {
        SetOamMatrix(18 + i, gUnknown_0840B188[i], 0, 0, gUnknown_0840B188[i]);
    }
}

static void sub_813EBBC(struct Sprite *sprite)
{
    if (gUnknown_0203931A != 0)
    {
        DestroySprite(sprite);
    }
    else
    {
        sprite->invisible = gSprites[sprite->data[0]].invisible;
        sprite->data[7] += 1;
        sprite->data[6] += 8;
        sprite->x = sprite->data[4] + gSineTable[(u8)(sprite->data[3] + 64)] * sprite->data[6] / 256;
        sprite->y = sprite->data[5] + gSineTable[(u8)(sprite->data[3] + 0)] * sprite->data[6] / 256;
        sprite->y2 = gSineTable[(u8)(sprite->data[1] + 0)] / 64;
        sprite->data[1] += 16;
        if (sprite->y < sprite->data[2])
            DestroySprite(sprite);
    }
}

static void sub_813EC90(struct Sprite *sprite)
{
    bool32 r6;
    s16 r1, r2;
    u8 spriteId;
    u16 foo;

    if (gUnknown_0203931A != 0)
    {
        DestroySprite(sprite);
    }
    else
    {
        sprite->data[7]++;
        sprite->invisible = TRUE;
        if (gSprites[sprite->data[0]].data[0] == 1)
        {
            r6 = sprite->data[7] & 1;
            if (!r6)
            {
                r1 = sprite->data[1] + gSprites[sprite->data[0]].x;
                r2 = sprite->data[2] + gSprites[sprite->data[0]].y;
                spriteId = CreateSprite(&gSpriteTemplate_840B1B0, r1, r2, gSprites[sprite->data[0]].subpriority + 1);
                if (spriteId != 64)
                {
                    gSprites[spriteId].oam.affineMode = 3;
                    gSprites[spriteId].oam.matrixNum = 17;
                    CalcCenterToCornerVec(&gSprites[spriteId], 0, 1, 3);
                    gSprites[spriteId].data[0] = sprite->data[0];
                    gSprites[spriteId].data[1] = ((sprite->data[7] >> 2) & 7) << 5;
                    gSprites[spriteId].data[2] = sprite->data[3];
                    gSprites[spriteId].data[3] = 232;
                    gSprites[spriteId].data[4] = r1;
                    gSprites[spriteId].data[5] = r2;
                    gSprites[spriteId].data[6] = r6;
                }
            }
            if (sprite->data[6] < 112)
                sprite->data[6] += 4;
        }
        foo = 256 - gSineTable[(u8)sprite->data[6]] / 2;
        SetOamMatrix(17, foo, 0, 0, foo);
    }
}

static void InitIntroMudkipAttackAnim(u8 a)
{
    u8 spriteId;

    spriteId = CreateSprite(&gSpriteTemplate_840B1C8, 0, 0, 0);
    if (spriteId != 64)
    {
        gSprites[spriteId].data[0] = a;
        gSprites[spriteId].data[1] = 0;
        gSprites[spriteId].data[2] = 12;
        gSprites[spriteId].data[3] = 24;
    }
}

static void sub_813EDFC(struct Sprite *sprite)
{
    u16 foo;

    //I'm not sure why a switch statement was used here.
    //if (sprite->data[0] != 1) would have been more appropriate.
    switch (sprite->data[0])
    {
    case 0:
    default:
        sprite->invisible = FALSE;
        sprite->oam.affineMode = 3;
        sprite->oam.matrixNum = 18;
        CalcCenterToCornerVec(sprite, 0, 3, 3);
        sprite->data[1] = 0;
        sprite->data[0] = 1;
        //fall through
    case 1:
        break;
    }
    sprite->data[7]++;
    if (sprite->data[7] & 1)
    {
        sprite->invisible = TRUE;
    }
    else
    {
        sprite->invisible = FALSE;
        if (sprite->data[1] < 64)
            sprite->data[1]++;
    }
    foo = 256 - gSineTable[(u8)sprite->data[1]] / 2;
    SetOamMatrix(18, foo, 0, 0, foo);
}
