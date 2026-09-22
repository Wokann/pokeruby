#include "constants/battle.h"
#include "battle_string_ids.h"
#include "constants/items.h"
#include "constants/songs.h"
#include "constants/game_stat.h"
	.include "include/macros.inc"
	.include "include/macros/battle_script.inc"
	.include "constants/constants.inc"
	.include "constants/battle_script_constants.inc"

	.section script_data, "aw", %progbits

	.align 2
gBattlescriptsForBallThrow:: @ 81D9E48
	.4byte BattleScript_BallThrow
	.4byte BattleScript_BallThrow
	.4byte BattleScript_BallThrow
	.4byte BattleScript_BallThrow
	.4byte BattleScript_BallThrow
	.4byte BattleScript_SafariBallThrow
	.4byte BattleScript_BallThrow
	.4byte BattleScript_BallThrow
	.4byte BattleScript_BallThrow
	.4byte BattleScript_BallThrow
	.4byte BattleScript_BallThrow
	.4byte BattleScript_BallThrow
	.4byte BattleScript_BallThrow

gBattlescriptsForUsingItem:: @ 81D9E7C
	.4byte BattleScript_PlayerUsesItem
	.4byte BattleScript_OpponentUsesHealItem
	.4byte BattleScript_OpponentUsesHealItem
	.4byte BattleScript_OpponentUsesStatusCureItem
	.4byte BattleScript_OpponentUsesXItem
	.4byte BattleScript_OpponentUsesGuardSpec

gBattlescriptsForRunningByItem:: @ 81D9E94
	.4byte BattleScript_RunByUsingItem

gBattlescriptsForSafariActions:: @ 81D9E98
	.4byte BattleScript_ActionWatchesCarefully
	.4byte BattleScript_ActionGetNear
	.4byte BattleScript_ActionThrowPokeblock
	.4byte BattleScript_ActionWallyThrow

BattleScript_BallThrow: @ 81D9EA8
	jumpifhalfword CMP_COMMON_BITS, gBattleTypeFlags, BATTLE_TYPE_WALLY_TUTORIAL, BattleScript_BallThrowByWally
	printstring BATTLE_TEXT_PlayerUsedItem
	handleballthrow

BattleScript_BallThrowByWally: @ 81D9EB8
	printstring BATTLE_TEXT_WallyUsedItem
	handleballthrow

BattleScript_SafariBallThrow: @ 81D9EBC
	printstring BATTLE_TEXT_PlayerUsedItem
	updatestatusicon USER
	handleballthrow

BattleScript_SuccessBallThrow:: @ 81D9EC2
	jumpifhalfword CMP_EQUAL, gLastUsedItem, ITEM_SAFARI_BALL, BattleScript_PrintCaughtMonInfo
	incrementgamestat GAME_STAT_POKEMON_CAPTURES

BattleScript_PrintCaughtMonInfo: @ 81D9ED0
	printstring BATTLE_TEXT_GotchaPkmnCaughtPlayer
	trysetcaughtmondexflags BattleScript_TryNicknameCaughtMon
	printstring BATTLE_TEXT_PkmnDataAddedToDex
	waitstate
	setbyte gBattleCommunication, 0
	displaydexinfo

BattleScript_TryNicknameCaughtMon: @ 81D9EE3
	printstring BATTLE_TEXT_GiveNicknameCaptured
	waitstate
	setbyte gBattleCommunication, 0
	trygivecaughtmonnick BattleScript_GiveCaughtMonEnd
	printstring BATTLE_TEXT_PkmnSentToPC
	waitmessage B_WAIT_TIME_LONG

BattleScript_GiveCaughtMonEnd: @ 81D9EF8
	givecaughtmon
	setbyte gBattleOutcome, B_OUTCOME_CAUGHT
	finishturn

BattleScript_WallyBallThrow:: @ 81D9F00
	printstring BATTLE_TEXT_GotchaPkmnCaughtWally
	setbyte gBattleOutcome, B_OUTCOME_CAUGHT
	finishturn

BattleScript_ShakeBallThrow:: @ 81D9F0A
	printfromtable gBallEscapeStringIds
	waitmessage B_WAIT_TIME_LONG
	jumpifbyte CMP_NO_COMMON_BITS, gBattleTypeFlags, BATTLE_TYPE_SAFARI, BattleScript_ShakeBallThrowEnd
	jumpifbyte CMP_NOT_EQUAL, gNumSafariBalls, 0, BattleScript_ShakeBallThrowEnd
	printstring BATTLE_TEXT_OutOfSafariBalls
	waitmessage B_WAIT_TIME_LONG
	setbyte gBattleOutcome, B_OUTCOME_NO_SAFARI_BALLS

