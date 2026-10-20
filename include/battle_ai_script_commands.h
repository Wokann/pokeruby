#ifndef GUARD_BATTLE_AI_SCRIPT_COMMANDS_H
#define GUARD_BATTLE_AI_SCRIPT_COMMANDS_H

// Return values for BattleAI_GetAIActionToUse; 0 - 3 are move indices.
#define AI_CHOICE_FLEE 4
#define AI_CHOICE_WATCH 5

// battle_ai
void BattleAI_SetupAIData(void);
u8 BattleAI_GetAIActionToUse(void);
void sub_810745C(void);
void BattleAI_HandleItemUseBeforeAISetup(void);
void RecordAbilityBattle(u8 battler, u8 abilityId);
void RecordItemEffectBattle(u8 battler, u8 itemEffect);

#endif // GUARD_BATTLE_AI_SCRIPT_COMMANDS_H
