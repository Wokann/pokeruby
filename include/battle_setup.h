#ifndef GUARD_BATTLE_SETUP_H
#define GUARD_BATTLE_SETUP_H

#define REMATCHES_COUNT 5
#define REMATCH_TABLE_ENTRIES 56
#define TRAINER_REMATCH_STEPS 255

// IV + LEVEL + SPECIES
struct TrainerMonNoItemDefaultMoves
{
    u16 iv;
    u8 level;
    u16 species;
};

// IV + LEVEL + SPECIES + MOVES
struct TrainerMonNoItemCustomMoves
{
    u16 iv;
    u8 level;
    u16 species;
    u16 moves[4];
};

// IV + LEVEL + SPECIES + ITEMS
struct TrainerMonItemDefaultMoves
{
    u16 iv;
    u8 level;
    u16 species;
    u16 heldItem;
};

// IV + LEVEL + SPECIES + ITEMS + MOVES
struct TrainerMonItemCustomMoves
{
    u16 iv;
    u8 level;
    u16 species;
    u16 heldItem;
    u16 moves[4];
};

struct RematchTrainer
{
    u16 trainerIds[REMATCHES_COUNT];
    u16 mapGroup;
    u16 mapNum;
};

extern const struct RematchTrainer gRematchTable[REMATCH_TABLE_ENTRIES];

void BattleSetup_StartWildBattle(void);
void BattleSetup_StartRoamerBattle(void);
void BattleSetup_StartScriptedWildBattle(void);
void BattleSetup_StartLatiBattle(void);
s8 BattleSetup_GetEnvironmentId(void);
u8 BattleSetup_GetBattleTowerBattleTransition(void);
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
void CB2_EndTrainerBattle(void);
void do_choose_name_or_words_screen(void);
u8 *BattleSetup_GetScriptAddrAfterBattle(void);
u8 *BattleSetup_GetTrainerPostBattleScript(void);
void ShowTrainerIntroSpeech(void);
void ShowTrainerCantBattleSpeech(void);
void PlayTrainerEncounterMusic(void);
u8 *GetTrainerLoseText(void);
void IncrementRematchStepCounter(void);
void TryUpdateRandomTrainerRematches(u16, u16);
bool32 DoesSomeoneWantRematchIn(u16 mapGroup, u16 mapNum);
bool32 IsRematchTrainerIn(u16 mapGroup, u16 mapNum);
bool8 ShouldTryRematchBattle(void);
bool8 IsTrainerReadyForRematch(void);

#endif // GUARD_BATTLE_SETUP_H
