#include "global.h"
#include "battle_transition.h"
#include "main.h"
#include "overworld.h"
#include "task.h"
#include "palette.h"
#include "trig.h"
#include "field_effect.h"
#include "field_weather.h"
#include "random.h"
#include "sprite.h"
#include "sound.h"
#include "field_camera.h"
#include "ewram.h"
#include "scanline_effect.h"
#include "constants/songs.h"
#include "constants/field_effects.h"
#include "constants/trainers.h"

void ScanlineEffect_Clear(void);

extern const struct OamData gObjectEventBaseOam_32x32;

#define PALTAG_UNUSED_MUGSHOT 0x100A

#define B_TRANS_DMA_FLAGS (1 | ((DMA_SRC_INC | DMA_DEST_FIXED | DMA_REPEAT | DMA_16BIT | DMA_START_HBLANK | DMA_ENABLE) << 16))

struct TransitionData
{
    vs8 VBlank_DMA;
    u16 WININ;
    u16 WINOUT;
    u16 WIN0H;
    u16 WIN0V;
    u16 unused1;
    u16 unused2;
    u16 BLDCNT;
    u16 BLDALPHA;
    u16 BLDY;
    s16 cameraX;
    s16 cameraY;
    s16 BG0HOFS_Lower;
    s16 BG0HOFS_Upper;
    s16 BG0VOFS;
    s16 unused3; // unused
    s16 counter;
    s16 unused4; // unused
    s16 data[11];
};

typedef bool8 (*TransitionStateFunc)(struct Task* task);
typedef bool8 (*TransitionSpriteCallback)(struct Sprite* sprite);

static void LaunchBattleTransitionTask(u8 transitionId);
static void Task_BattleTransition(u8 taskId);
static void Task_Intro(u8 taskId);
static void Task_Blur(u8 taskId);
static void Task_Swirl(u8 taskId);
static void Task_Shuffle(u8 taskId);
static void Task_BigPokeball(u8 taskId);
static void Task_PokeballsTrail(u8 taskId);
static void Task_ClockwiseWipe(u8 taskId);
static void Task_Ripple(u8 taskId);
static void Task_Wave(u8 taskId);
static void Task_Slice(u8 taskId);
static void Task_WhiteBarsFade(u8 taskId);
static void Task_GridSquares(u8 taskId);
static void Task_AngledWipes(u8 taskId);
static void Task_Sidney(u8 taskId);
static void Task_Phoebe(u8 taskId);
static void Task_Glacia(u8 taskId);
static void Task_Drake(u8 taskId);
static void Task_Steven(u8 taskId);
static bool8 Transition_StartIntro(struct Task* task);
static bool8 Transition_WaitForIntro(struct Task* task);
static bool8 Transition_StartMain(struct Task* task);
static bool8 Transition_WaitForMain(struct Task* task);
static void VBlankCB_Swirl(void);
static void HBlankCB_Swirl(void);
static void VBlankCB_Shuffle(void);
static void HBlankCB_Shuffle(void);
static void VBlankCB_PatternWeave(void);
static void VBlankCB_CircularMask(void);
static void VBlankCB_ClockwiseWipe(void);
static void VBlankCB_Ripple(void);
static void HBlankCB_Ripple(void);
static void VBlankCB_Wave(void);
static void VBlankCB_Slice(void);
static void HBlankCB_Slice(void);
static void VBlankCB_WhiteBarsFade(void);
static void VBlankCB_WhiteBarsFade_Blend(void);
static void HBlankCB_WhiteBarsFade(void);
static void VBlankCB_Mugshots(void);
static void VBlankCB_MugshotsFadeOut(void);
static void HBlankCB_Mugshots(void);
static void VBlankCB_AngledWipes(void);
static bool8 Blur_Init(struct Task* task);
static bool8 Blur_Main(struct Task* task);
static bool8 Blur_End(struct Task* task);
static bool8 Swirl_Init(struct Task* task);
static bool8 Swirl_End(struct Task* task);
static bool8 Shuffle_Init(struct Task* task);
static bool8 Shuffle_End(struct Task* task);
static bool8 BigPokeball_Init(struct Task* task);
static bool8 BigPokeball_SetGfx(struct Task* task);
static bool8 PatternWeave_Blend1(struct Task* task);
static bool8 PatternWeave_Blend2(struct Task* task);
static bool8 PatternWeave_FinishAppear(struct Task* task);
static bool8 PatternWeave_CircularMask(struct Task* task);
static bool8 PokeballsTrail_Init(struct Task* task);
static bool8 PokeballsTrail_Main(struct Task* task);
static bool8 PokeballsTrail_End(struct Task* task);
static bool8 ClockwiseWipe_Init(struct Task* task);
static bool8 ClockwiseWipe_TopRight(struct Task* task);
static bool8 ClockwiseWipe_Right(struct Task* task);
static bool8 ClockwiseWipe_Bottom(struct Task* task);
static bool8 ClockwiseWipe_Left(struct Task* task);
static bool8 ClockwiseWipe_TopLeft(struct Task* task);
static bool8 ClockwiseWipe_End(struct Task* task);
static bool8 Ripple_Init(struct Task* task);
static bool8 Ripple_Main(struct Task* task);
static bool8 Wave_Init(struct Task* task);
static bool8 Wave_Main(struct Task* task);
static bool8 Wave_End(struct Task* task);
static bool8 Slice_Init(struct Task* task);
static bool8 Slice_Main(struct Task* task);
static bool8 Slice_End(struct Task* task);
static bool8 WhiteBarsFade_Init(struct Task* task);
static bool8 WhiteBarsFade_StartBars(struct Task* task);
static bool8 WhiteBarsFade_WaitBars(struct Task* task);
static bool8 WhiteBarsFade_BlendToBlack(struct Task* task);
static bool8 WhiteBarsFade_End(struct Task* task);
static bool8 GridSquares_Init(struct Task* task);
static bool8 GridSquares_Main(struct Task* task);
static bool8 GridSquares_End(struct Task* task);
static bool8 AngledWipes_Init(struct Task* task);
static bool8 AngledWipes_SetWipeData(struct Task* task);
static bool8 AngledWipes_DoWipe(struct Task* task);
static bool8 AngledWipes_TryEnd(struct Task* task);
static bool8 AngledWipes_StartNext(struct Task* task);
static bool8 Mugshot_Init(struct Task* task);
static bool8 Mugshot_SetGfx(struct Task* task);
static bool8 Mugshot_ShowBanner(struct Task* task);
static bool8 Mugshot_StartOpponentSlide(struct Task* task);
static bool8 Mugshot_WaitStartPlayerSlide(struct Task* task);
static bool8 Mugshot_WaitPlayerSlide(struct Task* task);
static bool8 Mugshot_GradualWhiteFade(struct Task* task);
static bool8 Mugshot_InitFadeWhiteToBlack(struct Task* task);
static bool8 Mugshot_FadeToBlack(struct Task* task);
static bool8 Mugshot_End(struct Task* task);
static void DoMugshotTransition(u8 taskId);
static void Mugshots_CreateTrainerPics(struct Task* task);
static void SetTrainerPicSlideDirection(s16 spriteId, s16 value);
static void IncrementTrainerPicState(s16 spriteId);
static s16 IsTrainerPicSlideDone(s16 spriteId);
static bool8 MugshotTrainerPic_Pause(struct Sprite* sprite);
static bool8 MugshotTrainerPic_Init(struct Sprite* sprite);
static bool8 MugshotTrainerPic_Slide(struct Sprite* sprite);
static bool8 MugshotTrainerPic_SlideSlow(struct Sprite* sprite);
static bool8 MugshotTrainerPic_SlideOffscreen(struct Sprite* sprite);
static void CreateIntroTask(s16 fadeToGrayDelay, s16 fadeFromGrayDelay, s16 numFades, s16 fadeToGrayIncrement, s16 fadeFromGrayIncrement);
static bool8 IsIntroTaskDone(void);
static void Task_BattleTransition_Intro(u8 taskId);
static bool8 TransitionIntro_FadeToGray(struct Task* task);
static bool8 TransitionIntro_FadeFromGray(struct Task* task);
static void InitTransitionData(void);
static void VBlankCB_BattleTransition(void);
static void GetBg0TilesDst(u16** tilemap, u16** tileset);
static void GetBg0TilemapDst(u16** tileset);
static void FadeScreenBlack(void);
static void SetSinWave(s16* array, s16 sinAdd, s16 index, s16 indexIncrementer, s16 amplitude, s16 arrSize);
static void SetCircularMask(u16* buffer, s16 centerX, s16 centerY, s16 radius);
static void InitBlackWipe(s16* data, s16 startX, s16 startY, s16 endX, s16 endY, s16 xMove, s16 yMove);
static bool8 UpdateBlackWipe(s16* data, bool8 xExact, bool8 yExact);
static void SpriteCB_WhiteBarFade(struct Sprite* sprite);
static void SpriteCB_FldEffPokeballTrail(struct Sprite* sprite);
static void SpriteCB_MugshotTrainerPic(struct Sprite* sprite);

// const data

static const u32 sBigPokeball_Tileset[] = INCBIN_U32("graphics/battle_transitions/big_pokeball.4bpp");
static const u32 sPokeballTrail_Tileset[] = INCBIN_U32("graphics/battle_transitions/pokeball_trail.4bpp");
static const u8 sPokeball_Gfx[] = INCBIN_U8("graphics/battle_transitions/pokeball.4bpp");
static const u32 sEliteFour_Tileset[] = INCBIN_U32("graphics/battle_transitions/elite_four_bg.4bpp");
static const u8 sUnusedBrendan_Gfx[] = INCBIN_U8("graphics/battle_transitions/unused_brendan.4bpp");
static const u8 sUnusedLass_Gfx[] = INCBIN_U8("graphics/battle_transitions/unused_lass.4bpp");
static const u32 sShrinkingBoxTileset[] = INCBIN_U32("graphics/battle_transitions/shrinking_box.4bpp");

// Ruby keeps this state in its shared EWRAM buffer instead of allocating it.
static struct TransitionData * const sTransitionData = (struct TransitionData *)(gSharedMem + 0xC000);

static const TaskFunc sTasks_Intro[B_TRANSITION_COUNT] =
{
    [0 ... B_TRANSITION_COUNT - 1] = &Task_Intro
};

// After the intro each transition has a unique main task.
static const TaskFunc sTasks_Main[B_TRANSITION_COUNT] =
{
    [B_TRANSITION_BLUR] = Task_Blur,
    [B_TRANSITION_SWIRL] = Task_Swirl,
    [B_TRANSITION_SHUFFLE] = Task_Shuffle,
    [B_TRANSITION_BIG_POKEBALL] = Task_BigPokeball,
    [B_TRANSITION_POKEBALLS_TRAIL] = Task_PokeballsTrail,
    [B_TRANSITION_CLOCKWISE_WIPE] = Task_ClockwiseWipe,
    [B_TRANSITION_RIPPLE] = Task_Ripple,
    [B_TRANSITION_WAVE] = Task_Wave,
    [B_TRANSITION_SLICE] = Task_Slice,
    [B_TRANSITION_WHITE_BARS_FADE] = Task_WhiteBarsFade,
    [B_TRANSITION_GRID_SQUARES] = Task_GridSquares,
    [B_TRANSITION_ANGLED_WIPES] = Task_AngledWipes,
    [B_TRANSITION_SYDNEY] = Task_Sidney,
    [B_TRANSITION_PHOEBE] = Task_Phoebe,
    [B_TRANSITION_GLACIA] = Task_Glacia,
    [B_TRANSITION_DRAKE] = Task_Drake,
    [B_TRANSITION_STEVEN] = Task_Steven,
};

