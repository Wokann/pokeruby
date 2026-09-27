
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

extern const u8 gUnknown_083E0314[];
extern const u16 gUnknown_08E9F9E8[];
extern const u16 gUnknown_083E0274[];
extern const u8 gUnknown_08E9FC64[];
extern const u8 gUnknown_083E0354[];
extern const u8 gUnknown_08E9FD64[];
extern const u8 gUnknown_08E9FE54[];
extern const u8 gUnknown_08E9FD1C[];
extern const u16 gPokenavConditionSearch2_Pal[];
extern const u8 gUnknown_083E0334[];
extern const u16 gUnknown_083E02B4[];
extern const u8 gPokenavConditionSearch2_Gfx[];
extern const u8 gUnknown_083E0254[];
extern const u8 gConditionGraphData_Tilemap[];
extern const u8 gUnknown_083E01AC[];
extern const u8 gPokenavCondition_Tilemap[];
extern const u8 gPokenavConditionMenu2_Pal[];
extern const u8 gPokenavConditionView_Gfx[];
extern const u8 gPokenavRegionMapCityBannerFrames[];
extern const u8 gPokenavRegionMapCityBannerBlank[];
extern const u8 *const gPokenavCityMaps[][2];
extern const u8 gPokenavHoennMapSquares_Pal[];
extern const u8 gPokenavHoennMapSquares_Gfx[];
extern const u16 gUnknown_083E003C[];
extern const u8 *const gPokenavMainMenuDescriptions[];
extern const u8 *const gPokenavConditionMenuDescriptions[];
extern const u8 *const gPokenavConditionSearchDescriptions[];
extern u8 *gUnknown_083DFEC8;
extern const u8 gUnknown_083DFEEC[];
extern const u8 gUnknown_083E005C[];
extern const u8 gUnknown_083E007C[];
extern const u8 gPokenavOutlineTilemap[];
extern const u8 gPokenavOutlineTiles[];
extern const u8 gPokenavOutlinePalette[];
extern const u8 gUnknown_083DFECC[];
extern const u8 gUnknown_083DFF8C[];
extern const u8 gPokenavHoennMapMisc_Gfx[];
extern const u8 gUnknown_08E99FB0[];
extern const u8 gPokenavBottomToolbarTilemap[];
extern const u16 gPokenavHoennMap1_Pal[];
extern void (*const gPokenavListRowPrinters[])(u16, u16);
extern const u8 gUnknown_083E039C[];
extern const u8 gUnknown_083E03A0[];
extern const u8 gUnknown_083E01F4[];
extern const u8 *const gTrainerEyeDescriptions[];
extern const u8 gPokenavRibbonsSummaryTilemap[];
extern const u8 gPokenavRibbonView_Gfx[];
extern const u8 gPokenavRibbonIconsHalf_Gfx[];
extern const u16 gPokenavRibbonView_Pal[];
extern const u16 gUnknown_083E03A8[];
extern const u16 gUnknown_083E3C60[][16];
extern const u16 gPokenavRibbonsIconGfx[][2];
extern const u8 *const gRibbonDescriptions[][2];
extern const u8 *const gGiftRibbonDescriptions[][2];
extern const u8 gPokenavMonInfoHeaderTilemap[];
extern const u8 gPokenavMonInfoHeader_Gfx[];
extern const u16 gUnknown_083E0124[];
extern const u16 gUnknown_083E0144[];
extern const u8 gPokenavMenuOptions_Gfx[];
extern const u8 gPokenavConditionMenu_Gfx[];
extern const u8 gPokenavConditionSearch_Gfx[];
extern const struct SpriteTemplate gSpriteTemplate_83E4454;
extern const union AffineAnimCmd *const gSpriteAffineAnimTable_83E4450[];
extern const u16 gUnknown_083E42F8[];
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
extern const struct SpritePalette gUnknown_083E449C[];
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
extern const u8 gUnknown_083E3D00[];
extern const struct SpriteTemplate gSpriteTemplate_83E476C;
extern const struct SpriteSheet gUnknown_083E4784;
extern const struct SpritePalette gUnknown_083E478C;
extern const struct SpriteTemplate gSpriteTemplate_83E4800;
extern const s16 gUnknown_083E4794[][2];
extern const u8 gUnknown_083E329C[];
extern const struct SpritePalette gUnknown_083E4818;
extern const struct SpriteTemplate gSpriteTemplate_83E4850;
extern const struct SpritePalette gUnknown_083E4868;
extern const struct SpriteTemplate gSpriteTemplate_83E4878;

// Static ROM declarations

u8 sub_80F5E20(void);
u8 sub_80F5EE4(void);
u8 sub_80F5FB4(void);
u8 sub_80F6010(void);

// .rodata

extern const u8 gUnknown_083E4890[];

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
    gPokenavStructPtr->setupStep = 0;
}

void InitPokenavBackground(void)
{
    gPokenavStructPtr->setupStep = 0;
    if (!gPokenavStructPtr->unk6DAC)
    {
        while (LoadPokenavBackgroundStep())
            ;
    }
}

bool8 LoadPokenavBackgroundStep(void)
{
    switch (gPokenavStructPtr->setupStep)
    {
    case 0:
        LZ77UnCompVram(gPokenavHoennMapMisc_Gfx, (void *)VRAM + 0xC000);
        break;
    case 1:
        LZ77UnCompVram(gUnknown_08E99FB0, (void *)VRAM + 0xD800);
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
        gPokenavStructPtr->menuVerticalOffset = 0;
        REG_BG1HOFS = 0;
        REG_BG1VOFS = 0;
        REG_BG1CNT = 0x1B0C;
        gPokenavStructPtr->setupStep++;
    default:
        return FALSE;
    }
    gPokenavStructPtr->setupStep++;
    return TRUE;
}

bool8 SlideMenuHeaderUp(void)
{
    bool8 retVal = TRUE;

    if (gPokenavStructPtr->menuVerticalOffset == 32)
        return FALSE;

    gPokenavStructPtr->menuVerticalOffset += 2;
    if (gPokenavStructPtr->menuVerticalOffset > 31)
    {
        gPokenavStructPtr->menuVerticalOffset = 32;
        retVal = FALSE;
    }

    REG_BG1VOFS = gPokenavStructPtr->menuVerticalOffset;
    return retVal;
}

bool8 SlideMenuHeaderDown(void)
{
    bool8 retVal = TRUE;

    if (gPokenavStructPtr->menuVerticalOffset == 0)
        return FALSE;

    gPokenavStructPtr->menuVerticalOffset -= 2;
    if (gPokenavStructPtr->menuVerticalOffset <= 0)
    {
        gPokenavStructPtr->menuVerticalOffset = 0;
        retVal = FALSE;
    }

    REG_BG1VOFS = gPokenavStructPtr->menuVerticalOffset;
    return retVal;
}

void DrawPokenavBottomToolbar(u8 toolbarId)
{
    u8 linkActive;

    sub_809D104((void *)VRAM + 0xD800, 0, 22, gPokenavBottomToolbarTilemap, 0, 0, 17, 2);

    switch (toolbarId)
    {
    case 0:
        sub_809D104((void *)VRAM + 0xD800, 0, 22, gPokenavBottomToolbarTilemap, 17, 0, 10, 2);
        sub_809D104((void *)VRAM + 0xD800, 10, 22, gPokenavBottomToolbarTilemap, 0, 6, 7, 2);
        break;
    case 11:
        sub_809D104((void *)VRAM + 0xD800, 8, 22, gPokenavBottomToolbarTilemap, 0, 6, 7, 2);
        break;
    case 2:
        linkActive = gPokenavStructPtr->unk6DAC;
        if (!linkActive)
        {
            sub_809D104((void *)VRAM + 0xD800, 0, 22, gPokenavBottomToolbarTilemap, 10, 2, 10, 2);
            sub_809D104((void *)VRAM + 0xD800, 10, 22, gPokenavBottomToolbarTilemap, linkActive, 6, 7, 2);
        }
        else
        {
            sub_809D104((void *)VRAM + 0xD800, 10, 22, gPokenavBottomToolbarTilemap, 0, 6, 7, 2);
        }
        break;
    case 3:
        sub_809D104((void *)VRAM + 0xD800, 0, 22, gPokenavBottomToolbarTilemap, 0, 4, 10, 2);
        sub_809D104((void *)VRAM + 0xD800, 10, 22, gPokenavBottomToolbarTilemap, 0, 6, 7, 2);
        break;
    case 4:
        sub_809D104((void *)VRAM + 0xD800, 0, 22, gPokenavBottomToolbarTilemap, 20, 2, 10, 2);
        sub_809D104((void *)VRAM + 0xD800, 10, 22, gPokenavBottomToolbarTilemap, 0, 6, 7, 2);
        break;
    case 7:
        sub_809D104((void *)VRAM + 0xD800, 0, 22, gPokenavBottomToolbarTilemap, 10, 4, 10, 2);
        sub_809D104((void *)VRAM + 0xD800, 7, 22, gPokenavBottomToolbarTilemap, 0, 6, 7, 2);
        break;
    case 8:
        sub_809D104((void *)VRAM + 0xD800, 0, 22, gPokenavBottomToolbarTilemap, 20, 4, 10, 2);
        sub_809D104((void *)VRAM + 0xD800, 7, 22, gPokenavBottomToolbarTilemap, 0, 6, 7, 2);
        break;
    case 5:
    case 9:
        sub_809D104((void *)VRAM + 0xD800, 0, 22, gPokenavBottomToolbarTilemap, 0, 2, 10, 2);
        sub_809D104((void *)VRAM + 0xD800, 8, 22, gPokenavBottomToolbarTilemap, 0, 6, 7, 2);
        break;
    case 10:
        sub_809D104((void *)VRAM + 0xD800, 8, 22, gPokenavBottomToolbarTilemap, 0, 6, 7, 2);
        break;
    }
}

void InitPokenavMenuGfx(u8 menuType)
{
    gPokenavStructPtr->setupStep = 0;

    if (!gPokenavStructPtr->unk6DAC)
    {
        while (LoadPokenavMenuGfxStep(menuType))
            ;
    }
}

bool8 LoadPokenavMenuGfxStep(u8 menuType)
{
    switch (gPokenavStructPtr->setupStep)
    {
    case 0:
        ResetPokenavBgOffsets();
        break;
    case 1:
        Text_LoadWindowTemplate(&gWindowTemplate_81E7224);
        break;
    case 2:
        MultistepInitMenuWindowBegin(&gWindowTemplate_81E7224);
        break;
    case 3:
        if (!MultistepInitMenuWindowContinue())
            return TRUE;
        break;
    case 4:
    DmaCopy16Defvars(3, gUnknown_083DFEEC, (void *)VRAM + 0x5000, 0xA0);
        break;
    case 5:
        LZ77UnCompVram(gUnknown_083DFF8C, (void *)VRAM + 0xF800);
        break;
    case 6:
    DmaCopy16Defvars(3, gUnknown_083E005C, (void *)VRAM + 0x8000, 0x20);
        break;
    case 7:
        LZ77UnCompVram(gUnknown_083E007C, (void *)VRAM + 0xE000);
        break;
    case 8:
        LZ77UnCompVram(gPokenavOutlineTilemap, (void *)VRAM + 0xE800);
        break;
    case 9:
        LZ77UnCompVram(gPokenavOutlineTiles, (void *)VRAM + 0x8020);
        break;
    case 10:
        SetPokenavMenuPalette(menuType);
        LoadPalette(gUnknown_083DFECC, 0xF0, 0x20);
        LoadPalette(gPokenavOutlinePalette, 0x40, 0x20);
        StartPokenavBg3Scroll();
        break;
    case 11:
        REG_BG0CNT = 0x1F01;
        REG_BG2CNT = 0x1D0A;
        REG_BG3CNT = 0x1C0B;
        REG_BLDCNT = 0;

        gPokenavStructPtr->setupStep++;
        return FALSE;
    default:
        return FALSE;
    }

    gPokenavStructPtr->setupStep++;
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

    tileBuffer = gUnknown_083DFEC8;
    AlignStringInMenuWindow(&tileBuffer[0x800], description, 0xC0, 2);
    Menu_PrintText(&tileBuffer[0x800], 3, 17);
}

void StartPokenavPaletteTransition(u8 menuType)
{
    u16 currentIndex, targetIndex;

    if (menuType == 2)
        menuType = 1;

    gPokenavStructPtr->paletteGradientTarget = menuType * 30;
    currentIndex = gPokenavStructPtr->paletteGradientIndex;
    targetIndex = menuType * 30;
    if (currentIndex < targetIndex)
        gPokenavStructPtr->paletteGradientDelta = 2;
    else if (currentIndex > targetIndex)
        gPokenavStructPtr->paletteGradientDelta = -2;
    else
        gPokenavStructPtr->paletteGradientDelta = 0;
}

bool8 UpdatePokenavPaletteTransition(void)
{
    u16 *gradient;

    if (gPokenavStructPtr->paletteGradientIndex == gPokenavStructPtr->paletteGradientTarget)
    {
        return FALSE;
    }
    else
    {
        gPokenavStructPtr->paletteGradientIndex = gPokenavStructPtr->paletteGradientDelta + gPokenavStructPtr->paletteGradientIndex;

        gradient = gPokenavStructPtr->palettesCE52;
        LoadPalette(&gradient[gPokenavStructPtr->paletteGradientIndex], 0x31, 4);
        return TRUE;
    }

}

void SetPokenavMenuPalette(u8 menuType)
{
    if (menuType == 2)
        menuType = 1;

    gPokenavStructPtr->paletteGradientIndex = menuType * 30;
    LoadPalette(&gPokenavStructPtr->palettesCE52[gPokenavStructPtr->paletteGradientIndex], 0x31, 4);
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
            gPokenavStructPtr->palettesCE52[i] = 0;
        break;
    case 1:
        gradient = gPokenavStructPtr->palettesCE52;
        colors = gUnknown_083E003C;
        BuildPokenavPaletteGradient(&colors[1], &colors[3], 16, 2, gradient);
        break;
    case 2:
        gradient = gPokenavStructPtr->palettesCE8E;
        colors = gUnknown_083E003C;
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
        s32 r1 = Q_24_8(GET_R(*startColors));
        s32 g1 = Q_24_8(GET_G(*startColors));
        s32 b1 = Q_24_8(GET_B(*startColors));
        s32 r2 = Q_24_8(GET_R(*endColors));
        s32 g2 = Q_24_8(GET_G(*endColors));
        s32 b2 = Q_24_8(GET_B(*endColors));
        s32 dr = (r2 - r1) / stepCount;
        s32 dg = (g2 - g1) / stepCount;
        s32 db = (b2 - b1) / stepCount;
        u16 rf, gf, bf;

        output = gradient;
        for (j = 0; j < stepCount - 1; j++)
        {
            rf = Q_24_8_TO_INT(r1);
            gf = Q_24_8_TO_INT(g1);
            bf = Q_24_8_TO_INT(b1);
            *output = RGB2(rf, gf, bf);
            output += colorCount;
            r1 += dr;
            g1 += dg;
            b1 += db;
        }
        rf = Q_24_8_TO_INT(r2);
        gf = Q_24_8_TO_INT(g2);
        bf = Q_24_8_TO_INT(b2);
        *output = RGB2(rf, gf, bf);
        startColors++;
        endColors++;
        gradient++;
    }
}

void UpdatePokenavBg3Scroll(void)
{
    gPokenavStructPtr->bg3ScrollFrameCounter = (gPokenavStructPtr->bg3ScrollFrameCounter + 1) & 1;
    if (gPokenavStructPtr->bg3ScrollFrameCounter)
        gPokenavStructPtr->bg3ScrollOffset++;

    REG_BG3HOFS = gPokenavStructPtr->bg3ScrollOffset;
}

void Task_PokenavBg3Scroll(u8 taskId)
{
    if (gTasks[taskId].data[0] == 0 || (gPokenavStructPtr->bg3ScrollOffset & 0x7) != 0)
    {
        UpdatePokenavBg3Scroll();
    }
    else
    {
        u16 value = gPokenavStructPtr->bg3ScrollOffset & 0x7;
        gPokenavStructPtr->bg3ScrollOffset = value;
        gPokenavStructPtr->bg3ScrollFrameCounter = value;
        REG_BG3HOFS = value;
    }
}

void StartPokenavBg3Scroll(void)
{
    gPokenavStructPtr->bg3ScrollOffset = 0;
    gPokenavStructPtr->bg3ScrollFrameCounter = 0;
    gPokenavStructPtr->bg3ScrollTaskId = CreateTask(Task_PokenavBg3Scroll, 80);
}

void StopPokenavBg3Scroll(void)
{
    if (FuncIsActiveTask(Task_PokenavBg3Scroll))
        DestroyTask(gPokenavStructPtr->bg3ScrollTaskId);
}

void InitPokenavRegionMapGfx(void)
{
    gPokenavStructPtr->setupStep = 0;

    if (gPokenavStructPtr->unk6DAC == 0)
    {
        while (LoadPokenavRegionMapGfxStep() != 0)
            ;
    }
}

bool8 LoadPokenavRegionMapGfxStep(void)
{
    switch (gPokenavStructPtr->setupStep)
    {
    case 0:
        ResetPokenavBgOffsets();
        break;
    case 1:
        Text_LoadWindowTemplate(&gWindowTemplate_81E7224);
        break;
    case 2:
        MultistepInitMenuWindowBegin(&gWindowTemplate_81E7224);
        break;
    case 3:
        if (!MultistepInitMenuWindowContinue())
            return TRUE;
        break;
    case 4:
        Menu_EraseScreen();
        break;
    case 5:
        sub_80FA904(&gPokenavStructPtr->regionMap, gSaveBlock2.regionMapZoom ? TRUE : FALSE);
        break;
    case 6:
        if (sub_80FA940())
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
        if (!gPokenavStructPtr->regionMap.zoomed)
        {
            gPokenavStructPtr->regionMapBg0YOffset = 160;
            REG_BG0VOFS = 160;
        }
        else
        {
            gPokenavStructPtr->regionMapBg0YOffset = 256;
            REG_BG0VOFS = 0;
        }

        REG_BG0CNT = REG_BG0CNT;
        REG_BG0CNT |= 1;
        REG_BLDCNT = 0;
        break;
    default:
        return FALSE;
    }

    gPokenavStructPtr->setupStep++;
    return TRUE;
}

asm(".include \"constants/gba_constants.inc\"\n");

