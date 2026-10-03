#include "global.h"
#include "field_region_map.h"
#include "main.h"
#include "menu.h"
#include "palette.h"
#include "region_map.h"
#include "sprite.h"
#include "strings2.h"
#include "text.h"
#include "ewram.h"
#include "constants/rgb.h"

enum
{
    TAG_PLAYER_ICON,
    TAG_CURSOR,
};

enum
{
    FIELD_REGION_MAP_STATE_INIT,
    FIELD_REGION_MAP_STATE_WAIT_FADE_IN,
    FIELD_REGION_MAP_STATE_HANDLE_INPUT,
    FIELD_REGION_MAP_STATE_START_FADE_OUT,
    FIELD_REGION_MAP_STATE_WAIT_FADE_OUT,
};

static void MCB2_InitRegionMapRegisters(void);
static void VBCB_FieldUpdateRegionMap(void);
static void MCB2_FieldUpdateRegionMap(void);
static void FieldUpdateRegionMap(void);
static void PrintRegionMapSecName(void);

void FieldInitRegionMap(MainCallback callback)
{
    SetVBlankCallback(NULL);
    sRegionMapState.buffer.state = FIELD_REGION_MAP_STATE_INIT;
    sRegionMapState.callback = callback;
    SetMainCallback2(MCB2_InitRegionMapRegisters);
}

static void MCB2_InitRegionMapRegisters(void)
{
    REG_DISPCNT = 0;
    REG_BG0HOFS = 0;
    REG_BG0VOFS = 0;
    REG_BG1HOFS = 0;
    REG_BG1VOFS = 0;
    REG_BG2HOFS = 0;
    REG_BG2VOFS = 0;
    REG_BG3HOFS = 0;
    REG_BG3VOFS = 0;
    ResetSpriteData();
    FreeAllSpritePalettes();
    InitRegionMap(&sRegionMapState.regionMap, FALSE);
    CreateRegionMapPlayerIcon(TAG_PLAYER_ICON, TAG_PLAYER_ICON);
    CreateRegionMapCursor(TAG_CURSOR, TAG_CURSOR);
    Text_LoadWindowTemplate(&gMenuWindowTemplateBg0);
    InitMenuWindow(&gMenuWindowTemplateBg0);
    Menu_EraseScreen();
    REG_BG0CNT = BGCNT_PRIORITY(0) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(31) | BGCNT_16COLOR | BGCNT_TXT256x256;
    Menu_DrawStdWindowFrame(21, 0, 29, 3);
    MenuPrint_Centered(gOtherText_Hoenn, 0x16, 1, 0x38);
    Menu_DrawStdWindowFrame(16, 16, 29, 19);
    PrintRegionMapSecName();
    SetMainCallback2(MCB2_FieldUpdateRegionMap);
    SetVBlankCallback(VBCB_FieldUpdateRegionMap);
    BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
}

static void VBCB_FieldUpdateRegionMap(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void MCB2_FieldUpdateRegionMap(void)
{
    FieldUpdateRegionMap();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void FieldUpdateRegionMap(void)
{
    switch (sRegionMapState.buffer.state)
    {
    case FIELD_REGION_MAP_STATE_INIT:
        REG_DISPCNT = DISPCNT_MODE_1 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
        sRegionMapState.buffer.state++;
        break;
    case FIELD_REGION_MAP_STATE_WAIT_FADE_IN:
        if (!gPaletteFade.active)
            sRegionMapState.buffer.state++;
        break;
    case FIELD_REGION_MAP_STATE_HANDLE_INPUT:
        switch (DoRegionMapInputCallback())
        {
        case MAP_INPUT_MOVE_END:
            PrintRegionMapSecName();
            break;
        case MAP_INPUT_A_BUTTON:
        case MAP_INPUT_B_BUTTON:
            sRegionMapState.buffer.state++;
        }
        break;
    case FIELD_REGION_MAP_STATE_START_FADE_OUT:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        sRegionMapState.buffer.state++;
        break;
    case FIELD_REGION_MAP_STATE_WAIT_FADE_OUT:
        if (!gPaletteFade.active)
        {
            FreeRegionMapIconResources();
            SetMainCallback2(sRegionMapState.callback);
        }
        break;
    }
}

static void PrintRegionMapSecName(void)
{
    Menu_BlankWindowRect(17, 17, 28, 18);
    if (sRegionMapState.regionMap.mapSecType != MAPSECTYPE_NONE)
        Menu_PrintText(sRegionMapState.regionMap.mapSecName, 17, 17);
}
