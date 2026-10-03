#include "global.h"
#include "task.h"
#include "palette.h"
#include "main.h"
#include "menu_helpers.h"
#include "text.h"
#include "text_window.h"
#include "menu.h"
#include "overworld.h"
#include "decompress.h"
#include "data.h"
#include "sprite.h"
#include "item_use.h"
#include "pokeblock.h"
#include "party_menu.h"
#include "pokemon.h"
#include "strings.h"
#include "string_util.h"
#include "m4a.h"
#include "field_effect.h"
#include "sound.h"
#include "trig.h"
#include "ewram.h"
#include "constants/rgb.h"

extern struct SpriteTemplate gCreatingSpriteTemplate;

enum
{
    ANIMDATA_ROT_IDX,
    ANIMDATA_ROT_SPEED,
    ANIMDATA_SIN_AMPLITUDE,
    ANIMDATA_COS_AMPLITUDE,
    ANIMDATA_TIME,
    ANIMDATA_ROT_ACCEL,
    ANIMDATA_TARGET_X,
    ANIMDATA_TARGET_Y,
    ANIMDATA_APPR_TIME,
    ANIMDATA_IS_LAST,
    NUM_ANIMDATA,
    ANIMSTATE_INIT_X = NUM_ANIMDATA,
    ANIMSTATE_INIT_Y,
    ANIMSTATE_MAX_TIME,
    ANIMSTATE_MON_X,
    ANIMSTATE_MON_Y,
};

enum
{
    AFFINE_NONE,
    AFFINE_TURN_UP,
    AFFINE_TURN_UP_AND_DOWN,
    AFFINE_TURN_DOWN,
    AFFINE_TURN_DOWN_SLOW,
    AFFINE_TURN_DOWN_SLIGHT,
    AFFINE_TURN_UP_HIGH,
    AFFINE_UNUSED_1,
    AFFINE_UNUSED_2,
    AFFINE_UNUSED_3,
    NUM_MON_AFFINES,
};

enum
{
    NATURE_ANIM_ID,
    NATURE_AFFINE_ANIM,
};

#define MON_X 48
#define MON_Y 80
#define TAG_POKEBLOCK 14818

// The animation the Pokémon does during the feeding scene depends on its nature.
// These values are offsets into sMonPokeblockAnims.
#define ANIM_HARDY   0
#define ANIM_LONELY  (ANIM_HARDY + 3)
#define ANIM_BRAVE   (ANIM_LONELY + 1)
#define ANIM_ADAMANT (ANIM_BRAVE + 1)
#define ANIM_NAUGHTY (ANIM_ADAMANT + 5)
#define ANIM_BOLD    (ANIM_NAUGHTY + 3)
#define ANIM_DOCILE  (ANIM_BOLD + 2)
#define ANIM_RELAXED (ANIM_DOCILE + 1)
#define ANIM_IMPISH  (ANIM_RELAXED + 2)
#define ANIM_LAX     (ANIM_IMPISH + 1)
#define ANIM_TIMID   (ANIM_LAX + 1)
#define ANIM_HASTY   (ANIM_TIMID + 5)
#define ANIM_SERIOUS (ANIM_HASTY + 2)
#define ANIM_JOLLY   (ANIM_SERIOUS + 1)
#define ANIM_NAIVE   (ANIM_JOLLY + 1)
#define ANIM_MODEST  (ANIM_NAIVE + 4)
#define ANIM_MILD    (ANIM_MODEST + 3)
#define ANIM_QUIET   (ANIM_MILD + 1)
#define ANIM_BASHFUL (ANIM_QUIET + 2)
#define ANIM_RASH    (ANIM_BASHFUL + 3)
#define ANIM_CALM    (ANIM_RASH + 3)
#define ANIM_GENTLE  (ANIM_CALM + 1)
#define ANIM_SASSY   (ANIM_GENTLE + 1)
#define ANIM_CAREFUL (ANIM_SASSY + 1)
#define ANIM_QUIRKY  (ANIM_CAREFUL + 5)

#define STATE_START_THROW 255
#define STATE_SPAWN_PBLOCK (STATE_START_THROW + 14)
#define STATE_START_JUMP (STATE_SPAWN_PBLOCK + 12)
#define STATE_PRINT_MSG (STATE_START_JUMP + 16)

#define STATE_INIT 0

extern struct MusicPlayerInfo gMPlayInfo_BGM;

extern const u8 gPokeblockRed_Pal[];
extern const u8 gPokeblockBlue_Pal[];
extern const u8 gPokeblockPink_Pal[];
extern const u8 gPokeblockGreen_Pal[];
extern const u8 gPokeblockYellow_Pal[];
extern const u8 gPokeblockPurple_Pal[];
extern const u8 gPokeblockIndigo_Pal[];
extern const u8 gPokeblockBrown_Pal[];
extern const u8 gPokeblockLiteBlue_Pal[];
extern const u8 gPokeblockOlive_Pal[];
extern const u8 gPokeblockGray_Pal[];
extern const u8 gPokeblockBlack_Pal[];
extern const u8 gPokeblockWhite_Pal[];
extern const u8 gPokeblockGold_Pal[];
extern const u8 gPokeblock_Gfx[];
extern const u8 gBattleEnvironmentTiles_Building[];
extern const u8 gPokeblockFeedBg_Tilemap[];
extern const u8 gBattleEnvironmentPalette_BattleTower[];
// this file's functions
static void HandleInitBackgrounds(void);
static void CalculateMonAnimLength(void);
static void UpdateMonAnim(void);
static u8 CreatePokeblockCaseSpriteForFeeding(void);
static u8 CreatePokeblockSprite(void);
static u8 CreateMonSprite(struct Pokemon* mon);
static bool8 LoadMonAndSceneGfx(struct Pokemon* mon);
static void LaunchPokeblockFeedTask(u8 horizontalThrow);
static void StartMonJumpForPokeblock(u8);
static void SpriteCB_MonJumpForPokeblock(struct Sprite* sprite);
static void Task_PrintAtePokeblockMessage(u8 taskId);
static void Task_FadeOutPokeblockFeed(u8 taskId);
static void SetPokeblockSpritePal(u8);
static void DoPokeblockCaseThrowEffect(u8 spriteId, bool8 horizontalThrow);
static bool8 InitMonAnimStage(void);
static bool8 DoMonAnimStep(void);
static bool8 FreeMonSpriteOamMatrix(void);
static void CalculateMonAnimMovement(void);
static void SpriteCB_ThrownPokeblock(struct Sprite* sprite);
static void CalculateMonAnimMovementEnd(void);

