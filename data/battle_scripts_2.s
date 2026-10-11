#include "constants/battle.h"
#include "constants/battle_script_commands.h"
#include "constants/items.h"
#include "constants/songs.h"
	.include "asm/macros.inc"
	.include "asm/macros/battle_script.inc"
	.include "constants/constants.inc"
	.include "constants/battle_script_constants.inc"

	.section script_data, "aw", %progbits

	.align 2
gBattlescriptsForBallThrow:: @ 81D9E48
	.4byte BattleScript_BallThrow                        @ ITEM_NONE
	.4byte BattleScript_BallThrow                        @ ITEM_MASTER_BALL
	.4byte BattleScript_BallThrow                        @ ITEM_ULTRA_BALL
	.4byte BattleScript_BallThrow                        @ ITEM_GREAT_BALL
	.4byte BattleScript_BallThrow                        @ ITEM_POKE_BALL
	.4byte BattleScript_SafariBallThrow                  @ ITEM_SAFARI_BALL
	.4byte BattleScript_BallThrow                        @ ITEM_NET_BALL
	.4byte BattleScript_BallThrow                        @ ITEM_DIVE_BALL
	.4byte BattleScript_BallThrow                        @ ITEM_NEST_BALL
	.4byte BattleScript_BallThrow                        @ ITEM_REPEAT_BALL
	.4byte BattleScript_BallThrow                        @ ITEM_TIMER_BALL
	.4byte BattleScript_BallThrow                        @ ITEM_LUXURY_BALL
	.4byte BattleScript_BallThrow                        @ ITEM_PREMIER_BALL

gBattlescriptsForUsingItem:: @ 81D9E7C
	.4byte BattleScript_PlayerUsesItem
	.4byte BattleScript_OpponentUsesHealItem             @ AI_ITEM_FULL_RESTORE
	.4byte BattleScript_OpponentUsesHealItem             @ AI_ITEM_HEAL_HP
	.4byte BattleScript_OpponentUsesStatusCureItem       @ AI_ITEM_CURE_CONDITION
	.4byte BattleScript_OpponentUsesXItem                @ AI_ITEM_X_STAT
	.4byte BattleScript_OpponentUsesGuardSpec            @ AI_ITEM_GUARD_SPEC

gBattlescriptsForRunningByItem:: @ 81D9E94
	.4byte BattleScript_RunByUsingItem

gBattlescriptsForSafariActions:: @ 81D9E98
	.4byte BattleScript_ActionWatchesCarefully
	.4byte BattleScript_ActionGetNear
	.4byte BattleScript_ActionThrowPokeblock
	.4byte BattleScript_ActionWallyThrow

BattleScript_BallThrow: @ 81D9EA8
	jumpifhalfword CMP_COMMON_BITS, gBattleTypeFlags, BATTLE_TYPE_WALLY_TUTORIAL, BattleScript_BallThrowByWally
	printstring STRINGID_PLAYERUSEDITEM
	handleballthrow

BattleScript_BallThrowByWally: @ 81D9EB8
	printstring STRINGID_WALLYUSEDITEM
	handleballthrow

BattleScript_SafariBallThrow: @ 81D9EBC
	printstring STRINGID_PLAYERUSEDITEM
	updatestatusicon BS_ATTACKER
	handleballthrow

BattleScript_SuccessBallThrow:: @ 81D9EC2
	jumpifhalfword CMP_EQUAL, gLastUsedItem, ITEM_SAFARI_BALL, BattleScript_PrintCaughtMonInfo
	incrementgamestat 11

BattleScript_PrintCaughtMonInfo: @ 81D9ED0
	printstring STRINGID_GOTCHAPKMNCAUGHTPLAYER
	trysetcaughtmondexflags BattleScript_TryNicknameCaughtMon
	printstring STRINGID_PKMNDATAADDEDTODEX
	waitstate
	setbyte gBattleCommunication, 0
	displaydexinfo

BattleScript_TryNicknameCaughtMon: @ 81D9EE3
	printstring STRINGID_GIVENICKNAMECAPTURED
	waitstate
	setbyte gBattleCommunication, 0
	trygivecaughtmonnick BattleScript_GiveCaughtMonEnd
	printstring STRINGID_PKMNSENTTOPC
	waitmessage 64

BattleScript_GiveCaughtMonEnd: @ 81D9EF8
	givecaughtmon
	setbyte gBattleOutcome, B_OUTCOME_CAUGHT
	finishturn

