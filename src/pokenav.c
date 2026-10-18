
// Includes
#include "global.h"
#include "load_save.h"
#include "ewram.h"
#include "main.h"
#include "palette.h"
#include "decompress.h"
#include "trig.h"
#include "data.h"
#include "scanline_effect.h"
#include "pokemon_storage_system.h"
#include "text.h"
#include "menu.h"
#include "landmark.h"
#include "strings.h"
#include "string_util.h"
#include "event_data.h"
#include "pokeblock.h"
#include "overworld.h"
#include "pokemon_summary_screen.h"
#include "link.h"
#include "sound.h"
#include "battle_setup.h"
#include "constants/songs.h"
#include "constants/flags.h"
#include "constants/game_stat.h"
#include "pokenav.h"
#include "graphics.h"
#include "constants/rgb.h"

// Static type declarations

// Static RAM declarations

EWRAM_DATA u8 gUnknown_020388B0[4] = {};
EWRAM_DATA u16 gUnknown_020388B4 = 0;

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
extern const u8 gConditionGraphData_Pal[];
extern const u8 gGraphData_Tilemap[];
extern const u8 gUnknown_083E01AC[];
extern const u8 *const gPokenavCityMaps[][2];
extern const u8 gRegionMapCityZoomTiles_Pal[];
extern const u8 gRegionMapCityZoomTiles_Gfx[];
extern const u16 gUnknown_083E003C[];
extern const u8 *const gUnknown_083E31B0[];
extern const u8 *const gUnknown_083E31CC[];
extern const u8 *const gUnknown_083E31D8[];
extern u8 *gUnknown_083DFEC8;
extern const u8 gPokenavBgDotsTiles[];
extern const u8 gUnknown_083E005C[];
extern const u8 gUnknown_083E007C[];
extern const u8 gPokenavDeviceBgTilemap[];
extern const u8 gPokenavDeviceBgTiles[];
extern const u8 gPokenavDeviceBgPal[];
extern const u8 gPokenavBgDotsPal[];
extern const u8 gPokenavBgDotsTilemap[];
extern const u8 gPokenavHoennMapMisc_Gfx[];
extern const u16 gPokenavHoennMap1_Pal[];
extern void (*const gUnknown_083E3270[])(u16, u16);
extern const u8 gUnknown_083E039C[];
extern const u8 gUnknown_083E03A0[];
extern const u8 gUnknown_083E01F4[];
extern const u8 *const gTrainerEyeDescriptions[];
extern const u8 gRibbonIconsSmall_Gfx[];
extern const u16 gPokenavRibbonsSummaryBg_Pal[];
extern const u16 gUnknown_083E03A8[];
extern const u16 gUnknown_083E3C60[][16];
extern const u16 gRibbonGfxData[][2];
extern const u8 *const gRibbonDescriptionPointers[][2];
extern const u8 *const gGiftRibbonDescriptionPointers[][2];
extern const u8 gUnknown_08E9FF58[];
extern const u8 gPokenavRibbonPokeView_Gfx[];
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
extern const struct SpriteTemplate gSpriteTemplate_83E4484;
extern const u8 gPokenavConditionMenuOptions_Gfx[];
extern const u8 gPokenavConditionMenuOptions2_Gfx[];
extern const u8 gPokenavTrainersEyesHeader_Gfx[];
extern const struct SpritePalette gUnknown_083E449C[];
extern const struct SpriteTemplate gSpriteTemplate_83E4530;
extern const struct SpriteTemplate gSpriteTemplate_83E4548;
extern const struct SpriteTemplate gSpriteTemplate_83E44E0;
extern const struct SpriteTemplate gSpriteTemplate_83E44F8;
extern const struct SpriteSheet gUnknown_083E4568;
extern const struct SpriteTemplate gSpriteTemplate_83E4570;
extern const struct SpritePalette gUnknown_083E4588;
extern const struct SpriteSheet gUnknown_083E4590[3];
extern const struct SpritePalette gUnknown_083E45A8;
extern const u16 gUnknown_08E9F988[];
extern const struct SpriteTemplate gSpriteTemplate_83E45B8;
extern const struct SpriteTemplate gSpriteTemplate_83E45F0;
extern const struct SpriteSheet gUnknown_083E4628[4];
extern const struct SpritePalette gUnknown_083E4648[3];
extern const struct SpriteTemplate gSpriteTemplate_83E4660;
extern const u16 gMonMarkings_Pal[];
extern const u8 gRibbonIconsBig_Gfx[];
extern const struct SpriteTemplate gSpriteTemplate_RibbonIconBig;
extern const struct SpriteSheet gUnknown_083E4784;
extern const struct SpritePalette gUnknown_083E478C;
extern const struct SpriteTemplate gSpriteTemplate_ConditionSparkle;
extern const s16 gConditionSparkleCoords[][2];
extern const u8 gSpinningPokenav_Gfx[];
extern const struct SpritePalette gUnknown_083E4818;
extern const struct SpriteTemplate gSpinningPokenavSpriteTemplate;
extern const struct SpritePalette gUnknown_083E4868;
extern const struct SpriteTemplate gSpriteTemplate_83E4878;

// Static ROM declarations

u8 sub_80F5FB4(void);
u8 sub_80F6010(void);

// .rodata

extern const u8 gConditionToLineLength[];

// .text

void sub_80EEDE8(void)
{
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;
    REG_BG2VOFS = 0;
    REG_BG2HOFS = 0;
    REG_BG3HOFS = 0;
    REG_BG3VOFS = 0;
}

void sub_80EEE08(void)
{
    gPokenavStructPtr->unkD160 = 0;
}

void sub_80EEE20(void)
{
    gPokenavStructPtr->unkD160 = 0;
    if (!gPokenavStructPtr->unk6DAC)
    {
        while (sub_80EEE54())
            ;
    }
}

bool8 sub_80EEE54(void)
{
    switch (gPokenavStructPtr->unkD160)
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
        sub_80EF58C(0);
        sub_80EF58C(1);
        sub_80EF58C(2);
        break;
    case 4:
        gPokenavStructPtr->unk030C = 0;
        REG_BG1HOFS = 0;
        REG_BG1VOFS = 0;
        REG_BG1CNT = 0x1B0C;
        gPokenavStructPtr->unkD160++;
    default:
        return FALSE;
    }
    gPokenavStructPtr->unkD160++;
    return TRUE;
}

bool8 sub_80EEF34(void)
{
    bool8 retVal = TRUE;

    if (gPokenavStructPtr->unk030C == 32)
        return FALSE;

    gPokenavStructPtr->unk030C += 2;
    if (gPokenavStructPtr->unk030C > 31)
    {
        gPokenavStructPtr->unk030C = 32;
        retVal = FALSE;
    }

    REG_BG1VOFS = gPokenavStructPtr->unk030C;
    return retVal;
}

bool8 sub_80EEF78(void)
{
    bool8 retVal = TRUE;

    if (gPokenavStructPtr->unk030C == 0)
        return FALSE;

    gPokenavStructPtr->unk030C -= 2;
    if (gPokenavStructPtr->unk030C <= 0)
    {
        gPokenavStructPtr->unk030C = 0;
        retVal = FALSE;
    }

    REG_BG1VOFS = gPokenavStructPtr->unk030C;
    return retVal;
}

void sub_80EEFBC(u8 a)
{
    u8 var1;

    sub_809D104((void *)VRAM + 0xD800, 0, 22, gUnknown_08E9A100, 0, 0, 17, 2);

    switch (a)
    {
    case 0:
        sub_809D104((void *)VRAM + 0xD800, 0, 22, gUnknown_08E9A100, 17, 0, 10, 2);
        sub_809D104((void *)VRAM + 0xD800, 10, 22, gUnknown_08E9A100, 0, 6, 7, 2);
        break;
    case 11:
        sub_809D104((void *)VRAM + 0xD800, 8, 22, gUnknown_08E9A100, 0, 6, 7, 2);
        break;
    case 2:
        var1 = gPokenavStructPtr->unk6DAC;
        if (!var1)
        {
            sub_809D104((void *)VRAM + 0xD800, 0, 22, gUnknown_08E9A100, 10, 2, 10, 2);
            sub_809D104((void *)VRAM + 0xD800, 10, 22, gUnknown_08E9A100, var1, 6, 7, 2);
        }
        else
        {
            sub_809D104((void *)VRAM + 0xD800, 10, 22, gUnknown_08E9A100, 0, 6, 7, 2);
        }
        break;
    case 3:
        sub_809D104((void *)VRAM + 0xD800, 0, 22, gUnknown_08E9A100, 0, 4, 10, 2);
        sub_809D104((void *)VRAM + 0xD800, 10, 22, gUnknown_08E9A100, 0, 6, 7, 2);
        break;
    case 4:
        sub_809D104((void *)VRAM + 0xD800, 0, 22, gUnknown_08E9A100, 20, 2, 10, 2);
        sub_809D104((void *)VRAM + 0xD800, 10, 22, gUnknown_08E9A100, 0, 6, 7, 2);
        break;
    case 7:
        sub_809D104((void *)VRAM + 0xD800, 0, 22, gUnknown_08E9A100, 10, 4, 10, 2);
        sub_809D104((void *)VRAM + 0xD800, 7, 22, gUnknown_08E9A100, 0, 6, 7, 2);
        break;
    case 8:
        sub_809D104((void *)VRAM + 0xD800, 0, 22, gUnknown_08E9A100, 20, 4, 10, 2);
        sub_809D104((void *)VRAM + 0xD800, 7, 22, gUnknown_08E9A100, 0, 6, 7, 2);
        break;
    case 5:
    case 9:
        sub_809D104((void *)VRAM + 0xD800, 0, 22, gUnknown_08E9A100, 0, 2, 10, 2);
        sub_809D104((void *)VRAM + 0xD800, 8, 22, gUnknown_08E9A100, 0, 6, 7, 2);
        break;
    case 10:
        sub_809D104((void *)VRAM + 0xD800, 8, 22, gUnknown_08E9A100, 0, 6, 7, 2);
        break;
    }
}

void sub_80EF248(u8 a)
{
    gPokenavStructPtr->unkD160 = 0;

    if (!gPokenavStructPtr->unk6DAC)
    {
        while (sub_80EF284(a))
            ;
    }
}

bool8 sub_80EF284(u8 a)
{
    switch (gPokenavStructPtr->unkD160)
    {
    case 0:
        sub_80EEDE8();
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
    DmaCopy16Defvars(3, gPokenavBgDotsTiles, (void *)VRAM + 0x5000, 0xA0);
        break;
    case 5:
        LZ77UnCompVram(gPokenavBgDotsTilemap, (void *)VRAM + 0xF800);
        break;
    case 6:
    DmaCopy16Defvars(3, gUnknown_083E005C, (void *)VRAM + 0x8000, 0x20);
        break;
    case 7:
        LZ77UnCompVram(gUnknown_083E007C, (void *)VRAM + 0xE000);
        break;
    case 8:
        LZ77UnCompVram(gPokenavDeviceBgTilemap, (void *)VRAM + 0xE800);
        break;
    case 9:
        LZ77UnCompVram(gPokenavDeviceBgTiles, (void *)VRAM + 0x8020);
        break;
    case 10:
        sub_80EF54C(a);
        LoadPalette(gPokenavBgDotsPal, 0xF0, 0x20);
        LoadPalette(gPokenavDeviceBgPal, 0x40, 0x20);
        CreateMovingBgDotsTask();
        break;
    case 11:
        REG_BG0CNT = 0x1F01;
        REG_BG2CNT = 0x1D0A;
        REG_BG3CNT = 0x1C0B;
        REG_BLDCNT = 0;

        gPokenavStructPtr->unkD160++;
        return FALSE;
    default:
        return FALSE;
    }

    gPokenavStructPtr->unkD160++;
    return TRUE;
}

void sub_80EF428(u8 a, u8 b)
{
    u8 *tileBuffer;
    const u8 *pcText = 0;

    switch (a)
    {
    case 0:
        pcText = gUnknown_083E31B0[b];
        break;
    case 1:
        pcText = gUnknown_083E31CC[b];
        break;
    case 2:
        pcText = gUnknown_083E31D8[b];
        break;
    }

    tileBuffer = gUnknown_083DFEC8;
    AlignStringInMenuWindow(&tileBuffer[0x800], pcText, 0xC0, 2);
    Menu_PrintText(&tileBuffer[0x800], 3, 17);
}

void sub_80EF490(u8 a)
{
    u16 var1, var2;

    if (a == 2)
        a = 1;

    gPokenavStructPtr->unkCE4E = a * 30;
    var1 = gPokenavStructPtr->unkCE4C;
    var2 = a * 30;
    if (var1 < var2)
        gPokenavStructPtr->unkCE50 = 2;
    else if (var1 > var2)
        gPokenavStructPtr->unkCE50 = -2;
    else
        gPokenavStructPtr->unkCE50 = 0;
}

bool8 sub_80EF4F8(void)
{
    u16 *palettes;

    if (gPokenavStructPtr->unkCE4C == gPokenavStructPtr->unkCE4E)
    {
        return FALSE;
    }
    else
    {
        gPokenavStructPtr->unkCE4C = gPokenavStructPtr->unkCE50 + gPokenavStructPtr->unkCE4C;

        palettes = gPokenavStructPtr->palettesCE52;
        LoadPalette(&palettes[gPokenavStructPtr->unkCE4C], 0x31, 4);
        return TRUE;
    }

}

void sub_80EF54C(u8 a)
{
    if (a == 2)
        a = 1;

    gPokenavStructPtr->unkCE4C = a * 30;
    LoadPalette(&gPokenavStructPtr->palettesCE52[gPokenavStructPtr->unkCE4C], 0x31, 4);
}

void sub_80EF58C(u8 a)
{
    u16 i;
    u16 * palettes;
    const u16 *var1;

    switch (a)
    {
    case 0:
        for (i = 0; i < 62; i++)
            gPokenavStructPtr->palettesCE52[i] = 0;
        break;
    case 1:
        palettes = gPokenavStructPtr->palettesCE52;
        var1 = gUnknown_083E003C;
        sub_80EF624(&var1[1], &var1[3], 16, 2, palettes);
        break;
    case 2:
        palettes = gPokenavStructPtr->palettesCE8E;
        var1 = gUnknown_083E003C;
        sub_80EF624(&var1[3], &var1[7], 16, 2, palettes);
        break;
    }
}

void sub_80EF624(const u16 *src, const u16 *dest, u8 c, u8 size, u16 *palettes)
{
    u16 i;
    u16 j;
    u16 *palette;

    for (i = 0; i < size; i++)
    {
        s32 r1 = Q_24_8(GET_R(*src));
        s32 g1 = Q_24_8(GET_G(*src));
        s32 b1 = Q_24_8(GET_B(*src));
        s32 r2 = Q_24_8(GET_R(*dest));
        s32 g2 = Q_24_8(GET_G(*dest));
        s32 b2 = Q_24_8(GET_B(*dest));
        s32 dr = (r2 - r1) / c;
        s32 dg = (g2 - g1) / c;
        s32 db = (b2 - b1) / c;
        u16 rf, gf, bf;

        palette = palettes;
        for (j = 0; j < c - 1; j++)
        {
            rf = Q_24_8_TO_INT(r1);
            gf = Q_24_8_TO_INT(g1);
            bf = Q_24_8_TO_INT(b1);
            *palette = RGB2(rf, gf, bf);
            palette += size;
            r1 += dr;
            g1 += dg;
            b1 += db;
        }
        rf = Q_24_8_TO_INT(r2);
        gf = Q_24_8_TO_INT(g2);
        bf = Q_24_8_TO_INT(b2);
        *palette = RGB2(rf, gf, bf);
        src++;
        dest++;
        palettes++;
    }
}

void sub_80EF740(void)
{
    gPokenavStructPtr->unk6DA0 = (gPokenavStructPtr->unk6DA0 + 1) & 1;
    if (gPokenavStructPtr->unk6DA0)
        gPokenavStructPtr->unk6DA2++;

    REG_BG3HOFS = gPokenavStructPtr->unk6DA2;
}

void Task_MoveBgDots(u8 taskId)
{
    if (gTasks[taskId].data[0] == 0 || (gPokenavStructPtr->unk6DA2 & 0x7) != 0)
    {
        sub_80EF740();
    }
    else
    {
        u16 value = gPokenavStructPtr->unk6DA2 & 0x7;
        gPokenavStructPtr->unk6DA2 = value;
        gPokenavStructPtr->unk6DA0 = value;
        REG_BG3HOFS = value;
    }
}

void CreateMovingBgDotsTask(void)
{
    gPokenavStructPtr->unk6DA2 = 0;
    gPokenavStructPtr->unk6DA0 = 0;
    gPokenavStructPtr->bg3ScrollTaskId = CreateTask(Task_MoveBgDots, 80);
}