// EWRAM
EWRAM_DATA static struct CompressedSpritePalette sPokeblockSpritePal = {0};

// IWRAM common
struct Sprite* sMonSpritePtr;
u16 sMonSpecies;
bool8 sNoMonFlip;
u8 sMonSpriteId;
u8 sMonNature;
u16 sMonAnimLength;
u8 sUnused;
u8 sMonAnimRunState;
u8 sMonAnimId;
struct Sprite sSavedMonSprite;
u16 sTimer;
s16 sAnimData[24];

// rodata

static const u8 sNatureToMonPokeblockAnim[NUM_NATURES][2] =
{
    [NATURE_HARDY]   = { ANIM_HARDY,   AFFINE_NONE },
    [NATURE_LONELY]  = { ANIM_LONELY,  AFFINE_NONE },
    [NATURE_BRAVE]   = { ANIM_BRAVE,   AFFINE_TURN_UP },
    [NATURE_ADAMANT] = { ANIM_ADAMANT, AFFINE_NONE },
    [NATURE_NAUGHTY] = { ANIM_NAUGHTY, AFFINE_NONE },
    [NATURE_BOLD]    = { ANIM_BOLD,    AFFINE_NONE },
    [NATURE_DOCILE]  = { ANIM_DOCILE,  AFFINE_NONE },
    [NATURE_RELAXED] = { ANIM_RELAXED, AFFINE_TURN_UP_AND_DOWN },
    [NATURE_IMPISH]  = { ANIM_IMPISH,  AFFINE_NONE },
    [NATURE_LAX]     = { ANIM_LAX,     AFFINE_NONE },
    [NATURE_TIMID]   = { ANIM_TIMID,   AFFINE_NONE },
    [NATURE_HASTY]   = { ANIM_HASTY,   AFFINE_NONE },
    [NATURE_SERIOUS] = { ANIM_SERIOUS, AFFINE_TURN_DOWN },
    [NATURE_JOLLY]   = { ANIM_JOLLY,   AFFINE_NONE },
    [NATURE_NAIVE]   = { ANIM_NAIVE,   AFFINE_NONE },
    [NATURE_MODEST]  = { ANIM_MODEST,  AFFINE_TURN_DOWN_SLOW },
    [NATURE_MILD]    = { ANIM_MILD,    AFFINE_NONE },
    [NATURE_QUIET]   = { ANIM_QUIET,   AFFINE_NONE },
    [NATURE_BASHFUL] = { ANIM_BASHFUL, AFFINE_NONE },
    [NATURE_RASH]    = { ANIM_RASH,    AFFINE_NONE },
    [NATURE_CALM]    = { ANIM_CALM,    AFFINE_NONE },
    [NATURE_GENTLE]  = { ANIM_GENTLE,  AFFINE_TURN_DOWN_SLIGHT },
    [NATURE_SASSY]   = { ANIM_SASSY,   AFFINE_TURN_UP_HIGH },
    [NATURE_CAREFUL] = { ANIM_CAREFUL, AFFINE_NONE },
    [NATURE_QUIRKY]  = { ANIM_QUIRKY,  AFFINE_NONE },
};

