#include "global.h"
#include "constants/rgb.h"
#include "sprite.h"
#include "credits.h"
#include "data.h"
#include "decompress.h"
#include "event_data.h"
#include "graphics.h"
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
    PAGE_MAIN_GRAHPICS_DESIGNER,
    PAGE_POKEMON_GRAHPIC_DESIGNERS_1,
    PAGE_POKEMON_GRAHPIC_DESIGNERS_2,
    PAGE_POKEMON_GRAHPIC_DESIGNERS_3,
    PAGE_POKEMON_DESIGNERS_1,
    PAGE_POKEMON_DESIGNERS_2,
    PAGE_MUSIC_COMPOSITION,
    PAGE_SOUND_EFFECTS,
    PAGE_GAME_DESIGNERS_1,
    PAGE_GAME_DESIGNERS_2,
    PAGE_GAME_DESIGNERS_3,
    PAGE_SCENARIO_PLOT,
    PAGE_SCENARIO,
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
#define UNK_DEFINE_45 (0x45)
#define UNK_DEFINE_82 (0x82)
#define UNK_DEF_1F3 (499)
#elif GERMAN
#define NUM_MON_SLIDES 65
#define LAST_PAGE (PAGE_NOE_TESTING)
#define UNK_DEFINE_45 (8)
#define UNK_DEFINE_82 (0x8D)
#define UNK_DEF_1F3 (554)
#endif

#define COLOR_DARK_GREEN 0x1967
#define COLOR_LIGHT_GREEN 0x328D

enum
{
    TDA_0 = 0,
    TDA_TASK_C_ID = 1,
    TDA_TASK_E_ID = 2,
    TDA_TASK_D_ID = 3,
    TDA_4 = 4,
    TDA_PLAYER_CYCLIST = 5,
    TDA_RIVAL_CYCLIST = 6,
    TDA_7 = 7, // Has something to do with the bike scene
    TDA_11 = 11, // Gets set depending on whether the bike or the grass scene should be shown
    TDA_12 = 12,
    TDA_13 = 13,
    TDA_14 = 14,
    TDA_TASK_B_ID = 15,

    // Appears to be responsible for text
    TDB_0 = 0,
    TDB_TASK_A_ID = 1,
    TDB_CURRENT_PAGE = 2,
    TDB_3 = 3,

    TDC_0 = 0,
    TDC_1 = 1,
    TDC_2 = 2,
    TDC_3 = 3,
    TDC_4 = 4,
    TDC_5 = 5,

    TDD_STATE = 0,
    TDD_TASK_A_ID = 1,
    TDD_2 = 2,
    TDD_3 = 3,

    TDE_0 = 0,
    TDE_1 = 1,
    TDE_TASK_A_ID = 2,
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
    u8 unk;
    const u8 *text;
};



static EWRAM_DATA s16 sUnkVar = 0;
static EWRAM_DATA u16 sSavedTaskId = 0; // TASK A
EWRAM_DATA bool8 gHasHallOfFameRecords = 0;
static EWRAM_DATA bool8 sUsedSpeedUp = 0;


// data/hall_of_fame
extern void *gUnknown_0840B5A0[];

// data/credits
static const u16 sCredits_Pal[] = INCBIN_U16("graphics/credits/credits.gbapal");
const u8 gUnknown_0840B7FC[] = INCBIN_U8("graphics/credits/ampersand.4bpp");

static void SpriteCB_CreditsMonBg(struct Sprite *sprite);

static const u8 sTheEnd_LetterMap_T[] =
{
    0,    1, 0,
    0xFF, 1, 0xFF,
    0xFF, 1, 0xFF,
    0xFF, 1, 0xFF,
    0xFF, 1, 0xFF,
};

static const u8 sTheEnd_LetterMap_H[] =
{
    1, 0xFF, 1,
    1, 0xFF, 1,
    1, 2,    1,
    1, 0xFF, 1,
    1, 0xFF, 1,
};

static const u8 sTheEnd_LetterMap_E[] =
{
    1, 0, 0,
    1, 0xFF, 0xFF,
    1, 2,    2,
    1, 0xFF, 0xFF,
    1, 0x80, 0x80,
};

static const u8 sTheEnd_LetterMap_N[] =
{
    1, 3, 1,
    1, 4, 1,
    1, 5, 1,
    1, 0xC4, 1,
    1, 0xC3, 1,
};

static const u8 sTheEnd_LetterMap_D[] =
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
#include "data/credits.h"
#endif

static const u8 sMonSpritePos[][2] =
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

static const struct SpriteSheet sSpriteSheet_MonBg[] = {{gDecompressionBuffer, 0x1800, 1001}, {0}};
static const struct SpritePalette sSpritePalette_MonBg[] = {{(u16 *)(gDecompressionBuffer + 0x1800), 1001}, {0}};

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
    .tileTag = 1001,
    .paletteTag = 1001,
    .oam = &sOamData_MonBg,
    .anims = sAnims_MonBg,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_CreditsMonBg,
};

// graphics
extern u8 gCreditsCopyrightEnd_Gfx[];

