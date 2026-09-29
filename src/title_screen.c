#include "global.h"
#include "gba/m4a_internal.h"
#include "title_screen.h"
#include "clear_save_data_menu.h"
#include "decompress.h"
#include "event_data.h"
#include "graphics.h"
#include "intro.h"
#include "m4a.h"
#include "main.h"
#include "main_menu.h"
#include "overworld.h"
#include "palette.h"
#include "reset_rtc_screen.h"
#include "sound.h"
#include "sprite.h"
#include "task.h"
#include "scanline_effect.h"
#include "constants/songs.h"

#if ENGLISH
#define VERSION_BANNER_SHAPE 1
#define VERSION_BANNER_RIGHT_TILEOFFSET 64
#define VERSION_BANNER_BYTES 0x1000
#define VERSION_BANNER_LEFT_X 98
#define VERSION_BANNER_RIGHT_X 162
#define VERSION_BANNER_Y 26
#define VERSION_BANNER_Y_GOAL 66
#define START_BANNER_X DISPLAY_WIDTH / 2
#elif GERMAN
#define VERSION_BANNER_SHAPE 0
#define VERSION_BANNER_RIGHT_TILEOFFSET 128
#define VERSION_BANNER_BYTES 0x2000
#define VERSION_BANNER_LEFT_X 108
#define VERSION_BANNER_RIGHT_X 172
#ifdef SAPPHIRE
#define VERSION_BANNER_Y_GOAL 83
#else
#define VERSION_BANNER_Y_GOAL 84
#endif
#define VERSION_BANNER_Y (VERSION_BANNER_Y_GOAL - 40)
#define START_BANNER_X DISPLAY_WIDTH / 2 - 2
#endif

enum
{
    TAG_VERSION = 1000,
    TAG_PRESS_START_COPYRIGHT,
    TAG_LOGO_SHINE,
};

extern u8 gReservedSpritePaletteCount;
extern struct MusicPlayerInfo gMPlayInfo_BGM;
extern u16 gBattle_BG1_Y;
extern u16 gBattle_BG1_X;

static EWRAM_DATA u8 sTitleScreenTaskId = 0;

static void SpriteCB_VersionBannerLeft(struct Sprite *sprite);
static void SpriteCB_VersionBannerRight(struct Sprite *sprite);
static void SpriteCB_PressStartCopyrightBanner(struct Sprite *sprite);
static void SpriteCB_PokemonLogoShine(struct Sprite *sprite);