BattleScript_WallyBallThrow:: @ 81D9F00
	printstring STRINGID_GOTCHAPKMNCAUGHTWALLY
	setbyte gBattleOutcome, B_OUTCOME_CAUGHT
	finishturn

BattleScript_ShakeBallThrow:: @ 81D9F0A
	printfromtable gBallEscapeStringIds
	waitmessage 64
	jumpifbyte CMP_NO_COMMON_BITS, gBattleTypeFlags, BATTLE_TYPE_SAFARI, BattleScript_ShakeBallThrowEnd
	jumpifbyte CMP_NOT_EQUAL, gNumSafariBalls, 0, BattleScript_ShakeBallThrowEnd
	printstring STRINGID_OUTOFSAFARIBALLS
	waitmessage 64
	setbyte gBattleOutcome, B_OUTCOME_NO_SAFARI_BALLS

BattleScript_ShakeBallThrowEnd: @ 81D9F34
	finishaction

BattleScript_TrainerBallBlock:: @ 81D9F35
	waitmessage 64
	printstring STRINGID_TRAINERBLOCKEDBALL
	waitmessage 64
	printstring STRINGID_DONTBEATHIEF
	waitmessage 64
	finishaction

BattleScript_PlayerUsesItem: @ 81D9F45
	setbyte sMOVEEND_STATE, 15
	moveend 1, 0
	end

BattleScript_OpponentUsesHealItem: @ 81D9F4F
	pause 48
	playse SE_USE_ITEM
	printstring STRINGID_TRAINER1USEDITEM
	waitmessage 64
	useitemonopponent
	orword gHitMarker, HITMARKER_IGNORE_SUBSTITUTE
	healthbarupdate BS_ATTACKER
	datahpupdate BS_ATTACKER
	printstring STRINGID_PKMNSITEMRESTOREDHEALTH
	waitmessage 64
	updatestatusicon BS_ATTACKER
	setbyte sMOVEEND_STATE, 15
	moveend 1, 0
	finishaction

BattleScript_OpponentUsesStatusCureItem: @ 81D9F7B
	pause 48
	playse SE_USE_ITEM
	printstring STRINGID_TRAINER1USEDITEM
	waitmessage 64
	useitemonopponent
	printfromtable gTrainerItemCuredStatusStringIds
	waitmessage 64
	updatestatusicon BS_ATTACKER
	setbyte sMOVEEND_STATE, 15
	moveend 1, 0
	finishaction

BattleScript_OpponentUsesXItem: @ 81D9F9C
	pause 48
	playse SE_USE_ITEM
	printstring STRINGID_TRAINER1USEDITEM
	waitmessage 64
	useitemonopponent
	printfromtable gStatUpStringIds
	waitmessage 64
	setbyte sMOVEEND_STATE, 15
	moveend 1, 0
	finishaction

BattleScript_OpponentUsesGuardSpec: @ 81D9FBB
	pause 48
	playse SE_USE_ITEM
	printstring STRINGID_TRAINER1USEDITEM
	waitmessage 64
	useitemonopponent
	printfromtable gMistUsedStringIds
	waitmessage 64
	setbyte sMOVEEND_STATE, 15
	moveend 1, 0
	finishaction

BattleScript_RunByUsingItem: @ 81D9FDA
	playse SE_FLEE
	setbyte gBattleOutcome, B_OUTCOME_RAN
	finishturn

BattleScript_ActionWatchesCarefully: @ 81D9FE4
	printstring STRINGID_PKMNWATCHINGCAREFULLY
	waitmessage 64
	end2

BattleScript_ActionGetNear: @ 81D9FEB
	printfromtable gSafariGetNearStringIds
	waitmessage 64
	end2

BattleScript_ActionThrowPokeblock: @ 81D9FF4
	printstring STRINGID_THREWPOKEBLOCKATPKMN
	waitmessage 64
	playanimation BS_ATTACKER, B_ANIM_POKEBLOCK_THROW, 0x0
	printfromtable gSafariPokeblockResultStringIds
	waitmessage 64
	end2

BattleScript_ActionWallyThrow: @ 81DA00A
	printstring 2
	waitmessage 64
	returnatktoball
	waitstate
	trainerslidein BS_TARGET
	waitstate
	printstring STRINGID_YOUTHROWABALLNOWRIGHT
	waitmessage 64
	end2
