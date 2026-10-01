
// Includes
#include "global.h"
#include "ewram.h"
#include "main.h"
#include "palette.h"
#include "decompress.h"
#include "trig.h"
#include "data2.h"
#include "scanline_effect.h"
#include "pokemon_storage_system.h"
#include "text.h"
#include "menu.h"
#include "landmark.h"
#include "strings.h"
#include "string_util.h"
#include "event_data.h"
#include "use_pokeblock.h"
#include "overworld.h"
#include "pokemon_summary_screen.h"
#include "link.h"
#include "sound.h"
#include "battle_setup.h"
#include "constants/songs.h"
#include "constants/flags.h"
#include "constants/game_stat.h"
#include "pokenav.h"
#include "constants/rgb.h"

// Static type declarations

// Static RAM declarations

EWRAM_DATA u8 gRegionMapLinkLandmarkPrintState[4] = {};
EWRAM_DATA u16 gPokenavRibbonDescriptionId = 0;

extern const u8 gPokenavTrainerEyesList_Pal[];
extern const u16 gPokenavTrainerEyesListBg_Pal[];
extern const u16 gPokenavRibbonsList_Pal[];
extern const u8 gPokenavList_Tilemap[];
extern const u8 gPokenavTrainerEyesMisc_Gfx[];
extern const u8 gPokenavTrainerEyesListBottom_Tilemap[];
extern const u8 gPokenavTrainerEyesListTop_Tilemap[];
extern const u8 gPokenavListInfo_Tilemap[];
extern const u16 gPokenavConditionSearch2_Pal[];
extern const u8 gPokenavTrainerEyesMisc_Pal[];
extern const u16 gPokenavList_Pal[];
extern const u8 gPokenavConditionSearch2_Gfx[];
extern const u16 gConditionGraphData_Pal[];
extern const u8 gConditionGraphData_Tilemap[];
extern const u8 gPokenavConditionSearchGraphTilemap[];
extern const u8 gPokenavCondition_Tilemap[];
extern const u8 gPokenavConditionMenu2_Pal[];
extern const u8 gPokenavConditionView_Gfx[];
extern const u8 gPokenavRegionMapCityBannerFrames[];
extern const u8 gPokenavRegionMapCityBannerBlank[];
extern const u8 *const gPokenavCityMaps[][2];
extern const u8 gPokenavHoennMapSquares_Pal[];
extern const u8 gPokenavHoennMapSquares_Gfx[];
extern const u16 gPokenavMenuPaletteGradientColors[];
extern const u8 *const gPokenavMainMenuDescriptions[];
extern const u8 *const gPokenavConditionMenuDescriptions[];
extern const u8 *const gPokenavConditionSearchDescriptions[];
extern u8 *const gPokenavTileBuffer;
extern const u8 gPokenavMenuBackground_Gfx[];
extern const u8 gPokenavBg3Scroll_Gfx[];
extern const u8 gPokenavBg3Scroll_Tilemap[];
extern const u8 gPokenavOutlineTilemap[];
extern const u8 gPokenavOutlineTiles[];
extern const u8 gPokenavOutlinePalette[];
extern const u8 gPokenavMenuBackground_Pal[];
extern const u8 gPokenavMenuBackground_Tilemap[];
extern const u8 gPokenavHoennMapMisc_Gfx[];
extern const u8 gPokenavHoennMap_Tilemap[];
extern const u8 gPokenavBottomToolbarTilemap[];
extern const u16 gPokenavHoennMap1_Pal[];
extern void (*const gPokenavListRowPrinters[])(u16, u16);
extern const u8 gPokenavTrainerEyesRematchIconTilemap[];
extern const u8 gPokenavTrainerEyesNoRematchIconTilemap[];
extern const u8 gPokenavPokeblockGraphTilemap[];
extern const u8 *const gTrainerEyeDescriptions[];
extern const u8 gPokenavRibbonsSummaryTilemap[];
extern const u8 gPokenavRibbonView_Gfx[];
extern const u8 gPokenavRibbonIcons_Gfx[];
extern const u16 gPokenavRibbonView_Pal[];
extern const u16 gPokenavRibbonsSummary_Pal[];
extern const u16 gPokenavRibbonIconPalettes[][16];
extern const u16 gPokenavRibbonGfxData[][2];
extern const u8 *const gRibbonDescriptions[][2];
extern const u8 *const gGiftRibbonDescriptions[][2];
extern const u8 gPokenavMonInfoHeaderTilemap[];
extern const u8 gPokenavMonInfoHeader_Gfx[];
extern const u16 gPokenavConditionMonInfoHeader_Pal[];
extern const u16 gPokenavRibbonsMonInfoHeader_Pal[];
extern const u8 gPokenavMenuOptions_Gfx[];
extern const u8 gPokenavConditionMenu_Gfx[];
extern const u8 gPokenavConditionSearch_Gfx[];
extern const struct SpriteTemplate gPokenavMenuOptionSpriteTemplate;
extern const union AffineAnimCmd *const gPokenavMenuOptionZoomAffineAnimTable[];
extern const u16 gPokenavMenuOptionBlendAlphas[];
extern const u16 gPokenavMenuOptions1_Pal[];
extern const u16 gPokenavMenuOptions2_Pal[];
extern const u16 gPokenavConditionMenu_Pal[];
extern const u16 gPokenavCondition6_Pal[];
extern const u16 gPokenavCondition7_Pal[];
extern const struct SpriteSheet gSpriteSheet_PokenavBlueLight;
extern const struct SpritePalette gSpritePalette_PokenavBlueLight;
extern const struct SpriteTemplate gPokenavBlueLightSpriteTemplate;
extern const u8 gPokenavMainMenuHeader_Gfx[];
extern const u8 gPokenavConditionMenuHeader_Gfx[];
extern const u8 gPokenavRibbonsHeader_Gfx[];
extern const u8 gPokenavHoennMapHeader_Gfx[];
extern const u8 gPokenavConditionHeaderLabels_Gfx[];
extern const u8 gPokenavConditionHeaderLabels2_Gfx[];
extern const u8 gPokenavTrainersEyesHeader_Gfx[];
extern const struct SpritePalette gPokenavMenuHeaderSpritePalettes[];
extern const struct SpriteTemplate gPokenavSmallHeaderSpriteTemplate;
extern const struct SpriteTemplate gPokenavSmallHeaderAltSpriteTemplate;
extern const struct SpriteTemplate gPokenavLargeHeaderSpriteTemplate;
extern const struct SpriteTemplate gPokenavTrainersEyesHeaderSpriteTemplate;
extern const struct SpriteSheet gPokenavPortraitSpriteSheet;
extern const struct SpriteTemplate gPokenavPortraitSpriteTemplate;
extern const struct SpritePalette gPokenavPortraitSpritePalette;
extern const struct SpriteSheet gPokenavListArrowSpriteSheets[3];
extern const struct SpritePalette gPokenavListArrowSpritePalette;
extern const u16 gPokenavListArrowAltPalette[];
extern const struct SpriteTemplate gPokenavListRightArrowSpriteTemplate;
extern const struct SpriteTemplate gPokenavListUpDownArrowSpriteTemplate;
extern const struct SpriteSheet gPokenavConditionSelectionIconSheets[4];
extern const struct SpritePalette gPokenavConditionSelectionIconPalettes[3];
extern const struct SpriteTemplate gPokenavConditionSelectionIconTemplate;
extern const u16 gPokenavConditionMonMarkingsPalette[];
extern const u8 gPokenavRibbonIconsBigHalfGfx[];
extern const struct SpriteTemplate gPokenavBigRibbonSpriteTemplate;
extern const struct SpriteSheet gPokenavConditionSparkleSpriteSheet;
extern const struct SpritePalette gPokenavConditionSparkleSpritePalette;
extern const struct SpriteTemplate gPokenavConditionSparkleSpriteTemplate;
extern const s16 gPokenavConditionSparkleCoords[][2];
extern const u8 gPokenavIconGfx[];
extern const struct SpritePalette gPokenavIconSpritePalette;
extern const struct SpriteTemplate gPokenavIconSpriteTemplate;
extern const struct SpritePalette gPokenavTrainerEyesNameSpritePalette;
extern const struct SpriteTemplate gPokenavTrainerEyesNameSpriteTemplate;

// Static ROM declarations

u8 PokenavList_MoveCursorUp(void);
u8 PokenavList_MoveCursorDown(void);
u8 PokenavList_PageUp(void);
u8 PokenavList_PageDown(void);

// .rodata

extern const u8 gConditionToLineLength[];

// .text

void ResetPokenavBgOffsets(void)
{
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;
    REG_BG2VOFS = 0;
    REG_BG2HOFS = 0;
    REG_BG3HOFS = 0;
    REG_BG3VOFS = 0;
}

void ResetPokenavSetupStep(void)
{
    gPokenavState->setupStep = 0;
}

void InitPokenavBackground(void)
{
    gPokenavState->setupStep = 0;
    if (!gPokenavState->isLinkActive)
    {
        while (LoadPokenavBackgroundStep())
            ;
    }
}

bool8 LoadPokenavBackgroundStep(void)
{
    switch (gPokenavState->setupStep)
    {
    case 0:
        LZ77UnCompVram(gPokenavHoennMapMisc_Gfx, (void *)VRAM + 0xC000);
        break;
    case 1:
        LZ77UnCompVram(gPokenavHoennMap_Tilemap, (void *)VRAM + 0xD800);
        break;
    case 2:
        LoadPalette(gPokenavHoennMap1_Pal, 0x10, 0x20);
        break;
    case 3:
        InitPokenavPaletteGradient(0);
        InitPokenavPaletteGradient(1);
        InitPokenavPaletteGradient(2);
        break;
    case 4:
        gPokenavState->menuVerticalOffset = 0;
        REG_BG1HOFS = 0;
        REG_BG1VOFS = 0;
        REG_BG1CNT = 0x1B0C;
        gPokenavState->setupStep++;
    default:
        return FALSE;
    }
    gPokenavState->setupStep++;
    return TRUE;
}

bool8 SlideMenuHeaderUp(void)
{
    bool8 isSliding = TRUE;

    if (gPokenavState->menuVerticalOffset == 32)
        return FALSE;

    gPokenavState->menuVerticalOffset += 2;
    if (gPokenavState->menuVerticalOffset > 31)
    {
        gPokenavState->menuVerticalOffset = 32;
        isSliding = FALSE;
    }

    REG_BG1VOFS = gPokenavState->menuVerticalOffset;
    return isSliding;
}

bool8 SlideMenuHeaderDown(void)
{
    bool8 isSliding = TRUE;

    if (gPokenavState->menuVerticalOffset == 0)
        return FALSE;

    gPokenavState->menuVerticalOffset -= 2;
    if (gPokenavState->menuVerticalOffset <= 0)
    {
        gPokenavState->menuVerticalOffset = 0;
        isSliding = FALSE;
    }

    REG_BG1VOFS = gPokenavState->menuVerticalOffset;
    return isSliding;
}

void DrawPokenavBottomToolbar(u8 toolbarId)
{
    u8 linkActive;

    CopyTilemapRectFixedWidth((void *)VRAM + 0xD800, 0, 22, gPokenavBottomToolbarTilemap, 0, 0, 17, 2);

    switch (toolbarId)
    {
    case 0:
        CopyTilemapRectFixedWidth((void *)VRAM + 0xD800, 0, 22, gPokenavBottomToolbarTilemap, 17, 0, 10, 2);
        CopyTilemapRectFixedWidth((void *)VRAM + 0xD800, 10, 22, gPokenavBottomToolbarTilemap, 0, 6, 7, 2);
        break;
    case 11:
        CopyTilemapRectFixedWidth((void *)VRAM + 0xD800, 8, 22, gPokenavBottomToolbarTilemap, 0, 6, 7, 2);
        break;
    case 2:
        linkActive = gPokenavState->isLinkActive;
        if (!linkActive)
        {
            CopyTilemapRectFixedWidth((void *)VRAM + 0xD800, 0, 22, gPokenavBottomToolbarTilemap, 10, 2, 10, 2);
            CopyTilemapRectFixedWidth((void *)VRAM + 0xD800, 10, 22, gPokenavBottomToolbarTilemap, linkActive, 6, 7, 2);
        }
        else
        {
            CopyTilemapRectFixedWidth((void *)VRAM + 0xD800, 10, 22, gPokenavBottomToolbarTilemap, 0, 6, 7, 2);
        }
        break;
    case 3:
        CopyTilemapRectFixedWidth((void *)VRAM + 0xD800, 0, 22, gPokenavBottomToolbarTilemap, 0, 4, 10, 2);
        CopyTilemapRectFixedWidth((void *)VRAM + 0xD800, 10, 22, gPokenavBottomToolbarTilemap, 0, 6, 7, 2);
        break;
    case 4:
        CopyTilemapRectFixedWidth((void *)VRAM + 0xD800, 0, 22, gPokenavBottomToolbarTilemap, 20, 2, 10, 2);
        CopyTilemapRectFixedWidth((void *)VRAM + 0xD800, 10, 22, gPokenavBottomToolbarTilemap, 0, 6, 7, 2);
        break;
    case 7:
        CopyTilemapRectFixedWidth((void *)VRAM + 0xD800, 0, 22, gPokenavBottomToolbarTilemap, 10, 4, 10, 2);
        CopyTilemapRectFixedWidth((void *)VRAM + 0xD800, 7, 22, gPokenavBottomToolbarTilemap, 0, 6, 7, 2);
        break;
    case 8:
        CopyTilemapRectFixedWidth((void *)VRAM + 0xD800, 0, 22, gPokenavBottomToolbarTilemap, 20, 4, 10, 2);
        CopyTilemapRectFixedWidth((void *)VRAM + 0xD800, 7, 22, gPokenavBottomToolbarTilemap, 0, 6, 7, 2);
        break;
    case 5:
    case 9:
        CopyTilemapRectFixedWidth((void *)VRAM + 0xD800, 0, 22, gPokenavBottomToolbarTilemap, 0, 2, 10, 2);
        CopyTilemapRectFixedWidth((void *)VRAM + 0xD800, 8, 22, gPokenavBottomToolbarTilemap, 0, 6, 7, 2);
        break;
    case 10:
        CopyTilemapRectFixedWidth((void *)VRAM + 0xD800, 8, 22, gPokenavBottomToolbarTilemap, 0, 6, 7, 2);
        break;
    }
}

void InitPokenavMenuGfx(u8 menuType)
{
    gPokenavState->setupStep = 0;

    if (!gPokenavState->isLinkActive)
    {
        while (LoadPokenavMenuGfxStep(menuType))
            ;
    }
}

bool8 LoadPokenavMenuGfxStep(u8 menuType)
{
    switch (gPokenavState->setupStep)
    {
    case 0:
        ResetPokenavBgOffsets();
        break;
    case 1:
        Text_LoadWindowTemplate(&gPokenavRegionMapWindowTemplate);
        break;
    case 2:
        MultistepInitMenuWindowBegin(&gPokenavRegionMapWindowTemplate);
        break;
    case 3:
        if (!MultistepInitMenuWindowContinue())
            return TRUE;
        break;
    case 4:
    DmaCopy16Defvars(3, gPokenavMenuBackground_Gfx, (void *)VRAM + 0x5000, 0xA0);
        break;
    case 5:
        LZ77UnCompVram(gPokenavMenuBackground_Tilemap, (void *)VRAM + 0xF800);
        break;
    case 6:
    DmaCopy16Defvars(3, gPokenavBg3Scroll_Gfx, (void *)VRAM + 0x8000, 0x20);
        break;
    case 7:
        LZ77UnCompVram(gPokenavBg3Scroll_Tilemap, (void *)VRAM + 0xE000);
        break;
    case 8:
        LZ77UnCompVram(gPokenavOutlineTilemap, (void *)VRAM + 0xE800);
        break;
    case 9:
        LZ77UnCompVram(gPokenavOutlineTiles, (void *)VRAM + 0x8020);
        break;
    case 10:
        SetPokenavMenuPalette(menuType);
        LoadPalette(gPokenavMenuBackground_Pal, 0xF0, 0x20);
        LoadPalette(gPokenavOutlinePalette, 0x40, 0x20);
        StartPokenavBg3Scroll();
        break;
    case 11:
        REG_BG0CNT = 0x1F01;
        REG_BG2CNT = 0x1D0A;
        REG_BG3CNT = 0x1C0B;
        REG_BLDCNT = 0;

        gPokenavState->setupStep++;
        return FALSE;
    default:
        return FALSE;
    }

    gPokenavState->setupStep++;
    return TRUE;
}

void PrintPokenavMenuDescription(u8 menuType, u8 itemId)
{
    u8 *tileBuffer;
    const u8 *description = 0;

    switch (menuType)
    {
    case 0:
        description = gPokenavMainMenuDescriptions[itemId];
        break;
    case 1:
        description = gPokenavConditionMenuDescriptions[itemId];
        break;
    case 2:
        description = gPokenavConditionSearchDescriptions[itemId];
        break;
    }

    tileBuffer = gPokenavTileBuffer;
    AlignStringInMenuWindow(&tileBuffer[0x800], description, 0xC0, 2);
    Menu_PrintText(&tileBuffer[0x800], 3, 17);
}

void StartPokenavPaletteTransition(u8 menuType)
{
    u16 currentIndex, targetIndex;

    if (menuType == 2)
        menuType = 1;

    gPokenavState->paletteGradientTarget = menuType * 30;
    currentIndex = gPokenavState->paletteGradientIndex;
    targetIndex = menuType * 30;
    if (currentIndex < targetIndex)
        gPokenavState->paletteGradientDelta = 2;
    else if (currentIndex > targetIndex)
        gPokenavState->paletteGradientDelta = -2;
    else
        gPokenavState->paletteGradientDelta = 0;
}

bool8 UpdatePokenavPaletteTransition(void)
{
    u16 *gradient;

    if (gPokenavState->paletteGradientIndex == gPokenavState->paletteGradientTarget)
    {
        return FALSE;
    }
    else
    {
        gPokenavState->paletteGradientIndex = gPokenavState->paletteGradientDelta + gPokenavState->paletteGradientIndex;

        gradient = gPokenavState->palettesCE52;
        LoadPalette(&gradient[gPokenavState->paletteGradientIndex], 0x31, 4);
        return TRUE;
    }

}

void SetPokenavMenuPalette(u8 menuType)
{
    if (menuType == 2)
        menuType = 1;

    gPokenavState->paletteGradientIndex = menuType * 30;
    LoadPalette(&gPokenavState->palettesCE52[gPokenavState->paletteGradientIndex], 0x31, 4);
}

void InitPokenavPaletteGradient(u8 stage)
{
    u16 i;
    u16 *gradient;
    const u16 *colors;

    switch (stage)
    {
    case 0:
        for (i = 0; i < 62; i++)
            gPokenavState->palettesCE52[i] = 0;
        break;
    case 1:
        gradient = gPokenavState->palettesCE52;
        colors = gPokenavMenuPaletteGradientColors;
        BuildPokenavPaletteGradient(&colors[1], &colors[3], 16, 2, gradient);
        break;
    case 2:
        gradient = gPokenavState->palettesCE8E;
        colors = gPokenavMenuPaletteGradientColors;
        BuildPokenavPaletteGradient(&colors[3], &colors[7], 16, 2, gradient);
        break;
    }
}

void BuildPokenavPaletteGradient(const u16 *startColors, const u16 *endColors, u8 stepCount, u8 colorCount, u16 *gradient)
{
    u16 i;
    u16 j;
    u16 *output;

    for (i = 0; i < colorCount; i++)
    {
        s32 startRed = Q_24_8(GET_R(*startColors));
        s32 startGreen = Q_24_8(GET_G(*startColors));
        s32 startBlue = Q_24_8(GET_B(*startColors));
        s32 endRed = Q_24_8(GET_R(*endColors));
        s32 endGreen = Q_24_8(GET_G(*endColors));
        s32 endBlue = Q_24_8(GET_B(*endColors));
        s32 redIncrement = (endRed - startRed) / stepCount;
        s32 greenIncrement = (endGreen - startGreen) / stepCount;
        s32 blueIncrement = (endBlue - startBlue) / stepCount;
        u16 red, green, blue;

        output = gradient;
        for (j = 0; j < stepCount - 1; j++)
        {
            red = Q_24_8_TO_INT(startRed);
            green = Q_24_8_TO_INT(startGreen);
            blue = Q_24_8_TO_INT(startBlue);
            *output = RGB2(red, green, blue);
            output += colorCount;
            startRed += redIncrement;
            startGreen += greenIncrement;
            startBlue += blueIncrement;
        }
        red = Q_24_8_TO_INT(endRed);
        green = Q_24_8_TO_INT(endGreen);
        blue = Q_24_8_TO_INT(endBlue);
        *output = RGB2(red, green, blue);
        startColors++;
        endColors++;
        gradient++;
    }
}

void UpdatePokenavBg3Scroll(void)
{
    gPokenavState->bg3ScrollFrameCounter = (gPokenavState->bg3ScrollFrameCounter + 1) & 1;
    if (gPokenavState->bg3ScrollFrameCounter)
        gPokenavState->bg3ScrollOffset++;

    REG_BG3HOFS = gPokenavState->bg3ScrollOffset;
}

void Task_PokenavBg3Scroll(u8 taskId)
{
    if (gTasks[taskId].data[0] == 0 || (gPokenavState->bg3ScrollOffset & 0x7) != 0)
    {
        UpdatePokenavBg3Scroll();
    }
    else
    {
        u16 value = gPokenavState->bg3ScrollOffset & 0x7;
        gPokenavState->bg3ScrollOffset = value;
        gPokenavState->bg3ScrollFrameCounter = value;
        REG_BG3HOFS = value;
    }
}

void StartPokenavBg3Scroll(void)
{
    gPokenavState->bg3ScrollOffset = 0;
    gPokenavState->bg3ScrollFrameCounter = 0;
    gPokenavState->bg3ScrollTaskId = CreateTask(Task_PokenavBg3Scroll, 80);
}

void StopPokenavBg3Scroll(void)
{
    if (FuncIsActiveTask(Task_PokenavBg3Scroll))
        DestroyTask(gPokenavState->bg3ScrollTaskId);
}

void InitPokenavRegionMapGfx(void)
{
    gPokenavState->setupStep = 0;

    if (gPokenavState->isLinkActive == 0)
    {
        while (LoadPokenavRegionMapGfxStep() != 0)
            ;
    }
}