static const s16 sMonPokeblockAnims[][NUM_ANIMDATA] =
{
    // HARDY
    {   0,   4,   0,   8,  24,   0,   0,   0,  12,   0},
    {   0,   4,   0,  16,  24,   0,   0,   0,  12,   0},
    {   0,   4,   0,  32,  32,   0,   0,   0,  16,   1},

    // LONELY
    {   0,   3,   6,   0,  48,   0,   0,   0,  24,   1},

    // BRAVE
    {  64,  16, -24,   0,  32,   0,   0,   0,   0,   1},

    // ADAMANT
    {   0,   4,   8,   0,  16,   0,  -8,   0,   0,   0},
    {   0,   0,   0,   0,  16,   0,   0,   0,   0,   0},
    {   0,   4,   8,   0,  16,   0,  -8,   0,   0,   0},
    {   0,   0,   0,   0,  16,   0,   0,   0,   0,   0},
    {   0,   4, -16,   0,   4,   0,  16,   0,   0,   1},

    // NAUGHTY
    {   0,   3,   6,   0,  12,   0,   0,   0,   6,   0},
    {   0,   3,  -6,   0,  12,   0,   0,   0,   6,   0},
    {   0,  16,  16,   0,  45,   1,   0,   0,   0,   1},

    // BOLD
    {   0,  16,   0,  24,  32,   0,   0,   0,  16,   0},
    {   0,  16,   0,  23,  32,   0,   0,   0,  16,   1},

    // DOCILE
    {   0,   0,   0,   0,  80,   0,   0,   0,   0,   1},

    // RELAXED
    {   0,   2,   8,   0,  32,   0,   0,   0,   0,   0},
    {   0,   2,  -8,   0,  32,   0,   0,   0,   0,   1},

    // IMPISH
    {   0,  32,   2,   1,  48,   1,   0,   0,  24,   1},

    // LAX
    {   0,   2,  16,  16, 128,   0,   0,   0,   0,   1},

    // TIMID
    {   0,   2,  -8,   0,  48,   0, -24,   0,   0,   0},
    {   0,   0,   0,   0,   8,   0,   0,   0,   0,   0},
    {  64,  32,   2,   0,  36,   0,   0,   0,   0,   0},
    {   0,   0,   0,   0,   8,   0,   0,   0,   0,   0},
    {   0,   2,   8,   0,  48,   0,  24,   0,   0,   1},

    // HASTY
    {  64,  24,  16,   0,  32,   0,   0,   0,   0,   0},
    {   0,  28,   2,   1,  32,   1,   0,   0,  16,   1},

    // SERIOUS
    {   0,   0,   0,   0,  32,   0,   0,   0,   0,   1},

    // JOLLY
    {  64,  16, -16,   2,  48,   0,   0,   0,  32,   1},

    // NAIVE
    {   0,  12,  -8,   4,  24,   0,   8,   0,  12,   0},
    {   0,  12,   8,   8,  24,   0, -16,   0,  12,   0},
    {   0,  12,  -8,  16,  24,   0,  16,   0,  12,   0},
    {   0,  12,   8,  28,  24,   0,  -8,   0,  12,   1},

    // MODEST
    {   0,   0,   0,   0,   8,   0,   0,   0,   0,   0},
    {  64,  16,  -4,   0,  32,   0,   0,   0,   0,   0},
    {   0,   0,   0,   0,   8,   0,   0,   0,   0,   1},

    // MILD
    { 128,   4,   0,   8,  64,   0,   0,   0,   0,   1},

    // QUIET
    {   0,   2,  16,   0,  48,   0,   0,   0,   0,   0},
    { 128,   2,  16,   0,  48,   0,   0,   0,   0,   1},

    // BASHFUL
    {   0,   2,  -4,   0,  48,   0, -48,   0,   0,   0},
    {   0,   0,   0,   0,  80,   0,   0,   0,   0,   0},
    {   0,   2,   8,   0,  24,   0,  48,   0,   0,   1},

    // RASH
    {  64,   4,  64,  58,  52,   0, -88,   0,   0,   0},
    {   0,   0,   0,   0,  80,   0,   0,   0,   0,   0},
    {   0,  24,  80,   0,  32,   0,  88,   0,   0,   1},

    // CALM
    {   0,   2,  16,   4,  64,   0,   0,   0,   0,   1},

    // GENTLE
    {   0,   0,   0,   0,  32,   0,   0,   0,   0,   1},

    // SASSY
    {   0,   0,   0,   0,  42,   0,   0,   0,   0,   1},

    // CAREFUL
    {   0,   4,   0,   8,  24,   0,   0,   0,  12,   0},
    {   0,   0,   0,   0,  12,   0,   0,   0,   0,   0},
    {   0,   4,   0,  12,  24,   0,   0,   0,  12,   0},
    {   0,   0,   0,   0,  12,   0,   0,   0,   0,   0},
    {   0,   4,   0,   4,  24,   0,   0,   0,  12,   1},

    // QUIRKY
    {   0,   4,  16,  12,  64,   0,   0,   0,   0,   0},
    {   0,  -4,  16,  12,  64,   0,   0,   0,   0,   1},
};

