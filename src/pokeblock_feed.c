#include "global.h"
#include "item_menu.h"
#include "task.h"
#include "palette.h"
#include "main.h"
#include "menu_helpers.h"
#include "text.h"
#include "text_window.h"
#include "menu.h"
#include "overworld.h"
#include "decompress.h"
#include "graphics.h"
#include "data.h"
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

extern const u8 gBattleEnvironmentPalette_BattleTower[];

// this file's functions
static void HandleInitBackgrounds(void);
static void CalculateMonAnimLength(void);
static void UpdateMonAnim(void);
static u8 CreatePokeblockCaseSpriteForFeeding(void);
static u8 CreatePokeblockSprite(void);
static u8 CreateMonSprite(struct Pokemon* mon);
static bool8 LoadMonAndSceneGfx(struct Pokemon* mon);
static void LaunchPokeblockFeedTask(u8);
static void StartMonJumpForPokeblock(u8);
static void SpriteCB_MonJumpForPokeblock(struct Sprite* sprite);
static void Task_PrintAtePokeblockMessage(u8 taskId);
static void Task_FadeOutPokeblockFeed(u8 taskId);
static void SetPokeblockSpritePal(u8);
static void DoPokeblockCaseThrowEffect(u8, bool8);
static bool8 InitMonAnimStage(void);
static bool8 DoMonAnimStep(void);
static bool8 FreeMonSpriteOamMatrix(void);
static void CalculateMonAnimMovement(void);
static void SpriteCB_ThrownPokeblock(struct Sprite* sprite);
static void CalculateMonAnimMovementEnd(void);

// EWRAM
EWRAM_DATA static struct CompressedSpritePalette sPokeblockSpritePal = {0};

// IWRAM common
struct Sprite* gPokeblockFeedMonSpritePtr;
u16 gPokeblockFeedMonSpecies;
bool8 gPokeblockMonNotFlipped;
u8 gPokeblockFeedMonSpriteId;
u8 gPokeblockFeedMonNature;
u16 gPokeblockFeedMonAnimLength;
u8 gPokeblockFeedUnused0;
u8 gPokeblockFeedMonAnimRunState;
u8 gPokeblockFeedMonAnimId;
struct Sprite gPokeblockFeedSavedMonSprite;
u16 gPokeblockFeedTimer;
s16 gPokeblockFeedAnimData[24];

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

static const s16 sMonPokeblockAnims[][10] =
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

static const struct OamData sThrownPokeblockOamData =
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

static const union AnimCmd sThrownPokeblockSpriteAnim[] =
{
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END
};

static const union AnimCmd *const sThrownPokeblockAnimTable[] =
{
    sThrownPokeblockSpriteAnim,
};

static const union AffineAnimCmd sAffineAnim_Pokeblock[] =
{
    AFFINEANIMCMD_FRAME(0x100, 0x100, 0, 0),
    AFFINEANIMCMD_FRAME(-8, -8, 0, 1),
    AFFINEANIMCMD_JUMP(1)
};

static const union AffineAnimCmd *const sThrownPokeblockAffineAnimTable[] =
{
    sAffineAnim_Pokeblock
};

static const struct CompressedSpriteSheet sSpriteSheet_Pokeblock =
{
    gPokeblock_Gfx, 0x20, 14818
};

