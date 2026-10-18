#include "constants/abilities.h"
#include "constants/battle.h"
#include "constants/battle_anim.h"
#include "constants/battle_script_commands.h"
#include "constants/moves.h"
#include "constants/songs.h"
#include "constants/pokemon.h"
	.include "asm/macros.inc"
	.include "asm/macros/battle_script.inc"
	.include "constants/constants.inc"
	.include "constants/battle_script_constants.inc"

	.section script_data, "aw", %progbits

	.align 2
gBattleScriptsForMoveEffects:: @ 81D6BBC
	.4byte BattleScript_EffectHit
	.4byte BattleScript_EffectSleep
	.4byte BattleScript_EffectPoisonHit
	.4byte BattleScript_EffectAbsorb
	.4byte BattleScript_EffectBurnHit
	.4byte BattleScript_EffectFreezeHit
	.4byte BattleScript_EffectParalyzeHit
	.4byte BattleScript_EffectExplosion
	.4byte BattleScript_EffectDreamEater
	.4byte BattleScript_EffectMirrorMove
	.4byte BattleScript_EffectAttackUp
	.4byte BattleScript_EffectDefenseUp
	.4byte BattleScript_EffectSpeedUp
	.4byte BattleScript_EffectSpecialAttackUp
	.4byte BattleScript_EffectSpecialDefenseUp
	.4byte BattleScript_EffectAccuracyUp
	.4byte BattleScript_EffectEvasionUp
	.4byte BattleScript_EffectAlwaysHit
	.4byte BattleScript_EffectAttackDown
	.4byte BattleScript_EffectDefenseDown
	.4byte BattleScript_EffectSpeedDown
	.4byte BattleScript_EffectSpecialAttackDown
	.4byte BattleScript_EffectSpecialDefenseDown
	.4byte BattleScript_EffectAccuracyDown
	.4byte BattleScript_EffectEvasionDown
	.4byte BattleScript_EffectHaze
	.4byte BattleScript_EffectBide
	.4byte BattleScript_EffectRampage
	.4byte BattleScript_EffectRoar
	.4byte BattleScript_EffectMultiHit
	.4byte BattleScript_EffectConversion
	.4byte BattleScript_EffectFlinchHit
	.4byte BattleScript_EffectRestoreHp
	.4byte BattleScript_EffectToxic
	.4byte BattleScript_EffectPayDay
	.4byte BattleScript_EffectLightScreen
	.4byte BattleScript_EffectTriAttack
	.4byte BattleScript_EffectRest
	.4byte BattleScript_EffectOHKO
	.4byte BattleScript_EffectRazorWind
	.4byte BattleScript_EffectSuperFang
	.4byte BattleScript_EffectDragonRage
	.4byte BattleScript_EffectTrap
	.4byte BattleScript_EffectHighCritical
	.4byte BattleScript_EffectDoubleHit
	.4byte BattleScript_EffectRecoilIfMiss
	.4byte BattleScript_EffectMist
	.4byte BattleScript_EffectFocusEnergy
	.4byte BattleScript_EffectRecoil
	.4byte BattleScript_EffectConfuse
	.4byte BattleScript_EffectAttackUp2
	.4byte BattleScript_EffectDefenseUp2
	.4byte BattleScript_EffectSpeedUp2
	.4byte BattleScript_EffectSpecialAttackUp2
	.4byte BattleScript_EffectSpecialDefenseUp2
	.4byte BattleScript_EffectAccuracyUp2
	.4byte BattleScript_EffectEvasionUp2
	.4byte BattleScript_EffectTransform
	.4byte BattleScript_EffectAttackDown2
	.4byte BattleScript_EffectDefenseDown2
	.4byte BattleScript_EffectSpeedDown2
	.4byte BattleScript_EffectSpecialAttackDown2
	.4byte BattleScript_EffectSpecialDefenseDown2
	.4byte BattleScript_EffectAccuracyDown2
	.4byte BattleScript_EffectEvasionDown2
	.4byte BattleScript_EffectReflect
	.4byte BattleScript_EffectPoison
	.4byte BattleScript_EffectParalyze
	.4byte BattleScript_EffectAttackDownHit
	.4byte BattleScript_EffectDefenseDownHit
	.4byte BattleScript_EffectSpeedDownHit
	.4byte BattleScript_EffectSpecialAttackDownHit
	.4byte BattleScript_EffectSpecialDefenseDownHit
	.4byte BattleScript_EffectAccuracyDownHit
	.4byte BattleScript_EffectEvasionDownHit
	.4byte BattleScript_EffectSkyAttack
	.4byte BattleScript_EffectConfuseHit
	.4byte BattleScript_EffectTwineedle
	.4byte BattleScript_EffectVitalThrow
	.4byte BattleScript_EffectSubstitute
	.4byte BattleScript_EffectRecharge
	.4byte BattleScript_EffectRage
	.4byte BattleScript_EffectMimic
	.4byte BattleScript_EffectMetronome
	.4byte BattleScript_EffectLeechSeed
	.4byte BattleScript_EffectSplash
	.4byte BattleScript_EffectDisable
	.4byte BattleScript_EffectLevelDamage
	.4byte BattleScript_EffectPsywave
	.4byte BattleScript_EffectCounter
	.4byte BattleScript_EffectEncore
	.4byte BattleScript_EffectPainSplit
	.4byte BattleScript_EffectSnore
	.4byte BattleScript_EffectConversion2
	.4byte BattleScript_EffectLockOn
	.4byte BattleScript_EffectSketch
	.4byte BattleScript_EffectUnused60//Thaw
	.4byte BattleScript_EffectSleepTalk
	.4byte BattleScript_EffectDestinyBond
	.4byte BattleScript_EffectFlail
	.4byte BattleScript_EffectSpite
	.4byte BattleScript_EffectFalseSwipe
	.4byte BattleScript_EffectHealBell
	.4byte BattleScript_EffectQuickAttack
	.4byte BattleScript_EffectTripleKick
	.4byte BattleScript_EffectThief
	.4byte BattleScript_EffectMeanLook
	.4byte BattleScript_EffectNightmare
	.4byte BattleScript_EffectMinimize
	.4byte BattleScript_EffectCurse
	.4byte BattleScript_EffectUnused6E
	.4byte BattleScript_EffectProtect
	.4byte BattleScript_EffectSpikes
	.4byte BattleScript_EffectForesight
	.4byte BattleScript_EffectPerishSong
	.4byte BattleScript_EffectSandstorm
	.4byte BattleScript_EffectEndure
	.4byte BattleScript_EffectRollout
	.4byte BattleScript_EffectSwagger
	.4byte BattleScript_EffectFuryCutter
	.4byte BattleScript_EffectAttract
	.4byte BattleScript_EffectReturn
	.4byte BattleScript_EffectPresent
	.4byte BattleScript_EffectFrustration
	.4byte BattleScript_EffectSafeguard
	.4byte BattleScript_EffectThawHit
	.4byte BattleScript_EffectMagnitude
	.4byte BattleScript_EffectBatonPass
	.4byte BattleScript_EffectPursuit
	.4byte BattleScript_EffectRapidSpin
	.4byte BattleScript_EffectSonicboom
	.4byte BattleScript_EffectUnused83
	.4byte BattleScript_EffectMorningSun
	.4byte BattleScript_EffectSynthesis
	.4byte BattleScript_EffectMoonlight
	.4byte BattleScript_EffectHiddenPower
	.4byte BattleScript_EffectRainDance
	.4byte BattleScript_EffectSunnyDay
	.4byte BattleScript_EffectDefenseUpHit
	.4byte BattleScript_EffectAttackUpHit
	.4byte BattleScript_EffectAllStatsUpHit
	.4byte BattleScript_EffectUnused8D
	.4byte BattleScript_EffectBellyDrum
	.4byte BattleScript_EffectPsychUp
	.4byte BattleScript_EffectMirrorCoat
	.4byte BattleScript_EffectSkullBash
	.4byte BattleScript_EffectTwister
	.4byte BattleScript_EffectEarthquake
	.4byte BattleScript_EffectFutureSight
	.4byte BattleScript_EffectGust
	.4byte BattleScript_EffectStomp
	.4byte BattleScript_EffectSolarBeam
	.4byte BattleScript_EffectThunder
	.4byte BattleScript_EffectTeleport
	.4byte BattleScript_EffectBeatUp
	.4byte BattleScript_EffectSemiInvulnerable
	.4byte BattleScript_EffectDefenseCurl
	.4byte BattleScript_EffectSoftboiled
	.4byte BattleScript_EffectFakeOut
	.4byte BattleScript_EffectUproar
	.4byte BattleScript_EffectStockpile
	.4byte BattleScript_EffectSpitUp
	.4byte BattleScript_EffectSwallow
	.4byte BattleScript_EffectUnusedA3
	.4byte BattleScript_EffectHail
	.4byte BattleScript_EffectTorment
	.4byte BattleScript_EffectFlatter
	.4byte BattleScript_EffectWillOWisp
	.4byte BattleScript_EffectMemento
	.4byte BattleScript_EffectFacade
	.4byte BattleScript_EffectFocusPunch
	.4byte BattleScript_EffectSmellingsalt
	.4byte BattleScript_EffectFollowMe
	.4byte BattleScript_EffectNaturePower
	.4byte BattleScript_EffectCharge
	.4byte BattleScript_EffectTaunt
	.4byte BattleScript_EffectHelpingHand
	.4byte BattleScript_EffectTrick
	.4byte BattleScript_EffectRolePlay
	.4byte BattleScript_EffectWish
	.4byte BattleScript_EffectAssist
	.4byte BattleScript_EffectIngrain
	.4byte BattleScript_EffectSuperpower
	.4byte BattleScript_EffectMagicCoat
	.4byte BattleScript_EffectRecycle
	.4byte BattleScript_EffectRevenge
	.4byte BattleScript_EffectBrickBreak
	.4byte BattleScript_EffectYawn
	.4byte BattleScript_EffectKnockOff
	.4byte BattleScript_EffectEndeavor
	.4byte BattleScript_EffectEruption
	.4byte BattleScript_EffectSkillSwap
	.4byte BattleScript_EffectImprison
	.4byte BattleScript_EffectRefresh
	.4byte BattleScript_EffectGrudge
	.4byte BattleScript_EffectSnatch
	.4byte BattleScript_EffectLowKick
	.4byte BattleScript_EffectSecretPower
	.4byte BattleScript_EffectDoubleEdge
	.4byte BattleScript_EffectTeeterDance
	.4byte BattleScript_EffectBlazeKick
	.4byte BattleScript_EffectMudSport
	.4byte BattleScript_EffectPoisonFang
	.4byte BattleScript_EffectWeatherBall
	.4byte BattleScript_EffectOverheat
	.4byte BattleScript_EffectTickle
	.4byte BattleScript_EffectCosmicPower
	.4byte BattleScript_EffectSkyUppercut
	.4byte BattleScript_EffectBulkUp
	.4byte BattleScript_EffectPoisonTail
	.4byte BattleScript_EffectWaterSport
	.4byte BattleScript_EffectCalmMind
	.4byte BattleScript_EffectDragonDance
	.4byte BattleScript_EffectCamouflage

BattleScript_EffectHit: @ 81D6F14
BattleScript_EffectAccuracyDown2: @ 81D6F14
BattleScript_EffectAccuracyUp2: @ 81D6F14
BattleScript_EffectAccuracyUp: @ 81D6F14
BattleScript_EffectAlwaysHit: @ 81D6F14
BattleScript_EffectEvasionDown2: @ 81D6F14
BattleScript_EffectEvasionDownHit: @ 81D6F14
BattleScript_EffectEvasionUp2: @ 81D6F14
BattleScript_EffectFalseSwipe: @ 81D6F14
BattleScript_EffectHighCritical: @ 81D6F14
BattleScript_EffectHit: @ 81D6F14
BattleScript_EffectPursuit: @ 81D6F14
BattleScript_EffectQuickAttack: @ 81D6F14
BattleScript_EffectSpecialAttackDown2: @ 81D6F14
BattleScript_EffectSpecialAttackDown: @ 81D6F14
BattleScript_EffectSpecialDefenseDown: @ 81D6F14
BattleScript_EffectSpecialDefenseUp: @ 81D6F14
BattleScript_EffectSpeedUp: @ 81D6F14
BattleScript_EffectUnused60: @ 81D6F14
BattleScript_EffectUnused6E: @ 81D6F14
BattleScript_EffectUnused83: @ 81D6F14
BattleScript_EffectUnused8D: @ 81D6F14
BattleScript_EffectUnusedA3: @ 81D6F14
BattleScript_EffectVitalThrow: @ 81D6F14
	jumpifnotmove MOVE_SURF, BattleScript_HitFromAtkCanceler
	jumpifnostatus3 BS_TARGET, STATUS3_UNDERWATER, BattleScript_HitFromAtkCanceler
	orword gHitMarker, HITMARKER_IGNORE_UNDERWATER
	setbyte sDMG_MULTIPLIER, 2

BattleScript_HitFromAtkCanceler: @ 81D6F3A
	attackcanceler

BattleScript_HitFromAccCheck: @ 81D6F3B
	accuracycheck BattleScript_PrintMoveMissed, ACC_CURR_MOVE

BattleScript_HitFromAtkString: @ 81D6F42
	attackstring
	ppreduce

BattleScript_HitFromCritCalc:: @ 81D6F44
	critcalc
	damagecalc
	typecalc
	adjustnormaldamage

BattleScript_HitFromAtkAnimation: @ 81D6F48
	attackanimation
	waitanimation
	effectivenesssound
	hitanimation BS_TARGET
	waitstate
	healthbarupdate BS_TARGET
	datahpupdate BS_TARGET
	critmessage
	waitmessage 64
	resultmessage
	waitmessage 64
	seteffectwithchance
	tryfaintmon BS_TARGET, FALSE, NULL

BattleScript_MoveEnd:: @ 81D6F62
	setbyte sMOVEEND_STATE, 0
	moveend 0, 0
	end

BattleScript_MakeMoveMissed: @ 81D6F6C
	orbyte gMoveResultFlags, MOVE_RESULT_MISSED

BattleScript_PrintMoveMissed: @ 81D6F72
	attackstring
	ppreduce

BattleScript_MoveMissedPause:: @ 81D6F74
	pause 32

BattleScript_MoveMissed: @ 81D6F77
	effectivenesssound
	resultmessage
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectSleep: @ 81D6F81
	attackcanceler
	attackstring
	ppreduce
	jumpifstatus2 BS_TARGET, STATUS2_SUBSTITUTE, BattleScript_ButItFailed
	jumpifstatus BS_TARGET, STATUS1_SLEEP, BattleScript_AlreadyAsleep
	jumpifcantmakeasleep BattleScript_CantMakeAsleep
	jumpifstatus BS_TARGET, STATUS1_SLEEP | STATUS1_POISON | STATUS1_BURN | STATUS1_FREEZE | STATUS1_PARALYSIS | STATUS1_TOXIC_POISON, BattleScript_ButItFailed
	accuracycheck BattleScript_ButItFailed, ACC_CURR_MOVE
	jumpifsideaffecting BS_TARGET, SIDE_STATUS_SAFEGUARD, BattleScript_SafeguardProtected
	attackanimation
	waitanimation
	setmoveeffect MOVE_EFFECT_SLEEP
	seteffectprimary
	goto BattleScript_MoveEnd

BattleScript_AlreadyAsleep: @ 81D6FC4
	pause 32
	printstring STRINGID_PKMNALREADYASLEEP
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_WasntAffected: @ 81D6FD2
	pause 32
	printstring STRINGID_PKMNWASNTAFFECTED
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_CantMakeAsleep: @ 81D6FE0
	pause 32
	printfromtable gUproarAwakeStringIds
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectPoisonHit: @ 81D6FF0
BattleScript_EffectPoisonTail: @ 81D6FF0
	setmoveeffect MOVE_EFFECT_POISON
	goto BattleScript_EffectHit

BattleScript_EffectAbsorb: @ 81D6FFB
	attackcanceler
	accuracycheck BattleScript_PrintMoveMissed, ACC_CURR_MOVE
	attackstring
	ppreduce
	critcalc
	damagecalc
	typecalc
	adjustnormaldamage
	attackanimation
	waitanimation
	effectivenesssound
	hitanimation BS_TARGET
	waitstate
	healthbarupdate BS_TARGET
	datahpupdate BS_TARGET
	critmessage
	waitmessage 64
	resultmessage
	waitmessage 64
	negativedamage
	orword gHitMarker, HITMARKER_IGNORE_SUBSTITUTE
	jumpifability BS_TARGET, ABILITY_LIQUID_OOZE, BattleScript_AbsorbLiquidOoze
	setbyte cMULTISTRING_CHOOSER, 0
	goto BattleScript_AbsorbUpdateHp

BattleScript_AbsorbLiquidOoze: @ 81D7037
	manipulatedamage 0
	setbyte cMULTISTRING_CHOOSER, 1

BattleScript_AbsorbUpdateHp: @ 81D703F
	healthbarupdate BS_ATTACKER
	datahpupdate BS_ATTACKER
	jumpifmovehadnoeffect BattleScript_AbsorbTryFainting
	printfromtable gLeechSeedDrainStringIds
	waitmessage 64

BattleScript_AbsorbTryFainting: @ 81D7056
	tryfaintmon BS_ATTACKER, FALSE, NULL
	tryfaintmon BS_TARGET, FALSE, NULL
	goto BattleScript_MoveEnd

BattleScript_EffectBlazeKick: @ 81D7069
BattleScript_EffectBurnHit: @ 81D7069
	setmoveeffect MOVE_EFFECT_BURN
	goto BattleScript_EffectHit

BattleScript_EffectFreezeHit: @ 81D7074
	setmoveeffect MOVE_EFFECT_FREEZE
	goto BattleScript_EffectHit

BattleScript_EffectParalyzeHit: @ 81D707F
	setmoveeffect MOVE_EFFECT_PARALYSIS
	goto BattleScript_EffectHit

BattleScript_EffectExplosion: @ 81D708A
	attackcanceler
	attackstring
	ppreduce
	tryexplosion
	setatkhptozero
	waitstate
	jumpifbyte CMP_NO_COMMON_BITS, gMoveResultFlags, MOVE_RESULT_MISSED, BattleScript_ExplosionDoAnimStartLoop
	call BattleScript_PreserveMissedBitDoMoveAnim
	goto BattleScript_ExplosionLoop

BattleScript_ExplosionDoAnimStartLoop: @ 81D70A5
	attackanimation
	waitanimation

BattleScript_ExplosionLoop: @ 81D70A7
	movevaluescleanup
	critcalc
	damagecalc
	typecalc
	adjustnormaldamage
	accuracycheck BattleScript_ExplosionMissed, ACC_CURR_MOVE
	effectivenesssound
	hitanimation BS_TARGET
	waitstate
	healthbarupdate BS_TARGET
	datahpupdate BS_TARGET
	critmessage
	waitmessage 64
	resultmessage
	waitmessage 64
	tryfaintmon BS_TARGET, FALSE, NULL
	setbyte sMOVEEND_STATE, 0
	moveend 2, 16
	jumpifnexttargetvalid BattleScript_ExplosionLoop
	tryfaintmon BS_ATTACKER, FALSE, NULL
	end

BattleScript_ExplosionMissed: @ 81D70E0
	effectivenesssound
	resultmessage
	waitmessage 64
	setbyte sMOVEEND_STATE, 0
	moveend 2, 16
	jumpifnexttargetvalid BattleScript_ExplosionLoop
	tryfaintmon BS_ATTACKER, FALSE, NULL
	end

BattleScript_PreserveMissedBitDoMoveAnim: @ 81D70FB
	bicbyte gMoveResultFlags, MOVE_RESULT_MISSED
	attackanimation
	waitanimation
	orbyte gMoveResultFlags, MOVE_RESULT_MISSED
	return

BattleScript_EffectDreamEater: @ 81D710A
	attackcanceler
	jumpifstatus2 BS_TARGET, STATUS2_SUBSTITUTE, BattleScript_DreamEaterNoEffect
	jumpifstatus BS_TARGET, STATUS1_SLEEP, BattleScript_DreamEaterWorked

BattleScript_DreamEaterNoEffect: @ 81D711F
	attackstring
	ppreduce
	waitmessage 64
	goto BattleScript_WasntAffected

BattleScript_DreamEaterWorked: @ 81D7129
	accuracycheck BattleScript_PrintMoveMissed, ACC_CURR_MOVE
	attackstring
	ppreduce
	critcalc
	damagecalc
	typecalc
	adjustnormaldamage
	attackanimation
	waitanimation
	effectivenesssound
	hitanimation BS_TARGET
	waitstate
	healthbarupdate BS_TARGET
	datahpupdate BS_TARGET
	critmessage
	waitmessage 64
	resultmessage
	waitmessage 64
	negativedamage
	orword gHitMarker, HITMARKER_IGNORE_SUBSTITUTE
	healthbarupdate BS_ATTACKER
	datahpupdate BS_ATTACKER
	jumpifmovehadnoeffect BattleScript_DreamEaterTryFaintEnd
	printstring STRINGID_PKMNDREAMEATEN
	waitmessage 64

BattleScript_DreamEaterTryFaintEnd: @ 81D7167
	tryfaintmon BS_TARGET, FALSE, NULL
	goto BattleScript_MoveEnd

BattleScript_EffectMirrorMove: @ 81D7173
	attackcanceler
	attackstring
	pause 64
	trymirrormove
	ppreduce
	orbyte gMoveResultFlags, MOVE_RESULT_FAILED
	printstring STRINGID_MIRRORMOVEFAILED
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectAttackUp: @ 81D718B
	setstatchanger STAT_ATK, 1, FALSE
	goto BattleScript_EffectStatUp

BattleScript_EffectDefenseUp: @ 81D7196
	setstatchanger STAT_DEF, 1, FALSE
	goto BattleScript_EffectStatUp

BattleScript_EffectSpecialAttackUp: @ 81D71A1
	setstatchanger STAT_SPATK, 1, FALSE
	goto BattleScript_EffectStatUp

BattleScript_EffectEvasionUp: @ 81D71AC
	setstatchanger STAT_EVASION, 1, FALSE