static const union AffineAnimCmd sAffineAnim_Mon_None[] =
{
    AFFINEANIMCMD_FRAME(-0x100, 0x100, 0, 0),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_Mon_TurnUp[] =
{
    AFFINEANIMCMD_FRAME(0, 0, 12, 1),
    AFFINEANIMCMD_FRAME(0, 0, 0, 30),
    AFFINEANIMCMD_FRAME(0, 0, -12, 1),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_Mon_TurnUp_Flipped[] =
{
    AFFINEANIMCMD_FRAME(-0x100, 0x100, 0, 0),
    AFFINEANIMCMD_FRAME(0, 0, 12, 1),
    AFFINEANIMCMD_FRAME(0, 0, 0, 28),
    AFFINEANIMCMD_FRAME(0, 0, -4, 3),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_Mon_TurnUpAndDown[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, 1, 16),
    AFFINEANIMCMD_FRAME(0x0, 0x0, -1, 32),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 1, 16),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_Mon_TurnUpAndDown_Flipped[] =
{
    AFFINEANIMCMD_FRAME(-0x100, 0x100, 0, 0),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 1, 16),
    AFFINEANIMCMD_FRAME(0x0, 0x0, -1, 32),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 1, 16),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_Mon_TurnDown[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, -1, 8),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 0, 16),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 1, 8),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_Mon_TurnDown_Flipped[] =
{
    AFFINEANIMCMD_FRAME(-0x100, 0x100, 0, 0),
    AFFINEANIMCMD_FRAME(0x0, 0x0, -1, 8),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 0, 16),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 1, 8),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_Mon_TurnDownSlow[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, -1, 8),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 0, 32),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 1, 8),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_Mon_TurnDownSlow_Flipped[] =
{
    AFFINEANIMCMD_FRAME(-0x100, 0x100, 0, 0),
    AFFINEANIMCMD_FRAME(0x0, 0x0, -1, 8),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 0, 32),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 1, 8),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_Mon_TurnDownSlight[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, -1, 4),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 0, 24),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 1, 4),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_Mon_TurnDownSlight_Flipped[] =
{
    AFFINEANIMCMD_FRAME(-0x100, 0x100, 0, 0),
    AFFINEANIMCMD_FRAME(0x0, 0x0, -1, 4),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 0, 24),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 1, 4),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_Mon_TurnUpHigh[] =
{
    AFFINEANIMCMD_FRAME(0x0, 0x0, 1, 24),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 0, 16),
    AFFINEANIMCMD_FRAME(0x0, 0x0, -12, 2),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_Mon_TurnUpHigh_Flipped[] =
{
    AFFINEANIMCMD_FRAME(-0x100, 0x100, 0, 0),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 1, 24),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 0, 16),
    AFFINEANIMCMD_FRAME(0x0, 0x0, -12, 2),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd *const sAffineAnims_Mon[] =
{
    sAffineAnim_Mon_None,
    sAffineAnim_Mon_TurnUp,
    sAffineAnim_Mon_TurnUpAndDown,
    sAffineAnim_Mon_TurnDown,
    sAffineAnim_Mon_TurnDownSlow,
    sAffineAnim_Mon_TurnDownSlight,
    sAffineAnim_Mon_TurnUpHigh,
    sAffineAnim_Mon_None,
    sAffineAnim_Mon_None,
    sAffineAnim_Mon_None,
    sAffineAnim_Mon_None,
    sAffineAnim_Mon_TurnUp_Flipped,
    sAffineAnim_Mon_TurnUpAndDown_Flipped,
    sAffineAnim_Mon_TurnDown_Flipped,
    sAffineAnim_Mon_TurnDownSlow_Flipped,
    sAffineAnim_Mon_TurnDownSlight_Flipped,
    sAffineAnim_Mon_TurnUpHigh_Flipped,
    sAffineAnim_Mon_None,
    sAffineAnim_Mon_None,
    sAffineAnim_Mon_None,
    sAffineAnim_Mon_None,
};

static const u8 *const sPokeblocksPals[] =
{
    [PBLOCK_CLR_RED - 1]       = gPokeblockRed_Pal,
    [PBLOCK_CLR_BLUE - 1]      = gPokeblockBlue_Pal,
    [PBLOCK_CLR_PINK - 1]      = gPokeblockPink_Pal,
    [PBLOCK_CLR_GREEN - 1]     = gPokeblockGreen_Pal,
    [PBLOCK_CLR_YELLOW - 1]    = gPokeblockYellow_Pal,
    [PBLOCK_CLR_PURPLE - 1]    = gPokeblockPurple_Pal,
    [PBLOCK_CLR_INDIGO - 1]    = gPokeblockIndigo_Pal,
    [PBLOCK_CLR_BROWN - 1]     = gPokeblockBrown_Pal,
    [PBLOCK_CLR_LITE_BLUE - 1] = gPokeblockLiteBlue_Pal,
    [PBLOCK_CLR_OLIVE - 1]     = gPokeblockOlive_Pal,
    [PBLOCK_CLR_GRAY - 1]      = gPokeblockGray_Pal,
    [PBLOCK_CLR_BLACK - 1]     = gPokeblockBlack_Pal,
    [PBLOCK_CLR_WHITE - 1]     = gPokeblockWhite_Pal,
    [PBLOCK_CLR_GOLD - 1]      = gPokeblockGold_Pal,
};

static const union AffineAnimCmd sAffineAnim_Still[] =
{
    AFFINEANIMCMD_FRAME(-0x100, 0x100, 0, 0),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd *const sSpriteAffineAnimTable_MonNoFlip[] =
{
    sAffineAnim_Still
};

static const union AffineAnimCmd sAffineAnim_PokeblockCase_ThrowFromVertical[] =
{
    AFFINEANIMCMD_FRAME(-0x100, 0x100, 0, 0),
    AFFINEANIMCMD_FRAME(0x0, 0x0, -8, 1),
    AFFINEANIMCMD_FRAME(0x0, 0x0, -8, 1),
    AFFINEANIMCMD_FRAME(0x0, 0x0, -8, 1),
    AFFINEANIMCMD_FRAME(0x0, 0x0, -8, 1),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 0, 8),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 16, 1),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 16, 1),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 16, 1),
    AFFINEANIMCMD_FRAME(-0x100, 0x100, 0, 0),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd sAffineAnim_PokeblockCase_ThrowFromHorizontal[] =
{
    AFFINEANIMCMD_FRAME(-0x100, 0x100, 0, 0),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 8, 1),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 8, 1),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 8, 1),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 8, 1),
    AFFINEANIMCMD_FRAME(0x0, 0x0, 0, 8),
    AFFINEANIMCMD_FRAME(0x0, 0x0, -16, 1),
    AFFINEANIMCMD_FRAME(0x0, 0x0, -16, 1),
    AFFINEANIMCMD_FRAME(0x0, 0x0, -16, 1),
    AFFINEANIMCMD_FRAME(-0x100, 0x100, 0, 0),
    AFFINEANIMCMD_END
};

static const union AffineAnimCmd *const sAffineAnims_PokeblockCase_Still[] =
{
    sAffineAnim_Still
};

static const union AffineAnimCmd *const sAffineAnims_PokeblockCase_ThrowFromVertical[] =
{
    sAffineAnim_PokeblockCase_ThrowFromVertical
};