static void Task_WaitPaletteFade(u8 taskId);
static void Task_CreditsMain(u8 taskId);
static void Task_ReadyBikeScene(u8 taskId);
static void Task_SetBikeScene(u8 taskId);
static void Task_ReadyShowMons(u8 taskId);
static void Task_LoadShowMons(u8 taskId);
static void Task_CreditsTheEnd1(u8 taskId);
static void Task_CreditsTheEnd2(u8 taskId);
static void Task_CreditsTheEnd3(u8 taskId);
static void Task_CreditsTheEnd4(u8 taskId);
static void Task_CreditsTheEnd5(u8 taskId);
static void Task_CreditsTheEnd6(u8 taskId);
static void Task_CreditsSoftReset(u8 taskId);
static void ResetGpuAndVram(void);
static void Task_UpdatePage(u8 taskId);
static u8 CheckChangeScene(u8 page, u8 taskId);
static void Task_ShowMons(u8 taskId);
static bool8 LoadBikeScene(u8 scene, u8 taskId);
static void ResetCreditsTasks(u8 taskId);
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
    u8 taskId;
    s16 bikeTaskId;
    u8 pageTaskId;
    u16 savedIme;
    struct CreditsData *pCreditsData;

    ResetGpuAndVram();
    SetVBlankCallback(NULL);
    ResetPaletteFade();
    ResetTasks();

    taskId = CreateTask(Task_WaitPaletteFade, 0);

    gTasks[taskId].data[TDA_4] = 0;
    gTasks[taskId].data[TDA_7] = 0;
    gTasks[taskId].data[TDA_11] = 0;
    gTasks[taskId].data[TDA_13] = 1;

    while (TRUE)
    {
        if (LoadBikeScene(0, taskId))
            break;
    }

    bikeTaskId = gTasks[taskId].data[TDA_TASK_C_ID];
    gTasks[bikeTaskId].data[TDC_0] = 40;

    Text_LoadWindowTemplate(&gWindowTemplate_81E7208);
    InitMenuWindow(&gWindowTemplate_81E7208);
    LoadPalette(&sCredits_Pal, 0x80, sizeof(sCredits_Pal));

    CpuCopy16(&gUnknown_0840B7FC, (void *)(VRAM + 0xBEE0), sizeof(gUnknown_0840B7FC));

    REG_BG0VOFS = 0xFFFC;

    pageTaskId = CreateTask(Task_UpdatePage, 0);

    gTasks[pageTaskId].data[TDB_TASK_A_ID] = taskId;
    gTasks[taskId].data[TDA_TASK_B_ID] = pageTaskId;

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

    sSavedTaskId = taskId;
}

static void Task_WaitPaletteFade(u8 taskId)
{
    if (!gPaletteFade.active)
        gTasks[taskId].func = Task_CreditsMain;
}

static void Task_CreditsMain(u8 taskId)
{
    u16 mode;

    if (gTasks[taskId].data[TDA_4])
    {
        s16 bikeTaskId;

        bikeTaskId = gTasks[taskId].data[TDA_TASK_C_ID];
        gTasks[bikeTaskId].data[TDC_0] = 30;

        gTasks[taskId].data[TDA_12] = 0x100;
        gTasks[taskId].func = Task_CreditsTheEnd1;
        return;
    }

    sUnkVar = 0;
    mode = gTasks[taskId].data[TDA_11];

    if (gTasks[taskId].data[TDA_11] == 1)
    {
        gTasks[taskId].data[TDA_13] = mode;
        gTasks[taskId].data[TDA_11] = 0;
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        gTasks[taskId].func = Task_ReadyBikeScene;
    }
    else if (gTasks[taskId].data[TDA_11] == 2)
    {
        gTasks[taskId].data[TDA_13] = mode;
        gTasks[taskId].data[TDA_11] = 0;
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        gTasks[taskId].func = Task_ReadyShowMons;
    }
}

static void Task_ReadyBikeScene(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        REG_DISPCNT = 0;
        ResetCreditsTasks(taskId);
        gTasks[taskId].func = Task_SetBikeScene;
    }
}

static void Task_SetBikeScene(u8 taskId)
{
    u16 backup;

    SetVBlankCallback(NULL);

    if (LoadBikeScene(gTasks[taskId].data[TDA_7], taskId))
    {
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));

        backup = REG_IME;
        REG_IME = 0;
        REG_IE |= INTR_FLAG_VBLANK;
        REG_IME = backup;
        REG_DISPSTAT |= DISPSTAT_VBLANK_INTR;

        SetVBlankCallback(VBlankCB_Credits);
        gTasks[taskId].func = Task_WaitPaletteFade;
    }
}

static void Task_ReadyShowMons(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        REG_DISPCNT = 0;
        ResetCreditsTasks(taskId);
        gTasks[taskId].func = Task_LoadShowMons;
    }
}

