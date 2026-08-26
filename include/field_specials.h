#ifndef GUARD_FIELD_SPECIALS_H
#define GUARD_FIELD_SPECIALS_H

struct ElevatorMenuItem {
    u8 floorTextId;
    s8 mapGroup;
    s8 mapNum;
    u8 unused;
};

extern u8 gUnknown_02039250;
extern u8 gUnknown_02039251;
extern u32 gUnknown_02039254;
extern u8 gUnknown_02039258;
extern u8 gScrollableMenuItemCount;
extern u8 gScrollableMenuSelectedItem;
extern u8 gScrollableMenuScrollIndicatorFlags;

extern const u8 gFanClubGainMemberIds[];
extern const u8 gFanClubLossMemberIds[];

void ResetCyclingRoadChallengeData(void);
bool32 CountSSTidalStep(u16);
u8 GetSSTidalLocation(s8 *, s8 *, s16 *, s16 *);
void ScriptAddElevatorMenuItem(u8, u8, u8, u8);
void ScriptShowElevatorMenu(void);
void SetShoalItemFlag(u16);
u8 GetLeadMonIndex(void);
void ResetFanClub(void);
u8 TryGainNewFanFromCounter(u8 incrementId);
void UpdateTrainerFansAfterLinkBattle(void);
u16 GetSlotMachineId(void);
void UpdateTrainerFanClubGameClear(void);
int PlayerGainRandomTrainerFan(void);
int PlayerLoseRandomTrainerFan(void);

#endif // GUARD_FIELD_SPECIALS_H
