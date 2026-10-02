#include "global.h"
#include "gba/m4a_internal.h"
#include "intro.h"
#include "data2.h"
#include "decompress.h"
#include "graphics.h"
#include "hall_of_fame.h"
#include "intro_credits_graphics.h"
#include "libgncmultiboot.h"
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

extern struct SpriteTemplate gCreatingSpriteTemplate;
extern u16 gSaveFileStatus;
extern u8 gReservedSpritePaletteCount;
extern const u8 gIntroCopyright_Gfx[];
extern const u16 gIntroCopyright_Pal[];
extern const u16 gIntroCopyright_Tilemap[];

static EWRAM_DATA u16 sIntroCharacterGender = 0;
static EWRAM_DATA u16 sIntroBattleFinished = 0;

#define GFXTAG_DROPS_LOGO              2000
#define PALTAG_DROPS                   2000
#define PALTAG_LOGO                    2001
#define TAG_LATI_SILHOUETTE            2002
#define GFXTAG_INTRO_BATTLE_POKEBALL   2002
#define PALTAG_INTRO_BATTLE_POKEBALL   2002
#define GFXTAG_INTRO_BATTLE_EFFECTS    2003
#define PALTAG_INTRO_BATTLE_BURST      2003
#define PALTAG_INTRO_BATTLE_ATTACK     2004

enum
{
    INTRO_BATTLE_BG_GREEN,
    INTRO_BATTLE_BG_RED,
    INTRO_BATTLE_BG_BLUE,
};

// Used by the intro's task and sprite state machines.
#define tState data[0]
#define sState data[0]

#define TIMER_BIG_DROP_START             76
#define TIMER_BIG_DROP_FALLS            251
#define TIMER_SMALL_DROP_1              368
#define TIMER_SMALL_DROP_2              384
#define TIMER_GAME_FREAK_LOGO           560
#define TIMER_START_SCENE_1_PAN         739
#define TIMER_LATI_SILHOUETTE_APPEAR    880
#define TIMER_END_SCENE_1_PAN           904
#define TIMER_SCENE_1_FADE_OUT         1007
#define TIMER_START_SCENE_2            1026
#define TIMER_PLAYER_DRIFT_BACK        1109
#define TIMER_PLAYER_MOVE_FORWARD      1214
#define TIMER_LATI_ENTER               1394
#define TIMER_PLAYER_MOVE_BACKWARD     1398
#define TIMER_PLAYER_HOLD_POSITION     1586
#define TIMER_PLAYER_EXIT              1727
#define TIMER_SCENE_2_FADE_OUT         1823
#define TIMER_START_SCENE_3            2068

// The scene 3 counter restarts when the Poké Ball is shown.
#define TIMER_POKEBALL_FADE_OUT          44
#define TIMER_LOAD_BATTLE_STREAKS         59
#define TIMER_SHARPEDO_INTRO              80
#define TIMER_DUSKULL_INTRO              152
#define TIMER_TRAINER_INTRO              219
#define TIMER_TRAINER_THROW_POKEBALLS    304
#define TIMER_TRAINER_RETURN             384
#define TIMER_TRAINER_FADE               400
#define TIMER_TRAINER_TRANSFORM_START    432
#define TIMER_TRAINER_TRANSFORM_END      462
#define TIMER_SHARPEDO_BATTLE            463
#define TIMER_DUSKULL_BATTLE             539
#define TIMER_OPPONENTS_EXIT             623
#define TIMER_MUDKIP_BATTLE              624
#define TIMER_TORCHIC_BATTLE             700
#define TIMER_BATTLE_FINISH              776
#define TIMER_BATTLE_FLASH               781
#define TIMER_BATTLE_BLAST_SOUND         800
#define TIMER_BATTLE_FADE_OUT            850
#define TIMER_END_INTRO                  946

u32 gIntroFrameCounter;
struct GcmbStruct gMultibootProgramStruct;

//--------------------------------------------------
// Graphics Data
//--------------------------------------------------

static const u16 sIntroDrops_Pal[] = INCBIN_U16("graphics/intro/scene_1/drops.gbapal");
static const u16 sIntroLogo_Pal[] = INCBIN_U16("graphics/intro/scene_1/logo.gbapal");
#if ENGLISH
static const u8 sIntroDropsLogo_Gfx[] = INCBIN_U8("graphics/intro/scene_1/drops_logo.4bpp.lz");
#elif GERMAN
extern const u8 sIntroDropsLogo_Gfx[];
#endif
static const u16 sIntro1Bg_Pal[][16] =
{
    INCBIN_U16("graphics/intro/scene_1/bg_palette_1.gbapal"),
    INCBIN_U16("graphics/intro/scene_1/bg_palette_2.gbapal"),
    INCBIN_U16("graphics/intro/scene_1/bg_palette_3.gbapal"),
    INCBIN_U16("graphics/intro/scene_1/bg_palette_4.gbapal"),
    INCBIN_U16("graphics/intro/scene_1/bg_palette_5.gbapal"),
    INCBIN_U16("graphics/intro/scene_1/bg_palette_6.gbapal"),
    INCBIN_U16("graphics/intro/scene_1/bg_palette_7.gbapal"),
    INCBIN_U16("graphics/intro/scene_1/bg_palette_8.gbapal"),
    INCBIN_U16("graphics/intro/scene_1/bg_palette_9.gbapal"),
    INCBIN_U16("graphics/intro/scene_1/bg_palette_10.gbapal"),
    INCBIN_U16("graphics/intro/scene_1/bg_palette_11.gbapal"),
    INCBIN_U16("graphics/intro/scene_1/bg_palette_12.gbapal"),
    INCBIN_U16("graphics/intro/scene_1/bg_palette_13.gbapal"),
    INCBIN_U16("graphics/intro/scene_1/bg_palette_14.gbapal"),
    INCBIN_U16("graphics/intro/scene_1/bg_palette_15.gbapal"),
    INCBIN_U16("graphics/intro/scene_1/bg_palette_16.gbapal"),
};
static const u8 sIntro1Bg0_Tilemap[] = INCBIN_U8("graphics/intro/scene_1/bg0_map.bin.lz");
static const u8 sIntro1Bg1_Tilemap[] = INCBIN_U8("graphics/intro/scene_1/bg1_map.bin.lz");
static const u8 sIntro1Bg2_Tilemap[] = INCBIN_U8("graphics/intro/scene_1/bg2_map.bin.lz");
static const u8 sIntro1Bg3_Tilemap[] = INCBIN_U8("graphics/intro/scene_1/bg3_map.bin.lz");
static const u8 sIntro1Bg_Gfx[] = INCBIN_U8("graphics/intro/scene_1/bg_leaves.4bpp.lz");
static const u16 sIntroPokeball_Pal[] = INCBIN_U16("graphics/intro/scene_3/pokeball.gbapal");
static const u8 sIntroPokeball_Tilemap[] = INCBIN_U8("graphics/intro/scene_3/pokeball_map.bin.lz");
static const u8 sIntroPokeball_Gfx[] = INCBIN_U8("graphics/intro/scene_3/pokeball.8bpp.lz");
static const u16 sIntroStreaks_Pal[] = INCBIN_U16("graphics/intro/scene_3/streaks.gbapal");
static const u8 sIntroStreaks_Gfx[] = INCBIN_U8("graphics/intro/scene_3/streaks.4bpp.lz");
static const u8 sIntroStreaks_Tilemap[] = INCBIN_U8("graphics/intro/scene_3/streaks_map.bin.lz");
static const u16 sIntroBattleBurst_Pal[] = INCBIN_U16("graphics/intro/scene_3/battle_burst.gbapal");
static const u16 sIntroBattleAttack_Pal[] = INCBIN_U16("graphics/intro/scene_3/battle_attack.gbapal");
static const u8 sIntroBattleEffects_Gfx[] = INCBIN_U8("graphics/intro/scene_3/battle_effects.4bpp.lz");
static const u16 sIntroLati_Pal[] = INCBIN_U16("graphics/intro/scene_1/lati.gbapal");
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

enum
{
    WATER_DROP_WAIT,
    WATER_DROP_SLIDE,
    WATER_DROP_FALL,
};

