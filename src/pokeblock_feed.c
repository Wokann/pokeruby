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
#include "data2.h"
#include "sprite.h"
#include "item_use.h"
#include "pokeblock.h"
#include "party_menu.h"
#include "strings.h"
#include "string_util.h"
#include "m4a.h"
#include "field_effect.h"
#include "sound.h"
#include "trig.h"
#include "ewram.h"

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
    NATURE_ANIM_ID,
    NATURE_AFFINE_ANIM,
};

#define MON_X 48
#define MON_Y 80
#define TAG_POKEBLOCK 14818
#define NUM_MON_AFFINES 10

#define STATE_START_THROW 255
#define STATE_SPAWN_PBLOCK (STATE_START_THROW + 14)
#define STATE_START_JUMP (STATE_SPAWN_PBLOCK + 12)
#define STATE_PRINT_MSG (STATE_START_JUMP + 16)

extern struct MusicPlayerInfo gMPlayInfo_BGM;
extern u8 gPokeblockMonID;
extern s16 gPokeblockGain;

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
extern const struct CompressedSpriteSheet gPokeblockCase_SpriteSheet;
extern const struct CompressedSpritePalette gPokeblockCase_SpritePal;

bool8 IsPokeSpriteNotFlipped(u16 species);

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

static const u8 sNatureToMonPokeblockAnim[][2] =
{
    {  0, 0 }, // HARDY
    {  3, 0 }, // LONELY
    {  4, 1 }, // BRAVE
    {  5, 0 }, // ADAMANT
    { 10, 0 }, // NAUGHTY
    { 13, 0 }, // BOLD
    { 15, 0 }, // DOCILE
    { 16, 2 }, // RELAXED
    { 18, 0 }, // IMPISH
    { 19, 0 }, // LAX
    { 20, 0 }, // TIMID
    { 25, 0 }, // HASTY
    { 27, 3 }, // SERIOUS
    { 28, 0 }, // JOLLY
    { 29, 0 }, // NAIVE
    { 33, 4 }, // MODEST
    { 36, 0 }, // MILD
    { 37, 0 }, // QUIET
    { 39, 0 }, // BASHFUL
    { 42, 0 }, // RASH
    { 45, 0 }, // CALM
    { 46, 5 }, // GENTLE
    { 47, 6 }, // SASSY
    { 48, 0 }, // CAREFUL
    { 53, 0 }, // QUIRKY
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

static const u8* const sPokeblocksPals[] =
{
    gPokeblockRed_Pal,
    gPokeblockBlue_Pal,
    gPokeblockPink_Pal,
    gPokeblockGreen_Pal,
    gPokeblockYellow_Pal,
    gPokeblockPurple_Pal,
    gPokeblockIndigo_Pal,
    gPokeblockBrown_Pal,
    gPokeblockLiteBlue_Pal,
    gPokeblockOlive_Pal,
    gPokeblockGray_Pal,
    gPokeblockBlack_Pal,
    gPokeblockWhite_Pal,
    gPokeblockGold_Pal
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
    .affineMode = 3,
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
        ClearVideoCallbacks();
        ResetVramOamAndBgCntRegs();
        HandleInitBackgrounds();
        gMain.state++;
        break;
    case 1:
        ResetPaletteFade();
        gPaletteFade.bufferTransferDisabled = 1;
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
        Text_LoadWindowTemplate(&gWindowTemplate_81E6E50);
        gMain.state++;
        break;
    case 5:
        MultistepInitMenuWindowBegin(&gWindowTemplate_81E6E50);
        gMain.state++;
        break;
    case 6:
        if (MultistepInitMenuWindowContinue())
        {
            ePokeblockGfxState = 0;
            gMain.state++;
        }
        break;
    case 7:
        if (LoadMonAndSceneGfx(&gPlayerParty[gPokeblockMonID]))
        {
            gMain.state++;
        }
        break;
    case 8:
        ePokeblockFeedCaseSpriteId = CreatePokeblockCaseSpriteForFeeding();
        gMain.state++;
        break;
    case 9:
        ePokeblockMonSpriteId = CreateMonSprite(&gPlayerParty[gPokeblockMonID]);
        gMain.state++;
        break;
    case 10:
        Menu_DrawStdWindowFrame(0, 14, 29, 19);
        gMain.state++;
        break;
    case 11:
        if (sub_8055870() != 1)
        {
            gMain.state++;
        }
        break;
    case 12:
        {
            u16 savedIME = REG_IME;
            REG_IME = 0;
            REG_IE |= 1;
            REG_IME = savedIME;
            REG_DISPSTAT |= 8;
            SetVBlankCallback(VBlankCB_PokeblockFeed);
            gMain.state++;
        }
    case 13:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        gPaletteFade.bufferTransferDisabled = 0;
        SetMainCallback2(CB2_PokeblockFeed);
        return TRUE;
    }
    return FALSE;
}

