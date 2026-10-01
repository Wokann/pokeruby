// Includes
#include "global.h"
#include "constants/species.h"
#include "constants/maps.h"
#include "constants/songs.h"
#include "constants/region_map_sections.h"
#include "sound.h"
#include "ewram.h"
#include "main.h"
#include "palette.h"
#include "task.h"
#include "sprite.h"
#include "region_map.h"
#include "string_util.h"
#include "text.h"
#include "wild_encounter.h"
#include "roamer.h"
#include "overworld.h"
#include "event_data.h"
#include "trig.h"
#include "graphics.h"

#define MAP_GROUP_TOWNS_AND_ROUTES MAP_GROUP(MAP_PETALBURG_CITY)
#define MAP_GROUP_DUNGEONS          MAP_GROUP(MAP_METEOR_FALLS_1F_1R)
#define MAP_GROUP_SPECIAL_AREA      MAP_GROUP(MAP_SAFARI_ZONE_NORTHWEST)

#define AREA_SCREEN_WIDTH  32
#define AREA_SCREEN_HEIGHT 20

#define MAX_AREA_HIGHLIGHTS 64
#define MAX_AREA_MARKERS    32

#define TAG_AREA_MARKER  2
#define TAG_AREA_UNKNOWN 3

// Static type declarations

struct OverworldArea
{
    u8 mapGroup;
    u8 mapNum;
    u16 regionMapSectionId;
};

struct PokedexAreaScreen
{
    void (*callback)(void);
    MainCallback prev;
    MainCallback next;
    u16 state;
    u16 species;
    struct OverworldArea overworldAreasWithMons[MAX_AREA_HIGHLIGHTS];
    u16 numOverworldAreas;
    u16 numSpecialAreas;
    u16 drawAreaGlowState;
    u16 areaGlowTilemap[AREA_SCREEN_WIDTH * AREA_SCREEN_HEIGHT];
    u16 markerTimer;
    u16 glowTimer;
    u16 areaShadeBldArgLo;
    u16 areaShadeBldArgHi;
    u8 showingMarkers;
    u8 markerFlashCounter;
    u16 specialAreaRegionMapSectionIds[MAX_AREA_MARKERS];
    struct Sprite * areaMarkerSprites[MAX_AREA_MARKERS];
    u16 numAreaMarkerSprites;
    u8 *screenSwitchState;
    struct RegionMap regionMap;
    u8 charBuffer[0x40];
    struct Sprite * areaUnknownSprites[3];
    u8 areaUnknownGraphicsBuffer[0x600];
};

#define ePokedexAreaScreen (*(struct PokedexAreaScreen *)gSharedMem)

// Static RAM declarations

// Static ROM declarations

static void CB2_UnusedPokedexAreaScreen(void);
static void VBlankCB_PokedexAreaScreen(void);
static void MainCB_PokedexAreaScreen(void);
static void SetPokedexAreaScreenCallback(void (*func)(void));
static void ResetDrawAreaGlowState(void);
static bool8 DrawAreaGlow(void);
static void FindMapsWithMon(u16 species);
static void SetAreaHasMon(u16 mapGroup, u16 mapNum);
static void SetSpecialMapHasMon(u16 mapGroup, u16 mapNum);
static bool8 MapHasSpecies(const struct WildPokemonHeader *header, u16 species);
static bool8 MonListHasSpecies(const struct WildPokemonInfo *info, u16 species, u16 size);
static void BuildAreaGlowTilemap(void);
static void StartAreaGlow(void);
static void DoAreaGlow(void);
static void DebugCB_WaitFade(void);
static void DebugCB_WaitButton(void);
static void DebugCB_GoBack(void);
static void DebugCB_GoNext(void);

// .rodata

static const u16 sAreaGlow_Pal[] = INCBIN_U16("graphics/pokedex/area_glow.gbapal");

static const u8 sAreaGlow_Gfx[] = INCBIN_U8("graphics/pokedex/area_glow.4bpp.lz");

static const u16 sSpeciesHiddenFromAreaScreen[] = {SPECIES_WYNAUT};

static const u16 sFeebasData[][3] = {
    {SPECIES_FEEBAS, MAP_GROUP(MAP_ROUTE119), MAP_NUM(MAP_ROUTE119)},
    {NUM_SPECIES}
};

static const u16 sLandmarkData[][2] = {
    {MAPSEC_SKY_PILLAR,      FLAG_LANDMARK_SKY_PILLAR},
    {MAPSEC_SEAFLOOR_CAVERN, FLAG_LANDMARK_SEAFLOOR_CAVERN},
    {MAPSEC_NONE}
};

static struct PokedexAreaScreen *const sPokedexAreaScreen = &ePokedexAreaScreen;

// .text

void UnusedPokedexAreaScreen(u16 species, MainCallback prev, MainCallback next)
{
    sPokedexAreaScreen->prev = prev;
    sPokedexAreaScreen->next = next;
    sPokedexAreaScreen->species = species;
    SetMainCallback2(CB2_UnusedPokedexAreaScreen);
}

static void CB2_UnusedPokedexAreaScreen(void)
{
    switch (gMain.state)
    {
        case 0:
            SetVBlankCallback(NULL);
            REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_BG1_ON | DISPCNT_WIN0_ON | DISPCNT_WIN1_ON;
            ResetPaletteFade();
            ResetSpriteData();
            ResetTasks();
            FreeSpriteTileRanges();
            FreeAllSpritePalettes();
            REG_BG0HOFS = 0;
            REG_BG0VOFS = -8;
            REG_BG2VOFS = 0;
            REG_BG2HOFS = 0;
            REG_BG3HOFS = 0;
            REG_BG3VOFS = 0;
            break;
        case 1:
            InitRegionMap(&sPokedexAreaScreen->regionMap, FALSE);
            StringFill(sPokedexAreaScreen->charBuffer, CHAR_SPACE, 16);
            break;
        case 2:
            ResetDrawAreaGlowState();
            break;
        case 3:
            if (DrawAreaGlow())
                return;
            break;
        case 4:
            CreateRegionMapPlayerIcon(1, 1);
            PokedexAreaScreen_UpdateRegionMapVariablesAndVideoRegs(0, -8);
            SetVBlankCallback(VBlankCB_PokedexAreaScreen);
            break;
        case 5:
            BeginNormalPaletteFade(0xFFFFFFEB, 0, 16, 0, RGB(0, 0, 0));
            break;
        case 6:
            REG_BLDCNT = BLDCNT_TGT1_BG0 | BLDCNT_EFFECT_BLEND | BLDCNT_TGT2_BG0 | BLDCNT_TGT2_BG1 | BLDCNT_TGT2_BG2 | BLDCNT_TGT2_BG3 | BLDCNT_TGT2_OBJ | BLDCNT_TGT2_BD;
            StartAreaGlow();
            REG_DISPCNT = DISPCNT_MODE_1 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG1_ON | DISPCNT_BG2_ON | DISPCNT_OBJ_ON | DISPCNT_WIN0_ON | DISPCNT_WIN1_ON;
            break;
        case 7:
            SetPokedexAreaScreenCallback(DebugCB_WaitFade);
            SetMainCallback2(MainCB_PokedexAreaScreen);
            return;
    }
    gMain.state++;
}

