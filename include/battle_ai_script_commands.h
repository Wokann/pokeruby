#ifndef GUARD_BATTLE_AI_SCRIPT_COMMANDS_H
#define GUARD_BATTLE_AI_SCRIPT_COMMANDS_H

// battle_ai
void BattleAI_SetupAIData(void);
u8 BattleAI_GetAIActionToUse(void);
void sub_810745C(void);
void BattleAI_HandleItemUseBeforeAISetup(void);
void RecordAbilityBattle(u8 battler, u8 abilityId);
void RecordItemEffectBattle(u8 battler, u8 itemEffect);

#endif // GUARD_BATTLE_AI_SCRIPT_COMMANDS_H