static void Task_LoadShowMons(u8 taskId)
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

        ((u16 *)(gDecompressionBuffer + 0x1800))[0] = RGB( 0,  0,  0); // black
        ((u16 *)(gDecompressionBuffer + 0x1800))[1] = RGB(31, 31, 20); // light yellow
        ((u16 *)(gDecompressionBuffer + 0x1800))[2] = RGB(31, 20, 20); // light red
        ((u16 *)(gDecompressionBuffer + 0x1800))[3] = RGB(20, 20, 31); // light blue

        LoadSpriteSheet(sSpriteSheet_MonBg);
        LoadSpritePalette(sSpritePalette_MonBg);

        gMain.state += 1;
        break;
    }
    case 1:
        gTasks[taskId].data[TDA_TASK_D_ID] = CreateTask(Task_ShowMons, 0);
        gTasks[gTasks[taskId].data[TDA_TASK_D_ID]].data[TDD_STATE] = 1;
        gTasks[gTasks[taskId].data[TDA_TASK_D_ID]].data[TDD_TASK_A_ID] = taskId;
        gTasks[gTasks[taskId].data[TDA_TASK_D_ID]].data[TDD_2] = gTasks[taskId].data[TDA_7];

        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        REG_BG3HOFS = 0;
        REG_BG3VOFS = 32;
        REG_BG3CNT = BGCNT_PRIORITY(3) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(7) | BGCNT_16COLOR | BGCNT_TXT256x256;
        REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG3_ON | DISPCNT_OBJ_ON;

        gMain.state = 0;
        gIntroCredits_MovingSceneryState = 0;
        gTasks[taskId].func = Task_WaitPaletteFade;
        break;
    }
}

static void Task_CreditsTheEnd1(u8 taskId)
{
    if (gTasks[taskId].data[TDA_12])
    {
        gTasks[taskId].data[TDA_12] -= 1;
        return;
    }

    BeginNormalPaletteFade(0xFFFFFFFF, 12, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = Task_CreditsTheEnd2;
}

static void Task_CreditsTheEnd2(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        ResetCreditsTasks(taskId);
        gTasks[taskId].func = Task_CreditsTheEnd3;
    }
}

static void Task_CreditsTheEnd3(u8 taskId)
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

    gTasks[taskId].data[TDA_0] = 0x100;
    gTasks[taskId].func = Task_CreditsTheEnd4;
}

static void Task_CreditsTheEnd4(u8 taskId)
{
    if (gTasks[taskId].data[TDA_0])
    {
        gTasks[taskId].data[TDA_0] -= 1;
        return;
    }

    BeginNormalPaletteFade(0xFFFFFFFF, 6, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = Task_CreditsTheEnd5;
}

static void Task_CreditsTheEnd5(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        DrawTheEnd(0x3800, 0);

        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 0, RGB(0, 0, 0));
        gTasks[taskId].data[TDA_0] = 7200;
        gTasks[taskId].func = Task_CreditsTheEnd6;
    }
}

static void Task_CreditsTheEnd6(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        if (gTasks[taskId].data[TDA_0] == 0)
        {
            FadeOutBGM(4);
            BeginNormalPaletteFade(0xFFFFFFFF, 8, 0, 16, RGB_WHITEALPHA);
            gTasks[taskId].func = Task_CreditsSoftReset;
            return;
        }

        if (gMain.newKeys)
        {
            FadeOutBGM(4);
            BeginNormalPaletteFade(0xFFFFFFFF, 8, 0, 16, RGB_WHITEALPHA);
            gTasks[taskId].func = Task_CreditsSoftReset;
            return;
        }

        if (gTasks[taskId].data[TDA_0] == 7144)
        {
            FadeOutBGM(8);
        }

        if (gTasks[taskId].data[TDA_0] == 6840)
            m4aSongNumStart(MUS_END);

        gTasks[taskId].data[TDA_0] -= 1;
    }
}

static void Task_CreditsSoftReset(u8 taskId)
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