static void VBlankCB_PokedexAreaScreen(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void MainCB_PokedexAreaScreen(void)
{
    sPokedexAreaScreen->callback();
    DoAreaGlow();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void SetPokedexAreaScreenCallback(void (*func)(void))
{
    sPokedexAreaScreen->callback = func;
    sPokedexAreaScreen->state = 0;
}

static void ResetDrawAreaGlowState(void)
{
    sPokedexAreaScreen->drawAreaGlowState = 0;
}

static bool8 DrawAreaGlow(void)
{
    switch (sPokedexAreaScreen->drawAreaGlowState)
    {
        case 0:
            FindMapsWithMon(sPokedexAreaScreen->species);
            break;
        case 1:
            BuildAreaGlowTilemap();
            break;
        case 2:
            LZ77UnCompVram(sAreaGlow_Gfx, BG_CHAR_ADDR(3));
            break;
        case 3:
            DmaCopy16(3, sPokedexAreaScreen->areaGlowTilemap, BG_SCREEN_ADDR(30), 0x500);
            break;
        case 4:
            LoadPalette(sAreaGlow_Pal, 0, 32);
            break;
        case 5:
            REG_BG0CNT = BGCNT_PRIORITY(1) | BGCNT_CHARBASE(3) | BGCNT_16COLOR | BGCNT_SCREENBASE(30) | BGCNT_TXT256x256;
            sPokedexAreaScreen->drawAreaGlowState++;
            return FALSE;
        default:
            return FALSE;
    }
    sPokedexAreaScreen->drawAreaGlowState++;
    return TRUE;
}

static void FindMapsWithMon(u16 species)
{
    u16 i;
    struct Roamer *roamer;

    if (species != ROAMER_SPECIES)
    {
        sPokedexAreaScreen->numOverworldAreas = 0;
        sPokedexAreaScreen->numSpecialAreas = 0;
        for (i = 0; i < ARRAY_COUNT(sSpeciesHiddenFromAreaScreen); i++)
        {
            if (sSpeciesHiddenFromAreaScreen[i] == species)
                return;
        }
        for (i = 0; sFeebasData[i][0] != NUM_SPECIES; i++)
        {
            if (species == sFeebasData[i][0])
            {
                switch (sFeebasData[i][1])
                {
                    case MAP_GROUP_TOWNS_AND_ROUTES:
                        SetAreaHasMon(sFeebasData[i][1], sFeebasData[i][2]);
                        break;
                    case MAP_GROUP_DUNGEONS:
                    case MAP_GROUP_SPECIAL_AREA:
                        SetSpecialMapHasMon(sFeebasData[i][1], sFeebasData[i][2]);
                        break;
                }
            }
        }
        for (i = 0; gWildMonHeaders[i].mapGroup != 0xFF; i++)
        {
            if (MapHasSpecies(gWildMonHeaders + i, species))
            {
                switch (gWildMonHeaders[i].mapGroup)
                {
                    case MAP_GROUP_TOWNS_AND_ROUTES:
                        SetAreaHasMon(gWildMonHeaders[i].mapGroup, gWildMonHeaders[i].mapNum);
                        break;
                    case MAP_GROUP_DUNGEONS:
                    case MAP_GROUP_SPECIAL_AREA:
                        SetSpecialMapHasMon(gWildMonHeaders[i].mapGroup, gWildMonHeaders[i].mapNum);
                        break;
                }
            }
        }
    }
    else
    {
        sPokedexAreaScreen->numSpecialAreas = 0;
        roamer = &gSaveBlock1.roamer;
        if (roamer->active)
        {
            GetRoamerLocation(&sPokedexAreaScreen->overworldAreasWithMons[0].mapGroup, &sPokedexAreaScreen->overworldAreasWithMons[0].mapNum);
            sPokedexAreaScreen->overworldAreasWithMons[0].regionMapSectionId = Overworld_GetMapHeaderByGroupAndId(sPokedexAreaScreen->overworldAreasWithMons[0].mapGroup, sPokedexAreaScreen->overworldAreasWithMons[0].mapNum)->regionMapSectionId;
            sPokedexAreaScreen->numOverworldAreas = 1;
        }
        else
            sPokedexAreaScreen->numOverworldAreas = 0;
    }
}

static void SetAreaHasMon(u16 mapGroup, u16 mapNum)
{
    if (sPokedexAreaScreen->numOverworldAreas < MAX_AREA_HIGHLIGHTS)
    {
        sPokedexAreaScreen->overworldAreasWithMons[sPokedexAreaScreen->numOverworldAreas].mapGroup = mapGroup;
        sPokedexAreaScreen->overworldAreasWithMons[sPokedexAreaScreen->numOverworldAreas].mapNum = mapNum;
        sPokedexAreaScreen->overworldAreasWithMons[sPokedexAreaScreen->numOverworldAreas].regionMapSectionId = CorrectSpecialMapSecId(Overworld_GetMapHeaderByGroupAndId(mapGroup, mapNum)->regionMapSectionId);
        sPokedexAreaScreen->numOverworldAreas++;
    }
}

static void SetSpecialMapHasMon(u16 mapGroup, u16 mapNum)
{
    const struct MapHeader *mapHeader;
    u16 i;

    if (sPokedexAreaScreen->numSpecialAreas < MAX_AREA_MARKERS)
    {
        mapHeader = Overworld_GetMapHeaderByGroupAndId(mapGroup, mapNum);
        if (mapHeader->regionMapSectionId < MAPSEC_NONE)
        {
            for (i = 0; sLandmarkData[i][0] != MAPSEC_NONE; i++)
            {
                if (mapHeader->regionMapSectionId == sLandmarkData[i][0] && !FlagGet(sLandmarkData[i][1]))
                    return;
            }
            for (i = 0; i < sPokedexAreaScreen->numSpecialAreas; i++)
            {
                if (sPokedexAreaScreen->specialAreaRegionMapSectionIds[i] == mapHeader->regionMapSectionId)
                    break;
            }
            if (i == sPokedexAreaScreen->numSpecialAreas)
            {
                sPokedexAreaScreen->specialAreaRegionMapSectionIds[i] = mapHeader->regionMapSectionId;
                sPokedexAreaScreen->numSpecialAreas++;
            }
        }
    }
}

static bool8 MapHasSpecies(const struct WildPokemonHeader *header, u16 species)
{
    if (MonListHasSpecies(header->landMonsInfo, species, 12))
        return TRUE;
    if (MonListHasSpecies(header->waterMonsInfo, species, 5))
        return TRUE;
    if (MonListHasSpecies(header->fishingMonsInfo, species, 12))
        return TRUE;
    if (MonListHasSpecies(header->rockSmashMonsInfo, species, 5))
        return TRUE;
    return FALSE;
}

static bool8 MonListHasSpecies(const struct WildPokemonInfo *info, u16 species, u16 size)
{
    u16 i;
    if (info != NULL)
    {
        for (i = 0; i < size; i++)
        {
            if (info->wildPokemon[i].species == species)
                return TRUE;
        }
    }
    return FALSE;
}

#ifdef NONMATCHING
static void BuildAreaGlowTilemap(void)
{
    static EWRAM_DATA u16 gPokedexAreaGlowIndex = 0;
    static EWRAM_DATA u16 gPokedexAreaGlowX = 0;
    static EWRAM_DATA u16 gPokedexAreaGlowY = 0;
    static EWRAM_DATA u16 gPokedexAreaGlowTilemapIndex = 0;
    static EWRAM_DATA u16 gPokedexAreaGlowEdgeFlags = 0;
    static EWRAM_DATA u16 gPokedexAreaGlowCornerFlags = 0;
    u32 r3;

    for (gPokedexAreaGlowIndex = 0; gPokedexAreaGlowIndex < 0x280; gPokedexAreaGlowIndex++)
        sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowIndex] = 0;

    for (gPokedexAreaGlowIndex = 0; gPokedexAreaGlowIndex < sPokedexAreaScreen->numOverworldAreas; gPokedexAreaGlowIndex++)
    {
        gPokedexAreaGlowTilemapIndex = 0;
        for (gPokedexAreaGlowY = 0; gPokedexAreaGlowY < 20; gPokedexAreaGlowY++)
        {
            for (gPokedexAreaGlowX = 0; gPokedexAreaGlowX < 32; gPokedexAreaGlowX++)
            {
                if (GetRegionMapSecIdAt(gPokedexAreaGlowX, gPokedexAreaGlowY) == sPokedexAreaScreen->overworldAreasWithMons[gPokedexAreaGlowIndex].regionMapSectionId)
                {
                    sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowTilemapIndex] = 0xFFFF;
                }
                gPokedexAreaGlowTilemapIndex++;
            }
        }
    }

    gPokedexAreaGlowTilemapIndex = 0;
    for (gPokedexAreaGlowY = 0; gPokedexAreaGlowY < 20; gPokedexAreaGlowY++)
    {
        for (gPokedexAreaGlowX = 0; gPokedexAreaGlowX < 32; gPokedexAreaGlowX++)
        {
            if (sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowTilemapIndex] == 0xFFFF)
            {
                if (gPokedexAreaGlowX != 0 && sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowTilemapIndex - 1] != 0xFFFF)
                    sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowTilemapIndex - 1] |= 0x02;
                if (gPokedexAreaGlowX != 31 && sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowTilemapIndex + 1] != 0xFFFF)
                    sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowTilemapIndex + 1] |= 0x01;
                if (gPokedexAreaGlowY != 0 && sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowTilemapIndex - 32] != 0xFFFF)
                    sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowTilemapIndex - 32] |= 0x08;
                if (gPokedexAreaGlowY != 19 && sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowTilemapIndex + 32] != 0xFFFF)
                    sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowTilemapIndex + 32] |= 0x04;
                if (gPokedexAreaGlowX != 0 && gPokedexAreaGlowY != 0 && sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowTilemapIndex - 33] != 0xFFFF)
                    sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowTilemapIndex - 33] |= 0x10;
                if (gPokedexAreaGlowX != 31 && gPokedexAreaGlowY != 0 && sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowTilemapIndex - 31] != 0xFFFF)
                    sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowTilemapIndex - 31] |= 0x40;
                if (gPokedexAreaGlowX != 0 && gPokedexAreaGlowY != 19 && sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowTilemapIndex + 31] != 0xFFFF)
                    sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowTilemapIndex + 31] |= 0x20;
                if (gPokedexAreaGlowX != 31 && gPokedexAreaGlowY != 19 && sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowTilemapIndex + 33] != 0xFFFF)
                    sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowTilemapIndex + 33] |= 0x80;
            }
            gPokedexAreaGlowTilemapIndex++;
        }
    }

    for (gPokedexAreaGlowIndex = 0; gPokedexAreaGlowIndex < 0x280; gPokedexAreaGlowIndex++) // Register difference on induction: expected r3, got r1
    {
        if (sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowIndex] == 0xFFFF)
            sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowIndex] = 0x10;
        else if (sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowIndex] != 0)
        {
            if (sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowIndex] & 0x02)
                sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowIndex] &= 0xFFCF;
            if (sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowIndex] & 0x01)
                sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowIndex] &= 0xFF3F;
            if (sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowIndex] & 0x08)
                sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowIndex] &= 0xFFAF;
            if (sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowIndex] & 0x04)
                sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowIndex] &= 0xFF5F;
            gPokedexAreaGlowEdgeFlags = sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowIndex] & 0x0F;
            gPokedexAreaGlowCornerFlags = sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowIndex] & 0xF0;
            if (gPokedexAreaGlowCornerFlags)
            {
                sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowIndex] &= 0x0F;
                switch (gPokedexAreaGlowEdgeFlags)
                {
                    case 0:
                        if (gPokedexAreaGlowCornerFlags != 0)
                            sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowIndex] += (gPokedexAreaGlowCornerFlags >> 4) + 0x10;
                        break;
                    case 2:
                        if (gPokedexAreaGlowCornerFlags != 0)
                            sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowIndex] += (gPokedexAreaGlowCornerFlags >> 4) + 0x1E;
                        break;
                    case 1:
                        if (gPokedexAreaGlowCornerFlags != 0)
                            sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowIndex] += (gPokedexAreaGlowCornerFlags >> 6) + 0x20;
                        break;
                    case 8:
                        if (gPokedexAreaGlowCornerFlags != 0)
                        {
                            r3 = 0;
                            if (gPokedexAreaGlowCornerFlags & 0x80)
                                r3 |= 1;
                            if (gPokedexAreaGlowCornerFlags & 0x20)
                                r3 |= 2;
                            sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowIndex] += r3 + 0x20;
                        }
                        break;
                    case 4:
                        if (gPokedexAreaGlowCornerFlags != 0)
                        {
                            r3 = 0;
                            if (gPokedexAreaGlowCornerFlags & 0x40)
                                r3 |= 1;
                            if (gPokedexAreaGlowCornerFlags & 0x10)
                                r3 |= 2;
                            sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowIndex] += r3 + 0x21;
                        }
                        break;
                    case 5:
                    case 6:
                        sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowIndex] += 0x27;
                        break;
                    case 9:
                    case 10:
                        sPokedexAreaScreen->areaGlowTilemap[gPokedexAreaGlowIndex] += 0x25;
                        break;
                }
            }
        }
    }
}
# else
EWRAM_DATA u16 gPokedexAreaGlowIndex = 0;
EWRAM_DATA u16 gPokedexAreaGlowX = 0;
EWRAM_DATA u16 gPokedexAreaGlowY = 0;
EWRAM_DATA u16 gPokedexAreaGlowTilemapIndex = 0;
EWRAM_DATA u16 gPokedexAreaGlowEdgeFlags = 0;
EWRAM_DATA u16 gPokedexAreaGlowCornerFlags = 0;
static NAKED void BuildAreaGlowTilemap(void)
{
    asm_unified("\tpush {r4-r7,lr}\n"
                "\tmov r7, r10\n"
                "\tmov r6, r9\n"
                "\tmov r5, r8\n"
                "\tpush {r5-r7}\n"
                "\tsub sp, 0xC\n"
                "\tldr r1, _08110E5C @ =gPokedexAreaGlowIndex\n"
                "\tmovs r0, 0\n"
                "\tstrh r0, [r1]\n"
                "\tmov r9, r1\n"
                "\tldr r0, _08110E60 @ =gSharedMem\n"
                "\tmov r8, r0\n"
                "\tldr r1, _08110E64 @ =gPokedexAreaGlowX\n"
                "\tmov r12, r1\n"
                "\tmov r1, r9\n"
                "\tmovs r4, 0x8B\n"
                "\tlsls r4, 1\n"
                "\tadd r4, r8\n"
                "\tldr r3, _08110E68 @ =0x0000027f\n"
                "\tmovs r2, 0\n"
                "_08110C5C:\n"
                "\tldrh r0, [r1]\n"
                "\tlsls r0, 1\n"
                "\tadds r0, r4\n"
                "\tstrh r2, [r0]\n"
                "\tldrh r0, [r1]\n"
                "\tadds r0, 0x1\n"
                "\tstrh r0, [r1]\n"
                "\tlsls r0, 16\n"
                "\tlsrs r0, 16\n"
                "\tcmp r0, r3\n"
                "\tbls _08110C5C\n"
                "\tmovs r0, 0\n"
                "\tmov r3, r9\n"
                "\tstrh r0, [r3]\n"
                "\tmovs r1, 0x88\n"
                "\tlsls r1, 1\n"
                "\tadd r1, r8\n"
                "\tldrh r0, [r1]\n"
                "\tcmp r0, 0\n"
                "\tbeq _08110D04\n"
                "\tldr r4, _08110E6C @ =gPokedexAreaGlowTilemapIndex\n"
                "\tmovs r2, 0\n"
                "\tldr r5, _08110E64 @ =gPokedexAreaGlowX\n"
                "\tmov r0, r8\n"
                "\tstr r0, [sp, 0x8]\n"
                "\tmovs r1, 0x8B\n"
                "\tlsls r1, 1\n"
                "\tadd r1, r8\n"
                "\tmov r10, r1\n"
                "\tldr r6, _08110E70 @ =gPokedexAreaGlowY\n"
                "\tmov r7, r9\n"
                "_08110C9A:\n"
                "\tstrh r2, [r4]\n"
                "\tldr r3, _08110E70 @ =gPokedexAreaGlowY\n"
                "\tstrh r2, [r3]\n"
                "_08110CA0:\n"
                "\tmov r0, r12\n"
                "\tstrh r2, [r0]\n"
                "_08110CA4:\n"
                "\tldrh r0, [r5]\n"
                "\tldrh r1, [r6]\n"
                "\tstr r2, [sp]\n"
                "\tbl GetRegionMapSecIdAt\n"
                "\tldrh r1, [r7]\n"
                "\tlsls r1, 2\n"
                "\tldr r3, [sp, 0x8]\n"
                "\tadds r1, r3\n"
                "\tlsls r0, 16\n"
                "\tlsrs r0, 16\n"
                "\tldr r2, [sp]\n"
                "\tldrh r1, [r1, 0x12]\n"
                "\tcmp r0, r1\n"
                "\tbne _08110CCC\n"
                "\tldrh r0, [r4]\n"
                "\tlsls r0, 1\n"
                "\tadd r0, r10\n"
                "\tldr r1, _08110E74 @ =0x0000ffff\n"
                "\tstrh r1, [r0]\n"
                "_08110CCC:\n"
                "\tldrh r0, [r4]\n"
                "\tadds r0, 0x1\n"
                "\tstrh r0, [r4]\n"
                "\tldrh r0, [r5]\n"
                "\tadds r0, 0x1\n"
                "\tstrh r0, [r5]\n"
                "\tlsls r0, 16\n"
                "\tlsrs r0, 16\n"
                "\tcmp r0, 0x1F\n"
                "\tbls _08110CA4\n"
                "\tldrh r0, [r6]\n"
                "\tadds r0, 0x1\n"
                "\tstrh r0, [r6]\n"
                "\tlsls r0, 16\n"
                "\tlsrs r0, 16\n"
                "\tldr r1, _08110E64 @ =gPokedexAreaGlowX\n"
                "\tmov r12, r1\n"
                "\tcmp r0, 0x13\n"
                "\tbls _08110CA0\n"
                "\tldrh r0, [r7]\n"
                "\tadds r0, 0x1\n"
                "\tstrh r0, [r7]\n"
                "\tlsls r0, 16\n"
                "\tlsrs r0, 16\n"
                "\tldr r3, _08110E78 @ =gSharedMem + 0x110\n"
                "\tldrh r3, [r3]\n"
                "\tcmp r0, r3\n"
                "\tbcc _08110C9A\n"
                "_08110D04:\n"
                "\tmovs r0, 0\n"
                "\tldr r1, _08110E6C @ =gPokedexAreaGlowTilemapIndex\n"
                "\tstrh r0, [r1]\n"
                "\tldr r3, _08110E70 @ =gPokedexAreaGlowY\n"
                "\tstrh r0, [r3]\n"
                "\tldr r6, _08110E64 @ =gPokedexAreaGlowX\n"
                "\tmov r10, r0\n"
                "\tadds r3, r1, 0\n"
                "\tldr r5, _08110E7C @ =gSharedMem + 0x116\n"
                "\tldr r4, _08110E74 @ =0x0000ffff\n"
                "\tldr r7, _08110E70 @ =gPokedexAreaGlowY\n"
                "_08110D1A:\n"
                "\tmov r0, r10\n"
                "\tstrh r0, [r6]\n"
                "_08110D1E:\n"
                "\tldrh r1, [r3]\n"
                "\tlsls r0, r1, 1\n"
                "\tadds r0, r5\n"
                "\tldrh r0, [r0]\n"
                "\tcmp r0, r4\n"
                "\tbne _08110E10\n"
                "\tldrh r0, [r6]\n"
                "\tcmp r0, 0\n"
                "\tbeq _08110D42\n"
                "\tsubs r0, r1, 0x1\n"
                "\tlsls r0, 1\n"
                "\tadds r2, r0, r5\n"
                "\tldrh r1, [r2]\n"
                "\tcmp r1, r4\n"
                "\tbeq _08110D42\n"
                "\tmovs r0, 0x2\n"
                "\torrs r0, r1\n"
                "\tstrh r0, [r2]\n"
                "_08110D42:\n"
                "\tldrh r0, [r6]\n"
                "\tcmp r0, 0x1F\n"
                "\tbeq _08110D5C\n"
                "\tldrh r0, [r3]\n"
                "\tadds r0, 0x1\n"
                "\tlsls r0, 1\n"
                "\tadds r2, r0, r5\n"
                "\tldrh r1, [r2]\n"
                "\tcmp r1, r4\n"
                "\tbeq _08110D5C\n"
                "\tmovs r0, 0x1\n"
                "\torrs r0, r1\n"
                "\tstrh r0, [r2]\n"
                "_08110D5C:\n"
                "\tldrh r0, [r7]\n"
                "\tcmp r0, 0\n"
                "\tbeq _08110D76\n"
                "\tldrh r0, [r3]\n"
                "\tsubs r0, 0x20\n"
                "\tlsls r0, 1\n"
                "\tadds r2, r0, r5\n"
                "\tldrh r1, [r2]\n"
                "\tcmp r1, r4\n"
                "\tbeq _08110D76\n"
                "\tmovs r0, 0x8\n"
                "\torrs r0, r1\n"
                "\tstrh r0, [r2]\n"
                "_08110D76:\n"
                "\tldrh r0, [r7]\n"
                "\tcmp r0, 0x13\n"
                "\tbeq _08110D90\n"
                "\tldrh r0, [r3]\n"
                "\tadds r0, 0x20\n"
                "\tlsls r0, 1\n"
                "\tadds r2, r0, r5\n"
                "\tldrh r1, [r2]\n"
                "\tcmp r1, r4\n"
                "\tbeq _08110D90\n"
                "\tmovs r0, 0x4\n"
                "\torrs r0, r1\n"
                "\tstrh r0, [r2]\n"
                "_08110D90:\n"
                "\tldrh r0, [r6]\n"
                "\tcmp r0, 0\n"
                "\tbeq _08110DB0\n"
                "\tldrh r0, [r7]\n"
                "\tcmp r0, 0\n"
                "\tbeq _08110DB0\n"
                "\tldrh r0, [r3]\n"
                "\tsubs r0, 0x21\n"
                "\tlsls r0, 1\n"
                "\tadds r2, r0, r5\n"
                "\tldrh r1, [r2]\n"
                "\tcmp r1, r4\n"
                "\tbeq _08110DB0\n"
                "\tmovs r0, 0x10\n"
                "\torrs r0, r1\n"
                "\tstrh r0, [r2]\n"
                "_08110DB0:\n"
                "\tldrh r0, [r6]\n"
                "\tcmp r0, 0x1F\n"
                "\tbeq _08110DD0\n"
                "\tldrh r0, [r7]\n"
                "\tcmp r0, 0\n"
                "\tbeq _08110DD0\n"
                "\tldrh r0, [r3]\n"
                "\tsubs r0, 0x1F\n"
                "\tlsls r0, 1\n"
                "\tadds r2, r0, r5\n"
                "\tldrh r1, [r2]\n"
                "\tcmp r1, r4\n"
                "\tbeq _08110DD0\n"
                "\tmovs r0, 0x40\n"
                "\torrs r0, r1\n"
                "\tstrh r0, [r2]\n"
                "_08110DD0:\n"
                "\tldrh r0, [r6]\n"
                "\tcmp r0, 0\n"
                "\tbeq _08110DF0\n"
                "\tldrh r0, [r7]\n"
                "\tcmp r0, 0x13\n"
                "\tbeq _08110DF0\n"
                "\tldrh r0, [r3]\n"
                "\tadds r0, 0x1F\n"
                "\tlsls r0, 1\n"
                "\tadds r2, r0, r5\n"
                "\tldrh r1, [r2]\n"
                "\tcmp r1, r4\n"
                "\tbeq _08110DF0\n"
                "\tmovs r0, 0x20\n"
                "\torrs r0, r1\n"
                "\tstrh r0, [r2]\n"
                "_08110DF0:\n"
                "\tldrh r0, [r6]\n"
                "\tcmp r0, 0x1F\n"
                "\tbeq _08110E10\n"
                "\tldrh r0, [r7]\n"
                "\tcmp r0, 0x13\n"
                "\tbeq _08110E10\n"
                "\tldrh r0, [r3]\n"
                "\tadds r0, 0x21\n"
                "\tlsls r0, 1\n"
                "\tadds r2, r0, r5\n"
                "\tldrh r1, [r2]\n"
                "\tcmp r1, r4\n"
                "\tbeq _08110E10\n"
                "\tmovs r0, 0x80\n"
                "\torrs r0, r1\n"
                "\tstrh r0, [r2]\n"
                "_08110E10:\n"
                "\tldrh r0, [r3]\n"
                "\tadds r0, 0x1\n"
                "\tstrh r0, [r3]\n"
                "\tldrh r0, [r6]\n"
                "\tadds r0, 0x1\n"
                "\tstrh r0, [r6]\n"
                "\tlsls r0, 16\n"
                "\tlsrs r0, 16\n"
                "\tcmp r0, 0x1F\n"
                "\tbhi _08110E26\n"
                "\tb _08110D1E\n"
                "_08110E26:\n"
                "\tldrh r0, [r7]\n"
                "\tadds r0, 0x1\n"
                "\tstrh r0, [r7]\n"
                "\tlsls r0, 16\n"
                "\tlsrs r0, 16\n"
                "\tcmp r0, 0x13\n"
                "\tbhi _08110E36\n"
                "\tb _08110D1A\n"
                "_08110E36:\n"
                "\tmovs r0, 0\n"
                "\tmov r1, r9\n"
                "\tstrh r0, [r1]\n"
                "_08110E3C:\n"
                "\tmov r3, r9\n"
                "\tldrh r0, [r3]\n"
                "\tlsls r0, 1\n"
                "\tmovs r4, 0x8B\n"
                "\tlsls r4, 1\n"
                "\tadd r4, r8\n"
                "\tadds r3, r0, r4\n"
                "\tldrh r2, [r3]\n"
                "\tadds r1, r2, 0\n"
                "\tldr r0, _08110E74 @ =0x0000ffff\n"
                "\tcmp r1, r0\n"
                "\tbne _08110E80\n"
                "\tmovs r0, 0x10\n"
                "\tstrh r0, [r3]\n"
                "\tb _0811105E\n"
                "\t.align 2, 0\n"
                "_08110E5C: .4byte gPokedexAreaGlowIndex\n"
                "_08110E60: .4byte gSharedMem\n"
                "_08110E64: .4byte gPokedexAreaGlowX\n"
                "_08110E68: .4byte 0x0000027f\n"
                "_08110E6C: .4byte gPokedexAreaGlowTilemapIndex\n"
                "_08110E70: .4byte gPokedexAreaGlowY\n"
                "_08110E74: .4byte 0x0000ffff\n"
                "_08110E78: .4byte gSharedMem + 0x110\n"
                "_08110E7C: .4byte gSharedMem + 0x116\n"
                "_08110E80:\n"
                "\tcmp r1, 0\n"
                "\tbne _08110E86\n"
                "\tb _0811105E\n"
                "_08110E86:\n"
                "\tmovs r0, 0x2\n"
                "\tands r0, r2\n"
                "\tcmp r0, 0\n"
                "\tbeq _08110E94\n"
                "\tldr r0, _08110F28 @ =0x0000ffcf\n"
                "\tands r0, r2\n"
                "\tstrh r0, [r3]\n"
                "_08110E94:\n"
                "\tmov r1, r9\n"
                "\tldrh r0, [r1]\n"
                "\tlsls r0, 1\n"
                "\tadds r2, r0, r4\n"
                "\tldrh r1, [r2]\n"
                "\tmovs r0, 0x1\n"
                "\tands r0, r1\n"
                "\tcmp r0, 0\n"
                "\tbeq _08110EAC\n"
                "\tldr r0, _08110F2C @ =0x0000ff3f\n"
                "\tands r0, r1\n"
                "\tstrh r0, [r2]\n"
                "_08110EAC:\n"
                "\tmov r3, r9\n"
                "\tldrh r0, [r3]\n"
                "\tlsls r0, 1\n"
                "\tadds r2, r0, r4\n"
                "\tldrh r1, [r2]\n"
                "\tmovs r0, 0x8\n"
                "\tands r0, r1\n"
                "\tcmp r0, 0\n"
                "\tbeq _08110EC4\n"
                "\tldr r0, _08110F30 @ =0x0000ffaf\n"
                "\tands r0, r1\n"
                "\tstrh r0, [r2]\n"
                "_08110EC4:\n"
                "\tmov r1, r9\n"
                "\tldrh r0, [r1]\n"
                "\tlsls r0, 1\n"
                "\tadds r2, r0, r4\n"
                "\tldrh r1, [r2]\n"
                "\tmovs r0, 0x4\n"
                "\tands r0, r1\n"
                "\tcmp r0, 0\n"
                "\tbeq _08110EDC\n"
                "\tldr r0, _08110F34 @ =0x0000ff5f\n"
                "\tands r0, r1\n"
                "\tstrh r0, [r2]\n"
                "_08110EDC:\n"
                "\tldr r6, _08110F38 @ =gPokedexAreaGlowEdgeFlags\n"
                "\tmov r3, r9\n"
                "\tldrh r0, [r3]\n"
                "\tlsls r0, 1\n"
                "\tadds r0, r4\n"
                "\tldrh r1, [r0]\n"
                "\tmovs r5, 0xF\n"
                "\tadds r0, r5, 0\n"
                "\tands r0, r1\n"
                "\tstrh r0, [r6]\n"
                "\tldr r2, _08110F3C @ =gPokedexAreaGlowCornerFlags\n"
                "\tldrh r0, [r3]\n"
                "\tlsls r0, 1\n"
                "\tadds r0, r4\n"
                "\tldrh r1, [r0]\n"
                "\tmovs r0, 0xF0\n"
                "\tands r0, r1\n"
                "\tstrh r0, [r2]\n"
                "\tldrh r1, [r3]\n"
                "\tadds r3, r2, 0\n"
                "\tcmp r0, 0\n"
                "\tbne _08110F0A\n"
                "\tb _0811105E\n"
                "_08110F0A:\n"
                "\tlsls r0, r1, 1\n"
                "\tadds r0, r4\n"
                "\tldrh r2, [r0]\n"
                "\tadds r1, r5, 0\n"
                "\tands r1, r2\n"
                "\tstrh r1, [r0]\n"
                "\tldrh r0, [r6]\n"
                "\tcmp r0, 0xA\n"
                "\tbls _08110F1E\n"
                "\tb _0811105E\n"
                "_08110F1E:\n"
                "\tlsls r0, 2\n"
                "\tldr r1, _08110F40 @ =_08110F44\n"
                "\tadds r0, r1\n"
                "\tldr r0, [r0]\n"
                "\tmov pc, r0\n"
                "\t.align 2, 0\n"
                "_08110F28: .4byte 0x0000ffcf\n"
                "_08110F2C: .4byte 0x0000ff3f\n"
                "_08110F30: .4byte 0x0000ffaf\n"
                "_08110F34: .4byte 0x0000ff5f\n"
                "_08110F38: .4byte gPokedexAreaGlowEdgeFlags\n"
                "_08110F3C: .4byte gPokedexAreaGlowCornerFlags\n"
                "_08110F40: .4byte _08110F44\n"
                "\t.align 2, 0\n"
                "_08110F44:\n"
                "\t.4byte _08110F70\n"
                "\t.4byte _08110FAC\n"
                "\t.4byte _08110F8E\n"
                "\t.4byte _0811105E\n"
                "\t.4byte _08111002\n"
                "\t.4byte _08111036\n"
                "\t.4byte _08111036\n"
                "\t.4byte _0811105E\n"
                "\t.4byte _08110FCE\n"
                "\t.4byte _0811104A\n"
                "\t.4byte _0811104A\n"
                "_08110F70:\n"
                "\tldrh r0, [r3]\n"
                "\tlsls r1, r0, 16\n"
                "\tcmp r1, 0\n"
                "\tbeq _0811105E\n"
                "\tmov r0, r9\n"
                "\tldrh r2, [r0]\n"
                "\tlsls r2, 1\n"
                "\tmovs r0, 0x8B\n"
                "\tlsls r0, 1\n"
                "\tadd r0, r8\n"
                "\tadds r2, r0\n"
                "\tldrh r0, [r2]\n"
                "\tadds r0, 0x10\n"
                "\tlsrs r1, 20\n"
                "\tb _08110FC8\n"
                "_08110F8E:\n"
                "\tldrh r0, [r3]\n"
                "\tlsls r1, r0, 16\n"
                "\tcmp r1, 0\n"
                "\tbeq _0811105E\n"
                "\tmov r3, r9\n"
                "\tldrh r2, [r3]\n"
                "\tlsls r2, 1\n"
                "\tmovs r0, 0x8B\n"
                "\tlsls r0, 1\n"
                "\tadd r0, r8\n"
                "\tadds r2, r0\n"
                "\tldrh r0, [r2]\n"
                "\tadds r0, 0x1E\n"
                "\tlsrs r1, 20\n"
                "\tb _08110FC8\n"
                "_08110FAC:\n"
                "\tldrh r0, [r3]\n"
                "\tlsls r1, r0, 16\n"
                "\tcmp r1, 0\n"
                "\tbeq _0811105E\n"
                "\tmov r0, r9\n"
                "\tldrh r2, [r0]\n"
                "\tlsls r2, 1\n"
                "\tmovs r0, 0x8B\n"
                "\tlsls r0, 1\n"
                "\tadd r0, r8\n"
                "\tadds r2, r0\n"
                "\tldrh r0, [r2]\n"
                "\tadds r0, 0x20\n"
                "\tlsrs r1, 22\n"
                "_08110FC8:\n"
                "\tadds r0, r1\n"
                "\tstrh r0, [r2]\n"
                "\tb _0811105E\n"
                "_08110FCE:\n"
                "\tldrh r2, [r3]\n"
                "\tcmp r2, 0\n"
                "\tbeq _0811105E\n"
                "\tmovs r0, 0x80\n"
                "\tands r0, r2\n"
                "\tlsls r0, 16\n"
                "\tlsrs r0, 16\n"
                "\tnegs r0, r0\n"
                "\tlsrs r3, r0, 31\n"
                "\tmovs r0, 0x20\n"
                "\tands r0, r2\n"
                "\tcmp r0, 0\n"
                "\tbeq _08110FEC\n"
                "\tmovs r0, 0x2\n"
                "\torrs r3, r0\n"
                "_08110FEC:\n"
                "\tmov r0, r9\n"
                "\tldrh r1, [r0]\n"
                "\tlsls r1, 1\n"
                "\tmovs r0, 0x8B\n"
                "\tlsls r0, 1\n"
                "\tadd r0, r8\n"
                "\tadds r1, r0\n"
                "\tldrh r0, [r1]\n"
                "\tadds r0, 0x20\n"
                "\tadds r0, r3\n"
                "\tb _0811105C\n"
                "_08111002:\n"
                "\tldrh r2, [r3]\n"
                "\tcmp r2, 0\n"
                "\tbeq _0811105E\n"
                "\tmovs r0, 0x40\n"
                "\tands r0, r2\n"
                "\tlsls r0, 16\n"
                "\tlsrs r0, 16\n"
                "\tnegs r0, r0\n"
                "\tlsrs r3, r0, 31\n"
                "\tmovs r0, 0x10\n"
                "\tands r0, r2\n"
                "\tcmp r0, 0\n"
                "\tbeq _08111020\n"
                "\tmovs r0, 0x2\n"
                "\torrs r3, r0\n"
                "_08111020:\n"
                "\tmov r0, r9\n"
                "\tldrh r1, [r0]\n"
                "\tlsls r1, 1\n"
                "\tmovs r0, 0x8B\n"
                "\tlsls r0, 1\n"
                "\tadd r0, r8\n"
                "\tadds r1, r0\n"
                "\tldrh r0, [r1]\n"
                "\tadds r0, 0x21\n"
                "\tadds r0, r3\n"
                "\tb _0811105C\n"
                "_08111036:\n"
                "\tmov r3, r9\n"
                "\tldrh r1, [r3]\n"
                "\tlsls r1, 1\n"
                "\tmovs r0, 0x8B\n"
                "\tlsls r0, 1\n"
                "\tadd r0, r8\n"
                "\tadds r1, r0\n"
                "\tldrh r0, [r1]\n"
                "\tadds r0, 0x27\n"
                "\tb _0811105C\n"
                "_0811104A:\n"
                "\tmov r0, r9\n"
                "\tldrh r1, [r0]\n"
                "\tlsls r1, 1\n"
                "\tmovs r0, 0x8B\n"
                "\tlsls r0, 1\n"
                "\tadd r0, r8\n"
                "\tadds r1, r0\n"
                "\tldrh r0, [r1]\n"
                "\tadds r0, 0x25\n"
                "_0811105C:\n"
                "\tstrh r0, [r1]\n"
                "_0811105E:\n"
                "\tmov r3, r9\n"
                "\tldrh r0, [r3]\n"
                "\tadds r0, 0x1\n"
                "\tstrh r0, [r3]\n"
                "\tlsls r0, 16\n"
                "\tldr r1, _08111080 @ =0x027f0000\n"
                "\tcmp r0, r1\n"
                "\tbhi _08111070\n"
                "\tb _08110E3C\n"
                "_08111070:\n"
                "\tadd sp, 0xC\n"
                "\tpop {r3-r5}\n"
                "\tmov r8, r3\n"
                "\tmov r9, r4\n"
                "\tmov r10, r5\n"
                "\tpop {r4-r7}\n"
                "\tpop {r0}\n"
                "\tbx r0\n"
                "\t.align 2, 0\n"
                "_08111080: .4byte 0x027f0000");
}
#endif // NONMATCHING