static const union AffineAnimCmd *const sAffineAnims_PokeblockCase_ThrowFromHorizontal[] =
{
    sAffineAnim_PokeblockCase_ThrowFromHorizontal
};

static const struct OamData sOamData_Pokeblock =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_DOUBLE,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(8x8),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(8x8),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sAnim_Pokeblock[] =
{
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_Pokeblock[] =
{
    sAnim_Pokeblock,
};

static const union AffineAnimCmd sAffineAnim_Pokeblock[] =
{
    AFFINEANIMCMD_FRAME(0x100, 0x100, 0, 0),
    AFFINEANIMCMD_FRAME(-8, -8, 0, 1),
    AFFINEANIMCMD_JUMP(1)
};

static const union AffineAnimCmd *const sAffineAnims_Pokeblock[] =
{
    sAffineAnim_Pokeblock
};

static const struct CompressedSpriteSheet sSpriteSheet_Pokeblock =
{
    gPokeblock_Gfx, 0x20, TAG_POKEBLOCK
};

static const struct SpriteTemplate sSpriteTemplate_Pokeblock =
{
    .tileTag = TAG_POKEBLOCK,
    .paletteTag = TAG_POKEBLOCK,
    .oam = &sOamData_Pokeblock,
    .anims = sAnims_Pokeblock,
    .images = NULL,
    .affineAnims = sAffineAnims_Pokeblock,
    .callback = SpriteCB_ThrownPokeblock
};

// code

static void CB2_PokeblockFeed(void)
{
    AnimateSprites();
    BuildOamBuffer();
    RunTasks();
    UpdatePaletteFade();
}

static void VBlankCB_PokeblockFeed(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static bool8 LoadPokeblockFeedScene(void)
{
    switch (gMain.state)
    {
    case 0:
        SetVBlankHBlankCallbacksToNull();
        ResetVramOamAndBgCntRegs();
        HandleInitBackgrounds();
        gMain.state++;
        break;
    case 1:
        ResetPaletteFade();
        gPaletteFade.bufferTransferDisabled = TRUE;
        gMain.state++;
        break;
    case 2:
        ResetSpriteData();
        gMain.state++;
        break;
    case 3:
        FreeAllSpritePalettes();
        gMain.state++;
        break;
    case 4:
        Text_LoadWindowTemplate(&gPokeblockWindowTemplate);
        gMain.state++;
        break;
    case 5:
        MultistepInitMenuWindowBegin(&gPokeblockWindowTemplate);
        gMain.state++;
        break;
    case 6:
        if (MultistepInitMenuWindowContinue())
        {
            sPokeblockLoadGfxState = 0;
            gMain.state++;
        }
        break;
    case 7:
        if (LoadMonAndSceneGfx(&gPlayerParty[gPokeblockMonId]))
        {
            gMain.state++;
        }
        break;
    case 8:
        sPokeblockFeedCaseSpriteId = CreatePokeblockCaseSpriteForFeeding();
        gMain.state++;
        break;
    case 9:
        sPokeblockFeedMonSpriteId = CreateMonSprite(&gPlayerParty[gPokeblockMonId]);
        gMain.state++;
        break;
    case 10:
        Menu_DrawStdWindowFrame(0, 14, 29, 19);
        gMain.state++;
        break;
    case 11:
        if (Overworld_IsRecvQueueAtMax() != TRUE)
        {
            gMain.state++;
        }
        break;
    case 12:
        {
            u16 savedIME = REG_IME;
            REG_IME = 0;
            REG_IE |= INTR_FLAG_VBLANK;
            REG_IME = savedIME;
            REG_DISPSTAT |= DISPSTAT_VBLANK_INTR;
            SetVBlankCallback(VBlankCB_PokeblockFeed);
            gMain.state++;
        }
    case 13:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        gPaletteFade.bufferTransferDisabled = FALSE;
        SetMainCallback2(CB2_PokeblockFeed);
        return TRUE;
    }
    return FALSE;
}

void PreparePokeblockFeedScene(void)
{
    while (1)
    {
        if (LoadPokeblockFeedScene() == TRUE)
        {
            LaunchPokeblockFeedTask(TRUE);
            break;
        }
        if (MenuHelpers_IsLinkActive() == TRUE)
            break;
    }
}

static void HandleInitBackgrounds(void)
{
    REG_BG1CNT = BGCNT_PRIORITY(2) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(29);
    REG_DISPCNT = DISPCNT_OBJ_ON | DISPCNT_BG1_ON | DISPCNT_BG0_ON | DISPCNT_OBJ_1D_MAP;
}

static bool8 LoadMonAndSceneGfx(struct Pokemon* mon)
{
    u16 species;
    u32 personality, trainerId;
    switch (sPokeblockLoadGfxState)
    {
    case 0:
        species = GetMonData(mon, MON_DATA_SPECIES_OR_EGG);
        personality = GetMonData(mon, MON_DATA_PERSONALITY);
        HandleLoadSpecialPokePic(&gMonFrontPicTable[species], gMonFrontPicCoords[species].size, gMonFrontPicCoords[species].y_offset, (void *)EWRAM, gMonSpriteGfxBufferPtrs[1], species, personality);
        sPokeblockLoadGfxState++;
        break;
    case 1:
        {
            const struct CompressedSpritePalette* palette;

            species = GetMonData(mon, MON_DATA_SPECIES_OR_EGG);
            personality = GetMonData(mon, MON_DATA_PERSONALITY);
            trainerId = GetMonData(mon, MON_DATA_OT_ID);
            palette = GetMonSpritePalStructFromOtIdPersonality(species, trainerId, personality);
            LoadCompressedSpritePalette(palette);
            SetMultiuseSpriteTemplateToPokemon(palette->tag, 1);
            sPokeblockLoadGfxState++;
        }
        break;
    case 2:
        LoadCompressedSpriteSheet(&gPokeblockCase_SpriteSheet);
        sPokeblockLoadGfxState++;
        break;
    case 3:
        LoadCompressedSpritePalette(&gPokeblockCase_SpritePal);
        sPokeblockLoadGfxState++;
        break;
    case 4:
        LoadCompressedSpriteSheet(&sSpriteSheet_Pokeblock);
        sPokeblockLoadGfxState++;
        break;
    case 5:
        SetPokeblockSpritePal(gSpecialVar_ItemId);
        LoadCompressedSpritePalette(&sPokeblockSpritePal);
        sPokeblockLoadGfxState++;
        break;
    case 6:
        LZDecompressVram(gBattleEnvironmentTiles_Building, (void*)(VRAM));
        sPokeblockLoadGfxState++;
        break;
    case 7:
        LZDecompressVram(gPokeblockFeedBg_Tilemap, BG_SCREEN_ADDR(29));
        sPokeblockLoadGfxState++;
        break;
    case 8:
        LoadCompressedPalette(gBattleEnvironmentPalette_BattleTower, BG_PLTT_ID(2), 3 * PLTT_SIZE_4BPP);
        sPokeblockLoadGfxState = 0;
        return TRUE;
    }
    return FALSE;
}

static void SetPokeblockSpritePal(u8 pokeblockCaseId)
{
    u8 colorId = GetPokeblockData(&gSaveBlock1.pokeblocks[pokeblockCaseId], PBLOCK_COLOR);
    sPokeblockSpritePal.data = sPokeblocksPals[colorId - 1];
    sPokeblockSpritePal.tag = TAG_POKEBLOCK;
}

#define tState           data[0]
#define tHorizontalThrow data[1]

static void Task_HandlePokeblockFeed(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        switch (gTasks[taskId].tState)
        {
        case STATE_INIT:
            sMonAnimRunState = 0;
            sTimer = 0;
            CalculateMonAnimLength();
            break;
        case STATE_START_THROW:
            DoPokeblockCaseThrowEffect(sPokeblockFeedCaseSpriteId, gTasks[taskId].tHorizontalThrow);
            break;
        case STATE_SPAWN_PBLOCK:
            sPokeblockFeedPokeblockSpriteId = CreatePokeblockSprite();
            break;
        case STATE_START_JUMP:
            StartMonJumpForPokeblock(sPokeblockFeedMonSpriteId);
            break;
        case STATE_PRINT_MSG:
            gTasks[taskId].func = Task_PrintAtePokeblockMessage;
            return;
        }
        if (sTimer < sMonAnimLength)
            UpdateMonAnim();
        else if (sTimer == sMonAnimLength)
            gTasks[taskId].tState = STATE_START_THROW - 1;

        sTimer++;
        gTasks[taskId].tState++;
    }
}

static void LaunchPokeblockFeedTask(u8 horizontalThrow)
{
    u8 taskId = CreateTask(Task_HandlePokeblockFeed, 0);
    gTasks[taskId].tState = STATE_INIT;
    gTasks[taskId].tHorizontalThrow = horizontalThrow;
}

static void Task_WaitForAtePokeblockMessage(u8 taskId)
{
    if (Menu_UpdateWindowText() == 1)
        gTasks[taskId].func = Task_FadeOutPokeblockFeed;
}

static void Task_PrintAtePokeblockMessage(u8 taskId)
{
    struct Pokemon* mon = &gPlayerParty[gPokeblockMonId];
    struct Pokeblock* pokeblock = &gSaveBlock1.pokeblocks[gSpecialVar_ItemId];

    gPokeblockGain = PokeblockGetGain(GetNature(mon), pokeblock);
    GetMonNickname(mon, gStringVar1);
    PokeblockCopyName(pokeblock, gStringVar2);

    if (gPokeblockGain == 0)
        StringExpandPlaceholders(gStringVar4, gText_Var1AteTheVar2);
    else if (gPokeblockGain > 0)
        StringExpandPlaceholders(gStringVar4, gText_Var1HappilyAteVar2);
    else
        StringExpandPlaceholders(gStringVar4, gText_Var1DisdainfullyAteVar2);

    MenuPrintMessage(gStringVar4, 1, 15);
    gTasks[taskId].func = Task_WaitForAtePokeblockMessage;
}

static void Task_ExitPokeblockFeed(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        m4aMPlayVolumeControl(&gMPlayInfo_BGM, -1, 256);
        SetMainCallback2(gMain.savedCallback);
        DestroyTask(taskId);
    }
}