#ifdef SAPPHIRE
static const u16 sLegendaryMonPalettes[][16] =
{
    INCBIN_U16("graphics/title_screen/kyogre_dark.gbapal"),
    INCBIN_U16("graphics/title_screen/kyogre_glow.gbapal"),
};
static const u8 sLegendaryMonPixelData[] = INCBIN_U8("graphics/title_screen/kyogre.4bpp.lz");
static const u8 sLegendaryMonTilemap[] = INCBIN_U8("graphics/title_screen/kyogre_map.bin.lz");
static const u8 sBackdropTilemap[] = INCBIN_U8("graphics/title_screen/water_map.bin.lz");
#else
static const u16 sLegendaryMonPalettes[][16] =
{
    INCBIN_U16("graphics/title_screen/groudon_dark.gbapal"),
    INCBIN_U16("graphics/title_screen/groudon_glow.gbapal"),
};
static const u8 sLegendaryMonPixelData[] = INCBIN_U8("graphics/title_screen/groudon.4bpp.lz");
static const u8 sLegendaryMonTilemap[] = INCBIN_U8("graphics/title_screen/groudon_map.bin.lz");
static const u8 sBackdropTilemap[] = INCBIN_U8("graphics/title_screen/lava_map.bin.lz");
#endif
static const u8 sTitleScreenLogoShineGfx[] = INCBIN_U8("graphics/title_screen/logo_shine.4bpp.lz");
const u16 gTitleScreenAlphaBlend[] =
{
    0x10,
    0x110,
    0x210,
    0x310,
    0x410,
    0x510,
    0x610,
    0x710,
    0x810,
    0x910,
    0xA10,
    0xB10,
    0xC10,
    0xD10,
    0xE10,
    0xF10,
    0x100F,
    0x100E,
    0x100D,
    0x100C,
    0x100B,
    0x100A,
    0x1009,
    0x1008,
    0x1007,
    0x1006,
    0x1005,
    0x1004,
    0x1003,
    0x1002,
    0x1001,
    0x1000,
};
static const struct OamData sVersionBannerLeftOamData =
{
    .y = 160,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 1,
    .shape = VERSION_BANNER_SHAPE,
    .x = 0,
    .matrixNum = 0,
    .size = 3,
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};
static const struct OamData sVersionBannerRightOamData =
{
    .y = 160,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 1,
    .shape = VERSION_BANNER_SHAPE,
    .x = 0,
    .matrixNum = 0,
    .size = 3,
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};
static const union AnimCmd sVersionBannerLeftAnimSequence[] =
{
    ANIMCMD_FRAME(0, 30),
    ANIMCMD_END,
};
static const union AnimCmd sVersionBannerRightAnimSequence[] =
{
    ANIMCMD_FRAME(VERSION_BANNER_RIGHT_TILEOFFSET, 30),
    ANIMCMD_END,
};
static const union AnimCmd *const sVersionBannerLeftAnimTable[] =
{
    sVersionBannerLeftAnimSequence,
};
static const union AnimCmd *const sVersionBannerRightAnimTable[] =
{
    sVersionBannerRightAnimSequence,
};
static const struct SpriteTemplate sVersionBannerLeftSpriteTemplate =
{
    .tileTag = TAG_VERSION,
    .paletteTag = TAG_VERSION,
    .oam = &sVersionBannerLeftOamData,
    .anims = sVersionBannerLeftAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_VersionBannerLeft,
};
static const struct SpriteTemplate sVersionBannerRightSpriteTemplate =
{
    .tileTag = TAG_VERSION,
    .paletteTag = TAG_VERSION,
    .oam = &sVersionBannerRightOamData,
    .anims = sVersionBannerRightAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_VersionBannerRight,
};
static const struct CompressedSpriteSheet sSpriteSheet_RubySapphireVersion[] =
{
    {gTitleScreenRubySapphireVersionGfx, VERSION_BANNER_BYTES, TAG_VERSION},
    {NULL},
};
static const struct OamData sOamData_CopyrightBanner =
{
    .y = 160,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 1,
    .x = 0,
    .matrixNum = 0,
    .size = 1,
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};
static const union AnimCmd sAnim_PressStart_0[] =
{
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_END,
};
static const union AnimCmd sAnim_PressStart_1[] =
{
    ANIMCMD_FRAME(4, 4),
    ANIMCMD_END,
};
static const union AnimCmd sAnim_PressStart_2[] =
{
    ANIMCMD_FRAME(8, 4),
    ANIMCMD_END,
};
static const union AnimCmd sAnim_Copyright_0[] =
{
    ANIMCMD_FRAME(12, 4),
    ANIMCMD_END,
};
static const union AnimCmd sAnim_Copyright_1[] =
{
    ANIMCMD_FRAME(16, 4),
    ANIMCMD_END,
};
static const union AnimCmd sAnim_Copyright_2[] =
{
    ANIMCMD_FRAME(20, 4),
    ANIMCMD_END,
};
static const union AnimCmd sAnim_Copyright_3[] =
{
    ANIMCMD_FRAME(24, 4),
    ANIMCMD_END,
};
static const union AnimCmd sAnim_Copyright_4[] =
{
    ANIMCMD_FRAME(28, 4),
    ANIMCMD_END,
};
#if GERMAN
static const union AnimCmd sAnim_PressStart_German_3[] =
{
    ANIMCMD_FRAME(32, 4),
    ANIMCMD_END,
};
static const union AnimCmd sAnim_PressStart_German_4[] =
{
    ANIMCMD_FRAME(36, 4),
    ANIMCMD_END,
};
#endif
static const union AnimCmd *const sStartCopyrightBannerAnimTable[] =
{
    sAnim_PressStart_0,
    sAnim_PressStart_1,
    sAnim_PressStart_2,
    sAnim_Copyright_0,
    sAnim_Copyright_1,
    sAnim_Copyright_2,
    sAnim_Copyright_3,
    sAnim_Copyright_4,
#if GERMAN
    sAnim_PressStart_German_3,
    sAnim_PressStart_German_4,
#endif
};
static const struct SpriteTemplate sStartCopyrightBannerSpriteTemplate =
{
    .tileTag = TAG_PRESS_START_COPYRIGHT,
    .paletteTag = TAG_PRESS_START_COPYRIGHT,
    .oam = &sOamData_CopyrightBanner,
    .anims = sStartCopyrightBannerAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_PressStartCopyrightBanner,
};
static const struct CompressedSpriteSheet sSpriteSheet_PressStart[] =
{
    {gTitleScreenPressStartGfx, 0x520, TAG_PRESS_START_COPYRIGHT},
    {NULL},
};
static const struct SpritePalette sSpritePalette_PressStart[] =
{
    {gTitleScreenPressStartPal, TAG_PRESS_START_COPYRIGHT},
    {NULL},
};
static const struct OamData sPokemonLogoShineOamData =
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
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};
static const union AnimCmd sPokemonLogoShineAnimSequence[] =
{
    ANIMCMD_FRAME(0, 4),
    ANIMCMD_END,
};
static const union AnimCmd *const sPokemonLogoShineAnimTable[] =
{
    sPokemonLogoShineAnimSequence,
};
static const struct SpriteTemplate sPokemonLogoShineSpriteTemplate =
{
    .tileTag = TAG_LOGO_SHINE,
    .paletteTag = TAG_PRESS_START_COPYRIGHT,
    .oam = &sPokemonLogoShineOamData,
    .anims = sPokemonLogoShineAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_PokemonLogoShine,
};
static const struct CompressedSpriteSheet sPokemonLogoShineSpriteSheet[] =
{
    {sTitleScreenLogoShineGfx, 0x800, TAG_LOGO_SHINE},
    {NULL},
};