void DestroyMovingDotsBgTask(void)
{
    if (FuncIsActiveTask(Task_MoveBgDots))
        DestroyTask(gPokenavStructPtr->bg3ScrollTaskId);
}

void sub_80EF840(void)
{
    gPokenavStructPtr->unkD160 = 0;

    if (gPokenavStructPtr->unk6DAC == 0)
    {
        while (sub_80EF874() != 0)
            ;
    }
}

bool8 sub_80EF874(void)
{
    switch (gPokenavStructPtr->unkD160)
    {
    case 0:
        sub_80EEDE8();
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
        LZ77UnCompVram(gRegionMapCityZoomTiles_Gfx, (void *)VRAM + 0x5000);
        break;
    case 8:
        LoadPalette(gRegionMapCityZoomTiles_Pal, 0x30, 0x20);
        sub_80EFC3C();
        break;
    case 9:
        if (sub_80EFC64())
            return TRUE;
        break;
    case 10:
        Menu_DrawStdWindowFrame(13, 3, 29, 17);
        sub_80EF9F8();
        break;
    case 11:
        if (!gPokenavStructPtr->regionMap.zoomed)
        {
            gPokenavStructPtr->unk7698 = 160;
            REG_BG0VOFS = 160;
        }
        else
        {
            gPokenavStructPtr->unk7698 = 256;
            REG_BG0VOFS = 0;
        }

        REG_BG0CNT = REG_BG0CNT;
        REG_BG0CNT |= 1;
        REG_BLDCNT = 0;
        break;
    default:
        return FALSE;
    }

    gPokenavStructPtr->unkD160++;
    return TRUE;
}

asm(".include \"constants/gba_constants.inc\"\n");

void sub_80EF9F8(void)
{
    bool8 someBool = FALSE;
    u16 top = 4;
    u16 mapSectionId;
    u8 pos;

    switch (gPokenavStructPtr->regionMap.mapSecType)
    {
    case 0:
        break;
    case 1:
    case 4:
        sub_8072A18(gPokenavStructPtr->regionMap.mapSecName, 0x70, top * 8, 0x78, 1);
        top += 2;
        if (gLinkOpen == TRUE)
        {
            sub_80F1A80();
            someBool = TRUE;
        }
        else
        {
            u16 i;

            for (i = 0; i < 4; i++)
            {
                const u8 *landmarkName = GetLandmarkName(
                    gPokenavStructPtr->regionMap.mapSecId,
                    gPokenavStructPtr->regionMap.posWithinMapSec,
                    i);

                if (landmarkName == NULL)
                    break;
                sub_8072A18(landmarkName, 0x70, top * 8, 0x78, 1);
                top += 2;
            }
        }
        break;
    case 2:
        sub_8072A18(gPokenavStructPtr->regionMap.mapSecName, 0x70, top * 8, 0x78, 1);
        top += 2;
        mapSectionId = gPokenavStructPtr->regionMap.mapSecId;
        pos = gPokenavStructPtr->regionMap.posWithinMapSec;
        if (gPokenavStructPtr->unkCDCC[mapSectionId][pos] != NULL)
        {
            Menu_BlankWindowRect(14, top, 15, 15);
            Menu_BlankWindowRect(26, top, 28, 15);
            sub_8095C8C((void *)(VRAM + 0xF800), 16, 6, gPokenavStructPtr->unkCDCC[mapSectionId][pos], 0, 0, 10, 10, 10);
            top += 11;
        }
        break;
    case 3:
        sub_8072A18(gPokenavStructPtr->regionMap.mapSecName, 0x70, top * 8, 0x78, 1);
        top += 2;
        break;
    }

    // Epic fail by the compiler at optimizing this.
    if (!someBool && top < 16)
        Menu_BlankWindowRect(14, top, 28, 15);

    if (gPokenavStructPtr->regionMap.mapSecType == 2)
        sub_80EFD74();
    else
        sub_80EFDA0();
}

void sub_80EFBB0(void)
{
    if (!gPokenavStructPtr->regionMap.zoomed)
        sub_80EEFBC(8);
    else
        sub_80EEFBC(7);
}

bool8 sub_80EFBDC(bool8 a)
{
    bool8 retVal = TRUE;
    u16 var1 = gPokenavStructPtr->unk7698;

    if (a)
    {
        if (var1 > 168)
        {
            var1 = var1 - 8;
        }
        else
        {
            var1 = 160;
            retVal = FALSE;
        }
    }
    else
    {
        if (var1 < 248)
        {
            var1 = var1 + 8;
        }
        else
        {
            var1 = 256;
            retVal = FALSE;
        }
    }

    gPokenavStructPtr->unk7698 = var1;
    REG_BG0VOFS = var1 & 0xFF;

    return retVal;
}

void sub_80EFC3C(void)
{
    gPokenavStructPtr->unkBC9A = 0;
    gPokenavStructPtr->unkBC9B = 0;
    sub_80EFD3C();
}

bool8 sub_80EFC64(void)
{
    u16 i;
    u8 var1;
    u16 var2;

    if (gPokenavStructPtr->unkBC9A >= 16)
        return FALSE;

    var1 = gPokenavStructPtr->unkBC9A;
    var2 = gPokenavStructPtr->unkBC9B;
    for (i = 0; i < 2; i++)
    {
        if (gPokenavCityMaps[var1][i] != 0)
        {
            LZ77UnCompVram(gPokenavCityMaps[var1][i], gPokenavStructPtr->cityZoomPics[var2]);
            gPokenavStructPtr->unkCDCC[var1][i] = gPokenavStructPtr->cityZoomPics[var2];
            var2++;
        }
        else
        {
            gPokenavStructPtr->unkCDCC[var1][i] = NULL;
        }
    }

    if (++gPokenavStructPtr->unkBC9A >= 16)
        return FALSE;

    gPokenavStructPtr->unkBC9B = var2;
    return TRUE;
}

void sub_80EFD3C(void)
{
    gPokenavStructPtr->unk769E = 0;
    gPokenavStructPtr->unk769C = 47;
    gPokenavStructPtr->unk769D = 0;
    gPokenavStructPtr->unk769A = 0;
}

void sub_80EFD74(void)
{
    gPokenavStructPtr->unk769E = 1;
    if (gPokenavStructPtr->unk769D == 1)
        gPokenavStructPtr->unk769D = 2;
}

void sub_80EFDA0(void)
{
    sub_8095C8C((void *)VRAM + 0xF800, 14, 16, gUnknown_08E9AC2C, 0, 0, 15, 1, 15);
    gPokenavStructPtr->unk769E = 0;
}

void sub_80EFDE4(u8 param0)
{
    u16 var1 = 60 - gPokenavStructPtr->unk769C;

    if (var1 > 15)
        var1 = 15;

    if (gPokenavStructPtr->unk769E != 0)
    {
        sub_8095C8C((void *)VRAM + 0xF800, 14, 16, gUnknown_08E9ABB4, gPokenavStructPtr->unk769C, 0, var1, 1, 60);

        if (var1 < 15)
        {
            u16 var2 = var1 + 14;

            sub_8095C8C((void *)VRAM + 0xF800, var2, 16, gUnknown_08E9ABB4, 0, 0, (u16)(15 - var1), 1, 60);
        }
    }
}

void sub_80EFE7C(void)
{
    u16 var1;
    u8 var2 = gPokenavStructPtr->unk769D;

    switch (var2)
    {
    case 0:
        var1 = ++gPokenavStructPtr->unk769C;

        if (var1 > 59)
            gPokenavStructPtr->unk769C = var2;

        sub_80EFDE4(gPokenavStructPtr->unk769E);

        switch (gPokenavStructPtr->unk769C)
        {
        case 0:
        case 15:
        case 30:
        case 45:
            gPokenavStructPtr->unk769D = 1;
            gPokenavStructPtr->unk769A = 0;
            break;
        }
        break;
    case 1:
        var1 = ++gPokenavStructPtr->unk769A;
        if (var1 > 120)
        {
            gPokenavStructPtr->unk769A = 0;
            gPokenavStructPtr->unk769D = 0;
        }
        break;
    case 2:
        sub_80EFDE4(1);
        gPokenavStructPtr->unk769D = 1;
        break;
    }
}

void sub_80EFF34(void)
{
    gPokenavStructPtr->unkD160 = 0;

    if (gPokenavStructPtr->unk6DAC == 0)
    {
        while (sub_80EFF68())
            ;
    }
}

