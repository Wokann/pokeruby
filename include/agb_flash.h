#ifndef GUARD_AGB_FLASH_H
#define GUARD_AGB_FLASH_H

u16 SetFlashTimerIntr(u8 timerNum, void (**intrFunc)(void));
u32 ProgramFlashSectorAndVerify(u16 sectorNum, u8 *src);

#endif // GUARD_AGB_FLASH_H
