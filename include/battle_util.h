#ifndef GUARD_BATTLE_UTIL_H
#define GUARD_BATTLE_UTIL_H

#include "constants/battle_script_commands.h"

#define MOVE_LIMITATION_ZEROMOVE                (1 << 0)
#define MOVE_LIMITATION_PP                      (1 << 1)
#define MOVE_LIMITATION_DISABLED                (1 << 2)
#define MOVE_LIMITATION_TORMENTED               (1 << 3)
#define MOVE_LIMITATION_TAUNT                   (1 << 4)
#define MOVE_LIMITATION_IMPRISON                (1 << 5)
#define MOVE_LIMITATIONS_ALL                    0xFF

#define ABILITYEFFECT_ON_SWITCHIN                0x0
#define ABILITYEFFECT_ENDTURN                    0x1
#define ABILITYEFFECT_MOVES_BLOCK                0x2
#define ABILITYEFFECT_ABSORBING                  0x3
#define ABILITYEFFECT_ON_DAMAGE                  0x4
#define ABILITYEFFECT_IMMUNITY                   0x5
#define ABILITYEFFECT_FORECAST                   0x6
#define ABILITYEFFECT_SYNCHRONIZE                0x7
#define ABILITYEFFECT_ATK_SYNCHRONIZE            0x8
#define ABILITYEFFECT_INTIMIDATE1                0x9
#define ABILITYEFFECT_INTIMIDATE2                0xA
#define ABILITYEFFECT_TRACE                      0xB
#define ABILITYEFFECT_CHECK_OTHER_SIDE           0xC
#define ABILITYEFFECT_CHECK_BATTLER_SIDE         0xD
#define ABILITYEFFECT_FIELD_SPORT                0xE
#define ABILITYEFFECT_CHECK_FIELD_EXCEPT_BATTLER 0xF
#define ABILITYEFFECT_COUNT_OTHER_SIDE           0x10
#define ABILITYEFFECT_COUNT_BATTLER_SIDE         0x11
#define ABILITYEFFECT_COUNT_ON_FIELD             0x12
#define ABILITYEFFECT_CHECK_ON_FIELD             0x13
#define ABILITYEFFECT_MUD_SPORT                  253
#define ABILITYEFFECT_WATER_SPORT                254
#define ABILITYEFFECT_SWITCH_IN_WEATHER          0xFF

#define ABILITY_ON_OPPOSING_FIELD(battlerId, abilityId)(AbilityBattleEffects(ABILITYEFFECT_CHECK_OTHER_SIDE, battlerId, abilityId, 0, 0))
#define ABILITY_ON_FIELD(abilityId)(AbilityBattleEffects(ABILITYEFFECT_CHECK_ON_FIELD, 0, abilityId, 0, 0))
#define ABILITY_ON_FIELD2(abilityId)(AbilityBattleEffects(ABILITYEFFECT_FIELD_SPORT, 0, abilityId, 0, 0))

#define ITEMEFFECT_ON_SWITCH_IN                 0x0
#define ITEMEFFECT_NORMAL                       1
#define ITEMEFFECT_DUMMY                        2 // Unused, empty
#define ITEMEFFECT_MOVE_END                     0x3
#define ITEMEFFECT_KINGSROCK_SHELLBELL          0x4

#define WEATHER_HAS_EFFECT ((!AbilityBattleEffects(ABILITYEFFECT_CHECK_ON_FIELD, 0, ABILITY_CLOUD_NINE, 0, 0) && !AbilityBattleEffects(ABILITYEFFECT_CHECK_ON_FIELD, 0, ABILITY_AIR_LOCK, 0, 0)))
#define WEATHER_HAS_EFFECT2 ((!ABILITY_ON_FIELD2(ABILITY_CLOUD_NINE) && !ABILITY_ON_FIELD2(ABILITY_AIR_LOCK)))

#define DISOBEDIENCE_OBEDIENT 0
#define DISOBEDIENCE_IGNORED  1 // command is ignored
#define DISOBEDIENCE_OTHER    2 // random move or self-hit

void HandleAction_UseMove(void);
void HandleAction_Switch(void);
void HandleAction_UseItem(void);
void HandleAction_Run(void);
void HandleAction_WatchesCarefully(void);
void HandleAction_SafariZoneBallThrow(void);
void HandleAction_ThrowPokeblock(void);
void HandleAction_GoNear(void);
void HandleAction_SafariZoneRun(void);
void HandleAction_WallyBallThrow(void);
void HandleAction_TryFinish(void);
void HandleAction_NothingIsFainted(void);
void HandleAction_ActionFinished(void);
u8 GetBattlerForBattleScript(u8 caseId);
void PressurePPLose(u8 target, u8 attacker, u16 move);
void PressurePPLoseOnUsingPerishSong(u8 attacker);
void PressurePPLoseOnUsingImprison(u8 attacker);
void MarkBattlerForControllerExec(u8 battler);
void MarkBattlerReceivedLinkData(u8 battler);
void CancelMultiTurnMoves(u8 battler);
bool8 WasUnableToUseMove(u8 battler);
void PrepareStringBattle(u16 stringId, u8 battler);
void ResetSentPokesToOpponentValue(void);
void OpponentSwitchInResetSentPokesToOpponentValue(u8 battler);
void UpdateSentPokesToOpponentValue(u8 battler);
void BattleScriptPush(const u8 *bsPtr);
void BattleScriptPushCursor(void);
void BattleScriptPop(void);
u8 TrySetCantSelectMoveBattleScript(void);
u8 CheckMoveLimitations(u8 battlerId, u8 unusableMoves, u8 check);
bool8 AreAllMovesUnusable(void);
u8 GetImprisonedMovesCount(u8 battler, u16 move);
u8 DoFieldEndTurnEffects(void);
u8 DoBattlerEndTurnEffects(void);
bool8 HandleWishPerishSongOnTurnEnd(void);
bool8 HandleFaintedMonActions(void);
void TryClearRageStatuses(void);
u8 AtkCanceler_UnableToUseMove(void);
bool8 HasNoMonsToSwitch(u8 battler, u8 partyIdBattlerOn1, u8 partyIdBattlerOn2);
u8 CastformDataTypeChange(u8 battler);
u8 AbilityBattleEffects(u8 caseID, u8 battler, u8 ability, u8 special, u16 moveArg);
void BattleScriptExecute(const u8 *BS_ptr);
void BattleScriptPushCursorAndCallback(const u8 *BS_ptr);
u8 ItemBattleEffects(u8 caseID, u8 battler, bool8 moveTurn);
void HandleAction_RunBattleScript(void);
u8 GetMoveTarget(u16 move, u8 setTarget);
u8 IsMonDisobedient(void);

#endif // GUARD_BATTLE_UTIL_H