bool8 sub_80EFF68(void)
{
    switch (gPokenavStructPtr->unkD160)
    {
    case 0:
        sub_80EEDE8();
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
        sub_80F1614();
        break;
    case 6:
        if (sub_80F162C(0))
            return TRUE;
        break;
    case 7:
        LZ77UnCompVram(gPokenavCondition_Gfx, (void *)VRAM + 0x5000);
        break;
    case 8:
        LZ77UnCompVram(gPokenavCondition_Tilemap, (void *)VRAM + 0xF000);
        LoadPalette(gPokenavCondition_Pal, 0x20, 0x20);
        break;
    case 9:
        if (gPokenavStructPtr->inSearchMode == 1)
            sub_8095C8C((void *)VRAM + 0xF000, 0, 5, gUnknown_083E01AC, 0, 0, 9, 4, 9);
        break;
    case 10:
        LZ77UnCompVram(gGraphData_Tilemap, (void *)VRAM + 0xB800);
        break;
    case 11:
        LoadPalette(gConditionGraphData_Pal, 0x30, 0x20);
        LoadPalette(gUnknownPalette_81E6692, 0xB0, 0x20);
        LoadPalette(&gPokenavCondition_Pal[2], 0xB1, 0x2);
        LoadPalette(&gPokenavCondition_Pal[16], 0xB5, 0x2);
        LoadPalette(&gPokenavCondition_Pal[30], 0xBF, 0x2);
        ConditionGraph_InitWindow();
        break;
    case 12:
        sub_80F01E0(gPokenavStructPtr->loadId);
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

    gPokenavStructPtr->unkD160++;
    return TRUE;
}

void ToggleGraphData(bool8 bg2on)
{
    if (bg2on)
        REG_DISPCNT |= DISPCNT_BG2_ON;
    else
        REG_DISPCNT &= ~DISPCNT_BG2_ON;
}

void ConditionGraph_InitWindow(void)
{
    REG_WIN0H = WIN_RANGE(0, 240);
    REG_WIN1H = WIN_RANGE(0, 155);
    REG_WIN0V = WIN_RANGE(56, 121);
    REG_WIN1V = WIN_RANGE(56, 121);
    REG_WININ = 0x3F3F;
    REG_WINOUT = 0x001B;
}

void sub_80F01E0(u16 bufferIndex)
{
    Menu_PrintText(gPokenavStructPtr->nameText[bufferIndex], 13, 1);

    if (gPokenavStructPtr->inSearchMode == 1)
    {
        Menu_PrintText(gPokenavStructPtr->locationText[bufferIndex], 13, 3);
        sub_80F443C(gPokenavStructPtr->unk8788, gPokenavStructPtr->unk893c[gPokenavStructPtr->unk87DC].unk2_5);
        Menu_PrintText(gPokenavStructPtr->unk8788, 1, 6);
    }
}

void sub_80F0264(u8 a)
{
    gPokenavStructPtr->unk306 = 0;
    gPokenavStructPtr->unk87CA = a;

    if (gPokenavStructPtr->unk6DAC == 0)
    {
        while (sub_80F02A0())
            ;
    }
}

bool8 sub_80F02A0(void)
{
    const u16 *pointer;

    switch (gPokenavStructPtr->unk306)
    {
    case 0:
        sub_80EEDE8();

        gPokenavStructPtr->unk87C8 = gPokenavStructPtr->unk87CA == 1;
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

        if (gPokenavStructPtr->unk87CA == 0)
        {
            LoadPalette(gPokenavConditionSearch2_Pal, 0x30, 0x20);
            gPlttBufferUnfaded[0] = gPokenavConditionSearch2_Pal[5];
            LoadPalette(gUnknownPalette_81E6692, 0xB0, 0x20);
            LoadPalette(&gUnknown_083E02B4[1], 0xB1, 0x2);
            LoadPalette(&gUnknown_083E02B4[8], 0xB5, 0x2);
            LoadPalette(&gPokenavConditionSearch2_Pal[5], 0xBF, 0x2);
        }
        else if (gPokenavStructPtr->unk87CA == 1)
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
        if (gPokenavStructPtr->unk87CA != 2)
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
        sub_80F0900();
        break;
    case 12:
        if (sub_80F0944())
        {
            return TRUE;
        }
        break;
    case 13:
        if (gPokenavStructPtr->unk87CA != 2)
        {
            ShowMapNamePopUpWindow();
        }
        else
        {
            sub_80F081C(0);
            sub_80F0FFC((u8)gPokenavStructPtr->unk876E);
        }
        break;
    case 14:
        REG_BG2CNT = 0x1D0A;
        REG_BG3CNT = 0x1E03;
        REG_BG0CNT = 0x1F01;
        REG_BG3VOFS = 0xF8;

        gPokenavStructPtr->unk8776 = 0xF8;
        gPokenavStructPtr->unk8778 = 0;

        REG_BLDCNT = 0;
        gPokenavStructPtr->unkD160++;
        return FALSE;
    default:
        return FALSE;
    }

    gPokenavStructPtr->unk306++;
    return TRUE;
}

void sub_80F063C(s16 arg0)
{
    s16 var0;
    s16 var1;
    s16 var2;

    gPokenavStructPtr->unk877C = arg0 * 16;
    gPokenavStructPtr->unk877A = (arg0 == 1 || arg0 == -1) ? 4 : 8;
    if (arg0 < 0)
    {
        gPokenavStructPtr->unk877A *= -1;
        var0 = arg0 * 2 + gPokenavStructPtr->unk8778;
        var1 = arg0 + gPokenavStructPtr->windowTopIndex;
        if (var1 < 0)
            var1 += gPokenavStructPtr->unk8774 + 1;

        arg0 *= -1;
    }
    else
    {
        var0 = gPokenavStructPtr->unk8778 + 16;
        var1 = gPokenavStructPtr->unk8772 + 1;
    }

    if (var1 > gPokenavStructPtr->unk8774)
        var1 = 0;

    var2 = var0 & 0x1F;
    sub_80F0954(var1, var2, arg0);
}

bool8 sub_80F0718(void)
{
    if (!sub_80F098C())
    {
        if (gPokenavStructPtr->unk877C == 0)
            return FALSE;

        gPokenavStructPtr->unk877C -= gPokenavStructPtr->unk877A;
        gPokenavStructPtr->unk8776 += gPokenavStructPtr->unk877A;
        gPokenavStructPtr->unk8776 &= 0xFF;
        REG_BG3VOFS = gPokenavStructPtr->unk8776;
        if (gPokenavStructPtr->unk877C == 0)
        {
            gPokenavStructPtr->unk8778 = ((8 + gPokenavStructPtr->unk8776) & 0xFF) / 8;
            return FALSE;
        }
    }

    return TRUE;
}

void ShowMapNamePopUpWindow(void)
{
    switch (gPokenavStructPtr->unk87CA)
    {
    case 0:
        sub_80F443C(gPokenavStructPtr->unk8788, gPokenavStructPtr->unk893c[gPokenavStructPtr->unk876E].unk2_5);
        break;
    case 1:
        sub_80F445C(gPokenavStructPtr->unk8788, gPokenavStructPtr->unk876E + 1);
        break;
    default:
        return;
    }

    BasicInitMenuWindow(&gWindowTemplate_81E710C);
    Menu_PrintText(gPokenavStructPtr->unk8788, 1, 6);
}

void sub_80F081C(u8 arg0)
{
    u32 numTrainerBattles;

    BasicInitMenuWindow(&gWindowTemplate_81E710C);
    switch (arg0)
    {
    case 0:
    case 1:
        MenuPrint_RightAligned(gText_NumberRegistered, 10, 9);
        if (arg0 != 0)
            break;
        // fall through
    case 2:
        ConvertIntToDecimalStringN(
            gPokenavStructPtr->unk8788,
            gPokenavStructPtr->unk8774 + 1,
            STR_CONV_MODE_RIGHT_ALIGN,
            5);
        MenuPrint_RightAligned(gPokenavStructPtr->unk8788, 10, 11);
        if (arg0 != 0)
            break;
        // fall through
    case 3:
        MenuPrint_RightAligned(gText_NumberOfBattles, 10, 13);
        if (arg0 != 0)
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

void sub_80F08E4(void)
{
    BasicInitMenuWindow(&gWindowTemplate_81E710C);
    Menu_EraseWindowRect(0, 9, 11, 16);
}

void sub_80F0900(void)
{
    s16 var0 = (gPokenavStructPtr->unk8772 - gPokenavStructPtr->windowTopIndex) + 1;
    if (var0 < 8)
        Menu_EraseWindowRect(12, 1, 31, 15);

    sub_80F0954(gPokenavStructPtr->windowTopIndex, 0, var0);
}

bool8 sub_80F0944(void)
{
    return sub_80F098C();
}

void sub_80F0954(u16 arg0, u16 arg1, u16 arg2)
{
    gPokenavStructPtr->unk877E = arg0;
    gPokenavStructPtr->unk8780 = arg1;
    gPokenavStructPtr->unk8784 = arg2;
    gPokenavStructPtr->unk8786 = 0;
}

bool8 sub_80F098C(void)
{
    s32 r8;
    if (gPokenavStructPtr->unk8784 == 0)
        return FALSE;
    r8 = 0;
    while (1)
    {
        gUnknown_083E3270[gPokenavStructPtr->unk87CA](gPokenavStructPtr->unk877E, gPokenavStructPtr->unk8780);
        if (--gPokenavStructPtr->unk8784 == 0)
            return FALSE;
        if (++gPokenavStructPtr->unk877E > gPokenavStructPtr->unk8774)
            gPokenavStructPtr->unk877E = r8;
        gPokenavStructPtr->unk8780 += 2;
        gPokenavStructPtr->unk8780 &= 0x1F;
        break;
    }
    return TRUE;
}

void sub_80F0A24(u16 arg0, u16 arg1)
{
    u8 var0 = gPokenavStructPtr->unk87C8 == 0 ? 2 : 1;
    sub_80F4428(gPokenavStructPtr->unk8788, arg0, var0);
    BasicInitMenuWindow(&gWindowTemplate_81E70D4);
    Menu_PrintText(gPokenavStructPtr->unk8788, 13, arg1);
}

void sub_80F0A74(u16 arg0, u16 arg1)
{
    sub_80F700C(gPokenavStructPtr->unk8788, arg0);
    arg1 &= 0x1F;
    BasicInitMenuWindow(&gWindowTemplate_81E70D4);
    Menu_PrintTextPixelCoords(gPokenavStructPtr->unk8788, 97, arg1 * 8, 0);
    if (arg0 < gPokenavStructPtr->unkD158 && gPokenavStructPtr->trainersEye[arg0].rematchNo != 0)
        sub_8095C8C((void *)VRAM + 0xF000, 29, arg1, gUnknown_083E039C, 0, 0, 1, 2, 1);
    else
        sub_8095C8C((void *)VRAM + 0xF000, 29, arg1, gUnknown_083E03A0, 0, 0, 1, 2, 1);
}

void sub_80F0B24(void)
{
    gPokenavStructPtr->unkD15C = 0;
    gPokenavStructPtr->unk306 = 0;
}

bool8 sub_80F0B44(void)
{
    int top;
    if (gPokenavStructPtr->unkD15C > 8)
    {
        return FALSE;
    }

    if (++gPokenavStructPtr->unk306 > 1)
    {
        gPokenavStructPtr->unk306 = 0;
        if (gPokenavStructPtr->unkD15C < 8)
        {
            top = (gPokenavStructPtr->unk8778 + (gPokenavStructPtr->unkD15C * 2)) & 0x1F;
            if (gPokenavStructPtr->unkD15C != gPokenavStructPtr->selectedIndexOffset)
            {
                BasicInitMenuWindow(&gWindowTemplate_81E70D4);
                Menu_EraseWindowRect(12, top, 31, top + 1);
            }

            if (!gPokenavStructPtr->unkD15C)
                sub_80F08E4();

            gPokenavStructPtr->unkD15C++;
            return TRUE;
        }
        else
        {
            u16 i;
            BasicInitMenuWindow(&gWindowTemplate_81E70D4);
            top = (gPokenavStructPtr->unk8778 + 16) & 0x1F;
            for (i = 0; i < 8; i++)
            {
                Menu_EraseWindowRect(12, top, 31, top + 1);
                top = (top + 2) & 0x1F;
            }

            gPokenavStructPtr->unkD15C++;
            return FALSE;
        }
    }
    else
    {
        return TRUE;
    }
}

void sub_80F0C28(void)
{
    gPokenavStructPtr->unkD15C = 0;
    gPokenavStructPtr->unk306 = 0;
}

bool8 sub_80F0C48(void)
{
    if (gPokenavStructPtr->unkD15C > 7)
        return FALSE;

    if (++gPokenavStructPtr->unk306 > 1)
    {
        gPokenavStructPtr->unk306 = 0;
        BasicInitMenuWindow(&gWindowTemplate_81E70D4);
        sub_80F0A74(gPokenavStructPtr->windowTopIndex + gPokenavStructPtr->unkD15C,
                    gPokenavStructPtr->unk8778 + gPokenavStructPtr->unkD15C * 2);

        if ((++gPokenavStructPtr->unkD15C) > 7)
        {
            sub_80F081C(0);
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
    gPokenavStructPtr->unkD15C = 0;
    trainerEyesId = gPokenavStructPtr->trainersEye[gPokenavStructPtr->unk876E].rematchTableIdx;
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

bool8 sub_80F0D5C(void)
{
    u32 r5;

    if (gPokenavStructPtr->unkD15C == 7)
        return FALSE;
    if (++gPokenavStructPtr->unk306 < 2)
        return TRUE;
    gPokenavStructPtr->unk306 = 0;
    BasicInitMenuWindow(&gWindowTemplate_81E70D4);
    r5 = (gPokenavStructPtr->unk8778 + 2 + gPokenavStructPtr->unkD15C * 2) & 0x1F;
#ifndef NONMATCHING
    asm("":::"r2"); // fakematch
#endif //NONMATCHING
    switch (gPokenavStructPtr->unkD15C)
    {
    default:
        return FALSE;
    case 0:
        Menu_PrintTextPixelCoords(gOtherText_Strategy, 0x61, r5 * 8, 0);
        break;
    case 1:
        AlignStringInMenuWindow(gPokenavStructPtr->unk8788, gPokenavStructPtr->trainerEyeDescriptionLines[0], 0x88, 0);
        Menu_PrintTextPixelCoords(gPokenavStructPtr->unk8788, 0x61, r5 * 8, 0);
        break;
    case 2:
        Menu_PrintTextPixelCoords(gOtherText_TrainersPokemon, 0x61, r5 * 8, 0);
        break;
    case 3:
        AlignStringInMenuWindow(gPokenavStructPtr->unk8788, gPokenavStructPtr->trainerEyeDescriptionLines[1], 0x88, 0);
        Menu_PrintTextPixelCoords(gPokenavStructPtr->unk8788, 0x61, r5 * 8, 0);
        break;
    case 4:
        Menu_PrintTextPixelCoords(gOtherText_SelfIntroduction, 0x61, r5 * 8, 0);
        break;
    case 5:
        AlignStringInMenuWindow(gPokenavStructPtr->unk8788, gPokenavStructPtr->trainerEyeDescriptionLines[2], 0x88, 0);
        Menu_PrintTextPixelCoords(gPokenavStructPtr->unk8788, 0x61, r5 * 8, 0);
        break;
    case 6:
        AlignStringInMenuWindow(gPokenavStructPtr->unk8788, gPokenavStructPtr->trainerEyeDescriptionLines[3], 0x88, 0);
        Menu_PrintTextPixelCoords(gPokenavStructPtr->unk8788, 0x61, r5 * 8, 0);
        return FALSE;
    }
    gPokenavStructPtr->unkD15C++;
    return TRUE;
}

void sub_80F0EC0(void)
{
    gPokenavStructPtr->unk306 = 0;
    gPokenavStructPtr->unkD15C = 0;
    StringFill(gPokenavStructPtr->unk8788, CHAR_SPACE, 16);
}

bool8 sub_80F0EF4(void)
{
    int top;
    if (gPokenavStructPtr->unkD15C > 6)
        return FALSE;

    if (++gPokenavStructPtr->unk306 > 1)
    {
        gPokenavStructPtr->unk306 = 0;
        top = (gPokenavStructPtr->unk8778 + 2 + gPokenavStructPtr->unkD15C * 2) & 0x1F;
        BasicInitMenuWindow(&gWindowTemplate_81E70D4);
        Menu_EraseWindowRect(12, top, 31, top + 1);
        gPokenavStructPtr->unkD15C++;
    }

    return TRUE;
}

void sub_80F0F64(void)
{
    s16 var0 = gPokenavStructPtr->selectedIndexOffset;
    gPokenavStructPtr->unk877C = var0 * 16;
    gPokenavStructPtr->unk877A = var0 == 1 ? 4 : 8;
}

void sub_80F0FA0(void)
{
    s16 var0 = gPokenavStructPtr->selectedIndexOffset * -1;
    gPokenavStructPtr->unk877C = var0 * 16;
    gPokenavStructPtr->unk877A = var0 == -1 ? -4 : -8;
}

bool8 sub_80F0FEC(void)
{
    return sub_80F0718();
}

void sub_80F0FFC(u8 arg0)
{
    GetMapSectionName(gPokenavStructPtr->unk8788, gPokenavStructPtr->trainersEye[arg0].regionMapSectionId, 0);
    BasicInitMenuWindow(&gWindowTemplate_81E710C);
    sub_80F19DC(gPokenavStructPtr->unk8788);
    AlignStringInMenuWindow(gPokenavStructPtr->unkD138, gPokenavStructPtr->unk8788, 88, 2);
    Menu_PrintText(gPokenavStructPtr->unkD138, 0, 5);
}

void sub_80F105C(void)
{
    sub_80F0A74(gPokenavStructPtr->unk876E, gPokenavStructPtr->unk8778);
}

bool8 sub_80F1080(void)
{
    switch (gPokenavStructPtr->unkD160)
    {
    case 0:
        sub_80EEDE8();
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
        sub_80F1614();
        break;
    case 6:
        if (sub_80F162C(1))
            return TRUE;
        break;
    case 7:
        LZ77UnCompWram(gPokenavRibbonsSummaryBg_Tilemap, gPokenavStructPtr->unk934C);
        break;
    case 8:
        DrawMonRibbonIcons();
        break;
    case 9:
        sub_80F13FC();
        break;
    case 10:
        LZ77UnCompVram(gPokenavRibbonsSummaryBg_Gfx, (void *)(VRAM + 0x8000));
        break;
    case 11:
        LZ77UnCompVram(gRibbonIconsSmall_Gfx, (void *)(VRAM + 0x8200));
        break;
    case 12:
        LoadPalette(gPokenavRibbonsSummaryBg_Pal, 0x20, 0x20);
        LoadPalette(gUnknown_083E03A8, 0xF0, 0x20);
        LoadPalette(gUnknown_083E3C60[0], 0x30, 0xA0);
        LoadPalette(gUnknownPalette_81E6692, 0xB0, 0x20);
        LoadPalette(gUnknown_083E03A8 + 0xF, 0xBF, 0x2);
        break;
    case 13:
        sub_80F1438();
        break;
    case 14:
        REG_BG2CNT = 0x1E02;
        REG_BG3CNT = 0x170B;
        REG_BLDCNT = 0;
        gPokenavStructPtr->unkD160++;
        return FALSE;
    default:
        return FALSE;
    }

    gPokenavStructPtr->unkD160++;
    return TRUE;
}

void DrawMonRibbonIcons(void)
{
    u16 i;
    u16 offset;
    u8 ribbonId, palNum, tileNum;
    u8 numNormalRibbons;

    offset = 0x8B;
    numNormalRibbons = gPokenavStructPtr->unkBC8E - gPokenavStructPtr->numGiftRibbons;

    for (i = 0; i < 8; i++)
    CpuFill16(0x2000, &gPokenavStructPtr->unk934C[offset + i * 32], 0x24);

    for (i = 0; i < numNormalRibbons; i++)
    {
        ribbonId = gPokenavStructPtr->unkBC4C[i];
        palNum = gRibbonGfxData[ribbonId][1] + 3;
        tileNum = gRibbonGfxData[ribbonId][0] * 2 + 0x10;
        gPokenavStructPtr->unk934C[offset] = (palNum << 12) | tileNum;
        gPokenavStructPtr->unk934C[offset + 1] = ((palNum << 12) | 0x400) | tileNum;
        gPokenavStructPtr->unk934C[offset + 0x20] = (palNum << 12) | (tileNum + 1);
        gPokenavStructPtr->unk934C[offset + 0x21] = (palNum << 12) | 0x400 | (tileNum + 1);

        if ((i + 1) % 9 == 0)
            offset += 0x30;
        else
            offset += 2;
    }

    offset = 0x14B;
    for (i = 0; i < gPokenavStructPtr->numGiftRibbons; i++)
    {
        ribbonId = gPokenavStructPtr->unkBC4C[numNormalRibbons + i];
        palNum = gRibbonGfxData[ribbonId][1] + 3;
        tileNum = gRibbonGfxData[ribbonId][0] * 2 + 0x10;
        gPokenavStructPtr->unk934C[offset] = (palNum << 12) | tileNum;
        gPokenavStructPtr->unk934C[offset + 1] = ((palNum << 12) | 0x400) | tileNum;
        gPokenavStructPtr->unk934C[offset + 0x20] = (palNum << 12) | (tileNum + 1);
        gPokenavStructPtr->unk934C[offset + 0x21] = (palNum << 12) | 0x400 | (tileNum + 1);

        offset += 2;
    }
}

void sub_80F13FC(void)
{
    u16 *src = gPokenavStructPtr->unk934C;
    u16 *dest = (u16 *)(VRAM + 0xB800);
    DmaCopy32(3, src, dest, 0x500);
    gPlttBufferUnfaded[0] = *(gPokenavRibbonsSummaryBg_Pal + 14);
}

void sub_80F1438(void)
{
    Menu_PrintText(gPokenavStructPtr->nameText[0], 13, 1);
    sub_80F445C(gPokenavStructPtr->unk8788, gPokenavStructPtr->unk876E + 1);
    Menu_PrintText(gPokenavStructPtr->unk8788, 1, 5);
}

void sub_80F1480(void)
{
    Menu_EraseWindowRect(12, 13, 27, 16);
}

// This is a fakematching function, due to a hardcoded access of gSaveBlock1.
// Due to this hardcoded address access, gift ribbons do not properly display
// their descriptions, since the hardcoded access is inside of the LinkBattleRecords
// save data, rather than the giftRibbons array, which is almost certainly what the
// intended access is.
void sub_80F1494(void)
{
    u8 *arr;
    u8 *tileBuffer1 = &gUnknown_083DFEC8[0x800];
    u8 *tileBuffer2 = &gUnknown_083DFEC8[0xA98];

    if (gPokenavStructPtr->unkBC91 < 3)
    {
        gUnknown_020388B4 = gPokenavStructPtr->unkBC91 * 9 + gPokenavStructPtr->unkBC90;
        gUnknown_020388B4 = gPokenavStructPtr->unkBC4C[gUnknown_020388B4];
        AlignStringInMenuWindow(tileBuffer1, gRibbonDescriptionPointers[gUnknown_020388B4][0], 128, 0);
        AlignStringInMenuWindow(tileBuffer2, gRibbonDescriptionPointers[gUnknown_020388B4][1], 128, 0);
    }
    else
    {
        gUnknown_020388B4 = gPokenavStructPtr->unkBC8E - gPokenavStructPtr->numGiftRibbons;
        gUnknown_020388B4 = gPokenavStructPtr->unkBC4C[gUnknown_020388B4 + gPokenavStructPtr->unkBC90];

        // FIXME!
        arr = ((u8*)&gSaveBlock1);
        asm("ldrh r1, [r5]\n\
            add r0, r0, r1");
        gUnknown_020388B4 = arr[0x30F7];
        // The bug fix for this code is the following:
        // gUnknown_020388B4 = gSaveBlock1.giftRibbons[gUnknown_020388B4];
        if (gUnknown_020388B4)
        {
            gUnknown_020388B4--;
            AlignStringInMenuWindow(tileBuffer1, gGiftRibbonDescriptionPointers[gUnknown_020388B4][0], 128, 0);
            AlignStringInMenuWindow(tileBuffer2, gGiftRibbonDescriptionPointers[gUnknown_020388B4][1], 128, 0);
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

void sub_80F15A8(void)
{
    u8 *buffer;
    Menu_EraseWindowRect(12, 13, 27, 16);
    buffer = StringCopy(gPokenavStructPtr->unk8788, gText_Ribbons);
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

void sub_80F1614(void)
{
    gPokenavStructPtr->unk8764 = 0;
}

bool8 sub_80F162C(u8 arg0)
{
    switch (gPokenavStructPtr->unk8764)
    {
    case 0:
        break;
    case 1:
        LZ77UnCompVram(gUnknown_08E9FF58, (void *)(VRAM + 0xE800));
        break;
    case 2:
    DmaCopy16Defvars(3, gPokenavRibbonPokeView_Gfx, (void *)(VRAM + 0xE000), 0xE0);
        break;
    case 3:
        if (!arg0)
            LoadPalette(gUnknown_083E0124, 0xD0, 0x20);
        else
            LoadPalette(gUnknown_083E0144, 0xD0, 0x20);

        gPokenavStructPtr->monTransitionX = -80;
        REG_BG0CNT = 0x1D0D;
        gPokenavStructPtr->unk8764++;
        return FALSE;
    default:
        return FALSE;
    }

    gPokenavStructPtr->unk8764++;
    return TRUE;
}

bool8 MoveConditionMonOnscreen(void)
{
    gPokenavStructPtr->monTransitionX += 0x10;
    if (gPokenavStructPtr->monTransitionX > 0)
        gPokenavStructPtr->monTransitionX = 0;

    return gPokenavStructPtr->monTransitionX != 0;
}

bool8 MoveConditionMonOffscreen(void)
{
    gPokenavStructPtr->monTransitionX -= 0x10;
    if (gPokenavStructPtr->monTransitionX < -0x50)
        gPokenavStructPtr->monTransitionX = -0x50;

    return gPokenavStructPtr->monTransitionX != -0x50;
}

bool8 sub_80F1778(void)
{
    switch (gPokenavStructPtr->unkD160)
    {
    case 0:
        sub_80EEDE8();
        gPokenavStructPtr->unkD162 = 2;
        break;
    case 1:
        sub_80F1614();
        break;
    case 2:
        if (sub_80F162C(0))
            return TRUE;
        break;
    case 3:
        LZ77UnCompVram(gPokenavCondition_Gfx, (void *)(VRAM + 0x5000));
        break;
    case 4:
        LZ77UnCompVram(gPokenavCondition_Tilemap, (void *)(VRAM + 0xF000));
        LoadPalette(gPokenavCondition_Pal, 0x20, 0x20);
        break;
    case 5:
        sub_8095C8C((void *)VRAM + 0xF000, 0, 13, gUnknown_083E01F4, 0, 0, 12, 4, 12);
        break;
    case 6:
        LZ77UnCompVram(gGraphData_Tilemap, (void *)(VRAM + 0xB800));
        break;
    case 7:
        LoadPalette(gConditionGraphData_Pal, 0x30, 0x20);
        LoadPalette(gUnknownPalette_81E6692, 0xB0, 0x20);
        LoadPalette(&gPokenavCondition_Pal[2], 0xB1, 0x2);
        LoadPalette(&gPokenavCondition_Pal[16], 0xB5, 0x2);
        LoadPalette(&gPokenavCondition_Pal[30], 0xBF, 0x2);
        ConditionGraph_InitWindow();
        break;
    case 8:
        sub_80F01E0(gPokenavStructPtr->loadId);
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

    gPokenavStructPtr->unkD160++;
    return TRUE;
}

void sub_80F1934(void)
{
    u8 *buffer = gPokenavStructPtr->unk8788;
    if (gPokenavStructPtr->unk893c[gPokenavStructPtr->unk87DC].unk3_14)
    {
        u8 nature = GetNature(&gPlayerParty[GetPartyIdFromSelectionId_(gPokenavStructPtr->unk87DC)]);
        buffer = StringCopy(buffer, gText_NatureSlash);
        AlignStringInMenuWindow(buffer, gNatureNamePointers[nature], 87, 0);
    }
    else
    {
        AlignStringInMenuWindow(buffer, gEmptyString_81E72B0, 87, 0);
    }

    Menu_PrintTextPixelCoords(gPokenavStructPtr->unk8788, 1, 112, 1);
}

void sub_80F19DC(u8 *text)
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

void sub_80F19FC(void)
{
    // FIXME r4/r5 swapped
    register u8 *ptr asm("r5") = gUnknown_020388B0;
    if (ptr[0] == 1)
    {
        const u8 *landmarkName = GetLandmarkName(
            gPokenavStructPtr->regionMap.mapSecId,
            gPokenavStructPtr->regionMap.posWithinMapSec,
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

void sub_80F1A74(void)
{
    gUnknown_020388B0[0] = 0;
}

void sub_80F1A80(void)
{
    gUnknown_020388B0[0] = 1;
    gUnknown_020388B0[1] = 0;
}

void sub_80F1A90(void)
{
    gPokenavStructPtr->unk306 = 0;
    if (gPokenavStructPtr->unk6DAC == 0)
        while (sub_80F1AC4());
}

bool8 sub_80F1AC4(void)
{
    u16 i, j;

    switch (gPokenavStructPtr->unk306)
    {
    case 0:
        for (i = 0; i < 6; i++)
        {
            for (j = 0; j < 4; j++)
                gPokenavStructPtr->iconSprites[i][j] = NULL;
        }

        gPokenavStructPtr->unk311 = 0;
        break;
    case 1:
        LZ77UnCompWram(gPokenavMenuOptions_Gfx, gPokenavStructPtr->unk388);
        break;
    case 2:
        LZ77UnCompWram(gPokenavConditionMenu_Gfx, gPokenavStructPtr->unk1788);
        break;
    case 3:
        LZ77UnCompWram(gPokenavConditionSearch_Gfx, gPokenavStructPtr->unk2388);
        return FALSE;
    }

    gPokenavStructPtr->unk306++;
    return TRUE;
}

void sub_80F1B8C(u8 menuType)
{
    gPokenavStructPtr->unk306 = 0;
    if (gPokenavStructPtr->unk6DAC == 0)
        while (sub_80F1BC8(menuType));
}

bool8 sub_80F1BC8(u8 menuType)
{
    u16 animNum;
    u16 topOffset;
    u16 deltaY;
    u16 middle;
    u8 spriteId;

    switch (gPokenavStructPtr->unk306)
    {
    case 0:
    {
        sub_80F2458(menuType);
        break;
    }
    case 1:
    {
        sub_80F2514(menuType);
        break;
    }
    case 2:
    {
        u16 i, j;

        switch (menuType)
        {
        case 0:
            topOffset = 42;
            deltaY = 20;
            gPokenavStructPtr->unk30E = 5;
            break;
        case 1:
            topOffset = 56;
            deltaY = 20;
            gPokenavStructPtr->unk30E = 3;
            break;
        case 2:
            topOffset = 40;
            deltaY = 16;
            gPokenavStructPtr->unk30E = 6;
            break;
        default:
            return FALSE;
        }

        animNum = 0;
        for (i = 0; i < gPokenavStructPtr->unk30E; i++)
        {
            middle = (deltaY * i) + topOffset - 8;
            gPokenavStructPtr->unk314[i] = (middle << 8) | (middle + 0x11);
            if (!menuType)
            {
                if (gPokenavStructPtr->unk6DB2[i] == 0)
                {
                    for (j = 0; j < 4; j++)
                        gPokenavStructPtr->iconSprites[i][j] = NULL;
                    continue;
                }
                else
                {
                    animNum = (gPokenavStructPtr->unk6DB2[i] - 1) * 4;
                }
            }

            for (j = 0; j < 4; j++)
            {
                spriteId = CreateSprite(&gSpriteTemplate_83E4454, j * 32 + 256, (deltaY * i) + topOffset, 0);
                if (spriteId == MAX_SPRITES)
                    return FALSE;
                gPokenavStructPtr->iconSprites[i][j] = &gSprites[spriteId];
                gPokenavStructPtr->iconSprites[i][j]->data[0] = i;
                gPokenavStructPtr->iconSprites[i][j]->data[1] = j;
                gPokenavStructPtr->iconSprites[i][j]->data[2] = j * 32 + 152;
                gPokenavStructPtr->iconSprites[i][j]->data[3] = j * 32 + 256;
                StartSpriteAnim(gPokenavStructPtr->iconSprites[i][j], animNum++);

                if ((menuType == 2 || menuType == 0) && i > 2)
                {
                    gPokenavStructPtr->iconSprites[i][j]->oam.paletteNum = IndexOfSpritePaletteTag(0x1);
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

void sub_80F1DF0(void)
{
    u16 i, j;
    for (i = 0; i < gPokenavStructPtr->unk30E; i++)
    {
        for (j = 0; j < 4; j++)
        {
            if (gPokenavStructPtr->iconSprites[i][j])
                gPokenavStructPtr->iconSprites[i][j]->callback = sub_80F2218;
        }
    }

    PlaySE(SE_WIN_OPEN);
}

bool8 sub_80F1E50(void)
{
    if (sub_80F22F8())
    {
        sub_80F2108();
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

bool8 sub_80F1E6C(void)
{
    return !sub_80F22F8();
}

void sub_80F1E84(void)
{
    u16 i, j;

    gPokenavStructPtr->unk30F = 0;
    sub_80F2148();
    for (i = 0; i < gPokenavStructPtr->unk30E; i++)
    {
        if (i != gPokenavStructPtr->cursorPos)
        {
            for (j = 0; j < 4; j++)
            {
                if (gPokenavStructPtr->iconSprites[i][j])
                    gPokenavStructPtr->iconSprites[i][j]->callback = sub_80F2240;
            }
        }
    }
}

bool8 sub_80F1F10(void)
{
    u16 j;

    switch (gPokenavStructPtr->unk30F)
    {
    case 0:
        if (sub_80F2360())
        {
            for (j = 0; j < 4; j++)
            {
                struct Sprite *sprite = gPokenavStructPtr->iconSprites[gPokenavStructPtr->cursorPos][j];
                sprite->oam.affineMode = ST_OAM_AFFINE_DOUBLE;
                sprite->affineAnims = gSpriteAffineAnimTable_83E4450;
                InitSpriteAffineAnim(sprite);
                sprite->data[4] = j * 4 - 6;
                sprite->data[4] /= 2;
                sprite->data[5] = sprite->data[4] * 8;
                sprite->callback = sub_80F2240;
            }

            sub_80F2170();
            gPokenavStructPtr->unk30F++;
        }
        break;
    case 1:
        if (sub_80F23C8())
        {
            sub_80F21F8();
            FreeSpriteTilesByTag(0x0);
            FreeSpritePaletteByTag(0x0);
            FreeSpritePaletteByTag(0x1);
            return FALSE;
        }
        break;
    }

    return TRUE;
}

void sub_80F1FF0(void)
{
    if (gPokenavStructPtr->unk311 == 1)
    {
        REG_WIN0V = gPokenavStructPtr->unk314[gPokenavStructPtr->cursorPos];
        REG_BLDY = gSineTable[gPokenavStructPtr->unk312] >> 5;
        gPokenavStructPtr->unk312 += 3;
        gPokenavStructPtr->unk312 &= 0x7F;
    }
    else if (gPokenavStructPtr->unk311 == 2)
    {
        REG_BLDALPHA = gUnknown_083E42F8[gPokenavStructPtr->unk312];
        if (gPokenavStructPtr->unk312 < 15)
            gPokenavStructPtr->unk312++;
    }
}

void sub_80F208C(void)
{
    sub_80F2148();
}

void sub_80F2098(void)
{
    REG_WIN0H = 0x77F0;
    REG_WIN0V = gPokenavStructPtr->unk314[gPokenavStructPtr->cursorPos];
    REG_WININ = 0x3F;
    REG_WINOUT = 0x1F;
    REG_DISPCNT |= DISPCNT_WIN0_ON;
}

void sub_80F20F4(void)
{
    REG_DISPCNT &= ~DISPCNT_WIN0_ON;
}

void sub_80F2108(void)
{
    if (!gPokenavStructPtr->unk311)
    {
        gPokenavStructPtr->unk311 = 1;
        gPokenavStructPtr->unk312 = 0;
        REG_BLDCNT = 0x90;
        REG_BLDY = 0;
        sub_80F2098();
    }
}

void sub_80F2148(void)
{
    gPokenavStructPtr->unk311 = 0;
    REG_BLDCNT = 0;
    sub_80F20F4();
}

void sub_80F2170(void)
{
    u16 j;

    if (!gPokenavStructPtr->unk311)
    {
        sub_80F20F4();
        for (j = 0; j < 4; j++)
        {
            struct Sprite *sprite = gPokenavStructPtr->iconSprites[gPokenavStructPtr->cursorPos][j];
            sprite->oam.objMode = ST_OAM_OBJ_BLEND;
        }

        gPokenavStructPtr->unk311 = 2;
        gPokenavStructPtr->unk312 = 0;
        REG_BLDCNT = 0x3F40;
        REG_BLDALPHA = 0x10;
    }
}

void sub_80F21F8()
{
    gPokenavStructPtr->unk311 = 0;
    REG_BLDCNT = 0;
}

void sub_80F2218(struct Sprite *sprite)
{
    sprite->x -= 8;
    if (sprite->x <= sprite->data[2])
    {
        sprite->x = sprite->data[2];
        sprite->callback = sub_80F22B0;
    }
}

void sub_80F2240(struct Sprite *sprite)
{
    if (sprite->data[0] == gPokenavStructPtr->cursorPos)
    {
        if (sprite->data[5])
        {
            sprite->x += sprite->data[4];
            sprite->data[5] -= sprite->data[4];
        }

        if (sprite->affineAnimEnded)
            sub_80F240C(sprite);
    }
    else
    {
        sprite->x += 8;
        if (sprite->x >= sprite->data[3])
            sub_80F240C(sprite);
    }
}

void sub_80F22B0(struct Sprite *sprite)
{
    if (sprite->data[0] == gPokenavStructPtr->cursorPos)
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

bool8 sub_80F22F8(void)
{
    u16 i, j;

    for (i = 0; i < gPokenavStructPtr->unk6DAE; i++)
    {
        for (j = 0; j < 4; j++)
        {
            struct Sprite *sprite = gPokenavStructPtr->iconSprites[i][j];
            if (!sprite)
                return TRUE;

            if (sprite->x2 != 0 && sprite->x2 != -16)
                return FALSE;
        }
    }

    return TRUE;
}

bool8 sub_80F2360(void)
{
    u16 i, j;

    for (i = 0; i < gPokenavStructPtr->unk30E; i++)
    {
        if (i != gPokenavStructPtr->cursorPos)
        {
            for (j = 0; j < 4; j++)
            {
                struct Sprite *sprite = gPokenavStructPtr->iconSprites[i][j];
                if (sprite)
                    return FALSE;
            }
        }
    }

    return TRUE;
}

bool8 sub_80F23C8(void)
{
    u16 j;

    for (j = 0; j < 4; j++)
    {
        struct Sprite *sprite = gPokenavStructPtr->iconSprites[gPokenavStructPtr->cursorPos][j];
        if (sprite)
            return FALSE;
    }

    return TRUE;
}

void sub_80F240C(struct Sprite *sprite)
{
    gPokenavStructPtr->iconSprites[sprite->data[0]][sprite->data[1]] = NULL;
    if (sprite->affineAnimEnded)
        FreeOamMatrix(sprite->oam.matrixNum);

    DestroySprite(sprite);
}

void sub_80F2458(u8 menuType)
{
    switch (menuType)
    {
    case 0:
        gPokenavStructPtr->unk380.data = gPokenavStructPtr->unk388;
        gPokenavStructPtr->unk380.size = sizeof(gPokenavStructPtr->unk388);
        gPokenavStructPtr->unk380.tag = 0x0;
        break;
    case 1:
        gPokenavStructPtr->unk380.data = gPokenavStructPtr->unk1788;
        gPokenavStructPtr->unk380.size = sizeof(gPokenavStructPtr->unk1788);
        gPokenavStructPtr->unk380.tag = 0x0;
        break;
    case 2:
        gPokenavStructPtr->unk380.data = gPokenavStructPtr->unk2388;
        gPokenavStructPtr->unk380.size = sizeof(gPokenavStructPtr->unk2388);
        gPokenavStructPtr->unk380.tag = 0x0;
        break;
    default:
        return;
    }

    LoadSpriteSheet(&gPokenavStructPtr->unk380);
}

void sub_80F2514(u8 menuType)
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

void sub_80F2598(void)
{
    u8 spriteId;

    gPokenavStructPtr->blueLightSprite = NULL;
    if (DoesSomeoneWantRematchIn(gSaveBlock1.location.mapGroup, gSaveBlock1.location.mapNum) == TRUE)
    {
        LoadSpriteSheet(&gSpriteSheet_PokenavBlueLight);
        LoadSpritePalette(&gSpritePalette_PokenavBlueLight);
        spriteId = CreateSprite(&gSpriteTemplate_83E4484, 12, 96, 0);
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

void sub_80F2688(void)
{
    gPokenavStructPtr->unk306 = 0;
    if (!gPokenavStructPtr->unk6DAC)
        while(sub_80F26BC());
}

bool8 sub_80F26BC(void)
{
    switch (gPokenavStructPtr->unk306)
    {
    case 0:
        LZ77UnCompWram(gPokenavLeftHeaderMainMenu_Gfx, gPokenavStructPtr->unk3B98);
        break;
    case 1:
        LZ77UnCompWram(gPokenavLeftHeaderCondition_Gfx, gPokenavStructPtr->unk4198);
        break;
    case 2:
        LZ77UnCompWram(gPokenavLeftHeaderRibbons_Gfx, gPokenavStructPtr->unk4D98);
        break;
    case 3:
        LZ77UnCompWram(gPokenavLeftHeaderHoennMap_Gfx, gPokenavStructPtr->unk5398);
        break;
    case 4:
        LZ77UnCompWram(gPokenavConditionMenuOptions_Gfx, gPokenavStructPtr->unk5F98);
        break;
    case 5:
        LZ77UnCompWram(gPokenavConditionMenuOptions2_Gfx, gPokenavStructPtr->unk6798);
        break;
    case 6:
        LZ77UnCompWram(gPokenavTrainersEyesHeader_Gfx, gPokenavStructPtr->unk4798);
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

void sub_80F27DC(u8 menuGfxId)
{
    struct SpriteSheet spriteSheet;

    switch (menuGfxId)
    {
    case 0:
        spriteSheet.data = gPokenavStructPtr->unk3B98;
        spriteSheet.size = sizeof(gPokenavStructPtr->unk3B98);
        spriteSheet.tag = 0x1;
        break;
    case 1:
        spriteSheet.data = gPokenavStructPtr->unk4198;
        spriteSheet.size = sizeof(gPokenavStructPtr->unk4198);
        spriteSheet.tag = 0x1;
        break;
    case 3:
        spriteSheet.data = gPokenavStructPtr->unk4798;
        spriteSheet.size = sizeof(gPokenavStructPtr->unk4798);
        spriteSheet.tag = 0x1;
        break;
    case 2:
        spriteSheet.data = gPokenavStructPtr->unk4D98;
        spriteSheet.size = sizeof(gPokenavStructPtr->unk4D98);
        spriteSheet.tag = 0x1;
        break;
    case 4:
        spriteSheet.data = gPokenavStructPtr->unk5398;
        spriteSheet.size = sizeof(gPokenavStructPtr->unk5398);
        spriteSheet.tag = 0x1;
        break;
    case 5:
        spriteSheet.data = gPokenavStructPtr->unk5F98;
        spriteSheet.size = sizeof(gPokenavStructPtr->unk5F98);
        spriteSheet.tag = 0x2;
        break;
    case 6:
        spriteSheet.data = gPokenavStructPtr->unk6198;
        spriteSheet.size = sizeof(gPokenavStructPtr->unk6198);
        spriteSheet.tag = 0x2;
        break;
    case 8:
        spriteSheet.data = gPokenavStructPtr->unk6398;
        spriteSheet.size = sizeof(gPokenavStructPtr->unk6398);
        spriteSheet.tag = 0x2;
        break;
    case 9:
        spriteSheet.data = gPokenavStructPtr->unk6598;
        spriteSheet.size = sizeof(gPokenavStructPtr->unk6598);
        spriteSheet.tag = 0x2;
        break;
    case 11:
        spriteSheet.data = gPokenavStructPtr->unk6998;
        spriteSheet.size = sizeof(gPokenavStructPtr->unk6998);
        spriteSheet.tag = 0x2;
        break;
    case 10:
        spriteSheet.data = gPokenavStructPtr->unk6798;
        spriteSheet.size = sizeof(gPokenavStructPtr->unk6798);
        spriteSheet.tag = 0x2;
        break;
    case 7:
        spriteSheet.data = gPokenavStructPtr->unk6B98;
        spriteSheet.size = sizeof(gPokenavStructPtr->unk6B98);
        spriteSheet.tag = 0x2;
        break;
    default:
        return;
    }

    LoadSpriteSheet(&spriteSheet);
}

void sub_80F29B8(u8 menuGfxId)
{
    u16 i;
    s16 deltaX, endX, initialX;
    u16 y;
    s16 width;
    struct Sprite **sprites;
    const struct SpriteTemplate *spriteTemplate;
    u8 spriteId;

    spriteTemplate = NULL;
    switch (menuGfxId)
    {
    case 0:
    case 1:
    case 2:
    case 3:
        initialX = -96;
        y = 49 - gPokenavStructPtr->unk030C;
        deltaX = 8;
        endX = 32;
        width = 64;
        sprites = gPokenavStructPtr->leftHeaderSprites;
        spriteTemplate = menuGfxId != 3 ? &gSpriteTemplate_83E44E0 : &gSpriteTemplate_83E44F8;
        break;
    case 4:
        initialX = 272;
        y = 49 - gPokenavStructPtr->unk030C;
        deltaX = -8;
        endX = 152;
        width = 64;
        spriteTemplate = &gSpriteTemplate_83E44E0;
        sprites = gPokenavStructPtr->leftHeaderSprites;
        break;
    case 5:
    case 6:
    case 8:
    case 9:
        spriteTemplate = &gSpriteTemplate_83E4530;
        // fall through
    case 7:
    case 10:
    case 11:
        if (spriteTemplate == NULL)
            spriteTemplate = &gSpriteTemplate_83E4548;

        initialX = -96;
        y = 68 - gPokenavStructPtr->unk030C;
        deltaX = 8;
        endX = 16;
        width = 32;
        sprites = gPokenavStructPtr->submenuLeftHeaderSprites;
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
            gSprites[spriteId].data[3] = menuGfxId;
            if (menuGfxId == 4 && i == 1)
            {
                int anim = !gPokenavStructPtr->regionMap.zoomed ? 1 : 2;
                StartSpriteAnim(&gSprites[spriteId], anim);
            }
            else
            {
                StartSpriteAnim(&gSprites[spriteId], i);
            }

            if (menuGfxId < 4 && i == 1)
            {
                gSprites[spriteId].oam.shape = ST_OAM_SQUARE;
                gSprites[spriteId].oam.size = 2;
            }

            sprites[i] = &gSprites[spriteId];
        }
    }
}

#define sEndX sprite->data[1]
void sub_80F2BBC(struct Sprite *sprite)
{
    s16 x = sprite->x;
    sprite->x += sprite->data[0];
    if ((x <= sEndX && sprite->x >= sEndX) || (x >= sEndX && sprite->x <= sEndX))
    {
        sprite->x = sEndX;
        if (sprite->data[3] == 4 && sprite->data[2] == 1)
            sprite->callback = sub_80F2C58;
        else
            sprite->callback = SpriteCallbackDummy;
    }
}
#undef sEndX

void sub_80F2C14(struct Sprite *sprite)
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

void sub_80F2C58(struct Sprite *sprite)
{
    int anim = !gPokenavStructPtr->regionMap.zoomed ? 1 : 2;
    StartSpriteAnim(sprite, anim);
}

void sub_80F2C80(u8 menuGfxId)
{
    gPokenavStructPtr->unk306 = 0;
    if (!gPokenavStructPtr->unk6DAC)
        while (sub_80F2CBC(menuGfxId));
}

bool8 sub_80F2CBC(u8 menuGfxId)
{
    switch (gPokenavStructPtr->unk306)
    {
    case 0:
        sub_80F27DC(menuGfxId);
        gPokenavStructPtr->unk306++;
        return TRUE;
    case 1:
        sub_80F29B8(menuGfxId);
        gPokenavStructPtr->unk306++;
        return FALSE;
    default:
        return FALSE;
    }
}

void sub_80F2D04(u8 menuGfxId)
{
    u16 i;

    if (menuGfxId < 5)
    {
        for (i = 0; i < 2; i++)
            gPokenavStructPtr->leftHeaderSprites[i]->callback = sub_80F2C14;
    }
    else
    {
        for (i = 0; i < 2; i++)
            gPokenavStructPtr->submenuLeftHeaderSprites[i]->callback = sub_80F2C14;
    }
}

void sub_80F2D6C(u8 menuGfxId)
{
    u16 i;

    if (menuGfxId < 5)
    {
        FreeSpriteTilesByTag(0x1);
        for (i = 0; i < 2; i++)
            DestroySprite(gPokenavStructPtr->leftHeaderSprites[i]);
    }
    else
    {
        FreeSpriteTilesByTag(0x2);
        for (i = 0; i < 2; i++)
            DestroySprite(gPokenavStructPtr->submenuLeftHeaderSprites[i]);
    }
}

void sub_80F2DD8(void)
{
    CreateRegionMapCursor(7, 7);
    CreateRegionMapPlayerIcon(8, 8);
    TrySetPlayerIconBlink();
}

void sub_80F2DF4(void)
{
    FreeRegionMapIconResources();
}

void MonPicGfxSpriteCallback(struct Sprite *sprite)
{
    sprite->x = gPokenavStructPtr->monTransitionX + 38;
}

void sub_80F2E18(u8 arg0)
{
    u8 spriteId;
    struct SpriteTemplate spriteTemplate;
    struct SpritePalette spritePalette;
    struct SpriteSheet spriteSheet;

    if (!gPokenavStructPtr->unk8768)
    {
        spriteSheet = gUnknown_083E4568;
        spriteTemplate = gSpriteTemplate_83E4570;
        spritePalette = gUnknown_083E4588;

        spriteSheet.data = gPokenavStructPtr->unkD1E4[arg0];
        spritePalette.data = gPokenavStructPtr->unk0[arg0];
        gPokenavStructPtr->unkD1DC = LoadSpritePalette(&spritePalette);
        gPokenavStructPtr->unkD1DE = LoadSpriteSheet(&spriteSheet);

        spriteId = CreateSprite(&spriteTemplate, 38, 104, 0);
        if (spriteId == MAX_SPRITES)
        {
            FreeSpriteTilesByTag(0x6);
            FreeSpritePaletteByTag(0x6);
            gPokenavStructPtr->unk8768 = NULL;
        }
        else
        {
            gPokenavStructPtr->unk8768 = &gSprites[spriteId];
            gPokenavStructPtr->unkD1E0 = (void *)(VRAM + 0x10000) + gPokenavStructPtr->unkD1DE * 32;
            gPokenavStructPtr->unkD1DC = gPokenavStructPtr->unkD1DC * 16 + 0x100;
        }
    }
    else
    {
        DmaCopy16Defvars(3, gPokenavStructPtr->unkD1E4[arg0], gPokenavStructPtr->unkD1E0, 0x800);
        LoadPalette(gPokenavStructPtr->unk0[arg0], gPokenavStructPtr->unkD1DC, 0x20);
    }
}

void sub_80F2F48(void)
{
    if (gPokenavStructPtr->unk8768)
    {
        DestroySprite(gPokenavStructPtr->unk8768);
        FreeSpriteTilesByTag(0x6);
        FreeSpritePaletteByTag(0x6);
        gPokenavStructPtr->unk8768 = NULL;
    }
}

void sub_80F2F7C(u8 arg0)
{
    sub_80F2E18(arg0);
    gPokenavStructPtr->unkCED0 = gPokenavStructPtr->unk8768;
    gPokenavStructPtr->unkCED0->callback = sub_80F2FEC;
}

void sub_80F2FB0(void)
{
    if (gPokenavStructPtr->unkCED0)
    {
        DestroySprite(gPokenavStructPtr->unkCED0);
        FreeSpriteTilesByTag(0x6);
        FreeSpritePaletteByTag(0x6);
        gPokenavStructPtr->unkCED0 = NULL;
        gPokenavStructPtr->unk8768 = NULL;
    }
}

void sub_80F2FEC(struct Sprite *sprite)
{
    sprite->x = gPokenavStructPtr->unkD15A + 40;
    sprite->y = 104;
}

void sub_80F3008(u8 arg0)
{
    u16 i;
    u8 spriteId;
    struct SpritePalette spritePalette;
    struct SpriteSheet spriteSheets[3];

    memcpy(spriteSheets, gUnknown_083E4590, sizeof(gUnknown_083E4590));
    spritePalette = gUnknown_083E45A8;
    switch (arg0)
    {
    case 1:
    case 2:
        spritePalette.data = gUnknown_08E9F988;
        break;
    }

    LoadSpriteSheets(spriteSheets);
    LoadSpritePalette(&spritePalette);
    spriteId = CreateSprite(&gSpriteTemplate_83E45B8, 95, 0, 0);
    if (spriteId == MAX_SPRITES)
    {
        gPokenavStructPtr->rightArrow = NULL;
    }
    else
    {
        gPokenavStructPtr->rightArrow = &gSprites[spriteId];
        for (i = 0; i < 2; i++)
        {
            spriteId = CreateSprite(&gSpriteTemplate_83E45F0, 168, i * 128 + 8, 0);
            if (spriteId != MAX_SPRITES)
            {
                gPokenavStructPtr->unk87D0[i] = &gSprites[spriteId];
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
                gPokenavStructPtr->unk87D0[i] = NULL;
            }
        }
    }
}

void sub_80F3130(void)
{
    u16 i;

    if (gPokenavStructPtr->rightArrow)
    {
        DestroySprite(gPokenavStructPtr->rightArrow);
        FreeSpriteTilesByTag(0x9);
        FreeSpritePaletteByTag(0x9);
        gPokenavStructPtr->rightArrow = NULL;
    }

    for (i = 0; i < 2; i++)
    {
        if (gPokenavStructPtr->unk87D0[i])
        {
            DestroySprite(gPokenavStructPtr->unk87D0[i]);
            gPokenavStructPtr->unk87D0[i] = NULL;
        }
    }

    FreeSpriteTilesByTag(0xA);
}

void SpriteCB_RightArrow(struct Sprite *sprite)
{
    sprite->y = gPokenavStructPtr->selectedIndexOffset * 16 + 16;
}

void sub_80F31AC(struct Sprite *sprite)
{
    if (gPokenavStructPtr->unk87C9)
    {
        if (sprite->data[4])
        {
            if (!sprite->data[3])
                sprite->invisible = gPokenavStructPtr->windowTopIndex == 0;
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

void sub_80F3264(void)
{
    u16 i;

    for (i = 0; i < 2; i++)
    {
        if (gPokenavStructPtr->unk87D0[i])
            gPokenavStructPtr->unk87D0[i]->data[4] = 1;
    }
}

void sub_80F3294(u8 arg0)
{
    gPokenavStructPtr->rightArrow->invisible = arg0;
    if (gPokenavStructPtr->unk87C9)
    {
        if (arg0 == 1)
        {
            gPokenavStructPtr->unk87D0[0]->invisible = arg0;
            gPokenavStructPtr->unk87D0[1]->invisible = arg0;
        }
        else
        {
            gPokenavStructPtr->unk87D0[0]->data[4] = 1;
            gPokenavStructPtr->unk87D0[1]->data[4] = 1;
        }
    }
}

void SpriteCB_PartyPokeball(struct Sprite *sprite)
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

void sub_80F33A8(void)
{
    u16 i;
    u8 spriteId;
    struct SpriteSheet spriteSheets[4];
    struct SpritePalette spritePalettes[3];
    struct SpriteTemplate spriteTemplate;

    memcpy(spriteSheets, gUnknown_083E4628, sizeof(gUnknown_083E4628));
    memcpy(spritePalettes, gUnknown_083E4648, sizeof(gUnknown_083E4648));
    spriteTemplate = gSpriteTemplate_83E4660;
    LoadSpriteSheets(spriteSheets);
    LoadSpritePalettes(spritePalettes);

    for (i = 0; i < gPokenavStructPtr->unk87DA - 1; i++)
    {
        spriteId = CreateSprite(&spriteTemplate, 226, i * 20 + 8, 0);
        if (spriteId != MAX_SPRITES)
        {
            gPokenavStructPtr->unk87E4[i] = &gSprites[spriteId];
            gPokenavStructPtr->unk87E4[i]->data[0] = i;
        }
        else
        {
            gPokenavStructPtr->unk87E4[i] = NULL;
        }
    }

    spriteTemplate.tileTag = 0x4;
    spriteTemplate.callback = SpriteCallbackDummy;
    for (; i < 6; i++)
    {
        spriteId = CreateSprite(&spriteTemplate, 230, i * 20 + 8, 0);
        if (spriteId != MAX_SPRITES)
        {
            gPokenavStructPtr->unk87E4[i] = &gSprites[spriteId];
            gPokenavStructPtr->unk87E4[i]->oam.size = 0;
        }
        else
        {
            gPokenavStructPtr->unk87E4[i] = NULL;
        }
    }

    spriteTemplate.tileTag = 0x5;
    spriteTemplate.callback = HighlightCurrentPartyIndexPokeball;
    spriteId = CreateSprite(&spriteTemplate, 222, i * 20 + 8, 0);
    if (spriteId != MAX_SPRITES)
    {
        gPokenavStructPtr->unk87E4[i] = &gSprites[spriteId];
        gPokenavStructPtr->unk87E4[i]->oam.shape = ST_OAM_H_RECTANGLE;
        gPokenavStructPtr->unk87E4[i]->oam.size = 2;
    }
    else
    {
        gPokenavStructPtr->unk87E4[i] = NULL;
    }
}

void sub_80F357C(void)
{
    u16 i;

    for (i = 0; i < 7; i++)
    {
        if (gPokenavStructPtr->unk87E4[i])
        {
            DestroySprite(gPokenavStructPtr->unk87E4[i]);
            gPokenavStructPtr->unk87E4[i] = NULL;
        }
    }
}

void sub_80F35B4(void)
{
    struct Sprite *sprite;

    gPokenavStructPtr->marksMenu.baseTileTag = 0x1C;
    gPokenavStructPtr->marksMenu.basePaletteTag = 0x13;
    InitMonMarkingsMenu(&gPokenavStructPtr->marksMenu);
    BufferMonMarkingsMenuTiles();
    sprite = CreateMonMarkingAllCombosSprite(27, 21, gMonMarkings_Pal);
    sprite->oam.priority = 3;
    sprite->x = 192;
    sprite->y = 32;
    sprite->callback = MonMarkingsCallback;
    gPokenavStructPtr->monMarksSprite = sprite;
}

void sub_80F3614(void)
{
    DestroySprite(gPokenavStructPtr->monMarksSprite);
    FreeSpriteTilesByTag(0x1B);
    FreeSpritePaletteByTag(0x15);
}

void MonMarkingsCallback(struct Sprite *sprite)
{
    StartSpriteAnim(sprite, gPokenavStructPtr->monMarks[gPokenavStructPtr->loadId]);
}

void sub_80F3668(void)
{
    OpenMonMarkingsMenu(gPokenavStructPtr->monMarks[gPokenavStructPtr->loadId], 176, 32);
}

void sub_80F3698(void)
{
    struct UnkUsePokeblockSub *var0 = &gPokenavStructPtr->unk893c[gPokenavStructPtr->unk87DC];
    gPokenavStructPtr->monMarks[gPokenavStructPtr->loadId] = gPokenavStructPtr->marksMenu.markings;
    SetMonMarkings(var0->boxId, var0->monId, gPokenavStructPtr->marksMenu.markings);
    FreeMonMarkingsMenu();
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
        LZ77UnCompWram(gRibbonIconsBig_Gfx, gPokenavStructPtr->unk984C);
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
        gPokenavStructPtr->bigRibbonSprite = NULL;
        gPokenavStructPtr->unkBC92 = 0;
        break;
    case 1:
        sub_80F2E18(0);
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
    u8 ribbonId;
    u8 spriteId;

    if (arg1 < 3)
        var0 = arg0 + arg1 * 9;
    else
        var0 = arg0 + (gPokenavStructPtr->unkBC8E - gPokenavStructPtr->numGiftRibbons);

    ribbonId = gPokenavStructPtr->unkBC4C[var0];
    spriteTemplate = gSpriteTemplate_RibbonIconBig;
    spriteTemplate.tileTag = gRibbonGfxData[ribbonId][0] + 11;
    spriteTemplate.paletteTag = gRibbonGfxData[ribbonId][1] + 10;
    spriteId = CreateSprite(&spriteTemplate, arg0 * 16 + 96, arg1 * 16 + 40, 2);
    if (spriteId != MAX_SPRITES)
        return &gSprites[spriteId];
    else
        return NULL;
}

void sub_80F3B00(void)
{
    gPokenavStructPtr->bigRibbonSprite = sub_80F3A3C(gPokenavStructPtr->unkBC90, gPokenavStructPtr->unkBC91);
    if (gPokenavStructPtr->bigRibbonSprite)
    {
        StartSpriteAffineAnim(gPokenavStructPtr->bigRibbonSprite, 1);
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
        gPokenavStructPtr->unkBC92 = !gPokenavStructPtr->bigRibbonSprite->affineAnimEnded;
        return gPokenavStructPtr->unkBC92;
    }
    else
    {
        return FALSE;
    }
}

void sub_80F3B94(void)
{
    if (gPokenavStructPtr->bigRibbonSprite)
    {
        StartSpriteAffineAnim(gPokenavStructPtr->bigRibbonSprite, 2);
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
        gPokenavStructPtr->unkBC92 = !gPokenavStructPtr->bigRibbonSprite->affineAnimEnded;
        if (!gPokenavStructPtr->unkBC92)
        {
            FreeOamMatrix(gPokenavStructPtr->bigRibbonSprite->oam.matrixNum);
            DestroySprite(gPokenavStructPtr->bigRibbonSprite);
            gPokenavStructPtr->bigRibbonSprite = NULL;
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

    if (gPokenavStructPtr->bigRibbonSprite)
    {
        FreeOamMatrix(gPokenavStructPtr->bigRibbonSprite->oam.matrixNum);
        DestroySprite(gPokenavStructPtr->bigRibbonSprite);
        gPokenavStructPtr->bigRibbonSprite = NULL;
    }

    for (i = 0; i < 12; i++)
        FreeSpriteTilesByTag(i + 0xB);

    for (i = 0; i < 5; i++)
        FreeSpritePaletteByTag(i + 0xA);

    sub_80F2F48();
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
        gPokenavStructPtr->conditionSparkleSprites[i] = NULL;
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
    struct UnkUsePokeblockSub *var0 = &gPokenavStructPtr->unk893c[gPokenavStructPtr->unk87DC];

    if (!var0->unk3_14)
        return;

    count = gPokenavStructPtr->numSparkles[gPokenavStructPtr->loadId];
    for (i = 0; i < count + 1; i++)
    {
        spriteId = CreateSprite(&gSpriteTemplate_ConditionSparkle, 0, 0, 0);
        if (spriteId != MAX_SPRITES)
        {
            gPokenavStructPtr->conditionSparkleSprites[i] = &gSprites[spriteId];
            gPokenavStructPtr->conditionSparkleSprites[i]->invisible = TRUE;
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
        if (!gPokenavStructPtr->conditionSparkleSprites[i])
            return;

        DestroySprite(gPokenavStructPtr->conditionSparkleSprites[i]);
        gPokenavStructPtr->conditionSparkleSprites[i] = NULL;
    }
}

void SpriteCB_ConditionSparkle_DoNextAfterDelay(struct Sprite *sprite)
{
    if (++sprite->data[1] > 60)
    {
        sprite->data[1] = 0;
        InitConditionSparkles(sprite->data[2], 0);
    }
}

void SpriteCB_ConditionSparkle_WaitForAllAnim(struct Sprite *sprite)
{
    if (sprite->animEnded)
    {
        sprite->data[1] = 0;
        sprite->callback = SpriteCB_ConditionSparkle_DoNextAfterDelay;
    }
}

void SetConditionSparklePosition(struct Sprite *sprite)
{
    if (gPokenavStructPtr->unk8768)
    {
        sprite->x = gPokenavStructPtr->unk8768->x
                         + gPokenavStructPtr->unk8768->x2
                         + gConditionSparkleCoords[sprite->data[0]][0];
        sprite->y = gPokenavStructPtr->unk8768->y
                         + gPokenavStructPtr->unk8768->y2
                         + gConditionSparkleCoords[sprite->data[0]][1];
    }
    else
    {
        sprite->x = gConditionSparkleCoords[sprite->data[0]][0] + 40;
        sprite->y = gConditionSparkleCoords[sprite->data[0]][1] + 104;
    }
}

void SpriteCB_ConditionSparkle(struct Sprite *sprite)
{
    if (sprite->data[1])
    {
        if (--sprite->data[1])
            return;

        SeekSpriteAnim(sprite, 0);
        sprite->invisible = FALSE;
    }

    SetConditionSparklePosition(sprite);
    if (sprite->animEnded)
    {
        sprite->invisible = TRUE;
        if (sprite->data[3] == sprite->data[2])
        {
            if (sprite->data[3] == 9)
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

void InitConditionSparkles(u8 count, bool8 allowFirstShowAll)
{
    u16 i;

    for (i = 0; i < 10; i++)
    {
        if (gPokenavStructPtr->conditionSparkleSprites[i])
        {
            gPokenavStructPtr->conditionSparkleSprites[i]->data[0] = i;
            gPokenavStructPtr->conditionSparkleSprites[i]->data[1] = i * 16 + 1;
            gPokenavStructPtr->conditionSparkleSprites[i]->data[2] = count;
            gPokenavStructPtr->conditionSparkleSprites[i]->data[3] = i;

            if (!allowFirstShowAll || count != 9)
            {
                gPokenavStructPtr->conditionSparkleSprites[i]->callback = SpriteCB_ConditionSparkle;
            }
            else
            {
                SetConditionSparklePosition(gPokenavStructPtr->conditionSparkleSprites[i]);
                ShowAllConditionSparkles();
                gPokenavStructPtr->conditionSparkleSprites[i]->callback = SpriteCB_ConditionSparkle_WaitForAllAnim;
                gPokenavStructPtr->conditionSparkleSprites[i]->invisible = FALSE;
            }
        }
    }
}

void ShowAllConditionSparkles(void)
{
    u16 i;

    for (i = 0; i < 10; i++)
    {
        if (gPokenavStructPtr->conditionSparkleSprites[i])
        {
            SeekSpriteAnim(gPokenavStructPtr->conditionSparkleSprites[i], 0);
            gPokenavStructPtr->conditionSparkleSprites[i]->invisible = FALSE;
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
        LZ77UnCompWram(gSpinningPokenav_Gfx, gPokenavStructPtr->unk131E4);
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
        spriteId = CreateSprite(&gSpinningPokenavSpriteTemplate, 218, 14, 0);
        if (spriteId != MAX_SPRITES)
        {
            gPokenavStructPtr->spinningPokenav = &gSprites[spriteId];
            gPokenavStructPtr->spinningPokenav->data[0] = 0;
        }
        else
        {
            gPokenavStructPtr->spinningPokenav = NULL;
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
    sprite->y2 = -gPokenavStructPtr->unk030C;
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
        .data = gPokenavStructPtr->unkD1E4[0],
        .size = 0x500,
        .tag = 0x1A,
    };

    sub_80F4194(gPokenavStructPtr->unkD1E4[0], arg0);
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

    sub_80F4194(gPokenavStructPtr->unkD1E4[0], arg0);
    tile = GetSpriteTileStartByTag(0x1A);
    if (tile != 0xFFFF)
    DmaCopy32Defvars(3, gPokenavStructPtr->unkD1E4[0], (void *)(VRAM + 0x10000 + (tile * 32)), 0x500);
}

u8 *sub_80F4428(u8 *str, u16 listId, u8 arg2)
{
    return sub_80F6514(str, listId, arg2);
}

u8 *sub_80F443C(u8 *dst, u16 num)
{
    return AlignInt1InMenuWindow(StringCopy(dst, gText_Number2), num, 56, 1);
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

u32 GetBoxOrPartyMonData(u16 boxId, u16 monId, int request, u8 *dst)
{
    if (boxId == 14)
    {
        if (request == MON_DATA_NICKNAME || request == MON_DATA_OT_NAME)
            return GetMonData(&gPlayerParty[monId], request, dst);
        else
            return GetMonData(&gPlayerParty[monId], request);
    }
    else
    {
        if (request == MON_DATA_NICKNAME || request == MON_DATA_OT_NAME)
            return GetBoxMonData(&gPokemonStorage.boxes[boxId][monId], request, dst);
        else
            return GetBoxMonData(&gPokemonStorage.boxes[boxId][monId], request);
    }
}

void SetMonMarkings(u16 box, u16 monIndex, u8 markings)
{
    if (box == 14)
        SetMonData(&gPlayerParty[monIndex], MON_DATA_MARKINGS, &markings);
    else
        SetBoxMonData(&gPokemonStorage.boxes[box][monIndex], MON_DATA_MARKINGS, &markings);
}

void sub_80F45A0(s16 listId, u8 loadId)
{
    u8 boxId;
    u8 var0 = gPokenavStructPtr->unk893c[listId].unk3_14;
    if (var0)
    {
        sub_80F4428(gPokenavStructPtr->nameText[loadId], listId, 0);
        boxId = gPokenavStructPtr->unk893c[listId].boxId;
        if (boxId == 14)
            AlignStringInMenuWindow(gPokenavStructPtr->locationText[loadId], gText_InParty, 64, 0);
        else
            AlignStringInMenuWindow(gPokenavStructPtr->locationText[loadId], gPokemonStorage.boxNames[boxId], 64, 0);

        gPokenavStructPtr->unk8937[loadId] = 1;
    }
    else
    {
        AlignStringInMenuWindow(gPokenavStructPtr->nameText[loadId], gEmptyString_81E72B0, 104, 0);
        AlignStringInMenuWindow(gPokenavStructPtr->locationText[loadId], gEmptyString_81E72B0, 64, 0);
        gPokenavStructPtr->unk8937[loadId] = var0;
    }
}

void sub_80F468C(s16 listId, u8 loadId)
{
    u16 i;
    u16 boxId;
    u16 monId;

    if (gPokenavStructPtr->unk893c[listId].unk3_14)
    {
        boxId = gPokenavStructPtr->unk893c[listId].boxId;
        monId = gPokenavStructPtr->unk893c[listId].monId;
        gPokenavStructPtr->conditions[loadId][0] = GetBoxOrPartyMonData(boxId, monId, MON_DATA_COOL, NULL);
        gPokenavStructPtr->conditions[loadId][1] = GetBoxOrPartyMonData(boxId, monId, MON_DATA_TOUGH, NULL);
        gPokenavStructPtr->conditions[loadId][2] = GetBoxOrPartyMonData(boxId, monId, MON_DATA_SMART, NULL);
        gPokenavStructPtr->conditions[loadId][3] = GetBoxOrPartyMonData(boxId, monId, MON_DATA_CUTE, NULL);
        gPokenavStructPtr->conditions[loadId][4] = GetBoxOrPartyMonData(boxId, monId, MON_DATA_BEAUTY, NULL);

        gPokenavStructPtr->numSparkles[loadId] = GetBoxOrPartyMonData(boxId, monId, MON_DATA_SHEEN, NULL) != 255
                                           ? GetBoxOrPartyMonData(boxId, monId, MON_DATA_SHEEN, NULL) / 29
                                           : 9;

        gPokenavStructPtr->monMarks[loadId] = GetBoxOrPartyMonData(boxId, monId, MON_DATA_MARKINGS, NULL);
        ConditionGraph_CalcPositions(gPokenavStructPtr->conditions[loadId], gPokenavStructPtr->savedPositions[loadId]);
    }
    else
    {
        for (i = 0; i < 5; i++)
        {
            gPokenavStructPtr->conditions[loadId][i] = 0;
            gPokenavStructPtr->savedPositions[loadId][i].x = 0x9B;
            gPokenavStructPtr->savedPositions[loadId][i].y = 0x5B;
        }
    }
}

void sub_80F4824(s16 listId, u8 loadId)
{
    u16 species;
    u32 otId;
    u32 personality;
    u16 boxId;
    u16 monId;

    if (gPokenavStructPtr->unk893c[listId].unk3_14)
    {
        boxId = gPokenavStructPtr->unk893c[listId].boxId;
        monId = gPokenavStructPtr->unk893c[listId].monId;
        species = GetBoxOrPartyMonData(boxId, monId, MON_DATA_SPECIES_OR_EGG, NULL);
        otId = GetBoxOrPartyMonData(boxId, monId, MON_DATA_OT_ID, NULL);
        personality = GetBoxOrPartyMonData(boxId, monId, MON_DATA_PERSONALITY, NULL);

        HandleLoadSpecialPokePic(
            &gMonFrontPicTable[species],
            gMonFrontPicCoords[species].size,
            1,
            gPokenavStructPtr->unk131E4,
            gPokenavStructPtr->unkD1E4[loadId],
            species,
            personality);

        LZ77UnCompWram(GetMonSpritePalFromSpeciesAndPersonality(species, otId, personality), gPokenavStructPtr->unk0[loadId]);
        gPokenavStructPtr->unkD1D6[loadId] = species;
    }
}

void sub_80F4900(s16 listId, u8 loadId)
{
    sub_80F45A0(listId, loadId);
    sub_80F468C(listId, loadId);
    sub_80F4824(listId, loadId);
}

void sub_80F492C(void)
{
    gPokenavStructPtr->unk8FE4 = 0;
}

void InsertMonListItem(struct UnkUsePokeblockSub *item)
{
    u16 left, right, insertionIdx;

    left = 0;
    right = gPokenavStructPtr->unk8FE4;
    insertionIdx = left + (right - left)/ 2;
    while (right != insertionIdx)
    {
        if (item->unk0 > gPokenavStructPtr->unk893c[insertionIdx].unk0)
            right = insertionIdx;
        else
            left = insertionIdx + 1;

        insertionIdx = left + (right - left) / 2;
    }

    for ( right = gPokenavStructPtr->unk8FE4; right > insertionIdx; right--)
    {
        gPokenavStructPtr->unk893c[right] = gPokenavStructPtr->unk893c[right - 1];
    }

    gPokenavStructPtr->unk893c[insertionIdx] = *item;
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

    gPokenavStructPtr->selectedIndexOffset = 0;
    gPokenavStructPtr->windowTopIndex = 0;
    gPokenavStructPtr->unk876E = 0;
    gPokenavStructPtr->unk8772 = gPokenavStructPtr->unk8FE4 < 9 ? (gPokenavStructPtr->unk8FE4 - 1) : 7;
    gPokenavStructPtr->unk8774 = gPokenavStructPtr->unk8FE4 - 1;
    gPokenavStructPtr->unk87C9 = gPokenavStructPtr->unk8774 > 7;
}

void sub_80F4B20(void)
{
    s16 var0;
    s16 var1;

    sub_80F4900(gPokenavStructPtr->unk87DC, 0);
    sub_80F2E18(0);
    if (gPokenavStructPtr->unk87DA == 1)
    {
        gPokenavStructPtr->loadId = 0;
        gPokenavStructPtr->nextLoadIdDown = 0;
        gPokenavStructPtr->nextLoadIdUp = 0;
    }
    else
    {
        gPokenavStructPtr->loadId = 0;
        gPokenavStructPtr->nextLoadIdDown = 1;
        gPokenavStructPtr->nextLoadIdUp = 2;

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
            gPokenavStructPtr->unk893c[j].boxId = 14;
            gPokenavStructPtr->unk893c[j].monId = i;
            gPokenavStructPtr->unk893c[j].unk2_5 = j + 1;
            gPokenavStructPtr->unk893c[j].unk3_14 = 1;
            j++;
        }
    }

    gPokenavStructPtr->unk893c[j].boxId = 0;
    gPokenavStructPtr->unk893c[j].monId = 0;
    gPokenavStructPtr->unk893c[j].unk2_5 = 0;
    gPokenavStructPtr->unk893c[j].unk3_14 = 0;
    gPokenavStructPtr->unk87DC = 0;
    gPokenavStructPtr->unk87DA = j + 1;
    sub_80F4B20();
    gPokenavStructPtr->unk87CB = 1;
}

void sub_80F4CF0(void)
{
    gPokenavStructPtr->unk87DC = gPokenavStructPtr->unk876E;
    sub_80F4B20();

    if (gPokenavStructPtr->unk8774 == 0)
        gPokenavStructPtr->unk87CB = 0;
    else
        gPokenavStructPtr->unk87CB = 1;
}

void sub_80F4D44(void)
{
    gPokenavStructPtr->unk8FE6 = 0;
    gPokenavStructPtr->monId = 0;
    sub_80F492C();

    if (!gPokenavStructPtr->unk6DAC)
        while (sub_80F4D88());
}

bool8 sub_80F4D88(void)
{
    u16 i;
    register int mask asm("r3"); // FIXME
    int nextValue;
    struct UnkUsePokeblockSub item;

    switch (gPokenavStructPtr->unk8FE6)
    {
    default:
        item.unk3_14 = 1;
        for (i = 0; i < 15; i++)
        {

            if (GetBoxMonData(&gPokemonStorage.boxes[gPokenavStructPtr->unk8FE6][gPokenavStructPtr->monId], MON_DATA_SPECIES)
                && !GetBoxMonData(&gPokemonStorage.boxes[gPokenavStructPtr->unk8FE6][gPokenavStructPtr->monId], MON_DATA_IS_EGG))
            {
                item.boxId = gPokenavStructPtr->unk8FE6;
                item.monId = gPokenavStructPtr->monId;
                item.unk0 = GetBoxMonData(
                    &gPokemonStorage.boxes[gPokenavStructPtr->unk8FE6][gPokenavStructPtr->monId],
                    gPokenavStructPtr->conditionDataId);
                InsertMonListItem(&item);
            }

            gPokenavStructPtr->monId++;
            mask = 0xFF;
            if (gPokenavStructPtr->monId == 30)
            {
                gPokenavStructPtr->monId = 0;
                nextValue = gPokenavStructPtr->unk8FE6 + 1;
                gPokenavStructPtr->unk8FE6 = nextValue;
                if ((nextValue & mask) == 14)
                    break;
            }
        }
        break;
    case 14:
        item.unk3_14 = 1;
        item.boxId = 14;
        for (i = 0; i < gPokenavStructPtr->unk8828; i++)
        {
            if (!GetMonData(&gPlayerParty[i], MON_DATA_IS_EGG))
            {
                item.monId = i;
                item.unk0 = GetMonData(&gPlayerParty[i], gPokenavStructPtr->conditionDataId);
                InsertMonListItem(&item);
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

void DoConditionGraphEnterTransition(void)
{
    ConditionGraph_SetNewPositions(gPokenavStructPtr->savedPositions[3], gPokenavStructPtr->savedPositions[gPokenavStructPtr->loadId]);
    ConditionGraph_TryUpdate();
}

bool8 ConditionMenu_UpdateMonEnter(void)
{
    bool8 graphUpdating = ConditionGraph_TryUpdate();
    bool8 monUpdating = MoveConditionMonOnscreen();
    return graphUpdating || monUpdating;
}

void DoConditionGraphExitTransition(void)
{
    if (gPokenavStructPtr->inSearchMode || gPokenavStructPtr->unk87DC != gPokenavStructPtr->unk8828)
        ConditionGraph_SetNewPositions(gPokenavStructPtr->savedPositions[gPokenavStructPtr->loadId], gPokenavStructPtr->savedPositions[3]);
}

bool8 ConditionMenu_UpdateMonExit(void)
{
    bool8 graphUpdating = ConditionGraph_TryUpdate();
    bool8 monUpdating = MoveConditionMonOffscreen();
    return graphUpdating || monUpdating;
}

void sub_80F5060(bool8 moveUp)
{
    u16 newLoadId;
    bool8 wasNotLastMon;
    bool8 isNotLastMon;

    if (moveUp)
        newLoadId = gPokenavStructPtr->nextLoadIdUp;
    else
        newLoadId = gPokenavStructPtr->nextLoadIdDown;

    ConditionGraph_SetNewPositions(gPokenavStructPtr->savedPositions[gPokenavStructPtr->loadId], gPokenavStructPtr->savedPositions[newLoadId]);
    wasNotLastMon = gPokenavStructPtr->unk893c[gPokenavStructPtr->unk87DC].unk3_14;
    if (moveUp)
    {
        gPokenavStructPtr->nextLoadIdUp = gPokenavStructPtr->nextLoadIdDown;
        gPokenavStructPtr->nextLoadIdDown = gPokenavStructPtr->loadId;
        gPokenavStructPtr->loadId = newLoadId;
        gPokenavStructPtr->toLoadId = gPokenavStructPtr->nextLoadIdUp;

        gPokenavStructPtr->unk87DC = gPokenavStructPtr->unk87DC
                                     ? gPokenavStructPtr->unk87DC - 1
                                     : gPokenavStructPtr->unk87DA - 1;
        gPokenavStructPtr->toLoadListIndex = gPokenavStructPtr->unk87DC
                                     ? gPokenavStructPtr->unk87DC - 1
                                     : gPokenavStructPtr->unk87DA - 1;
    }
    else
    {
        gPokenavStructPtr->nextLoadIdDown = gPokenavStructPtr->nextLoadIdUp;
        gPokenavStructPtr->nextLoadIdUp = gPokenavStructPtr->loadId;
        gPokenavStructPtr->loadId = newLoadId;
        gPokenavStructPtr->toLoadId = gPokenavStructPtr->nextLoadIdDown;

        gPokenavStructPtr->unk87DC = (gPokenavStructPtr->unk87DC < gPokenavStructPtr->unk87DA - 1)
                                     ? gPokenavStructPtr->unk87DC + 1
                                     : 0;
        gPokenavStructPtr->toLoadListIndex = (gPokenavStructPtr->unk87DC < gPokenavStructPtr->unk87DA - 1)
                                     ? gPokenavStructPtr->unk87DC + 1
                                     : 0;
    }

    isNotLastMon = gPokenavStructPtr->unk893c[gPokenavStructPtr->unk87DC].unk3_14;
    if (!wasNotLastMon)
        gPokenavStructPtr->unk87E0 = sub_80F5264;
    else if (!isNotLastMon)
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
        sub_80F2E18(gPokenavStructPtr->loadId);
        sub_80F01E0(gPokenavStructPtr->loadId);
        gPokenavStructPtr->unk87DE++;
        // fall through
    case 1:
        if (!ConditionMenu_UpdateMonEnter())
        {
            sub_80F4900(gPokenavStructPtr->toLoadListIndex, gPokenavStructPtr->toLoadId);
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
        if (!ConditionMenu_UpdateMonExit())
        {
            sub_80F01E0(gPokenavStructPtr->loadId);
            sub_80F4900(gPokenavStructPtr->toLoadListIndex, gPokenavStructPtr->toLoadId);
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
        ConditionGraph_TryUpdate();
        if (!MoveConditionMonOffscreen())
        {
            sub_80F2E18(gPokenavStructPtr->loadId);
            sub_80F01E0(gPokenavStructPtr->loadId);
            gPokenavStructPtr->unk87DE++;
        }
        break;
    case 1:
        if (!ConditionMenu_UpdateMonEnter())
            gPokenavStructPtr->unk87DE++;
        break;
    case 2:
        sub_80F4900(gPokenavStructPtr->toLoadListIndex, gPokenavStructPtr->toLoadId);
        return FALSE;
    }

    return TRUE;
}

void ConditionGraph_SetNewPositions(struct UCoords16 *old, struct UCoords16 *new)
{
    u16 i, j;
    int coord;
    int increment;

    for (i = 0; i < 5; i++)
    {
        coord = old[i].x << 8;
        increment = ((new[i].x - old[i].x) << 8) / 10;
        for (j = 0; j < 9; j++)
        {
            gPokenavStructPtr->newPositions[j][i].x = (coord >> 8) + ((coord >> 7) & 1);
            coord += increment;
        }

        gPokenavStructPtr->newPositions[j][i].x = new[i].x;
        coord = old[i].y << 8;
        increment = ((new[i].y - old[i].y) << 8) / 10;
        for (j = 0; j < 9; j++)
        {
            gPokenavStructPtr->newPositions[j][i].y = (coord >> 8) + ((coord >> 7) & 1);
            coord += increment;
        }

        gPokenavStructPtr->newPositions[j][i].y = new[i].y;
    }

    gPokenavStructPtr->updateCounter = 0;
}

bool8 ConditionGraph_TryUpdate(void)
{
    if (gPokenavStructPtr->updateCounter < 10)
    {
        sub_80F556C(gPokenavStructPtr->newPositions[gPokenavStructPtr->updateCounter++]);
        return gPokenavStructPtr->updateCounter != 10;
    }
    else
    {
        return FALSE;
    }
}

void sub_80F5550(struct UCoords16 *old, struct UCoords16 *new)
{
    ConditionGraph_SetNewPositions(old, new);
}

bool8 sub_80F555C(void)
{
    return ConditionGraph_TryUpdate();
}

void sub_80F556C(struct UCoords16 *arg0)
{
    u16 i;

    for (i = 0; i < 5; i++)
        gPokenavStructPtr->curPositions[i] = arg0[i];

    gPokenavStructPtr->needsDraw = 1;
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

void sub_80F567C(u8 *conditions, struct UCoords16 positions[])
{
    ConditionGraph_CalcPositions(conditions, positions);
}

static void ConditionGraph_CalcLine(u16 * scanline, struct UCoords16 * pos1, struct UCoords16 * pos2, bool8 dir, u16 * overflowScanline)
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

        gPokenavStructPtr->bottom = top + i;
        scanline += (gPokenavStructPtr->bottom - 56) * 2;
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

        gPokenavStructPtr->bottom = top + i;
        overflowScanline += (gPokenavStructPtr->bottom - 56) * 2;
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
        gPokenavStructPtr->bottom = top;
        scanline += (top - 56) * 2;
        overflowScanline += (top - 56) * 2;
        scanline[1] = pos1->x + 1;
        overflowScanline[0] = pos2->x;
        overflowScanline[1] = 155;
        return;
    }

    ptr[dir] = dir + x2;
}

void ConditionGraph_CalcRightHalf(struct UCoords16 * curPositions)
{
    u16 i, y, bottom;

    if (curPositions[0].y < curPositions[1].y)
    {
        y = curPositions[0].y;
        ConditionGraph_CalcLine((u16 *)gPokenavStructPtr->scanlineRight, &curPositions[0], &curPositions[1], 1, NULL);
    }
    else
    {
        y = curPositions[1].y;
        ConditionGraph_CalcLine((u16 *)gPokenavStructPtr->scanlineRight, &curPositions[1], &curPositions[0], 0, NULL);
    }
    ConditionGraph_CalcLine((u16 *)gPokenavStructPtr->scanlineRight, &curPositions[1], &curPositions[2], 1, NULL);

    i = curPositions[2].y <= curPositions[3].y;
    ConditionGraph_CalcLine((u16 *)gPokenavStructPtr->scanlineRight, &curPositions[2], &curPositions[3], i, (u16 *)gPokenavStructPtr->scanlineLeft);
    for (i = 56; i < y; i++)
    {
        gPokenavStructPtr->scanlineRight[i - 56][0] = 0;
        gPokenavStructPtr->scanlineRight[i - 56][1] = 0;
    }

    for (i = curPositions[0].y; i <= gPokenavStructPtr->bottom; i++)
        gPokenavStructPtr->scanlineRight[i - 56][0] = 155;

    bottom = max(gPokenavStructPtr->bottom, curPositions[2].y);
    for (i = bottom + 1; i < 122; i++)
    {
        gPokenavStructPtr->scanlineRight[i - 56][0] = 0;
        gPokenavStructPtr->scanlineRight[i - 56][1] = 0;
    }

//    for (i = 56; i < 122; i++)
//    {
//        if (gPokenavStructPtr->scanlineRight[i - 56][0] == 0 && gPokenavStructPtr->scanlineRight[i - 56][1] != 0)
//            gPokenavStructPtr->scanlineRight[i - 56][0] = 155;
//    }
}

void ConditionGraph_CalcLeftHalf(struct UCoords16 *curPositions)
{
    u16 i, y, bottom;

    if (curPositions[0].y < curPositions[4].y)
    {
        y = curPositions[0].y;
        ConditionGraph_CalcLine((u16 *)gPokenavStructPtr->scanlineLeft, &curPositions[0], &curPositions[4], 0, NULL);
    }
    else
    {
        y = curPositions[4].y;
        ConditionGraph_CalcLine((u16 *)gPokenavStructPtr->scanlineLeft, &curPositions[4], &curPositions[0], 1, NULL);
    }

    ConditionGraph_CalcLine((u16 *)gPokenavStructPtr->scanlineLeft, &curPositions[4], &curPositions[3], 0, NULL);

    for (i = 56; i < y; i++)
    {
        gPokenavStructPtr->scanlineLeft[i - 56][0] = 0;
        gPokenavStructPtr->scanlineLeft[i - 56][1] = 0;
    }

    for (i = curPositions[0].y; i <= gPokenavStructPtr->bottom; i++)
        gPokenavStructPtr->scanlineLeft[i - 56][1] = 155;

    bottom = max(gPokenavStructPtr->bottom, curPositions[3].y + 1);
    for (i = bottom; i < 122; i++)
    {
        gPokenavStructPtr->scanlineLeft[i - 56][0] = 0;
        gPokenavStructPtr->scanlineLeft[i - 56][1] = 0;
    }

//    for (i = 0; i < 66; i++)
//    {
//        if (gPokenavStructPtr->scanlineLeft[i][0] >= gPokenavStructPtr->scanlineLeft[i][1])
//        {
//            gPokenavStructPtr->scanlineLeft[i][1] = 0;
//            gPokenavStructPtr->scanlineLeft[i][0] = 0;
//        }
//    }
}

void ConditionGraph_InitResetScanline(void)
{
    gPokenavStructPtr->scanlineResetState = 0;
}

extern const struct ScanlineEffectParams gConditionGraphScanline;

bool8 ConditionGraph_ResetScanline(void)
{
    s32 i;
    struct ScanlineEffectParams params;

    switch (gPokenavStructPtr->scanlineResetState)
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
        gPokenavStructPtr->scanlineResetState++;
        return TRUE;
    case 1:
        params = gConditionGraphScanline;
        ScanlineEffect_SetParams(params);
        gPokenavStructPtr->scanlineResetState++;
        break;
    }
    return FALSE;
}

void sub_80F5BDC(void)
{
    gScanlineEffect.state = 3;
    ScanlineEffect_InitHBlankDmaTransfer();
}

void ConditionGraph_Draw(void)
{
    u16 i;

    if (gPokenavStructPtr->needsDraw)
    {
        ConditionGraph_CalcRightHalf(gPokenavStructPtr->curPositions);
        ConditionGraph_CalcLeftHalf(gPokenavStructPtr->curPositions);
        for (i = 0; i < 66; i++)
        {
            gScanlineEffectRegBuffers[1][(i + 55) * 2 + 0] = gScanlineEffectRegBuffers[0][(i + 55) * 2 + 0] = (gPokenavStructPtr->scanlineRight[i][0] << 8) | (gPokenavStructPtr->scanlineRight[i][1]);
            gScanlineEffectRegBuffers[1][(i + 55) * 2 + 1] = gScanlineEffectRegBuffers[0][(i + 55) * 2 + 1] = (gPokenavStructPtr->scanlineLeft[i][0] << 8) | (gPokenavStructPtr->scanlineLeft[i][1]);
        }
        gPokenavStructPtr->needsDraw = 0;
    }
}

void sub_80F5CDC(u8 a0)
{
    u16 i, r5;

    if (gPokenavStructPtr->needsDraw)
    {
        ConditionGraph_CalcRightHalf(gPokenavStructPtr->curPositions);
        ConditionGraph_CalcLeftHalf(gPokenavStructPtr->curPositions);
        r5 = 2 * (55 - a0);
        for (i = 0; i < 66; i ++)
        {
            gScanlineEffectRegBuffers[1][r5 + 0] = gScanlineEffectRegBuffers[0][r5 + 0] = (gPokenavStructPtr->scanlineRight[i][0] << 8) | (gPokenavStructPtr->scanlineRight[i][1]);
            gScanlineEffectRegBuffers[1][r5 + 1] = gScanlineEffectRegBuffers[0][r5 + 1] = (gPokenavStructPtr->scanlineLeft[i][0] << 8) | (gPokenavStructPtr->scanlineLeft[i][1]);
            r5 += 2;
        }
        gPokenavStructPtr->needsDraw = 0;
    }
}

u8 sub_80F5DD4(void)
{
    if (JOY_REPEAT(DPAD_UP))
    {
        return PokenavList_MoveCursorUp();
    }
    else if (JOY_REPEAT(DPAD_DOWN))
    {
        return PokenavList_MoveCursorDown();
    }
    else if (JOY_REPEAT(DPAD_LEFT))
    {
        return sub_80F5FB4();
    }
    else if (JOY_REPEAT(DPAD_RIGHT))
    {
        return sub_80F6010();
    }
    else
    {
        return 0;
    }
}

u8 PokenavList_MoveCursorUp(void)
{
    if (gPokenavStructPtr->unk876E == 0)
    {
        return 0;
    }
    if (gPokenavStructPtr->unk87C9 != 0 && gPokenavStructPtr->selectedIndexOffset == 0)
    {
        sub_80F063C(-1);
        sub_80F6074(-1);
        return 2;
    }
    gPokenavStructPtr->selectedIndexOffset--;
    if (gPokenavStructPtr->unk87C9 == 0 && gPokenavStructPtr->selectedIndexOffset < 0)
    {
        gPokenavStructPtr->selectedIndexOffset = gPokenavStructPtr->unk8772;
    }
    gPokenavStructPtr->unk876E = gPokenavStructPtr->windowTopIndex + gPokenavStructPtr->selectedIndexOffset;
    if (gPokenavStructPtr->unk876E > gPokenavStructPtr->unk8774)
    {
        gPokenavStructPtr->unk876E -= gPokenavStructPtr->unk8774 + 1;
    }
    return 1;
}

u8 PokenavList_MoveCursorDown(void)
{
    if (gPokenavStructPtr->unk876E == gPokenavStructPtr->unk8774)
    {
        return 0;
    }
    if (gPokenavStructPtr->unk87C9 != 0 && gPokenavStructPtr->selectedIndexOffset == 7)
    {
        sub_80F063C(1);
        sub_80F6074(1);
        return 2;
    }
    gPokenavStructPtr->selectedIndexOffset++;
    if (gPokenavStructPtr->unk87C9 == 0 && gPokenavStructPtr->selectedIndexOffset > gPokenavStructPtr->unk8772)
    {
        gPokenavStructPtr->selectedIndexOffset = 0;
    }
    gPokenavStructPtr->unk876E = gPokenavStructPtr->windowTopIndex + gPokenavStructPtr->selectedIndexOffset;
    if (gPokenavStructPtr->unk876E > gPokenavStructPtr->unk8774)
    {
        gPokenavStructPtr->unk876E -= gPokenavStructPtr->unk8774 + 1;
    }
    return 1;
}

u8 sub_80F5FB4(void)
{
    s16 r4;
    if (gPokenavStructPtr->windowTopIndex == 0 || gPokenavStructPtr->unk87C9 == 0)
    {
        return 0;
    }
    if (gPokenavStructPtr->windowTopIndex < 8)
    {
        r4 = -gPokenavStructPtr->windowTopIndex;
    }
    else
    {
        r4 = -8;
    }
    sub_80F063C(r4);
    sub_80F6074(r4);
    return 2;
}

u8 sub_80F6010(void)
{
    s16 r4;
    if (gPokenavStructPtr->unk8772 == gPokenavStructPtr->unk8774 || gPokenavStructPtr->unk87C9 == 0)
    {
        return 0;
    }
    r4 = gPokenavStructPtr->unk8774 - gPokenavStructPtr->unk8772;
    if (r4 > 8)
    {
        r4 = 8;
    }
    sub_80F063C(r4);
    sub_80F6074(r4);
    return 2;
}

void sub_80F6074(s16 a0)
{
    gPokenavStructPtr->windowTopIndex += a0;
    if (gPokenavStructPtr->windowTopIndex > gPokenavStructPtr->unk8774)
    {
        gPokenavStructPtr->windowTopIndex -= gPokenavStructPtr->unk8774 + 1;
    }
    if (gPokenavStructPtr->windowTopIndex < 0)
    {
        gPokenavStructPtr->windowTopIndex += gPokenavStructPtr->unk8774 + 1;
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
    gPokenavStructPtr->unk876E += a0;
    if (gPokenavStructPtr->unk876E > gPokenavStructPtr->unk8774)
    {
        gPokenavStructPtr->unk876E -= gPokenavStructPtr->unk8774 + 1;
    }
    if (gPokenavStructPtr->unk876E < 0)
    {
        gPokenavStructPtr->unk876E += gPokenavStructPtr->unk8774 + 1;
    }
}

void sub_80F6134(void)
{
    if (gPokenavStructPtr->unk87C9 != 0)
    {
        if (gPokenavStructPtr->unk87DC < gPokenavStructPtr->unk8774 - 7)
        {
            gPokenavStructPtr->selectedIndexOffset = 0;
            gPokenavStructPtr->windowTopIndex = gPokenavStructPtr->unk87DC;
            gPokenavStructPtr->unk876E = gPokenavStructPtr->unk87DC;
            gPokenavStructPtr->unk8772 = gPokenavStructPtr->windowTopIndex + 7;
            if (gPokenavStructPtr->unk8772 > gPokenavStructPtr->unk8774)
            {
                gPokenavStructPtr->unk8772 -= gPokenavStructPtr->unk8774 + 1;
            }
        }
        else
        {
            gPokenavStructPtr->windowTopIndex = gPokenavStructPtr->unk8774 - 7;
            gPokenavStructPtr->unk8772 = gPokenavStructPtr->unk8774;
            gPokenavStructPtr->unk876E = gPokenavStructPtr->unk87DC;
            gPokenavStructPtr->selectedIndexOffset = 7 - (gPokenavStructPtr->unk8774 -  gPokenavStructPtr->unk876E);
        }
    }
    else
    {
        gPokenavStructPtr->selectedIndexOffset = gPokenavStructPtr->unk87DC;
        gPokenavStructPtr->unk876E = gPokenavStructPtr->unk87DC;
    }
}

void sub_80F6208(void)
{
    gPokenavStructPtr->unk8FE6 = 0;
    gPokenavStructPtr->monId = 0;
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
            if (GetBoxMonData(&gPokemonStorage.boxes[gPokenavStructPtr->unk8FE6 + 0][gPokenavStructPtr->monId], MON_DATA_RIBBON_COUNT) != 0)
            {
                gPokenavStructPtr->unk8FE8 = 1;
                return FALSE;
            }
            if (GetBoxMonData(&gPokemonStorage.boxes[gPokenavStructPtr->unk8FE6 + 7][gPokenavStructPtr->monId], MON_DATA_RIBBON_COUNT) != 0)
            {
                gPokenavStructPtr->unk8FE8 = 1;
                return FALSE;
            }
            if (++gPokenavStructPtr->monId >= 30)
            {
                gPokenavStructPtr->monId = 0;
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
    gPokenavStructPtr->monId = 0;
    sub_80F492C();
    if (gPokenavStructPtr->unk6DAC == 0)
    {
        while (sub_80F63D0())
            ;
    }
}

bool8 sub_80F63D0(void)
{
    struct UnkUsePokeblockSub item;
    u8 ribbons;
    u16 i;

    switch (gPokenavStructPtr->unk8FE6)
    {
    default:
        item.unk3_14 = 1;
        for (i = 0; i < 15; i++)
        {
            ribbons = GetBoxMonData(&gPokemonStorage.boxes[gPokenavStructPtr->unk8FE6][gPokenavStructPtr->monId + 0], MON_DATA_RIBBON_COUNT);
            if (ribbons != 0)
            {
                item.boxId = gPokenavStructPtr->unk8FE6;
                item.monId = gPokenavStructPtr->monId;
                item.unk0 = ribbons;
                InsertMonListItem(&item);
            }
            if (++gPokenavStructPtr->monId == 30)
            {
                gPokenavStructPtr->monId = 0;
                if (++gPokenavStructPtr->unk8FE6 == 14)
                    break;
            }
        }
        break;
    case 15:
        return FALSE;
    case 14:
        item.unk3_14 = 1;
        item.boxId = 14;
        for (i = 0; i < 6; i++)
        {
            ribbons = GetMonData(&gPlayerParty[i], MON_DATA_RIBBON_COUNT);
            if (ribbons != 0)
            {
                item.monId = i;
                item.unk0 = ribbons;
                InsertMonListItem(&item);
            }
        }
        sub_80F49F4();
        gPokenavStructPtr->unk8FE6++;
        return FALSE;
    }
    return TRUE;
}

u8 * sub_80F6514(u8 * str, u16 listId, u8 sp4)
{
    u8 * dest = str;
    u8 boxId = gPokenavStructPtr->unk893c[listId].boxId;
    u8 monId = gPokenavStructPtr->unk893c[listId].monId;
    u16 species;
    u16 level;
    u8 gender;

    if (!GetBoxOrPartyMonData(boxId, monId, MON_DATA_IS_EGG, NULL))
    {
        GetBoxOrPartyMonData(boxId, monId, MON_DATA_NICKNAME, dest);
        StringGet_Nickname(dest);
        species = GetBoxOrPartyMonData(boxId, monId, MON_DATA_SPECIES, NULL);
        if (boxId == 14)
        {
            level = GetMonData(&gPlayerParty[monId], MON_DATA_LEVEL);
            gender = GetMonGender(&gPlayerParty[monId]);
        }
        else
        {
            level = GetLevelFromBoxMonExp(&gPokemonStorage.boxes[boxId][monId]);
            gender = GetGenderFromSpeciesAndPersonality(species, GetBoxOrPartyMonData(boxId, monId, MON_DATA_PERSONALITY, NULL));
        }
        if (ShouldHideGenderIcon(species, str))
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
        if (sp4 == 1)
        {
            dest = AlignInt1InMenuWindow(dest, gPokenavStructPtr->unk893c[listId].unk0, 0x80, 0x01);
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
    u8 ribbonId;
    u32 ribbonFlags;
    u16 i, j;
    u8 r2;
    u8 r0;

    sub_80F6514(gPokenavStructPtr->nameText[0], gPokenavStructPtr->unk876E, 0);
    sub_80F4824(gPokenavStructPtr->unk876E, 0);
    gPokenavStructPtr->unk87DC = gPokenavStructPtr->unk876E;
    gPokenavStructPtr->unkBC8E = 0;
    ribbonId = 0;
    ribbonFlags = GetBoxOrPartyMonData(gPokenavStructPtr->unk893c[gPokenavStructPtr->unk876E].boxId, gPokenavStructPtr->unk893c[gPokenavStructPtr->unk876E].monId, MON_DATA_RIBBONS, NULL);
    gPokenavStructPtr->numGiftRibbons = 0;
    for (i = 0; i < 17; i++)
    {
        switch (gUnknown_083E499C[i])
        {
        case MON_DATA_COOL_RIBBON:
        case MON_DATA_BEAUTY_RIBBON:
        case MON_DATA_CUTE_RIBBON:
        case MON_DATA_SMART_RIBBON:
        case MON_DATA_TOUGH_RIBBON:
            r2 = ribbonFlags & 7;
            ribbonFlags >>= 3;
            r0 = 4;
            break;
        default:
            r2 = ribbonFlags & 1;
            ribbonFlags >>= 1;
            r0 = 1;
            break;
        };
        for (j = 0; j < r2; j++)
            gPokenavStructPtr->unkBC4C[gPokenavStructPtr->unkBC8E++] = ribbonId + j;
        if (r2 && ribbonId > 24)
            gPokenavStructPtr->numGiftRibbons++;
        ribbonId += r0;
    }
    if (gPokenavStructPtr->unkBC8E != gPokenavStructPtr->numGiftRibbons)
    {
        gPokenavStructPtr->unkBC90 = 0;
        gPokenavStructPtr->unkBC91 = 0;
    }
    else
    {
        gPokenavStructPtr->unkBC90 = 0;
        gPokenavStructPtr->unkBC91 = 3;
    }
    r2 = gPokenavStructPtr->unkBC8E - gPokenavStructPtr->numGiftRibbons;
    for (i = 0; i < 3; i++)
    {
        if (r2 > 8)
        {
            gPokenavStructPtr->unkBC96[i] = 9;
            r2 -= 9;
        }
        else
        {
            gPokenavStructPtr->unkBC96[i] = r2;
            r2 = 0;
        }
    }
    gPokenavStructPtr->unkBC96[i] = gPokenavStructPtr->numGiftRibbons;
}

u8 sub_80F68E8(void)
{
    s8 r5 = gPokenavStructPtr->unkBC90;
    s8 r4 = gPokenavStructPtr->unkBC91;
    s8 r12 = 1;
    do
    {
        if (JOY_REPEAT(DPAD_UP) && r4 > 0)
        {
            while (r4 > 0)
            {
                r4--;
                if (gPokenavStructPtr->unkBC96[r4] != 0)
                    break;
            }
            if (gPokenavStructPtr->unkBC96[r4] != 0)
            {
                if (r5 >= gPokenavStructPtr->unkBC96[r4])
                    r5 = gPokenavStructPtr->unkBC96[r4] - 1;
                break;
            }
            r4 = gPokenavStructPtr->unkBC91;
        }
        if (JOY_REPEAT(DPAD_DOWN) && r4 < 3)
        {
            while (r4 < 3)
            {
                r4++;
                if (gPokenavStructPtr->unkBC96[r4] != 0)
                    break;
            }
            if (gPokenavStructPtr->unkBC96[r4] != 0)
            {
                if (r5 >= gPokenavStructPtr->unkBC96[r4])
                    r5 = gPokenavStructPtr->unkBC96[r4] - 1;
                break;
            }
            r4 = gPokenavStructPtr->unkBC91;
        }
        if (JOY_REPEAT(DPAD_LEFT))
        {
            if (r5 > 0)
            {
                r5--;
                break;
            }
        }
        if (JOY_REPEAT(DPAD_RIGHT))
        {
            if (r5 < gPokenavStructPtr->unkBC96[r4] - 1)
            {
                r5++;
                break;
            }
        }
        r12 = 0;
    } while (0);
    if (r12)
    {
        if (r5 != gPokenavStructPtr->unkBC90 || r4 != gPokenavStructPtr->unkBC91)
        {
            gPokenavStructPtr->unkBC90 = r5;
            gPokenavStructPtr->unkBC91 = r4;
        }
        else
            r12 = 0;
    }
    return r12;
}

void sub_80F6A4C(s8 a0)
{
    gPokenavStructPtr->unk876E += a0;
    if (gPokenavStructPtr->unk876E < 0)
    {
        gPokenavStructPtr->unk876E = gPokenavStructPtr->unk8774;
    }
    if (gPokenavStructPtr->unk876E > gPokenavStructPtr->unk8774)
    {
        gPokenavStructPtr->unk876E = 0;
    }
    gPokenavStructPtr->unkBC94 = a0;
    gPokenavStructPtr->unk87DC = gPokenavStructPtr->unk876E;
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
        if (!MoveConditionMonOffscreen())
        {
            gPokenavStructPtr->unk87DE++;
        }
        break;
    case 1:
        REG_DISPCNT |= DISPCNT_WIN0_ON;
        sub_80F1480();
        sub_80F66E0();
        gPokenavStructPtr->unk87DE++;
        break;
    case 2:
        DrawMonRibbonIcons();
        gPokenavStructPtr->unk87DE++;
        break;
    case 3:
        sub_80F13FC();
        gPokenavStructPtr->unk87DE++;
        break;
    case 4:
        sub_80F4824(gPokenavStructPtr->unk876E, 0);
        gPokenavStructPtr->unk87DE++;
        break;
    case 5:
        sub_80F2E18(0);
        gPokenavStructPtr->unk87DE++;
        break;
    case 6:
        if (!MoveConditionMonOnscreen())
        {
            sub_80F1438();
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