static void StartAreaGlow(void)
{
    if (sPokedexAreaScreen->numSpecialAreas != 0 && sPokedexAreaScreen->numOverworldAreas == 0)
        sPokedexAreaScreen->showingMarkers = 1;
    else
        sPokedexAreaScreen->showingMarkers = 0;
    sPokedexAreaScreen->markerTimer = 0;
    sPokedexAreaScreen->glowTimer = 0;
    sPokedexAreaScreen->areaShadeBldArgLo = 0;
    sPokedexAreaScreen->areaShadeBldArgHi = 0x40;
    sPokedexAreaScreen->markerFlashCounter = 1;
    REG_BLDCNT = BLDCNT_TGT1_BG0 | BLDCNT_EFFECT_BLEND | BLDCNT_TGT2_BG0 | BLDCNT_TGT2_BG1 | BLDCNT_TGT2_BG2 | BLDCNT_TGT2_BG3 | BLDCNT_TGT2_OBJ | BLDCNT_TGT2_BD;
    REG_BLDALPHA = 0x1000;
    DoAreaGlow();
}

static void DoAreaGlow(void)
{
    u16 x;
    u16 y;
    u16 i;

    if (sPokedexAreaScreen->showingMarkers == 0)
    {
        if (sPokedexAreaScreen->markerTimer == 0)
        {
            sPokedexAreaScreen->glowTimer++;
            if (sPokedexAreaScreen->glowTimer & 1)
                sPokedexAreaScreen->areaShadeBldArgLo = (sPokedexAreaScreen->areaShadeBldArgLo + 4) & 0x7f;
            else
                sPokedexAreaScreen->areaShadeBldArgHi = (sPokedexAreaScreen->areaShadeBldArgHi + 4) & 0x7f;
            x = gSineTable[sPokedexAreaScreen->areaShadeBldArgLo] >> 4;
            y = gSineTable[sPokedexAreaScreen->areaShadeBldArgHi] >> 4;
            REG_BLDALPHA = x | (y << 8);
            sPokedexAreaScreen->markerTimer = 0;
            if (sPokedexAreaScreen->glowTimer == 0x40)
            {
                sPokedexAreaScreen->glowTimer = 0;
                if (sPokedexAreaScreen->numSpecialAreas != 0)
                    sPokedexAreaScreen->showingMarkers = 1;
            }
        }
        else
            sPokedexAreaScreen->markerTimer--;
    }
    else
    {
        sPokedexAreaScreen->markerTimer++;
        if (sPokedexAreaScreen->markerTimer > 12)
        {
            sPokedexAreaScreen->markerTimer = 0;
            sPokedexAreaScreen->markerFlashCounter++;
            for (i = 0; i < sPokedexAreaScreen->numSpecialAreas; i++)
            {
                sPokedexAreaScreen->areaMarkerSprites[i]->invisible = sPokedexAreaScreen->markerFlashCounter & 1;
            }
            if (sPokedexAreaScreen->markerFlashCounter > 4)
            {
                sPokedexAreaScreen->markerFlashCounter = 1;
                if (sPokedexAreaScreen->numOverworldAreas != 0)
                    sPokedexAreaScreen->showingMarkers = 0;
            }
        }
    }
}

