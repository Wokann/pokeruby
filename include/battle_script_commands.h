#ifndef GUARD_BATTLE_SCRIPT_COMMANDS_H
#define GUARD_BATTLE_SCRIPT_COMMANDS_H

#include "constants/battle_script_commands.h"

#define WINDOW_CLEAR            0x1
#define WINDOW_x80              0x80

void AI_CalcDmg(u8 attacker, u8 defender);
u8 TypeCalc(u16 move, u8 attacker, u8 defender);
u8 AI_TypeCalc(u16 move, u16 targetSpecies, u8 targetAbility);
u8 GetBattlerTurnOrderNum(u8 battler);
void SetMoveEffect(bool8 primary, u8 certain);
void sub_802BC6C(void);
void nullsub_6(void);
void BufferMoveToLearnIntoBattleTextBuff2(void);
void sub_802BBD4(u8 xStart, u8 yStart, u8 xEnd, u8 yEnd, u8 sp0);
bool8 UproarWakeUpCheck(u8 battler);

extern void (*const gBattleScriptingCommandsTable[])(void);

#endif // GUARD_BATTLE_SCRIPT_COMMANDS_H