static const struct SpriteTemplate sThrownPokeblockSpriteTemplate =
{
    .tileTag = 14818,
    .paletteTag = 14818,
    .oam = &sThrownPokeblockOamData,
    .anims = sThrownPokeblockAnimTable,
    .images = NULL,
    .affineAnims = sThrownPokeblockAffineAnimTable,
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
        if (LoadMonAndSceneGfx(&gPlayerParty[gPokeblockMonId]))
        {
            gMain.state++;
        }
        break;
    case 8:
        ePokeblockFeedCaseSpriteId = CreatePokeblockCaseSpriteForFeeding();
        gMain.state++;
        break;
    case 9:
        ePokeblockMonSpriteId = CreateMonSprite(&gPlayerParty[gPokeblockMonId]);
        gMain.state++;
        break;
    case 10:
        Menu_DrawStdWindowFrame(0, 14, 29, 19);
        gMain.state++;
        break;
    case 11:
        if (Overworld_IsRecvQueueAtMax() != 1)
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
        species = GetMonData(mon, MON_DATA_SPECIES_OR_EGG);
        personality = GetMonData(mon, MON_DATA_PERSONALITY);
        HandleLoadSpecialPokePic(&gMonFrontPicTable[species], gMonFrontPicCoords[species].size, gMonFrontPicCoords[species].y_offset, (void *)EWRAM, gMonSpriteGfx_Sprite_ptr[1], species, personality);
        ePokeblockGfxState++;
        break;
    case 1:
        {
            const struct CompressedSpritePalette* palette;

            species = GetMonData(mon, MON_DATA_SPECIES_OR_EGG);
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
    u8 colorId = GetPokeblockData(&gSaveBlock1.pokeblocks[pokeblockCaseId], PBLOCK_COLOR);
    sPokeblockSpritePal.data = sPokeblocksPals[colorId - 1];
    sPokeblockSpritePal.tag = 0x39E2;
}

static void Task_HandlePokeblockFeed(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        switch (gTasks[taskId].data[0])
        {
        case 0:
            gPokeblockFeedMonAnimRunState = 0;
            gPokeblockFeedTimer = 0;
            CalculateMonAnimLength();
            break;
        case 255:
            DoPokeblockCaseThrowEffect(ePokeblockFeedCaseSpriteId, gTasks[taskId].data[1]);
            break;
        case 269:
            ePokeblockSpriteId = CreatePokeblockSprite();
            break;
        case 281:
            StartMonJumpForPokeblock(ePokeblockMonSpriteId);
            break;
        case 297:
            gTasks[taskId].func = Task_PrintAtePokeblockMessage;
            return;
        }
        if (gPokeblockFeedTimer < gPokeblockFeedMonAnimLength)
            UpdateMonAnim();
        else if (gPokeblockFeedTimer == gPokeblockFeedMonAnimLength)
            gTasks[taskId].data[0] = 254;

        gPokeblockFeedTimer++;
        gTasks[taskId].data[0]++;
    }
}

static void LaunchPokeblockFeedTask(u8 horizontalThrow)
{
    u8 taskId = CreateTask(Task_HandlePokeblockFeed, 0);
    gTasks[taskId].data[0] = 0;
    gTasks[taskId].data[1] = horizontalThrow;
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
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = Task_ExitPokeblockFeed;
}

static u8 CreateMonSprite(struct Pokemon* mon)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES_OR_EGG);
    u8 spriteId = CreateSprite(&gMultiuseSpriteTemplate, 48, 80, 2);

    gPokeblockFeedMonSpecies = species;
    gPokeblockFeedMonSpriteId = spriteId;
    gPokeblockFeedMonNature = GetNature(mon);
    gSprites[spriteId].data[2] = species;
    gSprites[spriteId].callback = SpriteCallbackDummy;
    gPokeblockMonNotFlipped = 1;
    if (!IsMonSpriteNotFlipped(species))
    {
        gSprites[spriteId].affineAnims = sSpriteAffineAnimTable_MonNoFlip;
        gSprites[spriteId].oam.affineMode = 3;
        CalcCenterToCornerVec(&gSprites[spriteId], gSprites[spriteId].oam.shape, gSprites[spriteId].oam.size, gSprites[spriteId].oam.affineMode);
        gPokeblockMonNotFlipped = 0;
    }
    return spriteId;
}

static void StartMonJumpForPokeblock(u8 spriteId)
{
    gSprites[spriteId].x = 48;
    gSprites[spriteId].y = 80;
    gSprites[spriteId].data[0] = -8;
    gSprites[spriteId].data[1] = 1;
    gSprites[spriteId].callback = SpriteCB_MonJumpForPokeblock;
}

