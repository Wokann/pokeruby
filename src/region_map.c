#include "global.h"
#include "constants/region_map_sections.h"
#include "event_data.h"
#include "field_effect.h"
#include "field_specials.h"
#include "m4a.h"
#include "main.h"
#include "constants/maps.h"
#include "menu.h"
#include "palette.h"
#include "party_menu.h"
#include "pokemon_menu.h"
#include "region_map.h"
#include "overworld.h"
#include "secret_base.h"
#include "constants/songs.h"
#include "sprite.h"
#include "strings.h"
#include "string_util.h"
#include "text.h"
#include "trig.h"
#include "ewram.h"
#include "heal_location.h"
#include "constants/heal_locations.h"

#define MAP_WIDTH 28
#define MAP_HEIGHT 15
#define MAPCURSOR_X_MIN 1
#define MAPCURSOR_Y_MIN 2
#define MAPCURSOR_X_MAX (MAPCURSOR_X_MIN + MAP_WIDTH - 1)
#define MAPCURSOR_Y_MAX (MAPCURSOR_Y_MIN + MAP_HEIGHT - 1)

// Input events
static EWRAM_DATA struct RegionMap *sRegionMap = NULL;

static const u16 sRegionMapCursorPal[] = INCBIN_U16("graphics/pokenav/region_map/cursor.gbapal");
static const u8 sRegionMapCursorSmallGfxLZ[] = INCBIN_U8("graphics/pokenav/region_map/cursor_small.4bpp.lz");
static const u8 sRegionMapCursorLargeGfxLZ[] = INCBIN_U8("graphics/pokenav/region_map/cursor_large.4bpp.lz");
static const u16 sRegionMapPlayerIcon_BrendanPal[] = INCBIN_U16("graphics/pokenav/region_map/brendan_icon.gbapal");
static const u8 sRegionMapPlayerIcon_BrendanGfx[] = INCBIN_U8("graphics/pokenav/region_map/brendan_icon.4bpp");
static const u16 sRegionMapPlayerIcon_MayPal[] = INCBIN_U16("graphics/pokenav/region_map/may_icon.gbapal");
static const u8 sRegionMapPlayerIcon_MayGfx[] = INCBIN_U8("graphics/pokenav/region_map/may_icon.4bpp");
static const u16 sRegionMapBg_Pal[] = INCBIN_U16("graphics/pokenav/region_map/map.gbapal");
static const u8 sRegionMapBg_GfxLZ[] = INCBIN_U8("graphics/pokenav/region_map/map.8bpp.lz");
static const u8 sRegionMapBg_TilemapLZ[] = INCBIN_U8("graphics/pokenav/region_map/map.bin.lz");

#include "data/region_map/region_map_layout.h"

#if ENGLISH
#include "data/region_map/region_map_entries.h"
#elif GERMAN
#include "data/region_map/region_map_entries_de.h"
#endif

static const mapsec_u16_t sUnderwaterMaps[][2] =
{
    {MAPSEC_UNDERWATER_124,       MAPSEC_ROUTE_124},
    {MAPSEC_UNDERWATER_126,       MAPSEC_ROUTE_126},
    {MAPSEC_UNDERWATER_127,       MAPSEC_ROUTE_127},
    {MAPSEC_UNDERWATER_128,       MAPSEC_ROUTE_128},
    {MAPSEC_UNDERWATER_SOOTOPOLIS,       MAPSEC_SOOTOPOLIS_CITY},
    {MAPSEC_UNDERWATER_SEAFLOOR_CAVERN,       MAPSEC_ROUTE_128},
    {MAPSEC_EVIL_TEAM_HIDEOUT, MAPSEC_LILYCOVE_CITY},
    {MAPSEC_UNDERWATER_SEALED_CHAMBER,       MAPSEC_ROUTE_134},
    {MAPSEC_PETALBURG_WOODS,   MAPSEC_ROUTE_104},
    {MAPSEC_JAGGED_PASS,     MAPSEC_ROUTE_112},
    {MAPSEC_MT_PYRE,           MAPSEC_ROUTE_122},
    {MAPSEC_SKY_PILLAR,        MAPSEC_ROUTE_131},
    {MAPSEC_NONE,              MAPSEC_NONE},
};

static u8 ProcessRegionMapInput_Full(void);
static u8 MoveRegionMapCursor_Full(void);
static u8 ProcessRegionMapInput_Zoomed(void);
static u8 MoveRegionMapCursor_Zoomed(void);
static void CalcZoomScrollParams(s16 scrollX, s16 scrollY, s16 c, s16 d, u16 e, u16 f, u8 rotation);
static void RegionMap_SetBG2XAndBG2Y(s16 x, s16 y);
static mapsec_u16_t GetMapSecIdAt(u16 x, u16 y);
static void InitializeCursorPosition(void);
static void RegionMap_InitializeStateBasedOnSSTidalLocation(void);
static u16 sub_80FB758(mapsec_u16_t mapSecId);
static mapsec_u16_t GetOverworldMapFromUnderwaterMap_(mapsec_u16_t mapSecId);
static void GetPositionOfCursorWithinMapSec(void);
static bool8 RegionMap_IsMapSecIdInNextRow(u16);
void FreeRegionMapCursorSprite(void);
static void HideRegionMapPlayerIcon(void);
static void UnhideRegionMapPlayerIcon(void);
static void SpriteCB_PlayerIconMapFull(struct Sprite *sprite);
static void SpriteCB_PlayerIcon(struct Sprite *sprite);
static void SpriteCB_PlayerIconMapZoomed(struct Sprite *sprite);
static void VBlankCB_FlyMap(void);
void CB2_FlyRegionMap(void);
void SetFlyMapCallback(void (*callback)(void));
static void PrintFlyTargetName(void);
static void LoadFlyDestIcons(void);
static void CreateFlyDestIcons(void);
static void TryCreateRedOutlineFlyDestIcons(void);
static void SpriteCB_FlyDestIcon(struct Sprite *sprite);
static void CB_FadeInFlyMap(void);
static void CB_HandleFlyMapInput(void);
void sub_80FC69C(void);

void InitRegionMap(struct RegionMap *regionMap, bool8 zoomed)
{
    sub_80FA904(regionMap, zoomed);
    while (sub_80FA940())
        ;
}

void sub_80FA904(struct RegionMap *regionMap, bool8 zoomed)
{
    sRegionMap = regionMap;
    sRegionMap->initStep = 0;
    sRegionMap->zoomed = zoomed;
    sRegionMap->inputCallback = (zoomed == 0) ? ProcessRegionMapInput_Full : ProcessRegionMapInput_Zoomed;
}

bool8 sub_80FA940(void)
{
    switch (sRegionMap->initStep)
    {
    case 0:
        LZ77UnCompVram(sRegionMapBg_GfxLZ, (void *)(VRAM + 0x8000));
        break;
    case 1:
        LZ77UnCompVram(sRegionMapBg_TilemapLZ, (void *)(VRAM + 0xE000));
        break;
    case 2:
        LoadPalette(sRegionMapBg_Pal, 0x70, 0x60);  // Why isn't this the right size?
        break;
    case 3:
        LZ77UnCompWram(sRegionMapCursorSmallGfxLZ, sRegionMap->cursorSmallImage);
        break;
    case 4:
        LZ77UnCompWram(sRegionMapCursorLargeGfxLZ, sRegionMap->cursorLargeImage);
        break;
    case 5:
        InitializeCursorPosition();
        sRegionMap->playerIconSpritePosX = sRegionMap->cursorPosX;
        sRegionMap->playerIconSpritePosY = sRegionMap->cursorPosY;
        sRegionMap->mapSecType = sub_80FB758(sRegionMap->mapSecId);
        sRegionMap->mapSecId = GetOverworldMapFromUnderwaterMap_(sRegionMap->mapSecId);
        GetMapSectionName(sRegionMap->mapSecName, sRegionMap->mapSecId, 16);
        break;
    case 6:
        if (sRegionMap->zoomed == FALSE)
        {
            CalcZoomScrollParams(0, 0, 0, 0, 0x100, 0x100, 0);
        }
        else
        {
            sRegionMap->scrollX = sRegionMap->cursorPosX * 8 - 52;
            sRegionMap->scrollY = sRegionMap->cursorPosY * 8 - 68;
            sRegionMap->zoomedCursorPosX = sRegionMap->cursorPosX;
            sRegionMap->zoomedCursorPosY = sRegionMap->cursorPosY;
            CalcZoomScrollParams(sRegionMap->scrollX, sRegionMap->scrollY, 0x38, 0x48, 0x80, 0x80, 0);
        }
        break;
    case 7:
        GetPositionOfCursorWithinMapSec();
        UpdateRegionMapVideoRegs();
        sRegionMap->cursorSprite = NULL;
        sRegionMap->playerIconSprite = NULL;
        sRegionMap->cursorMovementFrameCounter = 0;
        sRegionMap->blinkPlayerIcon = FALSE;
        REG_BG2CNT = 0xBC8A;
        sRegionMap->initStep++;
    default:
        return FALSE;
    }
    sRegionMap->initStep++;
    return TRUE;
}