static void Task_UpdatePage(u8 taskId)
{
    u16 i;

    switch (gTasks[taskId].data[TDB_0])
    {
    case 0:
    case 6:
    case 7:
    case 8:
    case 9:
    default:
        if (!gPaletteFade.active)
        {
            gTasks[taskId].data[TDB_0] = 1;
            gTasks[taskId].data[TDB_3] = 0x58;
            gTasks[gTasks[taskId].data[TDB_TASK_A_ID]].data[TDA_14] = 0;
            sUnkVar = 0;
        }
        return;
    case 1:
        if (gTasks[taskId].data[TDB_3] != 0)
        {
            gTasks[taskId].data[TDB_3] -= 1;
            return;
        }
        gTasks[taskId].data[TDB_0] += 1;
        return;
    case 2:
        REG_DISPCNT &= ~DISPCNT_BG0_ON;
        if (gTasks[gTasks[taskId].data[TDB_TASK_A_ID]].func == Task_CreditsMain)
        {
            if (gTasks[taskId].data[TDB_CURRENT_PAGE] < PAGE_COUNT)
            {
                for (i = 0; i < 5; i++)
                    MenuPrint_Centered(sCreditsEntryPointerTable[gTasks[taskId].data[TDB_CURRENT_PAGE]][i]->text, 0, 9 + i * 2, 240);

                gTasks[taskId].data[TDB_CURRENT_PAGE] += 1;
                gTasks[taskId].data[TDB_0] += 1;

                gTasks[gTasks[taskId].data[TDB_TASK_A_ID]].data[TDA_14] = 1;

                if (gTasks[gTasks[taskId].data[TDB_TASK_A_ID]].data[TDA_13] == 1)
                    BeginNormalPaletteFade(0x300, 0, 16, 0, COLOR_LIGHT_GREEN);
                else
                    BeginNormalPaletteFade(0x300, 0, 16, 0, COLOR_DARK_GREEN);
                return;
            }
            gTasks[taskId].data[TDB_0] = 10;
            return;
        }
        gTasks[gTasks[taskId].data[TDB_TASK_A_ID]].data[TDA_14] = 0;
        return;
    case 3:
        REG_DISPCNT |= DISPCNT_BG0_ON;
        if (!gPaletteFade.active)
        {
            gTasks[taskId].data[TDB_3] = UNK_DEFINE_82;
            gTasks[taskId].data[TDB_0] += 1;
        }
        return;
    case 4:
        if (gTasks[taskId].data[TDB_3] != 0)
        {
            gTasks[taskId].data[TDB_3] -= 1;
            return;
        }

        if (CheckChangeScene((u8)gTasks[taskId].data[TDB_CURRENT_PAGE], (u8)gTasks[taskId].data[TDB_TASK_A_ID]))
        {
            gTasks[taskId].data[TDB_0] += 1;
            return;
        }
        gTasks[taskId].data[TDB_0] += 1;
        if (gTasks[gTasks[taskId].data[TDB_TASK_A_ID]].data[TDA_13] == 1)
            BeginNormalPaletteFade(0x300, 0, 0, 16, COLOR_LIGHT_GREEN);
        else
            BeginNormalPaletteFade(0x300, 0, 0, 16, COLOR_DARK_GREEN);
        return;
    case 5:
        if (!gPaletteFade.active)
        {
            Menu_EraseWindowRect(0, 9, 29, 19);
            gTasks[taskId].data[TDB_0] = 2;
        }
        return;

    case 10:
        gTasks[gTasks[taskId].data[TDB_TASK_A_ID]].data[TDA_4] = 1;
        DestroyTask(taskId);
        return;
    }
}

static u8 CheckChangeScene(u8 page, u8 taskId)
{
    // Starts with bike + ocean + morning

    if (page == PAGE_PROGRAMMERS_1)
    {
        // Grass patch
        gTasks[taskId].data[TDA_11] = 2;
    }

    if (page == PAGE_POKEMON_GRAHPIC_DESIGNERS_3)
    {
        // Bike + ocean + sunset
        gTasks[taskId].data[TDA_7] = 1;
        gTasks[taskId].data[TDA_11] = 1;
    }

    if (page == PAGE_GAME_DESIGNERS_2)
    {
        // Grass patch
        gTasks[taskId].data[TDA_11] = 2;
    }

    if (page == PAGE_MAP_DATA_DESIGNERS)
    {
        // Bike + forest + sunset
        gTasks[taskId].data[TDA_7] = 2;
        gTasks[taskId].data[TDA_11] = 1;
    }

    if (page == PAGE_SPECIAL_THANKS_1)
    {
        // Grass patch
        gTasks[taskId].data[TDA_11] = 2;
    }

    if (page == PAGE_TASK_MANAGERS)
    {
        // Bike + forest + sunset
        gTasks[taskId].data[TDA_7] = 3;
        gTasks[taskId].data[TDA_11] = 1;
    }

    if (page == PAGE_TRANSLATION_COORDINATOR)
    {
        // Grass patch
        gTasks[taskId].data[TDA_11] = 2;
    }

    if (page == LAST_PAGE)
    {
        // Bike + town + night
        gTasks[taskId].data[TDA_7] = 4;
        gTasks[taskId].data[TDA_11] = 1;
    }

    if (gTasks[taskId].data[TDA_11] != 0)
    {
        // Returns true if changed?
        return TRUE;
    }

    return FALSE;
}

