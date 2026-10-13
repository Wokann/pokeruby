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

// this file's functions
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
static void Phase2Task_Transition_Steven(u8 taskId);
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
static void HBlankCB_Phase2_Transition_Slice(void);
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
static bool8 Phase2_Transition_BigPokeball_Func6(struct Task* task);
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
static void Mugshots_CreateOpponentPlayerSprites(struct Task* task);
static void SetTrainerPicSlideDirection(s16 spriteId, s16 dirId);
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

static struct TransitionData * const sTransitionData = &TRANSITION_STRUCT;

static const TaskFunc sTasks_Intro[TRANSITIONS_NO] =
{
    [0 ... TRANSITIONS_NO - 1] = &Task_Intro
};

static const TaskFunc sTasks_Main[TRANSITIONS_NO] =
{
    Task_Blur,                        // 0
    Task_Swirl,                       // 1
    Task_Shuffle,                     // 2
    Task_BigPokeball,                 // 3
    Task_PokeballsTrail,              // 4
    Task_ClockwiseWipe,         // 5
    Task_Ripple,                      // 6
    Task_Wave,                        // 7
    Task_Slice,                       // 8
    Task_WhiteBarsFade,                   // 9
    Task_GridSquares,                 // 10
    Task_AngledWipes,                      // 11
    Task_Sidney,                      // 12
    Task_Phoebe,                      // 13
    Task_Glacia,                      // 14
    Task_Drake,                       // 15
    Phase2Task_Transition_Steven,                      // 16
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
    Phase2_Transition_BigPokeball_Func6
};

static const TransitionStateFunc sPokeballsTrail_Funcs[] =
{
    PokeballsTrail_Init,
    PokeballsTrail_Main,
    PokeballsTrail_End
};

static const s16 sPokeballsTrail_StartXCoords[2] = {-16, 256};
static const s16 sPokeballsTrail_Delays[5] = {0, 32, 64, 18, 48};
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

static const u8 sMugshotsTrainerPicIDsTable[MUGSHOTS_COUNT] = {TRAINER_PIC_ELITE_FOUR_SIDNEY, TRAINER_PIC_ELITE_FOUR_PHOEBE, TRAINER_PIC_ELITE_FOUR_GLACIA, TRAINER_PIC_ELITE_FOUR_DRAKE, TRAINER_PIC_STEVEN};
static const s16 sMugshotsOpponentRotationScales[MUGSHOTS_COUNT][2] =
{
    {0x200, 0x200},
    {0x200, 0x200},
    {0x1B0, 0x1B0},
    {0x1A0, 0x1A0},
    {0x188, 0x188},
};
static const s16 sMugshotsOpponentCoords[MUGSHOTS_COUNT][2] =
{
    {0,     0},
    {0,     0},
    {-4,    4},
    {0,     5},
    {0,     7},
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

static const s16 sWhiteBarsFade_StartDelays[8] = {0, 20, 15, 40, 10, 25, 35, 5};

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

static const s16 sAngledWipes_MoveData[][5] =
{
    {56,    0,      0,      160,    0},
    {104,   160,    240,    88,     1},
    {240,   72,     56,     0,      1},
    {0,     32,     144,    160,    0},
    {144,   160,    184,    0,      1},
    {56,    0,      168,    160,    0},
    {168,   160,    48,     0,      1},
};

static const s16 sAngledWipes_EndDelays[] = {8, 4, 2, 1, 1, 1, 0};

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
    .tileTag = 0xFFFF,
    .paletteTag = 4105,
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
    .tileTag = 0xFFFF,
    .paletteTag = 4106,
    .oam = &sOam_UnusedBrendanLass,
    .anims = sSpriteAnimTable_UnusedBrendanLass,
    .images = sImageTable_UnusedBrendan,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_MugshotTrainerPic
};

static const struct SpriteTemplate sSpriteTemplate_UnusedLass =
{
    .tileTag = 0xFFFF,
    .paletteTag = 4106,
    .oam = &sOam_UnusedBrendanLass,
    .anims = sSpriteAnimTable_UnusedBrendanLass,
    .images = sImageTable_UnusedLass,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_MugshotTrainerPic
};

static const u16 sFieldEffectPal_Pokeball[] = INCBIN_U16("graphics/field_effects/palettes/pokeball.gbapal");

const struct SpritePalette gSpritePalette_Pokeball =
{
    sFieldEffectPal_Pokeball, 0x1009
};

static const u16 sMugshotPal_Sidney[] = INCBIN_U16("graphics/battle_transitions/sidney_bg.gbapal");
static const u16 sMugshotPal_Phoebe[] = INCBIN_U16("graphics/battle_transitions/phoebe_bg.gbapal");
static const u16 sMugshotPal_Glacia[] = INCBIN_U16("graphics/battle_transitions/glacia_bg.gbapal");
static const u16 sMugshotPal_Drake[] = INCBIN_U16("graphics/battle_transitions/drake_bg.gbapal");
static const u16 sMugshotPal_Steven[] = INCBIN_U16("graphics/battle_transitions/steven_bg.gbapal");
static const u16 sMugshotPal_Brendan[] = INCBIN_U16("graphics/battle_transitions/brendan_bg.gbapal");
static const u16 sMugshotPal_May[] = INCBIN_U16("graphics/battle_transitions/may_bg.gbapal");

static const u16 * const sOpponentMugshotsPals[MUGSHOTS_COUNT] =
{
    sMugshotPal_Sidney,
    sMugshotPal_Phoebe,
    sMugshotPal_Glacia,
    sMugshotPal_Drake,
    sMugshotPal_Steven
};

static const u16 * const sPlayerMugshotsPals[2] =
{
    sMugshotPal_Brendan,
    sMugshotPal_May
};

static const u16 sUnusedTrainerPalette[] = INCBIN_U16("graphics/battle_transitions/unused_trainer.gbapal");
static const struct SpritePalette sSpritePalette_UnusedTrainer =
{
    sUnusedTrainerPalette, 0x100A
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
    if (task->data[1] != 0)
        task->data[1]--;
    else
    {
        task->data[1] = 4;
        if (++task->data[2] == 10)
            BeginNormalPaletteFade(0xFFFFFFFF, -1, 0, 16, RGB(0, 0, 0));
        REG_MOSAIC = (task->data[2] & 15) * 17;
        if (task->data[2] > 14)
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
    SetSinWave(gScanlineEffectRegBuffers[1], TRANSITION_STRUCT.cameraX, 0, 2, 0, 160);

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
    TRANSITION_STRUCT.VBlank_DMA = 0;
    task->data[1] += 4;
    task->data[2] += 8;

    SetSinWave(gScanlineEffectRegBuffers[0], TRANSITION_STRUCT.cameraX, task->data[1], 2, task->data[2], 160);

    if (!gPaletteFade.active)
    {
        u8 taskId = FindTaskIdByFunc(Task_Swirl);
        DestroyTask(taskId);
    }

    TRANSITION_STRUCT.VBlank_DMA++;
    return FALSE;
}

static void VBlankCB_Swirl(void)
{
    VBlankCB_BattleTransition();
    if (TRANSITION_STRUCT.VBlank_DMA)
        DmaCopy16(3, gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], 320);
}