void FreeRegionMapIconResources(void)
{
    if (sRegionMap->cursorSprite != NULL)
    {
        DestroySprite(sRegionMap->cursorSprite);
        FreeSpriteTilesByTag(sRegionMap->cursorTileTag);
        FreeSpritePaletteByTag(sRegionMap->cursorPaletteTag);
    }
    if (sRegionMap->playerIconSprite != NULL)
    {
        DestroySprite(sRegionMap->playerIconSprite);
        FreeSpriteTilesByTag(sRegionMap->playerIconTileTag);
        FreeSpritePaletteByTag(sRegionMap->playerIconPaletteTag);
    }
}

u8 DoRegionMapInputCallback(void)
{
    return sRegionMap->inputCallback();
}

static u8 ProcessRegionMapInput_Full(void)
{
    u8 input = MAP_INPUT_NONE;

    sRegionMap->cursorDeltaX = 0;
    sRegionMap->cursorDeltaY = 0;
    if (JOY_HELD(DPAD_UP) && sRegionMap->cursorPosY > MAPCURSOR_Y_MIN)
    {
        sRegionMap->cursorDeltaY = -1;
        input = MAP_INPUT_MOVE_START;
    }
    if (JOY_HELD(DPAD_DOWN) && sRegionMap->cursorPosY < MAPCURSOR_Y_MAX)
    {
        sRegionMap->cursorDeltaY = 1;
        input = MAP_INPUT_MOVE_START;
    }
    if (JOY_HELD(DPAD_LEFT) && sRegionMap->cursorPosX > MAPCURSOR_X_MIN)
    {
        sRegionMap->cursorDeltaX = -1;
        input = MAP_INPUT_MOVE_START;
    }
    if (JOY_HELD(DPAD_RIGHT) && sRegionMap->cursorPosX < MAPCURSOR_X_MAX)
    {
        sRegionMap->cursorDeltaX = 1;
        input = MAP_INPUT_MOVE_START;
    }

    if (JOY_NEW(A_BUTTON))
        input = MAP_INPUT_A_BUTTON;
    else if (JOY_NEW(B_BUTTON))
        input = MAP_INPUT_B_BUTTON;

    if (input == MAP_INPUT_MOVE_START)
    {
        sRegionMap->cursorMovementFrameCounter = 4;
        sRegionMap->inputCallback = MoveRegionMapCursor_Full;
    }
    return input;
}

static u8 MoveRegionMapCursor_Full(void)
{
    mapsec_u16_t mapSecId;

    if (sRegionMap->cursorMovementFrameCounter != 0)
        return MAP_INPUT_MOVE_CONT;

    if (sRegionMap->cursorDeltaX > 0)
        sRegionMap->cursorPosX++;

    if (sRegionMap->cursorDeltaX < 0)
        sRegionMap->cursorPosX--;

    if (sRegionMap->cursorDeltaY > 0)
        sRegionMap->cursorPosY++;

    if (sRegionMap->cursorDeltaY < 0)
        sRegionMap->cursorPosY--;

    mapSecId = GetMapSecIdAt(sRegionMap->cursorPosX, sRegionMap->cursorPosY);
    sRegionMap->mapSecType = sub_80FB758(mapSecId);
    if (mapSecId != sRegionMap->mapSecId)
    {
        sRegionMap->mapSecId = mapSecId;
        GetMapSectionName(sRegionMap->mapSecName, sRegionMap->mapSecId, 16);
    }
    GetPositionOfCursorWithinMapSec();
    sRegionMap->inputCallback = ProcessRegionMapInput_Full;
    return MAP_INPUT_MOVE_END;
}

static u8 ProcessRegionMapInput_Zoomed(void)
{
    u8 input = MAP_INPUT_NONE;

    sRegionMap->zoomedCursorDeltaX = 0;
    sRegionMap->zoomedCursorDeltaY = 0;
    if (JOY_HELD(DPAD_UP) && sRegionMap->scrollY > -52)
    {
        sRegionMap->zoomedCursorDeltaY = -1;
        input = MAP_INPUT_MOVE_START;
    }
    if (JOY_HELD(DPAD_DOWN) && sRegionMap->scrollY < 60)
    {
        sRegionMap->zoomedCursorDeltaY = 1;
        input = MAP_INPUT_MOVE_START;
    }
    if (JOY_HELD(DPAD_LEFT) && sRegionMap->scrollX > -44)
    {
        sRegionMap->zoomedCursorDeltaX = -1;
        input = MAP_INPUT_MOVE_START;
    }
    if (JOY_HELD(DPAD_RIGHT) && sRegionMap->scrollX < 172)
    {
        sRegionMap->zoomedCursorDeltaX = 1;
        input = MAP_INPUT_MOVE_START;
    }

    if (JOY_NEW(A_BUTTON))
        input = MAP_INPUT_A_BUTTON;
    if (JOY_NEW(B_BUTTON))
        input = MAP_INPUT_B_BUTTON;

    if (input == MAP_INPUT_MOVE_START)
    {
        sRegionMap->inputCallback = MoveRegionMapCursor_Zoomed;
        sRegionMap->zoomedCursorMovementFrameCounter = 0;
    }
    return input;
}

static u8 MoveRegionMapCursor_Zoomed(void)
{
    sRegionMap->scrollY += sRegionMap->zoomedCursorDeltaY;
    sRegionMap->scrollX += sRegionMap->zoomedCursorDeltaX;
    RegionMap_SetBG2XAndBG2Y(sRegionMap->scrollX, sRegionMap->scrollY);
    sRegionMap->zoomedCursorMovementFrameCounter++;
    if (sRegionMap->zoomedCursorMovementFrameCounter == 8)
    {
        u16 x = (sRegionMap->scrollX + 44) / 8 + 1;
        u16 y = (sRegionMap->scrollY + 52) / 8 + 2;

        if (x != sRegionMap->zoomedCursorPosX || y != sRegionMap->zoomedCursorPosY)
        {
            mapsec_u16_t mapSecId;

            sRegionMap->zoomedCursorPosX = x;
            sRegionMap->zoomedCursorPosY = y;
            mapSecId = GetMapSecIdAt(x, y);
            sRegionMap->mapSecType = sub_80FB758(mapSecId);
            if (mapSecId != sRegionMap->mapSecId)
            {
                sRegionMap->mapSecId = mapSecId;
                GetMapSectionName(sRegionMap->mapSecName, sRegionMap->mapSecId, 16);
            }
            GetPositionOfCursorWithinMapSec();
        }
        sRegionMap->zoomedCursorMovementFrameCounter = 0;
        sRegionMap->inputCallback = ProcessRegionMapInput_Zoomed;
        return MAP_INPUT_MOVE_END;
    }
    return MAP_INPUT_MOVE_CONT;
}

void SetRegionMapDataForZoom(void)
{
    if (sRegionMap->zoomed == FALSE)
    {
        sRegionMap->scrollY = 0;
        sRegionMap->scrollX = 0;
        sRegionMap->unk40 = 0;
        sRegionMap->unk3C = 0;
        sRegionMap->unk60 = sRegionMap->cursorPosX * 8 - 52;
        sRegionMap->unk62 = sRegionMap->cursorPosY * 8 - 68;
        sRegionMap->unk44 = (sRegionMap->unk60 << 8) / 16;
        sRegionMap->unk48 = (sRegionMap->unk62 << 8) / 16;
        sRegionMap->zoomedCursorPosX = sRegionMap->cursorPosX;
        sRegionMap->zoomedCursorPosY = sRegionMap->cursorPosY;
        sRegionMap->unk4C = (256 << 8);
        sRegionMap->unk50 = -0x800;
    }
    else
    {
        sRegionMap->unk3C = sRegionMap->scrollX * 256;
        sRegionMap->unk40 = sRegionMap->scrollY * 256;
        sRegionMap->unk60 = 0;
        sRegionMap->unk62 = 0;
        sRegionMap->unk44 = -(sRegionMap->unk3C / 16);
        sRegionMap->unk48 = -(sRegionMap->unk40 / 16);
        sRegionMap->cursorPosX = sRegionMap->zoomedCursorPosX;
        sRegionMap->cursorPosY = sRegionMap->zoomedCursorPosY;
        sRegionMap->unk4C = (128 << 8);
        sRegionMap->unk50 = 0x800;
    }
    sRegionMap->unk6E = 0;
    FreeRegionMapCursorSprite();
    HideRegionMapPlayerIcon();
}