static void Task_FadeOutPokeblockFeed(u8 taskId)
{
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    gTasks[taskId].func = Task_ExitPokeblockFeed;
}

#undef tState
#undef tHorizontalThrow

#define sSpeed   data[0]
#define sAccel   data[1]
#define sSpecies data[2]

static u8 CreateMonSprite(struct Pokemon* mon)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES_OR_EGG);
    u8 spriteId = CreateSprite(&gCreatingSpriteTemplate, MON_X, MON_Y, 2);

    sMonSpecies = species;
    sMonSpriteId = spriteId;
    sMonNature = GetNature(mon);
    gSprites[spriteId].sSpecies = species;
    gSprites[spriteId].callback = SpriteCallbackDummy;
    sNoMonFlip = TRUE;
    if (!IsPokeSpriteNotFlipped(species))
    {
        gSprites[spriteId].affineAnims = sSpriteAffineAnimTable_MonNoFlip;
        gSprites[spriteId].oam.affineMode = ST_OAM_AFFINE_DOUBLE;
        CalcCenterToCornerVec(&gSprites[spriteId], gSprites[spriteId].oam.shape, gSprites[spriteId].oam.size, gSprites[spriteId].oam.affineMode);
        sNoMonFlip = FALSE;
    }
    return spriteId;
}

