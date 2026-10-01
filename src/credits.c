#include "global.h"
#include "data2.h"
#include "decompress.h"
#include "event_data.h"
#include "hall_of_fame.h"
#include "intro_credits_graphics.h"
#include "m4a.h"
#include "main.h"
#include "menu.h"
#include "palette.h"
#include "pokedex.h"
#include "random.h"
#include "constants/songs.h"
#include "sound.h"
#include "constants/species.h"
#include "starter_choose.h"
#include "task.h"
#include "trig.h"
#include "ewram.h"

asm(".set REG_BASE, 0x4000000");
asm(".set OFFSET_REG_BLDCNT,      0x50");
asm(".set OFFSET_REG_BLDALPHA,    0x52");
asm(".set REG_BLDCNT,      REG_BASE + OFFSET_REG_BLDCNT");
asm(".set REG_BLDALPHA,    REG_BASE + OFFSET_REG_BLDALPHA");

enum
{
    PAGE_TITLE,
    PAGE_DIRECTOR,
    PAGE_ART_DIRECTOR,
    PAGE_BATTLE_DIRECTOR,
    PAGE_MAIN_PROGRAMMER,
    PAGE_BATTLE_SYSTEM_PROGRAMMER,
    PAGE_PROGRAMMERS_1,
    PAGE_PROGRAMMERS_2,
    PAGE_PROGRAMMERS_3,
    PAGE_MAIN_GRAPHICS_DESIGNER,
    PAGE_POKEMON_GRAPHIC_DESIGNERS_1,
    PAGE_POKEMON_GRAPHIC_DESIGNERS_2,
    PAGE_POKEMON_GRAPHIC_DESIGNERS_3,
    PAGE_POKEMON_DESIGNERS_1,
    PAGE_POKEMON_DESIGNERS_2,
    PAGE_MUSIC_COMPOSITION,
    PAGE_SOUND_EFFECTS,
    PAGE_GAME_DESIGNERS_1,
    PAGE_GAME_DESIGNERS_2,
    PAGE_GAME_DESIGNERS_3,
    PAGE_PLOT_SCENARIO,
    PAGE_GAME_SCENARIO,
    PAGE_SCRIPT_DESIGNERS,
    PAGE_MAP_DESIGNERS,
    PAGE_MAP_DATA_DESIGNERS,
    PAGE_PARAMETRIC_DESIGNERS,
    PAGE_POKEDEX_TEXT,
    PAGE_ENVIRONMENT_TOOLS,
    PAGE_PRODUCT_TESTING,
    PAGE_SPECIAL_THANKS,
    PAGE_SPECIAL_THANKS_1,
    PAGE_SPECIAL_THANKS_2,
    PAGE_SPECIAL_THANKS_3,
    PAGE_INFORMATION_SUPERVISORS,
    PAGE_COORDINATORS,
    PAGE_TASK_MANAGERS,
    PAGE_PRODUCERS,
    PAGE_EXECUTIVE_DIRECTOR,
    PAGE_EXECUTIVE_PRODUCERS_1,
    PAGE_EXECUTIVE_PRODUCERS_2,
    PAGE_TRANSLATION_COORDINATOR,
    PAGE_TRANSLATORS,
    PAGE_PROGRAMMERS,
    PAGE_GRAPHIC_DESIGNERS,
    PAGE_PRODUCT_SUPPORT,

#if ENGLISH
    PAGE_ARTWORK,
    PAGE_TEXT_EDITOR,
    PAGE_NOA_TESTING,
    PAGE_BRAILLE_CODE_CHECK_1,
    PAGE_BRAILLE_CODE_CHECK_2,
#elif GERMAN
    PAGE_NOE_TESTING,
    PAGE_BRAILLE_CODE_CHECK_1,
#endif

    PAGE_SPECIAL_THANKS_4,
    PAGE_SPECIAL_THANKS_5,

    PAGE_COUNT
};

#if ENGLISH
#define NUM_MON_SLIDES 68
#define LAST_PAGE (PAGE_TEXT_EDITOR)
#define FOREST_RIVAL_ARRIVAL_SINE_INDEX (0x45)
#define CREDITS_PAGE_HOLD_FRAMES (0x82)
#define FOREST_RIVAL_CHASE_DELAY (499)
#elif GERMAN
#define NUM_MON_SLIDES 65
#define LAST_PAGE (PAGE_NOE_TESTING)
#define FOREST_RIVAL_ARRIVAL_SINE_INDEX (8)
#define CREDITS_PAGE_HOLD_FRAMES (0x8D)
#define FOREST_RIVAL_CHASE_DELAY (554)
#endif

#define COLOR_DARK_GREEN 0x1967
#define COLOR_LIGHT_GREEN 0x328D

#define TAG_MON_BG 1001

enum
{
    POS_LEFT,
    POS_CENTER,
    POS_RIGHT,
};

enum
{
    MODE_NONE,
    MODE_BIKE_SCENE,
    MODE_SHOW_MONS,
};

enum
{
    TDA_BG_SCENERY_TASK_ID = 0,
    TDA_END_SCREEN_DELAY = 0, // The same task slot is reused after the scenery task ends.
    TDA_BIKE_SCENE_TASK_ID = 1,
    TDA_SCENERY_PALETTE_TASK_ID = 2,
    TDA_SHOW_MONS_TASK_ID = 3,
    TDA_END_CREDITS = 4,
    TDA_PLAYER_SPRITE_ID = 5,
    TDA_RIVAL_SPRITE_ID = 6,
    TDA_SCENE_NUM = 7,
    TDA_NEXT_MODE = 11,
    TDA_THE_END_DELAY = 12,
    TDA_CURRENT_MODE = 13,
    TDA_PRINTED_PAGE = 14,
    TDA_UPDATE_PAGE_TASK_ID = 15,

    // Credits page task
    TDB_STATE = 0,
    TDB_MAIN_TASK_ID = 1,
    TDB_CURRENT_PAGE = 2,
    TDB_DELAY = 3,

    TDC_STATE = 0,
    TDC_MAIN_TASK_ID = 1,
    TDC_PLAYER_SPRITE_ID = 2,
    TDC_RIVAL_SPRITE_ID = 3,
    TDC_DELAY = 4,
    TDC_SINE_INDEX = 5,

    TDD_STATE = 0,
    TDD_MAIN_TASK_ID = 1,
    TDD_SCENE_NUM = 2,
    TDD_DELAY = 3,

    TDE_SCENE_NUM = 0,
    TDE_TIMER = 1,
    TDE_MAIN_TASK_ID = 2,
};


struct CreditsData
{
    u16 monToShow[NUM_MON_SLIDES]; // List of Pokemon species ids that will show during the credits
    u16 imgCounter; //how many mon images have been shown
    u16 nextImgPos; //if the next image spawns left/center/right
    u16 currShownMon; //index into monToShow
    u16 numMonToShow; //number of pokemon to show, always NUM_MON_SLIDES after determine function
    u16 caughtMonIds[NATIONAL_DEX_COUNT]; //temporary location to hold a condensed array of all caught pokemon
    u16 numCaughtMon; //count of filled spaces in caughtMonIds
    u16 unused[7];
};

struct CreditsEntry
{
    u8 unused;
    const u8 *text;
};

extern struct SpriteTemplate gCreatingSpriteTemplate;

static EWRAM_DATA s16 sUnusedCreditsValue = 0;
static EWRAM_DATA u16 sSavedTaskId = 0; // TASK A
EWRAM_DATA u8 gHasHallOfFameRecords = 0;
static EWRAM_DATA u8 sUsedSpeedUp = 0;

extern u8 gReservedSpritePaletteCount;

// data/credits
static const u16 sCredits_Pal[] = INCBIN_U16("graphics/credits/credits.gbapal");
static const u8 sCreditsAmpersand_Gfx[] = INCBIN_U8("graphics/credits/ampersand.4bpp");

void SpriteCB_CreditsMonBg(struct Sprite *sprite);

const u8 sTheEnd_LetterMap_T[] =
{
    0,    1, 0,
    0xFF, 1, 0xFF,
    0xFF, 1, 0xFF,
    0xFF, 1, 0xFF,
    0xFF, 1, 0xFF,
};

const u8 sTheEnd_LetterMap_H[] =
{
    1, 0xFF, 1,
    1, 0xFF, 1,
    1, 2,    1,
    1, 0xFF, 1,
    1, 0xFF, 1,
};