static void Task_ShowMons(u8 taskId)
{
    struct CreditsData *r6 = &sCreditsData;
    u8 spriteId;

    switch (gTasks[taskId].data[TDD_STATE])
    {
    case 0:
        break;
    case 1:
        if (r6->nextImgPos == 0 && gTasks[gTasks[taskId].data[TDD_TASK_A_ID]].data[TDA_14] == 0)
            break;
        gTasks[gTasks[taskId].data[TDD_TASK_A_ID]].data[TDA_14] = 0;
        gTasks[taskId].data[TDD_STATE]++;
        break;
    case 2:
        if (r6->imgCounter == NUM_MON_SLIDES || gTasks[gTasks[taskId].data[TDD_TASK_A_ID]].func != Task_CreditsMain)
            break;
        spriteId = CreateCreditsMonSprite(r6->monToShow[r6->currShownMon], sMonSpritePos[r6->nextImgPos][0], sMonSpritePos[r6->nextImgPos][1], r6->nextImgPos);
        if (r6->currShownMon < r6->numMonToShow - 1)
        {
            r6->currShownMon++;
            gSprites[spriteId].data[3] = 50;
        }
        else
        {
            r6->currShownMon = 0;
            gSprites[spriteId].data[3] = 512;
        }
        r6->imgCounter++;
        if (r6->nextImgPos == 2)
            r6->nextImgPos = 0;
        else
            r6->nextImgPos++;
        gTasks[taskId].data[TDD_3] = 50;
        gTasks[taskId].data[TDD_STATE]++;
        break;
    case 3:
        if (gTasks[taskId].data[TDD_3] != 0)
            gTasks[taskId].data[TDD_3]--;
        else
            gTasks[taskId].data[TDD_STATE] = 1;
        break;
    }
}

static void Task_BikeScene(u8 taskId)
{
    switch (gTasks[taskId].data[TDC_0])
    {
    case 0:
        gIntroCredits_MovingSceneryVOffset = Sin((gTasks[taskId].data[TDC_5] >> 1) & 0x7F, 12);
        gTasks[taskId].data[TDC_5]++;
        break;
    case 1:
        if (gIntroCredits_MovingSceneryVOffset != 0)
        {
            gIntroCredits_MovingSceneryVOffset = Sin((gTasks[taskId].data[TDC_5] >> 1) & 0x7F, 12);
            gTasks[taskId].data[TDC_5]++;
        }
        else
        {
            gSprites[gTasks[taskId].data[TDC_2]].data[0] = 2;
            gTasks[taskId].data[TDC_5] = 0;
            gTasks[taskId].data[TDC_0]++;
        }
        break;
    case 2:
        if (gTasks[taskId].data[TDC_5] < 64)
        {
            gTasks[taskId].data[TDC_5]++;
            gIntroCredits_MovingSceneryVOffset = Sin(gTasks[taskId].data[TDC_5] & 0x7F, 20);
        }
        else
        {
            gTasks[taskId].data[TDC_0]++;
        }
        break;
    case 3:
        gSprites[gTasks[taskId].data[TDC_2]].data[0] = 3;
        gSprites[gTasks[taskId].data[TDC_3]].data[0] = 1;
        gTasks[taskId].data[TDC_4] = 120;
        gTasks[taskId].data[TDC_0]++;
        break;
    case 4:
        if (gTasks[taskId].data[TDC_4] != 0)
        {
            gTasks[taskId].data[TDC_4]--;
        }
        else
        {
            gTasks[taskId].data[TDC_5] = 64;
            gTasks[taskId].data[TDC_0]++;
        }
        break;
    case 5:
        if (gTasks[taskId].data[TDC_5] > 0)
        {
            gTasks[taskId].data[TDC_5]--;
            gIntroCredits_MovingSceneryVOffset = Sin(gTasks[taskId].data[TDC_5] & 0x7F, 20);
        }
        else
        {
            gSprites[gTasks[taskId].data[TDC_2]].data[0] = 1;
            gTasks[taskId].data[TDC_0]++;
        }
        break;
    case 6:
        gTasks[taskId].data[TDC_0] = 50;
        break;
    case 10:
        gSprites[gTasks[taskId].data[TDC_3]].data[0] = 2;
        gTasks[taskId].data[TDC_0] = 50;
        break;
    case 20:
        gSprites[gTasks[taskId].data[TDC_2]].data[0] = 4;
        gTasks[taskId].data[TDC_0] = 50;
        break;
    case 30:
        gSprites[gTasks[taskId].data[TDC_2]].data[0] = 5;
        gSprites[gTasks[taskId].data[TDC_3]].data[0] = 3;
        gTasks[taskId].data[TDC_0] = 50;
        break;
    case 50:
        gTasks[taskId].data[TDC_0] = 0;
        break;
    }
}

static void Task_CycleSceneryPalette(u8 taskId)
{
    s16 bikeTaskId;

    switch (gTasks[taskId].data[TDE_0])
    {
    default:
    case 0:
        if (gTasks[taskId].data[TDE_1] != 0x7FFF)
        {

            if (gTasks[gTasks[gTasks[taskId].data[TDE_TASK_A_ID]].data[TDA_TASK_B_ID]].data[TDB_CURRENT_PAGE] == PAGE_ART_DIRECTOR)
            {
                gTasks[gTasks[gTasks[taskId].data[TDE_TASK_A_ID]].data[TDA_TASK_C_ID]].data[TDC_0] = 20;
                gTasks[taskId].data[TDE_1] = 0x7FFF;
            }
        }
        CycleSceneryPalette(0);
        break;
    case 1:
        CycleSceneryPalette(0);
        break;
    case 2:
        if (gTasks[taskId].data[TDE_1] != 0x7FFF)
        {
            bikeTaskId = gTasks[gTasks[taskId].data[TDE_TASK_A_ID]].data[TDA_TASK_C_ID];

            // Floor to multiple of 128
            if ((gTasks[bikeTaskId].data[TDC_5] & -128) == 640)
            {
                gTasks[bikeTaskId].data[TDC_0] = 1;
                gTasks[taskId].data[TDE_1] = 0x7FFF;
            }
        }
        CycleSceneryPalette(1);
        break;
    case 3:
        if (gTasks[taskId].data[TDE_1] != 0x7FFF)
        {

            if (gTasks[taskId].data[TDE_1] == UNK_DEF_1F3)
            {
                gTasks[gTasks[gTasks[taskId].data[TDE_TASK_A_ID]].data[TDA_TASK_C_ID]].data[TDC_0] = 10;
                gTasks[taskId].data[TDE_1] = 0x7FFF;
            }
            else
            {
                gTasks[taskId].data[TDE_1] += 1;
            }
        }
        CycleSceneryPalette(1);
        break;
    case 4:
        CycleSceneryPalette(2);
        break;
    }
}