static const TransitionStateFunc sTaskHandlers[] =
{
    &Transition_StartIntro,
    &Transition_WaitForIntro,
    &Transition_StartMain,
    &Transition_WaitForMain
};

static const TransitionStateFunc sBlur_Funcs[] =
{
    Blur_Init,
    Blur_Main,
    Blur_End
};

static const TransitionStateFunc sSwirl_Funcs[] =
{
    Swirl_Init,
    Swirl_End,
};

static const TransitionStateFunc sShuffle_Funcs[] =
{
    Shuffle_Init,
    Shuffle_End,
};

static const TransitionStateFunc sBigPokeball_Funcs[] =
{
    BigPokeball_Init,
    BigPokeball_SetGfx,
    PatternWeave_Blend1,
    PatternWeave_Blend2,
    PatternWeave_FinishAppear,
    PatternWeave_CircularMask
};

static const TransitionStateFunc sPokeballsTrail_Funcs[] =
{
    PokeballsTrail_Init,
    PokeballsTrail_Main,
    PokeballsTrail_End
};

#define NUM_POKEBALL_TRAILS 5
static const s16 sPokeballsTrail_StartXCoords[2] = {-16, 256};
static const s16 sPokeballsTrail_Delays[NUM_POKEBALL_TRAILS] = {0, 32, 64, 18, 48};
static const s16 sPokeballsTrail_Speeds[2] = {8, -8};

static const TransitionStateFunc sClockwiseWipe_Funcs[] =
{
    ClockwiseWipe_Init,
    ClockwiseWipe_TopRight,
    ClockwiseWipe_Right,
    ClockwiseWipe_Bottom,
    ClockwiseWipe_Left,
    ClockwiseWipe_TopLeft,
    ClockwiseWipe_End
};

static const TransitionStateFunc sRipple_Funcs[] =
{
    Ripple_Init,
    Ripple_Main
};

static const TransitionStateFunc sWave_Funcs[] =
{
    Wave_Init,
    Wave_Main,
    Wave_End
};

static const TransitionStateFunc sMugshot_Funcs[] =
{
    Mugshot_Init,
    Mugshot_SetGfx,
    Mugshot_ShowBanner,
    Mugshot_StartOpponentSlide,
    Mugshot_WaitStartPlayerSlide,
    Mugshot_WaitPlayerSlide,
    Mugshot_GradualWhiteFade,
    Mugshot_InitFadeWhiteToBlack,
    Mugshot_FadeToBlack,
    Mugshot_End
};

static const u8 sMugshotsTrainerPicIDsTable[MUGSHOTS_COUNT] =
{
    [MUGSHOT_SYDNEY] = TRAINER_PIC_SIDNEY,
    [MUGSHOT_PHOEBE] = TRAINER_PIC_PHOEBE,
    [MUGSHOT_GLACIA] = TRAINER_PIC_GLACIA,
    [MUGSHOT_DRAKE] = TRAINER_PIC_DRAKE,
    [MUGSHOT_STEVEN] = TRAINER_PIC_STEVEN,
};
static const s16 sMugshotsOpponentRotationScales[MUGSHOTS_COUNT][2] =
{
    [MUGSHOT_SYDNEY] = {0x200, 0x200},
    [MUGSHOT_PHOEBE] = {0x200, 0x200},
    [MUGSHOT_GLACIA] = {0x1B0, 0x1B0},
    [MUGSHOT_DRAKE] = {0x1A0, 0x1A0},
    [MUGSHOT_STEVEN] = {0x188, 0x188},
};
static const s16 sMugshotsOpponentCoords[MUGSHOTS_COUNT][2] =
{
    [MUGSHOT_SYDNEY] = {0, 0},
    [MUGSHOT_PHOEBE] = {0, 0},
    [MUGSHOT_GLACIA] = {-4, 4},
    [MUGSHOT_DRAKE] = {0, 5},
    [MUGSHOT_STEVEN] = {0, 7},
};

static const TransitionSpriteCallback sMugshotTrainerPicFuncs[] =
{
    MugshotTrainerPic_Pause,
    MugshotTrainerPic_Init,
    MugshotTrainerPic_Slide,
    MugshotTrainerPic_SlideSlow,
    MugshotTrainerPic_Pause,
    MugshotTrainerPic_SlideOffscreen,
    MugshotTrainerPic_Pause
};

static const s16 sTrainerPicSlideSpeeds[2] = {12, -12};
static const s16 sTrainerPicSlideAccels[2] = {-1, 1};

static const TransitionStateFunc sSlice_Funcs[] =
{
    Slice_Init,
    Slice_Main,
    Slice_End
};

static const TransitionStateFunc sWhiteBarsFade_Funcs[] =
{
    WhiteBarsFade_Init,
    WhiteBarsFade_StartBars,
    WhiteBarsFade_WaitBars,
    WhiteBarsFade_BlendToBlack,
    WhiteBarsFade_End
};

#define NUM_WHITE_BARS 8
static const s16 sWhiteBarsFade_StartDelays[NUM_WHITE_BARS] = {0, 20, 15, 40, 10, 25, 35, 5};

static const TransitionStateFunc sGridSquares_Funcs[] =
{
    GridSquares_Init,
    GridSquares_Main,
    GridSquares_End
};

static const TransitionStateFunc sAngledWipes_Funcs[] =
{
    AngledWipes_Init,
    AngledWipes_SetWipeData,
    AngledWipes_DoWipe,
    AngledWipes_TryEnd,
    AngledWipes_StartNext
};

#define NUM_ANGLED_WIPES 7
static const s16 sAngledWipes_MoveData[NUM_ANGLED_WIPES][5] =
{
    {56,    0,      0,      160,    0},
    {104,   160,    240,    88,     1},
    {240,   72,     56,     0,      1},
    {0,     32,     144,    160,    0},
    {144,   160,    184,    0,      1},
    {56,    0,      168,    160,    0},
    {168,   160,    48,     0,      1},
};

static const s16 sAngledWipes_EndDelays[NUM_ANGLED_WIPES] = {8, 4, 2, 1, 1, 1, 0};

static const TransitionStateFunc sTransitionIntroFuncs[] =
{
    TransitionIntro_FadeToGray,
    TransitionIntro_FadeFromGray
};

static const struct SpriteFrameImage sSpriteImage_Pokeball[] =
{
    sPokeball_Gfx, 0x200
};

static const union AnimCmd sSpriteAnim_Pokeball[] =
{
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_Pokeball[] =
{
    sSpriteAnim_Pokeball
};

static const union AffineAnimCmd sSpriteAffineAnim_Pokeball1[] =
{
    AFFINEANIMCMD_FRAME(0, 0, -4, 1),
    AFFINEANIMCMD_JUMP(0)
};

static const union AffineAnimCmd sSpriteAffineAnim_Pokeball2[] =
{
    AFFINEANIMCMD_FRAME(0, 0, 4, 1),
    AFFINEANIMCMD_JUMP(0)
};

static const union AffineAnimCmd *const sSpriteAffineAnimTable_Pokeball[] =
{
    sSpriteAffineAnim_Pokeball1,
    sSpriteAffineAnim_Pokeball2
};

static const struct SpriteTemplate sSpriteTemplate_Pokeball =
{
    .tileTag = TAG_NONE,
    .paletteTag = FLDEFF_PAL_TAG_POKEBALL_TRAIL,
    .oam = &gObjectEventBaseOam_32x32,
    .anims = sSpriteAnimTable_Pokeball,
    .images = sSpriteImage_Pokeball,
    .affineAnims = sSpriteAffineAnimTable_Pokeball,
    .callback = SpriteCB_FldEffPokeballTrail
};

static const struct OamData sOam_UnusedBrendanLass =
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
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};

static const struct SpriteFrameImage sImageTable_UnusedBrendan[] =
{
    sUnusedBrendan_Gfx, 0x800
};

static const struct SpriteFrameImage sImageTable_UnusedLass[] =
{
    sUnusedLass_Gfx, 0x800
};