static void DebugCB_WaitFade(void)
{
    switch (sPokedexAreaScreen->state)
    {
        case 0:
            sPokedexAreaScreen->state = 1;
            break;
        case 1:
            if (!UpdatePaletteFade())
            {
                SetPokedexAreaScreenCallback(DebugCB_WaitButton);
            }
            break;
    }
}

static void DebugCB_WaitButton(void)
{
    switch (sPokedexAreaScreen->state)
    {
        case 0:
            if (JOY_NEW(B_BUTTON))
            {
                SetPokedexAreaScreenCallback(DebugCB_GoBack);
            }
            else if (JOY_NEW(DPAD_RIGHT) || (JOY_NEW(R_BUTTON) && gSaveBlock2.optionsButtonMode == OPTIONS_BUTTON_MODE_LR))
            {
                SetPokedexAreaScreenCallback(DebugCB_GoNext);
            }
            break;
    }
}

static void DebugCB_GoBack(void)
{
    switch (sPokedexAreaScreen->state)
    {
        case 0:
            BeginNormalPaletteFade(0xFFFFFFEB, 0, 0, 16, RGB(0, 0, 0));
            sPokedexAreaScreen->state++;
            break;
        case 1:
            if (!UpdatePaletteFade())
            {
                FreeRegionMapIconResources();
                SetMainCallback2(sPokedexAreaScreen->prev);
            }
            break;
    }
}