BattleScript_ShakeBallThrowEnd: @ 81D9F34
	finishaction

BattleScript_TrainerBallBlock:: @ 81D9F35
	waitmessage B_WAIT_TIME_LONG
	printstring BATTLE_TEXT_TrainerBlockedBall
	waitmessage B_WAIT_TIME_LONG
	printstring BATTLE_TEXT_DontBeAThief
	waitmessage B_WAIT_TIME_LONG
	finishaction

BattleScript_PlayerUsesItem: @ 81D9F45
	setbyte sMOVEEND_STATE, 15
	moveend 1, 0
	end

BattleScript_OpponentUsesHealItem: @ 81D9F4F
	pause B_WAIT_TIME_MED
	playse SE_USE_ITEM
	printstring BATTLE_TEXT_Trainer1UsedItem
	waitmessage B_WAIT_TIME_LONG
	useitemonopponent
	orword gHitMarker, HITMARKER_IGNORE_SUBSTITUTE
	healthbarupdate USER
	datahpupdate USER
	printstring BATTLE_TEXT_PkmnsItemRestoredHealth
	waitmessage B_WAIT_TIME_LONG
	updatestatusicon USER
	setbyte sMOVEEND_STATE, 15
	moveend 1, 0
	finishaction

BattleScript_OpponentUsesStatusCureItem: @ 81D9F7B
	pause B_WAIT_TIME_MED
	playse SE_USE_ITEM
	printstring BATTLE_TEXT_Trainer1UsedItem
	waitmessage B_WAIT_TIME_LONG
	useitemonopponent
	printfromtable gTrainerItemCuredStatusStringIds
	waitmessage B_WAIT_TIME_LONG
	updatestatusicon USER
	setbyte sMOVEEND_STATE, 15
	moveend 1, 0
	finishaction

BattleScript_OpponentUsesXItem: @ 81D9F9C
	pause B_WAIT_TIME_MED
	playse SE_USE_ITEM
	printstring BATTLE_TEXT_Trainer1UsedItem
	waitmessage B_WAIT_TIME_LONG
	useitemonopponent
	printfromtable gStatUpStringIds
	waitmessage B_WAIT_TIME_LONG
	setbyte sMOVEEND_STATE, 15
	moveend 1, 0
	finishaction

BattleScript_OpponentUsesGuardSpec: @ 81D9FBB
	pause B_WAIT_TIME_MED
	playse SE_USE_ITEM
	printstring BATTLE_TEXT_Trainer1UsedItem
	waitmessage B_WAIT_TIME_LONG
	useitemonopponent
	printfromtable gMistUsedStringIds
	waitmessage B_WAIT_TIME_LONG
	setbyte sMOVEEND_STATE, 15
	moveend 1, 0
	finishaction

BattleScript_RunByUsingItem: @ 81D9FDA
	playse SE_FLEE
	setbyte gBattleOutcome, B_OUTCOME_RAN
	finishturn

BattleScript_ActionWatchesCarefully: @ 81D9FE4
	printstring BATTLE_TEXT_PkmnWatchingCarefully
	waitmessage B_WAIT_TIME_LONG
	end2

BattleScript_ActionGetNear: @ 81D9FEB
	printfromtable gSafariGetNearStringIds
	waitmessage B_WAIT_TIME_LONG
	end2

BattleScript_ActionThrowPokeblock: @ 81D9FF4
	printstring BATTLE_TEXT_ThrewPokeblockAtPkmn
	waitmessage B_WAIT_TIME_LONG
	playanimation USER, B_ANIM_POKEBLOCK_THROW, NULL
	printfromtable gSafariPokeblockResultStringIds
	waitmessage B_WAIT_TIME_LONG
	end2

BattleScript_ActionWallyThrow: @ 81DA00A
	printstring STRINGID_RETURNMON
	waitmessage B_WAIT_TIME_LONG
	returnatktoball
	waitstate
	trainerslidein TARGET
	waitstate
	printstring BATTLE_TEXT_YouThrowABallNowRight
	waitmessage B_WAIT_TIME_LONG
	end2