BattleScript_EffectStatUp: @ 81D71B2
	attackcanceler

BattleScript_EffectStatUpAfterAtkCanceler: @ 81D71B3
	attackstring
	ppreduce
	statbuffchange MOVE_EFFECT_AFFECTS_USER | 0x1, BattleScript_StatUpEnd
	jumpifbyte CMP_NOT_EQUAL, cMULTISTRING_CHOOSER, 2, BattleScript_StatUpAttackAnim
	pause 32
	goto BattleScript_StatUpPrintString

BattleScript_StatUpAttackAnim: @ 81D71CE
	attackanimation
	waitanimation

BattleScript_StatUpDoAnim: @ 81D71D0
	setgraphicalstatchangevalues
	playanimation BS_ATTACKER, B_ANIM_STATS_CHANGE, sB_ANIM_ARG1

BattleScript_StatUpPrintString: @ 81D71D8
	printfromtable gStatUpStringIds
	waitmessage 64

BattleScript_StatUpEnd: @ 81D71E0
	goto BattleScript_MoveEnd

BattleScript_StatUp:: @ 81D71E5
	playanimation 2, B_ANIM_STATS_CHANGE, sB_ANIM_ARG1
	printfromtable gStatUpStringIds
	waitmessage 64
	return

BattleScript_EffectAttackDown: @ 81D71F5
	setstatchanger STAT_ATK, 1, TRUE
	goto BattleScript_EffectStatDown

BattleScript_EffectDefenseDown: @ 81D7200
	setstatchanger STAT_DEF, 1, TRUE
	goto BattleScript_EffectStatDown

BattleScript_EffectSpeedDown: @ 81D720B
	setstatchanger STAT_SPEED, 1, TRUE
	goto BattleScript_EffectStatDown

BattleScript_EffectAccuracyDown: @ 81D7216
	setstatchanger STAT_ACC, 1, TRUE
	goto BattleScript_EffectStatDown

BattleScript_EffectEvasionDown: @ 81D7221
	setstatchanger STAT_EVASION, 1, TRUE

BattleScript_EffectStatDown: @ 81D7227
	attackcanceler
	jumpifstatus2 BS_TARGET, STATUS2_SUBSTITUTE, BattleScript_FailedFromAtkString
	accuracycheck BattleScript_PrintMoveMissed, ACC_CURR_MOVE
	attackstring
	ppreduce
	statbuffchange 1, BattleScript_StatDownEnd
	jumpifbyte CMP_LESS_THAN, cMULTISTRING_CHOOSER, 2, BattleScript_StatDownDoAnim
	jumpifbyte CMP_EQUAL, cMULTISTRING_CHOOSER, 3, BattleScript_StatDownEnd
	pause 32
	goto BattleScript_StatDownPrintString

BattleScript_StatDownDoAnim: @ 81D725F
	attackanimation
	waitanimation
	setgraphicalstatchangevalues
	playanimation BS_TARGET, B_ANIM_STATS_CHANGE, sB_ANIM_ARG1

BattleScript_StatDownPrintString: @ 81D7269
	printfromtable gStatDownStringIds
	waitmessage 64

BattleScript_StatDownEnd: @ 81D7271
	goto BattleScript_MoveEnd

BattleScript_StatDown:: @ 81D7276
	playanimation 2, B_ANIM_STATS_CHANGE, sB_ANIM_ARG1
	printfromtable gStatDownStringIds
	waitmessage 64
	return

BattleScript_EffectHaze: @ 81D7286
	attackcanceler
	attackstring
	ppreduce
	attackanimation
	waitanimation
	normalisebuffs
	printstring STRINGID_STATCHANGESGONE
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectBide: @ 81D7297
	attackcanceler
	attackstring
	ppreduce
	attackanimation
	waitanimation
	orword gHitMarker, HITMARKER_CHARGING
	setbide
	goto BattleScript_MoveEnd

BattleScript_EffectRampage: @ 81D72AB
	attackcanceler
	accuracycheck BattleScript_PrintMoveMissed, ACC_CURR_MOVE
	attackstring
	jumpifstatus2 BS_ATTACKER, STATUS2_MULTIPLETURNS, BattleScript_EffectRampage2
	ppreduce

BattleScript_EffectRampage2: @ 81D72BF
	confuseifrepeatingattackends
	goto BattleScript_HitFromCritCalc

BattleScript_EffectRoar: @ 81D72C5
	attackcanceler
	attackstring
	ppreduce
	jumpifability BS_TARGET, ABILITY_SUCTION_CUPS, BattleScript_AbilityPreventsPhasingOut
	jumpifstatus3 BS_TARGET, STATUS3_ROOTED, BattleScript_PrintMonIsRooted
	accuracycheck BattleScript_ButItFailed, NO_ACC_CALC_CHECK_LOCK_ON
	accuracycheck BattleScript_MoveMissed, ACC_CURR_MOVE
	forcerandomswitch BattleScript_ButItFailed

BattleScript_EffectMultiHit: @ 81D72ED
	attackcanceler
	accuracycheck BattleScript_PrintMoveMissed, ACC_CURR_MOVE
	attackstring
	ppreduce
	setmultihitcounter 0
	initmultihitstring
	setbyte sMULTIHIT_EFFECT, 0

BattleScript_MultiHitLoop: @ 81D7300
	jumpifhasnohp BS_ATTACKER, BattleScript_MultiHitEnd
	jumpifhasnohp BS_TARGET, BattleScript_MultiHitPrintStrings
	jumpifhalfword CMP_EQUAL, gChosenMove, MOVE_SLEEP_TALK, BattleScript_DoMultiHit
	jumpifstatus BS_ATTACKER, STATUS1_SLEEP, BattleScript_MultiHitPrintStrings

BattleScript_DoMultiHit: @ 81D7322
	movevaluescleanup
	copybyte cEFFECT_CHOOSER, sMULTIHIT_EFFECT
	critcalc
	damagecalc
	typecalc
	jumpifmovehadnoeffect BattleScript_MultiHitNoMoreHits
	adjustnormaldamage
	attackanimation
	waitanimation
	effectivenesssound
	hitanimation BS_TARGET
	waitstate
	healthbarupdate BS_TARGET
	datahpupdate BS_TARGET
	critmessage
	waitmessage 64
	printstring STRINGID_EMPTYSTRING3
	waitmessage 1
	addbyte sMULTIHIT_STRING + 4, 1
	setbyte sMOVEEND_STATE, 0
	moveend 2, 16
	jumpifbyte CMP_COMMON_BITS, gMoveResultFlags, MOVE_RESULT_FOE_ENDURED, BattleScript_MultiHitPrintStrings
	decrementmultihit BattleScript_MultiHitLoop
	goto BattleScript_MultiHitPrintStrings

BattleScript_MultiHitNoMoreHits: @ 81D7374
	pause 32

BattleScript_MultiHitPrintStrings: @ 81D7377
	resultmessage
	waitmessage 64
	jumpifmovehadnoeffect BattleScript_MultiHitEnd
	copyarray gBattleTextBuff1, sMULTIHIT_STRING, 6
	printstring STRINGID_HITXTIMES
	waitmessage 64

BattleScript_MultiHitEnd: @ 81D7396
	seteffectwithchance
	tryfaintmon BS_TARGET, FALSE, NULL
	setbyte sMOVEEND_STATE, 2
	moveend 1, 0
	setbyte sMOVEEND_STATE, 4
	moveend 0, 0
	end

BattleScript_EffectConversion: @ 81D73B1
	attackcanceler
	attackstring
	ppreduce
	tryconversiontypechange BattleScript_ButItFailed
	attackanimation
	waitanimation
	printstring STRINGID_PKMNCHANGEDTYPE
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectFlinchHit: @ 81D73C6
	setmoveeffect MOVE_EFFECT_FLINCH
	goto BattleScript_EffectHit

BattleScript_EffectRestoreHp: @ 81D73D1
	attackcanceler
	attackstring
	ppreduce
	tryhealhalfhealth BattleScript_AlreadyAtFullHp, 1
	attackanimation
	waitanimation
	orword gHitMarker, HITMARKER_IGNORE_SUBSTITUTE
	healthbarupdate BS_ATTACKER
	datahpupdate BS_ATTACKER
	printstring STRINGID_PKMNREGAINEDHEALTH
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectToxic: @ 81D73F4
	attackcanceler
	attackstring
	ppreduce
	jumpifability BS_TARGET, ABILITY_IMMUNITY, BattleScript_ImmunityProtected
	jumpifstatus2 BS_TARGET, STATUS2_SUBSTITUTE, BattleScript_ButItFailed
	jumpifstatus BS_TARGET, STATUS1_POISON, BattleScript_AlreadyPoisoned
	jumpifstatus BS_TARGET, STATUS1_TOXIC_POISON, BattleScript_AlreadyPoisoned
	jumpifstatus BS_TARGET, STATUS1_SLEEP | STATUS1_POISON | STATUS1_BURN | STATUS1_FREEZE | STATUS1_PARALYSIS | STATUS1_TOXIC_POISON, BattleScript_ButItFailed
	jumpiftype BS_TARGET, TYPE_POISON, BattleScript_NotAffected
	jumpiftype BS_TARGET, TYPE_STEEL, BattleScript_NotAffected
	accuracycheck BattleScript_ButItFailed, ACC_CURR_MOVE
	jumpifsideaffecting BS_TARGET, SIDE_STATUS_SAFEGUARD, BattleScript_SafeguardProtected
	attackanimation
	waitanimation
	setmoveeffect MOVE_EFFECT_TOXIC
	seteffectprimary
	resultmessage
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_AlreadyPoisoned: @ 81D7455
	pause 64
	printstring STRINGID_PKMNALREADYPOISONED
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_ImmunityProtected: @ 81D7463
	copybyte gEffectBattler, gBattlerTarget
	setbyte cMULTISTRING_CHOOSER, 0
	call BattleScript_PSNPrevention
	goto BattleScript_MoveEnd

BattleScript_EffectPayDay: @ 81D747D
	setmoveeffect MOVE_EFFECT_PAYDAY
	goto BattleScript_EffectHit

BattleScript_EffectLightScreen: @ 81D7488
	attackcanceler
	attackstring
	ppreduce
	setlightscreen
	goto BattleScript_PrintReflectLightScreenSafeguardString

BattleScript_EffectTriAttack: @ 81D7491
	setmoveeffect MOVE_EFFECT_TRI_ATTACK
	goto BattleScript_EffectHit

BattleScript_EffectRest: @ 81D749C
	attackcanceler
	attackstring
	ppreduce
	jumpifstatus BS_ATTACKER, STATUS1_SLEEP, BattleScript_RestIsAlreadyAsleep
	jumpifcantmakeasleep BattleScript_RestCantSleep
	trysetrest BattleScript_AlreadyAtFullHp
	pause 32
	printfromtable gRestUsedStringIds
	waitmessage 64
	updatestatusicon BS_ATTACKER
	waitstate
	goto BattleScript_PresentHealTarget

BattleScript_RestCantSleep: @ 81D74C6
	pause 64
	printfromtable gUproarAwakeStringIds
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_RestIsAlreadyAsleep: @ 81D74D6
	pause 32
	printstring STRINGID_PKMNALREADYASLEEP2
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectOHKO: @ 81D74E4
	attackcanceler
	attackstring
	ppreduce
	accuracycheck BattleScript_ButItFailed, NO_ACC_CALC_CHECK_LOCK_ON
	typecalc
	jumpifmovehadnoeffect BattleScript_HitFromAtkAnimation
	tryKO BattleScript_KOFail
	trysetdestinybondtohappen
	goto BattleScript_HitFromAtkAnimation

BattleScript_KOFail: @ 81D7505
	pause 64
	printfromtable gKOFailedStringIds
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectRazorWind: @ 81D7515
	jumpifstatus2 BS_ATTACKER, STATUS2_MULTIPLETURNS, BattleScript_TwoTurnMovesSecondTurn
	jumpifword CMP_COMMON_BITS, gHitMarker, HITMARKER_NO_ATTACKSTRING, BattleScript_TwoTurnMovesSecondTurn
	setbyte sTWOTURN_STRINGID, 0
	call BattleScriptFirstChargingTurn
	goto BattleScript_MoveEnd

BattleScript_TwoTurnMovesSecondTurn: @ 81D753D
	attackcanceler
	setmoveeffect MOVE_EFFECT_CHARGING
	setbyte sB_ANIM_TURN, 1
	clearstatusfromeffect BS_ATTACKER
	orword gHitMarker, HITMARKER_NO_PPDEDUCT
	jumpifnotmove MOVE_SKY_ATTACK, BattleScript_HitFromAccCheck
	setmoveeffect MOVE_EFFECT_FLINCH
	goto BattleScript_HitFromAccCheck

BattleScriptFirstChargingTurn: @ 81D756C
	attackcanceler
	printstring STRINGID_EMPTYSTRING3
	ppreduce
	attackanimation
	waitanimation
	orword gHitMarker, HITMARKER_CHARGING
	setbyte cEFFECT_CHOOSER, 76
	seteffectprimary
	copybyte cMULTISTRING_CHOOSER, sTWOTURN_STRINGID
	printfromtable gFirstTurnOfTwoStringIds
	waitmessage 64
	return

BattleScript_EffectSuperFang: @ 81D7596
	attackcanceler
	accuracycheck BattleScript_PrintMoveMissed, ACC_CURR_MOVE
	attackstring
	ppreduce
	typecalc
	bicbyte gMoveResultFlags, MOVE_RESULT_SUPER_EFFECTIVE | MOVE_RESULT_NOT_VERY_EFFECTIVE
	damagetohalftargethp
	goto BattleScript_HitFromAtkAnimation

BattleScript_EffectDragonRage: @ 81D75AD
	attackcanceler
	accuracycheck BattleScript_PrintMoveMissed, ACC_CURR_MOVE
	attackstring
	ppreduce
	typecalc
	bicbyte gMoveResultFlags, MOVE_RESULT_SUPER_EFFECTIVE | MOVE_RESULT_NOT_VERY_EFFECTIVE
	setword gBattleMoveDamage, 40
	adjustsetdamage
	goto BattleScript_HitFromAtkAnimation

BattleScript_EffectTrap: @ 81D75DC
	jumpifnotmove MOVE_WHIRLPOOL, BattleScript_DoWrapEffect
	jumpifnostatus3 BS_TARGET, STATUS3_UNDERWATER, BattleScript_DoWrapEffect
	orword gHitMarker, HITMARKER_IGNORE_UNDERWATER
	setbyte sDMG_MULTIPLIER, 2

BattleScript_DoWrapEffect: @ 81D7602
	setmoveeffect MOVE_EFFECT_WRAP
	goto BattleScript_EffectHit

BattleScript_EffectDoubleHit: @ 81D760D
	attackcanceler
	accuracycheck BattleScript_PrintMoveMissed, ACC_CURR_MOVE
	attackstring
	ppreduce
	setmultihitcounter 2
	initmultihitstring
	setbyte sMULTIHIT_EFFECT, 0
	goto BattleScript_MultiHitLoop

BattleScript_EffectRecoilIfMiss: @ 81D7625
	attackcanceler
	accuracycheck BattleScript_MoveMissedDoDamage, ACC_CURR_MOVE
	goto BattleScript_HitFromAtkString

BattleScript_MoveMissedDoDamage: @ 81D7632
	attackstring
	ppreduce
	pause 64
	resultmessage
	waitmessage 64
	jumpifbyte CMP_COMMON_BITS, gMoveResultFlags, MOVE_RESULT_DOESNT_AFFECT_FOE, BattleScript_MoveEnd
	printstring STRINGID_PKMNCRASHED
	waitmessage 64
	damagecalc
	typecalc
	adjustnormaldamage
	manipulatedamage 1
	bicbyte gMoveResultFlags, MOVE_RESULT_MISSED
	orword gHitMarker, HITMARKER_IGNORE_SUBSTITUTE
	healthbarupdate BS_ATTACKER
	datahpupdate BS_ATTACKER
	tryfaintmon BS_ATTACKER, FALSE, NULL
	orbyte gMoveResultFlags, MOVE_RESULT_MISSED
	goto BattleScript_MoveEnd

BattleScript_EffectMist: @ 81D7676
	attackcanceler
	attackstring
	ppreduce
	setmist
	attackanimation
	waitanimation
	printfromtable gMistUsedStringIds
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectFocusEnergy: @ 81D7689
	attackcanceler
	attackstring
	ppreduce
	jumpifstatus2 BS_ATTACKER, STATUS2_FOCUS_ENERGY, BattleScript_ButItFailed
	setfocusenergy
	attackanimation
	waitanimation
	printfromtable gFocusEnergyUsedStringIds
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectRecoil: @ 81D76A6
	setmoveeffect MOVE_EFFECT_RECOIL_25 | MOVE_EFFECT_AFFECTS_USER | MOVE_EFFECT_CERTAIN
	jumpifnotmove MOVE_STRUGGLE, BattleScript_EffectHit
	incrementgamestat 27
	goto BattleScript_EffectHit

BattleScript_EffectConfuse: @ 81D76BF
	attackcanceler
	attackstring
	ppreduce
	jumpifability BS_TARGET, ABILITY_OWN_TEMPO, BattleScript_OwnTempoPrevents
	jumpifstatus2 BS_TARGET, STATUS2_SUBSTITUTE, BattleScript_ButItFailed
	jumpifstatus2 BS_TARGET, STATUS2_CONFUSION, BattleScript_AlreadyConfused
	accuracycheck BattleScript_ButItFailed, ACC_CURR_MOVE
	jumpifsideaffecting BS_TARGET, SIDE_STATUS_SAFEGUARD, BattleScript_SafeguardProtected
	attackanimation
	waitanimation
	setmoveeffect MOVE_EFFECT_CONFUSION
	seteffectprimary
	resultmessage
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_AlreadyConfused: @ 81D76FE
	pause 32
	printstring STRINGID_PKMNALREADYCONFUSED
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectAttackUp2: @ 81D770C
	setstatchanger STAT_ATK, 2, FALSE
	goto BattleScript_EffectStatUp

BattleScript_EffectDefenseUp2: @ 81D7717
	setstatchanger STAT_DEF, 2, FALSE
	goto BattleScript_EffectStatUp

BattleScript_EffectSpeedUp2: @ 81D7722
	setstatchanger STAT_SPEED, 2, FALSE
	goto BattleScript_EffectStatUp

BattleScript_EffectSpecialAttackUp2: @ 81D772D
	setstatchanger STAT_SPATK, 2, FALSE
	goto BattleScript_EffectStatUp

BattleScript_EffectSpecialDefenseUp2: @ 81D7738
	setstatchanger STAT_SPDEF, 2, FALSE
	goto BattleScript_EffectStatUp

BattleScript_EffectTransform: @ 81D7743
	attackcanceler
	attackstring
	ppreduce
	transformdataexecution
	attackanimation
	waitanimation
	printfromtable gTransformUsedStringIds
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectAttackDown2: @ 81D7756
	setstatchanger STAT_ATK, 2, TRUE
	goto BattleScript_EffectStatDown

BattleScript_EffectDefenseDown2: @ 81D7761
	setstatchanger STAT_DEF, 2, TRUE
	goto BattleScript_EffectStatDown

BattleScript_EffectSpeedDown2: @ 81D776C
	setstatchanger STAT_SPEED, 2, TRUE
	goto BattleScript_EffectStatDown

BattleScript_EffectSpecialDefenseDown2: @ 81D7777
	setstatchanger STAT_SPDEF, 2, TRUE
	goto BattleScript_EffectStatDown

BattleScript_EffectReflect: @ 81D7782
	attackcanceler
	attackstring
	ppreduce
	setreflect

BattleScript_PrintReflectLightScreenSafeguardString: @ 81D7786
	attackanimation
	waitanimation
	printfromtable gReflectLightScreenSafeguardStringIds
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectPoison: @ 81D7795
	attackcanceler
	attackstring
	ppreduce
	jumpifability BS_TARGET, ABILITY_IMMUNITY, BattleScript_ImmunityProtected
	jumpifstatus2 BS_TARGET, STATUS2_SUBSTITUTE, BattleScript_ButItFailed
	jumpifstatus BS_TARGET, STATUS1_POISON, BattleScript_AlreadyPoisoned
	jumpifstatus BS_TARGET, STATUS1_TOXIC_POISON, BattleScript_AlreadyPoisoned
	jumpiftype BS_TARGET, TYPE_POISON, BattleScript_NotAffected
	jumpiftype BS_TARGET, TYPE_STEEL, BattleScript_NotAffected
	jumpifstatus BS_TARGET, STATUS1_SLEEP | STATUS1_POISON | STATUS1_BURN | STATUS1_FREEZE | STATUS1_PARALYSIS | STATUS1_TOXIC_POISON, BattleScript_ButItFailed
	accuracycheck BattleScript_ButItFailed, ACC_CURR_MOVE
	jumpifsideaffecting BS_TARGET, SIDE_STATUS_SAFEGUARD, BattleScript_SafeguardProtected
	attackanimation
	waitanimation
	setmoveeffect MOVE_EFFECT_POISON
	seteffectprimary
	resultmessage
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectParalyze: @ 81D77F6
	attackcanceler
	attackstring
	ppreduce
	jumpifability BS_TARGET, ABILITY_LIMBER, BattleScript_LimberProtected
	jumpifstatus2 BS_TARGET, STATUS2_SUBSTITUTE, BattleScript_ButItFailed
	typecalc
	jumpifmovehadnoeffect BattleScript_ButItFailed
	jumpifstatus BS_TARGET, STATUS1_PARALYSIS, BattleScript_AlreadyParalyzed
	jumpifstatus BS_TARGET, STATUS1_SLEEP | STATUS1_POISON | STATUS1_BURN | STATUS1_FREEZE | STATUS1_PARALYSIS | STATUS1_TOXIC_POISON, BattleScript_ButItFailed
	accuracycheck BattleScript_ButItFailed, ACC_CURR_MOVE
	jumpifsideaffecting BS_TARGET, SIDE_STATUS_SAFEGUARD, BattleScript_SafeguardProtected
	attackanimation
	waitanimation
	setmoveeffect MOVE_EFFECT_PARALYSIS
	seteffectprimary
	resultmessage
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_AlreadyParalyzed: @ 81D784B
	pause 32
	printstring STRINGID_PKMNISALREADYPARALYZED
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_LimberProtected: @ 81D7859
	copybyte gEffectBattler, gBattlerTarget
	setbyte cMULTISTRING_CHOOSER, 0
	call BattleScript_PRLZPrevention
	goto BattleScript_MoveEnd