enum
{
    DROP_ANIM_UPPER_HALF,
    DROP_ANIM_LOWER_HALF,
    DROP_ANIM_REFLECTION,
    DROP_ANIM_RIPPLE,
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
    [DROP_ANIM_UPPER_HALF] = sAnim_WaterDrop_UpperHalf,
    [DROP_ANIM_LOWER_HALF] = sAnim_WaterDrop_LowerHalf,
    [DROP_ANIM_REFLECTION] = sAnim_WaterDrop_Reflection,
    [DROP_ANIM_RIPPLE] = sAnim_WaterDrop_Ripple,
};
static void SpriteCB_WaterDrop(struct Sprite *sprite);
static const struct SpriteTemplate sSpriteTemplate_WaterDrop =
{
    .tileTag = GFXTAG_DROPS_LOGO,
    .paletteTag = PALTAG_DROPS,
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

enum
{
    PLAYER_BICYCLE_ANIM_FAST,
    PLAYER_BICYCLE_ANIM_SLOW,
    PLAYER_BICYCLE_ANIM_LOOK_BACK,
    PLAYER_BICYCLE_ANIM_LOOK_FORWARD,
};

static const union AnimCmd *const sAnims_PlayerBicycle[] =
{
    [PLAYER_BICYCLE_ANIM_FAST] = sAnim_PlayerBicycle_Fast,
    [PLAYER_BICYCLE_ANIM_SLOW] = sAnim_PlayerBicycle_Slow,
    [PLAYER_BICYCLE_ANIM_LOOK_BACK] = sAnim_PlayerBicycle_LookBack,
    [PLAYER_BICYCLE_ANIM_LOOK_FORWARD] = sAnim_PlayerBicycle_LookForward,
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
static const union AnimCmd sAnim_PresentsLetter_GermanExtra[] =
{
    ANIMCMD_FRAME(118, 8),
    ANIMCMD_END,
};
#endif

enum
{
    GAMEFREAK_G,
    GAMEFREAK_A,
    GAMEFREAK_M,
    GAMEFREAK_E,
    GAMEFREAK_F,
    GAMEFREAK_R,
    GAMEFREAK_K,
};

enum
{
    PRESENTS_P,
    PRESENTS_R,
    PRESENTS_E,
    PRESENTS_S,
    PRESENTS_N,
    PRESENTS_T,
#if GERMAN
    PRESENTS_GERMAN_EXTRA,
#endif
};

static const union AnimCmd *const sAnims_GameFreakLetter[] =
{
    [GAMEFREAK_G] = sAnim_GameFreakLetter_G,
    [GAMEFREAK_A] = sAnim_GameFreakLetter_A,
    [GAMEFREAK_M] = sAnim_GameFreakLetter_M,
    [GAMEFREAK_E] = sAnim_GameFreakLetter_E,
    [GAMEFREAK_F] = sAnim_GameFreakLetter_F,
    [GAMEFREAK_R] = sAnim_GameFreakLetter_R,
    [GAMEFREAK_K] = sAnim_GameFreakLetter_K,
};
static const union AnimCmd *const sAnims_PresentsLetter[] =
{
    [PRESENTS_P] = sAnim_PresentsLetter_P,
    [PRESENTS_R] = sAnim_PresentsLetter_R,
    [PRESENTS_E] = sAnim_PresentsLetter_E,
    [PRESENTS_S] = sAnim_PresentsLetter_S,
    [PRESENTS_N] = sAnim_PresentsLetter_N,
    [PRESENTS_T] = sAnim_PresentsLetter_T,
#if GERMAN
    [PRESENTS_GERMAN_EXTRA] = sAnim_PresentsLetter_GermanExtra,
#endif
};
static const union AnimCmd *const sAnims_GameFreakLogo[] =
{
    sAnim_GameFreakLogo,
};
static const s16 sGameFreakLetterData[][2] =
{
    {GAMEFREAK_G, -72},
    {GAMEFREAK_A, -56},
    {GAMEFREAK_M, -40},
    {GAMEFREAK_E, -24},
    {GAMEFREAK_F, 8},
    {GAMEFREAK_R, 24},
    {GAMEFREAK_E, 40},
    {GAMEFREAK_A, 56},
    {GAMEFREAK_K, 72},
};
static const s16 sPresentsLetterData[][2] =
{
    {PRESENTS_P, -28},
    {PRESENTS_R, -20},
    {PRESENTS_E, -12},
    {PRESENTS_S, -4},
    {PRESENTS_E, 4},
    {PRESENTS_N, 12},
    {PRESENTS_T, 20},
    {PRESENTS_S, 28},
};
static void SpriteCB_LogoLetter(struct Sprite *sprite);
static const struct SpriteTemplate sSpriteTemplate_GameFreakLetter =
{
    .tileTag = GFXTAG_DROPS_LOGO,
    .paletteTag = PALTAG_LOGO,
    .oam = &sOamData_GameFreakLetter,
    .anims = sAnims_GameFreakLetter,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_LogoLetter,
};
static const struct SpriteTemplate sSpriteTemplate_PresentsLetter =
{
    .tileTag = GFXTAG_DROPS_LOGO,
    .paletteTag = PALTAG_LOGO,
    .oam = &sOamData_PresentsLetter,
    .anims = sAnims_PresentsLetter,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_LogoLetter,
};
static const struct SpriteTemplate sSpriteTemplate_GameFreakLogo =
{
    .tileTag = GFXTAG_DROPS_LOGO,
    .paletteTag = PALTAG_LOGO,
    .oam = &sOamData_GameFreakLogo,
    .anims = sAnims_GameFreakLogo,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_LogoLetter,
};
static const struct OamData sOamData_LatiSilhouette =
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
static const union AnimCmd sAnim_LatiSilhouette[] =
{
    ANIMCMD_FRAME(0, 10),
    ANIMCMD_JUMP(0),
};
static const union AnimCmd *const sAnims_LatiSilhouette[] =
{
    sAnim_LatiSilhouette,
};
static void SpriteCB_LatiSilhouette(struct Sprite *sprite);
static const struct SpriteTemplate sSpriteTemplate_LatiSilhouette =
{
    .tileTag = TAG_LATI_SILHOUETTE,
    .paletteTag = TAG_LATI_SILHOUETTE,
    .oam = &sOamData_LatiSilhouette,
    .anims = sAnims_LatiSilhouette,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_LatiSilhouette,
};
static const struct CompressedSpriteSheet sSpriteSheet_WaterDropsAndLogo[] =
{
    {sIntroDropsLogo_Gfx, 0x1400, GFXTAG_DROPS_LOGO},
    {NULL},
};
static const struct CompressedSpriteSheet sSpriteSheet_LatiSilhouette[] =
{
    {sIntroLati_Gfx, 0x400, TAG_LATI_SILHOUETTE},
    {NULL},
};
static const struct SpritePalette sSpritePalettes_Intro1[] =
{
    {sIntroDrops_Pal, PALTAG_DROPS},
    {sIntroLogo_Pal, PALTAG_LOGO},
    {sIntroLati_Pal, TAG_LATI_SILHOUETTE},
    {NULL},
};
static const union AnimCmd sAnim_IntroTrainerIdle[] =
{
    ANIMCMD_FRAME(3, 0),
    ANIMCMD_END,
};
static const union AnimCmd sAnim_IntroTrainerReady[] =
{
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_END,
};
static const union AnimCmd sAnim_IntroTrainerThrowPokeball[] =
{
    ANIMCMD_FRAME(1, 8),
    ANIMCMD_FRAME(2, 8),
    ANIMCMD_END,
};
static const union AnimCmd *const sAnims_IntroTrainer[] =
{
    sAnim_IntroTrainerIdle,
    sAnim_IntroTrainerReady,
    sAnim_IntroTrainerThrowPokeball,
};
static const struct OamData sOamData_IntroFlyingPokeball =
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
static const union AnimCmd sAnim_IntroFlyingPokeball[] =
{
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_END,
};
static const union AnimCmd *const sAnims_IntroFlyingPokeball[] =
{
    sAnim_IntroFlyingPokeball,
};
static void SpriteCB_IntroFlyingPokeball(struct Sprite *sprite);
static const struct SpriteTemplate sSpriteTemplate_IntroFlyingPokeball =
{
    .tileTag = GFXTAG_INTRO_BATTLE_POKEBALL,
    .paletteTag = PALTAG_INTRO_BATTLE_POKEBALL,
    .oam = &sOamData_IntroFlyingPokeball,
    .anims = sAnims_IntroFlyingPokeball,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_IntroFlyingPokeball,
};
static const struct OamData sOamData_IntroBurstParticle =
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
static const union AnimCmd sAnim_IntroBurstParticle[] =
{
    ANIMCMD_FRAME(1, 8),
    ANIMCMD_END,
};
static const union AnimCmd *const sAnims_IntroBurstParticle[] =
{
    sAnim_IntroBurstParticle,
};
static void SpriteCB_IntroBurstParticle(struct Sprite *sprite);
static const struct SpriteTemplate sSpriteTemplate_IntroBurstParticle =
{
    .tileTag = GFXTAG_INTRO_BATTLE_EFFECTS,
    .paletteTag = PALTAG_INTRO_BATTLE_BURST,
    .oam = &sOamData_IntroBurstParticle,
    .anims = sAnims_IntroBurstParticle,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_IntroBurstParticle,
};
static const struct OamData sOamData_IntroAttackParticle =
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
static const union AnimCmd sAnim_IntroAttackParticle[] =
{
    ANIMCMD_FRAME(14, 8),
    ANIMCMD_END,
};
static const union AnimCmd *const sAnims_IntroAttackParticle[] =
{
    sAnim_IntroAttackParticle,
};
static void SpriteCB_IntroAttackParticle(struct Sprite *sprite);
static const struct SpriteTemplate sSpriteTemplate_IntroAttackParticle =
{
    .tileTag = GFXTAG_INTRO_BATTLE_EFFECTS,
    .paletteTag = PALTAG_INTRO_BATTLE_ATTACK,
    .oam = &sOamData_IntroAttackParticle,
    .anims = sAnims_IntroAttackParticle,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_IntroAttackParticle,
};
static void SpriteCB_IntroAttackParticleSpawner(struct Sprite *sprite);
static const struct SpriteTemplate sSpriteTemplate_IntroAttackParticleSpawner =
{
    .tileTag = GFXTAG_INTRO_BATTLE_EFFECTS,
    .paletteTag = PALTAG_INTRO_BATTLE_ATTACK,
    .oam = &sOamData_IntroAttackParticle,
    .anims = sAnims_IntroAttackParticle,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_IntroAttackParticleSpawner,
};
static const struct OamData sOamData_IntroOrbitingParticle =
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
static const union AnimCmd sAnim_IntroOrbitingParticle[] =
{
    ANIMCMD_FRAME(6, 8),
    ANIMCMD_FRAME(6, 8, .hFlip = TRUE),
    ANIMCMD_JUMP(0),
};
static const union AnimCmd *const sAnims_IntroOrbitingParticle[] =
{
    sAnim_IntroOrbitingParticle,
};
static void SpriteCB_IntroOrbitingParticle(struct Sprite *sprite);
static const struct SpriteTemplate sSpriteTemplate_IntroOrbitingParticle =
{
    .tileTag = GFXTAG_INTRO_BATTLE_EFFECTS,
    .paletteTag = PALTAG_INTRO_BATTLE_ATTACK,
    .oam = &sOamData_IntroOrbitingParticle,
    .anims = sAnims_IntroOrbitingParticle,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_IntroOrbitingParticle,
};
static const struct OamData sOamData_IntroTorchicAttackParticle =
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
static const union AnimCmd sAnim_IntroTorchicAttackParticle[] =
{
    ANIMCMD_FRAME(10, 8),
    ANIMCMD_END,
};
static const union AnimCmd *const sAnims_IntroTorchicAttackParticle[] =
{
    sAnim_IntroTorchicAttackParticle,
};
static void SpriteCB_IntroTorchicAttackParticle(struct Sprite *sprite);
static const struct SpriteTemplate sSpriteTemplate_IntroTorchicAttackParticle =
{
    .tileTag = GFXTAG_INTRO_BATTLE_EFFECTS,
    .paletteTag = PALTAG_INTRO_BATTLE_ATTACK,
    .oam = &sOamData_IntroTorchicAttackParticle,
    .anims = sAnims_IntroTorchicAttackParticle,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_IntroTorchicAttackParticle,
};
static const u8 sTorchicAttackParticleAngles[] = {0xE6, 0xEB, 0xE4, 0xEA, 0xE5, 0xE9, 0xE7, 0xE8};
static void SpriteCB_IntroTorchicAttackSpawner(struct Sprite *sprite);
static const struct SpriteTemplate sSpriteTemplate_IntroTorchicAttackSpawner =
{
    .tileTag = GFXTAG_INTRO_BATTLE_EFFECTS,
    .paletteTag = PALTAG_INTRO_BATTLE_ATTACK,
    .oam = &sOamData_IntroTorchicAttackParticle,
    .anims = sAnims_IntroTorchicAttackParticle,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_IntroTorchicAttackSpawner,
};
static const u16 sTorchicAttackParticleScales[] = {0x200, 0x1C0, 0x180, 0x140, 0x100, 0xE0, 0xC0, 0xA0, 0x80, 0x80};
static const struct OamData sOamData_IntroMudkipAttackParticle =
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
static const union AnimCmd sAnim_IntroMudkipAttackParticle[] =
{
    ANIMCMD_FRAME(2, 8),
    ANIMCMD_END,
};
static const union AnimCmd *const sAnims_IntroMudkipAttackParticle[] =
{
    sAnim_IntroMudkipAttackParticle,
};
static void SpriteCB_IntroMudkipAttackParticle(struct Sprite *sprite);
static const struct SpriteTemplate sSpriteTemplate_IntroMudkipAttackParticle =
{
    .tileTag = GFXTAG_INTRO_BATTLE_EFFECTS,
    .paletteTag = PALTAG_INTRO_BATTLE_ATTACK,
    .oam = &sOamData_IntroMudkipAttackParticle,
    .anims = sAnims_IntroMudkipAttackParticle,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_IntroMudkipAttackParticle,
};
static void SpriteCB_IntroMudkipAttackSpawner(struct Sprite *sprite);
static const struct SpriteTemplate sSpriteTemplate_IntroMudkipAttackSpawner =
{
    .tileTag = GFXTAG_INTRO_BATTLE_EFFECTS,
    .paletteTag = PALTAG_INTRO_BATTLE_ATTACK,
    .oam = &sOamData_IntroMudkipAttackParticle,
    .anims = sAnims_IntroMudkipAttackParticle,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_IntroMudkipAttackSpawner,
};
static const struct OamData sOamData_IntroBattleFlash =
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
static const union AnimCmd sAnim_IntroBattleFlash[] =
{
    ANIMCMD_FRAME(16, 8),
    ANIMCMD_END,
};
static const union AnimCmd *const sAnims_IntroBattleFlash[] =
{
    sAnim_IntroBattleFlash,
};
static void SpriteCB_IntroBattleFlash(struct Sprite *sprite);
static const struct SpriteTemplate sSpriteTemplate_IntroBattleFlash =
{
    .tileTag = GFXTAG_INTRO_BATTLE_EFFECTS,
    .paletteTag = PALTAG_INTRO_BATTLE_BURST,
    .oam = &sOamData_IntroBattleFlash,
    .anims = sAnims_IntroBattleFlash,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_IntroBattleFlash,
};
static const struct CompressedSpriteSheet sSpriteSheet_IntroBattlePokeball[] =
{
    {gBallGfx_Poke, 0x100, GFXTAG_INTRO_BATTLE_POKEBALL},
    {NULL},
};
static const struct CompressedSpriteSheet sSpriteSheet_IntroBattleEffects[] =
{
    {sIntroBattleEffects_Gfx, 0xa00, GFXTAG_INTRO_BATTLE_EFFECTS},
    {NULL},
};
static const struct CompressedSpritePalette sSpritePalette_IntroBattlePokeball[] =
{
    {gBallPal_Poke, PALTAG_INTRO_BATTLE_POKEBALL},
    {NULL},
};
static const struct SpritePalette sSpritePalettes_IntroBattleEffects[] =
{
    {sIntroBattleBurst_Pal, PALTAG_INTRO_BATTLE_BURST},
    {sIntroBattleAttack_Pal, PALTAG_INTRO_BATTLE_ATTACK},
    {NULL},
};

// Game Freak probably used the raw address here.
// Treating this like a u8 * causes the compiler
// to remove it at link time.
static u8 (*const sIntroBattleTilemapBuffer)[32] = (u8 (*)[32])gSharedMem;

static void MainCB2_EndIntro(void);
void Task_Scene1_Load(u8);
static void Task_Scene1_FadeIn(u8);
static void Task_Scene1_WaterDrops(u8);
static void Task_Scene1_PanUpAndShowLati(u8);
static void Task_Scene1_End(u8);
static void Task_Scene2_Load(u8);
static void Task_Scene2_CreateSprites(u8);
static void Task_Scene2_BikeRide(u8);
static void Task_Scene2_End(u8);
static void Task_Scene3_Load(u8);
static void Task_Scene3_SpinPokeball(u8);
static void Task_Scene3_WaitForBattle(u8);
static void Task_Scene3_LoadBattleStreaks(u8);
static void Task_Scene3_InitBattleWindow(u8);
static void Task_Scene3_OpenBattleWindow(u8);
static void Task_Scene3_FinishOpeningWindow(u8);
static void Task_Scene3_StartPokemonBattle(u8);
static void Task_Scene3_PokemonBattle(u8);
static void Task_EndIntroMovie(u8);
static void Task_Scene3_AnimateBattleBackground(u8);
static void IntroResetGpuRegs(void);
static void Task_AnimateGameFreakLogoBlend(u8);
static u16 CreateIntroBattleMonSprite(u16, s16, s16, u16, u8);
static u8 CreateIntroTrainerSprite(u16, u16, u16, u16);
static void SetIntroBattleBackgroundColor(u8);
static void SpriteCB_WaterDrop_Slide(struct Sprite *);
static void SpriteCB_WaterDrop_ReachLeafEnd(struct Sprite *);
static void SpriteCB_WaterDrop_DangleFromLeaf(struct Sprite *);
static void SpriteCB_WaterDrop_Fall(struct Sprite *);
static u8 CreateWaterDrop(s16, s16, u16, u16, u16, u8);
static void SpriteCB_PlayerOnBicycle(struct Sprite *);
static void SpriteCB_Lati(struct Sprite *);
static u8 CreateGameFreakLogoSprites(s16, s16, u8);
static void SpriteCB_IntroOpposingPokemon(struct Sprite *);
static void SpriteCB_IntroTrainer(struct Sprite *);
static void SpriteCB_IntroOpposingPokemonBattle(struct Sprite *);
static void SpriteCB_IntroPartnerPokemonBattle(struct Sprite *);
static void CreateIntroBurstParticles(u16, u16);
static void CreateIntroOpposingAttackEffect(u8);
static void CreateIntroOrbitingParticles(u8);
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

static void LoadCopyrightGraphics(u16 tilesetAddress, u16 tilemapAddress, u16 paletteAddress)
{
    LZ77UnCompVram(gIntroCopyright_Gfx, (void *)(VRAM + tilesetAddress));
    LoadPalette(gIntroCopyright_Pal, paletteAddress, 0x20);
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
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, FADE_COLOR_WHITE);
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

#define sBigDropSpriteId data[0]
#define tBg2PosHi data[1]
#define tBg2PosLo data[2]
#define tBg1PosHi data[3]
#define tBg1PosLo data[4]
#define tBg0PosHi data[5]
#define tBg0PosLo data[6]

void Task_Scene1_Load(u8 taskId)
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
    LoadCompressedSpriteSheet(&sSpriteSheet_LatiSilhouette[0]);
    LoadSpritePalettes(sSpritePalettes_Intro1);
    CpuCopy16(gPlttBufferUnfaded + 0x100, gPlttBufferUnfaded + 0x1F0, 0x20);
    CpuCopy16(gPlttBufferUnfaded + 0x100, gPlttBufferUnfaded + 0x1E1, 0x1E);
    CpuCopy16(gPlttBufferUnfaded + 0x100, gPlttBufferUnfaded + 0x1D2, 0x1C);
    CpuCopy16(gPlttBufferUnfaded + 0x100, gPlttBufferUnfaded + 0x1C3, 0x1A);
    CpuCopy16(gPlttBufferUnfaded + 0x100, gPlttBufferUnfaded + 0x1B4, 0x18);
    CpuCopy16(gPlttBufferUnfaded + 0x100, gPlttBufferUnfaded + 0x1A5, 0x16);
    CpuCopy16(gPlttBufferUnfaded + 0x100, gPlttBufferUnfaded + 0x196, 0x14);
    gTasks[taskId].sBigDropSpriteId = CreateWaterDrop(236, -14, 0x200, 1, 0x78, FALSE);
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
    if (gIntroFrameCounter == TIMER_BIG_DROP_START)
        gSprites[gTasks[taskId].sBigDropSpriteId].sState = WATER_DROP_SLIDE;

    //drop rock
    if (gIntroFrameCounter == TIMER_BIG_DROP_FALLS)
        gSprites[gTasks[taskId].sBigDropSpriteId].sState = WATER_DROP_FALL;

    if (gIntroFrameCounter == TIMER_SMALL_DROP_1)
        CreateWaterDrop(48, 0, 0x400, 5, 0x70, TRUE);
    if (gIntroFrameCounter == TIMER_SMALL_DROP_2)
        CreateWaterDrop(200, 60, 0x400, 9, 0x80, TRUE);

    if (gIntroFrameCounter == TIMER_GAME_FREAK_LOGO)
        CreateGameFreakLogoSprites(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, CreateTask(Task_AnimateGameFreakLogoBlend, 0));

    if (gIntroFrameCounter > TIMER_START_SCENE_1_PAN)
    {
        gTasks[taskId].tBg2PosHi = 0x50;
        gTasks[taskId].tBg2PosLo = 0;
        gTasks[taskId].tBg1PosHi = 0x18;
        gTasks[taskId].tBg1PosLo = 0;
        gTasks[taskId].tBg0PosHi = 0x28;
        gTasks[taskId].tBg0PosLo = 0;
        gTasks[taskId].func = Task_Scene1_PanUpAndShowLati;
    }
}

static void Task_Scene1_PanUpAndShowLati(u8 taskId)
{
    if (gIntroFrameCounter < TIMER_END_SCENE_1_PAN)
    {
        s32 offset;

        //slide backgrounds downward
        offset = (gTasks[taskId].tBg2PosHi << 16) + (u16)gTasks[taskId].tBg2PosLo - 0xC000;
        gTasks[taskId].tBg2PosHi = offset >> 16;
        gTasks[taskId].tBg2PosLo = offset;
        REG_BG2VOFS = gTasks[taskId].tBg2PosHi;
        offset = (gTasks[taskId].tBg1PosHi << 16) + (u16)gTasks[taskId].tBg1PosLo - 0x10000;
        gTasks[taskId].tBg1PosHi = offset >> 16;
        gTasks[taskId].tBg1PosLo = offset;
        REG_BG1VOFS = gTasks[taskId].tBg1PosHi;
        offset = (gTasks[taskId].tBg0PosHi << 16) + (u16)gTasks[taskId].tBg0PosLo - 0x18000;
        gTasks[taskId].tBg0PosHi = offset >> 16;
        gTasks[taskId].tBg0PosLo = offset;
        REG_BG0VOFS = gTasks[taskId].tBg0PosHi;

        //show Lati@s sprite
        if (gIntroFrameCounter == TIMER_LATI_SILHOUETTE_APPEAR)
        {
            u8 spriteId = CreateSprite(&sSpriteTemplate_LatiSilhouette, 200, 160, 10);

            gSprites[spriteId].invisible = TRUE;
        }
    }
    else
    {
        //fade to white
        if (gIntroFrameCounter > TIMER_SCENE_1_FADE_OUT)
        {
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, FADE_COLOR_WHITE);
            gTasks[taskId].func = Task_Scene1_End;
        }
    }
}

