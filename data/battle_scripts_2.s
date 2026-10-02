#include "constants/battle.h"
#include "constants/battle_string_ids.h"
#include "constants/items.h"
#include "constants/songs.h"
#include "constants/game_stat.h"
	.include "include/macros.inc"
	.include "include/macros/battle_script.inc"
	.include "constants/constants.inc"
@ Prevent the C macros from substituting the script aliases below.
#undef MOVE_EFFECT_AFFECTS_USER
#undef MOVE_EFFECT_CERTAIN
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
	incrementgamestat GAME_STAT_POKEMON_CAPTURES

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
	waitmessage B_WAIT_TIME_LONG

BattleScript_GiveCaughtMonEnd: @ 81D9EF8
	givecaughtmon

BattleScript_SuccessBallThrowEnd:
	setbyte gBattleOutcome, B_OUTCOME_CAUGHT
	finishturn

BattleScript_WallyBallThrow:: @ 81D9F00
	printstring STRINGID_GOTCHAPKMNCAUGHTWALLY
	setbyte gBattleOutcome, B_OUTCOME_CAUGHT
	finishturn

BattleScript_ShakeBallThrow:: @ 81D9F0A
	printfromtable gBallEscapeStringIds
	waitmessage B_WAIT_TIME_LONG
	jumpifbyte CMP_NO_COMMON_BITS, gBattleTypeFlags, BATTLE_TYPE_SAFARI, BattleScript_ShakeBallThrowEnd
	jumpifbyte CMP_NOT_EQUAL, gNumSafariBalls, 0, BattleScript_ShakeBallThrowEnd
	printstring STRINGID_OUTOFSAFARIBALLS
	waitmessage B_WAIT_TIME_LONG
	setbyte gBattleOutcome, B_OUTCOME_NO_SAFARI_BALLS

BattleScript_ShakeBallThrowEnd: @ 81D9F34
	finishaction

BattleScript_TrainerBallBlock:: @ 81D9F35
	waitmessage B_WAIT_TIME_LONG
	printstring STRINGID_TRAINERBLOCKEDBALL
	waitmessage B_WAIT_TIME_LONG
	printstring STRINGID_DONTBEATHIEF
	waitmessage B_WAIT_TIME_LONG
	finishaction

BattleScript_PlayerUsesItem: @ 81D9F45
	moveendcase MOVEEND_MIRROR_MOVE
	end

BattleScript_OpponentUsesHealItem: @ 81D9F4F
	pause B_WAIT_TIME_MED
	playse SE_USE_ITEM
	printstring STRINGID_TRAINER1USEDITEM
	waitmessage B_WAIT_TIME_LONG
	useitemonopponent
	orword gHitMarker, HITMARKER_IGNORE_SUBSTITUTE
	healthbarupdate BS_ATTACKER
	datahpupdate BS_ATTACKER
	printstring STRINGID_PKMNSITEMRESTOREDHEALTH
	waitmessage B_WAIT_TIME_LONG
	updatestatusicon BS_ATTACKER
	moveendcase MOVEEND_MIRROR_MOVE
	finishaction

BattleScript_OpponentUsesStatusCureItem: @ 81D9F7B
	pause B_WAIT_TIME_MED
	playse SE_USE_ITEM
	printstring STRINGID_TRAINER1USEDITEM
	waitmessage B_WAIT_TIME_LONG
	useitemonopponent
	printfromtable gTrainerItemCuredStatusStringIds
	waitmessage B_WAIT_TIME_LONG
	updatestatusicon BS_ATTACKER
	moveendcase MOVEEND_MIRROR_MOVE
	finishaction

BattleScript_OpponentUsesXItem: @ 81D9F9C
	pause B_WAIT_TIME_MED
	playse SE_USE_ITEM
	printstring STRINGID_TRAINER1USEDITEM
	waitmessage B_WAIT_TIME_LONG
	useitemonopponent
	printfromtable gStatUpStringIds
	waitmessage B_WAIT_TIME_LONG
	moveendcase MOVEEND_MIRROR_MOVE
	finishaction

BattleScript_OpponentUsesGuardSpec: @ 81D9FBB
	pause B_WAIT_TIME_MED
	playse SE_USE_ITEM
	printstring STRINGID_TRAINER1USEDITEM
	waitmessage B_WAIT_TIME_LONG
	useitemonopponent
	printfromtable gMistUsedStringIds
	waitmessage B_WAIT_TIME_LONG
	moveendcase MOVEEND_MIRROR_MOVE
	finishaction

BattleScript_RunByUsingItem: @ 81D9FDA
	playse SE_FLEE
	setbyte gBattleOutcome, B_OUTCOME_RAN
	finishturn

BattleScript_ActionWatchesCarefully: @ 81D9FE4
	printstring STRINGID_PKMNWATCHINGCAREFULLY
	waitmessage B_WAIT_TIME_LONG
	end2

BattleScript_ActionGetNear: @ 81D9FEB
	printfromtable gSafariGetNearStringIds
	waitmessage B_WAIT_TIME_LONG
	end2

BattleScript_ActionThrowPokeblock: @ 81D9FF4
	printstring STRINGID_THREWPOKEBLOCKATPKMN
	waitmessage B_WAIT_TIME_LONG
	playanimation BS_ATTACKER, B_ANIM_POKEBLOCK_THROW, NULL
	printfromtable gSafariPokeblockResultStringIds
	waitmessage B_WAIT_TIME_LONG
	end2

BattleScript_ActionWallyThrow: @ 81DA00A
	printstring STRINGID_RETURNMON
	waitmessage B_WAIT_TIME_LONG
	returnatktoball
	waitstate
	trainerslidein BS_TARGET
	waitstate
	printstring STRINGID_YOUTHROWABALLNOWRIGHT
	waitmessage B_WAIT_TIME_LONG
	end2
