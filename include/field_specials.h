#ifndef GUARD_FIELD_SPECIALS_H
#define GUARD_FIELD_SPECIALS_H

extern bool8 gBikeCyclingChallenge;
extern u8 gBikeCollisions;

struct ElevatorMenu {
    u8 floorTextId;
    s8 mapGroup;
    s8 mapNum;
    u8 unused;
};

extern u8 gScrollableMenuItemCount;
extern u8 gScrollableMenuSelection;
extern u8 gScrollableMenuArrowFlags;

extern const u8 gFanClubMemberIdsForGainingFans[];
extern const u8 gFanClubMemberIdsForLosingFans[];

void ResetCyclingRoadChallengeData(void);
bool32 CountSSTidalStep(u16);
u8 GetSSTidalLocation(s8 *, s8 *, s16 *, s16 *);
void ScriptAddElevatorMenuItem(u8 floorTextId, u8 mapGroup, u8 mapNum, u8 unused);
void ScriptShowElevatorMenu(void);
void SetShoalItemFlag(u16 unused);
u8 GetLeadMonIndex(void);
void ResetFanClub(void);
u8 TryGainNewFanFromCounter(u8 incrementId);
void UpdateTrainerFansAfterLinkBattle(void);
u16 GetSlotMachineId(void);
void UpdateTrainerFanClubGameClear(void);
int PlayerGainRandomTrainerFan(void);
int PlayerLoseRandomTrainerFan(void);

#endif // GUARD_FIELD_SPECIALS_H
