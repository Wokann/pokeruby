#ifndef GUARD_FIELD_SPECIALS_H
#define GUARD_FIELD_SPECIALS_H

extern bool8 gBikeCyclingChallenge;
extern u8 gBikeCollisions;

struct ElevatorMenu {
    u8 var0;
    s8 var1;
    s8 var2;
    u8 var3;
};

extern u8 gUnknown_0203925A;
extern u8 gUnknown_0203925B;
extern u8 gUnknown_0203925C;

extern const u8 gUnknown_083F8408[];
extern const u8 gUnknown_083F8410[];

void ResetCyclingRoadChallengeData(void);
bool32 CountSSTidalStep(u16 delta);
u8 GetSSTidalLocation(s8 *mapGroup, s8 *mapNum, s16 *x, s16 *y);
void ScriptAddElevatorMenuItem(u8, u8, u8, u8);
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
