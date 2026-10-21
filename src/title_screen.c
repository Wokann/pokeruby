#include "global.h"
#include "constants/rgb.h"
#include "constants/songs.h"
#include "battle.h"
#include "graphics.h"
#include "gba/m4a_internal.h"
#include "title_screen.h"
#include "clear_save_data_menu.h"
#include "decompress.h"
#include "event_data.h"
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

enum {
    TAG_VERSION = 1000,
    TAG_PRESS_START_COPYRIGHT,
    TAG_LOGO_SHINE,
};

#if ENGLISH
#define NUM_PRESS_START_FRAMES 3
#define VERSION_BANNER_SHAPE SPRITE_SHAPE(64x32)
#define VERSION_BANNER_RIGHT_TILEOFFSET 64
#define VERSION_BANNER_BYTES 0x1000
#define VERSION_BANNER_LEFT_X 98
#define VERSION_BANNER_RIGHT_X 162
#define VERSION_BANNER_Y 26
#define VERSION_BANNER_Y_GOAL 66
#define START_BANNER_X DISPLAY_WIDTH / 2
#elif GERMAN
#define NUM_PRESS_START_FRAMES 5
#define VERSION_BANNER_SHAPE SPRITE_SHAPE(64x64)
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

static void SpriteCB_VersionBannerLeft(struct Sprite *sprite);
static void SpriteCB_VersionBannerRight(struct Sprite *sprite);
static void SpriteCB_PressStartCopyrightBanner(struct Sprite *sprite);
static void SpriteCB_PokemonLogoShine(struct Sprite *sprite);

static EWRAM_DATA u8 gUnknown_0202F7E4 = 0;

