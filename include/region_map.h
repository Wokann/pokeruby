#ifndef GUARD_REGION_MAP_H
#define GUARD_REGION_MAP_H

#define MAP_NAME_LENGTH 16

enum
{
    MAP_INPUT_NONE,
    MAP_INPUT_MOVE_START,
    MAP_INPUT_MOVE_CONT,
    MAP_INPUT_MOVE_END,
    MAP_INPUT_A_BUTTON,
    MAP_INPUT_B_BUTTON,
};

enum
{
    MAPSECTYPE_NONE,
    MAPSECTYPE_ROUTE,
    MAPSECTYPE_CITY_CANFLY,
    MAPSECTYPE_CITY_CANTFLY,
    MAPSECTYPE_BATTLE_TOWER,
    NUM_MAPSEC_TYPES,
};

struct RegionMapLocation
{
    u8 x, y;
    u8 width, height;
    const u8 *name;
};

struct RegionMap
{
    u8 mapSecName[20];
    /*0x14*/ u16 mapSecId;
    /*0x16*/ u8 mapSecType;
    /*0x17*/ u8 posWithinMapSec;
    /*0x18*/ u8 (*inputCallback)(void);
    /*0x1C*/ struct Sprite *cursorSprite;
    /*0x20*/ struct Sprite *playerIconSprite;
    /*0x24*/ s32 bg2x;
    /*0x28*/ s32 bg2y;
    /*0x2C*/ u32 bg2pa;
    /*0x30*/ u32 bg2pc;
    /*0x34*/ u32 bg2pb;
    /*0x38*/ u32 bg2pd;
    /*0x3C*/ s32 zoomScrollXFixed;
    /*0x40*/ s32 zoomScrollYFixed;
    /*0x44*/ s32 zoomScrollXDelta;
    /*0x48*/ s32 zoomScrollYDelta;
    /*0x4C*/ s32 zoomScale;
    /*0x50*/ s32 zoomScaleDelta;
    /*0x54*/ u16 cursorPosX;
    /*0x56*/ u16 cursorPosY;
    /*0x58*/ u16 cursorTileTag;
    /*0x5A*/ u16 cursorPaletteTag;
    /*0x5C*/ s16 scrollX;
    /*0x5E*/ s16 scrollY;
    /*0x60*/ s16 zoomTargetScrollX;
    /*0x62*/ s16 zoomTargetScrollY;
    /*0x64*/ u16 zoomedCursorPosX;
    /*0x66*/ u16 zoomedCursorPosY;
    /*0x68*/ u16 zoomedCursorDeltaY;
    /*0x6A*/ u16 zoomedCursorDeltaX;
    /*0x6C*/ u16 zoomedCursorMovementFrameCounter;
    /*0x6E*/ u16 zoomTransitionFrameCounter;
    /*0x70*/ u16 playerIconTileTag;
    /*0x72*/ u16 playerIconPaletteTag;
    /*0x74*/ u16 playerIconSpritePosX;
    /*0x76*/ u16 playerIconSpritePosY;
    /*0x78*/ bool8 zoomed;
    /*0x79*/ u8 initStep;
    /*0x7A*/ s8 cursorMovementFrameCounter;
    /*0x7B*/ s8 cursorDeltaX;
    /*0x7C*/ s8 cursorDeltaY;
    /*0x7D*/ bool8 needUpdateVideoRegs;
    /*0x7E*/ bool8 blinkPlayerIcon;
    /*0x7F*/ bool8 playerIsInCave;
    /*0x080*/ u8 filler_080[0x100];
    /*0x180*/ u8 cursorSmallImage[0x100];
    /*0x280*/ u8 cursorLargeImage[0x600];
}; // size: 0x880

union RegionMapStateBuffer
{
    u16 state;
    u8 tileBuffer[0x1C0];
};

struct RegionMapState
{
    void (*callback)(void);
    u16 state;
    u16 mapSecId;
    struct RegionMap regionMap;
    // Reused as a field-map state value or Fly Map icon decompression buffer.
    union RegionMapStateBuffer buffer;
    u8 nameBuffer[0x26];
    bool8 choseFlyLocation;
};

extern struct RegionMapState *const gRegionMapState;

void InitRegionMap(struct RegionMap *regionMap, bool8 zoomed);
void InitRegionMapData(struct RegionMap *regionMap, bool8 zoomed);
bool8 LoadRegionMapGfx(void);
void FreeRegionMapIconResources(void);
u8 DoRegionMapInputCallback(void);
void SetRegionMapDataForZoom(void);
bool8 UpdateRegionMapZoom(void);
void UpdateRegionMapVideoRegs(void);
void PokedexAreaScreen_UpdateRegionMapVariablesAndVideoRegs(s16, s16);
u16 CorrectSpecialMapSecId(u16 mapSecId);
void CreateRegionMapCursor(u16, u16);
void CreateRegionMapPlayerIcon(u16, u16);
const u8 *GetMapName(u8 *dest, u16 mapSecId, u16 padLength);
const u8 *GetMapNameGeneric(u8 *dest, u16 mapSecId);
const u8 *GetMapNameHandleEvilTeamHideout(u8 *dest, u16 mapSecId);
void CB2_OpenFlyMap(void);
void CB2_OpenDebugRegionMap(void);
u16 GetRegionMapSecIdAt(u16 x, u16 y);
void TrySetPlayerIconBlink(void);

extern const struct RegionMapLocation gRegionMapEntries[];

#endif // GUARD_REGION_MAP_H