static void DebugCB_GoNext(void)
{
    switch (sPokedexAreaScreen->state)
    {
        case 0:
            BeginNormalPaletteFade(0xFFFFFFEB, 0, 0, 16, RGB(0, 0, 0));
            sPokedexAreaScreen->state++;
            break;
        case 1:
            if (!UpdatePaletteFade())
            {
                FreeRegionMapIconResources();
                SetMainCallback2(sPokedexAreaScreen->next);
            }
            break;
    }
}

static void Task_ShowPokedexAreaScreen(u8 taskId);
static void Task_HandlePokedexAreaScreenInput(u8 taskId);
static void CreateAreaMarkerSprites(void);
static void DestroyAreaScreenSprites(void);
static void LoadAreaUnknownGraphics(void);
static void CreateAreaUnknownSprites(void);

#define tState      data[0]
#define tExitAction data[1]

void ShowPokedexAreaScreen(u16 species, u8 *screenSwitchState)
{
    u8 taskId;
    sPokedexAreaScreen->species = species;
    sPokedexAreaScreen->screenSwitchState = screenSwitchState;
    screenSwitchState[0] = 0;
    taskId = CreateTask(Task_ShowPokedexAreaScreen, 0);
    gTasks[taskId].tState = 0;
}

static void Task_ShowPokedexAreaScreen(u8 taskId)
{
    switch (gTasks[taskId].tState)
    {
        case 0:
            REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_BG1_ON | DISPCNT_WIN0_ON | DISPCNT_WIN1_ON;
            REG_BG0HOFS = 0;
            REG_BG0VOFS = -8;
            REG_BG2VOFS = 0;
            REG_BG2HOFS = 0;
            REG_BG3HOFS = 0;
            REG_BG3VOFS = 0;
            break;
        case 1:
            ResetPaletteFade();
            ResetSpriteData();
            FreeAllSpritePalettes();
            break;
        case 2:
            InitRegionMap(&sPokedexAreaScreen->regionMap, FALSE);
            StringFill(sPokedexAreaScreen->charBuffer, CHAR_SPACE, 16);
            break;
        case 3:
            ResetDrawAreaGlowState();
            break;
        case 4:
            if (DrawAreaGlow())
                return;
            break;
        case 5:
            CreateRegionMapPlayerIcon(1, 1);
            PokedexAreaScreen_UpdateRegionMapVariablesAndVideoRegs(0, -8);
            break;
        case 6:
            CreateAreaMarkerSprites();
            break;
        case 7:
            LoadAreaUnknownGraphics();
            break;
        case 8:
            CreateAreaUnknownSprites();
            break;
        case 9:
            BeginNormalPaletteFade(0xFFFFFFEB, 0, 16, 0, RGB(0, 0, 0));
            break;
        case 10:
            REG_BLDCNT = BLDCNT_TGT1_BG0 | BLDCNT_EFFECT_BLEND | BLDCNT_TGT2_BG0 | BLDCNT_TGT2_BG1 | BLDCNT_TGT2_BG2 | BLDCNT_TGT2_BG3 | BLDCNT_TGT2_OBJ | BLDCNT_TGT2_BD;
            StartAreaGlow();
            REG_DISPCNT = DISPCNT_MODE_1 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG1_ON | DISPCNT_BG2_ON | DISPCNT_OBJ_ON | DISPCNT_WIN0_ON | DISPCNT_WIN1_ON;
            break;
        case 11:
            gTasks[taskId].func = Task_HandlePokedexAreaScreenInput;
            gTasks[taskId].tState = 0;
            return;
    }
    gTasks[taskId].tState++;
}