static void SetBikeScene(u8 scene, u8 taskId)
{
    switch (scene)
    {
    case 0:
        gSprites[gTasks[taskId].data[TDA_PLAYER_CYCLIST]].invisible = FALSE;
        gSprites[gTasks[taskId].data[TDA_RIVAL_CYCLIST]].invisible = FALSE;
        gSprites[gTasks[taskId].data[TDA_PLAYER_CYCLIST]].x = 272;
        gSprites[gTasks[taskId].data[TDA_RIVAL_CYCLIST]].x = 272;
        gSprites[gTasks[taskId].data[TDA_PLAYER_CYCLIST]].y = 46;
        gSprites[gTasks[taskId].data[TDA_RIVAL_CYCLIST]].y = 46;
        gSprites[gTasks[taskId].data[TDA_PLAYER_CYCLIST]].data[0] = 0;
        gSprites[gTasks[taskId].data[TDA_RIVAL_CYCLIST]].data[0] = 0;
        gTasks[taskId].data[TDA_0] = CreateBicycleBgAnimationTask(0, 0x2000, 0x20, 8);
        break;
    case 1:
        gSprites[gTasks[taskId].data[TDA_PLAYER_CYCLIST]].invisible = FALSE;
        gSprites[gTasks[taskId].data[TDA_RIVAL_CYCLIST]].invisible = FALSE;
        gSprites[gTasks[taskId].data[TDA_PLAYER_CYCLIST]].x = 120;
        gSprites[gTasks[taskId].data[TDA_RIVAL_CYCLIST]].x = 272;
        gSprites[gTasks[taskId].data[TDA_PLAYER_CYCLIST]].y = 46;
        gSprites[gTasks[taskId].data[TDA_RIVAL_CYCLIST]].y = 46;
        gSprites[gTasks[taskId].data[TDA_PLAYER_CYCLIST]].data[0] = 0;
        gSprites[gTasks[taskId].data[TDA_RIVAL_CYCLIST]].data[0] = 0;
        gTasks[taskId].data[TDA_0] = CreateBicycleBgAnimationTask(0, 0x2000, 0x20, 8);
        break;
    case 2:
        gSprites[gTasks[taskId].data[TDA_PLAYER_CYCLIST]].invisible = FALSE;
        gSprites[gTasks[taskId].data[TDA_RIVAL_CYCLIST]].invisible = FALSE;
        gSprites[gTasks[taskId].data[TDA_PLAYER_CYCLIST]].x = 120;
        gSprites[gTasks[taskId].data[TDA_RIVAL_CYCLIST]].x = 272;
        gSprites[gTasks[taskId].data[TDA_PLAYER_CYCLIST]].y = 46;
        gSprites[gTasks[taskId].data[TDA_RIVAL_CYCLIST]].y = 46;
        gSprites[gTasks[taskId].data[TDA_PLAYER_CYCLIST]].data[0] = 0;
        gSprites[gTasks[taskId].data[TDA_RIVAL_CYCLIST]].data[0] = 0;
        gTasks[taskId].data[TDA_0] = CreateBicycleBgAnimationTask(1, 0x2000, 0x200, 8);
        break;
    case 3:
        gSprites[gTasks[taskId].data[TDA_PLAYER_CYCLIST]].invisible = FALSE;
        gSprites[gTasks[taskId].data[TDA_RIVAL_CYCLIST]].invisible = FALSE;
        gSprites[gTasks[taskId].data[TDA_PLAYER_CYCLIST]].x = 120;
        gSprites[gTasks[taskId].data[TDA_RIVAL_CYCLIST]].x = -32;
        gSprites[gTasks[taskId].data[TDA_PLAYER_CYCLIST]].y = 46;
        gSprites[gTasks[taskId].data[TDA_RIVAL_CYCLIST]].y = 46;
        gSprites[gTasks[taskId].data[TDA_PLAYER_CYCLIST]].data[0] = 0;
        gSprites[gTasks[taskId].data[TDA_RIVAL_CYCLIST]].data[0] = 0;
        gTasks[taskId].data[TDA_0] = CreateBicycleBgAnimationTask(1, 0x2000, 0x200, 8);
        break;
    case 4:
        gSprites[gTasks[taskId].data[TDA_PLAYER_CYCLIST]].invisible = FALSE;
        gSprites[gTasks[taskId].data[TDA_RIVAL_CYCLIST]].invisible = FALSE;
        gSprites[gTasks[taskId].data[TDA_PLAYER_CYCLIST]].x = 88;
        gSprites[gTasks[taskId].data[TDA_RIVAL_CYCLIST]].x = 152;
        gSprites[gTasks[taskId].data[TDA_PLAYER_CYCLIST]].y = 46;
        gSprites[gTasks[taskId].data[TDA_RIVAL_CYCLIST]].y = 46;
        gSprites[gTasks[taskId].data[TDA_PLAYER_CYCLIST]].data[0] = 0;
        gSprites[gTasks[taskId].data[TDA_RIVAL_CYCLIST]].data[0] = 0;
        gTasks[taskId].data[TDA_0] = CreateBicycleBgAnimationTask(2, 0x2000, 0x200, 8);
        break;
    }

    gTasks[taskId].data[TDA_TASK_E_ID] = CreateTask(Task_CycleSceneryPalette, 0);
    gTasks[gTasks[taskId].data[TDA_TASK_E_ID]].data[TDE_0] = scene;
    gTasks[gTasks[taskId].data[TDA_TASK_E_ID]].data[TDE_1] = 0;
    gTasks[gTasks[taskId].data[TDA_TASK_E_ID]].data[TDE_TASK_A_ID] = taskId;

    gTasks[taskId].data[TDA_TASK_C_ID] = CreateTask(Task_BikeScene, 0);
    gTasks[gTasks[taskId].data[TDA_TASK_C_ID]].data[TDC_0] = 0;
    gTasks[gTasks[taskId].data[TDA_TASK_C_ID]].data[TDC_1] = taskId;
    gTasks[gTasks[taskId].data[TDA_TASK_C_ID]].data[TDC_2] = gTasks[taskId].data[TDA_PLAYER_CYCLIST];
    gTasks[gTasks[taskId].data[TDA_TASK_C_ID]].data[TDC_3] = gTasks[taskId].data[TDA_RIVAL_CYCLIST];
    gTasks[gTasks[taskId].data[TDA_TASK_C_ID]].data[TDC_4] = 0;

    if (scene == 2)
        gTasks[gTasks[taskId].data[TDA_TASK_C_ID]].data[TDC_5] = UNK_DEFINE_45;
}

