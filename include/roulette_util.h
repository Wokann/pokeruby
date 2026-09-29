#ifndef GUARD_ROULETTE_UTIL_H
#define GUARD_ROULETTE_UTIL_H

#include "roulette.h"

void RouletteFlash_Reset(struct RouletteFlashUtil *flash);
u8 RouletteFlash_Add(struct RouletteFlashUtil *flash, u8 id, const struct RouletteFlashSettings *settings);
void RouletteFlash_Run(struct RouletteFlashUtil *flash);
void RouletteFlash_Enable(struct RouletteFlashUtil *flash, u16 flags);
void RouletteFlash_Stop(struct RouletteFlashUtil *flash, u16 flags);
void FillTilemapRect(u16 *, u16, u8, u8, u8, u8);
void SetTilemapRect(u16 *, u16 *, u8, u8, u8, u8);

#endif