BattleScript_EffectAttackDownHit: @ 81D7873
	setmoveeffect MOVE_EFFECT_ATK_MINUS_1
	goto BattleScript_EffectHit

BattleScript_EffectDefenseDownHit: @ 81D787E
	setmoveeffect MOVE_EFFECT_DEF_MINUS_1
	goto BattleScript_EffectHit

BattleScript_EffectSpeedDownHit: @ 81D7889
	setmoveeffect MOVE_EFFECT_SPD_MINUS_1
	goto BattleScript_EffectHit

BattleScript_EffectSpecialAttackDownHit: @ 81D7894
	setmoveeffect MOVE_EFFECT_SP_ATK_MINUS_1
	goto BattleScript_EffectHit

BattleScript_EffectSpecialDefenseDownHit: @ 81D789F
	setmoveeffect MOVE_EFFECT_SP_DEF_MINUS_1
	goto BattleScript_EffectHit

BattleScript_EffectAccuracyDownHit: @ 81D78AA
	setmoveeffect MOVE_EFFECT_ACC_MINUS_1
	goto BattleScript_EffectHit

BattleScript_EffectSkyAttack: @ 81D78B5
	jumpifstatus2 BS_ATTACKER, STATUS2_MULTIPLETURNS, BattleScript_TwoTurnMovesSecondTurn
	jumpifword CMP_COMMON_BITS, gHitMarker, HITMARKER_NO_ATTACKSTRING, BattleScript_TwoTurnMovesSecondTurn
	setbyte sTWOTURN_STRINGID, 3
	call BattleScriptFirstChargingTurn
	goto BattleScript_MoveEnd

BattleScript_EffectConfuseHit: @ 81D78DD
	setmoveeffect MOVE_EFFECT_CONFUSION
	goto BattleScript_EffectHit

BattleScript_EffectTwineedle: @ 81D78E8
	attackcanceler
	accuracycheck BattleScript_PrintMoveMissed, ACC_CURR_MOVE
	setbyte sMULTIHIT_EFFECT, 2
	attackstring
	ppreduce
	setmultihitcounter 2
	initmultihitstring
	goto BattleScript_MultiHitLoop

BattleScript_EffectSubstitute: @ 81D7900
	attackcanceler
	ppreduce
	attackstring
	waitstate
	jumpifstatus2 BS_ATTACKER, STATUS2_SUBSTITUTE, BattleScript_AlreadyHasSubstitute
	setsubstitute
	jumpifbyte CMP_NOT_EQUAL, cMULTISTRING_CHOOSER, 1, BattleScript_SubstituteAnim
	pause 32
	goto BattleScript_SubstituteString

BattleScript_SubstituteAnim: @ 81D7922
	attackanimation
	waitanimation
	healthbarupdate BS_ATTACKER
	datahpupdate BS_ATTACKER

BattleScript_SubstituteString: @ 81D7928
	printfromtable gSubstituteUsedStringIds
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_AlreadyHasSubstitute: @ 81D7935
	pause 32
	printstring STRINGID_PKMNHASSUBSTITUTE
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectRecharge: @ 81D7943
	attackcanceler
	accuracycheck BattleScript_PrintMoveMissed, ACC_CURR_MOVE
	setmoveeffect MOVE_EFFECT_RECHARGE | MOVE_EFFECT_AFFECTS_USER | MOVE_EFFECT_CERTAIN
	goto BattleScript_HitFromAtkString

BattleScript_MoveUsedMustRecharge:: @ 81D7956
	printstring STRINGID_PKMNMUSTRECHARGE
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectRage: @ 81D7961
	attackcanceler
	accuracycheck BattleScript_RageMiss, ACC_CURR_MOVE
	setmoveeffect MOVE_EFFECT_RAGE
	seteffectprimary
	setmoveeffect 0
	goto BattleScript_HitFromAtkString

BattleScript_RageMiss: @ 81D797B
	setmoveeffect MOVE_EFFECT_RAGE
	clearstatusfromeffect BS_ATTACKER
	goto BattleScript_PrintMoveMissed

BattleScript_EffectMimic: @ 81D7988
	attackcanceler
	attackstring
	ppreduce
	jumpifstatus2 BS_TARGET, STATUS2_SUBSTITUTE, BattleScript_ButItFailed
	accuracycheck BattleScript_ButItFailed, NO_ACC_CALC_CHECK_LOCK_ON
	mimicattackcopy BattleScript_ButItFailed
	attackanimation
	waitanimation
	printstring STRINGID_PKMNLEARNEDMOVE2
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectMetronome: @ 81D79AE
	attackcanceler
	attackstring
	pause 32
	attackanimation
	waitanimation
	setbyte sB_ANIM_TURN, 0
	setbyte sB_ANIM_TARGETS_HIT, 0
	metronome

BattleScript_EffectLeechSeed: @ 81D79C2
	attackcanceler
	attackstring
	pause 32
	ppreduce
	jumpifstatus2 BS_TARGET, STATUS2_SUBSTITUTE, BattleScript_ButItFailed
	accuracycheck BattleScript_DoLeechSeed, ACC_CURR_MOVE

BattleScript_DoLeechSeed: @ 81D79D9
	setseeded
	attackanimation
	waitanimation
	printfromtable gLeechSeedStringIds
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectSplash: @ 81D79E9
	attackcanceler
	attackstring
	ppreduce
	attackanimation
	waitanimation
	incrementgamestat 26
	printstring STRINGID_BUTNOTHINGHAPPENED
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectDisable: @ 81D79FB
	attackcanceler
	attackstring
	ppreduce
	accuracycheck BattleScript_ButItFailed, ACC_CURR_MOVE
	disablelastusedattack BattleScript_ButItFailed
	attackanimation
	waitanimation
	printstring STRINGID_PKMNMOVEWASDISABLED
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectLevelDamage: @ 81D7A17
	attackcanceler
	accuracycheck BattleScript_PrintMoveMissed, ACC_CURR_MOVE
	attackstring
	ppreduce
	typecalc
	bicbyte gMoveResultFlags, MOVE_RESULT_SUPER_EFFECTIVE | MOVE_RESULT_NOT_VERY_EFFECTIVE
	dmgtolevel
	adjustsetdamage
	goto BattleScript_HitFromAtkAnimation

BattleScript_EffectPsywave: @ 81D7A2F
	attackcanceler
	accuracycheck BattleScript_PrintMoveMissed, ACC_CURR_MOVE
	attackstring
	ppreduce
	typecalc
	bicbyte gMoveResultFlags, MOVE_RESULT_SUPER_EFFECTIVE | MOVE_RESULT_NOT_VERY_EFFECTIVE
	psywavedamageeffect
	adjustsetdamage
	goto BattleScript_HitFromAtkAnimation

BattleScript_EffectCounter: @ 81D7A47
	attackcanceler
	counterdamagecalculator BattleScript_FailedFromAtkString
	accuracycheck BattleScript_PrintMoveMissed, ACC_CURR_MOVE
	attackstring
	ppreduce
	typecalc2
	adjustsetdamage
	goto BattleScript_HitFromAtkAnimation

BattleScript_EffectEncore: @ 81D7A5D
	attackcanceler
	accuracycheck BattleScript_PrintMoveMissed, ACC_CURR_MOVE
	attackstring
	ppreduce
	trysetencore BattleScript_ButItFailed
	attackanimation
	waitanimation
	printstring STRINGID_PKMNGOTENCORE
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectPainSplit: @ 81D7A79
	attackcanceler
	attackstring
	ppreduce
	accuracycheck BattleScript_ButItFailed, NO_ACC_CALC_CHECK_LOCK_ON
	painsplitdmgcalc BattleScript_ButItFailed
	attackanimation
	waitanimation
	orword gHitMarker, HITMARKER_IGNORE_SUBSTITUTE
	healthbarupdate BS_ATTACKER
	datahpupdate BS_ATTACKER
	copyword gBattleMoveDamage, sPAINSPLIT_HP
	healthbarupdate BS_TARGET
	datahpupdate BS_TARGET
	printstring STRINGID_SHAREDPAIN
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectSnore: @ 81D7AB0
	attackcanceler
	jumpifstatus BS_ATTACKER, STATUS1_SLEEP, BattleScript_SnoreIsAsleep
	attackstring
	ppreduce
	goto BattleScript_ButItFailed

BattleScript_SnoreIsAsleep: @ 81D7AC2
	jumpifhalfword CMP_EQUAL, gChosenMove, MOVE_SLEEP_TALK, BattleScript_DoSnore
	printstring STRINGID_PKMNFASTASLEEP
	waitmessage 64
	statusanimation BS_ATTACKER

BattleScript_DoSnore: @ 81D7AD6
	attackstring
	ppreduce
	accuracycheck BattleScript_MoveMissed, ACC_CURR_MOVE
	setmoveeffect MOVE_EFFECT_FLINCH
	goto BattleScript_HitFromCritCalc

BattleScript_EffectConversion2: @ 81D7AEA
	attackcanceler
	attackstring
	ppreduce
	settypetorandomresistance BattleScript_ButItFailed
	attackanimation
	waitanimation
	printstring STRINGID_PKMNCHANGEDTYPE
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectLockOn: @ 81D7AFF
	attackcanceler
	attackstring
	ppreduce
	jumpifstatus2 BS_TARGET, STATUS2_SUBSTITUTE, BattleScript_ButItFailed
	accuracycheck BattleScript_ButItFailed, ACC_CURR_MOVE
	setalwayshitflag
	attackanimation
	waitanimation
	printstring STRINGID_PKMNTOOKAIM
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectSketch: @ 81D7B21
	attackcanceler
	attackstring
	ppreduce
	jumpifstatus2 BS_TARGET, STATUS2_SUBSTITUTE, BattleScript_ButItFailed
	copymovepermanently BattleScript_ButItFailed
	attackanimation
	waitanimation
	printstring STRINGID_PKMNSKETCHEDMOVE
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectSleepTalk: @ 81D7B40
	attackcanceler
	jumpifstatus BS_ATTACKER, STATUS1_SLEEP, BattleScript_SleepTalkIsAsleep
	attackstring
	ppreduce
	goto BattleScript_ButItFailed

BattleScript_SleepTalkIsAsleep: @ 81D7B52
	printstring STRINGID_PKMNFASTASLEEP
	waitmessage 64
	statusanimation BS_ATTACKER
	attackstring
	ppreduce
	orword gHitMarker, HITMARKER_NO_PPDEDUCT
	trychoosesleeptalkmove BattleScript_SleepTalkUsingMove
	pause 64
	goto BattleScript_ButItFailed

BattleScript_SleepTalkUsingMove: @ 81D7B72
	attackanimation
	waitanimation
	setbyte sB_ANIM_TURN, 0
	setbyte sB_ANIM_TARGETS_HIT, 0
	jumptocalledmove TRUE

BattleScript_EffectDestinyBond: @ 81D7B82
	attackcanceler
	attackstring
	ppreduce
	setdestinybond
	attackanimation
	waitanimation
	printstring STRINGID_PKMNTRYINGTOTAKEFOE
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectFlail: @ 81D7B93
	remaininghptopower
	goto BattleScript_EffectHit

BattleScript_EffectSpite: @ 81D7B99
	attackcanceler
	attackstring
	ppreduce
	accuracycheck BattleScript_ButItFailed, ACC_CURR_MOVE
	tryspiteppreduce BattleScript_ButItFailed
	attackanimation
	waitanimation
	printstring STRINGID_PKMNREDUCEDPP
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectHealBell: @ 81D7BB5
	attackcanceler
	attackstring
	ppreduce
	healpartystatus
	waitstate
	attackanimation
	waitanimation
	printfromtable gPartyStatusHealStringIds
	waitmessage 64
	jumpifnotmove MOVE_HEAL_BELL, BattleScript_PartyHealEnd
	jumpifbyte CMP_NO_COMMON_BITS, cMULTISTRING_CHOOSER, 1, BattleScript_CheckHealBellMon2Unaffected
	printstring STRINGID_PKMNSXBLOCKSY
	waitmessage 64

BattleScript_CheckHealBellMon2Unaffected: @ 81D7BE1
	jumpifbyte CMP_NO_COMMON_BITS, cMULTISTRING_CHOOSER, 2, BattleScript_PartyHealEnd
	printstring STRINGID_PKMNSXBLOCKSY2
	waitmessage 64

BattleScript_PartyHealEnd: @ 81D7BF2
	updatestatusicon BS_ATTACKER_WITH_PARTNER
	waitstate
	goto BattleScript_MoveEnd

BattleScript_EffectTripleKick: @ 81D7BFA
	attackcanceler
	attackstring
	ppreduce
	sethword sTRIPLE_KICK_POWER, 0
	initmultihitstring
	setmultihit 3

BattleScript_TripleKickLoop: @ 81D7C0C
	jumpifhasnohp BS_ATTACKER, BattleScript_TripleKickEnd
	jumpifhasnohp BS_TARGET, BattleScript_TripleKickNoMoreHits
	jumpifhalfword CMP_EQUAL, gChosenMove, MOVE_SLEEP_TALK, BattleScript_DoTripleKickAttack
	jumpifstatus BS_ATTACKER, STATUS1_SLEEP, BattleScript_TripleKickNoMoreHits

BattleScript_DoTripleKickAttack: @ 81D7C2E
	movevaluescleanup
	accuracycheck BattleScript_TripleKickNoMoreHits, ACC_CURR_MOVE
	addbyte sTRIPLE_KICK_POWER, 10
	addbyte sMULTIHIT_STRING + 4, 1
	copyhword gDynamicBasePower, sTRIPLE_KICK_POWER
	critcalc
	damagecalc
	typecalc
	adjustnormaldamage
	jumpifmovehadnoeffect BattleScript_TripleKickNoMoreHits
	attackanimation
	waitanimation
	effectivenesssound
	hitanimation BS_TARGET
	waitstate
	healthbarupdate BS_TARGET
	datahpupdate BS_TARGET
	critmessage
	waitmessage 64
	printstring STRINGID_EMPTYSTRING3
	waitmessage 1
	setbyte sMOVEEND_STATE, 0
	moveend 2, 16
	jumpifbyte CMP_COMMON_BITS, gMoveResultFlags, MOVE_RESULT_FOE_ENDURED, BattleScript_TripleKickPrintStrings
	decrementmultihit BattleScript_TripleKickLoop
	goto BattleScript_TripleKickPrintStrings

BattleScript_TripleKickNoMoreHits: @ 81D7C8D
	pause 32

BattleScript_TripleKickPrintStrings: @ 81D7C90
	resultmessage
	waitmessage 64
	jumpifbyte CMP_EQUAL, sMULTIHIT_STRING + 4, 0, BattleScript_TripleKickEnd
	copyarray gBattleTextBuff1,sMULTIHIT_STRING, 6
	printstring STRINGID_HITXTIMES
	waitmessage 64

BattleScript_TripleKickEnd: @ 81D7CAF
	seteffectwithchance
	tryfaintmon BS_TARGET, FALSE, NULL
	setbyte sMOVEEND_STATE, 14
	moveend 0, 0
	end

BattleScript_EffectThief: @ 81D7CC1
	setmoveeffect MOVE_EFFECT_STEAL_ITEM
	goto BattleScript_EffectHit

BattleScript_EffectMeanLook: @ 81D7CCC
	attackcanceler
	attackstring
	ppreduce
	accuracycheck BattleScript_ButItFailed, NO_ACC_CALC
	jumpifstatus2 BS_TARGET, STATUS2_ESCAPE_PREVENTION, BattleScript_ButItFailed
	attackanimation
	waitanimation
	setmoveeffect MOVE_EFFECT_PREVENT_ESCAPE
	seteffectprimary
	printstring STRINGID_TARGETCANTESCAPENOW
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectNightmare: @ 81D7CF4
	attackcanceler
	attackstring
	ppreduce
	jumpifstatus2 BS_TARGET, STATUS2_SUBSTITUTE, BattleScript_ButItFailed
	jumpifstatus2 BS_TARGET, STATUS2_NIGHTMARE, BattleScript_ButItFailed
	jumpifstatus BS_TARGET, STATUS1_SLEEP, BattleScript_NightmareWorked
	goto BattleScript_ButItFailed

BattleScript_NightmareWorked: @ 81D7D1A
	attackanimation
	waitanimation
	setmoveeffect MOVE_EFFECT_NIGHTMARE
	seteffectprimary
	printstring STRINGID_PKMNFELLINTONIGHTMARE
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectMinimize: @ 81D7D2E
	attackcanceler
	setminimize
	setstatchanger STAT_EVASION, 1, FALSE
	goto BattleScript_EffectStatUpAfterAtkCanceler

BattleScript_EffectCurse: @ 81D7D3B
	jumpiftype2 BS_ATTACKER, TYPE_GHOST, BattleScript_GhostCurse
	attackcanceler
	attackstring
	ppreduce
	jumpifstat BS_ATTACKER, CMP_GREATER_THAN, STAT_SPEED, 0, BattleScript_CurseTrySpeed
	jumpifstat BS_ATTACKER, CMP_NOT_EQUAL, STAT_ATK, 12, BattleScript_CurseTrySpeed
	jumpifstat BS_ATTACKER, CMP_EQUAL, STAT_DEF, 12, BattleScript_ButItFailed

BattleScript_CurseTrySpeed: @ 81D7D60
	copybyte gBattlerTarget, gBattlerAttacker
	setbyte sB_ANIM_TURN, 1
	attackanimation
	waitanimation
	setstatchanger STAT_SPEED, 1, TRUE
	statbuffchange MOVE_EFFECT_AFFECTS_USER | 0x1, BattleScript_CurseTryAttack
	printfromtable gStatDownStringIds
	waitmessage 64

BattleScript_CurseTryAttack: @ 81D7D86
	setstatchanger STAT_ATK, 1, FALSE
	statbuffchange MOVE_EFFECT_AFFECTS_USER | 0x1, BattleScript_CurseTryDefense
	printfromtable gStatUpStringIds
	waitmessage 64

BattleScript_CurseTryDefense: @ 81D7D9A
	setstatchanger STAT_DEF, 1, FALSE
	statbuffchange MOVE_EFFECT_AFFECTS_USER | 0x1, BattleScript_CurseEnd
	printfromtable gStatUpStringIds
	waitmessage 64

BattleScript_CurseEnd: @ 81D7DAE
	goto BattleScript_MoveEnd

BattleScript_GhostCurse: @ 81D7DB3
	jumpifbytenotequal gBattlerAttacker, gBattlerTarget, BattleScript_DoGhostCurse
	getmovetarget BS_ATTACKER

BattleScript_DoGhostCurse: @ 81D7DC4
	attackcanceler
	attackstring
	ppreduce
	jumpifstatus2 BS_TARGET, STATUS2_SUBSTITUTE, BattleScript_ButItFailed
	cursetarget BattleScript_ButItFailed
	orword gHitMarker, HITMARKER_IGNORE_SUBSTITUTE
	setbyte sB_ANIM_TURN, 0
	attackanimation
	waitanimation
	healthbarupdate BS_ATTACKER
	datahpupdate BS_ATTACKER
	printstring STRINGID_PKMNLAIDCURSE
	waitmessage 64
	tryfaintmon BS_ATTACKER, FALSE, NULL
	goto BattleScript_MoveEnd

BattleScript_EffectEndure: @ 81D7DFD
BattleScript_EffectProtect: @ 81D7DFD
	attackcanceler
	attackstring
	ppreduce
	setprotectlike
	attackanimation
	waitanimation
	printfromtable gProtectLikeUsedStringIds
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectSpikes: @ 81D7E10
	attackcanceler
	trysetspikes BattleScript_FailedFromAtkString
	attackstring
	ppreduce
	attackanimation
	waitanimation
	printstring STRINGID_SPIKESSCATTERED
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectForesight: @ 81D7E25
	attackcanceler
	accuracycheck BattleScript_PrintMoveMissed, ACC_CURR_MOVE
	attackstring
	ppreduce
	setforesight
	attackanimation
	waitanimation
	printstring STRINGID_PKMNIDENTIFIED
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectPerishSong: @ 81D7E3D
	attackcanceler
	attackstring
	ppreduce
	trysetperishsong BattleScript_ButItFailed
	attackanimation
	waitanimation
	printstring STRINGID_FAINTINTHREE
	waitmessage 64
	setbyte sBATTLER, 0

BattleScript_PerishSongLoop: @ 81D7E53
	jumpifability BS_SCRIPTING, ABILITY_SOUNDPROOF, BattleScript_PerishSongNotAffected

BattleScript_PerishSongLoopIncrement: @ 81D7E5A
	addbyte sBATTLER, 1
	jumpifbytenotequal sBATTLER, gBattlersCount, BattleScript_PerishSongLoop
	goto BattleScript_MoveEnd

BattleScript_PerishSongNotAffected: @ 81D7E73
	printstring STRINGID_PKMNSXBLOCKSY2
	waitmessage 64
	goto BattleScript_PerishSongLoopIncrement

BattleScript_EffectSandstorm: @ 81D7E7E
	attackcanceler
	attackstring
	ppreduce
	setsandstorm
	goto BattleScript_MoveWeatherChange

BattleScript_EffectRollout: @ 81D7E87
	attackcanceler
	attackstring
	jumpifstatus2 BS_ATTACKER, STATUS2_MULTIPLETURNS, BattleScript_RolloutCheckAccuracy
	ppreduce

BattleScript_RolloutCheckAccuracy: @ 81D7E94
	accuracycheck BattleScript_RolloutHit, ACC_CURR_MOVE

BattleScript_RolloutHit: @ 81D7E9B
	typecalc2
	rolloutdamagecalculation
	goto BattleScript_HitFromCritCalc