static void Task_Scene1_End(u8 taskId)
{
    if (gIntroFrameCounter > TIMER_START_SCENE_2)
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

#define tBgAnimTaskId data[0]
#define tPlayerSpriteId data[1]
#define tLatiSpriteId data[2]
#define tLatiTimer data[3]

enum
{
    PLAYER_BICYCLE_MOVE_FORWARD,
    PLAYER_BICYCLE_DRIFT_BACK,
    PLAYER_BICYCLE_MOVE_BACKWARD,
    PLAYER_BICYCLE_HOLD_POSITION,
    PLAYER_BICYCLE_EXIT,
};

enum
{
    LATI_WAIT,
    LATI_ENTER,
    LATI_APPROACH_PLAYER,
    LATI_EXIT,
};

static void Task_Scene2_CreateSprites(u8 taskId)
{
    u8 spriteId;

    if (sIntroCharacterGender == MALE)
        LoadCompressedSpriteSheet(gSpriteSheet_IntroBrendan);
    else
        LoadCompressedSpriteSheet(gSpriteSheet_IntroMay);
    LoadCompressedSpriteSheet(gSpriteSheet_IntroBicycle);
#ifdef SAPPHIRE
    LoadCompressedSpriteSheet(gSpriteSheet_IntroLatias);
#else
    LoadCompressedSpriteSheet(gSpriteSheet_IntroLatios);
#endif
    LoadSpritePalettes(gSpritePalettes_IntroPlayerLati);
    if (sIntroCharacterGender == MALE)
        spriteId = CreateIntroBrendanSprite(0x110, 100);
    else
        spriteId = CreateIntroMaySprite(0x110, 100);
    gSprites[spriteId].callback = SpriteCB_PlayerOnBicycle;
    gSprites[spriteId].anims = sAnims_PlayerBicycle;
    gTasks[taskId].tPlayerSpriteId = spriteId;
#ifdef SAPPHIRE
    spriteId = CreateIntroLatiasSprite(-0x40, 0x3C);
#else
    spriteId = CreateIntroLatiosSprite(-0x40, 0x3C);
#endif
    gSprites[spriteId].callback = SpriteCB_Lati;
    gTasks[taskId].tLatiSpriteId = spriteId;
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, FADE_COLOR_WHITE);
    SetVBlankCallback(VBlankCB_Intro);
#ifdef SAPPHIRE
    gTasks[taskId].tBgAnimTaskId = CreateBicycleBgAnimationTask(0, 0x4000, 0x40, 0x10);
    SetIntroPart2BgCnt(0);
#else
    gTasks[taskId].tBgAnimTaskId = CreateBicycleBgAnimationTask(1, 0x4000, 0x400, 0x10);
    SetIntroPart2BgCnt(1);
#endif
    gTasks[taskId].func = Task_Scene2_BikeRide;
}

