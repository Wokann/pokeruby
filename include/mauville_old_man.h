#ifndef GUARD_MAUVILLE_OLD_MAN_H
#define GUARD_MAUVILLE_OLD_MAN_H

#include "constants/mauville_old_man.h"

extern struct BardSong gBardSong;

void SetMauvilleOldMan(void);
void ResetMauvilleOldManFlag(void);
void SetMauvilleOldManObjEventGfx(void);
void debug_sub_810B32C(u8);
u8 GetCurrentMauvilleOldMan(void);

#endif // GUARD_MAUVILLE_OLD_MAN_H