static void Task_HandlePokedexAreaScreenInput(u8 taskId)
{
    DoAreaGlow();
    switch (gTasks[taskId].tState)
    {
        default:
            gTasks[taskId].tState = 0;
        case 0:
            if (gPaletteFade.active)
                return;
            break;
        case 1:
            if (JOY_NEW(B_BUTTON))
            {
                gTasks[taskId].tExitAction = 1;
                PlaySE(SE_PC_OFF);
            }
            else if (JOY_NEW(DPAD_RIGHT) || (JOY_NEW(R_BUTTON) && gSaveBlock2.optionsButtonMode == OPTIONS_BUTTON_MODE_LR))
            {
                gTasks[taskId].tExitAction = 2;
                PlaySE(SE_DEX_PAGE);
            }
            else
                return;
            break;
        case 2:
            BeginNormalPaletteFade(0xFFFFFFEB, 0, 0, 16, RGB(0, 0, 0));
            break;
        case 3:
            if (gPaletteFade.active)
                return;
            DestroyAreaScreenSprites();
            sPokedexAreaScreen->screenSwitchState[0] = gTasks[taskId].tExitAction;
            DestroyTask(taskId);
            return;
    }
    gTasks[taskId].tState++;
}

#undef tState
#undef tExitAction

static const u16 sAreaMarkerPalette[];
static const u8 sAreaMarkerTiles[];