#define _RGB(r, g, b) ((((b) & 31) << 10) + (((g) & 31) << 5) + ((r) & 31))

#ifdef SAPPHIRE
//Red Kyogre markings
#define LEGENDARY_MARKING_COLOR(c) RGB((c), 0, 0)
#else
//Blue Groundon markings
#define LEGENDARY_MARKING_COLOR(c) RGB(0, 0, (c))
#endif

#if defined(GERMAN) && defined(SAPPHIRE)
#define TITLE_SCREEN_BACKGROUND_COLOR_PLTT_OFFSET 9
#elif defined(SAPPHIRE)
#define TITLE_SCREEN_BACKGROUND_COLOR_PLTT_OFFSET 26
#else
#define TITLE_SCREEN_BACKGROUND_COLOR_PLTT_OFFSET 21
#endif

#define CLEAR_SAVE_BUTTON_COMBO (B_BUTTON | SELECT_BUTTON | DPAD_UP)
#define RESET_RTC_BUTTON_COMBO (B_BUTTON | SELECT_BUTTON | DPAD_LEFT)
#define A_B_START_SELECT (A_BUTTON | B_BUTTON | START_BUTTON | SELECT_BUTTON)

static void MainCB2(void);
static void Task_TitleScreenPhase1(u8);
static void Task_TitleScreenPhase2(u8);
static void Task_TitleScreenPhase3(u8);
static void CB2_GoToMainMenu(void);
#if DEBUG
static void CB2_GoToTestMenu(void);
#endif
static void CB2_GoToClearSaveDataScreen(void);
static void CB2_GoToResetRtcScreen(void);
static void CB2_GoToCopyrightScreen(void);
static void UpdateLegendaryMarkingColor(u8);

// Task data for the main title screen tasks (Task_TitleScreenPhase#).
#define tCounter                       data[0]
#define tSkipToNext                    data[1]
#define tPointless                     data[2]
#define tBg2Y                          data[3]
#define tBg1Y                          data[4]
#define tVersionBannerFadeTimer        data[5]

// Sprite data for the version-banner sprites.
#define sParentTaskId                  data[1]

// Sprite data for the Press Start banner sprites.
#define sAnimate                       data[0]
#define sTimer                         data[1]

// Sprite data for the Pokémon logo shine sprite.
#define sFlashBackground               data[0]
#define sBackgroundColor               data[1]

#define NUM_PRESS_START_FRAMES         3
#define NUM_COPYRIGHT_FRAMES           5
#if GERMAN
#define FIRST_GERMAN_PRESS_START_FRAME (NUM_PRESS_START_FRAMES + NUM_COPYRIGHT_FRAMES)
#endif