static void StartMonJumpForPokeblock(u8 spriteId)
{
    gSprites[spriteId].x = MON_X;
    gSprites[spriteId].y = MON_Y;
    gSprites[spriteId].sSpeed = -8;
    gSprites[spriteId].sAccel = 1;
    gSprites[spriteId].callback = SpriteCB_MonJumpForPokeblock;
}

static void SpriteCB_MonJumpForPokeblock(struct Sprite* sprite)
{
    sprite->x += 4;
    sprite->y += sprite->sSpeed;
    sprite->sSpeed += sprite->sAccel;
    if (sprite->sSpeed == 0)
        PlayCry_Normal(sprite->sSpecies, 0);
    if (sprite->sSpeed == 9)
        sprite->callback = SpriteCallbackDummy;
}

static u8 CreatePokeblockCaseSpriteForFeeding(void)
{
    u8 spriteId = CreatePokeblockCaseSprite(188, 100, 2);
    gSprites[spriteId].oam.affineMode = ST_OAM_AFFINE_NORMAL;
    gSprites[spriteId].affineAnims = sAffineAnims_PokeblockCase_Still;
    gSprites[spriteId].callback = SpriteCallbackDummy;
    InitSpriteAffineAnim(&gSprites[spriteId]);
    return spriteId;
}

static void DoPokeblockCaseThrowEffect(u8 spriteId, bool8 horizontalThrow)
{
    FreeOamMatrix(gSprites[spriteId].oam.matrixNum);
    gSprites[spriteId].oam.affineMode = ST_OAM_AFFINE_DOUBLE;
    if (!horizontalThrow)
        gSprites[spriteId].affineAnims = sAffineAnims_PokeblockCase_ThrowFromVertical;
    else
        gSprites[spriteId].affineAnims = sAffineAnims_PokeblockCase_ThrowFromHorizontal;
    InitSpriteAffineAnim(&gSprites[spriteId]);
}

static u8 CreatePokeblockSprite(void)
{
    u8 spriteId = CreateSprite(&sSpriteTemplate_Pokeblock, 174, 84, 1);
    gSprites[spriteId].sSpeed = -12;
    gSprites[spriteId].sAccel = 1;
    return spriteId;
}

static void SpriteCB_ThrownPokeblock(struct Sprite* sprite)
{
    sprite->x -= 4;
    sprite->y += sprite->sSpeed;
    sprite->sSpeed += sprite->sAccel;
    if (sprite->sSpeed == 10)
        DestroySprite(sprite);
}

#undef sSpeed
#undef sAccel
#undef sSpecies

static void CalculateMonAnimLength(void)
{
    u8 animId, i;

    sMonAnimLength = 1;
    animId = sNatureToMonPokeblockAnim[sMonNature][NATURE_ANIM_ID];
    for (i = 0; i < 8; i++, animId++)
    {
        sMonAnimLength += sMonPokeblockAnims[animId][ANIMDATA_TIME];
        if (sMonPokeblockAnims[animId][ANIMDATA_IS_LAST] == TRUE)
            break;
    }
}

static void UpdateMonAnim(void)
{
    switch (sMonAnimRunState)
    {
    case 0:
        sMonAnimId = sNatureToMonPokeblockAnim[sMonNature][NATURE_ANIM_ID];
        sMonSpritePtr = &gSprites[sMonSpriteId];
        sSavedMonSprite = *sMonSpritePtr;
        sMonAnimRunState = 10;
        break;
    case 1 ... 9:
        break;
    case 10:
        InitMonAnimStage();
        if (sNatureToMonPokeblockAnim[sMonNature][NATURE_AFFINE_ANIM] != AFFINE_NONE)
        {
            sMonSpritePtr->oam.affineMode = ST_OAM_AFFINE_DOUBLE;
            sMonSpritePtr->oam.matrixNum = 0;
            sMonSpritePtr->affineAnims = sAffineAnims_Mon;
            InitSpriteAffineAnim(sMonSpritePtr);
        }
        sMonAnimRunState = 50;
    case 50:
        if (sNatureToMonPokeblockAnim[sMonNature][NATURE_AFFINE_ANIM] != AFFINE_NONE)
        {
            if (sNoMonFlip == FALSE)
                StartSpriteAffineAnim(sMonSpritePtr, sNatureToMonPokeblockAnim[sMonNature][NATURE_AFFINE_ANIM] + NUM_MON_AFFINES);
            else
                StartSpriteAffineAnim(sMonSpritePtr, sNatureToMonPokeblockAnim[sMonNature][NATURE_AFFINE_ANIM]);
        }
        sMonAnimRunState = 60;
        break;
    case 60:
        if (DoMonAnimStep() == TRUE)
        {
            if (sAnimData[ANIMDATA_IS_LAST] == FALSE)
            {
                sMonAnimId++;
                InitMonAnimStage();
                sMonAnimRunState = 60;
            }
            else
            {
                FreeOamMatrix(sMonSpritePtr->oam.matrixNum);
                sMonAnimRunState = 70;
            }
        }
        break;
    case 70:
        FreeMonSpriteOamMatrix();
        sMonAnimId = 0;
        sMonAnimRunState = 0;
        break;
    case 71 ... 90:
        break;
    }
}