static const struct SpriteSheet sAreaMarkerSpriteSheet = {
    sAreaMarkerTiles, 0x80, TAG_AREA_MARKER
};

static const struct SpritePalette sAreaMarkerSpritePalette = {
    sAreaMarkerPalette, TAG_AREA_MARKER
};

static const struct OamData sAreaMarkerOamData = {
    .size = 1,
    .priority = 1
};

static const struct SpriteTemplate sAreaMarkerSpriteTemplate = {
    TAG_AREA_MARKER,
    TAG_AREA_MARKER,
    &sAreaMarkerOamData,
    gDummySpriteAnimTable,
    NULL,
    gDummySpriteAffineAnimTable,
    SpriteCallbackDummy
};

static const u16 sAreaMarkerPalette[] = INCBIN_U16("graphics/pokedex/area_marker.gbapal");
static const u8 sAreaMarkerTiles[] = INCBIN_U8("graphics/pokedex/area_marker.4bpp");

static void CreateAreaMarkerSprites(void)
{
    s16 i;
    u8 spriteId;
    s16 x;
    s16 y;
    s16 numSprites;
    s16 mapSecId;

    LoadSpriteSheet(&sAreaMarkerSpriteSheet);
    LoadSpritePalette(&sAreaMarkerSpritePalette);
    numSprites = 0;
    for (i = 0; i < sPokedexAreaScreen->numSpecialAreas; i++)
    {
        mapSecId = sPokedexAreaScreen->specialAreaRegionMapSectionIds[i];
        x = 8 * (gRegionMapEntries[mapSecId].x + 1) + 4;
        y = 8 * (gRegionMapEntries[mapSecId].y) + 28;
        x += 4 * (gRegionMapEntries[mapSecId].width - 1);
        y += 4 * (gRegionMapEntries[mapSecId].height - 1);
        spriteId = CreateSprite(&sAreaMarkerSpriteTemplate, x, y, 0);
        if (spriteId != MAX_SPRITES)
        {
            gSprites[spriteId].invisible = TRUE;
            sPokedexAreaScreen->areaMarkerSprites[numSprites++] = gSprites + spriteId;
        }
    }
    sPokedexAreaScreen->numAreaMarkerSprites = numSprites;
}

