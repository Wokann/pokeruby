#ifndef GUARD_PALETTE_UTIL_H
#define GUARD_PALETTE_UTIL_H

struct RouletteFlashSettings
{
    u16 color;
    u16 paletteOffset;
    u8 numColors;
    u8 delay;
    u8 unk6;
    s8 numFadeCycles:5;
    s8 unk7_5:2;
    s8 colorDeltaDir:1;
}; // size: 8

struct RouletteFlashPalette
{
    u8 state:7;
    bool8 available:1;
    u8 delayCounter;
    s8 fadeCycleCounter;
    s8 colorDelta;
    struct RouletteFlashSettings settings;
}; // size: 12

struct RouletteFlashUtil
{
    u8 enabled;
    u8 unused;
    u16 flags;
    struct RouletteFlashPalette palettes[16];
}; // size: 196

void FillTilemapRect(u16 *dest, u16 value, u8 left, u8 top, u8 width, u8 height);
void SetTilemapRect(u16 *dest, u16 *src, u8 left, u8 top, u8 width, u8 height);
void RouletteFlash_Run(struct RouletteFlashUtil *flash);
void RouletteFlash_Reset(struct RouletteFlashUtil *flash);
u8 RouletteFlash_Add(struct RouletteFlashUtil *flash, u8 id, const struct RouletteFlashSettings *settings);
void RouletteFlash_Stop(struct RouletteFlashUtil *flash, u16 flags);
void RouletteFlash_Enable(struct RouletteFlashUtil *flash, u16 flags);

#endif // GUARD_PALETTE_UTIL_H