void UpdateMapSecInfoWindow(void)
{
    bool8 someBool = FALSE;
    u16 top = 4;
    u16 mapSectionId;
    u8 b;

    switch (gPokenavStructPtr->regionMap.unk16)
    {
    case 0:
        break;
    case 1:
    case 4:
        sub_8072A18(gPokenavStructPtr->regionMap.mapSectionName, 0x70, top * 8, 0x78, 1);
        top += 2;
        if (gLinkOpen == TRUE)
        {
            StartRegionMapLinkLandmarks();
            someBool = TRUE;
        }
        else
        {
            u16 i;

            for (i = 0; i < 4; i++)
            {
                const u8 *secName = GetLandmarkName(
                    gPokenavStructPtr->regionMap.mapSectionId,
                    gPokenavStructPtr->regionMap.everGrandeCityArea,
                    i);

                if (secName == NULL)
                    break;
                sub_8072A18(secName, 0x70, top * 8, 0x78, 1);
                top += 2;
            }
        }
        break;
    case 2:
        sub_8072A18(gPokenavStructPtr->regionMap.mapSectionName, 0x70, top * 8, 0x78, 1);
        top += 2;
        mapSectionId = gPokenavStructPtr->regionMap.mapSectionId;
        b = gPokenavStructPtr->regionMap.everGrandeCityArea;
        if (gPokenavStructPtr->cityMapTilemapPtrs[mapSectionId][b] != NULL)
        {
            Menu_BlankWindowRect(14, top, 15, 15);
            Menu_BlankWindowRect(26, top, 28, 15);
            sub_8095C8C((void *)(VRAM + 0xF800), 16, 6, gPokenavStructPtr->cityMapTilemapPtrs[mapSectionId][b], 0, 0, 10, 10, 10);
            top += 11;
        }
        break;
    case 3:
        sub_8072A18(gPokenavStructPtr->regionMap.mapSectionName, 0x70, top * 8, 0x78, 1);
        top += 2;
        break;
    }

    // Epic fail by the compiler at optimizing this.
    if (!someBool && top < 16)
        Menu_BlankWindowRect(14, top, 28, 15);

    if (gPokenavStructPtr->regionMap.unk16 == 2)
        ShowRegionMapCityBanner();
    else
        HideRegionMapCityBanner();
}

void UpdateRegionMapBottomToolbar(void)
{
    if (!gPokenavStructPtr->regionMap.zoomed)
        DrawPokenavBottomToolbar(8);
    else
        DrawPokenavBottomToolbar(7);
}

bool8 UpdateRegionMapBgYForZoom(bool8 zoomOut)
{
    bool8 active = TRUE;
    u16 offset = gPokenavStructPtr->regionMapBg0YOffset;

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

    gPokenavStructPtr->regionMapBg0YOffset = offset;
    REG_BG0VOFS = offset & 0xFF;

    return active;
}

void InitCityMapDecompression(void)
{
    gPokenavStructPtr->cityMapSectionIndex = 0;
    gPokenavStructPtr->cityMapBufferIndex = 0;
    InitRegionMapCityBanner();
}

bool8 DecompressCityMapsStep(void)
{
    u16 i;
    u8 sectionIndex;
    u16 bufferIndex;

    if (gPokenavStructPtr->cityMapSectionIndex >= 16)
        return FALSE;

    sectionIndex = gPokenavStructPtr->cityMapSectionIndex;
    bufferIndex = gPokenavStructPtr->cityMapBufferIndex;
    for (i = 0; i < 2; i++)
    {
        if (gPokenavCityMaps[sectionIndex][i] != 0)
        {
            LZ77UnCompVram(gPokenavCityMaps[sectionIndex][i], gPokenavStructPtr->cityMapTilemaps[bufferIndex]);
            gPokenavStructPtr->cityMapTilemapPtrs[sectionIndex][i] = gPokenavStructPtr->cityMapTilemaps[bufferIndex];
            bufferIndex++;
        }
        else
        {
            gPokenavStructPtr->cityMapTilemapPtrs[sectionIndex][i] = NULL;
        }
    }

    if (++gPokenavStructPtr->cityMapSectionIndex >= 16)
        return FALSE;

    gPokenavStructPtr->cityMapBufferIndex = bufferIndex;
    return TRUE;
}

void InitRegionMapCityBanner(void)
{
    gPokenavStructPtr->cityBannerVisible = 0;
    gPokenavStructPtr->cityBannerFrame = 47;
    gPokenavStructPtr->cityBannerState = 0;
    gPokenavStructPtr->cityBannerDelay = 0;
}

void ShowRegionMapCityBanner(void)
{
    gPokenavStructPtr->cityBannerVisible = 1;
    if (gPokenavStructPtr->cityBannerState == 1)
        gPokenavStructPtr->cityBannerState = 2;
}

void HideRegionMapCityBanner(void)
{
    sub_8095C8C((void *)VRAM + 0xF800, 14, 16, gPokenavRegionMapCityBannerBlank, 0, 0, 15, 1, 15);
    gPokenavStructPtr->cityBannerVisible = 0;
}

void DrawRegionMapCityBannerFrame(u8 unused)
{
    u16 var1 = 60 - gPokenavStructPtr->cityBannerFrame;

    if (var1 > 15)
        var1 = 15;

    if (gPokenavStructPtr->cityBannerVisible != 0)
    {
        sub_8095C8C((void *)VRAM + 0xF800, 14, 16, gPokenavRegionMapCityBannerFrames, gPokenavStructPtr->cityBannerFrame, 0, var1, 1, 60);

        if (var1 < 15)
        {
            u16 var2 = var1 + 14;

            sub_8095C8C((void *)VRAM + 0xF800, var2, 16, gPokenavRegionMapCityBannerFrames, 0, 0, (u16)(15 - var1), 1, 60);
        }
    }
}

void UpdateRegionMapCityBanner(void)
{
    u16 var1;
    u8 var2 = gPokenavStructPtr->cityBannerState;

    switch (var2)
    {
    case 0:
        var1 = ++gPokenavStructPtr->cityBannerFrame;

        if (var1 > 59)
            gPokenavStructPtr->cityBannerFrame = var2;

        DrawRegionMapCityBannerFrame(gPokenavStructPtr->cityBannerVisible);

        switch (gPokenavStructPtr->cityBannerFrame)
        {
        case 0:
        case 15:
        case 30:
        case 45:
            gPokenavStructPtr->cityBannerState = 1;
            gPokenavStructPtr->cityBannerDelay = 0;
            break;
        }
        break;
    case 1:
        var1 = ++gPokenavStructPtr->cityBannerDelay;
        if (var1 > 120)
        {
            gPokenavStructPtr->cityBannerDelay = 0;
            gPokenavStructPtr->cityBannerState = 0;
        }
        break;
    case 2:
        DrawRegionMapCityBannerFrame(1);
        gPokenavStructPtr->cityBannerState = 1;
        break;
    }
}

void InitConditionGraphScreen(void)
{
    gPokenavStructPtr->setupStep = 0;

    if (gPokenavStructPtr->unk6DAC == 0)
    {
        while (LoadConditionGraphScreenStep())
            ;
    }
}

bool8 LoadConditionGraphScreenStep(void)
{
    switch (gPokenavStructPtr->setupStep)
    {
    case 0:
        ResetPokenavBgOffsets();
        gPokenavStructPtr->unkD162 = 11;
        break;
    case 1:
        Text_LoadWindowTemplate(&gWindowTemplate_81E7080);
        break;
    case 2:
        MultistepInitMenuWindowBegin(&gWindowTemplate_81E7080);
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
        if (gPokenavStructPtr->isConditionGraphSearchMode == 1)
            sub_8095C8C((void *)VRAM + 0xF000, 0, 5, gUnknown_083E01AC, 0, 0, 9, 4, 9);
        break;
    case 10:
        LZ77UnCompVram(gConditionGraphData_Tilemap, (void *)VRAM + 0xB800);
        break;
    case 11:
        LoadPalette(gUnknown_083E0254, 0x30, 0x20);
        LoadPalette(gUnknownPalette_81E6692, 0xB0, 0x20);
        LoadPalette(&gPokenavConditionMenu2_Pal[2], 0xB1, 0x2);
        LoadPalette(&gPokenavConditionMenu2_Pal[16], 0xB5, 0x2);
        LoadPalette(&gPokenavConditionMenu2_Pal[30], 0xBF, 0x2);
        SetConditionGraphWindowRegs();
        break;
    case 12:
        PrintConditionGraphMonInfo(gPokenavStructPtr->unk8fe9);
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

    gPokenavStructPtr->setupStep++;
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
    Menu_PrintText(gPokenavStructPtr->unk8829[portraitSlot], 13, 1);

    if (gPokenavStructPtr->isConditionGraphSearchMode == 1)
    {
        Menu_PrintText(gPokenavStructPtr->unk88E9[portraitSlot], 13, 3);
        sub_80F443C(gPokenavStructPtr->unk8788, gPokenavStructPtr->unk893c[gPokenavStructPtr->unk87DC].unk2_5);
        Menu_PrintText(gPokenavStructPtr->unk8788, 1, 6);
    }
}

void InitPokenavListScreen(u8 listMode)
{
    gPokenavStructPtr->unk306 = 0;
    gPokenavStructPtr->listMode = listMode;

    if (gPokenavStructPtr->unk6DAC == 0)
    {
        while (LoadPokenavListScreenStep())
            ;
    }
}

bool8 LoadPokenavListScreenStep(void)
{
    const u16 *pointer;

    switch (gPokenavStructPtr->unk306)
    {
    case 0:
        ResetPokenavBgOffsets();

        gPokenavStructPtr->isRibbonsList = gPokenavStructPtr->listMode == POKENAV_LIST_RIBBONS;
        gPokenavStructPtr->unkD162 = 11;
        break;
    case 1:
        Text_LoadWindowTemplate(&gWindowTemplate_81E70D4);
        break;
    case 2:
        MultistepInitMenuWindowBegin(&gWindowTemplate_81E70D4);
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
        LZ77UnCompVram(gUnknown_08E9FC64, (void *)VRAM + 0xE800);
        break;
    case 6:
        LZ77UnCompVram(gPokenavConditionSearch2_Gfx, (void *)VRAM + 0x8000);
        break;
    case 7:
        LoadPalette(gUnknown_083E02B4, 0xB0, 0x20);
        LoadPalette(gUnknown_083E02B4, 0xF0, 0x20);
        LoadPalette(gUnknown_083E0334, 0x40, 0x20);

        if (gPokenavStructPtr->listMode == POKENAV_LIST_CONDITION_SEARCH)
        {
            LoadPalette(gPokenavConditionSearch2_Pal, 0x30, 0x20);
            gPlttBufferUnfaded[0] = gPokenavConditionSearch2_Pal[5];
            LoadPalette(gUnknownPalette_81E6692, 0xB0, 0x20);
            LoadPalette(&gUnknown_083E02B4[1], 0xB1, 0x2);
            LoadPalette(&gUnknown_083E02B4[8], 0xB5, 0x2);
            LoadPalette(&gPokenavConditionSearch2_Pal[5], 0xBF, 0x2);
        }
        else if (gPokenavStructPtr->listMode == POKENAV_LIST_RIBBONS)
        {
            LoadPalette(gUnknown_083E0274, 0x30, 0x20);
            gPlttBufferUnfaded[0] = gUnknown_083E0274[5];
            LoadPalette(gUnknownPalette_81E6692, 0xB0, 0x20);
            LoadPalette(&gUnknown_083E02B4[1], 0xB1, 0x2);
            LoadPalette(&gUnknown_083E02B4[8], 0xB5, 0x2);
            LoadPalette(&gUnknown_083E0274[5], 0xBF, 0x2);
        }
        else
        {
            LoadPalette(gUnknown_08E9F9E8, 0x30, 0x20);
            gPlttBufferUnfaded[0] = *(pointer = &gUnknown_08E9F9E8[5]);
            LoadPalette(gUnknown_083E0314, 0x50, 0x20);
            LoadPalette(&gUnknown_083E02B4[1], 0xB1, 0x2);
            LoadPalette(&gUnknown_083E02B4[8], 0xB5, 0x2);
            LoadPalette(pointer, 0xBF, 0x2);
            LoadPalette(pointer, 0x5F, 0x2);
        }
        break;
    case 8:
        if (gPokenavStructPtr->listMode != POKENAV_LIST_TRAINERS_EYES)
        {
            sub_8095C8C((void *)VRAM + 0xE800, 0, 5, gUnknown_08E9FD1C, 0, 0, 9, 4, 9);
        }
        else
        {
            sub_8095C8C((void *)VRAM + 0xE800, 0, 4, gUnknown_08E9FE54, 0, 0, 12, 10, 12);
            sub_8095C8C((void *)VRAM + 0xE800, 0, 8, gUnknown_08E9FD64, 0, 0, 12, 10, 12);
        }
        break;
    case 9:
        LZ77UnCompVram(gUnknown_083E0354, (void *)VRAM + 0x5000);
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
        if (gPokenavStructPtr->listMode != POKENAV_LIST_TRAINERS_EYES)
        {
            PrintPokenavListSelectionInfo();
        }
        else
        {
            PrintTrainerEyesListStats(0);
            PrintTrainerEyesLocation((u8)gPokenavStructPtr->listSelectedIndex);
        }
        break;
    case 14:
        REG_BG2CNT = 0x1D0A;
        REG_BG3CNT = 0x1E03;
        REG_BG0CNT = 0x1F01;
        REG_BG3VOFS = 0xF8;

        gPokenavStructPtr->listBg3YOffset = 0xF8;
        gPokenavStructPtr->listTilemapRow = 0;

        REG_BLDCNT = 0;
        gPokenavStructPtr->setupStep++;
        return FALSE;
    default:
        return FALSE;
    }

    gPokenavStructPtr->unk306++;
    return TRUE;
}

void BeginPokenavListScroll(s16 rowsToScroll)
{
    s16 tilemapRow;
    s16 listIndex;
    s16 wrappedTilemapRow;

    gPokenavStructPtr->listScrollPixelsRemaining = rowsToScroll * 16;
    gPokenavStructPtr->listScrollStep = (rowsToScroll == 1 || rowsToScroll == -1) ? 4 : 8;
    if (rowsToScroll < 0)
    {
        gPokenavStructPtr->listScrollStep *= -1;
        tilemapRow = rowsToScroll * 2 + gPokenavStructPtr->listTilemapRow;
        listIndex = rowsToScroll + gPokenavStructPtr->unk8770;
        if (listIndex < 0)
            listIndex += gPokenavStructPtr->unk8774 + 1;

        rowsToScroll *= -1;
    }
    else
    {
        tilemapRow = gPokenavStructPtr->listTilemapRow + 16;
        listIndex = gPokenavStructPtr->unk8772 + 1;
    }

    if (listIndex > gPokenavStructPtr->unk8774)
        listIndex = 0;

    wrappedTilemapRow = tilemapRow & 0x1F;
    QueuePokenavListRows(listIndex, wrappedTilemapRow, rowsToScroll);
}

bool8 UpdatePokenavListScroll(void)
{
    if (!PrintPokenavListRowStep())
    {
        if (gPokenavStructPtr->listScrollPixelsRemaining == 0)
            return FALSE;

        gPokenavStructPtr->listScrollPixelsRemaining -= gPokenavStructPtr->listScrollStep;
        gPokenavStructPtr->listBg3YOffset += gPokenavStructPtr->listScrollStep;
        gPokenavStructPtr->listBg3YOffset &= 0xFF;
        REG_BG3VOFS = gPokenavStructPtr->listBg3YOffset;
        if (gPokenavStructPtr->listScrollPixelsRemaining == 0)
        {
            gPokenavStructPtr->listTilemapRow = ((8 + gPokenavStructPtr->listBg3YOffset) & 0xFF) / 8;
            return FALSE;
        }
    }

    return TRUE;
}

void PrintPokenavListSelectionInfo(void)
{
    switch (gPokenavStructPtr->listMode)
    {
    case POKENAV_LIST_CONDITION_SEARCH:
        sub_80F443C(gPokenavStructPtr->unk8788, gPokenavStructPtr->unk893c[gPokenavStructPtr->listSelectedIndex].unk2_5);
        break;
    case POKENAV_LIST_RIBBONS:
        sub_80F445C(gPokenavStructPtr->unk8788, gPokenavStructPtr->listSelectedIndex + 1);
        break;
    default:
        return;
    }

    BasicInitMenuWindow(&gWindowTemplate_81E710C);
    Menu_PrintText(gPokenavStructPtr->unk8788, 1, 6);
}

void PrintTrainerEyesListStats(u8 drawStep)
{
    u32 numTrainerBattles;

    BasicInitMenuWindow(&gWindowTemplate_81E710C);
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
            gPokenavStructPtr->unk8788,
            gPokenavStructPtr->unk8774 + 1,
            STR_CONV_MODE_RIGHT_ALIGN,
            5);
        MenuPrint_RightAligned(gPokenavStructPtr->unk8788, 10, 11);
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
            gPokenavStructPtr->unk8788,
            numTrainerBattles,
            STR_CONV_MODE_RIGHT_ALIGN,
            5);
        MenuPrint_RightAligned(gPokenavStructPtr->unk8788, 10, 15);
        break;
    }
}

void ClearTrainerEyesListStats(void)
{
    BasicInitMenuWindow(&gWindowTemplate_81E710C);
    Menu_EraseWindowRect(0, 9, 11, 16);
}

void InitPokenavListRows(void)
{
    s16 visibleRowCount = (gPokenavStructPtr->unk8772 - gPokenavStructPtr->unk8770) + 1;
    if (visibleRowCount < 8)
        Menu_EraseWindowRect(12, 1, 31, 15);

    QueuePokenavListRows(gPokenavStructPtr->unk8770, 0, visibleRowCount);
}

bool8 LoadPokenavListRowsStep(void)
{
    return PrintPokenavListRowStep();
}

void QueuePokenavListRows(u16 firstListIndex, u16 firstTilemapRow, u16 rowCount)
{
    gPokenavStructPtr->listRowToDraw = firstListIndex;
    gPokenavStructPtr->listTilemapRowToDraw = firstTilemapRow;
    gPokenavStructPtr->listRowsToDraw = rowCount;
    gPokenavStructPtr->unk8786 = 0;
}

bool8 PrintPokenavListRowStep(void)
{
    s32 wrappedListIndex;
    if (gPokenavStructPtr->listRowsToDraw == 0)
        return FALSE;
    wrappedListIndex = 0;
    while (1)
    {
        gPokenavListRowPrinters[gPokenavStructPtr->listMode](gPokenavStructPtr->listRowToDraw, gPokenavStructPtr->listTilemapRowToDraw);
        if (--gPokenavStructPtr->listRowsToDraw == 0)
            return FALSE;
        if (++gPokenavStructPtr->listRowToDraw > gPokenavStructPtr->unk8774)
            gPokenavStructPtr->listRowToDraw = wrappedListIndex;
        gPokenavStructPtr->listTilemapRowToDraw += 2;
        gPokenavStructPtr->listTilemapRowToDraw &= 0x1F;
        break;
    }
    return TRUE;
}

void PrintPokenavMonListRow(u16 listIndex, u16 tilemapRow)
{
    u8 displayMode = gPokenavStructPtr->isRibbonsList == 0 ? 2 : 1;
    sub_80F4428(gPokenavStructPtr->unk8788, listIndex, displayMode);
    BasicInitMenuWindow(&gWindowTemplate_81E70D4);
    Menu_PrintText(gPokenavStructPtr->unk8788, 13, tilemapRow);
}

void PrintTrainerEyesListRow(u16 listIndex, u16 tilemapRow)
{
    sub_80F700C(gPokenavStructPtr->unk8788, listIndex);
    tilemapRow &= 0x1F;
    BasicInitMenuWindow(&gWindowTemplate_81E70D4);
    Menu_PrintTextPixelCoords(gPokenavStructPtr->unk8788, 97, tilemapRow * 8, 0);
    if (listIndex < gPokenavStructPtr->unkD158 && gPokenavStructPtr->trainersEye[listIndex].rematchNo != 0)
        sub_8095C8C((void *)VRAM + 0xF000, 29, tilemapRow, gUnknown_083E039C, 0, 0, 1, 2, 1);
    else
        sub_8095C8C((void *)VRAM + 0xF000, 29, tilemapRow, gUnknown_083E03A0, 0, 0, 1, 2, 1);
}