static void HBlankCB_Swirl(void)
{
    u16 var = gScanlineEffectRegBuffers[1][REG_VCOUNT];
    REG_BG1HOFS = var;
    REG_BG2HOFS = var;
    REG_BG3HOFS = var;
}

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
    memset(gScanlineEffectRegBuffers[1], TRANSITION_STRUCT.cameraY, 0x140);

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

    TRANSITION_STRUCT.VBlank_DMA = 0;
    sinVal = task->data[1];
    amplitude = task->data[2] >> 8;
    task->data[1] += 4224;
    task->data[2] += 384;

    for (i = 0; i < 160; i++, sinVal += 4224)
    {
        u16 sinIndex = sinVal / 256;
        gScanlineEffectRegBuffers[0][i] = TRANSITION_STRUCT.cameraY + Sin(sinIndex, amplitude);
    }

    if (!gPaletteFade.active)
        DestroyTask(FindTaskIdByFunc(Task_Shuffle));

    TRANSITION_STRUCT.VBlank_DMA++;
    return FALSE;
}

static void VBlankCB_Shuffle(void)
{
    VBlankCB_BattleTransition();
    if (TRANSITION_STRUCT.VBlank_DMA)
        DmaCopy16(3, gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], 320);
}

static void HBlankCB_Shuffle(void)
{
    u16 var = gScanlineEffectRegBuffers[1][REG_VCOUNT];
    REG_BG1VOFS = var;
    REG_BG2VOFS = var;
    REG_BG3VOFS = var;
}

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

    task->data[1] = 16;
    task->data[2] = 0;
    task->data[4] = 0;
    task->data[5] = 0x4000;
    TRANSITION_STRUCT.WININ = 63;
    TRANSITION_STRUCT.WINOUT = 0;
    TRANSITION_STRUCT.WIN0H = 240;
    TRANSITION_STRUCT.WIN0V = 160;
    TRANSITION_STRUCT.BLDCNT = 0x3F41;
    TRANSITION_STRUCT.BLDALPHA = task->data[1] * 256; // 16 * 256 = 0x1000

    for (i = 0; i < 160; i++)
    {
        gScanlineEffectRegBuffers[1][i] = 240;
    }

    SetVBlankCallback(VBlankCB_PatternWeave);

    GetBg0TilesDst(&tilemap, & tileset);
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
    const u16* bigPokeballMap;

    bigPokeballMap = sBigPokeball_Tilemap;
    GetBg0TilesDst(&tilemap, &tileset);
    for (i = 0; i < 20; i++)
    {
        for (j = 0; j < 30; j++, bigPokeballMap++)
        {
            tilemap[i * 32 + j] = *bigPokeballMap | 0xF000;
        }
    }
    SetSinWave(gScanlineEffectRegBuffers[0], 0, task->data[4], 132, task->data[5], 160);

    task->tState++;
    return TRUE;
}

static bool8 PatternWeave_Blend1(struct Task* task)
{
    TRANSITION_STRUCT.VBlank_DMA = 0;
    if (task->data[3] == 0 || --task->data[3] == 0)
    {
        task->data[2]++;
        task->data[3] = 2;
    }
    TRANSITION_STRUCT.BLDALPHA = (task->data[1] << 8) | task->data[2];
    if (task->data[2] > 15)
        task->tState++;
    task->data[4] += 8;
    task->data[5] -= 256;

    SetSinWave(gScanlineEffectRegBuffers[0], 0, task->data[4], 132, task->data[5] >> 8, 160);

    TRANSITION_STRUCT.VBlank_DMA++;
    return FALSE;
}

static bool8 PatternWeave_Blend2(struct Task* task)
{
    TRANSITION_STRUCT.VBlank_DMA = 0;
    if (task->data[3] == 0 || --task->data[3] == 0)
    {
        task->data[1]--;
        task->data[3] = 2;
    }
    TRANSITION_STRUCT.BLDALPHA = (task->data[1] << 8) | task->data[2];
    if (task->data[1] == 0)
        task->tState++;
    task->data[4] += 8;
    task->data[5] -= 256;

    SetSinWave(gScanlineEffectRegBuffers[0], 0, task->data[4], 132, task->data[5] >> 8, 160);

    TRANSITION_STRUCT.VBlank_DMA++;
    return FALSE;
}

static bool8 PatternWeave_FinishAppear(struct Task* task)
{
    TRANSITION_STRUCT.VBlank_DMA = 0;
    task->data[4] += 8;
    task->data[5] -= 256;

    SetSinWave(gScanlineEffectRegBuffers[0], 0, task->data[4], 132, task->data[5] >> 8, 160);

    if (task->data[5] <= 0)
    {
        task->tState++;
        task->data[1] = 160;
        task->data[2] = 256;
        task->data[3] = 0;
    }

    TRANSITION_STRUCT.VBlank_DMA++;
    return FALSE;
}

static bool8 Phase2_Transition_BigPokeball_Func6(struct Task* task)
{
    TRANSITION_STRUCT.VBlank_DMA = 0;
    if (task->data[2] < 1024)
        task->data[2] += 128;
    if (task->data[1] != 0)
    {
        task->data[1] -= (task->data[2] >> 8);
        if (task->data[1] < 0)
            task->data[1] = 0;
    }
    SetCircularMask(gScanlineEffectRegBuffers[0], 120, 80, task->data[1]);
    if (task->data[1] == 0)
    {
        DmaStop(0);
        FadeScreenBlack();
        DestroyTask(FindTaskIdByFunc(Task_BigPokeball));
    }
    if (task->data[3] == 0)
    {
        task->data[3]++;
        SetVBlankCallback(VBlankCB_CircularMask);
    }

    TRANSITION_STRUCT.VBlank_DMA++;
    return FALSE;
}

static void VBlankCB_SetWinAndBlend(void)
{
    DmaStop(0);
    VBlankCB_BattleTransition();
    if (TRANSITION_STRUCT.VBlank_DMA)
        DmaCopy16(3, gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], 320);
    REG_WININ = TRANSITION_STRUCT.WININ;
    REG_WINOUT = TRANSITION_STRUCT.WINOUT;
    REG_WIN0V = TRANSITION_STRUCT.WIN0V;
    REG_BLDCNT = TRANSITION_STRUCT.BLDCNT;
    REG_BLDALPHA = TRANSITION_STRUCT.BLDALPHA;
}

