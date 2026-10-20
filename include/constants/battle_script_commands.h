#ifndef GUARD_CONSTANTS_BATTLE_SCRIPT_COMMANDS_H
#define GUARD_CONSTANTS_BATTLE_SCRIPT_COMMANDS_H

// Script opcodes. Macros keep this header usable by Ruby's VM assembly.
#define B_SCR_OP_ATTACKCANCELER                     0x00
#define B_SCR_OP_ACCURACYCHECK                      0x01
#define B_SCR_OP_ATTACKSTRING                       0x02
#define B_SCR_OP_PPREDUCE                           0x03
#define B_SCR_OP_CRITCALC                           0x04
#define B_SCR_OP_DAMAGECALC                         0x05
#define B_SCR_OP_TYPECALC                           0x06
#define B_SCR_OP_ADJUSTNORMALDAMAGE                 0x07
#define B_SCR_OP_ADJUSTNORMALDAMAGE2                0x08
#define B_SCR_OP_ATTACKANIMATION                    0x09
#define B_SCR_OP_WAITANIMATION                      0x0A
#define B_SCR_OP_HEALTHBARUPDATE                    0x0B
#define B_SCR_OP_DATAHPUPDATE                       0x0C
#define B_SCR_OP_CRITMESSAGE                        0x0D
#define B_SCR_OP_EFFECTIVENESSSOUND                 0x0E
#define B_SCR_OP_RESULTMESSAGE                      0x0F
#define B_SCR_OP_PRINTSTRING                        0x10
#define B_SCR_OP_PRINTSELECTIONSTRING               0x11
#define B_SCR_OP_WAITMESSAGE                        0x12
#define B_SCR_OP_PRINTFROMTABLE                     0x13
#define B_SCR_OP_PRINTSELECTIONSTRINGFROMTABLE      0x14
#define B_SCR_OP_SETEFFECTWITHCHANCE                0x15
#define B_SCR_OP_SETEFFECTPRIMARY                   0x16
#define B_SCR_OP_SETEFFECTSECONDARY                 0x17
#define B_SCR_OP_CLEARSTATUSFROMEFFECT              0x18
#define B_SCR_OP_TRYFAINTMON                        0x19
#define B_SCR_OP_DOFAINTANIMATION                   0x1A
#define B_SCR_OP_CLEAREFFECTSONFAINT                0x1B
#define B_SCR_OP_JUMPIFSTATUS                       0x1C
#define B_SCR_OP_JUMPIFSTATUS2                      0x1D
#define B_SCR_OP_JUMPIFABILITY                      0x1E
#define B_SCR_OP_JUMPIFSIDEAFFECTING                0x1F
#define B_SCR_OP_JUMPIFSTAT                         0x20
#define B_SCR_OP_JUMPIFSTATUS3CONDITION             0x21
#define B_SCR_OP_JUMPIFTYPE                         0x22
#define B_SCR_OP_GETEXP                             0x23
#define B_SCR_OP_CHECKTEAMSLOST                     0x24
#define B_SCR_OP_MOVEVALUESCLEANUP                  0x25
#define B_SCR_OP_SETMULTIHIT                        0x26
#define B_SCR_OP_DECREMENTMULTIHIT                  0x27
#define B_SCR_OP_GOTO                               0x28
#define B_SCR_OP_JUMPIFBYTE                         0x29
#define B_SCR_OP_JUMPIFHALFWORD                     0x2A
#define B_SCR_OP_JUMPIFWORD                         0x2B
#define B_SCR_OP_JUMPIFARRAYEQUAL                   0x2C
#define B_SCR_OP_JUMPIFARRAYNOTEQUAL                0x2D
#define B_SCR_OP_SETBYTE                            0x2E
#define B_SCR_OP_ADDBYTE                            0x2F
#define B_SCR_OP_SUBBYTE                            0x30
#define B_SCR_OP_COPYARRAY                          0x31
#define B_SCR_OP_COPYARRAYWITHINDEX                 0x32
#define B_SCR_OP_ORBYTE                             0x33
#define B_SCR_OP_ORHALFWORD                         0x34
#define B_SCR_OP_ORWORD                             0x35
#define B_SCR_OP_BICBYTE                            0x36
#define B_SCR_OP_BICHALFWORD                        0x37
#define B_SCR_OP_BICWORD                            0x38
#define B_SCR_OP_PAUSE                              0x39
#define B_SCR_OP_WAITSTATE                          0x3A
#define B_SCR_OP_HEALTHBAR_UPDATE                   0x3B
#define B_SCR_OP_RETURN                             0x3C
#define B_SCR_OP_END                                0x3D
#define B_SCR_OP_END2                               0x3E
#define B_SCR_OP_END3                               0x3F
#define B_SCR_OP_JUMPIFAFFECTEDBYPROTECT            0x40
#define B_SCR_OP_CALL                               0x41
#define B_SCR_OP_JUMPIFTYPE2                        0x42
#define B_SCR_OP_JUMPIFABILITYPRESENT               0x43
#define B_SCR_OP_ENDSELECTIONSCRIPT                 0x44
#define B_SCR_OP_PLAYANIMATION                      0x45
#define B_SCR_OP_PLAYANIMATION_VAR                  0x46
#define B_SCR_OP_SETGRAPHICALSTATCHANGEVALUES       0x47
#define B_SCR_OP_PLAYSTATCHANGEANIMATION            0x48
#define B_SCR_OP_MOVEEND                            0x49
#define B_SCR_OP_TYPECALC2                          0x4A
#define B_SCR_OP_RETURNATKTOBALL                    0x4B
#define B_SCR_OP_GETSWITCHEDMONDATA                 0x4C
#define B_SCR_OP_SWITCHINDATAUPDATE                 0x4D
#define B_SCR_OP_SWITCHINANIM                       0x4E
#define B_SCR_OP_JUMPIFCANTSWITCH                   0x4F
#define B_SCR_OP_OPENPARTYSCREEN                    0x50
#define B_SCR_OP_SWITCHHANDLEORDER                  0x51
#define B_SCR_OP_SWITCHINEFFECTS                    0x52
#define B_SCR_OP_TRAINERSLIDEIN                     0x53
#define B_SCR_OP_PLAYSE                             0x54
#define B_SCR_OP_FANFARE                            0x55
#define B_SCR_OP_PLAYFAINTCRY                       0x56
#define B_SCR_OP_ENDLINKBATTLE                      0x57
#define B_SCR_OP_RETURNTOBALL                       0x58
#define B_SCR_OP_HANDLELEARNNEWMOVE                 0x59
#define B_SCR_OP_YESNOBOXLEARNMOVE                  0x5A
#define B_SCR_OP_YESNOBOXSTOPLEARNINGMOVE           0x5B
#define B_SCR_OP_HITANIMATION                       0x5C
#define B_SCR_OP_GETMONEYREWARD                     0x5D
#define B_SCR_OP_UPDATEBATTLERMOVES                 0x5E
#define B_SCR_OP_SWAPATTACKERWITHTARGET             0x5F
#define B_SCR_OP_INCREMENTGAMESTAT                  0x60
#define B_SCR_OP_DRAWPARTYSTATUSSUMMARY             0x61
#define B_SCR_OP_HIDEPARTYSTATUSSUMMARY             0x62
#define B_SCR_OP_JUMPTOCALLEDMOVE                   0x63
#define B_SCR_OP_STATUSANIMATION                    0x64
#define B_SCR_OP_STATUS2ANIMATION                   0x65
#define B_SCR_OP_CHOSENSTATUSANIMATION              0x66
#define B_SCR_OP_YESNOBOX                           0x67
#define B_SCR_OP_CANCELALLACTIONS                   0x68
#define B_SCR_OP_ADJUSTSETDAMAGE                    0x69
#define B_SCR_OP_REMOVEITEM                         0x6A
#define B_SCR_OP_ATKNAMEINBUFF1                     0x6B
#define B_SCR_OP_DRAWLVLUPBOX                       0x6C
#define B_SCR_OP_RESETSENTMONSVALUE                 0x6D
#define B_SCR_OP_SETATKTOPLAYER0                    0x6E
#define B_SCR_OP_MAKEVISIBLE                        0x6F
#define B_SCR_OP_RECORDLASTABILITY                  0x70
#define B_SCR_OP_BUFFERMOVETOLEARN                  0x71
#define B_SCR_OP_JUMPIFPLAYERRAN                    0x72
#define B_SCR_OP_HPTHRESHOLDS                       0x73
#define B_SCR_OP_HPTHRESHOLDS2                      0x74
#define B_SCR_OP_USEITEMONOPPONENT                  0x75
#define B_SCR_OP_VARIOUS                            0x76
#define B_SCR_OP_SETPROTECTLIKE                     0x77
#define B_SCR_OP_TRYEXPLOSION                       0x78
#define B_SCR_OP_SETATKHPTOZERO                     0x79
#define B_SCR_OP_JUMPIFNEXTTARGETVALID              0x7A
#define B_SCR_OP_TRYHEALHALFHEALTH                  0x7B
#define B_SCR_OP_TRYMIRRORMOVE                      0x7C
#define B_SCR_OP_SETRAIN                            0x7D
#define B_SCR_OP_SETREFLECT                         0x7E
#define B_SCR_OP_SETSEEDED                          0x7F
#define B_SCR_OP_MANIPULATEDAMAGE                   0x80
#define B_SCR_OP_TRYSETREST                         0x81
#define B_SCR_OP_JUMPIFNOTFIRSTTURN                 0x82
#define B_SCR_OP_NOP                                0x83
#define B_SCR_OP_JUMPIFCANTMAKEASLEEP               0x84
#define B_SCR_OP_STOCKPILE                          0x85
#define B_SCR_OP_STOCKPILETOBASEDAMAGE              0x86
#define B_SCR_OP_STOCKPILETOHPHEAL                  0x87
#define B_SCR_OP_NEGATIVEDAMAGE                     0x88
#define B_SCR_OP_STATBUFFCHANGE                     0x89
#define B_SCR_OP_NORMALISEBUFFS                     0x8A
#define B_SCR_OP_SETBIDE                            0x8B
#define B_SCR_OP_CONFUSEIFREPEATINGATTACKENDS       0x8C
#define B_SCR_OP_SETMULTIHITCOUNTER                 0x8D
#define B_SCR_OP_INITMULTIHITSTRING                 0x8E
#define B_SCR_OP_FORCERANDOMSWITCH                  0x8F
#define B_SCR_OP_TRYCONVERSIONTYPECHANGE            0x90
#define B_SCR_OP_GIVEPAYDAYMONEY                    0x91
#define B_SCR_OP_SETLIGHTSCREEN                     0x92
#define B_SCR_OP_TRYKO                              0x93
#define B_SCR_OP_DAMAGETOHALFTARGETHP               0x94
#define B_SCR_OP_SETSANDSTORM                       0x95
#define B_SCR_OP_WEATHERDAMAGE                      0x96
#define B_SCR_OP_TRYINFATUATING                     0x97
#define B_SCR_OP_UPDATESTATUSICON                   0x98
#define B_SCR_OP_SETMIST                            0x99
#define B_SCR_OP_SETFOCUSENERGY                     0x9A
#define B_SCR_OP_TRANSFORMDATAEXECUTION             0x9B
#define B_SCR_OP_SETSUBSTITUTE                      0x9C
#define B_SCR_OP_MIMICATTACKCOPY                    0x9D
#define B_SCR_OP_METRONOME                          0x9E
#define B_SCR_OP_DMGTOLEVEL                         0x9F
#define B_SCR_OP_PSYWAVEDAMAGEEFFECT                0xA0
#define B_SCR_OP_COUNTERDAMAGECALCULATOR            0xA1
#define B_SCR_OP_MIRRORCOATDAMAGECALCULATOR         0xA2
#define B_SCR_OP_DISABLELASTUSEDATTACK              0xA3
#define B_SCR_OP_TRYSETENCORE                       0xA4
#define B_SCR_OP_PAINSPLITDMGCALC                   0xA5
#define B_SCR_OP_SETTYPETORANDOMRESISTANCE          0xA6
#define B_SCR_OP_SETALWAYSHITFLAG                   0xA7
#define B_SCR_OP_COPYMOVEPERMANENTLY                0xA8
#define B_SCR_OP_TRYCHOOSESLEEPTALKMOVE             0xA9
#define B_SCR_OP_SETDESTINYBOND                     0xAA
#define B_SCR_OP_TRYSETDESTINYBONDTOHAPPEN          0xAB
#define B_SCR_OP_REMAININGHPTOPOWER                 0xAC
#define B_SCR_OP_TRYSPITEPPREDUCE                   0xAD
#define B_SCR_OP_HEALPARTYSTATUS                    0xAE
#define B_SCR_OP_CURSETARGET                        0xAF
#define B_SCR_OP_TRYSETSPIKES                       0xB0
#define B_SCR_OP_SETFORESIGHT                       0xB1
#define B_SCR_OP_TRYSETPERISHSONG                   0xB2
#define B_SCR_OP_ROLLOUTDAMAGECALCULATION           0xB3
#define B_SCR_OP_JUMPIFCONFUSEDANDSTATMAXED         0xB4
#define B_SCR_OP_FURYCUTTERCALC                     0xB5
#define B_SCR_OP_FRIENDSHIPTODAMAGECALCULATION      0xB6
#define B_SCR_OP_PRESENTDAMAGECALCULATION           0xB7
#define B_SCR_OP_SETSAFEGUARD                       0xB8
#define B_SCR_OP_MAGNITUDEDAMAGECALCULATION         0xB9
#define B_SCR_OP_JUMPIFNOPURSUITSWITCHDMG           0xBA
#define B_SCR_OP_SETSUNNY                           0xBB
#define B_SCR_OP_MAXATTACKHALVEHP                   0xBC
#define B_SCR_OP_COPYFOESTATS                       0xBD
#define B_SCR_OP_RAPIDSPINFREE                      0xBE
#define B_SCR_OP_SETDEFENSECURLBIT                  0xBF
#define B_SCR_OP_RECOVERBASEDONSUNLIGHT             0xC0
#define B_SCR_OP_HIDDENPOWERCALC                    0xC1
#define B_SCR_OP_SELECTFIRSTVALIDTARGET             0xC2
#define B_SCR_OP_TRYSETFUTUREATTACK                 0xC3
#define B_SCR_OP_TRYDOBEATUP                        0xC4
#define B_SCR_OP_SETSEMIINVULNERABLEBIT             0xC5
#define B_SCR_OP_CLEARSEMIINVULNERABLEBIT           0xC6
#define B_SCR_OP_SETMINIMIZE                        0xC7
#define B_SCR_OP_SETHAIL                            0xC8
#define B_SCR_OP_TRYMEMENTO                         0xC9
#define B_SCR_OP_SETFORCEDTARGET                    0xCA
#define B_SCR_OP_SETCHARGE                          0xCB
#define B_SCR_OP_CALLENVIRONMENTATTACK              0xCC
#define B_SCR_OP_CUREIFBURNEDPARALYZEDORPOISONED    0xCD
#define B_SCR_OP_SETTORMENT                         0xCE
#define B_SCR_OP_JUMPIFNODAMAGE                     0xCF
#define B_SCR_OP_SETTAUNT                           0xD0
#define B_SCR_OP_TRYSETHELPINGHAND                  0xD1
#define B_SCR_OP_TRYSWAPITEMS                       0xD2
#define B_SCR_OP_TRYCOPYABILITY                     0xD3
#define B_SCR_OP_TRYWISH                            0xD4
#define B_SCR_OP_TRYSETROOTS                        0xD5
#define B_SCR_OP_DOUBLEDAMAGEDEALTIFDAMAGED         0xD6
#define B_SCR_OP_SETYAWN                            0xD7
#define B_SCR_OP_SETDAMAGETOHEALTHDIFFERENCE        0xD8
#define B_SCR_OP_SCALEDAMAGEBYHEALTHRATIO           0xD9
#define B_SCR_OP_TRYSWAPABILITIES                   0xDA
#define B_SCR_OP_TRYIMPRISON                        0xDB
#define B_SCR_OP_TRYSETGRUDGE                       0xDC
#define B_SCR_OP_WEIGHTDAMAGECALCULATION            0xDD
#define B_SCR_OP_ASSISTATTACKSELECT                 0xDE
#define B_SCR_OP_TRYSETMAGICCOAT                    0xDF
#define B_SCR_OP_TRYSETSNATCH                       0xE0
#define B_SCR_OP_TRYGETINTIMIDATETARGET             0xE1
#define B_SCR_OP_SWITCHOUTABILITIES                 0xE2
#define B_SCR_OP_JUMPIFHASNOHP                      0xE3
#define B_SCR_OP_GETSECRETPOWEREFFECT               0xE4
#define B_SCR_OP_PICKUP                             0xE5
#define B_SCR_OP_DOCASTFORMCHANGEANIMATION          0xE6
#define B_SCR_OP_TRYCASTFORMDATACHANGE              0xE7
#define B_SCR_OP_SETTYPEBASEDHALVERS                0xE8
#define B_SCR_OP_SETWEATHERBALLTYPE                 0xE9
#define B_SCR_OP_TRYRECYCLEITEM                     0xEA
#define B_SCR_OP_SETTYPETOENVIRONMENT               0xEB
#define B_SCR_OP_PURSUITDOUBLES                     0xEC
#define B_SCR_OP_SNATCHSETBATTLERS                  0xED
#define B_SCR_OP_REMOVELIGHTSCREENREFLECT           0xEE
#define B_SCR_OP_HANDLEBALLTHROW                    0xEF
#define B_SCR_OP_GIVECAUGHTMON                      0xF0
#define B_SCR_OP_TRYSETCAUGHTMONDEXFLAGS            0xF1
#define B_SCR_OP_DISPLAYDEXINFO                     0xF2
#define B_SCR_OP_TRYGIVECAUGHTMONNICK               0xF3
#define B_SCR_OP_SUBATTACKERHPBYDMG                 0xF4
#define B_SCR_OP_REMOVEATTACKERSTATUS1              0xF5
#define B_SCR_OP_FINISHACTION                       0xF6
#define B_SCR_OP_FINISHTURN                         0xF7

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
#define BS_FAINTED_LINK_MULTIPLE_1  5 // for openpartyscreen
#define BS_FAINTED_LINK_MULTIPLE_2  6 // for openpartyscreen
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
#define VARIOUS_GET_BATTLER_FAINTED             4
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

#define PARTY_SCREEN_OPTIONAL (1 << 7) // Flag for first argument to openpartyscreen
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
