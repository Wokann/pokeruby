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
#define MISS_TYPE               6
#define MSG_DISPLAY             7
#define BATTLE_COMMUNICATION_ENTRIES_COUNT  8

// Battle Script defines for getting the wanted battler.
#define BS_TARGET                   0
#define BS_ATTACKER                 1
#define BS_EFFECT_BATTLER           2
#define BS_FAINTED                  3
#define BS_ATTACKER_WITH_PARTNER    4 // for Cmd_updatestatusicon
#define BS_FAINTED_LINK_MULTIPLE_1  5 // for Cmd_openpartyscreen
#define BS_FAINTED_LINK_MULTIPLE_2  6 // for Cmd_openpartyscreen
#define BS_BATTLER_0                7
#define BS_ATTACKER_SIDE            8 // for Cmd_jumpifability
#define BS_NOT_ATTACKER_SIDE        9 // for Cmd_jumpifability
#define BS_SCRIPTING                10
#define BS_PLAYER1                  11
#define BS_OPPONENT1                12
#define BS_PLAYER2                  13 // for Cmd_updatestatusicon
#define BS_OPPONENT2                14

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
#define VARIOUS_GET_BATTLER_FAINTED             4
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

// Stat flags for Cmd_playstatchangeanimation.
#define BIT_HP                      (1 << 0)
#define BIT_ATK                     (1 << 1)
#define BIT_DEF                     (1 << 2)
#define BIT_SPEED                   (1 << 3)
#define BIT_SPATK                   (1 << 4)
#define BIT_SPDEF                   (1 << 5)
#define BIT_ACC                     (1 << 6)
#define BIT_EVASION                 (1 << 7)

#define PARTY_SCREEN_OPTIONAL (1 << 7) // Flag for first argument to Cmd_openpartyscreen

// Cases for Cmd_moveend.
#define MOVEEND_RAGE                              0
#define MOVEEND_DEFROST                           1
#define MOVEEND_SYNCHRONIZE_TARGET                2
#define MOVEEND_ON_DAMAGE_ABILITIES               3
#define MOVEEND_IMMUNITY_ABILITIES                4
#define MOVEEND_SYNCHRONIZE_ATTACKER              5
#define MOVEEND_CHOICE_MOVE                       6
#define MOVEEND_CHANGED_ITEMS                     7
#define MOVEEND_ATTACKER_INVISIBLE                8
#define MOVEEND_ATTACKER_VISIBLE                  9
#define MOVEEND_TARGET_VISIBLE                    10
#define MOVEEND_ITEM_EFFECTS_ALL                  11
#define MOVEEND_KINGSROCK_SHELLBELL               12
#define MOVEEND_SUBSTITUTE                        13
#define MOVEEND_UPDATE_LAST_MOVES                 14
#define MOVEEND_MIRROR_MOVE                       15
#define MOVEEND_NEXT_TARGET                       16
#define MOVEEND_COUNT                             17

#endif // GUARD_CONSTANTS_BATTLE_SCRIPT_COMMANDS_H
