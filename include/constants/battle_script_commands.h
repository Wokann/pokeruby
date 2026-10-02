#ifndef GUARD_CONSTANTS_BATTLE_SCRIPT_COMMANDS_H
#define GUARD_CONSTANTS_BATTLE_SCRIPT_COMMANDS_H

// Array entries for battle communication.
#define MULTIUSE_STATE          0
#define CURSOR_POSITION         1
#define TASK_ID                 1 // task Id and cursor position share the same field
#define SPRITES_INIT_STATE1     1 // shares the Id as well
#define SPRITES_INIT_STATE2     2
#define MOVE_EFFECT_BYTE        3
#define ACTIONS_CONFIRMED_COUNT 4
#define MULTISTRING_CHOOSER     5
#define MSG_DISPLAY             7
#define BATTLE_COMMUNICATION_ENTRIES_COUNT  8

// Battle Script defines for getting the wanted battler.
#define BS_ATTACKER_WITH_PARTNER    4 // for Cmd_updatestatusicon
#define BS_ATTACKER_SIDE            8 // for Cmd_jumpifability
#define BS_NOT_ATTACKER_SIDE        9 // for Cmd_jumpifability

// Cmd_accuracycheck
#define NO_ACC_CALC                 0xFFFE
#define NO_ACC_CALC_CHECK_LOCK_ON   0xFFFF
#define ACC_CURR_MOVE               0

// Compare operands.
#define CMP_EQUAL               0
#define CMP_NOT_EQUAL           1
#define CMP_GREATER_THAN        2
#define CMP_LESS_THAN           3
#define CMP_COMMON_BITS         4
#define CMP_NO_COMMON_BITS      5

// Cmd_various
#define VARIOUS_CANCEL_MULTI_TURN_MOVES         0
#define VARIOUS_SET_MAGIC_COAT_TARGET           1
#define VARIOUS_IS_RUNNING_IMPOSSIBLE           2
#define VARIOUS_GET_MOVE_TARGET                 3
#define VARIOUS_RESET_INTIMIDATE_TRACE_BITS     5
#define VARIOUS_UPDATE_CHOICE_MOVE_ON_LVL_UP    6
#define VARIOUS_EMIT_YESNOBOX                   13
#define VARIOUS_WAIT_CRY                        18
#define VARIOUS_RETURN_OPPONENT_MON1            19
#define VARIOUS_RETURN_OPPONENT_MON2            20
#define VARIOUS_SET_TELEPORT_OUTCOME            25
#define VARIOUS_PLAY_TRAINER_DEFEATED_MUSIC     26

// Cmd_manipulatedmg
#define DMG_CHANGE_SIGN            0
#define DMG_RECOIL_FROM_MISS       1
#define DMG_DOUBLED                2

// Cmd_jumpifcantswitch
#define SWITCH_IGNORE_ESCAPE_PREVENTION   (1 << 7)

// Cmd_statbuffchange
#define STAT_CHANGE_ALLOW_PTR               (1 << 0)
#define STAT_CHANGE_NOT_PROTECT_AFFECTED    (1 << 5)

// Stat change flags for Cmd_playstatchangeanimation.
#define STAT_CHANGE_NEGATIVE             (1 << 0)
#define STAT_CHANGE_BY_TWO               (1 << 1)
#define STAT_CHANGE_MULTIPLE_STATS       (1 << 2)
#define STAT_CHANGE_CANT_PREVENT         (1 << 3)

#endif // GUARD_CONSTANTS_BATTLE_SCRIPT_COMMANDS_H