static void SpriteCB_VersionBannerLeft(struct Sprite *sprite)
{
    struct Task *task = &gTasks[sprite->sParentTaskId];

    if (task->tSkipToNext != FALSE)
    {
        sprite->oam.objMode = 0;
        sprite->y = VERSION_BANNER_Y_GOAL;
        sprite->invisible = FALSE;
    }
    else
    {
        if (task->tVersionBannerFadeTimer != 0)
            task->tVersionBannerFadeTimer--;
        if (task->tVersionBannerFadeTimer < 64)
        {
            sprite->invisible = FALSE;
            if (sprite->y != VERSION_BANNER_Y_GOAL)
                sprite->y++;
            REG_BLDALPHA = gTitleScreenAlphaBlend[task->tVersionBannerFadeTimer / 2];
        }
    }
}

static void SpriteCB_VersionBannerRight(struct Sprite *sprite)
{
    struct Task *task = &gTasks[sprite->sParentTaskId];

    if (task->tSkipToNext != FALSE)
    {
        sprite->oam.objMode = 0;
        sprite->y = VERSION_BANNER_Y_GOAL;
        sprite->invisible = FALSE;
    }
    else if (task->tVersionBannerFadeTimer < 64)
    {
        sprite->invisible = FALSE;
        if (sprite->y != VERSION_BANNER_Y_GOAL)
            sprite->y++;
    }
}

static void SpriteCB_PressStartCopyrightBanner(struct Sprite *sprite)
{
    if (sprite->sAnimate == TRUE)
    {
        sprite->sTimer++;
        // Alternate between hidden and shown every 16th frame.
        if (sprite->sTimer & 16)
            sprite->invisible = FALSE;
        else
            sprite->invisible = TRUE;
    }
    else
    {
        sprite->invisible = FALSE;
    }
}

static void CreatePressStartBanner(s16 x, s16 y)
{
    u8 i;
    u8 spriteId;

    x -= 32;
    for (i = 0; i < NUM_PRESS_START_FRAMES; i++, x += 32)
    {
        spriteId = CreateSprite(&sStartCopyrightBannerSpriteTemplate, x, y, 0);
        StartSpriteAnim(&gSprites[spriteId], i);
        gSprites[spriteId].sAnimate = TRUE;
    }
#if GERMAN
    spriteId = CreateSprite(&sStartCopyrightBannerSpriteTemplate, x, y, 0);
    StartSpriteAnim(&gSprites[spriteId], FIRST_GERMAN_PRESS_START_FRAME);
    gSprites[spriteId].sAnimate = TRUE;
    spriteId = CreateSprite(&sStartCopyrightBannerSpriteTemplate, x - 0x60, y - 0x8, 0);
    StartSpriteAnim(&gSprites[spriteId], FIRST_GERMAN_PRESS_START_FRAME + 1);
    gSprites[spriteId].sAnimate = TRUE;
#endif
}

static void CreateCopyrightBanner(s16 x, s16 y)
{
    u8 i;
    u8 spriteId;

    x -= 64;
    for (i = 0; i < NUM_COPYRIGHT_FRAMES; i++, x += 32)
    {
        spriteId = CreateSprite(&sStartCopyrightBannerSpriteTemplate, x, y, 0);
        StartSpriteAnim(&gSprites[spriteId], i + NUM_PRESS_START_FRAMES);
    }
}

static void SpriteCB_PokemonLogoShine(struct Sprite *sprite)
{
    if (gTasks[sTitleScreenTaskId].tSkipToNext == FALSE && sprite->x < DISPLAY_WIDTH + 32)
    {
        if (sprite->sFlashBackground != FALSE)
        {
            u16 backgroundColor;

            if (sprite->x < DISPLAY_WIDTH / 2)
            {
                // Brighten background color.
                if (sprite->sBackgroundColor < 31)
                    sprite->sBackgroundColor++;
                if (sprite->sBackgroundColor < 31)
                    sprite->sBackgroundColor++;
            }
            else
            {
                // Darken background color.
                if (sprite->sBackgroundColor != 0)
                    sprite->sBackgroundColor--;
                if (sprite->sBackgroundColor != 0)
                    sprite->sBackgroundColor--;
            }
            backgroundColor = _RGB(sprite->sBackgroundColor, sprite->sBackgroundColor, sprite->sBackgroundColor);
            gPlttBufferFaded[0] = backgroundColor;
            gPlttBufferFaded[TITLE_SCREEN_BACKGROUND_COLOR_PLTT_OFFSET] = backgroundColor;
        }
        sprite->x += 4;
    }
    else
    {
        gPlttBufferFaded[0] = RGB_BLACK;
        gPlttBufferFaded[TITLE_SCREEN_BACKGROUND_COLOR_PLTT_OFFSET] = RGB_BLACK;
        DestroySprite(sprite);
    }
}

