#ifndef GUARD_BATTLE_AI_SCRIPT_COMMANDS_H
#define GUARD_BATTLE_AI_SCRIPT_COMMANDS_H

// Return values for BattleAI_ChooseMoveOrAction.
// 0 - 3 are move indices.
#define AI_CHOICE_FLEE  4
#define AI_CHOICE_WATCH 5

// AI states
enum
{
    AIState_SettingUp,
    AIState_Processing,
    AIState_FinishedProcessing,
    AIState_DoNotProcess
};

// battle_ai
void BattleAI_SetupAIData(void);
u8 BattleAI_ChooseMoveOrAction(void);
void BattleAI_DoAIProcessing(void);
void RecordLastUsedMoveByTarget(void);
void ClearBattlerMoveHistory(u8 battler);
void AIStackPushVar(const u8 *var);
u8 AIStackPop(void);
void BattleAI_HandleItemUseBeforeAISetup(void);
void RecordAbilityBattle(u8 battler, u8 abilityId);
void RecordItemEffectBattle(u8 battler, u8 itemEffect);

#endif // GUARD_BATTLE_AI_SCRIPT_COMMANDS_H