static void SpriteCB_MonJumpForPokeblock(struct Sprite* sprite)
{
    sprite->x += 4;
    sprite->y += sprite->data[0];
    sprite->data[0] += sprite->data[1];
    if (sprite->data[0] == 0)
        PlayCry_Normal(sprite->data[2], 0);
    if (sprite->data[0] == 9)
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
    u8 spriteId = CreateSprite(&sThrownPokeblockSpriteTemplate, 174, 84, 1);
    gSprites[spriteId].data[0] = -12;
    gSprites[spriteId].data[1] = 1;
    return spriteId;
}

static void SpriteCB_ThrownPokeblock(struct Sprite* sprite)
{
    sprite->x -= 4;
    sprite->y += sprite->data[0];
    sprite->data[0] += sprite->data[1];
    if (sprite->data[0] == 10)
        DestroySprite(sprite);
}

static void CalculateMonAnimLength(void)
{
    u8 animId, i;

    gPokeblockFeedMonAnimLength = 1;
    animId = sNatureToMonPokeblockAnim[gPokeblockFeedMonNature][0];
    for (i = 0; i < 8; i++, animId++)
    {
        gPokeblockFeedMonAnimLength += sMonPokeblockAnims[animId][4];
        if (sMonPokeblockAnims[animId][9] == 1)
            break;
    }
}

static void UpdateMonAnim(void)
{
    switch (gPokeblockFeedMonAnimRunState)
    {
    case 0:
        gPokeblockFeedMonAnimId = sNatureToMonPokeblockAnim[gPokeblockFeedMonNature][0];
        gPokeblockFeedMonSpritePtr = &gSprites[gPokeblockFeedMonSpriteId];
        gPokeblockFeedSavedMonSprite = *gPokeblockFeedMonSpritePtr;
        gPokeblockFeedMonAnimRunState = 10;
        break;
    case 1 ... 9:
        break;
    case 10:
        InitMonAnimStage();
        if (sNatureToMonPokeblockAnim[gPokeblockFeedMonNature][1] != 0)
        {
            gPokeblockFeedMonSpritePtr->oam.affineMode = 3;
            gPokeblockFeedMonSpritePtr->oam.matrixNum = 0;
            gPokeblockFeedMonSpritePtr->affineAnims = sAffineAnims_Mon;
            InitSpriteAffineAnim(gPokeblockFeedMonSpritePtr);
        }
        gPokeblockFeedMonAnimRunState = 50;
    case 50:
        if (sNatureToMonPokeblockAnim[gPokeblockFeedMonNature][1] != 0)
        {
            if (gPokeblockMonNotFlipped == 0)
                StartSpriteAffineAnim(gPokeblockFeedMonSpritePtr, sNatureToMonPokeblockAnim[gPokeblockFeedMonNature][1] + 10);
            else
                StartSpriteAffineAnim(gPokeblockFeedMonSpritePtr, sNatureToMonPokeblockAnim[gPokeblockFeedMonNature][1]);
        }
        gPokeblockFeedMonAnimRunState = 60;
        break;
    case 60:
        if (DoMonAnimStep() == 1)
        {
            if (gPokeblockFeedAnimData[9] == 0)
            {
                gPokeblockFeedMonAnimId++;
                InitMonAnimStage();
                gPokeblockFeedMonAnimRunState = 60;
            }
            else
            {
                FreeOamMatrix(gPokeblockFeedMonSpritePtr->oam.matrixNum);
                gPokeblockFeedMonAnimRunState = 70;
            }
        }
        break;
    case 70:
        FreeMonSpriteOamMatrix();
        gPokeblockFeedMonAnimId = 0;
        gPokeblockFeedMonAnimRunState = 0;
        break;
    case 71 ... 90:
        break;
    }
}

