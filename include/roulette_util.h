#ifndef GUARD_ROULETTE_UTIL_H
#define GUARD_ROULETTE_UTIL_H

#include "roulette.h"

void RouletteFlash_Reset(struct UnkStruct0 *);
u8 RouletteFlash_Add(struct UnkStruct0 *, u8, const struct UnkStruct1 *);
void RouletteFlash_Run(struct UnkStruct0 *);
void RouletteFlash_Enable(struct UnkStruct0 *, u16);
void RouletteFlash_Stop(struct UnkStruct0 *, u16);
void FillTilemapRect(u16 *, u16, u8, u8, u8, u8);
void SetTilemapRect(u16 *, u16 *, u8, u8, u8, u8);

#endif