static void StartPokemonLogoShine(bool8 flashBackground)
{
    u8 spriteId = CreateSprite(&sPokemonLogoShineSpriteTemplate, 0, 68, 0);

    gSprites[spriteId].oam.objMode = 2;
    gSprites[spriteId].sFlashBackground = flashBackground;
}

#undef sAnimate
#undef sTimer
#undef sFlashBackground
#undef sBackgroundColor

static void VBlankCB(void)
{
    ScanlineEffect_InitHBlankDmaTransfer();
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    REG_BG1VOFS = gBattle_BG1_Y;
}

void CB2_InitTitleScreen(void)
{
    switch (gMain.state)
    {
    default:
    case 0:
        SetVBlankCallback(NULL);
        REG_BLDCNT = 0;
        REG_BLDALPHA = 0;
        REG_BLDY = 0;
        *((u16 *)PLTT) = RGB_WHITE;
        REG_DISPCNT = 0;
        REG_BG2CNT = 0;
        REG_BG1CNT = 0;
        REG_BG0CNT = 0;
        REG_BG2HOFS = 0;
        REG_BG2VOFS = 0;
        REG_BG1HOFS = 0;
        REG_BG1VOFS = 0;
        REG_BG0HOFS = 0;
        REG_BG0VOFS = 0;
        DmaFill16(3, 0, (void *)VRAM, 0x18000);
        DmaFill32(3, 0, (void *)OAM, 0x400);
        DmaFill16(3, 0, (void *)(PLTT + 2), 0x3FE);
        ResetPaletteFade();
        gMain.state = 1;
        break;
    case 1:
        LZ77UnCompVram(gTitleScreenPokemonLogoGfx, (void *)VRAM);
        LZ77UnCompVram(gTitleScreenPokemonLogoTilemap, (void *)(VRAM + 0x4800));
        LoadPalette(gTitleScreenBgPalettes, 0, 0x1C0);
        LZ77UnCompVram(sLegendaryMonPixelData, (void *)(VRAM + 0x8000));
        LZ77UnCompVram(sLegendaryMonTilemap, (void *)(VRAM + 0xC000));
        LZ77UnCompVram(sBackdropTilemap, (void *)(VRAM + 0xC800));
        LoadPalette(sLegendaryMonPalettes, 0xE0, sizeof(sLegendaryMonPalettes));
        ScanlineEffect_Stop();
        ResetTasks();
        ResetSpriteData();
        FreeAllSpritePalettes();
        gReservedSpritePaletteCount = 14;
        LoadCompressedSpriteSheet(&sSpriteSheet_RubySapphireVersion[0]);
        LoadCompressedSpriteSheet(&sSpriteSheet_PressStart[0]);
        LoadCompressedSpriteSheet(&sPokemonLogoShineSpriteSheet[0]);
        LoadPalette(gTitleScreenBgPalettes, 0x100, 0x1C0);
        LoadSpritePalette(&sSpritePalette_PressStart[0]);
        gMain.state = 2;
        break;
    case 2:
    {
        u8 taskId = CreateTask(Task_TitleScreenPhase1, 0);

        gTasks[taskId].tCounter = 256;
        gTasks[taskId].tSkipToNext = FALSE;
        gTasks[taskId].tPointless = -16;
        gTasks[taskId].tBg2Y = -32;
        sTitleScreenTaskId = taskId;
        gMain.state = 3;
        break;
    }
    case 3:
        BeginNormalPaletteFade(0xFFFFFFFF, 1, 16, 0, FADE_COLOR_WHITE);
        SetVBlankCallback(VBlankCB);
        gMain.state = 4;
        break;
    case 4:
    {
        u16 savedIme;

        PanFadeAndZoomScreen(0x78, 0x50, 0x100, 0);
        REG_BG2X = -29 * 256;
        REG_BG2Y = -33 * 256;
        REG_WIN0H = 0;
        REG_WIN0V = 0;
        REG_WIN1H = 0;
        REG_WIN1V = 0;
        REG_WININ = 0x1F1F;
        REG_WINOUT = 0x3F1F;
        REG_BLDCNT = 0x84;
        REG_BLDALPHA = 0;
        REG_BLDY = 0x8;
        REG_BG0CNT = BGCNT_PRIORITY(3) | BGCNT_CHARBASE(2) | BGCNT_SCREENBASE(24) | BGCNT_16COLOR | BGCNT_TXT256x256;
        REG_BG1CNT = BGCNT_PRIORITY(2) | BGCNT_CHARBASE(2) | BGCNT_SCREENBASE(25) | BGCNT_16COLOR | BGCNT_TXT256x256;
        REG_BG2CNT = BGCNT_PRIORITY(1) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(9) | BGCNT_256COLOR | BGCNT_AFF256x256;
        savedIme = REG_IME;
        REG_IME = 0;
        REG_IE |= INTR_FLAG_VBLANK;
        REG_IME = savedIme;
        REG_DISPSTAT |= DISPSTAT_VBLANK_INTR;
        REG_DISPCNT = DISPCNT_MODE_1
                    | DISPCNT_OBJ_1D_MAP
                    | DISPCNT_BG2_ON
                    | DISPCNT_OBJ_ON
                    | DISPCNT_WIN0_ON
                    | DISPCNT_OBJWIN_ON;
        m4aSongNumStart(MUS_TITLE);
        gMain.state = 5;
        break;
    }
    case 5:
        if (!UpdatePaletteFade())
        {
            StartPokemonLogoShine(FALSE);
            ScanlineEffect_InitWave(0, DISPLAY_HEIGHT, 4, 4, 0, SCANLINE_EFFECT_REG_BG1HOFS, TRUE);
            SetMainCallback2(MainCB2);
        }
        break;
    }
}