const u8 sTheEnd_LetterMap_E[] =
{
    1, 0, 0,
    1, 0xFF, 0xFF,
    1, 2,    2,
    1, 0xFF, 0xFF,
    1, 0x80, 0x80,
};

const u8 sTheEnd_LetterMap_N[] =
{
    1, 3, 1,
    1, 4, 1,
    1, 5, 1,
    1, 0xC4, 1,
    1, 0xC3, 1,
};

const u8 sTheEnd_LetterMap_D[] =
{
    1, 6, 7,
    1, 8, 9,
    1, 0xFF, 1,
    1, 0x88, 0x89,
    1, 0x86, 0x87,
};

#ifdef GERMAN
const u8 sTheEnd_LetterMap_F[] = {
    1, 0, 0,
    1, 0xFF, 0xFF,
    1, 0x80, 0x8A,
    1, 0xFF, 0xFF,
    1, 0xFF, 0xFF,
};

const u8 sTheEnd_LetterMap_I[] = {
    0, 1, 0,
    0xFF, 1, 0xFF,
    0xFF, 1, 0xFF,
    0xFF, 1, 0xFF,
    0x80, 1, 0x80,
};
#endif

#ifdef GERMAN
#include "data/credits_de.h"
#else
#include "data/credits_en.h"
#endif

const u8 sMonSpritePos[][2] =
{
    {104, 36},
    {120, 36},
    {136, 36},
};