void InitTrainerEyesListErase(void)
{
    gPokenavStructPtr->trainerEyesRowStep = 0;
    gPokenavStructPtr->unk306 = 0;
}

bool8 EraseTrainerEyesListStep(void)
{
    int tilemapRow;
    if (gPokenavStructPtr->trainerEyesRowStep > 8)
    {
        return FALSE;
    }

    if (++gPokenavStructPtr->unk306 > 1)
    {
        gPokenavStructPtr->unk306 = 0;
        if (gPokenavStructPtr->trainerEyesRowStep < 8)
        {
            tilemapRow = (gPokenavStructPtr->listTilemapRow + (gPokenavStructPtr->trainerEyesRowStep * 2)) & 0x1F;
            if (gPokenavStructPtr->trainerEyesRowStep != gPokenavStructPtr->listCursorRow)
            {
                BasicInitMenuWindow(&gWindowTemplate_81E70D4);
                Menu_EraseWindowRect(12, tilemapRow, 31, tilemapRow + 1);
            }

            if (!gPokenavStructPtr->trainerEyesRowStep)
                ClearTrainerEyesListStats();

            gPokenavStructPtr->trainerEyesRowStep++;
            return TRUE;
        }
        else
        {
            u16 row;
            BasicInitMenuWindow(&gWindowTemplate_81E70D4);
            tilemapRow = (gPokenavStructPtr->listTilemapRow + 16) & 0x1F;
            for (row = 0; row < 8; row++)
            {
                Menu_EraseWindowRect(12, tilemapRow, 31, tilemapRow + 1);
                tilemapRow = (tilemapRow + 2) & 0x1F;
            }

            gPokenavStructPtr->trainerEyesRowStep++;
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
    gPokenavStructPtr->trainerEyesRowStep = 0;
    gPokenavStructPtr->unk306 = 0;
}

bool8 RestoreTrainerEyesListStep(void)
{
    if (gPokenavStructPtr->trainerEyesRowStep > 7)
        return FALSE;

    if (++gPokenavStructPtr->unk306 > 1)
    {
        gPokenavStructPtr->unk306 = 0;
        BasicInitMenuWindow(&gWindowTemplate_81E70D4);
        PrintTrainerEyesListRow(gPokenavStructPtr->unk8770 + gPokenavStructPtr->trainerEyesRowStep,
                                gPokenavStructPtr->listTilemapRow + gPokenavStructPtr->trainerEyesRowStep * 2);

        if ((++gPokenavStructPtr->trainerEyesRowStep) > 7)
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

    gPokenavStructPtr->unk306 = 0;
    gPokenavStructPtr->trainerEyesRowStep = 0;
    trainerEyesId = gPokenavStructPtr->trainersEye[gPokenavStructPtr->listSelectedIndex].rematchTableIdx;
    gPokenavStructPtr->trainerEyeDescriptionLines[0] = gTrainerEyeDescriptions[trainerEyesId];

    // Find the start of the 3 other lines in the Trainer's Eyes description.
    curChar = gPokenavStructPtr->trainerEyeDescriptionLines[0];
    for (i = 0; i < 3; i++)
    {
        while (*curChar != EOS)
            curChar++;
        gPokenavStructPtr->trainerEyeDescriptionLines[i + 1] = ++curChar;
    }
}

bool8 PrintTrainerEyesDescriptionStep(void)
{
    u32 tilemapRow;

    if (gPokenavStructPtr->trainerEyesRowStep == 7)
        return FALSE;
    if (++gPokenavStructPtr->unk306 < 2)
        return TRUE;
    gPokenavStructPtr->unk306 = 0;
    BasicInitMenuWindow(&gWindowTemplate_81E70D4);
    tilemapRow = (gPokenavStructPtr->listTilemapRow + 2 + gPokenavStructPtr->trainerEyesRowStep * 2) & 0x1F;
#ifndef NONMATCHING
    asm("":::"r2"); // fakematch
#endif //NONMATCHING
    switch (gPokenavStructPtr->trainerEyesRowStep)
    {
    default:
        return FALSE;
    case 0:
        Menu_PrintTextPixelCoords(gOtherText_Strategy, 0x61, tilemapRow * 8, 0);
        break;
    case 1:
        AlignStringInMenuWindow(gPokenavStructPtr->unk8788, gPokenavStructPtr->trainerEyeDescriptionLines[0], 0x88, 0);
        Menu_PrintTextPixelCoords(gPokenavStructPtr->unk8788, 0x61, tilemapRow * 8, 0);
        break;
    case 2:
        Menu_PrintTextPixelCoords(gOtherText_TrainersPokemon, 0x61, tilemapRow * 8, 0);
        break;
    case 3:
        AlignStringInMenuWindow(gPokenavStructPtr->unk8788, gPokenavStructPtr->trainerEyeDescriptionLines[1], 0x88, 0);
        Menu_PrintTextPixelCoords(gPokenavStructPtr->unk8788, 0x61, tilemapRow * 8, 0);
        break;
    case 4:
        Menu_PrintTextPixelCoords(gOtherText_SelfIntroduction, 0x61, tilemapRow * 8, 0);
        break;
    case 5:
        AlignStringInMenuWindow(gPokenavStructPtr->unk8788, gPokenavStructPtr->trainerEyeDescriptionLines[2], 0x88, 0);
        Menu_PrintTextPixelCoords(gPokenavStructPtr->unk8788, 0x61, tilemapRow * 8, 0);
        break;
    case 6:
        AlignStringInMenuWindow(gPokenavStructPtr->unk8788, gPokenavStructPtr->trainerEyeDescriptionLines[3], 0x88, 0);
        Menu_PrintTextPixelCoords(gPokenavStructPtr->unk8788, 0x61, tilemapRow * 8, 0);
        return FALSE;
    }
    gPokenavStructPtr->trainerEyesRowStep++;
    return TRUE;
}

void InitTrainerEyesDescriptionErase(void)
{
    gPokenavStructPtr->unk306 = 0;
    gPokenavStructPtr->trainerEyesRowStep = 0;
    StringFill(gPokenavStructPtr->unk8788, CHAR_SPACE, 16);
}

bool8 EraseTrainerEyesDescriptionStep(void)
{
    int tilemapRow;
    if (gPokenavStructPtr->trainerEyesRowStep > 6)
        return FALSE;

    if (++gPokenavStructPtr->unk306 > 1)
    {
        gPokenavStructPtr->unk306 = 0;
        tilemapRow = (gPokenavStructPtr->listTilemapRow + 2 + gPokenavStructPtr->trainerEyesRowStep * 2) & 0x1F;
        BasicInitMenuWindow(&gWindowTemplate_81E70D4);
        Menu_EraseWindowRect(12, tilemapRow, 31, tilemapRow + 1);
        gPokenavStructPtr->trainerEyesRowStep++;
    }

    return TRUE;
}

void BeginTrainerEyesDetailEnterScroll(void)
{
    s16 rowsToScroll = gPokenavStructPtr->listCursorRow;
    gPokenavStructPtr->listScrollPixelsRemaining = rowsToScroll * 16;
    gPokenavStructPtr->listScrollStep = rowsToScroll == 1 ? 4 : 8;
}

void BeginTrainerEyesDetailExitScroll(void)
{
    s16 rowsToScroll = gPokenavStructPtr->listCursorRow * -1;
    gPokenavStructPtr->listScrollPixelsRemaining = rowsToScroll * 16;
    gPokenavStructPtr->listScrollStep = rowsToScroll == -1 ? -4 : -8;
}

bool8 UpdateTrainerEyesDetailScroll(void)
{
    return UpdatePokenavListScroll();
}

void PrintTrainerEyesLocation(u8 listIndex)
{
    GetMapSectionName(gPokenavStructPtr->unk8788, gPokenavStructPtr->trainersEye[listIndex].regionMapSectionId, 0);
    BasicInitMenuWindow(&gWindowTemplate_81E710C);
    TruncateTrainerEyesLocationAtControlCode(gPokenavStructPtr->unk8788);
    AlignStringInMenuWindow(gPokenavStructPtr->unkD138, gPokenavStructPtr->unk8788, 88, 2);
    Menu_PrintText(gPokenavStructPtr->unkD138, 0, 5);
}

void RedrawSelectedTrainerEyesListRow(void)
{
    PrintTrainerEyesListRow(gPokenavStructPtr->listSelectedIndex, gPokenavStructPtr->listTilemapRow);
}

bool8 LoadRibbonsSummaryScreenStep(void)
{
    switch (gPokenavStructPtr->setupStep)
    {
    case 0:
        ResetPokenavBgOffsets();
        gPokenavStructPtr->unkD162 = 11;
        break;
    case 1:
        Text_LoadWindowTemplate(&gWindowTemplate_81E70B8);
        break;
    case 2:
        MultistepInitMenuWindowBegin(&gWindowTemplate_81E70B8);
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
        LZ77UnCompWram(gPokenavRibbonsSummaryTilemap, gPokenavStructPtr->ribbonsSummaryTilemap);
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
        LZ77UnCompVram(gPokenavRibbonIconsHalf_Gfx, (void *)(VRAM + 0x8200));
        break;
    case 12:
        LoadPalette(gPokenavRibbonView_Pal, 0x20, 0x20);
        LoadPalette(gUnknown_083E03A8, 0xF0, 0x20);
        LoadPalette(gUnknown_083E3C60[0], 0x30, 0xA0);
        LoadPalette(gUnknownPalette_81E6692, 0xB0, 0x20);
        LoadPalette(gUnknown_083E03A8 + 0xF, 0xBF, 0x2);
        break;
    case 13:
        PrintRibbonsSummaryMonInfo();
        break;
    case 14:
        REG_BG2CNT = 0x1E02;
        REG_BG3CNT = 0x170B;
        REG_BLDCNT = 0;
        gPokenavStructPtr->setupStep++;
        return FALSE;
    default:
        return FALSE;
    }

    gPokenavStructPtr->setupStep++;
    return TRUE;
}

void DrawMonRibbonIcons(void)
{
    u16 i;
    u16 offset;
    u8 ribbonId, palette, tile;
    u8 normalRibbonCount;

    offset = 0x8B;
    normalRibbonCount = gPokenavStructPtr->ribbonCount - gPokenavStructPtr->giftRibbonCount;

    for (i = 0; i < 8; i++)
    CpuFill16(0x2000, &gPokenavStructPtr->ribbonsSummaryTilemap[offset + i * 32], 0x24);

    for (i = 0; i < normalRibbonCount; i++)
    {
        ribbonId = gPokenavStructPtr->ribbonIds[i];
        palette = gPokenavRibbonsIconGfx[ribbonId][1] + 3;
        tile = gPokenavRibbonsIconGfx[ribbonId][0] * 2 + 0x10;
        gPokenavStructPtr->ribbonsSummaryTilemap[offset] = (palette << 12) | tile;
        gPokenavStructPtr->ribbonsSummaryTilemap[offset + 1] = ((palette << 12) | 0x400) | tile;
        gPokenavStructPtr->ribbonsSummaryTilemap[offset + 0x20] = (palette << 12) | (tile + 1);
        gPokenavStructPtr->ribbonsSummaryTilemap[offset + 0x21] = (palette << 12) | 0x400 | (tile + 1);

        if ((i + 1) % 9 == 0)
            offset += 0x30;
        else
            offset += 2;
    }

    offset = 0x14B;
    for (i = 0; i < gPokenavStructPtr->giftRibbonCount; i++)
    {
        ribbonId = gPokenavStructPtr->ribbonIds[normalRibbonCount + i];
        palette = gPokenavRibbonsIconGfx[ribbonId][1] + 3;
        tile = gPokenavRibbonsIconGfx[ribbonId][0] * 2 + 0x10;
        gPokenavStructPtr->ribbonsSummaryTilemap[offset] = (palette << 12) | tile;
        gPokenavStructPtr->ribbonsSummaryTilemap[offset + 1] = ((palette << 12) | 0x400) | tile;
        gPokenavStructPtr->ribbonsSummaryTilemap[offset + 0x20] = (palette << 12) | (tile + 1);
        gPokenavStructPtr->ribbonsSummaryTilemap[offset + 0x21] = (palette << 12) | 0x400 | (tile + 1);

        offset += 2;
    }
}

void CopyRibbonsSummaryTilemapToVram(void)
{
    u16 *src = gPokenavStructPtr->ribbonsSummaryTilemap;
    u16 *dest = (u16 *)(VRAM + 0xB800);
    DmaCopy32(3, src, dest, 0x500);
    gPlttBufferUnfaded[0] = *(gPokenavRibbonView_Pal + 14);
}

void PrintRibbonsSummaryMonInfo(void)
{
    Menu_PrintText(gPokenavStructPtr->unk8829[0], 13, 1);
    sub_80F445C(gPokenavStructPtr->unk8788, gPokenavStructPtr->listSelectedIndex + 1);
    Menu_PrintText(gPokenavStructPtr->unk8788, 1, 5);
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
    u8 *arr;
    u8 *tileBuffer1 = &gUnknown_083DFEC8[0x800];
    u8 *tileBuffer2 = &gUnknown_083DFEC8[0xA98];

    if (gPokenavStructPtr->ribbonPageIndex < 3)
    {
        gPokenavRibbonDescriptionId = gPokenavStructPtr->ribbonPageIndex * 9 + gPokenavStructPtr->ribbonCursorPos;
        gPokenavRibbonDescriptionId = gPokenavStructPtr->ribbonIds[gPokenavRibbonDescriptionId];
        AlignStringInMenuWindow(tileBuffer1, gRibbonDescriptions[gPokenavRibbonDescriptionId][0], 128, 0);
        AlignStringInMenuWindow(tileBuffer2, gRibbonDescriptions[gPokenavRibbonDescriptionId][1], 128, 0);
    }
    else
    {
        gPokenavRibbonDescriptionId = gPokenavStructPtr->ribbonCount - gPokenavStructPtr->giftRibbonCount;
        gPokenavRibbonDescriptionId = gPokenavStructPtr->ribbonIds[gPokenavRibbonDescriptionId + gPokenavStructPtr->ribbonCursorPos];

        // FIXME!
        arr = ((u8*)&gSaveBlock1);
        asm("ldrh r1, [r5]\n\
            add r0, r0, r1");
        gPokenavRibbonDescriptionId = arr[0x30F7];
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
            AlignStringInMenuWindow(tileBuffer1, gEmptyString_81E72B0, 128, 0);
            AlignStringInMenuWindow(tileBuffer2, gEmptyString_81E72B0, 128, 0);
        }
    }

    Menu_PrintText(tileBuffer1, 12, 13);
    Menu_PrintText(tileBuffer2, 12, 15);
}

void PrintRibbonsSummaryCount(void)
{
    u8 *buffer;
    Menu_EraseWindowRect(12, 13, 27, 16);
    buffer = StringCopy(gPokenavStructPtr->unk8788, gOtherText_Ribbons);
    buffer[0] = CHAR_SPACE;
    buffer++;
    buffer = ConvertIntToDecimalStringN(
        buffer,
        gPokenavStructPtr->unk893c[gPokenavStructPtr->unk87DC].unk0,
        STR_CONV_MODE_LEFT_ALIGN,
        2);
    buffer[0] = EOS;
    Menu_PrintText(gPokenavStructPtr->unk8788, 12, 13);
}

void InitPokenavMonInfoHeaderGfx(void)
{
    gPokenavStructPtr->monInfoHeaderGfxStep = 0;
}

bool8 LoadPokenavMonInfoHeaderGfxStep(u8 forRibbons)
{
    switch (gPokenavStructPtr->monInfoHeaderGfxStep)
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
            LoadPalette(gUnknown_083E0124, 0xD0, 0x20);
        else
            LoadPalette(gUnknown_083E0144, 0xD0, 0x20);

        gPokenavStructPtr->monInfoHeaderXOffset = -80;
        REG_BG0CNT = 0x1D0D;
        gPokenavStructPtr->monInfoHeaderGfxStep++;
        return FALSE;
    default:
        return FALSE;
    }

    gPokenavStructPtr->monInfoHeaderGfxStep++;
    return TRUE;
}

bool8 SlidePokenavMonInfoHeaderIn(void)
{
    gPokenavStructPtr->monInfoHeaderXOffset += 0x10;
    if (gPokenavStructPtr->monInfoHeaderXOffset > 0)
        gPokenavStructPtr->monInfoHeaderXOffset = 0;

    return gPokenavStructPtr->monInfoHeaderXOffset != 0;
}

bool8 SlidePokenavMonInfoHeaderOut(void)
{
    gPokenavStructPtr->monInfoHeaderXOffset -= 0x10;
    if (gPokenavStructPtr->monInfoHeaderXOffset < -0x50)
        gPokenavStructPtr->monInfoHeaderXOffset = -0x50;

    return gPokenavStructPtr->monInfoHeaderXOffset != -0x50;
}

bool8 LoadPokeblockConditionGraphScreenStep(void)
{
    switch (gPokenavStructPtr->setupStep)
    {
    case 0:
        ResetPokenavBgOffsets();
        gPokenavStructPtr->unkD162 = 2;
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
        sub_8095C8C((void *)VRAM + 0xF000, 0, 13, gUnknown_083E01F4, 0, 0, 12, 4, 12);
        break;
    case 6:
        LZ77UnCompVram(gConditionGraphData_Tilemap, (void *)(VRAM + 0xB800));
        break;
    case 7:
        LoadPalette(gUnknown_083E0254, 0x30, 0x20);
        LoadPalette(gUnknownPalette_81E6692, 0xB0, 0x20);
        LoadPalette(&gPokenavConditionMenu2_Pal[2], 0xB1, 0x2);
        LoadPalette(&gPokenavConditionMenu2_Pal[16], 0xB5, 0x2);
        LoadPalette(&gPokenavConditionMenu2_Pal[30], 0xBF, 0x2);
        SetConditionGraphWindowRegs();
        break;
    case 8:
        PrintConditionGraphMonInfo(gPokenavStructPtr->unk8fe9);
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

    gPokenavStructPtr->setupStep++;
    return TRUE;
}

void PrintPokeblockMonNature(void)
{
    u8 *buffer = gPokenavStructPtr->unk8788;
    if (gPokenavStructPtr->unk893c[gPokenavStructPtr->unk87DC].unk3_14)
    {
        u8 nature = GetNature(&gPlayerParty[sub_8137124(gPokenavStructPtr->unk87DC)]);
        buffer = StringCopy(buffer, gOtherText_Nature2);
        AlignStringInMenuWindow(buffer, gNatureNames[nature], 87, 0);
    }
    else
    {
        AlignStringInMenuWindow(buffer, gEmptyString_81E72B0, 87, 0);
    }

    Menu_PrintTextPixelCoords(gPokenavStructPtr->unk8788, 1, 112, 1);
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
    register u8 *ptr asm("r5") = gRegionMapLinkLandmarkPrintState;
    if (ptr[0] == 1)
    {
        const u8 *landmarkName = GetLandmarkName(
            gPokenavStructPtr->regionMap.mapSectionId,
            gPokenavStructPtr->regionMap.everGrandeCityArea,
            ptr[1]);

        if (landmarkName)
        {
            sub_8072A18(landmarkName, 0x70, 4 * (ptr[1] * 4 + 12), 0x78, 1);
            if (++ptr[1] != 4)
                return;
        }

        Menu_BlankWindowRect(14, ptr[1] * 2 + 6, 28, 15);
        ptr[0] = 0;
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
    gPokenavStructPtr->unk306 = 0;
    if (gPokenavStructPtr->unk6DAC == 0)
        while (LoadPokenavMenuOptionGfxStep());
}

bool8 LoadPokenavMenuOptionGfxStep(void)
{
    u16 i, j;

    switch (gPokenavStructPtr->unk306)
    {
    case 0:
        for (i = 0; i < 6; i++)
        {
            for (j = 0; j < 4; j++)
                gPokenavStructPtr->menuOptionSprites[i][j] = NULL;
        }

        gPokenavStructPtr->menuOptionBlendMode = 0;
        break;
    case 1:
        LZ77UnCompWram(gPokenavMenuOptions_Gfx, gPokenavStructPtr->menuOptionsGfx);
        break;
    case 2:
        LZ77UnCompWram(gPokenavConditionMenu_Gfx, gPokenavStructPtr->conditionMenuGfx);
        break;
    case 3:
        LZ77UnCompWram(gPokenavConditionSearch_Gfx, gPokenavStructPtr->conditionSearchGfx);
        return FALSE;
    }

    gPokenavStructPtr->unk306++;
    return TRUE;
}

void InitPokenavMenuOptionSprites(u8 menuType)
{
    gPokenavStructPtr->unk306 = 0;
    if (gPokenavStructPtr->unk6DAC == 0)
        while (LoadPokenavMenuOptionSpritesStep(menuType));
}

bool8 LoadPokenavMenuOptionSpritesStep(u8 menuType)
{
    u16 animNum;
    u16 topOffset;
    u16 height;
    u16 middle;
    u8 spriteId;

    switch (gPokenavStructPtr->unk306)
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
            gPokenavStructPtr->menuOptionRowCount = 5;
            break;
        case 1:
            topOffset = 56;
            height = 20;
            gPokenavStructPtr->menuOptionRowCount = 3;
            break;
        case 2:
            topOffset = 40;
            height = 16;
            gPokenavStructPtr->menuOptionRowCount = 6;
            break;
        default:
            return FALSE;
        }

        animNum = 0;
        for (i = 0; i < gPokenavStructPtr->menuOptionRowCount; i++)
        {
            middle = (height * i) + topOffset - 8;
            gPokenavStructPtr->menuOptionWin0V[i] = (middle << 8) | (middle + 0x11);
            if (!menuType)
            {
                if (gPokenavStructPtr->mainMenuItemIds[i] == 0)
                {
                    for (j = 0; j < 4; j++)
                        gPokenavStructPtr->menuOptionSprites[i][j] = NULL;
                    continue;
                }
                else
                {
                    animNum = (gPokenavStructPtr->mainMenuItemIds[i] - 1) * 4;
                }
            }

            for (j = 0; j < 4; j++)
            {
                spriteId = CreateSprite(&gSpriteTemplate_83E4454, j * 32 + 256, (height * i) + topOffset, 0);
                if (spriteId == MAX_SPRITES)
                    return FALSE;
                gPokenavStructPtr->menuOptionSprites[i][j] = &gSprites[spriteId];
                gPokenavStructPtr->menuOptionSprites[i][j]->data[0] = i;
                gPokenavStructPtr->menuOptionSprites[i][j]->data[1] = j;
                gPokenavStructPtr->menuOptionSprites[i][j]->data[2] = j * 32 + 152;
                gPokenavStructPtr->menuOptionSprites[i][j]->data[3] = j * 32 + 256;
                StartSpriteAnim(gPokenavStructPtr->menuOptionSprites[i][j], animNum++);

                if ((menuType == 2 || menuType == 0) && i > 2)
                {
                    gPokenavStructPtr->menuOptionSprites[i][j]->oam.paletteNum = IndexOfSpritePaletteTag(0x1);
                }
            }
        }
    }
        break;
    default:
        return FALSE;
    }

    gPokenavStructPtr->unk306++;
    return TRUE;
}