#ifdef SAPPHIRE
static const u16 sLegendaryMonPalettes[][PLTT_ID(1)] =
{
    INCBIN_U16("graphics/title_screen/kyogre_dark.gbapal"),
    INCBIN_U16("graphics/title_screen/kyogre_glow.gbapal"),
};
static const u8 sLegendaryMonPixelData[] = INCBIN_U8("graphics/title_screen/kyogre.4bpp.lz");
static const u8 sLegendaryMonTilemap[] = INCBIN_U8("graphics/title_screen/kyogre_map.bin.lz");
static const u8 sBackdropTilemap[] = INCBIN_U8("graphics/title_screen/water_map.bin.lz");
#else
static const u16 sLegendaryMonPalettes[][PLTT_ID(1)] =
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
    BLDALPHA_BLEND(16, 0),
    BLDALPHA_BLEND(16, 1),
    BLDALPHA_BLEND(16, 2),
    BLDALPHA_BLEND(16, 3),
    BLDALPHA_BLEND(16, 4),
    BLDALPHA_BLEND(16, 5),
    BLDALPHA_BLEND(16, 6),
    BLDALPHA_BLEND(16, 7),
    BLDALPHA_BLEND(16, 8),
    BLDALPHA_BLEND(16, 9),
    BLDALPHA_BLEND(16, 10),
    BLDALPHA_BLEND(16, 11),
    BLDALPHA_BLEND(16, 12),
    BLDALPHA_BLEND(16, 13),
    BLDALPHA_BLEND(16, 14),
    BLDALPHA_BLEND(16, 15),
    BLDALPHA_BLEND(15, 16),
    BLDALPHA_BLEND(14, 16),
    BLDALPHA_BLEND(13, 16),
    BLDALPHA_BLEND(12, 16),
    BLDALPHA_BLEND(11, 16),
    BLDALPHA_BLEND(10, 16),
    BLDALPHA_BLEND(9, 16),
    BLDALPHA_BLEND(8, 16),
    BLDALPHA_BLEND(7, 16),
    BLDALPHA_BLEND(6, 16),
    BLDALPHA_BLEND(5, 16),
    BLDALPHA_BLEND(4, 16),
    BLDALPHA_BLEND(3, 16),
    BLDALPHA_BLEND(2, 16),
    BLDALPHA_BLEND(1, 16),
    BLDALPHA_BLEND(0, 16),
};
static const struct OamData sVersionBannerLeftOamData =
{
    .y = DISPLAY_HEIGHT,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_8BPP,
    .shape = VERSION_BANNER_SHAPE,
    .x = 0,
    .matrixNum = 0,
    .size = ST_OAM_SIZE_3,
    .tileNum = 0,
    .priority = 0,
    .paletteNum = 0,
    .affineParam = 0,
};
static const struct OamData sVersionBannerRightOamData =
{
    .y = DISPLAY_HEIGHT,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_8BPP,
    .shape = VERSION_BANNER_SHAPE,
    .x = 0,
    .matrixNum = 0,
    .size = ST_OAM_SIZE_3,
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
static const struct CompressedSpriteSheet gUnknown_08393EFC[] =
{
    {gVersionTiles, VERSION_BANNER_BYTES, TAG_VERSION},
    {NULL},
};
static const struct OamData sOamData_CopyrightBanner =
{
    .y = DISPLAY_HEIGHT,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x8),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(32x8),
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
static const union AnimCmd sAnim_PressStart_3[] =
{
    ANIMCMD_FRAME(32, 4),
    ANIMCMD_END,
};
static const union AnimCmd sAnim_PressStart_4[] =
{
    ANIMCMD_FRAME(36, 4),
    ANIMCMD_END,
};
#endif
#define NUM_COPYRIGHT_FRAMES 5

// R/S German extra Press Start frames follow the copyright frames.
static const union AnimCmd *const sStartCopyrightBannerAnimTable[NUM_PRESS_START_FRAMES + NUM_COPYRIGHT_FRAMES] =
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
    sAnim_PressStart_3,
    sAnim_PressStart_4,
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
    .y = DISPLAY_HEIGHT,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(64x64),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(64x64),
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

#ifdef SAPPHIRE
//Red Kyogre markings
#define LEGENDARY_MARKING_COLOR(c) RGB((c), 0, 0)
#else
//Blue Groundon markings
#define LEGENDARY_MARKING_COLOR(c) RGB(0, 0, (c))
#endif

#if defined(GERMAN) && defined(SAPPHIRE)
#define PLTT_BUFFER_INDEX 9
#elif defined(SAPPHIRE)
#define PLTT_BUFFER_INDEX 26
#else
#define PLTT_BUFFER_INDEX 21
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

#define tCounter    data[0]
#define tSkipToNext data[1]
#define tPointless  data[2]
#define tBg2Y       data[3]
#define tBg1Y       data[4]

#define sParentTaskId  data[1]

static void SpriteCB_VersionBannerLeft(struct Sprite *sprite)
{
    struct Task *task = &gTasks[sprite->sParentTaskId];

    if (task->tSkipToNext != FALSE)
    {
        sprite->oam.objMode = ST_OAM_OBJ_NORMAL;
        sprite->y = VERSION_BANNER_Y_GOAL;
        sprite->invisible = FALSE;
    }
    else
    {
        if (task->data[5] != 0)
            task->data[5]--;
        if (task->data[5] < 64)
        {
            sprite->invisible = FALSE;
            if (sprite->y != VERSION_BANNER_Y_GOAL)
                sprite->y++;
            REG_BLDALPHA = gTitleScreenAlphaBlend[task->data[5] / 2];
        }
    }
}

static void SpriteCB_VersionBannerRight(struct Sprite *sprite)
{
    struct Task *task = &gTasks[sprite->sParentTaskId];

    if (task->tSkipToNext != FALSE)
    {
        sprite->oam.objMode = ST_OAM_OBJ_NORMAL;
        sprite->y = VERSION_BANNER_Y_GOAL;
        sprite->invisible = FALSE;
    }
    else
    {
        if (task->data[5] < 64)
        {
            sprite->invisible = FALSE;
            if (sprite->y != VERSION_BANNER_Y_GOAL)
                sprite->y++;
        }
    }
}

#define sAnimate data[0]
#define sTimer   data[1]

static void SpriteCB_PressStartCopyrightBanner(struct Sprite *sprite)
{
    if (sprite->sAnimate == TRUE)
    {
        sprite->sTimer++;
        //Alternate between hidden and shown every 16th frame
        if (sprite->sTimer & 16)
            sprite->invisible = FALSE;
        else
            sprite->invisible = TRUE;
    }
    else
        sprite->invisible = FALSE;
}

static void CreatePressStartBanner(s16 x, s16 y)
{
    u8 i;
    u8 spriteId;

    x -= 32;
    for (i = 0; i < 3; i++, x += 32)
    {
        spriteId = CreateSprite(&sStartCopyrightBannerSpriteTemplate, x, y, 0);
        StartSpriteAnim(&gSprites[spriteId], i);
        gSprites[spriteId].sAnimate = TRUE;
    }
#if GERMAN
    spriteId = CreateSprite(&sStartCopyrightBannerSpriteTemplate, x, y, 0);
    StartSpriteAnim(&gSprites[spriteId], 8);
    gSprites[spriteId].sAnimate = TRUE;
    spriteId = CreateSprite(&sStartCopyrightBannerSpriteTemplate, x - 0x60, y - 0x8, 0);
    StartSpriteAnim(&gSprites[spriteId], 9);
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
        StartSpriteAnim(&gSprites[spriteId], i + 3);
    }
}