static void VBlankCB_PatternWeave(void)
{
    VBlankCB_SetWinAndBlend();
    DmaSet(0, gScanlineEffectRegBuffers[1], &REG_BG0HOFS, 0xA2400001);
}

static void VBlankCB_CircularMask(void)
{
    VBlankCB_SetWinAndBlend();
    DmaSet(0, gScanlineEffectRegBuffers[1], &REG_WIN0H, 0xA2400001);
}

static void Task_PokeballsTrail(u8 taskId)
{
    while (sPokeballsTrail_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 PokeballsTrail_Init(struct Task* task)
{
    u16 *tilemap, *tileset;

    GetBg0TilesDst(&tilemap, &tileset);
    CpuSet(sPokeballTrail_Tileset, tileset, 0x20);
    CpuFill32(0, tilemap, 0x800);
    LoadPalette(sFieldEffectPal_Pokeball, 0xF0, 0x20);

    task->tState++;
    return FALSE;
}

static bool8 PokeballsTrail_Main(struct Task* task)
{
    s16 i;
    s16 side;
    s16 startX[2];
    s16 delays[5];

    memcpy(startX, sPokeballsTrail_StartXCoords, sizeof(sPokeballsTrail_StartXCoords));
    memcpy(delays, sPokeballsTrail_Delays, sizeof(sPokeballsTrail_Delays));
    side = Random() & 1;
    for (i = 0; i <= 4; i++, side ^= 1)
    {
        gFieldEffectArguments[0] = startX[side];      // x
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
    gSprites[spriteId].data[0] = gFieldEffectArguments[2];
    gSprites[spriteId].data[1] = gFieldEffectArguments[3];
    gSprites[spriteId].data[2] = -1;
    InitSpriteAffineAnim(&gSprites[spriteId]);
    StartSpriteAffineAnim(&gSprites[spriteId], gFieldEffectArguments[2]);
    return FALSE;
}

#define SOME_VRAM_STORE(ptr, posY, posX, toStore)                       \
{                                                                       \
    u32 index = (posY) * 32 + posX;                                     \
    ptr[index] = toStore;                                               \
}

static void SpriteCB_FldEffPokeballTrail(struct Sprite* sprite)
{
    s16 speeds[2];

    memcpy(speeds, sPokeballsTrail_Speeds, sizeof(sPokeballsTrail_Speeds));
    if (sprite->data[1] != 0)
        sprite->data[1]--;
    else
    {
        if (sprite->x >= 0 && sprite->x <= 240)
        {
            s16 posX = sprite->x >> 3;
            s16 posY = sprite->y >> 3;

            if (posX != sprite->data[2])
            {
                u32 var;
                u16 *ptr;

                sprite->data[2] = posX;
                var = (((REG_BG0CNT >> 8) & 0x1F) << 11);  // r2
                ptr = (u16 *)(VRAM + var);

                SOME_VRAM_STORE(ptr, posY - 2, posX, 0xF001);
                SOME_VRAM_STORE(ptr, posY - 1, posX, 0xF001);
                SOME_VRAM_STORE(ptr, posY - 0, posX, 0xF001);
                SOME_VRAM_STORE(ptr, posY + 1, posX, 0xF001);
            }
        }
        sprite->x += speeds[sprite->data[0]];
        if (sprite->x < -15 || sprite->x > 255)
            FieldEffectStop(sprite, FLDEFF_POKEBALL_TRAIL);
    }
}

static void Task_ClockwiseWipe(u8 taskId)
{
    while (sClockwiseWipe_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 ClockwiseWipe_Init(struct Task* task)
{
    u16 i;

    InitTransitionData();
    ScanlineEffect_Clear();

    TRANSITION_STRUCT.WININ = 0;
    TRANSITION_STRUCT.WINOUT = 63;
    TRANSITION_STRUCT.WIN0H = -3855;
    TRANSITION_STRUCT.WIN0V = 160;

    for (i = 0; i < 160; i++)
    {
        gScanlineEffectRegBuffers[1][i] = 0xF3F4;
    }

    SetVBlankCallback(VBlankCB_ClockwiseWipe);
    TRANSITION_STRUCT.data[4] = 120;

    task->tState++;
    return TRUE;
}

static bool8 ClockwiseWipe_TopRight(struct Task* task)
{
    TRANSITION_STRUCT.VBlank_DMA = 0;

    InitBlackWipe(TRANSITION_STRUCT.data, 120, 80, TRANSITION_STRUCT.data[4], -1, 1, 1);
    do
    {
        gScanlineEffectRegBuffers[0][TRANSITION_STRUCT.data[3]] = (TRANSITION_STRUCT.data[2] + 1) | 0x7800;
    } while (!UpdateBlackWipe(TRANSITION_STRUCT.data, 1, 1));

    TRANSITION_STRUCT.data[4] += 16;
    if (TRANSITION_STRUCT.data[4] >= 240)
    {
        TRANSITION_STRUCT.data[5] = 0;
        task->tState++;
    }

    TRANSITION_STRUCT.VBlank_DMA++;
    return FALSE;
}

static bool8 ClockwiseWipe_Right(struct Task* task)
{
    s16 start, end;
    vu8 finished = 0;

    TRANSITION_STRUCT.VBlank_DMA = 0;

    InitBlackWipe(TRANSITION_STRUCT.data, 120, 80, 240, TRANSITION_STRUCT.data[5], 1, 1);

    while (1)
    {
        start = 120, end = TRANSITION_STRUCT.data[2] + 1;
        if (TRANSITION_STRUCT.data[5] >= 80)
            start = TRANSITION_STRUCT.data[2], end = 240;
        gScanlineEffectRegBuffers[0][TRANSITION_STRUCT.data[3]] = (end) | (start << 8);
        if (finished != 0)
            break;
        finished = UpdateBlackWipe(TRANSITION_STRUCT.data, 1, 1);
    }

    TRANSITION_STRUCT.data[5] += 8;
    if (TRANSITION_STRUCT.data[5] >= 160)
    {
        TRANSITION_STRUCT.data[4] = 240;
        task->tState++;
    }
    else
    {
        while (TRANSITION_STRUCT.data[3] < TRANSITION_STRUCT.data[5])
        {
            gScanlineEffectRegBuffers[0][++TRANSITION_STRUCT.data[3]] = (end) | (start << 8);
        }
    }

    TRANSITION_STRUCT.VBlank_DMA++;
    return FALSE;
}

static bool8 ClockwiseWipe_Bottom(struct Task* task)
{
    TRANSITION_STRUCT.VBlank_DMA = 0;

    InitBlackWipe(TRANSITION_STRUCT.data, 120, 80, TRANSITION_STRUCT.data[4], 160, 1, 1);
    do
    {
        gScanlineEffectRegBuffers[0][TRANSITION_STRUCT.data[3]] = (TRANSITION_STRUCT.data[2] << 8) | 0xF0;
    } while (!UpdateBlackWipe(TRANSITION_STRUCT.data, 1, 1));

    TRANSITION_STRUCT.data[4] -= 16;
    if (TRANSITION_STRUCT.data[4] <= 0)
    {
        TRANSITION_STRUCT.data[5] = 160;
        task->tState++;
    }

    TRANSITION_STRUCT.VBlank_DMA++;
    return FALSE;
}

static bool8 ClockwiseWipe_Left(struct Task* task)
{
    s16 end, start, r3;
    vu8 finished = 0;

    TRANSITION_STRUCT.VBlank_DMA = 0;

    InitBlackWipe(TRANSITION_STRUCT.data, 120, 80, 0, TRANSITION_STRUCT.data[5], 1, 1);

    while (1)
    {
        end = gScanlineEffectRegBuffers[0][TRANSITION_STRUCT.data[3]] & 0xFF, start = TRANSITION_STRUCT.data[2];
        if (TRANSITION_STRUCT.data[5] <= 80)
            start = 120, end = TRANSITION_STRUCT.data[2];
        gScanlineEffectRegBuffers[0][TRANSITION_STRUCT.data[3]] = (end) | (start << 8);
        r3 = 0;
        if (finished != 0)
            break;
        finished = UpdateBlackWipe(TRANSITION_STRUCT.data, 1, 1);
    }

    TRANSITION_STRUCT.data[5] -= 8;
    if (TRANSITION_STRUCT.data[5] <= 0)
    {
        TRANSITION_STRUCT.data[4] = r3;
        task->tState++;
    }
    else
    {
        while (TRANSITION_STRUCT.data[3] > TRANSITION_STRUCT.data[5])
        {
            gScanlineEffectRegBuffers[0][--TRANSITION_STRUCT.data[3]] = (end) | (start << 8);
        }
    }

    TRANSITION_STRUCT.VBlank_DMA++;
    return FALSE;
}

static bool8 ClockwiseWipe_TopLeft(struct Task* task)
{
    TRANSITION_STRUCT.VBlank_DMA = 0;

    InitBlackWipe(TRANSITION_STRUCT.data, 120, 80, TRANSITION_STRUCT.data[4], 0, 1, 1);
    do
    {
        s16 start, end;

        start = 120, end = TRANSITION_STRUCT.data[2];
        if (TRANSITION_STRUCT.data[2] >= 120)
            start = 0, end = 240;
        gScanlineEffectRegBuffers[0][TRANSITION_STRUCT.data[3]] = (end) | (start << 8);

    } while (!UpdateBlackWipe(TRANSITION_STRUCT.data, 1, 1));

    TRANSITION_STRUCT.data[4] += 16;
    if (TRANSITION_STRUCT.data[2] > 120)
        task->tState++;

    TRANSITION_STRUCT.VBlank_DMA++;
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
    if (TRANSITION_STRUCT.VBlank_DMA != 0)
        DmaCopy16(3, gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], 320);
    REG_WININ = TRANSITION_STRUCT.WININ;
    REG_WINOUT = TRANSITION_STRUCT.WINOUT;
    REG_WIN0V = TRANSITION_STRUCT.WIN0V;
    REG_WIN0H = gScanlineEffectRegBuffers[1][0];
    DmaSet(0, gScanlineEffectRegBuffers[1], &REG_WIN0H, 0xA2400001);
}

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
        gScanlineEffectRegBuffers[1][i] = TRANSITION_STRUCT.cameraY;
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

    TRANSITION_STRUCT.VBlank_DMA = 0;

    amplitude = task->data[2] >> 8;
    sinVal = task->data[1];
    speed = 384;
    task->data[1] += 0x400;
    if (task->data[2] <= 0x1FFF)
        task->data[2] += 0x180;

    for (i = 0; i < 160; i++, sinVal += speed)
    {
        // todo: fix the asm
        s16 sinIndex = sinVal >> 8;
        asm("");
        gScanlineEffectRegBuffers[0][i] = TRANSITION_STRUCT.cameraY + Sin(sinIndex, amplitude);
        asm("");
    }

    if (++task->data[3] == 81)
    {
        task->data[4]++;
        BeginNormalPaletteFade(0xFFFFFFFF, -2, 0, 16, RGB(0, 0, 0));
    }

    if (task->data[4] != 0 && !gPaletteFade.active)
        DestroyTask(FindTaskIdByFunc(Task_Ripple));

    TRANSITION_STRUCT.VBlank_DMA++;
    return FALSE;
}

static void VBlankCB_Ripple(void)
{
    VBlankCB_BattleTransition();
    if (TRANSITION_STRUCT.VBlank_DMA)
        DmaCopy16(3, gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], 320);
}

static void HBlankCB_Ripple(void)
{
    u16 var = gScanlineEffectRegBuffers[1][REG_VCOUNT];
    REG_BG1VOFS = var;
    REG_BG2VOFS = var;
    REG_BG3VOFS = var;
}

static void Task_Wave(u8 taskId)
{
    while (sWave_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 Wave_Init(struct Task* task)
{
    u8 i;

    InitTransitionData();
    ScanlineEffect_Clear();

    TRANSITION_STRUCT.WININ = 63;
    TRANSITION_STRUCT.WINOUT = 0;
    TRANSITION_STRUCT.WIN0H = 240;
    TRANSITION_STRUCT.WIN0V = 160;

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

    TRANSITION_STRUCT.VBlank_DMA = 0;
    toStore = gScanlineEffectRegBuffers[0];
    sinIndex = task->data[2];
    task->data[2] += 16;
    task->data[1] += 8;

    for (i = 0, finished = TRUE; i < 160; i++, sinIndex += 4, toStore++)
    {
        s16 x = task->data[1] + Sin(sinIndex, 40);
        if (x < 0)
            x = 0;
        if (x > 240)
            x = 240;
        *toStore = (x << 8) | (0xF1);
        if (x < 240)
            finished = FALSE;
    }
    if (finished)
        task->tState++;

    TRANSITION_STRUCT.VBlank_DMA++;
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
    if (TRANSITION_STRUCT.VBlank_DMA != 0)
        DmaCopy16(3, gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], 320);
    REG_WININ = TRANSITION_STRUCT.WININ;
    REG_WINOUT = TRANSITION_STRUCT.WINOUT;
    REG_WIN0V = TRANSITION_STRUCT.WIN0V;
    DmaSet(0, gScanlineEffectRegBuffers[1], &REG_WIN0H, 0xA2400001);
}

#define tOpponentSpriteId  data[13]
#define tPlayerSpriteId    data[14]
#define tMugshotId          data[15]

static void Task_Sidney(u8 taskId)
{
    gTasks[taskId].tMugshotId = MUGSHOT_SIDNEY;
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

static void Phase2Task_Transition_Steven(u8 taskId)
{
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
    Mugshots_CreateOpponentPlayerSprites(task);

    task->data[1] = 0;
    task->data[2] = 1;
    task->data[3] = 239;
    TRANSITION_STRUCT.WININ = 63;
    TRANSITION_STRUCT.WINOUT = 62;
    TRANSITION_STRUCT.WIN0V = 160;

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
    const u16* mugshotsMap;

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
    s32 mergedValue;

    TRANSITION_STRUCT.VBlank_DMA = 0;

    toStore = gScanlineEffectRegBuffers[0];
    sinIndex = task->data[1];
    task->data[1] += 0x10;

    for (i = 0; i < 80; i++, toStore++, sinIndex += 0x10)
    {
        x = task->data[2] + Sin(sinIndex, 0x10);
        if (x < 0)
            x = 1;
        if (x > 0xF0)
            x = 0xF0;
        *toStore = x;
    }
    for (; i < 160; i++, toStore++, sinIndex += 0x10)
    {
        x = task->data[3] - Sin(sinIndex, 0x10);
        if (x < 0)
            x = 0;
        if (x > 0xEF)
            x = 0xEF;
        *toStore = (x << 8) | (0xF0);
    }

    task->data[2] += 8;
    task->data[3] -= 8;
    if (task->data[2] > 0xF0)
        task->data[2] = 0xF0;
    if (task->data[3] < 0)
        task->data[3] = 0;
    mergedValue = *(s32*)(&task->data[2]);
    if (mergedValue == 0xF0)
        task->tState++;

    TRANSITION_STRUCT.BG0HOFS_Lower -= 8;
    TRANSITION_STRUCT.BG0HOFS_Upper += 8;
    TRANSITION_STRUCT.VBlank_DMA++;
    return FALSE;
}

static bool8 Mugshot_StartOpponentSlide(struct Task* task)
{
    u8 i;
    u16* toStore;

    TRANSITION_STRUCT.VBlank_DMA = 0;

    for (i = 0, toStore = gScanlineEffectRegBuffers[0]; i < 160; i++, toStore++)
    {
        *toStore = 0xF0;
    }

    task->tState++;
    task->data[1] = 0;
    task->data[2] = 0;
    task->data[3] = 0;
    TRANSITION_STRUCT.BG0HOFS_Lower -= 8;
    TRANSITION_STRUCT.BG0HOFS_Upper += 8;

    SetTrainerPicSlideDirection(task->tOpponentSpriteId, 0);
    SetTrainerPicSlideDirection(task->tPlayerSpriteId, 1);
    IncrementTrainerPicState(task->tOpponentSpriteId);

    PlaySE(SE_MUGSHOT);

    TRANSITION_STRUCT.VBlank_DMA++;
    return FALSE;
}

static bool8 Mugshot_WaitStartPlayerSlide(struct Task* task)
{
    TRANSITION_STRUCT.BG0HOFS_Lower -= 8;
    TRANSITION_STRUCT.BG0HOFS_Upper += 8;
    if (IsTrainerPicSlideDone(task->tOpponentSpriteId))
    {
        task->tState++;
        IncrementTrainerPicState(task->tPlayerSpriteId);
    }
    return FALSE;
}

static bool8 Mugshot_WaitPlayerSlide(struct Task* task)
{
    TRANSITION_STRUCT.BG0HOFS_Lower -= 8;
    TRANSITION_STRUCT.BG0HOFS_Upper += 8;
    if (IsTrainerPicSlideDone(task->tPlayerSpriteId))
    {
        TRANSITION_STRUCT.VBlank_DMA = 0;
        SetVBlankCallback(NULL);
        DmaStop(0);
        memset(gScanlineEffectRegBuffers[0], 0, 0x140);
        memset(gScanlineEffectRegBuffers[1], 0, 0x140);
        REG_WIN0H = 0xF0;
        REG_BLDY = 0;
        task->tState++;
        task->data[3] = 0;
        task->data[4] = 0;
        TRANSITION_STRUCT.BLDCNT = 0xBF;
        SetVBlankCallback(VBlankCB_MugshotsFadeOut);
    }
    return FALSE;
}

static bool8 Mugshot_GradualWhiteFade(struct Task* task)
{
    bool32 active;

    TRANSITION_STRUCT.VBlank_DMA = 0;
    active = TRUE;
    TRANSITION_STRUCT.BG0HOFS_Lower -= 8;
    TRANSITION_STRUCT.BG0HOFS_Upper += 8;

    if (task->data[4] < 0x50)
        task->data[4] += 2;
    if (task->data[4] > 0x50)
        task->data[4] = 0x50;

    if (++task->data[3] & 1)
    {
        s16 i;
        for (i = 0, active = FALSE; i <= task->data[4]; i++)
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

    if (task->data[4] == 0x50 && !active)
        task->tState++;

    TRANSITION_STRUCT.VBlank_DMA++;
    return FALSE;
}

static bool8 Mugshot_InitFadeWhiteToBlack(struct Task* task)
{
    TRANSITION_STRUCT.VBlank_DMA = 0;
    BlendPalettes(0xFFFFFFFF, 16, RGB(31, 31, 31));
    TRANSITION_STRUCT.BLDCNT = 0xFF;
    task->data[3] = 0;

    task->tState++;
    return TRUE;
}

static bool8 Mugshot_FadeToBlack(struct Task* task)
{
    TRANSITION_STRUCT.VBlank_DMA = 0;

    task->data[3]++;
    memset(gScanlineEffectRegBuffers[0], task->data[3], 0x140);
    if (task->data[3] > 15)
        task->tState++;

    TRANSITION_STRUCT.VBlank_DMA++;
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
    if (TRANSITION_STRUCT.VBlank_DMA != 0)
        DmaCopy16(3, gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], 320);
    REG_BG0VOFS = TRANSITION_STRUCT.BG0VOFS;
    REG_WININ = TRANSITION_STRUCT.WININ;
    REG_WINOUT = TRANSITION_STRUCT.WINOUT;
    REG_WIN0V = TRANSITION_STRUCT.WIN0V;
    DmaSet(0, gScanlineEffectRegBuffers[1], &REG_WIN0H, 0xA2400001);
}

static void VBlankCB_MugshotsFadeOut(void)
{
    DmaStop(0);
    VBlankCB_BattleTransition();
    if (TRANSITION_STRUCT.VBlank_DMA != 0)
        DmaCopy16(3, gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], 320);
    REG_BLDCNT = TRANSITION_STRUCT.BLDCNT;
    DmaSet(0, gScanlineEffectRegBuffers[1], &REG_BLDY, 0xA2400001);
}

static void HBlankCB_Mugshots(void)
{
    if (REG_VCOUNT < 80)
        REG_BG0HOFS = TRANSITION_STRUCT.BG0HOFS_Lower;
    else
        REG_BG0HOFS = TRANSITION_STRUCT.BG0HOFS_Upper;
}

static void Mugshots_CreateOpponentPlayerSprites(struct Task* task)
{
    struct Sprite *opponentSprite, *playerSprite;

    s16 mugshotId = task->tMugshotId;
    task->tOpponentSpriteId = CreateTrainerSprite(sMugshotsTrainerPicIDsTable[mugshotId],
                                                     sMugshotsOpponentCoords[mugshotId][0] - 32,
                                                     sMugshotsOpponentCoords[mugshotId][1] + 42,
                                                     0,
        eTransitionMugshotDecompBuff);
    task->tPlayerSpriteId = CreateTrainerSprite(gSaveBlock2.playerGender, 272, 106, 0, eTransitionMugshotDecompBuff);

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
    while (sMugshotTrainerPicFuncs[sprite->data[0]](sprite));
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

    sprite->data[0]++;
    sprite->data[1] = speeds[sprite->data[7]];
    sprite->data[2] = accels[sprite->data[7]];
    return TRUE;
}

static bool8 MugshotTrainerPic_Slide(struct Sprite* sprite)
{
    sprite->x += sprite->data[1];
    if (sprite->data[7] && sprite->x < 133)
        sprite->data[0]++;
    else if (!sprite->data[7] && sprite->x > 103)
        sprite->data[0]++;
    return FALSE;
}

static bool8 MugshotTrainerPic_SlideSlow(struct Sprite* sprite)
{
    sprite->data[1] += sprite->data[2];
    sprite->x += sprite->data[1];
    if (sprite->data[1] == 0)
    {
        sprite->data[0]++;
        sprite->data[2] = -sprite->data[2];
        sprite->data[6] = 1;
    }
    return FALSE;
}

static bool8 MugshotTrainerPic_SlideOffscreen(struct Sprite* sprite)
{
    sprite->data[1] += sprite->data[2];
    sprite->x += sprite->data[1];
    if (sprite->x < -31 || sprite->x > 271)
        sprite->data[0]++;
    return FALSE;
}

static void SetTrainerPicSlideDirection(s16 spriteId, s16 dirId)
{
    gSprites[spriteId].data[7] = dirId;
}

static void IncrementTrainerPicState(s16 spriteId)
{
    gSprites[spriteId].data[0]++;
}

static s16 IsTrainerPicSlideDone(s16 spriteId)
{
    return gSprites[spriteId].data[6];
}

#undef tOpponentSpriteId
#undef tPlayerSpriteId
#undef tMugshotId

static void Task_Slice(u8 taskId)
{
    while (sSlice_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 Slice_Init(struct Task* task)
{
    u16 i;

    InitTransitionData();
    ScanlineEffect_Clear();

    task->data[2] = 256;
    task->data[3] = 1;
    TRANSITION_STRUCT.WININ = 63;
    TRANSITION_STRUCT.WINOUT = 0;
    TRANSITION_STRUCT.WIN0V = 160;

    for (i = 0; i < 160; i++)
    {
        gScanlineEffectRegBuffers[1][i] = TRANSITION_STRUCT.cameraX;
        gScanlineEffectRegBuffers[1][160 + i] = 0xF0;
    }

    REG_IE |= INTR_FLAG_HBLANK;
    REG_DISPSTAT |= DISPSTAT_HBLANK_INTR;

    SetVBlankCallback(VBlankCB_Slice);
    SetHBlankCallback(HBlankCB_Phase2_Transition_Slice);

    task->tState++;
    return TRUE;
}

static bool8 Slice_Main(struct Task* task)
{
    u16 i;

    TRANSITION_STRUCT.VBlank_DMA = 0;

    task->data[1] += (task->data[2] >> 8);
    if (task->data[1] > 0xF0)
        task->data[1] = 0xF0;
    if (task->data[2] <= 0xFFF)
        task->data[2] += task->data[3];
    if (task->data[3] < 128)
        task->data[3] <<= 1; // multiplying by two

    for (i = 0; i < 160; i++)
    {
        u16* storeLoc1 = &gScanlineEffectRegBuffers[0][i];
        u16* storeLoc2 = &gScanlineEffectRegBuffers[0][i + 160];
        if (1 & i)
        {
            *storeLoc1 = TRANSITION_STRUCT.cameraX + task->data[1];
            *storeLoc2 = 0xF0 - task->data[1];
        }
        else
        {
            *storeLoc1 = TRANSITION_STRUCT.cameraX - task->data[1];
            *storeLoc2 = (task->data[1] << 8) | (0xF1);
        }
    }

    if (task->data[1] > 0xEF)
        task->tState++;

    TRANSITION_STRUCT.VBlank_DMA++;
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
    REG_WININ = TRANSITION_STRUCT.WININ;
    REG_WINOUT = TRANSITION_STRUCT.WINOUT;
    REG_WIN0V = TRANSITION_STRUCT.WIN0V;
    if (TRANSITION_STRUCT.VBlank_DMA)
        DmaCopy16(3, gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], 640);
    DmaSet(0, &gScanlineEffectRegBuffers[1][160], &REG_WIN0H, 0xA2400001);
}

static void HBlankCB_Phase2_Transition_Slice(void)
{
    u16 var = gScanlineEffectRegBuffers[1][REG_VCOUNT];
    REG_BG1HOFS = var;
    REG_BG2HOFS = var;
    REG_BG3HOFS = var;
}

static void Task_WhiteBarsFade(u8 taskId)
{
    while (sWhiteBarsFade_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 WhiteBarsFade_Init(struct Task* task)
{
    u16 i;

    InitTransitionData();
    ScanlineEffect_Clear();

    TRANSITION_STRUCT.BLDCNT = 0xBF;
    TRANSITION_STRUCT.BLDY = 0;
    TRANSITION_STRUCT.WININ = 0x1E;
    TRANSITION_STRUCT.WINOUT = 0x3F;
    TRANSITION_STRUCT.WIN0V = 0xA0;

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
    s16 delays[8];
    struct Sprite* sprite;

    memcpy(delays, sWhiteBarsFade_StartDelays, sizeof(sWhiteBarsFade_StartDelays));
    for (i = 0, posY = 0; i < 8; i++, posY += 0x14)
    {
        sprite = &gSprites[CreateInvisibleSprite(SpriteCB_WhiteBarFade)];
        sprite->x = 0xF0;
        sprite->y = posY;
        sprite->data[5] = delays[i];
    }
    sprite->data[6]++;

    task->tState++;
    return FALSE;
}

static bool8 WhiteBarsFade_WaitBars(struct Task* task)
{
    TRANSITION_STRUCT.VBlank_DMA = 0;
    if (TRANSITION_STRUCT.counter > 7)
    {
        BlendPalettes(0xFFFFFFFF, 16, RGB(31, 31, 31));
        task->tState++;
    }
    return FALSE;
}

static bool8 WhiteBarsFade_BlendToBlack(struct Task* task)
{
    TRANSITION_STRUCT.VBlank_DMA = 0;

    DmaStop(0);
    SetVBlankCallback(0);
    SetHBlankCallback(0);

    TRANSITION_STRUCT.WIN0H = 0xF0;
    TRANSITION_STRUCT.BLDY = 0;
    TRANSITION_STRUCT.BLDCNT = 0xFF;
    TRANSITION_STRUCT.WININ = 0x3F;

    SetVBlankCallback(VBlankCB_WhiteBarsFade_Blend);

    task->tState++;
    return FALSE;
}

static bool8 WhiteBarsFade_End(struct Task* task)
{
   if (++TRANSITION_STRUCT.BLDY > 16)
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
    REG_BLDCNT = TRANSITION_STRUCT.BLDCNT;
    REG_WININ = TRANSITION_STRUCT.WININ;
    REG_WINOUT = TRANSITION_STRUCT.WINOUT;
    REG_WIN0V = TRANSITION_STRUCT.WIN0H;
    if (TRANSITION_STRUCT.VBlank_DMA)
        DmaCopy16(3, gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], 640);
    DmaSet(0, &gScanlineEffectRegBuffers[1][160], &REG_WIN0H, 0xA2400001);
}

static void VBlankCB_WhiteBarsFade_Blend(void)
{
    VBlankCB_BattleTransition();
    REG_BLDY = TRANSITION_STRUCT.BLDY;
    REG_BLDCNT = TRANSITION_STRUCT.BLDCNT;
    REG_WININ = TRANSITION_STRUCT.WININ;
    REG_WINOUT = TRANSITION_STRUCT.WINOUT;
    REG_WIN0H = TRANSITION_STRUCT.WIN0H;
    REG_WIN0V = TRANSITION_STRUCT.WIN0V;
}

static void HBlankCB_WhiteBarsFade(void)
{
    REG_BLDY = gScanlineEffectRegBuffers[1][REG_VCOUNT];
}

static void SpriteCB_WhiteBarFade(struct Sprite* sprite)
{
    if (sprite->data[5])
    {
        sprite->data[5]--;
        if (sprite->data[6])
            TRANSITION_STRUCT.VBlank_DMA = 1;
    }
    else
    {
        u16 i;
        u16* ptr1 = &gScanlineEffectRegBuffers[0][sprite->y];
        u16* ptr2 = &gScanlineEffectRegBuffers[0][sprite->y + 160];
        for (i = 0; i < 20; i++)
        {
            ptr1[i] = sprite->data[0] >> 8;
            ptr2[i] = (u8)(sprite->x);
        }
        if (sprite->x == 0 && sprite->data[0] == 0x1000)
            sprite->data[1] = 1;

        sprite->x -= 16;
        sprite->data[0] += 0x80;

        if (sprite->x < 0)
            sprite->x = 0;
        if (sprite->data[0] > 0x1000)
            sprite->data[0] = 0x1000;

        if (sprite->data[6])
            TRANSITION_STRUCT.VBlank_DMA = 1;

        if (sprite->data[1])
        {
            if (sprite->data[6] == 0 || (TRANSITION_STRUCT.counter > 6 && sprite->data[2]++ > 7))
            {
                TRANSITION_STRUCT.counter++;
                DestroySprite(sprite);
            }
        }
    }
}

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

    if (task->data[1] == 0)
    {
        GetBg0TilemapDst(&tileset);
        task->data[1] = 3;
        task->data[2]++;
        CpuSet(sShrinkingBoxTileset + (task->data[2] * 8), tileset, 0x10);
        if (task->data[2] > 0xD)
        {
            task->tState++;
            task->data[1] = 16;
        }
    }

    task->data[1]--;
    return FALSE;
}

static bool8 GridSquares_End(struct Task* task)
{
    if (--task->data[1] == 0)
    {
        FadeScreenBlack();
        DestroyTask(FindTaskIdByFunc(Task_GridSquares));
    }
    return FALSE;
}

static void Task_AngledWipes(u8 taskId)
{
    while (sAngledWipes_Funcs[gTasks[taskId].tState](&gTasks[taskId]));
}

static bool8 AngledWipes_Init(struct Task* task)
{
    u16 i;

    InitTransitionData();
    ScanlineEffect_Clear();

    TRANSITION_STRUCT.WININ = 0x3F;
    TRANSITION_STRUCT.WINOUT = 0;
    TRANSITION_STRUCT.WIN0V = 0xA0;

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
    InitBlackWipe(TRANSITION_STRUCT.data,
                sAngledWipes_MoveData[task->data[1]][0],
                sAngledWipes_MoveData[task->data[1]][1],
                sAngledWipes_MoveData[task->data[1]][2],
                sAngledWipes_MoveData[task->data[1]][3],
                1, 1);
    task->data[2] = sAngledWipes_MoveData[task->data[1]][4];
    task->tState++;
    return TRUE;
}

static bool8 AngledWipes_DoWipe(struct Task* task)
{
    s16 i;
    bool8 finished;

    TRANSITION_STRUCT.VBlank_DMA = 0;

    for (i = 0, finished = FALSE; i < 16; i++)
    {
        s16 r3 = gScanlineEffectRegBuffers[0][TRANSITION_STRUCT.data[3]] >> 8;
        s16 r4 = gScanlineEffectRegBuffers[0][TRANSITION_STRUCT.data[3]] & 0xFF;
        if (task->data[2] == 0)
        {
            if (r3 < TRANSITION_STRUCT.data[2])
                r3 = TRANSITION_STRUCT.data[2];
            if (r3 > r4)
                r3 = r4;
        }
        else
        {
            if (r4 > TRANSITION_STRUCT.data[2])
                r4 = TRANSITION_STRUCT.data[2];
            if (r4 <= r3)
                r4 = r3;
        }
        gScanlineEffectRegBuffers[0][TRANSITION_STRUCT.data[3]] = (r4) | (r3 << 8);
        if (finished)
        {
            task->tState++;
            break;
        }
        else
            finished = UpdateBlackWipe(TRANSITION_STRUCT.data, 1, 1);
    }

    TRANSITION_STRUCT.VBlank_DMA++;
    return FALSE;
}

static bool8 AngledWipes_TryEnd(struct Task* task)
{
    if (++task->data[1] < 7)
    {
        task->tState++;
        task->data[3] = sAngledWipes_EndDelays[task->data[1] - 1];
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
    if (--task->data[3] == 0)
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
    if (TRANSITION_STRUCT.VBlank_DMA)
        DmaCopy16(3, gScanlineEffectRegBuffers[0], gScanlineEffectRegBuffers[1], 320);
    REG_WININ = TRANSITION_STRUCT.WININ;
    REG_WINOUT = TRANSITION_STRUCT.WINOUT;
    REG_WIN0V = TRANSITION_STRUCT.WIN0V;
    REG_WIN0H = gScanlineEffectRegBuffers[1][0];
    DmaSet(0, gScanlineEffectRegBuffers[1], &REG_WIN0H, 0xA2400001);
}

static void CreateIntroTask(s16 fadeToGrayDelay, s16 fadeFromGrayDelay, s16 numFades, s16 fadeToGrayIncrement, s16 fadeFromGrayIncrement)
{
    u8 taskId = CreateTask(Task_BattleTransition_Intro, 3);
    gTasks[taskId].data[1] = fadeToGrayDelay;
    gTasks[taskId].data[2] = fadeFromGrayDelay;
    gTasks[taskId].data[3] = numFades;
    gTasks[taskId].data[4] = fadeToGrayIncrement;
    gTasks[taskId].data[5] = fadeFromGrayIncrement;
    gTasks[taskId].data[6] = fadeToGrayDelay;
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
    if (task->data[6] == 0 || --task->data[6] == 0)
    {
        task->data[6] = task->data[1];
        task->data[7] += task->data[4];
        if (task->data[7] > 16)
            task->data[7] = 16;
        BlendPalettes(0xFFFFFFFF, task->data[7], RGB(11, 11, 11));
    }
    if (task->data[7] > 15)
    {
        task->tState++;
        task->data[6] = task->data[2];
    }
    return FALSE;
}

static bool8 TransitionIntro_FadeFromGray(struct Task* task)
{
    if (task->data[6] == 0 || --task->data[6] == 0)
    {
        task->data[6] = task->data[2];
        task->data[7] -= task->data[5];
        if (task->data[7] < 0)
            task->data[7] = 0;
        BlendPalettes(0xFFFFFFFF, task->data[7], RGB(11, 11, 11));
    }
    if (task->data[7] == 0)
    {
        if (--task->data[3] == 0)
            DestroyTask(FindTaskIdByFunc(Task_BattleTransition_Intro));
        else
        {
            task->data[6] = task->data[1];
            task->tState = 0;
        }
    }
    return FALSE;
}

static void InitTransitionData(void)
{
    struct TransitionData* const* dummy = &sTransitionData;
    memset(*dummy, 0, sizeof(struct TransitionData));
    GetCameraOffsetWithPan(&TRANSITION_STRUCT.cameraX, &TRANSITION_STRUCT.cameraY);
}

static void VBlankCB_BattleTransition(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void GetBg0TilemapDst(u16** tileset)
{
    u16 charBase, *vram;

    charBase = REG_BG0CNT >> 2;
    charBase <<= 0xE;
    vram = (u16*)(VRAM + charBase);

    *tileset = vram;
}

static void GetBg0TilesDst(u16** tilemap, u16** tileset)
{
    u16 screenBase, charBase, *vram0, *vram1;

    screenBase = REG_BG0CNT >> 8;
    charBase = REG_BG0CNT >> 2;

    screenBase <<= 0xB;
    charBase <<= 0xE;

    vram0 = (u16*)(VRAM + screenBase);
    *tilemap = vram0;

    vram1 = (u16*)(VRAM + charBase);
    *tileset = vram1;
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
        s16 drawXLeft, drawYBottNext, drawYTopNext, drawX, drawYTop, drawYBott;

        sinResult = Sin(i, radius);
        cosResult = Cos(i, radius);

        drawXLeft = centerX - sinResult;
        drawX = centerX + sinResult;
        drawYTop = centerY - cosResult;
        drawYBott = centerY + cosResult;

        if (drawXLeft < 0)
            drawXLeft = 0;
        if (drawX > 0xF0)
            drawX = 0xF0;
        if (drawYTop < 0)
            drawYTop = 0;
        if (drawYBott > 0x9F)
            drawYBott = 0x9F;

        drawX |= (drawXLeft << 8);
        buffer[drawYTop] = drawX;
        buffer[drawYBott] = drawX;

        cosResult = Cos(i + 1, radius);
        drawYTopNext = centerY - cosResult;
        drawYBottNext = centerY + cosResult;

        if (drawYTopNext < 0)
            drawYTopNext = 0;
        if (drawYBottNext > 0x9F)
            drawYBottNext = 0x9F;

        while (drawYTop > drawYTopNext)
            buffer[--drawYTop] = drawX;
        while (drawYTop < drawYTopNext)
            buffer[++drawYTop] = drawX;

        while (drawYBott > drawYBottNext)
            buffer[--drawYBott] = drawX;
        while (drawYBott < drawYBottNext)
            buffer[++drawYBott] = drawX;
    }
}

static void InitBlackWipe(s16* data, s16 startX, s16 startY, s16 endX, s16 endY, s16 xMove, s16 yMove)
{
    data[0] = startX;
    data[1] = startY;
    data[2] = startX;
    data[3] = startY;
    data[4] = endX;
    data[5] = endY;
    data[6] = xMove;
    data[7] = yMove;
    data[8] = endX - startX;
    if (data[8] < 0)
    {
        data[8] = -data[8];
        data[6] = -xMove;
    }
    data[9] = endY - startY;
    if (data[9] < 0)
    {
        data[9] = -data[9];
        data[7] = -yMove;
    }
    data[10] = 0;
}

static bool8 UpdateBlackWipe(s16* data, bool8 xExact, bool8 yExact)
{
    u8 numFinished;
    if (data[8] > data[9])
    {
        data[2] += data[6];
        data[10] += data[9];
        if (data[10] > data[8])
        {
            data[3] += data[7];
            data[10] -= data[8];
        }
    }
    else
    {
        data[3] += data[7];
        data[10] += data[8];
        if (data[10] > data[9])
        {
            data[2] += data[6];
            data[10] -= data[9];
        }
    }
    numFinished = 0;
    if ((data[6] > 0 && data[2] >= data[4]) || (data[6] < 0 && data[2] <= data[4]))
    {
        numFinished++;
        if (xExact)
            data[2] = data[4];
    }
    if ((data[7] > 0 && data[3] >= data[5]) || (data[7] < 0 && data[3] <= data[5]))
    {
        numFinished++;
        if (yExact)
            data[3] = data[5];
    }
    if (numFinished == 2)
        return TRUE;
    else
        return FALSE;
}