bool8 UpdateRegionMapZoom(void)
{
    bool8 retVal;

    if (sRegionMap->unk6E >= 16)
        return 0;
    sRegionMap->unk6E++;
    if (sRegionMap->unk6E == 16)
    {
        sRegionMap->unk44 = 0;
        sRegionMap->unk48 = 0;
        sRegionMap->scrollX = sRegionMap->unk60;
        sRegionMap->scrollY = sRegionMap->unk62;
        sRegionMap->unk4C = (sRegionMap->zoomed == FALSE) ? (128 << 8) : (256 << 8);
        sRegionMap->zoomed = !sRegionMap->zoomed;
        sRegionMap->inputCallback = (sRegionMap->zoomed == FALSE) ? ProcessRegionMapInput_Full : ProcessRegionMapInput_Zoomed;
        CreateRegionMapCursor(sRegionMap->cursorTileTag, sRegionMap->cursorPaletteTag);
        UnhideRegionMapPlayerIcon();
        retVal = 0;
    }
    else
    {
        sRegionMap->unk3C += sRegionMap->unk44;
        sRegionMap->unk40 += sRegionMap->unk48;
        sRegionMap->scrollX = sRegionMap->unk3C >> 8;
        sRegionMap->scrollY = sRegionMap->unk40 >> 8;
        sRegionMap->unk4C += sRegionMap->unk50;
        if ((sRegionMap->unk44 < 0 && sRegionMap->scrollX < sRegionMap->unk60)
         || (sRegionMap->unk44 > 0 && sRegionMap->scrollX > sRegionMap->unk60))
        {
            sRegionMap->scrollX = sRegionMap->unk60;
            sRegionMap->unk44 = 0;
        }
        if ((sRegionMap->unk48 < 0 && sRegionMap->scrollY < sRegionMap->unk62)
         || (sRegionMap->unk48 > 0 && sRegionMap->scrollY > sRegionMap->unk62))
        {
            sRegionMap->scrollY = sRegionMap->unk62;
            sRegionMap->unk48 = 0;
        }
        if (sRegionMap->zoomed == FALSE)
        {
            if (sRegionMap->unk4C < (128 << 8))
            {
                sRegionMap->unk4C = (128 << 8);
                sRegionMap->unk50 = 0;
            }
        }
        else
        {
            if (sRegionMap->unk4C > (256 << 8))
            {
                sRegionMap->unk4C = (256 << 8);
                sRegionMap->unk50 = 0;
            }
        }
        retVal = 1;
    }
    CalcZoomScrollParams(sRegionMap->scrollX, sRegionMap->scrollY, 0x38, 0x48, sRegionMap->unk4C >> 8, sRegionMap->unk4C >> 8, 0);
    return retVal;
}

static void CalcZoomScrollParams(s16 scrollX, s16 scrollY, s16 c, s16 d, u16 e, u16 f, u8 rotation)
{
    s32 var1;
    s32 var2;
    s32 var3;
    s32 var4;

    sRegionMap->bg2pa = e * gSineTable[rotation + 64] >> 8;
    sRegionMap->bg2pc = e * -gSineTable[rotation] >> 8;
    sRegionMap->bg2pb = f * gSineTable[rotation] >> 8;
    sRegionMap->bg2pd = f * gSineTable[rotation + 64] >> 8;

    var1 = (scrollX << 8) + (c << 8);
    var2 = d * sRegionMap->bg2pb + sRegionMap->bg2pa * c;
    sRegionMap->bg2x = var1 - var2;

    var3 = (scrollY << 8) + (d << 8);
    var4 = sRegionMap->bg2pd * d + sRegionMap->bg2pc * c;
    sRegionMap->bg2y = var3 - var4;

    sRegionMap->needUpdateVideoRegs = TRUE;
}

static void RegionMap_SetBG2XAndBG2Y(s16 x, s16 y)
{
    sRegionMap->bg2x = (0x1C << 8) + (x << 8);
    sRegionMap->bg2y = (0x24 << 8) + (y << 8);
    sRegionMap->needUpdateVideoRegs = TRUE;
}

void UpdateRegionMapVideoRegs(void)
{
    if (sRegionMap->needUpdateVideoRegs)
    {
        REG_BG2PA = sRegionMap->bg2pa;
        REG_BG2PB = sRegionMap->bg2pb;
        REG_BG2PC = sRegionMap->bg2pc;
        REG_BG2PD = sRegionMap->bg2pd;
        REG_BG2X = sRegionMap->bg2x;
        REG_BG2Y = sRegionMap->bg2y;
        sRegionMap->needUpdateVideoRegs = FALSE;
    }
}

void PokedexAreaScreen_UpdateRegionMapVariablesAndVideoRegs(s16 x, s16 y)
{
    CalcZoomScrollParams(x, y, 0x38, 0x48, 0x100, 0x100, 0);
    UpdateRegionMapVideoRegs();
    if (sRegionMap->playerIconSprite != NULL)
    {
        sRegionMap->playerIconSprite->x2 = -x;
        sRegionMap->playerIconSprite->y2 = -y;
    }
}

static mapsec_u16_t GetMapSecIdAt(u16 x, u16 y)
{
    if (y < MAPCURSOR_Y_MIN || y > MAPCURSOR_Y_MAX || x < MAPCURSOR_X_MIN || x > MAPCURSOR_X_MAX)
        return MAPSEC_NONE;
    y -= MAPCURSOR_Y_MIN;
    x -= MAPCURSOR_X_MIN;
    return sRegionMap_MapSectionLayout[x + y * 28];
}

static void InitializeCursorPosition(void)
{
    const struct MapHeader *mapHeader;
    u16 mapWidth;
    u16 mapHeight;
    u16 x;
    u16 y;
    u16 dimensionScale;
    u16 xOnMap;

    if (gSaveBlock1.location.mapGroup == 25
     && (gSaveBlock1.location.mapNum == MAP_NUM(MAP_SS_TIDAL_CORRIDOR)
      || gSaveBlock1.location.mapNum == MAP_NUM(MAP_SS_TIDAL_LOWER_DECK)
      || gSaveBlock1.location.mapNum == MAP_NUM(MAP_SS_TIDAL_ROOMS)))
    {
        RegionMap_InitializeStateBasedOnSSTidalLocation();
        return;
    }

    switch (GetMapTypeByGroupAndId(gSaveBlock1.location.mapGroup, gSaveBlock1.location.mapNum) - 1)
    {
    default:
    case 0:
    case 1:
    case 2:
    case 4:
    case 5:
        sRegionMap->mapSecId = gMapHeader.regionMapSectionId;
        sRegionMap->playerIsInCave = FALSE;
        mapWidth = gMapHeader.mapLayout->width;
        mapHeight = gMapHeader.mapLayout->height;
        x = gSaveBlock1.pos.x;
        y = gSaveBlock1.pos.y;
        if (sRegionMap->mapSecId == MAPSEC_UNDERWATER_SEAFLOOR_CAVERN)
            sRegionMap->playerIsInCave = TRUE;
        break;
    case 3:
    case 6:
        mapHeader = Overworld_GetMapHeaderByGroupAndId(gSaveBlock1.escapeWarp.mapGroup, gSaveBlock1.escapeWarp.mapNum);
        sRegionMap->mapSecId = mapHeader->regionMapSectionId;
        sRegionMap->playerIsInCave = TRUE;
        mapWidth = mapHeader->mapLayout->width;
        mapHeight = mapHeader->mapLayout->height;
        x = gSaveBlock1.escapeWarp.x;
        y = gSaveBlock1.escapeWarp.y;
        break;
    case 8:
        mapHeader = Overworld_GetMapHeaderByGroupAndId(gSaveBlock1.dynamicWarp.mapGroup, gSaveBlock1.dynamicWarp.mapNum);
        sRegionMap->mapSecId = mapHeader->regionMapSectionId;
        sRegionMap->playerIsInCave = TRUE;
        mapWidth = mapHeader->mapLayout->width;
        mapHeight = mapHeader->mapLayout->height;
        x = gSaveBlock1.dynamicWarp.x;
        y = gSaveBlock1.dynamicWarp.y;
        break;
    case 7:
        {
            struct WarpData *warp;

            sRegionMap->mapSecId = gMapHeader.regionMapSectionId;
            if (sRegionMap->mapSecId != MAPSEC_DYNAMIC)
            {
                warp = &gSaveBlock1.escapeWarp;
                mapHeader = Overworld_GetMapHeaderByGroupAndId(warp->mapGroup, warp->mapNum);
            }
            else
            {
                warp = &gSaveBlock1.dynamicWarp;
                mapHeader = Overworld_GetMapHeaderByGroupAndId(warp->mapGroup, warp->mapNum);
                sRegionMap->mapSecId = mapHeader->regionMapSectionId;
            }
            sRegionMap->playerIsInCave = FALSE;
            mapWidth = mapHeader->mapLayout->width;
            mapHeight = mapHeader->mapLayout->height;
            x = warp->x;
            y = warp->y;
        }
        break;
    }

    xOnMap = x;

    dimensionScale = mapWidth / gRegionMapEntries[sRegionMap->mapSecId].width;
    if (dimensionScale == 0)
        dimensionScale = 1;
    x /= dimensionScale;
    if (x >= gRegionMapEntries[sRegionMap->mapSecId].width)
        x = gRegionMapEntries[sRegionMap->mapSecId].width - 1;

    dimensionScale = mapHeight / gRegionMapEntries[sRegionMap->mapSecId].height;
    if (dimensionScale == 0)
        dimensionScale = 1;
    y /= dimensionScale;
    if (y >= gRegionMapEntries[sRegionMap->mapSecId].height)
        y = gRegionMapEntries[sRegionMap->mapSecId].height - 1;

    switch (sRegionMap->mapSecId)
    {
    case MAPSEC_ROUTE_114:
        if (y != 0)
            x = 0;
        break;
    case MAPSEC_ROUTE_126:
    case MAPSEC_UNDERWATER_126:
        x = 0;
        if (gSaveBlock1.pos.x > 32)
            x = 1;
        if (gSaveBlock1.pos.x > 0x33)
            x++;
        y = 0;
        if (gSaveBlock1.pos.y > 0x25)
            y = 1;
        if (gSaveBlock1.pos.y > 0x38)
            y++;
        break;
    case MAPSEC_ROUTE_121:
        x = 0;
        if (xOnMap > 14)
            x = 1;
        if (xOnMap > 0x1C)
            x++;
        if (xOnMap > 0x36)
            x++;
        break;
    }
    sRegionMap->cursorPosX = gRegionMapEntries[sRegionMap->mapSecId].x + x + MAPCURSOR_X_MIN;
    sRegionMap->cursorPosY = gRegionMapEntries[sRegionMap->mapSecId].y + y + MAPCURSOR_Y_MIN;
}