static void Task_Scene2_BikeRide(u8 taskId)
{
    s16 sineIndex;
    u16 sine;

    if (gIntroFrameCounter > TIMER_SCENE_2_FADE_OUT)
    {
        BeginNormalPaletteFade(0xFFFFFFFF, 16, 0, 16, FADE_COLOR_WHITE);
        gTasks[taskId].func = Task_Scene2_End;
    }
    if (gIntroFrameCounter == TIMER_PLAYER_DRIFT_BACK)
        gSprites[gTasks[taskId].tPlayerSpriteId].sState = PLAYER_BICYCLE_DRIFT_BACK;
    if (gIntroFrameCounter == TIMER_PLAYER_MOVE_FORWARD)
        gSprites[gTasks[taskId].tPlayerSpriteId].sState = PLAYER_BICYCLE_MOVE_FORWARD;
    if (gIntroFrameCounter == TIMER_LATI_ENTER)
        gSprites[gTasks[taskId].tLatiSpriteId].sState = LATI_ENTER;
    if (gIntroFrameCounter == TIMER_PLAYER_MOVE_BACKWARD)
        gSprites[gTasks[taskId].tPlayerSpriteId].sState = PLAYER_BICYCLE_MOVE_BACKWARD;
    if (gIntroFrameCounter == TIMER_PLAYER_HOLD_POSITION)
        gSprites[gTasks[taskId].tPlayerSpriteId].sState = PLAYER_BICYCLE_HOLD_POSITION;
    if (gIntroFrameCounter == TIMER_PLAYER_EXIT)
        gSprites[gTasks[taskId].tPlayerSpriteId].sState = PLAYER_BICYCLE_EXIT;

    //TODO: Clean this up
    sineIndex = (((u16)gTasks[taskId].tLatiTimer << 16) >> 18) & 0x7F;
    sine = Sin(sineIndex, 48);
    gIntroCredits_MovingSceneryVOffset = sine;
    if (gTasks[taskId].tLatiTimer < 512)
        gTasks[taskId].tLatiTimer++;
#ifdef SAPPHIRE
    CycleSceneryPalette(0);
#else
    CycleSceneryPalette(1);
#endif
}

static void Task_Scene2_End(u8 taskId)
{
    if (gIntroFrameCounter > TIMER_START_SCENE_3)
    {
        DestroyTask(gTasks[taskId].tBgAnimTaskId);
        gTasks[taskId].func = Task_Scene3_Load;
    }
}

#define tAlpha data[0]
#define tZoomDiv data[1]
#define tZoomDivSpeed data[2]
#define tBattleWindowHeight data[0]
#define tBattleBgTaskId data[15]
#define tTrainerSpriteId data[1]
#define tSharpedoSpriteId data[2]
#define tDuskullSpriteId data[3]
#define tMudkipSpriteId data[4]
#define tTorchicSpriteId data[5]

enum
{
    INTRO_BATTLE_BG_HIDDEN,
    INTRO_BATTLE_BG_PARTNER_BATTLE = 2,
    INTRO_BATTLE_BG_OPPONENT_EXIT,
    INTRO_BATTLE_BG_TRAINER_THROW,
    INTRO_BATTLE_BG_PARTNER_BATTLE_SCROLL = 20,
    INTRO_BATTLE_BG_OPPONENT_EXIT_SCROLL = 0x1E,
    INTRO_BATTLE_BG_TRAINER_THROW_SCROLL = 0x28,
    INTRO_BATTLE_BG_DISABLED = 0xFF,
};

static void Task_Scene3_Load(u8 taskId)
{
    IntroResetGpuRegs();
    LZ77UnCompVram(sIntroPokeball_Gfx, (void *)VRAM);
    LZ77UnCompVram(sIntroPokeball_Tilemap, (void *)(VRAM + 0x4000));
    LoadPalette(sIntroPokeball_Pal, 0, 0x200);
    gTasks[taskId].tAlpha = 0;
    gTasks[taskId].tZoomDiv = 0;
    gTasks[taskId].tZoomDivSpeed = 0;
    gTasks[taskId].data[3] = 0;
    PanFadeAndZoomScreen(0x78, 0x50, 0, 0);
    ResetSpriteData();
    FreeAllSpritePalettes();
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, FADE_COLOR_WHITE);
    REG_BG2CNT = BGCNT_PRIORITY(3) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(8) | BGCNT_256COLOR | BGCNT_AFF256x256;
    REG_DISPCNT = DISPCNT_MODE_1 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
    gTasks[taskId].func = Task_Scene3_SpinPokeball;
    gIntroFrameCounter = 0;
    m4aSongNumStart(0x1BA);
}

static void Task_Scene3_SpinPokeball(u8 taskId)
{
    gTasks[taskId].tAlpha += 0x400;
    if (gTasks[taskId].tZoomDiv <= 0x6BF)
    {
        gTasks[taskId].tZoomDiv += gTasks[taskId].tZoomDivSpeed;
        gTasks[taskId].tZoomDivSpeed++;
    }
    else
    {
        gTasks[taskId].func = Task_Scene3_WaitForBattle;
    }
    PanFadeAndZoomScreen(0x78, 0x50, 0x10000 / gTasks[taskId].tZoomDiv, gTasks[taskId].tAlpha);
    if (gIntroFrameCounter == TIMER_POKEBALL_FADE_OUT)
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, FADE_COLOR_WHITE);
}

static void Task_Scene3_WaitForBattle(u8 taskId)
{
    if (gIntroFrameCounter > TIMER_LOAD_BATTLE_STREAKS)
        gTasks[taskId].func = Task_Scene3_LoadBattleStreaks;
}

static void Task_Scene3_LoadBattleStreaks(u8 taskId)
{
    u16 i;

    IntroResetGpuRegs();
    for (i = 0; i < 32; i++)
    {
        sIntroBattleTilemapBuffer[0][i] = 0;
        sIntroBattleTilemapBuffer[1][i] = 17;
        sIntroBattleTilemapBuffer[2][i] = 34;
    }
    DmaCopy16Defvars(3, sIntroBattleTilemapBuffer, (void *)(VRAM + 0x0), 0x60);
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
    SetIntroBattleBackgroundColor(INTRO_BATTLE_BG_RED);
    gPlttBufferUnfaded[0xF2] = RGB_BLACK;
    gPlttBufferFaded[0xF2] = RGB_BLACK;
    LZ77UnCompVram(sIntroStreaks_Gfx, (void *)(VRAM + 0x4000));
    LZ77UnCompVram(sIntroStreaks_Tilemap, (void *)(VRAM + 0x7000));
    LoadPalette(sIntroStreaks_Pal, 0, 0x20);
    ResetSpriteData();
    FreeAllSpritePalettes();
    gReservedSpritePaletteCount = 8;
    LoadCompressedSpriteSheet(&sSpriteSheet_IntroBattlePokeball[0]);
    LoadCompressedSpriteSheet(&sSpriteSheet_IntroBattleEffects[0]);
    LoadCompressedSpritePalette(&sSpritePalette_IntroBattlePokeball[0]);
    LoadSpritePalettes(sSpritePalettes_IntroBattleEffects);
    gTasks[taskId].func = Task_Scene3_InitBattleWindow;
}

static void Task_Scene3_InitBattleWindow(u8 taskId)
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
    gTasks[taskId].tBattleBgTaskId = CreateTask(Task_Scene3_AnimateBattleBackground, 0);
    gTasks[gTasks[taskId].tBattleBgTaskId].tState = INTRO_BATTLE_BG_HIDDEN;
    gTasks[taskId].tBattleWindowHeight = 0;
    gTasks[taskId].func = Task_Scene3_OpenBattleWindow;
}

static void Task_Scene3_OpenBattleWindow(u8 taskId)
{
    u16 windowHeight = gTasks[taskId].tBattleWindowHeight;

    if (gTasks[taskId].tBattleWindowHeight != 32)
    {
        u32 windowTop; //needed to match for some reason

        gTasks[taskId].tBattleWindowHeight += 4;
        REG_WIN0V = (gTasks[taskId].tBattleWindowHeight * 256) - (windowTop = windowHeight - 0x9C);
    }
    else
    {
        REG_WIN0V = 0x2080;
        gTasks[taskId].func = Task_Scene3_FinishOpeningWindow;
    }
}