static void MainCB2(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

// Shine the Pokémon logo two more times, and fade in the version banner.
static void Task_TitleScreenPhase1(u8 taskId)
{
    // Skip to next phase when A, B, Start, or Select is pressed.
    if (JOY_NEW(A_B_START_SELECT) || gTasks[taskId].tSkipToNext != FALSE)
    {
        gTasks[taskId].tSkipToNext = TRUE;
        gTasks[taskId].tCounter = 0;
    }

    if (gTasks[taskId].tCounter != 0)
    {
        u16 frameNum = gTasks[taskId].tCounter;

        if (frameNum == 160 || frameNum == 64)
            StartPokemonLogoShine(TRUE);
        gTasks[taskId].tCounter--;
    }
    else
    {
        u8 spriteId;

        REG_DISPCNT = DISPCNT_MODE_1 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
        REG_WININ = 0;
        REG_WINOUT = 0;
        REG_BLDCNT = 0x3F50;
        REG_BLDALPHA = 0x1F;
        REG_BLDY = 0;

        // Create left side of version banner.
        spriteId = CreateSprite(&sVersionBannerLeftSpriteTemplate, VERSION_BANNER_LEFT_X, VERSION_BANNER_Y, 0);
        gSprites[spriteId].invisible = TRUE;
        gSprites[spriteId].sParentTaskId = taskId;

        // Create right side of version banner.
        spriteId = CreateSprite(&sVersionBannerRightSpriteTemplate, VERSION_BANNER_RIGHT_X, VERSION_BANNER_Y, 0);
        gSprites[spriteId].invisible = TRUE;
        gSprites[spriteId].sParentTaskId = taskId;

        gTasks[taskId].tVersionBannerFadeTimer = 88;
        gTasks[taskId].tCounter = 144;
        gTasks[taskId].func = Task_TitleScreenPhase2;
    }
}

// Create "Press Start" and copyright banners, and slide Pokémon logo up.
static void Task_TitleScreenPhase2(u8 taskId)
{
    // Skip to next phase when A, B, Start, or Select is pressed.
    if (JOY_NEW(A_B_START_SELECT) || gTasks[taskId].tSkipToNext)
    {
        gTasks[taskId].tSkipToNext = TRUE;
        gTasks[taskId].tCounter = 0;
    }

    if (gTasks[taskId].tCounter != 0)
        gTasks[taskId].tCounter--;
    else
    {
        gTasks[taskId].tSkipToNext = TRUE;
        REG_DISPCNT = DISPCNT_MODE_1
                    | DISPCNT_OBJ_1D_MAP
                    | DISPCNT_BG0_ON
                    | DISPCNT_BG1_ON
                    | DISPCNT_BG2_ON
                    | DISPCNT_OBJ_ON;
        CreatePressStartBanner(START_BANNER_X, 108);
        CreateCopyrightBanner(DISPLAY_WIDTH / 2, 148);
        gTasks[taskId].tBg1Y = 0;
        gTasks[taskId].func = Task_TitleScreenPhase3;
    }

    if (!(gTasks[taskId].tCounter & 1) && gTasks[taskId].tBg2Y != 0)
        gTasks[taskId].tBg2Y++;

    // Slide Pokémon logo up.
    REG_BG2Y = gTasks[taskId].tBg2Y * 256;
}

// Show Kyogre/Groudon silhouette and process main title screen input.
static void Task_TitleScreenPhase3(u8 taskId)
{
    REG_BLDCNT = 0x2142;
    REG_BLDALPHA = 0x1F0F;
    REG_BLDY = 0;

    if (JOY_NEW(A_BUTTON) || JOY_NEW(START_BUTTON))
    {
        FadeOutBGM(4);
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, FADE_COLOR_WHITE);
        SetMainCallback2(CB2_GoToMainMenu);
    }
    else
    {
        if (JOY_HELD(CLEAR_SAVE_BUTTON_COMBO) == CLEAR_SAVE_BUTTON_COMBO)
            SetMainCallback2(CB2_GoToClearSaveDataScreen);
        if (JOY_HELD(RESET_RTC_BUTTON_COMBO) == RESET_RTC_BUTTON_COMBO
          && CanResetRTC() == TRUE)
        {
            FadeOutBGM(4);
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            SetMainCallback2(CB2_GoToResetRtcScreen);
        }
#if DEBUG
        else if (gMain.heldKeys == SELECT_BUTTON)
        {
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            SetMainCallback2(CB2_GoToTestMenu);
        }
#endif
        else
        {
            REG_BG2Y = 0;
            gTasks[taskId].tCounter++;
            if (gTasks[taskId].tCounter & 1)
            {
                gTasks[taskId].tBg1Y++;
                gBattle_BG1_Y = gTasks[taskId].tBg1Y;
                gBattle_BG1_X = 0;
            }
            UpdateLegendaryMarkingColor(gTasks[taskId].tCounter);
            if ((gMPlayInfo_BGM.status & 0xFFFF) == 0)
            {
                BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, FADE_COLOR_WHITE);
                SetMainCallback2(CB2_GoToCopyrightScreen);
            }
        }
    }
}

