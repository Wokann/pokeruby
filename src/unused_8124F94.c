#include "global.h"
#include "decompress.h"
#include "palette.h"

struct UnusedTileLoadRequest
{
    void *tileData;
    u8 tileWidth;
    u8 tileHeight;
    u8 unused;
    u16 isCompressed;
};

struct UnusedLoadedTile
{
    void *tileData;
    u8 tileWidth;
    u8 tileHeight;
    u8 unused;
    u16 tileOffset;
};

struct UnusedPaletteLoadRequest
{
    u16 *paletteData;
    u8 paletteId;
    u8 isCompressed;
};

struct UnusedMultiPaletteLoadRequest
{
    u16 *paletteData;
    u8 firstPaletteId;
    u8 numPalettes;
};

struct UnusedLoadedPalette
{
    u16 *paletteData;
    u8 paletteId;
    u8 paletteSlot;
};

struct UnusedTilePaletteLoader
{
    u8 paletteCount;
    u8 tileCount;
    u16 nextTileOffset;
    u8 *tileVram;
    struct UnusedLoadedPalette palettes[16];
    struct UnusedLoadedTile tiles[32];
};

void UnusedTilePaletteLoader_Reset(struct UnusedTilePaletteLoader *loader)
{
    loader->tileCount = 0;
    loader->paletteCount = 0;
    loader->nextTileOffset = 0;
    loader->tileVram = (void *)VRAM;
    DmaFill16(3, 0, loader->palettes, sizeof(loader->palettes));
    DmaFill16(3, 0, loader->tiles, sizeof(loader->tiles));
}

u8 UnusedTilePaletteLoader_LoadTiles(struct UnusedTilePaletteLoader *loader, const struct UnusedTileLoadRequest *requests)
{
    while (1)
    {
        s32 tileCount;
        s32 nextTileOffset;

        // Couldn't get it to match any other way
        if (loader->tileCount < 32 && requests->tileData == NULL)
            return 0;
        if (loader->tileCount >= 32)
            break;

        loader->tiles[loader->tileCount].tileData = requests->tileData;
        loader->tiles[loader->tileCount].unused = requests->unused;
        loader->tiles[loader->tileCount].tileWidth = requests->tileWidth;
        loader->tiles[loader->tileCount].tileHeight = requests->tileHeight;
        tileCount = requests->tileWidth * requests->tileHeight;
        if (loader->nextTileOffset + tileCount > 0x400)
            return 2;
        if (requests->isCompressed == 0)
        {
            DmaCopy16(3, requests->tileData, loader->tileVram + loader->nextTileOffset * 64, tileCount * 32);
        }
        else
        {
            LZDecompressWram(requests->tileData, loader->tileVram + loader->nextTileOffset * 64);
        }
        loader->tiles[loader->tileCount].tileOffset = loader->nextTileOffset;
        nextTileOffset = tileCount + loader->nextTileOffset;
        loader->nextTileOffset = nextTileOffset;
        loader->tileCount++;
        requests++;
    }
    return 1;
}

u8 UnusedTilePaletteLoader_LoadPalettes(struct UnusedTilePaletteLoader *loader, const struct UnusedPaletteLoadRequest *requests)
{
    while (1)
    {
        // Couldn't get it to match any other way
        if (loader->paletteCount < 16 && requests->paletteData == NULL)
            return 0;
        if (loader->paletteCount >= 16)
            break;

        loader->palettes[loader->paletteCount].paletteData = requests->paletteData;
        loader->palettes[loader->paletteCount].paletteId = requests->paletteId;
        if (requests->isCompressed == 0)
        {
            LoadPalette(requests->paletteData, loader->paletteCount * 16, 32);
        }
        else
        {
            u16 palette[16];

            LZDecompressWram(requests->paletteData, palette);
            LoadPalette(palette, loader->paletteCount * 16, 32);
        }
        loader->palettes[loader->paletteCount].paletteSlot = loader->paletteCount;
        loader->paletteCount = loader->paletteCount + 1;
        requests++;
    }
    return 1;
}

u8 UnusedTilePaletteLoader_LoadMultiplePalettes(struct UnusedTilePaletteLoader *loader, const struct UnusedMultiPaletteLoadRequest *request)
{
    u16 palettes[16][16] = {0};
    u8 numPalettes = request->numPalettes;
    u8 paletteIndex;

    LZDecompressWram(request->paletteData, palettes);
    for (paletteIndex = loader->paletteCount; paletteIndex < numPalettes; paletteIndex++)
    {
        if (loader->paletteCount + paletteIndex >= 16)
            return 1;
        loader->palettes[paletteIndex].paletteData = request->paletteData;
        loader->palettes[paletteIndex].paletteId = request->firstPaletteId + paletteIndex;
        loader->palettes[paletteIndex].paletteSlot = loader->paletteCount;
        LoadPalette(palettes[paletteIndex], loader->paletteCount * 16, sizeof(palettes[paletteIndex]));
        loader->paletteCount++;
    }
    return 0;
}