static const union AnimCmd sSpriteAnim_UnusedBrendanLass[] =
{
    ANIMCMD_FRAME(0, 1),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_UnusedBrendanLass[] =
{
    sSpriteAnim_UnusedBrendanLass
};

static const struct SpriteTemplate sSpriteTemplate_UnusedBrendan =
{
    .tileTag = TAG_NONE,
    .paletteTag = PALTAG_UNUSED_MUGSHOT,
    .oam = &sOam_UnusedBrendanLass,
    .anims = sSpriteAnimTable_UnusedBrendanLass,
    .images = sImageTable_UnusedBrendan,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_MugshotTrainerPic
};

static const struct SpriteTemplate sSpriteTemplate_UnusedLass =
{
    .tileTag = TAG_NONE,
    .paletteTag = PALTAG_UNUSED_MUGSHOT,
    .oam = &sOam_UnusedBrendanLass,
    .anims = sSpriteAnimTable_UnusedBrendanLass,
    .images = sImageTable_UnusedLass,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_MugshotTrainerPic
};

static const u16 sFieldEffectPal_Pokeball[] = INCBIN_U16("graphics/field_effects/palettes/pokeball.gbapal");

const struct SpritePalette gSpritePalette_Pokeball =
{
    sFieldEffectPal_Pokeball, FLDEFF_PAL_TAG_POKEBALL_TRAIL
};

static const u16 sMugshotPal_Sydney[] = INCBIN_U16("graphics/battle_transitions/sidney_bg.gbapal");
static const u16 sMugshotPal_Phoebe[] = INCBIN_U16("graphics/battle_transitions/phoebe_bg.gbapal");
static const u16 sMugshotPal_Glacia[] = INCBIN_U16("graphics/battle_transitions/glacia_bg.gbapal");
static const u16 sMugshotPal_Drake[] = INCBIN_U16("graphics/battle_transitions/drake_bg.gbapal");
static const u16 sMugshotPal_Steven[] = INCBIN_U16("graphics/battle_transitions/steven_bg.gbapal");
static const u16 sMugshotPal_Brendan[] = INCBIN_U16("graphics/battle_transitions/brendan_bg.gbapal");
static const u16 sMugshotPal_May[] = INCBIN_U16("graphics/battle_transitions/may_bg.gbapal");

static const u16 * const sOpponentMugshotsPals[MUGSHOTS_COUNT] =
{
    [MUGSHOT_SYDNEY] = sMugshotPal_Sydney,
    [MUGSHOT_PHOEBE] = sMugshotPal_Phoebe,
    [MUGSHOT_GLACIA] = sMugshotPal_Glacia,
    [MUGSHOT_DRAKE] = sMugshotPal_Drake,
    [MUGSHOT_STEVEN] = sMugshotPal_Steven,
};

static const u16 * const sPlayerMugshotsPals[2] =
{
    [MALE] = sMugshotPal_Brendan,
    [FEMALE] = sMugshotPal_May,
};

static const u16 sUnusedTrainerPalette[] = INCBIN_U16("graphics/battle_transitions/unused_trainer.gbapal");
static const struct SpritePalette sSpritePalette_UnusedTrainer =
{
    sUnusedTrainerPalette, PALTAG_UNUSED_MUGSHOT
};

static const u16 sBigPokeball_Tilemap[] = INCBIN_U16("graphics/battle_transitions/big_pokeball_map.bin");
static const u16 sMugshotsTilemap[] = INCBIN_U16("graphics/battle_transitions/elite_four_bg_map.bin");

// actual code starts here

void BattleTransition_StartOnField(u8 transitionId)
{
    gMain.callback2 = CB2_OverworldBasic;
    LaunchBattleTransitionTask(transitionId);
}

void BattleTransition_Start(u8 transitionId)
{
    LaunchBattleTransitionTask(transitionId);
}

#define tState          data[0]
#define tTransitionId   data[1]
#define tTransitionDone data[15]

#define tWipeStartX data[0]
#define tWipeStartY data[1]
#define tWipeCurrX  data[2]
#define tWipeCurrY  data[3]
#define tWipeEndX   data[4]
#define tWipeEndY   data[5]
#define tWipeXMove  data[6]
#define tWipeYMove  data[7]
#define tWipeXDist  data[8]
#define tWipeYDist  data[9]
#define tWipeTemp   data[10]

bool8 IsBattleTransitionDone(void)
{
    u8 taskId = FindTaskIdByFunc(Task_BattleTransition);
    if (gTasks[taskId].tTransitionDone)
    {
        DestroyTask(taskId);
        return TRUE;
    }
    else
        return FALSE;
}

static void LaunchBattleTransitionTask(u8 transitionId)
{
    u8 taskId = CreateTask(Task_BattleTransition, 2);
    gTasks[taskId].tTransitionId = transitionId;
}

static void Task_BattleTransition(u8 taskId)
{
    while (sTaskHandlers[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 Transition_StartIntro(struct Task* task)
{
    SetWeatherScreenFadeOut();
    CpuCopy32(gPlttBufferFaded, gPlttBufferUnfaded, 0x400);
    if (sTasks_Intro[task->tTransitionId] != NULL)
    {
        CreateTask(sTasks_Intro[task->tTransitionId], 4);
        task->tState++;
        return FALSE;
    }
    else
    {
        task->tState = 2;
        return TRUE;
    }
}

static bool8 Transition_WaitForIntro(struct Task* task)
{
    if (FindTaskIdByFunc(sTasks_Intro[task->tTransitionId]) == 0xFF)
    {
        task->tState++;
        return TRUE;
    }
    else
        return FALSE;
}

static bool8 Transition_StartMain(struct Task* task)
{
    CreateTask(sTasks_Main[task->tTransitionId], 0);
    task->tState++;
    return FALSE;
}

static bool8 Transition_WaitForMain(struct Task* task)
{
    task->tTransitionDone = 0;
    if (FindTaskIdByFunc(sTasks_Main[task->tTransitionId]) == 0xFF)
        task->tTransitionDone = 1;
    return FALSE;
}

static void Task_Intro(u8 taskId)
{
    if (gTasks[taskId].tState == 0)
    {
        gTasks[taskId].tState++;
        CreateIntroTask(0, 0, 3, 2, 2);
    }
    else if (IsIntroTaskDone())
        DestroyTask(taskId);
}

#define tDelay   data[1]
#define tCounter data[2]

static void Task_Blur(u8 taskId)
{
    while (sBlur_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 Blur_Init(struct Task* task)
{
    REG_MOSAIC = 0;
    REG_BG1CNT |= BGCNT_MOSAIC;
    REG_BG2CNT |= BGCNT_MOSAIC;
    REG_BG3CNT |= BGCNT_MOSAIC;
    task->tState++;
    return TRUE;
}

static bool8 Blur_Main(struct Task* task)
{
    if (task->tDelay != 0)
        task->tDelay--;
    else
    {
        task->tDelay = 4;
        if (++task->tCounter == 10)
            BeginNormalPaletteFade(0xFFFFFFFF, -1, 0, 16, RGB(0, 0, 0));
        REG_MOSAIC = (task->tCounter & 15) * 17;
        if (task->tCounter > 14)
            task->tState++;
    }
    return FALSE;
}

static bool8 Blur_End(struct Task* task)
{
    if (!gPaletteFade.active)
    {
        u8 taskId = FindTaskIdByFunc(Task_Blur);
        DestroyTask(taskId);
    }
    return FALSE;
}

#undef tDelay
#undef tCounter

#define tSinIndex  data[1]
#define tAmplitude data[2]

static void Task_Swirl(u8 taskId)
{
    while (sSwirl_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 Swirl_Init(struct Task* task)
{
    u16 savedIME;

    InitTransitionData();
    ScanlineEffect_Clear();
    BeginNormalPaletteFade(0xFFFFFFFF, 4, 0, 16, RGB(0, 0, 0));
    SetSinWave(gScanlineEffectRegBuffers[1], sTransitionData->cameraX, 0, 2, 0, 160);

    SetVBlankCallback(VBlankCB_Swirl);
    SetHBlankCallback(HBlankCB_Swirl);

    savedIME = REG_IME;
    REG_IME = 0;
    REG_IE |= (INTR_FLAG_VBLANK | INTR_FLAG_HBLANK);
    REG_IME = savedIME;
    REG_DISPSTAT |= (DISPSTAT_VBLANK_INTR | DISPSTAT_HBLANK_INTR);

    task->tState++;
    return FALSE;
}

static bool8 Swirl_End(struct Task* task)
{
    sTransitionData->VBlank_DMA = 0;
    task->tSinIndex += 4;
    task->tAmplitude += 8;

    SetSinWave(gScanlineEffectRegBuffers[0], sTransitionData->cameraX, task->tSinIndex, 2, task->tAmplitude, 160);

    if (!gPaletteFade.active)
    {
        u8 taskId = FindTaskIdByFunc(Task_Swirl);
        DestroyTask(taskId);
    }

    sTransitionData->VBlank_DMA++;
    return FALSE;
}

static void VBlankCB_Swirl(void)
{
    VBlankCB_BattleTransition();
    if (sTransitionData->VBlank_DMA)
        DmaCopy16(3, gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], 320);
}

static void HBlankCB_Swirl(void)
{
    u16 var = gScanlineEffectRegBuffers[1][REG_VCOUNT];
    REG_BG1HOFS = var;
    REG_BG2HOFS = var;
    REG_BG3HOFS = var;
}

#undef tSinIndex
#undef tAmplitude

#define tSinVal    data[1]
#define tAmplitude data[2]

static void Task_Shuffle(u8 taskId)
{
    while (sShuffle_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 Shuffle_Init(struct Task* task)
{
    u16 savedIME;

    InitTransitionData();
    ScanlineEffect_Clear();

    BeginNormalPaletteFade(0xFFFFFFFF, 4, 0, 16, RGB(0, 0, 0));
    memset(gScanlineEffectRegBuffers[1], sTransitionData->cameraY, 0x140);

    SetVBlankCallback(VBlankCB_Shuffle);
    SetHBlankCallback(HBlankCB_Shuffle);

    savedIME = REG_IME;
    REG_IME = 0;
    REG_IE |= (INTR_FLAG_VBLANK | INTR_FLAG_HBLANK);
    REG_IME = savedIME;
    REG_DISPSTAT |= (DISPSTAT_VBLANK_INTR | DISPSTAT_HBLANK_INTR);

    task->tState++;
    return FALSE;
}

static bool8 Shuffle_End(struct Task* task)
{
    u8 i;
    u16 amplitude, sinVal;

    sTransitionData->VBlank_DMA = 0;
    sinVal = task->tSinVal;
    amplitude = task->tAmplitude >> 8;
    task->tSinVal += 4224;
    task->tAmplitude += 384;

    for (i = 0; i < 160; i++, sinVal += 4224)
    {
        u16 sinIndex = sinVal / 256;
        gScanlineEffectRegBuffers[0][i] = sTransitionData->cameraY + Sin(sinIndex, amplitude);
    }

    if (!gPaletteFade.active)
        DestroyTask(FindTaskIdByFunc(Task_Shuffle));

    sTransitionData->VBlank_DMA++;
    return FALSE;
}

static void VBlankCB_Shuffle(void)
{
    VBlankCB_BattleTransition();
    if (sTransitionData->VBlank_DMA)
        DmaCopy16(3, gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], 320);
}

static void HBlankCB_Shuffle(void)
{
    u16 var = gScanlineEffectRegBuffers[1][REG_VCOUNT];
    REG_BG1VOFS = var;
    REG_BG2VOFS = var;
    REG_BG3VOFS = var;
}

#undef tSinVal
#undef tAmplitude

#define tBlendTarget1 data[1]
#define tBlendTarget2 data[2]
#define tBlendDelay   data[3]

#define tRadius      data[1]
#define tRadiusDelta data[2]
#define tVBlankSet   data[3]

#define tSinIndex  data[4]
#define tAmplitude data[5]

static void Task_BigPokeball(u8 taskId)
{
    while (sBigPokeball_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 BigPokeball_Init(struct Task* task)
{
    u16 i;
    u16 *tilemap, *tileset;

    InitTransitionData();
    ScanlineEffect_Clear();

    task->tBlendTarget1 = 16;
    task->tBlendTarget2 = 0;
    task->tSinIndex = 0;
    task->tAmplitude = 0x4000;
    sTransitionData->WININ = 63;
    sTransitionData->WINOUT = 0;
    sTransitionData->WIN0H = 240;
    sTransitionData->WIN0V = 160;
    sTransitionData->BLDCNT = 0x3F41;
    sTransitionData->BLDALPHA = task->tBlendTarget1 * 256; // 16 * 256 = 0x1000

    for (i = 0; i < 160; i++)
    {
        gScanlineEffectRegBuffers[1][i] = 240;
    }

    SetVBlankCallback(VBlankCB_PatternWeave);

    GetBg0TilesDst(&tilemap, &tileset);
    CpuFill16(0, tilemap, 0x800);
    CpuSet(sBigPokeball_Tileset, tileset, 0x2C0);
    LoadPalette(sFieldEffectPal_Pokeball, 240, 32);

    task->tState++;
    return FALSE;
}

static bool8 BigPokeball_SetGfx(struct Task* task)
{
    s16 i, j;
    u16 *tilemap, *tileset;
    const u16 *bigPokeballMap;

    bigPokeballMap = sBigPokeball_Tilemap;
    GetBg0TilesDst(&tilemap, &tileset);
    for (i = 0; i < 20; i++)
    {
        for (j = 0; j < 30; j++, bigPokeballMap++)
        {
            tilemap[i * 32 + j] = *bigPokeballMap | 0xF000;
        }
    }
    SetSinWave(gScanlineEffectRegBuffers[0], 0, task->tSinIndex, 132, task->tAmplitude, 160);

    task->tState++;
    return TRUE;
}

static bool8 PatternWeave_Blend1(struct Task* task)
{
    sTransitionData->VBlank_DMA = 0;
    if (task->tBlendDelay == 0 || --task->tBlendDelay == 0)
    {
        task->tBlendTarget2++;
        task->tBlendDelay = 2;
    }
    sTransitionData->BLDALPHA = (task->tBlendTarget1 << 8) | task->tBlendTarget2;
    if (task->tBlendTarget2 > 15)
        task->tState++;
    task->tSinIndex += 8;
    task->tAmplitude -= 256;

    SetSinWave(gScanlineEffectRegBuffers[0], 0, task->tSinIndex, 132, task->tAmplitude >> 8, 160);

    sTransitionData->VBlank_DMA++;
    return FALSE;
}

static bool8 PatternWeave_Blend2(struct Task* task)
{
    sTransitionData->VBlank_DMA = 0;
    if (task->tBlendDelay == 0 || --task->tBlendDelay == 0)
    {
        task->tBlendTarget1--;
        task->tBlendDelay = 2;
    }
    sTransitionData->BLDALPHA = (task->tBlendTarget1 << 8) | task->tBlendTarget2;
    if (task->tBlendTarget1 == 0)
        task->tState++;
    task->tSinIndex += 8;
    task->tAmplitude -= 256;

    SetSinWave(gScanlineEffectRegBuffers[0], 0, task->tSinIndex, 132, task->tAmplitude >> 8, 160);

    sTransitionData->VBlank_DMA++;
    return FALSE;
}

static bool8 PatternWeave_FinishAppear(struct Task* task)
{
    sTransitionData->VBlank_DMA = 0;
    task->tSinIndex += 8;
    task->tAmplitude -= 256;

    SetSinWave(gScanlineEffectRegBuffers[0], 0, task->tSinIndex, 132, task->tAmplitude >> 8, 160);

    if (task->tAmplitude <= 0)
    {
        task->tState++;
        task->tRadius = 160;
        task->tRadiusDelta = 256;
        task->tVBlankSet = 0;
    }

    sTransitionData->VBlank_DMA++;
    return FALSE;
}

static bool8 PatternWeave_CircularMask(struct Task* task)
{
    sTransitionData->VBlank_DMA = 0;
    if (task->tRadiusDelta < 1024)
        task->tRadiusDelta += 128;
    if (task->tRadius != 0)
    {
        task->tRadius -= (task->tRadiusDelta >> 8);
        if (task->tRadius < 0)
            task->tRadius = 0;
    }
    SetCircularMask(gScanlineEffectRegBuffers[0], 120, 80, task->tRadius);
    if (task->tRadius == 0)
    {
        DmaStop(0);
        FadeScreenBlack();
        DestroyTask(FindTaskIdByFunc(Task_BigPokeball));
    }
    if (task->tVBlankSet == 0)
    {
        task->tVBlankSet++;
        SetVBlankCallback(VBlankCB_CircularMask);
    }

    sTransitionData->VBlank_DMA++;
    return FALSE;
}

static void VBlankCB_SetWinAndBlend(void)
{
    DmaStop(0);
    VBlankCB_BattleTransition();
    if (sTransitionData->VBlank_DMA)
        DmaCopy16(3, gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], 320);
    REG_WININ = sTransitionData->WININ;
    REG_WINOUT = sTransitionData->WINOUT;
    REG_WIN0V = sTransitionData->WIN0V;
    REG_BLDCNT = sTransitionData->BLDCNT;
    REG_BLDALPHA = sTransitionData->BLDALPHA;
}

static void VBlankCB_PatternWeave(void)
{
    VBlankCB_SetWinAndBlend();
    DmaSet(0, gScanlineEffectRegBuffers[1], &REG_BG0HOFS, B_TRANS_DMA_FLAGS);
}

static void VBlankCB_CircularMask(void)
{
    VBlankCB_SetWinAndBlend();
    DmaSet(0, gScanlineEffectRegBuffers[1], &REG_WIN0H, B_TRANS_DMA_FLAGS);
}

#undef tBlendTarget1
#undef tBlendTarget2
#undef tBlendDelay
#undef tRadius
#undef tRadiusDelta
#undef tVBlankSet
#undef tSinIndex
#undef tAmplitude

#define sSide data[0]
#define sDelay data[1]
#define sPrevX data[2]

static void Task_PokeballsTrail(u8 taskId)
{
    while (sPokeballsTrail_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 PokeballsTrail_Init(struct Task* task)
{
    u16 *dst1, *dst2;

    GetBg0TilesDst(&dst1, &dst2);
    CpuSet(sPokeballTrail_Tileset, dst2, 0x20);
    CpuFill32(0, dst1, 0x800);
    LoadPalette(sFieldEffectPal_Pokeball, 0xF0, 0x20);

    task->tState++;
    return FALSE;
}

static bool8 PokeballsTrail_Main(struct Task* task)
{
    s16 i;
    s16 side;
    s16 startX[2];
    s16 delays[NUM_POKEBALL_TRAILS];

    memcpy(startX, sPokeballsTrail_StartXCoords, sizeof(sPokeballsTrail_StartXCoords));
    memcpy(delays, sPokeballsTrail_Delays, sizeof(sPokeballsTrail_Delays));
    side = Random() & 1;
    for (i = 0; i < NUM_POKEBALL_TRAILS; i++, side ^= 1)
    {
        gFieldEffectArguments[0] = startX[side];     // x
        gFieldEffectArguments[1] = (i * 32) + 16;   // y
        gFieldEffectArguments[2] = side;
        gFieldEffectArguments[3] = delays[i];
        FieldEffectStart(FLDEFF_POKEBALL_TRAIL);
    }

    task->tState++;
    return FALSE;
}

static bool8 PokeballsTrail_End(struct Task* task)
{
    if (!FieldEffectActiveListContains(FLDEFF_POKEBALL_TRAIL))
    {
        FadeScreenBlack();
        DestroyTask(FindTaskIdByFunc(Task_PokeballsTrail));
    }
    return FALSE;
}

bool8 FldEff_PokeballTrail(void)
{
    u8 spriteId = CreateSpriteAtEnd(&sSpriteTemplate_Pokeball, gFieldEffectArguments[0], gFieldEffectArguments[1], 0);
    gSprites[spriteId].oam.priority = 0;
    gSprites[spriteId].oam.affineMode = 1;
    gSprites[spriteId].sSide = gFieldEffectArguments[2];
    gSprites[spriteId].sDelay = gFieldEffectArguments[3];
    gSprites[spriteId].sPrevX = -1;
    InitSpriteAffineAnim(&gSprites[spriteId]);
    StartSpriteAffineAnim(&gSprites[spriteId], gFieldEffectArguments[2]);
    return FALSE;
}

#define SET_TILE(ptr, posY, posX, toStore)                               \
{                                                                       \
    u32 index = (posY) * 32 + posX;                                     \
    ptr[index] = toStore;                                               \
}

static void SpriteCB_FldEffPokeballTrail(struct Sprite* sprite)
{
    s16 speeds[2];

    memcpy(speeds, sPokeballsTrail_Speeds, sizeof(sPokeballsTrail_Speeds));
    if (sprite->sDelay != 0)
        sprite->sDelay--;
    else
    {
        if (sprite->x >= 0 && sprite->x <= 240)
        {
            s16 posX = sprite->x >> 3;
            s16 posY = sprite->y >> 3;

            if (posX != sprite->sPrevX)
            {
                u32 charBase;
                u16 *ptr;

                sprite->sPrevX = posX;
                charBase = ((REG_BG0CNT >> 8) & 0x1F) << 11;
                ptr = (u16 *)(VRAM + charBase);

                SET_TILE(ptr, posY - 2, posX, 0xF001);
                SET_TILE(ptr, posY - 1, posX, 0xF001);
                SET_TILE(ptr, posY - 0, posX, 0xF001);
                SET_TILE(ptr, posY + 1, posX, 0xF001);
            }
        }
        sprite->x += speeds[sprite->sSide];
        if (sprite->x < -15 || sprite->x > 255)
            FieldEffectStop(sprite, FLDEFF_POKEBALL_TRAIL);
    }
}

#undef SET_TILE
#undef sSide
#undef sDelay
#undef sPrevX

static void Task_ClockwiseWipe(u8 taskId)
{
    while (sClockwiseWipe_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 ClockwiseWipe_Init(struct Task* task)
{
    u16 i;

    InitTransitionData();
    ScanlineEffect_Clear();

    sTransitionData->WININ = 0;
    sTransitionData->WINOUT = 63;
    sTransitionData->WIN0H = -3855;
    sTransitionData->WIN0V = 160;

    for (i = 0; i < 160; i++)
    {
        gScanlineEffectRegBuffers[1][i] = 0xF3F4;
    }

    SetVBlankCallback(VBlankCB_ClockwiseWipe);
    sTransitionData->tWipeEndX = 120;

    task->tState++;
    return TRUE;
}

static bool8 ClockwiseWipe_TopRight(struct Task* task)
{
    sTransitionData->VBlank_DMA = 0;

    InitBlackWipe(sTransitionData->data, 120, 80, sTransitionData->tWipeEndX, -1, 1, 1);
    do
    {
        gScanlineEffectRegBuffers[0][sTransitionData->tWipeCurrY] = (sTransitionData->tWipeCurrX + 1) | 0x7800;
    } while (!UpdateBlackWipe(sTransitionData->data, 1, 1));

    sTransitionData->tWipeEndX += 16;
    if (sTransitionData->tWipeEndX >= 240)
    {
        sTransitionData->tWipeEndY = 0;
        task->tState++;
    }

    sTransitionData->VBlank_DMA++;
    return FALSE;
}

static bool8 ClockwiseWipe_Right(struct Task* task)
{
    s16 start, end;
    vu8 finished = FALSE;

    sTransitionData->VBlank_DMA = 0;

    InitBlackWipe(sTransitionData->data, 120, 80, 240, sTransitionData->tWipeEndY, 1, 1);

    while (1)
    {
        start = 120, end = sTransitionData->tWipeCurrX + 1;
        if (sTransitionData->tWipeEndY >= 80)
            start = sTransitionData->tWipeCurrX, end = 240;
        gScanlineEffectRegBuffers[0][sTransitionData->tWipeCurrY] = end | (start << 8);
        if (finished)
            break;
        finished = UpdateBlackWipe(sTransitionData->data, TRUE, TRUE);
    }

    sTransitionData->tWipeEndY += 8;
    if (sTransitionData->tWipeEndY >= 160)
    {
        sTransitionData->tWipeEndX = 240;
        task->tState++;
    }
    else
    {
        while (sTransitionData->tWipeCurrY < sTransitionData->tWipeEndY)
        {
            gScanlineEffectRegBuffers[0][++sTransitionData->tWipeCurrY] = end | (start << 8);
        }
    }

    sTransitionData->VBlank_DMA++;
    return FALSE;
}

static bool8 ClockwiseWipe_Bottom(struct Task* task)
{
    sTransitionData->VBlank_DMA = 0;

    InitBlackWipe(sTransitionData->data, 120, 80, sTransitionData->tWipeEndX, 160, 1, 1);
    do
    {
        gScanlineEffectRegBuffers[0][sTransitionData->tWipeCurrY] = (sTransitionData->tWipeCurrX << 8) | 0xF0;
    } while (!UpdateBlackWipe(sTransitionData->data, 1, 1));

    sTransitionData->tWipeEndX -= 16;
    if (sTransitionData->tWipeEndX <= 0)
    {
        sTransitionData->tWipeEndY = 160;
        task->tState++;
    }

    sTransitionData->VBlank_DMA++;
    return FALSE;
}

static bool8 ClockwiseWipe_Left(struct Task* task)
{
    s16 end, start, temp;
    vu8 finished = FALSE;

    sTransitionData->VBlank_DMA = 0;

    InitBlackWipe(sTransitionData->data, 120, 80, 0, sTransitionData->tWipeEndY, 1, 1);

    while (1)
    {
        end = gScanlineEffectRegBuffers[0][sTransitionData->tWipeCurrY] & 0xFF, start = sTransitionData->tWipeCurrX;
        if (sTransitionData->tWipeEndY <= 80)
            start = 120, end = sTransitionData->tWipeCurrX;
        gScanlineEffectRegBuffers[0][sTransitionData->tWipeCurrY] = end | (start << 8);
        temp = 0;
        if (finished)
            break;
        finished = UpdateBlackWipe(sTransitionData->data, TRUE, TRUE);
    }

    sTransitionData->tWipeEndY -= 8;
    if (sTransitionData->tWipeEndY <= 0)
    {
        sTransitionData->tWipeEndX = temp;
        task->tState++;
    }
    else
    {
        while (sTransitionData->tWipeCurrY > sTransitionData->tWipeEndY)
        {
            gScanlineEffectRegBuffers[0][--sTransitionData->tWipeCurrY] = end | (start << 8);
        }
    }

    sTransitionData->VBlank_DMA++;
    return FALSE;
}

static bool8 ClockwiseWipe_TopLeft(struct Task* task)
{
    sTransitionData->VBlank_DMA = 0;

    InitBlackWipe(sTransitionData->data, 120, 80, sTransitionData->tWipeEndX, 0, 1, 1);
    do
    {
        s16 start, end;

        start = 120, end = sTransitionData->tWipeCurrX;
        if (sTransitionData->tWipeCurrX >= 120)
            start = 0, end = 240;
        gScanlineEffectRegBuffers[0][sTransitionData->tWipeCurrY] = end | (start << 8);

    } while (!UpdateBlackWipe(sTransitionData->data, 1, 1));

    sTransitionData->tWipeEndX += 16;
    if (sTransitionData->tWipeCurrX > 120)
        task->tState++;

    sTransitionData->VBlank_DMA++;
    return FALSE;
}

static bool8 ClockwiseWipe_End(struct Task* task)
{
    DmaStop(0);
    FadeScreenBlack();
    DestroyTask(FindTaskIdByFunc(Task_ClockwiseWipe));
    return FALSE;
}

static void VBlankCB_ClockwiseWipe(void)
{
    DmaStop(0);
    VBlankCB_BattleTransition();
    if (sTransitionData->VBlank_DMA != 0)
        DmaCopy16(3, gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], 320);
    REG_WININ = sTransitionData->WININ;
    REG_WINOUT = sTransitionData->WINOUT;
    REG_WIN0V = sTransitionData->WIN0V;
    REG_WIN0H = gScanlineEffectRegBuffers[1][0];
    DmaSet(0, gScanlineEffectRegBuffers[1], &REG_WIN0H, B_TRANS_DMA_FLAGS);
}

#define tSinVal       data[1]
#define tAmplitudeVal data[2]
#define tTimer        data[3]
#define tFadeStarted  data[4]

static void Task_Ripple(u8 taskId)
{
    while (sRipple_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 Ripple_Init(struct Task* task)
{
    u8 i;

    InitTransitionData();
    ScanlineEffect_Clear();

    for (i = 0; i < 160; i++)
    {
        gScanlineEffectRegBuffers[1][i] = sTransitionData->cameraY;
    }

    SetVBlankCallback(VBlankCB_Ripple);
    SetHBlankCallback(HBlankCB_Ripple);

    REG_IE |= INTR_FLAG_HBLANK;
    REG_DISPSTAT |= DISPSTAT_HBLANK_INTR;

    task->tState++;
    return TRUE;
}

static bool8 Ripple_Main(struct Task* task)
{
    u8 i;
    s16 amplitude;
    u16 sinVal, speed;

    sTransitionData->VBlank_DMA = 0;

    amplitude = task->tAmplitudeVal >> 8;
    sinVal = task->tSinVal;
    speed = 384;
    task->tSinVal += 0x400;
    if (task->tAmplitudeVal <= 0x1FFF)
        task->tAmplitudeVal += 0x180;

    for (i = 0; i < 160; i++, sinVal += speed)
    {
        // todo: fix the asm
        s16 sinIndex = sinVal >> 8;
        asm("");
        gScanlineEffectRegBuffers[0][i] = sTransitionData->cameraY + Sin(sinIndex, amplitude);
        asm("");
    }

    if (++task->tTimer == 81)
    {
        task->tFadeStarted++;
        BeginNormalPaletteFade(0xFFFFFFFF, -2, 0, 16, RGB(0, 0, 0));
    }

    if (task->tFadeStarted != 0 && !gPaletteFade.active)
        DestroyTask(FindTaskIdByFunc(Task_Ripple));

    sTransitionData->VBlank_DMA++;
    return FALSE;
}

static void VBlankCB_Ripple(void)
{
    VBlankCB_BattleTransition();
    if (sTransitionData->VBlank_DMA)
        DmaCopy16(3, gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], 320);
}

static void HBlankCB_Ripple(void)
{
    u16 var = gScanlineEffectRegBuffers[1][REG_VCOUNT];
    REG_BG1VOFS = var;
    REG_BG2VOFS = var;
    REG_BG3VOFS = var;
}

#undef tSinVal
#undef tAmplitudeVal
#undef tTimer
#undef tFadeStarted

#define tX        data[1]
#define tSinIndex data[2]

static void Task_Wave(u8 taskId)
{
    while (sWave_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 Wave_Init(struct Task* task)
{
    u8 i;

    InitTransitionData();
    ScanlineEffect_Clear();

    sTransitionData->WININ = 63;
    sTransitionData->WINOUT = 0;
    sTransitionData->WIN0H = 240;
    sTransitionData->WIN0V = 160;

    for (i = 0; i < 160; i++)
    {
        gScanlineEffectRegBuffers[1][i] = 242;
    }

    SetVBlankCallback(VBlankCB_Wave);

    task->tState++;
    return TRUE;
}

static bool8 Wave_Main(struct Task* task)
{
    u8 i, sinIndex;
    u16* toStore;
    bool8 finished;

    sTransitionData->VBlank_DMA = 0;
    toStore = gScanlineEffectRegBuffers[0];
    sinIndex = task->tSinIndex;
    task->tSinIndex += 16;
    task->tX += 8;

    for (i = 0, finished = TRUE; i < 160; i++, sinIndex += 4, toStore++)
    {
        s16 x = task->tX + Sin(sinIndex, 40);
        if (x < 0)
            x = 0;
        if (x > 240)
            x = 240;
        *toStore = (x << 8) | 0xF1;
        if (x < 240)
            finished = FALSE;
    }
    if (finished)
        task->tState++;

    sTransitionData->VBlank_DMA++;
    return FALSE;
}

static bool8 Wave_End(struct Task* task)
{
    DmaStop(0);
    FadeScreenBlack();
    DestroyTask(FindTaskIdByFunc(Task_Wave));
    return FALSE;
}

static void VBlankCB_Wave(void)
{
    DmaStop(0);
    VBlankCB_BattleTransition();
    if (sTransitionData->VBlank_DMA != 0)
        DmaCopy16(3, gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], 320);
    REG_WININ = sTransitionData->WININ;
    REG_WINOUT = sTransitionData->WINOUT;
    REG_WIN0V = sTransitionData->WIN0V;
    DmaSet(0, gScanlineEffectRegBuffers[1], &REG_WIN0H, B_TRANS_DMA_FLAGS);
}

#undef tX
#undef tSinIndex

#define tSinIndex         data[1]
#define tTopBannerX       data[2]
#define tBottomBannerX    data[3]
#define tTimer            data[3]
#define tFadeSpread       data[4]
#define tOpponentSpriteId data[13]
#define tPlayerSpriteId   data[14]
#define tMugshotId        data[15]

#define sState      data[0]
#define sSlideSpeed data[1]
#define sSlideAccel data[2]
#define sDone       data[6]
#define sSlideDir   data[7]

static void Task_Sidney(u8 taskId)
{
    gTasks[taskId].tMugshotId = MUGSHOT_SYDNEY;
    DoMugshotTransition(taskId);
}

static void Task_Phoebe(u8 taskId)
{
    gTasks[taskId].tMugshotId = MUGSHOT_PHOEBE;
    DoMugshotTransition(taskId);
}

static void Task_Glacia(u8 taskId)
{
    gTasks[taskId].tMugshotId = MUGSHOT_GLACIA;
    DoMugshotTransition(taskId);
}

static void Task_Drake(u8 taskId)
{
    gTasks[taskId].tMugshotId = MUGSHOT_DRAKE;
    DoMugshotTransition(taskId);
}

static void Task_Steven(u8 taskId)
{
    // Ruby's fifth mugshot is Steven rather than Emerald's Wallace.
    gTasks[taskId].tMugshotId = MUGSHOT_STEVEN;
    DoMugshotTransition(taskId);
}

static void DoMugshotTransition(u8 taskId)
{
    while (sMugshot_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 Mugshot_Init(struct Task* task)
{
    u8 i;

    InitTransitionData();
    ScanlineEffect_Clear();
    Mugshots_CreateTrainerPics(task);

    task->tSinIndex = 0;
    task->tTopBannerX = 1;
    task->tBottomBannerX = 239;
    sTransitionData->WININ = 63;
    sTransitionData->WINOUT = 62;
    sTransitionData->WIN0V = 160;

    for (i = 0; i < 160; i++)
    {
        gScanlineEffectRegBuffers[1][i] = 0xF0F1;
    }

    SetVBlankCallback(VBlankCB_Mugshots);

    task->tState++;
    return FALSE;
}

static bool8 Mugshot_SetGfx(struct Task* task)
{
    s16 i, j;
    u16 *tilemap, *tileset;
    const u16 *mugshotsMap;

    mugshotsMap = sMugshotsTilemap;
    GetBg0TilesDst(&tilemap, &tileset);
    CpuSet(sEliteFour_Tileset, tileset, 0xF0);
    LoadPalette(sOpponentMugshotsPals[task->tMugshotId], 0xF0, 0x20);
    LoadPalette(sPlayerMugshotsPals[gSaveBlock2.playerGender], 0xFA, 0xC);

    for (i = 0; i < 20; i++)
    {
        for (j = 0; j < 32; j++, mugshotsMap++)
        {
            tilemap[i * 32 + j] = *mugshotsMap | 0xF000;
        }
    }

    REG_IE |= INTR_FLAG_HBLANK;
    REG_DISPSTAT |= DISPSTAT_HBLANK_INTR;
    SetHBlankCallback(HBlankCB_Mugshots);
    task->tState++;
    return FALSE;
}

static bool8 Mugshot_ShowBanner(struct Task* task)
{
    u8 i, sinIndex;
    u16* toStore;
    s16 x;
    s32 mergedX;

    sTransitionData->VBlank_DMA = 0;

    toStore = gScanlineEffectRegBuffers[0];
    sinIndex = task->tSinIndex;
    task->tSinIndex += 0x10;

    for (i = 0; i < 80; i++, toStore++, sinIndex += 0x10)
    {
        x = task->tTopBannerX + Sin(sinIndex, 0x10);
        if (x < 0)
            x = 1;
        if (x > 0xF0)
            x = 0xF0;
        *toStore = x;
    }
    for (; i < 160; i++, toStore++, sinIndex += 0x10)
    {
        x = task->tBottomBannerX - Sin(sinIndex, 0x10);
        if (x < 0)
            x = 0;
        if (x > 0xEF)
            x = 0xEF;
        *toStore = (x << 8) | 0xF0;
    }

    task->tTopBannerX += 8;
    task->tBottomBannerX -= 8;
    if (task->tTopBannerX > 0xF0)
        task->tTopBannerX = 0xF0;
    if (task->tBottomBannerX < 0)
        task->tBottomBannerX = 0;
    mergedX = *(s32*)(&task->tTopBannerX);
    if (mergedX == 0xF0)
        task->tState++;

    sTransitionData->BG0HOFS_Lower -= 8;
    sTransitionData->BG0HOFS_Upper += 8;
    sTransitionData->VBlank_DMA++;
    return FALSE;
}

static bool8 Mugshot_StartOpponentSlide(struct Task* task)
{
    u8 i;
    u16* toStore;

    sTransitionData->VBlank_DMA = 0;

    for (i = 0, toStore = gScanlineEffectRegBuffers[0]; i < 160; i++, toStore++)
    {
        *toStore = 0xF0;
    }

    task->tState++;
    task->tSinIndex = 0;
    task->tTopBannerX = 0;
    task->tBottomBannerX = 0;
    sTransitionData->BG0HOFS_Lower -= 8;
    sTransitionData->BG0HOFS_Upper += 8;

    SetTrainerPicSlideDirection(task->tOpponentSpriteId, 0);
    SetTrainerPicSlideDirection(task->tPlayerSpriteId, 1);
    IncrementTrainerPicState(task->tOpponentSpriteId);

    PlaySE(SE_MUGSHOT);

    sTransitionData->VBlank_DMA++;
    return FALSE;
}

static bool8 Mugshot_WaitStartPlayerSlide(struct Task* task)
{
    sTransitionData->BG0HOFS_Lower -= 8;
    sTransitionData->BG0HOFS_Upper += 8;
    if (IsTrainerPicSlideDone(task->tOpponentSpriteId))
    {
        task->tState++;
        IncrementTrainerPicState(task->tPlayerSpriteId);
    }
    return FALSE;
}

static bool8 Mugshot_WaitPlayerSlide(struct Task* task)
{
    sTransitionData->BG0HOFS_Lower -= 8;
    sTransitionData->BG0HOFS_Upper += 8;
    if (IsTrainerPicSlideDone(task->tPlayerSpriteId))
    {
        sTransitionData->VBlank_DMA = 0;
        SetVBlankCallback(NULL);
        DmaStop(0);
        memset(gScanlineEffectRegBuffers[0], 0, 0x140);
        memset(gScanlineEffectRegBuffers[1], 0, 0x140);
        REG_WIN0H = 0xF0;
        REG_BLDY = 0;
        task->tState++;
        task->tTimer = 0;
        task->tFadeSpread = 0;
        sTransitionData->BLDCNT = 0xBF;
        SetVBlankCallback(VBlankCB_MugshotsFadeOut);
    }
    return FALSE;
}

static bool8 Mugshot_GradualWhiteFade(struct Task* task)
{
    bool32 active;

    sTransitionData->VBlank_DMA = 0;
    active = TRUE;
    sTransitionData->BG0HOFS_Lower -= 8;
    sTransitionData->BG0HOFS_Upper += 8;

    if (task->tFadeSpread < 0x50)
        task->tFadeSpread += 2;
    if (task->tFadeSpread > 0x50)
        task->tFadeSpread = 0x50;

    if (++task->tTimer & 1)
    {
        s16 i;
        for (i = 0, active = FALSE; i <= task->tFadeSpread; i++)
        {
            s16 index1 = 0x50 - i;
            s16 index2 = 0x50 + i;
            if (gScanlineEffectRegBuffers[0][index1] <= 15)
            {
                active = TRUE;
                gScanlineEffectRegBuffers[0][index1]++;
            }
            if (gScanlineEffectRegBuffers[0][index2] <= 15)
            {
                active = TRUE;
                gScanlineEffectRegBuffers[0][index2]++;
            }
        }
    }

    if (task->tFadeSpread == 0x50 && !active)
        task->tState++;

    sTransitionData->VBlank_DMA++;
    return FALSE;
}

static bool8 Mugshot_InitFadeWhiteToBlack(struct Task* task)
{
    sTransitionData->VBlank_DMA = 0;
    BlendPalettes(0xFFFFFFFF, 16, RGB(31, 31, 31));
    sTransitionData->BLDCNT = 0xFF;
    task->tTimer = 0;

    task->tState++;
    return TRUE;
}

static bool8 Mugshot_FadeToBlack(struct Task* task)
{
    sTransitionData->VBlank_DMA = 0;

    task->tTimer++;
    memset(gScanlineEffectRegBuffers[0], task->tTimer, 0x140);
    if (task->tTimer > 15)
        task->tState++;

    sTransitionData->VBlank_DMA++;
    return FALSE;
}

static bool8 Mugshot_End(struct Task* task)
{
    DmaStop(0);
    FadeScreenBlack();
    DestroyTask(FindTaskIdByFunc(task->func));
    return FALSE;
}

static void VBlankCB_Mugshots(void)
{
    DmaStop(0);
    VBlankCB_BattleTransition();
    if (sTransitionData->VBlank_DMA != 0)
        DmaCopy16(3, gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], 320);
    REG_BG0VOFS = sTransitionData->BG0VOFS;
    REG_WININ = sTransitionData->WININ;
    REG_WINOUT = sTransitionData->WINOUT;
    REG_WIN0V = sTransitionData->WIN0V;
    DmaSet(0, gScanlineEffectRegBuffers[1], &REG_WIN0H, B_TRANS_DMA_FLAGS);
}

static void VBlankCB_MugshotsFadeOut(void)
{
    DmaStop(0);
    VBlankCB_BattleTransition();
    if (sTransitionData->VBlank_DMA != 0)
        DmaCopy16(3, gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], 320);
    REG_BLDCNT = sTransitionData->BLDCNT;
    DmaSet(0, gScanlineEffectRegBuffers[1], &REG_BLDY, B_TRANS_DMA_FLAGS);
}

static void HBlankCB_Mugshots(void)
{
    if (REG_VCOUNT < 80)
        REG_BG0HOFS = sTransitionData->BG0HOFS_Lower;
    else
        REG_BG0HOFS = sTransitionData->BG0HOFS_Upper;
}

static void Mugshots_CreateTrainerPics(struct Task* task)
{
    struct Sprite *opponentSprite, *playerSprite;

    s16 mugshotId = task->tMugshotId;
    task->tOpponentSpriteId = CreateTrainerSprite(sMugshotsTrainerPicIDsTable[mugshotId],
                                                     sMugshotsOpponentCoords[mugshotId][0] - 32,
                                                     sMugshotsOpponentCoords[mugshotId][1] + 42,
                                                     0,
        sMugshotDecompBuffer);
    task->tPlayerSpriteId = CreateTrainerSprite(gSaveBlock2.playerGender, 272, 106, 0, sMugshotDecompBuffer);

    opponentSprite = &gSprites[task->tOpponentSpriteId];
    playerSprite = &gSprites[task->tPlayerSpriteId];

    opponentSprite->callback = SpriteCB_MugshotTrainerPic;
    playerSprite->callback = SpriteCB_MugshotTrainerPic;

    opponentSprite->oam.affineMode = 3;
    playerSprite->oam.affineMode = 3;

    opponentSprite->oam.matrixNum = AllocOamMatrix();
    playerSprite->oam.matrixNum = AllocOamMatrix();

    opponentSprite->oam.shape = 1;
    playerSprite->oam.shape = 1;

    opponentSprite->oam.size = 3;
    playerSprite->oam.size = 3;

    CalcCenterToCornerVec(opponentSprite, 1, 3, 3);
    CalcCenterToCornerVec(playerSprite, 1, 3, 3);

    SetOamMatrixRotationScaling(opponentSprite->oam.matrixNum, sMugshotsOpponentRotationScales[mugshotId][0], sMugshotsOpponentRotationScales[mugshotId][1], 0);
    SetOamMatrixRotationScaling(playerSprite->oam.matrixNum, -512, 0x200, 0);
}

static void SpriteCB_MugshotTrainerPic(struct Sprite* sprite)
{
    while (sMugshotTrainerPicFuncs[sprite->sState](sprite));
}

static bool8 MugshotTrainerPic_Pause(struct Sprite* sprite)
{
    return FALSE;
}

static bool8 MugshotTrainerPic_Init(struct Sprite* sprite)
{
    s16 speeds[2];
    s16 accels[2];

    memcpy(speeds, sTrainerPicSlideSpeeds, sizeof(sTrainerPicSlideSpeeds));
    memcpy(accels, sTrainerPicSlideAccels, sizeof(sTrainerPicSlideAccels));

    sprite->sState++;
    sprite->sSlideSpeed = speeds[sprite->sSlideDir];
    sprite->sSlideAccel = accels[sprite->sSlideDir];
    return TRUE;
}

static bool8 MugshotTrainerPic_Slide(struct Sprite* sprite)
{
    sprite->x += sprite->sSlideSpeed;
    if (sprite->sSlideDir && sprite->x < 133)
        sprite->sState++;
    else if (!sprite->sSlideDir && sprite->x > 103)
        sprite->sState++;
    return FALSE;
}

static bool8 MugshotTrainerPic_SlideSlow(struct Sprite* sprite)
{
    sprite->sSlideSpeed += sprite->sSlideAccel;
    sprite->x += sprite->sSlideSpeed;
    if (sprite->sSlideSpeed == 0)
    {
        sprite->sState++;
        sprite->sSlideAccel = -sprite->sSlideAccel;
        sprite->sDone = TRUE;
    }
    return FALSE;
}

static bool8 MugshotTrainerPic_SlideOffscreen(struct Sprite* sprite)
{
    sprite->sSlideSpeed += sprite->sSlideAccel;
    sprite->x += sprite->sSlideSpeed;
    if (sprite->x < -31 || sprite->x > 271)
        sprite->sState++;
    return FALSE;
}

static void SetTrainerPicSlideDirection(s16 spriteId, s16 dirId)
{
    gSprites[spriteId].sSlideDir = dirId;
}

static void IncrementTrainerPicState(s16 spriteId)
{
    gSprites[spriteId].sState++;
}

static s16 IsTrainerPicSlideDone(s16 spriteId)
{
    return gSprites[spriteId].sDone;
}

#undef sState
#undef sSlideSpeed
#undef sSlideAccel
#undef sDone
#undef sSlideDir
#undef tSinIndex
#undef tTopBannerX
#undef tBottomBannerX
#undef tTimer
#undef tFadeSpread
#undef tOpponentSpriteId
#undef tPlayerSpriteId
#undef tMugshotId

#define tEffectX data[1]
#define tSpeed   data[2]
#define tAccel   data[3]

static void Task_Slice(u8 taskId)
{
    while (sSlice_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 Slice_Init(struct Task* task)
{
    u16 i;

    InitTransitionData();
    ScanlineEffect_Clear();

    task->tSpeed = 256;
    task->tAccel = 1;
    sTransitionData->WININ = 63;
    sTransitionData->WINOUT = 0;
    sTransitionData->WIN0V = 160;

    for (i = 0; i < 160; i++)
    {
        gScanlineEffectRegBuffers[1][i] = sTransitionData->cameraX;
        gScanlineEffectRegBuffers[1][160 + i] = 0xF0;
    }

    REG_IE |= INTR_FLAG_HBLANK;
    REG_DISPSTAT |= DISPSTAT_HBLANK_INTR;

    SetVBlankCallback(VBlankCB_Slice);
    SetHBlankCallback(HBlankCB_Slice);

    task->tState++;
    return TRUE;
}

static bool8 Slice_Main(struct Task* task)
{
    u16 i;

    sTransitionData->VBlank_DMA = 0;

    task->tEffectX += (task->tSpeed >> 8);
    if (task->tEffectX > 0xF0)
        task->tEffectX = 0xF0;
    if (task->tSpeed <= 0xFFF)
        task->tSpeed += task->tAccel;
    if (task->tAccel < 128)
        task->tAccel <<= 1; // multiplying by two

    for (i = 0; i < 160; i++)
    {
        u16* storeLoc1 = &gScanlineEffectRegBuffers[0][i];
        u16* storeLoc2 = &gScanlineEffectRegBuffers[0][i + 160];
        if (1 & i)
        {
            *storeLoc1 = sTransitionData->cameraX + task->tEffectX;
            *storeLoc2 = 0xF0 - task->tEffectX;
        }
        else
        {
            *storeLoc1 = sTransitionData->cameraX - task->tEffectX;
            *storeLoc2 = (task->tEffectX << 8) | (0xF1);
        }
    }

    if (task->tEffectX > 0xEF)
        task->tState++;

    sTransitionData->VBlank_DMA++;
    return FALSE;
}

static bool8 Slice_End(struct Task* task)
{
    DmaStop(0);
    FadeScreenBlack();
    DestroyTask(FindTaskIdByFunc(Task_Slice));
    return FALSE;
}

static void VBlankCB_Slice(void)
{
    DmaStop(0);
    VBlankCB_BattleTransition();
    REG_WININ = sTransitionData->WININ;
    REG_WINOUT = sTransitionData->WINOUT;
    REG_WIN0V = sTransitionData->WIN0V;
    if (sTransitionData->VBlank_DMA)
        DmaCopy16(3, gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], 640);
    DmaSet(0, &gScanlineEffectRegBuffers[1][160], &REG_WIN0H, B_TRANS_DMA_FLAGS);
}

static void HBlankCB_Slice(void)
{
    u16 var = gScanlineEffectRegBuffers[1][REG_VCOUNT];
    REG_BG1HOFS = var;
    REG_BG2HOFS = var;
    REG_BG3HOFS = var;
}

#undef tEffectX
#undef tSpeed
#undef tAccel

#define sFade            data[0]
#define sFinished        data[1]
#define sDestroyAttempts data[2]
#define sDelay           data[5]
#define sIsMainSprite    data[6]

#define FADE_TARGET (16 << 8)

static void Task_WhiteBarsFade(u8 taskId)
{
    while (sWhiteBarsFade_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 WhiteBarsFade_Init(struct Task* task)
{
    u16 i;

    InitTransitionData();
    ScanlineEffect_Clear();

    sTransitionData->BLDCNT = 0xBF;
    sTransitionData->BLDY = 0;
    sTransitionData->WININ = 0x1E;
    sTransitionData->WINOUT = 0x3F;
    sTransitionData->WIN0V = 0xA0;

    for (i = 0; i < 160; i++)
    {
        gScanlineEffectRegBuffers[1][i] = 0;
        gScanlineEffectRegBuffers[1][i + 160] = 0xF0;
    }

    REG_IE |= INTR_FLAG_HBLANK;
    REG_DISPSTAT |= DISPSTAT_HBLANK_INTR;

    SetHBlankCallback(HBlankCB_WhiteBarsFade);
    SetVBlankCallback(VBlankCB_WhiteBarsFade);

    task->tState++;
    return FALSE;
}

static bool8 WhiteBarsFade_StartBars(struct Task* task)
{
    s16 i, posY;
    s16 delays[NUM_WHITE_BARS];
    struct Sprite* sprite;

    memcpy(delays, sWhiteBarsFade_StartDelays, sizeof(sWhiteBarsFade_StartDelays));
    for (i = 0, posY = 0; i < NUM_WHITE_BARS; i++, posY += 0x14)
    {
        sprite = &gSprites[CreateInvisibleSprite(SpriteCB_WhiteBarFade)];
        sprite->x = 0xF0;
        sprite->y = posY;
        sprite->sDelay = delays[i];
    }
    sprite->sIsMainSprite++;

    task->tState++;
    return FALSE;
}

static bool8 WhiteBarsFade_WaitBars(struct Task* task)
{
    sTransitionData->VBlank_DMA = 0;
    if (sTransitionData->counter >= NUM_WHITE_BARS)
    {
        BlendPalettes(0xFFFFFFFF, 16, RGB(31, 31, 31));
        task->tState++;
    }
    return FALSE;
}

static bool8 WhiteBarsFade_BlendToBlack(struct Task* task)
{
    sTransitionData->VBlank_DMA = 0;

    DmaStop(0);
    SetVBlankCallback(0);
    SetHBlankCallback(0);

    sTransitionData->WIN0H = 0xF0;
    sTransitionData->BLDY = 0;
    sTransitionData->BLDCNT = 0xFF;
    sTransitionData->WININ = 0x3F;

    SetVBlankCallback(VBlankCB_WhiteBarsFade_Blend);

    task->tState++;
    return FALSE;
}

static bool8 WhiteBarsFade_End(struct Task* task)
{
   if (++sTransitionData->BLDY > 16)
   {
       FadeScreenBlack();
       DestroyTask(FindTaskIdByFunc(Task_WhiteBarsFade));
   }
   return FALSE;
}

static void VBlankCB_WhiteBarsFade(void)
{
    DmaStop(0);
    VBlankCB_BattleTransition();
    REG_BLDCNT = sTransitionData->BLDCNT;
    REG_WININ = sTransitionData->WININ;
    REG_WINOUT = sTransitionData->WINOUT;
    REG_WIN0V = sTransitionData->WIN0H;
    if (sTransitionData->VBlank_DMA)
        DmaCopy16(3, gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], 640);
    DmaSet(0, &gScanlineEffectRegBuffers[1][160], &REG_WIN0H, B_TRANS_DMA_FLAGS);
}

static void VBlankCB_WhiteBarsFade_Blend(void)
{
    VBlankCB_BattleTransition();
    REG_BLDY = sTransitionData->BLDY;
    REG_BLDCNT = sTransitionData->BLDCNT;
    REG_WININ = sTransitionData->WININ;
    REG_WINOUT = sTransitionData->WINOUT;
    REG_WIN0H = sTransitionData->WIN0H;
    REG_WIN0V = sTransitionData->WIN0V;
}

static void HBlankCB_WhiteBarsFade(void)
{
    REG_BLDY = gScanlineEffectRegBuffers[1][REG_VCOUNT];
}

static void SpriteCB_WhiteBarFade(struct Sprite* sprite)
{
    if (sprite->sDelay)
    {
        sprite->sDelay--;
        if (sprite->sIsMainSprite)
            sTransitionData->VBlank_DMA = 1;
    }
    else
    {
        u16 i;
        u16* ptr1 = &gScanlineEffectRegBuffers[0][sprite->y];
        u16* ptr2 = &gScanlineEffectRegBuffers[0][sprite->y + 160];
        for (i = 0; i < 20; i++)
        {
            ptr1[i] = sprite->sFade >> 8;
            ptr2[i] = (u8)(sprite->x);
        }
        if (sprite->x == 0 && sprite->sFade == FADE_TARGET)
            sprite->sFinished = TRUE;

        sprite->x -= 16;
        sprite->sFade += FADE_TARGET / 32;

        if (sprite->x < 0)
            sprite->x = 0;
        if (sprite->sFade > FADE_TARGET)
            sprite->sFade = FADE_TARGET;

        if (sprite->sIsMainSprite)
            sTransitionData->VBlank_DMA = 1;

        if (sprite->sFinished)
        {
            if (sprite->sIsMainSprite == 0 || (sTransitionData->counter >= NUM_WHITE_BARS - 1 && sprite->sDestroyAttempts++ > 7))
            {
                sTransitionData->counter++;
                DestroySprite(sprite);
            }
        }
    }
}

#undef sFade
#undef sFinished
#undef sDestroyAttempts
#undef sDelay
#undef sIsMainSprite

#define tDelay       data[1]
#define tShrinkStage data[2]

static void Task_GridSquares(u8 taskId)
{
    while (sGridSquares_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 GridSquares_Init(struct Task* task)
{
    u16 *tilemap, *tileset;

    GetBg0TilesDst(&tilemap, &tileset);
    CpuSet(sShrinkingBoxTileset, tileset, 0x10);
    CpuFill16(0xF000, tilemap, 0x800);
    LoadPalette(sFieldEffectPal_Pokeball, 0xF0, 0x20);

    task->tState++;
    return FALSE;
}

static bool8 GridSquares_Main(struct Task* task)
{
    u16* tileset;

    if (task->tDelay == 0)
    {
        GetBg0TilemapDst(&tileset);
        task->tDelay = 3;
        task->tShrinkStage++;
        CpuSet(sShrinkingBoxTileset + (task->tShrinkStage * 8), tileset, 0x10);
        if (task->tShrinkStage > 0xD)
        {
            task->tState++;
            task->tDelay = 16;
        }
    }

    task->tDelay--;
    return FALSE;
}

static bool8 GridSquares_End(struct Task* task)
{
    if (--task->tDelay == 0)
    {
        FadeScreenBlack();
        DestroyTask(FindTaskIdByFunc(Task_GridSquares));
    }
    return FALSE;
}

#undef tDelay
#undef tShrinkStage

#define tWipeId data[1]
#define tDir    data[2]
#define tDelay  data[3]

static void Task_AngledWipes(u8 taskId)
{
    while (sAngledWipes_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 AngledWipes_Init(struct Task* task)
{
    u16 i;

    InitTransitionData();
    ScanlineEffect_Clear();

    sTransitionData->WININ = 0x3F;
    sTransitionData->WINOUT = 0;
    sTransitionData->WIN0V = 0xA0;

    for (i = 0; i < 160; i++)
    {
        gScanlineEffectRegBuffers[0][i] = 0xF0;
    }

    CpuSet(gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], 0xA0);
    SetVBlankCallback(VBlankCB_AngledWipes);

    task->tState++;
    return TRUE;
}

static bool8 AngledWipes_SetWipeData(struct Task* task)
{
    InitBlackWipe(sTransitionData->data,
                sAngledWipes_MoveData[task->tWipeId][0],
                sAngledWipes_MoveData[task->tWipeId][1],
                sAngledWipes_MoveData[task->tWipeId][2],
                sAngledWipes_MoveData[task->tWipeId][3],
                1, 1);
    task->tDir = sAngledWipes_MoveData[task->tWipeId][4];
    task->tState++;
    return TRUE;
}

static bool8 AngledWipes_DoWipe(struct Task* task)
{
    s16 i;
    bool8 finished;

    sTransitionData->VBlank_DMA = 0;

    for (i = 0, finished = FALSE; i < 16; i++)
    {
        s16 start = gScanlineEffectRegBuffers[0][sTransitionData->tWipeCurrY] >> 8;
        s16 end = gScanlineEffectRegBuffers[0][sTransitionData->tWipeCurrY] & 0xFF;
        if (task->tDir == 0)
        {
            if (start < sTransitionData->tWipeCurrX)
                start = sTransitionData->tWipeCurrX;
            if (start > end)
                start = end;
        }
        else
        {
            if (end > sTransitionData->tWipeCurrX)
                end = sTransitionData->tWipeCurrX;
            if (end <= start)
                end = start;
        }
        gScanlineEffectRegBuffers[0][sTransitionData->tWipeCurrY] = end | (start << 8);
        if (finished)
        {
            task->tState++;
            break;
        }
        else
            finished = UpdateBlackWipe(sTransitionData->data, TRUE, TRUE);
    }

    sTransitionData->VBlank_DMA++;
    return FALSE;
}

static bool8 AngledWipes_TryEnd(struct Task* task)
{
    if (++task->tWipeId < NUM_ANGLED_WIPES)
    {
        task->tState++;
        task->tDelay = sAngledWipes_EndDelays[task->tWipeId - 1];
        return TRUE;
    }
    else
    {
        DmaStop(0);
        FadeScreenBlack();
        DestroyTask(FindTaskIdByFunc(Task_AngledWipes));
        return FALSE;
    }
}

static bool8 AngledWipes_StartNext(struct Task* task)
{
    if (--task->tDelay == 0)
    {
        task->tState = 1;
        return TRUE;
    }
    else
        return FALSE;
}

static void VBlankCB_AngledWipes(void)
{
    DmaStop(0);
    VBlankCB_BattleTransition();
    if (sTransitionData->VBlank_DMA)
        DmaCopy16(3, gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], 320);
    REG_WININ = sTransitionData->WININ;
    REG_WINOUT = sTransitionData->WINOUT;
    REG_WIN0V = sTransitionData->WIN0V;
    REG_WIN0H = gScanlineEffectRegBuffers[1][0];
    DmaSet(0, gScanlineEffectRegBuffers[1], &REG_WIN0H, B_TRANS_DMA_FLAGS);
}

#undef tWipeId
#undef tDir
#undef tDelay

#define tFadeToGrayDelay       data[1]
#define tFadeFromGrayDelay     data[2]
#define tNumFades              data[3]
#define tFadeToGrayIncrement   data[4]
#define tFadeFromGrayIncrement data[5]
#define tDelayTimer            data[6]
#define tBlend                 data[7]

static void CreateIntroTask(s16 fadeToGrayDelay, s16 fadeFromGrayDelay, s16 numFades, s16 fadeToGrayIncrement, s16 fadeFromGrayIncrement)
{
    u8 taskId = CreateTask(Task_BattleTransition_Intro, 3);
    gTasks[taskId].tFadeToGrayDelay = fadeToGrayDelay;
    gTasks[taskId].tFadeFromGrayDelay = fadeFromGrayDelay;
    gTasks[taskId].tNumFades = numFades;
    gTasks[taskId].tFadeToGrayIncrement = fadeToGrayIncrement;
    gTasks[taskId].tFadeFromGrayIncrement = fadeFromGrayIncrement;
    gTasks[taskId].tDelayTimer = fadeToGrayDelay;
}

static bool8 IsIntroTaskDone(void)
{
    if (FindTaskIdByFunc(Task_BattleTransition_Intro) == 0xFF)
        return TRUE;
    else
        return FALSE;
}

static void Task_BattleTransition_Intro(u8 taskId)
{
    while (sTransitionIntroFuncs[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 TransitionIntro_FadeToGray(struct Task* task)
{
    if (task->tDelayTimer == 0 || --task->tDelayTimer == 0)
    {
        task->tDelayTimer = task->tFadeToGrayDelay;
        task->tBlend += task->tFadeToGrayIncrement;
        if (task->tBlend > 16)
            task->tBlend = 16;
        BlendPalettes(0xFFFFFFFF, task->tBlend, RGB(11, 11, 11));
    }
    if (task->tBlend > 15)
    {
        task->tState++;
        task->tDelayTimer = task->tFadeFromGrayDelay;
    }
    return FALSE;
}

static bool8 TransitionIntro_FadeFromGray(struct Task* task)
{
    if (task->tDelayTimer == 0 || --task->tDelayTimer == 0)
    {
        task->tDelayTimer = task->tFadeFromGrayDelay;
        task->tBlend -= task->tFadeFromGrayIncrement;
        if (task->tBlend < 0)
            task->tBlend = 0;
        BlendPalettes(0xFFFFFFFF, task->tBlend, RGB(11, 11, 11));
    }
    if (task->tBlend == 0)
    {
        if (--task->tNumFades == 0)
            DestroyTask(FindTaskIdByFunc(Task_BattleTransition_Intro));
        else
        {
            task->tDelayTimer = task->tFadeToGrayDelay;
            task->tState = 0;
        }
    }
    return FALSE;
}

#undef tFadeToGrayDelay
#undef tFadeFromGrayDelay
#undef tNumFades
#undef tFadeToGrayIncrement
#undef tFadeFromGrayIncrement
#undef tDelayTimer
#undef tBlend

static void InitTransitionData(void)
{
    struct TransitionData* const* dummy = &sTransitionData;
    memset(*dummy, 0, sizeof(struct TransitionData));
    GetCameraOffsetWithPan(&sTransitionData->cameraX, &sTransitionData->cameraY);
}

static void VBlankCB_BattleTransition(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void GetBg0TilemapDst(u16** tileset)
{
    u16 charBase;

    charBase = REG_BG0CNT >> 2;
    charBase <<= 0xE;

    *tileset = (u16*)(VRAM + charBase);
}

static void GetBg0TilesDst(u16** tilemap, u16** tileset)
{
    u16 screenBase, charBase;

    screenBase = REG_BG0CNT >> 8;
    charBase = REG_BG0CNT >> 2;

    screenBase <<= 0xB;
    charBase <<= 0xE;

    *tilemap = (u16*)(VRAM + screenBase);
    *tileset = (u16*)(VRAM + charBase);
}

static void FadeScreenBlack(void)
{
    BlendPalettes(0xFFFFFFFF, 16, RGB(0, 0, 0));
}

static void SetSinWave(s16* array, s16 sinAdd, s16 index, s16 indexIncrementer, s16 amplitude, s16 arrSize)
{
    u8 i;
    for (i = 0; arrSize > 0; arrSize--, i++, index += indexIncrementer)
    {
        array[i] = sinAdd + Sin(0xFF & index, amplitude);
    }
}

static void SetCircularMask(u16* buffer, s16 centerX, s16 centerY, s16 radius)
{
    s16 i;

    memset(buffer, 0xA, 160 * sizeof(s16));
    for (i = 0; i < 64; i++)
    {
        s16 sinResult, cosResult;
        s16 drawXLeft, drawYBottomNext, drawYTopNext, drawX, drawYTop, drawYBottom;

        sinResult = Sin(i, radius);
        cosResult = Cos(i, radius);

        drawXLeft = centerX - sinResult;
        drawX = centerX + sinResult;
        drawYTop = centerY - cosResult;
        drawYBottom = centerY + cosResult;

        if (drawXLeft < 0)
            drawXLeft = 0;
        if (drawX > 0xF0)
            drawX = 0xF0;
        if (drawYTop < 0)
            drawYTop = 0;
        if (drawYBottom > 0x9F)
            drawYBottom = 0x9F;

        drawX |= (drawXLeft << 8);
        buffer[drawYTop] = drawX;
        buffer[drawYBottom] = drawX;

        cosResult = Cos(i + 1, radius);
        drawYTopNext = centerY - cosResult;
        drawYBottomNext = centerY + cosResult;

        if (drawYTopNext < 0)
            drawYTopNext = 0;
        if (drawYBottomNext > 0x9F)
            drawYBottomNext = 0x9F;

        while (drawYTop > drawYTopNext)
            buffer[--drawYTop] = drawX;
        while (drawYTop < drawYTopNext)
            buffer[++drawYTop] = drawX;

        while (drawYBottom > drawYBottomNext)
            buffer[--drawYBottom] = drawX;
        while (drawYBottom < drawYBottomNext)
            buffer[++drawYBottom] = drawX;
    }
}

static void InitBlackWipe(s16* data, s16 startX, s16 startY, s16 endX, s16 endY, s16 xMove, s16 yMove)
{
    tWipeStartX = startX;
    tWipeStartY = startY;
    tWipeCurrX = startX;
    tWipeCurrY = startY;
    tWipeEndX = endX;
    tWipeEndY = endY;
    tWipeXMove = xMove;
    tWipeYMove = yMove;
    tWipeXDist = endX - startX;
    if (tWipeXDist < 0)
    {
        tWipeXDist = -tWipeXDist;
        tWipeXMove = -xMove;
    }
    tWipeYDist = endY - startY;
    if (tWipeYDist < 0)
    {
        tWipeYDist = -tWipeYDist;
        tWipeYMove = -yMove;
    }
    tWipeTemp = 0;
}

static bool8 UpdateBlackWipe(s16* data, bool8 xExact, bool8 yExact)
{
    u8 numFinished;
    if (tWipeXDist > tWipeYDist)
    {
        tWipeCurrX += tWipeXMove;
        tWipeTemp += tWipeYDist;
        if (tWipeTemp > tWipeXDist)
        {
            tWipeCurrY += tWipeYMove;
            tWipeTemp -= tWipeXDist;
        }
    }
    else
    {
        tWipeCurrY += tWipeYMove;
        tWipeTemp += tWipeXDist;
        if (tWipeTemp > tWipeYDist)
        {
            tWipeCurrX += tWipeXMove;
            tWipeTemp -= tWipeYDist;
        }
    }
    numFinished = 0;
    if ((tWipeXMove > 0 && tWipeCurrX >= tWipeEndX) || (tWipeXMove < 0 && tWipeCurrX <= tWipeEndX))
    {
        numFinished++;
        if (xExact)
            tWipeCurrX = tWipeEndX;
    }
    if ((tWipeYMove > 0 && tWipeCurrY >= tWipeEndY) || (tWipeYMove < 0 && tWipeCurrY <= tWipeEndY))
    {
        numFinished++;
        if (yExact)
            tWipeCurrY = tWipeEndY;
    }
    if (numFinished == 2)
        return TRUE;
    else
        return FALSE;
}