static bool8 LoadBikeScene(u8 scene, u8 taskId)
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
            LoadCompressedSpriteSheet(gSpriteSheet_CreditsBrendan);
            LoadCompressedSpriteSheet(gSpriteSheet_CreditsRivalMay);
            LoadCompressedSpriteSheet(gSpriteSheet_IntroBicycle);
            LoadSpritePalettes(gSpritePalettes_Credits);

            spriteId = CreateIntroBrendanSprite(120, 46);
            gTasks[taskId].data[TDA_PLAYER_CYCLIST] = spriteId;
            gSprites[spriteId].callback = SpriteCB_Player;
            gSprites[spriteId].anims = sAnims_Player;

            spriteId = CreateIntroMaySprite(272, 46);
            gTasks[taskId].data[TDA_RIVAL_CYCLIST] = spriteId;
            gSprites[spriteId].callback = SpriteCB_Rival;
            gSprites[spriteId].anims = sAnims_Rival;
        }
        else
        {
            LoadCompressedSpriteSheet(gSpriteSheet_CreditsMay);
            LoadCompressedSpriteSheet(gSpriteSheet_CreditsRivalBrendan);
            LoadCompressedSpriteSheet(gSpriteSheet_IntroBicycle);
            LoadSpritePalettes(gSpritePalettes_Credits);

            spriteId = CreateIntroMaySprite(120, 46);
            gTasks[taskId].data[TDA_PLAYER_CYCLIST] = spriteId;
            gSprites[spriteId].callback = SpriteCB_Player;
            gSprites[spriteId].anims = sAnims_Player;

            spriteId = CreateIntroBrendanSprite(272, 46);
            gTasks[taskId].data[TDA_RIVAL_CYCLIST] = spriteId;
            gSprites[spriteId].callback = SpriteCB_Rival;
            gSprites[spriteId].anims = sAnims_Rival;
        };
        gMain.state += 1;
        break;
    case 3:
        SetBikeScene(scene, taskId);
        SetCreditsSceneBgCnt(scene);
        gMain.state = 0;
        return TRUE;
    }
    return FALSE;
}