static void RegionMap_InitializeStateBasedOnSSTidalLocation(void)
{
    u16 y = 0;
    u16 x = 0;
    u8 mapGroup;
    u8 mapNum;
    s16 xOnMap;
    s16 yOnMap;

    switch (GetSSTidalLocation(&mapGroup, &mapNum, &xOnMap, &yOnMap))
    {
    case 1:
        sRegionMap->mapSecId = MAPSEC_SLATEPORT_CITY;
        break;
    case 2:
        sRegionMap->mapSecId = MAPSEC_LILYCOVE_CITY;
        break;
    case 3:
        sRegionMap->mapSecId = MAPSEC_ROUTE_124;
        break;
    case 4:
        sRegionMap->mapSecId = MAPSEC_ROUTE_131;
        break;
    default:
    case 0:
        {
            const struct MapHeader *mapHeader = Overworld_GetMapHeaderByGroupAndId(mapGroup, mapNum);
            u16 dimensionScale;

            sRegionMap->mapSecId = mapHeader->regionMapSectionId;
            dimensionScale = mapHeader->mapLayout->width / gRegionMapEntries[sRegionMap->mapSecId].width;
            if (dimensionScale == 0)
                dimensionScale = 1;
            x = xOnMap / dimensionScale;
            if (x >= gRegionMapEntries[sRegionMap->mapSecId].width)
                x = gRegionMapEntries[sRegionMap->mapSecId].width - 1;

            dimensionScale = mapHeader->mapLayout->height / gRegionMapEntries[sRegionMap->mapSecId].height;
            if (dimensionScale == 0)
                dimensionScale = 1;
            y = yOnMap / dimensionScale;
            if (y >= gRegionMapEntries[sRegionMap->mapSecId].height)
                y = gRegionMapEntries[sRegionMap->mapSecId].height - 1;
        }
        break;
    }
    sRegionMap->playerIsInCave = FALSE;
    sRegionMap->cursorPosX = gRegionMapEntries[sRegionMap->mapSecId].x + x + MAPCURSOR_X_MIN;
    sRegionMap->cursorPosY = gRegionMapEntries[sRegionMap->mapSecId].y + y + MAPCURSOR_Y_MIN;
}

static u16 sub_80FB758(mapsec_u16_t mapSecId)
{
    switch (mapSecId)
    {
    case MAPSEC_NONE:
        return 0;
    case MAPSEC_LITTLEROOT_TOWN:
        return FlagGet(FLAG_VISITED_LITTLEROOT_TOWN) ? 2 : 3;
    case MAPSEC_OLDALE_TOWN:
        return FlagGet(FLAG_VISITED_OLDALE_TOWN) ? 2 : 3;
    case MAPSEC_DEWFORD_TOWN:
        return FlagGet(FLAG_VISITED_DEWFORD_TOWN) ? 2 : 3;
    case MAPSEC_LAVARIDGE_TOWN:
        return FlagGet(FLAG_VISITED_LAVARIDGE_TOWN) ? 2 : 3;
    case MAPSEC_FALLARBOR_TOWN:
        return FlagGet(FLAG_VISITED_FALLARBOR_TOWN) ? 2 : 3;
    case MAPSEC_VERDANTURF_TOWN:
        return FlagGet(FLAG_VISITED_VERDANTURF_TOWN) ? 2 : 3;
    case MAPSEC_PACIFIDLOG_TOWN:
        return FlagGet(FLAG_VISITED_PACIFIDLOG_TOWN) ? 2 : 3;
    case MAPSEC_PETALBURG_CITY:
        return FlagGet(FLAG_VISITED_PETALBURG_CITY) ? 2 : 3;
    case MAPSEC_SLATEPORT_CITY:
        return FlagGet(FLAG_VISITED_SLATEPORT_CITY) ? 2 : 3;
    case MAPSEC_MAUVILLE_CITY:
        return FlagGet(FLAG_VISITED_MAUVILLE_CITY) ? 2 : 3;
    case MAPSEC_RUSTBORO_CITY:
        return FlagGet(FLAG_VISITED_RUSTBORO_CITY) ? 2 : 3;
    case MAPSEC_FORTREE_CITY:
        return FlagGet(FLAG_VISITED_FORTREE_CITY) ? 2 : 3;
    case MAPSEC_LILYCOVE_CITY:
        return FlagGet(FLAG_VISITED_LILYCOVE_CITY) ? 2 : 3;
    case MAPSEC_MOSSDEEP_CITY:
        return FlagGet(FLAG_VISITED_MOSSDEEP_CITY) ? 2 : 3;
    case MAPSEC_SOOTOPOLIS_CITY:
        return FlagGet(FLAG_VISITED_SOOTOPOLIS_CITY) ? 2 : 3;
    case MAPSEC_EVER_GRANDE_CITY:
        return FlagGet(FLAG_VISITED_EVER_GRANDE_CITY) ? 2 : 3;

    case MAPSEC_BATTLE_TOWER:
        return FlagGet(FLAG_LANDMARK_BATTLE_TOWER) ? 4 : 0;
    case MAPSEC_SOUTHERN_ISLAND:
        return FlagGet(FLAG_LANDMARK_SOUTHERN_ISLAND) ? 1 : 0;
    default:
        return 1;
    }
}

mapsec_u16_t GetRegionMapSecIdAt(u16 x, u16 y)
{
    return GetMapSecIdAt(x, y);
}

static mapsec_u16_t GetOverworldMapFromUnderwaterMap_(mapsec_u16_t mapSecId)
{
    u16 i;

    for (i = 0; sUnderwaterMaps[i][0] != MAPSEC_NONE; i++)
    {
        if (sUnderwaterMaps[i][0] == mapSecId)
            return sUnderwaterMaps[i][1];
    }
    return mapSecId;
}

mapsec_u16_t GetOverworldMapFromUnderwaterMap(mapsec_u16_t mapSecId)
{
    return GetOverworldMapFromUnderwaterMap_(mapSecId);
}

static void GetPositionOfCursorWithinMapSec(void)
{
    u16 x;
    u16 y;
    u16 posWithinMapSec;

    if (sRegionMap->mapSecId == MAPSEC_NONE)
    {
        sRegionMap->posWithinMapSec = 0;
        return;
    }

    if (sRegionMap->zoomed == FALSE)
    {
        x = sRegionMap->cursorPosX;
        y = sRegionMap->cursorPosY;
    }
    else
    {
        x = sRegionMap->zoomedCursorPosX;
        y = sRegionMap->zoomedCursorPosY;
    }

    posWithinMapSec = 0;
    while (1)
    {
        if (x <= 1)
        {
            if (RegionMap_IsMapSecIdInNextRow(y))
            {
                y--;
                x = 0x1D;
            }
            else
            {
                break;
            }
        }
        else
        {
            x--;
            if (GetMapSecIdAt(x, y) == sRegionMap->mapSecId)
                posWithinMapSec++;
        }
    }

    sRegionMap->posWithinMapSec = posWithinMapSec;
}

static bool8 RegionMap_IsMapSecIdInNextRow(u16 a)
{
    u16 x;
    u16 y;

    y = a - 1;
    if (y == 0xFFFF)
        return FALSE;

    for (x = MAPCURSOR_X_MIN; x <= MAPCURSOR_X_MAX; x++)
    {
        if (GetMapSecIdAt(x, y) == sRegionMap->mapSecId)
            return TRUE;
    }
    return FALSE;
}