BattleScript_EffectSwagger: @ 81D7EA2
	attackcanceler
	jumpifstatus2 BS_TARGET, STATUS2_SUBSTITUTE, BattleScript_MakeMoveMissed
	accuracycheck BattleScript_PrintMoveMissed, ACC_CURR_MOVE
	attackstring
	ppreduce
	jumpifconfusedandstatmaxed 1, BattleScript_ButItFailed
	attackanimation
	waitanimation
	setstatchanger STAT_ATK, 2, FALSE
	statbuffchange 1, BattleScript_SwaggerTryConfuse
	jumpifbyte CMP_EQUAL, cMULTISTRING_CHOOSER, 2, BattleScript_SwaggerTryConfuse
	setgraphicalstatchangevalues
	playanimation BS_TARGET, B_ANIM_STATS_CHANGE, sB_ANIM_ARG1
	printfromtable gStatUpStringIds
	waitmessage 64

BattleScript_SwaggerTryConfuse: @ 81D7EE5
	jumpifability BS_TARGET, ABILITY_OWN_TEMPO, BattleScript_OwnTempoPrevents
	jumpifsideaffecting BS_TARGET, SIDE_STATUS_SAFEGUARD, BattleScript_SafeguardProtected
	setmoveeffect MOVE_EFFECT_CONFUSION
	seteffectprimary
	goto BattleScript_MoveEnd

BattleScript_EffectFuryCutter: @ 81D7F00
	attackcanceler
	attackstring
	ppreduce
	accuracycheck BattleScript_FuryCutterHit, ACC_CURR_MOVE

BattleScript_FuryCutterHit: @ 81D7F0A
	furycuttercalc
	critcalc
	damagecalc
	typecalc
	jumpifmovehadnoeffect BattleScript_FuryCutterHit
	adjustnormaldamage
	goto BattleScript_HitFromAtkAnimation

BattleScript_EffectAttract: @ 81D7F1F
	attackcanceler
	attackstring
	ppreduce
	accuracycheck BattleScript_ButItFailed, ACC_CURR_MOVE
	tryinfatuating BattleScript_ButItFailed
	attackanimation
	waitanimation
	printstring STRINGID_PKMNFELLINLOVE
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectFrustration: @ 81D7F3B
BattleScript_EffectReturn: @ 81D7F3B
	attackcanceler
	accuracycheck BattleScript_PrintMoveMissed, ACC_CURR_MOVE
	friendshiptodamagecalculation
	goto BattleScript_HitFromAtkString

BattleScript_EffectPresent: @ 81D7F49
	attackcanceler
	accuracycheck BattleScript_PrintMoveMissed, ACC_CURR_MOVE
	attackstring
	ppreduce
	typecalc
	presentdamagecalculation

BattleScript_EffectSafeguard: @ 81D7F55
	attackcanceler
	attackstring
	ppreduce
	setsafeguard
	goto BattleScript_PrintReflectLightScreenSafeguardString

BattleScript_EffectThawHit: @ 81D7F5E
	setmoveeffect MOVE_EFFECT_BURN
	goto BattleScript_EffectHit

BattleScript_EffectMagnitude: @ 81D7F69
	attackcanceler
	attackstring
	ppreduce
	selectfirstvalidtarget
	magnitudedamagecalculation
	pause 32
	printstring STRINGID_MAGNITUDESTRENGTH
	waitmessage 64
	goto BattleScript_HitsAllWithUndergroundBonusLoop

BattleScript_EffectBatonPass: @ 81D7F7C
	attackcanceler
	attackstring
	ppreduce
	jumpifcantswitch SWITCH_IGNORE_ESCAPE_PREVENTION | BS_ATTACKER, BattleScript_ButItFailed
	attackanimation
	waitanimation
	openpartyscreen BS_ATTACKER, BattleScript_ButItFailed
	switchoutabilities BS_ATTACKER
	waitstate
	switchhandleorder BS_ATTACKER, 2
	returntoball BS_ATTACKER
	getswitchedmondata BS_ATTACKER
	switchindataupdate BS_ATTACKER
	hpthresholds BS_ATTACKER
	printstring 3
	switchinanim BS_ATTACKER, 1
	waitstate
	switchineffects BS_ATTACKER
	goto BattleScript_MoveEnd

BattleScript_EffectRapidSpin: @ 81D7FA9
	setmoveeffect MOVE_EFFECT_RAPIDSPIN | MOVE_EFFECT_CERTAIN
	goto BattleScript_EffectHit

BattleScript_EffectSonicboom: @ 81D7FB4
	attackcanceler
	accuracycheck BattleScript_PrintMoveMissed, ACC_CURR_MOVE
	attackstring
	ppreduce
	typecalc
	bicbyte gMoveResultFlags, MOVE_RESULT_SUPER_EFFECTIVE | MOVE_RESULT_NOT_VERY_EFFECTIVE
	setword gBattleMoveDamage, 20
	adjustsetdamage
	goto BattleScript_HitFromAtkAnimation

BattleScript_EffectMoonlight: @ 81D7FE3
BattleScript_EffectMorningSun: @ 81D7FE3
BattleScript_EffectSynthesis: @ 81D7FE3
	attackcanceler
	attackstring
	ppreduce
	recoverbasedonsunlight BattleScript_AlreadyAtFullHp
	goto BattleScript_PresentHealTarget

BattleScript_EffectHiddenPower: @ 81D7FF0
	hiddenpowercalc
	goto BattleScript_EffectHit

BattleScript_EffectRainDance: @ 81D7FF6
	attackcanceler
	attackstring
	ppreduce
	setrain

BattleScript_MoveWeatherChange: @ 81D7FFA
	attackanimation
	waitanimation
	printfromtable gMoveWeatherChangeStringIds
	waitmessage 64
	call BattleScript_WeatherFormChanges
	goto BattleScript_MoveEnd

BattleScript_EffectSunnyDay: @ 81D800E
	attackcanceler
	attackstring
	ppreduce
	setsunny
	goto BattleScript_MoveWeatherChange

BattleScript_EffectDefenseUpHit: @ 81D8017
	setbyte cEFFECT_CHOOSER, 80
	goto BattleScript_EffectHit

BattleScript_EffectAttackUpHit: @ 81D8022
	setbyte cEFFECT_CHOOSER, 79
	goto BattleScript_EffectHit

BattleScript_EffectAllStatsUpHit: @ 81D802D
	setbyte cEFFECT_CHOOSER, 98
	goto BattleScript_EffectHit

BattleScript_EffectBellyDrum: @ 81D8038
	attackcanceler
	attackstring
	ppreduce
	maxattackhalvehp BattleScript_ButItFailed
	orword gHitMarker, HITMARKER_IGNORE_SUBSTITUTE
	attackanimation
	waitanimation
	healthbarupdate BS_ATTACKER
	datahpupdate BS_ATTACKER
	printstring STRINGID_PKMNCUTHPMAXEDATTACK
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectPsychUp: @ 81D805A
	attackcanceler
	attackstring
	ppreduce
	copyfoestats BattleScript_ButItFailed
	attackanimation
	waitanimation
	printstring STRINGID_PKMNCOPIEDSTATCHANGES
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectMirrorCoat: @ 81D806F
	attackcanceler
	mirrorcoatdamagecalculator BattleScript_FailedFromAtkString
	accuracycheck BattleScript_PrintMoveMissed, ACC_CURR_MOVE
	attackstring
	ppreduce
	typecalc2
	adjustsetdamage
	goto BattleScript_HitFromAtkAnimation

BattleScript_EffectSkullBash: @ 81D8085
	jumpifstatus2 BS_ATTACKER, STATUS2_MULTIPLETURNS, BattleScript_TwoTurnMovesSecondTurn
	jumpifword CMP_COMMON_BITS, gHitMarker, HITMARKER_NO_ATTACKSTRING, BattleScript_TwoTurnMovesSecondTurn
	setbyte sTWOTURN_STRINGID, 2
	call BattleScriptFirstChargingTurn
	setstatchanger STAT_DEF, 1, FALSE
	statbuffchange MOVE_EFFECT_AFFECTS_USER | 0x1, BattleScript_SkullBashEnd
	jumpifbyte CMP_EQUAL, cMULTISTRING_CHOOSER, 2, BattleScript_SkullBashEnd
	setgraphicalstatchangevalues
	playanimation BS_ATTACKER, B_ANIM_STATS_CHANGE, sB_ANIM_ARG1
	printfromtable gStatUpStringIds
	waitmessage 64

BattleScript_SkullBashEnd: @ 81D80CF
	goto BattleScript_MoveEnd

BattleScript_EffectTwister: @ 81D80D4
	jumpifnostatus3 BS_TARGET, STATUS3_ON_AIR, BattleScript_FlinchEffect
	orword gHitMarker, HITMARKER_IGNORE_ON_AIR
	setbyte sDMG_MULTIPLIER, 2

BattleScript_FlinchEffect: @ 81D80EE
	setmoveeffect MOVE_EFFECT_FLINCH
	goto BattleScript_EffectHit

BattleScript_EffectEarthquake: @ 81D80F9
	attackcanceler
	attackstring
	ppreduce
	selectfirstvalidtarget

BattleScript_HitsAllWithUndergroundBonusLoop: @ 81D80FD
	movevaluescleanup
	jumpifnostatus3 BS_TARGET, STATUS3_UNDERGROUND, BattleScript_HitsAllNoUndergroundBonus
	orword gHitMarker, HITMARKER_IGNORE_UNDERGROUND
	setbyte sDMG_MULTIPLIER, 2
	goto BattleScript_DoHitAllWithUndergroundBonus

BattleScript_HitsAllNoUndergroundBonus: @ 81D811D
	bicword gHitMarker, HITMARKER_IGNORE_UNDERGROUND
	setbyte sDMG_MULTIPLIER, 1

BattleScript_DoHitAllWithUndergroundBonus: @ 81D812C
	accuracycheck BattleScript_HitAllWithUndergroundBonusMissed, ACC_CURR_MOVE
	critcalc
	damagecalc
	typecalc
	adjustnormaldamage
	attackanimation
	waitanimation
	effectivenesssound
	hitanimation BS_TARGET
	waitstate
	healthbarupdate BS_TARGET
	datahpupdate BS_TARGET
	critmessage
	waitmessage 64
	resultmessage
	waitmessage 64
	printstring STRINGID_EMPTYSTRING3
	waitmessage 1
	tryfaintmon BS_TARGET, FALSE, NULL
	setbyte sMOVEEND_STATE, 0
	moveend 2, 16
	jumpifnexttargetvalid BattleScript_HitsAllWithUndergroundBonusLoop
	end

BattleScript_HitAllWithUndergroundBonusMissed: @ 81D8165
	pause 32
	typecalc
	effectivenesssound
	resultmessage
	waitmessage 64
	setbyte sMOVEEND_STATE, 0
	moveend 2, 16
	jumpifnexttargetvalid BattleScript_HitsAllWithUndergroundBonusLoop
	end

BattleScript_EffectFutureSight: @ 81D817D
	attackcanceler
	attackstring
	ppreduce
	trysetfutureattack BattleScript_ButItFailed
	attackanimation
	waitanimation
	printfromtable gFutureMoveUsedStringIds
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectGust: @ 81D8194
	jumpifnostatus3 BS_TARGET, STATUS3_ON_AIR, BattleScript_EffectHit
	orword gHitMarker, HITMARKER_IGNORE_ON_AIR
	setbyte sDMG_MULTIPLIER, 2
	goto BattleScript_EffectHit

BattleScript_EffectStomp: @ 81D81B3
	jumpifnostatus3 BS_TARGET, STATUS3_MINIMIZED, BattleScript_FlinchEffect
	setbyte sDMG_MULTIPLIER, 2
	goto BattleScript_FlinchEffect

BattleScript_EffectSolarBeam: @ 81D81C9
	jumpifabilitypresent ABILITY_CLOUD_NINE, BattleScript_SolarBeamDecideTurn
	jumpifabilitypresent ABILITY_AIR_LOCK, BattleScript_SolarBeamDecideTurn
	jumpifhalfword CMP_COMMON_BITS, gBattleWeather, 96, BattleScript_SolarBeamOnFirstTurn

BattleScript_SolarBeamDecideTurn: @ 81D81E1
	jumpifstatus2 BS_ATTACKER, STATUS2_MULTIPLETURNS, BattleScript_TwoTurnMovesSecondTurn
	jumpifword CMP_COMMON_BITS, gHitMarker, HITMARKER_NO_ATTACKSTRING, BattleScript_TwoTurnMovesSecondTurn
	setbyte sTWOTURN_STRINGID, 1
	call BattleScriptFirstChargingTurn
	goto BattleScript_MoveEnd

BattleScript_SolarBeamOnFirstTurn: @ 81D8209
	orword gHitMarker, HITMARKER_CHARGING
	setbyte cEFFECT_CHOOSER, 76
	seteffectprimary
	ppreduce
	goto BattleScript_TwoTurnMovesSecondTurn

BattleScript_EffectThunder: @ 81D821F
	setmoveeffect MOVE_EFFECT_PARALYSIS
	orword gHitMarker, HITMARKER_IGNORE_ON_AIR
	goto BattleScript_EffectHit

BattleScript_EffectTeleport: @ 81D8233
	attackcanceler
	attackstring
	ppreduce
	jumpifbattletype BATTLE_TYPE_TRAINER, BattleScript_ButItFailed
	getifcantrunfrombattle BS_ATTACKER
	jumpifbyte CMP_EQUAL, gBattleCommunication, 1, BattleScript_ButItFailed
	jumpifbyte CMP_EQUAL, gBattleCommunication, 2, BattleScript_PrintAbilityMadeIneffective
	attackanimation
	waitanimation
	printstring STRINGID_PKMNFLEDFROMBATTLE
	waitmessage 64
	setbyte gBattleOutcome, 5
	goto BattleScript_MoveEnd

BattleScript_EffectBeatUp: @ 81D826E
	attackcanceler
	accuracycheck BattleScript_PrintMoveMissed, ACC_CURR_MOVE
	attackstring
	pause 32
	ppreduce
	setbyte gBattleCommunication, 0

BattleScript_BeatUpLoop: @ 81D8281
	movevaluescleanup
	trydobeatup BattleScript_BeatUpEnd, BattleScript_ButItFailed
	printstring STRINGID_PKMNATTACK
	critcalc
	jumpifbyte CMP_NOT_EQUAL, gCritMultiplier, 2, BattleScript_BeatUpAttack
	manipulatedamage 2

BattleScript_BeatUpAttack: @ 81D829C
	adjustnormaldamage
	attackanimation
	waitanimation
	effectivenesssound
	hitanimation BS_TARGET
	waitstate
	healthbarupdate BS_TARGET
	datahpupdate BS_TARGET
	critmessage
	waitmessage 64
	resultmessage
	waitmessage 64
	tryfaintmon BS_TARGET, FALSE, NULL
	setbyte sMOVEEND_STATE, 0
	moveend 2, 16
	goto BattleScript_BeatUpLoop

BattleScript_BeatUpEnd: @ 81D82C4
	end

BattleScript_EffectSemiInvulnerable: @ 81D82C5
	jumpifstatus2 BS_ATTACKER, STATUS2_MULTIPLETURNS, BattleScript_SecondTurnSemiInvulnerable
	jumpifword CMP_COMMON_BITS, gHitMarker, HITMARKER_NO_ATTACKSTRING, BattleScript_SecondTurnSemiInvulnerable
	jumpifmove MOVE_FLY, BattleScript_FirstTurnFly
	jumpifmove MOVE_DIVE, BattleScript_FirstTurnDive
	jumpifmove MOVE_BOUNCE, BattleScript_FirstTurnBounce
	setbyte sTWOTURN_STRINGID, 5
	goto BattleScript_FirstTurnSemiInvulnerable

BattleScript_FirstTurnBounce: @ 81D830C
	setbyte sTWOTURN_STRINGID, 7
	goto BattleScript_FirstTurnSemiInvulnerable

BattleScript_FirstTurnDive: @ 81D8317
	setbyte sTWOTURN_STRINGID, 6
	goto BattleScript_FirstTurnSemiInvulnerable

BattleScript_FirstTurnFly: @ 81D8322
	setbyte sTWOTURN_STRINGID, 4

BattleScript_FirstTurnSemiInvulnerable: @ 81D8328
	call BattleScriptFirstChargingTurn
	setsemiinvulnerablebit
	goto BattleScript_MoveEnd

BattleScript_SecondTurnSemiInvulnerable: @ 81D8333
	attackcanceler
	setmoveeffect MOVE_EFFECT_CHARGING
	setbyte sB_ANIM_TURN, 1
	clearstatusfromeffect BS_ATTACKER
	orword gHitMarker, HITMARKER_NO_PPDEDUCT
	jumpifnotmove MOVE_BOUNCE, BattleScript_SemiInvulnerableTryHit
	setmoveeffect MOVE_EFFECT_PARALYSIS

BattleScript_SemiInvulnerableTryHit: @ 81D835D
	accuracycheck BattleScript_SemiInvulnerableMiss, ACC_CURR_MOVE
	clearsemiinvulnerablebit
	goto BattleScript_HitFromAtkString

BattleScript_SemiInvulnerableMiss: @ 81D836A
	clearsemiinvulnerablebit
	goto BattleScript_PrintMoveMissed

BattleScript_EffectDefenseCurl: @ 81D8370
	attackcanceler
	attackstring
	ppreduce
	setdefensecurlbit
	setstatchanger STAT_DEF, 1, FALSE
	statbuffchange MOVE_EFFECT_AFFECTS_USER | 0x1, BattleScript_DefenseCurlDoStatUpAnim
	jumpifbyte CMP_EQUAL, cMULTISTRING_CHOOSER, 2, BattleScript_StatUpPrintString
	attackanimation
	waitanimation

BattleScript_DefenseCurlDoStatUpAnim: @ 81D838D
	goto BattleScript_StatUpDoAnim

BattleScript_EffectSoftboiled: @ 81D8392
	attackcanceler
	attackstring
	ppreduce
	tryhealhalfhealth BattleScript_AlreadyAtFullHp, 0

BattleScript_PresentHealTarget:: @ 81D839B
	attackanimation
	waitanimation
	orword gHitMarker, HITMARKER_IGNORE_SUBSTITUTE
	healthbarupdate BS_TARGET
	datahpupdate BS_TARGET
	printstring STRINGID_PKMNREGAINEDHEALTH
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_AlreadyAtFullHp:: @ 81D83B5
	pause 32
	printstring STRINGID_PKMNHPFULL
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectFakeOut: @ 81D83C3
	attackcanceler
	jumpifnotfirstturn BattleScript_FailedFromAtkString
	setbyte cEFFECT_CHOOSER, 136
	goto BattleScript_EffectHit

BattleScript_FailedFromAtkString: @ 81D83D4
	attackstring

BattleScript_FailedFromPPReduce: @ 81D83D5
	ppreduce

BattleScript_ButItFailed:: @ 81D83D6
	pause 32
	orbyte gMoveResultFlags, MOVE_RESULT_FAILED
	resultmessage
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_NotAffected: @ 81D83E8
	pause 32
	orbyte gMoveResultFlags, MOVE_RESULT_DOESNT_AFFECT_FOE
	resultmessage
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectUproar: @ 81D83FA
	attackcanceler
	accuracycheck BattleScript_PrintMoveMissed, ACC_CURR_MOVE
	setbyte cEFFECT_CHOOSER, 74
	attackstring
	jumpifstatus2 BS_ATTACKER, STATUS2_MULTIPLETURNS, BattleScript_UproarHit
	ppreduce

BattleScript_UproarHit: @ 81D8414
	nop
	goto BattleScript_HitFromCritCalc

BattleScript_EffectStockpile: @ 81D841A
	attackcanceler
	attackstring
	ppreduce
	stockpile
	attackanimation
	waitanimation
	printfromtable gStockpileUsedStringIds
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectSpitUp: @ 81D842D
	attackcanceler
	jumpifbyte CMP_EQUAL, cMISS_TYPE, 1, BattleScript_SpitUpFailProtect
	attackstring
	ppreduce
	accuracycheck BattleScript_PrintMoveMissed, ACC_CURR_MOVE
	stockpiletobasedamage BattleScript_SpitUpFail
	typecalc
	adjustsetdamage
	goto BattleScript_HitFromAtkAnimation

BattleScript_SpitUpFail: @ 81D844E
	pause 32
	printstring STRINGID_FAILEDTOSPITUP
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_SpitUpFailProtect: @ 81D845C
	attackstring
	ppreduce
	pause 64
	stockpiletobasedamage BattleScript_SpitUpFail
	resultmessage
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectSwallow: @ 81D846F
	attackcanceler
	attackstring
	ppreduce
	stockpiletohpheal BattleScript_SwallowFail
	goto BattleScript_PresentHealTarget

BattleScript_SwallowFail: @ 81D847C
	pause 32
	printfromtable gSwallowFailStringIds
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectHail: @ 81D848C
	attackcanceler
	attackstring
	ppreduce
	sethail
	goto BattleScript_MoveWeatherChange

BattleScript_EffectTorment: @ 81D8495
	attackcanceler
	attackstring
	ppreduce
	accuracycheck BattleScript_ButItFailed, ACC_CURR_MOVE
	settorment BattleScript_ButItFailed
	attackanimation
	waitanimation
	printstring STRINGID_PKMNSUBJECTEDTOTORMENT
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectFlatter: @ 81D84B1
	attackcanceler
	jumpifstatus2 BS_TARGET, STATUS2_SUBSTITUTE, BattleScript_MakeMoveMissed
	accuracycheck BattleScript_PrintMoveMissed, ACC_CURR_MOVE
	attackstring
	ppreduce
	jumpifconfusedandstatmaxed 4, BattleScript_ButItFailed
	attackanimation
	waitanimation
	setstatchanger STAT_SPATK, 1, FALSE
	statbuffchange 1, BattleScript_FlatterTryConfuse
	jumpifbyte CMP_EQUAL, cMULTISTRING_CHOOSER, 2, BattleScript_FlatterTryConfuse
	setgraphicalstatchangevalues
	playanimation BS_TARGET, B_ANIM_STATS_CHANGE, sB_ANIM_ARG1
	printfromtable gStatUpStringIds
	waitmessage 64