static void Task_Scene3_FinishOpeningWindow(u8 taskId)
{
    gTasks[taskId].func = Task_Scene3_StartPokemonBattle;
}

static void Task_Scene3_StartPokemonBattle(u8 taskId)
{
    sIntroBattleFinished = 0;
    gTasks[taskId].func = Task_Scene3_PokemonBattle;
}

static void Task_Scene3_PokemonBattle(u8 taskId)
{
    u8 spriteId;

    if (gIntroFrameCounter == TIMER_SHARPEDO_INTRO)
    {
        spriteId = CreateIntroBattleMonSprite(SPECIES_SHARPEDO, 0xF0, 0xA0, 5, 1);
        gSprites[spriteId].callback = SpriteCB_IntroOpposingPokemon;
        gSprites[spriteId].data[1] = 1;
        gSprites[spriteId].data[2] = 0;
    }
    if (gIntroFrameCounter == TIMER_DUSKULL_INTRO)
    {
        spriteId = CreateIntroBattleMonSprite(SPECIES_DUSKULL, 0, 0xA0, 4, 1);
        gSprites[spriteId].callback = SpriteCB_IntroOpposingPokemon;
        gSprites[spriteId].data[1] = 2;
        gSprites[spriteId].data[2] = 1;
    }
    if (gIntroFrameCounter == TIMER_TRAINER_INTRO)
    {
        SetIntroBattleBackgroundColor(INTRO_BATTLE_BG_GREEN);
        spriteId = CreateIntroTrainerSprite(sIntroCharacterGender, 0x110, 0x60, 6);
        gSprites[spriteId].callback = SpriteCB_IntroTrainer;
        gTasks[taskId].tTrainerSpriteId = spriteId;
    }
    if (gIntroFrameCounter == TIMER_TRAINER_THROW_POKEBALLS)
    {
        gTasks[gTasks[taskId].tBattleBgTaskId].tState = INTRO_BATTLE_BG_TRAINER_THROW;
        gSprites[gTasks[taskId].tTrainerSpriteId].sState = 2;
    }
    if (gIntroFrameCounter == TIMER_TRAINER_RETURN)
    {
        gTasks[gTasks[taskId].tBattleBgTaskId].tState = INTRO_BATTLE_BG_HIDDEN;
        gSprites[gTasks[taskId].tTrainerSpriteId].sState = 4;
    }
    if (gIntroFrameCounter == TIMER_TRAINER_FADE)
    {
        BeginNormalPaletteFade(0xFF0000, 0, 16, 0, RGB(31, 23, 31));
    }
    if (gIntroFrameCounter == TIMER_TRAINER_TRANSFORM_START)
    {
        gSprites[gTasks[taskId].tTrainerSpriteId].sState = 5;
    }
    if (gIntroFrameCounter == TIMER_TRAINER_TRANSFORM_END)
    {
        gSprites[gTasks[taskId].tTrainerSpriteId].sState = 6;
        gTasks[gTasks[taskId].tBattleBgTaskId].tState = INTRO_BATTLE_BG_PARTNER_BATTLE;
    }
    if (gIntroFrameCounter == TIMER_SHARPEDO_BATTLE)
    {
        SetIntroBattleBackgroundColor(INTRO_BATTLE_BG_RED);
        spriteId = CreateIntroBattleMonSprite(SPECIES_SHARPEDO, 0xD0, 8, 5, 1);
        gSprites[spriteId].callback = SpriteCB_IntroOpposingPokemonBattle;
        gTasks[taskId].tSharpedoSpriteId = spriteId;
        CreateIntroOpposingAttackEffect(spriteId);
    }
    if (gIntroFrameCounter == TIMER_DUSKULL_BATTLE)
    {
        spriteId = CreateIntroBattleMonSprite(SPECIES_DUSKULL, 0xF8, 0x10, 4, 1);
        gSprites[spriteId].callback = SpriteCB_IntroOpposingPokemonBattle;
        gTasks[taskId].tDuskullSpriteId = spriteId;
        CreateIntroOrbitingParticles(spriteId);
    }
    if (gIntroFrameCounter == TIMER_OPPONENTS_EXIT)
    {
        gSprites[gTasks[taskId].tSharpedoSpriteId].sState = 2;
        gSprites[gTasks[taskId].tDuskullSpriteId].sState = 2;
        gTasks[gTasks[taskId].tBattleBgTaskId].tState = INTRO_BATTLE_BG_OPPONENT_EXIT;
    }
    if (gIntroFrameCounter == TIMER_MUDKIP_BATTLE)
    {
        SetIntroBattleBackgroundColor(INTRO_BATTLE_BG_GREEN);
        spriteId = CreateIntroBattleMonSprite(SPECIES_MUDKIP, 0x20, 0x98, 0, 0);
        gSprites[spriteId].callback = SpriteCB_IntroPartnerPokemonBattle;
        gTasks[taskId].tMudkipSpriteId = spriteId;
        InitIntroMudkipAttackAnim(spriteId);
    }
    if (gIntroFrameCounter == TIMER_TORCHIC_BATTLE)
    {
        spriteId = CreateIntroBattleMonSprite(SPECIES_TORCHIC, -8, 0x90, 1, 0);
        gSprites[spriteId].callback = SpriteCB_IntroPartnerPokemonBattle;
        gTasks[taskId].tTorchicSpriteId = spriteId;
        InitIntroTorchicAttackAnim(spriteId);
    }
    if (gIntroFrameCounter == TIMER_BATTLE_FINISH)
    {
        sIntroBattleFinished = 1;
        gSprites[gTasks[taskId].tMudkipSpriteId].sState = 2;
        gSprites[gTasks[taskId].tTorchicSpriteId].sState = 2;
        gTasks[gTasks[taskId].tBattleBgTaskId].tState = INTRO_BATTLE_BG_HIDDEN;
    }
    if (gIntroFrameCounter == TIMER_BATTLE_FLASH)
    {
        SetIntroBattleBackgroundColor(INTRO_BATTLE_BG_BLUE);
        gSprites[gTasks[taskId].tSharpedoSpriteId].sState = 3;
        gSprites[gTasks[taskId].tDuskullSpriteId].sState = 3;
        gSprites[gTasks[taskId].tMudkipSpriteId].sState = 3;
        gSprites[gTasks[taskId].tTorchicSpriteId].sState = 3;
        spriteId = CreateSprite(&sSpriteTemplate_IntroBattleFlash, 0x78, 0x50, 15);
        gSprites[spriteId].invisible = TRUE;
    }
    if (gIntroFrameCounter == TIMER_BATTLE_BLAST_SOUND)
        PlaySE(SE_INTRO_BLAST);
    if (gIntroFrameCounter == TIMER_BATTLE_FADE_OUT)
        BeginNormalPaletteFade(0xFFFFFFFF, 4, 0, 16, FADE_COLOR_WHITE);
    if (gIntroFrameCounter == TIMER_END_INTRO)
        gTasks[taskId].func = Task_EndIntroMovie;
}

static void Task_EndIntroMovie(u8 taskId)
{
    DestroyTask(taskId);
    SetMainCallback2(MainCB2_EndIntro);
}

#define tBattleBgState data[0]
#define tBattleBgVOffset data[1]
#define tBattleBgHOffset data[2]
#define tBattleBgSpeed data[3]
#define tBattleBgTimer data[15]