static const struct OamData sRegionMapCursorOam =
{
    .y = 0,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 1,
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sRegionMapCursorAnim1[] =
{
    ANIMCMD_FRAME(0, 20),
    ANIMCMD_FRAME(4, 20),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd sRegionMapCursorAnim2[] =
{
    ANIMCMD_FRAME(0, 10),
    ANIMCMD_FRAME(16, 10),
    ANIMCMD_FRAME(32, 10),
    ANIMCMD_FRAME(16, 10),
    ANIMCMD_JUMP(0),
};

static const union AnimCmd *const sRegionMapCursorAnimTable[] =
{
    sRegionMapCursorAnim1,
    sRegionMapCursorAnim2,
};

static void SpriteCB_CursorMapFull(struct Sprite *sprite)
{
    if (sRegionMap->cursorMovementFrameCounter != 0)
    {
        sprite->x += sRegionMap->cursorDeltaX * 2;
        sprite->y += sRegionMap->cursorDeltaY * 2;
        sRegionMap->cursorMovementFrameCounter--;
    }
}

static void SpriteCB_CursorMapZoomed(struct Sprite *sprite)
{
}

void CreateRegionMapCursor(u16 tileTag, u16 paletteTag)
{
    u8 spriteId;
    struct SpriteSheet spriteSheet;
    struct SpritePalette spritePalette =
    {
        .data = sRegionMapCursorPal,
    };
    struct SpriteTemplate spriteTemplate =
    {
        .oam = &sRegionMapCursorOam,
        .anims = sRegionMapCursorAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCB_CursorMapFull,
    };

    spriteSheet.tag = tileTag;
    spriteTemplate.tileTag = tileTag;
    sRegionMap->cursorTileTag = tileTag;

    spritePalette.tag = paletteTag;
    spriteTemplate.paletteTag = paletteTag;
    sRegionMap->cursorPaletteTag = paletteTag;

    if (sRegionMap->zoomed == FALSE)
    {
        spriteSheet.data = sRegionMap->cursorSmallImage;
        spriteSheet.size = 0x100;
        spriteTemplate.callback = SpriteCB_CursorMapFull;
    }
    else
    {
        spriteSheet.data = sRegionMap->cursorLargeImage;
        spriteSheet.size = 0x600;
        spriteTemplate.callback = SpriteCB_CursorMapZoomed;
    }
    LoadSpriteSheet(&spriteSheet);
    LoadSpritePalette(&spritePalette);
    spriteId = CreateSprite(&spriteTemplate, 0x38, 0x48, 0);
    if (spriteId != 64)
    {
        sRegionMap->cursorSprite = &gSprites[spriteId];
        if (sRegionMap->zoomed == TRUE)
        {
            sRegionMap->cursorSprite->oam.size = 2;
            sRegionMap->cursorSprite->x -= 8;
            sRegionMap->cursorSprite->y -= 8;
            StartSpriteAnim(sRegionMap->cursorSprite, 1);
        }
        else
        {
            sRegionMap->cursorSprite->oam.size = 1;
            sRegionMap->cursorSprite->x = sRegionMap->cursorPosX * 8 + 4;
            sRegionMap->cursorSprite->y = sRegionMap->cursorPosY * 8 + 4;
        }
        sRegionMap->cursorSprite->data[1] = 2;
        sRegionMap->cursorSprite->data[2] = IndexOfSpritePaletteTag(paletteTag) * 16 + 0x0101;
        sRegionMap->cursorSprite->data[3] = 1;
    }
}

void FreeRegionMapCursorSprite(void)
{
    if (sRegionMap->cursorSprite != NULL)
    {
        DestroySprite(sRegionMap->cursorSprite);
        FreeSpriteTilesByTag(sRegionMap->cursorTileTag);
        FreeSpritePaletteByTag(sRegionMap->cursorPaletteTag);
    }
}

void SetUnkCursorSpriteData(void)
{
    sRegionMap->cursorSprite->data[3] = 1;
}

void ClearUnkCursorSpriteData(void)
{
    sRegionMap->cursorSprite->data[3] = 0;
}

static const struct OamData sRegionMapPlayerIconOam =
{
    .y = 0,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 1,
    .tileNum = 0,
    .priority = 2,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sRegionMapPlayerIconAnim1[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END,
};

static const union AnimCmd *const sRegionMapPlayerIconAnimTable[] =
{
    sRegionMapPlayerIconAnim1,
};

void CreateRegionMapPlayerIcon(u16 tileTag, u16 paletteTag)
{
    u8 spriteId;
    struct SpriteSheet playerIconSpriteSheet =
    {
        .data = sRegionMapPlayerIcon_BrendanGfx,
        .size = 128,
        .tag = tileTag,
    };
    struct SpritePalette playerIconSpritePalette =
    {
        .data = sRegionMapPlayerIcon_BrendanPal,
        .tag = paletteTag,
    };
    struct SpriteTemplate playerIconSpriteTemplate =
    {
        .tileTag = tileTag,
        .paletteTag = paletteTag,
        .oam = &sRegionMapPlayerIconOam,
        .anims = sRegionMapPlayerIconAnimTable,
        .images = NULL,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = SpriteCallbackDummy,
    };

    if (gSaveBlock2.playerGender == FEMALE)
    {
        playerIconSpriteSheet.data = sRegionMapPlayerIcon_MayGfx;
        playerIconSpritePalette.data = sRegionMapPlayerIcon_MayPal;
    }
    LoadSpriteSheet(&playerIconSpriteSheet);
    LoadSpritePalette(&playerIconSpritePalette);
    spriteId = CreateSprite(&playerIconSpriteTemplate, 0, 0, 1);
    sRegionMap->playerIconSprite = &gSprites[spriteId];
    if (sRegionMap->zoomed == FALSE)
    {
        sRegionMap->playerIconSprite->x = sRegionMap->playerIconSpritePosX * 8 + 4;
        sRegionMap->playerIconSprite->y = sRegionMap->playerIconSpritePosY * 8 + 4;
        sRegionMap->playerIconSprite->callback = SpriteCB_PlayerIconMapFull;
    }
    else
    {
        sRegionMap->playerIconSprite->x = sRegionMap->playerIconSpritePosX * 16 - 48;
        sRegionMap->playerIconSprite->y = sRegionMap->playerIconSpritePosY * 16 - 66;
        sRegionMap->playerIconSprite->callback = SpriteCB_PlayerIconMapZoomed;
    }
}

static void HideRegionMapPlayerIcon(void)
{
    if (sRegionMap->playerIconSprite != NULL)
    {
        sRegionMap->playerIconSprite->invisible = TRUE;
        sRegionMap->playerIconSprite->callback = SpriteCallbackDummy;
    }
}

static void UnhideRegionMapPlayerIcon(void)
{
    if (sRegionMap->playerIconSprite != NULL)
    {
        if (sRegionMap->zoomed == TRUE)
        {
            sRegionMap->playerIconSprite->x = sRegionMap->playerIconSpritePosX * 16 - 48;
            sRegionMap->playerIconSprite->y = sRegionMap->playerIconSpritePosY * 16 - 66;
            sRegionMap->playerIconSprite->callback = SpriteCB_PlayerIconMapZoomed;
            sRegionMap->playerIconSprite->invisible = FALSE;
        }
        else
        {
            sRegionMap->playerIconSprite->x = sRegionMap->playerIconSpritePosX * 8 + 4;
            sRegionMap->playerIconSprite->y = sRegionMap->playerIconSpritePosY * 8 + 4;
            sRegionMap->playerIconSprite->x2 = 0;
            sRegionMap->playerIconSprite->y2 = 0;
            sRegionMap->playerIconSprite->callback = SpriteCB_PlayerIconMapFull;
            sRegionMap->playerIconSprite->invisible = FALSE;
        }
    }
}

static void SpriteCB_PlayerIconMapZoomed(struct Sprite *sprite)
{
    sprite->x2 = -(sRegionMap->scrollX * 2);
    sprite->y2 = -(sRegionMap->scrollY * 2);
    sprite->data[0] = sprite->y + sprite->y2 + sprite->centerToCornerVecY;
    sprite->data[1] = sprite->x + sprite->x2 + sprite->centerToCornerVecX;

    // Determine if sprite is on screen
    if (sprite->data[0] < -8 || sprite->data[0] > 0xA8 || sprite->data[1] < -8 || sprite->data[1] > 0xF8)
        sprite->data[2] = FALSE;
    else
        sprite->data[2] = TRUE;

    if (sprite->data[2] == TRUE)
        SpriteCB_PlayerIcon(sprite);
    else
        sprite->invisible = TRUE;
}

static void SpriteCB_PlayerIconMapFull(struct Sprite *sprite)
{
    SpriteCB_PlayerIcon(sprite);
}

static void SpriteCB_PlayerIcon(struct Sprite *sprite)
{
    if (sRegionMap->blinkPlayerIcon)
    {
        // Toggle visibility every 16 frames
        sprite->data[7]++;
        if (sprite->data[7] > 16)
        {
            sprite->data[7] = 0;
            sprite->invisible = !sprite->invisible;
        }
    }
    else
    {
        sprite->invisible = FALSE;
    }
}

void TrySetPlayerIconBlink(void)
{
    if (sRegionMap->playerIsInCave)
        sRegionMap->blinkPlayerIcon = TRUE;
}

const u8 *GetMapSectionName(u8 *dest, mapsec_u16_t regionMapId, u16 padLength)
{
    if (regionMapId == MAPSEC_SECRET_BASE)
        return GetSecretBaseMapName(dest);
    if (regionMapId < MAPSEC_NONE)
        return StringCopy(dest, gRegionMapEntries[regionMapId].name);
    if (padLength == 0)
        padLength = 18;
    return StringFill(dest, CHAR_SPACE, padLength);
}

const u8 *GetMapNameGeneric(u8 *dest, mapsec_u16_t mapSecId)
{
    switch (mapSecId)
    {
    case MAPSEC_DYNAMIC:
        return StringCopy(dest, gText_Ferry);
    case MAPSEC_SECRET_BASE:
        return StringCopy(dest, gText_SecretBase);
    default:
        return GetMapSectionName(dest, mapSecId, 0);
    }
}

const u8 *CopyLocationName(u8 *dest, mapsec_u16_t mapSecId)
{
    if (mapSecId == MAPSEC_EVIL_TEAM_HIDEOUT)
        return StringCopy(dest, gText_Hideout);
    else
        return GetMapNameGeneric(dest, mapSecId);
}

static void GetMapSecDimensions(mapsec_u16_t mapSecId, u16 *x, u16 *y, u16 *width, u16 *height)
{
    *x = gRegionMapEntries[mapSecId].x;
    *y = gRegionMapEntries[mapSecId].y;
    *width = gRegionMapEntries[mapSecId].width;
    *height = gRegionMapEntries[mapSecId].height;
}

static const u16 sRegionMapFramePal[] = INCBIN_U16("graphics/pokenav/region_map/frame.gbapal");
static const u8 sRegionMapFrameGfxLZ[] = INCBIN_U8("graphics/pokenav/region_map/frame.4bpp.lz");
static const u8 sRegionMapFrameTilemapLZ[] = INCBIN_U8("graphics/pokenav/region_map/frame.bin.lz");
static const u16 sFlyTargetIcons_Pal[] = INCBIN_U16("graphics/pokenav/region_map/fly_target_icons.gbapal");
static const u8 sFlyTargetIcons_Gfx[] = INCBIN_U8("graphics/pokenav/region_map/fly_target_icons.4bpp.lz");

static const u8 sMapHealLocations[][3] =
{
    [MAPSEC_LITTLEROOT_TOWN] = {MAP_GROUP(MAP_LITTLEROOT_TOWN), MAP_NUM(MAP_LITTLEROOT_TOWN), HEAL_LOCATION_LITTLEROOT_TOWN_BRENDANS_HOUSE_2F},
    [MAPSEC_OLDALE_TOWN] = {MAP_GROUP(MAP_OLDALE_TOWN), MAP_NUM(MAP_OLDALE_TOWN), HEAL_LOCATION_OLDALE_TOWN},
    [MAPSEC_DEWFORD_TOWN] = {MAP_GROUP(MAP_DEWFORD_TOWN), MAP_NUM(MAP_DEWFORD_TOWN), HEAL_LOCATION_DEWFORD_TOWN},
    [MAPSEC_LAVARIDGE_TOWN] = {MAP_GROUP(MAP_LAVARIDGE_TOWN), MAP_NUM(MAP_LAVARIDGE_TOWN), HEAL_LOCATION_LAVARIDGE_TOWN},
    [MAPSEC_FALLARBOR_TOWN] = {MAP_GROUP(MAP_FALLARBOR_TOWN), MAP_NUM(MAP_FALLARBOR_TOWN), HEAL_LOCATION_FALLARBOR_TOWN},
    [MAPSEC_VERDANTURF_TOWN] = {MAP_GROUP(MAP_VERDANTURF_TOWN), MAP_NUM(MAP_VERDANTURF_TOWN), HEAL_LOCATION_VERDANTURF_TOWN},
    [MAPSEC_PACIFIDLOG_TOWN] = {MAP_GROUP(MAP_PACIFIDLOG_TOWN), MAP_NUM(MAP_PACIFIDLOG_TOWN), HEAL_LOCATION_PACIFIDLOG_TOWN},
    [MAPSEC_PETALBURG_CITY] = {MAP_GROUP(MAP_PETALBURG_CITY), MAP_NUM(MAP_PETALBURG_CITY), HEAL_LOCATION_PETALBURG_CITY},
    [MAPSEC_SLATEPORT_CITY] = {MAP_GROUP(MAP_SLATEPORT_CITY), MAP_NUM(MAP_SLATEPORT_CITY), HEAL_LOCATION_SLATEPORT_CITY},
    [MAPSEC_MAUVILLE_CITY] = {MAP_GROUP(MAP_MAUVILLE_CITY), MAP_NUM(MAP_MAUVILLE_CITY), HEAL_LOCATION_MAUVILLE_CITY},
    [MAPSEC_RUSTBORO_CITY] = {MAP_GROUP(MAP_RUSTBORO_CITY), MAP_NUM(MAP_RUSTBORO_CITY), HEAL_LOCATION_RUSTBORO_CITY},
    [MAPSEC_FORTREE_CITY] = {MAP_GROUP(MAP_FORTREE_CITY), MAP_NUM(MAP_FORTREE_CITY), HEAL_LOCATION_FORTREE_CITY},
    [MAPSEC_LILYCOVE_CITY] = {MAP_GROUP(MAP_LILYCOVE_CITY), MAP_NUM(MAP_LILYCOVE_CITY), HEAL_LOCATION_LILYCOVE_CITY},
    [MAPSEC_MOSSDEEP_CITY] = {MAP_GROUP(MAP_MOSSDEEP_CITY), MAP_NUM(MAP_MOSSDEEP_CITY), HEAL_LOCATION_MOSSDEEP_CITY},
    [MAPSEC_SOOTOPOLIS_CITY] = {MAP_GROUP(MAP_SOOTOPOLIS_CITY), MAP_NUM(MAP_SOOTOPOLIS_CITY), HEAL_LOCATION_SOOTOPOLIS_CITY},
    [MAPSEC_EVER_GRANDE_CITY] = {MAP_GROUP(MAP_EVER_GRANDE_CITY), MAP_NUM(MAP_EVER_GRANDE_CITY), HEAL_LOCATION_EVER_GRANDE_CITY},
    [MAPSEC_ROUTE_101] = {MAP_GROUP(MAP_ROUTE101), MAP_NUM(MAP_ROUTE101), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_102] = {MAP_GROUP(MAP_ROUTE102), MAP_NUM(MAP_ROUTE102), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_103] = {MAP_GROUP(MAP_ROUTE103), MAP_NUM(MAP_ROUTE103), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_104] = {MAP_GROUP(MAP_ROUTE104), MAP_NUM(MAP_ROUTE104), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_105] = {MAP_GROUP(MAP_ROUTE105), MAP_NUM(MAP_ROUTE105), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_106] = {MAP_GROUP(MAP_ROUTE106), MAP_NUM(MAP_ROUTE106), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_107] = {MAP_GROUP(MAP_ROUTE107), MAP_NUM(MAP_ROUTE107), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_108] = {MAP_GROUP(MAP_ROUTE108), MAP_NUM(MAP_ROUTE108), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_109] = {MAP_GROUP(MAP_ROUTE109), MAP_NUM(MAP_ROUTE109), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_110] = {MAP_GROUP(MAP_ROUTE110), MAP_NUM(MAP_ROUTE110), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_111] = {MAP_GROUP(MAP_ROUTE111), MAP_NUM(MAP_ROUTE111), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_112] = {MAP_GROUP(MAP_ROUTE112), MAP_NUM(MAP_ROUTE112), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_113] = {MAP_GROUP(MAP_ROUTE113), MAP_NUM(MAP_ROUTE113), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_114] = {MAP_GROUP(MAP_ROUTE114), MAP_NUM(MAP_ROUTE114), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_115] = {MAP_GROUP(MAP_ROUTE115), MAP_NUM(MAP_ROUTE115), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_116] = {MAP_GROUP(MAP_ROUTE116), MAP_NUM(MAP_ROUTE116), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_117] = {MAP_GROUP(MAP_ROUTE117), MAP_NUM(MAP_ROUTE117), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_118] = {MAP_GROUP(MAP_ROUTE118), MAP_NUM(MAP_ROUTE118), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_119] = {MAP_GROUP(MAP_ROUTE119), MAP_NUM(MAP_ROUTE119), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_120] = {MAP_GROUP(MAP_ROUTE120), MAP_NUM(MAP_ROUTE120), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_121] = {MAP_GROUP(MAP_ROUTE121), MAP_NUM(MAP_ROUTE121), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_122] = {MAP_GROUP(MAP_ROUTE122), MAP_NUM(MAP_ROUTE122), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_123] = {MAP_GROUP(MAP_ROUTE123), MAP_NUM(MAP_ROUTE123), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_124] = {MAP_GROUP(MAP_ROUTE124), MAP_NUM(MAP_ROUTE124), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_125] = {MAP_GROUP(MAP_ROUTE125), MAP_NUM(MAP_ROUTE125), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_126] = {MAP_GROUP(MAP_ROUTE126), MAP_NUM(MAP_ROUTE126), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_127] = {MAP_GROUP(MAP_ROUTE127), MAP_NUM(MAP_ROUTE127), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_128] = {MAP_GROUP(MAP_ROUTE128), MAP_NUM(MAP_ROUTE128), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_129] = {MAP_GROUP(MAP_ROUTE129), MAP_NUM(MAP_ROUTE129), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_130] = {MAP_GROUP(MAP_ROUTE130), MAP_NUM(MAP_ROUTE130), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_131] = {MAP_GROUP(MAP_ROUTE131), MAP_NUM(MAP_ROUTE131), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_132] = {MAP_GROUP(MAP_ROUTE132), MAP_NUM(MAP_ROUTE132), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_133] = {MAP_GROUP(MAP_ROUTE133), MAP_NUM(MAP_ROUTE133), HEAL_LOCATION_NONE},
    [MAPSEC_ROUTE_134] = {MAP_GROUP(MAP_ROUTE134), MAP_NUM(MAP_ROUTE134), HEAL_LOCATION_NONE},
};

static const u8 *const sEverGrandeCityNames[] =
{
    gText_PokemonLeague,
    gText_PokemonCenter,
};

struct MultiNameFlyDest
{
    const u8 *const *name;
    mapsec_u16_t mapSecId;
    u16 flag;
};

// Map sections that are divided into multiple parts. Ever Grande City is the only one.
static const struct MultiNameFlyDest sMultiNameFlyDestinations[1] =
{
    {sEverGrandeCityNames, MAPSEC_EVER_GRANDE_CITY, FLAG_LANDMARK_POKEMON_LEAGUE},
};

struct RegionMapState *const gRegionMapState = &eRegionMapState;

static const struct SpritePalette sFlyTargetIconsSpritePalette = {sFlyTargetIcons_Pal, 2};

// Fly targets that are not cities or towns
static const u16 sRedOutlineFlyDestinations[][2] =
{
    // flag, mapSectionId
    {FLAG_LANDMARK_BATTLE_TOWER, MAPSEC_BATTLE_TOWER},
    {0xFFFF, MAPSEC_NONE},
};

static const struct OamData sFlyDestIcon_OamData =
{
    .y = 0,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = 0,
    .tileNum = 0,
    .priority = 2,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sFlyDestIcon_Anim_8x8CanFly[] =
{
    ANIMCMD_FRAME(0, 5),
    ANIMCMD_END,
};

static const union AnimCmd sFlyDestIcon_Anim_16x8CanFly[] =
{
    ANIMCMD_FRAME(1, 5),
    ANIMCMD_END,
};

static const union AnimCmd sFlyDestIcon_Anim_8x16CanFly[] =
{
    ANIMCMD_FRAME(3, 5),
    ANIMCMD_END,
};

static const union AnimCmd sFlyDestIcon_Anim_8x8CantFly[] =
{
    ANIMCMD_FRAME(5, 5),
    ANIMCMD_END,
};

static const union AnimCmd sFlyDestIcon_Anim_16x8CantFly[] =
{
    ANIMCMD_FRAME(6, 5),
    ANIMCMD_END,
};

static const union AnimCmd sFlyDestIcon_Anim_8x16CantFly[] =
{
    ANIMCMD_FRAME(8, 5),
    ANIMCMD_END,
};

static const union AnimCmd sFlyDestIcon_Anim_RedOutline[] =
{
    ANIMCMD_FRAME(10, 5),
    ANIMCMD_END,
};

static const union AnimCmd *const sFlyDestIcon_Anims[] =
{
    sFlyDestIcon_Anim_8x8CanFly,
    sFlyDestIcon_Anim_16x8CanFly,
    sFlyDestIcon_Anim_8x16CanFly,
    sFlyDestIcon_Anim_8x8CantFly,
    sFlyDestIcon_Anim_16x8CantFly,
    sFlyDestIcon_Anim_8x16CantFly,
    sFlyDestIcon_Anim_RedOutline,
};

static const struct SpriteTemplate sFlyDestIconSpriteTemplate =
{
    .tileTag = 2,
    .paletteTag = 2,
    .oam = &sFlyDestIcon_OamData,
    .anims = sFlyDestIcon_Anims,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

void CB2_OpenFlyMap(void)
{
    switch (gMain.state)
    {
    case 0:
        SetVBlankCallback(NULL);
        REG_DISPCNT = 0;
        REG_BG0HOFS = 0;
        REG_BG0VOFS = 0;
        REG_BG1HOFS = 0;
        REG_BG1VOFS = 0;
        REG_BG2VOFS = 0;
        REG_BG2HOFS = 0;
        REG_BG3HOFS = 0;
        REG_BG3VOFS = 0;
        ResetPaletteFade();
        ResetSpriteData();
        FreeSpriteTileRanges();
        FreeAllSpritePalettes();
        break;
    case 1:
        Text_LoadWindowTemplate(&gWindowTemplate_81E7224);
        break;
    case 2:
        InitMenuWindow(&gWindowTemplate_81E7224);
        Menu_EraseScreen();
        break;
    case 3:
        InitRegionMap(&gRegionMapState->regionMap, 0);
        CreateRegionMapCursor(0, 0);
        CreateRegionMapPlayerIcon(1, 1);
        gRegionMapState->mapSecId = gRegionMapState->regionMap.mapSecId;
        StringFill(gRegionMapState->blankMapName, CHAR_SPACE, 12);
        PrintFlyTargetName();
        break;
    case 4:
        LZ77UnCompVram(sRegionMapFrameGfxLZ, (void *)(VRAM + 0xC000));
        break;
    case 5:
        LZ77UnCompVram(sRegionMapFrameTilemapLZ, (void *)(VRAM + 0xF000));
        break;
    case 6:
        LoadPalette(sRegionMapFramePal, 16, 32);
        Menu_PrintTextPixelCoords(gText_FlyToWhere, 1, 0x90, 1);
        break;
    case 7:
        LoadFlyDestIcons();
        break;
    case 8:
        BlendPalettes(0xFFFFFFFF, 16, RGB(0, 0, 0));
        SetVBlankCallback(VBlankCB_FlyMap);
        break;
    case 9:
        REG_BLDCNT = 0;
        REG_BG1CNT = 0x1E0D;
        REG_DISPCNT = 0x1741;
        SetFlyMapCallback(CB_FadeInFlyMap);
        SetMainCallback2(CB2_FlyRegionMap);
        break;
    default:
        return;
    }
    gMain.state++;
}

static void VBlankCB_FlyMap(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

void CB2_FlyRegionMap(void)
{
    gRegionMapState->callback();
    AnimateSprites();
    BuildOamBuffer();
}

void SetFlyMapCallback(void (*callback)(void))
{
    gRegionMapState->callback = callback;
    gRegionMapState->state = 0;
}

static void PrintFlyTargetName(void)
{
    if (gRegionMapState->regionMap.mapSecType == 2 || gRegionMapState->regionMap.mapSecType == 4)
    {
        bool8 namePrinted = FALSE;
        u16 i;

        for (i = 0; i < ARRAY_COUNT(sMultiNameFlyDestinations); i++)
        {
            if (gRegionMapState->regionMap.mapSecId == sMultiNameFlyDestinations[i].mapSecId)
            {
                if (FlagGet(sMultiNameFlyDestinations[i].flag))
                {
                    Menu_DrawStdWindowFrame(16, 14, 29, 19);
                    Menu_PrintText(gRegionMapState->regionMap.mapSecName, 17, 15);
                    MenuPrint_RightAligned(sMultiNameFlyDestinations[i].name[gRegionMapState->regionMap.posWithinMapSec], 29, 17);
                    namePrinted = TRUE;
                }
                break;
            }
        }

        if (!namePrinted)
        {
            Menu_DrawStdWindowFrame(16, 16, 29, 19);
            Menu_PrintText(gRegionMapState->regionMap.mapSecName, 17, 17);
            Menu_EraseWindowRect(16, 14, 29, 15);
        }
    }
    else
    {
        Menu_DrawStdWindowFrame(16, 16, 29, 19);
        Menu_PrintText(gRegionMapState->blankMapName, 17, 17);
        Menu_EraseWindowRect(16, 14, 29, 15);
    }
}

static void LoadFlyDestIcons(void)
{
    struct SpriteSheet spriteSheet;

    LZ77UnCompWram(sFlyTargetIcons_Gfx, gRegionMapState->unk_888);
    spriteSheet.data = (u8 *)gRegionMapState->unk_888;
    spriteSheet.size = 0x1C0;
    spriteSheet.tag = 2;
    LoadSpriteSheet(&spriteSheet);
    LoadSpritePalette(&sFlyTargetIconsSpritePalette);
    CreateFlyDestIcons();
    TryCreateRedOutlineFlyDestIcons();
}

// Draws a light overlay on cities and towns that the player can fly to
static void CreateFlyDestIcons(void)
{
    u16 canFlyFlag = FLAG_VISITED_LITTLEROOT_TOWN;
    mapsec_u16_t mapSecId;

    for (mapSecId = 0; mapSecId < 16; mapSecId++)
    {
        u16 x;
        u16 y;
        u16 width;
        u16 height;
        u16 shape;
        u8 spriteId;

        GetMapSecDimensions(mapSecId, &x, &y, &width, &height);
        x = (x + 1) * 8 + 4;
        y = (y + 2) * 8 + 4;
        if (width == 2)
            shape = 1;
        else if (height == 2)
            shape = 2;
        else
            shape = 0;
        spriteId = CreateSprite(&sFlyDestIconSpriteTemplate, x, y, 10);
        if (spriteId != 64)
        {
            gSprites[spriteId].oam.shape = shape;
            if (FlagGet(canFlyFlag))
                gSprites[spriteId].callback = SpriteCB_FlyDestIcon;
            else
                shape += 3;
            StartSpriteAnim(&gSprites[spriteId], shape);
            gSprites[spriteId].data[0] = mapSecId;
        }
        canFlyFlag++;
    }
}

// Draws a red box on other fly targets
// The Battle Tower is the only one of these
static void TryCreateRedOutlineFlyDestIcons(void)
{
    u16 i;

    for (i = 0; sRedOutlineFlyDestinations[i][1] != MAPSEC_NONE; i++)
    {
        u16 x;
        u16 y;
        u16 width;
        u16 height;

        if (FlagGet(sRedOutlineFlyDestinations[i][0]))
        {
            mapsec_u16_t mapSecId = sRedOutlineFlyDestinations[i][1];
            u8 spriteId;

            GetMapSecDimensions(mapSecId, &x, &y, &width, &height);
            x = (x + 1) * 8;
            y = (y + 2) * 8;
            spriteId = CreateSprite(&sFlyDestIconSpriteTemplate, x, y, 10);
            if (spriteId != 64)
            {
                gSprites[spriteId].oam.size = 1;
                gSprites[spriteId].callback = SpriteCB_FlyDestIcon;
                StartSpriteAnim(&gSprites[spriteId], 6);
                gSprites[spriteId].data[0] = mapSecId;
            }
        }
    }
}

static void SpriteCB_FlyDestIcon(struct Sprite *sprite)
{
    // Blink if our mapSectionId is the one selected on the map
    if (gRegionMapState->regionMap.mapSecId == sprite->data[0])
    {
        // Toggle visibility every 16 frames
        sprite->data[1]++;
        if (sprite->data[1] > 16)
        {
            sprite->data[1] = 0;
            sprite->invisible = !sprite->invisible;
        }
    }
    else
    {
        sprite->data[1] = 16;
        sprite->invisible = FALSE;
    }
}

static void CB_FadeInFlyMap(void)
{
    switch (gRegionMapState->state)
    {
    case 0:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        gRegionMapState->state++;
        break;
    case 1:
        if (UpdatePaletteFade() != 0)
            break;
        SetFlyMapCallback(CB_HandleFlyMapInput);
        break;
    }
}

static void CB_HandleFlyMapInput(void)
{
    if (gRegionMapState->state == 0)
    {
        switch (DoRegionMapInputCallback())
        {
        case MAP_INPUT_NONE:
        case MAP_INPUT_MOVE_START:
        case MAP_INPUT_MOVE_CONT:
            break;
        case MAP_INPUT_MOVE_END:
            PrintFlyTargetName();
            break;
        case MAP_INPUT_A_BUTTON:
            if (gRegionMapState->regionMap.mapSecType == 2 || gRegionMapState->regionMap.mapSecType == 4)
            {
                m4aSongNumStart(SE_SELECT);
                gRegionMapState->choseFlyLocation = 1;
                SetFlyMapCallback(sub_80FC69C);
            }
            break;
        case MAP_INPUT_B_BUTTON:
            m4aSongNumStart(SE_SELECT);
            gRegionMapState->choseFlyLocation = 0;
            SetFlyMapCallback(sub_80FC69C);
            break;
        }
    }
}

void sub_80FC69C(void)
{
    switch (gRegionMapState->state)
    {
    case 0:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        gRegionMapState->state++;
        break;
    case 1:
        if (UpdatePaletteFade() != 0)
            break;
        FreeRegionMapIconResources();
        if (gRegionMapState->choseFlyLocation != 0)
        {
            switch (gRegionMapState->regionMap.mapSecId)
            {
            case MAPSEC_SOUTHERN_ISLAND:
                SetWarpDestinationToHealLocation(HEAL_LOCATION_SOUTHERN_ISLAND_EXTERIOR);
                break;
            case MAPSEC_BATTLE_TOWER:
                SetWarpDestinationToHealLocation(HEAL_LOCATION_BATTLE_TOWER_OUTSIDE);
                break;
            case MAPSEC_LITTLEROOT_TOWN:
                SetWarpDestinationToHealLocation((gSaveBlock2.playerGender == MALE) ? HEAL_LOCATION_LITTLEROOT_TOWN_BRENDANS_HOUSE : HEAL_LOCATION_LITTLEROOT_TOWN_MAYS_HOUSE);
                break;
            case MAPSEC_EVER_GRANDE_CITY:
                SetWarpDestinationToHealLocation((FlagGet(FLAG_LANDMARK_POKEMON_LEAGUE) && gRegionMapState->regionMap.posWithinMapSec == 0) ? HEAL_LOCATION_EVER_GRANDE_CITY_POKEMON_LEAGUE : HEAL_LOCATION_EVER_GRANDE_CITY);
                break;
            default:
                if (sMapHealLocations[gRegionMapState->regionMap.mapSecId][2] != HEAL_LOCATION_NONE)
                    SetWarpDestinationToHealLocation(sMapHealLocations[gRegionMapState->regionMap.mapSecId][2]);
                else
                    SetWarpDestinationToMapWarp(sMapHealLocations[gRegionMapState->regionMap.mapSecId][0], sMapHealLocations[gRegionMapState->regionMap.mapSecId][1], -1);
                break;
            }
            ReturnToFieldFromFlyMapSelect();
        }
        else
        {
            SetMainCallback2(CB2_ReturnToPartyMenuFromFlyMap);
        }
        break;
    }
}

#if DEBUG

void debug_sub_8110CCC(void)
{
    bool8 r7 = FALSE;
    u16 i;
    s16 indent;

    for (i = 0; i < ARRAY_COUNT(sMultiNameFlyDestinations); i++)
    {
        if (gRegionMapState->regionMap.mapSecId == sMultiNameFlyDestinations[i].mapSecId)
        {
            if (FlagGet(sMultiNameFlyDestinations[i].flag))
            {
                indent = 12 - StringLength(sMultiNameFlyDestinations[i].name[gRegionMapState->regionMap.posWithinMapSec]);
                if (indent < 0)
                    indent = 0;
                r7 = TRUE;
                Menu_DrawStdWindowFrame(16, 14, 29, 19);
                Menu_PrintText(gRegionMapState->regionMap.mapSecName, 17, 15);
                Menu_PrintText(sMultiNameFlyDestinations[i].name[gRegionMapState->regionMap.posWithinMapSec], 17 + indent, 17);
            }
            break;
        }
    }
    if (!r7)
    {
        Menu_DrawStdWindowFrame(16, 16, 29, 19);
        Menu_PrintText(gRegionMapState->regionMap.mapSecName, 17, 17);
        Menu_EraseWindowRect(16, 14, 29, 15);
    }
}

void debug_sub_8110D84(void)
{
    switch (gRegionMapState->state)
    {
    case 0:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        gRegionMapState->state++;
        break;
    case 1:
        if (!UpdatePaletteFade())
            gRegionMapState->state++;
        break;
    case 2:
        switch (DoRegionMapInputCallback())
        {
        case 0:
            break;
        case 3:
            debug_sub_8110CCC();
            break;
        case 4:
            if (gRegionMapState->regionMap.mapSecType != 0)
            {
                m4aSongNumStart(SE_SELECT);
                gSharedMem[0xA6E] = 1;  // TODO: what is this?
                SetFlyMapCallback(sub_80FC69C);
            }
            break;
        case 5:
            m4aSongNumStart(SE_SELECT);
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
            gRegionMapState->state++;
            break;
        }
        break;
    case 3:
        if (!UpdatePaletteFade())
            SetMainCallback2(CB2_ReturnToFieldWithOpenMenu);
        break;
    case 4:
        if (DoRegionMapInputCallback() != 0)
        {
            debug_sub_8110CCC();
        }
        else if (JOY_NEW(A_BUTTON))
        {
            FreeRegionMapCursorSprite();
            SetRegionMapDataForZoom();
            gRegionMapState->state++;
        }
        break;
    case 5:
        if (UpdateRegionMapZoom() == 0)
        {
            CreateRegionMapCursor(0, 0);
            gRegionMapState->state++;
        }
        break;
    case 6:
        if (DoRegionMapInputCallback() != 0)
        {
            debug_sub_8110CCC();
        }
        if (JOY_NEW(A_BUTTON))  // no "else if" like above?
        {
            FreeRegionMapCursorSprite();
            SetRegionMapDataForZoom();
            gRegionMapState->state++;
        }
        break;
    case 7:
        if (UpdateRegionMapZoom() == 0)
        {
            CreateRegionMapCursor(0, 0);
            gRegionMapState->state = 3;
        }
        break;
    }
}

void debug_sub_8110F28(void)
{
    CB2_OpenFlyMap();
    
    if (gMain.callback2 == CB2_FlyRegionMap)
    {
        TrySetPlayerIconBlink();
        SetFlyMapCallback(debug_sub_8110D84);
        debug_sub_8110CCC();
    }
}

#endif

