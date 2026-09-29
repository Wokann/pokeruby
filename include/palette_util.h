#ifndef GUARD_PALETTE_UTIL_H
#define GUARD_PALETTE_UTIL_H

#include "global.h"

#define FLASHUTIL_USE_EXISTING_COLOR (1 << 15)

struct RouletteFlashSettings
{
    u16 color;
    u16 paletteOffset;
    u8 numColors;
    u8 delay;
    s8 unk6;
    s8 numFadeCycles:5;
    s8 unk7_5:2;
    s8 colorDeltaDir:1;
};

struct RouletteFlashPalette
{
    u8 state:7;
    u8 available:1;
    u8 delayCounter;
    s8 fadeCycleCounter;
    s8 colorDelta;
    struct RouletteFlashSettings settings;
};

struct RouletteFlashUtil
{
    u8 enabled;
    u8 unused;
    u16 flags;
    struct RouletteFlashPalette palettes[16];
};

void RouletteFlash_Reset(struct RouletteFlashUtil *flash);
u8 RouletteFlash_Add(struct RouletteFlashUtil *flash, u8 id, const struct RouletteFlashSettings *settings);
void RouletteFlash_Run(struct RouletteFlashUtil *flash);
void RouletteFlash_Enable(struct RouletteFlashUtil *flash, u16 flags);
void RouletteFlash_Stop(struct RouletteFlashUtil *flash, u16 flags);
void FillTilemapRect(u16 *, u16, u8, u8, u8, u8);
void SetTilemapRect(u16 *, u16 *, u8, u8, u8, u8);

#endif // GUARD_PALETTE_UTIL_H
