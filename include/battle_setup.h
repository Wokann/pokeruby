#ifndef GUARD_BATTLE_SETUP_H
#define GUARD_BATTLE_SETUP_H

#define REMATCH_TABLE_ENTRIES 56

struct RematchTrainer
{
    u16 trainerIds[5];  // Each of these trainers has 5 increasingly stronger teams.
    u16 mapGroup;
    u16 mapNum;
};

extern const struct RematchTrainer gRematchTable[56];
extern u16 gTrainerBattleOpponent_A;

void BattleSetup_StartWildBattle(void);
void BattleSetup_StartRoamerBattle(void);
void StartWallyTutorialBattle(void);
void BattleSetup_StartScriptedWildBattle(void);
void BattleSetup_StartLatiBattle(void);
void ScrSpecial_StartRayquazaBattle(void);
void StartGroudonKyogreBattle(void);
void StartRegiBattle(void);
s8 BattleSetup_GetEnvironmentId(void);
u8 BattleSetup_GetBattleTowerBattleTransition(void);
void ChooseStarter(void);
u8 *BattleSetup_ConfigureTrainerBattle(const u8 *data);
void ConfigureAndSetUpOneTrainerBattle(u8 trainerObjEventId, const u8 *trainerScript);
bool32 GetTrainerFlagFromScriptPointer(const u8 *data);
void SetTrainerFacingDirection(void);
u8 GetTrainerBattleMode(void);
bool8 GetTrainerFlag(void);
bool8 HasTrainerBeenFought(u16 trainerId);
void SetTrainerFlag(u16 trainerId);
void ClearTrainerFlag(u16 trainerId);
void BattleSetup_StartTrainerBattle(void);
void BattleSetup_StartRematchBattle(void);
void ShowTrainerIntroSpeech(void);
u8 *BattleSetup_GetScriptAddrAfterBattle(void);
u8 *BattleSetup_GetTrainerPostBattleScript(void);
void ShowTrainerCantBattleSpeech(void);
void PlayTrainerEncounterMusic(void);
u8 *GetTrainerALoseText(void);
const u8 *GetTrainerWonSpeech(void);
void IncrementRematchStepCounter(void);
void TryUpdateRandomTrainerRematches(u16 mapGroup, u16 mapNum);
s32 DoesSomeoneWantRematchIn(u16 mapGroup, u16 mapNum);
s32 IsRematchTrainerIn(u16 mapGroup, u16 mapNum);
bool8 ShouldTryRematchBattle(void);
bool8 IsTrainerReadyForRematch(void);

void do_choose_name_or_words_screen(void);

#endif // GUARD_BATTLE_SETUP_H