static void DestroyAreaScreenSprites(void)
{
    u16 i;
    FreeSpriteTilesByTag(TAG_AREA_MARKER);
    FreeSpritePaletteByTag(TAG_AREA_MARKER);
    for (i = 0; i < sPokedexAreaScreen->numAreaMarkerSprites; i++)
    {
        DestroySprite(sPokedexAreaScreen->areaMarkerSprites[i]);
    }
    FreeSpriteTilesByTag(TAG_AREA_UNKNOWN);
    FreeSpritePaletteByTag(TAG_AREA_UNKNOWN);
    for (i = 0; i < 3; i++)
    {
        if (sPokedexAreaScreen->areaUnknownSprites[i] != NULL)
        {
            DestroySprite(sPokedexAreaScreen->areaUnknownSprites[i]);
        }
    }
}

static const struct SpritePalette sAreaUnknownSpritePalette = {gPokedexAreaScreenAreaUnknown_Pal, TAG_AREA_UNKNOWN};

static void LoadAreaUnknownGraphics(void)
{
    struct SpriteSheet spriteSheet = {sPokedexAreaScreen->areaUnknownGraphicsBuffer, 0x600, TAG_AREA_UNKNOWN};

    LZ77UnCompWram(gPokedexAreaScreenAreaUnknown_Gfx, sPokedexAreaScreen->areaUnknownGraphicsBuffer);
    LoadSpriteSheet(&spriteSheet);
    LoadSpritePalette(&sAreaUnknownSpritePalette);
}

static const struct OamData sAreaUnknownOamData = {
    .size = 2,
    .priority = 1
};

static const struct SpriteTemplate sAreaUnknownSpriteTemplate = {
    TAG_AREA_UNKNOWN,
    TAG_AREA_UNKNOWN,
    &sAreaUnknownOamData,
    gDummySpriteAnimTable,
    NULL,
    gDummySpriteAffineAnimTable,
    SpriteCallbackDummy
};

static void CreateAreaUnknownSprites(void)
{
    u16 i;
    u8 spriteId;

    if (sPokedexAreaScreen->numOverworldAreas != 0 || sPokedexAreaScreen->numSpecialAreas != 0)
    {
        for (i = 0; i < 3; i++)
        {
            sPokedexAreaScreen->areaUnknownSprites[i] = NULL;
        }
    }
    else
    {
        for (i = 0; i < 3; i++)
        {
            spriteId = CreateSprite(&sAreaUnknownSpriteTemplate, i * 32 + 0xa0, 0x8c, 0);
            if (spriteId != MAX_SPRITES)
            {
                gSprites[spriteId].oam.tileNum += i * 16;
                sPokedexAreaScreen->areaUnknownSprites[i] = gSprites + spriteId;
            }
            else
                sPokedexAreaScreen->areaUnknownSprites[i] = NULL;
        }
    }
}