BattleScript_FlatterTryConfuse: @ 81D84F4
	jumpifability BS_TARGET, ABILITY_OWN_TEMPO, BattleScript_OwnTempoPrevents
	jumpifsideaffecting BS_TARGET, SIDE_STATUS_SAFEGUARD, BattleScript_SafeguardProtected
	setmoveeffect MOVE_EFFECT_CONFUSION
	seteffectprimary
	goto BattleScript_MoveEnd

BattleScript_EffectWillOWisp: @ 81D850F
	attackcanceler
	attackstring
	ppreduce
	jumpifstatus2 BS_TARGET, STATUS2_SUBSTITUTE, BattleScript_ButItFailed
	jumpifstatus BS_TARGET, STATUS1_BURN, BattleScript_AlreadyBurned
	jumpiftype BS_TARGET, TYPE_FIRE, BattleScript_NotAffected
	jumpifability BS_TARGET, ABILITY_WATER_VEIL, BattleScript_WaterVeilPrevents
	jumpifstatus BS_TARGET, STATUS1_SLEEP | STATUS1_POISON | STATUS1_BURN | STATUS1_FREEZE | STATUS1_PARALYSIS | STATUS1_TOXIC_POISON, BattleScript_ButItFailed
	accuracycheck BattleScript_ButItFailed, ACC_CURR_MOVE
	jumpifsideaffecting BS_TARGET, SIDE_STATUS_SAFEGUARD, BattleScript_SafeguardProtected
	attackanimation
	waitanimation
	setmoveeffect MOVE_EFFECT_BURN
	seteffectprimary
	goto BattleScript_MoveEnd

BattleScript_WaterVeilPrevents: @ 81D855B
	copybyte gEffectBattler, gBattlerTarget
	setbyte cMULTISTRING_CHOOSER, 0
	call BattleScript_BRNPrevention
	goto BattleScript_MoveEnd

BattleScript_AlreadyBurned: @ 81D8575
	pause 32
	printstring STRINGID_PKMNALREADYHASBURN
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectMemento: @ 81D8583
	attackcanceler
	jumpifbyte CMP_EQUAL, cMISS_TYPE, 1, BattleScript_MementoTargetProtect
	attackstring
	ppreduce
	trymemento BattleScript_ButItFailed
	setatkhptozero
	attackanimation
	waitanimation
	jumpifstatus2 BS_TARGET, STATUS2_SUBSTITUTE, BattleScript_EffectMementoPrintNoEffect
	setbyte sSTAT_ANIM_PLAYED, 0
	playstatchangeanimation BS_TARGET, 18, 7
	playstatchangeanimation BS_TARGET, 2, 3
	setstatchanger STAT_ATK, 2, TRUE
	statbuffchange 1, BattleScript_EffectMementoTrySpAtk
	jumpifbyte CMP_GREATER_THAN, cMULTISTRING_CHOOSER, 1, BattleScript_EffectMementoTrySpAtk
	printfromtable gStatDownStringIds
	waitmessage 64

BattleScript_EffectMementoTrySpAtk: @ 81D85D0
	playstatchangeanimation BS_TARGET, 16, 3
	setstatchanger STAT_SPATK, 2, TRUE
	statbuffchange 1, BattleScript_EffectMementoTryFaint
	jumpifbyte CMP_GREATER_THAN, cMULTISTRING_CHOOSER, 1, BattleScript_EffectMementoTryFaint
	printfromtable gStatDownStringIds
	waitmessage 64

BattleScript_EffectMementoTryFaint: @ 81D85F3
	tryfaintmon BS_ATTACKER, FALSE, NULL
	goto BattleScript_MoveEnd

BattleScript_EffectMementoPrintNoEffect: @ 81D85FF
	printstring STRINGID_BUTNOEFFECT
	waitmessage 64
	goto BattleScript_EffectMementoTryFaint

BattleScript_MementoTargetProtect: @ 81D860A
	attackstring
	ppreduce
	trymemento BattleScript_MementoTargetProtectEnd

BattleScript_MementoTargetProtectEnd: @ 81D8611
	setatkhptozero
	pause 64
	effectivenesssound
	resultmessage
	waitmessage 64
	tryfaintmon BS_ATTACKER, FALSE, NULL
	goto BattleScript_MoveEnd

BattleScript_EffectFacade: @ 81D8626
	jumpifstatus BS_ATTACKER, STATUS1_POISON | STATUS1_BURN | STATUS1_PARALYSIS | STATUS1_TOXIC_POISON, BattleScript_FacadeDoubleDmg
	goto BattleScript_EffectHit

BattleScript_FacadeDoubleDmg: @ 81D8635
	setbyte sDMG_MULTIPLIER, 2
	goto BattleScript_EffectHit

BattleScript_EffectFocusPunch: @ 81D8640
	attackcanceler
	jumpifnodamage BattleScript_HitFromAccCheck
	ppreduce
	printstring STRINGID_PKMNLOSTFOCUS
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectSmellingsalt: @ 81D8652
	jumpifstatus2 BS_TARGET, STATUS2_SUBSTITUTE, BattleScript_EffectHit
	setmoveeffect MOVE_EFFECT_REMOVE_PARALYSIS | MOVE_EFFECT_CERTAIN
	jumpifstatus BS_TARGET, STATUS1_PARALYSIS, BattleScript_SmellingsaltDoubleDmg
	goto BattleScript_EffectHit

BattleScript_SmellingsaltDoubleDmg: @ 81D8671
	setbyte sDMG_MULTIPLIER, 2
	goto BattleScript_EffectHit

BattleScript_EffectFollowMe: @ 81D867C
	attackcanceler
	attackstring
	ppreduce
	setforcedtarget
	attackanimation
	waitanimation
	printstring STRINGID_PKMNCENTERATTENTION
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectNaturePower: @ 81D868D
	attackcanceler
	attackstring
	pause 32
	callenvironmentattack
	printstring STRINGID_NATUREPOWERTURNEDINTO
	waitmessage 64
	return

BattleScript_EffectCharge: @ 81D869A
	attackcanceler
	attackstring
	ppreduce
	setcharge
	attackanimation
	waitanimation
	printstring STRINGID_PKMNCHARGINGPOWER
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectTaunt: @ 81D86AB
	attackcanceler
	attackstring
	ppreduce
	accuracycheck BattleScript_ButItFailed, ACC_CURR_MOVE
	settaunt BattleScript_ButItFailed
	attackanimation
	waitanimation
	printstring STRINGID_PKMNFELLFORTAUNT
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectHelpingHand: @ 81D86C7
	attackcanceler
	attackstring
	ppreduce
	trysethelpinghand BattleScript_ButItFailed
	attackanimation
	waitanimation
	printstring STRINGID_PKMNREADYTOHELP
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectTrick: @ 81D86DC
	attackcanceler
	attackstring
	ppreduce
	jumpifstatus2 BS_TARGET, STATUS2_SUBSTITUTE, BattleScript_ButItFailed
	accuracycheck BattleScript_ButItFailed, ACC_CURR_MOVE
	tryswapitems BattleScript_ButItFailed
	attackanimation
	waitanimation
	printstring STRINGID_PKMNSWITCHEDITEMS
	waitmessage 64
	printfromtable gItemSwapStringIds
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectRolePlay: @ 81D870A
	attackcanceler
	attackstring
	ppreduce
	accuracycheck BattleScript_ButItFailed, NO_ACC_CALC_CHECK_LOCK_ON
	trycopyability BattleScript_ButItFailed
	attackanimation
	waitanimation
	printstring STRINGID_PKMNCOPIEDFOE
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectWish: @ 81D8726
	attackcanceler
	attackstring
	ppreduce
	trywish 0, BattleScript_ButItFailed
	attackanimation
	waitanimation
	goto BattleScript_MoveEnd

BattleScript_EffectAssist: @ 81D8736
	attackcanceler
	attackstring
	assistattackselect BattleScript_FailedFromPPReduce
	attackanimation
	waitanimation
	setbyte sB_ANIM_TURN, 0
	setbyte sB_ANIM_TARGETS_HIT, 0
	jumptocalledmove TRUE

BattleScript_EffectIngrain: @ 81D874D
	attackcanceler
	attackstring
	ppreduce
	trysetroots BattleScript_ButItFailed
	attackanimation
	waitanimation
	printstring STRINGID_PKMNPLANTEDROOTS
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectSuperpower: @ 81D8762
	setmoveeffect MOVE_EFFECT_ATK_DEF_DOWN | MOVE_EFFECT_AFFECTS_USER | MOVE_EFFECT_CERTAIN
	goto BattleScript_EffectHit

BattleScript_EffectMagicCoat: @ 81D876D
	attackcanceler
	trysetmagiccoat BattleScript_FailedFromAtkString
	attackstring
	ppreduce
	attackanimation
	waitanimation
	printstring STRINGID_PKMNSHROUDEDITSELF
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectRecycle: @ 81D8782
	attackcanceler
	attackstring
	ppreduce
	tryrecycleitem BattleScript_ButItFailed
	attackanimation
	waitanimation
	printstring STRINGID_XFOUNDONEY
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectRevenge: @ 81D8797
	doubledamagedealtifdamaged
	goto BattleScript_EffectHit

BattleScript_EffectBrickBreak: @ 81D879D
	attackcanceler
	accuracycheck BattleScript_PrintMoveMissed, ACC_CURR_MOVE
	attackstring
	ppreduce
	removelightscreenreflect
	critcalc
	damagecalc
	typecalc
	adjustnormaldamage
	jumpifbyte CMP_EQUAL, sB_ANIM_TURN, 0, BattleScript_BrickBreakAnim
	bicbyte gMoveResultFlags, 9

BattleScript_BrickBreakAnim: @ 81D87BD
	attackanimation
	waitanimation
	jumpifbyte CMP_LESS_THAN, sB_ANIM_TURN, 2, BattleScript_BrickBreakDoHit
	printstring STRINGID_THEWALLSHATTERED
	waitmessage 64

BattleScript_BrickBreakDoHit: @ 81D87D0
	typecalc2
	effectivenesssound
	hitanimation BS_TARGET
	waitstate
	healthbarupdate BS_TARGET
	datahpupdate BS_TARGET
	critmessage
	waitmessage 64
	resultmessage
	waitmessage 64
	seteffectwithchance
	tryfaintmon BS_TARGET, FALSE, NULL
	goto BattleScript_MoveEnd

BattleScript_EffectYawn: @ 81D87EE
	attackcanceler
	attackstring
	ppreduce
	jumpifability BS_TARGET, ABILITY_VITAL_SPIRIT, BattleScript_PrintBankAbilityMadeIneffective
	jumpifability BS_TARGET, ABILITY_INSOMNIA, BattleScript_PrintBankAbilityMadeIneffective
	jumpifstatus2 BS_TARGET, STATUS2_SUBSTITUTE, BattleScript_ButItFailed
	jumpifsideaffecting BS_TARGET, SIDE_STATUS_SAFEGUARD, BattleScript_SafeguardProtected
	accuracycheck BattleScript_ButItFailed, NO_ACC_CALC_CHECK_LOCK_ON
	jumpifcantmakeasleep BattleScript_ButItFailed
	setyawn BattleScript_ButItFailed
	attackanimation
	waitanimation
	printstring STRINGID_PKMNWASMADEDROWSY
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_PrintBankAbilityMadeIneffective: @ 81D882F
	copybyte sBATTLER, sBATTLER_WITH_ABILITY

BattleScript_PrintAbilityMadeIneffective: @ 81D8839
	pause 32
	printstring STRINGID_PKMNSXMADEITINEFFECTIVE
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectKnockOff: @ 81D8847
	setmoveeffect MOVE_EFFECT_KNOCK_OFF
	goto BattleScript_EffectHit

BattleScript_EffectEndeavor: @ 81D8852
	attackcanceler
	attackstring
	ppreduce
	setdamagetohealthdifference BattleScript_ButItFailed
	copyword gHpDealt, gBattleMoveDamage
	accuracycheck BattleScript_MoveMissed, ACC_CURR_MOVE
	typecalc
	jumpifmovehadnoeffect BattleScript_HitFromAtkAnimation
	bicbyte gMoveResultFlags, MOVE_RESULT_SUPER_EFFECTIVE | MOVE_RESULT_NOT_VERY_EFFECTIVE
	copyword gBattleMoveDamage, gHpDealt
	adjustsetdamage
	goto BattleScript_HitFromAtkAnimation

BattleScript_EffectEruption: @ 81D888D
	scaledamagebyhealthratio
	goto BattleScript_EffectHit

BattleScript_EffectSkillSwap: @ 81D8893
	attackcanceler
	attackstring
	ppreduce
	accuracycheck BattleScript_ButItFailed, NO_ACC_CALC_CHECK_LOCK_ON
	tryswapabilities BattleScript_ButItFailed
	attackanimation
	waitanimation
	printstring STRINGID_PKMNSWAPPEDABILITIES
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectImprison: @ 81D88AF
	attackcanceler
	attackstring
	ppreduce
	tryimprison BattleScript_ButItFailed
	attackanimation
	waitanimation
	printstring STRINGID_PKMNSEALEDOPPONENTMOVE
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectRefresh: @ 81D88C4
	attackcanceler
	attackstring
	ppreduce
	cureifburnedparalyzedorpoisoned BattleScript_ButItFailed
	attackanimation
	waitanimation
	printstring STRINGID_PKMNSTATUSNORMAL
	waitmessage 64
	updatestatusicon BS_ATTACKER
	goto BattleScript_MoveEnd

BattleScript_EffectGrudge: @ 81D88DB
	attackcanceler
	attackstring
	ppreduce
	trysetgrudge BattleScript_ButItFailed
	attackanimation
	waitanimation
	printstring STRINGID_PKMNWANTSGRUDGE
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectSnatch: @ 81D88F0
	attackcanceler
	trysetsnatch BattleScript_FailedFromAtkString
	attackstring
	ppreduce
	attackanimation
	waitanimation
	pause 32
	printstring STRINGID_PKMNWAITSFORTARGET
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectLowKick: @ 81D8908
	attackcanceler
	attackstring
	ppreduce
	weightdamagecalculation
	accuracycheck BattleScript_MoveMissed, ACC_CURR_MOVE
	goto BattleScript_HitFromCritCalc

BattleScript_EffectSecretPower: @ 81D8918
	getsecretpowereffect
	goto BattleScript_EffectHit

BattleScript_EffectDoubleEdge: @ 81D891E
	setmoveeffect MOVE_EFFECT_RECOIL_33 | MOVE_EFFECT_AFFECTS_USER | MOVE_EFFECT_CERTAIN
	goto BattleScript_EffectHit

BattleScript_EffectTeeterDance: @ 81D8929
	attackcanceler
	attackstring
	ppreduce
	setbyte gBattlerTarget, 0

BattleScript_TeeterDanceLoop: @ 81D8932
	movevaluescleanup
	setmoveeffect MOVE_EFFECT_CONFUSION
	jumpifbyteequal gBattlerAttacker, gBattlerTarget, BattleScript_TeeterDanceDoMoveEndIncrement
	jumpifability BS_TARGET, ABILITY_OWN_TEMPO, BattleScript_TeeterDanceLoopIncrement
	jumpifstatus2 BS_TARGET, STATUS2_SUBSTITUTE, BattleScript_TeeterDanceSubstitutePrevents
	jumpifstatus2 BS_TARGET, STATUS2_CONFUSION, BattleScript_TeeterDanceAlreadyConfused
	accuracycheck BattleScript_TeeterDanceMissed, ACC_CURR_MOVE
	jumpifsideaffecting BS_TARGET, SIDE_STATUS_SAFEGUARD, BattleScript_TeeterDanceSafeguardProtected
	attackanimation
	waitanimation
	seteffectprimary
	resultmessage
	waitmessage 64

BattleScript_TeeterDanceDoMoveEndIncrement: @ 81D8978
	setbyte sMOVEEND_STATE, 0
	moveend 2, 16
	addbyte gBattlerTarget, 1
	jumpifbytenotequal gBattlerTarget, gBattlersCount, BattleScript_TeeterDanceLoop
	end

BattleScript_TeeterDanceLoopIncrement: @ 81D8996
	pause 32
	printstring STRINGID_PKMNPREVENTSCONFUSIONWITH
	waitmessage 64
	goto BattleScript_TeeterDanceDoMoveEndIncrement

BattleScript_TeeterDanceSafeguardProtected: @ 81D89A4
	pause 32
	printstring STRINGID_PKMNUSEDSAFEGUARD
	waitmessage 64
	goto BattleScript_TeeterDanceDoMoveEndIncrement

BattleScript_TeeterDanceSubstitutePrevents: @ 81D89B2
	pause 32
	printstring STRINGID_BUTITFAILED
	waitmessage 64
	goto BattleScript_TeeterDanceDoMoveEndIncrement

BattleScript_TeeterDanceAlreadyConfused: @ 81D89C0
	pause 32
	printstring STRINGID_PKMNALREADYCONFUSED
	waitmessage 64
	goto BattleScript_TeeterDanceDoMoveEndIncrement

BattleScript_TeeterDanceMissed: @ 81D89CE
	resultmessage
	waitmessage 64
	goto BattleScript_TeeterDanceDoMoveEndIncrement

BattleScript_EffectMudSport: @ 81D89D7
BattleScript_EffectWaterSport: @ 81D89D7
	attackcanceler
	attackstring
	ppreduce
	settypebasedhalvers BattleScript_ButItFailed
	attackanimation
	waitanimation
	printfromtable gSportsUsedStringIds
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectPoisonFang: @ 81D89EE
	setmoveeffect MOVE_EFFECT_TOXIC
	goto BattleScript_EffectHit

BattleScript_EffectWeatherBall: @ 81D89F9
	setweatherballtype
	goto BattleScript_EffectHit

BattleScript_EffectOverheat: @ 81D89FF
	setmoveeffect MOVE_EFFECT_SP_ATK_TWO_DOWN | MOVE_EFFECT_AFFECTS_USER | MOVE_EFFECT_CERTAIN
	goto BattleScript_EffectHit

BattleScript_EffectTickle: @ 81D8A0A
	attackcanceler
	attackstring
	ppreduce
	jumpifstat BS_TARGET, CMP_GREATER_THAN, STAT_ATK, 0, BattleScript_TickleDoMoveAnim
	jumpifstat BS_TARGET, CMP_EQUAL, STAT_DEF, 0, BattleScript_CantLowerMultipleStats

BattleScript_TickleDoMoveAnim: @ 81D8A1F
	accuracycheck BattleScript_ButItFailed, ACC_CURR_MOVE
	attackanimation
	waitanimation
	setbyte sSTAT_ANIM_PLAYED, 0
	playstatchangeanimation BS_TARGET, 6, 5
	playstatchangeanimation BS_TARGET, 2, 1
	setstatchanger STAT_ATK, 1, TRUE
	statbuffchange 1, BattleScript_TickleTryLowerDef
	jumpifbyte CMP_EQUAL, cMULTISTRING_CHOOSER, 2, BattleScript_TickleTryLowerDef
	printfromtable gStatDownStringIds
	waitmessage 64

BattleScript_TickleTryLowerDef: @ 81D8A55
	playstatchangeanimation BS_TARGET, 4, 1
	setstatchanger STAT_DEF, 1, TRUE
	statbuffchange 1, BattleScript_TickleEnd
	jumpifbyte CMP_EQUAL, cMULTISTRING_CHOOSER, 2, BattleScript_TickleEnd
	printfromtable gStatDownStringIds
	waitmessage 64

BattleScript_TickleEnd: @ 81D8A78
	goto BattleScript_MoveEnd

BattleScript_CantLowerMultipleStats: @ 81D8A7D
	pause 32
	orbyte gMoveResultFlags, MOVE_RESULT_FAILED
	printstring STRINGID_STATSWONTDECREASE2
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectCosmicPower: @ 81D8A91
	attackcanceler
	attackstring
	ppreduce
	jumpifstat BS_ATTACKER, CMP_LESS_THAN, STAT_DEF, 12, BattleScript_CosmicPowerDoMoveAnim
	jumpifstat BS_ATTACKER, CMP_EQUAL, STAT_SPDEF, 12, BattleScript_CantRaiseMultipleStats

BattleScript_CosmicPowerDoMoveAnim: @ 81D8AA6
	attackanimation
	waitanimation
	setbyte sSTAT_ANIM_PLAYED, 0
	playstatchangeanimation BS_ATTACKER, 36, 0
	setstatchanger STAT_DEF, 1, FALSE
	statbuffchange MOVE_EFFECT_AFFECTS_USER | 0x1, BattleScript_CosmicPowerTrySpDef
	jumpifbyte CMP_EQUAL, cMULTISTRING_CHOOSER, 2, BattleScript_CosmicPowerTrySpDef
	printfromtable gStatUpStringIds
	waitmessage 64

BattleScript_CosmicPowerTrySpDef: @ 81D8AD1
	setstatchanger STAT_SPDEF, 1, FALSE
	statbuffchange MOVE_EFFECT_AFFECTS_USER | 0x1, BattleScript_CosmicPowerEnd
	jumpifbyte CMP_EQUAL, cMULTISTRING_CHOOSER, 2, BattleScript_CosmicPowerEnd
	printfromtable gStatUpStringIds
	waitmessage 64

BattleScript_CosmicPowerEnd: @ 81D8AF0
	goto BattleScript_MoveEnd

BattleScript_EffectSkyUppercut: @ 81D8AF5
	orword gHitMarker, HITMARKER_IGNORE_ON_AIR
	goto BattleScript_EffectHit

BattleScript_EffectBulkUp: @ 81D8B03
	attackcanceler
	attackstring
	ppreduce
	jumpifstat BS_ATTACKER, CMP_LESS_THAN, STAT_ATK, 12, BattleScript_BulkUpDoMoveAnim
	jumpifstat BS_ATTACKER, CMP_EQUAL, STAT_DEF, 12, BattleScript_CantRaiseMultipleStats