void PreparePokeblockFeedScene(void)
{
    while (1)
    {
        if (LoadPokeblockFeedScene() == 1)
        {
            LaunchPokeblockFeedTask(1);
            break;
        }
        if (MenuHelpers_IsLinkActive() == 1)
            break;
    }
}

static void HandleInitBackgrounds(void)
{
    REG_BG1CNT = 0x1D02l;
    REG_DISPCNT = 0x1340;
}

static bool8 LoadMonAndSceneGfx(struct Pokemon* mon)
{
    u16 species;
    u32 personality, trainerId;
    switch (ePokeblockGfxState)
    {
    case 0:
        species = GetMonData(mon, MON_DATA_SPECIES2);
        personality = GetMonData(mon, MON_DATA_PERSONALITY);
        HandleLoadSpecialPokePic(&gMonFrontPicTable[species], gMonFrontPicCoords[species].coords, gMonFrontPicCoords[species].y_offset, (void *)EWRAM, gMonSpriteGfx_Sprite_ptr[1], species, personality);
        ePokeblockGfxState++;
        break;
    case 1:
        {
            const struct CompressedSpritePalette* palette;

            species = GetMonData(mon, MON_DATA_SPECIES2);
            personality = GetMonData(mon, MON_DATA_PERSONALITY);
            trainerId = GetMonData(mon, MON_DATA_OT_ID);
            palette = GetMonSpritePalStructFromOtIdPersonality(species, trainerId, personality);
            LoadCompressedObjectPalette(palette);
            SetMultiuseSpriteTemplateToPokemon(palette->tag, 1);
            ePokeblockGfxState++;
        }
        break;
    case 2:
        LoadCompressedObjectPic(&gPokeblockCase_SpriteSheet);
        ePokeblockGfxState++;
        break;
    case 3:
        LoadCompressedObjectPalette(&gPokeblockCase_SpritePal);
        ePokeblockGfxState++;
        break;
    case 4:
        LoadCompressedObjectPic(&sSpriteSheet_Pokeblock);
        ePokeblockGfxState++;
        break;
    case 5:
        SetPokeblockSpritePal(gSpecialVar_ItemId);
        LoadCompressedObjectPalette(&sPokeblockSpritePal);
        ePokeblockGfxState++;
        break;
    case 6:
        LZDecompressVram(gBattleEnvironmentTiles_Building, (void*)(VRAM));
        ePokeblockGfxState++;
        break;
    case 7:
        LZDecompressVram(gPokeblockFeedBg_Tilemap, (void*)(VRAM + 0xE800));
        ePokeblockGfxState++;
        break;
    case 8:
        LoadCompressedPalette(gBattleEnvironmentPalette_BattleTower, 0x20, 0x60);
        ePokeblockGfxState = 0;
        return TRUE;
    }
    return FALSE;
}

static void SetPokeblockSpritePal(u8 pokeblockCaseId)
{
    u8 color = GetPokeblockData(&gSaveBlock1.pokeblocks[pokeblockCaseId], PBLOCK_COLOR);
    sPokeblockSpritePal.data = sPokeblocksPals[color - 1];
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
        case 0:
            sMonAnimRunState = 0;
            sTimer = 0;
            CalculateMonAnimLength();
            break;
        case STATE_START_THROW:
            DoPokeblockCaseThrowEffect(ePokeblockFeedCaseSpriteId, gTasks[taskId].tHorizontalThrow);
            break;
        case STATE_SPAWN_PBLOCK:
            ePokeblockSpriteId = CreatePokeblockSprite();
            break;
        case STATE_START_JUMP:
            StartMonJumpForPokeblock(ePokeblockMonSpriteId);
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
    gTasks[taskId].tState = 0;
    gTasks[taskId].tHorizontalThrow = horizontalThrow;
}

static void Task_WaitForAtePokeblockMessage(u8 taskId)
{
    if (Menu_UpdateWindowText() == 1)
        gTasks[taskId].func = Task_FadeOutPokeblockFeed;
}

static void Task_PrintAtePokeblockMessage(u8 taskId)
{
    struct Pokemon* mon = &gPlayerParty[gPokeblockMonID];
    struct Pokeblock* pokeblock = &gSaveBlock1.pokeblocks[gSpecialVar_ItemId];

    gPokeblockGain = PokeblockGetGain(GetNature(mon), pokeblock);
    GetMonNickname(mon, gStringVar1);
    PokeblockCopyName(pokeblock, gStringVar2);

    if (gPokeblockGain == 0)
        StringExpandPlaceholders(gStringVar4, gContestStatsText_NormallyAte);
    else if (gPokeblockGain > 0)
        StringExpandPlaceholders(gStringVar4, gContestStatsText_HappilyAte);
    else
        StringExpandPlaceholders(gStringVar4, gContestStatsText_DisdainfullyAte);

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
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = Task_ExitPokeblockFeed;
}

