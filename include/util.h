#ifndef GUARD_UTIL_H
#define GUARD_UTIL_H

#include "sprite.h"

extern const u8 gMiscBlank_Gfx[];
extern const u32 gBitTable[];

u8 CreateInvisibleSpriteWithCallback(void (*callback)(struct Sprite *));
void StoreWordInTwoHalfwords(u16 *h, u32 w);
void LoadWordFromTwoHalfwords(u16 *h, u32 *w);
int CountTrailingZeroBits(u32 value);
u16 CalcCRC16(const u8 *data, int length);
u16 CalcCRC16WithTable(u8 *data, int length);
void DoBgAffineSet(struct BgAffineDstData *dest, u32 texX, u32 texY, s16 scrX, s16 scrY, s16 sx, s16 sy, u16 alpha);
void CopySpriteTiles(u8 shape, u8 size, u8 *tiles, u16 *tilemap, u8 *output);

#endif // GUARD_UTIL_H