BattleScript_BulkUpDoMoveAnim: @ 81D8B18
	attackanimation
	waitanimation
	setbyte sSTAT_ANIM_PLAYED, 0
	playstatchangeanimation BS_ATTACKER, 6, 0
	setstatchanger STAT_ATK, 1, FALSE
	statbuffchange MOVE_EFFECT_AFFECTS_USER | 0x1, BattleScript_BulkUpTryDef
	jumpifbyte CMP_EQUAL, cMULTISTRING_CHOOSER, 2, BattleScript_BulkUpTryDef
	printfromtable gStatUpStringIds
	waitmessage 64

BattleScript_BulkUpTryDef: @ 81D8B43
	setstatchanger STAT_DEF, 1, FALSE
	statbuffchange MOVE_EFFECT_AFFECTS_USER | 0x1, BattleScript_BulkUpEnd
	jumpifbyte CMP_EQUAL, cMULTISTRING_CHOOSER, 2, BattleScript_BulkUpEnd
	printfromtable gStatUpStringIds
	waitmessage 64

BattleScript_BulkUpEnd: @ 81D8B62
	goto BattleScript_MoveEnd

BattleScript_EffectCalmMind: @ 81D8B67
	attackcanceler
	attackstring
	ppreduce
	jumpifstat BS_ATTACKER, CMP_LESS_THAN, STAT_SPATK, 12, BattleScript_CalmMindDoMoveAnim
	jumpifstat BS_ATTACKER, CMP_EQUAL, STAT_SPDEF, 12, BattleScript_CantRaiseMultipleStats

BattleScript_CalmMindDoMoveAnim: @ 81D8B7C
	attackanimation
	waitanimation
	setbyte sSTAT_ANIM_PLAYED, 0
	playstatchangeanimation BS_ATTACKER, 48, 0
	setstatchanger STAT_SPATK, 1, FALSE
	statbuffchange MOVE_EFFECT_AFFECTS_USER | 0x1, BattleScript_CalmMindTrySpDef
	jumpifbyte CMP_EQUAL, cMULTISTRING_CHOOSER, 2, BattleScript_CalmMindTrySpDef
	printfromtable gStatUpStringIds
	waitmessage 64

BattleScript_CalmMindTrySpDef: @ 81D8BA7
	setstatchanger STAT_SPDEF, 1, FALSE
	statbuffchange MOVE_EFFECT_AFFECTS_USER | 0x1, BattleScript_CalmMindEnd
	jumpifbyte CMP_EQUAL, cMULTISTRING_CHOOSER, 2, BattleScript_CalmMindEnd
	printfromtable gStatUpStringIds
	waitmessage 64

BattleScript_CalmMindEnd: @ 81D8BC6
	goto BattleScript_MoveEnd

BattleScript_CantRaiseMultipleStats: @ 81D8BCB
	pause 32
	orbyte gMoveResultFlags, MOVE_RESULT_FAILED
	printstring STRINGID_STATSWONTINCREASE2
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_EffectDragonDance: @ 81D8BDF
	attackcanceler
	attackstring
	ppreduce
	jumpifstat BS_ATTACKER, CMP_LESS_THAN, STAT_ATK, 12, BattleScript_DragonDanceDoMoveAnim
	jumpifstat BS_ATTACKER, CMP_EQUAL, STAT_SPEED, 12, BattleScript_CantRaiseMultipleStats

BattleScript_DragonDanceDoMoveAnim: @ 81D8BF4
	attackanimation
	waitanimation
	setbyte sSTAT_ANIM_PLAYED, 0
	playstatchangeanimation BS_ATTACKER, 10, 0
	setstatchanger STAT_ATK, 1, FALSE
	statbuffchange MOVE_EFFECT_AFFECTS_USER | 0x1, BattleScript_DragonDanceTrySpeed
	jumpifbyte CMP_EQUAL, cMULTISTRING_CHOOSER, 2, BattleScript_DragonDanceTrySpeed
	printfromtable gStatUpStringIds
	waitmessage 64

BattleScript_DragonDanceTrySpeed: @ 81D8C1F
	setstatchanger STAT_SPEED, 1, FALSE
	statbuffchange MOVE_EFFECT_AFFECTS_USER | 0x1, BattleScript_DragonDanceEnd
	jumpifbyte CMP_EQUAL, cMULTISTRING_CHOOSER, 2, BattleScript_DragonDanceEnd
	printfromtable gStatUpStringIds
	waitmessage 64

BattleScript_DragonDanceEnd: @ 81D8C3E
	goto BattleScript_MoveEnd

BattleScript_EffectCamouflage: @ 81D8C43
	attackcanceler
	attackstring
	ppreduce
	settypetoenvironment BattleScript_ButItFailed
	attackanimation
	waitanimation
	printstring STRINGID_PKMNCHANGEDTYPE
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_FaintAttacker:: @ 81D8C58
	playfaintcry BS_ATTACKER
	pause 64
	dofaintanimation BS_ATTACKER
	cleareffectsonfaint BS_ATTACKER
	printstring STRINGID_ATTACKERFAINTED
	return

BattleScript_FaintTarget:: @ 81D8C65
	playfaintcry BS_TARGET
	pause 64
	dofaintanimation BS_TARGET
	cleareffectsonfaint BS_TARGET
	printstring STRINGID_TARGETFAINTED
	return

BattleScript_GiveExp:: @ 81D8C72
	setbyte sGIVEEXP_STATE, 0
	getexp BS_TARGET
	end2

BattleScript_HandleFaintedMon:: @ 81D8C7B
	checkteamslost BattleScript_LinkHandleFaintedMonMultiple
	jumpifbyte CMP_NOT_EQUAL, gBattleOutcome, 0, BattleScript_FaintedMonEnd
	jumpifbattletype BATTLE_TYPE_TRAINER, BattleScript_FaintedMonTryChoose
	jumpifword CMP_NO_COMMON_BITS, gHitMarker, HITMARKER_PLAYER_FAINTED, BattleScript_FaintedMonTryChoose
	printstring STRINGID_USENEXTPKMN
	setbyte gBattleCommunication, 0
	yesnobox
	jumpifbyte CMP_EQUAL, (gBattleCommunication + CURSOR_POSITION), 0, BattleScript_FaintedMonTryChoose
	jumpifplayerran BattleScript_FaintedMonEnd
	printstring STRINGID_CANTESCAPE2

BattleScript_FaintedMonTryChoose: @ 81D8CC2
	openpartyscreen 3, BattleScript_FaintedMonEnd
	switchhandleorder 3, 2
	jumpifnotbattletype BATTLE_TYPE_TRAINER, BattleScript_FaintedMonSendOutNew
	jumpifbattletype BATTLE_TYPE_LINK, BattleScript_FaintedMonSendOutNew
	jumpifbattletype BATTLE_TYPE_BATTLE_TOWER, BattleScript_FaintedMonSendOutNew
	jumpifbattletype BATTLE_TYPE_DOUBLE, BattleScript_FaintedMonSendOutNew
	jumpifword CMP_COMMON_BITS, gHitMarker, HITMARKER_PLAYER_FAINTED, BattleScript_FaintedMonSendOutNew
	jumpifbyte CMP_EQUAL, sBATTLE_STYLE, 1, BattleScript_FaintedMonSendOutNew
	jumpifcantswitch BS_ATTACKER, BattleScript_FaintedMonSendOutNew
	printstring STRINGID_ENEMYABOUTTOSWITCHPKMN
	setbyte gBattleCommunication, 0
	yesnobox
	jumpifbyte CMP_EQUAL, (gBattleCommunication + CURSOR_POSITION), 1, BattleScript_FaintedMonSendOutNew
	setatktoplayer0
	openpartyscreen 129, BattleScript_FaintedMonSendOutNew
	switchhandleorder BS_ATTACKER, 2
	jumpifbyte CMP_EQUAL, gBattleCommunication, 6, BattleScript_FaintedMonSendOutNew
	atknameinbuff1
	resetintimidatetracebits BS_ATTACKER
	hpthresholds2 BS_ATTACKER
	printstring 2
	switchoutabilities BS_ATTACKER
	waitstate
	returnatktoball
	waitstate
	drawpartystatussummary 1
	getswitchedmondata BS_ATTACKER
	switchindataupdate BS_ATTACKER
	hpthresholds BS_ATTACKER
	printstring 3
	hidepartystatussummary 1
	switchinanim BS_ATTACKER, 0
	waitstate
	switchineffects BS_ATTACKER
	resetsentmonsvalue

BattleScript_FaintedMonSendOutNew: @ 81D8D66
	drawpartystatussummary 3
	getswitchedmondata 3
	switchindataupdate 3
	hpthresholds 3
	printstring 3
	hidepartystatussummary 3
	switchinanim 3, 0
	waitstate
	switchineffects 3
	jumpifbattletype BATTLE_TYPE_DOUBLE, BattleScript_FaintedMonEnd
	cancelallactions

BattleScript_FaintedMonEnd: @ 81D8D86
	end2

BattleScript_LinkHandleFaintedMonMultiple: @ 81D8D87
	openpartyscreen 5, BattleScript_LinkHandleFaintedMonMultipleStart

BattleScript_LinkHandleFaintedMonMultipleStart: @ 81D8D8D
	switchhandleorder 3, 0
	openpartyscreen 6, BattleScript_LinkHandleFaintedMonMultipleEnd
	switchhandleorder 3, 0

BattleScript_LinkHandleFaintedMonLoop: @ 81D8D99
	switchhandleorder 3, 2
	drawpartystatussummary 3
	getswitchedmondata 3
	switchindataupdate 3
	hpthresholds 3
	printstring 3
	hidepartystatussummary 3
	switchinanim 3, 0
	waitstate
	switchineffects 5
	jumpifbytenotequal gBattlerFainted, gBattlersCount, BattleScript_LinkHandleFaintedMonLoop

BattleScript_LinkHandleFaintedMonMultipleEnd: @ 81D8DBD
	end2

BattleScript_LocalTrainerBattleWon:: @ 81D8DBE
	printstring STRINGID_PLAYERDEFEATEDTRAINER1
	trainerslidein 1
	waitstate
	printstring STRINGID_TRAINER1LOSETEXT
	getmoneyreward
	printstring STRINGID_PLAYERGOTMONEY
	waitmessage 64

BattleScript_PayDayMoneyAndPickUpItems:: @ 81D8DCE
	givepaydaymoney
	pickup
	end2

BattleScript_LocalBattleLost:: @ 81D8DD1
	jumpifbattletype BATTLE_TYPE_BATTLE_TOWER, BattleScript_LocalBattleLostEnd
	jumpifbattletype BATTLE_TYPE_EREADER_TRAINER, BattleScript_LocalBattleLostEnd
	jumpifhalfword CMP_EQUAL, gTrainerBattleOpponent_A, TRAINER_SECRET_BASE, BattleScript_LocalBattleLostEnd
	printstring STRINGID_PLAYERWHITEOUT
	waitmessage 64
	printstring STRINGID_PLAYERWHITEOUT2
	waitmessage 64

BattleScript_LocalBattleLostEnd: @ 81D8E01
	end2

BattleScript_LinkBattleWonOrLost:: @ 81D8E02
	printstring 5
	waitmessage 64
	endlinkbattle
	waitmessage 64
	end2

gUnknown_081D8E0D:: @ 81D8E0D
	printstring STRINGID_PLAYERDEFEATEDTRAINER1
	trainerslidein 1
	waitstate
	end2

BattleScript_SmokeBallEscape:: @ 81D8E14
	playanimation BS_ATTACKER, B_ANIM_SMOKEBALL_ESCAPE, NULL
	printstring STRINGID_PKMNFLEDUSINGITS
	waitmessage 64
	end2

BattleScript_RanAwayUsingMonAbility:: @ 81D8E22
	printstring STRINGID_PKMNFLEDUSING
	waitmessage 64
	end2

BattleScript_GotAwaySafely:: @ 81D8E29
	printstring STRINGID_GOTAWAYSAFELY
	waitmessage 64
	end2

BattleScript_WildMonFled:: @ 81D8E30
	printstring STRINGID_WILDPKMNFLED
	waitmessage 64
	end2

BattleScript_PrintCantRunFromTrainer:: @ 81D8E37
	printstring STRINGID_NORUNNINGFROMTRAINERS
	end2

BattleScript_PrintFailedToRunString:: @ 81D8E3B
	printfromtable gNoEscapeStringIds
	waitmessage 64
	end2

BattleScript_PrintCantEscapeFromBattle:: @ 81D8E44
	printselectionstringfromtable gNoEscapeStringIds
	endselectionscript

BattleScript_PrintFullBox:: @ 81D8E4A
	printselectionstring STRINGID_BOXISFULL
	endselectionscript

BattleScript_ActionSwitch:: @ 81D8E4E
	hpthresholds2 BS_ATTACKER
	printstring 2
	setbyte sDMG_MULTIPLIER, 2
	jumpifbyte CMP_COMMON_BITS, gBattleTypeFlags, BATTLE_TYPE_DOUBLE, BattleScript_PursuitSwitchDmgSetMultihit
	setmultihit 1
	goto BattleScript_PursuitSwitchDmgLoop

BattleScript_PursuitSwitchDmgSetMultihit: @ 81D8E6B
	setmultihit 2

BattleScript_PursuitSwitchDmgLoop: @ 81D8E6D
	jumpifnopursuitswitchdmg BattleScript_DoSwitchOut
	swapattackerwithtarget
	trysetdestinybondtohappen
	call BattleScript_PursuitDmgOnSwitchOut
	swapattackerwithtarget

BattleScript_DoSwitchOut: @ 81D8E7A
	decrementmultihit BattleScript_PursuitSwitchDmgLoop
	switchoutabilities BS_ATTACKER
	waitstate
	returnatktoball
	waitstate
	drawpartystatussummary 1
	switchhandleorder BS_ATTACKER, 1
	getswitchedmondata BS_ATTACKER
	switchindataupdate BS_ATTACKER
	hpthresholds BS_ATTACKER
	printstring 3
	hidepartystatussummary 1
	switchinanim BS_ATTACKER, 0
	waitstate
	switchineffects BS_ATTACKER
	setbyte sMOVEEND_STATE, 4
	moveend 1, 0
	setbyte sMOVEEND_STATE, 15
	moveend 1, 0
	end2

BattleScript_PursuitDmgOnSwitchOut: @ 81D8EAD
	pause 32
	attackstring
	ppreduce
	critcalc
	damagecalc
	typecalc
	adjustnormaldamage
	attackanimation
	waitanimation
	effectivenesssound
	hitanimation BS_TARGET
	waitstate
	healthbarupdate BS_TARGET
	datahpupdate BS_TARGET
	critmessage
	waitmessage 64
	resultmessage
	waitmessage 64
	tryfaintmon BS_TARGET, FALSE, NULL
	setbyte sMOVEEND_STATE, 3
	moveend 2, 6
	various BS_TARGET, 4
	jumpifbyte CMP_EQUAL, gBattleCommunication, 0, BattleScript_PursuitDmgOnSwitchOutRet
	setbyte sGIVEEXP_STATE, 0
	getexp BS_TARGET

BattleScript_PursuitDmgOnSwitchOutRet: @ 81D8EEE
	return

BattleScript_Pausex20:: @ 81D8EEF
	pause 32
	return

BattleScript_LevelUp:: @ 81D8EF3
	fanfare MUS_LEVEL_UP
	printstring STRINGID_PKMNGREWTOLV
	setbyte sLVLBOX_STATE, 0
	drawlvlupbox
	handlelearnnewmove BattleScript_LearnedNewMove, BattleScript_LearnMoveReturn, 1
	goto BattleScript_AskToLearnMove

BattleScript_TryLearnMoveLoop: @ 81D8F0F
	handlelearnnewmove BattleScript_LearnedNewMove, BattleScript_LearnMoveReturn, 0

BattleScript_AskToLearnMove: @ 81D8F19
	buffermovetolearn
	printstring STRINGID_TRYTOLEARNMOVE1
	printstring STRINGID_TRYTOLEARNMOVE2
	printstring STRINGID_TRYTOLEARNMOVE3
	waitstate
	setbyte sLEARNMOVE_STATE, 0
	yesnoboxlearnmove BattleScript_ForgotAndLearnedNewMove
	printstring STRINGID_STOPLEARNINGMOVE
	waitstate
	setbyte sLEARNMOVE_STATE, 0
	yesnoboxstoplearningmove BattleScript_AskToLearnMove
	printstring STRINGID_DIDNOTLEARNMOVE
	goto BattleScript_TryLearnMoveLoop

BattleScript_ForgotAndLearnedNewMove: @ 81D8F46
	printstring STRINGID_123POOF
	printstring STRINGID_PKMNFORGOTMOVE
	printstring STRINGID_ANDELLIPSIS

BattleScript_LearnedNewMove: @ 81D8F4F
	buffermovetolearn
	fanfare MUS_LEVEL_UP
	printstring STRINGID_PKMNLEARNEDMOVE
	waitmessage 64
	updatechoicemoveonlvlup BS_ATTACKER
	goto BattleScript_TryLearnMoveLoop

BattleScript_LearnMoveReturn: @ 81D8F61
	return

BattleScript_RainContinuesOrEnds:: @ 81D8F62
	printfromtable gRainContinuesStringIds
	waitmessage 64
	jumpifbyte CMP_EQUAL, cMULTISTRING_CHOOSER, 2, BattleScript_RainContinuesOrEndsEnd
	playanimation BS_ATTACKER, B_ANIM_RAIN_CONTINUES, NULL

BattleScript_RainContinuesOrEndsEnd: @ 81D8F7C
	end2

BattleScript_DamagingWeatherContinues:: @ 81D8F7D
	printfromtable gSandStormHailContinuesStringIds
	waitmessage 64
	playanimation_var BS_ATTACKER, sB_ANIM_ARG1, NULL
	setbyte gBattleCommunication, 0

BattleScript_DamagingWeatherLoop: @ 81D8F95
	copyarraywithindex gBattlerAttacker, gBattlerByTurnOrder, gBattleCommunication, 1
	weatherdamage
	jumpifword CMP_EQUAL, gBattleMoveDamage, 0x0, BattleScript_DamagingWeatherLoopIncrement
	printfromtable gSandStormHailDmgStringIds
	waitmessage 64
	orword gHitMarker, HITMARKER_IGNORE_BIDE | HITMARKER_IGNORE_SUBSTITUTE | HITMARKER_PASSIVE_HP_UPDATE | HITMARKER_GRUDGE
	effectivenesssound
	hitanimation BS_ATTACKER
	healthbarupdate BS_ATTACKER
	datahpupdate BS_ATTACKER
	tryfaintmon BS_ATTACKER, FALSE, NULL
	checkteamslost BattleScript_DamagingWeatherLoopIncrement

BattleScript_DamagingWeatherLoopIncrement: @ 81D8FD6
	jumpifbyte CMP_NOT_EQUAL, gBattleOutcome, 0, BattleScript_DamagingWeatherContinuesEnd
	addbyte gBattleCommunication, 1
	jumpifbytenotequal gBattleCommunication, gBattlersCount, BattleScript_DamagingWeatherLoop

BattleScript_DamagingWeatherContinuesEnd: @ 81D8FF5
	bicword gHitMarker, HITMARKER_IGNORE_BIDE | HITMARKER_IGNORE_SUBSTITUTE | HITMARKER_PASSIVE_HP_UPDATE | HITMARKER_GRUDGE
	end2

BattleScript_SandStormHailEnds:: @ 81D8FFF
	printfromtable gSandStormHailEndStringIds
	waitmessage 64
	end2

BattleScript_SunlightContinues:: @ 81D9008
	printstring STRINGID_SUNLIGHTSTRONG
	waitmessage 64
	playanimation BS_ATTACKER, B_ANIM_SUN_CONTINUES, NULL
	end2

BattleScript_SunlightFaded:: @ 81D9016
	printstring STRINGID_SUNLIGHTFADED
	waitmessage 64
	end2

BattleScript_OverworldWeatherStarts:: @ 81D901D
	printfromtable gWeatherContinuesStringIds
	waitmessage 64
	playanimation_var BS_ATTACKER, sB_ANIM_ARG1, NULL
	end3

BattleScript_SideStatusWoreOff:: @ 81D9030
	printstring STRINGID_PKMNSXWOREOFF
	waitmessage 64
	end2

BattleScript_SafeguardProtected: @ 81D9037
	pause 32
	printstring STRINGID_PKMNUSEDSAFEGUARD
	waitmessage 64
	end2

BattleScript_SafeguardEnds:: @ 81D9041
	pause 32
	printstring STRINGID_PKMNSAFEGUARDEXPIRED
	waitmessage 64
	end2

BattleScript_LeechSeedTurnDrain:: @ 81D904B
	playanimation BS_ATTACKER, B_ANIM_LEECH_SEED_DRAIN, sB_ANIM_ARG1
	orword gHitMarker, HITMARKER_IGNORE_SUBSTITUTE | HITMARKER_PASSIVE_HP_UPDATE
	healthbarupdate BS_ATTACKER
	datahpupdate BS_ATTACKER
	copyword gBattleMoveDamage, gHpDealt
	jumpifability BS_ATTACKER, ABILITY_LIQUID_OOZE, BattleScript_LeechSeedTurnPrintLiquidOoze
	manipulatedamage 0
	setbyte cMULTISTRING_CHOOSER, 3
	goto BattleScript_LeechSeedTurnPrintAndUpdateHp

BattleScript_LeechSeedTurnPrintLiquidOoze: @ 81D907D
	setbyte cMULTISTRING_CHOOSER, 4

BattleScript_LeechSeedTurnPrintAndUpdateHp: @ 81D9083
	orword gHitMarker, HITMARKER_IGNORE_SUBSTITUTE | HITMARKER_PASSIVE_HP_UPDATE
	healthbarupdate BS_TARGET
	datahpupdate BS_TARGET
	printfromtable gLeechSeedStringIds
	waitmessage 64
	tryfaintmon BS_ATTACKER, FALSE, NULL
	tryfaintmon BS_TARGET, FALSE, NULL
	end2