static const union AnimCmd sAnim_Player_Slow[] =
{
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_FRAME(64, 8),
    ANIMCMD_FRAME(128, 8),
    ANIMCMD_FRAME(192, 8),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sAnim_Player_Fast[] =
{
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(64, 4),
    ANIMCMD_FRAME(128, 4),
    ANIMCMD_FRAME(192, 4),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sAnim_Player_LookBack[] =
{
    ANIMCMD_FRAME(256, 4),
    ANIMCMD_FRAME(320, 4),
    ANIMCMD_FRAME(384, 4),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_Player_LookForward[] =
{
    ANIMCMD_FRAME(384, 30),
    ANIMCMD_FRAME(320, 30),
    ANIMCMD_FRAME(256, 30),
    ANIMCMD_FRAME(256, 30),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_Player[] =
{
    sAnim_Player_Slow,
    sAnim_Player_Fast,
    sAnim_Player_LookBack,
    sAnim_Player_LookForward,
};

static const union AnimCmd sAnim_Rival_Slow[] =
{
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_FRAME(64, 8),
    ANIMCMD_FRAME(128, 8),
    ANIMCMD_FRAME(192, 8),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sAnim_Rival_Fast[] =
{
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_FRAME(64, 4),
    ANIMCMD_FRAME(128, 4),
    ANIMCMD_FRAME(192, 4),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sAnim_Rival_Still[] =
{
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_Rival[] =
{
    sAnim_Rival_Slow,
    sAnim_Rival_Fast,
    sAnim_Rival_Still,
};

#define MONBG_OFFSET 0x1800

static const struct SpriteSheet sSpriteSheet_MonBg[] = {{gDecompressionBuffer, MONBG_OFFSET, TAG_MON_BG}, {0}};
static const struct SpritePalette sSpritePalette_MonBg[] = {{(const u16 *)(gDecompressionBuffer + MONBG_OFFSET), TAG_MON_BG}, {0}};

static const struct OamData sOamData_MonBg =
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

static const union AnimCmd sAnim_MonBg_Yellow[] =
{
    ANIMCMD_FRAME(0, 8),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_MonBg_Red[] =
{
    ANIMCMD_FRAME(64, 8),
    ANIMCMD_END,
};

static const union AnimCmd sAnim_MonBg_Blue[] =
{
    ANIMCMD_FRAME(128, 8),
    ANIMCMD_END,
};

static const union AnimCmd *const sAnims_MonBg[] =
{
    sAnim_MonBg_Yellow,
    sAnim_MonBg_Red,
    sAnim_MonBg_Blue,
};

static const struct SpriteTemplate sSpriteTemplate_CreditsMonBg =
{
    .tileTag = TAG_MON_BG,
    .paletteTag = TAG_MON_BG,
    .oam = &sOamData_MonBg,
    .anims = sAnims_MonBg,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_CreditsMonBg,
};

// graphics
extern u8 gCreditsCopyrightEnd_Gfx[];
extern u16 gIntroCopyright_Pal[16];

static void Task_WaitPaletteFade(u8 taskIdA);
static void Task_CreditsMain(u8 taskIdA);
static void Task_ReadyBikeScene(u8 taskIdA);
static void Task_SetBikeScene(u8 taskIdA);
static void Task_ReadyShowMons(u8 taskIdA);
static void Task_LoadShowMons(u8 taskIdA);
static void Task_CreditsTheEnd1(u8 taskIdA);
static void Task_CreditsTheEnd2(u8 taskIdA);
static void Task_CreditsTheEnd3(u8 taskIdA);
static void Task_CreditsTheEnd4(u8 taskIdA);
static void Task_CreditsTheEnd5(u8 taskIdA);
static void Task_CreditsTheEnd6(u8 taskIdA);
static void Task_CreditsSoftReset(u8 taskIdA);
static void ResetGpuAndVram(void);
static void Task_UpdatePage(u8 taskIdB);
static u8 CheckChangeScene(u8 page, u8 taskIdA);
static void Task_ShowMons(u8 taskIdD);
static bool8 LoadBikeScene(u8 scene, u8 taskIdA);
static void ResetCreditsTasks(u8 taskIdA);
static void LoadTheEndScreen(u16 tileOffsetLoad, u16 tileOffsetWrite, u16 palOffset);
static void DrawTheEnd(u16 offset, u16 palette);
static void SpriteCB_Player(struct Sprite *sprite);
static void SpriteCB_Rival(struct Sprite *sprite);
static u8 CreateCreditsMonSprite(u16 species, u16 x, u16 y, u16 position);
static void DeterminePokemonToShow(void);

static void VBlankCB_Credits(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void CB2_Credits(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();

    if (JOY_HELD(B_BUTTON)
     && gHasHallOfFameRecords != 0
     && gTasks[sSavedTaskId].func == Task_CreditsMain)
    {
        VBlankCB_Credits();
        RunTasks();
        AnimateSprites();
        BuildOamBuffer();
        UpdatePaletteFade();
        sUsedSpeedUp = 1;
    }
}

void CB2_StartCreditsSequence(void)
{
    u8 taskIdA;
    s16 bikeTaskId;
    u8 pageTaskId;
    u16 savedIme;
    struct CreditsData *pCreditsData;

    ResetGpuAndVram();
    SetVBlankCallback(NULL);
    ResetPaletteFade();
    ResetTasks();

    taskIdA = CreateTask(Task_WaitPaletteFade, 0);

    gTasks[taskIdA].data[TDA_END_CREDITS] = 0;
    gTasks[taskIdA].data[TDA_SCENE_NUM] = SCENE_OCEAN_MORNING;
    gTasks[taskIdA].data[TDA_NEXT_MODE] = MODE_NONE;
    gTasks[taskIdA].data[TDA_CURRENT_MODE] = MODE_BIKE_SCENE;

    while (TRUE)
    {
        if (LoadBikeScene(SCENE_OCEAN_MORNING, taskIdA))
            break;
    }

    bikeTaskId = gTasks[taskIdA].data[TDA_BIKE_SCENE_TASK_ID];
    gTasks[bikeTaskId].data[TDC_STATE] = 40;

    Text_LoadWindowTemplate(&gWindowTemplate_Credits);
    InitMenuWindow(&gWindowTemplate_Credits);
    LoadPalette(sCredits_Pal, 0x80, sizeof(sCredits_Pal));

    CpuCopy16(sCreditsAmpersand_Gfx, (void *)(VRAM + 0xBEE0), sizeof(sCreditsAmpersand_Gfx));

    REG_BG0VOFS = 0xFFFC;

    pageTaskId = CreateTask(Task_UpdatePage, 0);

    gTasks[pageTaskId].data[TDB_MAIN_TASK_ID] = taskIdA;
    gTasks[taskIdA].data[TDA_UPDATE_PAGE_TASK_ID] = pageTaskId;

    BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));

    savedIme = REG_IME;
    REG_IME = 0;
    REG_IE |= INTR_FLAG_VBLANK;
    REG_IME = savedIme;
    REG_DISPSTAT |= DISPSTAT_VBLANK_INTR;


    SetVBlankCallback(VBlankCB_Credits);
    m4aSongNumStart(MUS_CREDITS);
    SetMainCallback2(CB2_Credits);
    sUsedSpeedUp = 0;

    pCreditsData = &sCreditsData;

    DeterminePokemonToShow();

    pCreditsData->imgCounter = 0;
    pCreditsData->nextImgPos = 0;
    pCreditsData->currShownMon = 0;

    sSavedTaskId = taskIdA;
}

static void Task_WaitPaletteFade(u8 taskIdA)
{
    if (!gPaletteFade.active)
        gTasks[taskIdA].func = Task_CreditsMain;
}

static void Task_CreditsMain(u8 taskIdA)
{
    u16 mode;

    if (gTasks[taskIdA].data[TDA_END_CREDITS])
    {
        s16 bikeTaskId;

        bikeTaskId = gTasks[taskIdA].data[TDA_BIKE_SCENE_TASK_ID];
        gTasks[bikeTaskId].data[TDC_STATE] = 30;

        gTasks[taskIdA].data[TDA_THE_END_DELAY] = 0x100;
        gTasks[taskIdA].func = Task_CreditsTheEnd1;
        return;
    }

    sUnusedCreditsValue = 0;
    mode = gTasks[taskIdA].data[TDA_NEXT_MODE];

    if (gTasks[taskIdA].data[TDA_NEXT_MODE] == MODE_BIKE_SCENE)
    {
        gTasks[taskIdA].data[TDA_CURRENT_MODE] = mode;
        gTasks[taskIdA].data[TDA_NEXT_MODE] = MODE_NONE;
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        gTasks[taskIdA].func = Task_ReadyBikeScene;
    }
    else if (gTasks[taskIdA].data[TDA_NEXT_MODE] == MODE_SHOW_MONS)
    {
        gTasks[taskIdA].data[TDA_CURRENT_MODE] = mode;
        gTasks[taskIdA].data[TDA_NEXT_MODE] = MODE_NONE;
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        gTasks[taskIdA].func = Task_ReadyShowMons;
    }
}

static void Task_ReadyBikeScene(u8 taskIdA)
{
    if (!gPaletteFade.active)
    {
        REG_DISPCNT = 0;
        ResetCreditsTasks(taskIdA);
        gTasks[taskIdA].func = Task_SetBikeScene;
    }
}

static void Task_SetBikeScene(u8 taskIdA)
{
    u16 backup;

    SetVBlankCallback(NULL);

    if (LoadBikeScene(gTasks[taskIdA].data[TDA_SCENE_NUM], taskIdA))
    {
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));

        backup = REG_IME;
        REG_IME = 0;
        REG_IE |= INTR_FLAG_VBLANK;
        REG_IME = backup;
        REG_DISPSTAT |= DISPSTAT_VBLANK_INTR;

        SetVBlankCallback(VBlankCB_Credits);
        gTasks[taskIdA].func = Task_WaitPaletteFade;
    }
}

static void Task_ReadyShowMons(u8 taskIdA)
{
    if (!gPaletteFade.active)
    {
        REG_DISPCNT = 0;
        ResetCreditsTasks(taskIdA);
        gTasks[taskIdA].func = Task_LoadShowMons;
    }
}

void Task_LoadShowMons(u8 taskIdA)
{
    switch (gMain.state)
    {
    default:
    case 0:
    {
        u16 i;

        ResetSpriteData();
        FreeAllSpritePalettes();
        gReservedSpritePaletteCount = 8;
        LZ77UnCompVram(&gBirchBagGrass_Gfx, (void *)VRAM);
        LZ77UnCompVram(&gBirchGrassTilemap, (void *)(VRAM + 0x3800));
        LoadPalette(gBirchBagGrass_Pal[0] + 1, 1, 31 * 2);

        for (i = 0; i < 0x800; i++)
            gDecompressionBuffer[i] = 0x11;
        for (i = 0; i < 0x800; i++)
            (gDecompressionBuffer + 0x800)[i] = 0x22;
        for (i = 0; i < 0x800; i++)
            (gDecompressionBuffer + 0x1000)[i] = 0x33;

        ((u16 *)(gDecompressionBuffer + MONBG_OFFSET))[0] = RGB( 0,  0,  0); // black
        ((u16 *)(gDecompressionBuffer + MONBG_OFFSET))[1] = RGB(31, 31, 20); // light yellow
        ((u16 *)(gDecompressionBuffer + MONBG_OFFSET))[2] = RGB(31, 20, 20); // light red
        ((u16 *)(gDecompressionBuffer + MONBG_OFFSET))[3] = RGB(20, 20, 31); // light blue

        LoadSpriteSheet(sSpriteSheet_MonBg);
        LoadSpritePalette(sSpritePalette_MonBg);

        gMain.state += 1;
        break;
    }
    case 1:
        gTasks[taskIdA].data[TDA_SHOW_MONS_TASK_ID] = CreateTask(Task_ShowMons, 0);
        gTasks[gTasks[taskIdA].data[TDA_SHOW_MONS_TASK_ID]].data[TDD_STATE] = 1;
        gTasks[gTasks[taskIdA].data[TDA_SHOW_MONS_TASK_ID]].data[TDD_MAIN_TASK_ID] = taskIdA;
        gTasks[gTasks[taskIdA].data[TDA_SHOW_MONS_TASK_ID]].data[TDD_SCENE_NUM] = gTasks[taskIdA].data[TDA_SCENE_NUM];

        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        REG_BG3HOFS = 0;
        REG_BG3VOFS = 32;
        REG_BG3CNT = BGCNT_PRIORITY(3) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(7) | BGCNT_16COLOR | BGCNT_TXT256x256;
        REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG3_ON | DISPCNT_OBJ_ON;

        gMain.state = 0;
        gIntroCredits_MovingSceneryState = INTROCRED_SCENERY_NORMAL;
        gTasks[taskIdA].func = Task_WaitPaletteFade;
        break;
    }
}

static void Task_CreditsTheEnd1(u8 taskIdA)
{
    if (gTasks[taskIdA].data[TDA_THE_END_DELAY])
    {
        gTasks[taskIdA].data[TDA_THE_END_DELAY] -= 1;
        return;
    }

    BeginNormalPaletteFade(0xFFFFFFFF, 12, 0, 16, RGB(0, 0, 0));
    gTasks[taskIdA].func = Task_CreditsTheEnd2;
}

static void Task_CreditsTheEnd2(u8 taskIdA)
{
    if (!gPaletteFade.active)
    {
        ResetCreditsTasks(taskIdA);
        gTasks[taskIdA].func = Task_CreditsTheEnd3;
    }
}

static void Task_CreditsTheEnd3(u8 taskIdA)
{
    u16 backup;

    ResetGpuAndVram();
    ResetPaletteFade();
    LoadTheEndScreen(0, 0x3800, 0);
    ResetSpriteData();
    FreeAllSpritePalettes();
    BeginNormalPaletteFade(0xFFFFFFFF, 8, 16, 0, RGB(0, 0, 0));

    REG_BG0CNT = BGCNT_PRIORITY(0) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(7) | BGCNT_16COLOR | BGCNT_TXT256x256;
    backup = REG_IME;
    REG_IME = 0;
    REG_IE |= INTR_FLAG_VBLANK;
    REG_IME = backup;
    REG_DISPSTAT |= DISPSTAT_VBLANK_INTR;
    REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON;

    gTasks[taskIdA].data[TDA_END_SCREEN_DELAY] = 0x100;
    gTasks[taskIdA].func = Task_CreditsTheEnd4;
}

static void Task_CreditsTheEnd4(u8 taskIdA)
{
    if (gTasks[taskIdA].data[TDA_END_SCREEN_DELAY])
    {
        gTasks[taskIdA].data[TDA_END_SCREEN_DELAY] -= 1;
        return;
    }

    BeginNormalPaletteFade(0xFFFFFFFF, 6, 0, 16, RGB(0, 0, 0));
    gTasks[taskIdA].func = Task_CreditsTheEnd5;
}

static void Task_CreditsTheEnd5(u8 taskIdA)
{
    if (!gPaletteFade.active)
    {
        DrawTheEnd(0x3800, 0);

        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 0, RGB(0, 0, 0));
        gTasks[taskIdA].data[TDA_END_SCREEN_DELAY] = 7200;
        gTasks[taskIdA].func = Task_CreditsTheEnd6;
    }
}

static void Task_CreditsTheEnd6(u8 taskIdA)
{
    if (!gPaletteFade.active)
    {
        if (gTasks[taskIdA].data[TDA_END_SCREEN_DELAY] == 0)
        {
            FadeOutBGM(4);
            BeginNormalPaletteFade(0xFFFFFFFF, 8, 0, 16, FADE_COLOR_WHITE);
            gTasks[taskIdA].func = Task_CreditsSoftReset;
            return;
        }

        if (gMain.newKeys)
        {
            FadeOutBGM(4);
            BeginNormalPaletteFade(0xFFFFFFFF, 8, 0, 16, FADE_COLOR_WHITE);
            gTasks[taskIdA].func = Task_CreditsSoftReset;
            return;
        }

        if (gTasks[taskIdA].data[TDA_END_SCREEN_DELAY] == 7144)
        {
            FadeOutBGM(8);
        }

        if (gTasks[taskIdA].data[TDA_END_SCREEN_DELAY] == 6840)
            m4aSongNumStart(MUS_END);

        gTasks[taskIdA].data[TDA_END_SCREEN_DELAY] -= 1;
    }
}

static void Task_CreditsSoftReset(u8 taskIdA)
{
    if (!gPaletteFade.active)
        SoftReset(0xFF);
}

static void ResetGpuAndVram(void)
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

    DmaFill16(3, 0, (void *)VRAM, VRAM_SIZE);
    DmaFill32(3, 0, (void *)OAM, OAM_SIZE);
    DmaFill16(3, 0, (void *)(PLTT + 2), PLTT_SIZE - 2);
}

static void Task_UpdatePage(u8 taskIdB)
{
    u16 i;

    switch (gTasks[taskIdB].data[TDB_STATE])
    {
    case 0:
    case 6:
    case 7:
    case 8:
    case 9:
    default:
        if (!gPaletteFade.active)
        {
            gTasks[taskIdB].data[TDB_STATE] = 1;
            gTasks[taskIdB].data[TDB_DELAY] = 0x58;
            gTasks[gTasks[taskIdB].data[TDB_MAIN_TASK_ID]].data[TDA_PRINTED_PAGE] = 0;
            sUnusedCreditsValue = 0;
        }
        return;
    case 1:
        if (gTasks[taskIdB].data[TDB_DELAY] != 0)
        {
            gTasks[taskIdB].data[TDB_DELAY] -= 1;
            return;
        }
        gTasks[taskIdB].data[TDB_STATE] += 1;
        return;
    case 2:
        REG_DISPCNT &= ~DISPCNT_BG0_ON;
        if (gTasks[gTasks[taskIdB].data[TDB_MAIN_TASK_ID]].func == Task_CreditsMain)
        {
            if (gTasks[taskIdB].data[TDB_CURRENT_PAGE] < PAGE_COUNT)
            {
                for (i = 0; i < 5; i++)
                    MenuPrint_Centered(gCreditsEntryPointerTable[gTasks[taskIdB].data[TDB_CURRENT_PAGE]][i]->text, 0, 9 + i * 2, 240);

                gTasks[taskIdB].data[TDB_CURRENT_PAGE] += 1;
                gTasks[taskIdB].data[TDB_STATE] += 1;

                gTasks[gTasks[taskIdB].data[TDB_MAIN_TASK_ID]].data[TDA_PRINTED_PAGE] = 1;

                if (gTasks[gTasks[taskIdB].data[TDB_MAIN_TASK_ID]].data[TDA_CURRENT_MODE] == MODE_BIKE_SCENE)
                    BeginNormalPaletteFade(0x300, 0, 16, 0, COLOR_LIGHT_GREEN);
                else
                    BeginNormalPaletteFade(0x300, 0, 16, 0, COLOR_DARK_GREEN);
                return;
            }
            gTasks[taskIdB].data[TDB_STATE] = 10;
            return;
        }
        gTasks[gTasks[taskIdB].data[TDB_MAIN_TASK_ID]].data[TDA_PRINTED_PAGE] = 0;
        return;
    case 3:
        REG_DISPCNT |= DISPCNT_BG0_ON;
        if (!gPaletteFade.active)
        {
            gTasks[taskIdB].data[TDB_DELAY] = CREDITS_PAGE_HOLD_FRAMES;
            gTasks[taskIdB].data[TDB_STATE] += 1;
        }
        return;
    case 4:
        if (gTasks[taskIdB].data[TDB_DELAY] != 0)
        {
            gTasks[taskIdB].data[TDB_DELAY] -= 1;
            return;
        }

        if (CheckChangeScene((u8)gTasks[taskIdB].data[TDB_CURRENT_PAGE], (u8)gTasks[taskIdB].data[TDB_MAIN_TASK_ID]))
        {
            gTasks[taskIdB].data[TDB_STATE] += 1;
            return;
        }
        gTasks[taskIdB].data[TDB_STATE] += 1;
        if (gTasks[gTasks[taskIdB].data[TDB_MAIN_TASK_ID]].data[TDA_CURRENT_MODE] == MODE_BIKE_SCENE)
            BeginNormalPaletteFade(0x300, 0, 0, 16, COLOR_LIGHT_GREEN);
        else
            BeginNormalPaletteFade(0x300, 0, 0, 16, COLOR_DARK_GREEN);
        return;
    case 5:
        if (!gPaletteFade.active)
        {
            Menu_EraseWindowRect(0, 9, 29, 19);
            gTasks[taskIdB].data[TDB_STATE] = 2;
        }
        return;

    case 10:
        gTasks[gTasks[taskIdB].data[TDB_MAIN_TASK_ID]].data[TDA_END_CREDITS] = 1;
        DestroyTask(taskIdB);
        return;
    }
}

static u8 CheckChangeScene(u8 page, u8 taskIdA)
{
    // Starts with bike + ocean + morning

    if (page == PAGE_PROGRAMMERS_1)
    {
        // Grass patch
        gTasks[taskIdA].data[TDA_NEXT_MODE] = MODE_SHOW_MONS;
    }

    if (page == PAGE_POKEMON_GRAPHIC_DESIGNERS_3)
    {
        // Bike + ocean + sunset
        gTasks[taskIdA].data[TDA_SCENE_NUM] = SCENE_OCEAN_SUNSET;
        gTasks[taskIdA].data[TDA_NEXT_MODE] = MODE_BIKE_SCENE;
    }

    if (page == PAGE_GAME_DESIGNERS_2)
    {
        // Grass patch
        gTasks[taskIdA].data[TDA_NEXT_MODE] = MODE_SHOW_MONS;
    }

    if (page == PAGE_MAP_DATA_DESIGNERS)
    {
        // Bike + forest + sunset
        gTasks[taskIdA].data[TDA_SCENE_NUM] = SCENE_FOREST_RIVAL_ARRIVE;
        gTasks[taskIdA].data[TDA_NEXT_MODE] = MODE_BIKE_SCENE;
    }

    if (page == PAGE_SPECIAL_THANKS_1)
    {
        // Grass patch
        gTasks[taskIdA].data[TDA_NEXT_MODE] = MODE_SHOW_MONS;
    }

    if (page == PAGE_TASK_MANAGERS)
    {
        // Bike + forest + sunset
        gTasks[taskIdA].data[TDA_SCENE_NUM] = SCENE_FOREST_CATCH_RIVAL;
        gTasks[taskIdA].data[TDA_NEXT_MODE] = MODE_BIKE_SCENE;
    }

    if (page == PAGE_TRANSLATION_COORDINATOR)
    {
        // Grass patch
        gTasks[taskIdA].data[TDA_NEXT_MODE] = MODE_SHOW_MONS;
    }

    if (page == LAST_PAGE)
    {
        // Bike + town + night
        gTasks[taskIdA].data[TDA_SCENE_NUM] = SCENE_CITY_NIGHT;
        gTasks[taskIdA].data[TDA_NEXT_MODE] = MODE_BIKE_SCENE;
    }

    if (gTasks[taskIdA].data[TDA_NEXT_MODE] != MODE_NONE)
    {
        // Returns true if changed?
        return TRUE;
    }

    return FALSE;
}

static void Task_ShowMons(u8 taskIdD)
{
    struct CreditsData *creditsData = &sCreditsData;
    u8 spriteId;

    switch (gTasks[taskIdD].data[TDD_STATE])
    {
    case 0:
        break;
    case 1:
        if (creditsData->nextImgPos == POS_LEFT && gTasks[gTasks[taskIdD].data[TDD_MAIN_TASK_ID]].data[TDA_PRINTED_PAGE] == 0)
            break;
        gTasks[gTasks[taskIdD].data[TDD_MAIN_TASK_ID]].data[TDA_PRINTED_PAGE] = 0;
        gTasks[taskIdD].data[TDD_STATE]++;
        break;
    case 2:
        if (creditsData->imgCounter == NUM_MON_SLIDES || gTasks[gTasks[taskIdD].data[TDD_MAIN_TASK_ID]].func != Task_CreditsMain)
            break;
        spriteId = CreateCreditsMonSprite(creditsData->monToShow[creditsData->currShownMon], sMonSpritePos[creditsData->nextImgPos][0], sMonSpritePos[creditsData->nextImgPos][1], creditsData->nextImgPos);
        if (creditsData->currShownMon < creditsData->numMonToShow - 1)
        {
            creditsData->currShownMon++;
            gSprites[spriteId].data[3] = 50;
        }
        else
        {
            creditsData->currShownMon = 0;
            gSprites[spriteId].data[3] = 512;
        }
        creditsData->imgCounter++;
        if (creditsData->nextImgPos == POS_RIGHT)
            creditsData->nextImgPos = POS_LEFT;
        else
            creditsData->nextImgPos++;
        gTasks[taskIdD].data[TDD_DELAY] = 50;
        gTasks[taskIdD].data[TDD_STATE]++;
        break;
    case 3:
        if (gTasks[taskIdD].data[TDD_DELAY] != 0)
            gTasks[taskIdD].data[TDD_DELAY]--;
        else
            gTasks[taskIdD].data[TDD_STATE] = 1;
        break;
    }
}

void Task_BikeScene(u8 bikeTaskId)
{
    switch (gTasks[bikeTaskId].data[TDC_STATE])
    {
    case 0:
        gIntroCredits_MovingSceneryVOffset = Sin((gTasks[bikeTaskId].data[TDC_SINE_INDEX] >> 1) & 0x7F, 12);
        gTasks[bikeTaskId].data[TDC_SINE_INDEX]++;
        break;
    case 1:
        if (gIntroCredits_MovingSceneryVOffset != 0)
        {
            gIntroCredits_MovingSceneryVOffset = Sin((gTasks[bikeTaskId].data[TDC_SINE_INDEX] >> 1) & 0x7F, 12);
            gTasks[bikeTaskId].data[TDC_SINE_INDEX]++;
        }
        else
        {
            gSprites[gTasks[bikeTaskId].data[TDC_PLAYER_SPRITE_ID]].data[0] = 2;
            gTasks[bikeTaskId].data[TDC_SINE_INDEX] = 0;
            gTasks[bikeTaskId].data[TDC_STATE]++;
        }
        break;
    case 2:
        if (gTasks[bikeTaskId].data[TDC_SINE_INDEX] < 64)
        {
            gTasks[bikeTaskId].data[TDC_SINE_INDEX]++;
            gIntroCredits_MovingSceneryVOffset = Sin(gTasks[bikeTaskId].data[TDC_SINE_INDEX] & 0x7F, 20);
        }
        else
        {
            gTasks[bikeTaskId].data[TDC_STATE]++;
        }
        break;
    case 3:
        gSprites[gTasks[bikeTaskId].data[TDC_PLAYER_SPRITE_ID]].data[0] = 3;
        gSprites[gTasks[bikeTaskId].data[TDC_RIVAL_SPRITE_ID]].data[0] = 1;
        gTasks[bikeTaskId].data[TDC_DELAY] = 120;
        gTasks[bikeTaskId].data[TDC_STATE]++;
        break;
    case 4:
        if (gTasks[bikeTaskId].data[TDC_DELAY] != 0)
        {
            gTasks[bikeTaskId].data[TDC_DELAY]--;
        }
        else
        {
            gTasks[bikeTaskId].data[TDC_SINE_INDEX] = 64;
            gTasks[bikeTaskId].data[TDC_STATE]++;
        }
        break;
    case 5:
        if (gTasks[bikeTaskId].data[TDC_SINE_INDEX] > 0)
        {
            gTasks[bikeTaskId].data[TDC_SINE_INDEX]--;
            gIntroCredits_MovingSceneryVOffset = Sin(gTasks[bikeTaskId].data[TDC_SINE_INDEX] & 0x7F, 20);
        }
        else
        {
            gSprites[gTasks[bikeTaskId].data[TDC_PLAYER_SPRITE_ID]].data[0] = 1;
            gTasks[bikeTaskId].data[TDC_STATE]++;
        }
        break;
    case 6:
        gTasks[bikeTaskId].data[TDC_STATE] = 50;
        break;
    case 10:
        gSprites[gTasks[bikeTaskId].data[TDC_RIVAL_SPRITE_ID]].data[0] = 2;
        gTasks[bikeTaskId].data[TDC_STATE] = 50;
        break;
    case 20:
        gSprites[gTasks[bikeTaskId].data[TDC_PLAYER_SPRITE_ID]].data[0] = 4;
        gTasks[bikeTaskId].data[TDC_STATE] = 50;
        break;
    case 30:
        gSprites[gTasks[bikeTaskId].data[TDC_PLAYER_SPRITE_ID]].data[0] = 5;
        gSprites[gTasks[bikeTaskId].data[TDC_RIVAL_SPRITE_ID]].data[0] = 3;
        gTasks[bikeTaskId].data[TDC_STATE] = 50;
        break;
    case 50:
        gTasks[bikeTaskId].data[TDC_STATE] = 0;
        break;
    }
}

void Task_CycleSceneryPalette(u8 taskIdE)
{
    s16 bikeTaskId;

    switch (gTasks[taskIdE].data[TDE_SCENE_NUM])
    {
    default:
    case SCENE_OCEAN_MORNING:
        if (gTasks[taskIdE].data[TDE_TIMER] != 0x7FFF)
        {

            if (gTasks[gTasks[gTasks[taskIdE].data[TDE_MAIN_TASK_ID]].data[TDA_UPDATE_PAGE_TASK_ID]].data[TDB_CURRENT_PAGE] == PAGE_ART_DIRECTOR)
            {
                gTasks[gTasks[gTasks[taskIdE].data[TDE_MAIN_TASK_ID]].data[TDA_BIKE_SCENE_TASK_ID]].data[TDC_STATE] = 20;
                gTasks[taskIdE].data[TDE_TIMER] = 0x7FFF;
            }
        }
        CycleSceneryPalette(0);
        break;
    case SCENE_OCEAN_SUNSET:
        CycleSceneryPalette(0);
        break;
    case SCENE_FOREST_RIVAL_ARRIVE:
        if (gTasks[taskIdE].data[TDE_TIMER] != 0x7FFF)
        {
            bikeTaskId = gTasks[gTasks[taskIdE].data[TDE_MAIN_TASK_ID]].data[TDA_BIKE_SCENE_TASK_ID];

            // Floor to multiple of 128
            if ((gTasks[bikeTaskId].data[TDC_SINE_INDEX] & -128) == 640)
            {
                gTasks[bikeTaskId].data[TDC_STATE] = 1;
                gTasks[taskIdE].data[TDE_TIMER] = 0x7FFF;
            }
        }
        CycleSceneryPalette(1);
        break;
    case SCENE_FOREST_CATCH_RIVAL:
        if (gTasks[taskIdE].data[TDE_TIMER] != 0x7FFF)
        {

            if (gTasks[taskIdE].data[TDE_TIMER] == FOREST_RIVAL_CHASE_DELAY)
            {
                gTasks[gTasks[gTasks[taskIdE].data[TDE_MAIN_TASK_ID]].data[TDA_BIKE_SCENE_TASK_ID]].data[TDC_STATE] = 10;
                gTasks[taskIdE].data[TDE_TIMER] = 0x7FFF;
            }
            else
            {
                gTasks[taskIdE].data[TDE_TIMER] += 1;
            }
        }
        CycleSceneryPalette(1);
        break;
    case SCENE_CITY_NIGHT:
        CycleSceneryPalette(2);
        break;
    }
}

static void SetBikeScene(u8 scene, u8 taskIdA)
{
    switch (scene)
    {
    case SCENE_OCEAN_MORNING:
        gSprites[gTasks[taskIdA].data[TDA_PLAYER_SPRITE_ID]].invisible = FALSE;
        gSprites[gTasks[taskIdA].data[TDA_RIVAL_SPRITE_ID]].invisible = FALSE;
        gSprites[gTasks[taskIdA].data[TDA_PLAYER_SPRITE_ID]].x = 272;
        gSprites[gTasks[taskIdA].data[TDA_RIVAL_SPRITE_ID]].x = 272;
        gSprites[gTasks[taskIdA].data[TDA_PLAYER_SPRITE_ID]].y = 46;
        gSprites[gTasks[taskIdA].data[TDA_RIVAL_SPRITE_ID]].y = 46;
        gSprites[gTasks[taskIdA].data[TDA_PLAYER_SPRITE_ID]].data[0] = 0;
        gSprites[gTasks[taskIdA].data[TDA_RIVAL_SPRITE_ID]].data[0] = 0;
        gTasks[taskIdA].data[TDA_BG_SCENERY_TASK_ID] = CreateBicycleBgAnimationTask(0, 0x2000, 0x20, 8);
        break;
    case SCENE_OCEAN_SUNSET:
        gSprites[gTasks[taskIdA].data[TDA_PLAYER_SPRITE_ID]].invisible = FALSE;
        gSprites[gTasks[taskIdA].data[TDA_RIVAL_SPRITE_ID]].invisible = FALSE;
        gSprites[gTasks[taskIdA].data[TDA_PLAYER_SPRITE_ID]].x = 120;
        gSprites[gTasks[taskIdA].data[TDA_RIVAL_SPRITE_ID]].x = 272;
        gSprites[gTasks[taskIdA].data[TDA_PLAYER_SPRITE_ID]].y = 46;
        gSprites[gTasks[taskIdA].data[TDA_RIVAL_SPRITE_ID]].y = 46;
        gSprites[gTasks[taskIdA].data[TDA_PLAYER_SPRITE_ID]].data[0] = 0;
        gSprites[gTasks[taskIdA].data[TDA_RIVAL_SPRITE_ID]].data[0] = 0;
        gTasks[taskIdA].data[TDA_BG_SCENERY_TASK_ID] = CreateBicycleBgAnimationTask(0, 0x2000, 0x20, 8);
        break;
    case SCENE_FOREST_RIVAL_ARRIVE:
        gSprites[gTasks[taskIdA].data[TDA_PLAYER_SPRITE_ID]].invisible = FALSE;
        gSprites[gTasks[taskIdA].data[TDA_RIVAL_SPRITE_ID]].invisible = FALSE;
        gSprites[gTasks[taskIdA].data[TDA_PLAYER_SPRITE_ID]].x = 120;
        gSprites[gTasks[taskIdA].data[TDA_RIVAL_SPRITE_ID]].x = 272;
        gSprites[gTasks[taskIdA].data[TDA_PLAYER_SPRITE_ID]].y = 46;
        gSprites[gTasks[taskIdA].data[TDA_RIVAL_SPRITE_ID]].y = 46;
        gSprites[gTasks[taskIdA].data[TDA_PLAYER_SPRITE_ID]].data[0] = 0;
        gSprites[gTasks[taskIdA].data[TDA_RIVAL_SPRITE_ID]].data[0] = 0;
        gTasks[taskIdA].data[TDA_BG_SCENERY_TASK_ID] = CreateBicycleBgAnimationTask(1, 0x2000, 0x200, 8);
        break;
    case SCENE_FOREST_CATCH_RIVAL:
        gSprites[gTasks[taskIdA].data[TDA_PLAYER_SPRITE_ID]].invisible = FALSE;
        gSprites[gTasks[taskIdA].data[TDA_RIVAL_SPRITE_ID]].invisible = FALSE;
        gSprites[gTasks[taskIdA].data[TDA_PLAYER_SPRITE_ID]].x = 120;
        gSprites[gTasks[taskIdA].data[TDA_RIVAL_SPRITE_ID]].x = -32;
        gSprites[gTasks[taskIdA].data[TDA_PLAYER_SPRITE_ID]].y = 46;
        gSprites[gTasks[taskIdA].data[TDA_RIVAL_SPRITE_ID]].y = 46;
        gSprites[gTasks[taskIdA].data[TDA_PLAYER_SPRITE_ID]].data[0] = 0;
        gSprites[gTasks[taskIdA].data[TDA_RIVAL_SPRITE_ID]].data[0] = 0;
        gTasks[taskIdA].data[TDA_BG_SCENERY_TASK_ID] = CreateBicycleBgAnimationTask(1, 0x2000, 0x200, 8);
        break;
    case SCENE_CITY_NIGHT:
        gSprites[gTasks[taskIdA].data[TDA_PLAYER_SPRITE_ID]].invisible = FALSE;
        gSprites[gTasks[taskIdA].data[TDA_RIVAL_SPRITE_ID]].invisible = FALSE;
        gSprites[gTasks[taskIdA].data[TDA_PLAYER_SPRITE_ID]].x = 88;
        gSprites[gTasks[taskIdA].data[TDA_RIVAL_SPRITE_ID]].x = 152;
        gSprites[gTasks[taskIdA].data[TDA_PLAYER_SPRITE_ID]].y = 46;
        gSprites[gTasks[taskIdA].data[TDA_RIVAL_SPRITE_ID]].y = 46;
        gSprites[gTasks[taskIdA].data[TDA_PLAYER_SPRITE_ID]].data[0] = 0;
        gSprites[gTasks[taskIdA].data[TDA_RIVAL_SPRITE_ID]].data[0] = 0;
        gTasks[taskIdA].data[TDA_BG_SCENERY_TASK_ID] = CreateBicycleBgAnimationTask(2, 0x2000, 0x200, 8);
        break;
    }

    gTasks[taskIdA].data[TDA_SCENERY_PALETTE_TASK_ID] = CreateTask(Task_CycleSceneryPalette, 0);
    gTasks[gTasks[taskIdA].data[TDA_SCENERY_PALETTE_TASK_ID]].data[TDE_SCENE_NUM] = scene;
    gTasks[gTasks[taskIdA].data[TDA_SCENERY_PALETTE_TASK_ID]].data[TDE_TIMER] = 0;
    gTasks[gTasks[taskIdA].data[TDA_SCENERY_PALETTE_TASK_ID]].data[TDE_MAIN_TASK_ID] = taskIdA;

    gTasks[taskIdA].data[TDA_BIKE_SCENE_TASK_ID] = CreateTask(Task_BikeScene, 0);
    gTasks[gTasks[taskIdA].data[TDA_BIKE_SCENE_TASK_ID]].data[TDC_STATE] = 0;
    gTasks[gTasks[taskIdA].data[TDA_BIKE_SCENE_TASK_ID]].data[TDC_MAIN_TASK_ID] = taskIdA;
    gTasks[gTasks[taskIdA].data[TDA_BIKE_SCENE_TASK_ID]].data[TDC_PLAYER_SPRITE_ID] = gTasks[taskIdA].data[TDA_PLAYER_SPRITE_ID];
    gTasks[gTasks[taskIdA].data[TDA_BIKE_SCENE_TASK_ID]].data[TDC_RIVAL_SPRITE_ID] = gTasks[taskIdA].data[TDA_RIVAL_SPRITE_ID];
    gTasks[gTasks[taskIdA].data[TDA_BIKE_SCENE_TASK_ID]].data[TDC_DELAY] = 0;

    if (scene == SCENE_FOREST_RIVAL_ARRIVE)
        gTasks[gTasks[taskIdA].data[TDA_BIKE_SCENE_TASK_ID]].data[TDC_SINE_INDEX] = FOREST_RIVAL_ARRIVAL_SINE_INDEX;
}

static bool8 LoadBikeScene(u8 scene, u8 taskIdA)
{
    u8 spriteId;

    switch (gMain.state)
    {
    default:
    case 0:
        REG_DISPCNT = 0;
        REG_BG3HOFS = 8;
        REG_BG3VOFS = 0;
        REG_BG2HOFS = 0;
        REG_BG2VOFS = 0;
        REG_BG1HOFS = 0;
        REG_BG1VOFS = 0;
        REG_BLDCNT = 0;
        REG_BLDALPHA = 0;
        ResetSpriteData();
        FreeAllSpritePalettes();
        gMain.state = 1;
        break;
    case 1:
        gIntroCredits_MovingSceneryVBase = 34;
        gIntroCredits_MovingSceneryVOffset = 0;
        LoadCreditsSceneGraphics(scene);
        gMain.state += 1;
        break;
    case 2:
        if (gSaveBlock2.playerGender == MALE)
        {
            LoadCompressedSpriteSheet(gSpriteSheet_IntroBrendan);
            LoadCompressedSpriteSheet(gSpriteSheet_CreditsRivalMay);
            LoadCompressedSpriteSheet(gSpriteSheet_IntroBicycle);
            LoadSpritePalettes(gSpritePalettes_IntroPlayerLati);

            spriteId = CreateIntroBrendanSprite(120, 46);
            gTasks[taskIdA].data[TDA_PLAYER_SPRITE_ID] = spriteId;
            gSprites[spriteId].callback = SpriteCB_Player;
            gSprites[spriteId].anims = sAnims_Player;

            spriteId = CreateIntroMaySprite(272, 46);
            gTasks[taskIdA].data[TDA_RIVAL_SPRITE_ID] = spriteId;
            gSprites[spriteId].callback = SpriteCB_Rival;
            gSprites[spriteId].anims = sAnims_Rival;
        }
        else
        {
            LoadCompressedSpriteSheet(gSpriteSheet_IntroMay);
            LoadCompressedSpriteSheet(gSpriteSheet_CreditsRivalBrendan);
            LoadCompressedSpriteSheet(gSpriteSheet_IntroBicycle);
            LoadSpritePalettes(gSpritePalettes_IntroPlayerLati);

            spriteId = CreateIntroMaySprite(120, 46);
            gTasks[taskIdA].data[TDA_PLAYER_SPRITE_ID] = spriteId;
            gSprites[spriteId].callback = SpriteCB_Player;
            gSprites[spriteId].anims = sAnims_Player;

            spriteId = CreateIntroBrendanSprite(272, 46);
            gTasks[taskIdA].data[TDA_RIVAL_SPRITE_ID] = spriteId;
            gSprites[spriteId].callback = SpriteCB_Rival;
            gSprites[spriteId].anims = sAnims_Rival;
        };
        gMain.state += 1;
        break;
    case 3:
        SetBikeScene(scene, taskIdA);
        SetCreditsSceneBgCnt(scene);
        gMain.state = 0;
        return TRUE;
    }
    return FALSE;
}

static void ResetCreditsTasks(u8 taskIdA)
{
    if (gTasks[taskIdA].data[TDA_BG_SCENERY_TASK_ID] != 0)
    {
        DestroyTask(gTasks[taskIdA].data[TDA_BG_SCENERY_TASK_ID]);
        gTasks[taskIdA].data[TDA_BG_SCENERY_TASK_ID] = 0;
    }

    if (gTasks[taskIdA].data[TDA_BIKE_SCENE_TASK_ID] != 0)
    {
        DestroyTask(gTasks[taskIdA].data[TDA_BIKE_SCENE_TASK_ID]);
        gTasks[taskIdA].data[TDA_BIKE_SCENE_TASK_ID] = 0;
    }

    if (gTasks[taskIdA].data[TDA_SCENERY_PALETTE_TASK_ID] != 0)
    {
        DestroyTask(gTasks[taskIdA].data[TDA_SCENERY_PALETTE_TASK_ID]);
        gTasks[taskIdA].data[TDA_SCENERY_PALETTE_TASK_ID] = 0;
    }

    if (gTasks[taskIdA].data[TDA_SHOW_MONS_TASK_ID] != 0)
    {
        DestroyTask(gTasks[taskIdA].data[TDA_SHOW_MONS_TASK_ID]);
        gTasks[taskIdA].data[TDA_SHOW_MONS_TASK_ID] = 0;
    }

    gIntroCredits_MovingSceneryState = INTROCRED_SCENERY_DESTROY;
}

static void LoadTheEndScreen(u16 tileOffsetLoad, u16 tileOffsetWrite, u16 palOffset)
{
    u16 baseTile;
    u16 i;

    LZ77UnCompVram(gCreditsCopyrightEnd_Gfx, (void *) (VRAM + tileOffsetLoad));
    LoadPalette(gIntroCopyright_Pal, palOffset, sizeof(gIntroCopyright_Pal));

    baseTile = (palOffset / 16) << 12;

    for (i = 0; i < 32 * 32; i++)
        ((u16 *) (VRAM + tileOffsetWrite))[i] = baseTile + 1;

    for (i = 0; i < 21; i++)
        ((u16 *) (VRAM + tileOffsetWrite))[7 * 32 + 4 + i] = i + 2 + baseTile;

    for (i = 0; i < 20; i++)
        ((u16 *) (VRAM + tileOffsetWrite))[9 * 32 + 4 + i] = i + 23 + baseTile;

    for (i = 0; i < 23; i++)
        ((u16 *) (VRAM + tileOffsetWrite))[11 * 32 + 4 + i] = i + 43 + baseTile;

    for (i = 0; i < 12; i++)
        ((u16 *) (VRAM + tileOffsetWrite))[13 * 32 + 4 + i] = i + 66 + baseTile;
}

u16 GetLetterMapTile(u8 baseTiles)
{
    u16 out = (baseTiles & 0x3F) + 80;

    if (baseTiles == 0xFF)
        return 1;

    if (baseTiles & (1 << 7))
        out |= 1 << 11;
    if (baseTiles & (1 << 6))
        out |= 1 << 10;

    return out;
}

void DrawLetterMapTiles(const u8 letterMap[], u8 baseX, u8 baseY, u16 offset, u16 palette)
{
    u8 y, x;
    const u16 tileOffset = (palette / 16) << 12;

    for (y = 0; y < 5; y++)
    {
        for (x = 0; x < 3; x++)
            ((u16 *) (VRAM + offset + (baseY + y) * 64))[baseX + x] = tileOffset + GetLetterMapTile(letterMap[y * 3 + x]);
    }
}

static void DrawTheEnd(u16 offset, u16 palette)
{
    u16 pos;
    u16 baseTile = (palette / 16) << 12;

    for (pos = 0; pos < 32 * 32; pos++)
        ((u16 *) (VRAM + offset))[pos] = baseTile + 1;

#if ENGLISH
    DrawLetterMapTiles(sTheEnd_LetterMap_T, 3, 7, offset, palette);
    DrawLetterMapTiles(sTheEnd_LetterMap_H, 7, 7, offset, palette);
    DrawLetterMapTiles(sTheEnd_LetterMap_E, 11, 7, offset, palette);
    DrawLetterMapTiles(sTheEnd_LetterMap_E, 16, 7, offset, palette);
    DrawLetterMapTiles(sTheEnd_LetterMap_N, 20, 7, offset, palette);
    DrawLetterMapTiles(sTheEnd_LetterMap_D, 24, 7, offset, palette);
#elif GERMAN
    DrawLetterMapTiles(sTheEnd_LetterMap_E, 7, 7, offset, palette);
    DrawLetterMapTiles(sTheEnd_LetterMap_N, 11, 7, offset, palette);
    DrawLetterMapTiles(sTheEnd_LetterMap_D, 15, 7, offset, palette);
    DrawLetterMapTiles(sTheEnd_LetterMap_E, 19, 7, offset, palette);
#endif
}

#define sState data[0]

static void SpriteCB_Player(struct Sprite *sprite)
{
    if (gIntroCredits_MovingSceneryState != INTROCRED_SCENERY_NORMAL)
    {
        DestroySprite(sprite);
        return;
    }

    switch (sprite->sState)
    {
    case 0:
        StartSpriteAnimIfDifferent(sprite, 0);
        break;
    case 1:
        StartSpriteAnimIfDifferent(sprite, 1);
        if (sprite->x > -32)
            sprite->x -= 1;
        break;
    case 2:
        StartSpriteAnimIfDifferent(sprite, 2);
        break;
    case 3:
        StartSpriteAnimIfDifferent(sprite, 3);
        break;
    case 4:
        StartSpriteAnimIfDifferent(sprite, 0);
        if (sprite->x > 120)
            sprite->x -= 1;
        break;
    case 5:
        StartSpriteAnimIfDifferent(sprite, 0);
        if (sprite->x > -32)
            sprite->x -= 1;
        break;
    }
}

static void SpriteCB_Rival(struct Sprite *sprite)
{
    if (gIntroCredits_MovingSceneryState != INTROCRED_SCENERY_NORMAL)
    {
        DestroySprite(sprite);
        return;
    }

    switch (sprite->sState)
    {
    case 0:
        sprite->y2 = 0;
        StartSpriteAnimIfDifferent(sprite, 0);
        break;
    case 1:
        if (sprite->x > 200)
            StartSpriteAnimIfDifferent(sprite, 1);
        else
            StartSpriteAnimIfDifferent(sprite, 2);
        if (sprite->x > -32)
            sprite->x -= 2;
        sprite->y2 = -gIntroCredits_MovingSceneryVOffset;
        break;
    case 2:
        sprite->data[7] += 1;
        StartSpriteAnimIfDifferent(sprite, 0);
        if ((sprite->data[7] & 3) == 0)
            sprite->x += 1;
        break;
    case 3:
        StartSpriteAnimIfDifferent(sprite, 0);
        if (sprite->x > -32)
            sprite->x -= 1;
        break;
    }
}

#define sPosition data[1]

void SpriteCB_CreditsMon(struct Sprite *sprite)
{
    if (gIntroCredits_MovingSceneryState != INTROCRED_SCENERY_NORMAL)
    {
        DestroySprite(sprite);
        return;
    }

    sprite->data[7] += 1;
    switch (sprite->sState)
    {
    case 0:
    default:
        sprite->oam.affineMode = 1;
        sprite->oam.matrixNum = sprite->sPosition;
        sprite->data[2] = 16;
        SetOamMatrix(sprite->sPosition, 0x10000 / sprite->data[2], 0, 0, 0x10000 / sprite->data[2]);
        sprite->invisible = FALSE;
        sprite->sState = 1;
        break;
    case 1:
        if (sprite->data[2] < 256)
        {
            sprite->data[2] += 8;
            SetOamMatrix(sprite->sPosition, 0x10000 / sprite->data[2], 0, 0, 0x10000 / sprite->data[2]);
        }
        else
        {
            sprite->sState++;
        }
        switch (sprite->sPosition)
        {
        case POS_LEFT + 1:
            if ((sprite->data[7] & 3) == 0)
                sprite->y += 1;
            sprite->x -= 2;
            break;
        case POS_CENTER + 1:
            break;
        case POS_RIGHT + 1:
            if ((sprite->data[7] & 3) == 0)
                sprite->y += 1;
            sprite->x += 2;
            break;
        }
        break;
    case 2:
        if (sprite->data[3] != 0)
        {
            sprite->data[3] -= 1;
        }
        else
        {
            REG_BLDCNT = 0xF40;
            REG_BLDALPHA = 0x10;
            sprite->oam.objMode = 1;
            sprite->data[3] = 16;
            sprite->sState++;
        }
        break;
    case 3:
        if (sprite->data[3] != 0)
        {
            int data3;
            vu16 *reg;

            sprite->data[3] -= 1;

            reg = &REG_BLDALPHA;
            data3 = 16 - sprite->data[3];
            *reg = (data3 << 8) + sprite->data[3];
        }
        else
        {
            sprite->invisible = TRUE;
            sprite->sState = 10;
        }
        break;
    case 10:
        REG_BLDCNT = 0;
        REG_BLDALPHA = 0;
        DestroySprite(sprite);
        break;
    }
}

#define sMonSpriteId data[0]

static u8 CreateCreditsMonSprite(u16 species, u16 x, u16 y, u16 position)
{
    u32 personality;
    const u8 *lzPaletteData;
    u8 spriteId;
    u8 bgSpriteId;

    species = NationalPokedexNumToSpecies(species);
    switch (species)
    {
    default:
        personality = 0;
        break;
    case SPECIES_SPINDA:
        personality = gSaveBlock2.pokedex.spindaPersonality;
        break;
    case SPECIES_UNOWN:
        personality = gSaveBlock2.pokedex.unownPersonality;
        break;
    }

    LoadSpecialPokePic(
        &gMonFrontPicTable[species],
        gMonFrontPicCoords[species].size,
        gMonFrontPicCoords[species].y_offset,
        (void *)EWRAM,
        gHallOfFameGfxBufferPtrs[position],
        species,
        personality,
        1
    );

    lzPaletteData = GetMonSpritePalFromSpeciesAndPersonality(species, 0, 0xFFFF);
    LoadCompressedPalette(lzPaletteData, 0x100 + (position * 16), 0x20);
    PrepareHallOfFameMonPicSpriteTemplate(position, position);

    spriteId = CreateSprite(&gCreatingSpriteTemplate, x, y, 0);
    gSprites[spriteId].oam.paletteNum = position;
    gSprites[spriteId].oam.priority = 1;
    gSprites[spriteId].sPosition = position + 1;
    gSprites[spriteId].invisible = TRUE;
    gSprites[spriteId].callback = SpriteCB_CreditsMon;

    bgSpriteId = CreateSprite(&sSpriteTemplate_CreditsMonBg, gSprites[spriteId].x, gSprites[spriteId].y, 1);
    gSprites[bgSpriteId].sMonSpriteId = spriteId;

    StartSpriteAnimIfDifferent(&gSprites[bgSpriteId], position);

    return spriteId;
}

void SpriteCB_CreditsMonBg(struct Sprite *sprite)
{
    if (gSprites[sprite->sMonSpriteId].sState == 10
     || gIntroCredits_MovingSceneryState != INTROCRED_SCENERY_NORMAL)
    {
        DestroySprite(sprite);
        return;
    }

    sprite->invisible = gSprites[sprite->sMonSpriteId].invisible;
    sprite->oam.objMode = gSprites[sprite->sMonSpriteId].oam.objMode;
    sprite->oam.affineMode = gSprites[sprite->sMonSpriteId].oam.affineMode;
    sprite->oam.matrixNum = gSprites[sprite->sMonSpriteId].oam.matrixNum;
    sprite->x = gSprites[sprite->sMonSpriteId].x;
    sprite->y = gSprites[sprite->sMonSpriteId].y;
}

static void DeterminePokemonToShow(void)
{
    struct CreditsData *creditsData = &sCreditsData;
    u16 starter = SpeciesToNationalPokedexNum(GetStarterPokemon(VarGet(VAR_STARTER_MON)));
    u16 seenTypesCount;
    u16 page;
    u16 dexNum;
    u16 j;

    for (dexNum = 1, seenTypesCount = 0; dexNum < 386; dexNum++)
    {
        if (GetSetPokedexFlag(dexNum, 1))
        {
            creditsData->caughtMonIds[seenTypesCount] = dexNum;
            seenTypesCount++;
        }
    }

    for (dexNum = seenTypesCount; dexNum < 386; dexNum++)
        creditsData->caughtMonIds[dexNum] = 0;

    creditsData->numCaughtMon = seenTypesCount;
    if (creditsData->numCaughtMon < NUM_MON_SLIDES)
        creditsData->numMonToShow = seenTypesCount;
    else
        creditsData->numMonToShow = NUM_MON_SLIDES;

    j = 0;
    do
    {
        page = Random() % creditsData->numCaughtMon;
        creditsData->monToShow[j] = creditsData->caughtMonIds[page];

        j++;
        creditsData->caughtMonIds[page] = 0;
        creditsData->numCaughtMon--;
        if (page != creditsData->numCaughtMon)
        {
            creditsData->caughtMonIds[page] = creditsData->caughtMonIds[creditsData->numCaughtMon];
            creditsData->caughtMonIds[creditsData->numCaughtMon] = 0;
        }
    }
    while (creditsData->numCaughtMon != 0 && j < NUM_MON_SLIDES);

    if (creditsData->numMonToShow < NUM_MON_SLIDES)
    {
        for (j = creditsData->numMonToShow, page = 0; j < NUM_MON_SLIDES; j++)
        {
            creditsData->monToShow[j] = creditsData->monToShow[page];

            page++;
            if (page == creditsData->numMonToShow)
                page = 0;
        }
        creditsData->monToShow[NUM_MON_SLIDES - 1] = starter;
    }
    else
    {
        for (dexNum = 0; creditsData->monToShow[dexNum] != starter && dexNum < NUM_MON_SLIDES; dexNum++);

        if (dexNum < creditsData->numMonToShow - 1)
        {
            creditsData->monToShow[dexNum] = creditsData->monToShow[NUM_MON_SLIDES - 1];
            creditsData->monToShow[NUM_MON_SLIDES - 1] = starter;
        }
        else
        {
            creditsData->monToShow[NUM_MON_SLIDES - 1] = starter;
        }
    }
    creditsData->numMonToShow = NUM_MON_SLIDES;
}