static void ResetCreditsTasks(u8 taskId)
{
    if (gTasks[taskId].data[TDA_0] != 0)
    {
        DestroyTask(gTasks[taskId].data[TDA_0]);
        gTasks[taskId].data[TDA_0] = 0;
    }

    if (gTasks[taskId].data[TDA_TASK_C_ID] != 0)
    {
        DestroyTask(gTasks[taskId].data[TDA_TASK_C_ID]);
        gTasks[taskId].data[TDA_TASK_C_ID] = 0;
    }

    if (gTasks[taskId].data[TDA_TASK_E_ID] != 0)
    {
        DestroyTask(gTasks[taskId].data[TDA_TASK_E_ID]);
        gTasks[taskId].data[TDA_TASK_E_ID] = 0;
    }

    if (gTasks[taskId].data[TDA_TASK_D_ID] != 0)
    {
        DestroyTask(gTasks[taskId].data[TDA_TASK_D_ID]);
        gTasks[taskId].data[TDA_TASK_D_ID] = 0;
    }

    gIntroCredits_MovingSceneryState = 1;
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

static u16 GetLetterMapTile(u8 baseTiles)
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

static void DrawLetterMapTiles(const u8 baseTiles[], u8 baseX, u8 baseY, u16 offset, u16 palette)
{
    u8 y, x;
    const u16 tileOffset = (palette / 16) << 12;

    for (y = 0; y < 5; y++)
    {
        for (x = 0; x < 3; x++)
            ((u16 *) (VRAM + offset + (baseY + y) * 64))[baseX + x] = tileOffset + GetLetterMapTile(baseTiles[y * 3 + x]);
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

static void SpriteCB_Player(struct Sprite *sprite)
{
    if (gIntroCredits_MovingSceneryState != 0)
    {
        DestroySprite(sprite);
        return;
    }

    switch (sprite->data[0])
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
    if (gIntroCredits_MovingSceneryState != 0)
    {
        DestroySprite(sprite);
        return;
    }

    switch (sprite->data[0])
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

static void SpriteCB_CreditsMon(struct Sprite *sprite)
{
    if (gIntroCredits_MovingSceneryState)
    {
        DestroySprite(sprite);
        return;
    }

    sprite->data[7] += 1;
    switch (sprite->data[0])
    {
    case 0:
    default:
        sprite->oam.affineMode = 1;
        sprite->oam.matrixNum = sprite->data[1];
        sprite->data[2] = 16;
        SetOamMatrix(sprite->data[1], 0x10000 / sprite->data[2], 0, 0, 0x10000 / sprite->data[2]);
        sprite->invisible = FALSE;
        sprite->data[0] = 1;
        break;
    case 1:
        if (sprite->data[2] < 256)
        {
            sprite->data[2] += 8;
            SetOamMatrix(sprite->data[1], 0x10000 / sprite->data[2], 0, 0, 0x10000 / sprite->data[2]);
        }
        else
        {
            sprite->data[0] += 1;
        }
        switch (sprite->data[1])
        {
        case 1:
            if ((sprite->data[7] & 3) == 0)
                sprite->y += 1;
            sprite->x -= 2;
            break;
        case 2:
            break;
        case 3:
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
            sprite->data[0] += 1;
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
            sprite->data[0] = 10;
        }
        break;
    case 10:
        REG_BLDCNT = 0;
        REG_BLDALPHA = 0;
        DestroySprite(sprite);
        break;
    }
}

static u8 CreateCreditsMonSprite(u16 species, u16 x, u16 y, u16 position)
{
    u32 personality;
    const u8 *lzPaletteData;
    u8 monSpriteId;
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
        gUnknown_0840B5A0[position],
        species,
        personality,
        1
    );

    lzPaletteData = GetMonSpritePalFromSpeciesAndPersonality(species, 0, 0xFFFF);
    LoadCompressedPalette(lzPaletteData, 0x100 + (position * 16), 0x20);
    sub_8143648(position, position);

    monSpriteId = CreateSprite(&gMultiuseSpriteTemplate, x, y, 0);
    gSprites[monSpriteId].oam.paletteNum = position;
    gSprites[monSpriteId].oam.priority = 1;
    gSprites[monSpriteId].data[1] = position + 1;
    gSprites[monSpriteId].invisible = TRUE;
    gSprites[monSpriteId].callback = SpriteCB_CreditsMon;

    bgSpriteId = CreateSprite(&sSpriteTemplate_CreditsMonBg, gSprites[monSpriteId].x, gSprites[monSpriteId].y, 1);
    gSprites[bgSpriteId].data[0] = monSpriteId;

    StartSpriteAnimIfDifferent(&gSprites[bgSpriteId], position);

    return monSpriteId;
}

static void SpriteCB_CreditsMonBg(struct Sprite *sprite)
{
    if (gSprites[sprite->data[0]].data[0] == 10
     || gIntroCredits_MovingSceneryState != 0)
    {
        DestroySprite(sprite);
        return;
    }

    sprite->invisible = gSprites[sprite->data[0]].invisible;
    sprite->oam.objMode = gSprites[sprite->data[0]].oam.objMode;
    sprite->oam.affineMode = gSprites[sprite->data[0]].oam.affineMode;
    sprite->oam.matrixNum = gSprites[sprite->data[0]].oam.matrixNum;
    sprite->x = gSprites[sprite->data[0]].x;
    sprite->y = gSprites[sprite->data[0]].y;
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