BattleScript_BideStoringEnergy:: @ 81D90A7
	printstring STRINGID_PKMNSTORINGENERGY
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_BideAttack:: @ 81D90B2
	printstring STRINGID_PKMNUNLEASHEDENERGY
	waitmessage 64
	accuracycheck BattleScript_MoveMissed, ACC_CURR_MOVE
	typecalc
	bicbyte gMoveResultFlags, MOVE_RESULT_SUPER_EFFECTIVE | MOVE_RESULT_NOT_VERY_EFFECTIVE
	copyword gBattleMoveDamage, sBIDE_DMG
	adjustsetdamage
	setbyte sB_ANIM_TURN, 1
	attackanimation
	waitanimation
	effectivenesssound
	hitanimation BS_TARGET
	waitstate
	healthbarupdate BS_TARGET
	datahpupdate BS_TARGET
	resultmessage
	waitmessage 64
	tryfaintmon BS_TARGET, FALSE, NULL
	goto BattleScript_MoveEnd

BattleScript_BideNoEnergyToAttack:: @ 81D90F1
	printstring STRINGID_PKMNUNLEASHEDENERGY
	waitmessage 64
	goto BattleScript_ButItFailed

BattleScript_SuccessForceOut:: @ 81D90FC
	attackanimation
	waitanimation
	switchoutabilities BS_TARGET
	returntoball BS_TARGET
	waitstate
	jumpifbattletype BATTLE_TYPE_TRAINER, BattleScript_TrainerBattleForceOut
	setbyte gBattleOutcome, 5
	finishaction

BattleScript_TrainerBattleForceOut: @ 81D9116
	getswitchedmondata BS_TARGET
	switchindataupdate BS_TARGET
	switchinanim BS_TARGET, 0
	waitstate
	printstring STRINGID_PKMNWASDRAGGEDOUT
	switchineffects BS_TARGET
	goto BattleScript_MoveEnd

BattleScript_MistProtected:: @ 81D9128
	pause 32
	printstring STRINGID_PKMNPROTECTEDBYMIST
	waitmessage 64
	return

BattleScript_RageIsBuilding:: @ 81D9132
	printstring STRINGID_PKMNRAGEBUILDING
	waitmessage 64
	return

BattleScript_MoveUsedIsDisabled:: @ 81D9139
	printstring STRINGID_PKMNMOVEISDISABLED
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_SelectingDisabledMove:: @ 81D9144
	printselectionstring STRINGID_PKMNMOVEISDISABLED
	endselectionscript

BattleScript_DisabledNoMore:: @ 81D9148
	printstring STRINGID_PKMNMOVEDISABLEDNOMORE
	waitmessage 64
	end2

BattleScript_EncoredNoMore:: @ 81D914F
	printstring STRINGID_PKMNENCOREENDED
	waitmessage 64
	end2

BattleScript_DestinyBondTakesLife:: @ 81D9156
	printstring STRINGID_PKMNTOOKFOE
	waitmessage 64
	orword gHitMarker, HITMARKER_IGNORE_SUBSTITUTE | HITMARKER_PASSIVE_HP_UPDATE
	healthbarupdate BS_ATTACKER
	datahpupdate BS_ATTACKER
	tryfaintmon BS_ATTACKER, FALSE, NULL
	return

BattleScript_SpikesOnAttacker:: @ 81D9171
	orword gHitMarker, HITMARKER_IGNORE_SUBSTITUTE | HITMARKER_PASSIVE_HP_UPDATE
	healthbarupdate BS_ATTACKER
	datahpupdate BS_ATTACKER
	call BattleScript_PrintHurtBySpikes
	tryfaintmon BS_ATTACKER, FALSE, NULL
	tryfaintmon BS_ATTACKER, TRUE, BattleScript_SpikesOnAttackerFainted
	return

BattleScript_SpikesOnAttackerFainted: @ 81D9192
	setbyte sGIVEEXP_STATE, 0
	getexp BS_ATTACKER
	goto BattleScript_HandleFaintedMon

BattleScript_SpikesOnTarget:: @ 81D919F
	orword gHitMarker, HITMARKER_IGNORE_SUBSTITUTE | HITMARKER_PASSIVE_HP_UPDATE
	healthbarupdate BS_TARGET
	datahpupdate BS_TARGET
	call BattleScript_PrintHurtBySpikes
	tryfaintmon BS_TARGET, FALSE, NULL
	tryfaintmon BS_TARGET, TRUE, BattleScript_SpikesOnTargetFainted
	return

BattleScript_SpikesOnTargetFainted: @ 81D91C0
	setbyte sGIVEEXP_STATE, 0
	getexp BS_TARGET
	goto BattleScript_HandleFaintedMon

BattleScript_SpikesOnFaintedBattler:: @ 81D91CD
	orword gHitMarker, HITMARKER_IGNORE_SUBSTITUTE | HITMARKER_PASSIVE_HP_UPDATE
	healthbarupdate 3
	datahpupdate 3
	call BattleScript_PrintHurtBySpikes
	tryfaintmon BS_FAINTED, FALSE, NULL
	tryfaintmon BS_FAINTED, TRUE, BattleScript_SpikesOnFaintedBattlerFainted
	return

BattleScript_SpikesOnFaintedBattlerFainted: @ 81D91EE
	setbyte sGIVEEXP_STATE, 0
	getexp 3
	goto BattleScript_HandleFaintedMon

BattleScript_PrintHurtBySpikes: @ 81D91FB
	printstring STRINGID_PKMNHURTBYSPIKES
	waitmessage 64
	return

BattleScript_PerishSongTakesLife:: @ 81D9202
	printstring STRINGID_PKMNPERISHCOUNTFELL
	waitmessage 64
	orword gHitMarker, HITMARKER_IGNORE_SUBSTITUTE | HITMARKER_PASSIVE_HP_UPDATE
	healthbarupdate BS_ATTACKER
	datahpupdate BS_ATTACKER
	tryfaintmon BS_ATTACKER, FALSE, NULL
	end2

BattleScript_PerishSongCountGoesDown:: @ 81D921D
	printstring STRINGID_PKMNPERISHCOUNTFELL
	waitmessage 64
	end2

BattleScript_AllStatsUp:: @ 81D9224
	jumpifstat BS_ATTACKER, CMP_LESS_THAN, STAT_ATK, 12, BattleScript_AllStatsUpAtk
	jumpifstat BS_ATTACKER, CMP_LESS_THAN, STAT_DEF, 12, BattleScript_AllStatsUpAtk
	jumpifstat BS_ATTACKER, CMP_LESS_THAN, STAT_SPEED, 12, BattleScript_AllStatsUpAtk
	jumpifstat BS_ATTACKER, CMP_LESS_THAN, STAT_SPATK, 12, BattleScript_AllStatsUpAtk
	jumpifstat BS_ATTACKER, CMP_EQUAL, STAT_SPDEF, 12, BattleScript_AllStatsUpRet

BattleScript_AllStatsUpAtk: @ 81D9251
	setbyte sSTAT_ANIM_PLAYED, 0
	playstatchangeanimation BS_ATTACKER, 62, 0
	setstatchanger STAT_ATK, 1, FALSE
	statbuffchange MOVE_EFFECT_AFFECTS_USER | 0x1, BattleScript_AllStatsUpDef
	printfromtable gStatUpStringIds
	waitmessage 64

BattleScript_AllStatsUpDef: @ 81D926F
	setstatchanger STAT_DEF, 1, FALSE
	statbuffchange MOVE_EFFECT_AFFECTS_USER | 0x1, BattleScript_AllStatsUpSpeed
	printfromtable gStatUpStringIds
	waitmessage 64

BattleScript_AllStatsUpSpeed: @ 81D9283
	setstatchanger STAT_SPEED, 1, FALSE
	statbuffchange MOVE_EFFECT_AFFECTS_USER | 0x1, BattleScript_AllStatsUpSpAtk
	printfromtable gStatUpStringIds
	waitmessage 64

BattleScript_AllStatsUpSpAtk: @ 81D9297
	setstatchanger STAT_SPATK, 1, FALSE
	statbuffchange MOVE_EFFECT_AFFECTS_USER | 0x1, BattleScript_AllStatsUpSpDef
	printfromtable gStatUpStringIds
	waitmessage 64

BattleScript_AllStatsUpSpDef: @ 81D92AB
	setstatchanger STAT_SPDEF, 1, FALSE
	statbuffchange MOVE_EFFECT_AFFECTS_USER | 0x1, BattleScript_AllStatsUpRet
	printfromtable gStatUpStringIds
	waitmessage 64

BattleScript_AllStatsUpRet: @ 81D92BF
	return

BattleScript_RapidSpinAway:: @ 81D92C0
	rapidspinfree
	return

BattleScript_WrapFree:: @ 81D92C2
	printstring STRINGID_PKMNGOTFREE
	waitmessage 64
	return

BattleScript_LeechSeedFree:: @ 81D92C9
	printstring STRINGID_PKMNSHEDLEECHSEED
	waitmessage 64
	return

BattleScript_SpikesFree:: @ 81D92D0
	printstring STRINGID_PKMNBLEWAWAYSPIKES
	waitmessage 64
	return

BattleScript_MonTookFutureAttack:: @ 81D92D7
	printstring STRINGID_PKMNTOOKATTACK
	waitmessage 64
	jumpifbyte CMP_NOT_EQUAL, cMULTISTRING_CHOOSER, 0, BattleScript_CheckDoomDesireMiss
	accuracycheck BattleScript_FutureAttackMiss, 248
	goto BattleScript_FutureAttackAnimate

BattleScript_CheckDoomDesireMiss: @ 81D92F4
	accuracycheck BattleScript_FutureAttackMiss, 353

BattleScript_FutureAttackAnimate: @ 81D92FB
	adjustnormaldamage2
	jumpifbyte CMP_NOT_EQUAL, cMULTISTRING_CHOOSER, 0, BattleScript_FutureHitAnimDoomDesire
	playanimation BS_ATTACKER, B_ANIM_FUTURE_SIGHT_HIT, NULL
	goto BattleScript_DoFutureAttackHit

BattleScript_FutureHitAnimDoomDesire: @ 81D9313
	playanimation BS_ATTACKER, B_ANIM_DOOM_DESIRE_HIT, NULL

BattleScript_DoFutureAttackHit: @ 81D931A
	effectivenesssound
	hitanimation BS_TARGET
	waitstate
	healthbarupdate BS_TARGET
	datahpupdate BS_TARGET
	resultmessage
	waitmessage 64
	tryfaintmon BS_TARGET, FALSE, NULL
	checkteamslost BattleScript_FutureAttackEnd

BattleScript_FutureAttackEnd: @ 81D9332
	setbyte sMOVEEND_STATE, 0
	moveend 1, 0
	setbyte sMOVEEND_STATE, 8
	moveend 2, 14
	setbyte gMoveResultFlags, 0
	end2

BattleScript_FutureAttackMiss: @ 81D934B
	pause 32
	setbyte gMoveResultFlags, 0
	orbyte gMoveResultFlags, MOVE_RESULT_FAILED
	resultmessage
	waitmessage 64
	setbyte gMoveResultFlags, 0
	end2

BattleScript_NoMovesLeft:: @ 81D9365
	printselectionstring STRINGID_PKMNHASNOMOVESLEFT
	endselectionscript

BattleScript_SelectingMoveWithNoPP:: @ 81D9369
	printselectionstring STRINGID_NOPPLEFT
	endselectionscript

BattleScript_NoPPForMove:: @ 81D936D
	attackstring
	pause 32
	printstring STRINGID_BUTNOPPLEFT
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_SelectingTormentedMove:: @ 81D937C
	printselectionstring STRINGID_PKMNCANTUSEMOVETORMENT
	endselectionscript

BattleScript_MoveUsedIsTormented::
	printstring STRINGID_PKMNCANTUSEMOVETORMENT
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_SelectingNotAllowedMoveTaunt:: @ 81D938B
	printselectionstring STRINGID_PKMNCANTUSEMOVETAUNT
	endselectionscript

BattleScript_MoveUsedIsTaunted:: @ 81D938F
	printstring STRINGID_PKMNCANTUSEMOVETAUNT
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_WishComesTrue:: @ 81D939A
	trywish 1, BattleScript_WishButFullHp
	playanimation BS_TARGET, B_ANIM_WISH_HEAL, NULL
	printstring STRINGID_PKMNWISHCAMETRUE
	waitmessage 64
	orword gHitMarker, HITMARKER_IGNORE_SUBSTITUTE
	healthbarupdate BS_TARGET
	datahpupdate BS_TARGET
	printstring STRINGID_PKMNREGAINEDHEALTH
	waitmessage 64
	end2

BattleScript_WishButFullHp: @ 81D93C1
	printstring STRINGID_PKMNWISHCAMETRUE
	waitmessage 64
	pause 32
	printstring STRINGID_PKMNHPFULL
	waitmessage 64
	end2

BattleScript_IngrainTurnHeal:: @ 81D93D1
	playanimation BS_ATTACKER, B_ANIM_INGRAIN_HEAL, NULL
	printstring STRINGID_PKMNABSORBEDNUTRIENTS
	waitmessage 64
	orword gHitMarker, HITMARKER_IGNORE_SUBSTITUTE
	healthbarupdate BS_ATTACKER
	datahpupdate BS_ATTACKER
	end2

BattleScript_PrintMonIsRooted: @ 81D93EC
	pause 32
	printstring STRINGID_PKMNANCHOREDITSELF
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_AtkDefDown:: @ 81D93FA
	setbyte sSTAT_ANIM_PLAYED, 0
	playstatchangeanimation BS_ATTACKER, 6, 13
	playstatchangeanimation BS_ATTACKER, 2, 9
	setstatchanger STAT_ATK, 1, TRUE
	statbuffchange MOVE_EFFECT_AFFECTS_USER | MOVE_EFFECT_CERTAIN | 0x1, BattleScript_AtkDefDown_TryDef
	jumpifbyte CMP_EQUAL, cMULTISTRING_CHOOSER, 2, BattleScript_AtkDefDown_TryDef
	printfromtable gStatDownStringIds
	waitmessage 64

BattleScript_AtkDefDown_TryDef: @ 81D9427
	playstatchangeanimation BS_ATTACKER, 4, 9
	setstatchanger STAT_DEF, 1, TRUE
	statbuffchange MOVE_EFFECT_AFFECTS_USER | MOVE_EFFECT_CERTAIN | 0x1, BattleScript_AtkDefDown_End
	jumpifbyte CMP_EQUAL, cMULTISTRING_CHOOSER, 2, BattleScript_AtkDefDown_End
	printfromtable gStatDownStringIds
	waitmessage 64

BattleScript_AtkDefDown_End: @ 81D944A
	return

BattleScript_KnockedOff:: @ 81D944B
	playanimation BS_TARGET, B_ANIM_ITEM_KNOCKOFF, NULL
	printstring STRINGID_PKMNKNOCKEDOFF
	waitmessage 64
	return

BattleScript_MoveUsedIsImprisoned:: @ 81D9459
	printstring STRINGID_PKMNCANTUSEMOVESEALED
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_SelectingImprisonedMove:: @ 81D9464
	printselectionstring STRINGID_PKMNCANTUSEMOVESEALED
	endselectionscript

BattleScript_GrudgeTakesPP:: @ 81D9468
	printstring STRINGID_PKMNLOSTPPGRUDGE
	waitmessage 64
	return

BattleScript_MagicCoatBounce:: @ 81D946F
	attackstring
	ppreduce
	pause 32
	printstring STRINGID_PKMNMOVEBOUNCED
	waitmessage 64
	orword gHitMarker, HITMARKER_ATTACKSTRING_PRINTED | HITMARKER_NO_PPDEDUCT | HITMARKER_ALLOW_NO_PP
	setmagiccoattarget BS_ATTACKER
	return

BattleScript_SnatchedMove:: @ 81D9487
	attackstring
	ppreduce
	snatchsetbattlers
	playanimation BS_TARGET, B_ANIM_SNATCH_MOVE, NULL
	printstring STRINGID_PKMNSNATCHEDMOVE
	waitmessage 64
	orword gHitMarker, HITMARKER_ATTACKSTRING_PRINTED | HITMARKER_NO_PPDEDUCT | HITMARKER_ALLOW_NO_PP
	swapattackerwithtarget
	return

BattleScript_EnduredMsg:: @ 81D94A2
	printstring STRINGID_PKMNENDUREDHIT
	waitmessage 64
	return

BattleScript_OneHitKOMsg:: @ 81D94A9
	printstring STRINGID_ONEHITKO
	waitmessage 64
	return

BattleScript_SAtkDown2:: @ 81D94B0
	setbyte sSTAT_ANIM_PLAYED, 0
	playstatchangeanimation BS_ATTACKER, 16, 11
	setstatchanger STAT_SPATK, 2, TRUE
	statbuffchange MOVE_EFFECT_AFFECTS_USER | MOVE_EFFECT_CERTAIN | 0x1, BattleScript_SAtkDown2End
	jumpifbyte CMP_EQUAL, cMULTISTRING_CHOOSER, 2, BattleScript_SAtkDown2End
	printfromtable gStatDownStringIds
	waitmessage 64

BattleScript_SAtkDown2End: @ 81D94D9
	return

BattleScript_FocusPunchSetUp:: @ 81D94DA
	printstring STRINGID_EMPTYSTRING3
	waitmessage 1
	playanimation BS_ATTACKER, B_ANIM_FOCUS_PUNCH_SETUP, NULL
	printstring STRINGID_PKMNTIGHTENINGFOCUS
	waitmessage 64
	end2

BattleScript_MoveUsedIsAsleep:: @ 81D94EE
	printstring STRINGID_PKMNFASTASLEEP
	waitmessage 64
	statusanimation BS_ATTACKER
	goto BattleScript_MoveEnd

BattleScript_MoveUsedWokeUp:: @ 81D94FB
	bicword gHitMarker, HITMARKER_WAKE_UP_CLEAR
	printfromtable gWokeUpStringIds
	waitmessage 64
	updatestatusicon BS_ATTACKER
	return

BattleScript_MonWokeUpInUproar:: @ 81D950F
	printstring STRINGID_PKMNWOKEUPINUPROAR
	waitmessage 64
	updatestatusicon BS_ATTACKER
	end2

BattleScript_PoisonTurnDmg:: @ 81D9518
	printstring STRINGID_PKMNHURTBYPOISON
	waitmessage 64

BattleScript_DoStatusTurnDmg: @ 81D951E
	statusanimation BS_ATTACKER

BattleScript_DoTurnDmg: @ 81D9520
	orword gHitMarker, HITMARKER_IGNORE_SUBSTITUTE | HITMARKER_PASSIVE_HP_UPDATE
	healthbarupdate BS_ATTACKER
	datahpupdate BS_ATTACKER
	tryfaintmon BS_ATTACKER, FALSE, NULL
	checkteamslost BattleScript_DoTurnDmgEnd

BattleScript_DoTurnDmgEnd: @ 81D9539
	end2

BattleScript_BurnTurnDmg:: @ 81D953A
	printstring STRINGID_PKMNHURTBYBURN
	waitmessage 64
	goto BattleScript_DoStatusTurnDmg

BattleScript_MoveUsedIsFrozen:: @ 81D9545
	printstring STRINGID_PKMNISFROZEN
	waitmessage 64
	statusanimation BS_ATTACKER
	goto BattleScript_MoveEnd

BattleScript_MoveUsedUnfroze:: @ 81D9552
	printfromtable gGotDefrostedStringIds
	waitmessage 64
	updatestatusicon BS_ATTACKER
	return

BattleScript_DefrostedViaFireMove:: @ 81D955D
	printstring STRINGID_PKMNWASDEFROSTED
	waitmessage 64
	updatestatusicon BS_TARGET
	return

BattleScript_MoveUsedIsParalyzed:: @ 81D9566
	printstring STRINGID_PKMNISPARALYZED
	waitmessage 64
	statusanimation BS_ATTACKER
	goto BattleScript_MoveEnd

BattleScript_MoveUsedFlinched:: @ 81D9573
	printstring STRINGID_PKMNFLINCHED
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_PrintUproarOverTurns:: @ 81D957E
	printfromtable gUproarOverTurnStringIds
	waitmessage 64
	end2

BattleScript_ThrashConfuses:: @ 81D9587
	chosenstatus2animation BS_ATTACKER, STATUS2_CONFUSION
	printstring STRINGID_PKMNFATIGUECONFUSION
	waitmessage 64
	end2

BattleScript_MoveUsedIsConfused:: @ 81D9595
	printstring STRINGID_PKMNISCONFUSED
	waitmessage 64
	status2animation BS_ATTACKER, STATUS2_CONFUSION
	jumpifbyte CMP_EQUAL, cMULTISTRING_CHOOSER, 0, BattleScript_MoveUsedIsConfusedRet

BattleScript_DoSelfConfusionDmg: @ 81D95AC
	cancelmultiturnmoves BS_ATTACKER
	adjustnormaldamage2
	printstring STRINGID_ITHURTCONFUSION
	waitmessage 64
	effectivenesssound
	hitanimation BS_ATTACKER
	waitstate
	orword gHitMarker, HITMARKER_IGNORE_SUBSTITUTE | HITMARKER_PASSIVE_HP_UPDATE
	healthbarupdate BS_ATTACKER
	datahpupdate BS_ATTACKER
	tryfaintmon BS_ATTACKER, FALSE, NULL
	goto BattleScript_MoveEnd

BattleScript_MoveUsedIsConfusedRet: @ 81D95D3
	return

BattleScript_MoveUsedIsConfusedNoMore:: @ 81D95D4
	printstring STRINGID_PKMNHEALEDCONFUSION
	waitmessage 64
	return

BattleScript_PrintPayDayMoneyString:: @ 81D95DB
	printstring STRINGID_PLAYERPICKEDUPMONEY
	waitmessage 64
	return

BattleScript_WrapTurnDmg:: @ 81D95E2
	playanimation BS_ATTACKER, B_ANIM_TURN_TRAP, sB_ANIM_ARG1
	printstring STRINGID_PKMNHURTBY
	waitmessage 64
	goto BattleScript_DoTurnDmg

BattleScript_WrapEnds:: @ 81D95F4
	printstring STRINGID_PKMNFREEDFROM
	waitmessage 64
	end2

BattleScript_MoveUsedIsInLove:: @ 81D95FB
	printstring STRINGID_PKMNINLOVE
	waitmessage 64
	status2animation BS_ATTACKER, STATUS2_INFATUATION
	return