#undef sAnimate
#undef sTimer

#define SHINE_SPEED  4

#define sBgColor  data[1]

static void SpriteCB_PokemonLogoShine(struct Sprite *sprite)
{
    if (gTasks[gUnknown_0202F7E4].tSkipToNext == FALSE && sprite->x < DISPLAY_WIDTH + 32)
    {
        if (sprite->data[0]) //Flash background
        {
            u16 backgroundColor;

            if (sprite->x < DISPLAY_WIDTH / 2)
            {
                //Brighten background color
                if (sprite->sBgColor < 31)
                    sprite->sBgColor++;
                if (sprite->sBgColor < 31)
                    sprite->sBgColor++;
            }
            else
            {
                //Darken background color
                if (sprite->sBgColor != 0)
                    sprite->sBgColor--;
                if (sprite->sBgColor != 0)
                    sprite->sBgColor--;
            }
            backgroundColor = _RGB(sprite->sBgColor, sprite->sBgColor, sprite->sBgColor);
            gPlttBufferFaded[0] = backgroundColor;
            gPlttBufferFaded[PLTT_BUFFER_INDEX] = backgroundColor;
        }
        sprite->x += SHINE_SPEED;
    }
    else
    {
        gPlttBufferFaded[0] = RGB_BLACK;
        gPlttBufferFaded[PLTT_BUFFER_INDEX] = RGB_BLACK;
        DestroySprite(sprite);
    }
}

static void StartPokemonLogoShine(bool8 flashBackground)
{
    u8 spriteId = CreateSprite(&sPokemonLogoShineSpriteTemplate, 0, 68, 0);

    gSprites[spriteId].oam.objMode = ST_OAM_OBJ_WINDOW;
    gSprites[spriteId].data[0] = flashBackground;
}