bool8 LoadPokenavRegionMapGfxStep(void)
{
    switch (gPokenavState->setupStep)
    {
    case 0:
        ResetPokenavBgOffsets();
        break;
    case 1:
        Text_LoadWindowTemplate(&gPokenavRegionMapWindowTemplate);
        break;
    case 2:
        MultistepInitMenuWindowBegin(&gPokenavRegionMapWindowTemplate);
        break;
    case 3:
        if (!MultistepInitMenuWindowContinue())
            return TRUE;
        break;
    case 4:
        Menu_EraseScreen();
        break;
    case 5:
        InitRegionMapData(&gPokenavState->regionMap, gSaveBlock2.regionMapZoom ? TRUE : FALSE);
        break;
    case 6:
        if (LoadRegionMapGfx())
            return TRUE;
        break;
    case 7:
        LZ77UnCompVram(gPokenavHoennMapSquares_Gfx, (void *)VRAM + 0x5000);
        break;
    case 8:
        LoadPalette(gPokenavHoennMapSquares_Pal, 0x30, 0x20);
        InitCityMapDecompression();
        break;
    case 9:
        if (DecompressCityMapsStep())
            return TRUE;
        break;
    case 10:
        Menu_DrawStdWindowFrame(13, 3, 29, 17);
        UpdateMapSecInfoWindow();
        break;
    case 11:
        if (!gPokenavState->regionMap.zoomed)
        {
            gPokenavState->regionMapBg0YOffset = 160;
            REG_BG0VOFS = 160;
        }
        else
        {
            gPokenavState->regionMapBg0YOffset = 256;
            REG_BG0VOFS = 0;
        }

        REG_BG0CNT = REG_BG0CNT;
        REG_BG0CNT |= 1;
        REG_BLDCNT = 0;
        break;
    default:
        return FALSE;
    }

    gPokenavState->setupStep++;
    return TRUE;
}

asm(".include \"constants/gba_constants.inc\"\n");

void UpdateMapSecInfoWindow(void)
{
    bool8 isUsingLinkLandmarks = FALSE;
    u16 top = 4;
    u16 mapSecId;
    u8 mapSecPosition;

    switch (gPokenavState->regionMap.mapSecType)
    {
    case MAPSECTYPE_NONE:
        break;
    case MAPSECTYPE_ROUTE:
    case MAPSECTYPE_BATTLE_TOWER:
        Menu_PrintTextWithNewlineIndent(gPokenavState->regionMap.mapSecName, 0x70, top * 8, 0x78, 1);
        top += 2;
        if (gLinkOpen == TRUE)
        {
            StartRegionMapLinkLandmarks();
            isUsingLinkLandmarks = TRUE;
        }
        else
        {
            u16 i;

            for (i = 0; i < 4; i++)
            {
                const u8 *landmarkName = GetLandmarkName(
                    gPokenavState->regionMap.mapSecId,
                    gPokenavState->regionMap.posWithinMapSec,
                    i);

                if (landmarkName == NULL)
                    break;
                Menu_PrintTextWithNewlineIndent(landmarkName, 0x70, top * 8, 0x78, 1);
                top += 2;
            }
        }
        break;
    case MAPSECTYPE_CITY_CANFLY:
        Menu_PrintTextWithNewlineIndent(gPokenavState->regionMap.mapSecName, 0x70, top * 8, 0x78, 1);
        top += 2;
        mapSecId = gPokenavState->regionMap.mapSecId;
        mapSecPosition = gPokenavState->regionMap.posWithinMapSec;
        if (gPokenavState->cityMapTilemapPtrs[mapSecId][mapSecPosition] != NULL)
        {
            Menu_BlankWindowRect(14, top, 15, 15);
            Menu_BlankWindowRect(26, top, 28, 15);
            CopyTilemapRect((void *)(VRAM + 0xF800), 16, 6, gPokenavState->cityMapTilemapPtrs[mapSecId][mapSecPosition], 0, 0, 10, 10, 10);
            top += 11;
        }
        break;
    case MAPSECTYPE_CITY_CANTFLY:
        Menu_PrintTextWithNewlineIndent(gPokenavState->regionMap.mapSecName, 0x70, top * 8, 0x78, 1);
        top += 2;
        break;
    }

    // Epic fail by the compiler at optimizing this.
    if (!isUsingLinkLandmarks && top < 16)
        Menu_BlankWindowRect(14, top, 28, 15);

    if (gPokenavState->regionMap.mapSecType == MAPSECTYPE_CITY_CANFLY)
        ShowRegionMapCityBanner();
    else
        HideRegionMapCityBanner();
}

void UpdateRegionMapBottomToolbar(void)
{
    if (!gPokenavState->regionMap.zoomed)
        DrawPokenavBottomToolbar(8);
    else
        DrawPokenavBottomToolbar(7);
}

bool8 UpdateRegionMapBgYForZoom(bool8 zoomOut)
{
    bool8 active = TRUE;
    u16 offset = gPokenavState->regionMapBg0YOffset;

    if (zoomOut)
    {
        if (offset > 168)
        {
            offset = offset - 8;
        }
        else
        {
            offset = 160;
            active = FALSE;
        }
    }
    else
    {
        if (offset < 248)
        {
            offset = offset + 8;
        }
        else
        {
            offset = 256;
            active = FALSE;
        }
    }

    gPokenavState->regionMapBg0YOffset = offset;
    REG_BG0VOFS = offset & 0xFF;

    return active;
}

void InitCityMapDecompression(void)
{
    gPokenavState->cityMapSectionIndex = 0;
    gPokenavState->cityMapBufferIndex = 0;
    InitRegionMapCityBanner();
}

bool8 DecompressCityMapsStep(void)
{
    u16 i;
    u8 sectionIndex;
    u16 bufferIndex;

    if (gPokenavState->cityMapSectionIndex >= 16)
        return FALSE;

    sectionIndex = gPokenavState->cityMapSectionIndex;
    bufferIndex = gPokenavState->cityMapBufferIndex;
    for (i = 0; i < 2; i++)
    {
        if (gPokenavCityMaps[sectionIndex][i] != 0)
        {
            LZ77UnCompVram(gPokenavCityMaps[sectionIndex][i], gPokenavState->cityMapTilemaps[bufferIndex]);
            gPokenavState->cityMapTilemapPtrs[sectionIndex][i] = gPokenavState->cityMapTilemaps[bufferIndex];
            bufferIndex++;
        }
        else
        {
            gPokenavState->cityMapTilemapPtrs[sectionIndex][i] = NULL;
        }
    }

    if (++gPokenavState->cityMapSectionIndex >= 16)
        return FALSE;

    gPokenavState->cityMapBufferIndex = bufferIndex;
    return TRUE;
}

void InitRegionMapCityBanner(void)
{
    gPokenavState->cityBannerVisible = 0;
    gPokenavState->cityBannerFrame = 47;
    gPokenavState->cityBannerState = 0;
    gPokenavState->cityBannerDelay = 0;
}

void ShowRegionMapCityBanner(void)
{
    gPokenavState->cityBannerVisible = 1;
    if (gPokenavState->cityBannerState == 1)
        gPokenavState->cityBannerState = 2;
}

void HideRegionMapCityBanner(void)
{
    CopyTilemapRect((void *)VRAM + 0xF800, 14, 16, gPokenavRegionMapCityBannerBlank, 0, 0, 15, 1, 15);
    gPokenavState->cityBannerVisible = 0;
}

void DrawRegionMapCityBannerFrame(u8 unused)
{
    u16 visibleWidth = 60 - gPokenavState->cityBannerFrame;

    if (visibleWidth > 15)
        visibleWidth = 15;

    if (gPokenavState->cityBannerVisible != 0)
    {
        CopyTilemapRect((void *)VRAM + 0xF800, 14, 16, gPokenavRegionMapCityBannerFrames, gPokenavState->cityBannerFrame, 0, visibleWidth, 1, 60);

        if (visibleWidth < 15)
        {
            u16 wrappedFrameX = visibleWidth + 14;

            CopyTilemapRect((void *)VRAM + 0xF800, wrappedFrameX, 16, gPokenavRegionMapCityBannerFrames, 0, 0, (u16)(15 - visibleWidth), 1, 60);
        }
    }
}

void UpdateRegionMapCityBanner(void)
{
    u16 counter;
    u8 state = gPokenavState->cityBannerState;

    switch (state)
    {
    case 0:
        counter = ++gPokenavState->cityBannerFrame;

        if (counter > 59)
            gPokenavState->cityBannerFrame = state;

        DrawRegionMapCityBannerFrame(gPokenavState->cityBannerVisible);

        switch (gPokenavState->cityBannerFrame)
        {
        case 0:
        case 15:
        case 30:
        case 45:
            gPokenavState->cityBannerState = 1;
            gPokenavState->cityBannerDelay = 0;
            break;
        }
        break;
    case 1:
        counter = ++gPokenavState->cityBannerDelay;
        if (counter > 120)
        {
            gPokenavState->cityBannerDelay = 0;
            gPokenavState->cityBannerState = 0;
        }
        break;
    case 2:
        DrawRegionMapCityBannerFrame(1);
        gPokenavState->cityBannerState = 1;
        break;
    }
}

void InitConditionGraphScreen(void)
{
    gPokenavState->setupStep = 0;

    if (gPokenavState->isLinkActive == 0)
    {
        while (LoadConditionGraphScreenStep())
            ;
    }
}

bool8 LoadConditionGraphScreenStep(void)
{
    switch (gPokenavState->setupStep)
    {
    case 0:
        ResetPokenavBgOffsets();
        gPokenavState->unkD162 = 11;
        break;
    case 1:
        Text_LoadWindowTemplate(&gPokenavConditionWindowTemplate);
        break;
    case 2:
        MultistepInitMenuWindowBegin(&gPokenavConditionWindowTemplate);
        break;
    case 3:
        if (!MultistepInitMenuWindowContinue())
            return TRUE;
        break;
    case 4:
        Menu_EraseScreen();
        break;
    case 5:
        InitPokenavMonInfoHeaderGfx();
        break;
    case 6:
        if (LoadPokenavMonInfoHeaderGfxStep(FALSE))
            return TRUE;
        break;
    case 7:
        LZ77UnCompVram(gPokenavConditionView_Gfx, (void *)VRAM + 0x5000);
        break;
    case 8:
        LZ77UnCompVram(gPokenavCondition_Tilemap, (void *)VRAM + 0xF000);
        LoadPalette(gPokenavConditionMenu2_Pal, 0x20, 0x20);
        break;
    case 9:
        if (gPokenavState->isConditionGraphSearchMode == 1)
            CopyTilemapRect((void *)VRAM + 0xF000, 0, 5, gPokenavConditionSearchGraphTilemap, 0, 0, 9, 4, 9);
        break;
    case 10:
        LZ77UnCompVram(gConditionGraphData_Tilemap, (void *)VRAM + 0xB800);
        break;
    case 11:
        LoadPalette(gConditionGraphData_Pal, 0x30, 0x20);
        LoadPalette(gPokenavShared_Pal, 0xB0, 0x20);
        LoadPalette(&gPokenavConditionMenu2_Pal[2], 0xB1, 0x2);
        LoadPalette(&gPokenavConditionMenu2_Pal[16], 0xB5, 0x2);
        LoadPalette(&gPokenavConditionMenu2_Pal[30], 0xBF, 0x2);
        SetConditionGraphWindowRegs();
        break;
    case 12:
        PrintConditionGraphMonInfo(gPokenavState->portraitSlot);
        break;
    case 13:
        REG_BG3CNT = 0x1E03;
        REG_BG2CNT = 0x1702;
        REG_BLDCNT = 0x844;
        REG_BLDALPHA = 0x40B;
        break;
    default:
        return FALSE;
    }

    gPokenavState->setupStep++;
    return TRUE;
}

void SetConditionGraphBg2Visible(bool8 visible)
{
    if (visible)
        REG_DISPCNT |= DISPCNT_BG2_ON;
    else
        REG_DISPCNT &= ~DISPCNT_BG2_ON;
}

void SetConditionGraphWindowRegs(void)
{
    REG_WIN0H = WIN_RANGE(0, 240);
    REG_WIN1H = WIN_RANGE(0, 155);
    REG_WIN0V = WIN_RANGE(56, 121);
    REG_WIN1V = WIN_RANGE(56, 121);
    REG_WININ = 0x3F3F;
    REG_WINOUT = 0x001B;
}

void PrintConditionGraphMonInfo(u16 portraitSlot)
{
    Menu_PrintText(gPokenavState->conditionMonNameText[portraitSlot], 13, 1);

    if (gPokenavState->isConditionGraphSearchMode == 1)
    {
        Menu_PrintText(gPokenavState->conditionMonLocationText[portraitSlot], 13, 3);
        BufferConditionSearchRankText(gPokenavState->pokenavTextBuffer, gPokenavState->monList[gPokenavState->detailSelectionIndex].rank);
        Menu_PrintText(gPokenavState->pokenavTextBuffer, 1, 6);
    }
}

void InitPokenavListScreen(u8 listMode)
{
    gPokenavState->loadState = 0;
    gPokenavState->listMode = listMode;

    if (gPokenavState->isLinkActive == 0)
    {
        while (LoadPokenavListScreenStep())
            ;
    }
}

bool8 LoadPokenavListScreenStep(void)
{
    const u16 *trainerEyesAccentPalette;

    switch (gPokenavState->loadState)
    {
    case 0:
        ResetPokenavBgOffsets();

        gPokenavState->isRibbonsList = gPokenavState->listMode == POKENAV_LIST_RIBBONS;
        gPokenavState->unkD162 = 11;
        break;
    case 1:
        Text_LoadWindowTemplate(&gPokenavListWindowTemplate);
        break;
    case 2:
        MultistepInitMenuWindowBegin(&gPokenavListWindowTemplate);
        break;
    case 3:
        if (!MultistepInitMenuWindowContinue())
        {
            return TRUE;
        }
        break;
    case 4:
        Menu_EraseScreen();
        break;
    case 5:
        LZ77UnCompVram(gPokenavList_Tilemap, (void *)VRAM + 0xE800);
        break;
    case 6:
        LZ77UnCompVram(gPokenavConditionSearch2_Gfx, (void *)VRAM + 0x8000);
        break;
    case 7:
        LoadPalette(gPokenavList_Pal, 0xB0, 0x20);
        LoadPalette(gPokenavList_Pal, 0xF0, 0x20);
        LoadPalette(gPokenavTrainerEyesMisc_Pal, 0x40, 0x20);

        if (gPokenavState->listMode == POKENAV_LIST_CONDITION_SEARCH)
        {
            LoadPalette(gPokenavConditionSearch2_Pal, 0x30, 0x20);
            gPlttBufferUnfaded[0] = gPokenavConditionSearch2_Pal[5];
            LoadPalette(gPokenavShared_Pal, 0xB0, 0x20);
            LoadPalette(&gPokenavList_Pal[1], 0xB1, 0x2);
            LoadPalette(&gPokenavList_Pal[8], 0xB5, 0x2);
            LoadPalette(&gPokenavConditionSearch2_Pal[5], 0xBF, 0x2);
        }
        else if (gPokenavState->listMode == POKENAV_LIST_RIBBONS)
        {
            LoadPalette(gPokenavRibbonsList_Pal, 0x30, 0x20);
            gPlttBufferUnfaded[0] = gPokenavRibbonsList_Pal[5];
            LoadPalette(gPokenavShared_Pal, 0xB0, 0x20);
            LoadPalette(&gPokenavList_Pal[1], 0xB1, 0x2);
            LoadPalette(&gPokenavList_Pal[8], 0xB5, 0x2);
            LoadPalette(&gPokenavRibbonsList_Pal[5], 0xBF, 0x2);
        }
        else
        {
            LoadPalette(gPokenavTrainerEyesListBg_Pal, 0x30, 0x20);
            gPlttBufferUnfaded[0] = *(trainerEyesAccentPalette = &gPokenavTrainerEyesListBg_Pal[5]);
            LoadPalette(gPokenavTrainerEyesList_Pal, 0x50, 0x20);
            LoadPalette(&gPokenavList_Pal[1], 0xB1, 0x2);
            LoadPalette(&gPokenavList_Pal[8], 0xB5, 0x2);
            LoadPalette(trainerEyesAccentPalette, 0xBF, 0x2);
            LoadPalette(trainerEyesAccentPalette, 0x5F, 0x2);
        }
        break;
    case 8:
        if (gPokenavState->listMode != POKENAV_LIST_TRAINERS_EYES)
        {
            CopyTilemapRect((void *)VRAM + 0xE800, 0, 5, gPokenavListInfo_Tilemap, 0, 0, 9, 4, 9);
        }
        else
        {
            CopyTilemapRect((void *)VRAM + 0xE800, 0, 4, gPokenavTrainerEyesListTop_Tilemap, 0, 0, 12, 10, 12);
            CopyTilemapRect((void *)VRAM + 0xE800, 0, 8, gPokenavTrainerEyesListBottom_Tilemap, 0, 0, 12, 10, 12);
        }
        break;
    case 9:
        LZ77UnCompVram(gPokenavTrainerEyesMisc_Gfx, (void *)VRAM + 0x5000);
        break;
    case 10:
    DmaClear16(3, (void *)VRAM + 0xF800, 0x800);
        break;
    case 11:
        InitPokenavListRows();
        break;
    case 12:
        if (LoadPokenavListRowsStep())
        {
            return TRUE;
        }
        break;
    case 13:
        if (gPokenavState->listMode != POKENAV_LIST_TRAINERS_EYES)
        {
            PrintPokenavListSelectionInfo();
        }
        else
        {
            PrintTrainerEyesListStats(0);
            PrintTrainerEyesLocation((u8)gPokenavState->listSelectedIndex);
        }
        break;
    case 14:
        REG_BG2CNT = 0x1D0A;
        REG_BG3CNT = 0x1E03;
        REG_BG0CNT = 0x1F01;
        REG_BG3VOFS = 0xF8;

        gPokenavState->listBg3YOffset = 0xF8;
        gPokenavState->listTilemapRow = 0;

        REG_BLDCNT = 0;
        gPokenavState->setupStep++;
        return FALSE;
    default:
        return FALSE;
    }

    gPokenavState->loadState++;
    return TRUE;
}

void BeginPokenavListScroll(s16 rowsToScroll)
{
    s16 tilemapRow;
    s16 listIndex;
    s16 wrappedTilemapRow;

    gPokenavState->listScrollPixelsRemaining = rowsToScroll * 16;
    gPokenavState->listScrollStep = (rowsToScroll == 1 || rowsToScroll == -1) ? 4 : 8;
    if (rowsToScroll < 0)
    {
        gPokenavState->listScrollStep *= -1;
        tilemapRow = rowsToScroll * 2 + gPokenavState->listTilemapRow;
        listIndex = rowsToScroll + gPokenavState->firstVisibleListIndex;
        if (listIndex < 0)
            listIndex += gPokenavState->lastListIndex + 1;

        rowsToScroll *= -1;
    }
    else
    {
        tilemapRow = gPokenavState->listTilemapRow + 16;
        listIndex = gPokenavState->lastVisibleListIndex + 1;
    }

    if (listIndex > gPokenavState->lastListIndex)
        listIndex = 0;

    wrappedTilemapRow = tilemapRow & 0x1F;
    QueuePokenavListRows(listIndex, wrappedTilemapRow, rowsToScroll);
}

bool8 UpdatePokenavListScroll(void)
{
    if (!PrintPokenavListRowStep())
    {
        if (gPokenavState->listScrollPixelsRemaining == 0)
            return FALSE;

        gPokenavState->listScrollPixelsRemaining -= gPokenavState->listScrollStep;
        gPokenavState->listBg3YOffset += gPokenavState->listScrollStep;
        gPokenavState->listBg3YOffset &= 0xFF;
        REG_BG3VOFS = gPokenavState->listBg3YOffset;
        if (gPokenavState->listScrollPixelsRemaining == 0)
        {
            gPokenavState->listTilemapRow = ((8 + gPokenavState->listBg3YOffset) & 0xFF) / 8;
            return FALSE;
        }
    }

    return TRUE;
}

void PrintPokenavListSelectionInfo(void)
{
    switch (gPokenavState->listMode)
    {
    case POKENAV_LIST_CONDITION_SEARCH:
        BufferConditionSearchRankText(gPokenavState->pokenavTextBuffer, gPokenavState->monList[gPokenavState->listSelectedIndex].rank);
        break;
    case POKENAV_LIST_RIBBONS:
        BufferRibbonsMonListIndexText(gPokenavState->pokenavTextBuffer, gPokenavState->listSelectedIndex + 1);
        break;
    default:
        return;
    }

    BasicInitMenuWindow(&gPokenavListInfoWindowTemplate);
    Menu_PrintText(gPokenavState->pokenavTextBuffer, 1, 6);
}

void PrintTrainerEyesListStats(u8 drawStep)
{
    u32 numTrainerBattles;

    BasicInitMenuWindow(&gPokenavListInfoWindowTemplate);
    switch (drawStep)
    {
    case 0:
    case 1:
        MenuPrint_RightAligned(gOtherText_NumberRegistered, 10, 9);
        if (drawStep != 0)
            break;
        // fall through
    case 2:
        ConvertIntToDecimalStringN(
            gPokenavState->pokenavTextBuffer,
            gPokenavState->lastListIndex + 1,
            STR_CONV_MODE_RIGHT_ALIGN,
            5);
        MenuPrint_RightAligned(gPokenavState->pokenavTextBuffer, 10, 11);
        if (drawStep != 0)
            break;
        // fall through
    case 3:
        MenuPrint_RightAligned(gOtherText_NumberBattles, 10, 13);
        if (drawStep != 0)
            break;
        // fall through
    case 4:
        numTrainerBattles = GetGameStat(GAME_STAT_TRAINER_BATTLES);
        if (numTrainerBattles > 99999)
            numTrainerBattles = 99999;

        ConvertIntToDecimalStringN(
            gPokenavState->pokenavTextBuffer,
            numTrainerBattles,
            STR_CONV_MODE_RIGHT_ALIGN,
            5);
        MenuPrint_RightAligned(gPokenavState->pokenavTextBuffer, 10, 15);
        break;
    }
}

void ClearTrainerEyesListStats(void)
{
    BasicInitMenuWindow(&gPokenavListInfoWindowTemplate);
    Menu_EraseWindowRect(0, 9, 11, 16);
}

void InitPokenavListRows(void)
{
    s16 visibleRowCount = (gPokenavState->lastVisibleListIndex - gPokenavState->firstVisibleListIndex) + 1;
    if (visibleRowCount < 8)
        Menu_EraseWindowRect(12, 1, 31, 15);

    QueuePokenavListRows(gPokenavState->firstVisibleListIndex, 0, visibleRowCount);
}

bool8 LoadPokenavListRowsStep(void)
{
    return PrintPokenavListRowStep();
}

