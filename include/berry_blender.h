#ifndef GUARD_BERRY_BLENDER_H
#define GUARD_BERRY_BLENDER_H

void debug_sub_80524BC(void);
void sub_804E990(void);

extern u8 gInGameOpponentsNo;
void DoBerryBlending(void);
void ShowBerryBlenderRecordWindow(void);
void SetPlayerBerryData(u8 playerId, u16 itemId);

#endif //GUARD_BERRY_BLENDER_H
