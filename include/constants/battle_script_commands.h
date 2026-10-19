#ifndef GUARD_CONSTANTS_BATTLE_SCRIPT_COMMANDS_H
#define GUARD_CONSTANTS_BATTLE_SCRIPT_COMMANDS_H

// Ruby battle scripting state in gSharedMem
#define sB_ANIM_TURN                (gSharedMem + 0x16002)
#define sBATTLER                    (gSharedMem + 0x16003)
#define sMOVEEND_STATE              (gSharedMem + 0x1600C)
#define sGIVEEXP_STATE              (gSharedMem + 0x1600F)
#define sPAINSPLIT_HP               (gSharedMem + 0x16014)
#define sLEARNMOVE_STATE            (gSharedMem + 0x1601A)
#define sSTATCHANGER                (gSharedMem + 0x1601E)
#define sDMG_MULTIPLIER             (gSharedMem + 0x1601F)
#define sTWOTURN_STRINGID           (gSharedMem + 0x16055)
#define sBATTLE_STYLE               (gSharedMem + 0x16084)
#define sBIDE_DMG                   (gSharedMem + 0x16090)
#define sLVLBOX_STATE               (gSharedMem + 0x1609C)
#define sB_ANIM_TARGETS_HIT         (gSharedMem + 0x160A1)
#define sB_ANIM_ARG1                (gSharedMem + 0x160A4)
#define sB_ANIM_ARG2                (gSharedMem + 0x160A5)
#define sSTAT_ANIM_PLAYED           (gSharedMem + 0x160DC)
#define sTRIPLE_KICK_POWER          (gSharedMem + 0x160DE)
#define sMULTIHIT_STRING            (gSharedMem + 0x160E0)
#define sBATTLER_WITH_ABILITY       (gSharedMem + 0x160F8)
#define sMULTIHIT_EFFECT            (gSharedMem + 0x16112)

// array entries for battle communication
#define MULTIUSE_STATE          0x0
#define CURSOR_POSITION         0x1
#define TASK_ID                 0x1 // task Id and cursor position share the same field
#define SPRITES_INIT_STATE1     0x1 // shares the Id as well
#define SPRITES_INIT_STATE2     0x2
#define MOVE_EFFECT_BYTE        0x3
#define ACTIONS_CONFIRMED_COUNT 0x4
#define MULTISTRING_CHOOSER     0x5
#define MISS_TYPE               0x6
#define MSG_DISPLAY             0x7
#define BATTLE_COMMUNICATION_ENTRIES_COUNT  0x8

#define cEFFECT_CHOOSER      (gBattleCommunication + MOVE_EFFECT_BYTE)
#define cMULTISTRING_CHOOSER (gBattleCommunication + MULTISTRING_CHOOSER)
#define cMISS_TYPE           (gBattleCommunication + MISS_TYPE)

#define BS_TARGET                   0
#define BS_ATTACKER                 1
#define BS_EFFECT_BATTLER           2
#define BS_FAINTED                  3
#define BS_ATTACKER_WITH_PARTNER    4 // for Cmd_updatestatusicon
#define BS_BATTLER_0                7
#define BS_ATTACKER_SIDE            8 // for Cmd_jumpifability
#define BS_NOT_ATTACKER_SIDE        9 // for Cmd_jumpifability
#define BS_SCRIPTING                10
#define BS_PLAYER1                  11
#define BS_OPPONENT1                12
#define BS_PLAYER2                  13
#define BS_OPPONENT2                14

#define NO_ACC_CALC 0xFFFE
#define NO_ACC_CALC_CHECK_LOCK_ON 0xFFFF
#define ACC_CURR_MOVE 0

#define CMP_EQUAL               0x0
#define CMP_NOT_EQUAL           0x1
#define CMP_GREATER_THAN        0x2
#define CMP_LESS_THAN           0x3
#define CMP_COMMON_BITS         0x4
#define CMP_NO_COMMON_BITS      0x5

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

#define DMG_CHANGE_SIGN        0
#define DMG_RECOIL_FROM_MISS   1
#define DMG_DOUBLED            2

#define SWITCH_IGNORE_ESCAPE_PREVENTION   0x80

#define STAT_CHANGE_ALLOW_PTR               0x1
#define STAT_CHANGE_NOT_PROTECT_AFFECTED    0x20

#define STAT_CHANGE_NEGATIVE         (1 << 0)
#define STAT_CHANGE_BY_TWO           (1 << 1)
#define STAT_CHANGE_MULTIPLE_STATS   (1 << 2)
#define STAT_CHANGE_CANT_PREVENT     (1 << 3)

// Ruby move-end order: item effects precede visibility updates.
#define MOVEEND_RAGE                            0
#define MOVEEND_DEFROST                         1
#define MOVEEND_SYNCHRONIZE_TARGET              2
#define MOVEEND_ON_DAMAGE_ABILITIES             3
#define MOVEEND_IMMUNITY_ABILITIES              4
#define MOVEEND_SYNCHRONIZE_ATTACKER            5
#define MOVEEND_CHOICE_MOVE                     6
#define MOVEEND_CHANGED_ITEMS                   7
#define MOVEEND_ITEM_EFFECTS_ALL                8
#define MOVEEND_KINGSROCK_SHELLBELL             9
#define MOVEEND_ATTACKER_INVISIBLE              10
#define MOVEEND_ATTACKER_VISIBLE                11
#define MOVEEND_TARGET_VISIBLE                  12
#define MOVEEND_SUBSTITUTE                      13
#define MOVEEND_UPDATE_LAST_MOVES               14
#define MOVEEND_MIRROR_MOVE                     15
#define MOVEEND_NEXT_TARGET                     16
#define MOVEEND_COUNT                           17

#endif // GUARD_CONSTANTS_BATTLE_SCRIPT_COMMANDS_H
