#ifndef GUARD_CABLE_CAR_UTIL_H
#define GUARD_CABLE_CAR_UTIL_H

// Exported type declarations

// Exported RAM declarations

// Exported ROM declarations

void CableCarUtil_FillWrapped(void *tilemap, u16 value, u8 left, u8 top, u8 width, u8 height);
void CableCarUtil_CopyWrapped(void *tilemap, const u16 *source, u8 left, u8 top, u8 width, u8 height);

#endif //GUARD_CABLE_CAR_UTIL_H
