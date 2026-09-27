#ifndef GUARD_FIELD_SPECIALS_H
#define GUARD_FIELD_SPECIALS_H

struct ElevatorMenu {
    u8 var0;
    s8 var1;
    s8 var2;
    u8 var3;
};

extern u8 gUnknown_02039250;
extern u8 gUnknown_02039251;
extern u32 gUnknown_02039254;
extern u8 gScrollableMenuItemCount;
extern u8 gScrollableMenuSelection;
extern u8 gScrollableMenuArrowFlags;

extern const u8 gFanClubMemberIdsForGainingFans[];
extern const u8 gFanClubMemberIdsForLosingFans[];

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