BattleScript_MoveUsedIsInLoveCantAttack:: @ 81D9608
	printstring STRINGID_PKMNIMMOBILIZEDBYLOVE
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_NightmareTurnDmg:: @ 81D9613
	printstring STRINGID_PKMNLOCKEDINNIGHTMARE
	waitmessage 64
	status2animation BS_ATTACKER, STATUS2_NIGHTMARE
	goto BattleScript_DoTurnDmg

BattleScript_CurseTurnDmg:: @ 81D9624
	printstring STRINGID_PKMNAFFLICTEDBYCURSE
	waitmessage 64
	status2animation BS_ATTACKER, STATUS2_CURSED
	goto BattleScript_DoTurnDmg

BattleScript_TargetPRLZHeal:: @ 81D9635
	printstring STRINGID_PKMNHEALEDPARALYSIS
	waitmessage 64
	updatestatusicon BS_TARGET
	return

BattleScript_MoveEffectSleep:: @ 81D963E
	statusanimation BS_EFFECT_BATTLER
	printfromtable gFellAsleepStringIds
	waitmessage 64

BattleScript_UpdateEffectStatusIconRet: @ 81D9648
	updatestatusicon BS_EFFECT_BATTLER
	waitstate
	return

BattleScript_YawnMakesAsleep:: @ 81D964C
	statusanimation BS_EFFECT_BATTLER
	printstring STRINGID_PKMNFELLASLEEP
	waitmessage 64
	updatestatusicon BS_EFFECT_BATTLER
	waitstate
	makevisible 2
	end2

BattleScript_MoveEffectPoison:: @ 81D965A
	statusanimation BS_EFFECT_BATTLER
	printfromtable gGotPoisonedStringIds
	waitmessage 64
	goto BattleScript_UpdateEffectStatusIconRet

BattleScript_MoveEffectBurn:: @ 81D9669
	statusanimation BS_EFFECT_BATTLER
	printfromtable gGotBurnedStringIds
	waitmessage 64
	goto BattleScript_UpdateEffectStatusIconRet

BattleScript_MoveEffectFreeze:: @ 81D9678
	statusanimation BS_EFFECT_BATTLER
	printfromtable gGotFrozenStringIds
	waitmessage 64
	goto BattleScript_UpdateEffectStatusIconRet

BattleScript_MoveEffectParalysis:: @ 81D9687
	statusanimation BS_EFFECT_BATTLER
	printfromtable gGotParalyzedStringIds
	waitmessage 64
	goto BattleScript_UpdateEffectStatusIconRet

BattleScript_MoveEffectUproar:: @ 81D9696
	printstring STRINGID_PKMNCAUSEDUPROAR
	waitmessage 64
	return

BattleScript_MoveEffectToxic:: @ 81D969D
	statusanimation BS_EFFECT_BATTLER
	printstring STRINGID_PKMNBADLYPOISONED
	waitmessage 64
	goto BattleScript_UpdateEffectStatusIconRet

BattleScript_MoveEffectPayDay:: @ 81D96AA
	printstring STRINGID_COINSSCATTERED
	waitmessage 64
	return

BattleScript_MoveEffectWrap:: @ 81D96B1
	printfromtable gWrappedStringIds
	waitmessage 64
	return

BattleScript_MoveEffectConfusion:: @ 81D96BA
	chosenstatus2animation BS_EFFECT_BATTLER, STATUS2_CONFUSION
	printstring STRINGID_PKMNWASCONFUSED
	waitmessage 64
	return

BattleScript_MoveEffectRecoil:: @ 81D96C8
	jumpifmove MOVE_STRUGGLE, BattleScript_DoRecoil
	jumpifability BS_ATTACKER, ABILITY_ROCK_HEAD, BattleScript_RecoilEnd

BattleScript_DoRecoil: @ 81D96DB
	orword gHitMarker, HITMARKER_IGNORE_SUBSTITUTE | HITMARKER_PASSIVE_HP_UPDATE
	healthbarupdate BS_ATTACKER
	datahpupdate BS_ATTACKER
	printstring STRINGID_PKMNHITWITHRECOIL
	waitmessage 64
	tryfaintmon BS_ATTACKER, FALSE, NULL

BattleScript_RecoilEnd: @ 81D96F5
	return

BattleScript_ItemSteal:: @ 81D96F6
	playanimation BS_TARGET, B_ANIM_ITEM_STEAL, NULL
	printstring STRINGID_PKMNSTOLEITEM
	waitmessage 64
	return

BattleScript_DrizzleActivates:: @ 81D9704
	pause 32
	printstring STRINGID_PKMNMADEITRAIN
	waitstate
	playanimation 7, B_ANIM_RAIN_CONTINUES, NULL
	call BattleScript_WeatherFormChanges
	end3

BattleScript_SpeedBoostActivates:: @ 81D9718
	playanimation BS_ATTACKER, B_ANIM_STATS_CHANGE, sB_ANIM_ARG1
	printstring STRINGID_PKMNRAISEDSPEED
	waitmessage 64
	end3

BattleScript_TraceActivates:: @ 81D9726
	pause 32
	printstring STRINGID_PKMNTRACED
	waitmessage 64
	end3

BattleScript_RainDishActivates:: @ 81D9730
	printstring STRINGID_PKMNSXRESTOREDHPALITTLE2
	waitmessage 64
	orword gHitMarker, HITMARKER_IGNORE_SUBSTITUTE
	healthbarupdate BS_ATTACKER
	datahpupdate BS_ATTACKER
	end3

BattleScript_SandstreamActivates:: @ 81D9744
	pause 32
	printstring STRINGID_PKMNSXWHIPPEDUPSANDSTORM
	waitstate
	playanimation 7, B_ANIM_SANDSTORM_CONTINUES, NULL
	call BattleScript_WeatherFormChanges
	end3

BattleScript_ShedSkinActivates:: @ 81D9758
	printstring STRINGID_PKMNSXCUREDYPROBLEM
	waitmessage 64
	updatestatusicon BS_ATTACKER
	end3

BattleScript_WeatherFormChanges: @ 81D9761
	setbyte sBATTLER, 0

BattleScript_WeatherFormChangesLoop: @ 81D9767
	trycastformdatachange
	addbyte sBATTLER, 1
	jumpifbytenotequal sBATTLER, gBattlersCount, BattleScript_WeatherFormChangesLoop
	return

BattleScript_CastformChange:: @ 81D977D
	call BattleScript_DoCastformChange
	end3

BattleScript_DoCastformChange: @ 81D9783
	docastformchangeanimation
	waitstate
	printstring STRINGID_PKMNTRANSFORMED
	waitmessage 64
	return

BattleScript_IntimidateActivatesEnd3:: @ 81D978C
	call BattleScript_PauseIntimidateActivates
	end3

BattleScript_PauseIntimidateActivates: @ 81D9792
	pause 32

BattleScript_IntimidateActivates:: @ 81D9795
	setbyte gBattlerTarget, 0
	setstatchanger STAT_ATK, 1, TRUE

BattleScript_IntimidateActivatesLoop: @ 81D97A1
	trygetintimidatetarget BattleScript_IntimidateActivatesReturn
	jumpifstatus2 BS_TARGET, STATUS2_SUBSTITUTE, BattleScript_IntimidateActivatesLoopIncrement
	jumpifability BS_TARGET, ABILITY_CLEAR_BODY, BattleScript_IntimidatePrevented
	jumpifability BS_TARGET, ABILITY_HYPER_CUTTER, BattleScript_IntimidatePrevented
	jumpifability BS_TARGET, ABILITY_WHITE_SMOKE, BattleScript_IntimidatePrevented
	statbuffchange 33, BattleScript_IntimidateActivatesLoopIncrement
	jumpifbyte CMP_GREATER_THAN, cMULTISTRING_CHOOSER, 1, BattleScript_IntimidateActivatesLoopIncrement
	setgraphicalstatchangevalues
	playanimation BS_TARGET, B_ANIM_STATS_CHANGE, sB_ANIM_ARG1
	printstring STRINGID_PKMNCUTSATTACKWITH
	waitmessage 64

BattleScript_IntimidateActivatesLoopIncrement: @ 81D97E4
	addbyte gBattlerTarget, 1
	goto BattleScript_IntimidateActivatesLoop

BattleScript_IntimidateActivatesReturn: @ 81D97EF
	return

BattleScript_IntimidatePrevented: @ 81D97F0
	pause 32
	printstring STRINGID_PREVENTEDFROMWORKING
	waitmessage 64
	goto BattleScript_IntimidateActivatesLoopIncrement

BattleScript_DroughtActivates:: @ 81D97FE
	pause 32
	printstring STRINGID_PKMNSXINTENSIFIEDSUN
	waitstate
	playanimation 7, B_ANIM_SUN_CONTINUES, NULL
	call BattleScript_WeatherFormChanges
	end3

BattleScript_TookAttack:: @ 81D9812
	attackstring
	pause 32
	printstring STRINGID_PKMNSXTOOKATTACK
	waitmessage 64
	orword gHitMarker, HITMARKER_ATTACKSTRING_PRINTED
	return

BattleScript_SturdyPreventsOHKO:: @ 81D9826
	pause 32
	printstring STRINGID_PKMNPROTECTEDBY
	pause 64
	goto BattleScript_MoveEnd

BattleScript_DampStopsExplosion:: @ 81D9834
	pause 32
	printstring STRINGID_PKMNPREVENTSUSAGE
	pause 64
	goto BattleScript_MoveEnd

BattleScript_MoveHPDrain_PPLoss:: @ 81D9842
	ppreduce

BattleScript_MoveHPDrain:: @ 81D9843
	attackstring
	pause 32
	orword gHitMarker, HITMARKER_IGNORE_SUBSTITUTE
	healthbarupdate BS_TARGET
	datahpupdate BS_TARGET
	printstring STRINGID_PKMNRESTOREDHPUSING
	waitmessage 64
	orbyte gMoveResultFlags, MOVE_RESULT_DOESNT_AFFECT_FOE
	goto BattleScript_MoveEnd

BattleScript_MonMadeMoveUseless_PPLoss:: @ 81D9865
	ppreduce

BattleScript_MonMadeMoveUseless:: @ 81D9866
	attackstring
	pause 32
	printstring STRINGID_PKMNSXMADEYUSELESS
	waitmessage 64
	orbyte gMoveResultFlags, MOVE_RESULT_DOESNT_AFFECT_FOE
	goto BattleScript_MoveEnd

BattleScript_FlashFireBoost_PPLoss:: @ 81D987B
	ppreduce

BattleScript_FlashFireBoost:: @ 81D987C
	attackstring
	pause 32
	printfromtable gFlashFireStringIds
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_AbilityPreventsPhasingOut: @ 81D988D
	pause 32
	printstring STRINGID_PKMNANCHORSITSELFWITH
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_AbilityNoStatLoss:: @ 81D989B
	pause 32
	printstring STRINGID_PKMNPREVENTSSTATLOSSWITH
	waitmessage 64
	return

BattleScript_BRNPrevention:: @ 81D98A5
	pause 32
	printfromtable gBRNPreventionStringIds
	waitmessage 64
	return

BattleScript_PRLZPrevention:: @ 81D98B1
	pause 32
	printfromtable gPRLZPreventionStringIds
	waitmessage 64
	return

BattleScript_PSNPrevention:: @ 81D98BD
	pause 32
	printfromtable gPSNPreventionStringIds
	waitmessage 64
	return

BattleScript_ObliviousPreventsAttraction:: @ 81D98C9
	pause 32
	printstring STRINGID_PKMNPREVENTSROMANCEWITH
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_FlinchPrevention:: @ 81D98D7
	pause 32
	printstring STRINGID_PKMNSXPREVENTSFLINCHING
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_OwnTempoPrevents: @ 81D98E5
	pause 32
	printstring STRINGID_PKMNPREVENTSCONFUSIONWITH
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_SoundproofProtected:: @ 81D98F3
	attackstring
	ppreduce
	pause 32
	printstring STRINGID_PKMNSXBLOCKSY
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_AbilityNoSpecificStatLoss:: @ 81D9903
	pause 32
	printstring STRINGID_PKMNSXPREVENTSYLOSS
	waitmessage 64
	setbyte cMULTISTRING_CHOOSER, 3
	return

BattleScript_StickyHoldActivates:: @ 81D9913
	pause 32
	printstring STRINGID_PKMNSXMADEYINEFFECTIVE
	waitmessage 64
	goto BattleScript_MoveEnd

BattleScript_ColorChangeActivates:: @ 81D9921
	printstring STRINGID_PKMNCHANGEDTYPEWITH
	waitmessage 64
	return

BattleScript_RoughSkinActivates:: @ 81D9928
	orword gHitMarker, HITMARKER_IGNORE_SUBSTITUTE | HITMARKER_PASSIVE_HP_UPDATE
	healthbarupdate BS_ATTACKER
	datahpupdate BS_ATTACKER
	printstring STRINGID_PKMNHURTSWITH
	waitmessage 64
	tryfaintmon BS_ATTACKER, FALSE, NULL
	return

BattleScript_CuteCharmActivates:: @ 81D9943
	status2animation BS_ATTACKER, STATUS2_INFATUATION
	printstring STRINGID_PKMNSXINFATUATEDY
	waitmessage 64
	return

BattleScript_ApplySecondaryEffect:: @ 81D9950
	waitstate
	seteffectsecondary
	return

BattleScript_SynchronizeActivates:: @ 81D9953
	waitstate
	seteffectprimary
	return

BattleScript_AbilityCuredStatus:: @ 81D9956
	printstring STRINGID_PKMNSXCUREDITSYPROBLEM
	waitmessage 64
	updatestatusicon BS_SCRIPTING
	return

BattleScript_IgnoresWhileAsleep:: @ 81D995F
	printstring STRINGID_PKMNIGNORESASLEEP
	waitmessage 64
	setbyte sMOVEEND_STATE, 0
	moveend 2, 16
	end

BattleScript_IgnoresAndUsesRandomMove:: @ 81D996F
	printstring STRINGID_PKMNIGNOREDORDERS
	waitmessage 64
	jumptocalledmove FALSE

BattleScript_MoveUsedLoafingAround:: @ 81D9977
	printfromtable gInobedientStringIds
	waitmessage 64
	setbyte sMOVEEND_STATE, 0
	moveend 2, 16
	end

BattleScript_IgnoresAndFallsAsleep:: @ 81D9989
	printstring STRINGID_PKMNBEGANTONAP
	waitmessage 64
	setmoveeffect MOVE_EFFECT_SLEEP | MOVE_EFFECT_AFFECTS_USER
	seteffectprimary
	setbyte sMOVEEND_STATE, 0
	moveend 2, 16
	end

BattleScript_IgnoresAndHitsItself:: @ 81D99A0
	printstring STRINGID_PKMNWONTOBEY
	waitmessage 64
	goto BattleScript_DoSelfConfusionDmg

BattleScript_SubstituteFade:: @ 81D99AB
	playanimation BS_TARGET, B_ANIM_SUBSTITUTE_FADE, NULL
	printstring STRINGID_PKMNSUBSTITUTEFADED
	return

BattleScript_BerryCurePrlzEnd2:: @ 81D99B6
	call BattleScript_BerryCureParRet
	end2

BattleScript_BerryCureParRet:: @ 81D99BC
	playanimation 10, B_ANIM_HELD_ITEM_EFFECT, NULL
	printstring STRINGID_PKMNSITEMCUREDPARALYSIS
	waitmessage 64
	updatestatusicon BS_SCRIPTING
	removeitem BS_SCRIPTING
	return

BattleScript_BerryCurePsnEnd2:: @ 81D99CE
	call BattleScript_BerryCurePsnRet
	end2

BattleScript_BerryCurePsnRet:: @ 81D99D4
	playanimation 10, B_ANIM_HELD_ITEM_EFFECT, NULL
	printstring STRINGID_PKMNSITEMCUREDPOISON
	waitmessage 64
	updatestatusicon BS_SCRIPTING
	removeitem BS_SCRIPTING
	return

BattleScript_BerryCureBrnEnd2:: @ 81D99E6
	call BattleScript_BerryCureBrnRet
	end2

BattleScript_BerryCureBrnRet:: @ 81D99EC
	playanimation 10, B_ANIM_HELD_ITEM_EFFECT, NULL
	printstring STRINGID_PKMNSITEMHEALEDBURN
	waitmessage 64
	updatestatusicon BS_SCRIPTING
	removeitem BS_SCRIPTING
	return

BattleScript_BerryCureFrzEnd2:: @ 81D99FE
	call BattleScript_BerryCureFrzRet
	end2

BattleScript_BerryCureFrzRet:: @ 81D9A04
	playanimation 10, B_ANIM_HELD_ITEM_EFFECT, NULL
	printstring STRINGID_PKMNSITEMDEFROSTEDIT
	waitmessage 64
	updatestatusicon BS_SCRIPTING
	removeitem BS_SCRIPTING
	return

BattleScript_BerryCureSlpEnd2:: @ 81D9A16
	call BattleScript_BerryCureSlpRet
	end2

BattleScript_BerryCureSlpRet:: @ 81D9A1C
	playanimation 10, B_ANIM_HELD_ITEM_EFFECT, NULL
	printstring STRINGID_PKMNSITEMWOKEIT
	waitmessage 64
	updatestatusicon BS_SCRIPTING
	removeitem BS_SCRIPTING
	return

BattleScript_BerryCureConfusionEnd2:: @ 81D9A2E
	call BattleScript_BerryCureConfusionRet
	end2

BattleScript_BerryCureConfusionRet:: @ 81D9A34
	playanimation 10, B_ANIM_HELD_ITEM_EFFECT, NULL
	printstring STRINGID_PKMNSITEMSNAPPEDOUT
	waitmessage 64
	removeitem BS_SCRIPTING
	return

BattleScript_BerryCureChosenStatusEnd2:: @ 81D9A44
	call BattleScript_BerryCureChosenStatusRet
	end2

BattleScript_BerryCureChosenStatusRet:: @ 81D9A4A
	playanimation 10, B_ANIM_HELD_ITEM_EFFECT, NULL
	printfromtable gBerryEffectStringIds
	waitmessage 64
	updatestatusicon BS_SCRIPTING
	removeitem BS_SCRIPTING
	return

BattleScript_WhiteHerbEnd2:: @ 81D9A5E
	call BattleScript_WhiteHerbRet
	end2

BattleScript_WhiteHerbRet:: @ 81D9A64
	playanimation 10, B_ANIM_HELD_ITEM_EFFECT, NULL
	printstring STRINGID_PKMNSITEMRESTOREDSTATUS
	waitmessage 64
	removeitem BS_SCRIPTING
	return

BattleScript_ItemHealHP_RemoveItem:: @ 81D9A74
	playanimation BS_ATTACKER, B_ANIM_HELD_ITEM_EFFECT, NULL
	printstring STRINGID_PKMNSITEMRESTOREDHEALTH
	waitmessage 64
	orword gHitMarker, HITMARKER_IGNORE_SUBSTITUTE
	healthbarupdate BS_ATTACKER
	datahpupdate BS_ATTACKER
	removeitem BS_ATTACKER
	end2

BattleScript_BerryPPHealEnd2:: @ 81D9A91
	playanimation BS_ATTACKER, B_ANIM_HELD_ITEM_EFFECT, NULL
	printstring STRINGID_PKMNSITEMRESTOREDPP
	waitmessage 64
	removeitem BS_ATTACKER
	end2

BattleScript_ItemHealHP_End2:: @ 81D9AA1
	call BattleScript_ItemHealHP_Ret
	end2

BattleScript_ItemHealHP_Ret:: @ 81D9AA7
	playanimation BS_ATTACKER, B_ANIM_HELD_ITEM_EFFECT, NULL
	printstring STRINGID_PKMNSITEMRESTOREDHPALITTLE
	waitmessage 64
	orword gHitMarker, HITMARKER_IGNORE_SUBSTITUTE
	healthbarupdate BS_ATTACKER
	datahpupdate BS_ATTACKER
	return

BattleScript_SelectingNotAllowedMoveChoiceItem:: @ 81D9AC2
	printselectionstring STRINGID_ITEMALLOWSONLYYMOVE
	endselectionscript

BattleScript_FocusBandActivates:: @ 81D9AC6
	playanimation BS_TARGET, B_ANIM_FOCUS_BAND, NULL
	printstring STRINGID_PKMNHUNGONWITHX
	waitmessage 64
	return

BattleScript_BerryConfuseHealEnd2:: @ 81D9AD4
	playanimation BS_ATTACKER, B_ANIM_HELD_ITEM_EFFECT, NULL
	printstring STRINGID_PKMNSITEMRESTOREDHEALTH
	waitmessage 64
	orword gHitMarker, HITMARKER_IGNORE_SUBSTITUTE
	healthbarupdate BS_ATTACKER
	datahpupdate BS_ATTACKER
	printstring STRINGID_FORXCOMMAYZ
	waitmessage 64
	setmoveeffect MOVE_EFFECT_CONFUSION | MOVE_EFFECT_AFFECTS_USER
	seteffectprimary
	removeitem BS_ATTACKER
	end2

BattleScript_BerryStatRaiseEnd2:: @ 81D9AFE
	playanimation BS_ATTACKER, B_ANIM_HELD_ITEM_EFFECT, NULL
	statbuffchange MOVE_EFFECT_AFFECTS_USER | 0x1, BattleScript_BerryStatRaiseDoStatUp

BattleScript_BerryStatRaiseDoStatUp: @ 81D9B0B
	setbyte cMULTISTRING_CHOOSER, 0x4
	call BattleScript_StatUp
	removeitem BS_ATTACKER
	end2

BattleScript_BerryFocusEnergyEnd2:: @ 81D9B19
	playanimation BS_ATTACKER, B_ANIM_HELD_ITEM_EFFECT, NULL
	printstring STRINGID_PKMNUSEDXTOGETPUMPED
	waitmessage 64
	removeitem BS_ATTACKER
	end2

BattleScript_ActionSelectionItemsCantBeUsed:: @ 81D9B29
	printselectionstring STRINGID_ITEMSCANTBEUSEDNOW
	endselectionscript

BattleScript_FlushMessageBox:: @ 81D9B2D
	printstring STRINGID_EMPTYSTRING3
	return
