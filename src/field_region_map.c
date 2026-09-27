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

void FieldInitRegionMap(MainCallback callback)
{
    SetVBlankCallback(NULL);
    eRegionMapState.unk_888[0] = 0;
    eRegionMapState.callback = callback;
    SetMainCallback2(MCB2_InitRegionMapRegisters);
}

void MCB2_InitRegionMapRegisters(void)
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
    InitRegionMap(&eRegionMapState.regionMap, 0);
    CreateRegionMapPlayerIcon(0, 0);
    CreateRegionMapCursor(1, 1);
    Text_LoadWindowTemplate(&gWindowTemplate_81E709C);
    InitMenuWindow(&gWindowTemplate_81E709C);
    Menu_EraseScreen();
    REG_BG0CNT = BGCNT_PRIORITY(0) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(31) | BGCNT_16COLOR | BGCNT_TXT256x256;
    Menu_DrawStdWindowFrame(21, 0, 29, 3);
    MenuPrint_Centered(gOtherText_Hoenn, 0x16, 1, 0x38);
    Menu_DrawStdWindowFrame(16, 16, 29, 19);
    PrintRegionMapSecName();
    SetMainCallback2(MCB2_FieldUpdateRegionMap);
    SetVBlankCallback(VBCB_FieldUpdateRegionMap);
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
}

void VBCB_FieldUpdateRegionMap(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

void MCB2_FieldUpdateRegionMap(void)
{
    FieldUpdateRegionMap();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

void FieldUpdateRegionMap(void)
{
    switch (eRegionMapState.unk_888[0])
    {
    case 0:
        REG_DISPCNT = DISPCNT_MODE_1 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG2_ON | DISPCNT_OBJ_ON;
        eRegionMapState.unk_888[0]++;
        break;
    case 1:
        if (!gPaletteFade.active)
            eRegionMapState.unk_888[0]++;
        break;
    case 2:
        switch (DoRegionMapInputCallback())
        {
        case MAP_INPUT_MOVE_END:
            PrintRegionMapSecName();
            break;
        case MAP_INPUT_A_BUTTON:
        case MAP_INPUT_B_BUTTON:
            eRegionMapState.unk_888[0]++;
        }
        break;
    case 3:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
        eRegionMapState.unk_888[0]++;
        break;
    case 4:
        if (!gPaletteFade.active)
        {
            FreeRegionMapIconResources();
            SetMainCallback2(eRegionMapState.callback);
        }
        break;
    }
}

void PrintRegionMapSecName(void)
{
    Menu_BlankWindowRect(17, 17, 28, 18);
    if (eRegionMapState.regionMap.unk16)
        Menu_PrintText(eRegionMapState.regionMap.mapSectionName, 17, 17);
}
