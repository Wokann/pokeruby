#ifndef GUARD_HALL_OF_FAME_H
#define GUARD_HALL_OF_FAME_H

void CB2_DoHallOfFameScreen(void);
void PrepareHallOfFameMonPicSpriteTemplate(u16 paletteTag, u8 spriteSlot);
void CB2_DoHallOfFamePC(void);
void PrepareHallOfFameTrainerPicSpriteTemplate(u16 paletteTag, u8 spriteSlot);
extern void *const gHallOfFameGfxBufferPtrs[];

#endif // GUARD_HALL_OF_FAME_H