#undef sBgColor

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
        DmaFill16(3, 0, (void *)VRAM, VRAM_SIZE);
        DmaFill32(3, 0, (void *)OAM, OAM_SIZE);
        DmaFill16(3, 0, (void *)(PLTT + 2), PLTT_SIZE - 2);
        ResetPaletteFade();
        gMain.state = 1;
        break;
    case 1:
        LZ77UnCompVram(gTitleScreenPokemonLogoGfx, BG_CHAR_ADDR(0));
        LZ77UnCompVram(gTitleScreenPokemonLogoTilemap, BG_SCREEN_ADDR(9));
        LoadPalette(gTitleScreenBgPalettes, BG_PLTT_ID(0), 14 * PLTT_SIZE_4BPP);
        LZ77UnCompVram(sLegendaryMonPixelData, BG_CHAR_ADDR(2));
        LZ77UnCompVram(sLegendaryMonTilemap, BG_SCREEN_ADDR(24));
        LZ77UnCompVram(sBackdropTilemap, BG_SCREEN_ADDR(25));
        LoadPalette(sLegendaryMonPalettes, BG_PLTT_ID(14), sizeof(sLegendaryMonPalettes));
        ScanlineEffect_Stop();
        ResetTasks();
        ResetSpriteData();
        FreeAllSpritePalettes();
        gReservedSpritePaletteCount = 14;
        LoadCompressedSpriteSheet(&gUnknown_08393EFC[0]);
        LoadCompressedSpriteSheet(&sSpriteSheet_PressStart[0]);
        LoadCompressedSpriteSheet(&sPokemonLogoShineSpriteSheet[0]);
        LoadPalette(gTitleScreenBgPalettes, OBJ_PLTT_ID(0), 14 * PLTT_SIZE_4BPP);
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
        gUnknown_0202F7E4 = taskId;
        gMain.state = 3;
        break;
    }
    case 3:
        BeginNormalPaletteFade(PALETTES_ALL, 1, 16, 0, RGB_WHITEALPHA);
        SetVBlankCallback(VBlankCB);
        gMain.state = 4;
        break;
    case 4:
    {
        u16 savedIme;

        PanFadeAndZoomScreen(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, 0x100, 0);
        REG_BG2X = -29 * 256;
        REG_BG2Y = -33 * 256;
        REG_WIN0H = 0;
        REG_WIN0V = 0;
        REG_WIN1H = 0;
        REG_WIN1V = 0;
        REG_WININ = WININ_WIN0_BG_ALL | WININ_WIN0_OBJ | WININ_WIN1_BG_ALL | WININ_WIN1_OBJ;
        REG_WINOUT = WINOUT_WIN01_BG_ALL | WINOUT_WIN01_OBJ | WINOUT_WINOBJ_ALL;
        REG_BLDCNT = BLDCNT_TGT1_BG2 | BLDCNT_EFFECT_LIGHTEN;
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

//Shine the Pokemon logo two more times, and fade in the version banner
static void Task_TitleScreenPhase1(u8 taskId)
{
    //Skip to next phase when A, B, Start, or Select is pressed
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
        REG_BLDCNT = BLDCNT_TGT1_OBJ | BLDCNT_EFFECT_BLEND | BLDCNT_TGT2_ALL;
        REG_BLDALPHA = BLDALPHA_BLEND(31, 0);
        REG_BLDY = 0;

        //Create left side of version banner
        spriteId = CreateSprite(&sVersionBannerLeftSpriteTemplate, VERSION_BANNER_LEFT_X, VERSION_BANNER_Y, 0);
        gSprites[spriteId].invisible = TRUE;
        gSprites[spriteId].sParentTaskId = taskId;

        //Create right side of version banner
        spriteId = CreateSprite(&sVersionBannerRightSpriteTemplate, VERSION_BANNER_RIGHT_X, VERSION_BANNER_Y, 0);
        gSprites[spriteId].invisible = TRUE;
        gSprites[spriteId].sParentTaskId = taskId;

        gTasks[taskId].data[5] = 88;
        gTasks[taskId].tCounter = 144;
        gTasks[taskId].func = Task_TitleScreenPhase2;
    }
}

#undef sParentTaskId

//Create "Press Start" and copyright banners, and slide Pokemon logo up
static void Task_TitleScreenPhase2(u8 taskId)
{
    //Skip to next phase when A, B, Start, or Select is pressed
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

    //Slide Pokemon logo up
    REG_BG2Y = gTasks[taskId].tBg2Y * 256;
}

//Show Kyogre/Groundon silhouette and process main title screen input
static void Task_TitleScreenPhase3(u8 taskId)
{
    REG_BLDCNT = BLDCNT_TGT1_BG1 | BLDCNT_EFFECT_BLEND | BLDCNT_TGT2_BG0 | BLDCNT_TGT2_BD;
    REG_BLDALPHA = BLDALPHA_BLEND(15, 31);
    REG_BLDY = 0;

    if (JOY_NEW(A_BUTTON) || JOY_NEW(START_BUTTON))
    {
        FadeOutBGM(4);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_WHITEALPHA);
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
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
            SetMainCallback2(CB2_GoToResetRtcScreen);
        }
#if DEBUG
        else if (gMain.heldKeys == SELECT_BUTTON)
        {
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
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
            if ((gMPlayInfo_BGM.status & MUSICPLAYER_STATUS_TRACK) == 0)
            {
                BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_WHITEALPHA);
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
    u16 color;

    if ((frameNum % 4) == 0) //Change color every 4th frame
    {
        u8 colorIntensity = (frameNum >> 2) & 31; //Take bits 2-6 of frameNum the color intensity
        u8 fadeDarker = (frameNum >> 2) & 32;

        if (!fadeDarker)
            color = LEGENDARY_MARKING_COLOR(colorIntensity);
        else
            color = LEGENDARY_MARKING_COLOR(31 - colorIntensity);
        LoadPalette(&color, BG_PLTT_ID(14) + 15, sizeof(color));
   }
}