void QueuePokenavListRows(u16 firstListIndex, u16 firstTilemapRow, u16 rowCount)
{
    gPokenavState->listRowToDraw = firstListIndex;
    gPokenavState->listTilemapRowToDraw = firstTilemapRow;
    gPokenavState->listRowsToDraw = rowCount;
    gPokenavState->unk8786 = 0;
}

bool8 PrintPokenavListRowStep(void)
{
    s32 wrappedListIndex;
    if (gPokenavState->listRowsToDraw == 0)
        return FALSE;
    wrappedListIndex = 0;
    while (1)
    {
        gPokenavListRowPrinters[gPokenavState->listMode](gPokenavState->listRowToDraw, gPokenavState->listTilemapRowToDraw);
        if (--gPokenavState->listRowsToDraw == 0)
            return FALSE;
        if (++gPokenavState->listRowToDraw > gPokenavState->lastListIndex)
            gPokenavState->listRowToDraw = wrappedListIndex;
        gPokenavState->listTilemapRowToDraw += 2;
        gPokenavState->listTilemapRowToDraw &= 0x1F;
        break;
    }
    return TRUE;
}

void PrintPokenavMonListRow(u16 listIndex, u16 tilemapRow)
{
    u8 displayMode = gPokenavState->isRibbonsList == 0 ? 2 : 1;
    BufferPokenavMonListRowText(gPokenavState->pokenavTextBuffer, listIndex, displayMode);
    BasicInitMenuWindow(&gPokenavListWindowTemplate);
    Menu_PrintText(gPokenavState->pokenavTextBuffer, 13, tilemapRow);
}

void PrintTrainerEyesListRow(u16 listIndex, u16 tilemapRow)
{
    BufferTrainerEyesListRowText(gPokenavState->pokenavTextBuffer, listIndex);
    tilemapRow &= 0x1F;
    BasicInitMenuWindow(&gPokenavListWindowTemplate);
    Menu_PrintTextPixelCoords(gPokenavState->pokenavTextBuffer, 97, tilemapRow * 8, 0);
    if (listIndex < gPokenavState->trainerEyesCount && gPokenavState->trainersEye[listIndex].rematchNo != 0)
        CopyTilemapRect((void *)VRAM + 0xF000, 29, tilemapRow, gPokenavTrainerEyesRematchIconTilemap, 0, 0, 1, 2, 1);
    else
        CopyTilemapRect((void *)VRAM + 0xF000, 29, tilemapRow, gPokenavTrainerEyesNoRematchIconTilemap, 0, 0, 1, 2, 1);
}

void InitTrainerEyesListErase(void)
{
    gPokenavState->trainerEyesRowStep = 0;
    gPokenavState->loadState = 0;
}

bool8 EraseTrainerEyesListStep(void)
{
    int tilemapRow;
    if (gPokenavState->trainerEyesRowStep > 8)
    {
        return FALSE;
    }

    if (++gPokenavState->loadState > 1)
    {
        gPokenavState->loadState = 0;
        if (gPokenavState->trainerEyesRowStep < 8)
        {
            tilemapRow = (gPokenavState->listTilemapRow + (gPokenavState->trainerEyesRowStep * 2)) & 0x1F;
            if (gPokenavState->trainerEyesRowStep != gPokenavState->listCursorRow)
            {
                BasicInitMenuWindow(&gPokenavListWindowTemplate);
                Menu_EraseWindowRect(12, tilemapRow, 31, tilemapRow + 1);
            }

            if (!gPokenavState->trainerEyesRowStep)
                ClearTrainerEyesListStats();

            gPokenavState->trainerEyesRowStep++;
            return TRUE;
        }
        else
        {
            u16 row;
            BasicInitMenuWindow(&gPokenavListWindowTemplate);
            tilemapRow = (gPokenavState->listTilemapRow + 16) & 0x1F;
            for (row = 0; row < 8; row++)
            {
                Menu_EraseWindowRect(12, tilemapRow, 31, tilemapRow + 1);
                tilemapRow = (tilemapRow + 2) & 0x1F;
            }

            gPokenavState->trainerEyesRowStep++;
            return FALSE;
        }
    }
    else
    {
        return TRUE;
    }
}

void InitTrainerEyesListRestore(void)
{
    gPokenavState->trainerEyesRowStep = 0;
    gPokenavState->loadState = 0;
}

bool8 RestoreTrainerEyesListStep(void)
{
    if (gPokenavState->trainerEyesRowStep > 7)
        return FALSE;

    if (++gPokenavState->loadState > 1)
    {
        gPokenavState->loadState = 0;
        BasicInitMenuWindow(&gPokenavListWindowTemplate);
        PrintTrainerEyesListRow(gPokenavState->firstVisibleListIndex + gPokenavState->trainerEyesRowStep,
                                gPokenavState->listTilemapRow + gPokenavState->trainerEyesRowStep * 2);

        if ((++gPokenavState->trainerEyesRowStep) > 7)
        {
            PrintTrainerEyesListStats(0);
            return FALSE;
        }
    }

    return TRUE;
}

void LoadTrainerEyesDescriptionLines(void)
{
    u16 i;
    int trainerEyesId;
    const u8 *curChar;

    gPokenavState->loadState = 0;
    gPokenavState->trainerEyesRowStep = 0;
    trainerEyesId = gPokenavState->trainersEye[gPokenavState->listSelectedIndex].rematchTableIdx;
    gPokenavState->trainerEyeDescriptionLines[0] = gTrainerEyeDescriptions[trainerEyesId];

    // Find the start of the 3 other lines in the Trainer's Eyes description.
    curChar = gPokenavState->trainerEyeDescriptionLines[0];
    for (i = 0; i < 3; i++)
    {
        while (*curChar != EOS)
            curChar++;
        gPokenavState->trainerEyeDescriptionLines[i + 1] = ++curChar;
    }
}

bool8 PrintTrainerEyesDescriptionStep(void)
{
    u32 tilemapRow;

    if (gPokenavState->trainerEyesRowStep == 7)
        return FALSE;
    if (++gPokenavState->loadState < 2)
        return TRUE;
    gPokenavState->loadState = 0;
    BasicInitMenuWindow(&gPokenavListWindowTemplate);
    tilemapRow = (gPokenavState->listTilemapRow + 2 + gPokenavState->trainerEyesRowStep * 2) & 0x1F;
#ifndef NONMATCHING
    asm("":::"r2"); // fakematch
#endif //NONMATCHING
    switch (gPokenavState->trainerEyesRowStep)
    {
    default:
        return FALSE;
    case 0:
        Menu_PrintTextPixelCoords(gOtherText_Strategy, 0x61, tilemapRow * 8, 0);
        break;
    case 1:
        AlignStringInMenuWindow(gPokenavState->pokenavTextBuffer, gPokenavState->trainerEyeDescriptionLines[0], 0x88, 0);
        Menu_PrintTextPixelCoords(gPokenavState->pokenavTextBuffer, 0x61, tilemapRow * 8, 0);
        break;
    case 2:
        Menu_PrintTextPixelCoords(gOtherText_TrainersPokemon, 0x61, tilemapRow * 8, 0);
        break;
    case 3:
        AlignStringInMenuWindow(gPokenavState->pokenavTextBuffer, gPokenavState->trainerEyeDescriptionLines[1], 0x88, 0);
        Menu_PrintTextPixelCoords(gPokenavState->pokenavTextBuffer, 0x61, tilemapRow * 8, 0);
        break;
    case 4:
        Menu_PrintTextPixelCoords(gOtherText_SelfIntroduction, 0x61, tilemapRow * 8, 0);
        break;
    case 5:
        AlignStringInMenuWindow(gPokenavState->pokenavTextBuffer, gPokenavState->trainerEyeDescriptionLines[2], 0x88, 0);
        Menu_PrintTextPixelCoords(gPokenavState->pokenavTextBuffer, 0x61, tilemapRow * 8, 0);
        break;
    case 6:
        AlignStringInMenuWindow(gPokenavState->pokenavTextBuffer, gPokenavState->trainerEyeDescriptionLines[3], 0x88, 0);
        Menu_PrintTextPixelCoords(gPokenavState->pokenavTextBuffer, 0x61, tilemapRow * 8, 0);
        return FALSE;
    }
    gPokenavState->trainerEyesRowStep++;
    return TRUE;
}

void InitTrainerEyesDescriptionErase(void)
{
    gPokenavState->loadState = 0;
    gPokenavState->trainerEyesRowStep = 0;
    StringFill(gPokenavState->pokenavTextBuffer, CHAR_SPACE, 16);
}

bool8 EraseTrainerEyesDescriptionStep(void)
{
    int tilemapRow;
    if (gPokenavState->trainerEyesRowStep > 6)
        return FALSE;

    if (++gPokenavState->loadState > 1)
    {
        gPokenavState->loadState = 0;
        tilemapRow = (gPokenavState->listTilemapRow + 2 + gPokenavState->trainerEyesRowStep * 2) & 0x1F;
        BasicInitMenuWindow(&gPokenavListWindowTemplate);
        Menu_EraseWindowRect(12, tilemapRow, 31, tilemapRow + 1);
        gPokenavState->trainerEyesRowStep++;
    }

    return TRUE;
}

void BeginTrainerEyesDetailEnterScroll(void)
{
    s16 rowsToScroll = gPokenavState->listCursorRow;
    gPokenavState->listScrollPixelsRemaining = rowsToScroll * 16;
    gPokenavState->listScrollStep = rowsToScroll == 1 ? 4 : 8;
}

void BeginTrainerEyesDetailExitScroll(void)
{
    s16 rowsToScroll = gPokenavState->listCursorRow * -1;
    gPokenavState->listScrollPixelsRemaining = rowsToScroll * 16;
    gPokenavState->listScrollStep = rowsToScroll == -1 ? -4 : -8;
}

bool8 UpdateTrainerEyesDetailScroll(void)
{
    return UpdatePokenavListScroll();
}

void PrintTrainerEyesLocation(u8 listIndex)
{
    GetMapName(gPokenavState->pokenavTextBuffer, gPokenavState->trainersEye[listIndex].regionMapSectionId, 0);
    BasicInitMenuWindow(&gPokenavListInfoWindowTemplate);
    TruncateTrainerEyesLocationAtControlCode(gPokenavState->pokenavTextBuffer);
    AlignStringInMenuWindow(gPokenavState->trainerEyesLocationTextBuffer, gPokenavState->pokenavTextBuffer, 88, 2);
    Menu_PrintText(gPokenavState->trainerEyesLocationTextBuffer, 0, 5);
}

void RedrawSelectedTrainerEyesListRow(void)
{
    PrintTrainerEyesListRow(gPokenavState->listSelectedIndex, gPokenavState->listTilemapRow);
}

bool8 LoadRibbonsSummaryScreenStep(void)
{
    switch (gPokenavState->setupStep)
    {
    case 0:
        ResetPokenavBgOffsets();
        gPokenavState->unkD162 = 11;
        break;
    case 1:
        Text_LoadWindowTemplate(&gPokenavRibbonsSummaryWindowTemplate);
        break;
    case 2:
        MultistepInitMenuWindowBegin(&gPokenavRibbonsSummaryWindowTemplate);
        break;
    case 3:
        if (!MultistepInitMenuWindowContinue())
            return TRUE;
        break;
    case 4:
        Menu_EraseScreen();
        break;
    case 5:
        InitPokenavMonInfoHeaderGfx();
        break;
    case 6:
        if (LoadPokenavMonInfoHeaderGfxStep(TRUE))
            return TRUE;
        break;
    case 7:
        LZ77UnCompWram(gPokenavRibbonsSummaryTilemap, gPokenavState->ribbonsSummaryTilemap);
        break;
    case 8:
        DrawMonRibbonIcons();
        break;
    case 9:
        CopyRibbonsSummaryTilemapToVram();
        break;
    case 10:
        LZ77UnCompVram(gPokenavRibbonView_Gfx, (void *)(VRAM + 0x8000));
        break;
    case 11:
        LZ77UnCompVram(gPokenavRibbonIcons_Gfx, (void *)(VRAM + 0x8200));
        break;
    case 12:
        LoadPalette(gPokenavRibbonView_Pal, 0x20, 0x20);
        LoadPalette(gPokenavRibbonsSummary_Pal, 0xF0, 0x20);
        LoadPalette(gPokenavRibbonIconPalettes[0], 0x30, 0xA0);
        LoadPalette(gPokenavShared_Pal, 0xB0, 0x20);
        LoadPalette(gPokenavRibbonsSummary_Pal + 0xF, 0xBF, 0x2);
        break;
    case 13:
        PrintRibbonsSummaryMonInfo();
        break;
    case 14:
        REG_BG2CNT = 0x1E02;
        REG_BG3CNT = 0x170B;
        REG_BLDCNT = 0;
        gPokenavState->setupStep++;
        return FALSE;
    default:
        return FALSE;
    }

    gPokenavState->setupStep++;
    return TRUE;
}

void DrawMonRibbonIcons(void)
{
    u16 i;
    u16 offset;
    u8 ribbonId, palette, tile;
    u8 normalRibbonCount;

    offset = 0x8B;
    normalRibbonCount = gPokenavState->ribbonCount - gPokenavState->giftRibbonCount;

    for (i = 0; i < 8; i++)
    CpuFill16(0x2000, &gPokenavState->ribbonsSummaryTilemap[offset + i * 32], 0x24);

    for (i = 0; i < normalRibbonCount; i++)
    {
        ribbonId = gPokenavState->ribbonIds[i];
        palette = gPokenavRibbonGfxData[ribbonId][1] + 3;
        tile = gPokenavRibbonGfxData[ribbonId][0] * 2 + 0x10;
        gPokenavState->ribbonsSummaryTilemap[offset] = (palette << 12) | tile;
        gPokenavState->ribbonsSummaryTilemap[offset + 1] = ((palette << 12) | 0x400) | tile;
        gPokenavState->ribbonsSummaryTilemap[offset + 0x20] = (palette << 12) | (tile + 1);
        gPokenavState->ribbonsSummaryTilemap[offset + 0x21] = (palette << 12) | 0x400 | (tile + 1);

        if ((i + 1) % 9 == 0)
            offset += 0x30;
        else
            offset += 2;
    }

    offset = 0x14B;
    for (i = 0; i < gPokenavState->giftRibbonCount; i++)
    {
        ribbonId = gPokenavState->ribbonIds[normalRibbonCount + i];
        palette = gPokenavRibbonGfxData[ribbonId][1] + 3;
        tile = gPokenavRibbonGfxData[ribbonId][0] * 2 + 0x10;
        gPokenavState->ribbonsSummaryTilemap[offset] = (palette << 12) | tile;
        gPokenavState->ribbonsSummaryTilemap[offset + 1] = ((palette << 12) | 0x400) | tile;
        gPokenavState->ribbonsSummaryTilemap[offset + 0x20] = (palette << 12) | (tile + 1);
        gPokenavState->ribbonsSummaryTilemap[offset + 0x21] = (palette << 12) | 0x400 | (tile + 1);

        offset += 2;
    }
}

void CopyRibbonsSummaryTilemapToVram(void)
{
    u16 *src = gPokenavState->ribbonsSummaryTilemap;
    u16 *dest = (u16 *)(VRAM + 0xB800);
    DmaCopy32(3, src, dest, 0x500);
    gPlttBufferUnfaded[0] = *(gPokenavRibbonView_Pal + 14);
}

void PrintRibbonsSummaryMonInfo(void)
{
    Menu_PrintText(gPokenavState->conditionMonNameText[0], 13, 1);
    BufferRibbonsMonListIndexText(gPokenavState->pokenavTextBuffer, gPokenavState->listSelectedIndex + 1);
    Menu_PrintText(gPokenavState->pokenavTextBuffer, 1, 5);
}

void ClearRibbonsSummaryDescription(void)
{
    Menu_EraseWindowRect(12, 13, 27, 16);
}

// This is a fakematching function, due to a hardcoded access of gSaveBlock1.
// Due to this hardcoded address access, gift ribbons do not properly display
// their descriptions, since the hardcoded access is inside of the LinkBattleRecords
// save data, rather than the giftRibbons array, which is almost certainly what the
// intended access is.
void PrintRibbonsSummaryDescription(void)
{
    u8 *saveBlockData;
    u8 *tileBuffer1 = &gPokenavTileBuffer[0x800];
    u8 *tileBuffer2 = &gPokenavTileBuffer[0xA98];

    if (gPokenavState->ribbonPageIndex < 3)
    {
        gPokenavRibbonDescriptionId = gPokenavState->ribbonPageIndex * 9 + gPokenavState->ribbonCursorPos;
        gPokenavRibbonDescriptionId = gPokenavState->ribbonIds[gPokenavRibbonDescriptionId];
        AlignStringInMenuWindow(tileBuffer1, gRibbonDescriptions[gPokenavRibbonDescriptionId][0], 128, 0);
        AlignStringInMenuWindow(tileBuffer2, gRibbonDescriptions[gPokenavRibbonDescriptionId][1], 128, 0);
    }
    else
    {
        gPokenavRibbonDescriptionId = gPokenavState->ribbonCount - gPokenavState->giftRibbonCount;
        gPokenavRibbonDescriptionId = gPokenavState->ribbonIds[gPokenavRibbonDescriptionId + gPokenavState->ribbonCursorPos];

        // FIXME!
        saveBlockData = (u8 *)&gSaveBlock1;
        asm("ldrh r1, [r5]\n\
            add r0, r0, r1");
        gPokenavRibbonDescriptionId = saveBlockData[0x30F7];
        // The bug fix for this code is the following:
        // gPokenavRibbonDescriptionId = gSaveBlock1.giftRibbons[gPokenavRibbonDescriptionId];
        if (gPokenavRibbonDescriptionId)
        {
            gPokenavRibbonDescriptionId--;
            AlignStringInMenuWindow(tileBuffer1, gGiftRibbonDescriptions[gPokenavRibbonDescriptionId][0], 128, 0);
            AlignStringInMenuWindow(tileBuffer2, gGiftRibbonDescriptions[gPokenavRibbonDescriptionId][1], 128, 0);
        }
        else
        {
            AlignStringInMenuWindow(tileBuffer1, gText_EmptyString, 128, 0);
            AlignStringInMenuWindow(tileBuffer2, gText_EmptyString, 128, 0);
        }
    }

    Menu_PrintText(tileBuffer1, 12, 13);
    Menu_PrintText(tileBuffer2, 12, 15);
}

void PrintRibbonsSummaryCount(void)
{
    u8 *buffer;
    Menu_EraseWindowRect(12, 13, 27, 16);
    buffer = StringCopy(gPokenavState->pokenavTextBuffer, gOtherText_Ribbons);
    buffer[0] = CHAR_SPACE;
    buffer++;
    buffer = ConvertIntToDecimalStringN(
        buffer,
        gPokenavState->monList[gPokenavState->detailSelectionIndex].data,
        STR_CONV_MODE_LEFT_ALIGN,
        2);
    buffer[0] = EOS;
    Menu_PrintText(gPokenavState->pokenavTextBuffer, 12, 13);
}

void InitPokenavMonInfoHeaderGfx(void)
{
    gPokenavState->monInfoHeaderGfxStep = 0;
}

bool8 LoadPokenavMonInfoHeaderGfxStep(u8 forRibbons)
{
    switch (gPokenavState->monInfoHeaderGfxStep)
    {
    case 0:
        break;
    case 1:
        LZ77UnCompVram(gPokenavMonInfoHeaderTilemap, (void *)(VRAM + 0xE800));
        break;
    case 2:
    DmaCopy16Defvars(3, gPokenavMonInfoHeader_Gfx, (void *)(VRAM + 0xE000), 0xE0);
        break;
    case 3:
        if (!forRibbons)
            LoadPalette(gPokenavConditionMonInfoHeader_Pal, 0xD0, 0x20);
        else
            LoadPalette(gPokenavRibbonsMonInfoHeader_Pal, 0xD0, 0x20);

        gPokenavState->monInfoHeaderXOffset = -80;
        REG_BG0CNT = 0x1D0D;
        gPokenavState->monInfoHeaderGfxStep++;
        return FALSE;
    default:
        return FALSE;
    }

    gPokenavState->monInfoHeaderGfxStep++;
    return TRUE;
}

bool8 SlidePokenavMonInfoHeaderIn(void)
{
    gPokenavState->monInfoHeaderXOffset += 0x10;
    if (gPokenavState->monInfoHeaderXOffset > 0)
        gPokenavState->monInfoHeaderXOffset = 0;

    return gPokenavState->monInfoHeaderXOffset != 0;
}

bool8 SlidePokenavMonInfoHeaderOut(void)
{
    gPokenavState->monInfoHeaderXOffset -= 0x10;
    if (gPokenavState->monInfoHeaderXOffset < -0x50)
        gPokenavState->monInfoHeaderXOffset = -0x50;

    return gPokenavState->monInfoHeaderXOffset != -0x50;
}

bool8 LoadPokeblockConditionGraphScreenStep(void)
{
    switch (gPokenavState->setupStep)
    {
    case 0:
        ResetPokenavBgOffsets();
        gPokenavState->unkD162 = 2;
        break;
    case 1:
        InitPokenavMonInfoHeaderGfx();
        break;
    case 2:
        if (LoadPokenavMonInfoHeaderGfxStep(FALSE))
            return TRUE;
        break;
    case 3:
        LZ77UnCompVram(gPokenavConditionView_Gfx, (void *)(VRAM + 0x5000));
        break;
    case 4:
        LZ77UnCompVram(gPokenavCondition_Tilemap, (void *)(VRAM + 0xF000));
        LoadPalette(gPokenavConditionMenu2_Pal, 0x20, 0x20);
        break;
    case 5:
        CopyTilemapRect((void *)VRAM + 0xF000, 0, 13, gPokenavPokeblockGraphTilemap, 0, 0, 12, 4, 12);
        break;
    case 6:
        LZ77UnCompVram(gConditionGraphData_Tilemap, (void *)(VRAM + 0xB800));
        break;
    case 7:
        LoadPalette(gConditionGraphData_Pal, 0x30, 0x20);
        LoadPalette(gPokenavShared_Pal, 0xB0, 0x20);
        LoadPalette(&gPokenavConditionMenu2_Pal[2], 0xB1, 0x2);
        LoadPalette(&gPokenavConditionMenu2_Pal[16], 0xB5, 0x2);
        LoadPalette(&gPokenavConditionMenu2_Pal[30], 0xBF, 0x2);
        SetConditionGraphWindowRegs();
        break;
    case 8:
        PrintConditionGraphMonInfo(gPokenavState->portraitSlot);
        break;
    case 9:
        REG_BG3CNT = 0x1E03;
        REG_BG2CNT = 0x1702;
        REG_BLDCNT = 0x844;
        REG_BLDALPHA = 0x40B;
        break;
    default:
        return FALSE;
    }

    gPokenavState->setupStep++;
    return TRUE;
}