#undef tState
#undef tHorizontalThrow

#define sSpeed   data[0]
#define sAccel   data[1]
#define sSpecies data[2]

static u8 CreateMonSprite(struct Pokemon* mon)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES2);
    u8 spriteId = CreateSprite(&gCreatingSpriteTemplate, MON_X, MON_Y, 2);

    sMonSpecies = species;
    sMonSpriteId = spriteId;
    sMonNature = GetNature(mon);
    gSprites[spriteId].sSpecies = species;
    gSprites[spriteId].callback = SpriteCallbackDummy;
    sNoMonFlip = 1;
    if (!IsPokeSpriteNotFlipped(species))
    {
        gSprites[spriteId].affineAnims = sSpriteAffineAnimTable_MonNoFlip;
        gSprites[spriteId].oam.affineMode = 3;
        CalcCenterToCornerVec(&gSprites[spriteId], gSprites[spriteId].oam.shape, gSprites[spriteId].oam.size, gSprites[spriteId].oam.affineMode);
        sNoMonFlip = 0;
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
    gSprites[spriteId].oam.affineMode = 1;
    gSprites[spriteId].affineAnims = sAffineAnims_PokeblockCase_Still;
    gSprites[spriteId].callback = SpriteCallbackDummy;
    InitSpriteAffineAnim(&gSprites[spriteId]);
    return spriteId;
}

static void DoPokeblockCaseThrowEffect(u8 spriteId, bool8 horizontalThrow)
{
    FreeOamMatrix(gSprites[spriteId].oam.matrixNum);
    gSprites[spriteId].oam.affineMode = 3;
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
        if (sNatureToMonPokeblockAnim[sMonNature][NATURE_AFFINE_ANIM] != 0)
        {
            sMonSpritePtr->oam.affineMode = 3;
            sMonSpritePtr->oam.matrixNum = 0;
            sMonSpritePtr->affineAnims = sAffineAnims_Mon;
            InitSpriteAffineAnim(sMonSpritePtr);
        }
        sMonAnimRunState = 50;
    case 50:
        if (sNatureToMonPokeblockAnim[sMonNature][NATURE_AFFINE_ANIM] != 0)
        {
            if (sNoMonFlip == 0)
                StartSpriteAffineAnim(sMonSpritePtr, sNatureToMonPokeblockAnim[sMonNature][NATURE_AFFINE_ANIM] + NUM_MON_AFFINES);
            else
                StartSpriteAffineAnim(sMonSpritePtr, sNatureToMonPokeblockAnim[sMonNature][NATURE_AFFINE_ANIM]);
        }
        sMonAnimRunState = 60;
        break;
    case 60:
        if (DoMonAnimStep() == 1)
        {
            if (sAnimData[ANIMDATA_IS_LAST] == 0)
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

    sMonSpritePtr->x2 = ePokeblockFeedMonAnimX[time];
    sMonSpritePtr->y2 = ePokeblockFeedMonAnimY[time];

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
        s16* xPos = &ePokeblockFeedMonAnimX[approachTime + i];
        s16 xOffset = *xPos - (x);

        s16* yPos = &ePokeblockFeedMonAnimY[approachTime + i];
        s16 yOffset = *yPos - y;

        *xPos -= xOffset * (i + 1) / time;
        *yPos -= yOffset * (i + 1) / time;
    }

    ePokeblockFeedMonAnimX[(approachTime + time) - 1] = x;
    ePokeblockFeedMonAnimY[(approachTime + time) - 1] = y;
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
            ePokeblockFeedMonAnimX[time] = Sin(sAnimData[ANIMDATA_ROT_IDX], sAnimData[ANIMDATA_SIN_AMPLITUDE] + amplitude / 256) + x;
            ePokeblockFeedMonAnimY[time] = Cos(sAnimData[ANIMDATA_ROT_IDX], sAnimData[ANIMDATA_COS_AMPLITUDE] + amplitude / 256) + y;
        }
        else
        {
            ePokeblockFeedMonAnimX[time] = Sin(sAnimData[ANIMDATA_ROT_IDX], sAnimData[ANIMDATA_SIN_AMPLITUDE] - amplitude / 256) + x;
            ePokeblockFeedMonAnimY[time] = Cos(sAnimData[ANIMDATA_ROT_IDX], sAnimData[ANIMDATA_COS_AMPLITUDE] - amplitude / 256) + y;
        }

        sAnimData[ANIMDATA_ROT_IDX] += sAnimData[ANIMDATA_ROT_SPEED];
        sAnimData[ANIMDATA_ROT_IDX] &= 0xFF;
        sAnimData[ANIMDATA_TIME]--;
    }
}