static void CB2_GoToMainMenu(void)
{
    if (!UpdatePaletteFade())
        SetMainCallback2(CB2_InitMainMenu);
}

#if DEBUG
static void CB2_GoToTestMenu(void)
{
    if (!UpdatePaletteFade())
        SetMainCallback2(CB2_InitTestMenu);
}
#endif

static void CB2_GoToCopyrightScreen(void)
{
    if (!UpdatePaletteFade())
        SetMainCallback2(CB2_InitCopyrightScreenAfterTitleScreen);
}

static void CB2_GoToClearSaveDataScreen(void)
{
    if (!UpdatePaletteFade())
        SetMainCallback2(CB2_InitClearSaveDataScreen);
}

static void CB2_GoToResetRtcScreen(void)
{
    if (!UpdatePaletteFade())
        SetMainCallback2(CB2_InitResetRtcScreen);
}

static void UpdateLegendaryMarkingColor(u8 frameNum)
{
    u16 palette;

    if ((frameNum % 4) == 0) //Change color every 4th frame
    {
        u8 colorIntensity = (frameNum >> 2) & 31; //Take bits 2-6 of frameNum the color intensity
        u8 fadeDarker = (frameNum >> 2) & 32;

        if (!fadeDarker)
            palette = LEGENDARY_MARKING_COLOR(colorIntensity);
        else
            palette = LEGENDARY_MARKING_COLOR(31 - colorIntensity);
        LoadPalette(&palette, 0xEF, sizeof(palette));
   }
}

#undef sParentTaskId
#undef tCounter
#undef tSkipToNext
#undef tPointless
#undef tBg2Y
#undef tBg1Y
#undef tVersionBannerFadeTimer
#undef NUM_PRESS_START_FRAMES
#undef NUM_COPYRIGHT_FRAMES
#if GERMAN
#undef FIRST_GERMAN_PRESS_START_FRAME
#endif