static bool8 InitMonAnimStage(void)
{
    u8 i;
    for (i = 0; i < 10; i++)
        gPokeblockFeedAnimData[i] = sMonPokeblockAnims[gPokeblockFeedMonAnimId][i];
    if (gPokeblockFeedAnimData[4] == 0)
        return TRUE;
    else
    {
        gPokeblockFeedAnimData[10] = Sin(gPokeblockFeedAnimData[0], gPokeblockFeedAnimData[2]);
        gPokeblockFeedAnimData[11] = Cos(gPokeblockFeedAnimData[0], gPokeblockFeedAnimData[3]);
        gPokeblockFeedAnimData[12] = gPokeblockFeedAnimData[4];
        gPokeblockFeedAnimData[13] = gPokeblockFeedMonSpritePtr->x2;
        gPokeblockFeedAnimData[14] = gPokeblockFeedMonSpritePtr->y2;
        CalculateMonAnimMovement();
        gPokeblockFeedAnimData[4] = gPokeblockFeedAnimData[12];
        CalculateMonAnimMovementEnd();
        gPokeblockFeedAnimData[4] = gPokeblockFeedAnimData[12];
        return FALSE;
    }
}

static bool8 DoMonAnimStep(void)
{
    u16 time = gPokeblockFeedAnimData[12] - gPokeblockFeedAnimData[4];

    gPokeblockFeedMonSpritePtr->x2 = ePokeblockFeedMonAnimX[time];
    gPokeblockFeedMonSpritePtr->y2 = ePokeblockFeedMonAnimY[time];

    if (--gPokeblockFeedAnimData[4] == 0)
        return TRUE;
    else
        return FALSE;
}

static bool8 FreeMonSpriteOamMatrix(void)
{
    FreeSpriteOamMatrix(gPokeblockFeedMonSpritePtr);
    return FALSE;
}

static void CalculateMonAnimMovementEnd(void)
{
    u16 i;
    u16 approachTime = gPokeblockFeedAnimData[8];
    u16 time = gPokeblockFeedAnimData[12] - approachTime;
    s16 x = gPokeblockFeedAnimData[13] + gPokeblockFeedAnimData[6];
    s16 y = gPokeblockFeedAnimData[14] + gPokeblockFeedAnimData[7];

    for (i = 0; i < time - 1; i++)
    {
        s16* r3 = &ePokeblockFeedMonAnimX[approachTime + i];
        s16 xOffset = *r3 - (x);

        s16* r5 = &ePokeblockFeedMonAnimY[approachTime + i];
        s16 yOffset = *r5 - y;

        *r3 -= xOffset * (i + 1) / time;
        *r5 -= yOffset * (i + 1) / time;
    }

    ePokeblockFeedMonAnimX[(approachTime + time) - 1] = x;
    ePokeblockFeedMonAnimY[(approachTime + time) - 1] = y;
}

static void CalculateMonAnimMovement(void)
{
    bool8 negative = FALSE;
    s16 x = gPokeblockFeedAnimData[13] - gPokeblockFeedAnimData[10];
    s16 y = gPokeblockFeedAnimData[14] - gPokeblockFeedAnimData[11];
    while (1)
    {
        u16 amplitude;
        u16 time;
        u16 acceleration;

        acceleration = abs(gPokeblockFeedAnimData[5]);
        amplitude = acceleration + gPokeblockFeedAnimData[3];
        gPokeblockFeedAnimData[3] = amplitude;

        if (gPokeblockFeedAnimData[2] < 0)
            negative = TRUE;

        time = gPokeblockFeedAnimData[12] - gPokeblockFeedAnimData[4];

        if (gPokeblockFeedAnimData[4] == 0)
            break;

        if (!negative)
        {
            ePokeblockFeedMonAnimX[time] = Sin(gPokeblockFeedAnimData[0], gPokeblockFeedAnimData[2] + amplitude / 256) + x;
            ePokeblockFeedMonAnimY[time] = Cos(gPokeblockFeedAnimData[0], gPokeblockFeedAnimData[3] + amplitude / 256) + y;
        }
        else
        {
            ePokeblockFeedMonAnimX[time] = Sin(gPokeblockFeedAnimData[0], gPokeblockFeedAnimData[2] - amplitude / 256) + x;
            ePokeblockFeedMonAnimY[time] = Cos(gPokeblockFeedAnimData[0], gPokeblockFeedAnimData[3] - amplitude / 256) + y;
        }

        gPokeblockFeedAnimData[0] += gPokeblockFeedAnimData[1];
        gPokeblockFeedAnimData[0] &= 0xFF;
        gPokeblockFeedAnimData[4]--;
    }
}