static void Task_Scene3_AnimateBattleBackground(u8 taskId)
{
#define BG2_FLAGS (BGCNT_PRIORITY(3) | BGCNT_CHARBASE(1) | BGCNT_SCREENBASE(14) | BGCNT_16COLOR | BGCNT_TXT256x256)
#define DISPCNT_FLAGS (DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG2_ON | DISPCNT_BG3_ON | DISPCNT_OBJ_ON | DISPCNT_WIN0_ON)

    gTasks[taskId].tBattleBgTimer++;
    switch (gTasks[taskId].tBattleBgState)
    {
    case INTRO_BATTLE_BG_HIDDEN:
        REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG3_ON | DISPCNT_OBJ_ON | DISPCNT_WIN0_ON;
        REG_BG2CNT = 0;
        gTasks[taskId].tBattleBgState = INTRO_BATTLE_BG_DISABLED;
        break;
    case INTRO_BATTLE_BG_PARTNER_BATTLE:
        BeginNormalPaletteFade(0x1, 0, 16, 0, FADE_COLOR_WHITE);
        REG_BG2CNT = BG2_FLAGS;
        REG_DISPCNT = DISPCNT_FLAGS;
        gTasks[taskId].tBattleBgVOffset = 0;
        gTasks[taskId].tBattleBgHOffset = 0;
        gTasks[taskId].tBattleBgState = INTRO_BATTLE_BG_PARTNER_BATTLE_SCROLL;
        //fall through
    case INTRO_BATTLE_BG_PARTNER_BATTLE_SCROLL:
        REG_BG2VOFS = gTasks[taskId].tBattleBgVOffset;
        REG_BG2HOFS = gTasks[taskId].tBattleBgHOffset;
        gTasks[taskId].tBattleBgVOffset += 6;
        gTasks[taskId].tBattleBgHOffset -= 8;
        break;
    case INTRO_BATTLE_BG_OPPONENT_EXIT:
        BeginNormalPaletteFade(0x1, 0, 16, 0, FADE_COLOR_WHITE);
        REG_BG2CNT = BG2_FLAGS;
        REG_DISPCNT = DISPCNT_FLAGS;
        gTasks[taskId].tBattleBgVOffset = 0;
        gTasks[taskId].tBattleBgHOffset = 0;
        gTasks[taskId].tBattleBgState = INTRO_BATTLE_BG_OPPONENT_EXIT_SCROLL;
        //fall through
    case INTRO_BATTLE_BG_OPPONENT_EXIT_SCROLL:
        REG_BG2VOFS = gTasks[taskId].tBattleBgVOffset;
        REG_BG2HOFS = gTasks[taskId].tBattleBgHOffset;
        gTasks[taskId].tBattleBgVOffset -= 6;
        gTasks[taskId].tBattleBgHOffset += 8;
        break;
    case INTRO_BATTLE_BG_TRAINER_THROW:
        BeginNormalPaletteFade(0x1, 5, 0, 16, RGB(23, 31, 13));
        REG_BG2CNT = BG2_FLAGS;
        REG_DISPCNT = DISPCNT_FLAGS;
        gTasks[taskId].tBattleBgVOffset = 0;
        gTasks[taskId].tBattleBgHOffset = 0;
        gTasks[taskId].tBattleBgSpeed = 8;
        gTasks[taskId].tBattleBgState = INTRO_BATTLE_BG_TRAINER_THROW_SCROLL;
        //fall through
    case INTRO_BATTLE_BG_TRAINER_THROW_SCROLL:
        REG_BG2VOFS = gTasks[taskId].tBattleBgVOffset;
        REG_BG2HOFS = gTasks[taskId].tBattleBgHOffset;
        gTasks[taskId].tBattleBgVOffset -= gTasks[taskId].tBattleBgSpeed;
        gTasks[taskId].tBattleBgHOffset += gTasks[taskId].tBattleBgSpeed;
        if (!(gTasks[taskId].tBattleBgTimer & 7) && gTasks[taskId].tBattleBgSpeed != 0)
            gTasks[taskId].tBattleBgSpeed--;
        break;
    case INTRO_BATTLE_BG_DISABLED:  //needed to prevent jump table optimization
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

static void Task_AnimateGameFreakLogoBlend(u8 taskId)
{
    switch (gTasks[taskId].tState)
    {
    default:
    case 0:
        REG_BLDCNT = 0x3F50;
        REG_BLDALPHA = 0x1000;
        REG_BLDY = 0;
        gTasks[taskId].data[1] = 0x40;
        gTasks[taskId].tState = 1;
        break;
    case 1:
        if (gTasks[taskId].data[1] != 0)
        {
            u8 blendIndex;

            gTasks[taskId].data[1]--;
            blendIndex = gTasks[taskId].data[1] / 2;
            REG_BLDALPHA = gTitleScreenAlphaBlend[blendIndex];
        }
        else
        {
            REG_BLDALPHA = gTitleScreenAlphaBlend[0];
            gTasks[taskId].data[1] = 0x80;
            gTasks[taskId].tState++;
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
            gTasks[taskId].tState++;
        }
        break;
    case 3:
        if (gTasks[taskId].data[1] <= 0x3D)
        {
            u8 blendIndex;

            gTasks[taskId].data[1]++;
            blendIndex = gTasks[taskId].data[1] / 2;
            REG_BLDALPHA = gTitleScreenAlphaBlend[blendIndex];
        }
        else
        {
            //_0813CDE0
            REG_BLDALPHA = gTitleScreenAlphaBlend[0x1F];
            gTasks[taskId].data[1] = 0x10;
            gTasks[taskId].tState++;
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

static u16 CreateIntroBattleMonSprite(u16 species, s16 x, s16 y, u16 spriteSlot, u8 useFrontPic)
{
    const u8 *lzPaletteData;
    u8 spriteId;

    if (useFrontPic)
        LoadSpecialPokePic(&gMonFrontPicTable[species], gMonFrontPicCoords[species].size, gMonFrontPicCoords[species].y_offset, (void *)EWRAM, gHallOfFameGfxBufferPtrs[spriteSlot], species, 0, 1);
    else
        LoadSpecialPokePic(&gMonBackPicTable[species], gMonBackPicCoords[species].size, gMonBackPicCoords[species].y_offset, (void *)EWRAM, gHallOfFameGfxBufferPtrs[spriteSlot], species, 0, 0);
    lzPaletteData = GetMonSpritePalFromSpeciesAndPersonality(species, 0, 0xFFFF);
    LoadCompressedPalette(lzPaletteData, 0x100 + spriteSlot * 0x10, 0x20);
    PrepareHallOfFameMonPicSpriteTemplate(spriteSlot, spriteSlot);
    spriteId = CreateSprite(&gCreatingSpriteTemplate, x, y, (spriteSlot + 1) * 4);
    gSprites[spriteId].oam.paletteNum = spriteSlot;
    gSprites[spriteId].oam.priority = 1;
    return spriteId;
}

static u8 CreateIntroTrainerSprite(u16 trainerPicId, u16 x, u16 y, u16 spriteSlot)
{
    u8 spriteId;

    DecompressPicFromTable(&gTrainerBackPicTable[trainerPicId], gTrainerBackPicCoords[trainerPicId].size, gTrainerBackPicCoords[trainerPicId].y_offset, (void *)EWRAM, gHallOfFameGfxBufferPtrs[spriteSlot], trainerPicId);
    LoadCompressedPalette(gTrainerBackPicPaletteTable[trainerPicId].data, 0x100 + spriteSlot * 0x10, 0x20);
    PrepareHallOfFameTrainerPicSpriteTemplate(spriteSlot, spriteSlot);
    gCreatingSpriteTemplate.anims = sAnims_IntroTrainer;
    spriteId = CreateSprite(&gCreatingSpriteTemplate, x, y, 1);
    gSprites[spriteId].oam.paletteNum = spriteSlot;
    gSprites[spriteId].oam.priority = 1;
    return spriteId;
}

static void SetIntroBattleBackgroundColor(u8 colorId)
{
    u16 color;

    switch (colorId)
    {
    default:
    case INTRO_BATTLE_BG_GREEN:
        color = RGB(22, 31, 15);
        break;
    case INTRO_BATTLE_BG_RED:
        color = RGB(31, 14, 12);
        break;
    case INTRO_BATTLE_BG_BLUE:
        color = RGB(12, 12, 20);
        break;
    }
    gPlttBufferUnfaded[241] = color;
    gPlttBufferFaded[241] = color;
}

static void SpriteCB_WaterDrop_Ripple(struct Sprite *sprite)
{
    u8 paletteNum;

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
            paletteNum = (sprite->data[2] - 192) / 128 + 9;
            if (paletteNum > 15)
                paletteNum = 15;
            sprite->oam.paletteNum = paletteNum;
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
        StartSpriteAnim(sprite, DROP_ANIM_RIPPLE);
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
    if (sprite->sState != WATER_DROP_WAIT)
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
    if (sprite->sState != WATER_DROP_FALL)
    {
        s16 sineIndex;

        sprite->data[4] += 8;
        sineIndex = gSineTable[(u8)sprite->data[4]] / 16 + 64;
        sprite->x2 = gSineTable[(u8)(sineIndex + 64)] / 64;
        sprite->y2 = gSineTable[(u8)sineIndex] / 64;
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
        StartSpriteAnim(sprite, DROP_ANIM_RIPPLE);
        sprite->data[2] = 1024;
        sprite->data[3] = 8 * (sprite->data[1] & 3);
        sprite->callback = SpriteCB_WaterDrop_Ripple;
        sprite->oam.shape = 1;
        sprite->oam.size = 3;
        CalcCenterToCornerVec(sprite, 1, 3, 2);
    }
}

// Identical to SpriteCB_WaterDrop_Fall.
// Used by the second and third drops to skip the leaf slide.
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
        StartSpriteAnim(sprite, DROP_ANIM_RIPPLE);
        sprite->data[2] = 1024;
        sprite->data[3] = 8 * (sprite->data[1] & 3);
        sprite->callback = SpriteCB_WaterDrop_Ripple;
        sprite->oam.shape = 1;
        sprite->oam.size = 3;
        CalcCenterToCornerVec(sprite, 1, 3, 2);
    }
}

static u8 CreateWaterDrop(s16 x, s16 y, u16 scale, u16 matrixNum, u16 fallY, u8 fallImmediately)
{
    u8 spriteId;
    u8 reflectionSpriteId;

    spriteId = CreateSprite(&sSpriteTemplate_WaterDrop, x, y, 0);
    gSprites[spriteId].sState = WATER_DROP_WAIT;
    gSprites[spriteId].data[7] = 0;
    gSprites[spriteId].data[1] = matrixNum;
    gSprites[spriteId].data[2] = scale;
    gSprites[spriteId].data[3] = scale;
    gSprites[spriteId].data[5] = fallY;
    gSprites[spriteId].data[6] = scale;
    gSprites[spriteId].oam.affineMode = 3;
    gSprites[spriteId].oam.matrixNum = matrixNum;
    CalcCenterToCornerVec(&gSprites[spriteId], 0, 2, 2);
    StartSpriteAnim(&gSprites[spriteId], DROP_ANIM_REFLECTION);
    if (!fallImmediately)
        gSprites[spriteId].callback = SpriteCB_WaterDrop;
    else
        gSprites[spriteId].callback = SpriteCB_WaterDropShort;
    reflectionSpriteId = spriteId;

    spriteId = CreateSprite(&sSpriteTemplate_WaterDrop, x, y, 0);
    gSprites[spriteId].data[7] = reflectionSpriteId;
    gSprites[spriteId].data[1] = matrixNum + 1;
    gSprites[spriteId].oam.affineMode = 3;
    gSprites[spriteId].oam.matrixNum = matrixNum + 1;
    CalcCenterToCornerVec(&gSprites[spriteId], 0, 2, 2);
    gSprites[spriteId].callback = SpriteCB_WaterDropHalf;

    spriteId = CreateSprite(&sSpriteTemplate_WaterDrop, x, y, 0);
    gSprites[spriteId].data[7] = reflectionSpriteId;
    gSprites[spriteId].data[1] = matrixNum + 2;
    StartSpriteAnim(&gSprites[spriteId], DROP_ANIM_LOWER_HALF);
    gSprites[spriteId].oam.affineMode = 3;
    gSprites[spriteId].oam.matrixNum = matrixNum + 2;
    CalcCenterToCornerVec(&gSprites[spriteId], 0, 2, 2);
    gSprites[spriteId].callback = SpriteCB_WaterDropHalf;

    SetOamMatrix(matrixNum, scale + 32, 0, 0, scale + 32);
    SetOamMatrix(matrixNum + 1, scale + 32, 0, 0, scale + 32);
    SetOamMatrix(matrixNum + 2, scale + 32, 0, 0, 2 * (scale + 32));

    return reflectionSpriteId;
}

static void SpriteCB_PlayerOnBicycle(struct Sprite *sprite)
{
    switch (sprite->sState)
    {
    case PLAYER_BICYCLE_MOVE_FORWARD:
        StartSpriteAnimIfDifferent(sprite, PLAYER_BICYCLE_ANIM_FAST);
        sprite->x--;
        break;
    case PLAYER_BICYCLE_DRIFT_BACK:
        StartSpriteAnimIfDifferent(sprite, PLAYER_BICYCLE_ANIM_FAST);
        if (gIntroFrameCounter & 7)
            return;
        sprite->x++;
        break;
    case PLAYER_BICYCLE_MOVE_BACKWARD:
        StartSpriteAnimIfDifferent(sprite, PLAYER_BICYCLE_ANIM_LOOK_BACK);
        if (sprite->x <= 120 || (gIntroFrameCounter & 7))
            sprite->x++;
        break;
    case PLAYER_BICYCLE_HOLD_POSITION:
        StartSpriteAnimIfDifferent(sprite, PLAYER_BICYCLE_ANIM_LOOK_FORWARD);
        break;
    case PLAYER_BICYCLE_EXIT:
        StartSpriteAnimIfDifferent(sprite, PLAYER_BICYCLE_ANIM_FAST);
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

static void SpriteCB_Lati(struct Sprite *sprite)
{
    switch (sprite->sState)
    {
    case LATI_WAIT:
        break;
    case LATI_ENTER:
        if (sprite->x2 + sprite->x < 304)
            sprite->x2 += 8;
        else
            sprite->sState = LATI_APPROACH_PLAYER;
        break;
    case LATI_APPROACH_PLAYER:
        if (sprite->x2 + sprite->x > 120)
            sprite->x2 -= 1;
        else
            sprite->sState = LATI_EXIT;
        break;
    case LATI_EXIT:
        if (sprite->x2 > 0)
            sprite->x2 -= 2;
        break;
    }
    sprite->y2 = Sin((u8)sprite->data[1], 8) - gIntroCredits_MovingSceneryVOffset;
    sprite->data[1] += 4;
}

#define sBlendTaskId data[0]

static void SpriteCB_LogoLetter(struct Sprite *sprite)
{
    if (gTasks[sprite->sBlendTaskId].tState == 0)
    {
        sprite->invisible = TRUE;
    }
    else if (gTasks[sprite->sBlendTaskId].tState != 4)
    {
        sprite->invisible = FALSE;
    }
    else
    {
        DestroySprite(sprite);
    }
}

static u8 CreateGameFreakLogoSprites(s16 x, s16 y, u8 blendTaskId)
{
    u8 spriteId;
    u16 i;

    for (i = 0; i < 9; i++)
    {
        spriteId = CreateSprite(&sSpriteTemplate_GameFreakLetter, sGameFreakLetterData[i][1] + x, y - 4, 0);
        gSprites[spriteId].sBlendTaskId = blendTaskId;
        StartSpriteAnim(&gSprites[spriteId], sGameFreakLetterData[i][0]);
    }
    for (i = 0; i < 8; i++)
    {
        spriteId = CreateSprite(&sSpriteTemplate_PresentsLetter, sPresentsLetterData[i][1] + x, y + 12, 0);
        gSprites[spriteId].sBlendTaskId = blendTaskId;
        StartSpriteAnim(&gSprites[spriteId], sPresentsLetterData[i][0]);
    }
    spriteId = CreateSprite(&sSpriteTemplate_GameFreakLogo, 120, y - 4, 0);
    gSprites[spriteId].sBlendTaskId = blendTaskId;

    return spriteId;
}

#undef sBlendTaskId

static void SpriteCB_LatiSilhouette(struct Sprite *sprite)
{
    sprite->data[7]++;

    switch (sprite->sState)
    {
    case 0:
    default:
        sprite->oam.affineMode = 3;
        sprite->oam.matrixNum = 1;
        CalcCenterToCornerVec(sprite, 1, 3, 3);
        sprite->invisible = FALSE;
        sprite->sState = 1;
        sprite->data[1] = 128;
        sprite->data[2] = -24;
        sprite->data[3] = 0;
        break;
    case 1:
    {
        s16 sin;
        s16 cos;
        s16 matrixA;
        s16 matrixB;
        s16 matrixC;
        s16 matrixD;

        if (sprite->data[3] < 0x50)
        {
            sprite->y2 = -Sin((u8)sprite->data[3], 0x78);
            sprite->x2 = -Sin((u8)sprite->data[3], 0x8C);
            if (sprite->data[3] > 64)
                sprite->oam.priority = 3;
        }
        sin = gSineTable[(u8)sprite->data[2]];
        cos = gSineTable[(u8)(sprite->data[2] + 64)];
        matrixD =  cos * sprite->data[1] / 256;
        matrixC = -sin * sprite->data[1] / 256;
        matrixB =  sin * sprite->data[1] / 256;
        matrixA = cos * sprite->data[1] / 256;

        SetOamMatrix(1, matrixA, matrixB, matrixC, matrixD);

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

static void SpriteCB_IntroOpposingPokemon(struct Sprite *sprite)
{
    switch (sprite->sState)
    {
    case 0:
    default:
        if (sprite->data[2] != 0)
            sprite->hFlip = TRUE;
        else
            sprite->hFlip = FALSE;
        sprite->sState = 1;
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
            sprite->sState++;
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
            sprite->sState++;
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
        sprite->sState++;
        sprite->data[4] = 0;
        break;
    case 4:
        sprite->data[4]++;
        if (sprite->y + sprite->y2 > -32
         && sprite->x + sprite->x2 > -64)
        {
            u16 scale;

            sprite->y2 = -(sprite->data[4] * sprite->data[4]) / 8;
            if (sprite->data[2] != 0)
                sprite->x2 += sprite->data[4];
            else
                sprite->x2 -= sprite->data[4];
            if (sprite->data[3] < 128)
                sprite->data[3] += 8;
            scale = 256 - sprite->data[3];
            if (sprite->data[2] != 0)
                SetOamMatrix(sprite->data[1], -scale, 0, 0, scale);
            else
                SetOamMatrix(sprite->data[1], scale, 0, 0, scale);
        }
        else
        {
            DestroySprite(sprite);
        }
    }
}

static void SpriteCB_IntroPartnerPokemon(struct Sprite *sprite)
{
    switch (sprite->sState)
    {
    case 0:
    default:
        sprite->invisible = FALSE;
        sprite->oam.affineMode = 1;
        sprite->oam.matrixNum = sprite->data[1];
        sprite->data[3] = 2048;
        sprite->sState = 1;
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
            sprite->sState++;
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

static void SpriteCB_IntroTrainer(struct Sprite *sprite)
{
    switch (sprite->sState)
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
            sprite->data[6] = CreateSprite(&sSpriteTemplate_IntroFlyingPokeball, 16, 104, 100);
            sprite->data[7] = CreateSprite(&sSpriteTemplate_IntroFlyingPokeball, 12, 106, 101);
            sprite->sState = 1;
        }
        break;
    case 1:
        break;
    case 2:
        StartSpriteAnim(sprite, 2);
        gSprites[sprite->data[6]].sState = 1;
        gSprites[sprite->data[7]].sState = 2;
        sprite->sState++;
        break;
    case 3:
        if (sprite->y > 160)
        {
            sprite->invisible = TRUE;
            sprite->sState = 1;
        }
        else
        {
            sprite->y += 2;
            sprite->x--;
        }
        break;
    case 4:
    {
        s16 ballX;
        s16 ballY;

        ballX = gSprites[sprite->data[6]].x + gSprites[sprite->data[6]].x2;
        ballY = gSprites[sprite->data[6]].y + gSprites[sprite->data[6]].y2;
        DestroySprite(&gSprites[sprite->data[6]]);
        sprite->data[6] = CreateIntroBattleMonSprite(SPECIES_TORCHIC, ballX, ballY, 2, 1);
        gSprites[sprite->data[6]].callback = SpriteCB_IntroPartnerPokemon;
        gSprites[sprite->data[6]].invisible = TRUE;
        gSprites[sprite->data[6]].data[1] = 1;
        gSprites[sprite->data[6]].data[2] = 1;
        CreateIntroBurstParticles(ballX, ballY);

        ballX = gSprites[sprite->data[7]].x + gSprites[sprite->data[7]].x2;
        ballY = gSprites[sprite->data[7]].y + gSprites[sprite->data[7]].y2;
        DestroySprite(&gSprites[sprite->data[7]]);
        sprite->data[7] = CreateIntroBattleMonSprite(SPECIES_MUDKIP, ballX, ballY, 3, 1);
        gSprites[sprite->data[7]].callback = SpriteCB_IntroPartnerPokemon;
        gSprites[sprite->data[7]].invisible = TRUE;
        gSprites[sprite->data[7]].data[1] = 2;
        gSprites[sprite->data[7]].data[2] = 0;
        CreateIntroBurstParticles(ballX, ballY);

        BeginNormalPaletteFade(0xFF0000, 0, 16, 16, RGB(31, 23, 31));
        sprite->sState = 1;
        break;
    }
    case 5:
        gSprites[sprite->data[6]].sState = 3;
        gSprites[sprite->data[7]].sState = 3;
        break;
    case 6:
        DestroySprite(&gSprites[sprite->data[6]]);
        DestroySprite(&gSprites[sprite->data[7]]);
        DestroySprite(sprite);
        break;
    }
}

static void SpriteCB_IntroOpposingPokemonBattle(struct Sprite *sprite)
{
    switch (sprite->sState)
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
            sprite->sState = 1;
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
            sprite->sState = 1;
        }
        break;
    }
}

static void SpriteCB_IntroPartnerPokemonBattle(struct Sprite *sprite)
{
    switch (sprite->sState)
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
            sprite->sState = 1;
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
            sprite->sState = 1;
        }
        break;
    }
}

static void SpriteCB_IntroFlyingPokeball(struct Sprite *sprite)
{
    u16 sine;
    u16 cosine;

    sprite->data[7]++;
    switch (sprite->sState)
    {
    case 0:
    default:
        break;
    case 1:
        sprite->oam.affineMode = 1;
        sprite->oam.matrixNum = 1;
        sprite->sState = 10;
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
        sine = gSineTable[(u8)sprite->data[3]];
        cosine = gSineTable[(u8)(sprite->data[3] + 64)];
        SetOamMatrix(1, cosine, sine, -sine, cosine);
        break;
    case 2:
        sprite->oam.affineMode = 1;
        sprite->oam.matrixNum = 2;
        sprite->sState = 20;
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
        sine = gSineTable[(u8)sprite->data[3]];
        cosine = gSineTable[(u8)(sprite->data[3] + 64)];
        SetOamMatrix(2, cosine, sine, -sine, cosine);
        break;
    }
}

#define sBurstAngleIndex data[0]

static void SpriteCB_IntroBurstParticle(struct Sprite *sprite)
{
    u16 radius;
    u16 sine;
    u16 cosine;

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
    radius = Sin((u8)sprite->data[2], 40);
    sprite->x2 = Cos((u8)(sprite->sBurstAngleIndex * 32), radius);
    sprite->y2 = Sin((u8)(sprite->sBurstAngleIndex * 32), radius);
    if (sprite->sBurstAngleIndex == 0)
    {
        sprite->data[3] -= sprite->data[1];
        if ((sprite->data[7] & 1) && sprite->data[1] != 0)
            sprite->data[1]--;
        sine = gSineTable[(u8)sprite->data[3]];
        cosine = gSineTable[(u8)(sprite->data[3] + 64)];
        SetOamMatrix(16, cosine, sine, -sine, cosine);
    }
}

static void CreateIntroBurstParticles(u16 x, u16 y)
{
    u8 i;
    u8 spriteId;

    for (i = 0; i < 8; i++)
    {
        spriteId = CreateSprite(&sSpriteTemplate_IntroBurstParticle, x, y, 0);
        gSprites[spriteId].oam.affineMode = 1;
        gSprites[spriteId].oam.matrixNum = 16;
        gSprites[spriteId].sBurstAngleIndex = i;
        gSprites[spriteId].data[1] = 32;
    }
}

#undef sBurstAngleIndex

#define sParentSpriteId data[0]

static void SpriteCB_IntroAttackParticle(struct Sprite *sprite)
{
    if (sIntroBattleFinished != 0)
    {
        DestroySprite(sprite);
    }
    else
    {
        sprite->invisible = gSprites[sprite->sParentSpriteId].invisible;
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

static void SpriteCB_IntroAttackParticleSpawner(struct Sprite *sprite)
{
    u8 spriteId;
    u8 i;
    s16 particleX;
    s16 particleY;

    if (sIntroBattleFinished != 0)
    {
        DestroySprite(sprite);
    }
    else
    {
        sprite->data[7]++;
        sprite->invisible = TRUE;
        if (gSprites[sprite->sParentSpriteId].sState == 1 && !(sprite->data[7] & 3))
        {
            particleX = sprite->data[1] + gSprites[sprite->sParentSpriteId].x;
            particleY = sprite->data[2] + gSprites[sprite->sParentSpriteId].y;
            for (i = 0; i < 3; i++)
            {
                u8 subpriority = gSprites[sprite->sParentSpriteId].subpriority - 1;
                //Make redundant copies of these variables to get the asm to match
                s16 particleXCopy = particleX;
                s16 particleYCopy = particleY;

                spriteId = CreateSprite(&sSpriteTemplate_IntroAttackParticle, particleXCopy, particleYCopy, subpriority);
                if (spriteId != 64)
                {
                    gSprites[spriteId].sParentSpriteId = sprite->sParentSpriteId;
                    gSprites[spriteId].data[1] = (((sprite->data[7] >> 2) & 7) << 5) + i * 85;
                    gSprites[spriteId].data[2] = sprite->data[3];
                    gSprites[spriteId].data[3] = 104;
                    gSprites[spriteId].data[4] = particleX;
                    gSprites[spriteId].data[5] = particleY;
                    gSprites[spriteId].data[6] = 0;
                }
            }
        }
    }
}

static void CreateIntroOpposingAttackEffect(u8 opponentSpriteId)
{
    u8 spriteId;

    spriteId = CreateSprite(&sSpriteTemplate_IntroAttackParticleSpawner, 0, 0, 0);
    if (spriteId != 64)
    {
        gSprites[spriteId].sParentSpriteId = opponentSpriteId;
        gSprites[spriteId].data[1] = -12;
        gSprites[spriteId].data[2] = 0;
        gSprites[spriteId].data[3] = 136;
    }
}

static void SpriteCB_IntroOrbitingParticle(struct Sprite *sprite)
{
    if (sIntroBattleFinished != 0)
    {
        DestroySprite(sprite);
    }
    else
    {
        sprite->invisible = gSprites[sprite->sParentSpriteId].invisible;
        sprite->data[7]++;
        if (sprite->data[3] < 40)
            sprite->data[3] += 2;
        //This useless '+ 0' is needed to make the asm match
        sprite->x = gSprites[sprite->sParentSpriteId].x + gSprites[sprite->sParentSpriteId].x2 + gSineTable[(u8)(sprite->data[1] + 64)] * sprite->data[3] / 256;
        sprite->y = gSprites[sprite->sParentSpriteId].y + gSprites[sprite->sParentSpriteId].y2 + gSineTable[(u8)(sprite->data[1] + 0)] * sprite->data[3] / 512;
        sprite->data[1] += 2;
        sprite->y2 = gSineTable[(u8)(sprite->data[2] + 0)] / 32;
        sprite->data[2] += 8;
        if ((sprite->data[1] & 0xFF) < 128)
            sprite->subpriority = gSprites[sprite->sParentSpriteId].subpriority - 1;
        else
            sprite->subpriority = gSprites[sprite->sParentSpriteId].subpriority + 1;
    }
}

static void CreateIntroOrbitingParticles(u8 opponentSpriteId)
{
    u8 i;
    u8 spriteId;

    for (i = 0; i < 8; i++)
    {
        spriteId = CreateSprite(&sSpriteTemplate_IntroOrbitingParticle, gSprites[opponentSpriteId].x, gSprites[opponentSpriteId].y, 0);
        if (spriteId != 64)
        {
            gSprites[spriteId].sParentSpriteId = opponentSpriteId;
            gSprites[spriteId].data[1] = i * 32;
        }
    }
}

static void SpriteCB_IntroTorchicAttackParticle(struct Sprite *sprite)
{
    if (sIntroBattleFinished != 0)
    {
        DestroySprite(sprite);
    }
    else
    {
        u8 scaleIndex;
        u16 matrixNum;

        sprite->invisible = gSprites[sprite->sParentSpriteId].invisible;
        sprite->data[7]++;
        sprite->data[6] += 8;
        sprite->x = sprite->data[4] + gSineTable[(u8)(sprite->data[3] + 64)] * sprite->data[6] / 256;
        sprite->y = sprite->data[5] + gSineTable[(u8)(sprite->data[3] + 0)] * sprite->data[6] / 256;
        scaleIndex = sprite->data[6] / 16;
        if (scaleIndex > 9)
            scaleIndex = 9;
        matrixNum = (scaleIndex + 18) & 31;
        sprite->oam.matrixNum = matrixNum;
        if (sprite->data[6] > 160)
            DestroySprite(sprite);
    }
}

static void SpriteCB_IntroTorchicAttackSpawner(struct Sprite *sprite)
{
    bool32 isOddFrame;
    s16 particleX;
    s16 particleY;
    u8 spriteId;

    if (sIntroBattleFinished != 0)
    {
        DestroySprite(sprite);
    }
    else
    {
        sprite->data[7]++;
        sprite->invisible = TRUE;
        if (gSprites[sprite->sParentSpriteId].sState == 1)
        {
            isOddFrame = (sprite->data[7] & 1);
            if (!isOddFrame)
            {
                particleX = sprite->data[1] + gSprites[sprite->sParentSpriteId].x;
                particleY = sprite->data[2] + gSprites[sprite->sParentSpriteId].y;
                spriteId = CreateSprite(&sSpriteTemplate_IntroTorchicAttackParticle, particleX, particleY, gSprites[sprite->sParentSpriteId].subpriority + 1);
                if (spriteId != 64)
                {
                    gSprites[spriteId].oam.affineMode = 3;
                    gSprites[spriteId].oam.matrixNum = 18;
                    CalcCenterToCornerVec(&gSprites[spriteId], 0, 1, 3);
                    gSprites[spriteId].sParentSpriteId = sprite->sParentSpriteId;
                    gSprites[spriteId].data[3] = sTorchicAttackParticleAngles[(sprite->data[7] >> 1) & 7];
                    gSprites[spriteId].data[4] = particleX;
                    gSprites[spriteId].data[5] = particleY;
                    gSprites[spriteId].data[6] = isOddFrame;
                }
            }
        }
    }
}

static void InitIntroTorchicAttackAnim(u8 pokemonSpriteId)
{
    u8 spriteId;
    u8 i;

    spriteId = CreateSprite(&sSpriteTemplate_IntroTorchicAttackSpawner, 0, 0, 0);
    if (spriteId != 64)
    {
        gSprites[spriteId].sParentSpriteId = pokemonSpriteId;
        gSprites[spriteId].data[1] = 0;
        gSprites[spriteId].data[2] = 8;
        gSprites[spriteId].data[3] = 24;
    }
    for (i = 0; i < 10; i++)
    {
        SetOamMatrix(18 + i, sTorchicAttackParticleScales[i], 0, 0, sTorchicAttackParticleScales[i]);
    }
}

static void SpriteCB_IntroMudkipAttackParticle(struct Sprite *sprite)
{
    if (sIntroBattleFinished != 0)
    {
        DestroySprite(sprite);
    }
    else
    {
        sprite->invisible = gSprites[sprite->sParentSpriteId].invisible;
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

static void SpriteCB_IntroMudkipAttackSpawner(struct Sprite *sprite)
{
    bool32 isOddFrame;
    s16 particleX;
    s16 particleY;
    u8 spriteId;
    u16 matrixScale;

    if (sIntroBattleFinished != 0)
    {
        DestroySprite(sprite);
    }
    else
    {
        sprite->data[7]++;
        sprite->invisible = TRUE;
        if (gSprites[sprite->sParentSpriteId].sState == 1)
        {
            isOddFrame = sprite->data[7] & 1;
            if (!isOddFrame)
            {
                particleX = sprite->data[1] + gSprites[sprite->sParentSpriteId].x;
                particleY = sprite->data[2] + gSprites[sprite->sParentSpriteId].y;
                spriteId = CreateSprite(&sSpriteTemplate_IntroMudkipAttackParticle, particleX, particleY, gSprites[sprite->sParentSpriteId].subpriority + 1);
                if (spriteId != 64)
                {
                    gSprites[spriteId].oam.affineMode = 3;
                    gSprites[spriteId].oam.matrixNum = 17;
                    CalcCenterToCornerVec(&gSprites[spriteId], 0, 1, 3);
                    gSprites[spriteId].sParentSpriteId = sprite->sParentSpriteId;
                    gSprites[spriteId].data[1] = ((sprite->data[7] >> 2) & 7) << 5;
                    gSprites[spriteId].data[2] = sprite->data[3];
                    gSprites[spriteId].data[3] = 232;
                    gSprites[spriteId].data[4] = particleX;
                    gSprites[spriteId].data[5] = particleY;
                    gSprites[spriteId].data[6] = isOddFrame;
                }
            }
            if (sprite->data[6] < 112)
                sprite->data[6] += 4;
        }
        matrixScale = 256 - gSineTable[(u8)sprite->data[6]] / 2;
        SetOamMatrix(17, matrixScale, 0, 0, matrixScale);
    }
}

static void InitIntroMudkipAttackAnim(u8 pokemonSpriteId)
{
    u8 spriteId;

    spriteId = CreateSprite(&sSpriteTemplate_IntroMudkipAttackSpawner, 0, 0, 0);
    if (spriteId != 64)
    {
        gSprites[spriteId].sParentSpriteId = pokemonSpriteId;
        gSprites[spriteId].data[1] = 0;
        gSprites[spriteId].data[2] = 12;
        gSprites[spriteId].data[3] = 24;
    }
}

static void SpriteCB_IntroBattleFlash(struct Sprite *sprite)
{
    u16 matrixScale;

    //I'm not sure why a switch statement was used here.
    //if (sprite->sState != 1) would have been more appropriate.
    switch (sprite->sState)
    {
    case 0:
    default:
        sprite->invisible = FALSE;
        sprite->oam.affineMode = 3;
        sprite->oam.matrixNum = 18;
        CalcCenterToCornerVec(sprite, 0, 3, 3);
        sprite->data[1] = 0;
        sprite->sState = 1;
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
    matrixScale = 256 - gSineTable[(u8)sprite->data[1]] / 2;
    SetOamMatrix(18, matrixScale, 0, 0, matrixScale);
}

#undef sParentSpriteId