static bool8 InitMonAnimStage(void)
{
    u8 i;
    for (i = 0; i < NUM_ANIMDATA; i++)
        sAnimData[i] = sMonPokeblockAnims[sMonAnimId][i];
    if (sAnimData[ANIMDATA_TIME] == 0)
        return TRUE;
    else
    {
        sAnimData[ANIMSTATE_INIT_X] = Sin(sAnimData[ANIMDATA_ROT_IDX], sAnimData[ANIMDATA_SIN_AMPLITUDE]);
        sAnimData[ANIMSTATE_INIT_Y] = Cos(sAnimData[ANIMDATA_ROT_IDX], sAnimData[ANIMDATA_COS_AMPLITUDE]);
        sAnimData[ANIMSTATE_MAX_TIME] = sAnimData[ANIMDATA_TIME];
        sAnimData[ANIMSTATE_MON_X] = sMonSpritePtr->x2;
        sAnimData[ANIMSTATE_MON_Y] = sMonSpritePtr->y2;
        CalculateMonAnimMovement();
        sAnimData[ANIMDATA_TIME] = sAnimData[ANIMSTATE_MAX_TIME];
        CalculateMonAnimMovementEnd();
        sAnimData[ANIMDATA_TIME] = sAnimData[ANIMSTATE_MAX_TIME];
        return FALSE;
    }
}

static bool8 DoMonAnimStep(void)
{
    u16 time = sAnimData[ANIMSTATE_MAX_TIME] - sAnimData[ANIMDATA_TIME];

    sMonSpritePtr->x2 = sPokeblockFeedMonAnimX[time];
    sMonSpritePtr->y2 = sPokeblockFeedMonAnimY[time];

    if (--sAnimData[ANIMDATA_TIME] == 0)
        return TRUE;
    else
        return FALSE;
}

static bool8 FreeMonSpriteOamMatrix(void)
{
    FreeSpriteOamMatrix(sMonSpritePtr);
    return FALSE;
}

static void CalculateMonAnimMovementEnd(void)
{
    u16 i;
    u16 approachTime = sAnimData[ANIMDATA_APPR_TIME];
    u16 time = sAnimData[ANIMSTATE_MAX_TIME] - approachTime;
    s16 x = sAnimData[ANIMSTATE_MON_X] + sAnimData[ANIMDATA_TARGET_X];
    s16 y = sAnimData[ANIMSTATE_MON_Y] + sAnimData[ANIMDATA_TARGET_Y];

    for (i = 0; i < time - 1; i++)
    {
        s16* xPos = &sPokeblockFeedMonAnimX[approachTime + i];
        s16 xOffset = *xPos - (x);

        s16* yPos = &sPokeblockFeedMonAnimY[approachTime + i];
        s16 yOffset = *yPos - y;

        *xPos -= xOffset * (i + 1) / time;
        *yPos -= yOffset * (i + 1) / time;
    }

    sPokeblockFeedMonAnimX[(approachTime + time) - 1] = x;
    sPokeblockFeedMonAnimY[(approachTime + time) - 1] = y;
}

static void CalculateMonAnimMovement(void)
{
    bool8 negative = FALSE;
    s16 x = sAnimData[ANIMSTATE_MON_X] - sAnimData[ANIMSTATE_INIT_X];
    s16 y = sAnimData[ANIMSTATE_MON_Y] - sAnimData[ANIMSTATE_INIT_Y];
    while (1)
    {
        u16 amplitude;
        u16 time;
        u16 acceleration;

        acceleration = abs(sAnimData[ANIMDATA_ROT_ACCEL]);
        amplitude = acceleration + sAnimData[ANIMDATA_COS_AMPLITUDE];
        sAnimData[ANIMDATA_COS_AMPLITUDE] = amplitude;

        if (sAnimData[ANIMDATA_SIN_AMPLITUDE] < 0)
            negative = TRUE;

        time = sAnimData[ANIMSTATE_MAX_TIME] - sAnimData[ANIMDATA_TIME];

        if (sAnimData[ANIMDATA_TIME] == 0)
            break;

        if (!negative)
        {
            sPokeblockFeedMonAnimX[time] = Sin(sAnimData[ANIMDATA_ROT_IDX], sAnimData[ANIMDATA_SIN_AMPLITUDE] + amplitude / 256) + x;
            sPokeblockFeedMonAnimY[time] = Cos(sAnimData[ANIMDATA_ROT_IDX], sAnimData[ANIMDATA_COS_AMPLITUDE] + amplitude / 256) + y;
        }
        else
        {
            sPokeblockFeedMonAnimX[time] = Sin(sAnimData[ANIMDATA_ROT_IDX], sAnimData[ANIMDATA_SIN_AMPLITUDE] - amplitude / 256) + x;
            sPokeblockFeedMonAnimY[time] = Cos(sAnimData[ANIMDATA_ROT_IDX], sAnimData[ANIMDATA_COS_AMPLITUDE] - amplitude / 256) + y;
        }

        sAnimData[ANIMDATA_ROT_IDX] += sAnimData[ANIMDATA_ROT_SPEED];
        sAnimData[ANIMDATA_ROT_IDX] &= 0xFF;
        sAnimData[ANIMDATA_TIME]--;
    }
}