void PrintPokeblockMonNature(void)
{
    u8 *buffer = gPokenavState->pokenavTextBuffer;
    if (gPokenavState->monList[gPokenavState->detailSelectionIndex].isValid)
    {
        u8 nature = GetNature(&gPlayerParty[GetPartyIdFromPokeblockSelection(gPokenavState->detailSelectionIndex)]);
        buffer = StringCopy(buffer, gOtherText_Nature2);
        AlignStringInMenuWindow(buffer, gNatureNames[nature], 87, 0);
    }
    else
    {
        AlignStringInMenuWindow(buffer, gText_EmptyString, 87, 0);
    }

    Menu_PrintTextPixelCoords(gPokenavState->pokenavTextBuffer, 1, 112, 1);
}

void TruncateTrainerEyesLocationAtControlCode(u8 *text)
{
    while (text[0] != EOS)
    {
        if (text[0] == EXT_CTRL_CODE_BEGIN && text[1] == CHAR_SPACE)
        {
            text[0] = EOS;
            break;
        }

        text++;
    }
}

void UpdateRegionMapLinkLandmarks(void)
{
    // FIXME r4/r5 swapped
    register u8 *landmarkState asm("r5") = gRegionMapLinkLandmarkPrintState;
    if (landmarkState[0] == 1)
    {
        const u8 *landmarkName = GetLandmarkName(
            gPokenavState->regionMap.mapSecId,
            gPokenavState->regionMap.posWithinMapSec,
            landmarkState[1]);

        if (landmarkName)
        {
            Menu_PrintTextWithNewlineIndent(landmarkName, 0x70, 4 * (landmarkState[1] * 4 + 12), 0x78, 1);
            if (++landmarkState[1] != 4)
                return;
        }

        Menu_BlankWindowRect(14, landmarkState[1] * 2 + 6, 28, 15);
        landmarkState[0] = 0;
    }
}

void StopRegionMapLinkLandmarks(void)
{
    gRegionMapLinkLandmarkPrintState[0] = 0;
}

void StartRegionMapLinkLandmarks(void)
{
    gRegionMapLinkLandmarkPrintState[0] = 1;
    gRegionMapLinkLandmarkPrintState[1] = 0;
}

void InitPokenavMenuOptionGfx(void)
{
    gPokenavState->loadState = 0;
    if (gPokenavState->isLinkActive == 0)
        while (LoadPokenavMenuOptionGfxStep());
}

bool8 LoadPokenavMenuOptionGfxStep(void)
{
    u16 i, j;

    switch (gPokenavState->loadState)
    {
    case 0:
        for (i = 0; i < 6; i++)
        {
            for (j = 0; j < 4; j++)
                gPokenavState->menuOptionSprites[i][j] = NULL;
        }

        gPokenavState->menuOptionBlendMode = 0;
        break;
    case 1:
        LZ77UnCompWram(gPokenavMenuOptions_Gfx, gPokenavState->menuOptionsGfx);
        break;
    case 2:
        LZ77UnCompWram(gPokenavConditionMenu_Gfx, gPokenavState->conditionMenuGfx);
        break;
    case 3:
        LZ77UnCompWram(gPokenavConditionSearch_Gfx, gPokenavState->conditionSearchGfx);
        return FALSE;
    }

    gPokenavState->loadState++;
    return TRUE;
}

void InitPokenavMenuOptionSprites(u8 menuType)
{
    gPokenavState->loadState = 0;
    if (gPokenavState->isLinkActive == 0)
        while (LoadPokenavMenuOptionSpritesStep(menuType));
}

bool8 LoadPokenavMenuOptionSpritesStep(u8 menuType)
{
    u16 animNum;
    u16 topOffset;
    u16 height;
    u16 middle;
    u8 spriteId;

    switch (gPokenavState->loadState)
    {
    case 0:
    {
        LoadMenuOptionSpriteSheet(menuType);
        break;
    }
    case 1:
    {
        LoadMenuOptionSpritePalettes(menuType);
        break;
    }
    case 2:
    {
        u16 i, j;

        switch (menuType)
        {
        case 0:
            topOffset = 42;
            height = 20;
            gPokenavState->menuOptionRowCount = 5;
            break;
        case 1:
            topOffset = 56;
            height = 20;
            gPokenavState->menuOptionRowCount = 3;
            break;
        case 2:
            topOffset = 40;
            height = 16;
            gPokenavState->menuOptionRowCount = 6;
            break;
        default:
            return FALSE;
        }

        animNum = 0;
        for (i = 0; i < gPokenavState->menuOptionRowCount; i++)
        {
            middle = (height * i) + topOffset - 8;
            gPokenavState->menuOptionWin0V[i] = (middle << 8) | (middle + 0x11);
            if (!menuType)
            {
                if (gPokenavState->mainMenuItemIds[i] == 0)
                {
                    for (j = 0; j < 4; j++)
                        gPokenavState->menuOptionSprites[i][j] = NULL;
                    continue;
                }
                else
                {
                    animNum = (gPokenavState->mainMenuItemIds[i] - 1) * 4;
                }
            }

            for (j = 0; j < 4; j++)
            {
                spriteId = CreateSprite(&gPokenavMenuOptionSpriteTemplate, j * 32 + 256, (height * i) + topOffset, 0);
                if (spriteId == MAX_SPRITES)
                    return FALSE;
                gPokenavState->menuOptionSprites[i][j] = &gSprites[spriteId];
                gPokenavState->menuOptionSprites[i][j]->data[0] = i;
                gPokenavState->menuOptionSprites[i][j]->data[1] = j;
                gPokenavState->menuOptionSprites[i][j]->data[2] = j * 32 + 152;
                gPokenavState->menuOptionSprites[i][j]->data[3] = j * 32 + 256;
                StartSpriteAnim(gPokenavState->menuOptionSprites[i][j], animNum++);

                if ((menuType == 2 || menuType == 0) && i > 2)
                {
                    gPokenavState->menuOptionSprites[i][j]->oam.paletteNum = IndexOfSpritePaletteTag(0x1);
                }
            }
        }
    }
        break;
    default:
        return FALSE;
    }

    gPokenavState->loadState++;
    return TRUE;
}

void StartMenuOptionSpritesSlideIn(void)
{
    u16 i, j;
    for (i = 0; i < gPokenavState->menuOptionRowCount; i++)
    {
        for (j = 0; j < 4; j++)
        {
            if (gPokenavState->menuOptionSprites[i][j])
                gPokenavState->menuOptionSprites[i][j]->callback = SpriteCB_SlideMenuOptionIn;
        }
    }

    PlaySE(SE_WIN_OPEN);
}