void StartMenuOptionSpritesSlideIn(void)
{
    u16 i, j;
    for (i = 0; i < gPokenavStructPtr->menuOptionRowCount; i++)
    {
        for (j = 0; j < 4; j++)
        {
            if (gPokenavStructPtr->menuOptionSprites[i][j])
                gPokenavStructPtr->menuOptionSprites[i][j]->callback = SpriteCB_SlideMenuOptionIn;
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

    gPokenavStructPtr->menuOptionExitStep = 0;
    StopMenuOptionHighlight();
    for (i = 0; i < gPokenavStructPtr->menuOptionRowCount; i++)
    {
        if (i != gPokenavStructPtr->menuCursorPos)
        {
            for (j = 0; j < 4; j++)
            {
                if (gPokenavStructPtr->menuOptionSprites[i][j])
                    gPokenavStructPtr->menuOptionSprites[i][j]->callback = SpriteCB_SlideMenuOptionOut;
            }
        }
    }
}

bool8 UpdateMenuOptionSpritesSlideOut(void)
{
    u16 j;

    switch (gPokenavStructPtr->menuOptionExitStep)
    {
    case 0:
        if (AreOtherMenuOptionSpritesGone())
        {
            for (j = 0; j < 4; j++)
            {
                struct Sprite *sprite = gPokenavStructPtr->menuOptionSprites[gPokenavStructPtr->menuCursorPos][j];
                sprite->oam.affineMode = ST_OAM_AFFINE_DOUBLE;
                sprite->affineAnims = gSpriteAffineAnimTable_83E4450;
                InitSpriteAffineAnim(sprite);
                sprite->data[4] = j * 4 - 6;
                sprite->data[4] /= 2;
                sprite->data[5] = sprite->data[4] * 8;
                sprite->callback = SpriteCB_SlideMenuOptionOut;
            }

            StartSelectedMenuOptionFadeOut();
            gPokenavStructPtr->menuOptionExitStep++;
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
    if (gPokenavStructPtr->menuOptionBlendMode == 1)
    {
        REG_WIN0V = gPokenavStructPtr->menuOptionWin0V[gPokenavStructPtr->menuCursorPos];
        REG_BLDY = gSineTable[gPokenavStructPtr->menuOptionBlendFrame] >> 5;
        gPokenavStructPtr->menuOptionBlendFrame += 3;
        gPokenavStructPtr->menuOptionBlendFrame &= 0x7F;
    }
    else if (gPokenavStructPtr->menuOptionBlendMode == 2)
    {
        REG_BLDALPHA = gUnknown_083E42F8[gPokenavStructPtr->menuOptionBlendFrame];
        if (gPokenavStructPtr->menuOptionBlendFrame < 15)
            gPokenavStructPtr->menuOptionBlendFrame++;
    }
}

void ResetMenuOptionBlendOnPokenavExit(void)
{
    StopMenuOptionHighlight();
}

void EnableMenuOptionHighlightWindow(void)
{
    REG_WIN0H = 0x77F0;
    REG_WIN0V = gPokenavStructPtr->menuOptionWin0V[gPokenavStructPtr->menuCursorPos];
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
    if (!gPokenavStructPtr->menuOptionBlendMode)
    {
        gPokenavStructPtr->menuOptionBlendMode = 1;
        gPokenavStructPtr->menuOptionBlendFrame = 0;
        REG_BLDCNT = 0x90;
        REG_BLDY = 0;
        EnableMenuOptionHighlightWindow();
    }
}

void StopMenuOptionHighlight(void)
{
    gPokenavStructPtr->menuOptionBlendMode = 0;
    REG_BLDCNT = 0;
    DisableMenuOptionHighlightWindow();
}

void StartSelectedMenuOptionFadeOut(void)
{
    u16 j;

    if (!gPokenavStructPtr->menuOptionBlendMode)
    {
        DisableMenuOptionHighlightWindow();
        for (j = 0; j < 4; j++)
        {
            struct Sprite *sprite = gPokenavStructPtr->menuOptionSprites[gPokenavStructPtr->menuCursorPos][j];
            sprite->oam.objMode = ST_OAM_OBJ_BLEND;
        }

        gPokenavStructPtr->menuOptionBlendMode = 2;
        gPokenavStructPtr->menuOptionBlendFrame = 0;
        REG_BLDCNT = 0x3F40;
        REG_BLDALPHA = 0x10;
    }
}

void StopMenuOptionFadeOut()
{
    gPokenavStructPtr->menuOptionBlendMode = 0;
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
    if (sprite->data[0] == gPokenavStructPtr->menuCursorPos)
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
    if (sprite->data[0] == gPokenavStructPtr->menuCursorPos)
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

    for (i = 0; i < gPokenavStructPtr->menuItemCount; i++)
    {
        for (j = 0; j < 4; j++)
        {
            struct Sprite *sprite = gPokenavStructPtr->menuOptionSprites[i][j];
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

    for (i = 0; i < gPokenavStructPtr->menuOptionRowCount; i++)
    {
        if (i != gPokenavStructPtr->menuCursorPos)
        {
            for (j = 0; j < 4; j++)
            {
                struct Sprite *sprite = gPokenavStructPtr->menuOptionSprites[i][j];
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
        struct Sprite *sprite = gPokenavStructPtr->menuOptionSprites[gPokenavStructPtr->menuCursorPos][j];
        if (sprite)
            return FALSE;
    }

    return TRUE;
}

void DestroyMenuOptionSprite(struct Sprite *sprite)
{
    gPokenavStructPtr->menuOptionSprites[sprite->data[0]][sprite->data[1]] = NULL;
    if (sprite->affineAnimEnded)
        FreeOamMatrix(sprite->oam.matrixNum);

    DestroySprite(sprite);
}

void LoadMenuOptionSpriteSheet(u8 menuType)
{
    switch (menuType)
    {
    case 0:
        gPokenavStructPtr->menuOptionSpriteSheet.data = gPokenavStructPtr->menuOptionsGfx;
        gPokenavStructPtr->menuOptionSpriteSheet.size = sizeof(gPokenavStructPtr->menuOptionsGfx);
        gPokenavStructPtr->menuOptionSpriteSheet.tag = 0x0;
        break;
    case 1:
        gPokenavStructPtr->menuOptionSpriteSheet.data = gPokenavStructPtr->conditionMenuGfx;
        gPokenavStructPtr->menuOptionSpriteSheet.size = sizeof(gPokenavStructPtr->conditionMenuGfx);
        gPokenavStructPtr->menuOptionSpriteSheet.tag = 0x0;
        break;
    case 2:
        gPokenavStructPtr->menuOptionSpriteSheet.data = gPokenavStructPtr->conditionSearchGfx;
        gPokenavStructPtr->menuOptionSpriteSheet.size = sizeof(gPokenavStructPtr->conditionSearchGfx);
        gPokenavStructPtr->menuOptionSpriteSheet.tag = 0x0;
        break;
    default:
        return;
    }

    LoadSpriteSheet(&gPokenavStructPtr->menuOptionSpriteSheet);
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

    gPokenavStructPtr->blueLightSprite = NULL;
    if (DoesSomeoneWantRematchIn(gSaveBlock1.location.mapGroup, gSaveBlock1.location.mapNum) == TRUE)
    {
        LoadSpriteSheet(&gSpriteSheet_PokenavBlueLight);
        LoadSpritePalette(&gSpritePalette_PokenavBlueLight);
        spriteId = CreateSprite(&gPokenavBlueLightSpriteTemplate, 12, 96, 0);
        if (spriteId != MAX_SPRITES)
        {
            gPokenavStructPtr->blueLightSprite = &gSprites[spriteId];
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
    if (gPokenavStructPtr->blueLightSprite)
    {
        DestroySprite(gPokenavStructPtr->blueLightSprite);
        FreeSpriteTilesByTag(0x19);
        FreeSpritePaletteByTag(0x11);
        gPokenavStructPtr->blueLightSprite = NULL;
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
    gPokenavStructPtr->unk306 = 0;
    if (!gPokenavStructPtr->unk6DAC)
        while(LoadPokenavMenuHeaderGfxStep());
}

bool8 LoadPokenavMenuHeaderGfxStep(void)
{
    switch (gPokenavStructPtr->unk306)
    {
    case 0:
        LZ77UnCompWram(gPokenavMainMenuHeader_Gfx, gPokenavStructPtr->mainMenuHeaderGfx);
        break;
    case 1:
        LZ77UnCompWram(gPokenavConditionMenuHeader_Gfx, gPokenavStructPtr->conditionMenuHeaderGfx);
        break;
    case 2:
        LZ77UnCompWram(gPokenavRibbonsHeader_Gfx, gPokenavStructPtr->ribbonsHeaderGfx);
        break;
    case 3:
        LZ77UnCompWram(gPokenavHoennMapHeader_Gfx, gPokenavStructPtr->hoennMapHeaderGfx);
        break;
    case 4:
        LZ77UnCompWram(gPokenavConditionHeaderLabels_Gfx, gPokenavStructPtr->searchHeaderGfx);
        break;
    case 5:
        LZ77UnCompWram(gPokenavConditionHeaderLabels2_Gfx, gPokenavStructPtr->smartHeaderGfx);
        break;
    case 6:
        LZ77UnCompWram(gPokenavTrainersEyesHeader_Gfx, gPokenavStructPtr->trainersEyesHeaderGfx);
        break;
    case 7:
        LoadSpritePalettes(gUnknown_083E449C);
        break;
    default:
        return FALSE;
    }

    gPokenavStructPtr->unk306++;
    return TRUE;
}

void LoadPokenavMenuHeaderSpriteSheet(u8 headerType)
{
    struct SpriteSheet spriteSheet;

    switch (headerType)
    {
    case 0:
        spriteSheet.data = gPokenavStructPtr->mainMenuHeaderGfx;
        spriteSheet.size = sizeof(gPokenavStructPtr->mainMenuHeaderGfx);
        spriteSheet.tag = 0x1;
        break;
    case 1:
        spriteSheet.data = gPokenavStructPtr->conditionMenuHeaderGfx;
        spriteSheet.size = sizeof(gPokenavStructPtr->conditionMenuHeaderGfx);
        spriteSheet.tag = 0x1;
        break;
    case 3:
        spriteSheet.data = gPokenavStructPtr->trainersEyesHeaderGfx;
        spriteSheet.size = sizeof(gPokenavStructPtr->trainersEyesHeaderGfx);
        spriteSheet.tag = 0x1;
        break;
    case 2:
        spriteSheet.data = gPokenavStructPtr->ribbonsHeaderGfx;
        spriteSheet.size = sizeof(gPokenavStructPtr->ribbonsHeaderGfx);
        spriteSheet.tag = 0x1;
        break;
    case 4:
        spriteSheet.data = gPokenavStructPtr->hoennMapHeaderGfx;
        spriteSheet.size = sizeof(gPokenavStructPtr->hoennMapHeaderGfx);
        spriteSheet.tag = 0x1;
        break;
    case 5:
        spriteSheet.data = gPokenavStructPtr->searchHeaderGfx;
        spriteSheet.size = sizeof(gPokenavStructPtr->searchHeaderGfx);
        spriteSheet.tag = 0x2;
        break;
    case 6:
        spriteSheet.data = gPokenavStructPtr->partyHeaderGfx;
        spriteSheet.size = sizeof(gPokenavStructPtr->partyHeaderGfx);
        spriteSheet.tag = 0x2;
        break;
    case 8:
        spriteSheet.data = gPokenavStructPtr->beautyHeaderGfx;
        spriteSheet.size = sizeof(gPokenavStructPtr->beautyHeaderGfx);
        spriteSheet.tag = 0x2;
        break;
    case 9:
        spriteSheet.data = gPokenavStructPtr->cuteHeaderGfx;
        spriteSheet.size = sizeof(gPokenavStructPtr->cuteHeaderGfx);
        spriteSheet.tag = 0x2;
        break;
    case 11:
        spriteSheet.data = gPokenavStructPtr->toughHeaderGfx;
        spriteSheet.size = sizeof(gPokenavStructPtr->toughHeaderGfx);
        spriteSheet.tag = 0x2;
        break;
    case 10:
        spriteSheet.data = gPokenavStructPtr->smartHeaderGfx;
        spriteSheet.size = sizeof(gPokenavStructPtr->smartHeaderGfx);
        spriteSheet.tag = 0x2;
        break;
    case 7:
        spriteSheet.data = gPokenavStructPtr->coolHeaderGfx;
        spriteSheet.size = sizeof(gPokenavStructPtr->coolHeaderGfx);
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
        y = 49 - gPokenavStructPtr->menuVerticalOffset;
        deltaX = 8;
        endX = 32;
        width = 64;
        sprites = gPokenavStructPtr->largeHeaderSprites;
        spriteTemplate = headerType != 3 ? &gPokenavLargeHeaderSpriteTemplate : &gPokenavTrainersEyesHeaderSpriteTemplate;
        break;
    case 4:
        initialX = 272;
        y = 49 - gPokenavStructPtr->menuVerticalOffset;
        deltaX = -8;
        endX = 152;
        width = 64;
        spriteTemplate = &gPokenavLargeHeaderSpriteTemplate;
        sprites = gPokenavStructPtr->largeHeaderSprites;
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
        y = 68 - gPokenavStructPtr->menuVerticalOffset;
        deltaX = 8;
        endX = 16;
        width = 32;
        sprites = gPokenavStructPtr->smallHeaderSprites;
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
                int anim = !gPokenavStructPtr->regionMap.zoomed ? 1 : 2;
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
    int anim = !gPokenavStructPtr->regionMap.zoomed ? 1 : 2;
    StartSpriteAnim(sprite, anim);
}

void BeginPokenavLeftHeaderLoad(u8 headerType)
{
    gPokenavStructPtr->unk306 = 0;
    if (!gPokenavStructPtr->unk6DAC)
        while (LoadPokenavLeftHeaderStep(headerType));
}

bool8 LoadPokenavLeftHeaderStep(u8 headerType)
{
    switch (gPokenavStructPtr->unk306)
    {
    case 0:
        LoadPokenavMenuHeaderSpriteSheet(headerType);
        gPokenavStructPtr->unk306++;
        return TRUE;
    case 1:
        CreatePokenavLeftHeaderSprites(headerType);
        gPokenavStructPtr->unk306++;
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
            gPokenavStructPtr->largeHeaderSprites[i]->callback = SpriteCB_SlideLeftHeaderOut;
    }
    else
    {
        for (i = 0; i < 2; i++)
            gPokenavStructPtr->smallHeaderSprites[i]->callback = SpriteCB_SlideLeftHeaderOut;
    }
}

void DestroyPokenavLeftHeaderSprites(u8 headerType)
{
    u16 i;

    if (headerType < 5)
    {
        FreeSpriteTilesByTag(0x1);
        for (i = 0; i < 2; i++)
            DestroySprite(gPokenavStructPtr->largeHeaderSprites[i]);
    }
    else
    {
        FreeSpriteTilesByTag(0x2);
        for (i = 0; i < 2; i++)
            DestroySprite(gPokenavStructPtr->smallHeaderSprites[i]);
    }
}

void CreatePokenavRegionMapIcons(void)
{
    CreateRegionMapCursor(7, 7);
    CreateRegionMapPlayerIcon(8, 8);
    sub_80FBF94();
}

void FreePokenavRegionMapIcons(void)
{
    FreeRegionMapIconResources();
}

void SpriteCB_UpdatePokenavPortraitSpriteX(struct Sprite *sprite)
{
    sprite->x = gPokenavStructPtr->monInfoHeaderXOffset + 38;
}

void CreateOrUpdatePokenavPortraitSprite(u8 portraitSlot)
{
    u8 spriteId;
    struct SpriteTemplate spriteTemplate;
    struct SpritePalette spritePalette;
    struct SpriteSheet spriteSheet;

    if (!gPokenavStructPtr->portraitSprite)
    {
        spriteSheet = gPokenavPortraitSpriteSheet;
        spriteTemplate = gPokenavPortraitSpriteTemplate;
        spritePalette = gPokenavPortraitSpritePalette;

        spriteSheet.data = gPokenavStructPtr->spriteGfxBuffers[portraitSlot];
        spritePalette.data = gPokenavStructPtr->unk0[portraitSlot];
        gPokenavStructPtr->portraitPaletteOffset = LoadSpritePalette(&spritePalette);
        gPokenavStructPtr->portraitTileStart = LoadSpriteSheet(&spriteSheet);

        spriteId = CreateSprite(&spriteTemplate, 38, 104, 0);
        if (spriteId == MAX_SPRITES)
        {
            FreeSpriteTilesByTag(0x6);
            FreeSpritePaletteByTag(0x6);
            gPokenavStructPtr->portraitSprite = NULL;
        }
        else
        {
            gPokenavStructPtr->portraitSprite = &gSprites[spriteId];
            gPokenavStructPtr->portraitVramDest = (void *)(VRAM + 0x10000) + gPokenavStructPtr->portraitTileStart * 32;
            gPokenavStructPtr->portraitPaletteOffset = gPokenavStructPtr->portraitPaletteOffset * 16 + 0x100;
        }
    }
    else
    {
        DmaCopy16Defvars(3, gPokenavStructPtr->spriteGfxBuffers[portraitSlot], gPokenavStructPtr->portraitVramDest, 0x800);
        LoadPalette(gPokenavStructPtr->unk0[portraitSlot], gPokenavStructPtr->portraitPaletteOffset, 0x20);
    }
}

void DestroyPokenavPortraitSprite(void)
{
    if (gPokenavStructPtr->portraitSprite)
    {
        DestroySprite(gPokenavStructPtr->portraitSprite);
        FreeSpriteTilesByTag(0x6);
        FreeSpritePaletteByTag(0x6);
        gPokenavStructPtr->portraitSprite = NULL;
    }
}

void CreateOrUpdateTrainerEyesPortrait(u8 portraitSlot)
{
    CreateOrUpdatePokenavPortraitSprite(portraitSlot);
    gPokenavStructPtr->trainerEyesPortraitSprite = gPokenavStructPtr->portraitSprite;
    gPokenavStructPtr->trainerEyesPortraitSprite->callback = SpriteCB_UpdateTrainerEyesPortraitPosition;
}

void DestroyTrainerEyesPortrait(void)
{
    if (gPokenavStructPtr->trainerEyesPortraitSprite)
    {
        DestroySprite(gPokenavStructPtr->trainerEyesPortraitSprite);
        FreeSpriteTilesByTag(0x6);
        FreeSpritePaletteByTag(0x6);
        gPokenavStructPtr->trainerEyesPortraitSprite = NULL;
        gPokenavStructPtr->portraitSprite = NULL;
    }
}

void SpriteCB_UpdateTrainerEyesPortraitPosition(struct Sprite *sprite)
{
    sprite->x = gPokenavStructPtr->trainerEyesPortraitXOffset + 40;
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
        gPokenavStructPtr->listRightArrowSprite = NULL;
    }
    else
    {
        gPokenavStructPtr->listRightArrowSprite = &gSprites[spriteId];
        for (i = 0; i < 2; i++)
        {
            spriteId = CreateSprite(&gPokenavListUpDownArrowSpriteTemplate, 168, i * 128 + 8, 0);
            if (spriteId != MAX_SPRITES)
            {
                gPokenavStructPtr->listUpDownArrowSprites[i] = &gSprites[spriteId];
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
                gPokenavStructPtr->listUpDownArrowSprites[i] = NULL;
            }
        }
    }
}

void DestroyPokenavListArrowSprites(void)
{
    u16 i;

    if (gPokenavStructPtr->listRightArrowSprite)
    {
        DestroySprite(gPokenavStructPtr->listRightArrowSprite);
        FreeSpriteTilesByTag(0x9);
        FreeSpritePaletteByTag(0x9);
        gPokenavStructPtr->listRightArrowSprite = NULL;
    }

    for (i = 0; i < 2; i++)
    {
        if (gPokenavStructPtr->listUpDownArrowSprites[i])
        {
            DestroySprite(gPokenavStructPtr->listUpDownArrowSprites[i]);
            gPokenavStructPtr->listUpDownArrowSprites[i] = NULL;
        }
    }

    FreeSpriteTilesByTag(0xA);
}

void SpriteCB_UpdatePokenavListRightArrow(struct Sprite *sprite)
{
    sprite->y = gPokenavStructPtr->listCursorRow * 16 + 16;
}

void SpriteCB_UpdatePokenavListUpDownArrow(struct Sprite *sprite)
{
    if (gPokenavStructPtr->hasListScrollArrows)
    {
        if (sprite->data[4])
        {
            if (!sprite->data[3])
                sprite->invisible = gPokenavStructPtr->unk8770 == 0;
            else
                sprite->invisible = gPokenavStructPtr->unk8772 == gPokenavStructPtr->unk8774;

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
        if (gPokenavStructPtr->listUpDownArrowSprites[i])
            gPokenavStructPtr->listUpDownArrowSprites[i]->data[4] = 1;
    }
}

void TogglePokenavListArrows(u8 invisible)
{
    gPokenavStructPtr->listRightArrowSprite->invisible = invisible;
    if (gPokenavStructPtr->hasListScrollArrows)
    {
        if (invisible == 1)
        {
            gPokenavStructPtr->listUpDownArrowSprites[0]->invisible = invisible;
            gPokenavStructPtr->listUpDownArrowSprites[1]->invisible = invisible;
        }
        else
        {
            gPokenavStructPtr->listUpDownArrowSprites[0]->data[4] = 1;
            gPokenavStructPtr->listUpDownArrowSprites[1]->data[4] = 1;
        }
    }
}

void SpriteCB_ConditionPartyPokeball(struct Sprite *sprite)
{
    if (sprite->data[0] == gPokenavStructPtr->unk87DC)
        StartSpriteAnim(sprite, 0);
    else
        StartSpriteAnim(sprite, 1);
}

void HighlightCurrentPartyIndexPokeball(struct Sprite *sprite)
{
    if (gPokenavStructPtr->unk87DC == gPokenavStructPtr->unk87DA - 1)
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

    for (i = 0; i < gPokenavStructPtr->unk87DA - 1; i++)
    {
        spriteId = CreateSprite(&spriteTemplate, 226, i * 20 + 8, 0);
        if (spriteId != MAX_SPRITES)
        {
            gPokenavStructPtr->conditionPartyIconSprites[i] = &gSprites[spriteId];
            gPokenavStructPtr->conditionPartyIconSprites[i]->data[0] = i;
        }
        else
        {
            gPokenavStructPtr->conditionPartyIconSprites[i] = NULL;
        }
    }

    spriteTemplate.tileTag = 0x4;
    spriteTemplate.callback = SpriteCallbackDummy;
    for (; i < 6; i++)
    {
        spriteId = CreateSprite(&spriteTemplate, 230, i * 20 + 8, 0);
        if (spriteId != MAX_SPRITES)
        {
            gPokenavStructPtr->conditionPartyIconSprites[i] = &gSprites[spriteId];
            gPokenavStructPtr->conditionPartyIconSprites[i]->oam.size = 0;
        }
        else
        {
            gPokenavStructPtr->conditionPartyIconSprites[i] = NULL;
        }
    }

    spriteTemplate.tileTag = 0x5;
    spriteTemplate.callback = HighlightCurrentPartyIndexPokeball;
    spriteId = CreateSprite(&spriteTemplate, 222, i * 20 + 8, 0);
    if (spriteId != MAX_SPRITES)
    {
        gPokenavStructPtr->conditionPartyIconSprites[i] = &gSprites[spriteId];
        gPokenavStructPtr->conditionPartyIconSprites[i]->oam.shape = ST_OAM_H_RECTANGLE;
        gPokenavStructPtr->conditionPartyIconSprites[i]->oam.size = 2;
    }
    else
    {
        gPokenavStructPtr->conditionPartyIconSprites[i] = NULL;
    }
}

void DestroyConditionPartyPokeballIndicators(void)
{
    u16 i;

    for (i = 0; i < 7; i++)
    {
        if (gPokenavStructPtr->conditionPartyIconSprites[i])
        {
            DestroySprite(gPokenavStructPtr->conditionPartyIconSprites[i]);
            gPokenavStructPtr->conditionPartyIconSprites[i] = NULL;
        }
    }
}

void CreateConditionMonMarkingsSprite(void)
{
    struct Sprite *sprite;

    gPokenavStructPtr->conditionMarkingsMenu.baseTileTag = 0x1C;
    gPokenavStructPtr->conditionMarkingsMenu.basePaletteTag = 0x13;
    sub_80F727C(&gPokenavStructPtr->conditionMarkingsMenu);
    sub_80F7404();
    sprite = sub_80F7920(27, 21, gPokenavConditionMonMarkingsPalette);
    sprite->oam.priority = 3;
    sprite->x = 192;
    sprite->y = 32;
    sprite->callback = MonMarkingsCallback;
    gPokenavStructPtr->conditionMonMarkingsSprite = sprite;
}

void DestroyConditionMonMarkingsSprite(void)
{
    DestroySprite(gPokenavStructPtr->conditionMonMarkingsSprite);
    FreeSpriteTilesByTag(0x1B);
    FreeSpritePaletteByTag(0x15);
}

void MonMarkingsCallback(struct Sprite *sprite)
{
    StartSpriteAnim(sprite, gPokenavStructPtr->conditionMonMarkings[gPokenavStructPtr->unk8fe9]);
}

void OpenConditionMonMarkingsMenu(void)
{
    sub_80F7418(gPokenavStructPtr->conditionMonMarkings[gPokenavStructPtr->unk8fe9], 176, 32);
}

void SaveAndCloseConditionMonMarkingsMenu(void)
{
    struct UnkUsePokeblockSub *var0 = &gPokenavStructPtr->unk893c[gPokenavStructPtr->unk87DC];
    gPokenavStructPtr->conditionMonMarkings[gPokenavStructPtr->unk8fe9] = gPokenavStructPtr->conditionMarkingsMenu.markings;
    SetMonMarkings(var0->unk1, var0->partyIdx, gPokenavStructPtr->conditionMarkingsMenu.markings);
    sub_80F7470();
}

void sub_80F36F0(void)
{
    gPokenavStructPtr->unk306 = 0;
    if (!gPokenavStructPtr->unk6DAC)
        while (sub_80F3724());
}

bool8 sub_80F3724(void)
{
    switch (gPokenavStructPtr->unk306)
    {
    case 0:
        LZ77UnCompWram(gUnknown_083E3D00, gPokenavStructPtr->unk984C);
        break;
    case 1:
        sub_80F379C();
        gPokenavStructPtr->unk306++;
        // fall through
    case 2:
        if (sub_80F37D0())
            return TRUE;
        break;
    default:
        return FALSE;
    }

    gPokenavStructPtr->unk306++;
    return TRUE;
}

void sub_80F379C(void)
{
    gPokenavStructPtr->unkBC93 = 0;
    if (!gPokenavStructPtr->unk6DAC)
        while (sub_80F37D0());
}

bool8 sub_80F37D0(void)
{
    u16 i;
    u8 j, k, l, m;

    if (gPokenavStructPtr->unkBC93 > 11)
        return FALSE;

    for (i = 0; i < 2; i++)
    {
        u8 *r4 = &gPokenavStructPtr->unk984C[gPokenavStructPtr->unkBC93][0];
        u8 *r5 = &gPokenavStructPtr->unkA44C[gPokenavStructPtr->unkBC93][0];
        for (j = 0; j < 4; j++)
        {
            CpuFastSet(r4, r5, 0x10);
            r5 += 0x40;
            r4 += 0x20;
            for (k = 0; k < 2; k++)
            {
                for (l = 0; l < 8; l++)
                {
                    r4 += 4;
                    for (m = 0; m < 4; m++)
                    {
                        r4 -= 1;
                        *r5 = (*r4 << 4) | ((*r4 >> 4) & 0xF);
                        r5++;
                    }

                    r4 += 4;
                }

                r4 -= 0x40;
            }

            r4 += 0x60;
        }

        if (++gPokenavStructPtr->unkBC93 > 11)
            return FALSE;
    }

    if (gPokenavStructPtr->unkBC93 > 11)
        return FALSE;

    return TRUE;
}

void sub_80F38B8(void)
{
    gPokenavStructPtr->unk306 = 0;
    if (!gPokenavStructPtr->unk6DAC)
        while (sub_80F38EC());
}

bool8 sub_80F38EC(void)
{
    switch (gPokenavStructPtr->unk306)
    {
    case 0:
        gPokenavStructPtr->unk9348 = NULL;
        gPokenavStructPtr->unkBC92 = 0;
        break;
    case 1:
        CreateOrUpdatePokenavPortraitSprite(0);
        break;
    case 2:
        sub_80F3970();
        gPokenavStructPtr->unk306++;
        // fall through
    case 3:
        if (sub_80F39A4())
            return TRUE;
        break;
    default:
        return FALSE;
    }

    gPokenavStructPtr->unk306++;
    return TRUE;
}

void sub_80F3970(void)
{
    gPokenavStructPtr->unkBC93 = 0;
    if (!gPokenavStructPtr->unk6DAC)
        while (sub_80F39A4());
}

bool8 sub_80F39A4(void)
{
    struct SpriteSheet spriteSheet;
    struct SpritePalette spritePalette;
    if (gPokenavStructPtr->unkBC93 > 11)
        return FALSE;

    spriteSheet.data = &gPokenavStructPtr->unkA44C[gPokenavStructPtr->unkBC93][0];
    spriteSheet.size = 0x200;
    spriteSheet.tag = gPokenavStructPtr->unkBC93 + 11;
    LoadSpriteSheet(&spriteSheet);
    if (gPokenavStructPtr->unkBC93 < 5)
    {
        spritePalette.data = gUnknown_083E3C60[gPokenavStructPtr->unkBC93];
        spritePalette.tag = gPokenavStructPtr->unkBC93 + 10;
        LoadSpritePalette(&spritePalette);
    }

    if (++gPokenavStructPtr->unkBC93 > 11)
        return FALSE;

    return TRUE;
}

struct Sprite *sub_80F3A3C(u16 arg0, u16 arg1)
{
    struct SpriteTemplate spriteTemplate;
    u16 var0;
    u8 ribbon;
    u8 spriteId;

    if (arg1 < 3)
        var0 = arg0 + arg1 * 9;
    else
        var0 = arg0 + (gPokenavStructPtr->ribbonCount - gPokenavStructPtr->giftRibbonCount);

    ribbon = gPokenavStructPtr->ribbonIds[var0];
    spriteTemplate = gSpriteTemplate_83E476C;
    spriteTemplate.tileTag = gPokenavRibbonsIconGfx[ribbon][0] + 11;
    spriteTemplate.paletteTag = gPokenavRibbonsIconGfx[ribbon][1] + 10;
    spriteId = CreateSprite(&spriteTemplate, arg0 * 16 + 96, arg1 * 16 + 40, 2);
    if (spriteId != MAX_SPRITES)
        return &gSprites[spriteId];
    else
        return NULL;
}

void sub_80F3B00(void)
{
    gPokenavStructPtr->unk9348 = sub_80F3A3C(gPokenavStructPtr->ribbonCursorPos, gPokenavStructPtr->ribbonPageIndex);
    if (gPokenavStructPtr->unk9348)
    {
        StartSpriteAffineAnim(gPokenavStructPtr->unk9348, 1);
        gPokenavStructPtr->unkBC92 = 1;
    }
    else
    {
        gPokenavStructPtr->unkBC92 = 0;
    }
}

bool8 sub_80F3B58(void)
{
    if (gPokenavStructPtr->unkBC92)
    {
        gPokenavStructPtr->unkBC92 = !gPokenavStructPtr->unk9348->affineAnimEnded;
        return gPokenavStructPtr->unkBC92;
    }
    else
    {
        return FALSE;
    }
}

void sub_80F3B94(void)
{
    if (gPokenavStructPtr->unk9348)
    {
        StartSpriteAffineAnim(gPokenavStructPtr->unk9348, 2);
        gPokenavStructPtr->unkBC92 = 1;
    }
    else
    {
        gPokenavStructPtr->unkBC92 = 0;
    }
}

bool8 sub_80F3BD4(void)
{
    if (gPokenavStructPtr->unkBC92)
    {
        gPokenavStructPtr->unkBC92 = !gPokenavStructPtr->unk9348->affineAnimEnded;
        if (!gPokenavStructPtr->unkBC92)
        {
            FreeOamMatrix(gPokenavStructPtr->unk9348->oam.matrixNum);
            DestroySprite(gPokenavStructPtr->unk9348);
            gPokenavStructPtr->unk9348 = NULL;
        }

        return gPokenavStructPtr->unkBC92;
    }
    else
    {
        return FALSE;
    }
}

void sub_80F3C2C(void)
{
    u16 i;

    if (gPokenavStructPtr->unk9348)
    {
        FreeOamMatrix(gPokenavStructPtr->unk9348->oam.matrixNum);
        DestroySprite(gPokenavStructPtr->unk9348);
        gPokenavStructPtr->unk9348 = NULL;
    }

    for (i = 0; i < 12; i++)
        FreeSpriteTilesByTag(i + 0xB);

    for (i = 0; i < 5; i++)
        FreeSpritePaletteByTag(i + 0xA);

    DestroyPokenavPortraitSprite();
}

void sub_80F3C94(void)
{
    u16 i;
    struct SpriteSheet spriteSheet;
    struct SpritePalette spritePalette;

    spriteSheet = gUnknown_083E4784;
    spritePalette = gUnknown_083E478C;
    LoadSpriteSheet(&spriteSheet);
    LoadSpritePalette(&spritePalette);
    for (i = 0; i < 10; i++)
        gPokenavStructPtr->unk8800[i] = NULL;
}

void sub_80F3CE8(void)
{
    move_anim_execute();
    FreeSpriteTilesByTag(0x17);
    FreeSpritePaletteByTag(0xF);
}

void sub_80F3D00(void)
{
    u8 spriteId;
    u16 i;
    u8 var1;
    struct UnkUsePokeblockSub *var0 = &gPokenavStructPtr->unk893c[gPokenavStructPtr->unk87DC];

    if (!var0->unk3_14)
        return;

    var1 = gPokenavStructPtr->unk8931[gPokenavStructPtr->unk8fe9];
    for (i = 0; i < var1 + 1; i++)
    {
        spriteId = CreateSprite(&gSpriteTemplate_83E4800, 0, 0, 0);
        if (spriteId != MAX_SPRITES)
        {
            gPokenavStructPtr->unk8800[i] = &gSprites[spriteId];
            gPokenavStructPtr->unk8800[i]->invisible = TRUE;
        }
        else
        {
            break;
        }
    }

    sub_80F3F20(var1, 1);
}

void move_anim_execute(void)
{
    u16 i;

    for (i = 0; i < 10; i++)
    {
        if (!gPokenavStructPtr->unk8800[i])
            return;

        DestroySprite(gPokenavStructPtr->unk8800[i]);
        gPokenavStructPtr->unk8800[i] = NULL;
    }
}

void sub_80F3DDC(struct Sprite *sprite)
{
    if (++sprite->data[1] > 60)
    {
        sprite->data[1] = 0;
        sub_80F3F20(sprite->data[2], 0);
    }
}

void sub_80F3E04(struct Sprite *sprite)
{
    if (sprite->animEnded)
    {
        sprite->data[1] = 0;
        sprite->callback = sub_80F3DDC;
    }
}

void sub_80F3E24(struct Sprite *sprite)
{
    if (gPokenavStructPtr->portraitSprite)
    {
        sprite->x = gPokenavStructPtr->portraitSprite->x
                         + gPokenavStructPtr->portraitSprite->x2
                         + gUnknown_083E4794[sprite->data[0]][0];
        sprite->y = gPokenavStructPtr->portraitSprite->y
                         + gPokenavStructPtr->portraitSprite->y2
                         + gUnknown_083E4794[sprite->data[0]][1];
    }
    else
    {
        sprite->x = gUnknown_083E4794[sprite->data[0]][0] + 40;
        sprite->y = gUnknown_083E4794[sprite->data[0]][1] + 104;
    }
}

void sub_80F3E9C(struct Sprite *sprite)
{
    if (sprite->data[1])
    {
        if (--sprite->data[1])
            return;

        SeekSpriteAnim(sprite, 0);
        sprite->invisible = FALSE;
    }

    sub_80F3E24(sprite);
    if (sprite->animEnded)
    {
        sprite->invisible = TRUE;
        if (sprite->data[3] == sprite->data[2])
        {
            if (sprite->data[3] == 9)
            {
                sub_80F3FAC();
                sprite->callback = sub_80F3E04;
            }
            else
            {
                sprite->callback = sub_80F3DDC;
            }
        }
        else
        {
            sprite->callback = SpriteCallbackDummy;
        }
    }
}

void sub_80F3F20(u8 arg0, u8 arg1)
{
    u16 i;

    for (i = 0; i < 10; i++)
    {
        if (gPokenavStructPtr->unk8800[i])
        {
            gPokenavStructPtr->unk8800[i]->data[0] = i;
            gPokenavStructPtr->unk8800[i]->data[1] = i * 16 + 1;
            gPokenavStructPtr->unk8800[i]->data[2] = arg0;
            gPokenavStructPtr->unk8800[i]->data[3] = i;

            if (!arg1 || arg0 != 9)
            {
                gPokenavStructPtr->unk8800[i]->callback = sub_80F3E9C;
            }
            else
            {
                sub_80F3E24(gPokenavStructPtr->unk8800[i]);
                sub_80F3FAC();
                gPokenavStructPtr->unk8800[i]->callback = sub_80F3E04;
                gPokenavStructPtr->unk8800[i]->invisible = FALSE;
            }
        }
    }
}

void sub_80F3FAC(void)
{
    u16 i;

    for (i = 0; i < 10; i++)
    {
        if (gPokenavStructPtr->unk8800[i])
        {
            SeekSpriteAnim(gPokenavStructPtr->unk8800[i], 0);
            gPokenavStructPtr->unk8800[i]->invisible = FALSE;
        }
    }
}

void sub_80F3FF0(void)
{
    gPokenavStructPtr->unk306 = 0;
    if (!gPokenavStructPtr->unk6DAC)
        while (sub_80F4024());
}

bool8 sub_80F4024(void)
{
    u8 paletteIndex;
    u8 spriteId;
    struct SpritePalette spritePalette;

    switch (gPokenavStructPtr->unk306)
    {
    case 0:
        LZ77UnCompWram(gUnknown_083E329C, gPokenavStructPtr->unk131E4);
        break;
    case 1:
    {
        struct SpriteSheet spriteSheet = {
            .data = gPokenavStructPtr->unk131E4,
            .size = sizeof(gPokenavStructPtr->unk131E4),
            .tag = 0x18,
        };
        LoadSpriteSheet(&spriteSheet);
        break;
    }
    case 2:
        spritePalette = gUnknown_083E4818;
        LoadSpritePalette(&spritePalette);
        paletteIndex = IndexOfSpritePaletteTag(0x10);
        gPokenavStructPtr->unk308 = -3 & ~(1 << (paletteIndex + 0x10));
        break;
    case 3:
        spriteId = CreateSprite(&gSpriteTemplate_83E4850, 218, 14, 0);
        if (spriteId != MAX_SPRITES)
        {
            gPokenavStructPtr->unk6D98 = &gSprites[spriteId];
            gPokenavStructPtr->unk6D98->data[0] = 0;
        }
        else
        {
            gPokenavStructPtr->unk6D98 = NULL;
        }

        gPokenavStructPtr->unk306++;
        return FALSE;
    default:
        return FALSE;
    }

    gPokenavStructPtr->unk306++;
    return TRUE;
}

void sub_80F4138(struct Sprite *sprite)
{
    sprite->y2 = -gPokenavStructPtr->menuVerticalOffset;
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

void sub_80F4194(u8 *arg0, u8 *text)
{
    u8 i;
    u8 *tileBuffer;
    u32 *tileBuf2;

    tileBuffer = gUnknown_083DFEC8;
    DmaFill16(3, 0x1111, tileBuffer, 0x280);
    DmaFill16Defvars(3, 0x1111, 0x400 + tileBuffer, 0x280);
    Text_InitWindow8004E3C(&gWindowTemplate_81E70F0, tileBuffer, text);

    DmaClear16(3, tileBuffer + 0x220, 0x60);
    DmaClear16(3, tileBuffer + 0x620, 0x60);

    tileBuf2 = (int *)tileBuffer + 0x80;
    tileBuf2[0] &= 0x0FFFFFFF;
    tileBuf2[1] &= 0x0FFFFFFF;
    tileBuf2[2] &= 0x0FFFFFFF;
    tileBuf2[3] &= 0x0FFFFFFF;
    tileBuf2[4] &= 0x0FFFFFFF;
    tileBuf2[5] &= 0x0FFFFFFF;
    tileBuf2[6] &= 0x0FFFFFFF;
    tileBuf2[7] &= 0x0FFFFFFF;

    tileBuf2 = (int *)tileBuffer + 0x180;
    tileBuf2[0] &= 0x0FFFFFFF;
    tileBuf2[1] &= 0x0FFFFFFF;
    tileBuf2[2] &= 0x0FFFFFFF;
    tileBuf2[3] &= 0x0FFFFFFF;
    tileBuf2[4] &= 0x0FFFFFFF;
    tileBuf2[5] &= 0x0FFFFFFF;
    tileBuf2[6] &= 0x0FFFFFFF;
    tileBuf2[7] &= 0x0FFFFFFF;

    for (i = 0; i < 5; i++)
    {
        DmaCopy16(3, &tileBuffer[128 * i], &arg0[i * 256], 128);
        i++;i--; // fakematch
        DmaCopy16(3, &tileBuffer[128 * i + 0x400], &arg0[32 * ((i * 8) + 4)], 128);
    }
}

void sub_80F42C4(u8 *arg0)
{
    u16 i, tileOffset;
    u8 spriteId;
    struct SpriteSheet spriteSheet = {
        .data = gPokenavStructPtr->spriteGfxBuffers[0],
        .size = 0x500,
        .tag = 0x1A,
    };

    sub_80F4194(gPokenavStructPtr->spriteGfxBuffers[0], arg0);
    LoadSpriteSheet(&spriteSheet);
    LoadSpritePalette(&gUnknown_083E4868);

    tileOffset = 0;
    for (i = 0; i < 5; i++)
    {
        spriteId = CreateSprite(&gSpriteTemplate_83E4878, i * 32 + 113, 16, 0);
        if (spriteId != MAX_SPRITES)
        {
            gSprites[spriteId].oam.tileNum += tileOffset;
            gPokenavStructPtr->unkCED4[i] = &gSprites[spriteId];
        }
        else
        {
            gPokenavStructPtr->unkCED4[i] = NULL;
        }

        tileOffset += 8;
    }
}

void sub_80F4394(void)
{
    u16 i;

    for (i = 0; i < 5; i++)
    {
        if (gPokenavStructPtr->unkCED4[i])
            DestroySprite(gPokenavStructPtr->unkCED4[i]);
    }

    FreeSpriteTilesByTag(0x1A);
    FreeSpritePaletteByTag(0x12);
}

void sub_80F43D4(u8 *arg0)
{
    u16 tile;

    sub_80F4194(gPokenavStructPtr->spriteGfxBuffers[0], arg0);
    tile = GetSpriteTileStartByTag(0x1A);
    if (tile != 0xFFFF)
    DmaCopy32Defvars(3, gPokenavStructPtr->spriteGfxBuffers[0], (void *)(VRAM + 0x10000 + (tile * 32)), 0x500);
}

u8 *sub_80F4428(u8 *arg0, u16 arg1, u8 arg2)
{
    return sub_80F6514(arg0, arg1, arg2);
}

u8 *sub_80F443C(u8 *arg0, u16 arg1)
{
    return AlignInt1InMenuWindow(StringCopy(arg0, gOtherText_Number), arg1, 56, 1);
}

u8 *sub_80F445C(u8 *arg0, u16 arg1)
{
    u8 *buffer = AlignInt1InMenuWindow(arg0, arg1, 23, 1);
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
    buffer = AlignInt1InMenuWindow(buffer, gPokenavStructPtr->unk8774 + 1, 50, 1);
    return buffer;
}

u32 sub_80F44B0(u16 box, u16 monIndex, int monDataField, u8 *text)
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

void sub_80F45A0(s16 arg0, u8 arg1)
{
    u8 box;
    u8 var0 = gPokenavStructPtr->unk893c[arg0].unk3_14;
    if (var0)
    {
        sub_80F4428(gPokenavStructPtr->unk8829[arg1], arg0, 0);
        box = gPokenavStructPtr->unk893c[arg0].unk1;
        if (box == 14)
            AlignStringInMenuWindow(gPokenavStructPtr->unk88E9[arg1], gOtherText_InParty, 64, 0);
        else
            AlignStringInMenuWindow(gPokenavStructPtr->unk88E9[arg1], gPokemonStorage.boxNames[box], 64, 0);

        gPokenavStructPtr->unk8937[arg1] = 1;
    }
    else
    {
        AlignStringInMenuWindow(gPokenavStructPtr->unk8829[arg1], gEmptyString_81E72B0, 104, 0);
        AlignStringInMenuWindow(gPokenavStructPtr->unk88E9[arg1], gEmptyString_81E72B0, 64, 0);
        gPokenavStructPtr->unk8937[arg1] = var0;
    }
}

void sub_80F468C(s16 arg0, u8 arg1)
{
    u16 i;
    u16 box;
    u16 monIndex;

    if (gPokenavStructPtr->unk893c[arg0].unk3_14)
    {
        box = gPokenavStructPtr->unk893c[arg0].unk1;
        monIndex = gPokenavStructPtr->unk893c[arg0].partyIdx;
        gPokenavStructPtr->unk8ff0[arg1][0] = sub_80F44B0(box, monIndex, MON_DATA_COOL, NULL);
        gPokenavStructPtr->unk8ff0[arg1][1] = sub_80F44B0(box, monIndex, MON_DATA_TOUGH, NULL);
        gPokenavStructPtr->unk8ff0[arg1][2] = sub_80F44B0(box, monIndex, MON_DATA_SMART, NULL);
        gPokenavStructPtr->unk8ff0[arg1][3] = sub_80F44B0(box, monIndex, MON_DATA_CUTE, NULL);
        gPokenavStructPtr->unk8ff0[arg1][4] = sub_80F44B0(box, monIndex, MON_DATA_BEAUTY, NULL);

        gPokenavStructPtr->unk8931[arg1] = sub_80F44B0(box, monIndex, MON_DATA_SHEEN, NULL) != 255
                                           ? sub_80F44B0(box, monIndex, MON_DATA_SHEEN, NULL) / 29
                                           : 9;

        gPokenavStructPtr->conditionMonMarkings[arg1] = sub_80F44B0(box, monIndex, MON_DATA_MARKINGS, NULL);
        sub_80F55AC(gPokenavStructPtr->unk8ff0[arg1], gPokenavStructPtr->unk9004[arg1]);
    }
    else
    {
        for (i = 0; i < 5; i++)
        {
            gPokenavStructPtr->unk8ff0[arg1][i] = 0;
            gPokenavStructPtr->unk9004[arg1][i].unk0 = 0x9B;
            gPokenavStructPtr->unk9004[arg1][i].unk2 = 0x5B;
        }
    }
}

void sub_80F4824(s16 arg0, u8 arg1)
{
    u16 species;
    u32 otId;
    u32 personality;
    u16 box;
    u16 monIndex;

    if (gPokenavStructPtr->unk893c[arg0].unk3_14)
    {
        box = gPokenavStructPtr->unk893c[arg0].unk1;
        monIndex = gPokenavStructPtr->unk893c[arg0].partyIdx;
        species = sub_80F44B0(box, monIndex, MON_DATA_SPECIES2, NULL);
        otId = sub_80F44B0(box, monIndex, MON_DATA_OT_ID, NULL);
        personality = sub_80F44B0(box, monIndex, MON_DATA_PERSONALITY, NULL);

        HandleLoadSpecialPokePic(
            &gMonFrontPicTable[species],
            gMonFrontPicCoords[species].coords,
            1,
            gPokenavStructPtr->unk131E4,
            gPokenavStructPtr->spriteGfxBuffers[arg1],
            species,
            personality);

        LZ77UnCompWram(GetMonSpritePalFromOtIdPersonality(species, otId, personality), gPokenavStructPtr->unk0[arg1]);
        gPokenavStructPtr->unkD1D6[arg1] = species;
    }
}

void sub_80F4900(s16 arg0, u8 arg1)
{
    sub_80F45A0(arg0, arg1);
    sub_80F468C(arg0, arg1);
    sub_80F4824(arg0, arg1);
}

void sub_80F492C(void)
{
    gPokenavStructPtr->unk8FE4 = 0;
}

void sub_80F4944(struct UnkUsePokeblockSub *arg0) // This looks like a sorting algorithm. Proposal: Make local variables min, max, and currPos
{
    u16 min, max, currPos;

    min = 0;
    max = gPokenavStructPtr->unk8FE4;
    currPos = min + (max - min)/ 2;
    while (max != currPos)
    {
        if (arg0->unk0 > gPokenavStructPtr->unk893c[currPos].unk0)
            max = currPos;
        else
            min = currPos + 1;

        currPos = min + (max - min) / 2;
    }

    for ( max = gPokenavStructPtr->unk8FE4; max > currPos; max--)
    {
        gPokenavStructPtr->unk893c[max] = gPokenavStructPtr->unk893c[max - 1];
    }

    gPokenavStructPtr->unk893c[currPos] = *arg0;
    gPokenavStructPtr->unk8FE4++;
}

void sub_80F49F4(void)
{
    u16 i;

    gPokenavStructPtr->unk893c[0].unk2_5 = 1;
    for (i = 1; i < gPokenavStructPtr->unk8FE4; i++)
    {
        if (gPokenavStructPtr->unk893c[i].unk0 == gPokenavStructPtr->unk893c[i - 1].unk0)
            gPokenavStructPtr->unk893c[i].unk2_5 = gPokenavStructPtr->unk893c[i - 1].unk2_5;
        else
            gPokenavStructPtr->unk893c[i].unk2_5 = i + 1;
    }

    gPokenavStructPtr->listCursorRow = 0;
    gPokenavStructPtr->unk8770 = 0;
    gPokenavStructPtr->listSelectedIndex = 0;
    gPokenavStructPtr->unk8772 = gPokenavStructPtr->unk8FE4 < 9 ? (gPokenavStructPtr->unk8FE4 - 1) : 7;
    gPokenavStructPtr->unk8774 = gPokenavStructPtr->unk8FE4 - 1;
    gPokenavStructPtr->hasListScrollArrows = gPokenavStructPtr->unk8774 > 7;
}

void sub_80F4B20(void)
{
    s16 var0;
    s16 var1;

    sub_80F4900(gPokenavStructPtr->unk87DC, 0);
    CreateOrUpdatePokenavPortraitSprite(0);
    if (gPokenavStructPtr->unk87DA == 1)
    {
        gPokenavStructPtr->unk8fe9 = 0;
        gPokenavStructPtr->unk8FEA = 0;
        gPokenavStructPtr->unk8FEB = 0;
    }
    else
    {
        gPokenavStructPtr->unk8fe9 = 0;
        gPokenavStructPtr->unk8FEA = 1;
        gPokenavStructPtr->unk8FEB = 2;

        var0 = gPokenavStructPtr->unk87DC + 1;
        if (var0 >= gPokenavStructPtr->unk87DA)
            var0 = 0;

        var1 = gPokenavStructPtr->unk87DC - 1;
        if (var1 < 0)
            var1 = gPokenavStructPtr->unk87DA - 1;

        sub_80F4900(var0, 1);
        sub_80F4900(var1, 2);
    }
}

void sub_80F4BD0(void)
{
    u16 i, j;

    for (i = 0, j = 0; i < gPokenavStructPtr->unk8828; i++)
    {
        if (!GetMonData(&gPlayerParty[i], MON_DATA_IS_EGG))
        {
            gPokenavStructPtr->unk893c[j].unk1 = 14;
            gPokenavStructPtr->unk893c[j].partyIdx = i;
            gPokenavStructPtr->unk893c[j].unk2_5 = j + 1;
            gPokenavStructPtr->unk893c[j].unk3_14 = 1;
            j++;
        }
    }

    gPokenavStructPtr->unk893c[j].unk1 = 0;
    gPokenavStructPtr->unk893c[j].partyIdx = 0;
    gPokenavStructPtr->unk893c[j].unk2_5 = 0;
    gPokenavStructPtr->unk893c[j].unk3_14 = 0;
    gPokenavStructPtr->unk87DC = 0;
    gPokenavStructPtr->unk87DA = j + 1;
    sub_80F4B20();
    gPokenavStructPtr->unk87CB = 1;
}

void sub_80F4CF0(void)
{
    gPokenavStructPtr->unk87DC = gPokenavStructPtr->listSelectedIndex;
    sub_80F4B20();

    if (gPokenavStructPtr->unk8774 == 0)
        gPokenavStructPtr->unk87CB = 0;
    else
        gPokenavStructPtr->unk87CB = 1;
}

void sub_80F4D44(void)
{
    gPokenavStructPtr->unk8FE6 = 0;
    gPokenavStructPtr->unk8FE7 = 0;
    sub_80F492C();

    if (!gPokenavStructPtr->unk6DAC)
        while (sub_80F4D88());
}

bool8 sub_80F4D88(void)
{
    u16 i;
    register int mask asm("r3"); // FIXME
    int nextValue;
    struct UnkUsePokeblockSub var0;

    switch (gPokenavStructPtr->unk8FE6)
    {
    default:
        var0.unk3_14 = 1;
        for (i = 0; i < 15; i++)
        {

            if (GetBoxMonData(&gPokemonStorage.boxes[gPokenavStructPtr->unk8FE6][gPokenavStructPtr->unk8FE7], MON_DATA_SPECIES)
                && !GetBoxMonData(&gPokemonStorage.boxes[gPokenavStructPtr->unk8FE6][gPokenavStructPtr->unk8FE7], MON_DATA_IS_EGG))
            {
                var0.unk1 = gPokenavStructPtr->unk8FE6;
                var0.partyIdx = gPokenavStructPtr->unk8FE7;
                var0.unk0 = GetBoxMonData(
                    &gPokemonStorage.boxes[gPokenavStructPtr->unk8FE6][gPokenavStructPtr->unk8FE7],
                    gPokenavStructPtr->unk87D8);
                sub_80F4944(&var0);
            }

            gPokenavStructPtr->unk8FE7++;
            mask = 0xFF;
            if (gPokenavStructPtr->unk8FE7 == 30)
            {
                gPokenavStructPtr->unk8FE7 = 0;
                nextValue = gPokenavStructPtr->unk8FE6 + 1;
                gPokenavStructPtr->unk8FE6 = nextValue;
                if ((nextValue & mask) == 14)
                    break;
            }
        }
        break;
    case 14:
        var0.unk3_14 = 1;
        var0.unk1 = 14;
        for (i = 0; i < gPokenavStructPtr->unk8828; i++)
        {
            if (!GetMonData(&gPlayerParty[i], MON_DATA_IS_EGG))
            {
                var0.partyIdx = i;
                var0.unk0 = GetMonData(&gPlayerParty[i], gPokenavStructPtr->unk87D8);
                sub_80F4944(&var0);
            }
        }

        sub_80F49F4();
        gPokenavStructPtr->unk87DA = gPokenavStructPtr->unk8FE4;
        gPokenavStructPtr->unk8FE6++;
        break;
    case 15:
        return FALSE;
    }

    return TRUE;
}

void sub_80F4F78(void)
{
    sub_80F53EC(gPokenavStructPtr->unk9004[3], gPokenavStructPtr->unk9004[gPokenavStructPtr->unk8fe9]);
    sub_80F5504();
}

bool8 sub_80F4FB4(void)
{
    bool8 var0 = sub_80F5504();
    bool8 var1 = SlidePokenavMonInfoHeaderIn();
    return var0 || var1;
}

void sub_80F4FDC(void)
{
    if (gPokenavStructPtr->isConditionGraphSearchMode || gPokenavStructPtr->unk87DC != gPokenavStructPtr->unk8828)
        sub_80F53EC(gPokenavStructPtr->unk9004[gPokenavStructPtr->unk8fe9], gPokenavStructPtr->unk9004[3]);
}

bool8 sub_80F5038(void)
{
    bool8 var0 = sub_80F5504();
    bool8 var1 = SlidePokenavMonInfoHeaderOut();
    return var0 || var1;
}

void sub_80F5060(u8 arg0)
{
    u16 var0;
    u8 var1;
    u8 var2;

    if (arg0)
        var0 = gPokenavStructPtr->unk8FEB;
    else
        var0 = gPokenavStructPtr->unk8FEA;

    sub_80F53EC(gPokenavStructPtr->unk9004[gPokenavStructPtr->unk8fe9], gPokenavStructPtr->unk9004[var0]);
    var1 = gPokenavStructPtr->unk893c[gPokenavStructPtr->unk87DC].unk3_14;
    if (arg0)
    {
        gPokenavStructPtr->unk8FEB = gPokenavStructPtr->unk8FEA;
        gPokenavStructPtr->unk8FEA = gPokenavStructPtr->unk8fe9;
        gPokenavStructPtr->unk8fe9 = var0;
        gPokenavStructPtr->unk8FEC = gPokenavStructPtr->unk8FEB;

        gPokenavStructPtr->unk87DC = gPokenavStructPtr->unk87DC
                                     ? gPokenavStructPtr->unk87DC - 1
                                     : gPokenavStructPtr->unk87DA - 1;
        gPokenavStructPtr->unk8FEE = gPokenavStructPtr->unk87DC
                                     ? gPokenavStructPtr->unk87DC - 1
                                     : gPokenavStructPtr->unk87DA - 1;
    }
    else
    {
        gPokenavStructPtr->unk8FEA = gPokenavStructPtr->unk8FEB;
        gPokenavStructPtr->unk8FEB = gPokenavStructPtr->unk8fe9;
        gPokenavStructPtr->unk8fe9 = var0;
        gPokenavStructPtr->unk8FEC = gPokenavStructPtr->unk8FEA;

        gPokenavStructPtr->unk87DC = (gPokenavStructPtr->unk87DC < gPokenavStructPtr->unk87DA - 1)
                                     ? gPokenavStructPtr->unk87DC + 1
                                     : 0;
        gPokenavStructPtr->unk8FEE = (gPokenavStructPtr->unk87DC < gPokenavStructPtr->unk87DA - 1)
                                     ? gPokenavStructPtr->unk87DC + 1
                                     : 0;
    }

    var2 = gPokenavStructPtr->unk893c[gPokenavStructPtr->unk87DC].unk3_14;
    if (!var1)
        gPokenavStructPtr->unk87E0 = sub_80F5264;
    else if (!var2)
        gPokenavStructPtr->unk87E0 = sub_80F52F8;
    else
        gPokenavStructPtr->unk87E0 = sub_80F5364;

    gPokenavStructPtr->unk87DE = 0;
}

bool8 gpu_sync_bg_show(void)
{
    return gPokenavStructPtr->unk87E0();
}

bool8 sub_80F5264(void)
{
    switch (gPokenavStructPtr->unk87DE)
    {
    case 0:
        CreateOrUpdatePokenavPortraitSprite(gPokenavStructPtr->unk8fe9);
        PrintConditionGraphMonInfo(gPokenavStructPtr->unk8fe9);
        gPokenavStructPtr->unk87DE++;
        // fall through
    case 1:
        if (!sub_80F4FB4())
        {
            sub_80F4900(gPokenavStructPtr->unk8FEE, gPokenavStructPtr->unk8FEC);
            gPokenavStructPtr->unk87DE++;
        }
        break;
    case 2:
        return FALSE;
    }

    return TRUE;
}

bool8 sub_80F52F8(void)
{
    switch (gPokenavStructPtr->unk87DE)
    {
    case 0:
        if (!sub_80F5038())
        {
            PrintConditionGraphMonInfo(gPokenavStructPtr->unk8fe9);
            sub_80F4900(gPokenavStructPtr->unk8FEE, gPokenavStructPtr->unk8FEC);
            gPokenavStructPtr->unk87DE++;
        }
        break;
    case 1:
        return FALSE;
    }

    return TRUE;
}

bool8 sub_80F5364(void)
{
    switch (gPokenavStructPtr->unk87DE)
    {
    case 0:
        sub_80F5504();
        if (!SlidePokenavMonInfoHeaderOut())
        {
            CreateOrUpdatePokenavPortraitSprite(gPokenavStructPtr->unk8fe9);
            PrintConditionGraphMonInfo(gPokenavStructPtr->unk8fe9);
            gPokenavStructPtr->unk87DE++;
        }
        break;
    case 1:
        if (!sub_80F4FB4())
            gPokenavStructPtr->unk87DE++;
        break;
    case 2:
        sub_80F4900(gPokenavStructPtr->unk8FEE, gPokenavStructPtr->unk8FEC);
        return FALSE;
    }

    return TRUE;
}

void sub_80F53EC(struct UnkPokenav11 *arg0, struct UnkPokenav11 *arg1)
{
    u16 i, j;
    int r5;
    int r6;

    for (i = 0; i < 5; i++)
    {
        r5 = arg0[i].unk0 << 8;
        r6 = ((arg1[i].unk0 - arg0[i].unk0) << 8) / 10;
        for (j = 0; j < 9; j++)
        {
            gPokenavStructPtr->unk9054[j][i].unk0 = (r5 >> 8) + ((r5 >> 7) & 1);
            r5 += r6;
        }

        gPokenavStructPtr->unk9054[j][i].unk0 = arg1[i].unk0;
        r5 = arg0[i].unk2 << 8;
        r6 = ((arg1[i].unk2 - arg0[i].unk2) << 8) / 10;
        for (j = 0; j < 9; j++)
        {
            gPokenavStructPtr->unk9054[j][i].unk2 = (r5 >> 8) + ((r5 >> 7) & 1);
            r5 += r6;
        }

        gPokenavStructPtr->unk9054[j][i].unk2 = arg1[i].unk2;
    }

    gPokenavStructPtr->unk9342 = 0;
}

bool8 sub_80F5504(void)
{
    if (gPokenavStructPtr->unk9342 < 10)
    {
        sub_80F556C(gPokenavStructPtr->unk9054[gPokenavStructPtr->unk9342++]);
        return gPokenavStructPtr->unk9342 != 10;
    }
    else
    {
        return FALSE;
    }
}

void sub_80F5550(struct UnkPokenav11 *arg0, struct UnkPokenav11 *arg1)
{
    sub_80F53EC(arg0, arg1);
}

bool8 sub_80F555C(void)
{
    return sub_80F5504();
}

void sub_80F556C(struct UnkPokenav11 *arg0)
{
    u16 i;

    for (i = 0; i < 5; i++)
        gPokenavStructPtr->unk911C[i] = arg0[i];

    gPokenavStructPtr->unk9344 = 1;
}

void sub_80F55AC(u8 *a0, struct UnkPokenav11 a1[])
{
    u16 i;
    u8 r2 = gUnknown_083E4890[*a0++];
    u8 r7;
    s8 r12;

    a1[0].unk0 = 0x9b;
    a1[0].unk2 = 0x5b - r2;

    r7 = 0x40;
    r12 = 0;
    for (i = 1; i < 5; i++)
    {
        r7 += 0x33;
        r12--;
        if (r12 < 0)
            r12 = 4;
        if (r12 == 2)
            r7++;
        r2 = gUnknown_083E4890[*a0++];
        a1[r12].unk0 = ((r2 * gSineTable[r7 + 0x40]) >> 8) + 0x9b;
        a1[r12].unk2 = 0x5b - ((r2 * gSineTable[r7]) >> 8);

        if (r12 <= 2 && (r2 != 0x20 || r12 != 2))
            a1[r12].unk0 = ((r2 * gSineTable[r7 + 0x40]) >> 8) + 0x9c;
    }
}

void sub_80F567C(u8 *a0, struct UnkPokenav11 a1[])
{
    sub_80F55AC(a0, a1);
}

void sub_80F5688(u16 * arg1, struct UnkPokenav11 * arg2, struct UnkPokenav11 * arg3, u8 arg4, u16 * arg5)
{
    u16 i, r8, r10, r0, var_30;
    u16 *ptr;
    s32 r4, var_2C;

    var_2C = 0;
    if (arg2->unk2 < arg3->unk2)
    {
        r10 = arg2->unk2;
        r0 = arg3->unk2;
        r4 = arg2->unk0 << 10;
        var_30 = arg3->unk0;
        r8 = r0 - r10;
        if (r8 != 0)
            var_2C = ((var_30 - arg2->unk0) << 10) / r8;
    }
    else
    {
        r0 = arg2->unk2;
        r10 = arg3->unk2;
        r4 = arg3->unk0 << 10;
        var_30 = arg2->unk0;
        r8 = r0 - r10;
        if (r8 != 0)
            var_2C = ((var_30 - arg3->unk0) << 10) / r8;
    }

    r8++;
    if (arg5 == NULL)
    {
        arg1 += (r10 - 56) * 2;
        for (i = 0; i < r8; i++)
        {
            arg1[arg4] = (r4 >> 10) + ((r4 >> 9) & 1) + arg4;
            r4 += var_2C;
            arg1 += 2;
        }

        ptr = arg1 - 2;
    }
    else if (var_2C > 0)
    {
        arg5 += (r10 - 56) * 2;
        // Less readable than the other loops, but it has to be written this way to match.
        for (i = 0; i < r8; arg5[arg4] = (r4 >> 10) + ((r4 >> 9) & 1) + arg4, r4 += var_2C, arg5 += 2, i++)
        {
            if (r4 >= (155 << 10))
                break;
        }

        gPokenavStructPtr->unk9340 = r10 + i;
        arg1 += (gPokenavStructPtr->unk9340 - 56) * 2;
        for (; i < r8; i++)
        {
            arg1[arg4] = (r4 >> 10) + ((r4 >> 9) & 1) + arg4;
            r4 += var_2C;
            arg1 += 2;
        }

        ptr = arg1 - 2;
    }
    else if (var_2C < 0)
    {
        arg1 += (r10 - 56) * 2;
        for (i = 0; i < r8; i++)
        {
            arg1[arg4] = (r4 >> 10) + ((r4 >> 9) & 1) + arg4;
            if (r4 < (155 << 10))
            {
                arg1[arg4] = 155;
                break;
            }
            r4 += var_2C;
            arg1 += 2;
        }

        gPokenavStructPtr->unk9340 = r10 + i;
        arg5 += (gPokenavStructPtr->unk9340 - 56) * 2;
        for (; i < r8; i++)
        {
            arg5[arg4] = (r4 >> 10) + ((r4 >> 9) & 1) + arg4;
            r4 += var_2C;
            arg5 += 2;
        }

        ptr = arg5 - 2;
    }
    else
    {
        gPokenavStructPtr->unk9340 = r10;
        arg1 += (r10 - 56) * 2;
        arg5 += (r10 - 56) * 2;
        arg1[1] = arg2->unk0 + 1;
        arg5[0] = arg3->unk0;
        arg5[1] = 155;
        return;
    }

    ptr[arg4] = arg4 + var_30;
}

void sub_80F58DC(struct UnkPokenav11 * a0)
{
    u16 i, r6, varMax;

    if (a0[0].unk2 < a0[1].unk2)
    {
        r6 = a0[0].unk2;
        sub_80F5688((u16 *)gPokenavStructPtr->unk9130, &a0[0], &a0[1], 1, NULL);
    }
    else
    {
        r6 = a0[1].unk2;
        sub_80F5688((u16 *)gPokenavStructPtr->unk9130, &a0[1], &a0[0], 0, NULL);
    }
    sub_80F5688((u16 *)gPokenavStructPtr->unk9130, &a0[1], &a0[2], 1, NULL);

    i = a0[2].unk2 <= a0[3].unk2;
    sub_80F5688((u16 *)gPokenavStructPtr->unk9130, &a0[2], &a0[3], i, (u16 *)gPokenavStructPtr->unk9238);
    for (i = 56; i < r6; i++)
    {
        gPokenavStructPtr->unk9130[i - 56][0] = 0;
        gPokenavStructPtr->unk9130[i - 56][1] = 0;
    }

    for (i = a0[0].unk2; i <= gPokenavStructPtr->unk9340; i++)
        gPokenavStructPtr->unk9130[i - 56][0] = 155;

    varMax = max(gPokenavStructPtr->unk9340, a0[2].unk2);
    for (i = varMax + 1; i < 122; i++)
    {
        gPokenavStructPtr->unk9130[i - 56][0] = 0;
        gPokenavStructPtr->unk9130[i - 56][1] = 0;
    }

//    for (i = 56; i < 122; i++)
//    {
//        if (gPokenavStructPtr->unk9130[i - 56][0] == 0 && gPokenavStructPtr->unk9130[i - 56][1] != 0)
//            gPokenavStructPtr->unk9130[i - 56][0] = 155;
//    }
}

void sub_80F5A1C(struct UnkPokenav11 *arg0)
{
    u16 i, r6, varMax;

    if (arg0[0].unk2 < arg0[4].unk2)
    {
        r6 = arg0[0].unk2;
        sub_80F5688((u16 *)gPokenavStructPtr->unk9238, &arg0[0], &arg0[4], 0, NULL);
    }
    else
    {
        r6 = arg0[4].unk2;
        sub_80F5688((u16 *)gPokenavStructPtr->unk9238, &arg0[4], &arg0[0], 1, NULL);
    }

    sub_80F5688((u16 *)gPokenavStructPtr->unk9238, &arg0[4], &arg0[3], 0, NULL);

    for (i = 56; i < r6; i++)
    {
        gPokenavStructPtr->unk9238[i - 56][0] = 0;
        gPokenavStructPtr->unk9238[i - 56][1] = 0;
    }

    for (i = arg0[0].unk2; i <= gPokenavStructPtr->unk9340; i++)
        gPokenavStructPtr->unk9238[i - 56][1] = 155;

    varMax = max(gPokenavStructPtr->unk9340, arg0[3].unk2 + 1);
    for (i = varMax; i < 122; i++)
    {
        gPokenavStructPtr->unk9238[i - 56][0] = 0;
        gPokenavStructPtr->unk9238[i - 56][1] = 0;
    }

//    for (i = 0; i < 66; i++)
//    {
//        if (gPokenavStructPtr->unk9238[i][0] >= gPokenavStructPtr->unk9238[i][1])
//        {
//            gPokenavStructPtr->unk9238[i][1] = 0;
//            gPokenavStructPtr->unk9238[i][0] = 0;
//        }
//    }
}

void sub_80F5B38(void)
{
    gPokenavStructPtr->unk9345 = 0;
}

extern const struct ScanlineEffectParams gUnknown_083E4990;

bool8 sub_80F5B50(void)
{
    s32 i;
    struct ScanlineEffectParams params;

    switch (gPokenavStructPtr->unk9345)
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
        gPokenavStructPtr->unk9345++;
        return TRUE;
    case 1:
        params = gUnknown_083E4990;
        ScanlineEffect_SetParams(params);
        gPokenavStructPtr->unk9345++;
        break;
    }
    return FALSE;
}

void sub_80F5BDC(void)
{
    gScanlineEffect.state = 3;
    ScanlineEffect_InitHBlankDmaTransfer();
}

void sub_80F5BF0(void)
{
    u16 i;

    if (gPokenavStructPtr->unk9344)
    {
        sub_80F58DC(gPokenavStructPtr->unk911C);
        sub_80F5A1C(gPokenavStructPtr->unk911C);
        for (i = 0; i < 66; i++)
        {
            gScanlineEffectRegBuffers[1][(i + 55) * 2 + 0] = gScanlineEffectRegBuffers[0][(i + 55) * 2 + 0] = (gPokenavStructPtr->unk9130[i][0] << 8) | (gPokenavStructPtr->unk9130[i][1]);
            gScanlineEffectRegBuffers[1][(i + 55) * 2 + 1] = gScanlineEffectRegBuffers[0][(i + 55) * 2 + 1] = (gPokenavStructPtr->unk9238[i][0] << 8) | (gPokenavStructPtr->unk9238[i][1]);
        }
        gPokenavStructPtr->unk9344 = 0;
    }
}

void sub_80F5CDC(u8 a0)
{
    u16 i, r5;

    if (gPokenavStructPtr->unk9344)
    {
        sub_80F58DC(gPokenavStructPtr->unk911C);
        sub_80F5A1C(gPokenavStructPtr->unk911C);
        r5 = 2 * (55 - a0);
        for (i = 0; i < 66; i ++)
        {
            gScanlineEffectRegBuffers[1][r5 + 0] = gScanlineEffectRegBuffers[0][r5 + 0] = (gPokenavStructPtr->unk9130[i][0] << 8) | (gPokenavStructPtr->unk9130[i][1]);
            gScanlineEffectRegBuffers[1][r5 + 1] = gScanlineEffectRegBuffers[0][r5 + 1] = (gPokenavStructPtr->unk9238[i][0] << 8) | (gPokenavStructPtr->unk9238[i][1]);
            r5 += 2;
        }
        gPokenavStructPtr->unk9344 = 0;
    }
}

u8 sub_80F5DD4(void)
{
    if (JOY_REPT(DPAD_UP))
    {
        return sub_80F5E20();
    }
    else if (JOY_REPT(DPAD_DOWN))
    {
        return sub_80F5EE4();
    }
    else if (JOY_REPT(DPAD_LEFT))
    {
        return sub_80F5FB4();
    }
    else if (JOY_REPT(DPAD_RIGHT))
    {
        return sub_80F6010();
    }
    else
    {
        return 0;
    }
}

u8 sub_80F5E20(void)
{
    if (gPokenavStructPtr->listSelectedIndex == 0)
    {
        return 0;
    }
    if (gPokenavStructPtr->hasListScrollArrows != 0 && gPokenavStructPtr->listCursorRow == 0)
    {
        BeginPokenavListScroll(-1);
        sub_80F6074(-1);
        return 2;
    }
    gPokenavStructPtr->listCursorRow--;
    if (gPokenavStructPtr->hasListScrollArrows == 0 && gPokenavStructPtr->listCursorRow < 0)
    {
        gPokenavStructPtr->listCursorRow = gPokenavStructPtr->unk8772;
    }
    gPokenavStructPtr->listSelectedIndex = gPokenavStructPtr->unk8770 + gPokenavStructPtr->listCursorRow;
    if (gPokenavStructPtr->listSelectedIndex > gPokenavStructPtr->unk8774)
    {
        gPokenavStructPtr->listSelectedIndex -= gPokenavStructPtr->unk8774 + 1;
    }
    return 1;
}

u8 sub_80F5EE4(void)
{
    if (gPokenavStructPtr->listSelectedIndex == gPokenavStructPtr->unk8774)
    {
        return 0;
    }
    if (gPokenavStructPtr->hasListScrollArrows != 0 && gPokenavStructPtr->listCursorRow == 7)
    {
        BeginPokenavListScroll(1);
        sub_80F6074(1);
        return 2;
    }
    gPokenavStructPtr->listCursorRow++;
    if (gPokenavStructPtr->hasListScrollArrows == 0 && gPokenavStructPtr->listCursorRow > gPokenavStructPtr->unk8772)
    {
        gPokenavStructPtr->listCursorRow = 0;
    }
    gPokenavStructPtr->listSelectedIndex = gPokenavStructPtr->unk8770 + gPokenavStructPtr->listCursorRow;
    if (gPokenavStructPtr->listSelectedIndex > gPokenavStructPtr->unk8774)
    {
        gPokenavStructPtr->listSelectedIndex -= gPokenavStructPtr->unk8774 + 1;
    }
    return 1;
}

u8 sub_80F5FB4(void)
{
    s16 r4;
    if (gPokenavStructPtr->unk8770 == 0 || gPokenavStructPtr->hasListScrollArrows == 0)
    {
        return 0;
    }
    if (gPokenavStructPtr->unk8770 < 8)
    {
        r4 = -gPokenavStructPtr->unk8770;
    }
    else
    {
        r4 = -8;
    }
    BeginPokenavListScroll(r4);
    sub_80F6074(r4);
    return 2;
}

u8 sub_80F6010(void)
{
    s16 r4;
    if (gPokenavStructPtr->unk8772 == gPokenavStructPtr->unk8774 || gPokenavStructPtr->hasListScrollArrows == 0)
    {
        return 0;
    }
    r4 = gPokenavStructPtr->unk8774 - gPokenavStructPtr->unk8772;
    if (r4 > 8)
    {
        r4 = 8;
    }
    BeginPokenavListScroll(r4);
    sub_80F6074(r4);
    return 2;
}

void sub_80F6074(s16 a0)
{
    gPokenavStructPtr->unk8770 += a0;
    if (gPokenavStructPtr->unk8770 > gPokenavStructPtr->unk8774)
    {
        gPokenavStructPtr->unk8770 -= gPokenavStructPtr->unk8774 + 1;
    }
    if (gPokenavStructPtr->unk8770 < 0)
    {
        gPokenavStructPtr->unk8770 += gPokenavStructPtr->unk8774 + 1;
    }
    gPokenavStructPtr->unk8772 += a0;
    if (gPokenavStructPtr->unk8772 > gPokenavStructPtr->unk8774)
    {
        gPokenavStructPtr->unk8772 -= gPokenavStructPtr->unk8774 + 1;
    }
    if (gPokenavStructPtr->unk8772 < 0)
    {
        gPokenavStructPtr->unk8772 += gPokenavStructPtr->unk8774 + 1;
    }
    gPokenavStructPtr->listSelectedIndex += a0;
    if (gPokenavStructPtr->listSelectedIndex > gPokenavStructPtr->unk8774)
    {
        gPokenavStructPtr->listSelectedIndex -= gPokenavStructPtr->unk8774 + 1;
    }
    if (gPokenavStructPtr->listSelectedIndex < 0)
    {
        gPokenavStructPtr->listSelectedIndex += gPokenavStructPtr->unk8774 + 1;
    }
}

void sub_80F6134(void)
{
    if (gPokenavStructPtr->hasListScrollArrows != 0)
    {
        if (gPokenavStructPtr->unk87DC < gPokenavStructPtr->unk8774 - 7)
        {
            gPokenavStructPtr->listCursorRow = 0;
            gPokenavStructPtr->unk8770 = gPokenavStructPtr->unk87DC;
            gPokenavStructPtr->listSelectedIndex = gPokenavStructPtr->unk87DC;
            gPokenavStructPtr->unk8772 = gPokenavStructPtr->unk8770 + 7;
            if (gPokenavStructPtr->unk8772 > gPokenavStructPtr->unk8774)
            {
                gPokenavStructPtr->unk8772 -= gPokenavStructPtr->unk8774 + 1;
            }
        }
        else
        {
            gPokenavStructPtr->unk8770 = gPokenavStructPtr->unk8774 - 7;
            gPokenavStructPtr->unk8772 = gPokenavStructPtr->unk8774;
            gPokenavStructPtr->listSelectedIndex = gPokenavStructPtr->unk87DC;
            gPokenavStructPtr->listCursorRow = 7 - (gPokenavStructPtr->unk8774 -  gPokenavStructPtr->listSelectedIndex);
        }
    }
    else
    {
        gPokenavStructPtr->listCursorRow = gPokenavStructPtr->unk87DC;
        gPokenavStructPtr->listSelectedIndex = gPokenavStructPtr->unk87DC;
    }
}

void sub_80F6208(void)
{
    gPokenavStructPtr->unk8FE6 = 0;
    gPokenavStructPtr->unk8FE7 = 0;
    gPokenavStructPtr->unk8FE8 = 0xFF;
    if (gPokenavStructPtr->unk6DAC == 0)
    {
        while (sub_80F6250())
            ;
    }
}

bool8 sub_80F6250(void)
{
    u16 i;

    if (gPokenavStructPtr->unk8FE8 != -1)
        return FALSE;

    switch (gPokenavStructPtr->unk8FE6)
    {
    default:
        for (i = 0; i < 10; i++)
        {
            if (GetBoxMonData(&gPokemonStorage.boxes[gPokenavStructPtr->unk8FE6 + 0][gPokenavStructPtr->unk8FE7], MON_DATA_RIBBON_COUNT) != 0)
            {
                gPokenavStructPtr->unk8FE8 = 1;
                return FALSE;
            }
            if (GetBoxMonData(&gPokemonStorage.boxes[gPokenavStructPtr->unk8FE6 + 7][gPokenavStructPtr->unk8FE7], MON_DATA_RIBBON_COUNT) != 0)
            {
                gPokenavStructPtr->unk8FE8 = 1;
                return FALSE;
            }
            if (++gPokenavStructPtr->unk8FE7 >= 30)
            {
                gPokenavStructPtr->unk8FE7 = 0;
                if (++gPokenavStructPtr->unk8FE6 >= 7)
                {
                    break;
                }
            }
        }
        break;
    case 8:
        return FALSE;
    case 7:
        gPokenavStructPtr->unk8FE8 = 0;
        for (i = 0; i < 6; i++)
        {
            if (GetMonData(&gPlayerParty[i], MON_DATA_RIBBON_COUNT) != 0)
            {
                gPokenavStructPtr->unk8FE8 = 1;
                break;
            }
        }
        gPokenavStructPtr->unk8FE6++;
        return FALSE;
    }
    return TRUE;
}

void sub_80F638C(void)
{
    gPokenavStructPtr->unk8FE6 = 0;
    gPokenavStructPtr->unk8FE7 = 0;
    sub_80F492C();
    if (gPokenavStructPtr->unk6DAC == 0)
    {
        while (sub_80F63D0())
            ;
    }
}

bool8 sub_80F63D0(void)
{
    struct UnkUsePokeblockSub sp0;
    u8 ribbons;
    u16 i;

    switch (gPokenavStructPtr->unk8FE6)
    {
    default:
        sp0.unk3_14 = 1;
        for (i = 0; i < 15; i++)
        {
            ribbons = GetBoxMonData(&gPokemonStorage.boxes[gPokenavStructPtr->unk8FE6][gPokenavStructPtr->unk8FE7 + 0], MON_DATA_RIBBON_COUNT);
            if (ribbons != 0)
            {
                sp0.unk1 = gPokenavStructPtr->unk8FE6;
                sp0.partyIdx = gPokenavStructPtr->unk8FE7;
                sp0.unk0 = ribbons;
                sub_80F4944(&sp0);
            }
            if (++gPokenavStructPtr->unk8FE7 == 30)
            {
                gPokenavStructPtr->unk8FE7 = 0;
                if (++gPokenavStructPtr->unk8FE6 == 14)
                    break;
            }
        }
        break;
    case 15:
        return FALSE;
    case 14:
        sp0.unk3_14 = 1;
        sp0.unk1 = 14;
        for (i = 0; i < 6; i++)
        {
            ribbons = GetMonData(&gPlayerParty[i], MON_DATA_RIBBON_COUNT);
            if (ribbons != 0)
            {
                sp0.partyIdx = i;
                sp0.unk0 = ribbons;
                sub_80F4944(&sp0);
            }
        }
        sub_80F49F4();
        gPokenavStructPtr->unk8FE6++;
        return FALSE;
    }
    return TRUE;
}

u8 * sub_80F6514(u8 * r10, u16 sp0, u8 sp4)
{
    u8 * dest = r10;
    u8 box = gPokenavStructPtr->unk893c[sp0].unk1;
    u8 monNo = gPokenavStructPtr->unk893c[sp0].partyIdx;
    u16 species;
    u16 level;
    u8 gender;

    if (!sub_80F44B0(box, monNo, MON_DATA_IS_EGG, NULL))
    {
        sub_80F44B0(box, monNo, MON_DATA_NICKNAME, dest);
        StringGet_Nickname(dest);
        species = sub_80F44B0(box, monNo, MON_DATA_SPECIES, NULL);
        if (box == 14)
        {
            level = GetMonData(&gPlayerParty[monNo], MON_DATA_LEVEL);
            gender = GetMonGender(&gPlayerParty[monNo]);
        }
        else
        {
            level = GetLevelFromBoxMonExp(&gPokemonStorage.boxes[box][monNo]);
            gender = GetGenderFromSpeciesAndPersonality(species, sub_80F44B0(box, monNo, MON_DATA_PERSONALITY, NULL));
        }
        if (ShouldHideGenderIcon(species, r10))
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

        dest = ConvertIntToDecimalString(dest, level);
        if (sp4 == 1)
        {
            dest = AlignInt1InMenuWindow(dest, gPokenavStructPtr->unk893c[sp0].unk0, 0x80, 0x01);
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

extern const u16 gUnknown_083E499C[];

void sub_80F66E0(void)
{
    u8 r9;
    u32 r7;
    u16 i, j;
    u8 r2;
    u8 r0;

    sub_80F6514(gPokenavStructPtr->unk8829[0], gPokenavStructPtr->listSelectedIndex, 0);
    sub_80F4824(gPokenavStructPtr->listSelectedIndex, 0);
    gPokenavStructPtr->unk87DC = gPokenavStructPtr->listSelectedIndex;
    gPokenavStructPtr->ribbonCount = 0;
    r9 = 0;
    r7 = sub_80F44B0(gPokenavStructPtr->unk893c[gPokenavStructPtr->listSelectedIndex].unk1, gPokenavStructPtr->unk893c[gPokenavStructPtr->listSelectedIndex].partyIdx, MON_DATA_RIBBONS, NULL);
    gPokenavStructPtr->giftRibbonCount = 0;
    for (i = 0; i < 17; i++)
    {
        switch (gUnknown_083E499C[i])
        {
        case MON_DATA_COOL_RIBBON:
        case MON_DATA_BEAUTY_RIBBON:
        case MON_DATA_CUTE_RIBBON:
        case MON_DATA_SMART_RIBBON:
        case MON_DATA_TOUGH_RIBBON:
            r2 = r7 & 7;
            r7 >>= 3;
            r0 = 4;
            break;
        default:
            r2 = r7 & 1;
            r7 >>= 1;
            r0 = 1;
            break;
        };
        for (j = 0; j < r2; j++)
            gPokenavStructPtr->ribbonIds[gPokenavStructPtr->ribbonCount++] = r9 + j;
        if (r2 && r9 > 24)
            gPokenavStructPtr->giftRibbonCount++;
        r9 += r0;
    }
    if (gPokenavStructPtr->ribbonCount != gPokenavStructPtr->giftRibbonCount)
    {
        gPokenavStructPtr->ribbonCursorPos = 0;
        gPokenavStructPtr->ribbonPageIndex = 0;
    }
    else
    {
        gPokenavStructPtr->ribbonCursorPos = 0;
        gPokenavStructPtr->ribbonPageIndex = 3;
    }
    r2 = gPokenavStructPtr->ribbonCount - gPokenavStructPtr->giftRibbonCount;
    for (i = 0; i < 3; i++)
    {
        if (r2 > 8)
        {
            gPokenavStructPtr->ribbonPageCounts[i] = 9;
            r2 -= 9;
        }
        else
        {
            gPokenavStructPtr->ribbonPageCounts[i] = r2;
            r2 = 0;
        }
    }
    gPokenavStructPtr->ribbonPageCounts[i] = gPokenavStructPtr->giftRibbonCount;
}

u8 sub_80F68E8(void)
{
    s8 r5 = gPokenavStructPtr->ribbonCursorPos;
    s8 r4 = gPokenavStructPtr->ribbonPageIndex;
    s8 r12 = 1;
    do
    {
        if (JOY_REPT(DPAD_UP) && r4 > 0)
        {
            while (r4 > 0)
            {
                r4--;
                if (gPokenavStructPtr->ribbonPageCounts[r4] != 0)
                    break;
            }
            if (gPokenavStructPtr->ribbonPageCounts[r4] != 0)
            {
                if (r5 >= gPokenavStructPtr->ribbonPageCounts[r4])
                    r5 = gPokenavStructPtr->ribbonPageCounts[r4] - 1;
                break;
            }
            r4 = gPokenavStructPtr->ribbonPageIndex;
        }
        if (JOY_REPT(DPAD_DOWN) && r4 < 3)
        {
            while (r4 < 3)
            {
                r4++;
                if (gPokenavStructPtr->ribbonPageCounts[r4] != 0)
                    break;
            }
            if (gPokenavStructPtr->ribbonPageCounts[r4] != 0)
            {
                if (r5 >= gPokenavStructPtr->ribbonPageCounts[r4])
                    r5 = gPokenavStructPtr->ribbonPageCounts[r4] - 1;
                break;
            }
            r4 = gPokenavStructPtr->ribbonPageIndex;
        }
        if (JOY_REPT(DPAD_LEFT))
        {
            if (r5 > 0)
            {
                r5--;
                break;
            }
        }
        if (JOY_REPT(DPAD_RIGHT))
        {
            if (r5 < gPokenavStructPtr->ribbonPageCounts[r4] - 1)
            {
                r5++;
                break;
            }
        }
        r12 = 0;
    } while (0);
    if (r12)
    {
        if (r5 != gPokenavStructPtr->ribbonCursorPos || r4 != gPokenavStructPtr->ribbonPageIndex)
        {
            gPokenavStructPtr->ribbonCursorPos = r5;
            gPokenavStructPtr->ribbonPageIndex = r4;
        }
        else
            r12 = 0;
    }
    return r12;
}

void sub_80F6A4C(s8 a0)
{
    gPokenavStructPtr->listSelectedIndex += a0;
    if (gPokenavStructPtr->listSelectedIndex < 0)
    {
        gPokenavStructPtr->listSelectedIndex = gPokenavStructPtr->unk8774;
    }
    if (gPokenavStructPtr->listSelectedIndex > gPokenavStructPtr->unk8774)
    {
        gPokenavStructPtr->listSelectedIndex = 0;
    }
    gPokenavStructPtr->unkBC94 = a0;
    gPokenavStructPtr->unk87DC = gPokenavStructPtr->listSelectedIndex;
    REG_WININ = 0x3F37;
    REG_WINOUT = 0x3F3F;
    REG_WIN0H = 0x58F0;
    REG_WIN0V = 0x2060;
    gPokenavStructPtr->unk87DE = 0;
}

bool8 sub_80F6AF0(void)
{
    switch (gPokenavStructPtr->unk87DE)
    {
    case 0:
        if (!SlidePokenavMonInfoHeaderOut())
        {
            gPokenavStructPtr->unk87DE++;
        }
        break;
    case 1:
        REG_DISPCNT |= DISPCNT_WIN0_ON;
        ClearRibbonsSummaryDescription();
        sub_80F66E0();
        gPokenavStructPtr->unk87DE++;
        break;
    case 2:
        DrawMonRibbonIcons();
        gPokenavStructPtr->unk87DE++;
        break;
    case 3:
        CopyRibbonsSummaryTilemapToVram();
        gPokenavStructPtr->unk87DE++;
        break;
    case 4:
        sub_80F4824(gPokenavStructPtr->listSelectedIndex, 0);
        gPokenavStructPtr->unk87DE++;
        break;
    case 5:
        CreateOrUpdatePokenavPortraitSprite(0);
        gPokenavStructPtr->unk87DE++;
        break;
    case 6:
        if (!SlidePokenavMonInfoHeaderIn())
        {
            PrintRibbonsSummaryMonInfo();
            REG_DISPCNT &= ~DISPCNT_WIN0_ON;
            gPokenavStructPtr->unk87DE++;
            return FALSE;
        }
        break;
    default:
        return FALSE;
    }

    return TRUE;
}