bool8 UpdateMenuOptionEntryHighlight(void)
{
    if (AreMenuOptionSpriteOffsetsAtRest())
    {
        StartMenuOptionHighlight();
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

bool8 AreMenuOptionSpriteOffsetsMoving(void)
{
    return !AreMenuOptionSpriteOffsetsAtRest();
}

void StartMenuOptionSpritesSlideOut(void)
{
    u16 i, j;

    gPokenavState->menuOptionExitStep = 0;
    StopMenuOptionHighlight();
    for (i = 0; i < gPokenavState->menuOptionRowCount; i++)
    {
        if (i != gPokenavState->menuCursorPos)
        {
            for (j = 0; j < 4; j++)
            {
                if (gPokenavState->menuOptionSprites[i][j])
                    gPokenavState->menuOptionSprites[i][j]->callback = SpriteCB_SlideMenuOptionOut;
            }
        }
    }
}

bool8 UpdateMenuOptionSpritesSlideOut(void)
{
    u16 j;

    switch (gPokenavState->menuOptionExitStep)
    {
    case 0:
        if (AreOtherMenuOptionSpritesGone())
        {
            for (j = 0; j < 4; j++)
            {
                struct Sprite *sprite = gPokenavState->menuOptionSprites[gPokenavState->menuCursorPos][j];
                sprite->oam.affineMode = ST_OAM_AFFINE_DOUBLE;
                sprite->affineAnims = gPokenavMenuOptionZoomAffineAnimTable;
                InitSpriteAffineAnim(sprite);
                sprite->data[4] = j * 4 - 6;
                sprite->data[4] /= 2;
                sprite->data[5] = sprite->data[4] * 8;
                sprite->callback = SpriteCB_SlideMenuOptionOut;
            }

            StartSelectedMenuOptionFadeOut();
            gPokenavState->menuOptionExitStep++;
        }
        break;
    case 1:
        if (AreSelectedMenuOptionSpritesGone())
        {
            StopMenuOptionFadeOut();
            FreeSpriteTilesByTag(0x0);
            FreeSpritePaletteByTag(0x0);
            FreeSpritePaletteByTag(0x1);
            return FALSE;
        }
        break;
    }

    return TRUE;
}

void UpdateMenuOptionBlendRegisters(void)
{
    if (gPokenavState->menuOptionBlendMode == 1)
    {
        REG_WIN0V = gPokenavState->menuOptionWin0V[gPokenavState->menuCursorPos];
        REG_BLDY = gSineTable[gPokenavState->menuOptionBlendFrame] >> 5;
        gPokenavState->menuOptionBlendFrame += 3;
        gPokenavState->menuOptionBlendFrame &= 0x7F;
    }
    else if (gPokenavState->menuOptionBlendMode == 2)
    {
        REG_BLDALPHA = gPokenavMenuOptionBlendAlphas[gPokenavState->menuOptionBlendFrame];
        if (gPokenavState->menuOptionBlendFrame < 15)
            gPokenavState->menuOptionBlendFrame++;
    }
}

void ResetMenuOptionBlendOnPokenavExit(void)
{
    StopMenuOptionHighlight();
}

void EnableMenuOptionHighlightWindow(void)
{
    REG_WIN0H = 0x77F0;
    REG_WIN0V = gPokenavState->menuOptionWin0V[gPokenavState->menuCursorPos];
    REG_WININ = 0x3F;
    REG_WINOUT = 0x1F;
    REG_DISPCNT |= DISPCNT_WIN0_ON;
}

void DisableMenuOptionHighlightWindow(void)
{
    REG_DISPCNT &= ~DISPCNT_WIN0_ON;
}

void StartMenuOptionHighlight(void)
{
    if (!gPokenavState->menuOptionBlendMode)
    {
        gPokenavState->menuOptionBlendMode = 1;
        gPokenavState->menuOptionBlendFrame = 0;
        REG_BLDCNT = 0x90;
        REG_BLDY = 0;
        EnableMenuOptionHighlightWindow();
    }
}

void StopMenuOptionHighlight(void)
{
    gPokenavState->menuOptionBlendMode = 0;
    REG_BLDCNT = 0;
    DisableMenuOptionHighlightWindow();
}

void StartSelectedMenuOptionFadeOut(void)
{
    u16 j;

    if (!gPokenavState->menuOptionBlendMode)
    {
        DisableMenuOptionHighlightWindow();
        for (j = 0; j < 4; j++)
        {
            struct Sprite *sprite = gPokenavState->menuOptionSprites[gPokenavState->menuCursorPos][j];
            sprite->oam.objMode = ST_OAM_OBJ_BLEND;
        }

        gPokenavState->menuOptionBlendMode = 2;
        gPokenavState->menuOptionBlendFrame = 0;
        REG_BLDCNT = 0x3F40;
        REG_BLDALPHA = 0x10;
    }
}

void StopMenuOptionFadeOut()
{
    gPokenavState->menuOptionBlendMode = 0;
    REG_BLDCNT = 0;
}

void SpriteCB_SlideMenuOptionIn(struct Sprite *sprite)
{
    sprite->x -= 8;
    if (sprite->x <= sprite->data[2])
    {
        sprite->x = sprite->data[2];
        sprite->callback = SpriteCB_UpdateMenuOptionSelectionOffset;
    }
}

void SpriteCB_SlideMenuOptionOut(struct Sprite *sprite)
{
    if (sprite->data[0] == gPokenavState->menuCursorPos)
    {
        if (sprite->data[5])
        {
            sprite->x += sprite->data[4];
            sprite->data[5] -= sprite->data[4];
        }

        if (sprite->affineAnimEnded)
            DestroyMenuOptionSprite(sprite);
    }
    else
    {
        sprite->x += 8;
        if (sprite->x >= sprite->data[3])
            DestroyMenuOptionSprite(sprite);
    }
}

void SpriteCB_UpdateMenuOptionSelectionOffset(struct Sprite *sprite)
{
    if (sprite->data[0] == gPokenavState->menuCursorPos)
    {
        if (sprite->x2 > -16)
            sprite->x2 -= 4;
    }
    else
    {
        if (sprite->x2 < 0)
            sprite->x2 += 4;
    }
}

bool8 AreMenuOptionSpriteOffsetsAtRest(void)
{
    u16 i, j;

    for (i = 0; i < gPokenavState->menuItemCount; i++)
    {
        for (j = 0; j < 4; j++)
        {
            struct Sprite *sprite = gPokenavState->menuOptionSprites[i][j];
            if (!sprite)
                return TRUE;

            if (sprite->x2 != 0 && sprite->x2 != -16)
                return FALSE;
        }
    }

    return TRUE;
}

bool8 AreOtherMenuOptionSpritesGone(void)
{
    u16 i, j;

    for (i = 0; i < gPokenavState->menuOptionRowCount; i++)
    {
        if (i != gPokenavState->menuCursorPos)
        {
            for (j = 0; j < 4; j++)
            {
                struct Sprite *sprite = gPokenavState->menuOptionSprites[i][j];
                if (sprite)
                    return FALSE;
            }
        }
    }

    return TRUE;
}

bool8 AreSelectedMenuOptionSpritesGone(void)
{
    u16 j;

    for (j = 0; j < 4; j++)
    {
        struct Sprite *sprite = gPokenavState->menuOptionSprites[gPokenavState->menuCursorPos][j];
        if (sprite)
            return FALSE;
    }

    return TRUE;
}

void DestroyMenuOptionSprite(struct Sprite *sprite)
{
    gPokenavState->menuOptionSprites[sprite->data[0]][sprite->data[1]] = NULL;
    if (sprite->affineAnimEnded)
        FreeOamMatrix(sprite->oam.matrixNum);

    DestroySprite(sprite);
}

void LoadMenuOptionSpriteSheet(u8 menuType)
{
    switch (menuType)
    {
    case 0:
        gPokenavState->menuOptionSpriteSheet.data = gPokenavState->menuOptionsGfx;
        gPokenavState->menuOptionSpriteSheet.size = sizeof(gPokenavState->menuOptionsGfx);
        gPokenavState->menuOptionSpriteSheet.tag = 0x0;
        break;
    case 1:
        gPokenavState->menuOptionSpriteSheet.data = gPokenavState->conditionMenuGfx;
        gPokenavState->menuOptionSpriteSheet.size = sizeof(gPokenavState->conditionMenuGfx);
        gPokenavState->menuOptionSpriteSheet.tag = 0x0;
        break;
    case 2:
        gPokenavState->menuOptionSpriteSheet.data = gPokenavState->conditionSearchGfx;
        gPokenavState->menuOptionSpriteSheet.size = sizeof(gPokenavState->conditionSearchGfx);
        gPokenavState->menuOptionSpriteSheet.tag = 0x0;
        break;
    default:
        return;
    }

    LoadSpriteSheet(&gPokenavState->menuOptionSpriteSheet);
}

void LoadMenuOptionSpritePalettes(u8 menuType)
{
    struct SpritePalette spritePalette;

    switch (menuType)
    {
    case 0:
        spritePalette.data = gPokenavMenuOptions1_Pal;
        spritePalette.tag = 0;
        LoadSpritePalette(&spritePalette);
        spritePalette.data = gPokenavMenuOptions2_Pal;
        spritePalette.tag = 0x1;
        break;
    case 1:
        spritePalette.data = gPokenavConditionMenu_Pal;
        spritePalette.tag = 0x0;
        break;
    case 2:
        spritePalette.data = gPokenavCondition6_Pal;
        spritePalette.tag = 0;
        LoadSpritePalette(&spritePalette);
        spritePalette.data = gPokenavCondition7_Pal;
        spritePalette.tag = 0x1;
        break;
    default:
        return;
    }

    LoadSpritePalette(&spritePalette);
}

void CreateRematchBlueLightSprite(void)
{
    u8 spriteId;

    gPokenavState->blueLightSprite = NULL;
    if (DoesSomeoneWantRematchIn(gSaveBlock1.location.mapGroup, gSaveBlock1.location.mapNum) == TRUE)
    {
        LoadSpriteSheet(&gSpriteSheet_PokenavBlueLight);
        LoadSpritePalette(&gSpritePalette_PokenavBlueLight);
        spriteId = CreateSprite(&gPokenavBlueLightSpriteTemplate, 12, 96, 0);
        if (spriteId != MAX_SPRITES)
        {
            gPokenavState->blueLightSprite = &gSprites[spriteId];
        }
        else
        {
            FreeSpriteTilesByTag(0x19);
            FreeSpritePaletteByTag(0x11);
        }
    }
}

void DestroyRematchBlueLightSprite(void)
{
    if (gPokenavState->blueLightSprite)
    {
        DestroySprite(gPokenavState->blueLightSprite);
        FreeSpriteTilesByTag(0x19);
        FreeSpritePaletteByTag(0x11);
        gPokenavState->blueLightSprite = NULL;
    }
}

void SpriteCB_BlinkingBlueLight(struct Sprite *sprite)
{
    if (++sprite->data[0] > 6)
    {
        sprite->data[0] = 0;
        sprite->invisible = !sprite->invisible;
    }
}

void InitPokenavMenuHeaderGfx(void)
{
    gPokenavState->loadState = 0;
    if (!gPokenavState->isLinkActive)
        while(LoadPokenavMenuHeaderGfxStep());
}

bool8 LoadPokenavMenuHeaderGfxStep(void)
{
    switch (gPokenavState->loadState)
    {
    case 0:
        LZ77UnCompWram(gPokenavMainMenuHeader_Gfx, gPokenavState->mainMenuHeaderGfx);
        break;
    case 1:
        LZ77UnCompWram(gPokenavConditionMenuHeader_Gfx, gPokenavState->conditionMenuHeaderGfx);
        break;
    case 2:
        LZ77UnCompWram(gPokenavRibbonsHeader_Gfx, gPokenavState->ribbonsHeaderGfx);
        break;
    case 3:
        LZ77UnCompWram(gPokenavHoennMapHeader_Gfx, gPokenavState->hoennMapHeaderGfx);
        break;
    case 4:
        LZ77UnCompWram(gPokenavConditionHeaderLabels_Gfx, gPokenavState->searchHeaderGfx);
        break;
    case 5:
        LZ77UnCompWram(gPokenavConditionHeaderLabels2_Gfx, gPokenavState->smartHeaderGfx);
        break;
    case 6:
        LZ77UnCompWram(gPokenavTrainersEyesHeader_Gfx, gPokenavState->trainersEyesHeaderGfx);
        break;
    case 7:
        LoadSpritePalettes(gPokenavMenuHeaderSpritePalettes);
        break;
    default:
        return FALSE;
    }

    gPokenavState->loadState++;
    return TRUE;
}

void LoadPokenavMenuHeaderSpriteSheet(u8 headerType)
{
    struct SpriteSheet spriteSheet;

    switch (headerType)
    {
    case 0:
        spriteSheet.data = gPokenavState->mainMenuHeaderGfx;
        spriteSheet.size = sizeof(gPokenavState->mainMenuHeaderGfx);
        spriteSheet.tag = 0x1;
        break;
    case 1:
        spriteSheet.data = gPokenavState->conditionMenuHeaderGfx;
        spriteSheet.size = sizeof(gPokenavState->conditionMenuHeaderGfx);
        spriteSheet.tag = 0x1;
        break;
    case 3:
        spriteSheet.data = gPokenavState->trainersEyesHeaderGfx;
        spriteSheet.size = sizeof(gPokenavState->trainersEyesHeaderGfx);
        spriteSheet.tag = 0x1;
        break;
    case 2:
        spriteSheet.data = gPokenavState->ribbonsHeaderGfx;
        spriteSheet.size = sizeof(gPokenavState->ribbonsHeaderGfx);
        spriteSheet.tag = 0x1;
        break;
    case 4:
        spriteSheet.data = gPokenavState->hoennMapHeaderGfx;
        spriteSheet.size = sizeof(gPokenavState->hoennMapHeaderGfx);
        spriteSheet.tag = 0x1;
        break;
    case 5:
        spriteSheet.data = gPokenavState->searchHeaderGfx;
        spriteSheet.size = sizeof(gPokenavState->searchHeaderGfx);
        spriteSheet.tag = 0x2;
        break;
    case 6:
        spriteSheet.data = gPokenavState->partyHeaderGfx;
        spriteSheet.size = sizeof(gPokenavState->partyHeaderGfx);
        spriteSheet.tag = 0x2;
        break;
    case 8:
        spriteSheet.data = gPokenavState->beautyHeaderGfx;
        spriteSheet.size = sizeof(gPokenavState->beautyHeaderGfx);
        spriteSheet.tag = 0x2;
        break;
    case 9:
        spriteSheet.data = gPokenavState->cuteHeaderGfx;
        spriteSheet.size = sizeof(gPokenavState->cuteHeaderGfx);
        spriteSheet.tag = 0x2;
        break;
    case 11:
        spriteSheet.data = gPokenavState->toughHeaderGfx;
        spriteSheet.size = sizeof(gPokenavState->toughHeaderGfx);
        spriteSheet.tag = 0x2;
        break;
    case 10:
        spriteSheet.data = gPokenavState->smartHeaderGfx;
        spriteSheet.size = sizeof(gPokenavState->smartHeaderGfx);
        spriteSheet.tag = 0x2;
        break;
    case 7:
        spriteSheet.data = gPokenavState->coolHeaderGfx;
        spriteSheet.size = sizeof(gPokenavState->coolHeaderGfx);
        spriteSheet.tag = 0x2;
        break;
    default:
        return;
    }

    LoadSpriteSheet(&spriteSheet);
}

void CreatePokenavLeftHeaderSprites(u8 headerType)
{
    u16 i;
    s16 deltaX, endX, initialX;
    u16 y;
    s16 width;
    struct Sprite **sprites;
    const struct SpriteTemplate *spriteTemplate;
    u8 spriteId;

    spriteTemplate = NULL;
    switch (headerType)
    {
    case 0:
    case 1:
    case 2:
    case 3:
        initialX = -96;
        y = 49 - gPokenavState->menuVerticalOffset;
        deltaX = 8;
        endX = 32;
        width = 64;
        sprites = gPokenavState->largeHeaderSprites;
        spriteTemplate = headerType != 3 ? &gPokenavLargeHeaderSpriteTemplate : &gPokenavTrainersEyesHeaderSpriteTemplate;
        break;
    case 4:
        initialX = 272;
        y = 49 - gPokenavState->menuVerticalOffset;
        deltaX = -8;
        endX = 152;
        width = 64;
        spriteTemplate = &gPokenavLargeHeaderSpriteTemplate;
        sprites = gPokenavState->largeHeaderSprites;
        break;
    case 5:
    case 6:
    case 8:
    case 9:
        spriteTemplate = &gPokenavSmallHeaderSpriteTemplate;
        // fall through
    case 7:
    case 10:
    case 11:
        if (spriteTemplate == NULL)
            spriteTemplate = &gPokenavSmallHeaderAltSpriteTemplate;

        initialX = -96;
        y = 68 - gPokenavState->menuVerticalOffset;
        deltaX = 8;
        endX = 16;
        width = 32;
        sprites = gPokenavState->smallHeaderSprites;
        break;
    default:
        return;
    }

    for (i = 0; i < 2; i++)
    {
        spriteId = CreateSprite(spriteTemplate, i * width + initialX, y, 0);
        if (spriteId != MAX_SPRITES)
        {
            gSprites[spriteId].data[0] = deltaX;
            gSprites[spriteId].data[1] =  endX + i * width;
            gSprites[spriteId].data[2] = i;
            gSprites[spriteId].data[3] = headerType;
            if (headerType == 4 && i == 1)
            {
                int anim = !gPokenavState->regionMap.zoomed ? 1 : 2;
                StartSpriteAnim(&gSprites[spriteId], anim);
            }
            else
            {
                StartSpriteAnim(&gSprites[spriteId], i);
            }

            if (headerType < 4 && i == 1)
            {
                gSprites[spriteId].oam.shape = ST_OAM_SQUARE;
                gSprites[spriteId].oam.size = 2;
            }

            sprites[i] = &gSprites[spriteId];
        }
    }
}

#define sEndX sprite->data[1]
void SpriteCB_SlideLeftHeaderIn(struct Sprite *sprite)
{
    s16 x = sprite->x;
    sprite->x += sprite->data[0];
    if ((x <= sEndX && sprite->x >= sEndX) || (x >= sEndX && sprite->x <= sEndX))
    {
        sprite->x = sEndX;
        if (sprite->data[3] == 4 && sprite->data[2] == 1)
            sprite->callback = SpriteCB_UpdateRegionMapHeaderZoom;
        else
            sprite->callback = SpriteCallbackDummy;
    }
}
#undef sEndX

void SpriteCB_SlideLeftHeaderOut(struct Sprite *sprite)
{
    u16 right;

    sprite->x -= sprite->data[0];
    right = sprite->x + 32;
    if (right > 304)
    {
        if (sprite->data[2] == 1)
        {
            if (sprite->data[3] < 5)
                FreeSpriteTilesByTag(0x1);
            else
                FreeSpriteTilesByTag(0x2);
        }

        DestroySprite(sprite);
    }
}

void SpriteCB_UpdateRegionMapHeaderZoom(struct Sprite *sprite)
{
    int anim = !gPokenavState->regionMap.zoomed ? 1 : 2;
    StartSpriteAnim(sprite, anim);
}

void BeginPokenavLeftHeaderLoad(u8 headerType)
{
    gPokenavState->loadState = 0;
    if (!gPokenavState->isLinkActive)
        while (LoadPokenavLeftHeaderStep(headerType));
}

bool8 LoadPokenavLeftHeaderStep(u8 headerType)
{
    switch (gPokenavState->loadState)
    {
    case 0:
        LoadPokenavMenuHeaderSpriteSheet(headerType);
        gPokenavState->loadState++;
        return TRUE;
    case 1:
        CreatePokenavLeftHeaderSprites(headerType);
        gPokenavState->loadState++;
        return FALSE;
    default:
        return FALSE;
    }
}

void StartPokenavLeftHeaderSlideOut(u8 headerType)
{
    u16 i;

    if (headerType < 5)
    {
        for (i = 0; i < 2; i++)
            gPokenavState->largeHeaderSprites[i]->callback = SpriteCB_SlideLeftHeaderOut;
    }
    else
    {
        for (i = 0; i < 2; i++)
            gPokenavState->smallHeaderSprites[i]->callback = SpriteCB_SlideLeftHeaderOut;
    }
}

void DestroyPokenavLeftHeaderSprites(u8 headerType)
{
    u16 i;

    if (headerType < 5)
    {
        FreeSpriteTilesByTag(0x1);
        for (i = 0; i < 2; i++)
            DestroySprite(gPokenavState->largeHeaderSprites[i]);
    }
    else
    {
        FreeSpriteTilesByTag(0x2);
        for (i = 0; i < 2; i++)
            DestroySprite(gPokenavState->smallHeaderSprites[i]);
    }
}

void CreatePokenavRegionMapIcons(void)
{
    CreateRegionMapCursor(7, 7);
    CreateRegionMapPlayerIcon(8, 8);
    TrySetPlayerIconBlink();
}

void FreePokenavRegionMapIcons(void)
{
    FreeRegionMapIconResources();
}

void SpriteCB_UpdatePokenavPortraitSpriteX(struct Sprite *sprite)
{
    sprite->x = gPokenavState->monInfoHeaderXOffset + 38;
}

void CreateOrUpdatePokenavPortraitSprite(u8 portraitSlot)
{
    u8 spriteId;
    struct SpriteTemplate spriteTemplate;
    struct SpritePalette spritePalette;
    struct SpriteSheet spriteSheet;

    if (!gPokenavState->portraitSprite)
    {
        spriteSheet = gPokenavPortraitSpriteSheet;
        spriteTemplate = gPokenavPortraitSpriteTemplate;
        spritePalette = gPokenavPortraitSpritePalette;

        spriteSheet.data = gPokenavState->spriteGfxBuffers[portraitSlot];
        spritePalette.data = gPokenavState->portraitPaletteBuffers[portraitSlot];
        gPokenavState->portraitPaletteOffset = LoadSpritePalette(&spritePalette);
        gPokenavState->portraitTileStart = LoadSpriteSheet(&spriteSheet);

        spriteId = CreateSprite(&spriteTemplate, 38, 104, 0);
        if (spriteId == MAX_SPRITES)
        {
            FreeSpriteTilesByTag(0x6);
            FreeSpritePaletteByTag(0x6);
            gPokenavState->portraitSprite = NULL;
        }
        else
        {
            gPokenavState->portraitSprite = &gSprites[spriteId];
            gPokenavState->portraitVramDest = (void *)(VRAM + 0x10000) + gPokenavState->portraitTileStart * 32;
            gPokenavState->portraitPaletteOffset = gPokenavState->portraitPaletteOffset * 16 + 0x100;
        }
    }
    else
    {
        DmaCopy16Defvars(3, gPokenavState->spriteGfxBuffers[portraitSlot], gPokenavState->portraitVramDest, 0x800);
        LoadPalette(gPokenavState->portraitPaletteBuffers[portraitSlot], gPokenavState->portraitPaletteOffset, 0x20);
    }
}

void DestroyPokenavPortraitSprite(void)
{
    if (gPokenavState->portraitSprite)
    {
        DestroySprite(gPokenavState->portraitSprite);
        FreeSpriteTilesByTag(0x6);
        FreeSpritePaletteByTag(0x6);
        gPokenavState->portraitSprite = NULL;
    }
}

void CreateOrUpdateTrainerEyesPortrait(u8 portraitSlot)
{
    CreateOrUpdatePokenavPortraitSprite(portraitSlot);
    gPokenavState->trainerEyesPortraitSprite = gPokenavState->portraitSprite;
    gPokenavState->trainerEyesPortraitSprite->callback = SpriteCB_UpdateTrainerEyesPortraitPosition;
}

void DestroyTrainerEyesPortrait(void)
{
    if (gPokenavState->trainerEyesPortraitSprite)
    {
        DestroySprite(gPokenavState->trainerEyesPortraitSprite);
        FreeSpriteTilesByTag(0x6);
        FreeSpritePaletteByTag(0x6);
        gPokenavState->trainerEyesPortraitSprite = NULL;
        gPokenavState->portraitSprite = NULL;
    }
}

void SpriteCB_UpdateTrainerEyesPortraitPosition(struct Sprite *sprite)
{
    sprite->x = gPokenavState->trainerEyesPortraitXOffset + 40;
    sprite->y = 104;
}

void CreatePokenavListArrowSprites(u8 paletteType)
{
    u16 i;
    u8 spriteId;
    struct SpritePalette spritePalette;
    struct SpriteSheet spriteSheets[3];

    memcpy(spriteSheets, gPokenavListArrowSpriteSheets, sizeof(gPokenavListArrowSpriteSheets));
    spritePalette = gPokenavListArrowSpritePalette;
    switch (paletteType)
    {
    case 1:
    case 2:
        spritePalette.data = gPokenavListArrowAltPalette;
        break;
    }

    LoadSpriteSheets(spriteSheets);
    LoadSpritePalette(&spritePalette);
    spriteId = CreateSprite(&gPokenavListRightArrowSpriteTemplate, 95, 0, 0);
    if (spriteId == MAX_SPRITES)
    {
        gPokenavState->listRightArrowSprite = NULL;
    }
    else
    {
        gPokenavState->listRightArrowSprite = &gSprites[spriteId];
        for (i = 0; i < 2; i++)
        {
            spriteId = CreateSprite(&gPokenavListUpDownArrowSpriteTemplate, 168, i * 128 + 8, 0);
            if (spriteId != MAX_SPRITES)
            {
                gPokenavState->listUpDownArrowSprites[i] = &gSprites[spriteId];
                gSprites[spriteId].invisible = TRUE;
                gSprites[spriteId].data[0] = 0;
                gSprites[spriteId].data[1] = 0;
                gSprites[spriteId].data[2] = i == 0 ? -1 : 1;
                gSprites[spriteId].data[3] = i;
                gSprites[spriteId].data[4] = 1;
                StartSpriteAnim(&gSprites[spriteId], i);
            }
            else
            {
                gPokenavState->listUpDownArrowSprites[i] = NULL;
            }
        }
    }
}

void DestroyPokenavListArrowSprites(void)
{
    u16 i;

    if (gPokenavState->listRightArrowSprite)
    {
        DestroySprite(gPokenavState->listRightArrowSprite);
        FreeSpriteTilesByTag(0x9);
        FreeSpritePaletteByTag(0x9);
        gPokenavState->listRightArrowSprite = NULL;
    }

    for (i = 0; i < 2; i++)
    {
        if (gPokenavState->listUpDownArrowSprites[i])
        {
            DestroySprite(gPokenavState->listUpDownArrowSprites[i]);
            gPokenavState->listUpDownArrowSprites[i] = NULL;
        }
    }

    FreeSpriteTilesByTag(0xA);
}

void SpriteCB_UpdatePokenavListRightArrow(struct Sprite *sprite)
{
    sprite->y = gPokenavState->listCursorRow * 16 + 16;
}

void SpriteCB_UpdatePokenavListUpDownArrow(struct Sprite *sprite)
{
    if (gPokenavState->hasListScrollArrows)
    {
        if (sprite->data[4])
        {
            if (!sprite->data[3])
                sprite->invisible = gPokenavState->firstVisibleListIndex == 0;
            else
                sprite->invisible = gPokenavState->lastVisibleListIndex == gPokenavState->lastListIndex;

            sprite->data[4] = 0;
        }

        if (++sprite->data[0] > 4)
        {
            sprite->data[0] = 0;
            if (++sprite->data[1] < 5)
            {
                sprite->y2 += sprite->data[2];
            }
            else
            {
                sprite->data[1] = 0;
                sprite->y2 = 0;
            }
        }
    }
}

void RefreshPokenavListArrowVisibility(void)
{
    u16 i;

    for (i = 0; i < 2; i++)
    {
        if (gPokenavState->listUpDownArrowSprites[i])
            gPokenavState->listUpDownArrowSprites[i]->data[4] = 1;
    }
}

void TogglePokenavListArrows(u8 invisible)
{
    gPokenavState->listRightArrowSprite->invisible = invisible;
    if (gPokenavState->hasListScrollArrows)
    {
        if (invisible == 1)
        {
            gPokenavState->listUpDownArrowSprites[0]->invisible = invisible;
            gPokenavState->listUpDownArrowSprites[1]->invisible = invisible;
        }
        else
        {
            gPokenavState->listUpDownArrowSprites[0]->data[4] = 1;
            gPokenavState->listUpDownArrowSprites[1]->data[4] = 1;
        }
    }
}

void SpriteCB_ConditionPartyPokeball(struct Sprite *sprite)
{
    if (sprite->data[0] == gPokenavState->detailSelectionIndex)
        StartSpriteAnim(sprite, 0);
    else
        StartSpriteAnim(sprite, 1);
}

void HighlightCurrentPartyIndexPokeball(struct Sprite *sprite)
{
    if (gPokenavState->detailSelectionIndex == gPokenavState->conditionGraphMonCount - 1)
        sprite->oam.paletteNum = IndexOfSpritePaletteTag(0x4);
    else
        sprite->oam.paletteNum = IndexOfSpritePaletteTag(0x5);
}

void CreateConditionPartyPokeballIndicators(void)
{
    u16 i;
    u8 spriteId;
    struct SpriteSheet spriteSheets[4];
    struct SpritePalette spritePalettes[3];
    struct SpriteTemplate spriteTemplate;

    memcpy(spriteSheets, gPokenavConditionSelectionIconSheets, sizeof(gPokenavConditionSelectionIconSheets));
    memcpy(spritePalettes, gPokenavConditionSelectionIconPalettes, sizeof(gPokenavConditionSelectionIconPalettes));
    spriteTemplate = gPokenavConditionSelectionIconTemplate;
    LoadSpriteSheets(spriteSheets);
    LoadSpritePalettes(spritePalettes);

    for (i = 0; i < gPokenavState->conditionGraphMonCount - 1; i++)
    {
        spriteId = CreateSprite(&spriteTemplate, 226, i * 20 + 8, 0);
        if (spriteId != MAX_SPRITES)
        {
            gPokenavState->conditionPartyIconSprites[i] = &gSprites[spriteId];
            gPokenavState->conditionPartyIconSprites[i]->data[0] = i;
        }
        else
        {
            gPokenavState->conditionPartyIconSprites[i] = NULL;
        }
    }

    spriteTemplate.tileTag = 0x4;
    spriteTemplate.callback = SpriteCallbackDummy;
    for (; i < 6; i++)
    {
        spriteId = CreateSprite(&spriteTemplate, 230, i * 20 + 8, 0);
        if (spriteId != MAX_SPRITES)
        {
            gPokenavState->conditionPartyIconSprites[i] = &gSprites[spriteId];
            gPokenavState->conditionPartyIconSprites[i]->oam.size = 0;
        }
        else
        {
            gPokenavState->conditionPartyIconSprites[i] = NULL;
        }
    }

    spriteTemplate.tileTag = 0x5;
    spriteTemplate.callback = HighlightCurrentPartyIndexPokeball;
    spriteId = CreateSprite(&spriteTemplate, 222, i * 20 + 8, 0);
    if (spriteId != MAX_SPRITES)
    {
        gPokenavState->conditionPartyIconSprites[i] = &gSprites[spriteId];
        gPokenavState->conditionPartyIconSprites[i]->oam.shape = ST_OAM_H_RECTANGLE;
        gPokenavState->conditionPartyIconSprites[i]->oam.size = 2;
    }
    else
    {
        gPokenavState->conditionPartyIconSprites[i] = NULL;
    }
}

void DestroyConditionPartyPokeballIndicators(void)
{
    u16 i;

    for (i = 0; i < 7; i++)
    {
        if (gPokenavState->conditionPartyIconSprites[i])
        {
            DestroySprite(gPokenavState->conditionPartyIconSprites[i]);
            gPokenavState->conditionPartyIconSprites[i] = NULL;
        }
    }
}

void CreateConditionMonMarkingsSprite(void)
{
    struct Sprite *sprite;

    gPokenavState->conditionMarkingsMenu.baseTileTag = 0x1C;
    gPokenavState->conditionMarkingsMenu.basePaletteTag = 0x13;
    InitMonMarkingsMenu(&gPokenavState->conditionMarkingsMenu);
    BufferMonMarkingsMenuTiles();
    sprite = CreateMonMarkingAllCombosSprite(27, 21, gPokenavConditionMonMarkingsPalette);
    sprite->oam.priority = 3;
    sprite->x = 192;
    sprite->y = 32;
    sprite->callback = MonMarkingsCallback;
    gPokenavState->conditionMonMarkingsSprite = sprite;
}

void DestroyConditionMonMarkingsSprite(void)
{
    DestroySprite(gPokenavState->conditionMonMarkingsSprite);
    FreeSpriteTilesByTag(0x1B);
    FreeSpritePaletteByTag(0x15);
}

void MonMarkingsCallback(struct Sprite *sprite)
{
    StartSpriteAnim(sprite, gPokenavState->conditionMonMarkings[gPokenavState->portraitSlot]);
}

void OpenConditionMonMarkingsMenu(void)
{
    OpenMonMarkingsMenu(gPokenavState->conditionMonMarkings[gPokenavState->portraitSlot], 176, 32);
}

void SaveAndCloseConditionMonMarkingsMenu(void)
{
    struct PokenavMonListItem *selectedMon = &gPokenavState->monList[gPokenavState->detailSelectionIndex];
    gPokenavState->conditionMonMarkings[gPokenavState->portraitSlot] = gPokenavState->conditionMarkingsMenu.markings;
    SetMonMarkings(selectedMon->boxId, selectedMon->monId, gPokenavState->conditionMarkingsMenu.markings);
    FreeMonMarkingsMenu();
}

void InitRibbonIconGfx(void)
{
    gPokenavState->loadState = 0;
    if (!gPokenavState->isLinkActive)
        while (LoadRibbonIconGfxStep());
}

bool8 LoadRibbonIconGfxStep(void)
{
    switch (gPokenavState->loadState)
    {
    case 0:
        LZ77UnCompWram(gPokenavRibbonIconsBigHalfGfx, gPokenavState->ribbonIconHalfGfx);
        break;
    case 1:
        InitRibbonIconExpansion();
        gPokenavState->loadState++;
        // fall through
    case 2:
        if (ExpandRibbonIconGfxStep())
            return TRUE;
        break;
    default:
        return FALSE;
    }

    gPokenavState->loadState++;
    return TRUE;
}

void InitRibbonIconExpansion(void)
{
    gPokenavState->ribbonIconLoadIndex = 0;
    if (!gPokenavState->isLinkActive)
        while (ExpandRibbonIconGfxStep());
}

bool8 ExpandRibbonIconGfxStep(void)
{
    u16 i;
    u8 j, k, l, m;

    if (gPokenavState->ribbonIconLoadIndex > 11)
        return FALSE;

    for (i = 0; i < 2; i++)
    {
        u8 *src = &gPokenavState->ribbonIconHalfGfx[gPokenavState->ribbonIconLoadIndex][0];
        u8 *dst = &gPokenavState->ribbonIconBigGfx[gPokenavState->ribbonIconLoadIndex][0];
        for (j = 0; j < 4; j++)
        {
            CpuFastSet(src, dst, 0x10);
            dst += 0x40;
            src += 0x20;
            for (k = 0; k < 2; k++)
            {
                for (l = 0; l < 8; l++)
                {
                    src += 4;
                    for (m = 0; m < 4; m++)
                    {
                        src -= 1;
                        *dst = (*src << 4) | ((*src >> 4) & 0xF);
                        dst++;
                    }

                    src += 4;
                }

                src -= 0x40;
            }

            src += 0x60;
        }

        if (++gPokenavState->ribbonIconLoadIndex > 11)
            return FALSE;
    }

    if (gPokenavState->ribbonIconLoadIndex > 11)
        return FALSE;

    return TRUE;
}

void InitRibbonsSummarySpriteGfx(void)
{
    gPokenavState->loadState = 0;
    if (!gPokenavState->isLinkActive)
        while (LoadRibbonsSummarySpriteGfxStep());
}

bool8 LoadRibbonsSummarySpriteGfxStep(void)
{
    switch (gPokenavState->loadState)
    {
    case 0:
        gPokenavState->bigRibbonSprite = NULL;
        gPokenavState->isBigRibbonAnimating = 0;
        break;
    case 1:
        CreateOrUpdatePokenavPortraitSprite(0);
        break;
    case 2:
        InitRibbonIconSpriteSheets();
        gPokenavState->loadState++;
        // fall through
    case 3:
        if (LoadRibbonIconSpriteSheetStep())
            return TRUE;
        break;
    default:
        return FALSE;
    }

    gPokenavState->loadState++;
    return TRUE;
}

void InitRibbonIconSpriteSheets(void)
{
    gPokenavState->ribbonIconLoadIndex = 0;
    if (!gPokenavState->isLinkActive)
        while (LoadRibbonIconSpriteSheetStep());
}

bool8 LoadRibbonIconSpriteSheetStep(void)
{
    struct SpriteSheet spriteSheet;
    struct SpritePalette spritePalette;
    if (gPokenavState->ribbonIconLoadIndex > 11)
        return FALSE;

    spriteSheet.data = &gPokenavState->ribbonIconBigGfx[gPokenavState->ribbonIconLoadIndex][0];
    spriteSheet.size = 0x200;
    spriteSheet.tag = gPokenavState->ribbonIconLoadIndex + 11;
    LoadSpriteSheet(&spriteSheet);
    if (gPokenavState->ribbonIconLoadIndex < 5)
    {
        spritePalette.data = gPokenavRibbonIconPalettes[gPokenavState->ribbonIconLoadIndex];
        spritePalette.tag = gPokenavState->ribbonIconLoadIndex + 10;
        LoadSpritePalette(&spritePalette);
    }

    if (++gPokenavState->ribbonIconLoadIndex > 11)
        return FALSE;

    return TRUE;
}

enum {
    RIBBONANIM_NORMAL,
    RIBBONANIM_ZOOM_IN,
    RIBBONANIM_ZOOM_OUT,
};

struct Sprite *CreateBigRibbonSprite(u16 column, u16 row)
{
    struct SpriteTemplate spriteTemplate;
    u16 ribbonIndex;
    u8 ribbonId;
    u8 spriteId;

    if (row < 3)
        ribbonIndex = column + row * 9;
    else
        ribbonIndex = column + (gPokenavState->ribbonCount - gPokenavState->giftRibbonCount);

    ribbonId = gPokenavState->ribbonIds[ribbonIndex];
    spriteTemplate = gPokenavBigRibbonSpriteTemplate;
    spriteTemplate.tileTag = gPokenavRibbonGfxData[ribbonId][0] + 11;
    spriteTemplate.paletteTag = gPokenavRibbonGfxData[ribbonId][1] + 10;
    spriteId = CreateSprite(&spriteTemplate, column * 16 + 96, row * 16 + 40, 2);
    if (spriteId != MAX_SPRITES)
        return &gSprites[spriteId];
    else
        return NULL;
}

void CreateAndZoomInSelectedRibbon(void)
{
    gPokenavState->bigRibbonSprite = CreateBigRibbonSprite(gPokenavState->ribbonCursorPos, gPokenavState->ribbonPageIndex);
    if (gPokenavState->bigRibbonSprite)
    {
        StartSpriteAffineAnim(gPokenavState->bigRibbonSprite, RIBBONANIM_ZOOM_IN);
        gPokenavState->isBigRibbonAnimating = 1;
    }
    else
    {
        gPokenavState->isBigRibbonAnimating = 0;
    }
}

bool8 IsRibbonZoomInAnimating(void)
{
    if (gPokenavState->isBigRibbonAnimating)
    {
        gPokenavState->isBigRibbonAnimating = !gPokenavState->bigRibbonSprite->affineAnimEnded;
        return gPokenavState->isBigRibbonAnimating;
    }
    else
    {
        return FALSE;
    }
}

void ZoomOutSelectedRibbon(void)
{
    if (gPokenavState->bigRibbonSprite)
    {
        StartSpriteAffineAnim(gPokenavState->bigRibbonSprite, RIBBONANIM_ZOOM_OUT);
        gPokenavState->isBigRibbonAnimating = 1;
    }
    else
    {
        gPokenavState->isBigRibbonAnimating = 0;
    }
}

bool8 UpdateRibbonZoomOutAndDestroySprite(void)
{
    if (gPokenavState->isBigRibbonAnimating)
    {
        gPokenavState->isBigRibbonAnimating = !gPokenavState->bigRibbonSprite->affineAnimEnded;
        if (!gPokenavState->isBigRibbonAnimating)
        {
            FreeOamMatrix(gPokenavState->bigRibbonSprite->oam.matrixNum);
            DestroySprite(gPokenavState->bigRibbonSprite);
            gPokenavState->bigRibbonSprite = NULL;
        }

        return gPokenavState->isBigRibbonAnimating;
    }
    else
    {
        return FALSE;
    }
}

void FreeRibbonsSummarySprites(void)
{
    u16 i;

    if (gPokenavState->bigRibbonSprite)
    {
        FreeOamMatrix(gPokenavState->bigRibbonSprite->oam.matrixNum);
        DestroySprite(gPokenavState->bigRibbonSprite);
        gPokenavState->bigRibbonSprite = NULL;
    }

    for (i = 0; i < 12; i++)
        FreeSpriteTilesByTag(i + 0xB);

    for (i = 0; i < 5; i++)
        FreeSpritePaletteByTag(i + 0xA);

    DestroyPokenavPortraitSprite();
}

void LoadConditionSparkles(void)
{
    u16 i;
    struct SpriteSheet spriteSheet;
    struct SpritePalette spritePalette;

    spriteSheet = gPokenavConditionSparkleSpriteSheet;
    spritePalette = gPokenavConditionSparkleSpritePalette;
    LoadSpriteSheet(&spriteSheet);
    LoadSpritePalette(&spritePalette);
    for (i = 0; i < 10; i++)
        gPokenavState->conditionSparkleSprites[i] = NULL;
}

void FreeConditionSparkles(void)
{
    DestroyConditionSparkleSprites();
    FreeSpriteTilesByTag(0x17);
    FreeSpritePaletteByTag(0xF);
}

void CreateConditionSparkleSprites(void)
{
    u8 spriteId;
    u16 i;
    u8 count;
    struct PokenavMonListItem *mon = &gPokenavState->monList[gPokenavState->detailSelectionIndex];

    if (!mon->isValid)
        return;

    count = gPokenavState->conditionNumSparkles[gPokenavState->portraitSlot];
    for (i = 0; i < count + 1; i++)
    {
        spriteId = CreateSprite(&gPokenavConditionSparkleSpriteTemplate, 0, 0, 0);
        if (spriteId != MAX_SPRITES)
        {
            gPokenavState->conditionSparkleSprites[i] = &gSprites[spriteId];
            gPokenavState->conditionSparkleSprites[i]->invisible = TRUE;
        }
        else
        {
            break;
        }
    }

    InitConditionSparkles(count, 1);
}

void DestroyConditionSparkleSprites(void)
{
    u16 i;

    for (i = 0; i < 10; i++)
    {
        if (!gPokenavState->conditionSparkleSprites[i])
            return;

        DestroySprite(gPokenavState->conditionSparkleSprites[i]);
        gPokenavState->conditionSparkleSprites[i] = NULL;
    }
}

#define sSparkleId data[0]
#define sDelayTimer data[1]
#define sNumExtraSparkles data[2]
#define sCurSparkleId data[3]

void SpriteCB_ConditionSparkle_DoNextAfterDelay(struct Sprite *sprite)
{
    if (++sprite->sDelayTimer > 60)
    {
        sprite->sDelayTimer = 0;
        InitConditionSparkles(sprite->sNumExtraSparkles, 0);
    }
}

void SpriteCB_ConditionSparkle_WaitForAllAnim(struct Sprite *sprite)
{
    if (sprite->animEnded)
    {
        sprite->sDelayTimer = 0;
        sprite->callback = SpriteCB_ConditionSparkle_DoNextAfterDelay;
    }
}

void SetConditionSparklePosition(struct Sprite *sprite)
{
    if (gPokenavState->portraitSprite)
    {
        sprite->x = gPokenavState->portraitSprite->x
                         + gPokenavState->portraitSprite->x2
                         + gPokenavConditionSparkleCoords[sprite->sSparkleId][0];
        sprite->y = gPokenavState->portraitSprite->y
                         + gPokenavState->portraitSprite->y2
                         + gPokenavConditionSparkleCoords[sprite->sSparkleId][1];
    }
    else
    {
        sprite->x = gPokenavConditionSparkleCoords[sprite->sSparkleId][0] + 40;
        sprite->y = gPokenavConditionSparkleCoords[sprite->sSparkleId][1] + 104;
    }
}

void SpriteCB_ConditionSparkle(struct Sprite *sprite)
{
    if (sprite->sDelayTimer)
    {
        if (--sprite->sDelayTimer)
            return;

        SeekSpriteAnim(sprite, 0);
        sprite->invisible = FALSE;
    }

    SetConditionSparklePosition(sprite);
    if (sprite->animEnded)
    {
        sprite->invisible = TRUE;
        if (sprite->sCurSparkleId == sprite->sNumExtraSparkles)
        {
            if (sprite->sCurSparkleId == 9)
            {
                ShowAllConditionSparkles();
                sprite->callback = SpriteCB_ConditionSparkle_WaitForAllAnim;
            }
            else
            {
                sprite->callback = SpriteCB_ConditionSparkle_DoNextAfterDelay;
            }
        }
        else
        {
            sprite->callback = SpriteCallbackDummy;
        }
    }
}

void InitConditionSparkles(u8 count, u8 allowFirstShowAll)
{
    u16 i;

    for (i = 0; i < 10; i++)
    {
        if (gPokenavState->conditionSparkleSprites[i])
        {
            gPokenavState->conditionSparkleSprites[i]->sSparkleId = i;
            gPokenavState->conditionSparkleSprites[i]->sDelayTimer = i * 16 + 1;
            gPokenavState->conditionSparkleSprites[i]->sNumExtraSparkles = count;
            gPokenavState->conditionSparkleSprites[i]->sCurSparkleId = i;

            if (!allowFirstShowAll || count != 9)
            {
                gPokenavState->conditionSparkleSprites[i]->callback = SpriteCB_ConditionSparkle;
            }
            else
            {
                SetConditionSparklePosition(gPokenavState->conditionSparkleSprites[i]);
                ShowAllConditionSparkles();
                gPokenavState->conditionSparkleSprites[i]->callback = SpriteCB_ConditionSparkle_WaitForAllAnim;
                gPokenavState->conditionSparkleSprites[i]->invisible = FALSE;
            }
        }
    }
}

void ShowAllConditionSparkles(void)
{
    u16 i;

    for (i = 0; i < 10; i++)
    {
        if (gPokenavState->conditionSparkleSprites[i])
        {
            SeekSpriteAnim(gPokenavState->conditionSparkleSprites[i], 0);
            gPokenavState->conditionSparkleSprites[i]->invisible = FALSE;
        }
    }
}

#undef sSparkleId
#undef sDelayTimer
#undef sNumExtraSparkles
#undef sCurSparkleId

void InitPokenavIconSprite(void)
{
    gPokenavState->loadState = 0;
    if (!gPokenavState->isLinkActive)
        while (LoadPokenavIconSpriteStep());
}

bool8 LoadPokenavIconSpriteStep(void)
{
    u8 paletteIndex;
    u8 spriteId;
    struct SpritePalette spritePalette;

    switch (gPokenavState->loadState)
    {
    case 0:
        LZ77UnCompWram(gPokenavIconGfx, gPokenavState->spriteGfxScratchBuffer);
        break;
    case 1:
    {
        struct SpriteSheet spriteSheet = {
            .data = gPokenavState->spriteGfxScratchBuffer,
            .size = sizeof(gPokenavState->spriteGfxScratchBuffer),
            .tag = 0x18,
        };
        LoadSpriteSheet(&spriteSheet);
        break;
    }
    case 2:
        spritePalette = gPokenavIconSpritePalette;
        LoadSpritePalette(&spritePalette);
        paletteIndex = IndexOfSpritePaletteTag(0x10);
        gPokenavState->paletteFadeMask = -3 & ~(1 << (paletteIndex + 0x10));
        break;
    case 3:
        spriteId = CreateSprite(&gPokenavIconSpriteTemplate, 218, 14, 0);
        if (spriteId != MAX_SPRITES)
        {
            gPokenavState->pokenavIconSprite = &gSprites[spriteId];
            gPokenavState->pokenavIconSprite->data[0] = 0;
        }
        else
        {
            gPokenavState->pokenavIconSprite = NULL;
        }

        gPokenavState->loadState++;
        return FALSE;
    default:
        return FALSE;
    }

    gPokenavState->loadState++;
    return TRUE;
}

void SpriteCB_UpdatePokenavIconPosition(struct Sprite *sprite)
{
    sprite->y2 = -gPokenavState->menuVerticalOffset;
    if (sprite->y2 <= -32)
    {
        if (sprite->data[0] == 0)
        {
            sprite->invisible = TRUE;
            sprite->data[0] = 1;
        }
    }
    else
    {
        if (sprite->data[0] == 1)
        {
            sprite->invisible = FALSE;
            sprite->data[0] = 0;
        }
    }
}

void BuildTrainerEyesNameSpriteGfx(u8 *dst, u8 *text)
{
    u8 i;
    u8 *tileBuffer;
    u32 *tileWords;

    tileBuffer = gPokenavTileBuffer;
    DmaFill16(3, 0x1111, tileBuffer, 0x280);
    DmaFill16Defvars(3, 0x1111, 0x400 + tileBuffer, 0x280);
    Text_InitWindow8004E3C(&gPokenavTrainerEyesNameWindowTemplate, tileBuffer, text);

    DmaClear16(3, tileBuffer + 0x220, 0x60);
    DmaClear16(3, tileBuffer + 0x620, 0x60);

    tileWords = (int *)tileBuffer + 0x80;
    tileWords[0] &= 0x0FFFFFFF;
    tileWords[1] &= 0x0FFFFFFF;
    tileWords[2] &= 0x0FFFFFFF;
    tileWords[3] &= 0x0FFFFFFF;
    tileWords[4] &= 0x0FFFFFFF;
    tileWords[5] &= 0x0FFFFFFF;
    tileWords[6] &= 0x0FFFFFFF;
    tileWords[7] &= 0x0FFFFFFF;

    tileWords = (int *)tileBuffer + 0x180;
    tileWords[0] &= 0x0FFFFFFF;
    tileWords[1] &= 0x0FFFFFFF;
    tileWords[2] &= 0x0FFFFFFF;
    tileWords[3] &= 0x0FFFFFFF;
    tileWords[4] &= 0x0FFFFFFF;
    tileWords[5] &= 0x0FFFFFFF;
    tileWords[6] &= 0x0FFFFFFF;
    tileWords[7] &= 0x0FFFFFFF;

    for (i = 0; i < 5; i++)
    {
        DmaCopy16(3, &tileBuffer[128 * i], &dst[i * 256], 128);
        i++;i--; // fakematch
        DmaCopy16(3, &tileBuffer[128 * i + 0x400], &dst[32 * ((i * 8) + 4)], 128);
    }
}

void CreateTrainerEyesNameSprites(u8 *text)
{
    u16 i, tileOffset;
    u8 spriteId;
    struct SpriteSheet spriteSheet = {
        .data = gPokenavState->spriteGfxBuffers[0],
        .size = 0x500,
        .tag = 0x1A,
    };

    BuildTrainerEyesNameSpriteGfx(gPokenavState->spriteGfxBuffers[0], text);
    LoadSpriteSheet(&spriteSheet);
    LoadSpritePalette(&gPokenavTrainerEyesNameSpritePalette);

    tileOffset = 0;
    for (i = 0; i < 5; i++)
    {
        spriteId = CreateSprite(&gPokenavTrainerEyesNameSpriteTemplate, i * 32 + 113, 16, 0);
        if (spriteId != MAX_SPRITES)
        {
            gSprites[spriteId].oam.tileNum += tileOffset;
            gPokenavState->trainerEyesNameSprites[i] = &gSprites[spriteId];
        }
        else
        {
            gPokenavState->trainerEyesNameSprites[i] = NULL;
        }

        tileOffset += 8;
    }
}

void DestroyTrainerEyesNameSprites(void)
{
    u16 i;

    for (i = 0; i < 5; i++)
    {
        if (gPokenavState->trainerEyesNameSprites[i])
            DestroySprite(gPokenavState->trainerEyesNameSprites[i]);
    }

    FreeSpriteTilesByTag(0x1A);
    FreeSpritePaletteByTag(0x12);
}

void UpdateTrainerEyesNameSprites(u8 *text)
{
    u16 tile;

    BuildTrainerEyesNameSpriteGfx(gPokenavState->spriteGfxBuffers[0], text);
    tile = GetSpriteTileStartByTag(0x1A);
    if (tile != 0xFFFF)
    DmaCopy32Defvars(3, gPokenavState->spriteGfxBuffers[0], (void *)(VRAM + 0x10000 + (tile * 32)), 0x500);
}

u8 *BufferPokenavMonListRowText(u8 *dst, u16 listIndex, u8 displayMode)
{
    return BufferPokenavMonNameGenderLevelText(dst, listIndex, displayMode);
}

u8 *BufferConditionSearchRankText(u8 *dst, u16 rank)
{
    return AlignInt1InMenuWindow(StringCopy(dst, gOtherText_Number), rank, 56, 1);
}

u8 *BufferRibbonsMonListIndexText(u8 *dst, u16 index)
{
    u8 *buffer = AlignInt1InMenuWindow(dst, index, 23, 1);
    buffer[0] = EXT_CTRL_CODE_BEGIN;
    buffer[1] = 0x11;
    buffer[2] = 1;
    buffer += 3;
    buffer[0] = CHAR_SLASH;
    buffer += 1;
    buffer[0] = EXT_CTRL_CODE_BEGIN;
    buffer[1] = 0x11;
    buffer[2] = 1;
    buffer += 3;
    buffer = AlignInt1InMenuWindow(buffer, gPokenavState->lastListIndex + 1, 50, 1);
    return buffer;
}

u32 GetBoxOrPartyMonData(u16 box, u16 monIndex, int monDataField, u8 *text)
{
    if (box == 14)
    {
        if (monDataField == MON_DATA_NICKNAME || monDataField == MON_DATA_OT_NAME)
            return GetMonData(&gPlayerParty[monIndex], monDataField, text);
        else
            return GetMonData(&gPlayerParty[monIndex], monDataField);
    }
    else
    {
        if (monDataField == MON_DATA_NICKNAME || monDataField == MON_DATA_OT_NAME)
            return GetBoxMonData(&gPokemonStorage.boxes[box][monIndex], monDataField, text);
        else
            return GetBoxMonData(&gPokemonStorage.boxes[box][monIndex], monDataField);
    }
}

void SetMonMarkings(u16 box, u16 monIndex, u8 markings)
{
    if (box == 14)
        SetMonData(&gPlayerParty[monIndex], MON_DATA_MARKINGS, &markings);
    else
        SetBoxMonData(&gPokemonStorage.boxes[box][monIndex], MON_DATA_MARKINGS, &markings);
}

void BufferConditionGraphMonText(s16 listId, u8 loadId)
{
    u8 boxId;
    u8 isValid = gPokenavState->monList[listId].isValid;
    if (isValid)
    {
        BufferPokenavMonListRowText(gPokenavState->conditionMonNameText[loadId], listId, 0);
        boxId = gPokenavState->monList[listId].boxId;
        if (boxId == 14)
            AlignStringInMenuWindow(gPokenavState->conditionMonLocationText[loadId], gOtherText_InParty, 64, 0);
        else
            AlignStringInMenuWindow(gPokenavState->conditionMonLocationText[loadId], gPokemonStorage.boxNames[boxId], 64, 0);

        gPokenavState->conditionMonIsValid[loadId] = 1;
    }
    else
    {
        AlignStringInMenuWindow(gPokenavState->conditionMonNameText[loadId], gText_EmptyString, 104, 0);
        AlignStringInMenuWindow(gPokenavState->conditionMonLocationText[loadId], gText_EmptyString, 64, 0);
        gPokenavState->conditionMonIsValid[loadId] = isValid;
    }
}

void LoadConditionGraphMonStats(s16 listId, u8 loadId)
{
    u16 i;
    u16 boxId;
    u16 monId;

    if (gPokenavState->monList[listId].isValid)
    {
        boxId = gPokenavState->monList[listId].boxId;
        monId = gPokenavState->monList[listId].monId;
        gPokenavState->conditionGraph.conditions[loadId][0] = GetBoxOrPartyMonData(boxId, monId, MON_DATA_COOL, NULL);
        gPokenavState->conditionGraph.conditions[loadId][1] = GetBoxOrPartyMonData(boxId, monId, MON_DATA_TOUGH, NULL);
        gPokenavState->conditionGraph.conditions[loadId][2] = GetBoxOrPartyMonData(boxId, monId, MON_DATA_SMART, NULL);
        gPokenavState->conditionGraph.conditions[loadId][3] = GetBoxOrPartyMonData(boxId, monId, MON_DATA_CUTE, NULL);
        gPokenavState->conditionGraph.conditions[loadId][4] = GetBoxOrPartyMonData(boxId, monId, MON_DATA_BEAUTY, NULL);

        gPokenavState->conditionNumSparkles[loadId] = GetBoxOrPartyMonData(boxId, monId, MON_DATA_SHEEN, NULL) != 255
                                           ? GetBoxOrPartyMonData(boxId, monId, MON_DATA_SHEEN, NULL) / 29
                                           : 9;

        gPokenavState->conditionMonMarkings[loadId] = GetBoxOrPartyMonData(boxId, monId, MON_DATA_MARKINGS, NULL);
        ConditionGraph_CalcPositions(gPokenavState->conditionGraph.conditions[loadId], gPokenavState->conditionGraph.savedPositions[loadId]);
    }
    else
    {
        for (i = 0; i < 5; i++)
        {
            gPokenavState->conditionGraph.conditions[loadId][i] = 0;
            gPokenavState->conditionGraph.savedPositions[loadId][i].x = 0x9B;
            gPokenavState->conditionGraph.savedPositions[loadId][i].y = 0x5B;
        }
    }
}

void LoadConditionGraphMonGfx(s16 listId, u8 loadId)
{
    u16 species;
    u32 otId;
    u32 personality;
    u16 boxId;
    u16 monId;

    if (gPokenavState->monList[listId].isValid)
    {
        boxId = gPokenavState->monList[listId].boxId;
        monId = gPokenavState->monList[listId].monId;
        species = GetBoxOrPartyMonData(boxId, monId, MON_DATA_SPECIES2, NULL);
        otId = GetBoxOrPartyMonData(boxId, monId, MON_DATA_OT_ID, NULL);
        personality = GetBoxOrPartyMonData(boxId, monId, MON_DATA_PERSONALITY, NULL);

        HandleLoadSpecialPokePic(
            &gMonFrontPicTable[species],
            gMonFrontPicCoords[species].coords,
            1,
            gPokenavState->spriteGfxScratchBuffer,
            gPokenavState->spriteGfxBuffers[loadId],
            species,
            personality);

        LZ77UnCompWram(GetMonSpritePalFromOtIdPersonality(species, otId, personality), gPokenavState->portraitPaletteBuffers[loadId]);
        gPokenavState->conditionMonSpecies[loadId] = species;
    }
}

void LoadConditionGraphMonData(s16 listId, u8 loadId)
{
    BufferConditionGraphMonText(listId, loadId);
    LoadConditionGraphMonStats(listId, loadId);
    LoadConditionGraphMonGfx(listId, loadId);
}

void ResetPokenavMonList(void)
{
    gPokenavState->monListCount = 0;
}

void InsertPokenavMonListItem(struct PokenavMonListItem *item)
{
    u16 left, right, insertionIdx;

    left = 0;
    right = gPokenavState->monListCount;
    insertionIdx = left + (right - left) / 2;
    while (right != insertionIdx)
    {
        if (item->data > gPokenavState->monList[insertionIdx].data)
            right = insertionIdx;
        else
            left = insertionIdx + 1;

        insertionIdx = left + (right - left) / 2;
    }

    for (right = gPokenavState->monListCount; right > insertionIdx; right--)
    {
        gPokenavState->monList[right] = gPokenavState->monList[right - 1];
    }

    gPokenavState->monList[insertionIdx] = *item;
    gPokenavState->monListCount++;
}

void FinalizePokenavMonListRanks(void)
{
    u16 i;

    gPokenavState->monList[0].rank = 1;
    for (i = 1; i < gPokenavState->monListCount; i++)
    {
        if (gPokenavState->monList[i].data == gPokenavState->monList[i - 1].data)
            gPokenavState->monList[i].rank = gPokenavState->monList[i - 1].rank;
        else
            gPokenavState->monList[i].rank = i + 1;
    }

    gPokenavState->listCursorRow = 0;
    gPokenavState->firstVisibleListIndex = 0;
    gPokenavState->listSelectedIndex = 0;
    gPokenavState->lastVisibleListIndex = gPokenavState->monListCount < 9 ? (gPokenavState->monListCount - 1) : 7;
    gPokenavState->lastListIndex = gPokenavState->monListCount - 1;
    gPokenavState->hasListScrollArrows = gPokenavState->lastListIndex > 7;
}

void LoadConditionGraphMonNeighbors(void)
{
    s16 nextListIndex;
    s16 previousListIndex;

    LoadConditionGraphMonData(gPokenavState->detailSelectionIndex, 0);
    CreateOrUpdatePokenavPortraitSprite(0);
    if (gPokenavState->conditionGraphMonCount == 1)
    {
        gPokenavState->portraitSlot = 0;
        gPokenavState->nextLoadIdDown = 0;
        gPokenavState->nextLoadIdUp = 0;
    }
    else
    {
        gPokenavState->portraitSlot = 0;
        gPokenavState->nextLoadIdDown = 1;
        gPokenavState->nextLoadIdUp = 2;

        nextListIndex = gPokenavState->detailSelectionIndex + 1;
        if (nextListIndex >= gPokenavState->conditionGraphMonCount)
            nextListIndex = 0;

        previousListIndex = gPokenavState->detailSelectionIndex - 1;
        if (previousListIndex < 0)
            previousListIndex = gPokenavState->conditionGraphMonCount - 1;

        LoadConditionGraphMonData(nextListIndex, 1);
        LoadConditionGraphMonData(previousListIndex, 2);
    }
}

void BuildPartyConditionGraphMonList(void)
{
    u16 i, j;

    for (i = 0, j = 0; i < gPokenavState->playerPartyCount; i++)
    {
        if (!GetMonData(&gPlayerParty[i], MON_DATA_IS_EGG))
        {
            gPokenavState->monList[j].boxId = 14;
            gPokenavState->monList[j].monId = i;
            gPokenavState->monList[j].rank = j + 1;
            gPokenavState->monList[j].isValid = 1;
            j++;
        }
    }

    gPokenavState->monList[j].boxId = 0;
    gPokenavState->monList[j].monId = 0;
    gPokenavState->monList[j].rank = 0;
    gPokenavState->monList[j].isValid = 0;
    gPokenavState->detailSelectionIndex = 0;
    gPokenavState->conditionGraphMonCount = j + 1;
    LoadConditionGraphMonNeighbors();
    gPokenavState->canScrollConditionGraph = 1;
}

void PrepareConditionGraphFromSearchResult(void)
{
    gPokenavState->detailSelectionIndex = gPokenavState->listSelectedIndex;
    LoadConditionGraphMonNeighbors();

    if (gPokenavState->lastListIndex == 0)
        gPokenavState->canScrollConditionGraph = 0;
    else
        gPokenavState->canScrollConditionGraph = 1;
}

void InitConditionSearchResults(void)
{
    gPokenavState->monListBuildBoxId = 0;
    gPokenavState->monListBuildMonId = 0;
    ResetPokenavMonList();

    if (!gPokenavState->isLinkActive)
        while (BuildConditionSearchResultsStep());
}

bool8 BuildConditionSearchResultsStep(void)
{
    u16 i;
    register int mask asm("r3"); // FIXME
    int nextBoxId;
    struct PokenavMonListItem item;

    switch (gPokenavState->monListBuildBoxId)
    {
    default:
        item.isValid = 1;
        for (i = 0; i < 15; i++)
        {

            if (GetBoxMonData(&gPokemonStorage.boxes[gPokenavState->monListBuildBoxId][gPokenavState->monListBuildMonId], MON_DATA_SPECIES)
                && !GetBoxMonData(&gPokemonStorage.boxes[gPokenavState->monListBuildBoxId][gPokenavState->monListBuildMonId], MON_DATA_IS_EGG))
            {
                item.boxId = gPokenavState->monListBuildBoxId;
                item.monId = gPokenavState->monListBuildMonId;
                item.data = GetBoxMonData(
                    &gPokemonStorage.boxes[gPokenavState->monListBuildBoxId][gPokenavState->monListBuildMonId],
                    gPokenavState->conditionSearchMonDataId);
                InsertPokenavMonListItem(&item);
            }

            gPokenavState->monListBuildMonId++;
            mask = 0xFF;
            if (gPokenavState->monListBuildMonId == 30)
            {
                gPokenavState->monListBuildMonId = 0;
                nextBoxId = gPokenavState->monListBuildBoxId + 1;
                gPokenavState->monListBuildBoxId = nextBoxId;
                if ((nextBoxId & mask) == 14)
                    break;
            }
        }
        break;
    case 14:
        item.isValid = 1;
        item.boxId = 14;
        for (i = 0; i < gPokenavState->playerPartyCount; i++)
        {
            if (!GetMonData(&gPlayerParty[i], MON_DATA_IS_EGG))
            {
                item.monId = i;
                item.data = GetMonData(&gPlayerParty[i], gPokenavState->conditionSearchMonDataId);
                InsertPokenavMonListItem(&item);
            }
        }

        FinalizePokenavMonListRanks();
        gPokenavState->conditionGraphMonCount = gPokenavState->monListCount;
        gPokenavState->monListBuildBoxId++;
        break;
    case 15:
        return FALSE;
    }

    return TRUE;
}

void StartConditionGraphMonEnter(void)
{
    ConditionGraph_SetNewPositions(gPokenavState->conditionGraph.savedPositions[3], gPokenavState->conditionGraph.savedPositions[gPokenavState->portraitSlot]);
    ConditionGraph_TryUpdate();
}

bool8 UpdateConditionGraphMonEnter(void)
{
    bool8 isGraphUpdating = ConditionGraph_TryUpdate();
    bool8 isHeaderSliding = SlidePokenavMonInfoHeaderIn();
    return isGraphUpdating || isHeaderSliding;
}

void StartConditionGraphMonExit(void)
{
    if (gPokenavState->isConditionGraphSearchMode || gPokenavState->detailSelectionIndex != gPokenavState->playerPartyCount)
        ConditionGraph_SetNewPositions(gPokenavState->conditionGraph.savedPositions[gPokenavState->portraitSlot], gPokenavState->conditionGraph.savedPositions[3]);
}

bool8 UpdateConditionGraphMonExit(void)
{
    bool8 isGraphUpdating = ConditionGraph_TryUpdate();
    bool8 isHeaderSliding = SlidePokenavMonInfoHeaderOut();
    return isGraphUpdating || isHeaderSliding;
}

void BeginConditionGraphMonScroll(bool8 moveUp)
{
    u16 newLoadId;
    u8 wasCurrentMonValid;
    u8 isNewMonValid;

    if (moveUp)
        newLoadId = gPokenavState->nextLoadIdUp;
    else
        newLoadId = gPokenavState->nextLoadIdDown;

    ConditionGraph_SetNewPositions(gPokenavState->conditionGraph.savedPositions[gPokenavState->portraitSlot], gPokenavState->conditionGraph.savedPositions[newLoadId]);
    wasCurrentMonValid = gPokenavState->monList[gPokenavState->detailSelectionIndex].isValid;
    if (moveUp)
    {
        gPokenavState->nextLoadIdUp = gPokenavState->nextLoadIdDown;
        gPokenavState->nextLoadIdDown = gPokenavState->portraitSlot;
        gPokenavState->portraitSlot = newLoadId;
        gPokenavState->toLoadId = gPokenavState->nextLoadIdUp;

        gPokenavState->detailSelectionIndex = gPokenavState->detailSelectionIndex
                                     ? gPokenavState->detailSelectionIndex - 1
                                     : gPokenavState->conditionGraphMonCount - 1;
        gPokenavState->toLoadListIndex = gPokenavState->detailSelectionIndex
                                     ? gPokenavState->detailSelectionIndex - 1
                                     : gPokenavState->conditionGraphMonCount - 1;
    }
    else
    {
        gPokenavState->nextLoadIdDown = gPokenavState->nextLoadIdUp;
        gPokenavState->nextLoadIdUp = gPokenavState->portraitSlot;
        gPokenavState->portraitSlot = newLoadId;
        gPokenavState->toLoadId = gPokenavState->nextLoadIdDown;

        gPokenavState->detailSelectionIndex = (gPokenavState->detailSelectionIndex < gPokenavState->conditionGraphMonCount - 1)
                                     ? gPokenavState->detailSelectionIndex + 1
                                     : 0;
        gPokenavState->toLoadListIndex = (gPokenavState->detailSelectionIndex < gPokenavState->conditionGraphMonCount - 1)
                                     ? gPokenavState->detailSelectionIndex + 1
                                     : 0;
    }

    isNewMonValid = gPokenavState->monList[gPokenavState->detailSelectionIndex].isValid;
    if (!wasCurrentMonValid)
        gPokenavState->conditionGraphScrollCallback = UpdateConditionGraphScrollToMon;
    else if (!isNewMonValid)
        gPokenavState->conditionGraphScrollCallback = UpdateConditionGraphScrollToEmpty;
    else
        gPokenavState->conditionGraphScrollCallback = UpdateConditionGraphScrollBetweenMons;

    gPokenavState->detailTransitionState = 0;
}

bool8 UpdateConditionGraphMonScroll(void)
{
    return gPokenavState->conditionGraphScrollCallback();
}

bool8 UpdateConditionGraphScrollToMon(void)
{
    switch (gPokenavState->detailTransitionState)
    {
    case 0:
        CreateOrUpdatePokenavPortraitSprite(gPokenavState->portraitSlot);
        PrintConditionGraphMonInfo(gPokenavState->portraitSlot);
        gPokenavState->detailTransitionState++;
        // fall through
    case 1:
        if (!UpdateConditionGraphMonEnter())
        {
            LoadConditionGraphMonData(gPokenavState->toLoadListIndex, gPokenavState->toLoadId);
            gPokenavState->detailTransitionState++;
        }
        break;
    case 2:
        return FALSE;
    }

    return TRUE;
}

bool8 UpdateConditionGraphScrollToEmpty(void)
{
    switch (gPokenavState->detailTransitionState)
    {
    case 0:
        if (!UpdateConditionGraphMonExit())
        {
            PrintConditionGraphMonInfo(gPokenavState->portraitSlot);
            LoadConditionGraphMonData(gPokenavState->toLoadListIndex, gPokenavState->toLoadId);
            gPokenavState->detailTransitionState++;
        }
        break;
    case 1:
        return FALSE;
    }

    return TRUE;
}

bool8 UpdateConditionGraphScrollBetweenMons(void)
{
    switch (gPokenavState->detailTransitionState)
    {
    case 0:
        ConditionGraph_TryUpdate();
        if (!SlidePokenavMonInfoHeaderOut())
        {
            CreateOrUpdatePokenavPortraitSprite(gPokenavState->portraitSlot);
            PrintConditionGraphMonInfo(gPokenavState->portraitSlot);
            gPokenavState->detailTransitionState++;
        }
        break;
    case 1:
        if (!UpdateConditionGraphMonEnter())
            gPokenavState->detailTransitionState++;
        break;
    case 2:
        LoadConditionGraphMonData(gPokenavState->toLoadListIndex, gPokenavState->toLoadId);
        return FALSE;
    }

    return TRUE;
}

void ConditionGraph_SetNewPositions(struct UCoords16 *oldPositions, struct UCoords16 *newPositions)
{
    u16 i, j;
    int coord;
    int increment;

    for (i = 0; i < 5; i++)
    {
        coord = oldPositions[i].x << 8;
        increment = ((newPositions[i].x - oldPositions[i].x) << 8) / 10;
        for (j = 0; j < 9; j++)
        {
            gPokenavState->conditionGraph.newPositions[j][i].x = (coord >> 8) + ((coord >> 7) & 1);
            coord += increment;
        }

        gPokenavState->conditionGraph.newPositions[j][i].x = newPositions[i].x;
        coord = oldPositions[i].y << 8;
        increment = ((newPositions[i].y - oldPositions[i].y) << 8) / 10;
        for (j = 0; j < 9; j++)
        {
            gPokenavState->conditionGraph.newPositions[j][i].y = (coord >> 8) + ((coord >> 7) & 1);
            coord += increment;
        }

        gPokenavState->conditionGraph.newPositions[j][i].y = newPositions[i].y;
    }

    gPokenavState->conditionGraph.updateCounter = 0;
}

bool8 ConditionGraph_TryUpdate(void)
{
    if (gPokenavState->conditionGraph.updateCounter < 10)
    {
        ConditionGraph_Update(gPokenavState->conditionGraph.newPositions[gPokenavState->conditionGraph.updateCounter++]);
        return gPokenavState->conditionGraph.updateCounter != 10;
    }
    else
    {
        return FALSE;
    }
}

void StartPokeblockConditionGraphReset(struct UCoords16 *oldPositions, struct UCoords16 *newPositions)
{
    ConditionGraph_SetNewPositions(oldPositions, newPositions);
}

bool8 UpdatePokeblockConditionGraphReset(void)
{
    return ConditionGraph_TryUpdate();
}

void ConditionGraph_Update(struct UCoords16 *positions)
{
    u16 i;

    for (i = 0; i < 5; i++)
        gPokenavState->conditionGraph.curPositions[i] = positions[i];

    gPokenavState->conditionGraph.needsDraw = 1;
}

void ConditionGraph_CalcPositions(u8 *conditions, struct UCoords16 positions[])
{
    u16 i;
    u8 lineLength = gConditionToLineLength[*conditions++];
    u8 sinIdx;
    s8 posIdx;

    positions[0].x = 0x9b;
    positions[0].y = 0x5b - lineLength;

    sinIdx = 0x40;
    posIdx = 0;
    for (i = 1; i < 5; i++)
    {
        sinIdx += 0x33;
        posIdx--;
        if (posIdx < 0)
            posIdx = 4;
        if (posIdx == 2)
            sinIdx++;
        lineLength = gConditionToLineLength[*conditions++];
        positions[posIdx].x = ((lineLength * gSineTable[sinIdx + 0x40]) >> 8) + 0x9b;
        positions[posIdx].y = 0x5b - ((lineLength * gSineTable[sinIdx]) >> 8);

        if (posIdx <= 2 && (lineLength != 0x20 || posIdx != 2))
            positions[posIdx].x = ((lineLength * gSineTable[sinIdx + 0x40]) >> 8) + 0x9c;
    }
}

void CalcPokeblockConditionGraphPositions(u8 *conditions, struct UCoords16 positions[])
{
    ConditionGraph_CalcPositions(conditions, positions);
}

void ConditionGraph_CalcLine(u16 *scanline, struct UCoords16 *pos1, struct UCoords16 *pos2, bool8 dir, u16 *overflowScanline)
{
    u16 i, height, top, bottom, x2;
    u16 *ptr;
    s32 x, xIncrement;

    xIncrement = 0;
    if (pos1->y < pos2->y)
    {
        top = pos1->y;
        bottom = pos2->y;
        x = pos1->x << 10;
        x2 = pos2->x;
        height = bottom - top;
        if (height != 0)
            xIncrement = ((x2 - pos1->x) << 10) / height;
    }
    else
    {
        bottom = pos1->y;
        top = pos2->y;
        x = pos2->x << 10;
        x2 = pos1->x;
        height = bottom - top;
        if (height != 0)
            xIncrement = ((x2 - pos2->x) << 10) / height;
    }

    height++;
    if (overflowScanline == NULL)
    {
        scanline += (top - 56) * 2;
        for (i = 0; i < height; i++)
        {
            scanline[dir] = (x >> 10) + ((x >> 9) & 1) + dir;
            x += xIncrement;
            scanline += 2;
        }

        ptr = scanline - 2;
    }
    else if (xIncrement > 0)
    {
        overflowScanline += (top - 56) * 2;
        // Less readable than the other loops, but it has to be written this way to match.
        for (i = 0; i < height; overflowScanline[dir] = (x >> 10) + ((x >> 9) & 1) + dir, x += xIncrement, overflowScanline += 2, i++)
        {
            if (x >= (155 << 10))
                break;
        }

        gPokenavState->conditionGraph.bottom = top + i;
        scanline += (gPokenavState->conditionGraph.bottom - 56) * 2;
        for (; i < height; i++)
        {
            scanline[dir] = (x >> 10) + ((x >> 9) & 1) + dir;
            x += xIncrement;
            scanline += 2;
        }

        ptr = scanline - 2;
    }
    else if (xIncrement < 0)
    {
        scanline += (top - 56) * 2;
        for (i = 0; i < height; i++)
        {
            scanline[dir] = (x >> 10) + ((x >> 9) & 1) + dir;
            if (x < (155 << 10))
            {
                scanline[dir] = 155;
                break;
            }
            x += xIncrement;
            scanline += 2;
        }

        gPokenavState->conditionGraph.bottom = top + i;
        overflowScanline += (gPokenavState->conditionGraph.bottom - 56) * 2;
        for (; i < height; i++)
        {
            overflowScanline[dir] = (x >> 10) + ((x >> 9) & 1) + dir;
            x += xIncrement;
            overflowScanline += 2;
        }

        ptr = overflowScanline - 2;
    }
    else
    {
        gPokenavState->conditionGraph.bottom = top;
        scanline += (top - 56) * 2;
        overflowScanline += (top - 56) * 2;
        scanline[1] = pos1->x + 1;
        overflowScanline[0] = pos2->x;
        overflowScanline[1] = 155;
        return;
    }

    ptr[dir] = dir + x2;
}

void ConditionGraph_CalcRightHalf(struct UCoords16 *positions)
{
    u16 i, y, bottom;

    if (positions[0].y < positions[1].y)
    {
        y = positions[0].y;
        ConditionGraph_CalcLine((u16 *)gPokenavState->conditionGraph.scanlineRight, &positions[0], &positions[1], 1, NULL);
    }
    else
    {
        y = positions[1].y;
        ConditionGraph_CalcLine((u16 *)gPokenavState->conditionGraph.scanlineRight, &positions[1], &positions[0], 0, NULL);
    }
    ConditionGraph_CalcLine((u16 *)gPokenavState->conditionGraph.scanlineRight, &positions[1], &positions[2], 1, NULL);

    i = positions[2].y <= positions[3].y;
    ConditionGraph_CalcLine((u16 *)gPokenavState->conditionGraph.scanlineRight, &positions[2], &positions[3], i, (u16 *)gPokenavState->conditionGraph.scanlineLeft);
    for (i = 56; i < y; i++)
    {
        gPokenavState->conditionGraph.scanlineRight[i - 56][0] = 0;
        gPokenavState->conditionGraph.scanlineRight[i - 56][1] = 0;
    }

    for (i = positions[0].y; i <= gPokenavState->conditionGraph.bottom; i++)
        gPokenavState->conditionGraph.scanlineRight[i - 56][0] = 155;

    bottom = max(gPokenavState->conditionGraph.bottom, positions[2].y);
    for (i = bottom + 1; i < 122; i++)
    {
        gPokenavState->conditionGraph.scanlineRight[i - 56][0] = 0;
        gPokenavState->conditionGraph.scanlineRight[i - 56][1] = 0;
    }

//    for (i = 56; i < 122; i++)
//    {
//        if (gPokenavState->conditionGraph.scanlineRight[i - 56][0] == 0 && gPokenavState->conditionGraph.scanlineRight[i - 56][1] != 0)
//            gPokenavState->conditionGraph.scanlineRight[i - 56][0] = 155;
//    }
}

void ConditionGraph_CalcLeftHalf(struct UCoords16 *positions)
{
    u16 i, y, bottom;

    if (positions[0].y < positions[4].y)
    {
        y = positions[0].y;
        ConditionGraph_CalcLine((u16 *)gPokenavState->conditionGraph.scanlineLeft, &positions[0], &positions[4], 0, NULL);
    }
    else
    {
        y = positions[4].y;
        ConditionGraph_CalcLine((u16 *)gPokenavState->conditionGraph.scanlineLeft, &positions[4], &positions[0], 1, NULL);
    }

    ConditionGraph_CalcLine((u16 *)gPokenavState->conditionGraph.scanlineLeft, &positions[4], &positions[3], 0, NULL);

    for (i = 56; i < y; i++)
    {
        gPokenavState->conditionGraph.scanlineLeft[i - 56][0] = 0;
        gPokenavState->conditionGraph.scanlineLeft[i - 56][1] = 0;
    }

    for (i = positions[0].y; i <= gPokenavState->conditionGraph.bottom; i++)
        gPokenavState->conditionGraph.scanlineLeft[i - 56][1] = 155;

    bottom = max(gPokenavState->conditionGraph.bottom, positions[3].y + 1);
    for (i = bottom; i < 122; i++)
    {
        gPokenavState->conditionGraph.scanlineLeft[i - 56][0] = 0;
        gPokenavState->conditionGraph.scanlineLeft[i - 56][1] = 0;
    }

//    for (i = 0; i < 66; i++)
//    {
//        if (gPokenavState->conditionGraph.scanlineLeft[i][0] >= gPokenavState->conditionGraph.scanlineLeft[i][1])
//        {
//            gPokenavState->conditionGraph.scanlineLeft[i][1] = 0;
//            gPokenavState->conditionGraph.scanlineLeft[i][0] = 0;
//        }
//    }
}

void ConditionGraph_InitResetScanline(void)
{
    gPokenavState->conditionGraph.scanlineResetState = 0;
}

extern const struct ScanlineEffectParams gConditionGraphScanlineParams;

bool8 ConditionGraph_ResetScanline(void)
{
    s32 i;
    struct ScanlineEffectParams params;

    switch (gPokenavState->conditionGraph.scanlineResetState)
    {
    case 0:
        ScanlineEffect_Clear();
        for (i = 0; i < 16; i++)
        {
            gScanlineEffectRegBuffers[0][16 + 2 * i] = 0xEF;
            gScanlineEffectRegBuffers[0][17 + 2 * i] = 0xEF;
            gScanlineEffectRegBuffers[1][16 + 2 * i] = 0xEF;
            gScanlineEffectRegBuffers[1][17 + 2 * i] = 0xEF;
        }
        gPokenavState->conditionGraph.scanlineResetState++;
        return TRUE;
    case 1:
        params = gConditionGraphScanlineParams;
        ScanlineEffect_SetParams(params);
        gPokenavState->conditionGraph.scanlineResetState++;
        break;
    }
    return FALSE;
}

void StopConditionGraphScanlineEffect(void)
{
    gScanlineEffect.state = 3;
    ScanlineEffect_InitHBlankDmaTransfer();
}

void ConditionGraph_Draw(void)
{
    u16 i;

    if (gPokenavState->conditionGraph.needsDraw)
    {
        ConditionGraph_CalcRightHalf(gPokenavState->conditionGraph.curPositions);
        ConditionGraph_CalcLeftHalf(gPokenavState->conditionGraph.curPositions);
        for (i = 0; i < 66; i++)
        {
            gScanlineEffectRegBuffers[1][(i + 55) * 2 + 0] = gScanlineEffectRegBuffers[0][(i + 55) * 2 + 0] = (gPokenavState->conditionGraph.scanlineRight[i][0] << 8) | (gPokenavState->conditionGraph.scanlineRight[i][1]);
            gScanlineEffectRegBuffers[1][(i + 55) * 2 + 1] = gScanlineEffectRegBuffers[0][(i + 55) * 2 + 1] = (gPokenavState->conditionGraph.scanlineLeft[i][0] << 8) | (gPokenavState->conditionGraph.scanlineLeft[i][1]);
        }
        gPokenavState->conditionGraph.needsDraw = 0;
    }
}

void ConditionGraph_DrawAtYOffset(u8 yOffset)
{
    u16 i, scanlineOffset;

    if (gPokenavState->conditionGraph.needsDraw)
    {
        ConditionGraph_CalcRightHalf(gPokenavState->conditionGraph.curPositions);
        ConditionGraph_CalcLeftHalf(gPokenavState->conditionGraph.curPositions);
        scanlineOffset = 2 * (55 - yOffset);
        for (i = 0; i < 66; i ++)
        {
            gScanlineEffectRegBuffers[1][scanlineOffset + 0] = gScanlineEffectRegBuffers[0][scanlineOffset + 0] = (gPokenavState->conditionGraph.scanlineRight[i][0] << 8) | (gPokenavState->conditionGraph.scanlineRight[i][1]);
            gScanlineEffectRegBuffers[1][scanlineOffset + 1] = gScanlineEffectRegBuffers[0][scanlineOffset + 1] = (gPokenavState->conditionGraph.scanlineLeft[i][0] << 8) | (gPokenavState->conditionGraph.scanlineLeft[i][1]);
            scanlineOffset += 2;
        }
        gPokenavState->conditionGraph.needsDraw = 0;
    }
}

u8 HandlePokenavListDpadInput(void)
{
    if (JOY_REPT(DPAD_UP))
    {
        return PokenavList_MoveCursorUp();
    }
    else if (JOY_REPT(DPAD_DOWN))
    {
        return PokenavList_MoveCursorDown();
    }
    else if (JOY_REPT(DPAD_LEFT))
    {
        return PokenavList_PageUp();
    }
    else if (JOY_REPT(DPAD_RIGHT))
    {
        return PokenavList_PageDown();
    }
    else
    {
        return 0;
    }
}

u8 PokenavList_MoveCursorUp(void)
{
    if (gPokenavState->listSelectedIndex == 0)
    {
        return 0;
    }
    if (gPokenavState->hasListScrollArrows != 0 && gPokenavState->listCursorRow == 0)
    {
        BeginPokenavListScroll(-1);
        UpdatePokenavListIndicesAfterScroll(-1);
        return 2;
    }
    gPokenavState->listCursorRow--;
    if (gPokenavState->hasListScrollArrows == 0 && gPokenavState->listCursorRow < 0)
    {
        gPokenavState->listCursorRow = gPokenavState->lastVisibleListIndex;
    }
    gPokenavState->listSelectedIndex = gPokenavState->firstVisibleListIndex + gPokenavState->listCursorRow;
    if (gPokenavState->listSelectedIndex > gPokenavState->lastListIndex)
    {
        gPokenavState->listSelectedIndex -= gPokenavState->lastListIndex + 1;
    }
    return 1;
}

u8 PokenavList_MoveCursorDown(void)
{
    if (gPokenavState->listSelectedIndex == gPokenavState->lastListIndex)
    {
        return 0;
    }
    if (gPokenavState->hasListScrollArrows != 0 && gPokenavState->listCursorRow == 7)
    {
        BeginPokenavListScroll(1);
        UpdatePokenavListIndicesAfterScroll(1);
        return 2;
    }
    gPokenavState->listCursorRow++;
    if (gPokenavState->hasListScrollArrows == 0 && gPokenavState->listCursorRow > gPokenavState->lastVisibleListIndex)
    {
        gPokenavState->listCursorRow = 0;
    }
    gPokenavState->listSelectedIndex = gPokenavState->firstVisibleListIndex + gPokenavState->listCursorRow;
    if (gPokenavState->listSelectedIndex > gPokenavState->lastListIndex)
    {
        gPokenavState->listSelectedIndex -= gPokenavState->lastListIndex + 1;
    }
    return 1;
}

u8 PokenavList_PageUp(void)
{
    s16 scroll;
    if (gPokenavState->firstVisibleListIndex == 0 || gPokenavState->hasListScrollArrows == 0)
    {
        return 0;
    }
    if (gPokenavState->firstVisibleListIndex < 8)
    {
        scroll = -gPokenavState->firstVisibleListIndex;
    }
    else
    {
        scroll = -8;
    }
    BeginPokenavListScroll(scroll);
    UpdatePokenavListIndicesAfterScroll(scroll);
    return 2;
}

u8 PokenavList_PageDown(void)
{
    s16 scroll;
    if (gPokenavState->lastVisibleListIndex == gPokenavState->lastListIndex || gPokenavState->hasListScrollArrows == 0)
    {
        return 0;
    }
    scroll = gPokenavState->lastListIndex - gPokenavState->lastVisibleListIndex;
    if (scroll > 8)
    {
        scroll = 8;
    }
    BeginPokenavListScroll(scroll);
    UpdatePokenavListIndicesAfterScroll(scroll);
    return 2;
}

void UpdatePokenavListIndicesAfterScroll(s16 rowsToScroll)
{
    gPokenavState->firstVisibleListIndex += rowsToScroll;
    if (gPokenavState->firstVisibleListIndex > gPokenavState->lastListIndex)
    {
        gPokenavState->firstVisibleListIndex -= gPokenavState->lastListIndex + 1;
    }
    if (gPokenavState->firstVisibleListIndex < 0)
    {
        gPokenavState->firstVisibleListIndex += gPokenavState->lastListIndex + 1;
    }
    gPokenavState->lastVisibleListIndex += rowsToScroll;
    if (gPokenavState->lastVisibleListIndex > gPokenavState->lastListIndex)
    {
        gPokenavState->lastVisibleListIndex -= gPokenavState->lastListIndex + 1;
    }
    if (gPokenavState->lastVisibleListIndex < 0)
    {
        gPokenavState->lastVisibleListIndex += gPokenavState->lastListIndex + 1;
    }
    gPokenavState->listSelectedIndex += rowsToScroll;
    if (gPokenavState->listSelectedIndex > gPokenavState->lastListIndex)
    {
        gPokenavState->listSelectedIndex -= gPokenavState->lastListIndex + 1;
    }
    if (gPokenavState->listSelectedIndex < 0)
    {
        gPokenavState->listSelectedIndex += gPokenavState->lastListIndex + 1;
    }
}

void RestorePokenavListSelection(void)
{
    if (gPokenavState->hasListScrollArrows != 0)
    {
        if (gPokenavState->detailSelectionIndex < gPokenavState->lastListIndex - 7)
        {
            gPokenavState->listCursorRow = 0;
            gPokenavState->firstVisibleListIndex = gPokenavState->detailSelectionIndex;
            gPokenavState->listSelectedIndex = gPokenavState->detailSelectionIndex;
            gPokenavState->lastVisibleListIndex = gPokenavState->firstVisibleListIndex + 7;
            if (gPokenavState->lastVisibleListIndex > gPokenavState->lastListIndex)
            {
                gPokenavState->lastVisibleListIndex -= gPokenavState->lastListIndex + 1;
            }
        }
        else
        {
            gPokenavState->firstVisibleListIndex = gPokenavState->lastListIndex - 7;
            gPokenavState->lastVisibleListIndex = gPokenavState->lastListIndex;
            gPokenavState->listSelectedIndex = gPokenavState->detailSelectionIndex;
            gPokenavState->listCursorRow = 7 - (gPokenavState->lastListIndex -  gPokenavState->listSelectedIndex);
        }
    }
    else
    {
        gPokenavState->listCursorRow = gPokenavState->detailSelectionIndex;
        gPokenavState->listSelectedIndex = gPokenavState->detailSelectionIndex;
    }
}

void InitAnyMonHasRibbonScan(void)
{
    gPokenavState->monListBuildBoxId = 0;
    gPokenavState->monListBuildMonId = 0;
    gPokenavState->hasAnyRibbons = 0xFF;
    if (gPokenavState->isLinkActive == 0)
    {
        while (AnyMonHasRibbonScanStep())
            ;
    }
}

bool8 AnyMonHasRibbonScanStep(void)
{
    u16 i;

    if (gPokenavState->hasAnyRibbons != -1)
        return FALSE;

    switch (gPokenavState->monListBuildBoxId)
    {
    default:
        for (i = 0; i < 10; i++)
        {
            if (GetBoxMonData(&gPokemonStorage.boxes[gPokenavState->monListBuildBoxId + 0][gPokenavState->monListBuildMonId], MON_DATA_RIBBON_COUNT) != 0)
            {
                gPokenavState->hasAnyRibbons = 1;
                return FALSE;
            }
            if (GetBoxMonData(&gPokemonStorage.boxes[gPokenavState->monListBuildBoxId + 7][gPokenavState->monListBuildMonId], MON_DATA_RIBBON_COUNT) != 0)
            {
                gPokenavState->hasAnyRibbons = 1;
                return FALSE;
            }
            if (++gPokenavState->monListBuildMonId >= 30)
            {
                gPokenavState->monListBuildMonId = 0;
                if (++gPokenavState->monListBuildBoxId >= 7)
                {
                    break;
                }
            }
        }
        break;
    case 8:
        return FALSE;
    case 7:
        gPokenavState->hasAnyRibbons = 0;
        for (i = 0; i < 6; i++)
        {
            if (GetMonData(&gPlayerParty[i], MON_DATA_RIBBON_COUNT) != 0)
            {
                gPokenavState->hasAnyRibbons = 1;
                break;
            }
        }
        gPokenavState->monListBuildBoxId++;
        return FALSE;
    }
    return TRUE;
}

void InitRibbonsMonList(void)
{
    gPokenavState->monListBuildBoxId = 0;
    gPokenavState->monListBuildMonId = 0;
    ResetPokenavMonList();
    if (gPokenavState->isLinkActive == 0)
    {
        while (BuildRibbonsMonListStep())
            ;
    }
}

bool8 BuildRibbonsMonListStep(void)
{
    struct PokenavMonListItem item;
    u8 ribbons;
    u16 i;

    switch (gPokenavState->monListBuildBoxId)
    {
    default:
        item.isValid = 1;
        for (i = 0; i < 15; i++)
        {
            ribbons = GetBoxMonData(&gPokemonStorage.boxes[gPokenavState->monListBuildBoxId][gPokenavState->monListBuildMonId + 0], MON_DATA_RIBBON_COUNT);
            if (ribbons != 0)
            {
                item.boxId = gPokenavState->monListBuildBoxId;
                item.monId = gPokenavState->monListBuildMonId;
                item.data = ribbons;
                InsertPokenavMonListItem(&item);
            }
            if (++gPokenavState->monListBuildMonId == 30)
            {
                gPokenavState->monListBuildMonId = 0;
                if (++gPokenavState->monListBuildBoxId == 14)
                    break;
            }
        }
        break;
    case 15:
        return FALSE;
    case 14:
        item.isValid = 1;
        item.boxId = 14;
        for (i = 0; i < 6; i++)
        {
            ribbons = GetMonData(&gPlayerParty[i], MON_DATA_RIBBON_COUNT);
            if (ribbons != 0)
            {
                item.monId = i;
                item.data = ribbons;
                InsertPokenavMonListItem(&item);
            }
        }
        FinalizePokenavMonListRanks();
        gPokenavState->monListBuildBoxId++;
        return FALSE;
    }
    return TRUE;
}

u8 *BufferPokenavMonNameGenderLevelText(u8 *dst, u16 listIndex, u8 displayMode)
{
    u8 *dest = dst;
    u8 box = gPokenavState->monList[listIndex].boxId;
    u8 monNo = gPokenavState->monList[listIndex].monId;
    u16 species;
    u16 level;
    u8 gender;

    if (!GetBoxOrPartyMonData(box, monNo, MON_DATA_IS_EGG, NULL))
    {
        GetBoxOrPartyMonData(box, monNo, MON_DATA_NICKNAME, dest);
        StringGet_Nickname(dest);
        species = GetBoxOrPartyMonData(box, monNo, MON_DATA_SPECIES, NULL);
        if (box == 14)
        {
            level = GetMonData(&gPlayerParty[monNo], MON_DATA_LEVEL);
            gender = GetMonGender(&gPlayerParty[monNo]);
        }
        else
        {
            level = GetLevelFromBoxMonExp(&gPokemonStorage.boxes[box][monNo]);
            gender = GetGenderFromSpeciesAndPersonality(species, GetBoxOrPartyMonData(box, monNo, MON_DATA_PERSONALITY, NULL));
        }
        if (ShouldHideGenderIcon(species, dst))
        {
            gender = MON_GENDERLESS;
        }
        dest += StringLength(dest);

        dest[0] = EXT_CTRL_CODE_BEGIN;
        dest[1] = 0x13; // CLEAR_TO
        dest[2] = 63;
        dest += 3;

        switch (gender)
        {
        case MON_MALE:
            dest[0] = EXT_CTRL_CODE_BEGIN;
            dest[1] = 0x01; // COLOR
            dest[2] = TEXT_COLOR_WHITE;
            dest[3] = EXT_CTRL_CODE_BEGIN;
            dest[4] = 0x03; // SHADOW
            dest[5] = TEXT_COLOR_SKY_BLUE;
            dest[6] = CHAR_MALE;
            dest += 7;
            break;
        case MON_FEMALE:
            dest[0] = EXT_CTRL_CODE_BEGIN;
            dest[1] = 0x01; // COLOR
            dest[2] = TEXT_COLOR_BLACK2;
            dest[3] = EXT_CTRL_CODE_BEGIN;
            dest[4] = 0x03; // SHADOW
            dest[5] = TEXT_COLOR_SILVER;
            dest[6] = CHAR_FEMALE;
            dest += 7;
            break;
        }
        dest[0] = EXT_CTRL_CODE_BEGIN;
        dest[1] = 0x01; // COLOR
        dest[2] = TEXT_COLOR_DARK_GREY;
        dest[3] = EXT_CTRL_CODE_BEGIN;
        dest[4] = 0x03; // SHADOW
        dest[5] = TEXT_COLOR_YELLOW;
        dest += 6;

        dest[0] = EXT_CTRL_CODE_BEGIN;
        dest[1] = 0x13; // CLEAR_TO
        dest[2] = 70;
        dest += 3;

        dest[0] = CHAR_SLASH;
        dest[1] = EXT_CTRL_CODE_BEGIN;
        dest[2] = EXT_CTRL_CODE_CLEAR;
        dest[3] = 1;
        dest[4] = CHAR_LV;
        dest += 5;

        dest = ConvertIntToDecimalStringSimple(dest, level);
        if (displayMode == 1)
        {
            dest = AlignInt1InMenuWindow(dest, gPokenavState->monList[listIndex].data, 0x80, 0x01);
        }
        else
        {
            dest[0] = EXT_CTRL_CODE_BEGIN;
            dest[1] = 0x13; // CLEAR_TO
            dest[2] = 103;
            dest += 3;
            *dest = EOS;
        }
    }
    else
    {
        *dest = EOS;
    }
    return dest;
}

extern const u16 gRibbonSummaryMonDataFields[];

void InitRibbonsSummaryMonData(void)
{
    u8 ribbonIdBase;
    u32 ribbonFlags;
    u16 i, j;
    u8 numRibbons;
    u8 numVariants;

    BufferPokenavMonNameGenderLevelText(gPokenavState->conditionMonNameText[0], gPokenavState->listSelectedIndex, 0);
    LoadConditionGraphMonGfx(gPokenavState->listSelectedIndex, 0);
    gPokenavState->detailSelectionIndex = gPokenavState->listSelectedIndex;
    gPokenavState->ribbonCount = 0;
    ribbonIdBase = 0;
    ribbonFlags = GetBoxOrPartyMonData(gPokenavState->monList[gPokenavState->listSelectedIndex].boxId, gPokenavState->monList[gPokenavState->listSelectedIndex].monId, MON_DATA_RIBBONS, NULL);
    gPokenavState->giftRibbonCount = 0;
    for (i = 0; i < 17; i++)
    {
        switch (gRibbonSummaryMonDataFields[i])
        {
        case MON_DATA_COOL_RIBBON:
        case MON_DATA_BEAUTY_RIBBON:
        case MON_DATA_CUTE_RIBBON:
        case MON_DATA_SMART_RIBBON:
        case MON_DATA_TOUGH_RIBBON:
            numRibbons = ribbonFlags & 7;
            ribbonFlags >>= 3;
            numVariants = 4;
            break;
        default:
            numRibbons = ribbonFlags & 1;
            ribbonFlags >>= 1;
            numVariants = 1;
            break;
        };
        for (j = 0; j < numRibbons; j++)
            gPokenavState->ribbonIds[gPokenavState->ribbonCount++] = ribbonIdBase + j;
        if (numRibbons && ribbonIdBase > 24)
            gPokenavState->giftRibbonCount++;
        ribbonIdBase += numVariants;
    }
    if (gPokenavState->ribbonCount != gPokenavState->giftRibbonCount)
    {
        gPokenavState->ribbonCursorPos = 0;
        gPokenavState->ribbonPageIndex = 0;
    }
    else
    {
        gPokenavState->ribbonCursorPos = 0;
        gPokenavState->ribbonPageIndex = 3;
    }
    numRibbons = gPokenavState->ribbonCount - gPokenavState->giftRibbonCount;
    for (i = 0; i < 3; i++)
    {
        if (numRibbons > 8)
        {
            gPokenavState->ribbonPageCounts[i] = 9;
            numRibbons -= 9;
        }
        else
        {
            gPokenavState->ribbonPageCounts[i] = numRibbons;
            numRibbons = 0;
        }
    }
    gPokenavState->ribbonPageCounts[i] = gPokenavState->giftRibbonCount;
}

u8 HandleRibbonsSummaryCursorInput(void)
{
    s8 cursorPos = gPokenavState->ribbonCursorPos;
    s8 pageIndex = gPokenavState->ribbonPageIndex;
    s8 moved = 1;
    do
    {
        if (JOY_REPT(DPAD_UP) && pageIndex > 0)
        {
            while (pageIndex > 0)
            {
                pageIndex--;
                if (gPokenavState->ribbonPageCounts[pageIndex] != 0)
                    break;
            }
            if (gPokenavState->ribbonPageCounts[pageIndex] != 0)
            {
                if (cursorPos >= gPokenavState->ribbonPageCounts[pageIndex])
                    cursorPos = gPokenavState->ribbonPageCounts[pageIndex] - 1;
                break;
            }
            pageIndex = gPokenavState->ribbonPageIndex;
        }
        if (JOY_REPT(DPAD_DOWN) && pageIndex < 3)
        {
            while (pageIndex < 3)
            {
                pageIndex++;
                if (gPokenavState->ribbonPageCounts[pageIndex] != 0)
                    break;
            }
            if (gPokenavState->ribbonPageCounts[pageIndex] != 0)
            {
                if (cursorPos >= gPokenavState->ribbonPageCounts[pageIndex])
                    cursorPos = gPokenavState->ribbonPageCounts[pageIndex] - 1;
                break;
            }
            pageIndex = gPokenavState->ribbonPageIndex;
        }
        if (JOY_REPT(DPAD_LEFT))
        {
            if (cursorPos > 0)
            {
                cursorPos--;
                break;
            }
        }
        if (JOY_REPT(DPAD_RIGHT))
        {
            if (cursorPos < gPokenavState->ribbonPageCounts[pageIndex] - 1)
            {
                cursorPos++;
                break;
            }
        }
        moved = 0;
    } while (0);
    if (moved)
    {
        if (cursorPos != gPokenavState->ribbonCursorPos || pageIndex != gPokenavState->ribbonPageIndex)
        {
            gPokenavState->ribbonCursorPos = cursorPos;
            gPokenavState->ribbonPageIndex = pageIndex;
        }
        else
            moved = 0;
    }
    return moved;
}

void BeginRibbonsSummaryMonSwitch(s8 direction)
{
    gPokenavState->listSelectedIndex += direction;
    if (gPokenavState->listSelectedIndex < 0)
    {
        gPokenavState->listSelectedIndex = gPokenavState->lastListIndex;
    }
    if (gPokenavState->listSelectedIndex > gPokenavState->lastListIndex)
    {
        gPokenavState->listSelectedIndex = 0;
    }
    gPokenavState->detailSelectionDelta = direction;
    gPokenavState->detailSelectionIndex = gPokenavState->listSelectedIndex;
    REG_WININ = 0x3F37;
    REG_WINOUT = 0x3F3F;
    REG_WIN0H = 0x58F0;
    REG_WIN0V = 0x2060;
    gPokenavState->detailTransitionState = 0;
}

bool8 UpdateRibbonsSummaryMonSwitch(void)
{
    switch (gPokenavState->detailTransitionState)
    {
    case 0:
        if (!SlidePokenavMonInfoHeaderOut())
        {
            gPokenavState->detailTransitionState++;
        }
        break;
    case 1:
        REG_DISPCNT |= DISPCNT_WIN0_ON;
        ClearRibbonsSummaryDescription();
        InitRibbonsSummaryMonData();
        gPokenavState->detailTransitionState++;
        break;
    case 2:
        DrawMonRibbonIcons();
        gPokenavState->detailTransitionState++;
        break;
    case 3:
        CopyRibbonsSummaryTilemapToVram();
        gPokenavState->detailTransitionState++;
        break;
    case 4:
        LoadConditionGraphMonGfx(gPokenavState->listSelectedIndex, 0);
        gPokenavState->detailTransitionState++;
        break;
    case 5:
        CreateOrUpdatePokenavPortraitSprite(0);
        gPokenavState->detailTransitionState++;
        break;
    case 6:
        if (!SlidePokenavMonInfoHeaderIn())
        {
            PrintRibbonsSummaryMonInfo();
            REG_DISPCNT &= ~DISPCNT_WIN0_ON;
            gPokenavState->detailTransitionState++;
            return FALSE;
        }
        break;
    default:
        return FALSE;
    }

    return TRUE;
}
